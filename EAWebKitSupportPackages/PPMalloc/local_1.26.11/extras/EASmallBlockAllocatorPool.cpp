///////////////////////////////////////////////////////////////////////////////
// SmallBlockAllocator
//
// Copyright (c) 2007, Electronic Arts. All Rights Reserved.
// Created by Henry Allen
//
///////////////////////////////////////////////////////////////////////////////

#include <EABase/eabase.h>

EA_DISABLE_VC_WARNING(4574)
#include <new>
#include <string.h> //for memset
EA_RESTORE_VC_WARNING()

#include "EASmallBlockAllocatorPool.h"
#include <PPMalloc/internal/shared.h>


#if defined(_MSC_VER)
	#pragma warning(push)
	#pragma warning(disable: 6011) // Dereferencing NULL pointer.  (VC++ /analysis is buggy)
#endif


//provides extra debuging/validation calls that will slow down the system
#define EA_SOAPOOL_DIAGNOSTICS ((PPM_DEBUG >= 2) || 0)

#if EA_SOA_HOOKS_ENABLED

	#define EA_SOAPOOL_HOOK(_type, _size)       \
	{                                           \
		HookInfo hookInfo((_type),(_size));     \
		mCallbacks.Hook(&hookInfo);             \
	}

#else

	#define EA_SOAPOOL_HOOK(_type, _size)

#endif


#if (EA_PLATFORM_PTR_SIZE == 4)
	#define _EA_SOAPOOL_PTRFMT_ "%08x"
#elif (EA_PLATFORM_PTR_SIZE == 8)
	#define _EA_SOAPOOL_PTRFMT_ "%16x"
#elif (EA_PLATFORM_PTR_SIZE == 16)
	#define _EA_SOAPOOL_PTRFMT_ "%32x"
#else
	#error unsupported EA_PLATFORM_PTR_SIZE
#endif


namespace EA
{
namespace Allocator
{

/// per Chunk debug data
struct SmallBlockAllocator::Pool::ChunkDebugData
{
	SmallBlockAllocator::ChunkRef mChunkAddr; /// the memory Chunk that this data is for
	const char*  mpAllocFile;        /// file the Chunk was allocated from
	uint32_t        mnRequestedSize;    /// the size the user actually requested
	uint32_t        mnAllocLine;        /// line number the Chunk was allocated from
	int32_t         mnGroupId;          /// groupId for this Chunk

	//mAllocName MUST REMAIN at the end of this struct!
	//The array will be longer than 1 byte!
	char         mAllocName[1];      /// name of the Chunk
};

/// per CoreBlock debug data
struct SmallBlockAllocator::Pool::CoreBlockDebugData
{
	//mChunkData MUST REMAIN at the end of this struct!
	//The array will be longer than 1 byte!
	ChunkDebugData* mChunkData[1];      /// array of debug 1 data for each Chunk in the CoreBlock
};

/// per pool debug data
struct SmallBlockAllocator::Pool::PoolDebugData
{
	PoolDebugData() : mnNumChunks(0), mHighWaterMarkNumChunks(0), mHighWaterMarkStats(),
				  mnNumGrows(0), mnNumShrinks(0), mnGroupId(0) {};
	~PoolDebugData() {};

	uint32_t                        mnNumChunks;
	uint32_t                        mHighWaterMarkNumChunks;
	SmallBlockAllocator::PoolInfo  mHighWaterMarkStats;

	uint32_t                        mnNumGrows;                 /// how many times has the pool had to grow
	uint32_t                        mnNumShrinks;               /// how many times has the pool shrunk
	int32_t                         mnGroupId;                  /// current group ID to assign to allocations in this pool
};

/******************************************************************************/
/*! SmallBlockAllocator::Pool::CoreBlockList

	\brief      [Put your brief description here.]

	\param       - none.
	\return      -

	[Put your optional additional description here.]
*/
/******************************************************************************/
SmallBlockAllocator::Pool::CoreBlockList::CoreBlockList() : mFirst(NULL)
{
}

/******************************************************************************/
/*! SmallBlockAllocator::Pool::CoreBlockList::~CoreBlockList

	\brief      [Put your brief description here.]

	\param       - none.
	\return      -

	[Put your optional additional description here.]
*/
/******************************************************************************/
SmallBlockAllocator::Pool::CoreBlockList::~CoreBlockList()
{
}

/******************************************************************************/
/*! SmallBlockAllocator::Pool::Pool

	\brief      [Put your brief description here.]

	\param      callbacks -
	\return               -

	[Put your optional additional description here.]
*/
/******************************************************************************/
SmallBlockAllocator::Pool::Pool(SmallBlockAllocator::Callbacks& callbacks)
	: mCallbacks(callbacks), mCoreBlockList()
{
	Construct();
}

/******************************************************************************/
/*! SmallBlockAllocator::Pool::Construct

	\brief      [Put your brief description here.]

	\param       - none.
	\return      - void.

	[Put your optional additional description here.]
*/
/******************************************************************************/
void SmallBlockAllocator::Pool::Construct()
{
	mpAllocCoreBlock = NULL;
	mpDebugData = NULL;
	mpCoreBlockPool = NULL;

	mnChunkSize = 0;
	mAutoShrink = true;
	mInitialized = false;
	mCoreBlockType = kCoreBlockTypeInvalid;
	mPoolStats = 0;
	mChunkStats = 0;
	mUnused = 0;
}

/******************************************************************************/
/*! SmallBlockAllocator::Pool::Init

	\brief      [Put your brief description here.]

	\param      nChunkSize      -
	\param      nInitCount      -
	\param      nGrowChunkCount -
	\param      nMinAlignment   -
	\return                    - void.

	[Put your optional additional description here.]
*/
/******************************************************************************/
void SmallBlockAllocator::Pool::Init(uint16_t nChunkSize, uint16_t nInitCount, uint16_t nGrowChunkCount, uint16_t nMinAlignment)
{
	PPM_ASSERT(this, mCoreBlockList.GetFirst() == NULL, kGAErrorInvalidInput, kGAErrorText[kGAErrorInvalidInput], NULL, NULL);
	PPM_ASSERT(this, mpAllocCoreBlock == NULL, kGAErrorInvalidInput, kGAErrorText[kGAErrorInvalidInput], NULL, NULL);

	mCoreBlockType = kCoreBlockTypeHeap;
	mHeapParams.mnMinCoreBlockAlign = nMinAlignment;
	PPM_ASSERT(this, PPM_IsPowerOf2(mHeapParams.mnMinCoreBlockAlign), kGAErrorInvalidInput, kGAErrorText[kGAErrorInvalidInput], NULL, NULL);

	mAutoShrink = true;

	//size of Chunks managed by this pool
	mnChunkSize = nChunkSize;
	//sizeof(uintptr_t) bytes minimum needed per Chunk for use as a pointer when free
	PPM_ASSERT(this, mnChunkSize >= sizeof(uintptr_t), kGAErrorInvalidInput, kGAErrorText[kGAErrorInvalidInput], NULL, NULL);
	//Chunk size needs to be evenly divisible by 4 for now since it makes things easier (TODO: determine if this should be 4 or sizeof(uintptr_t)?)
	PPM_ASSERT(this,  0 == (mnChunkSize % 4), kGAErrorInvalidInput, kGAErrorText[kGAErrorInvalidInput], NULL, NULL);

	//setup to initially grow by init count
	mHeapParams.mnGrowChunkCount = nInitCount;
	//do first grow
	Grow();
	//initial grow took place ... subsequent grows should use nGrowChunkCount
	mHeapParams.mnGrowChunkCount = nGrowChunkCount; //set correct mHeapParams.mnGrowChunkCount

	mInitialized = true;
}

/******************************************************************************/
/*! SmallBlockAllocator::Pool::Init

	\brief      [Put your brief description here.]

	\param      nChunkSize      -
	\param      CoreBlockAllocator -
	\return                    - void.

	[Put your optional additional description here.]
*/
/******************************************************************************/
void SmallBlockAllocator::Pool::Init(uint16_t nChunkSize, Pool* pCoreBlockAllocator)
{
	PPM_ASSERT(this, mCoreBlockList.GetFirst() == NULL, kGAErrorInvalidInput, kGAErrorText[kGAErrorInvalidInput], NULL, NULL);
	PPM_ASSERT(this, mpAllocCoreBlock == NULL, kGAErrorInvalidInput, kGAErrorText[kGAErrorInvalidInput], NULL, NULL);

	mCoreBlockType = kCoreBlockTypePooled;
	mpCoreBlockPool = pCoreBlockAllocator;
	PPM_ASSERT(this, mpCoreBlockPool, kGAErrorInvalidInput, kGAErrorText[kGAErrorInvalidInput], NULL, NULL);

	mAutoShrink = true;

	//size of Chunks managed by this pool
	mnChunkSize = nChunkSize;
	//sizeof(uintptr_t) bytes minimum needed per Chunk for use as a pointer when free
	PPM_ASSERT(this, mnChunkSize >= sizeof(uintptr_t), kGAErrorInvalidInput, kGAErrorText[kGAErrorInvalidInput], NULL, NULL);
	//Chunk size needs to be evenly divisible by 4 for now since it makes things easier (TODO: determine if this should be 4 or sizeof(uintptr_t)?)
	PPM_ASSERT(this,  0 == (mnChunkSize % 4), kGAErrorInvalidInput, kGAErrorText[kGAErrorInvalidInput], NULL, NULL);

	mInitialized = true;
}

/******************************************************************************/
/*! SmallBlockAllocator::Pool::~Pool

	\brief      [Put your brief description here.]

	\param       - none.
	\return      -

	[Put your optional additional description here.]
*/
/******************************************************************************/
SmallBlockAllocator::Pool::~Pool()
{
	mInitialized = false;

	CoreBlock* pCurCoreBlock(mCoreBlockList.GetFirst());

	//free all CoreBlocks
	CoreBlock* pDelCoreBlock(NULL);

	while ( pCurCoreBlock )
	{
		pDelCoreBlock = pCurCoreBlock;
		pCurCoreBlock = pCurCoreBlock->mpNext;

		#if EA_SOA_EXTENSIONS_ENABLED
			CoreBlockDebugDataFree(pDelCoreBlock);
		#endif

		//no need to call mCoreBlockList.Remove
		//the mCoreBlockList is going away anyway

		this->FreeCoreBlock(pDelCoreBlock);
	}

	#if EA_SOA_EXTENSIONS_ENABLED
		//shutdown debug data if present
		if ( mpDebugData )
		{
			mpDebugData->~PoolDebugData(); //destruct
			mCallbacks.DebugFree(mpDebugData); //free the memory
		}
	#endif
}

/******************************************************************************/
/*! SmallBlockAllocator::Pool::SetOption

	\brief      [Put your brief description here.]

	\param      nOption -
	\param      nValue  -
	\return             - void.

	[Put your optional additional description here.]
*/
/******************************************************************************/
void SmallBlockAllocator::Pool::SetOption(int nOption, int nValue)
{
	switch (nOption)
	{
		case kOptionAutoShrink:
			mAutoShrink = nValue ? true : false;
			break;

		case kOptionEnableExtensions:
			if (nValue != kExtensionNone)
			{
				EnableExtensions(nValue);
			}
			break;

		case kOptionGrowCoreBlockCount:
			//this is only valid for kCoreBlockTypeHeap
			PPM_ASSERT(this, (kCoreBlockTypeHeap == mCoreBlockType), kGAErrorInvalidInput, kGAErrorText[kGAErrorInvalidInput], NULL, NULL);
			//0 is valid if they want to disable growing
			PPM_ASSERT(this, (nValue >= 0), kGAErrorInvalidInput, kGAErrorText[kGAErrorInvalidInput], NULL, NULL);
			if (nValue >= 0 && (kCoreBlockTypeHeap == mCoreBlockType) )
			{
				mHeapParams.mnGrowChunkCount = static_cast<uint16_t>(nValue);
				//validate the cast
				PPM_ASSERT(this, nValue == static_cast<int>(mHeapParams.mnGrowChunkCount), kGAErrorInvalidInput, kGAErrorText[kGAErrorInvalidInput], NULL, NULL);
			}
			break;

		default:
			break;
	}
}

/******************************************************************************/
/*! SmallBlockAllocator::Pool::EnableExtensions

	\brief      [Put your brief description here.]

	\param      extensionAllocator -
	\return                        - void.

	[Put your optional additional description here.]
*/
/******************************************************************************/
void SmallBlockAllocator::Pool::EnableExtensions(int nValue)
{
	#if EA_SOA_EXTENSIONS_ENABLED
		PPM_ASSERT(this, !mpDebugData && !mChunkStats && !mPoolStats, kGAErrorInvalidInput, kGAErrorText[kGAErrorInvalidInput], NULL, NULL);  //enabling twice is invalid
		if ( !mpDebugData && !mChunkStats && !mPoolStats )
		{
			//note that currently Auditing has to enable poolStats and chunkStats
			//if desired we could split out the dependency on chunkStats by having a separate implementation for auditing when poolstats aren't enabled.
			mPoolStats = static_cast<uint8_t>((0 == (nValue & (kExtensionAll | kExtensionPoolStats | kExtensionAuditing))) ? 0 : 1);
			mChunkStats = static_cast<uint8_t>((0 == (nValue & (kExtensionAll | kExtensionAllocStats | kExtensionAuditing))) ? 0 : 1);

			if (mPoolStats) 
			{
				//allocate and construct the debug data storage
				mpDebugData = static_cast<PoolDebugData*>(mCallbacks.DebugAlloc(sizeof(PoolDebugData)));
				mpDebugData = new(mpDebugData) PoolDebugData; //construct
			}

			//ideally the user has enabled the extensions immediatly after initialization (and thus this loop will not run at all)
			//however, in case they've done some allocating, iterate through the CoreBlocks and allocate debug data for them (and update numGrows)
			//so that we can assume (from now on) that any valid CoreBlock has debug data
			for ( CoreBlock* pCurCoreBlock = mCoreBlockList.GetFirst() ; pCurCoreBlock != NULL; pCurCoreBlock = pCurCoreBlock->mpNext )
			{
				if (mChunkStats)
				{
					CoreBlockDebugDataAlloc(pCurCoreBlock);
				}

				if (mPoolStats) 
				{
					mpDebugData->mnNumGrows++;
					mpDebugData->mnNumChunks += (pCurCoreBlock->GetNumChunks() - pCurCoreBlock->GetNumFreeChunks());
				}
			}
		}
	#else
		EA_UNUSED(nValue);
	#endif
}

/******************************************************************************/
/*! SmallBlockAllocator::Pool::Alloc

	\brief      [Put your brief description here.]

	\param      nSize      -
	\param      allocInfo -
	\return               - void.

	[Put your optional additional description here.]
*/
/******************************************************************************/
void* SmallBlockAllocator::Pool::Alloc(size_t nSize, const EA::Allocator::AllocInfo& allocInfo)
{
	EA_UNUSED(nSize); EA_UNUSED(allocInfo);

	if ( mpAllocCoreBlock  )    //this is the most common case
	{
		AllocCoreBlockFound:

		uint8_t* pResult(NULL);
		CoreBlock* pCurCoreBlock(mpAllocCoreBlock);
		PPM_ASSERT(this, nSize <= mnChunkSize, kGAErrorInvalidInput, kGAErrorText[kGAErrorInvalidInput], NULL, NULL);
		PPM_ASSERT(this, pCurCoreBlock->mpFreeChunks, kGAErrorInternal, kGAErrorText[kGAErrorInternal], NULL, NULL);

		//pointer to memory Chunk to return
		pResult = reinterpret_cast<uint8_t*>(pCurCoreBlock->mpFreeChunks);
		//take the Chunk off our free list
		pCurCoreBlock->mpFreeChunks = pCurCoreBlock->mpFreeChunks->mpNextFreeChunk;
		pCurCoreBlock->mnNumFreeChunks--;

		EA_SOAPOOL_HOOK(kHookTypeMalloc,mnChunkSize);

		#if EA_SOA_EXTENSIONS_ENABLED
			//if there is core block debug data, let it know that we allocated a chunk
			if ( pCurCoreBlock->mpDebugData ) 
			{
				//conversion of nSize to uint32_t is safe since nSize <= mnChunkSize which is uint16_t
				uint32_t nRequestedSize( static_cast<uint32_t>(nSize) );
				ChunkDebugDataAlloc(pCurCoreBlock, pResult, nRequestedSize, allocInfo);
			}
		#endif

		//if this CoreBlock is fully allocated, it's no longer a valid CoreBlock to allocate from,
		//find the next one in the list...
		while ( mpAllocCoreBlock && !mpAllocCoreBlock->mpFreeChunks )
		{
			mpAllocCoreBlock = mpAllocCoreBlock->mpPrevious;
		}

		//NOTE: high water mark recording should happen at the end since, in the case of the call to FreeCachedChunks below,
		//we need to have everything ready since re-entrance may occur.
		#if EA_SOA_EXTENSIONS_ENABLED
			//if there is mpDebugData attached to this this pool, let it know about the event
			if ( mpDebugData )
			{
				mpDebugData->mnNumChunks++;

				//Finally, check for new high water mark (this should be the last thing we do during Alloc, see comments above and below)
				if ( mpDebugData->mnNumChunks > mpDebugData->mHighWaterMarkNumChunks )
				{
					//pooled based is straight forward, if we have a new high mnNumChunks then we have a new high water mark
					if ( kCoreBlockTypePooled == mCoreBlockType )
					{
						//store the new water mark and update our current high water mark stats
						mpDebugData->mHighWaterMarkNumChunks = mpDebugData->mnNumChunks;
						GetStats(mpDebugData->mHighWaterMarkStats);
					}
					else
					{
						//
						//Heap based is less straight forward because we may be funding one or more pools who may be holding onto memory
						//that they don't actually need at the moment. If this is the case then we may not be at a TRUE new high water mark..
						//We really want TRUE high water mark information since it is the only way to accurately know for tunning how much 
						//memory the system really needs. Therefore we have this ... 
						// If we think we see a new high water mark, we first reclaim any memory being retained in pools that isn't actually
						// in use at the moment. Then we see if we have a real high water mark and, if so, record the accurate data.
						// 
						// This feels a little dirty since it is altering behavior when this debug functionality is enabled vs when it's not.
						// However, I don't think it's a real problem since
						//   a. The memory is equally accessible either way, we are just giving up some of the benifit of caching each time this event occurs.
						//   b. This will occur often as the application is starting up and advancing into new areas however, it will become increasingly 
						//      in-frequent and in the interim (between high water marks) caching will be fully functional.
						// It also looks a bit potentially expensive at first glance ... however, the FreeCachedChunks calls during initialization should
						// be cheaper (since nothing will be available to be freed) and shortly, new high water marks will become infrequent. Additionally,
						// this is only being done for kCoreBlockTypeHeap whose Allocations are relatively infrequent compared to Pooled allocations.
						// 
						//
						// The only alternatives I can think of at the moment would be
						//   a. keep a constant running count of how many un-used CoreBlocks are sitting in each of the pools
						//   b. instead of checking mnNumChunks for high water mark for this pool type, each time Alloc occurs count the empty blocks in all pools 
						//        to subtract from mnNumChunks to compare to high water mark.
						// For both cases we'd have to also do some kind of special sauce in GetStats as well to ensure that the stats collected reflect
						// the available memory. This all just seems too painful & expensive.
						// 
						PPM_ASSERT(this, kCoreBlockTypeHeap == mCoreBlockType, kGAErrorInternal, kGAErrorText[kGAErrorInternal], NULL, NULL);

						//free up any un-used pages first so we can include them in our count (and avoid counting a highwater mark 
						// which erroneously includes memory that is actually available but still distributed to pools that used it
						// previously).
						mCallbacks.FreeCachedChunks(); //this may call Free and modify mnNumChunks among other things

						//mnNumChunks may have gotten smaller if any cached chunks were relinquished ... see if we have a true new high water mark
						if (mpDebugData->mnNumChunks > mpDebugData->mHighWaterMarkNumChunks) 
						{
							//store the new water mark and update our current high water mark stats
							mpDebugData->mHighWaterMarkNumChunks = mpDebugData->mnNumChunks;
							GetStats(mpDebugData->mHighWaterMarkStats);
						}
					}
				}
			}
		#endif


		#if EA_SOAPOOL_DIAGNOSTICS
			ValidatePool();
		#endif

		return reinterpret_cast<void*>(pResult);
	}

	//attempt to grow the pool
	Grow();

	if (mpAllocCoreBlock) //if grow yeilded available CoreBlock space
	{
		//we use goto to avoid extra branching in the code above because it is optimized for the most common case
		PPM_ASSERT(this, mpAllocCoreBlock->mpFreeChunks, kGAErrorInternal, kGAErrorText[kGAErrorInternal], NULL, NULL);
		goto AllocCoreBlockFound;
	}

	return NULL;
}

/******************************************************************************/
/*! SmallBlockAllocator::Pool::CoreBlockList::PushFront

	\brief      [Put your brief description here.]

	\param      pCoreBlock -
	\return           - void.

	[Put your optional additional description here.]
*/
/******************************************************************************/
void SmallBlockAllocator::Pool::CoreBlockList::PushFront(SmallBlockAllocator::Pool::CoreBlock* pCoreBlock)
{
	if (mFirst)
	{
		//validate the assumption that these are being inserted only when we're out of space
		//if this assert is firing we just need to update this insert call to find the correct
		//position before inserting instead of assuming first
		//PPM_ASSERT(this, 0 == mFirst->GetNumFreeChunks(), kGAErrorInternal, kGAErrorText[kGAErrorInternal], NULL, NULL);

		InsertBefore(pCoreBlock, mFirst);
		mFirst = pCoreBlock;
	}
	else
	{
		//nothing before us, point to self
		pCoreBlock->mpPrevious = NULL;
		pCoreBlock->mpNext = mFirst;
		mFirst = pCoreBlock;
	}
}

/******************************************************************************/
/*! SmallBlockAllocator::Pool::CoreBlockList::InsertBefore

	\brief      insert the pCoreBlock into the list before the specified CoreBlock

	\param      pCoreBlock     -
	\param      pInsertPos -
	\return               - void.

	[Put your optional additional description here.]
*/
/******************************************************************************/
void SmallBlockAllocator::Pool::CoreBlockList::InsertBefore(SmallBlockAllocator::Pool::CoreBlock* pCoreBlock, SmallBlockAllocator::Pool::CoreBlock* pInsertPos)
{
	pCoreBlock->mpPrevious = pInsertPos->mpPrevious;
	pCoreBlock->mpNext = pInsertPos;
	if (pCoreBlock->mpPrevious)
	{
		pCoreBlock->mpPrevious->mpNext = pCoreBlock;
	}
	else
	{
		//PPM_ASSERT(this, pInsertPos == mFirst, kGAErrorInternal, kGAErrorText[kGAErrorInternal], NULL, NULL);
		mFirst = pCoreBlock;
	}
	pInsertPos->mpPrevious = pCoreBlock;
}

/******************************************************************************/
/*! SmallBlockAllocator::Pool::CoreBlockList::Remove

	\brief      removes this pCoreBlock from the list (may modify mFirst)

	\param      pCoreBlock -
	\return           - void.

	[Put your optional additional description here.]
*/
/******************************************************************************/
void SmallBlockAllocator::Pool::CoreBlockList::Remove(SmallBlockAllocator::Pool::CoreBlock* pCoreBlock)
{
	if (mFirst == pCoreBlock)
	{
		//move first to the next entry in the list
		mFirst = mFirst->mpNext;
		if (mFirst)
		{
			mFirst->mpPrevious = NULL;
		}
	}
	else
	{
		//PPM_ASSERT(this, (pCoreBlock->mpPrevious), kGAErrorInternal, kGAErrorText[kGAErrorInternal], NULL, NULL);
		pCoreBlock->mpPrevious->mpNext = pCoreBlock->mpNext;
	}

	if (pCoreBlock->mpNext)
	{
		pCoreBlock->mpNext->mpPrevious = pCoreBlock->mpPrevious;
	}

	#if (PPM_DEBUG >= 1)
		pCoreBlock->mpNext = NULL;
		pCoreBlock->mpPrevious = NULL;
	#endif
}

/******************************************************************************/
/*! SmallBlockAllocator::Pool::CoreBlockList::AdjustCoreBlockPositionFree

	\brief      Move CoreBlocks MORE likely to be completely freed towards the front
		of the list (i.e. CoreBlocks towards the end have a higher priority for being
		used for allocations since they are less likely to be freed up) this must
		be called any time a CoreBlock mnNumFreeChunks increases to keep the list in order

	\param      pCoreBlock -
	\return           - void.

	[Put your optional additional description here.]
*/
/******************************************************************************/
void SmallBlockAllocator::Pool::CoreBlockList::AdjustCoreBlockPositionFree(SmallBlockAllocator::Pool::CoreBlock* pCoreBlock)
{
	CoreBlock* pCurPos(pCoreBlock);
	//PPM_ASSERT(this, pCurPos->mnNumFreeChunks>0, kGAErrorInternal, kGAErrorText[kGAErrorInternal], NULL, NULL); //free Chunks just increased so we can assume they are greater than 0

	//
	// List ordering notes:
	// 
	// The list is sorted based on how desirable this block is for use for funding allocations (from least desirable to most desirable).
	// This is generally based on how likely we think we are to be able to free the block (if we're not likely to be able to free it then 
	// it should be used for allocations to leave other blocks more space to increase the chances we can free them at some point).
	// 
	// E.g
	// 
	// :BEGIN: ... EMPTY BLOCKS ... PARTIAL BLOCKS ... FULL BLOCKS ... :END:
	//                                                ^
	//                                                 block we're currently allocating from is here, (right before the FULL blocks)
	// 
	// 
	// FULL BLOCKS: no space is left for allocations in these at the moment but they're desirable for future allocations since (being very full at the moment) they seem unlikely to become completely empty any time soon.
	// PARTIAL: These are partially full. Those with the the least Free chunks are sorted farthest to the end (figuring they are less likely to become empty than those with more free space)
	// EMPTY: These are available to give back to the system if needed. They are sorted so that those with more total space are farther towards the end (kind of arbitrary, would it make sense to reverse this?)
	//
	// 
	// Sorting logic:
	//  NOTE1:  we can assume this is not full since we are doing this sort as part of a method that is only supposed to be called when a chunk has been freed (so if we were full, we're not now)
	//  NOTE2:  for the same reasons, we should only have to worry about moving towards the front of the list (since freeing space never would make us move towards the back)
	// 
	bool isEmpty(pCoreBlock->IsEmpty());
	while ( EA_UNLIKELY( pCurPos->mpPrevious && (
			// If the previous block is FULL, shift back (we want full CoreBlocks at the end of the list)
			(!pCurPos->mpPrevious->GetNumFreeChunks()) ||
			// If this is empty,
			//      shift back if previous is not empty (empties should be towards the front)
			//      OR if numChunks < previousNumChunks (logic for sorting among empties ... not sure why, could be the other way around)
			(isEmpty && (!pCurPos->mpPrevious->IsEmpty() || (pCoreBlock->GetNumChunks() < pCurPos->mpPrevious->GetNumChunks()))) ||
			// If this is partial (not empty but not full)
			//      while previous is not empty, shift back only if FreeChunks > previousFreeChunks (logic for sorting among partials ... more free chunks is more likely to be completley freed)
			(!isEmpty && (!pCurPos->mpPrevious->IsEmpty() && pCoreBlock->GetNumFreeChunks() >= pCurPos->mpPrevious->GetNumFreeChunks()))
			) ) )
	{
		pCurPos = pCurPos->mpPrevious;
	}
	if (EA_UNLIKELY(pCurPos != pCoreBlock))
	{
		Remove(pCoreBlock);
		InsertBefore(pCoreBlock, pCurPos);
	}
}

/******************************************************************************/
/*! SmallBlockAllocator::Pool::Free

	\brief      [Put your brief description here.]

	\param      pAddress -
	\return            - void.

	[Put your optional additional description here.]
*/
/******************************************************************************/
void SmallBlockAllocator::Pool::Free(void* pAddress)
{
	PPM_ASSERT(this, ValidateAddress(pAddress, kAddressTypeAllocated), kGAErrorInvalidInput, kGAErrorText[kGAErrorInvalidInput], NULL, NULL);

	CoreBlock* pCurCoreBlock(mCoreBlockList.GetFirst());

	while ( pCurCoreBlock )
	{
		if ( pCurCoreBlock->ContainsMem(pAddress) )
		{
			Free(pCurCoreBlock, pAddress);
			break;
		}
		pCurCoreBlock = pCurCoreBlock->mpNext;
	}
}

/******************************************************************************/
/*! SmallBlockAllocator::Pool::Free

	\brief      [Put your brief description here.]

	\param      pAddress     -
	\param      pOwnerCoreBlock  -
	\return                - void.

	[Put your optional additional description here.]
*/
/******************************************************************************/
void SmallBlockAllocator::Pool::Free(CoreBlock* pOwnerCoreBlock, void* pAddress)
{
	PPM_ASSERT(this, pOwnerCoreBlock && pOwnerCoreBlock->ContainsMem(pAddress), kGAErrorInvalidInput, kGAErrorText[kGAErrorInvalidInput], NULL, NULL);
	PPM_ASSERT(this, pOwnerCoreBlock && (pOwnerCoreBlock->mpOwner == this), kGAErrorInvalidInput, kGAErrorText[kGAErrorInvalidInput], NULL, NULL);

	//adjust the pointer in case it's not pointing to the front of the actual Chunk
	//we find the distance from the end of the CoreBlock to pAddress and % that with Chunk size to find out the offset from the back of the previous Chunk,
	//then we subtract Chunk size from that and add to address to get a pointer to an exact Chunk
	//The +1 (in nOffsetAddress) is a trick to allow this code to work for the case where we actually do have a pointer to the start of a Chunk without needing to check for that case
	const uintptr_t nOffsetAddress = reinterpret_cast<uintptr_t>(pAddress) + 1;
	Chunk* const pFreedChunk = reinterpret_cast<Chunk*>(nOffsetAddress+(((reinterpret_cast<uintptr_t>(pOwnerCoreBlock->End()) - nOffsetAddress) % mnChunkSize) - mnChunkSize));

	#if EA_SOAPOOL_DIAGNOSTICS
		PPM_ASSERT(this, 0 == (reinterpret_cast<uintptr_t>(pOwnerCoreBlock->End())-reinterpret_cast<uintptr_t>(pFreedChunk))%mnChunkSize, kGAErrorInternal, kGAErrorText[kGAErrorInternal], NULL, NULL);
		PPM_ASSERT(this, reinterpret_cast<uint8_t*>(pAddress) >= (reinterpret_cast<uint8_t*>(pFreedChunk)), kGAErrorInternal, kGAErrorText[kGAErrorInternal], NULL, NULL);
		PPM_ASSERT(this, reinterpret_cast<uint8_t*>(pAddress) < (reinterpret_cast<uint8_t*>(pFreedChunk) + mnChunkSize), kGAErrorInternal, kGAErrorText[kGAErrorInternal], NULL, NULL);
		PPM_ASSERT(this, !pOwnerCoreBlock->IsFree(reinterpret_cast<uint8_t*>(pFreedChunk)), kGAErrorBadUserPtr, kGAErrorText[kGAErrorBadUserPtr], NULL, NULL);
	#endif

	pFreedChunk->mpNextFreeChunk = pOwnerCoreBlock->mpFreeChunks;
	pOwnerCoreBlock->mpFreeChunks = pFreedChunk;
	pOwnerCoreBlock->mnNumFreeChunks++;

	EA_SOAPOOL_HOOK(kHookTypeFree,mnChunkSize);

	#if EA_SOA_EXTENSIONS_ENABLED
		//if there is core block debug data, let it know that we freed a chunk
		if ( pOwnerCoreBlock->mpDebugData ) 
		{
			ChunkDebugDataFree(pOwnerCoreBlock, reinterpret_cast<ChunkRef>(pFreedChunk));
		}

		//if there is mpDebugData attached to this pool, let it know about this event
		if ( mpDebugData )
		{
			PPM_ASSERT(this, mpDebugData->mnNumChunks > 0, kGAErrorInternal, kGAErrorText[kGAErrorInternal], NULL, NULL);
			mpDebugData->mnNumChunks--;
		}
	#endif

	//keep the list sorted
	mCoreBlockList.AdjustCoreBlockPositionFree(pOwnerCoreBlock);

	//if there was no valid alloc CoreBlock, this one now has space to alloc from
	if ( !mpAllocCoreBlock )
	{
		mpAllocCoreBlock = pOwnerCoreBlock;
		PPM_ASSERT(this, !mpAllocCoreBlock || mpAllocCoreBlock->mpFreeChunks, kGAErrorInternal, kGAErrorText[kGAErrorInternal], NULL, NULL);
	}
	else
	{
		//else, if there is a better qualifed CoreBlock to be alloc CoreBlock with the new sort, find it
		//we want to allocate from the CoreBlock with any free Chunks that's closest to the end of the
		//list since CoreBlocks we feel are least likely to be completely freed up are sorted towards
		//the end of the list
		while ( mpAllocCoreBlock->mpNext && (mpAllocCoreBlock->mpNext->mnNumFreeChunks > 0) )
		{
			//the list should be ordered with empties towards the front, then by available free chunks (most to least)
			PPM_ASSERT(this, mpAllocCoreBlock->IsEmpty() || (mpAllocCoreBlock->mpNext->mnNumFreeChunks <= mpAllocCoreBlock->mnNumFreeChunks), kGAErrorInternal, kGAErrorText[kGAErrorInternal], NULL, NULL);
			mpAllocCoreBlock = mpAllocCoreBlock->mpNext;
		}
		PPM_ASSERT(this, !mpAllocCoreBlock || mpAllocCoreBlock->mpFreeChunks, kGAErrorInternal, kGAErrorText[kGAErrorInternal], NULL, NULL);
	}

	#if PPM_DEBUG_FILL && (PPM_DEBUG >= 2)
		if (GetChunkSize()>sizeof(Chunk))
		{
			//TODO; support filling with a specific value and checking against it?
			DebugMemFill(reinterpret_cast<uint8_t*>(pFreedChunk) + sizeof(Chunk), 0, GetChunkSize()-sizeof(Chunk));
		}
	#endif

	//check if we should free this pCoreBlock
	//we only decide to free CoreBlocks here if auto shrink is TRUE and if the CoreBlock is completely empty
	//Also, let's not auto shrink if this CoreBlock happens to be the current CoreBlock we're making allocations from
	//to try to avoid an edge case where we keep shrinking than re-growing constantly (the side-affect is that we
	//will never 'AutoShrink' down to zero core blocks ... we stop at 1).
	if ( mAutoShrink && pOwnerCoreBlock->IsEmpty() && (mpAllocCoreBlock != pOwnerCoreBlock) )
	{
		//this CoreBlock is no longer needed, kill it
		Shrink(pOwnerCoreBlock);
	}

	#if EA_SOAPOOL_DIAGNOSTICS
		ValidatePool();
	#endif
}

/******************************************************************************/
/*! SmallBlockAllocator::Pool::GetNestedCoreBlockFromMem

	\brief      [Put your brief description here.]

	\param      pAddress -
	\return            -

	[Put your optional additional description here.]
*/
/******************************************************************************/
SmallBlockAllocator::Pool::CoreBlock* SmallBlockAllocator::Pool::GetNestedCoreBlockFromMem(const void* pAddress) const
{
	CoreBlock* pCurCoreBlock(mCoreBlockList.GetFirst());

	//return CoreBlock if pAddress lies in any of our CoreBlocks
	while ( pCurCoreBlock )
	{
		if ( pCurCoreBlock->ContainsMem(pAddress) )
		{
			//this code assumes that pAddress is pointing to somewhere within the Chunk and not right to the front of the Chunk.
			//this assumption should be safe because the Chunk is actually a nested CoreBlock so the front of the Chunk is actually the CoreBlock header
			//which is something users shouldn't have access to and shouldn't be freeing.
			const uintptr_t nOffsetFromChunkStart = mnChunkSize - ( (reinterpret_cast<uintptr_t>(pCurCoreBlock->End()) - reinterpret_cast<uintptr_t>(pAddress)) % mnChunkSize );
			CoreBlock* pNestedCoreBlock = reinterpret_cast<CoreBlock*>(reinterpret_cast<uintptr_t>(pAddress)-nOffsetFromChunkStart);
			PPM_ASSERT(this, pNestedCoreBlock->mnSize == mnChunkSize, kGAErrorInternal, kGAErrorText[kGAErrorInternal], NULL, NULL);

			//if our assumptions about pAddress not pointing to the front of the Chunk (or nested CoreBlock) are incorrect, this assert will trigger
			//because the above code will give us a pointer to the Chunk in front of the correct Chunk
			PPM_ASSERT(this, pNestedCoreBlock->ContainsMem(pAddress), kGAErrorInternal, kGAErrorText[kGAErrorInternal], NULL, NULL);

			return pNestedCoreBlock;
		}
		pCurCoreBlock = pCurCoreBlock->mpNext;
	}
	return NULL;
}

/******************************************************************************/
/*! SmallBlockAllocator::Pool::NestedCoreBlockFree

	\brief      [Put your brief description here.]

	\param      pAddress -
	\return            -

	[Put your optional additional description here.]
*/
/******************************************************************************/
bool SmallBlockAllocator::Pool::NestedCoreBlockFree(void* pAddress)
{
	SmallBlockAllocator::Pool::CoreBlock* pNestedCoreBlock(GetNestedCoreBlockFromMem(pAddress));

	if (pNestedCoreBlock)
	{
		//free the pointer from the pool that owns this CoreBlock...
		//since we already know the CoreBlock that contains the memory
		//call the version of free that accepts a CoreBlock pointer so
		//we can skip having to search for the CoreBlock that owns pAddress
		pNestedCoreBlock->mpOwner->Free(pNestedCoreBlock, pAddress);
		return true;
	}
	return false;
}

/******************************************************************************/
/*! SmallBlockAllocator::Pool::NestedCoreBlockGetUsableSize

	\brief      [Put your brief description here.]

	\param      pAddress -
	\return              -

	[Put your optional additional description here.]
*/
/******************************************************************************/
size_t SmallBlockAllocator::Pool::NestedCoreBlockGetUsableSize(const void* pAddress) const
{
	size_t nUsableSize = (size_t)-1;

	SmallBlockAllocator::Pool::CoreBlock* pNestedCoreBlock(GetNestedCoreBlockFromMem(pAddress));

	if (pNestedCoreBlock)
	{
		nUsableSize = pNestedCoreBlock->GetChunkSize();

		size_t nOffset = (pNestedCoreBlock->End() - reinterpret_cast<const uint8_t*>(pAddress))%nUsableSize;

		if (nOffset)
		{
			nUsableSize = nOffset;
		}
	}
	return nUsableSize;
}

/******************************************************************************/
/*! SmallBlockAllocator::Pool::NestedCoreBlockGetAllocationCost

	\brief      calculate the cost of an allocation from a nested core block

	\param      pAddress -
	\param      nAllocationCostFlags - flags indicating the costs to calculate
	\return              - the cost OR 0.0f (if not a valid address or flags are not supplied)

		See NestedCoreBlockFree comments for info and assumptions regarding NestedCoreBlocks.
		This method is for debugging/statistics gathering only to see how much memory allocations cost (the size returned is NOT useable size)
		See AllocationCost for details on the costs that can be calculated.
		If the address does not belong to this pool (or no flags are passed in) then 0.0f is returned.
*/
/******************************************************************************/
float SmallBlockAllocator::Pool::NestedCoreBlockGetAllocationCost(const void* pAddress, int nAllocationCostFlags ) const
{
	float fTotalCost = 0.0f;

	SmallBlockAllocator::Pool::CoreBlock* pNestedCoreBlock(GetNestedCoreBlockFromMem(pAddress));

	if (pNestedCoreBlock)
	{
		if (AllocationCostChunkSize == (AllocationCostChunkSize & nAllocationCostFlags) ) 
		{
			fTotalCost += static_cast<float>(pNestedCoreBlock->GetChunkSize());
		}

		if (AllocationCostHeader == (AllocationCostHeader & nAllocationCostFlags) ) 
		{
			fTotalCost += (static_cast<float>(CoreBlockOverhead()) / static_cast<float_t>(pNestedCoreBlock->GetNumChunks()));
		}

		if (AllocationCostWaste == (AllocationCostWaste & nAllocationCostFlags) ) 
		{
			fTotalCost +=  (static_cast<float>(pNestedCoreBlock->GetWastedSpace()) / static_cast<float_t>(pNestedCoreBlock->GetNumChunks()));
		}
	}
	return fTotalCost;
}

/******************************************************************************/
/*! SmallBlockAllocator::Pool::NestedCoreBlockGetPool

	\brief      [Put your brief description here.]

	\param      pAddress -
	\return            -

	[Put your optional additional description here.]
*/
/******************************************************************************/
SmallBlockAllocator::Pool* SmallBlockAllocator::Pool::NestedCoreBlockGetPool(const void* pAddress)
{
	SmallBlockAllocator::Pool::CoreBlock* pNestedCoreBlock(GetNestedCoreBlockFromMem(pAddress));

	return (pNestedCoreBlock ? pNestedCoreBlock->mpOwner : NULL);
}

/******************************************************************************/
/*! SmallBlockAllocator::Pool::Grow

	\brief      if mHeapParams.mnGrowChunkCount != 0, adds mHeapParams.mnGrowChunkCount Chunks to the free
				list for subsequent allocation

	\param                      - none
	\return     bool         -

	[Put your optional additional description here.]
*/
/******************************************************************************/
SmallBlockAllocator::Pool::CoreBlock* SmallBlockAllocator::Pool::Grow()
{
	//allocate a CoreBlock and find out how big it is
	uint32_t nCoreBlockSize(0);
	CoreBlock* pNewCoreBlock = reinterpret_cast<CoreBlock*>(AllocateCoreBlock(nCoreBlockSize));
	if ( pNewCoreBlock )
	{
		PPM_ASSERT(this, nCoreBlockSize >= (sizeof(CoreBlock)), kGAErrorInternal, kGAErrorText[kGAErrorInternal], NULL, NULL);  //must be at least enough space for the CoreBlock header

		//find out how many Chunks we can create out of each CoreBlock (leave room for the CoreBlock header in the CoreBlock!)
		const uint32_t nGrowChunkCount( (nCoreBlockSize-(CoreBlockOverhead())) / mnChunkSize );
		//find out how much wasted space will remain in the CoreBlock after we put in the header and the Chunks
		const uint32_t nWastedSpace( (nCoreBlockSize - (nGrowChunkCount*mnChunkSize))-(CoreBlockOverhead()) );
		//check the math
		PPM_ASSERT(this, ((mnChunkSize*nGrowChunkCount)+CoreBlockOverhead())+nWastedSpace == nCoreBlockSize, kGAErrorInternal, kGAErrorText[kGAErrorInternal], NULL, NULL);

		#if PPM_DEBUG_FILL && (PPM_DEBUG >= 2)
			uint8_t* pCoreBlockStart = reinterpret_cast<uint8_t*>(pNewCoreBlock);
			DebugMemFill(pCoreBlockStart, 0, nCoreBlockSize);
		#endif

		pNewCoreBlock->mpOwner = this;
		pNewCoreBlock->mnSize = nCoreBlockSize;
		pNewCoreBlock->mnNumFreeChunks = 0;
		pNewCoreBlock->mnNumChunks = 0;
		pNewCoreBlock->mpFreeChunks = NULL;
		pNewCoreBlock->mpDebugData = NULL;

		uint8_t* pCurOffset = pNewCoreBlock->End(); //point just past the last 'Chunk'
		const uint8_t* pFirstChunk = pNewCoreBlock->mData + nWastedSpace;
		Chunk* pPrevChunk(NULL); //point the last new Chunk to NULL
		Chunk* pCurChunk(NULL);

		uint16_t nChunkCount(0);
		//build a linked list of free Chunks
		while ( pCurOffset > pFirstChunk )
		{
			pCurOffset -= mnChunkSize;

			//treat this as a Chunk, point to the previous Chunk and update pPrevChunk to point to this
			pCurChunk = reinterpret_cast<Chunk*>(pCurOffset);
			pCurChunk->mpNextFreeChunk = pPrevChunk;
			pPrevChunk = pCurChunk;

			nChunkCount++;
		}
		PPM_ASSERT(this, pCurOffset == pFirstChunk, kGAErrorInternal, kGAErrorText[kGAErrorInternal], NULL, NULL);

		pNewCoreBlock->mnNumChunks = nChunkCount;
		pNewCoreBlock->mnNumFreeChunks = nChunkCount;
		pNewCoreBlock->mpFreeChunks = pCurChunk;

		//insert into our CoreBlock list
		mCoreBlockList.PushFront(pNewCoreBlock);

		PPM_ASSERT(this, nGrowChunkCount==nChunkCount, kGAErrorInternal, kGAErrorText[kGAErrorInternal], NULL, NULL);

		#if EA_SOA_EXTENSIONS_ENABLED
			if (mChunkStats) 
			{
				CoreBlockDebugDataAlloc(pNewCoreBlock);
			}

			if ( mpDebugData )
			{
				mpDebugData->mnNumGrows++;
			}
		#endif

		//if we don't already have a current CoreBlock to alloc from, use this one.
		//currently we assume we only grow when we have to so we're assuming that mpAllocCoreBlock == NULL instead of checking
		PPM_ASSERT(this,  !mpAllocCoreBlock, kGAErrorInternal, kGAErrorText[kGAErrorInternal], NULL, NULL);
		mpAllocCoreBlock = pNewCoreBlock;
	}

	#if EA_SOAPOOL_DIAGNOSTICS
		ValidatePool();
	#endif

	return ( pNewCoreBlock );
}

/******************************************************************************/
/*! SmallBlockAllocator::Pool::Shrink

	\brief      search the CoreBlock list and free any CoreBlocks that we aren't still using

	\param       - none.
	\return          - void.


*/
/******************************************************************************/
void SmallBlockAllocator::Pool::Shrink()
{
	CoreBlock* pCurCoreBlock = mCoreBlockList.GetFirst();
	CoreBlock* pDelCoreBlock(NULL);

	//the list has empties at the front
	while (pCurCoreBlock && pCurCoreBlock->IsEmpty())
	{
		//if we're about to wipe out the alloc CoreBlock, set it to NULL
		if (mpAllocCoreBlock == pCurCoreBlock)
		{
			mpAllocCoreBlock = NULL;
		}

		//store pointer to the one we want to remove
		pDelCoreBlock = pCurCoreBlock;

		//advance cur CoreBlock to the next CoreBlock that we need to check
		pCurCoreBlock = pCurCoreBlock->mpNext;

		//shrink away the empty CoreBlock...
		Shrink(pDelCoreBlock);
	}
}

/******************************************************************************/
/*! SmallBlockAllocator::Pool::Shrink

	\brief      [Put your brief description here.]

	\param       - none.
	\return          - void.

	[Put your optional additional description here.]
*/
/******************************************************************************/
void SmallBlockAllocator::Pool::Shrink(CoreBlock* pCoreBlock)
{
	PPM_ASSERT(this, pCoreBlock->mnNumFreeChunks == pCoreBlock->mnNumChunks, kGAErrorInternal, kGAErrorText[kGAErrorInternal], NULL, NULL);
	PPM_ASSERT(this, pCoreBlock != mpAllocCoreBlock, kGAErrorInternal, kGAErrorText[kGAErrorInternal], NULL, NULL);

	//remove this CoreBlock from the list...
	mCoreBlockList.Remove(pCoreBlock);

	#if EA_SOA_EXTENSIONS_ENABLED
		CoreBlockDebugDataFree(pCoreBlock);
		if ( mpDebugData ) //indicates whether we track debug data
		{
			mpDebugData->mnNumShrinks++;
		}
	#endif

	//delete the pCoreBlock
	FreeCoreBlock(pCoreBlock);

	#if EA_SOAPOOL_DIAGNOSTICS
		ValidatePool();
	#endif
}

/******************************************************************************/
/*! SmallBlockAllocator::Pool::AllocateCoreBlock

	\brief      Allocates a new Core Block of the given requested size.

	\param      nCoreBlockSize - size in bytes.
	\return     pointer to new block. May be NULL if the Core Block couldn't be allocated.

	This function does not internally assert if the value to be returned is NULL.
*/
/******************************************************************************/
void* SmallBlockAllocator::Pool::AllocateCoreBlock(uint32_t &nCoreBlockSize)
{
	if ( kCoreBlockTypePooled == mCoreBlockType )
	{
		//we're using Chunks from another pool as our CoreBlocks
		nCoreBlockSize = mpCoreBlockPool->GetChunkSize();
		void* pNewCoreBlock = mpCoreBlockPool->Alloc(nCoreBlockSize);

		EA_SOAPOOL_HOOK(kHookTypeGrowPooled,nCoreBlockSize);

		return pNewCoreBlock;
	}
	else
	{
		PPM_ASSERT(this, kCoreBlockTypeHeap == mCoreBlockType, kGAErrorInternal, kGAErrorText[kGAErrorInternal], NULL, NULL);
		PPM_ASSERT(this, IsFull(), kGAErrorInternal, kGAErrorText[kGAErrorInternal], NULL, NULL);

		//try to free up any un-used pages first so we can re-use them if they are available (and avoid growing)
		mCallbacks.FreeCachedChunks();

		//grow only if mHeapParams.mnGrowChunkCount is specified and if the pool is still full
		//if the pool is no longer full we can just return NULL and the caller should recognize
		//that Chunks are now available again
		if (IsFull())
		{
			if (mHeapParams.mnGrowChunkCount)
			{
				const uint16_t chunkSize = GetChunkSize();
				const uint32_t coreBlockOverhead = CoreBlockOverhead();
				nCoreBlockSize = ((chunkSize * mHeapParams.mnGrowChunkCount) + coreBlockOverhead);
				void* pNewCoreBlock = mCallbacks.CoreAlloc(nCoreBlockSize, mHeapParams.mnMinCoreBlockAlign, coreBlockOverhead);

				//the address just past the header must be aligned as requested. If this assert fires, perhaps mAllocator does not support alignment or alignmentOffset?
				PPM_ASSERT(this,  0 == (( reinterpret_cast<uintptr_t>(pNewCoreBlock) + coreBlockOverhead) % mHeapParams.mnMinCoreBlockAlign), kGAErrorInternal, kGAErrorText[kGAErrorInternal], NULL, NULL);

				EA_SOAPOOL_HOOK(kHookTypeGrowHeap,nCoreBlockSize);

				return pNewCoreBlock;
			}
			else
			{
				EA_SOAPOOL_HOOK(kHookTypeGrowHeapDisabled,nCoreBlockSize);
			}
		}
	}
	return NULL;
}

/******************************************************************************/
/*! SmallBlockAllocator::Pool::FreeCoreBlock

	\brief      [Put your brief description here.]

	\param      pCoreBlock -
	\return           - void.

	[Put your optional additional description here.]
*/
/******************************************************************************/
void SmallBlockAllocator::Pool::FreeCoreBlock(void* pCoreBlock)
{
	if ( kCoreBlockTypePooled == mCoreBlockType )
	{
		EA_SOAPOOL_HOOK(kHookTypeShrinkPooled,0);
		mpCoreBlockPool->Free(pCoreBlock);
	}
	else
	{
		PPM_ASSERT(this, kCoreBlockTypeHeap == mCoreBlockType, kGAErrorInternal, kGAErrorText[kGAErrorInternal], NULL, NULL);
		EA_SOAPOOL_HOOK(kHookTypeShrinkHeap,0);
		mCallbacks.CoreFree(pCoreBlock);
	}
}

/******************************************************************************/
/*! SmallBlockAllocator::Pool::CoreBlock::IsFree

	\brief      returns true if the specified Chunk is free
				this method is not optimized but it's only expected to be used
				by diagnostic/reporting code

	\param      pAddress -
	\return            -

	[Put your optional additional description here.]
*/
/******************************************************************************/
bool SmallBlockAllocator::Pool::CoreBlock::IsFree(const void* pAddress) const
{
	for (const Chunk* pCurChunk = mpFreeChunks; pCurChunk != NULL; pCurChunk = pCurChunk->mpNextFreeChunk )
	{
		if (pCurChunk == pAddress)
		{
			return true;
		}
	}
	return false;
}

/******************************************************************************/
/*! SmallBlockAllocator::Pool::GetStats

	\brief      [Put your brief description here.]

	\param      stats -
	\return           - void.

	[Put your optional additional description here.]
*/
/******************************************************************************/
bool SmallBlockAllocator::Pool::GetStats(SmallBlockAllocator::PoolInfo& stats) const
{
	uint32_t nCoreBlockTotalSize(0);
	uint32_t nCoreBlockTotalChunks(0);
	uint32_t nCoreBlockTotalFreeChunks(0);
	uint32_t nCoreBlockTotalWaste(0);

	//iterate through each CoreBlock and accumulate some stats about this pool
	CoreBlock* pCurCoreBlock(mCoreBlockList.GetFirst());
	uint32_t nCoreBlockCount(0);
	uint32_t nCoreBlockEmptyCount(0);
	while ( pCurCoreBlock )
	{
		nCoreBlockTotalSize += pCurCoreBlock->mnSize;
		nCoreBlockTotalChunks += pCurCoreBlock->mnNumChunks;
		nCoreBlockTotalFreeChunks += pCurCoreBlock->mnNumFreeChunks;

		PPM_ASSERT(this, ((pCurCoreBlock->mnNumChunks * this->mnChunkSize)+CoreBlockOverhead()) <= pCurCoreBlock->mnSize, kGAErrorInternal, kGAErrorText[kGAErrorInternal], NULL, NULL);  //something's wrong with our calculation?
		//calculate the wasted space in this CoreBlock (wasted space is due to the fact that Chunks don't always divide evenly into CoreBlock size-CoreBlockhdr size
		nCoreBlockTotalWaste += pCurCoreBlock->GetWastedSpace();
		nCoreBlockEmptyCount += (pCurCoreBlock->IsEmpty() ? 1 : 0);

		nCoreBlockCount++;
		pCurCoreBlock = pCurCoreBlock->mpNext;
	}

	//record the stats
	stats.mnCoreBlockCount = nCoreBlockCount;
	stats.mnChunkCount = nCoreBlockTotalChunks;
	stats.mnFreeChunkCount = nCoreBlockTotalFreeChunks;
	stats.mnChunkSize = this->mnChunkSize;
	stats.mnCoreBlockWasteTotal = nCoreBlockTotalWaste;
	stats.mnCoreBlockSizeTotal = nCoreBlockTotalSize;
	stats.mnCoreBlockEmptyCount = nCoreBlockEmptyCount;

	#if EA_SOA_EXTENSIONS_ENABLED
	if (mpDebugData)
	{
		stats.mnNumGrows = mpDebugData->mnNumGrows;
		stats.mnNumShrinks = mpDebugData->mnNumShrinks;
	}
	else
	#endif
	{
		stats.mnNumGrows = static_cast<uint32_t>(-1);
		stats.mnNumShrinks = static_cast<uint32_t>(-1);
	}

	return true;
}

/******************************************************************************/
/*! GetHighWaterMarkStats

	\brief      [Put your brief description here.]

	\param      stats -
	\return           -

	[Put your optional additional description here.]
*/
/******************************************************************************/
bool SmallBlockAllocator::Pool::GetHighWaterMarkStats(SmallBlockAllocator::PoolInfo& stats) const
{
	#if EA_SOA_EXTENSIONS_ENABLED
		if (mpDebugData)
		{
			stats = mpDebugData->mHighWaterMarkStats;
		}
	#else
		EA_UNUSED(stats);
	#endif
	return (mpDebugData != NULL);
}

/******************************************************************************/
/*! SmallBlockAllocator::Pool::ReportBegin

	\brief      [Put your brief description here.]

	\param      pReportContext  -
	\param      nNodeTypeFlags -
	\return                    -

	[Put your optional additional description here.]
*/
/******************************************************************************/
SmallBlockAllocator::ReportContext* SmallBlockAllocator::Pool::ReportBegin(SmallBlockAllocator::ReportContext* pReportContext, uint32_t nNodeTypeFlags)
{
	memset(pReportContext, 0, sizeof(ReportContext));

	pReportContext->mNodeTypeFlags = nNodeTypeFlags;

	return pReportContext;
}

/******************************************************************************/
/*! SmallBlockAllocator::Pool::ReportNext

	\brief      [Put your brief description here.]

	\param      pReportContext -
	\param      nNodeTypeFlags -
	\return                    -

	[Put your optional additional description here.]
*/
/******************************************************************************/
const SmallBlockAllocator::ReportNode* SmallBlockAllocator::Pool::ReportNext(SmallBlockAllocator::ReportContext* pReportContext, uint32_t nNodeTypeFlags)
{
	PPM_ASSERT(this, sizeof(ReportContextInternal) == sizeof(ReportContext), kGAErrorInternal, kGAErrorText[kGAErrorInternal], NULL, NULL); // If this fails then we didn't keep ReportContext and ReportContextInternal in sync.

	// Used to handle pointer aliasing. This actually isn't the right way to do it; you really need to make a union of the types and not pointers to the types.
	union ReportContextU
	{
		ReportContext*         mRC;
		ReportContextInternal* mRCI;
	} rcu = { pReportContext };

	Pool*&      pCurPool      = rcu.mRCI->mPool;
	CoreBlock*& pCurCoreBlock = rcu.mRCI->mCoreBlock;
	uint8_t*&   pChunk        = rcu.mRCI->mChunk;

	nNodeTypeFlags &= rcu.mRCI->mNodeTypeFlags;

	if (!pCurPool)
	{
		pCurPool = this;
		pCurCoreBlock = mCoreBlockList.GetFirst();

		if (nNodeTypeFlags & kNodeTypePool)
		{
			rcu.mRCI->mNode.mNodeType = kNodeTypePool;
			GetStats(rcu.mRCI->mNode.mPoolInfo);
			return &rcu.mRCI->mNode;
		}
	}

	for ( ; pCurCoreBlock ; pCurCoreBlock = pCurCoreBlock->mpNext, pChunk = NULL )
	{
		if (!pChunk)
		{
			//point to the first Chunk in the CoreBlock
			pChunk = pCurCoreBlock->GetFirstChunk();

			if (nNodeTypeFlags & kNodeTypeCoreBlock)
			{
				rcu.mRCI->mNode.mNodeType = kNodeTypeCoreBlock;

				rcu.mRCI->mNode.mCoreBlockInfo.mCoreBlock      = reinterpret_cast<CoreBlockRef>(pCurCoreBlock);
				rcu.mRCI->mNode.mCoreBlockInfo.mnSize          = pCurCoreBlock->mnSize;
				rcu.mRCI->mNode.mCoreBlockInfo.mnChunkSize     = this->mnChunkSize;
				rcu.mRCI->mNode.mCoreBlockInfo.mnNumChunks     = pCurCoreBlock->GetNumChunks();
				rcu.mRCI->mNode.mCoreBlockInfo.mnNumFreeChunks = pCurCoreBlock->GetNumFreeChunks();

				return &rcu.mRCI->mNode;
			}
		}

		//skip all chunk processing if it's not required ...
		if (0 != (nNodeTypeFlags & (kNodeTypeChunkAllocated | kNodeTypeChunkFree)))
		{
			for ( ; pChunk < pCurCoreBlock->End() ; pChunk += this->mnChunkSize )
			{
				bool ChunkIsFree(pCurCoreBlock->IsFree(pChunk));

				//if Chunk matches nNodeTypeFlags, report it...
				if ( ( (nNodeTypeFlags & (kNodeTypeChunkAllocated | kNodeTypeChunkFree)) == (kNodeTypeChunkAllocated | kNodeTypeChunkFree) ) ||
					 ( (nNodeTypeFlags & kNodeTypeChunkAllocated) && !ChunkIsFree ) ||
					 ( (nNodeTypeFlags & kNodeTypeChunkFree) && ChunkIsFree ) )
				{
					rcu.mRCI->mNode.mNodeType = ChunkIsFree ? kNodeTypeChunkFree : kNodeTypeChunkAllocated;

					rcu.mRCI->mNode.mChunkInfo.mCoreBlock      = reinterpret_cast<CoreBlockRef>(pCurCoreBlock);
					rcu.mRCI->mNode.mChunkInfo.mnCoreBlockSize = pCurCoreBlock->mnSize;
					rcu.mRCI->mNode.mChunkInfo.mChunk          = reinterpret_cast<ChunkRef>(pChunk);
					rcu.mRCI->mNode.mChunkInfo.mnChunkSize     = mnChunkSize;

					ChunkDebugData* pDebugData(NULL);

					#if EA_SOA_EXTENSIONS_ENABLED
					{
						//if we have debug data show additional info (only if !ChunkIsFree because free Chunks don't have any debug data)
						if ( !ChunkIsFree && pCurCoreBlock->mpDebugData )
						{
							const intptr_t nChunkIndex( pCurCoreBlock->mnNumChunks - ((pCurCoreBlock->End() - ((pChunk))) / mnChunkSize) );

							pDebugData = pCurCoreBlock->mpDebugData->mChunkData[nChunkIndex];
						}

						if (pDebugData)
						{
							rcu.mRCI->mNode.mChunkInfo.mpAllocName     = pDebugData->mAllocName;
							rcu.mRCI->mNode.mChunkInfo.mpAllocFile     = pDebugData->mpAllocFile;
							rcu.mRCI->mNode.mChunkInfo.mnAllocLine     = pDebugData->mnAllocLine;
							rcu.mRCI->mNode.mChunkInfo.mnGroupId       = pDebugData->mnGroupId;
							rcu.mRCI->mNode.mChunkInfo.mnRequestedSize = pDebugData->mnRequestedSize;
						}
					}
					#endif

					if (!pDebugData)
					{
						rcu.mRCI->mNode.mChunkInfo.mpAllocName     = "";
						rcu.mRCI->mNode.mChunkInfo.mpAllocFile     = "";
						rcu.mRCI->mNode.mChunkInfo.mnAllocLine     = 0;
						rcu.mRCI->mNode.mChunkInfo.mnGroupId       = 0;
						rcu.mRCI->mNode.mChunkInfo.mnRequestedSize = mnChunkSize;
					}

					//update the report context for the next Chunk...
					pChunk += this->mnChunkSize;

					return &rcu.mRCI->mNode;
				}
			}
		}
	}
	return NULL; //nothing else to report
}

/******************************************************************************/
/*! SmallBlockAllocator::Pool::ReportEnd

	\brief      [Put your brief description here.]

	\param      pReportContext -
	\return                   - void.

	[Put your optional additional description here.]
*/
/******************************************************************************/
void SmallBlockAllocator::Pool::ReportEnd(SmallBlockAllocator::ReportContext* pReportContext)
{
	// We need do nothing significant in this implementation.
	// But we may need to do something in the future.
	memset(pReportContext, 0, sizeof(ReportContext));
}

/******************************************************************************/
/*! SmallBlockAllocator::Pool::TraceStats

	\brief      [Put your brief description here.]

	\return                           - void.

	[Put your optional additional description here.]
*/
/******************************************************************************/
void SmallBlockAllocator::Pool::TraceStats(SmallBlockAllocator::TraceCallback* pTraceCallback)
{
	if (!pTraceCallback)
	{
		pTraceCallback = &(mCallbacks.mTrace);
	}

	PoolInfo stats;
	GetStats(stats);

	pTraceCallback->Tracef("Pool stats (%d byte Chunks)\n"
					"  Num CoreBlocks = %d (%d bytes)\n"
					"  Total Chunks = %d (%d bytes)\n"
					"  Free Chunks = %d (%d bytes)\n"
					"  Total Waste = %d bytes\n"
					"  Avg Overhead per Chunk = %.2f bytes ((Total Waste + CoreBlock overhead)/Total Chunks)\n",
					stats.ChunkSize(), stats.CoreBlockCount(), stats.CoreBlockSizeTotal(), stats.ChunkCount(),
					stats.ChunkSizeTotal(), stats.FreeChunkCount(), stats.FreeChunkSizeTotal(), stats.CoreBlockWasteTotal(), stats.PerChunkOverhead());

	#if EA_SOA_EXTENSIONS_ENABLED
		//if we have debug data include additional details
		if ( mpDebugData )
		{
			pTraceCallback->Tracef("  Grows = %d\n  Shrinks = %d\n", mpDebugData->mnNumGrows, mpDebugData->mnNumShrinks);
		}
	#endif
}

/******************************************************************************/
/*! SmallBlockAllocator::Pool::TraceMemory

	\brief      [Put your brief description here.]

	\param      nNodeTypeFlags -
	\return                             - void.

	[Put your optional additional description here.]
*/
/******************************************************************************/
void SmallBlockAllocator::Pool::TraceMemory(SmallBlockAllocator::TraceCallback* pTraceCallback, uint32_t nNodeTypeFlags)
{
	if (!pTraceCallback)
	{
		pTraceCallback = &(mCallbacks.mTrace);
	}

	//if there isn't at least one CoreBlock in the list there is nothing to do
	if ( mCoreBlockList.GetFirst() )
	{
		SmallBlockAllocator::ReportContext reportContext;
		const ReportNode* pCurNode;

		if (kNodeTypeCoreBlock & nNodeTypeFlags)
		{
			//print summary of CoreBlocks...
			pTraceCallback->Trace("  CoreBlock Summary:\n");

			//iterate through each CoreBlock
			uint32_t nCoreBlockIndex(0);
			uint32_t nCoreBlockChunkBytes(0);
			uint32_t nCoreBlockFreeBytes(0);
			ReportBegin(&reportContext, kNodeTypeCoreBlock); //CoreBlock report
			for ( pCurNode = ReportNext(&reportContext,kNodeTypeCoreBlock) ; pCurNode != NULL; pCurNode = ReportNext(&reportContext,kNodeTypeCoreBlock) )
			{
				PPM_ASSERT(this,  kNodeTypeCoreBlock & pCurNode->mNodeType , kGAErrorInternal, kGAErrorText[kGAErrorInternal], NULL, NULL);
				const CoreBlockInfo* pCurCoreBlock = &(pCurNode->mCoreBlockInfo);

				nCoreBlockFreeBytes = (pCurCoreBlock->mnNumFreeChunks * pCurCoreBlock->mnChunkSize);
				nCoreBlockChunkBytes = (pCurCoreBlock->mnNumChunks * pCurCoreBlock->mnChunkSize);
				const char* tag = (static_cast<const void*>(pCurCoreBlock) == static_cast<const void*>(this->mpAllocCoreBlock) ? "*" : " "); //tag the active alloc core block
				pTraceCallback->Tracef("   CoreBlock %d [0x" _EA_SOAPOOL_PTRFMT_ " to 0x" _EA_SOAPOOL_PTRFMT_ "]-> %d free Chunks (%d bytes) out of %d Chunks (%d bytes)%s\n", nCoreBlockIndex, pCurCoreBlock->Begin(), pCurCoreBlock->End(), pCurCoreBlock->mnNumFreeChunks, nCoreBlockFreeBytes, pCurCoreBlock->mnNumChunks, nCoreBlockChunkBytes, tag);
				nCoreBlockIndex++;
			}
			ReportEnd(&reportContext);
		}

		if ( (kNodeTypeChunkAllocated | kNodeTypeChunkFree) & nNodeTypeFlags)
		{
			//clear any other bits in nNodeTypeFlags other than free and allocated Chunks
			nNodeTypeFlags &= (kNodeTypeChunkAllocated | kNodeTypeChunkFree);

			//report all Chunks
			pTraceCallback->Trace("Chunk dump:\nAddress\t\tSize\tRealSize\tName\tGroup\tCaller\n");
			ReportBegin(&reportContext, nNodeTypeFlags);
			for ( pCurNode = ReportNext(&reportContext, nNodeTypeFlags) ; pCurNode != NULL ; pCurNode = ReportNext(&reportContext, nNodeTypeFlags) )
			{
				PPM_ASSERT(this,  (kNodeTypeChunkAllocated | kNodeTypeChunkFree) & pCurNode->mNodeType , kGAErrorInternal, kGAErrorText[kGAErrorInternal], NULL, NULL);
				const ChunkInfo* pCurChunk = &(pCurNode->mChunkInfo);

				if ( kNodeTypeChunkFree & pCurNode->mNodeType)
				{
					pTraceCallback->Tracef("0x" _EA_SOAPOOL_PTRFMT_ "\t\t%d\t%d\t%s\tCoreBlock 0x" _EA_SOAPOOL_PTRFMT_ "\t%s (%d)\n", (pCurChunk->mChunk), pCurChunk->mnRequestedSize,
									pCurChunk->mnChunkSize, "FREE", (pCurChunk->mCoreBlock), "", 0);
				}
				else
				{
					pTraceCallback->Tracef("0x" _EA_SOAPOOL_PTRFMT_ "\t\t%d\t%d\t%s\tCoreBlock 0x" _EA_SOAPOOL_PTRFMT_ "\t%s (%d)\n", (pCurChunk->mChunk), pCurChunk->mnRequestedSize,
									pCurChunk->mnChunkSize, pCurChunk->mpAllocName, (pCurChunk->mCoreBlock), pCurChunk->mpAllocFile, pCurChunk->mnAllocLine);
				}
			}
			ReportEnd(&reportContext);
		}
		pTraceCallback->Trace("\n");
	}
}

/******************************************************************************/
/*! SmallBlockAllocator::Pool::CoreBlockDebugDataAlloc

	\brief      [Put your brief description here.]

	\param      pCoreBlock -
	\return           - void.

	[Put your optional additional description here.]
*/
/******************************************************************************/
#if EA_SOA_EXTENSIONS_ENABLED
void SmallBlockAllocator::Pool::CoreBlockDebugDataAlloc(CoreBlock* pCoreBlock)
{
	const uint32_t nAllocSize( sizeof(CoreBlockDebugData) + (sizeof(ChunkDebugData*)* (pCoreBlock->GetNumChunks()-1)) );

	pCoreBlock->mpDebugData = static_cast<CoreBlockDebugData*>(mCallbacks.DebugAlloc(nAllocSize));
	PPM_ASSERT(this, pCoreBlock->mpDebugData, kGAErrorInternal, kGAErrorText[kGAErrorInternal], NULL, NULL);  //code elsewhere assumes that pCoreBlock->mpDebugData will be valid

	if (pCoreBlock->mpDebugData)
	{
		//0 everything out, Chunkdebugdata pointers are all NULL to start
		memset(pCoreBlock->mpDebugData, 0, nAllocSize);
	}
}
#endif // EA_SOA_EXTENSIONS_ENABLED

/******************************************************************************/
/*! SmallBlockAllocator::Pool::CoreBlockDebugDataFree

	\brief      [Put your brief description here.]

	\param      pCoreBlock -
	\return           - void.

	[Put your optional additional description here.]
*/
/******************************************************************************/
#if EA_SOA_EXTENSIONS_ENABLED
void SmallBlockAllocator::Pool::CoreBlockDebugDataFree(CoreBlock* pCoreBlock)
{
	PPM_ASSERT(this, pCoreBlock->mnNumChunks == pCoreBlock->mnNumFreeChunks, kGAErrorInternal, kGAErrorText[kGAErrorUnknown], NULL, NULL); // If this fails then the user is leaking memory.
	if (pCoreBlock->mpDebugData)
	{
		mCallbacks.DebugFree(pCoreBlock->mpDebugData);
		pCoreBlock->mpDebugData = NULL;
	}
}
#endif // EA_SOA_EXTENSIONS_ENABLED

/******************************************************************************/
/*! SmallBlockAllocator::Pool::ChunkDebugDataAlloc

	\brief      [Put your brief description here.]

	\param      pCoreBlock          -
	\param      pChunk          -
	\param      nRequestedSize -
	\param      allocInfo      -
	\return                    - void.

	[Put your optional additional description here.]
*/
/******************************************************************************/
#if EA_SOA_EXTENSIONS_ENABLED
void SmallBlockAllocator::Pool::ChunkDebugDataAlloc(CoreBlock* pCoreBlock, ChunkRef pChunk, uint32_t nRequestedSize, const EA::Allocator::AllocInfo& allocInfo)
{
	PPM_ASSERT(this, pCoreBlock, kGAErrorInternal, kGAErrorText[kGAErrorInternal], NULL, NULL);

	CoreBlockDebugData* pCoreBlockDebugData = pCoreBlock->mpDebugData;
	PPM_ASSERT(this, pCoreBlockDebugData, kGAErrorInternal, kGAErrorText[kGAErrorInternal], NULL, NULL);

	const intptr_t nChunkIndex( pCoreBlock->mnNumChunks - ((pCoreBlock->End() - ((pChunk))) / mnChunkSize) );
	PPM_ASSERT(this, (pCoreBlock->GetNumChunks() > nChunkIndex) && (0 <= nChunkIndex), kGAErrorInternal, kGAErrorText[kGAErrorInternal], NULL, NULL);  //INVALID INDEX?
	PPM_ASSERT(this,  pCoreBlockDebugData && (pCoreBlockDebugData->mChunkData[nChunkIndex] == NULL), kGAErrorInternal, kGAErrorText[kGAErrorInternal], NULL, NULL);  //ALREADY ADDED? OR NOT REMOVED?

	//convert the pointers to non-null strings
	const char* pAllocationName( ( allocInfo.GetName() ? allocInfo.GetName() : "") );
	const char* pFileName( (allocInfo.GetFileName() ? allocInfo.GetFileName() : "") );

	const size_t nNameLen(strlen(pAllocationName));

	//we're going to make one allocation big enough to hold the ChunkDebugData + the name string + a NULL terminator
	const size_t nAllocSize( sizeof(ChunkDebugData) + nNameLen + 1 );

	ChunkDebugData* pDebugData = static_cast<ChunkDebugData*>(mCallbacks.DebugAlloc(nAllocSize));

	if (pDebugData)
	{
		pDebugData->mnRequestedSize = nRequestedSize;
		pDebugData->mChunkAddr = pChunk;
		pDebugData->mnGroupId = (mpDebugData ? mpDebugData->mnGroupId : 0); //Chunks get the current groupId
		pDebugData->mnAllocLine = allocInfo.GetLine();

		//store pointer to filename ... we're assuming it's static
		pDebugData->mpAllocFile = pFileName;

		//copy pAllocationName into buffer, we're not assuming that it's static
		//this is assuming that mAllocName is at the end of the pDebugData struct
		//and that we've allocated enough space for the name + NULL terminator!
		strcpy(pDebugData->mAllocName, pAllocationName);

		pCoreBlockDebugData->mChunkData[nChunkIndex] = pDebugData;
	}
}
#endif // EA_SOA_EXTENSIONS_ENABLED

/******************************************************************************/
/*! SmallBlockAllocator::Pool::ChunkDebugDataFree

	\brief      [Put your brief description here.]

	\param      pCoreBlock -
	\param      pChunk -
	\return           - void.

	[Put your optional additional description here.]
*/
/******************************************************************************/
#if EA_SOA_EXTENSIONS_ENABLED
void SmallBlockAllocator::Pool::ChunkDebugDataFree(CoreBlock* pCoreBlock, ChunkRef pChunk)
{
	CoreBlockDebugData* pCoreBlockDebugData = pCoreBlock->mpDebugData;
	PPM_ASSERT(this, pCoreBlockDebugData, kGAErrorInternal, kGAErrorText[kGAErrorInternal], NULL, NULL);

	const intptr_t nChunkIndex( pCoreBlock->mnNumChunks - ((pCoreBlock->End() - ((pChunk))) / mnChunkSize) );
	PPM_ASSERT(this, (pCoreBlock->GetNumChunks() > nChunkIndex) && (0 <= nChunkIndex), kGAErrorInternal, kGAErrorText[kGAErrorInternal], NULL, NULL);  //INVALID INDEX?

	//a NULL pointer can be here if EnableExtensions was called after this allocation was made
	if (pCoreBlockDebugData->mChunkData[nChunkIndex] != NULL)
	{
		PPM_ASSERT(this,  pChunk == pCoreBlockDebugData->mChunkData[nChunkIndex]->mChunkAddr, kGAErrorInternal, kGAErrorText[kGAErrorInternal], NULL, NULL);  //mismatch?
		mCallbacks.DebugFree(pCoreBlockDebugData->mChunkData[nChunkIndex]);
		pCoreBlockDebugData->mChunkData[nChunkIndex] = NULL;
	}
}
#endif //EA_SOA_EXTENSIONS_ENABLED

/******************************************************************************/
/*! SmallBlockAllocator::Pool::SetGroupId

	\brief      [Put your brief description here.]

	\param      nGroupId -
	\return             -

	[Put your optional additional description here.]
*/
/******************************************************************************/
int32_t SmallBlockAllocator::Pool::SetGroupId(int32_t nGroupId)
{
	#if EA_SOA_EXTENSIONS_ENABLED
		int32_t nPrevGroupId(0);
		if (mpDebugData)
		{
			nPrevGroupId = mpDebugData->mnGroupId;
			mpDebugData->mnGroupId = nGroupId;
		}
		return ( nPrevGroupId );
	#else
		EA_UNUSED(nGroupId);
		return 0;
	#endif

}

/******************************************************************************/
/*! SmallBlockAllocator::Pool::GetGroupId

	\brief      [Put your brief description here.]

	\param       - none.
	\return          -

	[Put your optional additional description here.]
*/
/******************************************************************************/
int32_t SmallBlockAllocator::Pool::GetGroupId() const
{
	#if EA_SOA_EXTENSIONS_ENABLED
		return ( mpDebugData ? mpDebugData->mnGroupId : 0 );
	#else
		return 0;
	#endif
}

/******************************************************************************/
/*! SmallBlockAllocator::Pool::Audit

	\brief      [Put your brief description here.]

	\param      nGroupId              -
	\param      pAuditCallback        -
	\param      pAuditCallbackContext -
	\return                          -

	[Put your optional additional description here.]
*/
/******************************************************************************/
uint32_t SmallBlockAllocator::Pool::Audit(int32_t nGroupId, AuditFunction pAuditCallback, void* pAuditCallbackContext)
{
	uint32_t nChunksFound = 0;

	#if EA_SOA_EXTENSIONS_ENABLED
		//iterate all allocated Chunks
		SmallBlockAllocator::ReportContext reportContext;
		ReportBegin(&reportContext, kNodeTypeChunkAllocated);
		for ( const SmallBlockAllocator::ReportNode* pCurNode = ReportNext(&reportContext, kNodeTypeChunkAllocated) ; pCurNode != NULL ; pCurNode = ReportNext(&reportContext, kNodeTypeChunkAllocated) )
		{
			PPM_ASSERT(this, kNodeTypeChunkAllocated & pCurNode->mNodeType, kGAErrorInternal, kGAErrorText[kGAErrorInternal], NULL, NULL);
			const SmallBlockAllocator::ChunkInfo* pCurChunk = &(pCurNode->mChunkInfo);

			if (pCurChunk->mnGroupId == nGroupId)
			{
				pAuditCallback(pCurChunk, pAuditCallbackContext);
				nChunksFound++;
			}
		}
		ReportEnd(&reportContext);
	#else
		EA_UNUSED(nGroupId); EA_UNUSED(pAuditCallback); EA_UNUSED(pAuditCallbackContext);
	#endif //EA_SOA_EXTENSIONS_ENABLED

	return nChunksFound;
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
void SmallBlockAllocator::Pool::AssertionFailure(const char* pExpression, int nErrorId, const char* pDescription, const void* pData, const void* pExtra) const
{
	AssertionFailureInfo afi;

	afi.mpAllocator        = NULL;
	afi.mpPool             = reinterpret_cast<const char*>(this);
	afi.mpExpression       = pExpression;
	afi.mnErrorId          = nErrorId;
	afi.mpDescription      = pDescription;
	afi.mpData             = pData;
	afi.mpExtra            = pExtra;

	mCallbacks.AssertionFailure(&afi);
}

/******************************************************************************/
/*! SmallBlockAllocator::Pool::ValidateAddress

	\brief      [Put your brief description here.]

	\param      pAddress              -
	\param      kAddressTypeAllocated -
	\return                           -

	[Put your optional additional description here.]
*/
/******************************************************************************/
const void* SmallBlockAllocator::Pool::ValidateAddress(const void* pAddress, int addressType) const
{
	for ( CoreBlock* pCurCoreBlock = mCoreBlockList.GetFirst(); pCurCoreBlock; pCurCoreBlock = pCurCoreBlock->mpNext )
	{
		if (pCurCoreBlock->ContainsMem(pAddress))
		{
			if(addressType == kAddressTypeOwned)
			{
				return pAddress;
			}

			//adjust the pointer in case it's not pointing to the front of the actual Chunk
			//we find the distance from the end of the CoreBlock to pAddress and % that with Chunk size to find out the offset from the back of the previous Chunk,
			//then we subtract Chunk size from that and add to address to get a pointer to an exact Chunk
			//The +1 (in nOffsetAddress) is a trick to allow this code to work for the case where we actually do have a pointer to the start of a Chunk without needing to check for that case
			const uintptr_t nOffsetAddress = reinterpret_cast<uintptr_t>(pAddress) + 1;
			uint8_t* pChunkAddress = reinterpret_cast<uint8_t*>(nOffsetAddress+(((reinterpret_cast<uintptr_t>(pCurCoreBlock->End()) - nOffsetAddress) % mnChunkSize) - mnChunkSize));

			PPM_ASSERT(this, 0 == (reinterpret_cast<uintptr_t>(pCurCoreBlock->End())-reinterpret_cast<uintptr_t>(pChunkAddress))%mnChunkSize, kGAErrorInternal, kGAErrorText[kGAErrorInternal], NULL, NULL);
			PPM_ASSERT(this, reinterpret_cast<const uint8_t*>(pAddress) >= (pChunkAddress), kGAErrorInternal, kGAErrorText[kGAErrorInternal], NULL, NULL);
			PPM_ASSERT(this, reinterpret_cast<const uint8_t*>(pAddress) < (pChunkAddress + mnChunkSize), kGAErrorInternal, kGAErrorText[kGAErrorInternal], NULL, NULL);

			if (!pCurCoreBlock->IsFree(pChunkAddress))
			{
				if(addressType == kAddressTypeAllocated)
				{
					return pChunkAddress;
				}

				// addressType == kAddressTypeSpecific
				if((const void*)pChunkAddress == pAddress)
				{
					return pChunkAddress;
				}
			}
			return NULL;
		}
	}
	return NULL;
}

/******************************************************************************/
/*! SmallBlockAllocator::Pool::ValidatePool

	\brief      [Put your brief description here.]

	\param      pErrorCount -
	\return                 -

	[Put your optional additional description here.]
*/
/******************************************************************************/
bool SmallBlockAllocator::Pool::ValidatePool(int* pErrorCount) const
{
	int nErrorCount = 0;
	bool allocCoreBlockFound(false);

	//the first CoreBlock is a special case that always stays at the head of the list and never gets freed so skip it..
	CoreBlock* pCurCoreBlock(mCoreBlockList.GetFirst());
	CoreBlock* pPrevCoreBlock(NULL);
	CoreBlock* pAllocCoreBlock(mpAllocCoreBlock);

	while ( pCurCoreBlock )
	{
		//verify owner
		nErrorCount += Verify(this, pCurCoreBlock->mpOwner == this, kGAErrorCorruptedHeap, kGAErrorText[kGAErrorCorruptedHeap], this, NULL);

		//verify that various member variables look sane
		nErrorCount += Verify(this, (pCurCoreBlock->mnNumFreeChunks == 0 ? pCurCoreBlock->mpFreeChunks == NULL : pCurCoreBlock->mpFreeChunks != NULL), kGAErrorCorruptedHeap, kGAErrorText[kGAErrorCorruptedHeap], this, NULL);
		nErrorCount += Verify(this, (pCurCoreBlock->mnNumFreeChunks <= pCurCoreBlock->mnNumChunks), kGAErrorCorruptedHeap, kGAErrorText[kGAErrorCorruptedHeap], this, NULL);
		nErrorCount += Verify(this, ((pCurCoreBlock->mnNumChunks * mnChunkSize + CoreBlockOverhead()) <= pCurCoreBlock->mnSize), kGAErrorCorruptedHeap, kGAErrorText[kGAErrorCorruptedHeap], this, NULL);

		//if (pCurCoreBlock->mpDebugData)
		//{
			//$$todo ... iterate Chunk debug data and verify that ChunkDebugData->ChunkRef == Chunk address
		//}

		//if it's kCoreBlockTypeHeap pool, verify CoreBlock conforms to mHeapParams.mnMinCoreBlockAlign...
		if ( kCoreBlockTypeHeap == mCoreBlockType)
		{
			nErrorCount += Verify(this, 0 == (( reinterpret_cast<uintptr_t>(pCurCoreBlock) + CoreBlockOverhead()) % mHeapParams.mnMinCoreBlockAlign), kGAErrorInternal, kGAErrorText[kGAErrorInternal], this, NULL);
		}

		//verify that previous pointer is sane
		nErrorCount += Verify(this, pPrevCoreBlock == pCurCoreBlock->mpPrevious, kGAErrorInternal, kGAErrorText[kGAErrorInternal], this, NULL);

		//verify CoreBlocks are in order
		if (pCurCoreBlock->mpNext)
		{
			//verify CoreBlocks are in order
			//the next CoreBlock should always be either empty, have more total Chunks or have less than or equal the current CoreBlock's free Chunks
			nErrorCount += Verify(this, ((pCurCoreBlock->mpNext->GetNumFreeChunks() == 0) || (pCurCoreBlock->mpNext->GetNumChunks() > pCurCoreBlock->GetNumChunks()) ||
											 (pCurCoreBlock->mpNext->GetNumFreeChunks() <= pCurCoreBlock->GetNumFreeChunks())), kGAErrorInternal, kGAErrorText[kGAErrorInternal], this, NULL);
		}

		if (allocCoreBlockFound)
		{
			//alloc CoreBlock should be pointing to the last CoreBlock with free Chunks
			nErrorCount += Verify(this, pCurCoreBlock->GetNumFreeChunks() == 0, kGAErrorInternal, kGAErrorText[kGAErrorInternal], this, NULL);
		}
		else
		{
			if (pAllocCoreBlock == pCurCoreBlock)
			{
				allocCoreBlockFound = true;
			}
			else if ( pCurCoreBlock->GetNumFreeChunks() != 0 )
			{
				//if we found a CoreBlock with more than 0 Chunks free, there must be an alloc CoreBlock (since this CoreBlock is a valid alloc CoreBlock)
				// and if there is an alloc CoreBlock, and this isn't it, make sure this isn't a better candidate for being the alloc CoreBlock
				// the better candidate is the one with more total Chunks or fewer free Chunks
				nErrorCount += Verify(this, (pAllocCoreBlock && ( (pAllocCoreBlock->GetNumChunks() > pCurCoreBlock->GetNumChunks()) || (pAllocCoreBlock->GetNumFreeChunks() <= pCurCoreBlock->GetNumFreeChunks()) )), kGAErrorInternal, kGAErrorText[kGAErrorInternal], this, NULL);
			}
		}

		uint32_t nFreeChunksFound = 0;
		for (const Chunk* pCurChunk = pCurCoreBlock->mpFreeChunks; pCurChunk != NULL; pCurChunk = pCurChunk->mpNextFreeChunk)
		{
			// Verify that all Chunks in the free list belong to the given CoreBlock.
			nErrorCount += Verify(this, pCurCoreBlock->ContainsMem(pCurChunk), kGAErrorCorruptedHeap, kGAErrorText[kGAErrorCorruptedHeap], this, NULL);

			// Verify that pCurChunk points to the beginning of a Chunk and not somewhere in the middle of one.
			nErrorCount += Verify(this, 0 == ((pCurCoreBlock->End()-reinterpret_cast<const uint8_t*>(pCurChunk))%mnChunkSize), kGAErrorCorruptedHeap, kGAErrorText[kGAErrorCorruptedHeap], this, NULL);

			// Verify any free guard fills are valid and not overwritten by a user as might happen with using a stale pointer.
			#if PPM_DEBUG_FILL && (PPM_DEBUG >= 2)
				//uint8_t* const pMemBegin = ((uint8_t*)(pCurChunk)) + sizeof(Chunk);
				//uint8_t* const pMemEnd   = ((uint8_t*)(pCurChunk)) + mnChunkSize;

				//$$todo
			#endif

			nFreeChunksFound++;
		}

		// Verify that mnFreeChunkCount matches the number of free Chunks in the list
		nErrorCount += Verify(this, (nFreeChunksFound == pCurCoreBlock->mnNumFreeChunks), kGAErrorCorruptedHeap, kGAErrorText[kGAErrorCorruptedHeap], this, NULL);

		pPrevCoreBlock = pCurCoreBlock;
		pCurCoreBlock = pCurCoreBlock->mpNext;
	}

	if ( pErrorCount )
	{
		*pErrorCount += nErrorCount;
	}
	return (nErrorCount==0);
}

/******************************************************************************/
/*! Pool::Verify

	\brief      [Put your brief description here.]

	\param      pAllocator   -
	\param      id           -
	\param      pDescription -
	\param      pData        -
	\param      pLocation    -
	\return                  -

	[Put your optional additional description here.]
*/
/******************************************************************************/
int SmallBlockAllocator::Pool::Verify(const EA::Allocator::SmallBlockAllocator::Pool* const pAllocator, bool bExpression,
						int id, const char* pDescription, const void* pData, const void* pLocation) const
{
	EA_UNUSED(pAllocator); EA_UNUSED(id); EA_UNUSED(pDescription); EA_UNUSED(pData); EA_UNUSED(pLocation);

	if(bExpression)
		return 0;
	PPM_ASSERT(pAllocator, false, id, pDescription, (void*)pData, (void*)pLocation);
	return 1;
}

} // namespace Allocator
} // namespace EA



#if defined(_MSC_VER)
	#pragma warning(pop)
#endif


