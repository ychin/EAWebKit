/*
 * Copyright (C) 2007 Staikos Computing Services Inc.
 * Copyright (C) 2007 Holger Hans Peter Freyther
 * Copyright (C) 2008 Apple, Inc. All rights reserved.
 * Copyright (C) 2008 Collabora, Ltd. All rights reserved.
 * Copyright (C) 2011, 2012, 2013, 2014 Electronic Arts, Inc. All rights reserved.
 *
 * Redistribution and use in source and binary forms, with or without
 * modification, are permitted provided that the following conditions
 * are met:
 *
 * 1.  Redistributions of source code must retain the above copyright
 *     notice, this list of conditions and the following disclaimer.
 * 2.  Redistributions in binary form must reproduce the above copyright
 *     notice, this list of conditions and the following disclaimer in the
 *     documentation and/or other materials provided with the distribution.
 * 3.  Neither the name of Apple Computer, Inc. ("Apple") nor the names of
 *     its contributors may be used to endorse or promote products derived
 *     from this software without specific prior written permission.
 *
 * THIS SOFTWARE IS PROVIDED BY APPLE AND ITS CONTRIBUTORS "AS IS" AND ANY
 * EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE IMPLIED
 * WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE ARE
 * DISCLAIMED. IN NO EVENT SHALL APPLE OR ITS CONTRIBUTORS BE LIABLE FOR ANY
 * DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES
 * (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES;
 * LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND
 * ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT
 * (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE OF
 * THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
 */

#include "config.h"
#include "FileSystem.h"
#include "FileMetadata.h"


#include <wtf/text/CString.h>
#include <wtf/Assertions.h>

#include <EAWebKit/EAWebKitFileSystem.h>
#include <internal/include/EAWebKitString.h>
#include <EAIO/EAFileDirectory.h>
#include EAWEBKIT_PLATFORM_HEADER
COMPILE_ASSERT(sizeof(WebCore::PlatformFileHandle) == sizeof(EA::WebKit::FileSystem::FileObject), PlatformFileHandleIsEAWebKitFileSystemFileObject);

namespace {

	// We use following as path separator when constructing the path ourselves.
#if defined(_MSC_VER)
	const char16_t kDirectorySeparator     = '\\';
	const char16_t kFilePathDriveSeparator = ':';
#else
	const char16_t kDirectorySeparator     = '/';
	const char16_t kFilePathDriveSeparator = 0;
#endif

	// The reason we check for the forward slash on Windows is that we are checking a path passed to us and while uncommon, Windows supports it.
	// http://en.wikipedia.org/wiki/Path_(computing)#MS-DOS.2FMicrosoft_Windows_style

	inline bool IsDirectorySeparator(char16_t c)
	{
#if defined(_MSC_VER)
		return (c == '/') || (c == '\\');
#else
		return (c == '/');
#endif
	}


	inline bool HasUNCPrefix(const char16_t* pBegin, const char16_t* pEnd)
	{
		// UNC paths specifically use '\' and not '/'.
		return (((pBegin + 2) <= pEnd) && (pBegin[0] == '\\') && (pBegin[1] == '\\'));
	}


	const char16_t* GetFileName(const char16_t* pBegin, const char16_t* pEnd = NULL)
	{
		if(pEnd == NULL) 
			pEnd = pBegin + EA::Internal::Strlen(pBegin);

		// If it's a trailing separator, then there's no file name
		if((pBegin < pEnd) && IsDirectorySeparator(pEnd[-1])) 
			return pEnd;

		const char16_t* pName = pEnd;

		// Skip over any non-file-path separators
		while((pName > pBegin)
			&& !IsDirectorySeparator(pName[-1])
			&& (pName[-1] != kFilePathDriveSeparator))
			--pName;

		// A UNC machine name is not, I am afraid, a file name
		if((pName == (pBegin + 2)) && HasUNCPrefix(pBegin, pEnd))
			return pEnd;

		return pName;
	}

}  // namespace

namespace WebCore {

//
// + Functions implemented in terms of the file system
//
bool fileExists(const String& path)
{
	EA::WebKit::FileSystem* pFS = EA::WebKit::GetFileSystem();
	if(pFS)
	{
		char path8[EA::WebKit::FileSystem::kMaxPathLength]; 
		EA::Internal::Strlcpy(path8, path.characters(), EA::WebKit::FileSystem::kMaxPathLength, path.length());
		return pFS->FileExists(path8);
	}

	return false;
}

bool directoryExists(const String& path)
{
	EA::WebKit::FileSystem* pFS = EA::WebKit::GetFileSystem();
	if(pFS)
	{
		char path8[EA::WebKit::FileSystem::kMaxPathLength]; 
		EA::Internal::Strlcpy(path8, path.characters(), EA::WebKit::FileSystem::kMaxPathLength, path.length());
		return pFS->DirectoryExists(path8);
	}

	return false;
}

bool deleteFile(const String& path)
{
	EA::WebKit::FileSystem* pFS = EA::WebKit::GetFileSystem();
	if(pFS)
	{
		char path8[EA::WebKit::FileSystem::kMaxPathLength];
		EA::Internal::Strlcpy(path8, path.characters(), EA::WebKit::FileSystem::kMaxPathLength, path.length());
		return pFS->RemoveFile(path8);
	}

	return false;
}

bool deleteEmptyDirectory(const String& path)
{
	EA::WebKit::FileSystem* pFS = EA::WebKit::GetFileSystem();
	if(pFS)
	{
		char path8[EA::WebKit::FileSystem::kMaxPathLength]; 
		EA::Internal::Strlcpy(path8, path.characters(), EA::WebKit::FileSystem::kMaxPathLength, path.length());
		return pFS->DeleteDirectory(path8);
	}

	return false;
}

bool getFileSize(const String& path, long long& result)
{
	EA::WebKit::FileSystem* pFS = EA::WebKit::GetFileSystem();
	if(pFS)
	{
		char path8[EA::WebKit::FileSystem::kMaxPathLength];
		EA::Internal::Strlcpy(path8, path.characters(), EA::WebKit::FileSystem::kMaxPathLength, path.length());
		int64_t resultInt64;
		bool returnbool =  pFS->GetFileSize(path8, resultInt64);
		result = resultInt64;
		return returnbool;
	}

	return false;
}

bool getFileModificationTime(const String& path, time_t& result)
{
	EA::WebKit::FileSystem* pFS = EA::WebKit::GetFileSystem();

	if(pFS)
	{
		char path8[EA::WebKit::FileSystem::kMaxPathLength];
		EA::Internal::Strlcpy(path8, path.characters(), EA::WebKit::FileSystem::kMaxPathLength, path.length());
		return pFS->GetFileModificationTime(path8, result);
	}

	return false;
}

bool makeAllDirectories(const String& path)
{
	EA::WebKit::FileSystem* pFS = EA::WebKit::GetFileSystem();

	if(pFS) 
	{
		char path8[EA::WebKit::FileSystem::kMaxPathLength];
		int  length = EA::Internal::Strlcpy(path8, path.characters(), EA::WebKit::FileSystem::kMaxPathLength, path.length());
		(void) length;
		if(!pFS->DirectoryExists(path8))
		{
			return pFS->MakeDirectory(path8);
		}

		return true;
	}

	return false;
}


String homeDirectoryPath()
{
	// The only use of this function seems to be to download the plugins to the browser specific directory
	EA::WebKit::FileSystem* pFS = EA::WebKit::GetFileSystem();
	if(pFS)
	{
		char path8[EA::WebKit::FileSystem::kMaxPathLength];
        if(pFS->GetDataDirectory(path8, EA::WebKit::FileSystem::kMaxPathLength - 1))
			return String(path8);
	}

	return String();
}

PlatformFileHandle openFile(const String& path, FileOpenMode mode)
{
    EA::WebKit::FileSystem* pFS = EA::WebKit::GetFileSystem();

    int openFlags = 0;
    int createDisposition = 0;
    if (mode == OpenForRead)
    {
        openFlags |= EA::WebKit::FileSystem::kRead;
        createDisposition = EA::WebKit::FileSystem::kCDOOpenExisting;
    }
    else if (mode == OpenForWrite)
    {
        openFlags |= EA::WebKit::FileSystem::kWrite;
        createDisposition = EA::WebKit::FileSystem::kCDOCreateAlways;
    }
    else
    {
        return EA::WebKit::FileSystem::kFileObjectInvalid;
    }

    EA::WebKit::FileSystem::FileObject file = pFS->CreateFileObject();
    if (!pFS->OpenFile(file, path.ascii().data(), openFlags, createDisposition)) {
        pFS->DestroyFileObject(file);
        return EA::WebKit::FileSystem::kFileObjectInvalid;
    }

    return file;
}

void closeFile(PlatformFileHandle& fileHandle)
{
	EA::WebKit::FileSystem* pFS = EA::WebKit::GetFileSystem();

	if(pFS && fileHandle != EA::WebKit::FileSystem::kFileObjectInvalid)
	{
		pFS->CloseFile(fileHandle);
		pFS->DestroyFileObject(fileHandle);
	}
}

int writeToFile(PlatformFileHandle fileHandle, const char* data, int length)
{
	EA::WebKit::FileSystem* pFS = EA::WebKit::GetFileSystem();

	if(pFS && fileHandle != EA::WebKit::FileSystem::kFileObjectInvalid)
	{
		if(pFS->WriteFile(fileHandle, data, length))
			return length;
	}
	return 0;
}

String openTemporaryFile(const String& prefix, PlatformFileHandle &fileHandle)
{
	EA::WebKit::FileSystem* pFS = EA::WebKit::GetFileSystem();
	if(pFS)
	{
		char path8[EA::WebKit::FileSystem::kMaxPathLength];
		fileHandle = pFS->OpenTempFile(prefix.ascii().data(), path8);
		if(fileHandle != EA::WebKit::FileSystem::kFileObjectInvalid)
		{
			return String(path8);
		}
	}

	fileHandle = invalidPlatformFileHandle;
	return String();
}

Vector<String> listDirectory(const String& path, const String& filter)
{
	String pathDup = path;
	String filterDup = filter;
	Vector<String> entries;

	using namespace EA::IO;
	DirectoryIterator di;
	DirectoryIterator::EntryList entryList;
	di.Read((pathDup.charactersWithNullTermination().data()), entryList, (filterDup.charactersWithNullTermination().data()), EA::IO::kDirectoryEntryFile, 1000);
	for(DirectoryIterator::EntryList::const_iterator iter = entryList.begin(); iter != entryList.end(); ++iter)
	{
		entries.append((iter->msName.c_str()));
	}
		
	return entries;
}


bool getFileMetadata(const String& path, FileMetadata& result)
{
    EA::WebKit::FileSystem* pFS = EA::WebKit::GetFileSystem();

    if(pFS)
	{
        bool is_file = fileExists(path);
        bool is_dir = directoryExists(path);

        if (is_file || is_dir)
        {
            time_t time;
            getFileModificationTime(path, time);
            result.modificationTime = (double) time;

            getFileSize(path, result.length);
            
            result.type = is_dir ? FileMetadata::TypeDirectory : FileMetadata::TypeFile;

            return true;
        }
	}

    return false;
}

//
// -
//


//
// Functions implemented locally
//

String pathByAppendingComponent(const String& path, const String& component)
{
	const unsigned len = path.length();

	if(len && IsDirectorySeparator(path[len - 1]))
		return path + component;

	String pathResult(path);
	pathResult.append(kDirectorySeparator);
	pathResult.append(component);

	return pathResult;
}

String pathGetFileName(const String& path)
{
    auto pathChars = path.charactersWithNullTermination();
	const char16_t* pFileName = GetFileName(pathChars.data());  
	return String(pFileName);
}

String directoryName(const String& path)
{
	// EA::IO::Path doesn't have a function to get the end of a directory, just a function to get start of file name.
	const UChar* pPath     = path.characters();
	const UChar* pFileName = GetFileName(pPath);  // Note that (PathString::iterator == UChar* == char16_t*)

	if(pFileName > pPath)
		--pFileName;

	if(IsDirectorySeparator(*pFileName))
		--pFileName;

	return String(pPath, (unsigned)((pFileName + 1) - pPath));
}



bool unloadModule(PlatformModule/* module*/)
{
	// This function is only called from the plugin related code which we don't support.
	ASSERT_NOT_REACHED();
	return false;
}

}

// vim: ts=4 sw=4 et
