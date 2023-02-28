/////////////////////////////////////////////////////////////////////////////
// EAAsyncExecutorUnix.cpp
//
// Copyright (c) 2019, Electronic Arts Inc. All rights reserved.
/////////////////////////////////////////////////////////////////////////////

#include <EAAssert/eaassert.h>
#include <EAIO/EAAsyncStream.h>
#include <EASTL/stack.h>

#include <aio.h>
#include <unistd.h>
#include <errno.h>

namespace EA
{
namespace IO
{

struct AsyncExecutorAsyncOp
{
	AsyncIoRequest* request;
	aiocb cb;
};

namespace Internal
{

/////////////////////////////////////////////////////////////////////////////////////////////////
// Base executor for Unix.  Uses KEvents for the core notifications.

class UnixAsyncExecutorImpl
	: public AsyncExecutorImpl
{
	using Super_t = AsyncExecutorImpl;

public:

	UnixAsyncExecutorImpl(EA::Allocator::ICoreAllocator* allocator, const ExecutorKey& key, uint32_t activeRequestLimit = kDefaultActiveRequestLimit)
		: Super_t(allocator, key, activeRequestLimit)
	{
		mFreeAsyncOps.get_container().get_allocator().set_allocator(allocator);
	}

	virtual ~UnixAsyncExecutorImpl()
	{
		EA::Allocator::ICoreAllocator* allocator = GetAllocator();
		while (!mFreeAsyncOps.empty())
		{
			mFreeAsyncOps.top()->~AsyncExecutorAsyncOp();
			allocator->Free(mFreeAsyncOps.top());
			mFreeAsyncOps.pop();
		}
	}

	virtual void Start() override
	{
		Super_t::Start();
	}

	virtual bool Read(AsyncResult& result, AsyncErrorCode& errorCode, size_t& bytesTransferred, AsyncIoRequest* request, int fd, off_type position, void* destination, size_type size, AsyncExecutorAsyncOp*& op)
	{
		// If this was a retry request, our op has not been freed yet so re-use it
		AsyncExecutorAsyncOp* asyncOp = op;
		if (!asyncOp)
			asyncOp = op = AllocAsyncOp(request);
		EA_ASSERT(asyncOp->request == request);

		InitCb(asyncOp, fd, position, destination, size);

		int results = ::aio_read(&asyncOp->cb);
		if (results == 0)
		{
			EA_ASSERT(::aio_error(&asyncOp->cb) != ENOSYS); // This is really bad if it happens
			return false;
		}
		else if (results == EAGAIN)
		{
			/**
			 * Kernel has ran out of kernel resources OR
			 * Our Process has a resource limit set   OR
			 * Kernel is trying to backpressure us
			 */

			result = AsyncResult::Retry;
			errorCode = AsyncErrorCode(results);
			return true;
		}
		result = AsyncResult::Error;
		errorCode = AsyncErrorCode(results);
		return true;
	}

	virtual bool Write(AsyncResult& result, AsyncErrorCode& errorCode, size_t& bytesTransferred, AsyncIoRequest* request, int fd, off_type position, const void* source, size_type size, AsyncExecutorAsyncOp*& op)
	{
		AsyncExecutorAsyncOp* asyncOp = op = AllocAsyncOp(request);
		InitCb(asyncOp, fd, position, const_cast<void*>(source), size);
		int results = ::aio_write(&asyncOp->cb);
		if (results == 0)
		{
			EA_ASSERT(::aio_error(&asyncOp->cb) != ENOSYS); // This is really bad if it happens
			return false;
		}
		result = AsyncResult::Error;
		errorCode = AsyncErrorCode(results);
		return true;
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
			new (asyncOp) AsyncExecutorAsyncOp();
		}

		memset(asyncOp, 0, sizeof(*asyncOp));
		asyncOp->request = request;
		return asyncOp;
	}

	void FreeAsyncOp(AsyncExecutorAsyncOp* asyncOp)
	{
		mFreeAsyncOps.push(asyncOp);
	}

	static bool HandleIoState(AsyncExecutorAsyncOp& asyncOp, int errorResult, AsyncResult& result, AsyncErrorCode& errorCode, size_t& bytesTransferred)
	{
		if (errorResult == EINPROGRESS)
			return false;

		if (errorResult == 0)
		{
			result = AsyncResult::Success;
			errorCode = 0;
			bytesTransferred = asyncOp.cb.aio_nbytes;
		}
		else if (errorResult == ECANCELED)
		{
			result = AsyncResult::Canceled;
			errorCode = 0;
			bytesTransferred = 0;
		}
		else
		{
			EA_ASSERT(errorResult != ENOSYS); // This is really bad if it happens
			result = AsyncResult::Error;
			errorCode = AsyncErrorCode(errorResult);
			bytesTransferred = 0;
		}
		return true;
	}

private:
	static void InitCb(AsyncExecutorAsyncOp* asyncOp, int fd, off_type position, void* buffer, size_type size)
	{
		memset(&asyncOp->cb, 0, sizeof(asyncOp->cb));
		asyncOp->cb.aio_fildes = fd;
		asyncOp->cb.aio_buf = const_cast<void*>(buffer);
		asyncOp->cb.aio_nbytes = size;
		asyncOp->cb.aio_offset = position;
		asyncOp->cb.aio_sigevent.sigev_notify = SIGEV_NONE;
	}

	eastl::stack<AsyncExecutorAsyncOp*, eastl::vector<AsyncExecutorAsyncOp*, EA::Allocator::EAIOEASTLCoreAllocator>> mFreeAsyncOps;
};

/////////////////////////////////////////////////////////////////////////////////////////////////
// Serialized executor will execute commands serially based on the priority

class UnixSequentialAsyncExecutorImpl
	: public UnixAsyncExecutorImpl
{
	using Super_t = UnixAsyncExecutorImpl;
	using UnixAsyncExecutorImpl::UnixAsyncExecutorImpl;
public:

	virtual AsyncExecutorType GetType() override
	{
		return AsyncExecutorType::Sequential;
	}

	virtual WaitResults Wait(const EA::Thread::ThreadTime& alertableAbsoluteTimeout, const EA::Thread::ThreadTime& notAlertableAbsoluteTimeout, bool hasMoreRequests) override
	{
		return WaitResults::Timeout;
	}
};

/////////////////////////////////////////////////////////////////////////////////////////////////
// Concurrent executor will go wide with commands. 

class UnixConcurrentAsyncFileExecutorImpl
	: public UnixAsyncExecutorImpl
{
	using Super_t = UnixAsyncExecutorImpl;
	static constexpr uint32_t kAioActiveRequestLimit = 64;

public:
	UnixConcurrentAsyncFileExecutorImpl(EA::Allocator::ICoreAllocator* allocator, const ExecutorKey& key)
		: UnixAsyncExecutorImpl(allocator, key, kAioActiveRequestLimit)
	{
		mAsyncAioOps = reinterpret_cast<AsyncExecutorAsyncOp**>(allocator->Alloc(sizeof(AsyncExecutorAsyncOp*) * kAioActiveRequestLimit, "UnixAioOps", 0, alignof(AsyncExecutorAsyncOp*), 0));
		mAioCbs = reinterpret_cast<aiocb**>(allocator->Alloc(sizeof(aiocb*) * kAioActiveRequestLimit, "UnixAioCbs", 0, alignof(aiocb*), 0));
	}

	virtual ~UnixConcurrentAsyncFileExecutorImpl()
	{
		EA::Allocator::ICoreAllocator* allocator = GetAllocator();
		allocator->Free(mAsyncAioOps);
		allocator->Free(mAioCbs);
	}

	virtual AsyncExecutorType GetType() override
	{
		return AsyncExecutorType::Concurrent;
	}

	void Bind(int fd)
	{
	}

	void Unbind(int fd)
	{
	}

	virtual bool Read(AsyncResult& result, AsyncErrorCode& errorCode, size_t& bytesTransferred, AsyncIoRequest* request,
		int fd, off_type position, void* destination, size_type size, AsyncExecutorAsyncOp*& op) override
	{
		bool results = UnixAsyncExecutorImpl::Read(result, errorCode, bytesTransferred, request, fd, position, destination, size, op);
		if (!results)
		{
			uint activeCount = mActiveAioCount++;
			mAsyncAioOps[activeCount] = op;
			mAioCbs[activeCount] = &op->cb;
		}
		return results;
	}

	virtual bool Write(AsyncResult& result, AsyncErrorCode& errorCode, size_t& bytesTransferred, AsyncIoRequest* request,
		int fd, off_type position, const void* source, size_type size, AsyncExecutorAsyncOp*& op) override
	{
		bool results = UnixAsyncExecutorImpl::Write(result, errorCode, bytesTransferred, request, fd, position, source, size, op);
		if (!results)
		{
			uint activeCount = mActiveAioCount++;
			mAsyncAioOps[activeCount] = op;
			mAioCbs[activeCount] = &op->cb;
		}
		return results;
	}

protected:
	virtual void ExecuteCancelCommand(AsyncRequestId requestId) override
	{
		for (uint i = 0; i < mActiveAioCount; ++i)
		{
			if (mAsyncAioOps[i]->request->mId == requestId)
			{
				int result = ::aio_cancel(mAsyncAioOps[i]->cb.aio_fildes, &mAsyncAioOps[i]->cb);
				EA_UNUSED(result);
				return;
			}
		}
		UnixAsyncExecutorImpl::ExecuteCancelCommand(requestId);
	}

	virtual WaitResults Wait(const EA::Thread::ThreadTime& alertableAbsoluteTimeout, const EA::Thread::ThreadTime& notAlertableAbsoluteTimeout, bool hasMoreRequests) override
	{
		WaitResults waitResults{ WaitResults::Timeout };

		// If we have no AIO, just return
		if (mActiveAioCount == 0)
			return WaitResults::Timeout;

		// Check for async IO completion.  If we have more requests, then only do an immediate wait.
		// Otherwise do a short delay so we can still check for more requests
		if (mActiveAioCount != 0)
		{
			EA::Thread::ThreadTime timeout = hasMoreRequests ? EA::Thread::kTimeoutImmediate : notAlertableAbsoluteTimeout;
			int results = ::aio_suspend(mAioCbs, mActiveAioCount, &timeout);
			if (results == 0 || errno == EINTR)
			{
				AsyncResult result;
				AsyncErrorCode errorCode;
				size_t bytesTransferred;
				for (uint i = 0; i != mActiveAioCount;)
				{
					results = ::aio_error(mAioCbs[i]);
					if (HandleIoState(*mAsyncAioOps[i], results, result, errorCode, bytesTransferred))
					{
						ExecuteCompleted(*mAsyncAioOps[i]->request, result, errorCode, bytesTransferred);
						--mActiveAioCount;
						mAsyncAioOps[i] = mAsyncAioOps[mActiveAioCount];
						mAioCbs[i] = mAioCbs[mActiveAioCount];
					}
					else
						++i;
				}
				return WaitResults::Request;
			}
			else if (errno == EAGAIN)
				return WaitResults::Timeout;
			else
				return WaitResults::Error;
		}
		return waitResults;
	}

private:
	AsyncExecutorAsyncOp** mAsyncAioOps{ nullptr };
	aiocb** mAioCbs{ nullptr };
	uint mActiveAioCount{ 0 };
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
		UnixConcurrentAsyncFileExecutorImpl* exec = reinterpret_cast<UnixConcurrentAsyncFileExecutorImpl*>(
			allocator->Alloc(sizeof(UnixConcurrentAsyncFileExecutorImpl), "UnixConcurrentAsyncExecutorImpl",
				0, alignof(UnixConcurrentAsyncFileExecutorImpl), 0));
		new (exec)UnixConcurrentAsyncFileExecutorImpl(allocator, key);
		return exec;
	}
	case AsyncExecutorType::Sequential:
	{
		UnixSequentialAsyncExecutorImpl* exec = reinterpret_cast<UnixSequentialAsyncExecutorImpl*>(
			allocator->Alloc(sizeof(UnixSequentialAsyncExecutorImpl), "UnixSequentialAsyncExecutorImpl",
				0, alignof(UnixSequentialAsyncExecutorImpl), 0));
		new (exec)UnixSequentialAsyncExecutorImpl(allocator, key);
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

/////////////////////////////////////////////////////////////////////////////////////////////////

void BindToUnixExecutor(AsyncExecutor& asyncExecutor, int fd)
{
	if (asyncExecutor.GetType() == AsyncExecutorType::Concurrent)
	{
		Internal::UnixConcurrentAsyncFileExecutorImpl& exec = static_cast<Internal::UnixConcurrentAsyncFileExecutorImpl&>(asyncExecutor);
		exec.Bind(fd);
	}
}

void UnbindFromUnixExecutor(AsyncExecutor& asyncExecutor, int fd)
{
	if (asyncExecutor.GetType() == AsyncExecutorType::Concurrent)
	{
		Internal::UnixConcurrentAsyncFileExecutorImpl& exec = static_cast<Internal::UnixConcurrentAsyncFileExecutorImpl&>(asyncExecutor);
		exec.Unbind(fd);
	}
}

void FreeAsyncOp(AsyncExecutor& asyncExecutor, AsyncExecutorAsyncOp* op)
{
	if (op == nullptr)
		return;

	Internal::UnixConcurrentAsyncFileExecutorImpl& exec = static_cast<Internal::UnixConcurrentAsyncFileExecutorImpl&>(asyncExecutor);
	exec.FreeAsyncOp(op);
}

bool IsAsyncOpDone(AsyncExecutor& asyncExecutor, AsyncExecutorAsyncOp& op, AsyncResult& result, AsyncErrorCode& errorCode, size_t& bytesTransferred)
{
	int results = ::aio_error(&op.cb);
	return Internal::UnixAsyncExecutorImpl::HandleIoState(op, results, result, errorCode, bytesTransferred);
}

bool AsyncRead(AsyncExecutor& asyncExecutor, AsyncExecutorAsyncOp*& op,
	AsyncResult& result, AsyncErrorCode& errorCode, size_t& bytesTransferred,
	AsyncIoRequest* request, int fd, size_type position, void* destination, size_type size)
{
	Internal::UnixConcurrentAsyncFileExecutorImpl& exec = static_cast<Internal::UnixConcurrentAsyncFileExecutorImpl&>(asyncExecutor);
	return exec.Read(result, errorCode, bytesTransferred, request, fd, position, destination, size, op);
}

bool AsyncWrite(AsyncExecutor& asyncExecutor, AsyncExecutorAsyncOp*& op,
	AsyncResult& result, AsyncErrorCode& errorCode, size_t& bytesTransferred,
	AsyncIoRequest* request, int fd, size_type position, const void* source, size_type size)
{
	Internal::UnixConcurrentAsyncFileExecutorImpl& exec = static_cast<Internal::UnixConcurrentAsyncFileExecutorImpl&>(asyncExecutor);
	return exec.Write(result, errorCode, bytesTransferred, request, fd, position, source, size, op);
}

} // namespace IO
} // namespace EA
