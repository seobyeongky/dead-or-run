#include "runner.h"
#include "res.h"
#include "util.h"
#include "coord.h"

Jeong::Jeong(const wchar_t * cl_name, const Texture * texture, const Color & color) :
	Runner(cl_name, texture, color),
	_grudge_rect(225, 0, 75, 80),
	_flash_rect(300, 0, 75, 80)
{
	_sprite.setOrigin(30.f, 32.f);
	_l_running_rect = IntRect(0, 0, 75, 80);
	_r_running_rect = IntRect(75, 0, 75, 80);
	_down_rect = IntRect(150, 0, 75, 80);
}

Jeong::~Jeong()
{
}

runner_id_t Jeong::GetId() const
{
	return JEONG;
}

const wchar_t * Jeong::GetName() const
{
	return L"jeong";
}

const wchar_t * Jeong::GetFaceName() const
{
	return L"face-of-jeong";
}

const wchar_t * Jeong::GetDesc() const
{
	return L"::: 머리귀신 :::\n"
			L"\"정체불명의 머리귀신을 조심하라!\"";
}

void Jeong::SetState(state_t new_state)
{
	switch(new_state)
	{
	case RUNNING_STATE:		_sprite.setTextureRect(_base_tex_vec + _l_running_rect); break;
	case FALL_DOWN_STATE:
		_sprite.setTextureRect(_base_tex_vec + _down_rect);
		sfx_mgr->Play(SFX_MALE_HIT);
		break;
	case Q_SKILL_STATE:
		_sprite.setTextureRect(_base_tex_vec + _grudge_rect);
		break;

	case W_SKILL_STATE:
		_sprite.setTextureRect(_base_tex_vec + _flash_rect);
		sfx_mgr->Play(SFX_FLASH);
		break;

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

void Jeong::Update()
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

void Jeong::Yell() const
{
	sfx_mgr->Play(SFX_MALE_YELL);
}

void Jeong::Hit() const
{
	sfx_mgr->Play(SFX_MALE_HIT);
}