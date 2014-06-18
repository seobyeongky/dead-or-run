#include "runner.h"
#include "res.h"
#include "util.h"
#include "coord.h"

Park::Park(const wchar_t * cl_name, const Texture * texture, const Color & color) :
	Runner(cl_name, texture, color),
	 _bayaba_rect(210, 0, 70, 77),
	 _l_lean_rect(280, 0, 70, 77),
	 _r_lean_rect(350, 0, 70, 77)
{
	_sprite.setOrigin(35.f, 35.f);
	_l_running_rect = IntRect(0, 0, 70, 77);
	_r_running_rect = IntRect(70, 0, 70, 77);
	_down_rect = IntRect(140, 0, 70, 77);
}

Park::~Park()
{
}

runner_id_t Park::GetId() const
{
	return PARK;
}

const wchar_t * Park::GetName() const
{
	return L"park";
}

const wchar_t * Park::GetFaceName() const
{
	return L"face-of-park";
}

const wchar_t * Park::GetDesc() const
{
	return L"::: 무직의 사나이 :::\n"
			L"\"큰 덩치의 쿨가이!\"";
}

void Park::SetState(state_t new_state)
{
	switch(new_state)
	{
	case RUNNING_STATE:	_sprite.setTextureRect(_base_tex_vec + _l_running_rect); break;
	case FALL_DOWN_STATE:
		_sprite.setTextureRect(_base_tex_vec + _down_rect);
		sfx_mgr->Play(SFX_MALE_HIT);
		break;

	case Q_SKILL_STATE:	
		_sprite.setTextureRect(_base_tex_vec + _bayaba_rect);
		_air = true;
		sfx_mgr->Play(SFX_BAYABA);
		break;

	case W_SKILL_STATE:	_sprite.setTextureRect(_base_tex_vec + _l_lean_rect); break;
	case E_SKILL_STATE:	_sprite.setTextureRect(_base_tex_vec + _r_lean_rect); break;
	case R_SKILL_STATE:	break;
	case DEAD_STATE:
		_sprite.setTextureRect(_base_tex_vec + _down_rect);
		sfx_mgr->Play(SFX_MALE_SCREAM);
		break;

	default:
		LeaveLog(L"Park : SetState : 엉뚱한 state를 요구받았습니다. (%d)", new_state);
	}
	_state = new_state;
}

void Park::Update()
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

void Park::Yell() const
{
	sfx_mgr->Play(SFX_MALE_YELL);
}

void Park::Hit() const
{
	sfx_mgr->Play(SFX_MALE_HIT);
}