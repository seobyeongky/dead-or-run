#include "play_scene.h"
#include "res.h"
#include "sv_module\sv_module.h"

#define Q_KEY 0x51
#define W_KEY 0x57
#define E_KEY 0x45
#define R_KEY 0x54

void PlayScene::HandleDirKeyInput(int dir_v_key, cl_to_sv_event_t key_event)
{
	key_state_t state = GetVKeyState(dir_v_key);
	if(state == KEY_DOWN)
	{
		Packet send_packet;
		send_packet << UNSIGNED_SHORT(CL_TO_SV_EVENT)
					<< UNSIGNED_SHORT(key_event)
					<< UNSIGNED_SHORT(KEY_DOWN);
		SafeSend(send_packet);
	}
	else if(state == KEY_UP)
	{
		Packet send_packet;
		send_packet << UNSIGNED_SHORT(CL_TO_SV_EVENT)
					<< UNSIGNED_SHORT(key_event)
					<< UNSIGNED_SHORT(KEY_UP);
		SafeSend(send_packet);
	}
}

void PlayScene::HandleSkillKeyInput(int dir_v_key, cl_to_sv_event_t key_event)
{
	if (GetVKeyState(dir_v_key) == KEY_DOWN)
	{
		auto &char_info = CHARACTER(_me->runner->GetId());
		const skill_info_t * skill;
		if (dir_v_key == Q_KEY) skill = SKILL(char_info.q_skill);
		else if (dir_v_key == W_KEY) skill = SKILL(char_info.w_skill);
		else if (dir_v_key == E_KEY) skill = SKILL(char_info.e_skill);
		else if (dir_v_key == R_KEY) skill = SKILL(char_info.r_skill);
		else return;

		if (skill == nullptr) return;

		if (skill->cost < _health)
		{
			bool can = false;
			if (dir_v_key == Q_KEY) can = _skill_box.CheckQSkillAvailable();
			else if (dir_v_key == W_KEY) can = _skill_box.CheckWSkillAvailable();
			else if (dir_v_key == E_KEY) can = _skill_box.CheckESkillAvailable();
			
			if (can)
			{
				Packet send_packet;
				send_packet << UNSIGNED_SHORT(CL_TO_SV_EVENT)
							<< UNSIGNED_SHORT(key_event);
				SafeSend(send_packet);
			}
		}
	}
}

void PlayScene::HandleSpacebarInput()
{
	if(GetVKeyState(VK_SPACE) == KEY_DOWN)
	{
		if(_sv_state == SV_TO_CL_WAITING_STATE)
		{
			Packet send_packet;
			send_packet	<< UNSIGNED_SHORT(CL_TO_SV_EVENT)
						<< UNSIGNED_SHORT(CL_TO_SV_EVENT_READY);
			SafeSend(send_packet);
		}
		else if(_sv_state == SV_TO_CL_PLAYING_STATE)
		{
			item_id_t item_id;
			if(!(_inventory_box.SpendItem(&item_id))) return;
			Packet send_packet;
			send_packet	<< UNSIGNED_SHORT(CL_TO_SV_EVENT)
						<< UNSIGNED_SHORT(CL_TO_SV_EVENT_ITEM)
						<< UNSIGNED_SHORT(item_id);
			SafeSend(send_packet);
		}
	}
}

void PlayScene::HandleNumberInput()
{
}

void PlayScene::HandleKeyInput()
{
	if (window.getSystemHandle() != GetFocus()) return;
	if (_me->runner == nullptr) return;

	HandleDirKeyInput(VK_LEFT, CL_TO_SV_EVENT_LEFT);
	HandleDirKeyInput(VK_UP, CL_TO_SV_EVENT_UP);
	HandleDirKeyInput(VK_RIGHT, CL_TO_SV_EVENT_RIGHT);
	HandleDirKeyInput(VK_DOWN, CL_TO_SV_EVENT_DOWN);

	HandleSkillKeyInput(Q_KEY, CL_TO_SV_EVENT_Q);
	HandleSkillKeyInput(W_KEY, CL_TO_SV_EVENT_W);
	HandleSkillKeyInput(E_KEY, CL_TO_SV_EVENT_E);
	HandleSkillKeyInput(R_KEY, CL_TO_SV_EVENT_R);

	HandleSpacebarInput();
	HandleNumberInput();
}

bool PlayScene::HandleTextEntered(wchar_t key)
{
	if(!_chat_box.CheckEnterDone())
	{
		if(key == UNICODE_ESC)
		{
			_chat_box.AlertEnterDone();
			ResetVKeyState();
			return true;
		}
		else return _chat_box.HandleTextEntered(key);
	}
	else
	{
		if(key == UNICODE_ESC)
		{
			DisconnectToServer();
			if(_sv_host) { SvModule::S_Delete(); }
			SetNextScene(STARTING_SCENE);
			return true;
		}
		else return false;
	}
}

bool PlayScene::HandleKeyPressed(Event::KeyEvent key)
{
	if(!_chat_box.CheckEnterDone())
	{
		bool result = _chat_box.HandleKeyPressed(key);
		if(_chat_box.CheckEnterDone()) ResetVKeyState();
		return result;
	}
	else
	{
		if(key.code == Keyboard::Return) {_chat_box.BeginToEnter(); return true; }
		else if(key.code == Keyboard::Num1)
		{ 
			_inventory_box.HandleNumberKeyInput(1);
			return true;
		}
		else if(key.code == Keyboard::Num2)
		{
			_inventory_box.HandleNumberKeyInput(2);
			return true;
		}
		else if(key.code == Keyboard::Num3)
		{
			_inventory_box.HandleNumberKeyInput(3);
			return true;
		}
	}
	return false;
}

bool PlayScene::HandleIMECompMsg(wchar_t key, bool done)
{
	return _chat_box.HandleIMECompMsg(key, done);
}