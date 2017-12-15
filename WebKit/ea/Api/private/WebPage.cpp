/*
    Copyright (C) 2008, 2009 Nokia Corporation and/or its subsidiary(-ies)
    Copyright (C) 2007 Staikos Computing Services Inc.
    Copyright (C) 2007 Apple Inc.
	Copyright (C) 2011, 2012, 2013, 2014 Electronic Arts, Inc. All rights reserved.

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

#include "config.h"
#include "WebPage.h"

#include <EAWebKit/EAWebKitView.h>
#include <EAWebKit/EAWebKit.h>
#include <EAWebKit/EAWebKitClient.h>
#include <EAWebKit/EAWebKitFileSystem.h>
#include <internal/include/EAWebkitEASTLHelpers.h>
#include <internal/include/EAWebKitAssert.h>

#include "WebFrame.h"
#include "WebPage_p.h"
#include "WebFrame_p.h"
#include "WebInspector.h"
//#include "WebKitVersion.h"

#include "CSSComputedStyleDeclaration.h"
#include "CSSParser.h"
#include "ApplicationCacheStorage.h"
//#include "BackForwardListImpl.h"
#include "MemoryCache.h"
#include "Chrome.h"
#include "ChromeClientEA.h"
#include "ClientRect.h"
//#include "ContextMenu.h"
//#include "ContextMenuClientEA.h"
//#include "ContextMenuController.h"
//#include "DeviceMotionClientEA.h"
//#include "DeviceOrientationClientEA.h"
#include "DocumentLoader.h"
#include "DragController.h"
#include "DragData.h"
#include "Editor.h"
#include "EditorClientEA.h"
#include "FocusController.h"
#include "FormState.h"
#include "Frame.h"
#include "FrameLoadRequest.h"
#include "FrameLoader.h"
#include "FrameLoader.h"
#include "FrameLoaderClientEA.h"
#include "FrameTree.h"
#include "FrameView.h"
#include "HTMLFormElement.h"
#include "HTMLFrameOwnerElement.h"
#include "HTMLInputElement.h"
#include "HTMLNames.h"
#include <wtf/HashMap.h>
#include "HitTestResult.h"
#include "Image.h"
#include "InspectorClientEA.h"
#include "InspectorController.h"
#include "URL.h"
#include "LocalizedStrings.h"
#include "MIMETypeRegistry.h"
#include "NavigationAction.h"
#include "NetworkingContext.h"
#include "NodeList.h"
#include "NotImplemented.h"
#include "Page.h"
#include "PageClientEA.h"
#include "PageGroup.h"
#include "Pasteboard.h"
#include "PlatformKeyboardEvent.h"
#include "PlatformTouchEvent.h"
#include "PlatformWheelEvent.h"
#include "PluginDatabase.h"
#include "PluginDatabase.h"
#include "PluginPackage.h"
#include "ProgressTracker.h"
#include <wtf/RefPtr.h>
#include "RenderTextControl.h"
#include "SchemeRegistry.h"
#include "Scrollbar.h"
#include "SecurityOrigin.h"
#include "Settings.h"
#include "TextIterator.h"
#include "WindowFeatures.h"
#include "WorkerThread.h"
#include <runtime/InitializeThreading.h>
#include <wtf/Threading.h>
#include "PageCache.h"
#include "EmptyClients.h"
#include <internal/include/EAWebKit_p.h>
#include "WebInspectorServer.h"

#include "DatabaseManager.h"
#include "DatabaseTracker.h"

namespace EA
{
namespace WebKit
{
extern const char8_t* kDefaultPageGroupName;

// Apply the global EAWebKit Params to the page
static void ApplyParamsToSettings(WebCore::Settings& settings)
{
    const EA::WebKit::Parameters& parameters =  EA::WebKit::GetParameters();
    // Transfer preset font families 
    WTF::AtomicString family;
    family = parameters.mFontFamilyStandard; 
    settings.setStandardFontFamily(family);
    family = parameters.mFontFamilyFantasy;      
    settings.setFantasyFontFamily(family); 
    family = parameters.mFontFamilyCursive;     
    settings.setCursiveFontFamily(family);  
    family = parameters.mFontFamilySerif;
    settings.setSerifFontFamily( family); 
    family = parameters.mFontFamilySansSerif;
    settings.setSansSerifFontFamily(family);  
    family = parameters.mFontFamilyMonospace;
    settings.setFixedFontFamily(family); 

    // Font sizes
    settings.setDefaultFontSize(parameters.mDefaultFontSize);
    settings.setDefaultFixedFontSize(parameters.mDefaultMonospaceFontSize);
    settings.setMinimumFontSize(parameters.mMinimumFontSize); 
    settings.setMinimumLogicalFontSize(parameters.mMinimumLogicalFontSize);

    // Set up EA overrides from WebCore setting defaults. Could expose some of these settings to user if needed/requested.          
    settings.setScriptEnabled(true);
    settings.setJavaScriptCanOpenWindowsAutomatically(true);
    settings.setAllowScriptsToCloseWindows(true);
    settings.setLoadsImagesAutomatically(true);   
    settings.setTextAreasAreResizable(true);
    settings.setHyperlinkAuditingEnabled(true);
    settings.setDOMPasteAllowed(true);
    settings.setJavaScriptCanAccessClipboard(true);
    settings.setDeveloperExtrasEnabled(true);
    settings.setOfflineWebApplicationCacheEnabled(true);
    settings.setXSSAuditorEnabled(true);

	char tempDir[EA::IO::kMaxDirectoryLength];
	memset(tempDir, 0, EA::IO::kMaxDirectoryLength);
	if(EA::WebKit::GetFileSystem()->GetTempDirectory(tempDir, EA::IO::kMaxDirectoryLength - 1))
	{
		WTF::String tempDirStr = WTF::String::fromUTF8(tempDir,strlen(tempDir));
		(void) tempDirStr;

		settings.setLocalStorageEnabled(true);
		if(settings.localStorageDatabasePath().isEmpty())
			settings.setLocalStorageDatabasePath(WebCore::pathByAppendingComponent(tempDirStr, "LocalStorage"));

		settings.setOfflineWebApplicationCacheEnabled(true);
		if(WebCore::cacheStorage().cacheDirectory().isEmpty())
			WebCore::cacheStorage().setCacheDirectory(WebCore::pathByAppendingComponent(tempDirStr, "OfflineCache"));

		//Web SQL database is deprecated and does not seem to function correctly anymore. 
#if 0// ENABLE(SQL_DATABASE)
		static bool dbInitialized = false;
		if(!dbInitialized)
		{
			WebCore::DatabaseManager::manager().initialize(WebCore::pathByAppendingComponent(tempDirStr, "Databases"));
			WebCore::DatabaseManager::manager().setIsAvailable(true);

			dbInitialized = true;
		}
#endif
	}

	// We disable page cache because it causes WebCore to hold onto some resources at shutdown and results in hard to reproduce bugs.   
    settings.setUsesPageCache(false);
}

// Lookup table mapping EA::WebKit::WebActions to the associated Editor commands
static const char* editorCommandWebActions[] =
{
	0, // OpenLink,

	0, // OpenLinkInNewWindow,
	0, // OpenFrameInNewWindow,

	0, // DownloadLinkToDisk,
	0, // CopyLinkToClipboard,

	0, // OpenImageInNewWindow,
	0, // DownloadImageToDisk,
	0, // CopyImageToClipboard,

	0, // Back,
	0, // Forward,
	0, // Stop,
	0, // Reload,

	"Cut", // Cut,
	"Copy", // Copy,
	"Paste", // Paste,

	"Undo", // Undo,
	"Redo", // Redo,
	"MoveForward", // MoveToNextChar,
	"MoveBackward", // MoveToPreviousChar,
	"MoveWordForward", // MoveToNextWord,
	"MoveWordBackward", // MoveToPreviousWord,
	"MoveDown", // MoveToNextLine,
	"MoveUp", // MoveToPreviousLine,
	"MoveToBeginningOfLine", // MoveToStartOfLine,
	"MoveToEndOfLine", // MoveToEndOfLine,
	"MoveToBeginningOfParagraph", // MoveToStartOfBlock,
	"MoveToEndOfParagraph", // MoveToEndOfBlock,
	"MoveToBeginningOfDocument", // MoveToStartOfDocument,
	"MoveToEndOfDocument", // MoveToEndOfDocument,
	"MoveForwardAndModifySelection", // SelectNextChar,
	"MoveBackwardAndModifySelection", // SelectPreviousChar,
	"MoveWordForwardAndModifySelection", // SelectNextWord,
	"MoveWordBackwardAndModifySelection", // SelectPreviousWord,
	"MoveDownAndModifySelection", // SelectNextLine,
	"MoveUpAndModifySelection", // SelectPreviousLine,
	"MoveToBeginningOfLineAndModifySelection", // SelectStartOfLine,
	"MoveToEndOfLineAndModifySelection", // SelectEndOfLine,
	"MoveToBeginningOfParagraphAndModifySelection", // SelectStartOfBlock,
	"MoveToEndOfParagraphAndModifySelection", // SelectEndOfBlock,
	"MoveToBeginningOfDocumentAndModifySelection", //SelectStartOfDocument,
	"MoveToEndOfDocumentAndModifySelection", // SelectEndOfDocument,
	"DeleteWordBackward", // DeleteStartOfWord,
	"DeleteWordForward", // DeleteEndOfWord,

	0, // SetTextDirectionDefault,
	0, // SetTextDirectionLeftToRight,
	0, // SetTextDirectionRightToLeft,

	"ToggleBold", // ToggleBold,
	"ToggleItalic", // ToggleItalic,
	"ToggleUnderline", // ToggleUnderline,

	0, // InspectElement,

	"InsertNewline", // InsertParagraphSeparator
	"InsertLineBreak", // InsertLineSeparator

	"SelectAll", // SelectAll
	0, // ReloadAndBypassCache,

	"PasteAndMatchStyle", // PasteAndMatchStyle
	"RemoveFormat", // RemoveFormat
	"Strikethrough", // ToggleStrikethrough,
	"Subscript", // ToggleSubscript
	"Superscript", // ToggleSuperscript
	"InsertUnorderedList", // InsertUnorderedList
	"InsertOrderedList", // InsertOrderedList
	"Indent", // Indent
	"Outdent", // Outdent,

	"AlignCenter", // AlignCenter,
	"AlignJustified", // AlignJustified,
	"AlignLeft", // AlignLeft,
	"AlignRight", // AlignRight,

	0, // StopScheduledPageRefresh,

	0, // CopyImageUrlToClipboard,

	0 // WebActionCount
};

// Lookup the appropriate editor command to use for WebAction \a action
const char* WebPagePrivate::editorCommandForWebActions(WebAction action)
{
	if ((action > EA::WebKit::NoWebAction) && (action < int(sizeof(editorCommandWebActions) / sizeof(const char*))))
		return editorCommandWebActions[action];
	return 0;
}


WebPagePrivate::WebPagePrivate(WebPage *wPage)
: webPage(wPage)
, page(0)
, mainFrame(0)
, insideOpenCall(false)
, m_totalBytes(0)
, m_bytesReceived()
, mouseCausedEventActive(false)
, clickCausedFocus(false)
, forwardUnsupportedContent(false)
, smartInsertDeleteEnabled(true)
, selectTrailingWhitespaceEnabled(false)
, linkPolicy(WebPage::DontDelegateLinks)
, viewportSize(WebCore::IntSize(0, 0))
, useFixedLayout(false)
, mInspector(NULL)
{
	WebCore::Page::PageClients pageClients;
	pageClients.chromeClient = new WebCore::ChromeClientEA(webPage);
	pageClients.editorClient = new WebCore::EditorClientEA(webPage);
	pageClients.dragClient = new WebCore::EmptyDragClient(); //Even though we don't suppor it, we still need to provide a stub implementation otherwise user action can cause it to crash. 
	pageClients.loaderClientForMainFrame = new WebCore::FrameLoaderClientEA();

#if ENABLE(INSPECTOR)
	pageClients.inspectorClient = new WebCore::InspectorClientEA(webPage);
#else
    pageClients.inspectorClient = NULL;
#endif

	page = new WebCore::Page(pageClients);
	page->setGroupName(kDefaultPageGroupName);

    // Set up the page settings
    ApplyParamsToSettings(page->settings());

#if 0
	memset(actions, 0, sizeof(actions));
#endif
	WebCore::PageGroup::setShouldTrackVisitedLinks(true);

#if ENABLE(NOTIFICATIONS)    
	//NotificationPresenterClientEA::notificationPresenter()->addClient();
#endif
}

WebPagePrivate::~WebPagePrivate()
{
	delete page;

	// This used to be deleted in the FrameLoaderClientEA file, but David moved it to here because
	// it wasn't being deleted all the time.
	// Update:06/15/2012
	// abaldeva: Not sure what was David's intention but this results in memory corruption. For now, resolve memory corruption to make this codebase
	// stable.
    // delete mainFrame;

#if ENABLE(NOTIFICATIONS)
	//NotificationPresenterClientEA::notificationPresenter()->removeClient();
#endif
}

WebCore::ViewportArguments WebPagePrivate::viewportArguments()
{
	return page ? page->viewportArguments() : WebCore::ViewportArguments();
}

WebCore::Page* WebPagePrivate::core(const WebPage* page)
{
	return page->d->page;
}

WebPagePrivate* WebPagePrivate::priv(WebPage* page)
{
	return page->d;
}

bool WebPagePrivate::acceptNavigationRequest(WebFrame* frame, const WebCore::ResourceRequest& request, WebPage::NavigationType type)
{
	if (insideOpenCall
		&& frame == mainFrame)
		return true;
	return webPage->acceptNavigationRequest(frame, request, type);
}

void WebPagePrivate::createMainFrame()
{
	if (!mainFrame) 
	{
		WebFrameData frameData(page);
		mainFrame = new WebFrame(webPage, &frameData);
	}
}

void WebPagePrivate::updateAction(WebAction action)
{
	//Possibly implement this.
}

void WebPagePrivate::updateNavigationActions()
{
 	if(!mainFrame)
 		return;

	if(EAWebKitClient* pClient = GetEAWebKitClient(webView))
	{
		WebCore::FrameLoader& loader = mainFrame->d->frame->loader();
		
		NavigationActionsInfo info;
		info.mpView = webView;
		info.mpUserData = webView->GetUserData();
		
		info.mCanGoBack = page->canGoBackOrForward(-1);
		info.mCanGoForward = page->canGoBackOrForward(1);
		info.mCanStop = loader.isLoading();
		info.mCanReload = !info.mCanStop;

		pClient->NavigationActionsUpdate(info);
	}
}

void WebPagePrivate::updateEditorActions()
{
	updateAction(EA::WebKit::Cut);
	updateAction(EA::WebKit::Copy);
	updateAction(EA::WebKit::Paste);
	updateAction(EA::WebKit::MoveToNextChar);
	updateAction(EA::WebKit::MoveToPreviousChar);
	updateAction(EA::WebKit::MoveToNextWord);
	updateAction(EA::WebKit::MoveToPreviousWord);
	updateAction(EA::WebKit::MoveToNextLine);
	updateAction(EA::WebKit::MoveToPreviousLine);
	updateAction(EA::WebKit::MoveToStartOfLine);
	updateAction(EA::WebKit::MoveToEndOfLine);
	updateAction(EA::WebKit::MoveToStartOfBlock);
	updateAction(EA::WebKit::MoveToEndOfBlock);
	updateAction(EA::WebKit::MoveToStartOfDocument);
	updateAction(EA::WebKit::MoveToEndOfDocument);
	updateAction(EA::WebKit::SelectNextChar);
	updateAction(EA::WebKit::SelectPreviousChar);
	updateAction(EA::WebKit::SelectNextWord);
	updateAction(EA::WebKit::SelectPreviousWord);
	updateAction(EA::WebKit::SelectNextLine);
	updateAction(EA::WebKit::SelectPreviousLine);
	updateAction(EA::WebKit::SelectStartOfLine);
	updateAction(EA::WebKit::SelectEndOfLine);
	updateAction(EA::WebKit::SelectStartOfBlock);
	updateAction(EA::WebKit::SelectEndOfBlock);
	updateAction(EA::WebKit::SelectStartOfDocument);
	updateAction(EA::WebKit::SelectEndOfDocument);
	updateAction(EA::WebKit::DeleteStartOfWord);
	updateAction(EA::WebKit::DeleteEndOfWord);
#if 0
	updateAction(EA::WebKit::SetTextDirectionDefault);
	updateAction(EA::WebKit::SetTextDirectionLeftToRight);
	updateAction(EA::WebKit::SetTextDirectionRightToLeft);
	updateAction(EA::WebKit::ToggleBold);
	updateAction(EA::WebKit::ToggleItalic);
	updateAction(EA::WebKit::ToggleUnderline);
#endif
	updateAction(EA::WebKit::InsertParagraphSeparator);
	updateAction(EA::WebKit::InsertLineSeparator);
#if 0
	updateAction(EA::WebKit::PasteAndMatchStyle);
	updateAction(EA::WebKit::RemoveFormat);
	updateAction(EA::WebKit::ToggleStrikethrough);
	updateAction(EA::WebKit::ToggleSubscript);
	updateAction(EA::WebKit::ToggleSuperscript);
	updateAction(EA::WebKit::InsertUnorderedList);
	updateAction(EA::WebKit::InsertOrderedList);
	updateAction(EA::WebKit::Indent);
	updateAction(EA::WebKit::Outdent);
	updateAction(EA::WebKit::AlignCenter);
	updateAction(EA::WebKit::AlignJustified);
	updateAction(EA::WebKit::AlignLeft);
	updateAction(EA::WebKit::AlignRight);
#endif
}

bool WebPagePrivate::mouseMoveEvent(const EA::WebKit::MouseMoveEvent& ev)
{
	WebCore::Frame* frame = WebFramePrivate::core(mainFrame);
	if (!frame->view())
		return false;

	bool accepted = frame->eventHandler().mouseMoved(WebCore::PlatformMouseEvent(&ev));
	return accepted;
}

bool WebPagePrivate::mousePressEvent(const EA::WebKit::MouseButtonEvent& ev)
{
	WebCore::Frame* frame = WebFramePrivate::core(mainFrame);
	if (!frame->view())
		return false;

	EAW_ASSERT_MSG(ev.mbDepressed, "Inside mousePressEvent but mbDepressed is not true");

	RefPtr<WebCore::Node> oldNode;
	WebCore::Frame* focusedFrame = page->focusController().focusedFrame();
	if (WebCore::Document* focusedDocument = focusedFrame ? focusedFrame->document() : 0)
		oldNode = focusedDocument->focusedElement();

	bool accepted = false;
	adjustPointForClicking(ev);

	mouseCausedEventActive = true;

	WebCore::PlatformMouseEvent mev(&ev);
	if (mev.button() != WebCore::NoButton)
		accepted = frame->eventHandler().handleMousePressEvent(mev);

	mouseCausedEventActive = false;
	RefPtr<WebCore::Node> newNode;
	focusedFrame = page->focusController().focusedFrame();
	if (WebCore::Document* focusedDocument = focusedFrame ? focusedFrame->document() : 0)
		newNode = focusedDocument->focusedElement();

	if (newNode && oldNode != newNode)
		clickCausedFocus = true; 

	return accepted;
}

bool WebPagePrivate::mouseReleaseEvent(const EA::WebKit::MouseButtonEvent& ev)
{
	WebCore::Frame* frame = WebFramePrivate::core(mainFrame);
	if (!frame->view())
		return false;

	EAW_ASSERT_MSG(!ev.mbDepressed, "Inside mouseReleaseEvent but mbDepressed is not false");
	
	bool accepted = false;
	adjustPointForClicking(ev);
	mouseCausedEventActive = true;
	
	WebCore::PlatformMouseEvent mev(&ev);
	// ignore the event if we can't map EAWebKit's mouse buttons to WebCore::MouseButton
	if (mev.button() != WebCore::NoButton)
		accepted = frame->eventHandler().handleMouseReleaseEvent(mev);

	mouseCausedEventActive = false;

	return accepted;
}

bool WebPagePrivate::wheelEvent(const EA::WebKit::MouseWheelEvent& ev)
{
	WebCore::Frame* frame = WebFramePrivate::core(mainFrame);
	if (!frame->view())
		return false;

	WebCore::PlatformWheelEvent pev(&ev);
	bool accepted = frame->eventHandler().handleWheelEvent(pev);
	return accepted;
}

WebAction WebPagePrivate::editorActionForKeyEvent(const EA::WebKit::KeyboardEvent* kbEvent)
{
	// Note by Arpit Baldeva:
	// The application passes the keyboard event to the View which ultimately is passed down the the event handler of the frame(focused or main).
	// The event handler passes it on the target node. If the node is interested, it does something with the event and it ends. Otherwise, the event
	// is passed back to the default handler which is frame event handler. Event handler passes the keyboard event to the Editor which in turn passes
	// it to the EditorClient. EditorClient can do something fancy or take a pre-determined action or both. The action of EditorClient is port specific
	// and is implemented in EditorClientEA.cpp.

	// We create a map here for RawKeyDown events which is written in terms of EAWebKitInput.h. Our last port was messy in this regards as it created 
	// duplicates of Windows Key codes without exposing it to the user and simply relied on the fact that it matched to InputMan.

	
	// If not a physical key event, make an early return. Skip the check for the Enter key since it does trigger a text insertion action.
	// This is not really clean but works. If turns out that there is a problem in this code, we would need to rewrite EditorClientEA::handleKeyboardEvent
	// The problem is that this code is based on the Qt input events(which combine physical key with Char keys) and our input system is based on the windows
	// (which sends different physical and char key events). 
	bool isParagraphSeparator = kbEvent->mId ==EA::WebKit::kNewLine || kbEvent->mId ==EA::WebKit::kCarriageReturn ;
 	if(kbEvent->mbChar && !isParagraphSeparator)
 		return EA::WebKit::NoWebAction;

	const uint32_t UnknownKeyMagicNumber = 9999999;
	const uint32_t actionModifierKey = EA::WebKit::kModifierMaskControl;
	static struct {
		uint32_t keyId;
		uint32_t modifiers;
		WebAction action;
	}editorActions[] = {
		{ EA::WebKit::kLetterX, actionModifierKey, EA::WebKit::Cut },
		{ EA::WebKit::kLetterC, actionModifierKey, EA::WebKit::Copy },
		{ EA::WebKit::kLetterV, actionModifierKey, EA::WebKit::Paste },
		//{ EA::WebKit::kLetterZ, actionModifierKey, EA::WebKit::Undo },
		//{ EA::WebKit::kLetterY, actionModifierKey, EA::WebKit::Redo },
		{ EA::WebKit::kLetterA, actionModifierKey, EA::WebKit::SelectAll },

        { EA::WebKit::kArrowDown,	0, EA::WebKit::MoveToNextLine },
		{ EA::WebKit::kArrowUp,		0, EA::WebKit::MoveToPreviousLine },
		{ EA::WebKit::kHome,		0, EA::WebKit::MoveToStartOfLine },
		{ EA::WebKit::kEnd,			0, EA::WebKit::MoveToEndOfLine },

		{ EA::WebKit::kHome,	actionModifierKey, EA::WebKit::MoveToStartOfDocument },
		{ EA::WebKit::kEnd,		actionModifierKey, EA::WebKit::MoveToEndOfDocument },

		{ EA::WebKit::kArrowDown,	EA::WebKit::kModifierMaskShift, EA::WebKit::SelectNextLine },
		{ EA::WebKit::kArrowUp,		EA::WebKit::kModifierMaskShift, EA::WebKit::SelectPreviousLine },
		{ EA::WebKit::kHome,		EA::WebKit::kModifierMaskShift, EA::WebKit::SelectStartOfLine },
		{ EA::WebKit::kEnd,			EA::WebKit::kModifierMaskShift, EA::WebKit::SelectEndOfLine },
		
		{ EA::WebKit::kHome,		actionModifierKey | EA::WebKit::kModifierMaskShift,	EA::WebKit::SelectStartOfDocument },
		{ EA::WebKit::kEnd,			actionModifierKey | EA::WebKit::kModifierMaskShift,	EA::WebKit::SelectEndOfDocument },
		{ EA::WebKit::kBackspace,	actionModifierKey,									EA::WebKit::DeleteStartOfWord },
		{ EA::WebKit::kDelete,		actionModifierKey,									EA::WebKit::DeleteEndOfWord },
		{ EA::WebKit::kNewLine,		0,																	EA::WebKit::InsertParagraphSeparator },
		{ EA::WebKit::kCarriageReturn,		0,															EA::WebKit::InsertParagraphSeparator },

		{ UnknownKeyMagicNumber, 0, EA::WebKit::NoWebAction }

        //No Equivalent exists for:
        // MoveToNextChar
        // MoveToPreviousChar
        // MoveToNextWord
        // MoveToPreviousWord 
        // MoveToStartOfBlock
        // MoveToEndOfBlock 
        // SelectNextChar 
        // SelectPreviousChar
        // SelectNextWord 
        // SelectPreviousWord
        // SelectStartOfBlock
        // SelectEndOfBlock 
        // InsertLineSeparator
    };

	for (uint32_t i = 0; editorActions[i].keyId != UnknownKeyMagicNumber; ++i)
		if ((kbEvent->mId == editorActions[i].keyId) && (kbEvent->mModifiers == editorActions[i].modifiers))
			return editorActions[i].action;

	return EA::WebKit::NoWebAction;
}







// Note by Arpit Baldeva: 
// Our input system(UTFWin/InputMan) is similar to Windows where for every key press, 3 events may be sent in following order. 
// 1. RawKeyDown containing the physical key info,
// 2. If applicable, RawKeyDown translated to a Char. For example, based on whether the caps lock is currently active or not, the mId will change to
// 65("A") or 97("a"). For the RawKeyDown, the mId will remain 65 irrespective of caps lock state.
// 3. Key up event.

// Details - 
//http://www.quirksmode.org/dom/events/keys.html
//https://lists.webkit.org/pipermail/webkit-dev/2007-December/002992.html


void WebPagePrivate::keyPressEvent(const EA::WebKit::KeyboardEvent& ev)
{
	bool handled = false;
	WebCore::Frame& frame = page->focusController().focusedOrMainFrame();
	// we forward the key event to WebCore first to handle potential DOM
	// defined event handlers and later on end up in EditorClientEA::handleKeyboardEvent
	// to trigger editor commands via triggerAction().
	handled = frame.eventHandler().keyEvent(WebCore::PlatformKeyboardEvent(&ev));

	// If not handled and a physical key.
	if (!handled && !ev.mbChar) 
	{
		handled = true;
		if (!handleScrolling(&ev, &frame)) {
			switch (ev.mId) {
			case EA::WebKit::kBrowserBack:
				webPage->triggerAction(EA::WebKit::Back);
				break;
			case EA::WebKit::kBrowserForward:
				webPage->triggerAction(EA::WebKit::Forward);
				break;
			case EA::WebKit::kBrowserStop:
				webPage->triggerAction(EA::WebKit::Stop);
				break;
			case EA::WebKit::kBrowserRefresh:
				webPage->triggerAction(EA::WebKit::Reload);
				break;
			case EA::WebKit::kBackspace:
				if (ev.mModifiers & EA::WebKit::kModifierMaskShift) 
					webPage->triggerAction(EA::WebKit::Forward);
				else
					webPage->triggerAction(EA::WebKit::Back);
				break;
			default:
				handled = false;
				break;
			}
		}
	}
}

void WebPagePrivate::keyReleaseEvent(const EA::WebKit::KeyboardEvent& ev)
{
	WebCore::Frame& frame = page->focusController().focusedOrMainFrame();
	EAW_ASSERT_MSG(!ev.mbChar, "Key release does not correspond to char event");
	EAW_ASSERT_MSG(!ev.mbDepressed, "mbDepressed true in keyReleaseEvent");

	bool handled = frame.eventHandler().keyEvent(&ev);
	(void)handled;

}

void WebPagePrivate::focusInEvent()
{
	WebCore::FocusController& focusController = page->focusController();
	focusController.setActive(true);
	focusController.setFocused(true);
	if (!focusController.focusedFrame())
		focusController.setFocusedFrame(WebFramePrivate::core(mainFrame));
}

void WebPagePrivate::focusOutEvent()
{
	// only set the focused frame inactive so that we stop painting the caret
	// and the focus frame. But don't tell the focus controller so that upon
	// focusInEvent() we can re-activate the frame.
	WebCore::FocusController& focusController = page->focusController();
	// Call setFocused first so that window.onblur doesn't get called twice
	focusController.setFocused(false);
	focusController.setActive(false);
}

bool WebPagePrivate::handleScrolling(const EA::WebKit::KeyboardEvent* keyboardEvent, WebCore::Frame *frame)
{
	//	ScrollByLine		- Typical for specifying the number of lines to scroll
	//	ScrollByPage		- Scroll by page/viewing area
	//	ScrollByDocument	- Scroll to the top/end of the document(web page)
	//	ScrollByPixel		- Scroll by pixel value(unused inside this function)

	// Tested example of scroll (with iframe as well )- http://www.samisite.com/test-csb2nf/id43.htm 

	// Using space bar to move page is common behavior in browsers(Verified on Chrome/Safari)
	// Space bar does not work in our port. Currently, disabled because of a bug that results in both the space bar
	// character inserted in the input field and scroll.
	
	WebCore::ScrollDirection direction;
    WebCore::ScrollGranularity granularity;
	if(/*(keyboardEvent->mId == EA::WebKit::kSpacebar && !(keyboardEvent->mModifiers & EA::WebKit::kModifierMaskShift))
		||*/ (keyboardEvent->mId == EA::WebKit::kPageDown)
		)
	{
		granularity = WebCore::ScrollByPage;
		direction	= WebCore::ScrollDown;
	}
	else if(/*(keyboardEvent->mId == EA::WebKit::kSpacebar && (keyboardEvent->mModifiers & EA::WebKit::kModifierMaskShift))
		|| */(keyboardEvent->mId == EA::WebKit::kPageUp)
		) 
	{
		granularity = WebCore::ScrollByPage;
		direction	= WebCore::ScrollUp;
	}
	else if(keyboardEvent->mId == EA::WebKit::kHome) 
	{
		granularity = WebCore::ScrollByDocument;
		direction	= WebCore::ScrollUp;
	}
	else if(keyboardEvent->mId == EA::WebKit::kEnd) 
	{
		granularity = WebCore::ScrollByDocument;
		direction	= WebCore::ScrollDown;
	}
    else 
	{
        switch (keyboardEvent->mId) 
		{
		case EA::WebKit::kArrowUp:
			granularity = WebCore::ScrollByLine;
            direction	= WebCore::ScrollUp;
            break;
		case EA::WebKit::kArrowDown:
            granularity = WebCore::ScrollByLine;
            direction	= WebCore::ScrollDown;
            break;
		case EA::WebKit::kArrowLeft:
            granularity = WebCore::ScrollByLine;
            direction	= WebCore::ScrollLeft;
            break;
		case EA::WebKit::kArrowRight:
            granularity = WebCore::ScrollByLine;
            direction	= WebCore::ScrollRight;
            break;
        default:
            return false;
        }
    }

    return frame->eventHandler().scrollRecursively(direction, granularity);
}

void WebPagePrivate::adjustPointForClicking(const EA::WebKit::MouseButtonEvent& ev)
{
	notImplemented();
}

void WebPagePrivate::privOnLoadProgressChanged(int)
{
	m_totalBytes = page->progress().totalPageAndResourceBytesToLoad();
	m_bytesReceived = page->progress().totalBytesReceived();
}

WebPage::ViewportAttributes::ViewportAttributes()
    : d(0)
    , m_initialScaleFactor(-1.0)
    , m_minimumScaleFactor(-1.0)
    , m_maximumScaleFactor(-1.0)
    , m_devicePixelRatio(-1.0)
    , m_isUserScalable(true)
    , m_isValid(false)
{

}

WebPage::ViewportAttributes::ViewportAttributes(const WebPage::ViewportAttributes& other)
    : d(other.d)
    , m_initialScaleFactor(other.m_initialScaleFactor)
    , m_minimumScaleFactor(other.m_minimumScaleFactor)
    , m_maximumScaleFactor(other.m_maximumScaleFactor)
    , m_devicePixelRatio(other.m_devicePixelRatio)
    , m_isUserScalable(other.m_isUserScalable)
    , m_isValid(other.m_isValid)
    , m_size(other.m_size)
{

}

WebPage::ViewportAttributes::~ViewportAttributes()
{

}

WebPage::ViewportAttributes& WebPage::ViewportAttributes::operator=(const WebPage::ViewportAttributes& other)
{
    if (this != &other) {
        d = other.d;
        m_initialScaleFactor = other.m_initialScaleFactor;
        m_minimumScaleFactor = other.m_minimumScaleFactor;
        m_maximumScaleFactor = other.m_maximumScaleFactor;
        m_isUserScalable = other.m_isUserScalable;
        m_isValid = other.m_isValid;
        m_size = other.m_size;
    }

    return *this;
}

WebPage::WebPage(View *pView)
: d(new WebPagePrivate(this))
#if ENABLE(INSPECTOR_SERVER) && ENABLE(INSPECTOR)
, mRemoteInspectorId(0)
#endif
{
    setView(pView);
#if ENABLE(INSPECTOR_SERVER) && ENABLE(INSPECTOR)
    mRemoteInspectorId = ::WebKit::WebInspectorServer::shared().registerPage(this);
#endif
}

WebPage::~WebPage()
{
	d->createMainFrame();
	WebCore::FrameLoader& loader = d->mainFrame->d->frame->loader();
    loader.detachFromParent();
#if ENABLE(INSPECTOR_SERVER) && ENABLE(INSPECTOR)    
    ::WebKit::WebInspectorServer::shared().unregisterPage(mRemoteInspectorId);
#endif
    delete d;
}

WebFrame* WebPage::mainFrame() const
{
    d->createMainFrame();
    return d->mainFrame;
}

WebFrame *WebPage::currentFrame() const
{
    d->createMainFrame();
    WebCore::Frame& frame = d->page->focusController().focusedOrMainFrame();
    return static_cast<WebFrame*>(frame.loader().networkingContext()->originatingObject());
}

WebFrame *WebPage::focusedFrame() const
{
	d->createMainFrame();
	WebCore::Frame *frame = d->page->focusController().focusedFrame();
	return static_cast<WebFrame*>(frame->loader().networkingContext()->originatingObject());
}

WebFrame* WebPage::frameAt(const WebCore::IntPoint& pos) const
{
    WebFrame* webFrame = mainFrame();
    if (!webFrame->geometry().contains(pos.x(),pos.y()))
        return 0;
    WebHitTestResult hitTestResult = webFrame->hitTestContent(pos);
    return hitTestResult.frame();
}

void WebPage::setView(View* pView)
{
	EAW_ASSERT_MSG(pView, "View can't be null");
	
	d->webView = pView;
	setViewportSize(pView ? WebCore::IntSize(pView->GetSize()) : WebCore::IntSize(0, 0));

    if (pView)
    {
		d->client = adoptPtr(new WebCore::PageClientWebView(pView, this));

#if USE(TILED_BACKING_STORE)
        if (pView->IsUsingTiledBackingStore())
        {
            d->page->settings().setTiledBackingStoreEnabled(true);
        }
#endif
    }
}


View *WebPage::view() const
{
    return d->webView;
}

void WebPage::triggerAction(WebAction action, bool)
{
	WebCore::Frame& frame = d->page->focusController().focusedOrMainFrame();

	WebCore::Editor& editor = frame.editor();
	const char *command = 0;

	switch (action) {
		case Back:
			d->page->goBack();
			break;
		case Forward:
			d->page->goForward();
			break;
		case Stop:
			mainFrame()->d->frame->loader().stopForUserCancel();
			d->updateNavigationActions();
			break;
		case Reload:
			mainFrame()->d->frame->loader().reload(/*endtoendreload*/false);
			break;
		case ReloadAndBypassCache:
			mainFrame()->d->frame->loader().reload(/*endtoendreload*/true);
			break;
		default:
			command = WebPagePrivate::editorCommandForWebActions(action);
			break;
	}

	if (command)
		editor.command(command).execute();
}


WebCore::IntSize WebPage::viewportSize() const
{
	if (d->mainFrame && d->mainFrame->d->frame->view())
		return d->mainFrame->d->frame->view()->frameRect().size();

	return d->viewportSize;
}

void WebPage::setViewportSize(const WebCore::IntSize& size) const
{
	d->viewportSize = size;

	WebFrame *frame = mainFrame();
	if (frame->d->frame && frame->d->frame->view()) {
		WebCore::FrameView* view = frame->d->frame->view();
		view->resize(size);
		view->adjustViewSize();
	}
}

bool WebPage::acceptNavigationRequest(WebFrame *frame, const WebCore::ResourceRequest &request, WebPage::NavigationType type)
{
	using namespace EA::WebKit;
	if(EAWebKitClient* const pClient = GetEAWebKitClient(d->webView))
	{
		LinkNotificationInfo  lni;
		lni.mpView					= d->webView;
		lni.mpUserData				= lni.mpView->GetUserData();
		lni.mLinkNotificationType	= LinkNotificationInfo::kLinkNavigationBlock;

		const WTF::String& webCoreURI = request.url().string();
		GetFixedString(lni.mOriginalURI)->assign(webCoreURI.characters(), webCoreURI.length());

		pClient->LinkNotification(lni);

		if(lni.mBlockNavigation)
			return false;
	}
	return true;
}


WTF::String WebPage::selectedText() const
{
	d->createMainFrame();
	WebCore::Frame& frame = d->page->focusController().focusedOrMainFrame();
	if (frame.selection().selection().selectionType() == WebCore::VisibleSelection::NoSelection)
		return WTF::String();
	return frame.editor().selectedText();
}

WTF::String WebPage::selectedHtml() const
{
	d->createMainFrame();
	return d->page->focusController().focusedOrMainFrame().editor().selectedRange()->toHTML();
}

void WebPage::setForwardUnsupportedContent(bool forward)
{
	d->forwardUnsupportedContent = forward;
}

bool WebPage::forwardUnsupportedContent() const
{
	return d->forwardUnsupportedContent;
}

void WebPage::setLinkDelegationPolicy(LinkDelegationPolicy policy)
{
	d->linkPolicy = policy;
}

WebPage::LinkDelegationPolicy WebPage::linkDelegationPolicy() const
{
	return d->linkPolicy;
}

WTF::String WebPage::userAgentForUrl(const WebCore::URL& url) const
{
	(void) url;
	const EA::WebKit::Parameters& parameters =  EA::WebKit::GetParameters();
	if(parameters.mpUserAgent && parameters.mpUserAgent[0])
		return WTF::String(parameters.mpUserAgent); 
	else
	{
		// Could not find the version info inside the source code base so took it from Wikipedia (http://en.wikipedia.org/wiki/Safari_version_history) October 22nd, 2013 release of Safari 
		return WTF::String("Mozilla/5.0 (Windows NT 6.1) AppleWebKit/537.71 (KHTML, like Gecko) Safari/537.71 WebKit/157437 EAWebKit/" EAWEBKIT_VERSION_S);
	}
}

uint64_t WebPage::totalBytes() const
{
    return d->m_totalBytes;
}

uint64_t WebPage::bytesReceived() const
{
    return d->m_bytesReceived;
}

void WebPage::SetInspectorDisplay(bool show)
{
#if ENABLE(INSPECTOR)
    WebCore::InspectorController *ic = d->page->inspectorController();
    if (ic)
    {
        if (show)
        {
			EA::WebKit::WebFrame* pMainFrame = mainFrame();
			int cursorX, cursorY;
			view()->GetCursorPosition(cursorX, cursorY);
			EA::WebKit::WebHitTestResult hitTestResult = pMainFrame->hitTestContent(WebCore::IntPoint(cursorX, cursorY), false);
			if (hitTestResult.d)  
            {
                WebCore::Node* nodeToInspect = hitTestResult.d->innerNonSharedNode.get();
                ic->inspect(nodeToInspect); 
            }
        }
        else
        {
            ic->close();
        }
    }
#endif
}

WebInspector *WebPage::GetInspector(void)
{
    if (d->mInspector == NULL)
    {
        d->mInspector = new WebInspector();
    }

    return d->mInspector;
}

void WebPage::DestroyInspector(void)
{
    delete d->mInspector;
    d->mInspector = NULL;
}

#if ENABLE(INSPECTOR_SERVER) && ENABLE(INSPECTOR)
void WebPage::remoteFrontendConnected()
{
    WebCore::InspectorController *ic = d->page->inspectorController();
    ic->connectFrontend(this);
}

void WebPage::remoteFrontendDisconnected()
{
    WebCore::InspectorController *ic = d->page->inspectorController();
    ic->disconnectFrontend();
}

void WebPage::dispatchMessageFromRemoteFrontend(const String& message)
{
    WebCore::InspectorController *ic = d->page->inspectorController();
    ic->dispatchMessageFromFrontend(message);
}

bool WebPage::sendMessageToFrontend(const String& message)
{
    ::WebKit::WebInspectorServer::shared().sendMessageOverConnection(mRemoteInspectorId, message);
    return true;
}
#endif

}}
