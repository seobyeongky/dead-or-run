/*OOPARTS OOPARTS OOPARTS OOPARTS OOPARTS OOPARTS OOPARTS
=========================================================
	scene.h

	기본 scene class의 header file입니다.
=======================================================*/

#pragma once

#include "internal.h"

enum scene_id_t
{
	// scene들입니다.
	INVALID_SCENE = -1,
	INTRO_SCENE = 0U,
	STARTING_SCENE = 1U,
	PLAY_SCENE = 2U,
	NR_SCENE = 3U
};

// starting scene -> playing scene으로 넘어갈 때 프로토콜
enum starting_to_playing_switching_header_t
{
	STPS_HOSTING_MODE = 0,		// hosting server
	STPS_JOINING_MODE = 1		// joining server
};

class Scene {
/*	client program에서 중요한 역할을 하는 scene의 기본 abstract class입니다.
	client program은 FSM model을 기반으로 scene을 관리합니다.
	scene이 하나의 state라고 생각할 수 있습니다. */
	
	friend class SceneMgr;

public:
	void				 OnInit() { _next_scene = INVALID_SCENE; Init(); }
	
	// member 변수들을 초기화시키는 함수입니다.
	// derived scene은 instance가 재사용되기 때문에 생성자함수가 아닌 Init함수로 초기화시키십시오.
	virtual void		 Init() = 0;

	// 사용자가 Text를 입력했을 때 main logic에서 호출합니다.
	virtual bool		 HandleTextEntered(wchar_t key) {return false; }

	// 사용자가 Key를 눌렀을 때 main logic에서 호출됩니다.
	virtual bool		 HandleKeyPressed(Event::KeyEvent key) {return false; }

	// IME comp
	virtual bool		 HandleIMECompMsg(wchar_t code, bool done) {return false; }

	// OPZNET client service에서 메시지가 왔을 때 호출됩니다.
	virtual bool		 HandleClientMsg(client_msg_t & msg) {return false;}

	// 매 프레임마다 호출되는 함수입니다.
	// 규칙 : 다음번 scene의 id를 반환해야 합니다.
	virtual void		 Go() = 0;

	scene_id_t			 GetId() const {return _id; }

	scene_id_t			 GetNextScene() const { return _next_scene; }

protected:
						 Scene(scene_id_t id);
	virtual				~Scene();

	void				 SetNextScene(scene_id_t id) { _next_scene = id; }

	// 아래 두 함수를 이용하여 유일하게 scene과 scene간에 통신할 수 있습니다.
	// SavePacket으로 packet을 저장하고 LoadPacket으로 저장한 packet을 불러옵니다.
	// packet은 전역 범위입니다.
	void				 SavePacket(const Packet &packet) {_packet = packet;}
	void				 LoadPacket(Packet *dest) { *dest = _packet; }

private:
	scene_id_t			_id;
	scene_id_t			_next_scene;
	static Packet		_packet;

						 Scene(const Scene &other);					// 복사 방지용
						 Scene & operator = (const Scene &other);	// 복사 방지용
};

class SceneMgr {
/*	scene은 FSM model의 state처럼 여겨집니다.
	그래서 각각의 state(scene)의 instance를 저장해야합니다.
	SceneMgr singleton class이 그 역할을 맡습니다. */
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

			 SceneMgr(const SceneMgr & other);					// 복사 방지용
			 SceneMgr & operator = (const SceneMgr & other);	// 복사 방지용
};