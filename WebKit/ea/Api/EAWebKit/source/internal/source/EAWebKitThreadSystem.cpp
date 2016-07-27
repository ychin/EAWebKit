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
// EAWebKitThreadSystem.cpp
// By Arpit Baldeva
// Maintained by EAWebKit Team
///////////////////////////////////////////////////////////////////////////////

//////////////////////////////////////////////////////////////////////////
// The implementation of this file is very similar to the ThreadingNone.cpp 
// distributed with WebKit trunk.
//////////////////////////////////////////////////////////////////////////

#include "config.h"

#include <EAWebKit/EAWebKitThreadInterface.h>
#include <internal/include/EAWebKitAssert.h>

#include <wtf/MainThread.h>
#include "SharedTimer.h"

namespace EA
{
namespace WebKit
{
class DefaultMutex : public IMutex
{
public:
	friend class DefaultThreadCondition;
	DefaultMutex()
		: mLockCount(0)
	{

	}
	void Lock() 
	{
		++mLockCount;
	}

	bool TryLock()
	{
		if(mLockCount)
			return false;
		
		Lock();
		return true;
	}

	void Unlock()
	{
		--mLockCount;
	}
private:
	int mLockCount;
};

class DefaultThreadCondition : public IThreadCondition
{
public:
	void Wait(IMutex* mutex)
	{
		
	}

	bool TimedWait(IMutex* mutex, double relativeTimeMS)
	{
		return false;
	}

	void Signal(bool broadcast)
	{

	}
private:
};


class DefaultThread : public IThread
{
public:
	virtual ThreadId Begin(ThreadFunc,void* pThreadContext, void* pUserData)
	{
		EAW_ASSERT_MSG(false, "Using a feature that requires thread creation. This is not supported.");
		return (ThreadId)0;
	}
	virtual void WaitForEnd(intptr_t* result)
	{
	}
	virtual void SetName(const char* pName)
	{
	}
};

class DefaultThreadLocalStorage : public IThreadLocalStorage
{
public:
	DefaultThreadLocalStorage()
		: m_value(0)
	{

	}
	virtual void* GetValue() 
	{
		return m_value;
	}
	bool SetValue(void* pData)
	{
		m_value = pData;
		return false;
	}
private:
	void* m_value;

};

class DefaultThreadSystem : public IThreadSystem
{
public:
	virtual bool Initialize();
	virtual bool Shutdown();

	virtual IMutex* CreateAMutex();
	virtual void DestroyAMutex(IMutex*);

	virtual IThreadCondition* CreateAThreadCondition();
	virtual void DestroyAThreadCondition(IThreadCondition*);

	virtual IThreadLocalStorage* CreateAThreadLocalStorage();
	virtual void DestroyAThreadLocalStorage(IThreadLocalStorage*);

	virtual IThread* CreateAThread();
	virtual void DestroyAThread(IThread*);

	virtual void ScheduleWork(EA::WebKit::ThreadFunc doWork, void* workContext);
	virtual ThreadId CurrentThreadId();
	virtual bool IsMainThread();
	virtual void YieldThread();
	virtual void SleepThread(uint32_t ms);

};

bool DefaultThreadSystem::Initialize()
{
	return true;
}

bool DefaultThreadSystem::Shutdown()
{
	return true;
}

IMutex* DefaultThreadSystem::CreateAMutex()
{
	return new DefaultMutex();
}

void DefaultThreadSystem::DestroyAMutex(IMutex* mutex)
{
	delete mutex;
}

IThreadCondition* DefaultThreadSystem::CreateAThreadCondition()
{
	return new DefaultThreadCondition();
}

void DefaultThreadSystem::DestroyAThreadCondition(IThreadCondition* threadCondition)
{
	delete threadCondition;
}

IThreadLocalStorage* DefaultThreadSystem::CreateAThreadLocalStorage()
{
	return new DefaultThreadLocalStorage();
}

void DefaultThreadSystem::DestroyAThreadLocalStorage(IThreadLocalStorage* threadLocalStorage)
{
	delete threadLocalStorage;
}

IThread* DefaultThreadSystem::CreateAThread()
{
	return new DefaultThread();
}

void DefaultThreadSystem::DestroyAThread(IThread* pThread)
{
	delete pThread;
}

void DefaultThreadSystem::ScheduleWork(EA::WebKit::ThreadFunc doWork, void* workContext)
{
	doWork(workContext);
}
EA::WebKit::ThreadId DefaultThreadSystem::CurrentThreadId()
{
	//06/03/2013 - We are taking advantage of this implementation in some places to detect if we have default thread system. So if the implementation ever changes, make sure to adjust the other code!
	return EA::WebKit::kThreadIdInvalid;
}

bool DefaultThreadSystem::IsMainThread()
{
	return true;
}

void DefaultThreadSystem::YieldThread()
{
}

void DefaultThreadSystem::SleepThread(uint32_t ms)
{
}


static IThreadSystem* spThreadSystem = NULL; 
void SetThreadSystem(IThreadSystem* pThreadSystem)
{
	EAW_ASSERT_MSG(!spThreadSystem, "Thread system already exists !");
	spThreadSystem = pThreadSystem;
	spThreadSystem->Initialize();
}

IThreadSystem* GetThreadSystem()
{
	if(!spThreadSystem)
	{
		static DefaultThreadSystem defaultThreadSystem;
		spThreadSystem = &defaultThreadSystem;
	}
		
	return spThreadSystem;
}

void ThreadCleanupTick()
{
	WTF::dispatchFunctionsFromMainThread();
	WebCore::fireTimerIfNeeded();
}

}
}
