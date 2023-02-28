/////////////////////////////////////////////////////////////////////////////
// EAAsyncWin32.cpp
//
// Copyright (c) 2019, Electronic Arts Inc. All rights reserved.
/////////////////////////////////////////////////////////////////////////////

#include <EAAssert/eaassert.h>
#include <EAStdC/EABitTricks.h>
#include <EAIO/EAAsyncStream.h>
#include <EASTL/stack.h>

#pragma warning(push, 0)
#ifndef NOMINMAX
#define NOMINMAX
#endif
#include <Windows.h>
#pragma warning(pop)

#if defined(EA_PLATFORM_XBSX)
#include <gxdk.h>
#endif

namespace EA
{
namespace IO
{

struct Win32AsyncGlobalState
{
#if defined(EA_PLATFORM_XBSX)
	IDStorageFactoryX* mFactory{ nullptr };
	IDStorageQueueX* mQueue{ nullptr };
#endif
};

Win32AsyncGlobalState* s_win32AsyncGlobalState; 

struct AsyncExecutorAsyncOp
{
	AsyncIoRequest* request;
	OVERLAPPED overlapped;
};

namespace Internal
{

#if defined(EA_PLATFORM_XBSX)
// Used to generate requests to read data
class BuildDStorageBuilder
	: public DStorageBuilder
{
	using Requests_t = DStorageIoRequest::Requests_t;

public:
	BuildDStorageBuilder(Requests_t& requests)
		: mRequests(requests)
	{}

	virtual void Read(void* destination, uint32_t destinationSize, IDStorageFileX& file, uint64_t fileOffset, uint32_t fileSize, uint64_t options) override final
	{
		DSTORAGE_REQUEST& request = mRequests.push_back();
		request.Options.AsUINT64 = options;
		request.Destination = destination;
		request.DestinationSize = destinationSize;
		request.File = &file;
		request.FileOffset = fileOffset;
		request.SourceSize = fileSize;
		request.IntermediateSize = 0;
		request.CancellationTag = 0;
		request.Name = nullptr;
	}

private:
	Requests_t& mRequests;
};

class DStorageIoRequestImpl : public DStorageIoRequest
{
public:
	DStorageIoRequestImpl(AsyncExecutor& asyncExecutor, size_t readCountHint, const DStorageIoCallback& func)
		: DStorageIoRequest(asyncExecutor, readCountHint)
		, mFunc(func)
	{
	}

	virtual void InvokeCallback(AsyncResult result, const AsyncErrorCode& errorCode, size_t bytesTransferred) override final
	{
		if (mFunc)
			mFunc(result, errorCode);
	}

protected:
	DStorageIoCallback mFunc;
};
#endif

/////////////////////////////////////////////////////////////////////////////////////////////////
// Base executor for Win32.  Implements the IOCP object used for notifications. 

class Win32AsyncExecutorImpl
	: public AsyncExecutorImpl
{
	using Super_t = AsyncExecutorImpl;

protected:
	enum CompletionKeys
	{
		kCKRequestQueued,
		kCKRequestCompleted,
	};

public:

	Win32AsyncExecutorImpl(EA::Allocator::ICoreAllocator* allocator, const ExecutorKey& key, uint32_t activeRequestLimit = kDefaultActiveRequestLimit)
		: Super_t(allocator, key, activeRequestLimit)
	{
#if defined(EA_PLATFORM_XBSX)
		mAvailableStatusIndices.get_allocator().set_allocator(allocator);
#endif
	}

	virtual ~Win32AsyncExecutorImpl()
	{
#if defined(EA_PLATFORM_XBSX)
		if (mStatusArray)
			mStatusArray->Release();
		mStatusArray = nullptr;
#endif

		::CloseHandle(mIOCP);

		EA::Allocator::ICoreAllocator* allocator = GetAllocator();
		while (!mFreeAsyncOps.empty())
		{
			mFreeAsyncOps.top()->~AsyncExecutorAsyncOp();
			allocator->Free(mFreeAsyncOps.top());
			mFreeAsyncOps.pop();
		}
	}

	virtual void Start()
	{
		mIOCP = ::CreateIoCompletionPort(INVALID_HANDLE_VALUE, NULL, 0, 0);

#if defined(EA_PLATFORM_XBSX)
		uint32_t activeRequestLimit = GetActiveRequestLimit();
		uint32_t maskCount = (activeRequestLimit + 31) / 32;
		mAvailableStatusIndices.resize(maskCount, ~0u);
#if _GXDK_EDITION >= 200400
		HRESULT hr = GetDStorageFactory().CreateStatusArray(activeRequestLimit, "AsyncExecutor", __uuidof(IDStorageStatusArrayX), (void**)&mStatusArray);
#else
		HRESULT hr = GetDStorageFactory().CreateStatusArray(activeRequestLimit, __uuidof(IDStorageStatusArrayX), (void**)&mStatusArray);
#endif
		EA_ASSERT_MESSAGE(SUCCEEDED(hr), "Unable to get direct storage status array");
		EA_UNUSED(hr);
#endif
		Super_t::Start();
	}

	virtual void SignalPushToOS(AsyncRequest& request) override
	{
		::PostQueuedCompletionStatus(mIOCP, 0, kCKRequestQueued, NULL);
	}

	bool Read(AsyncResult& result, AsyncErrorCode& errorCode, size_t& bytesTransferred, AsyncIoRequest* request, void* handle, size_type position, void* destination, size_type size, AsyncExecutorAsyncOp*& op)
	{
		AsyncExecutorAsyncOp* asyncOp = op = AllocAsyncOp(request);
		asyncOp->overlapped.OffsetHigh = uint64_t(position) >> 32;
		asyncOp->overlapped.Offset = position & 0xFFFFFFFF;
		DWORD read;
		if (::ReadFile(HANDLE(handle), destination, DWORD(size), &read, &asyncOp->overlapped))
		{
			if (GetType() == AsyncExecutorType::Concurrent)
				return false;
			bytesTransferred = read;
			result = AsyncResult::Success;
			return true;
		}
		else
		{
			DWORD err = ::GetLastError();
			if (err == ERROR_IO_PENDING)
				return false;
			errorCode = err;
			result = AsyncResult::Error;
			return true;
		}
	}

	bool Write(AsyncResult& result, AsyncErrorCode& errorCode, size_t& bytesTransferred, AsyncIoRequest* request, void* handle, size_type position, const void* source, size_type size, AsyncExecutorAsyncOp*& op)
	{
		AsyncExecutorAsyncOp* asyncOp = op = AllocAsyncOp(request);
		asyncOp->overlapped.OffsetHigh = uint64_t(position) >> 32;
		asyncOp->overlapped.Offset = position & 0xFFFFFFFF;
		DWORD written;
		if (::WriteFile(HANDLE(handle), source, DWORD(size), &written, &asyncOp->overlapped))
		{
			if (GetType() == AsyncExecutorType::Concurrent)
				return false;
			bytesTransferred = written;
			result = AsyncResult::Success;
			return true;
		}
		else
		{
			DWORD err = ::GetLastError();
			if (err == ERROR_IO_PENDING)
				return false;
			errorCode = err;
			result = AsyncResult::Error;
			return true;
		}
	}

	AsyncExecutorAsyncOp* AllocAsyncOp(AsyncIoRequest* request)
	{
		AsyncExecutorAsyncOp* asyncOp;
		if (!mFreeAsyncOps.empty())
		{
			asyncOp = mFreeAsyncOps.top();
			mFreeAsyncOps.pop();
		}
		else
		{
			EA::Allocator::ICoreAllocator* allocator = GetAllocator();
			asyncOp = reinterpret_cast<AsyncExecutorAsyncOp*>(allocator->Alloc(sizeof(AsyncExecutorAsyncOp), "AsyncExecutorAsyncOps", 0, alignof(AsyncExecutorAsyncOp), 0));
			new (asyncOp)AsyncExecutorAsyncOp();
		}

		memset(asyncOp, 0, sizeof(*asyncOp));
		asyncOp->request = request;
		return asyncOp;
	}

	void FreeAsyncOp(AsyncExecutorAsyncOp* asyncOp)
	{
		mFreeAsyncOps.push(asyncOp);
	}

#if defined(EA_PLATFORM_XBSX)
	IDStorageStatusArrayX& GetDStorageStatusArray()
	{
		EA_ASSERT(mStatusArray);
		return *mStatusArray;
	}

	uint32_t AcquireStatusArrayIndex()
	{
		uint32_t base{ 0 };
		for (auto& mask : mAvailableStatusIndices)
		{
			int index = EA::StdC::CountTrailing0Bits(mask);
			if (index != 32)
			{
				mask &= ~(1u << index);
				return base + index;
			}
			base += 32;
		}
		EA_ASSERT(false);
		return 0;
	}

	void ReleaseStatusArrayIndex(uint32_t index)
	{
		EA_ASSERT(index < GetActiveRequestLimit());
		int slot = index / 32;
		int bit = index % 32;
		uint32_t mask = 1u << bit;
		EA_ASSERT((mAvailableStatusIndices[slot] & mask) == 0);
		mAvailableStatusIndices[slot] |= mask;
	}
#endif

protected:

	uint32_t RelativeTimeoutFromAbsoluteTimeout(EA::Thread::ThreadTime timeoutAbsolute)
	{
		EAT_ASSERT((timeoutAbsolute == EA::Thread::kTimeoutImmediate) || (timeoutAbsolute > EATHREAD_MIN_ABSOLUTE_TIME)); // Assert that the user didn't make the mistake of treating time as relative instead of absolute.

		DWORD timeoutRelative = 0;

		if (timeoutAbsolute == EA::Thread::kTimeoutNone)
		{
			timeoutRelative = INFINITE;
		}
		else if (timeoutAbsolute == EA::Thread::kTimeoutImmediate)
		{
			timeoutRelative = 0;
		}
		else
		{
			EA::Thread::ThreadTime timeCurrent(EA::Thread::GetThreadTime());
			timeoutRelative = (timeoutAbsolute > timeCurrent) ? static_cast<DWORD>(timeoutAbsolute - timeCurrent) : 0;
		}

		EAT_ASSERT((timeoutRelative == INFINITE) || (timeoutRelative < 100000000)); // Assert that the timeout is a sane value and didn't wrap around.

		return timeoutRelative;
	}

	HANDLE mIOCP;

private:
	eastl::stack<AsyncExecutorAsyncOp*> mFreeAsyncOps;
#if defined(EA_PLATFORM_XBSX)
	eastl::vector<uint32_t, EA::Allocator::EAIOEASTLCoreAllocator> mAvailableStatusIndices;
	IDStorageStatusArrayX* mStatusArray{ nullptr };
#endif
};

/////////////////////////////////////////////////////////////////////////////////////////////////
// Serialized executor will execute commands serially based on the priority

class Win32SequentialAsyncExecutorImpl
	: public Win32AsyncExecutorImpl
{
	using Super_t = Win32AsyncExecutorImpl;
	using Win32AsyncExecutorImpl::Win32AsyncExecutorImpl;
public:

	virtual AsyncExecutorType GetType() override
	{
		return AsyncExecutorType::Sequential;
	}

private:
	virtual WaitResults Wait(const EA::Thread::ThreadTime& alertableAbsoluteTimeout, const EA::Thread::ThreadTime& notAlertableAbsoluteTimeout, bool hasMoreRequests) override
	{
		DWORD numberOfBytesTransferred = 0;
		ULONG_PTR completionKey = 0;
		LPOVERLAPPED overlapped = nullptr;
		BOOL results = ::GetQueuedCompletionStatus(mIOCP, &numberOfBytesTransferred, &completionKey, &overlapped,
			RelativeTimeoutFromAbsoluteTimeout(alertableAbsoluteTimeout));
		if (results)
			return WaitResults::Request;
		if (GetLastError() == ERROR_TIMEOUT)
			return WaitResults::Timeout;
		return WaitResults::Error;
	}
};

/////////////////////////////////////////////////////////////////////////////////////////////////
// Concurrent executor will go wide with commands. 

class Win32ConcurrentAsyncExecutorImpl
	: public Win32AsyncExecutorImpl
{
	using Super_t = Win32AsyncExecutorImpl;
	using Win32AsyncExecutorImpl::Win32AsyncExecutorImpl;

public:

	virtual AsyncExecutorType GetType() override
	{
		return AsyncExecutorType::Concurrent;
	}

	void Bind(void *handle)
	{
		::CreateIoCompletionPort((HANDLE)handle, mIOCP, kCKRequestCompleted, 1);
	}

	void Unbind(void* handle)
	{
	}

protected:
	virtual WaitResults Wait(const EA::Thread::ThreadTime& alertableAbsoluteTimeout, const EA::Thread::ThreadTime& notAlertableAbsoluteTimeout, bool hasMoreRequests) override
	{
		static constexpr ULONG kMaxCompleted = 64;
		OVERLAPPED_ENTRY completionPortEntries[kMaxCompleted];
		ULONG numEntriesRemoved;
		BeginWait();
		BOOL result = ::GetQueuedCompletionStatusEx(mIOCP, completionPortEntries, kMaxCompleted, &numEntriesRemoved, 
			RelativeTimeoutFromAbsoluteTimeout(alertableAbsoluteTimeout), TRUE);
		EndWait();
		if (result)
		{
			for (ULONG i = 0; i < numEntriesRemoved; ++i)
			{
				OVERLAPPED_ENTRY& entry = completionPortEntries[i];
				if (entry.lpCompletionKey == kCKRequestCompleted && entry.lpOverlapped)
				{
					AsyncExecutorAsyncOp* asyncOp = reinterpret_cast<AsyncExecutorAsyncOp*>(uintptr_t(entry.lpOverlapped) - EA_OFFSETOF(AsyncExecutorAsyncOp, overlapped));
					AsyncIoRequest* request = asyncOp->request;
					ExecuteCompleted(*request, AsyncResult::Success, AsyncErrorCode(), entry.dwNumberOfBytesTransferred);
				}
				else
				{
				}
			}
		}
		else
		{
			DWORD dwError = ::GetLastError();
			if (dwError != ERROR_TIMEOUT)
			{
			}
		}
		return WaitResults::Request;
	}
};

/////////////////////////////////////////////////////////////////////////////////////////////////

void PlatformInit(EA::Allocator::ICoreAllocator* allocator)
{
	s_win32AsyncGlobalState = reinterpret_cast<Win32AsyncGlobalState*>(allocator->Alloc(
		sizeof(Win32AsyncGlobalState), "EAWin32AsyncGlobal", 0, alignof(Win32AsyncGlobalState), 0));
	new (s_win32AsyncGlobalState)Win32AsyncGlobalState();

#if defined(EA_PLATFORM_XBSX)
	HRESULT hr = DStorageGetFactory(__uuidof(IDStorageFactoryX), (void**)&s_win32AsyncGlobalState->mFactory);
	EA_ASSERT_MESSAGE(SUCCEEDED(hr), "Unable to get direct storage factory");

	DSTORAGE_QUEUE_DESC qdesc;
	memset(&qdesc, 0, sizeof(qdesc));
	qdesc.SourceType = DSTORAGE_REQUEST_SOURCE_FILE;
	qdesc.Capacity = 1024*8;
	qdesc.Name = "EAIOAsynExecutor";
	qdesc.Priority = DSTORAGE_PRIORITY_NORMAL;
	hr = s_win32AsyncGlobalState->mFactory->CreateQueue(&qdesc, __uuidof(IDStorageQueueX), (void**)&s_win32AsyncGlobalState->mQueue);
	EA_ASSERT_MESSAGE(SUCCEEDED(hr), "Unable to get direct storage queue");
	EA_UNUSED(hr);
#endif
}

EA::IO::Internal::AsyncExecutorImpl* PlatformCreateExecutor(EA::Allocator::ICoreAllocator* const allocator, const Internal::ExecutorKey& key)
{
	using namespace Internal;

	switch (key.type)
	{
	case AsyncExecutorType::Concurrent:
	{
		Win32ConcurrentAsyncExecutorImpl* exec = reinterpret_cast<Win32ConcurrentAsyncExecutorImpl*>(
			allocator->Alloc(sizeof(Win32ConcurrentAsyncExecutorImpl), "Win32ConcurrentAsyncExecutorImpl",
				0, alignof(Win32ConcurrentAsyncExecutorImpl), 0));
		new (exec)Win32ConcurrentAsyncExecutorImpl(allocator, key);
		return exec;
	}
	case AsyncExecutorType::Sequential:
	{
		Win32SequentialAsyncExecutorImpl* exec = reinterpret_cast<Win32SequentialAsyncExecutorImpl*>(
			allocator->Alloc(sizeof(Win32SequentialAsyncExecutorImpl), "Win32SequentialAsyncExecutorImpl",
				0, alignof(Win32SequentialAsyncExecutorImpl), 0));
		new (exec)Win32SequentialAsyncExecutorImpl(allocator, key);
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
#if defined(EA_PLATFORM_XBSX)
	if (s_win32AsyncGlobalState->mQueue)
	{
		s_win32AsyncGlobalState->mQueue->Close();
		s_win32AsyncGlobalState->mQueue->Release();
	}
	if (s_win32AsyncGlobalState->mFactory)
		s_win32AsyncGlobalState->mFactory->Release();
#endif
}

} // namespace Internal

/////////////////////////////////////////////////////////////////////////////////////////////////

void BindToWin32Executor(AsyncExecutor& asyncExecutor, void* handle)
{
	if (asyncExecutor.GetType() == AsyncExecutorType::Concurrent)
	{
		Internal::Win32ConcurrentAsyncExecutorImpl& exec = static_cast<Internal::Win32ConcurrentAsyncExecutorImpl&>(asyncExecutor);
		exec.Bind(handle);
	}
}

void UnbindFromWin32Executor(AsyncExecutor& asyncExecutor, void* handle)
{
	if (asyncExecutor.GetType() == AsyncExecutorType::Concurrent)
	{
		Internal::Win32ConcurrentAsyncExecutorImpl& exec = static_cast<Internal::Win32ConcurrentAsyncExecutorImpl&>(asyncExecutor);
		exec.Unbind(handle);
	}
}

bool AsyncRead(AsyncExecutor& asyncExecutor, AsyncExecutorAsyncOp*& op,
	AsyncResult& result, AsyncErrorCode& errorCode, size_t& bytesTransferred,
	AsyncIoRequest* request, void* handle, size_type position, void* destination, size_type size)
{
	Internal::Win32ConcurrentAsyncExecutorImpl& exec = static_cast<Internal::Win32ConcurrentAsyncExecutorImpl&>(asyncExecutor);
	return exec.Read(result, errorCode, bytesTransferred, request, handle, position, destination, size, op);
}

bool AsyncWrite(AsyncExecutor& asyncExecutor, AsyncExecutorAsyncOp*& op,
	AsyncResult& result, AsyncErrorCode& errorCode, size_t& bytesTransferred,
	AsyncIoRequest* request, void* handle, size_type position, const void* source, size_type size)
{
	Internal::Win32ConcurrentAsyncExecutorImpl& exec = static_cast<Internal::Win32ConcurrentAsyncExecutorImpl&>(asyncExecutor);
	return exec.Write(result, errorCode, bytesTransferred, request, handle, position, source, size, op);
}

bool IsAsyncOpDone(AsyncExecutor& asyncExecutor, AsyncExecutorAsyncOp& op, AsyncResult& result, AsyncErrorCode& errorCode, size_t& bytesTransferred, void* handle)
{
	if (!HasOverlappedIoCompleted(&op.overlapped))
		return false;

	DWORD numberOfBytesTransferred = 0;
	if (GetOverlappedResult(HANDLE(handle), &op.overlapped, &numberOfBytesTransferred, FALSE) != 0)
	{
		result = AsyncResult::Success;
		bytesTransferred = numberOfBytesTransferred;
	}
	else if (::GetLastError() == ERROR_OPERATION_ABORTED)
	{
		result = AsyncResult::Canceled;
		errorCode = AsyncErrorCode(GetLastError());
	}
	else
	{
		result = AsyncResult::Error;
		errorCode = AsyncErrorCode(GetLastError());
	}
	return true;
}

void FreeAsyncOp(AsyncExecutor& asyncExecutor, AsyncExecutorAsyncOp* op)
{
	Internal::Win32ConcurrentAsyncExecutorImpl& exec = static_cast<Internal::Win32ConcurrentAsyncExecutorImpl&>(asyncExecutor);
	exec.FreeAsyncOp(op);
}

#if defined(EA_PLATFORM_XBSX)
DStorageIoRequest::DStorageIoRequest(AsyncExecutor& asyncExecutor, size_t readCountHint)
	: mAsyncExecutor(asyncExecutor)
{
	mRequests.get_allocator().set_allocator(asyncExecutor.GetAllocator());
	mRequests.reserve(Requests_t::size_type(readCountHint));
}

DStorageIoRequest::~DStorageIoRequest()
{
	if (mStatusIndex != eastl::numeric_limits<uint32_t>::max())
		ReleaseStatusArrayIndex(mAsyncExecutor, mStatusIndex);
}

void DStorageIoRequest::Build(const DStorageBuilderCallback& builder)
{
	Internal::BuildDStorageBuilder build(mRequests);
	builder(build);
}

AsyncIoMode DStorageIoRequest::GetAsyncIoMode() const
{
	return AsyncIoMode::Polled;
}

bool DStorageIoRequest::Execute(AsyncResult& result, AsyncErrorCode& errorCode, size_t& bytesTransferred)
{
	bytesTransferred = 0;

	IDStorageQueueX& queue = GetDStorageQueue();
	IDStorageStatusArrayX& statusArray = GetDStorageStatusArray(mAsyncExecutor);
	mStatusIndex = AcquireStatusArrayIndex(mAsyncExecutor);
	for (auto&& request : mRequests)
		queue.EnqueueRequest(&request);
	queue.EnqueueStatus(&statusArray, mStatusIndex);
	queue.Submit();

	if (!IsAsync())
	{
		WaitInternal(result, errorCode);
		return true;
	}
	else
	{
		return IsDone(result, errorCode, bytesTransferred);
	}
}

bool DStorageIoRequest::IsDone(AsyncResult& result, AsyncErrorCode& errorCode, size_t& bytesTransferred)
{
	IDStorageStatusArrayX& statusArray = GetDStorageStatusArray(mAsyncExecutor);
	HRESULT hr = statusArray.GetHResult(mStatusIndex);
	if (hr == E_PENDING)
		return false;
	if (SUCCEEDED(hr))
	{
		result = AsyncResult::Success;
		errorCode = 0;
		bytesTransferred = 0;
	}
	else
	{
		IDStorageQueueX& queue = GetDStorageQueue();
		HANDLE ehandle = queue.GetErrorEvent();
		for (;;)
		{
			DWORD status = ::WaitForSingleObject(ehandle, 0);
			if (status != WAIT_OBJECT_0)
				break;
			DSTORAGE_ERROR_RECORD error;
			memset(&error, 0, sizeof(error));
			queue.RetrieveErrorRecord(&error);
			EA_UNUSED(error);
		}
		result = AsyncResult::Error;
		errorCode = hr;
		bytesTransferred = 0;
	}
	return true;
}

void DStorageIoRequest::Complete(AsyncResult result, const AsyncErrorCode& errorCode, size_t bytesTransferred)
{
	EA::IO::AsyncResult localResult{ result };
	EA::IO::AsyncErrorCode localErrorCode{ errorCode };

	if (IsAsync())
		WaitInternal(localResult, localErrorCode);
	InvokeCallback(localResult, localErrorCode, bytesTransferred);
}

bool DStorageIoRequest::IsAsync() const
{
	return mAsyncExecutor.GetType() == AsyncExecutorType::Concurrent;
}

void DStorageIoRequest::WaitInternal(AsyncResult& result, AsyncErrorCode& errorCode)
{
	for (;;)
	{
		size_t bytesTransferred;
		if (IsDone(result, errorCode, bytesTransferred))
			return;
		EA::Thread::ThreadSleep(EA::IO::AsyncExecutor::GetPollingSleepTimeout());
	}
}

DStorageIoRequest& CreateDStorageRequest(AsyncExecutor& asyncExecutor, size_t readCountHint, const DStorageIoCallback& callback)
{
	return CreateDStorageRequest<Internal::DStorageIoRequestImpl>(eastl::ref(asyncExecutor), readCountHint, eastl::cref(callback));
}

IDStorageFactoryX& GetDStorageFactory()
{
	EA_ASSERT(s_win32AsyncGlobalState);
	return *s_win32AsyncGlobalState->mFactory;
}

IDStorageQueueX& GetDStorageQueue()
{
	EA_ASSERT(s_win32AsyncGlobalState);
	return *s_win32AsyncGlobalState->mQueue;
}

IDStorageStatusArrayX& GetDStorageStatusArray(AsyncExecutor& asyncExecutor)
{
	Internal::Win32AsyncExecutorImpl& exec = static_cast<Internal::Win32AsyncExecutorImpl&>(asyncExecutor);
	return exec.GetDStorageStatusArray();
}

uint32_t AcquireStatusArrayIndex(AsyncExecutor& asyncExecutor)
{
	Internal::Win32AsyncExecutorImpl& exec = static_cast<Internal::Win32AsyncExecutorImpl&>(asyncExecutor);
	return exec.AcquireStatusArrayIndex();
}

void ReleaseStatusArrayIndex(AsyncExecutor& asyncExecutor, uint32_t index)
{
	Internal::Win32AsyncExecutorImpl& exec = static_cast<Internal::Win32AsyncExecutorImpl&>(asyncExecutor);
	exec.ReleaseStatusArrayIndex(index);
}
#endif

} // namespace IO
} // namespace EA
