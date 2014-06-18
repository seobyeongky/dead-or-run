/*OOPARTS OOPARTS OOPARTS OOPARTS OOPARTS OOPARTS OOPARTS
=========================================================
	coord.h

	좌표계 관련 정의
=======================================================*/

#pragma once

#include "internal.h"

typedef Vector2f vector_t;
typedef Vector2f pos_t;

#define PI       3.14159265358979323846f
#define PI_2     1.57079632679489661923f
#define PI_4     0.785398163397448309616f

inline IntRect operator + (const Vector2i & lhs, const IntRect & rhs)
{
	return IntRect(	lhs.x + rhs.left,
					lhs.y + rhs.top,
					rhs.width ,
					rhs.height);
}

inline float operator * (const vector_t & lhs, const vector_t & rhs)
{
	return lhs.x * rhs.x + lhs.y * rhs.y;
}

inline float GetDistance(const vector_t & v1, const vector_t & v2)
{
	vector_t r = v1 - v2;
	return sqrtf(r * r);
}

inline float GetSize(const vector_t & v)
{
	return sqrtf(v * v);
}

inline vector_t GetUnitVector(float dir)
{
	return vector_t(sinf(dir), -cosf(dir));
}

inline vector_t GetUnitVector(const vector_t & v)
{
	float size = GetSize(v);
	return v / size;
}

inline float GetDir(const vector_t & v)
{
	float angle = atan2(v.x, -v.y);
	if(angle < 0.f) return angle + 2.f * PI;
	else return angle;
}

inline float ToDegree(float angle)
{
	return 180.f * angle / static_cast<float>(PI);
}
