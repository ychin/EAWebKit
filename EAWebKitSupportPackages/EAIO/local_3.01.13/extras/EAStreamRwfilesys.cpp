/////////////////////////////////////////////////////////////////////////////
// EAStreamRwfilesys.cpp
//
/////////////////////////////////////////////////////////////////////////////


#include "EAStreamRwfilesys.h"
#include <EAIO/FnEncode.h>
#include <EAStdC/EAString.h>
#include <string.h>
#include <stdio.h>


namespace EA
{

namespace IO
{



RwfilesysStream::RwfilesysStream(const char* pPath8)
  : mFileStream(),
	mnRefCount(0),
	mnAccessFlags(0),
	mnCD(0),
	mnLastError(kStateNotOpen)
{
	mPath8[0] = 0;
	RwfilesysStream::SetPath(pPath8); // Note that in a constructor, the virtual function mechanism is inoperable.
}


RwfilesysStream::RwfilesysStream(const char16_t* pPath16)
  : mFileStream(),
	mnRefCount(0),
	mnAccessFlags(0),
	mnCD(0),
	mnLastError(kStateNotOpen)
{
	mPath8[0] = 0;
	RwfilesysStream::SetPath(pPath16); // Note that in a constructor, the virtual function mechanism is inoperable.
}


RwfilesysStream::RwfilesysStream(const RwfilesysStream& fs)
  : mFileStream(),
	mnRefCount(0),
	mnAccessFlags(0),
	mnCD(0),
	mnLastError(kStateNotOpen)
{
	mPath8[0] = 0;
	RwfilesysStream::SetPath(fs.mPath8); // Note that in a constructor, the virtual function mechanism is inoperable.
}


RwfilesysStream::~RwfilesysStream()
{
	RwfilesysStream::Close(); // Note that in a destructor, the virtual function mechanism is inoperable.
}


RwfilesysStream& RwfilesysStream::operator=(const RwfilesysStream& fs)
{
	Close();
	SetPath(fs.mPath8);

	mnAccessFlags = 0;
	mnCD          = 0;
	mnLastError   = kStateNotOpen;

	return *this;
}


int RwfilesysStream::AddRef()
{
	return ++mnRefCount;
}


int RwfilesysStream::Release()
{
	if(mnRefCount > 1)
		return --mnRefCount;
	delete this;
	return 0;
}


void RwfilesysStream::SetPath(const char* pPath8)
{
	// To consider: Assert (mnAccessFlags == 0).
	if(pPath8)
		EA::StdC::Strlcpy(mPath8, pPath8, kMaxPathLength);
}


void RwfilesysStream::SetPath(const char16_t* pPath16)
{
	// To consider: Assert (mnAccessFlags == 0).
	if(pPath16)
		EA::StdC::Strlcpy(mPath8, pPath16, kMaxPathLength);
}


size_t RwfilesysStream::GetPath(char* pPath8, size_t nPathLength)
{
	// Return the length of the path we -tried- to create.
	return EA::StdC::Strlcpy(pPath8, mPath8, nPathLength);
}


size_t RwfilesysStream::GetPath(char16_t* pPath16, size_t nPathLength)
{
	if(pPath16)
		return EA::StdC::Strlcpy(pPath16, mPath8, nPathLength);

	// In this case the user wants just the length of the return value.
	char16_t pathTemp[kMaxPathLength];
	return GetPath(pathTemp, kMaxPathLength);
}


bool RwfilesysStream::Open(int nAccessFlags, int nCreationDisposition)
{
	bool bResult = false;

	if((mpFile == NULL) && nAccessFlags) // If not already open and if some kind of access is requested...
	{
		// rw::core::filesys::AsyncOp::

		/*
		FOPEN_READONLY      = 0x00,     ///< open the file in read-only mode (default)
		FOPEN_READWRITE     = 0x01,     ///< open the file in read-write mode.  This flag must be used when a new file is created, otherwise it will not be writable.
		FOPEN_CREATE        = 0x02,     ///< create the file if it does not already exist. If the file does already exist, the open will fail.
		FOPEN_OVERWRITE     = 0x04,     ///< truncate the file if it does exist
		FOPEN_SEQUENTIAL    = 0x08,     ///< hint to file system that the file will be read sequentially. Correct operation is still guaranteed even if the file is not read sequentially.
		FOPEN_NOBUFFERING   = 0x10,     ///< Maps to FILE_FLAG_NO_BUFFERING on Microsoft-based systems. Refer to documentation for CreateFile for information on restrictions (and advantages) this flag implies.
		FOPEN_READWHOLEFILE = 0x20,     ///< If the entire file will be read in a single Read, or if done in multiple reads, the parts which were previously read will not be modified until the entire file has been read. Use of this flag allows the file system to avoid the need to allocate a temporary sliding window buffer when decompressing refpack-compressed files. If the file is not compressed, this flag is ignored.
		FOPEN_DRM           = 0x40      ///< Open a file that is controlled by digital rights management.  Be aware that this feature is still considered a work in progress and could change in a future release.  We're still trying to figure out the best way of supporting files under DRM on the PS3.
		*/

		char accessFlags[4] = { 0 };

		// The C language specification for the access flags string is cocamamie.
		// We try to detangle it here to the best of our ability.

		if(nAccessFlags == kAccessFlagRead)
			strcpy(accessFlags, "rb");
		else if(nAccessFlags == kAccessFlagWrite)
		{
			if(nCreationDisposition == kCDOpenExisting)
				strcpy(accessFlags, "wb");
			else
				strcpy(accessFlags, "a+b");
		}
		else if(nAccessFlags == kAccessFlagReadWrite)
		{
			if(nCreationDisposition == kCDOpenExisting)
				strcpy(accessFlags, "r+b");
			else
				strcpy(accessFlags, "a+b");
		}

		bResult = (mFileStream.Open(mPath8, unsigned int modeflags) == 0);

		if(bResult) // If it failed...
		{
			mnAccessFlags = nAccessFlags;
			mnCD          = nCreationDisposition;
			mnLastError   = 0;
		}
		else
		{
			// mnAccessFlags = 0; // Not necessary, as these should already be zeroed.
			// mnCD          = 0;
			mnLastError      = errno;
		}
	}

	return bResult;
}


bool RwfilesysStream::Close()
{
	if(mnAccessFlags)
	{
		mFileStream.Close();

		mnAccessFlags = 0;
		mnCD          = 0;
		mnLastError   = kStateNotOpen;
	}

	return true;
}


int RwfilesysStream::GetAccessFlags() const
{
	return mnAccessFlags;
}


int RwfilesysStream::GetState() const
{
	return mnLastError;
}


size_type RwfilesysStream::GetSize() const
{
	if(mnAccessFlags)
		return (size_type)mFileStream.Size();

	return kSizeTypeError;
}


bool RwfilesysStream::SetSize(size_type size)
{
	if(mnAccessFlags)
	{
		const off_type savedPosition = GetPosition();

		if(mFileStream.Seek((int64_t)size, rw::core::filesys::IFileStream::SEEKORIGIN_BEGINNING) == 0)
			return true;

		SetPosition(savedPosition);
		mnLastError = kSizeTypeError;
	}

	return false;
}


off_type RwfilesysStream::GetPosition(PositionType positionType) const
{
	off_type result;

	if(positionType == kPositionTypeBegin)
		result = (off_type)Tell();
	else if(positionType == kPositionTypeEnd)
	{
		result = (off_type)Tell();

		if(result != (off_type)-1)
		{
			const size_type nSize = GetSize();

			if(nSize != kSizeTypeError)
				result = (off_type)(result - (off_type)nSize);
		}
	}
	else
		result = 0;

	if(result == -1)
		mnLastError = errno;

	return result;
}


bool RwfilesysStream::SetPosition(off_type position, PositionType positionType)
{
	if(mpFile != NULL)
	{
		int nMethod;

		switch(positionType)
		{
			default:
			case kPositionTypeBegin:
				nMethod = rw::core::filesys::IFileStream::SEEKORIGIN_BEGINNING;
				break;

			case kPositionTypeCurrent:
				nMethod = rw::core::filesys::IFileStream::SEEKORIGIN_CURRENT;
				break;

			case kPositionTypeEnd:
				nMethod = rw::core::filesys::IFileStream::SEEKORIGIN_END;
				break;
		}

		const off_t nResult = mFileStream.Seek((int64_t)position, );

		if(nResult != -1)
			return true;

		mnLastError = errno;
	}

	return false;
}


size_type RwfilesysStream::GetAvailable() const
{
	const off_type nPosition = GetPosition(kPositionTypeEnd);

	if(nPosition != kSizeTypeError)
		return (size_type)-nPosition;

	return kSizeTypeError;
}


size_type RwfilesysStream::Read(void* pData, size_type nSize)
{
	if(mnAccessFlags)
	{
		const uint64_t count = mFileStream.Read(pData, (size_t)nSize);

		if((int64_t)count >= 0)
			return (size_type)count;
		else
			mnLastError = errno;
	}

	return kSizeTypeError;
}


bool RwfilesysStream::Write(const void* pData, size_type nSize)
{
	if(mpFile != NULL)
	{
		const uint64_t count = mFileStream.Write(pData, (size_t)nSize);

		if(count == nSize)
			return true;
		else
			mnLastError = errno;
	}

	return false;
}


bool RwfilesysStream::Flush()
{
	// This is not currently possible.
	return false;
}


} // namespace IO


} // namespace EA















