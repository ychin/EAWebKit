/*
Copyright (C) 2009, 2010, 2011, 2012, 2013 Electronic Arts, Inc.  All rights reserved.

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
//
// Created by Nicki Vankoughnett
//
// This is strictly a utility class that is a cache of files obtained from 
// the Internet, usually obtained via HTTP and HTTPS.
///////////////////////////////////////////////////////////////////////////////


#ifndef TransportHandlerDiskCacheEA_H
#define TransportHandlerDiskCacheEA_H


#include <EAWebKit/EAWebKit.h>
#include <EAWebKit/EAWebKitTransport.h>
#include <internal/include/EAWebKitEASTLHelpers.h>
#include <EASTL/core_allocator_adapter.h>
#include <EASTL/string.h>
#include <EASTL/hash_map.h>
#include <EAIO/EAStream.h>
#include <EAIO/EAStreamMemory.h>


namespace WebCore
{
    class SharedBuffer;
}


namespace EA
{
    namespace WebKit
    {
        /// CacheResponseHeaderInfo
        ///
        /// Extracts useful info from the header directives  The TransportHandlerDiskCache class will
        /// use that info to decide if it ought to load from the cache, or from network.
        ///
        class CacheResponseHeaderInfo
        {
        public:
            CacheResponseHeaderInfo();
            
            bool SetDirectivesFromHeader(const TransportInfo* pTInfo);
            bool CanCacheToDisk() const {return m_CanCacheToDisk;}


        private:
			bool          m_CanCacheToDisk;

        public:    
            bool          m_NoStoreFound;       // Store nothing
            bool          m_PrivateFound;       // Private and no-cache (http://www.w3.org/Protocols/HTTP/Issues/cache-private.html)
            bool          m_PublicFound;        // If the public directive was found.
            bool          m_MaxAgeFound;        // If the max age directive was found.
            uint32_t      m_MaxAge;             // The origin server specifies a freshness lifetime for the entity, overriding lifetime values determined by the proxy caches.
            bool          m_ExpiresFound;       // Found the Expires tag.  Note: it will not search of it if max-age was already found as this will override the expires directive.
            bool          m_RevalidateFound;    // Found must revalidate    
            bool          m_NoTransformFound;   // Found the no transform tag.  This can be used by the image compression.    
        private:
            void Reset();
            bool ExtractCacheControlDirectives(const FixedString16_128& directive);
            bool ExtractExpiresDirective(const FixedString16_128& directive);
        };


		
        /// TransportHandlerDiskCache 
        ///
        /// Caches files that are obtained via http or https. 
        ///
        class TransportHandlerDiskCache : public TransportHandler
        {
        public:
            TransportHandlerDiskCache();
           ~TransportHandlerDiskCache();

            bool Init       (const char16_t* pScheme);
            bool Shutdown   (const char16_t* pScheme);
            bool InitJob    (TransportInfo* pTInfo, bool& bStateComplete);
            bool ShutdownJob(TransportInfo* pTInfo, bool& bStateComplete);
            bool Connect    (TransportInfo* pTInfo, bool& bStateComplete);
            bool Disconnect (TransportInfo* pTInfo, bool& bStateComplete);
            bool Transfer   (TransportInfo* pTInfo, bool& bStateComplete);

			bool SetDiskCacheParams(const EA::WebKit::DiskCacheInfo& diskCacheInfo);
			void GetDiskCacheUsage(EA::WebKit::DiskCacheUsageInfo& diskCacheInfo);

            void InvalidateCachedDataIfRequired(const TransportInfo* pTInfo);
            void CacheToDisk(const FixedString16_128& uriFNameStr, const FixedString8_128& mimeStr, const WebCore::SharedBuffer& requestData, const CacheResponseHeaderInfo& cacheHeaderInfo);

            //Other functionality
            bool CacheEnabled() { return mbEnabled; }
            bool EnableDiskCache(bool enabled);

            void ClearCache(); //clears the disk cache on demand.

            bool  SetCacheDirectory(const char16_t* pCacheDirectory);
            bool  SetCacheDirectory(const char8_t* pCacheDirectory);

            void  GetCacheDirectory(EA::WebKit::FixedString16_128& cacheDirectory);
            void  GetCacheDirectory(EA::WebKit::FixedString8_128& cacheDirectory);

            void  SetMaxFileCount(const uint32_t maxFileCount);
            void  SetMaxJobCount(const uint32_t count);
            void  SetMaxCacheSize(uint32_t nCacheSize);
            void  SetMinFileSize(const uint32_t size);
			bool  HandleAvailableToReadFile() const;

            bool IsCachedDataValid( const FixedString16_128& url );
            bool IsCachedDataValid( const FixedString8_128& url );

        private:
            enum CacheLocation
            {
                kCacheLocationNone    = 0x0,
                kCacheLocationDisk    = 0x2,
                kCacheLocationPending = 0x4  // an entry for this key has been created, but not committed to the cache
            };

            struct Info
            {
                FixedString8_128    msMIMEContentType;      /// MIME type (if known) of cached resource
                FixedString16_128   msCachedFileName;       /// File Name Only. Must append to cache directory to get full path.
                EA::IO::size_type	mnDataSize;             /// Cached file/data size.
                uint32_t			mnLocation;             /// BitField of locations where this is stored. Also indicates validity of cached data (kCacheLocationPending)
                uint32_t            mnTimeoutSeconds;       /// Number of relative seconds before the cache info should timeout. This is used to determine mnTimeTimeout.
                uint32_t            mnTimeCreated;          /// The time this cache info was created.
                uint32_t            mnTimeLastUsed;         /// The time this cache info was last created, updated, or retrieved.
                uint32_t            mnTimeTimeout;          /// The time of the timeout.
                uint32_t            mnChecksum;             /// File checksum to detect corruption.
                bool                mnRevalidate;           /// File must revalidate time before using.
            };

            struct FileInfo
            {
                FileSystem::FileObject mFileObject;
                int64_t                mFileSize;
                uint32_t               mCurChecksum; 
                
                FileInfo() 
                    : mFileObject(FileSystem::kFileObjectInvalid)
                    , mFileSize(-1)
                    , mCurChecksum(0)
                {
                }

            };

            /// cache map data type
            typedef eastl::hash_map<FixedString8_128, Info, eastl::string_hash<FixedString8_128>, eastl::equal_to<FixedString8_128>, EASTLAllocator> DataMap;

            /// This is a callback function which is called by the ini file reader in ReadCacheIniFile.
            static bool IniFileCallbackFunction(const char16_t* pKey, const char16_t* pValue, void* pContext);

            bool IsCachedDataValid( const Info& cacheInfo );
            bool GetCachedDataInfo(const FixedString8_128& pKey, Info& fileCacheInfo) const;

            bool RemoveCachedData(const FixedString8_128& pKey);
            void ClearCacheMap();
            bool UpdateCacheIniFile();
            bool ReadCacheIniFile();

            bool GetNewCacheFileName( int nMIMEType, int nMIMESubtype, FixedString16_128& sFileName);
            bool RemoveCachedFile(const char16_t* pFileName);
            bool RemoveUnusedCachedFiles();
            void DoPeriodicCacheMaintenance();
            DataMap::iterator FindLRUItem (); // oldest item in location
			char* GetCacheDownloadBuffer();                                     /// Get and allocate a shared download buffer if needed.
			void RemoveCacheDownloadBuffer();                                   /// Remove the allocated download buffer
            
            TransportHandlerDiskCache(const TransportHandlerDiskCache&);
            const TransportHandlerDiskCache& operator=(const TransportHandlerDiskCache&);

            bool                        mbInitialized;                          /// Have we been initialized or not?
            bool                        mbEnabled;                              /// Enables or disables the file cache
            FixedString16_128           msCacheDirectory;                       /// Path to cache directory on disk.
            FixedString16_128           msIniFileName;                          /// File name alone of cache ini file, which stores info about cached data on disk.
            DataMap                     mDataMap;                               /// Map of cached data key (string) to cached data.
            uint32_t                    mnCacheAccessCount;                     /// Count of number of times the cache was accessed.
            uint32_t                    mnCacheAccessCountSinceLastMaintenance; /// Count of number of times the cache was accessed since the last time we examined the cache for expirations.

		   EA::WebKit::DiskCacheInfo	mDiskCacheInfo;							/// User preferences. Name of the cache directory is accessed via msCacheDirectory and not this member. 
           int32_t						mCurCachedFilesCount;                   /// Number of cached files (not counting the ini file)
		   int32_t						mCurOpenFilesCount;						/// Used to verify jobs are all shut down and not leaked.
           char*						mpCacheDownloadBuffer;                  /// Shared buffer for downloading blocking reads 
 };

    } // namespace WebKit

} // namespace EA



#endif // TransportHandlerDiskCacheEA_H
