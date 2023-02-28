/*
Copyright (C) 2004,2009-2010 Electronic Arts, Inc.  All rights reserved.

Redistribution and use in source and binary forms, with or without
modification, are permitted provided that the following conditions
are met:

1.  Redistributions of source code must retain the above copyright
    notice, this list of conditions and the following disclaimer.
2.  Redistributions in binary form must reproduce the above copyright
    notice, this list of conditions and the following disclaimer in the
    documentation and/or other materials provided with the distribution.
3.  Neither the name of Electronic Arts, Inc. ("EA") nor the names of
    its contributors may be used to endorse or promote products derived
    from this software without specific prior written permission.

THIS SOFTWARE IS PROVIDED BY ELECTRONIC ARTS AND ITS CONTRIBUTORS "AS IS" AND ANY
EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE IMPLIED
WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE ARE
DISCLAIMED. IN NO EVENT SHALL ELECTRONIC ARTS OR ITS CONTRIBUTORS BE LIABLE FOR ANY
DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES
(INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES;
LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND
ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT
(INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE OF
THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
*/

///////////////////////////////////////////////////////////////////////////////
// EAStackAllocator
//
// By Paul Pedriana - 2004
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
// PPM_ENABLED (defined as 0 or 1, default is 1)
//
// When this is enabled, this module recognizes PPMalloc modules external to 
// this module. If disabled, then this module has no dependencies on anything
// except standard C/C++ libraries and you can thus use it with other
// allocation systems.
//


#ifndef PPM_ENABLED
    #define PPM_ENABLED 0
#endif
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
            (void)pContext;
            #if PPM_ENABLED
                #if defined(EA_MEMORY_DEBUG_ENABLED) && EA_MEMORY_DEBUG_ENABLED
                     void* const pReturnValue = EA::Allocator::gpEAGeneralAllocatorDebug->MallocDebug( nSize, 0, 0, (const char8_t *)pContext );
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
        StackAllocator::StackAllocator(void* pData, size_t nInitialSize, 
                                       CoreAllocationFunction pCoreAllocationFunction, 
                                       CoreFreeFunction pCoreFreeFunction, void* pCoreFunctionContext)
          : mnDefaultBlockSize(kDefaultBlockSize),
            mpCurrentBlock(NULL),
            mpCurrentBlockEnd(NULL),
            mpCurrentObjectBegin(NULL),
            #if PPM_INCREMENTAL_OBJECTS_ENABLED
            mpCurrentObjectEnd(NULL),
            #endif
            mpCoreAllocationFunction(pCoreAllocationFunction ? pCoreAllocationFunction : DefaultStackAllocationFunction),
            mpCoreFreeFunction(pCoreFreeFunction ? pCoreFreeFunction : DefaultStackFreeFunction),
            mpCoreFunctionContext(NULL),
            mpTopBookmark(NULL)
        {
            if(nInitialSize != (size_t)-1)
                Init(pData, nInitialSize, pCoreAllocationFunction, pCoreFreeFunction, pCoreFunctionContext);
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
        void StackAllocator::Init(void* pData, size_t nInitialSize, 
                                  CoreAllocationFunction pCoreAllocationFunction, 
                                  CoreFreeFunction pCoreFreeFunction, void* pCoreFunctionContext)
        {
            assert(nInitialSize || !pData); // It is bad to pass in a non-null pointer but say its size is zero.
            assert((nInitialSize == 0) || (nInitialSize >= sizeof(Block)));

            if(!mpCurrentBlock)
            {
                if(pCoreAllocationFunction)
                    mpCoreAllocationFunction = pCoreAllocationFunction;
                if(pCoreFreeFunction)
                    mpCoreFreeFunction = pCoreFreeFunction;
                mpCoreFunctionContext = pCoreFunctionContext;

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
        // Realloc
        //
        void* StackAllocator::Realloc(void* pData, size_t nNewSize, bool bBoundsCheck)
        {
            void* pReturnValue;

            #if PPM_INCREMENTAL_OBJECTS_ENABLED
                assert(mpCurrentObjectEnd == mpCurrentObjectBegin);
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
            assert((nAlignment & (nAlignment - 1)) == 0);
            assert((nAlignmentOffset & 7) == 0);
            #if PPM_INCREMENTAL_OBJECTS_ENABLED
                assert(mpCurrentObjectEnd == mpCurrentObjectBegin);
            #endif

            // Malloc() relies on the current pointer being aligned to kMinAlignment,
            // so we must ensure at least that much here.
            nAlignment = ((nAlignment - 1) | kMinAlignmentMask) + 1;
            nSize        = (nSize + kMinAlignmentMask) & kMinAlignmentMaskInverse;

            char* pAlignedObjectBegin = (char*)(((size_t)((mpCurrentObjectBegin + nAlignmentOffset) + (nAlignment - 1)) & ~(nAlignment - 1)) - nAlignmentOffset);
            assert(pAlignedObjectBegin >= mpCurrentObjectBegin); // Wraparound would be bad. Possibly do something about it.

            // In an optimized build, the bBoundsCheck test is optimized away as long as it is passed as a constant.
            if(bBoundsCheck && ((pAlignedObjectBegin + nSize) > mpCurrentBlockEnd))
            {
                if(AllocateNewBlock(nSize + nAlignment + nAlignmentOffset)) // Add extra space so that we know we can align in the new block.
                {
                    pAlignedObjectBegin = (char*)(((size_t)((mpCurrentObjectBegin + nAlignmentOffset) + (nAlignment - 1)) & ~(nAlignment - 1)) - nAlignmentOffset);
                    assert((pAlignedObjectBegin + nSize) <= mpCurrentBlockEnd);
                }
                else
                    return NULL;
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
                    nUsage += (size_t)(pBlock->mpEnd - mpCurrentObjectEnd);
                #else
                    nUsage += (size_t)(pBlock->mpEnd - mpCurrentObjectBegin);
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
            assert(mpCoreAllocationFunction);
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
            assert(!pAddress || GetBlockForAddress(pAddress));

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
            while(pBlock && ((pAddress < (void*)pBlock->mData) || (pAddress > (void*)pBlock->mpEnd)))
            {
                // Starting from the top of the bookmark stack, free any bookmarks that 
                // refer to the current core block or come from the current core block.
                while((mpTopBookmark != NULL) && 
                      (mpTopBookmark->mpCurrentObjectBegin >= (void*)pBlock->mData) && 
                      (mpTopBookmark->mpCurrentObjectBegin <  (void*)pBlock->mpEnd) &&
                      ((void*)mpTopBookmark                >= (void*)pBlock->mData) && 
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
                assert(pAddress == NULL); // Assert that in fact the user meant to free all memory.

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

    } // namespace Allocator

} // namespace EA





















