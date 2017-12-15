/*
 * Copyright (C) 2010 Nokia Inc. All rights reserved.
 * Copyright (C) 2011, 2012, 2014 Electronic Arts, Inc. All rights reserved.
 *
 * Redistribution and use in source and binary forms, with or without
 * modification, are permitted provided that the following conditions are
 * met:
 *
 *     * Redistributions of source code must retain the above copyright
 * notice, this list of conditions and the following disclaimer.
 *     * Redistributions in binary form must reproduce the above
 * copyright notice, this list of conditions and the following disclaimer
 * in the documentation and/or other materials provided with the
 * distribution.
 *     * Neither the name of Google Inc. nor the names of its
 * contributors may be used to endorse or promote products derived from
 * this software without specific prior written permission.
 *
 * THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS
 * "AS IS" AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT
 * LIMITED TO, THE IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR
 * A PARTICULAR PURPOSE ARE DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT
 * OWNER OR CONTRIBUTORS BE LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL,
 * SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT
 * LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES; LOSS OF USE,
 * DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND ON ANY
 * THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT
 * (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE
 * OF THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
 */

#ifndef SocketStreamHandlePrivate_h
#define SocketStreamHandlePrivate_h

#include "SocketStreamHandleBase.h"
#include "Timer.h"

#include <EAWebKit/EAWebKitTransport.h>
namespace WebCore 
{
class SocketStreamHandlePrivate : public EA::WebKit::SocketHandleClient
{
public:
	SocketStreamHandlePrivate(SocketStreamHandle*, const URL&);
    SocketStreamHandlePrivate(SocketStreamHandle* streamHandle, EA::WebKit::SocketHandle socketHandle);
	~SocketStreamHandlePrivate();

	// SocketHandleClient Interface
	virtual void RecvedData(const char8_t* data, int32_t length);
	virtual void SocketEvent(EA::WebKit::SocketEventType eventType);

	//Class members
	int32_t Send(const char8_t* data, int32_t length);
	void Close();

private:
	void timerFired(WebCore::Timer<SocketStreamHandlePrivate>*);
	
	SocketStreamHandle* m_streamHandle;
	EA::WebKit::SocketHandle m_socketHandle;
	EA::WebKit::SocketHandle m_CleanupTansportHandlerSocketHandle; // same as m_socketHandle, but when we set our m_socketHandle to NULL then we can't actually change the TransportHandler for it anymore, so we keep track of it and retain the original logic
	WebCore::Timer<SocketStreamHandlePrivate>   mPendingDataTimer;    // Timer to trigger send operation for pending data.    
};

}
#endif //SocketStreamHandlePrivate_h
