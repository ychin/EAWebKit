/*
    Copyright (C) 2009 Nokia Corporation and/or its subsidiary(-ies)
	Copyright (C) 2011, 2012, 2013 Electronic Arts, Inc. All rights reserved.

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
#include "ResourceRequest.h"

#include "ResourceHandleManagerEA.h"

namespace WebCore {

// The max connection count per host is configurable via Parameters. The default value is currently 6, which seems to be common amongst other WebKit ports.
//
unsigned initializeMaximumHTTPConnectionCountPerHost()
{
    return WebCore::ResourceHandleManager::sharedInstance()->GetMaxParallelConnectionsPerHost();
}

}

