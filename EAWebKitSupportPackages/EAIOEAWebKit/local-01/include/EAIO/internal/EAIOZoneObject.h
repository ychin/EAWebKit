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
// EAIOZoneObject.h
// Copyright (c) 2007, Electronic Arts. All rights reserved.
//
// This file was copied from MemoryMan/EAIOZoneObject.cpp in order to avoid a 
// dependency on the MemoryMan package.
///////////////////////////////////////////////////////////////////////////////

///////////////////////////////////////////////////////////////////////////////
// Implements the EAIOZoneObject base class, which is mechanism whereby the 
// deleter of an object can 'delete' it without knowing the memory allocator 
// needed to free the object. This is particularly useful for reference 
// counting systems and the 'delete this' often seen in the Release or 
// DecrementRef function.
///////////////////////////////////////////////////////////////////////////////


#ifndef EAIO_ZONEOBJECT_H
#define EAIO_ZONEOBJECT_H


#include <EABase/eabase.h>
#include <EAIO/internal/Config.h>
#include <coreallocator/icoreallocator_interface.h>
#include <new>


///////////////////////////////////////////////////////////////////////////////
// ZONE_OBJECT_MIN_ALIGN
//
#ifndef ZONE_OBJECT_MIN_ALIGN
    #ifdef PPM_MIN_ALIGN
        #define ZONE_OBJECT_MIN_ALIGN PPM_MIN_ALIGN
    #else
        #define ZONE_OBJECT_MIN_ALIGN 8
    #endif
#endif


namespace EA
{
    namespace Allocator
    {

        ///////////////////////////////////////////////////////////////////////////////
        /// EAIOZoneObject
        ///
        /// A class derived from EAIOZoneObject is able to be allocated from different 
        /// heaps without the owner of the object needing to know or care which heap
        /// the object came from or even what kind of allocator the object is using.
        /// The owner of the object can use 'delete' or the object can call 'delete'
        /// on itself (useful for reference counting systems) and the object will be 
        /// freed via the correct allocator.
        ///
        /// Zone object uses the ICoreAllocator interface to allocate 
        /// and free.
        ///
        /// Zone objects have a virtual destructor and thus a vtable. However, the user
        /// doesn't need to use any virtual functions in derived classes.
        ///
        /// EAIOZoneObject-deriving classes can be created via EA_NEW, but don't have 
        /// to be created by it. They can be created by regular new as well.
        ///
        /// Example usage:
        ///     class Widget : public EAIOZoneObject
        ///     {
        ///         uint32_t mRefCount;
        ///
        ///         Widget()
        ///            : mRefCount(0) { }
        ///
        ///         void AddRef()
        ///             { ++mRefCount; }
        ///
        ///         void Release()
        ///         {
        ///             if(mRefCount > 1) 
        ///                 --mRefCount;
        ///             else
        ///                 delete this; // Will use ICoreAllocator to free self.
        ///         }
        ///     };
        ///
        ///     Widget* pWidget = new(pCoreAllocator) Widget;
        ///     pWidget->AddRef();
        ///     pWidget->Release();
        ///////////////////////////////////////////////////////////////////////////////
        class EAIO_API EAIOZoneObject
        {
        public:
            // Destructor must be virtual in order for the the correct value 
            // of 'this' to be passed to operator delete.
            virtual ~EAIOZoneObject() {}

            /// This version uses EA::IO::GetAllocator() to allocate the object. 
            /// Example usage:
            ///    Widget* pWidget = new Widget;
            static void* operator new(size_t n);
            static void  operator delete(void* p);

            /// Example usage:
            ///    Widget* pWidget = new(pCoreAllocator) Widget;
            static void* operator new(size_t n, ICoreAllocator* pAllocator);
            static void  operator delete(void* p, ICoreAllocator* pAllocator);

            /// Example usage:
            ///    Widget* pWidget = new(pCoreAllocator, "Widget") Widget;
            static void* operator new(size_t n, ICoreAllocator* pAllocator, const char* pName);
            static void  operator delete(void* p, ICoreAllocator* pAllocator, const char* pName);

            /// Overloads the operator new signature used by EA_NEW.
            /// Example usage:
            ///    Widget* pWidget = EA_NEW(pCoreAllocator) Widget;
            static void* operator new(size_t n, const char* pName, const int flags = 0, 
                                      int debugFlags = 0, const char* pFile = NULL, int pLine = 0);
            static void  operator delete(void* p, const char* pName, const int flags, 
                                         int debugFlags, const char* pFile, int pLine);

        protected:
            /// PPMalloc's minimum alignment is 8 bytes, so we must force at least that much.
            /// We assume that ICoreAllocator-implementing allocators allocate memory on at 
            /// least 8 byte boundaries as well.
            static const uint32_t kOffset = ZONE_OBJECT_MIN_ALIGN;

            static void* DoInternalAllocate(size_t n, ICoreAllocator* pAllocator, 
                const char* pName, unsigned int flags);
            static void  DoInternalDeallocate(void* p);
        };
    
    }   // namespace Allocator
}   // namespace EA


#endif  // Header include guard













