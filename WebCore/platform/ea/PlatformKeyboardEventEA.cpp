/*
 * Copyright (C) 2006 Nikolas Zimmermann <zimmermann@kde.org>
 *
 * All rights reserved.
 *
 * Copyright (C) 2011, 2012, 2014 Electronic Arts, Inc. All rights reserved.
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

// Note by Arpit Baldeva : Some of the following code is ported from the KeyEventWin.cpp because that matches our input system more closely than Qt 
// which is to send RawKeyDown -> Char -> KeyUp event.

#include "config.h"
#include "PlatformKeyboardEvent.h"

#include "NotImplemented.h"
#include "WindowsKeyboardCodes.h"

#include <wtf/ASCIICType.h>

#include <EAWebKit/EAWebKitInput.h>
#include <wtf/CurrentTime.h>

namespace WebCore {

String keyIdentifierForEAKeyCode(int keyCode)
{
	using namespace EA::WebKit;
	
	switch (keyCode) {
	case kAlt:
	case kLAlt:
	case kRAlt:
        return ASCIILiteral("Alt");
    case kArrowDown:
        return ASCIILiteral("Down");
    case kEnd:
        return ASCIILiteral("End");
	case kNewLine:
	case kCarriageReturn:
        return ASCIILiteral("Enter");
    case kExecute:
        return ASCIILiteral("Execute");
    case kF1:
        return ASCIILiteral("F1");
    case kF2:
        return ASCIILiteral("F2");
    case kF3:
        return ASCIILiteral("F3");
    case kF4:
        return ASCIILiteral("F4");
    case kF5:
        return ASCIILiteral("F5");
    case kF6:
        return ASCIILiteral("F6");
    case kF7:
        return ASCIILiteral("F7");
    case kF8:
        return ASCIILiteral("F8");
    case kF9:
        return ASCIILiteral("F9");
    case kF10:
        return ASCIILiteral("F10");
    case kF11:
        return ASCIILiteral("F11");
    case kF12:
        return ASCIILiteral("F12");
    case kF13:
        return ASCIILiteral("F13");
    case kF14:
        return ASCIILiteral("F14");
    case kF15:
        return ASCIILiteral("F15");
    case kF16:
        return ASCIILiteral("F16");
    case kF17:
        return ASCIILiteral("F17");
    case kF18:
        return ASCIILiteral("F18");
    case kF19:
        return ASCIILiteral("F19");
    case kF20:
        return ASCIILiteral("F20");
    case kF21:
        return ASCIILiteral("F21");
    case kF22:
        return ASCIILiteral("F22");
    case kF23:
        return ASCIILiteral("F23");
    case kF24:
        return ASCIILiteral("F24");
    case kHelp:
        return ASCIILiteral("Help");
    case kHome:
        return ASCIILiteral("Home");
    case kInsert:
        return ASCIILiteral("Insert");
    case kArrowLeft:
        return ASCIILiteral("Left");
    case kPageDown:
        return ASCIILiteral("PageDown");
    case kPageUp:
        return ASCIILiteral("PageUp");
    case kPause:
        return ASCIILiteral("Pause");
    case kPrintScreen:
        return ASCIILiteral("PrintScreen");
    case kArrowRight:
        return ASCIILiteral("Right");
    case kSelect:
        return ASCIILiteral("Select");
    case kArrowUp:
        return ASCIILiteral("Up");
        // Standard says that DEL becomes U+007F.
    case kDelete:
        return ASCIILiteral("U+007F");
    case kBackspace:
        return ASCIILiteral("U+0008");
    case kTab:
        return ASCIILiteral("U+0009");
    default:
		// We use WTF::toASCIIUpper here instead of toupper as WebKit prefers that. toupper is "locale" dependent. See <wtf/DisallowCtype.h>.
		// Ported from Windows port.
        if (keyCode < 128)
        {
            return String::format("U+%04X", WTF::toASCIIUpper(keyCode));
        }

        return String();
    }
}

static bool isKeypadEvent(int keyCode, uint32_t modifiers, PlatformEvent::Type type)
{
    if (type != PlatformEvent::RawKeyDown && type != PlatformEvent::KeyUp)
		return false;

	using namespace EA::WebKit;

	switch (keyCode) {
		case kNumLock:
		case kNumpad0:
		case kNumpad1:
		case kNumpad2:
		case kNumpad3:
		case kNumpad4:
		case kNumpad5:
		case kNumpad6:
		case kNumpad7:
		case kNumpad8:
		case kNumpad9:
		case kNumpadMultiply:
		case kNumpadPlus:
		case kSeparator:
		case kNumpadMinus:
		case kNumpadDecimal:
		case kNumpadDivide:
			return true;
		case kNumpadEnter:
			return true; 
		case kInsert:
		case kDelete:
		case kPageUp:
		case kPageDown:
		case kEnd:
		case kHome:
		case kArrowLeft:
		case kArrowUp:
		case kArrowRight:
		case kArrowDown:
			// At the moment, We can't reliably detect this scenario inside EAWebKit. What we need from InputMan is to give us the information if the pressed key is extended key.
			// For example, if the user presses extended "insert" key with numLock on, we would return true in following. 
			return (modifiers & kModifierMaskNumLock);//!(HIWORD(keyData) & KF_EXTENDED);
		default:
			return false;
	}
}

static inline String singleCharacterString(UChar c) { return String(&c, 1); }

PlatformKeyboardEvent::PlatformKeyboardEvent(const EA::WebKit::KeyboardEvent* keyBoardEvent)
: PlatformEvent(keyBoardEvent->mbDepressed ? (keyBoardEvent->mbChar ? Char : RawKeyDown) : KeyUp)
, m_eaKeyboardEvent(keyBoardEvent) 
, m_text((m_type == Char) ? singleCharacterString(keyBoardEvent->mId) : String())
, m_unmodifiedText((m_type == Char) ? singleCharacterString(keyBoardEvent->mId) : String())
, m_keyIdentifier((m_type == Char) ? String() : keyIdentifierForEAKeyCode(keyBoardEvent->mId))
// Our key codes defined in the EAWebKitInput.h map to InputMan which itself map to Windows virtual key code.
, m_windowsVirtualKeyCode((m_type == RawKeyDown || m_type == KeyUp) ? keyBoardEvent->mId : 0)
, m_nativeVirtualKeyCode(m_windowsVirtualKeyCode)
, m_autoRepeat(false)
, m_isKeypad(isKeypadEvent(keyBoardEvent->mId, keyBoardEvent->mModifiers, (PlatformEvent::Type) m_type))
{
    m_modifiers = 0;
    m_modifiers = (keyBoardEvent->mModifiers & EA::WebKit::kModifierMaskShift)     ? m_modifiers | ShiftKey  : m_modifiers;
    m_modifiers = (keyBoardEvent->mModifiers & EA::WebKit::kModifierMaskControl)   ? m_modifiers | CtrlKey   : m_modifiers;
    m_modifiers = (keyBoardEvent->mModifiers & EA::WebKit::kModifierMaskAlt)       ? m_modifiers | AltKey    : m_modifiers;
    m_modifiers = (keyBoardEvent->mModifiers & EA::WebKit::kModifierMaskOS)        ? m_modifiers | MetaKey   : m_modifiers;

	m_timestamp = WTF::currentTime();
}

void PlatformKeyboardEvent::disambiguateKeyDownEvent(Type, bool)
{
	// No KeyDown events for us to disambiguate.
	ASSERT_NOT_REACHED();
}

bool PlatformKeyboardEvent::currentCapsLockState()
{
	// This is being used only from RenderTextControlSingleLine::capsLockStateMayHaveChanged().
	// Can't use m_eaKeyboardEvent here since this is a static function. Ported the implementation from Qt port.
	notImplemented();
	return false;
}

void PlatformKeyboardEvent::getCurrentModifierState(bool& shiftKey, bool& ctrlKey, bool& altKey, bool& metaKey)
{
	// This is being used in DragController and EventHandler.
	// Can't use m_eaKeyboardEvent here since this is a static function. Ported the implementation from Qt port.
	notImplemented();
	shiftKey = false;
	ctrlKey = false;
	altKey = false;
	metaKey = false;
}

}

