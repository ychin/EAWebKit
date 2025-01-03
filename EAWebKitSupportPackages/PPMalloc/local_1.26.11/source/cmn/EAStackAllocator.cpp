///////////////////////////////////////////////////////////////////////////////
// EAStackAllocator
//
// Copyright (c) 2004, Electronic Arts. All Rights Reserved.
// By Paul Pedriana, Maxis
//
// This is a custom C++ implementation of a stack allocator much like 
// the 'obstack' allocator used in the GNUC compiler project (libiberty), 
// among other things. The GNUC objstack allocator is a GPL
// (GNU Public License) piece of code and thus cannot be used
// without adhering to GPL provisions. Thus we implement an allocator
// here which has the same functionality but is not encumbered by 
// the GPL license. If you are familiar with the obstack allocator 
// then you should be comfortable with this allocator. Documentation
// for standard GNUC obstacks can be found at:
//     http://gcc.gnu.org/onlinedocs/libiberty/Obstacks.html#Obstacks
// This allocator is also similar to other memory stack allocators 
// such as the old Graphics Gems III fast allocator:
//     http://www.cse.ucsc.edu/~pang/160/f98/Gems/GemsIII/alloc/
// 
//
///////////////////////////////////////////////////////////////////////////////
//
// Lint settings
// The following error suppressions largely represent warnings that can't 
// be easily fixed in code but are also known to be insignificant for this module.
//lint -e18                // Symbol redeclared...
//lint -esym(818,pData)    // Pointer parameter could be declared as pointing to const.
//lint -e774               // Boolean within 'if' always evaluates to true.
//lint -e413               // Likely use of null pointer in argument to operator.
///////////////////////////////////////////////////////////////////////////////


#include <PPMalloc/EAStackAllocator.h>
#include <stdlib.h>  // malloc / new


///////////////////////////////////////////////////////////////////////////////
// PPM_ENABLED usage
//
// PPM_ENABLED is set in config.h
//
#if PPM_ENABLED
	#include <PPMalloc/EAGeneralAllocator.h>
	#if defined(EA_MEMORY_DEBUG_ENABLED) && EA_MEMORY_DEBUG_ENABLED
		#include <PPMalloc/EAGeneralAllocatorDebug.h>
	#endif

	namespace EA{
		namespace Allocator{
			extern PPM_API GeneralAllocator* gpEAGeneralAllocator;
			#if defined(EA_MEMORY_DEBUG_ENABLED) && EA_MEMORY_DEBUG_ENABLED
				extern PPM_API GeneralAllocatorDebug* gpEAGeneralAllocatorDebug;
			#endif
		}
	}
#endif
///////////////////////////////////////////////////////////////////////////////



namespace EA
{
	namespace Allocator
	{
		// Function declarations
		void* DefaultStackAllocationFunction(size_t nSize, size_t* pSizeResult, void* pContext);
		void DefaultStackFreeFunction(void* pData, void* pContext);
		
	
		///////////////////////////////////////////////////////////////////////////////
		// DefaultStackAllocationFunction
		//
		void* DefaultStackAllocationFunction(size_t nSize, size_t* pSizeResult, void* /*pContext*/)
		{
			#if PPM_ENABLED
				void* const pReturnValue = EA::Allocator::gpEAGeneralAllocator->Malloc(nSize);
			#else
				void* const pReturnValue = new char[nSize];
			#endif

			if(pSizeResult)
			{
				if(pReturnValue)
					*pSizeResult = nSize;
				else
					*pSizeResult = 0;
			}

			return pReturnValue;
		}


		///////////////////////////////////////////////////////////////////////////////
		// DefaultStackFreeFunction
		//
		void DefaultStackFreeFunction(void* pData, void* /*pContext*/)
		{
			#if PPM_ENABLED
				EA::Allocator::gpEAGeneralAllocator->Free(pData);
			#else
				delete[] (char*)pData;
			#endif
		}


		///////////////////////////////////////////////////////////////////////////////
		// TrackedStackAllocationFunction
		//
		void* TrackedStackAllocationFunction(size_t nSize, size_t* pSizeResult, void* pContext)
		{
			EA_UNUSED(pContext);
			#if PPM_ENABLED
				#if defined(EA_MEMORY_DEBUG_ENABLED) && EA_MEMORY_DEBUG_ENABLED
					void* const pReturnValue = EA::Allocator::gpEAGeneralAllocatorDebug->MallocDebug(nSize, 0, 0, (const char*)pContext);
				#else
					void* const pReturnValue = EA::Allocator::gpEAGeneralAllocator->Malloc(nSize);
				#endif
			#else
				void* const pReturnValue = new char[nSize];
			#endif

			if(pSizeResult)
			{
				if(pReturnValue)
					*pSizeResult = nSize;
				else
					*pSizeResult = 0;
			}

			return pReturnValue;
		}


		///////////////////////////////////////////////////////////////////////////////
		// DefaultStackFreeFunction
		//
		void TrackedStackFreeFunction(void* pData, void* /*pContext*/)
		{
			#if PPM_ENABLED
				EA::Allocator::gpEAGeneralAllocator->Free(pData);
			#else
				delete[] (char*)pData;
			#endif
		}


		///////////////////////////////////////////////////////////////////////////////
		// StackAllocator
		//
		StackAllocator::StackAllocator(void* pData,
									   size_t nInitialSize,
									   CoreAllocationFunction pCoreAllocationFunction,
									   CoreFreeFunction pCoreFreeFunction,
									   void* pCoreFunctionContext,
									   AssertionFailureInfoFunction pAssertionFailureFunction,
									   void* pAssertionFailureFunctionContext)
			: mnDefaultBlockSize(kDefaultBlockSize)
			, mpCurrentBlock(NULL)
			, mpCurrentBlockEnd(NULL)
			, mpCurrentObjectBegin(NULL)
			,
			#if PPM_INCREMENTAL_OBJECTS_ENABLED
			mpCurrentObjectEnd(NULL)
			,
			#endif
			mpCoreAllocationFunction(pCoreAllocationFunction ? pCoreAllocationFunction : DefaultStackAllocationFunction)
			, mpCoreFreeFunction(pCoreFreeFunction ? pCoreFreeFunction : DefaultStackFreeFunction)
			, mpCoreFunctionContext(pCoreFreeFunction ? pCoreFunctionContext : (void*)NULL)
			, mpTopBookmark(NULL)
			, mbMallocFailureAssertEnabled(false)
			, mpAssertionFailureFunction(pAssertionFailureFunction ? pAssertionFailureFunction : AssertionFailureFunctionDefault)
			, mpAssertionFailureFunctionContext(NULL)
		{
			if(nInitialSize != (size_t)-1)
				Init(pData, nInitialSize, pCoreAllocationFunction, pCoreFreeFunction, pCoreFunctionContext,
					 pAssertionFailureFunction, pAssertionFailureFunctionContext);
		}



		///////////////////////////////////////////////////////////////////////////////
		// ~StackAllocator
		//
		StackAllocator::~StackAllocator()
		{
			Shutdown();
		}



		///////////////////////////////////////////////////////////////////////////////
		// Init
		//
		void StackAllocator::Init(void* pData,
								  size_t nInitialSize,
								  CoreAllocationFunction pCoreAllocationFunction,
								  CoreFreeFunction pCoreFreeFunction,
								  void* pCoreFunctionContext,
								  AssertionFailureInfoFunction pAssertionFailureFunction,
								  void* pAssertionFailureFunctionContext)
		{
			PPM_ASSERT(this, nInitialSize || !pData, kGAErrorInternal, kGAErrorText[kGAErrorInternal], NULL, NULL); // It is bad to pass in a non-null pointer but say its size is zero.
			PPM_ASSERT(this, (nInitialSize == 0) || (nInitialSize >= sizeof(Block)), kGAErrorInternal, kGAErrorText[kGAErrorInternal], NULL, NULL);

			if(!mpCurrentBlock)
			{
				if(pCoreAllocationFunction)
					mpCoreAllocationFunction = pCoreAllocationFunction;
				if(pCoreFreeFunction)
					mpCoreFreeFunction = pCoreFreeFunction;
				mpCoreFunctionContext = pCoreFunctionContext;

				if(pAssertionFailureFunction)
					mpAssertionFailureFunction = pAssertionFailureFunction;
				if(pAssertionFailureFunctionContext)
					mpAssertionFailureFunctionContext = pAssertionFailureFunctionContext;


				if(nInitialSize == 0)
					nInitialSize = mnDefaultBlockSize;
				else if(nInitialSize < sizeof(Block))
					nInitialSize = sizeof(Block);

				if(pData == NULL)
					pData = (Block*)mpCoreAllocationFunction(nInitialSize, &nInitialSize, mpCoreFunctionContext); 

				if(pData)
				{
					mpCurrentBlock              = (Block*)pData;
					mpCurrentBlockEnd           = (char*)mpCurrentBlock + nInitialSize;
					mpCurrentBlock->mpEnd       = mpCurrentBlockEnd;
					mpCurrentBlock->mpPrevBlock = NULL; // This is so because this is the Init function.
					mpCurrentObjectBegin        = mpCurrentBlock->mData;
					#if PPM_INCREMENTAL_OBJECTS_ENABLED
						mpCurrentObjectEnd      = mpCurrentObjectBegin;
					#endif

					// Make sure the new data is aligned as needed.
					if((size_t)mpCurrentObjectBegin & kMinAlignmentMask)
					{
						mpCurrentObjectBegin = (char*)(((uintptr_t)mpCurrentObjectBegin + (uintptr_t)kMinAlignmentMask) & (uintptr_t)kMinAlignmentMaskInverse);
						#if PPM_INCREMENTAL_OBJECTS_ENABLED
							mpCurrentObjectEnd = mpCurrentObjectBegin;
						#endif
					}
				}
			}
		}


		///////////////////////////////////////////////////////////////////////////////
		// Shutdown
		//
		void StackAllocator::Shutdown()
		{
			FreeObjectsEx(NULL);
		}


		///////////////////////////////////////////////////////////////////////////////
		// Malloc
		//
		void* StackAllocator::Malloc(size_t nSize, bool bBoundsCheck)
		{
			#if PPM_INCREMENTAL_OBJECTS_ENABLED
				PPM_ASSERT(this, mpCurrentObjectEnd == mpCurrentObjectBegin, kGAErrorInternal, kGAErrorText[kGAErrorInternal], NULL, NULL);
			#endif

			nSize = (nSize + kMinAlignmentMask) & kMinAlignmentMaskInverse;

			// In an optimized build, the bBoundsCheck test is optimized away as 
			// long as it is passed as a constant. Tests with VC++ have shown that
			// this function boils down to just a few machine instructions in an
			// optimized build.

			// We do this the following way of checking because it deals with address space overflow as well as current block overflow.
			// We leave enough space after every allocation for the user to be able to put kMaxEquivalentBookmarkCount (arbitrary number) bookmarks after it.
			if(EA_UNLIKELY(bBoundsCheck && (((intptr_t)mpCurrentBlockEnd - ((intptr_t)mpCurrentObjectBegin + (intptr_t)nSize + (intptr_t)(kMaxEquivalentBookmarkCount * ((sizeof(Bookmark) + kMinAlignmentMask) & kMinAlignmentMaskInverse) ))) < 0)))
			{
				if(!AllocateNewBlock(nSize))
				{
					// This function has failed to allocate memory after all attempts.
					#if PPM_ASSERT_ENABLED
						if(mbMallocFailureAssertEnabled) // We don't currently have an AssertionFailure member function and so call PPM_ASSERT_STATIC instead of PPM_ASSERT.
						{
							PPM_ASSERT(this, mpCurrentObjectBegin == NULL /*unilaterally failing expression*/, kGAErrorInternal, kGAErrorText[kGAErrorInternal], NULL, NULL);
						}
					#endif

					return NULL;
				}
			}

			void* const pReturnValue = mpCurrentObjectBegin;
			mpCurrentObjectBegin += nSize;
			#if PPM_INCREMENTAL_OBJECTS_ENABLED
				 mpCurrentObjectEnd = mpCurrentObjectBegin;
			#endif
			PPM_ASSERT(this, ((size_t)mpCurrentObjectBegin & kMinAlignmentMask) == 0, kGAErrorInternal, kGAErrorText[kGAErrorInternal], NULL, NULL);
			return pReturnValue;
		}


		///////////////////////////////////////////////////////////////////////////////
		// Realloc
		//
		void* StackAllocator::Realloc(void* pData, size_t nNewSize, bool bBoundsCheck)
		{
			void* pReturnValue;

			#if PPM_INCREMENTAL_OBJECTS_ENABLED
				PPM_ASSERT(this, ((size_t)mpCurrentObjectBegin & kMinAlignmentMask) == 0, kGAErrorInternal, kGAErrorText[kGAErrorInternal], NULL, NULL);
			#endif

			if(pData)
			{
				if(nNewSize) // It's important that this check be on its own line.
				{
					Block* const pBlock    = StackAllocator::GetBlockForAddress(pData);
					void*  const pBlockEnd = pBlock->mpEnd;

					pReturnValue = Malloc(nNewSize, bBoundsCheck);
					if(nNewSize > (size_t)((char*)pBlockEnd - (char*)pData))
						nNewSize = (size_t)((char*)pBlockEnd - (char*)pData);
					if(pReturnValue)
						memcpy(pReturnValue, pData, nNewSize);
				}
				else
				{
					// In this case the standard calls for the same behaviour as free().
					pReturnValue = NULL;
				}
			}
			else
			{
				// In this case the standard calls for the same behaviour as malloc().
				pReturnValue = Malloc(nNewSize, bBoundsCheck);
			}

			return pReturnValue;
		}


		///////////////////////////////////////////////////////////////////////////////
		// MallocAligned
		//
		void* StackAllocator::MallocAligned(size_t nSize, size_t nAlignment, size_t nAlignmentOffset, bool bBoundsCheck)
		{
			PPM_ASSERT(this, (nAlignment & (nAlignment - 1)) == 0, kGAErrorInternal, kGAErrorText[kGAErrorInternal], NULL, NULL);
			PPM_ASSERT(this, (nAlignmentOffset & 7) == 0, kGAErrorInternal, kGAErrorText[kGAErrorInternal], NULL, NULL);
			#if PPM_INCREMENTAL_OBJECTS_ENABLED
				PPM_ASSERT(this, mpCurrentObjectEnd == mpCurrentObjectBegin, kGAErrorInternal, kGAErrorText[kGAErrorInternal], NULL, NULL);
			#endif

			// Malloc() relies on the current pointer being aligned to kMinAlignment,
			// so we must ensure at least that much here.
			nAlignment = ((nAlignment - 1) | kMinAlignmentMask) + 1;
			nSize        = (nSize + kMinAlignmentMask) & kMinAlignmentMaskInverse;

			char* pAlignedObjectBegin = (char*)(((size_t)((mpCurrentObjectBegin + nAlignmentOffset) + (nAlignment - 1)) & ~(nAlignment - 1)) - nAlignmentOffset);
			PPM_ASSERT(this, pAlignedObjectBegin >= mpCurrentObjectBegin, kGAErrorInternal, kGAErrorText[kGAErrorInternal], NULL, NULL); // Wraparound would be bad. Possibly do something about it.

			// In an optimized build, the bBoundsCheck test is optimized away as long as it is passed as a constant.
			if(bBoundsCheck && ((pAlignedObjectBegin + nSize + (intptr_t)(kMaxEquivalentBookmarkCount * ((sizeof(Bookmark) + kMinAlignmentMask) & kMinAlignmentMaskInverse) )) > mpCurrentBlockEnd))
			{
				if(AllocateNewBlock(nSize + nAlignment + nAlignmentOffset)) // Add extra space so that we know we can align in the new block.
				{
					pAlignedObjectBegin = (char*)(((size_t)((mpCurrentObjectBegin + nAlignmentOffset) + (nAlignment - 1)) & ~(nAlignment - 1)) - nAlignmentOffset);
					PPM_ASSERT(this, (pAlignedObjectBegin + nSize) <= mpCurrentBlockEnd, kGAErrorInternal, kGAErrorText[kGAErrorInternal], NULL, NULL);
				}
				else
				{
					// This function has failed to allocate memory after all attempts.
					#if PPM_ASSERT_ENABLED
						if(mbMallocFailureAssertEnabled) // We don't currently have an AssertionFailure member function and so call PPM_ASSERT_STATIC instead of PPM_ASSERT.
						{
							PPM_ASSERT(this, mpCurrentObjectBegin == NULL /*unilaterally failing expression*/, kGAErrorInternal, kGAErrorText[kGAErrorInternal], NULL, NULL);
						}
					#endif

					return NULL;
				}
			}

			mpCurrentObjectBegin = pAlignedObjectBegin + nSize;
			#if PPM_INCREMENTAL_OBJECTS_ENABLED
				mpCurrentObjectEnd = mpCurrentObjectBegin;
			#endif
			return pAlignedObjectBegin;
		}


		///////////////////////////////////////////////////////////////////////////////
		// GetMemoryUsage
		//
		size_t StackAllocator::GetMemoryUsage()
		{
			size_t nUsage = 0;

			Block* pBlock = mpCurrentBlock;

			if(pBlock)
			{
				#if PPM_INCREMENTAL_OBJECTS_ENABLED
					nUsage += (size_t)(mpCurrentObjectEnd - pBlock->mData);
				#else
					nUsage += (size_t)(mpCurrentObjectBegin - pBlock->mData);
				#endif

				pBlock = pBlock->mpPrevBlock;

				while(pBlock)
				{
					nUsage += (size_t)(pBlock->mpEnd - pBlock->mData); 
					pBlock = pBlock->mpPrevBlock;
				}
			}

			return nUsage;
		}


		void StackAllocator::SetOption(int option, int64_t nValue)
		{
			if(option == kOptionEnableMallocFailureAssert)
				 mbMallocFailureAssertEnabled = (nValue != 0);
		}


		int64_t StackAllocator::GetOption(int option) const
		{
			if(option == kOptionEnableMallocFailureAssert)
				 return mbMallocFailureAssertEnabled ? 1 : 0;

			return 0;
		}


		///////////////////////////////////////////////////////////////////////////////
		// ValidateAddress
		//
		const void* StackAllocator::ValidateAddress(const void* pAddress, int addressType) const
		{
			Block* pBlock = mpCurrentBlock;

			while(pBlock)
			{
				if(((uintptr_t)pAddress >= (uintptr_t)pBlock->mData) && // if pAddress belongs to the Block...
				   ((uintptr_t)pAddress  < (uintptr_t)pBlock->mpEnd))
				{
					if(addressType != kAddressTypeOwned)
					{
						// We treat kAddressTypeAllocated and kAddressTypeSpecific the same, as we currently can't know the difference between them.
						if((pBlock == mpCurrentBlock) && ((uintptr_t)pAddress >= (uintptr_t)mpCurrentBlock->mpEnd)) // If pAddress is beyond where we have allocated yet...
							return NULL;
					}
					// else fall through and always return true for pAddressTypeOwned.

					return pAddress;
				}
 
				pBlock = pBlock->mpPrevBlock;
			}

			return NULL;
		}


		///////////////////////////////////////////////////////////////////////////////
		// ValidateHeap
		//
		bool StackAllocator::ValidateHeap(HeapValidationLevel /*heapValidationLevel*/)
		{
			bool bResult = true;

			Bookmark* pBookmark = mpTopBookmark;

			while(pBookmark)
			{
				// To do: Enable this if/when we have code to make it so. Currently we don't.
				//Block* pBlockBookmark         = GetBlockForAddress(pBookmark);
				//Block* pBlockBookmarkLocation = GetBlockForAddress(pBookmark->mpCurrentObjectBegin);
				//if(pBlockBookmark != pBlockBookmarkLocation) // If the memory to store the bookmark is not in the same block as the memory the bookmark refers to...
				//    bResult = false;

				if(!ValidateAddress(pBookmark, kAddressTypeOwned)) // The bookmark memory itself should come from valid memory.
					bResult = false;

				if(!ValidateAddress(pBookmark->mpCurrentObjectBegin, kAddressTypeOwned)) // What the bookmark refers to should come from valid memory.
					bResult = false;

				#if PPM_INCREMENTAL_OBJECTS_ENABLED
					if(pBookmark->mpCurrentObjectEnd < pBookmark->mpCurrentObjectBegin) // If the bookmark is earlier in memory than the begin. Note that while Blocks in memory may be out of order, Objects being built are always within the same Block.
						bResult = false;

					if(!ValidateAddress(pBookmark->mpCurrentObjectEnd)) // What the bookmark refers to should come from valid memory.
						bResult = false;
				#endif

				if(pBookmark->mpPrevBookmark) // If we aren't at the last bookmark...
				{
					if(CompareAddresses(pBookmark, pBookmark->mpPrevBookmark) < 0) // If the pBookmark address itself is earlier in memory than mpPrevBookmark...
						bResult = false;

					if(CompareAddresses(pBookmark->mpCurrentObjectBegin, pBookmark->mpPrevBookmark->mpCurrentObjectBegin) < 0) // If pBookmark location is earlier in memory than mpPrevBookmark...
						bResult = false;
				}

				pBookmark = pBookmark->mpPrevBookmark;
			}

			return bResult;
		}


		///////////////////////////////////////////////////////////////////////////////
		// CompareAddresses
		//
		// Returns -1 if pAddr1 is less than pAddr2; 0 if equal, +1 if greater than.
		// Results are undefined if the addresses are not from this heap.
		//
		// We can't just compare addresses, 
		//
		int StackAllocator::CompareAddresses(const void* pAddr1, const void* pAddr2)
		{
			Block* pBlock      = mpCurrentBlock;
			int    blockIndex  = -1; // Equal to -1, -2, -3, etc.
			int    blockIndex1 =  0; // Zero until set to a value.
			int    blockIndex2 =  0;

			while(pBlock && (!blockIndex1 || !blockIndex2))
			{
				if(((uintptr_t)pAddr1 >= (uintptr_t)pBlock->mData) &&
				   ((uintptr_t)pAddr1  < (uintptr_t)pBlock->mpEnd))
				{
					blockIndex1 = blockIndex;
				}

				if(((uintptr_t)pAddr2 >= (uintptr_t)pBlock->mData) &&
				   ((uintptr_t)pAddr2  < (uintptr_t)pBlock->mpEnd))
				{
					blockIndex2 = blockIndex;
				}

				blockIndex--;
				pBlock = pBlock->mpPrevBlock;
			}

			if(blockIndex1 == blockIndex2)
			{
				if(pAddr1 < pAddr2)
					return -1;
				if(pAddr1 > pAddr2)
					return  1;
				return 0;
			}
			// Else compare blocks instead of addresses.

			if(blockIndex1 < blockIndex2)
				return -1;
			// else (blockIndex1 > blockIndex2)
			return  1;
		}


		///////////////////////////////////////////////////////////////////////////////
		// GetBlockForAddress
		//
		StackAllocator::Block* StackAllocator::GetBlockForAddress(const void* pAddress)
		{
			Block* pBlock = mpCurrentBlock;

			while(pBlock && ((pAddress < (void*)pBlock->mData) || (pAddress > (void*)pBlock->mpEnd)))
				pBlock = pBlock->mpPrevBlock;

			return pBlock;
		}


		///////////////////////////////////////////////////////////////////////////////
		// AllocateNewBlock
		//
		bool StackAllocator::AllocateNewBlock(size_t nExtraSize)
		{
			#if PPM_INCREMENTAL_OBJECTS_ENABLED
				const size_t nCurrentObjectSize = (size_t)(mpCurrentObjectEnd - mpCurrentObjectBegin);
			#else
				const size_t nCurrentObjectSize = 0;
			#endif

			// Calculate a new block size, which should be at least kDefaultBlockSize.
			size_t nNewBlockSize = (nCurrentObjectSize + nExtraSize + kMinAlignment) + (nCurrentObjectSize / 4) + (kDefaultBlockSize / 2);
			if(nNewBlockSize < kDefaultBlockSize)
				nNewBlockSize = kDefaultBlockSize;

			// Allocate the new block.
			PPM_ASSERT(this, mpCoreAllocationFunction, kGAErrorInternal, kGAErrorText[kGAErrorInternal], NULL, NULL);
			Block* const pNewBlock = (Block*)mpCoreAllocationFunction(nNewBlockSize, &nNewBlockSize, mpCoreFunctionContext);

			if(pNewBlock)
			{
				pNewBlock->mpPrevBlock = mpCurrentBlock;
				mpCurrentBlock         = pNewBlock;
				pNewBlock->mpEnd       = (char*)pNewBlock + nNewBlockSize;
				mpCurrentBlockEnd      = pNewBlock->mpEnd;

				// Make sure the new data is aligned as needed.
				char* pNewObjectBegin = pNewBlock->mData;
				if((size_t)pNewObjectBegin & kMinAlignmentMask)
					pNewObjectBegin = (char*)(((uintptr_t)pNewObjectBegin + (uintptr_t)kMinAlignmentMask) & (uintptr_t)kMinAlignmentMaskInverse);

				// Copy the old object data to its new location.
				if(nCurrentObjectSize && pNewObjectBegin && mpCurrentObjectBegin)
					memcpy(pNewObjectBegin, mpCurrentObjectBegin, nCurrentObjectSize);

				// Set the new block position.
				mpCurrentObjectBegin = pNewObjectBegin;
				#if PPM_INCREMENTAL_OBJECTS_ENABLED
					mpCurrentObjectEnd = pNewObjectBegin + nCurrentObjectSize;
				#endif

				return true;
			}

			return false;
		}


		///////////////////////////////////////////////////////////////////////////////
		// FreeObjectsEx
		//
		// Free all data allocated more recently than the data at input pAddress.
		// If input pAddress is NULL, then free all data.
		//
		void StackAllocator::FreeObjectsEx(const void* pAddress)
		{
			PPM_ASSERT(this, !pAddress || GetBlockForAddress(pAddress), kGAErrorInternal, kGAErrorText[kGAErrorInternal], NULL, NULL);

			Block* pBlock = mpCurrentBlock;

			// Looking through the latest version I have access to (1/09/2007 on EAOS), the only change 
			// I still have is that there is a potential crash if PushBookmark is used to push a bookmark 
			// address from an earlier block. When deleting blocks, FreeObjectsEx only checks to see if 
			// the current bookmark address is about to be deleted, and does not check the address of the 
			// actual bookmark struct itself. The fix is to add tests directly on "mpTopBookmark" as well 
			// as "mpTopBookmark->mpCurrentObjectBegin" inside FreeObjectsEx.

			// To consider: Verify that bookmarks are in order, that every succeeding bookmark
			// refers to a place that is after the previous bookmark. 

			// Starting from the top of the core block stack, free core blocks that don't
			// have pAddress in them.
			while(pBlock && ((pAddress < (void*)pBlock->mData) || (pAddress >= (void*)pBlock->mpEnd)))
			{
				// Starting from the top of the bookmark stack, free any bookmarks that 
				// refer to the current core block or come from the current core block.
				while((mpTopBookmark != NULL) && 
					  (mpTopBookmark->mpCurrentObjectBegin >= (void*)pBlock->mData) && 
					  (mpTopBookmark->mpCurrentObjectBegin <  (void*)pBlock->mpEnd) &&
					  ((void*)mpTopBookmark                >= (void*)pBlock->mData) &&  // Is it possible this would ever be false?
					  ((void*)mpTopBookmark                <  (void*)pBlock->mpEnd))
				{
					 mpTopBookmark = mpTopBookmark->mpPrevBookmark;
				}

				Block* const pBlockPrev = pBlock->mpPrevBlock;
				if(mpCoreFreeFunction)
					mpCoreFreeFunction(pBlock, mpCoreFunctionContext);
				pBlock = pBlockPrev;
			}

			if(pBlock) // If the user specified an address somewhere in the middle...
			{
				mpCurrentBlock       = pBlock;
				mpCurrentBlockEnd    = pBlock->mpEnd;
				mpCurrentObjectBegin = (char*)pAddress;
				#if PPM_INCREMENTAL_OBJECTS_ENABLED
					mpCurrentObjectEnd = mpCurrentObjectBegin;
				#endif
			}
			else
			{
				PPM_ASSERT(this, pAddress == NULL, kGAErrorInternal, kGAErrorText[kGAErrorInternal], NULL, NULL); // Assert that in fact the user meant to free all memory.

				// We set member data to all NULL. Note that if the user calls Malloc after this 
				// point in time then we are fine because the logic will simply allocate a new block.
				mpCurrentBlock       = NULL;
				mpCurrentBlockEnd    = NULL;
				mpCurrentObjectBegin = NULL;
				#if PPM_INCREMENTAL_OBJECTS_ENABLED
					mpCurrentObjectEnd = NULL;
				#endif
			}
		}

		void StackAllocator::SetAssertionFailureFunction(AssertionFailureInfoFunction pAssertionFailureFunction, void* pContext)
		{
			mpAssertionFailureFunction        = pAssertionFailureFunction;
			mpAssertionFailureFunctionContext = pContext;
		}

		void StackAllocator::AssertionFailure(const char* pExpression, int nErrorId, const char* pDescription, const void* pData, const void* pExtra) const
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

		void StackAllocator::AssertionFailureFunctionDefault(AssertionFailureInfo* pAssertionFailureInfo, void* pContext)
		{
			TraceFunctionDefault(pAssertionFailureInfo->mpDescription, pContext);
			TraceFunctionDefault("\n", pContext);
			PPM_DebugBreak();
		}

	} // namespace Allocator

} // namespace EA





















