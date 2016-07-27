/*
Copyright (C) 2010, 2011, 2012 Electronic Arts, Inc.  All rights reserved.

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
// TextPopupEA.h
///////////////////////////////////////////////////////////////////////////////

// This is to support simple text popups windows like tooltips but could be used
// for other text messages too.

#ifndef TextPopupEA_h
#define TextPopupEA_h

#include "font.h"
#include "Timer.h"

namespace EA
{
    namespace WebKit 
    {
        class View;
        class ISurface; 
    }
}

namespace WebCore {

class TextPopup
{
public:
    
    TextPopup(EA::WebKit::View*  view);
    virtual ~TextPopup();

    void show(const char16_t* pTip, int x, int y);
    void hide(void); 
    
private:    
    void timerFired(WebCore::Timer<TextPopup>*);
    void draw(const Font& font);
   
    EA::WebKit::View* mpViewEA;                 // 
    EA::WebKit::ISurface* mpSurface;            // Overlay surface used by tooltip if active.
    bool mActive;                               // True if tool tip is actively displayed.    
    int  mX;                                    // X view location of tooltip widget
    int  mY;                                    //
    int  mWidth;                                // Widget width
    int  mHeight;                               //
    WTF::String mTip;                           // Source tool tip text.    
    WTF::String mAdjustedTip;                   // Adjusted tool tip text (possibly clipped to fit on screen).
    double mLastTime;                           // Last time the tooltip was refreshed. Used to determine if it should be shut off.   
    WebCore::Timer<TextPopup>   mPopupTimer;    // Timer to trigger auto shut down of popup.    
 };

int ElidedTextEA(const Font& f, int width, int minWidth, const String& inStr, String &outStr);

}  // namespace WebCore

#endif //TextPopupEA_h

