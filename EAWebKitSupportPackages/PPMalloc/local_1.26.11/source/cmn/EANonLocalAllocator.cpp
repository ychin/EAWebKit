///////////////////////////////////////////////////////////////////////////////
// EANonLocalAllocator
//
// Copyright (c) 2007, Electronic Arts. All Rights Reserved.
// Created by Paul Pedriana
///////////////////////////////////////////////////////////////////////////////


#include <PPMalloc/EANonLocalAllocator.h>
#include <PPMalloc/EAGeneralAllocatorDebug.h>
#include <PPMalloc/internal/shared.h>
#include <string.h>
#include <stdio.h>


#if defined(_MSC_VER)
	#pragma warning(push)
	#pragma warning(disable: 6011) // Dereferencing NULL pointer
#endif


///////////////////////////////////////////////////////////////////////////////
// PPM_VERIFY
// Returns 0 if OK and 1 if false. The purpose is in order to assist
// in making functions that both assert while counting errors.
#if PPM_NLA_HEAP_VALIDATION_SUPPORTED
	static int PPM_VERIFY(const EA::Allocator::NonLocalAllocator* const pAllocator, bool bExpression, 
							int id, const char* pDescription, const void* pData, const void* pLocation)
	{
		EA_UNUSED(pAllocator); EA_UNUSED(id); EA_UNUSED(pDescription); EA_UNUSED(pData); EA_UNUSED(pLocation);

		if(bExpression)
			return 0;
		PPM_ASSERT(pAllocator, false, id, pDescription, (void*)pData, (void*)pLocation);
		return 1;
	}
#endif
///////////////////////////////////////////////////////////////////////////////



namespace EA
{
namespace Allocator
{


NonLocalAllocator::NonLocalAllocator()
{
	Construct();
}


NonLocalAllocator::NonLocalAllocator(LocalAllocationFunction pLocalAllocationFunction, 
									  LocalFreeFunction pLocalFreeFunction, void* pLocalAllocationContext,
									  void* pCore, size_t nSize, CoreFreeFunction pCoreFreeFunction, 
									  void* pCoreFreeFunctionContext, size_t nInitialBucketCount)
{
	Construct();
	Init(pLocalAllocationFunction, pLocalFreeFunction, pLocalAllocationContext, 
		 pCore, nSize, pCoreFreeFunction, pCoreFreeFunctionContext, nInitialBucketCount);
}


void NonLocalAllocator::Construct()
{
	PPM_CT_ASSERT((size_t)kMinAlignment >= (size_t)kMinAllocSize);
	PPM_CT_ASSERT((size_t)kMaxAllocSize <  (size_t)kNodeInUseBit);

	mbInitialized                = false;
	mbEnableDebugReads           = false;
	mbEnableDebugWrites          = false;
	mMinSplitSize                = kMinAllocSize;
	mFailureValue                = NULL;
	mbMallocFailureAssertEnabled = false;

	mpMemcpyFunction          = memcpy;
	mpMemsetFunction          = memset;
	mpMemoryFillCheckFunction = MemoryFillCheck;

	mpLocalAllocationFunction   = NULL;
	mpLocalFreeFunction         = NULL;
	mpLocalAllocationContext    = NULL;

	#if PPM_NLA_DEBUG_DATA_SUPPORTED
		mpLocalDebugAllocationFunction = NULL;
		mpLocalDebugFreeFunction       = NULL;
		mpLocalDebugAllocationContext  = NULL;
	#endif

	mpMallocFailureFunction        = NULL;
	mpMallocFailureFunctionContext = NULL;
	mnMaxMallocFailureCount        = 4;

	mpAssertionFailureFunction        = &AssertionFailureFunctionDefault;
	mpAssertionFailureFunctionContext = NULL;

	mpTraceFunction             = &TraceFunctionDefault;
	mpTraceFunctionContext      = NULL;
	mcTraceFieldDelimiter       = (unsigned char)'\t';
	mcTraceRecordDelimiter      = (unsigned char)'\n';

	#if (PPM_DEBUG >= 2) // Extended user debug builds.
		mnDebugDataFlags = (1 << kDebugDataIdGuard);
	#endif

	mbHeapValidationActive = false;

	#if PPM_AUTO_HEAP_VALIDATION_SUPPORTED
		mAutoHeapValidationLevel        = kHeapValidationLevelNone;
		mnAutoHeapValidationFrequency   = 0;
		mnAutoHeapValidationEventCount  = 0;
	#endif

	#if PPM_THREAD_SAFETY_SUPPORTED
		mpMutex = NULL;
		memset(mpMutexData, 0, sizeof(mpMutexData));

		#if PPM_THREAD_SAFETY_BY_DEFAULT
			SetOption(kOptionEnableThreadSafety, 1);
		#endif
	#endif

	#if PPM_NLA_DEBUG_DATA_SUPPORTED
		memset(&mDebugDataInfo, 0, sizeof(mDebugDataInfo));
		mnDebugDataFlags  = 0;
		mnCurrentGroupId  = 0;

		mfGuardSizeRatio  = 0.25f;
		mnMinGuardSize    = 8;
		mnMaxGuardSize    = 10000;

		mnFillFree        = kDefaultFillValueFree;
		mnFillDelayedFree = kDefaultFillValueDelayedFree;
		mnFillNew         = kDefaultFillValueNew;
		mnFillGuard       = kDefaultFillValueGuard;
		mnFillUnusedCore  = kDefaultFillValueUnusedCore;
	#endif

	#if PPM_NLA_HOOKS_SUPPORTED
		mpHookFunction        = NULL; 
		mpHookFunctionContext = NULL;
	#endif

	#if PPM_NLA_METRICS_SUPPORTED
		mbMetricsEnabled = false;
		mnAllocationCountHistorical = 0;
		memset(&mAllocationMetrics, 0, sizeof(mAllocationMetrics));
	#endif

	memset(&mHeadCoreBlock, 0, sizeof(mHeadCoreBlock));
	mHeadCoreBlock.mpListNext = &mHeadCoreBlock;
	mHeadCoreBlock.mpListPrev = &mHeadCoreBlock;

	memset(&mCoreBlockFirst, 0, sizeof(mCoreBlockFirst));

	memset(mFreeListArray, 0, sizeof(mFreeListArray));
	for(size_t i = 0; i < kBinCount; i++)
	{
		mFreeListArray[i].mpListNext = &mFreeListArray[i];
		mFreeListArray[i].mpListPrev = &mFreeListArray[i];
	}
}


NonLocalAllocator::~NonLocalAllocator()
{
	NonLocalAllocator::Shutdown();
}


bool NonLocalAllocator::Init(LocalAllocationFunction pLocalAllocationFunction, LocalFreeFunction pLocalFreeFunction, void* pLocalAllocationContext,
							  void* pCore, size_t nSize, CoreFreeFunction pCoreFreeFunction, void* pCoreFreeFunctionContext, size_t nInitialBucketCount)
{
	InitWithoutCore(pLocalAllocationFunction, pLocalFreeFunction, pLocalAllocationContext, nInitialBucketCount);

	return AddCore(pCore, nSize, pCoreFreeFunction, pCoreFreeFunctionContext);
}

bool NonLocalAllocator::InitWithoutCore(LocalAllocationFunction pLocalAllocationFunction, LocalFreeFunction pLocalFreeFunction, void* pLocalAllocationContext, 
										size_t nInitialBucketCount)
{
	mbInitialized = true;

	if(pLocalAllocationFunction)
	{
		SetLocalAllocationFunction(pLocalAllocationFunction, pLocalFreeFunction, pLocalAllocationContext);
		SetLocalDebugAllocationFunction(pLocalAllocationFunction, pLocalFreeFunction, pLocalAllocationContext);
	}

	if(nInitialBucketCount > 0)
		mHashTable.ReHash(nInitialBucketCount);

	return true;
}

bool NonLocalAllocator::Shutdown()
{
	if(mbInitialized)
	{
		mbInitialized = false;

		// Possibly trace allocated memory.
		#if (PPM_DEBUG >= 2) // Extended user debug builds.
			if(mpTraceFunction)
			{
				char pTraceHeading[128];
				PPM_Snprintf(pTraceHeading, 128, "NonLocalAllocator shutdown. Outstanding allocations:\n");
				mpTraceFunction(pTraceHeading, mpTraceFunctionContext);
			}

			TraceAllocatedMemory(); // Trace all allocated (i.e. leaked) memory.
		#endif

		mHashTable.Clear(false);

		// Free core memory
		while(mHeadCoreBlock.mpListNext != &mHeadCoreBlock) // While the CoreBlock list is not empty...
		{
			CoreBlock* const pCoreBlock = (CoreBlock*)mHeadCoreBlock.mpListNext;

			// Free any existing nodes associated with the CoreBlock.
			// If there are no memory leaks, then there will be exactly one Node per 
			// CoreBlock and that Node's size will be equal to the CoreBlock's size. 
			Node* pNode = pCoreBlock->mpMemNext;

			while(pNode != pCoreBlock)
			{
				Node* const pNext = pNode->mpMemNext;
				DeallocateNode(pNode);
				pNode = pNext;
			}

			// Remove pCoreBlock from the mHeadCoreBlock list.
			pCoreBlock->RemoveFromFreeList();

			if(pCoreBlock->mpCoreFreeFunction)
				pCoreBlock->mpCoreFreeFunction(this, pCoreBlock->mpBlock, pCoreBlock->GetSize(), pCoreBlock->mpCoreFreeFunctionContext);

			if(pCoreBlock != &mCoreBlockFirst) // mCoreBlockFirst is a member variable and isn't allocated on the heap. So we don't free it.
				DeallocateCoreBlock(pCoreBlock);
		}
	}

	#if PPM_THREAD_SAFETY_SUPPORTED
		if(mpMutex)
		{
			PPMMutexDestroy(mpMutex);
			mpMutex = NULL;
		}
	#endif

	return true;
}


// This previously was inline in the header, but that created a DLL build problem whereby 
// PPMMutexLock/PPMMutexUnlock were not DLL-exported. 
void NonLocalAllocator::Lock(bool bEnable)
{
	#if PPM_THREAD_SAFETY_SUPPORTED
		if(mpMutex)
		{
			if(bEnable)                 // This 'if' will be optimized away when bEnable is 
				PPMMutexLock(mpMutex);  // passed as 'true' or 'false', which it almost always is.
			else
				PPMMutexUnlock(mpMutex);
		}
	#else
		EA_UNUSED(bEnable);
	#endif
}


void NonLocalAllocator::SetLocalAllocationFunction(LocalAllocationFunction pLocalAllocationFunction, 
													LocalFreeFunction pLocalFreeFunction, void* pLocalAllocationContext)
{
	mpLocalAllocationFunction = pLocalAllocationFunction;
	mpLocalFreeFunction       = pLocalFreeFunction;
	mpLocalAllocationContext  = pLocalAllocationContext;

	mHashTable.SetAllocationFunction(this, pLocalAllocationFunction, pLocalFreeFunction, pLocalAllocationContext);
}


void NonLocalAllocator::SetLocalDebugAllocationFunction(LocalAllocationFunction pLocalAllocationFunction, 
														 LocalFreeFunction pLocalFreeFunction, void* pLocalAllocationContext)
{
	#if PPM_NLA_DEBUG_DATA_SUPPORTED
		mpLocalDebugAllocationFunction = pLocalAllocationFunction;
		mpLocalDebugFreeFunction       = pLocalFreeFunction;
		mpLocalDebugAllocationContext  = pLocalAllocationContext;
	#else
		EA_UNUSED(pLocalAllocationFunction);
		EA_UNUSED(pLocalFreeFunction);
		EA_UNUSED(pLocalAllocationContext);
	#endif
}


bool NonLocalAllocator::AddCore(void* pCore, size_t nSize, CoreFreeFunction pCoreFreeFunction, void* pCoreFreeFunctionContext)
{
	#if PPM_THREAD_SAFETY_SUPPORTED
		PPMAutoMutex autoMutex(mpMutex);
	#endif

	PPM_ASSERT(this, pCore != mFailureValue,   kGAErrorSuppliedCoreInvalid, kGAErrorText[kGAErrorSuppliedCoreInvalid], NULL, pCore); // If this fails, it is the user's fault.
	PPM_ASSERT(this, PPM_IsAligned(pCore, 16), kGAErrorSuppliedCoreInvalid, kGAErrorText[kGAErrorSuppliedCoreInvalid], NULL, pCore); // If this fails, it is the user's fault.
	PPM_ASSERT(this, nSize >= 16,              kGAErrorSuppliedCoreInvalid, kGAErrorText[kGAErrorSuppliedCoreInvalid], NULL, pCore); // If this fails, it is the user's fault.

	Node* const pNode = AllocateNode(); // In order to add new core memory, we need to at least be able to allocate one node for it.

	if(pNode)
	{
		// If mCoreBlockFirst is unused, then we assign pCore to it. Otherwise we allocate a
		// new CoreBlock and append it to our CoreBlock linked list (headed by mHeadCoreBlock).
		CoreBlock* const pCoreBlock = mCoreBlockFirst.mpBlock ? AllocateCoreBlock() : &mCoreBlockFirst;

		if(pCoreBlock)
		{
			// Setup the CoreBlock
			memset(pCoreBlock, 0, sizeof(CoreBlock));

			pCoreBlock->mpBlock                     = (char*)pCore;
			pCoreBlock->SetSize((size_type)nSize);
			pCoreBlock->mpMemPrev                   = pCoreBlock;
			pCoreBlock->mpMemNext                   = pCoreBlock;
			pCoreBlock->mpCoreFreeFunction          = pCoreFreeFunction;
			pCoreBlock->mpCoreFreeFunctionContext   = pCoreFreeFunctionContext;
			pCoreBlock->SetIsInUse(true);
			pCoreBlock->InsertIntoFreeList(mHeadCoreBlock.mpListPrev);

			// Setup the Node
			pNode->mpBlock = (char*)pCore;
			pNode->SetSize((size_type)nSize);
			pNode->mpData = mFailureValue;  // Usually mFailureValue is NULL (0), but for some environments 0 is a valid address and so mFailureValue could be something else.

			#if PPM_NLA_DEBUG_DATA_SUPPORTED
				pNode->mpDebugData = NULL;

				if(mbEnableDebugWrites && mpMemsetFunction)
					mpMemsetFunction(pCore, mnFillFree, nSize); // We fill with mnFillFree instead of mnFillUnusedCore because we immediately make this a free block below.
			#endif

			pNode->InsertIntoMemList(pCoreBlock);
			pNode->SetIsInUse(false);
			AddNodeToFreeList(pNode);

			return true;
		}

		DeallocateNode(pNode);
	}

	return false;
}


bool NonLocalAllocator::ExtendCore(void* pCore, size_t nSize)
{
	#if PPM_THREAD_SAFETY_SUPPORTED
		PPMAutoMutex autoMutex(mpMutex);
	#endif

	const bool bIsAligned = PPM_IsAligned(nSize, kMinAllocSize);
	PPM_ASSERT(this, bIsAligned, kGAErrorSuppliedCoreInvalid, kGAErrorText[kGAErrorSuppliedCoreInvalid], pCore, &nSize);

	if(bIsAligned)
	{
		for(CoreBlock* pCoreBlock  = (CoreBlock*)mHeadCoreBlock.mpListNext; 
					   pCoreBlock != &mHeadCoreBlock; 
					   pCoreBlock  = (CoreBlock*)pCoreBlock->mpListNext)
		{
			if(pCoreBlock->mpBlock == (char*)pCore)
			{
				// Get the last memory block in this core. If that block is allocated, create a new node.
				// If that block is free, adjust its size accordingly.

				Node* const pNodeLast = pCoreBlock->mpMemPrev;

				PPM_ASSERT(this, pNodeLast->mpBlock + pNodeLast->GetSize() == pCoreBlock->mpBlock + pCoreBlock->GetSize(), kGAErrorCorruptedChunk, kGAErrorText[kGAErrorCorruptedChunk], pNodeLast, pNodeLast);

				pCoreBlock->mnBlockSize += (size_type)nSize;

				if(pNodeLast->IsInUse())
				{
					Node* const pNode = AllocateNode();

					if(pNode)
					{
						#if PPM_NLA_DEBUG_DATA_SUPPORTED
							pNode->mpDebugData = NULL;
						#endif

						pNode->mpBlock = (pNodeLast->mpBlock + pNodeLast->GetSize());
						pNode->SetSize((size_type)nSize);
						pNode->InsertIntoMemList(pNodeLast);
						pNode->SetIsInUse(false);
						AddNodeToFreeList(pNode);

						return true;
					}
				}
				else
				{
					pNodeLast->RemoveFromFreeList(); // Need to remove it and re-add it because the free list is sorted by size, and size is changing here.
					pNodeLast->SetSize(pNodeLast->GetSize() + (size_type)nSize);
					AddNodeToFreeList(pNodeLast);

					return true;
				}
			}
		}
	}

	return false;
}


size_t NonLocalAllocator::TrimCore(void* pCore)
{
	size_t nFreedCoreBlockCount = 0;

	#if PPM_THREAD_SAFETY_SUPPORTED
		PPMAutoMutex autoMutex(mpMutex);
	#endif

	if(pCore) // If the user wants to free a specific CoreBlock.
	{
		// Find the CoreBlock for this memory and see if it is unused (and thus can be freed).
		CoreBlock* const pCoreBlock = FindCoreBlockForAddress(pCore);

		if(pCoreBlock) // If pCore refers to a currently valid CoreBlock...
		{
			Node* const     pNode          = pCoreBlock->mpMemNext;
			const size_type nCoreBlockSize = pCoreBlock->GetSize();
			const size_type nNodeSize      = pNode->GetSize();

			if(nNodeSize == nCoreBlockSize) // If the node size is equal to the core size...
			{
				if(mHashTable.Find(pNode->mpData) == NULL) // and if the node represents free memory...
				{
					// Remove and free the Node that manages the CoreBlock memory.
					pNode->RemoveFromFreeList();
					DeallocateNode(pNode);

					// Free the space the CoreBlock refers to.
					if(pCoreBlock->mpCoreFreeFunction)
						pCoreBlock->mpCoreFreeFunction(this, pCoreBlock->mpBlock, pCoreBlock->GetSize(), pCoreBlock->mpCoreFreeFunctionContext);

					// Remove and free the CoreBlock itself.
					pCoreBlock->RemoveFromFreeList();

					if(pCoreBlock == &mCoreBlockFirst) // mCoreBlockFirst is a member variable and isn't allocated on the heap. So we don't free it.
						memset(&mCoreBlockFirst, 0, sizeof(mCoreBlockFirst));
					else
						DeallocateCoreBlock(pCoreBlock);

					nFreedCoreBlockCount = 1;
				}
			}
		}
	}
	else // Else the user wants to free all unused CoreBlocks.
	{
		for(CoreBlock* pCoreBlock  = (CoreBlock*)mHeadCoreBlock.mpListNext; 
					   pCoreBlock != &mHeadCoreBlock; )
		{
			// We save the next CoreBlock because the call below might destroy pCoreBlock.
			CoreBlock* const pCoreBlockNext = (CoreBlock*)pCoreBlock->mpListNext;

			// This call (recusively to ourself) may or may not free the CoreBlock.
			nFreedCoreBlockCount += TrimCore(pCoreBlock->mpBlock);

			pCoreBlock = pCoreBlockNext;
		}
	}

	return nFreedCoreBlockCount;
}


NonLocalAllocator::CoreBlock* NonLocalAllocator::FindCoreBlockForAddress(const void* pAddress) const
{
	for(CoreBlock* pCoreBlock  = (CoreBlock*)mHeadCoreBlock.mpListPrev; 
				   pCoreBlock != &mHeadCoreBlock; 
				   pCoreBlock  = (CoreBlock*)pCoreBlock->mpListPrev)
	{
		if((pAddress >= (void*)pCoreBlock->mpBlock) && (pAddress < ((char*)pCoreBlock->mpBlock + pCoreBlock->GetSize())))
			return pCoreBlock;
	}

	return NULL;
}


void* NonLocalAllocator::Malloc(size_t nSize, int nAllocationFlags)
{
	PPM_ASSERT(this, nSize < kMaxAllocSize, kGAErrorRequestedSizeTooLarge, kGAErrorText[kGAErrorRequestedSizeTooLarge], NULL, NULL); // If this fails, it is probably the user's fault. The allocation size is too big.

	void* pResult = mFailureValue;

	#if PPM_THREAD_SAFETY_SUPPORTED
		PPMAutoMutex autoMutex(mpMutex);
	#endif

	#if PPM_AUTO_HEAP_VALIDATION_SUPPORTED
		if(EA_UNLIKELY(mAutoHeapValidationLevel && (++mnAutoHeapValidationEventCount >= mnAutoHeapValidationFrequency)))
		{
			mnAutoHeapValidationEventCount = 0;
			ValidateHeap(mAutoHeapValidationLevel);
		}
	#endif

	#if PPM_NLA_DEBUG_DATA_SUPPORTED
		GetDebugInfoForAllocation(mDebugDataInfo, nSize, nAllocationFlags, kMinAlignment);
		PPM_ASSERT(this, nSize < (kMaxAllocSize - mDebugDataInfo.mnGuardFillSize), kGAErrorRequestedSizeTooLarge, kGAErrorText[kGAErrorRequestedSizeTooLarge], NULL, NULL); // If this fails, it is probably the user's fault. The allocation size is too big.
		nSize += mDebugDataInfo.mnGuardFillSize;
	#endif

	#if PPM_NLA_HOOKS_SUPPORTED
		if(mpHookFunction)
		{
			// To consider: We need to solve the problem of the mutex being locked while we call external code.
			// There is no simple solution here, as we've already written to mDebugDataInfo and can't unlock 
			// the mutex without risking the stomping of mDebugDataInfo. But we need the data from mDebugDataInfo
			// to pass to the user. One possibility is to make a local DebugDataInfo or possibly a thread-specific
			// DebugDataInfo. Another possibility is to simply document that the user is limited in what can be 
			// done in the hook callback, as otherwise the user may risk deadlock.
			const HookInfo hookInfo = { this, true, kHookTypeMalloc, kHookSubTypeMalloc, nSize, NULL, 0, nSize, NULL, 0, nAllocationFlags, NULL, NULL, 0 };
			mpHookFunction(&hookInfo, mpHookFunctionContext);
		}
	#endif

	if(nSize < kMaxAllocSize)
	{
		Node* const pNode = MallocNode(nSize, nAllocationFlags);

		if(pNode)
		{
			#if PPM_NLA_DEBUG_DATA_SUPPORTED
				bool fillIsOK = VerifyFreeFill(pNode);
				AttachDebugDataToNode(mDebugDataInfo, pNode, fillIsOK);
			#endif

			mHashTable.Insert(pNode);

			#if PPM_NLA_METRICS_SUPPORTED
				CollectMetrics(pNode, true);
			#endif

			pResult = pNode->mpData;
		}
	}

	#if PPM_NLA_HOOKS_SUPPORTED
		if(mpHookFunction)
		{
			const HookInfo hookInfo = { this, false, kHookTypeMalloc, kHookSubTypeMalloc, nSize, NULL, 0, nSize, NULL, 0, nAllocationFlags, pResult, NULL, 0 };
			mpHookFunction(&hookInfo, mpHookFunctionContext);
		}
	#endif

	return pResult;
}


NonLocalAllocator::Node* NonLocalAllocator::MallocNode(size_t nSize, int nAllocationFlags)
{
	uint32_t        nMallocFailureCount   = 0;
	int             nAllocationFlagsSaved = nAllocationFlags;
	const size_type sizeRequired          = (nSize > kMinAllocSize) ? (size_type)PPM_AlignUp(nSize, kMinAllocSize) : kMinAllocSize;

	PPM_ASSERT(this, sizeRequired <= kMaxAllocSize, kGAErrorInternal, kGAErrorText[kGAErrorInternal], NULL, NULL);

	MallocTop:

	if((nAllocationFlags & kAllocationFlagPerm) == 0)
	{
		for(uint32_t binIndex = GetBinIndex(sizeRequired); 
					 binIndex < kBinCount; 
					 binIndex++)
		{
			for(Node* pNodeCurrent  =  mFreeListArray[binIndex].mpListNext; 
					  pNodeCurrent != &mFreeListArray[binIndex]; 
					  pNodeCurrent  =  pNodeCurrent->mpListNext)
			{
				PPM_ASSERT(this, binIndex == GetBinIndex(pNodeCurrent->GetSize()), kGAErrorInternal, kGAErrorText[kGAErrorInternal], pNodeCurrent, pNodeCurrent);
				PPM_ASSERT(this, pNodeCurrent->IsInUse() == 0, kGAErrorInternal, kGAErrorText[kGAErrorInternal], pNodeCurrent, pNodeCurrent);

				const size_type blockSize = pNodeCurrent->GetSize();

				if(sizeRequired <= blockSize) // If pNodeCurrent can accomodate the user's request...
				{
					// It turns out that we can ignore kAllocationFlagBestFit, as the node list is 
					// sorted from smallest to largest size and the user always gets best-fit.

					pNodeCurrent->RemoveFromFreeList();
					pNodeCurrent->SetIsInUse(true);
					pNodeCurrent->mpData = pNodeCurrent->mpBlock;

					PPM_ASSERT(this, pNodeCurrent->mpData + sizeRequired <= pNodeCurrent->mpBlock + blockSize, kGAErrorInternal, kGAErrorText[kGAErrorInternal], pNodeCurrent, pNodeCurrent);

					const size_type trailingSpace = (blockSize - sizeRequired);

					if(trailingSpace >= mMinSplitSize)  // If we have enough to split this block...
					{
						Node* const pNodeNext = AllocateNode();

						if(pNodeNext)
						{
							pNodeCurrent->SplitNext(sizeRequired, pNodeNext);
							pNodeNext->SetIsInUse(false);
							AddNodeToFreeList(pNodeNext);
						} // Else we are out of nodes but can still survive by not splitting the current node.

					} // Else we use the block as-is without splitting it.

					PPM_ASSERT(this, pNodeCurrent->GetSize() >= kMinAllocSize, kGAErrorInternal, kGAErrorText[kGAErrorInternal], pNodeCurrent, pNodeCurrent);
					PPM_ASSERT(this, PPM_IsAligned(pNodeCurrent->mpBlock, kMinAlignment), kGAErrorInternal, kGAErrorText[kGAErrorInternal], pNodeCurrent, pNodeCurrent);

					return pNodeCurrent;
				}
			}
		}
	}
	else
	{
		// If kAllocationFlagPerm is specified, then we need to do a different strategy whereby
		// we scan memory instead of scan free lists. That will be slower, but we are talking about
		// memory that is semi-permanently allocated and so this shouldn't be happening much.
		// For each core block, starting with the last and moving backwards; for each node in that
		// core block, starting with the last and moving backwards; if it works then use it.

		for(CoreBlock* pCoreBlock  = (CoreBlock*)mHeadCoreBlock.mpListPrev; 
					   pCoreBlock != &mHeadCoreBlock; 
					   pCoreBlock  = (CoreBlock*)pCoreBlock->mpListPrev)
		{
			for(Node* pNodeCurrent  = pCoreBlock->mpMemPrev; 
					  pNodeCurrent != pCoreBlock; 
					  pNodeCurrent  = pNodeCurrent->mpMemPrev)
			{
				if(!pNodeCurrent->IsInUse())
				{
					// To do: The code below is the same as equivalent code above. Once we have 
					// confidence that it will stay equivalent, remove the duplication.
					const size_type blockSize = pNodeCurrent->GetSize();

					if(sizeRequired <= blockSize) // If pNodeCurrent can accomodate the user's request...
					{
						// It turns out that we can ignore kAllocationFlagBestFit, as the node list is 
						// sorted from smallest to largest size and the user always gets best-fit.

						// Since we are allocating high, we want to split off any space below pNodeCurrent and
						// not above pNodeCurrent.

						pNodeCurrent->RemoveFromFreeList();
						pNodeCurrent->SetIsInUse(true);
						pNodeCurrent->mpData = pNodeCurrent->mpBlock + blockSize - sizeRequired;

						// Possibly split off leading memory.
						const size_type leadingSpace = (size_type)(pNodeCurrent->mpData - pNodeCurrent->mpBlock);
						PPM_ASSERT(this, leadingSpace <= pNodeCurrent->GetSize(), kGAErrorInternal, kGAErrorText[kGAErrorInternal], pNodeCurrent, pNodeCurrent);
						PPM_ASSERT(this, PPM_IsAligned(leadingSpace, kMinAlignment), kGAErrorInternal, kGAErrorText[kGAErrorInternal], pNodeCurrent, pNodeCurrent);

						if(leadingSpace >= mMinSplitSize) // If we have enough to split this block...
						{
							Node* const pNodePrev = AllocateNode();

							if(pNodePrev)
							{
								pNodeCurrent->SplitPrev(leadingSpace, pNodePrev);
								pNodePrev->SetIsInUse(false);
								AddNodeToFreeList(pNodePrev);
							} // Else we are out of nodes but can still survive by not splitting the current node.
						} // Else we use the block as-is without splitting it.

						PPM_ASSERT(this, pNodeCurrent->GetSize() >= kMinAllocSize, kGAErrorInternal, kGAErrorText[kGAErrorInternal], pNodeCurrent, pNodeCurrent);
						PPM_ASSERT(this, PPM_IsAligned(pNodeCurrent->mpBlock, kMinAlignment), kGAErrorInternal, kGAErrorText[kGAErrorInternal], pNodeCurrent, pNodeCurrent);

						return pNodeCurrent;
					}
				}
			}
		}
	}

	// There was no free memory. 
	if(mpMallocFailureFunction && (++nMallocFailureCount < mnMaxMallocFailureCount))
	{
		#if PPM_THREAD_SAFETY_SUPPORTED
			int i, nSavedLockCount = PPMMutexGetLockCount(mpMutex); // We have the mutex locked, so this is safe to call.
			for(i = 0; i < nSavedLockCount; i++)
				PPMMutexUnlock(mpMutex);
		#endif

		const bool bResult = mpMallocFailureFunction(this, nSize, sizeRequired, mpMallocFailureFunctionContext);

		#if PPM_THREAD_SAFETY_SUPPORTED
			for(i = 0; i < nSavedLockCount; i++)
				PPMMutexLock(mpMutex);
		#endif

		if(bResult)
		{
			nAllocationFlags = nAllocationFlagsSaved;
			goto MallocTop;
		}
	}

	// This function has failed to allocate memory after all attempts.
	#if PPM_ASSERT_ENABLED
		if(mbMallocFailureAssertEnabled)
			PPM_ASSERT(this, false, kGAErrorMemoryExhaustion, kGAErrorText[kGAErrorMemoryExhaustion], NULL, NULL);
	#endif

	return NULL; // Note that we don't return mFailureValue, as this return value is the Node and not the user pointer.
}


uint32_t NonLocalAllocator::GetBinIndexAndSize(size_type blockSize, size_type& binSize)
{
	/* graham: The general idea here is to continue using log2 binning for sizes < 64
			   and >= 2048, but then use explicit size ranges for other sizes to get
			   better binning distribution and take advantage of our large (40) bin
			   range. The non local allocator has bins sorted by sizes, and free lists
			   in each bin sorted by size. Therefore, as an additional optimization,
			   sizes are either rounded up to the nearest pow2 or range upper bound to
			   drastically reduce the # of unique allocations per bin, which is where
			   a large amount of performance cost comes from (not just related to # of allocs).
			   If all blocks in a bin are the same size then it never needs to walk
			   through the bin; it can just return the first bin element right away.
			   With such fine granularity per bin, rounding up to the range upper bound
			   does not waste too much additional memory. If alignment padding becomes
			   an issue here (doubtful though), the pow2 rounded for sizes >= 2048
			   could be removed.
	*/

	// GW-TODO: This should be generalized and have the NLA algorithm optimized when launch
	//          titles have settled. We should pursue future optimizations where either we have
	//          a tree of free blocks, or make NLA store pointers into the bin for elements
	//          of the same size. Storing pointers into the bins which indicate sub-sizes
	//          would make finding blocks in bins much faster. O(n) to ~0(1)
	size_type binIndex;

	if (blockSize < 64) // Bin 0
	{
		binIndex = 0;

		// Small sized allocations on pow2 increments make the NLA function
		// effectively as a small block allocator.
		binSize = PPM_AlignUp(blockSize, 16);
	}
	else if (blockSize > 2048) // Bin 29..39
	{
		binIndex = PPM_Log2(blockSize);

		if (PPM_IsAligned(blockSize, binIndex + 2))
			binSize = blockSize;
		else
			binSize = PPM_AlignUp(blockSize, binIndex + 3);
		
		// offset bin index account for explicit non-log2 range
		// sizes > 2048 would have started at bin11, but now need
		// to shift into bin29
		binIndex += 18;
	}
	else // Bin 1..28
	{
		// These ranges were chosen via extensive profiling
		// of production Warsaw levels.
		const uint32_t rangeCount = 29;
		static size_type blockRanges[rangeCount + 1] =
		{
			2048, 1920, 1792, 1664, 1536,
			1408, 1280, 1152, 1024,  960,
			 896,  832,  768,  704,  640,
			 576,  512,  448,  384,  320,
			 256,  192,  128,  112,   96,
			  88,   80,   72,   64
		};

		binIndex = rangeCount - 1;
		binSize  = blockSize;

		for (uint32_t index = 1; index < rangeCount; ++index)
		{
			const size_type blockRange = blockRanges[index];
			if (blockSize >= blockRange)
			{
				// Allocation request fits within this block range.
				binSize = blockRanges[index - 1]; // round to upper bound of range
				break;
			}

			--binIndex;
		}
	}

	// Account for bin overflow (large allocations)
	if (binIndex >= kBinCount)
		binIndex = (kBinCount - 1);

	return static_cast<uint32_t>(binIndex); // Cast down to uint32_t on 64 bit systems is OK because binIndex is never a large number.
}

// We locate this function close to Malloc in order to be more friendly with the instruction cache.
uint32_t NonLocalAllocator::GetBinIndex(size_type blockSize)
{
#if PPM_NLA_64BIT_BIN_COUNTS_ENABLED
	size_type binSize;
	size_type binIndex = GetBinIndexAndSize(blockSize, binSize);
#else
	// Map 4, 8, 16, 32, 64, 128, 256, 512, 1024, 2048, 4096, 8192, 16384, 32768, 65536, 131072, 262144, etc.
	//   to bins:
	//      0,   1,   2,   3,    4,    5,    6,    7,     8,     9,    10,     11,     12, etc.
	size_type binIndex = PPM_Log2(blockSize);

	if(EA_UNLIKELY(binIndex < kFirstBinPower))
		binIndex = kFirstBinPower;

	binIndex -= kFirstBinPower;

	if(binIndex >= kBinCount)
		binIndex = (kBinCount - 1);
#endif

	return static_cast<uint32_t>(binIndex); // Cast down to uint32_t on 64 bit systems is OK because binIndex is never a large number.
}


// We locate this function close to Malloc in order to be more friendly with the instruction cache.
void NonLocalAllocator::HashTable::Insert(Node* pNode)
{
	if(EA_UNLIKELY((4 * (mnElementCount + 1)) >= (2 * mnBucketCount))) // If we should expand the table...
	{
		size_t nNewBucketCount = (2 * mnBucketCount) + 1;

		if(nNewBucketCount < mnBucketCountMin)
		   nNewBucketCount = mnBucketCountMin;

		ReHash(nNewBucketCount); // Ignore the return value of ReHash. If re-hashing fails then we just use the previous less optimal structure.
	}

	const size_t h = Hash(pNode->mpData);

	pNode->mpListNext = mpBucketArray[h];
	mpBucketArray[h]  = pNode;
	mnElementCount++;
}

void* NonLocalAllocator::Split(void* p, size_t nOffset)
{
#if PPM_NULL_POINTER_FREE_ENABLED
	if (p != mFailureValue)
	{
#endif
		#if PPM_THREAD_SAFETY_SUPPORTED
			PPMAutoMutex autoMutex(mpMutex);
		#endif
		
		Node* pNode = mHashTable.Find(p);
		PPM_ASSERT(this, pNode, kGAErrorBadUserPtr, kGAErrorText[kGAErrorBadUserPtr], p, p);
		PPM_ASSERT(this, pNode->IsInUse(), kGAErrorInternal, kGAErrorText[kGAErrorInternal], pNode, pNode);

		if (pNode)
		{
			// Split the node.
			Node* pNewNode = AllocateNode();
			size_t actualOffset = nOffset + (uintptr_t(pNode->mpData) - uintptr_t(pNode->mpBlock));
			pNode->SplitNext(actualOffset, pNewNode);
			pNewNode->SetIsInUse(true);
			pNewNode->mpData = pNewNode->mpBlock;
			mHashTable.Insert(pNewNode);

			return pNewNode->mpData;
		}

#if PPM_NULL_POINTER_FREE_ENABLED
	} // if(p)
#endif
	return mFailureValue;
}


// We locate this function close to Malloc in order to be more friendly with the instruction cache.
void NonLocalAllocator::Free(void* pData)
{
	#if PPM_NULL_POINTER_FREE_ENABLED
	if(pData != mFailureValue){
	#endif
		{
			// It's important to have a scope here so that the autoMutex unlocks before 
			// exiting the scope. We use an auto-mutex so that any exceptions might occur 
			// while processing will trigger the mutex unlock via stack unwinding.

			#if PPM_NLA_HOOKS_SUPPORTED
				if(mpHookFunction)
				{
					const HookInfo hookInfo = { this, true, kHookTypeFree, kHookSubTypeNone, 0, pData, 0, 0, 0, 0, 0, NULL, NULL, 0 };
					mpHookFunction(&hookInfo, mpHookFunctionContext);
				}
			#endif

			#if PPM_THREAD_SAFETY_SUPPORTED
				PPMAutoMutex autoMutex(mpMutex);
			#endif

			#if PPM_AUTO_HEAP_VALIDATION_SUPPORTED
				if(EA_UNLIKELY(mAutoHeapValidationLevel && (++mnAutoHeapValidationEventCount >= mnAutoHeapValidationFrequency)))
				{
					mnAutoHeapValidationEventCount = 0;
					ValidateHeap(mAutoHeapValidationLevel);
				}
			#endif

			#if PPM_NLA_METRICS_SUPPORTED
				Node* pNode = mHashTable.Find(pData);
				PPM_ASSERT(this, pNode, kGAErrorBadUserPtr, kGAErrorText[kGAErrorBadUserPtr], pData, pData);

				if(pNode)
				{
					CollectMetrics(pNode, false);
					mHashTable.Remove(pNode);
				}
			#else
				Node* pNode = mHashTable.Remove(pData);
				PPM_ASSERT(this, pNode, kGAErrorBadUserPtr, kGAErrorText[kGAErrorBadUserPtr], pData, pData);
			#endif

			if(pNode)
			{
				#if PPM_NLA_DEBUG_DATA_SUPPORTED
					if(pNode->mpDebugData)
					{
						DebugDataLengthType blockSize;

						memcpy(&blockSize, pNode->mpDebugData, sizeof(blockSize));
						mpLocalDebugFreeFunction(this, pNode->mpDebugData, blockSize, mpLocalAllocationContext);
						pNode->mpDebugData = NULL;
					}
				#endif

				#if PPM_NLA_DEBUG_DATA_SUPPORTED
					VerifyGuardFill(pNode);

					if(mbEnableDebugWrites && mpMemsetFunction)
						mpMemsetFunction(pNode->mpBlock, mnFillFree, pNode->GetSize());
				#endif

				pNode->SetIsInUse(false);

				if(!pNode->mpMemNext->IsInUse()) // To consider: Put this if/else into the CoalesceWithNext function.
				{
					Node* const pNodeNext = pNode->CoalesceWithNext(pNode->mpMemNext);
					pNodeNext->RemoveFromFreeList();
					DeallocateNode(pNodeNext);
				}

				if(!pNode->mpMemPrev->IsInUse())
				{
					pNode = pNode->mpMemPrev;
					pNode->RemoveFromFreeList();  // Remove pNode from free list because upon coalescing it will be moved within the free list.

					Node* const pNodeNext = pNode->CoalesceWithNext(pNode->mpMemNext);
					//pNodeNext->RemoveFromFreeList(); // Don't do this because pNodeNext is the node the user is freeing and it is not in any free list.
					DeallocateNode(pNodeNext);
				}

				AddNodeToFreeList(pNode);
			}
		}

		#if PPM_NLA_HOOKS_SUPPORTED
			if(mpHookFunction)
			{
				const HookInfo hookInfo = { this, false, kHookTypeFree, kHookSubTypeNone, 0, pData, 0, 0, 0, 0, 0, NULL, NULL, 0 };
				mpHookFunction(&hookInfo, mpHookFunctionContext);
			}
		#endif

	#if PPM_NULL_POINTER_FREE_ENABLED
	} // if(pData)
	#endif
}


// We locate this function close to Malloc in order to be more friendly with the instruction cache.
NonLocalAllocator::Node* NonLocalAllocator::HashTable::Remove(const void* pData)
{
	if(mpBucketArray)
	{
		const size_t h = Hash(pData);

		for(Node* pNode = mpBucketArray[h], *pPrev = NULL; 
				  pNode;
				  pNode = pNode->mpListNext)
		{
			if(pNode->mpData == pData) // If we found our node...
			{
				if(pPrev)
					pPrev->mpListNext = pNode->mpListNext;
				else
					mpBucketArray[h] = pNode->mpListNext;

				mnElementCount--;

				return pNode;
			}

			pPrev = pNode;
		}
	}

	return NULL; // Note that we don't return mFailureValue, as this return value is the Node and not the user pointer.
}


// We locate this function close to Malloc in order to be more friendly with the instruction cache.
void NonLocalAllocator::AddNodeToFreeList(Node* pNode)
{
	PPM_ASSERT(this, pNode->IsInUse() == 0, kGAErrorCorruptedChunk, kGAErrorText[kGAErrorCorruptedChunk], pNode, pNode);
	#if PPM_NLA_DEBUG_DATA_SUPPORTED
		PPM_ASSERT(this, pNode->mpDebugData == NULL, kGAErrorCorruptedChunk, kGAErrorText[kGAErrorCorruptedChunk], pNode, pNode);
	#endif

	const size_type blockSize = pNode->GetSize();
	const uint32_t  binIndex  = GetBinIndex(blockSize);

	Node* pNodeCurrent;
	Node* pNodePrev;

	for(pNodeCurrent  = mFreeListArray[binIndex].mpListNext, pNodePrev = &mFreeListArray[binIndex]; 
		pNodeCurrent != &mFreeListArray[binIndex] && (blockSize > pNodeCurrent->GetSize());  // We use > here instead of >=
		pNodeCurrent  = pNodeCurrent->mpListNext)
	{
		pNodePrev = pNodeCurrent;
	}

	pNode->InsertIntoFreeList(pNodePrev);
}


void* NonLocalAllocator::MallocAligned(size_t nSize, size_t nAlignment, size_t nAlignmentOffset, int nAllocationFlags)
{
	PPM_ASSERT(this, nSize < kMaxAllocSize,                             kGAErrorRequestedSizeTooLarge,     kGAErrorText[kGAErrorRequestedSizeTooLarge],     &nSize,      &nAlignment);       // If this fails, it is probably the user's fault. The allocation size is too big.
	PPM_ASSERT(this, (nAlignment == 0) || PPM_IsPowerOf2(nAlignment),   kGAErrorRequestedAlignmentInvalid, kGAErrorText[kGAErrorRequestedAlignmentInvalid], &nAlignment, &nAlignmentOffset);
	PPM_ASSERT(this, nAlignment <= 0x10000000,                          kGAErrorRequestedAlignmentInvalid, kGAErrorText[kGAErrorRequestedAlignmentInvalid], &nAlignment, &nAlignmentOffset);
	PPM_ASSERT(this, (nAlignmentOffset % kMinAlignment) == 0,           kGAErrorRequestedAlignmentInvalid, kGAErrorText[kGAErrorRequestedAlignmentInvalid], &nAlignment, &nAlignmentOffset); // If this fails, the user is at fault. See the documented specifications for MallocAligned.
	PPM_ASSERT(this, nAlignmentOffset <= 0x10000000,                    kGAErrorRequestedAlignmentInvalid, kGAErrorText[kGAErrorRequestedAlignmentInvalid], &nAlignment, &nAlignmentOffset);

	if((nAlignment > kMinAlignment) || nAlignmentOffset) // If we really need to use an aligned allocation...
	{
		void* pResult = mFailureValue;

		#if PPM_THREAD_SAFETY_SUPPORTED
			PPMAutoMutex autoMutex(mpMutex);
		#endif

		#if PPM_AUTO_HEAP_VALIDATION_SUPPORTED
			if(EA_UNLIKELY(mAutoHeapValidationLevel && (++mnAutoHeapValidationEventCount >= mnAutoHeapValidationFrequency)))
			{
				mnAutoHeapValidationEventCount = 0;
				ValidateHeap(mAutoHeapValidationLevel);
			}
		#endif

		#if PPM_NLA_DEBUG_DATA_SUPPORTED
			GetDebugInfoForAllocation(mDebugDataInfo, nSize, nAllocationFlags, nAlignment, nAlignmentOffset);
			PPM_ASSERT(this, nSize < (kMaxAllocSize - mDebugDataInfo.mnGuardFillSize), kGAErrorRequestedSizeTooLarge, kGAErrorText[kGAErrorRequestedSizeTooLarge], NULL, NULL); // If this fails, it is probably the user's fault. The allocation size is too big.
			nSize += mDebugDataInfo.mnGuardFillSize;
		#endif

		#if PPM_NLA_HOOKS_SUPPORTED
			if(mpHookFunction)
			{
				// Don't lock the critical section during this call, as we are calling external  
				// code and mutex locks during external calls is risky for deadlocks.
				const HookInfo hookInfo = { this, true, kHookTypeMalloc, kHookSubTypeMallocAligned, nSize, NULL, 0, nSize, 0, nAlignment, nAllocationFlags, NULL, NULL, nAlignmentOffset };
				mpHookFunction(&hookInfo, mpHookFunctionContext);
			}
		#endif

		if(nSize < kMaxAllocSize)
		{
			Node* const pNode = MallocNodeAligned(nSize, nAlignment, nAlignmentOffset, nAllocationFlags);

			if(pNode)
			{
				#if PPM_NLA_DEBUG_DATA_SUPPORTED
					bool fillIsOK = VerifyFreeFill(pNode);
					AttachDebugDataToNode(mDebugDataInfo, pNode, fillIsOK);
				#endif

				mHashTable.Insert(pNode);

				#if PPM_NLA_METRICS_SUPPORTED
					CollectMetrics(pNode, true);
				#endif

				pResult = pNode->mpData;
			}
		}

		#if PPM_NLA_HOOKS_SUPPORTED
			if(mpHookFunction)
			{
				const HookInfo hookInfo = { this, false, kHookTypeMalloc, kHookSubTypeMallocAligned, nSize, NULL, 0, nSize, 0, nAlignment, nAllocationFlags, pResult, NULL, nAlignmentOffset };
				mpHookFunction(&hookInfo, mpHookFunctionContext);
			}
		#endif

		return pResult;
	}

	return Malloc(nSize, nAllocationFlags);
}


NonLocalAllocator::Node* NonLocalAllocator::MallocNodeAligned(size_t nSize, size_t nAlignment, size_t nAlignmentOffset, int nAllocationFlags)
{
	uint32_t  nMallocFailureCount   = 0;
	int       nAllocationFlagsSaved = nAllocationFlags;
	size_type sizeRequired;

	// We have some retries below where we retry with a different sizeRequired value. We do this because there's a 
	// fragmentation tradeoff below. The user can give a hint via kAllocationFlagBestFit, but if that doesn't work then 
	// it's possible that the allocation could succeed without it and so we need to try that too. The reverse is the case
	// too: if the user doesn't specify kAllocationFlagBestFit then we could fail to find an allocation that would be 
	// found with kAllocationFlagBestFit specified.
	bool bBestFitTried    = ((nAllocationFlags & kAllocationFlagBestFit) != 0);  // Have we tried with kAllocationFlagBestFit?
	bool bNonBestFitTried = ((nAllocationFlags & kAllocationFlagBestFit) == 0);  // Have we tried without kAllocationFlagBestFit?

	if(nSize >= kMinAllocSize)
		nSize = PPM_AlignUp(nSize, kMinAlignment);
	else
		nSize = kMinAllocSize;

	if(nAlignment < kMinAllocSize)
		nAlignment = kMinAllocSize;

	MallocTop:

	if((nAllocationFlags & kAllocationFlagPerm) == 0)
	{
		// If kAllocationFlagBestFit is specified, we look for a 
		// perfect alignment match (and not a perfect size match).

		if(nAllocationFlags & kAllocationFlagBestFit)
		{
			// In this case we're going to look for a block that is already aligned as-is.
			sizeRequired = (size_type)nSize;    // In this case we will be possibly checking a smaller sized bin that we would be with the code below and its additions.
		}
		else
		{
			// sizeRequired is the largest possible required space needed for a block that has 
			// the required alignment and alignment offset. There is an unusual case that can occur
			// whereby there is a free block that can satisfy the alignment requirements but is smaller
			// than sizeRequired. For efficiency reasons the code below rejects such blocks. This case
			// doesn't occur much in practice because typically you see it only with large alignment 
			// or alignment offset values relative to size values. We can possibly add logic to avoid
			// this situation, but it would likely slow down the algorithm for an unusual case. We can 
			// deal with this by having sizeRequired equal nSize and test each block below to see if it
			// can support the alignment requirements (possibly after being split).
			sizeRequired = (size_type)PPM_AlignUp(nSize + nAlignment + nAlignmentOffset, kMinAlignment);  // This is the most we could need to satisfy size, alignment, offset.
		}

		if(sizeRequired >= nSize) // If the required size increment above didn't trigger integer wraparound...
		{
			for(uint32_t binIndex = GetBinIndex(sizeRequired); 
						 binIndex < kBinCount;
						 binIndex++)
			{
				for(Node* pNodeCurrent  =  mFreeListArray[binIndex].mpListNext; 
						  pNodeCurrent != &mFreeListArray[binIndex]; 
						  pNodeCurrent  =  pNodeCurrent->mpListNext)
				{
					const size_type blockSize = pNodeCurrent->GetSize();

					if(sizeRequired <= blockSize) // If the size is right...
					{
						// If the user didn't specify best fit or if the alignment is right, use the node.
						if(((nAllocationFlags & kAllocationFlagBestFit) == 0) ||                    // In this case we over-allocated and it should always be possible to align within the node.
							PPM_IsAligned(pNodeCurrent->mpBlock + nAlignmentOffset, nAlignment))    // In this case we are looking for an exact match and so the node must have the desired alignment as the node is.
						{
							// To consider: Merge this code with the similar code below for kAllocationFlagPerm.

							// We will use pNodeCurrent. However, we may need to split off leading or trailing memory.
							PPM_ASSERT(this, PPM_IsAligned(pNodeCurrent->mpBlock, kMinAlignment), kGAErrorInternal, kGAErrorText[kGAErrorInternal], pNodeCurrent, pNodeCurrent);

							// Setup pNodeCurrent
							pNodeCurrent->RemoveFromFreeList();
							pNodeCurrent->SetIsInUse(true);
							pNodeCurrent->mpData = (char*)((intptr_t)(pNodeCurrent->mpBlock + nAlignmentOffset + (nAlignment - 1)) & -(intptr_t)nAlignment) - nAlignmentOffset;

							PPM_ASSERT(this, pNodeCurrent->mpData >= pNodeCurrent->mpBlock, kGAErrorInternal,   kGAErrorText[kGAErrorInternal], pNodeCurrent, pNodeCurrent);
							PPM_ASSERT(this, pNodeCurrent->mpData + nSize <= pNodeCurrent->mpBlock + blockSize, kGAErrorInternal, kGAErrorText[kGAErrorInternal], pNodeCurrent, pNodeCurrent);
							PPM_ASSERT(this, PPM_IsAligned(pNodeCurrent->mpData + nAlignmentOffset, nAlignment), kGAErrorInternal, kGAErrorText[kGAErrorInternal], pNodeCurrent, pNodeCurrent);

							// Possibly split off trailing memory.
							const size_type trailingSpace = (size_type)((pNodeCurrent->mpBlock + blockSize) - (pNodeCurrent->mpData + nSize));
							PPM_ASSERT(this, trailingSpace <= blockSize, kGAErrorInternal, kGAErrorText[kGAErrorInternal], pNodeCurrent, pNodeCurrent);

							if(trailingSpace >= mMinSplitSize)  // If we have enough to split this block...
							{
								Node* const pNodeNext = AllocateNode();

								if(pNodeNext)
								{
									pNodeCurrent->SplitNext(blockSize - trailingSpace, pNodeNext);
									pNodeNext->SetIsInUse(false);
									AddNodeToFreeList(pNodeNext);
								} // Else we are out of nodes but can still survive by not splitting the current node.
							}

							// Possibly split off leading memory.
							const size_type leadingSpace = (size_type)(pNodeCurrent->mpData - pNodeCurrent->mpBlock);
							PPM_ASSERT(this, leadingSpace <= pNodeCurrent->GetSize(), kGAErrorInternal, kGAErrorText[kGAErrorInternal], pNodeCurrent, pNodeCurrent);
							PPM_ASSERT(this, PPM_IsAligned(leadingSpace, kMinAlignment), kGAErrorInternal, kGAErrorText[kGAErrorInternal], pNodeCurrent, pNodeCurrent);

							if(leadingSpace >= mMinSplitSize) // If we have enough to split this block...
							{
								Node* const pNodePrev = AllocateNode();

								if(pNodePrev)
								{
									pNodeCurrent->SplitPrev(leadingSpace, pNodePrev);
									pNodePrev->SetIsInUse(false);
									AddNodeToFreeList(pNodePrev);
								} // Else we are out of nodes but can still survive by not splitting the current node.
							}

							PPM_ASSERT(this, pNodeCurrent->GetSize() >= kMinAllocSize, kGAErrorInternal, kGAErrorText[kGAErrorInternal], pNodeCurrent, pNodeCurrent);
							PPM_ASSERT(this, PPM_IsAligned(pNodeCurrent->mpData + nAlignmentOffset, nAlignment), kGAErrorInternal, kGAErrorText[kGAErrorInternal], pNodeCurrent, pNodeCurrent);

							return pNodeCurrent;
						}
					}
				}
			}
		}

		// In this case, the allocation attempt has failed. Usually we won't get here, as the heap will have space to fund the allocation.
		if((nAllocationFlags & kAllocationFlagBestFit) && !bNonBestFitTried)
		{
			// Try again but with best-fit disabled.
			nAllocationFlags &= ~kAllocationFlagBestFit;
			bNonBestFitTried = true;
			goto MallocTop;
		}
		else if(((nAllocationFlags & kAllocationFlagBestFit) == 0) && !bBestFitTried)
		{
			// Try again but with best-fit enabled.
			nAllocationFlags |= kAllocationFlagBestFit;
			bBestFitTried = true;
			goto MallocTop;
		}
		else
		{
			// It's possible that the user is asking for a block that is exactly the size of the free memory,
			// which can occur in particular if the user created this heap for the purpose of serving a single allocation.
			const uint32_t  binIndex  = GetBinIndex(sizeRequired); 
			Node* const     pNode     = mFreeListArray[binIndex].mpListNext; 
			const size_type blockSize = pNode->GetSize();

			if((nSize <= blockSize) && PPM_IsAligned(pNode->mpBlock + nAlignmentOffset, nAlignment))
			{
				nAllocationFlags = kAllocationFlagBestFit;
				goto MallocTop; // We know that this will succeed.
			}
		}
	}
	else
	{
		// If kAllocationFlagPerm is specified, then we need to do a different strategy whereby
		// we scan memory instead of scan free lists. That will be slower, but we are talking about
		// memory that is semi-permanently allocated and so this shouldn't be happening much.
		// For each core block, starting with the last and moving backwards; for each node in that
		// core block, starting with the last and moving backwards; if it works then use it.

		sizeRequired = (size_type)PPM_AlignUp(nSize + nAlignment + nAlignmentOffset, kMinAlignment);  // This is the most we could need to satisfy size, alignment, offset.

		for(CoreBlock* pCoreBlock  = (CoreBlock*)mHeadCoreBlock.mpListPrev; 
					   pCoreBlock != &mHeadCoreBlock; 
					   pCoreBlock  = (CoreBlock*)pCoreBlock->mpListPrev)
		{
			for(Node* pNodeCurrent  = pCoreBlock->mpMemPrev; 
					  pNodeCurrent != pCoreBlock; 
					  pNodeCurrent  = pNodeCurrent->mpMemPrev)
			{
				if(!pNodeCurrent->IsInUse())
				{
					const size_type blockSize = pNodeCurrent->GetSize();

					if(sizeRequired <= blockSize) // If pNodeCurrent can accomodate the user's request...
					{
						// Since we are allocating high, we want to split off any space below pNodeCurrent and
						// not above pNodeCurrent. Thus we set pNodeCurrent->mpData to be at the end of the 
						// block instead of at the beginning.

						pNodeCurrent->RemoveFromFreeList();
						pNodeCurrent->SetIsInUse(true);

						// We don't currently have a single formula that picks the best position for mpData.
						pNodeCurrent->mpData = (char*)((intptr_t)(pNodeCurrent->mpBlock + blockSize - nSize + nAlignmentOffset + (nAlignment - 1)) & -(intptr_t)nAlignment) - nAlignmentOffset;

						if((pNodeCurrent->mpData + nSize) > (pNodeCurrent->mpBlock + blockSize))
							pNodeCurrent->mpData = (char*)((intptr_t)(pNodeCurrent->mpBlock + blockSize - sizeRequired + nAlignmentOffset + (nAlignment - 1)) & -(intptr_t)nAlignment) - nAlignmentOffset;

						PPM_ASSERT(this, pNodeCurrent->mpData >= pNodeCurrent->mpBlock, kGAErrorInternal,   kGAErrorText[kGAErrorInternal], pNodeCurrent, pNodeCurrent);
						PPM_ASSERT(this, pNodeCurrent->mpData + nSize <= pNodeCurrent->mpBlock + blockSize, kGAErrorInternal, kGAErrorText[kGAErrorInternal], pNodeCurrent, pNodeCurrent);
						PPM_ASSERT(this, PPM_IsAligned(pNodeCurrent->mpData + nAlignmentOffset, nAlignment), kGAErrorInternal, kGAErrorText[kGAErrorInternal], pNodeCurrent, pNodeCurrent);

						// Possibly split off trailing memory.
						const size_type trailingSpace = (size_type)((pNodeCurrent->mpBlock + blockSize) - (pNodeCurrent->mpData + nSize));
						PPM_ASSERT(this, trailingSpace <= blockSize, kGAErrorInternal, kGAErrorText[kGAErrorInternal], pNodeCurrent, pNodeCurrent);

						if(trailingSpace >= mMinSplitSize)  // If we have enough to split this block...
						{
							Node* const pNodeNext = AllocateNode();

							if(pNodeNext)
							{
								pNodeCurrent->SplitNext(blockSize - trailingSpace, pNodeNext);
								pNodeNext->SetIsInUse(false);
								AddNodeToFreeList(pNodeNext);
							} // Else we are out of nodes but can still survive by not splitting the current node.
						}

						// Possibly split off leading memory.
						const size_type leadingSpace = (size_type)(pNodeCurrent->mpData - pNodeCurrent->mpBlock);
						PPM_ASSERT(this, leadingSpace <= pNodeCurrent->GetSize(), kGAErrorInternal, kGAErrorText[kGAErrorInternal], pNodeCurrent, pNodeCurrent);
						PPM_ASSERT(this, PPM_IsAligned(leadingSpace, kMinAlignment), kGAErrorInternal, kGAErrorText[kGAErrorInternal], pNodeCurrent, pNodeCurrent);

						if(leadingSpace >= mMinSplitSize) // If we have enough to split this block...
						{
							Node* const pNodePrev = AllocateNode();

							if(pNodePrev)
							{
								pNodeCurrent->SplitPrev(leadingSpace, pNodePrev);
								pNodePrev->SetIsInUse(false);
								AddNodeToFreeList(pNodePrev);
							} // Else we are out of nodes but can still survive by not splitting the current node.
						}

						PPM_ASSERT(this, pNodeCurrent->GetSize() >= kMinAllocSize, kGAErrorInternal, kGAErrorText[kGAErrorInternal], pNodeCurrent, pNodeCurrent);
						PPM_ASSERT(this, PPM_IsAligned(pNodeCurrent->mpData + nAlignmentOffset, nAlignment), kGAErrorInternal, kGAErrorText[kGAErrorInternal], pNodeCurrent, pNodeCurrent);

						return pNodeCurrent;
					}
				}
			}
		}

		// Try again but with Perm disabled and BestFit enabled. If that fails then we are exhausted of sufficient memory.
		nAllocationFlags &= ~kAllocationFlagPerm;
		nAllocationFlags |=  kAllocationFlagBestFit;
		goto MallocTop;
	}

	// There was no free memory. 
	// If the user has installed a callback function and if we haven't called it too many times and if the size didn't have integer wraparound...
	if(mpMallocFailureFunction && (++nMallocFailureCount < mnMaxMallocFailureCount) && (sizeRequired >= nSize))
	{
		#if PPM_THREAD_SAFETY_SUPPORTED
			int i, nSavedLockCount = PPMMutexGetLockCount(mpMutex); // We have the mutex locked, so this is safe to call.
			for(i = 0; i < nSavedLockCount; i++)
				PPMMutexUnlock(mpMutex);
		#endif

		const bool bResult = mpMallocFailureFunction(this, nSize, sizeRequired, mpMallocFailureFunctionContext);

		#if PPM_THREAD_SAFETY_SUPPORTED
			for(i = 0; i < nSavedLockCount; i++)
				PPMMutexLock(mpMutex);
		#endif

		if(bResult)
		{
			nAllocationFlags = nAllocationFlagsSaved;
			goto MallocTop;
		}
	}

	// This function has failed to allocate memory after all attempts.
	#if PPM_ASSERT_ENABLED
		if(mbMallocFailureAssertEnabled)
			PPM_ASSERT(this, false, kGAErrorMemoryExhaustion, kGAErrorText[kGAErrorMemoryExhaustion], NULL, NULL);
	#endif

	return NULL; // Note that we don't return mFailureValue, as this return value is the Node and not the user pointer.
}


void** NonLocalAllocator::MallocMultiple(size_t count, const size_t* pSizeArray, const size_t* pAlignmentArray, 
										 const size_t* pAlignmentOffsetArray, void* pResultArray[], int nAllocationFlags)
{
	// We have two approaches: One approach is to allocate all requested memory contiguously.
	// The other approach is to allocate each item individually. The latter can be used as 
	// a fallback mechanism where the former fails. Doing the first approach in the most 
	// optimally packed manner is not trivial, as different rearrangement of returned blocks
	// can result in better packing. We'll use kAllocationFlagContig to control strategy.

	size_t i;

	for(i = 0; i < count; i++)
	{
		pResultArray[i] = MallocAligned(pSizeArray[i], pAlignmentArray[i], pAlignmentOffsetArray[i], nAllocationFlags);

		if(EA_UNLIKELY(pResultArray[i] == mFailureValue))
			break;
	}

	if(i == count) // If all requests could be satisfied...
		return pResultArray;

	for(size_t j = 0; j < i; j++)
		Free(pResultArray[j]);

	return NULL; // Note that we don't return mFailureValue, as this return value is pResultArray and not an allocated pointer.
}


/////////////////////////////////////////////////////////////////////////////////
// Realloc
//
// The ISO C++ standard does not describe realloc but instead defers to the 
// ISO C89 standard.
//
// The ISO C89 standard states:
//     (1) realloc(NULL, newsize) is equivalent to malloc(newsize).
//     (2) realloc(pblock, 0) is equivalent to free(pblock) (except that NULL is returned).
//     (3) if the realloc() fails, the object pointed to by pblock is left unchanged.
//
// The ISO C99 standard states:
//     The realloc function deallocates the old object pointed to by ptr and 
//     returns a pointer to a new object that has the size specified by size. 
//     The contents of the new object shall be the same as that of the old 
//     object prior to deallocation, up to the lesser of the new and old sizes. 
//     Any bytes in the new object beyond the size of the old object have
//     indeterminate values.
//
//     If ptr is a null pointer, the realloc function behaves like the malloc 
//     function for the specified size. Otherwise, if ptr does not match a 
//     pointer earlier returned by the calloc, malloc, or realloc function, 
//     or if the space has been deallocated by a call to the free or realloc 
//     function, the behavior is undefined. If memory for the new object 
//     cannot be allocated, the old object is not deallocated and its value 
//     is unchanged.
//     
//     The realloc function returns a pointer to the new object (which may have 
//     the same value as a pointer to the old object), or a null pointer if 
//     the new object could not be allocated.
//
// Note that the C99 standard does not state that reallocing a pointer to 
// zero size acts the same as free and returns NULL. However, many C implementations
// and definitions of realloc specify this behaviour. It seems to this author
// that the C99 standard is more proper, as it specifies consistent behaviour
// regardless of the specified size.
//
// Our behaviour
//     ptr      size    return     semantics
//     -------------------------------------------------------------
//     NULL     >0      NULL       allocation failed.
//     NULL     >0      valid      allocation succeeded.
//     valid    >0      NULL       allocation failed, original pointer not freed.
//     valid    >0      valid      allocation failed, original pointer freed.
//  PPM_REALLOC_C99_ENABLED => 0
//     NULL      0      NULL       no-op.
//     valid     0      NULL       original pointer freed.
//  PPM_REALLOC_C99_ENABLED => 1 (this is the default)
//     NULL      0      NULL       allocation of zero size block failed.
//     NULL      0      valid      allocation of zero size block succeeded.
//     valid     0      NULL       allocation of zero size block failed, original pointer not freed.
//     valid     0      valid      allocation of zero size block succeeded, original pointer freed.
//
//     Reallocating a pointer to a smaller size is guaranteed to succeed, 
//     but is not guaranteed to return the original pointer.
//
// One of the results of all this is that strictly speaking it is unsafe to 
// write code like this:
//     pData = realloc(pData, 100);
// because if the realloc fails, the original pointer isn't freed. But the code 
// above loses the original pointer.
//
void* NonLocalAllocator::Realloc(void* pData, size_t nNewSize, int nAllocationFlags)
{
	// For new size of zero, C89 but not C99 standard calls for the same behaviour as free.
	#if !PPM_REALLOC_C99_ENABLED // If C89 behaviour...
		if(EA_UNLIKELY(nNewSize == 0))
		{
			if(pData != mFailureValue)
				Free(pData);
			return mFailureValue;
		}
	#endif

	void* pReturnValue = mFailureValue;

	if(pData != mFailureValue)  // If the old pointer was valid... (else we can just do a Malloc)
	{
		#if PPM_THREAD_SAFETY_SUPPORTED
			PPMAutoMutex autoMutex(mpMutex);
		#endif

		#if PPM_AUTO_HEAP_VALIDATION_SUPPORTED
			if(EA_UNLIKELY(mAutoHeapValidationLevel && (++mnAutoHeapValidationEventCount >= mnAutoHeapValidationFrequency)))
			{
				mnAutoHeapValidationEventCount = 0;
				ValidateHeap(mAutoHeapValidationLevel);
			}
		#endif

		Node* pNode = mHashTable.Find(pData);
		PPM_ASSERT(this, pNode, kGAErrorBadUserPtr, kGAErrorText[kGAErrorBadUserPtr], pData, pData);

		if(pNode)
		{
			// If we are resizing downward, we either split the node or leave it as it is.
			// If we are resizing upward, we see if we can coalesce with the succeeding node in memory.
			#if PPM_NLA_DEBUG_DATA_SUPPORTED
				const size_t guardSize = GetGuardSize(pNode);
			#else
				const size_t guardSize = 0;
			#endif

			const size_t userSize     = (pNode->GetUserSize() - guardSize);
			const size_t newSizeTotal = nNewSize + guardSize;

			// To consider: Move this hook function call to a place above before checking if pNode is valid. 
			#if PPM_NLA_HOOKS_SUPPORTED
				if(mpHookFunction)
				{
					// Don't lock the critical section during this call, as we are calling external  
					// code and mutex locks during external calls is risky for deadlocks.
					const HookInfo hookInfo = { this, true, kHookTypeMalloc, kHookSubTypeRealloc, newSizeTotal, pData, 0, newSizeTotal, NULL, 0, nAllocationFlags, NULL, NULL, 0 };
					mpHookFunction(&hookInfo, mpHookFunctionContext);
				}
			#endif

			if(nNewSize > userSize) // If we need to grow...
			{
				// In this case we allocate a new node, copy the node data, and free the previous node.
				Node* const pNodeNew = MallocNode(newSizeTotal, nAllocationFlags);

				if(pNodeNew)
				{
					// Transfer the debug data from the old node to the new node.
					pNodeNew->mpDebugData = pNode->mpDebugData;
					pNode->mpDebugData    = NULL;

					PPM_ASSERT(this, (userSize == 0) || (mpMemcpyFunction != NULL), kGAErrorInvalidInput, kGAErrorText[kGAErrorInvalidInput], NULL, NULL); // There must be a working memory copy for Realloc to work as expected.
					if(mpMemcpyFunction)
						mpMemcpyFunction(pNodeNew->mpData, pNode->mpData, userSize);

					// Implement the guard fills here.
					#if PPM_NLA_DEBUG_DATA_SUPPORTED
						if(mbEnableDebugWrites && mpMemsetFunction)
							WriteGuardFill(pNodeNew, guardSize);
					#endif

					mHashTable.Insert(pNodeNew);

					// To consider: We have the Node associated with pData, so perhaps we can free via that pNode instead of pData.
					if(pData != mFailureValue)
						Free(pData);

					#if PPM_NLA_METRICS_SUPPORTED
						CollectMetrics(pNodeNew, true);
					#endif

					pReturnValue = pNodeNew->mpData;
				}
			}
			else if(nNewSize < userSize) // Else we need to shrink...
			{
				// Currently we leave the node unsplit and reduce the user space.
				// To do: Have this split the node if there is sufficient extra space.

				#if PPM_NLA_METRICS_SUPPORTED
					CollectMetrics(pNode, false);
				#endif

				#if PPM_NLA_DEBUG_DATA_SUPPORTED
					if(mbEnableDebugWrites)
						SetGuardSize(pNode, guardSize + (userSize - nNewSize), pNode->mpData + nNewSize, true);
				#endif

				#if PPM_NLA_METRICS_SUPPORTED
					CollectMetrics(pNode, true);
				#endif
			}
			else // Else there is nothing to do.
				pReturnValue = pData;

			#if PPM_NLA_HOOKS_SUPPORTED
				if(mpHookFunction)
				{
					const HookInfo hookInfo = { this, false, kHookTypeMalloc, kHookSubTypeRealloc, nNewSize, pData, 0, nNewSize, NULL, 0, nAllocationFlags, pReturnValue, NULL, 0 };
					mpHookFunction(&hookInfo, mpHookFunctionContext);
				}
			#endif
		}
	}
	else
	{
		// For old pointer of NULL, C89 and C99 standards call for same behaviour as malloc.
		pReturnValue = Malloc(nNewSize, nAllocationFlags);
	}

	return pReturnValue;
}


void* NonLocalAllocator::ReallocAligned(void* p, size_t nNewSize, size_t nAlignment, size_t nAlignmentOffset, int nAllocationFlags)
{
	EA_UNUSED(p);
	EA_UNUSED(nNewSize);
	EA_UNUSED(nAlignment);
	EA_UNUSED(nAlignmentOffset);
	EA_UNUSED(nAllocationFlags);

	// To do. 
	// As of this writing, GeneralAllocator doesn't have a ReallocAligned function.
	// Perhaps we thus don't need it here either. Bug the maintainer of this package
	// if you need this functionality.

	return mFailureValue;
}


void* NonLocalAllocator::MallocDebug(size_t nSize, int nAllocationFlags, 
									 unsigned nDebugDataFlags, const char* pName, const char* pFile, int nLine)
{
	#if PPM_THREAD_SAFETY_SUPPORTED
		PPMAutoMutex autoMutex(mpMutex);
	#endif

	#if PPM_NLA_DEBUG_DATA_SUPPORTED
		mDebugDataInfo.mnNextDebugDataFlags = nDebugDataFlags;
		mDebugDataInfo.mpNextAllocationName = pName;
		mDebugDataInfo.mpNextAllocationFile = pFile;
		mDebugDataInfo.mnNextAllocationLine = nLine;
	#else
		EA_UNUSED(nDebugDataFlags); EA_UNUSED(pName); EA_UNUSED(pFile); EA_UNUSED(nLine);
	#endif

	void* const pResult = Malloc(nSize, nAllocationFlags);

	#if PPM_NLA_DEBUG_DATA_SUPPORTED
		mDebugDataInfo.mnNextDebugDataFlags = 0;
		mDebugDataInfo.mpNextAllocationName = NULL;
		mDebugDataInfo.mpNextAllocationFile = NULL;
	#endif

	return pResult;
}


void* NonLocalAllocator::MallocAlignedDebug(size_t nSize, size_t nAlignment, size_t nAlignmentOffset, int nAllocationFlags,
											 unsigned nDebugDataFlags, const char* pName, const char* pFile, int nLine)
{
	#if PPM_THREAD_SAFETY_SUPPORTED
		PPMAutoMutex autoMutex(mpMutex);
	#endif

	#if PPM_NLA_DEBUG_DATA_SUPPORTED
		mDebugDataInfo.mnNextDebugDataFlags = nDebugDataFlags;
		mDebugDataInfo.mpNextAllocationName = pName;
		mDebugDataInfo.mpNextAllocationFile = pFile;
		mDebugDataInfo.mnNextAllocationLine = nLine;
	#else
		EA_UNUSED(nDebugDataFlags); EA_UNUSED(pName); EA_UNUSED(pFile); EA_UNUSED(nLine);
	#endif

	void* const pResult = MallocAligned(nSize, nAlignment, nAlignmentOffset, nAllocationFlags);

	#if PPM_NLA_DEBUG_DATA_SUPPORTED
		mDebugDataInfo.mnNextDebugDataFlags = 0;
		mDebugDataInfo.mpNextAllocationName = NULL;
		mDebugDataInfo.mpNextAllocationFile = NULL;
	#endif

	return pResult;
}


void** NonLocalAllocator::MallocMultipleDebug(size_t count, const size_t* pSizeArray, const size_t* pAlignmentArray, 
											   const size_t* pAlignmentOffsetArray, void* pResultArray[], int nAllocationFlags,
											   unsigned nDebugDataFlags, const char* pName, const char* pFile, int nLine)
{
	#if PPM_THREAD_SAFETY_SUPPORTED
		PPMAutoMutex autoMutex(mpMutex);
	#endif

	#if PPM_NLA_DEBUG_DATA_SUPPORTED
		mDebugDataInfo.mnNextDebugDataFlags = nDebugDataFlags;
		mDebugDataInfo.mpNextAllocationName = pName;
		mDebugDataInfo.mpNextAllocationFile = pFile;
		mDebugDataInfo.mnNextAllocationLine = nLine;
	#else
		EA_UNUSED(nDebugDataFlags); EA_UNUSED(pName); EA_UNUSED(pFile); EA_UNUSED(nLine);
	#endif

	void** const pResult = MallocMultiple(count, pSizeArray, pAlignmentArray, pAlignmentOffsetArray, pResultArray, nAllocationFlags);

	#if PPM_NLA_DEBUG_DATA_SUPPORTED
		mDebugDataInfo.mnNextDebugDataFlags = 0;
		mDebugDataInfo.mpNextAllocationName = NULL;
		mDebugDataInfo.mpNextAllocationFile = NULL;
	#endif

	return pResult;
}


void* NonLocalAllocator::ReallocDebug(void* p, size_t nNewSize, int nAllocationFlags,
									  unsigned nDebugDataFlags, const char* pName, const char* pFile, int nLine)
{
	#if PPM_THREAD_SAFETY_SUPPORTED
		PPMAutoMutex autoMutex(mpMutex);
	#endif

	#if PPM_NLA_DEBUG_DATA_SUPPORTED
		mDebugDataInfo.mnNextDebugDataFlags = nDebugDataFlags;
		mDebugDataInfo.mpNextAllocationName = pName;
		mDebugDataInfo.mpNextAllocationFile = pFile;
		mDebugDataInfo.mnNextAllocationLine = nLine;
	#else
		EA_UNUSED(nDebugDataFlags); EA_UNUSED(pName); EA_UNUSED(pFile); EA_UNUSED(nLine);
	#endif

	void* const pResult = Realloc(p, nNewSize, nAllocationFlags);

	#if PPM_NLA_DEBUG_DATA_SUPPORTED
		mDebugDataInfo.mnNextDebugDataFlags = 0;
		mDebugDataInfo.mpNextAllocationName = NULL;
		mDebugDataInfo.mpNextAllocationFile = NULL;
	#endif

	return pResult;
}


void* NonLocalAllocator::ReallocAlignedDebug(void* p, size_t nNewSize, size_t nAlignment, size_t nAlignmentOffset, int nAllocationFlags,
											 unsigned nDebugDataFlags, const char* pName, const char* pFile, int nLine)
{
	#if PPM_THREAD_SAFETY_SUPPORTED
		PPMAutoMutex autoMutex(mpMutex);
	#endif

	#if PPM_NLA_DEBUG_DATA_SUPPORTED
		mDebugDataInfo.mnNextDebugDataFlags = nDebugDataFlags;
		mDebugDataInfo.mpNextAllocationName = pName;
		mDebugDataInfo.mpNextAllocationFile = pFile;
		mDebugDataInfo.mnNextAllocationLine = nLine;
	#else
		EA_UNUSED(nDebugDataFlags); EA_UNUSED(pName); EA_UNUSED(pFile); EA_UNUSED(nLine);
	#endif

	void* const pResult = ReallocAligned(p, nNewSize, nAlignment, nAlignmentOffset, nAllocationFlags);

	#if PPM_NLA_DEBUG_DATA_SUPPORTED
		mDebugDataInfo.mnNextDebugDataFlags = 0;
		mDebugDataInfo.mpNextAllocationName = NULL;
		mDebugDataInfo.mpNextAllocationFile = NULL;
	#endif

	return pResult;
}


void NonLocalAllocator::SetMallocFailureFunction(MallocFailureFunction pMallocFailureFunction, void* pContext)
{
	mpMallocFailureFunction        = pMallocFailureFunction;
	mpMallocFailureFunctionContext = pContext;
}


void NonLocalAllocator::SetMemcpyFunction(MemcpyFunction pMemcpyFunction)
{
	mpMemcpyFunction = pMemcpyFunction;
}


void NonLocalAllocator::SetMemsetFunction(MemsetFunction pMemsetFunction)
{
	mpMemsetFunction = pMemsetFunction;
}


void NonLocalAllocator::SetMemoryFillCheckFunction(MemoryFillCheckFunction pMemoryFillCheckFunction)
{
	mpMemoryFillCheckFunction = pMemoryFillCheckFunction;
}


void NonLocalAllocator::AssertionFailureFunctionDefault(AssertionFailureInfo* pAssertionFailureInfo, void* pContext)
{
	TraceFunctionDefault(pAssertionFailureInfo->mpDescription, pContext);
	TraceFunctionDefault("\n", pContext);
	PPM_DebugBreak();
}


void NonLocalAllocator::DoFailureNotification(const Node* pNode, const char* pMessage, int id) const
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
		DescribeNode(pNode, messageString + (nMessageLength + 1), nStringLength - (nMessageLength + 1), true);
		PPM_DEBUG_FAILURE(this, messageString, id, kGAErrorText[id], pNode, pNode);
		EA_UNUSED(id);
	}
}


void NonLocalAllocator::SetAssertionFailureFunction(NonLocalAllocator::AssertionFailureInfoFunction pAssertionFailureFunction, void* pContext)
{
	mpAssertionFailureFunction        = pAssertionFailureFunction;
	mpAssertionFailureFunctionContext = pContext;
}


void NonLocalAllocator::AssertionFailure(const char* pExpression, int nErrorId, const char* pDescription, const void* pData, const void* pExtra) const
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


void NonLocalAllocator::SetHookFunction(HookFunction pHookFunction, void* pContext)
{
	#if PPM_NLA_HOOKS_SUPPORTED
		mpHookFunction        = pHookFunction;
		mpHookFunctionContext = pContext;
	#else
		EA_UNUSED(pHookFunction); // Prevent compiler warnings.
		EA_UNUSED(pContext);
	#endif
}


void NonLocalAllocator::SetOption(int option, intptr_t nValue)
{
	switch (option)
	{
		case kOptionEnableThreadSafety:
			#if PPM_THREAD_SAFETY_SUPPORTED
				if(nValue)
				{
					if(!mpMutex)
					{
						mpMutex = PPMMutexCreate(mpMutexData);
						PPM_ASSERT(this, mpMutex != NULL, kGAErrorOS, kGAErrorText[kGAErrorOS], NULL, NULL); // If this fails, the system kernel is exhausted or corrupted.
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
			#endif
			break;

		case kOptionEnableHighAllocation:
			// Currently we just always enabled high allocation, 
			// as it doesn't have much of a cost in this class.
			break;

		case kOptionMaxMallocFailureCount:
			mnMaxMallocFailureCount = (unsigned)nValue;
			break;

		case kOptionEnableDebugReads:
			mbEnableDebugReads = (nValue != 0);
			break;

		case kOptionEnableDebugWrites:
			mbEnableDebugWrites = (nValue != 0);
			break;

		case kOptionEnableMetrics:
			#if PPM_NLA_METRICS_SUPPORTED
				mbMetricsEnabled = (nValue != 0);
			#endif
			break;

		case kOptionMinSplitSize:
			mMinSplitSize = PPM_MAX(kMinAllocSize, (size_type)nValue);
			break;

		case kOptionFailureValue:
			mFailureValue = (char*)nValue;
			break;

		case kOptionEnableMallocFailureAssert:
			mbMallocFailureAssertEnabled = (nValue != 0);
			break;

		default:
			PPM_ASSERT(this, option == 12345678, kGAErrorInvalidInput, kGAErrorText[kGAErrorInvalidInput], NULL, NULL); // Use bogus assetion in order to prevent compiler warnings.
			break;
	}
}


intptr_t NonLocalAllocator::GetOption(int option) const
{
	switch (option)
	{
		case kOptionEnableThreadSafety:
			#if PPM_THREAD_SAFETY_SUPPORTED
				return mpMutex ? 1 : 0;
			#else
				return 0;
			#endif

		case kOptionEnableHighAllocation:
			return 1;

		case kOptionMaxMallocFailureCount:
			return (int)mnMaxMallocFailureCount;

		case kOptionEnableDebugReads:
			return mbEnableDebugReads ? 1 : 0;

		case kOptionEnableDebugWrites:
			return mbEnableDebugWrites ? 1 : 0;

		case kOptionEnableMetrics:
			#if PPM_NLA_METRICS_SUPPORTED
				return mbMetricsEnabled ? 1 : 0;
			#else
				return false;
			#endif

		case kOptionMinSplitSize:
			return (int)mMinSplitSize;

		case kOptionFailureValue:
			return (intptr_t)mFailureValue;

		case kOptionEnableMallocFailureAssert:
			return mbMallocFailureAssertEnabled ? 1 : 0;
	}

	PPM_ASSERT(this, option == 12345678, kGAErrorInvalidInput, kGAErrorText[kGAErrorInvalidInput], NULL, NULL); // Use bogus assetion in order to prevent compiler warnings.
	return 0;
}


bool NonLocalAllocator::ValidateHeap(HeapValidationLevel heapValidationLevel)
{
	int nErrorCount = 0;

	#if PPM_NLA_HEAP_VALIDATION_SUPPORTED
		#if PPM_THREAD_SAFETY_SUPPORTED
			PPMAutoMutex autoMutex(mpMutex);
		#endif

		#if (PPM_DEBUG >= 3)  // For higher debug levels, we bump up the requested validation level.
			heapValidationLevel = (HeapValidationLevel)(heapValidationLevel + (PPM_DEBUG - 1));
		#endif

		if(!mbHeapValidationActive) // Check for recursion.
		{
			mbHeapValidationActive = true;

			for(const CoreBlock* pCoreBlock  = (CoreBlock*)mHeadCoreBlock.mpListNext; 
								 pCoreBlock != &mHeadCoreBlock; 
								 pCoreBlock  = (CoreBlock*)pCoreBlock->mpListNext)
			{
				const char* const pCoreMin = pCoreBlock->mpBlock;
				const char* const pNodeMin = pCoreBlock->mpMemNext->mpBlock;
				const char* const pCoreMax = pCoreBlock->mpBlock + pCoreBlock->GetSize();

				if(heapValidationLevel >= kHeapValidationLevelFull)
				{
					// Verify that this core block refers to memory that doesn't overlap with any other core block.
					for(const CoreBlock* pCoreBlock2  = (CoreBlock*)mHeadCoreBlock.mpListNext; 
										 pCoreBlock2 != &mHeadCoreBlock; 
										 pCoreBlock2  = (CoreBlock*)pCoreBlock2->mpListNext)
					{
						if(pCoreBlock2 != pCoreBlock)
						{
							const char* const pCore2Min = pCoreBlock2->mpBlock;
							const char* const pCore2Max = pCoreBlock2->mpBlock + pCoreBlock2->GetSize();

							nErrorCount += PPM_VERIFY(this, (pCore2Max <= pCoreMin) || (pCoreMax <= pCore2Min), kGAErrorSuppliedCoreInvalid, kGAErrorText[kGAErrorSuppliedCoreInvalid], pCoreBlock, pCoreBlock2);
						}
					}
				}

				// Verify individual nodes.
				const Node* pNodeLast = pCoreBlock->mpMemPrev;
				const char* pCurrent  = pNodeMin;

				// Verify that the first node doesn't point into the core block header that precedes it.
				nErrorCount += PPM_VERIFY(this, pNodeMin >= pCoreMin, kGAErrorCorruptedHeap, kGAErrorText[kGAErrorCorruptedHeap], pNodeMin, pCoreMin);

				// Verify that the last node in the core block ends at the end of the core block.
				size_type s = pNodeLast->GetSize();
				nErrorCount += PPM_VERIFY(this, (pNodeLast->mpBlock + s) == pCoreMax, kGAErrorCorruptedHeap, kGAErrorText[kGAErrorCorruptedHeap], pNodeLast, pNodeLast);

				for(const Node* pNode  = pCoreBlock->mpMemNext, *pMemPrev = pCoreBlock; 
								pNode != pCoreBlock; 
								pNode  = pNode->mpMemNext)
				{
					// Verify that the Node data is of minimum alignment.
					nErrorCount += PPM_VERIFY(this, PPM_IsAligned(pNode->mpBlock, kMinAlignment), kGAErrorInternal, kGAErrorText[kGAErrorInternal], pNode, pNode);

					// Verify that the linked list is sane.
					nErrorCount += PPM_VERIFY(this, (pNode->mpMemNext->mpMemPrev == pNode), kGAErrorCorruptedHeap, kGAErrorText[kGAErrorCorruptedHeap], pNode, pNode);

					// Verify that the current block begins where the previous block ends.
					nErrorCount += PPM_VERIFY(this, pNode->mpBlock == pCurrent, kGAErrorCorruptedHeap, kGAErrorText[kGAErrorCorruptedHeap], pNode, pNode);

					// Verify that the current block ends before the end of the core block. We use unusual math to prevent integer wraparound problems.
					s = pNode->GetSize();
					nErrorCount += PPM_VERIFY(this, (pCoreMax - s) >= pNode->mpBlock, kGAErrorCorruptedHeap, kGAErrorText[kGAErrorCorruptedHeap], pNode, pNode);

					if(pNode->IsInUse()) // If it is a node that is allocated by the user...
					{
						// Verify that the Node user data is of minimum alignment.
						nErrorCount += PPM_VERIFY(this, PPM_IsAligned(pNode->mpData, kMinAlignment), kGAErrorInternal, kGAErrorText[kGAErrorInternal], pNode, pNode);

						#if PPM_NLA_DEBUG_DATA_SUPPORTED
							// Verify that the guard fill is as expected.
							nErrorCount += PPM_VERIFY(this, VerifyGuardFill(pNode), kGAErrorChunkFillError, kGAErrorText[kGAErrorChunkFillError], pNode, pNode);

							// Make sure any debug info for blocks are consistent with the block data (where feasible to test).
							if(pNode->mpDebugData)
							{
								void* pDebugData = NULL;
								size_t debugDataLength = GetDebugDataLength(pNode, &pDebugData);

								// Verify that the debug data length is sane.
								nErrorCount += PPM_VERIFY(this, debugDataLength < 16000000, kGAErrorCorruptedHeap, kGAErrorText[kGAErrorCorruptedHeap], pNode, pNode);

								// Verify that pDebugData is sane.
								nErrorCount += PPM_VERIFY(this, pDebugData >= pNode->mpDebugData, kGAErrorCorruptedHeap, kGAErrorText[kGAErrorCorruptedHeap], pNode, pNode);
							}
						#endif
					}
					else // Else the node refers to free memory.
					{
						#if PPM_NLA_DEBUG_DATA_SUPPORTED
							// Verify that the free fill is as expected.
							if(mbEnableDebugWrites)
								nErrorCount += PPM_VERIFY(this, VerifyFreeFill(pNode), kGAErrorChunkFillError, kGAErrorText[kGAErrorChunkFillError], pNode, pNode);

							// Verify that free memory has no debug data attached to it.
							nErrorCount += PPM_VERIFY(this, pNode->mpDebugData == NULL, kGAErrorInternal, kGAErrorText[kGAErrorInternal], pNode, pNode);
						#endif

						// Verify that free entries have no debug data attached to them.
						nErrorCount += PPM_VERIFY(this, pNode->mpDebugData == NULL, kGAErrorInternal, kGAErrorText[kGAErrorInternal], pNode, pNode);

						// Verify that free nodes are coalesced, that there are not two contiguous free nodes.
						nErrorCount += PPM_VERIFY(this, (pMemPrev->IsInUse() != 0) && (pNode->mpMemNext->IsInUse() != 0), kGAErrorInternal, kGAErrorText[kGAErrorInternal], pNode, pNode);

						if(heapValidationLevel >= kHeapValidationLevelDetail)
						{
							// Verify that free entries are in the free list. We'll verify non-free entries when we look at the free lists.
							nErrorCount += PPM_VERIFY(this, FindNodeInFreeList(pNode) != NULL, kGAErrorCorruptedHeap, kGAErrorText[kGAErrorCorruptedHeap], pNode, pNode);
						}
					}

					pCurrent = pNode->mpBlock + pNode->GetSize();
					pMemPrev = pNode;

				} // For each node

			} // For each core block


			// Walk the free lists and make sure that each entry is in the correctly sized bin. 
			// Make sure there are no entries marked as allocated.
			if(heapValidationLevel >= kHeapValidationLevelDetail)
			{
				for(uint32_t binIndex = 0; 
							 binIndex < kBinCount; 
							 binIndex++)
				{
					for(Node* pNode  =  mFreeListArray[binIndex].mpListNext, *pNodePrev = &mFreeListArray[binIndex]; 
							  pNode != &mFreeListArray[binIndex]; 
							  pNode  =  pNode->mpListNext)
					{
						// Verify that the node refers to valid memory.
						CoreBlock* const pCoreBlock1 = FindCoreBlockForAddress(pNode->mpBlock);
						CoreBlock* const pCoreBlock2 = FindCoreBlockForAddress(pNode->mpBlock + (pNode->GetSize() - 1)); // -1 because we want the last byte, not one-after the last byte.
						nErrorCount += PPM_VERIFY(this, (pCoreBlock1 != NULL) && (pCoreBlock2 != NULL), kGAErrorChunkOutsideHeap, kGAErrorText[kGAErrorChunkOutsideHeap], pNode, pNode);

						// Verify that the node isn't marked as allocated.
						nErrorCount += PPM_VERIFY(this, pNode->IsInUse() == 0, kGAErrorCorruptedHeap, kGAErrorText[kGAErrorCorruptedHeap], pNode, pNode);

						// Verify that the node is in the expected bin.
						const uint32_t expectedBinIndex = GetBinIndex(pNode->GetSize());
						nErrorCount += PPM_VERIFY(this, expectedBinIndex == binIndex, kGAErrorInternal, kGAErrorText[kGAErrorInternal], pNode, pNode);

						// Verify that the node is sorted properly.
						nErrorCount += PPM_VERIFY(this, pNode->GetSize() >= pNodePrev->GetSize(), kGAErrorInternal, kGAErrorText[kGAErrorInternal], pNode, pNode);

						pNodePrev = pNode;
					}
				}
			}

			// Validate the hash table
			if(heapValidationLevel >= kHeapValidationLevelDetail)
			{
				if(mHashTable.mpBucketArray)
				{
					for(size_t b = 0; b < mHashTable.mnBucketCount; b++)
					{
						size_t count = 0;
						const size_t kMaxCount = 16384;

						for(const Node* p = mHashTable.mpBucketArray[b]; p && (count < kMaxCount); p = p->mpListNext)
							++count;

						// Verify the count isn't over some sanity check value and that there isn't an infinite loop.
						nErrorCount += PPM_VERIFY(this, count < kMaxCount, kGAErrorInternal, kGAErrorText[kGAErrorInternal], NULL, NULL);
					}
				}
			}

			mbHeapValidationActive = false;
		}
	#else
		EA_UNUSED(heapValidationLevel);
	#endif

	return (nErrorCount == 0);
}


void NonLocalAllocator::SetAutoHeapValidation(HeapValidationLevel heapValidationLevel, size_t nFrequency)
{
	#if PPM_AUTO_HEAP_VALIDATION_SUPPORTED
		mnAutoHeapValidationFrequency  = ((nFrequency > 0) ? nFrequency : 1);
		mAutoHeapValidationLevel       = heapValidationLevel;
		mnAutoHeapValidationEventCount = 0;
	#else
		EA_UNUSED(heapValidationLevel);
		EA_UNUSED(nFrequency);
	#endif
}


void NonLocalAllocator::SetTraceFunction(TraceFunction pTraceFunction, void* pContext)
{
	mpTraceFunction        = pTraceFunction;
	mpTraceFunctionContext = pContext;
}


void NonLocalAllocator::TraceAllocatedMemory(TraceFunction pTraceFunction, void* pTraceFunctionContext, int nBlockTypeFlags)
{
	#if PPM_THREAD_SAFETY_SUPPORTED
		PPMAutoMutex autoMutex(mpMutex);
	#endif

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
				const Node* const pNode = (const Node*)pBlockInfo->mpNode;

				DescribeNode(pNode, pTraceBuffer, sizeof(pTraceBuffer)/sizeof(pTraceBuffer[0]), true);
				pTraceFunction(pTraceBuffer, pTraceFunctionContext);
			}

			ReportEnd(&reportContext);
		}
	}
}


size_t NonLocalAllocator::DescribeData(const void* pData, char* pBuffer, size_t nBufferLength)
{
	#if PPM_THREAD_SAFETY_SUPPORTED
		PPMAutoMutex autoMutex(mpMutex);
	#endif

	Node* const pNode = mHashTable.Find(pData);

	if(pNode)
		return DescribeNode(pNode, pBuffer, nBufferLength, true);

	return 0;
}


const void* NonLocalAllocator::ValidateAddress(const void* pAddress, int addressType) const
{
	const void* pReturnValue;

	#if PPM_THREAD_SAFETY_SUPPORTED
		PPMAutoMutex autoMutex(mpMutex);
	#endif

	if(mHashTable.Find(pAddress))
		pReturnValue = pAddress;    // Regardless of addrssType, the return value is success.
	else
	{
		pReturnValue = mFailureValue;

		if(addressType != kAddressTypeSpecific)
		{
			const CoreBlock* const pCoreBlock = FindCoreBlockForAddress(pAddress);

			if(pCoreBlock) // If this address is in fact part of a core block (and thus is part of some node)...
			{
				if(addressType == kAddressTypeOwned) // If all the user wants to know is if this pointer is owned in any way by this allocator...
					pReturnValue = pAddress;
				else
				{
					for(const Node* pNode  = pCoreBlock->mpMemNext; 
									pNode != pCoreBlock; 
									pNode  = pNode->mpMemNext)
					{
						if(((char*)pAddress >= pNode->mpBlock) && ((char*)pAddress < (pNode->mpBlock + pNode->GetSize())))
						{
							if((addressType == kAddressTypeAllocated) && !pNode->IsInUse())
								return pReturnValue; // pReturnValue == mFailureValue.

							pReturnValue = pNode->mpData;
							break;
						}
					}
				}
			}
		} // else the hash table lookup failed, but the user specified kAddressTypeSpecific, so we have failure.
	}

	#if (PPM_DEBUG >= 2)
		if((pReturnValue != mFailureValue) && ((addressType == kAddressTypeSpecific) || (addressType == kAddressTypeAllocated)))
		{
			const Node* const pNode = mHashTable.Find(pReturnValue);
			VerifyGuardFill(pNode);
		}
	#endif

	return pReturnValue;
}


bool NonLocalAllocator::ReportHeap(HeapReportFunction pHeapReportFunction, void* pContext, int nBlockTypeFlags)
{
	bool bReturnValue = false;

	// No mutex lock needed because ReportBegin/ReportEnd implement locking.

	if(pHeapReportFunction)
	{
		ReportContext  reportContext;
		ReportContext* pReportContext = ReportBegin(&reportContext, nBlockTypeFlags);

		if(pReportContext)
		{
			bReturnValue = true;

			for(const BlockInfo* pBlockInfo = ReportNext(pReportContext, nBlockTypeFlags);
								 pBlockInfo && bReturnValue;
								 pBlockInfo = ReportNext(pReportContext, nBlockTypeFlags))
			{
				bReturnValue = pHeapReportFunction(pBlockInfo, pContext);
			}
		}

		ReportEnd(pReportContext);
	}

	return bReturnValue;
}


NonLocalAllocator::ReportContext* NonLocalAllocator::ReportBegin(ReportContext* pReportContext, int nBlockTypeFlags)
{
	Lock(true);

	pReportContext->mnBlockTypeFlags   = nBlockTypeFlags;
	pReportContext->mpCurrentCoreBlock = (CoreBlock*)mHeadCoreBlock.mpListNext;
	pReportContext->mpCurrentNode      = NULL;
	pReportContext->mbCoreBlockReported = false;
	memset(&pReportContext->mBlockInfo, 0, sizeof(BlockInfo));

	return pReportContext;
}


void NonLocalAllocator::ReportEnd(ReportContext* pReportContext)
{
	// We need do nothing significant in this implementation. 
	// But we may need to do something in the future.
	#if (PPM_DEBUG >= 1)
		memset(pReportContext, 0, sizeof(ReportContext));
	#else
		EA_UNUSED(pReportContext);
	#endif

	Lock(false);
}


const NonLocalAllocator::BlockInfo* NonLocalAllocator::ReportNext(ReportContext* pReportContext, int nBlockTypeFlags)
{
	if(pReportContext)
	{
		// In this case we iterate the heap as we go. The user must not modify the heap 
		// while we are doing this or else it is possible that an exception will occur.
		nBlockTypeFlags &= pReportContext->mnBlockTypeFlags;

		for(CoreBlock* pCoreBlock  = pReportContext->mpCurrentCoreBlock;
					   pCoreBlock != &mHeadCoreBlock;
					   pCoreBlock  = pReportContext->mpCurrentCoreBlock)
		{
			// Process the core block before proceeding to the node list within the Core block.
			//
			if ((nBlockTypeFlags & kBlockTypeCore) && !pReportContext->mbCoreBlockReported)
			{
				pReportContext->mbCoreBlockReported = true;
				GetBlockInfoForCoreBlock(pCoreBlock, &pReportContext->mBlockInfo);
				return &pReportContext->mBlockInfo;
			}

			const Node* const pEndNode = pCoreBlock;

			if(pCoreBlock->GetSize())
			{
				if(pReportContext->mpCurrentNode)
					pReportContext->mpCurrentNode = pReportContext->mpCurrentNode->mpMemNext;
				else
					pReportContext->mpCurrentNode = pCoreBlock->mpMemNext;

				while(((Node*)pReportContext->mpCurrentNode != pEndNode) && !NodeMatchesBlockType((Node*)pReportContext->mpCurrentNode, nBlockTypeFlags))
					pReportContext->mpCurrentNode = pReportContext->mpCurrentNode->mpMemNext; // Go to the next Node in contiguous memory.

				if(pReportContext->mpCurrentNode != pEndNode)
					break; // We have found the next matching node.
			}

			if(!pCoreBlock->GetSize() || (pReportContext->mpCurrentNode == pEndNode)) // If we should move onto the next core block...
			{
				do{
					pCoreBlock                         = (CoreBlock*)pReportContext->mpCurrentCoreBlock->mpListNext;
					EA_ANALYSIS_ASSUME(pCoreBlock != NULL);
					pReportContext->mpCurrentCoreBlock = pCoreBlock;
				} while((pCoreBlock != &mHeadCoreBlock) && !pCoreBlock->GetSize());

				pReportContext->mbCoreBlockReported = false;
				pReportContext->mpCurrentNode = NULL;
			}
		}

		if(pReportContext->mpCurrentCoreBlock != &mHeadCoreBlock) // If mpCurrentNode comes from a core block...
		{
				// At this point, mpCurrentNode has already been incremented to the node we want to return to the user.
				GetBlockInfoForNode(pReportContext->mpCurrentNode, &pReportContext->mBlockInfo,
									pReportContext->mpCurrentCoreBlock->mpMemNext->mpBlock);

			return &pReportContext->mBlockInfo;
		}
	}

	return NULL; // Note that we don't return mFailureValue, as this return value is BlockInfo and not an allocated pointer.
}




NonLocalAllocator::size_type NonLocalAllocator::NodeMatchesBlockType(const Node* pNode, int nBlockTypeFlags)
{
	// To consider: This could possibly be made more efficient by 
	// using a 2D boolean table instead of the if/else logic here.

	if((nBlockTypeFlags & (kBlockTypeAllocated | kBlockTypeFree)) == (kBlockTypeAllocated | kBlockTypeFree))
		return 1; // The user is asking for any type of flags.

	const size_type bIsAllocated = pNode->IsInUse();

	if(nBlockTypeFlags & kBlockTypeAllocated)
		return bIsAllocated;
	else if(nBlockTypeFlags & kBlockTypeFree)
		return bIsAllocated ? (size_type)0 : (size_type)1;

	return 0;
}


size_t NonLocalAllocator::GetUsableSize(const void* pData) const
{
	#if PPM_THREAD_SAFETY_SUPPORTED
		PPMAutoMutex autoMutex(mpMutex);
	#endif

	const Node* const pNode = mHashTable.Find(pData);

	if(pNode)
	{
		#if PPM_NLA_DEBUG_DATA_SUPPORTED
			const size_t guardSize = GetGuardSize(pNode);
			const size_t userSize  = (pNode->GetUserSize() - guardSize);
		#else
			const size_t userSize  = pNode->GetUserSize();
		#endif
	   
		return userSize;
	}

	return (size_t)-1;
}


size_t NonLocalAllocator::GetLargestFreeBlock()
{
	#if PPM_THREAD_SAFETY_SUPPORTED
		PPMAutoMutex autoMutex(mpMutex);
	#endif

	// We walk the free block bins backwards (i.e. from largest to smallest).
	for(int i = (kBinCount - 1); i >= 0; i--)
	{
		const Node* const pNodeLast = mFreeListArray[i].mpListPrev;

		if(pNodeLast != &mFreeListArray[i]) // If the list is non-empty...
			return pNodeLast->GetSize();
	}

	return 0;
}


NonLocalAllocator::Node* NonLocalAllocator::FindNodeInFreeList(const Node* pNode)
{
	// This function assumes that the node would be in an appropriate bin for its size.
	// If this is being called from heap validation code then it is assumed that 
	// the heap validation code will find out that the node is in the wrong bin
	// by other tests than this.

	const size_type blockSize = pNode->GetSize();
	const uint32_t  binIndex  = GetBinIndex(blockSize);

	Node* pNodeCurrent;
	Node* pNodePrev;

	for(pNodeCurrent  = mFreeListArray[binIndex].mpListNext, pNodePrev = &mFreeListArray[binIndex]; 
		pNodeCurrent != &mFreeListArray[binIndex] && (blockSize >= pNodeCurrent->GetSize()); // We use >= here instead of >
		pNodeCurrent  = pNodeCurrent->mpListNext)
	{
		if(pNode == pNodeCurrent)
			return pNodePrev; // We return the location that we would insert the Node.

		pNodePrev = pNodeCurrent;
	}

	return NULL; // Note that we don't return mFailureValue, as this return value is a Node and not the user pointer.
}


#if PPM_NLA_DEBUG_DATA_SUPPORTED
	void NonLocalAllocator::AttachDebugDataToNode(const DebugDataInfo& debugDataInfo, Node* pNode, bool skipNewFill)
	{
		if(mbEnableDebugWrites && mpMemsetFunction)
		{
			// Write the 'new' fill.
			if(!skipNewFill && (mnFillNew != kDefaultFillValueDisabled))
				mpMemsetFunction(pNode->mpData, mnFillNew, pNode->GetUserSize());

			// Write the guard fill.
			if(debugDataInfo.mnGuardFillSize)  // If there is any guard fill...
				WriteGuardFill(pNode, debugDataInfo.mnGuardFillSize);
		}

		SetDebugInfoForAllocation(debugDataInfo, pNode);
	}
#endif


#if PPM_NLA_DEBUG_DATA_SUPPORTED
	void NonLocalAllocator::WriteGuardFill(const Node* pNode, size_t nSize)
	{
		if(mpMemsetFunction)
		{
			const size_t userSize         = (size_t)pNode->GetUserSize();
			const size_t leadingSpaceSize = (size_t)(pNode->mpData - pNode->mpBlock);

			PPM_ASSERT(this, nSize <= userSize, kGAErrorInternal, kGAErrorText[kGAErrorInternal], NULL, NULL); // If this fails, PPMalloc is probably at fault.
			PPM_ASSERT(this, pNode->mpBlock + pNode->GetSize() == pNode->mpData + userSize, kGAErrorInternal, kGAErrorText[kGAErrorInternal], NULL, NULL); // If this fails, PPMalloc is probably at fault.

			if(EA_UNLIKELY(leadingSpaceSize)) // If there is any leading space...
				mpMemsetFunction(pNode->mpBlock, mnFillGuard, leadingSpaceSize);

			mpMemsetFunction(pNode->mpData + (userSize - nSize), mnFillGuard, nSize);
		}
	}
#endif


///////////////////////////////////////////////////////////////////////////////
// Node
///////////////////////////////////////////////////////////////////////////////

NonLocalAllocator::Node* NonLocalAllocator::Node::CoalesceWithNext(NonLocalAllocator::Node* pMemNext)
{
	PPM_ASSERT_STATIC((IsInUse() == 0) && (pMemNext->IsInUse() == 0), kGAErrorInternal, kGAErrorText[kGAErrorInternal], this, pMemNext); // If this fails, PPMalloc is probably at fault.

	mpMemNext = pMemNext->mpMemNext;
	mpMemNext->mpMemPrev = this;

	SetSize(GetSize() + pMemNext->GetSize());

	#if (PPM_DEBUG >= 1)
		pMemNext->mpMemPrev = NULL;
		pMemNext->mpMemNext = NULL;
	#endif

	return pMemNext;
}


void NonLocalAllocator::Node::SplitNext(size_type nNewSize, Node* pMemNewNext)
{
	#if (PPM_DEBUG > 0)
		memset(pMemNewNext, 0, sizeof(Node));
	#elif PPM_NLA_DEBUG_DATA_SUPPORTED
		pMemNewNext->mpDebugData = NULL;
	#endif

	pMemNewNext->mpMemNext = mpMemNext;
	mpMemNext->mpMemPrev   = pMemNewNext;
	pMemNewNext->mpMemPrev = this;
	mpMemNext              = pMemNewNext;

	pMemNewNext->mpBlock = mpBlock + nNewSize;
	pMemNewNext->SetSize(GetSize() - nNewSize);
	SetSize(nNewSize);
}


void NonLocalAllocator::Node::SplitPrev(size_type nNewPrevSize, Node* pMemNewPrev)
{
	#if (PPM_DEBUG > 0)
		memset(pMemNewPrev, 0, sizeof(Node));
	#elif PPM_NLA_DEBUG_DATA_SUPPORTED
		pMemNewPrev->mpDebugData = NULL;
	#endif

	pMemNewPrev->mpMemPrev = mpMemPrev;
	mpMemPrev->mpMemNext   = pMemNewPrev;
	pMemNewPrev->mpMemNext = this;
	mpMemPrev              = pMemNewPrev;

	pMemNewPrev->mpBlock = mpBlock;
	pMemNewPrev->SetSize(nNewPrevSize);
	mpBlock = pMemNewPrev->mpBlock + nNewPrevSize;
	SetSize(GetSize() - nNewPrevSize);
}




///////////////////////////////////////////////////////////////////////////////
// HashTable
///////////////////////////////////////////////////////////////////////////////

NonLocalAllocator::HashTable::HashTable()
  : mpBucketArray(NULL),
	mnBucketCount(0),
	mnBucketCountMin(4096),
	mnElementCount(0),
	mpNonLocalAllocator(NULL),
	mpAllocationFunction(NULL),
	mpFreeFunction(NULL),
	mpFunctionContext(NULL),
	mbReHashing(false)
{
}


NonLocalAllocator::HashTable::~HashTable()
{
	Clear(true);
}


NonLocalAllocator::Node* NonLocalAllocator::HashTable::Find(const void* pData) const
{
	if(mpBucketArray)
	{
		const size_t h = Hash(pData);

		for(Node* pNode = mpBucketArray[h]; 
				  pNode; 
				  pNode = pNode->mpListNext)
		{
			if(pNode->mpData == pData)
				return pNode;
		}
	}

	return NULL; // Note that we don't return mFailureValue, as this return value is a Node and not the user pointer.
}


bool NonLocalAllocator::HashTable::Remove(Node* pNodeRemove)
{
	if(mpBucketArray)
	{
		const size_t h = Hash(pNodeRemove->mpData);

		for(Node* pNode = mpBucketArray[h], *pPrev = NULL; 
				  pNode; 
				  pNode = pNode->mpListNext)
		{
			if(pNode->mpData == pNodeRemove->mpData) // If we found our node...
			{
				if(pPrev)
					pPrev->mpListNext = pNode->mpListNext;
				else
					mpBucketArray[h] = pNode->mpListNext;

				mnElementCount--;

				return true;
			}

			pPrev = pNode;
		}
	}

	return false;
}


void NonLocalAllocator::HashTable::Clear(bool bDestroyTable)
{
	if(mpBucketArray)
	{
		for(size_t i = 0; i < mnBucketCount; i++)
		{
			#if (PPM_DEBUG >= 1)
				for(Node* pNode = mpBucketArray[i], *pNext; 
						  pNode; 
						  pNode = pNext)
				{
					pNext = pNode->mpListNext;
					pNode->mpListNext = NULL;
				}
			#endif

			mpBucketArray[i] = NULL; // Note that this is a Node and not a user pointer; thus we don't use mFailureValue.
		}

		if(bDestroyTable)
		{
			if(mpFreeFunction)
				mpFreeFunction(mpNonLocalAllocator, mpBucketArray, sizeof(Node*) * mnBucketCount, mpFunctionContext);

			mpBucketArray = NULL;
			mnBucketCount = 0;
		}

		mnElementCount = 0;
	}
}


bool NonLocalAllocator::HashTable::ReHash(size_t nNewBucketCount)
{
	bool bReturnValue = true;

	if(!mbReHashing) // Re-entrancy prevention
	{
		mbReHashing = true;

		// To consider: We allocate the new buckets here before freeing the old buckets, but we can avoid
		// doing this by taking all the nodes and linking them together so we can find them, free the 
		// old bucket array, allocate the new bucket array, and then inserted the nodes from the list we made.

		const size_t nBucketCountOld  = mnBucketCount;
		Node** const ppBucketArrayOld = mpBucketArray;
		Node** const ppBucketArrayNew = (Node**)mpAllocationFunction(mpNonLocalAllocator, sizeof(Node*) * nNewBucketCount, mpFunctionContext);

		if(ppBucketArrayNew)
		{
			memset(ppBucketArrayNew, 0, sizeof(Node*) * nNewBucketCount); // Note that the allocation call above could trigger hash table reads or writes.

			mnBucketCount = nNewBucketCount;    // It's important that we do these assignments after the new table allocation in order
			mpBucketArray = ppBucketArrayNew;   // to deal  with re-entrancy that can occur during allocation of the new table.

			for(size_t i = 0; i < nBucketCountOld; i++)
			{
				for(Node* pNode = ppBucketArrayOld[i], *pNext; 
						  pNode; 
						  pNode = pNext)
				{
					const size_t h = Hash(pNode->mpData); // Hash will use the new bucket count.

					pNext               = pNode->mpListNext;
					pNode->mpListNext   = ppBucketArrayNew[h];
					ppBucketArrayNew[h] = pNode;
				}
			}

			if(ppBucketArrayOld)
				mpFreeFunction(mpNonLocalAllocator, ppBucketArrayOld, sizeof(Node*) * nBucketCountOld, mpFunctionContext);

			bReturnValue = true;
		}

		mbReHashing = false;
	}

	return bReturnValue;
}


void NonLocalAllocator::HashTable::SetAllocationFunction(NonLocalAllocator* pNonLocalAllocator, LocalAllocationFunction pAllocationFunction, 
															LocalFreeFunction pFreeFunction, void* pFunctionContext)
{
	mpNonLocalAllocator  = pNonLocalAllocator;
	mpAllocationFunction = pAllocationFunction;
	mpFreeFunction       = pFreeFunction;
	mpFunctionContext    = pFunctionContext;
}



///////////////////////////////////////////////////////////////////////////////
// NonLocalAllocator
///////////////////////////////////////////////////////////////////////////////

#if PPM_NLA_DEBUG_DATA_SUPPORTED
	void NonLocalAllocator::GetDebugInfoForAllocation(DebugDataInfo& debugDataInfo, size_t nSize, 
								int nAllocationFlags, size_t nAlignment, size_t nAlignmentOffset)
	{
		const unsigned nCurrentDebugDataFlags = (mnDebugDataFlags | mDebugDataInfo.mnNextDebugDataFlags);
		DebugDataInfoShared* pInfo;

		memset(debugDataInfo.mShared, 0, sizeof(debugDataInfo.mShared));
		debugDataInfo.mnGuardFillSize = 0;
		debugDataInfo.mnUserSize      = nSize;
		debugDataInfo.mnSeparateUsage = 0;      // We will increment this below as-needed.

		// Add allocator field size.
		if(nCurrentDebugDataFlags & (1 << kDebugDataIdAllocator))
		{
			pInfo = &debugDataInfo.mShared[kDebugDataIdAllocator];
			pInfo->mnRecordDataSize   = sizeof(NonLocalAllocator*);
			pInfo->mnRecordSize       = sizeof(NonLocalAllocator*) + kDebugRecordOverheadSize;
			debugDataInfo.mnSeparateUsage += pInfo->mnRecordSize;
		}

		// Add size field size.
		if(nCurrentDebugDataFlags & (1 << kDebugDataIdSize))
		{
			pInfo = &debugDataInfo.mShared[kDebugDataIdSize];
			pInfo->mnRecordDataSize   = sizeof(debugDataInfo.mnUserSize);
			pInfo->mnRecordSize       = sizeof(debugDataInfo.mnUserSize) + kDebugRecordOverheadSize;
			debugDataInfo.mnSeparateUsage += pInfo->mnRecordSize;
		}

		// Add allocation flags field size.
		if((nCurrentDebugDataFlags & (1 << kDebugDataIdFlags)) && nAllocationFlags)
		{
			pInfo = &debugDataInfo.mShared[kDebugDataIdFlags];
			pInfo->mnRecordDataSize   = sizeof(debugDataInfo.mnAllocationFlags);
			pInfo->mnRecordSize       = sizeof(debugDataInfo.mnAllocationFlags) + kDebugRecordOverheadSize;
			debugDataInfo.mnSeparateUsage += pInfo->mnRecordSize;
			debugDataInfo.mnAllocationFlags = nAllocationFlags;
		}

		// Add alignment field size.
		if(nCurrentDebugDataFlags & (1 << kDebugDataIdAlignment))
		{
			pInfo = &debugDataInfo.mShared[kDebugDataIdAlignment];
			pInfo->mnRecordDataSize         = nAlignment ? (nAlignmentOffset ? 2 * sizeof(size_t) : sizeof(size_t)) : 0;
			pInfo->mnRecordSize             = GetDebugDataRecordLength(pInfo->mnRecordDataSize);
			debugDataInfo.mnSeparateUsage += pInfo->mnRecordSize;
			debugDataInfo.mnAlignment       = nAlignment;
			debugDataInfo.mnAlignmentOffset = nAlignmentOffset;
		}

		// Add allocation name size.
		if(mDebugDataInfo.mpNextAllocationName) // Note that we don't check nCurrentDebugDataFlags for kDebugDataIdName here, though perhaps we could do so for consistency.
		{
			pInfo = &debugDataInfo.mShared[kDebugDataIdName];
			pInfo->mnRecordDataSize   = strlen(mDebugDataInfo.mpNextAllocationName) + 1; // '+ 1' for trailing 0.
			pInfo->mnRecordSize       = pInfo->mnRecordDataSize + kDebugRecordOverheadSize;
			debugDataInfo.mnSeparateUsage += pInfo->mnRecordSize;
		}

		// Add allocation place size.
		if(mDebugDataInfo.mpNextAllocationFile) // Note that we don't check nCurrentDebugDataFlags for kDebugDataIdPlace here, though perhaps we could do so for consistency.
		{
			pInfo = &debugDataInfo.mShared[kDebugDataIdPlace];
			pInfo->mnRecordDataSize   = sizeof(FileLineInfo);
			pInfo->mnRecordSize       = sizeof(FileLineInfo) + kDebugRecordOverheadSize;
			debugDataInfo.mnSeparateUsage += pInfo->mnRecordSize;
			debugDataInfo.mFileLineInfo.mpFile = mDebugDataInfo.mpNextAllocationFile; // Note that we don't strcpy the value, but simply assign the pointer.
			debugDataInfo.mFileLineInfo.mnLine = mDebugDataInfo.mnNextAllocationLine; // This is because we assume the pointer's value is a constant.
		}

		// Add the call stack field size.
		#if PPM_DEBUG_CALLSTACK_AVAILABLE    // If the given platform supports call stack traces...
			if(nCurrentDebugDataFlags & (1 << kDebugDataIdCallStack))
			{
				pInfo = &debugDataInfo.mShared[kDebugDataIdCallStack];
				pInfo->mnRecordDataSize   = sizeof(void*) * GetCallStack(debugDataInfo.mpReturnAddressArray, 24);
				pInfo->mnRecordSize       = GetDebugDataRecordLength(pInfo->mnRecordDataSize);
				debugDataInfo.mnSeparateUsage += pInfo->mnRecordSize;
			}
		#endif

		// Add checksum field size.
		if(nCurrentDebugDataFlags & (1 << kDebugDataIdChecksum))
		{
			pInfo = &debugDataInfo.mShared[kDebugDataIdChecksum];
			pInfo->mnRecordDataSize   = sizeof(size_t);
			pInfo->mnRecordSize       = sizeof(size_t) + kDebugRecordOverheadSize;
			debugDataInfo.mnSeparateUsage += pInfo->mnRecordSize;
		}

		// Add historical count size.
		if(nCurrentDebugDataFlags & (1 << kDebugDataIdAllocationCount))
		{
			pInfo = &debugDataInfo.mShared[kDebugDataIdAllocationCount];
			pInfo->mnRecordDataSize   = sizeof(mnAllocationCountHistorical);
			pInfo->mnRecordSize       = sizeof(mnAllocationCountHistorical) + kDebugRecordOverheadSize;
			debugDataInfo.mnSeparateUsage += pInfo->mnRecordSize;
		}

		// Add time field size.
		if(nCurrentDebugDataFlags & (1 << kDebugDataIdAllocationTime))
		{
			pInfo = &debugDataInfo.mShared[kDebugDataIdAllocationTime];
			pInfo->mnRecordDataSize   = sizeof(size_t);
			pInfo->mnRecordSize       = sizeof(size_t) + kDebugRecordOverheadSize;
			debugDataInfo.mnSeparateUsage += pInfo->mnRecordSize;
		}

		// Add guard fill field size.
		if(nCurrentDebugDataFlags & (1 << kDebugDataIdGuard))
		{
			// Guard fills in NonLocalAllocator are implemented differently from GeneralAllocator.
			// In NonLocalAllocator we merely store the size of the guard fill and not the guard fill itself.
			pInfo = &debugDataInfo.mShared[kDebugDataIdGuard];
			pInfo->mnRecordDataSize   = sizeof(size_t);
			pInfo->mnRecordSize       = sizeof(size_t) + kDebugRecordOverheadSize;
			debugDataInfo.mnGuardFillSize = GetGuardSizeForBlock(nSize);
			debugDataInfo.mnSeparateUsage += pInfo->mnRecordSize;
		}

		// Add overhead field size.
		if(nCurrentDebugDataFlags & (1 << kDebugDataIdOverhead))
		{
			pInfo = &debugDataInfo.mShared[kDebugDataIdOverhead];
			pInfo->mnRecordDataSize   = sizeof(size_t);
			pInfo->mnRecordSize       = sizeof(size_t) + kDebugRecordOverheadSize;
			debugDataInfo.mnSeparateUsage += pInfo->mnRecordSize;
		}

		// Add group id size.
		if(nCurrentDebugDataFlags & (1 << kDebugDataIdGroup))
		{
			pInfo = &debugDataInfo.mShared[kDebugDataIdGroup];
			pInfo->mnRecordDataSize   = sizeof(mnCurrentGroupId);
			pInfo->mnRecordSize       = sizeof(mnCurrentGroupId) + kDebugRecordOverheadSize;
			debugDataInfo.mnSeparateUsage += pInfo->mnRecordSize;
		}

		if(debugDataInfo.mnSeparateUsage)
			debugDataInfo.mnSeparateUsage += 2 * sizeof(DebugDataLengthType); // 2x because at the beginning of the debug info block is a block size, and at the end of the block is a current debug info size.

	}
#endif // #if PPM_NLA_DEBUG_DATA_SUPPORTED


#if PPM_NLA_DEBUG_DATA_SUPPORTED
	bool NonLocalAllocator::SetDebugInfoForAllocation(const DebugDataInfo& debugDataInfo, Node* pNode)
	{
		// No mutex lock here because this is an internal function.
		//#if PPM_THREAD_SAFETY_SUPPORTED
		//PPM_ASSERT(this, !mpMutex || PPMMutexGetLockCount(mpMutex) > 0, kGAErrorInternal, kGAErrorText[kGAErrorInternal], NULL, NULL); // Verify that the mutex is in fact locked. // If this fails, PPMalloc is at fault.
		//#endif

		if(debugDataInfo.mnSeparateUsage) // If there is debug info...
		{
			const DebugDataLengthType blockSize = (DebugDataLengthType)debugDataInfo.mnSeparateUsage;

			pNode->mpDebugData = (char*)mpLocalDebugAllocationFunction(this, blockSize, mpLocalAllocationContext);
			PPM_ASSERT(this, pNode->mpDebugData != NULL, kGAErrorDebugMemoryExhaustion, kGAErrorText[kGAErrorDebugMemoryExhaustion], NULL, NULL);

			if(pNode->mpDebugData)
			{
				memcpy(pNode->mpDebugData, &blockSize, sizeof(blockSize));

				// Write in the debug data size itself. 
				char* const pDebugInfoSize = pNode->mpDebugData + blockSize - sizeof(DebugDataLengthType);
				memset(pDebugInfoSize, 0, sizeof(DebugDataLengthType)); // It is zero now, but will be updated below as we add individual records.

				// Write allocator field.
				const DebugDataInfoShared* pInfo = &debugDataInfo.mShared[kDebugDataIdAllocator];
				if(pInfo->mnRecordSize)
				{
					PPM_ASSERT(this, pInfo->mnRecordDataSize == sizeof(NonLocalAllocator*), kGAErrorInternal, kGAErrorText[kGAErrorInternal], NULL, NULL); // If this fails, PPMalloc is at fault.
					const NonLocalAllocator* const pThis = this;
					SetDebugData(pNode, kDebugDataIdAllocator, &pThis, pInfo->mnRecordDataSize);
				}

				// Write size field.
				pInfo = &debugDataInfo.mShared[kDebugDataIdSize];
				if(pInfo->mnRecordSize)
				{
					PPM_ASSERT(this, pInfo->mnRecordDataSize == sizeof(debugDataInfo.mnUserSize), kGAErrorInternal, kGAErrorText[kGAErrorInternal], NULL, NULL); // If this fails, PPMalloc is at fault.
					SetDebugData(pNode, kDebugDataIdSize, &debugDataInfo.mnUserSize, pInfo->mnRecordDataSize);
				}

				// Write allocation flags field.
				pInfo = &debugDataInfo.mShared[kDebugDataIdFlags];
				if(pInfo->mnRecordSize)
				{
					PPM_ASSERT(this, pInfo->mnRecordDataSize == sizeof(debugDataInfo.mnAllocationFlags), kGAErrorInternal, kGAErrorText[kGAErrorInternal], NULL, NULL); // If this fails, PPMalloc is at fault.
					SetDebugData(pNode, kDebugDataIdFlags, &debugDataInfo.mnAllocationFlags, pInfo->mnRecordDataSize);
				}

				// Write alignment field.
				pInfo = &debugDataInfo.mShared[kDebugDataIdAlignment];
				if(pInfo->mnRecordSize)
				{
					if(debugDataInfo.mnAlignmentOffset)
					{
						const size_t pArray[2] = { debugDataInfo.mnAlignment, debugDataInfo.mnAlignmentOffset};
						PPM_ASSERT(this, pInfo->mnRecordDataSize == sizeof(pArray), kGAErrorInternal, kGAErrorText[kGAErrorInternal], NULL, NULL); // If this fails, PPMalloc is at fault.
						SetDebugData(pNode, kDebugDataIdAlignment, pArray, pInfo->mnRecordDataSize);
					}
					else
					{
						PPM_ASSERT(this, pInfo->mnRecordDataSize == sizeof(debugDataInfo.mnAlignment), kGAErrorInternal, kGAErrorText[kGAErrorInternal], NULL, NULL); // If this fails, PPMalloc is at fault.
						SetDebugData(pNode, kDebugDataIdAlignment, &debugDataInfo.mnAlignment, pInfo->mnRecordDataSize);
					}
				}

				// Write name field.
				pInfo = &debugDataInfo.mShared[kDebugDataIdName];
				if(pInfo->mnRecordSize)
				{
					// To be most safe, we protect agains an mpNextAllocationName that is NULL, even though that shouldn't normally happen.
					if(mDebugDataInfo.mpNextAllocationName)
					{
						PPM_ASSERT(this, pInfo->mnRecordDataSize == (strlen(mDebugDataInfo.mpNextAllocationName) + 1), kGAErrorInternal, kGAErrorText[kGAErrorInternal], NULL, NULL); // If this fails, PPMalloc is at fault.
						SetDebugData(pNode, kDebugDataIdName, mDebugDataInfo.mpNextAllocationName, pInfo->mnRecordDataSize);
					}
					else
					{
						const char emptyName = 0;
						SetDebugData(pNode, kDebugDataIdName, &emptyName, 1);
					}
				}

				// Write place field.
				pInfo = &debugDataInfo.mShared[kDebugDataIdPlace];
				if(pInfo->mnRecordSize)
				{
					PPM_ASSERT(this, pInfo->mnRecordDataSize == sizeof(debugDataInfo.mFileLineInfo), kGAErrorInternal, kGAErrorText[kGAErrorInternal], NULL, NULL); // If this fails, PPMalloc is at fault.
					SetDebugData(pNode, kDebugDataIdPlace, &debugDataInfo.mFileLineInfo, pInfo->mnRecordDataSize);
				}

				// Write call stack field.
				#if PPM_DEBUG_CALLSTACK_AVAILABLE
					pInfo = &debugDataInfo.mShared[kDebugDataIdCallStack];
					if(pInfo->mnRecordSize)
					{
						// To do: enable this: PPM_ASSERT(this, (pInfo->mnRecordDataSize % sizeof(uintptr_t)) == 0, kGAErrorInternal, kGAErrorText[kGAErrorInternal], NULL, NULL); // If this fails, PPMalloc is at fault.
						SetDebugData(pNode, kDebugDataIdCallStack, debugDataInfo.mpReturnAddressArray, pInfo->mnRecordDataSize);
					}
				#endif

				// Write checksum field.
				pInfo = &debugDataInfo.mShared[kDebugDataIdChecksum];
				if(pInfo->mnRecordSize)
				{
					size_t zero(0);
					PPM_ASSERT(this, pInfo->mnRecordDataSize == sizeof(zero), kGAErrorInternal, kGAErrorText[kGAErrorInternal], NULL, NULL); // If this fails, PPMalloc is at fault.
					SetDebugData(pNode, kDebugDataIdChecksum, &zero, pInfo->mnRecordDataSize);
				}

				// Write count field.
				pInfo = &debugDataInfo.mShared[kDebugDataIdAllocationCount];
				if(pInfo->mnRecordSize)
				{
					PPM_ASSERT(this, pInfo->mnRecordDataSize == sizeof(mnAllocationCountHistorical), kGAErrorInternal, kGAErrorText[kGAErrorInternal], NULL, NULL); // If this fails, PPMalloc is at fault.
					SetDebugData(pNode, kDebugDataIdAllocationCount, &mnAllocationCountHistorical, pInfo->mnRecordDataSize);
				}

				// Write time field.
				pInfo = &debugDataInfo.mShared[kDebugDataIdAllocationTime];
				if(pInfo->mnRecordSize)
				{
					const size_t nCurrentTime = (size_t)GetPPMallocDebugTime();
					PPM_ASSERT(this, pInfo->mnRecordDataSize == sizeof(nCurrentTime), kGAErrorInternal, kGAErrorText[kGAErrorInternal], NULL, NULL); // If this fails, PPMalloc is at fault.
					SetDebugData(pNode, kDebugDataIdAllocationTime, &nCurrentTime, pInfo->mnRecordDataSize);
				}

				// Write overhead field.
				pInfo = &debugDataInfo.mShared[kDebugDataIdOverhead];
				if(pInfo->mnRecordSize)
				{
					const size_t nOverhead = sizeof(Node) + debugDataInfo.mnSeparateUsage; 
					PPM_ASSERT(this, pInfo->mnRecordDataSize == sizeof(nOverhead), kGAErrorInternal, kGAErrorText[kGAErrorInternal], NULL, NULL); // If this fails, PPMalloc is at fault.
					SetDebugData(pNode, kDebugDataIdOverhead, &nOverhead, pInfo->mnRecordDataSize);
				}

				// Write group id size field.
				pInfo = &debugDataInfo.mShared[kDebugDataIdGroup];
				if(pInfo->mnRecordSize)
				{
					PPM_ASSERT(this, pInfo->mnRecordDataSize == sizeof(mnCurrentGroupId), kGAErrorInternal, kGAErrorText[kGAErrorInternal], NULL, NULL); // If this fails, PPMalloc is at fault.
					SetDebugData(pNode, kDebugDataIdGroup, &mnCurrentGroupId, pInfo->mnRecordDataSize);
				}
			}
		}

		return true;
	}
#endif // #if PPM_NLA_DEBUG_DATA_SUPPORTED


void NonLocalAllocator::SetGuardSize(float fGuardSizeRatio, size_t nMinGuardSize, size_t nMaxGuardSize)
{
	#if PPM_THREAD_SAFETY_SUPPORTED
		PPMAutoMutex autoMutex(mpMutex);
	#endif

	#if PPM_NLA_DEBUG_DATA_SUPPORTED
		mfGuardSizeRatio = fGuardSizeRatio;
		mnMinGuardSize   = nMinGuardSize;
		mnMaxGuardSize   = nMaxGuardSize;

		if(mfGuardSizeRatio < 0.f)
		   mfGuardSizeRatio = 0.f;

		if(mfGuardSizeRatio > 8.f)
		   mfGuardSizeRatio = 8.f;

		if(mnMinGuardSize > nMaxGuardSize)
		   mnMinGuardSize = nMaxGuardSize;

		if((mfGuardSizeRatio == 0) || (mnMaxGuardSize == 0)) // This code really doesn't need to exist.
			mnDebugDataFlags &= ~(1 << kDebugDataIdGuard);
	#else
		EA_UNUSED(fGuardSizeRatio);
		EA_UNUSED(nMinGuardSize);
		EA_UNUSED(nMaxGuardSize);
	#endif
}


#if PPM_NLA_DEBUG_DATA_SUPPORTED
	size_t NonLocalAllocator::GetGuardSizeForBlock(size_t nBlockSize)
	{
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
#endif


size_t NonLocalAllocator::GetDebugDataLength(const void* pData, void** ppDebugData) const
{
	#if PPM_NLA_DEBUG_DATA_SUPPORTED
		#if PPM_THREAD_SAFETY_SUPPORTED
			PPMAutoMutex autoMutex(mpMutex);
		#endif

		const Node* const pNode = mHashTable.Find(pData);

		return GetDebugDataLength(pNode, ppDebugData);
	#else
		EA_UNUSED(pData);
		EA_UNUSED(ppDebugData);
		return 0;
	#endif
}


#if PPM_NLA_DEBUG_DATA_SUPPORTED
	size_t NonLocalAllocator::GetDebugDataLength(const Node* pNode, void** ppDebugData) const
	{
		if(pNode && pNode->mpDebugData)
		{
			const DebugDataLengthType nDebugBlockLength  = *((DebugDataLengthType*)pNode->mpDebugData); // blockSize refers to the entire size of this block, including the space holding nDebugBlockSize.
			const char* const         pPositionDebugSize = pNode->mpDebugData + nDebugBlockLength - sizeof(DebugDataLengthType);

			DebugDataLengthType nDebugDataSize; // Refers to size of all debug data not including the field holding this size value.
			memcpy(&nDebugDataSize, pPositionDebugSize, sizeof(DebugDataLengthType));

			PPM_ASSERT(this, (size_type)(nDebugDataSize + sizeof(DebugDataLengthType)) <= (size_type)nDebugBlockLength, kGAErrorCorruptedChunk, kGAErrorText[kGAErrorCorruptedChunk], NULL, pNode->mpDebugData); // If this fails, the user probably corrupted the heap.
			if(ppDebugData)
				*ppDebugData = const_cast<char*>(((pNode->mpDebugData + nDebugBlockLength) - (nDebugDataSize + sizeof(DebugDataLengthType))));

			return (size_t)(nDebugDataSize + sizeof(DebugDataLengthType));
		}

		return 0;
	}
#endif


void* NonLocalAllocator::GetDebugData(const void* pData, DebugDataIdType id, 
										void* pDebugData, size_t nDataLength, size_t* pActualDataLength) const
{
	#if PPM_NLA_DEBUG_DATA_SUPPORTED
		#if PPM_THREAD_SAFETY_SUPPORTED
			PPMAutoMutex autoMutex(mpMutex);
		#endif

		const Node* const pNode = mHashTable.Find(pData);

		return GetDebugData(pNode, id, pDebugData, nDataLength, pActualDataLength);
	#else
		EA_UNUSED(pData); EA_UNUSED(id); EA_UNUSED(pDebugData); EA_UNUSED(nDataLength); EA_UNUSED(pActualDataLength);
		return NULL; // Note that we don't return mFailureValue, as this return value is debug data and not a user pointer.
	#endif
}


#if PPM_NLA_DEBUG_DATA_SUPPORTED
	void* NonLocalAllocator::GetDebugData(const Node* pNode, DebugDataIdType id, void* pDebugData, size_t nDataLength, size_t* pActualDataLength)
	{
		if(pNode && pNode->mpDebugData)
		{
			const DebugDataLengthType blockSize = *((DebugDataLengthType*)pNode->mpDebugData); // blockSize refers to the entire size of this block, including the space holding nDebugBlockSize.

			return GeneralAllocatorDebug::GetDebugDataStatic(pNode->mpDebugData, blockSize, id, pDebugData, nDataLength, pActualDataLength);
		}

		return NULL; // Note that we don't return mFailureValue, as this return value is debug data and not a user pointer.
	}
#endif


void* NonLocalAllocator::SetDebugData(void* pData, DebugDataIdType id, 
										const void* pDebugData, size_t nDataLength)
{
	#if PPM_NLA_DEBUG_DATA_SUPPORTED
		#if PPM_THREAD_SAFETY_SUPPORTED
			PPMAutoMutex autoMutex(mpMutex);
		#endif

		Node* const pNode = mHashTable.Find(pData);

		return SetDebugData(pNode, id, pDebugData, nDataLength);
	#else
		EA_UNUSED(pData); EA_UNUSED(id); EA_UNUSED(pDebugData); EA_UNUSED(nDataLength);
		return NULL;
	#endif
}


#if PPM_NLA_DEBUG_DATA_SUPPORTED
	void* NonLocalAllocator::SetDebugData(Node* pNode, DebugDataIdType id, const void* pDebugData, size_t nDataLength)
	{
		if(pNode && pNode->mpDebugData)
		{
			const DebugDataLengthType blockSize = *((DebugDataLengthType*)pNode->mpDebugData); // blockSize refers to the entire size of this block, including the space holding nDebugBlockSize.

			return GeneralAllocatorDebug::SetDebugDataStatic(pNode->mpDebugData, blockSize, id, pDebugData, nDataLength);
		}

		return NULL; // Note that we don't return mFailureValue, as this return value is debug data and not a user pointer.
	}
#endif


bool NonLocalAllocator::GetMallocDebugInfo(const char*& pAllocationName, const char*& pAllocationFile, int& nAllocationLine)
{
	#if PPM_NLA_DEBUG_DATA_SUPPORTED
		#if PPM_THREAD_SAFETY_SUPPORTED
			PPMAutoMutex autoMutex(mpMutex);
		#endif

		pAllocationName = mDebugDataInfo.mpNextAllocationName;
		pAllocationFile = mDebugDataInfo.mpNextAllocationFile;
		nAllocationLine = mDebugDataInfo.mnNextAllocationLine;

		return ((pAllocationName != NULL) && (pAllocationFile != NULL));
	#else
		pAllocationName = NULL;
		pAllocationFile = NULL;
		nAllocationLine = 0;

		return false;
	#endif
}


size_t NonLocalAllocator::DescribeNode(const Node* pNode, char* pBuffer, size_t nBufferLength, bool bAppendLineEnd) const
{
	#if PPM_THREAD_SAFETY_SUPPORTED
		PPMAutoMutex autoMutex(mpMutex);
	#endif

	int               nCount;
	char* const       pBufferStart = pBuffer;
	const char* const pBufferEnd   = pBuffer + nBufferLength;
	const size_type   nDataSize    = pNode->IsInUse() ? pNode->GetUserSize() : pNode->GetSize();
	const char* const pData        = pNode->IsInUse() ? pNode->mpData        : pNode->mpBlock;

	// See if we can write the "addr" field.
	#if defined(EA_PLATFORM_PTR_SIZE) && (EA_PLATFORM_PTR_SIZE == 8)
		size_t nRequiredSpace = 28;  // "addr: 0x1234567812345678\t\0" + 2
	#else
		size_t nRequiredSpace = 20;  // "addr: 0x12345678\t\0" + 2
	#endif

	if(nBufferLength >= nRequiredSpace)
	{
		// Write the "addr" field.
		#if defined(EA_PLATFORM_PTR_SIZE) && (EA_PLATFORM_PTR_SIZE == 8)
			nCount = PPM_Snprintf(pBuffer, nBufferLength, "addr: 0x%016" PRIx64 "%c", (uint64_t)(uintptr_t)pNode->mpData, mcTraceFieldDelimiter);
		#else
			nCount = PPM_Snprintf(pBuffer, nBufferLength, "addr: 0x%08x%c",  (unsigned)(uintptr_t)pNode->mpData, mcTraceFieldDelimiter);
		#endif

		PPM_ASSERT(this, nCount > 0, kGAErrorDebugMemoryExhaustion, kGAErrorText[kGAErrorDebugMemoryExhaustion], NULL, NULL); // If this fails, PPMalloc is at fault or the user didn't supply enough memory.
		pBuffer       += (size_t)nCount;
		nBufferLength -= (size_t)nCount;
	}


	// See if we can write the size field.
	nRequiredSpace = 31; // "size: 1234567890 (12345678)\t\0" + 2

	if(nBufferLength >= nRequiredSpace)
	{
		// Write the "size" field.
		nCount = PPM_Snprintf(pBuffer, nBufferLength, "size: %19" PRIuPTR " (%16" PRIxPTR ")%c", static_cast<uintptr_t>(nDataSize), static_cast<uintptr_t>(nDataSize), mcTraceFieldDelimiter);
		PPM_ASSERT(this, nCount > 0, kGAErrorDebugMemoryExhaustion, kGAErrorText[kGAErrorDebugMemoryExhaustion], NULL, NULL); // If this fails, PPMalloc is at fault or the user didn't supply enough memory.
		pBuffer       += (size_t)nCount;
		nBufferLength -= (size_t)nCount;
	}


	// See if we can write the data field.
	nRequiredSpace = 256 + 10; // We could write less data if the user had less space, but that would result in potentially inconsistent output as the user sees it.

	if((nBufferLength >= nRequiredSpace) && mbEnableDebugReads)
	{
		// Write the "data" field.
		const unsigned kDataStringCapacity = 256;
		char           dataString[kDataStringCapacity] = { 0 };

		GetDataPreview(pData, nDataSize, dataString, NULL, kDataStringCapacity);
		nCount = PPM_Snprintf(pBuffer, nBufferLength, "data: %s%c", dataString, mcTraceFieldDelimiter);
		PPM_ASSERT(this, nCount > 0, kGAErrorDebugMemoryExhaustion, kGAErrorText[kGAErrorDebugMemoryExhaustion], NULL, NULL); // If this fails, PPMalloc is at fault or the user didn't supply enough memory.
		pBuffer       += (size_t)nCount;
		nBufferLength -= (size_t)nCount;
	}


	// See if we can write the space field.
	if(!pNode->IsInUse())
	{
		nRequiredSpace = 11; // "<space>\t\0" + 2

		if(nBufferLength >= nRequiredSpace)
		{
			// Write the "space" field.
			nCount = PPM_Snprintf(pBuffer, nBufferLength, "<space>%c", mcTraceFieldDelimiter);
			PPM_ASSERT(this, nCount > 0, kGAErrorDebugMemoryExhaustion, kGAErrorText[kGAErrorDebugMemoryExhaustion], NULL, NULL); // If this fails, PPMalloc is at fault or the user didn't supply enough memory.
			pBuffer       += (size_t)nCount;
			nBufferLength -= (size_t)nCount;
		}
	}


	// Possibly get allocation flags.
	int nAllocationFlags = 0;

	if(GetDebugData(pNode, kDebugDataIdFlags, &nAllocationFlags, sizeof(nAllocationFlags)) && nAllocationFlags)
	{
		if((pBufferEnd - pBuffer) >= 24) // If there is enough space to hold the worst case situation...
		{
			strcpy(pBuffer, "flags:");
			pBuffer += 6; // += strlen("flags:");

			if(nAllocationFlags & kAllocationFlagHigh)
			{
				strcpy(pBuffer, " high");
				pBuffer += 5; // += strlen(" high");
			}

			if(nAllocationFlags & kAllocationFlagEndFit)
			{
				strcpy(pBuffer, " end-fit");
				pBuffer += 8; // += strlen(" end-fit");
			}

			*pBuffer++ = (char)mcTraceFieldDelimiter;
			*pBuffer = 0;
		}
	}


	// Possibly get allocation name.
	char   messageString[256] = { 0 };
	size_t nFieldDataLength = sizeof(messageString) / sizeof(messageString[0]);

	if(GetDebugData(pNode, kDebugDataIdName, messageString, nFieldDataLength, &nFieldDataLength) && nFieldDataLength)
	{
		if((size_t)(pBufferEnd - pBuffer) >= (nFieldDataLength + 12)) // If there is enough space...
		{
			const size_t nLength = (size_t)PPM_Snprintf(pBuffer, (size_t)(pBufferEnd - pBuffer), "name: %s%c", messageString, mcTraceFieldDelimiter);
			PPM_ASSERT(this, nLength > 0, kGAErrorDebugMemoryExhaustion, kGAErrorText[kGAErrorDebugMemoryExhaustion], NULL, NULL); // If this fails, PPMalloc is at fault or the user didn't supply enough memory.
			pBuffer += nLength;
		}
	}


	// Possibly get allocation place.
	FileLineInfo fileLineInfo = { NULL, 0 };

	if(GetDebugData(pNode, kDebugDataIdPlace, &fileLineInfo, sizeof(fileLineInfo)))
	{
		const size_t nFileNameLength = fileLineInfo.mpFile ? strlen(fileLineInfo.mpFile) : 0;

		if((size_t)(pBufferEnd - pBuffer) >= (nFileNameLength + 22)) // If there is enough space...
		{
			const size_t nLength = (size_t)PPM_Snprintf(pBuffer, (size_t)(pBufferEnd - pBuffer), "loc: %s, %d%c", fileLineInfo.mpFile, fileLineInfo.mnLine, mcTraceFieldDelimiter);
			PPM_ASSERT(this, nLength > 0, kGAErrorDebugMemoryExhaustion, kGAErrorText[kGAErrorDebugMemoryExhaustion], NULL, NULL); // If this fails, PPMalloc is at fault or the user didn't supply enough memory.
			pBuffer += nLength;
		}
	}


	// Possibly get allocation call stack.
	#if PPM_DEBUG_CALLSTACK_AVAILABLE
		void* pReturnAddressArray[24] = { 0 };

		if(GetDebugData(pNode, kDebugDataIdCallStack, pReturnAddressArray, sizeof(pReturnAddressArray), &nFieldDataLength) && nFieldDataLength)
		{
			if((pBufferEnd - pBuffer) > 32) // If there's enough room for at least "stack: 0x1234567812345678\t\0"
			{
				strcpy(pBuffer, "stack: ");
				pBuffer += 7; // += strlen("stack: ");

				const size_t nOutputCapacity = (size_t)(pBufferEnd - pBuffer) - 4; // Leave some extra room for trailing field and record delimites.
				const size_t nCharsWritten    = DescribeCallStack((const void**)pReturnAddressArray, nFieldDataLength / sizeof(void*), pBuffer, nOutputCapacity, nullptr); //lint !e1773 (Attempt to cast away const (or volatile))
				pBuffer += nCharsWritten;

				PPM_ASSERT(this, (pBufferEnd - pBuffer) > 0, kGAErrorDebugMemoryExhaustion, kGAErrorText[kGAErrorDebugMemoryExhaustion], NULL, NULL); // If this fails, PPMalloc is at fault or the user didn't supply enough memory.
				*pBuffer++ = (char)mcTraceFieldDelimiter;
				*pBuffer    = 0;
			}
		}
	#endif


	if(bAppendLineEnd)
	{
		// Append a trailing record delimiter.
		PPM_ASSERT(this, (pBufferEnd - pBuffer) > 0, kGAErrorDebugMemoryExhaustion, kGAErrorText[kGAErrorDebugMemoryExhaustion], NULL, NULL); // The above logic is set up to always guarantee at least one free byte at the end. // If this fails, PPMalloc is at fault.
		*pBuffer++ = (char)mcTraceRecordDelimiter;
		*pBuffer    = 0;
	}

	return (size_t)(pBuffer - pBufferStart);
}


void NonLocalAllocator::GetBlockInfoForCoreBlock(const CoreBlock* pCoreBlock, BlockInfo* pBlockInfo) const
{
	pBlockInfo->mpCore     = pCoreBlock->mpBlock;
	pBlockInfo->mpNode     = (const char*)pCoreBlock;
	pBlockInfo->mnSize     = pCoreBlock->GetSize();
	pBlockInfo->mpData     = pCoreBlock->mpBlock;
	pBlockInfo->mnDataSize = pCoreBlock->GetSize();
	pBlockInfo->mBlockType = kBlockTypeCore;
}


void NonLocalAllocator::GetBlockInfoForNode(const Node* pNode, BlockInfo* pBlockInfo, const void* pCore) const
{
	pBlockInfo->mpCore     = (const char*)pCore;
	pBlockInfo->mpNode     = (const char*)pNode;
	pBlockInfo->mnSize     = pNode->GetSize();
	pBlockInfo->mpData     = pNode->mpData;
	pBlockInfo->mnDataSize = pNode->GetUserSize();
	pBlockInfo->mBlockType = (char)(pNode->IsInUse() ? kBlockTypeAllocated : kBlockTypeFree);
}


#if PPM_NLA_DEBUG_DATA_SUPPORTED
	size_t NonLocalAllocator::GetGuardSize(const Node* pNode)
	{
		const void* const pGuardFillSize = GetDebugData(pNode, kDebugDataIdGuard, NULL, 0, NULL);

		if(pGuardFillSize) // If there is a fill...
		{
			size_t guardFillSize;
			memcpy(&guardFillSize, pGuardFillSize, sizeof(guardFillSize));
			return guardFillSize;
		}

		return 0;
	}
#endif


#if PPM_NLA_DEBUG_DATA_SUPPORTED
	bool NonLocalAllocator::SetGuardSize(const Node* pNode, size_t nSize, void* pGuardFillBegin, bool bDoFill)
	{
		if(mbEnableDebugWrites && mpMemsetFunction)
		{
			void* const pGuardFillSize = GetDebugData(pNode, kDebugDataIdGuard, NULL, 0, NULL);

			if(pGuardFillSize) // If there is an existing fill...
			{
				size_t guardFillSize;
				memcpy(&guardFillSize, pGuardFillSize, sizeof(guardFillSize));
				memcpy(pGuardFillSize, &nSize, sizeof(nSize));

				if(bDoFill && (nSize > guardFillSize)) // If the new guard is larger than the existing guard...
					mpMemsetFunction(pGuardFillBegin, mnFillGuard, nSize - guardFillSize);

				return true;
			}
		}

		// We don't add a fill record when there wasn't one previously.
		return false;
	}
#endif


#if PPM_NLA_DEBUG_DATA_SUPPORTED
	bool NonLocalAllocator::VerifyGuardFill(const Node* pNode) const
	{
		if(mbEnableDebugWrites && mpMemoryFillCheckFunction)
		{
			size_t guardFillSize;
			const void* const pGuardFillSize = GetDebugData(pNode, kDebugDataIdGuard, NULL, 0, NULL);

			if(pGuardFillSize) // If there is a fill...
			{
				memcpy(&guardFillSize, pGuardFillSize, sizeof(guardFillSize));

				const size_t userSize          = pNode->GetUserSize();
				const size_t leadingSpaceSize  = (size_t)(pNode->mpData - pNode->mpBlock);
				const size_t trailingSpaceSize = (size_t)(pNode->mpData + userSize - guardFillSize);

				PPM_ASSERT(this, leadingSpaceSize + userSize + guardFillSize <= pNode->GetSize(), kGAErrorCorruptedChunk, kGAErrorText[kGAErrorCorruptedChunk], pNode, pNode); // If this fails, the user probably corrupted the heap.

				if(EA_UNLIKELY(leadingSpaceSize)) // If there is any leading space...
				{
					// Possibly limit the check to a maximum size. Otherwise, verification can take an unacceptably long time.
					#if (PPM_DEBUG >= 3)
						const size_t leadingCheckSize = leadingSpaceSize;
					#elif (PPM_DEBUG >= 2)
						const size_t leadingCheckSize = PPM_MIN(leadingSpaceSize, 1024);
					#else
						const size_t leadingCheckSize = PPM_MIN(leadingSpaceSize, 64);
					#endif

					if(!mpMemoryFillCheckFunction((const unsigned char*)pNode->mpBlock, leadingCheckSize, mnFillGuard))
					{
						DoFailureNotification(pNode, "NonLocalAllocator::VerifyGuardFill failure.", kGAErrorChunkFillError);
						return false;
					}
				}

				// Possibly limit the check to a maximum size. Otherwise, verification can take an unacceptably long time.
				#if (PPM_DEBUG >= 3)
					const size_t trailingCheckSize = trailingSpaceSize;
				#elif (PPM_DEBUG >= 2)
					const size_t trailingCheckSize = PPM_MIN(trailingSpaceSize, 1024);
				#else
					const size_t trailingCheckSize = PPM_MIN(trailingSpaceSize, 64);
				#endif

				if(!mpMemoryFillCheckFunction((const unsigned char*)pNode->mpData + userSize, trailingCheckSize, mnFillGuard))
				{
					DoFailureNotification(pNode, "NonLocalAllocator::VerifyGuardFill failure.", kGAErrorChunkFillError);
					return false;
				}
			}
		}

		return true;
	}
#endif // #if PPM_NLA_DEBUG_DATA_SUPPORTED


#if PPM_NLA_DEBUG_DATA_SUPPORTED
	bool NonLocalAllocator::VerifyFreeFill(const Node* pNode) const
	{
		if(mbEnableDebugWrites && mpMemoryFillCheckFunction && !mpMemoryFillCheckFunction((const unsigned char*)pNode->mpBlock, pNode->GetSize(), mnFillFree))
		{
			DoFailureNotification(pNode, "NonLocalAllocator::VerifyFreeFill failure.", kGAErrorChunkFillError);
			return false;
		}

		return true;
	}
#endif


const NonLocalAllocator::Metrics* NonLocalAllocator::GetMetrics(MetricType metricsType)
{
	#if PPM_NLA_METRICS_SUPPORTED
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

		return mAllocationMetrics + metricsType;
	#else
		EA_UNUSED(metricsType);
		return NULL;
	#endif
}


#if PPM_NLA_METRICS_SUPPORTED
	void NonLocalAllocator::CollectMetrics(const Node* pNode, bool bMalloc)
	{
		// We always increase mnAllocationCountHistorical, even if we have metrics disabled.
		// The reason for this is that we want to be able to always support kDebugDataIdAllocationCount
		if(bMalloc)
			mnAllocationCountHistorical++;

		if(mbMetricsEnabled && pNode)
		{
			const size_t nBlockSize       = pNode->GetSize();
			const size_t nLeadingSize     = (size_t)(pNode->mpData - pNode->mpBlock);
			const size_t nGuardSize       = GetGuardSize(pNode);
			const size_t nDebugSize       = nLeadingSize + nGuardSize;
			const size_t nUserSize        = nBlockSize - nDebugSize;
			const size_t nSystemSize      = sizeof(Node) + (pNode->mpDebugData ? *((DebugDataLengthType*)pNode->mpDebugData) : 0);

			if(bMalloc) // If this is from allocating, and not freeing...
			{
				// kMetricTypeAll
				mAllocationMetrics[kMetricTypeAll].mnAllocationCount++;
				mAllocationMetrics[kMetricTypeAll].mnAllocationCountHistorical++;
				mAllocationMetrics[kMetricTypeAll].mnAllocationVolume           += nBlockSize;
				mAllocationMetrics[kMetricTypeAll].mnAllocationVolumeHistorical += nBlockSize;
			  //mAllocationMetrics[kMetricTypeAll].mnFreeVolume                 -= nBlockSize;

				if(mAllocationMetrics[kMetricTypeAll].mnAllocationCountMax  < mAllocationMetrics[kMetricTypeAll].mnAllocationCount)
				   mAllocationMetrics[kMetricTypeAll].mnAllocationCountMax  = mAllocationMetrics[kMetricTypeAll].mnAllocationCount;
				if(mAllocationMetrics[kMetricTypeAll].mnAllocationVolumeMax < mAllocationMetrics[kMetricTypeAll].mnAllocationVolume)
				   mAllocationMetrics[kMetricTypeAll].mnAllocationVolumeMax = mAllocationMetrics[kMetricTypeAll].mnAllocationVolume;

				// kMetricTypeSystem
				mAllocationMetrics[kMetricTypeSystem].mnAllocationVolume           += nSystemSize;
				mAllocationMetrics[kMetricTypeSystem].mnAllocationVolumeHistorical += nSystemSize;
			  //mAllocationMetrics[kMetricTypeSystem].mnFreeVolume                 -= nSystemSize;

				// kMetricTypeDebug
				mAllocationMetrics[kMetricTypeDebug].mnAllocationVolume           += nDebugSize;
				mAllocationMetrics[kMetricTypeDebug].mnAllocationVolumeHistorical += nDebugSize;
			  //mAllocationMetrics[kMetricTypeDebug].mnFreeVolume                 -= nDebugSize;

				// kMetricTypeUser
				mAllocationMetrics[kMetricTypeUser].mnAllocationVolume           += nUserSize;
				mAllocationMetrics[kMetricTypeUser].mnAllocationVolumeHistorical += nUserSize;
			  //mAllocationMetrics[kMetricTypeUser].mnFreeVolume                 -= nUserSize;
			}
			else // Else this is from freeing.
			{
				// kMetricTypeAll
				mAllocationMetrics[kMetricTypeAll].mnAllocationCount--;
				mAllocationMetrics[kMetricTypeAll].mnAllocationVolume     -= nBlockSize;
				mAllocationMetrics[kMetricTypeAll].mnFreeCountHistorical++;
			  //mAllocationMetrics[kMetricTypeAll].mnFreeVolume           += nBlockSize;
				mAllocationMetrics[kMetricTypeAll].mnFreeVolumeHistorical += nBlockSize;

				// kMetricTypeSystem
				mAllocationMetrics[kMetricTypeSystem].mnAllocationVolume     -= nSystemSize;
			  //mAllocationMetrics[kMetricTypeSystem].mnFreeVolume           += nSystemSize;
				mAllocationMetrics[kMetricTypeSystem].mnFreeVolumeHistorical += nSystemSize;

				// kMetricTypeDebug
				mAllocationMetrics[kMetricTypeDebug].mnAllocationVolume     -= nDebugSize;
			  //mAllocationMetrics[kMetricTypeDebug].mnFreeVolume            = 0;
				mAllocationMetrics[kMetricTypeDebug].mnFreeVolumeHistorical += nDebugSize;

				// kMetricTypeUser
				mAllocationMetrics[kMetricTypeUser].mnAllocationVolume      -= nUserSize;
			  //mAllocationMetrics[kMetricTypeUser].mnFreeVolume            += nUserSize;
				mAllocationMetrics[kMetricTypeUser].mnFreeVolumeHistorical  += nUserSize;
			}
		}
	}
#endif // #if PPM_NLA_METRICS_SUPPORTED


} // namespace Allocator

} // namespace EA


#if defined(_MSC_VER)
	#pragma warning(pop)
#endif


