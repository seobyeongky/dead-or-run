#include "sv_play_scene.h"

void SvPlayScene::CheckGameOver()
{
	if(_began && _runner_map.Size() <= 1 && !_game_over)
	{
		_game_over_time = sv_present_time;
		_game_over = true;
	}
}

void SvPlayScene::Begin()
{
	Packet send_packet;
	send_packet << UNSIGNED_SHORT(SV_TO_CL_STATE);
	send_packet << UNSIGNED_SHORT(SV_TO_CL_PLAYING_STATE);
	SafeSendToAll(send_packet);

	_began = true;
}

void SvPlayScene::SendResult()
{
	if(!_result_send)
	{
		Packet send_packet;
		send_packet << UNSIGNED_SHORT(SV_TO_CL_GAME_OVER);
		if(_runner_map.Size() == 0)
		{
			send_packet << UNSIGNED_SHORT(SV_TO_CL_NO_WINNER);
		}
		else
		{
			runner_map_t::Iter	iter = _runner_map.Begin();
			ID					cl_id = (*iter).GetKey();
			size_t				nr_accum_win = ++sv_context.clients[cl_id].nr_accum_win;

			send_packet << UNSIGNED_SHORT(SV_TO_CL_THERE_IS_WINNER)
						<< (*iter).GetKey()
						<< nr_accum_win;

			// 패자들의 연승 초기화
			for(auto iter = sv_context.clients.Begin();
				iter != sv_context.clients.End();
				++iter)
			{
				if((*iter).GetKey() == cl_id) continue;
				(*iter).GetEl().nr_accum_win = 0;
			}
		}
		SafeSendToAll(send_packet);
	
		_result_send = true;
	}
}

void SvPlayScene::End()
{
	Packet send_packet;
	send_packet << UNSIGNED_SHORT(SV_TO_CL_STATE);
	send_packet << UNSIGNED_SHORT(SV_TO_CL_WAITING_STATE);
	SafeSendToAll(send_packet);

	SetNextScene(SV_WAITING_SCENE);
}

float SvPlayScene::GetMinY()
{
	float min_y = 0.f;

	for(auto iter = _runner_map.Begin(); iter != _runner_map.End(); ++iter)
	{
		SvRunner * runner = (*iter).GetEl();
		if(runner->GetPos().y < min_y)
		{
			min_y = runner->GetPos().y;
		}
	}

	if(min_y < _min_y) _min_y = min_y;

	return _min_y;
}

sv_hair_t * SvPlayScene::GetHairAcrossedPortal(ID portal_id)
{
	auto it = std::find_if(	_hair_list.begin(),
							_hair_list.end(),
							[portal_id](const sv_hair_t & hair){return hair.owner_type == sv_hair_t::OWNER_PORTAL && hair.owner_id == portal_id;});

	if (it == _hair_list.end()) return nullptr;
	else return &*it;
}

void SvPlayScene::SendHairDead(ID oid)
{
	Packet send_packet;
	send_packet << UNSIGNED_SHORT(SV_TO_CL_OBJECT_DEAD)
				<< oid
				<< UNSIGNED_SHORT(OBJECT_HAIR);
	SafeSendToAll(send_packet);
}

sv_portal_t * SvPlayScene::GetPortal(ID oid)
{
	auto it = std::find_if(	_portal_list.begin(),
							_portal_list.end(),
							[oid](const sv_portal_t & portal){return portal.id == oid;});

	if (it == _portal_list.end()) return nullptr;
	else return &*it;
}

sv_hair_t * SvPlayScene::GetPortalBindingHair(ID portal_id)
{
	auto it = std::find_if(	_hair_list.begin(),
							_hair_list.end(),
							[portal_id](const sv_hair_t & hair){return hair.grab_type == sv_hair_t::GRAB_PORTAL && hair.grabbed_one == portal_id;} );
	if (it == _hair_list.end()) return nullptr;
	else return &*it;
}

void SvPlayScene::SendSkillAccepted(ID cl_id, sv_to_cl_skill_t stype)
{
	Packet send_packet;
	send_packet <<	UNSIGNED_SHORT(SV_TO_CL_SKILL_ACCEPTED)
				<<	UNSIGNED_SHORT(stype);
	SafeSend(cl_id, send_packet);
}