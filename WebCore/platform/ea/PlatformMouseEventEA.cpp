/*
 * Copyright (C) 2006 Zack Rusin <zack@kde.org>
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

#include "config.h"
#include "PlatformMouseEvent.h"

#include <EAWebKit/EAWebKitInput.h>
#include <wtf/CurrentTime.h>

namespace WebCore {

PlatformMouseEvent::PlatformMouseEvent(const EA::WebKit::MouseButtonEvent* buttonEvent)
{
	using namespace EA::WebKit;
	
	m_position			= IntPoint(buttonEvent->mX, buttonEvent->mY);
	m_globalPosition	= IntPoint(buttonEvent->mGlobalX,buttonEvent->mGlobalY);
	
	switch(buttonEvent->mId)
	{
	case kMouseLeft:
		m_button = LeftButton;
		break;
	case kMouseMiddle:
		m_button = MiddleButton;
		break;
	case kMouseRight:
		m_button = RightButton;
		break;
	default:
		ASSERT_NOT_REACHED();
		m_button = NoButton;
		break;
	}

	if(buttonEvent->mbDepressed)
	{
		m_type  = MousePressed;
	}
	else
	{
		m_type  = MouseReleased;
	}

	m_clickCount = buttonEvent->mClickCount;

    m_modifiers = 0;
    m_modifiers = (buttonEvent->mModifiers & EA::WebKit::kModifierMaskShift)     ? m_modifiers | ShiftKey  : m_modifiers;
    m_modifiers = (buttonEvent->mModifiers & EA::WebKit::kModifierMaskControl)   ? m_modifiers | CtrlKey   : m_modifiers;
    m_modifiers = (buttonEvent->mModifiers & EA::WebKit::kModifierMaskAlt)       ? m_modifiers | AltKey    : m_modifiers;
    m_modifiers = (buttonEvent->mModifiers & EA::WebKit::kModifierMaskOS)        ? m_modifiers | MetaKey   : m_modifiers;
	
	m_timestamp = WTF::currentTime();
	
	// Note by Arpit Baldeva: Following is not used in the Qt port. On Windows port, it has a straight mapping to Windows modifiers WORD. So it seems like 
	// something specific to Windows port only.
	m_modifierFlags = buttonEvent->mModifiers;
}

PlatformMouseEvent::PlatformMouseEvent(const EA::WebKit::MouseMoveEvent* moveEvent)
{
	using namespace EA::WebKit;

	m_position			= IntPoint(moveEvent->mX, moveEvent->mY);
	m_globalPosition	= IntPoint(moveEvent->mGlobalX,moveEvent->mGlobalY);

	// WebKit prefers to keep this information here instead of EventHandler code now. The button info is important in order to enable the drag selection 
	// behavior when moving the mouse with the left button pressed.
	switch(moveEvent->mId)
	{
	case kMouseLeft:
		m_button = LeftButton;
		break;
	case kMouseMiddle:
		m_button = MiddleButton;
		break;
	case kMouseRight:
		m_button = RightButton;
		break;
	default:
		m_button = NoButton;
		break;
	}
	m_type		= MouseMoved;
	m_clickCount	= 0;

    m_modifiers = 0;
    m_modifiers = (moveEvent->mModifiers & EA::WebKit::kModifierMaskShift)     ? m_modifiers | ShiftKey  : m_modifiers;
    m_modifiers = (moveEvent->mModifiers & EA::WebKit::kModifierMaskControl)   ? m_modifiers | CtrlKey   : m_modifiers;
    m_modifiers = (moveEvent->mModifiers & EA::WebKit::kModifierMaskAlt)       ? m_modifiers | AltKey    : m_modifiers;
    m_modifiers = (moveEvent->mModifiers & EA::WebKit::kModifierMaskOS)        ? m_modifiers | MetaKey   : m_modifiers;

	m_timestamp = WTF::currentTime();

	// Note by Arpit Baldeva: Following is not used in the Qt port. On Windows port, it has a straight mapping to Windows modifiers WORD. 
	// This information is later passed to the ChromeClient which is port specific. So this can be ignored unless the port makes some use of it. 
	m_modifierFlags = moveEvent->mModifiers;
}
}

// vim: ts=4 sw=4 et
