/*OOPARTS OOPARTS OOPARTS OOPARTS OOPARTS OOPARTS OOPARTS
=========================================================
	sv_res.h

	server�� ����ϴ� �ڿ����� �����߽��ϴ�.
=======================================================*/

#pragma once

#include "sv_context.h"

extern int				sv_present_time;
extern int				sv_delta_time;
extern sv_context_t		sv_context;

void	SafeSendToAll(const Packet & packet);
void	UnsafeSendToAll(const Packet & packet);