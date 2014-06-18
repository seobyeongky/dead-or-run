#include "runner.h"
#include "res.h"
#include "util.h"
#include "coord.h"

Oh::Oh(const wchar_t * cl_name, const Texture * texture, const Color & color) :
	Runner(cl_name, texture, color),
	 _contraction_rect(180, 0, 60, 70),
	 _release_rect(240, 0, 60, 70)
{
	_sprite.setOrigin(30.f, 32.f);
	_l_running_rect = IntRect(0, 0, 60, 70);
	_r_running_rect = IntRect(60, 0, 60, 70);
	_down_rect = IntRect(120, 0, 60, 70);
}

Oh::~Oh()
{
}

runner_id_t Oh::GetId() const
{
	return OH;
}

const wchar_t * Oh::GetName() const
{
	return L"oh";
}

const wchar_t * Oh::GetFaceName() const
{
	return L"face-of-oh";
}

const wchar_t * Oh::GetDesc() const
{
	return L"::: NBA player:::\n"
			L"\"거침없는 질주를 하는 NBA player!\"";
}

void Oh::SetState(state_t new_state)
{
	switch(new_state)
	{
	case RUNNING_STATE:		_sprite.setTextureRect(_base_tex_vec + _l_running_rect); break;
	case FALL_DOWN_STATE:
		_sprite.setTextureRect(_base_tex_vec + _down_rect);
		sfx_mgr->Play(SFX_MALE_HIT);
		break;
	
	case Q_SKILL_STATE:	
		_sprite.setTextureRect(_base_tex_vec + _contraction_rect);
		sfx_mgr->Play(SFX_STIMPACK);
		break;
	case W_SKILL_STATE:	break;
	case E_SKILL_STATE:	break;
	case R_SKILL_STATE:	break;
	case DEAD_STATE:
		_sprite.setTextureRect(_base_tex_vec + _down_rect);
		sfx_mgr->Play(SFX_MALE_SCREAM);
		break;
	
	default:
		LeaveLog(L"Oh : SetState : 엉뚱한 state를 요구받았습니다. (%d)", new_state);
	}
	_state = new_state;
}

void Oh::Update()
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
	else if(_state == Q_SKILL_STATE)
	{
		if(_left)
			_sprite.setTextureRect(_base_tex_vec + _contraction_rect);
		else
			_sprite.setTextureRect(_base_tex_vec + _release_rect);
	}
}

void Oh::Yell() const
{
	sfx_mgr->Play(SFX_MALE_YELL);
}

void Oh::Hit() const
{
	sfx_mgr->Play(SFX_MALE_HIT);
}