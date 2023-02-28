/////////////////////////////////////////////////////////////////////////////
// EAFileStreamNX.cpp
//
// Copyright (c) 2018, Electronic Arts Inc. All rights reserved.
//
// Provides a file stream for NX based on the EAFileStreamUnix implementation.
//
/////////////////////////////////////////////////////////////////////////////


#include <EAIO/internal/Config.h>
#include <EAIO/EAFileStream.h>
#include <EAIO/EAFileUtil.h>
#include <EAIO/Allocator.h>
#include <EAIO/FnEncode.h>
#include <coreallocator/icoreallocator.h>
#include <EAStdC/EAString.h>
#include EA_ASSERT_HEADER
#include <stdio.h>
#include <unistd.h>
#include <errno.h>
#include <fcntl.h>
#include <sys/types.h>
#include <sys/stat.h>

// Some libraries don't define all access identifiers.
#ifndef S_IRUSR
	#define S_IRWXU 00700
	#define S_IRUSR 00400
	#define S_IWUSR 00200
	#define S_IXUSR 00100
#endif
#ifndef S_IRGRP
	#define S_IRWXG 00070
	#define S_IRGRP 00040
	#define S_IWGRP 00020
	#define S_IXGRP 00010
#endif
#ifndef S_IROTH
	#define S_IRWXO 00007
	#define S_IROTH 00004
	#define S_IWOTH 00002
	#define S_IXOTH 00001
#endif


#if !EAIO_FTRUNCATE_PRESENT

	#ifdef F_CHSIZE
		int ftruncate(int fd, off_t length)
		{
			return fcntl(fd, F_CHSIZE, length);
		}

	#else
		#if defined(F_FREESP)
			#include <sys/stat.h>

			int ftruncate(int fd, off_t length)
			{
				struct stat filebuf;
				const char c = 0;

				if(fstat(fd, &filebuf) < 0)
					return -1;

				if(filebuf.st_size < length) /* If increasing the file size... */
				{
					/* Extend file length. */
					if(lseek(fd, (length - 1), SEEK_SET) < 0)
						return -1;

					/* Write a "0" byte. */
					if(write(fd, &c, 1) != 1)
						return -1;
				}
				else if(filebuf.st_size > length) /* If decreasing the file size... */
				{
					#if defined(F_FREESP)
						struct flock fl;

						/* Truncate length. */
						fl.l_whence = 0;
						fl.l_len    = 0;
						fl.l_start  = length;
						fl.l_type   = F_WRLCK;    /* write lock on file space */

						/* This relies on the undocumented F_FREESP argument to fcntl,
						   which truncates the file so that it ends at the position
						   indicated by fl.l_start.  */

						if(fcntl(fd, F_FREESP, &fl) < 0)
							return -1;
					#endif
				}

				return 0;
			}

		#else /* not F_CHSIZE nor F_FREESP */

			#if HAVE_CHSIZE
				int ftruncate (int fd, off_t length)
				{
					return chsize (fd, length);
				}
			#else /* not F_CHSIZE nor F_FREESP nor HAVE_CHSIZE */
				#include <errno.h>
				#ifndef errno
					extern int errno;
				#endif

				int ftruncate(int /*fd*/, off_t /*length*/)
				{
					errno = EIO;
					return -1;
				}

			#endif /* not HAVE_CHSIZE */

		#endif /* not F_FREESP */

	#endif /* not F_CHSIZE  */

#endif




namespace EA
{

namespace IO
{


const int kFileHandleInvalid = -1;



FileStream::FileStream(const char* pPath8)
  : mnFileHandle(kFileHandleInvalid),
	mnSize(0),
	mnPos(0),
	mnAccessFlags(0),
	mnCD(0),
	mnSharing(0),
	mnLastError(kStateNotOpen)
{
	mpPath8[0] = 0;
	FileStream::SetPath(pPath8); // Note that in a constructor, the virtual function mechanism is inoperable, so we qualify the function call.
}


FileStream::FileStream(const char16_t* pPath16)
  : mnFileHandle(kFileHandleInvalid),
	mnSize(0),
	mnPos(0),
	mnAccessFlags(0),
	mnCD(0),
	mnSharing(0),
	mnLastError(kStateNotOpen)
{
	mpPath8[0] = 0;
	FileStream::SetPath(pPath16);
}


FileStream::FileStream(const char32_t* pPath32)
  : mnFileHandle(kFileHandleInvalid),
	mnSize(0),
	mnPos(0),
	mnAccessFlags(0),
	mnCD(0),
	mnSharing(0),
	mnLastError(kStateNotOpen)
{
	mpPath8[0] = 0;
	FileStream::SetPath(pPath32);
}


#if EA_WCHAR_UNIQUE
	FileStream::FileStream(const wchar_t* pPathW)
	  : mnFileHandle(kFileHandleInvalid),
		mnSize(0),
		mnPos(0),
		mnAccessFlags(0),
		mnCD(0),
		mnSharing(0),
		mnLastError(kStateNotOpen)
	{
		mpPath8[0] = 0;
		FileStream::SetPath(pPathW);
	}
#endif

FileStream::FileStream(const FileStream& fs)
  : mnFileHandle(kFileHandleInvalid),
	mnSize(0),
	mnPos(0),
	mnAccessFlags(0),
	mnCD(0),
	mnSharing(0),
	mnUsageHints(fs.mnUsageHints),
	mnLastError(kStateNotOpen)
{
	mpPath8[0] = 0;
	FileStream::SetPath(fs.mpPath8);
}


FileStream::~FileStream()
{
	FileStream::Close(); // Note that in a destructor, the virtual function mechanism is inoperable, so we qualify the function call.
}


FileStream& FileStream::operator=(const FileStream& fs)
{
	if(&fs != this)
	{
		Close();
		SetPath(fs.mpPath8);

		mnFileHandle  = kFileHandleInvalid;
		mnSize        = 0;
		mnPos         = 0;
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
		EA::StdC::Strlcpy(mpPath8, pPath8, EAArrayCount(mpPath8));
}


void FileStream::SetPath(const char16_t* pPath16)
{
	if((mnFileHandle == kFileHandleInvalid) && pPath16)
		EA::StdC::Strlcpy(mpPath8, pPath16, EAArrayCount(mpPath8)); // Convert UCS2 to UTF8.
}


void FileStream::SetPath(const char32_t* pPath32)
{
	if((mnFileHandle == kFileHandleInvalid) && pPath32)
		EA::StdC::Strlcpy(mpPath8, pPath32, EAArrayCount(mpPath8)); // Convert UCS4 to UTF8.
}


#if EA_WCHAR_UNIQUE
void FileStream::SetPath(const wchar_t* pPathW)
{
	if((mnFileHandle == kFileHandleInvalid) && pPathW)
		EA::StdC::Strlcpy(mpPath8, pPathW, EAArrayCount(mpPath8)); // Convert wchar_t to UTF8.
}
#endif


size_t FileStream::GetPath(char* pPath8, size_t nPathCapacity)
{
	// Return the required strlen of the destination path.
	return EA::StdC::Strlcpy(pPath8, mpPath8, nPathCapacity);
}


size_t FileStream::GetPath(char16_t* pPath16, size_t nPathCapacity)
{
	// Return the required strlen of the destination path.
	return EA::StdC::Strlcpy(pPath16, mpPath8, nPathCapacity);
}


size_t FileStream::GetPath(char32_t* pPath32, size_t nPathCapacity)
{
	// Return the required strlen of the destination path.
	return EA::StdC::Strlcpy(pPath32, mpPath8, nPathCapacity);
}


#if EA_WCHAR_UNIQUE
	size_t FileStream::GetPath(wchar_t* pPathW, size_t nPathCapacity)
	{
		// Return the required strlen of the destination path.
		return EA::StdC::Strlcpy(pPathW, mpPath8, nPathCapacity);
	}
#endif


bool FileStream::Open(int nAccessFlags, int nCreationDisposition, int nSharing, int nUsageHints)
{
	if((mnFileHandle == kFileHandleInvalid) && nAccessFlags) // If not already open and if some kind of access is requested...
	{
		int nOpenFlags(0);

		if(nAccessFlags == kAccessFlagRead)
			nOpenFlags = O_RDONLY;
		else if(nAccessFlags == kAccessFlagWrite)
			nOpenFlags = O_WRONLY;
		else if(nAccessFlags == kAccessFlagReadWrite)
			nOpenFlags = O_RDWR;

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
				nOpenFlags |= O_CREAT;
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

		// We explicitly disable the ability to open a directory. Such functionality is supported
		// by some Unixes and not others, and in any case the behavior is unstandardized.
		const size_t length = strlen(mpPath8); // We know the length is at least 1 from our check above.

		if(length > 0)
		{
			const bool pathIsDirectory = ((mpPath8[length - 1] == '/') || Directory::Exists(mpPath8));

			if(!pathIsDirectory)
			{
				// Sharing
				// Unix doesn't have built-in support for file sharing with the file open/close API.
				// Some sharing functionality has been bolted on via flock() and fnctl(). We don't 
				// currently use it but perhaps we could. http://en.wikipedia.org/wiki/File_locking#In_UNIX

				mnFileHandle = open(mpPath8, nOpenFlags, (S_IRUSR | S_IWUSR) | (S_IRGRP | S_IWGRP) | (S_IROTH | S_IWOTH));

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
					struct stat tempStat;
					int result = fstat(mnFileHandle, &tempStat);
					if (result < 0)
					{
						Close();
						return false;
					}

					mnSize        = tempStat.st_size;
					mnPos         = 0;
					mnAccessFlags = nAccessFlags;
					mnCD          = nCreationDisposition;
					mnSharing     = nSharing;
					mnUsageHints  = nUsageHints;
					mnLastError   = 0;
				}
			}
		}
	}

	return (mnFileHandle != kFileHandleInvalid);
}


bool FileStream::Close()
{
	if((mnFileHandle != kFileHandleInvalid))
	{
		close(mnFileHandle); // This returns -1 upon error. But there's not much to do about it.

		mnFileHandle  = kFileHandleInvalid;
		mnSize        = 0;
		mnPos         = 0;
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
		return mnSize;
	}

	return kSizeTypeError;
}


bool FileStream::SetSize(size_type size)
{
	if(mnFileHandle != kFileHandleInvalid)
	{
		const int result = ftruncate(mnFileHandle, (off_t)size);
		if (result == 0) // If the result is OK...
		{
			// If shrinking the file before the current position
			if(size < mnPos)
				mnPos = size;

			mnSize = (size_type)size;

			return true;
		}
		mnLastError = errno;
	}

	return false;
}


off_type FileStream::GetPosition(PositionType positionType) const
{
	off_type result;

	if (positionType == kPositionTypeBegin)
		result = mnPos;
	else if(positionType == kPositionTypeEnd)
		result = mnPos - mnSize;
	else
		result = 0;
	return result;
}


bool FileStream::SetPosition(off_type position, PositionType positionType)
{
	if(mnFileHandle != kFileHandleInvalid)
	{
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

		if (nResult != -1)
		{
			mnPos = nResult;
			return true;
		}

		mnLastError = errno;
	}

	return false;
}


size_type FileStream::GetAvailable() const
{
	return mnSize - mnPos;
}


size_type FileStream::Read(void* pData, size_type nSize)
{
	if(mnFileHandle != kFileHandleInvalid)
	{
		if (mnPos + nSize >= mnSize)
			nSize = GetAvailable();

		const size_type nCount = read(mnFileHandle, pData, (size_t)nSize);
		if (nCount != kSizeTypeError)
		{
			mnPos += nCount;
			return nCount;
		}
	}
	return kSizeTypeError;
}


bool FileStream::Write(const void* pData, size_type nSize)
{
	const char* pData8 = (const char*)pData;

	if(mnFileHandle != kFileHandleInvalid)
	{
		int64_t  nCount = 0;
		uint64_t amountWritten = 0;

		do
		{
			nCount = write(mnFileHandle, pData8 + (size_t)amountWritten, (size_t)(nSize - amountWritten));

			if(nCount >= 0)
				amountWritten += (uint64_t)nCount;

		}while((nCount >= 0) && (amountWritten < nSize));
		
		mnLastError = ((nCount >= 0) && (amountWritten == nSize)) ? kStateSuccess : (int)nCount;
	}

	if (mnLastError == kStateSuccess)
	{
		mnPos += nSize;
		if (mnPos > mnSize)
			mnSize = mnPos;

		return true;
	}

	return false;
}


bool FileStream::Flush()
{
	if(mnFileHandle != kFileHandleInvalid)
	{
		#if (defined(EA_PLATFORM_LINUX) || defined(__APPLE__)) && !defined(__MINGW32__)
			// Linux: On kernels before 2.4, fsync on big files can be inefficient. 
			//          An alternative might be to use the O_SYNC flag to open(2).
			fsync(mnFileHandle);
		#else
			// Not consistently present on other Unix variants.
		#endif
	}
	return true;
}


} // namespace IO


} // namespace EA















