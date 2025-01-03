/*
 * Copyright (C) 2015 Electronic Arts, Inc. All rights reserved.
 *
 * All rights reserved.
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


#include "config.h"
#include "ProgressTrackerClientEA.h"
#include "ProgressTracker.h"
#include "WebPageClientEA.h"
#include "WebPage.h"
#include "Frame.h"
#include "WebFrame.h"
#include "WebFrame_p.h"
#include "Page.h"

#include <internal/include/EAWebKit_p.h>

#include <EAWebKit/EAWebKitView.h>
#include <EAWebKit/EAWebKitClient.h>
#include <internal/include/EAWebkitEASTLHelpers.h>
#include <EAWebKit/EAWebKitInput.h>
#include <internal/include/EAWebKitAssert.h>

# define EVENT_TYPE_STR(type) #type
namespace
{
	inline bool isMainFrame(const WebCore::Frame* frame)
	{
		// abaldeva: Seems like page can be detached from a frame through javascript so we need to be careful about accessing it.
		// Otherwise, it crashed accessing http://www.w3schools.com/Dom/dom_nodetype.asp 
		if (frame->page())
			return frame == &(frame->page()->mainFrame());

		return false;
	}
}
namespace WebCore
{
	ProgressTrackerClientEA::ProgressTrackerClientEA(EA::WebKit::WebPage *webPage)
		: m_webPage(webPage)
	{
	}

	ProgressTrackerClientEA::~ProgressTrackerClientEA()
	{
	}

	void ProgressTrackerClientEA::progressStarted(WebCore::Frame& frame)
	{
		using namespace EA::WebKit;
		if (m_webPage->mainFrame() && frame.page())
		{
	        // A new load starts, so lets clear the previous error.
	        m_loadError = ResourceError();
			m_webPage->mainFrame()->SetLoadState(WebFrame::kLoadStarted);
			if(isMainFrame(&frame)) 
			{
				if (EAWebKitClient* const pClient = GetEAWebKitClient(m_webPage->mainFrame()->page()->view()))
				{
					LoadInfo loadInfo(m_webPage->mainFrame()->page()->view(), m_webPage->mainFrame()->page()->view()->GetUserData(), kLETLoadStarted);
					loadInfo.mProgressEstimation = 0.0f;
					EAW_LOG(3, "Event %s : Sent a request to server for page load", EVENT_TYPE_STR(kLETLoadStarted));
					pClient->LoadUpdate(loadInfo);
				}
			}
			progressEstimateChanged(frame); // Passing in a dummy parameter (Function definition doesn't use it)
	    }
	
		if (frame.tree().parent() || !m_webPage->mainFrame())// abaldeva: Is m_frame->tree().parent() trying to detect if it is a subframe?
	        return;
	   
		m_webPage->mainFrame()->page()->d->updateNavigationActions();
	}
	
	void ProgressTrackerClientEA::progressEstimateChanged(WebCore::Frame& frame)
	{
		if (m_webPage->mainFrame() && frame.page())
		{
			using namespace EA::WebKit;
			
			m_webPage->mainFrame()->SetLoadState(WebFrame::kLoadInProgress);
			if (EAWebKitClient* const pClient = GetEAWebKitClient(m_webPage->mainFrame()->page()->view()))
			{
				LoadInfo loadInfo(m_webPage->mainFrame()->page()->view(), m_webPage->mainFrame()->page()->view()->GetUserData(), kLETLoadProgressUpdate);
				loadInfo.mProgressEstimation = frame.page()->progress().estimatedProgress();
				EAW_LOG(3, "Event %s : Page load in progress with estimate = %lf", EVENT_TYPE_STR(kLETLoadProgressUpdate) , loadInfo.mProgressEstimation );
				pClient->LoadUpdate(loadInfo);
			}
		}
	}
	
	void ProgressTrackerClientEA::progressFinished(WebCore::Frame& frame)
	{
	    // send a mousemove event to
	    // (1) update the cursor to change according to whatever is underneath the mouse cursor right now
	    // (2) display the tool tip if the mouse hovers a node which has a tool tip
		// Last port, we had a specific call to ResetToolTip in the page load start. Following should get rid
		// of that. Verify.
	
		/* We don't use this as it is non reliable and can be trigger multiple times. Instead, we use ::dispatchDidFinishLoad().
		Reference - http://old.nabble.com/FrameLoaderClient-notifications-tt16025970.html
		*/
	}


}

