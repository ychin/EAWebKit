///////////////////////////////////////////////////////////////////////////////
// EAHandleAllocator
// 
// Copyright (c) 2004, Electronic Arts. All Rights Reserved.
//
//  *** This module is deprecated and not maintained. ***
///////////////////////////////////////////////////////////////////////////////

///////////////////////////////////////////////////////////////////////////////
//
// Lint settings
// The following error suppressions largely represent warnings that can't 
// be easily fixed in code but are also known to be insignificant for this module.
//lint -esym(818,h)    // parameter could be declared as pointing to const.
//lint -e571           // Suspicious cast.
//lint -e428           // Negative subscript (-1) in operator 'ptr-int'.
///////////////////////////////////////////////////////////////////////////////


#include <PPMalloc/EAHandleAllocator.h>



namespace EA{

namespace Allocator{


HandleAllocator::HandleAllocator(GeneralAllocator* pGeneralAllocator, size_t nInitialHandleCount)
  : mnHandleCount(0),
	mHandleInfoAllocator(),
	mpGeneralAllocator(NULL),
	mpMutex(NULL)
{
	HandleAllocator::Init(pGeneralAllocator, nInitialHandleCount); // Init is normally a virtual function, but virtual functions don't work from ctor/dtor.
}



HandleAllocator::~HandleAllocator()
{
	HandleAllocator::Shutdown(); // Shutdown is normally a virtual function, but virtual functions don't work from ctor/dtor.
}



bool HandleAllocator::Init(GeneralAllocator* pGeneralAllocator, size_t nInitialHandleCount)
{
	if(!mpGeneralAllocator) //If we haven't already been initialized...
	{
		if(pGeneralAllocator)
			mpGeneralAllocator = pGeneralAllocator;

		// Thread safety
		#if PPM_THREAD_SAFETY_SUPPORTED && PPM_THREAD_SAFETY_BY_DEFAULT
			if(!mpMutex)
				mpMutex = PPMMutexCreate(mpMutexData);
		#endif

		mHandleInfoAllocator.Init(nInitialHandleCount, NULL, 0, CoreAllocationFunction, CoreFreeFunction, this);
	}

	return true;
}



bool HandleAllocator::Shutdown()
{
	if(mpGeneralAllocator) // If we've been initialized...
	{
		#if PPM_THREAD_SAFETY_SUPPORTED
			if(mpMutex)
				PPMMutexLock(mpMutex);
		#endif

		if(mnHandleCount)
		{
			// Possibly we should consider freeing memory associated with 
			// any existing valid handles. However, doing so would hide the
			// fact that the user has leaky code. So unless we can provide 
			// a sufficiently useful leak reporting mechanism here, it is 
			// best if we let the memory leak and have the underlying 
			// GeneralAllocator report the leaks upon its own shutdown.

			mHandleInfoAllocator.Shutdown();
			mnHandleCount = 0;
		}

		#if PPM_THREAD_SAFETY_SUPPORTED
			if(mpMutex)
			{
				void* const pMutex = mpMutex;
				mpMutex = NULL;
				PPMMutexUnlock(pMutex);
				PPMMutexDestroy(pMutex);
			}
		#endif
	}

	return true;
}



void* HandleAllocator::CoreAllocationFunction(size_t nSize, void* pContext)
{
	// Mutex lock not necessary, as we assume mpGeneralAllocator is invariant over our life. 
	HandleAllocator* const pHandleAllocator = (HandleAllocator*)pContext;
	return pHandleAllocator->mpGeneralAllocator->Malloc(nSize);
}



void HandleAllocator::CoreFreeFunction(void* pCore, void* pContext)
{
	// Mutex lock not necessary, as we assume mpGeneralAllocator is invariant over our life. 
	HandleAllocator* const pHandleAllocator = (HandleAllocator*)pContext;
	return pHandleAllocator->mpGeneralAllocator->Free(pCore);
}



HandleAllocator::Handle HandleAllocator::Malloc(size_t nSize)
{
	if(mpGeneralAllocator)
	{
		// By design and for deadlock protection, we don't lock our mutex around the general allocator call.
		void* const pData = mpGeneralAllocator->Malloc(nSize);

		if(pData)
		{
			#if PPM_THREAD_SAFETY_SUPPORTED
				if(mpMutex)
					PPMMutexLock(mpMutex);
			#endif

			HandleInfo* const pHandleInfo = (HandleInfo*)mHandleInfoAllocator.Malloc();

			if(pHandleInfo)
			{
				pHandleInfo->mpData                      = pData;
				pHandleInfo->mBitfield.mnLockCount       = 0;
				pHandleInfo->mBitfield.mnAlignment       = 0;
				pHandleInfo->mBitfield.mnAlignmentOffset = 0;
				pHandleInfo->mBitfield.mbHigh            = 0;
			}
			else
				mpGeneralAllocator->Free(pData);

			#if PPM_THREAD_SAFETY_SUPPORTED
				if(mpMutex)
					PPMMutexUnlock(mpMutex);
			#endif

			return pHandleInfo;
		}
	}

	return NULL;
}



HandleAllocator::Handle HandleAllocator::MallocAligned(size_t nSize, size_t nAlignment, size_t nAlignmentOffset)
{
	if(mpGeneralAllocator)
	{
		// By design and for deadlock protection, we don't lock our mutex around the general allocator call.
		void* const pData = mpGeneralAllocator->MallocAligned(nSize, nAlignment, nAlignmentOffset);

		if(pData)
		{
			#if PPM_THREAD_SAFETY_SUPPORTED
				if(mpMutex)
					PPMMutexLock(mpMutex);
			#endif

			HandleInfo* const pHandleInfo = (HandleInfo*)mHandleInfoAllocator.Malloc();

			if(pHandleInfo)
			{
				pHandleInfo->mpData                      = pData;
				pHandleInfo->mBitfield.mnLockCount       = 0;
				pHandleInfo->mBitfield.mnAlignment       = (int)nAlignment;
				pHandleInfo->mBitfield.mnAlignmentOffset = (int)nAlignmentOffset;
				pHandleInfo->mBitfield.mbHigh            = 0;
			}
			else
				mpGeneralAllocator->Free(pData);

			#if PPM_THREAD_SAFETY_SUPPORTED
				if(mpMutex)
					PPMMutexUnlock(mpMutex);
			#endif

			return pHandleInfo;
		}
	}

	return NULL;
}



HandleAllocator::Handle HandleAllocator::Calloc(size_t nElementCount, size_t nElementSize)
{
	if(mpGeneralAllocator)
	{
		// By design and for deadlock protection, we don't lock our mutex around the general allocator call.
		void* const pData = mpGeneralAllocator->Calloc(nElementCount, nElementSize);

		if(pData)
		{
			#if PPM_THREAD_SAFETY_SUPPORTED
				if(mpMutex)
					PPMMutexLock(mpMutex);
			#endif

			HandleInfo* const pHandleInfo = (HandleInfo*)mHandleInfoAllocator.Malloc();

			if(pHandleInfo)
			{
				pHandleInfo->mpData                      = pData;
				pHandleInfo->mBitfield.mnLockCount       = 0;
				pHandleInfo->mBitfield.mnAlignment       = 0;
				pHandleInfo->mBitfield.mnAlignmentOffset = 0;
				pHandleInfo->mBitfield.mbHigh            = 0;
			}
			else
				mpGeneralAllocator->Free(pData);

			#if PPM_THREAD_SAFETY_SUPPORTED
				if(mpMutex)
					PPMMutexUnlock(mpMutex);
			#endif

			return pHandleInfo;
		}
	}

	return NULL;
}



bool HandleAllocator::Realloc(Handle h, size_t nNewSize)
{
	// We don't lock the mutex because we don't modify member data structures.
	// The altering of the handle mpData below is an exception, as it is assumed
	// that the given handle belongs to the calling thread.

	if(mpGeneralAllocator)
	{
		HandleInfo* const pHandleInfo = reinterpret_cast<HandleInfo*>(h);

		PPM_ASSERT(mpGeneralAllocator, pHandleInfo->mBitfield.mnLockCount == 0, kGAErrorInternal, "Internal error.", NULL, NULL);
		PPM_ASSERT(mpGeneralAllocator, pHandleInfo->mpData, kGAErrorInternal, "Internal error.", NULL, NULL);
		PPM_ASSERT(mpGeneralAllocator, mpGeneralAllocator, kGAErrorInternal, "Internal error.", NULL, NULL);

		// By design and for deadlock protection, we don't lock our mutex around the general allocator call.
		void* const pData = mpGeneralAllocator->Realloc(pHandleInfo->mpData, nNewSize);
		if(pData)
		{
			pHandleInfo->mpData = pData;
			return true;
		}
	}

	return false;
}



void HandleAllocator::Free(Handle h)
{
	if(mpGeneralAllocator)
	{
		HandleInfo* const pHandleInfo = reinterpret_cast<HandleInfo*>(h);

		PPM_ASSERT(mpGeneralAllocator, pHandleInfo->mBitfield.mnLockCount == 0, kGAErrorInternal, "Internal error.", NULL, NULL);
		PPM_ASSERT(mpGeneralAllocator, pHandleInfo->mpData, kGAErrorInternal, "Internal error.", NULL, NULL);
		PPM_ASSERT(mpGeneralAllocator, mpGeneralAllocator, kGAErrorInternal, "Internal error.", NULL, NULL);

		// By design and for deadlock protection, we don't lock our mutex around the general allocator call.
		mpGeneralAllocator->Free(pHandleInfo->mpData);

		#if PPM_THREAD_SAFETY_SUPPORTED
			if(mpMutex)
				PPMMutexLock(mpMutex);
		#endif

		mHandleInfoAllocator.Free(h);

		#if PPM_THREAD_SAFETY_SUPPORTED
			if(mpMutex)
				PPMMutexUnlock(mpMutex);
		#endif
	}
}



void* HandleAllocator::Lock(Handle h)
{
	// We don't lock the mutex because we don't modify member data structures.
	// The altering of the handle data below is an exception, as it is assumed
	// that the given handle belongs to the calling thread.
	HandleInfo* const pHandleInfo = reinterpret_cast<HandleInfo*>(h);

	PPM_ASSERT(mpGeneralAllocator, (unsigned)pHandleInfo->mBitfield.mnLockCount < (unsigned)kMaxLockCount, kGAErrorInternal, "Internal error.", NULL, NULL);

	if(pHandleInfo->mBitfield.mnLockCount == 0)
		CompactHandle(h, kCompactionStrategyDefault);

	pHandleInfo->mBitfield.mnLockCount++;
	return pHandleInfo->mpData;
}



void HandleAllocator::Unlock(Handle h)
{
	// We don't lock the mutex because we don't modify member data structures.
	// The altering of the handle data below is an exception, as it is assumed
	// that the given handle belongs to the calling thread.
	HandleInfo* const pHandleInfo = reinterpret_cast<HandleInfo*>(h);

	PPM_ASSERT(mpGeneralAllocator, pHandleInfo->mBitfield.mnLockCount > 0, kGAErrorInternal, "Internal error.", NULL, NULL);

	if(pHandleInfo->mBitfield.mnLockCount)
	{
		if(--pHandleInfo->mBitfield.mnLockCount == 0)
			CompactHandle(h, kCompactionStrategyDefault);
	}
}



bool HandleAllocator::CompactHandle(Handle h, CompactionStrategy /*strategy*/)
{
	// We don't lock the mutex because we don't modify member data structures.
	// The altering of the handle data below is an exception, as it is assumed
	// that the given handle belongs to the calling thread.
	HandleInfo* const pHandleInfo = reinterpret_cast<HandleInfo*>(h);

	if(pHandleInfo->mBitfield.mnLockCount == 0) // If we can compact the handle...
	{
		// The primary strategy is to have chunks pack by migrating upward or downward over time.
		// 
		// Go through the free bins and see if any of them has space for the 
		// block lower or higher as desired. 
		// If that fails, then:
		//     If strategy is to move low:
		//         If the chunk below this block is free, move this data downward 
		//         to the beginning of the free chunk.
		//     If the strategy is to move high:
		//         If the chunk above this block is free, move this data upward to 
		//         the end of the free chunk.

		#if 0 // This is not yet complete.

			bool bChunkWasMoved(false);

			#if PPM_THREAD_SAFETY_SUPPORTED
				mpGeneralAllocator->Lock(true);
			#endif

			GeneralAllocator::Chunk* const     pChunk     = GeneralAllocator::GetChunkPtrFromDataPtr(pHandleInfo->mpData);
			const GeneralAllocator::size_type nChunkSize = GeneralAllocator::GetChunkSize(pChunk);

			// Try to relocate the chunk by finding a perfectly fitting space lower or higher in memory.
			if(pHandleInfo->mBitfield.mbHigh)
			{
				// Go through allocation bins and see if this block can be moved upward.
			}
			else
			{
				// Go through allocation bins and see if this block can be moved downward.
			}

			if(!bChunkWasMoved) // If the above didn't succeed...
			{
				GeneralAllocator::Chunk* const    pChunkTemp     = GetChunkAtOffset(pChunk, nChunkSize);
				GeneralAllocator::Chunk* const    pNextChunk     = GetChunkIsInUse(pChunkTemp) ? pChunkTemp : NULL;
				const GeneralAllocator::size_type nNextChunkSize = GeneralAllocator::GetChunkSize(pNextChunk);

				GeneralAllocator::Chunk* const    pPrevChunk     = GeneralAllocator::GetPrevChunkIsInUse(pChunk) ? GetChunkAtOffset(pChunk, -(long)pChunk->mnPriorSize) : NULL;
				const GeneralAllocator::size_type nPrevChunkSize = pPrevChunk ? pChunk->mnPriorSize : 0;

				// Decide whether we can move our chunk upward or downward.
				if(pHandleInfo->mBitfield.mbHigh)
				{
					if(pNextChunk)
					{
						// GeneralAllocator::UnlinkChunk(pNextChunk);
						// Move our chunk data up to the end of the next chunk.
						// Free our previous space.
					}
				}
				else
				{
					if(pPrevChunk)
					{
						// GeneralAllocator::UnlinkChunk(pPrevChunk);
						// Move our chunk data up to the end of the next chunk.
						// Free our previous space.
					}
				}
			}

			#if PPM_THREAD_SAFETY_SUPPORTED
				mpGeneralAllocator->Lock(false);
			#endif

		#endif // #if 0

		return true;
	}
	return false;
}



void HandleAllocator::Compact(CompactionLevel /*level*/, CompactionStrategy /*strategy*/)
{
	// Possible strategies:
	//    - For each handle, compact it.
	//    - Measure packing and if poor, then do massive relocation.
	// We stick with the first strategy for now.

	void* pIterator = mHandleInfoAllocator.BeginGetNext();
	for(HandleInfo* pHI = mHandleInfoAllocator.GetNext(pIterator); pHI; pHI = mHandleInfoAllocator.GetNext(pIterator))
		CompactHandle(pHI);
	mHandleInfoAllocator.EndGetNext(pIterator);
}



bool HandleAllocator::ValidateHandle(Handle h)
{
	#if PPM_THREAD_SAFETY_SUPPORTED
		if(mpMutex)
			PPMMutexLock(mpMutex);
	#endif

	const bool bReturnValue = mHandleInfoAllocator.ValidateAddress(h);

	#if PPM_THREAD_SAFETY_SUPPORTED
		if(mpMutex)
			PPMMutexUnlock(mpMutex);
	#endif

	return bReturnValue;
}



HandleAllocator::Handle HandleAllocator::GetHandleFromAddress(const void* pData)
{
	#if PPM_THREAD_SAFETY_SUPPORTED
		if(mpMutex)
			PPMMutexLock(mpMutex);
	#endif

	HandleInfo* const pHandleInfo = mHandleInfoAllocator.FindHandleInfo(pData);

	#if PPM_THREAD_SAFETY_SUPPORTED
		if(mpMutex)
			PPMMutexUnlock(mpMutex);
	#endif

	return pHandleInfo;
}



unsigned HandleAllocator::GetLockCount(Handle h)
{
	// We don't lock the mutex because we don't modify member data structures.
	// The altering of the handle data below is an exception, as it is assumed
	// that the given handle belongs to the calling thread.
	HandleInfo* const pHandleInfo = reinterpret_cast<HandleInfo*>(h);
	return (unsigned)pHandleInfo->mBitfield.mnLockCount;
}



///////////////////////////////////////////////////////////////////////////////
// SetOption
//
void HandleAllocator::SetOption(Option option, int nValue)
{
	switch (option){
		case kOptionEnableThreadSafety:
		{
			#if PPM_THREAD_SAFETY_SUPPORTED
				if(nValue)
				{
					if(!mpMutex)
					{
						mpMutex = PPMMutexCreate(mpMutexData);
						PPM_ASSERT(mpGeneralAllocator, mpMutex != NULL, kGAErrorInternal, "Internal error.", NULL, NULL);
					}
				}
				else
				{
					if(mpMutex)
					{
						PPMMutexLock(mpMutex);
						void* const pMutex = mpMutex;
						mpMutex = NULL;
						PPMMutexUnlock(pMutex);
						PPMMutexDestroy(pMutex);
					}
				}
			#else
				EA_UNUSED(nValue);
			#endif

			break;
		}

		case kOptionNone:   // VC gives a warning if we don't do this.
		default:            // GCC gives a warning if we don't do this.
			break; 
	}
}



HandleAllocator::HandleInfo* HandleAllocator::FixedAllocatorEx::FindHandleInfo(const void* pData)
{
	HandleInfo* pHI       = NULL;
	void*       pIterator = BeginGetNext();

	for(HandleInfo* pHITemp = GetNext(pIterator); pHITemp; pHITemp = GetNext(pIterator))
	{
		if(pHITemp->mpData == pData)
		{
			pHI = pHITemp;
			break;
		}
	}

	EndGetNext(pIterator);
	return pHI;
}



bool HandleAllocator::FixedAllocatorEx::ValidateAddress(const HandleAllocator::Handle h)
{
	for(CoreBlock* pCoreBlock = mpHeadCoreBlock; pCoreBlock; pCoreBlock = pCoreBlock->mpNext)
	{
		if((h >= pCoreBlock->mData) && (h < ((char*)pCoreBlock->mData + pCoreBlock->mnSize)))
			return true;
	}

	return false;
}


void* HandleAllocator::FixedAllocatorEx::BeginGetNext()
{
	CharHandleInfo* pCHI = (CharHandleInfo*)&mpHeadCoreBlock->mData[0];

	mIterationContext.mpCurrentCoreBlock  = mpHeadCoreBlock;
	mIterationContext.mpCurrentHandleInfo = &pCHI->mHandleInfo - 1; // Move back by one because that way GetNext will return the first value.

	return &mIterationContext;     //lint !e1536 (Exposing low access member)
}


HandleAllocator::HandleInfo* HandleAllocator::FixedAllocatorEx::GetNext(void* /*pContext*/)
{
	if(mIterationContext.mpCurrentHandleInfo)
	{
		mIterationContext.mpCurrentHandleInfo++;

		if((char*)mIterationContext.mpCurrentHandleInfo >= ((char*)mIterationContext.mpCurrentCoreBlock + mIterationContext.mpCurrentCoreBlock->mnSize))
		{
			mIterationContext.mpCurrentCoreBlock  = mIterationContext.mpCurrentCoreBlock->mpNext;

			if(mIterationContext.mpCurrentCoreBlock)
			{
				CharHandleInfo* pCHI = (CharHandleInfo*)&mpHeadCoreBlock->mData[0];
				mIterationContext.mpCurrentHandleInfo = &pCHI->mHandleInfo;
			}
			else
				mIterationContext.mpCurrentHandleInfo = NULL;
		}
	}

	return mIterationContext.mpCurrentHandleInfo;
}



void HandleAllocator::FixedAllocatorEx::EndGetNext(void* /*pContext*/)
{
	mIterationContext.mpCurrentCoreBlock  = NULL;
	mIterationContext.mpCurrentHandleInfo = NULL;
}



} // namespace Allocator

} // namespace EA




