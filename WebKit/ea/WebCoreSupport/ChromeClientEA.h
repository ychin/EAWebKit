/*
 * Copyright (C) 2006 Zack Rusin <zack@kde.org>
 * Copyright (C) 2010 Nokia Corporation and/or its subsidiary(-ies).
 * Copyright (C) 2011, 2012, 2014, 2015 Electronic Arts, Inc. All rights reserved.
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
    virtual void chromeDestroyed() override;

    virtual void setWindowRect(const FloatRect&) override;
    virtual FloatRect windowRect() override;

    virtual FloatRect pageRect() override;

    virtual void focus() override;
    virtual void unfocus() override;

    virtual bool canTakeFocus(FocusDirection) override;
    virtual void takeFocus(FocusDirection) override;

    virtual void focusedElementChanged(Element*) override;
    virtual void focusedFrameChanged(Frame*) override;

    virtual Page* createWindow(Frame*, const FrameLoadRequest&, const WindowFeatures&, const NavigationAction&) override;
    virtual void show() override;

    virtual bool canRunModal() override;
    virtual void runModal() override;

    virtual void setToolbarsVisible(bool) override;
    virtual bool toolbarsVisible() override;

    virtual void setStatusbarVisible(bool) override;
    virtual bool statusbarVisible() override;

    virtual void setScrollbarsVisible(bool) override;
    virtual bool scrollbarsVisible() override;

    virtual void setMenubarVisible(bool) override;
    virtual bool menubarVisible() override;

    virtual void setResizable(bool) override;

    virtual void addMessageToConsole(MessageSource, MessageLevel, const String& message, unsigned lineNumber, unsigned columnNumber, const String& sourceID) override;

    virtual bool canRunBeforeUnloadConfirmPanel() override;
    virtual bool runBeforeUnloadConfirmPanel(const String& message, Frame*) override;

    virtual void closeWindowSoon() override;

    virtual void runJavaScriptAlert(Frame*, const String&) override;
    virtual bool runJavaScriptConfirm(Frame*, const String&) override;
    virtual bool runJavaScriptPrompt(Frame*, const String& message, const String& defaultValue, String& result) override;

    virtual void setStatusbarText(const String&) override;

    virtual KeyboardUIMode keyboardUIMode() override;

    virtual void invalidateRootView(const IntRect&) override;
	virtual void invalidateContentsAndRootView(const IntRect&) override;
    virtual void invalidateContentsForSlowScroll(const IntRect&) override;
    virtual void scroll(const IntSize& scrollDelta, const IntRect& rectToScroll, const IntRect& clipRect) override;
	virtual IntPoint screenToRootView(const IntPoint&) const override;
	virtual IntRect rootViewToScreen(const IntRect&) const override;
	virtual PlatformPageClient platformPageClient() const override;
	virtual void contentsSizeChanged(Frame*, const IntSize&) const override;

    virtual void scrollbarsModeDidChange() const override{ }
    virtual void mouseDidMoveOverElement(const HitTestResult&, unsigned modifierFlags) override;

    virtual void setToolTip(const String&, TextDirection) override;

	virtual void print(Frame*) override;
#if ENABLE(SQL_DATABASE)
	virtual void exceededDatabaseQuota(Frame*, const String&, DatabaseDetails) override;
#endif
    virtual void reachedMaxAppCacheSize(int64_t spaceNeeded) override;
    virtual void reachedApplicationCacheOriginQuota(SecurityOrigin*, int64_t totalSpaceNeeded) override;

#if USE(ACCELERATED_COMPOSITING)
	// This is a hook for WebCore to tell us what we need to do with the GraphicsLayers.
	virtual void attachRootGraphicsLayer(Frame*, GraphicsLayer*) override;
	virtual void attachViewOverlayGraphicsLayer(Frame*, GraphicsLayer*) override {}
	virtual void setNeedsOneShotDrawingSynchronization() override;
	virtual void scheduleCompositingLayerFlush() override;
	virtual CompositingTriggerFlags allowedCompositingTriggers() const override;
	virtual bool allowsAcceleratedCompositing() const override;
#endif

#if USE(COORDINATED_GRAPHICS)
    virtual IntRect visibleRectForTiledBackingStore() const override;
#endif

#if ENABLE(TOUCH_EVENTS)
    virtual void needTouchEvents(bool) override { }
#endif



    virtual void runOpenPanel(Frame*, PassRefPtr<FileChooser>) override;
    virtual void loadIconForFiles(const Vector<String>&, FileIconLoader*) override;

    virtual void setCursor(const Cursor&) override;
    virtual void setCursorHiddenUntilMouseMoves(bool)  override{ }

#if ENABLE(REQUEST_ANIMATION_FRAME) && !USE(REQUEST_ANIMATION_FRAME_TIMER)
    virtual void scheduleAnimation() override;
#endif

    virtual void scrollRectIntoView(const IntRect&) const  override{ }

    virtual bool selectItemWritingDirectionIsNatural() override;
    virtual bool selectItemAlignmentFollowsMenuWritingDirection() override;
	virtual bool hasOpenedPopup() const  override;
    virtual RefPtr<PopupMenu> createPopupMenu(PopupMenuClient*)  const  override;
    virtual RefPtr<SearchPopupMenu> createSearchPopupMenu(PopupMenuClient*)  const  override;

	virtual void dispatchViewportPropertiesDidChange(const ViewportArguments&) const override;

	virtual void wheelEventHandlersChanged(bool)  override{ }
    

	EA::WebKit::WebPage* m_webPage;
    WebCore::URL lastHoverURL;
    WTF::String lastHoverTitle;
    WTF::String lastHoverContent;

    bool toolBarsVisible;
    bool statusBarVisible;
    bool menuBarVisible;

    static bool dumpVisitedLinksCallbacks;
#if USE(ACCELERATED_COMPOSITING)
	std::unique_ptr<TextureMapperLayerClientEA> m_textureMapperLayerClient;
#endif
};
}

#endif
