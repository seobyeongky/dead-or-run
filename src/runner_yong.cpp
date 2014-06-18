#include "runner.h"
#include "res.h"
#include "util.h"
#include "coord.h"


#include <stdio.h>

Yong::Yong(const wchar_t * cl_name, const Texture * texture, const Color & color) :
	Runner(cl_name, texture, color),
	_skill_rect(100, 0, 72, 74),
	_ground_attack_rect(175, 0, 75, 74)
{
	SetNormalOrigin();
	_l_running_rect = IntRect(0, 0, 50, 74);
	_r_running_rect = IntRect(50, 0, 50, 74);
	_down_rect = IntRect(100, 0, 72, 74);
}

Yong::~Yong()
{
}

runner_id_t Yong::GetId() const
{
	return YONG;
}

const wchar_t * Yong::GetName() const
{
	return L"yong";
}

const wchar_t * Yong::GetFaceName() const
{
	return L"face-of-yong";
}

const wchar_t * Yong::GetDesc() const
{
	return L"::: ·ÑÁö±¤ :::\n"
			L"\"³ª¶û²²!?\"";
}

void Yong::SetState(state_t new_state)
{
	switch(new_state)
	{
	case RUNNING_STATE:
		_sprite.setTextureRect(_base_tex_vec + _l_running_rect);
		SetNormalOrigin();
		break;
	case FALL_DOWN_STATE:
		_sprite.setTextureRect(_base_tex_vec + _down_rect);
		sfx_mgr->Play(SFX_MALE_HIT);
		SetWideOrigin();
		break;
	
	case Q_SKILL_STATE:	
		_sprite.setTextureRect(_base_tex_vec + _skill_rect);
		SetWideOrigin();
		break;
	case W_SKILL_STATE:
		_sprite.setTextureRect(_base_tex_vec + _ground_attack_rect);
		SetWideOrigin();
		sfx_mgr->Play(SFX_GROUND_ATTACK_VOICE);
		break;
	case E_SKILL_STATE:	break;
	case R_SKILL_STATE:	break;
	case DEAD_STATE:
		_sprite.setTextureRect(_base_tex_vec + _down_rect);
		sfx_mgr->Play(SFX_MALE_SCREAM);
		SetWideOrigin();
		break;
	
	default:
		LeaveLog(L"Yong : SetState : ¾û¶×ÇÑ state¸¦ ¿ä±¸¹Þ¾Ò½À´Ï´Ù. (%d)", new_state);
	}
	_state = new_state;
}

void Yong::Update()
{
	if(_state == RUNNING_STATE)
	{
		_accum_time += delta_time;
		int period;
		if(_speed <= 0.1f)
			period = 10000;
		else
			period = static_cast<int>(3000 / _speed);
		if(period < _accum_time)
		{
			_accum_time -= period;
			_left = 1 - _left;
		}
	}

	if(_state == RUNNING_STATE)
	{
		if(_left)
			_sprite.setTextureRect(_base_tex_vec + _l_running_rect);
		else
			_sprite.setTextureRect(_base_tex_vec + _r_running_rect);
	}
}

void Yong::Yell() const
{
	sfx_mgr->Play(SFX_MALE_YELL);
}

void Yong::Hit() const
{
	sfx_mgr->Play(SFX_MALE_HIT);
}

void Yong::SetNormalOrigin()
{
	_sprite.setOrigin(25.f, 36.f);
}

void Yong::SetWideOrigin()
{
	_sprite.setOrigin(36.f, 36.f);
}
