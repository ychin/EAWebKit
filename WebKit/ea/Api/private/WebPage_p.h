/*
    Copyright (C) 2008, 2009 Nokia Corporation and/or its subsidiary(-ies)
    Copyright (C) 2008 Holger Hans Peter Freyther
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

#ifndef WEBPAGE_P_H
#define WEBPAGE_P_H

#include "WebPage.h"
#include "WebFrame.h"

#include "IntPoint.h"
#include "URL.h"


#include <wtf/OwnPtr.h>
#include <wtf/RefPtr.h>

#include "ViewportArguments.h"

namespace WebCore 
{
    class ChromeClientEA;
    class ContextMenuClientEA;
    class ContextMenuItem;
    class ContextMenu;
    class Document;
    class EditorClientEA;
    class Element;
    class InspectorController;
    class IntRect;
    class Node;
    class NodeList;
    class Page;
    class Frame;
	class ResourceRequest;
}


class WebPageClient;
namespace EA { namespace WebKit 
{
	class WebInspector;
	class View;
}}

#include <EAWebKit/EAWebKitInput.h>
namespace EA
{
namespace WebKit
{
class ViewportAttributesPrivate
{
public:
	ViewportAttributesPrivate(WebPage::ViewportAttributes* attr)
		: vpAttr(attr)
	{ }

	WebPage::ViewportAttributes* vpAttr;
};

class WebPagePrivate 
{
public:
	WebPagePrivate(WebPage*);
	~WebPagePrivate();

	static WebCore::Page* core(const WebPage*);
	static WebPagePrivate* priv(WebPage*);

	void createMainFrame();

	void privOnLoadProgressChanged(int);
	
	void updateAction(WebAction action);
	void updateNavigationActions();
	void updateEditorActions();


	bool mouseMoveEvent(const EA::WebKit::MouseMoveEvent& ev);
	bool mousePressEvent(const EA::WebKit::MouseButtonEvent& ev);
	bool mouseReleaseEvent(const EA::WebKit::MouseButtonEvent& ev);

	bool wheelEvent(const EA::WebKit::MouseWheelEvent& ev);
	
	void keyPressEvent(const EA::WebKit::KeyboardEvent& ev);
	void keyReleaseEvent(const EA::WebKit::KeyboardEvent& ev);
	void focusInEvent();
	void focusOutEvent();

	bool handleScrolling(const EA::WebKit::KeyboardEvent*, WebCore::Frame*);

	void adjustPointForClicking(const EA::WebKit::MouseButtonEvent& ev);

	WebCore::ViewportArguments viewportArguments();

	static WebAction editorActionForKeyEvent(const EA::WebKit::KeyboardEvent* kbEvent);
	static const char* editorCommandForWebActions(WebAction action);

	WebPage* webPage;
	WebCore::Page* page;
	OwnPtr<WebPageClient> client;
	WebFrame* mainFrame;

	View* webView;

	bool insideOpenCall;
	uint64_t m_totalBytes;
	uint64_t m_bytesReceived;

	bool mouseCausedEventActive;
	bool clickCausedFocus;

	bool acceptNavigationRequest(WebFrame* frame, const WebCore::ResourceRequest& request, WebPage::NavigationType type);

	bool forwardUnsupportedContent;
	bool smartInsertDeleteEnabled;
	bool selectTrailingWhitespaceEnabled;
	WebPage::LinkDelegationPolicy linkPolicy;

	WebCore::IntSize viewportSize;
	WebCore::IntSize fixedLayoutSize;
	WebHitTestResult hitTestResult;

	bool useFixedLayout;

    //Add an Action class here?

    static bool drtRun;

    WebInspector *mInspector;
};

}
}

#endif //WEBPAGE_P_H
