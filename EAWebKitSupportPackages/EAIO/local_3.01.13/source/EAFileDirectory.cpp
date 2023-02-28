/////////////////////////////////////////////////////////////////////////////
// EAFileDirectory.cpp
//
// Copyright (c) 2003, Electronic Arts. All Rights Reserved.
// Created by Paul Pedriana, Maxis
//
// Implements file system directory searching and manipulation.
/////////////////////////////////////////////////////////////////////////////


#include <EAIO/internal/Config.h>
#include <EAIO/EAFileDirectory.h>
#include <EAIO/PathString.h>
#include <EAIO/FnEncode.h>
#include <EAIO/FnMatch.h>
#include <EAIO/EAFileUtil.h>
#include <coreallocator/icoreallocator_interface.h>
#include <EAStdC/EAString.h>
#include <EAStdC/EABitTricks.h>
#include <EAStdC/EAAlignment.h>
#include <eathread/eathread.h>
#include EA_ASSERT_HEADER

#if defined(EA_PLATFORM_MICROSOFT)
	#pragma warning(push, 0)
	#include <Windows.h>
	#pragma warning(pop)
#elif EAIO_USE_UNIX_IO
	#ifdef EA_PLATFORM_ANDROID
		#include <jni.h>
	#endif
	#include <sys/stat.h>
	#include <sys/types.h>
	#include <dirent.h>
	#ifdef EA_PLATFORM_NX
		#include <nn/fs/fs_FileSystem.h>
		#include <nn/fs/fs_File.h>
	#endif
#elif defined(EA_PLATFORM_SONY)
	#include <scebase_common.h>
	#include <kernel.h>
	#include <sdk_version.h>
#endif


#if (EASTDC_VERSION_N < 10801)
	#error EAStdC 1.08.01 or later is required to compile this version of EAIO. This is due to char32_t support in EAStdC 1.06.00+ and wchar_t support in 1.08.01+.
#endif


namespace EA
{

namespace IO
{


/// ENTRYLIST_NAME
///
/// Defines a default container name in the absence of a user-provided name.
///
#define ENTRYLIST_NAME "DirectoryIterator"


namespace Internal
{
	template<class T>
	T* Allocate(EA::Allocator::ICoreAllocator* pAllocator, const char* pName)
	{
		T* const pT = (T*)pAllocator->Alloc(sizeof(T), pName, EA::Allocator::MEM_TEMP);
		if(pT)
			new(pT) T;
		return pT;
	}

	template<class T>
	void Free(EA::Allocator::ICoreAllocator* pAllocator, T* pT)
	{
		if(pT)
		{
			pT->~T();
			pAllocator->Free(pT);
		}
	}

	#if defined(EA_PLATFORM_MICROSOFT)
		time_t SystemTimeToEAIOTime(const FILETIME& fileTime)
		{
			ULARGE_INTEGER ull;
			ull.LowPart  = fileTime.dwLowDateTime;
			ull.HighPart = fileTime.dwHighDateTime;
			return (time_t)((ull.QuadPart / 10000000ULL) - 11644473600ULL);
		}
	#endif


	void FilterEntries(EntryFindData* pEntryFindData, const wchar_t* pFilterPattern)
	{
		Path::EnsureTrailingSeparator(pEntryFindData->mDirectoryPath, kMaxPathLength);

		if(pFilterPattern)
			EA::StdC::Strlcpy(pEntryFindData->mEntryFilterPattern, pFilterPattern, kMaxPathLength);
		else
		{
			pEntryFindData->mEntryFilterPattern[0] = '*';
			pEntryFindData->mEntryFilterPattern[1] = 0;
		}
	}

} // namespace Internal



///////////////////////////////////////////////////////////////////////////////
// Read
//
size_t DirectoryIterator::Read(const wchar_t* pDirectory, EntryList& entryList, 
							 const wchar_t* pFilterPattern, int nDirectoryEntryFlags, 
							 size_t maxResultCount, bool bReadFileStat)
{
	EntryFindData entryFindData, *pEntryFindData;
	size_t        resultCount = 0;

	#if EASTL_NAME_ENABLED // If the EntryList doesn't have a unique name, we give it one here.
		if(entryList.get_allocator().get_name() && !strcmp(EASTL_LIST_DEFAULT_NAME, entryList.get_allocator().get_name()))
			entryList.get_allocator().set_name(ENTRYLIST_NAME);
	#endif

	entryFindData.mbReadFileStat = bReadFileStat;

	// Iterate entries.
	for(pEntryFindData = EntryFindFirst(pDirectory, pFilterPattern, &entryFindData); pEntryFindData && (resultCount < maxResultCount); )
	{
		if(!StrEq(pEntryFindData->mName, EA_DIRECTORY_CURRENT_W) && // If it is neither "./" nor "../"
		   !StrEq(pEntryFindData->mName, EA_DIRECTORY_PARENT_W))
		{
			if(pEntryFindData->mbIsDirectory) // If it's a directory...
			{
				if(nDirectoryEntryFlags & kDirectoryEntryDirectory) // If the user wants to enumerate directories...
				{
					resultCount++;

					entryList.push_back();
					Entry& entry = entryList.back();

					entry.mType             = kDirectoryEntryDirectory;
					entry.msName            = pEntryFindData->mName;
					entry.mCreationTime     = pEntryFindData->mCreationTime;
					entry.mModificationTime = pEntryFindData->mModificationTime;
					entry.mSize             = pEntryFindData->mSize;
				  //EA_ASSERT(entry.mSize < UINT64_C(10000000000)); // Sanity check.
				}
			}
			else    // Else it's a file.
			{
				if(nDirectoryEntryFlags & kDirectoryEntryFile) // If the user wants to enumerate files...
				{
					resultCount++;

					entryList.push_back();
					Entry& entry = entryList.back();

					entry.mType             = kDirectoryEntryFile;
					entry.msName            = pEntryFindData->mName;
					entry.mCreationTime     = pEntryFindData->mCreationTime;
					entry.mModificationTime = pEntryFindData->mModificationTime;
					entry.mSize             = pEntryFindData->mSize;
				  //EA_ASSERT(entry.mSize < UINT64_C(10000000000)); // Sanity check.
				}
			}
		}

		if(!EntryFindNext(pEntryFindData))
			break;
	}

	if(pEntryFindData)
	{
		if((nDirectoryEntryFlags & kDirectoryEntryParent) && (resultCount < maxResultCount))
		{
			// To do: We don't want to do this if the directory is a root directory. But how do we 
			// know for sure that we are at a root directory? Is it safe enough to compare to "/" on 
			// Unix-like systems and X:\\ on Microsoft systems? That seems reasonable, but what if 
			// the input pDirectory was a relative directory path and the user was dependent on the
			// system supporting relative paths?

			// if(the count of '/' and '\\' chars in pDirectory is > 1)
			{
				resultCount++;

				entryList.push_front();
				Entry& entry = entryList.front();

				entry.mType             = kDirectoryEntryDirectory;
				entry.msName            = EA_DIRECTORY_PARENT_W;
				entry.mCreationTime     = pEntryFindData->mCreationTime;
				entry.mModificationTime = pEntryFindData->mModificationTime;
				entry.mSize             = pEntryFindData->mSize;
			  //EA_ASSERT(entry.mSize < UINT64_C(10000000000)); // Sanity check.
			}
		}

		if((nDirectoryEntryFlags & kDirectoryEntryCurrent) && (resultCount < maxResultCount))
		{
			resultCount++;

			entryList.push_front();
			Entry& entry = entryList.front();

			entry.mType             = kDirectoryEntryDirectory;
			entry.msName            = EA_DIRECTORY_CURRENT_W;
			entry.mCreationTime     = pEntryFindData->mCreationTime;
			entry.mModificationTime = pEntryFindData->mModificationTime;
			entry.mSize             = pEntryFindData->mSize;
		  //EA_ASSERT(entry.mSize < UINT64_C(10000000000)); // Sanity check.
		}

		EntryFindFinish(pEntryFindData); // This doesn't free pEntryFindData, as pEntryFindData == &entryFindData.
	}

	return resultCount;
}



///////////////////////////////////////////////////////////////////////////////
// ReadRecursive
//
size_t DirectoryIterator::ReadRecursive(const wchar_t* pBaseDirectory, EntryList& entryList, const wchar_t* pFilterPattern, int nEntryTypeFlags, 
										bool bIncludeBaseDirectoryInSearch, bool bFullPaths, size_t maxResultCount, bool bReadFileStat)
{
	EA::IO::Path::PathStringW pathTemp;

	if(mnRecursionIndex++ == 0) // If being called for the first time...
	{
		#if EASTL_NAME_ENABLED // If the EntryList doesn't have a unique name, we give it one here.
			if(entryList.get_allocator().get_name() && !strcmp(EASTL_LIST_DEFAULT_NAME, entryList.get_allocator().get_name()))
			   entryList.get_allocator().set_name(ENTRYLIST_NAME);
		#endif

		mnListSize           = 0;
		mpBaseDirectory      = pBaseDirectory;
		mBaseDirectoryLength = (eastl_size_t)EA::StdC::Strlen(pBaseDirectory);
		if(!mBaseDirectoryLength || !IsFilePathSeparator(pBaseDirectory[mBaseDirectoryLength - 1]))
			mBaseDirectoryLength++;
	}

	if((nEntryTypeFlags & kDirectoryEntryFile) && 
	   (bIncludeBaseDirectoryInSearch || (mnRecursionIndex > 1)) && 
	   (mnListSize < maxResultCount))
	{
		// Add all files in the current directory into the list, using the filter pattern.
		const size_t additionCount = Read(pBaseDirectory, entryList, pFilterPattern, kDirectoryEntryFile, maxResultCount - mnListSize, bReadFileStat);

		EntryList::iterator it(entryList.end());
		eastl::advance(it, -(int32_t)(uint32_t)additionCount);

		for(; it != entryList.end(); ++it)
		{
			Entry& entry = *it;

			mnListSize++;

			const eastl_size_t savedLength = entry.msName.length();
			entry.msName.insert(0, pBaseDirectory);
			const eastl_size_t directoryEnd = entry.msName.length() - savedLength;

			if(directoryEnd && !IsFilePathSeparator(entry.msName[directoryEnd - 1]))
				entry.msName.insert(directoryEnd, 1, EA_FILE_PATH_SEPARATOR_W);

			if(!bFullPaths)
				entry.msName.erase(0, mBaseDirectoryLength);
		}
	}

	if(mnListSize < maxResultCount)
	{
		// To do: Find a way to avoid this temporary list.
		// Since the list is only a list of directories under the 
		// current directory, it shouldn't need all that many entries.
		EntryList entryListTemp(entryList.get_allocator());

		// Add all directories in the current directory into the list, ignoring the filter pattern.
		Read(pBaseDirectory, entryListTemp, NULL, kDirectoryEntryDirectory, kMaxEntryCountDefault, bReadFileStat);

		for(EntryList::iterator it = entryListTemp.begin(); (it != entryListTemp.end()) && (mnListSize < maxResultCount); ++it)
		{
			const Entry& entry = *it; 

			pathTemp.assign( pBaseDirectory );
			EA::IO::Path::Append( pathTemp, entry.msName.c_str() );  // This previously was Join but Join was calling Normalize, which was modifying the pBaseDirectory part of the path string, and we don't want that messed with. Actually we don't need any normalization.

			//ConcatenatePathComponents(pathTemp, pBaseDirectory, entry.msName.c_str());

			// Possibly add this directory to the entry list.
			if(nEntryTypeFlags & kDirectoryEntryDirectory)
			{
				if(!pFilterPattern || FnMatch(pFilterPattern, entry.msName.c_str(), kFNMCaseFold))
				{
					mnListSize++;
					entryList.push_back();
					Entry& listEntry = entryList.back();
					listEntry.mType  = kDirectoryEntryDirectory;
					listEntry.msName = pathTemp.c_str();

					if(!bFullPaths)
						listEntry.msName.erase(0, mBaseDirectoryLength);
				}
			}

			// Now recursively read the subdirectory.
			ReadRecursive(pathTemp.c_str(), entryList, pFilterPattern, nEntryTypeFlags, true, bFullPaths, maxResultCount, bReadFileStat);
		}
	}

	mnRecursionIndex--;

	return mnListSize;
}


template <typename T>
size_t ReadRecursiveImpl(DirectoryIterator *_this, const T* pBaseDirectory, EA::IO::DirectoryIterator::EntryList& entryList, const T* pFilterPattern, int nEntryTypeFlags, 
						 bool bIncludeBaseDirectoryInSearch, bool bFullPaths, size_t maxResultCount, bool bReadFileStat)
{
	// TODO - Most part of this code could be merged with EntryFindFirstImpl
	wchar_t directoryPathBuffer[kMaxPathLength];
	wchar_t filterPatternBuffer[kMaxPathLength];
	wchar_t *pConvertedFilterPattern = NULL;

	int directoryPathConversionResult = EA::StdC::Strlcpy(directoryPathBuffer, pBaseDirectory, kMaxPathLength);
	bool directoryPathConversionSucceeded = directoryPathConversionResult >= 0 && directoryPathConversionResult < kMaxPathLength;
		
	int filterPatternConversionResult;
	bool filterPatternConversionSucceeded = true;

	if (pFilterPattern)
	{
		filterPatternConversionResult = EA::StdC::Strlcpy(filterPatternBuffer, pFilterPattern, kMaxPathLength);
		filterPatternConversionSucceeded = filterPatternConversionResult >= 0 && filterPatternConversionResult < kMaxPathLength;
		pConvertedFilterPattern = filterPatternBuffer;
	}

	if (directoryPathConversionSucceeded && filterPatternConversionSucceeded)
	{
		return _this->ReadRecursive(directoryPathBuffer, entryList, pConvertedFilterPattern, nEntryTypeFlags, bIncludeBaseDirectoryInSearch, bFullPaths, maxResultCount, bReadFileStat);
	}

	return 0;
}

size_t DirectoryIterator::ReadRecursive(const char* pBaseDirectory, EntryList& entryList, const char* pFilterPattern, int nDirectoryEntryFlags, 
										bool bIncludeBaseDirectoryInSearch, bool bFullPaths, size_t maxResultCount, bool bReadFileStat)
{
	return ReadRecursiveImpl<char>(this, pBaseDirectory, entryList, pFilterPattern, nDirectoryEntryFlags, bIncludeBaseDirectoryInSearch, bFullPaths, maxResultCount, bReadFileStat);
}


#if EA_WCHAR_UNIQUE || EA_WCHAR_SIZE == 4
	size_t DirectoryIterator::ReadRecursive(const char16_t* pBaseDirectory, EntryList& entryList, const char16_t* pFilterPattern, int nDirectoryEntryFlags, 
											bool bIncludeBaseDirectoryInSearch, bool bFullPaths, size_t maxResultCount, bool bReadFileStat)
	{
		return ReadRecursiveImpl<char16_t>(this, pBaseDirectory, entryList, pFilterPattern, nDirectoryEntryFlags, bIncludeBaseDirectoryInSearch, bFullPaths, maxResultCount, bReadFileStat);
	}
#endif

	
#if EA_WCHAR_UNIQUE || EA_WCHAR_SIZE == 2
	size_t DirectoryIterator::ReadRecursive(const char32_t* pBaseDirectory, EntryList& entryList, const char32_t* pFilterPattern, int nDirectoryEntryFlags, 
											bool bIncludeBaseDirectoryInSearch, bool bFullPaths, size_t maxResultCount, bool bReadFileStat)
	{
		return ReadRecursiveImpl<char32_t>(this, pBaseDirectory, entryList, pFilterPattern, nDirectoryEntryFlags, bIncludeBaseDirectoryInSearch, bFullPaths, maxResultCount, bReadFileStat);
	}
#endif

#undef ENTRYLIST_NAME

									  
#if defined(EA_PLATFORM_MICROSOFT)

	////////////////////////////////////////////////////////////////////////////
	// EntryFindFirst
	//
	EAIO_API EntryFindData* EntryFindFirst(const wchar_t* pDirectoryPath, const wchar_t* pFilterPattern, EntryFindData* pEntryFindData)
	{
		using namespace Internal;

		EA_ASSERT(pDirectoryPath);
		if(pDirectoryPath[0] || (!pFilterPattern || pFilterPattern[0]))
		{
			const wchar_t pAny[] = { '*', 0 };
			Path::PathStringW pPathSpecification;
			pPathSpecification += pDirectoryPath;
			Path::Append(pPathSpecification, pFilterPattern ? pFilterPattern : pAny);

			if(!pEntryFindData)
			{
				pEntryFindData = Allocate<EntryFindData>(IO::GetAllocator(), EAIO_ALLOC_PREFIX "EAFileDirectory/EntryFindData");
				pEntryFindData->mbIsAllocated = true;
			}

			WIN32_FIND_DATAW win32FindDataW;
			#if EA_WINAPI_FAMILY_PARTITION(EA_WINAPI_PARTITION_DESKTOP)
				HANDLE hFindFile = FindFirstFileW(pPathSpecification.c_str(), &win32FindDataW);
			#else
				HANDLE hFindFile = FindFirstFileExW(pPathSpecification.c_str(), FindExInfoBasic, &win32FindDataW, FindExSearchNameMatch, NULL, 0);
			#endif

			if(hFindFile != INVALID_HANDLE_VALUE)
			{
				EA::StdC::Strlcpy(pEntryFindData->mName, win32FindDataW.cFileName, kMaxPathLength);
				pEntryFindData->mbIsDirectory  = (win32FindDataW.dwFileAttributes & FILE_ATTRIBUTE_DIRECTORY) != 0;
				if(pEntryFindData->mbIsDirectory)
					Path::EnsureTrailingSeparator(pEntryFindData->mName, kMaxPathLength);

				pEntryFindData->mCreationTime     = Internal::SystemTimeToEAIOTime(win32FindDataW.ftCreationTime);
				pEntryFindData->mModificationTime = Internal::SystemTimeToEAIOTime(win32FindDataW.ftLastWriteTime);
				pEntryFindData->mSize             = ((uint64_t)win32FindDataW.nFileSizeHigh << 32i64) + win32FindDataW.nFileSizeLow;

				EA::StdC::Strlcpy(pEntryFindData->mDirectoryPath, pDirectoryPath, kMaxPathLength);

				if(pFilterPattern)
					EA::StdC::Strlcpy(pEntryFindData->mEntryFilterPattern, pFilterPattern, kMaxPathLength);
				else
					pEntryFindData->mEntryFilterPattern[0] = 0;

				pEntryFindData->mPlatformHandle = (uintptr_t)hFindFile;

				return pEntryFindData;
			}

			if (pEntryFindData->mbIsAllocated)
				Free(EA::IO::GetAllocator(), pEntryFindData);
		}

		return NULL;
	}

	

	////////////////////////////////////////////////////////////////////////////
	// EntryFindNext
	//
	EAIO_API EntryFindData* EntryFindNext(EntryFindData* pEntryFindData)
	{
		if(pEntryFindData)
		{
			HANDLE hFindFile = (HANDLE)pEntryFindData->mPlatformHandle;

			#if defined(EA_PLATFORM_WINDOWS) || defined(EA_PLATFORM_WINDOWS_PHONE) // If wide character functions are supported...
				WIN32_FIND_DATAW win32FindDataW;

				if(FindNextFileW(hFindFile, &win32FindDataW))
				{
					EA::StdC::Strlcpy(pEntryFindData->mName, win32FindDataW.cFileName, kMaxPathLength);

					pEntryFindData->mbIsDirectory = (win32FindDataW.dwFileAttributes & FILE_ATTRIBUTE_DIRECTORY) != 0;
					if(pEntryFindData->mbIsDirectory)
						Path::EnsureTrailingSeparator(pEntryFindData->mName, kMaxPathLength);

					pEntryFindData->mCreationTime     = Internal::SystemTimeToEAIOTime(win32FindDataW.ftCreationTime);
					pEntryFindData->mModificationTime = Internal::SystemTimeToEAIOTime(win32FindDataW.ftLastWriteTime);
					pEntryFindData->mSize             = ((uint64_t)win32FindDataW.nFileSizeHigh << 32i64) + win32FindDataW.nFileSizeLow;

					return pEntryFindData;
				}
			#else
				WIN32_FIND_DATAA win32FindDataA;

				if(FindNextFileA(hFindFile, &win32FindDataA))
				{
					EA::StdC::Strlcpy(pEntryFindData->mName, win32FindDataA.cFileName, kMaxPathLength);

					pEntryFindData->mbIsDirectory = (win32FindDataA.dwFileAttributes & FILE_ATTRIBUTE_DIRECTORY) != 0;
					if(pEntryFindData->mbIsDirectory)
						Path::EnsureTrailingSeparator(pEntryFindData->mName, kMaxPathLength);

					pEntryFindData->mCreationTime     = Internal::SystemTimeToEAIOTime(win32FindDataA.ftCreationTime);
					pEntryFindData->mModificationTime = Internal::SystemTimeToEAIOTime(win32FindDataA.ftLastWriteTime);
					pEntryFindData->mSize             = ((uint64_t)win32FindDataA.nFileSizeHigh << 32i64) + win32FindDataA.nFileSizeLow;

					return pEntryFindData;
				}

				// Problem: The XBox and XBox 360 platforms don't always return "./" and "../" 
				// directories. We need to provide those if they aren't present and if they are
				// appropriate ("../" shouldn't be present for root directories).
			#endif
		}
		return NULL;
	}

	
	////////////////////////////////////////////////////////////////////////////
	// EntryFindFinish
	//
	EAIO_API void EntryFindFinish(EntryFindData* pEntryFindData)
	{
		using namespace Internal;

		if(pEntryFindData)
		{
			HANDLE hFindFile = (HANDLE)pEntryFindData->mPlatformHandle;

			if((hFindFile != 0) && (hFindFile != INVALID_HANDLE_VALUE))
				FindClose(hFindFile);

			if(pEntryFindData->mbIsAllocated)
				Free(EA::IO::GetAllocator(), pEntryFindData);
		}
	}


#elif EAIO_USE_UNIX_IO

	#ifdef EA_PLATFORM_ANDROID

		// include the header in the EA::IO namespace
		#ifndef ASSETMANAGER_INIT_FAILURE_MESSAGE
			#define ASSETMANAGER_INIT_FAILURE_MESSAGE "Note: EAIO.Startup(activityInstance) must be called from Java prior to accessing appbundle files."
		#endif
		#include "Android/assetmanagerjni.h"

		
		// The mPlatformHandle is set to point to this variable when the EntryFindData structure is
		// created for a appbundle:/ directory.  This avoids an expensive string operation to see
		// if the original directory was an appbundle directory.
		static char AndroidAppBundleEntryIndicator;

		struct AndroidEntryFindData
		{
			jobject list;
			int     curListIndex;
		};

		static EntryFindData* AndroidEntryFindNext(const AutoJNIEnv& autoEnv, EntryFindData* pEntryFindData)
		{
			AndroidEntryFindData* pAndroidData = reinterpret_cast<AndroidEntryFindData*>(pEntryFindData->mPlatformData);
			wchar_t* entryName = pEntryFindData->mName;

			for(;;)
			{     
				if(AssetManagerJNI::GetListElement(autoEnv, pAndroidData->list, pAndroidData->curListIndex, entryName, kMaxPathLength))
				{
					pAndroidData->curListIndex++;   // Move to the next element

					// HACK
					// There doesn't seem to currently be a nice efficient way to determine if an entry is a directory
					// or not. So we make the assumption that any entry name without an extension (e.g. .jpg) is a directory.
					wchar_t* pFileExtension = Path::GetFileExtension(entryName);

					if(*pFileExtension)
					{
						pEntryFindData->mbIsDirectory = false;
					}
					else
					{
						pEntryFindData->mbIsDirectory = true;
						pFileExtension[0] = '/';
						pFileExtension[1] = '\0';
					}

					if((pEntryFindData->mEntryFilterPattern[0] == 0) || 
						FnMatch(pEntryFindData->mEntryFilterPattern, entryName, kFNMNone))
					{
						// Return the item if it passes the filter
						return pEntryFindData;
					}
				}
				else
				{
					// There are no more items available so return NULL to indicate completion.
					return NULL;
				}
			}
		}

		static EntryFindData* AndroidEntryFindNext(EntryFindData* pEntryFindData)
		{
			AutoJNIEnv autoEnv;
			return AndroidEntryFindNext(autoEnv, pEntryFindData);
		}

		static void AndroidEntryFindFinish(const AutoJNIEnv &autoEnv, EntryFindData* pEntryFindData)
		{
			using namespace Internal;

			AndroidEntryFindData* pAndroidData = reinterpret_cast<AndroidEntryFindData*>(pEntryFindData->mPlatformData);
			AssetManagerJNI::FreeList(autoEnv, pAndroidData->list);

			if(pEntryFindData->mbIsAllocated)
				Free(EA::IO::GetAllocator(), pEntryFindData);
		}

		static void AndroidEntryFindFinish(EntryFindData* pEntryFindData)
		{
			AutoJNIEnv autoEnv;
			return AndroidEntryFindFinish(autoEnv, pEntryFindData);
		}

		static EntryFindData* AndroidEntryFindFirst(const wchar_t* pDirectoryPath, const wchar_t* pFilterPattern, EntryFindData* pEntryFindData)
		{
			using namespace Internal;

			AutoJNIEnv     autoEnv;
			const wchar_t* path = pDirectoryPath + APPBUNDLE_PATH_PREFIX_LENGTH;
			EA::IO::Path::PathStringW wPath(path);
			EA::IO::Path::Simplify(wPath);
			jobject        list = AssetManagerJNI::List(autoEnv, wPath.c_str(), wPath.length());

			if(list)
			{
				if(!pEntryFindData)
				{
					pEntryFindData = Allocate<EntryFindData>(IO::GetAllocator(), EAIO_ALLOC_PREFIX "EAFileDirectory/EntryFindData");
					memset(pEntryFindData, 0, sizeof(EntryFindData));   // Clear fields of structure
					pEntryFindData->mbIsAllocated = true;
				}
				else
				{
					memset(pEntryFindData, 0, sizeof(EntryFindData));   // Clear fields of structure
				}

				pEntryFindData->mPlatformHandle = reinterpret_cast<uintptr_t>(&AndroidAppBundleEntryIndicator);
				AndroidEntryFindData* pAndroidData = reinterpret_cast<AndroidEntryFindData*>(pEntryFindData->mPlatformData);

				FilterEntries(pEntryFindData, pFilterPattern);

				pAndroidData->list = list;
				pAndroidData->curListIndex = 0;
				EntryFindData* result = AndroidEntryFindNext(autoEnv, pEntryFindData);

				if(!result)
					AndroidEntryFindFinish(autoEnv, pEntryFindData);

				return result;
			}

			return NULL;
		}

	#endif // #if defined(EA_PLATFORM_ANDROID)

	static void UpdateEntry(const char* path, EntryFindData* pEntryFindData)
	{
		#if defined(EA_PLATFORM_NX)
			nn::fs::FileHandle fh;
			if (nn::fs::OpenFile(&fh, path, nn::fs::OpenMode_Read).IsSuccess())
			{
				int64_t fs;
				nn::fs::GetFileSize(&fs, fh);

				// There is no way to get file time on NX (other than a no-ship, debug only function)
				pEntryFindData->mCreationTime = 0;
				pEntryFindData->mModificationTime = 0;
				pEntryFindData->mSize = (uint64_t)fs;

				nn::fs::CloseFile(fh);
			}
		#else
			struct stat tempStat;
			const int result = stat(path, &tempStat);
			if (result == 0)
			{
				pEntryFindData->mCreationTime = (time_t)tempStat.st_ctime;
				pEntryFindData->mModificationTime = (time_t)tempStat.st_mtime;
				pEntryFindData->mSize = (uint64_t)tempStat.st_size;
			}
		#endif	
	}

	EAIO_API EntryFindData* EntryFindFirst(const wchar_t* pDirectoryPath, const wchar_t* pFilterPattern, EntryFindData* pEntryFindData)
	{
		using namespace Internal;

		EA_ASSERT(pDirectoryPath);
		if(pDirectoryPath[0] || (!pFilterPattern || pFilterPattern[0]))
		{
			Path::PathString8 directory8;
			Path::PathString8 filePattern8;

			// Measure how many UTF-8 chars we'll need (EASTL factors-in a hidden + 1 for NULL terminator)
			size_t nCharsNeeded = EA::StdC::Strlcpy((char*)NULL, pDirectoryPath, 0);
			directory8.resize(nCharsNeeded);
			EA::StdC::Strlcpy(&directory8[0], pDirectoryPath, nCharsNeeded + 1);

		  #ifdef EA_PLATFORM_ANDROID
			if(strstr(directory8.c_str(), APPBUNDLE_PATH_PREFIX) == directory8.c_str()) // If the path begins with the APPBUNDLE_PATH_PREFIX prefix..
			{
				Path::PathStringW wDirectoryPath(pDirectoryPath);
				Path::Simplify(wDirectoryPath);
				return AndroidEntryFindFirst(wDirectoryPath.c_str(), pFilterPattern, pEntryFindData);
			}
			else
		  #endif
			{
				DIR* const dir = opendir(directory8.c_str());

				if(!dir)
					return NULL;

				// Follow windows semantics: don't distinguish between an empty directory and not finding anything.
				#if EAIO_READDIR_R_PRESENT
					dirent  dirEntryDataStorage;
					dirent* dirEntryData = &dirEntryDataStorage;
				#endif
				dirent*  dirEntry;
				bool     bPatternMatch = false;

				#if EA_FILE_SYSTEM_CASE_SENSITIVE
					const int kMatchFlags = kFNMNone;
				#else
					const int kMatchFlags = kFNMCaseFold;
				#endif

				if(pFilterPattern)
					EA::StdC::Strlcpy<Path::PathString8, const wchar_t>(filePattern8, pFilterPattern);

				if(!pEntryFindData)
				{
					pEntryFindData = Allocate<EntryFindData>(IO::GetAllocator(), EAIO_ALLOC_PREFIX "EAFileDirectory/EntryFindData");
					pEntryFindData->mbIsAllocated = true;
				}

				do // Run a loop so we can filter out any "." and ".." entries that might be present and entries that don't match the input pattern.
				{
				  #if EAIO_READDIR_R_PRESENT
					if((readdir_r(dir, dirEntryData, &dirEntry) != 0) || !dirEntry)
				  #else
					if((dirEntry = readdir(dir)) == NULL)
				  #endif
					{
						break;
					}

					if((strcmp(dirEntry->d_name, ".")  != 0) &&
					   (strcmp(dirEntry->d_name, "..") != 0))
					{
					  // Test to see if the file has been deleted, which is the case if its file id is 0.
					  #if defined(EA_PLATFORM_BSD) || defined(EA_PLATFORM_APPLE)
						if(dirEntry->d_fileno != 0) // Linux has d_ino but the kernel doesn't treat it the same.
					  #endif
						{
							// The following char to wchar_t Strlcpy can return -1 if d_name is not valid UTF8. Unix file systems
							// normally are encoding-independent and names are merely an array of bytes. The large majority of the time
							// files are named with UTF8 encoding, but it's possible for one to not be named like so, such as if 
							// an app simply created a file with another encoding or if mounting another file system like Windows in 
							// a non-UTF8 mode. We are currently dependent on UTF8, as otherwise it would generate an explosion of 
							// encodings to support. We ignore any files that don't appear to be UTF8-encoded.
							int copyResult = EA::StdC::Strlcpy(pEntryFindData->mName, dirEntry->d_name, EAArrayCount(pEntryFindData->mName));

							if((copyResult > 0) && (copyResult < (int)EAArrayCount(pEntryFindData->mName))) // If d_name is valid UTF8 and if mName is long enough to hold the converted d_name...
								bPatternMatch = filePattern8.empty() || FnMatch(filePattern8.c_str(), dirEntry->d_name, kMatchFlags);
						}
					}
				} while(!bPatternMatch);

				if(bPatternMatch)
				{
					#if defined(__CYGWIN__) // Cygwin chooses to take the most limited view of the Posix standard and doesn't implement d_type.
						Path::EnsureTrailingSeparator(directory8);
						directory8 += dirEntry->d_name;
						pEntryFindData->mbIsDirectory = Directory::Exists(directory8.c_str());
					#else
						pEntryFindData->mbIsDirectory = (dirEntry->d_type == DT_DIR);
					#endif

					if(pEntryFindData->mbIsDirectory)
						Path::EnsureTrailingSeparator(pEntryFindData->mName, kMaxPathLength);

					EA::StdC::Strlcpy(pEntryFindData->mDirectoryPath, pDirectoryPath, EAArrayCount(pEntryFindData->mDirectoryPath)); // Copy wchar_t to wchar_t

					// mCreationTime / mModificationTime / mSize
					pEntryFindData->mCreationTime     = 0;
					pEntryFindData->mModificationTime = 0;
					pEntryFindData->mSize             = 0;

					if(pEntryFindData->mbReadFileStat)
					{
						Path::PathString8 path8;
						ConvertPath(path8, pEntryFindData->mDirectoryPath); // This conversion always succeeds because wchar_t strings are always valid, and PathString8 can expand to any size.
						path8 += dirEntry->d_name;

						UpdateEntry(path8.c_str(), pEntryFindData);
					}

					// Clean up and return
					FilterEntries(pEntryFindData, pFilterPattern);

					pEntryFindData->mPlatformHandle = (uintptr_t) dir;
				}
				else
				{
					if(pEntryFindData->mbIsAllocated) // If we allocated it above...
						Free(EA::IO::GetAllocator(), pEntryFindData);
					closedir(dir);
					return NULL; // If no directory entry matches the user-specified pattern, we'll usually be exiting via this line due to (dirEntry == NULL).
				}

				return pEntryFindData;
			}
		}

		return NULL;
	}


	EAIO_API EntryFindData* EntryFindNext(EntryFindData* pEntryFindData)
	{
		if(pEntryFindData)
		{
			#ifdef EA_PLATFORM_ANDROID
				if( pEntryFindData->mPlatformHandle == reinterpret_cast<uintptr_t>(&AndroidAppBundleEntryIndicator))
					return AndroidEntryFindNext(pEntryFindData);
			#endif

			// Unix defines DIR as an opaque pointer whose type is undefined. It would be useful to verify that in code here.
			// EA_COMPILETIME_ASSERT(sizeof(DIR) <= sizeof(pEntryFindData->mPlatformData));
			DIR* const dir = (DIR*)pEntryFindData->mPlatformHandle;
			  
			#if EAIO_READDIR_R_PRESENT
					dirent  dirEntryDataStorage;
					dirent *dirEntryData = &dirEntryDataStorage;
			#endif
			dirent* dirEntry;

			#if EA_FILE_SYSTEM_CASE_SENSITIVE
				const int kMatchFlags = kFNMNone;
			#else
				const int kMatchFlags = kFNMCaseFold;
			#endif

			#if EAIO_READDIR_R_PRESENT
			  if(readdir_r(dir, dirEntryData, &dirEntry) != 0)
			#else
			  if((dirEntry = readdir(dir)) == NULL)
			#endif
				return NULL; // If no directory entry matches the user-specified pattern, we'll usually be exiting via this line due to (dirEntry == NULL).

			Path::PathString8 filePattern8;
			EA::StdC::Strlcpy<Path::PathString8, const wchar_t>(filePattern8, pEntryFindData->mEntryFilterPattern);

			while(dirEntry)
			{
			  // Test to see if the file has been deleted, which is the case if its file id is 0.
			  #if defined(EA_PLATFORM_BSD) || defined(EA_PLATFORM_APPLE)
				if(dirEntry->d_fileno != 0) // Linux has d_ino but the kernel doesn't treat it the same.
			  #endif
				{
					// The following char to wchar_t Strlcpy can return -1 if d_name is not valid UTF8. Unix file systems
					// normally are encoding-independent and names are merely an array of bytes. The large majority of the time
					// files are named with UTF8 encoding, but it's possible for one to not be named like so, such as if 
					// an app simply created a file with another encoding or if mounting another file system like Windows in 
					// a non-UTF8 mode. We are currently dependent on UTF8, as otherwise it would generate an explosion of 
					// encodings to support. We ignore any files that don't appear to be UTF8-encoded.
					int copyResult = EA::StdC::Strlcpy(pEntryFindData->mName, dirEntry->d_name, EAArrayCount(pEntryFindData->mName)); 

					if((copyResult > 0) && (copyResult < (int)EAArrayCount(pEntryFindData->mName)) && // If d_name is valid UTF8 and if mName is long enough to hold the converted d_name...
						(filePattern8.empty() || FnMatch(filePattern8.c_str(), dirEntry->d_name, kMatchFlags)))  // If there is no filter pattern or if the filter pattern matches...
					{
						#if defined(__CYGWIN__) // Cygwin chooses to take the most limited view of the Posix standard and doesn't implement d_type.
							Path::PathString8 temp8; 
							EA::StdC::Strlcpy<Path::PathString8, const wchar_t>(temp8, pEntryFindData->mDirectoryPath); // Translate W to 8.
							temp8 += dirEntry->d_name;
							pEntryFindData->mbIsDirectory = Directory::Exists(temp8.c_str());
						#else
							pEntryFindData->mbIsDirectory = (dirEntry->d_type == DT_DIR);
						#endif

						if(pEntryFindData->mbIsDirectory)
							Path::EnsureTrailingSeparator(pEntryFindData->mName, kMaxPathLength);

						// mCreationTime / mModificationTime / mSize
						pEntryFindData->mCreationTime     = 0;
						pEntryFindData->mModificationTime = 0;
						pEntryFindData->mSize             = 0;

						if(pEntryFindData->mbReadFileStat)
						{
							Path::PathString8 path8;
							ConvertPath(path8, pEntryFindData->mDirectoryPath);
							path8 += dirEntry->d_name;

							UpdateEntry(path8.c_str(), pEntryFindData);
						}

						// Clean up and return
						return pEntryFindData;
					}
				}

				#if EAIO_READDIR_R_PRESENT
				  if(readdir_r(dir, dirEntryData, &dirEntry) != 0)
				#else
				  if((dirEntry = readdir(dir)) == NULL)
				#endif
				{
					return NULL;
				}
			}  
		}
		 
		return NULL;
	}


	EAIO_API void EntryFindFinish(EntryFindData* pEntryFindData)
	{
		using namespace Internal;

		if(pEntryFindData)
		{
			#ifdef EA_PLATFORM_ANDROID
				if( pEntryFindData->mPlatformHandle == reinterpret_cast<uintptr_t>(&AndroidAppBundleEntryIndicator))
				{
					AndroidEntryFindFinish(pEntryFindData);
					return;
				}
			#endif

			DIR* dir = (DIR*)pEntryFindData->mPlatformHandle;

			if(dir)
				closedir(dir);

			if(pEntryFindData->mbIsAllocated)
				Free(EA::IO::GetAllocator(), pEntryFindData);
		}
	}

#elif defined(EA_PLATFORM_SONY)

	struct EntryFindDataSony
	{
		void Init(int dirDescr, int bufferSize)
		{
			mBufferSize = bufferSize;
			mBytesRead = 0;
			mCurrPos = 0;
			mDirDescriptor = dirDescr;
			EA_ASSERT(mDirDescriptor >= 0);
			mBuffer = static_cast<char*>(GetAllocator()->Alloc(mBufferSize, EAIO_ALLOC_PREFIX "EAFileDirectory/EntryFindData/EntryBuffer", EA::Allocator::MEM_TEMP));
		}

		void DeInit()
		{
			if (mBuffer != NULL)
			{
				GetAllocator()->Free(mBuffer);
				mBuffer = NULL;
			}
		}

		// Reads ahead a number of entries
		bool ReadMultipleEntries()
		{
			mBytesRead = sceKernelGetdents(mDirDescriptor, mBuffer, mBufferSize);
			mCurrPos = 0;
			return mBytesRead > 0;
		}

		SceKernelDirent* GetNextEntry()
		{
			while(mCurrPos < mBytesRead)
			{
				SceKernelDirent* dirEnt = reinterpret_cast<SceKernelDirent*>(mBuffer + mCurrPos);
				mCurrPos += dirEnt->d_reclen;

				if(dirEnt->d_fileno != 0) // If not deleted...
					return dirEnt;
			} 

			return NULL;
		}

		int   mBufferSize; // The minimum buffer size depends on the file-system block size and for now we determine this at runtime
		int   mBytesRead;
		int   mCurrPos;
		char* mBuffer;
		int   mDirDescriptor;
	};

	EAIO_API EntryFindData* EntryFindFirst(const wchar_t* pDirectoryPath, const wchar_t* pFilterPattern, EntryFindData* pEntryFindData)
	{
		using namespace Internal;

		EA_COMPILETIME_ASSERT(sizeof(EntryFindDataSony) <= sizeof(pEntryFindData->mPlatformData));

		if (pDirectoryPath[0] || (!pFilterPattern || pFilterPattern[0]))
		{
			Path::PathString8  directory8;

			// Measure how many UTF-8 chars we'll need (EASTL factors-in a hidden + 1 for NULL terminator)
			size_t nCharsNeeded = EA::StdC::Strlcpy((char*)NULL, pDirectoryPath, 0);
			directory8.resize(nCharsNeeded);
			EA::StdC::Strlcpy(&directory8[0], pDirectoryPath, nCharsNeeded + 1);

			int flags = SCE_KERNEL_O_DIRECTORY | SCE_KERNEL_O_RDONLY;
			int dirDescr = sceKernelOpen(directory8.c_str(), flags, SCE_KERNEL_S_IRUSR);
			if (dirDescr < 0)
				return NULL;

			// Get the file stat for the block size info
			SceKernelStat st;
			if (sceKernelFstat(dirDescr, &st) != SCE_OK)
				return NULL;

			if (!pEntryFindData)
			{
				pEntryFindData = Allocate<EntryFindData>(IO::GetAllocator(), EAIO_ALLOC_PREFIX "EAFileDirectory/EntryFindData");
				pEntryFindData->mbIsAllocated = true;
			}

			// In case the user allocated the EntryFindData we still need to allocate our entry buffer
			EntryFindDataSony *entryDataKettle = reinterpret_cast<EntryFindDataSony*>(pEntryFindData->mPlatformData);
			EA_ASSERT(EA::StdC::IsPowerOf2<uint32_t>(st.st_blksize));
			const uint32_t bufferSize = EA::StdC::AlignUp<uint32_t>(MAX(st.st_size, st.st_blksize), st.st_blksize);
			entryDataKettle->Init(dirDescr, bufferSize);

			EA::StdC::Strlcpy(pEntryFindData->mDirectoryPath, pDirectoryPath, kMaxPathLength);
			FilterEntries(pEntryFindData, pFilterPattern);

			if (EntryFindNext(pEntryFindData) != NULL)
			{
				return pEntryFindData;
			}
			
			// Dealocate our structs
			EntryFindFinish(pEntryFindData);
		}

		return NULL;
	}


	EAIO_API EntryFindData* EntryFindNext(EntryFindData* pEntryFindData)
	{
		if (pEntryFindData)
		{
			EntryFindDataSony *entryDataKettle = reinterpret_cast<EntryFindDataSony*>(pEntryFindData->mPlatformData);
			
			const int kMatchFlags = kFNMNone;
			wchar_t entryName[kMaxPathLength];

			for (;;)
			{
				SceKernelDirent *dirEntry = entryDataKettle->GetNextEntry();

				if (dirEntry == NULL)
				{
					if (!entryDataKettle->ReadMultipleEntries())
					{
						// We reached the end
						return NULL;
					}

					dirEntry = entryDataKettle->GetNextEntry();

					EA_ASSERT(dirEntry); // We should have a new batch of entries
					if (dirEntry == NULL)
						return NULL;
				}

				EA::StdC::Strlcpy(entryName, dirEntry->d_name, kMaxPathLength, kLengthNull);

				if ((pEntryFindData->mEntryFilterPattern[0] == 0) || 
					FnMatch(pEntryFindData->mEntryFilterPattern, entryName, kMatchFlags))
				{
					EA::StdC::Strlcpy(pEntryFindData->mName, entryName, kMaxPathLength);
					
					pEntryFindData->mbIsDirectory = (dirEntry->d_type == SCE_KERNEL_DT_DIR);
	
					if( pEntryFindData->mbIsDirectory)
						Path::EnsureTrailingSeparator(pEntryFindData->mName, kMaxPathLength);

					// mCreationTime / mModificationTime / mSize
					pEntryFindData->mCreationTime     = 0;
					pEntryFindData->mModificationTime = 0;
					pEntryFindData->mSize             = 0;

					if (pEntryFindData->mbReadFileStat)
					{
						Path::PathString8 path8;
						ConvertPath(path8, pEntryFindData->mDirectoryPath);
						path8 += dirEntry->d_name;

						SceKernelStat tempStat;
						const int result = sceKernelStat(path8.c_str(), &tempStat);

						EA_ASSERT(result == SCE_OK);
						if (result == SCE_OK)
						{
							pEntryFindData->mCreationTime     = (time_t)tempStat.st_ctime;
							pEntryFindData->mModificationTime = (time_t)tempStat.st_mtime;
							pEntryFindData->mSize             = (uint64_t)tempStat.st_size;
						}
					}

					return pEntryFindData;
				}
			}  
		}

		return NULL;
	}


	EAIO_API void EntryFindFinish(EntryFindData* pEntryFindData)
	{
		using namespace Internal;

		if (pEntryFindData)
		{
			EntryFindDataSony *entryDataKettle = reinterpret_cast<EntryFindDataSony*>(pEntryFindData->mPlatformData);
			
			EA_ASSERT(entryDataKettle->mDirDescriptor >= 0);
			if (entryDataKettle->mDirDescriptor >= 0)
			{
				int ret = sceKernelClose(entryDataKettle->mDirDescriptor);
				EA_ASSERT(ret == SCE_OK); EA_UNUSED(ret);
			}

			// Free the buffer
			entryDataKettle->DeInit();

			if (pEntryFindData->mbIsAllocated)
				Free(EA::IO::GetAllocator(), pEntryFindData);
		}
	}

#else

	EAIO_API EntryFindData* EntryFindFirst(const wchar_t* /*pDirectoryPath*/, const wchar_t* /*pFilterPattern*/, EntryFindData* /*pEntryFindData*/)
	{
		// To do.
		return NULL;
	}

	EAIO_API EntryFindData* EntryFindNext(EntryFindData* /*pEntryFindData*/)
	{
		// To do.
		return NULL;
	}

	EAIO_API void EntryFindFinish(EntryFindData* /*pEntryFindData*/)
	{
		// To do.
	}

#endif // defined(<platform>)

	template<typename T>
	EntryFindData* EntryFindFirstImpl(const T* pDirectoryPath, const T* pFilterPattern, EntryFindData* pEntryFindData)
	{
		// TODO - Most part of this code could be merged with ReadRecursiveImpl
		wchar_t directoryPathBuffer[kMaxPathLength];
		wchar_t filterPatternBuffer[kMaxPathLength];
		wchar_t *pConvertedFilterPattern = NULL;

		int directoryPathConversionResult = EA::StdC::Strlcpy(directoryPathBuffer, pDirectoryPath, kMaxPathLength);
		bool directoryPathConversionSucceeded = directoryPathConversionResult >= 0 && directoryPathConversionResult < kMaxPathLength;
		
		int filterPatternConversionResult;
		bool filterPatternConversionSucceeded = true;

		if (pFilterPattern)
		{
			filterPatternConversionResult = EA::StdC::Strlcpy(filterPatternBuffer, pFilterPattern, kMaxPathLength);
			filterPatternConversionSucceeded = filterPatternConversionResult >= 0 && filterPatternConversionResult < kMaxPathLength;
			pConvertedFilterPattern = filterPatternBuffer;
		}

		if (directoryPathConversionSucceeded && filterPatternConversionSucceeded)
		{
			return EntryFindFirst(directoryPathBuffer, pConvertedFilterPattern, pEntryFindData);
		}

		return NULL;
	}

	EntryFindData* EntryFindFirst(const char*   pDirectoryPath, const char*  pFilterPattern, EntryFindData* pEntryFindData)
	{
		return EntryFindFirstImpl<char>(pDirectoryPath, pFilterPattern, pEntryFindData);
	}

	#if EA_WCHAR_UNIQUE || EA_WCHAR_SIZE == 4
		EntryFindData* EntryFindFirst(const char16_t*  pDirectoryPath, const char16_t* pFilterPattern, EntryFindData* pEntryFindData)
		{
			return EntryFindFirstImpl<char16_t>(pDirectoryPath, pFilterPattern, pEntryFindData);
		}
	#endif

	#if EA_WCHAR_UNIQUE || EA_WCHAR_SIZE == 2
		EntryFindData* EntryFindFirst(const char32_t*  pDirectoryPath, const char32_t* pFilterPattern, EntryFindData* pEntryFindData)
		{
			return EntryFindFirstImpl<char32_t>(pDirectoryPath, pFilterPattern, pEntryFindData);
		}
	#endif

} // namespace IO


} // namespace EA















