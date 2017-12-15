/*
Copyright (C) 2008, 2009, 2010, 2011, 2012, 2013, 2014 Electronic Arts, Inc.  All rights reserved.

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

///////////////////////////////////////////////////////////////////////////////
// EAWebKitFileSystem.cpp
// By Paul Pedriana
///////////////////////////////////////////////////////////////////////////////

#include "config.h"
#include <EAWebKit/EAWebKitFileSystem.h>
#include <internal/include/EAWebKitAssert.h>
#include <string.h>

//EAWebKitTODO - Investigate this warning project wide - http://stackoverflow.com/questions/14363929/vs2012-c-warning-c4005-useheader-macro-redefinition 
/*
1>  EAWebKitFileSystem.cpp
1>c:\program files (x86)\microsoft sdks\windows\v7.0a\include\sal_supp.h(57): warning C4005: '__useHeader' : macro redefinition
1>          C:\Program Files (x86)\Microsoft Visual Studio 11.0\VC\INCLUDE\sal.h(2872) : see previous definition of '__useHeader'
1>c:\program files (x86)\microsoft sdks\windows\v7.0a\include\specstrings_supp.h(77): warning C4005: '__on_failure' : macro redefinition
1>          C:\Program Files (x86)\Microsoft Visual Studio 11.0\VC\INCLUDE\sal.h(2882) : see previous definition of '__on_failure'
*/
    #include <stdio.h>

    #pragma warning(push, 1)
    #include EAWEBKIT_PLATFORM_HEADER
    #include <direct.h>
    #include <sys/stat.h>
	#include <sys/utime.h>
	#pragma warning(pop)

// EA_PLATFORM_UNIX is defined when EA_PLATFORM_OSX is defined.

	#ifndef INVALID_FILE_ATTRIBUTES
		#define INVALID_FILE_ATTRIBUTES ((DWORD)-1)
	#endif


namespace EA
{

namespace WebKit
{

FileSystem*       spFileSystem = NULL;

void SetFileSystem(FileSystem* pFileSystem)
{
    EAW_ASSERT_MSG(!spFileSystem, "FileSystem already exists. Do not call SetFileSystem more than once!");
	spFileSystem = pFileSystem;
}


FileSystem* GetFileSystem()
{
	if(!spFileSystem)
	{
		static FileSystemDefault defaultFileSystem;
		spFileSystem = &defaultFileSystem;
	}

	EAW_ASSERT_MSG(spFileSystem, "File system does not exist");
	return spFileSystem;
}



///////////////////////////////////////////////////////////////////////////////
// FileSystemDefault
///////////////////////////////////////////////////////////////////////////////

struct FileInfo
{
    FILE* mpFile;
    bool  mbOpen;

    FileInfo() : mpFile(NULL), mbOpen(false) { }
};


FileSystem::FileObject FileSystemDefault::CreateFileObject()
{
    FileInfo* pFileInfo = new FileInfo;

    return (uintptr_t)pFileInfo;
}


void FileSystemDefault::DestroyFileObject(FileObject fileObject)
{
    FileInfo* pFileInfo = reinterpret_cast<FileInfo*>(fileObject);

    delete pFileInfo; 
}


bool FileSystemDefault::OpenFile(FileObject fileObject, const char* path, int openFlags, int createDisposition)
{
    FileInfo* pFileInfo = reinterpret_cast<FileInfo*>(fileObject);

    EAW_ASSERT(!pFileInfo->mbOpen);
	if(path && *path)
	{
		//Note by Arpit Baldeva - 
		//Paul Pedriana's real fix for the "\n" in the EAWebKit cookies file not working as intended.
		//Add binary flag 
		//http://msdn.microsoft.com/en-us/library/yeby3zcb%28VS.80%29.aspx
		//b Open in binary (untranslated) mode; translations involving carriage-return and linefeed characters are suppressed. 

	#ifdef _MSC_VER
		pFileInfo->mpFile = fopen(path, openFlags & kWrite ? "wb" : "rb");
	#else
		pFileInfo->mpFile = fopen(path, openFlags & kWrite ? "w" : "r");
	#endif
	}

    if(pFileInfo->mpFile)
        pFileInfo->mbOpen = true;

    return pFileInfo->mbOpen;
}


/*
EAIO_API bool MakeTempPathName(char8_t* pPath, const char8_t* pDirectory, const char8_t* pFileName, const char8_t* pExtension, uint32_t nDestPathLength)
{
    // User must allocate space for the resulting temp path.
    if(pPath)
    {
        static const char8_t pFileNameDefault[]  = { 't', 'e', 'm', 'p', 0 };
        static const char8_t pExtensionDefault[] = { '.', 't', 'm', 'p', 0 };

        time_t nTime = time(NULL);

        char8_t tempPath[kMaxPathLength];

        if(!pFileName)
            pFileName = pFileNameDefault;

        if(!pExtension)
            pExtension = pExtensionDefault;

        if(!pDirectory)
        {
            if(!GetTempDirectory(tempPath))
                return false;
            pDirectory = tempPath;
        }

        for(size_t i = 0; i < 5000; i++, nTime--)
        {
            char8_t buffer[16];

            Path::PathString8 tempFilePath(pDirectory);
            Path::Append(tempFilePath, pFileName);

            tempFilePath.operator+=(EAIOItoa8((uint32_t)nTime, buffer));
            tempFilePath.operator+=(pExtension);

            uint32_t nSrcPathLength = (uint32_t)tempFilePath.length();
            if (nSrcPathLength > nDestPathLength)
                break;

            EAIOStrlcpy8(pPath, tempFilePath.c_str(), nDestPathLength);

            FileStream fileStream(pPath);
            if(fileStream.Open(kAccessFlagReadWrite, kCDCreateNew))
            {
                fileStream.Close();
                return true;
            }
        }
    }

    return false;
}
*/


FileSystem::FileObject FileSystemDefault::OpenTempFile(const char* prefix, char* path)
{
    // To do: Make generic version of this (EAIO code above):
    //
    // char path[EA::IO::kMaxPathLength];
    // 
    // if(EA::IO::MakeTempPathName(path, NULL, "EATemp", ".tmp", EA::IO::kMaxPathLength))
    // {
    //     EA::IO::FileStream* pFileStream = new EA::IO::FileStream(path);
    // 
    //     if(pFileStream->Open(EA::IO::kAccessFlagReadWrite, EA::IO::kCDCreateAlways)) // Could also use kCDOpenExisting
    //         return (uintptr_t)pFileStream;
    //     else
    //         delete pFileStream;
    // }

	//Complete this and implement the unit tests.
    EAW_FAIL_MSG("FileSystemDefault::OpenTempFile: not yet completed.");

    return kFileObjectInvalid;
}


void FileSystemDefault::CloseFile(FileObject fileObject)
{
    FileInfo* pFileInfo = reinterpret_cast<FileInfo*>(fileObject);

    EAW_ASSERT(pFileInfo->mbOpen);
    if(pFileInfo->mbOpen)
	{
		fclose(pFileInfo->mpFile);
		pFileInfo->mbOpen = false;
	}
}


int64_t FileSystemDefault::ReadFile(FileObject fileObject, void* buffer, int64_t size)
{
    FileInfo* pFileInfo = reinterpret_cast<FileInfo*>(fileObject);

    EAW_ASSERT(pFileInfo->mbOpen);
    int64_t result = (int64_t)fread(buffer, 1, size, pFileInfo->mpFile);

    if((result != size) && !feof(pFileInfo->mpFile))
        result = EA::WebKit::FileSystem::kSizeTypeError;

    return result;  // result might be different from size simply if the end of file was reached.
}

int64_t FileSystemDefault::ReadFileAsync(FileObject fileObject, void* buffer, int64_t size)
{
	return ReadFile(fileObject,buffer,size);
}

bool FileSystemDefault::WriteFile(FileObject fileObject, const void* buffer, int64_t size)
{
    FileInfo* pFileInfo = reinterpret_cast<FileInfo*>(fileObject);

    const int64_t result = (int64_t)fwrite(buffer, 1, size, pFileInfo->mpFile);

    return (result == size);
}


int64_t FileSystemDefault::GetFileSize(FileObject fileObject)
{
    FileInfo* pFileInfo = reinterpret_cast<FileInfo*>(fileObject); 

	EAW_ASSERT(pFileInfo->mpFile);
	
	if(!pFileInfo->mpFile)
		return EA::WebKit::FileSystem::kSizeTypeError;

    // Save the current offset. 
    const int savedPos = ftell(pFileInfo->mpFile); 

    // Read the size. 
    fseek(pFileInfo->mpFile, 0, SEEK_END); 
    const int endPos = ftell(pFileInfo->mpFile); 

    // Restore original offset. 
    fseek(pFileInfo->mpFile, savedPos, SEEK_SET); 

    return endPos; 
}

bool FileSystemDefault::SetFileSize(FileObject fileObject, int64_t size)
{
    FileInfo* pFileInfo = reinterpret_cast<FileInfo*>(fileObject);
    
    int status = 0;

    status = fseek(pFileInfo->mpFile, (long) size, SEEK_SET); 
    SetEndOfFile(pFileInfo->mpFile);
    
    return !status ? true : false;
}

int64_t FileSystemDefault::GetFilePosition(FileObject fileObject)
{
    FileInfo* pFileInfo = reinterpret_cast<FileInfo*>(fileObject);
	EAW_ASSERT(pFileInfo->mpFile);

	if(!pFileInfo->mpFile)
		return EA::WebKit::FileSystem::kSizeTypeError;
	
	return ftell(pFileInfo->mpFile);
}

bool FileSystemDefault::SetFilePosition(FileObject fileObject, int64_t position)
{
    FileInfo* pFileInfo = reinterpret_cast<FileInfo*>(fileObject);
    EAW_ASSERT(pFileInfo->mpFile);

    int status = fseek(pFileInfo->mpFile, (long) position, SEEK_SET); 
    return !status ? true : false;
}

bool FileSystemDefault::FlushFile(FileObject fileObject)
{
    FileInfo* pFileInfo = reinterpret_cast<FileInfo*>(fileObject);
    int status = fflush(pFileInfo->mpFile); 
    return !status ? true : false;
}

bool FileSystemDefault::FileExists(const char* path)
{
    // The following is copied from the EAIO package.
	if(path && *path)
	{

		const DWORD dwAttributes = ::GetFileAttributesA(path);
		return ((dwAttributes != INVALID_FILE_ATTRIBUTES) && ((dwAttributes & FILE_ATTRIBUTE_DIRECTORY) == 0));

	}

	return false;
}


bool FileSystemDefault::DirectoryExists(const char* path)
{
    // The following is copied from the EAIO package.
	
	if(path && *path)
	{
	        
			const DWORD dwAttributes = ::GetFileAttributesA(path);
		    return ((dwAttributes != INVALID_FILE_ATTRIBUTES) && ((dwAttributes & FILE_ATTRIBUTE_DIRECTORY) != 0));
	
	}
    return false;
}


bool FileSystemDefault::RemoveFile(const char* path)
{
    // The following is copied from the EAIO package.
	if(path && *path)
	{

        const BOOL bResult = ::DeleteFileA(path);
        return (bResult != 0);

	}

	return false;
}


bool FileSystemDefault::DeleteDirectory(const char* path)
{
    // The following is copied from the EAIO package.
    // This code is not smart enough to do a recursive delete, but the one in EAIO is.
	// We need to implement a recursive delete.

    // Windows doesn't like it when the directory path ends with a 
    // separator (e.g. '\') character, so we correct for this if needed.
    if(path && *path)
	{

		const size_t nStrlen = strlen(path);

		if((path[nStrlen - 1] != '/') && (path[nStrlen - 1] != '\\'))
		{
				return (RemoveDirectoryA(path) != 0);
		}

		// Else we need to remove the separator.
		char pathMod[EA::WebKit::FileSystem::kMaxPathLength];
		EAW_ASSERT_MSG(nStrlen < EA::WebKit::FileSystem::kMaxPathLength, "Directory path exceeds max path length");
		memcpy(pathMod, path, nStrlen - 1);   // Force 0 terminator in place of directory separator
		pathMod[nStrlen - 1] = 0;

		return DeleteDirectory(pathMod);  // Call ourselves recursively.
	}
	
	return false;
}


bool FileSystemDefault::GetFileSize(const char* path, int64_t& size)
{
    // The following is copied from the EAIO package.
	if(path && *path)
	{

			WIN32_FIND_DATAA win32FindDataA;
			HANDLE hFindFile = FindFirstFileA(path, &win32FindDataA);

			if(hFindFile != INVALID_HANDLE_VALUE)
			{
				size = win32FindDataA.nFileSizeLow | ((int64_t)win32FindDataA.nFileSizeHigh << (int64_t)32);
				FindClose(hFindFile);
				return true;
			}

			return false;

	}

	return false;
}


bool FileSystemDefault::GetFileModificationTime(const char* path, time_t& result)
{
    // The following is copied from the EAIO package.
	if(path && *path) 
	{

				struct _stat tempStat;
				const int r = _stat(path, &tempStat);
	    
			if(r == 0)
			{
				result = tempStat.st_mtime;
				return true;
			}

			return false;

	}

	return false;
}
#if defined(_MSC_VER)
static const char16_t kDirectorySeparator     = '\\';
#else
static const char16_t kDirectorySeparator     = '/';
#endif

static bool IsDirectorySeparator(char16_t c)
{
#if defined(_MSC_VER)
	return (c == '/') || (c == '\\');
#else
	return (c == '/');
#endif
}

bool FileSystemDefault::MakeDirectoryInternal(const char* path)
{
	if(path && *path)
	{
		const size_t nStrlen = strlen(path);

		if((path[nStrlen - 1] != '/') && (path[nStrlen - 1] != '\\'))
		{

			const BOOL bResult = CreateDirectoryA(path, NULL);
			return bResult || (GetLastError() == ERROR_ALREADY_EXISTS);

		}

		// Else we need to remove the separator.
		char pathMod[EA::WebKit::FileSystem::kMaxPathLength];
		EAW_ASSERT_MSG(nStrlen < EA::WebKit::FileSystem::kMaxPathLength, "Directory path exceeds max path length");
		memcpy(pathMod, path, nStrlen - 1);   // Force 0 terminator in place of directory separator
		pathMod[nStrlen - 1] = 0;

		return MakeDirectoryInternal(pathMod);  // Call ourselves recursively.
	}

	return false;
}

bool FileSystemDefault::MakeDirectory(const char* path)
{
	// 05/02/2011 - This function is now smart enough to create multiple levels
	// of directories.

	if(path && *path)
	{
		//Fast case - where we may be creating only a top level directory
		if(!DirectoryExists(path))
			MakeDirectoryInternal(path);

		if(DirectoryExists(path))
			return true;

		
		
		//Slow case - where we may be creating multiple levels of directory

		char path8[EA::WebKit::FileSystem::kMaxPathLength];
		const size_t nStrlen = strlen(path);
		EAW_ASSERT_MSG(nStrlen < EA::WebKit::FileSystem::kMaxPathLength, "Directory path exceeds max path length");
		strncpy(path8, path, EA::WebKit::FileSystem::kMaxPathLength);
		path8[EA::WebKit::FileSystem::kMaxPathLength-1] = 0;

		char8_t* p    = path8;
		char8_t* pEnd = path8 + nStrlen; 

		if(IsDirectorySeparator(*p))
		{
			if(IsDirectorySeparator(*++p)) // Move past an initial path separator.
				++p;
		}

		// 05/03/2011 - abaldeva: Fix a bug which could otherwise result in incorrect behavior on some platforms.
		char* rootDrive = strchr(p, ':');
		if(rootDrive)
		{
			p = ++rootDrive;
			while(IsDirectorySeparator(*p))
				++p;
		}
		/* //Old code
		if(p[0] && (p[1] == ':') && IsDirectorySeparator(p[2])) // Move past an initial C:/
			p += 3;
		*/

		if(IsDirectorySeparator(pEnd[-1])) // Remove a trailing path separator if present.
			pEnd[-1] = 0;

		for(; *p; ++p) // Walk through the path, creating each component of it if necessary.
		{
			if(IsDirectorySeparator(*p))
			{
				*p = 0;

				if(!DirectoryExists(path8))
				{
					if(!MakeDirectoryInternal(path8))//05/02/11 - abaldeva: Fix a bug otherwise multiple directories are not created.
						return false;
				}

				*p = kDirectorySeparator;
			}
		}

		if(!DirectoryExists(path8))
		{
			if(!MakeDirectoryInternal(path8))//05/02/11 - abaldeva: Fix a bug otherwise multiple directories are not created.
				return false;
		}

		return true; //12/20/11 - abaldeva: Fix a bug otherwise though multiple directories are created, the API returns false.

	}
	
	
	return false;
}


bool FileSystemDefault::GetDataDirectory(char* path, size_t pathBufferCapacity)
{
	if(path)    
	{
			strcpy(path, ".\\");
			return true;

	}
	return false;
}



bool FileSystemDefault::GetTempDirectory(char8_t* path, size_t pathBufferCapacity)
{
	if(path)
	{
		char8_t baseDir[EA::WebKit::FileSystem::kMaxPathLength];
		memset(baseDir, 0, EA::WebKit::FileSystem::kMaxPathLength);

		//Base directory should be absolute path.
		strcpy(baseDir,"c:\\temp\\EAWebKit\\pc\\");
        
		if(pathBufferCapacity > strlen(baseDir))
		{
			strcpy(path, baseDir);
			return true;
		}
	}

	return false;

}

} // namespace WebKit

} // namespace EA




