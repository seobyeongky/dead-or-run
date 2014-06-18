#include "sv_thread.h"
#include "../util.h"
#include "sv_wait_scene.h"
#include "sv_play_scene.h"
#include "poster.h"
#include "../string_converter.h"
#include "../util.h"
#include "myip.h"
#include "port_forwarder.h"

int	sv_present_time;
int sv_delta_time;

#pragma warning(disable: 4351)	// _scene_table 기본 초기화 관련 경고 끄기

SvThread::SvThread() : opzThread(),
	_bresult_set(false), _bresult(SUCCEEDED),
	_scene_table(),
	_cur_scene(nullptr),
	_clock(),
	_exit(false)
{
	timeBeginPeriod(1U);
}

#pragma warning(default: 4351)

SvThread::~SvThread()
{
	for(unsigned int i = 0U; i < SV_NR_SCENE; ++i)
		if(_scene_table[i]) delete _scene_table[i];

	timeEndPeriod(1U);
}

void SvThread::EndProcess() { EndServer(); }

bool SvThread::GetBeginningResult(bresult_t * result_ptr)
{
	if(_bresult_set) { *result_ptr = _bresult; return true;}
	else return false;
}

void SvThread::SetName(const wstring & name)
{
	sv_context.name = name;
}

//서버 컨텍스트
sv_context_t sv_context;

void SafeSendToAll(const Packet & packet)
{
	for(auto it = sv_context.clients.Begin();
		it != sv_context.clients.End();
		++it)
	{
		SafeSend((*it).GetKey(), packet);
	}
}

void UnsafeSendToAll(const Packet & packet)
{
	for(auto it = sv_context.clients.Begin();
		it != sv_context.clients.End();
		++it)
	{
		UnsafeSend((*it).GetKey(), packet);
	}
}

Poster * MakePoster()
{
	char proxy_url[256];
	GetPrivateProfileStringA("SERVER", "PROXY_URL", "http://seobyeongky.herokuapp.com/dead-or-run", proxy_url, 255, "./game.ini");
	int nr_max_clients = GetPrivateProfileInt(L"SERVER", L"MAX_NUM_OF_CLIENTS", 16, L"game.ini");
	string json;
	json += "{\"name\":\"";
	string sv_context_name;
	uni2multi(sv_context.name, &sv_context_name);
	json += sv_context_name;
	json += "\",\"nr_max_clients\":";
	json += to_string(nr_max_clients);
	json += "}";
	LeaveLog(L"Poster json : %hs", json.c_str());
	return new Poster(proxy_url, json);
}

void SvThread::Run()
{
	while( !BeginServer(sv_context.name))
	{
		Sleep(300);
	}

	IPv4 addr;
	if (!getMyIP(addr))
		ErrorMsg(L"포트 포워딩 실패 : locap ip 가져오기 실패");
	else
	{
		wstring iptext;
		iptext = to_wstring(addr.b1) + L"." + to_wstring(addr.b2) + L"." + to_wstring(addr.b3) + L"." + to_wstring(addr.b4);
		WCHAR iptextbuf[32];
		wcscpy_s(iptextbuf, iptext.c_str());
		int primary_port = GetPrivateProfileInt(L"GENERAL", L"PRIMARY_PORT", 38599, L"./game.ini");
		int ret = SetPortForwarding(iptextbuf, L"dead-or-run-primary", primary_port, primary_port);
		if (ret != 0)
		{
			ErrorMsg(L"primary 포트 포워딩 실패 : %d", ret);
		}
		int secondary_port = GetPrivateProfileInt(L"GENERAL", L"SECONDARY_PORT", 38600, L"./game.ini");
		ret = SetPortForwarding(iptextbuf, L"dead-or-run-secondary", secondary_port, secondary_port);
		if (ret != 0)
		{
			ErrorMsg(L"secondary 포트 포워딩 실패 : %d", ret);
		}
	}

	Poster * poster = MakePoster();

	_bresult_set = true;
	_bresult = SUCCEEDED;

#ifndef _DEBUG
	srand(static_cast<unsigned int>(time(nullptr)));
#endif
	CreateSceneTable();

	server_msg_t sv_msg;

	_cur_scene = _scene_table[SV_WAITING_SCENE];
	_cur_scene->OnInit();

	int last_time = _clock.getElapsedTime().asMilliseconds();
	int nr_tick = 0;

	while(1)
	{
		sv_present_time = _clock.getElapsedTime().asMilliseconds();
		sv_delta_time = sv_present_time - last_time;

		while(GetServerMsg(&sv_msg))
		{
			switch(sv_msg.type)
			{
			case SVMSG_PACKET_RECV:
				{
					header_t header_in_packet;
					if(!(sv_msg.packet >> header_in_packet)) return;
					cl_to_sv_t header(static_cast<cl_to_sv_t>(header_in_packet));
					
					if(header == CL_TO_SV_CHAT)
						HandleChatRequest(sv_msg.client_info, sv_msg.packet);
					else
						_cur_scene->HandleRecvPacket(
							sv_msg.client_info,
							header,
							sv_msg.packet );
				}
				break;

			case SVMSG_NEW_CLIENT:
				{
					_cur_scene->HandleNewCl(sv_msg.client_info);
					cl_context_t context;
					context.info = sv_msg.client_info;
					context.delay_time = 0;
					context.nr_accum_win = 0U;
					sv_context.clients.Insert(sv_msg.client_info.id, context);
				}
				break;

			case SVMSG_CLIENT_GONE:
				{
					SMap<ID, cl_context_t>::Iter iter;
					if(!sv_context.clients.Find(sv_msg.client_info.id, &iter))
					{
						LeaveLog(L"SVMSG_CLIENT_GONE : 해당 클라이언트(%u, %s) 못찾음",
									sv_msg.client_info.id, sv_msg.client_info.name.c_str());
						break;
					}
					_cur_scene->HandleGoneCl(sv_msg.client_info);
					sv_context.clients.Erase(iter);
				}
				break;

			case SVMSG_BIND_FAILED:
				ErrorMsg(L"server thread : bind 실패");
				Suspend(); break;
			
			case SVMSG_CLIENT_DELAY_TIME_UPDATED:
				{
					SMap<ID, cl_context_t>::Iter iter;
					if(!sv_context.clients.Find(sv_msg.client_info.id, &iter))
					{
						LeaveLog(L"SVMSG_CLIENT_DELAY_TIME_UPDATED : 해당 클라이언트(%u, %s) 못찾음",
							sv_msg.client_info.id, sv_msg.client_info.name.c_str());
						break;
					}
					(*iter).GetEl().delay_time = sv_msg.delay_time;
				}
				break;

			default:
				ErrorMsg(L"server thread : sv_msg의 type(%d)이 오염됐습니다.", sv_msg.type); Suspend();
			}
			if(_cur_scene->GetNextScene() != SV_INVALID_SCENE) goto CHANGE;
		}
		_cur_scene->Go();

CHANGE:
		if(_cur_scene->GetNextScene() != SV_INVALID_SCENE)
		{
			_cur_scene = _scene_table[_cur_scene->GetNextScene()];
			_cur_scene->OnInit();
		}
		else
		{
			int cur_time = _clock.getElapsedTime().asMilliseconds();
			if(cur_time - sv_present_time < SV_MSPF)
				Sleep(SV_MSPF - (cur_time - sv_present_time));
		}

		if (nr_tick % 500 == 0)
		{
			poster->Post();
		}


		last_time = sv_present_time;

		if(_exit) break;

		nr_tick++;
	}

	delete poster;
}

void SvThread::CreateSceneTable()
{
	_scene_table[SV_WAITING_SCENE] = new SvWaitScene(SV_WAITING_SCENE);
	_scene_table[SV_PLAYING_SCENE] = new SvPlayScene(SV_PLAYING_SCENE);
}

void SvThread::SendInfo(ID cl_id, const wstring & info)
{
	Packet send_packet;
	send_packet << UNSIGNED_SHORT(SV_TO_CL_INFO)
				<< info;
	SafeSend(cl_id, send_packet);
}

const wchar_t * HELP_TEXT =
	L"--- 명령어 목록 ---\n"
	L"/help : 도움말\n"
	L"/show players : 플레이어들의 정보 보기\n"
	L"/kick [ID] : 플레이어 추방시키기(방장만 가능)";

void SvThread::HandleChatRequest(const client_t & cl, Packet & recv_packet)
{
	wstring msg;
	if(!(recv_packet >> msg)) return;

	if(msg.empty()) return;

	if(msg[0] == L'/')
	{
		// 명령어
		if(msg.substr(1, 4) == L"help")
		{
			SendInfo(cl.id, HELP_TEXT);
		}
		else if(msg.substr(1, 12) == L"show players")
		{
			wstring send_msg(L"--- 플레이어들 정보 ---");
			for(auto iter = sv_context.clients.Begin();
				iter != sv_context.clients.End();
				++iter)
			{
				cl_context_t & cl_context = (*iter).GetEl();
				send_msg += L"\n" +
							to_wstring(cl_context.info.id) +
							L" : " +
							cl_context.info.name +
							L" : 지연시간(" +
							to_wstring(cl_context.delay_time) +
							L")";
				if(cl_context.nr_accum_win > 0)
					send_msg += L" - 현재 " +
								to_wstring(cl_context.nr_accum_win) +
								L"연승 중";
			}
			SendInfo(cl.id, send_msg);
		}
		else if(msg.substr(1, 4) == L"kick")
		{
			if(cl.id == 0)
			{
				// host
				int kicked_cl_id = stoi(msg.substr(6, msg.length() - 1));

				if(kicked_cl_id == 0)
					SendInfo(cl.id, L"자기 자신을 추방시킬 수는 없습니다.");
				else
					DisconnectClient(static_cast<ID>(kicked_cl_id));
			}
			else
			{
				// cl
				SendInfo(cl.id, L"추방 기능은 방장만 가능합니다.");
			}
		}
	}
	else
	{
		// 일반적인 채팅 메시지
		Packet send_packet;
		send_packet << UNSIGNED_SHORT(SV_TO_CL_CHAT);
		send_packet << cl.id << msg;

		for(auto iter = sv_context.clients.Begin();
			iter != sv_context.clients.End();
			++iter)
		{
			if(cl.id == (*iter).GetKey()) continue;
			SafeSend((*iter).GetKey(), send_packet);
		}
	}
}