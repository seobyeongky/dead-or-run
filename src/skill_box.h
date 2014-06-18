/*OOPARTS OOPARTS OOPARTS OOPARTS OOPARTS OOPARTS OOPARTS
=========================================================
	skill_box.h

	SkillBox class의 header file입니다.
=======================================================*/

#pragma once

#include "internal.h"
#include "cooldown_shadow.h"

class SkillBox : public Drawable, public Transformable
{
public:
	SkillBox();

	void Reset(skill_id_t q, skill_id_t w, skill_id_t e);
	void Update();

	void UseQSkill();
	void UseWSkill();
	void UseESkill();

	bool CheckQSkillAvailable() const;
	bool CheckWSkillAvailable() const;
	bool CheckESkillAvailable() const;
	bool CheckRSkillAvailable() const;

private:
	enum { MAX_INDEX = 3 };

	class Content : public Drawable, public Transformable
	{
	public:
		Content();
		void Reset(skill_id_t sid);
		void Use(); // force use... by absoulte logic sv
		bool CheckAvailable() const; // check cooldown time by cl
		void Update();
	private:
		virtual void draw(RenderTarget& target, RenderStates states) const;
	private:
		bool			_valid;
		Sprite			_skill;
		CooldownShadow	_shadow;
		Text			_text;
		int				_cooldown;
		int				_rest_time;
	};
	
private:
	virtual void draw(RenderTarget& target, RenderStates states) const;

private:
	Sprite _bg;
	
	Content _q_item;
	Content _w_item;
	Content _e_item;
};