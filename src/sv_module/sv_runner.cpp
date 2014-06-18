#include "sv_runner.h"
#include "sv_res.h"
#include "../util.h"

#define BASIC_SPEED 30.f

SvRunner::SvRunner(	runner_id_t id,
					float radius) :
	_id(id),
	_pos(), _radius(radius),
	_dir(0.f), _left_rotating(false), _right_rotating(false),
	_speed(0), _acceling(false), _breaking(false),
	_health(50),
	_state(RUNNING_STATE),
	_q_skill(CHARACTER(id).q_skill), _w_skill(CHARACTER(id).w_skill),
	_e_skill(CHARACTER(id).e_skill), _r_skill(CHARACTER(id).r_skill),
	_event_begin_time(0), _event_duration(0),
	_bayaba_flag(false),
	_leap_flag(false),
	_water_flag(false),
	_hair_flag(false),
	_flash_dist(0.f),
	_flash_flag(false),
	_ghost_flag(false),
	_nr_speed_buf(0),
	_item_vec()
{
	_item_vec.reserve(MAX_NR_ITEM_PER_RUNNER);
}

void SvRunner::BeginToRun()
{
	_state = RUNNING_STATE;
	_bayaba_flag = false;
	_hair_flag = false;
	_flash_flag = false;
}

void SvRunner::FallDown(int duration)
{
	_speed = 0.f;
	_state = FALL_DOWN_STATE;
	_event_begin_time = sv_present_time;
	_event_duration = duration;
	_bayaba_flag = false;
	_leap_flag = false;
	_flash_flag = false;
	_ghost_flag = false;
}

void SvRunner::UsingQSkill(int duration)
{
	_state = Q_SKILL_STATE;
	_event_begin_time = sv_present_time;
	_event_duration = duration;
}

void SvRunner::UsingWSkill(int duration)
{
	_state = W_SKILL_STATE;
	_event_begin_time = sv_present_time;
	_event_duration = duration;
}

void SvRunner::UsingESkill(int duration)
{
	_state = E_SKILL_STATE;
	_event_begin_time = sv_present_time;
	_event_duration = duration;
}

void SvRunner::UsingRSkill(int duration)
{
	_state = R_SKILL_STATE;
	_event_begin_time = sv_present_time;
	_event_duration = duration;
}

bool SvRunner::CheckInventoryFull() const
{
	if(_item_vec.size() < MAX_NR_ITEM_PER_RUNNER) return false;
	else return true;
}

bool SvRunner::AddItem(item_id_t item_id)
{
	if(_item_vec.size() >= MAX_NR_ITEM_PER_RUNNER) return false;
	_item_vec.push_back(item_id);
	return true;
}

bool SvRunner::SpendItem(item_id_t item_id)
{
	for(auto it = _item_vec.begin(); it != _item_vec.end(); ++it)
	{
		if(*it == item_id)
		{
			_item_vec.erase(it);
			return true;
		}
	}
	return false;
}

void SvRunner::Update()
{
	if(_flash_flag) { _flash_flag = false; _speed = 30.f; }

	if(_state == RUNNING_STATE)
	{
		HandleSpeed(BASIC_SPEED);

		HandleRotation(0.1f);

		UpdateHealth();
	}
	else if(_state == FALL_DOWN_STATE)
	{
		if(_health < 5.f) _health += sv_delta_time / 40.f;

		if(_event_begin_time + _event_duration < sv_present_time)
		{
			BeginToRun();
		}
	}
	else if(_state == Q_SKILL_STATE ||
		_state == W_SKILL_STATE ||
		_state == E_SKILL_STATE ||
		_state == R_SKILL_STATE)
	{
		if(_event_begin_time + _event_duration < sv_present_time)
		{
			BeginToRun();
			_leap_flag = false;
			_ghost_flag = false;
		}
		else
		{
			if(_state == Q_SKILL_STATE && _q_skill == GOD_COME_SKILL)
			{
				HandleSpeed(BASIC_SPEED + 7.f);
				HandleRotation(0.15f);

//				static int logcount = 0;
//				logcount++;
//				if (logcount % 10 == 0) sfx_mgr->Play(SFX_HEADSHOT);
			}
			else if(_state == W_SKILL_STATE && _w_skill == GHOST_SKILL)
			{
				HandleSpeed(BASIC_SPEED);
				HandleRotation(0.1f);
			}
		}
	}
}

void SvRunner::UpdateHealth()
{
	if(_state == RUNNING_STATE)
	{
		_health += (BASIC_SPEED - _speed) * sv_delta_time / 1000.f;
		if(_health < 0.f) FallDown(200);
		if(_health > 100.f) _health = 100.f;
	}
}

void SvRunner::ChangeHealth(float d)
{
	_health += d;
	if(_health > 100.f)
		_health = 100.f;
	if(_health < -20.f)
		_health = -20.f;
}

void SvRunner::HandleSpeed(float basic_speed)
{	
	if(_acceling && _health > 5.f)
		_speed = basic_speed + 10.f;
	else if(_breaking)
		_speed = basic_speed - 10.f;
	else
		_speed = basic_speed;

	if (_nr_speed_buf)
		_speed += _nr_speed_buf / 10;

	if (_nr_speed_buf > 0) _nr_speed_buf--;
	if (_nr_speed_buf < 0) _nr_speed_buf++;

	if(_speed < 0.f) _speed = 0.f;
}

void SvRunner::HandleRotation(float basic_speed)
{
	if(_left_rotating)
		_dir -= basic_speed * sv_delta_time / 100.f;
	if(_right_rotating)
		_dir += basic_speed * sv_delta_time / 100.f;
}