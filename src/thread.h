//------------------------------------------------------------------------
// File    : thread.hpp
// Author  : David Poon
// Written : 6 May 2001
// 
// WIN32 Thread class.
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

#ifndef WIN32_THREAD_CLASS_HPP
#define WIN32_THREAD_CLASS_HPP

#include <windows.h>
#include <process.h>

//------------------------------------------------------------------------
// Class   : Mutex
// Extends : none
//
// A Mutex allows threads mutually exclusive access to a resource.
//------------------------------------------------------------------------
class opzMutex
{
public:
	opzMutex() {InitializeCriticalSection(&_mutex);}
	~opzMutex() {DeleteCriticalSection(&_mutex);}

	void Acquire() {EnterCriticalSection(&_mutex);}
	void Release() {LeaveCriticalSection(&_mutex);}

private:
	CRITICAL_SECTION _mutex;

	opzMutex &operator=(const opzMutex &other);	//방지
	opzMutex(const opzMutex &other);			//방지
};

//------------------------------------------------------------------------
// Class   : Lock
// Extends : none
//
// A Lock provides a safe way to acquire and release a Mutex. The Mutex 
// is acquired when the Lock it created. The Mutex is released when the
// Lock goes out of scope.
//------------------------------------------------------------------------
class opzLock
{
public:
	opzLock(opzMutex &mutex) : _mutex(mutex) {_mutex.Acquire();}
	~opzLock() {_mutex.Release();}

private:
	opzMutex &_mutex;

	opzLock &operator=(const opzLock &other);	//방지
	opzLock(const opzLock &other);				//방지
};

//------------------------------------------------------------------------
// Class   : Win32Thread --> opzThread
// Extends : none
//
// WIN32 thread class. The Win32Thread is always created in a suspended
// state. The thread is not running until start() is called. If zero is
// used as the stack size of the new thread, then Windows will use the
// stack size of the main thread.
//
// To create your own thread, subclass Win32Thread and provide an
// implementation for the run() method. If you want to give other threads
// the ability to cleanly shutdown your thread (recommended), then your
// thread's run() method should periodically call canRun() to check if 
// another thread has made a requested to shutdown your thread. canRun()
// will return false if another thread has requested that your thread
// shutdown.
//------------------------------------------------------------------------
class opzThread
{
public:
	opzThread(unsigned int stackSize = 0);
	virtual ~opzThread();

	unsigned int	GetThreadId() const;
	void			Begin();
	void			Join();

	void			Resume();
	void			Suspend();
	
protected:
	virtual void	Run() = 0;
	virtual void	EndProcess() { }	//이 함수는 Run 쓰레드가 끝났을 때 호출됩니다.

private:
	static unsigned int __stdcall ThreadFunc(void *args);
	
	HANDLE			_hThread;
	unsigned int	_threadId;
	volatile bool	_suspended;
};

#define	LOCK(x)	opzLock __lock(x)

#endif
