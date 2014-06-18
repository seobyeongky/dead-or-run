/*OOPARTS OOPARTS OOPARTS OOPARTS OOPARTS OOPARTS OOPARTS
=========================================================
	object_interface.h

	IObject의 header file입니다.
=======================================================*/

#pragma once

#include "internal.h"
#include "coord.h"

class IObject
{
public:
	enum priority_t
	{
		FALL_DOWN_PRIORITY,
		GROUND_PRIORITY,
		AIR_PRIORITY
	};

public:
	virtual const pos_t &	GetPos() const = 0;
	virtual void			Draw(RenderTarget & target) = 0;
	virtual priority_t		GetPriority() const = 0;
	virtual void			CorrectPos() = 0;
};