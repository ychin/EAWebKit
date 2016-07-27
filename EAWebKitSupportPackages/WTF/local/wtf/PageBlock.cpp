/*
 * Copyright (C) 2010 Apple Inc. All rights reserved.
 * Copyright (C) 2011, 2012 Electronic Arts, Inc. All rights reserved.
 *
 * Redistribution and use in source and binary forms, with or without
 * modification, are permitted provided that the following conditions
 * are met:
 * 1. Redistributions of source code must retain the above copyright
 *    notice, this list of conditions and the following disclaimer.
 * 2. Redistributions in binary form must reproduce the above copyright
 *    notice, this list of conditions and the following disclaimer in the
 *    documentation and/or other materials provided with the distribution.
 *
 * THIS SOFTWARE IS PROVIDED BY APPLE INC. AND ITS CONTRIBUTORS ``AS IS''
 * AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO,
 * THE IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR
 * PURPOSE ARE DISCLAIMED. IN NO EVENT SHALL APPLE INC. OR ITS CONTRIBUTORS
 * BE LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR
 * CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF
 * SUBSTITUTE GOODS OR SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS
 * INTERRUPTION) HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN
 * CONTRACT, STRICT LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE)
 * ARISING IN ANY WAY OUT OF THE USE OF THIS SOFTWARE, EVEN IF ADVISED OF
 * THE POSSIBILITY OF SUCH DAMAGE.
 */

#include "config.h"
#include "PageBlock.h"

#if OS(UNIX)
#include <unistd.h>
#endif

#if OS(WINDOWS)
#include <malloc.h>
#include <windows.h>
#endif

//+EAWebKitChange
//10/17/2011
#if PLATFORM(EA)
#include <../../../../../WebKit/ea/Api/EAWebKit/include/EAWebKit/EAWebkitAllocator.h>
#endif
//-EAWebKitChange
namespace WTF {

static size_t s_pageSize;
static size_t s_pageMask;

//+EAWebKitChange
//10/17/2011
//10/23/2012 - Use PLATFORM(EA) first since OS(UNIX) is true on some platforms.
#if PLATFORM(EA)
inline size_t systemPageSize()
{
	EA::WebKit::Allocator* pAllocator = EA::WebKit::GetAllocator();
	if(pAllocator->SupportsOSMemoryManagement())
	{
		return pAllocator->SystemPageSize();
	}
	else
	{
		return 4096;
	}
}
#elif OS(UNIX)
//-EAWebKitChange

inline size_t systemPageSize()
{
    return getpagesize();
}

#elif OS(WINDOWS)

inline size_t systemPageSize()
{
    static size_t size = 0;
    SYSTEM_INFO system_info;
    GetSystemInfo(&system_info);
    size = system_info.dwPageSize;
    return size;
}

#endif

size_t pageSize()
{
    if (!s_pageSize)
        s_pageSize = systemPageSize();
    ASSERT(isPowerOfTwo(s_pageSize));
    return s_pageSize;
}

size_t pageMask()
{
    if (!s_pageMask)
        s_pageMask = ~(pageSize() - 1);
    return s_pageMask;
}

} // namespace WTF
