/*
 * Copyright (C) 2010 Apple Inc. All rights reserved.
 * Copyright (C) 2012, 2013 Electronic Arts, Inc. All rights reserved.
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

#ifndef PageAllocationAligned_h
#define PageAllocationAligned_h

#include <wtf/OSAllocator.h>
#include <wtf/PageReservation.h>

namespace WTF {

class PageAllocationAligned : private PageBlock {
public:
    PageAllocationAligned()
    {
    }

    using PageBlock::operator bool;
    using PageBlock::size;
    using PageBlock::base;

    WTF_EXPORT_PRIVATE static PageAllocationAligned allocate(size_t size, size_t alignment, OSAllocator::Usage usage = OSAllocator::UnknownUsage, bool writable = true);

    WTF_EXPORT_PRIVATE void deallocate();

private:
//+EAWebKitChange
// 06/25/2012 - Disabled on PLATFORM(EA)    
#if OS(DARWIN) && !PLATFORM(EA)
    PageAllocationAligned(void* base, size_t size)
        : PageBlock(base, size, false)
    {
    }
//-EAWebKitChange    
#else
	//+EAWebKitChange
	//02/25/2013 - Added alignment support
	PageAllocationAligned(void* base, size_t size, void* reservationBase, size_t reservationSize, size_t alignment)
        : PageBlock(base, size, false)
        , m_reservation(reservationBase, reservationSize, false)
        , m_alignment(alignment)
    {
    }

    PageBlock m_reservation;
    size_t m_alignment;
	//-EAWebKitChange
#endif
};


} // namespace WTF

using WTF::PageAllocationAligned;

#endif // PageAllocationAligned_h
