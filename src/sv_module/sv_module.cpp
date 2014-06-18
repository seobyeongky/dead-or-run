#include "sv_module.h"
#include "../util.h"

void SvModule::S_Create() { if(!_instance) new SvModule(); }

void SvModule::S_Delete(){ if(_instance) delete _instance; _instance = nullptr; }

bool SvModule::S_Begin(const wstring & sv_name)
{
	SvThread & thread = _instance->_thread;

	thread.SetName(sv_name);
	thread.Begin();

	SvThread::bresult_t bresult;

	for(int i = 0; i < 10; ++i)
	{
		Sleep(250);

		if(thread.GetBeginningResult(&bresult))
		{
			if(bresult == SvThread::SUCCEEDED) return true;
			else { ErrorMsg(L"알 수 없는 오류");}

			return false;
		}
	}

	// 시간 초과 -> 실패
	return false;
}

SvModule * SvModule::_instance = nullptr;

SvModule::SvModule()
	: _thread()
{
	_instance = this;
}

SvModule::~SvModule()
{
	_thread.SetExitFlag();
	_thread.Join();

	sv_context.clients.Clear();
}