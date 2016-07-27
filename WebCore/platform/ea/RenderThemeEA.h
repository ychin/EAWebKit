/*
 * This file is part of the theme implementation for form controls in WebCore.
 *
 * Copyright (C) 2009 Nokia Corporation and/or its subsidiary(-ies).
 * Copyright (C) 2011, 2012, 2013, 2014 Electronic Arts, Inc. All rights reserved.
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

#if ENABLE(PROGRESS_ELEMENT)
class RenderProgress;
#endif
class RenderStyle;
class HTMLMediaElement;
class GraphicsContext;
class IntRect;

class RenderThemeEA : public RenderTheme {
private:
    RenderThemeEA(Page* page);
    virtual ~RenderThemeEA();

public:
    
    static PassRefPtr<RenderTheme> create(Page*);
	static void paintEdgeHighligths(GraphicsContext* context,const IntRect& r);
#if ENABLE(VIDEO)
	virtual String extraMediaControlsStyleSheet() OVERRIDE;
#endif

	virtual int baselinePosition(const RenderObject* o) const OVERRIDE;
	
	virtual bool controlSupportsTints(const RenderObject*) const OVERRIDE;
	
	virtual bool supportsControlTints() const OVERRIDE;
	

	virtual bool supportsFocusRing(const RenderStyle* style) const OVERRIDE;
	virtual bool supportsHover(const RenderStyle*) const OVERRIDE;

	virtual Color platformActiveTextSearchHighlightColor() const OVERRIDE;
	virtual Color platformInactiveTextSearchHighlightColor() const OVERRIDE;

	virtual double caretBlinkInterval() const OVERRIDE;

    virtual void systemFont(CSSValueID propId, FontDescription&) const OVERRIDE;

    virtual int minimumMenuListSize(RenderStyle*) const OVERRIDE;

    virtual void adjustSliderThumbSize(RenderStyle*, Element*) const OVERRIDE;

#if ENABLE(PROGRESS_ELEMENT)
	virtual double animationRepeatIntervalForProgressBar(RenderProgress* renderProgress) const OVERRIDE;
	virtual double animationDurationForProgressBar(RenderProgress* renderProgress) const OVERRIDE;
	virtual IntRect progressBarRectForBounds(const RenderObject*, const IntRect&) const OVERRIDE;
#endif
#if ENABLE(VIDEO)
	virtual bool usesMediaControlVolumeSlider() const OVERRIDE { return false; }
	virtual IntPoint volumeSliderOffsetFromMuteButton(RenderBox* muteButtonBox, const IntSize& size) const OVERRIDE;
#endif

#if ENABLE(DATALIST_ELEMENT)
	virtual LayoutUnit sliderTickSnappingThreshold() const OVERRIDE;
	virtual IntSize sliderTickSize() const OVERRIDE;
	virtual int sliderTickOffsetFromTrackCenter() const OVERRIDE;
#endif

protected:
	virtual Color platformActiveSelectionBackgroundColor() const OVERRIDE;
	virtual Color platformInactiveSelectionBackgroundColor() const OVERRIDE;
	virtual Color platformActiveSelectionForegroundColor() const OVERRIDE;
	virtual Color platformInactiveSelectionForegroundColor() const OVERRIDE;
	
	virtual Color platformActiveListBoxSelectionBackgroundColor() const OVERRIDE;
	virtual Color platformInactiveListBoxSelectionBackgroundColor() const OVERRIDE;
	virtual Color platformActiveListBoxSelectionForegroundColor() const OVERRIDE ;
	virtual Color platformInactiveListBoxSelectionForegroundColor() const OVERRIDE;
	
	virtual bool paintCheckbox(RenderObject* o, const PaintInfo& i, const IntRect& r) OVERRIDE;
    virtual void setCheckboxSize(RenderStyle*) const OVERRIDE;

    virtual bool paintRadio(RenderObject* o, const PaintInfo& i, const IntRect& r) OVERRIDE;
    virtual void setRadioSize(RenderStyle*) const OVERRIDE;

    virtual void adjustButtonStyle(StyleResolver*, RenderStyle*, Element*) const OVERRIDE;
    virtual bool paintButton(RenderObject*, const PaintInfo&, const IntRect&) OVERRIDE;

	virtual void adjustTextFieldStyle(StyleResolver*, RenderStyle*, Element*) const OVERRIDE;
	virtual bool paintTextField(RenderObject*, const PaintInfo&, const IntRect&) OVERRIDE;

	virtual void adjustTextAreaStyle(StyleResolver*, RenderStyle*, Element*) const OVERRIDE;
	virtual bool paintTextArea(RenderObject*, const PaintInfo&, const IntRect&) OVERRIDE;

	virtual void adjustMenuListStyle(StyleResolver*, RenderStyle*, Element*) const OVERRIDE;
	virtual bool paintMenuList(RenderObject* o, const PaintInfo& i, const IntRect& r) OVERRIDE;

	virtual void adjustMenuListButtonStyle(StyleResolver*, RenderStyle*, Element*) const OVERRIDE;
	virtual bool paintMenuListButton(RenderObject*, const PaintInfo&, const IntRect&) OVERRIDE;

#if ENABLE(PROGRESS_ELEMENT)
    virtual void adjustProgressBarStyle(StyleResolver*, RenderStyle*, Element*) const OVERRIDE;
    virtual bool paintProgressBar(RenderObject*, const PaintInfo&, const IntRect&) OVERRIDE;
#endif

	virtual void adjustSliderTrackStyle(StyleResolver*, RenderStyle*, Element*) const OVERRIDE;
	virtual bool paintSliderTrack(RenderObject*, const PaintInfo&, const IntRect&) OVERRIDE;

	virtual void adjustSliderThumbStyle(StyleResolver*, RenderStyle*, Element*) const OVERRIDE;
	virtual bool paintSliderThumb(RenderObject*, const PaintInfo&, const IntRect&) OVERRIDE;

	virtual void adjustSearchFieldStyle(StyleResolver*, RenderStyle*, Element*) const OVERRIDE;
	virtual bool paintSearchField(RenderObject*, const PaintInfo&, const IntRect&) OVERRIDE; 

    virtual void adjustSearchFieldCancelButtonStyle(StyleResolver*, RenderStyle*, Element*) const OVERRIDE;
    virtual bool paintSearchFieldCancelButton(RenderObject*, const PaintInfo&, const IntRect&) OVERRIDE;

    virtual void adjustSearchFieldDecorationStyle(StyleResolver*, RenderStyle*, Element*) const OVERRIDE;
    virtual bool paintSearchFieldDecoration(RenderObject*, const PaintInfo&, const IntRect&) OVERRIDE;

    virtual void adjustSearchFieldResultsDecorationStyle(StyleResolver*, RenderStyle*, Element*) const OVERRIDE;
    virtual bool paintSearchFieldResultsDecoration(RenderObject*, const PaintInfo&, const IntRect&) OVERRIDE;

	virtual void adjustSearchFieldResultsButtonStyle(StyleResolver*, RenderStyle*, Element*) const OVERRIDE;
	virtual bool paintSearchFieldResultsButton(RenderObject*, const PaintInfo&, const IntRect&) OVERRIDE;

    virtual bool paintMediaFullscreenButton(RenderObject*, const PaintInfo&, const IntRect&) OVERRIDE;
    virtual bool paintMediaPlayButton(RenderObject*, const PaintInfo&, const IntRect&) OVERRIDE;
    virtual bool paintMediaMuteButton(RenderObject*, const PaintInfo&, const IntRect& OVERRIDE);
    virtual bool paintMediaSeekBackButton(RenderObject*, const PaintInfo&, const IntRect&) OVERRIDE;
    virtual bool paintMediaSeekForwardButton(RenderObject*, const PaintInfo&, const IntRect&) OVERRIDE;
    virtual bool paintMediaSliderTrack(RenderObject*, const PaintInfo&, const IntRect&) OVERRIDE;
    virtual bool paintMediaSliderThumb(RenderObject*, const PaintInfo&, const IntRect&) OVERRIDE;
    virtual bool paintMediaVolumeSliderContainer(RenderObject*, const PaintInfo&, const IntRect&) OVERRIDE;
    virtual bool paintMediaVolumeSliderTrack(RenderObject*, const PaintInfo&, const IntRect&) OVERRIDE;
    virtual bool paintMediaVolumeSliderThumb(RenderObject*, const PaintInfo&, const IntRect&) OVERRIDE;
    virtual bool paintMediaRewindButton(RenderObject*, const PaintInfo&, const IntRect&) OVERRIDE;
    virtual bool paintMediaReturnToRealtimeButton(RenderObject*, const PaintInfo&, const IntRect&) OVERRIDE;
    virtual bool paintMediaToggleClosedCaptionsButton(RenderObject*, const PaintInfo&, const IntRect&) OVERRIDE;
    virtual bool paintMediaControlsBackground(RenderObject*, const PaintInfo&, const IntRect&) OVERRIDE;
    virtual bool paintMediaCurrentTime(RenderObject*, const PaintInfo&, const IntRect&) OVERRIDE;
    virtual bool paintMediaTimeRemaining(RenderObject*, const PaintInfo&, const IntRect&) OVERRIDE;

private:
	void addIntrinsicMargins(RenderStyle* style) const;
	AtomicString m_buttonFontFamily;
};


}

#endif // RenderThemeEA_h
