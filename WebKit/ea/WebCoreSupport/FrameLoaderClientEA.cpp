/*
 * Copyright (C) 2006 Zack Rusin <zack@kde.org>
 * Copyright (C) 2006, 2011 Apple Inc. All rights reserved.
 * Copyright (C) 2009 Nokia Corporation and/or its subsidiary(-ies)
 * Copyright (C) 2008 Collabora Ltd. All rights reserved.
 * Coypright (C) 2008 Holger Hans Peter Freyther
 * Coypright (C) 2009, 2010 Girish Ramakrishnan <girish@forwardbias.in>
 * Copyright (C) 2011, 2012, 2013, 2014 Electronic Arts, Inc. All rights reserved.
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
#include "FrameLoaderClientEA.h"
#include "CSSComputedStyleDeclaration.h"
#include "CSSPropertyNames.h"
#include "DocumentLoader.h"
#include "EventHandler.h"
#include "FormState.h"
#include "FrameLoadRequest.h"
#include "FrameNetworkingContextEA.h"
#include "FrameTree.h"
#include "FrameView.h"
#include "HTMLAppletElement.h"
#include "HTMLFormElement.h"
#include "HTMLPlugInElement.h"
#include "HTTPParsers.h"
#include "HTTPStatusCodes.h"
#include "HistoryItem.h"
#include "HitTestResult.h"
#include "JSDOMWindowBase.h"
#include "MIMETypeRegistry.h"
#include "MainFrame.h"
#include "MouseEvent.h"
#include "NotImplemented.h"
#include "Page.h"
#include "PlatformMouseEvent.h"
#include "PluginData.h"
#include "PluginDatabase.h"
#include "PolicyChecker.h"
#include "ProgressTracker.h"
#include "ResourceRequest.h"
#include "HistoryItem.h"
#include "HTMLAppletElement.h"
#include "HTMLFormElement.h"
#include "HTMLPlugInElement.h"
#include "HTTPParsers.h"
#include "NotImplemented.h"
#include "ResourceHandleInternal.h"
#include "ResourceHandle.h"
#include "ScriptController.h"
#include "Settings.h"
#include "WebPageClientEA.h"
#include "ViewportArguments.h"
#include "NodeList.h"

#include "webpage.h"
#include "webpage_p.h"
#include "webframe.h"
#include "webframe_p.h"

#include <internal/include/EAWebKit_p.h>

#include <EAWebKit/EAWebKitView.h>
#include <EAWebKit/EAWebKitClient.h>
#include <internal/include/EAWebkitEASTLHelpers.h>
#include <EAWebKit/EAWebKitInput.h>
#include <internal/include/EAWebKitAssert.h>

// Note by Arpit Baldeva - 
// The frame load events in WebCore are pretty complex, some times non-intuitive. A discussion about some of them can be read
// at http://old.nabble.com/FrameLoaderClient-notifications-tt16025970.html
//
# define EVENT_TYPE_STR(type) #type
namespace EA
{
namespace WebKit
{
LoadInfo::LoadInfo(View* pView, void* pUserData, LoadEventType let)
	: mpView(pView)
	, mpUserData(pUserData)
	, mProgressEstimation(0.0f)
	, mLoadEventType(let)
	, mLoadErrorType(EA::WebKit::kLETNoError)
	, mStatusCode(0)
{

}

}
}

namespace 
{
inline bool isMainFrame(const WebCore::Frame* frame)
{
	// abaldeva: Seems like page can be detached from a frame through javascript so we need to be careful about accessing it.
	// Otherwise, it crashed accessing http://www.w3schools.com/Dom/dom_nodetype.asp 
	if(frame->page())
		return frame == &(frame->page()->mainFrame());
	
	return false;
}
}
namespace WebCore
{
bool FrameLoaderClientEA::deferMainResourceDataLoad = true;
bool FrameLoaderClientEA::policyDelegateEnabled = false;
bool FrameLoaderClientEA::policyDelegatePermissive = false;
/*
bool FrameLoaderClientEA::dumpFrameLoaderCallbacks = false;
bool FrameLoaderClientEA::dumpUserGestureInFrameLoaderCallbacks = false;
bool FrameLoaderClientEA::dumpResourceLoadCallbacks = false;
bool FrameLoaderClientEA::sendRequestReturnsNullOnRedirect = false;
bool FrameLoaderClientEA::sendRequestReturnsNull = false;
bool FrameLoaderClientEA::dumpResourceResponseMIMETypes = false;
bool FrameLoaderClientEA::dumpHistoryCallbacks = false;

bool FrameLoaderClientEA::policyDelegateEnabled = false;
bool FrameLoaderClientEA::policyDelegatePermissive = false;
*/
// Taken from DumpRenderTree/chromium/WebViewHost.cpp
// static const char* navigationTypeToString(NavigationType type)
// {
//     switch (type) {
//     case NavigationTypeLinkClicked:
//         return "link clicked";
//     case NavigationTypeFormSubmitted:
//         return "form submitted";
//     case NavigationTypeBackForward:
//         return "back/forward";
//     case NavigationTypeReload:
//         return "reload";
//     case NavigationTypeFormResubmitted:
//         return "form resubmitted";
//     case NavigationTypeOther:
//         return "other";
//     }
//     return "illegal value";
// }

FrameLoaderClientEA::FrameLoaderClientEA()
    : m_frame(0)
    , m_webFrame(0)
    , m_pluginView(0)
    , m_hasSentResponseToPlugin(false)
    , m_hasRepresentation(false)
    , m_loadError(ResourceError())
{
}


FrameLoaderClientEA::~FrameLoaderClientEA()
{
}

void FrameLoaderClientEA::setFrame(EA::WebKit::WebFrame* webFrame, Frame* frame)
{
    m_webFrame = webFrame;
    m_frame = frame;

    if (!m_webFrame || !m_webFrame->page()) {
        EAW_ASSERT_MSG(false, "FrameLoaderClientEA::setFrame frame without Page!");
        return;
    }
}

void FrameLoaderClientEA::callPolicyFunction(FramePolicyFunction function, PolicyAction action)
{
	function(action);
}

bool FrameLoaderClientEA::hasWebView() const
{
   return true;
}

void FrameLoaderClientEA::savePlatformDataToCachedFrame(CachedFrame*) 
{
    notImplemented();
}

void FrameLoaderClientEA::transitionToCommittedFromCachedFrame(CachedFrame*)
{
}

// So once the page is committed(some data is received), a 
// new FrameView is created for this frame.
// When the View is created, a default set of steps are executed(See FrameLoader::Init()) that cause following to 
// trigger(Without needing to load a URL). That results in initial background clearing.
void FrameLoaderClientEA::transitionToCommittedForNewPage()
{
    ASSERT(m_frame);
    ASSERT(m_webFrame);

    EA::WebKit::WebPage* page = m_webFrame->page();
    WebCore::Color backgroundColor(page->view()->GetBackgroundColor());
    m_webFrame->ClearDisplay(backgroundColor);

    //const IntSize preferredLayoutSize = page->preferredContentsSize();


    // abaldeva: For example, We may use this for disabling scrolling.
	ScrollbarMode hScrollbar = (ScrollbarMode) m_webFrame->scrollBarPolicy(EA::WebKit::ScrollbarHorizontal);
	ScrollbarMode vScrollbar = (ScrollbarMode) m_webFrame->scrollBarPolicy(EA::WebKit::ScrollbarVertical);
    bool hLock = hScrollbar != ScrollbarAuto;
    bool vLock = vScrollbar != ScrollbarAuto;

    // The HistoryController will update the scroll position later if needed.
    m_frame->createView(m_webFrame->page()->viewportSize(),
                        backgroundColor, 
                        
                        // Webkit assumes that if you pass true here it doesn't need to clear the background with the clear color.
                        // Since we're still rendering to a texture even when transparent we always want the background clear to happen.
                        // Therefore, we always tell webkit the background is opaque even if we have a transparent clear color.
                        false,
                       IntSize(),
					   IntRect(),
                        false,
                        hScrollbar, hLock,
                        vScrollbar, vLock);

    if (isMainFrame(m_frame) && page->d->client)
	{
		bool resizesToContents = page->d->client->viewResizesToContentsEnabled();

		m_frame->view()->setPaintsEntireContents(resizesToContents);
		m_frame->view()->setDelegatesScrolling(resizesToContents);
		
		
		
		
	}
}

void FrameLoaderClientEA::didSaveToPageCache(void)
{
	// Do nothing.
}

void FrameLoaderClientEA::didRestoreFromPageCache(void)
{
	// Do nothing.
}

void FrameLoaderClientEA::dispatchDidBecomeFrameset(bool)
{
}



void FrameLoaderClientEA::forceLayout()
{
    FrameView* view = m_frame->view();
    if (view)
        view->layout(true);
}


void FrameLoaderClientEA::forceLayoutForNonHTML()
{
}


void FrameLoaderClientEA::setCopiesOnScroll()
{
    // apparently mac specific
}


void FrameLoaderClientEA::detachedFromParent2()
{
}


void FrameLoaderClientEA::detachedFromParent3()
{
}

void FrameLoaderClientEA::dispatchDidHandleOnloadEvents()
{
	if(isMainFrame(m_frame))
	{
		using namespace EA::WebKit;
		if(EAWebKitClient* const pClient = GetEAWebKitClient(m_webFrame->page()->view()))
		{
			LoadInfo loadInfo(m_webFrame->page()->view(),m_webFrame->page()->view()->GetUserData(), kLETOnLoadEventsHandled);
			EAW_LOG(3, "Event %s : The Onload events have been handled", EVENT_TYPE_STR(kLETOnLoadEventsHandled));
			pClient->LoadUpdate(loadInfo);
		}

	}
}

// abaldeva: This is called if the server you are trying to reach redirects you to another location. We don't send the redirect
// event from here as a client may reject to redirect itself. The redirect event is sent in ::dispatchWillSendRequest function.
void FrameLoaderClientEA::dispatchDidReceiveServerRedirectForProvisionalLoad()
{
    notImplemented();
}

void FrameLoaderClientEA::dispatchDidCancelClientRedirect()
{
    notImplemented();
}


void FrameLoaderClientEA::dispatchWillPerformClientRedirect(const URL& url, double, double)
{
    notImplemented();
}

void FrameLoaderClientEA::dispatchDidNavigateWithinPage()
{
    if (!m_webFrame)
        return;

    FrameLoader& loader = m_frame->loader();
    bool loaderCompleted = !(loader.activeDocumentLoader() && loader.activeDocumentLoader()->isLoadingInAPISense());

    if (!loaderCompleted)
        return;

    dispatchDidCommitLoad();
    dispatchDidFinishLoad();
}

// abaldeva: This is called so that you can correctly change the "URL bar of your browser GUI" on pages with named anchor.
// Example - http://www.htmlcodetutorial.com/linking/anameexample.html
void FrameLoaderClientEA::dispatchDidChangeLocationWithinPage()
{
	bool bMainFrame = isMainFrame(m_frame);
	if(bMainFrame)
	{
		m_webFrame->d->urlChanged();
		using namespace EA::WebKit;
		if(EAWebKitClient* const pClient = GetEAWebKitClient(m_webFrame->page()->view()))
		{
			LoadInfo loadInfo(m_webFrame->page()->view(), m_webFrame->page()->view()->GetUserData(),kLETUrlChanged);
			EAW_LOG(3, "Event %s : The URL has been changed", EVENT_TYPE_STR(kLETUrlChanged) );
			pClient->LoadUpdate(loadInfo);
		}
		m_webFrame->page()->d->updateNavigationActions();
	}
}



//abaldeva: Following 3 State related methods are called from the History mechanism. Does not seem that important for us to 
// handle.
void FrameLoaderClientEA::dispatchDidPushStateWithinPage()
{
    notImplemented();
}

void FrameLoaderClientEA::dispatchDidReplaceStateWithinPage()
{
    notImplemented();
}

void FrameLoaderClientEA::dispatchDidPopStateWithinPage()
{
    notImplemented();
}

void FrameLoaderClientEA::dispatchWillClose()
{
	// In 1.x, we triggered kLETWillClose from this which is wrong. It seems like WebKit loads resources Frame by Frame and
	// this function is called when a new Frame starts to load and the last frame is done loading. If your page only has a single
	// frame, identifying this as a page close event ends up working by luck. If your page has multiple frames, this function is
	// called for each new frame load and will result in bugs. Worst, this was not even called when the user shuts down the page.
	
}


void FrameLoaderClientEA::dispatchDidStartProvisionalLoad()
{
	m_lastRequestedUrl = m_frame->loader().activeDocumentLoader()->requestURL();

	
	
	
}

void FrameLoaderClientEA::dispatchDidReceiveTitle(const WebCore::StringWithDirection &)
{
	// This is called for each frame but we filter the event only for the main frame.
	
	/* We can enable this implementation if the users of EAWebKit require it.
	if (!m_webFrame)
        return;

	if(isMainFrame(m_frame))
	{
		using namespace EA::WebKit;
		if(EAWebKitClient* const pClient = GetEAWebKitClient(m_webFrame->page()->view()))
		{
			LoadInfo loadInfo(m_webFrame->page()->view(),m_webFrame->page()->view()->GetUserData(),kLETTitleChanged);
			GetFixedString(loadInfo.mPageTitle)->assign(title.characters(), title.length());
			pClient->LoadUpdate(loadInfo);
		}
	}
	*/
}


void FrameLoaderClientEA::dispatchDidChangeIcons(WebCore::IconType)
{
	notImplemented();
}


void FrameLoaderClientEA::dispatchDidCommitLoad()
{
	// abaldeva: This is called when the first chunk of the data is received from the server for a requested load. 
    if (m_frame->tree().parent() || !m_webFrame)
        return;

	bool bMainFrame = isMainFrame(m_frame);
	if(bMainFrame)
	{
        m_webFrame->d->urlChanged();
		
		using namespace EA::WebKit;
		if(EAWebKitClient* const pClient = GetEAWebKitClient(m_webFrame->page()->view()))
		{
			LoadInfo loadInfo(m_webFrame->page()->view(), m_webFrame->page()->view()->GetUserData(),kLETLoadCommited);
			EAW_LOG(3, "Event %s : The first chunk of data from the server is available", EVENT_TYPE_STR(kLETLoadCommited));
			pClient->LoadUpdate(loadInfo);
		}
	}
    m_webFrame->page()->d->updateNavigationActions();

    /*
	// We should assume first the frame has no title. If it has, then the above dispatchDidReceiveTitle()
    // will be called very soon with the correct title.
    // This properly resets the title when we navigate to a URI without a title by sending
	// an empty string.
	if(bMainFrame)
	{
		using namespace EA::WebKit;
		if(EAWebKitClient* const pClient = GetEAWebKitClient(m_webFrame->page()->view()))
		{
			LoadInfo loadInfo(m_webFrame->page()->view(), m_webFrame->page()->view()->GetUserData(),kLETTitleChanged);
			GetFixedString(loadInfo.mPageTitle)->clear();
			
			pClient->LoadUpdate(loadInfo);
		}
	}
    */

}


void FrameLoaderClientEA::dispatchDidFinishDocumentLoad()
{
    if (m_frame->tree().parent() || !m_webFrame)
        return;

    m_webFrame->page()->d->updateNavigationActions();
}


void FrameLoaderClientEA::dispatchDidFinishLoad()
{
	// abaldeva: This is called when a frame finishes loading.
	
	WebCore::ResourceError lastError = m_loadError;
	m_loadError = ResourceError();
	if (!m_webFrame)
		return;
	
	using namespace EA::WebKit;
	m_webFrame->SetLoadState(WebFrame::kLoadFinished);
	
	bool allFramesLoaded = true;
	for (WebCore::Frame* frame = &(m_frame->page()->mainFrame()); frame; frame = frame->tree().traverseNext())
	{
		WebFrame* webFrame = WebFramePrivate::kit(frame);
		// This can be NULL some times when a Frame is created through JS and immediately somehow ends up in the load complete state.
		// we could possibly resolve this by moving this line in FrameLoader::init()as well.
		// m_networkingContext = m_client->createNetworkingContext();
		if(webFrame) 
		{
			WebFrame::LoadState loadState = webFrame->GetLoadState();
			if(loadState != WebFrame::kLoadFinished)
			{
				allFramesLoaded = false;
				break;
			}
		}
	}

	if(allFramesLoaded)
	{
		if(EAWebKitClient* const pClient = GetEAWebKitClient(m_webFrame->page()->view()))
		{
			LoadEventType letType = (lastError.isNull() ? kLETLoadCompletedWithoutErrors : kLETLoadCompletedWithErrors);
			if (letType == kLETLoadCompletedWithoutErrors)
				EAW_LOG(3, "Event %s : The load is completed without errors", EVENT_TYPE_STR(kLETLoadCompletedWithoutErrors) );
			else
				EAW_LOG(3, "Event %s : The load is completed with error - %d", EVENT_TYPE_STR(kLETLoadCompletedWithErrors), lastError.errorCode());
			LoadInfo loadInfo(m_webFrame->page()->view(), m_webFrame->page()->view()->GetUserData(), letType);
			pClient->LoadUpdate(loadInfo);
		}
	}
	
    m_webFrame->page()->d->updateNavigationActions();
}

void FrameLoaderClientEA::dispatchDidLayout(WebCore::LayoutMilestones mileStones)
{
	// This is the first time the user is actually going to be able to see something on the screen.
	if(mileStones & DidFirstVisuallyNonEmptyLayout)
	{
		if(isMainFrame(m_frame))
		{
			using namespace EA::WebKit;
			if(EAWebKitClient* const pClient = GetEAWebKitClient(m_webFrame->page()->view()))
			{
				LoadInfo loadInfo(m_webFrame->page()->view(),m_webFrame->page()->view()->GetUserData(),kLETVisualLayOutAvailable);
				EAW_LOG(3, "Event %s : Something appeared on the screen", EVENT_TYPE_STR(kLETVisualLayOutAvailable));
				pClient->LoadUpdate(loadInfo);
			}
		}
	}
}

void FrameLoaderClientEA::dispatchShow()
{
	// abaldeva: This is used for target="_blank" or target="_new". If the page is created by the application after handling
	// dispatchCreatePage, it will be called next (Not exactly sure why two different events - may be this is to transfer the
	// focus).
	// In 1.x, we used kLETWillShow and indicated that as window will be shown first time which was wrong.
	notImplemented();
}


void FrameLoaderClientEA::cancelPolicyCheck()
{
}


void FrameLoaderClientEA::dispatchWillSubmitForm(PassRefPtr<FormState>,FramePolicyFunction function)
{
    notImplemented();
    // FIXME: This is surely too simple
	// abaldeva: I am not sure what this FIXME refers to.
    callPolicyFunction(function, PolicyUse);
}

void FrameLoaderClientEA::postProgressStartedNotification()
{
	using namespace EA::WebKit;
	if (m_webFrame && m_frame->page())
	{
        // A new load starts, so lets clear the previous error.
        m_loadError = ResourceError();
		m_webFrame->SetLoadState(WebFrame::kLoadStarted);
		if(isMainFrame(m_frame)) 
		{
			if(EAWebKitClient* const pClient = GetEAWebKitClient(m_webFrame->page()->view()))
			{
				LoadInfo loadInfo(m_webFrame->page()->view(), m_webFrame->page()->view()->GetUserData(), kLETLoadStarted);
				loadInfo.mProgressEstimation = 0.0f;
				EAW_LOG(3, "Event %s : Sent a request to server for page load", EVENT_TYPE_STR(kLETLoadStarted));
				pClient->LoadUpdate(loadInfo);
			}
		}
        postProgressEstimateChangedNotification();
    }

	if (m_frame->tree().parent() || !m_webFrame)// abaldeva: Is m_frame->tree().parent() trying to detect if it is a subframe?
        return;
   
	m_webFrame->page()->d->updateNavigationActions();
}

void FrameLoaderClientEA::postProgressEstimateChangedNotification()
{
    if (m_webFrame && m_frame->page())
	{
		using namespace EA::WebKit;
		
		m_webFrame->SetLoadState(WebFrame::kLoadInProgress);
		if(EAWebKitClient* const pClient = GetEAWebKitClient(m_webFrame->page()->view()))
		{
			LoadInfo loadInfo(m_webFrame->page()->view(), m_webFrame->page()->view()->GetUserData(),kLETLoadProgressUpdate);
			loadInfo.mProgressEstimation = m_frame->page()->progress().estimatedProgress();
			EAW_LOG(3, "Event %s : Page load in progress with estimate = %lf", EVENT_TYPE_STR(kLETLoadProgressUpdate) , loadInfo.mProgressEstimation );
			pClient->LoadUpdate(loadInfo);
		}
	}
}

void FrameLoaderClientEA::postProgressFinishedNotification()
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

void FrameLoaderClientEA::setMainFrameDocumentReady(bool)
{
    // this is only interesting once we provide an external API for the DOM
}


void FrameLoaderClientEA::willChangeTitle(DocumentLoader*)
{
    // no need for, dispatchDidReceiveTitle is the right callback
}


void FrameLoaderClientEA::didChangeTitle(DocumentLoader*)
{
    // no need for, dispatchDidReceiveTitle is the right callback
}

void FrameLoaderClientEA::finishedLoading(DocumentLoader* loader)
{
    if (!m_pluginView)
        return;
    if (m_pluginView->isPluginView())
        m_pluginView->didFinishLoading();
    m_pluginView = 0;
    m_hasSentResponseToPlugin = false;
}

bool FrameLoaderClientEA::canShowMIMETypeAsHTML(const String& MIMEType) const
{
    notImplemented();
    return false;
}
    
bool FrameLoaderClientEA::canShowMIMEType(const String& MIMEType) const
{
    String type = MIMEType.lower();
    
    if (MIMETypeRegistry::canShowMIMEType(type))
        return true;

    if (m_frame && m_frame->settings().arePluginsEnabled()
        && PluginDatabase::installedPlugins()->isMIMETypeRegistered(type))
        return true;

    return false;
}

bool FrameLoaderClientEA::representationExistsForURLScheme(const String&) const
{
    return false;
}


String FrameLoaderClientEA::generatedMIMETypeForURLScheme(const String&) const
{
    notImplemented();
    return String();
}


void FrameLoaderClientEA::frameLoadCompleted()
{
    // Note that this can be called multiple times.
    if (!m_webFrame)
        return;
    m_webFrame->page()->d->updateNavigationActions();
}


void FrameLoaderClientEA::restoreViewState()
{
//     if (!m_webFrame)
//         return;
//     emit m_webFrame->page()->restoreFrameStateRequested(m_webFrame);
}


void FrameLoaderClientEA::provisionalLoadStarted()
{
    // don't need to do anything here
}


void FrameLoaderClientEA::didFinishLoad()
{
//     notImplemented();
}


void FrameLoaderClientEA::prepareForDataSourceReplacement()
{
}

void FrameLoaderClientEA::setTitle(const WebCore::StringWithDirection &,const WebCore::URL &)
{
    
}


String FrameLoaderClientEA::userAgent(const URL& url)
{
	if (m_webFrame) {
        return m_webFrame->page()->userAgentForUrl(url);
    }
    return String();
}

void FrameLoaderClientEA::dispatchDidReceiveIcon()
{
	
}

void FrameLoaderClientEA::frameLoaderDestroyed()
{
    delete m_webFrame;
	m_frame = 0;
    m_webFrame = 0;

    delete this;
}

bool FrameLoaderClientEA::canHandleRequest(const WebCore::ResourceRequest& request) const
{
	// We use WebPage::acceptNavigationRequest to block any navigation requests that we want instead of here. If we block it here,
	// FrameLoaderClientEA::cannotShowURLError is fired which is counter-intuitive. By returning false from WebPage::acceptNavigationRequest,
	// we set PolicyActionIgnore on the request and WebCore does not assert.
	return true;
}

void FrameLoaderClientEA::dispatchDidClearWindowObjectInWorld(DOMWrapperWorld& world)
{
    if (&world != &mainThreadNormalWorld())
		return;
 
	bool bMainFrame = isMainFrame(m_frame);
	if(bMainFrame)
	{
		using namespace EA::WebKit;
		if(EAWebKitClient* const pClient = GetEAWebKitClient(m_webFrame->page()->view()))
		{
			LoadInfo loadInfo(m_webFrame->page()->view(), m_webFrame->page()->view()->GetUserData(), kLETJSWindowObjectCleared);
			EAW_LOG(3, "Event %s : The JavaScript Window Object cleared", EVENT_TYPE_STR(kLETJSWindowObjectCleared));
			pClient->LoadUpdate(loadInfo);
		}
		
		// Rebind any persistent JS bound objects that have been registered so that client does not need to do it (and also preserve backward compatibility).
		m_webFrame->page()->view()->RebindJSBoundObjects();
		
	}
}

void FrameLoaderClientEA::documentElementAvailable()
{
    return;
}

void FrameLoaderClientEA::didPerformFirstNavigation() const
{
    if (m_frame->tree().parent() || !m_webFrame)
        return;
    m_webFrame->page()->d->updateNavigationActions();
}

void FrameLoaderClientEA::registerForIconNotification(bool)
{
    notImplemented();
}

void FrameLoaderClientEA::updateGlobalHistory()
{
}

void FrameLoaderClientEA::updateGlobalHistoryRedirectLinks()
{
    // Apple WebKit is the only port that makes use of this callback. It calls
    // WebCore::HistoryItem::addRedirectURL() with the contents of
    // loader->[server|client]RedirectDestinationForHistory().
    // WebCore can associate a bunch of redirect URLs with a particular
    // item in the history, presumably this allows Safari to skip the redirections
    // when navigating to that history item. 
}

bool FrameLoaderClientEA::shouldGoToHistoryItem(WebCore::HistoryItem *) const
{
    return true;
}

bool FrameLoaderClientEA::shouldStopLoadingForHistoryItem(WebCore::HistoryItem *) const
{
	return true;
}

void FrameLoaderClientEA::didDisplayInsecureContent()
{
    notImplemented();
}

void FrameLoaderClientEA::didRunInsecureContent(WebCore::SecurityOrigin *,const WebCore::URL &)
{
    notImplemented();
}

void FrameLoaderClientEA::didDetectXSS(const URL&, bool didBlockEntirePage)
{
	if (EA::WebKit::EAWebKitClient *pClient = EA::WebKit::GetEAWebKitClient(m_webFrame->page()->view()))
	{
		EA::WebKit::WatchDogNotificationInfo info;
		info.mpView = m_webFrame->page()->view();
		info.mpUserData = info.mpView->GetUserData();
		info.mWatchDogNotificationType = EA::WebKit::WatchDogNotificationInfo::kXSSDetected;

		pClient->WatchDogNotification(info);
	}
}

void FrameLoaderClientEA::saveViewStateToItem(WebCore::HistoryItem* item)
{
}

bool FrameLoaderClientEA::canCachePage() const
{
    return m_frame->document()->getElementsByTagName("video")->length() == 0 && 
           m_frame->document()->getElementsByTagName("audio")->length() == 0;
}


void FrameLoaderClientEA::setMainDocumentError(WebCore::DocumentLoader* loader, const WebCore::ResourceError& error)
{
	// abaldeva: Not sure of the context in which this function is called. At least, it is called if you cancel the load.
	// However, the implementation here seems like is active only for notifying plugins. May be for main page, there are other
	// mechanisms
	if (!m_pluginView)
        return;
    if (m_pluginView->isPluginView())
        m_pluginView->didFail(error);
    m_pluginView = 0;
    m_hasSentResponseToPlugin = false;
}

// FIXME: This function should be moved into WebCore. 
// abaldeva: above FIXME seems to exist for ages now.
void FrameLoaderClientEA::committedLoad(WebCore::DocumentLoader* loader, const char* data, int length)
{
    if (!m_pluginView)
        loader->commitData(data, length);
    
    // If we are sending data to MediaDocument, we should stop here and cancel the request.
    if (m_frame->document()->isMediaDocument())
        loader->cancelMainResourceLoad(pluginWillHandleLoadError(loader->response()));
    // We re-check here as the plugin can have been created
    if (m_pluginView && m_pluginView->isPluginView()) {
        if (!m_hasSentResponseToPlugin) {
            m_pluginView->didReceiveResponse(loader->response());
            // didReceiveResponse sets up a new stream to the plug-in. on a full-page plug-in, a failure in
            // setting up this stream can cause the main document load to be cancelled, setting m_pluginView
            // to null
            if (!m_pluginView)
                return;
            m_hasSentResponseToPlugin = true;
        }
        m_pluginView->didReceiveData(data, length);
    }
}

WebCore::ResourceError FrameLoaderClientEA::cancelledError(const WebCore::ResourceRequest& request)
{
	ResourceError error = ResourceError("NetworkDomain", EA::WebKit::kLETLoadCancelled, request.url().string(),"Request cancelled");
    error.setIsCancellation(true);
    return error;
}

WebCore::ResourceError FrameLoaderClientEA::blockedError(const WebCore::ResourceRequest& request)
{
	EAW_ASSERT_FORMATTED(false, "WebKit Error:URL blocked - %s",request.url().string().ascii().data());
	return ResourceError("WebKitErrorDomain", EA::WebKit::kLETBlocked, request.url().string(),"Request blocked");
}


WebCore::ResourceError FrameLoaderClientEA::cannotShowURLError(const WebCore::ResourceRequest& request)
{
	EAW_ASSERT_FORMATTED(false, "WebKit Error:Cannot show URL. Please verify the URL you are trying to load - %s",request.url().string().ascii().data());
	return ResourceError("WebKitErrorDomain", EA::WebKit::kLETCannotShowURL, request.url().string(),"Cannot show URL");
}

WebCore::ResourceError FrameLoaderClientEA::interruptedForPolicyChangeError(const WebCore::ResourceRequest &request)
{
	return ResourceError("WebKitErrorDomain", EA::WebKit::kLETFrameLoadInterruptedByPolicyChange, request.url().string(),"Frame load interrupted by policy change");
}

WebCore::ResourceError FrameLoaderClientEA::cannotShowMIMETypeError(const WebCore::ResourceResponse& response)
{
	return ResourceError("WebKitErrorDomain", EA::WebKit::kLETCannotShowMIMEType, response.url().string(),"Cannot show mimetype");
}

WebCore::ResourceError FrameLoaderClientEA::fileDoesNotExistError(const WebCore::ResourceResponse& response)
{
	return ResourceError("NetworkDomain", EA::WebKit::kLETContentNotFoundError, response.url().string(),"File does not exist");
}

WebCore::ResourceError FrameLoaderClientEA::pluginWillHandleLoadError(const WebCore::ResourceResponse& response)
{
	return ResourceError("WebKitErrorDomain", EA::WebKit::KLETPluginWillHandleLoadError, response.url().string(),"Loading is handled by the media engine");
}

bool FrameLoaderClientEA::shouldFallBack(const WebCore::ResourceError& error)
{
    DEFINE_STATIC_LOCAL(const ResourceError, cancelledError, (this->cancelledError(ResourceRequest())));
    DEFINE_STATIC_LOCAL(const ResourceError, pluginWillHandleLoadError, (this->pluginWillHandleLoadError(ResourceResponse())));
    DEFINE_STATIC_LOCAL(const ResourceError, errorInterruptedForPolicyChange, (this->interruptedForPolicyChangeError(ResourceRequest())));

    if (error.errorCode() == cancelledError.errorCode() && error.domain() == cancelledError.domain())
        return false;

    if (error.errorCode() == errorInterruptedForPolicyChange.errorCode() && error.domain() == errorInterruptedForPolicyChange.domain())
        return false;

    if (error.errorCode() == pluginWillHandleLoadError.errorCode() && error.domain() == pluginWillHandleLoadError.domain())
        return false;

    return true;
	
    //return !(error.isCancellation() || (error.errorCode() == EA::WebKit::kLETFrameLoadInterruptedByPolicyChange));
}

WTF::PassRefPtr<WebCore::DocumentLoader> FrameLoaderClientEA::createDocumentLoader(const WebCore::ResourceRequest& request, const SubstituteData& substituteData)
{
    RefPtr<DocumentLoader> loader = DocumentLoader::create(request, substituteData);
    if (!deferMainResourceDataLoad || substituteData.isValid())
        loader->setDeferMainResourceDataLoad(false);
    else
        m_frame->page()->setCustomHTMLTokenizerTimeDelay(-1);
    return loader.release();
}

void FrameLoaderClientEA::convertMainResourceLoadToDownload(WebCore::DocumentLoader*, const WebCore::ResourceRequest&, const WebCore::ResourceResponse&)
{
    if (!m_webFrame)
        return;
	/* 
	The download is usually handled through custom transport handler in EAWebKit.
	*/
	notImplemented();
    
}
void FrameLoaderClientEA::assignIdentifierToInitialRequest(unsigned long identifier, WebCore::DocumentLoader*, const WebCore::ResourceRequest& request)
{
	

}

void FrameLoaderClientEA::dispatchWillSendRequest(WebCore::DocumentLoader*, unsigned long identifier, WebCore::ResourceRequest& newRequest, const WebCore::ResourceResponse& redirectResponse)
{
	if(!newRequest.url().isEmpty() && !redirectResponse.url().isEmpty())
	{
		int statusCode = redirectResponse.httpStatusCode();
		EAW_ASSERT_MSG(statusCode >= 300 && statusCode < 400,"Redirection in progress but statusCode not in redirection range");

		using namespace EA::WebKit;
		//A redirection is in progress. Notify the application.
		if(EAWebKitClient* const pClient = GetEAWebKitClient(m_webFrame->page()->view()))
		{
			LoadInfo loadInfo(m_webFrame->page()->view(), m_webFrame->page()->view()->GetUserData(),kLETRedirectionReceived);
			loadInfo.mStatusCode = statusCode;
			
			const WTF::String& urlTo	= newRequest.url().string();
			const WTF::String& urlFrom	= redirectResponse.url().string();

			GetFixedString(loadInfo.mResourceURL)->assign(urlFrom.characters(), urlFrom.length());
			GetFixedString(loadInfo.mRedirectedURL)->assign(urlTo.characters(), urlTo.length());

			EAW_LOG(3, "Event %s : The server has a redirection request to %ls from %ls", EVENT_TYPE_STR(kLETRedirectionReceived) , GetFixedString(loadInfo.mRedirectedURL)->c_str(), GetFixedString(loadInfo.mResourceURL)->c_str() );
			pClient->LoadUpdate(loadInfo);
		}

	}

	// This is called for each link/resource. An application can possibly modify the navigation flow here.
	using namespace EA::WebKit;
	if(EAWebKitClient* const pClient = GetEAWebKitClient(m_webFrame->page()->view()))
	{
		LinkNotificationInfo  lni;
		lni.mpView					= m_webFrame->page()->view();
		lni.mpUserData				= lni.mpView->GetUserData();
		lni.mLinkNotificationType	= LinkNotificationInfo::kLinkNavigationModify;

		// Setup mOriginalURI
		const WTF::String& webCoreURI = newRequest.url().string();
		GetFixedString(lni.mOriginalURI)->assign(webCoreURI.characters(), webCoreURI.length());

		// Setup mOriginalHeaderMap. 
		EA::WebKit::HeaderMap& originalHeaderMap = *GetHeaderMap(lni.mOriginalHeaderMap);
		const WebCore::HTTPHeaderMap& webcoreHeaderMap = newRequest.httpHeaderFields();
		for(WebCore::HTTPHeaderMap::const_iterator it(webcoreHeaderMap.begin()); it != webcoreHeaderMap.end(); ++it)
		{
			const WTF::String& webCoreKey   = it->key;
			const WTF::String& webCoreValue = it->value;

			HeaderMap::value_type entry(HeaderMap::key_type(webCoreKey.characters(), webCoreKey.length()), 
				HeaderMap::mapped_type(webCoreValue.characters(), webCoreValue.length()));

			originalHeaderMap.insert(entry);
		}

		// Notify Application.
		pClient->LinkNotification(lni);
		
		EAW_ASSERT_MSG(!lni.mBlockNavigation, "If interested in blocking, it should happen only if event type is LinkNotificationInfo::kLinkNavigationBlock");
		
		// Application modified something and wants us to tackle the rest.
		if(!GetFixedString(lni.mModifiedURI)->empty())// If modified URL
		{
			WebCore::URL newURL(WebCore::URL(), GetFixedString(lni.mModifiedURI)->c_str());
			newRequest.setURL(newURL);
		}

		EA::WebKit::HeaderMap& newHeaderMap = *GetHeaderMap(lni.mModifiedHeaderMap);
		if(!newHeaderMap.empty())// If modified headers
		{
			// We need to const_cast this, as ResourceRequest doesn't have a function to clear the headers.
			WebCore::HTTPHeaderMap& webCoreHeaderMap = const_cast<WebCore::HTTPHeaderMap&>(newRequest.httpHeaderFields());
			webCoreHeaderMap.clear(); // We will be completely re-writing the headers.

			for(HeaderMap::const_iterator it = newHeaderMap.begin(); it != newHeaderMap.end(); ++it)
			{
				const WTF::AtomicString key(it->first.c_str(), it->first.length());
				const WTF::String val(it->second.c_str(), it->second.length());
				newRequest.addHTTPHeaderField(key, val);
			}
		}	
	}
}

bool FrameLoaderClientEA::shouldUseCredentialStorage(DocumentLoader*, unsigned long)
{
    notImplemented();
    return false;
}

void FrameLoaderClientEA::dispatchDidReceiveAuthenticationChallenge(DocumentLoader*, unsigned long, const AuthenticationChallenge&)
{
    notImplemented();
}

void FrameLoaderClientEA::dispatchDidCancelAuthenticationChallenge(DocumentLoader*, unsigned long, const AuthenticationChallenge&)
{
    notImplemented();
}

void FrameLoaderClientEA::dispatchDidReceiveResponse(WebCore::DocumentLoader*, unsigned long identifier, const WebCore::ResourceResponse& response)
{
    m_response = response;
	using namespace EA::WebKit;
	if(EAWebKitClient* const pClient = GetEAWebKitClient(m_webFrame->page()->view()))
	{
		LoadInfo loadInfo(m_webFrame->page()->view(), m_webFrame->page()->view()->GetUserData(),kLETResourceResponseReceived);
		loadInfo.mStatusCode = response.httpStatusCode();
		const WTF::String& resourceURL = response.url().string();
		if(resourceURL.length())
			GetFixedString(loadInfo.mResourceURL)->assign(resourceURL.characters(), resourceURL.length());

		EAW_LOG(3, "Event %s : The server has responded to a resource request URL - %ls with status = %ld", EVENT_TYPE_STR(kLETResourceResponseReceived) , GetFixedString(loadInfo.mResourceURL)->c_str() , loadInfo.mStatusCode );
		pClient->LoadUpdate(loadInfo);
	}
}

void FrameLoaderClientEA::dispatchDidReceiveContentLength(WebCore::DocumentLoader*, unsigned long, int)
{
}

void FrameLoaderClientEA::dispatchDidFinishLoading(WebCore::DocumentLoader*, unsigned long identifier)
{
	// abaldeva: This is called when the resource identified by the identifier finished loading.
}

void FrameLoaderClientEA::dispatchDidFailLoading(WebCore::DocumentLoader* loader, unsigned long identifier, const WebCore::ResourceError& error)
{
	m_loadError = error;
	
	using namespace EA::WebKit;
	if(EAWebKitClient* const pClient = GetEAWebKitClient(m_webFrame->page()->view()))
	{
		LoadInfo loadInfo(m_webFrame->page()->view(), m_webFrame->page()->view()->GetUserData(),kLETResourceLoadError);
		loadInfo.mLoadErrorType = (EA::WebKit::LoadErrorType) error.errorCode();
		const WTF::String& failingURL = error.failingURL();
		if(failingURL.length())
			GetFixedString(loadInfo.mResourceURL)->assign(failingURL.characters(), failingURL.length());

		EAW_LOG(3, "Event %s : Error = %d while loading resource - %ls", EVENT_TYPE_STR(kLETResourceLoadError), loadInfo.mLoadErrorType , GetFixedString(loadInfo.mResourceURL)->c_str() );
		pClient->LoadUpdate(loadInfo);
	}
}

bool FrameLoaderClientEA::dispatchDidLoadResourceFromMemoryCache(WebCore::DocumentLoader*, const WebCore::ResourceRequest&, const WebCore::ResourceResponse&, int)
{
    notImplemented();
    return false;
}

bool FrameLoaderClientEA::callErrorPageExtension(const WebCore::ResourceError& error)
{
	return false;
//     QWebPage* page = m_webFrame->page();
//		Seems like this is always false for default QWebPage implementation
//     if (page->supportsExtension(QWebPage::ErrorPageExtension)) {
//         QWebPage::ErrorPageExtensionOption option;
// 
//         if (error.domain() == "QtNetwork")
//             option.domain = QWebPage::QtNetwork;
//         else if (error.domain() == "HTTP")
//             option.domain = QWebPage::Http;
//         else if (error.domain() == "WebKit")
//             option.domain = QWebPage::WebKit;
//         else
//             return;
// 
//         option.url = QUrl(error.failingURL());
//         option.frame = m_webFrame;
//         option.error = error.errorCode();
//         option.errorString = error.localizedDescription();
// 
//         QWebPage::ErrorPageExtensionReturn output;
//         if (!page->extension(QWebPage::ErrorPageExtension, &option, &output))
//             return;
// 
//         URL baseUrl(output.baseUrl);
//         URL failingUrl(option.url);
// 
//         WebCore::ResourceRequest request(baseUrl);
//         WTF::RefPtr<WebCore::SharedBuffer> buffer = WebCore::SharedBuffer::create(output.content.constData(), output.content.length());
//         WebCore::SubstituteData substituteData(buffer, output.contentType, output.encoding, failingUrl);
//         m_frame->loader().load(request, substituteData, false);
//     }
}

void FrameLoaderClientEA::dispatchDidFailProvisionalLoad(const WebCore::ResourceError& error)
{
/*  We don't handle this and instead handle ::dispatchDidFailLoading 
    m_loadError = error;
    if (!error.isNull() && !error.isCancellation())
	{
		//callErrorPageExtension(error);
		using namespace EA::WebKit;
		if(EAWebKitClient* const pClient = GetEAWebKitClient(m_webFrame->page()->view()))
		{
			LoadInfo loadInfo(m_webFrame->page()->view(), m_webFrame->page()->view()->GetUserData(),kLETResourceLoadFailed);
			const WTF::String& failingURL = error.failingURL();
			if(failingURL.length())
				GetFixedString(loadInfo.mResourceURL)->assign(failingURL.characters(), failingURL.length());

			pClient->LoadUpdate(loadInfo);

		}
	}
*/
}

void FrameLoaderClientEA::dispatchDidFailLoad(const WebCore::ResourceError& error)
{

/*	We don't handle this and instead handle ::dispatchDidFailLoading 
    m_loadError = error;
    if (!error.isNull() && !error.isCancellation())
	{
		//callErrorPageExtension(error);
		using namespace EA::WebKit;
		if(EAWebKitClient* const pClient = GetEAWebKitClient(m_webFrame->page()->view()))
		{
			LoadInfo loadInfo(m_webFrame->page()->view(), m_webFrame->page()->view()->GetUserData(),kLETResourceLoadFailed);
			const WTF::String& failingURL = error.failingURL();
			if(failingURL.length())
				GetFixedString(loadInfo.mPageURL)->assign(failingURL.characters(), failingURL.length());

			pClient->LoadUpdate(loadInfo);

		}
	}
*/
}

WebCore::Frame* FrameLoaderClientEA::dispatchCreatePage(const WebCore::NavigationAction& action)
{
    if (!m_webFrame)
        return 0;
	using namespace EA::WebKit;
 	if(EAWebKitClient* const pClient = GetEAWebKitClient(m_webFrame->page()->view()))
 	{
 		CreateViewInfo createViewInfo;
 		createViewInfo.mpView = m_webFrame->page()->view();
		createViewInfo.mpUserData = createViewInfo.mpView->GetUserData();
		createViewInfo.mEventType = EA::WebKit::CreateViewInfo::kEventTargetBlank;
		WebCore::URL kurl = action.url();
 		FixedString16_128 urlToOpen(kurl.string().characters(),kurl.string().length());
 		createViewInfo.mpURLToOpen = urlToOpen.c_str();
 
 		pClient->CreateView(createViewInfo);
 		if(createViewInfo.mpCreatedView)
 		{
 			return createViewInfo.mpCreatedView->Page()->mainFrame()->d->frame;
 		}
 	}
 
 	return 0;
}

void FrameLoaderClientEA::dispatchDecidePolicyForResponse(const WebCore::ResourceResponse &response,const WebCore::ResourceRequest& ,WebCore::FramePolicyFunction function) 
{
	// We need to call directly here.
	switch (response.httpStatusCode()) {
    case HTTPResetContent:
		// FIXME: a 205 response requires that the requester reset the document view.
		// Fallthrough
    case HTTPNoContent:
		callPolicyFunction(function, PolicyIgnore);
		return;
	}

	if (WebCore::contentDispositionType(response.httpHeaderField("Content-Disposition")) == WebCore::ContentDispositionAttachment)
		callPolicyFunction(function, PolicyDownload);
	else if (canShowMIMEType(response.mimeType()))
		callPolicyFunction(function, PolicyUse);
	else
		callPolicyFunction(function, PolicyDownload);
}

void FrameLoaderClientEA::dispatchDecidePolicyForNewWindowAction(const WebCore::NavigationAction& action, const WebCore::ResourceRequest& request, PassRefPtr<WebCore::FormState>, const WTF::String&,FramePolicyFunction function)
{
    EAW_ASSERT_MSG(m_webFrame,"No WebFrame!");
	if(!m_webFrame)
		return;

	EA::WebKit::WebPage* page = m_webFrame->page();

	if (!page->d->acceptNavigationRequest(0, request, EA::WebKit::WebPage::NavigationType(action.type()))) {
        if (action.type() == NavigationTypeFormSubmitted || action.type() == NavigationTypeFormResubmitted)
            m_frame->loader().resetMultipleFormSubmissionProtection();

        if (action.type() == NavigationTypeLinkClicked && request.url().hasFragmentIdentifier()) {
			ResourceRequest emptyRequest;
            m_frame->loader().activeDocumentLoader()->setLastCheckedRequest(emptyRequest);
        }

        callPolicyFunction(function, PolicyIgnore);
        return;
    }
    callPolicyFunction(function, PolicyUse);
}

void FrameLoaderClientEA::dispatchDecidePolicyForNavigationAction(const WebCore::NavigationAction& action, const WebCore::ResourceRequest& request, PassRefPtr<WebCore::FormState>,FramePolicyFunction function)
{
	EAW_ASSERT_MSG(m_webFrame,"No WebFrame!");
	if(!m_webFrame)
		return;

	EA::WebKit::WebPage* page = m_webFrame->page();
	WebCore::PolicyAction result;

    // Currently, this is only enabled by DRT
    if (policyDelegateEnabled) {
		RefPtr<WebCore::Node> node;
        for (const Event* event = action.event(); event; event = event->underlyingEvent()) {
            if (event->isMouseEvent()) {
				const WebCore::MouseEvent* mouseEvent =  static_cast<const WebCore::MouseEvent*>(event);
				node = EA::WebKit::WebFramePrivate::core(m_webFrame)->eventHandler().hitTestResultAtPoint(
                    mouseEvent->absoluteLocation(), false).innerNonSharedNode();
                break;
            }
        }

        if (policyDelegatePermissive)
            result = PolicyUse;
        else
            result = PolicyIgnore;

        page->d->acceptNavigationRequest(m_webFrame, request, EA::WebKit::WebPage::NavigationType(action.type()));
        callPolicyFunction(function, result);
        return;
    }

	if (!page->d->acceptNavigationRequest(m_webFrame, request, EA::WebKit::WebPage::NavigationType(action.type()))) {
        if (action.type() == NavigationTypeFormSubmitted || action.type() == NavigationTypeFormResubmitted)
            m_frame->loader().resetMultipleFormSubmissionProtection();

        if (action.type() == NavigationTypeLinkClicked && request.url().hasFragmentIdentifier()) {
			WebCore::ResourceRequest emptyRequest;
            m_frame->loader().activeDocumentLoader()->setLastCheckedRequest(emptyRequest);
        }

        callPolicyFunction(function, PolicyIgnore);
        return;
    }
    callPolicyFunction(function, PolicyUse);
}

void FrameLoaderClientEA::dispatchUnableToImplementPolicy(const WebCore::ResourceError&)
{
    notImplemented();
}

void FrameLoaderClientEA::startDownload(const WebCore::ResourceRequest &, const WTF::String &)
{
//     if (!m_webFrame)
//         return;
// 
//     emit m_webFrame->page()->downloadRequested(request.toNetworkRequest(m_webFrame));
}

PassRefPtr<Frame> FrameLoaderClientEA::createFrame(const URL& url, const String& name, HTMLFrameOwnerElement* ownerElement,
                                        const String& referrer, bool allowsScrolling, int marginWidth, int marginHeight)
{
    if (!m_webFrame)
        return 0;

	using namespace EA::WebKit;
	
	WebFrameData frameData(m_frame->page(), m_frame, ownerElement, name);

    if (url.isEmpty())
        frameData.url = blankURL();
    else
        frameData.url = url;

    frameData.referrer = referrer;
    frameData.allowsScrolling = allowsScrolling;
    frameData.marginWidth = marginWidth;
    frameData.marginHeight = marginHeight;

    WebFrame* webFrame = new WebFrame(m_webFrame, &frameData);
    // The creation of the frame may have run arbitrary JavaScript that removed it from the page already.
    if (!webFrame->d->frame->page()) {
        frameData.frame.release();
		//ASSERT(webFrame.isNull());
        return 0;
    }

    //emit m_webFrame->page()->frameCreated(webFrame);

    // ### set override encoding if we have one

    m_frame->loader().loadURLIntoChildFrame(frameData.url, frameData.referrer, frameData.frame.get());

    // The frame's onload handler may have removed it from the document.
    if (!frameData.frame->tree().parent())
        return 0;

    return frameData.frame.release();
}

ObjectContentType FrameLoaderClientEA::objectContentType(const URL& url, const String& mimeTypeIn, bool)
{
    String extension = url.path().substring(url.path().reverseFind('.') + 1);
	if (url.isEmpty() && !mimeTypeIn.length())
        return ObjectContentNone;

    String mimeType = mimeTypeIn;
    if (!mimeType.length()) {

		mimeType = MIMETypeRegistry::getMIMETypeForExtension(extension);

    }

	bool arePluginsEnabled = (m_frame && m_frame->settings().arePluginsEnabled());
    if (arePluginsEnabled && !mimeType.length())
		mimeType = PluginDatabase::installedPlugins()->MIMETypeForExtension(extension);

    if (!mimeType.length())
        return ObjectContentFrame;

	ObjectContentType plugInType = ObjectContentNone;
    if (arePluginsEnabled && PluginDatabase::installedPlugins()->isMIMETypeRegistered(mimeType))
		plugInType = ObjectContentNetscapePlugin;
	else if (m_frame->page() && m_frame->page()->pluginData().supportsMimeType(mimeType,PluginData::AllPlugins))
		plugInType = ObjectContentOtherPlugin;

	if (MIMETypeRegistry::isSupportedImageMIMEType(mimeType))
		return true/*shouldPreferPlugInsForImages*/ && plugInType != ObjectContentNone ? plugInType : ObjectContentImage;

	if (plugInType != ObjectContentNone)
		return plugInType;

	if (MIMETypeRegistry::isSupportedNonImageMIMEType(mimeType))
		return ObjectContentFrame;

	if (url.protocol() == "about")
		return ObjectContentFrame;

	return ObjectContentNone;
}

PassRefPtr<Widget> FrameLoaderClientEA::createPlugin(const IntSize& pluginSize, HTMLPlugInElement* element, const URL& url, const Vector<String>& paramNames,
                                          const Vector<String>& paramValues, const String& mimeType, bool loadManually)
{
    if (!m_webFrame)
        return 0;

	// we don't support plugins.
	notImplemented();
	return 0;
}

void FrameLoaderClientEA::redirectDataToPlugin(Widget* pluginWidget)
{
    ASSERT(!m_pluginView);
    m_pluginView = static_cast<PluginView*>(pluginWidget);
    m_hasSentResponseToPlugin = false;
}

PassRefPtr<Widget> FrameLoaderClientEA::createJavaAppletWidget(const IntSize& pluginSize, HTMLAppletElement* element, const URL& url,
                                                    const Vector<String>& paramNames, const Vector<String>& paramValues)
{
    return createPlugin(pluginSize, element, url, paramNames, paramValues, "application/x-java-applet", true);
}

WTF::String FrameLoaderClientEA::overrideMediaType() const
{
	return WTF::String();
}

PassRefPtr<FrameNetworkingContext> FrameLoaderClientEA::createNetworkingContext()
{
    return FrameNetworkingContextEA::create(m_frame, m_webFrame,false);
}

}

