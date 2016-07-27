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
// EAStreamFixedMemory.cpp
//
// Copyright (c) 2007, Electronic Arts Inc. All rights reserved.
// Written by Talin
// Based on EAStreamMemory.h by Paul Pedriana.
//
// Implements a IO stream that reads from a fixed-length block of memory.
/////////////////////////////////////////////////////////////////////////////


#include <EAIO/internal/Config.h>
#include <EAIO/EAStreamFixedMemory.h>
#include <string.h>
#include EA_ASSERT_HEADER


///////////////////////////////////////////////////////////////////////////////
// FixedMemoryStream
///////////////////////////////////////////////////////////////////////////////

///////////////////////////////////////////////////////////////////////////////
EA::IO::FixedMemoryStream::FixedMemoryStream( void* pData, size_type nSize )
    : mpData( pData )
    , mnRefCount( 0 )
    , mnSize( nSize )
    , mnCapacity( nSize )
    , mnPosition( 0 )
{
}

///////////////////////////////////////////////////////////////////////////////
EA::IO::FixedMemoryStream::FixedMemoryStream( FixedMemoryStream& other )
    : IStream( )
    , mpData( other.mpData )
    , mnRefCount( 0 )
    , mnSize( other.mnSize )
    , mnCapacity( other.mnSize )
    , mnPosition( other.mnPosition )
{
}

///////////////////////////////////////////////////////////////////////////////
bool EA::IO::FixedMemoryStream::SetData( void* pData, size_type nSize )
{
    mpData = pData;
    mnSize = mnCapacity = nSize;
    mnPosition = 0;
    return true;
}

///////////////////////////////////////////////////////////////////////////////
bool EA::IO::FixedMemoryStream::SetSize( size_type size )
{
    if (size <= mnCapacity) // If there is a change...
    {
        mnSize = size;
        if (mnPosition > size)
            mnPosition = size;
        return true;
    }

    return false;
}

///////////////////////////////////////////////////////////////////////////////
EA::IO::off_type EA::IO::FixedMemoryStream::GetPosition( PositionType positionType ) const
{
    switch(positionType)
    {
        case kPositionTypeBegin:
            return (off_type)mnPosition;

        case kPositionTypeEnd:
            return (off_type)(mnPosition - mnSize);

        case kPositionTypeCurrent:
        default:
            break;
    }

    return 0; // For kPositionTypeCurrent the result is always zero for a 'get' operation.
}

///////////////////////////////////////////////////////////////////////////////
bool EA::IO::FixedMemoryStream::SetPosition( off_type nPosition, PositionType positionType )
{
    switch (positionType)
    {
        case kPositionTypeBegin:
            EA_ASSERT(nPosition >= 0);
            mnPosition = (size_type)nPosition; // We deal with negative positions below.
            break;

        case kPositionTypeCurrent:
            mnPosition = mnPosition + (size_type)nPosition;  // We have a signed/unsigned match, but the math should work anyway.
            break;

        case kPositionTypeEnd:
            mnPosition = mnSize + nPosition; // We deal with invalid resulting positions below.
            break;
    }

    // Deal with out-of-bounds situations that result from the above.
    if (mnPosition > mnSize)
    {
        EA_ASSERT( mnPosition < (size_type(-1) / 2) );
        mnPosition = mnSize;
        return false;
    }

    return true;
}

///////////////////////////////////////////////////////////////////////////////
EA::IO::size_type EA::IO::FixedMemoryStream::Read( void* pData, size_type nSize )
{
    if (nSize > 0)
    {
        EA_ASSERT(mnPosition <= mnSize);
        const size_type nBytesAvailable( mnSize - mnPosition );

        if (nBytesAvailable > 0)
        {
            if (nSize > nBytesAvailable)
                nSize = nBytesAvailable;

            memcpy( pData, (const uint8_t*)mpData + mnPosition, (size_t)nSize );
            mnPosition += nSize;

            return nSize;
        }
    }

    return 0;
}

///////////////////////////////////////////////////////////////////////////////
bool EA::IO::FixedMemoryStream::Write( const void* pData, size_type nSize )
{
    if (nSize > 0)
    {
        EA_ASSERT(mnPosition <= mnSize);
        size_type nRequiredSize( mnPosition + nSize );
        size_type nBytesToWrite( nSize );

        if (nRequiredSize > mnCapacity) // If we need to increase our capacity...
            nBytesToWrite = (mnSize - mnPosition);
        else if (mnSize < nRequiredSize)
           mnSize = nRequiredSize;

        // We assume that 99% of the time that 'nBytesToWrite' is > 0, 
        // so we don't bother to check to see if it is zero here.
        EA_ASSERT(mpData && pData);
        memcpy( (uint8_t *)mpData + mnPosition, pData, (size_t)nBytesToWrite );
        mnPosition += nBytesToWrite;

        return (nBytesToWrite == nSize); // This will be false if the user tried to write beyond the end.
    }

    return true;
}







