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
	, mnSize( nSize )
	, mnCapacity( nSize )
	, mnPosition( 0 )
{
}

///////////////////////////////////////////////////////////////////////////////
EA::IO::FixedMemoryStream::FixedMemoryStream( FixedMemoryStream& other )
	: StreamTmpl(other)
	, mpData( other.mpData )
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

			memmove( pData, (const uint8_t*)mpData + mnPosition, (size_t)nSize );
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







