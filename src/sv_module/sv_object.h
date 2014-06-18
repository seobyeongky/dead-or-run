/*OOPARTS OOPARTS OOPARTS OOPARTS OOPARTS OOPARTS OOPARTS
=========================================================
	sv_object.h

	server에서 관리하는 여러 object들입니다. (runner 제외)
=======================================================*/

#pragma once

#include "..\internal.h"
#include "..\coord.h"

typedef pos_t		sv_cactus_t;
#define CACTUS_RADIUS_F	13.f

typedef pos_t		sv_water_t;

// unpicked item on road ...
struct sv_item_t
{
	object_id_t	id;
	item_id_t	item_id;
	pos_t		pos;

	sv_item_t(void) :
		id(INVALID_OBJECT_ID), item_id(ITEM_INVALID), pos()
	{
	}

	sv_item_t(object_id_t _id, item_id_t _item_id, const pos_t & _pos) :
		id(_id), item_id(_item_id), pos(_pos)
	{
	}
};

struct sv_stone_t
{
	object_id_t		id;
	ID				owner_id;
	pos_t			pos;
	vector_t		vcity;
};
#define STONE_RADIUS_F	15.f
#define STONE_SPEED_F	200.f

struct sv_hair_t
{
	object_id_t		id;
	enum owner_type_t
	{
		OWNER_RUNNER,
		OWNER_PORTAL
	}				owner_type;
	ID				owner_id;
	pos_t			pos;
	vector_t		first_vcity;
	vector_t		vcity;
	bool			got;
	enum
	{
		GRAB_RUNNER,
		GRAB_PORTAL,
		GRAB_ITEM
	}				grab_type;
	ID				grabbed_one;
	int				begin_time;

	sv_hair_t(void) :
		id(INVALID_OBJECT_ID), owner_id(-1), pos(),
		first_vcity(), vcity(), got(false),
		grabbed_one(-1), begin_time(0)
	{
	}

	sv_hair_t(object_id_t _id, owner_type_t _owner_type, ID _owner_id, const pos_t & _pos,
		const vector_t & _vcity, bool _got, ID _grabbed_one,
		int _begin_time) :
	id(_id), owner_type(_owner_type), owner_id(_owner_id), pos(_pos),
		first_vcity(_vcity), vcity(_vcity),
		got(_got), grabbed_one(_grabbed_one), begin_time(_begin_time)
	{
	}
};
#define HAIR_SPEED_F		75.f
#define HAIR_FLY_TIME		520

struct sv_smoke_t
{
	object_id_t		id;
	ID				owner_id;
	pos_t			pos;
	vector_t		vcity;
	int				begin_time;

	sv_smoke_t(void) : id(INVALID_OBJECT_ID), owner_id(-1),
		pos(), vcity(), begin_time(0)
	{
	}

	sv_smoke_t(object_id_t _id, ID _owner_id, const pos_t & _pos,
		const vector_t & _vcity, int _begin_time)
		: id(_id), owner_id(_owner_id),
		pos(_pos), vcity(_vcity), begin_time(_begin_time)
	{
	}
};
#define SMOKE_SPEED_F		5.f
#define SMOKE_DURATION		3000

struct sv_portal_t
{
	object_id_t		id;
	ID				owner_id;
	pos_t			pos;
	float			dir;

	sv_portal_t(void): id(INVALID_OBJECT_ID), owner_id(-1),
		pos(), dir(0.f)
	{
	}

	sv_portal_t(object_id_t _id, ID _owner_id, const pos_t & _pos,
		float _dir)
		: id(_id), owner_id(_owner_id), pos(_pos), dir(_dir)
	{
	}
};
#define PORTAL_DIST_F		100.f
#define PORTAL_RADIUS_F		50.f
#define PORTAL_LENGTH_F		150.f

struct sv_booger_t
{
	object_id_t		id;
	ID				owner_id;
	pos_t			pos;
	vector_t		vcity;
};
#define BOOGER_RADIUS_F	15.f
#define BOOGER_SPEED_F	120.f