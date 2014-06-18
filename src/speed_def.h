/*OOPARTS OOPARTS OOPARTS OOPARTS OOPARTS OOPARTS OOPARTS
=========================================================
	speed_def.h

	speed의 단위에 대한 정의

	speed의 단위가 중요합니다.

	200ms 당 pixel로...
=======================================================*/

#pragma once

#include "coord.h"

// miliseconds per pixels (speed 관련)
#define MSPPIXEL	200
#define MSPPIXEL_F	200.f

// NOTE : Object는 GetSpeed와 GetDir이 구현된 class
template <class Object>
inline vector_t GetVcity(const Object * runner, int _delta_time)
{
	return (static_cast<float>(runner->GetSpeed() * _delta_time) / MSPPIXEL_F) *
		GetUnitVector(runner->GetDir());
}
