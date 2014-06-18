#include "runner.h"
#include "speed_def.h"
#include "res.h"
#include "runner_factory.h"

Runner::Runner(const wchar_t * cl_name, const Texture * texture, const Color & color)
	: _state(INVALID_STATE),
	_sprite(*texture),
	_base_tex_vec(),
	_l_running_rect(),
	_r_running_rect(),
	_down_rect(),
	_text(cl_name, normal_font, 15U),
	_pos(),
	_dir(0.f),
	_speed(0),
	_accum_time(0),
	_left(true),
	_air(false)
{
	_text.setColor(Color(255, 255, 255));
	_text.setOrigin(_text.getLocalBounds().width / 2.f, 0.f);
}

Runner::~Runner()
{
}

void Runner::ResetTexture(const Texture * texture)
{
	_sprite.setTexture(*texture, false);
}

void Runner::ResetTextureBase(const Vector2i & base)
{
	IntRect rect = _sprite.getTextureRect();
	rect.left += base.x - _base_tex_vec.x;
	rect.top += base.y - _base_tex_vec.y;
	_sprite.setTextureRect(rect);

	_base_tex_vec = base;
}

IObject::priority_t Runner::GetPriority() const 
{
	switch(_state)
	{
	case RUNNING_STATE: return GROUND_PRIORITY;
	case FALL_DOWN_STATE: return FALL_DOWN_PRIORITY;
	case Q_SKILL_STATE:
	case W_SKILL_STATE:
	case E_SKILL_STATE:
	case R_SKILL_STATE:
		return _air ? AIR_PRIORITY : GROUND_PRIORITY;
	default:
		return GROUND_PRIORITY;
	}
}

void Runner::OnSetState(state_t new_state)
{
	if(_state != new_state)
	{
		_air = false;
		SetState(new_state);
	}
}

void Runner::Draw(RenderTarget & target)
{
	target.draw(_sprite);
	target.draw(_text);
}

void Runner::SetPos(float x, float y)
{
	_sprite.setPosition(x, y);
	_pos.x = x; _pos.y = y;
	_pos_set_time = present_time;

	_text.setPosition(x, y - 45.f);
}

void Runner::CorrectPos()
{
	Vector2f pos = _pos + GetUnitVector(_dir) *	// 방향 벡터
		(_speed * (present_time - _pos_set_time)) / MSPPIXEL_F;	// 실제 변위
	_sprite.setPosition(pos);
	_text.setPosition(pos.x, pos.y - 45.f);
}