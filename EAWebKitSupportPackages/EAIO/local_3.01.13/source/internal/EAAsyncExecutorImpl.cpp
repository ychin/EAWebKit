/////////////////////////////////////////////////////////////////////////////
// EAAsyncExecutorImpl.cpp
//
// Copyright (c) 2019, Electronic Arts Inc. All rights reserved.
/////////////////////////////////////////////////////////////////////////////


#include <EAIO/internal/EAAsyncExecutorImpl.h>
#include <EAStdC/EASprintf.h>
#include <EAStdC/EABitTricks.h>
#include <EASTL/algorithm.h>
#include <EAAssert/eaassert.h>
#include <eathread/eathread_atomic.h>

#ifdef min
#undef min
#endif

namespace EA
{
namespace IO
{
namespace Internal
{

enum class AsyncCommandRequestType : uint8_t
{
	Undefined,
	Shutdown,
	Pause,
	Resume,
	Cancel,
	RegisterRequestFamily,
	UnregisterRequestFamily,
};

/// AsyncCommand
/// Commands are queued to the executor
///
struct AsyncCommandRequest : public AsyncRequest
{
	AsyncCommandRequest() : AsyncRequest(kAsyncCommandFamily, 0) {}
	AsyncCommandRequest(AsyncCommandRequestType type, uint32_t flags = 0) : AsyncRequest(kAsyncCommandFamily, flags), mType(type) {}

	AsyncCommandRequestType mType{ AsyncCommandRequestType::Undefined };
};

struct AsyncCommandCancelRequest : public AsyncCommandRequest
{
	AsyncCommandCancelRequest(AsyncRequestId requestId) : AsyncCommandRequest(AsyncCommandRequestType::Cancel), mRequestId(requestId) {}

	AsyncRequestId mRequestId;
};

struct AsyncCommandRequestFamilyRequest : public AsyncCommandRequest
{
	AsyncCommandRequestFamilyRequest(AsyncRequestFamily& family, AsyncCommandRequestType type) : AsyncCommandRequest(type), mFamily(family) {}

	AsyncRequestFamily& mFamily;
};

AsyncRequestAllocator::AsyncRequestAllocator(EA::Allocator::ICoreAllocator* coreAllocator, uint32_t size, uint32_t count, uint32_t alignment)
	: mCoreAllocator(coreAllocator)
	, mSize(size)
	, mCount(count)
	, mCountMask(count - 1)
	, mAlignment(alignment)
{
	EA_ASSERT(EA::StdC::IsPowerOf2(mCount));
	uint32_t totalSize = size * count;
	mBaseData = reinterpret_cast<uint8_t*>(mCoreAllocator->Alloc(size * count, "EAAsyncRequestAllocator", 0, alignment));
	memset(mBaseData, 0, totalSize);
	for (uint32_t i = 0; i < count; ++i)
	{
		AsyncRequest* request = reinterpret_cast<AsyncRequest*>(&mBaseData[i * mSize]);
		request->mFlags = kARFFree;
	}
}

void* AsyncRequestAllocator::Alloc(size_t requestSize, size_t alignment, uint32_t& extraFlags)
{
	if (requestSize <= mSize && alignment <= mAlignment)
	{
		for (uint32_t i = 0; i < 16; ++i)
		{
			uint32_t index = EA::Thread::AtomicFetchIncrement(&mNextIndex);
			index &= mCountMask;
			AsyncRequest* request = reinterpret_cast<AsyncRequest*>(&mBaseData[index * mSize]);
			if ((EA::Thread::AtomicFetchAnd(&request->mFlags, ~kARFFree) & kARFFree) != 0)
			{
				extraFlags = kARFCachable;
				return request;
			}
		}
	}

	extraFlags = 0;
	return mCoreAllocator->Alloc(requestSize, "EAAsyncRequest", 0, uint32_t(alignment), 0);
}

void AsyncRequestAllocator::Free(AsyncRequest* request)
{
	if (request->mFlags & kARFCachable)
	{
		request->~AsyncRequest();
		request->mNext = nullptr;
		EA::Thread::AtomicSetValue(&request->mFlags, kARFFree);
	}
	else
	{
		request->~AsyncRequest();
		mCoreAllocator->Free(request);
	}
}


AsyncExecutorImpl::AsyncExecutorImpl(EA::Allocator::ICoreAllocator* allocator, const ExecutorKey& key, uint32_t activeRequestLimit)
	: mCoreAllocator(allocator)
	, mKey(key)
	, mRequestAllocator(allocator, kCachedRequestMaxSize, kCachedRequestCount, kCachedRequestAlignment)
	, mActiveRequestLimit(activeRequestLimit)
{
	mRequests.get_container().get_allocator().set_allocator(allocator);
	mFamilyLookupMap.get_allocator().get_overflow_allocator().set_allocator(allocator);
	mActivePolledRequests.get_allocator().set_allocator(allocator);
}

AsyncExecutorImpl::~AsyncExecutorImpl()
{
	while (AsyncRequest* ptr = mRequestStack.Pop())
	{
		ptr->~AsyncRequest();
		mCoreAllocator->Free(ptr);
	}
	for (auto& kvp : mFamilyLookupMap)
		kvp.second->Unregister();
}

void AsyncExecutorImpl::Start()
{
	mThread.Begin(this);
}

bool AsyncExecutorImpl::IsShutdown()
{
	auto result = mThread.WaitForEnd();
	return result == EA::Thread::Thread::kStatusEnded || result == EA::Thread::Thread::kStatusNone;
}

void AsyncExecutorImpl::Push(AsyncRequest& request)
{
	mRequestStack.Push(&request);

	if (EA::Thread::AtomicFetchIncrement(&mRequestNotificationCount) == 0)
		mRequestNotification.Post();

	if (EA::Thread::AtomicSetValueConditional(&mInWait, 2, 1) == 1)
		SignalPushToOS(request);
}

void AsyncExecutorImpl::Pause()
{
	AllocAndPush<AsyncCommandRequest>(AsyncCommandRequestType::Pause);
}

void AsyncExecutorImpl::Resume()
{
	AllocAndPush<AsyncCommandRequest>(AsyncCommandRequestType::Resume);
}

void AsyncExecutorImpl::Cancel(AsyncRequestId requestId)
{
	AllocAndPush<AsyncCommandCancelRequest>(requestId);
}

void AsyncExecutorImpl::RegisterRequestFamily(AsyncRequestFamily& family)
{
	AllocAndPush<AsyncCommandRequestFamilyRequest>(eastl::ref(family), AsyncCommandRequestType::RegisterRequestFamily);
}

void AsyncExecutorImpl::UnregisterRequestFamily(AsyncRequestFamily& family)
{
	AllocAndPush<AsyncCommandRequestFamilyRequest>(eastl::ref(family), AsyncCommandRequestType::UnregisterRequestFamily);
}

#define EAIO_USE_AGGRESSIVE_IO_SCHEDULING 1

void AsyncExecutorImpl::Run(const EA::Thread::ThreadTime& waitTimeoutAbsolute, const EA::Thread::ThreadTime& runTimeoutAbsolute)
{
	if (mShutdownEventReceived)
		return;

	// When we need to do a wait, this will be the absolute outside value we are willing to wait
	EA::Thread::ThreadTime maxTimeoutAbsolute = eastl::min(waitTimeoutAbsolute, runTimeoutAbsolute);

	for (;;)
	{

		// Pop any waiting requests
		{
			bool poll = mActiveRequestCount != 0 || (!mRequests.empty() && !mIsPaused) || mReQueueRequest;
			WaitResults waitResults = PopPendingRequests(poll, maxTimeoutAbsolute);
			EA_UNUSED(waitResults);
		}

		// Exit if we got a shutdown
		if (mShutdownEventReceived)
			break;

		// If we have exceeded out execution quantum, then return
		if (runTimeoutAbsolute != EA::Thread::kTimeoutNone && runTimeoutAbsolute <= EA::Thread::GetThreadTime())
			return;

		// Execute the next request to be executed
		if (!mIsPaused)
		{
#if EAIO_USE_AGGRESSIVE_IO_SCHEDULING
			bool gotOne = false;
			while (mActiveRequestCount < mActiveRequestLimit)
#else
			if (mActiveRequestCount < mActiveRequestLimit)
#endif
			{
				if (AsyncQueueableRequest* request = Pop())
				{
#if EAIO_USE_AGGRESSIVE_IO_SCHEDULING
					gotOne = true;
#endif
					request->mFlags |= kARFActive;
					++mActiveRequestCount;
					{
						if (AsyncRequestFamily* family = GetRequestFamily(request->mFamily))
						{
							switch (family->ExecuteQueued(*request, mIsBlocking))
							{
							case AsyncExecuteQueuedResult::Done:
								FreeRequestInternal(request);
								break;
							case AsyncExecuteQueuedResult::Pending:
								break;
							case AsyncExecuteQueuedResult::ReQueue:
								{
									mReQueueRequest = request;
									--mActiveRequestCount;
									#if EAIO_USE_AGGRESSIVE_IO_SCHEDULING
										gotOne = false;
									#endif
								}
								break;
							case AsyncExecuteQueuedResult::Error:
								FreeRequestInternal(request);
								break;
							}
						}
						else
							FreeRequestInternal(request);
					}
#if !EAIO_USE_AGGRESSIVE_IO_SCHEDULING
					if (!mReQueueRequest)
						continue; // <--- this goes back to the start of the for loop to check the request queue
#endif
				}
#if EAIO_USE_AGGRESSIVE_IO_SCHEDULING
				else
					break; // <--- this breaks from the while loop

				if (mReQueueRequest)
					break; // <--- this breaks from the while loop
#endif
			}
#if EAIO_USE_AGGRESSIVE_IO_SCHEDULING
			if (gotOne)
				continue; // <--- this goes back to the start of the for loop to check the request queue
#endif
		}

		if (mActiveRequestCount != 0)
		{

			// Pool any pending polled requests
			if (!mActivePolledRequests.empty())
			{
				eastl::erase_if(mActivePolledRequests, 
					[this](AsyncIoRequest* request)
				{
					AsyncResult result;
					AsyncErrorCode errorCode;
					size_t bytesTransferred;
					if (request->IsDone(result, errorCode, bytesTransferred))
					{
						if (AsyncRequestFamily* family = GetRequestFamily(request->mFamily))
							family->ExecuteCompleted(*request, result, errorCode, bytesTransferred);
						else
						{
							EA_ASSERT_MSG(false, "This should never happen");
							request->Complete(result, errorCode, bytesTransferred);
						}
						FreeRequestInternal(request);
						return true;
					}
					return false;
				});
			}

			// Wait for any waitable requests
			if (mActiveRequestCount > mActivePolledRequests.size() || mReQueueRequest)
			{
				bool hasMoreRequests = !mRequests.empty() && mActiveRequestCount < mActiveRequestLimit;
				EA::Thread::ThreadTime alertableAbsoluteTimeout{ EA::Thread::kTimeoutImmediate };
				EA::Thread::ThreadTime notAlertableAbsoluteTimeout{ EA::Thread::kTimeoutImmediate };
				if (mActivePolledRequests.empty() && (mAlertableWaitTimeout != 0 || mNotAlertableWaitTimeout != 0))
				{
					EA::Thread::ThreadTime currentTime = EA::Thread::GetThreadTime();
					if (mAlertableWaitTimeout != 0)
						alertableAbsoluteTimeout = eastl::min(currentTime + mAlertableWaitTimeout, maxTimeoutAbsolute);
					if (mNotAlertableWaitTimeout != 0)
						notAlertableAbsoluteTimeout = eastl::min(currentTime + mNotAlertableWaitTimeout, maxTimeoutAbsolute);
				}
				WaitResults results = Wait(alertableAbsoluteTimeout, notAlertableAbsoluteTimeout, hasMoreRequests);
				switch (results)
				{
				case WaitResults::Request:
					break;
				case WaitResults::Timeout:
					break;
				case WaitResults::Error:
					// TODO - Log the error
					break;
				}
			}

			// Tiny sleep 
			if (!mActivePolledRequests.empty())
			{
				int timeout = GetPollingSleepTimeout();
				if (timeout >= 0)
					EA::Thread::ThreadSleep(timeout);
			}
		}

		// At this point we have to wait for activity.  If an immediate timeout, then just return
		if (waitTimeoutAbsolute == EA::Thread::kTimeoutImmediate)
			return;
	}
}

void AsyncExecutorImpl::ExecuteCommand(AsyncCommandRequest* command)
{
	switch (command->mType)
	{
	case AsyncCommandRequestType::Shutdown:
		mShutdownEventReceived = true;
		break;
	case AsyncCommandRequestType::Cancel:
	{
		const AsyncCommandCancelRequest& cancel = static_cast<AsyncCommandCancelRequest&>(*command);
		if (cancel.mRequestId != kInvalidAsyncRequestId)
			ExecuteCancelCommand(cancel.mRequestId);
		break;
	}
	case AsyncCommandRequestType::Pause:
		mIsPaused = true;
		break;
	case AsyncCommandRequestType::Resume:
		mIsPaused = false;
		break;
	case AsyncCommandRequestType::Undefined:
		break;
	case AsyncCommandRequestType::RegisterRequestFamily:
	{
		const AsyncCommandRequestFamilyRequest& family = static_cast<AsyncCommandRequestFamilyRequest&>(*command);
		RegisterRequestFamilyInternal(family.mFamily);
		break;
	}
	case AsyncCommandRequestType::UnregisterRequestFamily:
	{
		const AsyncCommandRequestFamilyRequest& family = static_cast<AsyncCommandRequestFamilyRequest&>(*command);
		UnregisterRequestFamilyInternal(family.mFamily);
		break;
	}
	}
}

void AsyncExecutorImpl::ExecuteCancelCommand(AsyncRequestId requestId)
{
	auto& container = mRequests.get_container();
	auto ix = eastl::find(container.begin(), container.end(), requestId,
		[](AsyncQueueableRequest* request, AsyncRequestId requestId) { return request->mId == requestId; });
	if (ix != container.end())
	{
		(*ix)->Cancel();
		FreeRequestInternal(*ix);
		container.erase(ix);
	}
}


void AsyncExecutorImpl::ExecuteCompleted(AsyncRequest& request, AsyncResult result, const AsyncErrorCode& errorCode, size_t bytesTransferred)
{
	if (AsyncRequestFamily* family = GetRequestFamily(request.mFamily))
		family->ExecuteCompleted(request, result, errorCode, bytesTransferred);
	FreeRequestInternal(&request);
}


intptr_t AsyncExecutorImpl::Run(void* pContext)
{
	static constexpr size_t kMaxNameLength = 64;
	char name[kMaxNameLength + 1];

	const char* type = "?";
	switch (mKey.type)
	{
	case AsyncExecutorType::Sequential: type = "Sequential"; break;
	case AsyncExecutorType::Concurrent: type = "Concurrent"; break;
	case AsyncExecutorType::Count: type = "?"; break;
	}

	const char* device = mKey.device[0] ? mKey.device : "global";

	EA::StdC::Snprintf(name, kMaxNameLength, "%sExecutor_%s_%d", type, device, mKey.platformSpecificData);
	name[kMaxNameLength] = 0;
	mThread.SetName(name);

	mIsBlocking = GetType() == AsyncExecutorType::Sequential;
	RegisterRequestFamilyInternal(mCommandFamily);
	RegisterRequestFamilyInternal(mIoFamily);

	Run(EA::Thread::kTimeoutNone, EA::Thread::kTimeoutNone);
	return 0;
}

AsyncExecutorImpl::WaitResults AsyncExecutorImpl::PopPendingRequests(bool poll, const EA::Thread::ThreadTime& waitTimeoutAbsolute)
{
	WaitResults waitResults = WaitResults::Request;

	// Do a simple interlocked checked to see if we have 
	uint32_t addend = uint32_t(-int32_t(mPriorRequestNotificationCount));
	mPriorRequestNotificationCount = EA::Thread::AtomicFetchAdd(&mRequestNotificationCount, addend) - mPriorRequestNotificationCount;

	// If nothing is waiting, then go into a wait state
	if (mPriorRequestNotificationCount == 0)
	{
		if (!poll)
		{
			int result = mRequestNotification.Wait(waitTimeoutAbsolute);
			switch (result)
			{
			case EA::Thread::Semaphore::kResultError:
				waitResults = WaitResults::Error;
				break;
			case EA::Thread::Semaphore::kResultTimeout:
				waitResults = WaitResults::Timeout;
				break;
			default:
				break;
			}

		}
	}

	// Pop any pending requests
	AsyncRequest* next = nullptr;
	AsyncRequest* request = Reverse(mRequestStack.PeelOff());
	for (; request; request = next)
	{
		next = request->mNext;

		if (AsyncRequestFamily* family = GetRequestFamily(request->mFamily))
		{
			switch (family->ExecuteImmediate(*request, mIsBlocking))
			{
			case AsyncExecuteImmediateResult::Done:
				FreeRequestInternal(request);
				break;
			case AsyncExecuteImmediateResult::Queue:
				if (request->mFlags & kARFHasId)
					mRequests.push(static_cast<AsyncQueueableRequest*>(request));
				else
				{
					EA_ASSERT_MSG(false, "AsyncRequets that are queued must be derived from AsyncQueueableRequest");
					FreeRequestInternal(request);
				}
				break;
			case AsyncExecuteImmediateResult::Error:
				FreeRequestInternal(request);
				break;
			}
		}
		else
			FreeRequestInternal(request);
	}
	return waitResults;
}

AsyncQueueableRequest* AsyncExecutorImpl::Pop()
{
	AsyncQueueableRequest* request = mReQueueRequest;
	if (request)
	{
		EA_ASSERT(request->mFlags & kARFHasId);
		mReQueueRequest = nullptr;
		return request;
	}

	if (mRequests.empty())
		return nullptr;

	request = mRequests.top();
	mRequests.pop();
	return request;
}

void* AsyncExecutorImpl::AllocRequestInternal(size_t requestSize, size_t alignment, uint32_t& extraFlags)
{
	return mRequestAllocator.Alloc(requestSize, alignment, extraFlags);
}

void AsyncExecutorImpl::FreeRequestInternal(AsyncRequest* request)
{
	EA_ASSERT((request->mFlags & kARFFree) == 0);
	if ((request->mFlags & kARFActive) != 0)
		--mActiveRequestCount;

	mRequestAllocator.Free(request);
}

AsyncRequest* AsyncExecutorImpl::Reverse(AsyncRequest* tail)
{
	AsyncRequest* next = nullptr;
	AsyncRequest* current = tail;

	while (current != nullptr)
	{
		AsyncRequest* prev = current->mNext;
		current->mNext = next;
		next = current;
		current = prev;
	}

	return next;
}

AsyncRequestFamily* AsyncExecutorImpl::GetRequestFamily(uint32_t id)
{
	auto ix = mFamilyLookupMap.find(id);
	if (ix == mFamilyLookupMap.end())
	{
		EA_ASSERT_MSG(false, "A given request for a family was sent but that family hasn't been registered to this executor");
		return nullptr;
	}
	return ix->second;
}

void AsyncExecutorImpl::CommandAsyncRequestFamily::Register(AsyncExecutor& executor)
{
	mExecutor = static_cast<AsyncExecutorImpl*>(&executor);
}

AsyncExecuteImmediateResult AsyncExecutorImpl::CommandAsyncRequestFamily::ExecuteImmediate(AsyncRequest& request, bool blocking)
{
	static_cast<AsyncExecutorImpl*>(mExecutor)->ExecuteCommand(static_cast<AsyncCommandRequest*>(&request));
	return AsyncExecuteImmediateResult::Done;
}

AsyncExecuteQueuedResult AsyncExecutorImpl::CommandAsyncRequestFamily::ExecuteQueued(AsyncRequest& request, bool blocking) 
{
	EA_ASSERT_MSG(false, "Not implemented");
	return AsyncExecuteQueuedResult::Error;
}

void AsyncExecutorImpl::CommandAsyncRequestFamily::ExecuteCompleted(AsyncRequest& request, AsyncResult result, const AsyncErrorCode& errorCode, size_t bytesTransferred)
{
	EA_ASSERT_MSG(false, "Not implemented");
}

void AsyncExecutorImpl::IoAsyncRequestFamily::Register(AsyncExecutor& executor)
{
	mExecutor = static_cast<AsyncExecutorImpl*>(&executor);
}

AsyncExecuteImmediateResult AsyncExecutorImpl::IoAsyncRequestFamily::ExecuteImmediate(AsyncRequest& request, bool blocking) 
{
	return AsyncExecuteImmediateResult::Queue;
}

AsyncExecuteQueuedResult AsyncExecutorImpl::IoAsyncRequestFamily::ExecuteQueued(AsyncRequest& request, bool blocking) 
{
	AsyncIoRequest& ioRequest = static_cast<AsyncIoRequest&>(request);
	AsyncErrorCode ec;
	AsyncResult result;
	size_t bytesTransferred;

	if (blocking)
	{
		bool done = ioRequest.Execute(result, ec, bytesTransferred);
		if (!done)
		{
			for (;;)
			{
				// We really shouldn't get here.  If we are processing a non-blocking thing 
				// here, then we are spinning and wasting context switches
				if (ioRequest.IsDone(result, ec, bytesTransferred))
					break;
				EA::Thread::ThreadSleep(EA::IO::AsyncExecutor::GetPollingSleepTimeout());
			}
		}
		ioRequest.Complete(result, ec, bytesTransferred);
		return AsyncExecuteQueuedResult::Done;
	}
	else
	{
		bool done = ioRequest.Execute(result, ec, bytesTransferred);
		if (done && (result != AsyncResult::Retry))
			ioRequest.Complete(result, ec, bytesTransferred);
		if (!done && ioRequest.GetAsyncIoMode() == AsyncIoMode::Polled)
			mExecutor->AddPolledIoRequest(ioRequest);

		if (!done)
			return AsyncExecuteQueuedResult::Pending;
		else if (result == AsyncResult::Retry)
			return AsyncExecuteQueuedResult::ReQueue;
		else
			return AsyncExecuteQueuedResult::Done;
	}
}


void AsyncExecutorImpl::IoAsyncRequestFamily::ExecuteCompleted(AsyncRequest& request, AsyncResult result, const AsyncErrorCode& errorCode, size_t bytesTransferred)
{
	static_cast<AsyncIoRequest*>(&request)->Complete(result, errorCode, bytesTransferred);
}


void AsyncExecutorImpl::RegisterRequestFamilyInternal(AsyncRequestFamily& family)
{
	mFamilyLookupMap.emplace(family.GetFamilyId(), &family);
	family.Register(*this);
}


void AsyncExecutorImpl::UnregisterRequestFamilyInternal(AsyncRequestFamily& family)
{
	auto ix = mFamilyLookupMap.find(family.GetFamilyId());
	if (ix == mFamilyLookupMap.end() || ix->second != &family)
		return;

	mFamilyLookupMap.erase(ix);
	family.Unregister();
}


void sendAsyncShutdownCommand(AsyncExecutor& exec)
{
	exec.AllocAndPush<AsyncCommandRequest>(AsyncCommandRequestType::Shutdown);
}

} // namespace Internal
} // namespace IO
} // namespace EA
