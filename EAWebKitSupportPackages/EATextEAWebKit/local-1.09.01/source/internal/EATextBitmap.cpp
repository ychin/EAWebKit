/*
Copyright (C) 2007, 2009, 2010, 2012 Electronic Arts, Inc.  All rights reserved.

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

/////////////////////////////////////////////////////////////////////////////
// EATextBitmap.cpp
// By Paul Pedriana - 2007
/////////////////////////////////////////////////////////////////////////////


#include <EAText/EATextConfig.h>
#include <EAText/EAText.h>
#include <EAText/internal/EATextBitmap.h>
#include <coreallocator/icoreallocator_interface.h>
#include <EASTL/algorithm.h>
#include <stdlib.h>
#include <stdio.h>
#include <math.h>
#include EA_ASSERT_HEADER



namespace EA
{

namespace Text
{

namespace Internal
{
    template <typename T>
    inline void swap(T& a, T& b)
    {
        T temp(a);
        a = b;
        b = temp;
    }
}



///////////////////////////////////////////////////////////////////////////////
// Bitmap32
///////////////////////////////////////////////////////////////////////////////

Bitmap32::Bitmap32(Allocator::ICoreAllocator* pCoreAllocator, int w, int h)
  : mpData(NULL),
    mnWidth(w),
    mnHeight(h),
    mnStride(w),
    mnCapacity(0),
    mpCoreAllocator(pCoreAllocator ? pCoreAllocator : EA::Text::GetAllocator())
{
    Init(w, h);
}


bool Bitmap32::Init(int w, int h)
{
    Shutdown();
    return SetSize(w, h);
}


void Bitmap32::Shutdown()
{
    if(mpData && mpCoreAllocator)
        mpCoreAllocator->Free(mpData);

    mpCoreAllocator = NULL;
    mpData          = NULL;
    mnWidth         = 0;
    mnHeight        = 0;
    mnStride        = 0;
    mnCapacity      = 0;
}


bool Bitmap32::SetSize(int w, int h)
{
    EA_ASSERT((w >= 0) && (h >= 0));

    if((w * h) > mnCapacity) // If we need to reallocate...
    {
        if(mpCoreAllocator)
        {
            if(mpData)
                mpCoreAllocator->Free(mpData);

            mnCapacity  = w * h;
            mpData      = (Pixel*)mpCoreAllocator->Alloc(mnCapacity * sizeof(Pixel), EATEXT_ALLOC_PREFIX "Bitmap8", 0);
            EA_ASSERT(mpData != NULL);
        }
    }

    mnWidth  = w;
    mnHeight = h;
    mnStride = w * sizeof(Pixel);

    return (mpData != NULL);
}


void Bitmap32::Acquire(Pixel* pData, int w, int h, int stride)
{
    mpData          = pData;
    mnWidth         = w;
    mnHeight        = h;
    mnStride        = stride;
    mnCapacity      = w * h;
    mpCoreAllocator = NULL;
}


Bitmap32::Pixel Bitmap32::GetPixelClamped(int x, int y) const
{
    if(x < 0)
        x = 0;
    else if(x >= mnWidth)
        x = mnWidth - 1;

    if(y < 0)
        y = 0;
    else if (y >= mnHeight)
        y = mnHeight - 1;

    return mpData[(y * mnWidth) + x];
}


Bitmap32::Pixel Bitmap32::GetPixelClamped(int x, int y, Pixel cOutOfBounds) const
{
    if((x >= 0) && (y >= 0) && (x < mnWidth) && (y < mnHeight))
        return mpData[(y * mnWidth) + x];
    return cOutOfBounds;
}


void Bitmap32::Blit(const Pixel* pSourceBitmap, int sourceX, int sourceY, int width, int height, int sourceStride, int destX, int destY)
{
    EA_ASSERT((sourceX >= 0) && (sourceY >= 0) && (destX >= 0) && (destY >= 0) && ((destX + width) <= mnWidth) && ((destY + height) <= mnHeight));

    for(int sx = sourceX, sxEnd = sourceX + width, dx = destX; sx < sxEnd; sx++, dx++)
    {
        for(int sy = sourceY, syEnd = sourceY + height, dy = destY; sy < syEnd; sy++, dy++)
        {
            Pixel c = pSourceBitmap[(sy * (sourceStride / sizeof(Pixel))) + sx];
            mpData[(dy * (mnStride / sizeof(Pixel))) + dx] = c;
        }
    }
}


void Bitmap32::Clear()
{
    Pixel* pDstRow = mpData;

    for(int y = 0; y < mnHeight; y++)
    {
        memset(pDstRow, 0, mnWidth * sizeof(Pixel));
        pDstRow += mnStride / sizeof(Pixel);
    }
}


void Bitmap32::Fill(Pixel c)
{
    for(int y = 0; y < mnHeight; y++)
    {
        for(int x = 0; x < mnWidth; x++)
            mpData[(y * (mnStride / sizeof(Pixel))) + x] = c;
    }
}


void Bitmap32::TransferFrom(Bitmap32* pSourceBitmap)
{
    Shutdown();

    mpData   = pSourceBitmap->mpData;
    mnWidth  = pSourceBitmap->mnWidth;
    mnHeight = pSourceBitmap->mnHeight;
    mnStride = pSourceBitmap->mnStride;

    pSourceBitmap->mpData   = NULL;
    pSourceBitmap->mnWidth  = 0;
    pSourceBitmap->mnHeight = 0;
    pSourceBitmap->mnStride = 0;
}


void Bitmap32::Swap(Bitmap32* pSourceBitmap)
{
    Internal::swap(mpData,   pSourceBitmap->mpData);
    Internal::swap(mnWidth,  pSourceBitmap->mnWidth);
    Internal::swap(mnHeight, pSourceBitmap->mnHeight);
    Internal::swap(mnStride, pSourceBitmap->mnStride);
}


#if EATEXT_DEBUG
    bool Bitmap32::WritePPMFile(const char* pPath, Pixel d)
    {
        // See http://netpbm.sourceforge.net/doc/ppm.html
        // PPM supports alpha somehow, but it's not well documented.

        FILE* const fp = fopen(pPath, "w");

        if(fp)
        {
            fprintf(fp, "P3\n");
            fprintf(fp, "# %s\n", pPath);
            fprintf(fp, "%d %d\n", mnWidth, mnHeight);
            fprintf(fp, "%d\n", 255);

            for(int y = 0; y < mnHeight; y++)
            {
                for(int x = 0; x < mnWidth; x++)
                {
                    const Pixel s = GetPixel(x, y);

                    if((s >> 24) == 0x00) // If source is transparent (which will often be so)...
                    {
                        const int32_t r = ((d >> 16) & 0xff);
                        const int32_t g = ((d >>  8) & 0xff);
                        const int32_t b = ((d >>  0) & 0xff);

                        fprintf(fp, "%03d %03d %03d \t", (int)r, (int)g, (int)b);
                    }
                    else
                    {
                        const float sa = ((s >> 24) & 0xff) / 255.f;
                        const float sr = ((s >> 16) & 0xff) / 255.f;
                        const float sg = ((s >>  8) & 0xff) / 255.f;
                        const float sb = ((s >>  0) & 0xff) / 255.f;

                        const float dr = ((d >> 16) & 0xff) / 255.f;
                        const float dg = ((d >>  8) & 0xff) / 255.f;
                        const float db = ((d >>  0) & 0xff) / 255.f;

                        const int32_t r = (int32_t)(255 * ((dr * (1.f - sa)) + (sr * sa)));
                        const int32_t g = (int32_t)(255 * ((dg * (1.f - sa)) + (sg * sa)));
                        const int32_t b = (int32_t)(255 * ((db * (1.f - sa)) + (sb * sa)));

                        fprintf(fp, "%03d %03d %03d \t", (int)r, (int)g, (int)b);
                    }
                }

                fprintf(fp, "\n");
            }

            fprintf(fp, "\n");
            fclose(fp);

            return true;
        }

        return false;
    }
#endif




///////////////////////////////////////////////////////////////////////////////
// Bitmap8
///////////////////////////////////////////////////////////////////////////////

Bitmap8::Bitmap8(Allocator::ICoreAllocator* pCoreAllocator, int w, int h)
  : mpData(NULL),
    mnWidth(w),
    mnHeight(h),
    mnStride(w),
    mnCapacity(0),
    mpCoreAllocator(pCoreAllocator ? pCoreAllocator : EA::Text::GetAllocator())
{
    Init(w, h);
}


bool Bitmap8::Init(int w, int h)
{
    Shutdown();
    return SetSize(w, h);
}


void Bitmap8::Shutdown()
{
    if(mpData && mpCoreAllocator)
        mpCoreAllocator->Free(mpData);

    mpCoreAllocator = NULL;
    mpData     = NULL;
    mnWidth    = 0;
    mnHeight   = 0;
    mnStride   = 0;
    mnCapacity = 0;
}


bool Bitmap8::SetSize(int w, int h)
{
    EA_ASSERT((w >= 0) && (h >= 0));

    if((w * h) > mnCapacity) // If we need to reallocate...
    {
        if(mpCoreAllocator)
        {
            if(mpData)
                mpCoreAllocator->Free(mpData);

            mnCapacity  = w * h;
            mpData      = (Pixel*)mpCoreAllocator->Alloc(mnCapacity * sizeof(Pixel), EATEXT_ALLOC_PREFIX "Bitmap8", 0);
            EA_ASSERT(mpData != NULL);
        }
    }

    mnWidth  = w;
    mnHeight = h;
    mnStride = w * sizeof(Pixel);

    return (mpData != NULL);
}


void Bitmap8::Acquire(Pixel* pData, int w, int h, int stride)
{
    mpData          = pData;
    mnWidth         = w;
    mnHeight        = h;
    mnStride        = stride;
    mnCapacity      = w * h;
    mpCoreAllocator = NULL;
}


Bitmap8::Pixel Bitmap8::GetPixelClamped(int x, int y, Pixel cOutOfBounds) const
{
    if((x >= 0) && (y >= 0) && (x < mnWidth) && (y < mnHeight))
        return mpData[(y * mnWidth) + x];
    return cOutOfBounds;
}


void Bitmap8::Blit(const Pixel* pSourceBitmap, int sourceX, int sourceY, int width, int height, int sourceStride, int destX, int destY)
{
    EA_ASSERT((sourceX >= 0) && (sourceY >= 0) && (destX >= 0) && (destY >= 0) && ((destX + width) <= mnWidth) && ((destY + height) <= mnHeight));

    const Pixel* pSrcRow     = pSourceBitmap + (sourceY * sourceStride) + sourceX;
    const Pixel* pSrcRowEnd  = pSrcRow + (height * sourceStride);
          Pixel* pDestRow    = mpData + (destY * mnStride) + destX;

    while(pSrcRow < pSrcRowEnd)
    {
        memcpy(pDestRow, pSrcRow, width * sizeof(Pixel));
        pSrcRow += (sourceStride / sizeof(Pixel));
    }
}


void Bitmap8::Blit1BPP(const uint8_t* pSourceBitmap, int sourceX, int sourceY, int width, int height, int sourceStride, int destX, int destY)
{
    (void)sourceX; (void)sourceY;
    EA_ASSERT((sourceX >= 0) && (sourceY >= 0) && (destX >= 0) && (destY >= 0) && ((destX + width) <= mnWidth) && ((destY + height) <= mnHeight));

    const uint8_t* pSrc8Row         = pSourceBitmap;
    const uint8_t* pSrc8RowEnd      = pSourceBitmap + (height * sourceStride);
    uint8_t*       pDest8Row        = mpData + (destY * mnWidth) + destX;

    while(pSrc8Row < pSrc8RowEnd)
    {
        const uint8_t*  pSrc8       = pSrc8Row;
        uint8_t*        pDest8      = pDest8Row;
        const uint8_t*  pDest8End   = pDest8 + width;
        uint32_t        nSourceMask = 0x00000080;

        while(pDest8 < pDest8End)
        {
            if(*pSrc8 & nSourceMask)
                *pDest8 = 255;

            if((nSourceMask >>= 1) == 0)
            {
                ++pSrc8;
                nSourceMask = 0x00000080;
            }

            ++pDest8;
        }

        pSrc8Row  += sourceStride;
        pDest8Row += mnStride;
    }
}


void Bitmap8::Clear()
{
    Pixel* pDstRow = mpData;

    for(int y = 0; y < mnHeight; y++)
    {
        memset(pDstRow, 0, mnWidth * sizeof(Pixel));
        pDstRow += mnStride / sizeof(Pixel);
    }
}


void Bitmap8::Fill(Pixel c)
{
    for(int y = 0; y < mnHeight; y++)
    {
        for(int x = 0; x < mnWidth; x++)
            mpData[(y * (mnStride / sizeof(Pixel))) + x] = c;
    }
}


void Bitmap8::CopyFrom(const Bitmap8* pBitmap8)
{
    EA_ASSERT((mnWidth  == pBitmap8->mnWidth) && 
              (mnHeight == pBitmap8->mnHeight));

    const Pixel* pSrcRow = pBitmap8->mpData;
          Pixel* pDstRow = mpData;

    for(int y = 0; y < mnHeight; y++)
    {
        memcpy(pDstRow, pSrcRow, mnWidth * sizeof(Pixel));
        pSrcRow += pBitmap8->mnStride / sizeof(Pixel);
        pDstRow += mnStride / sizeof(Pixel);
    }
}


#if EATEXT_DEBUG
    bool Bitmap8::WritePPMFile(const char* pPath, Pixel /*d*/)
    {
        // See http://netpbm.sourceforge.net/doc/ppm.html
        // PPM supports alpha somehow, but it's not well documented.

        FILE* const fp = fopen(pPath, "w");

        if(fp)
        {
            fprintf(fp, "P3\n");
            fprintf(fp, "# %s\n", pPath);
            fprintf(fp, "%d %d\n", mnWidth, mnHeight);
            fprintf(fp, "%d\n", 255);

            for(int y = 0; y < mnHeight; y++)
            {
                for(int x = 0; x < mnWidth; x++)
                {
                    const Pixel s = GetPixel(x, y);
                    fprintf(fp, "%03d %03d %03d \t", (int)s, (int)s, (int)s);
                }

                fprintf(fp, "\n");
            }

            fprintf(fp, "\n");
            fclose(fp);

            return true;
        }

        return false;
    }
#endif


#if 0
/*
///////////////////////////////////////////////////////////////////////////////
// ColorARGB8
///////////////////////////////////////////////////////////////////////////////

namespace ColorARGB8
{
    /// Colors are represented as a Pixel
    typedef Pixel color_t;

    //////////////////////////////////////////////////////////////////////////
    /// Pack values A, R, G and B into a 32-bit color
    inline color_t PackARGB( uint8_t a, uint8_t r, uint8_t g, uint8_t b ) {
        return color_t((a << 24) | (r << 16) | (g << 8) | b);
    }

    //////////////////////////////////////////////////////////////////////////
    /// Pack values A, R, G and B into a 32-bit color.
    inline color_t PackARGB( int32_t a, int32_t r, int32_t g, int32_t b ) {
        return color_t(((a & 0x00ff) << 24) | ((r & 0x00ff) << 16) | ((g & 0x00ff) << 8) | (b & 0x00ff));
    }

    //////////////////////////////////////////////////////////////////////////
    /// Pack values A, R, G and B into a 32-bit color
    inline color_t PackARGB( float a, float r, float g, float b ) {
        return PackARGB( int32_t( a * 255.0f ),
                         int32_t( r * 255.0f ),
                         int32_t( g * 255.0f ),
                         int32_t( b * 255.0f ) );
    }

    //////////////////////////////////////////////////////////////////////////
    /// Generate a color from hue, saturation and value
    color_t PackAHSV(float_t alpha, float_t hue, float_t saturation, float_t value);

    //////////////////////////////////////////////////////////////////////////
    /// Unpack just the Alpha component of a color
    inline uint8_t UnpackA( color_t color ) {
        return (uint8_t)(color >> 24);
    }

    //////////////////////////////////////////////////////////////////////////
    /// Unpack just the Red component of a color
    inline uint8_t UnpackR( color_t color ) {
        return (uint8_t)(color >> 16);
    }

    //////////////////////////////////////////////////////////////////////////
    /// Unpack just the Green component of a color
    inline uint8_t UnpackG( color_t color ) {
        return (uint8_t)(color >> 8);
    }

    //////////////////////////////////////////////////////////////////////////
    /// Unpack just the Blue component of a color
    inline uint8_t UnpackB( color_t color ) {
        return (uint8_t)color;
    }

    //////////////////////////////////////////////////////////////////////////
    /// Unpack just the Alpha component of a color
    inline float UnpackAf( color_t color ) {
        return (float)(UnpackA(color) * 1.0f/255.0f);
    }

    //////////////////////////////////////////////////////////////////////////
    /// Unpack just the Red component of a color
    inline float UnpackRf( color_t color ) {
        return (float)(UnpackR(color) * 1.0f/255.0f);
    }

    //////////////////////////////////////////////////////////////////////////
    /// Unpack just the Green component of a color
    inline float UnpackGf( color_t color ) {
        return (float)(UnpackG(color) * 1.0f/255.0f);
    }

    //////////////////////////////////////////////////////////////////////////
    /// Unpack just the Blue component of a color
    inline float UnpackBf( color_t color ) {
        return (float)(UnpackB(color) * 1.0f/255.0f);
    }

    //////////////////////////////////////////////////////////////////////////
    /// Convert ARGB to RGBA
    inline color_t ARGBToRGBA( color_t color ) {
        return (color << 8) | (color >> 24);
    }

    //////////////////////////////////////////////////////////////////////////
    /// Conver RGBA to ARGB
    inline color_t RGBAToARGB( color_t color ) 
    {
        return (color >> 8) | (color << 24);
    }

    //////////////////////////////////////////////////////////////////////////
    // Add
    inline color_t Add( color_t x, color_t y )
    {
        // rgb components are saturated, a wraps around.
        Pixel c = (x & 0xfefefefe) + (y & 0xfefefefe);
        return (c | ((c & 0x01010101) - ((c & 0x01010101) >> 8)));
    }

    //////////////////////////////////////////////////////////////////////////
    // Multiply
    inline color_t Multiply(color_t x, color_t y )
    {
        ////////////////////////////////////////////////////////
        // Reference implementation:
        //
        // Pixel a = (Pixel)(((((x >> 24) & 0xff) / 255.f) * (((y >> 24) & 0xff) / 255.f)) * 255.f);
        // Pixel r = (Pixel)(((((x >> 16) & 0xff) / 255.f) * (((y >> 16) & 0xff) / 255.f)) * 255.f);
        // Pixel g = (Pixel)(((((x >>  8) & 0xff) / 255.f) * (((y >>  8) & 0xff) / 255.f)) * 255.f);
        // Pixel b = (Pixel)(((((x >>  0) & 0xff) / 255.f) * (((y >>  0) & 0xff) / 255.f)) * 255.f);
        // 
        // return (a << 24) | (r << 16) | (g << 8) | b;
        ////////////////////////////////////////////////////////

        // The following could likely be done better with VMX instructions 
        // on processors that support such instructions. 

        Pixel a = ((x >> 24) * (y >> 24)) + 1;   // Multiply x alpha * y alpha
        a = ((a + (a >> 8)) >> 8);                  // Divide by 255

        Pixel r = (((x & 0x00ff0000) >> 16) * (y & 0x00ff0000)) + 0x00010000;
        r = ((r + (r >> 8)) >> 8) & 0x00ff0000;

        Pixel g = (((x & 0x0000ff00) >> 8) * (y & 0x0000ff00)) + 0x00000100;
        g = ((g + (g >> 8)) >> 8) & 0x0000ff00;

        Pixel b = (x & 0xff) * (y & 0xff) + 1;
        b = ((b + (b >> 8)) >> 8);

        return (a << 24) | r | g | b;
    }

    //////////////////////////////////////////////////////////////////////////
    /// Average
    /// Returns the average of the two colors calculated per channel:
    /// ((a.c + b.c) / 2), where [0, 255] is treated as [0,1].
    inline color_t Average( color_t x, color_t y ) 
    {
        Pixel vx = x & 0x00ff00ff;
        Pixel rb = vx + ((((y & 0x00ff00ff) - vx) * 127) >> 8) & 0x00ff00ff;
        vx = x & 0xff00ff00;
        return (rb | (vx + ((((y >> 8) & 0x00ff00ff) - (vx >> 8))) & 0xff00ff00));
    }

    //////////////////////////////////////////////////////////////////////////
    /// Interpolate between two colors, using a floating interpolation parameter
    inline color_t Lerp( color_t x, color_t y, float fParam )
    {
        // To do: Make a more efficient version of this.
        return PackARGB(
            (uint8_t)(UnpackA( x ) + uint8_t(((int32_t)UnpackA( y ) - (int32_t)UnpackA( x )) * fParam)),
            (uint8_t)(UnpackR( x ) + uint8_t(((int32_t)UnpackR( y ) - (int32_t)UnpackR( x )) * fParam)),
            (uint8_t)(UnpackG( x ) + uint8_t(((int32_t)UnpackG( y ) - (int32_t)UnpackG( x )) * fParam)),
            (uint8_t)(UnpackB( x ) + uint8_t(((int32_t)UnpackB( y ) - (int32_t)UnpackB( x )) * fParam)) );
    }

    //////////////////////////////////////////////////////////////////////////
    /// Interpolate between two colors, using a ratio of integers interpolation parameter
    inline color_t Lerp( color_t x, color_t y, int32_t nNumerator, int32_t nDenominator ) 
    {
        // To do: Make a more efficient version of this.
        return PackARGB(
            (uint8_t)(UnpackA( x ) + uint8_t((int32_t)UnpackA( y ) - (int32_t)UnpackA( x )) * nNumerator / nDenominator),
            (uint8_t)(UnpackR( x ) + uint8_t((int32_t)UnpackR( y ) - (int32_t)UnpackR( x )) * nNumerator / nDenominator),
            (uint8_t)(UnpackG( x ) + uint8_t((int32_t)UnpackG( y ) - (int32_t)UnpackG( x )) * nNumerator / nDenominator),
            (uint8_t)(UnpackB( x ) + uint8_t((int32_t)UnpackB( y ) - (int32_t)UnpackB( x )) * nNumerator / nDenominator) );
    }

    //////////////////////////////////////////////////////////////////////////
    /// Combine color x with color y, using x's alpha
    inline color_t AlphaCombine( color_t x, color_t y )
    {
        uint8_t alpha = UnpackA( x );
        return PackARGB(
            UnpackA( y ),
            uint8_t(UnpackR( y ) + ((int32_t)UnpackR( x ) - (int32_t)UnpackR( y )) * alpha / 255),
            uint8_t(UnpackG( y ) + ((int32_t)UnpackG( x ) - (int32_t)UnpackG( y )) * alpha / 255),
            uint8_t(UnpackB( y ) + ((int32_t)UnpackB( x ) - (int32_t)UnpackB( y )) * alpha / 255) );
    }

    //////////////////////////////////////////////////////////////////////////
    /// Return the 'lightness' (as defined by the HLS color space) of this color.
    /// This is defined as the average of the largest and smallest color component.
    inline float Lightness( color_t color ) {
        float r = (float)UnpackR( color ) * (1.0f/255.0f);
        float g = (float)UnpackG( color ) * (1.0f/255.0f);
        float b = (float)UnpackB( color ) * (1.0f/255.0f);
        float maxcomp = r;
        float mincomp = r;

        if (g > maxcomp) maxcomp = g;
        else if (g < mincomp) mincomp = g;

        if (b > maxcomp) maxcomp = b;
        else if (b < mincomp) mincomp = b;

        return (maxcomp - mincomp) * 0.5f;
    }

    //////////////////////////////////////////////////////////////////////////
    /// Lighten a color. Does not affect alpha.
    inline color_t Lighten( color_t color, float fParam ) 
    {
        return Lerp( color, color | 0x00ffffff, fParam );
    }

    //////////////////////////////////////////////////////////////////////////
    /// Darken a color. Does not affect alpha.
    //////////////////////////////////////////////////////////////////////////
    inline color_t Darken( color_t color, float fParam ) 
    {
        return Lerp( color, color & 0xff000000, fParam );
    }

} // namespace ColorARGB8

*/
#endif



} // namespace Text

} // namespace EA
















