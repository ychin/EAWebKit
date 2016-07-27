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

/////////////////////////////////////////////////////////////////////////////
// EAStreamMemory.h
//
// Copyright (c) 2007, Electronic Arts Inc. All rights reserved.
// Written by Paul Pedriana
//
// Implements a IO stream that reads and writes to a block of memory.
/////////////////////////////////////////////////////////////////////////////


#if !defined(EAIO_EASTREAMMEMORY_H) && !defined(FOUNDATION_EASTREAMMEMORY_H)
#define EAIO_EASTREAMMEMORY_H
#define FOUNDATION_EASTREAMMEMORY_H // For backward compatability. Eventually, we'll want to get rid of this.


#include <EAIO/internal/Config.h>
#ifndef EAIO_EASTREAM_H
    #include <EAIO/EAStream.h>
#endif
#ifndef EAIO_ZONEOBJECT_H
    #include <EAIO/internal/EAIOZoneObject.h>
#endif
#include <string.h>
#include <stddef.h>



/// namespace EA
/// Standard Electronic Arts Framework namespace.
namespace EA
{
    /// namespace IO
    /// The namespace for general IO (input/output) functionality.
    namespace IO
    {
        /// SharedPointer
        ///
        /// Implements a basic ref-counted pointer.
        ///
        /// This class is meant to be used like a COM object. When this object's 
        /// reference count goes to zero, the memory it holds is deleted and then
        /// this object calls 'delete this'. This class is similar to but doesn't
        /// work exactly the same as Boost's shared_ptr template.
        /// A typical usage pattern is like so:
        ///     SharedPointer* pSP = new SharedPointer(1000);
        ///     pSP->AddRef();
        ///     pSP->Release();
        ///
        class EAIO_API SharedPointer : public EA::Allocator::EAIOZoneObject
        {
        public:
            typedef EA::Allocator::ICoreAllocator Allocator;

            SharedPointer(void* pData = NULL, bool bFreeData = true, Allocator* pAllocator = NULL);
            SharedPointer(size_type nSize, const char* pName = NULL);
            SharedPointer(size_type nSize, Allocator* pAllocator, const char* pName = NULL);

            void*      GetPointer();
            int        AddRef();
            int        Release();
            Allocator* GetAllocator() const;

        protected:
            Allocator*  mpAllocator;
            uint8_t*    mpData;
            int         mnRefCount;
            bool        mbFreeData; // If true, we free the data when done.
        };


        /// class MemoryIOStream
        ///
        /// Implements an memory-based stream that supports the IStream interface.
        ///
        /// This class is not inherently thread-safe. As a result, thread-safe usage 
        /// between multiple threads requires higher level coordination, such as a mutex.
        ///
        class EAIO_API MemoryStream : public IStream
        {
        public:
            typedef EA::Allocator::ICoreAllocator Allocator;

            enum { kTypeMemoryStream = 0x347223d2 };

            /// enum Options
            /// Specifies policies regarding the internal operation of this class.
            enum Options
            {
                kOptionNone            =  0,    /// No options
                kOptionResizeEnabled   =  1,    /// 0 or 1. Default is disabled.  If set, then the buffer is automatically resized on beyond-bounds position sets, beyond-bounds writes, and beyond-bounds SetSize calls.
                kOptionResizeFactor    =  4,    /// 1.0+    Default is 1.5.       Specifies how much a resize multiplies in size; is applied before kOptionResizeIncrement. Can be 1.0 if kOptionResizeIncrement > 0.
                kOptionResizeIncrement =  8,    /// 0.0+    Default is 0.0.       Specifies how much a resize increments; is applied after kOptionResizeFactor. Can be set to zero if kOptionResizeFactor is > 1.
                kOptionResizeMaximum   = 16     /// 0+      Default is 0.         Specifies the maximum size (0 = unlimited).
              //kOptionClearNewMemory  = 32     /// 0 or 1. Default is 0.         If set, then newly allocated space is cleared to zero. Otherwise the space is left as it comes from the memory allocator.
            };

            MemoryStream(SharedPointer* pSharedPointer = NULL, size_type nSize = 0, const char* pName = NULL);
            MemoryStream(void* pData, size_type nSize, bool bUsePointer, bool bFreePointer = true, Allocator* pAllocator = NULL, const char* pName = NULL); // If bUsePointer is true, then we take over ownership of pData instead of copying from it.
            MemoryStream(const MemoryStream& memoryStream);
            virtual ~MemoryStream();

            int     AddRef();
            int     Release();

            float   GetOption(Options option) const;
            void    SetOption(Options option, float fValue);

            void       SetAllocator(Allocator* pAllocator);
            Allocator* GetAllocator() const;

            void*   GetData() const;
            bool    SetData(SharedPointer* pSharedPointer, size_type nSize);
            bool    SetData(void* pData, size_type nSize, bool bUsePointer, bool bFreePointer = true, Allocator *pAllocator = NULL);

            SharedPointer* GetSharedPointer();

            size_type   GetCapacity() const;
            bool        SetCapacity(size_type size);

            // IStream
            uint32_t    GetType() const;
            int         GetAccessFlags() const;
            int         GetState() const;
            bool        Close();

            size_type   GetSize() const;
            bool        SetSize(size_type size);

            off_type    GetPosition(PositionType positionType = kPositionTypeBegin) const;
            bool        SetPosition(off_type position, PositionType positionType = kPositionTypeBegin);

            size_type   GetAvailable() const;
            size_type   Read(void* pData, size_type nSize);

            bool        Flush();
            bool        Write(const void* pData, size_type nSize);

        protected:
            bool Realloc(size_type nSize);

            SharedPointer* mpSharedPointer;     /// Pointer to memory block.
            Allocator*     mpAllocator;         /// Allocator.
            const char*    mpName;              /// Memory allocation name.
            int            mnRefCount;          /// Reference count. May or may not be in use.
            size_type      mnSize;              /// The size of the stream, in bytes.
            size_type      mnCapacity;          /// The size of the memory buffer, in bytes.
            size_type      mnPosition;          /// Current position within memory block.
          //bool           mbClearNewMemory;    /// True if clearing of newly allocated memory is enabled.
            bool           mbResizeEnabled;     /// True if resizing is enabled.
            float          mfResizeFactor;      /// Specifies how capacity is increased.
            int            mnResizeIncrement;   /// Specifies how capacity is increased.
            int            mnResizeMax;         /// Maximum resize amount
        };

    } // namespace IO

} // namespace EA





///////////////////////////////////////////////////////////////////////////////
// Inlines
///////////////////////////////////////////////////////////////////////////////


///////////////////////////////////////////////////////////////////////////////
// SharedPointer
///////////////////////////////////////////////////////////////////////////////

inline void* EA::IO::SharedPointer::GetPointer()
{
    return mpData;
}


inline EA::IO::SharedPointer* EA::IO::MemoryStream::GetSharedPointer()
{
    return mpSharedPointer;
}


inline int EA::IO::SharedPointer::AddRef()
{
    return ++mnRefCount;
}


inline EA::IO::SharedPointer::Allocator* EA::IO::SharedPointer::GetAllocator() const
{
    return mpAllocator;
}



///////////////////////////////////////////////////////////////////////////////
// MemoryStream
///////////////////////////////////////////////////////////////////////////////

inline EA::IO::MemoryStream::~MemoryStream()
{
    if(mpSharedPointer)
        mpSharedPointer->Release();
}


inline int EA::IO::MemoryStream::AddRef()
{
    return ++mnRefCount;
}


inline int EA::IO::MemoryStream::Release()
{
    if(mnRefCount > 1)
        return --mnRefCount;
    delete this;
    return 0;
}


inline void EA::IO::MemoryStream::SetAllocator(Allocator* pAllocator)
{
    mpAllocator = pAllocator;
}


inline EA::IO::MemoryStream::Allocator* EA::IO::MemoryStream::GetAllocator() const
{
    return mpAllocator;
}


inline void* EA::IO::MemoryStream::GetData() const
{
    if(mpSharedPointer)
        return mpSharedPointer->GetPointer();
    return 0;
}


inline EA::IO::size_type EA::IO::MemoryStream::GetCapacity() const
{
    return mnCapacity;
}


inline uint32_t EA::IO::MemoryStream::GetType() const
{
    return kTypeMemoryStream;
}


inline int EA::IO::MemoryStream::GetAccessFlags() const
{
    return kAccessFlagReadWrite;
}


inline int EA::IO::MemoryStream::GetState() const
{
    return kStateSuccess;
}


inline bool EA::IO::MemoryStream::Close()
{
    return SetData(NULL, 0, false);
}


inline EA::IO::size_type EA::IO::MemoryStream::GetSize() const
{
    return mnSize;
}


inline EA::IO::size_type EA::IO::MemoryStream::GetAvailable() const
{
    // assert(mnPosition <= mnSize);
    return (mnSize - mnPosition);
}


inline bool EA::IO::MemoryStream::Flush()
{
    // Nothing to do.
    return true;
}




#endif // Header include guard







