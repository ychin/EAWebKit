/*
 * Copyright C 2006 Zack Rusin <zack@kde.org>
 * Copyright (C) 2010 Nokia Corporation and/or its subsidiary(-ies).
 * Copyright (C) 2011 Electronic Arts, Inc. All rights reserved.
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
 */

#include "config.h"
#include "SearchPopupMenuEA.h"
#include "NotImplemented.h"

namespace WebCore {

SearchPopupMenuEA::SearchPopupMenuEA(PassRefPtr<PopupMenu> popup)
    : m_popup(popup)
{
}

PopupMenu* SearchPopupMenuEA::popupMenu()
{
    return m_popup.get();
}

void SearchPopupMenuEA::saveRecentSearches(const AtomicString&, const Vector<String>&)
{
	notImplemented();
}

void SearchPopupMenuEA::loadRecentSearches(const AtomicString&, Vector<String>&)
{
	notImplemented();
}

bool SearchPopupMenuEA::enabled()
{
    return false;
}

}
