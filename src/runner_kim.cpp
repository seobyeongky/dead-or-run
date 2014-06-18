#include "runner.h"
#include "res.h"
#include "util.h"

runner_id_t Kim::GetId() const
{
	return KIM;
}

const wchar_t * Kim::GetName() const
{
	return L"kim";
}

const wchar_t * Kim::GetFaceName() const
{
	return L"face-of-kim";
}

const wchar_t * Kim::GetDesc() const
{
	return	L"::: 무당 :::\n";
}

void Kim::SetState(state_t new_state)
{
	_sprite.setColor(Color(255, 255, 255));
	
	switch(new_state)
	{
	case RUNNING_STATE:	_sprite.setTextureRect(_base_tex_vec + _l_running_rect); break;
	case FALL_DOWN_STATE:
		_sprite.setTextureRect(_base_tex_vec + _down_rect);
		sfx_mgr->Play(SFX_FEMALE_HIT);
		break;

	case Q_SKILL_STATE:	
		_sprite.setTextureRect(_base_tex_vec + _god_come_rect);
		_god_come_time = present_time;
		sfx_mgr->Play(SFX_GOD_COME);
		break;

	case W_SKILL_STATE:
		_sprite.setColor(Color(255, 255, 255, 128));
		sfx_mgr->Play(SFX_GHOST);
		break;

	case E_SKILL_STATE:	break;
	case DEAD_STATE:
		_sprite.setTextureRect(_base_tex_vec + _down_rect);
		break;

	default:
		LeaveLog(L"Park : SetState : 엉뚱한 state를 요구받았습니다. (%d)", new_state);
	}
	_state = new_state;
}

void Kim::Update()
{
	if(_state == RUNNING_STATE ||
		_state == Q_SKILL_STATE)
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
	else if(_state == Q_SKILL_STATE && _god_come_time + 500 < present_time)
	{
		if(_left)
			_sprite.setTextureRect(_base_tex_vec + _l_running_rect);
		else
			_sprite.setTextureRect(_base_tex_vec + _r_running_rect);
	}
}

void Kim::Yell() const
{
	sfx_mgr->Play(SFX_FEMALE_YELL);
}

void Kim::Hit() const
{
	sfx_mgr->Play(SFX_FEMALE_HIT);
}

Kim::Kim(const wchar_t * cl_name, const Texture * texture, const Color & color)
	: Runner(cl_name, texture, color),
	_god_come_rect(180, 0, 60, 65),
	_god_come_time(0)
{
	_sprite.setOrigin(30.f, 32.f);
	_l_running_rect = IntRect(0, 0, 60, 65);
	_r_running_rect = IntRect(60, 0, 60, 65);
	_down_rect = IntRect(120, 0, 60, 65);
}

Kim::~Kim()
{
}