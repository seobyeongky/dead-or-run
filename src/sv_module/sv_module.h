/*OOPARTS OOPARTS OOPARTS OOPARTS OOPARTS OOPARTS OOPARTS
=========================================================
	sv_module.h

	SvMoudle singleton class�� header file�Դϴ�.
=======================================================*/

#pragma once

#include "sv_thread.h"

class SvModule {
/*	server module singleton class�Դϴ�.	
	server�� �����մϴ�. */
public:
	static void S_Create();
	static void S_Delete();

	// server module�� �����մϴ�.
	// ��ȯ������ �������θ� �� �� �ֽ��ϴ�.
	static bool S_Begin(const wstring & sv_name);
	static void S_End() {_instance->_thread.Suspend(); }

private:
	static SvModule *	_instance;

	SvThread 			_thread;

	SvModule();
	~SvModule();
};