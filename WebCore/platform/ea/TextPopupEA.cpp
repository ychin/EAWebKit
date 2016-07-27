/*
Copyright (C) 2010, 2011, 2012, 2014 Electronic Arts, Inc.  All rights reserved.

Redistribution and use in source and binary forms, with or without
modification, are permitted provided that the following conditions
are met:

1.  Redistributions of source code must retain the above copyright
    notice, this list of conditions and the following disclaimer.
2.  Redistributions in binary form must reproduce the above copyright
    notice, this list of conditions and the following disclaimer in the
    documentation and/or other materials provided with the distribution.
3.  Neither the name of Electronic Arts, Inc. ("EA") nor the names of
    its contributors may be used to endorse or promote products derived
    from this software without specific prior written permission.

THIS SOFTWARE IS PROVIDED BY ELECTRONIC ARTS AND ITS CONTRIBUTORS "AS IS" AND ANY
EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE IMPLIED
WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE ARE
DISCLAIMED. IN NO EVENT SHALL ELECTRONIC ARTS OR ITS CONTRIBUTORS BE LIABLE FOR ANY
DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES
(INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES;
LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND
ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT
(INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE OF
THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
*/

///////////////////////////////////////////////////////////////////////////////
// BCTextPopupEA.cpp
///////////////////////////////////////////////////////////////////////////////
#include "config.h"
#include "TextPopupEA.h"
#include "FontDescription.h"
#include "FontCache.h"
#include "FontPlatformData.h"
#include "IntPoint.h"
#include "IntRect.h"
#include "GraphicsContext.h"
#include "RefPtrCairo.h"
#include "TextRun.h"
#include <wtf/CurrentTime.h>
#include <EAWebKit/EAWebKitView.h>
#include <internal/include/EAWebKit_p.h>

namespace WebCore {

// Tooltip constants. We can make some of these configurable in the themeParmas if needed. 
const int kToolTipTextOffsetX   = 3;        // Offset from border
const int kToolTipTextOffsetY   = 2;        // Offset from border
const int kToolTipSlideX        = 30;       // Allow text to slide this amount if it can't fit on screen.
const int kToolTipMinWidth      = 24;       // Min width of tool tip if clipped.
const float kToolTipFontSize    = 11.0f;    // Default tool tip font size
const double kTooltipDelay      = 1.25;     // Default setting for how long in seconds to keep the popup on screen (avoid flicker)
const int kToolTipYOffset       = 12;       // Offset tooltip a bit from the mouse position so we don't hide the cursor.

TextPopup::TextPopup(EA::WebKit::View* pView)
    : mpViewEA(pView)
    , mpSurface(0)
    , mActive(false)
    , mX(0)
    , mY(0)
    , mWidth(0)
    , mHeight(0)
    , mTip()
    , mAdjustedTip()
    , mLastTime(0)
    , mPopupTimer(this, &TextPopup::timerFired)
{
}

TextPopup::~TextPopup()
{
    hide();
}
  
void TextPopup::timerFired(WebCore::Timer<TextPopup>* /*pTextPopup*/)
{
    const double curTime = currentTime();
    const double timeDelta = (curTime - mLastTime);
    if (timeDelta >=  kTooltipDelay)
    {
        hide();
    }
    else
    {
        // Keep going with the remainder. 
        mPopupTimer.startOneShot(timeDelta);
    }
}

void TextPopup::show(const char16_t* pTip, int x, int y) 
{
    // The FontCachePurgePreventer locks the font cache as long as it's in scope
    // This prevents any fonts being removed out from under us while we're accessing cached font data
    //
    FontCachePurgePreventer fontCachePurgePreventer;

    // Shut off if no text.
    if (!pTip)
    {
        hide();        
        return;
    }

    // Keep running if same. 
    if (mActive)
    {
        if(mTip.contains(pTip))
        {
            mLastTime = currentTime();  // Refresh time to delay the self destruct.
            return;
        }
        else
        {
            hide();    
        }
    }
    
    // We need to show a new tool tip.
    mTip = pTip;

    // Find font to use
    AtomicString stdfont(EA::WebKit::GetParameters().mFontFamilyStandard);
    if(stdfont.isEmpty())
        stdfont = "Arial";
    FontDescription fontDescription;
    fontDescription.setComputedSize(kToolTipFontSize);
    fontDescription.setSpecifiedSize(kToolTipFontSize);   
    
    WTF::RefPtr<SimpleFontData> pSFD = fontCache()->getCachedFontData(fontDescription, stdfont, true);
    const Font toolTipFont(pSFD->platformData(), true);
    
    // Get the length of the text run using the font.
    const int offset2X = kToolTipTextOffsetX + kToolTipTextOffsetX;
    const int offset2Y = kToolTipTextOffsetY + kToolTipTextOffsetY;
    const TextRun textRun(mTip.characters(), mTip.length());
    int width = toolTipFont.width(textRun) + offset2X;  
    const FontMetrics& fontMetrics = toolTipFont.fontMetrics();
    const int height = (int) fontMetrics.floatHeight() + offset2Y;

    // Adjust position based on screen location.
    EA::WebKit::IntSize screenSize = mpViewEA->GetSize();

    // x calculation.
    x = x < 0 ? 0 : x;
    int deltaX = screenSize.mWidth - (x + width); 
    if (deltaX > 0)
    {
        // Text fits.        
        mAdjustedTip = pTip;
    }
    else
    {
        // Text does not fit.
        if ((x > kToolTipSlideX) && (-deltaX < kToolTipSlideX))    
        {
            // Text fits if moved to the left.  
            x -= kToolTipSlideX;  
            mAdjustedTip = pTip;
        }
        else
        {
            //Text does not fit so need to chop off some of the characters.
            int requestedWidth = screenSize.mWidth - x - offset2X;
            int adjustedWidth = ElidedTextEA(toolTipFont, requestedWidth, kToolTipMinWidth, mTip, mAdjustedTip);
            if (!adjustedWidth)
                return;
            width = adjustedWidth + offset2X;
        }    
    }

    // Y calculation.
    y += kToolTipYOffset;        // We offset the Y position of the tooltip from the mouse pos so it does not hide the cursor.    
    y = y < 0 ? 0 : y;
    int deltaY = screenSize.mHeight - (y + height); 
    if (deltaY < 0)
    {
        // Does not fit in Y. 
        y += deltaY;
        if(y < 0)
             return;
     }
    
    // Set the new location and width/height.
    mX = x;
    mY = y;
    mWidth = width;
    mHeight = height;

    // Create and draw the popup surface.
    mpSurface = mpViewEA->CreateOverlaySurface(x, y, width, height);
    draw(toolTipFont);

    // Set up self destruct timer.
    mLastTime = currentTime();
    mPopupTimer.startOneShot(kTooltipDelay);
    mActive = true;
}

// Shuts down the popup.
void TextPopup::hide(void)
{
    if (mPopupTimer.isActive())
        mPopupTimer.stop();

    // Remove from overlay draw list.
    if (mActive)   
    {
        mpViewEA->DestroyOverlaySurface(mpSurface);
        mpSurface = 0;
    }

    mLastTime = 0;
    mActive = false;
}

// This draws to the pop up texture to the overlay (not to the main view).
void TextPopup::draw(const Font& font)
{
    // Set up the draw context. Popups draw to their own surface which then gets pasted on top of the view after the frame render. 
    EA::WebKit::ISurface::SurfaceDescriptor surfaceDescriptor = {0};
    mpSurface->Lock(&surfaceDescriptor);    
    
    int surfaceW = 0;
    int surfaceH = 0;
    mpSurface->GetContentDimensions(&surfaceW, &surfaceH);
    
    RefPtr<cairo_surface_t> surface = adoptRef(cairo_image_surface_create_for_data((unsigned char*)surfaceDescriptor.mData, CAIRO_FORMAT_ARGB32, surfaceW, surfaceH, surfaceDescriptor.mStride));    
	RefPtr<cairo_t> cairoContext = adoptRef(cairo_create(surface.get()));
	WebCore::GraphicsContext context(cairoContext.get());    
    
    // Draw box and background.    
    context.setFillColor(Color::white, ColorSpaceDeviceRGB);
    context.setStrokeStyle(SolidStroke);
    context.setStrokeColor(Color::black, ColorSpaceDeviceRGB);
    const IntRect windowRect(0, 0, surfaceW, surfaceH);
    context.drawRect(windowRect);

   // Draw the text. 
    context.setFillColor(Color::black, ColorSpaceDeviceRGB);
    const TextRun textRun(mAdjustedTip.characters(), mAdjustedTip.length());
    context.drawText(font, textRun, IntPoint(kToolTipTextOffsetX, (int) kToolTipFontSize + kToolTipTextOffsetY));   

    mpSurface->Unlock();   

    // Request a redraw so that the new overlay gets transfered to the main view surface.
    EA::WebKit::IntRect dirtyRegion(mX, mY, surfaceW, surfaceH);
    mpViewEA->AddDirtyRegion(dirtyRegion);    
}

// Utility to trim a text string to a target width.
int ElidedTextEA(const Font& f, int width, int minWidth, const String& inStr, String &outStr)
{     
    int outWidth = 0;
    const int charCount = inStr.length();
    const TextRun textRun(inStr.characters(), charCount);
    int runWidth = f.width(textRun);
    if (runWidth <= width) 
    {
        // Text fits.
        outWidth = width;
        outStr  = inStr;
    }
    else
    {
        // Text does not fit so try to clip it .
        const int kAdjustCharCount = 3 + 3;         // 3 for "..." + some error margin. 
        float ratio = 0.0f;
        if (runWidth)
        {    
            ratio = (float) width / (float) runWidth; 
        }
        
        int nCount =  (int) ((float) charCount * ratio) - kAdjustCharCount;
        if (nCount > 0) 
        {
            String ellipsisStr("...", 3);                

            String adjustedStr(inStr.characters(), nCount); 
            
            adjustedStr.append(ellipsisStr);

            // Get the new width.
            const TextRun run(adjustedStr.characters(), adjustedStr.length());
            outWidth = f.width(run);    
            outStr = adjustedStr;
        }
        else
        {   
            // Not enough room to even clip the text.
            outWidth = 0;
            outStr = "";
        }
    }
    return outWidth;
}


} // namespace WebCore

