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
// EAStreamBuffer.cpp
//
// Copyright (c) 2007, Electronic Arts Inc. All rights reserved.
// Written by Paul Pedriana
//
// Implements smart buffering around a random access IO stream.
/////////////////////////////////////////////////////////////////////////////


#include <EAIO/internal/Config.h>
#include <EAIO/EAStreamBuffer.h>
#include <EAIO/PathString.h>
#include <coreallocator/icoreallocator_interface.h>
#include <string.h> // memcpy, etc.
#include <stdlib.h>
#include EA_ASSERT_HEADER



///////////////////////////////////////////////////////////////////////////////
// MIN / MAX
//
#define LOCAL_MIN(x, y) ((x) < (y) ? (x) : (y))
#define LOCAL_MAX(x, y) ((x) > (y) ? (x) : (y))


///////////////////////////////////////////////////////////////////////////////
// EA_STREAM_BUFFER_DEV_ASSERT
//
// The asserts in this module are intended to aid the developer, not for
// checking valid usage. Since these asserts check stream position, which can
// be drastically slow for some streams, they are disabled by default.
//
#define EA_STREAM_BUFFER_DEV_ASSERT(X) //EA_ASSERT(X)


namespace EA
{

namespace IO
{

namespace StreamBufferLocal
{

    ///////////////////////////////////////////////////////////////////////////////
    // kBufferSizeMin / kBufferSizeMax
    //
    // We define these here instead of in the module interface header because 
    // these values may change per-platform.
    //
    const size_t kBufferSizeMin = 4;
    const size_t kBufferSizeMax = 16000000;


    ///////////////////////////////////////////////////////////////////////////////
    // kSizeTypeUnset
    //
    const size_type kSizeTypeUnset = (size_type)-1;
}



///////////////////////////////////////////////////////////////////////////////
// StreamBuffer
//
StreamBuffer::StreamBuffer(size_type nReadBufferSize, size_type nWriteBufferSize, IStream* pStream, Allocator* pAllocator)
  : mpStream(NULL),
    mbEnableSizeCache(false),
    mnStreamSize(StreamBufferLocal::kSizeTypeUnset),
    mnRefCount(0),

    mnPositionExternal(0),
    mnPositionInternal(0),
    mpAllocator(pAllocator ? pAllocator : IO::GetAllocator()),

    mpReadBuffer(NULL),
    mnReadBufferSize(0),
    mnReadBufferStartPosition(0),
    mnReadBufferUsed(0),

    mpWriteBuffer(NULL),
    mnWriteBufferSize(0),
    mnWriteBufferStartPosition(0),
    mnWriteBufferUsed(0)
{
    SetBufferSizes(nReadBufferSize, nWriteBufferSize);
    SetStream(pStream);
}


///////////////////////////////////////////////////////////////////////////////
// ~StreamBuffer
//
StreamBuffer::~StreamBuffer()
{
    SetStream(NULL); // This will flush the write buffer.
    FreeBuffers();
}


///////////////////////////////////////////////////////////////////////////////
// FreeBuffers
//
void StreamBuffer::FreeBuffers()
{
    if(mpReadBuffer)
    {
        if(mpAllocator)
            mpAllocator->Free(mpReadBuffer, mnReadBufferSize);

        mpReadBuffer = NULL;

        #ifdef EA_DEBUG
            mnReadBufferSize          = 0;
            mnReadBufferStartPosition = 0;
            mnReadBufferUsed          = 0;
        #endif
    }

    if(mpWriteBuffer)
    {
        if(mpAllocator)
            mpAllocator->Free(mpWriteBuffer, mnWriteBufferSize);

        mpWriteBuffer = NULL;

        #ifdef EA_DEBUG
            mnWriteBufferSize          = 0;
            mnWriteBufferStartPosition = 0;
            mnWriteBufferUsed          = 0;
        #endif
    }
}


///////////////////////////////////////////////////////////////////////////////
// Realloc
//
void* StreamBuffer::Realloc(void* p, size_type prevSize, size_type n)
{
    void* pReturnValue = NULL;

    EA_ASSERT(n < 0xffffffff); // We are currently limited to 4GB.

    if(mpAllocator)
    {
        if(p)
        {
            if(n)
            {
                pReturnValue = mpAllocator->Alloc((size_t)n, EAIO_ALLOC_PREFIX "StreamBuffer", 0);

                if(pReturnValue)
                {
                    if(n > prevSize)
                       n = prevSize;

                    memcpy(pReturnValue, p, (size_t)n);
                    mpAllocator->Free(p, prevSize);
                }
            }
            // Not needed because pReturnValue is NULL by default.
            //else // In this case the standard calls for the same behaviour as free.
            //    pReturnValue = NULL;
        }
        else if(n)
        {
            // The C Standard calls for realloc to exhibit the same behaviour as malloc, including for a size of zero.
            pReturnValue = mpAllocator->Alloc((size_t)n, EAIO_ALLOC_PREFIX "StreamBuffer", 0);
        }
    }

    return pReturnValue;
}


///////////////////////////////////////////////////////////////////////////////
// SetStream
//
bool StreamBuffer::SetStream(IStream* pStream)
{
    using namespace StreamBufferLocal;

    bool bReturnValue = true;

    if(pStream != mpStream)
    {
        // Reset the cached stream size. We will initialize it when it is first needed.
        mnStreamSize = kSizeTypeUnset;

        if(mpStream)
            FlushAndClearBuffers();

        if(pStream)
        {
            pStream->AddRef();

            if(pStream->GetAccessFlags())
            {
                mnPositionExternal = (size_type)pStream->GetPosition();
                mnPositionInternal = mnPositionExternal;
            }
            else
                bReturnValue = false;
        }

        if(mpStream)
            mpStream->Release();

        mpStream = pStream;
    }

    return bReturnValue;
}


///////////////////////////////////////////////////////////////////////////////
// SetBufferSizes
//
bool StreamBuffer::SetBufferSizes(size_type nReadBufferSize, size_type nWriteBufferSize)
{
    using namespace StreamBufferLocal;

    if(nReadBufferSize != kBufferSizeUnspecified)
    {
        if(nReadBufferSize == kBufferSizeUseDefault)
            nReadBufferSize = kBufferSizeReadDefault;
        nReadBufferSize &= (size_type)~1;                               // If the value is odd, this makes it even.
        if((nReadBufferSize > 0) && (nReadBufferSize < kBufferSizeMin)) // We allow zero buffering, but otherwise nothing less than kBufferSizeMin
            nReadBufferSize = kBufferSizeMin;
        if(nReadBufferSize > kBufferSizeMax)
            nReadBufferSize = kBufferSizeMax;
        if(nReadBufferSize < mnReadBufferSize)
            ClearReadBuffer();
        char* const pReadBufferSaved = mpReadBuffer;
        mpReadBuffer = (char*)Realloc(mpReadBuffer, mnReadBufferSize, nReadBufferSize);  // Note that Realloc specifies that passing NULL to realloc acts the same as malloc.
        if(mpReadBuffer)                                                                 // Realloc also specifies that a NULL return value means the reallocation failed and the old pointer is not freed.
            mnReadBufferSize = nReadBufferSize;
        else
            mpReadBuffer = pReadBufferSaved;
    }

    if(nWriteBufferSize != kBufferSizeUnspecified)
    {
        if(nWriteBufferSize == kBufferSizeUseDefault)
            nWriteBufferSize = kBufferSizeWriteDefault;
        nWriteBufferSize &= (size_type)~1;
        if((nWriteBufferSize > 0) && (nWriteBufferSize < kBufferSizeMin))
            nWriteBufferSize = kBufferSizeMin;
        if(nWriteBufferSize > kBufferSizeMax)
            nWriteBufferSize = kBufferSizeMax;
        if(nWriteBufferSize < mnWriteBufferSize)
            FlushWriteBuffer();
        char* const pWriteBufferSaved = mpWriteBuffer;
        mpWriteBuffer = (char*)Realloc(mpWriteBuffer, mnWriteBufferSize, nWriteBufferSize);  // Note that Realloc specifies that passing NULL to realloc acts the same as malloc.
        if(mpWriteBuffer)                                                                    // Realloc also specifies that a NULL return value means the reallocation failed and the old pointer is not freed.
            mnWriteBufferSize = nWriteBufferSize;
        else
            mpWriteBuffer = pWriteBufferSaved;
    }

    return true;

}


///////////////////////////////////////////////////////////////////////////////
// SetBuffers
//
void StreamBuffer::SetBuffers(void* pReadBuffer, size_type nReadBufferSize, void* pWriteBuffer, size_type nWriteBufferSize)
{
    using namespace StreamBufferLocal;

    // Setting buffers directly is currently an exclusive alternative to setting buffers via mpAllocator.
    SetAllocator(NULL);

    // We don't have a means of reallocating buffers via this function. So we assert that this isn't being done. 
    EA_ASSERT(!pReadBuffer  || !mpReadBuffer);
    EA_ASSERT(!pWriteBuffer || !mpWriteBuffer);

    if(pReadBuffer && !mpReadBuffer)
    {
        EA_ASSERT(nReadBufferSize >= kBufferSizeMin);

        mpReadBuffer              = (char*)pReadBuffer;
        mnReadBufferSize          = nReadBufferSize;
        mnReadBufferStartPosition = 0;
        mnReadBufferUsed          = 0;
    }

    if(pWriteBuffer && !mpWriteBuffer)
    {
        EA_ASSERT(nWriteBufferSize >= kBufferSizeMin);

        mpWriteBuffer              = (char*)pWriteBuffer;
        mnWriteBufferSize          = nWriteBufferSize;
        mnWriteBufferStartPosition = 0;
        mnWriteBufferUsed          = 0;
    }
}


///////////////////////////////////////////////////////////////////////////////
// SetOption
//
void StreamBuffer::SetOption(int option, int value)
{
    if(option == kOptionCacheSize)
        mbEnableSizeCache = (value != 0);
}


///////////////////////////////////////////////////////////////////////////////
// GetType
//
uint32_t StreamBuffer::GetType() const
{
    return kTypeStreamBuffer;
}


///////////////////////////////////////////////////////////////////////////////
// AddRef
//
int StreamBuffer::AddRef()
{
     return ++mnRefCount;
}


///////////////////////////////////////////////////////////////////////////////
// Release
//
int StreamBuffer::Release()
{
     if(mnRefCount > 1)
          return --mnRefCount;
     delete this;
     return 0;
}


///////////////////////////////////////////////////////////////////////////////
// GetAccessFlags
//
int StreamBuffer::GetAccessFlags() const
{
    if(mpStream)
        return mpStream->GetAccessFlags();
    return 0;
}


///////////////////////////////////////////////////////////////////////////////
// GetState
//
int StreamBuffer::GetState() const
{
    if(mpStream)
        return mpStream->GetState();
    return 0;
}


///////////////////////////////////////////////////////////////////////////////
// Close
//
bool StreamBuffer::Close()
{
    if(mpStream)
    {
        // Verify that the position as the owned stream sees it is as we think it is.
        EA_STREAM_BUFFER_DEV_ASSERT(mnPositionInternal == (size_type)mpStream->GetPosition());
        // Our design stipulates that we only have at most one buffer active at a time.
        EA_STREAM_BUFFER_DEV_ASSERT(!mnReadBufferUsed || !mnWriteBufferUsed);

        FlushAndClearBuffers();

        return mpStream->Close();
    }

    return false;
}


///////////////////////////////////////////////////////////////////////////////
// GetSize
//
size_type StreamBuffer::GetSize() const
{
    using namespace StreamBufferLocal;

    if(mpStream)
    {
        if(mnStreamSize != kSizeTypeUnset)  // If the cached value is valid...
            return mnStreamSize;

        size_type nSize = mpStream->GetSize();

        if(nSize != kSizeTypeError) // If there wasn't an error...
        {
            if(mnWriteBufferUsed) // If there is any write buffer...
            {
                // If the write buffer extends beyond the current file end, adjust the reported file end.
                EA_STREAM_BUFFER_DEV_ASSERT(mnPositionExternal == (mnWriteBufferStartPosition + mnWriteBufferUsed));

                if(nSize < mnPositionExternal) // If the user has written past the end of the file...
                    nSize = mnPositionExternal;
            }
        }

        if(mbEnableSizeCache)
        {
            // To consider: Enable this assertion, as it often a bad idea to cache the size of writable file.
            // EA_ASSERT((mpStream->GetAccessFlags() & kAccessFlagWrite) == 0);
            mnStreamSize = nSize;
        }

        return nSize;
    }

    return kSizeTypeError;
}


///////////////////////////////////////////////////////////////////////////////
// SetSize
//
bool StreamBuffer::SetSize(size_type nSize)
{
    bool bReturnValue = false;

    if(mpStream)
    {
        // Verify that the position as the owned stream sees it is as we think it is.
        EA_STREAM_BUFFER_DEV_ASSERT(mnPositionInternal == (size_type)mpStream->GetPosition());
        // Our design stipulates that we only have at most one buffer active at a time.
        EA_STREAM_BUFFER_DEV_ASSERT(!mnReadBufferUsed || !mnWriteBufferUsed);

        // Possibly clear/flush buffers.
        ClearReadBuffer();
        FlushWriteBuffer(); // What do we do if this fails?

        bReturnValue = mpStream->SetSize(nSize);

        // Set current position information, which we do regardless of bReturnValue.
        mnPositionExternal = (size_type)mpStream->GetPosition();
        mnPositionInternal = mnPositionExternal;
    }

    return bReturnValue;
}


///////////////////////////////////////////////////////////////////////////////
// GetPosition
//
off_type StreamBuffer::GetPosition(PositionType positionType) const
{
    if(mpStream)
    {
        switch(positionType)
        {
            case kPositionTypeBegin:
                return (off_type)mnPositionExternal;
            case kPositionTypeEnd:
                return (off_type)(mnPositionExternal - GetSize()); // This will yield a value <= 0.
            case kPositionTypeCurrent:
            default:
                return 0; // kPositionTypeCurrent, which is always 0 for a 'get' operation.
        }
    }
    return (off_type)kSizeTypeError;
}


///////////////////////////////////////////////////////////////////////////////
// SetPosition
//
bool StreamBuffer::SetPosition(off_type nPosition, PositionType positionType)
{
    bool bReturnValue = false;

    if(mpStream)
    {
        // Verify that the position as the owned stream sees it is as we think it is.
        EA_STREAM_BUFFER_DEV_ASSERT(mnPositionInternal == (size_type)mpStream->GetPosition());
        // Our design stipulates that we only have at most one buffer active at a time.
        EA_STREAM_BUFFER_DEV_ASSERT(!mnReadBufferUsed  || !mnWriteBufferUsed);
        // With write buffers, the following should always be true.
        EA_STREAM_BUFFER_DEV_ASSERT(!mnWriteBufferUsed || (mnPositionExternal - mnPositionInternal) == mnWriteBufferUsed);
        EA_STREAM_BUFFER_DEV_ASSERT(!mnWriteBufferUsed || (mnPositionExternal == (mnWriteBufferStartPosition + mnWriteBufferUsed)));

        // Convert a relative seek specification to an absolute seek specification.
        if(positionType == kPositionTypeCurrent)
            nPosition = (off_type)(nPosition + mnPositionExternal); 
        else if(positionType == kPositionTypeEnd)
            nPosition = (off_type)(nPosition + GetSize());
        positionType = kPositionTypeBegin;

        // If we have a read buffer, bypass a seek via the owned stream and simply reset 
        // mnPositionExternal, which will be taken into account on the next read.
        // In practice, this will be a rather common pathway which is taken.
        if(mnReadBufferUsed && (nPosition >= 0)) // If we have read buffering happening and the new position is valid...
        {
            mnPositionExternal = (size_type)nPosition;
            bReturnValue = true;
        }
        else if(nPosition == (off_type)mnPositionExternal) // If there is no change...
            bReturnValue = true;
        else
        {
            // At this point, we do a true seek on the owned stream.
            // Possibly flush the write buffer. There's no reason to clear the read buffer.
            FlushWriteBuffer();

            // Do the seek with the owned stream.
            if(mpStream->SetPosition(nPosition, positionType))
                bReturnValue = true;
            else
                nPosition = mpStream->GetPosition();
            mnPositionExternal = mnPositionInternal = (size_type)nPosition;
        }
    }

    return bReturnValue;
}

          

///////////////////////////////////////////////////////////////////////////////
// GetAvailable
//
size_type StreamBuffer::GetAvailable() const
{
    const size_type nSize = (GetSize() - mnPositionExternal);
    return nSize;
}


///////////////////////////////////////////////////////////////////////////////
// Read
//
size_type StreamBuffer::Read(void* pData, size_type nSize)
{
    if(mpStream)
    {
        // Verify that the position as the owned stream sees it is as we think it is.
        EA_STREAM_BUFFER_DEV_ASSERT(mnPositionInternal == (size_type)mpStream->GetPosition());
        // Our buffer design stipulates that we only have at most one buffer active at a time.
        EA_STREAM_BUFFER_DEV_ASSERT(!mnReadBufferUsed  || !mnWriteBufferUsed);
        // Our design stipulates that if both the buffers' length are zero, then the internal and external positions are the same.
        EA_STREAM_BUFFER_DEV_ASSERT( mnReadBufferUsed  ||  mnWriteBufferUsed || (mnPositionExternal == mnPositionInternal));
        // With write buffers, the following should always be true.
        EA_STREAM_BUFFER_DEV_ASSERT(!mnWriteBufferUsed || (mnPositionExternal - mnPositionInternal) == mnWriteBufferUsed);
        EA_STREAM_BUFFER_DEV_ASSERT(!mnWriteBufferUsed || (mnPositionExternal == (mnWriteBufferStartPosition + mnWriteBufferUsed)));

        if(nSize)
        {
            // Possibly flush the write buffer.
            if(mnWriteBufferUsed)
                FlushWriteBuffer();

            // Possibly get data from the read buffer.
            if(mnReadBufferSize) // If read buffering is enabled...
            {
                char*     pData8                  = (char*)pData;
                bool      bReadStartIsWithinCache = false;
                size_type nBytesRemaining         = nSize;
                bool      bResult                 = true;

                if((mnPositionExternal >= mnReadBufferStartPosition) &&
                    (mnPositionExternal < (mnReadBufferStartPosition + mnReadBufferUsed)))
                {
                    bReadStartIsWithinCache = true;
                }

                if(bReadStartIsWithinCache)
                {
                    const size_type nOffsetWithinReadBuffer   = mnPositionExternal - mnReadBufferStartPosition;
                    const size_type nBytesToGetFromReadBuffer = LOCAL_MIN((mnReadBufferUsed - nOffsetWithinReadBuffer), nBytesRemaining);

                    EA_STREAM_BUFFER_DEV_ASSERT((nBytesToGetFromReadBuffer > 0) && (nBytesToGetFromReadBuffer < ((size_type)0 - mnReadBufferSize)) && (mnReadBufferSize >= nBytesToGetFromReadBuffer));

                    memcpy(pData8, mpReadBuffer + nOffsetWithinReadBuffer, (size_t)nBytesToGetFromReadBuffer);
                    nBytesRemaining    -= nBytesToGetFromReadBuffer;
                    pData8             += nBytesToGetFromReadBuffer;
                    mnPositionExternal += nBytesToGetFromReadBuffer;
                }

                while(nBytesRemaining) // If there is anything else to read...
                {
                    // We need to clear the read buffer, move the current internal file pointer to 
                    // be where we left off above, and start filling the cache from that position on.
                    ClearReadBuffer();
                    if(mnPositionInternal != mnPositionExternal)
                        bResult = mpStream->SetPosition((off_type)mnPositionExternal, kPositionTypeBegin);
                    if(bResult)
                    {
                        mnPositionInternal = mnPositionExternal;

                        // Check if the read is a large read -- say, twice the size of the
                        // read buffer. If the read is very large, bypass the read buffer and
                        // issue a read directly to the client's buffer.
                        if(nBytesRemaining > (2 * mnReadBufferSize))
                        {
                            EA_STREAM_BUFFER_DEV_ASSERT(mnPositionInternal == (size_type)mpStream->GetPosition());
                            const size_type nReadSize = mpStream->Read(pData8, nBytesRemaining);

                            if(nReadSize != kSizeTypeError)
                            {
                                mnPositionInternal += nReadSize;
                                mnPositionExternal += nReadSize;
                                nBytesRemaining    -= nReadSize;
                                pData8             += nReadSize;
                            }
                            else // else an error occurred
                                bResult = false;
                            break;
                        }
                        else
                            bResult = FillReadBuffer();
                    }

                    if(bResult && mnReadBufferUsed)
                    {
                        const size_type nBytesToGetFromReadBuffer = LOCAL_MIN(mnReadBufferUsed, nBytesRemaining);

                        EA_STREAM_BUFFER_DEV_ASSERT((nBytesToGetFromReadBuffer > 0) && (nBytesToGetFromReadBuffer < ((size_type)0 - mnReadBufferSize)) && (mnReadBufferSize >= nBytesToGetFromReadBuffer));

                        memcpy(pData8, mpReadBuffer, (size_t)nBytesToGetFromReadBuffer);
                        nBytesRemaining    -= nBytesToGetFromReadBuffer;
                        pData8             += nBytesToGetFromReadBuffer;
                        mnPositionExternal += nBytesToGetFromReadBuffer;
                    }
                    else // Else we hit the end of the file.
                        break;
                } // while

                EA_STREAM_BUFFER_DEV_ASSERT(nSize >= nBytesRemaining);
                return nSize - nBytesRemaining; // Normally, 'nBytesRemaining' will be zero due to being able to read all requested data.
            }
            else // Else non-buffered behaviour
            {
                EA_STREAM_BUFFER_DEV_ASSERT(!mnReadBufferUsed && !mnWriteBufferUsed && (mnPositionExternal == mnPositionInternal));

                const size_type nReadSize = mpStream->Read(pData, nSize);

                if(nReadSize != kSizeTypeError)
                    mnPositionInternal += nSize;
                else
                    mnPositionInternal = (size_type)mpStream->GetPosition();
                mnPositionExternal = mnPositionInternal;

                return nReadSize;
            }
        }
        else // Else the user requested a read of zero bytes.
            return 0;
    }

    return kSizeTypeError;
}


///////////////////////////////////////////////////////////////////////////////
// FlushAndClearBuffers
//
// This is an internal function.
//
void StreamBuffer::FlushAndClearBuffers()
{
    FlushWriteBuffer();
    ClearReadBuffer();
    ClearWriteBuffer();
    mnPositionExternal = 0;
    mnPositionInternal = 0;
}


///////////////////////////////////////////////////////////////////////////////
// ClearReadBuffer
//
// This is an internal function.
//
// This function does not resize the read buffer to zero but rather simply
// sets it to be empty.
//
void StreamBuffer::ClearReadBuffer()
{
    mnReadBufferStartPosition = 0;
    mnReadBufferUsed          = 0;
}


///////////////////////////////////////////////////////////////////////////////
// FillReadBuffer
//
// This is an internal function.
//
// This function erases anything that is in the read buffer and fills it
// completely with data from the current actual file position.
//
bool StreamBuffer::FillReadBuffer()
{
    EA_STREAM_BUFFER_DEV_ASSERT(mpStream && !mnReadBufferUsed && !mnWriteBufferUsed);

    const size_type nReadSize = mpStream->Read(mpReadBuffer, mnReadBufferSize);

    if(nReadSize != kSizeTypeError) // If there was no error...
    {
        mnReadBufferStartPosition = mnPositionInternal; // We leave 'mnPositionExternal' alone.
        mnReadBufferUsed          = nReadSize;
        mnPositionInternal       += nReadSize;
    }
    else
    {
        mnReadBufferStartPosition = 0;
        mnReadBufferUsed          = 0;
    }

    return (nReadSize != kSizeTypeError);
}


///////////////////////////////////////////////////////////////////////////////
// Flush
//
bool StreamBuffer::Flush()
{
    if(mpStream)
    {
        // Verify that the position as the owned stream sees it is as we think it is.
        EA_STREAM_BUFFER_DEV_ASSERT(mnPositionInternal == (size_type)mpStream->GetPosition());
        // With write buffers, the following should always be true.
        EA_STREAM_BUFFER_DEV_ASSERT(!mnWriteBufferUsed || (mnPositionExternal - mnPositionInternal) == mnWriteBufferUsed);
        EA_STREAM_BUFFER_DEV_ASSERT(!mnWriteBufferUsed || (mnPositionExternal == (mnWriteBufferStartPosition + mnWriteBufferUsed)));

        // Possibly flush the write buffer. No reason to clear the read buffer.
        return FlushWriteBuffer();
    }
    return false;
}


///////////////////////////////////////////////////////////////////////////////
// Write
//
bool StreamBuffer::Write(const void* pData, size_type nSize)
{
    bool bReturnValue = false;

    if(mpStream)
    {
        // Verify that the position as the owned stream sees it is as we think it is.
        EA_STREAM_BUFFER_DEV_ASSERT(mnPositionInternal == (size_type)mpStream->GetPosition());
        // Our buffer design stipulates that we only have at most one buffer active at a time.
        EA_STREAM_BUFFER_DEV_ASSERT(!mnReadBufferUsed  || !mnWriteBufferUsed);
        // Our design stipulates that if both the buffers' length are zero, then the internal and external positions are the same.
        EA_STREAM_BUFFER_DEV_ASSERT( mnReadBufferUsed  ||  mnWriteBufferUsed || (mnPositionExternal == mnPositionInternal));
        // With write buffers, the following should always be true.
        EA_STREAM_BUFFER_DEV_ASSERT(!mnWriteBufferUsed || (mnPositionExternal - mnPositionInternal) == mnWriteBufferUsed);
        EA_STREAM_BUFFER_DEV_ASSERT(!mnWriteBufferUsed || (mnPositionExternal == (mnWriteBufferStartPosition + mnWriteBufferUsed)));

        // Possibly clear the read buffer.
        if(mnReadBufferUsed)
        {
            // Our buffer design stipulates that we only have at most one buffer
            // active at a time. This isn't much of a big deal, as 95+% of the time
            // we only open files for either reading or writing.
            ClearReadBuffer();

            // If the position as the user sees it is different from the position as the 
            // owned stream sees it, we need to align these, because otherwise writes
            // will go to the place the owned stream sees it and not where the user expects.
            if(mnPositionExternal != mnPositionInternal)
                mpStream->SetPosition((off_type)mnPositionExternal); // Todo: deal with errors here.
        }

        // Possibly use the write buffer.
        if(mnWriteBufferSize) // If buffering is enabled...
        {
            // With write buffers, the following should always be true.
            EA_STREAM_BUFFER_DEV_ASSERT(!mnWriteBufferUsed || (mnPositionExternal - mnPositionInternal) == mnWriteBufferUsed);
            EA_STREAM_BUFFER_DEV_ASSERT(!mnWriteBufferUsed || (mnPositionExternal == (mnWriteBufferStartPosition + mnWriteBufferUsed)));

            bReturnValue = FillWriteBuffer((const char*)pData, nSize); // Todo: deal with errors here.
            mnPositionExternal += nSize;

            // With write buffers, the following should always be true.
            EA_STREAM_BUFFER_DEV_ASSERT(!mnWriteBufferUsed || (mnPositionExternal - mnPositionInternal) == mnWriteBufferUsed);
            EA_STREAM_BUFFER_DEV_ASSERT(!mnWriteBufferUsed || (mnPositionExternal == (mnWriteBufferStartPosition + mnWriteBufferUsed)));
        }
        else // Else non-buffered behaviour
        {
            EA_STREAM_BUFFER_DEV_ASSERT(!mnReadBufferUsed && !mnWriteBufferUsed && (mnPositionExternal == mnPositionInternal));

            bReturnValue = mpStream->Write(pData, nSize);
            if(bReturnValue)
                mnPositionInternal += nSize;
            else
                mnPositionInternal = (size_type)mpStream->GetPosition();
            mnPositionExternal = mnPositionInternal;
        }
    }

    return bReturnValue;
}


///////////////////////////////////////////////////////////////////////////////
// ClearWriteBuffer
//
// This is an internal function.
//
// This function does not resize the buffer to zero but rather simply
// sets it to be empty.
//
void StreamBuffer::ClearWriteBuffer()
{
    EA_STREAM_BUFFER_DEV_ASSERT(mnWriteBufferUsed == 0);

    mnWriteBufferStartPosition = 0;
    mnWriteBufferUsed          = 0;
}


///////////////////////////////////////////////////////////////////////////////
// FillWriteBuffer
//
// This is an internal function.
//
bool StreamBuffer::FillWriteBuffer(const char* pData, size_type nSize)
{
    bool bReturnValue = true;

    if(nSize > 0)
    {
        if(mnWriteBufferUsed == 0) // If this is our first write to the buffer since it was last purged...
        {
            EA_STREAM_BUFFER_DEV_ASSERT(mnPositionInternal == mnPositionExternal);
            mnWriteBufferStartPosition = mnPositionInternal;
        }

        if((mnWriteBufferUsed + nSize) <= mnWriteBufferSize)
        {
            // We simply append the data to the write buffer. This should be the most
            // common pathway. If we are finding that the write buffer is often too
            // small for the amounts of writes happening, then the write buffer needs
            // to be enlarged or discarded altogether.
            memcpy(mpWriteBuffer + mnWriteBufferUsed, pData, (size_t)nSize);
            mnWriteBufferUsed += nSize;
            EA_STREAM_BUFFER_DEV_ASSERT(mnWriteBufferUsed <= mnWriteBufferSize);
        }
        //else if(nSize > mnWriteBufferSize) - This could be a performance improvement if we implement it.
        //{
        //    FlushWriteBuffer();
        //    ClearWriteBuffer();
        //    bReturnValue = (Write the data directly to disk, bypassing the cache.)
        //}
        else // Else the input data overflows the write buffer.
        {
            // In this case we fill the write buffer as much as possible,
            // flush it, clear it, and fill with new data. This would be
            // faster if we detected large input data sizes and simply
            // wrote them directly to disk instead of copying them to
            // the buffer and then copying the buffer to disk.
            while(nSize && bReturnValue)
            {
                const size_type nSizeToBuffer = LOCAL_MIN(mnWriteBufferSize - mnWriteBufferUsed, nSize);

                if(nSizeToBuffer)
                {
                    memcpy(mpWriteBuffer + mnWriteBufferUsed, pData, (size_t)nSizeToBuffer);
                    mnWriteBufferUsed += nSizeToBuffer;
                    pData             += nSizeToBuffer;
                    nSize             -= nSizeToBuffer;
                }

                EA_STREAM_BUFFER_DEV_ASSERT((off_type)nSize >= 0);
                EA_STREAM_BUFFER_DEV_ASSERT(mnWriteBufferUsed <= mnWriteBufferSize);

                if(mnWriteBufferUsed == mnWriteBufferSize)
                    bReturnValue = FlushWriteBuffer();
            }
        }
    }

    return bReturnValue;
}


///////////////////////////////////////////////////////////////////////////////
// FlushWriteBuffer
//
// This is an internal function.
//
bool StreamBuffer::FlushWriteBuffer()
{
    bool bReturnValue = true;

    // We leave 'mnPositionExternal' alone. However, upon exit, the internal
    // and external positions should (usually) be equal. Actually, there is one case
    // where they won't be equal and that is when the Write function calls FillWriteBuffer
    // and the buffer needs to be flushed before returning from FillWriteBuffer. In that
    // case the Write function won't update the 'mnPositionExternal' variable
    // until after the FillWriteBuffer function returns. Then things will become
    // aligned properly.

    if(mnWriteBufferUsed) // If there is anything to write...
    {
        EA_STREAM_BUFFER_DEV_ASSERT(mpStream && !mnReadBufferUsed);
        EA_STREAM_BUFFER_DEV_ASSERT(mnPositionInternal == (size_type)mpStream->GetPosition());
        // The following is disabled because there is a case where this isn't true. See notes above.
        //EA_STREAM_BUFFER_DEV_ASSERT((mnPositionExternal - mnPositionInternal) == mnWriteBufferUsed);

        if(mpStream->Write(mpWriteBuffer, mnWriteBufferUsed))
        {
            mnPositionInternal        += mnWriteBufferUsed;
            mnWriteBufferStartPosition = mnPositionInternal;
            mnWriteBufferUsed          = 0;

            EA_STREAM_BUFFER_DEV_ASSERT(mnPositionInternal == (size_type)mpStream->GetPosition());
            // The following is disabled because there is a case where this isn't true. See notes above.
            //EA_STREAM_BUFFER_DEV_ASSERT(mnPositionInternal == mnPositionExternal);
        }
        else // Else we have a severe problem.
        {
            mnPositionInternal         = (size_type)mpStream->GetPosition();
            mnWriteBufferStartPosition = mnPositionInternal;
            mnWriteBufferUsed          = 0;
            bReturnValue               = false;
        }
    }

    return bReturnValue;
}





} // namespace IO

} // namespace EA



// For unity build friendliness, undef all local #defines.
#undef LOCAL_MIN
#undef LOCAL_MAX
#undef EA_STREAM_BUFFER_DEV_ASSERT







