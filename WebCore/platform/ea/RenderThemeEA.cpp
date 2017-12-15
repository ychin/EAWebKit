/*
 * This file is part of the WebKit project.
 *
 * Copyright (C) 2008 Nokia Corporation and/or its subsidiary(-ies)
 * Copyright (C) 2011, 2012, 2013, 2014 Electronic Arts, Inc. All rights reserved.
 *
 * Copyright (C) 2006 Zack Rusin <zack@kde.org>
 *               2006 Dirk Mueller <mueller@kde.org>
 *               2006 Nikolas Zimmermann <zimmermann@kde.org>
 * Copyright (C) 2008 Holger Hans Peter Freyther
 *
 * All rights reserved.
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
#include "RenderThemeEA.h"

#include "CSSStyleSheet.h"
#include "CSSFontSelector.h"
#include "Chrome.h"
#include "ChromeClientEA.h"
#include "Color.h"
#include "Document.h"
#include "Font.h"
#include "FontDescription.h"
#include "FontSelector.h"
#include "GraphicsContext.h"
#include "HTMLInputElement.h"
#include "HTMLMediaElement.h"
#include "HTMLNames.h"
#include "NotImplemented.h"
#include "Page.h"
#include "WebPageClientEA.h"
#include "StyleOptionWebComboBoxEA.h"
#include "StyleResolver.h"
#include "RenderBox.h"
#include "RenderObject.h"
#if ENABLE(PROGRESS_ELEMENT)
#include "RenderProgress.h"
#endif
#include "RenderSlider.h"
#include "RenderTheme.h"
#include "TimeRanges.h"
#include "UserAgentStyleSheets.h"
#include <EAWebKit/EAWebKit.h>
#include <internal/include/EAWebkit_p.h>
#include <internal/include/EAWebKitAssert.h>
#include "PaintInfo.h"
#include "MediaControlElements.h"
#include "Element.h"
#include "Frame.h"
#include "FrameSelection.h"

namespace ThemeEA
{
    // These values all match Safari/Win/Chromium
    const float kDefaultCancelButtonSize = 9;
    const float kMinCancelButtonSize = 5;
    const float kMaxCancelButtonSize = 21;
	const float kDefaultSearchFieldResultsDecorationSize = 13;
	const float kMinSearchFieldResultsDecorationSize = 9;
	const float kMaxSearchFieldResultsDecorationSize = 30;
	const float kDefaultSearchFieldResultsButtonWidth = 18;

	const int kTextFieldCursorPaddingVertical = 1;  // Offsets the caret from the top and bottom of a text field to help with centering. 
	const int kTextFieldCursorPaddingLeft = 2;      // Offsets the caret from the left edge of a text field so it is visible. 
	const int kTextFieldBorderColor = 0xFF808080;

	const int kPopupMenuArrowSize = 16;     // Area reserved for the menu list arrow
	const double kBlinkFrequency = 0.5;     // Caret blink rate in secs
	const int kSliderThumbWidth = 6;        // Slider thumb with.  0 disables the slider.
	const int kSliderThumbHeight = 16;      // Slider thumb Height. 0 disables the slider  
	const int kSliderThumbBorderColor = 0xff909090;
	const int kSliderThumbFillColor = 0xffe0e0e0;
	const int kSliderTrackColor = 0xffa8a8a8;
	const int kSliderTrackHighlight = 0xfff0f0f0;
	extern const int kPopupMenuBorderPadding = 2;  // Extra spacing between the menu list text and the menu list arrow.
	extern const int kScrollThickness;
}

namespace WebCore {

static void FillRectWithSolidBorder(const PaintInfo& i, const IntRect& r, const Color fillColor = Color::lightGray, const Color strokeColor = Color::black)
{
    i.context->save();
    
	i.context->setFillColor(fillColor, ColorSpaceDeviceRGB);
    i.context->setStrokeColor(strokeColor, ColorSpaceDeviceRGB);
    i.context->setStrokeStyle(SolidStroke);
    i.context->setStrokeThickness(1.0f);
    i.context->drawRect(r);   
    
	i.context->restore();
}

static void FillRectWithoutBorder(const PaintInfo& i, const IntRect& r, const Color fillColor = Color::lightGray)
{
    i.context->save();
   
	i.context->setFillColor(fillColor, ColorSpaceDeviceRGB);
    i.context->setStrokeStyle(NoStroke);
    i.context->drawRect(r);   
    
	i.context->restore();
}

PassRefPtr<RenderTheme> RenderThemeEA::create(Page* page)
{
    return adoptRef(new RenderThemeEA(page));
}

PassRefPtr<RenderTheme> RenderTheme::themeForPage(Page* page)
{
    if (page)
        return RenderThemeEA::create(page);

    static RenderTheme* fallback = RenderThemeEA::create(0).leakRef();
    return fallback;
}

RenderThemeEA::RenderThemeEA(Page* /*page*/)
    : RenderTheme()
{
    m_buttonFontFamily = EA::WebKit::GetParameters().mFontFamilyStandard;    
}

RenderThemeEA::~RenderThemeEA()
{
}

void RenderThemeEA::addIntrinsicMargins(RenderStyle* style) const
{
    // addIntrinsicMargins code is taken from RenderThemeAndroid.cpp
    // Cut out the intrinsic margins completely if we end up using a small font size
    if (style->fontSize() < 11)
        return;
    
    // Intrinsic margin value.
    const int m = 2;
    
    // FIXME: Using width/height alone and not also dealing with min-width/max-width is flawed.
    if (style->width().isIntrinsicOrAuto()) {
        if (style->marginLeft().quirk())
            style->setMarginLeft(Length(m, Fixed));
        if (style->marginRight().quirk())
            style->setMarginRight(Length(m, Fixed));
    }

    if (style->height().isAuto()) {
        if (style->marginTop().quirk())
            style->setMarginTop(Length(m, Fixed));
        if (style->marginBottom().quirk())
            style->setMarginBottom(Length(m, Fixed));
    }
}

bool RenderThemeEA::supportsHover(const RenderStyle*) const
{
    return true;
}

bool RenderThemeEA::supportsFocusRing(const RenderStyle* style) const
{
	// Following is unintuitive (negating focus ring enable bool) but correct thing to do. In fact, this fixes a bug due to which the focus ring does not show up around an input
	// element unless you customize css. 
	// WebCore is asking here if our theme can by itself does has a focus ring (like some native OS controls). And if yes, it will skip
	// drawing focus ring. All our controls are drawn in software without a focus ring so the correct return value is false if we actually
	// want to kick in the focus ring draw path. Rest can be controlled via css on page.

	return !EA::WebKit::GetThemeParameters().mEnableFocusRingDraw;
}

int RenderThemeEA::baselinePosition(const RenderObject* o) const
{
    if (!o->isBox())
        return 0;

    if (o->style()->appearance() == CheckboxPart || o->style()->appearance() == RadioPart)
        return toRenderBox(o)->marginTop() + toRenderBox(o)->height() - 2; // Same as in old khtml
    return RenderTheme::baselinePosition(o);
}

bool RenderThemeEA::controlSupportsTints(const RenderObject* o) const
{
    if (!isEnabled(o))
        return false;

    // Checkboxes only have tint when checked.
    if (o->style()->appearance() == CheckboxPart)
        return isChecked(o);

    // For now assume other controls have tint if enabled.
    return true;
}

bool RenderThemeEA::supportsControlTints() const
{
    return true;
}

Color RenderThemeEA::platformActiveSelectionBackgroundColor() const
{
    Color c(EA::WebKit::GetThemeParameters().mColorActiveSelectionBack);
    return c;
}

Color RenderThemeEA::platformInactiveSelectionBackgroundColor() const
{
    Color c(EA::WebKit::GetThemeParameters().mColorInactiveSelectionBack);
    return c;
}

Color RenderThemeEA::platformActiveSelectionForegroundColor() const
{
    Color c(EA::WebKit::GetThemeParameters().mColorActiveSelectionFore);
    return c;
}

Color RenderThemeEA::platformInactiveSelectionForegroundColor() const
{
    Color c(EA::WebKit::GetThemeParameters().mColorInactiveSelectionFore);
    return c;
}

Color RenderThemeEA::platformActiveTextSearchHighlightColor() const
{
    return Color(Color::transparent);
}

Color RenderThemeEA::platformInactiveTextSearchHighlightColor() const
{
	return Color(Color::transparent);
}

Color RenderThemeEA::platformActiveListBoxSelectionBackgroundColor() const
{
    return Color(Color::transparent);
}

Color RenderThemeEA::platformInactiveListBoxSelectionBackgroundColor() const
{
    return Color(Color::transparent);
}

Color RenderThemeEA::platformActiveListBoxSelectionForegroundColor() const
{
    return Color(Color::transparent);
}

Color RenderThemeEA::platformInactiveListBoxSelectionForegroundColor() const
{
    return Color(Color::transparent);
}

void RenderThemeEA::systemFont(CSSValueID propId, FontDescription& fd) const
{
    // This function is called to retrieve system font information. System fonts
    // refer to "operating system" fonts used in operating system GUI elements such
    // as menus, text inputs, buttons. For web browsers like Safari these system
    // fonts are not treated the same as HTML fonts.

    const EA::WebKit::Parameters& param = EA::WebKit::GetParameters();

	uint32_t fontSize = param.mSystemFontSize;
	if(propId == CSSValueWebkitSmallControl)
		fontSize -= 2;
	if (propId == CSSValueWebkitMiniControl)
		fontSize -= 4;

    if(param.mSystemFont[0])
    {
        fd.setOneFamily(param.mSystemFont);
        fd.setSpecifiedSize((float)param.mSystemFontSize);
        fd.setIsAbsoluteSize(true);
        fd.setSmallCaps(false);
        fd.setWeight(param.mSystemFontBold ? WebCore::FontWeightBold : WebCore::FontWeightNormal);
        fd.setItalic(param.mSystemFontItalic);
        fd.setGenericFamily(WebCore::FontDescription::NoFamily); // enum GenericFamilyType { NoFamily, StandardFamily, SerifFamily, SansSerifFamily, MonospaceFamily, CursiveFamily, FantasyFamily };
    }
}

int RenderThemeEA::minimumMenuListSize(RenderStyle*) const
{
    // 0 is the default but leaving here in case we want our own values.
    return 0;   
}

void RenderThemeEA::setCheckboxSize(RenderStyle* style) const
{
    // If the width and height are both specified, then we have nothing to do.
    if (!style->width().isIntrinsicOrAuto() && !style->height().isAuto())
        return;
    const Length length((int) EA::WebKit::GetThemeParameters().mCheckBoxSize, Fixed);
    style->setWidth(length);
    style->setHeight(length);
}

bool RenderThemeEA::paintCheckbox(RenderObject* o, const PaintInfo& i, const IntRect& r)
{
    const bool checkedFlag = isChecked(o);
	const bool enabledFlag = isEnabled(o);
    const bool hoveredFlag = isHovered(o);

    i.context->save();
    // Draw the outer box
    Color strokeCol(Color::black);
    Color fillCol(EA::WebKit::GetThemeParameters().mBoxEnabledFillColor);
    if(!enabledFlag)
    {
        strokeCol = EA::WebKit::GetThemeParameters().mBoxDisabledFillColor;
        fillCol = Color::white;
    }
    FillRectWithSolidBorder(i, r, fillCol, strokeCol);

    // Draw on mouse over
    if((hoveredFlag) && (enabledFlag))
    {
        strokeCol = EA::WebKit::GetThemeParameters().mHoverHighlightColor;
        fillCol = 0x0;       // 0 alpha shuts off the fill
        const IntRect selectRect(r.x() + 1, r.y() + 1, r.width() - 2, r.height() - 2);
        FillRectWithSolidBorder(i, selectRect, fillCol, strokeCol);
    }

    // Draw the check mark
    if(checkedFlag)
    {
        IntPoint a = r.minXMinYCorner(); 
        int halfW = r.width() >> 1;
        int halfH = r.height() >> 1;
        int quarterW = halfW >> 1;
        int quarterH = halfH >> 1;
        
        IntPoint left( a.x() + quarterW, a.y() + halfH );  
        IntPoint bottom( a.x() + halfW, a.y() + halfH + quarterH );  
        IntPoint right( a.x() + halfH + quarterW, a.y() + quarterH );

        i.context->setStrokeColor(EA::WebKit::GetThemeParameters().mCheckMarkColor, ColorSpaceDeviceRGB);
        i.context->setStrokeThickness(2.0f);
        i.context->drawLine(left, bottom);
        i.context->drawLine(bottom,right);       
    }
    i.context->restore();

    return false;
}

void RenderThemeEA::setRadioSize(RenderStyle* style) const
{
     // If the width and height are both specified, then we have nothing to do.
    if (!style->width().isIntrinsicOrAuto() && !style->height().isAuto())
        return;
    const Length length((int)EA::WebKit::GetThemeParameters().mRadioButtonSize, Fixed);
    style->setWidth(length);
    style->setHeight(length);
}

bool RenderThemeEA::paintRadio(RenderObject* o, const PaintInfo& i, const IntRect& r)
{
    const bool checkedFlag = isChecked(o);
	const bool enabledFlag = isEnabled(o);
    const bool hoveredFlag = isHovered(o);

    i.context->save();
    
    // Draw the outer circle
    Color strokeCol(Color::black);
    Color fillCol(EA::WebKit::GetThemeParameters().mBoxEnabledFillColor);
    if(!enabledFlag)
    {
        strokeCol = EA::WebKit::GetThemeParameters().mBoxDisabledFillColor;
        fillCol = Color::white;
    }
   
    i.context->setStrokeColor(strokeCol, ColorSpaceDeviceRGB);
    i.context->setFillColor(fillCol, ColorSpaceDeviceRGB);
    i.context->setStrokeStyle(SolidStroke);
    i.context->setStrokeThickness(1.0f);
    i.context->drawEllipse(r);

     // Draw on mouse over
    if((hoveredFlag) && (enabledFlag))
    {
        i.context->setFillColor(0, ColorSpaceDeviceRGB);    // 0 alpha shuts off the fill
        i.context->setStrokeColor(EA::WebKit::GetThemeParameters().mHoverHighlightColor, ColorSpaceDeviceRGB);
        IntRect selectRect(r.x() + 1, r.y() + 1, r.width() -2, r.height() -2 );
        i.context->drawEllipse(selectRect);
    }


    // Draw the radio select inner circle
    if(checkedFlag)
    {
        // Scale the rect by 50% to use as inner circle area. 
        // There seems to be a rounding issue with dawEllipse ints so added a +1 for odd values
        int w = r.width();
        int h = r.height();
        if(w&0x1)
            w++;
        if(h&0x1)
            h++;

        IntRect innerRect( r.x() + (w >>2), r.y() + (h >>2), (r.width() >>1), (r.height() >>1) );  
        i.context->setFillColor(EA::WebKit::GetThemeParameters().mCheckMarkColor, ColorSpaceDeviceRGB);
        i.context->setStrokeStyle(NoStroke);
        i.context->drawEllipse(innerRect);
    }
    i.context->restore();

    return false;
}

void RenderThemeEA::adjustButtonStyle(StyleResolver* resolver, RenderStyle* style, Element*) const
{
    // White-space is locked to pre
    style->setWhiteSpace(PRE);

    FontDescription fontDescription = style->fontDescription();
    fontDescription.setIsAbsoluteSize(true);
    fontDescription.setSpecifiedSize(style->fontSize());
    fontDescription.setComputedSize(style->fontSize());

    if(!fontDescription.familyCount())
		fontDescription.setOneFamily(m_buttonFontFamily);

    style->setFontDescription(fontDescription);
    style->font().update(resolver->fontSelector());

    style->setLineHeight(RenderStyle::initialLineHeight());
    setButtonSize(style);
    addIntrinsicMargins(style);
  
    // Rounded corners
    const int kBorderCorner = 3;
    WebCore::Length length(kBorderCorner,Fixed);
    LengthSize rad(length,length);
    style->setBorderRadius(rad);
}

void RenderThemeEA::paintEdgeHighligths(GraphicsContext* context,const IntRect& r)
{
	context->save();
	
	const IntPoint p1(r.x() + 1 , r.y() + 1);
	const IntPoint p2(r.x() + r.width() - 2, p1.y());
	const IntPoint p3(p1.x(), r.y() + r.height() - 2);
	const IntPoint p4(p2.x(), p3.y());
	const IntPoint p5(r.x() + r.width() - 1, p3.y());   // Seems the Cairo draw line clips out what it considers the end point 
	
	context->setStrokeThickness(1.0f); 
	context->setStrokeStyle(SolidStroke);
	context->setStrokeColor(Color::black, ColorSpaceDeviceRGB);
	context->drawLine(p2, p4);
	context->drawLine(p3, p5);
	context->setStrokeColor(Color::white, ColorSpaceDeviceRGB);
	context->drawLine(p1, p2);
	context->drawLine(p1, p3);
	
	context->restore();
}
bool RenderThemeEA::paintButton(RenderObject* o, const PaintInfo& i, const IntRect& r)
{
	if(RenderStyle* pStyle = o->style())
	{
		i.context->save();
		i.context->setFillColor(pStyle->visitedDependentColor(CSSPropertyBackgroundColor), ColorSpaceDeviceRGB);   
		if(isHovered(o))
		{
			i.context->setStrokeColor(EA::WebKit::GetThemeParameters().mHoverHighlightColor, ColorSpaceDeviceRGB);
			i.context->setStrokeStyle(SolidStroke);
		}
		else
		{
			i.context->setStrokeStyle(NoStroke);
		}
		//resizing the rect to a smaller rect gives the "click" effect.
		WebCore::IntRect rect = isPressed(o) ? WebCore::IntRect(r.x() + 1, r.y() + 1, r.width() - 2, r.height() - 2) : r;
		i.context->drawRect(rect);
		paintEdgeHighligths(i.context,rect);//this gives a slightly raised effect to the button
		i.context->restore();
		return false;
	}

	return true;
}

void RenderThemeEA::adjustTextFieldStyle(StyleResolver*, RenderStyle* style, Element*) const
{
	// Note: Resetting the borders here caused the text field to appear too small on some sites so we leave the default settings.  
    style->setWhiteSpace(PRE);

    // We add some padding to the top and bottom of the text field to resolve a caret centering issue on Google.com
    // QT does something similar, probably the extra padding forces a caret to be more centered and sidesteps centering layout bugs.
    // There is a fixme note in webcore RenderTextControlSingleLine::layout() which might indicate some issues with centering text fields.
	// This does however cause misalignment of graphics with text on some sites.
    style->setPaddingTop(Length(ThemeEA::kTextFieldCursorPaddingVertical, Fixed)); 
    style->setPaddingBottom(Length(ThemeEA::kTextFieldCursorPaddingVertical, Fixed));
    // Set up the border style
    addIntrinsicMargins(style);
    const EBorderStyle v = INSET; 
    style->setBorderTopStyle(v);
    style->setBorderLeftStyle(v);
    style->setBorderBottomStyle(v);
    style->setBorderRightStyle(v);
    const uint32_t textFieldBorderWidth = EA::WebKit::GetThemeParameters().mTextFieldBorderWidth;
    style->setBorderTopWidth(textFieldBorderWidth);
    style->setBorderLeftWidth(textFieldBorderWidth);
    style->setBorderBottomWidth(textFieldBorderWidth);
    style->setBorderRightWidth(textFieldBorderWidth);
}

bool RenderThemeEA::paintTextField(RenderObject* o, const PaintInfo& i, const IntRect& r)
{
    if(RenderStyle* pStyle = o->style())
    {
        Color c = isEnabled(o) ? o->style()->visitedDependentColor(CSSPropertyBackgroundColor) : EA::WebKit::GetThemeParameters().mBoxDisabledFillColor;
        
		i.context->save();
		
		i.context->setFillColor(c, ColorSpaceDeviceRGB);   
		i.context->setStrokeColor(ThemeEA::kTextFieldBorderColor, ColorSpaceDeviceRGB);
		i.context->setStrokeStyle(SolidStroke);
		i.context->drawRect(r); 
		
		i.context->restore();
		return false;
    }

	return true;
}

void RenderThemeEA::adjustTextAreaStyle(StyleResolver* resolver, RenderStyle* style, Element* element) const
{
    adjustTextFieldStyle(resolver, style, element);
}

bool RenderThemeEA::paintTextArea(RenderObject* o, const PaintInfo& i, const IntRect& r)
{
    return paintTextField(o, i, r);
}

void RenderThemeEA::adjustMenuListStyle(StyleResolver*, RenderStyle* style, Element*) const
{
	// Note: If the height is locked to auto, the Y size can be smaller than other browsers so we don't change the setting here.
    style->resetBorder();
    style->resetPadding();
    // White-space is locked to pre
    style->setWhiteSpace(PRE);

	style->setPaddingLeft(Length(ThemeEA::kPopupMenuBorderPadding, Fixed));
    
    // Reserve enough room for a scroll bar
	int rightAdjust = ThemeEA::kScrollThickness;
    if(rightAdjust < ThemeEA::kPopupMenuArrowSize)
        rightAdjust = ThemeEA::kPopupMenuArrowSize;
    
	style->setPaddingRight(Length(ThemeEA::kPopupMenuBorderPadding + rightAdjust, Fixed));  // Add space for the dropdown arrow graphic.

    const EBorderStyle v = INSET; 
    style->setBorderTopStyle(v);
    style->setBorderLeftStyle(v);
    style->setBorderBottomStyle(v);
    style->setBorderRightStyle(v);
    const uint32_t menuListBorderWidth = EA::WebKit::GetThemeParameters().mMenuListBorderWidth;
	style->setBorderTopWidth(menuListBorderWidth);
    style->setBorderLeftWidth(menuListBorderWidth);
    style->setBorderBottomWidth(menuListBorderWidth);
    style->setBorderRightWidth(menuListBorderWidth);
}

bool RenderThemeEA::paintMenuList(RenderObject* o, const PaintInfo& i, const IntRect& r)
{
    if(RenderStyle* pStyle = o->style())
    {
        i.context->save();
        i.context->setStrokeStyle(SolidStroke);
        i.context->setStrokeColor(Color::black, ColorSpaceDeviceRGB);
		i.context->setFillColor(pStyle->visitedDependentColor(CSSPropertyBackgroundColor), ColorSpaceDeviceRGB);   
		i.context->drawRect(r);
		
		// Fill in arrow background color as a square.
		Color fillCol = isHovered(o) ? EA::WebKit::GetThemeParameters().mBoxDisabledFillColor : Color::lightGray;
        WebCore::IntRect rect(r.x() + r.width() - ThemeEA::kPopupMenuArrowSize, r.y() , ThemeEA::kPopupMenuArrowSize, r.height());
		i.context->setFillColor(fillCol, ColorSpaceDeviceRGB);   
		i.context->drawRect(rect);
		paintEdgeHighligths(i.context,rect);
		
        // Draw arrow
        float arrowW  = ((float)ThemeEA::kPopupMenuArrowSize)/4.0f;
        float arrowX   = (float) (r.x() + r.width()  - (ThemeEA::kPopupMenuArrowSize/2));
        float arrowY   = (float) (r.y() + ( (r.height() - arrowW)/2 ));
       
        FloatPoint pts[3];
        pts[0].set(arrowX - arrowW, arrowY);
        pts[1].set(arrowX + arrowW, arrowY);
        pts[2].set(arrowX, arrowY + arrowW);

        i.context->setFillColor(Color::black, ColorSpaceDeviceRGB);
        i.context->drawConvexPolygon(3, pts, true);
        i.context->restore();
        return false;  
    }
    
	return true;
}

void RenderThemeEA::adjustMenuListButtonStyle(StyleResolver* styleSelect, RenderStyle* style, Element*element) const
{
    adjustMenuListStyle(styleSelect, style,element);
}

bool RenderThemeEA::paintMenuListButton(RenderObject* o, const PaintInfo& i, const IntRect& r)
{
    paintMenuList(o,i,r);
    return false;
}

#if ENABLE(PROGRESS_ELEMENT)
double RenderThemeEA::animationRepeatIntervalForProgressBar(RenderProgress* renderProgress) const
{
    if (renderProgress->position() >= 0)
        return 0;

    // FIXME: Use hard-coded value until http://bugreports.qt.nokia.com/browse/QTBUG-9171 is fixed.
    // Use the value from windows style which is 10 fps.
    return 0.1;
}

double RenderThemeEA::animationDurationForProgressBar(RenderProgress* renderProgress) const
{
    return 0;
}

IntRect RenderThemeEA::progressBarRectForBounds(const RenderObject*, const IntRect& bounds) const
{
	return bounds;
}

void RenderThemeEA::adjustProgressBarStyle(StyleResolver*, RenderStyle* style, Element*) const
{
    style->setBoxShadow(adoptPtr<ShadowData>(0));
}

bool RenderThemeEA::paintProgressBar(RenderObject* o, const PaintInfo& pi, const IntRect& r)
{
    if (!o->isProgress())
        return true;

    notImplemented();

    return false;
}
#endif

bool RenderThemeEA::paintSliderTrack(RenderObject* o, const PaintInfo& pi,
                                     const IntRect& r)
{
    // Fill the background
    Color fillColor = o->style()->visitedDependentColor(CSSPropertyBackgroundColor);
    FillRectWithoutBorder(pi,r,fillColor);

    const int offsetH = (r.height() >> 1) - 2; // -2 for thickness adjustment. 
    IntPoint left( r.x(), r.y() + offsetH);  
    IntPoint right( r.x() + r.width(), r.y() + offsetH );

    // Top line 
    pi.context->save();
    pi.context->setStrokeStyle(SolidStroke);
    pi.context->setStrokeThickness(1.0f);
    pi.context->setStrokeColor(0xffa8a8a8, ColorSpaceDeviceRGB);
    pi.context->drawLine(left, right);  

    // Left edge 
    const IntPoint bottom(left.x(), left.y() + 4);    // +4 = 1  thickness + 3 thickness
    pi.context->drawLine(left, bottom); 

    // Highlight line.
    left.move(1,2); // Offset by 1 in X so not to erase the vertical edge (left-bottom).
    right.move(0,2);
    pi.context->setStrokeThickness(3.0f);
	pi.context->setStrokeColor(ThemeEA::kSliderTrackHighlight, ColorSpaceDeviceRGB);
    pi.context->drawLine(left, right); // Bottom line
    pi.context->restore();
      
    return false;
}

void RenderThemeEA::adjustSliderTrackStyle(StyleResolver*, RenderStyle* style, Element*) const
{
    style->setBoxShadow(adoptPtr<ShadowData>(0));
}

bool RenderThemeEA::paintSliderThumb(RenderObject* o, const PaintInfo& pi,
                                     const IntRect& r)
{
    pi.context->save();
    pi.context->setStrokeThickness(1.0f);
    pi.context->setStrokeStyle(SolidStroke);
    pi.context->setStrokeColor(ThemeEA::kSliderThumbBorderColor, ColorSpaceDeviceRGB);
    pi.context->setFillColor(ThemeEA::kSliderThumbFillColor, ColorSpaceDeviceRGB);
    pi.context->drawEllipse(r);

    // Draw the highlight
    const int kHightlightOffsetX = 1;
    const int kHightlightOffsetY = 2;
    const IntPoint center(r.center());
    const IntPoint top( center.x() + kHightlightOffsetX, center.y() - kHightlightOffsetY); // offset from center +1 and  
    const IntPoint bottom( center.x() + kHightlightOffsetX, center.y() + kHightlightOffsetY);  
    pi.context->setStrokeColor(Color::white, ColorSpaceDeviceRGB);
    pi.context->drawLine(top, bottom); 
    pi.context->restore();

    return false;
}

void RenderThemeEA::adjustSliderThumbStyle(StyleResolver*, RenderStyle* style, Element*) const
{
    //style->setBoxShadow(adoptPtr<ShadowData>(0));
    ControlPart part = style->appearance();
    if (part != MediaSliderThumbPart)   // Other options are MediaVolumeSliderThumbPart, SliderThumbVerticalPart and SliderThumbHorizontalPart. 
    {
        style->setWidth(Length((ThemeEA::kSliderThumbWidth), Fixed));
        style->setHeight(Length(ThemeEA::kSliderThumbHeight, Fixed));
    }
}

#if ENABLE(DATALIST_ELEMENT)
 
WebCore::LayoutUnit RenderThemeEA::sliderTickSnappingThreshold() const
{
    //EAWebKitTODO: implementation
    notImplemented();

    return LayoutUnit();
}

WebCore::IntSize RenderThemeEA::sliderTickSize() const
{
    //EAWebKitTODO: implementation
    notImplemented();

    return IntSize(0, 0);
}

int RenderThemeEA::sliderTickOffsetFromTrackCenter() const
{
    //EAWebKitTODO: implementation
    notImplemented();

    return 0;
}

#endif

// The search field related code is ported from RenderThemeWin.cpp. We do not use a drop down for the search results though.
bool RenderThemeEA::paintSearchField(RenderObject* o, const PaintInfo& pi, const IntRect& r)
{
    return paintTextField(o, pi, r);
}

void RenderThemeEA::adjustSearchFieldStyle(StyleResolver* resolver, RenderStyle* style,
                                           Element* e) const
{
	// Override paddingSize to match AppKit text positioning.
	const int padding = 1;
	style->setPaddingLeft(Length(padding, Fixed));
	style->setPaddingRight(Length(padding, Fixed));
	style->setPaddingTop(Length(padding, Fixed));
	style->setPaddingBottom(Length(padding, Fixed));
	if (e && e->focused() && e->document().frame()->selection().isFocusedAndActive())
		style->setOutlineOffset(-2);
}

bool RenderThemeEA::paintSearchFieldCancelButton(RenderObject* o, const PaintInfo& pi,
                                                 const IntRect& r)
{
	IntRect bounds = r;
	ASSERT(o->parent());
	if (!o->parent() || !o->parent()->isBox())
		return false;

	RenderBox* parentRenderBox = toRenderBox(o->parent());

	IntRect parentBox = parentRenderBox->absoluteContentBox();

	// Make sure the scaled button stays square and will fit in its parent's box
	bounds.setHeight(std::min(parentBox.width(), std::min(parentBox.height(), bounds.height())));
	bounds.setWidth(bounds.height());

	// Center the button vertically.  Round up though, so if it has to be one pixel off-center, it will
	// be one pixel closer to the bottom of the field.  This tends to look better with the text.
	bounds.setY(parentBox.y() + (parentBox.height() - bounds.height() + 1) / 2);

	static Image* cancelImage = Image::loadPlatformResource("searchCancel").leakRef();
	static Image* cancelPressedImage = Image::loadPlatformResource("searchCancelPressed").leakRef();
	pi.context->drawImage(isPressed(o) ? cancelPressedImage : cancelImage, o->style()->colorSpace(), bounds);
	return false;
}

void RenderThemeEA::adjustSearchFieldCancelButtonStyle(StyleResolver* resolver, RenderStyle* style,
                                                       Element* e) const
{
	// Scale the button size based on the font size
	float fontScale = style->fontSize() / EA::WebKit::GetParameters().mSystemFontSize;
	int cancelButtonSize = lroundf(std::min(std::max(ThemeEA::kMinCancelButtonSize, ThemeEA::kDefaultCancelButtonSize * fontScale), ThemeEA::kMaxCancelButtonSize));
	style->setWidth(Length(cancelButtonSize, Fixed));
	style->setHeight(Length(cancelButtonSize, Fixed));
}

void RenderThemeEA::adjustSearchFieldDecorationStyle(StyleResolver* resolver, RenderStyle* style,
                                                     Element* e) const
{
	IntSize emptySize(1, 11);
	style->setWidth(Length(emptySize.width(), Fixed));
	style->setHeight(Length(emptySize.height(), Fixed));
}

void RenderThemeEA::adjustSearchFieldResultsDecorationStyle(StyleResolver* resolver, RenderStyle* style,
															Element* e) const
{
	// Scale the decoration size based on the font size
	float fontScale = style->fontSize() / EA::WebKit::GetParameters().mSystemFontSize;;
	int magnifierSize = lroundf(std::min(std::max(ThemeEA::kMinSearchFieldResultsDecorationSize, ThemeEA::kDefaultSearchFieldResultsDecorationSize * fontScale), 
		ThemeEA::kMaxSearchFieldResultsDecorationSize));
	style->setWidth(Length(magnifierSize, Fixed));
	style->setHeight(Length(magnifierSize, Fixed));
}

bool RenderThemeEA::paintSearchFieldResultsDecoration(RenderObject* o, const PaintInfo& pi,
													  const IntRect& r)
{
	IntRect bounds = r;
	ASSERT(o->parent());
	if (!o->parent() || !o->parent()->isBox())
		return false;

	RenderBox* parentRenderBox = toRenderBox(o->parent());
	IntRect parentBox = parentRenderBox->absoluteContentBox();

	// Make sure the scaled decoration stays square and will fit in its parent's box
	bounds.setHeight(std::min(parentBox.width(), std::min(parentBox.height(), bounds.height())));
	bounds.setWidth(bounds.height());

	// Center the decoration vertically.  Round up though, so if it has to be one pixel off-center, it will
	// be one pixel closer to the bottom of the field.  This tends to look better with the text.
	bounds.setY(parentBox.y() + (parentBox.height() - bounds.height() + 1) / 2);

	static Image* magnifierImage = Image::loadPlatformResource("searchMagnifier").leakRef();
	pi.context->drawImage(magnifierImage, o->style()->colorSpace(), bounds);
	return false;
}

void RenderThemeEA::adjustSearchFieldResultsButtonStyle(StyleResolver*, RenderStyle* style, Element*) const
{
	// Scale the button size based on the font size
	float fontScale = style->fontSize() / EA::WebKit::GetParameters().mSystemFontSize;;
	int magnifierHeight = lroundf(std::min(std::max(ThemeEA::kMinSearchFieldResultsDecorationSize, ThemeEA::kDefaultSearchFieldResultsDecorationSize * fontScale), 
		ThemeEA::kMaxSearchFieldResultsDecorationSize));
	int magnifierWidth = lroundf(magnifierHeight * ThemeEA::kDefaultSearchFieldResultsButtonWidth / ThemeEA::kDefaultSearchFieldResultsDecorationSize);
	style->setWidth(Length(magnifierWidth, Fixed));
	style->setHeight(Length(magnifierHeight, Fixed));
}

bool RenderThemeEA::paintSearchFieldResultsButton(RenderObject* o, const PaintInfo& paintInfo, const IntRect& r)
{
	IntRect bounds = r;
	ASSERT(o->parent());
	if (!o->parent())
		return false;
	if (!o->parent() || !o->parent()->isBox())
		return false;

	RenderBox* parentRenderBox = toRenderBox(o->parent());
	IntRect parentBox = parentRenderBox->absoluteContentBox();

	// Make sure the scaled decoration will fit in its parent's box
	bounds.setHeight(std::min(parentBox.height(), bounds.height()));
	bounds.setWidth(std::min<int>(parentBox.width(), bounds.height() * ThemeEA::kDefaultSearchFieldResultsButtonWidth / ThemeEA::kDefaultSearchFieldResultsDecorationSize));

	// Center the button vertically.  Round up though, so if it has to be one pixel off-center, it will
	// be one pixel closer to the bottom of the field.  This tends to look better with the text.
	bounds.setY(parentBox.y() + (parentBox.height() - bounds.height() + 1) / 2);

	static Image* magnifierImage = Image::loadPlatformResource("searchMagnifier"/*"searchMagnifierResults"*/).leakRef();
	paintInfo.context->drawImage(magnifierImage, o->style()->colorSpace(), bounds);
	return false;
}

bool RenderThemeEA::paintSearchFieldDecoration(RenderObject* o, const PaintInfo& pi,
                                               const IntRect& r)
{
	return false;
}






void RenderThemeEA::adjustSliderThumbSize(RenderStyle* style, Element*) const
{

}

bool RenderThemeEA::paintMediaFullscreenButton(RenderObject*, const PaintInfo& i, const IntRect& r)
{ 
    FillRectWithSolidBorder(i, r);
   
    const int offsetW = (r.width() >> 2) + 2;       // +2 to make if more rectangle or it looks like a stop button square.
    const int offsetH = (r.height() >> 2);   
    const IntPoint center(r.center());
    
    IntRect box((center.x() - offsetW), (r.y() + offsetH), (offsetW << 1), (offsetH << 1));
    Color c(Color::white);
    FillRectWithSolidBorder(i, box, c);
    return false;       // We return false if painted 
}
bool RenderThemeEA::paintMediaPlayButton(RenderObject* o, const PaintInfo& i, const IntRect& r) 
{ 
    // This displays a pause when playing and a play when paused.

    FillRectWithSolidBorder(i, r);

    const int offsetW = (r.width() >> 2);
    const int offsetH = (r.height() >> 2);   
    const IntPoint center(r.center());
    
    bool paused = false;
    HTMLMediaElement* pMediaElement = parentMediaElement(*o);
    EAW_ASSERT(pMediaElement);
    
    if (pMediaElement)
    {
        MediaPlayer* pPlayer = pMediaElement->player();
        if (pPlayer)
            paused = pPlayer->paused();    
    }
    
    i.context->save();   
    i.context->setFillColor(Color::black, ColorSpaceDeviceRGB);
    i.context->setStrokeStyle(NoStroke);
    if (!paused)
    {
        // build 2 a small rects
        int stepW = r.width() / 5;  
        IntRect box( r.x() + stepW, (center.y() - (offsetH)), stepW, offsetH << 1);
        i.context->drawRect(box);
        box.setX(box.x() + stepW + stepW);
        i.context->drawRect(box);    
    }
    else
    {
        // Build an arrow. 
        FloatPoint pts[3];
        pts[0].set((int) (center.x() - offsetW), (int) (center.y() - offsetH));
        pts[2].set((int) (center.x() + offsetW), (int) center.y());
        pts[1].set((int) (center.x() - offsetW), (int) (center.y() + offsetH));
        i.context->drawConvexPolygon(3, pts, true);
    }
    i.context->restore();
   
    return false;
}

bool RenderThemeEA::paintMediaMuteButton(RenderObject* obj, const PaintInfo& i, const IntRect& r)
{ 
    
    const int kMutedColor = 0xffaa0000;     // Red
    const int kUnMutedColor = 0xff000000;
    
    FillRectWithSolidBorder(i, r);

    const float offsetW = (float) (r.width() >> 2);     
    const float offsetH = (float) (r.height() / 3);     
    const IntPoint center(r.center());
   
    FloatPoint pts[3];
    pts[0].set(center.x() + offsetW, center.y() - offsetH);
    pts[2].set(center.x() - offsetW, center.y());
    pts[1].set(center.x() + offsetW, center.y() + offsetH);

    Color c;
    HTMLMediaElement* pMediaElement = parentMediaElement(*obj);
    EAW_ASSERT(pMediaElement);
    if (pMediaElement->muted())
    {
        c = kMutedColor;
    }
    else
    {
        c = kUnMutedColor;
    }
    i.context->save();
   
    i.context->setFillColor(c, ColorSpaceDeviceRGB);
    i.context->drawConvexPolygon(3, pts, true);

    // Build a small box
    IntRect box( (int) (center.x() - offsetW), (int) (center.y() - offsetH / 2.0f), (int) (offsetW), (int) (offsetH));
    i.context->setStrokeStyle(NoStroke);
    i.context->drawRect(box);
    i.context->restore();

    // (We could also add some vertical lines as sound waves when the speaker is active if needed).

    return false; 
}
bool RenderThemeEA::paintMediaSeekBackButton(RenderObject* o , const PaintInfo& i , const IntRect& r) 
{ 

    FillRectWithSolidBorder(i, r);

    const int offsetW = r.width() >> 2;     // Have the arrow width 1/2 the screen size so 1/4 on each size.
    const int offsetH = r.height() >> 2;     
    const IntPoint center(r.center());

    IntPoint p1(center.x() + offsetW, center.y() - offsetH);
    IntPoint p2(center.x() - offsetW, center.y());
    IntPoint p3(center.x() + offsetW, center.y() + offsetH);

    i.context->save();   
    i.context->setStrokeColor(Color::black, ColorSpaceDeviceRGB);
    i.context->setStrokeThickness(1.0f);
    i.context->drawLine(p1,p2);
    i.context->drawLine(p2,p3);
    i.context->restore();   

    return false;    
}
bool RenderThemeEA::paintMediaSeekForwardButton(RenderObject* o, const PaintInfo& i , const IntRect& r) 
{ 
    FillRectWithSolidBorder(i, r);
    
    const int offsetW = r.width() >> 2;     // Have the arrow width 1/2 the screen size so 1/4 on each size.
    const int offsetH = r.height() >> 2;     // Have the arrow width 1/2 the screen size so 1/4 on each size.
    const IntPoint center(r.center());

    IntPoint p1(center.x() - offsetW, center.y()- offsetH);
    IntPoint p2(center.x() + offsetW, center.y());
    IntPoint p3(center.x() - offsetW, center.y() + offsetH);

    i.context->save();   
    i.context->setStrokeColor(Color::black, ColorSpaceDeviceRGB);
    i.context->setStrokeThickness(1.0f);  
    i.context->drawLine(p1,p2);
    i.context->drawLine(p2,p3);
    i.context->restore();       
   
    return false;    
}
bool RenderThemeEA::paintMediaSliderTrack(RenderObject* o, const PaintInfo& i, const IntRect& r)
{ 
    const int kEdgeOffsetX = 4;
    const int kPlayedColor = 0xff444444;
    const int kRemainColor = 0xffeeeeee;

    HTMLMediaElement* pMediaElement = parentMediaElement(*o);
    EAW_ASSERT(pMediaElement);
    float ratio = 0.0f;
    if (pMediaElement)
    {
        MediaPlayer* pPlayer = pMediaElement->player();
        if(pPlayer)
		{
			const float min = pPlayer->currentTime(); 
			const float max = pPlayer->duration();    

			if (max > 0.f)  
			{
				ratio =  min / max;
				if(ratio > 1.0f)
					ratio = 1.0f;    
			}
		}
    }

    FillRectWithSolidBorder(i, r);
    
    // Draw the played time.
    int w = r.width() - (kEdgeOffsetX + kEdgeOffsetX);    // edge on each side.
    int wPlayed = (int) (w * ratio);
    IntRect rPlayed( r.x() + kEdgeOffsetX, r.y() + (r.height() >> 2), wPlayed, r.height() >> 1);
    if (wPlayed)
    {
        Color   cPlayed(kPlayedColor); 
        FillRectWithSolidBorder(i, rPlayed, cPlayed);
    }
   
    // Draw remain time.
    int wRemain = w - wPlayed;
    if (wRemain)
    {
        IntRect rRemain( rPlayed.x() + wPlayed, rPlayed.y(), wRemain, rPlayed.height());
        Color   cRemain(kRemainColor); 
        FillRectWithSolidBorder(i, rRemain, cRemain);
    }
    return false;    
}
bool RenderThemeEA::paintMediaSliderThumb(RenderObject*, const PaintInfo& i, const IntRect& r) 
{ 
    // We don't want to draw a media slider currently.
    return false;    
}
bool RenderThemeEA::paintMediaVolumeSliderContainer(RenderObject*, const PaintInfo& i, const IntRect& r) 
{ 
    FillRectWithSolidBorder(i, r);
    return false;    
}
bool RenderThemeEA::paintMediaVolumeSliderTrack(RenderObject*, const PaintInfo& i, const IntRect& r)
{ 
    FillRectWithSolidBorder(i, r);
    return false;    
}
bool RenderThemeEA::paintMediaVolumeSliderThumb(RenderObject*, const PaintInfo& i, const IntRect& r)
{ 
    FillRectWithSolidBorder(i, r);
    return false;    
}
bool RenderThemeEA::paintMediaRewindButton(RenderObject*, const PaintInfo& i, const IntRect& r) 
{ 
    FillRectWithSolidBorder(i, r);
    return false;    
}
bool RenderThemeEA::paintMediaReturnToRealtimeButton(RenderObject*, const PaintInfo& i, const IntRect& r)
{ 
    FillRectWithSolidBorder(i, r);
    return false;    
}
bool RenderThemeEA::paintMediaToggleClosedCaptionsButton(RenderObject*, const PaintInfo& i, const IntRect& r)
{ 
    FillRectWithSolidBorder(i, r);
    return false;    
}
bool RenderThemeEA::paintMediaControlsBackground(RenderObject* o, const PaintInfo& i, const IntRect& r)
{ 
    FillRectWithSolidBorder(i, r);
    return false;    
}
bool RenderThemeEA::paintMediaCurrentTime(RenderObject*, const PaintInfo& i , const IntRect& r) 
{ 
    FillRectWithSolidBorder(i, r);

    return false;    
}
bool RenderThemeEA::paintMediaTimeRemaining(RenderObject*, const PaintInfo& i, const IntRect& r)
{ 
   FillRectWithSolidBorder(i, r);
   return false;    
}

double RenderThemeEA::caretBlinkInterval() const
{
    return ThemeEA::kBlinkFrequency;
}

#if ENABLE(VIDEO)
// To modify the slider location. 
IntPoint RenderThemeEA::volumeSliderOffsetFromMuteButton(RenderBox* muteButtonBox, const IntSize& size) const
{
    int y = -size.height();
    FloatPoint absPoint = muteButtonBox->localToAbsolute(FloatPoint(muteButtonBox->offsetLeft(), y), IsFixed | UseTransforms);
    if (absPoint.y() < 0)
        y = muteButtonBox->height();
    return IntPoint(0, y);
}

String RenderThemeEA::extraMediaControlsStyleSheet()
{
    String result = String(mediaControlsEAUserAgentStyleSheet, sizeof(mediaControlsEAUserAgentStyleSheet));
    return result;
}
#endif


} // WebCore namespace

