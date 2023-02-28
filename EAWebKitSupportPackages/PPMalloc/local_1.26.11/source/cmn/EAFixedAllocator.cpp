///////////////////////////////////////////////////////////////////////////////
// EAFixedAllocator
//
// Copyright (c) 2004, Electronic Arts. All Rights Reserved.
// By Paul Pedriana, Maxis
//
// This module implements an allocator specialized for objects of fixed size.
//
// The benefit is that there is no overhead for object storage and that many
// similar objects can be allocated near each other for good cache behaviour.
// Also, allocation is very fast. The downside to this kind of allocator is 
// that any unallocated space is not available for use; this is what we call 
// "external fragmentation."
///////////////////////////////////////////////////////////////////////////////


#include <PPMalloc/EAFixedAllocator.h>
#include <stddef.h>


///////////////////////////////////////////////////////////////////////////////
// PPM_ENABLED (defined as 0 or 1, default is 1)
//
// When this is enabled, this module recognizes PPMalloc modules external to 
// this module. If disabled, then this module has no dependencies on anything
// except standard C/C++ libraries and you can thus use it with other
// allocation systems.
//
#ifndef PPM_ENABLED
	#define PPM_ENABLED 1
#endif

#if PPM_ENABLED
	#include <PPMalloc/EAGeneralAllocator.h>
	#include <PPMalloc/EAGeneralAllocatorDebug.h>

	namespace EA{
		namespace Allocator{
			extern PPM_API GeneralAllocator*      gpEAGeneralAllocator;
			extern PPM_API GeneralAllocatorDebug* gpEAGeneralAllocatorDebug;
		}
	}
#else
	#include <stdlib.h>
#endif
///////////////////////////////////////////////////////////////////////////////




namespace EA
{
namespace Allocator
{


///////////////////////////////////////////////////////////////////////////////
// FixedAllocatorBase
//
FixedAllocatorBase::FixedAllocatorBase(size_t nObjectSize, size_t nObjectAlignment, 
									   size_t nCountPerCoreBlock, void* pInitialCore, size_t nInitialCoreSize, 
									   CoreAllocationFunction pCoreAllocationFunction, 
									   CoreFreeFunction pCoreFreeFunction, void* pCoreFunctionContext)
  : mnObjectSize(nObjectSize),
	mnObjectAlignment(8),                           // Set to 8 for now, but the Init call below may modify it.
	mnCountPerCoreBlock(kDefaultCountPerCoreBlock), // Will check and set mnCountPerCoreBlock in Init().
	mpHeadCoreBlock(NULL),
	mpHeadChunk(NULL),
	mpCoreAllocationFunction(DefaultFixedAllocationFunction),
	mpCoreFreeFunction(DefaultFixedFreeFunction),
	mpCoreFunctionContext(NULL)
{
	Init(nObjectSize, nObjectAlignment, nCountPerCoreBlock, pInitialCore, 
		 nInitialCoreSize, pCoreAllocationFunction, pCoreFreeFunction, pCoreFunctionContext);
}


///////////////////////////////////////////////////////////////////////////////
// ~FixedAllocatorBase
//
FixedAllocatorBase::~FixedAllocatorBase()
{
	Shutdown();
}


///////////////////////////////////////////////////////////////////////////////
// Init
//
bool FixedAllocatorBase::Init(size_t nObjectSize, size_t nObjectAlignment, 
							  size_t nCountPerCoreBlock, void* pCore, size_t nCoreSize, 
							  CoreAllocationFunction pCoreAllocationFunction, 
							  CoreFreeFunction pCoreFreeFunction, void* pCoreFunctionContext)
{
	mnObjectAlignment = (nObjectAlignment <= 0) ? mnObjectAlignment : nObjectAlignment;
	if(nObjectSize)
	{
		mnObjectSize = (nObjectSize >= sizeof(Chunk) ? nObjectSize : sizeof(Chunk));
		mnObjectSize = (((mnObjectSize + (mnObjectAlignment - 1))) & ~(mnObjectAlignment - 1));
	}
	if(nCountPerCoreBlock)
		mnCountPerCoreBlock = nCountPerCoreBlock;
	if(pCoreAllocationFunction)
		mpCoreAllocationFunction = pCoreAllocationFunction;
	if(pCoreFreeFunction)
		mpCoreFreeFunction = pCoreFreeFunction;
	mpCoreFunctionContext = pCoreFunctionContext;

	if(!mpHeadCoreBlock) // If not already initialized...
		return AddCore(pCore, nCoreSize);
	return false;
}


///////////////////////////////////////////////////////////////////////////////
// Shutdown
//
bool FixedAllocatorBase::Shutdown()
{
	while(mpHeadCoreBlock)
	{
		CoreBlock* const pCoreBlock = mpHeadCoreBlock;
		mpHeadCoreBlock = mpHeadCoreBlock->mpNext;
		mpCoreFreeFunction(pCoreBlock, mpCoreFunctionContext);
	}

	mpHeadChunk = NULL;

	// The following can stay as-is:
	// mnObjectSize;
	// mnObjectAlignment;
	// mnCountPerCoreBlock;
	// mpHeadCoreBlock;
	// mpCoreAllocationFunction;
	// mpCoreFreeFunction;
	// mpCoreFunctionContext;

	return true;
}


///////////////////////////////////////////////////////////////////////////////
// Reset
//
void FixedAllocatorBase::Reset()
{
	for(CoreBlock* pCoreBlock = mpHeadCoreBlock; pCoreBlock; pCoreBlock = pCoreBlock->mpNext)
		LinkChunks(pCoreBlock);
}


///////////////////////////////////////////////////////////////////////////////
// AddCore
//
bool FixedAllocatorBase::AddCore(void* pCore, size_t nCoreSize)
{
	if(pCore)
	{
		// Add to linked list of core blocks.
		CoreBlock* const pCoreBlock = (CoreBlock*)pCore;
		pCoreBlock->mpNext = mpHeadCoreBlock;
		pCoreBlock->mnSize = nCoreSize;
		mpHeadCoreBlock    = pCoreBlock;

		// Create the initial links between this block's free chunks.
		return LinkChunks(pCoreBlock);
	}

	if(nCoreSize)
	{
		if(nCoreSize == (size_t)-1)
			return false;
		else
		{
			pCore = mpCoreAllocationFunction(nCoreSize, mpCoreFunctionContext);
			if(!pCore)
				return false;
		}
	}
	else
		nCoreSize = sizeof(CoreBlock) + (mnObjectSize * (mnCountPerCoreBlock + 2)); // +2 because we need +1 to be the NULL block and +1 to allow enough room for the array of objects to be aligned on the object alignment.

	return AddCore(pCore, nCoreSize);
}


#if defined(EA_COMPILER_CLANG)
// UBSAN: runtime error : index 15 out of bounds for type 'char [8]'
// pCoreBlock has: char mData[sizeof(void*)]; /// The actual size will be greater than this.
__attribute__((no_sanitize("array-bounds")))
#endif
bool FixedAllocatorBase::LinkChunks(CoreBlock* pCoreBlock)
{
	Chunk* pStart = (Chunk*)(((intptr_t)(pCoreBlock->mData + (mnObjectAlignment - 1))) & ~(mnObjectAlignment - 1));
	Chunk* pEnd   = (Chunk*)((char*)pCoreBlock + pCoreBlock->mnSize - (2 * mnObjectSize));

	pCoreBlock->mnChunkCount = 0;
	if(pStart <= pEnd) // If there is any room... (Usually there will enough room unless the user specifies a very small core size).
	{
		mpHeadChunk = pStart;
		pCoreBlock->mnChunkCount++;

		while(pStart <= pEnd)
		{
			Chunk* const pNext = (Chunk*)((char*)pStart + mnObjectSize);
			pStart->mpNext = pNext;
			pStart = pNext;
			pCoreBlock->mnChunkCount++;
		}

		pStart->mpNext = NULL;

		return true;
	}

	return false;
}

void FixedAllocatorBase::FreeUnusedCore()
{
	// look at every core except that original one (last in the list)
	CoreBlock* coreBlock = mpHeadCoreBlock;
	CoreBlock* prevBlock = NULL;
	while (coreBlock->mpNext != NULL)
	{
		// Count how many chunks are in the free list for this core block
		void *start = coreBlock;
		void *end = (char*)coreBlock + coreBlock->mnSize;
		Chunk* chunk = mpHeadChunk;
		size_t numFreeChunks = 0;

		while (chunk != NULL)
		{
			// see if the chunk lies within the core block
			if (chunk >= start && chunk < end)
			{
				numFreeChunks++;
			}
			chunk = chunk->mpNext;
		}

		// if the entire core block is unused
		if (numFreeChunks == coreBlock->mnChunkCount)
		{
			// remove all the chunks from the free list
			Chunk* prevChunk = NULL;
			chunk = mpHeadChunk;
			while (chunk != NULL)
			{
				// see if the chunk lies within the core block
				if (chunk >= start && chunk < end)
				{
					// remove the chunk from the list
					if (prevChunk == NULL)
					{
						mpHeadChunk = chunk->mpNext;
					}
					else
					{
						prevChunk->mpNext = chunk->mpNext;
					}
				}
				else
				{
					prevChunk = chunk;
				}
				chunk = chunk->mpNext;
			}

			// remove the core block
			if (prevBlock == NULL)
			{
				mpHeadCoreBlock = coreBlock->mpNext;
			}
			else
			{
				prevBlock->mpNext = coreBlock->mpNext;
			}

			CoreBlock *emptyBlock = coreBlock;
			coreBlock = coreBlock->mpNext;

			// free the core
			mpCoreFreeFunction(emptyBlock, mpCoreFunctionContext);
		}
		else
		{
			// the core block has 1 ore more allocated chunks, skip it
			prevBlock = coreBlock;
			coreBlock = coreBlock->mpNext;
		}
	}
}

///////////////////////////////////////////////////////////////////////////////
// DefaultFixedAllocationFunction
//
void* DefaultFixedAllocationFunction(size_t nSize, void* /*pContext*/)
{
	#if PPM_ENABLED
		return EA::Allocator::gpEAGeneralAllocator->Malloc(nSize);
	#else
		return malloc(nSize);
	#endif
}


///////////////////////////////////////////////////////////////////////////////
// DefaultFixedFreeFunction
//
void DefaultFixedFreeFunction(void* pData, void* /*pContext*/)
{
	#if PPM_ENABLED
		EA::Allocator::gpEAGeneralAllocator->Free(pData);
	#else
		free(pData);
	#endif
}

///////////////////////////////////////////////////////////////////////////////
// NamedFixedAllocationFunction
//
void* NamedFixedAllocationFunction(size_t nSize, void* pContext)
{
	using namespace EA::Allocator;

	NamedFixedAllocatorContext* pInfo = (EA::Allocator::NamedFixedAllocatorContext*) pContext;

	#if PPM_ENABLED
		if(gpEAGeneralAllocatorDebug && pInfo)
			return gpEAGeneralAllocatorDebug->MallocDebug(nSize, (int32_t)pInfo->mFlags, 0, pInfo->mpName);
		else
			return gpEAGeneralAllocator->Malloc(nSize, pInfo ? (int32_t)pInfo->mFlags : (int32_t)0);
	#else
		EA_UNUSED(pInfo);
		return malloc(nSize);
	#endif
}


///////////////////////////////////////////////////////////////////////////////
// NamedFixedFreeFunction
//
void NamedFixedFreeFunction(void* pData, void* pContext)
{
	using namespace EA::Allocator;

	NamedFixedAllocatorContext* pInfo = (EA::Allocator::NamedFixedAllocatorContext*) pContext;
	EA_UNUSED(pInfo);

	#if PPM_ENABLED
		if(gpEAGeneralAllocatorDebug)
			gpEAGeneralAllocatorDebug->Free(pData);
		else
			gpEAGeneralAllocator->Free(pData);
	#else
		free(pData);
	#endif
}


///////////////////////////////////////////////////////////////////////////////
// NULLFixedAllocationFunction
//
void* NULLFixedAllocationFunction(size_t /*nSize*/, void* /*pContext*/)
{
	return NULL;
}


///////////////////////////////////////////////////////////////////////////////
// NULLFixedFreeFunction
//
void NULLFixedFreeFunction(void* /*pData*/, void* /*pContext*/)
{
	// Empty
}



} // namespace Allocator

} // namespace EA








