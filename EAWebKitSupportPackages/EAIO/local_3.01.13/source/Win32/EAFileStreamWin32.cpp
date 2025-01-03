/////////////////////////////////////////////////////////////////////////////
// EAFileStreamWin32.cpp
//
// Copyright (c) 2003, Electronic Arts Inc. All rights reserved.
// Created by Paul Pedriana
//
// Provides a file stream for Microsoft-derived platforms. These include
// Win32, Win64, XBox
//
/////////////////////////////////////////////////////////////////////////////


#include <EAIO/internal/Config.h>
#include <EAIO/EAAsyncStream.h>
#include <EAIO/EAFileStream.h>
#include <coreallocator/icoreallocator.h>
#include <EAStdC/EAString.h>
#include <EAStdC/EAMemory.h>
#include EA_ASSERT_HEADER
#if   defined(EA_PLATFORM_MICROSOFT)
	#ifndef WIN32_LEAN_AND_MEAN
		#define WIN32_LEAN_AND_MEAN
	#endif
	#pragma warning(push, 0)
	#include <Windows.h>
	#pragma warning(pop)

	#if EAIO_WINDOWS8_EMULATION_ENABLED && (_WIN32_WINNT >= 0x0602)
		// CreateFileW is present in the kernel but not exposed via the header files.
		extern "C" WINBASEAPI HANDLE WINAPI CreateFileW(LPCWSTR, DWORD, DWORD, LPSECURITY_ATTRIBUTES, DWORD, DWORD, HANDLE);

		HANDLE CreateFile2EAIO(LPCWSTR lpFileName, DWORD dwDesiredAccess, DWORD dwShareMode, DWORD dwCreationDisposition, LPCREATEFILE2_EXTENDED_PARAMETERS pCreateExParams)
			{ return CreateFileW(lpFileName, dwDesiredAccess, dwShareMode, NULL, dwCreationDisposition, pCreateExParams ? pCreateExParams->dwFileFlags | pCreateExParams->dwFileAttributes : 0, NULL); }
	#endif
#endif




namespace EA
{

namespace IO
{

namespace FileStreamLocal
{
	const size_type kSizeTypeUnset     = (size_type)-1;
	const HANDLE    kFileHandleInvalid = INVALID_HANDLE_VALUE;

#ifdef EA_ISTREAM_HAS_ASYNC
	/////////////////////////////////////////////////////////////////////////////////////////////////
	/// AsyncWin32StreamIoRequest
	class AsyncWin32StreamIoRequest : public AsyncStreamIoRequest
	{
	public:
		AsyncWin32StreamIoRequest(
			AsyncExecutor& asyncExecutor,
			IStream& stream,
			void* handle,
			const AsyncIoFunctionCallback& func)
			: AsyncStreamIoRequest(stream, func)
			, mHandle(HANDLE(handle))
			, mAsyncExecutor(asyncExecutor) {}

		virtual ~AsyncWin32StreamIoRequest()
		{
			FreeAsyncOp(mAsyncExecutor, mOp);
		}

		virtual bool IsDone(AsyncResult& result, AsyncErrorCode& errorCode, size_t& bytesTransferred) override
		{
			return IsAsyncOpDone(mAsyncExecutor, *mOp, result, errorCode, bytesTransferred, mHandle);
		}

	protected:
		HANDLE mHandle;
		AsyncExecutor& mAsyncExecutor;
		AsyncExecutorAsyncOp* mOp{ nullptr };
	};

	/////////////////////////////////////////////////////////////////////////////////////////////////
	/// AsyncReadWin32StreamIoRequest
	/// Read a block of data from a IStream
	class AsyncReadWin32StreamIoRequest : public AsyncWin32StreamIoRequest
	{
	public:
		AsyncReadWin32StreamIoRequest(
			AsyncExecutor& asyncExecutor,
			IStream& stream,
			void* handle,
			off_type position,
			void* destination,
			size_type size,
			const AsyncIoFunctionCallback& func)
			: AsyncWin32StreamIoRequest(asyncExecutor, stream, handle, func)
			, mPosition(position)
			, mDestination(destination)
			, mSize(size) {}

		virtual bool Execute(AsyncResult& result, AsyncErrorCode& errorCode, size_t& bytesTransferred) override
		{
			return AsyncRead(mAsyncExecutor, mOp, result, errorCode, bytesTransferred, this, mHandle, mPosition, mDestination, mSize);
		}

	private:
		void* mDestination;
		off_type mPosition;
		size_type mSize;
	};

	/////////////////////////////////////////////////////////////////////////////////////////////////
	/// AsyncWriteWin32StreamIoRequest
	/// Read a block of data from a IStream
	class AsyncWriteWin32StreamIoRequest : public AsyncWin32StreamIoRequest
	{
	public:
		AsyncWriteWin32StreamIoRequest(
			AsyncExecutor& asyncExecutor,
			IStream& stream, void* handle,
			off_type position,
			const void* source,
			size_type size,
			const AsyncIoFunctionCallback& func)
			: AsyncWin32StreamIoRequest(asyncExecutor, stream, handle, func)
			, mPosition(position)
			, mSource(source)
			, mSize(size) {}

		virtual bool Execute(AsyncResult& result, AsyncErrorCode& errorCode, size_t& bytesTransferred) override
		{
			return AsyncWrite(mAsyncExecutor, mOp, result, errorCode, bytesTransferred, this, mHandle, mPosition, mSource, mSize);
		}

	private:
		const void* mSource;
		off_type mPosition;
		size_type mSize;
	};
#endif
}

FileStream::FileStream(const char8_t* pPath8)
  : mhFile(FileStreamLocal::kFileHandleInvalid),
	mnAccessFlags(0),
	mnCD(0),
	mnSharing(0),
	mnUsageHints(kUsageHintNone),
	mnLastError(kStateNotOpen),
	mnSize(FileStreamLocal::kSizeTypeUnset)
{
	mpPathW[0] = 0;
	FileStream::SetPath(pPath8); // Note that in a constructor, the virtual function mechanism is inoperable.
}


#if EA_CHAR16_NATIVE
FileStream::FileStream(const char16_t* pPath16)
  : mhFile(FileStreamLocal::kFileHandleInvalid),
	mnAccessFlags(0),
	mnCD(0),
	mnSharing(0),
	mnUsageHints(kUsageHintNone),
	mnLastError(kStateNotOpen),
	mnSize(FileStreamLocal::kSizeTypeUnset)
{
	mpPathW[0] = 0;
	FileStream::SetPath(pPath16); // Note that in a constructor, the virtual function mechanism is inoperable.
}
#endif

FileStream::FileStream(const wchar_t* pPathW)
  : mhFile(FileStreamLocal::kFileHandleInvalid),
	mnAccessFlags(0),
	mnCD(0),
	mnSharing(0),
	mnUsageHints(kUsageHintNone),
	mnLastError(kStateNotOpen),
	mnSize(FileStreamLocal::kSizeTypeUnset)
{
	mpPathW[0] = 0;
	FileStream::SetPath(pPathW); // Note that in a constructor, the virtual function mechanism is inoperable.
}


FileStream::FileStream(const char32_t* pPath32)
  : mhFile(FileStreamLocal::kFileHandleInvalid),
	mnAccessFlags(0),
	mnCD(0),
	mnSharing(0),
	mnUsageHints(kUsageHintNone),
	mnLastError(kStateNotOpen),
	mnSize(FileStreamLocal::kSizeTypeUnset)
{
	mpPathW[0] = 0;
	FileStream::SetPath(pPath32); // Note that in a constructor, the virtual function mechanism is inoperable.
}


FileStream::FileStream(const FileStream& fs)
  : mhFile(FileStreamLocal::kFileHandleInvalid),
	mnAccessFlags(0),
	mnCD(0),
	mnSharing(0),
	mnUsageHints(fs.mnUsageHints),
	mnLastError(kStateNotOpen),
	mnSize(FileStreamLocal::kSizeTypeUnset)
{
	mpPathW[0] = 0;
	FileStream::SetPath(fs.mpPathW); // Note that in a constructor, the virtual function mechanism is inoperable.
}


FileStream::~FileStream()
{
	FileStream::Close(); // Note that in a destructor, the virtual function mechanism is inoperable.
}


FileStream& FileStream::operator=(const FileStream& fs)
{
	using namespace FileStreamLocal;

	if(&fs != this)
	{
		Close();
		SetPath(fs.mpPathW);

		mhFile        = kFileHandleInvalid;
		mnAccessFlags = 0;
		mnCD          = 0;
		mnSharing     = 0;
		mnUsageHints  = fs.mnUsageHints;
		mnLastError   = kStateNotOpen;
		mnSize        = fs.mnSize;
	}

	return *this;
}


void FileStream::SetPath(const char8_t* pPath8)
{
	using namespace FileStreamLocal;

	if((mhFile == kFileHandleInvalid) && pPath8)
		EA::StdC::Strlcpy(mpPathW, pPath8, kMaxPathLength); // Convert UTF8 to UTF16.
}


void FileStream::SetPath(const char16_t* pPath16)
{
	using namespace FileStreamLocal;

	if((mhFile == kFileHandleInvalid) && pPath16)
		EA::StdC::Strlcpy(mpPathW, pPath16, kMaxPathLength);
}


#if EA_CHAR16_NATIVE
void FileStream::SetPath(const wchar_t* pPathW)
{
	using namespace FileStreamLocal;

	if ((mhFile == kFileHandleInvalid) && pPathW)
		EA::StdC::Strlcpy(mpPathW, pPathW, kMaxPathLength);
}
#endif

void FileStream::SetPath(const char32_t* pPath32)
{
	using namespace FileStreamLocal;

	if((mhFile == kFileHandleInvalid) && pPath32)
		EA::StdC::Strlcpy(mpPathW, pPath32, kMaxPathLength);
}


size_t FileStream::GetPath(char8_t* pPath8, size_t nPathLength)
{
	return (size_t)EA::StdC::Strlcpy(pPath8, mpPathW, nPathLength);
}


#if EA_CHAR16_NATIVE
size_t FileStream::GetPath(char16_t* pPath16, size_t nPathLength)
{
	return (size_t)EA::StdC::Strlcpy(pPath16, mpPathW, nPathLength);
}
#endif


size_t FileStream::GetPath(wchar_t* pPathW, size_t nPathLength)
{
	return (size_t)EA::StdC::Strlcpy(pPathW, mpPathW, nPathLength);
}


size_t FileStream::GetPath(char32_t* pPath32, size_t nPathLength)
{
	return (size_t)EA::StdC::Strlcpy(pPath32, mpPathW, nPathLength);
}


bool FileStream::Open(int nAccessFlags, int nCreationDisposition, int nSharing, int nUsageHints)
{
	using namespace FileStreamLocal;

	if((mhFile == kFileHandleInvalid) && nAccessFlags) // If not already open and if some kind of access is requested...
	{
		DWORD nAccess(0), nShare(0), nCreate(0), dwFlagsAndAttributes(0);

		if(nAccessFlags & kAccessFlagRead)
			nAccess |= GENERIC_READ;
		if(nAccessFlags & kAccessFlagWrite)
			nAccess |= GENERIC_WRITE;

		if(nSharing & kShareRead)
			nShare |= FILE_SHARE_READ;
		if(nSharing & kShareWrite)
			nShare |= (FILE_SHARE_READ | FILE_SHARE_WRITE);

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
			default:
			case kCDOpenExisting:
				nCreate = OPEN_EXISTING;
				break;
			case kCDCreateNew:
				nCreate = CREATE_NEW;
				break;
			case kCDCreateAlways:
				nCreate = CREATE_ALWAYS;
				break;
			case kCDOpenAlways:
				nCreate = OPEN_ALWAYS;
				break;
			case kCDTruncateExisting:
				nCreate = TRUNCATE_EXISTING;
				break;
		}

		if(nUsageHints & kUsageHintSequential)
			dwFlagsAndAttributes |= FILE_FLAG_SEQUENTIAL_SCAN;
		else if(nUsageHints & kUsageHintRandom)
			dwFlagsAndAttributes |= FILE_FLAG_RANDOM_ACCESS;

		if (nUsageHints & kUsageHintAsync)
			dwFlagsAndAttributes |= FILE_FLAG_OVERLAPPED;
		if (nUsageHints & kUsageHintNoCache)
			dwFlagsAndAttributes |= FILE_FLAG_NO_BUFFERING;

		#if   EA_WINAPI_FAMILY_PARTITION(EA_WINAPI_PARTITION_DESKTOP)
			mhFile = CreateFileW(mpPathW, nAccess, nShare, NULL, nCreate, dwFlagsAndAttributes, NULL);
		#else
			CREATEFILE2_EXTENDED_PARAMETERS temp_CREATEFILE2_EXTENDED_PARAMETERS;
			temp_CREATEFILE2_EXTENDED_PARAMETERS.dwSize               = sizeof(temp_CREATEFILE2_EXTENDED_PARAMETERS);
			temp_CREATEFILE2_EXTENDED_PARAMETERS.dwFileAttributes     = FILE_ATTRIBUTE_NORMAL;
			temp_CREATEFILE2_EXTENDED_PARAMETERS.dwFileFlags          = dwFlagsAndAttributes;
			temp_CREATEFILE2_EXTENDED_PARAMETERS.dwSecurityQosFlags   = SECURITY_ANONYMOUS;
			temp_CREATEFILE2_EXTENDED_PARAMETERS.lpSecurityAttributes = NULL;
			temp_CREATEFILE2_EXTENDED_PARAMETERS.hTemplateFile        = NULL;

			#if EAIO_WINDOWS8_EMULATION_ENABLED
				mhFile = CreateFile2EAIO(mpPathW, nAccess, nShare, nCreate, &temp_CREATEFILE2_EXTENDED_PARAMETERS);
			#else
				// Microsoft has declared that we should enable at least read sharing whenever CreateFile2 is used for this platform.
				mhFile = CreateFile2(mpPathW, nAccess, nShare | FILE_SHARE_READ, nCreate, &temp_CREATEFILE2_EXTENDED_PARAMETERS);
			#endif
		#endif

		if(mhFile == INVALID_HANDLE_VALUE) // If it failed...
		{
			// mnAccessFlags = 0; // Not necessary, as these should already be zeroed.
			// mnCD          = 0;
			// mnSharing     = 0;
			// mnUsageHints  = 0;
			mnLastError      = GetLastError();
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

	return (mhFile != kFileHandleInvalid);
}


bool FileStream::Close()
{
	using namespace FileStreamLocal;

	if(mhFile != kFileHandleInvalid)
	{
#ifdef EA_ISTREAM_HAS_ASYNC
		if (mAsyncExecutor)
			UnbindFromWin32Executor(*mAsyncExecutor, mhFile);
		mAsyncExecutor = nullptr;
#endif

		CloseHandle(mhFile); // This returns zero upon error. But there's not much to do about it.

		mhFile        = kFileHandleInvalid;
		mnAccessFlags = 0;
		mnCD          = 0;
		mnSharing     = 0;
		mnUsageHints  = 0;
		mnLastError   = kStateNotOpen;
		mnSize        = kSizeTypeUnset;
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
	using namespace FileStreamLocal;

	if(mhFile != kFileHandleInvalid)
	{
		if(mnSize != kSizeTypeUnset) // If the size is cached...
			return mnSize;

		#if EA_WINAPI_FAMILY_PARTITION(EA_WINAPI_PARTITION_DESKTOP)
			LARGE_INTEGER li;
			const BOOL bResult = GetFileSizeEx(mhFile, &li);

			if(bResult)  
			{
				#if (EAIO_SIZETYPE_SIZE >= 8)
					size_type returnValue = li.QuadPart;
				#else
					size_type returnValue = li.LowPart;
				#endif

				if(((mnAccessFlags & kAccessFlagWrite) == 0) && ((mnSharing & (kShareWrite | kShareDelete)) == 0)) // If the size is constant...
					mnSize = returnValue;

				return returnValue;
			}
		#else

			// Get the current position
			LARGE_INTEGER currentPos; 
			currentPos.QuadPart = 0;
			const BOOL bResult1 = SetFilePointerEx(mhFile, currentPos, &currentPos, FILE_CURRENT);

			if (bResult1)
			{
				// Move to the end of the file
				LARGE_INTEGER endPos; 
				endPos.QuadPart = 0;
				const BOOL bResult2 = SetFilePointerEx(mhFile, endPos, &endPos, FILE_END);

				// Set back to the original position
				const BOOL bResult3 = SetFilePointerEx(mhFile, currentPos, NULL, FILE_BEGIN);
				// If this fails, it means we could not restore the file pointer to the original position
				// This could trigger an issue later in the calling code
				EA_ASSERT(bResult3);
				EA_UNUSED(bResult3);

				if (bResult2)
				{
					size_type returnValue = endPos.QuadPart;

					if(((mnAccessFlags & kAccessFlagWrite) == 0) && ((mnSharing & (kShareWrite | kShareDelete)) == 0)) // If the size is constant...
						mnSize = returnValue;

					return returnValue;
				}
			}

		#endif

		mnLastError = GetLastError();
	}

	return kSizeTypeError;
}


bool FileStream::SetSize(size_type size)
{
	using namespace FileStreamLocal;

	if(mhFile != kFileHandleInvalid)
	{
		const off_type savedPosition = GetPosition();

		// Under Win32, you set the length of a file by calling SetFilePointer
		// to set the pointer to the given position, then SetEndOfFile (or Write)
		// to cement the action.
		LARGE_INTEGER li; li.QuadPart = size;
		const BOOL bResult = SetFilePointerEx(mhFile, li, &li, FILE_BEGIN);

		if(bResult)
		{
			if(SetEndOfFile(mhFile))
			{
				SetPosition(savedPosition);
				return true;
			}
		}

		SetPosition(savedPosition);
		mnLastError = GetLastError();
	}

	return false;
}


off_type FileStream::GetPosition(PositionType positionType) const
{
	LARGE_INTEGER li; li.QuadPart = 0;
	const BOOL bResult = SetFilePointerEx(mhFile, li, &li, FILE_CURRENT);

	if(bResult)
	{
		if(positionType == kPositionTypeBegin)
		{
			// Nothing to do.
		}
		else if(positionType == kPositionTypeEnd)
		{
			const size_type nSize = GetSize();

			if(nSize != kSizeTypeError)
				li.QuadPart = (li.QuadPart - nSize);
			else
				li.QuadPart = kSizeTypeError;
		}
		else // else by definition the result is 0.
			li.QuadPart = 0;

		#if (EAIO_SIZETYPE_SIZE >= 8)
			return li.QuadPart;
		#else
			return (off_type)li.LowPart;
		#endif
	}

	return kSizeTypeError;
}


bool FileStream::SetPosition(off_type position, PositionType positionType)
{
	using namespace FileStreamLocal;

	if(mhFile != kFileHandleInvalid)
	{
		DWORD nMethod;

		switch(positionType)
		{
			default:
			case kPositionTypeBegin:
				nMethod = FILE_BEGIN;
				break;

			case kPositionTypeCurrent:
				nMethod = FILE_CURRENT;
				break;

			case kPositionTypeEnd:
				nMethod = FILE_END;
				break;
		}

		LARGE_INTEGER li; li.QuadPart = position;
		const BOOL bResult = SetFilePointerEx(mhFile, li, &li, nMethod);

		if(bResult)
			return true;

		mnLastError = GetLastError();
	}

	return false;
}


size_type FileStream::GetAvailable() const
{
	const off_type nPosition = GetPosition(kPositionTypeEnd);

	if(nPosition != kSizeTypeError)
		return (size_type)-nPosition;

	return kSizeTypeError;
}


size_type FileStream::Read(void* pData, size_type nSize)
{
	using namespace FileStreamLocal;

	if(mhFile != kFileHandleInvalid)
	{
		DWORD dwReadCount;

		// http://cbloomrants.blogspot.ca/2013/10/10-03-13-setlasterror0.html
		// SetLastError(0);

		EA_ASSERT(nSize < 0xffffffff); // We are currently limited to 4GB per individual read.
		const BOOL bResult = ReadFile(mhFile, pData, (DWORD)nSize, &dwReadCount, NULL);

		if(bResult)
			return (size_type)dwReadCount;

		mnLastError = GetLastError();
	}

	return kSizeTypeError;
}


bool FileStream::Write(const void* pData, size_type nSize)
{
	using namespace FileStreamLocal;

	if(mhFile != kFileHandleInvalid)
	{
		DWORD dwWriteCount;

		// http://cbloomrants.blogspot.ca/2013/10/10-03-13-setlasterror0.html
		// SetLastError(0);

		EA_ASSERT(nSize < 0xffffffff); // We are currently limited to 4GB per individual read.
		const BOOL bResult = WriteFile(mhFile, pData, (DWORD)nSize, &dwWriteCount, NULL);

		#if 0 // (EAPATCH_DEBUG >= 3)
			const uint8_t kPattern[6] = { 0xae, 0x00, 0x01, 0x00, 0x06, 0xf5 };

			if(EA::StdC::Memmem(pData, (size_t)nSize, kPattern, EAArrayCount(kPattern)))
				EA_DEBUG_BREAK();
		#endif

		if(bResult)
		{
			EA_ASSERT(dwWriteCount == nSize);
			return true;
		}

		mnLastError = GetLastError();
	}
	return false;
}


bool FileStream::Flush()
{
	using namespace FileStreamLocal;

	bool bResult(false);

	if(mhFile != kFileHandleInvalid)
	{
		bResult = (FlushFileBuffers(mhFile) != 0);
		if(!bResult)
			mnLastError = GetLastError();
	}
	return bResult;
}


#ifdef EA_ISTREAM_HAS_ASYNC
AsyncExecutor* FileStream::GetAsyncExecutor()
{
	using namespace FileStreamLocal;

	if (mhFile != kFileHandleInvalid)
	{
		if (mAsyncExecutor == nullptr)
		{
			if ((mnUsageHints & FileStream::kUsageHintAsync) != 0)
				mAsyncExecutor = AsyncExecutor::StartExecutor(AsyncExecutorType::Concurrent, EA::IO::kWaitObjects);
			else
				mAsyncExecutor = AsyncExecutor::StartExecutor(AsyncExecutorType::Sequential, EA::IO::kDefault);
			BindToWin32Executor(*mAsyncExecutor, mhFile);
		}
		return mAsyncExecutor;
	}
	else
		return nullptr;
}


uint32_t FileStream::GetAsyncFileAlignment() const
{
	int hints = mnUsageHints;
	if ((hints & (kUsageHintAsync | kUsageHintNoCache)) == (kUsageHintAsync | kUsageHintNoCache))
		return 4 * 1024; // General guess for 4k page size

	return 1;
}

uint32_t FileStream::GetAsyncMemoryAlignment() const
{
	return GetAsyncFileAlignment();
}

AsyncRequestId FileStream::AsyncRead(AsyncExecutor& exec, void* destination, size_type size, const AsyncIoFunctionCallback& func)
{
	if ((mnUsageHints & kUsageHintAsync) == 0)
		return IStream::AsyncRead(exec, destination, size, func);
	else
	{
		EA_ASSERT_MSG(false, "Position is required for a Win32 file opened in async mode");
		if (func)
			func(AsyncResult::Error, AsyncErrorCode(), 0);
		return kInvalidAsyncRequestId;
	}
}


AsyncRequestId FileStream::AsyncRead(AsyncExecutor& exec, size_type position, void* destination, size_type size, const AsyncIoFunctionCallback& func)
{
	using namespace FileStreamLocal;

	if (&exec != mAsyncExecutor)
	{
		if (mAsyncExecutor == nullptr)
		{
			if (mhFile != kFileHandleInvalid)
			{
				mAsyncExecutor = &exec;
				BindToWin32Executor(*mAsyncExecutor, mhFile);
			}
		}
		else
		{
			if (exec.GetType() == AsyncExecutorType::Concurrent &&
				mAsyncExecutor->GetType() == AsyncExecutorType::Concurrent)
			{
				EA_ASSERT_MSG(false, "Can not send an asyncronous Win32 handle to two different concurrent executors");
				if (func)
					func(AsyncResult::Error, AsyncErrorCode(), 0);
				return kInvalidAsyncRequestId;
			}
		}
	}
	if ((mnUsageHints & kUsageHintAsync) == 0)
		return IStream::AsyncRead(exec, position, destination, size, func);
	else
		return exec.AllocAndPush<FileStreamLocal::AsyncReadWin32StreamIoRequest>(eastl::ref(exec), eastl::ref(*this), mhFile, position, destination, size, eastl::cref(func));
}


AsyncRequestId FileStream::AsyncWrite(AsyncExecutor& exec, const void* source, size_type size, const AsyncIoFunctionCallback& func)
{
	if ((mnUsageHints & kUsageHintAsync) == 0)
		return IStream::AsyncWrite(exec, source, size, func);
	else
	{
		EA_ASSERT_MSG(false, "Position is required for a Win32 file opened in async mode");
		if (func)
			func(AsyncResult::Error, AsyncErrorCode(), 0);
		return kInvalidAsyncRequestId;
	}
}


AsyncRequestId FileStream::AsyncWrite(AsyncExecutor& exec, size_type position, const void* source, size_type size, const AsyncIoFunctionCallback& func)
{
	using namespace FileStreamLocal;

	if (&exec != mAsyncExecutor)
	{
		if (mAsyncExecutor == nullptr)
		{
			if (mhFile != kFileHandleInvalid)
			{
				mAsyncExecutor = &exec;
				BindToWin32Executor(*mAsyncExecutor, mhFile);
			}
		}
		else
		{
			if (exec.GetType() == AsyncExecutorType::Concurrent &&
				mAsyncExecutor->GetType() == AsyncExecutorType::Concurrent)
			{
				EA_ASSERT_MSG(false, "Can not send an asyncronous Win32 handle to two different concurrent executors");
				if (func)
					func(AsyncResult::Error, AsyncErrorCode(), 0);
				return kInvalidAsyncRequestId;
			}
		}
	}
	if ((mnUsageHints & kUsageHintAsync) == 0)
		return IStream::AsyncWrite(exec, position, source, size, func);
	else
		return exec.AllocAndPush<FileStreamLocal::AsyncWriteWin32StreamIoRequest>(eastl::ref(exec), eastl::ref(*this), mhFile, position, source, size, eastl::cref(func));
}
#endif

} // namespace IO


} // namespace EA















