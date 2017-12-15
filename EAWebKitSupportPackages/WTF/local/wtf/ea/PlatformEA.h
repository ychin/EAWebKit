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

#ifndef WTF_PlatformEA_h
#define WTF_PlatformEA_h

#include <EABase/eabase.h>
#include <stdlib.h>

#define WTF_PLATFORM_EA 1

#define WTF_USE_CAIRO 1

#define WTF_USE_ZLIB 0 //There doesn't seem to be any feature that is enabled that needs this. We set it to 0 for compilation purpose(or we can choose to exclude compression.cpp

// If you enable USE_SYSTEM_MALLOC, it means that fastMalloc calls implement themselves in terms of, well, system malloc. We define it to 1, exclude FastMalloc.cpp from build and supply our versions so that any fastmalloc 
// call are routed to our allocator. 
#define USE_SYSTEM_MALLOC 1 

// We also override global new operators but set GLOBAL_FASTMALLOC_NEW to 0 to avoid fastmalloc.cpp implementation. The GLOBAL_FASTMALLOC_NEW define is not necessary as it is only used inside the !USE_SYSTEM_MALLOC block 
// but we do it here anyway to provide future safety.
#define GLOBAL_FASTMALLOC_NEW 0 

// We enable tiled backing storage. The tiled backing storage allows for faster scrolling by caching bitmaps outside the visible viewport. The concept is applicable both for software rendering or GPU 
// compositing. 
// Even though we compile with tiled backing storage available, the runtime is not enabled by default. It can be turned on/off by the users of EA::WebKit::View. 
#define WTF_USE_TILED_BACKING_STORE 1

// Enable accelerated compositing. We use texture mapper mechanism to provide accelerated compositing in our port.
#define WTF_USE_ACCELERATED_COMPOSITING 1
#define WTF_USE_TEXTURE_MAPPER 1

// Turn off 3D graphics/OpenGL for now.
#define WTF_USE_3D_GRAPHICS 0
#define	WTF_USE_OPENGL_ES_2 0
#define	WTF_USE_EGL 0

#define WTF_USE_PTHREADS 0

#define U_STATIC_IMPLEMENTATION 1

#if EA_SSE
#include <xmmintrin.h>
#define EA_CACHE_PREFETCH_128(addr)  _mm_prefetch((const char*)(uintptr_t)(addr), _MM_HINT_NTA)
#else 
// There is a downside to not have an error block here in the sense that it is hard to catch a new platform. 
// But since this is just an optimization, we can live without it.
#define EA_CACHE_PREFETCH_128(addr) 
#endif
// Some of the defines below can be removed but we keep them just in case. 
#define HAVE_SYS_TIMEB_H 1
#define HAVE_ALIGNED_MALLOC 1 //This does not seem to be used anywhere anymore
#define HAVE_ISDEBUGGERPRESENT 0 // This is used in Assertions.cpp but we have that code compiled out anyway
#define HAVE_VIRTUALALLOC 0 // This is used in TCSystemAlloc.h which we don't use
#define HAVE_MMAP 0 // Used only on OS(UNIX) which we don't have.
#define ENABLE_BINDING_INTEGRITY 0

#endif //Header include guard
