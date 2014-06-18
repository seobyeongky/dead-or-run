/*OOPARTS OOPARTS OOPARTS OOPARTS OOPARTS OOPARTS OOPARTS
=========================================================
	sv_thread.h

	SvThread class�� header file�Դϴ�.
=======================================================*/

#pragma once

#include "../internal.h"
#include "../thread.h"
#include "../s_map.h"
#include "../protocol.h"

#include "sv_scene.h"
#include "sv_context.h"

// server thread�� miliseconds per frame �� ����
#define SV_MSPF			17

class SvThread : public opzThread {
/*	server thread class�Դϴ�. */

public:
				 SvThread();
				~SvThread();

	// thread�� ����� �� ȣ��˴ϴ�.
	void		 EndProcess();

	// ó���� server ������ ����� �ƴ����� �˷��ݴϴ�.
	// ��ȯ���� true���� ����� ��ȯ �� ���Դϴ�. (false�� ���� �𸣴� ������ ��)
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