/////////////////////////////////////////////////////////////////////////////
// EAFileStreamAndroid.cpp
//
// Copyright (c) 2003, Electronic Arts Inc. All rights reserved.
//
// Provides a file stream for Android that handles the native file system
// as well as files stored in the application package file.
//
/////////////////////////////////////////////////////////////////////////////


#include <EABase/eabase.h>

#if defined(EA_PLATFORM_ANDROID)

#include <EAIO/internal/Config.h>
#include <EAIO/EAFileStream.h>
#include <EAIO/Allocator.h>
#include <EAIO/FnEncode.h>
#include <coreallocator/icoreallocator.h>
#include <EAStdC/EAString.h>
#include <eathread/eathread.h>
#include EA_ASSERT_HEADER
#include <stdio.h>
#include <unistd.h>
#include <errno.h>
#include <fcntl.h>
#include <sys/types.h>
#include <jni.h>
#include "com_ea_EAIO_EAIO.h"


namespace EA
{

namespace IO
{


const int kFileHandleInvalid = -1;


// Include this within the EA::IO namespace.
#ifndef ASSETMANAGER_INIT_FAILURE_MESSAGE
	#define ASSETMANAGER_INIT_FAILURE_MESSAGE "Note: EAIO.Startup(activityInstance) must be called from Java prior to accessing appbundle files."
#endif
#include "assetmanagerjni.inc"


FileStream::FileStream(const char* pPath8)
  : mJavaInputStream(NULL),
	mnFileHandle(kFileHandleInvalid),
	mPath8(),
	mnAccessFlags(0),
	mnCD(0),
	mnSharing(0),
	mnLastError(kStateNotOpen)
{
	FileStream::SetPath(pPath8); // Note that in a constructor, the virtual function mechanism is inoperable, so we qualify the function call.
}


FileStream::FileStream(const char16_t* pPath16)
  : mnFileHandle(kFileHandleInvalid),
	mPath8(),
	mnAccessFlags(0),
	mnCD(0),
	mnSharing(0),
	mnLastError(kStateNotOpen)
{
	FileStream::SetPath(pPath16);
}


FileStream::FileStream(const char32_t* pPath32)
  : mnFileHandle(kFileHandleInvalid),
	mPath8(),
	mnAccessFlags(0),
	mnCD(0),
	mnSharing(0),
	mnLastError(kStateNotOpen)
{
	FileStream::SetPath(pPath32);
}

#if EA_WCHAR_UNIQUE
FileStream::FileStream(const wchar_t* pPathW)
  : mnFileHandle(kFileHandleInvalid),
	mPath8(),
	mnAccessFlags(0),
	mnCD(0),
	mnSharing(0),
	mnLastError(kStateNotOpen)
{
	FileStream::SetPath(pPathW);
}
#endif

FileStream::FileStream(const FileStream& fs)
  : mnFileHandle(kFileHandleInvalid),
	mPath8(),
	mnAccessFlags(0),
	mnCD(0),
	mnSharing(0),
	mnUsageHints(fs.mnUsageHints),
	mnLastError(kStateNotOpen)
{
	FileStream::SetPath(fs.mPath8.c_str());
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
		SetPath(fs.mPath8.c_str());

		mJavaInputStream = NULL;
		mJavaFileSize = 0;
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
		mPath8 = pPath8;
		Path::Simplify(mPath8);
	}
}


void FileStream::SetPath(const char16_t* pPath16)
{
	if((mnFileHandle == kFileHandleInvalid) && pPath16)
	{
		ConvertPath(mPath8, pPath16);
		Path::Simplify(mPath8);
	}
}


void FileStream::SetPath(const char32_t* pPath32)
{
	if((mnFileHandle == kFileHandleInvalid) && pPath32)
	{
		ConvertPath(mPath8, pPath32);
		Path::Simplify(mPath8);
	}
}

#if EA_WCHAR_UNIQUE
void FileStream::SetPath(const wchar_t* pPathW)
{
	if((mnFileHandle == kFileHandleInvalid) && pPathW)
	{
		ConvertPath(mPath8, pPathW);
		Path::Simplify(mPath8);
	}
}
#endif


size_t FileStream::GetPath(char* pPath8, size_t nPathCapacity)
{
	// Return the required strlen of the destination path.
	return EA::StdC::Strlcpy(pPath8, mPath8.c_str(), nPathCapacity);
}


size_t FileStream::GetPath(char16_t* pPath16, size_t nPathCapacity)
{
	// Return the required strlen of the destination path.
	return EA::StdC::Strlcpy(pPath16, mPath8.c_str(), nPathCapacity, mPath8.length());
}


size_t FileStream::GetPath(char32_t* pPath32, size_t nPathCapacity)
{
	// Return the required strlen of the destination path.
	return EA::StdC::Strlcpy(pPath32, mPath8.c_str(), nPathCapacity, mPath8.length());
}


#if EA_WCHAR_UNIQUE
size_t FileStream::GetPath(wchar_t* pPathW, size_t nPathCapacity)
{
	// Return the required strlen of the destination path.
	return EA::StdC::Strlcpy(pPathW, mPath8.c_str(), nPathCapacity, mPath8.length());
}
#endif


bool FileStream::Open(int nAccessFlags, int nCreationDisposition, int nSharing, int nUsageHints)
{
	if(mPath8.find(APPBUNDLE_PATH_PREFIX, 0) == 0) // If the path begins with "appbundle:/" ...
	{
		mStreamType = IST_AppBundle;

		if(nAccessFlags == kAccessFlagRead)
		{
			AutoJNIEnv autoEnv;
			uint64_t fileSize;
			mJavaInputStream = AssetManagerJNI::Open(autoEnv, mPath8.c_str() + APPBUNDLE_PATH_PREFIX_LENGTH, &fileSize);
			mJavaFileSize = static_cast<size_type>(fileSize);
			mJavaFilePosition = 0;

			if(mJavaInputStream != NULL)
			{
				mnAccessFlags = nAccessFlags;
				mnCD          = nCreationDisposition;
				mnSharing     = nSharing;
				mnUsageHints  = nUsageHints;
				mnLastError   = 0;
				return true;
			}
		}
		mnLastError = kStateError;
		return false;
	}
	else
	{
		mStreamType = IST_Native;

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

			mnFileHandle = open(mPath8.c_str(), nOpenFlags, 0666);

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
		}

		return (mnFileHandle != kFileHandleInvalid);
	}
}


bool FileStream::Close()
{
	if( mStreamType == IST_AppBundle)
	{
		if(mJavaInputStream != NULL)
		{
			AutoJNIEnv autoEnv;
			AssetManagerJNI::Close(autoEnv, mJavaInputStream);
			mJavaInputStream = NULL;
		}
	}
	else
	{
		if((mnFileHandle != kFileHandleInvalid))
		{
			close(mnFileHandle); // This returns -1 upon error. But there's not much to do about it.

			mnFileHandle  = kFileHandleInvalid;
			mnAccessFlags = 0;
			mnCD          = 0;
			mnSharing     = 0;
			mnUsageHints  = 0;
			mnLastError   = kStateNotOpen;
		}
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
	if( mStreamType == IST_AppBundle)
	{
		if(mJavaInputStream != NULL)
		{
			// Note the file size returned may be -1 if we were unable to determine the file size.
			if(mJavaFileSize == kSizeTypeError)
			{
				// Instead of trying to figure out the file size immediately when the file is opened, we
				// wait until the file size is actually requested.  This technique avoids penalizing users
				// that don't require the file length.
				AutoJNIEnv autoEnv;
				mJavaFileSize = AssetManagerJNI::DetermineFileLengthBySkipping(autoEnv, mPath8.c_str() + APPBUNDLE_PATH_PREFIX_LENGTH);
			}
			return mJavaFileSize;
		}
	}
	else
	{
		if(mnFileHandle != kFileHandleInvalid)
		{
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
		}
	}

	return kSizeTypeError;
}


bool FileStream::SetSize(size_type size)
{
	// if(mStreamType == IST_AppBundle)
	// AppBundle stream types are read-only.

	if(mnFileHandle != kFileHandleInvalid)
	{
		// Technically speaking, the Posix Standard states that the file size 
		// isn't guaranteed to increase for all implementations under all circumstances.

		const int result = ftruncate(mnFileHandle, (off_t)size);
		if(result == 0) // If the result is OK...
			return true;
		mnLastError = errno;
	}

	return false;
}


off_type FileStream::GetPosition(PositionType positionType) const
{
	off_type position = (off_type)kSizeTypeError;

	if(mStreamType == IST_AppBundle)
	{
		if(mJavaInputStream) // If we have an open file...
		{
			if(positionType == kPositionTypeBegin)
				return mJavaFilePosition;
			else if(positionType == kPositionTypeEnd)
			{
				const size_type nSize = GetSize();

				if(nSize != kSizeTypeError) // This should never fail unless somehow our mJavaInputStream was made invalid behind our back.
					position = (mJavaFilePosition - (off_type)nSize);
			}
			else // kPostionTypeCurrent
				position = 0;
		}
		// Else fall through and return error.
	}
	else
	{
		if(mnFileHandle != kFileHandleInvalid) // If we have an open file...
		{
			EA_COMPILETIME_ASSERT(kSizeTypeError == (size_type)-1); // lseek returns -1 upon error and we take advantage of that fact that it matches kSizeTypeError.

			if(positionType == kPositionTypeBegin)
				position = (off_type)lseek(mnFileHandle, 0, SEEK_CUR);
			else if(positionType == kPositionTypeEnd)
			{
				position = (off_type)lseek(mnFileHandle, 0, SEEK_CUR);

				if(position != -1) // This should never fail unless somehow our mnFileHandle was made invalid behind our back.
				{
					const size_type nSize = GetSize(); // This should never fail unless somehow our mnFileHandle was made invalid behind our back.

					if(nSize != kSizeTypeError)
						position = (position - (off_type)nSize);
				}
			}
			else // kPostionTypeCurrent
				position = 0;
		}
		// Else fall through and return error.
	}

	return position;
}


bool FileStream::SetPosition(off_type position, PositionType positionType)
{
	if(mStreamType == IST_AppBundle)
	{
		off_t newPosition;

		switch(positionType)
		{
			default:
			case kPositionTypeBegin:
				newPosition = position;
				break;

			case kPositionTypeCurrent:
				newPosition = (mJavaFilePosition + position);
				break;

			case kPositionTypeEnd:
				if(mJavaFileSize == kSizeTypeError)
					return false;

				newPosition = (mJavaFileSize + position);
				break;
		}

		if(newPosition == mJavaFilePosition) // If there is no change...
			return true;

		AutoJNIEnv autoEnv;

		if(newPosition > mJavaFilePosition)
			mJavaFilePosition += AssetManagerJNI::Skip(autoEnv, mJavaInputStream, newPosition - mJavaFilePosition);
		else
		{
			// Re-open file to move to beginning of the stream.
			AssetManagerJNI::Close(autoEnv, mJavaInputStream);
			uint64_t fileSize;
			mJavaInputStream = AssetManagerJNI::Open(autoEnv, mPath8.c_str() + APPBUNDLE_PATH_PREFIX_LENGTH, &fileSize);

			// Skip to required position in file.
			mJavaFilePosition = AssetManagerJNI::Skip(autoEnv, mJavaInputStream, newPosition);
		}

		// To consider: What should we set mLastError to?
		// mnLastError = errno;

		return (mJavaFilePosition == newPosition);
	}
	else
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

			if(nResult != -1)
				return true;

			mnLastError = errno;
		}
	}

	return false;
}


size_type FileStream::GetAvailable() const
{
	const off_type nPosition = GetPosition(kPositionTypeEnd);

	if(nPosition != (off_type)kSizeTypeError)
		return (size_type)-nPosition;

	return kSizeTypeError;
}


size_type FileStream::Read(void* pData, size_type nSize)
{
	if( mStreamType == IST_AppBundle)
	{
		char *ptr = reinterpret_cast<char*>(pData);
		size_t totalAmountRead = 0;
		int amountRead;
		AutoJNIEnv autoEnv;
		do
		{
			amountRead = AssetManagerJNI::Read(autoEnv, mJavaInputStream, ptr, nSize-totalAmountRead);
			if(amountRead == -1)
			{
				// End of file has been reached
				break;
			}
			ptr += amountRead;
			totalAmountRead += static_cast<size_t>(amountRead);
		} while (amountRead != 0 && totalAmountRead < nSize);
		mJavaFilePosition += totalAmountRead;
		return totalAmountRead;
	}
	else
	{
		if(mnFileHandle != kFileHandleInvalid)
		{
			const size_type nCount = read(mnFileHandle, pData, (size_t)nSize);
			if(nCount != kSizeTypeError)
				return nCount;
		}
		return kSizeTypeError;
	}
}


bool FileStream::Write(const void* pData, size_type nSize)
{
	if(mnFileHandle != kFileHandleInvalid)
	{
		const size_type nCount = write(mnFileHandle, pData, (size_t)nSize);
		if(nCount != kSizeTypeError)
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

// JNI code

extern "C" __attribute__ ((visibility("default"))) void JNICALL Java_com_ea_EAIO_EAIO_StartupNativeImpl
  (JNIEnv *env, jclass, jobject assetManager, jstring dataDir, jstring filesDir, jstring externalDir)
{
	EA::IO::AssetManagerJNI::Startup(env, assetManager, dataDir, filesDir, externalDir);
}

extern "C" __attribute__ ((visibility("default"))) void JNICALL Java_com_ea_EAIO_EAIO_Shutdown
  (JNIEnv *env, jclass)
{
	EA::IO::AssetManagerJNI::Shutdown(env);
}

#endif // #if defined(EA_PLATFORM_ANDROID)


