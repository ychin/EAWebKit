/////////////////////////////////////////////////////////////////////////////
// EAAsyncExecutorImpl.cpp
//
// Copyright (c) 2019, Electronic Arts Inc. All rights reserved.
/////////////////////////////////////////////////////////////////////////////


#include <EAIO/EAAsyncExecutor.h>
#include <EAIO/internal/EAAsyncExecutorImpl.h>
#include <EAIO/internal/EAIOEASTLCoreAllocator.h>
#include <eathread/eathread_rwspinlock.h>
#include <EAAssert/eaassert.h>

#if defined(EA_PLATFORM_MICROSOFT)
	#include "Win32/EAAsyncExecutorWin32.cpp"
#elif defined(EA_PLATFORM_ANDROID)
	#include "StdC/EAAsyncExecutorStdC.cpp"
#elif defined(EA_PLATFORM_NX)
	#include "StdC/EAAsyncExecutorStdC.cpp"
#elif EAIO_USE_UNIX_IO
	#include "Unix/EAAsyncExecutorUnix.cpp"
#elif defined(EA_PLATFORM_SONY)
	#include "Sony/EAAsyncExecutorSony.cpp"
#else
	#include "StdC/EAAsyncExecutorStdC.cpp"
#endif

namespace EA
{
namespace IO
{
namespace Internal
{
static const char* s_emptyDeviceString = "";

void sendAsyncShutdownCommand(AsyncExecutor& exec);

/////////////////////////////////////////////////////////////////////////////////////////////////
// Singleton global state for the async system.  Mostly just keeps up with the active executors
struct AsyncGlobalState
{
	EA::Thread::RWSpinLock executorsLock;
	eastl::hash_map<ExecutorKey, AsyncExecutorImpl*, ExecutorKeyHash, eastl::equal_to<ExecutorKey>, Allocator::EAIOEASTLCoreAllocator> executors;
	AsyncExecutorImpl* defaultExecutors[int(AsyncExecutorType::Count)]{ nullptr, nullptr };
	bool isShuttingDown{ false };
#if defined(EA_PLATFORM_WINDOWS) || defined(EA_PLATFORM_XBSX) || defined(EA_PLATFORM_PS5)
	int pollingSleepTimeout{ 0 };
#else
	int pollingSleepTimeout{ 1 };
#endif
};
EA::Allocator::ICoreAllocator* a_asyncAllocator;
AsyncGlobalState* s_asyncGlobalState;
uint64_t s_nextRequestId;
} // namespace Internal

AsyncQueueableRequest::AsyncQueueableRequest(uint32_t family, int32_t priority, uint32_t flags)
	: AsyncRequest(family, flags | kARFHasId)
	, mId(EA::Thread::AtomicFetchIncrement(&Internal::s_nextRequestId) + 1)
	, mPriority(priority)
{}

/////////////////////////////////////////////////////////////////////////////////////////////////
// Static executor implementations

void AsyncExecutor::Init(EA::Allocator::ICoreAllocator* allocator)
{
	using namespace Internal;

	EA_ASSERT(s_asyncGlobalState == nullptr);
	if (allocator == nullptr)
		allocator = EA::IO::GetAllocator();

	a_asyncAllocator = allocator;
	s_asyncGlobalState = reinterpret_cast<AsyncGlobalState*>(allocator->Alloc(sizeof(AsyncGlobalState), "EAAsyncGlobal", 0, alignof(AsyncGlobalState), 0));
	new (s_asyncGlobalState)AsyncGlobalState();
	s_asyncGlobalState->executors.get_allocator().set_allocator(allocator);

	EA::IO::Internal::PlatformInit(allocator);
}

AsyncExecutor* AsyncExecutor::StartExecutor(
	AsyncExecutorType type,
	int platformSpecificData,
	const char* device)
{
	using namespace Internal;

	EA_ASSERT(s_asyncGlobalState != nullptr);
	Internal::AsyncGlobalState& state = *s_asyncGlobalState;

	// For simple requests, use the cache of default executors
	if (platformSpecificData == 0 && (device == nullptr || device[0] == 0))
	{
		EA::Thread::AutoRWSpinLock lock(state.executorsLock, EA::Thread::AutoRWSpinLock::kLockTypeRead);
		if (state.defaultExecutors[int(type)])
			return state.defaultExecutors[int(type)];
	}

	EA::Thread::AutoRWSpinLock lock(state.executorsLock, EA::Thread::AutoRWSpinLock::kLockTypeWrite);

	const char* dupdevice = Internal::s_emptyDeviceString;
	if (device != nullptr && device[0] != 0)
	{
		size_t devlen = EA::StdC::Strlen(device);
		char* dup = reinterpret_cast<char*>(a_asyncAllocator->Alloc(devlen + 1, "AsyncDeviceName", 0));
		memcpy(dup, device, devlen + 1);
		dupdevice = dup;
	}

	auto ixb = state.executors.emplace(Internal::ExecutorKey{ type, platformSpecificData, dupdevice }, nullptr);
	if (ixb.second)
	{
		ixb.first->second = EA::IO::Internal::PlatformCreateExecutor(a_asyncAllocator, ixb.first->first);

		ixb.first->second->Start();

		if (platformSpecificData == 0 && dupdevice[0] == 0)
			state.defaultExecutors[int(type)] = ixb.first->second;
	}
	else
		a_asyncAllocator->Free(const_cast<char*>(dupdevice));

	return ixb.first->second;
}

bool AsyncExecutor::StopExecutors()
{
	using namespace Internal;

	EA_ASSERT(s_asyncGlobalState != nullptr);
	AsyncGlobalState& state = *s_asyncGlobalState;

	EA::Thread::AutoRWSpinLock lock(state.executorsLock, EA::Thread::AutoRWSpinLock::kLockTypeWrite);

	bool sendShutdown = !state.isShuttingDown;
	state.isShuttingDown = true;

	for (auto ix = state.executors.begin(), e = state.executors.end(); ix != e;)
	{
		if (sendShutdown)
			Internal::sendAsyncShutdownCommand(*ix->second);

		if (ix->second->IsShutdown())
		{
			if (ix->first.device != Internal::s_emptyDeviceString)
				a_asyncAllocator->Free(const_cast<char*>(ix->first.device));
			ix->second->~AsyncExecutorImpl();
			a_asyncAllocator->Free(ix->second);
			ix = state.executors.erase(ix);
		}
		else
			++ix;
	}

	return state.executors.empty();
}

void AsyncExecutor::Shutdown(const EA::Thread::ThreadTime& timeoutAbsolute)
{
	using namespace Internal;

	if (s_asyncGlobalState == nullptr)
		return;

	// We start by trying to stop all the executors.  If we have called this
	// before, it will just do a check to see if everything is shutdown. 
	// Otherwise this starts the process.
	if (!StopExecutors())
	{
		while (timeoutAbsolute > EA::Thread::GetThreadTime())
		{
			EA::Thread::ThreadSleep(1);
			if (StopExecutors())
				break;
		}
	}

	EA::IO::Internal::PlatformShutdown(a_asyncAllocator);

	s_asyncGlobalState->~AsyncGlobalState();
	a_asyncAllocator->Free(Internal::s_asyncGlobalState);
	s_asyncGlobalState = nullptr;
	a_asyncAllocator = nullptr;
}

int AsyncExecutor::GetPollingSleepTimeout()
{
	using namespace Internal;
	return s_asyncGlobalState != nullptr ? s_asyncGlobalState->pollingSleepTimeout : 1;
}

void AsyncExecutor::SetPollingSleepTimeout(int timeout)
{
	using namespace Internal;
	if (s_asyncGlobalState != nullptr)
		s_asyncGlobalState->pollingSleepTimeout = timeout;
}

} // namespace IO
} // namespace EA
