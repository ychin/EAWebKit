/*
    Copyright (C) 2008 Nokia Corporation and/or its subsidiary(-ies)
	Copyright (C) 2011, 2013, 2014 Electronic Arts, Inc. All rights reserved.

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
#include "EventLoop.h"
#include <EAWebkit/EAWebkitClient.h>
#include "WebFrame_p.h"
#include "WebPage.h"
#include "Page.h"
#include <EAWebkit/EAWebKitView.h>
#include "WebInspector.h"
#include "JSDomWindowCustom.h"

namespace WebCore {

// Note by Arpit Baldeva: Seems like the sole purpose of this function is to be able to process events when the script has hit a break point
// in the web inspector.

void EventLoop::cycle(Page* page)
{
	if (page == NULL)
	{
		return;
	}

	EA::WebKit::WebPage *eaPage = EA::WebKit::WebFramePrivate::kit(page->mainFrame())->page();
	EA::WebKit::View *view = eaPage->view();
	
	if (EA::WebKit::EAWebKitClient *pClient = EA::WebKit::GetEAWebKitClient(view))
    {
        EA::WebKit::ProcessEventsInfo info;
        info.mpView = view;
        info.mpUserData = view->GetUserData();
        if (eaPage->GetInspector()->GetFrontend() != NULL)
        {
            info.mpInspectorView = eaPage->GetInspector()->GetFrontend();
            info.mpInspectorUserData = info.mpInspectorView->GetUserData();
        }
        else
        {
            info.mpInspectorView = NULL;
            info.mpInspectorUserData = NULL;
        }

        view->SetBeingDebugged(true);
        pClient->ProcessEvents(info);
        view->SetBeingDebugged(false);
    }
}

} // namespace WebCore
