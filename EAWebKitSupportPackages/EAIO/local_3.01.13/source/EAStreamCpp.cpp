///////////////////////////////////////////////////////////////////////////////
// EAStreamCpp.cpp
//
// Copyright (c) 2006 Electronic Arts Inc.
// Created by Paul Pedriana
//
// Implements a stream which wraps around a C++ std::istream and std::ostream.
///////////////////////////////////////////////////////////////////////////////



#include <EAIO/internal/Config.h>

#if EAIO_CPP_STREAM_ENABLED

#include <EAIO/EAStreamCpp.h>

#if defined(_MSC_VER)
	#pragma warning(push, 0)
	#pragma warning(disable: 4275) // non dll-interface class 'stdext::exception' used as base for dll-interface class 'std::bad_cast'
#endif

#include <iostream>
#include <assert.h>

#if defined(_MSC_VER)
	#pragma warning(pop)
#endif



namespace EA
{
namespace IO
{


int StreamCpp::GetAccessFlags() const
{
	int flags = 0;

	// This is the best we do for now.
	if(mpStdIstream && !mpStdIstream->bad())
		flags |= kAccessFlagRead;

	if(mpStdOstream && !mpStdOstream->bad())
		flags |= kAccessFlagWrite;

	return flags;
}


int StreamCpp::GetState() const
{
	if((mpStdIstream && mpStdIstream->bad()) || 
	   (mpStdOstream && mpStdOstream->bad()))
	{
		return kStateError;
	}

	return kStateSuccess;
}


size_type StreamCpp::GetSize() const
{
	// Currently we assume there is an istream present.
	if(mpStdIstream)
	{
		const std::streampos pos = mpStdIstream->tellg();
		mpStdIstream->seekg(0, std::ios::end);

		const std::streampos length = mpStdIstream->tellg(); // tellg returns -1 (coincidentally kSizeTypeError) upon error.
		mpStdIstream->seekg(pos);

		return (size_type)length;
	}

	return kSizeTypeError;
}


bool StreamCpp::SetSize(size_type size)
{
	// Currently we assume there is an ostream present.
	if(mpStdOstream)
	{
		mpStdOstream->seekp((std::streamoff)size, std::ios::beg);
		// We may need to write a byte here but make sure we don't overwrite any existing byte.
		return (mpStdOstream->rdstate() & (std::ios::failbit | std::ios::badbit)) == 0;
	}
	return false;
}


off_type StreamCpp::GetPosition(PositionType positionType) const
{
	// Currently we assume there is an istream present.
	if(mpStdIstream)
	{
		switch(positionType)
		{
			case kPositionTypeBegin:
			{
				const std::streampos pos = mpStdIstream->tellg(); // tellg returns -1 (coincidentally kSizeTypeError) upon error.
				return (off_type)pos; 
			}

			case kPositionTypeEnd:
			{
				const std::streampos pos = mpStdIstream->tellg(); // tellg returns -1 (coincidentally kSizeTypeError) upon error.
				if(pos != (std::streampos)-1)
					return (off_type)pos - (off_type)GetSize(); // This will yield a value <= 0.
				return (off_type)pos;
			}

			case kPositionTypeCurrent:
			default:
				return 0; // kPositionTypeCurrent, which is always 0 for a 'get' operation.
		}
	}

	return (off_type)kSizeTypeError; // Should this return 0 instead of kSizeTypeError?
}


bool StreamCpp::SetPosition(off_type position, PositionType positionType)
{
	// Currently we assume there is an istream present.
	if(mpStdIstream)
	{
		switch(positionType)
		{
			case kPositionTypeBegin:
				mpStdIstream->seekg((std::streamoff)position, std::ios::beg);
				return !(mpStdOstream->rdstate() & (std::ios::failbit | std::ios::badbit));

			case kPositionTypeCurrent:
				return SetPosition((off_type)mpStdIstream->tellg() + position, kPositionTypeBegin);

			case kPositionTypeEnd:
				return SetPosition((off_type)mpStdIstream->tellg() + (off_type)GetSize() + position, kPositionTypeBegin);
		}
	}

	return false;
}


size_type StreamCpp::GetAvailable() const
{
	const size_type size = GetSize();

	if(size != kSizeTypeError)
		return size - GetPosition(); // We make the assumption here that if GetSize succeeded then GetPosition will succeed as well.

	return kSizeTypeError;
}


size_type StreamCpp::Read(void* pData, size_type nSize)
{
	if(mpStdIstream && ((mpStdIstream->tellg() >= 0) || mpStdIstream->eof()))
	{
		mpStdIstream->read((char*)pData, (std::streamsize)nSize);
		nSize = (size_type)mpStdIstream->gcount();

		if(!mpStdIstream->bad())
			return nSize;
	}

	return kSizeTypeError;
}


bool StreamCpp::Flush()
{ 
	if(mpStdOstream)
		mpStdOstream->flush();
	return true;
}


bool StreamCpp::Write(const void* pData, size_type nSize)
{
	if(mpStdOstream)
	{
		mpStdOstream->write((const char*)pData, (std::streamsize)nSize);
		return !mpStdOstream->bad();
	}
	return false;
}


void StreamCpp::Clear(bool clearInput, bool clearOutput)
{
	if(clearInput && mpStdIstream)
		mpStdIstream->clear();
	if(clearOutput && mpStdOstream)
		mpStdOstream->clear();
}



} // namespace IO

} // namespace EA



#endif // EAIO_CPP_STREAM_ENABLED





