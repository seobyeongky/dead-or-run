#include "play_scene.h"
#include "res.h"
#include "util.h"
#include "play_scene_layout.h"

bool PlayScene::HandleClientMsg(client_msg_t &msg)
{
	switch(msg.type)
	{
	case CLMSG_DISCONNECTED:
		_chat_box.AddAlarmMsg(L"서버와의 접속이 끊어졌습니다.");
		break;

	case CLMSG_CLIENT_INTRODUCTION:
		HandleNewClient(msg);
		break;

	case CLMSG_CLIENT_GONE:
		HandleGoneClient(msg);
		break;

	case CLMSG_PACKET_RECV:
		HandleRecvPacket(msg.packet);
		break;

	case CLMSG_UNEXPECTED_ERROR:
		ErrorMsg(L"OPZNET client service : 예상치 못한 오류! (%ls)", msg.error_msg);
		break;
	}
	return true;
}

void PlayScene::HandleNewClient(client_msg_t & msg)
{
	player_t new_player(msg.client_info.name,
						PopColor(),
						false,
						nullptr,
						false);
	_player_map.Insert(msg.client_info.id, new_player);
	wstring str = msg.client_info.name + L"님께서 입장하셨습니다.";
	_chat_box.AddAlarmMsg(str);
}

void PlayScene::HandleGoneClient(client_msg_t & msg)
{
	SMap<ID, player_t>::Iter iter;
	_player_map.Find(msg.client_info.id, &iter);
	Runner * runner = (*iter).GetEl().runner;
	if(runner != nullptr)
	{
		for(auto it = _hair_map.Begin(); it != _hair_map.End();)
		{
			if((*it).GetEl()->GetOwner() == runner)
			{
				_object_palette.EraseObject((*it).GetEl());
				delete (*it).GetEl();
				it = _hair_map.Erase(it);
			}
			else ++it;
		}
		_object_palette.EraseObject(runner);
		_runner_factory.DeleteRunner(runner);
	}
	_player_map.Erase(iter);
	wstring str = msg.client_info.name + L"님께서 떠나셨습니다.";
	_chat_box.AddChatMsg(Color(255, 255, 255), L"알림",
				Color(200, 200, 200), str);
}

void PlayScene::HandleSvState(Packet & recv_packet)
{
	header_t state;
	if(!(recv_packet >> state)) return;

	_sv_state = static_cast<sv_to_cl_state_t>(state);

	if(state == SV_TO_CL_WAITING_STATE)
	{
		for(auto iter = _player_map.Begin(); iter != _player_map.End(); ++iter)
		{
			(*iter).GetEl().alive = false;
			(*iter).GetEl().ready = false;
		}
		_min_y = _cur_min_y = 0.f;
		_dark_bg.setFillColor(Color(0, 0, 0, 255));
		_char_desc.Reset();
		_camera.SetZero();
		_object_palette.Clear();	
		_tile_map.Reset();

		for(auto iter = _cactus_deq.begin(); iter != _cactus_deq.end(); ++iter)
			delete *iter;
		_cactus_deq.clear();

		for(auto iter = _object_map.Begin(); iter != _object_map.End(); ++iter)
			delete (*iter).GetEl();
		_object_map.Clear();

		for(auto iter = _hair_map.Begin(); iter != _hair_map.End(); ++iter)
			delete (*iter).GetEl();
		_hair_map.Clear();

		for(auto iter = _smoke_map.Begin(); iter != _smoke_map.End(); ++iter)
			delete (*iter).GetEl();
		_smoke_map.Clear();

		for(auto iter = _portal_map.Begin(); iter != _portal_map.End(); ++iter)
			delete (*iter).GetEl();
		_portal_map.Clear();

		for (auto iter = _effect_list.begin(); iter != _effect_list.end(); ++iter)
			delete *iter;
		_effect_list.clear();

		for(auto iter = _player_map.Begin(); iter != _player_map.End(); ++iter)
		{
			(*iter).GetEl().runner = nullptr;
		}
		_runner_factory.Clear();
		_inventory_box.Reset();

		_all_ready = false;
	}
	else if(state == SV_TO_CL_READY_STATE)
	{
		_big_msg.Show(L"준비");
		if (_me->runner != nullptr)
		{
			runner_id_t rid = _me->runner->GetId();
			auto & info = CHARACTER(rid);
			_skill_box.Reset(info.q_skill, info.w_skill, info.e_skill);
		}
	}
	else if(state == SV_TO_CL_PLAYING_STATE)
	{
		_big_msg.Show(L"시작!");
	}
}

void PlayScene::HandleChatMsg(Packet & recv_packet)
{
	ID cl_id;
	wstring wstr;
	if(!(recv_packet >> cl_id >> wstr)) { LeaveLog(L"CL : HandleChatMsg : packet 오염");  return; }
	
	SMap<ID, player_t>::Iter iter;
	if(!_player_map.Find(cl_id, &iter)) { LeaveLog(L"CL : HandleChatMsg : %u번 ID 존재하지 않음", cl_id); return; }
	player_t & player = (*iter).GetEl();
	
	wstring name = player.name;

	if(_sv_state == SV_TO_CL_PLAYING_STATE)
		if(player.runner && player.runner->GetState() == DEAD_STATE)
			name += L"(사망)";

	_chat_box.AddChatMsg(	(*iter).GetEl().color, name,
							Color(235, 235, 255), wstr);

	sfx_mgr->Play(SFX_LEAVE_CHAT);
}

void PlayScene::HandleInfoMsg(Packet & recv_packet)
{
	wstring info_str;
	if(!(recv_packet >> info_str)) return;

	_chat_box.AddInfoMsg(info_str);

	sfx_mgr->Play(SFX_INFO);
}

void PlayScene::HandleClSetMsg(Packet & recv_packet)
{
	ID			cl_id;
	header_t	__runner_id;
	runner_id_t	runner_id;
	int			pos;
	
	if(!(recv_packet >> cl_id >> __runner_id >> pos)) return;
	runner_id = static_cast<runner_id_t>(__runner_id);
	player_map_t::Iter iter;
	if(!_player_map.Find(cl_id, &iter)) return;

	sfx_mgr->Play(SFX_CL_SET);
	
	player_t & player = (*iter).GetEl();
	player.alive = true;
	
	Runner * & runner = (*iter).GetEl().runner;
	if(runner != nullptr)
	{
		_object_palette.EraseObject(runner);
		_runner_factory.DeleteRunner(runner);
	}
	runner = _runner_factory.CreateRunner(player.name.c_str(), runner_id, player.color);
	if(runner == nullptr) return;
	_object_palette.InsertObject(runner);

	runner->OnSetState(RUNNING_STATE);
	runner->SetPos(
		GetXPosByTileUnitPos(pos),
		0.f);

	if(cl_id == _my_id) _char_desc.Update(runner);
}

void PlayScene::HandleClReadyMsg(Packet & recv_packet)
{
	ID cl_id;
	if(!(recv_packet >> cl_id)) return;
	player_map_t::Iter iter;
	if(!(_player_map.Find(cl_id, &iter))) return;
	(*iter).GetEl().ready = true;
	sfx_mgr->Play(SFX_READY);

	{
		bool all_ready = true;
		for(auto iter = _player_map.Begin();
			iter != _player_map.End();
			++iter)
		{
			player_t & player = (*iter).GetEl();
			if(player.runner && !player.ready)
			{
				all_ready = false;
				break;
			}
		}
		if(all_ready)
		{
			_all_ready = true;
			_all_ready_accum_time = 1000;
			_all_ready_cnt = 3;
		}
	}
}

void PlayScene::HandleBasicInfo(Packet & recv_packet)
{
	size_t nr_runner;
	if(!(recv_packet >> nr_runner)) return;

	for(unsigned int i = 0; i < nr_runner; ++i)
	{
		ID cl_id;
		pos_t pos;
		float dir;
		float speed;
		float health;
		header_t state;
		if(!(recv_packet >>
			cl_id >> pos.x >> pos.y >> dir >> speed >> health >> state)) continue;
		player_map_t::Iter iter;
		if(!_player_map.Find(cl_id, &iter)) continue;
		Runner * runner = (*iter).GetEl().runner;
		if(runner == nullptr) continue;
		runner->SetPos(pos.x, pos.y);
		runner->SetDir(dir);
		runner->SetSpeed(speed);
		runner->OnSetState(static_cast<runner_state_t>(state));

		if(cl_id == _my_id)
		{
			_health = static_cast<int>(health);
			WCHAR buf[32];
			swprintf_s(buf, L"%d", _health);
			_hp_text.setString(buf);
			_hp_text_outline.setString(buf);
		}
	}

	size_t nr_hair;
	if(!(recv_packet >> nr_hair)) return;
	for(size_t i = 0U; i < nr_hair; ++i)
	{
		object_id_t id;
		pos_t		pos;
		if(!(recv_packet >> id >> pos.x >> pos.y)) return;
		hair_map_t::Iter iter;
		if(!_hair_map.Find(id, &iter)) return;
		Hair * hair = (*iter).GetEl();
		hair->SetPos(pos);
	}
}

void PlayScene::HandleTileRow(Packet & recv_packet)
{
	tile_row_t	new_row;
	for(int i = 0; i < NR_TILE_IN_ROW; ++i)
	{
		header_t tile;
		if(!(recv_packet >> tile)) return;
		new_row.tiles[i] = static_cast<tile_t>(tile);
		if(new_row.tiles[i] == CACTUS_TILE)
		{
			pos_t pos(	GetXFromTileUnit(i),
						_tile_map.GetCurY());
			Cactus * new_cactus = new Cactus(_cactus_sprite, pos);
			_cactus_deq.push_back(new_cactus);
			_object_palette.InsertObject(new_cactus);
		}
	}
	_tile_map.Add(new_row);
}

void PlayScene::HandleGameContext(Packet & recv_packet)
{
	size_t nr_player;
	if(!(recv_packet >> nr_player)) return;
	for(size_t i = 0U; i < nr_player; ++i)
	{
		ID cl_id;
		runner_id_t runner_id;
		header_t __runner_id;
		if(!(recv_packet >> cl_id >> __runner_id)) return;
		runner_id = static_cast<runner_id_t>(__runner_id);
		player_map_t::Iter iter;
		if(!(_player_map.Find(cl_id, &iter))) { LeaveLog(L"PlayScene : HandleGameContext : 예외 발생... 패킷 역전 가능성"); return; }
		player_t & player = (*iter).GetEl();
		player.runner = _runner_factory.CreateRunner(player.name.c_str(), runner_id, player.color);
		_object_palette.InsertObject(player.runner);
		player.alive = true;
	}
	
	_tile_map.Reset();
	size_t nr_accum_tile_row;
	if(!(recv_packet >> nr_accum_tile_row)) return;
	_tile_map.SetNrRow(nr_accum_tile_row);
	size_t nr_tile_row;
	if(!(recv_packet >> nr_tile_row)) return;
	for(size_t i = 0U; i < nr_tile_row; ++i) HandleTileRow(recv_packet);
}

void PlayScene::HandleDeadRunner(Packet & recv_packet)
{
	{
		// runner를 죽도록 적절하게 처리
		ID cl_id;
		if(!(recv_packet >> cl_id)) return;
		player_map_t::Iter iter;
		if(!_player_map.Find(cl_id, &iter)) return;
		player_t & player = (*iter).GetEl();
		player.alive = false;
		if(player.runner)
		{
			player.runner->OnSetState(DEAD_STATE);
		}
	}

	{
		// 이 사람이 죽고 마지막으로 사람이 남으면 소리치게 하는 논리
		size_t cnt = 0;
		player_map_t::Iter iter, alive_iter;
		for(iter = _player_map.Begin(); iter != _player_map.End(); ++iter)
		{
			Runner * runner = (*iter).GetEl().runner;
			if(!runner) continue;
			if(runner->GetState() != DEAD_STATE)
			{
				alive_iter = iter;
				++cnt;
			}
		}
		if(cnt == 1)
			(*alive_iter).GetEl().runner->Yell();
	}
}

void PlayScene::HandleNewObject(Packet & recv_packet)
{
	object_id_t object_id;
	if(!(recv_packet >> object_id)) return;
	header_t type_in_packet;
	if(!(recv_packet >> type_in_packet)) return;
	object_type_id_t type(static_cast<object_type_id_t>(type_in_packet));
	switch(type)
	{
	case OBJECT_ITEM:
		{
			header_t item_id_in_packet;
			if(!(recv_packet >> item_id_in_packet)) return;
			item_id_t item_id(static_cast<item_id_t>(item_id_in_packet));
			pos_t pos;
			if(!(recv_packet >> pos.x >> pos.y)) return;
			Item * new_item = new Item(item_id, pos);
			_object_map.Insert(object_id, new_item);
			_object_palette.InsertObject(new_item);
		}
		break;

	case OBJECT_STONE:
		{
			ID			owner_id;
			pos_t		pos;
			vector_t	vcity;
			if(!(recv_packet	>> owner_id
								>> pos.x
								>> pos.y
								>> vcity.x
								>> vcity.y )) return;

			Stone * new_stone = new Stone(pos, vcity);
			
			_object_map.Insert(object_id, new_stone);
			_object_palette.InsertObject(new_stone);
			sfx_mgr->Play(SFX_STONE_THROW);
		}
		break;

	case OBJECT_HAIR:
		{
			ID			owner_id;
			pos_t		pos;
			vector_t	vcity;
			if(!(recv_packet	>> owner_id
								>> pos.x
								>> pos.y
								>> vcity.x
								>> vcity.y )) return;
			Hair * new_hair = new Hair();
			player_map_t::Iter iter;
			if(!_player_map.Find(owner_id, &iter))
			{
				// 허공에서 시작된 머리카락
				new_hair->SetFirstPos(pos);
			}
			else
			{
				// 머리에서 시작된 머리카락
				Runner * runner = (*iter).GetEl().runner;
				if(runner)
				{
					sfx_mgr->Play(SFX_HAIR_SHOT);
					new_hair->SetOwner(runner);
				}
				else
				{
					delete new_hair;
					return;
				}
			}
			
			new_hair->SetVcity(vcity);
			new_hair->SetPos(pos);
			_hair_map.Insert(object_id, new_hair);
			_object_palette.InsertObject(new_hair);
		}
		break;
		
	case OBJECT_SMOKE:
		{
			ID			owner_id;
			pos_t		pos;
			vector_t	vcity;
			if(!(recv_packet	>> owner_id
								>> pos.x
								>> pos.y
								>> vcity.x
								>> vcity.y) ) return;
			bool transparent = false;
			if(owner_id == _my_id) transparent = true;
			Smoke * new_smoke = new Smoke(pos, vcity, transparent);
			_smoke_map.Insert(object_id, new_smoke);
			sfx_mgr->Play(SFX_SMOKE);
		}
		break;

	case OBJECT_PORTAL:
		{
			ID			owner_id;
			pos_t		pos;
			float		dir;
			if (!(recv_packet	>> owner_id
								>> pos.x
								>> pos.y
								>> dir) ) return;
			
			Portal * new_portal = new Portal(pos, dir);
			_portal_map.Insert(object_id, new_portal);
			_object_palette.InsertObject(new_portal);
			sfx_mgr->Play(SFX_PORTAL_OPEN);
		}
		break;

	case OBJECT_MAGIC_BOOGER:
		{
			ID			owner_id;
			pos_t		pos;
			vector_t	vcity;
			if(!(recv_packet	>> owner_id
								>> pos.x
								>> pos.y
								>> vcity.x
								>> vcity.y )) return;

			MagicBooger * new_booger = new MagicBooger(pos, vcity);
			
			_object_map.Insert(object_id, new_booger);
			_object_palette.InsertObject(new_booger);
			sfx_mgr->Play(SFX_BOOGER_THROW);
		}
		break;

	default:
		ErrorMsg(L"PlayScene : HandleNewObject : 알 수 없는 object id(%d)", object_id);
	}
}

void PlayScene::HandleGameOver(Packet & recv_packet)
{
	header_t header;
	if(!(recv_packet >> header)) return;
	switch(static_cast<sv_to_cl_game_over_type_t>(header))
	{
	case SV_TO_CL_NO_WINNER:
		_big_msg.Show(L"게임 오버... 그런데 승자가 없습니다..?");
		break;

	case SV_TO_CL_THERE_IS_WINNER:
		{
			ID		winner_id;
			size_t	nr_accum_win;
			if(!(recv_packet >> winner_id >> nr_accum_win)) return;
			player_map_t::Iter iter;
			if(!(_player_map.Find(winner_id, &iter))) return;
			player_t & player = (*iter).GetEl();

			if(nr_accum_win >= 3)
			{
				switch((nr_accum_win - 3) % 4)
				{
				case 0:
					_big_msg.Show(L"%s님이 미쳐 날뛰고 있습니다!", player.name.c_str());
					sfx_mgr->Play(SFX_VOICE_RAMPAGE);
					break;

				case 1:
					_big_msg.Show(L"%s님을 멈추게 할 수 없습니다!", player.name.c_str());
					sfx_mgr->Play(SFX_VOICE_UNSTOPPABLE);
					break;

				case 2:
					_big_msg.Show(L"%s님이 사막을 지배하고 있습니다!", player.name.c_str());
					sfx_mgr->Play(SFX_VOICE_DOMINATING);
					break;

				case 3:
					_big_msg.Show(L"%s님은 전지전능합니다!", player.name.c_str());
					sfx_mgr->Play(SFX_VOICE_GODLIKE);
					break;
				}
			}
			else
			{
				if(winner_id == _my_id)
				{	
					_big_msg.Show(L"승리!");
					sfx_mgr->Play(SFX_WIN);
				}
				else
				{
					_big_msg.Show(L"%s님께서 승리하셨습니다",
						player.name.c_str());
					sfx_mgr->Play(SFX_LOSE);
				}
			}
		}
		break;
	}

	// #@@@@
}

void PlayScene::HandleObjectDead(Packet & recv_packet)
{
	object_id_t object_id;
	if(!(recv_packet >> object_id)) return;
	header_t object_type;
	if(!(recv_packet >> object_type)) return;
	switch(static_cast<object_id_t>(object_type))
	{
	case OBJECT_ITEM:
		{
			object_map_t::Iter iter;
			if(!_object_map.Find(object_id, &iter)) return;
			IObject * item = (*iter).GetEl();
			_object_palette.EraseObject(item);
			delete item;
			_object_map.Erase(iter);
		}
		break;

	case OBJECT_MAGIC_BOOGER:
	case OBJECT_STONE:
		{
			object_map_t::Iter iter;
			if(!_object_map.Find(object_id, &iter)) return;
			IObject * item = (*iter).GetEl();
			_object_palette.EraseObject(item);
			delete item;
			_object_map.Erase(iter);	
		}
		break;

	case OBJECT_HAIR:
		{
			hair_map_t::Iter iter;
			if(!_hair_map.Find(object_id, &iter)) return;
			Hair * hair = (*iter).GetEl();
			_object_palette.EraseObject(hair);
			delete hair;
			_hair_map.Erase(iter);
		}
		break;

	case OBJECT_SMOKE:
		{
			smoke_map_t::Iter iter;
			if(!_smoke_map.Find(object_id, &iter)) return;
			Smoke * smoke = (*iter).GetEl();
			delete smoke;
			_smoke_map.Erase(iter);
		}
		break;

	case OBJECT_PORTAL:
		{
			portal_map_t::Iter iter;
			if (!_portal_map.Find(object_id, &iter)) return;
			Portal * portal = (*iter).GetEl();
			_object_palette.EraseObject(portal);
			delete portal;
			_portal_map.Erase(iter);
		}
		break;

	default:
		ErrorMsg(L"PlayScene : HandleObjectDead : 알 수 없는 object type(%d)", object_type);
	}
}

void PlayScene::HandleAddedItem(Packet & recv_packet)
{
	header_t item_id_in_packet;
	if(!(recv_packet >> item_id_in_packet)) return;
	item_id_t item_id(static_cast<item_id_t>(item_id_in_packet));
	_inventory_box.AddItem(item_id);
	sfx_mgr->Play(SFX_GET);
}

void PlayScene::HandleErasedItem(Packet & recv_packet)
{
	// Do nothing
}

void PlayScene::HandleSvEvent(Packet & recv_packet)
{
	header_t packet_event_type;
	if(!(recv_packet >> packet_event_type)) return;
	sv_to_cl_event_t event_type(static_cast<sv_to_cl_event_t>(packet_event_type));
	switch(event_type)
	{
	case SV_TO_CL_RUNNER_COL_TO_CACTUS:
		{
			ID cl_id;
			if(!(recv_packet >> cl_id)) return;
			player_map_t::Iter iter;
			if(!_player_map.Find(cl_id, &iter)) return;
			Runner * runner = (*iter).GetEl().runner;
			if(runner == nullptr) return;
			runner->Hit();
		}
		break;
		
	case SV_TO_CL_HAIR_GOT_RUNNER:
		break;

	case SV_TO_CL_RUNNER_GOT_WATER:
		{
			ID cl_id;
			if(!(recv_packet >> cl_id)) return;
			player_map_t::Iter iter;
			if(!_player_map.Find(cl_id, &iter)) return;
			Runner * runner = (*iter).GetEl().runner;
			if(runner == nullptr) return;
			sfx_mgr->Play(SFX_WATER);
		}
		break;

	case SV_TO_CL_STONE_HIT_RUNNER:
		sfx_mgr->Play(SFX_HEADSHOT);
		break;

	case SV_TO_CL_PORTAL_PASS:
		sfx_mgr->Play(SFX_PORTAL_PASSING);
		break;

	case SV_TO_CL_GROUND_ATTACK:
		{
			pos_t pos;
			if (!(recv_packet	>> pos.x
								>> pos.y )) return;

			sfx_mgr->Play(SFX_GROUND_ATTACK);
			GroundAttack * new_effect = new GroundAttack(pos);
			_effect_list.push_back(new_effect);
			_object_palette.InsertObject(new_effect);
		}
		break;

	case SV_TO_CL_BOOGER_HIT_RUNNER:
		{
			ID cl_id;
			if (!(recv_packet >> cl_id)) return;
			player_map_t::Iter iter;
			if (_player_map.Find(cl_id, &iter))
			{
				Runner * runner = (*iter).GetEl().runner;
				if (runner)
					runner->Hit();
			}

			sfx_mgr->Play(SFX_BOOGER_HIT);
		}
		break;
	}
}

void PlayScene::HandleAcceptedSkill(Packet & recv_packet)
{
	sv_to_cl_skill_t stype;
	header_t __stype;
	if (!(recv_packet >> __stype)) return;
	stype = static_cast<sv_to_cl_skill_t>(__stype);
	switch (stype)
	{
	case SV_TO_CL_Q_SKILL_ACCEPTED:
		_skill_box.UseQSkill();
		break;

	case SV_TO_CL_W_SKILL_ACCEPTED:
		_skill_box.UseWSkill();
		break;

	case SV_TO_CL_E_SKILL_ACCEPTED:
		_skill_box.UseESkill();
		break;

	case SV_TO_CL_R_SKILL_ACCEPTED:
		break;
	}
}

void PlayScene::HandleRecvPacket(Packet & recv_packet)
{
	header_t header;
	if(!(recv_packet >> header)) { LeaveLog(L"CL : HandleRecvPacket : packet 오염 (header 읽기 실패)"); return; }

	switch(header)
	{
	case SV_TO_CL_STATE:	HandleSvState(recv_packet); break;
	case SV_TO_CL_CHAT:		HandleChatMsg(recv_packet); break;
	case SV_TO_CL_INFO:		HandleInfoMsg(recv_packet); break;
	case SV_TO_CL_CL_SET:	HandleClSetMsg(recv_packet); break;
	case SV_TO_CL_PLAYER_READY:	HandleClReadyMsg(recv_packet); break;
	case SV_TO_CL_BASIC_INFO:HandleBasicInfo(recv_packet);break;
	case SV_TO_CL_TILE_ROW:	HandleTileRow(recv_packet); break;
	case SV_TO_CL_GAME_CONTEXT: HandleGameContext(recv_packet); break;
	case SV_TO_CL_RUNNER_DEAD: HandleDeadRunner(recv_packet); break;
	case SV_TO_CL_NEW_OBJECT: HandleNewObject(recv_packet); break;
	case SV_TO_CL_GAME_OVER: HandleGameOver(recv_packet); break;
	case SV_TO_CL_OBJECT_DEAD: HandleObjectDead(recv_packet); break;
	case SV_TO_CL_ADD_ITEM: HandleAddedItem(recv_packet); break;
	case SV_TO_CL_ERASE_ITEM: HandleErasedItem(recv_packet); break;
	case SV_TO_CL_EVENT: HandleSvEvent(recv_packet); break;
	case SV_TO_CL_SKILL_ACCEPTED: HandleAcceptedSkill(recv_packet); break;
	default:
		ErrorMsg(L"PlayScene : HandleRecvPacket : 알 수 없는 header(%d)", header);
	}
}