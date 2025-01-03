/////////////////////////////////////////////////////////////////////////////
// EAAsyncExecutorWin32.h
//
// Win32 specific definitions for the executor
/////////////////////////////////////////////////////////////////////////////

#ifndef EAIO_EAASYNCEXECUTOR_WIN32_H
#define EAIO_EAASYNCEXECUTOR_WIN32_H

#if defined(EA_PLATFORM_XBSX)
#include <dstorage_xs.h>
#include <EAIO/internal/EAIOEASTLCoreAllocator.h>
#include <EASTL/numeric_limits.h>
#include <EASTL/vector.h>
#endif

namespace EA
{
	namespace IO
	{
		struct AsyncExecutorAsyncOp;

		enum PlatformSpecificType
		{
			kDefault = 0,
			kWaitObjects = kDefault,
		};

		EAIO_API void BindToWin32Executor(AsyncExecutor& asyncExecutor, void* handle);
		EAIO_API void UnbindFromWin32Executor(AsyncExecutor& asyncExecutor, void* handle);

		EAIO_API bool AsyncRead(AsyncExecutor& asyncExecutor, AsyncExecutorAsyncOp*& op,
			AsyncResult& result, AsyncErrorCode& errorCode, size_t& bytesTransferred,
			AsyncIoRequest* request, void* handle, size_type position, void* destination, size_type size);

		EAIO_API bool AsyncWrite(AsyncExecutor& asyncExecutor, AsyncExecutorAsyncOp*& op,
			AsyncResult& result, AsyncErrorCode& errorCode, size_t& bytesTransferred,
			AsyncIoRequest* request, void* handle, size_type position, const void* source, size_type size);

		EAIO_API bool IsAsyncOpDone(AsyncExecutor& asyncExecutor, AsyncExecutorAsyncOp& op,
			AsyncResult& result, AsyncErrorCode& errorCode, size_t& bytesTransferred,
			void* handle);

		EAIO_API void FreeAsyncOp(AsyncExecutor& asyncExecutor, AsyncExecutorAsyncOp* op);

#if defined(EA_PLATFORM_XBSX)

		IDStorageFactoryX& GetDStorageFactory();
		IDStorageQueueX& GetDStorageQueue();
		IDStorageStatusArrayX& GetDStorageStatusArray(AsyncExecutor& asyncExecutor);
		uint32_t AcquireStatusArrayIndex(AsyncExecutor& asyncExecutor);
		void ReleaseStatusArrayIndex(AsyncExecutor& asyncExecutor, uint32_t index);

		// Used to generate requests to read data
		class DStorageBuilder
		{
		public:
			virtual ~DStorageBuilder() = default;

			virtual void Read(void* destination, uint32_t destinationSize, IDStorageFileX& file, uint64_t fileOffset, uint32_t fileSize, uint64_t options) = 0;
		};

		// When building the request, this callbacks is invoked by the builder to allow the caller
		// to populate the I/O requests.
		using DStorageBuilderCallback = eastl::function<void(DStorageBuilder&)>;

		// EAIO notification of a DStorage I/O completing
		using DStorageIoCallback = eastl::fixed_function<32, void(AsyncResult result, const AsyncErrorCode & ec)>;

		// Base class for creating DStorage requests that are processed by the AsyncExecutor.
		// Override the InvokeCallback method to send any required notifications
		class EAIO_API DStorageIoRequest
			: public AsyncIoRequest
		{
		public:
			using Requests_t = eastl::vector<DSTORAGE_REQUEST, EA::Allocator::EAIOEASTLCoreAllocator>;

		public:
			DStorageIoRequest(AsyncExecutor& asyncExecutor, size_t readCountHint);
			virtual ~DStorageIoRequest();

			void Build(const DStorageBuilderCallback& builder);

			virtual AsyncIoMode GetAsyncIoMode() const override final;
			virtual bool Execute(AsyncResult& result, AsyncErrorCode& errorCode, size_t& bytesTransferred) override final;
			virtual bool IsDone(AsyncResult& result, AsyncErrorCode& errorCode, size_t& bytesTransferred) override final;
			virtual void Complete(AsyncResult result, const AsyncErrorCode& errorCode, size_t bytesTransferred) override final;

		protected:
			virtual void InvokeCallback(AsyncResult result, const AsyncErrorCode& errorCode, size_t bytesTransferred) = 0;
			void WaitInternal(AsyncResult& result, AsyncErrorCode& errorCode);
			bool IsAsync() const;

			AsyncExecutor& mAsyncExecutor;
			Requests_t mRequests;
			uint32_t mStatusIndex{ eastl::numeric_limits<uint32_t>::max() };
		};

		// Create an EAIO specific DStorage request specific to delivering an DStorageIoCallback notification
		// In general, use the DStorageSubmit method to automate the whole process of creating a request
		DStorageIoRequest& CreateDStorageRequest(AsyncExecutor& asyncExecutor, size_t readCountHint, const DStorageIoCallback& callback);

		// Create an application defined DStorage request for delivering specialized notifications
		// In general, use the DStorageSubmit method to automate the whole process of creating a request
		template <typename RequestType, typename CallbackType>
		RequestType& CreateDStorageRequest(AsyncExecutor& asyncExecutor, size_t readCountHint, const CallbackType& callback)
		{
			return asyncExecutor.Alloc<RequestType>(eastl::ref(asyncExecutor), readCountHint, eastl::cref(callback));
		}

		// Create an submit an DStorage request
		inline AsyncRequestId DStorageSubmit(AsyncExecutor& asyncExecutor, size_t readCountHint, const DStorageIoCallback& callback, const DStorageBuilderCallback& builder)
		{
			AsyncRequestId outRequestId{ 0 };
			DStorageIoRequest& request = CreateDStorageRequest(asyncExecutor, readCountHint, callback);
			if ((request.mFlags & kARFHasId) != 0)
				outRequestId = static_cast<AsyncQueueableRequest&>(request).mId;

			request.Build(builder);

			asyncExecutor.Push(request);
			return outRequestId;
		}

		// Create an submit an DStorage request
		template <typename RequestType, typename CallbackType>
		AsyncRequestId DStorageSubmit(AsyncExecutor& asyncExecutor, size_t readCountHint, const CallbackType& callback, const DStorageBuilderCallback& builder)
		{
			AsyncRequestId outRequestId{ 0 };
			RequestType& request = CreateDStorageRequest<RequestType>(asyncExecutor, readCountHint, callback);
			if ((request.mFlags & kARFHasId) != 0)
				outRequestId = static_cast<AsyncQueueableRequest&>(request).mId;

			request.Build(builder);

			asyncExecutor.Push(request);
			return outRequestId;
		}

#endif

	} // namespace IO

} // namespace EA

#endif  // Header include guard
