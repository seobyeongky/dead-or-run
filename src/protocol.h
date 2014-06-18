/*OOPARTS OOPARTS OOPARTS OOPARTS OOPARTS OOPARTS OOPARTS
=========================================================
	protocol.h

	server와 client간의 통신 규약을 정했습니다.
=======================================================*/

#pragma once

#include <string>
using namespace std;

typedef unsigned short header_t;
#define UNSIGNED_SHORT static_cast<unsigned short>

enum sv_to_cl_state_t
{
	SV_TO_CL_WAITING_STATE,
	SV_TO_CL_READY_STATE,
	SV_TO_CL_PLAYING_STATE
};

enum sv_to_cl_game_over_type_t
{
	SV_TO_CL_NO_WINNER,
	SV_TO_CL_THERE_IS_WINNER
};

enum sv_to_cl_event_t
{
	SV_TO_CL_RUNNER_COL_TO_CACTUS,
	SV_TO_CL_HAIR_GOT_RUNNER,
	SV_TO_CL_RUNNER_GOT_WATER,
	SV_TO_CL_STONE_HIT_RUNNER,
	SV_TO_CL_PORTAL_PASS,
	SV_TO_CL_GROUND_ATTACK,
	SV_TO_CL_BOOGER_HIT_RUNNER
};

enum sv_to_cl_skill_t
{
	SV_TO_CL_Q_SKILL_ACCEPTED,
	SV_TO_CL_W_SKILL_ACCEPTED,
	SV_TO_CL_E_SKILL_ACCEPTED,
	SV_TO_CL_R_SKILL_ACCEPTED
};

enum sv_to_cl_t
{
/*	server -> server message header */
	SV_TO_CL_STATE,
	SV_TO_CL_CL_SET,
	SV_TO_CL_CHAT,
	SV_TO_CL_INFO,
	SV_TO_CL_PLAYER_READY,
	SV_TO_CL_BASIC_INFO,
	SV_TO_CL_TILE_ROW,
	SV_TO_CL_GAME_CONTEXT,	// 새로온 클라이언트에게 보내는 context입니다.
	SV_TO_CL_RUNNER_DEAD,
	SV_TO_CL_NEW_OBJECT,
	SV_TO_CL_OBJECT_DEAD,
	SV_TO_CL_ADD_ITEM,
	SV_TO_CL_ERASE_ITEM,
	SV_TO_CL_EVENT,	// miscellaneous events...
	SV_TO_CL_GAME_OVER,
	SV_TO_CL_SKILL_ACCEPTED
};

enum cl_to_sv_event_t
{
	CL_TO_SV_EVENT_UP,
	CL_TO_SV_EVENT_DOWN,
	CL_TO_SV_EVENT_LEFT,
	CL_TO_SV_EVENT_RIGHT,
	CL_TO_SV_EVENT_Q,
	CL_TO_SV_EVENT_W,
	CL_TO_SV_EVENT_E,
	CL_TO_SV_EVENT_R,
	CL_TO_SV_EVENT_READY,
	CL_TO_SV_EVENT_ITEM
};

enum cl_to_sv_t
{
/*	client -> server message header */
	CL_TO_SV_CHAT,
	CL_TO_SV_EVENT,
};

#define BASIC_SAND_STORM_DIST	250
#define BASIC_SAND_STORM_DIST_F	250.f
#define DIST_PER_RUNNER			10
#define DIST_PER_RUNNER_F		10.f


enum object_type_id_t
{
	OBJECT_ITEM,
	OBJECT_STONE,
	OBJECT_HAIR,
	OBJECT_SMOKE,
	OBJECT_PORTAL,
	OBJECT_MAGIC_BOOGER,
	OBJECT_INVALID
};

typedef unsigned short	object_id_t;

#define INVALID_OBJECT_ID	static_cast<object_id_t>(-1)

enum item_id_t
{
	ITEM_STONE,
	ITEM_BIG_WONTON,
	ITEM_CURRY_RICE,
	ITEM_DOUBLE_JAJANG,
	ITEM_JAJANG,
	ITEM_JOB_RICE,
	ITEM_JONG_HO_FOOD,
	ITEM_RAMYUN,
	ITEM_RICE,
	ITEM_TEOK_RAMYUN,
	ITEM_WONTON,
	ITEM_INVALID
};
#define MAX_NR_ITEM_PER_RUNNER	3

#include "tile.h"

enum skill_id_t
{
	BACK_ATTACK_SKILL,
	LEAP_SKILL,
	BAYABA_SKILL,
	MAD_DOG_SKILL,
	SMOKE_SKILL,
	GRUDGE_SKILL,
	FLASH_TRANSFER,
	GOD_COME_SKILL,
	GHOST_SKILL,
	HELL_GATE_SKILL,
	GROUND_ATTACK_SKILL,
	MAGIC_BOOGER_SKILL,
	PROVOKING_SKILL,
	INVALID_SKILL
};

struct skill_info_t
{
	wstring	name;
	int		cost;
	int		cooldown;
	wstring icon;
	wstring desc;

	skill_info_t(void)
		: name(), cost(), cooldown(),
		icon(), desc()
	{
	}

	skill_info_t(wstring _name, int _cost,
		int _cooldown, wstring _icon,
		wstring _desc)
		: name(_name), cost(_cost),
		cooldown(_cooldown), icon(_icon),
		desc(_desc)
	{
	}
};

const skill_info_t * SKILL(skill_id_t sid);


enum runner_id_t
{
	SEO,
	PARK,
	OH,
	JEONG,
	KIM,
	YONG,
	YOON,
	RANDOM_RUNNER,
	NR_RUNNER
};

struct runner_info_t
{
	skill_id_t	q_skill;
	skill_id_t	w_skill;
	skill_id_t	e_skill;
	skill_id_t	r_skill;

	runner_info_t(void)
		: q_skill(), w_skill(),
		e_skill(), r_skill()
	{
	}

	runner_info_t(skill_id_t q, skill_id_t w,
		skill_id_t e, skill_id_t r)
		: q_skill(q), w_skill(w),
		e_skill(e), r_skill(r)
	{
	}
};

enum runner_state_t
{
	RUNNING_STATE,
	Q_SKILL_STATE,
	W_SKILL_STATE,
	E_SKILL_STATE,
	R_SKILL_STATE,
	FALL_DOWN_STATE,
	DEAD_STATE,		// cl side에만 존재
	INVALID_STATE
};

const runner_info_t & CHARACTER(runner_id_t rid);