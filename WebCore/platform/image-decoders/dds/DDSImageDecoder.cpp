/*
Copyright (C) 2015 Electronic Arts, Inc.  All rights reserved.

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

#include "config.h"
#include "DDSImageDecoder.h"
#include "ddshelper.h"
#include <internal/include/EAWebKit_p.h>

namespace WebCore {

DDSImageDecoder::DDSImageDecoder(ImageSource::AlphaOption alphaOption,
                                 ImageSource::GammaAndColorProfileOption gammaAndColorProfileOption)
    : ImageDecoder(alphaOption, gammaAndColorProfileOption)
{
	setIgnoreGammaAndColorProfile(EA::WebKit::GetParameters().mIgnoreGammaAndColorProfile ? true : false);
}

DDSImageDecoder::~DDSImageDecoder()
{
}

bool DDSImageDecoder::isSizeAvailable()
{
    if (!isAllDataReceived())
        return false;

    if (!ImageDecoder::isSizeAvailable())
         frameBufferAtIndex(0); // instead of calling decode, we call this one to allow the ImageFrame to be set up.

    return ImageDecoder::isSizeAvailable();
}

bool DDSImageDecoder::setSize(unsigned width, unsigned height)
{
    if (!ImageDecoder::setSize(width, height))
        return false;
    return true;
}

ImageFrame* DDSImageDecoder::frameBufferAtIndex(size_t index)
{
    if (index)
        return 0;

    if (m_frameBufferCache.isEmpty()) {
        m_frameBufferCache.resize(1);
        m_frameBufferCache[0].setPremultiplyAlpha(m_premultiplyAlpha);
    }

    ImageFrame& frame = m_frameBufferCache[0];
    if (frame.status() != ImageFrame::FrameComplete)
        decode();
    return &frame;
}

bool DDSImageDecoder::setFailed()
{
    return ImageDecoder::setFailed();
}

unsigned DDSImageDecoder::frameBytesAtIndex(size_t index) const
{
    // This function is being called to figure out the decoded data size for cache calculation purpose. As we don't do any decode, we override this function to return a simple 4 byte (1 pixel). 
    // Technically we should return 0 but that might have some side effects. 1 pixel is small enough to avoid screwing up the cache calculations and at the same time, is legit value.
    return 4; 
}

bool DDSImageDecoder::decode()
{
    const unsigned char * data = (const unsigned char*) m_data->data();
    ssize_t dataLen = m_data->size();

    if (data)
    {
        ddsBuffer* buffer = (ddsBuffer*) data;
        int width, height;
        eDDSPixelFormat pf;
        DDSGetInfo(buffer, &width,&height,&pf );
        unsigned char *pixels = buffer->data;

        if (pf != DDS_PF_ARGB8888)
        {
            ASSERT_WITH_MESSAGE(false, "Only ARGB8888 format is currently supported");
            setFailed();
            return false;
        }
        
        setSize(width, height);
        ImageFrame& frame = m_frameBufferCache[0];
        frame.setSize(width, height, false);

        int mipMapCount = buffer->mipMapCount; 
        if (mipMapCount > 1)
        {
            ASSERT_WITH_MESSAGE(false, "Mipmap count more than expected. Only base texture will be used.");
        }

        // Instead of having a buffer in ImageFrame that is just a duplicate of raw ARGB in the sharedBuffer, we just fix up the frame's m_bytes 
        // pointer to point to it. The memory savings here is substantial as each image could be multiple MBs.
        frame.m_bytes = (ImageFrame::PixelData*) pixels;
        // Use a straight up memcpy instead DDSDecompressARGB8888 from ddshelper.h as that guy does a byte by byte copy
        //ImageFrame::PixelData* address = frame.getAddr(0, 0);
        //memcpy(address, pixels, width * height * 4);
        m_frameBufferCache.first().setStatus(ImageFrame::FrameComplete);

        return true;
    }
    
    ASSERT_WITH_MESSAGE(false, "Something went wrong in decoding");
    setFailed();
    return false;
}

} // namespace WebCore
