/*
Copyright (C) 2009, 2010, 2011, 2012, 2013, 2014 Electronic Arts, Inc.  All rights reserved.

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

// Created by Nicki Vankoughnett


// 4/16/10 CSidhall - Update:
// Changed and fixed cache directive detection.
// Added read loop with timeout and a larger buffer with instant retry if file might be done.
// Added checksum for corrupted files.
// Added version number to iniFile.
// Added open file limiter to limit too many open handles.
// Made system use max-age or expires values versus a 5 hour default.
// Added a shared buffer for blocking reads.

#include "config.h"
#include "TransportHandlerDiskCacheEA.h"
#include "SharedBuffer.h"
#include "INetMIMEEA.h"
#include <EAIO/PathString.h>
#include <EAIO/EAStreamMemory.h>
#include <EAIO/EAFileBase.h>
#include <EAIO/EAFileUtil.h>
#include <EAIO/EAFileDirectory.h>
#include <internal/include/EAWebKitString.h>
#include <internal/include/EAWebKitAssert.h>
#include <internal/include/EAWebKitEASTLHelpers.h>
#include <internal/include/EAWebKitNewDelete.h>
#include <stdio.h>
#include <time.h>
#include <EAWebKit/EAWebKit.h>  
#include "DateEA.h"           
#include <EAIO/FnEncode.h>
#include <EAWebKit/EAWebKitFileSystem.h>
#include <internal/include/EAWebkit_p.h>
#include <wtf/CryptographicallyRandomNumber.h>

namespace EA
{

namespace WebKit
{

const uint32_t	kDefaultAccessCountBeforeMaintenance	= 40;                 // Number of file accesses before we purge values.
const char16_t* kDefaultCacheDirectoryName				= EA_CHAR16("cache\\"); 
const char16_t* kDefaultIniFileName						= EA_CHAR16("FileCache.ini");
const char8_t*  kDefaultIniFileSection					= "Cache Entries: V 1.01.00"; // Change version # if format changes. 
const char8_t*  kCacheLineEntry							= "Cache Entry:";      // Each line
const char8_t*  kCacheChecksum							= "Cache Checksum:";   // Each line
const char16_t* kCachedFileExtension					= EA_CHAR16(".cache");
const char16_t* kSearchCachedFileExtension				= EA_CHAR16("*.cache");          // This should work on all platforms
const int32_t   kMaxTransferLoops						= 100;                 // Just a timeout safety which would limit the read loops in a frame for a file    
const double    kMaxTransferLoopTime					= (1.0/60.0);          // Timeout for large file read loops - ~16 ms. This is probably not the most appropriate value. 
const uint32_t  kCacheDownloadBufferSize				= 1024 * 64;           // Shared download buffer size  
const uint32_t  kCacheDownloadBufferAlign				= 16;                  // Shared download buffer alignment  
const uint32_t	kOverHeadFileHandlesReqd				= 2;					// (1 FileCache.ini, 1 cache file write operation)						

static int32_t GetTimeInSeconds()   
{
    return (int32_t)time(NULL);
}

// abaldeva:
// It is expensive solution that requires creating string object but since this code is
// not called a lot, it is alright.
static uint32_t AtoU32(const char16_t* pString)
{
	EA::WebKit::FixedString16_128 string16(pString);
	EA::WebKit::FixedString8_128 string8;
	EA::WebKit::ConvertToString8(string16,string8);

	return strtoul (string8.c_str(), NULL, 10 );
}


// Simple byte checksum. This is byte based so that we can checksum slices as
// we download them.  If we had the entire file at hand, we would use a higher
// resolution checksum system.
// This can yield duplicates but should be good enough for a simple error check.
static uint32_t GetByteChecksum(const char* buffer, const int32_t size, const uint32_t checksum)
{
    
    const int   kHashShift = 8; // Shift to generate a more unique number in order to make it order dependent (detect swaps)
    uint32_t    sum = checksum;
    int32_t     index =0;

    uint32_t workSum;
    // We run a 8 byte loop to limit branching
    int32_t loops = size >> 3;  
    while(loops--)
    {
        workSum = sum << kHashShift;
        workSum +=buffer[index+0];    
        sum +=workSum;
        workSum = sum << kHashShift;
        workSum +=buffer[index+1];    
        sum +=workSum;
        workSum = sum << kHashShift;
        workSum +=buffer[index+2];    
        sum +=workSum;
        workSum = sum << kHashShift;
        workSum +=buffer[index+3];    
        sum +=workSum;
        workSum = sum << kHashShift;
        workSum +=buffer[index+4];    
        sum +=workSum;     
        workSum = sum << kHashShift;
        workSum +=buffer[index+5];    
        sum +=workSum;
        workSum = sum << kHashShift;
        workSum +=buffer[index+6];    
        sum +=workSum;
        workSum = sum << kHashShift;
        workSum +=buffer[index+7];    
        sum +=workSum;
        index +=8;
    }

    // Deal with remainder if any in a 1 byte loop
    int32_t  remain = size & 0x07;         
    while(remain--)
    {
        workSum = sum << kHashShift;
        workSum +=buffer[index];    
        sum +=workSum;
        index +=1;
    }
    return sum;
}

// Converts a uint32_t value to a char16_t with 16 bytes (so total size is 32 bytes) 
static char16_t* Uint32ToAsciiChar16(uint32_t value, char16_t* buffer)
{
    buffer[15] = 0;
    uint32_t i = 14;
    
    // Write out the character representation of 'value' starting with the highest power of 10
    for (; value > 0; value /= 10)
        buffer[i--] = (char16_t)('0' + (value % 10));

    return &buffer[i + 1];
}


// pPath is set to a valid file name if this function returns true.
static bool MakeTempPathName(char16_t* pPath, const char16_t* pDirectory, const char16_t* pFileName, const char16_t* pExtension)
{
	if(!pDirectory || !pPath)
	{
		return false;
	}

    uint32_t nDestPathLength = EA::WebKit::FileSystem::kMaxPathLength;

    if(EA::WebKit::FileSystem* pFS = EA::WebKit::GetFileSystem())
    {
        static const char16_t pFileNameDefault[]  = { 't', 'e', 'm', 'p', 0 };
        static const char16_t pExtensionDefault[] = { '.', 't', 'm', 'p', 0 };

        if(!pFileName)
            pFileName = pFileNameDefault;

        if(!pExtension)
            pExtension = pExtensionDefault;

		//try up to 64 times to generate a random number and in turn a random file name that does not exist
        for(size_t i = 0; i < 64; i++) 
        {
			char16_t buffer[16];
			uint32_t randomNumber = WTF::cryptographicallyRandomNumber();

            EA::IO::Path::PathString16 tempFilePath(pDirectory);
            EA::IO::Path::Append(tempFilePath, pFileName);
            tempFilePath +=(Uint32ToAsciiChar16(randomNumber, buffer));
            tempFilePath +=(pExtension);

            uint32_t nSrcPathLength = (uint32_t)tempFilePath.length();
            if (nSrcPathLength > nDestPathLength)
                break;

            EA::IO::EAIOStrlcpy16(pPath, tempFilePath.c_str(), nDestPathLength);
            
            EA::WebKit::FixedString16_128 path16(pPath);
            EA::WebKit::FixedString8_128 path8;
            EA::WebKit::ConvertToString8(path16, path8);
            if(!pFS->FileExists(path8.c_str()))
                return true;
        }
    }

    return false;
}

/******************Implementation of CacheResponseHeaderInfo******************/

/*
From:  http://www.w3.org/Protocols/rfc2616/rfc2616-sec14.html

Cache-Control   = "Cache-Control" ":" 1#cache-directive

 cache-response-directive =
       "public"                               ; Section 14.9.1
     | "private" [ "=" <"> 1#field-name <"> ] ; Section 14.9.1
     | "no-cache" [ "=" <"> 1#field-name <"> ]; Section 14.9.1
     | "no-store"                             ; Section 14.9.2
     | "no-transform"                         ; Section 14.9.5
     | "must-revalidate"                      ; Section 14.9.4
     | "proxy-revalidate"                     ; Section 14.9.4
     | "max-age" "=" delta-seconds            ; Section 14.9.3
     | "s-maxage" "=" delta-seconds           ; Section 14.9.3
     | cache-extension                        ; Section 14.9.6
cache-extension = token [ "=" ( token | quoted-string ) ]


*/

CacheResponseHeaderInfo::CacheResponseHeaderInfo() 
{   
	Reset();
}

void CacheResponseHeaderInfo::Reset()
{
    m_CanCacheToDisk = false;
    m_NoStoreFound = false;
    m_PrivateFound = false;
    m_PublicFound = false;
    m_MaxAgeFound = false;
    m_MaxAge = 0;
    m_ExpiresFound = false;
    m_RevalidateFound = false;
    m_NoTransformFound = false;
}


// Extract the cache control header directive to see if we can cache this file
// If it returns true, we want to exit any further search for the file can't be cached.
bool CacheResponseHeaderInfo::ExtractCacheControlDirectives(const EA::WebKit::FixedString16_128& directive)
{
    bool exitFlag = false;      
    
    // Note: We don't currently check on must-revalidate because we do it by default anyway.
	if(directive.find(EA_CHAR16("no-store")) != EA::WebKit::FixedString16_128::npos) 
    {
        m_NoStoreFound  = true;
        exitFlag        = true;                 // We can exit the search for we can't cache this file.
    }
    else if( (directive.find(EA_CHAR16("private")) != EA::WebKit::FixedString16_128::npos) ||  
             (directive.find(EA_CHAR16("no-cache")) != EA::WebKit::FixedString16_128::npos) )
    {
        // Currently we don't support user private storage as would need this from the app.
        m_PrivateFound  = true;
        exitFlag        = true;                 // We can exit the search 
    }
    else if( (directive.find(EA_CHAR16("max-age"))) != EA::WebKit::FixedString16_128::npos)
    {
        uint32_t maxAge = 0;
		//abaldeva: added string conversions to avoid calling swscanf.
		EA::WebKit::FixedString8_128 directive8;
		EA::WebKit::ConvertToString8(directive, directive8);
        int32_t result = sscanf( directive8.c_str(), "max-age=%u", &maxAge);
        if(result == 1)
        {
			m_MaxAgeFound = true;           
			m_MaxAge = maxAge;          
        }
    }
    else if(directive.find(EA_CHAR16("public")) != EA::WebKit::FixedString16_128::npos)
    {
        m_PublicFound = true;
    }
    else if(directive.find(EA_CHAR16("must-revalidate")) != EA::WebKit::FixedString16_128::npos)
    {
        m_RevalidateFound = true;
    }
    else if(directive.find(EA_CHAR16("no-transform")) != EA::WebKit::FixedString16_128::npos)
    {
        m_NoTransformFound = true;
    }

    return exitFlag;
}

bool CacheResponseHeaderInfo::ExtractExpiresDirective(const FixedString16_128& directive)
{
    EA::WebKit::FixedString8_128 header8;    
    EA::WebKit::ConvertToString8(directive, header8);

    time_t expireTime = EA::WebKit::ReadDateString(header8.c_str());
	if(expireTime)
	{
		m_ExpiresFound = true;  // Found a valid expiration to cache

		time_t curTime = time(NULL);
		time_t deltaTime = expireTime - curTime;
		
		m_MaxAge = (uint32_t) deltaTime;
	}
    return m_ExpiresFound;
}

bool CacheResponseHeaderInfo::SetDirectivesFromHeader(const EA::WebKit::TransportInfo* pTInfo)
{
    bool exitFlag = false;   
    
    Reset();

    EA::WebKit::FixedString8_128 uri8;
    EA::WebKit::ConvertToString8(*GetFixedString(pTInfo->mURI), uri8);
	const HeaderMap& headers = *GetHeaderMap(pTInfo->mHeaderMapIn);

	// There is a bug in the disk cache system due to which if a .html/.htm/.xml file is cached, it can't be reloaded from cache. 
	// Fails with a "-5" error. That needs to be resolved. This is a hack to work around that problem.
	if(uri8.find(".html")!= EA::WebKit::FixedString16_128::npos || uri8.find(".htm")!= EA::WebKit::FixedString16_128::npos || uri8.find(".xml")!= EA::WebKit::FixedString16_128::npos)
	{
		return false;
	}
	//Another check on the content type in case the url does not specify the type of content.
	EA::WebKit::HeaderMap::const_iterator contentTypeEntry = headers.find(EA_CHAR16("Content-Type"));
	if(contentTypeEntry != headers.end())
	{
		if(contentTypeEntry->second.find(EA_CHAR16("text/html")) != EA::WebKit::FixedString16_128::npos 
			|| contentTypeEntry->second.find(EA_CHAR16("text/xml")) != EA::WebKit::FixedString16_128::npos
			|| contentTypeEntry->second.find(EA_CHAR16("application/xml")) != EA::WebKit::FixedString16_128::npos
			)
		{
			return false;
		}
	}

	// Reject caching paths with possible query 
    if(uri8.find("?") != EA::WebKit::FixedString16_128::npos)
    {
        return false;
    }

	if(!pTInfo->mpTransportHandler->CanCacheToDisk())// Some protocols should not be cached as they may already be local
	{
		EAW_ASSERT_MSG(false,"Original Transport handler does not allow caching." );
		return false;
	}
    
	EA::WebKit::HeaderMap::const_iterator entry = headers.find(EA_CHAR16("Cache-Control"));
    if (entry != headers.end())
    {
        FixedString16_128 curHeader;	
		curHeader.assign(entry->second.c_str()); 
 
        //evaluate header directive substrings within curHeader
        const char16_t comma = EA_CHAR16(',');
        const char16_t space = EA_CHAR16(' ');
        EA::WebKit::FixedString16_128 tmp;
        uint32_t a=0, b;
        uint32_t len = curHeader.length();

      
        do
        {
            // Shrink spaces before the first character:
            while(a < len && (curHeader[a] == space))
                ++a;
            
            // Find the token span by using a comma as limiter 
            b = a+1;
            while(b < len && (curHeader[b] != comma))
                ++b;

            exitFlag = ExtractCacheControlDirectives(curHeader.substr(a,b));
            
            a = b+1;        // Update position to next token after the comma

        } while((a <= len) && (!exitFlag));
    }

    // Rapid exit if we can't cache
    if(exitFlag)
        return false;

    // Check for the expired only if max-age is was not set
    if(!m_MaxAgeFound)
    {
        entry = headers.find(EA_CHAR16("Expires"));
        if (entry != headers.end())
        {
            if(!ExtractExpiresDirective(entry->second.c_str()))
                return false;
        }
    }
    
        
    // analyze results
    if( (!m_NoStoreFound) || (!m_PrivateFound) )
    {
        if(m_PublicFound)
        {
            if((m_MaxAgeFound) || (m_ExpiresFound))
            {
                m_CanCacheToDisk = true;
            }
            else
            {
                // We have a public file but no age information so don't cache.
            }
        }
        else if(uri8.find("https:") == EA::WebKit::FixedString16_128::npos)
        {
            // Not defined if public or private so assume we can chache if the is a time limit set
            // however we refuse SSL unless it has the public directive. 
            if(m_MaxAgeFound)
            {
                m_CanCacheToDisk = true;
            }
            else if (m_ExpiresFound)
            {
                // No cache key controls found but there is a valid expired within our limits
                m_CanCacheToDisk = true;
            }
        }
    }
    return m_CanCacheToDisk;
}

/*****************Implementation of TransportHandlerDiskCache*****************/

TransportHandlerDiskCache::TransportHandlerDiskCache()
  : mbInitialized(false)
  , mbEnabled(false)
  , msCacheDirectory ()
  , msIniFileName(kDefaultIniFileName)
  , mDataMap()
  , mnCacheAccessCount (0) 
  , mnCacheAccessCountSinceLastMaintenance(0)
  , mCurCachedFilesCount(0)
  , mCurOpenFilesCount(0)
  , mpCacheDownloadBuffer(0)
{
	
}

TransportHandlerDiskCache::~TransportHandlerDiskCache()
{
    Shutdown(NULL);
    RemoveCacheDownloadBuffer();
}

bool TransportHandlerDiskCache::HandleAvailableToReadFile() const
{
	return (mCurOpenFilesCount + kOverHeadFileHandlesReqd) < (int32_t)mDiskCacheInfo.mMaxNumberOfOpenFiles;
}

bool TransportHandlerDiskCache::EnableDiskCache(bool enabled) 
{ 
    if(enabled)
    {
        mbEnabled = enabled; 

        //Nicki Vankoughnett:  The return of the Init() function is handled by the base class,
        //and will return true even if it did not properly initialize.
        //so we call init and determine success by checking against initialized.
        Init(NULL);
        mbEnabled = mbInitialized;
    }
    else
    {
        Shutdown(NULL);
    }
    return mbEnabled;
}

bool TransportHandlerDiskCache::Init(const char16_t* pScheme)
{
    bool bReturnValue = true;
    if(!mbInitialized && mbEnabled)
    {
        EA::WebKit::FileSystem* pFS = EA::WebKit::GetFileSystem();
        if(msCacheDirectory.length() && pFS)
        {
             EA::WebKit::FixedString16_128 path16(msCacheDirectory.c_str());
             EA::WebKit::FixedString8_128 path8;
             EA::WebKit::ConvertToString8(path16, path8);

            if(!pFS->DirectoryExists(path8.c_str()))
            {
                bReturnValue = pFS->MakeDirectory(path8.c_str());
                EAW_ASSERT_MSG(bReturnValue, "TransportHandlerDiskCache::Init(): Unable to create cache directory.");
            }

            ReadCacheIniFile();
            RemoveUnusedCachedFiles();
            mbInitialized = true;     
        }
    }

    return bReturnValue;
}

bool TransportHandlerDiskCache::Shutdown(const char16_t* /*pScheme*/)
{
    if(mbInitialized)
    {
        DoPeriodicCacheMaintenance();
        UpdateCacheIniFile();
        ClearCacheMap();
        mbInitialized = false;
    }
    mbEnabled = false;
    return true;
}

// This only works if doing blocking reads.  Async reads would need individual buffers.
char* TransportHandlerDiskCache::GetCacheDownloadBuffer()
{
    if(!mpCacheDownloadBuffer)
    {
        mpCacheDownloadBuffer = (char*) GetAllocator()->MallocAligned(kCacheDownloadBufferSize,kCacheDownloadBufferAlign,0,0,"Cache file download buffer");
    }
	return mpCacheDownloadBuffer;
}

void TransportHandlerDiskCache::RemoveCacheDownloadBuffer()
{
    if(mpCacheDownloadBuffer)
    {
        GetAllocator()->Free(mpCacheDownloadBuffer,0);
        mpCacheDownloadBuffer = 0;    
    }
}

bool TransportHandlerDiskCache::InitJob(TransportInfo* pTInfo, bool& bStateComplete)
{
    using namespace EA::WebKit;

	if(!HandleAvailableToReadFile()) //If no additional handles are available right now, wait for one to become available.
	{
		bStateComplete = false;
		return true;
	}
    // We return true if we feel we can handle the job.
    FileSystem* pFS = GetFileSystem();

    if(pFS != NULL)
    {
        Allocator* pAllocator = GetAllocator();
        FileInfo*  pFileInfo  = new(pAllocator->Malloc(sizeof(FileInfo), 0, "EAWebKit/TransportHandlerDiskCache")) FileInfo; 

        pTInfo->mTransportHandlerData = (uintptr_t)pFileInfo;
        bStateComplete = true;

        //Obtain the name of the temp file that we cached this file under
        EA::WebKit::FixedString8_128 uri8;
        EA::WebKit::ConvertToString8(*GetFixedString(pTInfo->mURI), uri8);
        DataMap::iterator iter = mDataMap.find( uri8 );
        if (iter != mDataMap.end() )
        {
            const Info& cacheFileInfo = iter->second;

            EA::IO::Path::PathString16 sFilePath(msCacheDirectory.c_str());
            EA::IO::Path::Join(sFilePath, cacheFileInfo.msCachedFileName.c_str() );
			EA::WebKit::FixedString16_128 sFilePathStr(sFilePath.c_str());
			EA::WebKit::ConvertToString8(sFilePathStr, *GetFixedString(pTInfo->mPath));
        }

        ++mCurOpenFilesCount;
        return true;
    }

    return false;
}

bool TransportHandlerDiskCache::ShutdownJob(TransportInfo* pTInfo, bool& bStateComplete)
{
    if(pTInfo != NULL && pTInfo->mTransportHandlerData)
    {
        Disconnect(pTInfo, bStateComplete);

        FileInfo*  pFileInfo  = (FileInfo*)pTInfo->mTransportHandlerData;
        Allocator* pAllocator = GetAllocator();

        pFileInfo->~FileInfo();
        pAllocator->Free(pFileInfo, sizeof(FileInfo));

        pTInfo->mTransportHandlerData = 0;
        --mCurOpenFilesCount;

		EAW_ASSERT_MSG(mCurOpenFilesCount>= 0, "Negative job count is not possible");
    }

   // If we have not other pending jobs, might as well remove the buffer to save memory.
   if(mCurOpenFilesCount == 0)
       RemoveCacheDownloadBuffer();

    bStateComplete = true;
    return true;

}

bool TransportHandlerDiskCache::Connect(TransportInfo* pTInfo, bool& bStateComplete)
{
    using namespace EA::WebKit;

    bool bReturnValue = false;

    FileInfo* pFileInfo = (FileInfo*)pTInfo->mTransportHandlerData;
    EAW_ASSERT(pFileInfo != NULL);

    if(pFileInfo)
    {
        FileSystem* pFS = GetFileSystem();
        EAW_ASSERT(pFS != NULL);  // This should be non-NULL because InitJob found it to be non-NULL.

        pFileInfo->mFileObject = pFS->CreateFileObject();

        if(pFileInfo->mFileObject != FileSystem::kFileObjectInvalid)
        {
            if(pFS->OpenFile(pFileInfo->mFileObject, GetFixedString(pTInfo->mPath)->c_str(), FileSystem::kRead, FileSystem::kCDOOpenExisting))
            {
                bReturnValue = true;
            }
            else
            {
                pFS->DestroyFileObject(pFileInfo->mFileObject);
                pFileInfo->mFileObject = FileSystem::kFileObjectInvalid;
            }
        }
    }

    bStateComplete = true;
    return bReturnValue;
}

bool TransportHandlerDiskCache::Disconnect(TransportInfo* pTInfo, bool& bStateComplete)
{
    using namespace EA::WebKit;

    FileInfo* pFileInfo = (FileInfo*)pTInfo->mTransportHandlerData;
    EAW_ASSERT(pFileInfo != NULL);

    if(pFileInfo->mFileObject != FileSystem::kFileObjectInvalid)
    {
        FileSystem* pFS = GetFileSystem();
        EAW_ASSERT(pFS != NULL);

        if(pFS)
        {
            pFS->CloseFile(pFileInfo->mFileObject);
            pFS->DestroyFileObject(pFileInfo->mFileObject);
            pFileInfo->mFileObject = FileSystem::kFileObjectInvalid;
        }
    }
    bStateComplete = true;
    return true;
}

bool TransportHandlerDiskCache::Transfer(TransportInfo* pTInfo, bool& bStateComplete)
{
    using namespace EA::WebKit;

    bool bResult = true;

    FileInfo* pFileInfo = (FileInfo*)pTInfo->mTransportHandlerData;
    EAW_ASSERT(pFileInfo != NULL);
    
    char* pBuffer = GetCacheDownloadBuffer();
    if((pFileInfo->mFileObject != FileSystem::kFileObjectInvalid) && (pBuffer))
    {
        FileSystem* pFS = GetFileSystem();
        EAW_ASSERT(pFS != NULL);

        if(pFileInfo->mFileSize < 0) // If this is the first time through...
        {
            pFileInfo->mFileSize = pFS->GetFileSize(pFileInfo->mFileObject);
            pTInfo->mpTransportServer->SetExpectedLength(pTInfo, pFileInfo->mFileSize);
			EA::WebKit::FixedString8_128 uri8;
			EA::WebKit::ConvertToString8(*GetFixedString(pTInfo->mURI), uri8);
			DataMap::iterator iter = mDataMap.find( uri8 );
			if (iter != mDataMap.end() )
			{
				const Info& cacheFileInfo = iter->second;
				pTInfo->mpTransportServer->SetMimeType(pTInfo,cacheFileInfo.msMIMEContentType.c_str());
			}
            // pTInfo->mpTransportServer->SetEncoding(pTInfo, char* pEncoding);
            // pTInfo->mpTransportServer->SetMimeType(pTInfo);
            // pTInfo->mpTransportServer->HeadersReceived(pTInfo);
        }

        // To consider: Enable async reading of the entire file.  For this to work, would need
        // to directly download to the final data buffer and allow file IO asyn reads.
        double startTime = EA::WebKit::GetTime();
        bool loopFlag = false;  
        int loopCount = kMaxTransferLoops;
        do {
            --loopCount;
            loopFlag = false;  // Reset to not loop 
            const int64_t size = pFS->ReadFileAsync(pFileInfo->mFileObject, pBuffer, kCacheDownloadBufferSize);
            if(size > 0) 
            {
                pTInfo->mpTransportServer->DataReceived(pTInfo, pBuffer, size);
              
                pFileInfo->mCurChecksum = GetByteChecksum(pBuffer,size, pFileInfo->mCurChecksum);

                // Check if we have enough time to keep on going
                double curTime = EA::WebKit::GetTime();
                double deltaTime = curTime - startTime;
                if((deltaTime < kMaxTransferLoopTime) && (loopCount > 0))
                {
                    loopFlag = true;
                }
                else if((size < kCacheDownloadBufferSize) && (loopCount > 0))
                {
                    // We might have just finished so do a last loop so that we can speed things up.    
                    // This is because we also want to close a file as fast as possible.   
                    loopFlag = true;
                    loopCount = 1;  // This will allow for just 1 more run.   
                }
            }
            else if(size == FileSystem::kReadStatusComplete)
            {
                // Completed read
                bStateComplete = true;
                
                // Verify that the checksum is ok
                EA::WebKit::FixedString8_128 uri8;
                EA::WebKit::ConvertToString8(*GetFixedString(pTInfo->mURI), uri8);
                DataMap::iterator iter = mDataMap.find( uri8 );
                if (iter != mDataMap.end() )
                {
                    const Info& cacheFileInfo = iter->second;
                    if(pFileInfo->mCurChecksum == cacheFileInfo.mnChecksum)
                    {
                         bResult = true;    // No errors
                    }
                    else
                    {
                         bResult = false;    // Errors - Might need a way to tell it to retry using http
                        
                         // Remove this file from cache since checksum was suspect
                        InvalidateCachedDataIfRequired(pTInfo);
                    }
                }
                else
                {
                    // We would not expect this as if we are caching a file, we should have the 
                    // cache info available.
                    EAW_ASSERT(0);
                     bResult = false;    // Errors                    
                }
            }
            else if(size == FileSystem::kReadStatusDataNotReady)
            {
                // Nothing to do here as default values are already good.
				//bStateComplete = false;
                //bResult        = true;

				EAW_ASSERT_MSG(!bStateComplete && bResult,"State not correct while waiting for pending data");
            }
			else
			{
				// An error occured
				bStateComplete = true;
				bResult        = false;
			}
        } while(loopFlag);

    }
    else
    {
        bStateComplete = true;
        bResult        = false;
    }

    // For now, set it to either 200 (OK) or 404 (not found).
    if(bResult)
        pTInfo->mResultCode = 200;
    else
        pTInfo->mResultCode = 404;

    if(bStateComplete)
        pTInfo->mpTransportServer->DataDone(pTInfo, bResult);

    return true;
}

void TransportHandlerDiskCache::InvalidateCachedDataIfRequired(const EA::WebKit::TransportInfo* pTInfo)
{
    if(!mbEnabled)
        return;

    //Some HTTP methods MUST cause a cache to invalidate an entity. This is either 
    //the entity referred to by the Request-URI, or by the Location or 
    //Content-Location headers (if present). These methods are: POST, PUT, DELETE.

	bool invalidate = (	pTInfo->mHttpRequestType == EA::WebKit::kHttpRequestTypePOST ||
						pTInfo->mHttpRequestType == EA::WebKit::kHttpRequestTypePUT ||
						pTInfo->mHttpRequestType == EA::WebKit::kHttpRequestTypeDELETE );
    if(invalidate)
    {
        EA::WebKit::FixedString8_128 uri8;
		EA::WebKit::ConvertToString8(*GetFixedString(pTInfo->mURI), uri8);
        DataMap::iterator iter = mDataMap.find( uri8 );
        if (iter != mDataMap.end() )
        {
            const Info& cacheFileInfo = iter->second;
            RemoveCachedFile(cacheFileInfo.msCachedFileName.c_str());
            mDataMap.erase(iter);
        }
    }
}

void TransportHandlerDiskCache::CacheToDisk(const EA::WebKit::FixedString16_128& uriFNameStr, const EA::WebKit::FixedString8_128& mimeStr, const WebCore::SharedBuffer& requestData,const CacheResponseHeaderInfo &cacheHeaderInfo )
{
    // Writes the data in the requestData object to a cache file, and enters the relevant info into the 
    //mDataMap object.

    if(!mbEnabled || !cacheHeaderInfo.CanCacheToDisk()) 
       return;

    bool bSuccess = false;

    EA::WebKit::FixedString8_128 pKey;
    EA::WebKit::ConvertToString8(uriFNameStr, pKey);
    
    
    RemoveCachedData ( pKey ); // if we have something for this key already, purge it

    Info& newInfo ( (*mDataMap.insert ( eastl::make_pair (  pKey, Info() ) ).first).second );
    newInfo.mnDataSize     = 0;
    newInfo.mnLocation     = 0;
    newInfo.mnTimeoutSeconds = 0;
    newInfo.mnTimeCreated  = GetTimeInSeconds ();
    newInfo.mnTimeLastUsed = UINT32_MAX;    // trick so FindLRU will never find un-committed items
    newInfo.mnTimeTimeout  = newInfo.mnTimeCreated;
    newInfo.msCachedFileName = uriFNameStr.c_str();
    newInfo.msMIMEContentType = mimeStr;
    newInfo.mnChecksum = GetByteChecksum(requestData.data(), requestData.size(), 0);

    bool validName = false;
    
	
	EA::WebKit::FixedString16_128 tmp(msCacheDirectory);
	MIMEType mimeType, mimeSubtype;
    bool canCacheMimeType = MIMEStringToMIMETypes( mimeStr.c_str(), mimeType, mimeSubtype, (uint32_t) mimeStr.size()) ;
	if (canCacheMimeType)
    {
        newInfo.msCachedFileName = uriFNameStr.c_str();
        validName = GetNewCacheFileName ( mimeType, mimeSubtype, newInfo.msCachedFileName);
	    if(validName)
			tmp +=newInfo.msCachedFileName;
    }
	else
	{
		mDataMap.erase( pKey );
		return;
	}
    if(!validName)
	{
		EAW_ASSERT_MSG(validName,"Could not generate a valid Name. Not caching.");
		mDataMap.erase( pKey );
		return;
	}
    
    EA::WebKit::FixedString8_128 pathStr;
	EA::WebKit::ConvertToString8( tmp, pathStr );

    // We return true if we feel we can handle the job.
    FileSystem* pFS = GetFileSystem();
    if(pFS != NULL)
    {
        FileInfo fileInfo;

        fileInfo.mFileObject = pFS->CreateFileObject();
        if(fileInfo.mFileObject != FileSystem::kFileObjectInvalid)
        {
            if(pFS->OpenFile(fileInfo.mFileObject, pathStr.c_str(), FileSystem::kWrite, FileSystem::kCDOCreateAlways))
            {
                bSuccess = pFS->WriteFile(fileInfo.mFileObject, requestData.data(), requestData.size());
                pFS->CloseFile(fileInfo.mFileObject);

                if(bSuccess)
                {
                    ++mCurCachedFilesCount;
                    newInfo.mnLocation = kCacheLocationDisk;
                    newInfo.mnDataSize = requestData.size();
                    if((cacheHeaderInfo.m_ExpiresFound) || (cacheHeaderInfo.m_MaxAgeFound))
                    {
                        newInfo.mnTimeoutSeconds = cacheHeaderInfo.m_MaxAge;
                    }
                    else
                    {
                        EAW_ASSERT_MSG(false,"Trying to cache a file without expire or max age header. Should not get here."); 
                    }
                    newInfo.mnTimeTimeout = newInfo.mnTimeCreated + newInfo.mnTimeoutSeconds;
                    newInfo.mnTimeLastUsed  = GetTimeInSeconds ();
                    newInfo.mnRevalidate = cacheHeaderInfo.m_RevalidateFound;
                }
            }
            pFS->DestroyFileObject(fileInfo.mFileObject);
        }
    }

	EAW_ASSERT_MSG(bSuccess,"Failed to Write File successfully");
    if ( !bSuccess )  // failed to write cache file, so erase the cache entry
        mDataMap.erase( pKey );

    if ( ++mnCacheAccessCountSinceLastMaintenance > kDefaultAccessCountBeforeMaintenance )
        DoPeriodicCacheMaintenance();
}

bool TransportHandlerDiskCache::IniFileCallbackFunction(const char16_t* /*pKey*/, const char16_t* pValue, void* pContext)
{
    // Thread-safety not implemented here, as this is an internal function.
    const uint32_t   kIniFileFieldCount   ( 10 );

    if(*pValue)
    {
        TransportHandlerDiskCache* const pFileCacheHandler = reinterpret_cast<TransportHandlerDiskCache*>(pContext);
        if(!pFileCacheHandler->mbEnabled)
            return false;

        EA::WebKit::FixedString16_128 sTemp(pValue);
        EA::WebKit::FixedString16_128 sFields[ kIniFileFieldCount ];
        eastl_size_t  i, nCurrentFieldIndex ( kIniFileFieldCount - 1 );

        // The ini file value is a comma-delimited set of strings. Note that we intentionally 
        // don't use sscanf to parse this, as it wouldn't always work, due to the kinds of 
        // strings that could be subcomponents.
        for(i = sTemp.length() - 1; ((int)(unsigned)i >= 0) && (nCurrentFieldIndex > 0); i--)
        {
            if(sTemp[i] == ',')
            {
                sFields[nCurrentFieldIndex].assign(sTemp.data() + i + 1, sTemp.length() - (i + 1));  // Assign current field.
                sTemp.erase(i, sTemp.length() - i);                                             // Erase everything from ',' and on.
                nCurrentFieldIndex--;
            }
        }

        EAW_ASSERT(nCurrentFieldIndex == 0); // If the ini file has not been messed with, this should be so.

        if(nCurrentFieldIndex == 0)
        {
            Info cacheInfo;

            sFields[0]                      = sTemp;         // Assign the first field to be the rest of the string.
            cacheInfo.msMIMEContentType.sprintf("%ls", sFields[2].c_str());
            cacheInfo.msCachedFileName      = sFields[1];
            cacheInfo.mnDataSize            = AtoU32(sFields[3].c_str());
            cacheInfo.mnLocation            = kCacheLocationDisk;
            cacheInfo.mnTimeoutSeconds      = AtoU32(sFields[4].c_str());
            cacheInfo.mnTimeCreated         = AtoU32(sFields[5].c_str());
            cacheInfo.mnTimeLastUsed        = AtoU32(sFields[6].c_str());
            cacheInfo.mnTimeTimeout         = AtoU32(sFields[7].c_str());
            cacheInfo.mnChecksum            = AtoU32(sFields[8].c_str());
            cacheInfo.mnRevalidate          = (bool) AtoU32(sFields[9].c_str());

			const uint32_t nTimeNow = (uint32_t)GetTimeInSeconds();

			if(cacheInfo.mnTimeCreated > nTimeNow)  // Just fix the error.
				cacheInfo.mnTimeCreated = (uint32_t)nTimeNow;

			if(cacheInfo.mnTimeLastUsed > nTimeNow) // Just fix the error
				cacheInfo.mnTimeLastUsed = (uint32_t)nTimeNow;

			if(cacheInfo.mnTimeTimeout > nTimeNow)  // If file has not expired...
			{
				EA::IO::Path::PathString16 sFilePath(pFileCacheHandler->msCacheDirectory.c_str());
				EA::IO::Path::Join(sFilePath, cacheInfo.msCachedFileName.c_str());

				EA::WebKit::FileSystem* pFS = EA::WebKit::GetFileSystem();
				EA::WebKit::FixedString16_128 path16(sFilePath.c_str());
				EA::WebKit::FixedString8_128 path8;
				EA::WebKit::ConvertToString8(path16, path8);

				//FileSystem* pFS = GetFileSystem();
				if( (pFS) && (pFS->FileExists(path8.c_str())) )
				{
					EA::WebKit::FixedString8_128 sField8;
					EA::WebKit::ConvertToString8(sFields[0], sField8);
					pFileCacheHandler->mDataMap.insert(DataMap::value_type(sField8, cacheInfo));
					++(pFileCacheHandler->mCurCachedFilesCount);                                            
				}
			}
			else
			{
				pFileCacheHandler->RemoveCachedFile(cacheInfo.msCachedFileName.c_str());
			}
        }
        else
        {
            EAW_ASSERT_MSG(nCurrentFieldIndex != 0, "TransportHandlerDiskCache::IniFileCallbackFunction: Corrupt ini file entry.\n");
        }
    }
    return true;
}


bool TransportHandlerDiskCache::SetDiskCacheParams(const EA::WebKit::DiskCacheInfo& diskCacheInfo)
{
	bool createdDir = SetCacheDirectory(diskCacheInfo.mDiskCacheDirectory);
	if(createdDir)
	{
		SetMaxCacheSize(diskCacheInfo.mDiskCacheSize);
		if(diskCacheInfo.mMaxNumberOfOpenFiles < kOverHeadFileHandlesReqd+1) //+1 to read
		{
			EAW_ASSERT_FORMATTED(false,"Disabling disk cache. Minimum number of file handles required - %u. Provided - %u - Recommended - 24. ",kOverHeadFileHandlesReqd+1,diskCacheInfo.mMaxNumberOfOpenFiles);
			return false;
		}

		mDiskCacheInfo.mMaxNumberOfOpenFiles = diskCacheInfo.mMaxNumberOfOpenFiles;
		EnableDiskCache(true);
	}

	return createdDir && mbEnabled;

}

void TransportHandlerDiskCache::GetDiskCacheUsage(EA::WebKit::DiskCacheUsageInfo& diskCacheUsageInfo)
{
	diskCacheUsageInfo.mCurrentNumberOfCachedFiles = mCurCachedFilesCount;
	diskCacheUsageInfo.mCurrentNumberOfOpenFiles = mCurOpenFilesCount;
}

bool TransportHandlerDiskCache::SetCacheDirectory(const char16_t* pCacheDirectory)
{
    // The supplied directory must end with a trailing path separator.
    // The supplied directory string must be of length <= the maximum
    // designated path length for the given platform; otherwise this 
    // function will fail.
    // If the directory could not be created or accessed, this function
    // will fail.

    // We don't worry about thread safety here, as this function is only to be 
    // called from a single thread upon init.

    bool bReturnValue = false;

    if(pCacheDirectory[0])
    {
        // If we are going to change the cache directory, we may as well not leave
        // any cache files behind in the old cache directory.
        if(!msCacheDirectory.empty() && msCacheDirectory.comparei(pCacheDirectory) == 0)
			ClearCache();

        msCacheDirectory = pCacheDirectory;

        msCacheDirectory.push_back(0);
        if(!EA::IO::Path::EnsureTrailingSeparator(&msCacheDirectory[0], EA::IO::kLengthNull))
            msCacheDirectory.pop_back();

        
        FileSystem* pFS = GetFileSystem();
        if(pFS) 
        {    
            EA::WebKit::FixedString8_128 dir8;    
            EA::WebKit::ConvertToString8(msCacheDirectory, dir8);

            if(pFS->DirectoryExists(dir8.c_str()) )
            {
                bReturnValue = true;
            }
            else
            {
                bReturnValue = pFS->MakeDirectory(dir8.c_str());
                EAW_ASSERT_MSG(bReturnValue, "TransportHandlerDiskCache::SetCacheDirectory(): Unable to create cache directory.");
            }
        }
    }

    return bReturnValue;
}

bool TransportHandlerDiskCache::SetCacheDirectory(const char8_t* pCacheDirectory)
{
    //convenience wrapper.  We will convert the string into a 16 bit string, 
    //then call the above function.
	//Note by Arpit Baldeva: Old code did not do UTF encoding conversions. Replace it with this.
	EA::WebKit::FixedString8_128 cacheDir8(pCacheDirectory);
	EA::WebKit::FixedString16_128 cacheDir16;

	EA::WebKit::ConvertToString16(cacheDir8, cacheDir16);
	return SetCacheDirectory(cacheDir16.c_str());
}

void TransportHandlerDiskCache::GetCacheDirectory(EA::WebKit::FixedString16_128& cacheDirectory)
{
    cacheDirectory = msCacheDirectory;
}

void TransportHandlerDiskCache::GetCacheDirectory(EA::WebKit::FixedString8_128& cacheDirectory)
{
    EA::WebKit::FixedString16_128::size_type pathLen = msCacheDirectory.length();
    cacheDirectory.reserve(pathLen);
    cacheDirectory.resize(pathLen);
    int writeCount = EA::Internal::Strlcpy(&cacheDirectory[0], msCacheDirectory.c_str(), cacheDirectory.max_size(), pathLen); (void)writeCount;
    EAW_ASSERT(writeCount == (int) pathLen);
}

void TransportHandlerDiskCache::SetMaxCacheSize(uint32_t nCacheSize)
{
    uint32_t oldDiskCacheSize = mDiskCacheInfo.mDiskCacheSize;
    
	mDiskCacheInfo.mDiskCacheSize = nCacheSize;
    
	if(oldDiskCacheSize > mDiskCacheInfo.mDiskCacheSize)
        DoPeriodicCacheMaintenance();
}

bool TransportHandlerDiskCache::IsCachedDataValid( const EA::WebKit::FixedString16_128& pURLTxt )
{
    if(!mbEnabled)
        return false;

    EA::WebKit::FixedString8_128 tmp;
    EA::WebKit::ConvertToString8(pURLTxt, tmp);
    return IsCachedDataValid( tmp );
}

bool TransportHandlerDiskCache::IsCachedDataValid( const EA::WebKit::FixedString8_128& pURLTxt )
{
    if(!mbEnabled)
        return false;

    DataMap::iterator itSought(mDataMap.find ( pURLTxt ));
    if (itSought != mDataMap.end () )
    {
        return IsCachedDataValid ( (*itSought).second );
    }

    return false;
}

bool TransportHandlerDiskCache::IsCachedDataValid( const Info& cacheInfo )
{
    if(!mbEnabled)
        return false;
    
    // If  "must-revalidate" was not defined, are we forced to revalidate?
    //if(!cacheInfo.mnRevalidate) 
    //    return true

    if ( (cacheInfo.mnLocation & kCacheLocationPending) == 0 ) {
        return cacheInfo.mnTimeTimeout > (uint32_t) GetTimeInSeconds (); // Question: Why in the world would time ever be negative?
    }

    return false;
}

bool TransportHandlerDiskCache::GetCachedDataInfo(const EA::WebKit::FixedString8_128& pKey, Info& fileCacheInfo) const
{
    // Low level accessor. Gets copy of the data. Gets copy of data. Does *not* do expiration checks,
    // and so may return information for data that has expired. This is by design, as this function's
    // purpose is to allow the interpretation of the cached data as it currently is.
    if(!mbEnabled)
        return false;

    DataMap::const_iterator it = eastl::hashtable_find(mDataMap, pKey.c_str()); 

    if(it != mDataMap.end())
    {
        const Info& infci = (*it).second;
        fileCacheInfo = infci;
        return true;
    }
    return false;
}


void TransportHandlerDiskCache::ClearCache()
{
    mnCacheAccessCount++;

    DataMap::iterator iter;
    for(iter = mDataMap.begin(); iter != mDataMap.end(); ++iter)
    {
        Info& cacheInfo = (*iter).second;
        RemoveCachedFile(cacheInfo.msCachedFileName.c_str());
    }
    mDataMap.clear();
    UpdateCacheIniFile();
}


bool TransportHandlerDiskCache::RemoveCachedData(const EA::WebKit::FixedString8_128& pKey)
{
    // Consider: make an internal version of this that works on Info
    mnCacheAccessCount++;

    // To consider: Make this periodic maintenance based on time rather than access count.
    if ( ++mnCacheAccessCountSinceLastMaintenance > kDefaultAccessCountBeforeMaintenance ) {
        DoPeriodicCacheMaintenance();
    }

    DataMap::iterator it = eastl::hashtable_find(mDataMap, pKey.c_str());

    if(it != mDataMap.end()) // If the key is already in our data map...
    {
        Info& cacheInfo = (*it).second;
        RemoveCachedFile(cacheInfo.msCachedFileName.c_str());   // Remove the file if it is present.
        mDataMap.erase(it);                                     // Just erase it and move on.
        return true;
    }

    return false;
}

bool TransportHandlerDiskCache::GetNewCacheFileName( int nMIMEType, int nMIMESubtype, EA::WebKit::FixedString16_128& sFileName)
{
    if(!mbEnabled)
        return false;

    // Gets a new cached file name to use. The returned name is a file name only 
    // and the directory is the cache directory (See GetCacheDirectory/SetCacheDirectory).
    // GetNewCacheFilePath returns the same value but with the directory prepended.
    // 
    // If the input pKey looks like a file name (defined by having a dot in it), 
    // this function attempts to retain that extension in the returned file name. 

    // No thread safety checks for this method. It doesn't touch class data.

	char16_t pExtension[EA::WebKit::FileSystem::kMaxPathLength] = { '\0' };

	MIMETypesToFileExtension ( MIMEType(nMIMEType), MIMEType(nMIMESubtype), pExtension, EA::WebKit::FileSystem::kMaxPathLength );

    EA::Internal::Strcat ( pExtension, kCachedFileExtension );

	char16_t pFilePath[EA::WebKit::FileSystem::kMaxPathLength];

    const bool bResult = MakeTempPathName(pFilePath, msCacheDirectory.c_str(), NULL, pExtension);

    if ( bResult ) {
        // we know that the full path begins with the directory, so just copy the string after that
        sFileName.assign ( &pFilePath [ msCacheDirectory.size () ] );
    }

    return bResult;
}

void TransportHandlerDiskCache::ClearCacheMap()
{
    mDataMap.clear();
}



// UpdateCacheIniFile
//
// This function is normally called upon shutting down an instance of this class.
//   CA: started calling this from DoPeriodicCacheMaintenance to avoid ini file being out of
//   date on non-clean exit
//
bool TransportHandlerDiskCache::UpdateCacheIniFile()
{
    // Writes our cache information to the ini file used to store it.
    bool returnFlag = false;

    if(!mbEnabled)
        return returnFlag;

    // Access file
    FileSystem* pFS = GetFileSystem();
    if(!pFS)
        return returnFlag;

  
    // Build Path with file name    
    EA::WebKit::FixedString16_256 path16(msCacheDirectory.c_str()); 
    EA::WebKit::FixedString16_256 name16(msIniFileName.c_str()); 
    path16.append(name16);
    EA::WebKit::FixedString8_256 path8;    
    EA::WebKit::ConvertToString8(path16, path8);
        
    // Remove old ini file    
    pFS->RemoveFile(path8.c_str());
    
    // Create new one
    EA::WebKit::FileSystem::FileObject fileObject = pFS->CreateFileObject();
    if(fileObject != FileSystem::kFileObjectInvalid)
    {
        if(pFS->OpenFile(fileObject, path8.c_str(), FileSystem::kWrite, FileSystem::kCDOCreateAlways))
        {    
            
            // Add in title header
            EA::WebKit::FixedString8_64 title;
            title.sprintf("%s\n",kDefaultIniFileSection);
            uint32_t checksum = 0;
            checksum = GetByteChecksum(title.c_str(), title.length(), checksum);
            pFS->WriteFile(fileObject,title.c_str(),title.length());                


            // Add in each cache entry.
            // (Note: wondering if we should a hash key of the string instead of the full query string to save space)
            for(DataMap::iterator it = mDataMap.begin(); it != mDataMap.end(); ++it) // For each hash map entry, add an ini file section for it.
            {
                const EA::WebKit::FixedString8_128& sKey = (*it).first;
                Info& cacheInfo = (*it).second;

                if( ( cacheInfo.mnLocation & kCacheLocationDisk ) != 0 ) // only write about things on disk
                {
                           
                    EA::WebKit::FixedString8_256 line;
                    EA::WebKit::FixedString16_256 conv16(cacheInfo.msCachedFileName.c_str());
                    EA::WebKit::FixedString8_256 conv8;
                    EA::WebKit::ConvertToString8(conv16, conv8);
                    line.sprintf("%s%s,%s,%s,%u,%u,%u,%u,%u,%u,%u\n",
                        kCacheLineEntry,sKey.c_str(),
                        conv8.c_str(),
                        cacheInfo.msMIMEContentType.c_str(),    
                        cacheInfo.mnDataSize,
                        cacheInfo.mnTimeoutSeconds,
                        cacheInfo.mnTimeCreated,
                        cacheInfo.mnTimeLastUsed,
                        cacheInfo.mnTimeTimeout,
                        cacheInfo.mnChecksum,
                        cacheInfo.mnRevalidate);
                
                    checksum = GetByteChecksum(line.c_str(), line.length(), checksum);
                    pFS->WriteFile(fileObject,line.c_str(),line.length());                   
                }
            }

            // File Checksum
            EA::WebKit::FixedString8_64 fileChecksum;
            fileChecksum.sprintf("%s%u",kCacheChecksum,checksum);
            pFS->WriteFile(fileObject,fileChecksum.c_str(),fileChecksum.length());                   
            pFS->CloseFile(fileObject);
            returnFlag = true;      // Consider as success 
        }
        pFS->DestroyFileObject(fileObject);        
    }
    return returnFlag;
}

bool TransportHandlerDiskCache::ReadCacheIniFile()
{
    if(!mbEnabled)
        return false;

    // Build name path 
    EA::WebKit::FixedString16_256 path16(msCacheDirectory.c_str()); 
    path16.append(msIniFileName.c_str(),msIniFileName.size()); 
    EA::WebKit::FixedString8_256 path8;    
    EA::WebKit::ConvertToString8(path16, path8);

    FileSystem* pFS = GetFileSystem();
    if( (pFS) && (pFS->FileExists(path8.c_str())) )
    {
          EA::WebKit::FileSystem::FileObject fileObject = pFS->CreateFileObject();
          if( (fileObject != FileSystem::kFileObjectInvalid) &&
              (pFS->OpenFile(fileObject, path8.c_str(), EA::WebKit::FileSystem::kRead, EA::WebKit::FileSystem::kCDOOpenExisting)) )
          {
            int64_t numBytes = 0;
            pFS->GetFileSize(path8.c_str(),numBytes);
            if(numBytes>0) //Have something to read
            {
                char* pFileBuffer = new char8_t[numBytes+1];
                EAW_ASSERT(pFileBuffer);
                
                const int64_t fileSize = pFS->ReadFile(fileObject, pFileBuffer, numBytes);
                pFS->CloseFile(fileObject);

				if(fileSize < numBytes+1 && fileSize != EA::WebKit::FileSystem::kSizeTypeError)              
                {
                    pFileBuffer[fileSize] = '\0';
                    EA::WebKit::FixedString8_256 file8(pFileBuffer); 
                    
                    // Check if we have an ok header    
                    if(file8.find(kDefaultIniFileSection) != EA::WebKit::FixedString8_128::npos) 
                    {
						// Checksum verify
                        FixedString8_32::size_type checkSumPos=0;
                        checkSumPos = file8.find(kCacheChecksum);    
                        FixedString8_32::size_type sizeChecksumStr = strlen(kCacheChecksum);
                        FixedString8_32::size_type checkDelta = numBytes - checkSumPos - sizeChecksumStr;
                        int32_t result =0;
                        uint32_t checksum =0;
                        uint32_t savedChecksum =0;
                        if(checkDelta > 0)
                        {
                            FixedString8_256 check(file8.substr(checkSumPos + sizeChecksumStr, checkDelta));   
                            result = sscanf( check.c_str(), "%u", &savedChecksum);                                    
                            checksum = GetByteChecksum(file8.c_str(),checkSumPos,0);
                        }
						if((result == 1) && (checksum == savedChecksum)) 
                        {
                            bool exitFlag = false;                           
                            FixedString8_32::size_type start=0;                        
                            FixedString8_32::size_type end=0;
                            FixedString8_32::size_type pos=0;
                            FixedString8_32::size_type lineEntrySize = strlen(kCacheLineEntry);
                            start = file8.find(kCacheLineEntry);    

                            while((start != EA::WebKit::FixedString8_128::npos) && (!exitFlag))
                            {
                               // Isolate the line 
                               pos = start + lineEntrySize;                          
                               end   = file8.find(kCacheLineEntry, pos);     
                               start = end; 
                               if(end == EA::WebKit::FixedString8_128::npos)
                               { 
                                   // Set end to start of checksum
                                    end   = checkSumPos;                                
                                    exitFlag = true;
                               }

                               // Isolate the line 
                               FixedString8_32::size_type delta = end - pos;
                               if(delta > 0)
                               { 
                                   FixedString8_256 line8(file8.substr(pos,delta));                            
                                   
                                   // Now parse the line 
                                   FixedString16_256 line16;     
                                   EA::WebKit::ConvertToString16(line8, line16);
                                   IniFileCallbackFunction(0, line16.c_str(), this);                             
                               }
                               else
                               {
                                exitFlag = true;
                               } 
                            }
                        }
                    }
                }
                delete[] pFileBuffer; 
            }
		}
		pFS->DestroyFileObject(fileObject);
    }
    return true;
}


bool TransportHandlerDiskCache::RemoveCachedFile(const char16_t* pFileName)
{
    // This is a function which accepts a file name (name only, not including directory)
    // and returns the size of it. The argument takes a EA::WebKit::FixedString16_128& instead of char16_t* because
    // the only way it will ever be called is via a EA::WebKit::FixedString16_128.
    // Thread-safety not implemented here. Sometimes it's called from thread safe code and sometimes not

    if(pFileName && *pFileName)
    {
        EA::IO::Path::PathString16 sFilePath(msCacheDirectory.c_str());
        EA::IO::Path::Join(sFilePath, pFileName);
                    
        EA::WebKit::FixedString16_128 path16(sFilePath.c_str()); 
        EA::WebKit::FixedString8_128 path8;    
        EA::WebKit::ConvertToString8(path16, path8);

        FileSystem* pFS = GetFileSystem();
        if(pFS)
        {
            --mCurCachedFilesCount;
            return (pFS->RemoveFile(path8.c_str()) );
        }
    }

    return false;
}

bool TransportHandlerDiskCache::RemoveUnusedCachedFiles()
{
    // Thread-safety not implemented here, as this is only called from init.
    EA::IO::DirectoryIterator            directoryIterator;
    EA::IO::DirectoryIterator::EntryList entryList;

    if(directoryIterator.Read((msCacheDirectory.c_str()), entryList, (kSearchCachedFileExtension), EA::IO::kDirectoryEntryFile))
    {
        for(EA::IO::DirectoryIterator::EntryList::iterator it = entryList.begin(); it != entryList.end(); ++it)
        {
            const EA::IO::DirectoryIterator::Entry& entry = *it;
            DataMap::iterator itMap = mDataMap.begin();

            for(; itMap != mDataMap.end(); ++itMap) // For each hash map entry, add an ini file section for it.
            {
                const Info& cacheInfo = (*itMap).second;

                if(EA::Internal::Stricmp(cacheInfo.msCachedFileName.c_str(), (entry.msName.c_str())) == 0) // If the disk file is also in our cache map...
                    break;
            }

            if(itMap == mDataMap.end()) // If the file was not in our list...
                RemoveCachedFile((entry.msName.c_str()));
        }
    }
    return true;
}

TransportHandlerDiskCache::DataMap::iterator TransportHandlerDiskCache::FindLRUItem()
{
//   find oldest item in location
    DataMap::iterator itCur(mDataMap.begin()), itEnd(mDataMap.end());
    DataMap::iterator itSought ( itEnd );
    uint32_t nOldestTime ( UINT32_MAX ); // Note: pending resources have last access == UINT32_MAX so they should never be chosen
    for(; itCur!=itEnd; ++itCur) 
    {
    	Info& cacheInfo ( (*itCur).second );
        if ( cacheInfo.mnTimeLastUsed < nOldestTime ) 
        {
            itSought = itCur;
            nOldestTime = cacheInfo.mnTimeLastUsed;
        }
    }

    return itSought;
}

void TransportHandlerDiskCache::DoPeriodicCacheMaintenance()
{
    if(!mbEnabled)
        return;

    uint32_t fileCount =0;
    EA::IO::size_type nFileCacheMemoryUsage = 0;
      
    bool    bDiskFilesChanged = false; // should we update the ini file?

    mnCacheAccessCountSinceLastMaintenance = 0;

	// Need to check expiration dates of data map entries.
	const uint32_t nTimeNow = (uint32_t)GetTimeInSeconds();

	for(DataMap::iterator it = mDataMap.begin(); it != mDataMap.end(); ) // For each hash map entry, add an ini file section for it.
	{
		const Info& cacheInfo = (*it).second;

		if ( (nTimeNow >= cacheInfo.mnTimeTimeout) 
			&& ( ( cacheInfo.mnLocation & kCacheLocationPending ) == 0 ) ) 
			// If the cached item has expired...
			// && don't delete files that have not been committed
		{
			if ( cacheInfo.mnLocation & kCacheLocationDisk ) {
				RemoveCachedFile(cacheInfo.msCachedFileName.c_str());   // Delete File cached data if present.
				bDiskFilesChanged = true;
			}
			it = mDataMap.erase(it);
		}
		else
		{
			if ( cacheInfo.mnLocation & kCacheLocationDisk )
			{
				++fileCount;
				nFileCacheMemoryUsage += cacheInfo.mnDataSize;
			}
			++it;
		}
	}

    // OK, now we've purged any old files and know how much File 
    // space is being taken up by the cached data. We should do some purges
    // of the first-expiring data if we are using up too much memory.

    while((nFileCacheMemoryUsage > mDiskCacheInfo.mDiskCacheSize))
    {
        DataMap::iterator itLRU ( FindLRUItem() );
        if ( itLRU != mDataMap.end () ) 
        {
            Info& cacheInfo ( (*itLRU).second );
            RemoveCachedFile ( cacheInfo.msCachedFileName.c_str() );
            bDiskFilesChanged = true;
            nFileCacheMemoryUsage -= cacheInfo.mnDataSize;
            fileCount--;
            mDataMap.erase ( itLRU );
        }
        else 
        {
            break; // shouldn't happen
        }
    }

    mCurCachedFilesCount = fileCount;

    if ( bDiskFilesChanged )
    	UpdateCacheIniFile();
}

} //namespace WebKit
} //namespace EA
