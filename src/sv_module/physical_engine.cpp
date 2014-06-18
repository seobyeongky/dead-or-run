#include "physical_engine.h"
#include "../speed_def.h"
#include "../play_scene_layout.h"
#include "../util.h"

/***********************
		public
***********************/

PhysicalEngine::PhysicalEngine(ID * object_id_cnt_ptr)
	: _object_id_cnt_ptr(object_id_cnt_ptr), 
	_runners(), _cactus_list(), _stone_list(), _hair_list(),
	_portal_list(), _booger_list()
{
}

PhysicalEngine::~PhysicalEngine()
{
}

void PhysicalEngine::Reset(	runner_map_t * runner_map_ptr,
							list<sv_cactus_t> * cactus_list,
							list<sv_item_t> * item_list,
							list<sv_stone_t> * stone_list,
							list<sv_hair_t> * hair_list,
							list<sv_portal_t> * portal_list,
							list<sv_booger_t> * booger_list)
{
	_runners.Clear();
	for(auto iter = runner_map_ptr->Begin();
		iter != runner_map_ptr->End();
		++iter)
	{
		_runners.Insert((*iter).GetKey(),
						taged_runner_t((*iter).GetKey(), true, (*iter).GetEl()) );
	}

	_cactus_list = cactus_list;
	_item_list = item_list;
	_stone_list = stone_list;
	_hair_list = hair_list;
	_portal_list = portal_list;
	_booger_list = booger_list;
}

void PhysicalEngine::EraseRunner(ID cl_id)
{
	_runners.Erase(cl_id);
}

void PhysicalEngine::Update()
{
	for(auto iter = _runners.Begin(); iter != _runners.End(); ++iter)
	{
		taged_runner_t * trunner = &(*iter).GetEl();
		trunner->rest_time = 1.f;
		auto runner = trunner->runner;
		if (runner->GetFlashFlag())
			trunner->rvcity = runner->GetFlashDist() * GetUnitVector(runner->GetDir());
		else
			trunner->rvcity = GetVcity(runner, sv_delta_time);
	}

	_cur_time = 0.f;

	int cnt = 0;

	col_t c;
	while(PredictFirstCol(&c))
	{
		switch(c.type)
		{
		case RUNNER_COL: HandleRunnerCol(c); break;
		case LEFT_BOUNDARY_COL: HandleLeftBoundaryCol(c); break;
		case RIGHT_BOUNDARY_COL: HandleRightBoundaryCol(c); break;
		case CACTUS_COL: HandleCactusCol(c); break;
		case ITEM_COL: HandleItemCol(c); break;
		case STONE_COL: HandleStoneCol(c); break;
		case HAIR_COL: HandleHairCol(c); break;
		case PORTAL_COL_WITH_RUNNER: HandlePortalColWithRunner(c); break;
		case PORTAL_COL_WITH_HAIR: HandlePortalColWithHair(c); break;
		case BOOGER_COL: HandleBoogerCol(c); break;
		}

		_cur_time = c.time;

		// _cur_time 기준으로 동기화
		for(auto iter = _runners.Begin(); iter != _runners.End(); ++iter)
		{
			SyncToCurTime(&(*iter).GetEl());
		}

		if(++cnt > 8) break;
	}

	for(auto iter = _runners.Begin(); iter != _runners.End(); ++iter)
	{
		taged_runner_t * trunner = &(*iter).GetEl();
		trunner->runner->_pos += trunner->rest_time * trunner->rvcity;
	}

	for(auto iter = _hair_list->begin(); iter != _hair_list->end(); ++iter)
		iter->pos += iter->vcity * (sv_delta_time / MSPPIXEL_F);
	
	for(auto it = _stone_list->begin(); it != _stone_list->end(); ++it)
		it->pos += it->vcity * (sv_delta_time / MSPPIXEL_F);

	for (auto & booger : *_booger_list)
		booger.pos += booger.vcity * (sv_delta_time / MSPPIXEL_F);
}

/*
SvRunner * PhysicalEngine::GetRunnerInsideOfCircle(const vector_t & origin, float radius)
{
}
*/

void PhysicalEngine::SetRunnerAir(SvRunner * runner)
{

}

void PhysicalEngine::SetRunnerGround(SvRunner * runner)
{

}

/***********************
		private
***********************/

///////// COLLISION HANDLERS ////////////

void PhysicalEngine::HandleRunnerCol(const col_t & c)
{
	vector_t v = c.trunner1->rvcity;
	vector_t V = c.trunner2->rvcity;
	float m = c.trunner1->runner->GetRadius();
	float M = c.trunner2->runner->GetRadius();

	// wiki 백과 공식입니다
	vector_t v2 = (v * (m - M) + V * 2.0f * M) / (m + M);
	vector_t V2 = (V * (M - m) + v * 2.0f * m) / (m + M);

	// 각 캐릭터의 바야바상태에 따라서 논리가 분기됩니다.
	if(c.trunner1->runner->GetBayabaFlag() &&
		c.trunner2->runner->GetBayabaFlag())
	{
		v2 *= 2.0f;
		V2 *= 2.0f;
		c.trunner1->runner->FallDown(500);
		c.trunner1->runner->ChangeHealth(-30.f);
		c.trunner2->runner->FallDown(500);
		c.trunner2->runner->ChangeHealth(-30.f);
	}
	else if(c.trunner1->runner->GetBayabaFlag() &&
		!c.trunner2->runner->GetBayabaFlag())
	{
		v2 = v;
		V2 *= 1.5f;
		c.trunner2->runner->FallDown(1000);
		c.trunner2->runner->ChangeHealth(-30.f);
	}
	else if(!c.trunner1->runner->GetBayabaFlag() &&
		c.trunner2->runner->GetBayabaFlag())
	{
		v2 *= 1.5f;
		V2 = V;
		c.trunner1->runner->FallDown(1000);
		c.trunner1->runner->ChangeHealth(-30.f);
	}
	else
	{
		// 그리고 각 캐릭터의 점프 상태에 따라서 논리가 분기됩니다.
		if(c.trunner1->runner->_leap_flag && c.trunner2->runner->_leap_flag)
		{
			v2 *= 1.5f;
			V2 *= 1.5f;
			c.trunner1->runner->FallDown(500);
			c.trunner1->runner->ChangeHealth(-15.f);
			c.trunner2->runner->FallDown(500);
			c.trunner2->runner->ChangeHealth(-15.f);
		}
		else if(c.trunner1->runner->_leap_flag && !c.trunner2->runner->_leap_flag)
		{
			v2 = v;
			V2 *= 1.5f;
			c.trunner1->runner->UsingWSkill(1000);
			c.trunner2->runner->ChangeHealth(-10.f);
			c.trunner2->runner->FallDown(500);
		}
		else if(!c.trunner1->runner->_leap_flag && c.trunner2->runner->_leap_flag)
		{
			v2 *= 1.5f;
			V2 = V;
			c.trunner2->runner->UsingWSkill(1000);
			c.trunner1->runner->ChangeHealth(-10.f);
			c.trunner1->runner->FallDown(500);
		}
		
		//충돌 후 에너지 손실 적용...
		v2 *= 0.7f;
		V2 *= 0.7f;
	}

	//충돌하기 직전까지 전진
	c.trunner1->runner->_pos += (c.time - _cur_time) * v;
	c.trunner2->runner->_pos += (c.time - _cur_time) * V;

	//그리고 그들의 남은 시간도 줄어듭니다.
	c.trunner1->rest_time -= (c.time - _cur_time);
	c.trunner2->rest_time -= (c.time - _cur_time);

	//최종적으로 속도 수정... 이후의 일은 이 함수의 역할에서 벗어납니다.
	c.trunner1->rvcity = v2;
	c.trunner2->rvcity = V2;
}

void PhysicalEngine::HandleLeftBoundaryCol(const col_t & c)
{
	c.trunner1->runner->_pos += (c.time - _cur_time) * c.trunner1->rvcity;
	c.trunner1->rvcity = vector_t(ToPositive(c.trunner1->rvcity.x), c.trunner1->rvcity.y);
	c.trunner1->rest_time -= (c.time - _cur_time);
}

void PhysicalEngine::HandleRightBoundaryCol(const col_t & c)
{
	c.trunner1->runner->_pos += (c.time - _cur_time) * c.trunner1->rvcity;
	c.trunner1->rvcity = vector_t(ToNegative(c.trunner1->rvcity.x), c.trunner1->rvcity.y);
	c.trunner1->rest_time -= (c.time - _cur_time);
}

void PhysicalEngine::HandleCactusCol(const col_t & c)
{
	c.trunner1->runner->_pos += (c.time - _cur_time) * c.trunner1->rvcity;
	c.trunner1->runner->ChangeHealth(-5.f);
	c.trunner1->rest_time -= (c.time - _cur_time);

	vector_t dv = c.trunner1->runner->_pos - *c.cactus;

	c.trunner1->rvcity = dv / 2.f;

	Packet send_packet;
	send_packet << UNSIGNED_SHORT(SV_TO_CL_EVENT)
				<< UNSIGNED_SHORT(SV_TO_CL_RUNNER_COL_TO_CACTUS)
				<< c.trunner1->cl_id;
	SafeSendToAll(send_packet);
}

void PhysicalEngine::HandleItemCol(const col_t & c)
{
	if(c.trunner1->runner->CheckInventoryFull()) return;

	item_id_t item_id = c.item->item_id;

	bool erased = false;
	for(auto it = _item_list->begin(); it != _item_list->end(); ++it)
	{
		if(it->id == c.item->id)
		{
			Packet send_packet;
			send_packet << UNSIGNED_SHORT(SV_TO_CL_OBJECT_DEAD)
						<< c.item->id
						<< UNSIGNED_SHORT(OBJECT_ITEM);
			SafeSendToAll(send_packet);
			_item_list->erase(it);
			erased = true;
			break;
		}
	}
	
	assert(erased);

	Packet send_packet;
	send_packet << UNSIGNED_SHORT(SV_TO_CL_ADD_ITEM)
				<< UNSIGNED_SHORT(item_id);
	SafeSend(c.trunner1->cl_id, send_packet);
	c.trunner1->runner->AddItem(item_id);
	
}

void PhysicalEngine::HandleStoneCol(const col_t & c)
{
	c.trunner1->runner->_pos += (c.time - _cur_time) * c.trunner1->rvcity;

	c.trunner1->runner->FallDown(1000);
	c.trunner1->runner->ChangeHealth(-20.f);

	c.trunner1->rest_time -= (c.time - _cur_time);
	c.trunner1->rvcity += c.stone->vcity * (sv_delta_time / MSPPIXEL_F);

	Packet send_packet;
	send_packet << UNSIGNED_SHORT(SV_TO_CL_EVENT)
				<< UNSIGNED_SHORT(SV_TO_CL_STONE_HIT_RUNNER)
				<< c.trunner1->cl_id;
	SafeSendToAll(send_packet);
}

void PhysicalEngine::HandleHairCol(const col_t & c)
{
	c.trunner1->runner->_pos += (c.time - _cur_time) * c.trunner1->rvcity;
	c.trunner1->runner->ChangeHealth(-20.f);
	c.trunner1->rest_time -= (c.time - _cur_time);

	c.trunner1->runner->SetHairFlag(true);
	c.trunner1->runner->FallDown(800);
	auto hair = c.hair;
	hair->pos += c.hair->vcity * (sv_delta_time / MSPPIXEL_F) * _cur_time;
	hair->vcity = vector_t();
	hair->got = true;
	hair->grab_type = sv_hair_t::GRAB_RUNNER;
	hair->grabbed_one = c.trunner1->cl_id;

	c.trunner1->runner->SetPos(c.hair->pos);

	Packet send_packet;
	send_packet << UNSIGNED_SHORT(SV_TO_CL_EVENT)
				<< UNSIGNED_SHORT(SV_TO_CL_HAIR_GOT_RUNNER)
				<< c.trunner1->cl_id;
	SafeSendToAll(send_packet);
}

void PhysicalEngine::HandlePortalColWithRunner(const col_t & c)
{
	auto trunner = c.trunner1;
	auto runner = trunner->runner;
	auto portal = c.portal;
	
	runner->SetFlashFlag(true, 0.f);
	trunner->rvcity = PORTAL_LENGTH_F * GetUnitVector(portal->dir);
	trunner->rest_time = 1.f;

	Packet send_packet;
	send_packet << UNSIGNED_SHORT(SV_TO_CL_EVENT)
				<< UNSIGNED_SHORT(SV_TO_CL_PORTAL_PASS)
				<< trunner->cl_id;
	SafeSendToAll(send_packet);
}

void PhysicalEngine::HandlePortalColWithHair(const col_t & c)
{
	auto portal = c.portal;
	auto hair = c.hair;
	hair->pos += c.hair->vcity * (sv_delta_time / MSPPIXEL_F) * _cur_time;
	hair->got = true;
	hair->grab_type = sv_hair_t::GRAB_PORTAL;
	hair->grabbed_one = portal->id;

	pos_t warp_pos = hair->pos + PORTAL_LENGTH_F * GetUnitVector(portal->dir);

	sv_hair_t child_hair = sv_hair_t(	*_object_id_cnt_ptr,
										sv_hair_t::OWNER_PORTAL,
										portal->id,
										warp_pos,
										hair->vcity,
										false,
										-1,
										hair->begin_time );

	_hair_list->push_back(child_hair);

	Packet send_packet;
	send_packet << UNSIGNED_SHORT(SV_TO_CL_NEW_OBJECT)
				<< *_object_id_cnt_ptr
				<< UNSIGNED_SHORT(OBJECT_HAIR)
				<< static_cast<ID>(-1)
				<< child_hair.pos.x
				<< child_hair.pos.y
				<< child_hair.vcity.x
				<< child_hair.vcity.y;
	SafeSendToAll(send_packet);

	++(*_object_id_cnt_ptr);

	hair->vcity = vector_t();
}

void PhysicalEngine::HandleBoogerCol(const col_t & c)
{
	c.trunner1->runner->_pos += (c.time - _cur_time) * c.trunner1->rvcity;

	c.trunner1->runner->ChangeHealth(-10.f);
	c.trunner1->runner->SetNrSpeedBuf(-150);

	c.trunner1->rest_time -= (c.time - _cur_time);
	c.trunner1->rvcity += c.booger->vcity * (sv_delta_time / MSPPIXEL_F);

	Packet send_packet;
	send_packet << UNSIGNED_SHORT(SV_TO_CL_OBJECT_DEAD)
				<< c.booger->id
				<< UNSIGNED_SHORT(OBJECT_MAGIC_BOOGER);
	SafeSendToAll(send_packet);

	_booger_list->remove_if([c](sv_booger_t & booger_)
	{
		return booger_.id == c.booger->id;
	});

	send_packet.Clear();
	send_packet << UNSIGNED_SHORT(SV_TO_CL_EVENT)
				<< UNSIGNED_SHORT(SV_TO_CL_BOOGER_HIT_RUNNER)
				<< c.trunner1->cl_id;
	SafeSendToAll(send_packet);
}

////////////////// COLLISION PREDICTING /////////////////

void PhysicalEngine::SyncToCurTime(taged_runner_t * trunner)
{
	if(1.f - trunner->rest_time < _cur_time)
	{
		trunner->runner->_pos += (_cur_time - 1.f + trunner->rest_time) * trunner->rvcity;
		trunner->rest_time = 1.f - _cur_time;
	}
}

bool PhysicalEngine::PredictColToLeftBoundary(taged_runner_t * trunner, float * time_ptr)
{
	pos_t next_pos =	trunner->runner->_pos +
						trunner->rest_time * trunner->rvcity;

	const int left_boundary = -OKAWARU_BOX_WIDTH * (SV_MAX_NR_PLAYER / 2);

	if(	next_pos.x - trunner->runner->_radius < left_boundary)
	{
		if(trunner->runner->_pos.x - next_pos.x == 0.f)
			*time_ptr = 0.f;
		else
			*time_ptr = _cur_time +
				trunner->rest_time * ((trunner->runner->_pos.x - (left_boundary + trunner->runner->_radius)) / (trunner->runner->_pos.x - next_pos.x));
		return true;
	}
	else
		return false;
}

bool PhysicalEngine::PredictColToRightBoundary(taged_runner_t * trunner, float * time_ptr)
{
	pos_t next_pos =	trunner->runner->_pos +
						trunner->rest_time * trunner->rvcity;

	const int right_boundary =	OKAWARU_BOX_WIDTH * (SV_MAX_NR_PLAYER / 2);
	if( right_boundary < next_pos.x + trunner->runner->_radius)
	{
		if(next_pos.x - trunner->runner->_pos.x == 0.f)
			*time_ptr = 0.f;
		else
			*time_ptr = _cur_time +
				trunner->rest_time * (((right_boundary - trunner->runner->_radius) - trunner->runner->_pos.x) / (next_pos.x - trunner->runner->_pos.x));
		return true;
	}
	else
		return false;
}

bool PhysicalEngine::PredictColToCactus(	taged_runner_t * trunner,
											sv_cactus_t * cactus,
											float * time_ptr)
{
	vector_t dv = trunner->rvcity;
	vector_t dpos = trunner->runner->GetPos() -
					*cactus;

	float a = dv * dv;
	if(a == 0.f) return false;
	float b = 2.f * (dpos * dv);
	float len = (	trunner->runner->GetRadius() +
					CACTUS_RADIUS_F );
	float c = (dpos * dpos) - (len * len);
	float discriminant = (b * b) - 4.f * a * c;
	
	if(discriminant < POS_EPSILON) return false;

	float t = (-b - sqrtf(discriminant)) / (2.f * a);
	if(t < 0.f || 1.f - _cur_time < t) return false;

	*time_ptr = t + _cur_time;
	return true;
}

bool PhysicalEngine::PredictColToItem(	taged_runner_t * trunner,
										sv_item_t * item,
										float * time_ptr)
{
	vector_t dv = trunner->rvcity;
	vector_t dpos = trunner->runner->GetPos() -
					item->pos;

	float a = dv * dv;
	if(a == 0.f) return false;
	float b = 2.f * (dpos * dv);
	float len = (	trunner->runner->GetRadius() +
					CACTUS_RADIUS_F );
	float c = (dpos * dpos) - (len * len);
	float discriminant = (b * b) - 4.f * a * c;
	
	if(discriminant < POS_EPSILON) return false;

	float t = (-b - sqrtf(discriminant)) / (2.f * a);
	if(t < 0.f || 1.f - _cur_time < t) return false;

	*time_ptr = t + _cur_time;
	return true;
}

bool PhysicalEngine::PredictColToStone(	taged_runner_t * trunner,
										sv_stone_t * stone,
										float * time_ptr)
{
	vector_t dv = trunner->rvcity - (stone->vcity * (sv_delta_time / MSPPIXEL_F));
	vector_t dpos = trunner->runner->GetPos() -
					stone->pos;

	float a = dv * dv;
	if(a == 0.f) return false;
	float b = 2.f * (dpos * dv);
	float len = (	trunner->runner->GetRadius() +
					STONE_RADIUS_F );
	float c = (dpos * dpos) - (len * len);
	float discriminant = (b * b) - 4.f * a * c;
	
	if(discriminant < POS_EPSILON) return false;

	float t = (-b - sqrtf(discriminant)) / (2.f * a);
	if(t < 0.f || 1.f - _cur_time < t) return false;

	*time_ptr = t + _cur_time;
	return true;
}

bool PhysicalEngine::PredictColToHair(	taged_runner_t * trunner,
										sv_hair_t * hair,
										float * time_ptr)
{
	vector_t dv = trunner->rvcity - (hair->vcity * (sv_delta_time / MSPPIXEL_F));
	vector_t dpos = trunner->runner->GetPos() -
					hair->pos;

	float a = dv * dv;
	if(a == 0.f) return false;
	float b = 2.f * (dpos * dv);
	float len = ( trunner->runner->GetRadius() + 13.f);
	float c = (dpos * dpos) - (len * len);
	float discriminant = (b * b) - 4.f * a * c;
	
	if(discriminant < POS_EPSILON) return false;

	float t = (-b - sqrtf(discriminant)) / (2.f * a);
	if(t < 0.f || 1.f - _cur_time < t) return false;

	*time_ptr = t + _cur_time;
	return true;
}

bool PhysicalEngine::PredictColToPortal(const pos_t & pos,
										const vector_t & vel,
										sv_portal_t * portal,
										float * time_ptr)
{
	vector_t dv = vel;
	vector_t dpos = pos - portal->pos;

	float a = dv * dv;
	if(a == 0.f) return false;
	float b = 2.f * (dpos * dv);
	float len = PORTAL_RADIUS_F;
	float c = (dpos * dpos) - (len * len);
	float discriminant = (b * b) - 4.f * a * c;
	
	if(discriminant < POS_EPSILON) return false;

	float t = (-b - sqrtf(discriminant)) / (2.f * a);
	if(t < 0.f || 1.f - _cur_time < t) return false;

	*time_ptr = t + _cur_time;
	return true;
}


bool PhysicalEngine::PredictColToBooger(taged_runner_t * trunner,
									sv_booger_t * booger,
									float * time_ptr)
{
	vector_t dv = trunner->rvcity - (booger->vcity * (sv_delta_time / MSPPIXEL_F));
	vector_t dpos = trunner->runner->GetPos() -
					booger->pos;

	float a = dv * dv;
	if(a == 0.f) return false;
	float b = 2.f * (dpos * dv);
	float len = (	trunner->runner->GetRadius() +
					BOOGER_RADIUS_F );
	float c = (dpos * dpos) - (len * len);
	float discriminant = (b * b) - 4.f * a * c;
	
	if(discriminant < POS_EPSILON) return false;

	float t = (-b - sqrtf(discriminant)) / (2.f * a);
	if(t < 0.f || 1.f - _cur_time < t) return false;

	*time_ptr = t + _cur_time;
	return true;
}


bool PhysicalEngine::PredictCol(	taged_runner_t * trunner1,
									taged_runner_t * trunner2,
									float * time_ptr)
{
	vector_t dv = trunner1->rvcity - trunner2->rvcity;
	vector_t dpos = trunner1->runner->GetPos() -
					trunner2->runner->GetPos();

	float a = dv * dv;
	if(a == 0.f) return false;
	float b = 2.f * (dpos * dv);
	float len = (	trunner1->runner->GetRadius() +
					trunner2->runner->GetRadius() );
	float c = (dpos * dpos) - (len * len);
	float discriminant = (b * b) - 4.f * a * c;
	
	if(discriminant < POS_EPSILON) return false;

	float t = (-b - sqrtf(discriminant)) / (2.f * a);
	if(t < 0.f || 1.f - _cur_time < t) return false;

	*time_ptr = t + _cur_time;
	return true;
}

bool PhysicalEngine::PredictFirstCol(col_t * col_ptr)
{
	bool		first_found = true;

	float time;
	for(auto iter = _runners.Begin(); iter != _runners.End(); ++iter)
	{
		taged_runner_t * trunner1 = &(*iter).GetEl();
		// 왼쪽 경계 충돌 예측 검사
		if(trunner1->runner->GetHairFlag() == false)
		{
			if(PredictColToLeftBoundary(trunner1, &time))
			{
				if(first_found || col_ptr->time < time)
				{
					col_ptr->type = LEFT_BOUNDARY_COL;
					col_ptr->trunner1 = trunner1;
					col_ptr->time = time;
					first_found = false;
				}
			}
		
			// 오른쪽 경계 충돌 예측 검사
			if(PredictColToRightBoundary(trunner1, &time))
			{
				if(first_found || col_ptr->time < time)
				{
					col_ptr->type = RIGHT_BOUNDARY_COL;
					col_ptr->trunner1 = trunner1;
					col_ptr->time = time;
					first_found = false;
				}
			}
		}

		// 선인장과 충돌 예측 검사
		if(trunner1->runner->_leap_flag == false &&
			trunner1->runner->GetFlashFlag() == false &&
			trunner1->runner->GetGhostFlag() == false)
		{
			for(auto iter2 = _cactus_list->begin(); iter2 != _cactus_list->end(); ++iter2)
			{
				if(PredictColToCactus(trunner1, &*iter2, &time))
				{
					if(first_found || time < col_ptr->time)
					{
						col_ptr->type = CACTUS_COL;
						col_ptr->trunner1 = trunner1;
						col_ptr->cactus = &*iter2;
						col_ptr->time = time;
						first_found = false;
					}
				}
			}
		}

		// 길 위에 있는 아이템과 충돌 예측 검사
		if(!trunner1->runner->CheckInventoryFull() && trunner1->runner->_leap_flag == false)
		{
			for(auto iter2 = _item_list->begin(); iter2 != _item_list->end(); ++iter2)
			{
				if(PredictColToItem(trunner1, &*iter2, &time))
				{
					if(first_found || time < col_ptr->time)
					{
						col_ptr->type = ITEM_COL;
						col_ptr->trunner1 = trunner1;
						col_ptr->item = &*iter2;
						col_ptr->time = time;
						first_found = false;
					}
				}
			}
		}

		// 짱돌과 충돌 예측 검사
		if(trunner1->runner->GetState() != FALL_DOWN_STATE &&
			trunner1->runner->GetFlashFlag() == false &&
			trunner1->runner->GetGhostFlag() == false)
		{
			for(auto iter2 = _stone_list->begin(); iter2 != _stone_list->end(); ++iter2)
			{
				if(trunner1->cl_id == iter2->owner_id) continue;

				if(PredictColToStone(trunner1, &*iter2, &time))
				{
					if(first_found || time < col_ptr->time)
					{
						col_ptr->type = STONE_COL;
						col_ptr->trunner1 = trunner1;
						col_ptr->stone = &*iter2;
						col_ptr->time = time;
						first_found = false;
					}
				}
			}
		}

		if(!trunner1->runner->GetHairFlag() &&
			trunner1->runner->GetFlashFlag() == false &&
			trunner1->runner->GetGhostFlag() == false)
		{
			// 머리카락과 충돌 예측 검사
			for(auto iter2 = _hair_list->begin(); iter2 != _hair_list->end(); ++iter2)
			{
				if(iter2->got ||
	iter2->owner_type == sv_hair_t::OWNER_RUNNER && iter2->owner_id == trunner1->cl_id) continue;
				if(PredictColToHair(trunner1, &*iter2, &time))
				{
					if(first_found || time < col_ptr->time)
					{
						col_ptr->type = HAIR_COL;
						col_ptr->trunner1 = trunner1;
						col_ptr->hair = &*iter2;
						col_ptr->time = time;
						first_found = false;
					}
				}
			}
		}

		// 코딱지와 충돌 예측 검사
		if(trunner1->runner->GetState() != FALL_DOWN_STATE &&
			trunner1->runner->GetFlashFlag() == false &&
			trunner1->runner->GetGhostFlag() == false)
		{
			for(auto iter2 = _booger_list->begin(); iter2 != _booger_list->end(); ++iter2)
			{
				if(trunner1->cl_id == iter2->owner_id) continue;

				if(PredictColToBooger(trunner1, &*iter2, &time))
				{
					if(first_found || time < col_ptr->time)
					{
						col_ptr->type = BOOGER_COL;
						col_ptr->trunner1 = trunner1;
						col_ptr->booger = &*iter2;
						col_ptr->time = time;
						first_found = false;
					}
				}
			}
		}

		// runner간의 충돌 예측 검사
		for(auto iter2 = _runners.Begin(); iter2 != iter; ++iter2)
		{
			taged_runner_t * trunner2 = &(*iter2).GetEl();

			if(trunner1->runner->GetState() == FALL_DOWN_STATE ||
				trunner2->runner->GetState() == FALL_DOWN_STATE ||
				trunner1->runner->GetFlashFlag() ||
				trunner2->runner->GetFlashFlag() ||
				trunner1->runner->GetGhostFlag() ||
				trunner2->runner->GetGhostFlag()) continue;

			if(PredictCol(trunner1, trunner2, &time))
			{
				if(first_found || time < col_ptr->time)
				{
					col_ptr->type = RUNNER_COL;
					col_ptr->trunner1 = trunner1;
					col_ptr->trunner2 = trunner2;
					col_ptr->time = time;
					first_found = false;
				}
			}
		}
	}

	// 포탈 처리
	for (auto iter1 = _portal_list->begin(); iter1 != _portal_list->end(); ++iter1)
	{
		sv_portal_t * portal = &*iter1;
		// runner
		for (auto iter2 = _runners.Begin(); iter2 != _runners.End(); ++iter2)
		{
			taged_runner_t * trunner = &(*iter2).GetEl();
			if (trunner->runner->GetFlashFlag() == false)
			{
				if (PredictColToPortal( trunner->runner->GetPos(),
										trunner->rvcity,
										portal,
										&time ))
				{
					if (first_found || time < col_ptr->time)
					{
						col_ptr->type = PORTAL_COL_WITH_RUNNER;
						col_ptr->trunner1 = trunner;
						col_ptr->portal = portal;
						col_ptr->time = time;
						first_found = false;
					}
				}
			}
		}

		// hair
		for (auto iter2 = _hair_list->begin(); iter2 != _hair_list->end(); ++iter2)
		{
			sv_hair_t * hair = &*iter2;
			if (!iter2->got)
			{
				if (PredictColToPortal(	hair->pos,
										hair->vcity * (sv_delta_time / MSPPIXEL_F),
										portal,
										&time) )
				{
					if (first_found || time < col_ptr->time)
					{
					
						col_ptr->type = PORTAL_COL_WITH_HAIR;
						col_ptr->hair = hair;
						col_ptr->portal = portal;
						col_ptr->time = time;
						first_found = false;
					}
				}
			}
		}
	}

	return !first_found;
}