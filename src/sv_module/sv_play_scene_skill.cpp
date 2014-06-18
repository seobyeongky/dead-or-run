#include "sv_play_scene.h"

void SvPlayScene::HandleBackAttackSkill(SvRunner * runner)
{
	runner->UsingQSkill(500);

	// hit circle
	vector_t	hit_origin =	runner->GetPos() +
								GetUnitVector(runner->GetDir()) * 25.f;
	float		hit_radius = 30.f;

	for(auto iter = _runner_map.Begin(); iter != _runner_map.End(); ++iter)
	{
		SvRunner * cur_runner = (*iter).GetEl();
		if(cur_runner == runner) continue;
		if(GetDistance(cur_runner->GetPos(), hit_origin) < hit_radius &&
			cur_runner->GetGhostFlag() == false)
		{
			cur_runner->FallDown(1000);
			cur_runner->ChangeHealth(-10.f);
		}
	}
}

void SvPlayScene::HandleMadDogSkill(SvRunner * runner)
{
	runner->UsingQSkill(7200);

	runner->SetSpeed(41.f);
}

void SvPlayScene::HandleBayabaSkill(SvRunner * runner)
{
	runner->UsingQSkill(120);
	
	runner->SetSpeed(200.f);

	runner->SetBayabaFlag();
}

void SvPlayScene::HandleGrudgeSkill(ID cl_id, SvRunner * runner)
{
	runner->UsingQSkill(HAIR_FLY_TIME);

	pos_t pos(runner->GetPos() + GetUnitVector(runner->GetDir()) * 10.f);
	vector_t vcity(	GetUnitVector(runner->GetDir())
					* (HAIR_SPEED_F + runner->GetSpeed()) );

	sv_hair_t new_hair(	_object_id_cnt,
						sv_hair_t::OWNER_RUNNER,
						cl_id,
						pos,
						vcity,
						false,
						-1,
						sv_present_time);

	_hair_list.push_back(new_hair);

	Packet send_packet;
	send_packet << UNSIGNED_SHORT(SV_TO_CL_NEW_OBJECT)
				<< _object_id_cnt
				<< UNSIGNED_SHORT(OBJECT_HAIR)
				<< cl_id
				<< pos.x
				<< pos.y
				<< vcity.x
				<< vcity.y;
	SafeSendToAll(send_packet);

	++_object_id_cnt;
}

void SvPlayScene::HandleSmokeSkill(ID cl_id, SvRunner * runner)
{
	runner->UsingWSkill(300);

	sv_smoke_t smoke(	_object_id_cnt,
						cl_id,
						runner->GetPos(),
						(runner->GetSpeed() + SMOKE_SPEED_F) * GetUnitVector(runner->GetDir()),
						sv_present_time );

	_smoke_list.push_back(smoke);

	Packet send_packet;
	send_packet << UNSIGNED_SHORT(SV_TO_CL_NEW_OBJECT)
				<< smoke.id
				<< UNSIGNED_SHORT(OBJECT_SMOKE)
				<< smoke.owner_id
				<< smoke.pos.x
				<< smoke.pos.y
				<< smoke.vcity.x
				<< smoke.vcity.y;
	SafeSendToAll(send_packet);

	++_object_id_cnt;
}

void SvPlayScene::HandleLeapSkill(SvRunner * runner)
{
	runner->UsingWSkill(1500);

	runner->SetSpeed(55.f);

	runner->SetLeapFlag();
}

void SvPlayScene::HandleFlashTransfer(SvRunner * runner)
{
	runner->UsingWSkill(200);
	runner->SetFlashFlag(true, 70.f);
}

void SvPlayScene::HandleGodComeSkill(SvRunner * runner)
{
	runner->UsingQSkill(5000);
}

void SvPlayScene::HandleGhostSkill(SvRunner * runner)
{
	runner->UsingWSkill(1800);

	runner->SetGhostFlag(true);
}

void SvPlayScene::HandleHellGateSkill(ID cl_id, SvRunner * runner)
{
	runner->UsingQSkill(250);

	sv_portal_t gate(	_object_id_cnt, 
						cl_id,
						runner->GetPos() + PORTAL_DIST_F * GetUnitVector(runner->GetDir()),
						runner->GetDir() );

	_portal_list.push_back(gate);

	Packet send_packet;
	send_packet << UNSIGNED_SHORT(SV_TO_CL_NEW_OBJECT)
				<< gate.id
				<< UNSIGNED_SHORT(OBJECT_PORTAL)
				<< gate.owner_id
				<< gate.pos.x
				<< gate.pos.y
				<< gate.dir;
	SafeSendToAll(send_packet);

	++_object_id_cnt;
}

void SvPlayScene::HandleGroundAttackSkill(ID cl_id, SvRunner * runner)
{
	runner->UsingWSkill(300);

	// hit circle
	vector_t	hit_origin =	runner->GetPos();
	float		hit_radius = 70.f;

	for(auto iter = _runner_map.Begin(); iter != _runner_map.End(); ++iter)
	{
		SvRunner * cur_runner = (*iter).GetEl();
		if(cur_runner == runner) continue;
		if(GetDistance(cur_runner->GetPos(), hit_origin) < hit_radius &&
			cur_runner->GetGhostFlag() == false)
		{
			cur_runner->FallDown(500);
			cur_runner->ChangeHealth(-10.f);
			cur_runner->SetSpeed(0.f);
		}
	}

	Packet send_packet;
	send_packet << UNSIGNED_SHORT(SV_TO_CL_EVENT)
				<< UNSIGNED_SHORT(SV_TO_CL_GROUND_ATTACK)
				<< runner->GetPos().x
				<< runner->GetPos().y;
	SafeSendToAll(send_packet);
}

void SvPlayScene::HandleMagicBoogerSkill(ID cl_id, SvRunner * runner)
{
	runner->UsingQSkill(200);

	sv_booger_t booger;
	booger.id = _object_id_cnt;
	booger.owner_id = cl_id;
	booger.pos = runner->GetPos();
	booger.vcity = GetUnitVector(runner->GetDir()) * (BOOGER_SPEED_F + runner->GetSpeed());
	_booger_list.push_back(booger);
			
	Packet send_packet;
	send_packet << UNSIGNED_SHORT(SV_TO_CL_NEW_OBJECT)
				<< _object_id_cnt
				<< UNSIGNED_SHORT(OBJECT_MAGIC_BOOGER)
				<< booger.owner_id
				<< booger.pos.x
				<< booger.pos.y
				<< booger.vcity.x
				<< booger.vcity.y;
	SafeSendToAll(send_packet);
	++_object_id_cnt;
}

void SvPlayScene::HandleProvokingSkill(ID cl_id, SvRunner * runner)
{
	runner->UsingWSkill(350);

	
}

bool SvPlayScene::HandleSkillEvent(ID cl_id, 
									  const skill_id_t & skill_id,
									  SvRunner * runner)
{
	if (skill_id < 0 || skill_id >= INVALID_SKILL) return false;
	if (runner->GetState() == FALL_DOWN_STATE) return false;

	float cost = static_cast<float>(SKILL(skill_id)->cost);
	if(runner->GetHealth() >= cost) runner->ChangeHealth(-cost);
	else return false;

	switch(skill_id)
	{
	case BACK_ATTACK_SKILL: HandleBackAttackSkill(runner); break;
	case MAD_DOG_SKILL: HandleMadDogSkill(runner); break;
	case BAYABA_SKILL: HandleBayabaSkill(runner); break;
	case GRUDGE_SKILL: HandleGrudgeSkill(cl_id, runner); break;
	case SMOKE_SKILL: HandleSmokeSkill(cl_id, runner); break;
	case LEAP_SKILL: HandleLeapSkill(runner); break;
	case FLASH_TRANSFER: HandleFlashTransfer(runner); break;
	case GOD_COME_SKILL: HandleGodComeSkill(runner); break;
	case GHOST_SKILL: HandleGhostSkill(runner); break;
	case HELL_GATE_SKILL: HandleHellGateSkill(cl_id, runner); break;
	case GROUND_ATTACK_SKILL: HandleGroundAttackSkill(cl_id, runner); break;
	case MAGIC_BOOGER_SKILL: HandleMagicBoogerSkill(cl_id, runner); break;
	case PROVOKING_SKILL: HandleProvokingSkill(cl_id, runner); break;
	}

	return true;
}
