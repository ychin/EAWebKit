/*
Copyright (C) 2009, 2012, 2013 Electronic Arts, Inc.  All rights reserved.

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
// EAFileStreamStdC.cpp
//
// Copyright (c) 2003, Electronic Arts Inc. All rights reserved.
// Created by Paul Pedriana
//
/////////////////////////////////////////////////////////////////////////////


#include <EAIO/internal/Config.h>
#include <EAIO/EAFileStream.h>
#include <EAIO/Allocator.h>
#include <EAIO/FnEncode.h>
#include EA_ASSERT_HEADER
#include <stdio.h>
#include <errno.h>
#include <fcntl.h>
#if   defined(_MSC_VER)
    #include <io.h>
#endif


#if defined(_MSC_VER)
    #define O_RDONLY _O_RDONLY
    #define O_WRONLY _O_WRONLY
    #define O_RDWR   _O_RDWR  
    #define O_APPEND _O_APPEND
    #define O_CREAT  _O_CREAT 
    #define O_TRUNC  _O_TRUNC 
    #define O_EXCL   _O_EXCL  

    #define open      _open
    #define close     _close
    #define read      _read
    #define write     _write
    #define lseek     _lseek
    #define ftruncate _ftruncate
    #define commit    _commit
#endif


namespace EA
{

namespace IO
{


const int kFileHandleInvalid = -1;


FileStream::FileStream(const char8_t* pPath8)
  : mnFileHandle(kFileHandleInvalid),
    mPath8(),
    mnRefCount(0),
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
    mnRefCount(0),
    mnAccessFlags(0),
    mnCD(0),
    mnSharing(0),
    mnLastError(kStateNotOpen)
{
    FileStream::SetPath(pPath16);
}


FileStream::FileStream(const FileStream& fs)
  : IStream(),
    mnFileHandle(kFileHandleInvalid),
    mPath8(),
    mnRefCount(0),
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
    Close();
    SetPath(fs.mPath8.c_str());

    mnFileHandle  = kFileHandleInvalid;
    mnAccessFlags = 0;
    mnCD          = 0;
    mnSharing     = 0;
    mnUsageHints  = fs.mnUsageHints;
    mnLastError   = kStateNotOpen;

    return *this;
}


int FileStream::AddRef()
{
    return ++mnRefCount;
}


int FileStream::Release()
{
    if(mnRefCount > 1)
        return --mnRefCount;
    delete this;
    return 0;
}


void FileStream::SetPath(const char8_t* pPath8)
{
    if((mnFileHandle == kFileHandleInvalid) && pPath8)
        mPath8 = pPath8;
}


void FileStream::SetPath(const char16_t* pPath16)
{
    if((mnFileHandle == kFileHandleInvalid) && pPath16)
        ConvertPathUTF16ToUTF8(mPath8, pPath16);
}


size_t FileStream::GetPath(char8_t* pPath8, size_t nPathCapacity)
{
    // Return the required strlen of the destination path.
    return EAIOStrlcpy8(pPath8, mPath8.c_str(), nPathCapacity);
}


size_t FileStream::GetPath(char16_t* pPath16, size_t nPathCapacity)
{
    // Return the required strlen of the destination path.
    return StrlcpyUTF8ToUTF16(pPath16, nPathCapacity, mPath8.c_str(), mPath8.length());
}


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

        mnFileHandle = open(mPath8.c_str(), nOpenFlags);

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


bool FileStream::Close()
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

    return kSizeTypeError;
}


bool FileStream::SetSize(size_type size)
{
    if(mnFileHandle != kFileHandleInvalid)
    {
        // Technically speaking, the Standard states that the file size isn't guaranteed to 
        // increase for all implementations under all circumstances.
        #if defined(__CYGWIN__)
            // Solution for this?
            (void)size;
        #elif defined(_MSC_VER)
            // Solution for this?
            (void)size;
        #else
            const int result = ftruncate(mnFileHandle, (off_t)size);
            if(result == 0) // If the result is OK...
                return true;
            mnLastError = errno;
        #endif
    }

    return false;
}


off_type FileStream::GetPosition(PositionType positionType) const
{
    off_type result;

    if(positionType == kPositionTypeBegin)
        result = (off_type)lseek(mnFileHandle, 0, SEEK_CUR);
    else if(positionType == kPositionTypeEnd)
    {
        result = (off_type)lseek(mnFileHandle, 0, SEEK_CUR);

        if(result != (off_type)-1)
        {
            const size_type nSize = GetSize();
            if(nSize != kSizeTypeError)
                result = (off_type)(result - (off_type)nSize);
        }
    }
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

        if(nResult != -1)
            return true;

        mnLastError = errno;
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
    if(mnFileHandle != kFileHandleInvalid)
    {
        const size_type nCount = read(mnFileHandle, pData, (unsigned)nSize);
        if(nCount != kSizeTypeError)
            return nCount;
    }
    return kSizeTypeError;
}


bool FileStream::Write(const void* pData, size_type nSize)
{
    if(mnFileHandle != kFileHandleInvalid)
    {
        const size_type nCount = write(mnFileHandle, pData, (unsigned)nSize);
        if(nCount != kSizeTypeError)
            return true;
    }
    return false;
}


bool FileStream::Flush()
{
    if(mnFileHandle != kFileHandleInvalid)
    {
    }
    return true;
}


} // namespace IO


} // namespace EA

