/*
 * Copyright (C) 2007 Apple Inc. All rights reserved.
 * Copyright (C) 2007 Justin Haygood (jhaygood@reaktix.com)
 * Copyright (C) 2011, 2012, 2014 Electronic Arts, Inc. All rights reserved.
 * 
 * Redistribution and use in source and binary forms, with or without
 * modification, are permitted provided that the following conditions
 * are met:
 *
 * 1.  Redistributions of source code must retain the above copyright
 *     notice, this list of conditions and the following disclaimer. 
 * 2.  Redistributions in binary form must reproduce the above copyright
 *     notice, this list of conditions and the following disclaimer in the
 *     documentation and/or other materials provided with the distribution. 
 * 3.  Neither the name of Apple Computer, Inc. ("Apple") nor the names of
 *     its contributors may be used to endorse or promote products derived
 *     from this software without specific prior written permission. 
 *
 * THIS SOFTWARE IS PROVIDED BY APPLE AND ITS CONTRIBUTORS "AS IS" AND ANY
 * EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE IMPLIED
 * WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE ARE
 * DISCLAIMED. IN NO EVENT SHALL APPLE OR ITS CONTRIBUTORS BE LIABLE FOR ANY
 * DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES
 * (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES;
 * LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND
 * ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT
 * (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE OF
 * THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
 */

#include "config.h"
#include "Threading.h"

#include "DateMath.h"
#include "dtoa.h"
#include "CurrentTime.h"
#include "HashMap.h"
#include "MainThread.h"
#include "RandomNumberSeed.h"
#include <wtf/WTFThreadData.h>
#include <wtf/Assertions.h>

#include <../../../../../WebKit/ea/Api/EAWebKit/include/EAWebKit/EAWebKitThreadInterface.h>


namespace WTF {

COMPILE_ASSERT(sizeof(EA::WebKit::ThreadId) == sizeof(WTF::ThreadIdentifier), sizeof_EAWebKit_ThreadId_must_be_same_as_WTF_ThreadIdentifier); //Currently, both are typedef of uint32_t

static Mutex* atomicallyInitializedStaticMutex = NULL;

static Mutex* spthreadMapMutex = NULL;
static HashMap<EA::WebKit::ThreadId, EA::WebKit::IThread*>* spthreadMap = NULL;
// We "new" following in order to avoid running static destructors at the dll unload.
static Mutex& threadMapMutex()
{
	if(!spthreadMapMutex)
		spthreadMapMutex = new Mutex;
	
	return *spthreadMapMutex;
}

static HashMap<EA::WebKit::ThreadId, EA::WebKit::IThread*>& threadMap()
{
	if(!spthreadMap)
		spthreadMap = new HashMap<EA::WebKit::ThreadId, EA::WebKit::IThread*>();
	return *spthreadMap;
}

static EA::WebKit::IThread* threadForIdentifier(EA::WebKit::ThreadId id)
{
	if(!id)
		return NULL;

	MutexLocker locker(threadMapMutex());
	return threadMap().get(id);
}

static void clearThreadForIdentifier(EA::WebKit::ThreadId id)
{
	if(!id)
		return;

	MutexLocker locker(threadMapMutex());
	ASSERT(threadMap().contains(id));
	
	EA::WebKit::IThread* threadToDelete = threadMap().get(id);
	EA::WebKit::GetThreadSystem()->DestroyAThread(threadToDelete);

	threadMap().remove(id);
}

void initializeThreading() 
{ 
	if (!atomicallyInitializedStaticMutex) {
		// StringImpl::empty() does not construct its static string in a threadsafe fashion,
		// so ensure it has been initialized from here.
		StringImpl::empty();
		atomicallyInitializedStaticMutex = new Mutex;
		threadMapMutex();
		initializeRandomNumberGenerator();
		wtfThreadData();
		
		s_dtoaP5Mutex = new Mutex;
		initializeDates();
		
		ASSERT(EA::WebKit::GetThreadSystem()->IsMainThread());
	}
}

// Return ThreadIdentifier if successful or zero for failure.
ThreadIdentifier createThreadInternal(ThreadFunction entryPoint, void* data, const char*)
{
	EA::WebKit::IThread* createdThread = EA::WebKit::GetThreadSystem()->CreateAThread();
	if(createdThread)
	{
		EA::WebKit::ThreadId createdThreadId = createdThread->Begin(entryPoint, data,EA::WebKit::GetThreadSystem());
		if(createdThreadId)
		{
			MutexLocker locker(threadMapMutex());
			ASSERT(!threadMap().contains(createdThreadId)); // Make sure we don't have colliding thread ids.
			threadMap().add(createdThreadId, createdThread);
			return (ThreadIdentifier)createdThreadId;
		}
	}

	return 0;
}

void initializeCurrentThreadInternal(const char* name) 
{ 
	//Making sure that this is not called for the main thread.
	ASSERT(!EA::WebKit::GetThreadSystem()->IsMainThread());

	EA::WebKit::ThreadId currentThreadId = EA::WebKit::GetThreadSystem()->CurrentThreadId();
	EA::WebKit::IThread* currentThread = threadForIdentifier(currentThreadId);
	if(currentThread) //Any thread other than the main thread.
	{
		currentThread->SetName(name);
	}
}


// Return 0 on success, same as pthread_join().
int waitForThreadCompletion(ThreadIdentifier threadIdentifier)
{
	const EA::WebKit::ThreadId threadID = (EA::WebKit::ThreadId)threadIdentifier;
	EA::WebKit::IThread* theThread = threadForIdentifier(threadID);

	if(theThread)
	{
		intptr_t threadReturnValue = 0;
		theThread->WaitForEnd(&threadReturnValue);
		
		clearThreadForIdentifier(threadID);
		return 0;
	}

	return -1;

}

void detachThread(ThreadIdentifier threadIdentifier)
{
	const EA::WebKit::ThreadId threadID = (EA::WebKit::ThreadId)threadIdentifier;
	ASSERT(threadID);
	clearThreadForIdentifier(threadID);
}
/*
void waitForAllThreadsCompletion()
{
	while(1)
	{
		bool allThreadsCompleted = false;
		{
			MutexLocker locker(threadMapMutex());
			if(threadMap().isEmpty())
				allThreadsCompleted = true;
			
			if(allThreadsCompleted)
				break;
		}
		EA::WebKit::ThreadCleanupTick();
		EA::WebKit::GetThreadSystem()->SleepThread(1);
	}

}
*/

ThreadIdentifier currentThread()
{
	return (ThreadIdentifier)EA::WebKit::GetThreadSystem()->CurrentThreadId();
}

bool isMainThread()
{
	return EA::WebKit::GetThreadSystem()->IsMainThread();
	// Following would work too but using IsMainThread call allows the thread system implementation to optimize the call. WebKit has tons of isMainThread() calls
	// in debug mode so it is important to optimize this call in order to keep debug builds running at a reasonable frame rate.
	//(currentThread() == mainThreadIdentifier);
}

void yield()
{
	EA::WebKit::GetThreadSystem()->YieldThread();
}

void lockAtomicallyInitializedStaticMutex()
{
	ASSERT(atomicallyInitializedStaticMutex);
	atomicallyInitializedStaticMutex->lock();
}

void unlockAtomicallyInitializedStaticMutex()
{
	atomicallyInitializedStaticMutex->unlock();
}


Mutex::Mutex()
{
	m_mutex = EA::WebKit::GetThreadSystem()->CreateAMutex();
}

Mutex::~Mutex()
{
	EA::WebKit::GetThreadSystem()->DestroyAMutex(m_mutex);
}

void Mutex::lock()
{
	m_mutex->Lock();
}

bool Mutex::tryLock()
{
	return m_mutex->TryLock();
}

void Mutex::unlock()
{
	m_mutex->Unlock();
}

ThreadCondition::ThreadCondition()
{ 
	m_condition = EA::WebKit::GetThreadSystem()->CreateAThreadCondition();
}
ThreadCondition::~ThreadCondition() 
{ 
	 EA::WebKit::GetThreadSystem()->DestroyAThreadCondition(m_condition);
}
void ThreadCondition::wait(Mutex& mutex) 
{ 
	m_condition->Wait(mutex.impl());
}
bool ThreadCondition::timedWait(Mutex& mutex, double absoluteTime) 
{ 
	double currentTime = WTF::currentTime();

	// Time is in the past - return immediately.
	if (absoluteTime < currentTime)
		return false;

	// Time is too far in the future (and would overflow unsigned long) - wait forever.
	if (absoluteTime - currentTime > static_cast<double>(INT_MAX) / 1000.0) {
		wait(mutex);
		return true;
	}

	double intervalMilliseconds = (absoluteTime - currentTime) * 1000.0;
	return m_condition->TimedWait(mutex.impl(),intervalMilliseconds);
	
}

void ThreadCondition::signal() 
{ 
	m_condition->Signal(false); //Wake up one thread waiting thread
}
void ThreadCondition::broadcast() 
{ 
	m_condition->Signal(true);//Wake up all waiting threads
}

} // namespace WTF

