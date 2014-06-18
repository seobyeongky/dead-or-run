#include "starting_scene.h"
#include "res.h"
#include "util.h"
#include "sv_module\sv_module.h"
#include <curl/curl.h>
#include <curl/easy.h>
#include "string_converter.h"
#include "json.h"


size_t writer(char *ptr, size_t size, size_t nmemb, string * paper){
	if (!paper) return 0;

	paper->append(ptr, size*nmemb);
	
	return size*nmemb;
}

void StartingScene::AddOnlineServer()
{
	char proxy_url[256];
	GetPrivateProfileStringA("SERVER", "PROXY_URL", "http://seobyeongky.herokuapp.com/dead-or-run", proxy_url, 255, "./game.ini");

	CURL * curl = curl_easy_init();
	assert(curl && "Failed to initiate curl");
	curl_easy_setopt(curl, CURLOPT_URL, proxy_url);
    curl_easy_setopt(curl, CURLOPT_FOLLOWLOCATION, 1L);
	
	string raw;
	curl_easy_setopt(curl, CURLOPT_WRITEDATA, &raw);
	curl_easy_setopt(curl, CURLOPT_WRITEFUNCTION, writer);

/*	curl_easy_setopt(curl, CURLOPT_WRITEFUNCTION, &[&rawdata](char *ptr, size_t size, size_t nmemb, void *userdata) -> size_t {
		rawdata.append(ptr, size*nmemb);
		return size*nmemb;
	});
*/	CURLcode res = curl_easy_perform(curl);
	if (res != CURLE_OK)
	{
		ErrorMsg(L"curl_easy_perform 실패 : %hs", curl_easy_strerror(res));
	}
	curl_easy_cleanup(curl);

	vector<char> buf;
	for(auto c : raw) buf.push_back(c);
	buf.push_back('\0');
	char *errorPos = 0;
	char *errorDesc = 0;
	int errorLine = 0;
	block_allocator allocator(1 << 10); // 1 KB per block
        
	json_value *root = json_parse(&buf[0], &errorPos, &errorDesc, &errorLine, &allocator);
	
	if (!root)
	{
		ErrorMsg(L"Json parsing 실패 line : %d, desc: %hs\n", errorLine, errorDesc);
	}

	for (json_value *item = root->first_child; item; item = item->next_sibling)
	{
		server_info_t sv;
		sv.max_num_of_clients = (size_t)-1;
		sv.num_of_clients = (size_t)-1;
		string aaddr(item->name);
		multi2uni(aaddr, &sv.address);
		for (json_value * field = item->first_child; field; field = field->next_sibling)
		{
			if (strcmp(field->name, "name") == 0) {
				string aname(field->string_value);
				multi2uni(aname, &sv.name);
			} else if (strcmp(field->name, "nr_max_clients") == 0) {
				sv.max_num_of_clients = static_cast<size_t>(field->int_value);
			}
		}
		AddRemoteServer(sv);
	}
}

StartingScene::StartingScene(scene_id_t scene_id) : 
	Scene(scene_id),
	_bg(*sprite_map[L"bg"]),
	_name_query_text(String(L"당신의 이름을 입력하세요"), normal_font, 20U),
	_name_text(normal_font, 20U),
	_mode_query_text(String(L"서버 접속하기(0) 또는 서버 만들기(1)를 입력하세요"), normal_font, 20U),
	_mode_text(normal_font, 20U),
	_sv_name_query_text(String(L"서버의 이름을 입력해주세요"), normal_font, 20U),
	_sv_name_text(normal_font, 20U),
	_sv_list_text(String(L""), normal_font, 20U),
	_sv_index_cnt(0),
	_sv_addr_map(),
	_sv_index_text(normal_font, 20U),
	_text_bg(),
	_help_text(String(L"[도움말] 엔터키 : 입력 완료, ESC키 : 입력 취소"), normal_font, 20U),
	_help_bg(),
	_state(GET_NAME)
{
	FitScaleToScreen(&_bg);

	_name_query_text.setColor(Color(0, 0, 0));
	_name_query_text.setStyle(Text::Italic);

	_name_text.setTextColor(Color(0, 100, 0));
	_mode_query_text.setColor(Color(0, 0, 0));
	_mode_query_text.setStyle(Text::Italic);

	_mode_text.setTextColor(Color(0, 100, 0));

	_sv_name_query_text.setColor(Color(0, 0, 0));
	_sv_name_query_text.setStyle(Text::Italic);
	_sv_list_text.setColor(Color(0, 0, 0));

	_sv_name_text.setTextColor(Color(0, 100, 0));

	_sv_index_text.setTextColor(Color(0, 100, 0));
	
	_text_bg.setFillColor(Color(255, 255, 255, 127));
	_text_bg.setPosition(0.f ,0.f);

	{
		_help_text.setColor(Color(0, 0, 0));
		_help_text.setStyle(Text::Italic);
		float x_space = 100.f, height = 40.f;
		_help_bg.setFillColor(Color(255, 255, 255));
		_help_bg.setSize(Vector2f(window.getSize().x - 2 * x_space, height));
		_help_bg.setPosition(x_space, window.getSize().y - height);
	}

	ChangeState(GET_NAME);
}

StartingScene::~StartingScene()
{
}

void StartingScene::Init()
{
}

bool StartingScene::HandleTextEntered(wchar_t key)
{
	if(key == UNICODE_ESC)
	{
		if(_state == GET_MODE)				ChangeState(GET_NAME);
		else if(_state == GET_SERVER_NAME)	ChangeState(GET_MODE);
		else if(_state == SERVER_SEARCHING) ChangeState(GET_MODE);
		else if(_state == GET_SERVER_INDEX) ChangeState(GET_MODE);
		else return false;
		return true;
	}
	else
	{
		bool result;
		if(_state == GET_NAME)				result = _name_text.handleEnteredKey(key);
		else if(_state == GET_MODE)			result = _mode_text.handleEnteredKey(key);
		else if(_state == GET_SERVER_NAME)	result =  _sv_name_text.handleEnteredKey(key);
		else if(_state == SERVER_SEARCHING) result = false;
		else if(_state == GET_SERVER_INDEX)	result = _sv_index_text.handleEnteredKey(key);
		else return false;
		if(result) UpdateGUI();
		return result;
	}
	return false;
}

bool StartingScene::HandleKeyPressed(Event::KeyEvent key)
{
	switch(_state)
	{
	case GET_NAME:
		if(key.code == Keyboard::Return)
		{
			if(_name_text.getString().isEmpty()) ErrorMsg(L"이름을 입력하고 엔터를 누르세요");
			else { ChangeState(GET_MODE); sfx_mgr->Play(SFX_ENTER); }
			return true;
		}
		else { bool result = _name_text.handleKeyPress(key); UpdateGUI(); return result; }
		
	case GET_MODE:
		if(key.code == Keyboard::Return)
		{
			const String &str = _mode_text.getString();
			if(str == L"0")
			{
				_sv_list_text.setString(L"------ 찾은 서버 목록 ------\n");
				_sv_addr_map.Clear();
				_sv_index_cnt = 0;
				RequestSearchLocalNetServers();
				AddOnlineServer();
				ChangeState(SERVER_SEARCHING);
				sfx_mgr->Play(SFX_ENTER);
			}
			else if(str == L"1")
			{
				ChangeState(GET_SERVER_NAME);
				sfx_mgr->Play(SFX_ENTER);
			}
			else
			{
				ErrorMsg(L"0과 1 중 하나를 입력하세요");
				return true;
			}
			return true;
		}
		else { bool result = _mode_text.handleKeyPress(key); UpdateGUI(); return result; }

	case GET_SERVER_NAME:
		if(key.code == Keyboard::Return)
		{
			if(_sv_name_text.getString().isEmpty())
			{
				ErrorMsg(L"서버 이름을 입력해주세요.");
				return true;
			}
			else
			{
				sfx_mgr->Play(SFX_ENTER);
				return BeginSvModule();
			}
		}
		else { bool result = _sv_name_text.handleKeyPress(key); UpdateGUI(); return result; }

	case SERVER_SEARCHING:
		FindingOver();
		return true;

	case GET_SERVER_INDEX:
		if(key.code == Keyboard::Return)
		{
			const String &str = _sv_index_text.getString();
			if(str.isEmpty()) { ErrorMsg(L"찾은 서버의 번호 또는 IP주소를 입력해주세요."); return true; }
			if(IsDigits(str.toAnsiString()))
			{
				int index = stoi(str.toAnsiString());
				SMap<int, wstring>::Iter iter;
				if(!_sv_addr_map.Find(index, &iter)) { ErrorMsg(L"%d번 서버는 없습니다.", index); return false; }
				RequestConnectToServer((*iter).GetEl().c_str(), _name_text.getString().toWideString().c_str());			
			}
			else if(IsValidAddr(str.toWideString()))
			{
				RequestConnectToServer(str.toWideString().c_str(), _name_text.getString().toWideString().c_str());
			}
			else { ErrorMsg(L"이해하지 못했습니다. (찾은 서버의 번호 또는 IP주소를 입력해주세요.)"); return true; }
	
			ChangeState(WAITING_FOR_REMOTE_SERVER);
			sfx_mgr->Play(SFX_ENTER);

			return true;
		}
		else return _sv_index_text.handleKeyPress(key);
	}
	return false;
}

bool StartingScene::HandleIMECompMsg(wchar_t key, bool done)
{
	bool result;
	switch(_state)
	{
	case GET_NAME: result = _name_text.handleIMEUpdated(key, done); break;
	case GET_MODE: result = _mode_text.handleIMEUpdated(key, done); break;
	case GET_SERVER_INDEX: result = _sv_index_text.handleIMEUpdated(key, done); break;
	case GET_SERVER_NAME: result = _sv_name_text.handleIMEUpdated(key, done); break;
	default: result = false; break;
	}

	if (result) UpdateGUI();

	return result;
}

bool StartingScene::HandleClientMsg(client_msg_t &msg)
{
	if(msg.type == CLMSG_UNEXPECTED_ERROR) { ErrorMsg(L"OPZNET client service : 예상치 못한 오류"); return true; }
	
	switch(_state)
	{
	case SERVER_SEARCHING:
		if(msg.type == CLMSG_LOCAL_SERVER_FOUND) { AddRemoteServer(msg.server_info); return true; }
		else if(msg.type == CLMSG_FINDING_LOCAL_SERVER_OVER) { FindingOver(); return true; }
		else return false;

	case WAITING_FOR_LOCAL_SERVER:
		if(msg.type == CLMSG_CONNECTED)
		{
			Packet send_packet;
			send_packet << STPS_HOSTING_MODE;
			send_packet << _sv_name_text.getString().toWideString();
			send_packet << msg.my_id;
			send_packet << _name_text.getString().toWideString();
			SavePacket(send_packet);

			ChangeState(GET_SERVER_NAME);	// 돌아올 떄를 대비해서
			UpdateGUI();
			SetNextScene(PLAY_SCENE);

			return true;
		}
		else if(msg.type == CLMSG_CONNECT_FAILED)
		{
			ErrorMsg(L"연결 실패 : %ls", msg.error_msg);
			ChangeState(GET_SERVER_NAME);
			UpdateGUI();
			return true;
		}
		return false;
		
	case WAITING_FOR_REMOTE_SERVER:
		if(msg.type == CLMSG_CONNECTED)
		{
			Packet send_packet;
			send_packet << STPS_JOINING_MODE;
			send_packet << msg.my_id;
			send_packet << _name_text.getString().toWideString();
			SavePacket(send_packet);

			ChangeState(GET_SERVER_INDEX);	// 돌아올 떄를 대비해서
			UpdateGUI();
			SetNextScene(PLAY_SCENE);

			return true;
		}
		else if(msg.type == CLMSG_CONNECT_FAILED)
		{
			ErrorMsg(L"연결 실패 : %ls", msg.error_msg);
			ChangeState(GET_SERVER_INDEX);
			UpdateGUI();
			return true;
		}
		return false;
	}

	return false;
}

#define DRAW(x) window.draw(x)

void StartingScene::Go()
{
	DRAW(_bg);
	DRAW(_help_bg);
	DRAW(_text_bg);
	DRAW(_help_text);
	
	// switching에 따라서 GUI를 그릴지 말지를 결정합니다.
	if(_name_switch) {DRAW(_name_query_text); DRAW(_name_text); }
	if(_mode_switch) {DRAW(_mode_query_text); DRAW(_mode_text); }
	if(_sv_list_switch) { DRAW(_sv_list_text); }
	if(_sv_index_switch) { DRAW(_sv_index_text); }
	if(_sv_name_switch) { DRAW(_sv_name_query_text); DRAW(_sv_name_text); }
}

void StartingScene::UpdateGUI()
{
	float x_max = 0.f;
	float y_offset = 0.f;

#define UPDATE_TEXT_POS(x)	x.setPosition(0.f, y_offset)
	
#define UPDATE_COORD(x)	{y_offset += x.getLocalBounds().height; \
						if(x_max < x.getLocalBounds().width) x_max = x.getLocalBounds().width;}

	if(_name_switch)
	{
		UPDATE_TEXT_POS(_name_query_text);		UPDATE_COORD(_name_query_text);
		UPDATE_TEXT_POS(_name_text);		UPDATE_COORD(_name_text);
	}

	if(_mode_switch)
	{
		UPDATE_TEXT_POS(_mode_query_text);		UPDATE_COORD(_mode_query_text);
		UPDATE_TEXT_POS(_mode_text);		UPDATE_COORD(_mode_text);
	}

	if(_sv_list_switch)
	{
		UPDATE_TEXT_POS(_sv_list_text);			UPDATE_COORD(_sv_list_text);
	}

	if(_sv_index_switch)
	{
		UPDATE_TEXT_POS(_sv_index_text);	UPDATE_COORD(_sv_index_text);
	}

	if(_sv_name_switch)
	{
		UPDATE_TEXT_POS(_sv_name_query_text);	UPDATE_COORD(_sv_name_query_text);

		UPDATE_TEXT_POS(_sv_name_text);	UPDATE_COORD(_sv_name_text);
	}

	_text_bg.setSize(Vector2f(x_max + 5.f, y_offset + 10.f));

	_help_text.setPosition(	window.getSize().x / 2  - _help_text.getLocalBounds().width / 2,
							static_cast<float>(window.getSize().y) - _help_text.getLocalBounds().height - 10.f );
}

void StartingScene::FindingOver()
{
	String str = _sv_list_text.getString();
	str += L"서버 찾기 종료!";
	_sv_list_text.setString(str);
	ChangeState(GET_SERVER_INDEX);
	UpdateGUI();
}

void StartingScene::AddRemoteServer(const server_info_t & sv_info)
{
	_sv_addr_map.Insert(_sv_index_cnt, sv_info.address);

	String str = _sv_list_text.getString();
	wstring nr;
	if (sv_info.num_of_clients == (size_t)-1)
		nr = L"?";
	else
		nr = to_wstring(sv_info.num_of_clients);
	wstring nr_max;
	if (sv_info.max_num_of_clients == (size_t)-1)
		nr_max = L"?";
	else
		nr_max = to_wstring(sv_info.max_num_of_clients);

	str += to_wstring(_sv_index_cnt)
		+ L"번 서버 : "
		+  sv_info.address
		+ L" [" + sv_info.name + L"]  ("
		+ nr + L"/"
		+ nr_max
		+ L"명 접속)\n";
	
	_sv_list_text.setString(str);
	UpdateGUI();

	++_sv_index_cnt;
}

bool StartingScene::BeginSvModule()
{
	SvModule::S_Create();
	if(!SvModule::S_Begin(_sv_name_text.getString().toWideString()))
	{
		SvModule::S_Delete();
		return false;
	}

	RequestConnectToServer(L"localhost", _name_text.getString().toWideString().c_str());

	ChangeState(WAITING_FOR_LOCAL_SERVER);

	return true;
}

void StartingScene::ChangeState(state_t new_state)
{
	_state = new_state;

#define TURN_ON(x) x = true
#define TURN_OFF(x) x = false

	_name_text.drawCaret(false);	_mode_text.drawCaret(false);
	_sv_index_text.drawCaret(false);	_sv_name_text.drawCaret(false);

	switch(_state)
	{
	case GET_NAME:
		_name_text.drawCaret(true);
		TURN_ON(_name_switch);	TURN_OFF(_mode_switch);	TURN_OFF(_sv_list_switch);	TURN_OFF(_sv_index_switch);	TURN_OFF(_sv_name_switch);
		break;
	case GET_MODE:
		_mode_text.drawCaret(true);
		TURN_ON(_name_switch);	TURN_ON(_mode_switch);	TURN_OFF(_sv_list_switch);	TURN_OFF(_sv_index_switch);	TURN_OFF(_sv_name_switch);
		break;
	case SERVER_SEARCHING:
		TURN_ON(_name_switch);	TURN_ON(_mode_switch);	TURN_ON(_sv_list_switch);	TURN_OFF(_sv_index_switch);	TURN_OFF(_sv_name_switch);
		break;
	case GET_SERVER_INDEX:
		_sv_index_text.drawCaret(true);
	case WAITING_FOR_REMOTE_SERVER:
		TURN_ON(_name_switch);	TURN_ON(_mode_switch);	TURN_ON(_sv_list_switch);	TURN_ON(_sv_index_switch);	TURN_OFF(_sv_name_switch);
		break;
	case GET_SERVER_NAME:
		_sv_name_text.drawCaret(true);
	case WAITING_FOR_LOCAL_SERVER:
		TURN_ON(_name_switch);	TURN_ON(_mode_switch);	TURN_OFF(_sv_list_switch);	TURN_OFF(_sv_index_switch);	TURN_ON(_sv_name_switch);
		break;
	}

	UpdateGUI();
}