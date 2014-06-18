/*OOPARTS OOPARTS OOPARTS OOPARTS OOPARTS OOPARTS OOPARTS
=========================================================
	camera.h

	Camera class의 header file입니다.
=======================================================*/

#pragma once

#include "internal.h"

class Camera
{
public:
					Camera();
	
	float			GetBottomOffset() const;
	const View &	GetView() const { return _view; }
	float			GetCurY() const { return _cur_y; }

	void			SetZero();
	void			SetY(float y);
	void			Move();
	void			StraightMove(float delta_y);

private:
	View	_view;
	float	_cur_y;
	float	_dest_y;
};