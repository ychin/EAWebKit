/*
 * Copyright (C) 2010 Apple Inc. All rights reserved.
 * Copyright (C) 2011, 2013, 2014 Electronic Arts, Inc. All rights reserved.
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
#include "OSAllocator.h"
#include "PageBlock.h"

#include <wtf/Assertions.h> 

// It's a layering violation to include this file from here but it is a trick we can play since JavaScriptCore is embedded inside
// EAWebKit package.
#include <../../../../../WebKit/ea/Api/EAWebKit/include/EAWebKit/EAWebkitAllocator.h>

namespace WTF {

void* OSAllocator::reserveUncommitted(size_t bytes, Usage usage, bool writable, bool executable, bool includesGuardPages)
{
#if !ENABLE(JIT)
	ASSERT(!executable);
#endif
	
	EA::WebKit::Allocator* pAllocator = EA::WebKit::GetAllocator();
	if(pAllocator->SupportsOSMemoryManagement())
	{
		return pAllocator->ReserveUncommitted(bytes,writable,executable);
	}
	else
	{
		// We don't make a distinction.
		return reserveAndCommit(bytes, usage, writable, executable, includesGuardPages);
	}
}



void* OSAllocator::reserveAndCommit(size_t bytes, Usage usage, bool writable, bool executable, bool includesGuardPages)
{
#if !ENABLE(JIT)
    ASSERT(!executable);
#endif

	EA::WebKit::Allocator* pAllocator = EA::WebKit::GetAllocator();
	if(pAllocator->SupportsOSMemoryManagement())
	{
		return pAllocator->ReserveAndCommit(bytes,writable,executable);
	}
	else
	{
		return WTF::fastMallocAligned(bytes, WTF::pageSize(), 0, 0, 0);
	}
}

void OSAllocator::releaseDecommitted(void* address, size_t bytes)
{
	EA::WebKit::Allocator* pAllocator = EA::WebKit::GetAllocator();
	if(pAllocator->SupportsOSMemoryManagement())
	{
		pAllocator->ReleaseDecommitted(address,bytes);
	}
	else
	{
		WTF::fastFree(address);
	}
}


void* OSAllocator::reserveAndCommitAligned(size_t bytes, size_t alignment, void*& reserveBase, size_t& reserveSize, Usage usage, bool writable, bool executable, bool includesGuardPages)
{
#if !ENABLE(JIT)
    ASSERT(!executable);
#endif

    EA::WebKit::Allocator* pAllocator = EA::WebKit::GetAllocator();
    if(pAllocator->SupportsOSMemoryManagement())
    {
        return pAllocator->ReserveAndCommitAligned(bytes,alignment,reserveBase,reserveSize,writable,executable);
    }
    else
    {
        reserveBase =  WTF::fastMallocAligned(bytes, alignment, 0, 0, 0);
		reserveSize = bytes;
		return reserveBase;
    }
}

void OSAllocator::releaseDecommittedAligned(void* address, size_t bytes, size_t alignment)
{
	EA::WebKit::Allocator* pAllocator = EA::WebKit::GetAllocator();
	if(pAllocator->SupportsOSMemoryManagement())
	{
		pAllocator->ReleaseDecommittedAligned(address,bytes, alignment);
	}
	else
	{
		WTF::fastFree(address);
	}
}

void OSAllocator::commit(void* address, size_t bytes, bool writable, bool executable)
{
#if !ENABLE(JIT)
	ASSERT(!executable);
#endif
	
	EA::WebKit::Allocator* pAllocator = EA::WebKit::GetAllocator();
	if(pAllocator->SupportsOSMemoryManagement())
	{
		pAllocator->Commit(address, bytes,writable,executable);
	}
}

void OSAllocator::decommit(void* address, size_t bytes)
{
	EA::WebKit::Allocator* pAllocator = EA::WebKit::GetAllocator();
	if(pAllocator->SupportsOSMemoryManagement())
	{
		pAllocator->Decommit(address, bytes);
	}
}





} // namespace WTF
