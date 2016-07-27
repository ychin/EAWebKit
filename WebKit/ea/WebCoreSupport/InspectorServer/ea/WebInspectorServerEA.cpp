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

#include "WebInspectorServer.h" 
#include "WebPage.h"

#include <wtf/text/CString.h>
#include <wtf/text/StringBuilder.h>
#include <wtf/text/StringConcatenate.h>

using namespace WebCore;

namespace WebKit 
{

bool WebInspectorServer::platformResourceForPath(const String& path, Vector<char>& data, String& contentType)
{
    // The page list contains an unformated list of pages that can be inspected with a link to open a session.
    if (path == "/pagelist.json" || path == "/json") {
        buildPageList(data, contentType);
        return true;
    }
    return false;
}

void WebInspectorServer::buildPageList(Vector<char>& data, String& contentType)
{
    //stick with only this standard json formatting for now, though it doens't seem to really buy us anything
    //we may be able to generate a nicer html page for the user too

    // chromedevtools (http://code.google.com/p/chromedevtools) 0.3.8 expected JSON format:
    // {
    //  "title": "Foo",
    //  "url": "http://foo",
    //  "devtoolsFrontendUrl": "/Main.html?ws=localhost:9222/devtools/page/1",
    //  "webSocketDebuggerUrl": "ws://localhost:9222/devtools/page/1"
    // },

    StringBuilder builder;
    builder.appendLiteral("[ ");
    ClientMap::iterator end = m_clientMap.end();
    for (ClientMap::iterator it = m_clientMap.begin(); it != end; ++it) {
        //EA::WebKit::WebPage* webPage = it->value;
        if (it != m_clientMap.begin())
            builder.appendLiteral(", ");
        builder.appendLiteral("{ \"id\": ");
        builder.appendNumber(it->key);
        builder.appendLiteral(", \"title\": \"");
        builder.append("");       //webPage->pageLoadState().title()
        builder.appendLiteral("\", \"url\": \"");
        builder.append("");   //webPage->pageLoadState().activeURL()
        builder.appendLiteral("\", \"inspectorUrl\": \"");
        builder.appendLiteral("/Main.html?page=");
        builder.appendNumber(it->key);
        builder.appendLiteral("\", \"devtoolsFrontendUrl\": \"");
        builder.appendLiteral("/Main.html?ws=");
        builder.append(bindAddress());
        builder.appendLiteral(":");
        builder.appendNumber(port());
        builder.appendLiteral("/devtools/page/");
        builder.appendNumber(it->key);
        builder.appendLiteral("\", \"webSocketDebuggerUrl\": \"");
        builder.appendLiteral("ws://");
        builder.append(bindAddress());
        builder.appendLiteral(":");
        builder.appendNumber(port());
        builder.appendLiteral("/devtools/page/");
        builder.appendNumber(it->key);
        builder.appendLiteral("\" }");
    }
    builder.appendLiteral(" ]");
    CString cstr = builder.toString().utf8();
    data.append(cstr.data(), cstr.length());
    contentType = "application/json; charset=utf-8";
}


} //namespace WebKit

#endif //ENABLE(INSPECTOR_SERVER)
