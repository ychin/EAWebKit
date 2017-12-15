/*
    Copyright (C) 2008 Nokia Corporation and/or its subsidiary(-ies)
    Copyright (C) 2008 Holger Hans Peter Freyther
    Copyright (C) 2006, 2008 Apple Inc. All rights reserved.
    Copyright (C) 2007 Nicholas Shanks <webkit@nickshanks.com>
    Copyright (C) 2011, 2012, 2014 Electronic Arts, Inc. All rights reserved.

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
#include "FontCache.h"
#include "FontDescription.h"
#include "FontPlatformData.h"
#include "Font.h"
#include <utility>
#include <wtf/ListHashSet.h>
#include <wtf/StdLibExtras.h>
#include <wtf/text/StringHash.h>

#include <internal/include/EAWebKit_p.h>
#include <internal/include/EAWebKitString.h>
#include <EAWebkit/EAWebKitTextInterface.h>

using namespace WTF;

namespace WebCore {

float ConvertFontSizeToPoints(float fontSizeInPixels)
{
	// Before sending down the font size to the platform layer, WebCore converts the font size to be in pixels using the translation provided at http://www.w3.org/TR/css3-values/. 
	// So 1 css inch = 96 css pixels = 72 css points. A good explanation can be found at http://docs.webplatform.org/wiki/tutorials/understanding-css-units 

	// Note that 1 css inch is not necessarily map to 1 physical inch and 1 css pixel does not necessarily map to 1 device pixel. WebCore calculations are done in CSSPrimitiveValue::computeLengthDouble. In CSSHelper.h, cssPixelsPerInch is set to 96.

	// Our text implementation expects the font size to be in terms of points. So we do that conversion here by using following formula.
	// fontSizeInPoints = 0.75 * fontSizeInPixels

	// The dpi and pt provide a consistent size when printing but not when rendering on screen. pt and pixel are correlated with a constant factor. An image printer program can actually understand dpi etc.

	float fontSizeInPoints = 0.75f * fontSizeInPixels;  

	// Turns out that some queries on the fonts does not really need to have size (for example, is the font fixed pitch). However, EAText needs a size specification when creating fonts. So we pass a dummy size here.
	if(fontSizeInPoints <= 0.0f)
		fontSizeInPoints = 1.0f; 
	
	return fontSizeInPoints;
}

float ConvertFontWeight(FontWeight weight)
{
	COMPILE_ASSERT( FontWeight100 == 0, FontWeight);
	COMPILE_ASSERT( FontWeight900 == 8, FontWeight);

	return ((weight + 1) * 100.f);
}

bool ShouldSmoothText(float fontSize, FontSmoothingMode mode)
{
	bool shouldSmooth = false;

	switch (mode)
	{
	case NoSmoothing:
		break;

	case SubpixelAntialiased:
	case Antialiased:
		shouldSmooth = true;
		break;

	case AutoSmoothing:
	default:
		{
			const uint32_t smoothAboveSize = EA::WebKit::GetParameters().mSmoothFontSize;    
			if ((uint32_t) fontSize >= smoothAboveSize) 
			{
				shouldSmooth = true;
			}
		}
		break;
	}

	return shouldSmooth;
}

static void CopyFontFamilyName(char16_t* pDest, const WTF::AtomicString& name)
{
	size_t numCharsToCopy = eastl::min_alt<size_t>(name.length(), EA::WebKit::kFamilyNameCapacity - 1);
	EAW_ASSERT_MSG(numCharsToCopy < EA::WebKit::kFamilyNameCapacity, "Font family name too long. Font is not going to be detected properly");
	EA::Internal::Strncpy(pDest, name.characters(),numCharsToCopy);
	pDest[numCharsToCopy] = 0;
}


void FontCache::platformInit()
{
}

static EA::WebKit::IFont* createFont(const FontDescription& fontDescription, const AtomicString& familyName, const UChar* sampleChars = NULL, size_t length = 0)
{
	//Given the info, check if the text system is capable of creating a font. If yes, create it otherwise return null.
	if(EA::WebKit::ITextSystem* pTextSystem = EA::WebKit::GetTextSystem())
	{
		EA::WebKit::TextStyle textStyle;  

		// Note by Arpit Baldeva:
		// The way WebCore expects us to handle creation of FontPlatformData is to take the familyName
		// in FontCacheEA.cpp and return a valid pointer only if that particular font can be created.
		// In CSS, one can specify multiple font families in order of preference. If you return NULL,
		// WebCore sends the next highest priority font. Note that it also converts generic font family
		// name (such as serif) to the one you specify as your default in the Settings (For example,
		// Times New Roman for us). So the system is supposed to be pretty simple.

		// The problem we have is that EAText wants to builds a priority array in one shot. Also, if you
		// request a font and it is not found, it may return one of the fallback fonts even though you may
		// have something down the priority order available in EAText.

		// So what we do here is build up that array.

		const uint32_t familyNameArrayCapacity = pTextSystem->GetFamilyNameArrayCapacity();

		uint32_t i = 0;
		if(!familyName.isEmpty()) //family name can be empty in some situations. Say using "^" character.
		{
			CopyFontFamilyName(textStyle.mFamilyNameArray[i],familyName);
			++i;
		}

		if(i < familyNameArrayCapacity)
		{
			*textStyle.mFamilyNameArray[i] = 0;
		}

		const float fontSizeInPixels = fontDescription.computedSize();
		textStyle.mfSize = ConvertFontSizeToPoints(fontSizeInPixels); 

		if(fontDescription.italic() == WebCore::FontItalicOn)
			textStyle.mStyle = EA::WebKit::kStyleItalic; 

		textStyle.mfWeight = ConvertFontWeight(fontDescription.weight());

		if(fontDescription.smallCaps())
			textStyle.mVariant = EA::WebKit::kVariantSmallCaps;

		//Pitch is pretty much ignored as the font family itself determines the type of the pitch

		if(ShouldSmoothText(fontSizeInPixels, fontDescription.fontSmoothing()))
			textStyle.mSmooth = EA::WebKit::kSmoothEnabled;    

		//EAW_ASSERT_MSG(length <= 1,"Only first character is looked up"); //Turns out that length can be 2 for characters beyond UCS-2 (which we don't intend to support)
		// We test only first char whereas we should preferably be testing full array. However, other ports are suffering from the same issue. 
		// There is a talk about renaming this function to work with single char. In practice, this bug is unlikely to happen because it'd be pretty strange to have multiple script characters form
		// a single word.

		return pTextSystem->GetFont(textStyle,sampleChars ? sampleChars[0] : EA::WebKit::kCharInvalid);
	}

	return nullptr;
}
PassOwnPtr<FontPlatformData> FontCache::createFontPlatformData(const FontDescription& fontDescription, const AtomicString& familyName)
{
	if(EA::WebKit::IFont* pFont = createFont(fontDescription,familyName))
	{
		FontPlatformDataPrivate* privData = new FontPlatformDataPrivate();
		privData->mpFont = pFont;
		return adoptPtr(new FontPlatformData(privData,fontDescription.computedSize(),fontDescription.weight() >= FontWeightBold,fontDescription.italic() == FontItalicOn));
	}

	return nullptr;
}


PassRefPtr<SimpleFontData> FontCache::systemFallbackForCharacters(const FontDescription& fontDescription, const WebCore::SimpleFontData*, bool, const UChar* sampleChars, int length)
{
    // This function is called if WebCore can't display the character with the font data it received from the platform earlier. The situation mostly arises for lightweight font resources.
	// For example, by default, our "Times New Roman" does not contain foreign language characters (such as Chinese). A page might request Times New Roman for Chinese script. Since during the initial query
	// (FontCache::createFontPlatformData), a sample character is not available, we'd end up returning a font incapable of rendering the actual character. So WebCore makes a 2nd attempt by specifying the 
	// sample Char. We now create appropriate font and send it back to WebCore.

	if(EA::WebKit::IFont* pFont = createFont(fontDescription,fontDescription.firstFamily(),sampleChars,length))
	{
		FontPlatformDataPrivate* privData = new FontPlatformDataPrivate();
		privData->mpFont = pFont;
		FontPlatformData fontPlatformData(privData,fontDescription.computedSize(),fontDescription.weight() >= FontWeightBold,fontDescription.italic() == FontItalicOn);
		return getCachedFontData(&fontPlatformData, DoNotRetain);
	}
	
	return NULL;
}

PassRefPtr<SimpleFontData> FontCache::getLastResortFallbackFont(const FontDescription& fontDescription, ShouldRetain shouldRetain)
{
	// If a font could not be obtained, just get one which the application trusts most. This will also be the font that will
	// be used in case downloadable font fails.

	AtomicString fontFamily(EA::WebKit::GetParameters().mFontFamilyStandard);

	FontPlatformData* platformData = getCachedFontPlatformData(fontDescription,  fontFamily, false); 
	return getCachedFontData(platformData,shouldRetain);
}

void FontCache::getTraitsInFamily(const AtomicString&, Vector<unsigned>&)
{
	//Most ports ignore this. Seems like called only for downloadable font.
}


} // namespace WebCore

