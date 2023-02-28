/////////////////////////////////////////////////////////////////////////////
// EAFileStreamStdC.cpp
//
// Copyright (c) 2003, Electronic Arts Inc. All rights reserved.
// Created by Paul Pedriana
//
/////////////////////////////////////////////////////////////////////////////


#include <EAIO/internal/Config.h>
#include <EAIO/EAFileStream.h>
#include <coreallocator/icoreallocator.h>
#include EA_ASSERT_HEADER
#include <stdio.h>
#include <errno.h>




namespace EA
{

namespace IO
{


const int kFileHandleInvalid = -1;


FileStream::FileStream(const char* pPath8)
  : mnFileHandle(kFileHandleInvalid),
	mpPath8(NULL),
	mnAccessFlags(0),
	mnCD(0),
	mnSharing(0),
	mnUsageHints(kUsageHintNone),
	mnLastError(kStateNotOpen)
{
	FileStream::SetPath(pPath8); // Note that in a constructor, the virtual function mechanism is inoperable.
}


FileStream::FileStream(const char16_t* pPath16)
  : mnFileHandle(kFileHandleInvalid),
	mpPath8(NULL),
	mnAccessFlags(0),
	mnCD(0),
	mnSharing(0),
	mnUsageHints(kUsageHintNone),
	mnLastError(kStateNotOpen)
{
	FileStream::SetPath(pPath16); // Note that in a constructor, the virtual function mechanism is inoperable.
}


FileStream::FileStream(const char32_t* pPath32)
  : mnFileHandle(kFileHandleInvalid),
	mpPath8(NULL),
	mnAccessFlags(0),
	mnCD(0),
	mnSharing(0),
	mnUsageHints(kUsageHintNone),
	mnLastError(kStateNotOpen)
{
	FileStream::SetPath(pPath32); // Note that in a constructor, the virtual function mechanism is inoperable.
}


#if EA_WCHAR_UNIQUE
	FileStream::FileStream(const wchar_t* pPathW)
	  : mnFileHandle(kFileHandleInvalid),
		mpPath8(NULL),
		mnAccessFlags(0),
		mnCD(0),
		mnSharing(0),
		mnUsageHints(kUsageHintNone),
		mnLastError(kStateNotOpen)
	{
		FileStream::SetPath(pPathW); // Note that in a constructor, the virtual function mechanism is inoperable.
	}
#endif


FileStream::FileStream(const FileStream& fs)
  : mnFileHandle(kFileHandleInvalid),
	mpPath8(NULL),
	mnAccessFlags(0),
	mnCD(0),
	mnSharing(0),
	mnUsageHints(fs.mnUsageHints),
	mnLastError(kStateNotOpen)
{
	FileStream::SetPath(fs.mpPath8); // Note that in a constructor, the virtual function mechanism is inoperable.
}


FileStream::~FileStream()
{
	FileStream::Close(); // Note that in a destructor, the virtual function mechanism is inoperable.
}


FileStream& FileStream::operator=(const FileStream& fs)
{
	if(&fs != this)
	{
		Close();
		SetPath(fs.mpPath8);

		mnFileHandle  = kFileHandleInvalid;
		mnAccessFlags = 0;
		mnCD          = 0;
		mnSharing     = 0;
		mnUsageHints  = fs.mnUsageHints;
		mnLastError   = kStateNotOpen;
	}

	return *this;
}


void FileStream::SetPath(const char* pPath8)
{
	if((mnFileHandle == kFileHandleInvalid) && pPath8)
	{
		// Convert UTF8 to UTF16.
	}  
}


void FileStream::SetPath(const char16_t* pPath16)
{
	if((mnFileHandle == kFileHandleInvalid) && pPath16)
	{
		// Copy path.
	}  
}


void FileStream::SetPath(const char32_t* pPath32)
{
	if((mnFileHandle == kFileHandleInvalid) && pPath32)
	{
		// Copy path.
	}  
}


#if EA_WCHAR_UNIQUE
	void FileStream::SetPath(const wchar_t* pPathW)
	{
		if((mnFileHandle == kFileHandleInvalid) && pPathW)
		{
			// Copy path.
		}  
	}
#endif


size_t FileStream::GetPath(char* pPath8, size_t nPathLength)
{
	// Convert from UTF16 to UTF8.
	return 0;
}


size_t FileStream::GetPath(char16_t* pPath16, size_t nPathLength)
{
	if(pPath16 && mpPath8)
	{
		// Copy path.
	}
	return 0;
}


size_t FileStream::GetPath(char32_t* pPath32, size_t nPathLength)
{
	if(pPath32 && mpPath8)
	{
		// Copy path.
	}
	return 0;
}


#if EA_WCHAR_UNIQUE
	size_t FileStream::GetPath(wchar_t* pPathW, size_t nPathLength)
	{
		if(pPathW && mpPath8)
		{
			// Copy path.
		}
		return 0;
	}
#endif


bool FileStream::Open(int nAccessFlags, int nCreationDisposition, int nSharing, int nUsageHints)
{
	if((mnFileHandle == kFileHandleInvalid) && nAccessFlags) // If not already open and if some kind of access is requested...
	{
		// Not yet implemented.

		/*
		int nOpenFlags(0);

		if(nAccessFlags == kAccessFlagRead)
			nOpenFlags = O_RDONLY;
		else if(nAccessFlags == kAccessFlagWrite)
			nOpenFlags = O_WRONLY;
		else if(nAccessFlags == kAccessFlagReadWrite)
			nOpenFlags = O_RDWR;
		//nOpenFlags |= O_BINARY;

		if(nCreationDisposition == kCDDefault)
		{
			// To consider: A proposal is on the table that specifies that the 
			// default disposition for write is kCDCreateAlways and the default
			// disposition for read/write is kCDOpenAlways. However, such a change
			// may break existing code.
			if(nAccessFlags & kAccessFlagWrite)
				nCreationDisposition = kCDOpenAlways;
			else
				nCreationDisposition = kCDOpenExisting;
		}

		switch(nCreationDisposition)
		{
			case kCDCreateNew:
				nOpenFlags |= O_EXCL;       // Open only if it doesn't already exist.
				break;

			case kCDCreateAlways:
				nOpenFlags |= O_CREAT;      // Always make it like the file was just created.
				nOpenFlags |= O_TRUNC;
				break;

			case kCDOpenExisting:           // Open the file if it exists, else fail.
				break;                      // Nothing to do.

			case kCDOpenAlways:
				nOpenFlags |= O_CREAT;      // Open the file no matter what.
				break;

			case kCDTruncateExisting:       // Open the file if it exists, and truncate it if so.
				nOpenFlags |= O_TRUNC;
				break;
		}

		mnFileHandle = open(mpPath8, nOpenFlags);

		if(mnFileHandle == kFileHandleInvalid) // If it failed...
		{
			// mnAccessFlags = 0; // Not necessary, as these should already be zeroed.
			// mnCD          = 0;
			// mnSharing     = 0;
			// mnUsageHints  = 0;
			mnLastError      = errno;
		}
		else
		{
			mnAccessFlags = nAccessFlags;
			mnCD          = nCreationDisposition;
			mnSharing     = nSharing;
			mnUsageHints  = nUsageHints;
			mnLastError   = 0;
		}
		*/
	}

	return (mnFileHandle != kFileHandleInvalid);
}


bool FileStream::Close()
{
	if((mnFileHandle != kFileHandleInvalid))
	{
		// Not implemented
		// close(mnFileHandle); // This returns -1 upon error. But there's not much to do about it.

		mnFileHandle  = 0;
		mnAccessFlags = 0;
		mnCD          = 0;
		mnSharing     = 0;
		mnUsageHints  = 0;
		mnLastError   = kStateNotOpen;
	}

	return true;
}


int FileStream::GetAccessFlags() const
{
	return mnAccessFlags;
}


int FileStream::GetState() const
{
	return mnLastError;
}


size_type FileStream::GetSize() const
{
	if(mnFileHandle != kFileHandleInvalid)
	{
		// Not yet implemented.

		/*
		const off_t nPositionSaved = lseek(mnFileHandle, 0, SEEK_CUR);

		if(nPositionSaved >= 0)
		{
			const off_t nSize = lseek(mnFileHandle, 0, SEEK_END);

			if(nSize >= 0)
			{
				lseek(mnFileHandle, nPositionSaved, SEEK_SET);
				return (size_type)nSize;
			}
		}

		mnLastError = errno;
		*/
	}

	return kSizeTypeError;
}


bool FileStream::SetSize(size_type size)
{
	if(mnFileHandle != kFileHandleInvalid)
	{
		// Not yet implemented.

		// Technically speaking, the standard states that the file size isn't guaranteed to 
		// increase for all implementations under all circumstances.
		//const int result = ftruncate(mnFileHandle, (off_t)size);
		//if(result == 0) // If the result is OK...
		//    return true;
		//mnLastError = errno;
	}
	return false;
}


off_type FileStream::GetPosition(PositionType positionType) const
{
	off_type result;

	// Not yet implemented.

	/*
	if(positionType == kPositionTypeBegin)
		result = (off_type)lseek(mnFileHandle, 0, SEEK_CUR);
	else if(positionType == kPositionTypeEnd)
	{
		result = (off_type)lseek(mnFileHandle, 0, SEEK_CUR);

		if(result != (off_type)-1)
		{
			const size_type nSize = GetSize();
			if(nSize != kSizeTypeError)
				result = (off_type)(result - (off_type)nSize);
		}
	}
	else */
		result = 0;
	return result;
}


bool FileStream::SetPosition(off_type position, PositionType positionType)
{
	if(mnFileHandle != kFileHandleInvalid)
	{
		// Not yet implemented.

		/*
		int nMethod;

		switch(positionType)
		{
			default:
			case kPositionTypeBegin:
				nMethod = SEEK_SET;
				break;

			case kPositionTypeCurrent:
				nMethod = SEEK_CUR;
				break;

			case kPositionTypeEnd:
				nMethod = SEEK_END;
				break;
		}

		const off_t nResult = lseek(mnFileHandle, (off_t)position, nMethod);

		if(nResult != -1)
			return true;

		mnLastError = errno;
		*/
	}

	return false;
}


size_type FileStream::GetAvailable() const
{
	const off_type nPosition = GetPosition(kPositionTypeEnd);
	return (size_type)-nPosition;
}


size_type FileStream::Read(void* pData, size_type nSize)
{
	if(mnFileHandle != kFileHandleInvalid)
	{
		// Not yet implemented.

		//const size_t nCount = (size_t)read(mnFileHandle, pData, (size_t)nSize);
		//if(nCount != (size_t)-1)
		//    return (size_type)nCount;
	}
	return kSizeTypeError;
}


bool FileStream::Write(const void* pData, size_type nSize)
{
	if(mnFileHandle != kFileHandleInvalid)
	{
		// Not yet implemented.

		//const size_t nCount = (size_t)write(mnFileHandle, pData, (size_t)nSize);
		//if(nCount != (size_t)-1)
		//    return true;
	}
	return false;
}


bool FileStream::Flush()
{
	if(mnFileHandle != kFileHandleInvalid)
	{
		// Not yet implemented.

		// fsync(mnFileHandle);
	}
	return true;
}


} // namespace IO


} // namespace EA















