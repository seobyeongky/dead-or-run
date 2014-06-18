/*OOPARTS OOPARTS OOPARTS OOPARTS OOPARTS OOPARTS OOPARTS
=========================================================
	sv_module.h

	SvMoudle singleton class의 header file입니다.
=======================================================*/

#pragma once

#include "sv_thread.h"

class SvModule {
/*	server module singleton class입니다.	
	server를 관리합니다. */
public:
	static void S_Create();
	static void S_Delete();

	// server module을 실행합니다.
	// 반환값으로 성공여부를 알 수 있습니다.
	static bool S_Begin(const wstring & sv_name);
	static void S_End() {_instance->_thread.Suspend(); }

private:
	static SvModule *	_instance;

	SvThread 			_thread;

	SvModule();
	~SvModule();
};