/*
 * Copyright (C) 2006 Zack Rusin <zack@kde.org>
 * Copyright (C) 2010 Nokia Corporation and/or its subsidiary(-ies).
 * Copyright (C) 2011, 2012, 2014 Electronic Arts, Inc. All rights reserved.
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

#ifndef ChromeClientEA_H
#define ChromeClientEA_H

#include "ChromeClient.h"
#include "FloatRect.h"
#include "URL.h"
#include <wtf/PassOwnPtr.h>
#include <wtf/RefCounted.h>
#include <wtf/text/WTFString.h>

namespace EA
{
namespace WebKit
{
class WebPage;
}
}

namespace WebCore 
{

class FileChooser;
class FloatRect;
class Page;
class RefreshAnimation;
struct FrameLoadRequest;
struct ViewportArguments;
class TextureMapperLayerClientEA;

class ChromeClientEA : public ChromeClient
{
public:
	ChromeClientEA(EA::WebKit::WebPage* webPage);
    virtual ~ChromeClientEA();
    virtual void chromeDestroyed() OVERRIDE;

    virtual void setWindowRect(const FloatRect&) OVERRIDE;
    virtual FloatRect windowRect() OVERRIDE;

    virtual FloatRect pageRect() OVERRIDE;

    virtual void focus() OVERRIDE;
    virtual void unfocus() OVERRIDE;

    virtual bool canTakeFocus(FocusDirection) OVERRIDE;
    virtual void takeFocus(FocusDirection) OVERRIDE;

    virtual void focusedElementChanged(Element*) OVERRIDE;
    virtual void focusedFrameChanged(Frame*) OVERRIDE;

    virtual Page* createWindow(Frame*, const FrameLoadRequest&, const WindowFeatures&, const NavigationAction&) OVERRIDE;
    virtual void show() OVERRIDE;

    virtual bool canRunModal() OVERRIDE;
    virtual void runModal() OVERRIDE;

    virtual void setToolbarsVisible(bool) OVERRIDE;
    virtual bool toolbarsVisible() OVERRIDE;

    virtual void setStatusbarVisible(bool) OVERRIDE;
    virtual bool statusbarVisible() OVERRIDE;

    virtual void setScrollbarsVisible(bool) OVERRIDE;
    virtual bool scrollbarsVisible() OVERRIDE;

    virtual void setMenubarVisible(bool) OVERRIDE;
    virtual bool menubarVisible() OVERRIDE;

    virtual void setResizable(bool) OVERRIDE;

    virtual void addMessageToConsole(MessageSource, MessageLevel, const String& message, unsigned lineNumber, unsigned columnNumber, const String& sourceID) OVERRIDE;

    virtual bool canRunBeforeUnloadConfirmPanel() OVERRIDE;
    virtual bool runBeforeUnloadConfirmPanel(const String& message, Frame*) OVERRIDE;

    virtual void closeWindowSoon() OVERRIDE;

    virtual void runJavaScriptAlert(Frame*, const String&) OVERRIDE;
    virtual bool runJavaScriptConfirm(Frame*, const String&) OVERRIDE;
    virtual bool runJavaScriptPrompt(Frame*, const String& message, const String& defaultValue, String& result) OVERRIDE;
    virtual bool shouldInterruptJavaScript() OVERRIDE;

    virtual void setStatusbarText(const String&) OVERRIDE;

    virtual KeyboardUIMode keyboardUIMode() OVERRIDE;
    virtual IntRect windowResizerRect() const OVERRIDE;

    virtual void invalidateRootView(const IntRect&, bool) OVERRIDE;
	virtual void invalidateContentsAndRootView(const IntRect&, bool) OVERRIDE;
    virtual void invalidateContentsForSlowScroll(const IntRect&, bool) OVERRIDE;
    virtual void scroll(const IntSize& scrollDelta, const IntRect& rectToScroll, const IntRect& clipRect) OVERRIDE;
#if USE(TILED_BACKING_STORE)
    virtual void delegatedScrollRequested(const IntPoint& scrollPoint) OVERRIDE;
#endif

	virtual IntPoint screenToRootView(const IntPoint&) const OVERRIDE;
	virtual IntRect rootViewToScreen(const IntRect&) const OVERRIDE;
	virtual PlatformPageClient platformPageClient() const OVERRIDE;
	virtual void contentsSizeChanged(Frame*, const IntSize&) const OVERRIDE;

    virtual void scrollbarsModeDidChange() const OVERRIDE{ }
    virtual void mouseDidMoveOverElement(const HitTestResult&, unsigned modifierFlags) OVERRIDE;

    virtual void setToolTip(const String&, TextDirection) OVERRIDE;

	virtual void print(Frame*);
#if ENABLE(SQL_DATABASE)
	virtual void exceededDatabaseQuota(Frame*, const String&, DatabaseDetails) OVERRIDE;
#endif
    virtual void reachedMaxAppCacheSize(int64_t spaceNeeded) OVERRIDE;
    virtual void reachedApplicationCacheOriginQuota(SecurityOrigin*, int64_t totalSpaceNeeded) OVERRIDE;

#if USE(ACCELERATED_COMPOSITING)
	// This is a hook for WebCore to tell us what we need to do with the GraphicsLayers.
	virtual void attachRootGraphicsLayer(Frame*, GraphicsLayer*) OVERRIDE;
	virtual void setNeedsOneShotDrawingSynchronization() OVERRIDE;
	virtual void scheduleCompositingLayerFlush() OVERRIDE;
	virtual CompositingTriggerFlags allowedCompositingTriggers() const OVERRIDE;
	virtual bool allowsAcceleratedCompositing() const OVERRIDE;
#endif

#if USE(TILED_BACKING_STORE)
    virtual IntRect visibleRectForTiledBackingStore() const OVERRIDE;
#endif

#if ENABLE(TOUCH_EVENTS)
    virtual void needTouchEvents(bool) OVERRIDE { }
#endif



    virtual void runOpenPanel(Frame*, PassRefPtr<FileChooser>) OVERRIDE;
    virtual void loadIconForFiles(const Vector<String>&, FileIconLoader*) OVERRIDE;

    virtual void formStateDidChange(const Node*)  OVERRIDE{ }

    virtual void setCursor(const Cursor&) OVERRIDE;
    virtual void setCursorHiddenUntilMouseMoves(bool)  OVERRIDE{ }

#if ENABLE(REQUEST_ANIMATION_FRAME) && !USE(REQUEST_ANIMATION_FRAME_TIMER)
    virtual void scheduleAnimation() OVERRIDE;
#endif

    virtual void scrollRectIntoView(const IntRect&) const  OVERRIDE{ }

    virtual bool selectItemWritingDirectionIsNatural() OVERRIDE;
    virtual bool selectItemAlignmentFollowsMenuWritingDirection() OVERRIDE;
	virtual bool hasOpenedPopup() const  OVERRIDE;
    virtual PassRefPtr<PopupMenu> createPopupMenu(PopupMenuClient*)  const  OVERRIDE;
    virtual PassRefPtr<SearchPopupMenu> createSearchPopupMenu(PopupMenuClient*)  const  OVERRIDE;
    virtual void populateVisitedLinks()  OVERRIDE;

	virtual void dispatchViewportPropertiesDidChange(const ViewportArguments&) const OVERRIDE;

	virtual void numWheelEventHandlersChanged(unsigned)  OVERRIDE{ }
    

	EA::WebKit::WebPage* m_webPage;
    WebCore::URL lastHoverURL;
    WTF::String lastHoverTitle;
    WTF::String lastHoverContent;

    bool toolBarsVisible;
    bool statusBarVisible;
    bool menuBarVisible;

    static bool dumpVisitedLinksCallbacks;
#if USE(ACCELERATED_COMPOSITING)
     OwnPtr<TextureMapperLayerClientEA> m_textureMapperLayerClient;
#endif
};
}

#endif
