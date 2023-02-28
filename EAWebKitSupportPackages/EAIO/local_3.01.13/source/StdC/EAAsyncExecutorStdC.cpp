/////////////////////////////////////////////////////////////////////////////
// EAAsyncExecutorStdC.cpp
//
// Copyright (c) 2019, Electronic Arts Inc. All rights reserved.
/////////////////////////////////////////////////////////////////////////////

#include <EAIO/EAAsyncStream.h>

namespace EA
{
namespace IO
{
namespace Internal
{

/////////////////////////////////////////////////////////////////////////////////////////////////
// Base executor for StdC.  Implements the IOCP object used for notifications. 

class StdCAsyncExecutor
	: public AsyncExecutorImpl
{
	using Super_t = AsyncExecutorImpl;
	using AsyncExecutorImpl::AsyncExecutorImpl;

private:
	// Since StdC doesn't support async, this should never get called anyway.
	virtual WaitResults Wait(const EA::Thread::ThreadTime& alertableAbsoluteTimeout, const EA::Thread::ThreadTime& notAlertableAbsoluteTimeout, bool hasMoreRequests) override
	{
		return WaitResults::Timeout;
	}
};

/////////////////////////////////////////////////////////////////////////////////////////////////
// Serialized executor will execute commands serially based on the priority

class StdCSequentialAsyncExecutorImpl
	: public StdCAsyncExecutor
{
	using Super_t = StdCAsyncExecutor;
	using StdCAsyncExecutor::StdCAsyncExecutor;
public:

	virtual AsyncExecutorType GetType() override
	{
		return AsyncExecutorType::Sequential;
	}
};

/////////////////////////////////////////////////////////////////////////////////////////////////
// Concurrent executor will go wide with commands. 

class StdCConcurrentAsyncExecutorImpl
	: public StdCAsyncExecutor
{
	using Super_t = StdCAsyncExecutor;
	using StdCAsyncExecutor::StdCAsyncExecutor;
public:

	virtual AsyncExecutorType GetType() override
	{
		return AsyncExecutorType::Sequential;
	}
};

/////////////////////////////////////////////////////////////////////////////////////////////////

void PlatformInit(EA::Allocator::ICoreAllocator* allocator)
{
}

EA::IO::Internal::AsyncExecutorImpl* PlatformCreateExecutor(EA::Allocator::ICoreAllocator* const allocator, const Internal::ExecutorKey& key)
{
	using namespace Internal;

	switch (key.type)
	{
	case AsyncExecutorType::Concurrent:
	{
		StdCConcurrentAsyncExecutorImpl* exec = reinterpret_cast<StdCConcurrentAsyncExecutorImpl*>(
			allocator->Alloc(sizeof(StdCConcurrentAsyncExecutorImpl), "StdCConcurrentAsyncExecutorImpl",
				0, alignof(StdCConcurrentAsyncExecutorImpl), 0));
		new (exec)StdCConcurrentAsyncExecutorImpl(allocator, key);
		return exec;
	}
	case AsyncExecutorType::Sequential:
	{
		StdCSequentialAsyncExecutorImpl* exec = reinterpret_cast<StdCSequentialAsyncExecutorImpl*>(
			allocator->Alloc(sizeof(StdCSequentialAsyncExecutorImpl), "StdCSequentialAsyncExecutorImpl",
				0, alignof(StdCSequentialAsyncExecutorImpl), 0));
		new (exec)StdCSequentialAsyncExecutorImpl(allocator, key);
		return exec;
	}
	default:
	case AsyncExecutorType::Count:
		EA_ASSERT(false);
		return nullptr;
	}
}

void PlatformShutdown(EA::Allocator::ICoreAllocator* allocator)
{
}

} // namespace Internal

} // namespace IO
} // namespace EA
