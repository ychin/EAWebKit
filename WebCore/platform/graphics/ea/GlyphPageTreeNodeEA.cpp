/*
    Copyright (C) 2008 Nokia Corporation and/or its subsidiary(-ies)
    Copyright (C) 2008 Holger Hans Peter Freyther
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
#include "GlyphPageTreeNode.h"
#include "SimpleFontData.h"
#include <wtf/Assertions.h>
#include <EAWebKit/EAWebKitTextInterface.h>

namespace WebCore {


// Given a particular font(created earlier in the flow based on the style info), request the platform specific glyph ids.
// Generally, these glyph ids are stuffed in a Glyph page (a block of contiguous 256 unicode code points). So seems like the glyphs are
// being cached here for faster render later.

// Seems like most implementations return true even if a single glyph could be found. We do the same here.
bool GlyphPage::fill(unsigned offset, unsigned length, UChar* buffer, unsigned bufferLength, const SimpleFontData* pSimpleFontData)
{
    bool haveGlyphs = false;

    // If a page has characters like http://www.charbase.com/1f603-unicode-smiling-face-with-open-mouth (d83d de03) - so wider than 2 bytes, we need to have following
	// check. Otherwise, we run into memory corruption. This does mean that we are not rendering these characters but this is not our typical prediction scenario.
	if (bufferLength <= GlyphPage::size)
    {
		if (EA::WebKit::IFont* const pFont = pSimpleFontData->getEAFont())
		{
			for (unsigned i = 0; i < bufferLength; i++)
			{
				EA::WebKit::GlyphId glyphId = EA::WebKit::kGlyphIdInvalid;

				//EAWebKitTODO: Convert this to get glyph Ids in one shot as this API is already array like.
				pFont->GetGlyphIds(&buffer[i], 1, &glyphId, false);

				if (glyphId != EA::WebKit::kGlyphIdInvalid)
				{
					setGlyphDataForIndex(i, glyphId, pSimpleFontData);
					haveGlyphs = true;
				}
				else
					setGlyphDataForIndex(i, 0, 0);
			}
		}
    }
    
	return haveGlyphs;
}
}
