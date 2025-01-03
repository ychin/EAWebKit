/*
Copyright (C) 2008, 2009, 2010, 2011, 2012, 2013, 2014, 2015 Electronic Arts, Inc.  All rights reserved.

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
// DirtySDK TransportHandler Implementation
// Created by Paul Pedriana 
// Maintained by EAWebKit Team
/////////////////////////////////////////////////////////////////////////////


#include "config.h"

#if ENABLE(DIRTYSDK_IN_DLL)
#include <EAWebKit/EAWebKitConfig.h>
#include "TransportHandlerDirtySDKEA.h"
#include <EAWebKit/EAWebKitView.h>
#include <EAWebKit/EAWebKitClient.h>
#include <EAWebKit/EAWebkitSTLWrapper.h>

#include <internal/include/EAWebKit_p.h>
#include <EAAssert/eaassert.h>
#include <EAIO/FnEncode.h>          // For Strlcpy and friends.
#include <stdio.h>
#include <DirtySDK/proto/protossl.h>

#if !BUILDING_EAWEBKIT_DLL
#include "netconn.h"
#endif

#if (BUILDING_EAWEBKIT_DLL)
#if defined(EA_PLATFORM_MICROSOFT)
#include "winsock2.h"
#endif
#include <DirtySDK/platform.h>
#include <DirtySDK/dirtysock/dirtynet.h>
#endif

#include "HTTPHeaderNames.h"
#include <wtf/text/StringView.h>
#include "StreamDecompressorEA.h"

#define MULTICHAR_CONST(a,b,c,d) (a << 24 | b << 16 | c << 8 | d) 

#define DIRTY_OPEN						MULTICHAR_CONST('o','p','e','n')
#define DIRTY_ESSL						MULTICHAR_CONST('e','s','s','l')
#define DIRTY_PIPE						MULTICHAR_CONST('p','i','p','e')
#define DIRTY_PIPE_WITHOUT_KEEP_ALIVE	MULTICHAR_CONST('p','w','k','a')
#define DIRTY_MAX_REDIRECTION			MULTICHAR_CONST('r','m','a','x')
#define DIRTY_REQUEST_TIMEOUT			MULTICHAR_CONST('t','i','m','e')
#define DIRTY_LOG_LEVEL					MULTICHAR_CONST('s','p','a','m')
#define DIRTY_CALLBACK_USER_POINTER		MULTICHAR_CONST('c','b','u','p')
#define DIRTY_DISABLE_CERT_VALIDATION	MULTICHAR_CONST('n','c','r','t')
#define DIRTY_RESPONSE_CODE				MULTICHAR_CONST('c','o','d','e')
#define DIRTY_CIPHER_SUITE				MULTICHAR_CONST('c','i','p','h')
#define DIRTY_CERT      				MULTICHAR_CONST('c','e','r','t')
#define DIRTY_AUTO      				MULTICHAR_CONST('a','u','t','o')
#define DIRTY_STAT						MULTICHAR_CONST('s','t','a','t')
#define DIRTY_SOCKERROR					MULTICHAR_CONST('s','e','r','r')


#ifdef _MSC_VER
#pragma warning(disable: 4365) // conversion from 'int' to 'uint32_t', signed/unsigned mismatch
#pragma warning(disable: 4242) // conversion from 'int' to 'char8_t', possible loss of data
#pragma warning(disable: 4244) // conversion from 'int' to 'char16_t', possible loss of data
#pragma warning(disable: 4127) // conditional expression is constant
#endif

namespace EA
{
namespace TransportHelper
{
int Stricmp(const char16_t* pString1, const char16_t* pString2)
{
	char16_t c1, c2;

	// PowerPC has lhzu and sthu instructions which make pre-increment 
	// updates faster for the PowerPC's load and store architecture.
#ifdef EA_PROCESSOR_POWERPC
	--pString1;
	--pString2;
	while((c1 = tolower(*++pString1)) == (c2 = tolower(*++pString2)))
	{
		if(c1 == 0)
			return 0;
	}
#else
	while((c1 = tolower(*pString1++)) == (c2 = tolower(*pString2++)))
	{
		if(c1 == 0)
			return 0;
	}
#endif

	return (c1 - c2);
}

size_t Strlen(const char8_t* pString)
{
	size_t nLength = (size_t)-1; 

	// PowerPC has lhzu and sthu instructions which make pre-increment 
	// updates faster for the PowerPC's load and store architecture.
#ifdef EA_PROCESSOR_POWERPC
	const char8_t* p = pString - 1;
	do{
		++nLength;
	}while(*++p);
#else
	do{
		++nLength;
	}while(*pString++);

#endif

	return (size_t)nLength;
}

size_t Strlen(const char16_t* pString)
{
	size_t nLength = (size_t)-1;

	// PowerPC has lhzu and sthu instructions which make pre-increment 
	// updates faster for the PowerPC's load and store architecture.
#ifdef EA_PROCESSOR_POWERPC
	const char16_t* p = pString - 1;
	do{
		++nLength;
	}while(*++p);
#else
	do{
		++nLength;
	}while(*pString++);

#endif

	return nLength;
}

// Commented out since unused.
/*
static void ConvertToString8(const EA::TransportHelper::TransportString16& s16, EA::TransportHelper::TransportString8& s8)
{
	// A 16 bit string of strlen n16 will convert to an 8 bit UTF8 string with a strlen >= n16.
	eastl_size_t size16 = s16.size();
	eastl_size_t size8  = size16 + 8;   // Give it some extra space to detect that Strlcpy needed more space..

	// Most of the time we are copying ascii text and the dest length == source length.
	s8.resize(size8);
	size_t destLen = EA::IO::StrlcpyUTF16ToUTF8(&s8[0], size8 + 1, s16.c_str(), size16); // +1 because there is a 0 char at the end that is included in the string capacity.

	if(destLen > size16) // If there were multibyte expansions, and thus that we need to use a dest size > source size...
	{
		destLen = EA::IO::StrlcpyUTF16ToUTF8(NULL, 0, s16.c_str(), size16); // Call with NULL in order to get the required strlen.
		s8.resize((eastl_size_t)destLen);
		EA::IO::StrlcpyUTF16ToUTF8(&s8[0], destLen + 1, s16.c_str(), size16);
	}
	else
		s8.resize((eastl_size_t)destLen);
}
*/		
static void ConvertToString16(const EA::TransportHelper::TransportString8& s8, EA::TransportHelper::TransportString16& s16)
{
	// An 8 bit UTF8 string with a strlen of n8 will convert to a 16 bit string with a strlen <= n8.
	eastl_size_t size8 = s8.size();

	// Most of the time we are copying ascii text and the dest length == source length.
	s16.resize(size8);
	const size_t destLen = EA::IO::StrlcpyUTF8ToUTF16(&s16[0], size8 + 1, s8.c_str(), size8); // +1 because there is a 0 char at the end that is included in the string capacity.
	s16.resize((eastl_size_t)destLen);
}

static void WriteHeaderMapEntry(EA::TransportHelper::TransportHeaderMap::key_type& sKey, EA::TransportHelper::TransportHeaderMap::mapped_type& sValue, EA::TransportHelper::TransportHeaderMap& headerMap)
{
	while(!sKey.empty() && ((sKey.back() == ' ') || (sKey.back() == '\t')))
		sKey.pop_back(); // Remove trailing whitespace.

	while(!sValue.empty() && ((sValue.back() == ' ') || (sValue.back() == '\t')))
		sValue.pop_back(); // Remove trailing whitespace.

	if(!sKey.empty() && !sValue.empty())
	{
		const EA::TransportHelper::TransportHeaderMap::value_type entry(sKey, sValue);
		headerMap.insert(entry);
	}
}

static void CopyHeaderLine(const char16_t* pKey, const EA::TransportHelper::TransportHeaderMap& headerMap, char*& pHeader, uint32_t& uHeaderCapacity)
{
	// Write the Host field.
	EA::TransportHelper::TransportHeaderMap::const_iterator it = headerMap.find_as(pKey, EA::TransportHelper::str_iless());

	if(it != headerMap.end())
	{
		const EA::TransportHelper::TransportHeaderMap::mapped_type& sKey   = it->first;
		const EA::TransportHelper::TransportHeaderMap::mapped_type& sValue = it->second;

		const uint32_t lineSize = (uint32_t)(sKey.length() + 2 + sValue.length() + 2 + 1); //1 for the terminating 0 char

		if(uHeaderCapacity > lineSize) // Use > instead of >= because we want to write a terminating 0 char.
		{
			for(eastl_size_t i = 0, iEnd = sKey.length(); i != iEnd; ++i)
				*pHeader++ = (char)sKey[i];

			*pHeader++ = ':';
			*pHeader++ = ' ';

			for(eastl_size_t i = 0, iEnd = sValue.length(); i != iEnd; ++i)
				*pHeader++ = (char)sValue[i];

			*pHeader++ = '\r';
			*pHeader++ = '\n';
			*pHeader   = 0;

			uHeaderCapacity -= lineSize; 
		}
	}
}
// We read multi-line text like the following:
//    aaaa : bbbb\n
//    aaaa:    bbbb\n
//    aaaa:bb\n
//     bb\n
//    aaaa : bbbb\n

static bool SetHeaderMapWrapperFromText(const char* pHeaderMapText, uint32_t textSize, EA::TransportHelper::TransportHeaderMap& headerMap, bool bExpectFirstCommandLine, bool bClearMap)
{
	bool processedWithError = false;

	enum Mode
	{
		kModeKey,               // We are in the process of reading the key
		kModeKeyValueSeparator, // We are in the process of reading the characters (e.g. " : ") between the key and the value.
		kModeValue,             // We are in the process of reading the value (which may span multiple lines).
		kModeNewLine            // We are in the process of reading the newline after a value.
	};

	Mode               mode = kModeKey;
	const char*        p    = pHeaderMapText;
	const char*        pEnd = pHeaderMapText + textSize;
	EA::TransportHelper::TransportHeaderMap::key_type		sKey;
	EA::TransportHelper::TransportHeaderMap::mapped_type	sValue;
	
	//Update 12/20/2010: Disabled this check because we use strings that allow overflow. So actually limiting the key/value can cause
	// bugs silently.

	//const eastl_size_t kMaxKeySize = 128;
	//const eastl_size_t kMaxValueSize = 2048;
	bool               inErrorCondition = false;
	bool               colonFound = false;

	if(bClearMap)
		headerMap.clear();

	if(bExpectFirstCommandLine)
	{
		// The first line should always be the status line, without a key:value pair. So skip it.
		while(*p && (*p != '\r') && (*p != '\n') && (p < pEnd))
			++p;

		if(*p == '\r')
			++p;

		if(*p == '\n')
			++p;
	}

	if((*p == ' ') || (*p == '\t') || (*p == '\r') || (*p == '\n'))
	{
		processedWithError = true;
		return processedWithError;
	}

	while(*p && (p < pEnd))
	{
		char c = *p++;

		switch(mode)
		{
		case kModeKey:
			if((c == '\r') || (c == '\n'))  // If we have reached a newline before reading whitespace or ':' char, the header is malformed.
			{
				processedWithError = inErrorCondition = true;
				mode = kModeNewLine;
			}
			else if((c == ' ') || (c == '\t') || (c == ':'))
			{
				mode = kModeKeyValueSeparator;
				if(c == ':')
					colonFound = true;
			}
			else
			{
				//if(sKey.length() < kMaxKeySize)
					sKey += (char16_t)(c);
			}
			break;

		case kModeKeyValueSeparator:
			if((c == '\r') ||(c == '\n')) // If we have reached a newline before any chars of the value, the value is empty. 
			{
				inErrorCondition = !colonFound; // We are OK only if a colon was found between the key and the value.
				mode = kModeNewLine;
			}
			else if((c != ' ') && (c != '\t') && (c != ':')) // If it is part of the value...
			{
				sValue = (uint8_t)c;
				mode   = kModeValue;
			}
			else if(c == ':')
				colonFound = true;

			break;

		case kModeValue:
			if((c == '\r') || (c == '\n')) // If we reached the end of the line...
				mode = kModeNewLine;
			else
			{
				//if(sValue.length() < kMaxValueSize)
					sValue += (uint8_t)c;
			}
			break;

		case kModeNewLine:
			{
				if((c != '\r') && (c != '\n')) // If we have reached the next line...
				{
					if((c == ' ') || (c == '\t'))  // If we have a value that is continued from the previous line...
					{
						mode = kModeValue; // Go back to appending to the value.

						// Eat leading whitespace.
						while((p < pEnd) && ((c = *p) == ' ') && (c == '\t'))
							p++;
					}
					else
					{
						// We beginning the new key/value pair. Finalize the previous one.
						if(!inErrorCondition) //Make sure that you are not in an error condition when writing this Key-Value pair
						{
							//if((sKey.length() < kMaxKeySize) && (sValue.length() < kMaxValueSize)) // If no overflow occurred...
								EA::TransportHelper::WriteHeaderMapEntry(sKey, sValue, headerMap);
						}
						else
							processedWithError = true; //Also, indicate to the caller that you did encounter some error in processing.

						inErrorCondition = false;  // Reset the error condition for next line.
						mode = kModeKey;           // This is part of a new key.
						colonFound = false;
						sKey = (uint8_t)c;
						sValue.clear();
					}
				}

				break;
			}
		}
	}

	// Finalize the last entry.
	if(!inErrorCondition /*&& (sKey.length() < kMaxKeySize) && (sValue.length() < kMaxValueSize)*/) // If no overflow occurred...
		EA::TransportHelper::WriteHeaderMapEntry(sKey, sValue, headerMap);

	return processedWithError;
}


}
}

namespace EA
{
namespace WebKit
{
#if ENABLE_PAYLOAD_DECOMPRESSION	
	static void DecompressedDataCallbackFunc(void* userData, uint8_t* decompressedData, uint32_t decompressedDataLength)
	{
		EA::WebKit::TransportInfo* pTInfo = reinterpret_cast<EA::WebKit::TransportInfo*>(userData);
		pTInfo->mpTransportServer->DataReceived(pTInfo, decompressedData, decompressedDataLength);
	}
#endif

	TransportHandlerDirtySDK::DirtySDKInfo::DirtySDKInfo():
		mHttpHandle(0), 
		mURI(), 
		mbHeadersReceived(false), 
		mSendIndex(0), 
		mPostData(), 
		mbPostActive(false), 
		mPostBufferSize(0), 
		mPostBufferPosition(0), 
		mStreamDecompressor(0) 
		{ 

		}

	TransportHandlerDirtySDK::DirtySDKInfo::~DirtySDKInfo()
	{
#if ENABLE_PAYLOAD_DECOMPRESSION
		if(mStreamDecompressor)
		{
			mStreamDecompressor->~IStreamDecompressor();
			EA::WebKit::Allocator* pAllocator = GetAllocator();
			pAllocator->Free(mStreamDecompressor,0);
			mStreamDecompressor = 0;
		}
#endif
	}
	
	TransportHandlerDirtySDK::TransportHandlerDirtySDK()
	: mpHttpManager(NULL)
	, mpHttpManagerSyncJobs(NULL)
	, mSharedDecompressionBuffer(NULL)
	, mActiveSchemesCount(0)
#ifdef _DEBUG
   	, mJobCount(0)
#endif
	, mIsDirtySockStartedHere(false)
	{

	}


	TransportHandlerDirtySDK::~TransportHandlerDirtySDK()
	{
		#ifdef EA_DEBUG
			EA_ASSERT(mJobCount == 0); // If this fails then the TransportServer is leaking jobs.
		#endif
	}

	bool TransportHandlerDirtySDK::SetupHttpManager(HttpManagerRefT*& pHttpManager, bool syncJobs /* = false */)
	{
		const EA::WebKit::Parameters& params = EA::WebKit::GetParameters();
		if(!syncJobs)
			pHttpManager = HttpManagerCreate(params.mHttpRequestResponseBufferSize, params.mMaxTransportJobs);
		else
			pHttpManager = HttpManagerCreate(params.mHttpRequestResponseBufferSize, 1);

		// set callback function pointers
#if DIRTYVERS > 0x07050300
		HttpManagerCallback(pHttpManager, &TransportHandlerDirtySDK::DirtySDKSendHeaderCallbackStatic, &TransportHandlerDirtySDK::DirtySDKRecvHeaderCallbackStatic);
#else
		HttpManagerCallback(pHttpManager, &TransportHandlerDirtySDK::DirtySDKSendHeaderCallbackStatic, &TransportHandlerDirtySDK::DirtySDKRecvHeaderCallbackStatic, NULL);
#endif
		//Disable/Enable pipelining
#if (		((DIRTYVERS >= 0x07070000) && (DIRTYVERS < 0x08000000))			||				(DIRTYVERS >= 0x08050000))
		int32_t pipelining = params.mHttpPipeliningEnabled ? 1 : 0;
		HttpManagerControl(pHttpManager, -1, DIRTY_PIPE, pipelining, 0, NULL); 
#endif
		// set the redirection limit to a higher value
		HttpManagerControl(pHttpManager, -1, DIRTY_MAX_REDIRECTION, 10, 0, NULL); 

		//Set timeout for DirtySDK based on the page timeout of EAWebKit params
		HttpManagerControl(pHttpManager, -1, DIRTY_REQUEST_TIMEOUT, (int32_t)(params.mPageTimeoutSeconds * 1000), 0, NULL);

		// set debug level - currently it has no affect on non-debug builds.
#if defined(EA_DEBUG) || defined(_DEBUG)
		HttpManagerControl(pHttpManager, -1, DIRTY_LOG_LEVEL, 1, 0, NULL); 
#endif

#if (DIRTYVERS > 0x08090000)
		// Enable all the cipher suites including the ones added recently
		HttpManagerControl(pHttpManager, -1, DIRTY_CIPHER_SUITE, PROTOSSL_CIPHER_ALL, 0, NULL);
#endif

#if (DIRTYVERS > 0x08120000)
		// 02/29/2012 - abaldeva
		// Only tick the HttpManager from the NetConnIdle() if application indicated so. This setting helps resolve the situation where an application may want
		// to tick the EAWebKit and DirtySDK on different threads.
		// Prior to this change/DirtSDK version, we had a limitation that they 
		// should both be on same thread.
		if(!params.mTickHttpManagerFromNetConnIdle)
			HttpManagerControl(pHttpManager, -1, DIRTY_AUTO, FALSE, 0, NULL); 
#endif
		return true;
	}
	bool TransportHandlerDirtySDK::Init(const char16_t* pScheme)
	{
		if(EA::TransportHelper::Stricmp(pScheme, EA_CHAR16("http")) == 0 || EA::TransportHelper::Stricmp(pScheme, EA_CHAR16("https")) == 0)
		{
		#if (BUILDING_EAWEBKIT_DLL)
#if defined(EA_PLATFORM_SONY)
            SocketCreate(SCE_KERNEL_PRIO_FIFO_DEFAULT, 0, SCE_KERNEL_CPUMASK_USER_ALL);//PS
#elif defined(EA_PLATFORM_MICROSOFT)
            SocketCreate(IDLE_PRIORITY_CLASS, 0, 1);//PC&Xone.
#endif

         #else
			bool isStarted = (NetConnStatus(DIRTY_OPEN, 0, NULL, 0) != 0);	
			if ( !isStarted )
			{
				//Note by Arpit Baldeva: This assert is to safeguard against the accidental DirtySock/network layer start up of the application.
				//Even though the transport handler here has the capability to turn on the DirtySock network layer (and later shut it down), this is usually
				//not what a game team wants. 
				//It is safer to place a NetConnStartup("") somewhere in your application start up code and call NetConnShutdown(0) during shutdown to avoid 
				//this assert.
				EA_ASSERT_MSG(false,"EAWebKit Warning: Starting the network layer/DirtySock inside EAWebKit Transport Handler since application did not start it yet.\n");
				
				mIsDirtySockStartedHere = true;
				NetConnStartup(""); 
			}
		#endif
			
			//We handle both http and https schemes through the same HttpManager.
			if (!mpHttpManager)
			{
				SetupHttpManager(mpHttpManager);
				//We don't set up mpHttpManagerSyncJobs here. We adopt lazy initialization for it.
			}

			++mActiveSchemesCount;
			return true;
		}
		EA_ASSERT_MSG(false,"Unknown Scheme.\n");
		return false;
	}


	bool TransportHandlerDirtySDK::Shutdown(const char16_t* /*pScheme*/)
	{
		--mActiveSchemesCount;
		EA_ASSERT_MSG(mActiveSchemesCount >=0, "Are you calling Shutdown more number of times than Init?");
		
		// We don't shutdown unless we are the last scheme.
		if(mActiveSchemesCount == 0)
		{
			if(mSharedDecompressionBuffer)
			{
				GetAllocator()->Free(mSharedDecompressionBuffer,0);
				mSharedDecompressionBuffer = NULL;
			}
			if (mpHttpManager)
			{
				HttpManagerDestroy(mpHttpManager);
				mpHttpManager = NULL;
			}

			if(mpHttpManagerSyncJobs)
			{
				HttpManagerDestroy(mpHttpManagerSyncJobs);
				mpHttpManagerSyncJobs = NULL;
			}

		#if (BUILDING_EAWEBKIT_DLL)
			SocketDestroy(0);
		#else
			if ( mIsDirtySockStartedHere )
			{
				NetConnShutdown(0); 
				mIsDirtySockStartedHere = false;
			}
		#endif
		}
		return true;
	}


	bool TransportHandlerDirtySDK::InitJob(EA::WebKit::TransportInfo* pTInfo, bool& bStateComplete)
	{
		EA::WebKit::Allocator* pAllocator = GetAllocator();
		DirtySDKInfo* pDirtySDKInfo = new(pAllocator->Malloc(sizeof(DirtySDKInfo), 0, "EAWebKit/TransportHandlerDirtySDK")) DirtySDKInfo; 
		pTInfo->mTransportHandlerData = (uintptr_t)pDirtySDKInfo;

		HttpManagerRefT* pHttpManager = mpHttpManager;
		if(!pTInfo->mbAsync)
		{
			if(!mpHttpManagerSyncJobs)
				SetupHttpManager(mpHttpManagerSyncJobs,true);//Fixed a bug whereby the number of refs created in synchronous job pool were same as asynchronous job pool instead of intended lower number of refs.

			pHttpManager = mpHttpManagerSyncJobs;
		}

		// allocate an HTTP transfer handle
		pDirtySDKInfo->mHttpHandle = HttpManagerAlloc(pHttpManager);
		
		if (pDirtySDKInfo->mHttpHandle == 0)
		{
			EA_ASSERT_MSG(false,"TransportHandlerDirtySDK: ProtoHttpCreate failed.\n");
			return false;
		}

		// Copy the 16 bit URI to our 8 bit version. No encoding translations are done.
		const char16_t* tInfoURI = pTInfo->mURI.GetCharacters();
		eastl_size_t iEnd = (eastl_size_t)EA::TransportHelper::Strlen(tInfoURI);
		pDirtySDKInfo->mURI.resize(iEnd+1);
		for(eastl_size_t i = 0; i < iEnd; ++i)
			pDirtySDKInfo->mURI[i] = (char8_t)(tInfoURI[i]);

		pDirtySDKInfo->mURI[iEnd] ='\0';
	    
		// Make sure our header map has an Accept-Encoding entry.
		// By default WebKit doesn't create this header, nor does DirtySDK, 
		// yet some web servers are non-conforming and require it.
		if (!pTInfo->mHeaderMapOut.GetValue(httpHeaderNameString(WebCore::HTTPHeaderName::AcceptEncoding).upconvertedCharacters()))
		{
	#if ENABLE_PAYLOAD_DECOMPRESSION
			//Note by Arpit Baldeva: put deflate first as it has a lower overhead compared to gzip. Since we support both raw DEFLATE stream and zlib wrapped DEFLATE stream transparently,
			//we end up with lower overhead.
            pTInfo->mHeaderMapOut.SetValue(httpHeaderNameString(WebCore::HTTPHeaderName::AcceptEncoding).upconvertedCharacters(), EA_CHAR16("deflate,gzip,identity"));
	#else
            pTInfo->mHeaderMapOut.SetValue(httpHeaderNameString(WebCore::HTTPHeaderName::AcceptEncoding).upconvertedCharacters(), EA_CHAR16("identity"));
	#endif
		}
		
        if (!pTInfo->mHeaderMapOut.GetValue(httpHeaderNameString(WebCore::HTTPHeaderName::AcceptLanguage).upconvertedCharacters()))
		{
			const EA::WebKit::Parameters& params = GetParameters();
			if(params.mpAcceptLanguageHttpHeaderValue && params.mpAcceptLanguageHttpHeaderValue[0])
			{
				EA::TransportHelper::TransportString8 acceptLanguage8(params.mpAcceptLanguageHttpHeaderValue);
				EA::TransportHelper::TransportString16 acceptLanguage16;
				EA::TransportHelper::ConvertToString16(acceptLanguage8, acceptLanguage16);

                pTInfo->mHeaderMapOut.SetValue(httpHeaderNameString(WebCore::HTTPHeaderName::AcceptLanguage).upconvertedCharacters(), acceptLanguage16.c_str());
			}
		}
		
		pDirtySDKInfo->mbHeadersReceived = false;

		#ifdef EA_DEBUG
			mJobCount++;
		#endif

		bStateComplete = true;
		return true;
	}

	bool TransportHandlerDirtySDK::ShutdownJob(EA::WebKit::TransportInfo* pTInfo, bool& bStateComplete)
	{
		if(pTInfo->mTransportHandlerData)
		{
			DirtySDKInfo*          pDirtySDKInfo  = (DirtySDKInfo*)pTInfo->mTransportHandlerData;
			EA::WebKit::Allocator* pAllocator     = GetAllocator();

			HttpManagerRefT* pHttpManager = mpHttpManager;
			if(!pTInfo->mbAsync)
				pHttpManager = mpHttpManagerSyncJobs;
			
			if(pDirtySDKInfo->mHttpHandle)
				HttpManagerFree(pHttpManager, pDirtySDKInfo->mHttpHandle);

			pDirtySDKInfo->~DirtySDKInfo();
			pAllocator->Free(pDirtySDKInfo, sizeof(DirtySDKInfo));
			pTInfo->mTransportHandlerData = 0;

			#ifdef EA_DEBUG
				mJobCount--;
			#endif
		}

		bStateComplete = true;
		return true;
	}



	bool TransportHandlerDirtySDK::Connect(EA::WebKit::TransportInfo* pTInfo, bool& bStateComplete)
	{
		bool    bReturnValue = true;
		int32_t iResult = 0;

		EA_ASSERT(pTInfo->mTransportHandlerData);

		DirtySDKInfo* pDirtySDKInfo = (DirtySDKInfo*)pTInfo->mTransportHandlerData;

		HttpManagerRefT* pHttpManager = mpHttpManager;
		if(!pTInfo->mbAsync)
			pHttpManager = mpHttpManagerSyncJobs;

		// set callback user info
		HttpManagerControl(pHttpManager, pDirtySDKInfo->mHttpHandle, DIRTY_CALLBACK_USER_POINTER, 0, 0, (void *)pTInfo);

#if 0
		// Added some debug code so that you can hit the breakpoint quickly in a URL of interest. This assists 
		// in debugging.
		if(pDirtySDKInfo->mURI.find("facebook.com/extern") != EA::TransportHelper::TransportString8::npos)
		{
			int hitBreakPoint =0;
			++hitBreakPoint;
		}
#endif
		switch(pTInfo->mHttpRequestType)
		{
		case EA::WebKit::kHttpRequestTypeGET:
		case EA::WebKit::kHttpRequestTypeHEAD:
			//iResult = HttpManagerGet(pHttpManager, pDirtySDKInfo->mHttpHandle, pDirtySDKInfo->mURI.c_str(), PROTOHTTP_HEADBODY);
			iResult = HttpManagerRequest(pHttpManager, pDirtySDKInfo->mHttpHandle, pDirtySDKInfo->mURI.c_str(), NULL, 0, ((pTInfo->mHttpRequestType == EA::WebKit::kHttpRequestTypeGET) ? PROTOHTTP_REQUESTTYPE_GET : PROTOHTTP_REQUESTTYPE_HEAD));
			break;
		case EA::WebKit::kHttpRequestTypePOST:
		case EA::WebKit::kHttpRequestTypePUT:
			{

				// Original code that did a chunked send but which doesn't work with HTTP servers.
				// pDirtySDKInfo->mbPostActive        = true;
				// pDirtySDKInfo->mPostBufferSize     = 0;
				// pDirtySDKInfo->mPostBufferPosition = 0;
				// iResult = ProtoHttpPost(pDirtySDKInfo->mpProtoHttp, pDirtySDKInfo->mURI.c_str(), NULL, PROTOHTTP_STREAM_BEGIN, PROTOHTTP_POST);
				// if(iResult == PROTOHTTP_STREAM_BEGIN) // PROTOHTTP_STREAM_BEGIN == -1, so we can have an iResult that is negative yet not really an error.
				//    iResult = 0;

				// New code whereby we don't use chunked data.
				pDirtySDKInfo->mPostData.clear();      // Shouldn't be necessary.
				pDirtySDKInfo->mbPostActive = false;    // Skip right to reading the data from the server (see the Transfer function).

				char    buffer[256];
				int64_t size;

				do {
					size = pTInfo->mpTransportServer->ReadData(pTInfo, buffer, sizeof(buffer));
					if(size > 0)
						pDirtySDKInfo->mPostData.append(buffer, (EA::TransportHelper::TransportString8::size_type)(size));
				} while(size > 0);

				EA_ASSERT(size == 0);
				if(size == 0)
				{
					// abaldeva: Don't want to use swprintf. That may be broken on OS X depending upon the 
					// build flags.
					const size_t kMaxDigits = 32; 
					char8_t bufferLen[kMaxDigits];
					int numChars = sprintf(bufferLen, "%u",(unsigned)pDirtySDKInfo->mPostData.length());
					EA_ASSERT_FORMATTED(numChars < kMaxDigits-1, ("Impossible length for POST data - %d", numChars));

					char16_t bufferLen16[kMaxDigits];
					char8_t* pSrc = bufferLen;
					char16_t* pDest = bufferLen16;
					while(numChars > 0)
					{
						*pDest++ = (char16_t)(*pSrc++);
						--numChars;
					}
					*pDest = 0;


                    pTInfo->mHeaderMapOut.SetValue(httpHeaderNameString(WebCore::HTTPHeaderName::ContentLength).upconvertedCharacters(), bufferLen16);
					//iResult = HttpManagerPost(pHttpManager, pDirtySDKInfo->mHttpHandle, pDirtySDKInfo->mURI.c_str(), pDirtySDKInfo->mPostData.c_str(), (int32_t) pDirtySDKInfo->mPostData.length(), PROTOHTTP_POST);
					iResult = HttpManagerRequest(pHttpManager, pDirtySDKInfo->mHttpHandle, pDirtySDKInfo->mURI.c_str(), pDirtySDKInfo->mPostData.c_str(), (int32_t) pDirtySDKInfo->mPostData.length(), ((pTInfo->mHttpRequestType == EA::WebKit::kHttpRequestTypePOST) ? PROTOHTTP_REQUESTTYPE_POST : PROTOHTTP_REQUESTTYPE_PUT));

				}
				break;
			}

		case EA::WebKit::kHttpRequestTypeOPTIONS:
#if DIRTYVERS >= 0x08060000		
			iResult = HttpManagerRequest(pHttpManager, pDirtySDKInfo->mHttpHandle, pDirtySDKInfo->mURI.c_str(), NULL, 0, PROTOHTTP_REQUESTTYPE_OPTIONS);
#endif
			break;
		case EA::WebKit::kHttpRequestTypeDELETE:
			iResult = HttpManagerRequest(pHttpManager, pDirtySDKInfo->mHttpHandle, pDirtySDKInfo->mURI.c_str(), NULL, 0, PROTOHTTP_REQUESTTYPE_DELETE);
			break;
		default:
			EA_ASSERT_MSG(false,"Unsupported Http request method\n");
			break;
		}

		if(iResult < 0)
		{
			EA_ASSERT_MSG(false, "TransportHandlerDirtySDK: Http request failed.\n");
			bReturnValue = false;
		}

		// Disable certificate validation if set to do so. An application normally doesn't want to do this, but it's useful for debugging.
		if(!pTInfo->mbVerifySSLCert)
			HttpManagerControl(pHttpManager, pDirtySDKInfo->mHttpHandle, DIRTY_DISABLE_CERT_VALIDATION,1, 0, NULL); 

		bStateComplete = true;
		return bReturnValue;
	}

	#if DIRTYVERS > 0x07050300 
	int32_t 
	#else
	void
	#endif
	TransportHandlerDirtySDK::DirtySDKSendHeaderCallbackStatic(ProtoHttpRefT* pState, char* pHeader, uint32_t uHeaderCapacity, const char* pBody, int64_t uBodyLen, void* pUserRef)
	{
		EA::WebKit::TransportInfo* pTInfo = static_cast<EA::WebKit::TransportInfo*>(pUserRef);
	#if DIRTYVERS > 0x07050300 
		int32_t iResult = ((TransportHandlerDirtySDK*)pTInfo->mpTransportHandler)->DirtySDKSendHeaderCallback(pState, pHeader, uHeaderCapacity, pBody, uBodyLen, pTInfo);
		return iResult;
	#else
		((TransportHandlerDirtySDK*)pTInfo->mpTransportHandler)->DirtySDKSendHeaderCallback(pState, pHeader, uHeaderCapacity, pBody, uBodyLen, pTInfo);
	#endif
	}

	// This is called by DirtySDK before it sends headers to the HTTP server. We can possibly revise the headers here.
	//
	// pHeader is 0-terminated the raw header text received by ProtoHttp.
	// uHeaderCapacity is the capacity of the buffer pointed to by pHeader, which will usually be > than the strlen of pHeader.
	// pData is the immutable body data that will be sent. It is data and so not necessarily 0-terminated.
	// uDataLen is the length of the body data.
	//
	// We are expected to possibly rewrite pHeader, though we can't use more than uHeaderCapacity space.
	//
	int32_t TransportHandlerDirtySDK::DirtySDKSendHeaderCallback(ProtoHttpRefT* pState, char* pHeader, uint32_t uHeaderCapacity, const char* /*pBody*/, uint32_t /*uBodyLen*/, EA::WebKit::TransportInfo* pTInfo)
	{
		
		EA::TransportHelper::TransportHeaderMap     headerMapDSDK;
		EA::TransportHelper::TransportString16		sCommandLine;
		char* orignialHeaderLoc = pHeader;
		int32_t originalCapacity = uHeaderCapacity;
		char* p = pHeader;
		DirtySDKInfo* pDirtySDKInfo = (DirtySDKInfo*)pTInfo->mTransportHandlerData;

		pDirtySDKInfo->mSendIndex++;  // The first time through, this changes from 0 to 1.

		if(pDirtySDKInfo->mSendIndex > 1) // If this is the 2nd or later time through... then that means we have been redirected by the server to a new URL...
		{
			// If this is a redirect (mSendIndex > 1) then remove cookies from the 
			// previous time through (when mSendIndex == 0) and re-add them now, as they may
			// have changed, especially if the redirect happened to give us a new cookie. 
			// Some servers (including EA servers) rely on this.
			ReattachCookies(pTInfo);
		}

		// if(pDirtySDKInfo->mSendIndex == 1) // If this is the first GET and not a possible subsequent GET after a (e.g. 302) redirect...
		{
			// Save the first line from pHeader, as it is the command line (e.g. the "GET" line).
			while(*p && ((*p != '\r') && (*p != '\n')))
				sCommandLine += (uint8_t)*p++;

			// Move past the new line.
			while(*p && ((*p == '\r') || (*p == '\n')))
				p++;

			// We read the text into a temp header map, do any modifications, then write the result out.
			bool errorEncountered = EA::TransportHelper::SetHeaderMapWrapperFromText(p, (uint32_t)strlen(p), headerMapDSDK, false, true);
			(void)(errorEncountered);
			//TODO: Do something useful here in the release build? Indicate the error?
			EA_ASSERT_MSG(!errorEncountered, "The incoming text header map has some error. The resource response object created using partial header info may not be valid.");

			// Write sCommandLine
			const uint32_t commandSize = (uint32_t)(sCommandLine.length() + 2); // +2 for \r\n. 

			if(uHeaderCapacity >= commandSize) // Use > instead of >= because we want to write a terminating 0 char.
			{
				for(eastl_size_t i = 0, iEnd = sCommandLine.length(); i != iEnd; ++i)
					*pHeader++ = (uint8_t)sCommandLine[i];

				*pHeader++ = '\r';
				*pHeader++ = '\n';
				*pHeader   = 0;

				uHeaderCapacity -= commandSize;
			}

			// Fix the Host header to remove :80 if it is present. Some web servers (e.g. Google's www.gstatic.com or www.twitter.com) 
			// are broken and expect that :80 is never present, even though :80 is valid.
            EA::TransportHelper::TransportHeaderMap::iterator itHost = headerMapDSDK.find_as(httpHeaderNameString(WebCore::HTTPHeaderName::Host).upconvertedCharacters(), EA::TransportHelper::str_iless());

			if(itHost != headerMapDSDK.end())  // This should always be true.
			{
				//Note by Arpit Baldeva: As it happens, we can send a request as "http". Server redirects that request to a "https" url. Now, DirtySDK
				//send the headers back but we failed to strip the port information in following logic previously because our scheme stays as "http".
				//Following is a workaround for this.
				
				//no other side effects are known at this time of EAWebKit and DirtySDK being a little our of sync
				//in this case. Probably a better fix would be to changes the transportInfo scheme but I am not sure of the ramifications currently.
				if(EA::TransportHelper::Stricmp(pTInfo->mScheme, EA_CHAR16("http")) == 0 || EA::TransportHelper::Stricmp(pTInfo->mScheme, EA_CHAR16("https")) == 0)
				{
					EA::TransportHelper::TransportHeaderMap::mapped_type& sValueHost = itHost->second;
					eastl_size_t sizeStr = sValueHost.size();
					if((sizeStr >= 3) && sValueHost.substr(sizeStr-3,sizeStr) == EA_CHAR16(":80")) //strlen ":80" = 3
					{
						sValueHost.erase(sizeStr-3);  // Remove the ":80", as some servers mistakenly fail when it's present for HTTP.
					}
					else if((sizeStr >= 4) && sValueHost.substr(sizeStr-4,sizeStr) == EA_CHAR16(":443"))//strlen ":443" = 4
					{
						sValueHost.erase(sizeStr-4);  // Remove the ":443", as some servers mistakenly fail when it's present for HTTPS.
					}
				}
				/*else if(EA::TransportHelper::Stricmp(pTInfo->mScheme, L"https") == 0)
				{
					EA::TransportHelper::TransportHeaderMap::mapped_type& sValueHost = itHost->second;
					size_t sizeStr = sValueHost.size();
					
				}*/
			}

            EA::TransportHelper::CopyHeaderLine(httpHeaderNameString(WebCore::HTTPHeaderName::Host).upconvertedCharacters(), headerMapDSDK, pHeader, uHeaderCapacity);
            EA::TransportHelper::CopyHeaderLine(httpHeaderNameString(WebCore::HTTPHeaderName::TransferEncoding).upconvertedCharacters(), headerMapDSDK, pHeader, uHeaderCapacity);
            EA::TransportHelper::CopyHeaderLine(httpHeaderNameString(WebCore::HTTPHeaderName::Connection).upconvertedCharacters(), headerMapDSDK, pHeader, uHeaderCapacity);
	  
			//TODO: DirtySDK user-agent detection. Make this a simple static operation. Do it once at the transport handler init.

			// We append the DirtySDK user-agent string to our user-agent string.
            EA::TransportHelper::TransportHeaderMap::const_iterator itDSDK = headerMapDSDK.find_as(httpHeaderNameString(WebCore::HTTPHeaderName::UserAgent).upconvertedCharacters(), EA::TransportHelper::str_iless());
			if(itDSDK != headerMapDSDK.end())  // This happens to always be true.
			{
				const EA::TransportHelper::TransportHeaderMap::mapped_type& sValueDSDK = itDSDK->second;
                EA::TransportHelper::TransportString16 userAgentWebKit(pTInfo->mHeaderMapOut.GetValue(httpHeaderNameString(WebCore::HTTPHeaderName::UserAgent).upconvertedCharacters()));
				if(!userAgentWebKit.empty())
				{
					if(pDirtySDKInfo->mSendIndex == 1) // Only do it the first time (i.e. not on redirects), lest we repeat appending each time through.
					{
						userAgentWebKit += EA_CHAR16(' ');
						userAgentWebKit += sValueDSDK;
						//Note by Arpit Baldeva:Comment out the line below and modify its content to set up a user-agent string for debug purpose.
						//userAgentWebKit = L"Mozilla/5.0 (Windows; U; Windows NT 5.1; en-US; rv:1.9.0.19) Gecko/2010031422 Firefox/3.0.19 (.NET CLR 3.5.30729)";
                        pTInfo->mHeaderMapOut.SetValue(httpHeaderNameString(WebCore::HTTPHeaderName::UserAgent).upconvertedCharacters(), userAgentWebKit.c_str());
					}
				}
				else
				{
                    pTInfo->mHeaderMapOut.SetValue(httpHeaderNameString(WebCore::HTTPHeaderName::UserAgent).upconvertedCharacters(), sValueDSDK.c_str());
				}
			}
			
			*pHeader = 0; // In case there are no header lines in mHeaderMapOut which we write next.

			if((pTInfo->mHttpRequestType == EA::WebKit::kHttpRequestTypePOST || pTInfo->mHttpRequestType == EA::WebKit::kHttpRequestTypePUT) 
                && !(pTInfo->mHeaderMapOut.GetValue(httpHeaderNameString(WebCore::HTTPHeaderName::ContentType).upconvertedCharacters())) //If content type does not exist already....
				)
			{
                pTInfo->mHeaderMapOut.SetValue(httpHeaderNameString(WebCore::HTTPHeaderName::ContentType).upconvertedCharacters(), EA_CHAR16("application/x-www-form-urlencoded"));
			}


			// Nicki Vankoughnett: A redirection may create a situation whereby we need to send different
			// headers than we sent for the original request. An example would be if the original request
			// was a POST (with body content) but the redirect was to a location that we need to GET from,
			// whereby we no longer have to provide the body content and thus its associated headers.
			if(sCommandLine.find(EA_CHAR16("GET")) == 0)
			{
                pTInfo->mHeaderMapOut.EraseValue(httpHeaderNameString(WebCore::HTTPHeaderName::ContentLength).upconvertedCharacters());
                pTInfo->mHeaderMapOut.EraseValue(httpHeaderNameString(WebCore::HTTPHeaderName::ContentType).upconvertedCharacters());
			}

			int32_t textSizeRequired = SetTextFromHeaderMapWrapper(pTInfo->mHeaderMapOut, pHeader, uHeaderCapacity);
			if(textSizeRequired > (int32_t) uHeaderCapacity)
			{
				return textSizeRequired + (originalCapacity - uHeaderCapacity);
			}
			else
			{
				int32_t returnVal = (int32_t)EA::TransportHelper::Strlen(orignialHeaderLoc);
				return returnVal;
			}
			
		}
	}


	#if DIRTYVERS > 0x07050300 
	int32_t 
	#else
	void
	#endif
	TransportHandlerDirtySDK::DirtySDKRecvHeaderCallbackStatic(ProtoHttpRefT* pState, const char* pHeader, uint32_t uHeaderSize, void* pUserRef)
	{
		EA::WebKit::TransportInfo* pTInfo = static_cast<EA::WebKit::TransportInfo*>(pUserRef);

		((TransportHandlerDirtySDK*)pTInfo->mpTransportHandler)->DirtySDKRecvHeaderCallback(pState, pHeader, uHeaderSize, pTInfo);
	#if DIRTYVERS > 0x07050300 
		return 0;
	#endif
	}

	void TransportHandlerDirtySDK::DirtySDKRecvHeaderCallback(ProtoHttpRefT* pState, const char* pHeader, uint32_t uHeaderSize, EA::WebKit::TransportInfo* pTInfo)
	{
		// pHeader includes all the received header text.
		DirtySDKInfo* pDirtySDKInfo = (DirtySDKInfo*)pTInfo->mTransportHandlerData;
		HttpManagerRefT* pHttpManager = mpHttpManager;
		if(!pTInfo->mbAsync)
			pHttpManager = mpHttpManagerSyncJobs;

		pDirtySDKInfo->mbHeadersReceived = true;

		// Check for a 200, 404, etc. code. ProtoHttpStatus returns -1 if headers have not been successfully received.
 		// abaldeva: 05/02/11 - Since we are in a Callback, we should be using ProtoHttp directly instead of the HttpManager(as per DirtySDK team)
		// Otherwise, in some edge cases, we may end up not getting a valid response code.
		pTInfo->mResultCode = ProtoHttpStatus(pState, DIRTY_RESPONSE_CODE, NULL, 0); 
		EA_ASSERT_MSG(pTInfo->mResultCode != -1, "Expecting a valid Http response code\n");

		//Store the headers in the incoming header map.
		bool errorEncountered = SetHeaderMapWrapperFromText(pHeader, uHeaderSize, pTInfo->mHeaderMapIn, true, true);
		(void) errorEncountered;
		EA_ASSERT_MSG(!errorEncountered, "The incoming text header map has some error. The resource response object created using partial header info may not be valid.");

		SetupStreamDecompressor(pTInfo);
		
		pTInfo->mpTransportServer->HeadersReceived(pTInfo);

		CookiesReceived(pTInfo);
	}


	
	void TransportHandlerDirtySDK::SetupStreamDecompressor(EA::WebKit::TransportInfo* pTInfo)
	{
		(void) pTInfo;
#if ENABLE_PAYLOAD_DECOMPRESSION
		DirtySDKInfo* pDirtySDKInfo = (DirtySDKInfo*)pTInfo->mTransportHandlerData;
		
        const char16_t* pContentEncodingType(pTInfo->mHeaderMapIn.GetValue(httpHeaderNameString(WebCore::HTTPHeaderName::ContentEncoding).upconvertedCharacters()));
		if(pContentEncodingType)
		{
			EA::WebKit::Allocator* pAllocator = GetAllocator();
			//This can happen, for example, on a redirect. Free the existing decompressor in case the content encoding type changes on the redirect.
			if(pDirtySDKInfo->mStreamDecompressor)
			{
				pDirtySDKInfo->mStreamDecompressor->~IStreamDecompressor();
				GetAllocator()->Free(pDirtySDKInfo->mStreamDecompressor,0);
				pDirtySDKInfo->mStreamDecompressor = 0;
			}

			bool streamEncoded = false;
			if(EA::TransportHelper::Stricmp(pContentEncodingType, EA_CHAR16("deflate")) == 0)
			{

				pDirtySDKInfo->mStreamDecompressor = new(pAllocator->Malloc(sizeof(DeflateStreamDecompressor),0,0)) DeflateStreamDecompressor(eStreamTypeZLib);
				pDirtySDKInfo->mStreamDecompressor->SetDecompressedDataCallback(DecompressedDataCallbackFunc, pTInfo);
				streamEncoded = true;

			}
			else if(EA::TransportHelper::Stricmp(pContentEncodingType, EA_CHAR16("gzip")) == 0)
			{
				pDirtySDKInfo->mStreamDecompressor = new(pAllocator->Malloc(sizeof(DeflateStreamDecompressor),0,0)) DeflateStreamDecompressor(eStreamTypeGZip);
				pDirtySDKInfo->mStreamDecompressor->SetDecompressedDataCallback(DecompressedDataCallbackFunc, pTInfo);
				streamEncoded = true;
			}

			//Do lazy allocation of the shared decompression buffer
			if(!mSharedDecompressionBuffer && streamEncoded)
				mSharedDecompressionBuffer = (uint8_t*)pAllocator->Malloc(EA::TransportHelper::kSharedDecompressionBufferSize,0,"EAWebKitZLibSharedDecompressionBuffer");

		}
		else
		{
			//This can happen, for example, on a redirect where the original response was compressed but redirected response is not compressed. Free the existing decompressor.
			if(pDirtySDKInfo->mStreamDecompressor)
			{
				pDirtySDKInfo->mStreamDecompressor->~IStreamDecompressor();
				GetAllocator()->Free(pDirtySDKInfo->mStreamDecompressor,0);
				pDirtySDKInfo->mStreamDecompressor = 0;
			}
		}
#endif
	}

	bool TransportHandlerDirtySDK::Disconnect(EA::WebKit::TransportInfo* /*pTInfo*/, bool& bStateComplete)
	{
		// We don't do any kind of disconnect here. We do all disconnect/shutdown 
		// functionality in out ShutdownJob function.
		bStateComplete = true;
		return true;
	}

	bool TransportHandlerDirtySDK::Tick()
	{
		if(mpHttpManager)    
			HttpManagerUpdate(mpHttpManager);
		if(mpHttpManagerSyncJobs)
			HttpManagerUpdate(mpHttpManagerSyncJobs);
		
		return true;
	}

	bool TransportHandlerDirtySDK::Transfer(EA::WebKit::TransportInfo* pTInfo, bool& bStateComplete)
	{
		bool          bReturnValue = true;
		char          buffer[1024]; 
		int32_t       iResult = 0;
		DirtySDKInfo* pDirtySDKInfo = (DirtySDKInfo*)pTInfo->mTransportHandlerData;
		HttpManagerRefT* pHttpManager = mpHttpManager;
		if(!pTInfo->mbAsync)
			pHttpManager = mpHttpManagerSyncJobs;

		const double dCurrentTime = GetTime();
	 
		if(dCurrentTime > pTInfo->mTimeout)
			bReturnValue = false;

		switch(pTInfo->mHttpRequestType)
		{
		//Note by Arpit Baldeva: GET and POST method response usually would have some data to go with.
		case EA::WebKit::kHttpRequestTypeGET:
		case EA::WebKit::kHttpRequestTypePOST:
		
		case EA::WebKit::kHttpRequestTypePUT:
		case EA::WebKit::kHttpRequestTypeHEAD:
		case EA::WebKit::kHttpRequestTypeDELETE:
		case EA::WebKit::kHttpRequestTypeOPTIONS:
			{
				while((iResult = HttpManagerRecv(pHttpManager, pDirtySDKInfo->mHttpHandle, buffer, 1, sizeof(buffer))) > 0)  // While there is received data...
				{
					EA_ASSERT_MSG(pDirtySDKInfo->mbHeadersReceived,"The headers should have been received and processed by this time through the DirtySDK callback\n");
	#if ENABLE_PAYLOAD_DECOMPRESSION
					if(pDirtySDKInfo->mStreamDecompressor)
					{
						if(pDirtySDKInfo->mStreamDecompressor->Decompress((uint8_t*)buffer,iResult,mSharedDecompressionBuffer,EA::TransportHelper::kSharedDecompressionBufferSize)<0)//if there is any error in the processing of stream, error out.
						{
							pTInfo->mpTransportServer->DataDone(pTInfo, false);
							bReturnValue   = false;
							bStateComplete = true;
							return bReturnValue;
						}
					}
					else
	#endif
					{
						pTInfo->mpTransportServer->DataReceived(pTInfo, buffer,iResult);
					}
				}
				break;
			}

		// handle a default case as otherwise it fails to compile
		default:
			break;
		}
		
		// iResult is one of:
		//   #define  PROTOHTTP_RECVDONE   (-1) // receive operation complete, and all data has been read 
		//   #define  PROTOHTTP_RECVFAIL   (-2) // receive operation failed 
		//   #define  PROTOHTTP_RECVWAIT   (-3) // waiting for body data 
		//   #define  PROTOHTTP_RECVHEAD   (-4) // in headonly mode and header has been received 
		//   #define  PROTOHTTP_RECVBUFF   (-5) // recvall did not have enough space in the provided buffer 

		EA::WebKit::EAWebKitClient* const pClient = GetEAWebKitClient();
		switch (iResult)
		{
#if DIRTYVERS <= 0x080D0000 //8.13.0
		/* 05/02/11 - abaldeva: Fix for reload not working. WebKit now sends an if-modified-since header to the server. In case the resource
		is not modified, the server sends a 304 response. Currently, DirtySDK does not send us a PROTOHTTP_RECVDONE in this case. So we need
		to tell the transport server by detecting this condition that we are done with transfer.
		The reason to make it a conditional compile is so that we are not dependent on this workaround in the future versions.
		*/
		case 0:
			if(pTInfo->mResultCode == 304)
			{
				EA_ASSERT_MSG(pDirtySDKInfo->mbHeadersReceived,"The headers should have been received and processed by this time through the DirtySDK callback\n");
				pTInfo->mpTransportServer->DataDone(pTInfo, true);
				bReturnValue   = true;
				bStateComplete = true;
			}
			break;
#endif
		case PROTOHTTP_RECVDONE:
			//Note by Arpit Baldeva: Old code processed the header here as well. But we should really be processing headers from the DirtySDK callback. 
			//Old comment - "It's possible this will be called if the body size is zero."
			EA_ASSERT_MSG(pDirtySDKInfo->mbHeadersReceived,"The headers should have been received and processed by this time through the DirtySDK callback\n");
			pTInfo->mpTransportServer->DataDone(pTInfo, true);
			bReturnValue   = true;
			bStateComplete = true;
			break;

		case PROTOHTTP_RECVFAIL:
			{
				pTInfo->mpTransportServer->DataDone(pTInfo, false);
				bReturnValue   = false;
				bStateComplete = true;

				//We would rather not have code structured as follows (multiple if conditions) but in DirtySDK, there is no uniform set of errors. The error interpretation
				//depends on what you are querying. 
				//We don't see this getting bigger so its okay for now.
				if(pClient)
				{
					EA::WebKit::NetworkErrorInfo info;
					eastl::string8 errorMessage;
					int32_t timeout = HttpManagerStatus(pHttpManager, pDirtySDKInfo->mHttpHandle, DIRTY_REQUEST_TIMEOUT, NULL, 0);
					if(timeout)
					{
						info.mNetworkErrorType = EA::WebKit::kNetworkErrorTimeOut;
						info.mNetworkErrorCode = timeout;
						errorMessage.append_sprintf("Timeout on %s",pDirtySDKInfo->mURI.c_str());
						info.mErrorMessage =  errorMessage.c_str();
				
						pClient->NetworkError(info);
						break;
					}

#if DIRTYVERS >= 0x07060A00
					int32_t essl = HttpManagerStatus(pHttpManager, pDirtySDKInfo->mHttpHandle, DIRTY_ESSL, NULL, 0);
					if(essl < 0 && (pDirtySDKInfo->mURI.find("https") != EA::TransportHelper::TransportString8::npos))//DirtySDK triggers the error for even non SSL links. Don't report those errors.
					{
                       	info.mNetworkErrorType = EA::WebKit::kNetworkErrorSSLCert;
						info.mNetworkErrorCode = essl;
						errorMessage.append_sprintf("Navigation won't succeed. SSL processing error on %s, DirtySDK error code is %d",pDirtySDKInfo->mURI.c_str(), info.mNetworkErrorCode);

                        // 4/25/11 - CSidhall added more cert info printout for when new lib is available    
                        #if (DIRTYVERS >= 0x080D0000)  // 8.13.00 
                        // Retrieve more info with new lib support if this is a certificate fail
                        if( (essl == PROTOSSL_ERROR_CERT_INVALID) || 
                            (essl == PROTOSSL_ERROR_CERT_HOST) ||  
                            (essl == PROTOSSL_ERROR_CERT_NOTRUST) )
                        {
                            ProtoSSLCertInfoT certInfo;
                            int32_t certInfoSize = sizeof(ProtoSSLCertInfoT);
                            int32_t status = HttpManagerStatus(pHttpManager, pDirtySDKInfo->mHttpHandle, DIRTY_CERT, &certInfo, certInfoSize);
                            if(status == PROTOSSL_ERROR_NONE)
                            {
                                // The needed authority is not always set in common so we print out all fields to give the user feedback.
                                errorMessage.append_sprintf("\n Country: %s \n State: %s \n City: %s \n Org: %s \n Unit: %s \n Common: %s \n",
                                certInfo.Ident.strCountry,
                                certInfo.Ident.strState,
                                certInfo.Ident.strCity,
                                certInfo.Ident.strOrg,
                                certInfo.Ident.strUnit,
                                certInfo.Ident.strCommon);
                            }
                        }
                    #endif
                        
                        // Send out the final message notification
						info.mErrorMessage =  errorMessage.c_str();
                        pClient->NetworkError(info);
						break;
					}
#endif //DIRTYVERS >= 0x07060A00

					//Default case.

					errorMessage.append_sprintf("Unknown network error on %s",pDirtySDKInfo->mURI.c_str());
					info.mErrorMessage =  errorMessage.c_str();
					pClient->NetworkError(info);
				}

				break;
			}

		case PROTOHTTP_RECVWAIT:
			break;
		case PROTOHTTP_RECVHEAD:
			//Note by Arpit Baldeva: Sticking a DataDone here because DirtySDK explicitly notifies about this return value and PROTOHTTP_RECVDONE
			//is not returned.
			EA_ASSERT_MSG(pDirtySDKInfo->mbHeadersReceived,"The headers should have been received and processed by this time through the DirtySDK callback\n");
			pTInfo->mpTransportServer->DataDone(pTInfo, true);
			bReturnValue   = true;
			bStateComplete = true;
		case PROTOHTTP_RECVBUFF:
			break;
		}
#if 0 //Note by Arpit Baldeva: Disable the unused code for http post streaming.	
		else // POST
		{
			if(pDirtySDKInfo->mPostBufferSize == 0)
			{
				EA_ASSERT(pDirtySDKInfo->mPostBufferPosition == 0);

				pDirtySDKInfo->mPostBufferSize = pTInfo->mpTransportServer->ReadData(pTInfo, pDirtySDKInfo->mPostBuffer, sizeof(pDirtySDKInfo->mPostBuffer));

				if(pDirtySDKInfo->mPostBufferSize == 0) // If there was nothing else to read, and the read was thus completed...
				{
					HttpManagerSend(pHttpManager, pDirtySDKInfo->mHttpHandle, NULL, PROTOHTTP_STREAM_END);
					pDirtySDKInfo->mbPostActive = false;
	                
				}
				else if(pDirtySDKInfo->mPostBufferSize < 0)
				{
					bReturnValue   = false;
					bStateComplete = true;
				}
				// Else we have POST data, so fall through and send it
			}

			int64_t postSize = (pDirtySDKInfo->mPostBufferSize - pDirtySDKInfo->mPostBufferPosition);

			if(postSize > 0)
			{
				iResult = HttpManagerSend(pHttpManager, pDirtySDKInfo->mHttpHandle, pDirtySDKInfo->mPostBuffer + pDirtySDKInfo->mPostBufferPosition, (int32_t)postSize);

				if(iResult > 0)
				{
					pDirtySDKInfo->mPostBufferPosition += iResult;;

					if(pDirtySDKInfo->mPostBufferPosition == pDirtySDKInfo->mPostBufferSize)
					{
						pDirtySDKInfo->mPostBufferPosition = 0;
						pDirtySDKInfo->mPostBufferSize     = 0;
					}
				}
				else if(iResult < 0)
				{
					bReturnValue   = false;
					bStateComplete = true;
				}
				// else do nothing because nothing was sent.
			}
		}
#endif //if 0

		return bReturnValue;
	}


	bool TransportHandlerDirtySDK::CanCacheToDisk()
	{
		return true;
	}

} // namespace WebKit
} // namespace EA

namespace EA
{
namespace WebKit
{
// Only AF_INET (IPv4) socket family is supported. IPv6 is not supported.
// All socket calls are asynchronous(read/write/connect) etc.
SocketTransportHandlerDirtySDK::SocketTransportHandlerDirtySDK()
{
}

SocketTransportHandlerDirtySDK::~SocketTransportHandlerDirtySDK()
{

}

bool SocketTransportHandlerDirtySDK::Init()
{
	bool success = mSocketInfoList.empty();
	EA_ASSERT_MSG(success, "SocketInfoList should be empty at Init");

	return success;
}

bool SocketTransportHandlerDirtySDK::Shutdown()
{
	SocketInfoList::iterator iter = mSocketInfoList.begin();
	while(iter != mSocketInfoList.end())
	{
		SocketInfoT* sockInfo = *iter;
		EA_ASSERT_MSG(sockInfo->closeRequested,"All instances should have close requested");
		
		ProtoSSLDestroy(sockInfo->socketHandle);
		delete sockInfo;
		iter = mSocketInfoList.erase(iter);
	}

	mSocketInfoList.resize(0);

	return true;
}

EA::WebKit::SocketHandle SocketTransportHandlerDirtySDK::OpenSocket(const char8_t* hostUrl, uint32_t port, EA::WebKit::SocketType sockType, EA::WebKit::SocketHandleClient* sockHandleClient)
{
	ProtoSSLRefT* socketHandle = ProtoSSLCreate();
	if(socketHandle)
	{
		EA_ASSERT_MSG((sockType == EA::WebKit::kSecuredTCP || sockType == EA::WebKit::kUnsecuredTCP), "Unknown socket type");
		int32_t isSecure = (sockType == EA::WebKit::kSecuredTCP ? 1 : 0);
		
		int32_t errorCode = ProtoSSLConnect(socketHandle,isSecure,hostUrl,0,port);
		if(errorCode == SOCKERR_NONE)
		{
			SocketInfoT* sockInfo = new SocketInfoT();
			sockInfo->port = port;
			sockInfo->socketHandle = socketHandle;
			sockInfo->socketHandleClient = sockHandleClient;
			sockInfo->errorCode = SOCKERR_NONE;
			sockInfo->connected = false;
            sockInfo->listening = false;
			sockInfo->closeRequested = false;

			mSocketInfoList.push_back(sockInfo);

			return (EA::WebKit::SocketHandle)(socketHandle);
		}
	}
	return NULL;
}

EA::WebKit::SocketHandle SocketTransportHandlerDirtySDK::Listen(const char8_t* addr, uint32_t port, EA::WebKit::SocketHandleClient* sockHandleClient)
{
	ProtoSSLRefT* socketHandle = ProtoSSLCreate();
	if(socketHandle)
	{
        int32_t errorCode;
        struct sockaddr bindAddr;
        SockaddrInit(&bindAddr, AF_INET);
        SockaddrInSetPort(&bindAddr, port);
        SockaddrInSetAddrText(&bindAddr, addr);

        if ((errorCode = ProtoSSLBind(socketHandle, &bindAddr, sizeof(sockaddr))) == SOCKERR_NONE)
        {
            if((errorCode = ProtoSSLListen(socketHandle, 2))  == SOCKERR_NONE)
	        {
		        SocketInfoT* sockInfo = new SocketInfoT();
		        sockInfo->port = port;
		        sockInfo->socketHandle = socketHandle;
		        sockInfo->socketHandleClient = sockHandleClient;
		        sockInfo->errorCode = SOCKERR_NONE;
		        sockInfo->connected = false;  // we dont' try to send/recieve along our listener socket
                sockInfo->listening = true;
		        sockInfo->closeRequested = false;

		        mSocketInfoList.push_back(sockInfo);

		        return (EA::WebKit::SocketHandle)(socketHandle);
	        }
            else
            {
                ProtoSSLDestroy(socketHandle);
                EA_ASSERT_MSG(false, "SocketTransportHandlerDirtySDK::Listen failed ProtoSSLListen.\n");
            }
        }
        else
        {
            ProtoSSLDestroy(socketHandle);
            EA_ASSERT_MSG(false, "SocketTransportHandlerDirtySDK::Listen failed ProtoSSLBind.\n");
        }
	}
    else
    {
        EA_ASSERT_MSG(false, "SocketTransportHandlerDirtySDK::Listen failed ProtoSSLCreate.\n");
    }
	return NULL;
}

void SocketTransportHandlerDirtySDK::SetSocketHandleClient(EA::WebKit::SocketHandle socketHandle, EA::WebKit::SocketHandleClient* newSockHandleClient)
{
    SocketInfoT* sockInfo = GetSocketInfoT(socketHandle);
    if (sockInfo != NULL)
    {
        sockInfo->socketHandleClient = newSockHandleClient;
        return;
    }
    EA_ASSERT_MSG(false, "SocketTransportHandlerDirtySDK::SetSocketHandleClient failed, socketHandle not found.\n");
}

bool SocketTransportHandlerDirtySDK::CloseSocket(EA::WebKit::SocketHandle socketHandle)
{
	bool success = false;
	for(SocketInfoList::iterator it = mSocketInfoList.begin(); it != mSocketInfoList.end(); ++it)
	{
		SocketInfoT* sockInfo = *it;
		if(sockInfo->socketHandle == (ProtoSSLRefT*)(socketHandle))
		{
			// We mark the socket to be closed later because this can be requested by webcore while receiving data from each socket
			// iteratively in the ::Tick() call. If we delete instantaneouly, we invalidate our list.
			sockInfo->closeRequested = true;
			success = true;
			break;
		}
	}
	EA_ASSERT_MSG(success, "Unable to find the socket to close. Are you closing it multiple times?");
	return success;
}

int32_t SocketTransportHandlerDirtySDK::SendData(EA::WebKit::SocketHandle socketHandle, const char8_t* data, int32_t length)
{
	int errorCode = ProtoSSLSend((ProtoSSLRefT*)(socketHandle), data, length);
	if(errorCode >= SOCKERR_NONE)
	{
		return length;
	}
	else
	{
		// We don't fire the error event right back as WebKit expects us to do it separately.
		if(SocketInfoT* sockInfo = GetSocketInfoT(socketHandle))
		{
			sockInfo->errorCode = errorCode;
		}

		return 0;
	}
}
bool SocketTransportHandlerDirtySDK::Tick()
{
	SocketInfoList::iterator iter = mSocketInfoList.begin();
	while(iter != mSocketInfoList.end())
	{
		SocketInfoT* sockInfo = *iter;
		ProtoSSLUpdate(sockInfo->socketHandle);
		
		// If socket was not previously connected, check if it is now.
        if(!sockInfo->connected && !sockInfo->listening) // If we were not previously connected
		{
			if(ProtoSSLStat(sockInfo->socketHandle, DIRTY_STAT, NULL, 0) > 0) // and we are connected now
			{
				sockInfo->connected = true;
				sockInfo->socketHandleClient->SocketEvent(EA::WebKit::kSocketConnected);
			}
		}

        if(sockInfo->listening)
        {
            struct sockaddr connectionAddr;
            int32_t iAddrLen = sizeof(connectionAddr);
            ProtoSSLRefT *newSocket = NULL;
            if ((newSocket = ProtoSSLAccept(sockInfo->socketHandle, 0 /*unsecure*/, &connectionAddr, &iAddrLen)) != NULL)
            {
                //we have a new connection
                SocketInfoT* newSockInfo = new SocketInfoT();
                newSockInfo->port = SockaddrInGetPort(&connectionAddr);
		        newSockInfo->socketHandle = newSocket;
		        newSockInfo->socketHandleClient = NULL; //the server will have a chance to set the HandleClient for this socket in the ConnectionAccepted callback
		        newSockInfo->errorCode = SOCKERR_NONE;
		        newSockInfo->connected = true;
                newSockInfo->listening = false;
		        newSockInfo->closeRequested = false;
		        mSocketInfoList.push_back(newSockInfo);

                //inform the upper layers of a new connection
                sockInfo->socketHandleClient->ConnectionAccepted((EA::WebKit::SocketHandle)newSocket);
            }
        }


		// Receive any pending data
		if(sockInfo->connected)
		{
			char8_t recvBuffer[1024];
			int32_t bytesReceived = 0;
			while((bytesReceived = ProtoSSLRecv(sockInfo->socketHandle,recvBuffer,sizeof(recvBuffer))) > 0)
			{
                if (sockInfo->socketHandleClient != NULL)   //in some edge cases when closing a socket we may have cleared the socketHandleClient
                {
				    sockInfo->socketHandleClient->RecvedData(recvBuffer,bytesReceived);
                }
			}

			if(bytesReceived < SOCKERR_NONE)
			{
				// abaldeva: This assert may get fired if the remote host closes the connection. For now, keeping it enabled
				// since this code is fairly new. If this assert becomes an issue for your team, please notify.
				EA_ASSERT_FORMATTED(false, ("Error receiving data on socket. Error code - %d", bytesReceived));
				sockInfo->errorCode = bytesReceived;
			}
		}
		
		// Check if any of the processing so far has resulted in an error on client side or if webcore has requested a socket close. 
		if((sockInfo->errorCode < SOCKERR_NONE) || sockInfo->closeRequested)
		{
            if (sockInfo->socketHandleClient != NULL)
            {
			    if(sockInfo->closeRequested)
                {
				    sockInfo->socketHandleClient->SocketEvent(EA::WebKit::kSocketClose);
                }
                else
                {
				    sockInfo->socketHandleClient->SocketEvent(EA::WebKit::kSocketError);
                }
            }
			
			ProtoSSLDestroy(sockInfo->socketHandle);
			delete sockInfo;
			iter = mSocketInfoList.erase(iter);
		}
		else
		{
			++iter;
		}

	}

	return true;
}

SocketInfoT* SocketTransportHandlerDirtySDK::GetSocketInfoT(const EA::WebKit::SocketHandle socketHandle)
{
	for(SocketInfoList::iterator it = mSocketInfoList.begin(); it != mSocketInfoList.end(); ++it)
	{
		SocketInfoT* sockInfo = *it;
		if(sockInfo->socketHandle == (ProtoSSLRefT*)(socketHandle))
		{
			return sockInfo;
		}
	}

	return NULL;
}

}
}
#endif // ENABLE(DIRTYSDK_IN_DLL)


// Extra
/////////////////////////////////////////////////////////////////////////////
// How to POST with 'Content-Type: multipart/form-data' and 'Content-Type: application/x-www-form-urlencoded':
//     http://www.w3.org/TR/html401/interact/forms.html#submit-format
//
// Example POST using 'Content-Type: application/x-www-form-urlencoded':
//
//     POST /action HTTP/1.1
//     Host: www.blah.com
//     User-Agent: Mozilla/5.0 (X11; U; Linux i686; en-US; rv:1.4b) Gecko/20030516 Mozilla Firebird/0.6
//     Accept: text/xml,application/xml,application/xhtml+xml,text/html;q=0.9,text/plain;q=0.8,image/png,image/jpeg,image/gif;q=0.2,*/*;q=0.1
//     Connection: Close
//     Content-Type: application/x-www-form-urlencoded
//     Content-Length: 12 
//
//     var=12345678 
/////////////////////////////////////////////////////////////////////////////
