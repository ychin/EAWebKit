/*
 * Copyright (C) 2007 Apple Inc.  All rights reserved.
 * Copyright (C) 2008 Nokia Corporation and/or its subsidiary(-ies)
 * Copyright (C) 2008 Holger Hans Peter Freyther
 * Copyright (C) 2011, 2012 Electronic Arts, Inc. All rights reserved.
 *
 * Redistribution and use in source and binary forms, with or without
 * modification, are permitted provided that the following conditions
 * are met:
 *
 * 1.  Redistributions of source code must retain the above copyright
 *     notice, this list of conditions and the following disclaimer. 
 * 2.  Redistributions in binary form must reproduce the above copyright
 *     notice, this list of conditions and the following disclaimer in the
 *     documentation and/or other materials provided with the distribution. 
 * 3.  Neither the name of Apple Computer, Inc. ("Apple") nor the names of
 *     its contributors may be used to endorse or promote products derived
 *     from this software without specific prior written permission. 
 *
 * THIS SOFTWARE IS PROVIDED BY APPLE AND ITS CONTRIBUTORS "AS IS" AND ANY
 * EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE IMPLIED
 * WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE ARE
 * DISCLAIMED. IN NO EVENT SHALL APPLE OR ITS CONTRIBUTORS BE LIABLE FOR ANY
 * DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES
 * (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES;
 * LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND
 * ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT
 * (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE OF
 * THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
 */

#include "config.h"
#include "PlatformScreen.h"

#include "FloatRect.h"
#include "Frame.h"
#include "FrameView.h"
#include "HostWindow.h"
#include "Widget.h"
#include "WebPageClientEA.h"

// Note by Arpit Baldeva: JavaScript has a window.screen object and methods below implement that API. The should be sufficient
// for our usage. If a more accurate implementation seems necessary later on, we'll need to query following using the 
// EAWebKitClient mechanism.

namespace WebCore {
/*
static int screenNumber(Widget* w)
{
    if (!w)
        return 0;

    WebPageClient* client = w->root()->hostWindow()->platformPageClient();
    return client ? client->screenNumber() : 0;
}
*/
int screenDepth(Widget* w)
{
	return 4 * 8; //From 1.x, bytes per pixel * 8.
}

int screenDepthPerComponent(Widget* w)
{
	return 8; // We assume A8R8G8B8 (or similar)
}

bool screenIsMonochrome(Widget* w)
{
    return false; 
}

FloatRect screenRect(Widget* w)
{
	return w->frameRect(); // For our usage, screenRect is same as our surface size.
}

FloatRect screenAvailableRect(Widget* w)
{
	// Update 10/14/2010 - Note by Arpit Baldeva: We should simply return our screenRect(Which is View surface for us) here
	// as the entire surface can be used for display purpose. The Windows port of WebKit returns MONITORINFOEX.rcWork here 
	// and MONITORINFOEX.rcMonitor for screenRect.
	// The difference between two is explained at http://msdn.microsoft.com/en-us/library/dd145066(VS.85).aspx.  
	return screenRect(w);
}

} // namespace WebCore
