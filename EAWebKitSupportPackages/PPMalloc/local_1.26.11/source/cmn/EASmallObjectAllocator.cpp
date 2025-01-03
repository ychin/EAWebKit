///////////////////////////////////////////////////////////////////////////////
// EASmallObjectAllocator
//
// Copyright (c) 2007, Electronic Arts. All Rights Reserved.
// Created by Paul Pedriana
//
// This implementation has roots in various small object allocators present 
// at EA, including DynamicSOA, CMemPool, and LionSmallAlloc.
///////////////////////////////////////////////////////////////////////////////


#include <PPMalloc/EASmallObjectAllocator.h>

EA_DISABLE_VC_WARNING(4574)
#include <string.h>
#include <stdio.h>
#include <new>
EA_RESTORE_VC_WARNING()

#if   defined(_MSC_VER)
	#pragma warning(push)
	#pragma warning(disable: 6011) // Dereferencing NULL pointer
#endif


///////////////////////////////////////////////////////////////////////////////
// PPM_VERIFY
// Returns 0 if OK and 1 if false. The purpose is in order to assist
// in making functions that both assert while counting errors.
static int PPM_VERIFY(const EA::Allocator::SmallObjectAllocator* const pAllocator, bool bExpression, 
						int id, const char* pDescription, const void* pData, const void* pLocation)
{
	EA_UNUSED(pAllocator); EA_UNUSED(id); EA_UNUSED(pDescription); EA_UNUSED(pData); EA_UNUSED(pLocation);

	if(bExpression)
		return 0;
	PPM_ASSERT(pAllocator, false, id, pDescription, (void*)pData, (void*)pLocation);
	return 1;
}



namespace EA
{
namespace Allocator
{


inline uintptr_t PPM_GetAlignment(void* p)
{
	return (uintptr_t)(((intptr_t)p ^ ((intptr_t)p - 1)) >> 1) + 1;
}


// PPM_IsolateHighestBit
// How to isolate the highest 1 bit.
// Returns 0 for an input of 0.
// Example:
//    01000100 -> 01000000
//    00000000 -> 00000000
inline uint32_t PPM_IsolateHighestBit(uint32_t x)
{
	x |= x >>  1; 
	x |= x >>  2; 
	x |= x >>  4; 
	x |= x >>  8; 
	x |= x >> 16;

	return x ^ (x >> 1);
}


SmallObjectAllocator::Pool::Pool()
  : mCoreBlockList(),
	mpCoreBlockCurrent(NULL),
	mnChunkSize(0),
	mnChunkAlignment(0),
	mnCoreBlockSize(0),
	mbAutoShrink(true)
{
}


SmallObjectAllocator::Parameters::Parameters()
{
	mnPoolCount      = kDefaultPoolCount;
	mnMinAlignment   = kDefaultMinAlignment;
	mnMaxMallocWaste = kDefaultMaxMallocWaste;
	mnMaxMallocSize  = kDefaultMaxPoolSize;
	mpPoolMemory     = NULL;

	for(uint32_t i = 0, n = kDefaultPoolIncrement; i < kDefaultPoolCount; ++i, n += kDefaultPoolIncrement)
	{
		mnPoolSize[i]      = n;
		mnCoreBlockSize[i] = GetCoreBlockSizeFromCapacity(kDefaultCoreBlockCapacity, n);
	}
}


SmallObjectAllocator::SmallObjectAllocator()
{
	Construct();
}


SmallObjectAllocator::SmallObjectAllocator(const Parameters& parameters, CoreAllocationFunction pAllocationFunction, 
											CoreFreeFunction pFreeFunction, void* pContext)
{
	Construct();
	Init(parameters, pAllocationFunction, pFreeFunction, pContext);
}


void SmallObjectAllocator::Construct()
{
	mPoolArray                        = NULL;
	mnPoolCount                       = 0;
	mbPoolAllocated                   = false;
	mbAlwaysFreeCoreBlocks            = false;
	mbMallocFailureAssertEnabled      = false;
	mnCoreBlockAlignmentMask          = 0;
	mnMaxMallocSize                   = 0;
	mPoolIndexTable                   = NULL;
	mnPoolIndexCountMax               = 0;
	mnMinAlignment                    = 0;
	mnMaxMallocWaste                  = 0;
	mpCoreAllocationFunction          = NULL;
	mpCoreFreeFunction                = NULL;
	mpCoreAllocationContext           = NULL;
	mpAssertionFailureFunction        = &AssertionFailureFunctionDefault;
	mpAssertionFailureFunctionContext = NULL;
	mpTraceFunction                   = &TraceFunctionDefault;
	mpTraceFunctionContext            = NULL;
	mcTraceFieldDelimiter             = (unsigned char)'\t';
	mcTraceRecordDelimiter            = (unsigned char)'\n';

	// It's important (though not critically so) that CoreBlock have a size <= 32 on 32 bit systems, 
	// else it will often be wasting some extra memory (~16 or 32 bytes per CoreBlock).
	#if defined(_MSC_VER) // Some other compilers complain about using offsetof in a compile-time constant expression.
		PPM_CT_ASSERT(((sizeof(void*) == 4) && (EA_OFFSETOF(CoreBlock, mData) <= 32)) || 
					  ((sizeof(void*) == 8) && (EA_OFFSETOF(CoreBlock, mData) <= 64)));
	#endif

	// Verify that mnChunkSize and kMaxPoolSize are in sync.
	PPM_CT_ASSERT(((sizeof(mPoolArray->mpCoreBlockCurrent->mnChunkSize) == 2) && (kMaxPoolSize == 0x0000ffff)) || 
				  ((sizeof(mPoolArray->mpCoreBlockCurrent->mnChunkSize) == 4) && (kMaxPoolSize == 0xffffffff)));

	// Verify that kPoolIndexTableIncrement is OK.
	PPM_CT_ASSERT(kPoolIndexTableIncrement <= kMinPoolSize);
}


SmallObjectAllocator::~SmallObjectAllocator()
{
	Shutdown();
}


bool SmallObjectAllocator::Init(const Parameters& parameters, CoreAllocationFunction pAllocationFunction, 
									CoreFreeFunction pFreeFunction, void* pContext)
{
	PPM_ASSERT(this, mPoolArray == NULL, kGAErrorInvalidInput, kGAErrorText[kGAErrorInvalidInput], NULL, NULL);

	if(pAllocationFunction)
		SetCoreAllocationFunction(pAllocationFunction, pFreeFunction, pContext);

	if(!mPoolArray) // If not already initialized...
	{
		mnPoolCount      = parameters.mnPoolCount;
		mnMaxMallocSize  = parameters.mnPoolSize[parameters.mnPoolCount - 1];
		mnMinAlignment   = parameters.mnMinAlignment;
		mnMaxMallocWaste = parameters.mnMaxMallocWaste;

		PPM_ASSERT(this, (mnPoolCount >= 1) && (mnPoolCount <= kMaxPoolCount),  kGAErrorInvalidInput, kGAErrorText[kGAErrorInvalidInput], NULL, NULL);
		PPM_ASSERT(this, PPM_IsPowerOf2(mnMinAlignment),                        kGAErrorRequestedAlignmentInvalid, kGAErrorText[kGAErrorRequestedAlignmentInvalid], NULL, NULL);

		if(parameters.mpPoolMemory)
		{
			mPoolArray      = (Pool*)parameters.mpPoolMemory;
			mbPoolAllocated = false;
		}
		else
		{
			mPoolArray      = (Pool*)AllocatePoolMemory(mnPoolCount);
			mbPoolAllocated = true;
		}

		PPM_ASSERT(this, mPoolArray != NULL, kGAErrorUninitializedHeap, kGAErrorText[kGAErrorUninitializedHeap], NULL, NULL);

		if(mPoolArray)
		{
			PPM_ASSERT(this, PPM_IsAligned(mPoolArray, kPoolAlignment), kGAErrorInvalidInput, kGAErrorText[kGAErrorInvalidInput], NULL, NULL);

			// Setup mPoolArray
			for(uint32_t i = 0; i < mnPoolCount; ++i)
			{
				new(mPoolArray + i) Pool;

				PPM_ASSERT(this, (parameters.mnPoolSize[i] % kPoolIndexTableIncrement) == 0, kGAErrorInvalidInput, kGAErrorText[kGAErrorInvalidInput], NULL, NULL);
				PPM_ASSERT(this, parameters.mnPoolSize[i] <= kMaxPoolSize, kGAErrorInvalidInput, kGAErrorText[kGAErrorInvalidInput], NULL, NULL);
				PPM_ASSERT(this, (i == 0) || (parameters.mnPoolSize[i] >= parameters.mnPoolSize[i - 1]), kGAErrorInvalidInput, kGAErrorText[kGAErrorInvalidInput], NULL, NULL);

				mPoolArray[i].mnCoreBlockSize = parameters.mnCoreBlockSize[i];
				mPoolArray[i].mnChunkSize     = parameters.mnPoolSize[i];

				if(mPoolArray[i].mnChunkSize < kMinPoolSize)
					mPoolArray[i].mnChunkSize = kMinPoolSize;

				mPoolArray[i].mnChunkAlignment = PPM_IsolateHighestBit(mPoolArray[i].mnChunkSize); // 7, 8, 9, 15, 16, 17, 31, 32, 33 -> 4, 8, 8, 8, 16, 16, 16, 32, 32
			}

			// Setup mPoolIndexTable
			mnPoolIndexCountMax = ((mPoolArray[mnPoolCount - 1].mnChunkSize - 1) / kPoolIndexTableIncrement) + 1;
			mPoolIndexTable     = (uint8_t *)mpCoreAllocationFunction(this, mnPoolIndexCountMax, 4, 0, mpCoreAllocationContext);

			PPM_ASSERT(this, mPoolIndexTable != NULL, kGAErrorUninitializedHeap, kGAErrorText[kGAErrorUninitializedHeap], NULL, NULL);

			if(mPoolIndexTable)
			{
				for(uint32_t j = 0; j < mnPoolCount; j++)
				{
					const uint32_t nPoolIndexBegin = j ? ((mPoolArray[j - 1].mnChunkSize - 1) / kPoolIndexTableIncrement) + 1 : 0;
					const uint32_t nPoolIndexEnd   =     ((mPoolArray[j - 0].mnChunkSize - 1) / kPoolIndexTableIncrement) + 1;

					// Verify that mPoolIndexTable can index the highest memory size. If this fails then there probably isn't much to do other than increase kPoolIndexCountMax (or less ideally: increase kMinAlignment and thus kPoolIndexTableIncrement).
					PPM_ASSERT(this, nPoolIndexEnd <= mnPoolIndexCountMax, kGAErrorInvalidInput, kGAErrorText[kGAErrorInvalidInput], NULL, NULL);

					for(uint32_t k = nPoolIndexBegin; (k < mnPoolIndexCountMax) && (k < nPoolIndexEnd); k++)
						mPoolIndexTable[k] = (uint8_t)j;
				}
			}

			return true;
		}
	}

	return false;
}


bool SmallObjectAllocator::Shutdown()
{
	if(mPoolArray) // If initialized...
	{
		// Possibly trace allocated memory.
		#if (PPM_DEBUG >= 2)
			if(mpTraceFunction)
			{
				char pTraceHeading[128];
				PPM_Snprintf(pTraceHeading, 128, "SmallObjectAllocator shutdown. Outstanding allocations:\n");
				mpTraceFunction(pTraceHeading, mpTraceFunctionContext);
			}

			TraceAllocatedMemory(); // Trace all allocated (i.e. leaked) memory.
		#endif

		for(uint32_t i = 0; i < mnPoolCount; i++)
		{
			Pool* const pPool = mPoolArray + i;

			for(CoreBlock* pCoreBlock = pPool->mCoreBlockList.mpCoreBlockFirst; pCoreBlock; )
			{
				CoreBlock* const pTemp = pCoreBlock;
				pCoreBlock = pCoreBlock->mpNext;
				FreeCoreBlock(pTemp);
			}

			pPool->~Pool();
		}

		if(mbPoolAllocated)
		{
			FreePoolMemory(mPoolArray);
			mbPoolAllocated = false;
		}

		mPoolArray  = NULL;
		mnPoolCount = 0;

		mnCoreBlockAlignmentMask = 0;
	}

	if(mPoolIndexTable)
	{
		FreePoolMemory(mPoolIndexTable);
		mPoolIndexTable = NULL;
		mnPoolIndexCountMax = 0;
	}

	return true;
}


uint32_t SmallObjectAllocator::GetCoreBlockSizeFromCapacity(uint32_t nCoreBlockCapacity, uint32_t nPoolSize)
{
	// This code here is in sync with the code in GrowPool. If you change 
	// code in here or GrowPool, you may need to check the other location.
	// We have a CoreBlock struct with an mData member and the ChunkData is 
	// stored in the mData area and beyond. mData is aligned to the pool aligment.

	const uint32_t nCoreBlockSize = EA_OFFSETOF(CoreBlock, mData) + (nCoreBlockCapacity * nPoolSize);

	return nCoreBlockSize;
}


uint32_t SmallObjectAllocator::GetCoreBlockCapacityFromSize(uint32_t nCoreBlockSize, uint32_t nPoolSize)
{
	// This code here is in sync with the code in GrowPool. If you change 
	// code in here or GrowPool, you may need to check the other location.
	// We have a CoreBlock struct with an mData member and the ChunkData is 
	// stored in the mData area and beyond. mData is aligned to the pool aligment.

	const uint32_t nCoreBlockCapacity = (nCoreBlockSize - EA_OFFSETOF(CoreBlock, mData)) / nPoolSize;

	return nCoreBlockCapacity;
}


bool SmallObjectAllocator::GetParameters(Parameters& parameters)
{
	if(mPoolArray)
	{
		parameters.mnPoolCount      = mnPoolCount;
		parameters.mnMinAlignment   = mnMinAlignment;
		parameters.mnMaxMallocWaste = mnMaxMallocWaste;
		parameters.mnMaxMallocSize  = mnMaxMallocSize;
		parameters.mpPoolMemory     = mPoolArray;

		for(uint32_t i = 0; i < mnPoolCount; ++i)
		{
			parameters.mnPoolSize[i]      = mPoolArray[i].mnChunkSize;
			parameters.mnCoreBlockSize[i] = mPoolArray[i].mnCoreBlockSize;
		}

		return true;
	}

	return false;
}


void SmallObjectAllocator::SetCoreAllocationFunction(CoreAllocationFunction pCoreAllocationFunction, 
													 CoreFreeFunction pCoreFreeFunction, 
													 void* pCoreBlockAllocationContext)
{
	mpCoreAllocationFunction = pCoreAllocationFunction;
	mpCoreFreeFunction       = pCoreFreeFunction;
	mpCoreAllocationContext  = pCoreBlockAllocationContext;
}


void* SmallObjectAllocator::AllocatePoolMemory(uint32_t poolCount)
{
	PPM_ASSERT(this, mpCoreAllocationFunction != NULL, kGAErrorUninitializedHeap, kGAErrorText[kGAErrorUninitializedHeap], NULL, NULL); // If this fails, it is the user's fault.

	void* pPoolMemory = NULL;

	if(mpCoreAllocationFunction)
		pPoolMemory = mpCoreAllocationFunction(this, poolCount * kPoolSize, kPoolAlignment, 0, mpCoreAllocationContext);

	return pPoolMemory;
}


SmallObjectAllocator::CoreBlock* SmallObjectAllocator::AllocateCoreBlock(uint32_t nSize, uint32_t nMinChunkAlignment)
{
	CoreBlock* pCoreBlock = NULL;

	if(mpCoreAllocationFunction)
	{
		const uint32_t nAlignment = PPM_MAX(nMinChunkAlignment, kMinCoreBlockDataAlignment);

		pCoreBlock = (CoreBlock*)mpCoreAllocationFunction(this, nSize, nAlignment, EA_OFFSETOF(CoreBlock, mData), mpCoreAllocationContext);
	}

	return pCoreBlock;
}


void* SmallObjectAllocator::Malloc(size_t nSize)
{
	if(nSize <= mnMaxMallocSize) // This should usually be true.
	{
		Top:
		size_t      poolIndex  = mPoolIndexTable[((int32_t)nSize - 1) / kPoolIndexTableIncrement]; // This is a power-of-two divide and thus arithmetic right shift.
		Pool*       pPool      = mPoolArray + poolIndex;
		CoreBlock*  pCoreBlock = pPool->mpCoreBlockCurrent;

		// Verify that pPool is correctly and optimally chosen.
		PPM_ASSERT(this, (nSize <= pPool->mnChunkSize) && ((poolIndex == 0) || (nSize > pPool[-1].mnChunkSize)), kGAErrorInternal, kGAErrorText[kGAErrorInternal], NULL, NULL);

		if(pCoreBlock) // This will usually be true. If not then it means we have no core blocks with free memory.
		{
			Success:
			PPM_ASSERT(this, (pCoreBlock->mpChunkList != NULL) && (pCoreBlock->mnFreeChunkCount > 0), kGAErrorInternal, kGAErrorText[kGAErrorInternal], NULL, NULL);
			PPM_ASSERT(this, pCoreBlock->mpChunkList == pCoreBlock->GetChunk(pCoreBlock->mpChunkList), kGAErrorBadUserPtr, kGAErrorText[kGAErrorBadUserPtr], NULL, NULL);
			#if (PPM_DEBUG >= 2)
				if(PPM_IsPowerOf2(pPool->mnChunkSize))
					PPM_ASSERT(this, PPM_IsAligned(pCoreBlock->mpChunkList, pPool->mnChunkAlignment), kGAErrorInvalidInput, kGAErrorText[kGAErrorInvalidInput], NULL, NULL);
			#endif

			Chunk* const pChunk = pCoreBlock->mpChunkList;
			PPM_ASSERT(this, pChunk != NULL, kGAErrorInternal, kGAErrorText[kGAErrorInternal], NULL, NULL);

			pCoreBlock->mnFreeChunkCount--;
			pCoreBlock->mpChunkList = pChunk->mpNext;
			pPool->mCoreBlockList.AdjustCoreBlockPositionMalloc(pCoreBlock);

			if(pCoreBlock->mpChunkList) // If there are more Chunks in the list (this will usually be true)...
				return pChunk;

			do    (pPool->mpCoreBlockCurrent =   pPool->mpCoreBlockCurrent->mpPrev);
			while (pPool->mpCoreBlockCurrent && !pPool->mpCoreBlockCurrent->mpChunkList);

			return pChunk;
		}

		// This is the uncommon pathway, so speed isn't quite as important.
		// Need to find a new Pool or allocate a new CoreBlock.
		const Pool* const pPoolEnd = mPoolArray + mnPoolCount;

		while((pPool < pPoolEnd) && (pPool->mnChunkSize < (nSize + mnMaxMallocWaste)))
		{
			if(pPool->mpCoreBlockCurrent)
			{
				pCoreBlock = pPool->mpCoreBlockCurrent;
				goto Success; // We use an upward goto like this because the code above is optimized for the most common success pathway.
			}

			++pPool;
		}

		// At this point there is no existing memory that can fund this allocation from any Pool.
		// We need to allocate a new CoreBlock and add it to the current Pool.
		if(GrowPool(mPoolArray + poolIndex))
			goto Top; // We use an upward goto like this because the code above is optimized for the most common success pathway.
	}

	// Verify that the user has called Init.
	PPM_ASSERT(this, mPoolArray, kGAErrorUninitializedHeap, kGAErrorText[kGAErrorUninitializedHeap], NULL, NULL);

	// This function has failed to allocate memory after all attempts.
	#if PPM_ASSERT_ENABLED
		if(mbMallocFailureAssertEnabled)
			PPM_ASSERT(this, false, kGAErrorMemoryExhaustion, kGAErrorText[kGAErrorMemoryExhaustion], NULL, NULL);
	#endif

	return NULL;
}


// We locate this next to Malloc in because these are the most frequently used functions.
void SmallObjectAllocator::Free(void* p, size_t nSize)
{
	#if PPM_NULL_POINTER_FREE_ENABLED
	if(p)
	{
	#endif
		// To consider: If we want support freeing of pointers that aren't directly 
		// on Chunk boundaries, we'll need to use the CoreBlock::GetChunk function.

		Pool*      pPool;
		CoreBlock* pCoreBlock;
		Chunk*     pChunk = (Chunk*)p;

		if(mnCoreBlockAlignmentMask) // If all core block memory comes from identically aligned pages...
		{
			#if (PPM_DEBUG >= 2)
				PPM_ASSERT(this, ValidateAddress(p, kAddressTypeSpecific) == pChunk, kGAErrorBadUserPtr, kGAErrorText[kGAErrorBadUserPtr], NULL, NULL);
			#elif (PPM_DEBUG >= 1)
				PPM_ASSERT(this, GetAllocationInfo(p, nSize, pPool, pCoreBlock) == pChunk, kGAErrorBadUserPtr, kGAErrorText[kGAErrorBadUserPtr], NULL, NULL);
			#endif

			pCoreBlock = (CoreBlock*)((uintptr_t)p & mnCoreBlockAlignmentMask);
			pPool      = pCoreBlock->mpPool;
		}
		else
		{
			// The implementation here loops through the pools in order to find the CoreBlock that the 
			// pointer p belongs to. This executes in O(n) time where n is the total number of CoreBlocks that
			// exist amonst the Pools. This function could execute significantly faster -- in O(1) time --
			// if all CoreBlocks are of the same alignment and fit within a page of that alignemnt. 
			// In that case we can directly convert p to its corresponding CoreBlock by simply masking off
			// the appropriate small bits of p. See above for the handling of this latter option.

			// If the user supplies an nSize value, this lookup can proceed significantly faster,
			// as poolIndex will likely point to the pool we are looking for. Otherwise this 
			// function does a linear search through the pools to see which one owns the pointer.
			const uint32_t poolIndex = (nSize ? (uint32_t)mPoolIndexTable[((int32_t)nSize - 1) / kPoolIndexTableIncrement] : (uint32_t)0); // This is a power-of-two divide.
			Pool* pPoolEnd;

			for(pPool = mPoolArray + poolIndex, pPoolEnd = mPoolArray + mnPoolCount; pPool != pPoolEnd; ++pPool)
			{
				// Ideally there should be only one or two CoreBlocks per pool here.
				for(pCoreBlock = pPool->mCoreBlockList.mpCoreBlockFirst; pCoreBlock; pCoreBlock = pCoreBlock->mpNext)
				{
					if(pCoreBlock->Contains(p))
					{
						PPM_ASSERT(this, pChunk == pCoreBlock->GetChunk(p), kGAErrorBadUserPtr, kGAErrorText[kGAErrorBadUserPtr], NULL, NULL);
						goto Success; // Until C++ supports directed breaks, goto remains the most efficient means here.
					}
				}
			}

			// The pointer is not recognized.
			PPM_ASSERT(this, false, kGAErrorBadUserPtr, kGAErrorText[kGAErrorBadUserPtr], NULL, NULL);
			return;
		}

		Success:
		#if (PPM_DEBUG >= 2)
			PPM_ASSERT(this, !pCoreBlock->IsChunkFree(pChunk), kGAErrorBadUserPtr, kGAErrorText[kGAErrorBadUserPtr], NULL, NULL);

			if(PPM_IsPowerOf2(pPool->mnChunkSize))
				PPM_ASSERT(this, PPM_IsAligned(pChunk, pPool->mnChunkAlignment), kGAErrorInvalidInput, kGAErrorText[kGAErrorInvalidInput], NULL, NULL);
		#endif

		// Verify that all free blocks are aligned as per the Pool's specification.
		// PPM_ASSERT(this, PPM_IsAligned(pChunk, kMinAlignment), kGAErrorInternal, kGAErrorText[kGAErrorInternal], NULL, NULL);

		pChunk->mpNext = pCoreBlock->mpChunkList;
		pCoreBlock->mpChunkList = pChunk;
		pCoreBlock->mnFreeChunkCount++;

		// Here we consider the possibility of freeing pCoreBlock.
		// We free CoreBlocks here only if mbAutoShrink is enabled and if the CoreBlock is completely free.
		// Don't shrink if pCoreBlock happens to be the current CoreBlock we're making allocations
		// from in order to try to avoid the edge case where we repeatedly shrink then re-grow.

		const bool bCoreBlockIsCurrent  = (pCoreBlock == pPool->mpCoreBlockCurrent);
		const bool bCoreBlockIsUnused   = (pCoreBlock->mnChunkCount == pCoreBlock->mnFreeChunkCount);
		const bool bShouldFreeCoreBlock = bCoreBlockIsUnused && pPool->mbAutoShrink && (mbAlwaysFreeCoreBlocks || !bCoreBlockIsCurrent);

		if(bShouldFreeCoreBlock)
		{
			pPool->mCoreBlockList.Remove(pCoreBlock);
			FreeCoreBlock(pCoreBlock);

			if(bCoreBlockIsCurrent)  // If we just removed mpCoreBlockCurrent... find a new value for mpCoreBlockCurrent.
			{
				pPool->mpCoreBlockCurrent = pPool->mCoreBlockList.mpCoreBlockFirst;

				if(pPool->mpCoreBlockCurrent)
				{
					while(pPool->mpCoreBlockCurrent->mpNext && (pPool->mpCoreBlockCurrent->mpNext->mnFreeChunkCount > 0))
						  pPool->mpCoreBlockCurrent = pPool->mpCoreBlockCurrent->mpNext;

					if(!pPool->mpCoreBlockCurrent->mnFreeChunkCount)
						pPool->mpCoreBlockCurrent = NULL;
				}
			}
		}
		else
		{
			// Keep the list sorted.
			pPool->mCoreBlockList.AdjustCoreBlockPositionFree(pCoreBlock);

			// If there was no valid alloc chunk, this one now has space to alloc from.
			if(!pPool->mpCoreBlockCurrent)
				pPool->mpCoreBlockCurrent = pCoreBlock;

			// Set mpCoreBlockCurrent to the CoreBlock that has the least number of free Chunks (that are still greater than zero).
			// Recall that the CoreBlock list is sorted from most free to least free.
			while(pPool->mpCoreBlockCurrent->mpNext && (pPool->mpCoreBlockCurrent->mpNext->mnFreeChunkCount > 0))
				  pPool->mpCoreBlockCurrent = pPool->mpCoreBlockCurrent->mpNext;

			#if PPM_DEBUG_FILL && (PPM_DEBUG >= 2)
				// Do guard fill of Chunk memory after pChunk->mpNext.
				uint8_t* const pMemBegin = (uint8_t*)(&pChunk->mpNext + 1);
				uint8_t* const pMemEnd   = (uint8_t*)pChunk + pCoreBlock->mnChunkSize;
				DebugMemFill(pMemBegin, kDefaultFillValueFree, (size_t)(pMemEnd - pMemBegin));
			#endif
		}

		// mpCoreBlockCurrent should only point to a CoreBlock that has free space.
		// mpCoreBlockCurrent should only ever be NULL if the CoreBlockList has no free space (is empty or all its CoreBlocks are fully used).
		#if PPM_ASSERT_ENABLED
			if(pPool->mpCoreBlockCurrent)
				PPM_ASSERT(this, (pPool->mpCoreBlockCurrent->mpChunkList) && (pPool->mpCoreBlockCurrent->mnFreeChunkCount), kGAErrorBadUserPtr, kGAErrorText[kGAErrorInternal], NULL, NULL);
			else
			{
				bool bFreeSpaceExists = false;

				for(pCoreBlock = pPool->mCoreBlockList.mpCoreBlockFirst; pCoreBlock && !bFreeSpaceExists; pCoreBlock = pCoreBlock->mpNext)
				{
					if(pCoreBlock->mnFreeChunkCount)
						bFreeSpaceExists = false;
				}

				PPM_ASSERT(this, !bFreeSpaceExists, kGAErrorBadUserPtr, kGAErrorText[kGAErrorInternal], NULL, NULL);
			}
		#endif

	#if PPM_NULL_POINTER_FREE_ENABLED
	}
	#endif
}


void SmallObjectAllocator::CoreBlockList::AdjustCoreBlockPositionFree(CoreBlock* pCoreBlock)
{
	CoreBlock* pTemp = pCoreBlock;

	// Free blocks just increased so we can assume they are greater than 0.
	// PPM_ASSERT(this, pTemp->mnFreeChunkCount > 0, kGAErrorInternal, kGAErrorText[kGAErrorInternal], NULL, NULL);

	while(EA_UNLIKELY(pTemp->mpPrev && (
			// CoreBlocks of an equal block count should be ordered based on the number of 
			// remaining free blocks figuring that those with less current free blocks are 
			// less likely to become completely free.
			((pTemp->mnChunkCount == pTemp->mpPrev->mnChunkCount) && (pTemp->mnFreeChunkCount >= pTemp->mpPrev->mnFreeChunkCount)) ||

			// pCoreBlock should be moved in front of any that have 0 free blocks 
			// available since we want full pCoreBlocks at the end of the list.
			!pTemp->mpPrev->mnFreeChunkCount ||

			// pCoreBlock should be in front of any CoreBlocks that have a larger total 
			// block count since it seems likely that those with larger block counts are less 
			// likely to ever be completely freed.
			(pTemp->mnChunkCount < pTemp->mpPrev->mnChunkCount))))
	{
		pTemp = pTemp->mpPrev;
	}

	if(EA_UNLIKELY(pTemp != pCoreBlock))
	{
		Remove(pCoreBlock);
		InsertBefore(pCoreBlock, pTemp);
	}
}


SmallObjectAllocator::CoreBlock* SmallObjectAllocator::GrowPool(Pool* pPool)
{
	const uint32_t nSize      = pPool->mnCoreBlockSize;
	CoreBlock*     pCoreBlock = AllocateCoreBlock(nSize, pPool->mnChunkAlignment);

	if(pCoreBlock)
	{
		PPM_ASSERT(this, nSize >= sizeof(CoreBlock), kGAErrorInternal, kGAErrorText[kGAErrorInternal], NULL, NULL);

		// Maintain CoreBlock alignment statistics. We do this because it turns out that
		// if all CoreBlocks are equivalently aligned and fit entirely within one page of
		// their alignment, we can take advantage of knowing this and make some operations faster.
		if(mnCoreBlockAlignmentMask)
		{
			const uintptr_t nObservedAlignment     = PPM_GetAlignment(pCoreBlock);
			const uintptr_t nObservedAlignmentMask = ~(nObservedAlignment - 1);
	
			// Verify that if the user specified paged CoreBlocks, the allocated CoreBlocks are in fact appropriately page sized and aligned.
			PPM_ASSERT(this, (nSize <= nObservedAlignment) && (nObservedAlignmentMask <= mnCoreBlockAlignmentMask), kGAErrorInvalidInput, kGAErrorText[kGAErrorInvalidInput], NULL, NULL);

			if((nSize > nObservedAlignment) || (nObservedAlignmentMask > mnCoreBlockAlignmentMask))
				mnCoreBlockAlignmentMask = 0;
		}

		// Set up the CoreBlock.
		pCoreBlock->mnSize           = nSize;
		pCoreBlock->mnChunkSize      = (uint16_t)pPool->mnChunkSize;
		pCoreBlock->mnChunkCount     = 0;
		pCoreBlock->mnFreeChunkCount = 0;
		pCoreBlock->mpChunkList      = NULL;
		pCoreBlock->mpPool           = pPool;

		// Fill the CoreBlock with debug fill
		#if PPM_DEBUG_FILL && (PPM_DEBUG >= 2)
			DebugMemFill(pCoreBlock->mData, kDefaultFillValueFree, (size_t)((uint8_t*)pCoreBlock + pCoreBlock->mnSize - pCoreBlock->mData));
		#endif

		// Divide the CoreBlock into Chunks.
		Chunk* pStart = (Chunk*)PPM_AlignUp(pCoreBlock->mData, pPool->mnChunkAlignment); // To consider: For non-power-of-two mnChunkSize, this may not need specialized alignment, though it does help with user-request MallocAligned requests.
		Chunk* pEnd   = (Chunk*)((uintptr_t)pStart + ((((uintptr_t)pCoreBlock + pCoreBlock->mnSize) - (uintptr_t)pStart) / pPool->mnChunkSize * pPool->mnChunkSize));

		PPM_ASSERT(this, pStart < pEnd, kGAErrorInternal, kGAErrorText[kGAErrorInternal], NULL, NULL);
		PPM_ASSERT(this, PPM_IsAligned(pStart, pPool->mnChunkAlignment), kGAErrorInternal, kGAErrorText[kGAErrorInternal], NULL, NULL);
		PPM_ASSERT(this, ((uintptr_t)pStart + pPool->mnChunkSize) <= ((uintptr_t)pCoreBlock + pCoreBlock->mnSize), kGAErrorInternal, kGAErrorText[kGAErrorInternal], NULL, NULL);

		if(pStart < pEnd) // If there is any room... (Usually there will enough room unless the user specifies a very small core size).
		{
			pCoreBlock->mpChunkData  = pStart;
			pCoreBlock->mpChunkList  = pStart;
			pCoreBlock->mnChunkCount = (uint16_t)(((intptr_t)pEnd - (intptr_t)pStart) / pPool->mnChunkSize);

			PPM_ASSERT(this, pStart == pCoreBlock->GetChunk(pStart), kGAErrorInternal, kGAErrorText[kGAErrorInternal], NULL, NULL);

			for(uint32_t i = 0; i < pCoreBlock->mnChunkCount; i++)
			{
				PPM_ASSERT(this, ((uintptr_t)pStart + pPool->mnChunkSize) <= ((uintptr_t)pCoreBlock + pCoreBlock->mnSize), kGAErrorInternal, kGAErrorText[kGAErrorInternal], NULL, NULL);
				PPM_ASSERT(this, PPM_IsAligned(pStart, kMinAlignment),  kGAErrorInternal, kGAErrorText[kGAErrorInternal], NULL, NULL);
				#if (PPM_DEBUG >= 1) && PPM_ASSERT_ENABLED
					if(PPM_IsPowerOf2(pPool->mnChunkSize))
						PPM_ASSERT(this, PPM_IsAligned(pStart, pPool->mnChunkAlignment), kGAErrorInvalidInput, kGAErrorText[kGAErrorInvalidInput], NULL, NULL);
				#endif
				PPM_ASSERT(this, pStart == pCoreBlock->GetChunk(pStart), kGAErrorInternal, kGAErrorText[kGAErrorInternal], NULL, NULL);

				if((i + 1) < pCoreBlock->mnChunkCount) // If there are more loops to do ...
				{
					Chunk* const pNext = (Chunk*)((uintptr_t)pStart + pPool->mnChunkSize);
					pStart->mpNext = pNext;
					pStart = pNext;
				}
				else
					pStart->mpNext = NULL;
			}

			pCoreBlock->mnFreeChunkCount = pCoreBlock->mnChunkCount;
			pPool->mCoreBlockList.PushFront(pCoreBlock);
			pPool->mpCoreBlockCurrent = pCoreBlock;
		}
		else
		{
			// There wasn't any room in the CoreBlock for user data. Either we have a bug here or the user needs to specify a higher Parameters::mnCoreBlockSize.
			FreeCoreBlock(pCoreBlock);  
			pCoreBlock = NULL;
		}
	}

	return pCoreBlock;
}


size_t SmallObjectAllocator::GetUsableSize(const void* p, size_t nSize) const
{
	Pool*      pPool = NULL;
	CoreBlock* pCoreBlock = NULL;

	if(GetAllocationInfo(p, nSize, pPool, pCoreBlock))
		return pCoreBlock->mnChunkSize;
	
	return (size_t)-1;
}


const SmallObjectAllocator::Chunk* SmallObjectAllocator::GetAllocationInfo(const void* p, size_t nSize, Pool*& pPool, CoreBlock*& pCoreBlock) const
{
	if(mnCoreBlockAlignmentMask) // If all core block memory comes from identically aligned pages...
	{
		// This pathway is O(1) with respect to the number of managed CoreBlocks and the size of the heap.
		#if (PPM_DEBUG >= 2)
			PPM_ASSERT(this, ValidateAddress(p, kAddressTypeSpecific) == p, kGAErrorBadUserPtr, kGAErrorText[kGAErrorBadUserPtr], NULL, NULL);
		#endif

		pCoreBlock = (CoreBlock*)((uintptr_t)p & mnCoreBlockAlignmentMask);
		pPool      = pCoreBlock->mpPool;

		return (Chunk*)p;
	}
	else
	{
		// This pathway is O(n) with respect to the number of managed CoreBlocks (and thus also the size of the heap).
		const uint32_t poolIndex = (nSize ? (uint32_t)mPoolIndexTable[((int32_t)nSize - 1) / kPoolIndexTableIncrement] : (uint32_t)0); // This is a power-of-two divide.
		Pool* pPoolEnd;

		for(pPool = mPoolArray + poolIndex, pPoolEnd = mPoolArray + mnPoolCount; pPool != pPoolEnd; ++pPool)
		{
			// Ideally there should be only one or two CoreBlocks per pool here.
			for(pCoreBlock = pPool->mCoreBlockList.mpCoreBlockFirst; pCoreBlock; pCoreBlock = pCoreBlock->mpNext)
			{
				if(pCoreBlock->Contains(p))
				{
					Chunk* const pChunk = pCoreBlock->GetChunk(p);

					return pChunk;
				}
			}
		}
	}

	return NULL;
}


void* SmallObjectAllocator::MallocAligned(size_t nSize, size_t nAlignment)
{
	if(nSize <= mnMaxMallocSize) // This should usually be true.
	{
		int growCount = 0;

		if(nAlignment < kMinAlignment)
			nAlignment = kMinAlignment;

		// Currently we take the appoach that we linearly scan the available Chunks 
		// for one that is appropriately aligned. Another approach that we can use 
		// additionally or instead is to use a larger Chunk than nSize and return 
		// a pointer to within the Chunk instead of a pointer to the front of the Chunk.
		// However, this requires the Free function to be able to deal with such pointers.

		Top:
		size_t      poolIndex  = mPoolIndexTable[((int32_t)nSize - 1) / kPoolIndexTableIncrement]; // This is a power-of-two divide and thus arithmetic right shift.
		Pool*       pPool      = mPoolArray + poolIndex;
		CoreBlock*  pCoreBlock = pPool->mpCoreBlockCurrent;

		Loop:
		// Verify that no CoreBlock after pCoreBlock has any free space. ** Temporarily disabled while we track down a bug. **
		// PPM_ASSERT(this, !pCoreBlock || !pCoreBlock->mpNext || (pCoreBlock->mpNext->mnFreeChunkCount == 0), kGAErrorInternal, kGAErrorText[kGAErrorInternal], NULL, NULL);
		// Disabled because it seems to be triggering, though it really shouldn't be. It's not a serious failure if it does trigger, though.
		// PPM_ASSERT(this, !pCoreBlock || !pCoreBlock->mpNext || (pCoreBlock->mpNext->mnFreeChunkCount <= pCoreBlock->mnFreeChunkCount), kGAErrorInternal, kGAErrorText[kGAErrorInternal], NULL, NULL);

		while(pCoreBlock)
		{
			// Verify that pCoreBlock has free space.
			PPM_ASSERT(this, (pCoreBlock->mpChunkList != NULL) && (pCoreBlock->mnFreeChunkCount > 0), kGAErrorInternal, kGAErrorText[kGAErrorInternal], NULL, NULL);

			// Walk through the Chunks of the CoreBlock till we find one that matches the desired alignment.
			for(Chunk* pChunk = pCoreBlock->mpChunkList, *pChunkPrev = NULL; pChunk; pChunkPrev = pChunk, pChunk = pChunk->mpNext)
			{
				if(((uintptr_t)pChunk & (nAlignment - 1)) == 0) // If aligned appropriately...
				{
					pCoreBlock->RemoveChunk(pChunk, pChunkPrev);
					pCoreBlock->mnFreeChunkCount--;
					pPool->mCoreBlockList.AdjustCoreBlockPositionMalloc(pCoreBlock);

					if(pCoreBlock->mpChunkList) // If there are more Chunks in the list...
						return pChunk;  // This should be the most common return pathway, and it should be the fastest return pathway.

					do    (pPool->mpCoreBlockCurrent =   pPool->mpCoreBlockCurrent->mpPrev);
					while (pPool->mpCoreBlockCurrent && !pPool->mpCoreBlockCurrent->mpChunkList);

					return pChunk;
				}
			}

			pCoreBlock = pCoreBlock->mpPrev;
		}

		// This is the uncommon pathway, so speed isn't quite as important.
		// Need to find a new Pool or allocate a new CoreBlock.
		if(mnMaxMallocWaste >= nAlignment)
		{
			++pPool;

			for(const Pool* const pPoolEnd = mPoolArray + mnPoolCount; 
				(pPool < pPoolEnd) && (pPool->mnChunkSize < (nSize + mnMaxMallocWaste)); 
				pPool++)
			{
				if(pPool->mpCoreBlockCurrent)
				{
					pCoreBlock = pPool->mpCoreBlockCurrent;
					goto Loop; // We use an upward goto like this because the code above is optimized for the most common success pathway.
				}
			}
		}

		// Should we grow the pool here? What if the required alignment is such that 
		// growing the pool won't help the situation but instead will needlessly waste
		// the pool space? Currently we implement a small '*8' heuristic.
		if((growCount++ == 0) && ((mPoolArray[poolIndex].mnChunkSize * 8) >= nAlignment) && ((mnMaxMallocSize >= nAlignment * 2))) // If it's possible to fund such an allocation with the kind of memory we allocate in this pool...
		{
			// At this point there is no existing memory that can fund this allocation from any Pool.
			// We need to allocate a new CoreBlock and add it to the current Pool.
			if(GrowPool(mPoolArray + poolIndex))
				goto Top; // We use an upward goto like this because the code above is optimized for the most common success pathway.
		}
	}

	// Verify that the user has called Init.
	PPM_ASSERT(this, mPoolArray, kGAErrorUninitializedHeap, kGAErrorText[kGAErrorUninitializedHeap], NULL, NULL);

	// This function has failed to allocate memory after all attempts.
	#if PPM_ASSERT_ENABLED
		if(mbMallocFailureAssertEnabled)
			PPM_ASSERT(this, false, kGAErrorMemoryExhaustion, kGAErrorText[kGAErrorMemoryExhaustion], NULL, NULL);
	#endif

	return NULL;
}


void SmallObjectAllocator::CoreBlockList::AdjustCoreBlockPositionMalloc(CoreBlock* pCoreBlock)
{
	CoreBlock* pTemp = pCoreBlock;

	// mnFreeChunkCount has increased so we can assume they are greater than 0.
	// PPM_ASSERT(this, pTemp->mnFreeChunkCount > 0, kGAErrorInternal, kGAErrorText[kGAErrorInternal], NULL, NULL);

	while(EA_UNLIKELY(pTemp->mpNext && (pCoreBlock->mnFreeChunkCount < pTemp->mpNext->mnFreeChunkCount)))
		pTemp = pTemp->mpNext;

	if(EA_UNLIKELY(pTemp != pCoreBlock))
	{
		Remove(pCoreBlock);
		InsertAfter(pCoreBlock, pTemp);
	}
}


void* SmallObjectAllocator::Realloc(void* pOld, size_t nNewSize, size_t nOldSize)
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
		Pool*        pPool      = NULL;
		CoreBlock*   pCoreBlock = NULL;
		const Chunk* pChunk     = GetAllocationInfo(pOld, nOldSize, pPool, pCoreBlock);  

		// If we can make the realloc a no-op, do so.
		if(pChunk && (nNewSize < pPool->mnChunkSize) && ((int32_t)nNewSize > (int32_t)(pPool->mnChunkSize - mnMaxMallocWaste)))
			return pOld;

		// The user must either specify an old size or pOld must be a valid pointer for us. Otherwise we cannot know how to copy the old data.
		PPM_ASSERT(this, (pChunk != NULL) || (nOldSize == 0), kGAErrorBadUserPtr, kGAErrorText[kGAErrorBadUserPtr], NULL, NULL);

		void* const pNew = Malloc(nNewSize);

		if(pNew)
		{
			if(pChunk)
				nOldSize = pPool->mnChunkSize;

			const size_t nCopySize = (nOldSize < nNewSize) ? nOldSize : nNewSize;
			memcpy(pNew, pOld, nCopySize);

			if(pChunk)
				Free(pOld, pPool->mnChunkSize);
		 }

		 return pNew;
	}

	// For old pointer of NULL, C89 and C99 standards call for same behaviour as malloc.
	return Malloc(nNewSize);
}


void SmallObjectAllocator::SetOption(int option, int nValue)
{
	switch (option)
	{
		case kOptionMaxChunkWaste:
			mnMaxMallocWaste = (uint32_t)(unsigned)nValue;
			break;

		//case kOptionGuardFill:
		//    // This is not currently implemented. 
		//    break;

		case kOptionPagedCoreBlockSize:
		{
			PPM_ASSERT(this, PPM_IsPowerOf2((unsigned)nValue), kGAErrorInvalidInput, kGAErrorText[kGAErrorInvalidInput], NULL, NULL);
			mnCoreBlockAlignmentMask = ~((uintptr_t)(unsigned)nValue - 1);
			break;
		}

		case kOptionAutoShrinkAll:
			for(uint32_t i = 0; i < mnPoolCount; i++)
				mPoolArray[i].mbAutoShrink = (nValue != 0);
			break;

		case kOptionAlwaysFreeCoreBlocks:
			mbAlwaysFreeCoreBlocks = (nValue != 0);
			break;

		case kOptionEnableMallocFailureAssert:
			mbMallocFailureAssertEnabled = (nValue != 0);
			break;

		default:
		{
			const unsigned index = (unsigned)option - (unsigned)kOptionAutoShrinkPoolBegin;
			const unsigned limit = (unsigned)(kOptionAutoShrinkPoolBegin + mnPoolCount);

			PPM_ASSERT(this, index < limit, kGAErrorInvalidInput, kGAErrorText[kGAErrorInvalidInput], NULL, NULL);
			if(index < limit)
				mPoolArray[option - kOptionAutoShrinkPoolBegin].mbAutoShrink = (nValue != 0);
			break;
		}
	}
}


int SmallObjectAllocator::GetOption(int option) const
{
	switch (option)
	{
		case kOptionMaxChunkWaste:
			return (int)mnMaxMallocWaste;

		//case kOptionGuardFill:
		//    // This is not currently implemented. 
		//    break;

		case kOptionPagedCoreBlockSize:
			return (int)(~mnCoreBlockAlignmentMask + 1);

		case kOptionAutoShrinkAll:
			return (mPoolArray[0].mbAutoShrink ? 1 : 0); // This isn't correct; we need to test for all of them.

		case kOptionAlwaysFreeCoreBlocks:
			return (mbAlwaysFreeCoreBlocks ? 1 : 0);

		case kOptionEnableMallocFailureAssert:
			return (mbMallocFailureAssertEnabled ? 1 : 0);

		default:
			if(((unsigned)option - (unsigned)kOptionAutoShrinkPoolBegin) < (unsigned)(kOptionAutoShrinkPoolBegin + mnPoolCount))
				return (mPoolArray[option - kOptionAutoShrinkPoolBegin].mbAutoShrink ? 1 : 0);
	}

	return 0;
}


void SmallObjectAllocator::SetAssertionFailureFunction(AssertionFailureInfoFunction pAssertionFailureFunction, void* pContext)
{
	mpAssertionFailureFunction        = pAssertionFailureFunction;
	mpAssertionFailureFunctionContext = pContext;
}


void SmallObjectAllocator::AssertionFailure(const char* pExpression, int nErrorId, const char* pDescription, 
											const void* pData, const void* pExtra) const
{
	if(mpAssertionFailureFunction)
	{
		AssertionFailureInfo afi;

		afi.mpAllocator        = this;
		afi.mpExpression       = pExpression;
		afi.mnErrorId          = nErrorId;
		afi.mpDescription      = pDescription;
		afi.mpData             = pData;
		afi.mpExtra            = pExtra;

		mpAssertionFailureFunction(&afi, mpAssertionFailureFunctionContext);
	}
}


void SmallObjectAllocator::AssertionFailureFunctionDefault(AssertionFailureInfo* pAssertionFailureInfo, void* pContext)
{
	TraceFunctionDefault(pAssertionFailureInfo->mpDescription, pContext);
	TraceFunctionDefault("\n", pContext);
	PPM_DebugBreak();
}


void SmallObjectAllocator::SetTraceFunction(TraceFunction pTraceFunction, void* pContext)
{
	mpTraceFunction        = pTraceFunction;
	mpTraceFunctionContext = pContext;
}


void SmallObjectAllocator::TraceAllocatedMemory(TraceFunction pTraceFunction, void* pTraceFunctionContext, int nBlockTypeFlags)
{
	char pTraceBuffer[PPM_TRACE_BUFFER_SIZE];

	if(!pTraceFunction)
		 pTraceFunction = mpTraceFunction;

	if(!pTraceFunctionContext)
		 pTraceFunctionContext = mpTraceFunctionContext;

	if(pTraceFunction)
	{
		ReportContext reportContext;

		if(ReportBegin(&reportContext, nBlockTypeFlags))
		{
			for(const BlockInfo* pBlockInfo = ReportNext(&reportContext); 
								 pBlockInfo; 
								 pBlockInfo = ReportNext(&reportContext))
			{
				// To consider: Add more descriptive information that done here.

				// Write the "addr" field.
				#if defined(EA_PLATFORM_PTR_SIZE) && (EA_PLATFORM_PTR_SIZE == 8)
					PPM_Snprintf(pTraceBuffer, PPM_TRACE_BUFFER_SIZE, "addr: 0x%016" PRIx64 "%c", (uint64_t)(uintptr_t)pBlockInfo->mpData, mcTraceFieldDelimiter);
				#else
					PPM_Snprintf(pTraceBuffer, PPM_TRACE_BUFFER_SIZE, "addr: 0x%08x%c", (unsigned)(uintptr_t)pBlockInfo->mpData, mcTraceFieldDelimiter);
				#endif

				pTraceFunction(pTraceBuffer, pTraceFunctionContext);
			}

			ReportEnd(&reportContext);
		}
	}
}


const void* SmallObjectAllocator::ValidateAddress(const void* pAddress, int addressType) const
{
	// We have little choice but to simply ask each pool if it recognizes the address.

	for(Pool* pPool = mPoolArray, *pPoolEnd = mPoolArray + mnPoolCount; pPool != pPoolEnd; ++pPool)
	{
		// Ideally there should be only one or two CoreBlocks per pool here.
		for(CoreBlock* pCoreBlock = pPool->mCoreBlockList.mpCoreBlockFirst; pCoreBlock; pCoreBlock = pCoreBlock->mpNext)
		{
			if(pCoreBlock->Contains(pAddress))
			{
				if(addressType == kAddressTypeOwned)
					return pAddress;

				Chunk* const pChunk = pCoreBlock->GetChunk(pAddress);

				if(!pCoreBlock->IsChunkFree(pChunk)) // Unfortunately, this can be a slow function call.
				{
					if(addressType == kAddressTypeAllocated)
						return pChunk;

					// addressType == kAddressTypeSpecific
					if((const void*)pChunk == pAddress)
						return pChunk;
				}

				return NULL;
			}
		}
	}

	return NULL;
}


bool SmallObjectAllocator::ValidateHeap(HeapValidationLevel /*heapValidationLevel*/)
{
	int nErrorCount = 0;

	// Disabled because this function doesn't currently use heapValidationLevel anywhere.
	// #if (PPM_DEBUG >= 3)  // For higher debug levels, we bump up the requested validation level.
	//     heapValidationLevel = (HeapValidationLevel)(heapValidationLevel + (PPM_DEBUG - 1));
	// #endif

	if(mPoolArray)
	{
		uint32_t chunkSizeCurrent = 0;

		for(const Pool* pPool = mPoolArray, *pPoolEnd = mPoolArray + mnPoolCount; pPool != pPoolEnd; ++pPool)
		{
			// Verify that Pools are sorted by size.
			nErrorCount += PPM_VERIFY(this, pPool->mnChunkSize >= chunkSizeCurrent, kGAErrorInvalidInput, kGAErrorText[kGAErrorInvalidInput], NULL, NULL);
			chunkSizeCurrent = pPool->mnChunkSize;

			// Verify that Pool::mnChunkSize is sane.
			nErrorCount += PPM_VERIFY(this, pPool->mnChunkSize <= mnMaxMallocSize, kGAErrorInternal, kGAErrorText[kGAErrorInternal], pPool, NULL);

			// Verify that Pool::mpCoreBlockCurrent is sane.
			nErrorCount += PPM_VERIFY(this, !pPool->mCoreBlockList.mpCoreBlockFirst || !pPool->mpCoreBlockCurrent || pPool->mCoreBlockList.Contains(pPool->mpCoreBlockCurrent), kGAErrorInternal, kGAErrorText[kGAErrorInternal], pPool, NULL);

			uint32_t coreBlockFreeCurrent = 0xffffffff;

			for(const CoreBlock* pCoreBlock = pPool->mCoreBlockList.mpCoreBlockFirst; pCoreBlock; pCoreBlock = pCoreBlock->mpNext)
			{
				// Verify that CoreBlock list pointers are sane.
				if(pCoreBlock->mpPrev)
					nErrorCount += PPM_VERIFY(this, pCoreBlock->mpPrev->mpNext == pCoreBlock, kGAErrorInternal, kGAErrorText[kGAErrorInternal], pCoreBlock, NULL);
				else
					nErrorCount += PPM_VERIFY(this, pCoreBlock == pPool->mCoreBlockList.mpCoreBlockFirst, kGAErrorInternal, kGAErrorText[kGAErrorInternal], pCoreBlock, NULL);

				// Verify that CoreBlock::mpChunkData is sane.
				nErrorCount += PPM_VERIFY(this, ((uint8_t*)pCoreBlock->mpChunkData >= pCoreBlock->mData) && ((uint8_t*)pCoreBlock->mpChunkData < ((uint8_t*)pCoreBlock + pCoreBlock->mnSize)), kGAErrorCorruptedHeap, kGAErrorText[kGAErrorCorruptedHeap], pCoreBlock, NULL);

				// Verify that CoreBlock::mnChunkSize is sane.
				nErrorCount += PPM_VERIFY(this, pCoreBlock->mnChunkSize == pPool->mnChunkSize, kGAErrorInternal, kGAErrorText[kGAErrorInternal], pCoreBlock, NULL);

				// Verify that CoreBlock::mnChunkCount is sane.
				nErrorCount += PPM_VERIFY(this, ((uint32_t)pCoreBlock->mnChunkCount * (uint32_t)pCoreBlock->mnChunkSize) <= pCoreBlock->mnSize, kGAErrorInternal, kGAErrorText[kGAErrorInternal], pCoreBlock, NULL);

				// Verify that CoreBlock::mnFreeChunkCount is sane.
				nErrorCount += PPM_VERIFY(this, pCoreBlock->mnFreeChunkCount <= pCoreBlock->mnChunkCount, kGAErrorInternal, kGAErrorText[kGAErrorInternal], pCoreBlock, NULL);

				// Verify that CoreBlocks are sorted by count of free blocks (most to least).
				nErrorCount += PPM_VERIFY(this, pCoreBlock->mnFreeChunkCount <= coreBlockFreeCurrent, kGAErrorInvalidInput, kGAErrorText[kGAErrorInvalidInput], NULL, NULL);
				coreBlockFreeCurrent = pCoreBlock->mnFreeChunkCount;

				// Walk through the free list for this CoreBlock.
				for(const Chunk* pCurrent = pCoreBlock->mpChunkList; pCurrent; pCurrent = pCurrent->mpNext)
				{
					// Verify that all free blocks in power-of-two pools are aligned as per the Pool's specification.
					if(PPM_IsPowerOf2(pPool->mnChunkSize))
						nErrorCount += PPM_VERIFY(this, PPM_IsAligned(pCurrent, pPool->mnChunkAlignment), kGAErrorInvalidInput, kGAErrorText[kGAErrorInvalidInput], NULL, NULL);

					// Verify that all blocks in the free list belong to the given CoreBlock.
					nErrorCount += PPM_VERIFY(this, pCoreBlock->Contains(pCurrent), kGAErrorInternal, kGAErrorText[kGAErrorInternal], NULL, NULL);

					// Verify that pCurrent points to the beginning of a Chunk and not somewhere in the middle of one.
					nErrorCount += PPM_VERIFY(this, const_cast<CoreBlock*>(pCoreBlock)->GetChunk(pCurrent) == pCurrent, kGAErrorInternal, kGAErrorText[kGAErrorInternal], NULL, NULL);

					// Verify any free guard fills are valid and not overwritten by a user as might happen with using a stale pointer.
					#if PPM_DEBUG_FILL && (PPM_DEBUG >= 2)
						uint8_t* const pMemBegin = (uint8_t*)(&pCurrent->mpNext + 1);
						uint8_t* const pMemEnd   = (uint8_t*)pCurrent + pCoreBlock->mnChunkSize;

						nErrorCount += PPM_VERIFY(this, DebugMemCheck(pMemBegin, (size_t)(pMemEnd - pMemBegin), kDefaultFillValueFree), kGAErrorCorruptedChunk, kGAErrorText[kGAErrorCorruptedChunk], NULL, NULL);
					#endif
				}

				// Verify that mnCoreBlockAlignmentMask is consistent with this CoreBlock.
				if(mnCoreBlockAlignmentMask) // If the mnCoreBlockAlignmentMask value is enabled...
				{
					// This mask calculation is a nifty bit manipulation trick. For values 7, 8, 9, 15, 16, 17, it returns 0x3, 0x3, 0x7, 0x7, 0xf.
					const uintptr_t nObservedAlignmentMask = (uintptr_t)(((intptr_t)pCoreBlock ^ ((intptr_t)pCoreBlock - 1)) >> 1);

					nErrorCount += PPM_VERIFY(this, mnCoreBlockAlignmentMask >= nObservedAlignmentMask, kGAErrorInternal, kGAErrorText[kGAErrorInternal], NULL, NULL);
				}

				// Verify that all allocated Chunks have valid guard fills and not overwritten by a user.
				// To do.
				// This check is a little slow to do because currently the only way we have to tell allocated Chunk from
				// free Chunk is to take each Chunk in the CoreBlock and see if it isn't in the free list.
			}
		}
	}

	return (nErrorCount == 0);
}


SmallObjectAllocator::ReportContext* SmallObjectAllocator::ReportBegin(ReportContext* pReportContext, int nBlockTypeFlags)
{
	memset(pReportContext, 0, sizeof(ReportContext));

	if(mPoolArray)
	{
		pReportContext->mnBlockTypeFlags = nBlockTypeFlags;
		pReportContext->mpPool           = mPoolArray;
		pReportContext->mpCoreBlock      = mPoolArray[0].mCoreBlockList.mpCoreBlockFirst;
		pReportContext->mpChunk          = pReportContext->mpCoreBlock ? mPoolArray[0].mCoreBlockList.mpCoreBlockFirst->mpChunkData : NULL;

		return pReportContext;
	}

	return NULL;
}


const SmallObjectAllocator::BlockInfo* SmallObjectAllocator::ReportNext(ReportContext* pReportContext, int nBlockTypeFlags)
{
	PPM_ASSERT(this, sizeof(ReportContextInternal) == sizeof(ReportContext), kGAErrorInternal, kGAErrorText[kGAErrorInternal], NULL, NULL); // If this fails then we didn't keep ReportContext and ReportContextInternal in sync.

	if(pReportContext)
	{
		// In this case we iterate the heap as we go. The user must not modify the heap 
		// while we are doing this or else it is possible that an exception will occur.
		nBlockTypeFlags &= pReportContext->mnBlockTypeFlags;

		// We make these tricky looking references, but all they are is a way to treat void pointers
		// as typed pointers. The reason ReportContext::mpPool, mpCoreBlock, mpChunk are void pointers
		// and not of type Pool*, CoreBlock*, Chunk* is that the ReportContext struct is public but 
		// these types are protected and not visible. So we convert void* to Pool*, etc. 
		// To do: Instead of using this technique, make an internal ReportContext which is a union of
		// or alias of the public version in a way that allows us to avoid the trickery here.

		// Used to handle pointer aliasing. This actually isn't the right way to do it; you really need to make a union of the types and not pointers to the types.
		union ReportContextU
		{
			ReportContext*         mRC;
			ReportContextInternal* mRCI;
		} rcu = { pReportContext };

		Pool*&      pPool      = rcu.mRCI->mpPool;
		CoreBlock*& pCoreBlock = rcu.mRCI->mpCoreBlock;
		Chunk*&     pChunk     = rcu.mRCI->mpChunk;

		// For each Pool...
		for(pPool = pPool ? pPool : &mPoolArray[mnPoolCount]; 
			pPool != &mPoolArray[mnPoolCount]; 
			pPool++, pCoreBlock = ((pPool != &mPoolArray[mnPoolCount]) ? pPool->mCoreBlockList.mpCoreBlockFirst : NULL), pChunk = NULL)
		{
			// For each CoreBlock in each Pool...
			for(pCoreBlock = pCoreBlock ? pCoreBlock : pPool->mCoreBlockList.mpCoreBlockFirst;
				pCoreBlock;
				pCoreBlock = pCoreBlock->mpNext, pChunk = (pCoreBlock ? pCoreBlock->mpChunkData : NULL))
			{
				// For each Chunk in each CoreBlock...
				for(pChunk = pChunk ? pChunk : pCoreBlock->mpChunkData; 
					pChunk && (pChunk < (Chunk*)((uintptr_t)pCoreBlock->mpChunkData + (pCoreBlock->mnChunkCount * pCoreBlock->mnChunkSize))); 
					pChunk = (Chunk*)((uintptr_t)pChunk + pCoreBlock->mnChunkSize))
				{
					bool bChunkIsAllocated = false;

					if(ChunkMatchesBlockType(pCoreBlock, pChunk, nBlockTypeFlags, bChunkIsAllocated))
					{
						// Set the next report Chunk/CoreBlock/Pool.
						pChunk = (Chunk*)((uintptr_t)pChunk + pCoreBlock->mnChunkSize);

						// Set the user data.
						pReportContext->mBlockInfo.mpCore     = (const char*)(uintptr_t)pCoreBlock;
						pReportContext->mBlockInfo.mnCoreSize = pCoreBlock->mnSize;
						pReportContext->mBlockInfo.mpData     = (void*)(uintptr_t)pChunk;
						pReportContext->mBlockInfo.mnDataSize = pCoreBlock->mnChunkSize;
						pReportContext->mBlockInfo.mBlockType = bChunkIsAllocated ? (char)kBlockTypeAllocated : (char)kBlockTypeFree;

						return &pReportContext->mBlockInfo;
					}
				}
			}
		}
	}

	return NULL;
}


void SmallObjectAllocator::ReportEnd(ReportContext* pReportContext)
{
	// We need do nothing significant in this implementation. 
	// But we may need to do something in the future.
	memset(pReportContext, 0, sizeof(ReportContext));
}


bool SmallObjectAllocator::ChunkMatchesBlockType(const CoreBlock* pCoreBlock, const Chunk* pChunk, int nBlockTypeFlags, bool& bChunkIsAllocated)
{
	bChunkIsAllocated = !pCoreBlock->IsChunkFree(pChunk);

	if((nBlockTypeFlags & (kBlockTypeAllocated | kBlockTypeFree)) == (kBlockTypeAllocated | kBlockTypeFree))
		return true; // The user is asking for any type of flags.
	if(nBlockTypeFlags & kBlockTypeAllocated)
		return bChunkIsAllocated;
	if(nBlockTypeFlags & kBlockTypeFree)
		return !bChunkIsAllocated;

	return false;
}


bool SmallObjectAllocator::CoreBlock::IsChunkFree(const Chunk* pChunk) const
{
	for(const Chunk* pCurrent = mpChunkList; pCurrent; pCurrent = pCurrent->mpNext)
	{
		if(pCurrent == pChunk)
			return true;
	}

	return false;
}


SmallObjectAllocator::CoreBlockList::CoreBlockList()
	: mpCoreBlockFirst(NULL)
{
	// Empty
}


bool SmallObjectAllocator::CoreBlockList::Contains(const CoreBlock* pCoreBlock) const
{
	for(const CoreBlock* pCurrent = mpCoreBlockFirst; pCurrent; pCurrent = pCurrent->mpNext)
	{
		if(pCurrent == pCoreBlock)
			return true;
	}

	return false;
}


void SmallObjectAllocator::CoreBlockList::PushFront(CoreBlock* pCoreBlock)
{
	if(mpCoreBlockFirst)
	{
		// Verify that pCoreBlock is being inserted only when other CoreBlocks are exhausted.
		// PPM_ASSERT(this, mpCoreBlockFirst->mnFreeChunkCount == 0, kGAErrorInternal, kGAErrorText[kGAErrorInternal], NULL, NULL);

		InsertBefore(pCoreBlock, mpCoreBlockFirst);
		mpCoreBlockFirst = pCoreBlock;
	}
	else
	{
		pCoreBlock->mpPrev = NULL;
		pCoreBlock->mpNext = mpCoreBlockFirst;
		mpCoreBlockFirst   = pCoreBlock;
	}
}


void SmallObjectAllocator::CoreBlockList::InsertBefore(CoreBlock* pCoreBlock, CoreBlock* pCoreBlockNext)
{
	pCoreBlock->mpNext = pCoreBlockNext;
	pCoreBlock->mpPrev = pCoreBlockNext->mpPrev;

	if(pCoreBlock->mpPrev)
		pCoreBlock->mpPrev->mpNext = pCoreBlock;
	else
	{
		// PPM_ASSERT(this, pCoreBlockNext == mpCoreBlockFirst, kGAErrorInternal, kGAErrorText[kGAErrorInternal], NULL, NULL);
		mpCoreBlockFirst = pCoreBlock;
	}

	pCoreBlockNext->mpPrev = pCoreBlock;
}


void SmallObjectAllocator::CoreBlockList::InsertAfter(CoreBlock* pCoreBlock, CoreBlock* pCoreBlockPrev)
{
	pCoreBlock->mpPrev = pCoreBlockPrev;
	pCoreBlock->mpNext = pCoreBlockPrev->mpNext;
	
	if(pCoreBlockPrev->mpNext)
		pCoreBlock->mpNext->mpPrev = pCoreBlock;

	pCoreBlockPrev->mpNext = pCoreBlock;
}


void SmallObjectAllocator::CoreBlockList::Remove(CoreBlock* pCoreBlock)
{
	if(mpCoreBlockFirst == pCoreBlock)
	{
		mpCoreBlockFirst = mpCoreBlockFirst->mpNext;

		if(mpCoreBlockFirst)
			mpCoreBlockFirst->mpPrev = NULL;
	}
	else
	{
		// PPM_ASSERT(this, pCoreBlock->mpPrev != NULL, kGAErrorInternal, kGAErrorText[kGAErrorInternal], NULL, NULL);
		pCoreBlock->mpPrev->mpNext = pCoreBlock->mpNext;
	}

	if(pCoreBlock->mpNext)
	   pCoreBlock->mpNext->mpPrev = pCoreBlock->mpPrev;

	#if (PPM_DEBUG >= 1)
		pCoreBlock->mpNext = NULL;
		pCoreBlock->mpPrev = NULL;
	#endif
}




} // namespace Allocator

} // namespace EA


#if   defined(_MSC_VER)
	#pragma warning(pop)
#endif



