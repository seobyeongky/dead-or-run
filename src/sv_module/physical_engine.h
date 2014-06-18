/*OOPARTS OOPARTS OOPARTS OOPARTS OOPARTS OOPARTS OOPARTS
=========================================================
	physical_engine.h

	물리 계산 class의 header file입니다.
=======================================================*/

#pragma once

#include "sv_scene.h"
#include "sv_runner.h"
#include "sv_object.h"

#define POS_EPSILON		0.1f
#define TIME_EPSILON	0.01f

class PhysicalEngine {
/*	물리적 효과 (속도로 인한 위치계산, 충돌 후 효과) 계산 클래스입니다.
	약간 무거운 연산을 맡습니다. 
	SvPlayScene의 컨테이너의 포인터를 받아서 내부적으로 씁니다.
	runner의 경우 내부적으로 추가정보가 필요해서 처음에 초기화했다가
	거시기하게 관리합니다. runner가 사라지면 EraseRunner를 호출해줘야합니다.
	다른 거(선인장, 짱돌, 머리카락)는 안해도 됩니다.
	원래 item은 따로 쉽게 처리하려고 했는데, item먹는 것이 timing 싸움이 치열할 것 같아서
	좀 복잡하더라도 이 class에서 처리해야겠습니다.
	*/

public:
				 PhysicalEngine(ID * object_id_cnt_ptr);
				~PhysicalEngine();
		
	void		 Reset(	runner_map_t * runner_map_ptr,
						list<sv_cactus_t> * cactus_list,
						list<sv_item_t> * item_list,
						list<sv_stone_t> * stone_list,
						list<sv_hair_t> * hair_list,
						list<sv_portal_t> * portal_list,
						list<sv_booger_t> * booger_list);
	void		 EraseRunner(ID cl_id);
	void		 Update();

//	SvRunner *	 GetRunnerInsideOfCircle(const vector_t & origin, float radius);

	void		 SetRunnerAir(SvRunner * runner);
	void		 SetRunnerGround(SvRunner * runner);

private:
	struct taged_runner_t
	{
		ID			cl_id;
		float		rest_time;
		SvRunner *	runner;
		vector_t	rvcity;

		taged_runner_t(void) : cl_id(-1), rest_time(1.f), runner(nullptr) { }
		taged_runner_t(ID _cl_id, float _rest_time, SvRunner * _runner) :
			cl_id(_cl_id), rest_time(_rest_time), runner(_runner) { }
	};

	typedef SMap<ID, taged_runner_t> taged_runner_map_t;

	enum col_type_t
	{
		RUNNER_COL,
		LEFT_BOUNDARY_COL,
		RIGHT_BOUNDARY_COL,
		CACTUS_COL,
		ITEM_COL,
		STONE_COL,
		HAIR_COL,
		PORTAL_COL_WITH_RUNNER,
		PORTAL_COL_WITH_HAIR,
		BOOGER_COL
	};

	struct col_t
	{
		col_type_t			type;
		float				time;
		taged_runner_t	*	trunner1;
		taged_runner_t 	*	trunner2;
		sv_cactus_t *		cactus;
		sv_item_t *			item;
		sv_stone_t *		stone;
		sv_hair_t *			hair;
		sv_portal_t *		portal;
		sv_booger_t *		booger;

		col_t(void) : time(0.f), trunner1(), trunner2() { }
	};

private:
	float					_cur_time;	// [0.0f ~ 1.0f]
	taged_runner_map_t		_runners;
	list<sv_cactus_t> *		_cactus_list;
	list<sv_item_t> *		_item_list;
	list<sv_stone_t> *		_stone_list;
	list<sv_hair_t> *		_hair_list;
	list<sv_portal_t> *		_portal_list;
	list<sv_booger_t> *		_booger_list;
	
	ID *					_object_id_cnt_ptr;

	void	HandleRunnerCol(const col_t & c);
	void	HandleLeftBoundaryCol(const col_t & c);
	void	HandleRightBoundaryCol(const col_t & c);
	void	HandleCactusCol(const col_t & c);
	void	HandleItemCol(const col_t & c);
	void	HandleStoneCol(const col_t & c);
	void	HandleHairCol(const col_t & c);
	void	HandlePortalColWithRunner(const col_t & c);
	void	HandlePortalColWithHair(const col_t & c);
	void	HandleBoogerCol(const col_t & c);

	void	SyncToCurTime(taged_runner_t * trunner);
	bool	PredictColToLeftBoundary(taged_runner_t * trunner, float * time_ptr);
	bool	PredictColToRightBoundary(taged_runner_t * trunner, float * time_ptr);
	bool	PredictColToCactus(	taged_runner_t * trunner,
								sv_cactus_t * cactus,
								float * time_ptr);
	bool	PredictColToItem(	taged_runner_t * trunner,
								sv_item_t * item,
								float * time_ptr);
	bool	PredictColToStone(	taged_runner_t * trunner,
								sv_stone_t * stone,
								float * time_ptr);
	bool	PredictColToHair(	taged_runner_t * trunner,
								sv_hair_t * hair,
								float * time_ptr);
	bool	PredictColToPortal(	const pos_t & pos,
								const vector_t & vel,
								sv_portal_t * portal,
								float * time_ptr);
	bool	PredictColToBooger(	taged_runner_t * trunner,
								sv_booger_t * booger,
								float * time_ptr);
	bool	PredictCol(	taged_runner_t * trunner1,
						taged_runner_t * trunner2,
						float * time_ptr);
	bool	PredictFirstCol(col_t * col_ptr);
};