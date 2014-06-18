/*OOPARTS OOPARTS OOPARTS OOPARTS OOPARTS OOPARTS OOPARTS
=========================================================
	smoke.h

	오현석의 연막!
=======================================================*/

#pragma once

#include "internal.h"
#include "coord.h"

#define SMOKE_RADIUS	220.f

class Smoke : public Drawable
{
public:
			Smoke(const pos_t & pos, const vector_t & vcity, bool transparent);

	void	Update();

private:
	void draw(RenderTarget& target, RenderStates states) const;

private:
	Sprite		_sprite;
	pos_t		_pos;
	vector_t	_vcity;
	int			_set_time;
};
