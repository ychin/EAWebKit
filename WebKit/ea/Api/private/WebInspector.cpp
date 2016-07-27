/*
    Copyright (C) 2009 Nokia Corporation and/or its subsidiary(-ies)
	Copyright (C) 2011, 2012 Electronic Arts, Inc. All rights reserved.

    This library is free software; you can redistribute it and/or
    modify it under the terms of the GNU Library General Public
    License as published by the Free Software Foundation; either
    version 2 of the License, or (at your option) any later version.

    This library is distributed in the hope that it will be useful,
    but WITHOUT ANY WARRANTY; without even the implied warranty of
    MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
    Library General Public License for more details.

    You should have received a copy of the GNU Library General Public License
    along with this library; see the file COPYING.LIB.  If not, write to
    the Free Software Foundation, Inc., 51 Franklin Street, Fifth Floor,
    Boston, MA 02110-1301, USA.
*/

#include "config.h"
#include "WebInspector.h"
#include "InspectorClientEA.h"

namespace EA { namespace WebKit {

WebInspector::WebInspector(void)
: mFrontendView(NULL)
, mFrontendClient(NULL)
{
    // Do nothing.
}

WebInspector::~WebInspector(void)
{
    // Do nothing.
}

void WebInspector::SetFrontend(View *frontendView)
{
    mFrontendView = frontendView;
}

View *WebInspector::GetFrontend(void)
{
    return mFrontendView;
}

void WebInspector::SetFrontendClient(WebCore::InspectorFrontendClientEA *client)
{
    mFrontendClient = client;
}

void WebInspector::Shutdown(void)
{
    WebCore::InspectorFrontendClientEA *pFrontendClient = mFrontendClient;
	// set these before and not after calling disconnectFromBackend as that call might actually destroy and deallocate this
	mFrontendClient = NULL;
	mFrontendView = NULL;

	if (pFrontendClient)
	{
		pFrontendClient->disconnectFromBackend();
	}
}

}}
