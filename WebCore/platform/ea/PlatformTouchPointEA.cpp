/*
 * This file is part of the WebKit project.
 *
 * Copyright (C) 2009 Nokia Corporation and/or its subsidiary(-ies)
 * Copyright (C) 2011, 2012, 2014, 2015 Electronic Arts, Inc. All rights reserved.
 *
 * This library is free software; you can redistribute it and/or
 * modify it under the terms of the GNU Library General Public
 * License as published by the Free Software Foundation; either
 * version 2 of the License, or (at your option) any later version.
 *
 * This library is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
 * Library General Public License for more details.
 *
 * You should have received a copy of the GNU Library General Public License
 * along with this library; see the file COPYING.LIB.  If not, write to
 * the Free Software Foundation, Inc., 51 Franklin Street, Fifth Floor,
 * Boston, MA 02110-1301, USA.
 *
 */

#include "config.h"
#include "PlatformTouchPoint.h"

#include <EAWebKit/EAWebKitInput.h>

#if ENABLE(TOUCH_EVENTS)

namespace WebCore {

	PlatformTouchPoint::PlatformTouchPoint(const EA::WebKit::TouchPoint touchPoint)
	{
		using namespace EA::WebKit;
		IntPoint point((int)touchPoint.mX, (int)touchPoint.mY);
		m_pos = point;
		m_force = touchPoint.mStrength;
		m_id = touchPoint.mTouchId;

		switch (touchPoint.mState)
		{
		case TouchPointState::TouchPointPressed:
			m_state = State::TouchPressed;
			break;
		case TouchPointState::TouchPointMoved:
			m_state = State::TouchMoved;
			break;
		case TouchPointState::TouchPointReleased:
			m_state = State::TouchReleased;
			break;
		case TouchPointState::TouchPointCancelled:
			m_state = State::TouchCancelled;
			break;
		case TouchPointState::TouchPointStationary:
			m_state = State::TouchStationary;
			break;
		default:
			ASSERT_NOT_REACHED();
		}
	}
}
#endif
