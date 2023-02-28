///////////////////////////////////////////////////////////////////////////////
// ScratchpadAllocator
//
// Copyright (c) 2014, Electronic Arts. All Rights Reserved.
///////////////////////////////////////////////////////////////////////////////


#include <PPMalloc/EAScratchpadAllocator.h>
#include <eathread/eathread_storage.h>
#include <PPMalloc/internal/shared.h>
#include <new>


namespace EA
{
	namespace Allocator
	{
		///////////////////////////////////////////////////////////////////////////////
		// StackAllocatorInfo
		///////////////////////////////////////////////////////////////////////////////

		struct StackAllocatorInfo
		{
			StackAllocatorInfo() : mStackAllocator(), mUserCount(0){}

			StackAllocator mStackAllocator; // The StackAllocator instance for every ScratchpadAllocator that's using it. 
			int            mUserCount;      // Incremented once for every ScratchpadAllocator that's using it. 
		};



		///////////////////////////////////////////////////////////////////////////////
		// ScratchpadAllocator
		///////////////////////////////////////////////////////////////////////////////

		ScratchpadAllocator::ScratchpadAllocator(ScratchpadManager* pManager) // Data members are initialized below.
		{
			if(pManager)
				mpScratchpadManager = pManager;
			else
				mpScratchpadManager = GetScratchpadManager();
			PPM_ASSERT_STATIC(mpScratchpadManager != NULL, kGAErrorUninitializedHeap, "The application needs to call SetScratchpadManager() on startup.", NULL, NULL);

			mpStackAllocatorInfo = mpScratchpadManager->GetStackAllocatorInfoForCurrentThread();
			mCount               = ++mpStackAllocatorInfo->mUserCount; // This doesn't need to be an atomic because mpStackAllocatorInfo is thread-specific.
			mpLastAllocation     = NULL;
			mpAllocationBase     = mpStackAllocatorInfo->mStackAllocator.GetCurrentObjectBegin(); // We expect that the ScratchpadManager would have created this.
			PPM_ASSERT_STATIC(mpAllocationBase != NULL, kGAErrorInternal, kGAErrorText[kGAErrorInternal], NULL, NULL);
		}


		ScratchpadAllocator::~ScratchpadAllocator()
		{
			mpStackAllocatorInfo->mStackAllocator.FreeObjects(mpAllocationBase);

			if(--mpStackAllocatorInfo->mUserCount == 0) // If we are the last (earliest) user of this StackAllocator... // This doesn't need to be an atomic because mpStackAllocatorInfo is thread-specific.
				mpScratchpadManager->DestroyStackAllocatorInfoForCurrentThread(mpStackAllocatorInfo);
		}


		void* ScratchpadAllocator::Malloc(size_t nSize)
		{
			// If this fails then another ScratchpadAllocator has since become current and owns the top of the StackAllocator's stack. It is the user's error.
			PPM_ASSERT_STATIC(mCount == mpStackAllocatorInfo->mUserCount, kGAErrorInvalidInput, kGAErrorText[kGAErrorInvalidInput], NULL, NULL);

			mpLastAllocation = mpStackAllocatorInfo->mStackAllocator.Malloc(nSize);
			return mpLastAllocation;
		}


		void* ScratchpadAllocator::MallocAligned(size_t nSize, size_t nAlignment, size_t nAlignmentOffset)
		{
			// If this fails then another ScratchpadAllocator has since become current and owns the top of the StackAllocator's stack. It is the user's error.
			PPM_ASSERT_STATIC(mCount == mpStackAllocatorInfo->mUserCount, kGAErrorInvalidInput, kGAErrorText[kGAErrorInvalidInput], NULL, NULL);

			mpLastAllocation = mpStackAllocatorInfo->mStackAllocator.MallocAligned(nSize, nAlignment, nAlignmentOffset);
			return mpLastAllocation;
		}


		void ScratchpadAllocator::Free(void* pData)
		{
			// If this fails then another ScratchpadAllocator has since become current and owns the top of the StackAllocator's stack. It is the user's error.
			PPM_ASSERT_STATIC(mCount == mpStackAllocatorInfo->mUserCount, kGAErrorInvalidInput, kGAErrorText[kGAErrorInvalidInput], NULL, NULL);

			// We currently have only simple logic here: we call FreeObjects only if pData refers to 
			// the last thing returned by Malloc. We don't (currently) keep a list/stack of all allocations 
			// returned by Malloc and so we can't do this for multiple consecutive Frees. However, even 
			// this simple logic allows you to call Malloc, Free, Malloc, Free and avoid growing the 
			// scratchpad memory requirement.

			if((pData == mpLastAllocation) && (mCount == mpStackAllocatorInfo->mUserCount))
				mpStackAllocatorInfo->mStackAllocator.FreeObjects(mpLastAllocation); // No need to set mpLastAllocation to NULL after this.
		}


		StackAllocator* ScratchpadAllocator::GetStackAllocator() const
		{
			return &mpStackAllocatorInfo->mStackAllocator;
		}


		///////////////////////////////////////////////////////////////////////////////
		// ScratchpadManager
		///////////////////////////////////////////////////////////////////////////////

		#if defined(EA_THREAD_LOCAL)    // Disabled until we can make sure we are handling Microsoft DLL issues.
			#undef EA_THREAD_LOCAL      // http://blogs.msdn.com/b/oldnewthing/archive/2010/11/22/10094489.aspx
		#endif

		#if defined(EA_THREAD_LOCAL) // If native thread-local storage is supported on the platform...
			// gThreadLocalStackAllocatorInfo appears to be a single variable, but there is actually one of 
			// these per thread. This variable does cross DLL boundaries (which is what we want), and so 
			// multiple DLLs executing in the same thread will see the same value for this.
			EA_THREAD_LOCAL StackAllocatorInfo* gThreadLocalStackAllocatorInfo = NULL; 
		#endif


		// This gpScratchpadManager global is currently DLL-specific. So if you are using this system with 
		// code from multiple DLLs/shared libraries then there will be an inefficiency because each DLL will have 
		// its own copy of gScratchpadManager. There are two primary solutions to this problem:
		//     1 Explicitly call SetScratchpadmanager once each each DLL's init.
		//     2 Make ScratchpadManager be an OSGlobal (e.g. EAStdC OSGlobal). 
		// Solution #2 is the simpler and more reliable, but creates a dependency on the OSGlobal code.
		// We don't have guarded thread access to gpScratchpadManager, as it's expected that the 
		// application will handle this.

		ScratchpadManager* gpScratchpadManager = NULL;

		PPM_API void SetScratchpadManager(ScratchpadManager* pScratchpadManager)
		{
			gpScratchpadManager = pScratchpadManager;
		}

		PPM_API ScratchpadManager* GetScratchpadManager()
		{
			return gpScratchpadManager;
		}



		ScratchpadManager::ScratchpadManager(StackAllocator::CoreAllocationFunction pCoreAllocationFunction, StackAllocator::CoreFreeFunction pCoreFreeFunction, 
											 void* pCoreFunctionContext, size_t initialStackAllocatorCacheCount)
		  : mFutex()
		  , mThreadLocalStorage() // To consider: Make this a lazy-allocated pointer instead of concrete.
		  , mDefaultCoreBlockSize(kStackAllocatorDefaultCoreBlockSize)
		  //mStackAllocatorCache[]
		  , mStackAllocatorCacheCount(0)
		  , mStackAllocatorMaxCacheCount(kStackAllocatorCacheDefault)
		  , mpCoreAllocationFunction(pCoreAllocationFunction)
		  , mpCoreFreeFunction(pCoreFreeFunction)
		  , mpCoreFunctionContext(pCoreFunctionContext)
		{
			SetStackAllocatorCacheCount(initialStackAllocatorCacheCount);
		}



		ScratchpadManager::~ScratchpadManager()
		{
			// What do we do about any threads that may exist which have live ScratchpadAllocators?
			// This really shouldn't happen as ScratchpadManager should be destroyed only after all usage.

			SetStackAllocatorCacheCount(0);

			if(gpScratchpadManager == this)
				gpScratchpadManager = NULL;
		}


		void ScratchpadManager::SetAllocatorCallbacks(StackAllocator::CoreAllocationFunction pCoreAllocationFunction, StackAllocator::CoreFreeFunction pCoreFreeFunction, void* pCoreFunctionContext)
		{
			EA::Thread::AutoFutex autoFutex(mFutex);

			mpCoreAllocationFunction = pCoreAllocationFunction;
			mpCoreFreeFunction       = pCoreFreeFunction;
			mpCoreFunctionContext    = pCoreFunctionContext;
		}


		void ScratchpadManager::SetDefaultCoreBlockSize(size_t defaultCoreBlockSize)
		{
			EA::Thread::AutoFutex autoFutex(mFutex);

			mDefaultCoreBlockSize = defaultCoreBlockSize;
		}


		void ScratchpadManager::SetStackAllocatorMaxCacheCount(size_t stackAllocatorMaxCacheCount)
		{
			EA::Thread::AutoFutex autoFutex(mFutex);

			if(stackAllocatorMaxCacheCount > kStackAllocatorCacheCapacity)
				stackAllocatorMaxCacheCount = kStackAllocatorCacheCapacity;

			mStackAllocatorMaxCacheCount = stackAllocatorMaxCacheCount;
		}


		StackAllocatorInfo* ScratchpadManager::GetThreadLocal()
		{
			#if defined(EA_THREAD_LOCAL) // If native thread-local storage is supported on the platform...
				if(gpScratchpadManager == this) // If we are the primary ScratchpadManager...
					return gThreadLocalStackAllocatorInfo;
				else
					return static_cast<StackAllocatorInfo*>(mThreadLocalStorage.GetValue());
			#else
				return static_cast<StackAllocatorInfo*>(mThreadLocalStorage.GetValue());
			#endif
		}


		void ScratchpadManager::SetThreadLocal(StackAllocatorInfo* pStackAllocatorInfo)
		{
			// If the platform supports true thread-local storage, and if we are the primary ScratchpadManager,
			// we store pStackAllocator in a thread-local variable, as that allows for the fastest access of it.
			// This results in unilateral usage of 4 bytes of thread-local storage.
			#if defined(EA_THREAD_LOCAL) // If native thread-local storage is supported on the platform...
				if(gpScratchpadManager == this) // If we are the primary ScratchpadManager...
					gThreadLocalStackAllocatorInfo = pStackAllocatorInfo;
				else
					mThreadLocalStorage.SetValue(pStackAllocatorInfo);
			#else
				mThreadLocalStorage.SetValue(pStackAllocatorInfo);
			#endif
		}


		void ScratchpadManager::ClearThreadLocal(StackAllocatorInfo* pCurrentValue)
		{
			#if defined(EA_THREAD_LOCAL) // If native thread-local storage is supported on the platform...
				if(gThreadLocalStackAllocatorInfo == pCurrentValue) // If pStackAllocatorInfo is the thread-local version, clear it before we destroy it. 
					gThreadLocalStackAllocatorInfo = NULL;
				else
					mThreadLocalStorage.SetValue(NULL);
			#else
				EA_UNUSED(pCurrentValue);
				mThreadLocalStorage.SetValue(NULL);
			#endif
		}


		void ScratchpadManager::SetStackAllocatorCacheCount(size_t stackAllocatorCacheCount)
		{
			EA::Thread::AutoFutex autoFutex(mFutex);

			if(stackAllocatorCacheCount > mStackAllocatorMaxCacheCount)
				stackAllocatorCacheCount = mStackAllocatorMaxCacheCount;

			PPM_ASSERT_STATIC(mpCoreFreeFunction || (mStackAllocatorCacheCount <= stackAllocatorCacheCount), kGAErrorInvalidInput, kGAErrorText[kGAErrorInvalidInput], NULL, NULL);
			if(mpCoreFreeFunction)
			{
				while(mStackAllocatorCacheCount > stackAllocatorCacheCount)
				{
					StackAllocatorInfo* pStackAllocatorInfo = mStackAllocatorCache[--mStackAllocatorCacheCount];

					ClearThreadLocal(pStackAllocatorInfo);
					pStackAllocatorInfo->~StackAllocatorInfo();
					mpCoreFreeFunction(pStackAllocatorInfo, mpCoreFunctionContext);
				}
			}

			PPM_ASSERT_STATIC(mpCoreAllocationFunction || (mStackAllocatorCacheCount >= stackAllocatorCacheCount), kGAErrorInvalidInput, kGAErrorText[kGAErrorInvalidInput], NULL, NULL);
			if(mpCoreAllocationFunction)
			{
				while(mStackAllocatorCacheCount < stackAllocatorCacheCount)
				{
					StackAllocatorInfo* pStackAllocatorInfo = new(mpCoreAllocationFunction(sizeof(StackAllocatorInfo), NULL, mpCoreFunctionContext)) StackAllocatorInfo;

					pStackAllocatorInfo->mStackAllocator.Init(NULL, mDefaultCoreBlockSize, mpCoreAllocationFunction, mpCoreFreeFunction, mpCoreFunctionContext);
					mStackAllocatorCache[mStackAllocatorCacheCount++] = pStackAllocatorInfo;
				}
			}
		}


		StackAllocatorInfo* ScratchpadManager::GetStackAllocatorInfoForCurrentThread()
		{
			// This function shouldn't need its own thread protection.
			StackAllocatorInfo* pStackAllocatorInfo = GetThreadLocal();

			if(pStackAllocatorInfo)
			{
				PPM_ASSERT_STATIC((pStackAllocatorInfo->mUserCount > 0) && (pStackAllocatorInfo->mUserCount < 50000), kGAErrorInternal, kGAErrorText[kGAErrorInternal], NULL, NULL); // Sanity check.
				return pStackAllocatorInfo;
			}

			return CreateStackAllocatorInfoForCurrentThread();
		}
			

		StackAllocatorInfo* ScratchpadManager::CreateStackAllocatorInfoForCurrentThread()
		{
			EA::Thread::AutoFutex autoFutex(mFutex);

			StackAllocatorInfo* pStackAllocatorInfo;

			// Check the cache first.
			if(mStackAllocatorCacheCount)
			{
				pStackAllocatorInfo = mStackAllocatorCache[--mStackAllocatorCacheCount];
				PPM_ASSERT_STATIC(pStackAllocatorInfo->mUserCount == 0, kGAErrorInternal, kGAErrorText[kGAErrorInternal], NULL, NULL); // Sanity check.
			}
			else
			{
				pStackAllocatorInfo = new(mpCoreAllocationFunction(sizeof(StackAllocatorInfo), NULL, mpCoreFunctionContext)) StackAllocatorInfo;
				pStackAllocatorInfo->mStackAllocator.Init(NULL, mDefaultCoreBlockSize, mpCoreAllocationFunction, mpCoreFreeFunction, mpCoreFunctionContext);
			}

			SetThreadLocal(pStackAllocatorInfo);

			return pStackAllocatorInfo;
		}


		void ScratchpadManager::DestroyStackAllocatorInfoForCurrentThread(StackAllocatorInfo* pStackAllocatorInfo)
		{
			EA::Thread::AutoFutex autoFutex(mFutex);

			ClearThreadLocal(pStackAllocatorInfo);

			if(mStackAllocatorCacheCount < mStackAllocatorMaxCacheCount)
			{
				// To consider: Have the pStackAllocator purge all but its last core block.
				mStackAllocatorCache[mStackAllocatorCacheCount++] = pStackAllocatorInfo;

				// To consider: We could also save our thread id and put it on a list of threads that may be exiting in the future. If we detect the thread has 
				// exited in the future then perhaps we could know to free the StackAllocator that went with it, as opposed to keeping that StackAllocator around in a cache.
			}
			else
			{
				pStackAllocatorInfo->~StackAllocatorInfo();
				mpCoreFreeFunction(pStackAllocatorInfo, mpCoreFunctionContext);
			}
		}

	} // namespace Allocator

} // namespace EA








