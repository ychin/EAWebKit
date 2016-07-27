/*
 * Copyright (C) 2007, 2008 Apple Inc. All Rights Reserved.
 * Copyright (C) 2007 Staikos Computing Services Inc. <info@staikos.net>
 * Copyright (C) 2008 Nokia Corporation and/or its subsidiary(-ies)
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
 * THIS SOFTWARE IS PROVIDED BY APPLE INC. ``AS IS'' AND ANY
 * EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE
 * IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR
 * PURPOSE ARE DISCLAIMED.  IN NO EVENT SHALL APPLE INC. OR
 * CONTRIBUTORS BE LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL,
 * EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT LIMITED TO,
 * PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES; LOSS OF USE, DATA, OR
 * PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND ON ANY THEORY
 * OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT
 * (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE
 * OF THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
 */

#include "config.h"
#include "ScrollbarThemeEA.h"

#include "GraphicsContext.h"
#include "PlatformMouseEvent.h"
#include "RenderThemeEA.h"
#include "ScrollView.h"
#include "Scrollbar.h"
#include <internal/include/EAWebKitAssert.h>
#include <EAWebKit/EAWebKit.h>
#include <internal/include/EAWebkit_p.h>

namespace ThemeEA
{
	const int kScrollBorderColor = 0xFF808080;
	const float kScrollThemeBorderThickness = 1.0f;
	const int kScrollTrackColor = 0xFFE4E4E4;
	const int kScrollCornerColor = 0xFFE4E4E4;
	const int kScrollButtonNormalColor = 0xFFCACACA;
	const int kScrollButtonHoveredColor = 0xFFDADADA;
	const int kScrollButtonPressedColor = 0xFFA8A8A8;
	const int kScrollButtonArrowColor = 0xFF000000;
	const int kScrollThumbNormalColor = 0xFFC4C4C4;
	const int kScrollThumbHoveredColor = 0xFFD0D0D0;
	const int kScrollThumbPressedColor = 0xFFA8A8A8;
	const int kScrollButtonLength = 16;
	const int kScrollThumbMinLength = 16;

	extern const int kScrollThickness = 16;

}

namespace WebCore {

enum ScrollButtonType{kScrollButtonBack,kScrollButtonForward};  // Scroll button types to go up/down and left/right.

static ScrollbarThemeEA* spThemeEA = NULL;

static void GetScrollButtonRect(ScrollButtonType type, ScrollbarOrientation orientation, const IntRect& r, IntRect& outRect)
{
    outRect = r;
    int length = ThemeEA::kScrollButtonLength;

    if(orientation == VerticalScrollbar)
    {
        const int h = r.height();
        if(h <= (length+length))
            length =0;  // This will invalidate the rect since there is not enough room.
        else if(type == kScrollButtonForward)
            outRect.setY(r.y() + h - length );            

        outRect.setHeight(length);
    }
    else
    {
        const int w = r.width();
        if(w <= (length+length))
            length =0;  // This will invalidate the rect since there is not enough room.
        else if(type == kScrollButtonForward)
            outRect.setX(r.x() + w - length );            

        outRect.setWidth(length);
    }
}

static int GetTrackRect(ScrollbarOrientation orientation, const IntRect& r, IntRect& outRect)
{
    int trackLength;    
    outRect = r;
    int buttonlength = ThemeEA::kScrollButtonLength;
    
    if(orientation == VerticalScrollbar)
    {
        trackLength = r.height();
        if(trackLength > (buttonlength + buttonlength))
        { 
            trackLength -=(buttonlength + buttonlength);
            outRect.setHeight(trackLength);
            outRect.setY(r.y() + buttonlength);            
        }
    }
    else
    {
        trackLength = r.width();
        if(trackLength > (buttonlength + buttonlength))
        {
            trackLength -=(buttonlength + buttonlength);
            outRect.setWidth(trackLength);
            outRect.setX(r.x() + buttonlength);    
        }
    }
  
    return trackLength;
}

static void GetThumbRect(ScrollbarOrientation orientation, int length, int offset, const IntRect& r, IntRect& outRect)
{
    const int kEdge= (int) ThemeEA::kScrollThemeBorderThickness;  // Have the thumb fit inside the track by using an edge offset.
    outRect = r;
    if(length >= ThemeEA::kScrollThumbMinLength)
    {    
        if(orientation == VerticalScrollbar)
        {
             outRect.setX(r.x() + kEdge);
             outRect.setWidth(r.width() - (kEdge << 1)); 

             outRect.setY(r.y() + offset);
             outRect.setHeight(length);
        }
        else
        {
            outRect.setX(r.x() + offset);
            outRect.setWidth(length);
        
             outRect.setY(r.y() + kEdge);
             outRect.setHeight(r.height() - (kEdge << 1));
        }    
    }
    else
    {
        // Case where we don't have enough space to display a thumb
        outRect.setWidth(0);
        outRect.setHeight(0);
    }
}


struct ScrollPaintInfo
{
    bool mEnabled;
    bool mVertical;
    bool mHovered;
    bool mPressed;

    ScrollPaintInfo(bool enabled, bool vert, bool hovered=false, bool pressed=false) : mEnabled(enabled),mVertical(vert)
        ,mHovered(hovered),mPressed(pressed) {}
};

static void paintButton(ScrollPaintInfo& info, ScrollButtonType type, GraphicsContext* context, const IntRect& r)
{
    if( r.isEmpty() || !info.mEnabled )
        return;

    // Draw the rect
    Color fillColor;
    if(info.mPressed)
        fillColor = ThemeEA::kScrollButtonPressedColor;
    else if (info.mHovered)
        fillColor = ThemeEA::kScrollButtonHoveredColor;
    else
        fillColor = ThemeEA::kScrollButtonNormalColor;

    context->setFillColor(fillColor, ColorSpaceDeviceRGB);
    context->setStrokeColor(ThemeEA::kScrollBorderColor, ColorSpaceDeviceRGB);
    context->setStrokeStyle(SolidStroke);
    context->drawRect(r);

    // Fill in the arrow
    const int kArrowSizeRatio = 5;    // 5 is just to get it small enough. 
    float wOffset = (float) (r.width()/kArrowSizeRatio);   
    float hOffset = (float) (r.height()/kArrowSizeRatio);
    const IntPoint center = r.center();    
    const FloatPoint c(center);
    FloatPoint pts[3];

    if(info.mVertical)
    {
         if(type == kScrollButtonBack)
            hOffset = -hOffset;
            
         pts[0].set( c.x(), (c.y() + hOffset) );         
         pts[1].set( (c.x() + wOffset), (c.y() - hOffset));         
         pts[2].set( (c.x() - wOffset), (c.y() - hOffset));        
    }
    else
    {
        if(type == kScrollButtonBack)
            wOffset = -wOffset;
            
         pts[0].set((c.x() + wOffset), c.y());         
         pts[1].set((c.x() - wOffset), (c.y() + hOffset));         
         pts[2].set((c.x() - wOffset), (c.y() - hOffset));         
    }
    context->setFillColor(ThemeEA::kScrollButtonArrowColor, ColorSpaceDeviceRGB);
    context->drawConvexPolygon(3, pts, true);

    // Highlight edges
    RenderThemeEA::paintEdgeHighligths(context,r);   

}

static void paintTrack(ScrollPaintInfo& info, GraphicsContext* context, const IntRect& r)
{
    if(r.isEmpty())
        return;

    Color fillColor = ThemeEA::kScrollTrackColor;
    context->setFillColor(fillColor, ColorSpaceDeviceRGB);   
    context->setStrokeColor(ThemeEA::kScrollBorderColor, ColorSpaceDeviceRGB);
    context->setStrokeStyle(SolidStroke);
    context->setStrokeThickness(ThemeEA::kScrollThemeBorderThickness);        // The Cairo draw rect hardcodes the stroke thickness to 1.0 but we set it here anyway for better compatibility
    context->drawRect(r);                                        
}

static void paintThumb(ScrollPaintInfo& info, GraphicsContext* context, const IntRect& r) 
{
    if(r.isEmpty() || !info.mEnabled) 
        return;
    
    // Fill
    Color fillColor; 
    if(info.mPressed)
        fillColor = ThemeEA::kScrollThumbPressedColor;
    else if (info.mHovered)
        fillColor = ThemeEA::kScrollThumbHoveredColor;
    else
        fillColor = ThemeEA::kScrollThumbNormalColor;
    
    context->setFillColor(fillColor, ColorSpaceDeviceRGB);  
    context->setStrokeStyle(NoStroke);
    context->drawRect(r);

     // Highlight edges
    RenderThemeEA::paintEdgeHighligths(context,r);   

    // Draw Grip
    IntPoint a,b; 
    IntPoint center = r.center();
    int dx=0;
    int dy=0;
    const int kGripCount = 3;       // 4 looks good too but is more expensive


    if(info.mVertical)
    {
        int w = r.width() >> 2;     // /4 makes the grip lines about 1/2 width.
        a.move(center.x() - w, center.y() - kGripCount);   
        b.move(center.x() + w, center.y() - kGripCount);   
        dy =1;                      // Step
    }
    else
    {
        int h = r.height() >> 2;
        a.move(center.x() - kGripCount, center.y() - h);   
        b.move(center.x() - kGripCount, center.y() + h); 
        dx =1;    
    }
    
    context->setStrokeStyle(SolidStroke);
    context->setStrokeThickness(1.0f);  // The grip is needs to be 1.0.  Large lines just don't work well.
    for(int i = 0; i < kGripCount; i++)
    {
        context->setStrokeColor(Color::white, ColorSpaceDeviceRGB);
        context->drawLine(a, b);
        a.move(dx,dy);
        b.move(dx,dy);
        context->setStrokeColor(ThemeEA::kScrollBorderColor, ColorSpaceDeviceRGB);
        context->drawLine(a, b);
        a.move(dx,dy);
        b.move(dx,dy);    
    }
}

// For static cleanup on exit
void ScrollbarThemeEA::staticFinalize()
{
    if(spThemeEA)
    {
        delete spThemeEA;
        spThemeEA = NULL;
    }    
}

ScrollbarTheme* ScrollbarTheme::nativeTheme()
{
    if(!spThemeEA)
    {
        spThemeEA = new ScrollbarThemeEA;
        EAW_ASSERT(spThemeEA);
    }
    return spThemeEA;
}

ScrollbarThemeEA::~ScrollbarThemeEA()
{
}

bool ScrollbarThemeEA::paint(ScrollbarThemeClient* scrollbar, GraphicsContext* context, const IntRect& damageRect)
{
    if (context->updatingControlTints())
    {
       scrollbar->invalidateRect(damageRect);
       return false;
    }

    const IntRect r = scrollbar->frameRect();
    if(!damageRect.intersects(r))
        return false;

    const ScrollbarOrientation  orientation = scrollbar->orientation();  
    const ScrollbarPart pressedPart = scrollbar->pressedPart();
    const ScrollbarPart hoveredPart = scrollbar->hoveredPart();

    ScrollPaintInfo paintInfo(scrollbar->enabled(), orientation == VerticalScrollbar);

    // Draw the parts. For now we redraw the full scroll bar to avoid some of the update bugs that we saw in 1.x but it might be possible to fine tune this.
    context->save();

    // Draw the track. 
    paintTrack(paintInfo, context, r);    // We draw the full scroll as track to fill in under the buttons and avoid a line with the buttons
    
    // Draw the buttons
    IntRect buttonRect;
    GetScrollButtonRect(kScrollButtonBack, orientation, r, buttonRect);
    paintInfo.mPressed = pressedPart == BackButtonStartPart;
    paintInfo.mHovered = hoveredPart == BackButtonStartPart;
    paintButton(paintInfo, kScrollButtonBack, context, buttonRect);
    
    GetScrollButtonRect(kScrollButtonForward, orientation, r, buttonRect);
    paintInfo.mPressed = pressedPart == ForwardButtonStartPart;
    paintInfo.mHovered = hoveredPart == ForwardButtonStartPart;
    
    paintButton(paintInfo, kScrollButtonForward, context, buttonRect);
    
    // Draw the thumb
    int length = thumbLength(scrollbar);
    int offset = thumbPosition(scrollbar);   
    paintInfo.mPressed = pressedPart == ThumbPart;
    paintInfo.mHovered = hoveredPart == ThumbPart;

    IntRect trackRect;
    IntRect thumbRect;
    GetTrackRect(orientation, r,trackRect);       
    GetThumbRect(orientation,length, offset, trackRect,thumbRect);    
    paintThumb(paintInfo, context, thumbRect);    

    context->restore();

    return true;
}

ScrollbarPart ScrollbarThemeEA::hitTest(ScrollbarThemeClient* scrollbar, const IntPoint& p)
{
    if(!scrollbar->enabled())
         return NoPart;

    IntRect r = scrollbar->frameRect();
    IntPoint pos = scrollbar->convertFromContainingWindow(p);
    pos.move(r.x(),r.y());

    // Check the buttons
    IntRect backRect;
    IntRect forwardRect;
   
    ScrollbarOrientation  orientation = scrollbar->orientation();  
    GetScrollButtonRect(kScrollButtonBack, orientation, r, backRect);
    GetScrollButtonRect(kScrollButtonForward, orientation, r, forwardRect);
   
    if(backRect.contains(pos))
        return BackButtonStartPart;

     if(forwardRect.contains(pos))
        return ForwardButtonStartPart;

    // Check the thumb
    IntRect trackRect;
    GetTrackRect(orientation, r, trackRect);
    int length = thumbLength(scrollbar);
    int offset = thumbPosition(scrollbar);   
    IntRect thumbRect;
    GetThumbRect(orientation,length, offset, trackRect,thumbRect);   
    if(thumbRect.contains(pos))
        return ThumbPart;

    //Check the track
    if(trackRect.contains(pos))
    {
        // Find out which side of the track we are on
        ScrollbarPart part = BackButtonStartPart;
        if(orientation == VerticalScrollbar)
        {
            if(pos.y() > thumbRect.y())
                part = ForwardButtonStartPart;        
        }
        else
        {
             if(pos.x() > thumbRect.x())
                part = ForwardButtonStartPart;  
        }
            
        return part;
    }

    // Nothing found
    return NoPart;
}

bool ScrollbarThemeEA::shouldCenterOnThumb(ScrollbarThemeClient*, const PlatformMouseEvent& evt)
{
    // Middle click centers slider thumb (if supported)
//    return style()->styleHint(QStyle::SH_ScrollBar_MiddleClickAbsolutePosition) && evt.button() == MiddleButton;

    return false;
}

void ScrollbarThemeEA::invalidatePart(ScrollbarThemeClient* scrollbar, ScrollbarPart)
{
    // FIXME: Do more precise invalidation.
    scrollbar->invalidate();
}

int ScrollbarThemeEA::scrollbarThickness(ScrollbarControlSize controlSize)
{
    return ThemeEA::kScrollThickness;
}

int ScrollbarThemeEA::thumbPosition(ScrollbarThemeClient* scrollbar)
{
    if (!scrollbar->enabled())
        return 0;
    
    int offset = (int)((float)scrollbar->currentPos() * (trackLength(scrollbar) - thumbLength(scrollbar)) / scrollbar->maximum());
    return offset;
}

int ScrollbarThemeEA::thumbLength(ScrollbarThemeClient* scrollbar)
{
     if(!scrollbar->enabled())
        return 0;   

    const float length = (float) trackLength(scrollbar);
    const float ratio = (float) scrollbar->visibleSize() / (float) scrollbar->totalSize();
    int thumbLength = (int) (length * ratio);
    if(thumbLength < ThemeEA::kScrollThumbMinLength)
    {
        if(length <=  ThemeEA::kScrollThumbMinLength)
            thumbLength = 0;                         // Too small so invalidate
        else
            thumbLength = ThemeEA::kScrollThumbMinLength;    // Clamp to a min size
    }
    return thumbLength;    
}

int ScrollbarThemeEA::trackPosition(ScrollbarThemeClient* scrollbar)
{
    IntRect r = scrollbar->frameRect();
    ScrollbarOrientation  orientation = scrollbar->orientation();      
    IntRect trackRect;
    GetTrackRect(orientation, r, trackRect);
    return (orientation == HorizontalScrollbar ? trackRect.x() - r.x() : trackRect.y() - r.y());
}

int ScrollbarThemeEA::trackLength(ScrollbarThemeClient* scrollbar)
{
    IntRect r = scrollbar->frameRect();
    ScrollbarOrientation  orientation = scrollbar->orientation();      
    IntRect trackRect;
    int length = GetTrackRect(orientation, r, trackRect);
    return length;
}

void ScrollbarThemeEA::paintScrollCorner(ScrollView* scrollView, GraphicsContext* context, const IntRect& rect)
{
    if (context->updatingControlTints()) {
       scrollView->invalidateRect(rect);
       return;
    }

    if(rect.isEmpty())
        return;

    // Draw
    context->save();
    context->setFillColor(ThemeEA::kScrollCornerColor, ColorSpaceDeviceRGB);
    context->setStrokeStyle(NoStroke);
    context->drawRect(rect);
    context->restore();
}


}

