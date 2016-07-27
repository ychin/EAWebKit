/*
 * Copyright (C) 2006 Apple Computer, Inc.  All rights reserved.
 * Copyright (C) 2013 Apple Computer, Inc.  All rights reserved.
 *
 * Redistribution and use in source and binary forms, with or without
 * modification, are permitted provided that the following conditions
 * are met:
 * 1. Redistributions of source code must retain the above copyright
 *    notice, this list of conditions and the following disclaimer.
 * 2. Redistributions in binary form must reproduce the above copyright
 *    notice, this list of conditions and the following disclaimer in the
 *    documentation and/or other materials provided with the distribution.
 *
 * THIS SOFTWARE IS PROVIDED BY APPLE COMPUTER, INC. ``AS IS'' AND ANY
 * EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE
 * IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR
 * PURPOSE ARE DISCLAIMED.  IN NO EVENT SHALL APPLE COMPUTER, INC. OR
 * CONTRIBUTORS BE LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL,
 * EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT LIMITED TO,
 * PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES; LOSS OF USE, DATA, OR
 * PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND ON ANY THEORY
 * OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT
 * (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE
 * OF THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE. 
 */

#ifndef ResourceHandleClient_h
#define ResourceHandleClient_h

#include <wtf/PassRefPtr.h>

#if USE(CFNETWORK)
#include <CFNetwork/CFURLCachePriv.h>
#include <CFNetwork/CFURLResponsePriv.h>
#endif

#if PLATFORM(MAC)
OBJC_CLASS NSCachedURLResponse;
#endif

namespace WebCore {
    class AuthenticationChallenge;
    class Credential;
    class URL;
    class ProtectionSpace;
    class ResourceHandle;
    class ResourceError;
    class ResourceRequest;
    class ResourceResponse;
    class SharedBuffer;

    enum CacheStoragePolicy {
        StorageAllowed,
        StorageAllowedInMemoryOnly,
        StorageNotAllowed
    };
    
    class ResourceHandleClient {
    public:
        ResourceHandleClient();
        virtual ~ResourceHandleClient();

        // Request may be modified.
        virtual void willSendRequest(ResourceHandle*, ResourceRequest&, const ResourceResponse& /*redirectResponse*/) { }
        virtual void didSendData(ResourceHandle*, unsigned long long /*bytesSent*/, unsigned long long /*totalBytesToBeSent*/) { }

        virtual void didReceiveResponse(ResourceHandle*, const ResourceResponse&) { }
        
        virtual void didReceiveData(ResourceHandle*, const char*, int, int /*encodedDataLength*/) { }
        virtual void didReceiveBuffer(ResourceHandle*, PassRefPtr<SharedBuffer>, int encodedDataLength);
        
        virtual void didFinishLoading(ResourceHandle*, double /*finishTime*/) { }
        virtual void didFail(ResourceHandle*, const ResourceError&) { }
        virtual void wasBlocked(ResourceHandle*) { }
        virtual void cannotShowURL(ResourceHandle*) { }

        virtual bool usesAsyncCallbacks() { return false; }

        // Client will pass an updated request using ResourceHandle::continueWillSendRequest() when ready.
        virtual void willSendRequestAsync(ResourceHandle*, const ResourceRequest&, const ResourceResponse& redirectResponse);

        // Client will call ResourceHandle::continueDidReceiveResponse() when ready.
        virtual void didReceiveResponseAsync(ResourceHandle*, const ResourceResponse&);

        // Client will pass an updated request using ResourceHandle::continueShouldUseCredentialStorage() when ready.
        virtual void shouldUseCredentialStorageAsync(ResourceHandle*);
#if USE(PROTECTION_SPACE_AUTH_CALLBACK)
        // Client will pass an updated request using ResourceHandle::continueCanAuthenticateAgainstProtectionSpace() when ready.
        virtual void canAuthenticateAgainstProtectionSpaceAsync(ResourceHandle*, const ProtectionSpace&);
#endif
#if PLATFORM(MAC)
        // Client will pass an updated request using ResourceHandle::continueWillCacheResponse() when ready.
        virtual void willCacheResponseAsync(ResourceHandle*, NSCachedURLResponse *);
#endif

#if USE(NETWORK_CFDATA_ARRAY_CALLBACK)
        virtual bool supportsDataArray() { return false; }
        virtual void didReceiveDataArray(ResourceHandle*, CFArrayRef) { }
#endif

#if USE(SOUP)
        virtual char* getOrCreateReadBuffer(size_t /*requestedLength*/, size_t& /*actualLength*/) { return 0; }
#endif

   		//+EAWebKitChange
		// 03/08/2011 - Note by Arpit Baldeva: Added getResourceData() to ResourceHandleClient(Ported from our current port). This is used for the Disk Cache system. The problem is that the disk cache
		// system expects to write the entire file in 1 shot which is probably a good idea for the smaller web resources like .htm, .js, .jpeg etc (usually ranging
		// under 100k). The resource data travels from our transport system to the ResourceHandleClient(MainResourceLoader and SubResourceLoader derive from ResourceHandleClient).
		// The problem is that data is downloaded in chunks and we don't keep the data around ourselves, only ResourceHandleClient does. If we did, we won't need to add this accessor
		// here but we'd end up double buffering the data.
		// I looked at the other implementations of WebKit. From what I understood, the disk cache system is part of the transport layer in all the implementations and is
		// hidden. So it is not clear if they double buffer the data or write the data to the disk as soon as it is received. In any case, both these options are not suitable
		// given the memory constraints we have. I don't think following is ideal solution as it does not even fit nicely with the purpose of ResourceHandleClient but something
		// I can live with for the time being.
#if PLATFORM(EA) 
		virtual const SharedBuffer* getResourceData() const { return NULL; }
#endif
		//-EAWebKitChange

        virtual bool shouldUseCredentialStorage(ResourceHandle*) { return false; }
        virtual void didReceiveAuthenticationChallenge(ResourceHandle*, const AuthenticationChallenge&) { }
        virtual void didCancelAuthenticationChallenge(ResourceHandle*, const AuthenticationChallenge&) { }
#if USE(PROTECTION_SPACE_AUTH_CALLBACK)
        virtual bool canAuthenticateAgainstProtectionSpace(ResourceHandle*, const ProtectionSpace&) { return false; }
#endif
        virtual void receivedCancellation(ResourceHandle*, const AuthenticationChallenge&) { }

#if PLATFORM(MAC)
#if USE(CFNETWORK)
        virtual CFCachedURLResponseRef willCacheResponse(ResourceHandle*, CFCachedURLResponseRef response) { return response; }
#else
        virtual NSCachedURLResponse *willCacheResponse(ResourceHandle*, NSCachedURLResponse *response) { return response; }
#endif
        virtual void willStopBufferingData(ResourceHandle*, const char*, int) { }
#endif // PLATFORM(MAC)
#if PLATFORM(WIN) && USE(CFNETWORK)
        virtual bool shouldCacheResponse(ResourceHandle*, CFCachedURLResponseRef) { return true; }
#endif

    };

}

#endif
