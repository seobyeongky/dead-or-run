/*OOPARTS OOPARTS OOPARTS OOPARTS OOPARTS OOPARTS OOPARTS
=========================================================
	starting_scene.h

	StartingScene class의 header file입니다.
=======================================================*/

#pragma once

#include "scene.h"
#include "edit_text.h"
#include "s_map.h"

class StartingScene : public Scene
{
	/*	게임을 시작하기 전에 할 일을 모두 해주는 중요한 scene입니다.
		주로 사용자로부터 알맞고 합리적인 방식으로 입력을 받는데 초점이 맞춰져있습니다.
		방을 만들건지 참여할건지를 고르는데서부터 시작해서 아무튼 여러가지 입력을 받습니다. */
public:
				 StartingScene(scene_id_t id);
				~StartingScene();

	void		 Init();
	
	bool		 HandleTextEntered(wchar_t key);
	bool		 HandleKeyPressed(Event::KeyEvent key);
	bool		 HandleIMECompMsg(wchar_t key, bool done);
	bool		 HandleClientMsg(client_msg_t &msg);

	void		 Go();

private:
	Sprite				_bg;

	bool				_name_switch;
	Text				_name_query_text;
	EditText			_name_text;
	
	bool				_mode_switch;
	Text				_mode_query_text;
	EditText			_mode_text;
	
	bool				_sv_name_text_switch;
	Text				_sv_name_query_text; //게임 만들 때
	
	bool				_sv_name_switch;
	EditText			_sv_name_text;	// 게임 만들 때
	
	bool				_sv_list_switch;
	Text				_sv_list_text;	// 게임 찾을 때
	int					_sv_index_cnt;	// 게임 찾을 때
	SMap<int, wstring>	_sv_addr_map;	// 게임 찾을 때
	
	bool				_sv_index_switch;
	EditText			_sv_index_text;	// 게임 찾을 때

	RectangleShape		_text_bg;
	
	Text				_help_text;
	RectangleShape		_help_bg;

	enum state_t
	{
		GET_NAME,
		GET_MODE,
		SERVER_SEARCHING,
		GET_SERVER_INDEX,
		WAITING_FOR_REMOTE_SERVER,
		GET_SERVER_NAME,
		WAITING_FOR_LOCAL_SERVER
	}					_state;

	void		 UpdateGUI();
	void		 FindingOver();

	void		 AddRemoteServer(const server_info_t & sv_info);

	bool		 BeginSvModule();

	void		 ChangeState(state_t new_state);

	void		 AddOnlineServer();
};