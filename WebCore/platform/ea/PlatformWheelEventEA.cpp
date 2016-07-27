/*
    Copyright (C) 2008 Nokia Corporation and/or its subsidiary(-ies)
	
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

// Note by Arpit Baldeva: This file is called WheelEventQt.cpp/WheelEventWin.cpp in those ports but it is confusing as there is a WheelEvent.h and PlatformWheelEvent.h. 
// This is also not in sync with other files for keyboard/mouse. I decided to call it PlatformWheelEventEA.cpp.

#include "config.h"
#include "WheelEvent.h"

#include "PlatformMouseEvent.h"
#include "PlatformWheelEvent.h"
#include "Scrollbar.h"

#include <EAWebKit/EAWebKitInput.h>

namespace WebCore {

// Note by Arpit Baldeva: The calculation of m_deltaY and m_wheelTicksY is different from out last port. In our last port, m_deltaY was 
// simply the line delta and we translated it into the "pixels to scroll" in ScrollView. m_wheelTicksY was ignored.
// I looked at both the Qt and Windows port and pixels to scroll are calculated right here. So we do the same. 

void PlatformWheelEvent::applyDelta(int delta, float numLines, bool verticalScroll)
{
    // A delta that is not mod 120 indicates a device that is sending
    // fine-resolution scroll events, so use the delta as number of wheel ticks
    // and number of pixels to scroll.See also webkit.org/b/29601
    bool fullTick = !(delta % 120);

    if (verticalScroll) {
		m_deltaX = 0;
		m_deltaY = (fullTick) ? delta / 120.0f : delta;
    } else {
		m_deltaX = (fullTick) ? delta / 120.0f : delta;
		m_deltaY = 0;
    }

    // Note by Arpit Baldeva: It seems rather questionable to me that the wheelTicks are equal to delta for delta < 120.
	// We'll need to go through the bug listed above and verify that is the case. It should be rather uncommon to have this
	// case though as the minimum resolution supported by Windows is 120.

	m_wheelTicksX = m_deltaX;
    m_wheelTicksY = m_deltaY;

    m_deltaX *= (fullTick) ? numLines * WebCore::Scrollbar::pixelsPerLineStep() : 1;
	m_deltaY *= (fullTick) ? numLines * WebCore::Scrollbar::pixelsPerLineStep() : 1;
}



PlatformWheelEvent::PlatformWheelEvent(const EA::WebKit::MouseWheelEvent* wheelEvent)
{
	m_position			= IntPoint(wheelEvent->mX, wheelEvent->mY);
	m_globalPosition	= IntPoint(wheelEvent->mGlobalX, wheelEvent->mGlobalY);

    m_modifiers = 0;
    m_modifiers = (wheelEvent->mModifiers & EA::WebKit::kModifierMaskShift)     ? m_modifiers | ShiftKey  : m_modifiers;
    m_modifiers = (wheelEvent->mModifiers & EA::WebKit::kModifierMaskControl)   ? m_modifiers | CtrlKey   : m_modifiers;
    m_modifiers = (wheelEvent->mModifiers & EA::WebKit::kModifierMaskAlt)       ? m_modifiers | AltKey    : m_modifiers;
    m_modifiers = (wheelEvent->mModifiers & EA::WebKit::kModifierMaskOS)        ? m_modifiers | MetaKey   : m_modifiers;

	applyDelta(wheelEvent->mZDelta, fabs(wheelEvent->mNumLines), !shiftKey());

	m_granularity	= ScrollByPixelWheelEvent;
}
} // namespace WebCore
