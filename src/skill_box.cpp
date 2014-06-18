#include "skill_box.h"
#include "res.h"

SkillBox::Content::Content()
	: _valid(false), _skill(),
	_shadow(Color(0, 20, 200, 150)),
	_text(), _cooldown(0), _rest_time(0)
{	
}

void SkillBox::Content::Reset(skill_id_t sid)
{
	if (sid != INVALID_SKILL)
	{
		_valid = true;
		_skill = *sprite_map[SKILL(sid)->icon];
		_text.setFont(normal_font);
		_text.setColor(Color::White);
		_text.setPosition(Vector2f(25.f, 25.f));
		_cooldown = SKILL(sid)->cooldown * 1000;
		_rest_time = 0;
	}
	else
	{
		_valid = false;
	}
}

void SkillBox::Content::Use()
{
	if (!_valid) return;

	_rest_time = _cooldown;
	_shadow.SetRatio(0.f);
}

bool SkillBox::Content::CheckAvailable() const
{
	if (!_valid) return false;
	if (_rest_time > 0) return false;

	return true;
}

void SkillBox::Content::Update()
{
	if (!_valid) return;

	_rest_time -= delta_time;
	if (_rest_time < 0)
	{
		_rest_time = 0;
	}
	else
	{
		_shadow.SetRatio(1.f - static_cast<float>(_rest_time) / _cooldown);
		_text.setString(to_wstring(_rest_time/1000));
		auto bounds = _text.getLocalBounds();
		_text.setOrigin(bounds.width / 2.f, bounds.height / 2.f);
	}
}

void SkillBox::Content::draw(RenderTarget& target, RenderStates states) const
{
	if (!_valid) return;

	states.transform *= getTransform();
	
	target.draw(_skill, states.transform);
	if (_rest_time  > 0)
	{
		target.draw(_shadow, states.transform);
		target.draw(_text, states.transform);
	}
}

SkillBox::SkillBox()
	: _bg(*sprite_map[L"skill-box"]),
	_q_item(), _w_item(), _e_item()
{
	float begin_x = 20.f;
	float begin_y = 15.f;
	float y_interval = 59.f;
	
	_q_item.setPosition(begin_x, begin_y);
	_w_item.setPosition(begin_x, begin_y + y_interval);
	_e_item.setPosition(begin_x, begin_y + 2.f * y_interval);
}

void SkillBox::Reset(skill_id_t q, skill_id_t w, skill_id_t e)
{
	_q_item.Reset(q);
	_w_item.Reset(w);
	_e_item.Reset(e);
}

void SkillBox::UseQSkill()
{
	_q_item.Use();
}

void SkillBox::UseWSkill()
{
	_w_item.Use();
}

void SkillBox::UseESkill()
{
	_e_item.Use();
}

bool SkillBox::CheckQSkillAvailable() const
{
	return _q_item.CheckAvailable();
}

bool SkillBox::CheckWSkillAvailable() const
{
	return _w_item.CheckAvailable();
}

bool SkillBox::CheckESkillAvailable() const
{
	return _e_item.CheckAvailable();
}

void SkillBox::Update()
{
	_q_item.Update();
	_w_item.Update();
	_e_item.Update();
}

void SkillBox::draw(RenderTarget &target, RenderStates states) const
{
	target.draw(_bg, getTransform());

	target.draw(_q_item, getTransform());
	target.draw(_w_item, getTransform());
	target.draw(_e_item, getTransform());
}