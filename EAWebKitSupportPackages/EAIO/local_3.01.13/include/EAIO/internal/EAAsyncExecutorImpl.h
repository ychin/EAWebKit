/////////////////////////////////////////////////////////////////////////////
// EAAsyncExecutorImpl.h
//
// Copyright (c) 2019, Electronic Arts Inc. All rights reserved.
/////////////////////////////////////////////////////////////////////////////

#ifndef EAIO_EAASYNCEXECUTORIMPL_H
#define EAIO_EAASYNCEXECUTORIMPL_H

#include <EABase/eabase.h>
#include <EAIO/EAAsyncExecutor.h>
#include <EAIO/internal/EAIOEASTLCoreAllocator.h>
#include <EAStdC/EAString.h>
#include <EASTL/fixed_hash_map.h>
#include <EASTL/priority_queue.h>
#include <EASTL/vector.h>
#include <eathread/eathread_semaphore.h>
#include <eathread/eathread_thread.h>
#include <eathread/eathread_atomic.h>

namespace EA
{
namespace IO
{

namespace Internal
{

constexpr size_t Log2(size_t n)
{
	return (n < 2) ? 0 : 1 + Log2(n/2);
}

template <size_t Alignment>
class TaggedPointer
{
public:

	/**
	 * Use uint64_t as the tagged pointer size for all platforms
	 * ARM32 and x86 both support 64-bit atomics for the versions of those ISA that we support
	 */

#if EA_PLATFORM_PTR_SIZE == 4
	static constexpr uint64_t HighBitCount = 32;
	static constexpr uint64_t LowBitCount = 0;
	static constexpr uint64_t PointerBitCount = 32;
#else
	static constexpr uint64_t HighBitCount = 7;
	static constexpr uint64_t LowBitCount = Log2(Alignment);
	static constexpr uint64_t PointerBitCount = 57;
#endif
	static constexpr uint64_t CounterBitCount = HighBitCount + LowBitCount;
	static constexpr uint64_t CounterBitMask = ((uint64_t(1) << CounterBitCount) - 1);

	static constexpr uint64_t HighBitTaggedMask = (((uint64_t(1) << HighBitCount) - 1) << PointerBitCount);
	static constexpr uint64_t LowBitTaggedMask = ((uint64_t(1) << LowBitCount) - 1);
	static constexpr uint64_t PointerTaggedMask = (((uint64_t(1) << PointerBitCount) - 1) << LowBitCount);

	static constexpr uint64_t HighBitCounterMask = (((uint64_t(1) << HighBitCount) - 1) << LowBitCount);
	static constexpr uint64_t LowBitCounterMask = ((uint64_t(1) << LowBitCount) - 1);
	static constexpr uint64_t PointerMask = ((uint64_t(1) << PointerBitCount) - 1);

	static void* ExtractPointer(uint64_t x)
	{
		const uint64_t ret = (((x & PointerMask) & ~HighBitTaggedMask) & ~LowBitTaggedMask);
		return reinterpret_cast<void*>(ret);
	}

	static uint32_t ExtractCounter(uint64_t x)
	{
		const uint64_t lowBit = (x & LowBitTaggedMask);
		const uint64_t highBit = (x & HighBitTaggedMask) >> (PointerBitCount);
		return static_cast<uint32_t>(((highBit << LowBitCount) | (lowBit)) & CounterBitMask);
	}

	static uint64_t MakeTagged(void* ptr, uint32_t counter)
	{
		const uint64_t uintPtr = (reinterpret_cast<uint64_t>(ptr) & PointerMask);
		const uint64_t lowBitCounter = (static_cast<uint64_t>(counter) & LowBitCounterMask);
		const uint64_t highBitCounter = (static_cast<uint64_t>(counter) & HighBitCounterMask) >> LowBitCount;

		return (highBitCounter << PointerBitCount) | (uintPtr) | (lowBitCounter);
	}

	TaggedPointer() = default;
	~TaggedPointer() = default;

	TaggedPointer(const TaggedPointer& rhs)
		: mValue(rhs.GetValue())
	{
	}

	explicit TaggedPointer(uint64_t value)
		: mValue(value)
	{
	}

	explicit TaggedPointer(void* ptr, uint32_t counter)
		: mValue(MakeTagged(ptr, counter))
	{
	}

	void* GetPointer() const
	{
		return ExtractPointer(GetValue());
	}

	uint32_t GetCounter() const
	{
		return ExtractCounter(GetValue());
	}

	uint64_t GetValue() const
	{
		return EA::Thread::AtomicGetValue(&mValue);
	}

	uint64_t GetValueRaw() const
	{
		return mValue;
	}

	void* GetPointerRaw() const
	{
		return ExtractPointer(GetValueRaw());
	}

	uint32_t GetCounterRaw() const
	{
		return ExtractCounter(GetValueRaw());
	}

	uint32_t GetCounterIncrementedRaw() const
	{
		return ExtractCounter(GetValueRaw()) + 1;
	}

	void SetValue(uint64_t value)
	{
		EA::Thread::AtomicSetValue(&mValue, value);
	}

	void SetPointerAndCounter(void* ptr, uint32_t counter)
	{
		SetValue(MakeTagged(ptr, counter));
	}

	uint64_t SetValueRaw(uint64_t value)
	{
		const uint64_t ret = GetValueRaw();
		mValue = value;
		return ret;
	}

	void SetPointerAndCounterRaw(void* ptr, uint32_t counter)
	{
		SetValueRaw(MakeTagged(ptr, counter));
	}

	bool SetValueConditional(uint64_t desired, uint64_t expected)
	{
		return EA::Thread::AtomicSetValueConditional(&mValue, desired, expected);
	}

	bool SetValueConditional(const TaggedPointer& desired, const TaggedPointer& expected)
	{
		return SetValueConditional(desired.GetValue(), expected.GetValue());
	}

private:

	// Assume we have 64-bit Atomic Instructions on 32-bit platforms
	EA_PREFIX_ALIGN(8) uint64_t mValue EA_POSTFIX_ALIGN(8) = 0;
};

template <typename T, size_t Offset>
class AtomicIntrusiveStack
{
	using TaggedPtrType = TaggedPointer<alignof(T)>;
public:
	AtomicIntrusiveStack() = default;
	~AtomicIntrusiveStack() = default;

public:

	T* Pop()

	{
		return static_cast<T*>(PopUntyped());
	}

	void Push(T* object)
	{
		PushUntyped(static_cast<void*>(object));
	}

	T* PeelOff()
	{
		return static_cast<T*>(PeelOffUntyped());
	}

private:

	void* PopUntyped()
	{
		for (;;)
		{
			const uint64_t value = mHead.GetValue();
			void* object = TaggedPtrType::ExtractPointer(value);

			if (object == nullptr)
			{
				return nullptr;
			}

			// NOTE(mwinkler):
			// Notice we read from head to get the next
			//
			// There Can Be Only one consumer thread
			//
			// Or if there are MORE Consumer Threads AND
			//
			// if the node is not backed by some form
			// of SMR(Safe Memory Reclamation), LFRC(Lock Free Reference Counting),
			// Allocator that does not free back to the OS (i.e. common Pool allocator)
			// there is a potentional that after the node is removed and deleted, another thread may
			// read from head (the now deleted node).

			uintptr_t* ptrToNext = BasePtrToNextPtr(object, Offset);

			void* next = reinterpret_cast<void*>(EA::Thread::AtomicGetValue(ptrToNext));

			if (CompareXchg(mHead, TaggedPtrType{value}, next))
			{
				EACompilerMemoryBarrier();
				EAWriteBarrier();

				EA::Thread::AtomicSetValue(ptrToNext, (uintptr_t)0);

				return object;
			}

			EAProcessorPause();
		}
	}

	void PushUntyped(void* object)
	{
		uintptr_t* next = BasePtrToNextPtr(object, Offset);

		for (;;)
		{
			const uint64_t value = mHead.GetValue();

			EA::Thread::AtomicSetValue(next, reinterpret_cast<uintptr_t>(TaggedPtrType::ExtractPointer(value)));
			EACompilerMemoryBarrier();
			EAWriteBarrier();

			if (CompareXchg(mHead, TaggedPtrType{value}, object))
			{
				return;
			}

			EAProcessorPause();
		}
	}

	/* Peel Off as entire list resetting stack to empty */
	void* PeelOffUntyped()
	{
		for (;;)
		{
			const uint64_t value = mHead.GetValue();
			void* object = TaggedPtrType::ExtractPointer(value);

			if (object == nullptr)
			{
				return nullptr;
			}

			void* next = nullptr;

			if (CompareXchg(mHead, TaggedPtrType{value}, next))
			{
				return object;
			}

			EAProcessorPause();
		}
	}

private:

	static bool CompareXchg(TaggedPtrType& dest, TaggedPtrType expected, void* desired)
	{
		const uint32_t nextCounter = expected.GetCounterIncrementedRaw();
		return dest.SetValueConditional(TaggedPtrType{desired, nextCounter}, expected);
	}

	static uintptr_t* BasePtrToNextPtr(void* ptr, size_t offset)
	{
		return reinterpret_cast<uintptr_t*>(reinterpret_cast<char*>(ptr) + offset);
	}


private:
	TaggedPtrType mHead;
};

} // namespace Internal

} // namespace IO
} // namespace EA

namespace EA
{
	namespace IO
	{
		namespace Internal
		{
			// Forward declarations
			class AsyncExecutorImpl;
			struct AsyncCommandRequest;
		
			/////////////////////////////////////////////////////////////////////////////////////////////////
			// ExecutorKey - When an executor is needed, it can utilize an existing one with the same 
			//		key information.

			struct ExecutorKey
			{
				AsyncExecutorType type;
				int platformSpecificData;
				const char* device;

				bool operator == (const ExecutorKey& rhs) const
				{
					return
						type == rhs.type &&
						platformSpecificData == rhs.platformSpecificData &&
						EA::StdC::Stricmp(device, rhs.device) == 0;
				}
			};

			struct ExecutorKeyHash
			{
				size_t operator()(const ExecutorKey& key) const
				{
					size_t type = eastl::hash<int>{}(static_cast<int>(key.type));
					size_t data = eastl::hash<int>{}(key.platformSpecificData);
					size_t device = eastl::hash<char*>{}(key.device);
					return type ^ (data << 1) ^ (device << 2);
				}
			};

			class AsyncRequestAllocator
			{
			public:
				AsyncRequestAllocator(EA::Allocator::ICoreAllocator* coreAllocator, uint32_t size, uint32_t count, uint32_t alignment);

				void* Alloc(size_t requestSize, size_t alignment, uint32_t& extraFlags);
				void Free(AsyncRequest* request);

			private:
				EA::Allocator::ICoreAllocator* mCoreAllocator{ nullptr };
				uint32_t mSize{ 0 };
				uint32_t mCount{ 0 };
				uint32_t mCountMask{ 0 };
				uint32_t mAlignment{ 0 };
				uint8_t* mBaseData{ nullptr };
				uint32_t mNextIndex{ 0 };
			};

			/////////////////////////////////////////////////////////////////////////////////////////////////
			// Common base executor implementation.  Contains the code to handle the request queues without
			//		any of the wait code.

			class AsyncExecutorImpl
				: public AsyncExecutor
				, public EA::Thread::IRunnable
			{
			public:
				static constexpr int kCachedRequestAlignment = 16;
				static constexpr int kCachedRequestMaxSize = 512;
				static constexpr int kCachedRequestCount = 128;
				static constexpr uint32_t kDefaultActiveRequestLimit = 1024;
				static constexpr uint32_t kDefaultAlertableWaitTimeout = 50; // milliseconds
				static constexpr uint32_t kDefaultNotAltertableWaitTimeout = 1; // milliseconds

			public:
				AsyncExecutorImpl(EA::Allocator::ICoreAllocator* allocator, const ExecutorKey& key, uint32_t activeRequestLimit = kDefaultActiveRequestLimit);

				virtual ~AsyncExecutorImpl();

				virtual void Start();

				bool IsShutdown();

				virtual void Push(AsyncRequest& request) override final;

				virtual void Pause() override final;

				virtual void Resume() override final;

				virtual void Cancel(AsyncRequestId requestId) override final;

				virtual void Run(const EA::Thread::ThreadTime& waitTimeoutAbsolute, const EA::Thread::ThreadTime& runTimeoutAbsolute) override;

				virtual void RegisterRequestFamily(AsyncRequestFamily& requestFamily) override;

				virtual void UnregisterRequestFamily(AsyncRequestFamily& requestFamily) override;

				virtual EA::Allocator::ICoreAllocator* GetAllocator() override { return mCoreAllocator; }

				void AddPolledIoRequest(AsyncIoRequest& request) { mActivePolledRequests.push_back(&request); }

			protected:
				enum class WaitResults
				{
					Request,			// A request has been queued
					Timeout,			// The wait timedout
					Error,				// Error during the wait
				};

				virtual void ExecuteCommand(AsyncCommandRequest* command);

				virtual void ExecuteCancelCommand(AsyncRequestId requestId);

				virtual void SignalPushToOS(AsyncRequest& request) {}

				virtual WaitResults Wait(const EA::Thread::ThreadTime& alertableAbsoluteTimeout, const EA::Thread::ThreadTime& notAlertableAbsoluteTimeout, bool hasMoreRequests) = 0;

				void FreeRequestInternal(AsyncRequest* request);

				uint32_t GetActiveRequestCount() const { return mActiveRequestCount; }

				uint32_t GetActiveRequestLimit() const { return mActiveRequestLimit; }

				AsyncRequestFamily* GetRequestFamily(uint32_t id);

				void ExecuteCompleted(AsyncRequest& request, AsyncResult result, const AsyncErrorCode& errorCode, size_t bytesTransferred);

				WaitResults PopPendingRequests(bool poll, const EA::Thread::ThreadTime& waitTimeoutAbsolute);

				void SetAlertableWaitTimeout(uint32_t timeout) { mAlertableWaitTimeout = timeout; }

				void SetNotAlertableWaitTimeout(uint32_t timeout) { mNotAlertableWaitTimeout = timeout; }

				void BeginWait() { EA::Thread::AtomicSetValue(&mInWait, 1); }

				void EndWait() { EA::Thread::AtomicSetValue(&mInWait, 0); }

			private:
				virtual intptr_t Run(void* pContext) override;

				AsyncQueueableRequest* Pop();

				virtual void* AllocRequestInternal(size_t requestSize, size_t alignment, uint32_t& extraFlags) override;

				static AsyncRequest* Reverse(AsyncRequest* tail);

				void RegisterRequestFamilyInternal(AsyncRequestFamily& family);

				void UnregisterRequestFamilyInternal(AsyncRequestFamily& family);

				class CommandAsyncRequestFamily
					: public AsyncRequestFamilyTmpl<kAsyncCommandFamily>
				{
				public:
					virtual void Register(AsyncExecutor& executor) override;

					virtual void Unregister() override {}

					virtual AsyncExecuteImmediateResult ExecuteImmediate(AsyncRequest& request, bool blocking) override;

					virtual AsyncExecuteQueuedResult ExecuteQueued(AsyncRequest& request, bool blocking) override;

					virtual void ExecuteCompleted(AsyncRequest& request, AsyncResult result, const AsyncErrorCode& errorCode, size_t bytesTransferred) override;

				protected:
					AsyncExecutor* mExecutor{ nullptr };
				};

				class IoAsyncRequestFamily
					: public AsyncRequestFamilyTmpl<kAsyncIoFamily>
				{
				public:
					virtual void Register(AsyncExecutor& executor) override;

					virtual void Unregister() override {}

					virtual AsyncExecuteImmediateResult ExecuteImmediate(AsyncRequest& request, bool blocking) override;

					virtual AsyncExecuteQueuedResult ExecuteQueued(AsyncRequest& request, bool blocking) override;

					virtual void ExecuteCompleted(AsyncRequest& request, AsyncResult result, const AsyncErrorCode& errorCode, size_t bytesTransferred) override;

				protected:
					AsyncExecutorImpl* mExecutor{ nullptr };
				};

				using FamilyLookupMap = eastl::fixed_hash_map<uint32_t, AsyncRequestFamily*, 64, 65, true, eastl::hash<uint32_t>, eastl::equal_to<uint32_t>, false, EA::Allocator::EAIOEASTLCoreAllocator>;

				struct AsyncRequestCmp
				{
					bool operator()(AsyncQueueableRequest* lhs, AsyncQueueableRequest* rhs) const
					{
						if (lhs->mPriority != rhs->mPriority)
							return lhs->mPriority < rhs->mPriority;
						return lhs->mId > rhs->mId;
					}
				};

				using PendingRequestsQueue = eastl::priority_queue<AsyncQueueableRequest*, eastl::vector<AsyncQueueableRequest*, EA::Allocator::EAIOEASTLCoreAllocator>, AsyncRequestCmp>;

				EA::Thread::Thread mThread;
				EA::Thread::Semaphore mRequestNotification;
				uint32_t mRequestNotificationCount{ 0 };
				uint32_t mPriorRequestNotificationCount{ 0 };
				uint32_t mInWait{ 0 };
				uint32_t mAlertableWaitTimeout{ kDefaultAlertableWaitTimeout };
				uint32_t mNotAlertableWaitTimeout{ kDefaultNotAltertableWaitTimeout };
				EA::Allocator::ICoreAllocator* mCoreAllocator;
				const ExecutorKey& mKey;
				Internal::AtomicIntrusiveStack<AsyncRequest, offsetof(AsyncRequest, mNext)> mRequestStack;
				AsyncRequestAllocator mRequestAllocator;
				PendingRequestsQueue mRequests;
				AsyncQueueableRequest* mReQueueRequest{ nullptr };
				eastl::vector<AsyncIoRequest*, EA::Allocator::EAIOEASTLCoreAllocator> mActivePolledRequests;
				bool mShutdownEventReceived{ false };
				bool mIsPaused{ false };
				bool mIsBlocking{ false };
				uint32_t mActiveRequestCount{ 0 };
				uint32_t mActiveRequestLimit{ kDefaultActiveRequestLimit };
				CommandAsyncRequestFamily mCommandFamily;
				IoAsyncRequestFamily mIoFamily;
				FamilyLookupMap mFamilyLookupMap;
			};
		} // namespace Internal
	} // namespace IO
} // namespace EA

#endif // EAIO_EAASYNCEXECUTORIMPL_H
