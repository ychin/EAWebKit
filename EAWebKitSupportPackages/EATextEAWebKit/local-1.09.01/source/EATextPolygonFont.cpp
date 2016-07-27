/*
Copyright (C) 2004, 2009, 2010, 2012 Electronic Arts, Inc.  All rights reserved.

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
// EATextPolygonFont.cpp
//
// By Paul Pedriana - 2004
//
///////////////////////////////////////////////////////////////////////////////


#include <EAText/EATextPolygonFont.h>
#include <EAText/EATextFont.h>
#include <EAIO/EAStreamMemory.h>
#include <EAIO/EAStreamAdapter.h>
#include <EAText/internal/StdC.h>
#include <EASTL/string.h>
#include <EASTL/fixed_string.h>
#include <stdio.h>
#include EA_ASSERT_HEADER


#ifdef _MSC_VER
    #pragma warning(push)
    #pragma warning(disable: 4061) // enumerate in switch of enum is not explicitly handled by a case label.
#endif


namespace EA
{

namespace Text
{

namespace PolygonFontInternal
{
    inline uint16_t ReadFromBigEndianUint16(const void* pData)
    {
        return (uint16_t)((uint32_t)(((const uint8_t*)pData)[0] << 8) | 
                          (uint32_t)(((const uint8_t*)pData)[1] << 0));
    }

    inline uint32_t ReadFromBigEndianUint32(const void* pData)
    {
        return (uint32_t)((uint32_t)(((const uint8_t*)pData)[0] << 24) | 
                          (uint32_t)(((const uint8_t*)pData)[1] << 16) | 
                          (uint32_t)(((const uint8_t*)pData)[2] <<  8) | 
                          (uint32_t)(((const uint8_t*)pData)[3] <<  0));
    }

    inline float ReadFromBigEndianFloat(const void* pData)
    {
        union FloatUint32 { uint32_t i; float f; } u;

        // Working with floating point values is safe here because 
        // at no time does the FPU see an invalid swizzled float.
        u.i = ReadFromBigEndianUint32(pData);
        return u.f;
    }
}



// FontFileLineReader
//
// Reads lines from a font file/stream consectively.
// This reader always reads linearly in the file unless a pathological situation occurs.
//
struct FontFileLineReader
{
    // This space may be too large for some stacks. 
    // To consider: Hook this up to the user-supplied allocator.
    static const uint32_t kBufferSize = 4095;
    eastl::fixed_string<char8_t,  kBufferSize + 1, true> mBuffer8;

    EA::IO::size_type ReadLine(EA::IO::IStream* pIS);
};


// ReadLine
//
// This is a variation of the IO::ReadLine function. It's purpose is to deal with 
// the possibility of binary data on a line. We could encode the binary data in some 
// kind of encoding format, but that would increase the size of the file, but file
// size is very important to us, as it affects application load times from DVD.
// 
// We just read lines as per the IO::ReadLine function. The only difference is that 
// there is an additional provision to have the line begin with the line size instead
// of the text of the line. If the line begins with a \t char, then the folowing eight 
// digits are presumed to the 0-terminated integral length of the remaining characters
// on the line, including the terminating \n.
// Example file with the third line using an explicit line size:
//    blah: abcd\n
//    blah: abcd\n
//    \t      11\0blah: abcd\n
//    blah: abcd\n
//    blah: abcd\n
EA::IO::size_type FontFileLineReader::ReadLine(EA::IO::IStream* pIS)
{
    using namespace EA::IO;

    char8_t   cCurrent;
    size_type nCount(0); // Number of chars in the line, not including the line end characters(s).
    size_type nRead(0);  // Number of chars successfully read from stream. Will be >= nCount (due to presence of newlines).
    size_type nResult;

    mBuffer8.resize(0);

    for(;;)
    {
        nResult = pIS->Read(&cCurrent, sizeof(cCurrent));

        if(nResult == sizeof(cCurrent))
        {
            ++nRead;

            if((nRead == 1) && (cCurrent == '\t')) // If this is the first character and it is a tab...
            {
                // We read the entire rest of the line in one shot
                char lineSize[8];

                pIS->Read(&lineSize, 8);
                nCount = atoi(lineSize);
                mBuffer8.resize((eastl_size_t)nCount);

                pIS->Read((char*)mBuffer8.data(), nCount);

                return nCount;
            }

            if((cCurrent == '\r') || (cCurrent == '\n'))
            {
                if(cCurrent == '\r') // If we have a "\r", then we read again, expecting a "\n".
                {
                    char8_t cNext;
                    nResult = (size_type)pIS->Read(&cNext, sizeof(cNext));
                }

                break;
            }
            else
            {
                mBuffer8.push_back(cCurrent);
                ++nCount;
            }
        }
        else // Else we are done.
        {
            if(nRead == 0)
                nCount = kSizeTypeDone;

            break;
        }
    }

    return nCount;
}




PolygonGlyph::PolygonGlyph()
  : mnBackFaceVertexStart(0),
    mnBackFaceIndexStart(0),
    mVertexArray(EATEXT_ALLOC_PREFIX "PolygonGlyph"),
    mIndexArray(EATEXT_ALLOC_PREFIX "PolygonGlyph")
{
    // Empty
}


PolygonFont::PolygonFont(Allocator::ICoreAllocator* pCoreAllocator)
  : Font(pCoreAllocator),
    mbUserGlyphInfo(false),
    mFontDescription(),
    mFontMetrics(),
    mGlyphBitmap(),
    mGlyphMetricsMap(EA::Allocator::EASTLICoreAllocator(EATEXT_ALLOC_PREFIX "PolygonFont/GlyphMetricsMap", mpCoreAllocator)),
    mPolygonGlyphMap(EA::Allocator::EASTLICoreAllocator(EATEXT_ALLOC_PREFIX "PolygonFont/GlyphMetricsMap", mpCoreAllocator)),
    mCharGlyphMap   (EA::Allocator::EASTLICoreAllocator(EATEXT_ALLOC_PREFIX "PolygonFont/GlyphMetricsMap", mpCoreAllocator))
{
    // Empty
}


PolygonFont::~PolygonFont()
{
    PolygonFont::Close(); 
}


void PolygonFont::SetAllocator(Allocator::ICoreAllocator* pCoreAllocator)
{
    Font::SetAllocator(pCoreAllocator);

    mGlyphMetricsMap.get_allocator().set_allocator(pCoreAllocator);
    mPolygonGlyphMap.get_allocator().set_allocator(pCoreAllocator);
    mCharGlyphMap   .get_allocator().set_allocator(pCoreAllocator);
}


void PolygonFont::SetOption(int32_t option, int32_t value)
{
    if(option == kOptionUserGlyphInfo)
        mbUserGlyphInfo = (value != 0);
}


bool PolygonFont::Open(IO::IStream* pStream)
{
    //using namespace EA::StdC;
    using namespace EA::Text::PolygonFontInternal;

    bool                bResult = false;
    IO::size_type       size;
    eastl_size_t        pos;
    const IO::size_type posSaved = (IO::size_type)pStream->GetPosition();
    FontFileLineReader  reader;

    // Long lines wrap to the next line, so we shouldn't run out of space during the read.
    while((size = reader.ReadLine(pStream)) < EA::IO::kSizeTypeDone)    
    {
        pos = reader.mBuffer8.find(':');

        if(pos < reader.mBuffer8.length())
        {
            // Move past any whitespace after the ':' char.
            reader.mBuffer8[pos] = 0;
            pos = reader.mBuffer8.find_first_not_of(" \t", pos + 1);
            const char8_t* pData8 = reader.mBuffer8.c_str() + (pos < reader.mBuffer8.length() ? pos : reader.mBuffer8.length());
            
            if(Stricmp(reader.mBuffer8.c_str(), "Family") == 0)
                Strlcpy(mFontDescription.mFamily, pData8, kFamilyNameCapacity);
            else if(Stricmp(reader.mBuffer8.c_str(), "Size") == 0)
                mFontDescription.mfSize = (float)Atof(pData8);
            else if(Stricmp(reader.mBuffer8.c_str(), "Style") == 0)
            {
                if(Stristr(pData8, "italic"))
                    mFontDescription.mStyle = kStyleItalic;
                else if(Stristr(pData8, "oblique"))
                    mFontDescription.mStyle = kStyleOblique;
                else
                    mFontDescription.mStyle = kStyleNormal;
            }
            else if(Stricmp(reader.mBuffer8.c_str(), "Weight") == 0)
                mFontDescription.mfWeight = (float)Atof(pData8);
            else if(Stricmp(reader.mBuffer8.c_str(), "Stretch") == 0)
                mFontDescription.mfStretch = (float)Atof(pData8);
            else if(Stricmp(reader.mBuffer8.c_str(), "Smooth") == 0)
            {
                if(Stristr(pData8, "Yes"))
                    mFontDescription.mSmooth = kSmoothEnabled;
                else
                    mFontDescription.mSmooth = kSmoothNone;
            }
            else if(Stricmp(reader.mBuffer8.c_str(), "Variant") == 0)
            {
                if(Stristr(pData8, "SmallCaps"))
                    mFontDescription.mVariant = kVariantSmallCaps;
                else
                    mFontDescription.mVariant = kVariantNormal;
            }
            else if(Stricmp(reader.mBuffer8.c_str(), "FixedPitch") == 0)
            {
                if(Stristr(pData8, "Enabled") == 0)
                {
                    mFontMetrics.mPitch     = kPitchFixed; 
                    mFontDescription.mPitch = kPitchFixed;
                }
                else
                {
                    mFontMetrics.mPitch     = kPitchVariable; 
                    mFontDescription.mPitch = kPitchVariable;
                }
            }
            else if(Stricmp(reader.mBuffer8.c_str(), "HAdvanceXMax") == 0)
                mFontMetrics.mfHAdvanceXMax = (float)Atof(pData8);
            else if(Stricmp(reader.mBuffer8.c_str(), "VAdvanceYMax") == 0)
                mFontMetrics.mfVAdvanceYMax = (float)Atof(pData8);
            else if(Stricmp(reader.mBuffer8.c_str(), "Ascent") == 0)
                mFontMetrics.mfAscent = (float)Atof(pData8);
            else if(Stricmp(reader.mBuffer8.c_str(), "Descent") == 0)
                mFontMetrics.mfDescent = (float)Atof(pData8);
            else if(Stricmp(reader.mBuffer8.c_str(), "Leading") == 0)
                mFontMetrics.mfLeading = (float)Atof(pData8);
            else if(Stricmp(reader.mBuffer8.c_str(), "Baseline") == 0)
                mFontMetrics.mfBaseline = (float)Atof(pData8);
            else if(Stricmp(reader.mBuffer8.c_str(), "LineHeight") == 0)
                mFontMetrics.mfLineHeight = (float)Atof(pData8);
            else if(Stricmp(reader.mBuffer8.c_str(), "XHeight") == 0)
                mFontMetrics.mfXHeight = (float)Atof(pData8);
            else if(Stricmp(reader.mBuffer8.c_str(), "CapsHeight") == 0)
                mFontMetrics.mfCapsHeight = (float)Atof(pData8);
            else if(Stricmp(reader.mBuffer8.c_str(), "UnderlinePosition") == 0)
                mFontMetrics.mfUnderlinePosition = (float)Atof(pData8);
            else if(Stricmp(reader.mBuffer8.c_str(), "UnderlineThickness") == 0) // bmpFont files previously misspelled this as "UnderlingThickness".
                mFontMetrics.mfUnderlineThickness = (float)Atof(pData8);
            else if(Stricmp(reader.mBuffer8.c_str(), "StrikethroughPosition") == 0)
                mFontMetrics.mfLinethroughPosition = (float)Atof(pData8);
            else if(Stricmp(reader.mBuffer8.c_str(), "StrikethroughThickness") == 0)
                mFontMetrics.mfLinethroughThickness = (float)Atof(pData8);
            else if(Stricmp(reader.mBuffer8.c_str(), "GlyphMetricsMap") == 0)
            {
                bool    bShouldReadAnotherLine;
                //int32_t nEntryCount = AtoI328(pData8); // We don't need this.

                while(*pData8 != ' ') // Skip past the entry count digits.
                    pData8++;

                do{ // Read each line
                    bShouldReadAnotherLine = false;

                    do{ // Read the entries within each line.
                        unsigned        glyphId;
                        GlyphMetrics    gm;
                        const int       nFieldCount = sscanf(pData8, "%u %f %f %f %f %f", 
                                                                        &glyphId, &gm.mfSizeX, &gm.mfSizeY, 
                                                                        &gm.mfHBearingX, &gm.mfHBearingY, &gm.mfHAdvanceX);
                        (void)nFieldCount;
                        EA_ASSERT(nFieldCount == 6);
                        mGlyphMetricsMap.insert(GlyphMetricsMap::value_type((GlyphId)glyphId, gm));

                        pData8 = Strchr(pData8, ',');
                    } while(pData8++ && (pData8 < (reader.mBuffer8.data() + reader.mBuffer8.length())));

                    // To read multi-line glyph metric map.
                    if(reader.mBuffer8[(eastl_size_t)size - 1] == ',')
                    {
                        bShouldReadAnotherLine = true;

                        size = reader.ReadLine(pStream);
                        pData8 = reader.mBuffer8.c_str();
                    }
                } while(bShouldReadAnotherLine);
            }
            else if(Stricmp(reader.mBuffer8.c_str(), "CharMapSet") == 0)
            {
                bool bShouldReadAnotherLine;

                while(*pData8 != ' ') // Skip past the entry count digits.
                    pData8++;

                do{ // Read each line
                    bShouldReadAnotherLine = false;

                    do{ // Read the entries within each line.
                        unsigned character;
                        unsigned glyphId; 

                        const int nFieldCount = sscanf(pData8, "%u %u", &character, &glyphId);
                        (void)nFieldCount;
                        EA_ASSERT(nFieldCount == 2);
                        mCharGlyphMap.insert(CharGlyphMap::value_type((Char)character, (GlyphId)glyphId));

                        pData8 = Strchr(pData8, ',');
                    } while(pData8++ && (pData8 < (reader.mBuffer8.data() + reader.mBuffer8.length())));

                    // To read multi-line glyph metric map.
                    if(reader.mBuffer8[(eastl_size_t)size - 1] == ',')
                    {
                        bShouldReadAnotherLine = true;

                        size = reader.ReadLine(pStream);
                        pData8 = reader.mBuffer8.c_str();
                    }
                } while(bShouldReadAnotherLine);
            }
            else if(Stristr(reader.mBuffer8.c_str(), "Glyph") == reader.mBuffer8.c_str())
            {
                if(!mbUserGlyphInfo)
                {
                    Allocator::ICoreAllocator* const pAllocator = mPolygonGlyphMap.get_allocator().get_allocator();

                    const uint32_t glyphId = AtoU32(reader.mBuffer8.c_str() + 5); // reader.mBuffer8 looks something like "Glyph37: # # # #, array array"

                    unsigned vertexArraySize;
                    unsigned indexArraySize;
                    unsigned backFaceVertexStart;
                    unsigned backFaceIndexStart;

                    const int nFieldCount = sscanf(pData8, "%u %u %u %u", &vertexArraySize, &indexArraySize, &backFaceVertexStart, &backFaceIndexStart);
                    EA_ASSERT(nFieldCount == 4); (void)nFieldCount;

                    // Set up the PolygonGlyph
                    PolygonGlyphPtr& glyphPtr = mPolygonGlyphMap[(uint16_t)glyphId];
                    glyphPtr.mpPolygonGlyph = (PolygonGlyph*)pAllocator->Alloc(sizeof(PolygonGlyph), "PolygonGlyph", 0);

                    if(glyphPtr.mpPolygonGlyph)
                    {
                        // Use placement new to construct the PolygonGlyph.
                        glyphPtr.mpPolygonGlyph = new(glyphPtr.mpPolygonGlyph) PolygonGlyph;

                        if(vertexArraySize) // Some glyphs have no geometry (e.g. the space char)
                        {
                            PolygonGlyph& glyph = *glyphPtr.mpPolygonGlyph;

                            glyph.mVertexArray.resize(vertexArraySize);
                            glyph.mIndexArray.resize(indexArraySize);

                            // Skip past the above-read values.
                            pData8 = Strchr(pData8, ',') + 1;

                            // Copy the vertex data.
                            const float* pVertexData = (const float*)pData8;

                            for(uint32_t v = 0; v < vertexArraySize; v++)
                            {
                                glyph.mVertexArray[v].mX  = ReadFromBigEndianFloat(pVertexData++);
                                glyph.mVertexArray[v].mY  = ReadFromBigEndianFloat(pVertexData++);
                                glyph.mVertexArray[v].mZ  = ReadFromBigEndianFloat(pVertexData++);
                                glyph.mVertexArray[v].mNX = ReadFromBigEndianFloat(pVertexData++);
                                glyph.mVertexArray[v].mNY = ReadFromBigEndianFloat(pVertexData++);
                                glyph.mVertexArray[v].mNZ = ReadFromBigEndianFloat(pVertexData++);
                            }

                            // Copy the index data.
                            const uint16_t* const pIndexData = (const uint16_t*)(const char*)pVertexData;

                            for(eastl_size_t i = 0; i < indexArraySize; i++)
                                glyph.mIndexArray[i] = ReadFromBigEndianUint16(pIndexData + i);
                        }
                    }
                }
            }
        }
    }

    bResult = (!mCharGlyphMap.empty() && !mGlyphMetricsMap.empty());

    if(bResult)
    {
        // Set up mSupportedScriptSet.
        SetupSupportedScriptSet();

        // Set up mReplacementGlyphId.
        mReplacementGlyphId = mGlyphMetricsMap.begin()->first;
        Char c = '_';
        GetGlyphIds(&c, 1, &mReplacementGlyphId, false);

        // Add a kGlyphIdZeroWidth glyph.
        GlyphMetrics gmZeroWidth;
        memset(&gmZeroWidth, 0, sizeof(gmZeroWidth));
        mGlyphMetricsMap.insert(GlyphMetricsMap::value_type(kGlyphIdZeroWidth, gmZeroWidth));

        // We don't create a mPolygonGlyphMap entry for kGlyphIdZeroWidth, as we simply use the glyph for the space char.
    }

    // Restore the original stream position.
    pStream->SetPosition((EA::IO::off_type)posSaved);

    return bResult;
}


bool PolygonFont::Open(const void* pSourceData, uint32_t nSourceSize)
{
    Allocator::ICoreAllocator* const pAllocator = mPolygonGlyphMap.get_allocator().get_allocator();

    IO::MemoryStream memoryStream((void*)pSourceData, nSourceSize, true, false, pAllocator);

    return Open(&memoryStream);
}


bool PolygonFont::Close()
{
    if(!mbUserGlyphInfo) // If we allocated the info (and the user didn't)...
    {
        Allocator::ICoreAllocator* const pAllocator = mPolygonGlyphMap.get_allocator().get_allocator();

        for(PolygonGlyphMap::iterator it = mPolygonGlyphMap.begin(); it != mPolygonGlyphMap.end(); ++it)
        {
            PolygonGlyphPtr& polygonGlyphPtr = it->second;

            polygonGlyphPtr.mpPolygonGlyph->~PolygonGlyph();
            pAllocator->Free(polygonGlyphPtr.mpPolygonGlyph, sizeof(PolygonGlyphPtr));
        }
    }

    return true;
}


Font::BitmapFormat PolygonFont::GetBitmapFormat() const
{
    return kBFNone;
}


FontType PolygonFont::GetFontType()
{
    return kFontTypePolygon;
}


OTF* PolygonFont::GetOTF()
{
    return NULL; // Currently we have no means to access the originating .ttf OpenType information.
}


bool PolygonFont::GetFontDescription(FontDescription& fontDescription)
{
    fontDescription = mFontDescription;
    return true;
}


bool PolygonFont::GetFontMetrics(FontMetrics& fontMetrics)
{
    fontMetrics = mFontMetrics;
    return true;
}


bool PolygonFont::GetGlyphMetrics(GlyphId glyphId, GlyphMetrics& glyphMetrics)
{
    const GlyphMetricsMap::const_iterator it = mGlyphMetricsMap.find(glyphId);

    if(it != mGlyphMetricsMap.end()) // If found...
    {
        const GlyphMetrics& glyphMetricsTemp = (*it).second;
        glyphMetrics = glyphMetricsTemp;

        return true;
    }

    return false;
}


uint32_t PolygonFont::GetGlyphIds(const Char* pCharArray, uint32_t nCharArrayCount, GlyphId* pGlyphIdArray, 
                                    bool bUseReplacementGlyph, const uint32_t nGlyphIdStride, bool bWriteInvalidGlyphs)
{
    GlyphId     glyphId;    // Scratch memory in case pGlyphIdArray is NULL.
    const Char* pChar       = pCharArray;
    const Char* pCharEnd    = pCharArray + nCharArrayCount;
    GlyphId*    pGlyphId    = pGlyphIdArray ? pGlyphIdArray : &glyphId;
    uint32_t    glyphCount  = 0;

    while(pChar < pCharEnd)
    {
        CharGlyphMap::const_iterator it = mCharGlyphMap.find(*pChar);

        bool bGlyphOK = it != mCharGlyphMap.end();

        if(bGlyphOK) // The large majority of the time this will be true.
            *pGlyphId = it->second;
        else
        {
            bGlyphOK = true;

            if(IsCharZeroWidth(*pChar))
                *pGlyphId = kGlyphIdZeroWidth;
            else if(bUseReplacementGlyph)
                *pGlyphId = mReplacementGlyphId;
            else if(bWriteInvalidGlyphs)
                *pGlyphId = kGlyphIdInvalid;
            else
                bGlyphOK = false;
        }

        if(bGlyphOK)
        {
            glyphCount++;

            if(pGlyphIdArray)
                pGlyphId = (GlyphId*)((char*)pGlyphId + nGlyphIdStride);
        }

        pChar++;
    }

    return glyphCount;
}


uint32_t PolygonFont::GetGlyphIds(const char8_t* pCharArray, uint32_t nCharArrayCount, GlyphId* pGlyphIdArray, 
                                    bool bUseReplacementGlyph, const uint32_t nGlyphIdStride, bool bWriteInvalidGlyphs)
{
    GlyphId        glyphId;    // Scratch memory in case pGlyphIdArray is NULL.
    const char8_t* pChar       = pCharArray;
    const char8_t* pCharEnd    = pCharArray + nCharArrayCount;
    GlyphId*       pGlyphId    = pGlyphIdArray ? pGlyphIdArray : &glyphId;
    uint32_t       glyphCount  = 0;

    while(pChar < pCharEnd)
    {
        CharGlyphMap::const_iterator it = mCharGlyphMap.find((uint8_t)*pChar);

        bool bGlyphOK = it != mCharGlyphMap.end();

        if(bGlyphOK) // The large majority of the time this will be true.
            *pGlyphId = it->second;
        else
        {
            bGlyphOK = true;

            if(IsCharZeroWidth((uint8_t)*pChar))
                *pGlyphId = kGlyphIdZeroWidth;
            else if(bUseReplacementGlyph)
                *pGlyphId = mReplacementGlyphId;
            else if(bWriteInvalidGlyphs)
                *pGlyphId = kGlyphIdInvalid;
            else
                bGlyphOK = false;
        }

        if(bGlyphOK)
        {
            glyphCount++;

            if(pGlyphIdArray)
                pGlyphId = (GlyphId*)((char*)pGlyphId + nGlyphIdStride);
        }

        pChar++;
    }

    return glyphCount;
}


float PolygonFont::GetSize() const
{
    return mFontDescription.mfSize;
}


bool PolygonFont::SetTransform(float /*f00*/, float /*f01*/, float /*f10*/, float /*f11*/)
{
    // We currently don't support runtime transformation modifications.
    // We can implement this by simply transforming the points of all glyphs.
    return false;
}


bool PolygonFont::SetTransform(float /*fSize*/, float /*fAngle*/, float /*fSkew*/)
{
    // We currently don't support runtime transformation modifications.
    // We can implement this by simply transforming the points of all glyphs.
    return false;
}


void PolygonFont::SetUserGlyphInfo(GlyphId glyphId, void* pUserInfo)
{
    PolygonGlyphMap::iterator it = mPolygonGlyphMap.find(glyphId);

    if(it != mPolygonGlyphMap.end()) // If we have this glyph...
    {
        PolygonGlyphPtr& polygonGlyphPtr = (*it).second;
        polygonGlyphPtr.mpUserGlyph = pUserInfo;
    }
}


bool PolygonFont::RenderGlyphBitmap(const GlyphBitmap** pGlyphBitmap, GlyphId glyphId, 
                                        uint32_t /*renderFlags*/, float /*fXFraction*/, float /*fYFraction*/)
{
    PolygonGlyphMap::const_iterator it = mPolygonGlyphMap.find(glyphId);

    if(it != mPolygonGlyphMap.end()) // If we have this glyph...
    {
        const PolygonGlyphPtr& polygonGlyphPtr = (*it).second;

        // Copy the GlyphMetrics to the structure that will be returned to the caller.
        GetGlyphMetrics(glyphId, mGlyphBitmap.mGlyphMetrics);

        mGlyphBitmap.mnWidth       = (uint32_t)mGlyphBitmap.mGlyphMetrics.mfSizeX;
        mGlyphBitmap.mnHeight      = (uint32_t)mGlyphBitmap.mGlyphMetrics.mfSizeY;
        mGlyphBitmap.mBitmapFormat = kBFNone;
        mGlyphBitmap.mpData        = &polygonGlyphPtr;

        *pGlyphBitmap = &mGlyphBitmap;

        return true;
    }

    return false;
}


void PolygonFont::DoneGlyphBitmap(const GlyphBitmap* /*pGlyphBitmap*/)
{
    // Nothing to do.
}


bool PolygonFont::IsCharSupported(Char c, Script script)
{
    // We make the assumption here that all fonts support all ASCII characters.
    // This is an assumption that we can likely make for a font engine like this
    // which is used in conditions controlled by us and doesn't need to be 
    // fully arbitrary.
    if(c < 0x0080)
        return true;

    // Some scripts are directly tied to a specific language, and if a font
    // supports that script then it almost certainly supports all characters
    // of interest for both that script and the language associated with that
    // script. Examples of such scripts are Hebrew, Thai, Greek.
    // Some scripts work for multiple languages and thus if a font supports
    // such a script, it may not support all the languages for that script.
    // Examples of such scripts are Latin (English, French, Spanish, etc.) and 
    // Han (Japanese, Traditional Chinese, Simplified Chinese).
    switch (script)
    {
        default:
            return mSupportedScriptSet.test((size_t)script);

        case kScriptUnknown:
            if(c == kCharInvalid)
                return true; // In this case the user doesn't care if the char or script is supported.
            break;

        case kScriptHan:
        case kScriptLatin:
        case kScriptCommon:
            // With these scripts, we cannot possibly tell if a given 
            // character is supported based on the script alone.
            break;
    }

    return mCharGlyphMap.find(c) != mCharGlyphMap.end();
}


bool PolygonFont::GetKerning(GlyphId /*g1*/, GlyphId /*g2*/, Kerning& kerning, 
                                int /*direction*/, bool /*bHorizontalLayout*/)
{
    // We don't currently implement custom kerning at the font level. 
    // The only way we support kerning is via OpenType GPOS information 
    // applied to complex scripts. There's nothing preventing us from 
    // implementing such functionality here, though. We simply need to 
    // update the PolygonFont file format to provide font-level kerning info.

    kerning.mfKernX = 0.f;
    kerning.mfKernY = 0.f;

    return false;
}




} // namespace Text

} // namespace EA



#ifdef _MSC_VER
    #pragma warning(pop)
#endif

















