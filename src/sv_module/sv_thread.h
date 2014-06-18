/*OOPARTS OOPARTS OOPARTS OOPARTS OOPARTS OOPARTS OOPARTS
=========================================================
	sv_thread.h

	SvThread class의 header file입니다.
=======================================================*/

#pragma once

#include "../internal.h"
#include "../thread.h"
#include "../s_map.h"
#include "../protocol.h"

#include "sv_scene.h"
#include "sv_context.h"

// server thread의 miliseconds per frame 값 정의
#define SV_MSPF			17

class SvThread : public opzThread {
/*	server thread class입니다. */

public:
				 SvThread();
				~SvThread();

	// thread가 종료될 때 호출됩니다.
	void		 EndProcess();

	// 처음에 server 시작이 제대로 됐는지를 알려줍니다.
	// 반환값이 true여야 제대로 반환 된 것입니다. (false는 아직 모르는 상태일 때)
	enum		 bresult_t { SUCCEEDED, BIND_FAILED};
	bool		 GetBeginningResult(bresult_t * result_ptr);

	void		 SetName(const wstring & name);
	void		 SetExitFlag() { _exit = true; }

protected:
	void		 Run();

private:
	bool				_bresult_set;
	bresult_t			_bresult;

	SvScene	*			_scene_table[SV_NR_SCENE];
	SvScene *			_cur_scene;

	Clock				_clock;

	bool				_exit;

	void	CreateSceneTable();
	
	void	SendInfo(ID cl_id, const wstring & info);
	void	HandleChatRequest(const client_t & cl, Packet & recv_packet);
};