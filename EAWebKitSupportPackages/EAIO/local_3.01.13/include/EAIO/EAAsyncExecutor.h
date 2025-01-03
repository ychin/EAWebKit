/////////////////////////////////////////////////////////////////////////////
// EAAsync.h
//
// Copyright (c) 2019, Electronic Arts Inc. All rights reserved.
/////////////////////////////////////////////////////////////////////////////


#ifndef EAIO_EAASYNCEXECUTOR_H
#define EAIO_EAASYNCEXECUTOR_H

#ifndef INCLUDED_eabase_H
	#include <EABase/eabase.h>
#endif

#include <EAIO/internal/Config.h>
#include <EAIO/EAAsyncTypes.h>
#include <eathread/eathread.h>
#include <EASTL/functional.h>

namespace EA
{
	namespace IO
	{
		// Forward declarations
		class AsyncExecutor;
		struct AsyncExecutorAsyncOp;
		
		enum AsyncRequestFlags : uint32_t
		{
			kARFActive		= 1 << 0,			/// Set when the request has been queued to the operating system and waiting for results
			kARFCachable	= 1 << 1,			/// Set when the underlying request data can be returned to the request cache
			kARFFree		= 1 << 2,			/// Set when the request is in the free table
			kARFHasId		= 1 << 3,			/// Set then derived from AsyncQueueableRequest
		};

		/// AsyncRequest
		/// Most base structure defining a request sent to an executor
		struct AsyncRequest
		{
			AsyncRequest(uint32_t family, uint32_t flags)
				: mFamily(family), mFlags(flags) {}
			virtual ~AsyncRequest() = default; // needed to avoid problems with subclassed commands that need a vtable

			AsyncRequest* mNext{ nullptr };
			uint32_t mFamily;
			uint32_t mFlags;
		};

		/// AsyncQueueableRequest
		/// Requests that need to be queued for non-immediate execution must be derived from this 
		/// request type.  It provides an id for later use and an associated priority.
		struct EAIO_API AsyncQueueableRequest : public AsyncRequest
		{
			AsyncQueueableRequest(uint32_t family, int32_t priority, uint32_t flags);

			virtual void Cancel() {}

			AsyncRequestId mId;
			int32_t mPriority{ 0 };
		};
		
		// Unique identifiers associated with async request families.  
		// See AsyncRequestFamily for how it is used.
		static constexpr uint32_t kAsyncCommandFamily = 'ACMD';
		static constexpr uint32_t kAsyncIoFamily = 'AIO ';

		/// AsyncIoMode 
		/// Provides a hint as to how the I/O will complete. Currently the requirements is that any
		/// I/O that needs IsDone to be invoked (i.e. polling for completion) must return Polled to 
		/// operate properly.
		enum class AsyncIoMode : uint8_t
		{
			Undefined,
			Blocking,
			Polled,
			Wait,
		};

		/// AsyncIoRequest
		/// Io requests are derived from this structure and are processed by the Io request family
		struct AsyncIoRequest : public AsyncQueueableRequest
		{
			AsyncIoRequest(int32_t priority = 0, uint32_t flags = 0) : AsyncQueueableRequest(kAsyncIoFamily, priority, flags) {}
			virtual ~AsyncIoRequest() = default;

			virtual AsyncIoMode GetAsyncIoMode() const { return AsyncIoMode::Undefined; }
			virtual bool Execute(AsyncResult& result, AsyncErrorCode& errorCode, size_t& bytesTransferred) = 0;
			virtual bool IsDone(AsyncResult& result, AsyncErrorCode& errorCode, size_t& bytesTransferred) = 0;
			virtual void Complete(AsyncResult result, const AsyncErrorCode& errorCode, size_t bytesTransferred) = 0;
		};

		enum class AsyncExecuteImmediateResult : uint8_t
		{
			Done,
			Queue,
			Error,
		};

		enum class AsyncExecuteQueuedResult : uint8_t
		{
			Done,
			Pending,
			Error,
			ReQueue,
		};

		/// AsyncRequestFamily
		///
		/// Requests families provide the mechanism for external code creators to add to the executor the
		/// ability to execute arbitrary requests.  For example, every executor provides a command request family (i.e.
		/// pause, resume, shutdown, ...), IO request family, and a function request family.  
		///
		/// External software, such as a Scatter Streamer can register the request family with an executor 
		/// and then queue requests specific to that family.
		class EAIO_API AsyncRequestFamily
		{
		public:
			virtual uint32_t GetFamilyId() const = 0;
			virtual void Register(AsyncExecutor& executor) = 0;
			virtual void Unregister() = 0;
			virtual AsyncExecuteImmediateResult ExecuteImmediate(AsyncRequest& request, bool blocking) = 0;
			virtual AsyncExecuteQueuedResult ExecuteQueued(AsyncRequest& request, bool blocking) = 0;
			virtual void ExecuteCompleted(AsyncRequest& request, AsyncResult result, const AsyncErrorCode& errorCode, size_t bytesTransferred) = 0;
		};

		template <uint32_t FamilyId>
		class AsyncRequestFamilyTmpl
			: public AsyncRequestFamily
		{
		public:
			static constexpr uint32_t kFamilyId = FamilyId;

			virtual uint32_t GetFamilyId() const override { return kFamilyId; }
		};

		/// AsyncExecutorType
		/// Type of executor needed to execute requests of the same class
		/// Sequential executors are for requests that can not be done asyncronously.
		/// Those requests are executed serially while the operation executes.
		/// Concurrent executors are for requests that support waiting for completion asyncronously.
		/// It is assumed that the Execute method is not blocking.  
		enum class AsyncExecutorType
		{
			Sequential,
			Concurrent,
			Count,
		};

		/// AsyncExecutor
		/// Handles the execution of any async requests
		class EAIO_API AsyncExecutor
		{
		public:
			static void Init(
				EA::Allocator::ICoreAllocator* allocator = nullptr);

			static AsyncExecutor* StartExecutor(
				AsyncExecutorType type,
				int platformSpecificData = 0, 
				const char* device = nullptr);

			static bool StopExecutors();

			static void Shutdown(
				const EA::Thread::ThreadTime& timeoutAbsolute = EA::Thread::kTimeoutNone);

			static int GetPollingSleepTimeout();

			// Number of milliseconds to pause between polls of non-waitable asynchronous I/O requests
			// Value of zero can result in the executor thread yielding to other threads.
			// Value of less than zero disables the sleep.
			static void SetPollingSleepTimeout(int timeout);

			virtual void Push(
				AsyncRequest& request) = 0;

			virtual void Pause() = 0;

			virtual void Resume() = 0;

			virtual void Cancel(
				AsyncRequestId requestId) = 0;

			virtual void RegisterRequestFamily(
				AsyncRequestFamily& requestFamily) = 0;

			virtual void UnregisterRequestFamily(
				AsyncRequestFamily& requestFamily) = 0;

			template <typename RequestType, typename ... Args>
			RequestType& Alloc(Args ... args)
			{
				uint32_t extraFlags = 0;
				void* ptr = AllocRequestInternal(sizeof(RequestType), EA_ALIGN_OF(RequestType), extraFlags);
				new (ptr) RequestType(args...);
				RequestType& request = *reinterpret_cast<RequestType*>(ptr);
				request.mFlags |= extraFlags;
				return request;
			}

			template <typename RequestType, typename ... Args>
			AsyncRequestId AllocAndPush(Args ... args)
			{
				AsyncRequestId outRequestId{ 0 };
				AsyncRequest& request = Alloc<RequestType>(args...);
				if ((request.mFlags & kARFHasId) != 0)
					outRequestId = static_cast<AsyncQueueableRequest&>(request).mId;
				Push(request);
				return outRequestId;
			}

			virtual void Run(
				const EA::Thread::ThreadTime& waitTimeoutAbsolute, 
				const EA::Thread::ThreadTime& runTimeoutAbsolute) = 0;

			virtual AsyncExecutorType GetType() = 0;

			virtual EA::Allocator::ICoreAllocator* GetAllocator() = 0;

		protected:
			virtual void* AllocRequestInternal(size_t requestSize, size_t alignment, uint32_t& extraFlags) = 0;
			virtual ~AsyncExecutor() = default;
		};

		/// AsyncFunctionRequest
		/// Simple request for invoking a function in the context of the service thread
		using AsyncFunctionCallback = eastl::function<void()>;
		class AsyncFunctionRequest : public AsyncIoRequest
		{
		public:
			AsyncFunctionRequest(AsyncFunctionCallback func) : mFunc(func) {}

			virtual AsyncIoMode GetAsyncIoMode() const override
			{
				return AsyncIoMode::Blocking;
			}

			virtual bool Execute(AsyncResult& result, AsyncErrorCode& errorCode, size_t& bytesTransferred) override
			{ 
				if (mFunc)
					mFunc(); 
				result = AsyncResult::Success;
				bytesTransferred = 0;
				return true; 
			}

			virtual bool IsDone(AsyncResult& result, AsyncErrorCode& errorCode, size_t& bytesTransferred) override
			{
				result = AsyncResult::Success;
				bytesTransferred = 0;
				return true;
			}

			virtual void Complete(AsyncResult result, const AsyncErrorCode& errorCode, size_t bytesTransferred) override
			{
			}

		private:
			AsyncFunctionCallback mFunc;
		};

		inline void AsyncFunction(AsyncExecutor* exec, AsyncFunctionCallback func)
		{
			exec->AllocAndPush<AsyncFunctionRequest>(func);
		}

	} // namespace IO

} // namespace EA

#include <EAIO/EAStream.h> // This is only needed for the off_type and size_type

#if defined(EA_PLATFORM_MICROSOFT)
	#include <EAIO/Win32/EAAsyncExecutorWin32.h>
#elif defined(EA_PLATFORM_ANDROID)
	#include <EAIO/StdC/EAAsyncExecutorStdC.h>
#elif defined(EA_PLATFORM_NX)
	#include <EAIO/StdC/EAAsyncExecutorStdC.h>
#elif EAIO_USE_UNIX_IO
	#include "Unix/EAAsyncExecutorUnix.h"
#elif defined(EA_PLATFORM_SONY)
	#include <EAIO/Sony/EAAsyncExecutorSony.h>
#else
#include <EAIO/StdC/EAAsyncExecutorStdC.h>
#endif

#endif // Header include guard














