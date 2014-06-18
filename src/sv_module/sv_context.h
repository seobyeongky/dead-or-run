/*OOPARTS OOPARTS OOPARTS OOPARTS OOPARTS OOPARTS OOPARTS
=========================================================
	sv_context.h

	server context 구조체를 정의했습니다.
=======================================================*/

#pragma once

#include "../s_map.h"
#include "../internal.h"

struct cl_context_t
{
	client_t	info;
	int			delay_time;
	size_t		nr_accum_win;
};


struct sv_context_t
{
	wstring					name;

	SMap<ID, cl_context_t>	clients;
};