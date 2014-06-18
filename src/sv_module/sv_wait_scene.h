/*OOPARTS OOPARTS OOPARTS OOPARTS OOPARTS OOPARTS OOPARTS
=========================================================
	sv_wait_scene.h

	SvWaitScene class의 header file입니다.
=======================================================*/

#pragma once

#include "sv_scene.h"
#include "../runner.h"
#include "../util.h"

class SvWaitScene : public SvScene {
/*	SvWaitScene에서 client들끼리 chatting할 수 있습니다.
	그리고 각 client는 자리를 고를 수 있고 캐릭터를 고를 수 있습니다. */

public:
		 SvWaitScene(sv_scene_id_t id);
		~SvWaitScene();

	void Init();

	void HandleNewCl(const client_t & new_c);
	void HandleGoneCl(const client_t & gone_cl);
	void HandleRecvPacket(const client_t & cl, cl_to_sv_t header, Packet & recv_packet);

	void Go();

private:
	struct set_t
	{
		bool		mask;
		ID			cl_id;
		
		set_t(void) : mask(false), cl_id(-1) { }
		set_t(bool _mask, ID _cl_id) :
			mask(_mask), cl_id(_cl_id) { }
	};

	struct player_t
	{
		bool		ready;
		runner_id_t	runner_id;
		int			set_pos;

		player_t() : ready(false), runner_id(NR_RUNNER), set_pos(-1) { }
		player_t(bool _ready, runner_id_t _runner_id, int _set_pos) :
			ready(_ready), runner_id(_runner_id),
			set_pos(_set_pos) { }
	};

	typedef SMap<ID, player_t>	player_map_t;

private:
	set_t			_set[SV_MAX_NR_PLAYER];
	player_map_t	_player_map;
	list<ID>		_observers;	// 경쟁에서 진 관전자들

	bool			_all_ready;
	int				_all_ready_time;
	
	void	SendClSet(ID cl_id, int pos);
	void	HandleMovingEvent(ID cl_id, int set_pos, int delta);
	bool	CheckAllReady();
	void	HandleClEvent(const client_t & cl, Packet & recv_packet);

	void	SwitchToPlayScene();

	void	ChangeRandomRunner();
};