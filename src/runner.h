/*OOPARTS OOPARTS OOPARTS OOPARTS OOPARTS OOPARTS OOPARTS
=========================================================
	runner.h

	Runner class�� header file�Դϴ�.
=======================================================*/

#pragma once

#include "internal.h"
#include "coord.h"
#include "object_interface.h"

class Runner : public IObject
{
	friend class RunnerFactory;

public:
	typedef runner_state_t state_t;

public:
	virtual runner_id_t		 GetId() const = 0;
	virtual const wchar_t *	 GetName() const = 0;
	virtual const wchar_t *	 GetFaceName() const = 0;
	virtual const wchar_t *	 GetDesc() const = 0;
	
	void					 ResetTexture(const Texture * texture);
	void					 ResetTextureBase(const Vector2i & base);

	const pos_t	&			 GetPos() const { return _pos; }
	float					 GetDir() const { return _dir; }
	float					 GetSpriteY() const { return _sprite.getPosition().y; }
	priority_t				 GetPriority() const;
	state_t					 GetState() const { return _state; }

	void					 SetPos(float x, float y);
	void					 SetDir(float dir) { _dir = dir; _sprite.setRotation(ToDegree(_dir)); }
	void					 SetSpeed(float speed) { _speed = speed; }
	
	void					 OnSetState(state_t new_state);
	virtual void			 Update() = 0;
	void					 Draw(RenderTarget & target);

	virtual void			 Yell() const = 0;
	virtual void			 Hit() const = 0;

	// �� �����Ӹ��� ȣ���ϴ� ��ġ ���� �Լ�
	virtual void			 CorrectPos();

protected:
	state_t			_state;
	
	Sprite			_sprite;
	Vector2i		_base_tex_vec;

	IntRect			_l_running_rect;
	IntRect			_r_running_rect;
	IntRect			_down_rect;

	Text			_text;
	
	pos_t			_pos;	// server�κ��� ������� ����ȭ�� ��¥ ��ġ
	float			_dir;
	float			_speed;

	int				_accum_time;
	int				_left;			//�޹��� ����ִ��� �����մϴ�.
	
	bool			_air;	// ���߿� ������

	virtual void			 SetState(state_t new_state) = 0;
	
							 Runner(const wchar_t * cl_name, const Texture * texture, const Color & color);
	virtual					~Runner();

private:
	// ��ġ ������ �ð� POSITION SET �ð�
	int				_pos_set_time;

				Runner(const Runner & other);		// ���� ������ ����
	Runner &	operator = (const Runner & rhs);	// ���� ������ ����
};

class Seo : public Runner
{
	friend class RunnerFactory;

public:
	runner_id_t			 GetId() const;
	const wchar_t *		 GetName() const;
	const wchar_t *		 GetFaceName() const;
	const wchar_t *		 GetDesc() const;
	
	void				 SetState(state_t new_state);
	void				 Update();

	void				 Yell() const;
	void				 Hit() const;

private:
	IntRect			_back_attack_rect;
	IntRect			_jump_rect;

	 Seo(const wchar_t * cl_name, const Texture * texture, const Color & color);
	~Seo();
};

class Park : public Runner
{
	friend class RunnerFactory;

public:
	runner_id_t			 GetId() const;
	const wchar_t *		 GetName() const;
	const wchar_t *		 GetFaceName() const;
	const wchar_t *		 GetDesc() const;
	
	void				 SetState(state_t new_state);
	void				 Update();

	void				 Yell() const;
	void				 Hit() const;

private:
	IntRect			_bayaba_rect;
	IntRect			_l_lean_rect;
	IntRect			_r_lean_rect;

	 Park(const wchar_t * cl_name, const Texture * texture, const Color & color);
	~Park();
};

class Oh : public Runner
{
	friend class RunnerFactory;

public:
	runner_id_t			 GetId() const;
	const wchar_t *		 GetName() const;
	const wchar_t *		 GetFaceName() const;
	const wchar_t *		 GetDesc() const;
	
	void				 SetState(state_t new_state);
	void				 Update();

	void				 Yell() const;
	void				 Hit() const;

private:
	IntRect			_contraction_rect;	//��ģ�� ��ų
	IntRect			_release_rect;		//��ģ�� ��ų
	
	 Oh(const wchar_t * cl_name, const Texture * texture, const Color & color);
	~Oh();
};

class Jeong : public Runner
{
	friend class RunnerFactory;

public:
	runner_id_t			 GetId() const;
	const wchar_t *		 GetName() const;
	const wchar_t *		 GetFaceName() const;
	const wchar_t *		 GetDesc() const;

	void				 SetState(state_t new_state);
	void				 Update();

	void				 Yell() const;
	void				 Hit() const;

private:
	 Jeong(const wchar_t * cl_name, const Texture * texture, const Color & color);
	~Jeong();

	IntRect			_grudge_rect;
	IntRect			_flash_rect;
};

class Kim : public Runner
{
	friend class RunnerFactory;

public:
	runner_id_t			GetId() const;
	const wchar_t *		GetName() const;
	const wchar_t *		GetFaceName() const;
	const wchar_t *		GetDesc() const;

	void				SetState(state_t new_state);
	void				Update();

	void				Yell() const;
	void				Hit() const;

private:
	 Kim(const wchar_t * cl_name, const Texture * texture, const Color & color);
	~Kim();

	IntRect			_god_come_rect;
	int				_god_come_time;
};

class Yong : public Runner
{
	friend class RunnerFactory;

public:
	runner_id_t			GetId() const;
	const wchar_t *		GetName() const;
	const wchar_t *		GetFaceName() const;
	const wchar_t *		GetDesc() const;

	void				SetState(state_t new_state);
	void				Update();

	void				Yell() const;
	void				Hit() const;

private:
						Yong(const wchar_t * cl_name, const Texture * texture, const Color & color);
						~Yong();

	IntRect				_skill_rect;
	IntRect				_ground_attack_rect;

	void				SetNormalOrigin();
	void				SetWideOrigin();
};

class Yoon : public Runner
{
	friend class RunnerFactory;

public:
	runner_id_t			 GetId() const;
	const wchar_t *		 GetName() const;
	const wchar_t *		 GetFaceName() const;
	const wchar_t *		 GetDesc() const;
	
	void				 SetState(state_t new_state);
	void				 Update();

	void				 Yell() const;
	void				 Hit() const;

private:
	IntRect				_magic_booger_rect;
	IntRect				_provoking_rect;

	 Yoon(const wchar_t * cl_name, const Texture * texture, const Color & color);
	~Yoon();

	void				SetRunningState();
	void				SetStandingState();
	void				SetBoogerState();
	void				SetProvokingState();
	void				SetFalldownState();
};

class RandomRunner : public Runner
{
	friend class RunnerFactory;

public:
	runner_id_t			GetId() const;
	const wchar_t *		GetName() const;
	const wchar_t *		GetFaceName() const;
	const wchar_t *		GetDesc() const;

	void				SetState(state_t new_state);
	void				Update();

	void				Yell() const;
	void				Hit() const;

private:
	 RandomRunner(const wchar_t * cl_name, const Texture * texture, const Color & color);
	~RandomRunner();
};