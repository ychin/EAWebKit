///////////////////////////////////////////////////////////////////////////////
// EAHandleAllocator
// 
// Copyright (c) 2004, Electronic Arts. All Rights Reserved.
//
//  *** This module is deprecated and not maintained. ***
///////////////////////////////////////////////////////////////////////////////


#ifndef PPMALLOC_EAHANDLEALLOCATOR_H
#define PPMALLOC_EAHANDLEALLOCATOR_H


#include <PPMalloc/internal/config.h>
#include <PPMalloc/EAGeneralAllocator.h>
#include <PPMalloc/EAFixedAllocator.h>
#include <stddef.h>


namespace EA
{
	namespace Allocator
	{
		///////////////////////////////////////////////////////////////////////////////
		/// class HandleAllocator
		///
		/// @brief Implements a handle-based layer on top of GeneralAllocator. 
		///
		class PPM_API HandleAllocator
		{ 
		public:
			typedef void* Handle;       /// Handles identify allocated memory. A handle with a value of zero is invalid.

			enum CompactionLevel
			{
				kCompactionLevelNone,   /// No compaction.
				kCompactionLevelMinor,  /// Small but quick compaction.
				kCompactionLevelMajor   /// Large but slower compaction.
			};

			enum CompactionStrategy
			{
				kCompactionStrategyNone,   /// 
				kCompactionStrategyLow,    /// Attempt to compact memory downward in address space.
				kCompactionStrategyHigh,   /// Attempt to compact memory upward in address space.
				kCompactionStrategyDefault /// Use the default strategy.
			};

			/// HandleAllocator
			/// Constructs the HandleAllocator with the given GeneralAllocator. The GeneralAllocator
			/// may be NULL, which means to postpone association of initialization until Init is called.
			/// If constructed with a non-NULL pGeneralAllocator, then Init does not need to be called.
			HandleAllocator(EA::Allocator::GeneralAllocator* pGeneralAllocator = NULL, size_t nInitialHandleCount = 256);

			/// ~HandleAllocator
			/// Destroy all resources associated with the HandleAllocator. Frees all memory 
			/// that is still allocated. Does not shut down or destroy the associated GeneralAllocator.
			PPM_VIRTUAL ~HandleAllocator();

			/// Init
			/// Initializes the HandleAllocator with a given GeneralAllocator. The GeneralAllocator
			/// may be one that is used for other purposes beyond handle-based allocations. It may
			/// for example be your primary application allocator.
			PPM_VIRTUAL bool Init(EA::Allocator::GeneralAllocator* pGeneralAllocator, size_t nInitialHandleCount = 256);

			/// Shutdown
			/// De-initializes the HandleAllocator and returns it to its newly constructed state.
			PPM_VIRTUAL bool Shutdown();

			/// Malloc
			/// Allocates a block of the given size. Returns a valid handle or zero. 
			PPM_VIRTUAL Handle Malloc(size_t nSize);

			/// MallocAligned
			/// Allocates a block of the given size, alignment, and alignment offset. Returns a valid handle or zero. 
			PPM_VIRTUAL Handle MallocAligned(size_t nSize, size_t nAlignment, size_t nAlignmentOffset = 0);

			/// Calloc
			/// Allocates a block as with the standard calloc function. Returns a valid handle or zero. 
			PPM_VIRTUAL Handle Calloc(size_t nElementCount, size_t nElementSize);

			/// Realloc
			/// Reallocates a block identified by a given handle. The handle must be locked (lock count of non-zero),
			/// otherwise any use of outstanding locked pointers for the handle may fail. Either way, the lock
			/// count for the handle is not modified during the Realloc operation. If the Realloc operation 
			/// fails (with a return value of false), the Handle will continue to validly refer to the previous memory.
			PPM_VIRTUAL bool Realloc(Handle h, size_t nNewSize);
	
			/// Free
			/// Releases the memory allocated to the handle and makes the handle unavailable for further use.
			/// The handle should be locked (lock count of zero), otherwise any use of outstanding locked 
			/// pointers for the handle will yield unpredictable behaviour.
			PPM_VIRTUAL void Free(Handle h);

			/// Lock
			/// Yields a usable pointer from the input Handle and locks the data in place for 
			/// use until Unlock is called. Locks are cumulative; three Locks must be matched
			/// by three Unlocks for the handle to become fully unlocked (with a lock count of zero).
			/// There are a maximum of 32767 possible locks for a handle. If there is a need for
			/// more than this amount, then it is not likely that a handle-based system is going
			/// to be very benefical.
			PPM_VIRTUAL void* Lock(Handle h);

			/// Unlock
			/// Unlocks a handle to match a Lock call. An attempt to Unlock a handle more times
			/// than it was locked results in no action taken. 
			PPM_VIRTUAL void Unlock(Handle h);

			/// CompactHandle
			/// Explicitly attempts to relocate the block referred to by the given handle to a 
			/// more desirable location. The given handle must be unlocks or there will be no 
			/// action taken. The return value is true if the handle could be relocated (regardless 
			/// of if it actually was relocated).
			PPM_VIRTUAL bool CompactHandle(Handle h, CompactionStrategy strategy = kCompactionStrategyDefault);

			/// Compact
			/// Explictly attempts to compact all handles associated with the HandleAllocator.
			PPM_VIRTUAL void Compact(CompactionLevel level = kCompactionLevelMajor, CompactionStrategy strategy = kCompactionStrategyDefault);

			/// ValidateHandle
			/// Returns true if the given input handle is currently valid.
			PPM_VIRTUAL bool ValidateHandle(Handle h);

			/// GetHandleFromAddress
			/// Given a pointer to valid memory returned by Lock, returns the Handle associated with the pointer.
			/// Returns NULL if the pointer is not associated with a Handle. This is a potentially expensive
			/// function and should not generally be used except for debugging purposes.
			PPM_VIRTUAL Handle GetHandleFromAddress(const void* pData);

			/// GetLockCount
			/// Returns the current lock count for the given handle. The lock count can never go below zero.
			PPM_VIRTUAL unsigned GetLockCount(Handle h);

			/// Runtime Options
			enum Option{
				kOptionNone,               /// Placeholder for no options.
				kOptionEnableThreadSafety  /// If non-zero, enabled thread-safety, otherwise disable it. If you enable thread safety here you must ensure that the GeneralAllocator associated with this HandleAllocator also has thread-safety enabled.
			};

			/// SetOption
			/// Runtime options
			PPM_VIRTUAL void SetOption(Option option, int nValue);

		private:
			// Copying this structure doesn't make sense, and leads to an inconsistent state.
			HandleAllocator(const HandleAllocator&) {}
			void operator=(const HandleAllocator&) {}

		protected:
			static void* CoreAllocationFunction(size_t nSize, void* pContext);
			static void  CoreFreeFunction(void* pCore, void* pContext);

			enum { kMaxLockCount = ((1 << 15) - 1) };

			/// HandleInfo
			/// @brief Internal-use struct for HandleAllocator
			///
			/// A Handle is really a pointer to HandleInfo.
			struct HandleInfo {
				void* mpData;                    /// Current pointer to allocation.
				struct {
					int mnLockCount       : 15;  /// Lock count. When zero, the pointer is relocatable. Max value is 32767. If you need this many locks, then you likely can't benefit from handles.
					int mnAlignment       :  7;  /// Alignment, for MallocAligned. Max value is 255.
					int mnAlignmentOffset :  8;  /// Alignment offset, for MallocAligned. Max value is 255.
					int mbHigh            :  2;  /// If true, then CompactionStrategy for this handle is kCompactionStrategyHigh.
				} mBitfield;
			};

			// We create a union of char[] and HandleInfo in order to allow us to 
			// alias the two (i.e. use them interchangeably). 
			union CharHandleInfo {
				char       mData;
				HandleInfo mHandleInfo;
			};

		protected:
			/// FixedAllocatorEx
			/// @brief Internal-use struct for HandleAllocator
			class PPM_API FixedAllocatorEx : public FixedAllocator<HandleInfo> {
			public:
				enum { kCountPerCoreBlock = 512 };

				FixedAllocatorEx() : FixedAllocator<HandleInfo>(kCountPerCoreBlock){}

				HandleInfo* FindHandleInfo(const void* pData);
				bool        ValidateAddress(const Handle h);

				void*       BeginGetNext();
				HandleInfo* GetNext(void* pContext);
				void        EndGetNext(void* pContext);

			private:
				// Copying this structure doesn't make sense, and leads to an inconsistent state.
				FixedAllocatorEx(const FixedAllocatorEx&) : FixedAllocator<HandleInfo>(kCountPerCoreBlock) {}
				void operator=(const FixedAllocatorEx&) {}

			protected:
				struct IterationContext{
					CoreBlock*  mpCurrentCoreBlock;
					HandleInfo* mpCurrentHandleInfo;
				};
				IterationContext mIterationContext;
			};

		protected:
			int               mnHandleCount;
			FixedAllocatorEx  mHandleInfoAllocator;
			GeneralAllocator* mpGeneralAllocator;
			void*             mpMutex;
			char              mpMutexData[32];          /// Provides memory from which the mutex is allocated (so we don't have to allocate it from a heap). Format of this data is platform-specific.

		}; // class HandleAllocator

	} // namespace Allocator

} // namespace EA



#endif // PPMALLOC_EAHANDLEALLOCATOR_H












