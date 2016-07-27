/*
Copyright (C) 2008, 2009, 2010, 2011, 2012, 2014 Electronic Arts, Inc.  All rights reserved.

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
// DirtySDK based Tx Handler Implementation
// Created by Paul Pedriana
// Maintained by EAWebKit Team
/////////////////////////////////////////////////////////////////////////////

#ifndef TransportHandlerDirtySDKEA_h
#define TransportHandlerDirtySDKEA_h

#include <EAWebKit/EAWebKitTransport.h>
#include <EAWebKit/EAWebkitAllocator.h>

#include <eastl/fixed_string.h>
#include <eastl/fixed_map.h>

namespace EA
{
namespace WebKit
{
	class IStreamDecompressor;
	class EASTLAllocator;
}
}
namespace EA
{
namespace TransportHelper
{
	const int32_t kSharedDecompressionBufferSize = 16384;
	typedef eastl::fixed_string<char16_t, 64, true, EA::WebKit::EASTLAllocator> TransportString16;
	typedef eastl::fixed_string<char8_t, 64, true,  EA::WebKit::EASTLAllocator> TransportString8;

	// Used for HTTP header entries.
	struct fstr_iless : public eastl::binary_function<TransportString16, TransportString16, bool>
	{
		bool operator()(const TransportString16& a, const TransportString16& b) const { return (a.comparei(b) < 0); }
	};

	// Used for HeaderMap::find_as() calls.
	struct str_iless : public eastl::binary_function<TransportString16, const char16_t*, bool>
	{
		bool operator()(const TransportString16& a, const char16_t*      b) const { return (a.comparei(b) < 0); }
		bool operator()(const char16_t*      b, const TransportString16& a) const { return (a.comparei(b) > 0); }
	};
	typedef eastl::fixed_multimap<TransportString16, TransportString16, 8, true, fstr_iless,EA::WebKit::EASTLAllocator> TransportHeaderMap;

}
}

#include "protohttpmanager.h"
#include "dirtyvers.h" // Defines (e.g.) #define DIRTYVERS (0x07000000)


namespace EA
{
namespace WebKit
{
	
class TransportHandlerDirtySDK : public EA::WebKit::TransportHandler
{
public:
    TransportHandlerDirtySDK();
   ~TransportHandlerDirtySDK();

    bool Init           (const char16_t* pScheme);
    bool Shutdown       (const char16_t* pScheme);
    
	bool InitJob        (EA::WebKit::TransportInfo* pTInfo, bool& bStateComplete);
	bool Connect        (EA::WebKit::TransportInfo* pTInfo, bool& bStateComplete);
	bool Transfer       (EA::WebKit::TransportInfo* pTInfo, bool& bStateComplete);
	bool Disconnect     (EA::WebKit::TransportInfo* pTInfo, bool& bStateComplete);
	bool ShutdownJob    (EA::WebKit::TransportInfo* pTInfo, bool& bStateComplete);
    
	bool CanCacheToDisk ();  
	bool Tick			();

#if DIRTYVERS > 0x07050300 
	static int32_t DirtySDKSendHeaderCallbackStatic(ProtoHttpRefT* pState, char* pHeader, uint32_t uHeaderSize, const char* pData, uint32_t uDataLen, void* pUserRef);
#else
	static void DirtySDKSendHeaderCallbackStatic(ProtoHttpRefT* pState, char* pHeader, uint32_t uHeaderSize, const char* pData, uint32_t uDataLen, void* pUserRef);
#endif
	int32_t        DirtySDKSendHeaderCallback(ProtoHttpRefT* pState, char* pHeader, uint32_t uHeaderSize, const char* pData, uint32_t uDataLen, EA::WebKit::TransportInfo* pTInfo);

#if DIRTYVERS > 0x07050300
	static int32_t DirtySDKRecvHeaderCallbackStatic(ProtoHttpRefT* pState, const char* pHeader, uint32_t uHeaderSize, void* pUserRef);
#else
	static void DirtySDKRecvHeaderCallbackStatic(ProtoHttpRefT* pState, const char* pHeader, uint32_t uHeaderSize, void* pUserRef);
#endif
	void        DirtySDKRecvHeaderCallback(ProtoHttpRefT* pState, const char* pHeader, uint32_t uHeaderSize, EA::WebKit::TransportInfo* pTInfo);

private:
    
	struct DirtySDKInfo  // Our info for a given job.
	{
		int32_t									mHttpHandle;           // Handle for current HTTP transaction
		EA::TransportHelper::TransportString8	mURI;
		bool									mbHeadersReceived;
		int										mSendIndex;            // If we receive 300-family redirects then DirtySDK will resend requests for the new URL. We keep track of this.

		EA::TransportHelper::TransportString8	mPostData;             // String which holds all POST data.
		bool									mbPostActive;          // Deprecated unless/until we go back to using chunked data. True if we are writing POST data. When we are done writing the POST data, we set this to false and go into receive mode as if we were doing GET.
		char									mPostBuffer[512];      // Deprecated unless/until we go back to using chunked data.
		int64_t									mPostBufferSize;       // Deprecated unless/until we go back to using chunked data. 
		int64_t									mPostBufferPosition;   // Deprecated unless/until we go back to using chunked data.
		IStreamDecompressor*					mStreamDecompressor;
		DirtySDKInfo();
		~DirtySDKInfo();

	};
	
	bool SetupHttpManager(HttpManagerRefT*& pHttpManager, bool syncJobs = false);
	void SetupStreamDecompressor(EA::WebKit::TransportInfo* pTInfo);

    HttpManagerRefT* mpHttpManager;
	HttpManagerRefT* mpHttpManagerSyncJobs; //We create a Http Manager instead of a new ref each time. This keeps code clean and probably also more efficient if lot of requests are made over the time.
	uint8_t* mSharedDecompressionBuffer;
	int mActiveSchemesCount;

    
#ifdef _DEBUG
	int mJobCount;  // Used to verify jobs are all shut down and not leaked.
#endif
	bool mIsDirtySockStartedHere;

};


} 
} 

struct ProtoSSLRefT;

#include <eastl/list.h>
namespace EA
{
namespace WebKit
{
struct SocketInfoT
{
struct ProtoSSLRefT*			socketHandle;
uint32_t						port;
EA::WebKit::SocketHandleClient* socketHandleClient;
int32_t							errorCode;
bool							connected;
bool                            listening;
bool							closeRequested;
};

class SocketTransportHandlerDirtySDK : public EA::WebKit::SocketTransportHandler
{
public:
	SocketTransportHandlerDirtySDK();
	virtual ~SocketTransportHandlerDirtySDK();

	bool Init();
	bool Shutdown();

	EA::WebKit::SocketHandle OpenSocket(const char8_t* hostUrl, uint32_t port, EA::WebKit::SocketType sockType, EA::WebKit::SocketHandleClient* sockHandleClient);
	EA::WebKit::SocketHandle Listen(const char8_t* hostUrl, uint32_t port, EA::WebKit::SocketHandleClient* sockHandleClient);
    void SetSocketHandleClient(EA::WebKit::SocketHandle socketHandle, EA::WebKit::SocketHandleClient* sockHandleClient);
	bool CloseSocket(EA::WebKit::SocketHandle socketHandle);
	int32_t SendData(EA::WebKit::SocketHandle socketHandle, const char8_t* data, int32_t length);

	bool Tick ();
private:
	typedef eastl::list<SocketInfoT*, EA::WebKit::EASTLAllocator> SocketInfoList;
	SocketInfoT* GetSocketInfoT(const EA::WebKit::SocketHandle socketHandle);

	SocketInfoList mSocketInfoList;
};
}
}



#endif // Header include guard
