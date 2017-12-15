/*
 * Copyright (C) 2006 Nikolas Zimmermann <zimmermann@kde.org>
 * Copyright (C) 2006 Zack Rusin <zack@kde.org>
 * Copyright (C) 2006, 2008 Apple Inc. All rights reserved.
 * Copyright (C) 2009 Nokia Corporation and/or its subsidiary(-ies)
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
#include "EditorClientEA.h"

#include "Document.h"
#include "Editor.h"
#include "FocusController.h"
#include "Frame.h"
#include "HTMLElement.h"
#include "HTMLInputElement.h"
#include "HTMLNames.h"
#include "KeyboardEvent.h"
#include "NotImplemented.h"
#include "Page.h"
#include "Pasteboard.h"
#include "PlatformKeyboardEvent.h"
#include "WebPageClientEA.h"
#include "Range.h"
#include "Settings.h"
#include "WindowsKeyboardCodes.h"
#include "SpatialNavigation.h"
#include "StylePropertySet.h"
#include "webpage.h"
#include "webpage_p.h"

#include <EAWebKit/EAWebKitInput.h>
#include <EAWebKit/EAWebKitClient.h>
#include <internal/include/EAWebKitAssert.h>

#include <stdio.h>

namespace WebCore {

struct KeyBoardClassMap
{
	EA::WebKit::KeyboardType mKeyboardType;
	const char*	mKeyboardClass;			
};
static KeyBoardClassMap sKeyBoardClassMap[ ] =
{
	{EA::WebKit::kDefaultKeyBoard,					"eaw-kb-default"},
	{EA::WebKit::kNumericKeyBoard,					"eaw-kb-numeric"},
	{EA::WebKit::kJapaneseFullKeyBoard,				"eaw-kb-japanese-full"},
	{EA::WebKit::kJapaneseHiraganaKeyBoard,			"eaw-kb-japanese-hiragana"},
	{EA::WebKit::kJapaneseKatakanaKeyBoard,			"eaw-kb-japanese-katakana"},
	{EA::WebKit::kRussianFullKeyBoard,				"eaw-kb-russian"},
	{EA::WebKit::kKoreanFullKeyBoard,				"eaw-kb-korean"},
	{EA::WebKit::kTraditionalChineseFullKeyBoard,	"eaw-kb-traditional-chinese"},
	{EA::WebKit::kSimplifiedChineseKeyBoard,		"eaw-kb-simplified-chinese"},
	{EA::WebKit::kLatinFullKeyBoard,				"eaw-kb-latin-full"},
	{EA::WebKit::kURLKeyBoard,						"eaw-kb-url"},
	{EA::WebKit::kPasswordKeyBoard,					"eaw-kb-password"},
	{EA::WebKit::kEmailKeyBoard,					"eaw-kb-email"},
	{EA::WebKit::kGamerTagKeyBoard,					"eaw-kb-gamertag"},
	{EA::WebKit::kPhoneKeyBoard,					"eaw-kb-phone"},
	{EA::WebKit::kIPAddressKeyBoard,				"eaw-kb-ipaddress"},


	{EA::WebKit::kCountKeyBoardTypes,				0}
};

	
bool EditorClientEA::dumpEditingCallbacks = false;
bool EditorClientEA::acceptsEditing = true;

using namespace HTMLNames;

bool EditorClientEA::canCopyCut(WebCore::Frame *, bool defaultValue) const 
{
    return defaultValue;
}

bool EditorClientEA::canPaste(WebCore::Frame *, bool defaultValue) const
{
    return defaultValue;
}

bool EditorClientEA::shouldDeleteRange(Range* range)
{
//     if (dumpEditingCallbacks)
//         printf("EDITING DELEGATE: shouldDeleteDOMRange:%s\n", dumpRange(range).toUtf8().constData());
	notImplemented();
    return true;
}

bool EditorClientEA::isContinuousSpellCheckingEnabled()
{
	notImplemented();
	return false;
}

bool EditorClientEA::isGrammarCheckingEnabled()
{
	notImplemented();
	return false;
}

int EditorClientEA::spellCheckerDocumentTag()
{
	notImplemented();
	return 0;
}

bool EditorClientEA::shouldBeginEditing(WebCore::Range* range)
{
//     if (dumpEditingCallbacks)
//         printf("EDITING DELEGATE: shouldBeginEditingInDOMRange:%s\n", dumpRange(range).toUtf8().constData());
	notImplemented();
	return true;
}

bool EditorClientEA::shouldEndEditing(WebCore::Range* range)
{
//     if (dumpEditingCallbacks)
//         printf("EDITING DELEGATE: shouldEndEditingInDOMRange:%s\n", dumpRange(range).toUtf8().constData());
	notImplemented();
	return true;
}

bool EditorClientEA::shouldInsertText(const String& string, Range* range, EditorInsertAction action)
{
	notImplemented();
	return acceptsEditing;
}

bool EditorClientEA::shouldChangeSelectedRange(Range* currentRange, Range* proposedRange, EAffinity selectionAffinity, bool stillSelecting)
{
//     if (dumpEditingCallbacks) {
//         static const char *affinitystring[] = {
//             "NSSelectionAffinityUpstream",
//             "NSSelectionAffinityDownstream"
//         };
//         static const char *boolstring[] = {
//             "FALSE",
//             "TRUE"
//         };
// 
//         printf("EDITING DELEGATE: shouldChangeSelectedDOMRange:%s toDOMRange:%s affinity:%s stillSelecting:%s\n",
//                dumpRange(currentRange).toUtf8().constData(),
//                dumpRange(proposedRange).toUtf8().constData(),
//                affinitystring[selectionAffinity], boolstring[stillSelecting]);
//     }
	notImplemented();
	return acceptsEditing;
}

bool EditorClientEA::shouldApplyStyle(StylePropertySet*, Range*)
{
	notImplemented();
	return acceptsEditing;
}

bool EditorClientEA::shouldMoveRangeAfterDelete(WebCore::Range*, WebCore::Range*)
{
    notImplemented();
    return true;
}

void EditorClientEA::didBeginEditing()
{
//     if (dumpEditingCallbacks)
//         printf("EDITING DELEGATE: webViewDidBeginEditing:WebViewDidBeginEditingNotification\n");
    m_editing = true;
}

void EditorClientEA::respondToChangedContents()
{
//     if (dumpEditingCallbacks)
//         printf("EDITING DELEGATE: webViewDidChange:WebViewDidChangeNotification\n");
//     m_page->d->updateEditorActions();
// 
//     emit m_page->contentsChanged();

	notImplemented();

}

void EditorClientEA::respondToChangedSelection(Frame*)
{
	notImplemented();
}

void EditorClientEA::didEndEditing()
{
//     if (dumpEditingCallbacks)
//         printf("EDITING DELEGATE: webViewDidEndEditing:WebViewDidEndEditingNotification\n");
    m_editing = false;
}

void EditorClientEA::willWriteSelectionToPasteboard(Range*)
{
    notImplemented();
}

void EditorClientEA::didWriteSelectionToPasteboard()
{
	notImplemented();
}

void EditorClientEA::getClientPasteboardDataForRange(Range*, Vector<String>& pasteboardTypes, Vector<RefPtr<WebCore::SharedBuffer> >& pasteboardData)
{
	notImplemented();
}

void EditorClientEA::registerUndoStep(PassRefPtr<UndoStep>)
{
	notImplemented();
}

void EditorClientEA::registerRedoStep(PassRefPtr<UndoStep>)
{
	notImplemented();
}

void EditorClientEA::clearUndoRedoOperations()
{
	notImplemented();
}

bool EditorClientEA::canUndo() const
{
	return false;
}

bool EditorClientEA::canRedo() const
{
	return false;
}

void EditorClientEA::undo()
{
	notImplemented();
}

void EditorClientEA::redo()
{
	notImplemented();
}

bool EditorClientEA::shouldInsertNode(Node* node, Range* range, EditorInsertAction action)
{
//     if (dumpEditingCallbacks) {
//         static const char *insertactionstring[] = {
//             "WebViewInsertActionTyped",
//             "WebViewInsertActionPasted",
//             "WebViewInsertActionDropped",
//         };
// 
//         printf("EDITING DELEGATE: shouldInsertNode:%s replacingDOMRange:%s givenAction:%s\n", dumpPath(node).toUtf8().constData(),
//                dumpRange(range).toUtf8().constData(), insertactionstring[action]);
//     }
	notImplemented();
    return acceptsEditing;
}

void EditorClientEA::pageDestroyed()
{
    delete this;
}

bool EditorClientEA::smartInsertDeleteEnabled()
{
    return m_page->d->smartInsertDeleteEnabled;
}

bool EditorClientEA::isSelectTrailingWhitespaceEnabled()
{
    return m_page->d->selectTrailingWhitespaceEnabled;
}

void EditorClientEA::toggleContinuousSpellChecking()
{
    notImplemented();
}

void EditorClientEA::toggleGrammarChecking()
{
    notImplemented();
}
void EditorClientEA::handleKeyboardEvent(KeyboardEvent* event)
{
    const PlatformKeyboardEvent* kevent = event->keyEvent();
    if (!kevent || kevent->type() == PlatformKeyboardEvent::KeyUp)
        return;

    EA::WebKit::WebAction action = EA::WebKit::WebPagePrivate::editorActionForKeyEvent(kevent->eaKeyboardEvent());
    // Special handling for copy to the clipboard.  The node does not have to be selected to allow copy.
    if (action == EA::WebKit::Copy)
    {
         m_page->triggerAction(action);
         return;
    }

    Frame& frame = m_page->d->page->focusController().focusedOrMainFrame();
    if(!frame.document()->focusedElement())
		return;

    Node* start = frame.selection().start().containerNode();
    if (!start)
        return;

	EAW_ASSERT_MSG((kevent->type() != PlatformKeyboardEvent::KeyDown),"EAWebKit has Windows style key handling. So we handle RawKeyDown and Char rather than KeyDown");
    
    if (start->isContentEditable()) {
        bool doSpatialNavigation = false;
        if (isSpatialNavigationEnabled(&frame)) {
            if (!kevent->modifiers()) {
                switch (kevent->windowsVirtualKeyCode()) {
				case EA::WebKit::kArrowLeft:
				case EA::WebKit::kArrowRight:
				case EA::WebKit::kArrowUp:
				case EA::WebKit::kArrowDown:
                    doSpatialNavigation = true;
                }
            }
        }
		
		if (action != EA::WebKit::NoWebAction && !doSpatialNavigation) {
			const char* cmd = EA::WebKit::WebPagePrivate::editorCommandForWebActions(action);
            // WebKit doesn't have enough information about mode to decide how commands that just insert text if executed via Editor should be treated,
            // so we leave it upon WebCore to either handle them immediately (e.g. Tab that changes focus) or let a keypress event be generated
            // (e.g. Tab that inserts a Tab character, or Enter).
            // For RawKeyDown, if the command is not text insertion, simply return without executing command.
			// If Char, trigger the action mapped for Command and return.
			// This code is simply equivalent but written differently from WebView.cpp on Windows.
			if (cmd && frame.editor().command(cmd).isTextInsertion()
                && kevent->type() == PlatformKeyboardEvent::RawKeyDown)
                return;

            m_page->triggerAction(action);
			
        } else
        switch (kevent->windowsVirtualKeyCode()) { //If the command is not found, additional commands are here for RawKeyDown only.
		case EA::WebKit::kBackspace:
            frame.editor().deleteWithDirection(WebCore::DirectionBackward,
                    CharacterGranularity, false, true);
            break;
		case EA::WebKit::kDelete:
            frame.editor().deleteWithDirection(WebCore::DirectionForward,
                    CharacterGranularity, false, true);
            break;
		case EA::WebKit::kArrowLeft:
            if (kevent->shiftKey())
                frame.editor().command("MoveLeftAndModifySelection").execute();
            else if (!frame.editor().command("MoveLeft").execute())
                return;
            break;
		case EA::WebKit::kArrowRight:
            if (kevent->shiftKey())
                frame.editor().command("MoveRightAndModifySelection").execute();
            else if (!frame.editor().command("MoveRight").execute())
                return;
            break;
		case EA::WebKit::kArrowUp:
            if (kevent->shiftKey())
                frame.editor().command("MoveUpAndModifySelection").execute();
            else if (!frame.editor().command("MoveUp").execute())
                return;
            break;
		case EA::WebKit::kArrowDown:
            if (kevent->shiftKey())
                frame.editor().command("MoveDownAndModifySelection").execute();
            else if (!frame.editor().command("MoveDown").execute())
                return;
            break;
		case EA::WebKit::kPageUp: // PageUp
            if (kevent->shiftKey())
                frame.editor().command("MovePageUpAndModifySelection").execute();
            else
                frame.editor().command("MovePageUp").execute();
            break;
		case EA::WebKit::kPageDown: // PageDown
            if (kevent->shiftKey())
                frame.editor().command("MovePageDownAndModifySelection").execute();
            else
                frame.editor().command("MovePageDown").execute();
            break;
		case EA::WebKit::kTab:
            return;
        default:
            if (kevent->type() != PlatformKeyboardEvent::KeyDown && !kevent->ctrlKey()
                // We need to exclude checking for Alt because it is just a different Shift
                && !kevent->altKey()
                && !kevent->text().isEmpty() && !(kevent->text()[0] < ' ')) { // abaldeva: Exclude these characters as done in Windows port
                frame.editor().insertText(kevent->text(), event);
            } 
			else if (
				kevent->ctrlKey()
				) 
			{
                int modifierFilter = PlatformKeyboardEvent::CtrlKey;

                switch (kevent->windowsVirtualKeyCode()) {
				case EA::WebKit::kLetterA:
                    if(kevent->modifiers() == modifierFilter)
                        frame.editor().command("SelectAll").execute();
                    break;
				case EA::WebKit::kLetterB:
                    if(kevent->modifiers() == modifierFilter)
                        frame.editor().command("ToggleBold").execute();
                    break;
				case EA::WebKit::kLetterI:
                    if(kevent->modifiers() == modifierFilter)
                        frame.editor().command("ToggleItalic").execute();
                    break;
                default:
                    // catch combination AltGr+key or Ctrl+Alt+key
                    if (kevent->type() != PlatformKeyboardEvent::KeyDown && kevent->altKey() && !kevent->text().isEmpty()) {
                        frame.editor().insertText(kevent->text(), event);
                        break;
                    }
                    return;
                }
            } else
                return;
        }
    } else {
           return;
    }
    event->setDefaultHandled();
}

void EditorClientEA::handleInputMethodKeydown(KeyboardEvent*)
{
}

EditorClientEA::EditorClientEA(EA::WebKit::WebPage* page)
    : m_page(page), m_editing(false)
{
}

void EditorClientEA::textFieldDidBeginEditing(Element*)
{
    m_editing = true;
}

void EditorClientEA::textFieldDidEndEditing(Element*)
{
    m_editing = false;
}

void EditorClientEA::textDidChangeInTextField(Element*)
{
}

bool EditorClientEA::doTextFieldCommandFromEvent(Element*, KeyboardEvent*)
{
    return false;
}

void EditorClientEA::textWillBeDeletedInTextField(Element*)
{
}

void EditorClientEA::textDidChangeInTextArea(Element*)
{
}


void EditorClientEA::updateSpellingUIWithGrammarString(const String&, const GrammarDetail&)
{
    notImplemented();
}

void EditorClientEA::updateSpellingUIWithMisspelledWord(const String&)
{
    notImplemented();
}

void EditorClientEA::showSpellingUI(bool)
{
    notImplemented();
}

bool EditorClientEA::spellingUIIsShowing()
{
    notImplemented();
    return false;
}

bool EditorClientEA::supportsGlobalSelection(void)
{
    notImplemented();
    return false;
}

bool EditorClientEA::isEditing() const
{
    return m_editing;
}

void EditorClientEA::willSetInputMethodState()
{
}

void EditorClientEA::setInputMethodState(bool active)
{
	WebCore::Node* pNode = NULL;
	Frame& frame = m_page->d->page->focusController().focusedOrMainFrame();
	if (frame.document() && frame.document()->focusedElement())
	{
		pNode = frame.document()->focusedElement();
	}	

	// Note by Arpit Baldeva: If on console(true by default on consoles/can be enabled on PC through emulation), 
	// we detect if the node was focused due to the user input or through javascript.
	// If the node was focused by user input(such as click), we present the editor(emulated keyboard on consoles, debug log on PC).
	// If the node was focused by javascript, we make the cursor jump to the node. In addition, we blur <input>/<textarea> element 
	// so that the user click can bring up the emulated keyboard.
	EA::WebKit::View* pView = m_page->view();
#if defined(EA_PLATFORM_CONSOLE)
	bool onConsole = true;
#else
	bool onConsole = pView->IsEmulatingConsoleOnPC();
#endif
	
	if(onConsole && !pView->AllowJSTextInputStateNotificationOnConsole())
	{
		if(!m_page->handle()->mouseCausedEventActive)
		{
			pView->MoveMouseCursorToNode(pNode);
			if (pNode && pNode->isHTMLElement())
			{
				if(isHTMLInputElement(frame.document()->focusedElement()) || isHTMLTextAreaElement(frame.document()->focusedElement())) 
				{
					HTMLElement* pElement = static_cast<HTMLElement*> (pNode);
					pElement->blur();
				}
			}
			return;
		}
	}
	
	// CSidhall 1/22/09 Added notify user app of text input state for possible virtual keyboard...
	// We can't fully trust the enabled flag because the input field might be a password in which case we still
	// want to activate the keyboard input. So we do our own checking and also get extra info...   

	//Note by Arpit Baldeva: We are interested in the <input> and <textarea> elements. The problem is that we can't rely on the shouldUseInputMethod() of node to reliably detect an
	//editable node since it does not include password(as noted above). Webkit trunk has some cryptic comment explaining why that is the right thing to do. So we do as follows. 
	// We could add a new method to the class hierarchy to achieve following but want to minimize the changes inside core layer.

	bool inputActive	= active;
	bool searchActive	= false;
	bool passwordActive = false;
	EA::WebKit::KeyboardType kbType = EA::WebKit::kDefaultKeyBoard;
	EA::WebKit::InputType inputType = EA::WebKit::kInputTypeNone;

	
    if( pNode && pNode->isHTMLElement())
	{
		if(pNode->hasTagName(WebCore::HTMLNames::inputTag) ) 
		{	
			HTMLInputElement* pInputElement = static_cast<HTMLInputElement*> (pNode);
			inputActive		= pInputElement->isTextField(); // This check makes sure that we are not firing this event with inputActive set to true in case of HTML like <input type="button" value="Click me" onclick="msg()">
			//+ Deprecated
			searchActive	= pInputElement->isSearchField();
			passwordActive	= pInputElement->isPasswordField();
			//-
			
			//New HTML5 input types (text field related)
			if(pInputElement->isEmailField())
				inputType = EA::WebKit::kInputTypeEmail;
			else if (pInputElement->isNumberField())
				inputType = EA::WebKit::kInputTypeNumber;
			else if(pInputElement->isSearchField())
				inputType = EA::WebKit::kInputTypeSearch;
			else if(pInputElement->isTelephoneField())
				inputType = EA::WebKit::kInputTypeTelephone;
			else if(pInputElement->isURLField())
				inputType = EA::WebKit::kInputTypeUrl;
			else if(pInputElement->isPasswordField())
				inputType = EA::WebKit::kInputTypePassword;

		}
/*			//Note by Arpit Baldeva: This will always come back as true but provided the commented out code here for the sack of clarity
		else if(pNode->hasTagName(WebCore::HTMLNames::textareaTag))
		{
			inputActive = enabled;
		}
*/

		HTMLElement* pElement = static_cast<HTMLElement*> (pNode);
		if(pElement->hasClass())
		{
			const WebCore::SpaceSplitString& classNames = pElement->classNames();
			for(int i = 0; i < EA::WebKit::kCountKeyBoardTypes; ++i)
			{
				if(classNames.contains(sKeyBoardClassMap[i].mKeyboardClass))
				{
					kbType = sKeyBoardClassMap[i].mKeyboardType;
					break;
				}
			}
		}
		// Update - 12/20/2010. Since we have the password information available from the HTML, we use it
		// if the keyboard is not overridden.
		if(passwordActive && (kbType == EA::WebKit::kDefaultKeyBoard))
		{
			kbType = EA::WebKit::kPasswordKeyBoard;
		}
    }    

    // Check if should disable the caret.
    if (pView->IsCaretDisabledOnConsole()) 
        frame.selection().setCaretVisible(false);

	using namespace EA::WebKit;
	if(EAWebKitClient* const pClient = GetEAWebKitClient(pView))
	{
        // Store the current settings
        EA::WebKit::TextInputStateInfo textInfo;
        textInfo.mpView				= pView;
		textInfo.mpUserData			= pView->GetUserData();
        textInfo.mIsActivated		= inputActive;
		textInfo.mInputType			= inputType;

        pClient->TextInputState(textInfo);
    }
}

}

// vim: ts=4 sw=4 et
