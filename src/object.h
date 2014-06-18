/*OOPARTS OOPARTS OOPARTS OOPARTS OOPARTS OOPARTS OOPARTS
=========================================================
	object.h

	여러 object들입니다.
=======================================================*/

#pragma once

#include "internal.h"
#include "object_interface.h"
#include "sfx_mgr.h"

class Cactus : public IObject
{
public:
					Cactus() : _sprite(), _pos() { }
					Cactus(const Sprite * sprite, const pos_t & pos)
						: _sprite(*sprite), _pos(pos)
	{
		_sprite.setOrigin(16.f, 40.f);
		_sprite.setPosition(_pos);
	}

	const pos_t &	GetPos() const { return _pos; }
	void			Draw(RenderTarget & target) { target.draw(_sprite); }
	priority_t		GetPriority() const { return GROUND_PRIORITY; }
	void			CorrectPos() { }

private:
	Sprite		_sprite;
	pos_t		_pos;
};

class Item : public IObject
{
public:
	static const Sprite *	GetSprite(item_id_t item_id);
	static sfx_t			GetSFX(item_id_t item_id);
//	static void GetName(wstring * str_ptr);

public:
					
					Item() : _sprite(), _pos() { }
					Item(item_id_t item_id, const pos_t & pos);
	
	const pos_t &	GetPos() const { return _pos; }
	void			Draw(RenderTarget & target) { target.draw(_sprite); }
	priority_t		GetPriority() const { return GROUND_PRIORITY; }
	void			CorrectPos() { }

private:
	Sprite		_sprite;
	pos_t		_pos;
};

class Stone : public IObject
{
public:
					Stone(const pos_t & pos,
						const vector_t & vcity);


	void			SetPos(const pos_t & pos);
	const pos_t &	GetPos() const { return _pos; }

	void			Draw(RenderTarget & target) { target.draw(_sprite);}
	priority_t		GetPriority() const { return AIR_PRIORITY; }
	
	void			CorrectPos();

private:
	void	ResetPos(const pos_t & pos);

private:
	Sprite			_sprite;
	pos_t			_set_pos;
	pos_t			_pos;
	vector_t		_vcity;
	int				_set_time;
};

class Runner;

class Hair : public IObject
{
public:
					Hair();

	Runner *		GetOwner() { return _owner; }
	void			SetOwner(Runner * owner);
	void			SetPos(const pos_t & pos);
	void			SetVcity(const vector_t & vcity) { _vcity = vcity; }
	float			GetLength() const;
	void			SetFirstPos(const pos_t & fpos);

	const pos_t &	GetPos() const { return _pos; }
	void			Draw(RenderTarget & target) { target.draw(_sprite);}
	priority_t		GetPriority() const { return AIR_PRIORITY; }
	void			CorrectPos();

private:
	void			ResetPos(const pos_t & pos);

private:
	Sprite		_sprite;
	Runner *	_owner;
	pos_t		_set_pos;
	pos_t		_pos;
	vector_t	_vcity;
	int			_set_time;
	pos_t		_first_pos;
};

class Portal : public IObject
{
public:
					Portal(const pos_t & pos, float dir);

	const pos_t &	GetPos() const { return _pos; }
	void			Draw(RenderTarget & target);
	priority_t		GetPriority() const { return AIR_PRIORITY; }
	void			CorrectPos();

private:
	vector<IntRect>	_rects;
	Sprite			_sprite;
	int				_accum_time;
	size_t			_index;
	pos_t			_pos;

	void			Update();
};

class MagicBooger : public IObject
{
public:
					MagicBooger(const pos_t & pos, const vector_t & vcity);

	void			SetPos(const pos_t & pos);
	const pos_t &	GetPos() const { return _pos; }

	void			Draw(RenderTarget & target) { target.draw(_sprite);}
	priority_t		GetPriority() const { return AIR_PRIORITY; }
	
	void			CorrectPos();

private:
	void	ResetPos(const pos_t & pos);

private:
	Sprite			_sprite;
	pos_t			_set_pos;
	pos_t			_pos;
	vector_t		_vcity;
	int				_set_time;
};

class IEffect : public IObject
{
public:
	virtual int GetDuration() const = 0;
	virtual int	GetBeginTime() const  = 0;
};

class GroundAttack : public IEffect
{
public:
	enum {
		DURATION = 300
	};

public:
					GroundAttack(const pos_t & pos);

	const pos_t &	GetPos() const { return _pos; }
	void			Draw(RenderTarget & target);
	priority_t		GetPriority() const { return FALL_DOWN_PRIORITY; }
	void			CorrectPos();
	int				GetDuration() const { return DURATION; }
	int				GetBeginTime() const { return _begin_time; }

private:
	pos_t			_pos;
	int				_begin_time;
	Sprite			_sprite;
};