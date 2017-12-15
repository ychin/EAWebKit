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
// EAWebKitFileSystem.h
// By Paul Pedriana 
// Maintained by EAWebKit Team
///////////////////////////////////////////////////////////////////////////////


#ifndef EAWEBKIT_EAWEBKITFILESYSTEM_H
#define EAWEBKIT_EAWEBKITFILESYSTEM_H


#include <EAWebKit/EAWebKitConfig.h>
#include <EABase/eabase.h>
#include <EAWebKit/EAWebKitSystem.h>

namespace EA
{
namespace WebKit
{
class FileSystem; 

void        SetFileSystem(FileSystem* pFileSystem);
FileSystem* GetFileSystem();


// Files open for writing are expected to be created if needed, truncated, and positioned at zero.
//
// Example usage:
//     FileSystem::FileObject file = pFileSystem->CreateFileObject();
//
//     if(file != FileSystem::kFileObjectInvalid){
//         if(pFileSystem->OpenFile(file, "/dir/file.txt", FileSystem::kRead)){
//             char buffer[20];
//             
//             int64_t result = pFileSystem->ReadFile(file, buffer, 20);
//             // A result >= 0 means OK.
//
//             pFileSystem->CloseFile(file);
//         }
//         
//         pFileSystem->DestroyFileObject(file);
//     }
//

class FileSystem
{
public:
	// File system paths are UTF8-encoded.
	// For simplicity, we define the max. path length as 260 across all platforms. If needed, this can change in the future per platform.
	// We also define it ourselves rather than using EAIO in order to maintain the decoupling.
	// From Windows documentation,
	// For example, the maximum path on drive D is "D:\some 256-character path string<NUL>" where "<NUL>" represents the invisible terminating 
	// null character for the current system codepage.
	static const size_t kMaxPathLength = 260;

	enum OpenFlags
    {
        kRead  = 0x01,
        kWrite = 0x02
    };

    enum CreateDispositionOptions
    {
        kCDONone             = 0,      /// Fails.  
        kCDOCreateNew        = 1,      /// Fails if file already exists.
        kCDOCreateAlways     = 2,      /// Never fails, always opens or creates and truncates to 0.
        kCDOOpenExisting     = 3,      /// Fails if file doesn't exist, keeps contents.
        kCDOOpenAlways       = 4,      /// Never fails, creates if doesn't exist, keeps contents.
    };

	enum ReadStatus
	{
		kReadStatusComplete		= 0,
		kReadStatusError		= -1,		/// Something went wrong with the file read
		kReadStatusDataNotReady	= -2		/// Unable to return requested data but retry in future (used to make file read completely async
	};

    // A FileObject can hold a pointer or an integer.
    typedef uintptr_t FileObject;
    static const uintptr_t kFileObjectInvalid = 0;
	static const int64_t kSizeTypeError = (int64_t)(-1);
    virtual ~FileSystem(){ }

    // Synchronous file IO
    virtual FileObject	CreateFileObject() = 0;
    virtual void		DestroyFileObject(FileObject fileObject) = 0;
    virtual bool		OpenFile(FileObject fileObject, const utf8_t* path, int openFlags, int createDisposition =  kCDONone) = 0;  
    // Given a prefix, create a temp file and provide the full path name to buffer pointed by pDestPath
	virtual FileObject	OpenTempFile(const utf8_t* prefix, utf8_t* pDestPath) = 0;
    virtual void		CloseFile(FileObject fileObject) = 0;
    // Returns (int64_t)(ReadStatus::kReadError) in case of an error that is not recoverable
	// if Returns > 0, the total number of bytes read.
	// if Returns ReadStatus::kReadComplete, the file read is complete and no more data to read.
	virtual int64_t		ReadFile(FileObject fileObject, void* buffer, int64_t size) = 0;
	// Same as ReadFile with following added capability.
	// Returns (int64_t)(ReadStatus::KReadDataNotReady) in case of data may be available in future
	virtual int64_t		ReadFileAsync(FileObject fileObject, void* buffer, int64_t size)
	{
		return ReadFile(fileObject,buffer,size);
	}
    virtual bool		WriteFile(FileObject fileObject, const void* buffer, int64_t size) = 0;
    virtual int64_t		GetFileSize(FileObject fileObject) = 0;
    virtual bool        SetFileSize(FileObject fileObject, int64_t size) = 0;
    virtual int64_t		GetFilePosition(FileObject fileObject) = 0;
    virtual bool		SetFilePosition(FileObject fileObject, int64_t position) = 0;
    virtual bool        FlushFile(FileObject fileObject) = 0;

    // File system functionality
    virtual bool		FileExists(const utf8_t* path) = 0;
    virtual bool		DirectoryExists(const utf8_t* path) = 0;
    virtual bool		RemoveFile(const utf8_t* path) = 0;
    virtual bool		DeleteDirectory(const utf8_t* path) = 0;
    virtual bool		GetFileSize(const utf8_t* path, int64_t& size) = 0;
    virtual bool		GetFileModificationTime(const utf8_t* path, time_t& result) = 0;
    virtual bool		MakeDirectory(const utf8_t* path) = 0;
    virtual bool		GetDataDirectory(utf8_t* path, size_t pathBufferCapacity) = 0;

	// GetTempDirectory slightly similar to GetDataDirectory() function just above it but differs in 2 significant ways.
	// 1. If the user returns a valid path, it is used as the base path for all other temporary file paths like cookies/cache and those paths are
	// assumed to be relative paths. Note that GetDataDirectory is not assumed to be a writable location but GetTempDirectory is. 
	// 2. It is a new function in order to be backward compatible. If the user does not want the current behavior to change, i.e., 
	// cookies/caches etc. paths specified as full paths, simply not implement this function. Default implementation is provided below.
	virtual bool		GetTempDirectory(utf8_t* path, size_t pathBufferCapacity) 
	{ 
		path[0] = 0; 
		return false;
	}
};


// We provide an optional default implementation of FileSystem.
// A given application may well want to ignore this and provide its own.
class FileSystemDefault : public FileSystem
{
public:
    FileObject CreateFileObject();
    void       DestroyFileObject(FileObject);
    bool       OpenFile(FileObject, const utf8_t* path, int openFlags, int createDisposition = kCDONone);
    FileObject OpenTempFile(const utf8_t* prefix, utf8_t* path);
    void       CloseFile(FileObject);
    int64_t    ReadFile(FileObject, void* buffer, int64_t size);
	int64_t	   ReadFileAsync(FileObject fileObject, void* buffer, int64_t size);
    bool       WriteFile(FileObject, const void* buffer, int64_t size);
    int64_t    GetFileSize(FileObject fileObject);
    bool       SetFileSize(FileObject fileObject, int64_t size);
    int64_t    GetFilePosition(FileObject fileObject);
    bool	   SetFilePosition(FileObject fileObject, int64_t position);
    bool       FlushFile(FileObject fileObject);

    // File system functionality
    bool       FileExists(const utf8_t* path);
    bool       DirectoryExists(const utf8_t* path);
    bool       RemoveFile(const utf8_t* path);
    bool       DeleteDirectory(const utf8_t* path);
    bool       GetFileSize(const utf8_t* path, int64_t& size);
    bool       GetFileModificationTime(const utf8_t* path, time_t& result);
    bool       MakeDirectory(const utf8_t* path); // This version in default file system is smart enough to create multiple directory levels if required.
    bool       GetDataDirectory(utf8_t* path, size_t pathBufferCapacity);
	bool	   GetTempDirectory(utf8_t* path, size_t pathBufferCapacity); 
private:
	bool		MakeDirectoryInternal(const utf8_t* path);
};

} // namespace WebKit

} // namespace EA


#endif // EAWEBKIT_EAWEBKITFILESYSTEM_H
