/*
Copyright (C) 2012, 2014 Electronic Arts, Inc.  All rights reserved.

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
// EAWebKitSurface.h
// By David Siems
// Maintained by EAWebKit Team
///////////////////////////////////////////////////////////////////////////////

#ifndef EAWEBKIT_EAWEBKITSURFACE_H
#define EAWEBKIT_EAWEBKITSURFACE_H

#include <EAWebKit/EAWebKitConfig.h>
#include <EAWebKit/EAWebKitSystem.h>

namespace EA { namespace WebKit {

// This class is what allows our surface rendering
// to target both hardware and software.
// It can be used in a few different ways:
//  * To back the view with a direct-to-hardware write.
//  * To back the view with a software surface.
//  * By the hardware compositor to store layers for hardware accelerated rendering. (See EAWebkitHardwareRendering.h)
class ISurface
{
public:
    struct SurfaceDescriptor
    {
        void *mData; // Pointer to the memory region in response for a Lock call on the surface for a given rectangle.
        int mStride; // Stride of the surface
    };

    ISurface(void)
    : mContentWidth(0)
    , mContentHeight(0)
	, mStride(0)
    {}

    virtual ~ISurface(void) {}
    // If rect is NULL, Lock the entire surface. Otherwise, lock the given rectangle and fill the SurfaceDescriptor struct.
	virtual void Lock(SurfaceDescriptor *pSDOut, const IntRect *rect = NULL) = 0;
    // Unlock corresponding to the previous Lock call. 
	virtual void Unlock(void) = 0;
	// This is on the only API we have for destroying the surface. It's up to the client to free all memory
	// including the ISurface itself.
	virtual void Release(void) = 0;
	// Is the backing storage for the surface allocated?
	// 12/08/2014 - IsAllocated is now deprecated and will be removed in a TBD future release.
    virtual bool IsAllocated(void) const = 0;
    // Clear the surface to transparent black.
	// 12/08/2014 - Reset is now deprecated and will be removed in a TBD future release.
    virtual void Reset(void) = 0;
    // Get the current content dimensions
	void GetContentDimensions(int *widthOut, int *heightOut) const
    {
        *widthOut = mContentWidth;
        *heightOut = mContentHeight;
    }
	// Set the current content dimensions. This calls AllocateSurface to do actual backing store allocation.
    void SetContentDimensions(int width, int height)
    {
        if (width && height && (width != mContentWidth || height != mContentHeight))
        {
            AllocateSurface(width, height);
        }

        mContentWidth = width;
        mContentHeight = height;
    }

protected:
    virtual void AllocateSurface(int width, int height) = 0;

    int mContentWidth;
    int mContentHeight;
	uint32_t mStride;
};

// This is a simple implementation of the ISurface class where backing store is simply a memory buffer. It is instantiated for main view in the software
// rendering mode if the application does not provide an ISurface in InitView. It's also instantiated for overlays in software mode.
class SoftwareSurface : public ISurface 
{
public:
    SoftwareSurface(void);

    virtual void Lock(SurfaceDescriptor *pSDOut, const IntRect *rect);
    virtual void Unlock(void) {};
    virtual void Release(void);
    virtual bool IsAllocated(void) const;
    virtual void Reset(void);

private:
    virtual void AllocateSurface(int width, int height);
    unsigned char *mData;
};

}}

#endif
