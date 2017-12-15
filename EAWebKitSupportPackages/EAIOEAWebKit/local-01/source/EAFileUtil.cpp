/*
Copyright (C) 2007,2009, 2011, 2012, 2014 Electronic Arts, Inc.  All rights reserved.

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
// EAFileUtil.cpp
// 
// Created by Paul Pedriana - 2007
//
// Implements various file and directory manipulation functions.
/////////////////////////////////////////////////////////////////////////////


#include <EAIO/internal/Config.h>
#include <EABase/eabase.h>
#include <EAIO/EAFileBase.h>
#include <EAIO/EAFileDirectory.h>
#include <EAIO/EAFileStream.h>
#include <EAIO/EAFileUtil.h>
#include <EAIO/FnEncode.h>
#include <EAIO/PathString.h>
#include <string.h>
#include <time.h>
#include EA_ASSERT_HEADER

    #ifndef _WIN32_WINNT 
        #define _WIN32_WINNT 0x0500 // We use GetVolumePathName, which is a Windows 2000 API.
    #endif
    #include <windows.h>
    #include <direct.h>
    #include <sys/stat.h>
    #include <sys/utime.h>
    #include <shlobj.h>
    #include <objbase.h>

    #ifdef _MSC_VER
        #pragma comment(lib, "ole32.lib")	// Required for CoInitialize, etc.
		#pragma comment(lib, "shell32.lib") // Required for SHGetFolderPath, etc.
    #endif


#ifdef _MSC_VER
    #pragma warning(disable: 4127) // conditional expression is constant
#endif


namespace
{
    // EAIOItoa16
    // Convert an unsigned integer to a wide-character string (decimal).
    // The buffer parameter must hold at least 16 characters.
    // Returns a pointer to somewhere within the input buffer.
    char16_t* EAIOItoa16(uint32_t value, char16_t* buffer)
    {
        buffer[15] = 0;
        uint32_t i = 14;
        
        // Write out the character representation of 'value' starting with the highest power of 10
        for (; value > 0; value /= 10)
            buffer[i--] = (char16_t)('0' + (value % 10));

        return &buffer[i + 1];
    }

    char8_t* EAIOItoa8(uint32_t value, char8_t* buffer)
    {
        buffer[15] = 0;
        uint32_t i = 14;
        
        // Write out the character representation of 'value' starting with the highest power of 10
        for (; value > 0; value /= 10)
            buffer[i--] = (char8_t)('0' + (value % 10));

        return &buffer[i + 1];
    }

}


namespace EA
{

namespace IO
{





////////////////////////////////////////////////////////////////////////////
// gpTempDirectory
//
// Temporary directory that the user can specify to take precedence over 
// a system default temp directory. The user can use the GetTempDirectory
// and SetTempDirectory functions to get and set this value.
//
const uint32_t kTempDirectoryLength             = kMaxDirectoryLength + 1;
char16_t gpTempDirectory[kTempDirectoryLength]  = { 0 };



////////////////////////////////////////////////////////////////////////////
// gpCurrentWorkingDirectory
//
// This holds the current working directory. On platforms whereby the system
// supports the concept of current working directory, this value is not defined.
// On platforms whereby the the system does not support the concept of a 
// current working directory, this value is defined. The user can use 
// GetCurrentWorkingDirectory and SetCurrentWorkingDirectory to get and set
// this value.
//
// Note: A weakness of this variable is that system file io code is not aware
// of it and cannot use it. It can only be of use if the user intervenes and 
// reads this value before calling system file io functions.
//
    // Define nothing, as the system supports this.



////////////////////////////////////////////////////////////////////////////
// DriveInfo16::DriveInfo16
//
DriveInfo16::DriveInfo16(const char16_t* pName, DriveType driveType)
    : mType(driveType)
{
    if(pName)
        mName.assign(pName);
}


////////////////////////////////////////////////////////////////////////////
// DriveInfo8::DriveInfo8
//
DriveInfo8::DriveInfo8(const char8_t* pName, DriveType driveType)
    : mType(driveType)
{
    if(pName)
        mName.assign(pName);
}


////////////////////////////////////////////////////////////////////////////
// File::Create
//
EAIO_API bool File::Create(const char16_t* pPath, bool bTruncate)
{


            SECURITY_ATTRIBUTES tempSECURITY_ATTRIBUTES;
            tempSECURITY_ATTRIBUTES.nLength              = sizeof(tempSECURITY_ATTRIBUTES);
            tempSECURITY_ATTRIBUTES.lpSecurityDescriptor = NULL;
            tempSECURITY_ATTRIBUTES.bInheritHandle       = FALSE;

            // We have a problem here if the given file exists and is open by another 
            // entity but in write mode with no ability for us to read the file.
            // In this case we will probably want to change this code to try to test
            // for the existence of the file before trying to create it.
            HANDLE hFile = CreateFileW(pPath, GENERIC_READ, FILE_SHARE_READ | FILE_SHARE_WRITE, 
                                        &tempSECURITY_ATTRIBUTES, OPEN_ALWAYS, FILE_ATTRIBUTE_NORMAL, NULL);


        if(hFile != INVALID_HANDLE_VALUE)
        {
            if(bTruncate)
            {
                SetFilePointer(hFile, 0, 0, FILE_BEGIN);
                SetEndOfFile(hFile);
            }

            CloseHandle(hFile);
            return true;
        }

        return false;

}

EAIO_API bool File::Create(const char8_t* pPath, bool bTruncate)
{
    using namespace Path;


            PathString16 path16;
            ConvertPathUTF8ToUTF16(path16, pPath);

            SECURITY_ATTRIBUTES tempSECURITY_ATTRIBUTES;
            tempSECURITY_ATTRIBUTES.nLength              = sizeof(tempSECURITY_ATTRIBUTES);
            tempSECURITY_ATTRIBUTES.lpSecurityDescriptor = NULL;
            tempSECURITY_ATTRIBUTES.bInheritHandle       = FALSE;

            // We have a problem here if the given file exists and is open by another 
            // entity but in write mode with no ability for us to read the file.
            // In this case we will probably want to change this code to try to test
            // for the existence of the file before trying to create it.
            HANDLE hFile = CreateFileW(path16.c_str(), GENERIC_READ, FILE_SHARE_READ | FILE_SHARE_WRITE, 
                                        &tempSECURITY_ATTRIBUTES, OPEN_ALWAYS, FILE_ATTRIBUTE_NORMAL, NULL);


        if(hFile != INVALID_HANDLE_VALUE)
        {
            if(bTruncate)
            {
                SetFilePointer(hFile, 0, 0, FILE_BEGIN);
                SetEndOfFile(hFile);
            }

            CloseHandle(hFile);
            return true;
        }

        return false;

}


////////////////////////////////////////////////////////////////////////////
// File::Exists
//
EAIO_API bool File::Exists(const char16_t* pPath)
{
    if(pPath && *pPath)
    {

            const DWORD dwAttributes = ::GetFileAttributesW(pPath);
            return ((dwAttributes != INVALID_FILE_ATTRIBUTES) && ((dwAttributes & FILE_ATTRIBUTE_DIRECTORY) == 0));

    }

    return false;
}

EAIO_API bool File::Exists(const char8_t* pPath)
{
    using namespace Path;

    if(pPath && *pPath)
    {

            PathString16 path16;
            ConvertPathUTF8ToUTF16(path16, pPath);

            const DWORD dwAttributes = ::GetFileAttributesW(path16.c_str());
            return ((dwAttributes != INVALID_FILE_ATTRIBUTES) && ((dwAttributes & FILE_ATTRIBUTE_DIRECTORY) == 0));

    }

    return false;
}



////////////////////////////////////////////////////////////////////////////
// File::PatternExists
//
EAIO_API bool File::PatternExists(const char16_t* pPathPattern)
{
    using namespace Path;

    // The EntryFindFirst function requires a valid directory path.
    // We must split pPathPattern into a directory and file component.
    const char16_t* const pFileNamePattern = Path::GetFileName(pPathPattern);
    PathString16 directory16(pPathPattern, pFileNamePattern);

    EntryFindData entryFindData;
    EntryFindData* const pEntryFindData = EntryFindFirst(directory16.c_str(), pFileNamePattern, &entryFindData);

    if(pEntryFindData)
        EntryFindFinish(pEntryFindData);

    return (pEntryFindData != NULL);
}

EAIO_API bool File::PatternExists(const char8_t* pPathPattern)
{
    using namespace Path;

    PathString16 pathPattern16;
    ConvertPathUTF8ToUTF16(pathPattern16, pPathPattern);

    return PatternExists(pathPattern16.c_str());
}




////////////////////////////////////////////////////////////////////////////
// File::IsWritable
//
EAIO_API bool File::IsWritable(const char16_t* pPath)
{
    // Todo: Complete this function properly. The current implementation
    //       of reading the attribute isn't enough for a rigorous test.
    return (GetAttributes(pPath) & kAttributeWritable) != 0;
}

EAIO_API bool File::IsWritable(const char8_t* pPath)
{
    // Todo: Complete this function properly. The current implementation
    //       of reading the attribute isn't enough for a rigorous test.
    return (GetAttributes(pPath) & kAttributeWritable) != 0;
}




////////////////////////////////////////////////////////////////////////////
// File::Remove
//
EAIO_API bool File::Remove(const char16_t* pPath)
{

        const BOOL bResult = ::DeleteFileW(pPath);
        // Possibly save off an error value here.
        return (bResult != 0);

}

EAIO_API bool File::Remove(const char8_t* pPath)
{
    using namespace Path;


        PathString16 path16;
        ConvertPathUTF8ToUTF16(path16, pPath);

        const BOOL bResult = ::DeleteFileW(path16.c_str());
        return (bResult != 0); // Possibly save off an error value here.

}


////////////////////////////////////////////////////////////////////////////
// File::Move
//
EAIO_API bool File::Move(const char16_t* pPathSource, const char16_t* pPathDestination, bool bOverwriteIfPresent)
{
    using namespace EA::IO::Path;


        if(bOverwriteIfPresent || !Exists(pPathDestination))
            return (::MoveFileExW(pPathSource, pPathDestination, MOVEFILE_REPLACE_EXISTING | MOVEFILE_COPY_ALLOWED) != 0);
        return false;

}

EAIO_API bool File::Move(const char8_t* pPathSource, const char8_t* pPathDestination, bool bOverwriteIfPresent)
{
    using namespace Path;


        PathString16 pathSource16;
        ConvertPathUTF8ToUTF16(pathSource16, pPathSource);

        PathString16 pathDest16;
        ConvertPathUTF8ToUTF16(pathDest16, pPathDestination);

        if(bOverwriteIfPresent || !Exists(pathDest16.c_str()))
            return (::MoveFileExW(pathSource16.c_str(), pathDest16.c_str(), MOVEFILE_REPLACE_EXISTING | MOVEFILE_COPY_ALLOWED) != 0);

        return false;

}



///////////////////////////////////////////////////////////////////////////////
// File::Copy
//
EAIO_API bool File::Copy(const char16_t* pPathSource, const char16_t* pPathDestination, bool bOverwriteIfPresent)
{

        return ::CopyFileW(pPathSource, pPathDestination, !bOverwriteIfPresent) != 0;

}

EAIO_API bool File::Copy(const char8_t* pPathSource, const char8_t* pPathDestination, bool bOverwriteIfPresent)
{

        return ::CopyFileA(pPathSource, pPathDestination, !bOverwriteIfPresent) != 0;

}



///////////////////////////////////////////////////////////////////////////////
// File::GetSize
//
// Returns the file size in bytes.
//
EAIO_API size_type File::GetSize(const char16_t* pPath)
{
    size_type nReturnValue = kSizeTypeError;


        #ifdef _MSC_VER
            #pragma warning(push)
            #pragma warning(disable: 4127)  // conditional expression is constant.
        #endif

        WIN32_FIND_DATAW win32FindDataW;
        const HANDLE hFindFile = FindFirstFileW(pPath, &win32FindDataW);

        if(hFindFile != INVALID_HANDLE_VALUE)
        {
            nReturnValue = (size_type)win32FindDataW.nFileSizeLow;
            if(sizeof(size_type) >= sizeof(uint64_t))
                nReturnValue |= (size_type)((uint64_t)win32FindDataW.nFileSizeHigh << (uint64_t)32);
            FindClose(hFindFile);
        }

        #ifdef _MSC_VER
            #pragma warning(pop)
        #endif


    return nReturnValue;
}

EAIO_API size_type File::GetSize(const char8_t* pPath)
{
    using namespace EA::IO::Path;


        PathString16 path16;
        ConvertPathUTF8ToUTF16(path16, pPath);

        return File::GetSize(path16.c_str());

}




///////////////////////////////////////////////////////////////////////////////
// File::GetAttributes
//
EAIO_API int File::GetAttributes(const char16_t* pPath)
{
    int nAttributes = 0;



            const DWORD dwCurrentAttributes = GetFileAttributesW(pPath);
            if (dwCurrentAttributes == INVALID_FILE_ATTRIBUTES)
                return 0;


        // Under Win32, all files effectively have the 'readable' flag set.
        // As such, we do nothing here as this test can never fail.
        nAttributes |= kAttributeReadable;

        // Win32 doesn't have a Unix-like writability mask, 
        // but instead has a read-only mask.
        if((dwCurrentAttributes & FILE_ATTRIBUTE_READONLY) == 0)
            nAttributes |= kAttributeWritable;

        // Win32 doesn't have a Unix-like executable mask. As such, all
        // files effectively have the 'executable' flag set.
        nAttributes |= kAttributeExecutable;

        if(dwCurrentAttributes & FILE_ATTRIBUTE_DIRECTORY)
            nAttributes |= kAttributeDirectory;

        if(dwCurrentAttributes & FILE_ATTRIBUTE_ARCHIVE)
            nAttributes |= kAttributeArchive;

        if(dwCurrentAttributes & FILE_ATTRIBUTE_HIDDEN)
            nAttributes |= kAttributeHidden;

        if(dwCurrentAttributes & FILE_ATTRIBUTE_SYSTEM)
            nAttributes |= kAttributeSystem;
    

    return nAttributes;
}

EAIO_API int File::GetAttributes(const char8_t* pPath)
{
    using namespace Path;

    int nAttributes = 0;


            PathString16 path16;
            ConvertPathUTF8ToUTF16(path16, pPath);

            const DWORD dwCurrentAttributes = GetFileAttributesW(path16.c_str());
            if (dwCurrentAttributes == INVALID_FILE_ATTRIBUTES)
                return 0;


        // Under Win32, all files effectively have the 'readable' flag set.
        // As such, we do nothing here as this test can never fail.
        nAttributes |= kAttributeReadable;

        // Win32 doesn't have a Unix-like writability mask, 
        // but instead has a read-only mask.
        if((dwCurrentAttributes & FILE_ATTRIBUTE_READONLY) == 0)
            nAttributes |= kAttributeWritable;

        // Win32 doesn't have a Unix-like executable mask. As such, all
        // files effectively have the 'executable' flag set.
        nAttributes |= kAttributeExecutable;

        if(dwCurrentAttributes & FILE_ATTRIBUTE_DIRECTORY)
            nAttributes |= kAttributeDirectory;

        if(dwCurrentAttributes & FILE_ATTRIBUTE_ARCHIVE)
            nAttributes |= kAttributeArchive;

        if(dwCurrentAttributes & FILE_ATTRIBUTE_HIDDEN)
            nAttributes |= kAttributeHidden;

        if(dwCurrentAttributes & FILE_ATTRIBUTE_SYSTEM)
            nAttributes |= kAttributeSystem;
    

    return nAttributes;
}



///////////////////////////////////////////////////////////////////////////////
// File::SetAttributes
//
EAIO_API bool File::SetAttributes(const char16_t* pPath, int nAttributeMask, bool bEnable)
{


            const DWORD dwCurrentAttributes = GetFileAttributesW(pPath);


        DWORD dwNewAttributes = dwCurrentAttributes;

        if(nAttributeMask & kAttributeReadable){
            // Under Win32, all files effectively have the 'readable' flag set.
            // If the caller tries to set this flag, we do nothing.
        }

        if(nAttributeMask & kAttributeWritable){
            // Win32 doesn't have a Unix-like writability mask, but instead has a read-only mask.
            if(bEnable)
                dwNewAttributes &= ~FILE_ATTRIBUTE_READONLY; // Turn off this flag.
            else
                dwNewAttributes |=  FILE_ATTRIBUTE_READONLY; // Turn on this flag.
        }

        if(nAttributeMask & kAttributeExecutable){
            // Win32 doesn't have a Unix-like executable mask. As such, all
            // files effectively have the 'executable' flag set. If the
            // caller tries to set this flag, we do nothing.
        }

        if(nAttributeMask & kAttributeDirectory){
            // You cannot set this attribute. Perhaps some unusual OS will allow you
            // to do this, but we always disallow it, for consistency.
            // EA_FAIL_MESSAGE("SetFileAttributes: kAttributeDirectory can't be altered.\n\n");
            return false;
        }

        if(nAttributeMask & kAttributeAlias){
            // You cannot set this attribute. Perhaps some unusual OS will allow 
            // you to do this, but we always disallow it, for consistency.
            // EA_FAIL_MESSAGE("SetFileAttributes: kAttributeAlias can't be altered.\n\n");
            return false;
        }

        if(nAttributeMask & kAttributeArchive){
            if(bEnable)
                dwNewAttributes |=  FILE_ATTRIBUTE_ARCHIVE;
            else
                dwNewAttributes &= ~FILE_ATTRIBUTE_ARCHIVE;
        }

        if(nAttributeMask & kAttributeHidden){
            if(bEnable)
                dwNewAttributes |=  FILE_ATTRIBUTE_HIDDEN;
            else
                dwNewAttributes &= ~FILE_ATTRIBUTE_HIDDEN;
        }

        if(nAttributeMask & kAttributeSystem){
            if(bEnable)
                dwNewAttributes |=  FILE_ATTRIBUTE_SYSTEM;
            else
                dwNewAttributes &= ~FILE_ATTRIBUTE_SYSTEM;
        }

        if(dwNewAttributes == dwCurrentAttributes)
            return true;



            return (SetFileAttributesW(pPath, dwNewAttributes) != 0);


}

EAIO_API bool File::SetAttributes(const char8_t* pPath, int nAttributeMask, bool bEnable)
{
    using namespace Path;


            PathString16 path16;
            ConvertPathUTF8ToUTF16(path16, pPath);

            const DWORD dwCurrentAttributes = GetFileAttributesW(path16.c_str());


        DWORD dwNewAttributes = dwCurrentAttributes;

        if(nAttributeMask & kAttributeReadable){
            // Under Win32, all files effectively have the 'readable' flag set.
            // If the caller tries to set this flag, we do nothing.
        }

        if(nAttributeMask & kAttributeWritable){
            // Win32 doesn't have a Unix-like writability mask, but instead has a read-only mask.
            if(bEnable)
                dwNewAttributes &= ~FILE_ATTRIBUTE_READONLY; // Turn off this flag.
            else
                dwNewAttributes |=  FILE_ATTRIBUTE_READONLY; // Turn on this flag.
        }

        if(nAttributeMask & kAttributeExecutable){
            // Win32 doesn't have a Unix-like executable mask. As such, all
            // files effectively have the 'executable' flag set. If the
            // caller tries to set this flag, we do nothing.
        }

        if(nAttributeMask & kAttributeDirectory){
            // You cannot set this attribute. Perhaps some unusual OS will allow you
            // to do this, but we always disallow it, for consistency.
            // EA_FAIL_MESSAGE("SetFileAttributes: kAttributeDirectory can't be altered.\n\n");
            return false;
        }

        if(nAttributeMask & kAttributeAlias){
            // You cannot set this attribute. Perhaps some unusual OS will allow 
            // you to do this, but we always disallow it, for consistency.
            // EA_FAIL_MESSAGE("SetFileAttributes: kAttributeAlias can't be altered.\n\n");
            return false;
        }

        if(nAttributeMask & kAttributeArchive){
            if(bEnable)
                dwNewAttributes |=  FILE_ATTRIBUTE_ARCHIVE;
            else
                dwNewAttributes &= ~FILE_ATTRIBUTE_ARCHIVE;
        }

        if(nAttributeMask & kAttributeHidden){
            if(bEnable)
                dwNewAttributes |=  FILE_ATTRIBUTE_HIDDEN;
            else
                dwNewAttributes &= ~FILE_ATTRIBUTE_HIDDEN;
        }

        if(nAttributeMask & kAttributeSystem){
            if(bEnable)
                dwNewAttributes |=  FILE_ATTRIBUTE_SYSTEM;
            else
                dwNewAttributes &= ~FILE_ATTRIBUTE_SYSTEM;
        }

        if(dwNewAttributes == dwCurrentAttributes)
            return true;



            return (SetFileAttributesW(path16.c_str(), dwNewAttributes) != 0);


}




///////////////////////////////////////////////////////////////////////////////
// File::GetTime
//
EAIO_API time_t File::GetTime(const char16_t* pPath, FileTimeType timeType)
{

            struct _stat tempStat;
            const int result = _wstat(pPath, &tempStat);
    
        if(result == 0)
        {
            if(timeType == kFileTimeTypeCreation)
                return tempStat.st_ctime; // Some implementations of stat define ctime to be something other than creation time.
            else if(timeType == kFileTimeTypeLastModification)
                return tempStat.st_mtime;
            else if(timeType == kFileTimeTypeLastAccess)
                return tempStat.st_atime;
        }

        return 0;

}

EAIO_API time_t File::GetTime(const char8_t* pPath, FileTimeType timeType)
{

            struct _stat tempStat;
            const int result = _stat(pPath, &tempStat);
    
        if(result == 0)
        {
            if(timeType == kFileTimeTypeCreation)
                return tempStat.st_ctime; // Some implementations of stat define ctime to be something other than creation time.
            else if(timeType == kFileTimeTypeLastModification)
                return tempStat.st_mtime;
            else if(timeType == kFileTimeTypeLastAccess)
                return tempStat.st_atime;
        }

        return 0;

}




///////////////////////////////////////////////////////////////////////////////
// File::SetTime
//
EAIO_API bool File::SetTime(const char16_t* pPath, int nFileTimeTypeFlags, time_t nTime)
{

            _utimbuf timbuf;
    
        // Not supported by most platforms:
        //if(nFileTimeTypeFlags & kFileTimeTypeCreation)
        //    timbuf.modtime = nTime;
        //else
        //    timbuf.modtime = GetFileTime(pPath, kFileTimeTypeCreation);
    
        if(nFileTimeTypeFlags & kFileTimeTypeLastModification)
            timbuf.modtime = nTime;
        else
            timbuf.modtime = File::GetTime(pPath, kFileTimeTypeLastModification);
    
        if(nFileTimeTypeFlags & kFileTimeTypeLastAccess)
            timbuf.actime  = nTime;
        else
            timbuf.actime = File::GetTime(pPath, kFileTimeTypeLastAccess);
    
            return (_wutime(pPath, &timbuf) == 0);

}

EAIO_API bool File::SetTime(const char8_t* pPath, int nFileTimeTypeFlags, time_t nTime)
{

            _utimbuf timbuf;
    
        // Not supported by most platforms:
        //if(nFileTimeTypeFlags & kFileTimeTypeCreation)
        //    timbuf.modtime = nTime;
        //else
        //    timbuf.modtime = GetFileTime(pPath, kFileTimeTypeCreation);
    
        if(nFileTimeTypeFlags & kFileTimeTypeLastModification)
            timbuf.modtime = nTime;
        else
            timbuf.modtime = File::GetTime(pPath, kFileTimeTypeLastModification);
    
        if(nFileTimeTypeFlags & kFileTimeTypeLastAccess)
            timbuf.actime  = nTime;
        else
            timbuf.actime = File::GetTime(pPath, kFileTimeTypeLastAccess);
    
            return (_utime(pPath, &timbuf) == 0);

}




///////////////////////////////////////////////////////////////////////////////
// File::ResolveAlias
//
EAIO_API File::ResolveAliasResult File::ResolveAlias(const char16_t* pPathSource, char16_t* pPathDestination, 
                                                     uint32_t nDestLength)
{
    ResolveAliasResult result = kRARNone;
    if(pPathDestination && (pPathSource != pPathDestination))
        pPathDestination[0] = 0;

        // Try to ignore these typedefs. They come from Microsoft's 'objbase.h' header.
        typedef HRESULT(STDAPICALLTYPE *CoInitializeFunctionType)(LPVOID);
        typedef void   (STDAPICALLTYPE *CoUninitializeFunctionType)(void);
        typedef HRESULT(STDAPICALLTYPE *CoCreateInstanceFunctionType)(REFCLSID, LPUNKNOWN, DWORD, REFIID, LPVOID FAR*);

        const size_t nStrlen = EAIOStrlen16(pPathSource);

        if(nStrlen > 0)
        {
            size_t posExt = nStrlen - 1;

            while(posExt > 0)
            {
                if(pPathSource[posExt] == '.')
                {
                    if(!StrEq16(pPathSource + posExt, L".lnk"))
                        posExt = 0;
                    break;
                }
                posExt--;
            }

            if(posExt > 0) // If the file name ends with ".lnk"...
            {
                WIN32_FIND_DATAW fd;
                IPersistFile*    pPersistFile = NULL;
                IShellLinkW*     pShellLink = NULL;
                HRESULT          hResult;
                WCHAR            szPathW[kMaxPathLength + 1];

                fd.cFileName[0] = 0;
                szPathW[0]      = 0;

                HINSTANCE hOle32Library = LoadLibraryA("Ole32.dll");

                if(hOle32Library)
                {
                    CoInitializeFunctionType     CoInitializeFunction     = (CoInitializeFunctionType)    (void*)::GetProcAddress(hOle32Library, TEXT("CoInitialize"));
                    CoUninitializeFunctionType   CoUninitializeFunction   = (CoUninitializeFunctionType)  (void*)::GetProcAddress(hOle32Library, TEXT("CoUninitialize"));
                    CoCreateInstanceFunctionType CoCreateInstanceFunction = (CoCreateInstanceFunctionType)(void*)::GetProcAddress(hOle32Library, TEXT("CoCreateInstance"));

                    // Initialize COM
                    if(CoInitializeFunction && CoUninitializeFunction && CoCreateInstanceFunction)
                    {
                        hResult = CoInitializeFunction(NULL);
                        if(SUCCEEDED(hResult))
                        {
                            // Create an instance of shell link interface
                            hResult = CoCreateInstanceFunction(CLSID_ShellLink, NULL, CLSCTX_INPROC_SERVER, IID_IShellLinkW, (void**)&pShellLink);
                            if(SUCCEEDED(hResult))
                            {
                                // Get the persist file interface
                                hResult = pShellLink->QueryInterface(IID_IPersistFile,(void**)&pPersistFile);
                                if(SUCCEEDED(hResult))
                                {
                                    // Load the persist file with the link
                                    hResult = pPersistFile->Load((LPCWSTR)pPathSource, 0);
                                    if(SUCCEEDED(hResult))
                                    {
                                        // Resolve the link
                                        hResult = pShellLink->Resolve(NULL, SLR_NO_UI);
                                        if(SUCCEEDED(hResult))
                                        {
                                            // Get the path from the link
                                            hResult = pShellLink->GetPath(szPathW, sizeof(szPathW)/sizeof(szPathW[0]), &fd, 0);
                                            if(hResult == S_OK)
                                            {
                                                // Copy to destination
                                                if(EAIOStrlen16(szPathW) < kMaxPathLength)
                                                {
                                                    EAIOStrlcpy16(pPathDestination, (char16_t*)szPathW, nDestLength);
                                                    result = kRARAlias;
                                                }
                                            }
                                        }
                                    }
                                    pPersistFile->Release();
                                    pPersistFile = NULL;
                                }
                                pShellLink->Release();
                                pShellLink = NULL;
                            }
                            CoUninitializeFunction();
                        }
                    }
                    ::FreeLibrary(hOle32Library);
                }
            }
        }

    if(pPathDestination && (pPathSource != pPathDestination))
        EAIOStrlcpy16(pPathDestination, pPathSource, nDestLength);

    return result;
}

EAIO_API File::ResolveAliasResult File::ResolveAlias(const char8_t* pPathSource, char8_t* pPathDestination,
                                                     uint32_t nDestCapacity)
{
    using namespace EA::IO::Path;

    // We do back and forth conversions, but Windows applications (which would be using this 
    // function the most) are often going to be using 16 bit strings for UI and file systems anyway.
    PathString16 pathSource16;
    ConvertPathUTF8ToUTF16(pathSource16, pPathSource);

    char16_t pDirectory16[kMaxDirectoryLength];
    const File::ResolveAliasResult result = File::ResolveAlias(&pathSource16[0], pDirectory16, kMaxDirectoryLength);

    StrlcpyUTF16ToUTF8(pPathDestination, nDestCapacity, pDirectory16);

    return result;
}




///////////////////////////////////////////////////////////////////////////////
// File::CreateAlias
//
EAIO_API bool File::CreateAlias(const char16_t* pDestinationPath, const char16_t* pShortcutPath, 
                                const char16_t* pShortcutDescription, const char16_t* pShortcutArguments)
{
    using namespace EA::IO::Path;


        HRESULT      hResult;
        IShellLinkW* pShellLink;
        
        hResult = ::CoInitialize(NULL); // Or ::CoInitializeEx(NULL, COINIT_MULTITHREADED);
 
        if(SUCCEEDED(hResult))
        {
            // Get a pointer to the IShellLink interface.
            hResult = ::CoCreateInstance(CLSID_ShellLink, NULL, CLSCTX_INPROC_SERVER, IID_IShellLinkW, (VOID**)&pShellLink);

            if(SUCCEEDED(hResult))
            {
                // Set the shortcut parameters
                pShellLink->SetPath(pDestinationPath);

                if(pShortcutDescription && pShortcutDescription[0])
                    pShellLink->SetDescription(pShortcutDescription);

                if(pShortcutArguments && pShortcutArguments[0])
                    pShellLink->SetArguments(pShortcutArguments);

                // Query IShellLink for the IPersistFile interface for saving the
                // shortcut in persistent storage.
                IPersistFile* pPersistFile; 
                hResult = pShellLink->QueryInterface(IID_IPersistFile, (VOID**)&pPersistFile);
           
                if(SUCCEEDED(hResult))
                {
                    PathString16 path16(pShortcutPath);

                    // Make sure the shortcut ends with .lnk. It won't work unless it has this extension.
                    if (!StrEq16(Path::GetFileExtension(path16), L".lnk"))
                        Path::Join(path16, L".lnk");

                    // Remove the file if it exists.
                    File::Remove(path16.c_str());

                    // Save the link by calling IPersistFile::Save.
                    hResult = pPersistFile->Save(path16.c_str(), TRUE);
                    pPersistFile->Release();
                }

                pShellLink->Release();
            }

            ::CoUninitialize();
        }

        return SUCCEEDED(hResult);

}

EAIO_API bool File::CreateAlias(const char8_t* /*pDestinationPath*/,     const char8_t* /*pShortcutPath*/, 
                                const char8_t* /*pShortcutDescription*/, const char8_t* /*pShortcutArguments*/)
{
    // To do.
    return false;
}



///////////////////////////////////////////////////////////////////////////////
// IsVolumeAvailable
//
EAIO_API bool IsVolumeAvailable(const char16_t* pPath, int /*timeoutMS*/)
{
        // Complete this for other platforms as required.
        (void)pPath;
        return true;
}

EAIO_API bool IsVolumeAvailable(const char8_t* pPath, int /*timeoutMS*/)
{
        // Complete this for other platforms as required.
        (void)pPath;
        return true;
}


///////////////////////////////////////////////////////////////////////////////
// InitializeFileSystem
//
EAIO_API bool InitializeFileSystem(bool /*bInitialize*/)
{
        // Nothing to do on this platform.
        return true;
}


///////////////////////////////////////////////////////////////////////////////
// MakeTempPathName
//
EAIO_API bool MakeTempPathName(char16_t* pPath, const char16_t* pDirectory, const char16_t* pFileName, const char16_t* pExtension, uint32_t nDestPathLength)
{
        using namespace EA::IO::Path;
    

        // User must allocate space for the resulting temp path.
        if(pPath)
        {
            static const char16_t pFileNameDefault[]  = { 't', 'e', 'm', 'p', 0 };
            static const char16_t pExtensionDefault[] = { '.', 't', 'm', 'p', 0 };

        time_t nTime = clock();

            char16_t tempPath[kMaxPathLength];

            if(!pFileName)
                pFileName = pFileNameDefault;

            if(!pExtension)
                pExtension = pExtensionDefault;

            if(!pDirectory)
            {
                if (!GetTempDirectory(tempPath))
                    return false;
                pDirectory = tempPath;
            }

            for(size_t i = 0; i < 64; i++, nTime--)
            {
                char16_t buffer[20];

                PathString16 tempFilePath(pDirectory);
                Path::Append(tempFilePath, pFileName);
                
                tempFilePath.operator+=(EAIOItoa16((uint32_t)nTime, buffer));
                tempFilePath.operator+=(pExtension);

                uint32_t nSrcPathLength = (uint32_t)tempFilePath.length();
                if (nSrcPathLength > nDestPathLength)
                    break;

                EAIOStrlcpy16(pPath, tempFilePath.c_str(), nDestPathLength);

                FileStream fileStream(pPath);
                if(fileStream.Open(kAccessFlagReadWrite, kCDCreateNew))
                {
                    fileStream.Close();
                    return true;
                }
                else if(fileStream.GetState() == kFSErrorWriteProtect) // If the directory cannot be written to...
                    break;
            }

        }

        return false;
}

EAIO_API bool MakeTempPathName(char8_t* pPath, const char8_t* pDirectory, const char8_t* pFileName, const char8_t* pExtension, uint32_t nDestPathLength)
{
        // User must allocate space for the resulting temp path.
        if(pPath)
        {
            static const char8_t pFileNameDefault[]  = { 't', 'e', 'm', 'p', 0 };
            static const char8_t pExtensionDefault[] = { '.', 't', 'm', 'p', 0 };

			time_t nTime = clock();

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

            for(size_t i = 0; i < 64; i++, nTime--)
            {
                char8_t buffer[20];

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
                else if(fileStream.GetState() == kFSErrorWriteProtect) // If the directory cannot be written to...
                    break;
            }
        }

        return false;
}


///////////////////////////////////////////////////////////////////////////////
// GetTempDirectory
//
EAIO_API int GetTempDirectory(char16_t* pDirectory, uint32_t nMaxPermittedLength)
{
    // Check to see if the user manually specified a temp directory.
    if(gpTempDirectory[0])
    {
        EAIOStrlcpy16(pDirectory, gpTempDirectory, nMaxPermittedLength);
        return (int)EAIOStrlen16(pDirectory);
    }


        DWORD dwResult = GetTempPathW(kMaxDirectoryLength, pDirectory);
        if(dwResult) // If succeeded...
        {
            // According to the docs, these arguments can overlap like this.
            dwResult = GetLongPathNameW(pDirectory, pDirectory, nMaxPermittedLength-1);
            if(Path::EnsureTrailingSeparator(pDirectory, kMaxDirectoryLength))
                dwResult++;
            return (int)dwResult;
        }
        return -1;

}

EAIO_API int GetTempDirectory(char8_t* pDirectory, uint32_t nMaxPermittedLength)
{
    // You need to use the "game data" functionality to dynamically create
    // scratch drive space on the console. You would then call our SetTempDirectory function
    // with the system-assigned directory and then this GetTempDirectory function will 
    // return it.

    // Check to see if the user manually specified a temp directory.
    if(gpTempDirectory[0])
        return (int)(unsigned)StrlcpyUTF16ToUTF8(pDirectory, nMaxPermittedLength, gpTempDirectory);


        char16_t pPath16[kMaxPathLength];
        
        DWORD dwResult = GetTempPathW(kMaxDirectoryLength, pPath16);
        if(dwResult) // If succeeded...
        {
            // According to the docs, these arguments can overlap like this.
            GetLongPathNameW(pPath16, pPath16, kMaxDirectoryLength);
            return (int)(unsigned)StrlcpyUTF16ToUTF8(pDirectory, kMaxDirectoryLength, pPath16);
        }

        return -1;

}


///////////////////////////////////////////////////////////////////////////////
// SetTempDirectory
//
EAIO_API bool SetTempDirectory(const char16_t* pDirectory)
{
    using namespace EA::IO::Path;

    const size_t nDirectoryLength = EAIOStrlen16(pDirectory);

    // We use +2 because we need room for trailing nul char and for a possible appending
    // of a path separator. Strictly speaking, we should make the code a little smarter
    // about the required length and the trailing path separator, but we are simple for now.
    if(nDirectoryLength + 2 <= kTempDirectoryLength)
    {
        if(pDirectory[0])
        {
            EAIOStrlcpy16(gpTempDirectory, pDirectory, kTempDirectoryLength);
            Path::EnsureTrailingSeparator(gpTempDirectory, kTempDirectoryLength);

            if(!Directory::Exists(gpTempDirectory))
                return Directory::Create(gpTempDirectory);
        }
        else
            gpTempDirectory[0] = 0;

        return true;
    }

    return false;
}

EAIO_API bool SetTempDirectory(const char8_t* pDirectory)
{
    EA::IO::Path::PathString16 path16;
    ConvertPathUTF8ToUTF16(path16, pDirectory);

    return SetTempDirectory(path16.c_str());
}




///////////////////////////////////////////////////////////////////////////////
// GetDriveFreeSpace
//
EAIO_API uint64_t GetDriveFreeSpace(const char16_t* pPath)
{
        char16_t pPathResult[kMaxDriveLength];

        // Convert "vol:\abc\def\ghi" to "vol:\"
        // Convert "abc/def/ghi" to "/abc/"
        char16_t* p0 = pPathResult;

        for(const char16_t* p1 = pPath; *p1 && ((*p1 == ':') || !IsFilePathSeparator(*p1)); ++p0, ++p1)
            *p0 = *p1;

        *p0++ = kFilePathSeparator16;
        *p0   = 0;

            ULARGE_INTEGER freeBytesAvailableToCaller;
            ULARGE_INTEGER totalBytes;
            ULARGE_INTEGER totalFreeBytes;

            // Windows requires there to be a trailing path separator if the path is a UNC path.
            Path::EnsureTrailingSeparator(pPathResult, kMaxDriveLength);

            if(GetDiskFreeSpaceExW(pPathResult, &freeBytesAvailableToCaller, &totalBytes, &totalFreeBytes))
                return freeBytesAvailableToCaller.QuadPart;
            // Fall through

    

    return UINT64_C(0xffffffffffffffff);
}


EAIO_API uint64_t GetDriveFreeSpace(const char8_t* pPath)
{
        EA::IO::Path::PathString16 path16;
        ConvertPathUTF8ToUTF16(path16, pPath);

        return GetDriveFreeSpace(path16.c_str());
}



///////////////////////////////////////////////////////////////////////////////
// GetDriveName
//
EAIO_API int GetDriveName(const char16_t* pPath, char16_t* pName)
{
    EA_ASSERT(*pPath); // Assert that the input is non-empty.

        WCHAR root[kMaxPathLength];

        if(GetVolumePathNameW((const WCHAR*)pPath, root, kMaxPathLength))
        {
            DWORD fsFlags;

            if(GetVolumeInformationW(root, (WCHAR*)pName, (DWORD)kMaxVolumeNameLength, NULL, 0, &fsFlags, NULL, 0))
                return (int)EAIOStrlen16(pName);
        }

        pName[0] = 0;
        return -1;

}

EAIO_API int GetDriveName(const char8_t* /*pPath*/, char8_t* /*pName*/)
{
    // To do.
    return false;
}




///////////////////////////////////////////////////////////////////////////////
// GetDriveFreeSpace
//
EAIO_API int GetDriveSerialNumber(const char16_t* pPath, char16_t* pSerialNumber)
{
        WCHAR root[kMaxPathLength];

        if (GetVolumePathNameW((const WCHAR*)pPath, root, kMaxPathLength))
        {
            DWORD fsFlags;
            DWORD dwSerialNumber;

            if (GetVolumeInformationW(root, NULL, 0, &dwSerialNumber, NULL, &fsFlags, NULL, 0))
            {
                wsprintfW(pSerialNumber, L"%04X-%04X", dwSerialNumber >> 16, dwSerialNumber & 0xffff);
                return 10;
            }
        }

        pSerialNumber[0] = 0;
        return 0;

}

EAIO_API int GetDriveSerialNumber(const char8_t* pPath, char8_t* pSerialNumber)
{
    char16_t pPath16[kMaxPathLength];
    char16_t pSerialNumber16[kMaxVolumeSerialNumberLength];

    StrlcpyUTF8ToUTF16(pPath16, kMaxPathLength, pPath);
    const int result = GetDriveSerialNumber(pPath16, pSerialNumber16);
    StrlcpyUTF16ToUTF8(pSerialNumber, kMaxVolumeSerialNumberLength, pSerialNumber16);

    return result;
}



///////////////////////////////////////////////////////////////////////////////
// GetDriveTypeValue
//
EAIO_API DriveType GetDriveTypeValue(const char16_t* pPath)
{

        const uint32_t nDriveType = GetDriveTypeW(pPath);

        switch (nDriveType)
        {
            case DRIVE_FIXED:     return kDriveTypeFixed;
            case DRIVE_REMOVABLE: return kDriveTypeRemovable;
            case DRIVE_REMOTE:    return kDriveTypeRemote;
            case DRIVE_CDROM:     return kDriveTypeCD;
            case DRIVE_RAMDISK:   return kDriveTypeRAM;
        }

        return kDriveTypeUnknown;

}

EAIO_API DriveType GetDriveTypeValue(const char8_t* pPath)
{
    // To do: Flip the 16 and 8 bit versions so the 8 bit version does all the work.
    EA::IO::Path::PathString16 path16;
    ConvertPathUTF8ToUTF16(path16, pPath);

    return GetDriveTypeValue(path16.c_str());
}



///////////////////////////////////////////////////////////////////////////////
// GetDriveInfo
// 9/25/09 CSidhall - Removed unused drive info 

///////////////////////////////////////////////////////////////////////////////
// Directory::Exists
//
EAIO_API bool Directory::Exists(const char16_t* pDirectory)
{
    if(pDirectory && *pDirectory)
    {

            const DWORD dwAttributes = ::GetFileAttributesW(pDirectory);
            return ((dwAttributes != INVALID_FILE_ATTRIBUTES) && ((dwAttributes & FILE_ATTRIBUTE_DIRECTORY) != 0));

    }

    return false;
}

EAIO_API bool Directory::Exists(const char8_t* pDirectory)
{
    if(pDirectory && *pDirectory)
    {

            const DWORD dwAttributes = ::GetFileAttributesA(pDirectory);
            return ((dwAttributes != INVALID_FILE_ATTRIBUTES) && ((dwAttributes & FILE_ATTRIBUTE_DIRECTORY) != 0));

    }

    return false;
}


///////////////////////////////////////////////////////////////////////////////
// Directory::EnsureExists
//
EAIO_API bool Directory::EnsureExists(const char16_t* pDirectory)
{
    if(pDirectory && *pDirectory)
    {
        if(Directory::Exists(pDirectory))
            return true;
        return Directory::Create(pDirectory);    
    }
    return false;
}

EAIO_API bool Directory::EnsureExists(const char8_t* pDirectory)
{
    if(pDirectory && *pDirectory)
    {
        if(Directory::Exists(pDirectory))
            return true;
        return Directory::Create(pDirectory);    
    }
    return false;
}



////////////////////////////////////////////////////////////////////////////
// Directory::Create
//
//  This function actually builds a path or file. Note that if the directory
//  didn't exist previously, this function makes it. This function leaves the
//  system as is if not able to create the entry. Thus, this function is not
//  the same as the C runtime 'mkdir' function or similar OS-supplied functions,
//  as they will only create a directory if the parent directory already exists.
//
//  Win32 example:
//      C:\blah1\blah2\blah3\        Directory
//      C:\blah1\                    Part1
//      C:\blah1\blah2\              Part2
//      C:\blah1\blah2\blah3\        Part3
//
//  Unix example:
//      /blah1/blah2/blah3/          Directory
//      /blah1/                      Part1
//      /blah1/blah2/                Part2
//      /blah1/blah2/blah3/          Part3
//
//  Win32 UNC Example:
//      \\SomeServer\SomeVolume\blah2\blah3\     // Win32
//
namespace
{
    bool CreateDirectoryInternal(const char16_t* pDirectory)
    {
        // To consider: Perhaps this function should not worry about trailing separators -- 
        // it is currently called only from from Create and Create is taking care of them.
            if(pDirectory[0]) // Our code below requires at least one char in the string.
            {
                char16_t path[kMaxPathLength];

                const size_t nStrlen = EAIOStrlen16(pDirectory);

                // Windows doesn't like it when the directory path ends with a 
                // separator (e.g. '\') character, so we remove it if needed.
                if(IsFilePathSeparator(pDirectory[nStrlen - 1]))  // If the path ends with a '\' char...
                {
                    EAIOStrlcpy16(path, pDirectory, nStrlen);   // Force 0-terminator in place of directory separator.
                    pDirectory = path;
                }

                const BOOL bResult = CreateDirectoryW(pDirectory, NULL);
                return bResult || (GetLastError() == ERROR_ALREADY_EXISTS);
            }

            return false;

    }
}


///////////////////////////////////////////////////////////////////////////////
// Create
//
// This function does a smart directory create. It doesn't require that the 
// parent directory exist beforehand as with conventional directory creation
// API functions.
// 
EAIO_API bool Directory::Create(const char16_t* pDirectory)
{
    using namespace Path;
    PathString16 fullDirPath(pDirectory);
    Path::EnsureTrailingSeparator(fullDirPath);
    
    // Path for Scratchwork
    char16_t scratchPath[kMaxPathLength];
    bool success(true);

    PathString16::iterator it = GetPathComponentEnd(fullDirPath.begin(), fullDirPath.end(), 1); 
    PathString16::iterator firstNewDirIt = NULL; // Mark the top-most directory that we created; if we need to do a rollback, we recursively remove this directory.
    while (it != fullDirPath.end())
    {
        
        // Example of first iteration:
        //  fullDirPath = "/usr/foo/bar/"
        //           it =     "/foo/bar/"
        // it - fullDirPath.begin() = "/usr"   // We reserve +1 for NULL terminator (don't want path separator for directory creation).
        uint32_t nLength = (uint32_t)(it - fullDirPath.begin() + 1);
        if (nLength >= (uint32_t)kMaxDirectoryLength)
        {
            success = false;
            break;
        }

        // If necessary, create the next component in our path
        EAIOStrlcpy16(scratchPath, fullDirPath.c_str(), nLength);
        if (!Directory::Exists(scratchPath))
        {
            if (!CreateDirectoryInternal(scratchPath))
            {
                success = false;
                break;
            }
            else if (!firstNewDirIt)
                firstNewDirIt = it;
        }

        // Advance forward by one component from our current component
        it = GetPathComponentEnd(it, fullDirPath.end(), 1);
    }

    // An error occurred mid-way through, remove any directories that we've created thus far.
    if (!success && firstNewDirIt)
    {
        // We can do a recursive wipe here because we know that the directory hierarchy we're 
        // about to remove was been created solely by us.
        EAIOStrlcpy16(scratchPath, fullDirPath.c_str(), firstNewDirIt - fullDirPath.begin() + 1);
        Directory::Remove(scratchPath);
    }

    return success;
}


EAIO_API bool Directory::Create(const char8_t* pDirectory)
{
    EA::IO::Path::PathString16 path16;
    ConvertPathUTF8ToUTF16(path16, pDirectory);

    return Directory::Create(path16.c_str());
}



namespace
{
    bool RemoveDirectoryRecursiveInternal(char16_t* pDirectory, size_t pathLen) 
    {
        DirectoryIterator            di;
        DirectoryIterator::EntryList entryList(DirectoryIterator::EntryList::allocator_type(EASTL_NAME_VAL(EAIO_ALLOC_PREFIX "FileUtil")));

        size_t namePos = pathLen;
        bool   success = true;
        
        if (di.Read(pDirectory, entryList, 0, kDirectoryEntryFile | kDirectoryEntryDirectory))
        {
            // Make sure the directory ends in a path separator. Note: If we have a
            // relative drive specification (c:), we don't want to add a backslash to it!
            if (pathLen)
            {
                char16_t lastChar = pDirectory[pathLen - 1];

                if (!IsFilePathSeparator(lastChar) && (lastChar != EA::IO::kFilePathDriveSeparator16) && (pathLen < (kMaxPathLength - 1)))
                    pDirectory[namePos++] = kFilePathSeparator16;
            }

            for(DirectoryIterator::EntryList::const_iterator it(entryList.begin()), itEnd(entryList.end()); it != itEnd; ++it)
            {
                const DirectoryIterator::Entry& ent = *it;
                const eastl_size_t nameLen = ent.msName.length();

                if ((nameLen + pathLen) > (kMaxPathLength - 1))
                    success = false;
                else
                {
                    ent.msName.copy(pDirectory + namePos, nameLen);
                    pDirectory[namePos + nameLen] = 0;

                    if (ent.mType == kDirectoryEntryDirectory)
                    {
                        if (!RemoveDirectoryRecursiveInternal(pDirectory, namePos + nameLen))
                            success = false;
                    }
                    else
                    {
                        if (!File::Remove(pDirectory))
                            success = false;
                    }
                }
            }
        }

        pDirectory[pathLen] = 0;
        
        if(!Directory::Remove(pDirectory, false))
            success = false;

        return success;
    }
}



///////////////////////////////////////////////////////////////////////////////
// Remove
//
EAIO_API bool Directory::Remove(const char16_t* pDirectory, bool bAllowRecursiveRemoval)
{
    if (bAllowRecursiveRemoval)
    {
        // Create a mutable version of the path.
        char16_t path[kMaxPathLength];
        EAIOStrlcpy16(path, pDirectory, kMaxPathLength);

        return RemoveDirectoryRecursiveInternal(path, EAIOStrlen16(path));
    }
    else // Non-recursive 
    {
            // Windows doesn't like it when the directory path ends with a 
            // separator (e.g. '\') character, so we correct for this if needed.
            const size_t nStrlen = EAIOStrlen16(pDirectory);

            if(!IsFilePathSeparator(pDirectory[nStrlen - 1]))
                return 0 != RemoveDirectoryW(pDirectory);

            // Else we need to remove the separator.
            char16_t path[kMaxPathLength];
            EAIOStrlcpy16(path, pDirectory, nStrlen); // Force NULL terminator in place of directory separator
            return 0 != RemoveDirectoryW(pDirectory);

    }
}

EAIO_API bool Directory::Remove(const char8_t* pDirectory, bool bAllowRecursiveRemoval)
{
    EA::IO::Path::PathString16 path16;
    ConvertPathUTF8ToUTF16(path16, pDirectory);

    return Directory::Remove(path16.c_str(), bAllowRecursiveRemoval);
}



///////////////////////////////////////////////////////////////////////////////
// Rename
//
EAIO_API bool Directory::Rename(const char16_t* pDirectoryOld, const char16_t* pDirectoryNew)
{

        // Under Win32 you can rename a directory by using the file move API.
        return (File::Move(pDirectoryOld, pDirectoryNew) != 0);

}

EAIO_API bool Directory::Rename(const char8_t* pDirectoryOld, const char8_t* pDirectoryNew)
{
        EA::IO::Path::PathString16 oldPath16;
        ConvertPathUTF8ToUTF16(oldPath16, pDirectoryOld);

        EA::IO::Path::PathString16 newPath16;
        ConvertPathUTF8ToUTF16(newPath16, pDirectoryNew);

        return Directory::Rename(oldPath16.c_str(), newPath16.c_str());
}



///////////////////////////////////////////////////////////////////////////////
// Copy
//
EAIO_API bool Directory::Copy(const char16_t* pDirectorySource, const char16_t* pDirectoryDestination, bool bRecursive, bool bOverwriteIfAlreadyPresent)
{
    using namespace EA::IO::Path;

    bool bResult;

    PathString16 source16(pDirectorySource);
    Path::Normalize(source16);
    PathString16 dest16(pDirectoryDestination);
    Path::Normalize(dest16);

    bResult = Directory::Exists(source16.c_str());

    if(bResult)
    {
        bResult = Directory::EnsureExists(dest16.c_str());

        if(bResult)
        {
            DirectoryIterator            directoryIterator;
            DirectoryIterator::EntryList entryList(DirectoryIterator::EntryList::allocator_type(EASTL_NAME_VAL(EAIO_ALLOC_PREFIX "EAFileNotification/FSEntry")));

            directoryIterator.Read(source16.c_str(), entryList, NULL, bRecursive ? kDirectoryEntryDirectory | kDirectoryEntryFile : kDirectoryEntryFile);

            for(DirectoryIterator::EntryList::iterator it(entryList.begin()); it != entryList.end(); ++it)
            {
                DirectoryIterator::Entry& entry = *it;

                PathString16 sourcePath16(source16);
                PathString16 destPath16(dest16);

                Path::Join(sourcePath16, entry.msName.c_str());
                Path::Join(destPath16, entry.msName.c_str());

                if((entry.mType == kDirectoryEntryDirectory) && bRecursive)
                {
                    if(!Directory::Copy(sourcePath16.c_str(), destPath16.c_str(), bOverwriteIfAlreadyPresent))
                        bResult = false;
                }
                else if(entry.mType == kDirectoryEntryFile)
                {
                    if(!File::Copy(sourcePath16.c_str(), destPath16.c_str(), bOverwriteIfAlreadyPresent))
                        bResult = false;
                }
            }
        }
    }

    return bResult;
}

EAIO_API bool Directory::Copy(const char8_t* /*pDirectorySource*/, const char8_t* /*pDirectoryDestination*/, 
                     bool /*bRecursive*/, bool /*bOverwriteIfAlreadyPresent*/)
{
    // To do
    return false;
}



///////////////////////////////////////////////////////////////////////////////
// SetAttributes
//
EAIO_API bool Directory::SetAttributes(const char16_t* pBaseDirectory, int nAttributeMask, bool bEnable, bool bRecursive, bool bIncludeBaseDirectory)
{
    using namespace EA::IO::Path;

    bool bResult = true;

    PathString16 base16(pBaseDirectory);
    Path::Normalize(base16);

    if(bIncludeBaseDirectory)
        bResult = Directory::SetAttributes(base16.c_str(), nAttributeMask, bEnable);

    if(bResult)
    {
        DirectoryIterator            directoryIterator;
        DirectoryIterator::EntryList entryList(DirectoryIterator::EntryList::allocator_type(EASTL_NAME_VAL(EAIO_ALLOC_PREFIX "EAFileNotification/FSEntry")));

        bResult = (directoryIterator.Read(base16.c_str(), entryList, NULL, bRecursive ? kDirectoryEntryDirectory | kDirectoryEntryFile : kDirectoryEntryFile) != 0);

        if(bResult)
        {
            for(DirectoryIterator::EntryList::iterator it(entryList.begin()); it != entryList.end(); ++it)
            {
                DirectoryIterator::Entry& entry = *it;

                PathString16 path16(base16);
                Path::Join(path16, entry.msName.c_str());

                if((entry.mType == kDirectoryEntryDirectory) && bRecursive)
                {
                    if(!Directory::SetAttributes(path16.c_str(), nAttributeMask, bEnable))
                        bResult = false;
                }
                else if(entry.mType == kDirectoryEntryFile)
                {
                    if(!File::SetAttributes(path16.c_str(), nAttributeMask, bEnable))
                        bResult = false;
                }
            }
        }
    }

    return bResult;
}

EAIO_API bool Directory::SetAttributes(const char8_t* pBaseDirectory, int nAttributeMask, bool bEnable, bool bRecursive, bool bIncludeBaseDirectory)
{
    EA::IO::Path::PathString16 path16;
    ConvertPathUTF8ToUTF16(path16, pBaseDirectory);

    return Directory::SetAttributes(path16.c_str(), nAttributeMask, bEnable, bRecursive, bIncludeBaseDirectory);
}



///////////////////////////////////////////////////////////////////////////////
// GetTime
//
EAIO_API time_t Directory::GetTime(const char16_t* pPath, FileTimeType timeType)
{
    return File::GetTime(pPath, timeType);
}

EAIO_API time_t Directory::GetTime(const char8_t* pPath, FileTimeType timeType)
{
    return File::GetTime(pPath, timeType);
}



///////////////////////////////////////////////////////////////////////////////
// SetTime
//
EAIO_API bool Directory::SetTime(const char16_t* pPath, int nFileTimeTypeFlags, time_t nTime)
{
    return File::SetTime(pPath, nFileTimeTypeFlags, nTime);
}
 
EAIO_API bool Directory::SetTime(const char8_t* pPath, int nFileTimeTypeFlags, time_t nTime)
{
    return File::SetTime(pPath, nFileTimeTypeFlags, nTime);
}
 


///////////////////////////////////////////////////////////////////////////////
// GetCurrentWorkingDirectory
//
EAIO_API int Directory::GetCurrentWorkingDirectory(char16_t* pDirectory, uint32_t nMaxPermittedLength)
{
        // Windows has a GetCurrentDirectory function, but for symmetry with 
        // our SetCurrentWorkingDirectory function, we use _wgetcwd().
        const wchar_t* const pResult = _wgetcwd(pDirectory, kMaxDirectoryLength - 1); // '- 1' so that a '/' can possibly be appended.

        if(pResult)
        {
            Path::EnsureTrailingSeparator(pDirectory, nMaxPermittedLength);
            return (int)wcslen(pDirectory);
        }

        pDirectory[0] = 0;
        return 0;

}

EAIO_API int Directory::GetCurrentWorkingDirectory(char8_t* pDirectory, uint32_t nMaxPermittedLength)
{
        char16_t pDirectory16[kMaxPathLength];

        Directory::GetCurrentWorkingDirectory(pDirectory16, kMaxPathLength);

        return (int)(unsigned)StrlcpyUTF16ToUTF8(pDirectory, nMaxPermittedLength, pDirectory16);
}



///////////////////////////////////////////////////////////////////////////////
// SetCurrentWorkingDirectory
//
EAIO_API bool Directory::SetCurrentWorkingDirectory(const char16_t* pDirectory)
{
        // Windows has a SetCurrentDirectory function, but it doesn't change any settings
        // in the C runtime. _wchdir calls the OS SetCurrentDirectory and sets the C runtime values.
        return _wchdir(pDirectory) == 0;

}

EAIO_API bool Directory::SetCurrentWorkingDirectory(const char8_t* pDirectory)
{
        EA::IO::Path::PathString16 path16;
        ConvertPathUTF8ToUTF16(path16, pDirectory);

        return Directory::SetCurrentWorkingDirectory(path16.c_str());
}


EAIO_API bool IsDirectoryEmpty(const char16_t* pDirectory, int nDirectoryEntryFlags, bool bRecursive)
{
    EA::IO::DirectoryIterator::EntryList entryList;
    EA::IO::DirectoryIterator it;

    if(bRecursive)
        return it.ReadRecursive(pDirectory, entryList, NULL, nDirectoryEntryFlags, true, true, 1) == 0;

    return it.Read(pDirectory, entryList, NULL, nDirectoryEntryFlags, 1) == 0;
}

EAIO_API bool IsDirectoryEmpty(const char8_t*  pDirectory, int nDirectoryEntryFlags, bool bRecursive)
{
    EA::IO::Path::PathString16 path16;
    ConvertPathUTF8ToUTF16(path16, pDirectory);
	
	return IsDirectoryEmpty(path16.c_str(), nDirectoryEntryFlags, bRecursive);
}

///////////////////////////////////////////////////////////////////////////////
// GetSpecialDirectory
//
EAIO_API int GetSpecialDirectory(SpecialDirectory specialDirectory, char16_t* pDirectory, 
                                 bool bEnsureDirectoryExistence, uint32_t nMaxPermittedLength)
{
    pDirectory[0] = 0;


        static const int kFolderTable[][2] = {
            0,                              0,                              // kSpecialDirectoryNone
            0,                              0,                              // kSpecialDirectoryTemp
            0,                              0,                              // kSpecialDirectoryOperatingSystem
            CSIDL_BITBUCKET,                0,                              // kSpecialDirectoryOperatingSystemTrash
            CSIDL_FONTS,                    0,                              // kSpecialDirectoryOperatingSystemFonts
            0,                              0,                              // kSpecialDirectoryCurrentApplication
            CSIDL_DESKTOPDIRECTORY,         CSIDL_COMMON_DESKTOPDIRECTORY,  // kSpecialDirectoryUserDesktop
            CSIDL_COMMON_DESKTOPDIRECTORY,  CSIDL_DESKTOPDIRECTORY,         // kSpecialDirectoryCommonDesktop
            CSIDL_APPDATA,                  CSIDL_COMMON_APPDATA,           // kSpecialDirectoryUserApplicationData
            CSIDL_COMMON_APPDATA,           CSIDL_APPDATA,                  // kSpecialDirectoryCommonApplicationData
            CSIDL_PERSONAL,                 CSIDL_COMMON_DOCUMENTS,         // kSpecialDirectoryUserDocuments
            CSIDL_COMMON_DOCUMENTS,         CSIDL_PERSONAL,                 // kSpecialDirectoryCommonDocuments
            CSIDL_MYMUSIC,                  CSIDL_COMMON_MUSIC,             // kSpecialDirectoryUserMusic
            CSIDL_COMMON_MUSIC,             CSIDL_MYMUSIC,                  // kSpecialDirectoryCommonMusic
            CSIDL_PROGRAMS,                 CSIDL_COMMON_PROGRAMS,          // kSpecialDirectoryUserProgramsShortcuts
            CSIDL_COMMON_PROGRAMS,          CSIDL_PROGRAMS,                 // kSpecialDirectoryCommonProgramsShortcuts
        };

        // Add some assert to help test if the user has modified the SpecialDirectory enumeration.
        EA_COMPILETIME_ASSERT(kSpecialDirectoryOperatingSystemTrash    == 3);
        EA_COMPILETIME_ASSERT(kSpecialDirectoryCommonProgramsShortcuts == 15);

        bool success = false;

        switch(specialDirectory)
        {
            case kSpecialDirectoryTemp:
                success = (GetTempPathW(nMaxPermittedLength, pDirectory) != 0);
                if(success)
                    Path::EnsureTrailingSeparator(pDirectory, nMaxPermittedLength);
                break;

            case kSpecialDirectoryOperatingSystem:
                success = (GetWindowsDirectoryW(pDirectory, nMaxPermittedLength) != 0);
                if(success)
                    Path::EnsureTrailingSeparator(pDirectory, nMaxPermittedLength);
                break;

            case kSpecialDirectoryCurrentApplication:
            {
                char16_t filename[MAX_PATH];

                if(GetModuleFileNameW(GetModuleHandle(NULL), filename, MAX_PATH))
                {
                    LPWSTR pFilePart;

                    // GetFullPathNameW memsets pDirectory with nMaxPermittedLength chars.
                    if(GetFullPathNameW(filename, nMaxPermittedLength, pDirectory, &pFilePart))
                    {
                        *pFilePart = 0;
                        success = true;
                    }
                }
                break;
            }

            case kSpecialDirectoryOperatingSystemTrash:
            case kSpecialDirectoryOperatingSystemFonts:
            case kSpecialDirectoryUserDesktop:
            case kSpecialDirectoryCommonDesktop:
            case kSpecialDirectoryUserApplicationData:
            case kSpecialDirectoryCommonApplicationData:
            case kSpecialDirectoryUserDocuments:
            case kSpecialDirectoryCommonDocuments:
            case kSpecialDirectoryUserMusic:
            case kSpecialDirectoryCommonMusic:
            case kSpecialDirectoryUserProgramsShortcuts:
            case kSpecialDirectoryCommonProgramsShortcuts:
            case kSpecialDirectoryNone:
            default:
            {
                const int typeFlags = (specialDirectory == kSpecialDirectoryOperatingSystemTrash) ? SHGFP_TYPE_DEFAULT : SHGFP_TYPE_CURRENT;
                const int idFlags   = bEnsureDirectoryExistence ? CSIDL_FLAG_CREATE : 0;
                int       id        = kFolderTable[specialDirectory][0];

                if(id && SUCCEEDED(SHGetFolderPathW(NULL, id | idFlags, NULL, typeFlags, pDirectory)))
                    success = true;
                else
                {
                    id = kFolderTable[specialDirectory][1];

                    if(id && SUCCEEDED(SHGetFolderPathW(NULL, id | idFlags, NULL, typeFlags, pDirectory)))
                        success = true;
                }

                if(success)
                {
                    const size_t len = wcslen(pDirectory);

                    // Check if the path ends in a separator and add it if not.
                    if(len && !IsFilePathSeparator(pDirectory[len - 1]))
                    {
                        if(len >= (kMaxPathLength - 1))
                            success = false; // We don't have enough room to add the path separator, so fail the call.
                        else
                        {
                            pDirectory[len]     = kFilePathSeparator16;
                            pDirectory[len + 1] = 0;
                        }
                    }
                }
            }
            break;
        }

        // To do: We have a small API compliance issue here in that as of this writing,     
        //        the function is documented to not modify pDirectory if the function fails.
        //        However, that is necessarily not the case here.
        if(success && bEnsureDirectoryExistence && !Directory::Exists(pDirectory)) // If it succeeded but the directory couldn't be made...
            success = false;
        if(success)
            return (int)EAIOStrlen16(pDirectory);
        return -1;

}

EAIO_API int GetSpecialDirectory(SpecialDirectory specialDirectory, char8_t* pDirectory, 
                        bool bEnsureDirectoryExistence, uint32_t nMaxPermittedLength)
{
        EA::IO::Path::PathString16 path16;
        path16.resize(nMaxPermittedLength);

        if(GetSpecialDirectory(specialDirectory, &path16[0], bEnsureDirectoryExistence, nMaxPermittedLength))
        {
            return (int)StrlcpyUTF16ToUTF8(pDirectory, nMaxPermittedLength, path16.c_str());
        }
        
        return -1;
}



///////////////////////////////////////////////////////////////////////////////
// Reserved Windows path names
//
static const char16_t* pReservedNamesWin32[] =
{
    EA_CHAR16("con"),  EA_CHAR16("prn"),  EA_CHAR16("aux"),  EA_CHAR16("clock$"), EA_CHAR16("nul"),    // These reserved words (alone or with any extension) cannot be used 
    EA_CHAR16("com1"), EA_CHAR16("com2"), EA_CHAR16("com3"), EA_CHAR16("com4"),   EA_CHAR16("com5"),   // as the name of a file, directory, server, or volume. We define these
    EA_CHAR16("com6"), EA_CHAR16("com7"), EA_CHAR16("com8"), EA_CHAR16("com9"),   EA_CHAR16("lpt1"),   // outside #ifdef EA_PLATFORM_WINDOWS because this function is meant to 
    EA_CHAR16("lpt2"), EA_CHAR16("lpt3"), EA_CHAR16("lpt4"), EA_CHAR16("lpt5"),   EA_CHAR16("lpt6"),   // be able to test Win32 paths while running under Unix, etc.
    EA_CHAR16("lpt7"), EA_CHAR16("lpt8"), EA_CHAR16("lpt9")
};


// Returns a string (sCurrentComponent) of the characters from current position up to but not including the next path separator char.
// If there is no '/' (or platform-appropriate path separator), returns the rest of the string.
// Sets nCurrentPosition to be one past the '/' char or one past the last char in the string.
// May possibly return an empty string, in the case of two successive '/' chars.
// Returns true if there were any charaters left to read.
// Returns false if the input position is last the end of the string.
//
static bool GetNextPathComponent(const EA::IO::Path::PathString16& sInputPath, EA::IO::Path::PathString16& sCurrentComponent, eastl_size_t& nCurrentPosition)
{
    const eastl_size_t nInputPathLength = sInputPath.length();

    if(nCurrentPosition < nInputPathLength)
    {
        eastl_size_t i;

        for(i = nCurrentPosition; i < nInputPathLength; i++)
        {
            if(IsFilePathSeparator(sInputPath[i]))
            {
                sCurrentComponent.assign(sInputPath, nCurrentPosition, i - nCurrentPosition);
                nCurrentPosition = i+1;
                return true;
            }
        }

        sCurrentComponent.assign(sInputPath, nCurrentPosition, i-nCurrentPosition);
        nCurrentPosition = i;
        return true;
   }

   return false;
}



///////////////////////////////////////////////////////////////////////////////
// IsFileNameCharValid
//
// Returns true if the input character can always be used in a path file name.
// You cannot use this function one-by-one on a file name to determine if the file
// name is valid, as there are special combination cases that make this so.
//
bool IsFileNameCharValid(char16_t c, FileSystem fileSystemType)
{
    switch (fileSystemType)
    {
        case kFileSystemUnix:       // General Unix paths
        case kFileSystemISO9660:
        case kFileSystemJoliet:
        case kFileSystemUDF:
        case kFileSystemNone:
            return (c != '/');

        case kFileSystemWindows:
        case kFileSystemFAT:
        case kFileSystemFATX:
        case kFileSystemFAT32:
        case kFileSystemNTFS:
        case kFileSystemUDFX:
        default:
            return ((c != '<') && (c != '>') && (c != ':') && (c != '\\') && 
                    (c != '/') && (c != '\"') && (c != '|') && (c != '*') && (c != '?'));
    }

    // return true;  // The above switch handles all possibilities.
}


///////////////////////////////////////////////////////////////////////////////
// IsFileNameStringValid
//
// Returns true if the input file name is valid for the file system.
//
bool IsFileNameStringValid(const char16_t* pName, FileSystem fileSystemType)
{
	using namespace EA::IO::Path;

	PathString16 sName(pName);

	if(sName.length() < (eastl_size_t)kMaxFileNameLength) // Problem: This value is tied to the compilation target and not the fileSystemType.
	{
		//Check for bad characters. This is an OK technique even for multi-byte locales.
		for(eastl_size_t i(0), iEnd(sName.length()); i < iEnd; i++)
		{
			const char16_t c = sName[i];  //We don't have to worry about multi-byte text because we use UTF8 which is not hostile to what we do here.

			if(!IsFileNameCharValid(c, fileSystemType))
				return false;
		}

		// Do reserved word tests
		if((fileSystemType == kFileSystemWindows) || 
			(fileSystemType == kFileSystemFAT)     ||
			(fileSystemType == kFileSystemFAT32)   ||
			(fileSystemType == kFileSystemNTFS))
		{ 
			PathString16 sCurrentComponentLowerCaseMinusExtension(sName);
			sCurrentComponentLowerCaseMinusExtension.make_lower();

			// Remove anything past a dot because that's insignificant to Windows when testing reserved words.
			const eastl_size_t nDotPosition(sCurrentComponentLowerCaseMinusExtension.find('.'));
			if(nDotPosition != PathString16::npos)
				sCurrentComponentLowerCaseMinusExtension.erase(nDotPosition);

			// Also trim off any spaces at the end because those are insignificant to Windows when testing reserved words.
			sCurrentComponentLowerCaseMinusExtension.rtrim();

			for(eastl_size_t i(0); i < (sizeof(pReservedNamesWin32) / sizeof(pReservedNamesWin32[0])); i++)
			{
				if(sCurrentComponentLowerCaseMinusExtension == pReservedNamesWin32[i])
					return false;
			}
		}

		// Components that consist of just '.' and/or ' ' chars are illegal.
		// The Win32 documentation doesn't specifically state that these combinations
		// are illegal, but in practice they are. Thus, "...", "   ", and ". ." are
		// all invalid.
		if(sName.find_first_not_of(EA_CHAR16(". ")) == PathString16::npos)
			return false;

		return true;
	}

	return false;
}






bool IsDirectoryNameStringValid(const char16_t* pName, FileSystem fileSystemType)
{
    return IsFileNameStringValid(pName, fileSystemType); // This works for all currently supported platforms.
}


///////////////////////////////////////////////////////////////////////////////
// IsFilePathStringValid
//
// Here are some helper URLs from Microsoft, though they are actually not complete:
//    http://msdn.microsoft.com/library/default.asp?url=/library/en-us/dnfiles/html/msdn_longfile.asp
//    http://msdn.microsoft.com/library/default.asp?url=/library/en-us/fileio/storage_7mn9.asp
//    http://msdn.microsoft.com/library/default.asp?url=/library/en-us/fileio/volref_6wfi.asp
//    http://msdn.microsoft.com/library/default.asp?url=/library/en-us/vccore98/HTML/_core_device_names_for_windows.asp
//
// Recognized file system types include:
//     Unix:      UNIX, 
//     Macintosh: HFS, 
//     Windows:   FAT (same as VFAT for Windows 95 and later), HPFS, NTFS 
//     CDROM:     UDF, CDFS (Windows), Joliet, Rockridge (Unix compatible)

bool IsFilePathStringValid(const char16_t* pPath, FileSystem fileSystemType)
{
    using namespace EA::IO::Path;

    bool                bReturnValue = false;
    PathString16  sPath(pPath);
	const char16_t      pReservedCharactersWin32[] = EA_CHAR16("<>:\"|*?"); // We don't include '/' and '\' because we deal with them separately.

    // Do path length tests
        if(sPath.length() < 3)                  // A length less than 3 chars cannot be a full path, as Windows requires at least a drive or UNC prefix.
            return false;

        if(sPath.find(L"\\\\?\\UNC\\") == 0)    // Under Windows, if a path is prefixed with `\\?\UNC\` then it has no length limit.
            sPath.erase(2, 6);                  // Convert (e.g.) `\\?\UNC\server\volume\Temp\temp.txt` to `\\server\volume\Temp\temp.txt`.
        else if(sPath.find(L"\\\\?\\") == 0)    // Under Windows, if a path is prefixed with `\\?\` then it has no length limit.
            sPath.erase(0, 4);                  // Convert (e.g.) `\\?\C:\Temp\temp.txt` to `C:\Temp\temp.txt`.
        else if(sPath.length() > kMaxPathLength)
            return false;


    // Break up the path into separate components.
    // We'll analyze them below.
    PathString16 sDrive, sDirectory, sFileName, sExtension;

    Split(sPath.c_str(), &sDrive, &sDirectory, &sFileName, &sExtension);

    // Do validation per file system
    if((fileSystemType == kFileSystemWindows) || 
       (fileSystemType == kFileSystemFAT)     ||
       (fileSystemType == kFileSystemFAT32)   ||
       (fileSystemType == kFileSystemNTFS))
    { 
        // Set the return value to true until proven false for the rest of this section.
        eastl_size_t nPosition = 0;
        bReturnValue = true;

        //Go through each directory and the file name in the path and see if 
        //it has any of the above reserved names. Note that directory names
        //can't be "com1.txt" any more than file names can't be "com1.txt".
		if(sPath.find(EA_CHAR16("\\\\")) == 0) // If it is a UNC path... (\\<server>\<volume>\<dir path>). '/' is not accepted as part of the UNC path prefix.
        {
            const eastl_size_t nPosition2 = 1;
            const eastl_size_t nPosition3 = sPath.find('\\', 2);
            const eastl_size_t nPosition4 = sPath.find('\\', nPosition3 + 1); // We know from earlier that there are at least 4 '\' chars.

            if((nPosition3 > (nPosition2 + 1)) && (nPosition4 > (nPosition3 + 1)))
            {
                const PathString16 sServer(sPath, nPosition2 + 1, nPosition3 - nPosition2);
                const PathString16 sVolume(sPath, nPosition3 + 1, nPosition4 - nPosition3);

                // We're a little conservative with the server and volume name checks here.
                // Their limitations don't appear to be well documented, so we err on acceptance.
                if(sServer.find_first_of(pReservedCharactersWin32) != PathString16::npos)
                    bReturnValue = false;
                else if(sVolume.find_first_of(pReservedCharactersWin32) != PathString16::npos)
                    bReturnValue = false;
                else
                {
                    PathString16 sServerLowerCaseMinusExtension(sServer);
                    PathString16 sVolumeLowerCaseMinusExtension(sVolume);

                    sServerLowerCaseMinusExtension.make_lower();
                    const eastl_size_t nServerDotPosition(sServerLowerCaseMinusExtension.find('.'));    
                    if(nServerDotPosition != PathString16::npos)
                        sServerLowerCaseMinusExtension.erase(nServerDotPosition);

                    sVolumeLowerCaseMinusExtension.make_lower();
                    const eastl_size_t nVolumeDotPosition(sVolumeLowerCaseMinusExtension.find('.'));    
                    if(nVolumeDotPosition != PathString16::npos)
                        sVolumeLowerCaseMinusExtension.erase(nVolumeDotPosition);

                    for(size_t i(0); i < sizeof(pReservedNamesWin32) / sizeof(pReservedNamesWin32[0]); i++)
                    {
                        if(sServerLowerCaseMinusExtension == pReservedNamesWin32[i])
                        {
                            bReturnValue = false;
                            break;
                        }

                        if(sVolumeLowerCaseMinusExtension == pReservedNamesWin32[i])
                        {
                            bReturnValue = false;
                            break;
                        }
                    }
                    nPosition = nPosition4 + 1;
                }
            }
            else
                bReturnValue = false;
        }
        else
        {
            if((sPath.length() >= 3) &&     // Similar to iswalpha(sPath[0])
               ((sPath[0] >= 'a' && sPath[0] <= 'z') || (sPath[0] >= 'A' && sPath[0] <= 'Z')) && // If is a Windows drive letter...
               (sPath[1] == ':') &&
               (sPath[2] == '\\'))       // '/' (as opposed to '\') is allowed as directory separators but not after "C:". That's Microsoft for ya.
            {
                nPosition = 3;
            }
            else
                bReturnValue = false;
        }

        if(bReturnValue) // If the code directly above found everything OK so far...
        {
            PathString16 sCurrentComponent;

            while(bReturnValue && GetNextPathComponent(sPath, sCurrentComponent, nPosition))
            {
                if(bReturnValue)
                {
                    if(sCurrentComponent.empty() || (sCurrentComponent.length() > (eastl_size_t)kMaxFileNameLength))
                    {
                        bReturnValue = false;
                        break;
                    }
                }

                // Do reserved word tests
                if(bReturnValue)
                {
                    PathString16 sCurrentComponentLowerCaseMinusExtension(sCurrentComponent);

                    sCurrentComponentLowerCaseMinusExtension.make_lower();
                    const eastl_size_t nDotPosition(sCurrentComponentLowerCaseMinusExtension.find('.'));

                    if(nDotPosition != PathString16::npos)
                        sCurrentComponentLowerCaseMinusExtension.erase(nDotPosition);

                    for(size_t i(0); i < sizeof(pReservedNamesWin32) / sizeof(pReservedNamesWin32[0]); i++)
                    {
                        if(sCurrentComponentLowerCaseMinusExtension == pReservedNamesWin32[i])
                        {
                            bReturnValue = false;
                            break;
                        }
                    }

                    if(bReturnValue)
                    {
                        // Components that consist of just '.' and/or ' ' chars are illegal.
                        // The Win32 documentation doesn't specifically state that these combinations
                        // are illegal, but in practice they are. Thus, "...", "    ", and ". ." are
                        // all invalid.
						if(sCurrentComponent.find_first_not_of(EA_CHAR16(". ")) == PathString16::npos)
                            bReturnValue = false;
                    }
                }

                // Do reserved character tests
                if(bReturnValue)
                {
                    // Windows documentation says that chars 0-31 are invalid, but curiously, if you
                    // type such characters into the Windows 2000 file browser for a name, it accepts them.
                    // Don't be fooled. It actually isn't accepting them, but is instead converting them
                    // to alternative Unicode equivalents for the same glyphs before accepting them. 

                    for(char16_t c(0); c <= 31; c++)
                    {
                        if(sCurrentComponent.find(c) != PathString16::npos)
                            bReturnValue = false;
                    }

                    if(bReturnValue)
                    {
                        if(sCurrentComponent.find_first_of(pReservedCharactersWin32) != PathString16::npos)
                            bReturnValue = false;
                    }
                }
            }
        }
    }
    else
    {
        EA_FAIL_MESSAGE("IsFilePathStringValid: Not implemented for the given file system.");
        return true; // Play it safe and return true. If we were using this function to implement in a security-conscious way, we might instead choose to return false.
    }

   return bReturnValue;
}


} // namespace IO


} // namespace EA














