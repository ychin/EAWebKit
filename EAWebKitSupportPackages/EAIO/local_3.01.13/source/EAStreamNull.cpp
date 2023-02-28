/////////////////////////////////////////////////////////////////////////////
// EAStreamNull.cpp
//
// Copyright (c) 2007, Electronic Arts Inc. All rights reserved.
// Written by Paul Pedriana
/////////////////////////////////////////////////////////////////////////////


#include <EAIO/internal/Config.h>
#include <EAIO/EAStreamNull.h>


namespace EA{

namespace IO{


///////////////////////////////////////////////////////////////////////////////
// StreamNull
//
StreamNull::StreamNull()
{
}


///////////////////////////////////////////////////////////////////////////////
// StreamNull
//
StreamNull::StreamNull(const StreamNull&x)
	: StreamTmpl(x)
{
}


///////////////////////////////////////////////////////////////////////////////
// operator=
//
StreamNull& StreamNull::operator=(const StreamNull&)
{
	return *this;
}


///////////////////////////////////////////////////////////////////////////////
// GetAccessFlags
//
int StreamNull::GetAccessFlags() const
{
	return kAccessFlagReadWrite;
}


///////////////////////////////////////////////////////////////////////////////
// GetState
//
int StreamNull::GetState() const
{
	return kStateSuccess;
}


///////////////////////////////////////////////////////////////////////////////
// Close
//
bool StreamNull::Close()
{
	// To consider: Have the Close function cause future accesses to fail.
	return true;
}


///////////////////////////////////////////////////////////////////////////////
// GetSize
//
size_type StreamNull::GetSize() const
{
	// To consider; Have the GetSize function return a count of the 
	// number of bytes that have been written.
	return 0;
}


///////////////////////////////////////////////////////////////////////////////
// SetSize
//
bool StreamNull::SetSize(size_type)
{
	return true;
}


///////////////////////////////////////////////////////////////////////////////
// GetPosition
//
off_type StreamNull::GetPosition(PositionType /*positionType*/) const
{
	return 0;
}


///////////////////////////////////////////////////////////////////////////////
// SetPosition
//
bool StreamNull::SetPosition(off_type /*nPosition*/, PositionType /*positionType*/)
{
	return true;
}

		  

///////////////////////////////////////////////////////////////////////////////
// GetAvailable
//
size_type StreamNull::GetAvailable() const
{
	return 0x0fffffff;
}


///////////////////////////////////////////////////////////////////////////////
// Read
//
size_type StreamNull::Read(void* /*pData*/, size_type nSize)
{
	// We don't do this, as it merely burns CPU cycles that have no meaning.
	// memset(pData, 0, nSize);
	return nSize;
}


///////////////////////////////////////////////////////////////////////////////
// Flush
//
bool StreamNull::Flush()
{
	return true;
}


///////////////////////////////////////////////////////////////////////////////
// Write
//
bool StreamNull::Write(const void* /*pData*/, size_type /*nSize*/)
{
	return true;
}




} // namespace IO

} // namespace EA















