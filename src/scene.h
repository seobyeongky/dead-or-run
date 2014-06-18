/*OOPARTS OOPARTS OOPARTS OOPARTS OOPARTS OOPARTS OOPARTS
=========================================================
	scene.h

	�⺻ scene class�� header file�Դϴ�.
=======================================================*/

#pragma once

#include "internal.h"

enum scene_id_t
{
	// scene���Դϴ�.
	INVALID_SCENE = -1,
	INTRO_SCENE = 0U,
	STARTING_SCENE = 1U,
	PLAY_SCENE = 2U,
	NR_SCENE = 3U
};

// starting scene -> playing scene���� �Ѿ �� ��������
enum starting_to_playing_switching_header_t
{
	STPS_HOSTING_MODE = 0,		// hosting server
	STPS_JOINING_MODE = 1		// joining server
};

class Scene {
/*	client program���� �߿��� ������ �ϴ� scene�� �⺻ abstract class�Դϴ�.
	client program�� FSM model�� ������� scene�� �����մϴ�.
	scene�� �ϳ��� state��� ������ �� �ֽ��ϴ�. */
	
	friend class SceneMgr;

public:
	void				 OnInit() { _next_scene = INVALID_SCENE; Init(); }
	
	// member �������� �ʱ�ȭ��Ű�� �Լ��Դϴ�.
	// derived scene�� instance�� ����Ǳ� ������ �������Լ��� �ƴ� Init�Լ��� �ʱ�ȭ��Ű�ʽÿ�.
	virtual void		 Init() = 0;

	// ����ڰ� Text�� �Է����� �� main logic���� ȣ���մϴ�.
	virtual bool		 HandleTextEntered(wchar_t key) {return false; }

	// ����ڰ� Key�� ������ �� main logic���� ȣ��˴ϴ�.
	virtual bool		 HandleKeyPressed(Event::KeyEvent key) {return false; }

	// IME comp
	virtual bool		 HandleIMECompMsg(wchar_t code, bool done) {return false; }

	// OPZNET client service���� �޽����� ���� �� ȣ��˴ϴ�.
	virtual bool		 HandleClientMsg(client_msg_t & msg) {return false;}

	// �� �����Ӹ��� ȣ��Ǵ� �Լ��Դϴ�.
	// ��Ģ : ������ scene�� id�� ��ȯ�ؾ� �մϴ�.
	virtual void		 Go() = 0;

	scene_id_t			 GetId() const {return _id; }

	scene_id_t			 GetNextScene() const { return _next_scene; }

protected:
						 Scene(scene_id_t id);
	virtual				~Scene();

	void				 SetNextScene(scene_id_t id) { _next_scene = id; }

	// �Ʒ� �� �Լ��� �̿��Ͽ� �����ϰ� scene�� scene���� ����� �� �ֽ��ϴ�.
	// SavePacket���� packet�� �����ϰ� LoadPacket���� ������ packet�� �ҷ��ɴϴ�.
	// packet�� ���� �����Դϴ�.
	void				 SavePacket(const Packet &packet) {_packet = packet;}
	void				 LoadPacket(Packet *dest) { *dest = _packet; }

private:
	scene_id_t			_id;
	scene_id_t			_next_scene;
	static Packet		_packet;

						 Scene(const Scene &other);					// ���� ������
						 Scene & operator = (const Scene &other);	// ���� ������
};

class SceneMgr {
/*	scene�� FSM model�� stateó�� �������ϴ�.
	�׷��� ������ state(scene)�� instance�� �����ؾ��մϴ�.
	SceneMgr singleton class�� �� ������ �ý��ϴ�. */
public:
	static void		S_Create();
	static void		S_Delete();

	static void		S_Bind(scene_id_t id, Scene *scene);
	static Scene *	S_GetScene(scene_id_t id);

private:
	static SceneMgr *	_instance;

	vector<Scene *>		_scene_container;

	void	 CreateSceneTable();

			 SceneMgr();
			~SceneMgr();

			 SceneMgr(const SceneMgr & other);					// ���� ������
			 SceneMgr & operator = (const SceneMgr & other);	// ���� ������
};