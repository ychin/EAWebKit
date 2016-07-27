/*
    Copyright (C) 2008 Holger Hans Peter Freyther
    Copyright (C) 2009 Torch Mobile Inc. http://www.torchmobile.com/
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
#include "FontPlatformData.h"

namespace WebCore {

FontPlatformData::FontPlatformData(FontPlatformDataPrivate* privData, int size, bool bold, bool italic)
	: m_syntheticBold(bold)
	, m_syntheticOblique(italic)
	, m_orientation(Horizontal)
	, m_size(size) //EAWebKitTODO: Ideally, this m_size should be equal everywhere so we need to get rid of scaling
	, m_widthVariant(RegularWidth)
	, m_privData(privData)
	, m_isColorBitmapFont(false)
	, m_isCompositeFontReference(false)
{
	
}

FontPlatformData::~FontPlatformData()
{
    if (m_privData && m_privData != hashTableDeletedFontValue())
	{
		--m_privData->refCount;

		if (!m_privData->refCount)
		{
			delete m_privData;
			m_privData = 0;
		}
	}
}

void FontPlatformData::platformDataInit(const FontPlatformData &other)
{
	m_privData = other.m_privData;
	if (m_privData && m_privData != hashTableDeletedFontValue())
		++m_privData->refCount;
}

const FontPlatformData& FontPlatformData::platformDataAssign(const FontPlatformData& other)
{
    EAW_ASSERT(m_privData != other.m_privData);
    
	if (m_privData && m_privData != hashTableDeletedFontValue()) 
	{
        --m_privData->refCount;
        if (!m_privData->refCount)
            delete m_privData;
    }
    
	m_privData = other.m_privData;
    
	if (m_privData && m_privData != hashTableDeletedFontValue())
        ++m_privData->refCount;
    
	return *this;
}

bool FontPlatformData::platformIsEqual(const FontPlatformData& other) const
{
    if (m_privData == other.m_privData)
        return true;

	if (!m_privData || !other.m_privData
        || m_privData == hashTableDeletedFontValue() || other.m_privData == hashTableDeletedFontValue())
        return  false;

    return m_privData->mpFont == other.m_privData->mpFont;
}

bool FontPlatformData::isFixedPitch() const
{
	// This code can be called on SVG fonts as well which we don't handle on our end.
	if (m_privData && m_privData->mpFont)
    {
        EA::WebKit::FontMetrics fontMetrics;
        m_privData->mpFont->GetFontMetrics(fontMetrics);
        return fontMetrics.mPitch == EA::WebKit::kPitchFixed;
    }
    return false;
}

EA::WebKit::IFont* FontPlatformData::font() const
{
    EAW_ASSERT(m_privData != hashTableDeletedFontValue()); //We don't check for m_privData here as it can be null for SVG fonts and this code may be called in that scenario as well
	
    if (m_privData)
        return m_privData->mpFont;
    return 0;
}

#ifndef NDEBUG
String FontPlatformData::description() const
{
    return String();
}
#endif

}
