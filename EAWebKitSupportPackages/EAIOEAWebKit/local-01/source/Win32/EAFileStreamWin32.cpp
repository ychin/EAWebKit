/*
Copyright (C) 2009, 2013 Electronic Arts, Inc.  All rights reserved.

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
// EAFileStreamWin32.cpp
//
// Copyright (c) 2003, Electronic Arts Inc. All rights reserved.
// Created by Paul Pedriana
//
// Provides a file stream for Microsoft-derived platforms. These include
// Win32, Win64, XBox, WinCE.
//
/////////////////////////////////////////////////////////////////////////////


#include <EAIO/internal/Config.h>
#include EA_ASSERT_HEADER
#ifndef EAIO_EAFILESTREAM_H
    #include <EAIO/EAFileStream.h>
#endif
    #ifndef WIN32_LEAN_AND_MEAN
        #define WIN32_LEAN_AND_MEAN
    #endif
    #include <windows.h>


namespace EA
{

namespace IO
{

namespace FileStreamLocal
{
    const size_type kSizeTypeUnset     = (size_type)-1;
    const HANDLE    kFileHandleInvalid = INVALID_HANDLE_VALUE;
}


FileStream::FileStream(const char8_t* pPath8)
  : mhFile(FileStreamLocal::kFileHandleInvalid),
    mnRefCount(0),
    mnAccessFlags(0),
    mnCD(0),
    mnSharing(0),
    mnUsageHints(kUsageHintNone),
    mnLastError(0),
    mnSize(FileStreamLocal::kSizeTypeUnset)
{
    mpPath16[0] = 0;
    FileStream::SetPath(pPath8); // Note that in a constructor, the virtual function mechanism is inoperable.
}


FileStream::FileStream(const char16_t* pPath16)
  : mhFile(FileStreamLocal::kFileHandleInvalid),
    mnRefCount(0),
    mnAccessFlags(0),
    mnCD(0),
    mnSharing(0),
    mnUsageHints(kUsageHintNone),
    mnLastError(0),
    mnSize(FileStreamLocal::kSizeTypeUnset)
{
    mpPath16[0] = 0;
    FileStream::SetPath(pPath16); // Note that in a constructor, the virtual function mechanism is inoperable.
}


FileStream::FileStream(const FileStream& fs)
  : mhFile(FileStreamLocal::kFileHandleInvalid),
    mnRefCount(0),
    mnAccessFlags(0),
    mnCD(0),
    mnSharing(0),
    mnUsageHints(fs.mnUsageHints),
    mnLastError(0),
    mnSize(FileStreamLocal::kSizeTypeUnset)
{
    mpPath16[0] = 0;
    FileStream::SetPath(fs.mpPath16); // Note that in a constructor, the virtual function mechanism is inoperable.
}


FileStream::~FileStream()
{
    FileStream::Close(); // Note that in a destructor, the virtual function mechanism is inoperable.
}


FileStream& FileStream::operator=(const FileStream& fs)
{
    using namespace FileStreamLocal;

    Close();
    SetPath(fs.mpPath16);

    mhFile        = kFileHandleInvalid;
    mnAccessFlags = 0;
    mnCD          = 0;
    mnSharing     = 0;
    mnUsageHints  = fs.mnUsageHints;
    mnLastError   = 0;
    mnSize        = fs.mnSize;

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
    using namespace FileStreamLocal;

    if((mhFile == kFileHandleInvalid) && pPath8)
        MultiByteToWideChar(CP_UTF8, 0, pPath8, -1, mpPath16, kMaxPathLength); // Convert UTF8 to UTF16.
}


void FileStream::SetPath(const char16_t* pPath16)
{
    using namespace FileStreamLocal;

    if((mhFile == kFileHandleInvalid) && pPath16)
        wcsncpy(mpPath16, pPath16, kMaxPathLength);
}


size_t FileStream::GetPath(char8_t* pPath8, size_t nPathLength)
{
    // Convert from UTF16 to UTF8.
    if(pPath8 && nPathLength)
        return (size_t)WideCharToMultiByte(CP_UTF8, 0, mpPath16, -1, pPath8, (int)nPathLength, NULL, NULL) - 1; // WideCharToMultiByte() - 1 because we don't want to include the terminating null in the returned count. 

    // In this case the user wants just the length of the return value.
    char8_t pathTemp[kMaxPathLength];
    return GetPath(pathTemp, kMaxPathLength);
}


size_t FileStream::GetPath(char16_t* pPath16, size_t nPathLength)
{
    if(pPath16 && nPathLength)
    {
        wcsncpy(pPath16, mpPath16, nPathLength);
        pPath16[nPathLength - 1] = 0;
    }

    return wcslen(mpPath16);
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

            mhFile = CreateFileW(mpPath16, nAccess, nShare, NULL, nCreate, dwFlagsAndAttributes, NULL);

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
        CloseHandle(mhFile); // This returns zero upon error. But there's not much to do about it.

        mhFile        = kFileHandleInvalid;
        mnAccessFlags = 0;
        mnCD          = 0;
        mnSharing     = 0;
        mnUsageHints  = 0;
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
    using namespace FileStreamLocal;

    if(mhFile == kFileHandleInvalid)
        return kStateNotOpen;

    return mnLastError;
}


size_type FileStream::GetSize() const
{
    using namespace FileStreamLocal;

    if(mhFile != kFileHandleInvalid)
    {
        if(mnSize != kSizeTypeUnset) // If the size is cached...
            return mnSize;

        LARGE_INTEGER li;
        const BOOL bResult = GetFileSizeEx(mhFile, &li);

        if(bResult)  
        {
            #if EAIO_64_BIT_SIZE_ENABLED
                size_type returnValue = li.QuadPart;
            #else
                size_type returnValue = li.LowPart;
            #endif

            if(((mnAccessFlags & kAccessFlagWrite) == 0) && ((mnSharing & (kShareWrite | kShareDelete)) == 0)) // If the size is constant...
                mnSize = returnValue;

            return returnValue;
        }

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

        #if EAIO_64_BIT_SIZE_ENABLED
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

        EA_ASSERT(nSize < 0xffffffff); // We are currently limited to 4GB per individual read.
        const BOOL bResult = WriteFile(mhFile, pData, (DWORD)nSize, &dwWriteCount, NULL);

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


} // namespace IO


} // namespace EA















