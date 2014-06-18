#include "d_input.h"

SHORT result_arr[0xFF] = {0x0000, };

void ResetVKeyState()
{
	for(int i = 0; i < 0xFF; ++i)
	{
		GetAsyncKeyState(i);
		result_arr[i] = 0;
	}
}

key_state_t GetVKeyState(int v_key)
{
	if(v_key >= 0xff) return KEY_TOP_HOLD;

	SHORT result = GetAsyncKeyState(v_key);
	key_state_t func_result;

	if(result_arr[v_key])
	{
		// 이전에 눌렸다.
		if(result) func_result = KEY_BOTTOM_HOLD;
		else func_result = KEY_UP;
	}
	else
	{
		// 이전에 눌리지 않았다.
		if(result) func_result = KEY_DOWN;
		else func_result = KEY_TOP_HOLD;
	}

	result_arr[v_key] = result;

	return func_result;
}