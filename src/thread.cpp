//------------------------------------------------------------------------
// File    : thread.cpp
// Author  : David Poon
// Written : 6 May 2001
// 
// Implementation file for the threading classes defined in thread.hpp
//
// Copyright (C) 2001 David Poon
//
// This program is free software; you can redistribute it and/or modify
// it under the terms of the GNU General Public License as published by
// the Free Software Foundation; either version 2 of the License, or
// (at your option) any later version.
//
// This program is distributed in the hope that it will be useful, but
// WITHOUT ANY WARRANTY; without even the implied warranty of 
// MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the GNU 
// General Public License for more details.
//
// You should have received a copy of the GNU General Public License 
// along with this program; if not, write to the Free Software Foundation
// Inc., 59 Temple Place, Suite 330, Boston, MA 02111-1307 USA
//------------------------------------------------------------------------

#include "thread.h"

#include <cstdio>
#include <cassert>

opzThread::opzThread(unsigned int stackSize)
	: _hThread(0),
	_threadId(0U),
	_suspended(true)
{
	_hThread = reinterpret_cast<HANDLE>(_beginthreadex(0, stackSize, 
		ThreadFunc, this, CREATE_SUSPENDED, &_threadId));

	assert(_hThread);
}

opzThread::~opzThread()
{
	TerminateThread(_hThread, 0);
	EndProcess();
	if (_hThread)
		CloseHandle(_hThread);
}

void opzThread::Join()
{
	WaitForSingleObject(_hThread, INFINITE);
}

void opzThread::Resume()
{
	int supendCount;
	do
	{
		//재실행이 됨을 보장한다.
		supendCount = ResumeThread(_hThread); 
	}while(supendCount > 0);

#ifdef _DEBUG
	printf("[Resume] ResumeThread pthread : %d\n", this);
#endif
}

void opzThread::Begin()
{
	Resume();
}

void opzThread::Suspend()
{
	SuspendThread(_hThread);
}

unsigned int opzThread::GetThreadId() const
{
	return _threadId;
}

unsigned int __stdcall opzThread::ThreadFunc(void *args)
{
	opzThread *pThread = reinterpret_cast<opzThread*>(args);
#ifdef _DEBUG
	printf("[ThreadFunc] pThread : %d\n", pThread);
#endif
	if (pThread)
		pThread->Run();

	pThread->EndProcess();

	_endthreadex(0);
	return 0;
} 