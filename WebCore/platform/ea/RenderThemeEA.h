/*
 * This file is part of the theme implementation for form controls in WebCore.
 *
 * Copyright (C) 2009 Nokia Corporation and/or its subsidiary(-ies).
 * Copyright (C) 2011, 2012, 2013, 2014, 2015 Electronic Arts, Inc. All rights reserved.
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
#ifndef RenderThemeEA_h
#define RenderThemeEA_h

#include "RenderTheme.h"

namespace WebCore {

class RenderProgress;
class RenderStyle;
class HTMLMediaElement;
class GraphicsContext;
class IntRect;

class RenderThemeEA : public RenderTheme {
private:
    RenderThemeEA(Page* page);
    virtual ~RenderThemeEA();

public:
    
    static Ref<RenderTheme> create(Page*);
	static void paintEdgeHighlights(GraphicsContext* context,const IntRect& r);
#if ENABLE(VIDEO)
	virtual String mediaControlsStyleSheet() override;
	virtual	String mediaControlsScript() override;
	virtual String extraMediaControlsStyleSheet() override;
#endif

	virtual int baselinePosition(const RenderBox&) const override;
	
	virtual bool controlSupportsTints(const RenderObject&) const override;
	
	virtual bool supportsControlTints() const override;
	

	virtual bool supportsFocusRing(const RenderStyle& style) const override;
	virtual bool supportsHover(const RenderStyle&) const override;

	virtual Color platformActiveTextSearchHighlightColor() const override;
	virtual Color platformInactiveTextSearchHighlightColor() const override;

	virtual double caretBlinkInterval() const override;

    void systemFont(CSSValueID propId, FontDescription&) const;

    virtual int minimumMenuListSize(RenderStyle&) const override;

    virtual void adjustSliderThumbSize(RenderStyle&, Element*) const override;

	virtual double animationRepeatIntervalForProgressBar(RenderProgress& renderProgress) const override;
	virtual double animationDurationForProgressBar(RenderProgress& renderProgress) const override;
	virtual IntRect progressBarRectForBounds(const RenderObject&, const IntRect&) const override;

#if ENABLE(VIDEO)
	virtual bool usesMediaControlVolumeSlider() const override { return false; }
	virtual LayoutPoint volumeSliderOffsetFromMuteButton(const RenderBox&, const LayoutSize&) const override;
#endif

#if ENABLE(DATALIST_ELEMENT)
	virtual LayoutUnit sliderTickSnappingThreshold() const override;
	virtual IntSize sliderTickSize() const override;
	virtual int sliderTickOffsetFromTrackCenter() const override;
#endif

protected:
	virtual Color platformActiveSelectionBackgroundColor() const override;
	virtual Color platformInactiveSelectionBackgroundColor() const override;
	virtual Color platformActiveSelectionForegroundColor() const override;
	virtual Color platformInactiveSelectionForegroundColor() const override;
	
	virtual Color platformActiveListBoxSelectionBackgroundColor() const override;
	virtual Color platformInactiveListBoxSelectionBackgroundColor() const override;
	virtual Color platformActiveListBoxSelectionForegroundColor() const override ;
	virtual Color platformInactiveListBoxSelectionForegroundColor() const override;
	
	virtual bool paintCheckbox(const RenderObject& o, const PaintInfo& i, const IntRect& r) override;
    virtual void setCheckboxSize(RenderStyle&) const override;

    virtual bool paintRadio(const RenderObject& o, const PaintInfo& i, const IntRect& r) override;
    virtual void setRadioSize(RenderStyle&) const override;

    virtual void adjustButtonStyle(StyleResolver&, RenderStyle&, Element*) const override;
    virtual bool paintButton(const RenderObject&, const PaintInfo&, const IntRect&) override;

	virtual void adjustTextFieldStyle(StyleResolver&, RenderStyle&, Element*) const override;
	virtual bool paintTextField(const RenderObject&, const PaintInfo&, const FloatRect&) override;

	virtual void adjustTextAreaStyle(StyleResolver&, RenderStyle&, Element*) const override;
	virtual bool paintTextArea(const RenderObject&, const PaintInfo&, const FloatRect&) override;

	virtual void adjustMenuListStyle(StyleResolver&, RenderStyle&, Element*) const override;
	virtual bool paintMenuList(const RenderObject& o, const PaintInfo& i, const FloatRect& r) override;

	virtual void adjustMenuListButtonStyle(StyleResolver&, RenderStyle&, Element*) const override;
	virtual bool paintMenuListButtonDecorations(const RenderBox&, const PaintInfo&, const FloatRect&) override;

    virtual void adjustProgressBarStyle(StyleResolver&, RenderStyle&, Element*) const override;
    virtual bool paintProgressBar(const RenderObject&, const PaintInfo&, const IntRect&) override;

	virtual void adjustSliderTrackStyle(StyleResolver&, RenderStyle&, Element*) const override;
	virtual bool paintSliderTrack(const RenderObject&, const PaintInfo&, const IntRect&) override;

	virtual void adjustSliderThumbStyle(StyleResolver&, RenderStyle&, Element*) const override;
	virtual bool paintSliderThumb(const RenderObject&, const PaintInfo&, const IntRect&) override;

	virtual void adjustSearchFieldStyle(StyleResolver&, RenderStyle&, Element*) const override;
	virtual bool paintSearchField(const RenderObject&, const PaintInfo&, const IntRect&) override;
	virtual void adjustSearchFieldDecorationPartStyle(StyleResolver&, RenderStyle&, Element*) const override;

    virtual void adjustSearchFieldCancelButtonStyle(StyleResolver&, RenderStyle&, Element*) const override;
	virtual bool paintSearchFieldCancelButton(const RenderObject&, const PaintInfo&, const IntRect&) override;

	virtual bool paintSearchFieldDecorations(const RenderObject&, const PaintInfo&, const IntRect&) override;

	virtual void adjustSearchFieldResultsDecorationPartStyle(StyleResolver&, RenderStyle&, Element*) const override;
	virtual bool paintSearchFieldResultsDecorationPart(const RenderObject&, const PaintInfo&, const IntRect&) override;

	virtual void adjustSearchFieldResultsButtonStyle(StyleResolver&, RenderStyle&, Element*) const override;
	virtual bool paintSearchFieldResultsButton(const RenderObject&, const PaintInfo&, const IntRect&) override;

    virtual bool paintMediaFullscreenButton(const RenderObject&, const PaintInfo&, const IntRect&) override;
    virtual bool paintMediaPlayButton(const RenderObject&, const PaintInfo&, const IntRect&) override;
    virtual bool paintMediaMuteButton(const RenderObject&, const PaintInfo&, const IntRect&) override;
    virtual bool paintMediaSeekBackButton(const RenderObject&, const PaintInfo&, const IntRect&) override;
    virtual bool paintMediaSeekForwardButton(const RenderObject&, const PaintInfo&, const IntRect&) override;
    virtual bool paintMediaSliderTrack(const RenderObject&, const PaintInfo&, const IntRect&) override;
	virtual bool paintMediaSliderThumb(const RenderObject&, const PaintInfo&, const IntRect&) override;
	virtual bool paintMediaVolumeSliderContainer(const RenderObject&, const PaintInfo&, const IntRect&) override;
	virtual bool paintMediaVolumeSliderTrack(const RenderObject&, const PaintInfo&, const IntRect&) override;
	virtual bool paintMediaVolumeSliderThumb(const RenderObject&, const PaintInfo&, const IntRect&) override;
	virtual bool paintMediaRewindButton(const RenderObject&, const PaintInfo&, const IntRect&) override;
	virtual bool paintMediaReturnToRealtimeButton(const RenderObject&, const PaintInfo&, const IntRect&) override;
	virtual bool paintMediaToggleClosedCaptionsButton(const RenderObject&, const PaintInfo&, const IntRect&) override;
	virtual bool paintMediaControlsBackground(const RenderObject&, const PaintInfo&, const IntRect&) override;
	virtual bool paintMediaCurrentTime(const RenderObject&, const PaintInfo&, const IntRect&) override;
	virtual bool paintMediaTimeRemaining(const RenderObject&, const PaintInfo&, const IntRect&) override;

	virtual void updateCachedSystemFontDescription(CSSValueID systemFontID, FontDescription&) const override;

private:
	void addIntrinsicMargins(RenderStyle& style) const;
	AtomicString m_buttonFontFamily;
	String m_mediaControlsScript;
	String m_mediaControlsStyleSheet;
};


}

#endif // RenderThemeEA_h
