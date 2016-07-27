/*
 * Copyright (C) 2011 Collabora Ltd.
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
#include "PlatformPathCairo.h"

#include <cairo.h>

namespace WebCore {

//+EAWebKitChange
//10/14/2011 - This is an important change to avoid allocation before Init call.
cairo_surface_t* CairoPath::pathSurfaceInst = NULL;

cairo_surface_t* CairoPath::pathSurface()
{
	if(!CairoPath::pathSurfaceInst)
		CairoPath::pathSurfaceInst = cairo_image_surface_create(CAIRO_FORMAT_A8, 1, 1);

	return CairoPath::pathSurfaceInst;
}

void CairoPath::finalize()
{
	if(CairoPath::pathSurfaceInst)
	{
		cairo_surface_destroy(CairoPath::pathSurfaceInst);
		CairoPath::pathSurfaceInst = NULL;
	}
}

CairoPath::CairoPath()
: m_cr(adoptRef(cairo_create(CairoPath::pathSurface())))
{
}
//-EAWebKitChange

}
