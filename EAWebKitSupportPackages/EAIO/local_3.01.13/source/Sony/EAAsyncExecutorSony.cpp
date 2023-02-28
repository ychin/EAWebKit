/////////////////////////////////////////////////////////////////////////////
// EAAsyncExecutorSony.cpp
//
// Copyright (c) 2019, Electronic Arts Inc. All rights reserved.
/////////////////////////////////////////////////////////////////////////////

#include <EAAssert/eaassert.h>
#include <EAIO/EAAsyncStream.h>
#include <EASTL/fixed_vector.h>
#include <EASTL/stack.h>
#include <EAStdC/EABitTricks.h>

#include <EAIO/Sony/EAAsyncExecutorSony.h>
#include <EAIO/internal/EAAsyncExecutorImpl.h>

#include <kernel/equeue.h>

#define EA_SONY_AMPR_FORCE_SYNC false

namespace EA
{
namespace IO
{

struct AsyncExecutorAsyncOp
{
	SceKernelAioSubmitId id;
	AsyncIoRequest* request;
	SceKernelAioResult result;
};

namespace Internal
{

#if defined(EA_PLATFORM_PS5)
static constexpr size_t kFastCommandBufferCount = 32;
static constexpr size_t kFastCommandBufferSize = 64 * 1024;
static constexpr size_t kFastCommandBufferAlignment = 64 * 1024;

struct FreeFastCommandBuffer
{
	FreeFastCommandBuffer* mNext;
};
#endif

struct SonyAsyncGlobalState
{
#if defined(EA_PLATFORM_PS5)
	EA::Allocator::ICoreAllocator* allocator{ nullptr };
	void* fastCommandBuffersBegin{ nullptr };
	void* fastCommandBuffersEnd{ nullptr };
	Internal::AtomicIntrusiveStack<Internal::FreeFastCommandBuffer, offsetof(Internal::FreeFastCommandBuffer, mNext)> freeCommandBuffers;
#if defined(EA_DEBUG)
	bool amprMemoryValidation{ true };
#else
	bool amprMemoryValidation{ false };
#endif
#endif
};

SonyAsyncGlobalState* s_sonyAsyncGlobalState;

/////////////////////////////////////////////////////////////////////////////////////////////////
// Base executor for Sony.  Uses KEvents for the core notifications.

class SonyAsyncExecutorImpl
	: public AsyncExecutorImpl
{
	using Super_t = AsyncExecutorImpl;

public:
	static constexpr int RequestEventId = 1;
#if defined(EA_PLATFORM_PS5)
	static constexpr int AmprIoEventId = 1 << 10;
#endif

public:

	SonyAsyncExecutorImpl(EA::Allocator::ICoreAllocator* allocator, const ExecutorKey& key, uint32_t activeRequestLimit = kDefaultActiveRequestLimit)
		: Super_t(allocator, key, activeRequestLimit)
	{
		mFreeAsyncOps.get_container().get_allocator().set_allocator(allocator);
#if defined(EA_PLATFORM_PS5)
		mAvailable.get_allocator().set_allocator(allocator);
#endif
	}

	virtual ~SonyAsyncExecutorImpl()
	{
		::sceKernelDeleteUserEvent(mKernelEventQueue, RequestEventId);
#if defined(EA_PLATFORM_PS5)
		for (uint i = 0, limit = GetActiveRequestLimit(); i < limit; ++i)
			::sceKernelDeleteAmprEvent(mKernelEventQueue, AmprIoEventId + i);
#endif
		::sceKernelDeleteEqueue(mKernelEventQueue);

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
		int result;
		EA_UNUSED(result);

		result = ::sceKernelCreateEqueue(&mKernelEventQueue, "EAIOAsyncEqueue");
		EA_ASSERT(result == SCE_OK);
		result = ::sceKernelAddUserEventEdge(mKernelEventQueue, RequestEventId);
		EA_ASSERT(result == SCE_OK);
#if defined(EA_PLATFORM_PS5)
		uint activeRequestLimit = GetActiveRequestLimit();
		for (uint i = 0; i < activeRequestLimit; ++i)
		{
			result = ::sceKernelAddAmprEvent(mKernelEventQueue, AmprIoEventId + i, nullptr);
			EA_ASSERT(result == SCE_OK);
		}
		uint maskCount = (activeRequestLimit + 31) / 32;
		mAvailable.resize(maskCount, ~0u);
#endif
		Super_t::Start();
	}

	virtual void SignalPushToOS(AsyncRequest& request) override
	{
		int result = ::sceKernelTriggerUserEvent(mKernelEventQueue, RequestEventId, nullptr);
		EA_ASSERT(result == SCE_OK);
		EA_UNUSED(result);
	}

	virtual bool Read(AsyncResult& result, AsyncErrorCode& errorCode, size_t& bytesTransferred, AsyncIoRequest* request, int fd, off_type position, void* destination, size_type size, AsyncExecutorAsyncOp*& op)
	{
		AsyncExecutorAsyncOp* asyncOp = op = AllocAsyncOp(request);
		SceKernelAioRWRequest rwrequest{ position, size, destination, &asyncOp->result, fd };
		int results = sceKernelAioSubmitReadCommands(&rwrequest, 1, SCE_KERNEL_AIO_PRIORITY_MID, &asyncOp->id);
		if (results == SCE_OK)
			return false;
		result = AsyncResult::Error;
		errorCode = AsyncErrorCode(results);
		return true;
	}

	virtual bool Write(AsyncResult& result, AsyncErrorCode& errorCode, size_t& bytesTransferred, AsyncIoRequest* request, int fd, off_type position, const void* source, size_type size, AsyncExecutorAsyncOp*& op)
	{
		AsyncExecutorAsyncOp* asyncOp = op = AllocAsyncOp(request);
		SceKernelAioRWRequest rwrequest{ position, size, const_cast<void*>(source), &asyncOp->result, fd };
		int results = sceKernelAioSubmitWriteCommands(&rwrequest, 1, SCE_KERNEL_AIO_PRIORITY_MID, &asyncOp->id);
		if (results == SCE_OK)
			return false;
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


	static bool HandleIoState(AsyncExecutorAsyncOp& asyncOp, uint state, AsyncResult& result, AsyncErrorCode& errorCode, size_t& bytesTransferred)
	{
		bool notified = (state & SCE_KERNEL_AIO_STATE_NOTIFIED) != 0;
		EA_UNUSED(notified);
		state &= ~SCE_KERNEL_AIO_STATE_NOTIFIED;

		switch (state)
		{
		case SCE_KERNEL_AIO_STATE_COMPLETED:
		{
			if (asyncOp.result.returnValue < 0)
			{
				result = AsyncResult::Error;
				errorCode = AsyncErrorCode(asyncOp.result.returnValue);
				bytesTransferred = 0;
			}
			else
			{
				result = AsyncResult::Success;
				errorCode = AsyncErrorCode{ 0 };
				bytesTransferred = asyncOp.result.returnValue;
			}
			return true;
		}
		case SCE_KERNEL_AIO_STATE_ABORTED:
		{
			result = AsyncResult::Canceled;
			errorCode = AsyncErrorCode(asyncOp.result.returnValue);
			bytesTransferred = 0;
			return true;
		}
		default:
			return false;
		}
	}

	SceKernelEqueue GetKernelQueue()
	{
		return mKernelEventQueue;
	}

#if defined(EA_PLATFORM_PS5)
	int AcquireAmprRequestId()
	{
		int base = AmprIoEventId;
		for (auto& mask : mAvailable)
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

	void ReleaseAmprRequestId(int id)
	{
		EA_ASSERT(id >= AmprIoEventId && id <= AmprIoEventId + GetActiveRequestLimit());
		id -= AmprIoEventId;
		int slot = id / 32;
		int bit = id % 32;
		uint32_t mask = 1u << bit;
		EA_ASSERT((mAvailable[slot] & mask) == 0);
		mAvailable[slot] |= mask;
	}
#endif

protected:

	template <int kEventCount>
	WaitResults WaitInternalAbsolute(const EA::Thread::ThreadTime& waitTimeoutAbsolute)
	{
		SceKernelUseconds timeout{ EA::Thread::RelativeTimeoutFromAbsoluteTimeout(waitTimeoutAbsolute) };
		return WaitInternalRelative<kEventCount>(timeout);
	}

	template <int kEventCount>
	WaitResults WaitInternalRelative(SceKernelUseconds timeout)
	{
		SceKernelUseconds localTimeout{ timeout };
		SceKernelEvent events[kEventCount];
		int out;

		BeginWait();
		int result = ::sceKernelWaitEqueue(mKernelEventQueue, events, kEventCount, &out, &localTimeout);
		EndWait();
		if (result == SCE_OK)
		{
			for (int i = 0; i < out; ++i)
			{
				SceKernelEvent& evt = events[i];
				uintptr_t id = sceKernelGetEventId(&evt);
				EA_UNUSED(id);
				if (sceKernelGetEventFilter(&evt) == SCE_KERNEL_EVFILT_USER)
				{
					EA_ASSERT(id == RequestEventId);
				}
#if defined(EA_PLATFORM_PS5)
				else if (id >= AmprIoEventId && id <= AmprIoEventId + GetActiveRequestLimit())
				{
					AsyncIoRequest* request = reinterpret_cast<AsyncIoRequest*>(sceKernelGetEventData(&evt));
					ReleaseAmprRequestId(id);
					ExecuteCompleted(*request, AsyncResult::Success, AsyncErrorCode{ 0 }, 0); // values are dummy in nature
				}
#endif
			}
			return WaitResults::Request;
		}
		else if (result == SCE_KERNEL_ERROR_ETIMEDOUT)
			return WaitResults::Timeout;
		else
			return WaitResults::Error;
	}

	SceKernelEqueue mKernelEventQueue{ nullptr };

private:
	eastl::stack<AsyncExecutorAsyncOp*, eastl::vector<AsyncExecutorAsyncOp*, EA::Allocator::EAIOEASTLCoreAllocator>> mFreeAsyncOps;
#if defined(EA_PLATFORM_PS5)
	eastl::vector<uint32_t, EA::Allocator::EAIOEASTLCoreAllocator> mAvailable;
#endif
};

/////////////////////////////////////////////////////////////////////////////////////////////////
// Serialized executor will execute commands serially based on the priority

class SonySequentialAsyncExecutorImpl
	: public SonyAsyncExecutorImpl
{
	using Super_t = SonyAsyncExecutorImpl;
	using SonyAsyncExecutorImpl::SonyAsyncExecutorImpl;
public:

	virtual AsyncExecutorType GetType() override
	{
		return AsyncExecutorType::Sequential;
	}
	virtual WaitResults Wait(const EA::Thread::ThreadTime& alertableAbsoluteTimeout, const EA::Thread::ThreadTime& notAlertableAbsoluteTimeout, bool hasMoreRequests) override
	{
		return WaitInternalAbsolute<1>(alertableAbsoluteTimeout);
	}
};

/////////////////////////////////////////////////////////////////////////////////////////////////
// Concurrent executor will go wide with commands. 

class SonyConcurrentAsyncFileExecutorImpl
	: public SonyAsyncExecutorImpl
{
	using Super_t = SonyAsyncExecutorImpl;

public:

	SonyConcurrentAsyncFileExecutorImpl(EA::Allocator::ICoreAllocator* allocator, const ExecutorKey& key)
		: SonyAsyncExecutorImpl(allocator, key, SCE_KERNEL_AIO_ID_NUM_MAX)
	{
		mActiveAio = reinterpret_cast<SceKernelAioSubmitId*>(allocator->Alloc(sizeof(SceKernelAioSubmitId) * SCE_KERNEL_AIO_ID_NUM_MAX, "SonyActiveSubmitIds", 0, alignof(SceKernelAioSubmitId), 0));
		mToDeleteAio = reinterpret_cast<SceKernelAioSubmitId*>(allocator->Alloc(sizeof(SceKernelAioSubmitId) * SCE_KERNEL_AIO_ID_NUM_MAX, "SonyToDeleteSubmitIds", 0, alignof(SceKernelAioSubmitId), 0));
		mAsyncAioOps = reinterpret_cast<AsyncExecutorAsyncOp**>(allocator->Alloc(sizeof(AsyncExecutorAsyncOp*) * SCE_KERNEL_AIO_ID_NUM_MAX, "SonyToResults", 0, alignof(AsyncExecutorAsyncOp*), 0));
		mStates = reinterpret_cast<int*>(allocator->Alloc(sizeof(int) * SCE_KERNEL_AIO_ID_NUM_MAX, "SonyStates", 0, alignof(int), 0));
	}

	virtual ~SonyConcurrentAsyncFileExecutorImpl()
	{
		EA::Allocator::ICoreAllocator* allocator = GetAllocator();
		allocator->Free(mActiveAio);
		allocator->Free(mToDeleteAio);
		allocator->Free(mAsyncAioOps);
		allocator->Free(mStates);
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
		bool results = SonyAsyncExecutorImpl::Read(result, errorCode, bytesTransferred, request, fd, position, destination, size, op);
		if (!results)
		{
			uint activeCount = mActiveAioCount++;
			mActiveAio[activeCount] = op->id;
			mAsyncAioOps[activeCount] = op;
		}
		return results;
	}

	virtual bool Write(AsyncResult& result, AsyncErrorCode& errorCode, size_t& bytesTransferred, AsyncIoRequest* request,
		int fd, off_type position, const void* source, size_type size, AsyncExecutorAsyncOp*& op) override
	{
		bool results = SonyAsyncExecutorImpl::Write(result, errorCode, bytesTransferred, request, fd, position, source, size, op);
		if (!results)
		{
			uint activeCount = mActiveAioCount++;
			mActiveAio[activeCount] = op->id;
			mAsyncAioOps[activeCount] = op;
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
				int state = 0;
				int result = ::sceKernelAioCancelRequest(mAsyncAioOps[i]->id, &state);
				EA_UNUSED(result);
				return;
			}
		}
		SonyAsyncExecutorImpl::ExecuteCancelCommand(requestId);
	}

	virtual WaitResults Wait(const EA::Thread::ThreadTime& alertableAbsoluteTimeout, const EA::Thread::ThreadTime& notAlertableAbsoluteTimeout, bool hasMoreRequests) override
	{
		WaitResults waitResults{ WaitResults::Timeout };

		// Check for async IO completion.  If we have more requests, then only do an immediate wait.
		// Otherwise do a short delay so we can still check for more requests
		if (mActiveAioCount != 0)
		{
			memset(mStates, 0, sizeof(int) * mActiveAioCount);
			SceKernelUseconds timeout{ 0 };
			if (!hasMoreRequests)
				timeout = EA::Thread::RelativeTimeoutFromAbsoluteTimeout(notAlertableAbsoluteTimeout);

			uint toDelete = 0;

			EA_ASSERT(mActiveAioCount <= SCE_KERNEL_AIO_ID_NUM_MAX);
			int result = ::sceKernelAioWaitRequests(mActiveAio, mActiveAioCount, mStates, SCE_KERNEL_AIO_WAIT_OR, &timeout);
			if (result == SCE_OK)
			{

				// Process all the states
				AsyncResult result;
				AsyncErrorCode errorCode;
				size_t bytesTransferred;
				for (uint i = 0; i != mActiveAioCount;)
				{
					AsyncExecutorAsyncOp* asyncOp = mAsyncAioOps[i];
					if (HandleIoState(*asyncOp, mStates[i], result, errorCode, bytesTransferred))
					{
						ExecuteCompleted(*asyncOp->request, result, errorCode, bytesTransferred);
						mToDeleteAio[toDelete++] = mActiveAio[i];
						asyncOp->id = 0; // clear so the request doesn't try to delete
						--mActiveAioCount;
						mActiveAio[i] = mActiveAio[mActiveAioCount];
						mStates[i] = mStates[mActiveAioCount];
						mAsyncAioOps[i] = mAsyncAioOps[mActiveAioCount];
					}
					else
						++i;
				}

				// If the new end if less than the count, then we have to delete the requests
				if (toDelete)
				{
					int result = ::sceKernelAioDeleteRequests(mToDeleteAio, toDelete, mStates);
					EA_ASSERT(result == SCE_OK);
					EA_UNUSED(result);
				}
				waitResults = WaitResults::Request;
			}
			else if (result == SCE_KERNEL_ERROR_ETIMEDOUT)
			{
			}
			else
			{
				return WaitResults::Error;
			}
		}

		// If we have more requests or still have async IO in progress, do a quick poll.
		// We continue looping only if we don't have more requests pending
		if (hasMoreRequests || mActiveAioCount != 0)
		{
			WaitResults waitResults = WaitInternalRelative<SCE_KERNEL_AIO_ID_NUM_MAX>(SceKernelUseconds{ 0 });
			switch (waitResults)
			{
			case WaitResults::Request:
				waitResults = WaitResults::Request;
				break;
			case WaitResults::Timeout:
				waitResults = WaitResults::Timeout;
				break;
			case WaitResults::Error:
				return WaitResults::Error;
			}
		}
		else
		{
			WaitResults waitResults = WaitInternalAbsolute<SCE_KERNEL_AIO_ID_NUM_MAX>(alertableAbsoluteTimeout);
			switch (waitResults)
			{
			case WaitResults::Request:
				waitResults = WaitResults::Request;
				break;
			case WaitResults::Timeout:
				waitResults = WaitResults::Timeout;
				break;
			case WaitResults::Error:
				return WaitResults::Error;
			}
		}
		return waitResults;
	}

private:
	SceKernelAioSubmitId* mActiveAio{ nullptr };
	SceKernelAioSubmitId* mToDeleteAio{ nullptr };
	AsyncExecutorAsyncOp** mAsyncAioOps{ nullptr };
	uint mActiveAioCount{ 0 };
	int* mStates{ nullptr };
};

#if defined(EA_PLATFORM_PS5)
class MeasureAprBuilder
	: public AprBuilder
{
public:
	virtual void Read(void* buffer, uint32_t handle, uint64_t offset, uint64_t size) override
	{
		int elements = sce::Ampr::MeasureAprCommandSize::readFile((SceAprFileId)handle, buffer, size, offset);
		EA_ASSERT(elements >= 0);
		m_numElements += elements;
	}

	virtual void ReadGather(uint64_t offset, uint64_t size) override
	{
		int elements = sce::Ampr::MeasureAprCommandSize::readFileGather(size, offset);
		EA_ASSERT(elements >= 0);
		m_numElements += elements;
	}

	virtual void ReadScatter(void* buffer, uint64_t size) override
	{
		int elements = sce::Ampr::MeasureAprCommandSize::readFileScatter(buffer, size);
		EA_ASSERT(elements >= 0);
		m_numElements += elements;
	}

	virtual void ReadGatherScatter(void* buffer, uint64_t offset, uint64_t size) override
	{
		int elements = sce::Ampr::MeasureAprCommandSize::readFileGatherScatter(buffer, size, offset);
		EA_ASSERT(elements >= 0);
		m_numElements += elements;
	}

	size_t GetSize() const { return m_numElements; }

private:
	int m_numElements{ 0 };
};

class BuildAprBuilder
	: public AprBuilder
{

public:
	BuildAprBuilder(sce::Ampr::AprCommandBuffer& apr, bool validate)
		: mApr(apr)
		, mValidate(validate)
	{}

	virtual void Read(void* buffer, uint32_t handle, uint64_t offset, uint64_t size) override
	{
		ValidateMemory(buffer, size);
		int ret = mApr.readFile((SceAprFileId)handle, buffer, size, offset);
		EA_UNUSED(ret);
		EA_ASSERT(ret == 0);
	}

	virtual void ReadGather(uint64_t offset, uint64_t size) override
	{
		int ret = mApr.readFileGather(size, offset);
		EA_UNUSED(ret);
		EA_ASSERT(ret == 0);
	}

	virtual void ReadScatter(void* buffer, uint64_t size) override
	{
		ValidateMemory(buffer, size);
		int ret = mApr.readFileScatter(buffer, size);
		EA_UNUSED(ret);
		EA_ASSERT(ret == 0);
	}

	virtual void ReadGatherScatter(void* buffer, uint64_t offset, uint64_t size) override
	{
		ValidateMemory(buffer, size);
		int ret = mApr.readFileGatherScatter(buffer, size, offset);
		EA_UNUSED(ret);
		EA_ASSERT(ret == 0);
	}

private:
	void ValidateMemory(void* buffer, uint64_t size)
	{
		if (mValidate)
		{
			int prot;
			int32_t results = sceKernelQueryMemoryProtection(buffer, nullptr, nullptr, &prot);
			EA_UNUSED(results);
			EA_ASSERT(results >= 0);
			EA_ASSERT((prot & SCE_KERNEL_PROT_AMPR_RW) == SCE_KERNEL_PROT_AMPR_RW);
		}
	}

	sce::Ampr::AprCommandBuffer& mApr;
	bool mValidate;
};

class MeasureAmmBuilder
	: public AmmBuilder
{
public:
	virtual void Map(uint64_t va, uint64_t size, int type, int prot) override
	{
		int elements = sce::Ampr::MeasureAmmCommandSize::map(va, size, type, prot);
		EA_ASSERT(elements >= 0);
		m_numElements += elements;
	}

	virtual void MapDirect(uint64_t va, uint64_t dmemOffset, uint64_t size, int type, int prot) override
	{
		int elements = sce::Ampr::MeasureAmmCommandSize::mapDirect(va, dmemOffset, size, type, prot);
		EA_ASSERT(elements >= 0);
		m_numElements += elements;
	}

#if SCE_PROSPERO_SDK_VERSION < 0x00950000u

	virtual void MultiMap(uint64_t va, uint64_t vaAlias, uint64_t vaSize, int type, int prot) override
	{
#if SCE_PROSPERO_SDK_VERSION >= 0x00900020u
		int elements = sce::Ampr::MeasureAmmCommandSize::multiMap(va, vaAlias, vaSize, prot);
#else
		int elements = sce::Ampr::MeasureAmmCommandSize::multiMap(va, vaAlias, vaSize, type, prot);
#endif
		EA_ASSERT(elements >= 0);
		m_numElements += elements;
	}

	virtual void Remap(uint64_t vaNewStart, uint64_t vaOldStart, uint64_t vaSize, int type, int prot) override
	{
#if SCE_PROSPERO_SDK_VERSION >= 0x00900020u
		int elements = sce::Ampr::MeasureAmmCommandSize::remap(vaNewStart, vaOldStart, vaSize, prot);
#else
		int elements = sce::Ampr::MeasureAmmCommandSize::remap(vaNewStart, vaOldStart, vaSize, type, prot);
#endif
		EA_ASSERT(elements >= 0);
		m_numElements += elements;
	}
#endif

	virtual void Unamp(uint64_t va, uint64_t size) override
	{
		int elements = sce::Ampr::MeasureAmmCommandSize::unmap(va, size);
		EA_ASSERT(elements >= 0);
		m_numElements += elements;
	}

	size_t GetSize() const { return m_numElements * sizeof(uint32_t); }

private:
	int m_numElements{ 0 };
};

class BuildAmmBuilder
	: public AmmBuilder
{

public:
	BuildAmmBuilder(sce::Ampr::AmmCommandBuffer& amm, bool validate)
		: mAmm(amm)
	{}

	virtual void Map(uint64_t va, uint64_t size, int type, int prot) override
	{
		int ret = mAmm.map(va, size, type, prot);
		EA_UNUSED(ret);
		EA_ASSERT(ret == 0);
	}

	virtual void MapDirect(uint64_t va, uint64_t dmemOffset, uint64_t size, int type, int prot) override
	{
		int ret = mAmm.mapDirect(va, dmemOffset, size, type, prot);
		EA_UNUSED(ret);
		EA_ASSERT(ret == 0);
	}

#if SCE_PROSPERO_SDK_VERSION < 0x00950000u
	virtual void MultiMap(uint64_t va, uint64_t vaAlias, uint64_t vaSize, int type, int prot) override
	{
#if SCE_PROSPERO_SDK_VERSION >= 0x00900020u
		int ret = mAmm.multiMap(va, vaAlias, vaSize, prot);
#else
		int ret = mAmm.multiMap(va, vaAlias, vaSize, type, prot);
#endif
		EA_UNUSED(ret);
		EA_ASSERT(ret == 0);
	}

	virtual void Remap(uint64_t vaNewStart, uint64_t vaOldStart, uint64_t vaSize, int type, int prot) override
	{
#if SCE_PROSPERO_SDK_VERSION >= 0x00900020u
		int ret = mAmm.remap(vaNewStart, vaOldStart, vaSize, prot);
#else
		int ret = mAmm.remap(vaNewStart, vaOldStart, vaSize, type, prot);
#endif
		EA_UNUSED(ret);
		EA_ASSERT(ret == 0);
	}
#endif
	virtual void Unamp(uint64_t va, uint64_t size) override
	{
		int ret = mAmm.unmap(va, size);
		EA_UNUSED(ret);
		EA_ASSERT(ret == 0);
	}

private:
	sce::Ampr::AmmCommandBuffer& mAmm;
};

class AmprIoRequestImpl : public AmprIoRequest
{
	// NOTES: APR and AMM differ in the API available (as of 850)
	// To do a synchronous execution, for APR we can use the API.  But for AMM we have to spin waiting for a memory location.
public:
	AmprIoRequestImpl(AsyncExecutor& asyncExecutor, const AmprIoCallback& func, size_t builderCount)
		: AmprIoRequest(asyncExecutor, builderCount)
		, mFunc(func)
	{
	}

	~AmprIoRequestImpl()
	{
	}

	virtual void InvokeCallback(AsyncResult result, const AsyncErrorCode& errorCode, size_t bytesTransferred) override final
	{
		if (mFunc)
			mFunc(result, errorCode);
	}

protected:
	AmprIoCallback mFunc;
};
#endif

/////////////////////////////////////////////////////////////////////////////////////////////////

void PlatformInit(EA::Allocator::ICoreAllocator* allocator)
{
	s_sonyAsyncGlobalState = reinterpret_cast<SonyAsyncGlobalState*>(allocator->Alloc(
		sizeof(SonyAsyncGlobalState), "EASonyAsyncGlobal", 0, alignof(SonyAsyncGlobalState), 0));
	new (s_sonyAsyncGlobalState)SonyAsyncGlobalState();

#if defined(EA_PLATFORM_PS5)
	s_sonyAsyncGlobalState->allocator = allocator;
	size_t totalSize = kFastCommandBufferCount * kFastCommandBufferSize;
	s_sonyAsyncGlobalState->fastCommandBuffersBegin = allocator->Alloc(totalSize, "FastCommandBuffer", 0, kFastCommandBufferAlignment);
	s_sonyAsyncGlobalState->fastCommandBuffersEnd = (void*)(uintptr_t(s_sonyAsyncGlobalState->fastCommandBuffersBegin) + totalSize);
	for (size_t i = 0; i < kFastCommandBufferCount; ++i)
	{
		FreeFastCommandBuffer* cmdBuf = (FreeFastCommandBuffer*)(uintptr_t(s_sonyAsyncGlobalState->fastCommandBuffersBegin) + (i * kFastCommandBufferSize));
		cmdBuf->mNext = nullptr;
		s_sonyAsyncGlobalState->freeCommandBuffers.Push(cmdBuf);
	}
#endif
}

EA::IO::Internal::AsyncExecutorImpl* PlatformCreateExecutor(EA::Allocator::ICoreAllocator* const allocator, const Internal::ExecutorKey& key)
{
	using namespace Internal;

	switch (key.type)
	{
	case AsyncExecutorType::Concurrent:
		switch (key.platformSpecificData)
		{
		case kDefault:
		{
			SonyConcurrentAsyncFileExecutorImpl* exec = reinterpret_cast<SonyConcurrentAsyncFileExecutorImpl*>(
				allocator->Alloc(sizeof(SonyConcurrentAsyncFileExecutorImpl), "SonyConcurrentAsyncExecutorImpl",
					0, alignof(SonyConcurrentAsyncFileExecutorImpl), 0));
			new (exec)SonyConcurrentAsyncFileExecutorImpl(allocator, key);
			return exec;
		}
		default:
			EA_ASSERT(false);
			return nullptr;
		}
	case AsyncExecutorType::Sequential:
	{
		SonySequentialAsyncExecutorImpl* exec = reinterpret_cast<SonySequentialAsyncExecutorImpl*>(
			allocator->Alloc(sizeof(SonySequentialAsyncExecutorImpl), "SonySequentialAsyncExecutorImpl",
				0, alignof(SonySequentialAsyncExecutorImpl), 0));
		new (exec)SonySequentialAsyncExecutorImpl(allocator, key);
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
#if defined(EA_PLATFORM_PS5)
	allocator->Free(s_sonyAsyncGlobalState->fastCommandBuffersBegin);
#endif
	allocator->Free(s_sonyAsyncGlobalState);
}

} // namespace Internal

/////////////////////////////////////////////////////////////////////////////////////////////////

#if defined(EA_PLATFORM_PS5)
AmprIoRequestBuilder::CommandBuffer::CommandBuffer() { }
AmprIoRequestBuilder::CommandBuffer::CommandBuffer(CommandBuffer&& rhs) { EA_FAIL(); } // should never be called
AmprIoRequestBuilder::CommandBuffer::~CommandBuffer() { EA_ASSERT(type == Type::Unknown && commandBuffer == nullptr); };

AmprIoRequestBuilder::AmprIoRequestBuilder(EA::Allocator::ICoreAllocator* allocator, size_t builderCount)
{
	mCommandBuffers.set_allocator(allocator);
	mCommandBuffers.reserve(builderCount);
}

AmprIoRequestBuilder::~AmprIoRequestBuilder()
{
	Reset();
}

void AmprIoRequestBuilder::Build(size_t index, AprBuilderCallback&& builder)
{
	Build<Type::Apr>(index, eastl::move(builder));
}

void AmprIoRequestBuilder::Build(size_t index, AmmBuilderCallback&& builder) 
{
	Build<Type::Amm>(index, eastl::move(builder));
}

void AmprIoRequestBuilder::Finalize()
{
	for (size_t i = 0, e = mCommandBuffers.size(); i < e; ++i)
	{
		CommandBuffer& cmd = mCommandBuffers[i];
		bool first = i == 0;
		bool last = i == e - 1;

		switch (cmd.type)
		{
		case Type::Apr: Finalize<Type::Apr>(cmd, i, first, last); break;
		case Type::Amm: Finalize<Type::Amm>(cmd, i, first, last); break;
		case Type::Unknown: EA_FAIL(); break;
		}
	}
}

template <AmprIoRequestBuilder::Type TypeValue>
void AmprIoRequestBuilder::Build(size_t index, typename Types<TypeValue>::BuilderCallbackType&& builder)
{
	static uint64_t dummyValue;

	// Measure the base size of the command buffer
	typename Types<TypeValue>::MeasureBuilderType measure;
	builder(measure, AmprBuilderPhase::Measure);
	size_t size = measure.GetSize();
	if (size == 0)
		return;

	// If we already have command buffers, then we will need to perform a wait 
	if (!mCommandBuffers.empty())
	{
		int elements = sce::Ampr::MeasureCommandSize::waitOnAddress(
			&dummyValue,
			mCommandBuffers.size(),
			SCE_AMPR_WAIT_COMPARE_EQUAL,
			SCE_AMPR_WAIT_COMMAND_FETCH_FLUSH_DISABLE);
		EA_ASSERT(elements >= 0);
		size += elements;
	}

	// Allocate space for a trailing count update
	{
		int elements = sce::Ampr::MeasureCommandSize::writeAddressOnCompletion(
			&dummyValue,
			mCommandBuffers.size() + 1);
		EA_ASSERT(elements >= 0);
		size += elements;
	}

	// Allocate space for a possible kernel event
	{
		int elements = sce::Ampr::MeasureCommandSize::writeKernelEventQueueOnCompletion(SceKernelEqueue(1), 1, 1);
		EA_ASSERT(elements >= 0);
		size += elements;
	}

	// Make room for a uint64_t at the end
	if (index == 0)
	{
		size += sizeof(uint64_t) - 1;
		size &= ~size_t(sizeof(uint64_t) - 1);
		size += sizeof(uint64_t);
	}

	// Add the command to the table
	CommandBuffer& cmd = mCommandBuffers.push_back();
	cmd.type = TypeValue;
	new (&Types<TypeValue>::GetBuilderCallback(cmd)) typename Types<TypeValue>::BuilderCallbackType(builder);
	cmd.measureSize = size;
	new (&Types<TypeValue>::GetCommandBuffer(cmd)) typename Types<TypeValue>::CommandBufferType();
}

template <AmprIoRequestBuilder::Type TypeValue>
void AmprIoRequestBuilder::Finalize(CommandBuffer& cmd, size_t index, bool first, bool last)
{
	cmd.commandBuffer = AllocSonyCommandBuffer(cmd.measureSize);

	typename Types<TypeValue>::CommandBufferType& commandBuffer = Types<TypeValue>::GetCommandBuffer(cmd);
	typename Types<TypeValue>::BuilderCallbackType& builderCallback = Types<TypeValue>::GetBuilderCallback(cmd);

	if (first)
		mCommandsCompleted = reinterpret_cast<uint64_t*>(reinterpret_cast<uintptr_t>(cmd.commandBuffer) + cmd.measureSize - sizeof(uint64_t));

	commandBuffer.setBuffer(cmd.commandBuffer, cmd.measureSize);
	if (!first)
	{
		int ret = commandBuffer.waitOnAddress(
			mCommandsCompleted,
			index,
			SCE_AMPR_WAIT_COMPARE_EQUAL,
			SCE_AMPR_WAIT_COMMAND_FETCH_FLUSH_DISABLE);
		EA_UNUSED(ret);
		EA_ASSERT(ret == 0);
	}

	typename Types<TypeValue>::BuildBuilderType builder(commandBuffer, Internal::s_sonyAsyncGlobalState->amprMemoryValidation);
	builderCallback(builder, AmprBuilderPhase::Build);

	int ret = commandBuffer.writeAddressOnCompletion(
		mCommandsCompleted,
		index + 1);
	EA_UNUSED(ret);
	EA_ASSERT(ret == 0);
}

void AmprIoRequestBuilder::SubmitSync(AsyncResult& result, AsyncErrorCode& errorCode)
{
	if (IsEmpty())
	{
		errorCode = EA::IO::AsyncErrorCode(0);
		return;
	}

	for (size_t i = 0, e = mCommandBuffers.size(); i < e; ++i)
	{
		CommandBuffer& cmd = mCommandBuffers[i];

		switch (cmd.type)
		{
		case Type::Unknown:
			EA_ASSERT_MSG(false, "Command buffer is in an invalid state");
			break;
		case Type::Apr:
		{
			if (i == e - 1)
			{
				int out = sce::Ampr::Apr::submitCommandBufferAndGetResult(&cmd.apr, sce::Ampr::Apr::Priority::kPriority5, &cmd.resultBuffer, &cmd.submitId);
				EA_ASSERT_FORMATTED(out >= 0, ("Error submitting command buffer: %d", out));
				if (out < 0)
				{
					result = EA::IO::AsyncResult::Error;
					errorCode = EA::IO::AsyncErrorCode(out);
					return;
				}

				out = sce::Ampr::Apr::waitCommandBufferCompletion(cmd.submitId);
				EA_ASSERT_FORMATTED(out >= 0, ("Error submitting command buffer: %d", out));
				if (out < 0)
				{
					result = EA::IO::AsyncResult::Error;
					errorCode = EA::IO::AsyncErrorCode(out);
					return;
				}
				EA_ASSERT(cmd.resultBuffer.result == 0);
				result = EA::IO::AsyncResult::Success;
				return;
			}
			else
			{
				int out = sce::Ampr::Apr::submitCommandBuffer(&cmd.apr, sce::Ampr::Apr::Priority::kPriority5);
				EA_ASSERT_FORMATTED(out >= 0, ("Error submitting command buffer: %d", out));
				if (out < 0)
				{
					result = EA::IO::AsyncResult::Error;
					errorCode = EA::IO::AsyncErrorCode(out);
					return;
				}
			}
			break;
		}
		case Type::Amm:
		{
			EA_ASSERT_FORMATTED(i != e - 1, ("AMM request can not be the last element"));
			int out = sce::Ampr::Amm::submitCommandBuffer(&cmd.amm, sce::Ampr::Amm::Priority::kMid);
			EA_ASSERT_FORMATTED(out >= 0, ("Error submitting command buffer: %d", out));
			if (out < 0)
			{
				result = EA::IO::AsyncResult::Error;
				errorCode = EA::IO::AsyncErrorCode(out);
				return;
			}
			break;
		}
		}
	}
}

bool AmprIoRequestBuilder::SubmitAsync(AsyncResult& result, AsyncErrorCode& errorCode)
{
	if (IsEmpty())
	{
		errorCode = EA::IO::AsyncErrorCode(0);
		return true;
	}

	for (size_t i = 0, e = mCommandBuffers.size(); i < e; ++i)
	{
		CommandBuffer& cmd = mCommandBuffers[i];

		switch (cmd.type)
		{
		case Type::Unknown:
			EA_ASSERT_MSG(false, "Command buffer is in an invalid state");
			break;
		case Type::Apr:
		{
			int out = sce::Ampr::Apr::submitCommandBufferAndGetResult(&cmd.apr, sce::Ampr::Apr::Priority::kPriority5, &cmd.resultBuffer, &cmd.submitId);
			EA_ASSERT_FORMATTED(out >= 0, ("Error submitting command buffer: %d", out));
			if (out < 0)
			{
				result = EA::IO::AsyncResult::Error;
				errorCode = EA::IO::AsyncErrorCode(out);
				return true;
			}
			break;
		}
		case Type::Amm:
		{
			EA_ASSERT_FORMATTED(i != e - 1, ("AMM request can not be the last element"));
			int out = sce::Ampr::Amm::submitCommandBuffer(&cmd.amm, sce::Ampr::Amm::Priority::kMid);
			EA_ASSERT_FORMATTED(out >= 0, ("Error submitting command buffer: %d", out));
			if (out < 0)
			{
				result = EA::IO::AsyncResult::Error;
				errorCode = EA::IO::AsyncErrorCode(out);
				return true;
			}
			break;
		}
		}
	}

	result = EA::IO::AsyncResult::Success;
	return false;
}

bool AmprIoRequestBuilder::SubmitAsync(AsyncResult& result, AsyncErrorCode& errorCode, SceKernelEqueue kq, int eventId, uintptr_t eventData)
{
	if (IsEmpty())
	{
		errorCode = EA::IO::AsyncErrorCode(0);
		return true;
	}
	
	CommandBuffer& cmd = mCommandBuffers.back();
	switch (cmd.type)
	{
	case Type::Unknown:
		EA_ASSERT_MSG(false, "Command buffer is in an invalid state");
		break;
	case Type::Apr:
	{
		int ret = cmd.apr.writeKernelEventQueueOnCompletion(
			kq,
			eventId,
			eventData);
		EA_UNUSED(ret);
		EA_ASSERT(ret == 0);
		break;
	}
	case Type::Amm:
	{
		int ret = cmd.amm.writeKernelEventQueueOnCompletion(
			kq,
			eventId,
			eventData);
		EA_UNUSED(ret);
		EA_ASSERT(ret == 0);
		break;
	}
	}

	return SubmitAsync(result, errorCode);
}

void AmprIoRequestBuilder::WaitAsync(AsyncResult& result, AsyncErrorCode& errorCode)
{
	result = AsyncResult::Success;
	errorCode = AsyncErrorCode{ 0 };
	WaitAsyncInternal(result, errorCode);
}

void AmprIoRequestBuilder::Reset()
{
	for (CommandBuffer& cmd : mCommandBuffers)
	{
		switch (cmd.type)
		{
		case Type::Unknown:
			break;
		case Type::Apr:
			cmd.aprBuilderCallback.~AprBuilderCallback();
			cmd.apr.~AprCommandBuffer();
			break;
		case Type::Amm:
			cmd.ammBuilderCallback.~AmmBuilderCallback();
			cmd.amm.~AmmCommandBuffer();
			break;
		}
		cmd.type = Type::Unknown;
		EA_ASSERT(cmd.commandBuffer);
		FreeSonyCommandBuffer(cmd.commandBuffer);
		cmd.commandBuffer = nullptr; // don't remove, needed for assert
	}

	mCommandBuffers.clear();
	mCommandsCompleted = 0;
}

void AmprIoRequestBuilder::WaitAsyncInternal(AsyncResult& result, AsyncErrorCode& errorCode)
{
	if (IsEmpty())
		return;

	CommandBuffer& cmd = mCommandBuffers.back();
	switch (cmd.type)
	{
	case Type::Unknown:
	case Type::Amm:
		break;
	case Type::Apr:
	{
		int out = sce::Ampr::Apr::waitCommandBufferCompletion(cmd.submitId);
		EA_ASSERT(out >= 0);
		if (out < 0)
		{
			result = EA::IO::AsyncResult::Error;
			errorCode = EA::IO::AsyncErrorCode(out);
		}
		break;
	}
	}
}
#endif

/////////////////////////////////////////////////////////////////////////////////////////////////

#if defined(EA_PLATFORM_PS5)
AmprIoRequest::AmprIoRequest(AsyncExecutor& asyncExecutor, size_t builderCount)
	: AmprIoRequestBuilder(asyncExecutor.GetAllocator(), builderCount)
	, mAsyncExecutor(asyncExecutor)
{
}

AsyncIoMode AmprIoRequest::GetAsyncIoMode() const
{
	return AsyncIoMode::Wait;
}

bool AmprIoRequest::Execute(AsyncResult& result, AsyncErrorCode& errorCode, size_t& bytesTransferred)
{
	bytesTransferred = 0;

	if (!IsAsync())
	{
		SubmitSync(result, errorCode);
		return true;
	}
	else
	{
		SceKernelEqueue kq = GetSonyKernelQueue(mAsyncExecutor);
		int eventId = static_cast<Internal::SonyAsyncExecutorImpl&>(mAsyncExecutor).AcquireAmprRequestId();
		return SubmitAsync(result, errorCode, kq, eventId, uintptr_t(this));
	}
}

bool AmprIoRequest::IsDone(AsyncResult& result, AsyncErrorCode& errorCode, size_t& bytesTransferred) 
{
	if (!AmprIoRequestBuilder::IsDone())
		return false;

	result = EA::IO::AsyncResult::Success;
	bytesTransferred = 0;
	return true;
}

void AmprIoRequest::Complete(AsyncResult result, const AsyncErrorCode& errorCode, size_t bytesTransferred)
{
	EA::IO::AsyncResult localResult{ result };
	EA::IO::AsyncErrorCode localErrorCode{ errorCode };

	if (IsAsync())
		WaitAsyncInternal(localResult, localErrorCode);
	InvokeCallback(localResult, localErrorCode, bytesTransferred);
}

bool AmprIoRequest::IsAsync() const 
{ 
	return !EA_SONY_AMPR_FORCE_SYNC && mAsyncExecutor.GetType() == AsyncExecutorType::Concurrent;
}
#endif

/////////////////////////////////////////////////////////////////////////////////////////////////

_SceKernelEqueue* GetSonyKernelQueue(AsyncExecutor& asyncExecutor)
{
	Internal::SonyAsyncExecutorImpl& exec = static_cast<Internal::SonyAsyncExecutorImpl&>(asyncExecutor);
	return exec.GetKernelQueue();
}

#if defined(EA_PLATFORM_PS5)
void* AllocSonyCommandBuffer(AsyncExecutor& asyncExecutor, size_t size)
{
	return AllocSonyCommandBuffer(size);
}

void FreeSonyCommandBuffer(AsyncExecutor& asyncExecutor, void* buffer)
{
	FreeSonyCommandBuffer(buffer);
}

void SetSonyAmprMemoryValidation(AsyncExecutor& asyncExecutor, bool validate)
{
	SetSonyAmprMemoryValidation(validate);
}

void* AllocSonyCommandBuffer(size_t size)
{
	EA_ASSERT(Internal::s_sonyAsyncGlobalState != nullptr);
	void* buffer = nullptr;
	if (size <= Internal::kFastCommandBufferSize)
		buffer = (void*)Internal::s_sonyAsyncGlobalState->freeCommandBuffers.Pop();
	if (buffer == nullptr)
	{
		EA::Allocator::ICoreAllocator* allocator = Internal::s_sonyAsyncGlobalState->allocator;
		size = (size + Internal::kFastCommandBufferSize - 1) & ~(Internal::kFastCommandBufferSize - 1);
		buffer = allocator->Alloc(size, "SlowCommandBuffer", 0, Internal::kFastCommandBufferAlignment);
	}
	return buffer;
}

void FreeSonyCommandBuffer(void* buffer)
{
	EA_ASSERT(Internal::s_sonyAsyncGlobalState != nullptr);
	EA_ASSERT((uintptr_t(buffer) & (Internal::kFastCommandBufferSize - 1)) == 0);
	if (buffer >= Internal::s_sonyAsyncGlobalState->fastCommandBuffersBegin &&
		buffer < Internal::s_sonyAsyncGlobalState->fastCommandBuffersEnd)
	{
		((Internal::FreeFastCommandBuffer*)buffer)->mNext = nullptr;
		Internal::s_sonyAsyncGlobalState->freeCommandBuffers.Push((Internal::FreeFastCommandBuffer*)buffer);
	}
	else
	{
		EA::Allocator::ICoreAllocator* allocator = Internal::s_sonyAsyncGlobalState->allocator;
		allocator->Free(buffer);
	}
}

void SetSonyAmprMemoryValidation(bool validate)
{
	EA_ASSERT(Internal::s_sonyAsyncGlobalState != nullptr);
	Internal::s_sonyAsyncGlobalState->amprMemoryValidation = validate;
}

AmprIoRequest& CreateAmprRequest(AsyncExecutor& asyncExecutor, const AmprIoCallback& callback, size_t builderCount)
{
	return CreateAmprRequest<Internal::AmprIoRequestImpl>(eastl::ref(asyncExecutor), eastl::cref(callback), builderCount);
}
#endif

void BindToSonyExecutor(AsyncExecutor& asyncExecutor, int fd)
{
	if (asyncExecutor.GetType() == AsyncExecutorType::Concurrent)
	{
		Internal::SonyConcurrentAsyncFileExecutorImpl& exec = static_cast<Internal::SonyConcurrentAsyncFileExecutorImpl&>(asyncExecutor);
		exec.Bind(fd);
	}
}

void UnbindFromSonyExecutor(AsyncExecutor& asyncExecutor, int fd)
{
	if (asyncExecutor.GetType() == AsyncExecutorType::Concurrent)
	{
		Internal::SonyConcurrentAsyncFileExecutorImpl& exec = static_cast<Internal::SonyConcurrentAsyncFileExecutorImpl&>(asyncExecutor);
		exec.Unbind(fd);
	}
}

void FreeAsyncOp(AsyncExecutor& asyncExecutor, AsyncExecutorAsyncOp* op)
{
	if (op == nullptr)
		return;

	if (op->id != 0)
	{
		int ret;
		::sceKernelAioDeleteRequest(op->id, &ret);
	}
	Internal::SonyConcurrentAsyncFileExecutorImpl& exec = static_cast<Internal::SonyConcurrentAsyncFileExecutorImpl&>(asyncExecutor);
	exec.FreeAsyncOp(op);
}

bool IsAsyncOpDone(AsyncExecutor& asyncExecutor, AsyncExecutorAsyncOp& op, AsyncResult& result, AsyncErrorCode& errorCode, size_t& bytesTransferred)
{
	int state = 0;
	int results = ::sceKernelAioPollRequest(op.id, &state);
	if (results != SCE_OK)
	{
		errorCode = AsyncErrorCode(results);
		result = AsyncResult::Error;
		return true;
	}

	return Internal::SonyAsyncExecutorImpl::HandleIoState(op, state, result, errorCode, bytesTransferred);
}

bool AsyncRead(AsyncExecutor& asyncExecutor, AsyncExecutorAsyncOp*& op,
	AsyncResult& result, AsyncErrorCode& errorCode, size_t& bytesTransferred,
	AsyncIoRequest* request, int fd, size_type position, void* destination, size_type size)
{
	Internal::SonyConcurrentAsyncFileExecutorImpl& exec = static_cast<Internal::SonyConcurrentAsyncFileExecutorImpl&>(asyncExecutor);
	return exec.Read(result, errorCode, bytesTransferred, request, fd, position, destination, size, op);
}

bool AsyncWrite(AsyncExecutor& asyncExecutor, AsyncExecutorAsyncOp*& op,
	AsyncResult& result, AsyncErrorCode& errorCode, size_t& bytesTransferred,
	AsyncIoRequest* request, int fd, size_type position, const void* source, size_type size)
{
	Internal::SonyConcurrentAsyncFileExecutorImpl& exec = static_cast<Internal::SonyConcurrentAsyncFileExecutorImpl&>(asyncExecutor);
	return exec.Write(result, errorCode, bytesTransferred, request, fd, position, source, size, op);
}

} // namespace IO
} // namespace EA
