/*OOPARTS OOPARTS OOPARTS OOPARTS OOPARTS OOPARTS OOPARTS
=========================================================
	play_scene.h

	PlayScene class의 header file입니다.
=======================================================*/

#pragma once

#include "scene.h"
#include "edit_text.h"
#include "s_map.h"
#include "runner.h"
#include "internal.h"
#include "coord.h"
#include "tile.h"
#include "tilemap.h"
#include "char_desc.h"
#include "big_msg.h"
#include "camera.h"
#include "chat_box.h"
#include "player.h"
#include "sandstorm.h"
#include "runner_factory.h"
#include "object_palette.h"
#include "object.h"
#include "inventory_box.h"
#include "smoke.h"
#include "portal.h"
#include "skill_box.h"

class PlayScene : public Scene {
/*	클라이언트쪽 게임플레이 scene입니다.
	사용자로부터 입력받은 내용을 잘 처리해서 서버에게 보냅니다.
	그리고 서버로부터 갱신된 게임월드 정보를 계속 알아냅니다.
	그리고 보여줍니다. */

public:
				 PlayScene(scene_id_t id);
				~PlayScene();

	void		 Init();
	
	bool		 HandleTextEntered(wchar_t key);
	bool		 HandleKeyPressed(Event::KeyEvent key);
	bool		 HandleIMECompMsg(wchar_t key, bool done);
	bool		 HandleClientMsg(client_msg_t & msg);

	void		 Go();

private:
	typedef SMap<ID, player_t> player_map_t;

	// Virtual Key, Client to Server Key Event
	typedef pair<int, cl_to_sv_event_t> key_event_pair_t;

	typedef	SMap<object_id_t, IObject *> object_map_t;
	typedef SMap<object_id_t, Hair *> hair_map_t;
	typedef SMap<object_id_t, Smoke *> smoke_map_t;
	typedef SMap<object_id_t, Portal *> portal_map_t;

	typedef list<IEffect *> effect_list_t;

private:
	// for debug
	Text						_debug_text;

	// server 정보
	wstring						_sv_name;
	
	// server host 인지 여부
	bool						_sv_host;

	// player 정보
	player_map_t				_player_map;
	ID							_my_id;
	player_t *					_me;

	// Object Palette
	ObjectPalette				_object_palette;

	// Runner Factory
	RunnerFactory				_runner_factory;

	// chat box
	ChatBox						_chat_box;

	// 가장 위 좌표 계산값
	float						_min_y;
	float						_cur_min_y;
	
	// dark
	RectangleShape				_dark_bg;

	// desc
	CharDesc					_char_desc;

	// big msg
	BigMsg						_big_msg;

	// green jone
	CircleShape					_green_zone[SV_MAX_NR_PLAYER];

	// ready text
	Text						_ready_text;

	// camera
	Camera						_camera;
	bool						_camera_on;

	// tlie 관련
	TileMap						_tile_map;
	deque<Cactus *>				_cactus_deq;
	const Sprite *				_cactus_sprite;

	// 체력 관련
	int							_health;

	// state
	sv_to_cl_state_t			_sv_state;

	// 모래 폭풍
	Sandstorm					_sandstorm;

	// objects
	object_map_t				_object_map;
	hair_map_t					_hair_map;
	smoke_map_t					_smoke_map;
	portal_map_t				_portal_map;

	// 기타 효과들
	effect_list_t				_effect_list;

	// UI
	InventoryBox				_inventory_box;
	SkillBox					_skill_box;
	Text						_hp_text;
	Text						_hp_text_outline;

	// 그... 모두 ready하고나서
	bool						_all_ready;
	size_t						_all_ready_cnt;
	int							_all_ready_accum_time;

	// INPUT
	void						HandleDirKeyInput(int dir_v_key, cl_to_sv_event_t key_event);
	void						HandleSkillKeyInput(int dir_v_key, cl_to_sv_event_t key_event);
	void						HandleSpacebarInput();
	void						HandleNumberInput();
	void						HandleKeyInput();

	// CAMERA
	void						HandleCamera();

	// DRAW
	void						HandleEffects();
	void						Draw();

	// NETWORK COMMUNICATION
	void						HandleNewClient(client_msg_t & msg);
	void						HandleGoneClient(client_msg_t & msg);

	void						HandleSvState(Packet & recv_packet);
	void						HandleChatMsg(Packet & recv_packet);
	void						HandleInfoMsg(Packet & recv_packet);
	void						HandleClSetMsg(Packet & recv_packet);
	void						HandleClReadyMsg(Packet & recv_packet);
	void						HandleBasicInfo(Packet & recv_packet);
	void						HandleTileRow(Packet & recv_packet);
	void						HandleGameContext(Packet & recv_packet);
	void						HandleDeadRunner(Packet & recv_packet);
	void						HandleNewObject(Packet & recv_packet);
	void						HandleGameOver(Packet & recv_packet);
	void						HandleObjectDead(Packet & recv_packet);
	void						HandleAddedItem(Packet & recv_packet);
	void						HandleErasedItem(Packet & recv_packet);
	void						HandleSvEvent(Packet & recv_packet);
	void						HandleAcceptedSkill(Packet & recv_packet);
	void						HandleRecvPacket(Packet & recv_packet);
	
	// UTIL
	void						HandleTickTock();
	void						SetDebugMsg(const wchar_t * format_str, ...);
	void						CameraOn();
	void						CameraOff();
	void						HandleCactus();
	void						FlyBodies();
	void						RecalculateMinY();
	size_t						GetNRRunner();
	float						GetXPosByTileUnitPos(int tile_unit_x);
};