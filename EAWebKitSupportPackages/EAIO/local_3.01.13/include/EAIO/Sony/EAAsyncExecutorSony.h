/////////////////////////////////////////////////////////////////////////////
// EAAsyncExecutorSony.h
//
// Sony specific definitions for the executor
/////////////////////////////////////////////////////////////////////////////

#ifndef EAIO_EAASYNCEXECUTOR_SONY_H
#define EAIO_EAASYNCEXECUTOR_SONY_H

#include <EASTL/functional.h>
#include <EASTL/fixed_function.h>

#if defined(EA_PLATFORM_PS5)
#include <EASTL/fixed_vector.h>
#include <ampr/ampr.h>
#endif

extern "C"
{
	struct _SceKernelEqueue;
}

namespace EA
{
	namespace IO
	{
		struct AsyncExecutorAsyncOp;

		enum PlatformSpecificType
		{
			kDefault = 0,
			kAsyncFileIo = kDefault,		// The default implementation handles both async fio, AMPR, and kernel events.
			kKernelEvents = kDefault,
		};

		_SceKernelEqueue* GetSonyKernelQueue(AsyncExecutor& asyncExecutor);

#if defined(EA_PLATFORM_PS5)

		// Allocate and free from a cache of AMPR command buffers. If there are no more cached
		// buffers, then a new one will be allocated.  All of these memory regions are 64k aligned
		// and thus have a minimum size of 64k.
		void* AllocSonyCommandBuffer(size_t size);
		void FreeSonyCommandBuffer(void* buffer);

		// When using the Apr/Amm builders, this enables the checking of buffer pointers to
		// verify that they are AMPR enabled memory regions.
		void SetSonyAmprMemoryValidation(bool validate);

		// Old API
		void* AllocSonyCommandBuffer(AsyncExecutor& asyncExecutor, size_t size);
		void FreeSonyCommandBuffer(AsyncExecutor& asyncExecutor, void* buffer);
		void SetSonyAmprMemoryValidation(AsyncExecutor& asyncExecutor, bool validate);

		// When using the Apr/Amm builders, they are invoked twice, once to measure the size
		// of the command buffer and then once to build the command buffer.
		enum class AmprBuilderPhase : uint8_t
		{
			Measure,
			Build,
		};

		// Used to generate command buffers to read data
		class AprBuilder
		{
		public:
			virtual ~AprBuilder() = default;

			virtual void Read(void* buffer, uint32_t handle, uint64_t offset, uint64_t size) = 0;
			virtual void ReadGather(uint64_t offset, uint64_t size) = 0;
			virtual void ReadScatter(void* buffer, uint64_t size) = 0;
			virtual void ReadGatherScatter(void* buffer, uint64_t offset, uint64_t size) = 0;
		};

		// Used to generate command buffers to map memory
		class AmmBuilder
		{
		public:
			virtual ~AmmBuilder() = default;

			virtual void Map(uint64_t va, uint64_t size, int type, int prot) = 0;
			virtual void MapDirect(uint64_t va, uint64_t dmemOffset, uint64_t size, int type, int prot) = 0;
#if SCE_PROSPERO_SDK_VERSION < 0x00950000u
			virtual void MultiMap(uint64_t va, uint64_t vaAlias, uint64_t vaSize, int type, int prot) = 0;
			virtual void Remap(uint64_t vaNewStart, uint64_t vaOldStart, uint64_t vaSize, int type, int prot) = 0;
#endif
			virtual void Unamp(uint64_t va, uint64_t size) = 0;
		};

		// When building command buffers, these callbacks are invoked by the builder to allow the caller
		// to populate the command buffers.
		using AprBuilderCallback = eastl::function<void(AprBuilder&, AmprBuilderPhase)>;
		using AmmBuilderCallback = eastl::function<void(AmmBuilder&, AmprBuilderPhase)>;

		// EAIO notification of an AMPR I/O completing
		using AmprIoCallback = eastl::fixed_function<32, void(AsyncResult result, const AsyncErrorCode& ec)>;

		namespace Internal
		{
		class MeasureAprBuilder;
		class BuildAprBuilder;
		class MeasureAmmBuilder;
		class BuildAmmBuilder;
		}

		// The AmprIoRequestBuilder is the base class that can be used to make AMPR requests.  It is used internally
		// and made publicly available when either synchronous calls are needed or asynchronous notifications are
		// being handled outside of the EAIO async executors.
		//
		// Example of doing synchronous AMPR request
		//
		//		EA::IO::AmprIoRequestBuilder request(FB_GLOBAL_ARENA.getICoreAllocator(), 1);
		//		request.Build(0, [this, destination, byteCount](EA::IO::AprBuilder& builder, EA::IO::AmprBuilderPhase builderPhase)
		//		{
		//			builder.Read(destination, m_fd, m_pos, byteCount);
		//		});
		//		request.Finalize();
		//
		//		EA::IO::AsyncResult results;
		//		EA::IO::AsyncErrorCode errorCode;
		//		request.SubmitSync(results, errorCode);
		//		if (results == EA::IO::AsyncResult::Success)
		//		{
		//			m_pos += byteCount;
		//			return 0;
		//		}
		//		else
		//		return errorCode.GetValue();
		//
		// Example of doing asynchronous AMPR request;
		//
		//		state->amprSize = size;
		//		state->amprBuilder->Build(0, [buffer, destination, offset, size](EA::IO::AprBuilder& builder, EA::IO::AmprBuilderPhase builderPhase)
		//		{
		//			builder.Read(destination, buffer->getNativeHandle(), offset, size);
		//		});
		//		state->amprBuilder->Finalize();
		//		
		//		EA::IO::AsyncResult results;
		//		EA::IO::AsyncErrorCode errorCode;
		//		state->amprBuilder->SubmitAsync(results, errorCode);
		//		if (results == EA::IO::AsyncResult::Success)
		//			return 0;
		//		else
		//			return errorCode.GetValue();
		//
		//		/* later */
		//
		//		EA::IO::AsyncResult results;
		//		EA::IO::AsyncErrorCode errorCode;
		//		state->amprBuilder->WaitAsync(results, errorCode);
		//		state->amprBuilder->Reset();
		//		if (results == EA::IO::AsyncResult::Success)
		//		{
		//			if (bytesRead)
		//				*bytesRead = state->amprSize;
		//			return true;
		//		}
		//		else
		//		{
		//			*error = errorCode.GetValue();
		//			return false;
		//		}
		class EAIO_API AmprIoRequestBuilder
		{
		public:
			enum class Type : uint8_t
			{
				Unknown,
				Apr,
				Amm,
			};

			struct CommandBuffer
			{
				CommandBuffer();
				CommandBuffer(CommandBuffer&& rhs);
				~CommandBuffer();

				union
				{
					AprBuilderCallback aprBuilderCallback;
					AmmBuilderCallback ammBuilderCallback;
				};
				union
				{
					sce::Ampr::AprCommandBuffer apr;
					sce::Ampr::AmmCommandBuffer amm;
				};
				void* commandBuffer{ nullptr };
				SceAprSubmitId submitId{ 0 };
				SceAprResultBuffer resultBuffer{ 0 };
				uint64_t measureSize{ 0 };
				Type type{ Type::Unknown };
			};

			// Helper structures to derive APR/AMM types from the enumeration
			template <Type TypeValue> struct Types {};

			template <>
			struct Types<Type::Apr>
			{
				using MeasureBuilderType = Internal::MeasureAprBuilder;
				using BuildBuilderType = Internal::BuildAprBuilder;
				using BuilderCallbackType = AprBuilderCallback;
				using MeasureType = sce::Ampr::MeasureAprCommandSize;
				using CommandBufferType = sce::Ampr::AprCommandBuffer;

				static BuilderCallbackType& GetBuilderCallback(CommandBuffer& cmd) { return cmd.aprBuilderCallback; }
				static CommandBufferType& GetCommandBuffer(CommandBuffer& cmd) { return cmd.apr; }
			};

			template <>
			struct Types<Type::Amm>
			{
				using MeasureBuilderType = Internal::MeasureAmmBuilder;
				using BuildBuilderType = Internal::BuildAmmBuilder;
				using BuilderCallbackType = AmmBuilderCallback;
				using MeasureType = sce::Ampr::MeasureAmmCommandSize;
				using CommandBufferType = sce::Ampr::AmmCommandBuffer;

				static BuilderCallbackType& GetBuilderCallback(CommandBuffer& cmd) { return cmd.ammBuilderCallback; }
				static CommandBufferType& GetCommandBuffer(CommandBuffer& cmd) { return cmd.amm; }
			};

			AmprIoRequestBuilder(EA::Allocator::ICoreAllocator* allocator, size_t builderCount);
			~AmprIoRequestBuilder();

			// API for building the AMPR request
			void Build(size_t index, AprBuilderCallback&& builder);
			void Build(size_t index, AmmBuilderCallback&& builder);
			void Finalize();

			// Post build API
			bool IsEmpty() const { return mCommandBuffers.empty(); }
			bool IsDone() const { return mCommandsCompleted != nullptr && *mCommandsCompleted >= mCommandBuffers.size(); }
			void SubmitSync(AsyncResult& result, AsyncErrorCode& errorCode);
			bool SubmitAsync(AsyncResult& result, AsyncErrorCode& errorCode);
			bool SubmitAsync(AsyncResult& result, AsyncErrorCode& errorCode, SceKernelEqueue kq, int eventId, uintptr_t eventData);
			void WaitAsync(AsyncResult& result, AsyncErrorCode& errorCode);
			void Reset();

		protected:
			// Internal version doesn't initialize output 
			void WaitAsyncInternal(AsyncResult& result, AsyncErrorCode& errorCode);

			eastl::fixed_vector<CommandBuffer, 2, true> mCommandBuffers;
			uint64_t* mCommandsCompleted{ nullptr };

		private:
			template <Type TypeValue>
			void Build(size_t index, typename Types<TypeValue>::BuilderCallbackType&& builder);

			template <Type TypeValue>
			void Finalize(CommandBuffer& cmd, size_t index, bool first, bool last);
		};

		// Base class for creating AMPR requests that are processed by the AsyncExecutor.
		// Override the InvokeCallback method to send any required notifications
		class EAIO_API AmprIoRequest
			: public AsyncIoRequest
			, public AmprIoRequestBuilder
		{
		public:
			AmprIoRequest(AsyncExecutor& asyncExecutor, size_t builderCount);
			virtual ~AmprIoRequest() = default;

			virtual AsyncIoMode GetAsyncIoMode() const override final;
			virtual bool Execute(AsyncResult& result, AsyncErrorCode& errorCode, size_t& bytesTransferred) override final;
			virtual bool IsDone(AsyncResult& result, AsyncErrorCode& errorCode, size_t& bytesTransferred) override final;
			virtual void Complete(AsyncResult result, const AsyncErrorCode& errorCode, size_t bytesTransferred) override final;

		protected:
			virtual void InvokeCallback(AsyncResult result, const AsyncErrorCode& errorCode, size_t bytesTransferred) = 0;
			bool IsAsync() const;

			AsyncExecutor& mAsyncExecutor;
		};

		// Create an EAIO specific AMPR request specific to delivering an AmprIoCallback notification
		// In general, use the AmprSubmit method to automate the whole process of creating a request
		AmprIoRequest& CreateAmprRequest(AsyncExecutor& asyncExecutor, const AmprIoCallback& callback, size_t builderCount);

		// Create an application defined AMPR request for delivering specialized notifications
		// In general, use the AmprSubmit method to automate the whole process of creating a request
		template <typename RequestType, typename ... Args>
		RequestType& CreateAmprRequest(AsyncExecutor& asyncExecutor, Args&& ... args)
		{
			return asyncExecutor.Alloc<RequestType>(eastl::ref(asyncExecutor), args...);
		}

		// Helper methods to invoke the builders
		inline void AmprBuild(AmprIoRequest& request, size_t index)
		{
			return;
		}

		template <typename BuilderCallback, typename ... BuilderCallbacks>
		void AmprBuild(AmprIoRequest& request, size_t index, BuilderCallback&& builder, BuilderCallbacks&& ... builders)
		{
			request.Build(index, builder);
			AmprBuild(request, index + 1, builders...);
		}

		// Create an submit an AMPR request
		template <typename ... BuilderCallbacks>
		AsyncRequestId AmprSubmit(AsyncExecutor& asyncExecutor, const AmprIoCallback& callback, BuilderCallbacks&& ... builders)
		{
			AsyncRequestId outRequestId{ 0 };
			AmprIoRequest& request = CreateAmprRequest(asyncExecutor, callback, sizeof...(builders));
			if ((request.mFlags & kARFHasId) != 0)
				outRequestId = static_cast<AsyncQueueableRequest&>(request).mId;

			AmprBuild(request, 0, eastl::forward<BuilderCallbacks...>(builders...));
			request.Finalize();

			asyncExecutor.Push(request);
			return outRequestId;
		}

		// Create an submit an AMPR request
		template <typename RequestType, typename CallbackType, typename ... BuilderCallbacks>
		AsyncRequestId AmprSubmit(AsyncExecutor& asyncExecutor, const CallbackType& callback, BuilderCallbacks&& ... builders)
		{
			AsyncRequestId outRequestId{ 0 };
			RequestType& request = CreateAmprRequest<RequestType>(asyncExecutor, callback, sizeof...(builders));
			if ((request.mFlags & kARFHasId) != 0)
				outRequestId = static_cast<AsyncQueueableRequest&>(request).mId;

			AmprBuild(request, 0, eastl::forward<BuilderCallbacks...>(builders...));
			request.Finalize();

			asyncExecutor.Push(request);
			return outRequestId;
		}
#endif

		void BindToSonyExecutor(AsyncExecutor& asyncExecutor, int fd);
		void UnbindFromSonyExecutor(AsyncExecutor& asyncExecutor, int fd);

		bool AsyncRead(AsyncExecutor& asyncExecutor, AsyncExecutorAsyncOp*& op,
			AsyncResult& result, AsyncErrorCode& errorCode, size_t& bytesTransferred,
			AsyncIoRequest* request, int fd, size_type position, void* destination, size_type size);

		bool AsyncWrite(AsyncExecutor& asyncExecutor, AsyncExecutorAsyncOp*& op, 
			AsyncResult& result, AsyncErrorCode& errorCode, size_t& bytesTransferred, 
			AsyncIoRequest* request, int fd, size_type position, const void* source, size_type size);

		bool IsAsyncOpDone(AsyncExecutor& asyncExecutor, AsyncExecutorAsyncOp& op,
			AsyncResult& result, AsyncErrorCode& errorCode, size_t& bytesTransferred);

		void FreeAsyncOp(AsyncExecutor& asyncExecutor, AsyncExecutorAsyncOp* op);
	} // namespace IO

} // namespace EA

#endif  // Header include guard
