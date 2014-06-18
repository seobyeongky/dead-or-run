#include "sv_play_scene.h"
#include "../runner.h"
#include "../util.h"
#include "../play_scene_layout.h"

void SvPlayScene::Init()
{
	for(auto it = _runner_map.Begin(); it != _runner_map.End(); ++it)
		delete (*it).GetEl();
	_runner_map.Clear();

	Packet recv_packet;
	LoadPacket(&recv_packet);
	size_t nr_runner;
	recv_packet >> nr_runner;
	for(size_t i = 0U; i < nr_runner; ++i)
	{
		ID			cl_id;
		runner_id_t runner_id;
		header_t __runner_id;
		int			pos;
		recv_packet >> cl_id >> __runner_id >> pos;
		runner_id = static_cast<runner_id_t>(__runner_id);

		SvRunner * new_runner = nullptr;
		switch(runner_id)
		{
		case SEO:
			new_runner = new SvRunner(SEO, 15.f);
			break;

		case PARK:
			new_runner = new SvRunner(PARK,	20.f);
			break;

		case OH:
			new_runner = new SvRunner(OH, 15.f);
			break;

		case JEONG:
			new_runner = new SvRunner(JEONG, 15.f);
			break;

		case KIM:
			new_runner = new SvRunner(KIM, 15.f);
			break;

		case YONG:
			new_runner = new SvRunner(YONG, 25.f);
			break;

		case YOON:
			new_runner = new SvRunner(YOON, 20.f);
			break;

		default:
			ErrorMsg(L"SvPlayScene : Init : 알 수 없는 runner id(%u)", runner_id);
		}

		assert(new_runner != nullptr);
		if (new_runner != nullptr)
		{
			new_runner->SetPos(
				pos_t(static_cast<float>(OKAWARU_BOX_WIDTH * (pos - SV_MAX_NR_PLAYER / 2) + OKAWARU_BOX_WIDTH / 2),
				0.f) );
		
			_runner_map.Insert(cl_id, new_runner);
		}
	}

	Packet send_packet;
	send_packet << UNSIGNED_SHORT(SV_TO_CL_STATE);
	send_packet << UNSIGNED_SHORT(SV_TO_CL_READY_STATE);
	SafeSendToAll(send_packet);

	_begin_time = sv_present_time;
	_began = false;
	
	_p_engine.Reset(&_runner_map, &_cactus_list, &_item_list,
		&_stone_list, &_hair_list, &_portal_list, &_booger_list);

	_tile_row_deq.clear();
	_nr_tile_row = 0U;
	_last_water_row = 0U;

	_min_y = 0.f;

	_object_id_cnt = 0u;
	_cactus_list.clear();
	_water_list.clear();
	_item_list.clear();
	_stone_list.clear();
	_hair_list.clear();
	_smoke_list.clear();
	_portal_list.clear();
	_last_item_row = 0U;

	_result_send = false;
	_game_over = false;

	_accum_time = 0;
}
