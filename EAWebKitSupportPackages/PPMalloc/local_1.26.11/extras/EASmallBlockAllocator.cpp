///////////////////////////////////////////////////////////////////////////////
// EASmallBlockAllocator
//
// Copyright (c) 2007, Electronic Arts. All Rights Reserved.
// Created by Henry Allen
//
///////////////////////////////////////////////////////////////////////////////

#include <EABase/eabase.h>

EA_DISABLE_VC_WARNING(4574)
#include <new> //for placement new
#include <string.h> //for memset
#include <stdio.h>
#include <stdarg.h>
EA_RESTORE_VC_WARNING()

#include "EASmallBlockAllocator.h"
#include "EASmallBlockAllocatorPool.h"
#include <PPMalloc/internal/shared.h>


#if defined(EA_COMPILER_MSVC)
	#pragma warning(push)
	#pragma warning(disable: 4355)
	#pragma warning(disable: 6011) // Dereferencing NULL pointer.  (VC++ /analysis is buggy)
#endif


namespace EA
{
namespace Allocator
{

/******************************************************************************/
/*! SmallBlockAllocator::Parameters::Parameters

	\brief      [Put your brief description here.]

	\param       - none.
	\return      -

	[Put your optional additional description here.]
*/
/******************************************************************************/
SmallBlockAllocator::Parameters::Parameters() : mnCoreBlockSize(kDefaultCoreBlockSize), mnInitCoreBlockCount(kDefaultInitCoreBlockCount), mnGrowCoreBlockCount(kDefaultGrowCoreBlockCount),
	mnMinAlignment(kDefaultMinAlignment), mnCustomPoolSize(NULL), mnMaxAlignment(kDefaultMaxAlignment), mnMaxAllocSize(kDefaultMaxMallocSize)
{
	if(mnMinAlignment < sizeof(uintptr_t))
		mnMinAlignment = sizeof(uintptr_t);
}

/******************************************************************************/
/*! SmallBlockAllocator::Parameters::Parameters

	\brief      [Put your brief description here.]

	\param      nCoreBlockSize      -
	\param      nInitCoreBlockCount -
	\param      nGrowCoreBlockCount -
	\param      nMinAlign       -
	\param      nMaxAlign       -
	\return                     -

	[Put your optional additional description here.]
*/
/******************************************************************************/
SmallBlockAllocator::Parameters::Parameters(uint16_t nCoreBlockSize, uint16_t nInitCoreBlockCount, uint16_t nGrowCoreBlockCount, uint16_t nMinAlign, uint16_t nMaxAlign, uint16_t nMaxAllocSize)
	: mnCoreBlockSize(nCoreBlockSize), mnInitCoreBlockCount(nInitCoreBlockCount), mnGrowCoreBlockCount(nGrowCoreBlockCount), mnMinAlignment(nMinAlign), mnCustomPoolSize(NULL), mnMaxAlignment(nMaxAlign), mnMaxAllocSize(nMaxAllocSize)
{
	if(mnMinAlignment < sizeof(uintptr_t))
		mnMinAlignment = sizeof(uintptr_t);
}

/******************************************************************************/
/*! SmallBlockAllocator::HookInfo::HookInfo

	\brief      [Put your brief description here.]

	\param      Type  -
	\param      nSize -
	\return           -

	[Put your optional additional description here.]
*/
/******************************************************************************/
SmallBlockAllocator::HookInfo::HookInfo(HookType Type, size_t nSize) : mType(Type), mnSize(nSize)
{
}

/******************************************************************************/
/*! SmallBlockAllocator::TraceCallback::TraceCallback

	\brief      [Put your brief description here.]

	\param       - none.
	\return      -

	[Put your optional additional description here.]
*/
/******************************************************************************/
SmallBlockAllocator::TraceCallback::TraceCallback() : mpTrace(NULL), mpTraceContext(NULL)
{
}

/******************************************************************************/
/*! SmallBlockAllocator::TraceCallback::TraceCallback

	\brief      [Put your brief description here.]

	\param      pTraceFunction -
	\param      pContext       -
	\return                    -

	[Put your optional additional description here.]
*/
/******************************************************************************/
SmallBlockAllocator::TraceCallback::TraceCallback(TraceFunction pTraceFunction, void* pContext) : mpTrace(pTraceFunction), mpTraceContext(pContext)
{
}

/******************************************************************************/
/*! SmallBlockAllocator::TraceCallback::~TraceCallback

	\brief      [Put your brief description here.]

	\param       - none.
	\return      -

	[Put your optional additional description here.]
*/
/******************************************************************************/
SmallBlockAllocator::TraceCallback::~TraceCallback()
{
}

/******************************************************************************/
/*! SmallBlockAllocator::Callbacks::Callbacks

	\brief      [Put your brief description here.]

	\param      poolMgr -
	\return             -

	[Put your optional additional description here.]
*/
/******************************************************************************/
SmallBlockAllocator::Callbacks::Callbacks(SmallBlockAllocator* poolMgr) : mpSmallBlockAllocator(poolMgr), mTrace(), mpAlloc(NULL), mpFree(NULL), mpAllocationContext(NULL),
	mpAllocationDebugContext(NULL), mpAssertionFailure(NULL), mpAssertionFailureContext(NULL), mpHookFunction(NULL), mpHookFunctionContext(NULL)
{
}

/******************************************************************************/
/*! SmallBlockAllocator::Callbacks::~Callbacks

	\brief      [Put your brief description here.]

	\param       - none.
	\return      -

	[Put your optional additional description here.]
*/
/******************************************************************************/
SmallBlockAllocator::Callbacks::~Callbacks()
{
}

/******************************************************************************/
/*! SmallBlockAllocator::AssertionFailureFunctionDefault

	\brief      [Put your brief description here.]

	\param      pAssertionFailureInfo -
	\param      pContext              -
	\return                           - void.

	[Put your optional additional description here.]
*/
/******************************************************************************/
void SmallBlockAllocator::AssertionFailureFunctionDefault(AssertionFailureInfo* pAssertionFailureInfo, void* pContext)
{
	TraceFunctionDefault(pAssertionFailureInfo->mpDescription, pContext);
	TraceFunctionDefault("\n", pContext);
	PPM_DebugBreak();
}

/******************************************************************************/
/*! SmallBlockAllocator::Callbacks::Tracef

	\brief      [Put your brief description here.]

	\param      fmt -
	\param      ... -
	\return         -

	[Put your optional additional description here.]
*/
/******************************************************************************/
int32_t SmallBlockAllocator::TraceCallback::Tracef(const char *fmt, ...)
{
#ifndef EA_SHIP
	char line[1024];
	va_list args;
	va_start(args,fmt);
	int32_t result = PPM_Vsnprintf(line, EAArrayCount(line), fmt, args);
	va_end(args);

	mpTrace(line, mpTraceContext);

	return result;
#else
	EA_UNUSED(fmt);
	return 0;       // vsprintf returns the number of characters written
#endif
}

/******************************************************************************/
/*! SmallBlockAllocator::SmallBlockAllocator

	\brief      [Put your brief description here.]

	\param      coreAllocator   - [description]
	\param      debugAllocator  - [description]
	\param      poolParams      - [description]
	\return                     - void

	[Put your optional additional description here.]
*/
/******************************************************************************/
SmallBlockAllocator::SmallBlockAllocator(SmallBlockAllocator::Parameters* pPoolParams, CoreAllocationFunction pAllocationFunction, CoreFreeFunction pFreeFunction, void* pAllocationContext, void* pAllocationDebugContext)
 : mCallbacks(this), mPagePool(NULL), mPools(NULL), mnPoolCount(0), mnCoreBlockAlignmentMask(0), mnMaxChunkSize(0),
	mnMaxAlignment(kDefaultMaxAlignment), mnMaxWastePerChunk(kDefaultMaxMallocWaste), mnPoolIncrementSize(kDefaultMinAlignment), mPoolRemap(NULL)
{
	mCallbacks.SetAssertionFailureFunction(AssertionFailureFunctionDefault, NULL);
	mCallbacks.SetTraceFunction(TraceFunctionDefault, NULL);

	Init(pPoolParams, pAllocationFunction, pFreeFunction, pAllocationContext, pAllocationDebugContext);
}

/******************************************************************************/
/*! SmallBlockAllocator::SmallBlockAllocator

	\brief      [Put your brief description here.]

	\param       - none.
	\return      -

	[Put your optional additional description here.]
*/
/******************************************************************************/
SmallBlockAllocator::SmallBlockAllocator() : mCallbacks(this), mPagePool(NULL), mPools(NULL), mnPoolCount(0), mnCoreBlockAlignmentMask(0), mnMaxChunkSize(0),
	mnMaxAlignment(kDefaultMaxAlignment), mnMaxWastePerChunk(kDefaultMaxMallocWaste), mnPoolIncrementSize(kDefaultMinAlignment), mPoolRemap(NULL)
{
	mCallbacks.SetAssertionFailureFunction(AssertionFailureFunctionDefault, NULL);
	mCallbacks.SetTraceFunction(TraceFunctionDefault, NULL);
}

/******************************************************************************/
/*! SmallBlockAllocator::~SmallBlockAllocator

	\brief      [Put your brief description here.]

	\param                      - none
	\return                     - void

	[Put your optional additional description here.]
*/
/******************************************************************************/
SmallBlockAllocator::~SmallBlockAllocator()
{
	Shutdown();
}

/******************************************************************************/
/*! SmallBlockAllocator::GetMaxAlignment

	\brief      [Put your brief description here.]

	\param      nCoreBlockSize           -
	\param      nDesiredMaxAlignment -
	\return                          -

	[Put your optional additional description here.]
*/
/******************************************************************************/
uint16_t SmallBlockAllocator::GetMaxAlignment(uint16_t nCoreBlockSize, uint16_t nDesiredMaxAlignment)
{
	uint16_t nMaxAlignment = nDesiredMaxAlignment;

	if((nCoreBlockSize % nMaxAlignment) != 0)
	{
		//find the largest power of two that evenly divides into CoreBlock size
		nMaxAlignment = 0;
		for ( uint16_t testAlignment = 8; (testAlignment <= nCoreBlockSize) && (0 == nCoreBlockSize%testAlignment); testAlignment *= 2 )
			nMaxAlignment = testAlignment;

		PPM_ASSERT(this, nMaxAlignment>=8, kGAErrorInvalidInput, kGAErrorText[kGAErrorInvalidInput], NULL, NULL);
		PPM_ASSERT(this, 0==nCoreBlockSize%nMaxAlignment, kGAErrorInvalidInput, kGAErrorText[kGAErrorInvalidInput], NULL, NULL);
	}

	PPM_ASSERT(this, PPM_IsPowerOf2(nMaxAlignment), kGAErrorInvalidInput, kGAErrorText[kGAErrorInvalidInput], NULL, NULL);
	return nMaxAlignment;
}

/******************************************************************************/
/*! SmallBlockAllocator::Init

	\brief      [Put your brief description here.]

	\param      coreAllocator -
	\param      debugAllocator  -
	\param      poolParams -
	\return                - void.

	[Put your optional additional description here.]
*/
/******************************************************************************/
void SmallBlockAllocator::Init(SmallBlockAllocator::Parameters* pPoolParams, CoreAllocationFunction pAllocationFunction, CoreFreeFunction pFreeFunction, void* pAllocationContext, void* pAllocationDebugContext)
{
	PPM_ASSERT(this, pPoolParams && (pPoolParams->mnCustomPoolSize || (pPoolParams->mnMaxAllocSize >= sizeof(void*)) || (pPoolParams->mnMaxAllocSize == kDefaultMaxMallocSize)), kGAErrorInvalidInput, kGAErrorText[kGAErrorInvalidInput], NULL, NULL);
	PPM_ASSERT(this, !mnPoolCount, kGAErrorInvalidInput, kGAErrorText[kGAErrorInvalidInput], NULL, NULL);  //already initialized?? mnPoolCount >0 indicates that we've been initalized

	mCallbacks.SetAllocationFunctions(pAllocationFunction, pFreeFunction, pAllocationContext, pAllocationDebugContext);

	mnMaxAlignment = GetMaxAlignment(pPoolParams->mnCoreBlockSize, pPoolParams->mnMaxAlignment);

	uint16_t nMinAlign = pPoolParams->mnMinAlignment;
	PPM_ASSERT(this, nMinAlign && PPM_IsPowerOf2(nMinAlign) && (nMinAlign <= mnMaxAlignment), kGAErrorInvalidInput, kGAErrorText[kGAErrorInvalidInput], NULL, NULL);
	PPM_ASSERT(this, nMinAlign >= sizeof(uintptr_t), kGAErrorInvalidInput, kGAErrorText[kGAErrorInvalidInput], NULL, NULL);

	if(nMinAlign < sizeof(uintptr_t))
		nMinAlign = sizeof(uintptr_t);

	//set pool increment size ... it is equal to nMinAlign since having pools for sizes between alignment increments wouldn't make sense
	//thus mnPoolIncrementSize is also our min allocation alignment
	mnPoolIncrementSize = nMinAlign;

	//memCoreBlockSize must be evenly divisible by mnMaxAlignment!
	//this ensures that we can allocate Chunks of CoreBlocks and have the alignment
	//for each be correct to satisfy min alignment.
	PPM_ASSERT(this,  0 == (pPoolParams->mnCoreBlockSize%mnMaxAlignment) , kGAErrorInvalidInput, kGAErrorText[kGAErrorInvalidInput], NULL, NULL);
	//mnPoolIncrementSize is our mininum alignment as well ... by enforcing that CoreBlocksize is an even multiple of this we can force all Chunks
	//to be aligned to the min value by default
	PPM_ASSERT(this,  0 == (pPoolParams->mnCoreBlockSize%mnPoolIncrementSize) , kGAErrorInvalidInput, kGAErrorText[kGAErrorInvalidInput], NULL, NULL);

	//initialize the pool of CoreBlocks that will be shared by all of the individual small alloc pools
	//the maximum alignment required by any allocations is given to the CoreBlock pool as the minimum alignment of CoreBlocks to ensure
	//that CoreBlocks are at least aligned such that any allocation taken from the start of the CoreBlock meets min alignment requirements.
	//this allows us to assume that allocations that require alignment <= mnMaxAlignment will be aligned if taken from a pool whose
	//Chunk size is evenly divisible by 'alignment'.
	mPagePool = reinterpret_cast<Pool*>( mCallbacks.CoreAlloc(sizeof(Pool)) );
	mPagePool = new(mPagePool) Pool(mCallbacks);
	mPagePool->Init(pPoolParams->mnCoreBlockSize, pPoolParams->mnInitCoreBlockCount, pPoolParams->mnGrowCoreBlockCount, mnMaxAlignment);

	uint32_t nPoolCount = 0;

	if (pPoolParams->mnCustomPoolSize)
	{
		//validate the custom params
		uint16_t nPrevSize = 0;
		uint16_t* pPoolSize = pPoolParams->mnCustomPoolSize;

		while (*pPoolSize)
		{
			PPM_ASSERT(this,  nPrevSize < *pPoolSize, kGAErrorInvalidInput, kGAErrorText[kGAErrorInvalidInput], NULL, NULL);
			PPM_ASSERT(this,  0 == ((*pPoolSize)%mnPoolIncrementSize) , kGAErrorInvalidInput, kGAErrorText[kGAErrorInvalidInput], NULL, NULL);

			nPrevSize = *pPoolSize;
			nPoolCount++;
			pPoolSize++;
		}

		mnMaxChunkSize = nPrevSize;
		PPM_ASSERT(this, static_cast<uint32_t>(mnMaxChunkSize) <= (pPoolParams->mnCoreBlockSize-CoreBlockOverhead()), kGAErrorInternal, kGAErrorText[kGAErrorInternal], NULL, NULL);

		const uint32_t nRemapTableSize = static_cast<uint32_t>(mnMaxChunkSize / mnPoolIncrementSize);

		mPoolRemap = reinterpret_cast<uint16_t*>( mCallbacks.CoreAlloc(sizeof(uint16_t) * nRemapTableSize) );
		PPM_ASSERT(this, mPoolRemap, kGAErrorInternal, kGAErrorText[kGAErrorInternal], NULL, NULL);

		//allocate buffer large enough to hold nPoolCount Pool objects
		mPools = reinterpret_cast<Pool*>( mCallbacks.CoreAlloc(sizeof(Pool) * nPoolCount) );

		Pool* pCoreBlockedPool;
		unsigned int nCurChunkSize = mnPoolIncrementSize;
		unsigned int nRemapIndex = 0;

		for (uint16_t nPoolIndex = 0; nPoolIndex < nPoolCount; nPoolIndex++)
		{
			uint16_t nPoolSize = pPoolParams->mnCustomPoolSize[nPoolIndex];
			//placement new the CoreBlocked pool in the corect place in the buffer then intialize it
			pCoreBlockedPool = new(&(mPools[nPoolIndex])) Pool(mCallbacks);
			pCoreBlockedPool->Init(nPoolSize, mPagePool);
			pCoreBlockedPool->SetOption(kOptionAutoShrink, 0); //disable auto shrink, we'll shrink as needed

			for ( ; nCurChunkSize <= nPoolSize; nCurChunkSize += mnPoolIncrementSize)
				mPoolRemap[nRemapIndex++] = nPoolIndex;
		}
		PPM_ASSERT(this, nRemapIndex==nRemapTableSize, kGAErrorInternal, kGAErrorText[kGAErrorInternal], NULL, NULL);
	}
	else
	{
		const unsigned int nUsableCoreBlockSize = pPoolParams->mnCoreBlockSize-CoreBlockOverhead();

		if(pPoolParams->mnMaxAllocSize == kDefaultMaxMallocSize)
		{
			mnMaxChunkSize = static_cast<uint16_t>(nUsableCoreBlockSize / 2);
			//make sure the cast to uint16_t didn't lose any data
			PPM_ASSERT(this, static_cast<uint32_t>(mnMaxChunkSize) == (nUsableCoreBlockSize) / 2, kGAErrorInternal, kGAErrorText[kGAErrorInternal], NULL, NULL);
		}
		else
		{
			mnMaxChunkSize = pPoolParams->mnMaxAllocSize;
			PPM_ASSERT(this,  mnMaxChunkSize <= nUsableCoreBlockSize , kGAErrorInvalidInput, kGAErrorText[kGAErrorInvalidInput], NULL, NULL);
		}
		PPM_ASSERT(this,  0 == (mnMaxChunkSize%mnPoolIncrementSize) , kGAErrorInvalidInput, kGAErrorText[kGAErrorInvalidInput], NULL, NULL);

		const unsigned int nRemapTableSize = static_cast<uint32_t>(mnMaxChunkSize / mnPoolIncrementSize);

		mPoolRemap = reinterpret_cast<uint16_t*>( mCallbacks.CoreAlloc(sizeof(uint16_t) * nRemapTableSize) );
		PPM_ASSERT(this, mPoolRemap, kGAErrorInternal, kGAErrorText[kGAErrorInternal], NULL, NULL);

		//fill in the table and count out how many pools we need so we can allocate an array of the appropriate size..
		unsigned int nCurChunksPerCoreBlock = 0;
		unsigned int nRemapIndex = 0;
		uint16_t nPoolIndex = 0;

		PPM_ASSERT(this, mnMaxChunkSize >= mnPoolIncrementSize, kGAErrorInternal, kGAErrorText[kGAErrorInternal], NULL, NULL);  // If this fails then something about the parameters is wrong and we have an impossible situation to deal with.
		for (unsigned int nCurChunkSize = mnPoolIncrementSize; nCurChunkSize <= mnMaxChunkSize; nCurChunkSize+=mnPoolIncrementSize )
		{
			nCurChunksPerCoreBlock = nUsableCoreBlockSize / nCurChunkSize;
			for (uint32_t nNextChunkSize = nCurChunkSize+mnPoolIncrementSize; (nNextChunkSize <= mnMaxChunkSize) && (nCurChunksPerCoreBlock == (nUsableCoreBlockSize / nNextChunkSize)); nCurChunkSize = nNextChunkSize, nNextChunkSize=nCurChunkSize+mnPoolIncrementSize)
				mPoolRemap[nRemapIndex++] = nPoolIndex;
			mPoolRemap[nRemapIndex++] = nPoolIndex++;
		}

		PPM_ASSERT(this, nPoolIndex > 0, kGAErrorInternal, kGAErrorText[kGAErrorInternal], NULL, NULL);  // If this fails then something about the parameters is wrong.
		PPM_ASSERT(this, nRemapIndex==nRemapTableSize, kGAErrorInternal, kGAErrorText[kGAErrorInternal], NULL, NULL);
		nPoolCount = nPoolIndex;

		//allocate buffer large enough to hold nPoolCount Pool objects
		mPools = reinterpret_cast<Pool*>( mCallbacks.CoreAlloc(sizeof(Pool) * nPoolCount) );

		//traverse the remap table logic again, this time initializing the pools
		Pool* pCoreBlockedPool;
		nCurChunksPerCoreBlock = 0;
		nPoolIndex = 0;
		for (unsigned int nCurChunkSize = mnPoolIncrementSize; nCurChunkSize <= mnMaxChunkSize; nCurChunkSize+=mnPoolIncrementSize )
		{
			nCurChunksPerCoreBlock = nUsableCoreBlockSize / nCurChunkSize;
			for (unsigned int nNextChunkSize = nCurChunkSize+mnPoolIncrementSize; (nNextChunkSize <= mnMaxChunkSize) && (nCurChunksPerCoreBlock == (nUsableCoreBlockSize / nNextChunkSize)); nCurChunkSize = nNextChunkSize, nNextChunkSize=nCurChunkSize+mnPoolIncrementSize)
			{
				//nothing to do here, just skipping remapped pools
			}

			//placement new the CoreBlocked pool in the corect place in the buffer then intialize it
			pCoreBlockedPool = new(&(mPools[nPoolIndex])) Pool(mCallbacks);
			pCoreBlockedPool->Init(static_cast<uint16_t>(nCurChunkSize), mPagePool);
			pCoreBlockedPool->SetOption(kOptionAutoShrink, 0); //disable auto shrink, we'll shrink as needed

			nPoolIndex++;
		}
		PPM_ASSERT(this, nPoolIndex == nPoolCount, kGAErrorInternal, kGAErrorText[kGAErrorInternal], NULL, NULL);
	}

	if (mnMaxAlignment >= mPagePool->GetChunkSize() && PPM_IsPowerOf2(mPagePool->GetChunkSize()))
		mnCoreBlockAlignmentMask = ~((uintptr_t)(unsigned)mPagePool->GetChunkSize() - 1);

	//this statement comes last because
	//mnPoolCount > 0 indicates that the system is fully initialized
	mnPoolCount = nPoolCount;
}

/******************************************************************************/
/*! SmallBlockAllocator::Shutdown

	\brief      [Put your brief description here.]

	\param       - none.
	\return      - void.

	[Put your optional additional description here.]
*/
/******************************************************************************/
void SmallBlockAllocator::Shutdown()
{
	if ( IsInitialized() )
	{
		//store the mnPoolCount in a temporary and set to 0 to indicate that
		//this SmallBlockAllocator is no longer valid/initialized during Shutdown
		uint32_t nPoolCount = mnPoolCount;
		mnPoolCount = 0;

		if ( mPools )
		{
			//we used placement new to init these guys so destruct each one manually
			for ( uint32_t nPoolIndex = 0 ; nPoolIndex < nPoolCount; nPoolIndex++ )
				mPools[nPoolIndex].~Pool();

			//free the memory for the pool objects
			mCallbacks.CoreFree(mPools);
			mPools = NULL;

			if (mPagePool)
			{
				mPagePool->~Pool();
				mCallbacks.CoreFree(mPagePool);
				mPagePool = NULL;
			}

			PPM_ASSERT(this, mPoolRemap, kGAErrorInternal, kGAErrorText[kGAErrorInternal], NULL, NULL);
			mCallbacks.CoreFree(mPoolRemap);
			mPoolRemap = NULL;
		}
	}
}

/******************************************************************************/
/*! SmallBlockAllocator::Malloc

	\brief      [Put your brief description here.]

	\param      nSize      -
	\param      allocInfo -
	\return               - void.

	[Put your optional additional description here.]
*/
/******************************************************************************/
void* SmallBlockAllocator::Malloc(size_t nSize, const EA::Allocator::AllocInfo& allocInfo)
{
	void* pAddress(NULL);

	if (nSize <= mnMaxChunkSize) // This should usually be true.
	{
		//calculate a pool index assuming we have one pool per mnPoolIncrementSize increment
		size_t nPoolIndex(nSize ? ((nSize-1) / (mnPoolIncrementSize)) : 0);
		//remap the index to our actual pools
		nPoolIndex = mPoolRemap[nPoolIndex];
		PPM_ASSERT(this, (nPoolIndex < mnPoolCount), kGAErrorInternal, kGAErrorText[kGAErrorInternal], NULL, NULL);

		//if the pool can handle the request without too much waste, allocate
		if ( (mPools[nPoolIndex].GetChunkSize() - nSize) <= mnMaxWastePerChunk )
		{
			pAddress = mPools[nPoolIndex].Alloc(nSize, allocInfo);

			#if (PPM_DEBUG >= 2)
				//verify that address is aligned to mnPoolIncrementSize which is also min alignment
				PPM_ASSERT(this, !pAddress || PPM_IsAligned(pAddress, mnPoolIncrementSize), kGAErrorInternal, kGAErrorText[kGAErrorInternal], NULL, NULL);
			#endif
		}
	}

	return pAddress;
}

/******************************************************************************/
/*! SmallBlockAllocator::Malloc

	\brief      [Put your brief description here.]

	\param      nSize            -
	\param      nAlignment       -
	\param      nAlignmentOffset -
	\param      allocInfo       -
	\return                     - void.

	[Put your optional additional description here.]
*/
/******************************************************************************/
void* SmallBlockAllocator::Malloc(size_t nSize, size_t nAlignment, size_t nAlignmentOffset, const EA::Allocator::AllocInfo& allocInfo)
{
	void* pAddress(NULL);

	//pools don't support alignment offset or allocations above mnMaxChunkSize or requests for alignment > mnMaxAlignment
	if ( !nAlignmentOffset && (nSize <= mnMaxChunkSize) && (nAlignment <= mnMaxAlignment) )
	{
		//calculate a pool index assuming we have one pool per mnPoolIncrementSize increment
		size_t nPoolIndex(nSize ? ((nSize-1) / (mnPoolIncrementSize)) : 0);
		//remap the index to our actual pools
		nPoolIndex = mPoolRemap[nPoolIndex];
		PPM_ASSERT(this, (nPoolIndex < mnPoolCount), kGAErrorInternal, kGAErrorText[kGAErrorInternal], NULL, NULL);

		if ( !nAlignment || (0 == (mPools[nPoolIndex].GetChunkSize() % nAlignment)) )  // If the user doesn't specify alignment or given pool's Chunk size doesn't match the alignment...
		{
			//if the pool can handle the request without too much waste, allocate
			if ( (mPools[nPoolIndex].GetChunkSize() - nSize) <= mnMaxWastePerChunk )
			{
				pAddress = mPools[nPoolIndex].Alloc(nSize, allocInfo);

				#if (PPM_DEBUG >= 2)
					//verify that address is aligned to nAlignment if specified, otherwise mnPoolIncrementSize which is the default min alignment
					PPM_ASSERT(this, !pAddress || PPM_IsAligned(pAddress, (nAlignment ? nAlignment : mnPoolIncrementSize) ), kGAErrorInternal, kGAErrorText[kGAErrorInternal], NULL, NULL);
				#endif
			}
		}
		else
		{
			//loop through the current and subsequent pools until we find one with proper alignment or 
			//one that has a big enough Chunk size to allow us to align the allocation within a Chunk.
			Pool* pCurPool(NULL);

			for ( ; nPoolIndex < mnPoolCount; nPoolIndex++)
			{
				pCurPool = &(mPools[nPoolIndex]);

				if((pCurPool->GetChunkSize() % nAlignment) == 0)
				{
					//if the pool can handle the request without too much waste, allocate
					if ( (pCurPool->GetChunkSize() - nSize) <= mnMaxWastePerChunk )
					{
						pAddress = pCurPool->Alloc(nSize, allocInfo);

						#if (PPM_DEBUG >= 2)
							//verify that address is aligned to nAlignment
							PPM_ASSERT(this, !pAddress || PPM_IsAligned(pAddress, nAlignment), kGAErrorInternal, kGAErrorText[kGAErrorInternal], NULL, NULL);
						#endif
					}
					break;
				}
				//TODO: I think there may be a trick we can do here to reduce alignment space required in some cases due to knowing that we are already aligned to at least mnPoolIncrementSize
				else if ( (pCurPool->GetChunkSize()-nSize) >= nAlignment )
				{
					//if the pool can handle the request without too much waste, allocate
					if ( (pCurPool->GetChunkSize() - nSize) <= mnMaxWastePerChunk )
					{
						pAddress = pCurPool->Alloc(nSize, allocInfo);

						if (pAddress)
							pAddress = PPM_AlignUp(pAddress, nAlignment);
					}
					break;
				}
			}
		}
	}

	return pAddress;
}

/******************************************************************************/
/*! SmallBlockAllocator::Realloc

	\brief      [Put your brief description here.]

	\param      pOld     -
	\param      nNewSize -
	\param      nOldSize -
	\return              - void.

	[Put your optional additional description here.]
*/
/******************************************************************************/
void* SmallBlockAllocator::Realloc(void* pOld, size_t nNewSize, size_t nOldSize)
{
	// For new size of zero, C89 but not C99 standard calls for the same behaviour as free.
	#if !PPM_REALLOC_C99_ENABLED // If C89 behaviour...
		if(EA_UNLIKELY(nNewSize == 0))
		{
			if(pOld)
				Free(pOld);
			return NULL;
		}
	#endif

	if(pOld)
	{
		Pool* pNestedCoreBlockOwner(mPagePool->NestedCoreBlockGetPool(pOld));

		// pOld must be a valid pointer for us, otherwise we cannot know how to copy the old data.
		PPM_ASSERT(this, (pNestedCoreBlockOwner != NULL), kGAErrorBadUserPtr, kGAErrorText[kGAErrorBadUserPtr], NULL, NULL);

		nOldSize = pNestedCoreBlockOwner->GetChunkSize();

		// If we can make the realloc a no-op, do so.
		if((nNewSize < nOldSize) && ((int32_t)nNewSize > (int32_t)(nOldSize - mnMaxWastePerChunk)))
			return pOld;

		void* const pNew = Malloc(nNewSize);

		if(pNew)
		{
			const uint32_t nCopySize = (nOldSize < nNewSize) ? (uint32_t)nOldSize : (uint32_t)nNewSize;
			memcpy(pNew, pOld, nCopySize);

			pNestedCoreBlockOwner->Free(pOld);
		 }

		 return pNew;
	}

	// For old pointer of NULL, C89 and C99 standards call for same behaviour as malloc.
	return Malloc(nNewSize);
}

/******************************************************************************/
/*! SmallBlockAllocator::FreeSafe

	\brief      [Put your brief description here.]

	\param      pAddress -
	\param      nSize   -
	\return            - void.

	[Put your optional additional description here.]
*/
/******************************************************************************/
bool SmallBlockAllocator::FreeSafe(void* pAddress, size_t)
{
	#if PPM_NULL_POINTER_FREE_ENABLED
	if(pAddress)
	#endif
	{
		//we know that if this memory lies within any of our pools it also lies
		//within our page pool (since they are all funded by it).
		//In this case it's faster to use NestedCoreBlockFree than to search all of
		//the pools loooking for the one to free this memory from.
		return mPagePool->NestedCoreBlockFree(pAddress);
	}

	#if PPM_NULL_POINTER_FREE_ENABLED
	return true; //null pointer free...
	#endif
}

/******************************************************************************/
/*! SmallBlockAllocator::Free

	\brief      [Put your brief description here.]

	\param      pAddress -
	\param      size_t   -
	\return              - void.

	[Put your optional additional description here.]
*/
/******************************************************************************/
void SmallBlockAllocator::Free(void* pAddress, size_t)
{
	#if PPM_NULL_POINTER_FREE_ENABLED
	if(pAddress)
	#endif
	{
		if (mnCoreBlockAlignmentMask)
		{
			//we can gaurentee that mPagePool Chunks are always aligned to their own size, we
			//can use this information to calculate the CoreBlock address rather than searching for it
			#if (PPM_DEBUG >= 2)
				PPM_ASSERT(this, ValidateAddress(pAddress, kAddressTypeAllocated) != NULL, kGAErrorBadUserPtr, kGAErrorText[kGAErrorBadUserPtr], NULL, NULL);
			#endif

			//this is the fastest way to free ... AlignedCoreBlockFree assumes that all CoreBlocks
			//are aligned to mPagePool->GetChunkSize, mPagePool->GetChunkSize is a power of 2,
			//and that pAddress was actually allocated from this allocator
			SmallBlockAllocator::Pool::AlignedCoreBlockFree(pAddress,mnCoreBlockAlignmentMask);
		}
		else
		{
			//we know that if this memory lies within any of our pools it also lies
			//within our page pool (since they are all funded by it).
			//In this case it's faster to use NestedCoreBlockFree than to search all of
			//the pools loooking for the one to free this memory from.
			mPagePool->NestedCoreBlockFree(pAddress);
		}
	}
}

/******************************************************************************/
/*! SmallBlockAllocator::ShrinkPools

	\brief      [Put your brief description here.]

	\param       - none.
	\return          - void.

	shrinks all pools in the pool manager to attempt to free up memory
*/
/******************************************************************************/
void SmallBlockAllocator::ShrinkPools()
{
	if (IsInitialized())
	{
		PPM_ASSERT(this, mPools, kGAErrorInternal, kGAErrorText[kGAErrorInternal], NULL, NULL);

		for (uint32_t nPoolIndex = 0; nPoolIndex < mnPoolCount; nPoolIndex++ )
			mPools[nPoolIndex].Shrink();
	}
}

/******************************************************************************/
/*! SmallBlockAllocator::SetOption

	\brief      [Put your brief description here.]

	\param      nOption -
	\param      nValue  -
	\return             - void.

	[Put your optional additional description here.]
*/
/******************************************************************************/
void SmallBlockAllocator::SetOption(int nOption, int nValue)
{
	switch (nOption)
	{
		case kOptionMaxAllocWaste:
			mnMaxWastePerChunk = static_cast<uint16_t>(nValue);
			//validate that the input fits into uint16_t
			PPM_ASSERT(this, (nValue>=0) && (nValue==static_cast<int>(mnMaxWastePerChunk)), kGAErrorInvalidInput, kGAErrorText[kGAErrorInvalidInput], NULL, NULL);
			break;

		case kOptionEnableExtensions:
			if (nValue != kExtensionNone)
			{
				if (kExtensionRestrictToPagePool == (nValue & kExtensionRestrictToPagePool))
					mPagePool->SetOption(kOptionEnableExtensions, nValue); //enable extension(s) on only the page pool
				else
				{
					//apply to page pool and all of the individual pools
					mPagePool->SetOption(kOptionEnableExtensions, nValue); //enable extensions

					uint32_t nPoolIndex(0);                                                                
					for ( nPoolIndex = 0; (nPoolIndex < mnPoolCount) ; nPoolIndex++ )                      
						mPools[nPoolIndex].SetOption(kOptionEnableExtensions, nValue); //enable extensions 
				}
			}
			break;

		case kOptionGrowCoreBlockCount:
			mPagePool->SetOption(nOption, nValue);
			break;

		default:
			break;
	}
}

/******************************************************************************/
/*! SmallBlockAllocator::ReportBegin

	\brief      [Put your brief description here.]

	\param      pReportContext -
	\param      nNodeTypeFlags -
	\return                   -

	[Put your optional additional description here.]
*/
/******************************************************************************/
SmallBlockAllocator::ReportContext* SmallBlockAllocator::ReportBegin(ReportContext* pReportContext, uint32_t nNodeTypeFlags)
{
	memset(pReportContext, 0, sizeof(ReportContext));

	pReportContext->mNodeTypeFlags = nNodeTypeFlags;

	return pReportContext;
}

/******************************************************************************/
/*! SmallBlockAllocator::ReportNext

	\brief      [Put your brief description here.]

	\param      pReportContext -
	\param      nNodeTypeFlags -
	\return                   -

	[Put your optional additional description here.]
*/
/******************************************************************************/
const SmallBlockAllocator::ReportNode* SmallBlockAllocator::ReportNext(ReportContext* pReportContext, uint32_t nNodeTypeFlags)
{
	const ReportNode* pResult(NULL);
	uint32_t nPoolIndex(pReportContext->mnPoolIndex);
	Pool* pCurPool = &(mPools[nPoolIndex]);

	if (!pReportContext->mPool)
	{
		pCurPool->ReportBegin(pReportContext,nNodeTypeFlags);
		pReportContext->mnPoolIndex = nPoolIndex; //restore this since it may have just been wiped out
	}

	pResult = pCurPool->ReportNext(pReportContext,nNodeTypeFlags);

	if (pResult)
		return pResult;

	pCurPool->ReportEnd(pReportContext);

	nPoolIndex++;

	if (nPoolIndex < mnPoolCount)
	{
		pReportContext->mnPoolIndex = nPoolIndex;
		return ReportNext(pReportContext,nNodeTypeFlags);
	}

	return NULL;
}

/******************************************************************************/
/*! SmallBlockAllocator::ReportEnd

	\brief      [Put your brief description here.]

	\param      pReportContext -
	\return                   - void.

	[Put your optional additional description here.]
*/
/******************************************************************************/
void SmallBlockAllocator::ReportEnd(ReportContext* pReportContext)
{
	// We need do nothing significant in this implementation.
	// But we may need to do something in the future.
	#if (PPM_DEBUG >= 1)
		memset(pReportContext, 0, sizeof(ReportContext));
	#else
		EA_UNUSED(pReportContext);
	#endif
}

/******************************************************************************/
/*! SmallBlockAllocator::DumpPools

	\brief      [Put your brief description here.]

	\param      pCallback        -
	\param      pCallbackContext -
	\return                      - void.

	[Put your optional additional description here.]
*/
/******************************************************************************/
void SmallBlockAllocator::DumpPools(SmallBlockAllocator::TraceCallback* pTraceCallback, uint32_t nodeTypeFlags)
{
	if (!pTraceCallback)
		pTraceCallback = &mCallbacks.mTrace;

	pTraceCallback->Trace( "SmallBlockAllocator dump (for CoreBlock pool). This is the main pool whose 'Chunks' are used as 'CoreBlocks' by all of the individual pools.\n");

	//TODO, clean this up ... the tracestats method should probably take a Poolinfo as a param since it ends up calling GetStats and we end up needing to 
	// re-call it here (same below for each of the invidual pools)
	// 
	mPagePool->TraceStats(pTraceCallback);
	mPagePool->TraceMemory(pTraceCallback, nodeTypeFlags);

	PoolInfo poolStats;
	mPagePool->GetStats(poolStats);

	//get available core blocks in the page pool for our summary
	//(chunks from the page pool are core blocks for the other pools)
	uint32_t nCoreBlockSize = poolStats.ChunkSize();
	uint32_t nPagePoolEmptyCoreBlocks = poolStats.FreeChunkCount();

	pTraceCallback->Tracef( "\nSmallBlockAllocator dump (for %d pools attached to the CoreBlock pool). 'CoreBlocks' in each of these pools are 'Chunks' from the main CoreBlock pool.\n", mnPoolCount);

	uint32_t nTotalChunkCount(0);
	uint32_t nTotalFreeChunkCount(0);
	uint32_t nTotalWaste(0);
	uint32_t nTotalOverhead(0);
	uint32_t nTotalAvailableMem(0);
	uint32_t nTotalUseableMem(0);
	uint32_t nTotalCoreBlockEmptyCount(0);

	uint32_t nTotalChunkCountHighWaterMark(0);
	uint32_t nTotalFreeChunkCountHighWaterMark(0);
	uint32_t nTotalWasteHighWaterMark(0);
	uint32_t nTotalOverheadHighWaterMark(0);
	uint32_t nTotalAvailableMemHighWaterMark(0);
	uint32_t nTotalUseableMemHighWaterMark(0);
	bool highWaterMarkStatsValid = true;

	uint32_t nPoolIndex(0);
	for ( nPoolIndex = 0; (nPoolIndex < mnPoolCount) ; nPoolIndex++ )
	{
		Pool* pool(&(mPools[nPoolIndex]));

		PPM_ASSERT(this, pool->Initialized(), kGAErrorInternal, kGAErrorText[kGAErrorInternal], NULL, NULL);

		pool->TraceStats(pTraceCallback);
		pool->TraceMemory(pTraceCallback, nodeTypeFlags);

		pool->GetStats(poolStats);
		nTotalChunkCount += poolStats.ChunkCount();
		nTotalFreeChunkCount += poolStats.FreeChunkCount();
		nTotalWaste += poolStats.CoreBlockWasteTotal();
		nTotalOverhead += poolStats.TotalOverhead();
		nTotalAvailableMem += poolStats.FreeChunkSizeTotal();
		nTotalUseableMem += poolStats.ChunkSizeTotal();
		nTotalCoreBlockEmptyCount += poolStats.CoreBlockEmptyCount();

		if (highWaterMarkStatsValid)
		{
			if (pool->GetHighWaterMarkStats(poolStats))
			{
				nTotalChunkCountHighWaterMark += poolStats.ChunkCount();
				nTotalFreeChunkCountHighWaterMark += poolStats.FreeChunkCount();
				nTotalWasteHighWaterMark += poolStats.CoreBlockWasteTotal();
				nTotalOverheadHighWaterMark += poolStats.TotalOverhead();
				nTotalAvailableMemHighWaterMark += poolStats.FreeChunkSizeTotal();
				nTotalUseableMemHighWaterMark += poolStats.ChunkSizeTotal();
			}
			else
				highWaterMarkStatsValid = false;
		}
	}

	//summarize waste and overhead for this set of pools
	const float   nWastePerChunk( static_cast<float>(nTotalWaste) / static_cast<float>(nTotalChunkCount) );
	const float   nOverheadPerChunk( static_cast<float>(nTotalOverhead) / static_cast<float>(nTotalChunkCount) );
	const float   nOverheadPerAllocatedChunk( static_cast<float>(nTotalOverhead + nTotalAvailableMem) / static_cast<float>(nTotalChunkCount-nTotalFreeChunkCount) );
	pTraceCallback->Tracef( "\nSummary (based on current snapshot of all pools):\n"
					" Total Chunks = %d (%d bytes)\n"
					" Unused Chunks = %d (%d bytes)\n"
					" Waste total (because Chunks don't always divide evenly into CoreBlocks) = %d bytes (%.02f bytes per Chunk)\n"
					" Overhead total (includes waste and CoreBlock headers) = %d bytes (%.02f bytes per Chunk)\n"
					" Unused+overhead per allocated Chunk = %.02f bytes per Chunk\n"
					" Empty core blocks in core block pool = %d (%d bytes)\n"
					" Empty core blocks in the other pools = %d (%d bytes) [these are incuded in the Unused chunks count]\n",
					nTotalChunkCount, nTotalUseableMem,
					nTotalFreeChunkCount, nTotalAvailableMem,
					nTotalWaste, nWastePerChunk ,
					nTotalOverhead, nOverheadPerChunk,
					nOverheadPerAllocatedChunk,
					nPagePoolEmptyCoreBlocks,  nCoreBlockSize*nPagePoolEmptyCoreBlocks,
					nTotalCoreBlockEmptyCount, nCoreBlockSize*nTotalCoreBlockEmptyCount );

	if (highWaterMarkStatsValid)
	{
		const float   nWastePerChunkHighWaterMark( static_cast<float>(nTotalWasteHighWaterMark) / static_cast<float>(nTotalChunkCountHighWaterMark) );
		const float   nOverheadPerChunkHighWaterMark( static_cast<float>(nTotalOverheadHighWaterMark) / static_cast<float>(nTotalChunkCountHighWaterMark) );
		const float   nOverheadPerAllocatedChunkHighWaterMark( static_cast<float>(nTotalOverheadHighWaterMark + nTotalAvailableMemHighWaterMark) / static_cast<float>(nTotalChunkCountHighWaterMark-nTotalFreeChunkCountHighWaterMark) );
		pTraceCallback->Tracef( "\nHigh Water Mark Summary (assuming each pool is at its high water mark simultaneously (not likely)):\n"
						" Total Chunks = %d (%d bytes)\n"
						" Unused Chunks = %d (%d bytes)\n"
						" Waste total (because Chunks don't always divide evenly into CoreBlocks) = %d bytes (%.02f bytes per Chunk)\n"
						" Overhead total (includes waste and CoreBlock headers) = %d bytes (%.02f bytes per Chunk)\n"
						" Unused+overhead per allocated Chunk = %.02f bytes per Chunk\n",
						nTotalChunkCountHighWaterMark, nTotalUseableMemHighWaterMark,
						nTotalFreeChunkCountHighWaterMark, nTotalAvailableMemHighWaterMark,
						nTotalWasteHighWaterMark, nWastePerChunkHighWaterMark,
						nTotalOverheadHighWaterMark, nOverheadPerChunkHighWaterMark,
						nOverheadPerAllocatedChunkHighWaterMark );
	}

	//summarize CoreBlock usage...
	if (mPagePool->GetHighWaterMarkStats(poolStats))
	{
		pTraceCallback->Trace("\nCoreBlock Usage Summary:\n");

		if (poolStats.CoreBlockCount() > 1)
		{
			//the Chunks of the page pool are 'CoreBlocks' for the individual pools. See how many we were using at high water mark.
			uint32_t peakCoreBlockCount = (poolStats.ChunkCount() - poolStats.FreeChunkCount());
			//consider increasing initMemCoreBlocks
			pTraceCallback->Tracef( " At peak usage, the main CoreBlock pool had to grow to %d CoreBlocks (%d bytes)."
				" For optimum performace increase initMemCoreBlocks to at least %d so that the main CoreBlock pool doesn't have to grow after the initial grow.\n",
				peakCoreBlockCount, (peakCoreBlockCount*poolStats.ChunkSize()), peakCoreBlockCount);
		}
		else
		{
			pTraceCallback->Tracef( " %d CoreBlocks (out of initMemCoreBlocks for this manager) were never used (%d bytes).\n",
				poolStats.FreeChunkCount(), poolStats.FreeChunkSizeTotal());
		}
	}
}

/******************************************************************************/
/*! SmallBlockAllocator::SetGroupId

	\brief      [Put your brief description here.]

	\param      groupId -
	\return             -

	[Put your optional additional description here.]
*/
/******************************************************************************/
int32_t SmallBlockAllocator::SetGroupId(int32_t nGroupId)
{
	//all pools (including the CoreBlock pool) share the same Id
	uint32_t nPoolIndex(0);
	for ( nPoolIndex = 0; (nPoolIndex < mnPoolCount) ; nPoolIndex++ )
	{
		Pool* pPool(&(mPools[nPoolIndex]));
		pPool->SetGroupId(nGroupId);
	}
	return mPagePool->SetGroupId(nGroupId);
}

/******************************************************************************/
/*! SmallBlockAllocator::GetGroupId

	\brief      [Put your brief description here.]

	\param       - none.
	\return          -

	[Put your optional additional description here.]
*/
/******************************************************************************/
int32_t SmallBlockAllocator::GetGroupId() const
{
	//all pools (including the CoreBlock pool) share the same Id
	return mPagePool->GetGroupId();
}

/******************************************************************************/
/*! SmallBlockAllocator::Audit

	\brief      [Put your brief description here.]

	\param      nGroupId         -
	\param      pCallBack        -
	\param      pCallbackContext -
	\return                      -

	[Put your optional additional description here.]
*/
/******************************************************************************/
uint32_t SmallBlockAllocator::Audit(int32_t nGroupId, AuditFunction pCallBack, void* pCallbackContext)
{
	uint32_t nChunksFound = 0;

	uint32_t nPoolIndex(0);
	for ( nPoolIndex = 0; (nPoolIndex < mnPoolCount) ; nPoolIndex++ )
	{
		Pool* pPool(&(mPools[nPoolIndex]));
		nChunksFound += pPool->Audit(nGroupId, pCallBack, pCallbackContext);
	}

	//Don't audit the CoreBlock pool
	// The CoreBlocks are only allocated to pools as needed for other specific allocations
	// and those allocations are being audited or not as appropriate so auditing the
	// CoreBlocks isn't really necessary. Additionally, CoreBlock auditing wouldn't really be
	// accurate since it's possible for allocations of different groups to get into the
	// CoreBlock (meaning that it might not be freed by the time the group it was originally
	// allocated for is being audited).
	//nChunksFound += mPagePool->Audit(nGroupId, pCallBack, pCallbackContext);

	return nChunksFound;
}

/******************************************************************************/
/*! SmallBlockAllocator::GetUsableSize

	\brief      [Put your brief description here.]

	\param      pAddress -
	\return              -

	[Put your optional additional description here.]
*/
/******************************************************************************/
size_t SmallBlockAllocator::GetUsableSize(const void* pAddress) const
{
	return mPagePool->NestedCoreBlockGetUsableSize(pAddress);
}

/******************************************************************************/
/*! SmallBlockAllocator::GetAllocationCost

	\brief      get the cost of an allocation

	\param      pAddress -
	\return     nAllocationCostFlags -

		This method is for debugging/statistics gathering only to see how much memory allocations cost (the size returned is NOT useable size)
		See AllocationCost for details on the costs that can be calculated.
		If the address does not belong to this allocator (or no flags are passed in) then 0.0f is returned.

*/
/******************************************************************************/
float SmallBlockAllocator::GetAllocationCost(const void* pAddress, int nAllocationCostFlags ) const
{
	return mPagePool->NestedCoreBlockGetAllocationCost(pAddress, nAllocationCostFlags);
}

/******************************************************************************/
/*! SmallBlockAllocator::CoreBlockOverhead

	\brief      [Put your brief description here.]

	\param       - none.
	\return          -

	[Put your optional additional description here.]
*/
/******************************************************************************/
uint32_t SmallBlockAllocator::CoreBlockOverhead()
{
	return Pool::CoreBlockOverhead();
}

/******************************************************************************/
/*! SmallBlockAllocator::PoolOverhead

	\brief      [Put your brief description here.]

	\param       - none.
	\return          -

	[Put your optional additional description here.]
*/
/******************************************************************************/
uint32_t SmallBlockAllocator::PoolOverhead()
{
	return sizeof(Pool);
}

/******************************************************************************/
/*! SmallBlockAllocator::SetTraceFunction

	\brief      [Put your brief description here.]

	\param      pTraceFunction -
	\param      pContext       -
	\return                    - void.

	[Put your optional additional description here.]
*/
/******************************************************************************/
void SmallBlockAllocator::SetTraceFunction(TraceFunction pTraceFunction, void* pContext)
{
	mCallbacks.mTrace.SetTraceFunction(pTraceFunction, pContext);
}

/******************************************************************************/
/*! SmallBlockAllocator::SetAssertionFailureFunction

	\brief      [Put your brief description here.]

	\param      pAssertionFailureFunction -
	\param      pContext                  -
	\return                               - void.

	[Put your optional additional description here.]
*/
/******************************************************************************/
void SmallBlockAllocator::SetAssertionFailureFunction(AssertionFailureInfoFunction pAssertionFailureFunction, void* pContext)
{
	mCallbacks.SetAssertionFailureFunction(pAssertionFailureFunction, pContext);
}

/******************************************************************************/
/*! SmallBlockAllocator::AssertionFailure

	\brief      [Put your brief description here.]

	\param      pExpression  -
	\param      nErrorId     -
	\param      pDescription -
	\param      pData        -
	\param      pExtra       -
	\return                  - void.

	[Put your optional additional description here.]
*/
/******************************************************************************/
void SmallBlockAllocator::AssertionFailure(const char* pExpression, int nErrorId, const char* pDescription, const void* pData, const void* pExtra) const
{
	AssertionFailureInfo afi;

	afi.mpAllocator        = this;
	afi.mpPool             = NULL;
	afi.mpExpression       = pExpression;
	afi.mnErrorId          = nErrorId;
	afi.mpDescription      = pDescription;
	afi.mpData             = pData;
	afi.mpExtra            = pExtra;

	mCallbacks.AssertionFailure(&afi);
}

/******************************************************************************/
/*! SmallBlockAllocator::ValidateAddress

	\brief      [Put your brief description here.]

	\param      pAddress              -
	\param      kAddressTypeAllocated -
	\return                           -

	[Put your optional additional description here.]
*/
/******************************************************************************/
const void* SmallBlockAllocator::ValidateAddress(const void* pAddress, int addressType) const
{
	if(addressType == kAddressTypeOwned)
	{
		//we don't need to check every pool since if it's in our mPagePool, it's
		//in one of the pools funded by mPagePool
		return ( mPagePool->ValidateAddress(pAddress, kAddressTypeOwned) );
	}

	Pool* pNestedCoreBlockOwner(mPagePool->NestedCoreBlockGetPool(pAddress));
	return ( pNestedCoreBlockOwner ? pNestedCoreBlockOwner->ValidateAddress(pAddress, addressType) : NULL );
}

/******************************************************************************/
/*! Verify

	\brief      [Put your brief description here.]

	\param      pAllocator   -
	\param      bExpression  -
	\param      id           -
	\param      pDescription -
	\param      pData        -
	\param      pLocation    -
	\return                  -

	[Put your optional additional description here.]
*/
/******************************************************************************/
int SmallBlockAllocator::Verify(const EA::Allocator::SmallBlockAllocator* const pAllocator, bool bExpression,
				int id, const char* pDescription, const void* pData, const void* pLocation) const
{
	EA_UNUSED(pAllocator); EA_UNUSED(id); EA_UNUSED(pDescription); EA_UNUSED(pData); EA_UNUSED(pLocation);

	if(bExpression)
		return 0;
	PPM_ASSERT(pAllocator, false, id, pDescription, (void*)pData, (void*)pLocation);
	return 1;
}

/******************************************************************************/
/*! SmallBlockAllocator::Validate

	\brief      [Put your brief description here.]

	\param      pErrorCount -
	\return                 -

	[Put your optional additional description here.]
*/
/******************************************************************************/
bool SmallBlockAllocator::Validate(int* pErrorCount) const
{
	int nErrorCount = 0;

	if (IsInitialized())
	{
		nErrorCount += Verify(this, (mnPoolCount > 0), kGAErrorCorruptedHeap, kGAErrorText[kGAErrorCorruptedHeap], this, NULL);
		nErrorCount += Verify(this, (mPools != NULL), kGAErrorCorruptedHeap, kGAErrorText[kGAErrorCorruptedHeap], this, NULL);

		uint32_t nPoolIndex(0);
		for ( nPoolIndex = 0; (nPoolIndex < mnPoolCount) ; nPoolIndex++ )
			mPools[nPoolIndex].ValidatePool(&nErrorCount);

		nErrorCount += Verify(this, (mPagePool != NULL), kGAErrorCorruptedHeap, kGAErrorText[kGAErrorCorruptedHeap], this, NULL);

		if(mPagePool) // Currently mPagePool should always be valid.
			mPagePool->ValidatePool(&nErrorCount);

		//$$todo, additional validation of member variables
	}
	else
	{
		//verify some member variables are in proper un-initialized states
		nErrorCount += Verify(this, (mnPoolCount == 0), kGAErrorCorruptedHeap, kGAErrorText[kGAErrorCorruptedHeap], this, NULL);
		nErrorCount += Verify(this, (mPagePool == NULL), kGAErrorCorruptedHeap, kGAErrorText[kGAErrorCorruptedHeap], this, NULL);
		nErrorCount += Verify(this, (mPools == NULL), kGAErrorCorruptedHeap, kGAErrorText[kGAErrorCorruptedHeap], this, NULL);
	}

	if ( pErrorCount )
		*pErrorCount += nErrorCount;

	return (nErrorCount == 0); //returns true for error, else false
}

} // namespace Allocator
} // namespace EA


#if defined(EA_COMPILER_MSVC)
	#pragma warning(pop)
#endif

