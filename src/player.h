/*OOPARTS OOPARTS OOPARTS OOPARTS OOPARTS OOPARTS OOPARTS
=========================================================
	player.h

	player_t 구조체를 정의했습니다.
=======================================================*/

#pragma once

#include "internal.h"

class Runner;

struct player_t
{
	wstring		name;
	Color		color;
	bool		alive;
	Runner *	runner;
	bool		ready;

	player_t() : name(), color(), alive(false), runner(nullptr), ready(false) { }
	player_t(const wstring & _name, const Color & _color, bool _alive,
		Runner * _runner, bool _ready)
		: name(_name), color(_color), alive(_alive), runner(_runner),
		ready(_ready)
	{ }
};