/*OOPARTS OOPARTS OOPARTS OOPARTS OOPARTS OOPARTS OOPARTS
=========================================================
	sv_scene.h

	SvScene basic abstract class�� �����߽��ϴ�.
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
/*	server module�� client�� ���������� FSM model�� �����ϴ�.
	�׸��� FSM�� state�� �ش��ϴ� ���� SvScene class�Դϴ�. */

	friend class SvThread;

public:
	void				OnInit() {_next_scene = SV_INVALID_SCENE; Init(); }
	
	// member �������� �ʱ�ȭ��Ű�� �Լ��Դϴ�.
	// derived scene�� instance�� ����Ǳ� ������ �������Լ��� �ƴ� Init�Լ��� �ʱ�ȭ��Ű�ʽÿ�.
	virtual void		Init() = 0;

	// ���ο� client�� ������ ���Ŀ� ȣ��˴ϴ�.
	// server context���� �� client�� ���� ���� �����Դϴ�.
	virtual void		HandleNewCl(const client_t & new_cl) = 0;

	// � client�� �������� ���� �ڿ� ȣ��˴ϴ�.
	// server context���� ���� client�� ���� �ִ� �����Դϴ�.
	virtual void		HandleGoneCl(const client_t & gone_cl) = 0;

	virtual void		HandleRecvPacket(	const client_t & cl,
											cl_to_sv_t header,
											Packet & recv_packet) = 0;

	// �� �����Ӹ��� ȣ��Ǵ� �Լ��Դϴ�.
	virtual void		Go() = 0;

	sv_scene_id_t		GetNextScene() { return _next_scene; }

protected:
					 SvScene(sv_scene_id_t id) :
						 _id(id),
						 _next_scene(SV_INVALID_SCENE) { }
	virtual			~SvScene() { }

	void			 SetNextScene(sv_scene_id_t next_scene) {_next_scene = next_scene; }

	// �Ʒ� �� �Լ��� �̿��Ͽ� �����ϰ� scene�� scene���� ����� �� �ֽ��ϴ�.
	// SavePacket���� packet�� �����ϰ� LoadPacket���� ������ packet�� �ҷ��ɴϴ�.
	// packet�� ���� �����Դϴ�.
	void			 SavePacket(const Packet &packet) {_packet = packet;}
	void			 LoadPacket(Packet *dest) { *dest = _packet; }

private:
	sv_scene_id_t	_id;
	sv_scene_id_t	_next_scene;
	static Packet	_packet;

				SvScene(const SvScene &other);		// ���� ������
	SvScene &	operator = (const SvScene &other);	// ���� ������
};