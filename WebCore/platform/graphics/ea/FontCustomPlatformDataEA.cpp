/*
    Copyright (C) 2008 Nokia Corporation and/or its subsidiary(-ies)
    Copyright (C) 2011, 2012, 2013, 2014 Electronic Arts, Inc. All rights reserved.

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
#include "FontCustomPlatformData.h"
#include "FontPlatformData.h"
#include "SharedBuffer.h"
#include <EAWebKit/EAWebKit.h>
#include <internal/include/EAWebkit_p.h>
#include "WOFFFileFormat.h"

namespace WebCore {

FontCustomPlatformData::FontCustomPlatformData(PassRefPtr<SharedBuffer> buffer)  
: m_buffer(buffer)
{
	
}

FontCustomPlatformData::~FontCustomPlatformData()
{
}



FontPlatformData FontCustomPlatformData::fontPlatformData(int size, bool bold, bool italic, FontOrientation, FontWidthVariant, FontRenderingMode)
{
	FontPlatformDataPrivate* privData = new FontPlatformDataPrivate();
	privData->size = ConvertFontSizeToPoints(size);

	// Seems like if the data is "not correct", EAText would crash. For example, try "abc",3.
	privData->mpFont = EA::WebKit::GetTextSystem()->CreateNewFont(EA::WebKit::kFontTypeOutline, m_buffer->data(),(uint32_t)m_buffer->size());
	privData->mpFont->SetSize(privData->size);

	bool shouldSmooth = ShouldSmoothText(privData->size);
	EA::WebKit::Smooth smooth = shouldSmooth ? EA::WebKit::kSmoothEnabled : EA::WebKit::kSmoothNone;
	privData->mpFont->SetSmoothing(smooth);
	
	// EAWebKitTODO We are currently unable to set bold and italic info. Figure out a way for it. 

	return FontPlatformData(privData, size, bold, italic);
}

bool FontCustomPlatformData::supportsFormat(const String& format)
{
// EAWebKitTODO - Seems like new trunk is using OPENTYPE_SANITIZER for dealing with woff type. Can we use that? 
    if(equalIgnoringCase(format, "woff"))
        // If a page explicitly sets the font format to WOFF (e.g. format('woff')), we need to accept it so that the fontface can be considered for use.
        // A WOOF can be a truetype or opentype font but we don't need to verify this here since we will check if the font data can be created/supported in createFontCustomPlatformData(). 
        return true;

	if(EA::WebKit::ITextSystem* pTextSystem = EA::WebKit::GetTextSystem())
	{
		if(equalIgnoringCase(format, "truetype"))
			return pTextSystem->SupportsFormat(EA::WebKit::kTrueType);
		if(equalIgnoringCase(format, "opentype"))
			return pTextSystem->SupportsFormat(EA::WebKit::kOpenType);
	}
	return false;
}

// We need to figure out if our text system is capable of dealing with this type of data. We do it by creating a Font and then throw it right away. The reason we have to do this now
// is so that we can return a NULL for the webcore to use a fallback font. If we do it later, we crash.
static bool canHandleSourceData(const char* data, unsigned size)
{
	if (EA::WebKit::IFont* pFont = EA::WebKit::GetTextSystem()->CreateNewFont(EA::WebKit::kFontTypeOutline, data,size))
	{
		pFont->Release(); 
		return true;
	}
	return false;
}

std::unique_ptr<FontCustomPlatformData> createFontCustomPlatformData(SharedBuffer* buffer)
{
    ASSERT_ARG(buffer, buffer);

    // Check if we are dealing with a compressed WOFF font format.
    if (isWOFF(buffer))
    {
        Vector<char> sfnt;
        convertWOFFToSfnt(buffer, sfnt);
        if (!sfnt.size())
            return nullptr;
        RefPtr<SharedBuffer> bufferWOFF = SharedBuffer::adoptVector(sfnt);
        if(canHandleSourceData(bufferWOFF->data(),bufferWOFF->size()))
			return std::make_unique<FontCustomPlatformData>(bufferWOFF.get());
    }
    else
    {
        if(canHandleSourceData(buffer->data(),buffer->size()))
			return std::make_unique<FontCustomPlatformData>(buffer);
    }

	return nullptr;
}




}
