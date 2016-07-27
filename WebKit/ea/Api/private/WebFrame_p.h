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

#ifndef WEBFRAME_P_H
#define WEBFRAME_P_H

#include "webframe.h"
#include "mainframe.h"
#include "webpage_p.h"
#include "Node.h"

#include "EventHandler.h"
#include "GraphicsContext.h"
#include "URL.h"

#include "wtf/RefPtr.h"
#include "RefPtrCairo.h"
#include "Frame.h"
#include "ViewportArguments.h"

#include <EAWebkit/EAWebKitHardwareRenderer.h>

namespace WebCore 
{
    class FrameLoaderClientEA;
    class FrameView;
    class HTMLFrameOwnerElement;
    class Scrollbar;
    class TextureMapperContentLayer;
}

namespace EA
{
namespace WebKit
{

class WebPage;

class WebFrameData 
{
public:
	WebFrameData(WebCore::Page*, WebCore::Frame* parentFrame = 0,
		WebCore::HTMLFrameOwnerElement* = 0,
		const WTF::String& frameName = WTF::String());

	WebCore::URL url;
	WTF::String name;
	WebCore::HTMLFrameOwnerElement* ownerElement;
	WebCore::Page* page;
	RefPtr<WebCore::Frame> frame;
	WebCore::FrameLoaderClientEA* frameLoaderClient;

	WTF::String referrer;
	bool allowsScrolling;
	int marginWidth;
	int marginHeight;
};

class WebFramePrivate 
{
public:
	WebFramePrivate()
		: webFrame(0)
		, parentWebFrame(0)
		, horizontalScrollBarPolicy(EA::WebKit::ScrollbarAuto)
		, verticalScrollBarPolicy(EA::WebKit::ScrollbarAuto)
		, frameLoaderClient(0)
		, frame(0)
		, page(0)
		, allowsScrolling(true)
		, marginWidth(-1)
		, marginHeight(-1)
		, mLoadState(WebFrame::kLoadNone)
        , mClearDisplaySurface(false)
        , mHorizontalScroll(NULL)
        , mVerticalScroll(NULL)
        , mScrollCorner(NULL)
	{
	}
	void init(WebFrame* frame, WebFrameData* frameData);
	void setPage(WebPage*);

	inline WebFrame* parentFrame() 
	{ 
		return parentWebFrame;  
	}

	inline void setParentFrame(WebFrame* wFrame)
	{
		parentWebFrame = wFrame;
	}

    WebCore::Scrollbar* horizontalScrollBar() const;
    WebCore::Scrollbar* verticalScrollBar() const;

	static WebCore::Frame* core(const WebFrame*);
	static WebFrame* kit(const WebCore::Frame*);
    static WebFrame* kit(const WebCore::MainFrame&);

	void urlChanged();

	WebFrame* webFrame;
	WebFrame* parentWebFrame;
	EA::WebKit::ScrollbarPolicy horizontalScrollBarPolicy;
	EA::WebKit::ScrollbarPolicy verticalScrollBarPolicy;
	WebCore::FrameLoaderClientEA *frameLoaderClient;
	WebCore::Frame *frame;
	WebPage* page;
    WebCore::URL url;

    bool allowsScrolling;
    int marginWidth;
    int marginHeight;
	WebFrame::LoadState mLoadState;

	bool mClearDisplaySurface; // A flag is used because we want the clear to happen from within the render API.
    WebCore::Color mClearDisplayColor;

    ISurface *mHorizontalScroll;
    ISurface *mVerticalScroll;
    ISurface *mScrollCorner;
};

class WebHitTestResultPrivate {
public:
	WebHitTestResultPrivate() : isContentEditable(false), isContentSelected(false), isScrollBar(false) {}
	WebHitTestResultPrivate(const WebCore::HitTestResult &hitTest);

	bool isContentEditable;
	bool isContentSelected;
	bool isScrollBar;
	WebFrame* frame;
	RefPtr<WebCore::Node> innerNode;
	RefPtr<WebCore::Node> innerNonSharedNode;
};

}
}

#endif//WEBFRAME_P_H
