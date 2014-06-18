#include "play_scene.h"
#include "util.h"
#include "play_scene_layout.h"
#include "protocol.h"
#include "sv_module/sv_module.h"

#include <cstdio>

/*
 *		PlayScene public
 */

PlayScene::PlayScene(scene_id_t id)
	: Scene(id),
	_debug_text(L"debug", normal_font, 20U),
	_sv_name(),
	_sv_host(false),
	_player_map(),
	_my_id(),
	_me(nullptr),
	_object_palette(),
	_runner_factory(),
	_chat_box(),
	_min_y(0.f),
	_dark_bg(),
	_ready_text(L"¡ÿ∫Ò", normal_font, 20U),
	_green_zone(),
	_camera(),
	_camera_on(false),
	_tile_map(20),
	_cactus_deq(),
	_cactus_sprite(sprite_map[L"cactus"]),
	_sv_state(),
	_sandstorm(3000, &_camera),
	_object_map(),
	_hair_map(),
	_smoke_map(),
	_effect_list(),
	_inventory_box(),
	_skill_box(),
	_hp_text(L"100", normal_font, 35U),
	_hp_text_outline(L"100", normal_font, 37U),
	_all_ready(false),
	_all_ready_cnt(0U),
	_all_ready_accum_time(0)
{
	_debug_text.setPosition(400, 50);

	_dark_bg.setPosition(0.f, 0.f);
	FitSizeToScreen(&_dark_bg);

	_ready_text.setColor(Color(255, 255, 255));
	_ready_text.setOrigin(_ready_text.getLocalBounds().width / 2.f,
		_ready_text.getLocalBounds().height / 2.f);

	for(int i = 0; i < SV_MAX_NR_PLAYER; ++i)
	{
		_green_zone[i].setRadius(35.f);
		_green_zone[i].setFillColor(Color(0, 255, 0, 127));
		_green_zone[i].setPosition(
			window.getSize().x / 2.f + OKAWARU_BOX_WIDTH * (i - SV_MAX_NR_PLAYER / 2) + OKAWARU_BOX_WIDTH / 2,
			static_cast<float>(window.getSize().y - CHEIBRIADOS - OKAWARU_BOX_HEIGHT / 2) );
		_green_zone[i].setOrigin(35.f, 35.f);
	}

	_skill_box.setPosition(700.f, 250.f);

	_hp_text.setStyle(Text::Bold);
	_hp_text.setColor(Color(255, 0, 0));
	_hp_text.setPosition(30.f, 30.f);
	_hp_text_outline.setStyle(Text::Bold);
	_hp_text_outline.setColor(Color(255, 255, 255));
	_hp_text_outline.setPosition(30.f, 30.f);
}

PlayScene::~PlayScene()
{
	_runner_factory.Clear();

	for(auto iter = _cactus_deq.begin(); iter != _cactus_deq.end(); ++iter)
		delete *iter;

	for(auto iter = _object_map.Begin(); iter != _object_map.End(); ++iter)
		delete (*iter).GetEl();

	for(auto iter = _hair_map.Begin(); iter != _hair_map.End(); ++iter)
		delete (*iter).GetEl();

	for(auto iter = _smoke_map.Begin(); iter != _smoke_map.End(); ++iter)
		delete (*iter).GetEl();

	for (auto iter = _portal_map.Begin(); iter != _portal_map.End(); ++iter)
		delete (*iter).GetEl();

	for (auto iter = _effect_list.begin(); iter != _effect_list.end(); ++iter)
		delete *iter;
}

void PlayScene::Init()
{
	_player_map.Clear();

	Packet recv_packet;
	LoadPacket(&recv_packet);
	
	int header;
	recv_packet >> header;
	
	player_t	me;
	_sv_host = false;
	switch(header)
	{
	case STPS_HOSTING_MODE:
		recv_packet >> _sv_name;
		_sv_host = true;

	case STPS_JOINING_MODE:
		recv_packet >> _my_id;
		recv_packet >> me.name;
		me.color = PopColor();
	}

	_player_map.Insert(_my_id, me);
	_me = &_player_map[_my_id];

	_object_palette.Clear();

	_runner_factory.Clear();

	_chat_box.Reset(*_me);

	_min_y = 0.f;

	_dark_bg.setFillColor(Color(0, 0, 0));

	_char_desc.Reset();

	_camera.SetZero();
	_camera_on = false;
	
	_tile_map.Reset();
	for(auto iter = _cactus_deq.begin(); iter != _cactus_deq.end(); ++iter)
		delete *iter;
	_cactus_deq.clear();

	_sv_state = SV_TO_CL_WAITING_STATE;

	for(auto iter = _object_map.Begin(); iter != _object_map.End(); ++iter)
		delete (*iter).GetEl();
	_object_map.Clear();

	for(auto iter = _hair_map.Begin(); iter != _hair_map.End(); ++iter)
		delete (*iter).GetEl();
	_hair_map.Clear();

	for(auto iter = _smoke_map.Begin(); iter != _smoke_map.End(); ++iter)
		delete (*iter).GetEl();
	_smoke_map.Clear();

	for (auto iter = _portal_map.Begin(); iter != _portal_map.End(); ++iter)
		delete (*iter).GetEl();
	_portal_map.Clear();

	for (auto iter = _effect_list.begin(); iter != _effect_list.end(); ++iter)
		delete *iter;
	_effect_list.clear();

	_all_ready = false;
	_all_ready_cnt = 0U;
	_all_ready_accum_time = 0;
}

void PlayScene::Go()
{
	HandleTickTock();

	if(_chat_box.CheckEnterDone()) HandleKeyInput();
	_chat_box.Update();
	_big_msg.Update();
	_skill_box.Update();

	RecalculateMinY();
	if(_sv_state == SV_TO_CL_PLAYING_STATE ||
		_sv_state == SV_TO_CL_READY_STATE)
	{
		HandleCamera();
		_sandstorm.Update(_min_y, BASIC_SAND_STORM_DIST_F + GetNRRunner() * DIST_PER_RUNNER_F);
		HandleCactus();
		FlyBodies();
	}
	
	for(auto iter = _player_map.Begin(); iter != _player_map.End(); ++iter)
	{
		player_t & player = (*iter).GetEl();
		Runner * runner = player.runner;
		if(runner)
		{
			runner->Update();
		}
	}

	HandleEffects();
	Draw();
}
