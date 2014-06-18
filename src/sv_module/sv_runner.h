/*OOPARTS OOPARTS OOPARTS OOPARTS OOPARTS OOPARTS OOPARTS
=========================================================
	sv_runner.h

	server������ �����ϴ� runner ��ü�� header file�Դϴ�.
=======================================================*/

#pragma once

#include "../coord.h"
#include "../protocol.h"
#include "sv_object.h"

class SvRunner {
/*	�׷��ϴ�*/
	friend class PhysicalEngine;

public:
	typedef runner_state_t	state_t;

public:
						 SvRunner(runner_id_t id,
								float radius);
	virtual				~SvRunner() { }

	// EVENT
	void				 BeginToRun();
	void				 FallDown(int duration);
	void				 UsingQSkill(int duration);
	void				 UsingWSkill(int duration);
	void				 UsingESkill(int duration);
	void				 UsingRSkill(int duration);

	// FLAG
	void				 SetBayabaFlag() { _bayaba_flag = true; }
	bool				 GetLeapFlag() const { return _leap_flag; }
	void				 SetLeapFlag() { _leap_flag = true; }
	bool				 GetWaterFlag() { return _water_flag; }
	void				 SetWaterFlag(bool b) { _water_flag = b; }
	bool				 GetHairFlag() const { return _hair_flag; }
	void				 SetHairFlag(bool b) { _hair_flag = b; }
	bool				 GetFlashFlag() const { return _flash_flag; }
	float				 GetFlashDist() const { return _flash_dist; }
	void				 SetFlashFlag(bool b, float dist) { _flash_flag = b; _flash_dist = dist;}
	bool				 GetGhostFlag() const { return _ghost_flag; }
	void				 SetGhostFlag(bool b) { _ghost_flag = b; }
	int					 GetNrSpeedBuf() { return _nr_speed_buf; }
	void				 SetNrSpeedBuf(int n) { _nr_speed_buf = n; }

	// iTEM
	bool				 CheckInventoryFull() const;
	bool				 AddItem(item_id_t item_id);
	bool				 SpendItem(item_id_t item_id);

	// KEY EVENT
	void				 LeftRotating(bool b) { _left_rotating = b; }
	void				 RightRotating(bool b) { _right_rotating = b; }
	void				 Acceling(bool b) { _acceling = b; }
	void				 Breaking(bool b) { _breaking = b; }
	
	// UPDATE
	void				 Update();

	// GET SERIES
	runner_id_t			 GetId() const { return _id; }
	const pos_t	&		 GetPos() const { return _pos; }
	float				 GetRadius() const { return _radius; }
	float				 GetDir() const { return _dir; }
	float				 GetSpeed() const { return _speed; }
	float				 GetHealth() const { return _health; }
	state_t				 GetState() const { return _state; }
	skill_id_t		 GetQSkill() const { return _q_skill; }
	skill_id_t		 GetWSkill() const { return _w_skill; }
	skill_id_t		 GetESkill() const { return _e_skill; }
	skill_id_t		 GetRSkill() const { return _r_skill; }

	bool				 GetBayabaFlag() const { return _bayaba_flag; }

	// SET SERIES
	void				 SetPos(pos_t new_pos) { _pos = new_pos; }
	void				 SetSpeed(float new_speed) { _speed = new_speed; }
	void				 SetHealth(float new_health) { _health = new_health; }
	void				 ChangeHealth(float d);

private:
	runner_id_t			_id;
	pos_t				_pos;
	float				_radius;
	float				_dir;
	bool				_left_rotating;
	bool				_right_rotating;
	float				_speed;
	bool				_acceling;
	bool				_breaking;
	float				_health;
	state_t				_state;
	skill_id_t		_q_skill;
	skill_id_t		_w_skill;
	skill_id_t		_e_skill;
	skill_id_t		_r_skill;

	int					_event_begin_time;
	int					_event_duration;

	// �پ߹� ��ų ���� ������
	bool				_bayaba_flag;
	// ���� ��ų ������
	bool				_leap_flag;
	// ���� ��������
	bool				_water_flag;
	// �Ӹ�ī���� ��������
	bool				_hair_flag;
	// ���� �� ���� ������
	float				_flash_dist;
	bool				_flash_flag;
	// ��üȭ ������
	bool				_ghost_flag;
	// ���ǵ� ������ ����... (���� ���� ���� ���� �ִ�)
	int					_nr_speed_buf;

	vector<item_id_t>	_item_vec;

	// ���� speed�� dependant�ϰ� ü���� �����մϴ�.
	void			 UpdateHealth();
	
	void			 HandleSpeed(float basic_speed);
	void			 HandleRotation(float basic_speed);
};

typedef SMap<ID, SvRunner *> runner_map_t;