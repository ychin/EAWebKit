/*
 * Copyright (C) 2006 Zack Rusin <zack@kde.org>
 * Copyright (C) 2006, 2011 Apple Inc. All rights reserved.
 * Copyright (C) 2008 Nokia Corporation and/or its subsidiary(-ies)
 * Copyright (C) 2008 Collabora Ltd. All rights reserved.
 * Copyright (C) 2011, 2014, 2015 Electronic Arts, Inc. All rights reserved.
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
#include "PluginViewBase.h"
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
    virtual void frameLoaderDestroyed() override;

	void setFrame(EA::WebKit::WebFrame* webFrame, Frame* frame);

    virtual bool hasWebView() const override; // mainly for assertions

    virtual void makeRepresentation(DocumentLoader*) override{ }
    virtual void forceLayoutForNonHTML() override;

    virtual void setCopiesOnScroll() override;

    virtual void detachedFromParent2() override;
    virtual void detachedFromParent3() override;

    virtual void assignIdentifierToInitialRequest(unsigned long identifier, WebCore::DocumentLoader*, const WebCore::ResourceRequest&) override;

    virtual void dispatchWillSendRequest(WebCore::DocumentLoader*, unsigned long, WebCore::ResourceRequest&, const WebCore::ResourceResponse&) override;
    virtual bool shouldUseCredentialStorage(DocumentLoader*, unsigned long identifier) override;
    virtual void dispatchDidReceiveAuthenticationChallenge(DocumentLoader*, unsigned long identifier, const AuthenticationChallenge&) override;
    virtual void dispatchDidCancelAuthenticationChallenge(DocumentLoader*, unsigned long identifier, const AuthenticationChallenge&) override;
    virtual void dispatchDidReceiveResponse(WebCore::DocumentLoader*, unsigned long, const WebCore::ResourceResponse&) override;
    virtual void dispatchDidReceiveContentLength(WebCore::DocumentLoader*, unsigned long, int) override;
    virtual void dispatchDidFinishLoading(WebCore::DocumentLoader*, unsigned long) override;
    virtual void dispatchDidFailLoading(WebCore::DocumentLoader*, unsigned long, const WebCore::ResourceError&) override;
    virtual bool dispatchDidLoadResourceFromMemoryCache(WebCore::DocumentLoader*, const WebCore::ResourceRequest&, const WebCore::ResourceResponse&, int) override;

    virtual void dispatchDidHandleOnloadEvents() override;
    virtual void dispatchDidReceiveServerRedirectForProvisionalLoad() override;
    virtual void dispatchDidCancelClientRedirect() override;
    virtual void dispatchWillPerformClientRedirect(const URL&, double interval, double fireDate) override;
    virtual void dispatchDidNavigateWithinPage() override;
    virtual void dispatchDidChangeLocationWithinPage() override;
    virtual void dispatchDidPushStateWithinPage() override;
    virtual void dispatchDidReplaceStateWithinPage() override;
    virtual void dispatchDidPopStateWithinPage() override;
    virtual void dispatchWillClose() override;
    virtual void dispatchDidReceiveIcon() override;
    virtual void dispatchDidStartProvisionalLoad() override;
    virtual void dispatchDidReceiveTitle(const StringWithDirection&) override;
    virtual void dispatchDidChangeIcons(WebCore::IconType) override;
    virtual void dispatchDidCommitLoad() override;
    virtual void dispatchDidFailProvisionalLoad(const ResourceError&) override;
    virtual void dispatchDidFailLoad(const WebCore::ResourceError&) override;
    virtual void dispatchDidFinishDocumentLoad() override;
    virtual void dispatchDidFinishLoad() override;
    virtual void dispatchDidLayout(WebCore::LayoutMilestones) override;

    virtual WebCore::Frame* dispatchCreatePage(const WebCore::NavigationAction&) override;
    virtual void dispatchShow() override;

    virtual void dispatchDecidePolicyForResponse(const ResourceResponse&, const ResourceRequest&, FramePolicyFunction) override;
    virtual void dispatchDecidePolicyForNewWindowAction(const WebCore::NavigationAction&, const WebCore::ResourceRequest&, PassRefPtr<FormState>, const WTF::String&,FramePolicyFunction) override;
    virtual void dispatchDecidePolicyForNavigationAction(const WebCore::NavigationAction&, const WebCore::ResourceRequest&, PassRefPtr<FormState>,FramePolicyFunction) override;
    virtual void cancelPolicyCheck() override;

    virtual void dispatchUnableToImplementPolicy(const WebCore::ResourceError&) override;

    virtual void dispatchWillSendSubmitEvent(PassRefPtr<FormState>) override{ }
    virtual void dispatchWillSubmitForm(PassRefPtr<FormState>,FramePolicyFunction) override;

    virtual void revertToProvisionalState(DocumentLoader*) override{ }
    virtual void setMainDocumentError(DocumentLoader*, const ResourceError&) override;

    virtual void setMainFrameDocumentReady(bool) override;

    virtual void startDownload(const WebCore::ResourceRequest&, const String& suggestedName = String()) override;

    virtual void willChangeTitle(DocumentLoader*) override;
    virtual void didChangeTitle(DocumentLoader*) override;

	virtual void willReplaceMultipartContent() override { }
	virtual void didReplaceMultipartContent() override { }

    virtual void committedLoad(WebCore::DocumentLoader*, const char*, int) override;
    virtual void finishedLoading(DocumentLoader*) override;

    virtual void updateGlobalHistory() override;
    virtual void updateGlobalHistoryRedirectLinks() override;
    virtual bool shouldGoToHistoryItem(HistoryItem*) const override;
    virtual void didDisplayInsecureContent() override;
    virtual void didRunInsecureContent(SecurityOrigin*, const URL&) override;
    virtual void didDetectXSS(const URL&, bool didBlockEntirePage) override;

    virtual ResourceError cancelledError(const ResourceRequest&) override;
    virtual ResourceError blockedError(const ResourceRequest&) override;
    virtual ResourceError cannotShowURLError(const ResourceRequest&) override;
    virtual ResourceError interruptedForPolicyChangeError(const ResourceRequest&) override;

    virtual ResourceError cannotShowMIMETypeError(const ResourceResponse&) override;
    virtual ResourceError fileDoesNotExistError(const ResourceResponse&) override;
    virtual ResourceError pluginWillHandleLoadError(const ResourceResponse&) override;

    virtual bool shouldFallBack(const ResourceError&) override;

    virtual bool canHandleRequest(const WebCore::ResourceRequest&) const override;
    virtual bool canShowMIMEType(const String& MIMEType) const override;
    virtual bool canShowMIMETypeAsHTML(const String& MIMEType) const override;
    virtual bool representationExistsForURLScheme(const String& URLScheme) const override;
    virtual String generatedMIMETypeForURLScheme(const String& URLScheme) const override;

    virtual void frameLoadCompleted() override;
    virtual void saveViewStateToItem(WebCore::HistoryItem*) override;
    virtual void restoreViewState() override;
    virtual void provisionalLoadStarted() override;
    virtual void didFinishLoad() override;
    virtual void prepareForDataSourceReplacement() override;

    virtual WTF::Ref<WebCore::DocumentLoader> createDocumentLoader(const WebCore::ResourceRequest&, const WebCore::SubstituteData&) override;
	virtual void updateCachedDocumentLoader(WebCore::DocumentLoader&) override { }
    virtual void setTitle(const StringWithDirection&, const URL&) override;

    virtual String userAgent(const WebCore::URL&) override;

    virtual void savePlatformDataToCachedFrame(WebCore::CachedFrame*) override;
    virtual void transitionToCommittedFromCachedFrame(WebCore::CachedFrame*) override;
    virtual void transitionToCommittedForNewPage() override;

    virtual void didSaveToPageCache() override;
    virtual void didRestoreFromPageCache() override;

    virtual void dispatchDidBecomeFrameset(bool) override;

    virtual bool canCachePage() const override;
    virtual void convertMainResourceLoadToDownload(WebCore::DocumentLoader*, const WebCore::ResourceRequest&, const WebCore::ResourceResponse&) override;

    virtual RefPtr<Frame> createFrame(const URL&, const String& name, HTMLFrameOwnerElement*, const String& referrer, bool allowsScrolling, int marginWidth, int marginHeight) override;
    virtual RefPtr<Widget> createPlugin(const IntSize&, HTMLPlugInElement*, const URL&, const Vector<String>&, const Vector<String>&, const String&, bool) override;
    virtual void recreatePlugin(Widget*) override{ }
    virtual void redirectDataToPlugin(Widget* pluginWidget) override;

    virtual PassRefPtr<Widget> createJavaAppletWidget(const IntSize&, HTMLAppletElement*, const URL& baseURL, const Vector<String>& paramNames, const Vector<String>& paramValues) override;

    virtual ObjectContentType objectContentType(const URL&, const String& mimeTypeIn, bool shouldPreferPlugInsForImages) override;
    virtual String overrideMediaType() const override;

    virtual void dispatchDidClearWindowObjectInWorld(DOMWrapperWorld&) override;

    virtual void registerForIconNotification(bool) override;

    virtual PassRefPtr<FrameNetworkingContext> createNetworkingContext() override;
	
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
    WebCore::PluginViewBase* m_pluginView;
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
