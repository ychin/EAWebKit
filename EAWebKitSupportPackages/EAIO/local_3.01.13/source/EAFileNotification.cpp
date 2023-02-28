///////////////////////////////////////////////////////////////////////////////
// EAFileNotification.cpp
//
// Copyright (c) 2007 Maxis, Inc. -- All Rights Reserved World Wide.
// Created and maintained by Paul Pedriana, Maxis
//
// Implements automatic notifications of changes in the file system.
///////////////////////////////////////////////////////////////////////////////


#include <EAIO/internal/Config.h>
#include <EAIO/EAFileNotification.h>
#include <EAIO/PathString.h>
#include <EAIO/FnEncode.h>
#include <EAStdC/EAString.h>
#include EA_ASSERT_HEADER


#if defined(EA_TRACE_ENABLED) && EA_TRACE_ENABLED
	// #include <EATrace/EATrace.h>
	// This file uses EATrace, but we don't have build file support for it yet since the build file
	// needs to be updated to read a global configuration option which brings in a dependency on 
	// the EATrace package. For the time being we disable EATrace usage in this file.
	#undef  EA_TRACE_ENABLED
	#define EA_TRACE_ENABLED 0
#endif


///////////////////////////////////////////////////////////////////////////////
// Windows implementation
//
#if !FCN_POLL_ENABLED && EA_WINAPI_FAMILY_PARTITION(EA_WINAPI_PARTITION_DESKTOP)

#include <EAIO/EAFileUtil.h>

#pragma warning(push, 0)
#include <process.h>
#include <Windows.h>
#pragma warning(pop)



#ifndef WinAPIFunctionDecl
	#define WinAPIFunctionDecl(dllName, functionName, functionType); \
	\
	namespace { \
		class WinAPI_##functionName \
		{ \
		public: \
			WinAPI_##functionName() \
			{ \
				mhModule = LoadLibraryA(dllName); \
				if(mhModule) \
					mpFunction = (functionType)(void*)GetProcAddress(mhModule, #functionName); \
				else \
					mpFunction = NULL; \
			} \
			\
		~WinAPI_##functionName() \
				{ FreeLibrary(mhModule); } \
			\
			functionType operator()()const \
				{ return mpFunction; } \
			\
			typedef functionType (WinAPI_##functionName::*bool_)() const; \
			operator bool_() const \
			{ \
				if(mpFunction) \
					return &WinAPI_##functionName::operator(); \
				return 0; \
			} \
			\
		protected: \
			HMODULE      mhModule; \
			functionType mpFunction; \
		}; \
		\
		WinAPI_##functionName p##functionName;\
		\
	}
#endif


typedef WINBASEAPI BOOL (WINAPI *ReadDirectoryChangesWType)(HANDLE hDirectory, LPVOID lpBuffer, DWORD nBufferLength, BOOL bWatchSubtree, DWORD dwNotifyFilter, LPDWORD lpBytesReturned, LPOVERLAPPED lpOverlapped, LPOVERLAPPED_COMPLETION_ROUTINE lpCompletionRoutine);
WinAPIFunctionDecl("kernel32.dll", ReadDirectoryChangesW, ReadDirectoryChangesWType);


#undef WinAPIFunctionDecl



namespace EA{

namespace IO{

	namespace
	{
	#if defined(_MSC_VER) && (_MSC_VER >= 1300) // If VC7.0 or later...
		#define EAIOAlignOf(type) __alignof(type)
	#elif defined(__GNUC__)
		#define EAIOAlignOf(type) __alignof__(type)
	#else
		template <typename T> struct EAAlignOf1 { enum { s = sizeof (T), value = s ^ (s & (s - 1)) }; };
		template <typename T> struct EAAlignOf2;
		template <int size_diff> struct helper { template <typename T> struct Val { enum { value = size_diff }; }; };
		template <> struct helper<0> { template <typename T> struct Val { enum { value = EAAlignOf2<T>::value }; }; };
		template <typename T> struct EAAlignOf2 { struct Big { T x; char c; };
		enum { diff = sizeof (Big) - sizeof (T), value = helper<diff>::template Val<Big>::value }; };
		template <typename T> struct EAAlignof3 { enum { x = EAAlignOf2<T>::value, y = EAAlignOf1<T>::value, value = x < y ? x : y }; };
		#define EAIOAlignOf(type) ((size_t)EAAlignof3<type>::value)
	#endif
	}


static void __cdecl ThreadFunction(void* pContext);


///////////////////////////////////////////////////////////////////////////////
// FileChangeNotification
//
FileChangeNotification::FileChangeNotification(EA::Allocator::ICoreAllocator* pCoreAllocator)
  : mbInitialized(false),
	mpCoreAllocator(pCoreAllocator ? pCoreAllocator : IO::GetAllocator()),
	mpFileChangeCallbackContext(NULL),
	mnChangeTypeFlags(kChangeTypeFlagNone),
	mnOptionFlags(kOptionFlagDefaults),
	mpFileChangeCallback(NULL),
	mhDirectory(NULL),
	mhWaitingThread(NULL),
	mbThreadShouldRun(false)
{
	mDirectoryPath[0] = 0;

	memset(&mOverlapped, 0, sizeof(mOverlapped));
	mhEvents[0]  = NULL;
	mhEvents[1]  = NULL;

	EA_COMPILETIME_ASSERT(sizeof(FCNOverlapped) == sizeof(OVERLAPPED));
	EA_COMPILETIME_ASSERT(EAIOAlignOf(FCNOverlapped) >= EAIOAlignOf(OVERLAPPED));
	EA_COMPILETIME_ASSERT(sizeof(FCNHandle) == sizeof(HANDLE));
	EA_COMPILETIME_ASSERT(EAIOAlignOf(FCNHandle) >= EAIOAlignOf(HANDLE));
}


///////////////////////////////////////////////////////////////////////////////
// ~FileChangeNotification
//
FileChangeNotification::~FileChangeNotification()
{
	Shutdown();
}


///////////////////////////////////////////////////////////////////////////////
// Init
//
bool FileChangeNotification::Init()
{
	if(!mbInitialized)
	{
		// Nothing special required for this platform.
		mbInitialized = true;
	}
	return true;
}


///////////////////////////////////////////////////////////////////////////////
// Shutdown
//
bool FileChangeNotification::Shutdown()
{
	if(mbInitialized)
	{
		mbInitialized = false;

		if(mhWaitingThread)
			Stop();

		// Close the shutdown event. mhEvents[0] is the algorithm-specific
		// event created by the thread itself, so we don't close it here.
		if(mhEvents[1])
		{
			CloseHandle(mhEvents[1]);
			mhEvents[1] = NULL;
		}

		if(mhDirectory)
		{
			CloseHandle(mhDirectory);
			mhDirectory = NULL;
		}

		return true;
	}

	return false;
}



///////////////////////////////////////////////////////////////////////////////
// GetWatchedDirectory
//
const wchar_t* FileChangeNotification::GetWatchedDirectory() const
{
	return mDirectoryPath;
}



///////////////////////////////////////////////////////////////////////////////
// SetWatchedDirectory
//
bool FileChangeNotification::SetWatchedDirectory(const wchar_t* pDirectory)
{
	bool bReturnValue = false;

	if(!mhWaitingThread)
	{
		EA::StdC::Strlcpy(mDirectoryPath, pDirectory, kMaxDirectoryLength);
		bReturnValue = true;
	}

	return bReturnValue;
}

bool FileChangeNotification::SetWatchedDirectory(const char* pDirectory)
{
	wchar_t pDirectoryW[kMaxPathLength];
	EA::StdC::Strlcpy(pDirectoryW, pDirectory, kMaxPathLength);

	return SetWatchedDirectory(pDirectoryW);
}


///////////////////////////////////////////////////////////////////////////////
// SetNotificationCallback
//
void FileChangeNotification::SetNotificationCallback(FileChangeCallback pFileChangeCallback, void* pContext)
{
	// We actually have a thread safety issue here. 
	// If the user calls this function from multiple threads or if the user 
	// calls this function while a callback is reading these member variables
	// from a separate thread, bad things can happen.
	// One fix for this is to use an atomic 64 bit operation to modify both
	// variables at the same time in a thread-safe way.

	mpFileChangeCallback        = pFileChangeCallback;
	mpFileChangeCallbackContext = pContext;
}



///////////////////////////////////////////////////////////////////////////////
// GetChangeTypeFlags
//
int FileChangeNotification::GetChangeTypeFlags() const
{
	return mnChangeTypeFlags;
}



///////////////////////////////////////////////////////////////////////////////
// SetChangeTypeFlags
//
bool FileChangeNotification::SetChangeTypeFlags(int nChangeTypeFlags)
{
	bool bReturnValue = false;

	if(!mhWaitingThread)
	{
		mnChangeTypeFlags = nChangeTypeFlags;
		bReturnValue      = true;
	}

	return bReturnValue;
}



///////////////////////////////////////////////////////////////////////////////
// GetOptionFlags
//
int FileChangeNotification::GetOptionFlags() const
{
	return mnOptionFlags;
}



///////////////////////////////////////////////////////////////////////////////
// SetOptionFlags
//
bool FileChangeNotification::SetOptionFlags(int nOptionFlags)
{
	bool bReturnValue = false;

	if(!mhWaitingThread)
	{
		mnOptionFlags = nOptionFlags;
		bReturnValue  = true;
	}

	return bReturnValue;
}


///////////////////////////////////////////////////////////////////////////////
// SetAllocator
//
void FileChangeNotification::SetAllocator(EA::Allocator::ICoreAllocator* pCoreAllocator)
{
	mpCoreAllocator = pCoreAllocator;
}


///////////////////////////////////////////////////////////////////////////////
// Start
//
bool FileChangeNotification::Start()
{
	bool bReturnValue = false;

	if(!mhWaitingThread && (mnChangeTypeFlags != kChangeTypeFlagNone) && mDirectoryPath[0])
	{
		if(!mpCoreAllocator)
			SetAllocator(IO::GetAllocator());

		if(pReadDirectoryChangesW) // If using ReadDirectoryChangesW...
		{
			if(!mhDirectory)
				 mhDirectory = CreateFileW(mDirectoryPath,
										   FILE_LIST_DIRECTORY,
										   FILE_SHARE_READ | FILE_SHARE_WRITE | FILE_SHARE_DELETE,
										   NULL,
										   OPEN_EXISTING,
										   FILE_FLAG_BACKUP_SEMANTICS | FILE_FLAG_OVERLAPPED,
										   NULL);
		}

		mhEvents[1] = CreateEvent(NULL, FALSE, FALSE, NULL);

		if(!pReadDirectoryChangesW || mhDirectory)
		{
			mbThreadShouldRun = true;

			unsigned threadId;
			mhWaitingThread = (HANDLE)_beginthreadex(NULL, 0, ThreadFunction, static_cast<FileChangeNotification*>(this), 0, &threadId);
			if(mhWaitingThread)
				bReturnValue = true;
		}
	}

	return bReturnValue;
}



///////////////////////////////////////////////////////////////////////////////
// GetIsStarted
//
bool FileChangeNotification::GetIsStarted() const
{
	return mhWaitingThread != NULL;
}


///////////////////////////////////////////////////////////////////////////////
// Stop
//
bool FileChangeNotification::Stop()
{
	bool bReturnValue = false;

	if(mhWaitingThread)
	{
		mbThreadShouldRun = false;

		// Signal the waiting thread to exit and wait for it to do so.
		SetEvent(mhEvents[1]);
		WaitForSingleObject(mhWaitingThread, INFINITE);
		CloseHandle(mhWaitingThread);
		mhWaitingThread = NULL;

		bReturnValue = true;
	}

	return bReturnValue;
}


///////////////////////////////////////////////////////////////////////////////
// Poll
//
void FileChangeNotification::Poll(int /*timeoutMS*/)
{
	// Do nothing. We are actively driven by the OS file system.
}


///////////////////////////////////////////////////////////////////////////////
// SendNotification
//
void FileChangeNotification::SendNotification(const wchar_t* pDirectory, const wchar_t* pFile, int nChangeTypeFlags)
{
	if(mpFileChangeCallback)
	{
		// In this implementation we don't really know which change type 
		// occurred, so we just pass back mnChangeTypeFlags.
		mpFileChangeCallback(this, pDirectory, pFile, nChangeTypeFlags, mpFileChangeCallbackContext);
	}
}



///////////////////////////////////////////////////////////////////////////////
// ThreadFunction
//
unsigned __stdcall FileChangeNotification::ThreadFunction(void* pContext)
{
	FileChangeNotification* const pFileChangeNotification = reinterpret_cast<FileChangeNotification*>(pContext);

	const int      nChangeTypeFlags   = pFileChangeNotification->GetChangeTypeFlags();
	const wchar_t* pWatchedDirectory  = pFileChangeNotification->GetWatchedDirectory();
	DWORD          dwNotifyFilter     = 0;

	if(nChangeTypeFlags & FileChangeNotification::kChangeTypeFlagFileName)
		dwNotifyFilter |= FILE_NOTIFY_CHANGE_FILE_NAME | FILE_NOTIFY_CHANGE_CREATION;
	if(nChangeTypeFlags & FileChangeNotification::kChangeTypeFlagDirectoryName)
		dwNotifyFilter |= FILE_NOTIFY_CHANGE_DIR_NAME;
	if(nChangeTypeFlags & FileChangeNotification::kChangeTypeFlagModified)
		dwNotifyFilter |= FILE_NOTIFY_CHANGE_LAST_WRITE;
	if(nChangeTypeFlags & FileChangeNotification::kChangeTypeFlagAttributes)
		dwNotifyFilter |= FILE_NOTIFY_CHANGE_ATTRIBUTES;
	if(nChangeTypeFlags & FileChangeNotification::kChangeTypeFlagSize)
		dwNotifyFilter |= FILE_NOTIFY_CHANGE_SIZE;

	if(nChangeTypeFlags)
	{
		if(pReadDirectoryChangesW)
		{
			pFileChangeNotification->ThreadFunctionRDC(dwNotifyFilter);
		}
		else
		{
			if(pWatchedDirectory && pWatchedDirectory[0])
				pFileChangeNotification->ThreadFunctionFFCN(dwNotifyFilter, pWatchedDirectory);
		}
	}

	return 0;
}

///////////////////////////////////////////////////////////////////////////////
// ThreadFunctionRDC
//
void FileChangeNotification::ThreadFunctionRDC(uint32_t dwNotifyFilter)
{
	const int       nOptionFlags                = mnOptionFlags;
  //const bool      bStopAfterFirstNotification = (nOptionFlags & FileChangeNotification::kOptionFlagStopAfterFirstNotification) != 0;
	const BOOL      bWatchSubtree               = (nOptionFlags & FileChangeNotification::kOptionFlagWatchSubdirectories) ? 1 : 0;
	char            pBuffer[8192]; pBuffer[0] = 0;

	BOOL bResult = true;

	// Create event for overlapped I/O.
	mhEvents[0] = CreateEvent(NULL, FALSE, FALSE, NULL);
	if (!mhEvents[0])
		return;

	mOverlapped.hEvent = mhEvents[0];

	while(mbThreadShouldRun)
	{
		DWORD dwBytesReturned = 0;

		bResult = pReadDirectoryChangesW()(mhDirectory, pBuffer, sizeof(pBuffer), bWatchSubtree, dwNotifyFilter, &dwBytesReturned, (OVERLAPPED *)&mOverlapped, NULL);
		if (!bResult)
			break;

		// Wait for either the overlapped I/O event or the shutdown event
		// to fire.
		DWORD dwWaitResult = WaitForMultipleObjectsEx(2, mhEvents, FALSE, INFINITE, FALSE);

		// If the shutdown event fired or we got an error, exit.
		if (dwWaitResult != WAIT_OBJECT_0)
			break;

		if(mbThreadShouldRun)
		{
			FILE_NOTIFY_INFORMATION* pFNI = (FILE_NOTIFY_INFORMATION*)pBuffer;

			for(;;)
			{
				// Get file name.
				wchar_t pFileName[kMaxFileNameLength];
				memcpy(pFileName, pFNI->FileName, pFNI->FileNameLength); // Docs say that FileNameLength is in bytes.
				pFileName[pFNI->FileNameLength / sizeof(wchar_t)] = 0;

				// Convert file name from short name to long name.
				Path::PathStringW path(mDirectoryPath);
				Path::Join(path, pFileName);
				Path::PathStringW longPath;
				longPath.resize(kMaxPathLength); // Ensure that GetLongPathNameW has enough space
				DWORD dwResult = GetLongPathNameW(path.c_str(), &longPath[0], kMaxPathLength);
				if((dwResult == 0) || (dwResult > kMaxPathLength))
					longPath = path;

				// Filter out undesired notifications
				// Unfortunately, ReadDirectoryChanges will tell you a directory changed 
				// when some file within that directory changed. We check for !!EA::IO::Directory::Exists
				// because that will tell us if the path is not a directory but instead is a file.
				// In reviewing this code, I'm (Paul Pedriana) not sure why it doesn't use EA::IO::File::Exists
				// other than possibly the OS may return true for File::Exists if it is a directory.
				if((mnChangeTypeFlags & FileChangeNotification::kChangeTypeFlagDirectoryName) || !Directory::Exists(longPath.c_str()))
				{
					// Split the long path apart again.
					Path::PathStringW::const_iterator itFileName(Path::GetFileName(longPath));
					const wchar_t* pLongFileName = longPath.c_str() + (itFileName - longPath.begin());

					EA::StdC::Strlcpy(pFileName, pLongFileName, kMaxFileNameLength);
					if(*pLongFileName)
						*const_cast<wchar_t*>(pLongFileName) = 0;

					// Do notification.
					int nChangeFlag = kChangeTypeFlagModified;

					if(mnChangeTypeFlags & kChangeTypeFlagSize) // we don't attempt to distinguish, assuming avoiding false positives (kChangeTypeFlagModified was *also* set and write time changed but size did not) is not worth extra cost of checking size change
						nChangeFlag |= kChangeTypeFlagSize;

					if(pFNI->Action == FILE_ACTION_ADDED || pFNI->Action == FILE_ACTION_RENAMED_NEW_NAME)
					{
						// The FILE_NOTIFY_INFORMATION doesn't specify if it's referring to a file or directory.
						// We can tell by seeing if the path refers to a file or directory path.
						if(EA::IO::Directory::Exists(path.c_str()))
							nChangeFlag = kChangeTypeFlagDirectoryName;
						else
							nChangeFlag = kChangeTypeFlagFileName;
					}
					else if(pFNI->Action == FILE_ACTION_REMOVED || pFNI->Action == FILE_ACTION_RENAMED_OLD_NAME)
					{
						// We have no easy means of telling if the item deleted was a file or a directory.
						nChangeFlag = kChangeTypeFlagFileName | kChangeTypeFlagDirectoryName;
					}

					SendNotification(longPath.c_str(), pFileName, nChangeFlag); // longPath16 is the directory, pFileName is the file name or directory name alone.
				}

				// Move to possible next record.
				if(pFNI->NextEntryOffset)
					pFNI = (FILE_NOTIFY_INFORMATION*)((char*)pFNI + pFNI->NextEntryOffset);
				else
					break;
			}
		}
	}

	// Cancel the overlapped ReadDirectoryChangesW() call.
	CancelIo(mhDirectory);

	// Close overlapped I/O event.
	CloseHandle(mhEvents[0]);
	mhEvents[0] = NULL;
}

///////////////////////////////////////////////////////////////////////////////
// ThreadFunctionFFCN
//
void FileChangeNotification::ThreadFunctionFFCN(uint32_t dwNotifyFilter, const wchar_t* pWatchedDirectory)
{
	const int  nOptionFlags                = mnOptionFlags;
	const bool bStopAfterFirstNotification = (nOptionFlags & FileChangeNotification::kOptionFlagStopAfterFirstNotification) != 0;
	const BOOL bWatchSubtree               = (nOptionFlags & FileChangeNotification::kOptionFlagWatchSubdirectories) ? 1 : 0;

	mhEvents[0] = ::FindFirstChangeNotificationW(pWatchedDirectory, bWatchSubtree, dwNotifyFilter);

	if(mhEvents[0] == INVALID_HANDLE_VALUE)
		return;

	while(mbThreadShouldRun)
	{
		// Wait for change notification or shutdown request.
		DWORD dwWaitStatus = ::WaitForMultipleObjects(2, mhEvents, FALSE, INFINITE); 

		// Exit if requested to do so or if we encounter an error (i.e. it
		// wasn't the change notification event that fired).
		if (!mbThreadShouldRun || dwWaitStatus != WAIT_OBJECT_0)
			break;

		// Notify client.
		SendNotification(mDirectoryPath, NULL, mnChangeTypeFlags);

		// Exit if we're supposed to stop after the first notification.
		if (bStopAfterFirstNotification)
			break;

		// Reinstate change notification.
		if (!::FindNextChangeNotification(mhEvents[0]))
			break;
	}

	// We don't close mhEvents[1] as that is the shutdown event created
	// by the main thread.
	::FindCloseChangeNotification(mhEvents[0]);
}



} // namespace IO

} // namespace EA


#else // Other platforms.


#include <EAIO/EAFileDirectory.h>
#include <EAIO/EAFileUtil.h>

#if defined(EA_PLATFORM_MICROSOFT)
	#ifndef WIN32_LEAN_AND_MEAN
		#define WIN32_LEAN_AND_MEAN
	#endif
	#if EA_COMPILER_VERSION >= 1900 // VS2015+
	// required for windows.h that has mismatch that is included in this file
		EA_DISABLE_VC_WARNING(5031 5032)// #pragma warning(pop): likely mismatch, popping warning state pushed in different file / detected #pragma warning(push) with no corresponding
	#endif
	#include <Windows.h>
	#if EA_COMPILER_VERSION >= 1900 // VS2015+
		EA_RESTORE_VC_WARNING()// #pragma warning(pop): likely mismatch, popping warning state pushed in different file / detected #pragma warning(push) with no corresponding
	#endif
#elif EAIO_USE_UNIX_IO
	#include <sys/stat.h>
	#include <sys/types.h>
	#include <time.h>
	#ifdef EA_PLATFORM_NX
		#include <nn/fs/fs_FileSystem.h>
		#include <nn/fs/fs_File.h>
	#endif
#endif

#ifdef _MSC_VER
	#pragma warning(disable: 4127) // conditional expression is constant
#endif


/// GetCurrentFCNTime
///
/// Returns the current time in milliseconds.
/// It's less important that the absolute time be accurate but rather
/// that relative milliseconds be accurate.
/// 
static int GetCurrentFCNTime()
{
	#if defined (EA_PLATFORM_MICROSOFT)
		#if EA_WINAPI_FAMILY_PARTITION(EA_WINAPI_PARTITION_DESKTOP)
			return (int)GetTickCount();
		#else
			return (int)GetTickCount64();
		#endif
	#elif EAIO_USE_UNIX_IO
		return (int)(clock() * INT64_C(1000) / CLOCKS_PER_SEC);
	#elif defined(EA_PLATFORM_SONY)
		SceKernelTimespec timeVal;
		sceKernelClockGettime(SCE_KERNEL_CLOCK_MONOTONIC, &timeVal);
		double time = static_cast<double>(timeVal.tv_sec) * 1000.0 + static_cast<double>(timeVal.tv_nsec) * 1.0e-6;
		return static_cast<int>(time);
	#else
		return 0; // Fill in others here as needed.
	#endif
}

namespace EA{

namespace IO{



///////////////////////////////////////////////////////////////////////////////
// FSEntryHashCompare::operator()
//
// Note that we don't has the pointer but has the value.
//
size_t FileChangeNotification::FSEntryHashCompare::operator()(const FSEntryARC& fseARC) const
{
	const wchar_t* pString = fseARC->msName.c_str();
	size_t c, nValue = 0x811c9dc5; // Use the FNV string hash algorithm.
	while((c = (size_t)((uint16_t)*pString++)) != 0)
		nValue = (size_t)((nValue * 16777619) ^ c);
	return nValue;
}


///////////////////////////////////////////////////////////////////////////////
// FSEntryHashCompare::operator()
//
// Note that we don't compare pointers but instead compare values.
//
bool FileChangeNotification::FSEntryHashCompare::operator()(const FSEntryARC& fseARC1, const FSEntryARC& fseARC2) const 
{
	return (fseARC1->mpParentEntry == fseARC2->mpParentEntry) && (fseARC1->msName == fseARC2->msName);
}                                                                                                 


///////////////////////////////////////////////////////////////////////////////
// FileChangeNotification
//
FileChangeNotification::FileChangeNotification(EA::Allocator::ICoreAllocator* pCoreAllocator)
  : mbInitialized(false),
	mpCoreAllocator(pCoreAllocator ? pCoreAllocator : IO::GetAllocator()),
	//mDirectoryPath(),
	mnChangeTypeFlags(kChangeTypeFlagNone),
	mnOptionFlags(kOptionFlagDefaults),
	mpFileChangeCallback(NULL),
	mpFileChangeCallbackContext(NULL),
  #if EAIO_THREAD_SAFETY_ENABLED
	mnBusy(0),
	mThread(),
  #endif
	mbThreadShouldRun(false),
	mbStarted(false),
	mEntryDirectoryRoot(),
	mpEntryDirectoryCurrent(NULL),
	mCurrentChangeSet(EA::Allocator::EAIOEASTLCoreAllocator(EAIO_ALLOC_PREFIX "EAFileChangeNotification/FSEntrySet", mpCoreAllocator))
{
	mDirectoryPath[0] = 0;
	mEntryDirectoryRoot.mpCoreAllocator = mpCoreAllocator;
}


///////////////////////////////////////////////////////////////////////////////
// ~FileChangeNotification
//
FileChangeNotification::~FileChangeNotification()
{
	Shutdown();
}


///////////////////////////////////////////////////////////////////////////////
// Init
//
bool FileChangeNotification::Init()
{
	if(!mbInitialized)
	{
		// Nothing special required for this platform.
		mbInitialized = true;
	}
	return true;
}


///////////////////////////////////////////////////////////////////////////////
// Shutdown
//
bool FileChangeNotification::Shutdown()
{
	if(mbInitialized)
	{
		mbInitialized = false;
		if(mbStarted)
			Stop();
		return true;
	}
	return false;
}


///////////////////////////////////////////////////////////////////////////////
// Start
//
bool FileChangeNotification::Start()
{
	if(!mbStarted)
	{
		if(!mpCoreAllocator)
			SetAllocator(IO::GetAllocator());

		if(EA::IO::Directory::Exists(mDirectoryPath))
		{
			mbStarted = true;

			mEntryDirectoryRoot.mChildEntrySet.clear(); // This will Release all entries recursively.
			BuildEntry(&mEntryDirectoryRoot, mDirectoryPath, NULL, true);
			mpEntryDirectoryCurrent = NULL;
			// TraceEntryTree(&mEntryDirectoryRoot); // Debugging only.

			#if EAIO_THREAD_SAFETY_ENABLED
				if(mnOptionFlags & kOptionFlagMultithreaded)
				{
					mbThreadShouldRun = true;
					Thread::ThreadParameters tp;
					// In this thread we need more than the default 8K
					tp.mnStackSize = 32768;
					mThread.Begin(this, NULL, &tp);
				}
			#endif
		}
	}
	return mbStarted;
}


///////////////////////////////////////////////////////////////////////////////
// GetIsStarted
//
bool FileChangeNotification::GetIsStarted() const
{
	return mbStarted;
}


	///////////////////////////////////////////////////////////////////////////////
// Stop
//
bool FileChangeNotification::Stop()
{
	if(mbStarted)
	{
		mbStarted = false;

		#if EAIO_THREAD_SAFETY_ENABLED
			if(mThread.GetStatus() == EA::Thread::Thread::kStatusRunning)
			{
				mbThreadShouldRun = false;
				mThread.WaitForEnd();
			}
		#endif
	}
	return true;
}


///////////////////////////////////////////////////////////////////////////////
// Poll
//
// Iterates the file system, looking for changes.
//
void FileChangeNotification::Poll(int timeoutMS)
{
	if(mbStarted)
	{
		#if EAIO_THREAD_SAFETY_ENABLED
		if(mnBusy.SetValueConditional(1, 0)) // If we can grab the mini-mutex...
		#endif
		{
			const int timeoutAbsolute = GetCurrentFCNTime() + timeoutMS;
			if(!mpEntryDirectoryCurrent)
			{
				mpEntryDirectoryCurrent = &mEntryDirectoryRoot;
				mEntryDirectoryRoot.mChildEntrySetIt = mEntryDirectoryRoot.mChildEntrySet.begin();
			}
			mCurrentChangeSet.clear();

			PollInternal(mpEntryDirectoryCurrent, timeoutAbsolute);

			for(FSEntrySet::iterator it(mCurrentChangeSet.begin()); it != mCurrentChangeSet.end(); ++it)
			{
				const FSEntry* const pFSEntry = *it;

				wchar_t pDirectoryPath[EA::IO::kMaxPathLength];
				EA_ASSERT(!pFSEntry->mbDirectory && pFSEntry->mpParentEntry);
				GetEntryPath(pFSEntry->mpParentEntry, pDirectoryPath);

				#if defined(EA_TRACE_ENABLED) && EA_TRACE_ENABLED // If EATrace is available and we aren't instead using something else...
					EA_TRACE_FORMATTED(("FileChangeNotification: File changed: %d \"%ls%ls\"\n", pFSEntry->mnChangeFlags, pDirectoryPath, pFSEntry->msName.c_str()));
				#endif

				SendNotification(pDirectoryPath, pFSEntry->msName.c_str(), pFSEntry->mnChangeFlags);
			}

			#if EAIO_THREAD_SAFETY_ENABLED
				mnBusy = 0;
			#endif
		}
	}
}


///////////////////////////////////////////////////////////////////////////////
// PollInternal
//
// Upon return from this function, mCurrentChangeSet will have the set of changes
// (if any) that were detected.
//
// What we implement here is directory tree iteration that can be 
// interrupted at any time and which collects detected changes 
// as it goes. Pretty tricky indeed.
//
bool FileChangeNotification::PollInternal(FSEntry* pFSEntryBase, int timeoutAbsolute)
{
	bool bTimeExpired = false;

	EA_ASSERT(pFSEntryBase->mbDirectory);

	// Get a list of the current file entries in this directory.
	// We will compare these to our tree.
	FSEntrySet childEntrySet(EA::Allocator::EAIOEASTLCoreAllocator(EAIO_ALLOC_PREFIX "EAFileChangeNotification/FSEntrySet", mpCoreAllocator));
	GetChildFileEntrySet(pFSEntryBase, childEntrySet);

	// We continue our iteration of this directory where we last left off.
	while((pFSEntryBase->mChildEntrySetIt != pFSEntryBase->mChildEntrySet.end()) && !bTimeExpired)
	{
		FSEntry* const pFSEntryTemp = *pFSEntryBase->mChildEntrySetIt;
		bool bAdvanceToNext = true;     // True if we need to advance to the next dir

		if(pFSEntryTemp->mbDirectory)
		{
			// We ignore the case of this directory having been removed.
			if(EntryExists(pFSEntryTemp))
			{
				#if defined(EA_TRACE_ENABLED) && EA_TRACE_ENABLED && FCN_POLL_TRACE_ENABLED
					wchar_t pPath[EA::IO::kMaxPathLength]; // Will be a directory path ending in directory separator or will be a file path.
					GetEntryPath(pFSEntryTemp, pPath);
					EA_TRACE_FORMATTED(("FileChangeNotification: Polling directory: \"%ls\"...\n", pPath));
				#endif

				mpEntryDirectoryCurrent        = pFSEntryTemp;
				pFSEntryTemp->mChildEntrySetIt = pFSEntryTemp->mChildEntrySet.begin();
				bTimeExpired = PollInternal(pFSEntryTemp, timeoutAbsolute);
			}
		}
		else
		{
			#if defined(EA_TRACE_ENABLED) && EA_TRACE_ENABLED && FCN_POLL_TRACE_ENABLED
				wchar_t pPath[EA::IO::kMaxPathLength]; // Will be a directory path ending in directory separator or will be a file path.
				GetEntryPath(pFSEntryTemp, pPath);
				EA_TRACE_FORMATTED(("FileChangeNotification: Polling file: \"%ls\"\n", pPath));
			#endif

			FSEntrySet::iterator it = childEntrySet.find(pFSEntryTemp);

			if(it == childEntrySet.end()) // If fsEntryTemp was removed...
			{
				// Add fsEntryTemp to the file change list.
				pFSEntryTemp->mnChangeFlags = kChangeTypeFlagFileName;
				mCurrentChangeSet.insert(pFSEntryTemp);

				// Remove fsEntryTemp from our tree.
				pFSEntryBase->mChildEntrySetIt = pFSEntryBase->mChildEntrySet.erase( pFSEntryBase->mChildEntrySetIt );
				bAdvanceToNext = false; // We erased the entry, so we don't need to advance
			}
			else
			{
				// Get/update the change time of fsEntryTemp.
				// We want to re-read the file time and size because it may have changed since we last read it, 
				// and the purpose of this class is to detect such changes.
				time_t nCurrentTime;
				size_type nCurrentSize;
				GetFileTimeAndSize(pFSEntryTemp, &nCurrentTime, &nCurrentSize);

				if(nCurrentTime != pFSEntryTemp->mnTime || nCurrentSize != pFSEntryTemp->mnSize)
				{
					// Add fsEntryTemp to the file change list.
					if(nCurrentTime != pFSEntryTemp->mnTime)
					{
						pFSEntryTemp->mnChangeFlags |= kChangeTypeFlagModified;
						pFSEntryTemp->mnTime = nCurrentTime;
					}
					if(nCurrentSize != pFSEntryTemp->mnSize)
					{
						pFSEntryTemp->mnChangeFlags |= kChangeTypeFlagSize;
						pFSEntryTemp->mnSize = nCurrentSize;
					}

					mCurrentChangeSet.insert(pFSEntryTemp);
				}

				// We won't be needing to refer to this entry any more, 
				// so we can be slightly more efficient by removing it now.
				childEntrySet.erase(it);
			}
		}

		if(!bTimeExpired)
		{
			if (bAdvanceToNext)
				++pFSEntryBase->mChildEntrySetIt;
			bTimeExpired = (GetCurrentFCNTime() >= timeoutAbsolute);
		}
	}

	// We are done with this directory. Reset our iterator and go back to the parent directory.
	if(pFSEntryBase->mChildEntrySetIt == pFSEntryBase->mChildEntrySet.end())
	{
		// Now check for any entries in childEntrySet that weren't in our tree. 
		// These entries would be new entries to the tree.
		for(FSEntrySet::iterator it = childEntrySet.begin(); it != childEntrySet.end(); ++it)
		{
			FSEntry* const pFSEntryTemp = *it;

			if(pFSEntryBase->mChildEntrySet.find(pFSEntryTemp) == pFSEntryBase->mChildEntrySet.end()) // If we appear to have a new file...
			{
				// Add fsEntryTemp to the file change list.
				pFSEntryTemp->mnChangeFlags = kChangeTypeFlagFileName;
				mCurrentChangeSet.insert(pFSEntryTemp);

				// Add the new fsEntry to our tree. This insert might result in invalidating
				// mChildEntrySetIt, but that's OK because we will be resetting it to 
				// mChildEntrySet.begin() below.
				pFSEntryBase->mChildEntrySet.insert(pFSEntryTemp);
			}
		}

		pFSEntryBase->mChildEntrySetIt = pFSEntryBase->mChildEntrySet.begin();
		mpEntryDirectoryCurrent        = pFSEntryBase->mpParentEntry;

		// If the time has expired, then advance the iterator to skip over the
		// parent directory (since the caller won't do it with the expired flag set.)
		if(bTimeExpired && mpEntryDirectoryCurrent)
			++mpEntryDirectoryCurrent->mChildEntrySetIt;
	}

	return bTimeExpired;
}


///////////////////////////////////////////////////////////////////////////////
// Run
//
intptr_t FileChangeNotification::Run(void* /*pContext*/)
{
	while(mbThreadShouldRun)
	{
		Poll(100);

		#if EAIO_THREAD_SAFETY_ENABLED
			EA::Thread::ThreadSleep(3000);
		#elif defined(EA_PLATFORM_WINDOWS)
			Sleep(3000);
		#else
			// To consider: Implement something.
		#endif
	}

	return 0;
}


int FileChangeNotification::FSEntry::AddRef()
{
	return ++mnRefCount;
}


int FileChangeNotification::FSEntry::Release()
{
	if(mnRefCount > 1)
		return --mnRefCount;

	EA_ASSERT(mpCoreAllocator != NULL);
	EA_CA_DELETE(this, mpCoreAllocator);
	return 0;
}


void FileChangeNotification::FSEntry::SetAllocator(EA::Allocator::ICoreAllocator* pCoreAllocator)
{
	mpCoreAllocator = pCoreAllocator;
	mChildEntrySet.get_allocator().set_allocator(pCoreAllocator);

	#if defined(EASTL_VERSION_N) && (EASTL_VERSION_N >= 11000)
		msName.get_overflow_allocator().set_allocator(pCoreAllocator);
	#endif
}


///////////////////////////////////////////////////////////////////////////////
// GetChildFileEntrySet
//
void FileChangeNotification::GetChildFileEntrySet(FSEntry* pFSEntryBase, FSEntrySet& childEntrySet)
{
	DirectoryIterator            directoryIterator;
	DirectoryIterator::EntryList entryList(DirectoryIterator::EntryList::allocator_type(EASTL_NAME_VAL(EAIO_ALLOC_PREFIX "EAFileNotification/FSEntry")));
	wchar_t                      pPath[EA::IO::kMaxPathLength]; // Will be a directory path ending in directory separator or will be a file path.

	GetEntryPath(pFSEntryBase, pPath);

	if(directoryIterator.Read(pPath, entryList, NULL, kDirectoryEntryFile))
	{
		for(DirectoryIterator::EntryList::iterator it = entryList.begin(); it != entryList.end(); ++it)
		{
			const DirectoryIterator::Entry& entry = *it;

			EA_ASSERT(pFSEntryBase->mpCoreAllocator != NULL);
			FSEntryARC pFSEntry = EA_CA_NEW(FSEntry, pFSEntryBase->mpCoreAllocator, EAIO_ALLOC_PREFIX "EAFileChangeNotification/FSEntry");

			if(pFSEntry)
			{
				pFSEntry->msName.assign(entry.msName.data(), entry.msName.length());
				pFSEntry->mbDirectory = false;
				pFSEntry->mpParentEntry = pFSEntryBase;
				pFSEntry->mnChangeFlags = 0;
				pFSEntry->mpCoreAllocator = pFSEntryBase->mpCoreAllocator;
				GetFileTimeAndSize(pFSEntry, &pFSEntry->mnTime, &pFSEntry->mnSize);

				childEntrySet.insert(pFSEntry); // AutoRefCount operation.
			}
		}
	}
}


///////////////////////////////////////////////////////////////////////////////
// GetEntryPath
//
// Gets a full path from a single entry.
//
void FileChangeNotification::GetEntryPath(const FSEntry* pFSEntry, wchar_t* pPath, 
										  uint32_t nMaxPermittedLength)
{
	const FSEntry* entryArray[32];
	int            entryArrayCount = 0;
	wchar_t*       pPathCurrent = pPath;

	while(pFSEntry)
	{
		entryArray[entryArrayCount++] = pFSEntry;
		pFSEntry = pFSEntry->mpParentEntry;
	}

	while(--entryArrayCount >= 0)
	{
		const wchar_t*  const pCurrent = entryArray[entryArrayCount]->msName.c_str();
		const bool      bDirectory     = entryArray[entryArrayCount]->mbDirectory;
		const uint32_t  nCurrentLength = (uint32_t)EA::StdC::Strlen(pCurrent);

		EA::StdC::Strlcpy(pPathCurrent, pCurrent, nMaxPermittedLength);

		pPathCurrent        += nCurrentLength;
		nMaxPermittedLength -= nCurrentLength;

		if((entryArrayCount > 0) || bDirectory)
		{
			if(EA::IO::Path::EnsureTrailingSeparator(pPath, nMaxPermittedLength))  // If one was added...
				++pPathCurrent;
		}
	}
}


///////////////////////////////////////////////////////////////////////////////
// EntryExists
//
bool FileChangeNotification::EntryExists(const FSEntry* pFSEntry)
{
	wchar_t pPath[EA::IO::kMaxPathLength]; // Will be a directory path ending in directory separator or will be a file path.
	GetEntryPath(pFSEntry, pPath);
	if(pFSEntry->mbDirectory)
		return EA::IO::Directory::Exists(pPath);
	return EA::IO::File::Exists(pPath);
}


///////////////////////////////////////////////////////////////////////////////
// GetFileTimeAndSize
//
void FileChangeNotification::GetFileTimeAndSize(const FSEntry* pFSEntry, time_t* pTime, size_type* pSize)
{
	wchar_t pPath[EA::IO::kMaxPathLength]; // Will be a directory path ending in directory separator or will be a file path.
	GetEntryPath(pFSEntry, pPath);

	// We directly use stat instead of making separate calls to GetTime and GetSize. This is because
	// GetTime and GetSize would both call stat internally and be a bit slower. To do: Implement this
	// approach for other platforms as well.

	#if defined(EA_PLATFORM_NX)
		Path::PathString8 path8;
		ConvertPath(path8, pPath);

		nn::fs::FileHandle fh;
		if (nn::fs::OpenFile(&fh, path8.c_str(), nn::fs::OpenMode_Read).IsSuccess())
		{
			int64_t fs;
			nn::fs::GetFileSize(&fs, fh);

			*pTime = 0;	// There is no way to get file time on NX (other than a no-ship, debug only function)
			*pSize = fs;

			nn::fs::CloseFile(fh);
		}
		else
		{
			*pTime = 0;
			*pSize = 0;
		}
	#elif EAIO_USE_UNIX_IO
		Path::PathString8 path8;
		ConvertPath(path8, pPath);

		#if defined(EA_PLATFORM_ANDROID)
			const char *path8Cstr = path8.c_str();
			if(strstr(path8Cstr, APPBUNDLE_PATH_PREFIX) == path8Cstr) // If the path begins with the APPBUNDLE_PATH_PREFIX prefix..
			{
				*pTime = EA::IO::File::GetTime(pPath, EA::IO::kFileTimeTypeLastModification);
				*pSize = EA::IO::File::GetSize(pPath);
			}
		#endif

		struct stat tempStat;
		const int result = stat(path8.c_str(), &tempStat);
		if (result == 0)
		{
			*pTime = (time_t)tempStat.st_mtime;
			*pSize = (size_type)tempStat.st_size;
		}
		else
		{
			*pTime = 0;
			*pSize = 0;
		}
	#else
		*pTime = EA::IO::File::GetTime(pPath, EA::IO::kFileTimeTypeLastModification);
		*pSize = EA::IO::File::GetSize(pPath);
	#endif
}


///////////////////////////////////////////////////////////////////////////////
// BuildEntry
//
// Sets up the given entry and recursively sets up child entries.
//
void FileChangeNotification::BuildEntry(FSEntry* pFSEntryBase, const wchar_t* pName, FSEntry* pEntryParent, bool bDirectory)
{
	pFSEntryBase->msName        = pName;
	pFSEntryBase->mbDirectory   = bDirectory;
	pFSEntryBase->mpParentEntry = pEntryParent;
	pFSEntryBase->mnChangeFlags = 0;

	// We remove a trailing file separator from the name if it is present.
	if(bDirectory && !pFSEntryBase->msName.empty() && EA::IO::IsFilePathSeparator(pFSEntryBase->msName.back()))
		pFSEntryBase->msName.pop_back();

	wchar_t pPath[EA::IO::kMaxPathLength]; // Will be a directory path ending in directory separator or will be a file path.
	GetEntryPath(pFSEntryBase, pPath);

	if(bDirectory)
	{
		pFSEntryBase->mnTime = 0;
		pFSEntryBase->mnSize = 0;
	}
	else
	{
		pFSEntryBase->mnTime = EA::IO::File::GetTime(pPath, EA::IO::kFileTimeTypeLastModification);
		pFSEntryBase->mnSize = EA::IO::File::GetSize(pPath);
	}

	if(bDirectory)
	{
		DirectoryIterator            directoryIterator;
		DirectoryIterator::EntryList entryList(DirectoryIterator::EntryList::allocator_type(EASTL_NAME_VAL(EAIO_ALLOC_PREFIX "EAFileNotification/FSEntry"), mpCoreAllocator));
		const int                    nEntryFlags = (mnOptionFlags & kOptionFlagWatchSubdirectories) ? kDirectoryEntryDirectory | kDirectoryEntryFile : kDirectoryEntryFile;

		if(directoryIterator.Read(pPath, entryList, NULL, nEntryFlags))
		{
			for(DirectoryIterator::EntryList::iterator it = entryList.begin(); it != entryList.end(); ++it)
			{
				const DirectoryIterator::Entry& entry = *it;

				FSEntryARC pFSEntry = EA_CA_NEW(FSEntry, pFSEntryBase->mpCoreAllocator, EAIO_ALLOC_PREFIX "EAFileChangeNotification/FSEntry");
				pFSEntry->mpCoreAllocator = pFSEntryBase->mpCoreAllocator;

				BuildEntry(pFSEntry, entry.msName.c_str(), pFSEntryBase, entry.mType == kDirectoryEntryDirectory);
				pFSEntryBase->mChildEntrySet.insert(pFSEntry); // AutoRefCount operation.
			}
		}
	}    
}


///////////////////////////////////////////////////////////////////////////////
// TraceEntryTree
//
// Prints out a user-readable text tree of the file system as we see it.
//
void FileChangeNotification::TraceEntryTree(const FSEntry* pFSEntry, int nCurrentDepth)
{
	#if defined(EA_TRACE_ENABLED) && EA_TRACE_ENABLED
		Path::PathString8 path8;
		const int  kSpaceSize = 3;
		const int  kMaxDepth  = 32;
		char       leadingSpace[(kMaxDepth + 1) * kSpaceSize];
		int        i;

		// If this is the first time through, print the tree base.
		if(nCurrentDepth == 0)
		{
			ConvertPath(path8, pFSEntry->msName.c_str());

			#if defined(EA_TRACE_ENABLED) && EA_TRACE_ENABLED // If EATrace is available and we aren't instead using something else...
				if(pFSEntry->mbDirectory)
					EA_TRACE_FORMATTED(("%s%c", path8.c_str(), EA_FILE_PATH_SEPARATOR_STRING_8));
				else
					EA_TRACE_MESSAGE(path8.c_str());
			#endif

			++nCurrentDepth;
		}

		// Setup the indenting space.
		for(i = 0; (i < nCurrentDepth) && (i < kMaxDepth); i++){
			for(int j = 0; j < kSpaceSize; j++)
				leadingSpace[(i * kSpaceSize) + j] = ' ';
		}
		leadingSpace[i * kSpaceSize] = 0;

		// Trace the elements.
		for(FSEntrySet::const_iterator it = pFSEntry->mChildEntrySet.begin(); it != pFSEntry->mChildEntrySet.end(); ++it)
		{
			const FSEntry* const pFSEntry = *it;
			ConvertPath(path8, pFSEntry->msName.c_str());

			#if defined(EA_TRACE_ENABLED) && EA_TRACE_ENABLED // If EATrace is available and we aren't instead using something else...
				if(pFSEntry->mbDirectory)
					EA_TRACE_FORMATTED(("%s%s%c", leadingSpace, path8.c_str(), EA_FILE_PATH_SEPARATOR_STRING_8));
				else
					EA_TRACE_FORMATTED(("%s%s", leadingSpace, path8.c_str()));
			#endif

			if(pFSEntry->mbDirectory)
				TraceEntryTree(pFSEntry, nCurrentDepth + 1);
		}
	#else
		(void)pFSEntry;
		(void)nCurrentDepth;
	#endif
}


///////////////////////////////////////////////////////////////////////////////
// GetWatchedDirectory
//
const wchar_t* FileChangeNotification::GetWatchedDirectory() const
{
	return mDirectoryPath;
}


///////////////////////////////////////////////////////////////////////////////
// SetWatchedDirectory
//
bool FileChangeNotification::SetWatchedDirectory(const wchar_t* pDirectory)
{
	if(!mbStarted)
	{
		EA::StdC::Strlcpy(mDirectoryPath, pDirectory, kMaxDirectoryLength);
		return true;
	}
	return false;
}

bool FileChangeNotification::SetWatchedDirectory(const char* pDirectory)
{
	Path::PathStringW directoryW;
	ConvertPath(directoryW, pDirectory);

	return SetWatchedDirectory(directoryW.c_str());
}



///////////////////////////////////////////////////////////////////////////////
// SetNotificationCallback
//
void FileChangeNotification::SetNotificationCallback(FileChangeCallback pFileChangeCallback, void* pContext)
{
	mpFileChangeCallback        = pFileChangeCallback;
	mpFileChangeCallbackContext = pContext;
}


///////////////////////////////////////////////////////////////////////////////
// GetChangeTypeFlags
//
int FileChangeNotification::GetChangeTypeFlags() const
{
	return mnChangeTypeFlags;
}


///////////////////////////////////////////////////////////////////////////////
// SetChangeTypeFlags
//
bool FileChangeNotification::SetChangeTypeFlags(int nChangeTypeFlags)
{
	if(!mbStarted)
	{
		mnChangeTypeFlags = nChangeTypeFlags;
		return true;
	}
	return false;
}


///////////////////////////////////////////////////////////////////////////////
// GetOptionFlags
//
int FileChangeNotification::GetOptionFlags() const
{
	return mnOptionFlags;
}


///////////////////////////////////////////////////////////////////////////////
// SetOptionFlags
//
bool FileChangeNotification::SetOptionFlags(int nOptionFlags)
{
	if(!mbStarted)
	{
		mnOptionFlags = nOptionFlags;
		return true;
	}
	return false;
}


///////////////////////////////////////////////////////////////////////////////
// SetAllocator
//
void FileChangeNotification::SetAllocator(EA::Allocator::ICoreAllocator* pCoreAllocator)
{
	mpCoreAllocator = pCoreAllocator;
	mCurrentChangeSet.get_allocator().set_allocator(pCoreAllocator);
	mEntryDirectoryRoot.SetAllocator(pCoreAllocator);
}


///////////////////////////////////////////////////////////////////////////////
// SendNotification
//
void FileChangeNotification::SendNotification(const wchar_t* pDirectory, const wchar_t* pFile, int nChangeTypeFlags)
{
	if(mpFileChangeCallback)
	{
		// In this implementation we don't really know which change type 
		// occurred, so we just pass back mnChangeTypeFlags.
		mpFileChangeCallback(this, pDirectory, pFile, nChangeTypeFlags, mpFileChangeCallbackContext);
	}
}


} // namespace EA

} // namespace IO


#endif // Platforms-specific switch
