/*
    Copyright (C) 2006 Apple Computer, Inc.  All rights reserved.
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
#include "FontDescription.h"
#include "SimpleFontData.h"
#include "NotImplemented.h"
#include "FontCache.h"

#include <EAWebKit/EAWebKitTextInterface.h>
#include <internal/include/EAWebKitAssert.h>
/*
Fonts scratch notes - Putting it here as I don't want to modify webcore files.

###
WebCore::Glyph is defined as unsigned short. Is it an index in a buffer?

###
GlyphBuffer
#Has a collection of SimpleFontData, Glyphs and Advances.
The SimpleFontData is a collection of 2048 pointers ! Does that mean that we could have so many fonts alive?
What is SimpleFontData representing?

#It seems like GlyphBuffer is an orderly collection of the above mentioned elements. It might be better to
simply create a struct to emphasize that relationship (you may end up taking a caching penalty though depending
upon access behavior though). It exposes accessors for manipulating this orderly buffer.

###
FontCustomPlatformData
# A buffer downloaded over the network is kept around. The custom instance can whip out as many individual FontPlatformData instances as needed (customized in terms of size, boldness etc.)

###
FontPlatformData
# This guy seems to be the main interface class that needs to be implemented by port.
# This guy serves as the heavy lifter/glue for the font resource on platform end with respect to WebCore.

###
FontDescription
# This guy encapsulates the font description defined in css/html and passes it on to the FontPlatformData/platform layer to create appropriate resource.
*/

/*
Changes to Do:
 # Change GlyphBufferAdvance from FloatSize to float. Easily saves few Ks.
 */

/* Questions

# What is the difference between bold and syntheticBold?

*/
namespace WebCore {

void SimpleFontData::platformInit()
{
    EA::WebKit::IFont* pFont = m_platformData.font();
	EAW_ASSERT(pFont);    
	if(pFont)
    {
		// visit https://developer.apple.com/library/ios/documentation/StringsTextFonts/Conceptual/TextAndWebiPhoneOS/Introduction/Introduction.html for the terminology used
		
		EA::WebKit::FontMetrics fontMetrics;
		pFont->GetFontMetrics(fontMetrics);
		
		// The usage of ceiling here is questionable but is probably safer to do.
		const float ascent = ceilf(fontMetrics.mfAscent);
		const float descent = ceilf(abs(fontMetrics.mfDescent));
		m_fontMetrics.setAscent(ascent);
		m_fontMetrics.setDescent(descent);   

		// This fix was contributed by YC Chin.  The Line spacing and gap calculations were wrong and did not include leading.
		m_fontMetrics.setLineGap(fontMetrics.mfLeading);        
		// Instead of using line height by EAText, we calculate it ourselves (in sync with other webkit ports). 
		//m_fontMetrics.setLineSpacing(fontMetrics.mfLineHeight);
		float lineHeight = ascent + descent + ceilf(fontMetrics.mfLeading);
		m_fontMetrics.setLineSpacing(lineHeight);
		m_fontMetrics.setUnitsPerEm(1000);                          // This is a typical value for a TrueType font, not necessarily the actual value.
		m_fontMetrics.setXHeight(fontMetrics.mfXHeight);            // This is the height of lower case characters.


		// Load in a space character to get the width metrics for it.
		const EA::WebKit::Char  cSpace  = ' ';
		EA::WebKit::GlyphId  glyphId = 0;
		pFont->GetGlyphIds(&cSpace, 1, &glyphId, true);

		EA::WebKit::GlyphMetrics glyphMetrics;
		pFont->GetGlyphMetrics(glyphId, glyphMetrics);
		m_spaceWidth = glyphMetrics.mfHAdvanceX;
    }
}

void SimpleFontData::platformDestroy()
{
    m_derivedFontData.clear();
}

void SimpleFontData::platformCharWidthInit()
{
    m_avgCharWidth = 0.f;       
    m_maxCharWidth = 0.f;
    initCharWidths();       // Use the default width estimator since EAText metrics does not seem to have it.
}

FloatRect SimpleFontData::platformBoundsForGlyph(Glyph) const
{
    // Seems like we could get this info but some other platforms seem to just return a empty rect too. 
    return FloatRect();
}

WTF::PassRefPtr<SimpleFontData> SimpleFontData::smallCapsFontData(const FontDescription& fontDescription) const
{
    if (!m_derivedFontData)
    {
        m_derivedFontData = DerivedFontData::create(isCustomFont());
    }

    if (!m_derivedFontData->smallCaps)
    {
        // Note by Paul Pedriana:
        // We simply create a version of this font that is 70% of its current size.
        // Question: Is there a means to get a more proper small caps variant?
        // Actually I think we just need to modify the code below to get a true small
        // caps font, but shouldn't desc.m_smallCaps be set to true in such a case?
        // and if m_smallCaps is true then we don't need 70% size.

        FontDescription desc = FontDescription(fontDescription);
        desc.setSpecifiedSize(0.70f * fontDescription.computedSize());

        const AtomicString& familyName = desc.firstFamily();     

        m_derivedFontData->smallCaps = fontCache()->getCachedFontData(desc, familyName);
    }

    return m_derivedFontData->smallCaps;
}

bool SimpleFontData::containsCharacters(const UChar* characters, int length) const
{
    int i = 0;
    EA::WebKit::IFont* const pFont =  m_platformData.font();
    EAW_ASSERT(pFont);

    if(pFont)
    {
        while(pFont->IsCharSupported(characters[i]))
            ++i;
    }
    return (i == length);
}

void SimpleFontData::determinePitch()
{
     m_treatAsFixedPitch = m_platformData.isFixedPitch();
}

float SimpleFontData::platformWidthForGlyph(Glyph glyphId) const
{
    EA::WebKit::IFont* const pFont = getEAFont();
    EAW_ASSERT(pFont);

    EA::WebKit::GlyphMetrics glyphMetrics;
    glyphMetrics.mfHAdvanceX = m_spaceWidth; // Default value.
    if(pFont)
		pFont->GetGlyphMetrics(glyphId, glyphMetrics);

    return glyphMetrics.mfHAdvanceX;
}

WTF::PassRefPtr<SimpleFontData> SimpleFontData::emphasisMarkFontData(const FontDescription&) const
{
    // EAWebkitTODO
    return NULL;
}

//EAWebKitTODO: a proper implementation
PassRefPtr<SimpleFontData> SimpleFontData::platformCreateScaledFontData(const FontDescription&, float scaleFactor) const
{
	notImplemented();
    return nullptr;
}

} // WebCore
