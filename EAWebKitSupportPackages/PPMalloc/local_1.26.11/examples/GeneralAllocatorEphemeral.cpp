///////////////////////////////////////////////////////////////////////////////
// GeneralAllocatorEphemeral
//
// Copyright (c) 2007 Criterion Software Limited and its licensors. All Rights Reserved.
// By Paul Pedriana
//
// Implements "ephemeral" allocation.
///////////////////////////////////////////////////////////////////////////////



// This is a special kind of allocation that is neither high (permanent) nor 
// low (temporary). It is an allocation that is very temporary, such as for
// less than one graphics frame. The purpose of ephemeral allocation is to 
// allocate memory away from all other memory so that when the ephemeral allocation
// goes away it won't leave any holes in the heap due to other allocations 
// occurring in the meantime. A typical usage might be to allocate temporary 
// decompression buffer as ephemeral, with the assumption that the decompression
// process might generate additional heap allocations. So when you free the 
// decompression buffer it won't leave a hole in the heap and the aforementioned
// additional heap allocations will be tightly packed.
//
// Ephemeral allocations by their nature temporarily fragment the heap with the
// goal that after they are freed the heap will be less fragmented than it 
// would have been otherwise. You need to free ephemeral allocations soon or
// else you will be worse off then you would be otherwise.
//
// The mechanism of this function is to allocate a large chunk out from the 
// large "top chunk" in the middle of the heap. Then allocate the user chunk,
// then free the large top chunk. This will result in the user chunk being located
// in the middle of a big open area. Normally this is a bad thing, but if it is 
// very temporary and is freed before the user needs that open space then it 
// results in less fragmented memory.


namespace EA
{

namespace Allocator
{



class Ephemeral
{
public:
	Ephemeral();

	void* Malloc(GeneralAllocator* pAllocator, size_t nSize, int nAllocationFlags);
	void  Free(GeneralAllocator* pAllocator, void* p);

protected:
	static const size_t kSlotCount = 32;

	int   mnEphemeralCount;
	void* mEphemeralArray[kSlotCount];
};



Ephemeral::Ephemeral()
  : mnEphemeralCount(0)
{
	#if (PPM_DEBUG > 0)
		memset(mEphemeralArray, 0, sizeof(mEphemeralArray));
	#endif
}



void* Ephemeral::Malloc(GeneralAllocator* pAllocator, size_t nSize, int nAllocationFlags)
{
	void* pData = NULL;

	if(mnEphemeralCount < kSlotCount) // If we haven't run out of slots...
	{
		if(mnEphemeralCount == 0) // If this is the first one (of if all have been freed since the last one)...
		{
			// mpTopChunk is not exposed by GeneralAllocator as of this writing.
			// Previous implementations of Ephemeral were internal to GeneralAllocator and 
			// thus had access to mpTopChunk. To do: Expose mpTopChunk somehow.
			// const size_t nTopChunkSize = (size_t)pAllocator->GetChunkSize(pAllocator->mpTopChunk);
			const size_t nTopChunkSize = GetLargestFreeBlock(false); // Potentially flaws alternative, as the largest block might not be the top chunk.

			if(nTopChunkSize > ((nSize + kMinAllocSize) * 5 / 4)) // If ephemeral allocation looks like it can succeed...
			{
				const size_t nDummySize = (size_t)((nTopChunkSize / 2) - (nSize / 2));
				void* const  pDummy     = pAllocator->Malloc(nDummySize, GeneralAllocator::kAllocationFlagUseTopChunk);

				if(pDummy)
				{
					pData = pAllocator->Malloc(nSize, nAllocationFlags | GeneralAllocator::kAllocationFlagUseTopChunk);
					pAllocator->Free(pDummy);
				}
			}
		}
		else
		{
			// This is not the first ephemeral allocation, place the block right after the last one.
			pData = pAllocator->Malloc(nSize, nAllocationFlags | GeneralAllocator::kAllocationFlagUseTopChunk);
		}

		if(pData)
			mEphemeralArray[mnEphemeralCount++] = pData;
	}
	else
	{
		// Cannot do ephemeral allocation.
		pData = pAllocator->Malloc(nSize, nAllocationFlags);
	}

	return pData;
}



void Ephemeral::Free(GeneralAllocator* pAllocator, void* p)
{
	if(mnEphemeralCount) // The vast majority of the time mnEphemeralCount should be 0.
	{
		// See if the freed block was allocated as ephemeral. 
		// Currently we implement a small linear list lookup.
		for(int i = 0; i < mnEphemeralCount; i++)
		{
			if(mEphemeralArray[i] == pData)
			{
				memcpy(mEphemeralArray + i, mEphemeralArray + (i + 1), (mnEphemeralCount - (i + 1)) * sizeof(void*));
				mnEphemeralCount--;
				break;
			}
		}
	}

	pGeneralAllocator->Free(p);
}

} // namespace Allocator

} // namespace EA






