/*
 * Copyright (C) 2006 Apple Computer, Inc.  All rights reserved.
 * Copyright (C) 2011, 2012, 2013, 2014 Electronic Arts, Inc. All rights reserved.
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
 * THIS SOFTWARE IS PROVIDED BY APPLE COMPUTER, INC. ``AS IS'' AND ANY
 * EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE
 * IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR
 * PURPOSE ARE DISCLAIMED.  IN NO EVENT SHALL APPLE COMPUTER, INC. OR
 * CONTRIBUTORS BE LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL,
 * EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT LIMITED TO,
 * PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES; LOSS OF USE, DATA, OR
 * PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND ON ANY THEORY
 * OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT
 * (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE
 * OF THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
 */

#include "config.h"
#include "Font.h"
#include "GraphicsContext.h"
#include "GlyphBuffer.h"
#include "SimpleFontData.h"
#include "WidthIterator.h"
#include <cairo/cairo.h>
#include <EAWebKit/EAWebKit.h>
#include <EAWebKit/EAWebkitAllocator.h>
#include <EAWebKit/EAWebKitTextInterface.h>
#include <internal/include/EAWebKitAssert.h>
#include <EASTL/fixed_vector.h>
#include <internal/include/EAWebkit_p.h>
#include <wtf/MathExtras.h>
#include "PlatformContextCairo.h"
#include "TextRun.h"
#include "ShadowBlur.h"

namespace WebCore {

void Font::drawEmphasisMarksForComplexText(GraphicsContext*, const TextRun&, const AtomicString& mark, const FloatPoint&, int from, int to) const
{
    // Not implemented.
}

bool Font::canExpandAroundIdeographsInComplexText(void)
{
    // Not implemented.
    return false;
}

EA_FORCE_INLINE uint32_t DivideBy255Rounded(uint32_t a)
{
    a += 128;
    return (a + (a >> 8)) >> 8;
}

EA_FORCE_INLINE uint32_t DividePairBy255Rounded(uint32_t a)
{
    a += 0x00800080;
    a = (a + ((a >> 8) & 0x00ff00ff)) >> 8;
    return (a & 0x00ff00ff);
}

// Multiply all the components of a color (including alpha) by
// an alpha value
EA_FORCE_INLINE uint32_t MultiplyColorAlpha(uint32_t color, uint32_t alpha)
{
    // Pack data into pairs to decrease the number of calculations
    uint32_t srcAlphaGreen = (color & 0xff00ff00) >> 8;
    uint32_t srcRedBlue = (color & 0x00ff00ff);

    // Multiply the color parts with alpha
    uint32_t dstAlphaGreen = DividePairBy255Rounded(srcAlphaGreen * alpha);
    uint32_t dstRedBlue = DividePairBy255Rounded(srcRedBlue * alpha);

    return (dstAlphaGreen << 8) | dstRedBlue;
}

typedef eastl::fixed_vector<EA::WebKit::GlyphDrawInfo, 500, true, EA::WebKit::EASTLAllocator> GlyphDrawInfoArray;


// If USE_FONT_COLOR_TABLE defined, it builds runtime color tables with pre-multiplied colors and allows alpha filtration.  
// This avoids having to pre multiply the pen color for every glyph pixel that we draw. 
// Also allows to set up an optional alpha filter to remove some of the smoothing side effects, especially with darker pen colors.
#define USE_FONT_COLOR_TABLE 1
#ifdef USE_FONT_COLOR_TABLE

// A curve table to filter down the existing alpha values. This is mostly to weaken the smoothing effect. 
// This was extracted from the EAText J curve effect with a few manual tweaks in the higher values.
const uint8_t kAlphaTable[256] = {
    0x0, 0x0, 0x0, 0x0, 0x0, 0x0, 0x1, 0x1, 0x1, 0x1, 0x1, 0x2, 0x2, 0x2, 0x2, 0x2,
    0x3, 0x3, 0x3, 0x3, 0x3, 0x3, 0x4, 0x4, 0x4, 0x4, 0x4, 0x5, 0x5, 0x5, 0x5, 0x5,
    0x6, 0x6, 0x6, 0x7, 0x7, 0x7, 0x8, 0x8, 0x9, 0x9, 0x9, 0xa, 0xa, 0xa, 0xb, 0xb,
    0xc, 0xc, 0xd, 0xe, 0xf, 0xf, 0x10, 0x11, 0x12, 0x12, 0x13, 0x14, 0x15, 0x15, 0x16, 0x17,
    0x18, 0x18, 0x19, 0x1a, 0x1b, 0x1c, 0x1d, 0x1e, 0x1f, 0x1f, 0x20, 0x21, 0x22, 0x23, 0x24, 0x25,
    0x26, 0x27, 0x28, 0x29, 0x2a, 0x2b, 0x2c, 0x2d, 0x2f, 0x30, 0x31, 0x32, 0x33, 0x34, 0x35, 0x36,
    0x38, 0x39, 0x3a, 0x3b, 0x3d, 0x3e, 0x3f, 0x40, 0x42, 0x43, 0x44, 0x45, 0x47, 0x48, 0x49, 0x4a,
    0x4c, 0x4d, 0x4e, 0x4f, 0x50, 0x51, 0x52, 0x53, 0x55, 0x56, 0x57, 0x58, 0x59, 0x5a, 0x5b, 0x5c,
    0x5e, 0x5f, 0x60, 0x61, 0x62, 0x63, 0x64, 0x65, 0x67, 0x68, 0x69, 0x6a, 0x6b, 0x6c, 0x6d, 0x6e,
    0x70, 0x71, 0x73, 0x74, 0x76, 0x77, 0x79, 0x7a, 0x7c, 0x7d, 0x7f, 0x80, 0x82, 0x83, 0x85, 0x86,
    0x88, 0x89, 0x8a, 0x8b, 0x8c, 0x8d, 0x8e, 0x8f, 0x91, 0x92, 0x93, 0x94, 0x95, 0x96, 0x97, 0x98,
    0x9a, 0x9b, 0x9c, 0x9e, 0x9f, 0xa0, 0xa2, 0xa3, 0xa5, 0xa6, 0xa7, 0xa9, 0xaa, 0xab, 0xad, 0xae,
    0xb0, 0xb1, 0xb2, 0xb3, 0xb4, 0xb5, 0xb6, 0xb7, 0xb9, 0xba, 0xbb, 0xbc, 0xbd, 0xbe, 0xbf, 0xc0,
    0xc2, 0xc3, 0xc4, 0xc6, 0xc7, 0xc8, 0xca, 0xcb, 0xcd, 0xce, 0xcf, 0xd1, 0xd2, 0xd3, 0xd5, 0xd6,
    0xd8, 0xd9, 0xda, 0xdb, 0xdc, 0xdd, 0xdf, 0xe0, 0xe1, 0xe2, 0xe3, 0xe5, 0xe6, 0xe7, 0xe8, 0xe9,
    0xeb, 0xec, 0xed, 0xee, 0xf0, 0xf1, 0xf2, 0xf3, 0xf5, 0xf6, 0xf7, 0xf9, 0xfb, 0xfc, 0xff, 0xff}; 

static const uint32_t* GetColorTable(uint32_t penRGB)
{
    struct AlphaTable
    {
        uint32_t mColor;            // The color to be pre-multiplied. Should only be RGB value without alpha.
        uint32_t mTable[256];

        AlphaTable()
            : mColor(0xffffffff)    // Init with alpha since alpha should never be set so we can use this to detect init.
        {
        }
    };  
    
    // Keep table as static so we can reuse table if pen color is the same between text runs (often the case).
    // We could consider using a hash map instead but it would increase memory a bit. 
    static AlphaTable sAlphaTable;  // 1K table + size of AlphaTable 
    
    if (penRGB == sAlphaTable.mColor)
    {
        // Found. 
        return  sAlphaTable.mTable;
    }
    
    // Store the new pen color    
    sAlphaTable.mColor = penRGB;
 
    // Find if should use filter
    bool useFilter = false;
    const EA::WebKit::Parameters& params = EA::WebKit::GetParameters();
    if (params.mEnableFontAlphaFilter) 
    {
        const uint32_t intensity = params.mFontFilterColorIntensity; // Brighter colors don't filter as well.         
             
        // Filter if one of the channels is dark enough.
        if ( ((penRGB & 0xff) <= intensity) ||
             ((penRGB >> 16) <=  intensity) ||
             (((penRGB >> 8) & 0xff) <=  intensity) )
        {
            useFilter = true;
        }
    }
    
    sAlphaTable.mTable[0] = 0;  // 0 alpha is always 0 rgb
    if (useFilter)
    {
        for (int a = 1; a < 256; ++a)
        {
            uint32_t alpha = (uint32_t) kAlphaTable[a];         
            uint32_t rgb = MultiplyColorAlpha(penRGB, alpha);  
            sAlphaTable.mTable[a] = (alpha << 24 ) | rgb;
        }
    }
    else
    {
        for (int a = 1; a < 256; ++a)
        {
            uint32_t rgb = MultiplyColorAlpha(penRGB, a);  
            sAlphaTable.mTable[a] = (a << 24 ) | rgb;
        }
    }
    return sAlphaTable.mTable;
}
#endif

static bool drawGlyphsShadow(GraphicsContext* graphicsContext, float x, float y, float w, float h, cairo_surface_t *surface)
{
    
    ShadowBlur& shadow = graphicsContext->platformContext()->shadowBlur();
    if (!(graphicsContext->textDrawingMode() & TextModeFill) || shadow.type() == ShadowBlur::NoShadow)
        return false;
    
    FloatRect destRect(x, y, w, h);         
    if (GraphicsContext* shadowContext = shadow.beginShadowLayer(graphicsContext, destRect))
    {
        cairo_set_source_surface(shadowContext->platformContext()->cr(), surface, x, y);
        cairo_paint(shadowContext->platformContext()->cr());
        shadow.endShadowLayer(graphicsContext);
    }
    return true;  
}

static void drawGlyphsToContext(GraphicsContext* pGraphicsContext, const EA::WebKit::GlyphDrawInfo *glyphs, const int glyphCount, int srcW, int srcH, float x, float y, float xMin, float yMin)
{
    // Set up the intermediate 32bit draw surface.
    eastl::fixed_vector<uint32_t, 500, true, EA::WebKit::EASTLAllocator> glyphRGBABuffer;
    const uint32_t dataBufferSize = srcW * srcH;
    glyphRGBABuffer.reserve(dataBufferSize);
    memset(glyphRGBABuffer.data(), 0, dataBufferSize * sizeof(uint32_t));

    // Set up the pen color.
    const Color penColor = pGraphicsContext->fillColor();
    uint32_t  penC    = penColor.rgb();
    uint32_t  penA    = (penC >> 24);
    uint32_t  penRGB  = (penC & 0x00ffffff);
   
    #ifdef USE_FONT_COLOR_TABLE
    const uint32_t* pColorTable = GetColorTable(penRGB); 
    #endif

    for (int i = 0; i < glyphCount; ++i)
    {
        const EA::WebKit::GlyphDrawInfo& gdi    = glyphs[i];
        EAW_ASSERT(gdi.mpData);    // These should have been filtered already
        
        const int textureSize = (int)gdi.mSize;   
        const int yOffset = (srcH + (int) yMin) - (int)gdi.y1;

        const int bufferIndex = (yOffset * srcW) + (int)(gdi.x1 - xMin);
        EAW_ASSERT_FORMATTED(bufferIndex >= 0, "Buffer Index is negative. This would corrupt memory. yOffset:%d,destWidth:%u,gdi.x1:%d",yOffset,srcW,(int)gdi.x1);
        uint32_t*            pDestColor  = glyphRGBABuffer.data() + bufferIndex;

        const int            glyphWidth  = (int) (gdi.x2 - gdi.x1);
        const int            glyphHeight = (int) (gdi.y1 - gdi.y2);			

        const int tx = (int)(gdi.mU0 * textureSize);
        const int ty = (int)(gdi.mV0 * textureSize);

        // We need to check what bit format was are in here.
        if ((gdi.mFormat == EA::WebKit::kBFARGB) || (gdi.mFormat == EA::WebKit::kBFARGB_FLIP))     
        {
            const uint32_t stride = gdi.mStride >> 2; // >> 2 for 32 bits           
            const uint32_t* pGlyphAlpha = (uint32_t*) ( gdi.mpData + (ty * gdi.mStride) + (tx << 2) );
            const uint32_t alphaShift = (gdi.mFormat  == EA::WebKit::kBFARGB_FLIP ? 0 : 24);     // To shift needed to bring alpha in first byte.

            // Normal 32 bit render using the pen color
            for (int y = 0; y < glyphHeight; ++y)
            {
                for (int x = 0; x < glyphWidth; ++x)
                {
                    uint32_t surfaceAlpha = pDestColor[x] >> 24; 
                    uint32_t glyphColor = pGlyphAlpha[x]; 

                    // ARGB here
                    uint32_t glyphAlpha = glyphColor >> alphaShift; 
                    glyphAlpha &= 0xff;    // Clean out in case of 0 shift case 

                    uint32_t destAlpha = DivideBy255Rounded(penA * glyphAlpha) | surfaceAlpha;
                        
                    // Skip this pixel if alpha is 0.
                    if (destAlpha)
                    {
                        // Uncomment to bounds check.
                        //if (pDestColor + x > glyphRGBABuffer.data() + dataBufferSize)
                        //{
                        //    __debugbreak();
                        //}

                        // Get the premultiplied alpha from table. Alpha needs to be within 0xff;
                        #ifdef USE_FONT_COLOR_TABLE
                            pDestColor[x] = pColorTable[destAlpha];
                        #else
                            uint32_t destRGB = MultiplyColorAlpha(penRGB, destAlpha);
                            pDestColor[x] = (destAlpha << 24 ) | destRGB;
                        #endif
                    }
                }
                pDestColor  += srcW;
                pGlyphAlpha += stride;   
            }
        }
        else if(gdi.mFormat == EA::WebKit::kBFGrayscale)              
        {
            // This is for the more compact 8bit format. Just passes down the alpha and the pen color provides the RGB
            const uint32_t stride = gdi.mStride;
            const uint8_t* pGlyphAlpha = (gdi.mpData) + (ty * stride) + tx;

            for (int y = 0; y < glyphHeight; ++y)
            {
                for (int x = 0; x < glyphWidth; ++x)
                {
                    //+ 2/23/10 YChin - We have switched to using premultiplied colors, so we need to multiply the alpha onto the final color so the Blt
                    uint32_t destAlpha = DivideBy255Rounded(penA * pGlyphAlpha[x]) | (pDestColor[x] >> 24);
                   
                    // Skip this pixel if alpha is 0.
                    if (destAlpha)
                    {
                        // Get the premultiplied alpha from table. Alpha needs to be within 0xff;
                        #ifdef USE_FONT_COLOR_TABLE
                            pDestColor[x] = pColorTable[destAlpha];
                        #else
                            uint32_t destRGB = MultiplyColorAlpha(penRGB, destAlpha);
                            pDestColor[x] = (destAlpha << 24 ) | destRGB;					
                        #endif
                    }
                }
                pDestColor  += srcW;
                pGlyphAlpha += stride;
            }
        }
        else
        {
            EAW_ASSERT_FORMATTED(0, "FontEA: unsuported glyph texture format: %d", gdi.mFormat);
        }
     }

     // Draw the final image using Cairo lib   
     cairo_t* context = pGraphicsContext->platformContext()->cr();
     cairo_save(context);
     cairo_surface_t *surface = cairo_image_surface_create_for_data((unsigned char*)glyphRGBABuffer.data(), CAIRO_FORMAT_ARGB32, srcW, srcH, srcW * sizeof(uint32_t));
     EAW_ASSERT(cairo_surface_status(surface) == CAIRO_STATUS_SUCCESS);
     
     // If we pass sub pixel offsets (e.g. 75.5), some Cario interpolation kicks in and can degrade the font quality.
     // So we use pixel grid "snapping" to disable this.  This might cause some jitter if the font is animating/moving slowly. 
     x = roundf(x);
     y = roundf(y);
     
     cairo_set_source_surface(context, surface, x, y);
     drawGlyphsShadow(pGraphicsContext, x, y, srcW, srcH, surface);
     cairo_paint(context);
 
     // Uncomment to save text runs.
     //static int runNumber = 0;
     //eastl::string8 filepath;
     //filepath.sprintf("text/run%d.png", runNumber++);
     //cairo_surface_write_to_png(surface, filepath.c_str());
     //cairo_surface_write_to_png(cairo_get_target(context), filepath.c_str());
     
     cairo_surface_destroy(surface);
     cairo_restore(context);
 
     // Uncomment to visualize text runs.
     //cairo_save(context);
     //cairo_set_source_rgb(context, 0, 0, 255);
     //cairo_set_line_width(context, 1);
     //cairo_rectangle(context, x, y, srcW, srcH);
     //cairo_stroke(context);
     //cairo_restore(context);
}

namespace
{
    void GetMetrics(EA::WebKit::IFont *font, EA::WebKit::GlyphId *glyphInOut, EA::WebKit::GlyphMetrics *metricsOut)
    {
        if(!font->GetGlyphMetrics(*glyphInOut, *metricsOut))
        {
            EAW_ASSERT_MSG(false, "Font::drawGlyphs: invalid glyph/Font combo.");
			font->GetGlyphIds(EA_CHAR16("?"), 1, glyphInOut, true);
            font->GetGlyphMetrics(*glyphInOut, *metricsOut);
        }
    }

    void GetClipExtentsX(GraphicsContext *pGraphicsContext, float *leftOut, float *rightOut)
    {
        // Get the clip region. For now, only care about horizontal clipping.
        cairo_t *cr = pGraphicsContext->platformContext()->cr();
        double clipX1 = 0.0;
        double clipY1 = 0.0;
        double clipX2 = 0.0;
        double clipY2 = 0.0;
        cairo_clip_extents(cr, &clipX1, &clipY1, &clipX2, &clipY2);

        *leftOut = (float)clipX1;
        *rightOut = (float)clipX2;
    }
}
  
// We need to draw the glyphBuffer glyphs/advances with the pSimpleFontData font onto the 
// pGraphicsContext pSurface. We draw glyphCount glyphs starting at the glyphIndexBegin.
void Font::drawGlyphs(GraphicsContext* pGraphicsContext, const SimpleFontData* pSimpleFontData, const GlyphBuffer& glyphBuffer,
                        int glyphIndexBegin, int glyphCount, const FloatPoint& point) const
{
    if (glyphCount <= 0)
    {
        return;
    }

    EA::WebKit::ITextSystem *pTextSystem = EA::WebKit::GetTextSystem();
    if(!pTextSystem)
    {
        EAW_ASSERT_MSG(pTextSystem, "TextSystem is not set");
        return;
    }   

    // Get the clip extents. Only care about horizontal clipping for now.
    float clipLeft = 0.0f;
    float clipRight = 0.0f;
    GetClipExtentsX(pGraphicsContext, &clipLeft, &clipRight);

    // Translate the clip region into offset space.
    clipLeft -= point.x();
    clipRight -= point.x();

    // If we have a shadow offset or blur, we need to also consider glyphs just beyond the clip area for they might be casting a shadow into the draw area.
    const ShadowBlur& shadow = pGraphicsContext->platformContext()->shadowBlur();
    if ((pGraphicsContext->textDrawingMode() & TextModeFill) && shadow.type() != ShadowBlur::NoShadow)
    {
        // Add the shadow offset to the clip offset.
        const FloatSize shadowOffset(pGraphicsContext->state().shadowOffset);
        float clipOffset = shadowOffset.width() >= 0.f ? shadowOffset.width() : -shadowOffset.width();
        
        // Add the blur to the clip offset.  
        clipOffset += pGraphicsContext->state().shadowBlur;
        
        // Add the max shadow clip offset to both sides regardless of shadow/blur direction.  The Cairo clip rect will refine it if needed.
        clipLeft -= clipOffset;
        clipRight += clipOffset;
    }


    EA::WebKit::IFont *pFont = pSimpleFontData->getEAFont();
	if(!pFont)
	{
		EAW_ASSERT_MSG(pFont, "Font is not available");
		return;
	}   
    pFont->BeginDraw();    

    EA::WebKit::GlyphMetrics glyphMetrics;
    const GlyphBufferGlyph *glyphs = glyphBuffer.glyphs(glyphIndexBegin);

    // Collect the glyphs texture info for the text run.
    GlyphDrawInfoArray gdiTextures(glyphCount);
    pTextSystem->GetGlyphs(pFont, glyphs, glyphCount, gdiTextures.data());
    GlyphDrawInfoArray::iterator it(gdiTextures.begin());
    
    GlyphDrawInfoArray gdiArray;     

    float offset = 0;
    float x_offset = 0;

    // Setup using the first glyph.
    EA::WebKit::GlyphId firstGlyph = glyphs[0];
    GetMetrics(pFont, &firstGlyph, &glyphMetrics);

    float xMin = glyphMetrics.mfHBearingX;
    float xMax = glyphMetrics.mfHBearingX + glyphMetrics.mfSizeX;
    float yMin = glyphMetrics.mfHBearingY - glyphMetrics.mfSizeY;
    float yMax = glyphMetrics.mfHBearingY;

    if (xMin < 0.0f)
    {
        offset = -glyphMetrics.mfHBearingX;
        x_offset = xMin;
        xMin = 0.0f;
        xMax +=offset;      // Correct x offset for sliding it to the right to keep same width.
    }

    if (xMax >= clipLeft && xMin <= clipRight)
    {
        if (it->mpData && (it->mV1 != 0.0f || it->mU1 != 0.0f))
        {
            it->x1 = xMin;
            it->x2 = xMax;
            it->y1 = yMax;
            it->y2 = yMin;
            gdiArray.push_back(*it);
        }
    }
    else
    {
        xMin = FLT_MAX;
        xMax = FLT_MIN;
        yMin = FLT_MAX;
        yMax = FLT_MIN;
    }

    offset += glyphBuffer.advanceAt(glyphIndexBegin).width();

    // Walk through the list of glyphs and build up render info for each one.
    for (int i = 1; i < glyphCount; ++i)
    {
        ++it;    
        EA::WebKit::GlyphId g = glyphs[i];
        GetMetrics(pFont, &g, &glyphMetrics);

        // Apply kerning.
        // 1/25/12 - We also modified the width iterator code (see WidthIterator.cpp) to take kerning into account during the layout (not accounted for by default, just the raw glyph width).
        EA::WebKit::Kerning kerning;
        if (pFont->GetKerning(glyphs[i - 1], g, kerning, 0))
        {
            // The rounding system used needs to stay in sync with the kerning rounding used in WidthIterator.cpp.  Since the 
            // kerning brings the glyphs closer together, we don't want the rounding to pack the glyphs too close.
            offset += ceilf(kerning.mfKernX);
        }

        // The values we calculate here are relative to the current pen position at the 
        // baseline position of [xoffset, 0], where +X is rightward and +Y is upward.
        float x1 = offset + glyphMetrics.mfHBearingX;
        float x2 = offset + glyphMetrics.mfHBearingX + glyphMetrics.mfSizeX;
        float y1 = glyphMetrics.mfHBearingY;
        float y2 = glyphMetrics.mfHBearingY - glyphMetrics.mfSizeY;

        EAW_ASSERT(x1 <= x2);
        EAW_ASSERT(y2 <= y1);   // This is not a typo. Y2 should actually be less than y1.

        if (x2 >= clipLeft)
        {
            if (x1 <= clipRight)
            {
                xMin = eastl::min_alt(x1, xMin);
                xMax = eastl::max_alt(x2, xMax);
                yMin = eastl::min_alt(y2, yMin);
                yMax = eastl::max_alt(y1, yMax);

                if (it->mpData && (it->mV1 != 0.0f || it->mU1 != 0.0f))
                {
                    it->x1 = x1;
                    it->y1 = y1;
                    it->x2 = x2;
                    it->y2 = y2;
                    gdiArray.push_back(*it);
                }
            }
            else
            {
                break;  // This break assumes that we are rendering text from left to right.
            }
        }

        // advanceAt should return a value that is usually equivalent to glyphMetrics.mfHAdvanceX, at least 
        // for most simple Western text. A case where it would be different would be Arabic combining glyphs,
        // and custom kerning, though kerning adjustments are handled above.
        offset += glyphBuffer.advanceAt(glyphIndexBegin + i).width();
    }

    gdiTextures.clear();    // Free up some space before the draw as we no longer need this info.

    if(gdiArray.size())
    {
        const int destWidth  = (int)(xMax - xMin);
        const int destHeight = (int)(yMax - yMin);

        // (If we are drawing just space chars (which often happens), then destWidth and/or destHeight will be zero)
        if (destWidth && destHeight) 
        {
            const float x = point.x() + x_offset + xMin;
            const float y = point.y() - yMax;
            drawGlyphsToContext(pGraphicsContext, gdiArray.data(), gdiArray.size(), destWidth, destHeight, x, y, xMin, yMin);
        }
    }

    pFont->EndDraw();    
}

void Font::drawComplexText(GraphicsContext* ctx, const TextRun& run, const FloatPoint& point, int from, int to) const
{
    EA::WebKit::ITextSystem *pTextSystem = EA::WebKit::GetTextSystem();
    if(!pTextSystem)
    {
        return;
    }   

    const SimpleFontData* pSimpleFontData = primaryFont();
    EA::WebKit::IFont *pFont = pSimpleFontData->getEAFont();
    if (!pFont)    
        return;

    pFont->BeginDraw();    

    // Set up clip area.
    float clipLeft = 0.0f;
    float clipRight = 0.0f;
    GetClipExtentsX(ctx, &clipLeft, &clipRight);

    // Translate the clip region into offset space.
    clipLeft -= point.x();
    clipRight -= point.x();

    const EA::WebKit::Char* pText = (const EA::WebKit::Char*) (run.characters16() + from); 
    EA::WebKit::TextDrawInfo dInfo;
    bool result = pTextSystem->GetDrawInfoForComplexText(pFont, pText, (to - from), clipLeft, clipRight, dInfo);
    if (result == false)
    {
        // Was just empty spaces for example.
        pFont->EndDraw();
        return;
    }

    // Send the generated glyph draw info to the draw.
    const float x = point.x() + dInfo.mXMin;
    const float y = point.y() - dInfo.mH - dInfo.mYMin;
    drawGlyphsToContext(ctx, dInfo.mpGDI, dInfo.mGDICount, dInfo.mW, dInfo.mH, x, y, dInfo.mXMin, dInfo.mYMin);

    pFont->EndDraw();
}

float Font::floatWidthForComplexText(const TextRun& run, HashSet<const SimpleFontData*>* /*fallbackFonts*/, GlyphOverflow* /*glyphOverflow*/) const
{
    EA::WebKit::ITextSystem *pTextSystem = EA::WebKit::GetTextSystem();
    if(!pTextSystem)
        return 0.f;

    const SimpleFontData* pSimpleFontData = primaryFont();
    EA::WebKit::IFont *pFont = pSimpleFontData->getEAFont();
    if (!pFont)    
        return 0.f;

    return pTextSystem->GetWidthForComplexText(pFont, run.characters16(), run.length());
}

int Font::offsetForPositionForComplexText(const TextRun& /*run*/, float /*position*/, bool /*includePartialGlyphs*/) const
{
    return 0;
}

FloatRect Font::selectionRectForComplexText(const TextRun& /*run*/, const FloatPoint& /*pt*/, int /*h*/, int /*from*/, int /*to*/) const
{
    return FloatRect();
}

bool Font::canReturnFallbackFontsForComplexText()
{
    return false;
}


} // WebCore

