///////////////////////////////////////////////////////////////////////////////////////////////
// ScratchpadAllocator
//
// Copyright (c) 2014, Electronic Arts. All Rights Reserved.
//
// ScratchpadAllocator is an allocator adapter which implements a per-thread 
// StackAllocator (a.k.a. LinearAllocator). The purpose of this is to allow code 
// to make fast temporary allocations from a heap without interfering with the 
// main application heap(s). It works by maintaining a container of StackAllocators, 
// with each StackAllocator being created on-demand per thread.
//
// Who would want to use this?
// This class is useful for when you want to quickly allocate temporary memory in a 
// function but don't want to hit the main heap for it. Allocating this memory is 
// much faster than allocating main heap memory because it's allocated via a 
// StackAllocator (a.k.a. LinearAllocator) with no thread synchronization required.
// Avoiding the main heap has the benefit of reducing memory fragmentation in that heap
//
// How efficient is this system?
// There are two primary costs in this system: 1) a Futex lock/unlock in the construction
// of the first (and only the first) instance of a ScratchpadAllocator for a thread.
// and 2) the heap allocation cost needed when a ScratchpadAllocator needs to expand
// its memory. The large majority of the time there are no synchronization primitives
// required and no heap allocation done and so allocation is similar in speed to directly
// using a growable StackAllocator or alloca.
//
// Basic usage:
// The end-user of ScratchpadAllocator simply creates a local instance of ScratchpadAllocator
// in a function, and uses its Malloc and (optionally) Free functionality. You can declare
// these in multiple functions, as per the example usage below. End users don't normally 
// need to care about the ScratchpadManager class except for custom cases, and can usually
// leave the ScratchpadManager constructor argument NULL. However, if you don't want to 
// use the default global ScratchpadManager then you can create one yourself and use it
// instead of the default one. This will result in a separate StackAllocator that you can
// use instead of, and in parallel to, the default one.
//
// The application normally needs to create the default global ScratchpadManager instance
// which serves as the default for ScratchpadAllocator instances. You would normally do
// this on startup by declarating an instances of ScratchpadManager, calling its setup
// functions and calling SetScratchpadManager with it. The ScratchpadAllocator class
// instances will automatically find that global manager class via GetScratchpadManager.
///////////////////////////////////////////////////////////////////////////////////////////////


#ifndef PPMALLOC_EASCRATCHPADALLOCATOR_H
#define PPMALLOC_EASCRATCHPADALLOCATOR_H


#include <PPMalloc/EAStackAllocator.h>
#include <eathread/eathread_storage.h>
#include <eathread/eathread_futex.h>


namespace EA
{
	namespace Allocator
	{
		class  ScratchpadManager;
		struct StackAllocatorInfo;


		/// ScratchpadAllocator
		///
		/// There are two primary limiting requirements of ScratchpadAllocator:
		///    - For any individual thread, the order of ScratchpadAllocator destruction must be the reverse of construction. 
		///      If you you create instances of it on the stack like in the example below, this will naturally work out.
		///    - The user of a ScratchpadAllocator may not call its Malloc if another ScratchpadAllocator has become the 
		///      current one for the thread. This could conceivably occur if you create two ScratchpadAllocators in a 
		///      single function scope (which use the same manager) and interweave Malloc/Free calls to them.
		///
		/// Example usage:
		///    void DoSomething1() {
		///        ScratchpadAllocator scratchpad;
		///
		///        void* p1 = scratchpad.Malloc(10);
		///        void* p2 = scratchpad.MallocAligned(10, 64);
		///
		///        scratchpad.Free(p1); // Calling Free is optional, though it allows for the potential of more efficient usage of scatch memory.
		///        scratchpad.Free(p2);
		///    }
		///
		///    void DoSomething2() {
		///        ScratchpadAllocator scratchpad;
		///
		///        void* p1 = scratchpad.Malloc(10);
		///        DoSomething1();
		///
		///        scratchpad.Free(p1);
		///    }
		///
		class PPM_API ScratchpadAllocator
		{
		public:
			ScratchpadAllocator(ScratchpadManager* pManager = NULL);
		   ~ScratchpadAllocator();

			void* Malloc(size_t nSize);
			void* MallocAligned(size_t nSize, size_t nAlignment, size_t nAlignmentOffset = 0);

			// Usage of Free is optional, though it allows for the potential of more efficient usage of 
			// scatch memory. There is no ordering requirement for the Free calls through a given ScratchpadAllocator.
			void  Free(void* pData); // See notes above about usage of Free.

			StackAllocator* GetStackAllocator() const;

		protected:
			StackAllocatorInfo* mpStackAllocatorInfo;   /// The StackAllocator we use, plus some related data.
			void*               mpAllocationBase;       /// Where the StackAllocator points when we first start using it.
			void*               mpLastAllocation;       /// The last memory allocation we did, which we use to reclaim some StackAllocator memory in some cases.
			ScratchpadManager*  mpScratchpadManager;    /// The manager we are using.
			int                 mCount;                 /// The count of this ScratchpadAllocator in the nested set of ScratchpadAllocators for this thread.
		};



		/// ScratchpadManager
		///
		/// Users of ScratchpadAllocator typically don't need to use this class, as the default global
		/// instance of this class will be used automatically by default. However, you can create a 
		/// custom instance of this class and use it with ScratchpadAllocator instances.
		///
		/// Example usage:
		///    EA::Allocator::ScratchpadManager gScratchpadManager;
		///
		///    int main(int, char**) {
		///        EA::Allocator::SetScratchpadManager(&gScratchpadManager);
		///        gScratchpadManager.SetAllocatorCallbacks(ScratchpadAllocationFunction, ScratchpadFreeFunction, NULL);
		///        // Possibly call other manager setup functions.
		///
		///        ScratchpadAllocator a;
		///        a.Free(a.Malloc(16));
		///    }
		///
		class PPM_API ScratchpadManager
		{
		public:
			ScratchpadManager(StackAllocator::CoreAllocationFunction pCoreAllocationFunction = NULL, StackAllocator::CoreFreeFunction pCoreFreeFunction = NULL, 
								void* pCoreFunctionContext = NULL, size_t initialStackAllocatorCacheCount = 0);
		   ~ScratchpadManager();

			StackAllocatorInfo* GetStackAllocatorInfoForCurrentThread();

			// This must be called before any use of this class directly or via ScratchpadAllocator.
			void SetAllocatorCallbacks(StackAllocator::CoreAllocationFunction pCoreAllocationFunction, StackAllocator::CoreFreeFunction pCoreFreeFunction, void* pCoreFunctionContext);

			// This can be called at any time.
			void SetDefaultCoreBlockSize(size_t defaultCoreBlockSize);

			// This can be called at any time. If called with a value smaller than the 
			// currently cached count, no caches are purged. You can call the SetStackAllocatorCacheCount
			// function to force a specific cache entry count. This max count is limited by 
			// the value of kStackAllocatorCacheCapacity.
			void SetStackAllocatorMaxCacheCount(size_t stackAllocatorMaxCacheCount);

			// Returns the current stack allocator cache count.
			size_t GetStackAllocatorCacheCount() const
				{ return mStackAllocatorCacheCount; }

			// This can be called at any time. The existing stack allocator count can increase or 
			// decrease in response to this function.
			void SetStackAllocatorCacheCount(size_t stackAllocatorCacheCount);

		protected:
			friend class ScratchpadAllocator;

			StackAllocatorInfo* CreateStackAllocatorInfoForCurrentThread();
			void                DestroyStackAllocatorInfoForCurrentThread(StackAllocatorInfo* pStackAllocatorInfo);

			StackAllocatorInfo* GetThreadLocal();
			void SetThreadLocal(StackAllocatorInfo* pStackAllocatorInfo);
			void ClearThreadLocal(StackAllocatorInfo* pStackAllocatorInfo);

			ScratchpadManager(const ScratchpadManager&){}
			void operator=(const ScratchpadManager&){}

		protected:
			enum {
				kStackAllocatorCacheCapacity        = 8,
				kStackAllocatorCacheDefault         = 4,
				kStackAllocatorDefaultCoreBlockSize = 4096
			};

			EA::Thread::Futex                      mFutex;                                              //
			EA::Thread::ThreadLocalStorage         mThreadLocalStorage;                                 // Used to store thread-specific instances of StackAllocator. We use thread-local data instead of ThreadLocalStorage when possible.
			size_t                                 mDefaultCoreBlockSize;                               // 
			StackAllocatorInfo*                    mStackAllocatorCache[kStackAllocatorCacheCapacity];  // 
			size_t                                 mStackAllocatorCacheCount;                           // Number of StackAllocators currently in mStackAllocatorCache.
			size_t                                 mStackAllocatorMaxCacheCount;                        // Max number of StackAllocators to cache.
			StackAllocator::CoreAllocationFunction mpCoreAllocationFunction;                            // Callback for allocating a new Block.
			StackAllocator::CoreFreeFunction       mpCoreFreeFunction;                                  // Callback for freeing an existing Block.
			void*                                  mpCoreFunctionContext;                               // Context passed to CoreAllocationFunction, CoreFreeFunction.
		};



		/// SetScratchpadManager / GetScratchpadManager
		///
		/// Sets the default application global ScratchpadManager. You would typically call 
		/// SetScratchpadManager once on startup.
		/// 
		PPM_API void               SetScratchpadManager(ScratchpadManager* pScratchpadManager);
		PPM_API ScratchpadManager* GetScratchpadManager();


	} // namespace Allocator

} // namespace EA


#endif // Header include guard



