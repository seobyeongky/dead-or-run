/*OOPARTS OOPARTS OOPARTS OOPARTS OOPARTS OOPARTS OOPARTS
=========================================================

========================================================*/
#pragma once

#include "internal.h"

enum key_state_t
{
	KEY_DOWN,			// �Ʒ��� ������ ��
	KEY_BOTTOM_HOLD,	// ������ ������ ��
	KEY_UP,				// ���� ������ ��
	KEY_TOP_HOLD		// �ƹ��͵�...			
};

void			ResetVKeyState();
key_state_t		GetVKeyState(int v_key);