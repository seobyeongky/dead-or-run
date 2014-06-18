#include "runner.h"
#include "res.h"
#include "util.h"
#include "coord.h"

Seo::Seo(const wchar_t * cl_name, const Texture * texture, const Color & color) :
	Runner(cl_name, texture, color),
	 _back_attack_rect(180, 0, 60, 60),
	 _jump_rect(240, 0, 60, 60)
{
	_sprite.setOrigin(30.f, 30.f);
	_l_running_rect = IntRect(0, 0, 60, 60);
	_r_running_rect = IntRect(60, 0, 60, 60);
	_down_rect = IntRect(120, 0, 65, 60);
}

Seo::~Seo()
{
}

runner_id_t Seo::GetId() const
{
	return SEO;
}

const wchar_t * Seo::GetName() const
{
	return L"seo";
}

const wchar_t * Seo::GetFaceName() const
{
	return L"face-of-seo";
}

const wchar_t * Seo::GetDesc() const
{
	return L"::: 트러블 메이커 :::\n"
			L"\"트러블 메이커, 그는 당신의 뒤를 노리고있다!\"";
}

void Seo::SetState(state_t new_state)
{
	switch(new_state)
	{
	case RUNNING_STATE:	_sprite.setTextureRect(_base_tex_vec + _l_running_rect); break;
	case FALL_DOWN_STATE:
		_sprite.setTextureRect(_base_tex_vec + _down_rect);
		sfx_mgr->Play(SFX_MALE_HIT);
		break;

	case Q_SKILL_STATE:
		_sprite.setTextureRect(_base_tex_vec + _back_attack_rect);
		sfx_mgr->Play(SFX_BACK_ATTACK);
		break;

	case W_SKILL_STATE:
		_sprite.setTextureRect(_base_tex_vec + _jump_rect);
		_air = true;
		sfx_mgr->Play(SFX_LEAP);
		break;
	case E_SKILL_STATE:	break;
	case R_SKILL_STATE:	break;
	case DEAD_STATE:
		_sprite.setTextureRect(_base_tex_vec + _down_rect);
		sfx_mgr->Play(SFX_MALE_SCREAM);
		break;

	default:
		LeaveLog(L"Seo : SetState : 엉뚱한 state를 요구받았습니다. (%d)", new_state);
	}
	_state = new_state;
}

void Seo::Update()
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

void Seo::Yell() const
{
	sfx_mgr->Play(SFX_MALE_YELL);
}

void Seo::Hit() const
{
	sfx_mgr->Play(SFX_MALE_HIT);
}