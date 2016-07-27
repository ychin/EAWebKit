/*
Copyright (C) 2014 Electronic Arts, Inc.  All rights reserved.

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


#include "config.h"
#if ENABLE(INSPECTOR_SERVER)
#include "WebSocketServer.h"
#include "WebInspectorServer.h" 
#include "WebSocketServerConnection.h"
#include "SocketStreamHandle.h"
#include "Logging.h"
#include <internal/include/EAWebKit_p.h>

using namespace WebCore;

namespace WebKit 
{

void WebSocketServer::RecvedData(const char8_t* data, int32_t length)
{
    //the listener shouldnt recieve data so we'll ignore it
}

void WebSocketServer::SocketEvent(EA::WebKit::SocketEventType eventType)
{
    //event types; kSocketConnected, kSocketClose, kSocketError
    //do nothing, normal cleanup flows should be fine
}

void WebSocketServer::ConnectionAccepted(EA::WebKit::SocketHandle newClient)
{
    //this will hand off the newClient to be managed by SocketStreamHandle callbacks
    OwnPtr<WebSocketServerConnection> webSocketConnection = adoptPtr(new WebSocketServerConnection(this->client(), this));
    webSocketConnection->setSocketHandle(SocketStreamHandle::create(newClient, webSocketConnection.get()));
    this->didAcceptConnection(webSocketConnection.release());
}

void WebSocketServer::platformInitialize()
{
    mListenerSocketHandle = NULL;
}

bool WebSocketServer::platformListen(const String& bindAddress, unsigned short port)
{
    ASSERT(EA::WebKit::GetSocketTransportHandler() != NULL);
    ASSERT(mListenerSocketHandle == NULL);
    mListenerSocketHandle = EA::WebKit::GetSocketTransportHandler()->Listen((const char *)bindAddress.charactersWithNullTermination().data(), port, this);
    
    if (mListenerSocketHandle != NULL)
    {
        LOG(Network, "Remote web inspector is listening on port %d\n", port);
        return true;
    }
    else
    {
        LOG(Network, "Remote web inspector failed listening on port %d\n", port);
        return false;
    }
}

void WebSocketServer::platformClose()
{
    ASSERT(EA::WebKit::GetSocketTransportHandler() != NULL);
    ASSERT(mListenerSocketHandle != NULL);
    EA::WebKit::GetSocketTransportHandler()->CloseSocket(mListenerSocketHandle);
    mListenerSocketHandle = NULL;
    LOG(Network, "Remote web inspector has stopped listening\n");
}


} //namespace WebKit

#endif // ENABLE(INSPECTOR_SERVER)
