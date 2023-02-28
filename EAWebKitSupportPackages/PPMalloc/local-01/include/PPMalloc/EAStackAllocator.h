/*
Copyright (C) 2009-2010 Electronic Arts, Inc.  All rights reserved.

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
// Supported user-modifyable defines
//     PPM_INCREMENTAL_OBJECTS_ENABLED
//
///////////////////////////////////////////////////////////////////////////////


#ifndef PPMALLOC_EASTACKALLOCATOR_H
#define PPMALLOC_EASTACKALLOCATOR_H


#include <PPMalloc/internal/config.h>
#include <PPMalloc/internal/dllinfo.h>
#include <stddef.h>
#include <string.h>
#include <assert.h> 


/// namespace EA
///
/// This is the standard Electronic Arts C++ namespace.
///
namespace EA
{
    /// namespace Allocator
    ///
    /// This is the standard Electronic Arts memory allocator C++ namespace.
    ///
    namespace Allocator
    {
        ///////////////////////////////////////////////////////////////////////////////
        /// class StackAllocator
        ///
        /// Implements a memory stack, which allows you to allocate memory very 
        /// quickly. It works by starting with a block of memory and incrementing
        /// a pointer as the user makes allocation requests from it. Its primary
        /// advantage is that it is very fast and is very efficient with space. 
        /// Its primary disadvantage is that you cannot arbitrarily free the 
        /// memory you allocate from it. You can only free backwards from your
        /// most recent allocations. This allocator is nevertheless useful for 
        /// a number of situations.
        ///
        /// This kind of allocator is sometimes referred to as a LinearAllocator.
        ///
        /// Note that you can swap two StackAllocators via the std::swap function 
        /// or like so:
        ///     void swap(StackAllocator& s1, StackAllocator& s2)
        ///     {
        ///         StackAllocator temp(s1);
        ///         s1 = s2;
        ///         s2 = temp;
        ///     }
        /// 
        class PPM_API StackAllocator
        {
        public:
            typedef void* (*CoreAllocationFunction)(size_t nSize, size_t* pSizeResult, void* pContext);
            typedef void  (*CoreFreeFunction)(void* pBlock, void* pContext);

            /// StackAllocator
            /// @brief Class constructor
            ///
            /// The arguments for this class work the same as with the Init function.
            /// An nInitialSize of -1 (~0) causes no initial allocation to take place.
            StackAllocator(void* pData = NULL, size_t nInitialSize = (size_t)(~0), 
                           CoreAllocationFunction pCoreAllocationFunction = NULL, 
                           CoreFreeFunction pCoreFreeFunction = NULL, void* pCoreFunctionContext = NULL);

            /// StackAllocator
            /// @brief Class copy constructor
            ///
            /// Implements a conventional copy constructor
            StackAllocator(const StackAllocator& stackAllocator); 

            /// ~StackAllocator
            /// @brief Class destructor
            ~StackAllocator();

            /// operator=
            /// @brief Class assignment operator.
            ///
            /// Implements a coventional assignment operator.
            StackAllocator& operator=(const StackAllocator& stackAllocator); 

            /// Init
            /// @brief Initialize the memory stack.
            ///
            /// If the input pData is non-null, then it is assumed to have nInitialSize
            /// bytes of data. If pData is non-null, then memory is allocated internally
            /// of either nInitialSize bytes or a default value if nInitialSize is 0.
            /// nDefaultAlignment is the default alignment with which to allocate objects.
            /// pCoreAllocationFunction and pCoreFreeFunction are callback with which 
            /// more block memory will be allocated if it is exhausted. If these are 
            /// NULL then the default system allocator is used. 
            void Init(void* pData = NULL, size_t nInitialSize = 0, 
                      CoreAllocationFunction pCoreAllocationFunction = NULL, 
                      CoreFreeFunction pCoreFreeFunction = NULL, void* pCoreFunctionContext = NULL);

            /// Shutdown
            /// @brief Returns the instance to its originally constructed state.
            void Shutdown();

            /// Malloc
            /// @brief Allocate an object of a given size and default alignment.
            void* Malloc(size_t nSize, bool bBoundsCheck = true);

            /// Calloc
            /// @brief Allocate 'nElementCount' elements of 'nElementSize' bytes each, all initialized to 0.
            void* Calloc(size_t nElementCount, size_t nElementSize, bool bBoundsCheck = true);

            /// Realloc
            /// @brief Re-allocate the previously allocated block in p, making the new block nNewSize bytes in size.
            ///
            /// Due to the nature of this stack-like allocator, a new block is always created. 
            /// This function is not very efficient but is provided for compatibility with 
            /// conventional generalized allocators.
            void* Realloc(void* pData, size_t nNewSize, bool bBoundsCheck = true);

            /// MallocAligned
            /// @brief Allocate 'nSize' bytes allocated to 'nAlignment' bytes.
            ///
            /// Input 'nAlignment' must be a multiple power of 2. If it is not 
            /// a power of 2, the next greater power of 2 is used. The minimum 
            /// alignment returned is 8. This is much like the commonly seen 
            /// memalign function. The alignment offset parameter specifies 
            /// how many bytes into the allocation that the alignment should 
            /// be found.
            void* MallocAligned(size_t nSize, size_t nAlignment, size_t nAlignmentOffset = 0, bool bBoundsCheck = true);

            /// Free
            /// @brief Frees a given object which was allocated via Malloc, etc.
            ///
            /// This function is basically implemented as a no-op but is provided for
            /// compatibility with conventional generalized allocators. 
            void Free(void* pData);

            /// FreeObjects
            /// @brief Frees all memory that is after the input pAddress.
            ///
            /// This function acts as a rollback of the memory stack to a previous point in time. 
            /// The passed in pointer need not point to the base of a previously allocated object 
            /// but instead can refer to the middle of an object. However, the object which such 
            /// a use might split would be henceforth unusable.
            /// Any bookmarks that referred to space that was freed will be removed.
            void FreeObjects(const void* pAddress);

            /// Reset
            /// @brief Frees all space allocated by the user and clears all bookmarks.
            ///
            /// This function does not free internal core memory.
            void Reset();

            /// GetDefaultBlockSize
            /// @brief Returns the default block size used to allocate new core blocks.
            size_t GetDefaultBlockSize();

            /// SetDefaultBlockSize
            /// @brief Sets the default block size used to allocate new core blocks.
            void SetDefaultBlockSize(size_t nDefaultBlockSize);

            /// PushBookmark
            /// @brief Records an allocation position for a later call to PopBookmark.
            ///
            /// Returns the address the bookmark refers to or NULL if the operation 
            /// could not occur (typically due to an out-of-memory condition).
            /// If the input pAddress is NULL, the current stack top is used.
            /// PushBookmark must later be matched by a PopBookmark, or else a memory
            /// leak may occur. The matching PopBookmark calls must be called in the 
            /// reverse order of the PushBookmark calls, else a memory leak may occur.
            /// The same address may be used for PushBookmark multiple times in a row.
            void* PushBookmark(const void* pAddress = NULL);

            /// PopBookmark
            /// @brief Calls FreeObjects with the address that was last pushed via PushBookmark.
            void PopBookmark();

            /// GetTopBookmark
            /// @brief Returns the value of the most recently pushed bookmark.
            void* GetTopBookmark();

            /// GetMemoryUsage()
            /// @brief Returns the amount of memory currently allocated.
            size_t GetMemoryUsage();


            ////////////////////////////////////////////////////////////////////////////
            // Incremental object building functionality
            //
            // This is useful for building streams of data on the fly and is also 
            // useful for building static string tables or similar tables on the fly.
            ////////////////////////////////////////////////////////////////////////////
            #if PPM_INCREMENTAL_OBJECTS_ENABLED

                /// GetCurrentObjectBegin
                /// Returns the current value for the beginning of the current object. It is 
                /// possible that subsequent calls that append data to the current object could
                /// change the object's base position due to exhaustion of the current block.
                void* GetCurrentObjectBegin();

                /// GetCurrentObjectEnd
                /// Reurns the memory address of the memory right after the current object.
                /// This is the same as GetCurrentObjectBegin if no data has been appended 
                /// to the current object. It is also the address where the next data in the
                /// current object would be placed or where the next object would begin if 
                /// the current object were to be subsequently finished.
                void* GetCurrentObjectEnd();

                /// GetCurrentObjectSize
                /// Gets the size of the current object. The size of an object can continue
                /// to grow until FinishCurrentObject is called.
                size_t GetCurrentObjectSize();

                /// GetCurrentFreeSpace
                /// Returns the amount of space that is available in the current block for
                /// the current object. This function is useful for knowing how much space
                /// can be added to the current object before the current block is exhausted.
                size_t GetCurrentFreeSpace();

                /// ExtendCurrentObject
                /// @brief Increases the object currently being allocated by nSize bytes.
                /// 
                /// The data is uninitialized. If bBoundsCheck is false, then it is assumed 
                /// that the current object can be extended without running out of block 
                /// space and so bounds checks are skipped. It is intended that this function 
                /// be called with bBoundsCheck as a constant (e.g. 'true') expression, 
                /// as this function is implemented inline and a constant expression will 
                /// cause any optimizing compiler to discard all but the code that is 
                /// necessary to complete the operation.
                bool ExtendCurrentObject(size_t nSize, bool bBoundsCheck = true);

                /// AppendToCurrentObject
                /// This function is like ExtendCurrentObject except that the newly extended
                /// space is initialized with data pointed at by pData. As with ExtendCurrentObject,
                /// the bBoundsCheck argument should be a constant expression if you want to maximize
                /// efficiency.
                bool AppendToCurrentObject(const void* pData, size_t nSize, bool bBoundsCheck = true);

                /// AppendToCurrentObject
                /// This function is the same as AppendToCurrentObjectMemcpy except that the 
                /// object is copied by a C++-level object copy and placement operator new at 
                /// the destination. This allows a deep copy of an object. In using this 
                /// version you must ensure that the destination is aligned properly for
                /// the object. The copied object's constructor is called upon copy and
                /// thus it is possible that if the current object gets moved as a result
                /// of exhaustion of the current block space. As a result, objects appended
                /// via this means must execute code in their constructors which makes them
                /// dependent on their location at the time of the construction. 
                template<class T>
                bool AppendToCurrentObject(const T& t, bool bBoundsCheck = true);

                /// AppendToCurrentObjectMemcpy
                /// This function appends an arbitrary object to the object currently being 
                /// allocated. In this version of the function, the input object t is copied
                /// via memcpy and not by a C++-level object copy. 
                template<class T>
                bool AppendToCurrentObjectMemcpy(const T& t, bool bBoundsCheck = true);

                /// FinishCurrentObject. 
                /// Completes the allocation of the current object and starts a new object.
                void* FinishCurrentObject();

            #endif // PPM_INCREMENTAL_OBJECTS_ENABLED

        protected:
            // Internal data types
            enum Constants{
                kMinAlignment            = 8, /// Eight because that's the size of uint64_t and usually the size of double and long long.
                kMinAlignmentMask        = kMinAlignment - 1,
                kMinAlignmentMaskInverse = ~kMinAlignmentMask,
                kDefaultBlockSize        = 8192 
            };

            struct Block{
                Block* mpPrevBlock;  /// Address of previous block or NULL.
                char*  mpEnd;        /// Points to the end of this block (and thus the end of mData).
                char   mData[4];     /// Allocated data begins here.
            };

            struct Bookmark{
                 Bookmark*  mpPrevBookmark;
                 void*      mpCurrentObjectBegin;                 /// Address of object we are building.
                #if PPM_INCREMENTAL_OBJECTS_ENABLED
                     void*  mpCurrentObjectEnd;                   /// Where to add next bytes to current object.
                #endif
            };

            // Internal functions
            Block* GetBlockForAddress(const void* pAddress);
            bool   AllocateNewBlock(size_t nExtraSize); // Allocates a new core block with the assumption that nExtraSize bytes need to be added to the current object.
            void   FreeObjectsEx(const void* pAddress);

            // Member data
            size_t                  mnDefaultBlockSize;           /// Preferred size to allocate blocks in.
            Block*                  mpCurrentBlock;               /// Address of current Block.
            char*                   mpCurrentBlockEnd;            /// Address of char after current block.
            char*                   mpCurrentObjectBegin;         /// Address of object we are building.
            #if PPM_INCREMENTAL_OBJECTS_ENABLED
            char*                   mpCurrentObjectEnd;           /// Where to add next bytes to current object.
            #endif
            CoreAllocationFunction  mpCoreAllocationFunction;     /// Callback for allocating a new Block.
            CoreFreeFunction        mpCoreFreeFunction;           /// Callback for freeing an existing Block.
            void*                   mpCoreFunctionContext;        /// Context passed to CoreAllocationFunction, CoreFreeFunction.
            Bookmark*               mpTopBookmark;

        }; // class StackAllocator



        /// An alternative block allocation function that enables memory tracking.
        /// You should pass in the tracking name as the "pContext" parameter of
        /// the constructor.
        /// 
        /// Example usage:
        ///     StackAllocator sa(NULL, 0, TrackedStackAllocationFunction, TrackedStackFreeFunction, "My Custom Name");
        ///         or
        ///     StackAllocator sa(...);
        ///     sa.Init(NULL, 0, TrackedStackAllocationFunction, TrackedStackFreeFunction, "My Custom Name");
        ///
        void* TrackedStackAllocationFunction(size_t nSize, size_t* pSizeResult, void* pContext);
        void  TrackedStackFreeFunction(void* pData, void* pContext);



    } // namespace Allocator

} // namespace EA






namespace EA
{
    namespace Allocator
    {

        inline StackAllocator& StackAllocator::operator=(const StackAllocator& stackAllocator)
        {
            if(this != &stackAllocator)
                memcpy(this, &stackAllocator, sizeof(stackAllocator)); // A simple memcpy is all that's needed.
            return *this;
        }

        inline StackAllocator::StackAllocator(const StackAllocator& stackAllocator)
        {
            operator=(stackAllocator);
        }

        inline void* StackAllocator::Malloc(size_t nSize, bool bBoundsCheck)
        {
            #if PPM_INCREMENTAL_OBJECTS_ENABLED
                assert(mpCurrentObjectEnd == mpCurrentObjectBegin);
            #endif

            nSize = (nSize + kMinAlignmentMask) & kMinAlignmentMaskInverse;

            // In an optimized build, the bBoundsCheck test is optimized away as 
            // long as it is passed as a constant. Tests with VC++ have shown that
            // this function boils down to just a few machine instructions in an
            // optimized build.

            // We do this the following way of checking because it deals with address space overflow as well as current block overflow.
            if(EA_UNLIKELY(bBoundsCheck && (((intptr_t)mpCurrentBlockEnd - ((intptr_t)mpCurrentObjectBegin + (intptr_t)nSize)) < 0)))
            {
                if(!AllocateNewBlock(nSize))
                    return NULL;
            }

            void* const pReturnValue = mpCurrentObjectBegin;
            mpCurrentObjectBegin += nSize;
            #if PPM_INCREMENTAL_OBJECTS_ENABLED
                 mpCurrentObjectEnd = mpCurrentObjectBegin;
            #endif
            assert(((size_t)mpCurrentObjectBegin & kMinAlignmentMask) == 0);
            return pReturnValue;
        }


        inline void* StackAllocator::Calloc(size_t nElementCount, size_t nElementSize, bool bBoundsCheck)
        {
            const size_t nSize = nElementCount * nElementSize;
            void* const pReturnValue = Malloc(nSize, bBoundsCheck);

            if(!bBoundsCheck || pReturnValue) // In an optimized build, the bBoundsCheck test is optimized away as long as it is passed as a constant.
                memset(pReturnValue, 0, nSize);
            return pReturnValue;
        }


        inline void StackAllocator::Free(void* /*pData*/)
        {
            // We do nothing, as you can't free an individual object.
            // If you want to free memory, then you need to call FreeObjects.
        }


        inline void StackAllocator::FreeObjects(const void* pAddress)
        {
            if((pAddress > mpCurrentBlock) && (pAddress < (void*)mpCurrentBlockEnd))
            {
                mpCurrentObjectBegin = (char*)pAddress;
                #if PPM_INCREMENTAL_OBJECTS_ENABLED
                    mpCurrentObjectEnd = mpCurrentObjectBegin;
                #endif
            }
            else
                FreeObjectsEx(pAddress);
        }


        inline void StackAllocator::Reset()
        {
            // Currently we implement this via the brute-force Shutdown function.
            Shutdown();
        }


        inline size_t StackAllocator::GetDefaultBlockSize()
        {
            return mnDefaultBlockSize;
        }


        inline void StackAllocator::SetDefaultBlockSize(size_t nDefaultBlockSize)
        {
            mnDefaultBlockSize = nDefaultBlockSize;
        }


        inline void* StackAllocator::FinishCurrentObject()
        {
            void* const pReturnValue = mpCurrentObjectBegin;

            mpCurrentObjectEnd = (char*)(((size_t)mpCurrentObjectEnd + kMinAlignmentMask) & kMinAlignmentMaskInverse);
            if(mpCurrentObjectEnd > (char*)mpCurrentBlockEnd)
                mpCurrentObjectEnd = mpCurrentBlockEnd;
            mpCurrentObjectBegin = mpCurrentObjectEnd;
            return pReturnValue;
        }


        inline void* StackAllocator::PushBookmark(const void* pAddress)
        {
            Bookmark* const pBookmark = (Bookmark*)Malloc(sizeof(Bookmark));

            if(pBookmark)
            {
                void *pBegin, *pEnd;

                if(pAddress)
                    pBegin = pEnd = (void*)pAddress;
                else
                {
                    pBegin = mpCurrentObjectBegin;
                    pEnd   = mpCurrentObjectEnd;
                }

                #if PPM_INCREMENTAL_OBJECTS_ENABLED
                    if(mpCurrentObjectBegin != mpCurrentObjectEnd)
                        FinishCurrentObject();
                #endif

                pBookmark->mpPrevBookmark = mpTopBookmark;
                pBookmark->mpCurrentObjectBegin = pBegin;
                #if PPM_INCREMENTAL_OBJECTS_ENABLED
                    pBookmark->mpCurrentObjectEnd = pEnd;
                #endif
                mpTopBookmark = pBookmark;

                return pBegin;
            }

            return NULL;
        }


        inline void StackAllocator::PopBookmark()
        {
            assert(mpTopBookmark != NULL);

            if(mpTopBookmark != NULL)
            {
                void* const pCurrentObjectBegin = mpTopBookmark->mpCurrentObjectBegin;
                #if PPM_INCREMENTAL_OBJECTS_ENABLED
                    void* const pCurrentObjectEnd = mpTopBookmark->mpCurrentObjectEnd;
                #endif

                Bookmark* const pCurrentBookmark = mpTopBookmark;
                mpTopBookmark = mpTopBookmark->mpPrevBookmark;
                Free(pCurrentBookmark);

                FreeObjects(pCurrentObjectBegin);
                #if PPM_INCREMENTAL_OBJECTS_ENABLED
                    mpCurrentObjectEnd = (char*)pCurrentObjectEnd;
                #endif
             }
        }

        inline void* StackAllocator::GetTopBookmark()
        {
            return mpTopBookmark ? mpTopBookmark->mpCurrentObjectBegin : NULL;
        }


        #if PPM_INCREMENTAL_OBJECTS_ENABLED

        inline void* StackAllocator::GetCurrentObjectBegin()
        {
            return mpCurrentObjectBegin;
        }


        inline void* StackAllocator::GetCurrentObjectEnd()
        {
            return mpCurrentObjectEnd;
        }


        inline size_t StackAllocator::GetCurrentObjectSize()
        {
            return (size_t)(mpCurrentObjectEnd - mpCurrentObjectBegin);
        }


        inline size_t StackAllocator::GetCurrentFreeSpace()
        {
            return (size_t)(mpCurrentBlockEnd - mpCurrentObjectEnd);
        }


        inline bool StackAllocator::ExtendCurrentObject(size_t nSize, bool bBoundsCheck)
        {
            // In an optimized build, the bBoundsCheck test is optimized away as long as it is passed as a constant.
            if(EA_UNLIKELY(bBoundsCheck && ((mpCurrentObjectEnd + nSize) > mpCurrentBlockEnd)))
            {
                if(!AllocateNewBlock(nSize))
                    return false;
            }

            mpCurrentObjectEnd += nSize;
            return true;
        }


        inline bool StackAllocator::AppendToCurrentObject(const void* pData, size_t nSize, bool bBoundsCheck)
        {
            // In an optimized build, the bBoundsCheck test is optimized away as long as it is passed as a constant.
            if(EA_UNLIKELY(bBoundsCheck && ((mpCurrentObjectEnd + nSize) > mpCurrentBlockEnd)))
            {
                if(!AllocateNewBlock(nSize))
                    return false;
            }

            memcpy(mpCurrentObjectEnd, pData, nSize);
            mpCurrentObjectEnd += nSize;
            return true;
        }


        template <class T>
        inline bool StackAllocator::AppendToCurrentObjectMemcpy(const T& t, bool bBoundsCheck)
        {
            // In an optimized build, the bBoundsCheck test is optimized away as long as it is passed as a constant.
            if(EA_UNLIKELY(bBoundsCheck && ((mpCurrentObjectEnd + sizeof(T)) > mpCurrentBlockEnd)))
            {
                if(!AllocateNewBlock(sizeof(T)))
                    return false;
            }

            memcpy(mpCurrentObjectEnd, &t, sizeof(T));
            mpCurrentObjectEnd += sizeof(T);
            return true;
        }


        template <class T>
        inline bool StackAllocator::AppendToCurrentObject(const T& t, bool bBoundsCheck)
        {
            // In an optimized build, the bBoundsCheck test is optimized away as long as it is passed as a constant.
            if(EA_UNLIKELY(bBoundsCheck && ((mpCurrentObjectEnd + sizeof(T)) > mpCurrentBlockEnd)))
            {
                if(!AllocateNewBlock(sizeof(T)))
                    return false;
            }

            new(mpCurrentObjectEnd) T(t);      // We assume the destination is properly aligned for T. 
            mpCurrentObjectEnd += sizeof(T);  // For built-in types like 'int', the above 'new' statement becomes just an assignment.
            return true;
        }


        #endif // PPM_INCREMENTAL_OBJECTS_ENABLED


    }  // namespace Allocator

} // namespace EA


#endif // PPMALLOC_EASTACKALLOCATOR_H

















