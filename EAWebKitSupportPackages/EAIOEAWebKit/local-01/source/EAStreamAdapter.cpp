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
// EAStreamAdapter.cpp
//
// Copyright (c) 2007, Electronic Arts Inc. All rights reserved.
//
// Implements adapters for reading and writing basic types in an 
// endian-proper way with the EA::Stream interfaces.
//
// Written by Paul Pedriana
/////////////////////////////////////////////////////////////////////////////


#include <EAIO/internal/Config.h>
#include <EAIO/EAStream.h>
#include <EAIO/EAStreamAdapter.h>
#include <limits.h>

namespace {

    // Swizzling functions, copied from EAEndianConst.h. We don't use
    // the optimized, processor specific versions of these functions
    // defined in EndianValue.h, although perhaps we should consider
    // doing that.

    // The implementations are defined in a simplistic way so that 
    // the compiler can optimize away the logic in each function.
    // These functions are intentionally not optimized with tricks such 
    // as assembly language, compiler intrinsics, or memory tricks, as
    // such things would interfere with the compiler's ability to optimize
    // away these operations.
    inline uint16_t SwizzleUint16(uint16_t x)
    {
        return (uint16_t) ((x >> 8) | (x << 8));
    }

    inline uint32_t SwizzleUint32(uint32_t x)
    {
        // An alternative to the mechanism of using shifts and ors below
        // is to use byte addressing.
        return (uint32_t)
            ((x >> 24)               |
            ((x << 24) & 0xff000000) |
            ((x <<  8) & 0x00ff0000) |
            ((x >>  8) & 0x0000ff00)); 
    }

    inline uint64_t SwizzleUint64(uint64_t x)
    {
        return (uint64_t)
            ((x        & 0x000000ff) << 56) |
            ((x >> 56) & 0x000000ff)        |
            ((x        & 0x0000ff00) << 40) |
            ((x >> 40) & 0x0000ff00)        |
            ((x        & 0x00ff0000) << 24) |
            ((x >> 24) & 0x00ff0000)        |
            ((x        & 0xff000000) <<  8) |
            ((x >>  8) & 0xff000000);

        // Alternative implementation:
        //const uint32_t high32Bits = EASwizzleUint32((uint32_t)(x));
        //const uint32_t low32Bits  = EASwizzleUint32((uint32_t)(x >> 32));
        //return ((uint64_t)high32Bits << 32) | low32Bits;
    }
} // namespace



bool EA::IO::StreamAdapter::VerifyArraySize(uint32_t elementCount, uint32_t elementSize)
{
    const uint64_t          elementDataSize = (uint64_t)elementCount * elementSize;
    const EA::IO::off_type  currentPos      = mpStream->GetPosition();
    const EA::IO::size_type streamSize      = mpStream->GetSize();

    // It's possible to seek beyond the end of a stream and not
    // get an error -- this is frequently done to extend a stream.
    if((EA::IO::size_type)currentPos > streamSize)
    {
        VerifyIO(false);  // It's not clear that we should unilaterally do this, as the user hasn't actually tried to read something.
        return false;
    }

    const uint64_t sizeRemaining = (uint64_t)streamSize - (uint64_t)currentPos;

    if(sizeRemaining < elementDataSize)
    {
        VerifyIO(false);  // It's not clear that we should unilaterally do this, as the user hasn't actually tried to read something.
        return false;
    }

    return true;
}


EAIO_API EA::IO::size_type EA::IO::CopyStream(IStream* pSource, IStream* pDestination, size_type nSize)
{
    char            buffer[2048];
    size_type       nCurrentCount, nRemaining;
    const size_type nSourceSize = pSource->GetSize();

    if(nSourceSize == kSizeTypeError) // If the source size is of undetermined size...
        nSize = kLengthNull;          // Read all of the source.
    else if(nSize > nSourceSize)      // If the user size is too high...
        nSize = nSourceSize;          // Reduce the user size to be the source size.

    for(nRemaining = nSize; nRemaining != 0; nRemaining -= nCurrentCount)
    {
        nCurrentCount = ((nRemaining >= sizeof(buffer)) ? sizeof(buffer) : nRemaining);
        nCurrentCount = pSource->Read(buffer, nCurrentCount);

        if((nCurrentCount == kSizeTypeError) || !pDestination->Write(buffer, nCurrentCount))
            return kSizeTypeError;

        if(nCurrentCount == 0) // If we have read the entire source...
            break;
    }

    return (nSize - nRemaining); // Return the number of bytes copied. Note that nRemaining might be non-zero.
}



EAIO_API bool EA::IO::ReadBool8(IStream* pIS, bool& value)
{
    bool8_t n;

    if(pIS->Read(&n, sizeof(n)) == sizeof(n))
    {
        value = (n != 0);
        return true;
    }
    return false;
}


EAIO_API bool EA::IO::ReadBool8(IStream* pIS, bool* value, size_type count )
{
    return pIS->Read(value, count * sizeof(*value)) == count * sizeof(*value);
}


EAIO_API bool EA::IO::ReadInt8(IStream* pIS, int8_t& value)
{
    return pIS->Read(&value, sizeof(value)) == sizeof(value);
}


EAIO_API bool EA::IO::ReadInt8(IStream* pIS, int8_t* value, size_type count )
{
    return pIS->Read(value, count * sizeof(*value)) == count * sizeof(*value);
}


EAIO_API bool EA::IO::ReadUint8(IStream* pIS, uint8_t& value)
{
    return pIS->Read(&value, sizeof(value)) == sizeof(value);
}


EAIO_API bool EA::IO::ReadUint8(IStream* pIS, uint8_t* value, size_type count )
{
    return pIS->Read(value, count * sizeof(*value)) == count * sizeof(*value);
}


EAIO_API bool EA::IO::ReadUint16(IStream* pIS, uint16_t& value, Endian endianSource)
{
    if(pIS->Read(&value, sizeof(value)) == sizeof(value))
    {
        if(endianSource != kEndianLocal)
            value = SwizzleUint16(value);
        return true;
    }
    return false;
}


EAIO_API bool EA::IO::ReadUint16(IStream* pIS, uint16_t* value, size_type count, Endian endianSource)
{
    if(pIS->Read( value, count * sizeof(*value)) == count * sizeof(*value))
    {
        // Swizzle in place after the read
        if(endianSource != kEndianLocal)
        {
            while(count--)
            {
                const uint16_t c = *value;
                *value++ = SwizzleUint16(c); 
            }
        }
        return true;
    }
    return false;
}


EAIO_API bool EA::IO::ReadInt16(IStream* pIS, int16_t& value, Endian endianSource)
{
    return ReadUint16(pIS, (uint16_t&)value, endianSource);
}


EAIO_API bool EA::IO::ReadInt16(IStream* pIS, int16_t* value, size_type count, Endian endianSource)
{
    return ReadUint16(pIS, (uint16_t*)value, count, endianSource);
}


EAIO_API bool EA::IO::ReadUint32(IStream* pIS, uint32_t& value, Endian endianSource)
{
    if(pIS->Read(&value, sizeof(value)) == sizeof(value))
    {
        if(endianSource != kEndianLocal)
            value = SwizzleUint32(value);
        return true;
    }
    return false;
}


EAIO_API bool EA::IO::ReadUint32(IStream* pIS, uint32_t* value, size_type count, Endian endianSource)
{
    if(pIS->Read(value, count * sizeof(*value)) == count * sizeof(*value))
    {
        // Swizzle in place after the read
        if(endianSource != kEndianLocal)
        {
            while(count--)
            {
                const uint32_t c = *value;
                *value++ = SwizzleUint32(c);
            }
        }
        return true;
    }
    return false;
}


EAIO_API bool EA::IO::ReadInt32(IStream* pIS, int32_t& value, Endian endianSource)
{
    return ReadUint32(pIS, (uint32_t&)value, endianSource);
}


EAIO_API bool EA::IO::ReadInt32(IStream* pIS, int32_t* value, size_type count, Endian endianSource)
{
    return ReadUint32(pIS, (uint32_t*)value, count, endianSource);
}


EAIO_API bool EA::IO::ReadUint64(IStream* pIS, uint64_t& value, Endian endianSource)
{
    if(pIS->Read(&value, sizeof(value)) == sizeof(value))
    {
        if(endianSource != kEndianLocal)
            value = SwizzleUint64(value);
        return true;
    }
    return false;
}


EAIO_API bool EA::IO::ReadUint64(IStream* pIS, uint64_t* value, size_type count, Endian endianSource)
{
    if(pIS->Read(value, count * sizeof(*value)) == count * sizeof(*value))
    {
        // Swizzle in place after the read
        if(endianSource != kEndianLocal)
        {
            while(count--)
            {
                const uint64_t c = *value;
                *value++ = SwizzleUint64(c);
            }
        }
        return true;
    }
    return false;
}


EAIO_API bool EA::IO::ReadInt64(IStream* pIS, int64_t& value, Endian endianSource)
{
   return ReadUint64(pIS, (uint64_t&)value, endianSource);
}


EAIO_API bool EA::IO::ReadInt64(IStream* pIS, int64_t* value, size_type count, Endian endianSource)
{
    return ReadUint64(pIS, (uint64_t*)value, count, endianSource);
}


//EAIO_API bool EA::IO::ReadUint128(IStream* pIS, uint128_t& value, Endian endianSource)
//{
//    if(pIS->Read(&value, sizeof(value)) == sizeof(value))
//    {
//       if(endianSource != kEndianLocal)
//          value = SwizzleUint128(value);
//       return true;
//    }
//    return false;
//}


//EAIO_API bool EA::IO::ReadInt128(IStream* pIS, int128_t& value, Endian endianSource)
//{
//   return ReadUint128(pIS, (uint128_t&)value, endianSource);
//}


EAIO_API bool EA::IO::ReadFloat(IStream* pIS, float& value, Endian endianSource)
{
    return ReadUint32(pIS, (uint32_t&)value, endianSource);
}


EAIO_API bool EA::IO::ReadFloat(IStream* pIS, float* value, size_type count, Endian endianSource)
{
    return ReadUint32(pIS, (uint32_t*)value, count, endianSource);
}


EAIO_API bool EA::IO::ReadDouble(IStream* pIS, double& value, Endian endianSource)
{
    return ReadUint64(pIS, (uint64_t&)value, endianSource);
}


EAIO_API bool EA::IO::ReadDouble(IStream* pIS, double* value, size_type count, Endian endianSource)
{
    return ReadUint64(pIS, (uint64_t*)value, count, endianSource);
}


EAIO_API EA::IO::size_type EA::IO::ReadString(IStream* pIS, char8_t* pBuffer, size_type nMaxCount, EA::IO::Endian endianSource)
{
    const off_type nInitialPosition(pIS->GetPosition());

    char8_t   cCurrent;
    uint32_t  nLength(0);
    size_type nCount(0); // Number of chars returned to user.
    size_type nResult;

    if(!ReadUint32(pIS, nLength, endianSource))
        return kSizeTypeError;

    // If no buffer has been provided, just reset the stream and return the length.
    if(!pBuffer)
    {
        pIS->SetPosition(nInitialPosition);
        return (size_type)nLength;
    }

    // Determine how many characters we'll actually read into the buffer.
    // 'nMaxCount - 1' because we want to leave room for terminating NUL.
    size_type nReadLength = (nLength < nMaxCount - 1) ? nLength : nMaxCount - 1;

    while(pBuffer && (nCount < nReadLength)) 
    {
        nResult = pIS->Read(&cCurrent, sizeof(cCurrent));

        if(nResult != sizeof(cCurrent))
            break;

        *pBuffer++ = cCurrent;
        ++nCount;
    }

    // We may not have been able to read the entire string out of the stream
    // due to the nMaxCount limit, but we still want to advance the stream's
    // position to the end of the string.
    pIS->SetPosition(nInitialPosition + (off_type)sizeof(uint32_t) + (off_type)nLength);

    if(pBuffer)
        *pBuffer = '\0';

    return nLength; // Note that we return nLength and not nCount.
}


EAIO_API EA::IO::size_type EA::IO::ReadString(IStream* pIS, char16_t* pBuffer, size_type nMaxCount, EA::IO::Endian endianSource)
{
    const off_type nInitialPosition(pIS->GetPosition());

    char16_t  cCurrent;
    uint32_t  nLength(0);
    size_type nCount(0); // Number of chars returned to user.

    if(!ReadUint32(pIS, nLength, endianSource))
        return kSizeTypeError;

    // If no buffer has been provided, just reset the stream and return the length.
    if(!pBuffer)
    {
        pIS->SetPosition(nInitialPosition);
        return (size_type)nLength;
    }

    // Determine how many characters we'll actually read into the buffer.
    // 'nMaxCount - 1' because we want to leave room for terminating NUL.
    size_type nReadLength = (nLength < nMaxCount - 1) ? nLength : nMaxCount - 1;

    while(pBuffer && (nCount < nReadLength)) 
    {
        if(!EA::IO::ReadUint16(pIS, (uint16_t&)cCurrent, endianSource))
            break;

        *pBuffer++ = cCurrent;
        ++nCount;
    }

    // We may not have been able to read the entire string out of the stream
    // due to the nMaxCount limit, but we still want to advance the stream's
    // position to the end of the string.
    pIS->SetPosition(nInitialPosition + (off_type)sizeof(uint32_t) + (off_type)(nLength * sizeof(char16_t)));

    if(pBuffer)
        *pBuffer = '\0';

    return nLength; // Note that we return nLength and not nCount.
}


EAIO_API EA::IO::size_type EA::IO::ReadLine(IStream* pIS, char8_t* pLine, size_type nMaxCount)
{
    char8_t   cCurrent;
    size_type nCount(0); // Number of chars in the line, not including the line end characters(s).
    size_type nRead(0);  // Number of chars successfully read from stream. Will be >= nCount (due to presence of newlines).
    size_type nResult;
    off_type  nInitialPosition(0);

    if(!pLine)
        nInitialPosition = pIS->GetPosition();

    for(;;)
    {
        // We are reading one character at a time, which can be slow if the stream is 
        // not buffered. We read one character at a time because we don't want to read
        // past the end of the line and thus trigger seeks, which may not even be possible
        // for some streams.
        nResult = pIS->Read(&cCurrent, sizeof(cCurrent));

        if(nResult == sizeof(cCurrent))
        {
            ++nRead;

            if((cCurrent == '\r') || (cCurrent == '\n'))
            {
                // It's possible that we have a "\n" or "\r\n" sequence, and we want 
                // to read past the sequence, but not past anything else. This code here takes
                // care not to read past the first "\n" in a "\n\n" sequence, but is smart 
                // enough to read past the just first "\r\n" in a "\r\n\r\n" sequence.
                char8_t cNext = cCurrent;

                if(cCurrent == '\r') // If we have a "\r", then we read again, expecting a "\n".
                    nResult = (size_type)pIS->Read(&cNext, sizeof(cNext));

                if(nResult == sizeof(cNext) && cNext != '\n')
                {
                    // We have encountered an unexpected sequence: We have a "\rx" instead of "\n" or "\r\n".
                    // This call requires a stream that can back up.
                    pIS->SetPosition(-(off_type)sizeof(cNext), kPositionTypeCurrent);
                }

                break;
            }
            else
            {
                if(pLine && (nCount < (nMaxCount - 1))) // '- 1' because we want to leave room for terminating null.
                    *pLine++ = cCurrent;

                ++nCount;
            }
        }
        else
        {
            // In this case, there was nothing left to read in the file.
            // We need to differentiate between an empty line vs. nothing 
            // left to read in the file. To deal with that we return kSizeTypeDone.
            if(nRead == 0)
                nCount = kSizeTypeDone;

            break;
        }
    }

    if(pLine)
        *pLine = 0;
    else
        pIS->SetPosition(nInitialPosition);

    return nCount;
}


EAIO_API EA::IO::size_type EA::IO::ReadLine(IStream* pIS, char16_t* pLine, size_type nMaxCount, Endian endianSource)
{
    char16_t  cCurrent;
    size_type nCount(0); // Number of chars in the line, not including the line end characters(s).
    size_type nRead(0);  // Number of chars successfully read from stream. Will be >= nCount (due to presence of newlines).
    size_type nResult;
    off_type  nInitialPosition(0);
    char16_t  cr, lf;

    if(!pLine)
        nInitialPosition = pIS->GetPosition();

    if(endianSource == kEndianLocal)
    {
        cr = '\r';
        lf = '\n';
    }
    else
    {
        cr = SwizzleUint16('\r');
        lf = SwizzleUint16('\n');
    }

    for(;;)
    {
        // We are reading one character at a time, which can be slow if the stream is 
        // not buffered. We read one character at a time because we don't want to read
        // past the end of the line and thus trigger seeks, which may not even be possible
        // for some streams.
        nResult = pIS->Read(&cCurrent, sizeof(cCurrent));

        if(nResult == sizeof(cCurrent))
        {
            ++nRead;

            if((cCurrent == cr) || (cCurrent == lf))
            {
                // It's possible that we have a "\n" or "\r\n" sequence, and we want 
                // to read past the sequence, but not past anything else. This code here takes
                // care not to read past the first "\n" in a "\n\n" sequence, but is smart 
                // enough to read past the just first "\r\n" in a "\r\n\r\n" sequence.
                char16_t cNext = cCurrent;

                if(cCurrent == cr) // If we have a "\r", then we read again, expecting a "\n".
                    nResult = (size_type)pIS->Read(&cNext, sizeof(cNext));

                if(cNext != lf)
                {
                    // We have encountered an unexpected sequence: We have a "\rx" instead of "\n" or "\r\n".
                    // This call requires a stream that can back up.
                    pIS->SetPosition(-(off_type)sizeof(cNext), kPositionTypeCurrent);
                }

                break;
            }
            else
            {
                if(pLine && (nCount < (nMaxCount - 1))) // '- 1' because we want to leave room for terminating null.
                {
                    if(endianSource != kEndianLocal)
                        cCurrent = SwizzleUint16(cCurrent);

                    *pLine++ = cCurrent;
                }

                ++nCount;
            }
        }
        else
        {
            // In this case, there was nothing left to read in the file.
            // We need to differentiate between an empty line vs. nothing 
            // left to read in the file. To deal with that we return kSizeTypeDone.
            if(nRead == 0)
                nCount = kSizeTypeDone;

            break;
        }
    }

    if(pLine)
        *pLine = 0;
    else
        pIS->SetPosition(nInitialPosition);

    return nCount;
}


EAIO_API bool EA::IO::WriteBool8(IStream* pOS, bool value)
{
    const uint8_t n(value ? 1u : 0u);
    return pOS->Write(&n, sizeof(n)) == sizeof(n);
}


EAIO_API bool EA::IO::WriteBool8(IStream* pOS, const bool* value, size_type count)
{
    return pOS->Write(value, count * sizeof(*value));
}


EAIO_API bool EA::IO::WriteInt8(IStream* pOS, int8_t value)
{
    return pOS->Write(&value, sizeof(value)) == sizeof(value);
}


EAIO_API bool EA::IO::WriteInt8(IStream* pOS, const int8_t* value, size_type count)
{
    return pOS->Write(value, count * sizeof(*value));
}


EAIO_API bool EA::IO::WriteUint8(IStream* pOS, uint8_t value)
{
    return pOS->Write(&value, sizeof(value)) == sizeof(value);
}


EAIO_API bool EA::IO::WriteUint8(IStream* pOS, const uint8_t* value, size_type count)
{
    return pOS->Write(value, count * sizeof(*value));
}


EAIO_API bool EA::IO::WriteUint16(IStream* pOS, uint16_t value, Endian endianDestination)
{
    if(endianDestination != kEndianLocal)
        value = SwizzleUint16(value);

    return pOS->Write(&value, sizeof(value));
}


EAIO_API bool EA::IO::WriteUint16(IStream* pOS, const uint16_t* value, size_type count, Endian endianDestination)
{
    if(endianDestination != kEndianLocal)
    {
        while(count--)
        {
            const uint16_t n = SwizzleUint16(*value++);

            if(!pOS->Write( &n, sizeof( n )))
                return false;
        }
    }
    else
    {
        if(!pOS->Write(value, count * sizeof(*value)))
            return false;
    }

    return true;
}


EAIO_API bool EA::IO::WriteInt16(IStream* pOS, int16_t value, Endian endianDestination)
{
   return WriteUint16(pOS, (uint16_t)value, endianDestination);
}


EAIO_API bool EA::IO::WriteInt16(IStream* pOS, const int16_t* value, size_type count, Endian endianDestination)
{
    return WriteUint16(pOS, (uint16_t*)value, count, endianDestination);
}


EAIO_API bool EA::IO::WriteUint32(IStream* pOS, uint32_t value, Endian endianDestination)
{
    if(endianDestination != kEndianLocal)
        value = SwizzleUint32(value);

    return pOS->Write(&value, sizeof(value));
}


EAIO_API bool EA::IO::WriteUint32(IStream* pOS, const uint32_t* value, size_type count, Endian endianDestination)
{
    if(endianDestination != kEndianLocal)
    {
        while(count--)
        {
            const uint32_t n = SwizzleUint32(*value++);

            if(!pOS->Write(&n, sizeof(n)))
                return false;
        }
    }
    else
    {
        if(!pOS->Write(value, count * sizeof(*value)))
            return false;
    }

    return true;
}


EAIO_API bool EA::IO::WriteInt32(IStream* pOS, int32_t value, Endian endianDestination)
{
   return WriteUint32(pOS, (uint32_t)value, endianDestination);
}


EAIO_API bool EA::IO::WriteInt32(IStream* pOS, const int32_t* value, size_type count, Endian endianDestination)
{
    return WriteUint32(pOS, (uint32_t*)value, count, endianDestination);
}


EAIO_API bool EA::IO::WriteUint64(IStream* pOS, uint64_t value, Endian endianDestination)
{
    if(endianDestination != kEndianLocal)
        value = SwizzleUint64(value);

    return pOS->Write(&value, sizeof(value));
}


EAIO_API bool EA::IO::WriteUint64(IStream* pOS, const uint64_t* value, size_type count, Endian endianDestination)
{
    if(endianDestination != kEndianLocal)
    {
        while(count--)
        {
            const uint64_t n = SwizzleUint64(*value++);

            if(!pOS->Write(&n, sizeof(n)))
                return false;
        }
    }
    else
    {
        if(!pOS->Write(value, count * sizeof(*value)))
            return false;
    }

    return true;
}


EAIO_API bool EA::IO::WriteInt64(IStream* pOS, int64_t value, Endian endianDestination)
{
   return WriteUint64(pOS, (uint64_t)value, endianDestination);
}


EAIO_API bool EA::IO::WriteInt64(IStream* pOS, const int64_t* value, size_type count, Endian endianDestination)
{
    return WriteUint64(pOS, (uint64_t*)value, count, endianDestination);
}


//EAIO_API bool EA::IO::WriteUint128(IStream* pOS, uint128_t value, Endian endianDestination)
//{
//    if(endianDestination != kEndianLocal)
//       value = SwizzleUint128(value);
//    return pOS->Write(&value, sizeof(value));
//


//EAIO_API bool EA::IO::WriteInt128(IStream* pOS, int128_t value, Endian endianDestination)
//{
//    return WriteUint128(pOS, (uint128_t)value, endianDestination);
//}


EAIO_API bool EA::IO::WriteFloat(IStream* pOS, float value, Endian endianDestination)
{
    return WriteFloat(pOS, &value, 1, endianDestination);
}


EAIO_API bool EA::IO::WriteFloat(IStream* pOS, const float* value, size_type count, Endian endianDestination)
{
    return WriteUint32(pOS, (const uint32_t*)(const char*)value, count, endianDestination);
}


EAIO_API bool EA::IO::WriteDouble(IStream* pOS, double value, Endian endianDestination)
{
    return WriteDouble(pOS, &value, 1, endianDestination);
}


EAIO_API bool EA::IO::WriteDouble(IStream* pOS, const double* value, size_type count, Endian endianDestination)
{
    return WriteUint64(pOS, (const uint64_t*)(const char*)value, count, endianDestination);
}


EAIO_API bool EA::IO::WriteString(IStream* pOS, const char8_t* pBuffer, size_t nCount, Endian endianSource)
{
    bool bResult(true);

    if(nCount == kLengthNull)
    {
        nCount = 0;

        // For maximal portability and modularity, we invent our own strlen function here.
        const char8_t* pCurrent = pBuffer;
        while(*pCurrent++)
            ++nCount;
    }

    // Embed the string's length at the nCount of the buffer.
    bResult = WriteUint32(pOS, (uint32_t)nCount, endianSource);

    if(bResult && nCount)
        bResult = pOS->Write(pBuffer, nCount * sizeof(char8_t));

    return bResult;
}


EAIO_API bool EA::IO::WriteString(IStream* pOS, const char16_t* pBuffer, size_t nCount, Endian endianSource)
{
    bool bResult(true);

    if(nCount == kLengthNull)
    {
        nCount = 0;

        // For maximal portability and modularity, we invent our own strlen function here.
        const char16_t* pCurrent = pBuffer;
        while(*pCurrent++)
            ++nCount;
    }

    // Embed the string's length at the beginning of the buffer.
    bResult = WriteUint32(pOS, (uint32_t)nCount, endianSource);

    if(bResult && nCount)
        bResult = WriteUint16(pOS, (const uint16_t*)pBuffer, nCount, endianSource);

    return bResult;
}


EAIO_API bool EA::IO::WriteLine(IStream* pOS, const char8_t* pLine, size_type nCount, LineEnd lineEndToUse)
{
    bool bResult(true);

    if(nCount == kLengthNull)
    {  
        nCount = 0;

        // For maximal portability and modularity, we invent our own strlen function here.
        const char8_t* pCurrent = pLine;
        while(*pCurrent++)
            ++nCount;
    }

    if(nCount)
        bResult = pOS->Write(pLine, nCount * sizeof(char8_t));

    if(bResult)
    {
        if(lineEndToUse == kLineEndAuto)
        {
            if(!nCount || ((pLine[nCount - 1] != '\n') && (pLine[nCount - 1] != '\r')))
                lineEndToUse = kLineEndNewline;
        }

        if(lineEndToUse == kLineEndWindows)
        {
            const char8_t terminator[2]= { '\r', '\n' };
            bResult = pOS->Write(terminator, sizeof(terminator));

        }
        else if(lineEndToUse == kLineEndNewline)
        {
            const char8_t terminator[1]= { '\n' };
            bResult = pOS->Write(terminator, sizeof(terminator));
        }
    }

    return bResult;
}


EAIO_API bool EA::IO::WriteLine(IStream* pOS, const char16_t* pLine, size_type nCount, LineEnd lineEndToUse, Endian endianDestination)
{
    bool bResult(true);

    if(nCount == kLengthNull)
    {
        nCount = 0;

        // For maximial portability and modularity, we invent our own strlen function here for char16_t.
        const char16_t* pCurrent = pLine;
        while(*pCurrent++)
            ++nCount;
    }

    if(nCount)
        bResult = WriteUint16(pOS, (const uint16_t*)pLine, nCount, endianDestination);

    if(bResult)
    {
        if(lineEndToUse == kLineEndAuto)
        {
            if(!nCount || ((pLine[nCount - 1] != '\n') && (pLine[nCount - 1] != '\r')))
                lineEndToUse = kLineEndNewline;
        }

        if(lineEndToUse == kLineEndWindows)
        {
            const uint16_t terminator[2]= { '\r', '\n' };
            bResult = WriteUint16(pOS, terminator, 2, endianDestination);

        }
        else if(lineEndToUse == kLineEndNewline)
        {
            const uint16_t terminator[1]= { '\n' };
            bResult = WriteUint16(pOS, terminator, endianDestination);
        }
    }

    return bResult;
}

