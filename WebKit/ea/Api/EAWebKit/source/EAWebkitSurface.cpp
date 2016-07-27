/*
Copyright (C) 2008, 2009, 2010, 2011, 2012 Electronic Arts, Inc.  All rights reserved.

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
// EAWebKitSurface.cpp
// By David Siems
///////////////////////////////////////////////////////////////////////////////
 
#include <EAWebkit/EAWebKitSurface.h>
#include <internal/include/EAWebKitAssert.h>
#include <string.h>

namespace EA { namespace WebKit {

static const uint32_t kBytesPerPixel = 4;
SoftwareSurface::SoftwareSurface(void)
: mData(NULL)
{
    // Do nothing.
}

void SoftwareSurface::Lock(SurfaceDescriptor *pSDOut, const IntRect *rect)
{
	EAW_ASSERT_MSG(mStride == mContentWidth * kBytesPerPixel, "Stride calculation mismatch");
	
	pSDOut->mStride = mStride; 
	if (rect)
    {
        pSDOut->mData = mData + (rect->mLocation.mX * kBytesPerPixel) + (rect->mLocation.mY * mStride) ;
        EAW_ASSERT(pSDOut->mData < mData + (mContentHeight * mStride));
        EAW_ASSERT(pSDOut->mData >= mData);
    }
    else
    {
        pSDOut->mData = mData;
    }

}

void SoftwareSurface::AllocateSurface(int width, int height) 
{
    delete[] mData;
	mStride = width * kBytesPerPixel;
	mData = new unsigned char[height * mStride];
}

void SoftwareSurface::Release(void) 
{
    delete[] mData;
    mData = NULL;
	mStride = 0;

	delete this;
}

bool SoftwareSurface::IsAllocated(void) const
{
    return mData != NULL;
}

void SoftwareSurface::Reset(void)
{
    memset(mData, 0, mContentHeight * mStride);
}

}}
