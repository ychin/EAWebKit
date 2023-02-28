/*
Copyright (C) 2014, 2015 Electronic Arts, Inc.  All rights reserved.

Redistribution and use in source and binary forms, with or without
modification, are permitted provided that the following conditions
are met:

1.  Redistributions of source code must retain the above copyright
    notice, this list of conditions and the following disclaimer.
2.  Redistributions in binary form must reproduce the above copyright
    notice, this list of conditions and the following disclaimer in the
    documentation and/or other materials provided with the distribution.
3.  Neither the name of Electronic Arts, Inc. ("EA") nor the names of
    its contributors may be used to endorse or promote products derived
    from this software without specific prior written permission.

THIS SOFTWARE IS PROVIDED BY ELECTRONIC ARTS AND ITS CONTRIBUTORS "AS IS" AND ANY
EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE IMPLIED
WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE ARE
DISCLAIMED. IN NO EVENT SHALL ELECTRONIC ARTS OR ITS CONTRIBUTORS BE LIABLE FOR ANY
DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES
(INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES;
LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND
ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT
(INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE OF
THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
*/

	
#ifndef WTF_FeatureDefinesEA_h
#define WTF_FeatureDefinesEA_h

#include <EABase/eabase.h>
/* Some comments duplicated from FeatureDefines.h
Use this file to list _all_ ENABLE() macros. Define the macros to be one of the following values:
*  - "0" disables the feature by default. The feature can still be enabled for a specific port or environment.
*  - "1" enables the feature by default. The feature can still be disabled for a specific port or environment.

Use this file to define ENABLE() macros only. Do not use this file to define USE() or macros !

*/

/* The list below is not comprehensive (meaning it does not include all the webcore/wtf/javascriptcore defines) but we can build over time as needed. */

#if !defined(ENABLE_PROMISES)
#define ENABLE_PROMISES 1
#endif

#if !defined(ENABLE_CHANNEL_MESSAGING)
#define ENABLE_CHANNEL_MESSAGING 1
#endif

#if !defined(ENABLE_SQL_DATABASE)
#define ENABLE_SQL_DATABASE 1
#endif

#if !defined(ENABLE_FULLSCREEN_API)
#define ENABLE_FULLSCREEN_API 1
#endif

#if !defined(ENABLE_HIGH_DPI_CANVAS)
#define ENABLE_HIGH_DPI_CANVAS 1
#endif

#if !defined(ENABLE_LEGACY_CSS_VENDOR_PREFIXES)
#define ENABLE_LEGACY_CSS_VENDOR_PREFIXES 1
#endif

#if !defined(ENABLE_METER_ELEMENT)
#define ENABLE_METER_ELEMENT 1
#endif

#if !defined(ENABLE_SHARED_WORKERS)
#define ENABLE_SHARED_WORKERS 1
#endif

#if !defined(ENABLE_SVG_FONTS)
#define ENABLE_SVG_FONTS 1
#endif

#if !defined(ENABLE_WEB_SOCKETS)
#define ENABLE_WEB_SOCKETS 1
#endif

#if !defined(ENABLE_NOTIFICATIONS)
#define ENABLE_NOTIFICATIONS 1
#endif

#if !defined(ENABLE_DATALIST_ELEMENT)
#define ENABLE_DATALIST_ELEMENT 1
#endif

#if !defined(ENABLE_VIDEO)
#define ENABLE_VIDEO 1
#endif

#if !defined(ENABLE_VIDEO_TRACK)
#define ENABLE_VIDEO_TRACK 1
#endif

#if !defined(ENABLE_WEBVTT_REGIONS)
#define ENABLE_WEBVTT_REGIONS 1
#endif

#if !defined(ENABLE_REQUEST_ANIMATION_FRAME)
#define ENABLE_REQUEST_ANIMATION_FRAME 1
#endif

#if !defined(ENABLE_CSS_BOX_DECORATION_BREAK)
#define ENABLE_CSS_BOX_DECORATION_BREAK 1
#endif

#if !defined(ENABLE_CSS_IMAGE_SET)
#define ENABLE_CSS_IMAGE_SET 1
#endif

#if !defined(ENABLE_CSS_REGIONS)
#define ENABLE_CSS_REGIONS 1
#endif

#if !defined(ENABLE_CSS_TRANSFORMS_ANIMATIONS_TRANSITIONS_UNPREFIXED)
#define ENABLE_CSS_TRANSFORMS_ANIMATIONS_TRANSITIONS_UNPREFIXED 1
#endif


#if !defined(ENABLE_CSS_SHAPES)
#define ENABLE_CSS_SHAPES 1
#endif

#if !defined(ENABLE_CSS_SELECTORS_LEVEL4)
#define ENABLE_CSS_SELECTORS_LEVEL4 1
#endif

#if !defined(ENABLE_CSS_GRID_LAYOUT)
#define ENABLE_CSS_GRID_LAYOUT 1
#endif

#if !defined(ENABLE_CSS_SHAPE_INSIDE) 
#define ENABLE_CSS_SHAPE_INSIDE 1
#endif

#if !defined(ENABLE_VIEW_MODE_CSS_MEDIA)
#define ENABLE_VIEW_MODE_CSS_MEDIA 1
#endif

#if !defined(ENABLE_COMPARE_AND_SWAP) 
#define ENABLE_COMPARE_AND_SWAP 1
#endif

// perspective transform (works regardless of CPU/GPU)
#if !defined(ENABLE_3D_TRANSFORMS)
#define ENABLE_3D_TRANSFORMS 1
#endif

#if !defined(ENABLE_TOUCH_EVENTS)
#define ENABLE_TOUCH_EVENTS 1
#endif

#if !defined(ENABLE_WEBVTT_REGIONS)
#define ENABLE_WEBVTT_REGIONS 1
#endif

#if !defined(ENABLE_SUBPIXEL_LAYOUT)
#define ENABLE_SUBPIXEL_LAYOUT 1
#endif

#if !defined(ENABLE_MEDIA_STATISTICS)
#define ENABLE_MEDIA_STATISTICS 1
#endif

#if !defined(ENABLE_MEDIA_CONTROLS_SCRIPT)
#define ENABLE_MEDIA_CONTROLS_SCRIPT 1
#endif

#if !defined(ENABLE_XHR_TIMEOUT)
#define ENABLE_XHR_TIMEOUT 1
#endif

#if !defined(ENABLE_MOUSE_CURSOR_SCALE)
#define ENABLE_MOUSE_CURSOR_SCALE 1
#endif

#if !defined(ENABLE_CURSOR_VISIBILITY)
#define ENABLE_CURSOR_VISIBILITY 1
#endif

/**********************************************************************************************
//
// Disabled Features
//
***********************************************************************************************/

#if !defined(ENABLE_MATHML)
#define ENABLE_MATHML 0
#endif

#if !defined(ENABLE_GEOLOCATION)
#define ENABLE_GEOLOCATION 0
#endif

#if !defined(ENABLE_DETAILS_ELEMENT)
#define ENABLE_DETAILS_ELEMENT 0
#endif

#if !defined(ENABLE_ICONDATABASE)
#define ENABLE_ICONDATABASE 0
#endif

#if !defined(ENABLE_XSLT)
#define ENABLE_XSLT 0 /*Need libxslt*/
#endif

#if !defined(DIRECTORY_UPLOAD)
#define DIRECTORY_UPLOAD 0
#endif

#if !defined(ENABLE_QUOTA)
#define ENABLE_QUOTA 0
#endif

#if !defined(ENABLE_DASHBOARD_SUPPORT)
#define ENABLE_DASHBOARD_SUPPORT 0
#endif

#if !defined(ENABLE_MHTML)
#define ENABLE_MHTML 0
#endif

#if !defined(ENABLE_BLOB)
#define ENABLE_BLOB 0
#endif

#if !defined(ENABLE_INPUT_TYPE_COLOR)
#define ENABLE_INPUT_TYPE_COLOR 0
#endif

#if !defined(ENABLE_INPUT_SPEECH)
#define ENABLE_INPUT_SPEECH 0
#endif

#if !defined(ENABLE_WEB_AUDIO)
#define ENABLE_WEB_AUDIO 0
#endif

#if !defined(ENABLE_MEDIA_SOURCE)
#define ENABLE_MEDIA_SOURCE 0
#endif

#if !defined(ENABLE_MEDIA_STREAM)
#define ENABLE_MEDIA_STREAM 0
#endif

#if !defined(ENABLE_TOUCH_ICON_LOADING)
#define ENABLE_TOUCH_ICON_LOADING 0
#endif

#if !defined(ENABLE_NETSCAPE_PLUGIN_API)
#define ENABLE_NETSCAPE_PLUGIN_API 0
#endif

#if !defined(ENABLE_WEB_TIMING)
#define ENABLE_WEB_TIMING 0
#endif

#if !defined(ENABLE_FTPDIR)
#define ENABLE_FTPDIR 0
#endif

#if !defined(ENABLE_CONTEXT_MENUS)
#define ENABLE_CONTEXT_MENUS 0
#endif

#if !defined(ENABLE_WEBGL)
#define ENABLE_WEBGL 0
#endif

#if !defined(ENABLE_ACCELERATED_2D_CANVAS)
#define ENABLE_ACCELERATED_2D_CANVAS 0
#endif

#if !defined(ENABLE_CSS_SELECTOR_JIT)
#define ENABLE_CSS_SELECTOR_JIT 0
#endif

/* JIT off for now in x86_64 + MSVC */

#if defined(EA_PLATFORM_CONSOLE) || defined (EA_PLATFORM_STADIA)
#define ENABLE_JIT 0
#define ENABLE_DFG_JIT 0
#define ENABLE_LLINT_C_LOOP 1
#else
#define ENABLE_LLINT_C_LOOP 0
#define ENABLE_JIT 1
#define ENABLE_DFG_JIT 1
#endif

#endif
