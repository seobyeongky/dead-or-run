/*OOPARTS OOPARTS OOPARTS OOPARTS OOPARTS OOPARTS OOPARTS
=========================================================
	starting_scene.h

	StartingScene class�� header file�Դϴ�.
=======================================================*/

#pragma once

#include "scene.h"
#include "edit_text.h"
#include "s_map.h"

class StartingScene : public Scene
{
	/*	������ �����ϱ� ���� �� ���� ��� ���ִ� �߿��� scene�Դϴ�.
		�ַ� ����ڷκ��� �˸°� �ո����� ������� �Է��� �޴µ� ������ �������ֽ��ϴ�.
		���� ������� �����Ұ����� ���µ������� �����ؼ� �ƹ�ư �������� �Է��� �޽��ϴ�. */
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
	Text				_sv_name_query_text; //���� ���� ��
	
	bool				_sv_name_switch;
	EditText			_sv_name_text;	// ���� ���� ��
	
	bool				_sv_list_switch;
	Text				_sv_list_text;	// ���� ã�� ��
	int					_sv_index_cnt;	// ���� ã�� ��
	SMap<int, wstring>	_sv_addr_map;	// ���� ã�� ��
	
	bool				_sv_index_switch;
	EditText			_sv_index_text;	// ���� ã�� ��

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