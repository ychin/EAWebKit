///////////////////////////////////////////////////////////////////////////////
// EAGeneralAllocatorDebug (a.k.a. GeneralAllocatorDebug) 
//
// Copyright (c) 2004, Electronic Arts. All Rights Reserved.
// By Paul Pedriana, Maxis
//
// This module implements a user-level debug layer on top of PPMalloc. 
//
// PPMalloc provides some basic self-debugging functionality but doesn't 
// provide the user with higher level debugging functionality that would be
// useful for tracking and debugging memory at the application level.
//
///////////////////////////////////////////////////////////////////////////////
//
// Lint settings
// The following error suppressions largely represent warnings that can't 
// be easily fixed in code but are also known to be insignificant for this module.
//lint -e826                // Suspicious pointer-to-pointer conversion (area too small).
//lint -e1551               // Function may throw exception.
//lint -e1740               // Pointer member not directly freed or zero'ed by destructor.
//lint -e1776               // Converting a string literal to void * is not const safe.
//lint -e641                // Converting enum 'Constants' to int.
//lint -e534                // Ignoring return value of function.
//lint -e539                // Did not expect positive indentation.
//lint -e525                // Negative indentation.
//lint -e506                // Constant value Boolean
//lint -esym(752,dummy__)   // Local declarator '' (same file location) not referenced
//lint -e613                // Possible use of null pointer.
//lint -esym(644,p)         // Variable may not have been initialized
//lint -e526                // Symbol 'Symbol' (Location) not defined.
///////////////////////////////////////////////////////////////////////////////

#include <PPMalloc/EAGeneralAllocatorDebug.h>
#include <PPMalloc/internal/atomic.inl>
#include <PPMalloc/internal/shared.h>
#include <stdio.h>
#include <string.h>
#include <new>

#if   defined(_MSC_VER)
	#pragma warning(push)
	#pragma warning(disable: 6011) // Dereferencing NULL pointer
#endif


namespace EA{

namespace Allocator{


void GeneralAllocatorDebug::HashTable::Clear(bool bDestroyTable, bool bFreeValues)
{
	if(mppHashTable)
	{
		for(size_t i = 0; i < mnHashTableSize; i++)
		{
			HashPair* p = mppHashTable[i];

			while(p)
			{
				HashPair* const pNext = p->mpNext;
				if(p->mpValue && bFreeValues)
					mpFreeFunction(p->mpValue, mpFunctionContext);
				p->~HashPair();
				mpFreeFunction(p, mpFunctionContext);
				p = pNext;
			}
			mppHashTable[i] = NULL;
		}

		if(bDestroyTable)
		{
			mpFreeFunction(mppHashTable, mpFunctionContext);
			mppHashTable = NULL;
			mnHashTableSize = 0;
		}

		mnHashElementCount = 0;
	}
}


void GeneralAllocatorDebug::HashTable::Nuke()
{
	mppHashTable       = NULL;
	mbReHashing        = false;
	mnHashTableSize    = 0;
	mnHashElementCount = 0;
}


bool GeneralAllocatorDebug::HashTable::Insert(const void* pKey, char* pValue)
{
	if((4 * (mnHashElementCount + 1)) >= (2 * mnHashTableSize))
	{
		size_t nTableSizeNew = (2 * mnHashTableSize) + 1;
		if(nTableSizeNew < mnHashTableSizeMin)
			nTableSizeNew = mnHashTableSizeMin;
		if(!ReHash(nTableSizeNew))
			return false;
	}

	void* const pMemory = mpAllocationFunction(sizeof(HashPair), mpFunctionContext);
	if(pMemory)
	{
		const size_t h = Hash(pKey);
		HashPair* const pHashPair    = mppHashTable[h];
		HashPair* const pHashPairNew = new(pMemory) HashPair(pKey, pValue, pHashPair);
		mppHashTable[h] = pHashPairNew;
		mnHashElementCount++;
	}

	return (pMemory != NULL);
}


bool GeneralAllocatorDebug::HashTable::Find(const void* pKey, char*** pppValue) const
{
	if(mppHashTable)
	{
		const size_t h = Hash(pKey);

		for(HashPair* p = mppHashTable[h]; p; p = p->mpNext)
		{
			if(p->mpKey == pKey)
			{
				/// We want to return to the user the address of a pointer.
				/// The pointer's type is char*. The address of this is char**.
				/// For us to modify this char** and pass it back to the user,
				/// the parameter type must be either char*** or char**&.
				/// Parameter pppValue is the address of a variable the user 
				/// passes which itself stores the address of a pointer.
				if(pppValue)
				  *pppValue = &p->mpValue;
				return true;
			}
		}
	}
	return false;
}


bool GeneralAllocatorDebug::HashTable::Remove(const void* pKey, bool bFreeValue)
{
	if(mppHashTable)
	{
		const size_t h  = Hash(pKey);
		HashPair* p     = mppHashTable[h];
		HashPair* pPrev = NULL;

		while(p)
		{
			HashPair* pNext = p->mpNext;

			if(p->mpKey == pKey)
			{
				if(pPrev)
					pPrev->mpNext = p->mpNext;
				else
					mppHashTable[h] = p->mpNext;
				if(p->mpValue && bFreeValue)
					mpFreeFunction(p->mpValue, mpFunctionContext);
				p->~HashPair();
				mpFreeFunction(p, mpFunctionContext);
				mnHashElementCount--;
				return true;
			}

			pPrev = p;
			p      = pNext;
		}
	}

	return false;
}


bool GeneralAllocatorDebug::HashTable::ReHash(size_t nTableSizeNew)
{
	bool bReturnValue = true;

	if(!mbReHashing) // Re-entrancy prevention
	{
		mbReHashing = true;

		const size_t nTableSizeOld  = mnHashTableSize;
		HashPair** const ppTableOld = mppHashTable;
		HashPair** const ppTableNew = (HashPair**)mpAllocationFunction(sizeof(HashPair*) * nTableSizeNew, mpFunctionContext);

		bReturnValue = (ppTableNew != NULL);
		if(ppTableNew)
		{
			memset(ppTableNew, 0, sizeof(HashPair*) * nTableSizeNew); // Note that the allocation call above could trigger hash table reads or writes.

			mnHashTableSize = nTableSizeNew; // It's important that we do these assignments after the new table allocation in order
			mppHashTable    = ppTableNew;    // to deal  with re-entrancy that can occur during allocation of the new table.

			for(size_t i = 0; i < nTableSizeOld; i++)
			{
				HashPair* p = ppTableOld[i];

				while(p)
				{
					const size_t h = Hash(p->mpKey); // Hash will use the use the new table size.
					HashPair* const pNext = p->mpNext;
					p->mpNext = ppTableNew[h];
					ppTableNew[h] = p;
					p = pNext;
				}
			}

			if(ppTableOld)
				mpFreeFunction(ppTableOld, mpFunctionContext);
		}

		mbReHashing = false;
	}

	return bReturnValue;
}





///////////////////////////////////////////////////////////////////////////////
// GeneralAllocatorDebug
//
GeneralAllocatorDebug::GeneralAllocatorDebug(void* pInitialCore, size_t nInitialCoreSize, 
											 bool bShouldFreeInitialCore, bool bShouldTrimInitialCore, 
											 CoreFreeFunction pInitialCoreFreeFunction, void* pInitialCoreFreeFunctionContext,
											 CoreExtendFunction pInitialCoreExtendFunction, void* pInitialCoreExtendFunctionContext)
	:  GeneralAllocator(pInitialCore, nInitialCoreSize, bShouldFreeInitialCore, bShouldTrimInitialCore, 
						pInitialCoreFreeFunction, pInitialCoreFreeFunctionContext,
						pInitialCoreExtendFunction, pInitialCoreExtendFunctionContext)
{
	Construct(Parameters(pInitialCore, nInitialCoreSize, nInitialCoreSize, bShouldFreeInitialCore, bShouldTrimInitialCore, 
			  pInitialCoreFreeFunction, pInitialCoreFreeFunctionContext, pInitialCoreExtendFunction, pInitialCoreExtendFunctionContext));
}



///////////////////////////////////////////////////////////////////////////////
// ~GeneralAllocatorDebug
//
GeneralAllocatorDebug::~GeneralAllocatorDebug()
{
	// No mutex lock because it would be pointless here.

	GeneralAllocatorDebug::Shutdown(); // virtual function mechanism doesn't work in constructors or destructors.
}


///////////////////////////////////////////////////////////////////////////////
// Construct
//
void GeneralAllocatorDebug::Construct(const Parameters&)
{
	// No mutex lock because it would be pointless here.

	mbInitializedDebug = false;

	// Debug options
	#if (PPM_DEBUG >= 2) // Extended user debug builds.
		mnDebugDataFlags  = (1 << kDebugDataIdGuard);
	#else
		mnDebugDataFlags  = 0;
	#endif
	mnNextDebugDataFlags    = 0;
	mpNextAllocationName    = NULL;
	mpNextAllocationFile    = NULL;
	mnNextAllocationLine    = 0;
	mnCurrentGroupId        = 0;
	mnPtrValidationDisabled = 0;

	// Guard settings
	mfGuardSizeRatio = 0.25f;
	mnMinGuardSize   = 8;
	mnMaxGuardSize   = 10000;

	// Debug fill values
	mnFillFree        = kDefaultFillValueFree;
	mnFillDelayedFree = kDefaultFillValueDelayedFree;
	mnFillNew         = kDefaultFillValueNew;
	mnFillGuard       = kDefaultFillValueGuard;
	mnFillUnusedCore  = kDefaultFillValueUnusedCore;

	// Delayed free settings
	#if (PPM_DEBUG >= 2) // Extended user debug builds.
		mDelayedFreePolicy      = kDelayedFreePolicyCount;
		mDelayedFreePolicyValue = 1000;
	#else
		mDelayedFreePolicy        = kDelayedFreePolicyNone;
		mDelayedFreePolicyValue = 0;
	#endif
	mDelayedFreeChunkListHead.mnPriorSize = 0;
	mDelayedFreeChunkListHead.mnSize      = sizeof(mDelayedFreeChunkListHead);
	mDelayedFreeChunkListHead.mpPrevChunk = &mDelayedFreeChunkListHead;
	mDelayedFreeChunkListHead.mpNextChunk = &mDelayedFreeChunkListHead;
	mDelayedFreeListSize = 0;
	mDelayedFreeVolume   = 0;

	// Metrics
	mbMetricsEnabled  = false;
	mnAllocationCountHistorical = 0;
	#if (PPM_DEBUG >= 3) // Developer builds only.
		// These memsets are always done in Init, but we do them here in a debug build to appease code analysis tools.
		memset(mAllocationMetrics, 0, sizeof(mAllocationMetrics));
	#endif

	// Debug data map and valid pointer tracking.
	mpAllocator                  = this;
	mnDebugDataLocationDefault   = kDebugDataLocationBlock;
	mAddressRepLookup            = nullptr;
	mbHashTableEnabled           = false;
	mHashTable.SetAllocationFunction(HashAllocationFunction, HashFreeFunction, (GeneralAllocatorDebug*)this);
	//#if (PPM_DEBUG >= 2) // Extended user debug builds.
	//    SetOption(kOptionEnablePtrValidation, 1);
	//#endif

	// PPMalloc calls Init, and we follow suit. Note that ctors cannot use the     
	// virtual function mechanism, so that's why we have to also do the call here.
	// Otherwise, our own init function wouldn't be called. But we specifically
	// call Init with zero/NULL parameters to prevent Init from doing any 
	// core allocation.
	GeneralAllocatorDebug::Init(NULL, 0); // virtual function mechanism doesn't work in constructors or destructors.
}

///////////////////////////////////////////////////////////////////////////////
// Init
//
bool GeneralAllocatorDebug::Init(void* pInitialCore, size_t nInitialCoreSize, bool bShouldFreeInitialCore, bool bShouldTrimInitialCore, 
								 CoreFreeFunction pInitialCoreFreeFunction, void* pInitialCoreFreeFunctionContext,
								 CoreExtendFunction pInitialCoreExtendFunction, void* pInitialCoreExtendFunctionContext)
{
	return Init(Parameters(pInitialCore, nInitialCoreSize, nInitialCoreSize, bShouldFreeInitialCore, bShouldTrimInitialCore, 
				pInitialCoreFreeFunction, pInitialCoreFreeFunctionContext, pInitialCoreExtendFunction, pInitialCoreExtendFunctionContext));
}

bool GeneralAllocatorDebug::Init(const Parameters& parameters)
{
	// No mutex lock because we assume the init caller code is thread-safe.

	const bool bReturnValue = GeneralAllocator::Init(parameters);
	if(!mbInitializedDebug)
	{
		mbInitializedDebug = true;
		memset(mAllocationMetrics, 0, sizeof(mAllocationMetrics));
	}

	return bReturnValue;
	
}



///////////////////////////////////////////////////////////////////////////////
// Shutdown
//
bool GeneralAllocatorDebug::Shutdown()
{
	// No mutex lock because we assume the shutdown caller code is thread-safe.
	// Perhaps we should change this an lock anyway. If we do then we need to 
	// take care because GeneralAllocator::Shutdown will destroy the mutex.

	ClearDelayedFreeList(kDelayedFreePolicyNone, 0);
	const bool bReturnValue = GeneralAllocator::Shutdown();

	mbInitializedDebug = false;  // Set this to false before calling HashTable.Clear below so that the memory functions know that we are dead.
	if(mpAllocator == this)      // If the GeneralAllocator::Shutdown killed the hash table and its nodes...
		mHashTable.Nuke();
	else
		mHashTable.Clear(true, true); // We do this after GeneralAllocator::Shutdown because our data might be needed for consultation.

	return bReturnValue;
}


///////////////////////////////////////////////////////////////////////////////
// AsInterface
//
void* GeneralAllocatorDebug::AsInterface(int id)
{
	if(id == GeneralAllocatorDebug::kTypeId)
		return static_cast<GeneralAllocatorDebug*>(this); // The cast isn't necessary, but makes the intent more clear.
	return GeneralAllocator::AsInterface(id);
}


///////////////////////////////////////////////////////////////////////////////
// HashAllocationFunction
//
void* GeneralAllocatorDebug::HashAllocationFunction(size_t n, void* pContext)
{
	return ((GeneralAllocatorDebug*)pContext)->AllocatorMalloc(n);
}


///////////////////////////////////////////////////////////////////////////////
// HashFreeFunction
//
void GeneralAllocatorDebug::HashFreeFunction(void* p, void* pContext)
{
	return ((GeneralAllocatorDebug*)pContext)->AllocatorFree(p);
}


///////////////////////////////////////////////////////////////////////////////
// AllocatorMalloc
//
void* GeneralAllocatorDebug::AllocatorMalloc(size_t n)
{
	if(mpAllocator == this) // If our internal allocator is set to be ourselves...
	{
		const HeapValidationLevel hvlSaved(mAutoHeapValidationLevel);
		mAutoHeapValidationLevel = kHeapValidationLevelNone; // Temporarily disable heap validation while in the middle of an operation.

		TryAllocation:
		void* const pData = MallocInternal(n, kAllocationFlagInternal); // It's important that we call MallocInternal because otherwise if we called Malloc then we would get into an infinitely recursive loop.
		if(pData)
			SetChunkIsInternal(GetChunkPtrFromDataPtr(pData));
		else
		{
			// Else the malloc failed. If we have any delayed free blocks, we can free them and try again.
			if(mDelayedFreeChunkListHead.mpNextChunk != &mDelayedFreeChunkListHead) // If there is anything to free...
			{
				ClearDelayedFreeList(kDelayedFreePolicyNone, 0);
				goto TryAllocation; // Jump back above and try again.
			}
		}

		mAutoHeapValidationLevel = hvlSaved;

		return pData;
	}
	return mpAllocator->Malloc(n); // Intentionally don't use the kAllocationFlagInternal flag, as the allocator we are using here is external to us.
}


///////////////////////////////////////////////////////////////////////////////
// AllocatorFree
//
void GeneralAllocatorDebug::AllocatorFree(void* p)
{
	if(mpAllocator == this) // If our internal allocator is set to be ourselves...
	{
		// We have a nasty chicken-and-egg problem here. This function can be 
		// called after the GeneralAllocator pool is shut down as part of the hash
		// table clearing operation. Hash table clearing needs to be done 
		// afterwards like this because our data needs to be available for 
		// the Shutdown so that existing blocks can be described. But our table 
		// itself may have been allocated via GeneralAllocator. In this case 
		// there is no solution but to let Shutdown think the block is a leak      
		// and to not try to free the (already freed by Shutdown) block here. 
		// At least we have flagged the block as internal so Shutdown realizes 
		// it came from some internal mechanism.
		if(mbInitializedDebug)
		{
			const HeapValidationLevel hvlSaved(mAutoHeapValidationLevel);
			mAutoHeapValidationLevel = kHeapValidationLevelNone; // Temporarily disable heap validation while in the middle of an operation.

			FreeInternal(p);  // It's important that we call FreeInternal because otherwise if we called Free then we would get into an infinitely recursive loop.

			mAutoHeapValidationLevel = hvlSaved;
		}
	}
	else
		mpAllocator->Free(p);
}



///////////////////////////////////////////////////////////////////////////////
// SetAllocator
//
void GeneralAllocatorDebug::SetAllocator(GeneralAllocator* pAllocator)
{
	// assert(no allocations have taken place);
	if(pAllocator)
	{
		mpAllocator = pAllocator;

		// Possibly consider enabling this as an optimization. But perhaps we should
		// instead leave it disabled and just let the user control the setting.
		//if(mpAllocator != this)
		//    mpAllocator->SetOption(kOptionEnablePtrValidation, 0);
	}
	else
		mpAllocator = this;
}


///////////////////////////////////////////////////////////////////////////////
// ClearDelayedFreeList
//
// This is a user-level clearing function.
//
void GeneralAllocatorDebug::ClearDelayedFreeList()
{
	// No mutex lock here because the call below take care of it.
	ClearDelayedFreeList(kDelayedFreePolicyNone, 0);
}



///////////////////////////////////////////////////////////////////////////////
// ClearDelayedFreeList
//
// This is an internal function.
//
void GeneralAllocatorDebug::ClearDelayedFreeList(DelayedFreePolicy delayedFreePolicy, int nDelayedFreePolicyValue)
{
	#if PPM_THREAD_SAFETY_SUPPORTED
		PPMAutoMutex autoMutex(mpMutex);
	#endif

	// Temporarily disable ptr validation, as delayed free chunks won't be present in 
	// the valid pointer list. They are pointers that have been freed by the user.
	mnPtrValidationDisabled++;

	if(delayedFreePolicy == kDelayedFreePolicyNone) // If we should unilaterally clear all chunks...
	{
		while(mDelayedFreeChunkListHead.mpNextChunk != &mDelayedFreeChunkListHead)
		{
			Chunk* const pChunk      = mDelayedFreeChunkListHead.mpNextChunk;
			const  void* pData       = GetDataPtrFromChunkPtr(pChunk);
			const size_t nUsableSize = GeneralAllocator::GetUsableSize(pData); // Intentionally call inherited version.
			PPM_ASSERT(this, nUsableSize < 0x40000000, kGAErrorCorruptedChunk, kGAErrorText[kGAErrorCorruptedChunk], pChunk, pChunk); // Sanity check. // If this fails, the Chunk is probably corrupted.

			UnlinkChunk(pChunk);
			mDelayedFreeListSize--;
			mDelayedFreeVolume -= nUsableSize;
			FreeDelayedFreeChunk(pChunk); // This will also check the delayed free chunk.
		}
		PPM_ASSERT(this, (mDelayedFreeListSize == 0) && (mDelayedFreeVolume == 0), kGAErrorInternal, kGAErrorText[kGAErrorInternal], NULL, NULL); // If this fails, PPMalloc is at fault.
	}
	else if(delayedFreePolicy == kDelayedFreePolicyCount)
	{
		while((mDelayedFreeChunkListHead.mpNextChunk != &mDelayedFreeChunkListHead) && (mDelayedFreeListSize > (size_t)nDelayedFreePolicyValue))
		{
			Chunk* const pChunk      = mDelayedFreeChunkListHead.mpNextChunk;
			const  void* pData       = GetDataPtrFromChunkPtr(pChunk);
			const size_t nUsableSize = GeneralAllocator::GetUsableSize(pData); // Intentionally call inherited version.
			PPM_ASSERT(this, nUsableSize < 0x40000000, kGAErrorCorruptedChunk, kGAErrorText[kGAErrorCorruptedChunk], pChunk, pChunk); // Sanity check. // If this fails, the Chunk is probably corrupted.

			UnlinkChunk(pChunk);
			mDelayedFreeListSize--;
			mDelayedFreeVolume -= nUsableSize; 
			FreeDelayedFreeChunk(pChunk); // This will also check the delayed free chunk.
		}
		PPM_ASSERT(this, (mDelayedFreeListSize < 0x80000000) && (mDelayedFreeVolume < 0x80000000), kGAErrorInternal, kGAErrorText[kGAErrorInternal], NULL, NULL); // If this fails, PPMalloc is at fault.
	}
	else if(delayedFreePolicy == kDelayedFreePolicyVolume)
	{
		while((mDelayedFreeChunkListHead.mpNextChunk != &mDelayedFreeChunkListHead) && (mDelayedFreeVolume > (size_t)nDelayedFreePolicyValue))
		{
			Chunk* const pChunk      = mDelayedFreeChunkListHead.mpNextChunk;
			const  void* pData       = GetDataPtrFromChunkPtr(pChunk);
			const size_t nUsableSize = GeneralAllocator::GetUsableSize(pData); // Intentionally call inherited version.
			PPM_ASSERT(this, nUsableSize < 0x40000000, kGAErrorCorruptedChunk, kGAErrorText[kGAErrorCorruptedChunk], pChunk, pChunk); // Sanity check. // If this fails, the Chunk is probably corrupted.

			UnlinkChunk(pChunk);
			mDelayedFreeListSize--;
			mDelayedFreeVolume -= nUsableSize; 
			FreeDelayedFreeChunk(pChunk); // This will also check the delayed free chunk.
		}
		PPM_ASSERT(this, (mDelayedFreeListSize < 0x80000000) && (mDelayedFreeVolume < 0x80000000), kGAErrorInternal, kGAErrorText[kGAErrorInternal], NULL, NULL); // If this fails, PPMalloc is at fault.
	}
	else if(delayedFreePolicy == kDelayedFreePolicyTime)
	{
		unsigned nCurrentTime = GetPPMallocDebugTime();
		unsigned nEndTime     = nCurrentTime + (unsigned)nDelayedFreePolicyValue;

		if(nEndTime < nCurrentTime) // If wraparound occurred...
			nEndTime = 0;                // Flag all blocks for freeing.

		while(mDelayedFreeChunkListHead.mpNextChunk != &mDelayedFreeChunkListHead)
		{
			Chunk* const pChunk      = mDelayedFreeChunkListHead.mpNextChunk;
			const  void* pData       = GetDataPtrFromChunkPtr(pChunk);
			const size_t nUsableSize = GeneralAllocator::GetUsableSize(pData); // Intentionally call inherited version.
			PPM_ASSERT(this, nUsableSize < 0x40000000, kGAErrorCorruptedChunk, kGAErrorText[kGAErrorCorruptedChunk], pChunk, pChunk); // Sanity check. // If this fails, the Chunk is probably corrupted.

			unsigned nMostRecentTime(0);
			memcpy(&nMostRecentTime, GetDataPtrFromChunkPtr(pChunk), sizeof(nMostRecentTime));

			if(nMostRecentTime > nEndTime)
			{
				UnlinkChunk(pChunk);
				mDelayedFreeListSize--;
				mDelayedFreeVolume -= nUsableSize; 
				FreeDelayedFreeChunk(pChunk); // This will also check the delayed free chunk.
			}
			else
				break;
		}
		PPM_ASSERT(this, (mDelayedFreeListSize < 0x80000000) && (mDelayedFreeVolume < 0x80000000), kGAErrorInternal, kGAErrorText[kGAErrorInternal], NULL, NULL); // If this fails, PPMalloc is at fault.
	}

	// Restore the ptr validation level.
	mnPtrValidationDisabled--;
}


///////////////////////////////////////////////////////////////////////////////
// AllocationIsDelayedFree
//
bool GeneralAllocatorDebug::AllocationIsDelayedFree(const void* pData) const
{
	#if PPM_THREAD_SAFETY_SUPPORTED
		PPMAutoMutex autoMutex(mpMutex);
	#endif

	for(const Chunk* pChunk = mDelayedFreeChunkListHead.mpNextChunk; 
		 pChunk != &mDelayedFreeChunkListHead;
		 pChunk = pChunk->mpNextChunk)
	{
		const void* const pDataTemp = GetDataPtrFromChunkPtr(pChunk);

		if(pDataTemp == pData)
			return true;
	}

	return false;
}


///////////////////////////////////////////////////////////////////////////////
// FreeDelayedFreeChunk
//
void GeneralAllocatorDebug::FreeDelayedFreeChunk(const Chunk* pChunk)
{
	// No mutex lock here because this is an internal function. 
	#if PPM_THREAD_SAFETY_SUPPORTED
	PPM_ASSERT(this, !mpMutex || PPMMutexGetLockCount(mpMutex) > 0, kGAErrorInternal, kGAErrorText[kGAErrorInternal], NULL, NULL); // Verify that the mutex is in fact locked. // If this fails, PPMalloc is at fault.
	#endif

	VerifyGuardFill(pChunk, true, false);
	VerifyDelayedFreeFill(pChunk);

	CollectMetrics(pChunk, false);

	// We call FreeInternal instead of Free or GeneralAllocator::Free because 
	// calling those functions would trigger the hook calling functionality,
	// which we don't want because we already called it for the given block
	// when it was first freed by the user.
	FreeInternal(GetDataPtrFromChunkPtr(pChunk));
}



///////////////////////////////////////////////////////////////////////////////
// CollectMetrics
//
void GeneralAllocatorDebug::CollectMetrics(const Chunk* pChunk, bool bMalloc)
{
	// No mutex lock here because this is an internal function.
	#if PPM_THREAD_SAFETY_SUPPORTED
	PPM_ASSERT(this, !mpMutex || PPMMutexGetLockCount(mpMutex) > 0, kGAErrorInternal, kGAErrorText[kGAErrorInternal], NULL, NULL); // Verify that the mutex is in fact locked. // If this fails, PPMalloc is at fault.
	#endif

	// We always increase mnAllocationCountHistorical, even if we have metrics disabled.
	// The reason for this is that we want to be able to always support kDebugDataIdAllocationCount
	if(bMalloc)
		mnAllocationCountHistorical++;

	if(mbMetricsEnabled && pChunk)
	{
		const size_t nMMapExtraSize   = (GetChunkIsMMapped(pChunk) ? pChunk->mnPriorSize : 0);
		const size_t nChunkSize       = GetChunkSize(pChunk) + nMMapExtraSize;
		const size_t nUsableChunkSize = GetUsableChunkSize(pChunk);
		const size_t nSystemSize      = kDataPtrOffset + nMMapExtraSize; // Note that actually this is 'kDataPtrOffset' (i.e. 8 bytes) for PPM_DEBUG but 'kDataPtrOffset / 2' (i.e. 4 bytes) for release builds for non-mmapped blocks.
		const size_t nDebugSize       = GetDebugDataLength(GetDataPtrFromChunkPtr(pChunk), kDebugDataLocationBlock);
		const size_t nUserSize        = (nUsableChunkSize - nDebugSize) - kDataPtrOffset;

		PPM_ASSERT(this, (nSystemSize + nDebugSize + nUserSize) == (nUsableChunkSize + nMMapExtraSize), kGAErrorInternal, kGAErrorText[kGAErrorInternal], NULL, NULL); // If this fails, PPMalloc is at fault.
		PPM_ASSERT(this, (nChunkSize == (nUsableChunkSize + nMMapExtraSize)) ||                                                                 // This covers debug build chunks which don't extend into the next contiguous chunk.
						 ((nChunkSize + kSizeTypeSize) == (nUsableChunkSize + nMMapExtraSize)), kGAErrorInternal, kGAErrorText[kGAErrorInternal], NULL, NULL);    // This covers release build chunks which extend into the next contiguous chunk by kSizeTypeSize bytes.

		if(bMalloc) // If this is from allocating, and not freeing...
		{
			// kMetricTypeAll
			mAllocationMetrics[kMetricTypeAll].mnAllocationCount++;
			mAllocationMetrics[kMetricTypeAll].mnAllocationCountHistorical++;
			mAllocationMetrics[kMetricTypeAll].mnAllocationVolume += nChunkSize;
			mAllocationMetrics[kMetricTypeAll].mnAllocationVolumeHistorical += nChunkSize;
		  //mAllocationMetrics[kMetricTypeAll].mnFreeVolume -= nChunkSize;
			if(mAllocationMetrics[kMetricTypeAll].mnAllocationCountMax  < mAllocationMetrics[kMetricTypeAll].mnAllocationCount)
			   mAllocationMetrics[kMetricTypeAll].mnAllocationCountMax  = mAllocationMetrics[kMetricTypeAll].mnAllocationCount;
			if(mAllocationMetrics[kMetricTypeAll].mnAllocationVolumeMax < mAllocationMetrics[kMetricTypeAll].mnAllocationVolume)
			   mAllocationMetrics[kMetricTypeAll].mnAllocationVolumeMax = mAllocationMetrics[kMetricTypeAll].mnAllocationVolume;

			// kMetricTypeSystem
			mAllocationMetrics[kMetricTypeSystem].mnAllocationVolume += nSystemSize;
			mAllocationMetrics[kMetricTypeSystem].mnAllocationVolumeHistorical += nSystemSize;
		  //mAllocationMetrics[kMetricTypeSystem].mnFreeVolume -= nSystemSize;

			// kMetricTypeDebug
			mAllocationMetrics[kMetricTypeDebug].mnAllocationVolume += nDebugSize;
			mAllocationMetrics[kMetricTypeDebug].mnAllocationVolumeHistorical += nDebugSize;
		  //mAllocationMetrics[kMetricTypeDebug].mnFreeVolume -= nDebugSize;

			// kMetricTypeUser
			mAllocationMetrics[kMetricTypeUser].mnAllocationVolume += nUserSize;
			mAllocationMetrics[kMetricTypeUser].mnAllocationVolumeHistorical += nUserSize;
		  //mAllocationMetrics[kMetricTypeUser].mnFreeVolume -= nUserSize;
		}
		else // Else this is from freeing.
		{
			// kMetricTypeAll
			mAllocationMetrics[kMetricTypeAll].mnAllocationCount--;
			mAllocationMetrics[kMetricTypeAll].mnAllocationVolume -= nChunkSize;
			mAllocationMetrics[kMetricTypeAll].mnFreeCountHistorical++;
		  //mAllocationMetrics[kMetricTypeAll].mnFreeVolume += nChunkSize;
			mAllocationMetrics[kMetricTypeAll].mnFreeVolumeHistorical += nChunkSize;

			// kMetricTypeSystem
			mAllocationMetrics[kMetricTypeSystem].mnAllocationVolume -= nSystemSize;
		  //mAllocationMetrics[kMetricTypeSystem].mnFreeVolume += nSystemSize;
			mAllocationMetrics[kMetricTypeSystem].mnFreeVolumeHistorical += nSystemSize;

			// kMetricTypeDebug
			mAllocationMetrics[kMetricTypeDebug].mnAllocationVolume -= nDebugSize;
		  //mAllocationMetrics[kMetricTypeDebug].mnFreeVolume = 0;
			mAllocationMetrics[kMetricTypeDebug].mnFreeVolumeHistorical += nDebugSize;

			// kMetricTypeUser
			mAllocationMetrics[kMetricTypeUser].mnAllocationVolume -= nUserSize;
		  //mAllocationMetrics[kMetricTypeUser].mnFreeVolume += nUserSize;
			mAllocationMetrics[kMetricTypeUser].mnFreeVolumeHistorical += nUserSize;
		}
	}
}



///////////////////////////////////////////////////////////////////////////////
// GetMetrics
//
const GeneralAllocatorDebug::Metrics& GeneralAllocatorDebug::GetMetrics(MetricType metricsType)
{
	#if PPM_THREAD_SAFETY_SUPPORTED
		PPMAutoMutex autoMutex(mpMutex);
	#endif

	if(metricsType != kMetricTypeAll)
	{
		// Replicate data which is normally only maintained in the 'mAllocationMetrics[kMetricTypeAll]' member.
		mAllocationMetrics[metricsType].mnAllocationCount           = mAllocationMetrics[kMetricTypeAll].mnAllocationCount;
		mAllocationMetrics[metricsType].mnAllocationCountHistorical = mAllocationMetrics[kMetricTypeAll].mnAllocationCountHistorical;
		mAllocationMetrics[metricsType].mnAllocationCountMax        = mAllocationMetrics[kMetricTypeAll].mnAllocationCountMax;
		mAllocationMetrics[metricsType].mnFreeCountHistorical       = mAllocationMetrics[kMetricTypeAll].mnFreeCountHistorical;
	}

	return mAllocationMetrics[metricsType];
}



///////////////////////////////////////////////////////////////////////////////
// GetAllocationCount
//
uint64_t GeneralAllocatorDebug::GetAllocationCount() const
{
	return mnAllocationCountHistorical;
}



///////////////////////////////////////////////////////////////////////////////
// SetOption
//
void GeneralAllocatorDebug::SetOption(int option, int64_t nValue)
{
	if(option == kOptionEnableMetrics)
	{
		// At this time, you can only enable this option before any allocations
		// have occured. Otherwise the allocation data may be incorrect or 
		// inconsistent.
		mbMetricsEnabled = (nValue != 0);
	}
	else if(option == kOptionEnablePtrValidation)
	{
		// At this time, you can only enable this option before any allocations 
		// have occurred. Otherwise asserts will trigger. This situation can
		// be rectified by rebuilding the hash table via a walk of the entire
		// heap. We'll leave that as a todo for now.
		const bool bHashTableEnabled = (nValue != 0);

		if(mbHashTableEnabled != bHashTableEnabled)
		{
			// If you have the debug data location set to 'separate', you must enable 
			// pointer validation. This is because the separate debug data is stored in
			// the same hash table where pointer validation is done.
			if(!bHashTableEnabled && (mnDebugDataLocationDefault == kDebugDataLocationSeparate))
				return; // Ignore the request to disable pointer validation.

			mbHashTableEnabled  = bHashTableEnabled;
			if(!bHashTableEnabled) // If disabling pointer validation...
				mHashTable.Clear(true, true);
		}
	}
	else
		GeneralAllocator::SetOption(option, nValue);
}


///////////////////////////////////////////////////////////////////////////////
// GetDebugInfoForAllocation
//
void GeneralAllocatorDebug::GetDebugInfoForAllocation(DebugDataInfo& debugDataInfo, size_t nSize, int nAllocationFlags, size_t nAlignment, size_t nAlignmentOffset)
{
	// No mutex lock here because this is an internal function.
	#if PPM_THREAD_SAFETY_SUPPORTED
	PPM_ASSERT(this, !mpMutex || PPMMutexGetLockCount(mpMutex) > 0, kGAErrorInternal, kGAErrorText[kGAErrorInternal], NULL, NULL); // Verify that the mutex is in fact locked. // If this fails, PPMalloc is at fault.
	#endif

	const unsigned nCurrentDebugDataFlags = (mnDebugDataFlags | mnNextDebugDataFlags);
	DebugDataInfoShared* pInfo;

	for(size_t i(0); i < kDebugDataIdInternalCount; i++)
	{
		debugDataInfo.mShared[i].mnRecordSize       = 0;
		debugDataInfo.mShared[i].mnRecordDataSize   = 0;
		debugDataInfo.mShared[i].mDebugDataLocation = kDebugDataLocationDefault;
	}
	debugDataInfo.mnUserSize = nSize; // Write this so that we know when writing guard fills which byte to start the fill at. We want the guard fill to start immediately after the user data.

	// Add Debug block size itself. 
	debugDataInfo.mnDebugDataSize = kDebugBlockSizeSize; // or equivalently: sizeof(DebugDataLengthType);

	// Add allocator field size.
	if(nCurrentDebugDataFlags & (1 << kDebugDataIdAllocator))
	{
		pInfo = &debugDataInfo.mShared[kDebugDataIdAllocator];
		pInfo->mnRecordDataSize   = sizeof(GeneralAllocatorDebug*);
		pInfo->mnRecordSize       = GetDebugDataRecordLength(pInfo->mnRecordDataSize);
		pInfo->mDebugDataLocation = GetDefaultDebugDataLocation(kDebugDataIdAllocator);
	}

	// Add size field size.
	if(nCurrentDebugDataFlags & (1 << kDebugDataIdSize))
	{
		pInfo = &debugDataInfo.mShared[kDebugDataIdSize];
		pInfo->mnRecordDataSize   = sizeof(debugDataInfo.mnUserSize);
		pInfo->mnRecordSize       = GetDebugDataRecordLength(pInfo->mnRecordDataSize);
		pInfo->mDebugDataLocation = GetDefaultDebugDataLocation(kDebugDataIdSize);
	}


	// Add allocation flags field size.
	if((nCurrentDebugDataFlags & (1 << kDebugDataIdFlags)) && nAllocationFlags)
	{
		pInfo = &debugDataInfo.mShared[kDebugDataIdFlags];
		pInfo->mnRecordDataSize   = sizeof(debugDataInfo.mnAllocationFlags);
		pInfo->mnRecordSize       = GetDebugDataRecordLength(pInfo->mnRecordDataSize);
		pInfo->mDebugDataLocation = GetDefaultDebugDataLocation(kDebugDataIdFlags);
		debugDataInfo.mnAllocationFlags = nAllocationFlags;
	}

	// Add alignment field size.
	if(nCurrentDebugDataFlags & (1 << kDebugDataIdAlignment))
	{
		pInfo = &debugDataInfo.mShared[kDebugDataIdAlignment];
		pInfo->mnRecordDataSize         = nAlignment ? (nAlignmentOffset ? 2 * sizeof(size_t) : sizeof(size_t)) : 0;
		pInfo->mnRecordSize             = GetDebugDataRecordLength(pInfo->mnRecordDataSize);
		pInfo->mDebugDataLocation       = GetDefaultDebugDataLocation(kDebugDataIdAlignment);
		debugDataInfo.mnAlignment       = nAlignment;
		debugDataInfo.mnAlignmentOffset = nAlignmentOffset;
	}

	// Add allocation name size.
	if(mpNextAllocationName) // Note that we don't check nCurrentDebugDataFlags for kDebugDataIdName here, though perhaps we could do so for consistency.
	{
		pInfo = &debugDataInfo.mShared[kDebugDataIdName];
		pInfo->mnRecordDataSize   = strlen(mpNextAllocationName) + 1; // '+ 1' for trailing 0.
		pInfo->mnRecordSize       = GetDebugDataRecordLength(pInfo->mnRecordDataSize);
		pInfo->mDebugDataLocation = GetDefaultDebugDataLocation(kDebugDataIdName);
	}

	// Add allocation place size.
	if(mpNextAllocationFile) // Note that we don't check nCurrentDebugDataFlags for kDebugDataIdPlace here, though perhaps we could do so for consistency.
	{
		pInfo = &debugDataInfo.mShared[kDebugDataIdPlace];
		pInfo->mnRecordDataSize   = sizeof(FileLineInfo);
		pInfo->mnRecordSize       = GetDebugDataRecordLength(pInfo->mnRecordDataSize);
		pInfo->mDebugDataLocation = GetDefaultDebugDataLocation(kDebugDataIdPlace);
		debugDataInfo.mFileLineInfo.mpFile = mpNextAllocationFile; // Note that we don't strcpy the value, but simply assign the pointer.
		debugDataInfo.mFileLineInfo.mnLine = mnNextAllocationLine; // This is because we assume the pointer's value is a constant.
	}

	// Add the call stack field size.
	#if PPM_DEBUG_CALLSTACK_AVAILABLE    // If the given platform supports call stack traces...
		if(nCurrentDebugDataFlags & (1 << kDebugDataIdCallStack))
		{
			pInfo = &debugDataInfo.mShared[kDebugDataIdCallStack];
			pInfo->mnRecordDataSize   = sizeof(void*) * GetCallStack(debugDataInfo.mpReturnAddressArray, 24);
			pInfo->mnRecordSize       = GetDebugDataRecordLength(pInfo->mnRecordDataSize);
			pInfo->mDebugDataLocation = GetDefaultDebugDataLocation(kDebugDataIdCallStack);
		}
	#endif

	// Add checksum field size.
	if(nCurrentDebugDataFlags & (1 << kDebugDataIdChecksum))
	{
		pInfo = &debugDataInfo.mShared[kDebugDataIdChecksum];
		pInfo->mnRecordDataSize   = sizeof(size_t);
		pInfo->mnRecordSize       = GetDebugDataRecordLength(pInfo->mnRecordDataSize);
		pInfo->mDebugDataLocation = GetDefaultDebugDataLocation(kDebugDataIdChecksum);
	}

	// Add historical count size.
	if(nCurrentDebugDataFlags & (1 << kDebugDataIdAllocationCount))
	{
		pInfo = &debugDataInfo.mShared[kDebugDataIdAllocationCount];
		pInfo->mnRecordDataSize   = sizeof(mnAllocationCountHistorical);
		pInfo->mnRecordSize       = GetDebugDataRecordLength(pInfo->mnRecordDataSize);
		pInfo->mDebugDataLocation = GetDefaultDebugDataLocation(kDebugDataIdAllocationCount);
	}

	// Add time field size.
	if(nCurrentDebugDataFlags & (1 << kDebugDataIdAllocationTime))
	{
		pInfo = &debugDataInfo.mShared[kDebugDataIdAllocationTime];
		pInfo->mnRecordDataSize   = sizeof(size_t);
		pInfo->mnRecordSize       = GetDebugDataRecordLength(pInfo->mnRecordDataSize);
		pInfo->mDebugDataLocation = GetDefaultDebugDataLocation(kDebugDataIdAllocationTime);
	}

	// Add guard fill field size.
	if(nCurrentDebugDataFlags & (1 << kDebugDataIdGuard))
	{
		pInfo = &debugDataInfo.mShared[kDebugDataIdGuard];
		pInfo->mnRecordDataSize   = GetGuardSizeForBlock(nSize);
		pInfo->mnRecordSize       = GetDebugDataRecordLength(pInfo->mnRecordDataSize);
		pInfo->mDebugDataLocation = kDebugDataLocationBlock; // Guard fills by design always go into the allocation block.
	}

	// Add overhead field size.
	if(nCurrentDebugDataFlags & (1 << kDebugDataIdOverhead))
	{
		pInfo = &debugDataInfo.mShared[kDebugDataIdOverhead];
		pInfo->mnRecordDataSize   = sizeof(size_t);
		pInfo->mnRecordSize       = GetDebugDataRecordLength(pInfo->mnRecordDataSize);
		pInfo->mDebugDataLocation = GetDefaultDebugDataLocation(kDebugDataIdOverhead);
	}

	// Add group id size.
	if(nCurrentDebugDataFlags & (1 << kDebugDataIdGroup))
	{
		pInfo = &debugDataInfo.mShared[kDebugDataIdGroup];
		pInfo->mnRecordDataSize   = sizeof(mnCurrentGroupId);
		pInfo->mnRecordSize       = GetDebugDataRecordLength(pInfo->mnRecordDataSize);
		pInfo->mDebugDataLocation = GetDefaultDebugDataLocation(kDebugDataIdGroup);
	}

	// Get the sums of the memory usage.
	debugDataInfo.mnBlockUsage    = debugDataInfo.mnDebugDataSize;
	debugDataInfo.mnSeparateUsage = debugDataInfo.mnDebugDataSize;

	for(size_t j(0); j < kDebugDataIdInternalCount; j++)
	{
		if(debugDataInfo.mShared[j].mDebugDataLocation == kDebugDataLocationBlock)
			debugDataInfo.mnBlockUsage    += debugDataInfo.mShared[j].mnRecordSize;
		else
			debugDataInfo.mnSeparateUsage += debugDataInfo.mShared[j].mnRecordSize;
	}
}


///////////////////////////////////////////////////////////////////////////////
// SetDebugInfoForAllocation
//
bool GeneralAllocatorDebug::SetDebugInfoForAllocation(DebugDataInfo& debugDataInfo, void* pData, bool bAttemptNewFill)
{
	// No mutex lock here because this is an internal function.
	#if PPM_THREAD_SAFETY_SUPPORTED
	PPM_ASSERT(this, !mpMutex || PPMMutexGetLockCount(mpMutex) > 0, kGAErrorInternal, kGAErrorText[kGAErrorInternal], NULL, NULL); // Verify that the mutex is in fact locked. // If this fails, PPMalloc is at fault.
	#endif

	const Chunk* const pChunk = GetChunkPtrFromDataPtr(pData);
	const size_type    nUsableChunkSize = GetUsableChunkSize(pChunk); // GetUsableChunkSize returns the amount of memory in the chunk which is available to the caller of Malloc. May be slightly bigger than the originally requested size.

	// Write the 'new' fill.
	if(bAttemptNewFill && (mnFillNew != kDefaultFillValueDisabled))
	{
		// Note that the byte count written here may be greater than the initially requested size.
		// Note also that some of the bytes we fill here will be overwritten below.
		const size_t nUsableSize = GeneralAllocator::GetUsableSize(pData); // Intentionally call inherited version.
		memset(pData, mnFillNew, nUsableSize);
	}

	// Write in the debug data size itself. 
	char* const pPosition = (char*)pChunk + nUsableChunkSize - sizeof(DebugDataLengthType);
	memset(pPosition, 0, sizeof(DebugDataLengthType)); // It is zero now, but will be updated below as we add individual records.

	// It's important that we do that after setting the debug data size to zero above because
	// the AddToDebugDataMap function could trigger internal allocations and block validations
	// and if the debug data size was not initialized then a crash would likely occur.
	if(AddToDebugDataMap(pData, debugDataInfo.mnSeparateUsage, NULL, false))
	{
		if((debugDataInfo.mnBlockUsage     > kDebugBlockSizeSize) || // If there is any debug information...
			(debugDataInfo.mnSeparateUsage > kDebugBlockSizeSize))
		{
			DebugDataInfoShared* pInfo;

			// Write allocator field.
			pInfo = &debugDataInfo.mShared[kDebugDataIdAllocator];
			if(pInfo->mnRecordSize)
			{
				PPM_ASSERT(this, pInfo->mnRecordDataSize == sizeof(GeneralAllocatorDebug*), kGAErrorInternal, kGAErrorText[kGAErrorInternal], NULL, NULL); // If this fails, PPMalloc is at fault.
				const GeneralAllocatorDebug* const pThis = this;
				SetDebugData(pData, kDebugDataIdAllocator, &pThis, pInfo->mnRecordDataSize, pInfo->mDebugDataLocation);
			}

			// Write size field.
			pInfo = &debugDataInfo.mShared[kDebugDataIdSize];
			if(pInfo->mnRecordSize)
			{
				PPM_ASSERT(this, pInfo->mnRecordDataSize == sizeof(debugDataInfo.mnUserSize), kGAErrorInternal, kGAErrorText[kGAErrorInternal], NULL, NULL); // If this fails, PPMalloc is at fault.
				SetDebugData(pData, kDebugDataIdSize, &debugDataInfo.mnUserSize, pInfo->mnRecordDataSize, pInfo->mDebugDataLocation);
			}

			// Write allocation flags field.
			pInfo = &debugDataInfo.mShared[kDebugDataIdFlags];
			if(pInfo->mnRecordSize)
			{
				PPM_ASSERT(this, pInfo->mnRecordDataSize == sizeof(debugDataInfo.mnAllocationFlags), kGAErrorInternal, kGAErrorText[kGAErrorInternal], NULL, NULL); // If this fails, PPMalloc is at fault.
				SetDebugData(pData, kDebugDataIdFlags, &debugDataInfo.mnAllocationFlags, pInfo->mnRecordDataSize, pInfo->mDebugDataLocation);
			}

			// Write alignment field.
			pInfo = &debugDataInfo.mShared[kDebugDataIdAlignment];
			if(pInfo->mnRecordSize)
			{
				if(debugDataInfo.mnAlignmentOffset)
				{
					const size_t pArray[2] = { debugDataInfo.mnAlignment, debugDataInfo.mnAlignmentOffset};
					PPM_ASSERT(this, pInfo->mnRecordDataSize == sizeof(pArray), kGAErrorInternal, kGAErrorText[kGAErrorInternal], NULL, NULL); // If this fails, PPMalloc is at fault.
					SetDebugData(pData, kDebugDataIdAlignment, pArray, pInfo->mnRecordDataSize, pInfo->mDebugDataLocation);
				}
				else
				{
					PPM_ASSERT(this, pInfo->mnRecordDataSize == sizeof(debugDataInfo.mnAlignment), kGAErrorInternal, kGAErrorText[kGAErrorInternal], NULL, NULL); // If this fails, PPMalloc is at fault.
					SetDebugData(pData, kDebugDataIdAlignment, &debugDataInfo.mnAlignment, pInfo->mnRecordDataSize, pInfo->mDebugDataLocation);
				}
			}

			// Write name field.
			pInfo = &debugDataInfo.mShared[kDebugDataIdName];
			if(pInfo->mnRecordSize)
			{
				// To be most safe, we protect agains an mpNextAllocationName that is NULL, even though that shouldn't normally happen.
				if(mpNextAllocationName)
				{
					PPM_ASSERT(this, pInfo->mnRecordDataSize == (strlen(mpNextAllocationName) + 1), kGAErrorInternal, kGAErrorText[kGAErrorInternal], NULL, NULL); // If this fails, PPMalloc is at fault.
					SetDebugData(pData, kDebugDataIdName, mpNextAllocationName, pInfo->mnRecordDataSize, pInfo->mDebugDataLocation);
				}
				else
				{
					const char emptyName = 0;
					SetDebugData(pData, kDebugDataIdName, &emptyName, 1, pInfo->mDebugDataLocation);
				}
			}

			// Write place field.
			pInfo = &debugDataInfo.mShared[kDebugDataIdPlace];
			if(pInfo->mnRecordSize)
			{
				PPM_ASSERT(this, pInfo->mnRecordDataSize == sizeof(debugDataInfo.mFileLineInfo), kGAErrorInternal, kGAErrorText[kGAErrorInternal], NULL, NULL); // If this fails, PPMalloc is at fault.
				SetDebugData(pData, kDebugDataIdPlace, &debugDataInfo.mFileLineInfo, pInfo->mnRecordDataSize, pInfo->mDebugDataLocation);
			}

			// Write call stack field.
			#if PPM_DEBUG_CALLSTACK_AVAILABLE
				pInfo = &debugDataInfo.mShared[kDebugDataIdCallStack];
				if(pInfo->mnRecordSize)
				{
					// To do: enable this: PPM_ASSERT(this, (pInfo->mnRecordDataSize % sizeof(uintptr_t)) == 0, kGAErrorInternal, kGAErrorText[kGAErrorInternal], NULL, NULL); // If this fails, PPMalloc is at fault.
					SetDebugData(pData, kDebugDataIdCallStack, debugDataInfo.mpReturnAddressArray, pInfo->mnRecordDataSize, pInfo->mDebugDataLocation);
				}
			#endif

			// Write checksum field.
			pInfo = &debugDataInfo.mShared[kDebugDataIdChecksum];
			if(pInfo->mnRecordSize)
			{
				size_t zero(0);
				PPM_ASSERT(this, pInfo->mnRecordDataSize == sizeof(zero), kGAErrorInternal, kGAErrorText[kGAErrorInternal], NULL, NULL); // If this fails, PPMalloc is at fault.
				SetDebugData(pData, kDebugDataIdChecksum, &zero, pInfo->mnRecordDataSize, pInfo->mDebugDataLocation);
			}

			// Write count field.
			pInfo = &debugDataInfo.mShared[kDebugDataIdAllocationCount];
			if(pInfo->mnRecordSize)
			{
				PPM_ASSERT(this, pInfo->mnRecordDataSize == sizeof(mnAllocationCountHistorical), kGAErrorInternal, kGAErrorText[kGAErrorInternal], NULL, NULL); // If this fails, PPMalloc is at fault.
				SetDebugData(pData, kDebugDataIdAllocationCount, &mnAllocationCountHistorical, pInfo->mnRecordDataSize, pInfo->mDebugDataLocation);
			}

			// Write time field.
			pInfo = &debugDataInfo.mShared[kDebugDataIdAllocationTime];
			if(pInfo->mnRecordSize)
			{
				const size_t nCurrentTime = (size_t)GetPPMallocDebugTime();
				PPM_ASSERT(this, pInfo->mnRecordDataSize == sizeof(nCurrentTime), kGAErrorInternal, kGAErrorText[kGAErrorInternal], NULL, NULL); // If this fails, PPMalloc is at fault.
				SetDebugData(pData, kDebugDataIdAllocationTime, &nCurrentTime, pInfo->mnRecordDataSize, pInfo->mDebugDataLocation);
			}

			// We write the guard record *last* (below). 
			// This is so that overruns get caught more easily.

			// Write overhead field.
			pInfo = &debugDataInfo.mShared[kDebugDataIdOverhead];
			if(pInfo->mnRecordSize)
			{
				// Note that in an optimized build, PPMalloc has 'nUsableChunkSize - nSize - sizeof(size_type)' overhead, 
				// due to the end of the allocation chunk intentionally running over into the first four bytes of
				// the next contiguous chunk. We have a dilemma here -- do we report the debug overhead or the 
				// optimized overhead. The debug overhead is more accurate right now in a debug build but that's
				// not usually what the user wants. Similarly, we have the same issue with sizeof(DebugDataLengthType).
				// What we do is subtract the four bytes but not the user-added debug info. This way the information
				// is closest to what the user actually expects.
				const size_t nOverhead = (nUsableChunkSize - debugDataInfo.mnUserSize) - kSizeTypeSize; 
				PPM_ASSERT(this, pInfo->mnRecordDataSize == sizeof(nOverhead), kGAErrorInternal, kGAErrorText[kGAErrorInternal], NULL, NULL); // If this fails, PPMalloc is at fault.
				SetDebugData(pData, kDebugDataIdOverhead, &nOverhead, pInfo->mnRecordDataSize, pInfo->mDebugDataLocation);
			}

			// Write group id size field.
			pInfo = &debugDataInfo.mShared[kDebugDataIdGroup];
			if(pInfo->mnRecordSize)
			{
				PPM_ASSERT(this, pInfo->mnRecordDataSize == sizeof(mnCurrentGroupId), kGAErrorInternal, kGAErrorText[kGAErrorInternal], NULL, NULL); // If this fails, PPMalloc is at fault.
				SetDebugData(pData, kDebugDataIdGroup, &mnCurrentGroupId, pInfo->mnRecordDataSize, pInfo->mDebugDataLocation);
			}

			// Write guard record. 
			// It's important that the guard record be written last, as that would put it closest to the user data.
			pInfo = &debugDataInfo.mShared[kDebugDataIdGuard];
			if(pInfo->mnRecordSize)
			{
				void* pGuardField = SetDebugData(pData, kDebugDataIdGuard, NULL, pInfo->mnRecordDataSize, pInfo->mDebugDataLocation);
				PPM_ASSERT(this, pGuardField, kGAErrorInternal, kGAErrorText[kGAErrorInternal], NULL, NULL); // If this fails, PPMalloc is at fault.

				if(pGuardField)
				{
					memset(pGuardField, mnFillGuard, pInfo->mnRecordDataSize);

					// If there is any space between the user data and the guard data,
					// we extend the guard data back to right after the user data.
					// This is useful because we want to detect any memory overruns that
					// the user code might mistakenly do. We have to do this here and
					// cannot precalculate the extra space because we can't know ahead
					// of time what block size the allocator will give us.
					intptr_t nDifference = (intptr_t)((const char*)pGuardField - ((const char*)pData + debugDataInfo.mnUserSize));
					PPM_ASSERT(this, nDifference >= 0, kGAErrorInternal, kGAErrorText[kGAErrorInternal], NULL, NULL);  // Ensure the difference was not negative. If it was negative then it means we miscalculated the guard area position/size such that the user space is running into it, whereas it should be after the user space in the block.

					// Disabled since there were cases this was being triggered for large mapped memory allocations. Perhaps re-enable if we can handle these unusual cases.
					// const int kMaxDifference = 65536;
					// PPM_ASSERT(this, (nDifference >= 0) && (nDifference < kMaxDifference), kGAErrorInternal, kGAErrorText[kGAErrorInternal], NULL, NULL);  // Ensure the difference was not negative.

					if(nDifference) // If there is any space...
					{
						// Do a small fill on the difference area.
						memset((char*)pData + debugDataInfo.mnUserSize, mnFillGuard, (size_t)nDifference);

						// Write the new field size (size of just the guard fill area).
						PPM_ASSERT(this, (size_t)nDifference <= (1 << (sizeof(DebugDataLengthType) * 8)), kGAErrorInternal, kGAErrorText[kGAErrorInternal], NULL, NULL); // DebugDataLengthType can only count this many bytes. 
						const DebugDataLengthType nFieldSize = (DebugDataLengthType)(pInfo->mnRecordDataSize + nDifference);
						memcpy((char*)pGuardField + pInfo->mnRecordDataSize + kDebugRecordIdSize, &nFieldSize, sizeof(nFieldSize));

						// Write the new debug data size (size of all debug data).
						DebugDataLengthType nDebugDataSize; 
						char* const pDebugDataSize = (char*)pChunk + nUsableChunkSize - sizeof(DebugDataLengthType);
						memcpy(&nDebugDataSize, pDebugDataSize, sizeof(nDebugDataSize));
						nDebugDataSize = (DebugDataLengthType)(nDebugDataSize + nDifference);
						memcpy(pDebugDataSize, &nDebugDataSize, sizeof(nDebugDataSize));
					}
				}
			}

			// Don't put any new record types here; they should be done before the guard record.
		}

		return true;
	}

	return false;
}



///////////////////////////////////////////////////////////////////////////////
// FormatPlace
//
// We make a string like this:
//     __FILE__, __LINE__
//     Simulation.cpp, 33
// or this:
//     __FILE__, __LINE__, EA_CURRENT_FUNCTION
//     Simulation.cpp, 33, SimulateCars()
// or this:
//     EA_CURRENT_FUNCTION
//     SimulateCars()
//
// Returns strlen of resulting pBuffer.
//
size_t GeneralAllocatorDebug::FormatPlace(char* pBuffer, size_t nBufferLength, const char* pFile, int nLine, const char* pFunction) const
{
	// No mutex lock here because this is an internal function.
	#if PPM_THREAD_SAFETY_SUPPORTED
	PPM_ASSERT(this, !mpMutex || PPMMutexGetLockCount(mpMutex) > 0, kGAErrorInternal, kGAErrorText[kGAErrorInternal], NULL, NULL); // Verify that the mutex is in fact locked. // If this fails, PPMalloc is at fault.
	#endif

	char* pCurrent = pBuffer;
	char* pEnd      = pBuffer + nBufferLength - 1; // -1 to leave space for null-terminator.

	*pCurrent = 0;

	if(pFile)
	{
		// Throw away the directory, leaving just the file name.
		const char* pFileUsed = pFile + strlen(pFile);
		while((pFileUsed >= pFile) && (*pFileUsed != '\\') && (*pFileUsed != '/')) // This check should work well enough for all supported platforms.
			--pFileUsed;
		++pFileUsed;

		// Copy the file name
		while((pCurrent < pEnd) && *pFileUsed)
			*pCurrent++ = *pFileUsed++;

		// Add a comma
		if((pCurrent + 2) < pEnd)
		{
			*pCurrent++ = ',';
			*pCurrent++ = ' ';
		}

		// Append the line number
		char buffer[32];
		sprintf(buffer, "%d", nLine);
		const char* pLine = buffer;
		while((pCurrent < pEnd) && *pLine)
			*pCurrent++ = *pLine++;
	}

	if(pFunction)
	{
		if(pFile)
		{
			// Add a comma
			if((pCurrent + 2) < pEnd)
			{
				*pCurrent++ = ',';
				*pCurrent++ = ' ';
			}
		}

		// Copy the function name
		while(pCurrent < pEnd)
			*pCurrent++ = *pFunction++;
	}

	// Place a terminating zero. This should always work because we left space for it earlier.
	*pCurrent = 0;

	return (size_t)(pCurrent - pBuffer); // This is the same as strlen(pBuffer).
}


///////////////////////////////////////////////////////////////////////////////
// Malloc
//
// Note that we replicate the functionality of GeneralAllocator::Malloc here
// (e.g. calling the hook function) instead of calling GeneralAllocator::Malloc
// from here. The reason for this is that the second hook call *must* be made
// after we've assigned debug info to the allocation, but that debug info 
// is not assigned until after we return from calling GeneralAllocator. 
// So we simply take over the functionality here.
//
void* GeneralAllocatorDebug::Malloc(size_t nUserSize, int nAllocationFlags)
{
	void* pReturnValue;

	// We need to lock the mutex here before calling the external hook function. 
	// We cannot keep mutexes locked while calling external code. The only 
	// reason we need to lock the mutex here is that we currently access
	// mpAllocationName, etc. in GetDebugInfoForAllocation. If you are concerned
	// about this mutex now, contact Paul Pedriana or somebody else in charge of 
	// this library to work on the update.
	#if PPM_THREAD_SAFETY_SUPPORTED
		PPMMutexLock(mpMutex);
	#endif

	AtomicUint nEventId = PPMAtomicIncrement(mEventId);

	DebugDataInfo debugDataInfoScratch;
	GetDebugInfoForAllocation(debugDataInfoScratch, nUserSize, nAllocationFlags, kMinEffectiveAlignment);
	const size_t nSize = nUserSize + debugDataInfoScratch.mnBlockUsage;

	if(nSize < nUserSize) // If there was integer wraparound due to an overly large request...
	{
		#if PPM_THREAD_SAFETY_SUPPORTED
			PPMMutexUnlock(mpMutex);
		#endif
		return NULL;
	}

	#if PPM_THREAD_SAFETY_SUPPORTED
		if(!mbLockDuringHookCalls)
			PPMMutexUnlock(mpMutex);
	#endif

	#if PPM_HOOKS_SUPPORTED
		HookInfo hookInfo;

		if(mpHookFunction)
		{
			hookInfo.mpGeneralAllocator = this; 
			hookInfo.mbEntry            = true;
			hookInfo.mHookType          = kHookTypeMalloc;
			hookInfo.mHookSubType       = kHookSubTypeMalloc;
			hookInfo.mnSizeInputTotal   = nUserSize;    // This is the input size, not the lower level allocated size. mnSizeOutput will 
			hookInfo.mpDataInput        = NULL;         // contain the final allocated size in the exit hook callback below in this function.
			hookInfo.mnCountInput       = 0;
			hookInfo.mnSizeInput        = nUserSize;
			hookInfo.mpSizeInputArray   = NULL;
			hookInfo.mnAlignmentInput   = 0;
			hookInfo.mnAllocationFlags  = nAllocationFlags;
			hookInfo.mpDataOutput       = NULL;
			hookInfo.mpArrayOutput      = NULL;
			hookInfo.mnSizeOutput       = 0;
			hookInfo.mEventId           = nEventId;

			mpHookFunction(&hookInfo, mpHookFunctionContext);
		}
	#else
		EA_UNUSED(nEventId);
	#endif

	#if PPM_THREAD_SAFETY_SUPPORTED
		if(!mbLockDuringHookCalls)
			PPMMutexLock(mpMutex);
	#endif

	TryAllocation:
	{
		pReturnValue = MallocInternal(nSize, nAllocationFlags); 

		if(pReturnValue)
		{
			if(SetDebugInfoForAllocation(debugDataInfoScratch, pReturnValue, true)) // This will in turn call AddToDebugDataMap.
			{
				CollectMetrics(GetChunkPtrFromDataPtr(pReturnValue), true);

				#if (PPM_DEBUG >= 3) // Developer builds only.
					DebugDataLengthType nDebugDataLength;
					Chunk* const pChunk     = GetChunkPtrFromDataPtr(pReturnValue);
					const size_t nChunkSize = GetUsableChunkSize(pChunk);
					memcpy(&nDebugDataLength, (char*)pChunk + nChunkSize - sizeof(DebugDataLengthType), sizeof(DebugDataLengthType));
					PPM_ASSERT(this, nUserSize <= (nChunkSize - kDataPtrOffset - nDebugDataLength - sizeof(DebugDataLengthType)), kGAErrorCorruptedHeap, kGAErrorText[kGAErrorCorruptedHeap], pChunk, pChunk); // If this fails, PPMalloc may be at fault. The user may have corrupted the heap, alternatively.
				#endif
			}
			else
			{
				FreeInternal(pReturnValue);
				pReturnValue = NULL;
			}
		}
		else
		{
			// Else the malloc failed. If we have any delayed free blocks, we can free them and try again.
			if(mDelayedFreeChunkListHead.mpNextChunk != &mDelayedFreeChunkListHead) // If there is anything to free...
			{
				ClearDelayedFreeList(kDelayedFreePolicyNone, 0);
				goto TryAllocation; // Jump back above and try again.
			}
		}
	}

	#if PPM_THREAD_SAFETY_SUPPORTED
		if(!mbLockDuringHookCalls)
			PPMMutexUnlock(mpMutex);
	#endif

	#if PPM_HOOKS_SUPPORTED
		if(mpHookFunction)
		{
			hookInfo.mbEntry      = false;
			hookInfo.mpDataOutput = pReturnValue;
			hookInfo.mnSizeOutput  = GetUsableSize(pReturnValue); // or GetBlockSize(pReturnValue, true);

			mpHookFunction(&hookInfo, mpHookFunctionContext);
		}
	#endif

	#if PPM_THREAD_SAFETY_SUPPORTED
		if(mbLockDuringHookCalls)
			PPMMutexUnlock(mpMutex);
	#endif

	return pReturnValue;
}



///////////////////////////////////////////////////////////////////////////////
// MallocDebug
//
void* GeneralAllocatorDebug::MallocDebug(size_t nSize, int nAllocationFlags,  
										 unsigned nDebugDataFlags, const char* pName, const char* pFile, int nLine)
{
	// We leave the mutex locked for this call, though ideally we would respect mbLockDuringHookCalls.
	// As it stands now we have no choice about locking here and must lock for 
	// the duration of the operation. This probably isn't an issue but under 
	// some circumstances can be a potential issue when hooks are installed.
	#if PPM_THREAD_SAFETY_SUPPORTED
		PPMAutoMutex autoMutex(mpMutex);
	#endif

	mnNextDebugDataFlags = nDebugDataFlags;
	mpNextAllocationName = pName;
	mpNextAllocationFile = pFile;
	mnNextAllocationLine = nLine;

	void* const pReturnValue = Malloc(nSize, nAllocationFlags);

	mnNextDebugDataFlags = 0;
	mpNextAllocationName = NULL;
	mpNextAllocationFile = NULL;
	mnNextAllocationLine = 0;

	return pReturnValue;
}



///////////////////////////////////////////////////////////////////////////////
// MallocAligned
//
void* GeneralAllocatorDebug::MallocAligned(size_t nUserSize, size_t nAlignment, size_t nAlignmentOffset, int nAllocationFlags)
{
	void* pReturnValue;

	// See GeneralAllocatorDebug::Malloc for comments about this mutex locking.
	#if PPM_THREAD_SAFETY_SUPPORTED
		PPMMutexLock(mpMutex);
	#endif

	AtomicUint nEventId = PPMAtomicIncrement(mEventId);

	DebugDataInfo debugDataInfo;
	GetDebugInfoForAllocation(debugDataInfo, nUserSize, nAllocationFlags, nAlignment, nAlignmentOffset);
	const size_t nSize = nUserSize + debugDataInfo.mnBlockUsage;

	if(nSize < nUserSize) // If there was integer wraparound due to an overly large request...
	{
		#if PPM_THREAD_SAFETY_SUPPORTED
			PPMMutexUnlock(mpMutex);
		#endif
		return NULL;
	}

	#if PPM_THREAD_SAFETY_SUPPORTED
		if(!mbLockDuringHookCalls)
			PPMMutexUnlock(mpMutex);
	#endif

	#if PPM_HOOKS_SUPPORTED
		HookInfo hookInfo;

		if(mpHookFunction)
		{
			hookInfo.mpGeneralAllocator = this; 
			hookInfo.mbEntry            = true;
			hookInfo.mHookType          = kHookTypeMalloc;
			hookInfo.mHookSubType       = kHookSubTypeMallocAligned;
			hookInfo.mnSizeInputTotal   = nUserSize;
			hookInfo.mpDataInput        = NULL;
			hookInfo.mnCountInput       = 0;
			hookInfo.mnSizeInput        = nUserSize;
			hookInfo.mpSizeInputArray   = NULL;
			hookInfo.mnAlignmentInput   = nAlignment;
			hookInfo.mnAllocationFlags  = nAllocationFlags;
			hookInfo.mpDataOutput       = NULL;
			hookInfo.mpArrayOutput      = NULL;
			hookInfo.mnSizeOutput       = nAlignmentOffset;
			hookInfo.mEventId           = nEventId;

			mpHookFunction(&hookInfo, mpHookFunctionContext);
		}
	#else
		EA_UNUSED(nEventId);
	#endif

	#if PPM_THREAD_SAFETY_SUPPORTED
		if(!mbLockDuringHookCalls)
			PPMMutexLock(mpMutex);
	#endif

	TryAllocation:
	{
		pReturnValue = GeneralAllocator::MallocAlignedInternal(nSize, nAlignment, nAlignmentOffset, nAllocationFlags); 
		PPM_ASSERT(this, (nAlignment == 0) || PPM_IsAligned((char*)pReturnValue + nAlignmentOffset, nAlignment), kGAErrorInternal, kGAErrorText[kGAErrorInternal], NULL, pReturnValue); // If this fails, PPMalloc is at fault.

		if(pReturnValue)
		{
			if(SetDebugInfoForAllocation(debugDataInfo, pReturnValue, true)) // This will in turn call AddToDebugDataMap.
				CollectMetrics(GetChunkPtrFromDataPtr(pReturnValue), true);
			else
			{
				FreeInternal(pReturnValue);
				pReturnValue = NULL;
			}
		}
		else
		{
			// Else the malloc failed. If we have any delayed free blocks, we can free them and try again.
			if(mDelayedFreeChunkListHead.mpNextChunk != &mDelayedFreeChunkListHead) // If there is anything to free...
			{
				ClearDelayedFreeList(kDelayedFreePolicyNone, 0);
				goto TryAllocation; // Jump back above and try again.
			}
		}
	}

	#if PPM_THREAD_SAFETY_SUPPORTED
		if(!mbLockDuringHookCalls)
			PPMMutexUnlock(mpMutex);
	#endif

	#if PPM_HOOKS_SUPPORTED
		if(mpHookFunction)
		{
			hookInfo.mbEntry       = false;
			hookInfo.mpDataOutput  = pReturnValue;
			hookInfo.mnSizeOutput  = GetUsableSize(pReturnValue); // or GetBlockSize(pReturnValue, true);

			mpHookFunction(&hookInfo, mpHookFunctionContext);
		}
	#endif

	#if PPM_THREAD_SAFETY_SUPPORTED
		if(mbLockDuringHookCalls)
			PPMMutexUnlock(mpMutex);
	#endif

	return pReturnValue;
}


void* GeneralAllocatorDebug::MallocAlignedDebug(size_t nSize, size_t nAlignment, size_t nAlignmentOffset, int nAllocationFlags,
												unsigned nDebugDataFlags, const char* pName, const char* pFile, int nLine)
{
	// We leave the mutex locked for this call, though ideally we would respect mbLockDuringHookCalls.
	// As it stands now we have no choice about locking here and must lock for 
	// the duration of the operation. This probably isn't an issue but under 
	// some circumstances can be a potential issue when hooks are installed.
	#if PPM_THREAD_SAFETY_SUPPORTED
		PPMAutoMutex autoMutex(mpMutex);
	#endif

	mnNextDebugDataFlags = nDebugDataFlags;
	mpNextAllocationName = pName;
	mpNextAllocationFile = pFile;
	mnNextAllocationLine = nLine;

	void* const pReturnValue = MallocAligned(nSize, nAlignment, nAlignmentOffset, nAllocationFlags);
	PPM_ASSERT(this, (nAlignment == 0) || PPM_IsAligned((char*)pReturnValue + nAlignmentOffset, nAlignment), kGAErrorInternal, kGAErrorText[kGAErrorInternal], NULL, pReturnValue); // If this fails, PPMalloc is at fault.

	mnNextDebugDataFlags = 0;
	mpNextAllocationName = NULL;
	mpNextAllocationFile = NULL;
	mnNextAllocationLine = 0;

	return pReturnValue;
}



///////////////////////////////////////////////////////////////////////////////
// Calloc
//
void* GeneralAllocatorDebug::Calloc(size_t nElementCount, size_t nElementSize, int nAllocationFlags)
{
	void* pReturnValue;

	// We have a little problem here because calloc uses two arguments:
	// one for element count and one for element size. So we can't just
	// add nNewSize to either of these and have decent efficiency. So the 
	// best we can do yet still call GeneralAllocator::Calloc is to find out how
	// to best modify either nElementCount and/or nElementSize to allow 
	// the debug information to be added most efficiently. We do this by 
	// simply setting element count to one and element size to the newly 
	// required size.
	const size_t nCallocSize = nElementCount * nElementSize;

	// See GeneralAllocatorDebug::Malloc for comments about this mutex locking.
	#if PPM_THREAD_SAFETY_SUPPORTED
		PPMMutexLock(mpMutex);
	#endif

	AtomicUint nEventId = PPMAtomicIncrement(mEventId);

	DebugDataInfo debugDataInfo;
	GetDebugInfoForAllocation(debugDataInfo, nCallocSize, nAllocationFlags, kMinEffectiveAlignment);
	const size_t nSize = nCallocSize + debugDataInfo.mnBlockUsage;

	if(nSize < nCallocSize) // If there was integer wraparound due to an overly large request...
	{
		#if PPM_THREAD_SAFETY_SUPPORTED
			PPMMutexUnlock(mpMutex);
		#endif
		return NULL;
	}

	#if PPM_THREAD_SAFETY_SUPPORTED
		if(!mbLockDuringHookCalls)
			PPMMutexUnlock(mpMutex);
	#endif

	#if PPM_HOOKS_SUPPORTED
		HookInfo hookInfo;

		if(mpHookFunction)
		{
			hookInfo.mpGeneralAllocator = this; 
			hookInfo.mbEntry            = true;
			hookInfo.mHookType          = kHookTypeMalloc;
			hookInfo.mHookSubType       = kHookSubTypeCalloc;
			hookInfo.mnSizeInputTotal   = nCallocSize;
			hookInfo.mpDataInput        = NULL;
			hookInfo.mnCountInput       = nElementCount;
			hookInfo.mnSizeInput        = nElementSize;
			hookInfo.mpSizeInputArray   = NULL;
			hookInfo.mnAlignmentInput   = 0;
			hookInfo.mnAllocationFlags  = nAllocationFlags;
			hookInfo.mpDataOutput       = NULL;
			hookInfo.mpArrayOutput      = NULL;
			hookInfo.mnSizeOutput       = 0;
			hookInfo.mEventId           = nEventId;

			mpHookFunction(&hookInfo, mpHookFunctionContext);
		}
	#else
		EA_UNUSED(nEventId);
	#endif

	#if PPM_THREAD_SAFETY_SUPPORTED
		if(!mbLockDuringHookCalls)
			PPMMutexLock(mpMutex);
	#endif

	TryAllocation:
	{
		pReturnValue = GeneralAllocator::CallocInternal(1, nSize, nAllocationFlags); 

		if(pReturnValue)
		{
			if(SetDebugInfoForAllocation(debugDataInfo, pReturnValue, false)) // This will in turn call AddToDebugDataMap. Call with 'false' here (don't do new fill) because Calloc returns memory filled with zero.
				CollectMetrics(GetChunkPtrFromDataPtr(pReturnValue), true);
			else
			{
				FreeInternal(pReturnValue);
				pReturnValue = NULL;
			}
		}
		else
		{
			// Else the malloc failed. If we have any delayed free blocks, we can free them and try again.
			if(mDelayedFreeChunkListHead.mpNextChunk != &mDelayedFreeChunkListHead) // If there is anything to free...
			{
				ClearDelayedFreeList(kDelayedFreePolicyNone, 0);
				goto TryAllocation; // Jump back above and try again.
			}
		}
	}

	#if PPM_THREAD_SAFETY_SUPPORTED
		if(!mbLockDuringHookCalls)
			PPMMutexUnlock(mpMutex);
	#endif

	#if PPM_HOOKS_SUPPORTED
		if(mpHookFunction)
		{
			hookInfo.mbEntry       = false;
			hookInfo.mpDataOutput  = pReturnValue;
			hookInfo.mnSizeOutput  = GetUsableSize(pReturnValue); // or GetBlockSize(pReturnValue, true);

			mpHookFunction(&hookInfo, mpHookFunctionContext);
		}
	#endif

	#if PPM_THREAD_SAFETY_SUPPORTED
		if(mbLockDuringHookCalls)
			PPMMutexUnlock(mpMutex);
	#endif

	return pReturnValue;
}


void* GeneralAllocatorDebug::CallocDebug(size_t nElementCount, size_t nElementSize, int nAllocationFlags,
										 unsigned nDebugDataFlags, const char* pName, const char* pFile, int nLine)
{
	// We leave the mutex locked for this call, though ideally we would respect mbLockDuringHookCalls.
	// As it stands now we have no choice about locking here and must lock for 
	// the duration of the operation. This probably isn't an issue but under 
	// some circumstances can be a potential issue when hooks are installed.
	#if PPM_THREAD_SAFETY_SUPPORTED
		PPMAutoMutex autoMutex(mpMutex);
	#endif

	mnNextDebugDataFlags = nDebugDataFlags;
	mpNextAllocationName = pName;
	mpNextAllocationFile = pFile;
	mnNextAllocationLine = nLine;

	void* const pReturnValue = Calloc(nElementCount, nElementSize, nAllocationFlags);

	mnNextDebugDataFlags = 0;
	mpNextAllocationName = NULL;
	mpNextAllocationFile = NULL;
	mnNextAllocationLine = 0;

	return pReturnValue;
}




///////////////////////////////////////////////////////////////////////////////
// Realloc
//
// See the GeneralAllocator::Realloc function for a description of exactly 
// how Realloc is expected to behave.
//
// Our current behaviour is that we do not delay-free blocks that are freed
// by this Realloc function. 
//
// What we need to do here is a little tricky, due to the complex specification
// for how the C realloc function works. It's also tough for us here because 
// we want to preserve debug data attached to the original allocation.
//
void* GeneralAllocatorDebug::Realloc(void* pData, size_t nNewUserSize, int nAllocationFlags)
{
	void*         pReturnValue;
	size_t        nNewSize;
	const Chunk*  pChunk;
	size_t        nDebugDataSize;
	DebugDataInfo debugDataInfo;
	char          pDebugDataSavedStack[1024]; // It's fastest to use stack memory. Only use heap memory if debug data is too large for the stack.
	void*         pDebugDataSavedHeap = NULL;
	void*         pDebugDataSaved;

	#if PPM_THREAD_SAFETY_SUPPORTED
		PPMAutoMutex autoMutex(mpMutex);
	#endif

	AtomicUint nEventId = PPMAtomicIncrement(mEventId);

	if(pData)
	{
		void* pDebugDataSource;

		pChunk         = GetChunkPtrFromDataPtr(pData);
		nDebugDataSize = GetDebugDataLength(pData, kDebugDataLocationBlock, &pDebugDataSource);

		if(nDebugDataSize > (sizeof(pDebugDataSavedStack) - 32)) // Give some leeway.
		{
			TryAllocation1:
			{
				pDebugDataSaved = pDebugDataSavedHeap = MallocInternal(nDebugDataSize, kAllocationFlagInternal); 
				if(pDebugDataSaved)
					SetChunkIsInternal(GetChunkPtrFromDataPtr(pDebugDataSaved));
				else
				{
					// Else the malloc failed. If we have any delayed free blocks, we can free them and try again.
					if(mDelayedFreeChunkListHead.mpNextChunk != &mDelayedFreeChunkListHead) // If there is anything to free...
					{
						ClearDelayedFreeList(kDelayedFreePolicyNone, 0);
						goto TryAllocation1; // Jump back above and try again.
					}

					return NULL; // There isn't an elegant way out here without simply returning NULL right here.
				}
			}
		}
		else
			pDebugDataSaved = pDebugDataSavedStack;

		//if(pDebugDataSaved) // This 'if' is logically unnecessary, as the above logic makes it impossible to be NULL.
			memmove(pDebugDataSaved, pDebugDataSource, nDebugDataSize);

		#if PPM_REALLOC_C99_ENABLED // If C99 behaviour...
			nNewSize     = nNewUserSize + nDebugDataSize;
		#else // else C89 behaviour
			if(nNewUserSize > 0)
				nNewSize = nNewUserSize + nDebugDataSize;
			else
				nNewSize = 0; // For C89, a new size of zero means to free pData and return NULL.
		#endif

		PPM_ASSERT(this, VerifyFromDebugDataMap(pData), kGAErrorBadUserPtr, kGAErrorText[kGAErrorBadUserPtr], NULL, NULL); // If this fails, it is probably the user's fault.
	}
	else
	{
		// In this case the user is reallocating NULL to a given size. We act the same as 
		// if this was a call to Malloc. We don't simply call Malloc because that would 
		// make our tracking and hook calling behaviour inaccurate.
		pChunk          = NULL;
		nDebugDataSize  = 0;
		pDebugDataSaved = NULL;

		#if PPM_REALLOC_C99_ENABLED // If C99 behaviour...
			GetDebugInfoForAllocation(debugDataInfo, nNewUserSize, nAllocationFlags, kMinEffectiveAlignment);
			nNewSize = nNewUserSize + debugDataInfo.mnBlockUsage;
		#else
			if(nNewUserSize > 0) // For C89, a new size of zero means to free pData and return NULL.
			{
				GetDebugInfoForAllocation(debugDataInfo, nNewUserSize, nAllocationFlags, kMinEffectiveAlignment);
				nNewSize = nNewUserSize + debugDataInfo.mnBlockUsage;
			}
			else
				nNewSize = 0;
		#endif
	}

	#if PPM_HOOKS_SUPPORTED
		HookInfo hookInfo;

		if(mpHookFunction)
		{
			// For consistency with other malloc functions, we unlock across the hook function.
			// If an exception happens during processing of the hook function, then we would 
			// want to make sure we re-lock the mutex, but we'll have to leave that as a todo.
			#if PPM_THREAD_SAFETY_SUPPORTED
				if(!mbLockDuringHookCalls)
					PPMMutexUnlock(mpMutex);
			#endif

			hookInfo.mpGeneralAllocator = this; 
			hookInfo.mbEntry            = true;
			hookInfo.mHookType          = kHookTypeMalloc;
			hookInfo.mHookSubType       = kHookSubTypeRealloc;
			hookInfo.mnSizeInputTotal   = nNewUserSize;
			hookInfo.mpDataInput        = pData;
			hookInfo.mnCountInput       = 0;
			hookInfo.mnSizeInput        = nNewUserSize;
			hookInfo.mpSizeInputArray   = NULL;
			hookInfo.mnAlignmentInput   = 0;
			hookInfo.mnAllocationFlags  = nAllocationFlags;
			hookInfo.mpDataOutput       = NULL;
			hookInfo.mpArrayOutput      = NULL;
			hookInfo.mnSizeOutput       = 0;
			hookInfo.mEventId           = nEventId;

			mpHookFunction(&hookInfo, mpHookFunctionContext);

			#if PPM_THREAD_SAFETY_SUPPORTED
				if(!mbLockDuringHookCalls)
					PPMMutexLock(mpMutex);
			#endif
		}
	#else
		EA_UNUSED(nEventId);
	#endif

	// Collect stats after the entry hook has been called.
	CollectMetrics(pChunk, false); // We simply act as if we are freeing then allocating in turn. For consistency, we call CollectMetrics -after- we call the entry hook function.

	TryAllocation2:
	{
		pReturnValue = ReallocInternal(pData, nNewSize, nAllocationFlags);

		if(pReturnValue) // A pReturnValue of NULL may or may not signal a failed operation. It depends on the input.
		{
			if(pData) // If the user passed in a valid pointer... Recall that Reallocing a NULL pointer is valid and is the same as calling malloc.
			{
				Chunk* const    pChunkNew           = GetChunkPtrFromDataPtr(pReturnValue);
				const size_type nUsableChunkSizeNew = GetUsableChunkSize(pChunkNew);

				// As of this writing (2/2014), pDebugDataSaved is logically guaranteed to be valid (non-NULL), but it might be unsafe 
				// for us to simplify the following code to assume this, as future maintentance might change this current guarantee.
				if(pDebugDataSaved)
					memcpy((char*)pChunkNew + nUsableChunkSizeNew - nDebugDataSize, pDebugDataSaved, nDebugDataSize);
				else // Else at least clear the debug information.
					memset((char*)pChunkNew + nUsableChunkSizeNew - sizeof(DebugDataLengthType), 0, sizeof(DebugDataLengthType));

				// Update the valid pointer information.
				if(mbHashTableEnabled)
				{
					char** ppDebugBlock = NULL;
					const bool bResult = mHashTable.Find(pData, &ppDebugBlock);
					PPM_ASSERT(this, bResult, kGAErrorBadUserPtr, kGAErrorText[kGAErrorBadUserPtr], NULL, NULL); // If this fails, it is probably the user's fault.

					if(bResult)
					{
						// It's possible that pReturnValue is equal to pData. In this case we 
						// have no need to add a new pointer to the map and remove the old one. 
						if(pReturnValue != pData)
						{
							char* const pDebugBlock = *ppDebugBlock; // Save the value that ppDebugBlock stores.
							RemoveFromDebugDataMap(pData, false);     // 'false' here means to not free the debug block.
							if(!AddToDebugDataMap(pReturnValue, 0, pDebugBlock, false))
							{
								FreeInternal(pReturnValue);
								pReturnValue = NULL;
							}
						} // Otherwise the pointer is already in the map and so we need do nothing.
					}
				}

				// Update kDebugDataIdSize.
				if(pReturnValue)
				{
					void* const pDebugData = GetDebugData(pReturnValue, kDebugDataIdSize, NULL, 0); // Get the pointer to the debug data.
					if(pDebugData)
					{
						PPM_CT_ASSERT(sizeof(debugDataInfo.mnUserSize) == sizeof(nNewUserSize)); // If this fails, PPMalloc is at fault.
						memcpy(pDebugData, &nNewUserSize, sizeof(nNewUserSize));
					}

					// Collect statistics.
					CollectMetrics(pChunkNew, true);
				}
			}
			else // Else the user called this function with pData as NULL and thus requesting malloc-like behaviour.
			{
				// In this case we do the same thing as with Malloc.
				if(SetDebugInfoForAllocation(debugDataInfo, pReturnValue, true)) // This will in turn call AddToDebugDataMap.
					CollectMetrics(GetChunkPtrFromDataPtr(pReturnValue), true);
				else
				{
					FreeInternal(pReturnValue);
					pReturnValue = NULL;
				}
			}
		}
		else // Else the return value was NULL.
		{
			// If we have any delayed free blocks, we can free them and try again.
			if(mDelayedFreeChunkListHead.mpNextChunk != &mDelayedFreeChunkListHead) // If there is anything to free...
			{
				ClearDelayedFreeList(kDelayedFreePolicyNone, 0);
				goto TryAllocation2; // Jump back above and try again.
			}

			if(pData)
				RemoveFromDebugDataMap(pData, true);
			// Else the input was NULL and the output was NULL. This is due to either the 
			// allocation of a non-zero sized block failing or due to a C89-style request 
			// to free the input pData via requesting a new size of zero.
		}
	}

	if(pDebugDataSavedHeap)
		FreeInternal(pDebugDataSavedHeap);

	#if PPM_HOOKS_SUPPORTED
		if(mpHookFunction)
		{
			#if PPM_THREAD_SAFETY_SUPPORTED
				if(!mbLockDuringHookCalls)
					PPMMutexUnlock(mpMutex);
			#endif

			hookInfo.mbEntry       = false;
			hookInfo.mpDataOutput  = pReturnValue;
			hookInfo.mnSizeOutput  = GetUsableSize(pReturnValue); // or GetBlockSize(pReturnValue, true);

			mpHookFunction(&hookInfo, mpHookFunctionContext);

			#if PPM_THREAD_SAFETY_SUPPORTED
				if(!mbLockDuringHookCalls)
					PPMMutexLock(mpMutex);
			#endif
		}
	#endif

	return pReturnValue;
}



///////////////////////////////////////////////////////////////////////////////
// Free
//
void GeneralAllocatorDebug::Free(void* pData)
{
	AtomicUint nEventId = PPMAtomicIncrement(mEventId);

	#if PPM_NULL_POINTER_FREE_ENABLED
	if(pData){
	#endif
		#if PPM_THREAD_SAFETY_SUPPORTED
			if(mbLockDuringHookCalls)
				PPMMutexLock(mpMutex);
		#endif

		#if PPM_HOOKS_SUPPORTED
			HookInfo hookInfo;

			if(mpHookFunction)
			{
				hookInfo.mpGeneralAllocator = this; 
				hookInfo.mbEntry            = true;
				hookInfo.mHookType          = kHookTypeFree;
				hookInfo.mHookSubType       = kHookSubTypeNone;
				hookInfo.mnSizeInputTotal   = 0;
				hookInfo.mpDataInput        = pData;
				hookInfo.mnCountInput       = 0;
				hookInfo.mnSizeInput        = 0;
				hookInfo.mpSizeInputArray   = NULL;
				hookInfo.mnAlignmentInput   = 0;
				hookInfo.mnAllocationFlags  = 0;
				hookInfo.mpDataOutput       = NULL;
				hookInfo.mpArrayOutput      = NULL;
				hookInfo.mnSizeOutput       = 0;
				hookInfo.mEventId           = nEventId;

				mpHookFunction(&hookInfo, mpHookFunctionContext);
			}
		#else
			EA_UNUSED(nEventId);
		#endif

		#if PPM_THREAD_SAFETY_SUPPORTED
			if(!mbLockDuringHookCalls)
				PPMMutexLock(mpMutex);
		#endif

		const bool bPointerIsValid = VerifyFromDebugDataMap(pData);
		PPM_ASSERT(this, bPointerIsValid, kGAErrorBadUserPtr, kGAErrorText[kGAErrorBadUserPtr], NULL, NULL); // If this fails, it is the user's fault. The pointer is invalid, possibly due to being double-freed or possibly due to the pointer belonging to some other heap.

		if(bPointerIsValid)
		{
			ClearDelayedFreeList(mDelayedFreePolicy, mDelayedFreePolicyValue);

			Chunk* const pChunk = GetChunkPtrFromDataPtr(pData);
			VerifyGuardFill(pChunk, false, false); // Verify that guard values are as expected.

			if(mDelayedFreePolicy == kDelayedFreePolicyNone) 
			{
				CollectMetrics(pChunk, false);
				FreeInternal(pData);
			}
			else // else we do delayed freeing of the block.
			{ 
				const size_t nUsableSize = GetUsableSize(pData); // User-level usable size (i.e. not including our debug size; total size minus our debug size).

				// To add a chunk to the delayed free list, the chunk needs to 
				// have at least enough space to store the list pointers. 
				// But we also have our debug information block size uint16 
				// storage at the end of the block. The result is that we 
				// skip delayed free of blocks that don't have at least enough
				// space to store the list and debug information.
				if(nUsableSize < kChunkListPointersSize) // If not enough room...then don't do a delayed free.
				{
					CollectMetrics(pChunk, false);
					FreeInternal(pData);
				}
				else
				{
					// Write mnFillDelayedFree into user area after the list pointers (mpPrevChunk/mpNextChunk).
					memset((char*)pData + kChunkListPointersSize, mnFillDelayedFree, nUsableSize - kChunkListPointersSize);

					// Possibly add a free timestamp.
					if(mDelayedFreePolicy == kDelayedFreePolicyTime) // If our delayed free is time-based...
					{
						const unsigned nFreeTime = GetPPMallocDebugTime();
						memcpy(pData, &nFreeTime, sizeof(nFreeTime));  // Possibly we should add this as a debug record instead. A problem with doing so is that it would require another two bytes and remove some (small) blocks as candidates for time delayed frees due to lack of space.
					}

					// Add to the back of the chunk list.
					LinkChunk(pChunk, &mDelayedFreeChunkListHead, mDelayedFreeChunkListHead.mpNextChunk);
					mDelayedFreeListSize++;
					mDelayedFreeVolume += GeneralAllocator::GetUsableSize(pData); // Note that GeneralAllocator::GetUsableSize() != GeneralAllocatorDebug::GetUsableSize().
				}
			}

			RemoveFromDebugDataMap(pData, true);
		}

		#if PPM_THREAD_SAFETY_SUPPORTED
			if(!mbLockDuringHookCalls)
				PPMMutexUnlock(mpMutex);
		#endif

		#if PPM_HOOKS_SUPPORTED
			if(mpHookFunction)
			{
				hookInfo.mbEntry = false;
				mpHookFunction(&hookInfo, mpHookFunctionContext);
			}
		#endif

		#if PPM_THREAD_SAFETY_SUPPORTED
			if(mbLockDuringHookCalls)
				PPMMutexUnlock(mpMutex);
		#endif

	#if PPM_NULL_POINTER_FREE_ENABLED
	} // if(pData)
	#endif
}



///////////////////////////////////////////////////////////////////////////////
// MallocInternal
//
// Used to trap internal allocations so we can attach empty debug data to them.
// As of this writing, we should only need to trap calls to MallocInternal
// from GeneralAllocator internally and not those from the user through us.
//
void* GeneralAllocatorDebug::MallocInternal(size_t nSize, int nAllocationFlags)
{
	const int bAttachDebugData = (nAllocationFlags & kAllocationFlagInternal); // True if this function is being called internally and not by the user.

	if(bAttachDebugData) 
		nSize += sizeof(DebugDataLengthType);

	void* const pReturnValue = GeneralAllocator::MallocInternal(nSize, nAllocationFlags);

	if(pReturnValue) // Regardless of the value of bAttachDebugData, we always write in a zero debug length. Every block ever allocated must have a valid size here.
	{
		// Here we simply write a debug data length of zero. This way any debug 
		// data queries will result in not finding anything instead of finding
		// random debug data and likely crashing the app.
		const Chunk* const pChunk           = GetChunkPtrFromDataPtr(pReturnValue);
		const size_type    nUsableChunkSize = GetUsableChunkSize(pChunk);
		char* const        pPosition        = (char*)pChunk + nUsableChunkSize - sizeof(DebugDataLengthType);
		memset(pPosition, 0, sizeof(DebugDataLengthType));
	}

	return pReturnValue;
}


///////////////////////////////////////////////////////////////////////////////
// CallMallocFailureFunction
//
bool GeneralAllocatorDebug::CallMallocFailureFunction(GeneralAllocator* pGeneralAllocator, size_t nMallocRequestedSize, size_t nAllocatorRequestedSize, void* pContext)
{
	// Save some member variables, as the call below occurs with our mutex unlocked. See the GeneralAllocator::MallocInternal function
	// to see it unlock the mutex before calling this. 
	unsigned    nNextDebugDataFlags = mnNextDebugDataFlags;
	const char* pNextAllocationName = mpNextAllocationName;
	const char* pNextAllocationFile = mpNextAllocationFile;
	int         nNextAllocationLine = mnNextAllocationLine;

	bool bResult = GeneralAllocator::CallMallocFailureFunction(pGeneralAllocator, nMallocRequestedSize, nAllocatorRequestedSize, pContext);

	// Restore the variables.
	mnNextDebugDataFlags = nNextDebugDataFlags;
	mpNextAllocationName = pNextAllocationName;
	mpNextAllocationFile = pNextAllocationFile;
	mnNextAllocationLine = nNextAllocationLine;

	return bResult;
}



///////////////////////////////////////////////////////////////////////////////
// GetUsableSize
//
size_t GeneralAllocatorDebug::GetUsableSize(const void* pData) const
{
	// No mutex lock here because the data is considered to be owned by the calling thread and won't change.

	if(pData)
	{
		const Chunk* const pChunk = GetChunkPtrFromDataPtr(pData);

		// We check to make sure the chunk isn't in the fast bins because if it was 
		// in the fast bins, then that means it was freed by the user. And if it was 
		// free by the user, then our debug data at the end of the block would be 
		// obliterated. In that case we simply use the GeneralAllocator::GetUsableSize
		// function, as the freed block's usable size is the entire size since we 
		// no longer own it. 
		if(!GetChunkIsFastBin(pChunk))
		{
			const size_type     nUsableChunkSize = GetUsableChunkSize(pChunk);
			const char* const   pPosition        = (const char*)pChunk + nUsableChunkSize - sizeof(DebugDataLengthType);
			DebugDataLengthType nDebugDataSize; // Refers to size of all debug data not including the field holding this size value.
			memcpy(&nDebugDataSize, pPosition, sizeof(DebugDataLengthType));

			const char* const pPositionEnd = pPosition - nDebugDataSize;
			PPM_ASSERT(this, pPositionEnd >= pData, kGAErrorCorruptedHeap, kGAErrorText[kGAErrorCorruptedHeap], pChunk, pData); // If this fails, the user probably corrupted the heap.

			if(pPositionEnd >= pData)
				return (size_t)(pPositionEnd - (const char*)pData);
		}
	}

	return GeneralAllocator::GetUsableSize(pData);
}


///////////////////////////////////////////////////////////////////////////////
// ValidateChunk
//
bool GeneralAllocatorDebug::ValidateChunk(const void* pChunkVoid, HeapValidationLevel heapValidationLevel)
{
	#if PPM_THREAD_SAFETY_SUPPORTED
		PPMAutoMutex autoMutex(mpMutex);
	#endif

	const Chunk* const pChunk = (const Chunk*)pChunkVoid;

	bool bReturnValue = GeneralAllocator::ValidateChunk(pChunk, heapValidationLevel);

	if(bReturnValue)
	{
		// Check to see if the Chunk is in our DebugDataMap.
		// If the chunk is internal or in a fastbin then we don't check because 
		// Such 'used' chunks are not in the DebugDataMap. Also, delayed free 
		// chunks are also not in the DebugDataMap and so we ignore them as well.
		// But we do the test for is-delayed-free after checking the DebugDataMap
		// because checking the delayed free list is significantly more expensive
		// than the previous checks.
		if(GetChunkIsInUse(pChunk) && !GetChunkIsInternal(pChunk) && !GetChunkIsFastBin(pChunk)) // Fast bin chunks have the prev chunk in use flag set, as if they were allocated.
		{
			const void* const pData           = GetDataPtrFromChunkPtr(pChunk);
			const bool        bInDebugDataMap = VerifyFromDebugDataMap(pData); // This returns true if pData is a live user allocation, or true if DebugDataMap is disabled.
			const bool        bDelayedFree    = AllocationIsDelayedFree(pData);

			if(bInDebugDataMap) // If the allocation is a live user allocation (as expected because GetChunkIsInUse above was true)...
			{
				if(!VerifyGuardFill(pChunk, bDelayedFree, true))
					bReturnValue = false;
			}
			else
				bReturnValue = bDelayedFree; // If the allocation wasn't in the DebugDataMap, then it better be in the DelayedFree list or else something is wrong.
		}
	}

	return bReturnValue;
}


///////////////////////////////////////////////////////////////////////////////
// ValidateHeap
//
bool GeneralAllocatorDebug::ValidateHeap(HeapValidationLevel heapValidationLevel)
{
	bool bReturnValue(true);

	if(!mbHeapValidationActive) // If not already checking the heap... (recursion prevention)
	{
		if(!GeneralAllocator::ValidateHeap(heapValidationLevel))
			bReturnValue = false;

		if(bReturnValue)
		{
			mbHeapValidationActive = true; // Set this so that calls below don't trigger recursive validation attempts.

			// We use an auto-mutex so that any exceptions might occur while processing 
			// will trigger the mutex unlock via stack unwinding.
			#if PPM_THREAD_SAFETY_SUPPORTED
				PPMAutoMutex autoMutex(mpMutex);
			#endif

			// Validate the delayed free chunk list.
			for(const Chunk* pChunk = mDelayedFreeChunkListHead.mpNextChunk; 
				 pChunk != &mDelayedFreeChunkListHead;
				 pChunk = pChunk->mpNextChunk)
			{
				CheckDelayedFreeChunk(pChunk);
			}

			// The following is disabled, as it has undesirable effects. In particular, a primary reason
			// you would want to validate the heap is to diagnose some problem. However, by clearing
			// the delayed free list here, we are modifying the heap and this changing the runtime
			// behaviour of the system. Ideally, heap validation would not change the heap at all.
			// Disabled:
			//     ClearDelayedFreeList(kDelayedFreePolicyNone, 0); // This call can potentially generate further attempts at heap validation.

			// Check guard fills of allocated memory.
			if(heapValidationLevel > kHeapValidationLevelBasic)
			{
				SnapshotImage storage;

				const void* const pContext = ReportBegin(NULL, kBlockTypeAllocated, false, &storage, sizeof(storage));

				for(const BlockInfo* pBlockInfo = ReportNext(pContext, kBlockTypeAllocated); pBlockInfo; pBlockInfo = ReportNext(pContext, kBlockTypeAllocated))
				{
					const Chunk* const pChunkTemp = GetChunkPtrFromDataPtr(pBlockInfo->mpData);

					if(!VerifyFromDebugDataMap(pBlockInfo->mpData))
						bReturnValue = false;

					if(!VerifyGuardFill(pChunkTemp, false, true))
						bReturnValue = false;
				}
				ReportEnd(pContext);
			}

			// Verify that all pointers in the pointer hash table are valid
			// Here we iterate the hash table of valid pointers and make sure 
			// they are all present and sensible in the actual heap space. 
			// Consider that a user could completely nuke a chunk header and 
			// with appropriate manipulation of the previous chunk header make 
			// the chunk 'disappear' from the heap.
			if(mbHashTableEnabled && (mpAllocator == this))
			{
				for(size_t b = 0; b < mHashTable.mnHashTableSize; b++) // For each bucket...
				{
					for(HashTable::HashPair* pEntry = mHashTable.mppHashTable[b]; pEntry; pEntry = pEntry->mpNext) // For each entry in the current bucket...
					{
						Chunk* const pChunk = GetChunkPtrFromDataPtr(pEntry->mpValue);
						CheckUsedChunk(pChunk);
					}
				}
			}

			mbHeapValidationActive = false;
		}
	}

	return bReturnValue;
}



///////////////////////////////////////////////////////////////////////////////
// TraceAllocatedMemory
//
void GeneralAllocatorDebug::TraceAllocatedMemory(TraceFunction pTraceFunction, void* pTraceFunctionContext, 
												 void* pStorage, size_t nStorageSize, int nBlockTypeFlags)
{
	// No mutex lock here because the calls below take care of it.
	ClearDelayedFreeList(kDelayedFreePolicyNone, 0);
	GeneralAllocator::TraceAllocatedMemory(pTraceFunction, pTraceFunctionContext, pStorage, nStorageSize, nBlockTypeFlags);
}



///////////////////////////////////////////////////////////////////////////////
// TakeSnapshot
//
void* GeneralAllocatorDebug::TakeSnapshot(int nBlockTypeFlags, bool bMakeCopy, void* pStorage, size_t nStorageSize)
{
	// No mutex lock here because the calls below take care of it.
	ClearDelayedFreeList(kDelayedFreePolicyNone, 0);
	return GeneralAllocator::TakeSnapshot(nBlockTypeFlags, bMakeCopy, pStorage, nStorageSize);
}



///////////////////////////////////////////////////////////////////////////////
// FreeSnapshot
//
void GeneralAllocatorDebug::FreeSnapshot(void* pSnapshot)
{
	// No mutex lock here because the calls below take care of it.
	// ClearDelayedFreeList(kDelayedFreePolicyNone, 0); // This call shouldn't be necessary.
	return GeneralAllocator::FreeSnapshot(pSnapshot);
}



///////////////////////////////////////////////////////////////////////////////
// SetDelayedFreePolicy
//
void GeneralAllocatorDebug::SetDelayedFreePolicy(DelayedFreePolicy policy, int value)
{
	#if PPM_THREAD_SAFETY_SUPPORTED
		PPMAutoMutex autoMutex(mpMutex);
	#endif

	if(mDelayedFreePolicy != policy)
	{
		ClearDelayedFreeList(kDelayedFreePolicyNone, 0);
		mDelayedFreePolicy      = policy;
		mDelayedFreePolicyValue = value;
	}
}


///////////////////////////////////////////////////////////////////////////////
// AddToDebugDataMap
//
// Returns true if the data (pData/nDebugBlockSize) is already in the debug 
// data map (i.e. hash table) or could be added to the map. If the map is 
// disabled then this function is a no-op and returns true. The argument
// bAllowDuplicate is present to help us debug and has no other functionality.
//
#if PPM_ASSERT_ENABLED
	bool GeneralAllocatorDebug::AddToDebugDataMap(const void* pData, size_t nDebugBlockSize, char* pDebugBlock, bool bAllowDuplicate)
#else
	bool GeneralAllocatorDebug::AddToDebugDataMap(const void* pData, size_t nDebugBlockSize, char* pDebugBlock, bool)
#endif
{
	// No mutex lock here because this is an internal function.
	#if PPM_THREAD_SAFETY_SUPPORTED
	PPM_ASSERT(this, !mpMutex || PPMMutexGetLockCount(mpMutex) > 0, kGAErrorInternal, kGAErrorText[kGAErrorInternal], NULL, NULL); // Verify that the mutex is in fact locked. // If this fails, PPMalloc is at fault.
	#endif

	bool bReturnValue = true;

	if(mbHashTableEnabled)
	{
		const bool bResult = mHashTable.Find(pData, NULL);
		PPM_ASSERT(this, bAllowDuplicate || !bResult, kGAErrorInternal, kGAErrorText[kGAErrorInternal], NULL, NULL); // If this fails, PPMalloc is probably at fault. However, it has been seen that the user can cause this by freeing blocks with the wrong allocator. Try enabling kOptionEnablePtrValidation to auto-detect this case.

		if(!bResult)
		{
			bReturnValue = false; // False for now. The code below will have to succeed for this to be considered successful.

			bool bDebugBlockAllocated = false;

			PPM_ASSERT(this, pDebugBlock || (nDebugBlockSize >= sizeof(DebugDataLengthType)), kGAErrorInternal, kGAErrorText[kGAErrorInternal], NULL, NULL); // If this fails, PPMalloc is probably at fault.
			if(!pDebugBlock && nDebugBlockSize)
			{
				pDebugBlock = (char*)AllocatorMalloc(nDebugBlockSize);
				if(pDebugBlock)
				{
					*((DebugDataLengthType*)pDebugBlock) = (DebugDataLengthType)nDebugBlockSize;
					memset(pDebugBlock + nDebugBlockSize - sizeof(DebugDataLengthType), 0, sizeof(DebugDataLengthType)); // Set the block size (which is at the end of the data) to zero.
					bDebugBlockAllocated = true;
				}
			}

			if(pDebugBlock)
			{
				if(mHashTable.Insert(pData, pDebugBlock))
					bReturnValue = true;
				else if(bDebugBlockAllocated)
					AllocatorFree(pDebugBlock);
			} // Else we failed to allocate the space above.
		} // Else the pointer is already in the table, so we don't need to add it.
	} // Else the hash table is intentionally disabled, so this function is not considered to have failed.

	return bReturnValue;
}



///////////////////////////////////////////////////////////////////////////////
// RemoveFromDebugDataMap
//
bool GeneralAllocatorDebug::RemoveFromDebugDataMap(const void* pData, bool bFreeDebugData)
{
	// No mutex lock here because this is an internal function.
	#if PPM_THREAD_SAFETY_SUPPORTED
	PPM_ASSERT(this, !mpMutex || PPMMutexGetLockCount(mpMutex) > 0, kGAErrorInternal, kGAErrorText[kGAErrorInternal], NULL, NULL); // Verify that the mutex is in fact locked. // If this fails, PPMalloc is at fault.
	#endif

	bool bReturnValue;

	if(mbHashTableEnabled)
	{
		bReturnValue = mHashTable.Find(pData, NULL);
		PPM_ASSERT(this, bReturnValue, kGAErrorChunkOutsideHeap, kGAErrorText[kGAErrorChunkOutsideHeap], NULL, pData); // If this fails, the user probably corrupted the heap. There may be a bug in PPMalloc, however.

		if(bReturnValue)
			mHashTable.Remove(pData, bFreeDebugData);
	}
	else
		bReturnValue = false;

	return bReturnValue;
}


///////////////////////////////////////////////////////////////////////////////
// VerifyFromDebugDataMap
//
bool GeneralAllocatorDebug::VerifyFromDebugDataMap(const void* pData) const
{
	// No mutex lock here because this is an internal function.
	#if PPM_THREAD_SAFETY_SUPPORTED
	PPM_ASSERT(this, !mpMutex || PPMMutexGetLockCount(mpMutex) > 0, kGAErrorInternal, kGAErrorText[kGAErrorInternal], NULL, NULL); // Verify that the mutex is in fact locked. // If this fails, PPMalloc is at fault.
	#endif

	return !mbHashTableEnabled || mnPtrValidationDisabled || mHashTable.Find(pData, NULL);
}


///////////////////////////////////////////////////////////////////////////////
// ValidateAddress
//
const void* GeneralAllocatorDebug::ValidateAddress(const void* pData, int addressType) const
{
	#if PPM_THREAD_SAFETY_SUPPORTED
		PPMAutoMutex autoMutex(mpMutex);
	#endif

	const void* pReturnValue = NULL;

	if((addressType == kAddressTypeSpecific) && mbHashTableEnabled)
	{
		if(VerifyFromDebugDataMap(pData))
			pReturnValue = pData;
	}
	else
	{
		pReturnValue = GeneralAllocator::ValidateAddress(pData, addressType);

		// We need to deal with the case whereby the pointer was determined to be valid
		// but actually it is a delayed free pointer. We don't want to clear the delayed
		// free list, as that would modify the heap. So we check to see if pReturnValue
		// is in the delayed free list. If the caller specified kAddressTypeOwned then 
		// it doesn't matter if the Chunk is in a free list, as Owned merely means that
		// the chunk comes from one of our core blocks. The one exception is mapped memory,
		// which is not part of our core blocks and so freed mapped memory that is in a 
		// delayed free list is not considered Owned for the purposes of this function.
		if(pReturnValue)
		{
			const Chunk* const pTestChunk = GetChunkPtrFromDataPtr(pData);

			if((addressType != kAddressTypeOwned) || GetChunkIsMMapped(pTestChunk))
			{
				// In this case, addressType == kAddressTypeSpecific or kAddressTypeAllocated.
				// We don't have much choice but to walk the delayed free list. We can't check
				// our hash table because we would have done that above if possible.

				for(const Chunk* pChunk  = mDelayedFreeChunkListHead.mpNextChunk; 
								 pChunk != &mDelayedFreeChunkListHead;
								 pChunk  = pChunk->mpNextChunk)
				{
					if(pChunk == pTestChunk) // If indeed pData is in the delayed free list
					{
						pReturnValue = NULL;
						break;
					}
				}
			}
		}
	}

	#if (PPM_DEBUG >= 2) // This is usually going to evaluate as true, given that this is a 'debug' class.
		if(pReturnValue && ((addressType == kAddressTypeSpecific) || (addressType == kAddressTypeAllocated)))
		{
			const Chunk* const pChunk = GetChunkPtrFromDataPtr(pReturnValue);
			VerifyGuardFill(pChunk, false, true);
		}
	#endif

	return pReturnValue;
}


///////////////////////////////////////////////////////////////////////////////
// SetFillValues
//
void GeneralAllocatorDebug::SetFillValues(unsigned char cFree, unsigned char cDelayedFree, 
										  unsigned char cNew,  unsigned char cGuard,
										  unsigned char cUnusedCore)
{
	#if PPM_THREAD_SAFETY_SUPPORTED
		PPMAutoMutex autoMutex(mpMutex);
	#endif

	// We unilaterally change the following values, as we don't need to 
	// do anything if they have changed.
	mnFillNew          = cNew;
	mnFillUnusedCore = cUnusedCore;

	// Free memory fills
	if(mnFillFree != cFree) // If there is a free fill change...
	{
		mnFillFree = cFree;

		// We go through all free blocks and change their current 
		// fill from the previous value to the new value.
		CoreBlock* pCoreBlock = mHeadCoreBlock.mpNextCoreBlock;
		while(pCoreBlock != &mHeadCoreBlock) // For each core block...
		{
			const Chunk* pChunk      = (Chunk*)pCoreBlock->mpCore;
			const Chunk* pFenceChunk = GetFenceChunk(pCoreBlock);

			while(pChunk < pFenceChunk) // For each chunk in the core block...
			{
				if(!GetChunkIsInUse(pChunk)) // If this is a free chunk (and thus has a free fill).
				{
					void* const pFill = GetPostHeaderPtrFromChunkPtr(pChunk);
					const size_type nChunkSize = GetChunkSize(pChunk);
					memset(pFill, mnFillFree, nChunkSize - sizeof(Chunk));
				}
				pChunk = GetNextChunk(pChunk);
			}

			pCoreBlock = pCoreBlock->mpNextCoreBlock;
		}
	}

	// Delayed free fills
	if(mnFillDelayedFree != cDelayedFree) // If there is a delayed free fill change...
	{
		mnFillDelayedFree = cDelayedFree;

		if(mnFillDelayedFree == kDefaultFillValueDisabled)
			mDelayedFreePolicy = kDelayedFreePolicyNone;
		
		// Note that we still update the fill values even if mnFillDelayedFree is kDefaultFillValueDisabled.
		// This is done for consistency of expectations when examining debug data.
		for(const Chunk* pChunk = mDelayedFreeChunkListHead.mpNextChunk; 
			 pChunk != &mDelayedFreeChunkListHead;
			 pChunk = pChunk->mpNextChunk)
		{
			const unsigned char* pData = (const unsigned char*)GetDataPtrFromChunkPtr(pChunk);
			const size_t nUsableSize = GetUsableSize(pData); // User-level usable size (i.e. not including our debug size).

			// Write mnFillDelayedFree into user area after the list pointers (mpPrevChunk/mpNextChunk).
			memset((char*)pData + kChunkListPointersSize, mnFillDelayedFree, nUsableSize - kChunkListPointersSize);
		}
	}

	// Guard fills
	if(mnFillGuard != cGuard) // If there is a guard fill change...
	{
		mnFillGuard = cGuard;

		if(mnFillGuard == kDefaultFillValueDisabled)
			mnDebugDataFlags &= ~(1 << kDebugDataIdGuard);

		// Note that we still update the fill values even if mnFillGuard is kDefaultFillValueDisabled.
		// This is done for consistency of expectations when examining debug data.
		const void* const pContext = ReportBegin(NULL, kBlockTypeAllocated, false);

		for(const BlockInfo* pBlockInfo = ReportNext(pContext, kBlockTypeAllocated); pBlockInfo; pBlockInfo = ReportNext(pContext, kBlockTypeAllocated))
		{
			size_t             nActualDataLength;
			const Chunk* const pChunkTemp = (const Chunk*)pBlockInfo->mpBlock;
			const char*  const pData      = (const char*)GetDataPtrFromChunkPtr(pChunkTemp);
			char*              pDebugData = (char*)GetDebugData(pData, kDebugDataIdGuard, NULL, 0, &nActualDataLength, kDebugDataLocationBlock);

			if(pDebugData) // If a guard fill field was found...
			{
				const char* const pDebugDataEnd = pDebugData + nActualDataLength;

				if(pDebugData < (pData + kChunkListPointersSize))
					pDebugData += (size_t)((pData + kChunkListPointersSize) - pDebugData);

				memset(pDebugData, (int)mnFillGuard, (size_t)(pDebugDataEnd - pDebugData));
			}
		}
		ReportEnd(pContext);
	}

	#if (PPM_DEBUG >= 3) // Developer builds only.
		CheckState(kHeapValidationLevelDetail); // Verify that our code above worked OK.
	#endif
}



///////////////////////////////////////////////////////////////////////////////
// SetGuardSize
//
void GeneralAllocatorDebug::SetGuardSize(float fGuardSizeRatio, size_t nMinGuardSize, size_t nMaxGuardSize)
{
	#if PPM_THREAD_SAFETY_SUPPORTED
		PPMAutoMutex autoMutex(mpMutex);
	#endif

	if(nMaxGuardSize > kMaxDebugDataSize)
	   nMaxGuardSize = kMaxDebugDataSize;

	mfGuardSizeRatio = fGuardSizeRatio;
	mnMinGuardSize   = nMinGuardSize;
	mnMaxGuardSize   = nMaxGuardSize;

	if(mfGuardSizeRatio < 0.f)          // Sanity check.
	   mfGuardSizeRatio = 0.f;
	if(mfGuardSizeRatio > 8.f)          // Sanity check.
	   mfGuardSizeRatio = 8.f;
	if(mnMinGuardSize > nMaxGuardSize)  // if min > max, chop it.
	   mnMinGuardSize = nMaxGuardSize;

	if((mfGuardSizeRatio == 0) || (mnMaxGuardSize == 0))
		mnDebugDataFlags &= ~(1 << kDebugDataIdGuard);
}



///////////////////////////////////////////////////////////////////////////////
// GetGuardSizeForBlock
//
size_t GeneralAllocatorDebug::GetGuardSizeForBlock(size_t nBlockSize)
{
	#if PPM_THREAD_SAFETY_SUPPORTED
		PPMAutoMutex autoMutex(mpMutex);
	#endif

	size_t nGuardSize = 0;

	if(mnDebugDataFlags & (1 << kDebugDataIdGuard)) // If guards are enabled...
	{
		// Cast nBlockSize to float because otherwise the expression will be promoted to 
		// double, which is slow or not even implemented in hardware on some platforms.
		nGuardSize = (size_t)((float)nBlockSize * mfGuardSizeRatio);
		if(nGuardSize < mnMinGuardSize)
		   nGuardSize = mnMinGuardSize;
		if(nGuardSize > mnMaxGuardSize)
		   nGuardSize = mnMaxGuardSize;
	}

	return nGuardSize;
}


///////////////////////////////////////////////////////////////////////////////
// VerifyGuardFill
//
bool GeneralAllocatorDebug::VerifyGuardFill(const Chunk* pChunk, bool bChunkIsInAList, bool bForceVerify) const
{
	// No mutex lock here because this is an internal function.
	#if PPM_THREAD_SAFETY_SUPPORTED
	PPM_ASSERT(this, !mpMutex || PPMMutexGetLockCount(mpMutex) > 0, kGAErrorInternal, kGAErrorText[kGAErrorInternal], NULL, NULL); // Verify that the mutex is in fact locked. // If this fails, PPMalloc is at fault.
	#endif

	if(bForceVerify || (mnDebugDataFlags & (1 << kDebugDataIdGuard))) // We don't verify guard fills if the guard fill system is disabled. 
	{
		size_t nActualDataLength;
		const unsigned char* pData      = (const unsigned char*)GetDataPtrFromChunkPtr(pChunk);
		const unsigned char* pDebugData = (const unsigned char*)GetDebugData(pData, kDebugDataIdGuard, NULL, 0, &nActualDataLength, kDebugDataLocationBlock);

		if(pDebugData) // If a guard fill field was found...
		{
			#if (PPM_DEBUG >= 3)     // Developer builds only.
				const unsigned char* const pDebugDataEnd = pDebugData + nActualDataLength;
			#elif (PPM_DEBUG >= 2)  // Extended user debug builds.
				const unsigned char* const pDebugDataEnd = pDebugData + PPM_MIN(nActualDataLength, 1024); // Limit it to a maximum size. Otherwise, verification can take an unacceptably long time.
			#else
				const unsigned char* const pDebugDataEnd = pDebugData + PPM_MIN(nActualDataLength, 64);
			#endif

			// If the chunk is in a list (e.g. delayed free list), then it is entirely possible
			// that the mpNextChunk and mpPrevChunk pointers overlap with the guard fill space.
			// If so, then the list pointer usage will have intentionally overridden the guard
			// fill space. So here we check for that and just ignore overlapping guard bytes.
			if(bChunkIsInAList && (pDebugData < (pData + kChunkListPointersSize)))
				pDebugData += (size_t)((pData + kChunkListPointersSize) - pDebugData);

			if(!MemoryFillCheck(pDebugData, (size_t)(pDebugDataEnd - pDebugData), mnFillGuard))
			{
				DoFailureNotification(pChunk, "GeneralAllocatorDebug::VerifyGuardFill failure.");
				return false;
			}
		}

		// We don't force guard fills to exist, even if kDebugDataIdGuard is enabled. The reason for 
		// this is that it's possible that kDebugDataIdGuard was enabled after allocations have already
		// occurred. It's possible that data was corrupted in such a way that a guard fied originally
		// existed by was corrupted away. However, we'd likely catch that corruption via other means.
		//else
		//{
		//    DoFailureNotification(pChunk, "GeneralAllocatorDebug::VerifyGuardFill failure.");
		//    return false;
		//}
	}
	return true;
}


///////////////////////////////////////////////////////////////////////////////
// VerifyDelayedFreeFill
//
bool GeneralAllocatorDebug::VerifyDelayedFreeFill(const Chunk* pChunk) const
{
	// No mutex lock here because this is an internal function.
	#if PPM_THREAD_SAFETY_SUPPORTED
	PPM_ASSERT(this, !mpMutex || PPMMutexGetLockCount(mpMutex) > 0, kGAErrorInternal, kGAErrorText[kGAErrorInternal], NULL, NULL); // Verify that the mutex is in fact locked. // If this fails, PPMalloc is at fault.
	#endif

	// We verify that non-debug user bytes in the chunk are as expected.
	const unsigned char* pData = (const unsigned char*)GetDataPtrFromChunkPtr(pChunk);
	const size_t nUsableSize    = GetUsableSize(pData); // Get user-level usable size.

	if(nUsableSize > kChunkListPointersSize) // If there was enough room for any delayed free fill...
	{
		#if (PPM_DEBUG >= 3)     // Developer builds only.
			const unsigned char* const pDataEnd = pData + nUsableSize;
		#elif (PPM_DEBUG >= 2)  // Extended user debug builds.
			const unsigned char* const pDataEnd = pData + PPM_MIN(nUsableSize, 4096); // Limit it to a maximum size. Otherwise, verification can take an unacceptably long time.
		#else
			const unsigned char* const pDataEnd = pData + PPM_MIN(nUsableSize, 256);
		#endif

		pData += kChunkListPointersSize;
		if(mDelayedFreePolicy == kDelayedFreePolicyTime) // If the policy is a time-policy, then we have stored 
			pData += sizeof(mDelayedFreePolicyValue);      // time of free in the first few bytes of the block.

		if(!MemoryFillCheck(pData, (size_t)(pDataEnd - pData), mnFillDelayedFree))
		{
			DoFailureNotification(pChunk, "GeneralAllocatorDebug::VerifyDelayedFreeFill failure.");
			return false;
		}
	}
	return true;
}


///////////////////////////////////////////////////////////////////////////////
// DoFailureNotification
//
void GeneralAllocatorDebug::DoFailureNotification(const Chunk* pChunk, const char* pMessage) const
{
	// No mutex lock here because we don't access member data here and the functions we call are themselves thread-safe.

	char         messageString[768];
	const size_t nStringLength  = (sizeof(messageString) / sizeof(messageString[0])) - 1;
	const size_t nMessageLength = strlen(pMessage); 

	PPM_ASSERT(this, (nMessageLength + 1) < nStringLength, kGAErrorInternal, kGAErrorText[kGAErrorInternal], NULL, NULL); // If this fails, PPMalloc is probably at fault.
	if((nMessageLength + 1) < nStringLength) // '+1' for the '\n' we append to the end.
	{
		strcpy(messageString, pMessage);
		messageString[nMessageLength] = '\n';
		DescribeChunk(pChunk, messageString + (nMessageLength + 1), nStringLength - (nMessageLength + 1), true);
		PPM_DEBUG_FAILURE(this, pMessage, kGAErrorChunkFillError, messageString, pChunk, pChunk);
	}
}


///////////////////////////////////////////////////////////////////////////////
// CheckDelayedFreeChunk
//
int GeneralAllocatorDebug::CheckDelayedFreeChunk(const Chunk* pChunk)
{
	// No mutex lock here because this is an internal function.
	#if PPM_THREAD_SAFETY_SUPPORTED
	PPM_ASSERT(this, !mpMutex || PPMMutexGetLockCount(mpMutex) > 0, kGAErrorInternal, kGAErrorText[kGAErrorInternal], NULL, NULL); // Verify that the mutex is in fact locked. // If this fails, PPMalloc is at fault.
	#endif

	int nErrorCount = 0;

	if(!VerifyDelayedFreeFill(pChunk))
		nErrorCount++;

	// As it stands right now, we can't call CheckFreeChunk, as it expects things
	// to be a little different than we do. In particular, it uses a different fill and
	// expects the chunk to be marked as free.
	// nErrorCount += LOCAL_VERIFY(CheckFreeChunk(pChunk) != 0);

	return nErrorCount;
}


///////////////////////////////////////////////////////////////////////////////
// DescribeChunk
//
size_t GeneralAllocatorDebug::DescribeChunk(const Chunk* pChunk, char* pBuffer, size_t nBufferLength, bool bAppendLineEnd) const
{
	#if PPM_THREAD_SAFETY_SUPPORTED
		PPMAutoMutex autoMutex(mpMutex);
	#endif

	const void* const pData          = GetDataPtrFromChunkPtr(pChunk);
	const char* const pBufferEnd     = pBuffer + nBufferLength;
	char*             pBufferCurrent = pBuffer;

	// Call parent version of DescribeChunk. 
	const size_t nGALength = GeneralAllocator::DescribeChunk(pChunk, pBufferCurrent, nBufferLength, false);
	pBufferCurrent += nGALength;

	// Possibly remove a trailing record delimiter that the GeneralAllocator::DescribeChunk function would have appended.
	if((pBufferCurrent > pBuffer) && (pBufferCurrent[-1] == mcTraceRecordDelimiter))
		*--pBufferCurrent = 0;

	// Possibly get allocation flags.
	int nAllocationFlags;

	if(GetDebugData(pData, kDebugDataIdFlags, &nAllocationFlags, sizeof(nAllocationFlags)) && nAllocationFlags)
	{
		if((pBufferEnd - pBufferCurrent) >= 24) // If there is enough space to hold the worst case situation...
		{
			strcpy(pBufferCurrent, "flags:");
			pBufferCurrent += 6; // += strlen("flags:");

			if(nAllocationFlags & kAllocationFlagHigh)
			{
				strcpy(pBufferCurrent, " high");
				pBufferCurrent += 5; // += strlen(" high");
			}

			if(nAllocationFlags & kAllocationFlagEndFit)
			{
				strcpy(pBufferCurrent, " end-fit");
				pBufferCurrent += 8; // += strlen(" end-fit");
			}

			*pBufferCurrent++ = (char)mcTraceFieldDelimiter;
			*pBufferCurrent = 0;
		}
	}


	// Possibly get allocation name.
	char    messageString[512];
	size_t nFieldDataLength = sizeof(messageString) / sizeof(messageString[0]);

	if(GetDebugData(pData, kDebugDataIdName, messageString, nFieldDataLength, &nFieldDataLength) && nFieldDataLength)
	{
		if((size_t)(pBufferEnd - pBufferCurrent) >= (nFieldDataLength + 12)) // If there is enough space...
		{
			const size_t nLength = (size_t)PPM_Snprintf(pBufferCurrent, (size_t)(pBufferEnd - pBufferCurrent), "name: %s%c", messageString, mcTraceFieldDelimiter);
			PPM_ASSERT(this, nLength > 0, kGAErrorDebugMemoryExhaustion, kGAErrorText[kGAErrorDebugMemoryExhaustion], NULL, NULL); // If this fails, PPMalloc is at fault or the user didn't supply enough memory.
			pBufferCurrent += nLength;
		}
	}


	// Possibly get allocation place.
	FileLineInfo fileLineInfo;

	if(GetDebugData(pData, kDebugDataIdPlace, &fileLineInfo, sizeof(fileLineInfo)))
	{
		const size_t nFileNameLength = strlen(fileLineInfo.mpFile);

		if((size_t)(pBufferEnd - pBufferCurrent) >= (nFileNameLength + 22)) // If there is enough space...
		{
			const size_t nLength = (size_t)PPM_Snprintf(pBufferCurrent, (size_t)(pBufferEnd - pBufferCurrent), "loc: %s, %u%c", fileLineInfo.mpFile, (unsigned)fileLineInfo.mnLine, mcTraceFieldDelimiter);
			PPM_ASSERT(this, nLength > 0, kGAErrorDebugMemoryExhaustion, kGAErrorText[kGAErrorDebugMemoryExhaustion], NULL, NULL); // If this fails, PPMalloc is at fault or the user didn't supply enough memory.
			pBufferCurrent += nLength;
		}
	}


	// Possibly get allocation call stack.
	#if PPM_DEBUG_CALLSTACK_AVAILABLE
		void* pReturnAddressArray[24];

		if(GetDebugData(pData, kDebugDataIdCallStack, pReturnAddressArray, sizeof(pReturnAddressArray), &nFieldDataLength) && nFieldDataLength)
		{
			if((pBufferEnd - pBufferCurrent) > 32) // If there's enough room for at least "stack: 0x1234567812345678\t\0"
			{
				strcpy(pBufferCurrent, "stack: ");
				pBufferCurrent += 7; // += strlen("stack: ");

				const size_t nOutputCapacity = (size_t)(pBufferEnd - pBufferCurrent) - 4; // Leave some extra room for trailing field and record delimites.
				const size_t nCharsWritten    = DescribeCallStack((const void**)pReturnAddressArray, nFieldDataLength / sizeof(void*), pBufferCurrent, nOutputCapacity, mAddressRepLookup); //lint !e1773 (Attempt to cast away const (or volatile))
				pBufferCurrent += nCharsWritten;

				PPM_ASSERT(this, (pBufferEnd - pBufferCurrent) > 0, kGAErrorDebugMemoryExhaustion, kGAErrorText[kGAErrorDebugMemoryExhaustion], NULL, NULL); // If this fails, PPMalloc is at fault or the user didn't supply enough memory.
				*pBufferCurrent++ = (char)mcTraceFieldDelimiter;
				*pBufferCurrent    = 0;
			}
		}
	#endif

	if(bAppendLineEnd)
	{
		// Append a trailing record delimiter.
		PPM_ASSERT(this, (pBufferEnd - pBufferCurrent) > 0, kGAErrorDebugMemoryExhaustion, kGAErrorText[kGAErrorDebugMemoryExhaustion], NULL, NULL); // The above logic is set up to always guarantee at least one free byte at the end. // If this fails, PPMalloc is at fault.
		*pBufferCurrent++ = (char)mcTraceRecordDelimiter;
		*pBufferCurrent    = 0;
	}

	return (size_t)(pBufferCurrent - pBuffer);
}



///////////////////////////////////////////////////////////////////////////////
// GetDebugDataLength
//
// User-level debug data size-getting function. Returns the size of the entire
// debug data block. Returns zero if the block could not be found.
//
size_t GeneralAllocatorDebug::GetDebugDataLength(const void* pData, DebugDataLocation debugDataLocation, void** ppDebugData)
{
	#if PPM_THREAD_SAFETY_SUPPORTED
		PPMAutoMutex autoMutex(mpMutex);
	#endif

	size_t nReturnValue = 0;

	if(debugDataLocation == kDebugDataLocationDefault)
		debugDataLocation = mnDebugDataLocationDefault;

	if(debugDataLocation == kDebugDataLocationBlock)
	{
		const Chunk* const pChunk = GetChunkPtrFromDataPtr(pData);
		const size_type    nUsableChunkSize = GetUsableChunkSize(pChunk);

		// We pass a copy of the entire user block, not just starting at wherever the debug data happens to be.
		nReturnValue = GetDebugDataLengthInternal((const char*)pChunk + kDataPtrOffset, nUsableChunkSize - kDataPtrOffset, ppDebugData);
	}
	else // Else kDebugDataLocationSeparate
	{
		if(mbHashTableEnabled)
		{
			// Read the length from the map.
			char** ppDebugBlock = NULL;
			const bool bResult = mHashTable.Find(pData, &ppDebugBlock);
			PPM_ASSERT(this, bResult, kGAErrorBadUserPtr, kGAErrorText[kGAErrorBadUserPtr], NULL, NULL); // If this fails, it is probably the user's fault. The pointer is invalid and may have already been freed.

			if(bResult && ppDebugBlock)
			{
				const DebugDataLengthType nDebugBlockSize = *((DebugDataLengthType*)(*ppDebugBlock)); // 'nDebugBlockSize' refers to the entire size of this block, including the space holding nDebugBlockSize.
				if(nDebugBlockSize) // It can be zero in the case that all debug information is in the block and not separate.
					nReturnValue = GetDebugDataLengthInternal(*ppDebugBlock + sizeof(nDebugBlockSize), nDebugBlockSize - sizeof(nDebugBlockSize), ppDebugData);
			}
		}
	}

	return nReturnValue;
}


///////////////////////////////////////////////////////////////////////////////
// GetDebugDataLengthInternal
//
size_t GeneralAllocatorDebug::GetDebugDataLengthInternal(const char* pDebugBlock, size_t nDebugBlockLength, void** ppDebugData) const
{
	// No mutex lock here because this is an internal function.
	#if PPM_THREAD_SAFETY_SUPPORTED
	PPM_ASSERT(this, !mpMutex || PPMMutexGetLockCount(mpMutex) > 0, kGAErrorInternal, kGAErrorText[kGAErrorInternal], NULL, NULL); // Verify that the mutex is in fact locked. // If this fails, PPMalloc is at fault.
	#endif

	const char* const pPositionDebugSize = pDebugBlock + nDebugBlockLength - sizeof(DebugDataLengthType);
	DebugDataLengthType nDebugDataSize; // Refers to size of all debug data not including the field holding this size value.

	memcpy(&nDebugDataSize, pPositionDebugSize, sizeof(DebugDataLengthType));

	PPM_ASSERT(this, ((size_t)nDebugDataSize + sizeof(DebugDataLengthType)) <= nDebugBlockLength, kGAErrorCorruptedChunk, kGAErrorText[kGAErrorCorruptedChunk], NULL, pDebugBlock); // If this fails, the user probably corrupted the heap.
	if(ppDebugData)
		*ppDebugData = const_cast<char*>(((pDebugBlock + nDebugBlockLength) - (nDebugDataSize + sizeof(DebugDataLengthType))));

	return (size_t)(nDebugDataSize + sizeof(DebugDataLengthType));
}


///////////////////////////////////////////////////////////////////////////////
// SetDebugData
//
// User-level debug data getting function.
//
void* GeneralAllocatorDebug::GetDebugData(const void* pData, DebugDataIdType id, void* pDebugData, size_t dataLength, size_t* pActualDataLength, DebugDataLocation debugDataLocation) const
{
	#if PPM_THREAD_SAFETY_SUPPORTED
		PPMAutoMutex autoMutex(mpMutex);
	#endif

	void* pReturnValue = NULL;

	const bool bValid = VerifyFromDebugDataMap(pData);
	PPM_ASSERT(this, bValid, kGAErrorBadUserPtr, kGAErrorText[kGAErrorBadUserPtr], NULL, pData); // If this fails, it is probably the user's fault. The pointer is invalid and may have already been freed.

	if(bValid) // If we don't track for valid pointers or if the pointer is valid...
	{
		if(debugDataLocation == kDebugDataLocationDefault)
			debugDataLocation = GetDefaultDebugDataLocation(id);

		if(debugDataLocation == kDebugDataLocationBlock)
		{
			const Chunk* const pChunk = GetChunkPtrFromDataPtr(pData);
			const size_type    nUsableChunkSize = GetUsableChunkSize(pChunk);

			// We pass a copy of the entire user block, not just starting at wherever the debug data happens to be.
			pReturnValue = GetDebugDataStatic((const char*)pChunk + kDataPtrOffset, nUsableChunkSize - kDataPtrOffset, id, pDebugData, dataLength, pActualDataLength);
		}
		else
		{
			if(mbHashTableEnabled)
			{
				// Read the length from the map.
				char** ppDebugBlock = NULL;
				const bool bResult = mHashTable.Find(pData, &ppDebugBlock);
				PPM_ASSERT(this, bResult, kGAErrorBadUserPtr, kGAErrorText[kGAErrorBadUserPtr], NULL, pData); // If this fails, it is probably the user's fault. The pointer is invalid and may have already been freed.

				if(bResult && ppDebugBlock)
				{
					const DebugDataLengthType nDebugBlockSize = *((DebugDataLengthType*)(*ppDebugBlock)); // 'nDebugBlockSize' refers to the entire size of this block, including the space holding nDebugBlockSize.
					if(nDebugBlockSize) // It can be zero in the case that all debug information is in the block and not separate.
						pReturnValue = GetDebugDataStatic(*ppDebugBlock + sizeof(nDebugBlockSize), nDebugBlockSize - sizeof(nDebugBlockSize), id, pDebugData, dataLength, pActualDataLength);
				}
			}
		}
	}

	return pReturnValue;
}



///////////////////////////////////////////////////////////////////////////////
// GetDebugDataStatic
//
// Reads debug data from an arbitrary buffer.
//
// Description of a GeneralAllocatorDebug Chunk, with some example debug data:
//
//       |----+----|-----------------------------------|----------+------+------------+------------+------------------------------------------+|
//       Header    pData                               Debug data                                                                              Next Chunk
//       Prev Cur                                      GuardFill  Time   Count        File/line    Callstack                                  debug data size (2 bytes at end)
//
// Description of the Debug data area:
//
//      |--------------------------------|------------------|-----------------------|------------------ ...
//      Guard fill data                  kDebugDataIdGuard  Guard field size        Time data  
//      G bytes                          2 bytes            2 bytes                 T bytes
//                                                          value equal to G + 2
//
// In this function, the pDebugBlock space actually begins at the end of 
// the Chunk and ends at "Debug data" in the above diagram. So it is ordered
// backwards in the chunk. This is because that allows us to find it, given an 
// arbitrary Chunk with user data at the front. All we really need to know 
// in order to navigate the debug data of a Chunk is the location of the 
// two byte debug data size (nDebugDataSize) that is always at the very end 
// of a Chunk. That size allows us to walk the debug data backwards, as we 
// do below. It usually happens that the caller of this function passes
// pData for pDebugBlock but with an nDebugBlockLength that still makes it
// so pData + nDebugBlockLength points to the end of the Chunk.
//
void* GeneralAllocatorDebug::GetDebugDataStatic(const char* pDebugBlock, size_t nDebugBlockLength, DebugDataIdType id, void* pDebugData, size_t dataLength, size_t* pActualDataLength)
{
	const char*         pPosition;      // The current position we are reading from in the data.
	const char*         pPositionEnd;   // One-past the last valid position in the data. Note that we read from right-to-left, so pPositionEnd <= pPosition.
	DebugDataLengthType nDebugDataSize; // Refers to size of all debug data not including the field holding this size value.
	DebugDataLengthType nFieldSize;     // Refers to size of individual debug data not including the field holding this size value and not including the field id size.
	DebugDataIdType     nFieldId;

	pPosition = pDebugBlock + nDebugBlockLength - sizeof(DebugDataLengthType);
	memcpy(&nDebugDataSize, pPosition, sizeof(DebugDataLengthType));
	pPositionEnd = pPosition - nDebugDataSize; // pPositionEnd points to the first byte of debug information in the block.
	PPM_ASSERT_STATIC(pPositionEnd >= pDebugBlock, kGAErrorCorruptedChunk, kGAErrorText[kGAErrorCorruptedChunk], NULL, pDebugBlock); // If this fails, the user likely corrupted the heap.

	if(pPositionEnd >= pDebugBlock) // Just to be safe...
	{
		while(pPosition > pPositionEnd) // While there is more debug information...
		{
			pPosition -= sizeof(DebugDataLengthType);
			memcpy(&nFieldSize, pPosition, sizeof(DebugDataLengthType));
			pPosition -= sizeof(DebugDataIdType);
			memcpy(&nFieldId, pPosition, sizeof(DebugDataIdType));
			pPosition -= nFieldSize;

			if(nFieldId == id) // If this is the field the user was looking for...
			{
				if(pDebugData) // If the user provided a buffer to copy the field data into... (else we'll just return its length below).
				{
					dataLength = PPM_MIN(dataLength, nFieldSize);
					memcpy(pDebugData, pPosition, dataLength);

					if((id == kDebugDataIdName) && dataLength) // If it's the kDebugDataIdName type, which is a 0-terminated string...
					{
						char* pLastChar = (char*)((uintptr_t)pDebugData + (dataLength - 1));
						*pLastChar = 0; // Guarantee that the returned string is 0-terminated.
					}
				}

				if(pActualDataLength)
					*pActualDataLength = nFieldSize;
				return (void*)pPosition;
			}
		}
	}

	if(pActualDataLength)
		*pActualDataLength = 0;
	return NULL;
}



///////////////////////////////////////////////////////////////////////////////
// SetDebugData
//
// User-level debug data setting function.
// If the data location is kDebugDataLocationBlock, then the space is required
// to already be free for setting (otherwise we'd need to be able to go back 
// in time, and that hasn't been invented yet). If the data location is 
// kDebugDataLocationSeparate, then we possibly resize the separate data area
// to hold this debug information if needed.
//
void* GeneralAllocatorDebug::SetDebugData(void* pData, DebugDataIdType id, const void* pDebugData, size_t dataLength, DebugDataLocation debugDataLocation)
{
	#if PPM_THREAD_SAFETY_SUPPORTED
		PPMAutoMutex autoMutex(mpMutex);
	#endif

	void* pReturnValue = NULL;

	PPM_ASSERT(this, dataLength <= kMaxDebugDataSize, kGAErrorRequestedSizeTooLarge, kGAErrorText[kGAErrorRequestedSizeTooLarge], NULL, pData); // If this fails, it is the user's fault. Too much data.

	if(debugDataLocation == kDebugDataLocationDefault)
		debugDataLocation = GetDefaultDebugDataLocation(id);

	if(debugDataLocation == kDebugDataLocationBlock)
	{
		const Chunk* const pChunk = GetChunkPtrFromDataPtr(pData);
		const size_type    nUsableChunkSize = GetUsableChunkSize(pChunk);

		// We pass a copy of the entire user block, not just starting at wherever the debug data happens to be.
		pReturnValue = SetDebugDataStatic((char*)pChunk + kDataPtrOffset, nUsableChunkSize - kDataPtrOffset, id, pDebugData, dataLength);
	}
	else // kDebugDataLocationSeparate
	{
		if(mbHashTableEnabled)
		{
			// Get map data, possibly resize it, and write to it.
			char** ppDebugBlock = NULL;
			const bool bResult = mHashTable.Find(pData, &ppDebugBlock);

			PPM_ASSERT(this, bResult, kGAErrorBadUserPtr, kGAErrorText[kGAErrorBadUserPtr], NULL, pData); // If this fails, it is probably the user's fault. The pointer is invalid and may have already been freed.
			if(bResult)
			{
				bool  bDebugBlockAllocated = false;
				char* pDebugBlock = *ppDebugBlock; // This is just a convenient alias.
				DebugDataLengthType nDebugBlockSize;

				if(pDebugBlock)
				{
					nDebugBlockSize = *((DebugDataLengthType*)pDebugBlock); // 'nDebugBlockSize' refers to the entire size of this block, including the space holding nDebugBlockSize.
					
					// Here we do a query to see if the given debug data is present.
					size_t nActualDataLength = 0;
					const void* const pDebugDataTest = GetDebugDataStatic(pDebugBlock + sizeof(nDebugBlockSize), nDebugBlockSize - sizeof(nDebugBlockSize), id, NULL, 0, &nActualDataLength);

					if(!pDebugDataTest) // If the record doesn't currently exist...
					{
						size_t nNewDebugBlockSize  = nDebugBlockSize + kDebugRecordOverheadSize + dataLength;
						char* const pNewDebugBlock = (char*)AllocatorMalloc(nNewDebugBlockSize);

						if(pNewDebugBlock)
						{
							memcpy(pNewDebugBlock + (nNewDebugBlockSize - nDebugBlockSize), pDebugBlock, nDebugBlockSize);
							AllocatorFree(pDebugBlock);

							pDebugBlock     = pNewDebugBlock;
							nDebugBlockSize = (DebugDataLengthType)nNewDebugBlockSize;

							*((DebugDataLengthType*)pDebugBlock) = nDebugBlockSize;
							*ppDebugBlock = pDebugBlock;

							bDebugBlockAllocated = true;
						}
					}
				}
				else // Else we need to create the block for the first time here.
				{
					// Add three shorts because one is needed for the block record size, and two are needed for the individual record: 
					nDebugBlockSize = (DebugDataLengthType)(sizeof(nDebugBlockSize) + sizeof(DebugDataLengthType) + kDebugRecordOverheadSize + dataLength);
					pDebugBlock     = (char*)AllocatorMalloc(nDebugBlockSize);

					if(pDebugBlock)
					{
						memset(pDebugBlock + nDebugBlockSize - sizeof(DebugDataLengthType), 0, sizeof(DebugDataLengthType)); // Set the block size (which is at the end of the data) to zero.

						*((DebugDataLengthType*)pDebugBlock) = nDebugBlockSize;
						*ppDebugBlock = pDebugBlock;

						bDebugBlockAllocated = true;
					}
				}

				if(pDebugBlock)
				{
					pReturnValue = SetDebugDataStatic(pDebugBlock + sizeof(nDebugBlockSize), nDebugBlockSize - sizeof(nDebugBlockSize), id, pDebugData, dataLength);
					if(!pReturnValue && bDebugBlockAllocated)
						AllocatorFree(pDebugBlock);
				}
			}
		}
	}

	return pReturnValue;
}


///////////////////////////////////////////////////////////////////////////////
// SetDebugDataStatic
//
// Writes a debug record to a debug block. If the input DebugData is NULL, 
// then the address of the debug data is returned.
//
// Algorithm:
//    Find the given id, which may or may not currently exist.
//    If it already exists, use it. Assert that it is big enough to hold input data length. Sorry, no realloc for now.
//    If it doesn't already exist, create a new field if possible. Enough space should have been set aside.
//    Write the total size field if new field was created.
//
void* GeneralAllocatorDebug::SetDebugDataStatic(char* pDebugBlock, size_t nDebugBlockLength, DebugDataIdType id, const void* pDebugData, size_t dataLength)
{
	char*               pPosition;           // The current position we are reading from in the data.
	char*               pPositionEnd;        // One-past the last valid position in the data. Note that we read from right-to-left, so pPositionEnd <= pPosition.
	char*               pPositionDebugSize;  // Position of where the data size field is.
	DebugDataLengthType nDebugDataSize;      // Refers to size of all debug data not including the field holding this size value. Thus its initial value is zero and not sizeof(unsigned short).
	DebugDataLengthType nFieldSize;          // Refers to size of individual debug data not including the field holding this size value and not including the field id size.
	DebugDataIdType     nFieldId;            // Current field id.

	pPosition = pDebugBlock + nDebugBlockLength - sizeof(DebugDataLengthType);
	pPositionDebugSize = pPosition;
	memcpy(&nDebugDataSize, pPosition, sizeof(DebugDataLengthType));
	PPM_ASSERT_STATIC(nDebugDataSize <= nDebugBlockLength, kGAErrorInternal, kGAErrorText[kGAErrorInternal], NULL, NULL); // If this fails, PPMalloc is probably at fault.
	pPositionEnd = pPosition - nDebugDataSize;

	while(pPosition > pPositionEnd)
	{
		pPosition -= sizeof(DebugDataLengthType);
		memcpy(&nFieldSize, pPosition, sizeof(DebugDataLengthType));
		pPosition -= sizeof(DebugDataIdType);
		memcpy(&nFieldId, pPosition, sizeof(DebugDataIdType));
		pPosition -= nFieldSize;

		if(nFieldId == id)
		{
			// Overwrite the existing record data.
			PPM_ASSERT_STATIC(dataLength <= nFieldSize, kGAErrorInternal, kGAErrorText[kGAErrorInternal], NULL, NULL); // If this fails, PPMalloc is probably at fault.

			if(dataLength <= nFieldSize)
			{
				// What we might want to do here is resize the field(s) if dataLength != nFieldSize.
				if(pDebugData)
				{
					memcpy(pPosition, pDebugData, dataLength);
					if(dataLength < nFieldSize)
						memset(pPosition + dataLength, 0, nFieldSize - dataLength);
				}
				#if (PPM_DEBUG >= 2) // Extended user debug builds.
					else
						memset(pPosition, 0, nFieldSize);
				#endif

				return pPosition;
			}

			memset(pPosition, 0, nFieldSize); // This is bad.
			return NULL;
		}
	}

	//PPM_ASSERT_STATIC(pPosition == pPositionEnd, kGAErrorInternal, kGAErrorText[kGAErrorInternal], NULL, NULL); // If this fails, PPMalloc is at fault.
	//PPM_ASSERT_STATIC(((pPosition - kDebugRecordOverheadSize) - dataLength) >= pDebugBlock, kGAErrorInternal, kGAErrorText[kGAErrorInternal], NULL, NULL); // If this fails, PPMalloc is at fault.

	if(((pPosition - kDebugRecordOverheadSize) - dataLength) >= pDebugBlock) // If the record data wouldn't go out of bounds...
	{
		// Write our new record data.
		pPosition -= sizeof(DebugDataLengthType);
		DebugDataLengthType dataLengthTemp = (DebugDataLengthType)dataLength; // Need to make a temporary so the size written matches the data type size.
		memcpy(pPosition, &dataLengthTemp, sizeof(dataLengthTemp));
		pPosition -= sizeof(DebugDataIdType);
		memcpy(pPosition, &id, sizeof(DebugDataIdType));
		pPosition -= dataLength;
		if(pDebugData)
			memcpy(pPosition, pDebugData, dataLength);
		#if (PPM_DEBUG >= 2) // Extended user debug builds.
			else
				memset(pPosition, 0, dataLength);
		#endif

		// Update the debug data size field.
		nDebugDataSize = (DebugDataLengthType)(nDebugDataSize + dataLength + kDebugRecordOverheadSize);
		memcpy(pPositionDebugSize, &nDebugDataSize, sizeof(DebugDataLengthType));

		return pPosition;
	}

	return NULL;
}



} // namespace Allocator

} // namespace EA


#if   defined(_MSC_VER)
	#pragma warning(pop)
#endif













