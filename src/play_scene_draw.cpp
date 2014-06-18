#include "play_scene.h"
#include "res.h"

void PlayScene::HandleCamera()
{
	_camera.SetY(_cur_min_y + 200.f);
	_camera.Move();
}

void PlayScene::HandleEffects()
{
	for (auto iter = _effect_list.begin(); iter != _effect_list.end();)
	{
		IEffect * eff = *iter;
		if (present_time > eff->GetBeginTime() + eff->GetDuration())
		{
			_object_palette.EraseObject(eff);
			delete eff;
			iter = _effect_list.erase(iter);
		}
		else
		{
			++iter;
		}
	}
}

void PlayScene::Draw()
{
	if(_sv_state == SV_TO_CL_READY_STATE ||
		_sv_state == SV_TO_CL_PLAYING_STATE)
	{
		// DRAW TILES
		CameraOn();
		window.draw(_tile_map);
	}

	if(_sv_state == SV_TO_CL_READY_STATE)
	{
		// DRAW BACK GROUND
		int a = _dark_bg.getFillColor().a;
		--a;
		if(a < 0) a = 0;
		_dark_bg.setFillColor(Color(0, 0, 0, a));
		CameraOff();
		window.draw(_dark_bg);
	}

	if(_sv_state == SV_TO_CL_WAITING_STATE ||
		GetVKeyState(VK_TAB) == KEY_BOTTOM_HOLD || KEY_DOWN)
	{
		// DRAW CHARACTER's DESCRIPTION
		CameraOff();
		window.draw(_char_desc);
	}

	if(_sv_state == SV_TO_CL_WAITING_STATE)
	{
		// DRAW GREEN ZONE
		CameraOff();
		for(int i = 0; i < SV_MAX_NR_PLAYER; ++i)
			window.draw(_green_zone[i]);

		// DRAW READY TEXT
		CameraOn();
		for(auto iter = _player_map.Begin(); iter != _player_map.End(); ++iter)
		{
			Runner * runner = (*iter).GetEl().runner;
			if(!runner || !(*iter).GetEl().ready) continue;
			pos_t pos = runner->GetPos();
			pos.y -= 80.f;
			_ready_text.setPosition(pos);
			window.draw(_ready_text);
		}
	}

	// DRAW OBJECTS
	CameraOn();
	_object_palette.Update();
	window.draw(_object_palette);

	// DRAW SMOKES
	for(auto iter = _smoke_map.Begin(); iter != _smoke_map.End(); ++iter)
	{
		(*iter).GetEl()->Update();
		window.draw(*(*iter).GetEl());
	}

	CameraOff();
	if(_sv_state == SV_TO_CL_PLAYING_STATE ||
		_sv_state == SV_TO_CL_READY_STATE)
	{
		window.draw(_inventory_box);
		window.draw(_skill_box);
		window.draw(_sandstorm);
	}
	window.draw(_chat_box);

	// DRAW HP TEXT
	if(_sv_state == SV_TO_CL_READY_STATE ||
		_sv_state == SV_TO_CL_PLAYING_STATE)
	{
		window.draw(_hp_text_outline);
		window.draw(_hp_text);
	}
	// DRAW BIG MESSAGE
	window.draw(_big_msg);

	// DRAW DEBUG TEXT
#ifdef _DEBUG
	window.draw(_debug_text);
#endif
}

void PlayScene::RecalculateMinY()
{
	float min_y = 0.f;

	for(auto iter = _player_map.Begin(); iter != _player_map.End(); ++iter)
	{
		Runner * runner = (*iter).GetEl().runner;
		if(runner == nullptr) continue;
		if(runner->GetSpriteY() < min_y)
			min_y = runner->GetSpriteY();
	}
	_cur_min_y = min_y;
	if(min_y < _min_y) _min_y = min_y;
}