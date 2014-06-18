/*OOPARTS OOPARTS OOPARTS OOPARTS OOPARTS OOPARTS OOPARTS
=========================================================
	cooldown_shadow.h

	CooldownShadow class의 header file입니다.
=======================================================*/

#pragma once

#include "internal.h"

class CooldownShadow : public Drawable
{
public:
			CooldownShadow(const Color & color);
	void	SetRatio(float ratio);

private:
	virtual void draw(RenderTarget& target, RenderStates states) const;

private:
	static const vector<Vertex>	frame;

	Color			_color;
	VertexArray		_vertices;
};
