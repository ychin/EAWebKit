/*
Copyright (C) 2009 Electronic Arts, Inc.  All rights reserved.

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


#ifndef EA_ALLOCATOR_ICOREALLOCATOR_INTERFACE_H
#define EA_ALLOCATOR_ICOREALLOCATOR_INTERFACE_H

#ifdef _MSC_VER
#pragma once
#endif

/// @brief This header file defines the ICoreAllocator interface class.

#include <stddef.h>     // size_t definition

#ifndef ICOREALLOCATOR_INTERFACE_API
	#if defined(EA_DLL) && defined(_MSC_VER)
		// For a DLL, mark up the ICoreAllocator interface class for
		// DLL export.  Even though there is no functionality to export,
		// doing this allows clients to not suppress warning C4275
		// for their ICoreAllocator implementation.
		#define ICOREALLOCATOR_INTERFACE_API __declspec(dllexport)
	#else

		// We are not making a DLL, so this macro
		// should define to nothing in this case.
		#define ICOREALLOCATOR_INTERFACE_API

	#endif
#endif

namespace EA
{
    namespace Allocator
    {
        //------------------------------------------------------
        //          ICoreAllocator
        //
        // Base level abstract interface designed for allocating and
        // deallocating memory.
        //------------------------------------------------------

        // ideally, the implementation has the same flags so no flag parsing
        // is necessary per allocation.
        enum AllocFlags
        {
            /// hint to the heap implementation that this
            /// block will be freed shortly after it was
            /// allocated.  To reduce heap fragmentation,
            /// these blocks are usually allocated at the
            /// opposite end of the heap from where
            /// permanent allocations are made.
            MEM_TEMP = 0,

            /// Normal block of memory which will need to
            /// be retained for a long time.
            MEM_PERM = 1
        };

        /// ICoreAllocator is the base level abstract
        /// interface for allocating and deallocating memory.
        class ICoreAllocator
        {
        public:

            /// Virtual destructor is required so if the allocator is deleted
            /// as the base type, the derived class' destructor is called.
            virtual ~ICoreAllocator() {}

            /// @param size:    size of the requested block.
            /// @param name:    name of the block (normally used for debugging purposes)
            /// @param flags:   additional information on how to allocate the block (MEM_PERM or MEM_TEMP)
            ///
            /// @return A pointer to the allocated block if successful.
            /// NULL if the block could not be allocated.
            ///
            /// @remarks
            /// The ICoreAllocator::Alloc function allocates a memory
            /// block of at least size bytes.  Depending on the
            /// implementation, the block may be larger than size bytes
            /// because of space required for alignment and maintenance
            /// information.  The 'name' parameter is not guaranteed to
            /// be used by the implementation and is very likely to be
            /// ignored in a release build.  Use the aligned version of
            /// Alloc if you want to get a memory block aligned to a
            /// particular power-of-2 boundary.
            ///
            /// @par Requirements:
            /// header:     icoreallocator/icoreallocator_interface.h \n
            ///
            /// @sa
            /// Free

            virtual void *Alloc(size_t size, const char *name, unsigned int flags) = 0;

            /// @param size:    size of the requested block.
            /// @param name:    name of the block (normally used for debugging purposes)
            /// @param flags:   additional information on how to allocate the block (MEM_PERM or MEM_TEMP)
            /// @param align:   some power-of-2 boundary the block must be allocated on.
            /// address returned is guaranteed to be aligned to the requested boundary.
            /// @param alignOffset: a "header" amount which is subtracted from the
            /// power-of-2 alignment.  A location inside a memory block will be aligned
            /// to this boundary.
            ///
            /// @return A pointer to the allocated block if successful.
            /// NULL if the block could not be allocated.
            ///
            /// @remarks
            /// This function is the same as Alloc(size_t, const char *, unsigned int),
            /// but allows for a block to aligned to a specified power-of-2 boundary.
            ///
            /// @par Requirements:
            /// header:     icoreallocator/icoreallocator_interface.h \n
            ///
            /// @sa
            /// Free

            virtual void *Alloc(size_t size, const char *name, unsigned int flags,
                unsigned int align, unsigned int alignOffset = 0) = 0;

            /// @param block:   pointer to the block to be freed.
            /// @param size:    optional.  It may be used to find the pool
            /// that the block belongs to.  The implementation is free to
            /// ignore this and the caller cannot assume that the
            /// implementation will do anything with it.
            ///
            /// @remarks
            /// ICoreAllocator::Free frees a block of memory previously
            /// allocated through a  call to ICoreAllocator::Alloc. The
            /// number of bytes freed equals the number of bytes that
            /// were allocated.  After the call, the memory block pointed
            /// to by "block" is invalid and can no longer be used.
            ///
            /// @sa
            /// Alloc

            virtual void Free(void *block, size_t size=0) = 0;

            /// @remarks
            /// Holds name, file, and line number to be associated with an allocation.

            struct DebugParams
            {
                const char* const       mName;
                const char* const       mFileName;
                const int               mLineNum;
                const int               mPad;

                DebugParams(const char* const allocationName, const char* const fileName, const int lineNum) 
                    : mName(allocationName), mFileName(fileName), mLineNum(lineNum), mPad(0) 
                {}

                DebugParams(const DebugParams& obj) 
                    : mName(obj.mName), mFileName(obj.mFileName), mLineNum(obj.mLineNum), mPad(0) 
                {}

                private:
                //private and undefined (members are all const so assignment isn't possible) ... this is here so the compiler doesn't try to generate it.
                DebugParams& operator=(const DebugParams& rhs);
            };

            /// @param size:    size of the requested block.
            /// @param debugParms:    name of the block + file & line the allocation is coming from (normally used for debugging purposes)
            /// @param flags:   additional information on how to allocate the block (MEM_PERM or MEM_TEMP)
            ///
            /// @return A pointer to the allocated block if successful.
            /// NULL if the block could not be allocated.
            ///
            /// @remarks
            /// This is just a version of alloc that allows the user to supply additional debug information associated with the allocation.

            virtual void *AllocDebug(size_t size, const DebugParams debugParams, unsigned int flags)
            {
                return Alloc(size,debugParams.mName,flags);
            }

            /// @param size:    size of the requested block.
            /// @param debugParms:    name of the block + file & line the allocation is coming from (normally used for debugging purposes)
            /// @param flags:   additional information on how to allocate the block (MEM_PERM or MEM_TEMP)
            /// @param align:   some power-of-2 boundary the block must be allocated on.
            /// address returned is guaranteed to be aligned to the requested boundary.
            /// @param alignOffset: a "header" amount which is subtracted from the
            /// power-of-2 alignment.  A location inside a memory block will be aligned
            /// to this boundary.
            ///
            /// @return A pointer to the allocated block if successful.
            /// NULL if the block could not be allocated.
            ///
            /// @remarks
            /// This is just a version of alloc that supports alignment and allows the user to supply additional debug information associated with the allocation.

            virtual void *AllocDebug(size_t size, const DebugParams debugParams, unsigned int flags, unsigned int align, unsigned int alignOffset = 0)
            {
                return Alloc(size,debugParams.mName,flags,align,alignOffset);
            }

            /// The user is responsible for providing an implementation of
            /// EA::Allocator::ICoreAllocator::GetDefaultAllocator in their
            /// applications' codebase.  The package contains
            /// implementations of operator new and delete which call this
            /// function.  Library authors should not call this function.
            static ICOREALLOCATOR_INTERFACE_API ICoreAllocator *GetDefaultAllocator();
        };
    }
}

#endif
