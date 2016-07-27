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
// EAStreamBuffer.h
//
// Copyright (c) 2007, Electronic Arts Inc. All rights reserved.
// Written by Paul Pedriana
//
// Implements smart buffering around a random access IO stream.
/////////////////////////////////////////////////////////////////////////////


#if !defined(EAIO_EASTREAMBUFFFER_H) && !defined(FOUNDATION_EASTREAMBUFFFER_H)
#define EAIO_EASTREAMBUFFFER_H
#define FOUNDATION_EASTREAMBUFFFER_H    // For backward compatability. Eventually, we'll want to get rid of this.


#include <EAIO/internal/Config.h>
#ifndef EAIO_EASTREAM_H
    #include <EAIO/EAStream.h>
#endif
#include <string.h>



namespace EA
{
    namespace Allocator
    {
        class ICoreAllocator;
    }

    namespace IO
    {
        /// class StreamBuffer
        ///
        /// Implements a smart buffer around a random-access stream.
        /// This buffering is significantly smarter than the file buffering 
        /// you get with the C or C++ standard libaries, and it gives the 
        /// user some configurability options as well.
        ///
        class EAIO_API StreamBuffer : public IStream
        {
        public:
            enum Option
            {
                kOptionCacheSize = 1   /// If enabled, then the size of the stream is cached, for higher performance. You must only enable this if you know the stream size is unchanging, as with a read-only file. This option can be set at any time, including after a stream has been used.
            };

            static const uint32_t  kTypeStreamBuffer = 0x12ea45bc;

            static const size_type kBufferSizeReadDefault  = (size_type)2000;   /// This is the default write buffer size.
            static const size_type kBufferSizeWriteDefault = (size_type)2000;   /// This is the default write buffer size.
            static const size_type kBufferSizeUseDefault   = (size_type)  -1;   /// This means that the buffer is used with the default size.
            static const size_type kBufferSizeUnspecified  = (size_type)  -2;   /// This means that the user means to leave the given buffer as-is.

            typedef Allocator::ICoreAllocator Allocator;

        public:
            StreamBuffer(size_type nReadBufferSize = kBufferSizeUnspecified, 
                         size_type nWriteBufferSize = kBufferSizeUnspecified, 
                         IStream* pStream = NULL, Allocator* pAllocator = NULL);
           ~StreamBuffer();

            IStream* GetStream() const;
            bool     SetStream(IStream* pStream);

            void GetBufferSizes(size_type& nReadBufferSize, size_type& nWriteBufferSize) const;
            bool SetBufferSizes(size_type  nReadBufferSize, size_type  nWriteBufferSize);
            void SetBuffers(void* pReadBuffer, size_type nReadBufferSize, void* pWriteBuffer, size_type nWriteBufferSize);

            void SetOption(int option, int value);
            void SetAllocator(Allocator* pAllocator);

            virtual int       AddRef();
            virtual int       Release();
            virtual uint32_t  GetType() const;
            virtual int       GetAccessFlags() const;
            virtual int       GetState() const;
            virtual bool      Close();
            virtual size_type GetSize() const;
            virtual bool      SetSize(size_type size);
            virtual off_type  GetPosition(PositionType positionType = kPositionTypeBegin) const;
            virtual bool      SetPosition(off_type position, PositionType positionType = kPositionTypeBegin);            

            virtual size_type GetAvailable() const;
            virtual size_type Read(void* pData, size_type nSize);

            virtual bool      Flush();
            virtual bool      Write(const void* pData, size_type nSize);

        protected:
            void        FreeBuffers();
            void*       Realloc(void* p, size_type prevSize, size_type newSize);
            void        FlushAndClearBuffers();
            void        ClearReadBuffer();
            bool        FillReadBuffer();
            void        ClearWriteBuffer();
            bool        FillWriteBuffer(const char* pData, size_type nSize);
            bool        FlushWriteBuffer();

        protected:
            IStream*            mpStream;                      /// The stream that we are buffering.
            bool                mbEnableSizeCache;             /// If true, allow caching of the size of mpStream, for performance improvement.
            mutable size_type   mnStreamSize;                  /// Cached version of the size of mpStream, for performance improvement. Can be used when mpStream is read-only.
            int                 mnRefCount;                    /// The reference count, which may or may not be used.
            size_type           mnPositionExternal;            /// This is the position of the the file pointer as the the user sees it. It is where the next byte read or write will come from or go to.
            size_type           mnPositionInternal;            /// This is the position of the the file pointer as the owned stream sees it.
            Allocator*          mpAllocator;                   /// If non-NULL, then mpReadBuffer/mpWriteBuffer were allocated with it and should be freed with it.

            char*               mpReadBuffer;                  /// Pointer to buffer data.
            size_type           mnReadBufferSize;              /// This is the size of the read buffer available for our usage.
            size_type           mnReadBufferStartPosition;     /// This is where in the file the beginning the read buffer corresponds to. 
            size_type           mnReadBufferUsed;              /// This is the count of bytes in the read buffer that are valid.

            char*               mpWriteBuffer;                 /// Pointer to buffer data.
            size_type           mnWriteBufferSize;             /// This is the size of the write buffer available for our usage.
            size_type           mnWriteBufferStartPosition;    /// This is where in the file the beginning the write buffer corresponds to.
            size_type           mnWriteBufferUsed;             /// This is the count of bytes in the write buffer that are valid.
        };


    } // namespace IO

} // namespace EA





/////////////////////////////////////////////////////////////////////////////
// inlines
/////////////////////////////////////////////////////////////////////////////

namespace EA
{
    namespace IO
    {

        inline
        void StreamBuffer::SetAllocator(Allocator* pAllocator)
        {
            mpAllocator = pAllocator;
        }

        inline
        IStream* StreamBuffer::GetStream() const
        {
            // We do not AddRef the returned stream.
            return mpStream;
        }

        inline
        void StreamBuffer::GetBufferSizes(size_type& nReadBufferSize, size_type& nWriteBufferSize) const
        {
            nReadBufferSize  = mnReadBufferSize;
            nWriteBufferSize = mnWriteBufferSize;
        }


    } // namespace IO

} // namespace EA


#endif // Header include guard







