/*OOPARTS OOPARTS OOPARTS OOPARTS OOPARTS OOPARTS OOPARTS
=========================================================
	sv_scene.h

	SvScene basic abstract class를 정의했습니다.
=======================================================*/

#pragma once

#include "../internal.h"
#include "../protocol.h"
#include "sv_context.h"
#include "sv_res.h"

enum sv_scene_id_t
{
	SV_WAITING_SCENE = 0U,
	SV_PLAYING_SCENE = 1U,
	SV_NR_SCENE = 2U,
	SV_INVALID_SCENE = 3U
};

class SvScene {
/*	server module도 client와 마찬가지로 FSM model을 따릅니다.
	그리고 FSM의 state에 해당하는 것이 SvScene class입니다. */

	friend class SvThread;

public:
	void				OnInit() {_next_scene = SV_INVALID_SCENE; Init(); }
	
	// member 변수들을 초기화시키는 함수입니다.
	// derived scene은 instance가 재사용되기 때문에 생성자함수가 아닌 Init함수로 초기화시키십시오.
	virtual void		Init() = 0;

	// 새로운 client가 접속한 직후에 호출됩니다.
	// server context에는 새 client가 아직 없는 상태입니다.
	virtual void		HandleNewCl(const client_t & new_cl) = 0;

	// 어떤 client가 서버에서 나간 뒤에 호출됩니다.
	// server context에는 나간 client가 아직 있는 상태입니다.
	virtual void		HandleGoneCl(const client_t & gone_cl) = 0;

	virtual void		HandleRecvPacket(	const client_t & cl,
											cl_to_sv_t header,
											Packet & recv_packet) = 0;

	// 매 프레임마다 호출되는 함수입니다.
	virtual void		Go() = 0;

	sv_scene_id_t		GetNextScene() { return _next_scene; }

protected:
					 SvScene(sv_scene_id_t id) :
						 _id(id),
						 _next_scene(SV_INVALID_SCENE) { }
	virtual			~SvScene() { }

	void			 SetNextScene(sv_scene_id_t next_scene) {_next_scene = next_scene; }

	// 아래 두 함수를 이용하여 유일하게 scene과 scene간에 통신할 수 있습니다.
	// SavePacket으로 packet을 저장하고 LoadPacket으로 저장한 packet을 불러옵니다.
	// packet은 전역 범위입니다.
	void			 SavePacket(const Packet &packet) {_packet = packet;}
	void			 LoadPacket(Packet *dest) { *dest = _packet; }

private:
	sv_scene_id_t	_id;
	sv_scene_id_t	_next_scene;
	static Packet	_packet;

				SvScene(const SvScene &other);		// 복사 방지용
	SvScene &	operator = (const SvScene &other);	// 복사 방지용
};