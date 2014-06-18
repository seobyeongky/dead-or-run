/*OOPARTS OOPARTS OOPARTS OOPARTS OOPARTS OOPARTS OOPARTS
=========================================================
	sv_play_scene.h

	SvPlayScene class의 header file입니다.
=======================================================*/

#pragma once

#include "sv_scene.h"
#include "sv_runner.h"
#include "../tile.h"
#include "physical_engine.h"
#include "sv_object.h"

// play scene의 miliseconds per 동기화 packet 값 정의
#define PLAY_SCENE_MSPPACKET	80

class SvPlayScene : public SvScene {
/*	SvWaitScene에서 client들끼리 chatting할 수 있습니다.
	그리고 각 client는 자리를 고를 수 있고 캐릭터를 고를 수 있습니다. */

public:
		 SvPlayScene(sv_scene_id_t id);
		~SvPlayScene();

	void Init();

	void HandleNewCl(const client_t & new_c);
	void HandleGoneCl(const client_t & gone_cl);
	void HandleRecvPacket(const client_t & cl, cl_to_sv_t header, Packet & recv_packet);

	void Go();

private:
	runner_map_t		_runner_map;
	int					_begin_time;
	bool				_began;

	// TILE 관련
	deque<tile_row_t>	_tile_row_deq;
	size_t				_nr_tile_row;
	size_t				_last_water_row;

	float				_min_y;

	// OBJECT 관련
	object_id_t			_object_id_cnt;
	list<sv_cactus_t>	_cactus_list;
	list<sv_water_t>	_water_list;
	list<sv_item_t>		_item_list;
	list<sv_stone_t>	_stone_list;
	list<sv_hair_t>		_hair_list;
	list<sv_smoke_t>	_smoke_list;
	list<sv_portal_t>	_portal_list;
	list<sv_booger_t>	_booger_list;
	size_t				_last_item_row;
	
	PhysicalEngine		_p_engine;

	bool				_result_send;
	int					_game_over_time;
	bool				_game_over;

	// 동기화 packet 전송 시간 계산용 시간 누적 변수
	int					_accum_time;
	
	void	HandleBackAttackSkill(SvRunner * runner);
	void	HandleMadDogSkill(SvRunner * runner);
	void	HandleBayabaSkill(SvRunner * runner);
	void	HandleGrudgeSkill(ID cl_id, SvRunner * runner);
	void	HandleSmokeSkill(ID cl_id, SvRunner * runner);
	void	HandleLeapSkill(SvRunner * runner);
	void	HandleFlashTransfer(SvRunner * runner);
	void	HandleGodComeSkill(SvRunner * runner);
	void	HandleGhostSkill(SvRunner * runner);
	void	HandleHellGateSkill(ID cl_id, SvRunner * runner);
	void	HandleGroundAttackSkill(ID cl_id, SvRunner * runner);
	void	HandleMagicBoogerSkill(ID cl_id, SvRunner * runner);
	void	HandleProvokingSkill(ID cl_id, SvRunner * runner);
	bool	HandleSkillEvent(ID cl_id, const skill_id_t & skill_id, SvRunner * runner);
	void	HandleItemSpending(item_id_t item_id, ID cl_id, SvRunner * runner);
	void	HandleClEvent(ID cl_id, SvRunner * runner, Packet & recv_packet);
	
	// GO
	void	HandleItemCreation();
	void	HandleHair();
	void	HandleSmoke();
	void	HandleWaterCol();
	void	HandleDeadObject();
	void	HandleObject();	
	void	HandleDeadMan();

	void	HandleTileCreation();
	void	HandleRunnerWaterCol();

	void	SendBasicInfo();

	void	CheckGameOver();
	
	// UTIL
	void			Begin();
	void			SendResult();
	void			End();
	float			GetMinY();
	sv_hair_t *		GetHairAcrossedPortal(ID portal_id);
	void			SendHairDead(ID oid);
	sv_portal_t *	GetPortal(ID oid);
	sv_hair_t *		GetPortalBindingHair(ID portal_id);
	void			SendSkillAccepted(ID cl_id, sv_to_cl_skill_t stype);
};