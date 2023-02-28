///////////////////////////////////////////////////////////////////////////////
// AllocationRecorder 
//
// Copyright (c) 2004, Electronic Arts Inc. All rights reserved.
// By Paul Pedriana, Maxis
//
// The class records and plays back allocation activity. This is useful for 
// offline analysis of allocation activity and for testing allocators.
//
///////////////////////////////////////////////////////////////////////////////
//
// Lint settings
// The following error suppressions largely represent warnings that can't 
// be easily fixed in code but are also known to be insignificant for this module.
//lint -e1551                // Function may throw exception.
//lint -e1740                // Pointer member not directly freed or zero'ed by destructor.
//lint -esym(644,p)          // Variable may not have been initialized.
//lint -e613                 // Possible use of null pointer.
//lint -e534                 // Ignoring return value of function.
//lint -e539                 // Did not expect positive indentation.
//lint -e525                 // Negative indentation.
//lint -esym(429,pPtr)       // Custodial pointer has not been freed or returned.
//lint -esym(429,pData)      // Custodial pointer has not been freed or returned.
//lint -e605                 // Increase in pointer capability.
///////////////////////////////////////////////////////////////////////////////



#include <PPMalloc/EAAllocationRecorder.h>
#include <PPMalloc/internal/shared.h>
#include <stdio.h>
#include <string.h>
#include <stdlib.h>

#if   defined(EA_PLATFORM_MICROSOFT)
	#pragma warning(push, 0)
	#include <Windows.h> // For a little bit of debug code below.
	#pragma warning(pop)
#elif defined(EA_PLATFORM_ANDROID)
	#include <android/log.h>
#endif

#if defined(EA_COMPILER_MSVC)
	#pragma warning(push)
	#pragma warning(disable: 4296)  // size_t >= 0 checks don't generate warnings
	#pragma warning(disable: 6308)  // 'realloc' might return null pointer: assigning null pointer to 'pPtr', which is passed as an argument to 'realloc', will cause the original memory block to be leaked.
	#pragma warning(disable: 6280)  // 'pPtr' is allocated with 'new []', but deleted with 'realloc'
#endif


// Microsoft's standard library requires that you use 'b' to specify binary read/write of files.
#if defined(EA_COMPILER_MSVC)
	#define kReadBinary  "rb"
	#define kWriteBinary "wb"
#else
	#define kReadBinary  "r"
	#define kWriteBinary "w"
#endif


namespace EA
{
	namespace Allocator
	{
		char*    AR_PtrToString(const void* pValue, char* pBuffer, int nBase, char** ppBufferEnd);
		char*    AR_SizeToString(size_t nValue, char* pBuffer, int nBase, char** ppBufferEnd);
		void     AR_Assert(bool bExpression, char* pMessage);
	}
}


///////////////////////////////////////////////////////////////////////////////
// AR_ASSERT_ENABLED
//
// Defined as 0 or 1. Default is 1 if EA_DEBUG.
//
///////////////////////////////////////////////////////////////////////////////

#ifndef AR_ASSERT_ENABLED
	#ifdef EA_DEBUG
		#define AR_ASSERT_ENABLED 1
	#else
		#define AR_ASSERT_ENABLED 0
	#endif
#endif

#if AR_ASSERT_ENABLED
	#define AR_ASSERT AR_Assert
#else
	#define AR_ASSERT(bExpression, pMessage)
#endif




///////////////////////////////////////////////////////////////////////////////
// CFileIO
///////////////////////////////////////////////////////////////////////////////

namespace EA
{
namespace Allocator
{
namespace Internal
{


class CFileIO : public FileIO
{
public:
	uintptr_t Open(const char* pFilePath, const char* pMode)
	{
		#if PPM_FOPEN_AVAILABLE
			return (uintptr_t)fopen(pFilePath, pMode);
		#else
			EA_UNUSED(pFilePath); EA_UNUSED(pMode);
			return 0;
		#endif
	}

	void Close(uintptr_t pFile)
	{
		#if PPM_FOPEN_AVAILABLE
			fclose((FILE*)pFile);
		#else
			EA_UNUSED(pFile);
		#endif
	}

	size_t Read(void* data, size_t count, uintptr_t pFile)
	{
		#if PPM_FOPEN_AVAILABLE
			return fread(data, 1, count, (FILE*)pFile);
		#else
			EA_UNUSED(data); EA_UNUSED(count); EA_UNUSED(pFile);
			return 0;
		#endif
	}

	size_t Write(const void* data, size_t count, uintptr_t pFile)
	{
		#if PPM_FOPEN_AVAILABLE
			return fwrite (data, 1, count, (FILE*)pFile);
		#else
			EA_UNUSED(data); EA_UNUSED(count); EA_UNUSED(pFile);
			return 0;
		#endif
	}

	int SetPosition(uintptr_t pFile, long position, int relativeTo)
	{
		#if PPM_FOPEN_AVAILABLE
			return fseek((FILE*)pFile, position, relativeTo);
		#else
			EA_UNUSED(pFile); EA_UNUSED(position); EA_UNUSED(relativeTo);
			return 0;
		#endif
	}

	long GetPosition(uintptr_t pFile)
	{
		#if PPM_FOPEN_AVAILABLE
			return ftell((FILE*)pFile);
		#else
			EA_UNUSED(pFile);
			return 0;
		#endif
	}

	char* GetLine(char* buffer, int capacity, uintptr_t pFile)
	{
		#if PPM_FOPEN_AVAILABLE
			return fgets(buffer, capacity, (FILE*)pFile);
		#else
			EA_UNUSED(pFile);
			if(capacity)
				buffer[0] = 0;
			return buffer;
		#endif
	}
};

} // namespace Internal
} // namespace Allocator
} // namespace EA




///////////////////////////////////////////////////////////////////////////////
// AllocationRecorder::HashTable
///////////////////////////////////////////////////////////////////////////////

void EA::Allocator::AllocationRecorder::HashTable::Clear(bool bDestroyTable)
{
	if(mppTable)
	{
		for(size_t i = 0; i < mnTableSize; i++)
		{
			HashPair* p = mppTable[i];

			while(p)
			{
				HashPair* const pNext = p->mpNext;
				p->~HashPair();
				delete p;
				p = pNext;
			}
			mppTable[i] = NULL;
		}

		if(bDestroyTable)
		{
			delete[] mppTable;
			mppTable = NULL;
			mnTableSize = 0;
		}

		mnElementCount = 0;
	}
}


void EA::Allocator::AllocationRecorder::HashTable::Nuke()
{
	mppTable       = NULL;
	mbReHashing    = false;
	mnTableSize    = 0;
	mnElementCount = 0;
}


void EA::Allocator::AllocationRecorder::HashTable::Insert(const void* pKey, const void* pValue)
{
	++mnElementCount;

	if(mnElementCount >= mnTableSize)   // Make it roughly 1:1.
	{
		size_t nTableSizeNew = (mnTableSize * 3 / 2) + 1; // Grow it by about 50%.
		if(nTableSizeNew < mnTableSizeMin)
			nTableSizeNew = mnTableSizeMin;
		ReHash(nTableSizeNew);
	}

	const size_t h = Hash(pKey);
	HashPair* const pHashPair = mppTable[h];
	HashPair* const pHashPairNew = new HashPair(pKey, pValue, pHashPair);
	mppTable[h] = pHashPairNew;
}


bool EA::Allocator::AllocationRecorder::HashTable::Find(const void* pKey, void** ppValue) const
{
	if(mppTable)
	{
		const size_t h = Hash(pKey);

		for(HashPair* p = mppTable[h]; p; p = p->mpNext)
		{
			if(p->mpKey == pKey)
			{
				*ppValue = (void*)p->mpValue;
				return true;
			}
		}
	}
	return false;
}


bool EA::Allocator::AllocationRecorder::HashTable::Remove(const void* pKey)
{
	if(mppTable)
	{
		const size_t h     = Hash(pKey);
		HashPair*    p     = mppTable[h];
		HashPair*    pPrev = NULL;

		while(p)
		{
			HashPair* pNext = p->mpNext;

			if(p->mpKey == pKey)
			{
				if(pPrev)
					pPrev->mpNext = p->mpNext;
				else
					mppTable[h] = p->mpNext;
				p->~HashPair();
				delete p;
				mnElementCount--;
				return true;
			}

			pPrev = p;
			p      = pNext;
		}
	}

	return false;
}


void EA::Allocator::AllocationRecorder::HashTable::ReHash(size_t nTableSizeNew)
{
	if(!mbReHashing) // Re-entrancy prevention
	{
		mbReHashing = true;

		const size_t nTableSizeOld  = mnTableSize;
		HashPair** const ppTableOld = mppTable;

		HashPair** const ppTableNew = (HashPair**)new HashPair*[sizeof(HashPair*) * (size_t)nTableSizeNew];
		memset(ppTableNew, 0, sizeof(HashPair*) * (size_t)nTableSizeNew); // Note that the allocation call above could trigger hash table reads or writes.

		mnTableSize = nTableSizeNew; // It's important that we do these assignments after the new table allocation in order
		mppTable     = ppTableNew;     // to deal  with re-entrancy that can occur during allocation of the new table.

		for(size_t i = 0; i < nTableSizeOld; i++)
		{
			HashPair* p = ppTableOld[i];

			while(p)
			{
				const size_t h = Hash(p->mpKey); // Hash will use the use the new table size.
				HashPair* const pNext = p->mpNext;
				p->mpNext = ppTableNew[h];
				ppTableNew[h] = p;
				p = pNext;
			}
		}

		if(ppTableOld)
			delete[] ppTableOld;

		mbReHashing = false;
	}
}


///////////////////////////////////////////////////////////////////////////////
// AllocationRecorder
///////////////////////////////////////////////////////////////////////////////

EA::Allocator::AllocationRecorder::AllocationRecorder()
  : mMode(kModeNone),
	mPlaybackAllocatorType(kPlaybackAllocatorTypeGeneralDebug),
  //mStreamName
	mStreamHandle(0),
	mpStreamData(NULL),
	mnStreamDataSize(0),
	mpStreamPosition(NULL),
	mpStreamEnd(NULL),
	mnCurrentEventCount(0),
	mnTotalEventCount(0),
	mnEventTime(0),
	mpHookFunction(NULL),
	mpHookFunctionContext(NULL),
	mpFileIO(),
	#if PPM_ENABLED
		mpRecordedAllocator(NULL),
		mpPlaybackAllocator(NULL),
	#endif
	#if PPM_THREAD_SAFETY_SUPPORTED
		mpMutex(NULL),
	  //mpMutexData
	#endif
	mHashTable()
{
	memset(mStreamName, 0, sizeof(mStreamName));
	#if PPM_THREAD_SAFETY_SUPPORTED
		memset(mpMutexData, 0, sizeof(mpMutexData));
	#endif
}

EA::Allocator::AllocationRecorder::~AllocationRecorder()
{
	Shutdown();
}


bool EA::Allocator::AllocationRecorder::InitFile(Mode mode, const char* pStreamName, PlaybackAllocatorType playbackAllocatorType)
{
	using namespace Internal;

	bool    bReturnValue(false);
	CFileIO cFileIO;
	FileIO* pFileIO = mpFileIO ? mpFileIO : &cFileIO;

	mPlaybackAllocatorType = playbackAllocatorType;

	#if PPM_THREAD_SAFETY_SUPPORTED
		if(!mpMutex)
			mpMutex = PPMMutexCreate(mpMutexData);
	#endif

	#if !PPM_ENABLED // If it is impossible to use GeneralAllocator...
		if((mPlaybackAllocatorType == kPlaybackAllocatorTypeGeneral) || 
			(mPlaybackAllocatorType == kPlaybackAllocatorTypeGeneralDebug))
		{
			mPlaybackAllocatorType = kPlaybackAllocatorTypeNew;
		}
	#endif

	if(mMode == kModeNone)
	{
		mMode = mode;

		// Clear the pointer-based stream data.
		mpStreamData     = NULL;
		mnStreamDataSize = 0;
		mpStreamPosition = NULL;
		mpStreamEnd      = NULL;

		// Setup the file-based stream data.
		if(pStreamName)
		{
			strncpy(mStreamName, pStreamName, sizeof(mStreamName) / sizeof(mStreamName[0]));
			mStreamName[(sizeof(mStreamName) / sizeof(mStreamName[0])) - 1] = 0;
		}

		// Open the stream
		if(mMode == kModeRecord)
			mStreamHandle = pFileIO->Open(mStreamName, kWriteBinary);
		else
			mStreamHandle = pFileIO->Open(mStreamName, kReadBinary);

		if(mStreamHandle)
		{
			bReturnValue = true;

			if(mMode == kModeRecord)
				mnTotalEventCount = 0;
			else
			{
				// If the file is huge then we could spend a lot of time reading it just to count the newlines.
				// A simpler solution which may be acceptable is to estimate the size based on the size of N 
				// bytes of file data.
				mnTotalEventCount = (size_t)-1; // Unknown.

				// Get the file size.
				pFileIO->SetPosition(mStreamHandle, 0, SEEK_END);
				size_t fileSize = (size_t)pFileIO->GetPosition(mStreamHandle);
				pFileIO->SetPosition(mStreamHandle, 0, SEEK_SET);

				const size_t kBufferSize = 2048;
				char         buffer[kBufferSize];
				size_t       readSize = PPM_MIN(fileSize, kBufferSize);
				size_t       result = pFileIO->Read(buffer, readSize, mStreamHandle);

				//PPM_ASSERT_STATIC(result == readSize, id, pDescription, pData, pExtra);
				if(result == readSize) // If successful...
				{
					// We simply count the number of newlines (\n) in the data.
					mnTotalEventCount = 0;

					for(const char *p = buffer, *pEnd = p + readSize; p < pEnd; ++p)
					{
						if(*p == '\n')
							mnTotalEventCount++;
					}
				}

				if(fileSize > readSize) // If we merely sampled a part of the file... scale up our guess
				{
					mnTotalEventCount = (size_t)((uint64_t)mnTotalEventCount * fileSize / readSize);
					mnTotalEventCount = mnTotalEventCount * 102 / 100; // Fudge factor.
				}

				pFileIO->SetPosition(mStreamHandle, 0, SEEK_SET);
			}

			// Set up the allocator.
			if(mMode == kModePlayback)
			{
				#if PPM_ENABLED
					if(!mpPlaybackAllocator && (mPlaybackAllocatorType == kPlaybackAllocatorTypeGeneral))
						mpPlaybackAllocator = &mGeneralAllocator;
					else if(!mpPlaybackAllocator && (mPlaybackAllocatorType == kPlaybackAllocatorTypeGeneralDebug))
						mpPlaybackAllocator = &mGeneralAllocatorDebug;
				#endif
			}
		}
	}

	return bReturnValue;
}


bool EA::Allocator::AllocationRecorder::InitData(Mode mode, const char* pData, size_t dataLength, PlaybackAllocatorType playbackAllocatorType)
{
	bool bReturnValue(false);

	mPlaybackAllocatorType = playbackAllocatorType;

	#if PPM_THREAD_SAFETY_SUPPORTED
		if(!mpMutex)
			mpMutex = PPMMutexCreate(mpMutexData);
	#endif

	#if !PPM_ENABLED // If it is impossible to use GeneralAllocator...
		if((mPlaybackAllocatorType == kPlaybackAllocatorTypeGeneral) || 
			(mPlaybackAllocatorType == kPlaybackAllocatorTypeGeneralDebug))
		{
			mPlaybackAllocatorType = kPlaybackAllocatorTypeNew;
		}
	#endif

	if(mMode == kModeNone)
	{
		bReturnValue = true;

		mMode = mode;

		// Clear the file-based stream data.
		mStreamName[0]    = 0;

		// Setup the pointer-based stream data.
		mpStreamData      = pData;
		mnStreamDataSize  = dataLength;
		mpStreamPosition  = pData;
		mpStreamEnd       = pData + dataLength;

		if(mMode == kModePlayback)
		{
			// Calculate mnTotalEventCount.
			mnTotalEventCount = 0;
			for(const char* p = pData; p != pData + dataLength; ++p)
			{
				if(*p == '\n')
					mnTotalEventCount++;
			}

			#if PPM_ENABLED
				if(!mpPlaybackAllocator && (mPlaybackAllocatorType == kPlaybackAllocatorTypeGeneral))
					mpPlaybackAllocator = &mGeneralAllocator;
				else if(!mpPlaybackAllocator && (mPlaybackAllocatorType == kPlaybackAllocatorTypeGeneralDebug))
					mpPlaybackAllocator = &mGeneralAllocatorDebug;
			#endif
		}
	}

	return bReturnValue;
}


char* EA::Allocator::AllocationRecorder::ReadFileIntoData(const char* pStreamName, size_t* pFileSize, size_t* pEventCount, FileIO* pFileIO)
{
	using namespace Internal;

	char*   pReturnValue = NULL;
	CFileIO cFileIO;

	if(!pFileIO)
		pFileIO = &cFileIO;

	uintptr_t streamHandle = pFileIO->Open(pStreamName, kReadBinary);

	if(streamHandle)
	{
		const int nResult = pFileIO->SetPosition(streamHandle, 0, SEEK_END);

		if(nResult == 0) // If fseek was successful.
		{
			const long nFileSize = pFileIO->GetPosition(streamHandle);

			if(nFileSize >= 0)
			{
				if(pFileSize)
					*pFileSize = (size_t)nFileSize;

				pReturnValue = new char[(size_t)nFileSize];

				if(pReturnValue)
				{
					pFileIO->SetPosition(streamHandle, 0, SEEK_SET);
					size_t result = pFileIO->Read(pReturnValue, (size_t)nFileSize, streamHandle); // To do: Error-check this.

					if(pEventCount && (result > 0))
					{
						// We simply count the number of newlines (\n) in the data.
						for(const char *pCurrent = pReturnValue, *pEnd = pCurrent + (size_t)nFileSize; pCurrent < pEnd; ++pCurrent)
						{
							if(*pCurrent == '\n')
								++*pEventCount;
						}
					}
				}
			}
		}

		pFileIO->Close(streamHandle);
	}

	return pReturnValue;
}



#if defined(EA_COMPILER_MSVC)
   #pragma warning(push)
   #pragma warning(disable: 4127)  // conditional expression is constant
#endif

bool EA::Allocator::AllocationRecorder::Shutdown()
{
	using namespace Internal;

	CFileIO cFileIO;
	FileIO* pFileIO = mpFileIO ? mpFileIO : &cFileIO;

	#if PPM_ENABLED
		if(mpRecordedAllocator)
		{
			RecordAllocator(mpRecordedAllocator, false);
			mpRecordedAllocator = NULL;
		}

		if(mpPlaybackAllocator)
		{
			mpPlaybackAllocator->Shutdown();
			mpPlaybackAllocator = NULL;
		}
	#endif

	#if PPM_THREAD_SAFETY_SUPPORTED
		if(mpMutex)
		{
			PPMMutexDestroy(mpMutex);
			mpMutex = NULL;
		}
	#endif

	// Below is code that plays back frees that should have happened 
	// but didn't. This useful for fixing memory leaks in recordings.
	// This code reads private data of the hash table, but we don't 
	// have an iterator mechanism and we are restricted to not using
	// C++ STL containers in this module, due to shared library issues.
	for(size_t i = 0; i < mHashTable.mnTableSize; i++)
	{
		HashTable::HashPair* p = mHashTable.mppTable[i];

		while(p)
		{
			char buffer[32];

			if(sizeof(void*) == 4) //lint !e774 !e506 (Boolean within 'if' always evaluates to true; Constant value Boolean)
				sprintf(buffer, "f %08x\n", (unsigned)(uintptr_t)p->mpKey);
			else
				sprintf(buffer, "f %016" PRIx64 "\n", (uint64_t)(uintptr_t)p->mpKey);

			#if defined(EA_PLATFORM_MICROSOFT)
				#if PPM_DEBUG || !defined(EA_PLATFORM_WINDOWS_PHONE)    // Explicitly disable this in Windows phone non-debug builds, 
					OutputDebugStringA(buffer);                         // as it is a TRC violation to link to OutputDebugStringA.
				#endif
			#elif defined(EA_PLATFORM_ANDROID)
				__android_log_write(ANDROID_LOG_INFO, "PPMalloc", buffer);
			#else
				printf("%s", buffer);
			#endif

			p = p->mpNext;
		}
	}

	// Clear the file-based stream data.
	if(mStreamHandle)
	{
		pFileIO->Close(mStreamHandle);
		mStreamHandle = 0;
	}

	// Clear the data-based stream data.
	mpStreamData         = NULL;
	mnStreamDataSize     = 0;
	mpStreamPosition     = NULL;
	mpStreamEnd          = NULL;

	mnCurrentEventCount  = 0;
	mnTotalEventCount    = 0;
	mnEventTime          = 0;
	mMode                = kModeNone;
	mHashTable.Clear(true);

	return true;
}

#if defined(EA_COMPILER_MSVC)
   #pragma warning(pop)
#endif


size_t EA::Allocator::AllocationRecorder::GetCurrentEventCount() const
{
	return mnCurrentEventCount;
}


size_t EA::Allocator::AllocationRecorder::GetTotalEventCount() const
{
	if(mMode == kModePlayback)
		return mnTotalEventCount;
	return mnCurrentEventCount;
}


void EA::Allocator::AllocationRecorder::RecordMalloc(const void* pResult, size_t nSize, size_t nAlignment, size_t nOffset, int nAllocationFlags, char* pBuffer)
{
	using namespace Internal;

	CFileIO cFileIO;
	FileIO* pFileIO = mpFileIO ? mpFileIO : &cFileIO;

	if(((mMode == kModeRecord) && mStreamHandle) || pBuffer)
	{
		char  buffer[128]; buffer[0] = 0;
		char* pCurrent = buffer;
		const uint64_t nCurrentTime = GetPPMallocDebugTimeUs();

		// The code below could be made more compact, but doing so would make it a bit harder
		// for the new reader to quickly tell what's going on. That's why it's the way it is.
		if(nAlignment == 0)
		{
			// Write a string like this: "m c23a17bc 123\n"
			*pCurrent++ = 'm';
			*pCurrent++ = ' ';
			 AR_PtrToString(pResult, pCurrent, 16, &pCurrent);
			*pCurrent++ = ' ';
			 AR_SizeToString(nSize, pCurrent, 10, &pCurrent);
		}
		else
		{
			if(nOffset == 0)
			{
				// Write a string like this: "a c23a17bc 123 16\n"
				*pCurrent++ = 'a';
				*pCurrent++ = ' ';
				AR_PtrToString(pResult, pCurrent, 16, &pCurrent);
				*pCurrent++ = ' ';
				AR_SizeToString(nSize, pCurrent, 10, &pCurrent);
				*pCurrent++ = ' ';
				AR_SizeToString(nAlignment, pCurrent, 10, &pCurrent);
			}
			else
			{
				// Write a string like this: "o c23a17bc 123 16 8\n"
				*pCurrent++ = 'o';
				*pCurrent++ = ' ';
				AR_PtrToString(pResult, pCurrent, 16, &pCurrent);
				*pCurrent++ = ' ';
				AR_SizeToString(nSize, pCurrent, 10, &pCurrent);
				*pCurrent++ = ' ';
				AR_SizeToString(nAlignment, pCurrent, 10, &pCurrent);
				*pCurrent++ = ' ';
				AR_SizeToString(nOffset, pCurrent, 10, &pCurrent);
			}
		}

		if(nAllocationFlags)
		{
			*pCurrent++ = ' ';
			 AR_SizeToString((size_t)nAllocationFlags, pCurrent, 16, &pCurrent);
		}
		*pCurrent++ = '\n';

		if(pBuffer)
			strcpy(pBuffer, buffer);
		if(mStreamHandle)
		{
			#if PPM_THREAD_SAFETY_SUPPORTED
				PPMMutexLock(mpMutex);
			#endif

			pFileIO->Write(buffer, (size_t)(pCurrent - buffer), mStreamHandle);

			#if PPM_THREAD_SAFETY_SUPPORTED
				PPMMutexUnlock(mpMutex);
			#endif
		}

		mnCurrentEventCount++;
		mnEventTime = nCurrentTime;
	}
}


void EA::Allocator::AllocationRecorder::RecordRealloc(const void* pResult, const void* pData, size_t nSize, int nAllocationFlags, char* pBuffer)
{
	using namespace Internal;

	CFileIO cFileIO;
	FileIO* pFileIO = mpFileIO ? mpFileIO : &cFileIO;

	if(((mMode == kModeRecord) && mStreamHandle) || pBuffer)
	{
		char  buffer[128]; buffer[0] = 0;
		char* pCurrent = buffer;
		const uint64_t nCurrentTime = GetPPMallocDebugTimeUs();

		// Write a string like this: "r c23a17bc 18cb34a2 28\n"
		*pCurrent++ = 'r';
		*pCurrent++ = ' ';
		 AR_PtrToString(pResult, pCurrent, 16, &pCurrent);
		*pCurrent++ = ' ';
		 AR_PtrToString(pData, pCurrent, 16, &pCurrent);
		*pCurrent++ = ' ';
		 AR_SizeToString(nSize, pCurrent, 10, &pCurrent);
		 if(nAllocationFlags)
		 {
			*pCurrent++ = ' ';
			 AR_SizeToString((size_t)nAllocationFlags, pCurrent, 16, &pCurrent);
		 }
		*pCurrent++ = '\n';

		if(pBuffer)
			strcpy(pBuffer, buffer);
		if(mStreamHandle)
		{
			#if PPM_THREAD_SAFETY_SUPPORTED
				PPMMutexLock(mpMutex);
			#endif

			pFileIO->Write(buffer, (size_t)(pCurrent - buffer), mStreamHandle);

			#if PPM_THREAD_SAFETY_SUPPORTED
				PPMMutexUnlock(mpMutex);
			#endif
		}

		mnCurrentEventCount++;
		mnEventTime = nCurrentTime;
	}
}


void EA::Allocator::AllocationRecorder::RecordCalloc(const void* pResult, size_t nElementCount, size_t nElementSize, int nAllocationFlags, char* pBuffer)
{
	using namespace Internal;

	CFileIO cFileIO;
	FileIO* pFileIO = mpFileIO ? mpFileIO : &cFileIO;

	if(((mMode == kModeRecord) && mStreamHandle) || pBuffer)
	{
		char  buffer[128]; buffer[0] = 0;
		char* pCurrent = buffer;
		const uint64_t nCurrentTime = GetPPMallocDebugTimeUs();

		// Write a string like this: "c c23a17bc 28 2\n"
		*pCurrent++ = 'c';
		*pCurrent++ = ' ';
		 AR_PtrToString(pResult, pCurrent, 16, &pCurrent);
		*pCurrent++ = ' ';
		 AR_SizeToString(nElementCount, pCurrent, 10, &pCurrent);
		*pCurrent++ = ' ';
		 AR_SizeToString(nElementSize, pCurrent, 10, &pCurrent);
		 if(nAllocationFlags)
		 {
			*pCurrent++ = ' ';
			 AR_SizeToString((size_t)nAllocationFlags, pCurrent, 16, &pCurrent);
		 }
		*pCurrent++ = '\n';

		if(pBuffer)
			strcpy(pBuffer, buffer);
		if(mStreamHandle)
		{
			#if PPM_THREAD_SAFETY_SUPPORTED
				PPMMutexLock(mpMutex);
			#endif

			pFileIO->Write(buffer, (size_t)(pCurrent - buffer), mStreamHandle);

			#if PPM_THREAD_SAFETY_SUPPORTED
				PPMMutexUnlock(mpMutex);
			#endif
		}

		mnCurrentEventCount++;
		mnEventTime = nCurrentTime;
	}
}


void EA::Allocator::AllocationRecorder::RecordFree(const void* pData, char* pBuffer)
{
	using namespace Internal;

	CFileIO cFileIO;
	FileIO* pFileIO = mpFileIO ? mpFileIO : &cFileIO;

	if(((mMode == kModeRecord) && mStreamHandle) || pBuffer)
	{
		char  buffer[128]; buffer[0] = 0;
		char* pCurrent = buffer;
		const uint64_t nCurrentTime = GetPPMallocDebugTimeUs();

		// Write a string like this: "f c23a17bc\n"
		*pCurrent++ = 'f';
		*pCurrent++ = ' ';
		 AR_PtrToString(pData, pCurrent, 16, &pCurrent);
		*pCurrent++ = '\n';

		if(pBuffer)
			strcpy(pBuffer, buffer);
		if(mStreamHandle)
		{
			#if PPM_THREAD_SAFETY_SUPPORTED
				PPMMutexLock(mpMutex);
			#endif

			pFileIO->Write(buffer, (size_t)(pCurrent - buffer), mStreamHandle);

			#if PPM_THREAD_SAFETY_SUPPORTED
				PPMMutexUnlock(mpMutex);
			#endif
		}

		mnCurrentEventCount++;
		mnEventTime = nCurrentTime;
	}
}


#if PPM_ENABLED
	void EA::Allocator::AllocationRecorder::RecordAllocator(EA::Allocator::GeneralAllocator* pAllocator, bool bEnable)
	{
		if(pAllocator)
		{
			if(bEnable)
			{
				if(!mpRecordedAllocator)
				{
					mpRecordedAllocator = pAllocator;
					mpRecordedAllocator->SetHookFunction(HookFunction, static_cast<AllocationRecorder*>(this));
				}
			}
			else
			{
				if(pAllocator == mpRecordedAllocator)
				{
					mpRecordedAllocator->SetHookFunction(NULL, NULL);
					mpRecordedAllocator = NULL;
				}
			}
		}
	}


	void EA::Allocator::AllocationRecorder::HookFunction(const GeneralAllocator::HookInfo* pHookInfo, void* pContext)
	{
		AllocationRecorder* const pAllocationRecorder = reinterpret_cast<AllocationRecorder*>(pContext);

		if(pAllocationRecorder)
		{
			if(!pHookInfo->mbEntry) // If we are being called at the end of the function (so we can see the result)...
			{
				if(pHookInfo->mHookType == GeneralAllocator::kHookTypeMalloc)
				{
					switch (pHookInfo->mHookSubType)
					{
						default:
						case GeneralAllocator::kHookSubTypeMalloc:
							pAllocationRecorder->RecordMalloc(pHookInfo->mpDataOutput, pHookInfo->mnSizeInputTotal, 0, 0, pHookInfo->mnAllocationFlags);
							break;

						case GeneralAllocator::kHookSubTypeMallocAligned:
							pAllocationRecorder->RecordMalloc(pHookInfo->mpDataOutput, pHookInfo->mnSizeInputTotal, pHookInfo->mnAlignmentInput, pHookInfo->mnSizeOutput, pHookInfo->mnAllocationFlags);
							break;

						case GeneralAllocator::kHookSubTypeCalloc:
							pAllocationRecorder->RecordCalloc(pHookInfo->mpDataOutput, pHookInfo->mnCountInput, pHookInfo->mnSizeInput, pHookInfo->mnAllocationFlags);
							break;

						case GeneralAllocator::kHookSubTypeRealloc:
							pAllocationRecorder->RecordRealloc(pHookInfo->mpDataOutput, (void*)pHookInfo->mpDataInput, pHookInfo->mnSizeInputTotal, pHookInfo->mnAllocationFlags);
							break;

						// Must handle these or some compilers will complain.
						case GeneralAllocator::kHookSubTypeMallocMultiple1:
						case GeneralAllocator::kHookSubTypeMallocMultiple2:
						case GeneralAllocator::kHookSubTypeNone:
						   break;
					}
				}
				else if(pHookInfo->mHookType == GeneralAllocator::kHookTypeFree)
					pAllocationRecorder->RecordFree(pHookInfo->mpDataInput);
			}
		}
	}


	EA::Allocator::GeneralAllocator* EA::Allocator::AllocationRecorder::GetPlaybackAllocator()
	{
		if(!mpPlaybackAllocator)
		{
			#if (defined(EA_DEBUG) || defined(_DEBUG))
				mpPlaybackAllocator = new GeneralAllocatorDebug;
			#else
				mpPlaybackAllocator = new GeneralAllocator;
			#endif
		}
		return mpPlaybackAllocator;
	}

#endif // PPM_ENABLED



//////////////////////////////////////////////////////////////////////////
// SetPlaybackHook
//
void EA::Allocator::AllocationRecorder::SetPlaybackHook(HookFunctionType pHookFunction, void* pHookFunctionContext)
{
	mpHookFunction          = pHookFunction;
	mpHookFunctionContext = pHookFunctionContext;
}



//////////////////////////////////////////////////////////////////////////
// Playback
//
// Plays back all allocation steps.
//
void EA::Allocator::AllocationRecorder::Playback(size_t nStepCount)
{
	while((nStepCount > 0) && PlaybackStep())
		--nStepCount;
}


//////////////////////////////////////////////////////////////////////////
// PlaybackStep
//
// Plays back a single allocation step from mpStreamPosition.
//
// We have strings in one of the following forms:
//     "; <arbitrary comment text>\n"
//     "\n"
//     "m c23a17bc 123\n"
//     "a c23a17bc 123 16\n"
//     "o c23a17bc 123 16 8\n"
//     "r c23a17bc 18cb34a2 28\n"
//     "c c23a17bc 28 2\n"
//     "f c23a17bc\n"
//
bool EA::Allocator::AllocationRecorder::PlaybackStep()
{
	using namespace Internal;

	bool    bReturnValue(false);
	CFileIO cFileIO;
	FileIO* pFileIO = mpFileIO ? mpFileIO : &cFileIO;

	if(mMode == kModePlayback)
	{
		char lineBuffer[256];

		if(mStreamHandle) // If playing back from a file (as oppposed to from memory)...
		{
			bReturnValue = true;

			char* pLine = lineBuffer;

			// Run the loop while until there was an action, while there isn't an error, and while there are more lines to process.
			for(char cAction = 0; (cAction == 0) && bReturnValue && pLine; )
			{
				pLine = pFileIO->GetLine(lineBuffer, (int)(unsigned)sizeof(lineBuffer), mStreamHandle);

				if(pLine)
					bReturnValue = PlaybackStep(pLine, &cAction);
				else
					bReturnValue = false;
			}
		}
		else
		{
			bReturnValue = (mpStreamPosition < mpStreamEnd);

			if(bReturnValue)
			{
				// Run the loop while until there was an action, while there isn't an error, and while there are more lines to process.
				for(char cAction = 0; (cAction == 0) && bReturnValue && (mpStreamPosition < mpStreamEnd); )
				{
					const char* pLineEnd;

					// Read to one-past the next newline.
					for(pLineEnd = mpStreamPosition; (pLineEnd < mpStreamEnd) && (*pLineEnd >= 32); pLineEnd++)
						{ } // Empty; do nothing

					while((&pLineEnd[1] < mpStreamEnd) && (pLineEnd[1] < 32)) // While additional carriage returns and/or line feeds are found...
						pLineEnd++;

					memcpy(lineBuffer, mpStreamPosition, (size_t)(pLineEnd - mpStreamPosition));
					lineBuffer[(size_t)(pLineEnd - mpStreamPosition)] = 0;

					bReturnValue = PlaybackStep(lineBuffer, &cAction);

					mpStreamPosition = pLineEnd + 1;
				}
			}
		}
	}

	return bReturnValue;
}



//////////////////////////////////////////////////////////////////////////
// PlaybackStep
//
bool EA::Allocator::AllocationRecorder::PlaybackStep(const char* pLine, char* pActionResult)
{
	bool        bReturnValue(false);
	const char* p(pLine);
	const char  c(*p++);
	void*       pResult;
	unsigned    nSize;
	unsigned    nAlignment;
	unsigned    nAlignmentOffset;
	int         nAllocationFlags = 0;    // Cleared because it may or may not be assigned during the line reading.
	void*       pData;
	unsigned    nElementCount;
	int         nScanfResult = 0;        // Cleared because it controls the loop processing as well as the line reading.
	char        errorBuffer[128];

	switch(c)
	{
		// We use sscanf below to read values. However, at least one version of sscanf 
		// calls strlen on the source string. For large allocation recordings this 
		// results in astronomical amounts of time spent in strlen. To deal with this,    
		// we put a NULL in the newline position to deal with this.

		case 'm': // Malloc.
		{
			if(pActionResult)
				*pActionResult = c;

			nScanfResult = sscanf(p, " %p %u %d", &pResult, &nSize, &nAllocationFlags);

			if((nScanfResult >= 2) && (nScanfResult <= 3))
			{
				// Regardless of the success of the malloc call below, we consider the processing of this line to be a success.
				bReturnValue = true;

				#if PPM_ENABLED
					if((mPlaybackAllocatorType == kPlaybackAllocatorTypeGeneral) || (mPlaybackAllocatorType == kPlaybackAllocatorTypeGeneralDebug))
						pData = mpPlaybackAllocator->Malloc(nSize, nAllocationFlags);
					else
				#endif
				if(mPlaybackAllocatorType == kPlaybackAllocatorTypeMalloc)
					pData = malloc(nSize);
				else if(mPlaybackAllocatorType == kPlaybackAllocatorTypeNew)
					pData = new char[nSize];
				else if(mPlaybackAllocatorType == kPlaybackAllocatorTypeCustom)
				{
					pData = NULL;
					if(mpHookFunction)
					{
						HookInfo hookInfo;
						memset(&hookInfo, 0, sizeof(hookInfo));
						hookInfo.mHookType         = kHookTypeMalloc;
						hookInfo.mnSizeInputTotal  = nSize;
						hookInfo.mnSizeInput       = nSize;
						hookInfo.mnAllocationFlags = nAllocationFlags;
						mpHookFunction(&hookInfo, mpHookFunctionContext);
						pData = hookInfo.mpDataOutput;
					}
				}
				else // kPlaybackAllocatorTypeNULL
					pData = (void*)(mnCurrentEventCount + 1);

				if(pData)
				{
					if(mPlaybackAllocatorType != kPlaybackAllocatorTypeNULL)
						memset(pData, 0xee, nSize);
					mHashTable.Insert(pResult, pData);
				}
				else
				{
					sprintf(errorBuffer, "Failure of Malloc(%u) (event #%u).", (unsigned)nSize, (unsigned)mnCurrentEventCount);
					AR_ASSERT(false, errorBuffer);
				}
			}
			else
			{
				sprintf(errorBuffer, "Bad record (event #%u): %s.", (unsigned)mnCurrentEventCount, p - 1);
				AR_ASSERT(false, errorBuffer);
			}
			break;
		}

		case 'a': // MallocAligned without a specified alignment offset.
		{
			if(pActionResult)
				*pActionResult = c;

			nScanfResult = sscanf(p, " %p %u %u %d", &pResult, &nSize, &nAlignment, &nAllocationFlags);

			if((nScanfResult >= 3) && (nScanfResult <= 4))
			{
				// Regardless of the success of the malloc call below, we consider the processing of this line to be a success.
				bReturnValue = true;

				#if PPM_ENABLED
					if((mPlaybackAllocatorType == kPlaybackAllocatorTypeGeneral) || (mPlaybackAllocatorType == kPlaybackAllocatorTypeGeneralDebug))
						pData = mpPlaybackAllocator->MallocAligned(nSize, nAlignment, 0, nAllocationFlags);
					else
				#endif
				if(mPlaybackAllocatorType == kPlaybackAllocatorTypeMalloc)
					pData = malloc(nSize);    // There is no aligned allocation in C or C++.
				else if(mPlaybackAllocatorType == kPlaybackAllocatorTypeNew)
					pData = new char[nSize];
				else if(mPlaybackAllocatorType == kPlaybackAllocatorTypeCustom)
				{
					pData = NULL;
					if(mpHookFunction)
					{
						HookInfo hookInfo;
						memset(&hookInfo, 0, sizeof(hookInfo));
						hookInfo.mHookType         = kHookTypeMallocAligned,
						hookInfo.mnSizeInputTotal  = nSize;
						hookInfo.mnSizeInput       = nSize;
						hookInfo.mnAlignmentInput  = nAlignment;
						hookInfo.mnAllocationFlags = nAllocationFlags;
						mpHookFunction(&hookInfo, mpHookFunctionContext);
						pData = hookInfo.mpDataOutput;
					}
				}
				else // kPlaybackAllocatorTypeNULL
					pData = (void*)(mnCurrentEventCount + 1);

				if(pData)
				{
					if(mPlaybackAllocatorType != kPlaybackAllocatorTypeNULL)
						memset(pData, 0xee, nSize);
					mHashTable.Insert(pResult, pData);
				}
				else
				{
					sprintf(errorBuffer, "Failure of MallocAligned(%u, %u) (event #%u).", (unsigned)nSize, (unsigned)nAlignment, (unsigned)mnCurrentEventCount);
					AR_ASSERT(false, errorBuffer);
				}
			}
			else
			{
				sprintf(errorBuffer, "Bad record (event #%u): %s.", (unsigned)mnCurrentEventCount, p - 1);
				AR_ASSERT(false, errorBuffer);
			}
			break;
		}

		case 'o': // MallocAligned with a specified alignment offset.
		{
			if(pActionResult)
				*pActionResult = c;

			nScanfResult = sscanf(p, " %p %u %u %u %d", &pResult, &nSize, &nAlignment, &nAlignmentOffset, &nAllocationFlags);

			if((nScanfResult >= 4) && (nScanfResult <= 5))
			{
				// Regardless of the success of the malloc call below, we consider the processing of this line to be a success.
				bReturnValue = true;

				#if PPM_ENABLED
					if((mPlaybackAllocatorType == kPlaybackAllocatorTypeGeneral) || (mPlaybackAllocatorType == kPlaybackAllocatorTypeGeneralDebug))
						pData = mpPlaybackAllocator->MallocAligned(nSize, nAlignment, nAlignmentOffset, nAllocationFlags);
					else
				#endif
				if(mPlaybackAllocatorType == kPlaybackAllocatorTypeMalloc)
					pData = malloc(nSize); // There is no aligned allocation in C or C++.
				else if(mPlaybackAllocatorType == kPlaybackAllocatorTypeNew)
					pData = new char[nSize];
				else if(mPlaybackAllocatorType == kPlaybackAllocatorTypeCustom)
				{
					pData = NULL;
					if(mpHookFunction)
					{
						HookInfo hookInfo;
						memset(&hookInfo, 0, sizeof(hookInfo));
						hookInfo.mHookType              = kHookTypeMallocAligned,
						hookInfo.mnSizeInputTotal       = nSize;
						hookInfo.mnSizeInput            = nSize;
						hookInfo.mnAlignmentInput       = nAlignment;
						hookInfo.mnAlignmentOffsetInput = nAlignmentOffset;
						hookInfo.mnAllocationFlags      = nAllocationFlags;
						mpHookFunction(&hookInfo, mpHookFunctionContext);
						pData = hookInfo.mpDataOutput;
					}
				}
				else // kPlaybackAllocatorTypeNULL
					pData = (void*)(mnCurrentEventCount + 1);

				if(pData)
				{
					if(mPlaybackAllocatorType != kPlaybackAllocatorTypeNULL)
						memset(pData, 0xee, nSize);
					mHashTable.Insert(pResult, pData);
				}
				else
				{
					sprintf(errorBuffer, "Failure of MallocAligned(%u, %u, %u) (event #%u).", (unsigned)nSize, (unsigned)nAlignment, (unsigned)nAlignmentOffset, (unsigned)mnCurrentEventCount);
					AR_ASSERT(false, errorBuffer);
				}
			}
			else
			{
				sprintf(errorBuffer, "Bad record (event #%u): %s.", (unsigned)mnCurrentEventCount, p - 1);
				AR_ASSERT(false, errorBuffer);
			}
			break;
		}

		case 'c': // Calloc.
		{
			if(pActionResult)
				*pActionResult = c;

			nScanfResult = sscanf(p, " %p %u %u %d", &pResult, &nElementCount, &nSize, &nAllocationFlags);

			if((nScanfResult >= 3) && (nScanfResult <= 4))
			{
				// Regardless of the success of the malloc call below, we consider the processing of this line to be a success.
				bReturnValue = true;

				#if PPM_ENABLED
					if((mPlaybackAllocatorType == kPlaybackAllocatorTypeGeneral) || (mPlaybackAllocatorType == kPlaybackAllocatorTypeGeneralDebug))
						pData = mpPlaybackAllocator->Calloc(nElementCount, nSize, nAllocationFlags);
					else
				#endif
				if(mPlaybackAllocatorType == kPlaybackAllocatorTypeMalloc)
					pData = calloc(nElementCount, nSize);
				else if(mPlaybackAllocatorType == kPlaybackAllocatorTypeNew)
				{
					pData = new char[nElementCount * (size_t)nSize]; //lint !e671 (Possibly passing a negative value)
					// We could clear the memory, but that won't be necessary due to the fill below.
				}
				else if(mPlaybackAllocatorType == kPlaybackAllocatorTypeCustom)
				{
					pData = NULL;
					if(mpHookFunction)
					{
						HookInfo hookInfo;
						memset(&hookInfo, 0, sizeof(hookInfo));
						hookInfo.mHookType         = kHookTypeCalloc,
						hookInfo.mnSizeInputTotal  = nElementCount * nSize;
						hookInfo.mnCountInput      = nElementCount;
						hookInfo.mnSizeInput       = nSize;
						hookInfo.mnAllocationFlags = nAllocationFlags;
						mpHookFunction(&hookInfo, mpHookFunctionContext);
						pData = hookInfo.mpDataOutput;
					}
				}
				else // kPlaybackAllocatorTypeNULL
					pData = (void*)(mnCurrentEventCount + 1);

				if(pData)
				{
					if(mPlaybackAllocatorType != kPlaybackAllocatorTypeNULL)
						memset(pData, 0xee, nElementCount * (size_t)nSize); //lint !e671 (Possibly passing a negative value)
					mHashTable.Insert(pResult, pData);
				}
				else
				{
					sprintf(errorBuffer, "Failure of Calloc(%u) (event #%u).", (unsigned)nSize, (unsigned)mnCurrentEventCount);
					AR_ASSERT(false, errorBuffer);
				}
			}
			else
			{
				sprintf(errorBuffer, "Bad record (event #%u): %s", (unsigned)mnCurrentEventCount, p - 1);
				AR_ASSERT(false, errorBuffer);
			}
			break;
		}

		case 'r': // Realloc.
		{
			if(pActionResult)
				*pActionResult = c;

			nScanfResult = sscanf(p, " %p %p %u %d", &pResult, &pData, &nSize, &nAllocationFlags);

			if((nScanfResult >= 3) && (nScanfResult <= 4))
			{
				// Regardless of the success of the malloc call below, we consider the processing of this line to be a success.
				bReturnValue = true;

				void* pPtr = NULL;

				if(!pData || mHashTable.Find(pData, &pPtr)) // Note that Realloc allows pData to be NULL.
				{
					if(pData)
						mHashTable.Remove(pData);
					else
						pPtr = NULL;

					#if PPM_ENABLED
						if((mPlaybackAllocatorType == kPlaybackAllocatorTypeGeneral) || (mPlaybackAllocatorType == kPlaybackAllocatorTypeGeneralDebug))
							pPtr = mpPlaybackAllocator->Realloc(pPtr, nSize, nAllocationFlags);
						else
					#endif
					if(mPlaybackAllocatorType == kPlaybackAllocatorTypeMalloc)
						pPtr = realloc(pPtr, nSize); // We assume realloc won't fail and thus blindly assign the return value to pPtr.
					else if(mPlaybackAllocatorType == kPlaybackAllocatorTypeNew)
					{
						const char* const pDataOld = (const char*)pPtr;
						pPtr = new char[nSize];
						// We could memcpy the old data to new, but that won't be necessary due to the fill below.
						delete[] pDataOld;
					}
					else if(mPlaybackAllocatorType == kPlaybackAllocatorTypeCustom)
					{
						if(mpHookFunction)
						{
							HookInfo hookInfo;
							memset(&hookInfo, 0, sizeof(hookInfo));
							hookInfo.mHookType         = kHookTypeRealloc,
							hookInfo.mpDataInput       = pPtr;
							hookInfo.mnSizeInputTotal  = nSize;
							hookInfo.mnSizeInput       = nSize;
							hookInfo.mnAllocationFlags = nAllocationFlags;
							mpHookFunction(&hookInfo, mpHookFunctionContext);
							pPtr = hookInfo.mpDataOutput;
						}
						else
							pPtr = NULL;
					}
					else // kPlaybackAllocatorTypeNULL
						pPtr = (void*)(mnCurrentEventCount + 1);

					if(pPtr)
					{
						if(mPlaybackAllocatorType != kPlaybackAllocatorTypeNULL)
							memset(pPtr, 0xee, nSize);
						mHashTable.Insert(pResult, pPtr);
					}
					else
					{
						sprintf(errorBuffer, "Failure of Realloc(0x%p, %u) (event #%u).", pPtr, (unsigned)nSize, (unsigned)mnCurrentEventCount);
						AR_ASSERT(false, errorBuffer);
					}
				}
				else
				{
					sprintf(errorBuffer, "Bad record (event #%u): %s", (unsigned)mnCurrentEventCount, p - 1);
					AR_ASSERT(false, errorBuffer);
				}
			}
			break;
		}

		case 'f': // Free.
		{
			if(pActionResult)
				*pActionResult = c;

			nScanfResult = sscanf(p, " %p", &pData);

			if(nScanfResult == 1)
			{
				// Regardless of the success of the malloc call below, we consider the processing of this line to be a success.
				bReturnValue = true;

				if(pData)
				{
					void* pPtr = NULL;

					if(mHashTable.Find(pData, &pPtr))
					{
						mHashTable.Remove(pData);

						#if PPM_ENABLED
							if((mPlaybackAllocatorType == kPlaybackAllocatorTypeGeneral) || (mPlaybackAllocatorType == kPlaybackAllocatorTypeGeneralDebug))
								mpPlaybackAllocator->Free(pPtr);
							else
						#endif
						if(mPlaybackAllocatorType == kPlaybackAllocatorTypeMalloc)
							free(pPtr);
						else if(mPlaybackAllocatorType == kPlaybackAllocatorTypeNew)
							delete[] (const char*)pPtr;
						else if(mPlaybackAllocatorType == kPlaybackAllocatorTypeCustom)
						{
							if(mpHookFunction)
							{
								HookInfo hookInfo;
								memset(&hookInfo, 0, sizeof(hookInfo));
								hookInfo.mHookType   = kHookTypeFree,
								hookInfo.mpDataInput = pPtr;
								mpHookFunction(&hookInfo, mpHookFunctionContext);
							}
						}
						// else do nothing for kPlaybackAllocatorTypeNULL
					}
					else
					{
						sprintf(errorBuffer, "Pointer not found for Free: 0x%p (event #%u).", pPtr, (unsigned)mnCurrentEventCount);
						AR_ASSERT(false, errorBuffer);
					}
				}
			}
			else
			{
				sprintf(errorBuffer, "Bad record (event #%u): %s", (unsigned)mnCurrentEventCount, p - 1);
				AR_ASSERT(false, errorBuffer);
			}
			break;
		}

		case ';':
		case '\0':
		{
			if(pActionResult)
				*pActionResult = 0;

			// Lines that begin with semicolons are considered comment lines and are considered successful no-ops.
			// Lines that are empty are considered successful no-ops.
			bReturnValue = true;
			break;
		}

		default:
		{
			if(pActionResult)
				*pActionResult = 0;

			sprintf(errorBuffer, "Bad record (event #%u): %s", (unsigned)mnCurrentEventCount, p - 1);
			AR_ASSERT(false, errorBuffer);

			break;
		}

	} // switch

	if(nScanfResult > 0)
	{
		mnCurrentEventCount++;
		mnEventTime = GetPPMallocDebugTimeUs();
	}

	return bReturnValue;
}



///////////////////////////////////////////////////////////////////////////////
// AR helper functions
///////////////////////////////////////////////////////////////////////////////

void EA::Allocator::AR_Assert(bool bExpression, char* pMessage)
{
	if(!bExpression)
		printf("%s\n", pMessage);
}


//////////////////////////////////////////////////////////////////////////
// AR_PtrToString
//
// This function converts a pointer to a string. It is is like calling 
// "sprintf(pBuffer, "%p", pValue) except that it lets you pick the 
// output numerical base and returns you a pointer to the end of the 
// buffer when finished. And this function does no memory allocation.
//
char* EA::Allocator::AR_PtrToString(const void* pValue, char* pBuffer, int nBase, char** ppBufferEnd)
{
	int       i(0), j(0);
	char      pBufferBackwards[96];
	uintptr_t nValueUnsigned = (uintptr_t)pValue;

	// Check for invalid base values.
	if((nBase < 2) || (nBase > 16)){
		pBuffer[0] = 0;
		return pBuffer;
	}

	// Convert the value to a string.
	do{
		char c = (char)('0' + (nValueUnsigned % (unsigned)nBase));
		if(c > '9')
			c += (('a' - '9') - 1);
		pBufferBackwards[j++] = c;
		nValueUnsigned /= (unsigned)nBase;
	}while(nValueUnsigned > 0);

	// Copy the backwards buffer into the destination buffer.
	--j; //Move back to the last valid character.
	while(j >= 0)
		pBuffer[i++] = pBufferBackwards[j--];

	// Null terminate the destination buffer and return.
	pBuffer[i] = 0;
	if(ppBufferEnd)
		*ppBufferEnd = pBuffer + i;
	return pBuffer;
}



//////////////////////////////////////////////////////////////////////////
// AR_SizeToString
//
// This function converts a size_t to a string. It is is like calling 
// "sprintf(pBuffer, "%u", nValue) except that it lets you pick the 
// output numerical base and returns you a pointer to the end of the 
// buffer when finished. And this function does no memory allocation.
//
char* EA::Allocator::AR_SizeToString(size_t nValue, char* pBuffer, int nBase, char** ppBufferEnd)
{
	int    i(0), j(0);
	char   pBufferBackwards[96];
	size_t nValueUnsigned = (size_t)nValue;

	// Check for invalid base values.
	if((nBase < 2) || (nBase > 16)){
		pBuffer[0] = 0;
		return pBuffer;
	}

	// Convert the value to a string.
	do{
		char c = (char)('0' + (nValueUnsigned % (size_t)nBase));
		if(c > '9')
			c += (('a' - '9') - 1);
		pBufferBackwards[j++] = c;
		nValueUnsigned /= (size_t)nBase;
	}while(nValueUnsigned > 0);

	// Copy the backwards buffer into the destination buffer.
	--j; //Move back to the last valid character.
	while(j >= 0)
		pBuffer[i++] = pBufferBackwards[j--];

	// Null terminate the destination buffer and return.
	pBuffer[i] = 0;
	if(ppBufferEnd)
		*ppBufferEnd = pBuffer + i;
	return pBuffer;
}


#if defined(EA_COMPILER_MSVC)
	#pragma warning(pop)
#endif




