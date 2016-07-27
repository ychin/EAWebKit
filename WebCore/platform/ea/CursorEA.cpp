/*
 * Copyright (C) 2010 Apple Inc. All rights reserved.
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

#include "config.h"
#include "Cursor.h"
#include "Image.h"
#include "IntPoint.h"
#include <internal/include/EAWebKitAssert.h>
#include <EAWebKit/EAWebKitClient.h>

namespace WebCore {

// We could have a switch to translate these but just simpler to directly map as it would only change with a trunk update.  If this assert is triggered, we need up keep the  
// EA::WebKit::CursorType in sync with Cursor::Type.  
COMPILE_ASSERT( (int)EA::WebKit::kCursorIdPointer					== (int) Cursor::Pointer,					CursorTypeOutOfSync);
COMPILE_ASSERT( (int)EA::WebKit::kCursorIdCross						== (int)Cursor::Cross,						CursorTypeOutOfSync);
COMPILE_ASSERT( (int)EA::WebKit::kCursorIdHand						== (int)Cursor::Hand,						CursorTypeOutOfSync);
COMPILE_ASSERT( (int)EA::WebKit::kCursorIdIBeam						== (int)Cursor::IBeam,						CursorTypeOutOfSync);
COMPILE_ASSERT( (int)EA::WebKit::kCursorIdWait						== (int)Cursor::Wait,						CursorTypeOutOfSync);
COMPILE_ASSERT( (int)EA::WebKit::kCursorIdHelp						== (int) Cursor::Help,						CursorTypeOutOfSync);
COMPILE_ASSERT( (int)EA::WebKit::kCursorIdEastResize				== (int) Cursor::EastResize,				CursorTypeOutOfSync);
COMPILE_ASSERT( (int)EA::WebKit::kCursorIdNorthResize				== (int) Cursor::NorthResize,				CursorTypeOutOfSync);
COMPILE_ASSERT( (int)EA::WebKit::kCursorIdNorthEastResize			== (int) Cursor::NorthEastResize,			CursorTypeOutOfSync);
COMPILE_ASSERT( (int)EA::WebKit::kCursorIdNorthWestResize			== (int) Cursor::NorthWestResize,			CursorTypeOutOfSync);
COMPILE_ASSERT( (int)EA::WebKit::kCursorIdSouthResize				== (int) Cursor::SouthResize,				CursorTypeOutOfSync);
COMPILE_ASSERT( (int)EA::WebKit::kCursorIdSouthEastResize			== (int) Cursor::SouthEastResize,			CursorTypeOutOfSync);
COMPILE_ASSERT( (int)EA::WebKit::kCursorIdSouthWestResize			== (int) Cursor::SouthWestResize,			CursorTypeOutOfSync);
COMPILE_ASSERT( (int)EA::WebKit::kCursorIdWestResize				== (int) Cursor::WestResize,				CursorTypeOutOfSync);
COMPILE_ASSERT( (int)EA::WebKit::kCursorIdNorthSouthResize			== (int) Cursor::NorthSouthResize,			CursorTypeOutOfSync);
COMPILE_ASSERT( (int)EA::WebKit::kCursorIdEastWestResize			== (int) Cursor::EastWestResize,			CursorTypeOutOfSync);
COMPILE_ASSERT( (int)EA::WebKit::kCursorIdNorthEastSouthWestResize	== (int) Cursor::NorthEastSouthWestResize,	CursorTypeOutOfSync);
COMPILE_ASSERT( (int)EA::WebKit::kCursorIdNorthWestSouthEastResize	== (int) Cursor::NorthWestSouthEastResize,	CursorTypeOutOfSync);
COMPILE_ASSERT( (int)EA::WebKit::kCursorIdColumnResize				== (int) Cursor::ColumnResize,				CursorTypeOutOfSync);
COMPILE_ASSERT( (int)EA::WebKit::kCursorIdRowResize					== (int) Cursor::RowResize,					CursorTypeOutOfSync);
COMPILE_ASSERT( (int)EA::WebKit::kCursorIdMiddlePanning				== (int) Cursor::MiddlePanning,				CursorTypeOutOfSync);
COMPILE_ASSERT( (int)EA::WebKit::kCursorIdEastPanning				== (int) Cursor::EastPanning,				CursorTypeOutOfSync);
COMPILE_ASSERT( (int)EA::WebKit::kCursorIdNorthPanning				== (int) Cursor::NorthPanning,				CursorTypeOutOfSync);
COMPILE_ASSERT( (int)EA::WebKit::kCursorIdNorthEastPanning			== (int) Cursor::NorthEastPanning,			CursorTypeOutOfSync);
COMPILE_ASSERT( (int)EA::WebKit::kCursorIdNorthWestPanning			== (int) Cursor::NorthWestPanning,			CursorTypeOutOfSync);
COMPILE_ASSERT( (int)EA::WebKit::kCursorIdSouthPanning				== (int) Cursor::SouthPanning,				CursorTypeOutOfSync);
COMPILE_ASSERT( (int)EA::WebKit::kCursorIdSouthEastPanning			== (int) Cursor::SouthEastPanning,			CursorTypeOutOfSync);
COMPILE_ASSERT( (int)EA::WebKit::kCursorIdSouthWestPanning			== (int) Cursor::SouthWestPanning,			CursorTypeOutOfSync);
COMPILE_ASSERT( (int)EA::WebKit::kCursorIdWestPanning				== (int) Cursor::WestPanning,				CursorTypeOutOfSync);
COMPILE_ASSERT( (int)EA::WebKit::kCursorIdMove						== (int) Cursor::Move,						CursorTypeOutOfSync);
COMPILE_ASSERT( (int)EA::WebKit::kCursorIdVerticalText				== (int) Cursor::VerticalText,				CursorTypeOutOfSync);
COMPILE_ASSERT( (int)EA::WebKit::kCursorIdCell						== (int) Cursor::Cell,						CursorTypeOutOfSync);
COMPILE_ASSERT( (int)EA::WebKit::kCursorIdContextMenu				== (int) Cursor::ContextMenu,				CursorTypeOutOfSync);
COMPILE_ASSERT( (int)EA::WebKit::kCursorIdAlias						== (int) Cursor::Alias,						CursorTypeOutOfSync);
COMPILE_ASSERT( (int)EA::WebKit::kCursorIdProgress					== (int) Cursor::Progress,					CursorTypeOutOfSync);
COMPILE_ASSERT( (int)EA::WebKit::kCursorIdNoDrop					== (int) Cursor::NoDrop,					CursorTypeOutOfSync);
COMPILE_ASSERT( (int)EA::WebKit::kCursorIdCopy						== (int) Cursor::Copy,						CursorTypeOutOfSync);
COMPILE_ASSERT( (int)EA::WebKit::kCursorIdNone						== (int) Cursor::None,						CursorTypeOutOfSync);
COMPILE_ASSERT( (int)EA::WebKit::kCursorIdNotAllowed				== (int) Cursor::NotAllowed,				CursorTypeOutOfSync);
COMPILE_ASSERT( (int)EA::WebKit::kCursorIdZoomIn					== (int) Cursor::ZoomIn,					CursorTypeOutOfSync);
COMPILE_ASSERT( (int)EA::WebKit::kCursorIdZoomOut					== (int) Cursor::ZoomOut,					CursorTypeOutOfSync);
COMPILE_ASSERT( (int)EA::WebKit::kCursorIdGrab						== (int) Cursor::Grab,						CursorTypeOutOfSync);
COMPILE_ASSERT( (int)EA::WebKit::kCursorIdGrabbing					== (int) Cursor::Grabbing,					CursorTypeOutOfSync);
COMPILE_ASSERT( (int)EA::WebKit::kCursorIdCustom					== (int) Cursor::Custom,					CursorTypeOutOfSync);

Cursor::Cursor(const Cursor& other)
    : m_platformCursor(other.m_platformCursor)
{
}

Cursor::Cursor(Image* image, const IntPoint& hotSpot)
    : m_platformCursor(0)
{
}

Cursor::~Cursor()
{
}

Cursor& Cursor::operator=(const Cursor& other)
{
    m_platformCursor = other.m_platformCursor;
    return *this;
}

Cursor::Cursor(PlatformCursor c)
    : m_platformCursor(c)
{
}


Cursor* Cursor::m_spCursorArray = NULL;
Cursor* Cursor::GetCursorArray()
{
    if(!m_spCursorArray)
    {
        // We keep an array allcoated throughout. 
        m_spCursorArray = new Cursor[EA::WebKit::kCursorIdCount];
        EAW_ASSERT(m_spCursorArray);
    }
    return m_spCursorArray;
}

const Cursor& Cursor::EnumeratedType(Cursor::Type id)
{
    if((int)(id) >= (int)EA::WebKit::kCursorIdCount)
    {
        EAW_ASSERT_MSG((int)(id) < (int)EA::WebKit::kCursorIdCount,"Cursor type out of range. Falling back to pointer cursor type");
        id = Cursor::Pointer;
    }

    Cursor& cursor = Cursor::GetCursorArray()[id];
    
    cursor.m_platformCursor = id;
    return cursor;
}

const Cursor& pointerCursor()
{
    return Cursor::EnumeratedType(Cursor::Pointer);
}

const Cursor& crossCursor()
{
    return Cursor::EnumeratedType(Cursor::Cross);
}

const Cursor& handCursor()
{
    return Cursor::EnumeratedType(Cursor::Hand);
}

const Cursor& moveCursor()
{
    return Cursor::EnumeratedType(Cursor::Move);
}

const Cursor& verticalTextCursor()
{
    return Cursor::EnumeratedType(Cursor::VerticalText);
}

const Cursor& cellCursor()
{
    return Cursor::EnumeratedType(Cursor::Cell);
}

const Cursor& contextMenuCursor()
{
    return Cursor::EnumeratedType(Cursor::ContextMenu);
}

const Cursor& aliasCursor()
{
    return Cursor::EnumeratedType(Cursor::Alias);
}

const Cursor& zoomInCursor()
{
    return Cursor::EnumeratedType(Cursor::ZoomIn);
}

const Cursor& zoomOutCursor()
{
    return Cursor::EnumeratedType(Cursor::ZoomOut);
}

const Cursor& copyCursor()
{
    return Cursor::EnumeratedType(Cursor::Copy);
}

const Cursor& noneCursor()
{
    return Cursor::EnumeratedType(Cursor::None);
}

const Cursor& progressCursor()
{
    return Cursor::EnumeratedType(Cursor::Progress);
}

const Cursor& noDropCursor()
{
    return Cursor::EnumeratedType(Cursor::NoDrop);
}

const Cursor& notAllowedCursor()
{
    return Cursor::EnumeratedType(Cursor::NotAllowed);
}

const Cursor& iBeamCursor()
{
    return Cursor::EnumeratedType(Cursor::IBeam);
}

const Cursor& waitCursor()
{
    return Cursor::EnumeratedType(Cursor::Wait);
}

const Cursor& helpCursor()
{
    return Cursor::EnumeratedType(Cursor::Help);
}

const Cursor& eastResizeCursor()
{
    return Cursor::EnumeratedType(Cursor::EastResize);
}

const Cursor& northResizeCursor()
{
    return Cursor::EnumeratedType(Cursor::NorthResize);
}

const Cursor& northEastResizeCursor()
{
    return Cursor::EnumeratedType(Cursor::NorthEastResize);
}

const Cursor& northWestResizeCursor()
{
    return Cursor::EnumeratedType(Cursor::NorthWestResize);
}

const Cursor& southResizeCursor()
{
    return Cursor::EnumeratedType(Cursor::SouthResize);
}

const Cursor& southEastResizeCursor()
{
    return Cursor::EnumeratedType(Cursor::SouthEastResize);
}

const Cursor& southWestResizeCursor()
{
    return Cursor::EnumeratedType(Cursor::SouthWestResize);
}

const Cursor& westResizeCursor()
{
    return Cursor::EnumeratedType(Cursor::WestResize);
}

const Cursor& northSouthResizeCursor()
{
    return Cursor::EnumeratedType(Cursor::NorthSouthResize);
}

const Cursor& eastWestResizeCursor()
{
    return Cursor::EnumeratedType(Cursor::EastWestResize);
}

const Cursor& northEastSouthWestResizeCursor()
{
    return Cursor::EnumeratedType(Cursor::NorthEastSouthWestResize);
}

const Cursor& northWestSouthEastResizeCursor()
{
    return Cursor::EnumeratedType(Cursor::NorthWestSouthEastResize);
}

const Cursor& columnResizeCursor()
{
    return Cursor::EnumeratedType(Cursor::ColumnResize);
}

const Cursor& rowResizeCursor()
{
    return Cursor::EnumeratedType(Cursor::RowResize);
}

const Cursor& middlePanningCursor()
{
    return Cursor::EnumeratedType(Cursor::MiddlePanning);
}
    
const Cursor& eastPanningCursor()
{
    return Cursor::EnumeratedType(Cursor::EastPanning);
}
    
const Cursor& northPanningCursor()
{
    return Cursor::EnumeratedType(Cursor::NorthPanning);
}
    
const Cursor& northEastPanningCursor()
{
    return Cursor::EnumeratedType(Cursor::NorthEastPanning);
}
    
const Cursor& northWestPanningCursor()
{
    return Cursor::EnumeratedType(Cursor::NorthWestPanning);
}
    
const Cursor& southPanningCursor()
{
    return Cursor::EnumeratedType(Cursor::SouthPanning);
}
    
const Cursor& southEastPanningCursor()
{
    return Cursor::EnumeratedType(Cursor::SouthEastPanning);
}
    
const Cursor& southWestPanningCursor()
{
    return Cursor::EnumeratedType(Cursor::SouthWestPanning);
}
    
const Cursor& westPanningCursor()
{
    return Cursor::EnumeratedType(Cursor::WestPanning);
}

const Cursor& grabCursor()
{
    return Cursor::EnumeratedType(Cursor::Grab);
}

const Cursor& grabbingCursor()
{
    return Cursor::EnumeratedType(Cursor::Grabbing);
}

} // Namespace WebCore





