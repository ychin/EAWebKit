/*
 * Copyright (C) 2010 Nokia Corporation and/or its subsidiary(-ies).
 * Copyright (C) 2011, 2012, 2014 Electronic Arts, Inc. All rights reserved.
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
 */

#ifndef PopupMenuEA_h
#define PopupMenuEA_h

#include "PopupMenu.h"
#include "IntRect.h"
#include "ScrollableArea.h"
#include <EAWebKit/EAWebKitInput.h>
 

namespace EA {
    namespace WebKit {
        class ISurface; 
        class View;
    }
}

namespace WebCore {

class ChromeClientEA;
class FrameView;
class PopupMenuClient;
class IntRect;
class RenderText;
class PopupInputClient;
class Scrollbar;

class PopupMenuEA : public PopupMenu, public EA::WebKit::IOverlayInputClient, private ScrollableArea
{
public:
    PopupMenuEA(PopupMenuClient*, const ChromeClientEA*);
    ~PopupMenuEA();

    virtual void show(const IntRect&, FrameView*, int index) OVERRIDE;
    virtual void hide() OVERRIDE;
    virtual void updateFromElement() OVERRIDE;
    virtual void disconnectClient() OVERRIDE;


    // Overlay event handling (mostly input)
    void OverlayBegin(void);
	void OverlayEnd(void);
    void OnKeyboardEvent(const EA::WebKit::KeyboardEvent& keyboardEvent);
	void OnMouseMoveEvent(const EA::WebKit::MouseMoveEvent& mouseMoveEvent);
	void OnMouseButtonEvent(const EA::WebKit::MouseButtonEvent& mouseButtonEvent);
	void OnMouseWheelEvent(const EA::WebKit::MouseWheelEvent& mouseWheelEvent);
	bool OnButtonEvent(const EA::WebKit::ButtonEvent& buttonEvent);     
	void OnFocusChangeEvent(bool hasFocus);               

private:
    // ScrollableArea
    virtual int scrollSize(ScrollbarOrientation) const OVERRIDE;
    virtual int scrollPosition(Scrollbar*) const OVERRIDE;
    virtual void setScrollOffset(const IntPoint&) OVERRIDE;
    virtual void invalidateScrollbarRect(Scrollbar*, const IntRect&) OVERRIDE { } ;
    virtual void invalidateScrollCornerRect(const IntRect&) OVERRIDE { }
    virtual bool isActive() const OVERRIDE { return true; }
    ScrollableArea* enclosingScrollableArea() const OVERRIDE { return 0; }
    virtual bool isScrollCornerVisible() const OVERRIDE { return false; }
    virtual IntRect scrollCornerRect() const OVERRIDE { return IntRect(); }
    virtual Scrollbar* verticalScrollbar() const OVERRIDE { return m_scrollBar.get(); }
    virtual int visibleHeight() const OVERRIDE;
    virtual int visibleWidth() const OVERRIDE;
    virtual IntSize contentsSize() const OVERRIDE;
    virtual IntRect scrollableAreaBoundingBox() const OVERRIDE;
    virtual bool updatesScrollLayerPositionOnMainThread() const OVERRIDE { return true; }

private:
    int  getNextFocusIndex(const int delta);
    void validateIndex(int& requestedIndex);
    void validateScrollOffset(int& requestedOffset);
    void updateScrollOffset(const int requestedOffset);
    void updateFocusIndex(int index, bool updateSelected = false, bool updateScroll = false);
    bool layout(void);
    void draw(void);
    void drawScrollBar(void);
    
private:
    PopupMenuClient* m_popupClient;
	const ChromeClientEA* m_chromeClient;
    int m_itemCount;                // Total count 
    int m_numItemsVisible;          // Visible items in the popup at one time (for scroll)
    int m_itemScrollOffset;         // Start offset of visible items (for scroll)   
    int m_scrollOffset;             // For scrollable area to keep the Y offset.
    int m_itemHeight;               // Height of a single item
    int m_focusedIndex;             // The currently focused index
    IntRect m_selectElementRect;	// The select element rect on page(some text and dropdown arrow). In absolute coordinates. 
    IntRect m_poppedUpSurfaceRect;  // The popped up surface we create(contains the item text/scroll bar etc.). In Window coordinates.
    IntRect m_scrollRect;
    IntRect m_mouseScrollRect;      // Same as scroll Rect but with some extra x padding
    bool m_flipped;                 // Flip the menu up if not enough room under
    bool m_scrollBarActive;
    bool m_scrollbarCapturingMouse; // For thumb dragging
    bool m_scrollbarWasUnderMouse;  // For updating mouse over events
    
    const FrameView* m_frameView;
    EA::WebKit::View* m_viewEA;     
    EA::WebKit::ISurface* m_surface;
    RefPtr<Scrollbar> m_scrollBar;
};

}

#endif // PopupMenuEA_h
