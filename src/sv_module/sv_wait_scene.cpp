#include "sv_wait_scene.h"

SvWaitScene::SvWaitScene(sv_scene_id_t id) :
	SvScene(id),
	_set(),
	_player_map(),
	_observers(),
	_all_ready(false),
	_all_ready_time(0)
{
}

SvWaitScene::~SvWaitScene()
{
}

void SvWaitScene::Init()
{
	_player_map.Clear();
	_observers.clear();

	for(int i = 0; i < SV_MAX_NR_PLAYER; ++i)
	{
		_set[i].mask = false;
	}

	for(auto iter = sv_context.clients.Begin();
		iter != sv_context.clients.End();
		++iter)
	{
		cl_context_t & context = (*iter).GetEl();
		HandleNewCl(context.info);
	}

	_all_ready = false;
	_all_ready_time = 0;
}

void SvWaitScene::HandleNewCl(const client_t & new_cl)
{
	Packet send_packet;
	send_packet << UNSIGNED_SHORT(SV_TO_CL_STATE);
	send_packet << SV_TO_CL_WAITING_STATE;
	SafeSend(new_cl.id, send_packet);

	// 새로운 client를 위해 기존 player들의 set을 알려줍니다.
	for(int i = 0; i < SV_MAX_NR_PLAYER; ++i)
	{
		if(_set[i].mask)
		{
			send_packet.Clear();
			send_packet << UNSIGNED_SHORT(SV_TO_CL_CL_SET)
						<< _set[i].cl_id
						<< UNSIGNED_SHORT(_player_map[_set[i].cl_id].runner_id)
						<< i;
			SafeSend(new_cl.id, send_packet);
		}
	}

	if(_player_map.Size() < SV_MAX_NR_PLAYER)
	{
		player_t new_player(false,
							static_cast<runner_id_t>(rand() % RANDOM_RUNNER),
							-1);

		// 새로온 client를 위해 자리를 배정합니다.
		int i;
		for(i = 0; i < SV_MAX_NR_PLAYER; ++i)
		{
			if(!_set[i].mask)
			{
				_set[i].cl_id = new_cl.id;		
				new_player.set_pos = i;
				_set[i].mask = true;
				break;
			}
		}
		assert(i < SV_MAX_NR_PLAYER);

		_player_map[new_cl.id] = new_player;

		send_packet.Clear();
		send_packet << UNSIGNED_SHORT(SV_TO_CL_CL_SET)
					<< new_cl.id
					<< UNSIGNED_SHORT(new_player.runner_id)
					<< i;

		SafeSend(new_cl.id, send_packet);
		SafeSendToAll(send_packet);
	}
	else if(_player_map.Size() < SV_MAX_NR_CL)
	{
		// player가 꽉찼으므로 관전만 가능합니다.
		_observers.push_back(new_cl.id);
	}
	else
	{
		// 최대 client를 넘어가므로 연결을 끊습니다.
		DisconnectClient(new_cl.id);
	}
}

void SvWaitScene::HandleGoneCl(const client_t & gone_cl)
{
	int pos = -1;
	
	player_map_t::Iter iter;
	if(_player_map.Find(gone_cl.id, &iter))
	{
		// player map에 있는 player라면
		pos = (*iter).GetEl().set_pos;
		_set[pos].mask = false;
		_player_map.Erase(iter);
	}
	else
	{
		// player map에 없으면 observer라는 뜻!
		_observers.remove_if([gone_cl] (ID id) {return id == gone_cl.id;});
	}

	if(sv_context.clients.Size() > SV_MAX_NR_PLAYER)
	{
		// 이 경우에는 나간 플레이어가 발판을 가지고 있을 경우
		// 나머지 guest 중에 한 명이 그 발판을 받을 수 있습니다.		
		if(pos >= 0)
		{
			// 나간 플레이어가 발판을 가지고 있을 경우
			// 임의의 guest가 플레이어로 접속할 수 있게 합니다.
			int rest = (sv_context.clients.Size() - 1) - SV_MAX_NR_PLAYER;
			int nth = rand() % rest;
			int count = 0;
			for(auto it = _observers.begin(); it != _observers.end(); ++it)
			{
				if(count++ == nth)
				{
					player_t new_player(false,
										static_cast<runner_id_t>(rand() % NR_RUNNER),
										pos);
					_set[pos].mask = true;
					_set[pos].cl_id = *it;
					_player_map[*it] = new_player;
					_observers.erase(it);
					break;
				}
			}
		}
	}

	if(CheckAllReady()) SwitchToPlayScene();
}

void SvWaitScene::HandleRecvPacket(const client_t & cl,
									  cl_to_sv_t header,
									  Packet & recv_packet)
{
	switch(header)
	{
	case CL_TO_SV_EVENT:
		HandleClEvent(cl, recv_packet);
		break;

	default:
		LeaveLog(L"SvWaitScene : HandleRecvPacket : 알 수 없는 header(%d)", header);
	}
}

void SvWaitScene::Go()
{
	if(_all_ready && _all_ready_time + 3000 <= sv_present_time)
	{
		SwitchToPlayScene();
	}
}

void SvWaitScene::SendClSet(ID cl_id, int pos)
{
	Packet send_packet;
	send_packet << UNSIGNED_SHORT(SV_TO_CL_CL_SET)
				<< cl_id
				<< UNSIGNED_SHORT(_player_map[cl_id].runner_id)
				<< pos;
	SafeSendToAll(send_packet);
}

void SvWaitScene::HandleMovingEvent(ID cl_id, int set_pos, int delta)
{
	int next_pos = set_pos + delta;
	for(; 0 <= next_pos && next_pos < SV_MAX_NR_PLAYER; next_pos += delta)
	{
		if(!_set[next_pos].mask) break;
	}
	if(0 <= next_pos && next_pos < SV_MAX_NR_PLAYER)
	{
		_set[set_pos].mask = false;
		_set[next_pos].mask = true;
		_set[next_pos].cl_id = cl_id;		
		_player_map[cl_id].set_pos = next_pos;
		SendClSet(cl_id, next_pos);
	}
}

bool SvWaitScene::CheckAllReady()
{
	for(auto iter = _player_map.Begin();
		iter != _player_map.End();
		++iter)
	{
		if(!(*iter).GetEl().ready) return false;
	}
	return true;
}

void SvWaitScene::HandleClEvent(const client_t & cl, Packet & recv_packet)
{
	if(find_if(	_observers.begin(), _observers.end(),
				[cl] (ID id) { return id == cl.id; }) != _observers.end())
	{
		return;	// 관전자의 event는 무시합니다.
	}

	player_t & player = _player_map[cl.id];

	header_t event;
	if(!(recv_packet >> event)) return;

	header_t key_state;

	switch(event)
	{
	case CL_TO_SV_EVENT_UP:
		if(player.ready) return;
		if(!(recv_packet >> key_state)) return;
		if(key_state == KEY_DOWN)
		{
			if(player.runner_id == 0)
				player.runner_id = static_cast<runner_id_t>(NR_RUNNER - 1);
			else
				player.runner_id = static_cast<runner_id_t>(player.runner_id - 1);
			SendClSet(cl.id, player.set_pos);
		}
		break;

	case CL_TO_SV_EVENT_DOWN:
		if(player.ready) return;
		if(!(recv_packet >> key_state)) return;
		if(key_state == KEY_DOWN)
		{
			player.runner_id = static_cast<runner_id_t>(player.runner_id + 1);
			if(player.runner_id >= NR_RUNNER)
				player.runner_id = static_cast<runner_id_t>(0);
			SendClSet(cl.id, player.set_pos);
		}
		break;

	case CL_TO_SV_EVENT_LEFT:
		if(!(recv_packet >> key_state)) return;
		if(key_state == KEY_DOWN)
			HandleMovingEvent(cl.id, player.set_pos, -1);
		break;

	case CL_TO_SV_EVENT_RIGHT:
		if(!(recv_packet >> key_state)) return;
		if(key_state == KEY_DOWN)
			HandleMovingEvent(cl.id, player.set_pos, 1);
		break;

	case CL_TO_SV_EVENT_READY:
		if(!player.ready)
#ifndef _DEBUG
		if(_player_map.Size() >= 2) {
#endif
		{
			player.ready = true;
			Packet send_packet;
			send_packet << UNSIGNED_SHORT(SV_TO_CL_PLAYER_READY)
						<< cl.id;
			SafeSendToAll(send_packet);

			if(CheckAllReady())
			{
				_all_ready = true;
				_all_ready_time = sv_present_time;

				ChangeRandomRunner();
			}
		}
#ifndef _DEBUG
		}
#endif
		break;

	case CL_TO_SV_EVENT_Q:
	case CL_TO_SV_EVENT_W:
	case CL_TO_SV_EVENT_E:
	case CL_TO_SV_EVENT_R:
		break;
		
	default:
		LeaveLog(L"SvWaitScene : HandleClEvent : 알 수 없는 event(%u)", event);
	}
}

void SvWaitScene::SwitchToPlayScene()
{
	Packet send_packet;
	send_packet << _player_map.Size();
	for(auto iter = _player_map.Begin(); iter != _player_map.End(); ++iter)
	{
		send_packet << (*iter).GetKey();
		send_packet << UNSIGNED_SHORT((*iter).GetEl().runner_id);
		send_packet << (*iter).GetEl().set_pos;
	}
	SavePacket(send_packet);

	SetNextScene(SV_PLAYING_SCENE);
}

void SvWaitScene::ChangeRandomRunner()
{
	for(auto iter = _player_map.Begin(); iter != _player_map.End(); ++iter)
	{
		ID			cl_id = (*iter).GetKey();
		player_t &	player = (*iter).GetEl();
		if(player.runner_id == RANDOM_RUNNER)
		{
			player.runner_id = static_cast<runner_id_t>(rand() % RANDOM_RUNNER);
			
			Packet send_packet;
			send_packet	<< UNSIGNED_SHORT(SV_TO_CL_CL_SET)
						<< cl_id
						<< UNSIGNED_SHORT(_player_map[cl_id].runner_id)
						<< player.set_pos;
			SafeSendToAll(send_packet);
		}
	}
}