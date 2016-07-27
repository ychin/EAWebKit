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
// EAIOZoneObject.cpp
// Copyright (c) 2007, Electronic Arts. All rights reserved.
//
// This file was copied from MemoryMan/EAIOZoneObject.cpp in order to avoid a 
// dependency on the MemoryMan package.
///////////////////////////////////////////////////////////////////////////////


#include <EAIO/PathString.h> // Currently for IO::GetAllocator()
#include <EAIO/internal/EAIOZoneObject.h>
#include <EAIO/internal/Config.h>


namespace EA
{
    namespace Allocator
    {

        void* EAIOZoneObject::operator new(size_t n)
        {
            ICoreAllocator* const pAllocator = IO::GetAllocator();
            return DoInternalAllocate(n, pAllocator, EAIO_ALLOC_PREFIX "EAIOZoneObject", 0);
        }


        void* EAIOZoneObject::operator new(size_t n, ICoreAllocator* pAllocator)
        {
            if(!pAllocator)
                pAllocator = IO::GetAllocator();
            return DoInternalAllocate(n, pAllocator, EAIO_ALLOC_PREFIX "EAIOZoneObject", 0);
        }


        void* EAIOZoneObject::operator new(size_t n, ICoreAllocator* pAllocator, const char* pName)
        {
            return DoInternalAllocate(n, pAllocator ? pAllocator : IO::GetAllocator(), pName, 0);
        }


        void* EAIOZoneObject::operator new(size_t n, const char* pName, const int flags, int /*debugFlags*/, const char* /*pFile*/, int /*pLine*/)
        {
            ICoreAllocator* const pCoreAllocator = IO::GetAllocator();
            return DoInternalAllocate(n, pCoreAllocator, pName, (unsigned int)flags);
        }


        /// \brief Helper function that allocates memory for operator new()
        void* EAIOZoneObject::DoInternalAllocate(size_t n, ICoreAllocator* pAllocator, const char* pName, unsigned int flags)
        {
            // We stash a pointer to the allocator before the actual object:
            //
            // +------------------------------------------+
            // |               memory block               |
            // |..........................................|
            // |alloc    . aligned object                 |
            // +------------------------------------------+
            //           ^
            //    aligned as requested
            //
            // This assumes that the object being allocated has no offset
            // requirement, only an alignment requirement. Currently this
            // is true of C/C++ aligned objects and RenderWare objects.

            static const size_t kAlignOfT = EA_ALIGN_OF(EAIOZoneObject);

            void* const p = pAllocator->Alloc(n + kOffset, pName, flags, kAlignOfT, kOffset);

            if(p)
            {
                ICoreAllocator** const pp = (ICoreAllocator**)p;
                *pp = pAllocator;

                // Return the start of the memory segment which is dedicated to the object itself.
                return (void*)((uintptr_t)pp + kOffset);
            }

            return NULL;
        }





        void EAIOZoneObject::operator delete(void* p)
        {
            DoInternalDeallocate(p);
        }


        void EAIOZoneObject::operator delete(void* p, ICoreAllocator* /*pAllocator*/)
        {
            DoInternalDeallocate(p);
        }


        void EAIOZoneObject::operator delete(void* p, ICoreAllocator* /*pAllocator*/, const char* /*pName*/)
        {
            DoInternalDeallocate(p);
        }


        void EAIOZoneObject::operator delete(void* p, const char* /*pName*/, const int /*flags*/, int /*debugFlags*/, 
            const char* /*pFile*/, int /*pLine*/)
        {
            DoInternalDeallocate(p);
        }


        /// \brief deletes an object using the allocator pointer stored 'kOffset' bytes before the object
        void EAIOZoneObject::DoInternalDeallocate(void* p)
        {
            if(p) // As per the C++ standard, deletion of NULL results in a no-op.
            {
                // DoInternalAllocaote() stashed a pointer to the allocator before the actual object:
                //
                // +------------------------------------------+
                // |               memory block               |
                // |..........................................|
                // |alloc    . aligned object                 |
                // +------------------------------------------+
                //           ^
                //           p
                //
                // Given 'p', compute true start of memory block
                ICoreAllocator** const pAllocatedBlockStart = (ICoreAllocator**)((char*)p - kOffset);

                // Start of block contains ICoreAllocator* responsible for that block
                ICoreAllocator*  const pStashedAllocator    = (ICoreAllocator*)(*pAllocatedBlockStart);

                // TODO: check that the pointer is not garbage
                // Something like this? (http://www.codeproject.com/macro/openvc.asp)
                //#if defined(EA_DEBUG)
                //    // validate address:
                //    if (FALSE == AfxIsValidAddress(pObj, sizeof(CObject), FALSE))
                //        return false;
                //
                //    // check to make sure the VTable pointer is valid
                //    void** vfptr = (void**)*(void**)pObj;
                //    if (!AfxIsValidAddress(vfptr, sizeof(void*), FALSE))
                //        return false;
                //
                //    // validate the first vtable entry
                //    void* pvtf0 = vfptr[0];
                //    if (IsBadCodePtr((FARPROC)pvtf0))
                //        return false;
                //#endif

                // Free the entire block (including space allocated for the ICoreAllocator*)
                pStashedAllocator->Free(pAllocatedBlockStart);
            }
        }

    }   // namespace Allocator
}   // namespace EA










