/*
Copyright (C) 2010, 2011, 2012, 2013, 2014 Electronic Arts, Inc.  All rights reserved.

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

///////////////////////////////////////////////////////////////////////////////
// EAWebKitNewDelete.cpp
// By Arpit Baldeva 
///////////////////////////////////////////////////////////////////////////////

#include "config.h"
#include <internal/include/EAWebKitNewDelete.h>
#include <EAWebKit/EAWebkitAllocator.h>
#include <EAWebKit/EAWebKit.h>
#include <internal/include/EAWebKitAssert.h>

#if EAWEBKIT_MEMORY_DEBUG
#include <stdio.h>
#endif

#ifdef _MSC_VER
#define snprintf _snprintf

// Code below stops exporting std::mutex and _init_locks symbols in the dll - http://social.msdn.microsoft.com/Forums/vstudio/en-US/4692205a-0296-4f41-adbb-fa8339597f5c/unwanted-exports 
#include <new>
namespace std { const nothrow_t nothrow = nothrow_t(); }

#endif

namespace EA
{
namespace WebKit
{
extern	Allocator* spEAWebKitAllocator;
}
}

void* operator new(size_t size)
{
	return EA::WebKit::spEAWebKitAllocator->Malloc(size, 0, "EAWebKit Anonymous new");
}

void* operator new[](size_t size)
{
    return EA::WebKit::spEAWebKitAllocator->Malloc(size, 0, "EAWebKit Anonymous new[]");
}

void operator delete(void *p)
{
	EA::WebKit::spEAWebKitAllocator->Free(p,0);
}

void operator delete[](void *p)
{
	EA::WebKit::spEAWebKitAllocator->Free(p,0);
}

//EAWebKit code does not call std::nothrow_t variations but some system libs may.
void* operator new(size_t size, const std::nothrow_t& /*nothrow_constant*/)
{
	return operator new(size);
}

void operator delete(void *p, const std::nothrow_t& /*nothrow_constant*/)
{
	operator delete(p);
}

void* operator new[](size_t size, const std::nothrow_t& /*nothrow_constant*/)
{
	return operator new[](size);
}

void operator delete[](void *p, const std::nothrow_t& /*nothrow_constant*/)
{
	operator delete[](p);
}

void* operator new(size_t size, const char* name, int flags, 
				   unsigned debugFlags, const char* file, int line)
{
	(void)name; (void)flags; (void)debugFlags; (void)file; (void)line;

#if EAWEBKIT_MEMORY_DEBUG
	char allocTag[256];
	snprintf(allocTag,sizeof(allocTag),"tag:%s - File:%s - Line:%d",name,file,line);
	return EA::WebKit::spEAWebKitAllocator->Malloc(size, flags, allocTag);
#else
	return EA::WebKit::spEAWebKitAllocator->Malloc(size, flags, name);
#endif
}

void* operator new(size_t size, size_t alignment, size_t alignmentOffset, const char* name, 
				   int flags, unsigned debugFlags, const char* file, int line)
{
	(void)alignmentOffset; (void)name; (void)flags; (void)debugFlags; (void)file; (void)line;

#if EAWEBKIT_MEMORY_DEBUG
	char allocTag[256];
	snprintf(allocTag,sizeof(allocTag),"tag:%s - File:%s - Line:%d",name,file,line);
	return EA::WebKit::spEAWebKitAllocator->MallocAligned(size, alignment, alignmentOffset, 0, allocTag);
#else
	return EA::WebKit::spEAWebKitAllocator->MallocAligned(size, alignment, alignmentOffset, 0, name);
#endif
}

/*
void operator delete(void *p, int flags)
{
	(void)flags;
	EA::WebKit::GetAllocator()->Free(p,0);
}
*/

void* operator new[](size_t size, const char* name, int flags, 
					 unsigned debugFlags, const char* file, int line)
{
	(void)name; (void)flags; (void)debugFlags; (void)file; (void)line;
	
#if EAWEBKIT_MEMORY_DEBUG
	char allocTag[256];
	snprintf(allocTag,sizeof(allocTag),"tag:%s - File:%s - Line:%d",name,file,line);
	return EA::WebKit::spEAWebKitAllocator->Malloc(size, flags, allocTag);
#else
	return EA::WebKit::spEAWebKitAllocator->Malloc(size, flags, name);
#endif

}

void* operator new[](size_t size, size_t alignment, size_t alignmentOffset, const char* name, 
					 int flags, unsigned debugFlags, const char* file, int line)
{
	(void)alignmentOffset;(void)name; (void)flags; (void)debugFlags; (void)file; (void)line;

#if EAWEBKIT_MEMORY_DEBUG
	char allocTag[256];
	snprintf(allocTag,sizeof(allocTag),"tag:%s - File:%s - Line:%d",name,file,line);
	return EA::WebKit::spEAWebKitAllocator->MallocAligned(size, alignment, alignmentOffset, 0, allocTag);
#else
	return EA::WebKit::spEAWebKitAllocator->MallocAligned(size, alignment, alignmentOffset, 0, name);
#endif

}

