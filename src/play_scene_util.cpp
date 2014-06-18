#include "play_scene.h"
#include "play_scene_layout.h"
#include "res.h"
#include "sv_module\sv_module.h"
#include "util.h"
#include "speed_def.h"

void PlayScene::HandleTickTock()
{
	if(	_sv_state == SV_TO_CL_WAITING_STATE &&
		_all_ready)
	{
		_all_ready_accum_time += delta_time;
		if(_all_ready_accum_time >= 1000)
		{
			_all_ready_accum_time -= 1000;
			wstring msg(L"게임 시작까지 ");
			msg += to_wstring(_all_ready_cnt) +
				L"초 남았습니다...";
			_chat_box.AddInfoMsg(msg);
			sfx_mgr->Play(SFX_BELL);
			if(_all_ready_cnt >= 0U) --_all_ready_cnt;
		}
	}
}

void PlayScene::SetDebugMsg(const wchar_t * format_str, ...)
{
	va_list args;
	va_start(args, format_str);
	int length = _vscwprintf(format_str, args) + 1;  
	WCHAR *buf = static_cast<WCHAR *>(
		_malloca(length * sizeof(WCHAR)));
	vswprintf_s(buf, length, format_str, args);
	
	_debug_text.setString(buf);

	_freea(buf);
	va_end(args);
}

void PlayScene::CameraOn()
{
	if(_camera_on) return;
	window.setView(_camera.GetView());
	_camera_on = true;
}

void PlayScene::CameraOff()
{
	if(!_camera_on) return;
	window.setView(window.getDefaultView());
	_camera_on = false;
}

void PlayScene::FlyBodies()
{
	for(auto iter = _player_map.Begin(); iter != _player_map.End(); ++iter)
	{
		Runner * runner = (*iter).GetEl().runner;
		if(runner != nullptr && !(*iter).GetEl().alive)
		{
			pos_t pos = runner->GetPos();
			pos.x += 50.f * delta_time / MSPPIXEL_F;
			if(pos.x > static_cast<float>(window.getSize().x))
				pos.x = -static_cast<float>(window.getSize().x);
			
			float storm_dist = DIST_PER_RUNNER_F * GetNRRunner() + BASIC_SAND_STORM_DIST_F;
			float dest_y = _min_y + storm_dist;
			pos.y = (pos.y + dest_y) / 2.f;

			runner->SetPos(pos.x, pos.y);

			float dir = runner->GetDir();
			dir += delta_time / 100.f;
			if(dir > 2.f * PI) dir -= 2.f * PI;
			runner->SetDir(dir);
		}
	}
}

void PlayScene::HandleCactus()
{
	for(auto iter = _cactus_deq.begin(); iter != _cactus_deq.end();)
	{
		float dist = BASIC_SAND_STORM_DIST_F + DIST_PER_RUNNER_F * GetNRRunner();
		if((*iter)->GetPos().y > _min_y + dist)
		{
			_object_palette.EraseObject(*iter);
			delete *iter;
			iter = _cactus_deq.erase(iter);
		}
		else
			++iter;
	}
}

size_t PlayScene::GetNRRunner()
{
	size_t count = 0U;
	for(auto iter = _player_map.Begin(); iter != _player_map.End(); ++iter)
	{
		if((*iter).GetEl().alive && (*iter).GetEl().runner) ++count;
	}
	return count;
}

float PlayScene::GetXPosByTileUnitPos(int tile_unit_x)
{
	return static_cast<float>(
		OKAWARU_BOX_WIDTH * (tile_unit_x - SV_MAX_NR_PLAYER / 2) + OKAWARU_BOX_WIDTH / 2);
}