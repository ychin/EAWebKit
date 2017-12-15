/*
 * Copyright (C) 2006 Zack Rusin <zack@kde.org>
 * Copyright (C) 2006 Apple Computer, Inc.  All rights reserved.
 * Copyright (C) 2010 Nokia Corporation and/or its subsidiary(-ies).
 * Copyright (C) 2011, 2012, 2013, 2014, 2015 Electronic Arts, Inc. All rights reserved.
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
#include "ChromeClientEA.h"

#include "ApplicationCacheStorage.h"
#include "ColorChooser.h"
#include "ColorChooserClient.h"
#include "DatabaseManager.h"
#include "Document.h"
#include "FileChooser.h"
#include "FileIconLoader.h"
#include "FrameLoadRequest.h"
#include "FrameLoader.h"
#include "FrameLoaderClientEA.h"
#include "FrameView.h"
#include "Geolocation.h"
#include "HTMLFormElement.h"
#include "HitTestResult.h"
#include "Icon.h"
#include "MainFrame.h"
#include "NavigationAction.h"
#include "NetworkingContext.h"
#include "NotImplemented.h"
#include "Page.h"
#include "PageClientEA.h"
#include "PopupMenuEA.h"
#include "WebPageClientEA.h"
#include "ScrollbarTheme.h"
#include "SearchPopupMenuEA.h"
#include "SecurityOrigin.h"
#include "ViewportArguments.h"
#include "WindowFeatures.h"

#include "webframe_p.h"
#include "webpage.h"
#include "webpage_p.h"
#include <wtf/CurrentTime.h>
#include <wtf/OwnPtr.h>
#include "HTMLNames.h"

#if USE(ACCELERATED_COMPOSITING)
#include "GraphicsLayer.h"
#endif

#include <EAWebKit/EAWebKitView.h>
#include <EAWebKit/EAWebKitClient.h>
#include <EAWebKit/EAWebKit.h>
#include <internal/include/EAWebKit_p.h>
#include <internal/include/EAWebKitAssert.h>
#include <internal/include/EAWebkitEASTLHelpers.h>

//+EAWebKitChange
//1/16/2015
namespace EA
{
	namespace WebKit
	{
        bool DoCssFilterInHardware();
	}
}
//-EAWebKitChange

// Note by Arpit Baldeva: 
// Most of the functionality in this file is related to the window object in the JavaScript API. For example, window.open()/window.close().
// So more like multiple Windows (as opposed to multiple Views).
// The actual creation of the window(also known as Chrome in WebCore - Not to be confused with Google Chrome) is done through this client.
namespace WebCore {

bool ChromeClientEA::dumpVisitedLinksCallbacks = false;

ChromeClientEA::ChromeClientEA(EA::WebKit::WebPage* webPage)
    : m_webPage(webPage)
{
    //abaldeva: By default, we don't show these. This is part of JavaScript API for window.open call.
	toolBarsVisible = statusBarVisible = menuBarVisible = false;
}

ChromeClientEA::~ChromeClientEA()
{

}

void ChromeClientEA::setWindowRect(const FloatRect& rect)
{
	// This function is called whenever the document wants to change the position and size of the window. For example, this can happen 
	// through window.MoveBy call or window.open call.
	notImplemented();
}

/*!
    windowRect represents the rect of the Window, including all interface elements
    like toolbars/scrollbars etc. It is used by the viewport meta tag as well as
    by the DOM Window object: outerHeight(), outerWidth(), screenX(), screenY().
*/
FloatRect ChromeClientEA::windowRect()
{
	if (!platformPageClient())
        return FloatRect();
    return platformPageClient()->windowRect();
}



FloatRect ChromeClientEA::pageRect()
{
    if (!m_webPage)
        return FloatRect();
    return FloatRect(0.0f, 0.0f, m_webPage->viewportSize().width(), m_webPage->viewportSize().height());
}




void ChromeClientEA::focus()
{
    // called on window.focus().
}


void ChromeClientEA::unfocus()
{
    //called on window.unfocus()
}

bool ChromeClientEA::canTakeFocus(FocusDirection)
{
    // This is called when cycling through links/focusable objects and we
    // reach the last focusable object. 
	
	// If Chrome/Window wants to claim the focus without wrapping, return true.
	// return true;

    // It may be beneficial for our usage to let it wrap around since our users won't have a concept of Window.
	return false;
}

void ChromeClientEA::takeFocus(FocusDirection)
{
    // Evaluate and provide appropriate impl.
	notImplemented();

}

// Can be useful in some contexts.
void ChromeClientEA::focusedElementChanged(Element*)
{
}

void ChromeClientEA::focusedFrameChanged(Frame*)
{
}

Page* ChromeClientEA::createWindow(Frame*, const FrameLoadRequest& request, const WindowFeatures& features, const NavigationAction& action)
{
	if(features.dialog)
	{
		EAW_ASSERT_MSG(false, " Modal dialogs are currently neither created nor handled\n");
		return 0;
	}

	Page* page = 0;
	using namespace EA::WebKit;

	CreateViewInfo createViewInfo;
	if(EAWebKitClient* const pClient = GetEAWebKitClient(m_webPage->view()))
	{
		createViewInfo.mpView = m_webPage->view();
		createViewInfo.mpUserData = createViewInfo.mpView->GetUserData();

		if(features.xSet)
			createViewInfo.mLeft = (uint16_t)features.x;

		if(features.ySet)
			createViewInfo.mTop	= (uint16_t)features.y;

		if(features.widthSet)
			createViewInfo.mWidth = (uint16_t)features.width;

		if(features.heightSet)
			createViewInfo.mHeight = (uint16_t)features.height;

		createViewInfo.mResizable = features.resizable;
		createViewInfo.mScrollBars = features.scrollbarsVisible;

		createViewInfo.mEventType = EA::WebKit::CreateViewInfo::kEventWindowOpen;
		
		const WTF::String& urlString = request.resourceRequest().url().string();
		EA::WebKit::FixedString16_128 urlToOpen(urlString.characters(),urlString.length());
		createViewInfo.mpURLToOpen = urlToOpen.c_str();
		
		pClient->CreateView(createViewInfo);
 		if(createViewInfo.mpCreatedView)
 		{
			page = WebPagePrivate::core(createViewInfo.mpCreatedView->Page());
		}
	}
 	
 	return page;

}

void ChromeClientEA::show()
{
    if (!m_webPage)
        return;

    //Bring up this window to the front.
}


bool ChromeClientEA::canRunModal()
{
    return false; //abaldeva: We don't support this.
}


void ChromeClientEA::runModal()
{
}

// Following are called for the JavaScript window.open call which allows for few configuration variables when creating
// a window.
void ChromeClientEA::setToolbarsVisible(bool visible)
{
    toolBarsVisible = visible;
}


bool ChromeClientEA::toolbarsVisible()
{
    return toolBarsVisible;
}


void ChromeClientEA::setStatusbarVisible(bool visible)
{
    statusBarVisible = visible;
}

bool ChromeClientEA::statusbarVisible()
{
    return statusBarVisible;
}


void ChromeClientEA::setScrollbarsVisible(bool)
{
    notImplemented();
}


bool ChromeClientEA::scrollbarsVisible()
{
    notImplemented();
    return true;
}


void ChromeClientEA::setMenubarVisible(bool visible)
{
    menuBarVisible = visible;
}

bool ChromeClientEA::menubarVisible()
{
    return menuBarVisible;
}

void ChromeClientEA::setResizable(bool)
{
    notImplemented();
}

void ChromeClientEA::addMessageToConsole(MessageSource, MessageLevel level, const String& message, unsigned lineNumber, unsigned columnNumber, const String& sourceID)
{
    if (EA::WebKit::EAWebKitClient *pClient = EA::WebKit::GetEAWebKitClient(m_webPage->view()))
    {
        EA::WebKit::DebugLogInfo dli;
        dli.mpView = m_webPage->view();
        dli.mpUserData = dli.mpView->GetUserData();
        dli.mType = EA::WebKit::kDebugLogJavascript;
        
        CString cMessage = message.ascii();
        CString cId = sourceID.ascii();

        eastl::string8 prefix;
        switch (level)
        {
		case WarningMessageLevel: 
			prefix = "EAWebKit: JS Warning - "; 
			break;
		case ErrorMessageLevel: 
			prefix = "EAWebKit: JS Error -"; 
			break;
        default: 
			prefix = "EAWebKit: JS -"; break;
        }
        
        eastl::string8 logMessage;
        logMessage.sprintf("%s %s Line-%u Column-%u Message: %s", prefix.c_str(), cId.data(), lineNumber, columnNumber, cMessage.data());
        dli.mpLogText = logMessage.c_str();

        pClient->DebugLog(dli);
    }
}

void ChromeClientEA::chromeDestroyed()
{
    delete this;
}

bool ChromeClientEA::canRunBeforeUnloadConfirmPanel()
{
    return true;
}

bool ChromeClientEA::runBeforeUnloadConfirmPanel(const String& message, Frame* frame)
{
    return runJavaScriptConfirm(frame, message);
}

// abaldeva: This is called when window.close() is called. This is NOT called when the user clicks the close button on the Window.
void ChromeClientEA::closeWindowSoon()
{
    m_webPage->d->page->setGroupName(String());
	m_webPage->mainFrame()->d->frame->loader().stopAllLoaders();

	if (EA::WebKit::EAWebKitClient *pClient = EA::WebKit::GetEAWebKitClient(m_webPage->view()))
	{
		EA::WebKit::DestroyViewInfo destroyViewInfo;
		destroyViewInfo.mpView = m_webPage->view();
		destroyViewInfo.mpUserData = destroyViewInfo.mpView->GetUserData();
		destroyViewInfo.mEventType = EA::WebKit::DestroyViewInfo::kEventWindowClose;
		pClient->DestroyView(destroyViewInfo);
	}
}

void ChromeClientEA::runJavaScriptAlert(Frame* f, const String& msg)
{
    if (EA::WebKit::EAWebKitClient *pClient = EA::WebKit::GetEAWebKitClient(m_webPage->view()))
    {
        EA::WebKit::JSMessageBoxInfo info;
        info.mpView = m_webPage->view();
        info.mpUserData = info.mpView->GetUserData();
        info.mType = EA::WebKit::kJSMessageBoxAlert;
        
        GetFixedString(info.mMessage)->assign(msg.characters(), msg.length());
        info.mConfirm = false;

        pClient->JSMessageBox(info);
    }
}

bool ChromeClientEA::runJavaScriptConfirm(Frame* f, const String& msg)
{
    if (EA::WebKit::EAWebKitClient *pClient = EA::WebKit::GetEAWebKitClient(m_webPage->view()))
    {
        EA::WebKit::JSMessageBoxInfo info;
        info.mpView = m_webPage->view();
        info.mpUserData = info.mpView->GetUserData();
        info.mType = EA::WebKit::kJSMessageBoxConfirm;

        GetFixedString(info.mMessage)->assign(msg.characters(), msg.length());
        info.mConfirm = false;

        pClient->JSMessageBox(info);

        return info.mConfirm;
    }

    return false;
}

bool ChromeClientEA::runJavaScriptPrompt(Frame* f, const String& message, const String& defaultValue, String& result)
{
    if (EA::WebKit::EAWebKitClient *pClient = EA::WebKit::GetEAWebKitClient(m_webPage->view()))
    {
        EA::WebKit::JSMessageBoxInfo info;
        info.mpView = m_webPage->view();
        info.mpUserData = info.mpView->GetUserData();
        info.mType = EA::WebKit::kJSMessageBoxPrompt;

        GetFixedString(info.mMessage)->assign(message.characters(), message.length());
        info.mConfirm = false;
        GetFixedString(info.mPromptDefault)->assign(defaultValue.characters(), defaultValue.length());

        pClient->JSMessageBox(info);
        
        result = info.mPromptResponse.GetCharacters();

        return info.mConfirm;
    }

    return false;
}

void ChromeClientEA::setStatusbarText(const WTF::String& msg)
{
    // Do nothing.
}

// This notification is called when the JavaScript is unresponsive. We simply let the WatchDog know and it can take the required action.
bool ChromeClientEA::shouldInterruptJavaScript() 
{
	bool shouldInterruptScript = true;
	
	if (EA::WebKit::EAWebKitClient *pClient = EA::WebKit::GetEAWebKitClient(m_webPage->view()))
	{
		EA::WebKit::WatchDogNotificationInfo info;
		info.mpView = m_webPage->view();
		info.mpUserData = info.mpView->GetUserData();
		info.mWatchDogNotificationType = EA::WebKit::WatchDogNotificationInfo::kScriptRunningTooLong;

		pClient->WatchDogNotification(info);

		shouldInterruptScript = info.mInterruptScript;
	}
	
	
	return shouldInterruptScript;
}

KeyboardUIMode ChromeClientEA::keyboardUIMode(void) 
{
	return KeyboardAccessTabsToLinks; //Allows us to tab to links
}


IntRect ChromeClientEA::windowResizerRect() const
{
	notImplemented();
	return IntRect();
}

void ChromeClientEA::invalidateRootView(const IntRect &windowRect, bool)
{
#if USE(TILED_BACKING_STORE)
    if (platformPageClient()) {
        WebCore::TiledBackingStore* backingStore = EA::WebKit::WebFramePrivate::core(m_webPage->mainFrame())->tiledBackingStore();
        if (!backingStore)
            return;
        backingStore->invalidate(windowRect);
    }
#endif
}

void ChromeClientEA::invalidateContentsAndRootView(const IntRect& windowRect, bool immediate)
{
	if (platformPageClient()) {
        IntRect rect(windowRect);
        rect.intersect(IntRect(IntPoint(0, 0), m_webPage->viewportSize()));
        // abaldeva: Send the rect to the View via platformPageClient so that it can paint the window and send the update to the application.
		if (!rect.isEmpty())
			platformPageClient()->update(rect);
    }

    // FIXME: There is no "immediate" support for window painting.  This should be done always whenever the flag
    // is set.
}
// invalidateContentsForSlowScroll is meant to be for slow scroll. Based on various conditions like if the platform is capable of blitting or the
// number of fixed objects, WebCore calls this and sends us the window rect of the dirty region(usually the entire visible screen).
// In software mode, we simply pass on the Dirty region to our dirty region list for a paint.
// In hardware mode(when using Tiled backing storage), we have special case code in platformPageClient()->scroll to invalidate the tiled backing 
// store. This special case code works around the existing bugs in the tiled backing storage backend implementation.
void ChromeClientEA::invalidateContentsForSlowScroll(const IntRect& windowRect, bool immediate)
{
#if USE(TILED_BACKING_STORE)
	if (platformPageClient())
		platformPageClient()->scroll(0, 0, windowRect);
#else
	invalidateContentsAndRootView(windowRect, immediate);
#endif
}

// ChromeClientEA::scroll is meant to be for fast scroll. You receive the scroll amount and the window position of the dirty region. In addition, WebCore code has some
// smarts about fixed elements and takes care of repainting them to a max of 5 objects. If more than 5 are found, it uses the slow scroll path which
// simply involves updating the entire windowRect.
// However, our implementation is not smart - software or hardware(does not blit) and we ultimately end up making the entire window/screen dirty in both the paths.
void ChromeClientEA::scroll(const IntSize& delta, const IntRect& scrollViewRect, const IntRect&)
{
    if (platformPageClient())
        platformPageClient()->scroll(delta.width(), delta.height(), scrollViewRect);
}

#if USE(TILED_BACKING_STORE)
void ChromeClientEA::delegatedScrollRequested(const IntPoint& delta)
{
	EAW_ASSERT_MSG(false, "This method should not be called on our port");
}
#endif

IntRect ChromeClientEA::rootViewToScreen(const IntRect& rect) const
{
	notImplemented();
	return rect;
}

IntPoint ChromeClientEA::screenToRootView(const IntPoint& point) const
{
	notImplemented();
	return point;
}

PlatformPageClient ChromeClientEA::platformPageClient() const
{
    return m_webPage->d->client.get();
}

void ChromeClientEA::contentsSizeChanged(Frame* frame, const IntSize& size) const
{
	notImplemented();
}

void ChromeClientEA::mouseDidMoveOverElement(const HitTestResult& result, unsigned)
{
#if 0
	// This could have interesting usage in UI. On PC, it is mostly used to display info in the
	// status panel.
	TextDirection dir;
    if (result.absoluteLinkURL() != lastHoverURL
        || result.title(dir) != lastHoverTitle
        || result.textContent() != lastHoverContent) {
        lastHoverURL = result.absoluteLinkURL();
        lastHoverTitle = result.title(dir);
        lastHoverContent = result.textContent();
        m_webPage->linkHovered(lastHoverURL.string(),lastHoverTitle, lastHoverContent);
    }
#endif
}

void ChromeClientEA::setToolTip(const String &tip, TextDirection)
{
    if(!EA::WebKit::GetParameters().mDefaultToolTipEnabled)
        return;

    EA::WebKit::View* pView = m_webPage->view();
    String tipText(tip);
    pView->SetToolTip(tipText.charactersWithNullTermination().data()); 
}

void ChromeClientEA::print(Frame* frame)
{
}

#if ENABLE(SQL_DATABASE)
void ChromeClientEA::exceededDatabaseQuota(Frame* frame, const String& databaseName,DatabaseDetails)
{
	uint64_t quota = 50* 1024 * 1024; //50 MB - If needed, we can expose it. This is disk usage, not RAM.

    if (!DatabaseManager::manager().hasEntryForOrigin(frame->document()->securityOrigin()))
        DatabaseManager::manager().setQuota(frame->document()->securityOrigin(), quota);

}
#endif


void ChromeClientEA::reachedMaxAppCacheSize(int64_t size)
{
    // FIXME: Free some space.
    notImplemented();
	// abaldeva: This should not be really fired since the quota is set to ApplicationCacheStorage::noQuota() by default.
	EAW_ASSERT_FORMATTED(false, "Max App Cache Size %lld reached. Need a way to handle this.",size);
}


void ChromeClientEA::reachedApplicationCacheOriginQuota(WebCore::SecurityOrigin* origin,int64_t)
{
    notImplemented();
	// abaldeva: This should not be really fired since the quota is set to ApplicationCacheStorage::noQuota() by default.
	EAW_ASSERT_FORMATTED(false, "Max App Cache Size for %S origin reached. Need a way to handle this.",origin->host().charactersWithNullTermination().data());
}




void ChromeClientEA::runOpenPanel(Frame* frame, PassRefPtr<FileChooser> prpFileChooser)
{
	// abaldeva: This is for the file open dialog so that you can attach a file. We don't support that yet.
	notImplemented();
}

void ChromeClientEA::loadIconForFiles(const Vector<String>&, FileIconLoader*) 
{
}
void ChromeClientEA::setCursor(const Cursor& cursor)
{
    WebPageClient* pageClient = platformPageClient();
	if (!pageClient)
        return;
    pageClient->setCursor(cursor.impl());

}

#if ENABLE(REQUEST_ANIMATION_FRAME) && !USE(REQUEST_ANIMATION_FRAME_TIMER)
void ChromeClientEA::scheduleAnimation(void)
{
    if (!m_webPage) {
        return;
    }

    m_webPage->view()->ScheduleAnimation();
}

#endif
#if USE(ACCELERATED_COMPOSITING)
void ChromeClientEA::attachRootGraphicsLayer(Frame* frame, GraphicsLayer* graphicsLayer)
{
	if (!m_textureMapperLayerClient)
		m_textureMapperLayerClient = adoptPtr(new TextureMapperLayerClientEA(m_webPage->mainFrame()));
	m_textureMapperLayerClient->setRootGraphicsLayer(graphicsLayer);
}

void ChromeClientEA::setNeedsOneShotDrawingSynchronization()
{
	// we want the layers to synchronize next time we update the screen anyway
	if (m_textureMapperLayerClient)
		m_textureMapperLayerClient->markForSync(false);
}

void ChromeClientEA::scheduleCompositingLayerFlush()
{
	// we want the layers to synchronize ASAP
	if (m_textureMapperLayerClient)
		m_textureMapperLayerClient->markForSync(true);
}

ChromeClient::CompositingTriggerFlags ChromeClientEA::allowedCompositingTriggers() const
{
	ChromeClient::CompositingTriggerFlags flags = 0;
	if (allowsAcceleratedCompositing())
		flags |= ThreeDTransformTrigger | CanvasTrigger | AnimationTrigger | AnimatedOpacityTrigger;

    if(m_webPage->view()->GetHardwareRenderer() && EA::WebKit::DoCssFilterInHardware())
		flags |= FilterTrigger;

	return flags;
}

bool ChromeClientEA::allowsAcceleratedCompositing() const
{
	if(m_webPage->view()->HardwareAccelerated())
		return true;

	if(m_webPage->view()->IsUsingCPUCompositing())
		return true;

	return false;
}
#endif


#if USE(TILED_BACKING_STORE)
IntRect ChromeClientEA::visibleRectForTiledBackingStore() const
{
    if (!platformPageClient() || !m_webPage)
        return IntRect();

    if (!platformPageClient()->viewResizesToContentsEnabled())
        return WebCore::IntRect(m_webPage->mainFrame()->scrollPosition(), m_webPage->mainFrame()->geometry().size());

    return enclosingIntRect(FloatRect(platformPageClient()->graphicsItemVisibleRect()));
}
#endif

void ChromeClientEA::dispatchViewportPropertiesDidChange(const ViewportArguments&) const
{
	notImplemented();
	
}

bool ChromeClientEA::selectItemWritingDirectionIsNatural()
{
    return false;
}

bool ChromeClientEA::selectItemAlignmentFollowsMenuWritingDirection(void)
{
	return true;
}

bool ChromeClientEA::hasOpenedPopup() const
{
    //We have to return false here for pop up to open later on.
    notImplemented();
    return false;
}

PassRefPtr<PopupMenu> ChromeClientEA::createPopupMenu(PopupMenuClient* client) const
{
	return adoptRef(new PopupMenuEA(client, this));
}

PassRefPtr<SearchPopupMenu> ChromeClientEA::createSearchPopupMenu(PopupMenuClient* client) const
{
    return adoptRef(new SearchPopupMenuEA(createPopupMenu(client)));
}

void ChromeClientEA::populateVisitedLinks()
{
    // We don't need to do anything here because history is tied to QWebPage rather than stored
    // in a separate database
}

} // namespace WebCore
