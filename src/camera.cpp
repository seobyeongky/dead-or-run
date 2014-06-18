#include "camera.h"
#include "res.h"
#include "play_scene_layout.h"

Camera::Camera()
	: _view(),
	_cur_y(0.f),
	_dest_y(0.f)
{
}

float Camera::GetBottomOffset() const 
{
	return static_cast<float>(window.getSize().y - CHEIBRIADOS - 35.f);
}

void Camera::SetZero()
{
	_view = window.getDefaultView();
	_view.move(	-(window.getSize().x / 2.f),
				-GetBottomOffset());
	_cur_y = 0.f;
	_dest_y = 0.f;
}

void Camera::SetY(float _y)
{
	_dest_y = _y;
}

void Camera::Move()
{
	float d = (_dest_y - _cur_y) * 0.7f * static_cast<float>(delta_time) / 50.f;
	_view.move(0.f, d);
	_cur_y += d;
}

void Camera::StraightMove(float delta_y)
{
	_view.move(0.f, delta_y);
}