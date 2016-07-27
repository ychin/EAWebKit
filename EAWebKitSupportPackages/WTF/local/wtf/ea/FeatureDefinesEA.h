/*
Copyright (C) 2014 Electronic Arts, Inc.  All rights reserved.

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

#if !defined(ENABLE_IFRAME_SEAMLESS)
#define ENABLE_IFRAME_SEAMLESS 1
#endif

#if !defined(ENABLE_PROMISES)
#define ENABLE_PROMISES 1
#endif

#if !defined(ENABLE_CHANNEL_MESSAGING)
#define ENABLE_CHANNEL_MESSAGING 1
#endif

#if !defined(ENABLE_SQL_DATABASE)
#define ENABLE_SQL_DATABASE 1
#endif

#if !defined(ENABLE_FILTERS)
#define ENABLE_FILTERS 1
#endif

#if !defined(ENABLE_FULLSCREEN_API)
#define ENABLE_FULLSCREEN_API 1
#endif

#if !defined(ENABLE_HIGH_DPI_CANVAS)
#define ENABLE_HIGH_DPI_CANVAS 1
#endif

#if !defined(ENABLE_JAVASCRIPT_DEBUGGER)
#define ENABLE_JAVASCRIPT_DEBUGGER 1
#endif

#if !defined(ENABLE_LEGACY_CSS_VENDOR_PREFIXES)
#define ENABLE_LEGACY_CSS_VENDOR_PREFIXES 1
#endif

#if !defined(ENABLE_METER_ELEMENT)
#define ENABLE_METER_ELEMENT 1
#endif

#if !defined(ENABLE_PAGE_VISIBILITY_API)
#define ENABLE_PAGE_VISIBILITY_API 1
#endif

#if !defined(ENABLE_SHARED_WORKERS)
#define ENABLE_SHARED_WORKERS 1
#endif

#if !defined(ENABLE_SVG)
#define ENABLE_SVG 1
#endif

#if ENABLE_SVG
#if !defined(ENABLE_SVG_FONTS)
#define ENABLE_SVG_FONTS 1
#endif
#endif

#if !defined(ENABLE_WEB_SOCKETS)
#define ENABLE_WEB_SOCKETS 1
#endif

#if !defined(ENABLE_WORKERS)
#define ENABLE_WORKERS 1
#endif

#if !defined(ENABLE_PROGRESS_ELEMENT)
#define ENABLE_PROGRESS_ELEMENT 1
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

#if !defined(ENABLE_INSPECTOR)
#define ENABLE_INSPECTOR 1
#endif

#if !defined(ENABLE_INSPECTOR_SERVER)
#define ENABLE_INSPECTOR_SERVER 1
#endif

#if !defined(ENABLE_REQUEST_ANIMATION_FRAME)
#define ENABLE_REQUEST_ANIMATION_FRAME 1
#endif

#if !defined(ENABLE_IMAGE_COMPRESSION)
#define ENABLE_IMAGE_COMPRESSION 1 /*Also, need the runtime preference bool in addition to this define*/
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

#if !defined(ENABLE_CSS_STICKY_POSITION)
#define ENABLE_CSS_STICKY_POSITION 1
#endif

#if !defined(ENABLE_CSS_TRANSFORMS_ANIMATIONS_TRANSITIONS_UNPREFIXED)
#define ENABLE_CSS_TRANSFORMS_ANIMATIONS_TRANSITIONS_UNPREFIXED 1
#endif

#if !defined(ENABLE_PLUGIN_PACKAGE_SIMPLE_HASH)
#define ENABLE_PLUGIN_PACKAGE_SIMPLE_HASH 1
#endif

#if !defined(ENABLE_CSS_FILTERS)
#define ENABLE_CSS_FILTERS 1
#endif

#if !defined(ENABLE_COMPARE_AND_SWAP) 
#define ENABLE_COMPARE_AND_SWAP 1
#endif

// perspective transform (works regardless of CPU/GPU)
#if !defined(ENABLE_3D_RENDERING)
#define ENABLE_3D_RENDERING 1
#endif

#if !defined(ENABLE_TOUCH_EVENTS)
#define ENABLE_TOUCH_EVENTS 1
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

#if !defined(ENABLE_MEDIA_STATISTICS)
#define ENABLE_MEDIA_STATISTICS 0
#endif

#if !defined(ENABLE_MEDIA_STREAM)
#define ENABLE_MEDIA_STREAM 0
#endif

#if !defined(ENABLE_VIDEO_TRACK)
#define ENABLE_VIDEO_TRACK 0
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

#if !defined(ENABLE_VIEW_MODE_CSS_MEDIA)
#define ENABLE_VIEW_MODE_CSS_MEDIA 0
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

/* JIT off for now in x86_64 + MSVC */

#if !defined(ENABLE_JIT) && ((CPU(X86_64) && COMPILER(MSVC)) || defined(EA_PLATFORM_CONSOLE))
#define ENABLE_JIT 0
#endif

// Make sure this is off on OSX (and elsewhere)
// In theory it works on OSX but has a pthread dependency we need to address before it can be turned on
#if !defined(ENABLE_DFG_JIT)
#define ENABLE_DFG_JIT 0
#endif

#endif
