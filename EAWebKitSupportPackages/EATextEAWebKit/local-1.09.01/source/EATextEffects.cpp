/*
Copyright (C) 2004, 2009, 2010, 2012, 2013 Electronic Arts, Inc.  All rights reserved.

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
// EATextStyle.cpp
// Created by Paul Pedriana - 2007
///////////////////////////////////////////////////////////////////////////////


#include <EAText/EAText.h>
#include <EAText/EATextEffects.h>
#include <EAText/EATextOutlineFont.h>
#include <EAText/internal/EATextCoreAllocatorNew.h>
#include <EAText/internal/StdC.h>
#include <coreallocator/icoreallocator_interface.h>
#include EA_ASSERT_HEADER
#include <ctype.h>
#include <math.h>
#include <stdio.h>
#include <string.h>
#include <stdlib.h>

#if EATEXT_USE_FREETYPE
    #include <ft2build.h>
    #include FT_FREETYPE_H  // #include <freetype/freetype.h>
    #include FT_SIZES_H     // #include <freetype/ftsizes.h>
#endif


///////////////////////////////////////////////////////////////////////////////
// Defines
//
#define FFFixed16ToFloat(x) (((float)(x) * (1.f / 65536.f))) // 1/65536 optimizes to an exact floating point constant, as 65536 is a power of 2.
#define FFFixed26ToFloat(x) (((float)(x) * (1.f / 64.f)))    // 1/64 optimizes to an exact floating point constant, as 65536 is a power of 2.
#define kBrushEnd INT8_MAX
#define kPenUnset INT32_MAX
#define kPosUnset INT32_MAX


namespace EA
{

namespace Text
{

namespace Effects
{
    const int   kSampleCount        = 16;
    const int   kSampleRange        = 256 / kSampleCount;
    const float kOneDivSampleCount  = 1.f / kSampleCount;
    const float kOneDivSampleRange  = 1.f / kSampleRange;

                                               // 0, 16, 32, 48, 64, 80, 96, 112, 128, 144, 160, 176, 192, 208, 224, 240, 256       // Input
    const uint8_t kJSamples[kSampleCount + 1] = { 0,  3,  6, 12, 24, 38, 56,  76,  94, 112, 136, 154, 176, 194, 216, 235, 255 };    // Output

                                               // 0, 16, 32, 48, 64, 80, 96, 112, 128, 144, 160, 176, 192, 208, 224, 240, 256       // Input
    const uint8_t kFSamples[kSampleCount + 1] = { 0, 20, 40, 62, 80,102,120, 144, 162, 180, 200, 218, 230, 242, 248, 252, 255 };    // Output

                                               // 0, 16, 32, 48, 64, 80, 96, 112, 128, 144, 160, 176, 192, 208, 224, 240, 256       // Input
    const uint8_t kSSamples[kSampleCount + 1] = { 0,  3,  6, 12, 24, 44, 72, 100, 128, 158, 184, 214, 230, 242, 248, 252, 255 };    // Output


    //////////////////////////////////////////////////////////////////////////
    // CombineColors
    //
    // Combine source color ARGB with destination color ARGB.
    // Here is a representation of this where color are represented as float [0.0-1.0]:
    //     dest_alpha = (dest_alpha * (1 - source_alpha)) + source_alpha
    //     dest_color = (dest_color * (1 - source_alpha)) + (source_color * source_alpha)
    // However, we are working with uint8_t where components are represented as [0-255].
    //
    // We use the fast CPU fixed point multiply trick for uint8_t components:
    //      c = a * b                   // Where the values are [0-255] and represent fractions from [0.0-1.0]).
    //         ->
    //      c = ((a * b) + 1);          // Multiply glyph opacity * brush opacity. +1 to deal with roundoff error.
    //      c = ((c + (c >> 8)) >> 8);  // Divide by 255, fixed point. This is a trick shifting version of this.
    //
    // However, this function could be further optimized.
    //
    inline uint32_t Multiply(uint32_t a, uint32_t b)
    {
        uint32_t c = ((a * b) + 1);     // Multiply glyph opacity * brush opacity. +1 to deal with roundoff error.
        return ((c + (c >> 8)) >> 8);   // Divide by 255, fixed point. This is a trick shifting version of this.
    }

    inline uint32_t CombineColors(uint32_t s, uint32_t d)
    {
        const uint32_t sA  = (s >> 24) & 0xff;  // Source alpha
        const uint32_t dA  = (d >> 24) & 0xff;  // Destination alpha
        const uint32_t sR  = (s >> 16) & 0xff;
        const uint32_t dR  = (d >> 16) & 0xff;
        const uint32_t sG  = (s >>  8) & 0xff;
        const uint32_t dG  = (d >>  8) & 0xff;
        const uint32_t sB  = (s      ) & 0xff;
        const uint32_t dB  = (d      ) & 0xff;
        const uint32_t iA  = 255 - sA;          // Inverse source alpha
        const uint32_t rA  = Multiply(dA, iA) + sA;     
        const uint32_t rR  = Multiply(dR, iA) + Multiply(sR, sA);
        const uint32_t rG  = Multiply(dG, iA) + Multiply(sG, sA);
        const uint32_t rB  = Multiply(dB, iA) + Multiply(sB, sA);

        return (rA << 24) | (rR << 16) | (rG <<  8) | rB;
    }


    #if defined(EA_PLATFORM_WINDOWS) && defined(EA_PROCESSOR_X86)
        // The x86 processor is particularly slow at float->int conversions, so we use a specialized version.
        // An alternative approach is to use _mm_cvtss_si32(_mm_set_ss(fValue + 0.5f))
        const int32_t kFToIBiasF32 = 3 << 22;
        const int32_t kFToIBiasS32 = 0x4B400000;

        inline int32_t RoundToInt32(float fValue)
        {
            const union {
                float   f;
                int32_t i;
            } converter = { fValue + kFToIBiasF32 };

            return converter.i - kFToIBiasS32;
        }
    #else
        inline int32_t RoundToInt32(float fValue)
        {
            return (int32_t)fValue; // We don't need the additional functionality of the floorf function.
        }
    #endif


} // namespace Effects




///////////////////////////////////////////////////////////////////////////////
// CreateInstructionList
///////////////////////////////////////////////////////////////////////////////

EATEXT_API uint32_t CreateInstructionList(const char* pInstructionString, EA::Text::EIWord* pInstructionList, uint32_t /*instructionListCapacity*/, int32_t* pErrorLine)
{
    using namespace EA::Text;

    const char* pInstructionStringEnd = pInstructionString + strlen(pInstructionString);
    const char* pLineNext(pInstructionString);
    const char* pLine;
    const char* pLineEnd;
    const int   kMaxCommandCount = 6;
    const int   kMaxCommandSize = 32;
    char        pCommand[kMaxCommandCount][kMaxCommandSize];
    int         nCommandCount;
    uint32_t    instructionCount = 0;
    int32_t     nCurrentLine = 1;   // We'll use 1-based counting for the text lines.
    int32_t     nErrorLine = -1;

    do{
        pLine    = pLineNext;
        pLineEnd = GetTextLine(pLine, pInstructionStringEnd, &pLineNext);

        // Read the instruction name into pCommand.
        memset(pCommand, 0, sizeof(pCommand));

        for(nCommandCount = 0; (nCommandCount < kMaxCommandCount) && SplitTokenSeparated(pLine, pLineEnd - pLine, (char8_t)' ', &pCommand[nCommandCount][0], kMaxCommandSize, &pLine); ++nCommandCount)
        {
            // Do nothing; just loop.
        }

        if(nCommandCount)
        {
            if(Stricmp(pCommand[0], "SetExpansion") == 0)
            {
                if(nCommandCount < 3)
                    nErrorLine = nCurrentLine;
                pInstructionList[instructionCount++].mU32 = kEISetExpansion;
                pInstructionList[instructionCount++].mU32 = AtoU32(pCommand[1]);
                pInstructionList[instructionCount++].mU32 = AtoU32(pCommand[2]);
            }
            else if(Stricmp(pCommand[0], "SetGlyphMinAlpha") == 0)
            {
                if(nCommandCount < 2)
                    nErrorLine = nCurrentLine;
                pInstructionList[instructionCount++].mU32 = kEISetGlyphMinAlpha;
                pInstructionList[instructionCount++].mU32 = AtoU32(pCommand[1]);
            }
            else if(Stricmp(pCommand[0], "SetGlyphColor") == 0)
            {
                if(nCommandCount < 2)
                    nErrorLine = nCurrentLine;
                pInstructionList[instructionCount++].mU32 = kEISetGlyphColor;
                pInstructionList[instructionCount++].mU32 = StrtoU32(pCommand[1], NULL, 16);
            }
            else if(Stricmp(pCommand[0], "SetGlyphOffset") == 0)
            {
                if(nCommandCount < 3)
                    nErrorLine = nCurrentLine;
                pInstructionList[instructionCount++].mU32 = kEISetGlyphOffset;
                pInstructionList[instructionCount++].mI32 = AtoI32(pCommand[1]);
                pInstructionList[instructionCount++].mI32 = AtoI32(pCommand[2]);
            }
            else if(Stricmp(pCommand[0], "SetGlyphHSpace") == 0)
            {
                if(nCommandCount < 3)
                    nErrorLine = nCurrentLine;
                pInstructionList[instructionCount++].mU32 = kEISetGlyphHSpace;
                pInstructionList[instructionCount++].mI32 = AtoI32(pCommand[1]);
                pInstructionList[instructionCount++].mI32 = AtoI32(pCommand[2]);
            }
            else if(Stricmp(pCommand[0], "SetGlyphVSpace") == 0)
            {
                if(nCommandCount < 3)
                    nErrorLine = nCurrentLine;
                pInstructionList[instructionCount++].mU32 = kEISetGlyphVSpace;
                pInstructionList[instructionCount++].mI32 = AtoI32(pCommand[1]);
                pInstructionList[instructionCount++].mI32 = AtoI32(pCommand[2]);
            }
            else if(Stricmp(pCommand[0], "SetGlyphSmooth") == 0)
            {
                if(nCommandCount < 2)
                    nErrorLine = nCurrentLine;
                pInstructionList[instructionCount++].mU32  = kEISetGlyphSmooth;
                pInstructionList[instructionCount++].mBool = AtoU32(pCommand[1]) ? 1 : 0;
            }
            else if(Stricmp(pCommand[0], "SetGlyphBrush") == 0)
            {
                if(nCommandCount < 5)
                    nErrorLine = nCurrentLine;
                pInstructionList[instructionCount++].mU32 = kEISetGlyphBrush;
                pInstructionList[instructionCount++].mU32 = AtoU32(pCommand[1]);
                pInstructionList[instructionCount++].mU32 = AtoU32(pCommand[2]);
                pInstructionList[instructionCount++].mF32 = (float)Atof(pCommand[3]);
                pInstructionList[instructionCount++].mF32 = (float)Atof(pCommand[4]);
            }
            else if(Stricmp(pCommand[0], "SetTargetImage") == 0)
            {
                if(nCommandCount < 2)
                    nErrorLine = nCurrentLine;
                pInstructionList[instructionCount++].mU32 = kEISetTargetImage;
                pInstructionList[instructionCount++].mU32 = AtoU32(pCommand[1]);
            }
            else if(Stricmp(pCommand[0], "SetTargetMask") == 0)
            {
                if(nCommandCount < 2)
                    nErrorLine = nCurrentLine;
                pInstructionList[instructionCount++].mU32 = kEISetTargetMask;
                pInstructionList[instructionCount++].mU32 = AtoU32(pCommand[1]);
            }
            else if(Stricmp(pCommand[0], "DrawGlyphSmearOutline") == 0)
            {
                if(nCommandCount < 1)
                    nErrorLine = nCurrentLine;
                pInstructionList[instructionCount++].mU32 = kEIDrawGlyphSmearOutline;
            }
            else if(Stricmp(pCommand[0], "DrawGlyphBrushOutline") == 0)
            {
                if(nCommandCount < 1)
                    nErrorLine = nCurrentLine;
                pInstructionList[instructionCount++].mU32 = kEIDrawGlyphBrushOutline;
            }
            else if(Stricmp(pCommand[0], "DrawGlyph") == 0)
            {
                if(nCommandCount < 1)
                    nErrorLine = nCurrentLine;
                pInstructionList[instructionCount++].mU32 = kEIDrawGlyph;
            }
            else if(Stricmp(pCommand[0], "Clear") == 0)
            {
                if(nCommandCount < 1)
                    nErrorLine = nCurrentLine;
                pInstructionList[instructionCount++].mU32 = kEIClear;
            }
            else if(Stricmp(pCommand[0], "ApplyCurve") == 0)
            {
                if(nCommandCount < 3)
                    nErrorLine = nCurrentLine;
                pInstructionList[instructionCount++].mU32 = kEIApplyCurve;
                pInstructionList[instructionCount++].mU32 = AtoU32(pCommand[1]);
                pInstructionList[instructionCount++].mF32 = (float)Atof(pCommand[2]);
            }
            else if(Stricmp(pCommand[0], "ApplyBoxBlur") == 0)
            {
                if(nCommandCount < 4)
                    nErrorLine = nCurrentLine;
                pInstructionList[instructionCount++].mU32 = kEIApplyBoxBlur;
                pInstructionList[instructionCount++].mF32 = (float)Atof(pCommand[1]);
                pInstructionList[instructionCount++].mF32 = (float)Atof(pCommand[2]);
                pInstructionList[instructionCount++].mU32 = AtoU32(pCommand[3]);
            }
            else if(Stricmp(pCommand[0], "Merge") == 0)
            {
                if(nCommandCount < 1)
                    nErrorLine = nCurrentLine;
                pInstructionList[instructionCount++].mU32 = kEIMerge;
            }
            else if(Stricmp(pCommand[0], "CallUser") == 0)
            {
                if(nCommandCount < 2)
                    nErrorLine = nCurrentLine;
                pInstructionList[instructionCount++].mU32 = kEICallUser;
                pInstructionList[instructionCount++].mU32 = StrtoU32(pCommand[1], NULL, 0);
            }
            else
            {
                if(strstr(pCommand[0], "//") != pCommand[0]) // If the line doesn't begin with a C++-like comment...
                    nErrorLine = nCurrentLine;
                // Else we have a line that begins with //, which is OK and ignored.
            }
        } // Else we have an empty line, which is ignored.

        nCurrentLine++;
    }while((pLineNext != pInstructionStringEnd) && (nErrorLine == -1));

    if(nErrorLine != -1)
        instructionCount = 0;
    if(pErrorLine)
        *pErrorLine = nErrorLine;

    return instructionCount;
}


EATEXT_API uint32_t CreateInstructionList(const char* pInstructionString, EA::Text::EffectsState& es, int32_t* pError)
{
    es.ClearInstructionList();  // You normally don't need to call ClearInstructionList unless you are re-using an EffectsState.
    es.ResetParameters();       // You normally don't need to call ResetParameters unless you are re-using an EffectsState.
    es.mInstructionListSize = CreateInstructionList(pInstructionString, es.mInstructionList, EA::Text::kEffectInstructionListCapacity, pError);
    return es.mInstructionListSize;
}



///////////////////////////////////////////////////////////////////////////////
// EffectsBitmaps
///////////////////////////////////////////////////////////////////////////////


EffectsBitmaps::EffectsBitmaps()
  : mFloatImage()
  , mBaseImage()
  , mBlurPrecalcSumImage()
  , mpUser(NULL)
  , mUserCount(0)
{
}


void EffectsBitmaps::BeginUse(void* pUser, EA::Allocator::ICoreAllocator* pAllocator)
{
    #if defined(EA_DEBUG)
        EA_ASSERT(!mpUser);
    #endif

    mpUser = pUser;

    if(pAllocator != mFloatImage.mpCoreAllocator)
    {
        mFloatImage.Shutdown();
        mFloatImage.mpCoreAllocator = pAllocator;

        mBaseImage.Shutdown();
        mBaseImage.mpCoreAllocator = pAllocator;

        mBlurPrecalcSumImage.Shutdown();
        mBlurPrecalcSumImage.mpCoreAllocator = pAllocator;
    }

    // Don't unlock the mutex; we unlock it in the EndUse function.
}


void EffectsBitmaps::EndUse(void* pUser)
{
    // Don't shutdown the images, as we might well need the space for the next glyph drawing.

    #if defined(EA_DEBUG)
        EA_ASSERT(mpUser == pUser);
    #else
        (void)pUser;
    #endif

    mpUser = NULL;
}


bool EffectsBitmaps::IsInUse() const
{
    // We don't lock the mutex because we just want to know if the pointer is NULL,
    // and synchronization of this thread's view of mpUser shouldn't matter.
    return (mpUser != NULL);
}


bool EffectsBitmaps::IsInUse(void* pUser)
{
    return (pUser == mpUser);
}


void EffectsBitmaps::Reset()
{
    mFloatImage.Shutdown();
    mBaseImage.Shutdown();
    mBlurPrecalcSumImage.Shutdown();
}



///////////////////////////////////////////////////////////////////////////////
// EffectsState
///////////////////////////////////////////////////////////////////////////////


EffectsBitmaps EffectsState::mEffectsBitmaps;


EffectsState::EffectsState(EA::Allocator::ICoreAllocator* pCoreAllocator)
  : mInstructionListSize(0),
    mpCurrentInstruction(mInstructionList),
    mpOutlineFont(NULL),
    mGlyphId(kGlyphIdInvalid),
    mRequiredGutterSpace(kGlyphGutterSizeDefault),
    mbFloatImageClear(true),
    mbBaseImageClear(true),
    mImagePenX(kPenUnset),
    mImagePenY(kPenUnset),
    mActualGlyphLeft(0),
    mActualGlyphTop(0),
    mActualGlyphRight(0),
    mActualGlyphBottom(0),
    mGlyphExpansion(0),
    mGlyphMetrics()
{
    // Currently we don't need to use pCoreAllocator.
    (void)pCoreAllocator;

    #if EATEXT_DEBUG
        memset(mInstructionList, 0, sizeof(mInstructionList));
        memset(mBrush, 0, sizeof(mBrush));
    #endif
    ResetParameters();
    mBrush[0].mDX = kBrushEnd;

    mEffectsBitmaps.mUserCount++;
}


EffectsState::~EffectsState()
{
    if(mEffectsBitmaps.mUserCount-- == 0) // If we are the last user...
    {
        EA_ASSERT(!mEffectsBitmaps.IsInUse());  // This should be so because there are no more users.
        mEffectsBitmaps.Reset();                // Free any existing memory allocated by the shared EffectsBitmaps.
    }
}


void EffectsState::ResetParameters()
{
    mGlyphMinAlpha       = 2;
    mGlyphColor          = 0xff000000;
    mGlyphDX             = 0;
    mGlyphDY             = 0;
    mGlyphLeftSpace      = 0;
    mGlyphTopSpace       = 0;
    mGlyphRightSpace     = 0;
    mGlyphBottomSpace    = 0;
    mMaxExpansion        = kMaxExpansionDefault;
    mGlyphSmooth         = 1;
    mGlyphBrushShape     = kBSRound;
    mGlyphBrushSize      = 3;
    mGlyphBrushHardness  = 0.5f;
    mGlyphBrushOpacity   = 1.0f;
    mTargetImage         = kETIFloat;
    mTargetMask          = kECMAlpha | kECMColor;
}


void EffectsState::ResetExecution(OutlineFont* pOutlineFont, GlyphId glyphId)
{
    mpCurrentInstruction = mInstructionList;
    mpOutlineFont        = pOutlineFont;
    mGlyphId             = glyphId;
    mBrush[0].mDX        = kBrushEnd;
    mRequiredGutterSpace = kGlyphGutterSizeDefault;
    mImagePenX           = kPenUnset;
    mImagePenY           = kPenUnset;
    mActualGlyphLeft     = 0;
    mActualGlyphTop      = 0;
    mActualGlyphRight    = 0;
    mActualGlyphBottom   = 0;
    mGlyphExpansion      = 0;
  //mGlyphMetrics
}


void EffectsState::ClearInstructionList()
{
    mInstructionListSize = 0;
    #if EATEXT_DEBUG
        memset(mInstructionList, 0, sizeof(mInstructionList));
    #endif
}


void EffectsState::AppendInstructionList(const EIWord* pInstructionList, uint32_t nInstructionCount)
{
    EA_ASSERT((mInstructionListSize + nInstructionCount) <= kEffectInstructionListCapacity);
    memcpy(mInstructionList + mInstructionListSize, pInstructionList, nInstructionCount * sizeof(EIWord));
    mInstructionListSize += nInstructionCount;
}


uint32_t EffectsState::HashInstructionList(const EIWord* pInstructionList, uint32_t nInstructionCount)
{
    // Note that this function treats floating point instruction values of -0 and +0 as different.
    uint32_t h = 0;

    for(const uint8_t* p8    = (const uint8_t*)(const void*)(pInstructionList), 
                     * p8End = (const uint8_t*)(const void*)(pInstructionList + nInstructionCount); 
                         p8 != p8End; ++p8)
    {
            h = *p8 + (h << 6) + (h << 16) - h;
    }

    return h;
}


void EffectsState::SetExpansion(uint32_t gutterSize, uint32_t maxExpansion)
{
    EA_ASSERT((mInstructionListSize + 2) <= kEffectInstructionListCapacity);
    mInstructionList[mInstructionListSize++].mU32 = kEISetExpansion;
    mInstructionList[mInstructionListSize++].mU32 = gutterSize;
    mInstructionList[mInstructionListSize++].mU32 = maxExpansion;
}


void EffectsState::SetGlyphMinAlpha(uint32_t colorA)
{
    EA_ASSERT((mInstructionListSize + 2) <= kEffectInstructionListCapacity);
    mInstructionList[mInstructionListSize++].mU32 = kEISetGlyphMinAlpha;
    mInstructionList[mInstructionListSize++].mU32 = colorA;
}


void EffectsState::SetGlyphColor(uint32_t colorARGB)
{
    EA_ASSERT((mInstructionListSize + 2) <= kEffectInstructionListCapacity);
    mInstructionList[mInstructionListSize++].mU32 = kEISetGlyphColor;
    mInstructionList[mInstructionListSize++].mU32 = colorARGB;
}


void EffectsState::SetGlyphOffset(int32_t dx, int32_t dy)
{
    EA_ASSERT((mInstructionListSize + 3) <= kEffectInstructionListCapacity);
    mInstructionList[mInstructionListSize++].mU32 = kEISetGlyphOffset;
    mInstructionList[mInstructionListSize++].mI32 = dx;
    mInstructionList[mInstructionListSize++].mI32 = dy;
}


void EffectsState::SetGlyphHSpace(int32_t leftSpace, int32_t rightSpace)
{
    EA_ASSERT((mInstructionListSize + 3) <= kEffectInstructionListCapacity);
    mInstructionList[mInstructionListSize++].mU32 = kEISetGlyphHSpace;
    mInstructionList[mInstructionListSize++].mI32 = leftSpace;
    mInstructionList[mInstructionListSize++].mI32 = rightSpace;
}


void EffectsState::SetGlyphVSpace(int32_t topSpace, int32_t bottomSpace)
{
    EA_ASSERT((mInstructionListSize + 3) <= kEffectInstructionListCapacity);
    mInstructionList[mInstructionListSize++].mU32 = kEISetGlyphVSpace;
    mInstructionList[mInstructionListSize++].mI32 = topSpace;
    mInstructionList[mInstructionListSize++].mI32 = bottomSpace;
}


void EffectsState::SetGlyphSmooth(bool bEnable)
{
    EA_ASSERT((mInstructionListSize + 2) <= kEffectInstructionListCapacity);
    mInstructionList[mInstructionListSize++].mU32  = kEISetGlyphSmooth;
    mInstructionList[mInstructionListSize++].mBool = bEnable ? 1 : 0;
}


void EffectsState::SetGlyphBrush(uint32_t shape, uint32_t size, float hardness, 
                                  float opacity)
{
    EA_ASSERT((mInstructionListSize + 5) <= kEffectInstructionListCapacity);
    mInstructionList[mInstructionListSize++].mU32 = kEISetGlyphBrush;
    mInstructionList[mInstructionListSize++].mU32 = shape;
    mInstructionList[mInstructionListSize++].mU32 = size;
    mInstructionList[mInstructionListSize++].mF32 = hardness;
    mInstructionList[mInstructionListSize++].mF32 = opacity;
}


void EffectsState::SetTargetImage(uint32_t image)
{
    EA_ASSERT((mInstructionListSize + 2) <= kEffectInstructionListCapacity);
    mInstructionList[mInstructionListSize++].mU32 = kEISetTargetImage;
    mInstructionList[mInstructionListSize++].mU32 = image;
}


void EffectsState::SetTargetMask(uint32_t mask)
{
    EA_ASSERT((mInstructionListSize + 2) <= kEffectInstructionListCapacity);
    mInstructionList[mInstructionListSize++].mU32 = kEISetTargetMask;
    mInstructionList[mInstructionListSize++].mU32 = mask;
}


void EffectsState::DrawGlyphSmearOutline()
{
    EA_ASSERT((mInstructionListSize + 1) <= kEffectInstructionListCapacity);
    mInstructionList[mInstructionListSize++].mU32 = kEIDrawGlyphSmearOutline;
}


void EffectsState::DrawGlyphBrushOutline()
{
    EA_ASSERT((mInstructionListSize + 1) <= kEffectInstructionListCapacity);
    mInstructionList[mInstructionListSize++].mU32 = kEIDrawGlyphBrushOutline;
}


void EffectsState::DrawGlyph()
{
    EA_ASSERT((mInstructionListSize + 1) <= kEffectInstructionListCapacity);
    mInstructionList[mInstructionListSize++].mU32 = kEIDrawGlyph;
}


void EffectsState::Clear()
{
    EA_ASSERT((mInstructionListSize + 1) <= kEffectInstructionListCapacity);
    mInstructionList[mInstructionListSize++].mU32 = kEIClear;
}


void EffectsState::ApplyCurve(uint32_t type, float strength)
{
    EA_ASSERT((mInstructionListSize + 3) <= kEffectInstructionListCapacity);
    mInstructionList[mInstructionListSize++].mU32 = kEIApplyCurve;
    mInstructionList[mInstructionListSize++].mU32 = type;
    mInstructionList[mInstructionListSize++].mF32 = strength;
}


void EffectsState::ApplyBoxBlur(float radius, float gain, uint32_t passCount)
{
    EA_ASSERT((mInstructionListSize + 4) <= kEffectInstructionListCapacity);
    mInstructionList[mInstructionListSize++].mU32 = kEIApplyBoxBlur;
    mInstructionList[mInstructionListSize++].mF32 = radius;
    mInstructionList[mInstructionListSize++].mF32 = gain;
    mInstructionList[mInstructionListSize++].mU32 = passCount;
}


void EffectsState::Merge()
{
    EA_ASSERT((mInstructionListSize + 1) <= kEffectInstructionListCapacity);
    mInstructionList[mInstructionListSize++].mU32 = kEIMerge;
}


void EffectsState::CallUser(uint32_t id)
{
    EA_ASSERT((mInstructionListSize + 2) <= kEffectInstructionListCapacity);
    mInstructionList[mInstructionListSize++].mU32 = kEICallUser;
    mInstructionList[mInstructionListSize++].mU32 = id;
}




///////////////////////////////////////////////////////////////////////////////
// EffectsProcessor
///////////////////////////////////////////////////////////////////////////////

EffectsProcessor::EffectsProcessor(EA::Allocator::ICoreAllocator* pCoreAllocator)
  : mpCoreAllocator(pCoreAllocator ? pCoreAllocator : EA::Text::GetAllocator()),
    mEffectsState(mpCoreAllocator),
    mpEffectsPlugin(NULL),
    mpUserData(NULL),
    mGlyphState(kGlyphNotDrawn)
{
    #if EATEXT_DEBUG
        mbDebugPrint = false;
    #endif
}


EffectsProcessor::~EffectsProcessor()
{
}

void EffectsProcessor::SetAllocator(Allocator::ICoreAllocator* pCoreAllocator)
{
    mpCoreAllocator = pCoreAllocator;
}

void EffectsProcessor::SetUserCallback(EffectsPlugin* pEffectsPlugin, void* pUserData)
{
    mpEffectsPlugin = pEffectsPlugin;
    mpUserData      = pUserData;
}

EffectsState* EffectsProcessor::GetEffectsState()
{
    return &mEffectsState;
}

void EffectsProcessor::BeginEffectBitmapsUse(void* pUser, EA::Allocator::ICoreAllocator* pAllocator)
{
    EffectsState::mEffectsBitmaps.BeginUse(pUser, pAllocator);
}

void EffectsProcessor::EndEffectBitmapsUse(void* pUser)
{
    EffectsState::mEffectsBitmaps.EndUse(pUser);
}

bool EffectsProcessor::IsEffectBitmapsInUse(void* pUser)
{
    return EffectsState::mEffectsBitmaps.IsInUse(pUser);
}

void EffectsProcessor::ResetEffectBitmaps()
{
    EffectsState::mEffectsBitmaps.Reset();
}


void EffectsProcessor::Execute(OutlineFont* pOutlineFont, GlyphId glyphId)
{
    // Reset the appropriate parts of our state
    mEffectsState.ResetParameters();
    mEffectsState.ResetExecution(pOutlineFont, glyphId);

    #if EATEXT_DEBUG
        bool bGlyphDrawn  = false;
        bool bGlyphCopied = false;
        mbDebugPrint = false;
    #endif

    // Possibly call the user plugin saying we are beginning.
    if(mpEffectsPlugin)
        mpEffectsPlugin->ProcessEffect(kEUIBegin, this, &mEffectsState, mpUserData);

    // Run a loop through the instruction list.
    const EIWord* const pEnd = mEffectsState.mInstructionList + mEffectsState.mInstructionListSize; 

    while(mEffectsState.mpCurrentInstruction != pEnd)
    {
        const uint32_t instruction = (mEffectsState.mpCurrentInstruction++)->mU32;

        switch(instruction)
        {
            ///////////////////////////////////////////////////////////////////
            // Drawing state
            ///////////////////////////////////////////////////////////////////
            case kEISetExpansion:
            {
                #if EATEXT_DEBUG
                    EA_ASSERT(!bGlyphDrawn);
                #endif
                mEffectsState.mRequiredGutterSpace = (mEffectsState.mpCurrentInstruction++)->mU32;
                mEffectsState.mMaxExpansion        = (mEffectsState.mpCurrentInstruction++)->mU32;
                break;
            }

            case kEISetGlyphMinAlpha:
            {
                mEffectsState.mGlyphMinAlpha = (mEffectsState.mpCurrentInstruction++)->mU32;
                break;
            }

            case kEISetGlyphColor:
            {
                mEffectsState.mGlyphColor = (mEffectsState.mpCurrentInstruction++)->mU32;
                if((mEffectsState.mGlyphColor & 0xff000000) == 0x00000000) // If the user specified no alpha...
                    mEffectsState.mGlyphColor |= 0xff000000;               // Assume full alpha.
                break;
            }

            case kEISetGlyphOffset:
            {
                mEffectsState.mGlyphDX = (mEffectsState.mpCurrentInstruction++)->mI32;
                mEffectsState.mGlyphDY = (mEffectsState.mpCurrentInstruction++)->mI32;
                break;
            }

            case kEISetGlyphHSpace:
            {
                mEffectsState.mGlyphLeftSpace  = (mEffectsState.mpCurrentInstruction++)->mI32;
                mEffectsState.mGlyphRightSpace = (mEffectsState.mpCurrentInstruction++)->mI32;
                break;
            }

            case kEISetGlyphVSpace:
            {
                mEffectsState.mGlyphTopSpace    = (mEffectsState.mpCurrentInstruction++)->mI32;
                mEffectsState.mGlyphBottomSpace = (mEffectsState.mpCurrentInstruction++)->mI32;
                break;
            }

            case kEISetGlyphSmooth:
            {
                mEffectsState.mGlyphSmooth = (mEffectsState.mpCurrentInstruction++)->mBool;
                break;
            }

            case kEISetGlyphBrush:
            {
                mEffectsState.mGlyphBrushShape    = (mEffectsState.mpCurrentInstruction++)->mU32;
                mEffectsState.mGlyphBrushSize     = (mEffectsState.mpCurrentInstruction++)->mU32;
                mEffectsState.mGlyphBrushHardness = (mEffectsState.mpCurrentInstruction++)->mF32;
                mEffectsState.mGlyphBrushOpacity  = (mEffectsState.mpCurrentInstruction++)->mF32;
                mEffectsState.mBrush[0].mDX       = kBrushEnd; // Set the brush as needing to be built.
                break;
            }

            case kEISetTargetImage:
            {
                mEffectsState.mTargetImage = (mEffectsState.mpCurrentInstruction++)->mU32;
                break;
            }

            case kEISetTargetMask:
            {
                mEffectsState.mTargetMask = (mEffectsState.mpCurrentInstruction++)->mU32;
                break;
            }


            ///////////////////////////////////////////////////////////////////
            // Glyph drawing actions
            ///////////////////////////////////////////////////////////////////
            case kEIDrawGlyphSmearOutline:
            {
                DrawGlyphSmearOutline();
                #if EATEXT_DEBUG
                    bGlyphDrawn = true;
                    if(mbDebugPrint)
                        DebugPrint();
                #endif
                break;
            }

            case kEIDrawGlyphBrushOutline:
            {
                DrawGlyphBrushOutline();
                #if EATEXT_DEBUG
                    bGlyphDrawn = true;
                    if(mbDebugPrint)
                        DebugPrint();
                #endif
                break;
            }

            case kEIDrawGlyph:
            {
                DrawGlyph();
                #if EATEXT_DEBUG
                    bGlyphDrawn = true;
                    if(mbDebugPrint)
                        DebugPrint();
                #endif
                break;
            }


            ///////////////////////////////////////////////////////////////////
            // Image actions
            ///////////////////////////////////////////////////////////////////
            case kEIClear:
            {
                Clear();
                #if EATEXT_DEBUG
                    if(mbDebugPrint)
                        DebugPrint();
                #endif
                break;
            }

            case kEIApplyCurve:
            {
                const uint32_t curveType = (mEffectsState.mpCurrentInstruction++)->mU32;
                const float    fStrength = (mEffectsState.mpCurrentInstruction++)->mF32;

                ApplyCurve(curveType, fStrength);
                #if EATEXT_DEBUG
                    if(mbDebugPrint)
                        DebugPrint();
                #endif
                break;
            }

            case kEIApplyBoxBlur:
            {
                const float    fRadius    = (mEffectsState.mpCurrentInstruction++)->mF32;
                const float    fGain      = (mEffectsState.mpCurrentInstruction++)->mF32;
                const uint32_t nPassCount = (mEffectsState.mpCurrentInstruction++)->mU32;

                ApplyBoxBlur(fRadius, fGain, nPassCount);
                #if EATEXT_DEBUG
                    if(mbDebugPrint)
                        DebugPrint();
                #endif
                break;
            }

            case kEIMerge:
            {
                Merge();
                #if EATEXT_DEBUG
                    bGlyphCopied = true;
                    if(mbDebugPrint)
                        DebugPrint();
                #endif
                break;
            }


            ///////////////////////////////////////////////////////////////////
            // User action
            ///////////////////////////////////////////////////////////////////
            case kEICallUser:
            {
                const uint32_t id = (mEffectsState.mpCurrentInstruction++)->mU32;

                if(mpEffectsPlugin)
                    mpEffectsPlugin->ProcessEffect(id, this, &mEffectsState, mpUserData);
                break;
            }

        } // switch

    } // while there are still instructions

    #if EATEXT_DEBUG
        EA_ASSERT((bGlyphDrawn && bGlyphCopied) || (mEffectsState.mInstructionListSize == 0)); // We expect that if there are instructions, they should do something. 
    #endif

    // Apply expansion
    if(mEffectsState.mGlyphExpansion > mEffectsState.mMaxExpansion)
       mEffectsState.mGlyphExpansion = mEffectsState.mMaxExpansion;
    mEffectsState.mActualGlyphLeft   -= mEffectsState.mGlyphExpansion;
    mEffectsState.mActualGlyphTop    -= mEffectsState.mGlyphExpansion;
    mEffectsState.mActualGlyphRight  += mEffectsState.mGlyphExpansion;
    mEffectsState.mActualGlyphBottom += mEffectsState.mGlyphExpansion;

    // Do some clipping.
    if(mEffectsState.mActualGlyphLeft < 0)
       mEffectsState.mActualGlyphLeft = 0;
    if(mEffectsState.mActualGlyphTop  < 0)
       mEffectsState.mActualGlyphTop  = 0;
    if(mEffectsState.mActualGlyphRight  >= mEffectsState.mEffectsBitmaps.mFloatImage.mnWidth)
       mEffectsState.mActualGlyphRight   = mEffectsState.mEffectsBitmaps.mFloatImage.mnWidth;
    if(mEffectsState.mActualGlyphBottom >= mEffectsState.mEffectsBitmaps.mFloatImage.mnHeight)
       mEffectsState.mActualGlyphBottom  = mEffectsState.mEffectsBitmaps.mFloatImage.mnHeight;

    // Calculate the new glyph position metrics.
    #if EATEXT_USE_FREETYPE
        FT_Face const pFace = mEffectsState.mpOutlineFont->mpFaceData->mpFTFace;
        EA_ASSERT(pFace && pFace->glyph);
        const float xAdvance = FFFixed26ToFloat(pFace->glyph->advance.x);

        //#if EATEXT_DEBUG
        //    EA_ASSERT(!bGlyphDrawn || pFace->glyph->WhatDoWeUseHereToTellThatItWasDrawn);
        //#endif
    #endif

    mEffectsState.mGlyphMetrics.mfSizeX     = (float)(mEffectsState.mActualGlyphRight  - mEffectsState.mActualGlyphLeft);
    mEffectsState.mGlyphMetrics.mfSizeY     = (float)(mEffectsState.mActualGlyphBottom - mEffectsState.mActualGlyphTop);
    mEffectsState.mGlyphMetrics.mfHBearingX = (float)(mEffectsState.mActualGlyphLeft   - mEffectsState.mImagePenX);
    mEffectsState.mGlyphMetrics.mfHBearingY = (float)(mEffectsState.mImagePenY         - mEffectsState.mActualGlyphTop);
    mEffectsState.mGlyphMetrics.mfHAdvanceX = xAdvance;

    // Glyphs that have no initial size are deemed to be intentionally invisible and 
    // thus we don't apply any modifications to them. We may need to distinguish
    // between intentionally non-spacing glyphs (e.g. ZWG) and simply invisible 
    // glyphs (e.g. space). The latter have advance while the former do not.
    // We always apply the metrics changes for fixed-pitch fonts so they continue to
    // act as fixed-pitch.

    const bool bFixedPitch = (mEffectsState.mpOutlineFont->mFontDescription.mPitch == kPitchFixed);

    if((mEffectsState.mGlyphMetrics.mfSizeX != 0.f) || bFixedPitch)
    {
        // Non-advancing glyphs but non-zero width glyphs are presumed to be special 
        // glyphs such as diacritics, and so we don't want to modify their advance.
        if(mEffectsState.mGlyphMetrics.mfHAdvanceX != 0.f)
        {
            // Apply any explicit metrics modifiers applied by the user.
            // Account for mEffectsState.mGlyphLeftSpace (which may be zero).
            mEffectsState.mGlyphMetrics.mfHBearingX += mEffectsState.mGlyphLeftSpace;
            mEffectsState.mGlyphMetrics.mfHAdvanceX += mEffectsState.mGlyphLeftSpace;

            // Account for mEffectsState.mGlyphRightSpace (which may be zero).
            mEffectsState.mGlyphMetrics.mfHAdvanceX += mEffectsState.mGlyphRightSpace;
        }
    }

    // Do some sanity checks on the resulting metrics.
    #if EATEXT_DEBUG
        if(bGlyphDrawn)
        {
            EA_ASSERT(mEffectsState.mGlyphMetrics.mfSizeX     >= 0.f    &&
                      mEffectsState.mGlyphMetrics.mfSizeX     <  1000.f &&
                      mEffectsState.mGlyphMetrics.mfSizeY     >= 0.f    &&
                      mEffectsState.mGlyphMetrics.mfSizeY     <  1000.f &&
                      mEffectsState.mGlyphMetrics.mfHBearingX >= -100.f &&
                      mEffectsState.mGlyphMetrics.mfHBearingX <  1000.f &&
                      mEffectsState.mGlyphMetrics.mfHBearingY >= -100.f &&
                      mEffectsState.mGlyphMetrics.mfHBearingY <  1000.f &&
                      mEffectsState.mGlyphMetrics.mfHAdvanceX >=    0.f &&
                      mEffectsState.mGlyphMetrics.mfHAdvanceX <  1000.f);
        }
    #endif

    // Possibly call the user plugin saying that this glyph is done.
    if(mpEffectsPlugin)
        mpEffectsPlugin->ProcessEffect(kEUIEnd, this, &mEffectsState, mpUserData);

    // Free the glyph
    SetCurrentGlyphState(kGlyphNotDrawn);
}


///////////////////////////////////////////////////////////////////////////////
// BuildBrush
//
// Consult the 'EATextEffects Brushes.png' file in the EAText documentation
// for a picture of these brushes.
//
void EffectsProcessor::BuildBrush()
{
    if(mEffectsState.mBrush[0].mDX == kBrushEnd) // If the brush is empty (not built)...
    {
        int   n = 0;
        int   nBrushSize = (mEffectsState.mGlyphBrushSize <= kEffectMaxBrushSize) ? (int)mEffectsState.mGlyphBrushSize : (int)kEffectMaxBrushSize;
        float fOpacity = mEffectsState.mGlyphBrushOpacity;
        int   x, y, yBegin, yEnd;

        // If the brush is a small round brush (which is square) or any kind of square brush then we do a simple 2D loop.
        if((mEffectsState.mGlyphBrushSize <= 4) || (mEffectsState.mGlyphBrushShape == kBSSquare))
        {
            for(yBegin = -nBrushSize / 2, y = yBegin, yEnd = yBegin + nBrushSize; y < yEnd; ++y)
            {
                for(x = yBegin; x < yEnd; ++x, ++n) // We intentionally use x = yBegin and x < yEnd. This is because x and y are symmetrical about 0.
                {
                    mEffectsState.mBrush[n].mDX    = (int8_t)x;
                    mEffectsState.mBrush[n].mDY    = (int8_t)y;
                    mEffectsState.mBrush[n].mAlpha = 255;
                }
            }

            if(mEffectsState.mGlyphBrushShape == kBSRound)
            {
                if(mEffectsState.mGlyphBrushSize == 3) // Size 3 round brushes are square but have just a tad of alpha at the four corners.
                {
                    mEffectsState.mBrush[0].mAlpha = 
                    mEffectsState.mBrush[2].mAlpha = 
                    mEffectsState.mBrush[6].mAlpha = 
                    mEffectsState.mBrush[8].mAlpha = (int8_t)(255 * mEffectsState.mGlyphBrushHardness);
                }
                else if(mEffectsState.mGlyphBrushSize == 4) // Size 4 round brushes are square but are absent at the four corners.
                {
                    mEffectsState.mBrush[ 0].mAlpha = 
                    mEffectsState.mBrush[ 3].mAlpha = 
                    mEffectsState.mBrush[12].mAlpha = 
                    mEffectsState.mBrush[15].mAlpha = 0;
                }
            }
            else
            {
                // We follow the convention that a square brush is not hard simply contributes to opacity.
                fOpacity = mEffectsState.mGlyphBrushOpacity * mEffectsState.mGlyphBrushHardness;
            }
        }
        else
        {
            // In this case we have size 5+ round brushes. These are uncommon, and we take a little more time to build them.
            const float fInverseRadiusSquared = 1.f / /*sqrtf*/(((float)nBrushSize / 2) * ((float)nBrushSize / 2));

            for(yBegin = -nBrushSize / 2, y = yBegin, yEnd = yBegin + nBrushSize; y < yEnd; ++y)
            {
                for(x = yBegin; x < yEnd; ++x) // We intentionally use x = yBegin and x < yEnd. We can do this because x and y are symmetrical about 0.
                {
                    const float fDistanceFromCenterSquared = /*sqrtf*/((float)(y * y) + (float)(x * x));
                    const float fClosenessToCenter         = 1.f - (fDistanceFromCenterSquared * fInverseRadiusSquared);
                    const float fAlpha                     = (fClosenessToCenter * mEffectsState.mGlyphBrushHardness * 255);

                    //if(fAlpha > 0)
                    {
                        mEffectsState.mBrush[n].mDX    = (int8_t)x;
                        mEffectsState.mBrush[n].mDY    = (int8_t)y;
                        //mEffectsState.mBrush[n].mAlpha = (int8_t)fAlpha;
                        mEffectsState.mBrush[n].mAlpha = (int8_t)((fAlpha > 0) ? fAlpha : 0.f);
                        ++n;
                    }
                }
            }
        }

        // Possibly apply the opacity setting.
        if(fOpacity != 1.f)
        {
            for(int i = 0; i < n; i++)
                mEffectsState.mBrush[i].mAlpha = (int8_t)(mEffectsState.mBrush[i].mAlpha * fOpacity);
        }

        // Set the end indicator, which tells where the end of the brush data is.
        mEffectsState.mBrush[n].mDX = kBrushEnd;
    }
}


///////////////////////////////////////////////////////////////////////////////
// DrawGlyphSmearOutline
//
// In this case we draw the glyph onto mEffectsState.mEffectsBitmaps.mFloatImage by convolving 
// the brush with the glyph. This means that for every pixel in the glyph,
// we apply the brush to the destination. Another way of looking at it is 
// that we draw the glyph repeatedly in a smeared pattern, with the pattern 
// being the brush. 
// The brush is multiplied against the glyph. If the brush has 50% transparency
// then the glyph is drawn with all its pixels 50% more transparent than 
// otherwise. 
// The drawing of the modulated glyph is done by increasing the opacity of
// the mEffectsState.mEffectsBitmaps.mFloatImage, just like with the DrawGlyph function. 
// The only difference is that the glyph we are copying to mEffectsState.mEffectsBitmaps.mFloatImage 
// may be more transparent than it normally would be.
//
// In order to avoid bouncing between the FPU and the CPU, we do it on the CPU 
// with fixed point math below.
//
// To consider: Merge this function with the DrawGlyph function, 
// as they share a similar body.
//
void EffectsProcessor::DrawGlyphSmearOutline()
{
    SetCurrentGlyphState(mEffectsState.mGlyphSmooth ? kGlyphDrawnGray : kGlyphDrawnMono);  // This should rebuild the glyph only if not already built.
    BuildBrush(); // This should build the brush only if not already built.

    #if EATEXT_USE_FREETYPE
        FT_GlyphSlot pGlyphSlot = mEffectsState.mpOutlineFont->mpFaceData->mpFTFace->glyph;
        EA_ASSERT(pGlyphSlot);

        const int32_t srcWidth = pGlyphSlot->bitmap.width;
    #endif

    if(srcWidth)
    {
        #if EATEXT_USE_FREETYPE
        const int32_t  srcHeight             = pGlyphSlot->bitmap.rows;
        const int32_t  xBearing              = pGlyphSlot->bitmap_left;
        const int32_t  yBearing              = pGlyphSlot->bitmap_top;
        const int32_t  nPitch                = pGlyphSlot->bitmap.pitch;
        const uint8_t* pBits                 = pGlyphSlot->bitmap.buffer;
        #endif

        const int32_t destBufferGlyphBaseX  = (mEffectsState.mImagePenX + xBearing) + mEffectsState.mGlyphDX;
        const int32_t destBufferGlyphBaseY  = (mEffectsState.mImagePenY - yBearing) + mEffectsState.mGlyphDY;
        const int32_t destBufferGlyphL      = destBufferGlyphBaseX + (-(int32_t)mEffectsState.mGlyphBrushSize / 2);
        const int32_t destBufferGlyphT      = destBufferGlyphBaseY + (-(int32_t)mEffectsState.mGlyphBrushSize / 2);
        const int32_t destBufferGlyphR      = destBufferGlyphL + srcWidth  + mEffectsState.mGlyphBrushSize;
        const int32_t destBufferGlyphB      = destBufferGlyphT + srcHeight + mEffectsState.mGlyphBrushSize;

        // We don't clear mEffectsState.mEffectsBitmaps.mFloatImage, as we assume the user did a manual Clear on it previously. 
        for(EffectsState::BrushPixel* pBrushPixel = mEffectsState.mBrush; pBrushPixel->mDX != kBrushEnd; ++pBrushPixel) // For each pixel in the brush...
        {
            const int32_t nBrushAlpha = pBrushPixel->mAlpha;

            if(nBrushAlpha >= (int32_t)mEffectsState.mGlyphMinAlpha) // If this pixel of the brush has any opacity...
            {
                const int32_t  destBufferGlyphX = destBufferGlyphBaseX + pBrushPixel->mDX;
                const int32_t  destBufferGlyphY = destBufferGlyphBaseY + pBrushPixel->mDY;
                const int32_t  height           = eastl::min_alt(srcHeight, (int32_t)(mEffectsState.mEffectsBitmaps.mFloatImage.mnHeight - destBufferGlyphY));
                const uint8_t* pSrc8Row         = pBits;
                const uint8_t* pSrc8RowEnd      = pSrc8Row + (height * nPitch);
                uint8_t*       pDest8Row        = mEffectsState.mEffectsBitmaps.mFloatImage.GetPixelPtrChecked(destBufferGlyphX, destBufferGlyphY);

                if(pDest8Row) // If the starting position is within mFloatImage... (else don't draw the glyph at all)
                {
                    const int32_t width = eastl::min_alt((int32_t)srcWidth, (int32_t)(mEffectsState.mEffectsBitmaps.mFloatImage.mnWidth - destBufferGlyphX));

                    if(mEffectsState.mGlyphSmooth) // If the glyph data is 8 bpp (and thus directly matches mFloatImage) ...
                    {
                        while(pSrc8Row < pSrc8RowEnd) // For each row...
                        {
                            const uint8_t* pSrc8    = pSrc8Row;
                            const uint8_t* pSrc8End = pSrc8Row + width;
                            uint8_t*       pDest8   = pDest8Row;
                            int32_t        glyphAlpha;

                            while(pSrc8 < pSrc8End) // For each pixel in the current row...
                            {
                                // EA_ASSERT((pDest8 >= mEffectsState.mEffectsBitmaps.mFloatImage.mpData) && (pDest8 < (mEffectsState.mEffectsBitmaps.mFloatImage.mpData + (mEffectsState.mEffectsBitmaps.mFloatImage.mnWidth * mEffectsState.mEffectsBitmaps.mFloatImage.mnHeight))));

                                if(*pSrc8) // It will fairly often be that *pSrc is 0 (empty pixel), enough to make this check worthwhile.
                                {
                                    if(*pSrc8 == 255)               // It will fairly often be that *pSrc is 255 (completely solid pixel).
                                        glyphAlpha = nBrushAlpha;
                                    else if(nBrushAlpha == 255)     // Smaller brushes will be at least half solid.
                                        glyphAlpha = *pSrc8;
                                    else
                                    {
                                        glyphAlpha = (*pSrc8 * nBrushAlpha) + 1;                // Multiply glyph opacity * brush opacity. +1 to deal with roundoff error.
                                        glyphAlpha = ((glyphAlpha + (glyphAlpha >> 8)) >> 8);   // Divide by 255, fixed point. This is a trick shifting version of this.
                                    }

                                    int32_t a = ((255 - glyphAlpha) * (255 - *pDest8)) + 1;     // Multiply src transparency * dest transparency. +1 to deal with roundoff error.
                                            a = ((a + (a >> 8)) >> 8);                          // Divide by 255, fixed point. This is a trick shifting version of this.
                                    *pDest8   = (uint8_t)(255 - a);                             // Set the destination to the new opacity (opacity => 255 - transparency).
                                }

                                ++pSrc8;
                                ++pDest8;
                            }

                            pSrc8Row  += nPitch;
                            pDest8Row += mEffectsState.mEffectsBitmaps.mFloatImage.mnStride;
                        }
                    }
                    else // Else the glyph data is 1 bpp (and thus also bit packed)
                    {
                        while(pSrc8Row < pSrc8RowEnd) // For each row...
                        {
                            const uint8_t*  pSrc8       = pSrc8Row;
                            uint8_t*        pDest8      = pDest8Row;
                            const uint8_t*  pDest8End   = pDest8 + width;
                            uint32_t        nSourceMask = 0x00000080;

                            while(pDest8 < pDest8End) // For each pixel in the current row...
                            {
                                // EA_ASSERT((pDest8 >= mEffectsState.mEffectsBitmaps.mFloatImage.mpData) && (pDest8 < (mEffectsState.mEffectsBitmaps.mFloatImage.mpData + (mEffectsState.mEffectsBitmaps.mFloatImage.mnWidth * mEffectsState.mEffectsBitmaps.mFloatImage.mnHeight))));

                                if(*pSrc8 & nSourceMask)
                                {
                                    if(nBrushAlpha == 255)  // Smaller brushes will have a lot of 255 pixels, and solid brushes will have all 255 pixels.
                                        *pDest8 = 255;
                                    else
                                    {
                                        int32_t a = ((255 - nBrushAlpha) * (255 - *pDest8)) + 1;    // Multiply src transparency * dest transparency. +1 to deal with roundoff error.
                                                a = ((a + (a >> 8)) >> 8);                          // Divide by 255, fixed point. This is a trick shifting version of this.
                                        *pDest8   = (uint8_t)(255 - a);                             // Set the destination to the new opacity (opacity => 255 - transparency).
                                    }
                                }

                                if((nSourceMask >>= 1) == 0)
                                {
                                    ++pSrc8;
                                    nSourceMask = 0x00000080;
                                }

                                ++pDest8;
                            }

                            pSrc8Row  += nPitch;
                            pDest8Row += mEffectsState.mEffectsBitmaps.mFloatImage.mnStride;
                        }
                    }
                }
            }
        }

        // Update the glyph bounds.
        if(mEffectsState.mActualGlyphLeft   > destBufferGlyphL)
           mEffectsState.mActualGlyphLeft   = destBufferGlyphL;
        if(mEffectsState.mActualGlyphTop    > destBufferGlyphT)
           mEffectsState.mActualGlyphTop    = destBufferGlyphT;
        if(mEffectsState.mActualGlyphRight  < destBufferGlyphR)
           mEffectsState.mActualGlyphRight  = destBufferGlyphR;
        if(mEffectsState.mActualGlyphBottom < destBufferGlyphB)
           mEffectsState.mActualGlyphBottom = destBufferGlyphB;

        mEffectsState.mbFloatImageClear = false;
    }
    else
    {
        if(mEffectsState.mActualGlyphLeft == kPosUnset)
        {
            mEffectsState.mActualGlyphLeft   = mEffectsState.mImagePenX;
            mEffectsState.mActualGlyphTop    = mEffectsState.mImagePenY;
            mEffectsState.mActualGlyphRight  = mEffectsState.mImagePenX;
            mEffectsState.mActualGlyphBottom = mEffectsState.mImagePenY;
        }
    }
}


///////////////////////////////////////////////////////////////////////////////
// DrawGlyphBrushOutline
//
void EffectsProcessor::DrawGlyphBrushOutline()
{
    // What we want to do here is implement a span list-based renderer, 
    // at least for glyphs that are >= size 20 (at 96 dpi). For the 
    // time being, we revert to a slower smear-based draw.
    DrawGlyphSmearOutline();
}


///////////////////////////////////////////////////////////////////////////////
// DrawGlyph
//
// In this function we draw a single unmodified glyph.
// Recall that mEffectsState.mEffectsBitmaps.mFloatImage is an alpha map or luminescence map.
// Thus a zero means the pixel is transparent (or dim) and 255 means it is opaque (or bright).
// We draw the glyph on top of whatever is currently at mEffectsState.mEffectsBitmaps.mFloatImage.
// Often mEffectsState.mEffectsBitmaps.mFloatImage will be clear (all zeroes). If so then we 
// simply copy the glyph to mFloatImage. 
// If mEffectsState.mEffectsBitmaps.mFloatImage is not clear, then we add our glyph to it. 
// In adding to it we are making it more opaque (or bright). 
// It's conceptually more correct to think in terms of opacity instead of 
// brightness because opacity is conceptually a 0% to 100% thing whereas brightness
// is conceptually a 0 to infinity thing. But computers are often stuck putting 
// limits on brightness, so it works in practice.
//
// See:
//    http://keithp.com/~keithp/porterduff/p253-porter.pdf
//
// In order to avoid bouncing between the FPU and the CPU, we do it on the CPU 
// with fixed point math below.
//
// To consider: Merge this function with the DrawGlyphSmearOutline function, 
// as they share a similar body.
//
// To consider: There is a small optimization opportunity whereby we can bypass
// the pixel copying/merging loops below if the glyph we draw is going to be
// directly copied to the destination without modification such as a blur.
//
void EffectsProcessor::DrawGlyph()
{
    SetCurrentGlyphState(mEffectsState.mGlyphSmooth ? kGlyphDrawnGray : kGlyphDrawnMono);  // This should rebuild the glyph only if not already built.

    #if EATEXT_USE_FREETYPE
        FT_GlyphSlot pGlyphSlot = mEffectsState.mpOutlineFont->mpFaceData->mpFTFace->glyph;
        EA_ASSERT(pGlyphSlot);

        const int32_t srcWidth = pGlyphSlot->bitmap.width;
    #endif

    if(srcWidth)
    {
        #if EATEXT_USE_FREETYPE
        const int32_t  srcHeight             = pGlyphSlot->bitmap.rows;
        const int32_t  xBearing              = pGlyphSlot->bitmap_left;
        const int32_t  yBearing              = pGlyphSlot->bitmap_top;
        const int32_t  nPitch                = pGlyphSlot->bitmap.pitch;
        const uint8_t* pBits                 = pGlyphSlot->bitmap.buffer;
        #endif

        const int32_t  destBufferGlyphL = (mEffectsState.mImagePenX + xBearing) + mEffectsState.mGlyphDX;
        const int32_t  destBufferGlyphT = (mEffectsState.mImagePenY - yBearing) + mEffectsState.mGlyphDY;
        const int32_t  destBufferGlyphR = destBufferGlyphL + srcWidth;
        const int32_t  destBufferGlyphB = destBufferGlyphT + srcHeight;
        const int32_t  height           = eastl::min_alt(srcHeight, (int32_t)(mEffectsState.mEffectsBitmaps.mFloatImage.mnHeight - destBufferGlyphT));
        const uint8_t* pSrc8Row         = pBits;
        const uint8_t* pSrc8RowEnd      = pSrc8Row + (height * nPitch);
        uint8_t*       pDest8Row        = mEffectsState.mEffectsBitmaps.mFloatImage.GetPixelPtrChecked(destBufferGlyphL, destBufferGlyphT); // The entire destination rect should always be valid.

        if(pDest8Row) // If the starting position is within mFloatImage... (else don't draw the glyph at all)
        {
            const int width = eastl::min_alt((int)srcWidth, (int)(mEffectsState.mEffectsBitmaps.mFloatImage.mnWidth - destBufferGlyphL));

            if(mEffectsState.mGlyphSmooth)
            {
                while(pSrc8Row < pSrc8RowEnd) // For each row...
                {
                    const uint8_t* pSrc8  = pSrc8Row;
                    uint8_t*       pDest8 = pDest8Row;

                    if(mEffectsState.mbFloatImageClear)       // This will always be so for at least the first glyph draw.
                        memcpy(pDest8, pSrc8, width);
                    else // Else we need to combine each pixel
                    {
                        const uint8_t* pSrc8End = pSrc8Row + width;

                        while(pSrc8 < pSrc8End) // For each pixel in the current row...
                        {
                            EA_ASSERT((pDest8 >= mEffectsState.mEffectsBitmaps.mFloatImage.mpData) && (pDest8 < (mEffectsState.mEffectsBitmaps.mFloatImage.mpData + (mEffectsState.mEffectsBitmaps.mFloatImage.mnWidth * mEffectsState.mEffectsBitmaps.mFloatImage.mnHeight))));

                            if(*pSrc8) // It will fairly often be that *pSrc is 0 (empty pixel), enough to make this check worthwhile.
                            {
                                if(*pSrc8 == 255)   // It will fairly often be that *pSrc is 255 (completely solid pixel).
                                    *pDest8 = 255;
                                else
                                {
                                    int32_t a = ((255 - *pSrc8) * (255 - *pDest8)) + 1; // Multiply src transparency * dest transparency. +1 to deal with roundoff error.
                                            a = ((a + (a >> 8)) >> 8);                  // Divide by 255, fixed point. This is a trick shifting version of this.
                                      *pDest8 = (uint8_t)(255 - a);                     // Set the destination to the new opacity (opacity => 255 - transparency).
                                }
                            }

                            ++pSrc8;
                            ++pDest8;
                        }
                    }

                    pSrc8Row  += nPitch;
                    pDest8Row += mEffectsState.mEffectsBitmaps.mFloatImage.mnStride;
                }
            }
            else
            {
                while(pSrc8Row < pSrc8RowEnd) // For each row...
                {
                    const uint8_t*  pSrc8       = pSrc8Row;
                    uint8_t*        pDest8      = pDest8Row;
                    const uint8_t*  pDest8End   = pDest8 + width;
                    uint32_t        nSourceMask = 0x00000080;

                    while(pDest8 < pDest8End) // For each pixel in the current row...
                    {
                        EA_ASSERT((pDest8 >= mEffectsState.mEffectsBitmaps.mFloatImage.mpData) && (pDest8 < (mEffectsState.mEffectsBitmaps.mFloatImage.mpData + (mEffectsState.mEffectsBitmaps.mFloatImage.mnWidth * mEffectsState.mEffectsBitmaps.mFloatImage.mnHeight))));

                        if(*pSrc8 & nSourceMask)        // Since all source pixels are completely opaque or completely transparent,
                            *pDest8 = 255;              // we simply copy them to the destination. There is nothing to modulate.

                        if((nSourceMask >>= 1) == 0)
                        {
                            ++pSrc8;
                            nSourceMask = 0x00000080;
                        }

                        ++pDest8;
                    }

                    pSrc8Row  += nPitch;
                    pDest8Row += mEffectsState.mEffectsBitmaps.mFloatImage.mnStride;
                }
            }
        }

        // Update the glyph bounds.
        if(mEffectsState.mActualGlyphLeft   > destBufferGlyphL)
           mEffectsState.mActualGlyphLeft   = destBufferGlyphL;
        if(mEffectsState.mActualGlyphTop    > destBufferGlyphT)
           mEffectsState.mActualGlyphTop    = destBufferGlyphT;
        if(mEffectsState.mActualGlyphRight  < destBufferGlyphR)
           mEffectsState.mActualGlyphRight  = destBufferGlyphR;
        if(mEffectsState.mActualGlyphBottom < destBufferGlyphB)
           mEffectsState.mActualGlyphBottom = destBufferGlyphB;

        mEffectsState.mbFloatImageClear = false;
    }
    else
    {
        if(mEffectsState.mActualGlyphLeft == kPosUnset)
        {
            mEffectsState.mActualGlyphLeft   = mEffectsState.mImagePenX;
            mEffectsState.mActualGlyphTop    = mEffectsState.mImagePenY;
            mEffectsState.mActualGlyphRight  = mEffectsState.mImagePenX;
            mEffectsState.mActualGlyphBottom = mEffectsState.mImagePenY;
        }
    }
}


///////////////////////////////////////////////////////////////////////////////
// GetCurrentGlyphState
//
EffectsProcessor::GlyphState EffectsProcessor::GetCurrentGlyphState() const
{
    return mGlyphState;
}


///////////////////////////////////////////////////////////////////////////////
// SetCurrentGlyphState
//
// This function redraws the glyph if needed in the new state. If there is no
// change then this function does nothing. 
//
bool EffectsProcessor::SetCurrentGlyphState(GlyphState glyphState)
{
    bool bReturnValue = true;

    const GlyphState glyphStateCurrent = GetCurrentGlyphState();

    if(glyphStateCurrent != glyphState)
    {
        if(glyphState != kGlyphNotDrawn)
        {
            #if EATEXT_USE_FREETYPE
                FT_Error nFTError = FT_Activate_Size(mEffectsState.mpOutlineFont->mpFTSize);
                EA_ASSERT(nFTError == 0); (void)nFTError;

                // FreeType is a little confusing about load flags, load target flags and render flags. 
                // You pass FT_LOAD and FT_LOAD_TARGET flags to FT_Load_Glyph, and you
                // pass FT_RENDER flags to FT_Render_Glyph. Its a little confusing because their names
                // overlap in ways that make it easy to get confused about the effect of each.

                int32_t loadFlags       = 0;        // Any of FT_LOAD_DEFAULT, FT_LOAD_NO_HINTING, FT_LOAD_NO_BITMAP, FT_LOAD_FORCE_AUTOHINT, FT_LOAD_PEDANTIC.
                int32_t loadTargetFlags = 0;        // One of FT_LOAD_TARGET_NORMAL, FT_LOAD_TARGET_LCD or FT_LOAD_TARGET_LIGHT. Don't want FT_LOAD_TARGET_MONO, as that targets the hinting algorithm at a true monochrome video screen and is not the same as rendering in mono.

                if(mEffectsState.mpOutlineFont->mbUseAutoHinting)
                    loadFlags |= FT_LOAD_FORCE_AUTOHINT;
                else if(!mEffectsState.mpOutlineFont->mbEnableHinting)
                    loadFlags |= FT_LOAD_NO_HINTING;

                if(mEffectsState.mpOutlineFont->mbLCD)
                    loadTargetFlags = FT_LOAD_TARGET_LCD;
                else
                    loadTargetFlags = FT_LOAD_TARGET_NORMAL;

                nFTError = FT_Load_Glyph(mEffectsState.mpOutlineFont->mpFaceData->mpFTFace, mEffectsState.mGlyphId, loadFlags | loadTargetFlags);
                EA_ASSERT(nFTError == 0); (void)nFTError;

                if(nFTError == 0)
                {
                    const FT_Render_Mode renderMode = (mEffectsState.mpOutlineFont->mbLCD ? FT_RENDER_MODE_LCD : FT_RENDER_MODE_NORMAL); // FT_RENDER_MODE_NORMAL, FT_RENDER_MODE_LIGHT, FT_RENDER_MODE_MONO, FT_RENDER_MODE_LCD

                    nFTError = FT_Render_Glyph(mEffectsState.mpOutlineFont->mpFaceData->mpFTFace->glyph, renderMode);
                    EA_ASSERT(nFTError == 0); (void)nFTError;
                }

                bReturnValue = (nFTError == 0);
            #endif

            if(bReturnValue && (mEffectsState.mImagePenX == kPenUnset)) // If OK and if we haven't created mFloatImage/mBaseImage yet...
                SetupImages(); // We setup images here after the first glyph draw because we need to know the actual glyph dimensions prior to setting up images.
        }

        mGlyphState = glyphState;
    }

    return bReturnValue;
}


///////////////////////////////////////////////////////////////////////////////
// SetupImages
//
void EffectsProcessor::SetupImages()
{
    EA_ASSERT(mEffectsState.mImagePenX == kPenUnset);

    #if EATEXT_USE_FREETYPE
        FT_GlyphSlot pGlyphSlot = mEffectsState.mpOutlineFont->mpFaceData->mpFTFace->glyph;
        EA_ASSERT(pGlyphSlot);

        const int32_t srcWidth  = pGlyphSlot->bitmap.width;
        const int32_t srcHeight = pGlyphSlot->bitmap.rows;
        const int32_t xBearing  = pGlyphSlot->bitmap_left;
        const int32_t yBearing  = pGlyphSlot->bitmap_top;
    #endif

    const int imageWidth  = srcWidth  + (mEffectsState.mRequiredGutterSpace * 2);
    const int imageHeight = srcHeight + (mEffectsState.mRequiredGutterSpace * 2);

    mEffectsState.mEffectsBitmaps.mFloatImage.SetSize(imageWidth, imageHeight);
    mEffectsState.mEffectsBitmaps.mFloatImage.Clear();
    mEffectsState.mbFloatImageClear = true;

    mEffectsState.mEffectsBitmaps.mBaseImage.SetSize(imageWidth, imageHeight);
    mEffectsState.mEffectsBitmaps.mBaseImage.Clear();
    mEffectsState.mbBaseImageClear = true;

    // Set the glyph dimensions to be a rect with impossible dimensions so they are reset during any compare.
    mEffectsState.mActualGlyphLeft = kPosUnset;
    mEffectsState.mActualGlyphTop  = kPosUnset;
    //mEffectsState.mActualGlyphRight  = 0;  It would be redundant to set this to zero.
    //mEffectsState.mActualGlyphBottom = 0;

    // Set pen position for this set of instructions.
    mEffectsState.mImagePenX = ((mEffectsState.mEffectsBitmaps.mBaseImage.mnWidth  / 2) - (srcWidth  / 2)) - xBearing;
    mEffectsState.mImagePenY = ((mEffectsState.mEffectsBitmaps.mBaseImage.mnHeight / 2) - (srcHeight / 2)) + yBearing;
}


///////////////////////////////////////////////////////////////////////////////
// Clear
//
void EffectsProcessor::Clear()
{
    if(mEffectsState.mTargetImage == kETIFloat)
    {
        mEffectsState.mEffectsBitmaps.mFloatImage.Clear();
        mEffectsState.mbFloatImageClear = true;
    }
    else
    {
        mEffectsState.mEffectsBitmaps.mBaseImage.Clear();
        mEffectsState.mbBaseImageClear = true;
    }
}


///////////////////////////////////////////////////////////////////////////////
// ApplyCurve
//
// We operate directly on mEffectsState.mEffectsBitmaps.mFloatImage in place.
//
void EffectsProcessor::ApplyCurve(uint32_t curveType, float /*fStrength*/)
{
    using namespace Effects;

    if(mEffectsState.mTargetImage == kETIFloat)
    {
        const uint8_t* pCurveSamples;

        if(curveType == kECTS)
            pCurveSamples = kSSamples;
        else if(curveType == kECTF)
            pCurveSamples = kFSamples;
        else // kECTJ
            pCurveSamples = kJSamples;

        uint8_t* pDstRow = mEffectsState.mEffectsBitmaps.mFloatImage.mpData;

        for(int y = 0, yEnd = mEffectsState.mEffectsBitmaps.mFloatImage.mnHeight; y < yEnd; y++)
        {
            for(int x = 0, xEnd = mEffectsState.mEffectsBitmaps.mFloatImage.mnWidth; x < xEnd; x++)
            {
                const int c = pDstRow[x];

                // We expect that a lot of pixels will be zero.
                // We make the assumption that pCurveSamples[0] is always zero, as it is for our j, s, f curves.
                // Thus we do nothing to c if it is already zero.
                if(c != 0) 
                {
                    const int   index  = c / kSampleRange;                                      // Convert 0-255 range to 0-31 range.
                    const float fValue = (c - (index * kSampleRange)) * kOneDivSampleCount;     // fValue is in the range of [0, 1).
                    const float fSlope = (pCurveSamples[index + 1] - pCurveSamples[index]) * kOneDivSampleRange;

                    pDstRow[x] = (uint8_t)(unsigned)RoundToInt32(pCurveSamples[index] + ((fValue * fSlope) * kSampleRange));
                }
            }

            pDstRow += mEffectsState.mEffectsBitmaps.mFloatImage.mnStride;
        }
    }
    else // (mEffectsState.mTargetImage == kETIBase)
    {
        // We don't yet support applying this to the ARGB dest image, though really 
        // it isn't very hard to implement. It's just a matter of priorities.
        EA_FAIL_MSG("EffectsProcessor::ApplyCurve: Not supported.");
    }
}


///////////////////////////////////////////////////////////////////////////////
// ApplyBoxBlur
//
// Box blur info:
//     http://www.gamasutra.com/features/20010209/evans_01.htm
//
// Gaussian blur info:
//     http://en.wikipedia.org/wiki/Gaussian_blur
//     http://ciri.be/blog/?p=14
// 
void EffectsProcessor::ApplyBoxBlur(float fRadius, float gain, uint32_t passCount)
{
    using namespace Effects;

    if(mEffectsState.mTargetImage == kETIFloat)
    {
        // We skip blurring if the image is empty partly for performance, but more important so that the glyph 
        // metrics calculations aren't modified. A blurred empty glyph is still empty, and we should know that.
        if(!mEffectsState.mbFloatImageClear)
        {
            const int   nRadius = RoundToInt32(fRadius); // We don't currently have a means to implement a fractional radius.
            const float mul     = (gain / (((nRadius * 2) + 1) * ((nRadius * 2) + 1)));
            const int   srcW    = mEffectsState.mEffectsBitmaps.mFloatImage.mnWidth;          // To do: Reduce the blur bounds to include just the used sub-area of mFloatImage.
            const int   srcH    = mEffectsState.mEffectsBitmaps.mFloatImage.mnHeight;
            int         x, y;

            mEffectsState.mEffectsBitmaps.mBlurPrecalcSumImage.SetSize(srcW, srcH);
            mEffectsState.mEffectsBitmaps.mBlurPrecalcSumImage.Clear();

            for(uint32_t i = 0; i < passCount; ++i)
            {
                // We do the common optimization of generating a sum precalculation.
                uint8_t*  pSrcRow8     = mEffectsState.mEffectsBitmaps.mFloatImage.mpData;
                uint8_t*  pSrcRow8End  = mEffectsState.mEffectsBitmaps.mFloatImage.mpData + (mEffectsState.mEffectsBitmaps.mFloatImage.mnHeight * mEffectsState.mEffectsBitmaps.mFloatImage.mnStride);
                uint32_t* pDestRow32   = mEffectsState.mEffectsBitmaps.mBlurPrecalcSumImage.mpData;
                uint8_t*  pDestRow8    = mEffectsState.mEffectsBitmaps.mFloatImage.mpData;

                // Generate the precalc buffer.
                EA_ASSERT((mEffectsState.mEffectsBitmaps.mBlurPrecalcSumImage.mnStride) == mEffectsState.mEffectsBitmaps.mBlurPrecalcSumImage.mnWidth * 4); // The code below assumes that (pixel stride == pixel width).

                while(pSrcRow8 < pSrcRow8End)
                {
                    const uint8_t* pSrc8    = pSrcRow8;
                    const uint8_t* pSrc8End = pSrcRow8 + mEffectsState.mEffectsBitmaps.mFloatImage.mnWidth;
                    uint32_t*      pDest32  = pDestRow32;

                    while(pSrc8 < pSrc8End)
                    {
                        uint32_t sum = *pSrc8;

                        if(pSrc8 > pSrcRow8) // If the x position > 0...
                        {
                            //EA_ASSERT((pDest32 - 1) >= mEffectsState.mEffectsBitmaps.mBlurPrecalcSumImage.mpData);
                            sum += pDest32[-1];
                        }

                        if(pSrcRow8 > pDestRow8) // If the y position > 0...
                        {
                            //EA_ASSERT((pDest32 - srcW) >= mEffectsState.mEffectsBitmaps.mBlurPrecalcSumImage.mpData);
                            sum += pDest32[-srcW];

                            if(pSrc8 > pSrcRow8) // If the x position > 0...
                            {
                                //EA_ASSERT((pDest32 - srcW - 1) >= mEffectsState.mEffectsBitmaps.mBlurPrecalcSumImage.mpData);
                                sum -= pDest32[-srcW - 1];
                            }
                        }

                        ++pSrc8;
                        *pDest32++ = sum;
                    }

                    pSrcRow8   += mEffectsState.mEffectsBitmaps.mFloatImage.mnStride / sizeof(uint8_t);
                    pDestRow32 += mEffectsState.mEffectsBitmaps.mBlurPrecalcSumImage.mnStride / sizeof(uint32_t);
                }


                // Write the blurred image into mEffectsState.mEffectsBitmaps.mFloatImage over its previous image.
                // mEffectsState.mEffectsBitmaps.mFloatImage.Clear();      We will be overwriting every non-zero pixel, so it's redundant to clear.
                // mEffectsState.mbFloatImageClear = true; Pointless to do this because we will write on it below.
                y = 0;

                while(pDestRow8 < pSrcRow8End)
                {
                    uint8_t*       pDest8    = pDestRow8;
                    const uint8_t* pDest8End = pDestRow8 + mEffectsState.mEffectsBitmaps.mFloatImage.mnWidth;
                    x = 0;

                    while(pDest8 < pDest8End)
                    {
                        const int   br    = mEffectsState.mEffectsBitmaps.mBlurPrecalcSumImage.GetPixelClamped(x + nRadius,     y + nRadius    );
                        const int   tl    = mEffectsState.mEffectsBitmaps.mBlurPrecalcSumImage.GetPixelClamped(x - nRadius - 1, y - nRadius - 1);
                        const int   bl    = mEffectsState.mEffectsBitmaps.mBlurPrecalcSumImage.GetPixelClamped(x - nRadius - 1, y + nRadius    );
                        const int   tr    = mEffectsState.mEffectsBitmaps.mBlurPrecalcSumImage.GetPixelClamped(x + nRadius,     y - nRadius - 1);
                        const int   sum   = +br +tl -bl -tr; 
                        const float fVal  = (sum * mul);
                        const int   iVal  = RoundToInt32(fVal);

                        *pDest8++ = (uint8_t)((iVal < 256) ? iVal : 255);
                        ++x;
                    }

                    pDestRow8 += mEffectsState.mEffectsBitmaps.mFloatImage.mnStride / sizeof(uint8_t);
                    ++y;
                }
            }

            // Adjust the actual glyph metrics based on the blur parameters. 
            // We probably need to do a more involved calculation whereby we
            // look at the pixel data and compare to mGlyphMinAlpha instead of 
            // trying to calculate the bounds.
            const int32_t expansion = (int32_t)(fRadius * gain / 2 * (float)(int32_t)passCount); // We may need to tweak this calculation; it's a 'guesstimate.'
            mEffectsState.mGlyphExpansion += expansion;
        }
    }
    else // (mEffectsState.mTargetImage == kETIBase)
    {
        // We don't yet support applying this to the ARGB dest image, though really 
        // it isn't very hard to implement. It's just a matter of priorities.
        EA_FAIL_MSG("EffectsProcessor::ApplyBoxBlur: Not supported.");
    }
}


///////////////////////////////////////////////////////////////////////////////
// Merge
//
// We blend the source 8 bit alphamap to the destination 32 bit ARGB pixmap.
//
void EffectsProcessor::Merge()
{
    using namespace Effects;

    EA_ASSERT(((mEffectsState.mEffectsBitmaps.mBaseImage.mnWidth  >= mEffectsState.mEffectsBitmaps.mFloatImage.mnWidth) && 
               (mEffectsState.mEffectsBitmaps.mBaseImage.mnHeight >= mEffectsState.mEffectsBitmaps.mFloatImage.mnHeight)) && mEffectsState.mEffectsBitmaps.mFloatImage.mpData && mEffectsState.mEffectsBitmaps.mBaseImage.mpData);

    const uint32_t cAlpha       = (mEffectsState.mGlyphColor & 0xff000000) >> 24;
    const uint32_t cSolid       = mEffectsState.mGlyphColor | 0xff000000;
    const uint32_t cTransparent = mEffectsState.mGlyphColor & 0x00ffffff;
    const uint8_t* pSrcRow8     = mEffectsState.mEffectsBitmaps.mFloatImage.mpData;
    const uint8_t* pSrcRow8End  = mEffectsState.mEffectsBitmaps.mFloatImage.mpData + (mEffectsState.mEffectsBitmaps.mFloatImage.mnHeight * mEffectsState.mEffectsBitmaps.mFloatImage.mnStride);
    uint32_t*      pDestRow32   = mEffectsState.mEffectsBitmaps.mBaseImage.mpData;

    while(pSrcRow8 < pSrcRow8End)
    {
        const uint8_t* pSrc8    = pSrcRow8;
        const uint8_t* pSrc8End = pSrcRow8 + mEffectsState.mEffectsBitmaps.mFloatImage.mnWidth;
        uint32_t*      pDest32  = pDestRow32;

        while(pSrc8 < pSrc8End)
        {
            int32_t aSrc;

            // If source color alpha == 0xff then just overwrite the dest with the source color (cSolid).
            // If source color alpha != 0xff then multiply the source color alpha with the source glyph alpha at the given pixel.
            if(cAlpha == 0xff)  // This will often be so with plain glyph draws.
                aSrc = *pSrc8;
            else
            {   // The following implements (aSrc = *pSrc8 * cAlpha), where the values are [0-255] and represent fractions from [0.0-1.0]
                aSrc = ((*pSrc8 * cAlpha) + 1);         // Multiply glyph opacity * brush opacity. +1 to deal with roundoff error.
                aSrc = ((aSrc + (aSrc >> 8)) >> 8);     // Divide by 255, fixed point. This is a trick shifting version of this.
            }

            if(aSrc == 255)         // This will often be so for a lot of the pixels in a glyph.
                *pDest32 = cSolid;
            else if(aSrc != 0)
            {
                const uint32_t argbSrc = ((uint32_t)aSrc << 24) | cTransparent;

                if(*pDest32)    // If the dest has anything on it... do a combine.
                    *pDest32 = CombineColors(argbSrc, *pDest32);
                else            // Else just do a copy.
                    *pDest32 = argbSrc;
            }

            ++pSrc8;
            ++pDest32;
        }

        pSrcRow8   += mEffectsState.mEffectsBitmaps.mFloatImage.mnStride  / sizeof(uint8_t);
        pDestRow32 += mEffectsState.mEffectsBitmaps.mBaseImage.mnStride / sizeof(uint32_t);
    }

    mEffectsState.mbBaseImageClear = mEffectsState.mbFloatImageClear;  // If SrcImage is clear then so will DestImage be.
}


///////////////////////////////////////////////////////////////////////////////
// AdjustFontMetrics
//
// Must be called right after rendering a glyph. This is because we use the
// results of a glyph render to calculate the metrics. 
//
void EffectsProcessor::AdjustFontMetrics(FontMetrics& fontMetrics)
{
    // If we haven't executed yet, we run though just the instructions we 
    // need to tell the font metrics.
    if(mEffectsState.mpCurrentInstruction == mEffectsState.mInstructionList)
    {
        for(const EIWord* pWord = mEffectsState.mInstructionList; pWord < mEffectsState.mInstructionList + mEffectsState.mInstructionListSize; )
        {
            const uint32_t instruction = (pWord++)->mU32;

            switch(instruction)
            {
                case kEISetGlyphHSpace:
                    mEffectsState.mGlyphLeftSpace  = (pWord++)->mI32;
                    mEffectsState.mGlyphRightSpace = (pWord++)->mI32;
                    break;

                case kEISetGlyphVSpace:
                    mEffectsState.mGlyphTopSpace    = (pWord++)->mI32;
                    mEffectsState.mGlyphBottomSpace = (pWord++)->mI32;
                    break;

                case kEISetGlyphBrush:          // These instructions have four arguments.
                    ++pWord;
                    // Fall through

                case kEIApplyBoxBlur:           // These instructions have three arguments.
                    ++pWord;
                    // Fall through

                case kEISetExpansion:
                case kEISetGlyphOffset:         // These instructions have two arguments.
                case kEIApplyCurve:
                    ++pWord;
                    // Fall through

                case kEISetGlyphMinAlpha:       // These instructions have one argument.
                case kEISetGlyphColor:
                case kEISetGlyphSmooth:
                case kEISetTargetImage:
                case kEISetTargetMask:
                case kEICallUser:
                    ++pWord;
                    // Fall through

                case kEIDrawGlyphSmearOutline:  // These instructions have no arguments.
                case kEIDrawGlyphBrushOutline:
                case kEIDrawGlyph:
                case kEIClear:
                case kEIMerge:
                    break;
            }
        }
    }

    // Leave fontMetrics.mfSize alone. It does not explicitly refer to any specific pixel metric.
    // Leave fontMetrics.mPitch alone. It is unaffected by this.
    fontMetrics.mfHAdvanceXMax      += mEffectsState.mGlyphLeftSpace + mEffectsState.mGlyphRightSpace;
    // Leave fontMetrics.mfVAdvanceYMax alone. We don't currently support vertical metrics.
    fontMetrics.mfAscent            += mEffectsState.mGlyphTopSpace;
    fontMetrics.mfDescent           -= mEffectsState.mGlyphBottomSpace;
    // Leave mfLeading alone. It is unaffected by this.
    fontMetrics.mfBaseline          += mEffectsState.mGlyphTopSpace;
    fontMetrics.mfLineHeight        = fontMetrics.mfBaseline - fontMetrics.mfDescent;
    fontMetrics.mfXHeight           += mEffectsState.mGlyphTopSpace;
    fontMetrics.mfCapsHeight        += mEffectsState.mGlyphTopSpace;
    fontMetrics.mfUnderlinePosition -= mEffectsState.mGlyphBottomSpace;
    // Leave fontMetrics.mfUnderlineThickness alone.
    // Leave fontMetrics.fontMetrics.mfLinethroughPosition alone.
    // Leave fontMetrics.mfLinethroughThickness alone.
    fontMetrics.mfOverlinePosition  += mEffectsState.mGlyphTopSpace;
    // Leave fontMetrics.mfOverlineThickness alone.
}




#if EATEXT_DEBUG
    void EffectsProcessor::DebugPrint()
    {
        char filePath[64];

        sprintf(filePath, "C:\\temp\\glyph\\SrcImage_%u.ppm", mEffectsState.mpCurrentInstruction - mEffectsState.mInstructionList);
        mEffectsState.mEffectsBitmaps.mFloatImage.WritePPMFile(filePath, 0x00);

        sprintf(filePath, "C:\\temp\\glyph\\DestImage_%u.ppm", mEffectsState.mpCurrentInstruction - mEffectsState.mInstructionList);
        mEffectsState.mEffectsBitmaps.mBaseImage.WritePPMFile(filePath, 0xff000000);
    }
#endif


} // namespace Text

} // namespace EA



// Undefine values defined earlier.
#undef FFFixed16ToFloat
#undef FFFixed26ToFloat








