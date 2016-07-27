/*
    Copyright (C) 2008 Nokia Corporation and/or its subsidiary(-ies)
    Copyright (C) 2007 Staikos Computing Services Inc.
	Copyright (C) 2011, 2012, 2014 Electronic Arts, Inc. All rights reserved.

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

#ifndef WEBPAGE_H
#define WEBPAGE_H

namespace WebCore 
{
    class ChromeClientEA;
    class EditorClientEA;
    class FrameLoaderClientEA;
    class InspectorClientEA;
    class InspectorFrontendClientEA;
    class NotificationPresenterClientEA;
    class GeolocationPermissionClientEA;
    class ResourceHandle;
    class HitTestResult;
	class URL;
	class ResourceRequest;

    struct FrameLoadRequest;
}

#include <EAWebKit/EAWebKitSystem.h>
#include <EAWebKit/EAWebKitConfig.h>

#include <wtf/text/WTFString.h>
#include "IntPoint.h"
#include "IntSize.h"
#include "IntRect.h"

#if ENABLE(INSPECTOR_SERVER) && ENABLE(INSPECTOR)
	#include "InspectorFrontendChannel.h"
#endif

namespace EA { namespace WebKit
{

class View;
class WebFrame;
class WebPagePrivate;
class ViewportAttributesPrivate;
class WebInspector;

class WebPage 
#if ENABLE(INSPECTOR_SERVER) && ENABLE(INSPECTOR)
    : public WebCore::InspectorFrontendChannel
#endif
{
public:
	enum NavigationType 
	{
		NavigationTypeLinkClicked,
		NavigationTypeFormSubmitted,
		NavigationTypeBackOrForward,
		NavigationTypeReload,
		NavigationTypeFormResubmitted,
		NavigationTypeOther
	};

	enum LinkDelegationPolicy 
	{
		DontDelegateLinks,
		DelegateExternalLinks,
		DelegateAllLinks
	};

	enum WebWindowType 
	{
		WebBrowserWindow,
		WebModalDialog
	};

	enum PermissionPolicy 
	{
		PermissionUnknown,
		PermissionGrantedByUser,
		PermissionDeniedByUser
	};

	enum Feature 
	{
		Notifications,
		Geolocation
	};

	class ViewportAttributes 
	{
	public:
		ViewportAttributes();
		ViewportAttributes(const WebPage::ViewportAttributes& other);

		~ViewportAttributes();

		WebPage::ViewportAttributes& operator=(const WebPage::ViewportAttributes& other);

		inline float initialScaleFactor() const { return m_initialScaleFactor; }
		inline float minimumScaleFactor() const { return m_minimumScaleFactor; }
		inline float maximumScaleFactor() const { return m_maximumScaleFactor; }
		inline float devicePixelRatio() const { return m_devicePixelRatio; }
		inline bool isUserScalable() const { return m_isUserScalable; }
		inline bool isValid() const { return m_isValid; }
		inline WebCore::IntSize size() const { return m_size; }

	private:
		ViewportAttributesPrivate* d;
		float m_initialScaleFactor;
		float m_minimumScaleFactor;
		float m_maximumScaleFactor;
		float m_devicePixelRatio;
		bool m_isUserScalable;
		bool m_isValid;
		WebCore::IntSize m_size;

		friend class WebCore::ChromeClientEA;
		friend class WebPage;
	};


	explicit WebPage(View* pView);
	~WebPage();

	WebFrame* mainFrame() const;
	WebFrame* currentFrame() const;
	WebFrame* focusedFrame() const;
	WebFrame* frameAt(const WebCore::IntPoint& pos) const;

	void setView(View* pView);
	View* view() const;

	uint64_t totalBytes() const;
	uint64_t bytesReceived() const;

	bool hasSelection() const;
	WTF::String selectedText() const;
	WTF::String selectedHtml() const;

    void triggerAction(WebAction action, bool checked = false);

	WebCore::IntSize viewportSize() const;
	void setViewportSize(const WebCore::IntSize &size) const;

	WebCore::IntSize preferredContentsSize() const;
	void setPreferredContentsSize(const WebCore::IntSize& size) const;
	void setActualVisibleContentRect(const WebCore::IntRect& rect) const;
    
#if 0 //A easy wrapper around tab functionality. I am not sure
	// if the impl needs to send a key up event as well.
	bool focusNextPrevChild(bool next);
#endif
	void setForwardUnsupportedContent(bool forward);
	bool forwardUnsupportedContent() const;

	void setLinkDelegationPolicy(LinkDelegationPolicy policy);
	LinkDelegationPolicy linkDelegationPolicy() const;

	inline WebPagePrivate* handle() const { return d; }
	
	//Interesting example of how to may be open link in an external browser
	bool acceptNavigationRequest(WebFrame *frame, const WebCore::ResourceRequest &request, WebPage::NavigationType type);
	WTF::String userAgentForUrl(const WebCore::URL& url) const;

    void SetInspectorDisplay(bool show);

    WebInspector *GetInspector(void);
    void DestroyInspector(void);

#if ENABLE(INSPECTOR_SERVER) && ENABLE(INSPECTOR)
    void remoteFrontendConnected();
    void remoteFrontendDisconnected();
    void dispatchMessageFromRemoteFrontend(const String& message);
    virtual bool sendMessageToFrontend(const String& message);
#endif


private:
	WebPagePrivate *d;
#if ENABLE(INSPECTOR_SERVER) && ENABLE(INSPECTOR)
    int32_t mRemoteInspectorId;
#endif
	friend class WebFrame;
	friend class WebPagePrivate;
	friend class View;
	friend struct ViewPrivate;
	friend class GraphicsView;
	friend class GraphicsViewPrivate;
	friend class WebInspector;
	friend class WebCore::ChromeClientEA;
	friend class WebCore::EditorClientEA;
	friend class WebCore::FrameLoaderClientEA;
	friend class WebCore::ResourceHandle;
	friend class DumpRenderTreeSupportEA;
    friend class WebCore::InspectorFrontendClientEA;
    friend class WebCore::InspectorClientEA;
};

}}

#endif//WEBPAGE_H
