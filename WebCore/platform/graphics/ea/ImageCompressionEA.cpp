/*
Copyright (C) 2009, 2010, 2011, 2012, 2013, 2014 Electronic Arts, Inc.  All rights reserved.

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

/*
    RGB_ to CoCg_Y conversion and back.
    Copyright (C) 2007 Id Software, Inc.
    Written by J.M.P. van Waveren
    Copyright (C) 2009, 2012 Electronic Arts, Inc.  All rights reserved.

    This code is free software; you can redistribute it and/or
    modify it under the terms of the GNU Lesser General Public
    License as published by the Free Software Foundation; either
    version 2.1 of the License, or (at your option) any later version.
    
    This code is distributed in the hope that it will be useful,
    but WITHOUT ANY WARRANTY; without even the implied warranty of
    MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
    Lesser General Public License for more details.
*/


#include "config.h"
#if ENABLE(IMAGE_COMPRESSION)
#include "ImageCompressionEA.h"
#include <stdlib.h>
#include <EAWebKit/EAWebKitConfig.h>
#include <EAWebKit/EAWebkitAllocator.h>

namespace WebCore {


static const int kMaxRLETokenSize = 255;    // Max repetition for an RLE token count
static const int kRLEAlphaShift = 24;       // Shift offset to the location of alpha (ARGB has alpha at 24)

// Small header for compression to allow more than one format. 
struct CompressionHeader
{
    enum CompressionType
    {
        kInvalid,
        kRLEVariableAlpha,      // Variable alpha RLE. Pixels can have different alpha values
        kRLEConstantAlpha,      // Single alpha value for the entire texture. 
        kYCOCGDXT5
    };
    
    EA_ALIGN(4) CompressionType mType;      // The format type
    EA_ALIGN(4) int mSize;                  // Compressed size in bytes not including the header (size starts after the header)
    EA_ALIGN(4) int mWidth;                 // Width of source image
    EA_ALIGN(4) int mHeight;                // Height of source image
    EA_ALIGN(4) int mAlphaValue;            // RLE alpha value (already shifted). 

    CompressionHeader(CompressionType type, int size, int w, int h, int aValue = 0)
        : mType(type)
        , mSize(size)
        , mWidth(w)
        , mHeight(h)
        , mAlphaValue(aValue)
    {
    }
};

//--------------------------------------
//--- Start of YCoCgDXT5 Compression ---
//--------------------------------------

// See header on top of this file for license. 
// This code was modified for was missing some Emit functions but have tried to keep it as close as possible to the original version.
// Also removed some alpha handling which was never used and added a few overloaded functions (like ExtractBlock).

/* 
    RGB <-> YCoCg
    
    Y  = [ 1/4  1/2  1/4] [R]
    Co = [ 1/2    0 -1/2] [G]
    CG = [-1/4  1/2 -1/4] [B]
    
    R  = [   1    1   -1] [Y]
    G  = [   1    0    1] [Co]
    B  = [   1   -1   -1] [Cg]
    
*/


// Keeping uchar as "byte" here in order to keep the source code as intact as possible
#ifndef byte
    typedef unsigned char   byte;
#endif

byte CLAMP_BYTE( int x ) { return ( (x) < 0 ? (0) : ( (x) > 255 ? 255 : (x) ) ); }
 
#define RGB_TO_YCOCG_Y( r, g, b )   ( ( (    r +   (g<<1) +  b     ) + 2 ) >> 2 )
#define RGB_TO_YCOCG_CO( r, g, b )  ( ( (   (r<<1)        - (b<<1) ) + 2 ) >> 2 )
#define RGB_TO_YCOCG_CG( r, g, b )  ( ( ( -  r +   (g<<1) -  b     ) + 2 ) >> 2 )
 
#define COCG_TO_R( co, cg )         ( co - cg )
#define COCG_TO_G( co, cg )         ( cg )
#define COCG_TO_B( co, cg )         ( - co - cg )
 
void ConvertRGBToCoCg_Y(byte *pImage, int width, int height, int stride)
{

   byte *pImageRow = pImage;
    
    for (int h = 0; h < height; ++h)
    {
        for (int w = 0; w < width; ++w)
        {
        
            byte *pImageColumn = pImageRow + (w << 2);
#ifdef EA_SYSTEM_LITTLE_ENDIAN
            int r = pImageColumn[0];
            int g = pImageColumn[1];
            int b = pImageColumn[2];
            int a = pImageColumn[3];
#else
            int a = pImageColumn[0];
            int b = pImageColumn[1];
            int g = pImageColumn[2];
            int r = pImageColumn[3];
#endif
           pImageColumn[0] = CLAMP_BYTE( RGB_TO_YCOCG_CO( r, g, b ) + 128 );
           pImageColumn[1] = CLAMP_BYTE( RGB_TO_YCOCG_CG( r, g, b ) + 128 );
           pImageColumn[2] = a;
           pImageColumn[3] = CLAMP_BYTE( RGB_TO_YCOCG_Y( r, g, b ) );
        }
        pImageRow += stride;            // Next line
    }
}

// This function doesn't load alpha from the source.
void ConvertCoCg_YToRGBNoAlpha(byte *pImage, int width, int height, int strideIn) {
    byte *pImageRow = pImage;
    
    for (int h = 0; h < height; ++h)
    {
        for (int w = 0; w < width; ++w)
        {
            
            byte *pImageColumn = pImageRow + (w << 2);

            int co = pImageColumn[0] - 128;
            int cg = pImageColumn[1] - 128;
            int y  = pImageColumn[3];

#ifdef EA_SYSTEM_LITTLE_ENDIAN  
            pImageColumn[0] = CLAMP_BYTE( y + COCG_TO_R( co, cg ) );
            pImageColumn[1] = CLAMP_BYTE( y + COCG_TO_G( co, cg ) );
            pImageColumn[2] = CLAMP_BYTE( y + COCG_TO_B( co, cg ) );
            pImageColumn[3] = 255;
#else
           pImageColumn[0] = 255;
           pImageColumn[1] = CLAMP_BYTE( y + COCG_TO_B( co, cg ) );
           pImageColumn[2] = CLAMP_BYTE( y + COCG_TO_G( co, cg ) );
           pImageColumn[3] = CLAMP_BYTE( y + COCG_TO_R( co, cg ) );
#endif
        }
        pImageRow += strideIn;             // Next line
    }
}

#ifndef word
typedef unsigned short  word;
#endif 

#ifndef dword
typedef unsigned int    dword;
#endif

#define INSET_COLOR_SHIFT       4       // inset color bounding box
#define INSET_ALPHA_SHIFT       5       // inset alpha bounding box
 
#define C565_5_MASK             0xF8    // 0xFF minus last three bits
#define C565_6_MASK             0xFC    // 0xFF minus last two bits
 
#define NVIDIA_G7X_HARDWARE_BUG_FIX     // keep the colors sorted as: max, min
 
byte *globalOutData;
 
ALWAYS_INLINE word ColorTo565( const byte *color )
{
    return ( ( color[ 0 ] >> 3 ) << 11 ) | ( ( color[ 1 ] >> 2 ) << 5 ) | ( color[ 2 ] >> 3 );
}

ALWAYS_INLINE void EmitByte( byte b )
{
    globalOutData[0] = b;
    globalOutData += 1;
}

ALWAYS_INLINE void EmitUInt( unsigned int s )
{
   globalOutData[0] = ( s >>  0 ) & 255;
   globalOutData[1] = ( s >>  8 ) & 255;
   globalOutData[2] = ( s >>  16 ) & 255;
   globalOutData[3] = ( s >>  24 ) & 255;
   globalOutData += 4;
}

ALWAYS_INLINE void EmitUShort( unsigned short s )
{
    globalOutData[0] = ( s >>  0 ) & 255;
    globalOutData[1] = ( s >>  8 ) & 255;
    globalOutData += 2;
}

ALWAYS_INLINE void EmitWord( word s )
{
    globalOutData[0] = ( s >>  0 ) & 255;
    globalOutData[1] = ( s >>  8 ) & 255;
    globalOutData += 2;
}
 
ALWAYS_INLINE void EmitDoubleWord( dword i )
{
    globalOutData[0] = ( i >>  0 ) & 255;
    globalOutData[1] = ( i >>  8 ) & 255;
    globalOutData[2] = ( i >> 16 ) & 255;
    globalOutData[3] = ( i >> 24 ) & 255;
    globalOutData += 4;
}
 
ALWAYS_INLINE void ExtractBlock( const byte *inPtr, const int width, const int stride, byte *colorBlock )
{
    for ( int j = 0; j < 4; ++j )
    {
        memcpy( &colorBlock[j*16], inPtr, 16);   // 16 = 4*4
        inPtr += stride;
    }
}

// This box extract replicates the last rows and columns if the row or columns are not 4 texels aligned
// This is so we don't get random pixels which could affect the color interpolation
void ExtractBlock( const byte *inPtr, const int width, const int stride, const int widthRemain, const int heightRemain, byte *colorBlock )
{
   int *pBlock32 = (int *) colorBlock;  // Since we are using ARGA, we assume 4 byte alignment is already being used
   int *pSource32 = (int*) inPtr; 
  
   int hIndex = 0;
   for (int j = 0; j < 4; ++j)
   {
        int wIndex = 0;
        for (int i = 0; i < 4; ++i)
        {
            pBlock32[i] = pSource32[wIndex];    
            // Set up offset for next column source (keep existing if we are at the end)         
            if (wIndex < (widthRemain - 1))
            {
                wIndex++;
            }
        }

        // Set up offset for next texel row source (keep existing if we are at the end)
        pBlock32 += 4;    
        hIndex++;
        if (hIndex < (heightRemain - 1))
        {
            pSource32 += (stride >> 2);
        }
   }
}

void GetMinMaxYCoCg( byte *colorBlock, byte *minColor, byte *maxColor )
{
    minColor[0] = minColor[1] = minColor[2] = minColor[3] = 255;
    maxColor[0] = maxColor[1] = maxColor[2] = maxColor[3] = 0;

    for ( int i = 0; i < 16; ++i )
    {
        if ( colorBlock[i*4+0] < minColor[0] )
        {
            minColor[0] = colorBlock[i*4+0];
        }
        if ( colorBlock[i*4+1] < minColor[1] )
        {
            minColor[1] = colorBlock[i*4+1];
        }
// Note: the alpha is not used so no point in checking for it
        if ( colorBlock[i*4+3] < minColor[3] )
        {
            minColor[3] = colorBlock[i*4+3];
        }
        if ( colorBlock[i*4+0] > maxColor[0] )
        {
            maxColor[0] = colorBlock[i*4+0];
        }
        if ( colorBlock[i*4+1] > maxColor[1] )
        {
            maxColor[1] = colorBlock[i*4+1];
        }
// Note: the alpha is not used so no point in checking for it
        if ( colorBlock[i*4+3] > maxColor[3] )
        {
            maxColor[3] = colorBlock[i*4+3];
        }
    }
}

// EA/Alex Mole: abs isn't inlined and gets called a *lot* in this code :)
// Let's make us an inlined one!
static ALWAYS_INLINE int absEA( int liArg )
{
    return ( liArg >= 0 ) ? liArg : -liArg;
}


void ScaleYCoCg( byte *colorBlock, byte *minColor, byte *maxColor )
{
    int m0 = absEA( minColor[0] - 128 );      // (the 128 is to center to color to grey (128,128) )
    int m1 = absEA( minColor[1] - 128 );
    int m2 = absEA( maxColor[0] - 128 );
    int m3 = absEA( maxColor[1] - 128 );
    
    if ( m1 > m0 ) m0 = m1;
    if ( m3 > m2 ) m2 = m3;
    if ( m2 > m0 ) m0 = m2;
    
    const int s0 = 128 / 2 - 1;
    const int s1 = 128 / 4 - 1;
    
    int mask0 = -( m0 <= s0 );
    int mask1 = -( m0 <= s1 );
    int scale = 1 + ( 1 & mask0 ) + ( 2 & mask1 );
    
    minColor[0] = ( minColor[0] - 128 ) * scale + 128;
    minColor[1] = ( minColor[1] - 128 ) * scale + 128;
    minColor[2] = ( scale - 1 ) << 3;
    
    maxColor[0] = ( maxColor[0] - 128 ) * scale + 128;
    maxColor[1] = ( maxColor[1] - 128 ) * scale + 128;
    maxColor[2] = ( scale - 1 ) << 3;
    
    for ( int i = 0; i < 16; ++i )
    {
        colorBlock[i*4+0] = ( colorBlock[i*4+0] - 128 ) * scale + 128;
        colorBlock[i*4+1] = ( colorBlock[i*4+1] - 128 ) * scale + 128;
    }
}

void InsetYCoCgBBox( byte *minColor, byte *maxColor )
{
    int inset[4];
    int mini[4];
    int maxi[4];
    
    inset[0] = ( maxColor[0] - minColor[0] ) - ((1<<(INSET_COLOR_SHIFT-1))-1);
    inset[1] = ( maxColor[1] - minColor[1] ) - ((1<<(INSET_COLOR_SHIFT-1))-1);
    inset[3] = ( maxColor[3] - minColor[3] ) - ((1<<(INSET_ALPHA_SHIFT-1))-1);
    
    mini[0] = ( ( minColor[0] << INSET_COLOR_SHIFT ) + inset[0] ) >> INSET_COLOR_SHIFT;
    mini[1] = ( ( minColor[1] << INSET_COLOR_SHIFT ) + inset[1] ) >> INSET_COLOR_SHIFT;
    mini[3] = ( ( minColor[3] << INSET_ALPHA_SHIFT ) + inset[3] ) >> INSET_ALPHA_SHIFT;
    
    maxi[0] = ( ( maxColor[0] << INSET_COLOR_SHIFT ) - inset[0] ) >> INSET_COLOR_SHIFT;
    maxi[1] = ( ( maxColor[1] << INSET_COLOR_SHIFT ) - inset[1] ) >> INSET_COLOR_SHIFT;
    maxi[3] = ( ( maxColor[3] << INSET_ALPHA_SHIFT ) - inset[3] ) >> INSET_ALPHA_SHIFT;
    
    mini[0] = ( mini[0] >= 0 ) ? mini[0] : 0;
    mini[1] = ( mini[1] >= 0 ) ? mini[1] : 0;
    mini[3] = ( mini[3] >= 0 ) ? mini[3] : 0;
    
    maxi[0] = ( maxi[0] <= 255 ) ? maxi[0] : 255;
    maxi[1] = ( maxi[1] <= 255 ) ? maxi[1] : 255;
    maxi[3] = ( maxi[3] <= 255 ) ? maxi[3] : 255;
    
    minColor[0] = ( mini[0] & C565_5_MASK ) | ( mini[0] >> 5 );
    minColor[1] = ( mini[1] & C565_6_MASK ) | ( mini[1] >> 6 );
    minColor[3] = mini[3];
    
    maxColor[0] = ( maxi[0] & C565_5_MASK ) | ( maxi[0] >> 5 );
    maxColor[1] = ( maxi[1] & C565_6_MASK ) | ( maxi[1] >> 6 );
    maxColor[3] = maxi[3];
}
 
void SelectYCoCgDiagonal( const byte *colorBlock, byte *minColor, byte *maxColor )
{
    byte mid0 = ( (int) minColor[0] + maxColor[0] + 1 ) >> 1;
    byte mid1 = ( (int) minColor[1] + maxColor[1] + 1 ) >> 1;
    
    byte side = 0;
    for ( int i = 0; i < 16; i++ )
    {
        byte b0 = colorBlock[i*4+0] >= mid0;
        byte b1 = colorBlock[i*4+1] >= mid1;
        side += ( b0 ^ b1 );
    }
    
    byte mask = -( side > 8 );
    
#ifdef NVIDIA_7X_HARDWARE_BUG_FIX
    mask &= -( minColor[0] != maxColor[0] );
#endif
    
    byte c0 = minColor[1];
    byte c1 = maxColor[1];
    
    // For platform compiler warning fix:    
    // c0 ^= c1 ^= mask &= c0 ^= c1;    // Orignial code
    byte c2 = c0 ^ c1;
    c0 = c2;
    c0 ^= c1 ^= mask &= c2;
    
    minColor[1] = c0;
    maxColor[1] = c1;
}
 
void EmitAlphaIndices( const byte *colorBlock, const byte minAlpha, const byte maxAlpha )
{
    ASSERT( maxAlpha >= minAlpha );
    
    const int ALPHA_RANGE = 7;
    
    byte mid, ab1, ab2, ab3, ab4, ab5, ab6, ab7;
    byte indexes[16];
    
    mid = ( maxAlpha - minAlpha ) / ( 2 * ALPHA_RANGE );
    
    ab1 = minAlpha + mid;
    ab2 = ( 6 * maxAlpha + 1 * minAlpha ) / ALPHA_RANGE + mid;
    ab3 = ( 5 * maxAlpha + 2 * minAlpha ) / ALPHA_RANGE + mid;
    ab4 = ( 4 * maxAlpha + 3 * minAlpha ) / ALPHA_RANGE + mid;
    ab5 = ( 3 * maxAlpha + 4 * minAlpha ) / ALPHA_RANGE + mid;
    ab6 = ( 2 * maxAlpha + 5 * minAlpha ) / ALPHA_RANGE + mid;
    ab7 = ( 1 * maxAlpha + 6 * minAlpha ) / ALPHA_RANGE + mid;
    
    for ( int i = 0; i < 16; ++i )
    {
        byte a = colorBlock[i*4+3]; // Here it seems to be using the Y (luna) for the alpha
        int b1 = ( a <= ab1 );
        int b2 = ( a <= ab2 );
        int b3 = ( a <= ab3 );
        int b4 = ( a <= ab4 );
        int b5 = ( a <= ab5 );
        int b6 = ( a <= ab6 );
        int b7 = ( a <= ab7 );
        int index = ( b1 + b2 + b3 + b4 + b5 + b6 + b7 + 1 ) & 7;
        indexes[i] = index ^ ( 2 > index );
    }
    
    EmitByte( (indexes[ 0] >> 0) | (indexes[ 1] << 3) | (indexes[ 2] << 6) );
    EmitByte( (indexes[ 2] >> 2) | (indexes[ 3] << 1) | (indexes[ 4] << 4) | (indexes[ 5] << 7) );
    EmitByte( (indexes[ 5] >> 1) | (indexes[ 6] << 2) | (indexes[ 7] << 5) );
    
    EmitByte( (indexes[ 8] >> 0) | (indexes[ 9] << 3) | (indexes[10] << 6) );
    EmitByte( (indexes[10] >> 2) | (indexes[11] << 1) | (indexes[12] << 4) | (indexes[13] << 7) );
    EmitByte( (indexes[13] >> 1) | (indexes[14] << 2) | (indexes[15] << 5) );
}
 
void EmitColorIndices( const byte *colorBlock, const byte *minColor, const byte *maxColor )
{
    word colors[4][4];
    unsigned int result = 0;
    
    colors[0][0] = ( maxColor[0] & C565_5_MASK ) | ( maxColor[0] >> 5 );
    colors[0][1] = ( maxColor[1] & C565_6_MASK ) | ( maxColor[1] >> 6 );
    colors[0][2] = ( maxColor[2] & C565_5_MASK ) | ( maxColor[2] >> 5 );
    colors[0][3] = 0;
    colors[1][0] = ( minColor[0] & C565_5_MASK ) | ( minColor[0] >> 5 );
    colors[1][1] = ( minColor[1] & C565_6_MASK ) | ( minColor[1] >> 6 );
    colors[1][2] = ( minColor[2] & C565_5_MASK ) | ( minColor[2] >> 5 );
    colors[1][3] = 0;
    colors[2][0] = ( 2 * colors[0][0] + 1 * colors[1][0] ) / 3;
    colors[2][1] = ( 2 * colors[0][1] + 1 * colors[1][1] ) / 3;
    colors[2][2] = ( 2 * colors[0][2] + 1 * colors[1][2] ) / 3;
    colors[2][3] = 0;
    colors[3][0] = ( 1 * colors[0][0] + 2 * colors[1][0] ) / 3;
    colors[3][1] = ( 1 * colors[0][1] + 2 * colors[1][1] ) / 3;
    colors[3][2] = ( 1 * colors[0][2] + 2 * colors[1][2] ) / 3;
    colors[3][3] = 0;
    
    for ( int i = 15; i >= 0; i-- )
    {
        int c0, c1;
 
        c0 = colorBlock[i*4+0];
        c1 = colorBlock[i*4+1];

        int d0 = absEA( colors[0][0] - c0 ) + absEA( colors[0][1] - c1 );
        int d1 = absEA( colors[1][0] - c0 ) + absEA( colors[1][1] - c1 );
        int d2 = absEA( colors[2][0] - c0 ) + absEA( colors[2][1] - c1 );
        int d3 = absEA( colors[3][0] - c0 ) + absEA( colors[3][1] - c1 );
        
        bool b0 = d0 > d3;
        bool b1 = d1 > d2;
        bool b2 = d0 > d2;
        bool b3 = d1 > d3;
        bool b4 = d2 > d3;
        
        int x0 = b1 & b2;
        int x1 = b0 & b3;
        int x2 = b0 & b4;
        
        int indexFinal =  ( x2 | ( ( x0 | x1 ) << 1 ) ) << ( i << 1 );
        result |= indexFinal;
    }
    
    EmitUInt( result );
}

/*F*************************************************************************************************/
/*!
    \Function    CompressToYCoCgDXT5( const byte *inBuf, byte *outBuf, const int width, const int height, const int stride ) 

    \Description        This is the C version of the YcoCgDXT5.  
                  
                        Input data needs to be converted from ARGB to YCoCg before calling this function.
                        
                        Does not support alpha at all since it uses the alpha channel to store the Y (luma).
                        
                        The output size is 4:1 but will be based on rounded up texture sizes on 4 texel boundaries 
                        So for example if the source texture is 33 x 32, the compressed size will be 36x32. 
                        
                        The DXT5 compresses groups of 4x4 texels into 16 bytes (4:1 saving) 
                        
                        The compressed format:
                        2 bytes of min and max Y luma values (these are used to rebuild an 8 element Luma table)
                        6 bytes of indexes into the luma table
                            3 bits per index so 16 indexes total 
                        2 shorts of min and max color values (these are used to rebuild a 4 element chroma table)
                            5 bits Co
                            6 bits Cg
                            5 bits Scale. The scale can only be 1, 2 or 4. 
                        4 bytes of indexes into the Chroma CocG table 
                            2 bits per index so 16 indexes total

    \Input              const byte *inBuf   Input buffer of the YCoCG textel data
    \Input              const byte *outBuf  Output buffer for the compressed data
    \Input              int width           in source width 
    \Input              int height          in source height
    \Input              int stride          in source in buffer stride in bytes

    \Output             int ouput size

*/
/*************************************************************************************************F*/
int CompressToYCoCgDXT5( const byte *inBuf, byte *outBuf, const int width, const int height , const int stride)
{
    int outputBytes = 0;
    byte block[64];
    byte minColor[4];
    byte maxColor[4];
    
    globalOutData = outBuf;
    int blockLineSize = stride * 4;  // 4 lines per loop

    for ( int j = 0; j < height; j += 4, inBuf +=blockLineSize )
    {
        int heightRemain = height - j;    
        for ( int i = 0; i < width; i += 4 )
        {
  
            // Note: Modified from orignal source so that it can handle the edge blending better with non aligned 4x textures
            int widthRemain = width - i;
            if ((heightRemain < 4) || (widthRemain < 4) )
            {
                ExtractBlock( inBuf + i * 4, width, stride, widthRemain, heightRemain,  block );  
            }
            else
            {
                ExtractBlock( inBuf + i * 4, width, stride, block );
            }
            // A simple min max extract for each color channel including alpha             
            GetMinMaxYCoCg( block, minColor, maxColor );
            ScaleYCoCg( block, minColor, maxColor );    // Sets the scale in the min[2] and max[2] offset
            InsetYCoCgBBox( minColor, maxColor );
            SelectYCoCgDiagonal( block, minColor, maxColor );
            
            EmitByte( maxColor[3] );    // Note: the luma is stored in the alpha channel
            EmitByte( minColor[3] );
  
            EmitAlphaIndices( block, minColor[3], maxColor[3] );
            
            EmitUShort( ColorTo565( maxColor ) );
            EmitUShort( ColorTo565( minColor ) );
            
            EmitColorIndices( block, minColor, maxColor );
        }
    }
    
    outputBytes = globalOutData - outBuf;
    
    return outputBytes;
}

//--- YCoCgDXT5 Decompression ---
static void RestoreLumaAlphaBlock(  const void * pSource, byte * colorBlock)
{
    byte *pS=(unsigned char *) pSource;
    byte luma[8];     

    // Grabbed this standard table building from undxt.cpp UnInterpolatedAlphaBlock() 
    luma[0] = *pS++;                                                      
    luma[1] = *pS++;                                                      
    luma[2] = (byte)((6 * luma[0] + 1 * luma[1] + 3) / 7);    
    luma[3] = (byte)((5 * luma[0] + 2 * luma[1] + 3) / 7);    
    luma[4] = (byte)((4 * luma[0] + 3 * luma[1] + 3) / 7);    
    luma[5] = (byte)((3 * luma[0] + 4 * luma[1] + 3) / 7);    
    luma[6] = (byte)((2 * luma[0] + 5 * luma[1] + 3) / 7);    
    luma[7] = (byte)((1 * luma[0] + 6 * luma[1] + 3) / 7);    
    
    int rawIndexes;
    int raw;
    int colorIndex=3;
    // We have 6 bytes of indexes (3 bits * 16 texels)
    // Easier to process in 2 groups of 8 texels... 
    for (int j=0; j < 2; ++j)
    {
        // Pack the indexes so we can shift out the indexes as a group 
        rawIndexes = *pS++;
        raw = *pS++;
        rawIndexes |= raw << 8;
        raw = *pS++;
        rawIndexes |= raw << 16;  

        // Since we still have to operate on the texels, just store it in a linear array workspace     
        for (int i=0; i < 8; ++i)
        {       
            static const int LUMA_INDEX_FILTER = 0x7;   // To isolate the 3 bit luma index
            
            byte index = (byte)(rawIndexes & LUMA_INDEX_FILTER);
            colorBlock[colorIndex] = luma[index];
            colorIndex += 4;                                            
            rawIndexes  >>= 3;      
        }
    }   
}

// Converts a 5.6.5 short back into 3 bytes 
static ALWAYS_INLINE void Convert565ToColor( const unsigned short value , byte *pOutColor ) 
{
    int c = value >> (5+6);
    pOutColor[0] = c << 3;  // Was a 5 bit so scale back up 
    
    c = value >> 5;
    c &= 0x3f;               // Filter out the top value
    pOutColor[1] = c << 2;  // Was a 6 bit 
    
    c = value & 0x1f;         // Filter out the top values
    pOutColor[2] = c << 3;  // was a 5 bit so scale back up 
}

// Flip around the 2 bytes in a short
static ALWAYS_INLINE short ShortFlipBytes( short raw ) 
{
    return ((raw >> 8) & 0xff) | (raw << 8);
}

static void RestoreChromaBlock( const void * pSource, byte *colorBlock)
{
   unsigned short *pS =(unsigned short *) pSource;
   pS += 4;  // Color info stars after 8 bytes (first 8 is the Y/alpha channel info)
   
   unsigned short rawColor = *pS++;     
#ifndef EA_SYSTEM_LITTLE_ENDIAN  
    rawColor = ShortFlipBytes(rawColor);
 #endif

    byte color[4][4];   // Color workspace 

    // Build the color lookup table 
    // The luma should have already been extracted and sitting at offset[3]
    Convert565ToColor( rawColor , &color[0][0] );     
    rawColor = *pS++;     
#ifndef EA_SYSTEM_LITTLE_ENDIAN  
    rawColor = ShortFlipBytes(rawColor);
#endif
    
    Convert565ToColor( rawColor , &color[1][0] ); 

    
    color[2][0] = (byte) ( (color[0][0] * 0.75f) + (color[1][0] * 0.25f) );
    color[2][1] = (byte) ( (color[0][1] * 0.75f) + (color[1][1] * 0.25f) );
    color[3][0] = (byte) ( (color[0][0] * 0.25f) + (color[1][0] * 0.75f) );
    color[3][1] = (byte) ( (color[0][1] * 0.25f) + (color[1][1] * 0.75f) );
    
    byte scale = ((color[0][2] >> 3) + 1) >> 1; // Adjust for shifts instead of divide

    // Scale back values here so we don't have to do it for all 16 texels
    // Note: This is really only for the software version.  In hardware, the scale would need to be restored during the YCoCg to RGB conversion.
    for (int i=0; i < 4; ++i)
    {
        color[i][0] = ((color[i][0] - 128) >> scale) + 128;
        color[i][1] = ((color[i][1] - 128) >> scale) + 128;
    }
  
    // Rebuild the color block using the indexes (2 bits per texel)
    int rawIndexes;
    int colorIndex = 0;
    
    // We have 2 shorts of indexes (2 bits * 16 texels = 32 bits). (If can confirm 4x alignment, can grab it as a word with single loop) 
    for (int j=0; j < 2; ++j)
    {
         rawIndexes = *pS++;
#ifndef EA_SYSTEM_LITTLE_ENDIAN  
          rawIndexes = ShortFlipBytes(rawIndexes);
#endif
         
         // Since we still have to operate on block, just store it in a linear array workspace     
         for (int i=0; i < 8; ++i)
         {       
             static const int COCG_INDEX_FILTER = 0x3;   // To isolate the 2 bit chroma index

             unsigned char index = (unsigned char)(rawIndexes & COCG_INDEX_FILTER);
             colorBlock[colorIndex] = color[index][0];
             colorBlock[colorIndex+1] = color[index][1];
             colorBlock[colorIndex+2] = 255;  
             colorIndex += 4;                                            
             rawIndexes >>= 2;      
         }
    }   
}

// This stores a 4x4 texel block but can overflow the output rectangle size if it is not 4 texels aligned in size
int ALWAYS_INLINE StoreBlock( const byte *colorBlock, const int stride, byte *outPtr )
{
   
    for ( int j = 0; j < 4; ++j )
    {
        memcpy( (void*) outPtr,&colorBlock[j*16], 16 ); // 16 = 4*4
        outPtr += stride;
    }
    return 64;
}

// This store only the texels that are within the width and height boundaries so does not overflow
int StoreBlock( const byte *colorBlock , const int stride, const int widthRemain, const int heightRemain,  byte *outPtr) 
{
    int outCount =0;
    int width = stride >> 2;    // Convert to int offsets

    int *pBlock32 = (int *) colorBlock;  // Since we are using ARGB, we assume 4 byte alignment is already being used
    int *pOutput32 = (int*) outPtr; 

    int widthMax = 4;
    if (widthRemain < 4) 
    {
        widthMax = widthRemain;
    }

    int heightMax = 4;
    if (heightRemain < 4)
    {
        heightMax = heightRemain;    
    }

    for (int j =0; j < heightMax; ++j)
    {
        for (int i=0; i < widthMax; ++i)
        {
            pOutput32[i] = pBlock32[i];    
            outCount += 4;       
        }
        
        // Set up offset for next texel row source (keep existing if we are at the end)
        pBlock32 += 4;    
        pOutput32 += width;
    }
    return outCount;
}




/*F*************************************************************************************************/
/*!
    \Function    DeCompressYCoCgDXT5( const byte *inBuf, byte *outBuf, const int width, const int height, const int stride ) 

    \Description  Decompression for YCoCgDXT5  
                  Bascially does the reverse order of he compression.  
                  
                  Ouptut data still needs to be converted from YCoCg to ARGB after this function has completed
                  (probably more efficient to convert it inside here but have not done so to stay closer to the orginal
                  sample code and just make it easier to follow).

                  16 bytes get unpacked into a 4x4 texel block (64 bytes output).

                  The compressed format:
                    2 bytes of min and max Y luma values (these are used to rebuild an 8 element Luma table)
                    6 bytes of indexes into the luma table
                        3 bits per index so 16 indexes total 
                    2 shorts of min and max color values (these are used to rebuild a 4 element chroma table)
                        5 bits Co
                        6 bits Cg
                        5 bits Scale. The scale can only be 1, 2 or 4. 
                    4 bytes of indexes into the Chroma CocG table 
                        2 bits per index so 16 indexes total


    \Input          const byte *inBuf
    \Input          byte *outBuf, 
    \Input          const int width
    \input          const int height 
    \input          const int stride for inBuf

    \Output         int size output in bytes
        

    \Version    1.0        01/12/09 Created
                1.1        12/21/09 Alex Mole: removed branches from tight inner loop
                1.2        11/10/10 CSidhall: Added stride for textures with different image and canvas sizes.
*/
/*************************************************************************************************F*/
int DecompressDXT(const byte *inBuf, byte *outBuf, const int width, const int height, const int stride) 
{
    byte colorBlock[64];    // 4x4 texel work space a linear array 
    int outByteCount = 0;
    const byte *pCurInBuffer = inBuf;

    int blockLineSize = stride * 4;  // 4 lines per loop
    for (int j = 0; j < ( height & ~3 ); j += 4, outBuf += blockLineSize)
    {
        int i;
        for (i = 0; i < ( width & ~3 ); i += 4)
        {
            RestoreLumaAlphaBlock(pCurInBuffer, colorBlock);
            RestoreChromaBlock(pCurInBuffer, colorBlock);           
            outByteCount += StoreBlock(colorBlock, stride, outBuf + i * 4);
            pCurInBuffer += 16; // 16 bytes per block of compressed data
        }

        // Do we have some leftover columns?
        if (width & 3)
        {
            int widthRemain = width & 3;

            RestoreLumaAlphaBlock(pCurInBuffer, colorBlock);
            RestoreChromaBlock(pCurInBuffer, colorBlock);
           
            outByteCount += StoreBlock(colorBlock , stride, widthRemain, 4 /* heightRemain >= 4 */, outBuf + i * 4);

            pCurInBuffer += 16; // 16 bytes per block of compressed data
        }
    }

    // Do we have some leftover lines?
    if (height & 3)
    {
        int heightRemain = height & 3;
    
        for (int i = 0; i < width; i += 4)
        {
            RestoreLumaAlphaBlock(pCurInBuffer, colorBlock);
            RestoreChromaBlock(pCurInBuffer, colorBlock);
           
            int widthRemain = width - i;
            outByteCount += StoreBlock(colorBlock , stride, widthRemain, heightRemain,  outBuf + i * 4);

            pCurInBuffer += 16; // 16 bytes per block of compressed data
        }
    }
    return outByteCount;
}

/*F*************************************************************************************************/
/*!
    \Function           CompressDXT()

    \Description        YCoCgDXT5. Does not work for images with alpha.  
                      
    \Input              const void* pIn  Source data to compress
    \Input              const int sourceSize  size of the source to compress
    \Input              int w  source image width to compress
    \Input              int h  source image height to compress
    \Input              int stride  source image line stride                   
    \Input              void* pOut  Where to ouput the data
    \Input              const int outSizeMax  size of the out buffer (include compression header space).  

    \Output             int size in bytes that is output
                        0 if fail.      
*/
/*************************************************************************************************F*/
static int CompressDXT(void* pSource,const int sourceSize, const int width, const int height, const int stride, void* pOut, const int outSize)
{
    // At this point, we have everything we need so can use the old ARGB buffer directly and modify it
	ConvertRGBToCoCg_Y((byte*)pSource, width, height, stride);
    const int headerSize = sizeof(CompressionHeader);
    int outputSize = 0;
    byte* pOutBuffer = (byte*) pOut + headerSize;

    outputSize = CompressToYCoCgDXT5((byte*)pSource, pOutBuffer, width, height, stride); 
    if (outputSize <= 0)
    {
        // Compression failed. This is almost impossible unless the height or width is 0?  
        // Restore colors just in case...
        ConvertCoCg_YToRGBNoAlpha((byte*)pSource, width, height, stride);
        return 0;
    }

    const int usedSize = outputSize + headerSize;

    // Verify that we did not exceed the buffer
    ASSERT( usedSize <= outSize);                 

    // Set up header
    CompressionHeader header(CompressionHeader::kYCOCGDXT5, usedSize, width, height);
    CompressionHeader* pHeader = reinterpret_cast<CompressionHeader*> (pOut);
    *pHeader = header;
    
    return   usedSize;
}

//------------------------------------
//--- End of YCoCgDXT5 Compression ---
//------------------------------------




/*F*************************************************************************************************/
/*!
    \Function           CompressToVariableAlphaRLE

    \Description        Simple RLE that supports variable alpha.  
                        
                        Format: Header
                                1 byte token for repetitions followed by 4 bytes for the texel data.
                        
                        This RLE performs well for simple flat textures but poorly for photo like images.

                        Somes other options possible here to explore if needed
                        A) If we know we have GIF, we could examine building a color table
                        b) Split up the ARGB into 4 channels and RLE each channel. 
                        However this might cause more memory movement when outputing the compressed data.


    \Input              const void* pIn  Source data to compress
    \Input              void* pOut  Where to ouput the data
    \Input              const int sourceSize  size of the source to compress
    \Input              const int outSizeMax  size of the out buffer. This is used to control the 
                        compression rate 
    \Input              int width  source image width to compress
    \Input              int height  source image height to compress
    \Input              int stride  source image line stride        


    \Output             int size in bytes that is output
                        0 if fail.  It will fail if it overflows the pOut buffer size            

*/
/*************************************************************************************************F*/
static int CompressToVariableAlphaRLE(const void* pIn, void* pOut, const int sourceSize, const int outSizeMax, const int width, const int height, const int stride)
{
    static const int kRLECompressedInfoSize = 5;    // 1 bytes for the token count and 4 bytes for the ARGB value to stamp
    
    int* pColumn = (int*) pIn;
    char* pDst = (char*) pOut;    
    char* pRefSouce = (char *) &pColumn[0];
    int refSource = pColumn[0];                 // Init the first ARGB value
    int curSource;
    int count = 0;
    int usedSize = sizeof(CompressionHeader);   // Reserve compression header.
  
    for (int h = 0; h < height; ++h)
    {
        for (int w = 0; w < width; ++w)
        {

            curSource = pColumn[w];            
            
            if (curSource == refSource)
            {
               // Increment the token count since the ARGB value is same as last            
                count++;
            }  
            else
            {
                // End of token so store       
                while (count > 0)
                {
                    int tokenSize;
                    // Store the count                        
                    if (count >  kMaxRLETokenSize)
                    {
                        tokenSize =  kMaxRLETokenSize;
                    }
                    else
                    {
                        tokenSize = count;
                    }    

                    // Check for buffer overflow and easily exit in case of bad compression rate
                    // 5 = 4 bytes ARGB + 1 byte RLE count
                    if ((usedSize + kRLECompressedInfoSize) > outSizeMax)
                    {
                        // Overflow handling:  bigger than what we want so so forget about it.  
                        return 0; 
                    }

                    // Store token and ARGB
                    pDst[usedSize+0] = tokenSize;    
                    pDst[usedSize+1] = pRefSouce[0];                                       
                    pDst[usedSize+2] = pRefSouce[1];       
                    pDst[usedSize+3] = pRefSouce[2];       
                    pDst[usedSize+4] = pRefSouce[3];       
                    usedSize +=  kRLECompressedInfoSize;                
                    count -=  kMaxRLETokenSize;
                }
            
                pRefSouce = (char*) &pColumn[w];
                count = 1;     
                refSource = curSource;
            }
        }
           pColumn += (stride >> 2);
    }

    // Clean up if we have a count remain. Normally just 1.
    while (count > 0)
    {
        int tokenSize;
        // Store the count                        
        if (count >  kMaxRLETokenSize)
        {
            tokenSize =  kMaxRLETokenSize;
        }
        else
        {
            tokenSize = count;
        }    

        // Check for buffer overflow 
        if ((usedSize +  kRLECompressedInfoSize)  > outSizeMax)
        {
           return 0; 
        }

        // Store token and ARGB
        pDst[usedSize+0] = tokenSize;    
        pDst[usedSize+1] = pRefSouce[0];                                       
        pDst[usedSize+2] = pRefSouce[1];       
        pDst[usedSize+3] = pRefSouce[2];       
        pDst[usedSize+4] = pRefSouce[3];       
        usedSize +=  kRLECompressedInfoSize;                
        count -=  kMaxRLETokenSize;
    }
 
    // Add the RLE header with the updated size info
    CompressionHeader header(CompressionHeader::kRLEVariableAlpha, usedSize, width, height);
    CompressionHeader* pHeader = reinterpret_cast<CompressionHeader*> (pDst);
    *pHeader = header;

    return usedSize;
}

/*F*************************************************************************************************/
/*!
    \Function           CompressToConstantAlphaRLE()

    \Description        Simple RLE that does not support variable alpha (will use the first alpha for
                        the entire image).
                        By using a constant alpha, the source is reduce already by 75% in since we don't
                        need to store the alpha channel, just a single fill alpha value.
                        
                        Format: Header followed by the data.
                                4 bytes for a pixel where 1 byte for the repetition token is stored 
                                in the alpha field. RGB are left alone. 
                        
                        This RLE performs well for simple flat textures but poorly for photo like images.

    \Input              const void* pIn  Source data to compress
    \Input              void* pOut  Where to ouput the data
    \Input              const int sourceSize  size of the source to compress
    \Input              const int outSizeMax  size of the out buffer. This is used to control the 
                        compression rate 
    \Input              int width  source image width to compress
    \Input              int height source image height to compress
    \Input              int stride  source image line stride        


    \Output             int size in bytes that is output
                        0 if fail.  It will fail if it overflows the pOut buffer size            

*/
/*************************************************************************************************F*/
static int CompressToConstantAlphaRLE(const void* pIn, void* pOut, const int sourceSize, const int outSizeMax, const int width, const int height, const int stride)
{
    int* pColumn = (int*) pIn;
    int colorRef = pColumn[0];                  // First ARGB value
    const int alphaMask = 0xff << kRLEAlphaShift; 
    const int alphaRef = colorRef & alphaMask;  // Get the alpha value (hopefully the same for the full image)
    const int colorMask = ~(alphaMask);
    int usedSize = sizeof(CompressionHeader);     // Reserve size for header
    int colorCount = 0;      
    int colorSource;

    int* pDst = (int*) pOut + (usedSize >> 2);   // Adjust for header

    for (int h = 0; h < height; ++h)
    {
        for (int w = 0; w < width; ++w)
        {

            colorSource = pColumn[w];            
            if (colorSource == colorRef)
            {
               // Build the token count            
                colorCount++;
            }  
            else
            {
                // Store the color toke but clip it to char size       
                while (colorCount > 0)
                {
                    // Check for buffer overflow
                    int expectedSize = usedSize + sizeof(int);
                    if (expectedSize > outSizeMax)
                    {
                       // Overflow handling: this RLE is bigger than our target size.  
                       return 0; 
                    }
                    // Store the count but keep it under 255     
                    int tokenCount;
                    if (colorCount >  kMaxRLETokenSize)
                    {
                        tokenCount =  kMaxRLETokenSize;
                    }
                    else
                    {
                        tokenCount = colorCount;
                    }    
                    
                    // Store the token count in the alpha 
                    colorRef &= colorMask;
                    tokenCount <<= kRLEAlphaShift;
                    colorRef |= tokenCount;
                    *pDst = colorRef;    // Store RGB + token as alpha
                    pDst++;
                    usedSize += sizeof(int); 
                    colorCount -= kMaxRLETokenSize;          
                }
            
                // Set up the next token
                colorCount = 1;     // 1 since we know the next one is different already 
                colorRef = colorSource; 
            }
        }
           pColumn += (stride >> 2);
    }
    //Last loop for any remainder (a least 1 token expected)   
    while (colorCount > 0)
    {
        // check for buffer overflow
        int expectedSize = usedSize + sizeof(int);
        if (expectedSize > outSizeMax)
        {
           // Overflow handling: this RLE is bigger than our expect out size.  
           return 0; 
        }

        // Store the count but keep it under 255     
        int tokenCount;
        if (colorCount >  kMaxRLETokenSize)
        {
            tokenCount =  kMaxRLETokenSize;
        }
        else
        {
            tokenCount = colorCount;
        }    
        
        // Store the token count in the alpha 
        colorRef &= colorMask;
        tokenCount <<= kRLEAlphaShift;
        colorRef |= tokenCount;
        *pDst = colorRef;    // Store RGB + token as alpha
        pDst++;
        usedSize += sizeof(int);
        colorCount -= kMaxRLETokenSize;          
    }

    // Update the RLE header at the end
    CompressionHeader header(CompressionHeader::kRLEConstantAlpha, usedSize, width, height, alphaRef);
    CompressionHeader* pHeader = reinterpret_cast<CompressionHeader*> (pOut);
    *pHeader = header;
    
    return usedSize;
}

/*F*************************************************************************************************/
/*!
    \Function           CompressRLE()

    \Description        Simple RLE.  Error checks the params and selects which RLE to call 
                        (variable alpha or constant alpha = solid)
                      
    \Input              const void* pIn  Source data to compress
    \Input              const int sourceSize  size of the source to compress
    \Input              int w  source image width to compress
    \Input              int h  source image height to compress
    \Input              int stride  source image line stride                   
    \Input              bool hasAlpha   Needed to determine which RLE to use
    \Input              void* pOut  Where to ouput the data
    \Input              const int outSizeMax  size of the out buffer. This is used to control the 
                        compression rate 

    \Output             int size in bytes that is output
                        0 if fail.  It will fail if it overflows the pOut buffer size            
*/
/*************************************************************************************************F*/
static int CompressRLE(const void* pSource,const int sourceSize, const int w, const int h, const int stride, const bool hasAlpha, void* pOut, const int outSizeMax)
{
    static const int kRLEMinSize = 64;   // Not worth even considering if under this size in bytes

    if ((pOut == NULL) || (pSource == NULL) || (sourceSize <= 0) || (outSizeMax <= 0))
    {
        ASSERT(0);
        return 0;
    }

    if ((sourceSize <= kRLEMinSize) || (w <= 1) || (h <= 1))
    {
        // Texture is too small for RLE to be effective.    
        return 0;
    }
    
    // Switch between variable alpha or and constant alpha version (constant being more efficient since no alpha channel needed).
    if (!hasAlpha) 
        return CompressToConstantAlphaRLE(pSource, pOut, sourceSize, outSizeMax, w, h, stride);
    else
        return CompressToVariableAlphaRLE(pSource, pOut, sourceSize, outSizeMax, w, h, stride);
}

// Release the buffer which has the compressed image.
void ImageCompressionReleaseBuffer(void* pBuffer)
{
    EA::WebKit::Allocator* pAllocator = EA::WebKit::GetAllocator();
    pAllocator->Free(pBuffer, 0);
}

// Release the decompressed ARGB buffer used by the surface.
void ImageCompressionReleaseNativeSurfaceBuffer(void* pBuffer)
{
   delete [] (unsigned char*) pBuffer;
}

NativeImagePtr  DecompressImage(void* pIn)
{
    if (!pIn)
        return 0;
    
    int outSize = 0;
    int curIndex = 0; 
    int curRow = 1;

    CompressionHeader *pHeader = reinterpret_cast<CompressionHeader*> (pIn);
    const int w = pHeader->mWidth;
    const int h = pHeader->mHeight;
    const int stride = w * 4; 
    const int curStride = (stride >> 2);    

    // Allocate the out buffer
    const int bufferSize = stride * h;
    unsigned char* pOut = new unsigned char[bufferSize];
    if (!pOut)
        return NULL;

    switch (pHeader->mType)
    {
        case CompressionHeader::kRLEVariableAlpha:
        {
            const int loopCount = pHeader->mSize - sizeof(CompressionHeader);
            const int maxBufferSize = bufferSize >> 2;      

            int* pDst = (int*) pOut;    
            char* pCompressed = (char*) (pHeader + 1);    

            for (int i = 0; i < loopCount; i += 5) {
                // Byte loads because does not check alignment   
                unsigned char token  = pCompressed[i+0];  
#ifdef EA_SYSTEM_LITTLE_ENDIAN
                unsigned char a =  pCompressed[i + 1];   
                unsigned char r =  pCompressed[i + 2]; 
                unsigned char g =  pCompressed[i + 3];   
                unsigned char b =  pCompressed[i + 4];   
#else
                unsigned char b =  pCompressed[i + 1];   
                unsigned char g =  pCompressed[i + 2]; 
                unsigned char r =  pCompressed[i + 3];   
                unsigned char a =  pCompressed[i + 4];   
#endif
                // Pack it back to ARGB
                int colorValue = ((b << 24) | (g << 16) | (r << 8) | a);
                
                int sizeCheck = (token >> 2) + outSize; 
                if (sizeCheck > maxBufferSize)
                {
                    ASSERT(0);                
                    return 0;
                }

                // Store raw value back
                while (token-- > 0)
                {
                    // Consider limiting this checking with if the token is within overflow range instead but none of this
                    // is optimized anyway...
                    if (curIndex >= w)
                    {
                        pDst += curStride;
                        curIndex = 0;
                        curRow++;    
                        ASSERT(curRow <= h);        // A memory overflow might occur
                    }
                    
                    pDst[curIndex] = colorValue;
                    curIndex++;
                    outSize++;
                }    
            }
            outSize <<= 2;   
        }
        break;

        case CompressionHeader::kRLEConstantAlpha:
        {
            // The token count is inside the alpha value

            const int maxBufferSize = bufferSize >> 2;
            const int loopCount = (pHeader->mSize - sizeof(CompressionHeader)) >> 2;
            const int alphaMask = 0xff << kRLEAlphaShift; 
            const int alphaValue = pHeader->mAlphaValue;
            const int colorMask = ~(alphaMask);

            int* pDst = (int*) pOut;    
            int *pCompressed = (int *) (pHeader + 1);
            unsigned int token;
            int colorValue;

            for (int i = 0; i < loopCount; ++i)
            {
                colorValue = pCompressed[i];
                               
                token = ((unsigned int) (colorValue & alphaMask)) >> kRLEAlphaShift;   
                colorValue &= colorMask;
                colorValue |= alphaValue;

                // Probably not needed but an extra overflow check here in case of corrupt data
                int sizeCheck = (token >> 2) + outSize; 
                if (sizeCheck > maxBufferSize)
                {
                    ASSERT(0);           
                    return 0;
                }

                // Store raw value back
                while (token-- > 0)
                {
                    // Consider limiting this checking with if the token is within overflow range instead but none of this
                    // is optimized anyway...
                    if (curIndex >= w)
                    {
                        pDst += curStride;
                        curIndex = 0;
                        curRow++;    
                        ASSERT(curRow <= h);        // A memory overflow might occur
                    }
                    
                    pDst[curIndex] = colorValue;
                    curIndex++;
                    outSize++;
                }        
            }
            outSize <<= 2;         
        }
        break;
    

        case CompressionHeader::kYCOCGDXT5:
        {
            char* pCompressed = (char*) (pHeader + 1);    

            outSize = DecompressDXT((byte*) pCompressed, (byte*) pOut, w, h, stride); 
            ConvertCoCg_YToRGBNoAlpha((byte*)pOut, w, h, stride);
        }
        break;

        default:
            ASSERT(0);      // Unknown compression format           
            break;
    }

    // Create the Cairo surface.
    cairo_surface_t* pImage = cairo_image_surface_create_for_data(pOut,CAIRO_FORMAT_ARGB32, w, h, stride);
    
    // By attaching the buffer as user data, we get a notification when the surface is destroyed.     
    cairo_status_t status = cairo_surface_set_user_data(pImage,(cairo_user_data_key_t*) ImageCompressionGetUserDataKey(), pOut, ImageCompressionReleaseNativeSurfaceBuffer);
    (void) status;
    ASSERT (status == CAIRO_STATUS_SUCCESS); // If this fails, the surface data buffer will leak.
    
    return pImage;
}

void* CompressImage(NativeImagePtr pImage, bool hasAlpha, int& outSize)
{
    // Verify image format.  We only support ARGB.
    cairo_format_t format = cairo_image_surface_get_format (pImage.get());
    if (format != CAIRO_FORMAT_ARGB32)
    {
        ASSERT(0);
        return 0;
    }
    void* pSource = cairo_image_surface_get_data(pImage.get());
    ASSERT(pSource);   

    int width = cairo_image_surface_get_width(pImage.get());
    int height = cairo_image_surface_get_height(pImage.get());
    int stride = cairo_image_surface_get_stride (pImage.get());
    const int sourceSize = width * height * 4;                                  
    const int headerSize = sizeof(CompressionHeader);

    // Set up the compressed buffer.  The size depends on if we have alpha or not.
    // If we don't have alpha, we know we can compress DXT at 4:1 min so require RLE to have that same min target.
    // If we have alpha, then only RLE will work so we can use a lower compression ratio.
    int destSize = 0; 
    if (!hasAlpha)
    {
        int widthAlign = (width + 3) & 0xfffffffc;   // For DXT, we need aligned width and height by 4.
        int heightAlign = (height + 3) & 0xfffffffc;
        destSize = (heightAlign * widthAlign) + headerSize;   // 4:1 + header
    } 
    else
    {
        
        const float kRLECompressionRatio = 0.85f;   // Allow up to 85% of the original buffer size - 
        
        destSize = (int) (sourceSize * kRLECompressionRatio) + headerSize;
    } 

    // Set up the buffer
    EA::WebKit::Allocator* pAllocator = EA::WebKit::GetAllocator();
    
    void* pDest = pAllocator->Malloc(destSize, 0, 0);
    if (!pDest)   
        return 0;   

    // RLE compression
    int compressedSize = CompressRLE(pSource, sourceSize, width, height, stride, hasAlpha, pDest, destSize);
    if (compressedSize)
    {
        // Resize to compressed size
        pAllocator->Realloc(pDest, compressedSize, 0);
        return pDest;
    }
   
    // DXT compression
    // We can use the same buffer directly since YCOCGDXT5 is fixed 4: 1 + header.
    if (!hasAlpha)
    {
        compressedSize = CompressDXT(pSource, sourceSize, width, height, stride, pDest, destSize);
        outSize = compressedSize;
        return pDest;
    }

    // Failed (this can happen if the RLE failed and the image had alpha so DXT was not used)
    if (!compressedSize)
    {
        // Failed to RLE within the memory budget        
        pAllocator->Free(pDest, 0);
        outSize = 0;
        pDest = 0;
    }
    return pDest;
}


 void* ImageCompressionGetUserDataKey(void)
 {
    static cairo_user_data_key_t sKey;
    return &sKey;
 }


} // WebCore

#endif
