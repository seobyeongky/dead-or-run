#include "runner.h"
#include "res.h"
#include "util.h"
#include "coord.h"

Yoon::Yoon(const wchar_t * cl_name, const Texture * texture, const Color & color) :
	Runner(cl_name, texture, color),
	_magic_booger_rect(240, 0, 80, 80),
	_provoking_rect(320, 0, 80, 80)
{
	_l_running_rect = IntRect(80, 0, 80, 80);
	_r_running_rect = IntRect(160, 0, 80, 80);
	_down_rect = IntRect(0, 0, 80, 80);

	SetStandingState();
}

Yoon::~Yoon()
{
}

runner_id_t Yoon::GetId() const
{
	return YOON;
}

const wchar_t * Yoon::GetName() const
{
	return L"yoon";
}

const wchar_t * Yoon::GetFaceName() const
{
	return L"face-of-yoon";
}

const wchar_t * Yoon::GetDesc() const
{
	return L"::: 장호리얼 :::\n"
			L"\"신비한 요술을 부리는 탐험가\"";
}

void Yoon::SetState(state_t new_state)
{
	switch(new_state)
	{
	case RUNNING_STATE: SetRunningState(); break;
	case FALL_DOWN_STATE: SetFalldownState(); break;
	case Q_SKILL_STATE: SetBoogerState(); break;
	case W_SKILL_STATE: SetProvokingState(); break;
	case E_SKILL_STATE:	break;
	case R_SKILL_STATE:	break;
	case DEAD_STATE:
		_sprite.setTextureRect(_base_tex_vec + _down_rect);
		sfx_mgr->Play(SFX_MALE_SCREAM);
		break;

	default:
		LeaveLog(L"Yoon : SetState : 엉뚱한 state를 요구받았습니다. (%d)", new_state);
	}
	_state = new_state;
}

void Yoon::Update()
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
			if(_left)
				_sprite.setTextureRect(_base_tex_vec + _l_running_rect);
			else
				_sprite.setTextureRect(_base_tex_vec + _r_running_rect);
		}
	}
}

void Yoon::Yell() const
{
	sfx_mgr->Play(SFX_MALE_YELL);
}

void Yoon::Hit() const
{
	sfx_mgr->Play(SFX_MALE_HIT);
}

void Yoon::SetRunningState()
{
	_sprite.setTextureRect(_base_tex_vec + _l_running_rect);
	_sprite.setOrigin(40.f, 42.f);
	_air = false;
}

void Yoon::SetStandingState()
{
	_sprite.setTextureRect(_base_tex_vec + _down_rect);
	_sprite.setOrigin(40.f, 42.f);
	_air = false;
}

void Yoon::SetBoogerState()
{
	_sprite.setTextureRect(_base_tex_vec + _magic_booger_rect);
	_sprite.setOrigin(40.f, 42.f);
	_air = false;
}

void Yoon::SetProvokingState()
{
	_sprite.setTextureRect(_base_tex_vec + _provoking_rect);
	_sprite.setOrigin(40.f, 66.f);
	_air = true;
	sfx_mgr->Play(SFX_PROVOKATION);
}

void Yoon::SetFalldownState()
{
	_sprite.setTextureRect(_base_tex_vec + _down_rect);
	_sprite.setOrigin(40.f, 42.f);
	_air = false;
	sfx_mgr->Play(SFX_MALE_HIT);
}