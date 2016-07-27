/*
    Copyright (C) 2010 Nokia Corporation and/or its subsidiary(-ies)
	Copyright (C) 2011, 2014 Electronic Arts, Inc. All rights reserved.

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

#include "FrameNetworkingContextEA.h"

#include <WebFrame.h>
#include <WebPage.h>

namespace WebCore {

FrameNetworkingContextEA::FrameNetworkingContextEA(Frame* frame, void* originatingObject, bool mimeSniffingEnabled)
    : FrameNetworkingContext(frame)
    , m_originatingObject(originatingObject)
	, m_mimeSniffingEnabled(mimeSniffingEnabled)
{
}

PassRefPtr<FrameNetworkingContextEA> FrameNetworkingContextEA::create(Frame* frame, void* originatingObject, bool mimeSniffingEnabled)
{
    return adoptRef(new FrameNetworkingContextEA(frame, originatingObject,mimeSniffingEnabled));
}

void* FrameNetworkingContextEA::originatingObject() const
{
    return m_originatingObject;
}

}
