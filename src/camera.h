/*OOPARTS OOPARTS OOPARTS OOPARTS OOPARTS OOPARTS OOPARTS
=========================================================
	camera.h

	Camera class�� header file�Դϴ�.
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