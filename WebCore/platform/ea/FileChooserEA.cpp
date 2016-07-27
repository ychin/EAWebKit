/*
 * Copyright (C) 2006 Nikolas Zimmermann <zimmermann@kde.org>
 * Copyright (C) 2011, 2012 Electronic Arts, Inc. All rights reserved.
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
#include "FileChooser.h"

//#include "Font.h"
//#include "LocalizedStrings.h"
//#include "TextRun.h"

namespace WebCore {

// Note by Arpit Baldeva: This function returns a string that fits within the "width" pixels given the font used. If the string is bigger, then
// "..." (Ellipsis) is used. It is up to implementation to place the Ellipsis at the left/middle/side of the string.
//
/*
String FileChooser::basenameForWidth(const Font& f, int width) const
{
	String string;
	if (width <= 0)
        return string;

    if (m_filenames.isEmpty())
        string = fileButtonNoFileSelectedLabel();
    else if (m_filenames.size() == 1) {
        String fname = m_filenames[0];
        ElidedTextEA(f,width, 1,  fname, string);
    }   
    else {
		String fname = WTF::String::format("%d file(s)",(int)(m_filenames.size()));
		ElidedTextEA(f,width, 1,  fname, string);
    }
    return string;
}*/
}

// vim: ts=4 sw=4 et
