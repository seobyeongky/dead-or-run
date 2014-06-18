#include "sv_play_scene.h"
#include "../runner.h"
#include "../util.h"
#include "../play_scene_layout.h"
#include "../config.h"
#include "../speed_def.h"

SvPlayScene::SvPlayScene(	sv_scene_id_t id) : 
	SvScene(id),
	_runner_map(),
	_begin_time(),
	_began(false),
	_tile_row_deq(),
	_nr_tile_row(0U),
	_last_water_row(0U),
	_min_y(0.f),
	_object_id_cnt(0u),
	_cactus_list(),
	_water_list(),
	_item_list(),
	_stone_list(),
	_hair_list(),
	_smoke_list(),
	_portal_list(),
	_booger_list(),
	_last_item_row(0U),
	_p_engine(&_object_id_cnt),
	_result_send(false),
	_game_over_time(),
	_game_over(false),
	_accum_time(0)
{
}

SvPlayScene::~SvPlayScene()
{
	for(auto it = _runner_map.Begin(); it != _runner_map.End(); ++it)
		delete (*it).GetEl();
	_runner_map.Clear();
}

void SvPlayScene::HandleNewCl(const client_t & new_cl)
{
	Packet send_packet;
	send_packet << UNSIGNED_SHORT(SV_TO_CL_STATE);
	send_packet << UNSIGNED_SHORT(SV_TO_CL_PLAYING_STATE);
	SafeSend(new_cl.id, send_packet);

	send_packet.Clear();
	send_packet << UNSIGNED_SHORT(SV_TO_CL_GAME_CONTEXT);
	send_packet << _runner_map.Size();
	for(auto iter = _runner_map.Begin(); iter != _runner_map.End(); ++iter)
	{
		send_packet << (*iter).GetKey()
					<< UNSIGNED_SHORT((*iter).GetEl()->GetId());
	}
	send_packet << _nr_tile_row - _tile_row_deq.size();
	send_packet << _tile_row_deq.size();	
	for(auto it = _tile_row_deq.rbegin(); it != _tile_row_deq.rend(); ++it)
	{
		for(int i = 0; i < NR_TILE_IN_ROW; ++i)
			send_packet << UNSIGNED_SHORT(it->tiles[i]);
	}
	SafeSend(new_cl.id, send_packet);
}

void SvPlayScene::HandleGoneCl(const client_t & gone_cl)
{
	runner_map_t::Iter iter;
	if(_runner_map.Find(gone_cl.id, &iter))
	{
		_p_engine.EraseRunner(gone_cl.id);
		delete (*iter).GetEl();
		_runner_map.Erase(iter);
	}
}

void SvPlayScene::HandleRecvPacket(	const client_t & cl,
										cl_to_sv_t header,
										Packet & recv_packet)
{
	switch(header)
	{
	case CL_TO_SV_EVENT:
		{
			runner_map_t::Iter iter;
			if(!_runner_map.Find(cl.id, &iter)) return;
			SvRunner * runner = (*iter).GetEl();
			if(_began) HandleClEvent(cl.id, runner, recv_packet);
		}
		break;
		
	default:
		LeaveLog(L"SvPlayScene : HandleRecvPacket : 알 수 없는 header(%d)", header);
	}
}

void SvPlayScene::Go()
{
	if(!_began && _begin_time + 3000 < sv_present_time) Begin();
	if(_game_over && _game_over_time + 2000 < sv_present_time) SendResult();
	if(_game_over && _game_over_time + 5000 < sv_present_time) End();
	
	_p_engine.Update();

	for(auto iter = _runner_map.Begin();
		iter != _runner_map.End();
		++iter)
	{
		(*iter).GetEl()->Update();
	}

	HandleObject();

	HandleDeadMan();

	HandleTileCreation();

	_accum_time += sv_delta_time;
	if(_accum_time > PLAY_SCENE_MSPPACKET)
	{
		_accum_time -= PLAY_SCENE_MSPPACKET;
		if(_accum_time >= PLAY_SCENE_MSPPACKET)
			LeaveLog(L"SvPlayScene : 컴퓨터 성능이 너무 안좋습니다.");
		
		SendBasicInfo();
	}

	CheckGameOver();
}

void SvPlayScene::HandleClEvent(ID cl_id, SvRunner * runner,Packet & recv_packet)
{
	header_t event;
	if(!(recv_packet >> event)) return;
	
	header_t b;

	switch(event)
	{
	case CL_TO_SV_EVENT_LEFT:
		if(!(recv_packet >> b)) return;
		if(b == KEY_DOWN) runner->LeftRotating(true);
		else if(b == KEY_UP) runner->LeftRotating(false);
		break;

	case CL_TO_SV_EVENT_UP:
		if(!(recv_packet >> b)) return;
		if(b == KEY_DOWN) runner->Acceling(true);
		else if(b == KEY_UP) runner->Acceling(false);
		break;

	case CL_TO_SV_EVENT_RIGHT:
		if(!(recv_packet >> b)) return;
		if(b == KEY_DOWN) runner->RightRotating(true);
		else if(b == KEY_UP) runner->RightRotating(false);
		break;

	case CL_TO_SV_EVENT_DOWN:
		if(!(recv_packet >> b)) return;
		if(b == KEY_DOWN) runner->Breaking(true);
		else if(b == KEY_UP) runner->Breaking(false);
		break;

	case CL_TO_SV_EVENT_Q:
		if (HandleSkillEvent(cl_id, runner->GetQSkill(), runner))
			SendSkillAccepted(cl_id, SV_TO_CL_Q_SKILL_ACCEPTED);
		break;

	case CL_TO_SV_EVENT_W:
		if (HandleSkillEvent(cl_id, runner->GetWSkill(), runner))
			SendSkillAccepted(cl_id, SV_TO_CL_W_SKILL_ACCEPTED);
		break;

	case CL_TO_SV_EVENT_E:
		if (HandleSkillEvent(cl_id, runner->GetESkill(), runner))
			SendSkillAccepted(cl_id, SV_TO_CL_E_SKILL_ACCEPTED);
		break;

	case CL_TO_SV_EVENT_R:
		if (HandleSkillEvent(cl_id, runner->GetRSkill(), runner))
			SendSkillAccepted(cl_id, SV_TO_CL_R_SKILL_ACCEPTED);
		break;

	case CL_TO_SV_EVENT_ITEM:
		if(!(recv_packet >> b)) return;
		HandleItemSpending(static_cast<item_id_t>(b), cl_id, runner);
		break;

	case CL_TO_SV_EVENT_READY:
		break;
	}
}

void SvPlayScene::HandleDeadObject()
{
	GetMinY();

	float storm_y = _min_y + BASIC_SAND_STORM_DIST_F + _runner_map.Size() * DIST_PER_RUNNER_F;

	// 모래바람 아래 object 지우기
	for(auto iter = _cactus_list.begin(); iter != _cactus_list.end();)
	{
		if(iter->y > storm_y) iter = _cactus_list.erase(iter);
		else ++iter;
	}

	for(auto iter = _water_list.begin(); iter != _water_list.end();)
	{
		if(iter->y > storm_y) iter = _water_list.erase(iter);
		else ++iter;
	}

	for(auto iter = _item_list.begin(); iter != _item_list.end();)
	{
		if(iter->pos.y > storm_y)
		{
			Packet send_packet;
			send_packet << UNSIGNED_SHORT(SV_TO_CL_OBJECT_DEAD)
						<< iter->id
						<< UNSIGNED_SHORT(OBJECT_ITEM);
			
			SafeSendToAll(send_packet);

			iter = _item_list.erase(iter);
		}
		else ++iter;
	}

	for(auto iter = _stone_list.begin(); iter != _stone_list.end();)
	{
		if(iter->pos.y > storm_y || iter->pos.y < _min_y - 700.f)
		{
			Packet send_packet;
			send_packet << UNSIGNED_SHORT(SV_TO_CL_OBJECT_DEAD)
						<< iter->id
						<< UNSIGNED_SHORT(OBJECT_STONE);
			
			SafeSendToAll(send_packet);

			iter = _stone_list.erase(iter);
		}
		else ++iter;
	}

	for(auto iter = _hair_list.begin(); iter != _hair_list.end();)
	{
		if(iter->pos.y > storm_y)
		{
			Packet send_packet;
			send_packet << UNSIGNED_SHORT(SV_TO_CL_OBJECT_DEAD)
						<< iter->id
						<< UNSIGNED_SHORT(OBJECT_HAIR);
			
			SafeSendToAll(send_packet);

			iter = _hair_list.erase(iter);
		}
		else ++iter;
	}

	for(auto iter = _portal_list.begin(); iter != _portal_list.end();)
	{
		if (iter->pos.y > storm_y)
		{
			Packet send_packet;
			send_packet << UNSIGNED_SHORT(SV_TO_CL_OBJECT_DEAD)
						<< iter->id
						<< UNSIGNED_SHORT(OBJECT_HAIR);

			SafeSendToAll(send_packet);

			iter = _portal_list.erase(iter);
		}
		else ++iter;
	}

	for(auto iter = _booger_list.begin(); iter != _booger_list.end();)
	{
		if(iter->pos.y > storm_y || iter->pos.y < _min_y - 700.f)
		{
			Packet send_packet;
			send_packet << UNSIGNED_SHORT(SV_TO_CL_OBJECT_DEAD)
						<< iter->id
						<< UNSIGNED_SHORT(OBJECT_MAGIC_BOOGER);
			
			SafeSendToAll(send_packet);

			iter = _booger_list.erase(iter);
		}
		else ++iter;
	}
}

void SvPlayScene::HandleItemSpending(item_id_t item_id, ID cl_id, SvRunner * runner)
{
	Packet send_packet;
	send_packet << UNSIGNED_SHORT(SV_TO_CL_ERASE_ITEM)
				<< UNSIGNED_SHORT(item_id);
	SafeSend(cl_id, send_packet);
	if(!(runner->SpendItem(item_id))) return;

	switch(item_id)
	{
	case ITEM_STONE:
		{
			sv_stone_t stone;
			stone.id = _object_id_cnt;
			stone.owner_id = cl_id;
			stone.pos = runner->GetPos();
			stone.vcity = GetUnitVector(runner->GetDir()) * (STONE_SPEED_F + runner->GetSpeed());
			_stone_list.push_back(stone);
			
			Packet send_packet;
			send_packet << UNSIGNED_SHORT(SV_TO_CL_NEW_OBJECT)
						<< _object_id_cnt
						<< UNSIGNED_SHORT(OBJECT_STONE)
						<< stone.owner_id
						<< stone.pos.x
						<< stone.pos.y
						<< stone.vcity.x
						<< stone.vcity.y;
			SafeSendToAll(send_packet);
			++_object_id_cnt;
		}
		break;
	case ITEM_BIG_WONTON: runner->ChangeHealth(35.f); break;
	case ITEM_CURRY_RICE: runner->ChangeHealth(35.f); break;
	case ITEM_DOUBLE_JAJANG: runner->ChangeHealth(35.f); break;
	case ITEM_JAJANG: runner->ChangeHealth(30.f); break;
	case ITEM_JOB_RICE: runner->ChangeHealth(10.f); break;
	case ITEM_JONG_HO_FOOD: runner->ChangeHealth(40.f); break;
	case ITEM_RAMYUN: runner->ChangeHealth(20.f); break;
	case ITEM_RICE: runner->ChangeHealth(20.f); break;
	case ITEM_TEOK_RAMYUN: runner->ChangeHealth(30.f); break;
	case ITEM_WONTON: runner->ChangeHealth(25.f); break;
	default:
		LeaveLog(L"SvPlayScene : HandleItemSpending : 알 수 없는 item(%d)", item_id);
	}
}

void SvPlayScene::HandleItemCreation()
{
	if(_began && _last_item_row + 25U < _nr_tile_row)
	{
		int rnd_in_31 = rand() % 31;
		if(rnd_in_31 == 7)
		{
			item_id_t item_id = ITEM_INVALID;
			switch(rand() % 11)
			{
			case 0: item_id = ITEM_BIG_WONTON; break;
			case 1: item_id = ITEM_CURRY_RICE; break;
			case 2: item_id = ITEM_DOUBLE_JAJANG; break;
			case 3: item_id = ITEM_JAJANG; break;
			case 4: item_id = ITEM_JOB_RICE; break;
			case 5: item_id = ITEM_JONG_HO_FOOD; break;
			case 6: item_id = ITEM_RAMYUN; break;
			case 7: item_id = ITEM_RICE; break;
			case 8: item_id = ITEM_TEOK_RAMYUN; break;
			case 9: item_id = ITEM_WONTON; break;
			case 10: item_id = ITEM_STONE; break;
			}
			pos_t pos(	GetXFromTileUnit(rand() % NR_TILE_IN_ROW),
						_min_y - 10 * TILE_HEIGHT);
			sv_item_t new_item(_object_id_cnt, item_id, pos);
			
			Packet send_packet;
			send_packet << UNSIGNED_SHORT(SV_TO_CL_NEW_OBJECT)
						<< _object_id_cnt
						<< UNSIGNED_SHORT(OBJECT_ITEM)
						<< UNSIGNED_SHORT(item_id)
						<< pos.x
						<< pos.y;
			SafeSendToAll(send_packet);

			_item_list.push_back(new_item);
			++_object_id_cnt;
			_last_item_row = _nr_tile_row;
		}
	}
}

void SvPlayScene::HandleHair()
{
	for(auto it = _hair_list.begin(); it != _hair_list.end();)
	{
		if(it->got)
		{
			switch (it->grab_type)
			{
				case sv_hair_t::GRAB_RUNNER:
				{
					pos_t owner_pos;
					float owner_radius;
					vector_t owner_vcity;
					if(it->owner_type == sv_hair_t::OWNER_RUNNER)
					{
						runner_map_t::Iter runner_iter;
						if(!_runner_map.Find(it->owner_id, &runner_iter))
						{
							SendHairDead(it->id);
							it = _hair_list.erase(it);
							continue;
						}
						SvRunner * owner = (*runner_iter).GetEl();
						owner_pos = owner->GetPos();
						owner_radius = owner->GetRadius();
						owner_vcity = owner->GetSpeed() * GetUnitVector(owner->GetDir());
					}
					else if(it->owner_type == sv_hair_t::OWNER_PORTAL)
					{
						auto portal = GetPortal(it->owner_id);
						if (portal == nullptr)
						{
							SendHairDead(it->id);
							it = _hair_list.erase(it);
							continue;
						}
						owner_pos = portal->pos + PORTAL_LENGTH_F * GetUnitVector(portal->dir);
						owner_radius = PORTAL_RADIUS_F;
						owner_vcity = vector_t();
					}

					vector_t dpos(owner_pos - it->pos);
					vector_t vcity = HAIR_SPEED_F * GetUnitVector(dpos) + owner_vcity;
					it->vcity = vcity;
					runner_map_t::Iter grabbed_one_iter;
					if(!_runner_map.Find(it->grabbed_one, &grabbed_one_iter))
					{
						SendHairDead(it->id);
						it = _hair_list.erase(it);
						continue;
					}
					SvRunner * grabbed_one = (*grabbed_one_iter).GetEl();
					grabbed_one->SetPos(it->pos);
					
					// 머리카락이 도착했는지 검사
					if(GetSize(owner_pos - it->pos) < owner_radius)
					{
						if(it->owner_type == sv_hair_t::OWNER_PORTAL)
						{
							sv_hair_t * parent_hair = GetPortalBindingHair(it->owner_id);

							if (parent_hair == nullptr)
								LeaveLog(L"exception... parent hair not found");
							else
							{
								parent_hair->grab_type = it->grab_type;
								parent_hair->grabbed_one = it->grabbed_one;
								grabbed_one->SetPos(parent_hair->pos);
							}
						}
						
						SendHairDead(it->id);
						it = _hair_list.erase(it);
						continue;
					}
				}
				break;

				case sv_hair_t::GRAB_PORTAL:
				{
					// 자신의 자식 머리 조사
					sv_hair_t * child_hair = GetHairAcrossedPortal(it->grabbed_one);
					if (child_hair == nullptr)
					{
						SendHairDead(it->id);
						it = _hair_list.erase(it);
						continue;
					}
				}
				break;
			}
		}
		else
		{
			if(it->begin_time + HAIR_FLY_TIME < sv_present_time)
			{
				Packet send_packet;
				send_packet << UNSIGNED_SHORT(SV_TO_CL_OBJECT_DEAD)
							<< it->id
							<< UNSIGNED_SHORT(OBJECT_HAIR);
				SafeSendToAll(send_packet);
				it = _hair_list.erase(it);
				continue;	
			}
		}
		++it;
	}
}

void SvPlayScene::HandleSmoke()
{
	for(auto it = _smoke_list.begin(); it != _smoke_list.end();)
	{
		if(it->begin_time + SMOKE_DURATION < sv_present_time)
		{
			Packet send_packet;
			send_packet << UNSIGNED_SHORT(SV_TO_CL_OBJECT_DEAD)
						<< it->id
						<< UNSIGNED_SHORT(OBJECT_SMOKE);
			SafeSendToAll(send_packet);
			it = _smoke_list.erase(it);
		}
		else
		{
			it->pos += it->vcity * (sv_delta_time / MSPPIXEL_F);
			++it;
		}
	}
}

void SvPlayScene::HandleWaterCol()
{
	for(auto iter = _runner_map.Begin(); iter != _runner_map.End(); ++iter)
	{
		SvRunner * runner = (*iter).GetEl();
		bool hit = false;
		if(!runner->GetLeapFlag())
		{
			for(auto iter2 = _water_list.begin(); iter2 != _water_list.end(); ++iter2)
			{
				if(GetDistance(runner->GetPos(), *iter2) < 30.f)
				{
					runner->SetHealth(100.f);
					runner->SetSpeed(10.f);
					hit = true;
					break;
				}
			}
		}

		if(hit)
		{
			if(!runner->GetWaterFlag())
			{
				Packet send_packet;
				send_packet << UNSIGNED_SHORT(SV_TO_CL_EVENT)
							<< UNSIGNED_SHORT(SV_TO_CL_RUNNER_GOT_WATER)
							<< (*iter).GetKey();
				SafeSendToAll(send_packet);
				runner->SetWaterFlag(true);
			}
		}
		else if(runner->GetWaterFlag())
		{
			runner->BeginToRun();
			runner->SetWaterFlag(false);
		}
	}
}

void SvPlayScene::HandleObject()
{
	HandleItemCreation();
	HandleHair();
	HandleSmoke();
	HandleWaterCol();
	HandleDeadObject();
}

void SvPlayScene::HandleDeadMan()
{
	float min_y = GetMinY();
	for(auto iter = _runner_map.Begin(); iter != _runner_map.End();)
	{
		SvRunner * runner = (*iter).GetEl();
		if(runner->GetPos().y > min_y + (BASIC_SAND_STORM_DIST_F + _runner_map.Size() * DIST_PER_RUNNER_F))
		{
			_p_engine.EraseRunner((*iter).GetKey());
			Packet send_packet;
			send_packet << UNSIGNED_SHORT(SV_TO_CL_RUNNER_DEAD);
			send_packet << (*iter).GetKey();
			SafeSendToAll(send_packet);
			delete runner;
			iter = _runner_map.Erase(iter);
		}
		else ++iter;
	}
}

void SvPlayScene::HandleTileCreation()
{
	float min_y = GetMinY();

	while(_nr_tile_row * TILE_HEIGHT < -min_y + 7 * TILE_HEIGHT)
	{
		tile_row_t new_row;

		// 선인장 만들기
		if(_began && _nr_tile_row % 2 == 1)
		{
			int rand_in_100 = rand() % 100;

			if(rand_in_100 < 6)
			{
				// 6% 확률로 선인장 둘을 만듭니다.
				int first = rand() % NR_TILE_IN_ROW;
				new_row.tiles[first] = CACTUS_TILE;
				_cactus_list.push_back(
					sv_cactus_t(GetXFromTileUnit(first), GetYFromTileUnit(_nr_tile_row)) );
				int second = rand() % (NR_TILE_IN_ROW - 1);
				if(second >= first) ++second;
				new_row.tiles[second] = CACTUS_TILE;
				_cactus_list.push_back(
					sv_cactus_t(GetXFromTileUnit(second), GetYFromTileUnit(_nr_tile_row)) );
			}
			else if(rand_in_100 < 12)
			{
				// 6% 확률로 선인장 하나를 만듭니다.
				int pos = rand() % NR_TILE_IN_ROW;
				new_row.tiles[pos] = CACTUS_TILE;
				_cactus_list.push_back(
					sv_cactus_t(GetXFromTileUnit(pos), GetYFromTileUnit(_nr_tile_row)) );

			}
		}

		// 오아시스 만들기
		if(_began && _last_water_row + 30U < _nr_tile_row)
		{
			int rand_in_100 = rand() % 100;
		
			if(rand_in_100 < 1)
			{
				// 1% 확률로 왼쪽에 오아시스를 만듭니다.
				new_row.tiles[0] = WATER_TILE;
				_water_list.push_back(
					sv_water_t(	GetXFromTileUnit(0) + TILE_WIDTH / 2.f,
								GetYFromTileUnit(_nr_tile_row) + TILE_HEIGHT / 2.f) );
			}
			else if(rand_in_100 < 2)
			{
				// 1% 확률로 오른쪽에 오아시스를 만듭니다.
				new_row.tiles[NR_TILE_IN_ROW - 1] = WATER_TILE;
				_water_list.push_back(
					sv_water_t(GetXFromTileUnit(NR_TILE_IN_ROW - 1) + TILE_WIDTH / 2.f,
					GetYFromTileUnit(_nr_tile_row) + TILE_HEIGHT / 2.f)  );
			}
		}

		_tile_row_deq.push_front(new_row);
		++_nr_tile_row;
		Packet send_packet;
		send_packet << UNSIGNED_SHORT(SV_TO_CL_TILE_ROW);
		for(int i = 0; i < NR_TILE_IN_ROW; ++i)
			send_packet << UNSIGNED_SHORT(new_row.tiles[i]);
		SafeSendToAll(send_packet);
	}
}

void SvPlayScene::HandleRunnerWaterCol()
{
	for(auto iter = _runner_map.Begin();
		iter != _runner_map.End();
		++iter)
	{
		SvRunner * runner = (*iter).GetEl();
		FloatRect fas;
	}
}

void SvPlayScene::SendBasicInfo()
{
	Packet send_packet;
	send_packet << UNSIGNED_SHORT(SV_TO_CL_BASIC_INFO);
	send_packet << _runner_map.Size();
	for(auto iter = _runner_map.Begin();
		iter != _runner_map.End();
		++iter)
	{
		send_packet << (*iter).GetKey();
		SvRunner * runner = (*iter).GetEl();
		send_packet << runner->GetPos().x << runner->GetPos().y
					<< runner->GetDir()
					<< runner->GetSpeed()
					<< runner->GetHealth()
					<< UNSIGNED_SHORT(runner->GetState());
	}

	send_packet << _hair_list.size();
	for(auto it = _hair_list.begin(); it != _hair_list.end(); ++it)
	{
		send_packet << it->id
					<< it->pos.x
					<< it->pos.y;
	}

#if NET_CONFIG == ALL_TCP
	SafeSendToAll(send_packet);
#elif NET_CONFIG == PORTION_UDP
	UnsafeSendToAll(send_packet);
#endif
}
