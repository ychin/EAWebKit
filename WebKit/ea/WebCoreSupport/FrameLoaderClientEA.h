/*
 * Copyright (C) 2006 Zack Rusin <zack@kde.org>
 * Copyright (C) 2006, 2011 Apple Inc. All rights reserved.
 * Copyright (C) 2008 Nokia Corporation and/or its subsidiary(-ies)
 * Copyright (C) 2008 Collabora Ltd. All rights reserved.
 * Copyright (C) 2011, 2014 Electronic Arts, Inc. All rights reserved.
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
#ifndef FrameLoaderClientEA_h
#define FrameLoaderClientEA_h

#include "FormState.h"
#include "Frame.h"
#include "FrameLoader.h"
#include "FrameLoaderClient.h"
#include "HTMLFormElement.h"
#include "URL.h"
#include <wtf/OwnPtr.h>
#include "PluginView.h"
#include <wtf/RefCounted.h>
#include "ResourceError.h"
#include "ResourceResponse.h"
#include <wtf/Forward.h>
namespace EA
{
namespace WebKit
{
class WebFrame;
}
}

namespace WebCore {

class AuthenticationChallenge;
class DocumentLoader;
class Element;
class FormState;
class NavigationAction;
class FrameNetworkingContext;
class ResourceLoader;

struct LoadErrorResetToken;

class FrameLoaderClientEA : public FrameLoaderClient {

	friend class EA::WebKit::WebFrame;
    void callPolicyFunction(FramePolicyFunction, PolicyAction);
    bool callErrorPageExtension(const ResourceError&);
public:
    FrameLoaderClientEA();
    ~FrameLoaderClientEA();
    virtual void frameLoaderDestroyed() OVERRIDE;

	void setFrame(EA::WebKit::WebFrame* webFrame, Frame* frame);

    virtual bool hasWebView() const OVERRIDE; // mainly for assertions

    virtual void makeRepresentation(DocumentLoader*) OVERRIDE{ }
    virtual void forceLayout() OVERRIDE;
    virtual void forceLayoutForNonHTML() OVERRIDE;

    virtual void setCopiesOnScroll() OVERRIDE;

    virtual void detachedFromParent2() OVERRIDE;
    virtual void detachedFromParent3() OVERRIDE;

    virtual void assignIdentifierToInitialRequest(unsigned long identifier, WebCore::DocumentLoader*, const WebCore::ResourceRequest&) OVERRIDE;

    virtual void dispatchWillSendRequest(WebCore::DocumentLoader*, unsigned long, WebCore::ResourceRequest&, const WebCore::ResourceResponse&) OVERRIDE;
    virtual bool shouldUseCredentialStorage(DocumentLoader*, unsigned long identifier) OVERRIDE;
    virtual void dispatchDidReceiveAuthenticationChallenge(DocumentLoader*, unsigned long identifier, const AuthenticationChallenge&) OVERRIDE;
    virtual void dispatchDidCancelAuthenticationChallenge(DocumentLoader*, unsigned long identifier, const AuthenticationChallenge&) OVERRIDE;
    virtual void dispatchDidReceiveResponse(WebCore::DocumentLoader*, unsigned long, const WebCore::ResourceResponse&) OVERRIDE;
    virtual void dispatchDidReceiveContentLength(WebCore::DocumentLoader*, unsigned long, int) OVERRIDE;
    virtual void dispatchDidFinishLoading(WebCore::DocumentLoader*, unsigned long) OVERRIDE;
    virtual void dispatchDidFailLoading(WebCore::DocumentLoader*, unsigned long, const WebCore::ResourceError&) OVERRIDE;
    virtual bool dispatchDidLoadResourceFromMemoryCache(WebCore::DocumentLoader*, const WebCore::ResourceRequest&, const WebCore::ResourceResponse&, int) OVERRIDE;

    virtual void dispatchDidHandleOnloadEvents() OVERRIDE;
    virtual void dispatchDidReceiveServerRedirectForProvisionalLoad() OVERRIDE;
    virtual void dispatchDidCancelClientRedirect() OVERRIDE;
    virtual void dispatchWillPerformClientRedirect(const URL&, double interval, double fireDate) OVERRIDE;
    virtual void dispatchDidNavigateWithinPage() OVERRIDE;
    virtual void dispatchDidChangeLocationWithinPage() OVERRIDE;
    virtual void dispatchDidPushStateWithinPage() OVERRIDE;
    virtual void dispatchDidReplaceStateWithinPage() OVERRIDE;
    virtual void dispatchDidPopStateWithinPage() OVERRIDE;
    virtual void dispatchWillClose() OVERRIDE;
    virtual void dispatchDidReceiveIcon() OVERRIDE;
    virtual void dispatchDidStartProvisionalLoad() OVERRIDE;
    virtual void dispatchDidReceiveTitle(const StringWithDirection&) OVERRIDE;
    virtual void dispatchDidChangeIcons(WebCore::IconType) OVERRIDE;
    virtual void dispatchDidCommitLoad() OVERRIDE;
    virtual void dispatchDidFailProvisionalLoad(const ResourceError&) OVERRIDE;
    virtual void dispatchDidFailLoad(const WebCore::ResourceError&) OVERRIDE;
    virtual void dispatchDidFinishDocumentLoad() OVERRIDE;
    virtual void dispatchDidFinishLoad() OVERRIDE;
    virtual void dispatchDidLayout(WebCore::LayoutMilestones) OVERRIDE;

    virtual WebCore::Frame* dispatchCreatePage(const WebCore::NavigationAction&) OVERRIDE;
    virtual void dispatchShow() OVERRIDE;

    virtual void dispatchDecidePolicyForResponse(const ResourceResponse&, const ResourceRequest&, FramePolicyFunction) OVERRIDE;
    virtual void dispatchDecidePolicyForNewWindowAction(const WebCore::NavigationAction&, const WebCore::ResourceRequest&, PassRefPtr<FormState>, const WTF::String&,FramePolicyFunction) OVERRIDE;
    virtual void dispatchDecidePolicyForNavigationAction(const WebCore::NavigationAction&, const WebCore::ResourceRequest&, PassRefPtr<FormState>,FramePolicyFunction) OVERRIDE;
    virtual void cancelPolicyCheck() OVERRIDE;

    virtual void dispatchUnableToImplementPolicy(const WebCore::ResourceError&) OVERRIDE;

    virtual void dispatchWillSendSubmitEvent(PassRefPtr<FormState>) OVERRIDE{ }
    virtual void dispatchWillSubmitForm(PassRefPtr<FormState>,FramePolicyFunction) OVERRIDE;

    virtual void revertToProvisionalState(DocumentLoader*) OVERRIDE{ }
    virtual void setMainDocumentError(DocumentLoader*, const ResourceError&) OVERRIDE;

    virtual void postProgressStartedNotification() OVERRIDE;
    virtual void postProgressEstimateChangedNotification() OVERRIDE;
    virtual void postProgressFinishedNotification() OVERRIDE;

    virtual void setMainFrameDocumentReady(bool) OVERRIDE;

    virtual void startDownload(const WebCore::ResourceRequest&, const String& suggestedName = String()) OVERRIDE;

    virtual void willChangeTitle(DocumentLoader*) OVERRIDE;
    virtual void didChangeTitle(DocumentLoader*) OVERRIDE;

    virtual void committedLoad(WebCore::DocumentLoader*, const char*, int) OVERRIDE;
    virtual void finishedLoading(DocumentLoader*) OVERRIDE;

    virtual void updateGlobalHistory() OVERRIDE;
    virtual void updateGlobalHistoryRedirectLinks() OVERRIDE;
    virtual bool shouldGoToHistoryItem(HistoryItem*) const OVERRIDE;
    virtual bool shouldStopLoadingForHistoryItem(HistoryItem*) const OVERRIDE;
    virtual void didDisplayInsecureContent() OVERRIDE;
    virtual void didRunInsecureContent(SecurityOrigin*, const URL&) OVERRIDE;
    virtual void didDetectXSS(const URL&, bool didBlockEntirePage) OVERRIDE;

    virtual ResourceError cancelledError(const ResourceRequest&) OVERRIDE;
    virtual ResourceError blockedError(const ResourceRequest&) OVERRIDE;
    virtual ResourceError cannotShowURLError(const ResourceRequest&) OVERRIDE;
    virtual ResourceError interruptedForPolicyChangeError(const ResourceRequest&) OVERRIDE;

    virtual ResourceError cannotShowMIMETypeError(const ResourceResponse&) OVERRIDE;
    virtual ResourceError fileDoesNotExistError(const ResourceResponse&) OVERRIDE;
    virtual ResourceError pluginWillHandleLoadError(const ResourceResponse&) OVERRIDE;

    virtual bool shouldFallBack(const ResourceError&) OVERRIDE;

    virtual bool canHandleRequest(const WebCore::ResourceRequest&) const OVERRIDE;
    virtual bool canShowMIMEType(const String& MIMEType) const OVERRIDE;
    virtual bool canShowMIMETypeAsHTML(const String& MIMEType) const OVERRIDE;
    virtual bool representationExistsForURLScheme(const String& URLScheme) const OVERRIDE;
    virtual String generatedMIMETypeForURLScheme(const String& URLScheme) const OVERRIDE;

    virtual void frameLoadCompleted() OVERRIDE;
    virtual void saveViewStateToItem(WebCore::HistoryItem*) OVERRIDE;
    virtual void restoreViewState() OVERRIDE;
    virtual void provisionalLoadStarted() OVERRIDE;
    virtual void didFinishLoad() OVERRIDE;
    virtual void prepareForDataSourceReplacement() OVERRIDE;

    virtual WTF::PassRefPtr<WebCore::DocumentLoader> createDocumentLoader(const WebCore::ResourceRequest&, const WebCore::SubstituteData&) OVERRIDE;
    virtual void setTitle(const StringWithDirection&, const URL&) OVERRIDE;

    virtual String userAgent(const WebCore::URL&) OVERRIDE;

    virtual void savePlatformDataToCachedFrame(WebCore::CachedFrame*) OVERRIDE;
    virtual void transitionToCommittedFromCachedFrame(WebCore::CachedFrame*) OVERRIDE;
    virtual void transitionToCommittedForNewPage() OVERRIDE;

    virtual void didSaveToPageCache() OVERRIDE;
    virtual void didRestoreFromPageCache() OVERRIDE;

    virtual void dispatchDidBecomeFrameset(bool) OVERRIDE;

    virtual bool canCachePage() const OVERRIDE;
    virtual void convertMainResourceLoadToDownload(WebCore::DocumentLoader*, const WebCore::ResourceRequest&, const WebCore::ResourceResponse&) OVERRIDE;

    virtual PassRefPtr<Frame> createFrame(const URL&, const String& name, HTMLFrameOwnerElement*, const String& referrer, bool allowsScrolling, int marginWidth, int marginHeight) OVERRIDE;
    virtual PassRefPtr<Widget> createPlugin(const IntSize&, HTMLPlugInElement*, const URL&, const Vector<String>&, const Vector<String>&, const String&, bool) OVERRIDE;
    virtual void recreatePlugin(Widget*) OVERRIDE{ }
    virtual void redirectDataToPlugin(Widget* pluginWidget) OVERRIDE;

    virtual PassRefPtr<Widget> createJavaAppletWidget(const IntSize&, HTMLAppletElement*, const URL& baseURL, const Vector<String>& paramNames, const Vector<String>& paramValues) OVERRIDE;

    virtual ObjectContentType objectContentType(const URL&, const String& mimeTypeIn, bool shouldPreferPlugInsForImages) OVERRIDE;
    virtual String overrideMediaType() const OVERRIDE;

    virtual void dispatchDidClearWindowObjectInWorld(DOMWrapperWorld&) OVERRIDE;
    virtual void documentElementAvailable() OVERRIDE;
    virtual void didPerformFirstNavigation() const OVERRIDE;

    virtual void registerForIconNotification(bool) OVERRIDE;

    virtual PassRefPtr<FrameNetworkingContext> createNetworkingContext() OVERRIDE;
	
    const URL& lastRequestedUrl() const { return m_lastRequestedUrl; }
    
	static bool dumpFrameLoaderCallbacks;
    static bool dumpUserGestureInFrameLoaderCallbacks;
    static bool dumpResourceLoadCallbacks;
    static bool dumpResourceResponseMIMETypes;
    static bool sendRequestReturnsNullOnRedirect;
    static bool sendRequestReturnsNull;
    static bool policyDelegateEnabled;
    static bool policyDelegatePermissive;
    static bool deferMainResourceDataLoad;
    static bool dumpHistoryCallbacks;

private:
    Frame *m_frame;
	EA::WebKit::WebFrame *m_webFrame;

    ResourceResponse m_response;

    // Plugin view to redirect data to
    WebCore::PluginView* m_pluginView;
    bool m_hasSentResponseToPlugin;

    // True if makeRepresentation was called.  We don't actually have a concept
    // of a "representation", but we need to know when we're expected to have one.
    // See finishedLoading().
    bool m_hasRepresentation;

	ResourceError m_loadError;
	URL m_lastRequestedUrl;
	
};

}

#endif
