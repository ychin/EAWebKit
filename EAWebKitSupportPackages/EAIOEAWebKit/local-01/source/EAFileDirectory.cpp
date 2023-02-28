/*
Copyright (C) 2003,2009-2011, 2013, 2014, 2015 Electronic Arts, Inc.  All rights reserved.

Redistribution and use in source and binary forms, with or without
modification, are permitted provided that the following conditions
are met:

1.  Redistributions of source code must retain the above copyright
    notice, this list of conditions and the following disclaimer.
2.  Redistributions in binary form must reproduce the above copyright
    notice, this list of conditions and the following disclaimer in the
    documentation and/or other materials provided with the distribution.
3.  Neither the name of Electronic Arts, Inc. ("EA") nor the names of
    its contributors may be used to endorse or promote products derived
    from this software without specific prior written permission.

THIS SOFTWARE IS PROVIDED BY ELECTRONIC ARTS AND ITS CONTRIBUTORS "AS IS" AND ANY
EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE IMPLIED
WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE ARE
DISCLAIMED. IN NO EVENT SHALL ELECTRONIC ARTS OR ITS CONTRIBUTORS BE LIABLE FOR ANY
DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES
(INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES;
LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND
ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT
(INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE OF
THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
*/

/////////////////////////////////////////////////////////////////////////////
// EAFileDirectory.cpp
//
// Created by Paul Pedriana - 2003
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
#include <string.h>
#include EA_ASSERT_HEADER

#if   defined(EA_PLATFORM_WINDOWS)
    #include <windows.h>
#elif defined (EA_PLATFORM_UNIX)
    #include <sys/types.h>
    #include <dirent.h>
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
        T* const pT = (T*)pAllocator->Alloc(sizeof(T), pName, 0, 0);
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
}



///////////////////////////////////////////////////////////////////////////////
// Read
//
size_t DirectoryIterator::Read(const char16_t* pDirectory, EntryList& entryList, 
                             const char16_t* pFilterPattern, int nDirectoryEntryFlags, 
                             size_t maxResultCount)
{
    EntryFindData entryFindData, *pEntryFindData;
    size_t        resultCount = 0;

    #if EASTL_NAME_ENABLED // If the EntryList doesn't have a unique name, we give it one here.
        if(entryList.get_allocator().get_name() && !strcmp(EASTL_LIST_DEFAULT_NAME, entryList.get_allocator().get_name()))
            entryList.get_allocator().set_name(ENTRYLIST_NAME);
    #endif

    // Iterate entries.
    for(pEntryFindData = EntryFindFirst(pDirectory, pFilterPattern, &entryFindData); pEntryFindData && (resultCount < maxResultCount); )
    {
        if(!StrEq16(pEntryFindData->mName, EA_DIRECTORY_CURRENT_16) && // If it is neither "./" nor "../"
           !StrEq16(pEntryFindData->mName, EA_DIRECTORY_PARENT_16))
        {
            if(pEntryFindData->mbIsDirectory)
            {
                if(nDirectoryEntryFlags & kDirectoryEntryDirectory)
                {
                    resultCount++;
                    entryList.push_back();
                    entryList.back().mType  = kDirectoryEntryDirectory;
                    entryList.back().msName = pEntryFindData->mName;
                }
            }
            else
            {
                if(nDirectoryEntryFlags & kDirectoryEntryFile)
                {
                    resultCount++;
                    entryList.push_back();
                    entryList.back().mType  = kDirectoryEntryFile;
                    entryList.back().msName = pEntryFindData->mName;
                }
            }
        }

        if(!EntryFindNext(pEntryFindData))
        {
            EntryFindFinish(pEntryFindData);
            break;
        }
    }

    if(pEntryFindData)
    {
        if((nDirectoryEntryFlags & kDirectoryEntryCurrent) && (resultCount < maxResultCount))
        {
            resultCount++;
            entryList.push_front();
            entryList.front().mType  = kDirectoryEntryDirectory;
            entryList.front().msName = EA_DIRECTORY_CURRENT_16;
        }

        if((nDirectoryEntryFlags & kDirectoryEntryParent) && (resultCount < maxResultCount))
        {
            // To do: We don't want to do this if the directory is a root directory.
            resultCount++;
            entryList.push_front();
            entryList.front().mType  = kDirectoryEntryDirectory;
            entryList.front().msName = EA_DIRECTORY_PARENT_16;
        }
    }

    return resultCount;
}



///////////////////////////////////////////////////////////////////////////////
// ReadRecursive
//
size_t DirectoryIterator::ReadRecursive(const char16_t* pBaseDirectory, EntryList& entryList, 
                                      const char16_t* pFilterPattern, int nEntryTypeFlags, 
                                      bool bIncludeBaseDirectoryInSearch, bool bFullPaths, 
                                      size_t maxResultCount)
{
    EA::IO::Path::PathString16  pathTemp;
    //char16_t pathTemp[kMaxPathLength];

    if(mnRecursionIndex++ == 0) // If being called for the first time...
    {
        #if EASTL_NAME_ENABLED // If the EntryList doesn't have a unique name, we give it one here.
            if(entryList.get_allocator().get_name() && !strcmp(EASTL_LIST_DEFAULT_NAME, entryList.get_allocator().get_name()))
                entryList.get_allocator().set_name(ENTRYLIST_NAME);
        #endif

        mnListSize           = 0;
        mpBaseDirectory      = pBaseDirectory;
        mBaseDirectoryLength = (eastl_size_t)EAIOStrlen16(pBaseDirectory);
        if(!mBaseDirectoryLength || !IsFilePathSeparator(pBaseDirectory[mBaseDirectoryLength - 1]))
            mBaseDirectoryLength++;
    }

    if((nEntryTypeFlags & kDirectoryEntryFile) && 
       (bIncludeBaseDirectoryInSearch || (mnRecursionIndex > 1)) && 
       (mnListSize < maxResultCount))
    {
        // Add all files in the current directory into the list, using the filter pattern.
        const size_t additionCount = Read(pBaseDirectory, entryList, pFilterPattern, kDirectoryEntryFile, maxResultCount - mnListSize);

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
                entry.msName.insert(directoryEnd, 1, kFilePathSeparator16);

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
        Read(pBaseDirectory, entryListTemp, NULL, kDirectoryEntryDirectory, kMaxEntryCountDefault);

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
            ReadRecursive(pathTemp.c_str(), entryList, pFilterPattern, nEntryTypeFlags, true, bFullPaths, maxResultCount);
        }
    }

    mnRecursionIndex--;

    return mnListSize;
}


#undef ENTRYLIST_NAME


////////////////////////////////////////////////////////////////////////////
// EntryFindFirst
////////////////////////////////////////////////////////////////////////////

////////////////////////////////////////////////////////////////////////////
// EntryFindFirst / EntryFindNext / EntryFindFinish
//
#if defined(EA_PLATFORM_WINDOWS) 

    EAIO_API EntryFindData* EntryFindFirst(const char16_t* pDirectoryPath, const char16_t* pFilterPattern, EntryFindData* pEntryFindData)
    {
        using namespace Internal;

        const char16_t pAny[] = { '*', 0 };
        Path::PathString16 pPathSpecification;
        if(pDirectoryPath)
            pPathSpecification += pDirectoryPath;
        Path::Append(pPathSpecification, pFilterPattern ? pFilterPattern : pAny);

        if(!pEntryFindData)
        {
            pEntryFindData = Allocate<EntryFindData>(IO::GetAllocator(), EAIO_ALLOC_PREFIX "EAFileDirectory/EntryFindData");
            pEntryFindData->mbIsAllocated = true;
        }

        #if defined(EA_PLATFORM_WINDOWS) // If wide character functions are supported...
            WIN32_FIND_DATAW win32FindDataW;
            HANDLE hFindFile = FindFirstFileW(reinterpret_cast<LPCWSTR>(pPathSpecification.c_str()), &win32FindDataW);

            if(hFindFile != INVALID_HANDLE_VALUE)
            {
                EAIOStrlcpy16(pEntryFindData->mName, reinterpret_cast<const char16_t *>(win32FindDataW.cFileName), kMaxPathLength);

                pEntryFindData->mbIsDirectory  = (win32FindDataW.dwFileAttributes & FILE_ATTRIBUTE_DIRECTORY) != 0;
                if(pEntryFindData->mbIsDirectory)
                    Path::EnsureTrailingSeparator(pEntryFindData->mName, kMaxPathLength);

                if(pDirectoryPath)
                    EAIOStrlcpy16(pEntryFindData->mDirectoryPath, pDirectoryPath, kMaxPathLength);
                else
                    pEntryFindData->mDirectoryPath[0] = 0;

                if(pFilterPattern)
                    EAIOStrlcpy16(pEntryFindData->mEntryFilterPattern, pFilterPattern, kMaxPathLength);
                else
                    pEntryFindData->mEntryFilterPattern[0] = 0;

                pEntryFindData->mPlatformHandle = (uintptr_t)hFindFile;

                return pEntryFindData;
            }
        #else
            Path::PathString8 directory8;
            // Measure how many UTF-8 chars we'll need (EASTL factors-in a hidden + 1 for NULL terminator)
            size_t nCharsNeeded = StrlcpyUTF16ToUTF8(NULL, 0, pPathSpecification.c_str());
            directory8.resize(nCharsNeeded);
            StrlcpyUTF16ToUTF8(&directory8[0], nCharsNeeded + 1, pPathSpecification.c_str());
            
            WIN32_FIND_DATAA win32FindDataA;
            HANDLE hFindFile = FindFirstFileA(directory8.c_str(), &win32FindDataA);

            if(hFindFile != INVALID_HANDLE_VALUE)
            {
                StrlcpyUTF8ToUTF16(pEntryFindData->mName, kMaxPathLength, win32FindDataA.cFileName);

                pEntryFindData->mbIsDirectory = (win32FindDataA.dwFileAttributes & FILE_ATTRIBUTE_DIRECTORY) != 0;
                if(pEntryFindData->mbIsDirectory)
                    Path::EnsureTrailingSeparator(pEntryFindData->mName, kMaxPathLength);

                if(pDirectoryPath)
                    EAIOStrlcpy16(pEntryFindData->mDirectoryPath, pDirectoryPath, kMaxPathLength);
                else
                    pEntryFindData->mDirectoryPath[0] = 0;

                if(pFilterPattern)
                    EAIOStrlcpy16(pEntryFindData->mEntryFilterPattern, pFilterPattern, kMaxPathLength);
                else
                    pEntryFindData->mEntryFilterPattern[0] = 0;

                pEntryFindData->mPlatformHandle = (uintptr_t)hFindFile;

                return pEntryFindData;
            }
        #endif

        if (pEntryFindData->mbIsAllocated)
            Free(EA::IO::GetAllocator(), pEntryFindData);

        return NULL;
    }


    EAIO_API EntryFindData* EntryFindNext(EntryFindData* pEntryFindData)
    {
        if(pEntryFindData)
        {
            HANDLE hFindFile = (HANDLE)pEntryFindData->mPlatformHandle;

            #if defined(EA_PLATFORM_WINDOWS) // If wide character functions are supported...
                WIN32_FIND_DATAW win32FindDataW;

                if(FindNextFileW(hFindFile, &win32FindDataW))
                {
                    EAIOStrlcpy16(pEntryFindData->mName, reinterpret_cast<const char16_t *>(win32FindDataW.cFileName), kMaxPathLength);

                    pEntryFindData->mbIsDirectory = (win32FindDataW.dwFileAttributes & FILE_ATTRIBUTE_DIRECTORY) != 0;
                    if(pEntryFindData->mbIsDirectory)
                        Path::EnsureTrailingSeparator(pEntryFindData->mName, kMaxPathLength);

                    return pEntryFindData;
                }
            #else
                WIN32_FIND_DATAA win32FindDataA;

                if(FindNextFileA(hFindFile, &win32FindDataA))
                {
                    StrlcpyUTF8ToUTF16(pEntryFindData->mName, kMaxPathLength, win32FindDataA.cFileName);

                    pEntryFindData->mbIsDirectory = (win32FindDataA.dwFileAttributes & FILE_ATTRIBUTE_DIRECTORY) != 0;
                    if(pEntryFindData->mbIsDirectory)
                        Path::EnsureTrailingSeparator(pEntryFindData->mName, kMaxPathLength);

                    return pEntryFindData;
                }

                // Problem: Not all platforms always return "./" and "../" directories.
                // We need to provide those if they aren't present and if they are
                // appropriate ("../" shouldn't be present for root directories).
            #endif
        }
        return NULL;
    }


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


#elif defined(EA_PLATFORM_UNIX) 

    EAIO_API EntryFindData* EntryFindFirst(const char16_t* pDirectoryPath, const char16_t* pFilterPattern, EntryFindData* pEntryFindData)
    {
        using namespace Internal;

        Path::PathString16 directoryTemp16;
        Path::PathString8  directory8;

        if(pDirectoryPath)
            directoryTemp16 += pDirectoryPath;

        // Measure how many UTF-8 chars we'll need (EASTL factors-in a hidden + 1 for NULL terminator)
        size_t nCharsNeeded = StrlcpyUTF16ToUTF8(NULL, 0, directoryTemp16.c_str());
        directory8.resize(nCharsNeeded);
        StrlcpyUTF16ToUTF8(&directory8[0], nCharsNeeded + 1, directoryTemp16.c_str());

        DIR* const dir = opendir(directory8.c_str());
        if(!dir)
            return NULL;
         
        // Follow windows semantics: don't distinguish between an empty directory and not finding anything.
        dirent   dirEntryData;
        dirent*  dirEntry;
        bool     bPatternMatch = false;
        char16_t entryName[kMaxPathLength]; // match pEntryFindData->mName

        do { // Run a loop so we can filter out any "." and ".." entries that might be present and entries that don't match the input pattern.
            #if defined(EA_PLATFORM_UNIX)
                if((readdir_r(dir, &dirEntryData, &dirEntry) != 0) || !dirEntry)
                {
                    closedir(dir);
                    return NULL; // If no directory entry matches the user-specified pattern, we'll usually be exiting via this line due to (dirEntry == NULL).
                }
            #else
                (void)dirEntryData;
                dirEntry = readdir(dir);

                if(!dirEntry)
                {
                    closedir(dir);
                    return NULL; // If no directory entry matches the user-specified pattern, we'll usually be exiting via this line due to (dirEntry == NULL).
                }
            #endif

            if((strcmp(dirEntry->d_name, ".") == 0) ||
               (strcmp(dirEntry->d_name, "..") == 0))
            {
                bPatternMatch = false;
            }
            else
            {
                StrlcpyUTF8ToUTF16(entryName, kMaxPathLength, dirEntry->d_name, kLengthNull);
                bPatternMatch = !pFilterPattern || FnMatch(pFilterPattern, entryName, kFNMNone);
            }
        } while(!bPatternMatch);

        if(!pEntryFindData)
        {
            pEntryFindData = Allocate<EntryFindData>(IO::GetAllocator(), EAIO_ALLOC_PREFIX "EAFileDirectory/EntryFindData");
            pEntryFindData->mbIsAllocated = true;
        }

        EAIOStrlcpy16(pEntryFindData->mName, entryName, kMaxPathLength);

        #if defined(__CYGWIN__) // Cygwin chooses to take the most limited view of the Posix standard and doesn't implement d_type.
            directory8 += dirEntry->d_name;
            pEntryFindData->mbIsDirectory = Directory::Exists(directory8.c_str());
        #else
            pEntryFindData->mbIsDirectory = (dirEntry->d_type == DT_DIR);
        #endif

        if(pEntryFindData->mbIsDirectory)
            Path::EnsureTrailingSeparator(pEntryFindData->mName, kMaxPathLength);

        EAIOStrlcpy16(pEntryFindData->mDirectoryPath, pDirectoryPath, kMaxPathLength);
        Path::EnsureTrailingSeparator(pEntryFindData->mDirectoryPath, kMaxPathLength);

        if(pFilterPattern)
            EAIOStrlcpy16(pEntryFindData->mEntryFilterPattern, pFilterPattern, kMaxPathLength);
        else
        {
            pEntryFindData->mEntryFilterPattern[0] = '*';
            pEntryFindData->mEntryFilterPattern[1] = 0;
        }

        pEntryFindData->mPlatformHandle = (uintptr_t) dir;
         
        return pEntryFindData;
    }


    EAIO_API EntryFindData* EntryFindNext(EntryFindData* pEntryFindData)
    {
        if(pEntryFindData)
        {
        	// Unix defines DIR as an opaque pointer whose type is undefined. It would be useful to verify that in code here.
            // EA_COMPILETIME_ASSERT(sizeof(DIR) <= sizeof(pEntryFindData->mPlatformData));
            DIR* const dir = (DIR*)pEntryFindData->mPlatformHandle;
              
            dirent  dirEntryData;
            dirent* dirEntry;

            #if defined(EA_PLATFORM_UNIX)
                if(readdir_r(dir, &dirEntryData, &dirEntry) != 0)
                {
                    // closedir(dir); // We don't need to do this, as it will be called in EntryFindFinish after we return NULL.
                    // dir = NULL;
                    return NULL; // If no directory entry matches the user-specified pattern, we'll usually be exiting via this line due to (dirEntry == NULL).
                }
            #else
                (void)dirEntryData;
                dirEntry = readdir(dir);

                if(!dirEntry)
                {
                    // closedir(dir); // We don't need to do this, as it will be called in EntryFindFinish after we return NULL.
                    // dir = NULL;
                    return NULL;
                }
            #endif

            char16_t entryName[kMaxPathLength]; // match pEntryFindData->mName
              
            while(dirEntry)
            {     
                StrlcpyUTF8ToUTF16(entryName, kMaxPathLength, dirEntry->d_name, kLengthNull);

                if((pEntryFindData->mEntryFilterPattern[0] == 0) || 
                    FnMatch(pEntryFindData->mEntryFilterPattern, entryName, kFNMNone))
                {
                    EAIOStrlcpy16(pEntryFindData->mName, entryName, kMaxPathLength);

                    #if defined(__CYGWIN__) // Cygwin chooses to take the most limited view of the Posix standard and doesn't implement d_type.
                        Path::PathString16 temp16(pEntryFindData->mDirectoryPath);
                        temp16 += entryName;
                        pEntryFindData->mbIsDirectory = Directory::Exists(temp16.c_str());
                    #else
                        pEntryFindData->mbIsDirectory = (dirEntry->d_type == DT_DIR);
                    #endif

                    if(pEntryFindData->mbIsDirectory)
                        Path::EnsureTrailingSeparator(pEntryFindData->mName, kMaxPathLength);

                    return pEntryFindData;
                }
                
                #if defined(EA_PLATFORM_UNIX)
                    if(readdir_r(dir, &dirEntryData, &dirEntry) != 0)
                    {
                        // closedir(dir); // We don't need to do this, as it will be called in EntryFindFinish after we return NULL.
                        // dir = NULL;
                        return NULL; // If no directory entry matches the user-specified pattern, we'll usually be exiting via this line due to (dirEntry == NULL).
                    }
                #else
                    (void)dirEntryData;
                    dirEntry = readdir(dir);

                    if(!dirEntry)
                    {
                        // closedir(dir); // We don't need to do this, as it will be called in EntryFindFinish after we return NULL.
                        // dir = NULL;
                        return NULL;
                    }
                #endif
            }     
        }
         
        return NULL;
    }

    EAIO_API void EntryFindFinish(EntryFindData* pEntryFindData)
    {
        using namespace Internal;

        if(pEntryFindData)
        {
            DIR* dir = (DIR*)pEntryFindData->mPlatformHandle;

            if(dir)
                closedir(dir);

            if(pEntryFindData->mbIsAllocated)
                Free(EA::IO::GetAllocator(), pEntryFindData);
        }
    }

#else

    // If it doesn't have anything built-in; you need to implement 
    // your own enumeration mechanism, such as by putting a file in each 
    // directory listing the directory's contents.

    EAIO_API EntryFindData* EntryFindFirst(const char16_t* /*pDirectoryPath*/, const char16_t* /*pFilterPattern*/, EntryFindData* /*pEntryFindData*/)
    {
        // To do.
        return NULL;
    }

    EAIO_API EntryFindData* EntryFindNext(EntryFindData* /*pEntryFindData*/)
    {
        // To do.
        return nullptr;
    }

    EAIO_API void EntryFindFinish(EntryFindData* /*pEntryFindData*/)
    {
        // To do.
    }

#endif // defined(<platform>)



} // namespace IO


} // namespace EA















