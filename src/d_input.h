/*OOPARTS OOPARTS OOPARTS OOPARTS OOPARTS OOPARTS OOPARTS
=========================================================

========================================================*/
#pragma once

#include "internal.h"

enum key_state_t
{
	KEY_DOWN,			// 아래로 움직일 때
	KEY_BOTTOM_HOLD,	// 눌려진 상태일 때
	KEY_UP,				// 위로 움직일 때
	KEY_TOP_HOLD		// 아무것도...			
};

void			ResetVKeyState();
key_state_t		GetVKeyState(int v_key);