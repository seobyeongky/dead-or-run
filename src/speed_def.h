/*OOPARTS OOPARTS OOPARTS OOPARTS OOPARTS OOPARTS OOPARTS
=========================================================
	speed_def.h

	speed�� ������ ���� ����

	speed�� ������ �߿��մϴ�.

	200ms �� pixel��...
=======================================================*/

#pragma once

#include "coord.h"

// miliseconds per pixels (speed ����)
#define MSPPIXEL	200
#define MSPPIXEL_F	200.f

// NOTE : Object�� GetSpeed�� GetDir�� ������ class
template <class Object>
inline vector_t GetVcity(const Object * runner, int _delta_time)
{
	return (static_cast<float>(runner->GetSpeed() * _delta_time) / MSPPIXEL_F) *
		GetUnitVector(runner->GetDir());
}
