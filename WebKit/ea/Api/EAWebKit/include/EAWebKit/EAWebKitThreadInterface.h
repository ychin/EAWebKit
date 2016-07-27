/*
Copyright (C) 2011, 2012, 2013, 2014 Electronic Arts, Inc.  All rights reserved.

Redistribution and use in source and binary forms, with or without
modification, are permitted provided that the following conditions
are met:

1.  Redistributions of source code must retain the above copyright
    notice, this list of conditions and the following disclaimer.
2.  Redistributions in binary form must reproduce the above copyright
    notice, this list of conditions and the following disclaimer in the
    documentation and/or other materials provided with the distribution.
3.  Neither the name of Electronic Arts, Inc. ("EA") nor the names of
    its contributors may be used to endorse or promote products derived
    from this software without specific prior written permission.

THIS SOFTWARE IS PROVIDED BY ELECTRONIC ARTS AND ITS CONTRIBUTORS "AS IS" AND ANY
EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE IMPLIED
WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE ARE
DISCLAIMED. IN NO EVENT SHALL ELECTRONIC ARTS OR ITS CONTRIBUTORS BE LIABLE FOR ANY
DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES
(INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES;
LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND
ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT
(INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE OF
THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
*/

///////////////////////////////////////////////////////////////////////////////
// EAWebkitThreadInterface.h
// By Arpit Baldeva 
// Maintained by EAWebKit Team
///////////////////////////////////////////////////////////////////////////////

///////////////////////////////////////////////////////////////////////////////
//
// This interface exposes a mechanism for the applications to allow thread creation when using EAWebKit. 
// Some HTML5 features may not work correctly without it. Note that EAWebKit API remains single threaded. 
//
///////////////////////////////////////////////////////////////////////////////

#ifndef EAWEBKIT_EAWEBKITTHREADINTERFACE_H
#define EAWEBKIT_EAWEBKITTHREADINTERFACE_H

#include <EABase/eabase.h>
#include "EAWebKitConfig.h"

namespace EA
{
namespace WebKit
{
	
typedef uint32_t ThreadId; //We use uint32_t instead of say uint64_t/uintptr_t because internal threading system(WTF::ThreadIdentifier) uses that.
typedef void (*ThreadFunc)(void* argument);

const ThreadId kThreadIdInvalid = 0;


class IMutex
{
public:
	virtual ~IMutex(){}

	virtual void Lock() = 0;
	virtual bool TryLock() = 0;
	virtual void Unlock() = 0;
};

class IThreadCondition
{
public:
	virtual ~IThreadCondition(){}

	virtual void Wait(IMutex* mutex) = 0;
	virtual bool TimedWait(IMutex*, double relativeTimeMS) = 0;
	virtual void Signal(bool broadcast) = 0;
};

class IThreadLocalStorage
{
public: 
	virtual ~IThreadLocalStorage() {}

	virtual void* GetValue() = 0;
	virtual bool SetValue(void* pData) = 0;
};

class IThread
{
public:
	virtual ~IThread() {}

	virtual ThreadId Begin(ThreadFunc,void* pThreadContext=NULL, void* pUserdata=NULL) = 0;
	virtual void WaitForEnd(intptr_t* result) = 0;
	virtual void SetName(const char* pName) = 0;
};

class IThreadSystem
{
public:
	virtual ~IThreadSystem() {}

	virtual bool Initialize() = 0;
	virtual bool Shutdown() = 0;

	// Would rather call it CreateMutex than CreateAMutex but it doesn't play nice with some windows headers and would
	// rather rename API than resorting to #undef tricks.
	virtual IMutex* CreateAMutex() = 0;
	virtual void DestroyAMutex(IMutex*) = 0;

	virtual IThreadCondition* CreateAThreadCondition() = 0;
	virtual void DestroyAThreadCondition(IThreadCondition*) = 0;

	virtual IThreadLocalStorage* CreateAThreadLocalStorage() = 0;
	virtual void DestroyAThreadLocalStorage(IThreadLocalStorage*) = 0;

	virtual IThread* CreateAThread() = 0;
	virtual void DestroyAThread(IThread*) = 0;

	virtual void ScheduleWork(EA::WebKit::ThreadFunc doWork, void* workContext) = 0;
	virtual ThreadId CurrentThreadId() = 0;
	virtual bool IsMainThread() = 0;
	virtual void YieldThread() = 0;
	virtual void SleepThread(uint32_t ms) = 0;
};


void SetThreadSystem(IThreadSystem*);
IThreadSystem* GetThreadSystem();

void ThreadCleanupTick();
}
}


#endif // EAWEBKIT_EAWEBKITTHREADINTERFACE_H
