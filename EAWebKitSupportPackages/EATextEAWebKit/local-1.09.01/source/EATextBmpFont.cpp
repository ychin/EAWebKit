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
// EATextBmpFont.cpp
//
// By Sumedha Kshirsagar and Paul Pedriana.
///////////////////////////////////////////////////////////////////////////////


#include <EAText/EATextBmpFont.h>
#include <EAText/EATextUnicode.h>
#include <coreallocator/icoreallocatormacros.h>
#include <EAText/internal/StdC.h>
//#include <EAStdC/EAString.h>
//#include <EAStdC/EAScanf.h>
#include <EAIO/EAFileStream.h>
#include <EAIO/EAStreamMemory.h>
#include <EASTL/string.h>
#include <stdio.h>
#include EA_ASSERT_HEADER


namespace EA
{

namespace Text
{


///////////////////////////////////////////////////////////////////////////////
// BmpTextureInfo
///////////////////////////////////////////////////////////////////////////////

///////////////////////////////////////////////////////////////////////////////
// BmpTextureInfo
//
BmpTextureInfo::BmpTextureInfo()
  : mTextureFilePath(),
    mnTextureFileSize(0)
{
    mpSource   = mTextureFilePath.c_str();
    mbWritable = false;
}


///////////////////////////////////////////////////////////////////////////////
// ~BmpTextureInfo
//
BmpTextureInfo::~BmpTextureInfo()
{
}



///////////////////////////////////////////////////////////////////////////////
// BmpFont
///////////////////////////////////////////////////////////////////////////////

///////////////////////////////////////////////////////////////////////////////
// BmpFont
//
BmpFont::BmpFont(Allocator::ICoreAllocator* pCoreAllocator)
  : Font(pCoreAllocator),
    mFontDescription(),
    mFontMetrics(),
    mGlyphBitmap(),
    mBmpGlyphMetricsMap(EA::Allocator::EASTLICoreAllocator(EATEXT_ALLOC_PREFIX "BmpFont/BmpGlyphMetricsMap", mpCoreAllocator)),
    mKerningMap        (EA::Allocator::EASTLICoreAllocator(EATEXT_ALLOC_PREFIX "BmpFont/KerningMap",    mpCoreAllocator)),
    mTextureInfoArray(),
    mCharGlyphMap      (EA::Allocator::EASTLICoreAllocator(EATEXT_ALLOC_PREFIX "BmpFont/CharGlyphMap", mpCoreAllocator)),
    mpGlyphCache(NULL),
    mbEnableDirectGlyphCacheUse(false),  // Defaults to false because the application drawing system needs to be a little smarter to use it. UTFWin currently can't use it.
    mbSystemMemoryCopy(true)            // This is true for backwards compatibility. It will eventually be disabled.
{
    memset(&mGlyphBitmap, 0, sizeof(mGlyphBitmap));
}


///////////////////////////////////////////////////////////////////////////////
// ~BmpFont
//
BmpFont::~BmpFont()
{
    BmpFont::Close(); 
}


///////////////////////////////////////////////////////////////////////////////
// SetAllocator
//
void BmpFont::SetAllocator(Allocator::ICoreAllocator* pCoreAllocator)
{
    Font::SetAllocator(pCoreAllocator);

    mBmpGlyphMetricsMap.get_allocator().set_allocator(pCoreAllocator);
    mKerningMap        .get_allocator().set_allocator(pCoreAllocator);
    mCharGlyphMap      .get_allocator().set_allocator(pCoreAllocator);
}


///////////////////////////////////////////////////////////////////////////////
// GetFontType
//
FontType BmpFont::GetFontType()
{
    return kFontTypeBitmap;
}


///////////////////////////////////////////////////////////////////////////////
// GetGlyphCache
//
GlyphCache* BmpFont::GetGlyphCache() const
{
    return mpGlyphCache;
}


///////////////////////////////////////////////////////////////////////////////
// SetGlyphCache
//
void BmpFont::SetGlyphCache(GlyphCache* pGlyphCache)
{
    // Verify that the font isn't already open.
    EA_ASSERT(mBmpGlyphMetricsMap.empty());

    mpGlyphCache = pGlyphCache;
}


///////////////////////////////////////////////////////////////////////////////
// GetFontDescription
//
bool BmpFont::GetFontDescription(FontDescription& fontDescription)
{
    if(mFontDescription.mFamily[0]) // If the font info is valid...
    {
        fontDescription = mFontDescription;
        return true;
    }

    return false;
}


///////////////////////////////////////////////////////////////////////////////
// GetFontMetrics
//
bool BmpFont::GetFontMetrics(FontMetrics& fontMetrics)
{
    if(mFontDescription.mFamily[0]) // If the font info is valid...
    {
        fontMetrics = mFontMetrics;
        return true;
    }

    return false;
}


///////////////////////////////////////////////////////////////////////////////
// Open
//
bool BmpFont::Open(IO::IStream* pStreamArray[], uint32_t streamCount)
{
    EA_ASSERT((streamCount >= 1) && pStreamArray[0] && (pStreamArray[0]->GetAccessFlags() & IO::kAccessFlagRead));

    bool bReturnValue;

    if(!mBmpGlyphMetricsMap.empty()) // If already opened...
        bReturnValue = true;
    else
    {
        bReturnValue = false;

        if(pStreamArray[0])
        {
            bReturnValue = ReadBmpFontInfo(pStreamArray[0]);
            EA_ASSERT(bReturnValue);

            if(bReturnValue)
            {
                for(uint32_t i = 1; i < streamCount; i++) // Streams (1 to N) refer to i textures (0 to N-1).
                {
                    if(!ReadBmpTexture(pStreamArray[i], i - 1))
                    {
                        EA_FAIL_MESSAGE("BmpFont::Open: ReadBmpTexture failed.");
                        bReturnValue = false;
                        break;
                    }
                }
            }
        }

        if(bReturnValue)
        {
            // Add a kGlyphIdZeroWidth glyph.
            BmpGlyphMetrics bgm;
            memset(&bgm, 0, sizeof(bgm));
            mBmpGlyphMetricsMap.insert(BmpGlyphMetricsMap::value_type(kGlyphIdZeroWidth, bgm));

            SetupSupportedScriptSet();
            RegisterGlyphMetrics();
        }

        if(!bReturnValue)
            Close();
    }

    EA_ASSERT(bReturnValue);
    return bReturnValue;
}


///////////////////////////////////////////////////////////////////////////////
// RegisterGlyphMetrics
//
void BmpFont::RegisterGlyphMetrics()
{
    // To consider: Enable this assert. The problem is that previous versions of 
    // this class didn't really require mpGlyphCache to be valid at this point.
    // EA_ASSERT(mpGlyphCache);

    if(mpGlyphCache)
    {
        // Write mBmpGlyphMetricsMap to mpGlyphCache.
        //
        // To consider: This copying of data to the glyph cache results in data 
        //    that is more or less replicated between BmpFont and GlyphCache. 
        //    It would be better if we could store this in one place and give 
        //    the other a way to retrieve it. GlyphCache is a generic server
        //    which works with multiple font types, so it can't just hard-code
        //    a knowledge of BmpFont in it. Well maybe we can.

        for(BmpGlyphMetricsMap::const_iterator it = mBmpGlyphMetricsMap.begin(), itEnd = mBmpGlyphMetricsMap.end(); it != itEnd; ++it)
        {
            const BmpGlyphMetricsMap::value_type entry = *it;
            const BmpGlyphMetrics& bmpGM               = entry.second;
                  BmpTextureInfo*  pBmpTextureInfo     = mTextureInfoArray[(eastl_size_t)bmpGM.mnTextureIndex];

            const GlyphTextureInfo glyphTextureInfo = 
            {
                pBmpTextureInfo,
               (bmpGM.mnPositionX + bmpGM.mfHBearingX) * pBmpTextureInfo->mfSizeInverse,
               (bmpGM.mnPositionY - bmpGM.mfHBearingY) * pBmpTextureInfo->mfSizeInverse,
               (bmpGM.mnPositionX + bmpGM.mfHBearingX + bmpGM.mfSizeX) * pBmpTextureInfo->mfSizeInverse,
               (bmpGM.mnPositionY - bmpGM.mfHBearingY + bmpGM.mfSizeY) * pBmpTextureInfo->mfSizeInverse
            };

            mpGlyphCache->SetGlyphTextureInfo(this, entry.first, glyphTextureInfo);
        }
    }
}


///////////////////////////////////////////////////////////////////////////////
// Close
//
bool BmpFont::Close()
{
    mFontDescription.mFamily[0] = 0;

    mBmpGlyphMetricsMap.clear();
    mKerningMap.clear();
    mCharGlyphMap.clear();

    for(TextureInfoArray::iterator it = mTextureInfoArray.begin(); it != mTextureInfoArray.end(); ++it)
    {
        BmpTextureInfo* const pTI = *it;

        if(pTI)
            pTI->Release();
    }

    mTextureInfoArray.clear();

    return true;
}


void BmpFont::SetOption(int32_t option, int32_t value)
{
    // We don't lock any mutex because these options must be used before the font is opened.

    switch (option)
    {
        // Not applicable:
        //case kOptionOpenTypeFeatures:
        //case kOptionDPI:
        //case kOptionEnableHinting:
        //case kOptionUseAutoHinting:
        //case kOptionLCD:
        //    break;

        case kOptionEnableDirectGlyphCacheUse:
            mbEnableDirectGlyphCacheUse = (value != 0);
            break;

        case kOptionSystemMemoryCopy:
            mbSystemMemoryCopy = (value != 0);
            break;
    }
}


///////////////////////////////////////////////////////////////////////////////
// ReadBmpFontInfo
//
bool BmpFont::ReadBmpFontInfo(IO::IStream* pStream)
{
    IO::size_type  size;
    eastl_size_t   pos;
    const uint32_t kBufferSize = 512;

    eastl::fixed_string<char8_t,  kBufferSize + 1, true> sBuffer8(kBufferSize, 0);

    const IO::size_type streamPositionSaved = (IO::size_type)pStream->GetPosition();

    // Long lines wrap to the next line, so we shouldn't run out of space during the read.
    while((size = EA::IO::ReadLine(pStream, &sBuffer8[0], kBufferSize)) < EA::IO::kSizeTypeDone)    
    {
        sBuffer8.resize((eastl_size_t)size);
        pos = sBuffer8.find(':');

        if(pos < sBuffer8.length())
        {
            // Move past any whitespace after the ':' char.
            sBuffer8[pos] = 0;
            pos = sBuffer8.find_first_not_of(" \t", pos + 1);
            const char8_t* pData8 = sBuffer8.c_str() + (pos < sBuffer8.length() ? pos : sBuffer8.length());
            
            if(Stricmp(sBuffer8.c_str(), "Family") == 0)
                Strlcpy(mFontDescription.mFamily, pData8, kFamilyNameCapacity); // It's not a critical problem if there wasn't enough capacity for the full string.
            else if(Stricmp(sBuffer8.c_str(), "Size") == 0)
                mFontDescription.mfSize = (float)Atof(pData8);
            else if(Stricmp(sBuffer8.c_str(), "Style") == 0)
            {
                if(Stristr(pData8, "italic"))
                    mFontDescription.mStyle = kStyleItalic;
                else if(Stristr(pData8, "oblique"))
                    mFontDescription.mStyle = kStyleOblique;
                else
                    mFontDescription.mStyle = kStyleNormal;
            }
            else if(Stricmp(sBuffer8.c_str(), "Weight") == 0)
                mFontDescription.mfWeight = (float)Atof(pData8);
            else if(Stricmp(sBuffer8.c_str(), "Stretch") == 0)
                mFontDescription.mfStretch = (float)Atof(pData8);
            else if(Stricmp(sBuffer8.c_str(), "Smooth") == 0)
            {
                if(Stristr(pData8, "Yes"))
                    mFontDescription.mSmooth = kSmoothEnabled;
                else
                    mFontDescription.mSmooth = kSmoothNone;
            }
            else if(Stricmp(sBuffer8.c_str(), "Variant") == 0)
            {
                if(Stristr(pData8, "SmallCaps"))
                    mFontDescription.mVariant = kVariantSmallCaps;
                else
                    mFontDescription.mVariant = kVariantNormal;
            }
            else if(Stricmp(sBuffer8.c_str(), "FixedPitch") == 0)
            {
                if(Stristr(pData8, "Yes") == 0)
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
            else if(Stricmp(sBuffer8.c_str(), "HAdvanceXMax") == 0)
                mFontMetrics.mfHAdvanceXMax = (float)Atof(pData8);
            else if(Stricmp(sBuffer8.c_str(), "VAdvanceYMax") == 0)
                mFontMetrics.mfVAdvanceYMax = (float)Atof(pData8);
            else if(Stricmp(sBuffer8.c_str(), "Ascent") == 0)
                mFontMetrics.mfAscent = (float)Atof(pData8);
            else if(Stricmp(sBuffer8.c_str(), "Descent") == 0)
                mFontMetrics.mfDescent = (float)Atof(pData8);
            else if(Stricmp(sBuffer8.c_str(), "Leading") == 0)
                mFontMetrics.mfLeading = (float)Atof(pData8);
            else if(Stricmp(sBuffer8.c_str(), "Baseline") == 0)
                mFontMetrics.mfBaseline = (float)Atof(pData8);
            else if(Stricmp(sBuffer8.c_str(), "LineHeight") == 0)
                mFontMetrics.mfLineHeight = (float)Atof(pData8);
            else if(Stricmp(sBuffer8.c_str(), "XHeight") == 0)
                mFontMetrics.mfXHeight = (float)Atof(pData8);
            else if(Stricmp(sBuffer8.c_str(), "CapsHeight") == 0)
                mFontMetrics.mfCapsHeight = (float)Atof(pData8);
            else if(Stricmp(sBuffer8.c_str(), "UnderlinePosition") == 0)
                mFontMetrics.mfUnderlinePosition = (float)Atof(pData8);
            else if(Stricmp(sBuffer8.c_str(), "UnderlineThickness") == 0) // bmpFont files previously misspelled this as "UnderlingThickness".
                mFontMetrics.mfUnderlineThickness = (float)Atof(pData8);
            else if(Stricmp(sBuffer8.c_str(), "LinethroughPosition") == 0)
                mFontMetrics.mfLinethroughPosition = (float)Atof(pData8);
            else if(Stricmp(sBuffer8.c_str(), "LinethroughThickness") == 0)
                mFontMetrics.mfLinethroughThickness = (float)Atof(pData8);
            else if(Stricmp(sBuffer8.c_str(), "GlyphMetricsMap") == 0)
            {
                bool    bShouldReadAnotherLine;
                //int32_t nEntryCount = AtoI328(pData8);

                while(*pData8 != ' ')
                    pData8++;

                do{ // Read each line
                    bShouldReadAnotherLine = false;

                    do{ // Read the entries within each line.
                        unsigned        glyphId;
                        BmpGlyphMetrics bgm;
                        int             textureIndex, positionX, positionY; // Use temp variables because bgm uses bitfields.
                        const int       nFieldCount = sscanf(pData8, "%u %d %d %d %f %f %f %f %f", 
                                                                        &glyphId, &textureIndex, &positionX, 
                                                                        &positionY, &bgm.mfSizeX, &bgm.mfSizeY, 
                                                                        &bgm.mfHBearingX, &bgm.mfHBearingY, &bgm.mfHAdvanceX);
                        (void)nFieldCount;
                        EA_ASSERT(nFieldCount == 9);
                        bgm.mnTextureIndex = textureIndex;
                        bgm.mnPositionX    = positionX; 
                        bgm.mnPositionY    = positionY;
                        mBmpGlyphMetricsMap.insert(BmpGlyphMetricsMap::value_type((GlyphId)glyphId, bgm));

                        pData8 = Strchr(pData8, ',');
                    } while(pData8++ && (pData8 < (sBuffer8.data() + sBuffer8.length())));

                    // To read multi-line glyph metric map.
                    if(sBuffer8[(eastl_size_t)size - 1] == ',')
                    {
                        bShouldReadAnotherLine = true;

                        sBuffer8.resize(kBufferSize);
                        size = EA::IO::ReadLine(pStream, &sBuffer8[0], kBufferSize);
                        sBuffer8.resize((eastl_size_t)size);
                        pData8 = sBuffer8.c_str();
                    }
                } while(bShouldReadAnotherLine);
            }
            else if(Stricmp(sBuffer8.c_str(), "KerningMap") == 0)
            {
                bool    bShouldReadAnotherLine;
                //int32_t nEntryCount = AtoI328(pData8);

                while(*pData8 != ' ')
                    pData8++;

                do{ // Read each line
                    bShouldReadAnotherLine = false;

                    do{ // Read the entries within each line.
                        unsigned glyphId1, glyphId2;
                        Kerning  kerning;

                        const int nFieldCount = sscanf(pData8, "%u %u %f", &glyphId1, &glyphId2, &kerning.mfKernX);
                        (void)nFieldCount;
                        EA_ASSERT(nFieldCount == 3);
                        mKerningMap.insert(KerningMap::value_type(GlyphIdPair((GlyphId)glyphId1, (GlyphId)glyphId2), kerning));

                        pData8 = Strchr(pData8, ',');
                    } while(pData8++ && (pData8 < (sBuffer8.data() + sBuffer8.length())));

                    // To read multi-line glyph metric map.
                    if(sBuffer8[(eastl_size_t)size - 1] == ',')
                    {
                        bShouldReadAnotherLine = true;

                        sBuffer8.resize(kBufferSize);
                        size = EA::IO::ReadLine(pStream, &sBuffer8[0], kBufferSize);
                        sBuffer8.resize((eastl_size_t)size);
                        pData8 = sBuffer8.c_str();
                    }
                } while(bShouldReadAnotherLine);
            }
            else if(Stricmp(sBuffer8.c_str(), "CharMapSet") == 0)
            {
                bool    bShouldReadAnotherLine;
                //int32_t nEntryCount = AtoI328(pData8);

                while(*pData8 != ' ')
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
                    } while(pData8++ && (pData8 < (sBuffer8.data() + sBuffer8.length())));

                    // To read multi-line glyph metric map.
                    if(sBuffer8[(eastl_size_t)size - 1] == ',')
                    {
                        bShouldReadAnotherLine = true;

                        sBuffer8.resize(kBufferSize);
                        size = EA::IO::ReadLine(pStream, &sBuffer8[0], kBufferSize);
                        sBuffer8.resize((eastl_size_t)size);
                        pData8 = sBuffer8.c_str();
                    }
                } while(bShouldReadAnotherLine);
            }
            else if(Stristr(sBuffer8.c_str(), "Texture") == sBuffer8.c_str())
            {
                // We will have a line like this: 
                //     Texture0: 19387 256 256 EATextDemo 0.png
                // whereby pData8 points to this part:
                //     19387 256 256 EATextDemo 0.png

                BmpTextureInfo* const pBmpTextureInfo = CORE_NEW(mpCoreAllocator, EATEXT_ALLOC_PREFIX "BmpTextureInfo", 0) BmpTextureInfo;
                pBmpTextureInfo->AddRef(); // AddRef for the mTextureInfoArray container.
                pBmpTextureInfo->mpCoreAllocator = mpCoreAllocator;

                sscanf(pData8, "%u %u %u", (unsigned*)&pBmpTextureInfo->mnTextureFileSize,
                                                     (unsigned*)&pBmpTextureInfo->mnSize, 
                                                     (unsigned*)&pBmpTextureInfo->mnSize);

                pBmpTextureInfo->mfSizeInverse = 1.f / pBmpTextureInfo->mnSize;

                // Note that it is feasible that the file name isn't a disk file name  
                // but instead is a resource descriptor or 'bigfile' path.
                const char8_t* pTextureFileName = Strchr(pData8, ' ');
                pTextureFileName = Strchr(++pTextureFileName, ' ');
                pTextureFileName = Strchr(++pTextureFileName, ' ');

                // At this point we have a file (or resource) name. In the case of a file name, 
                // we may have just the file name alone and not the directory it is in.
                // We need to convert from 8 bit to 16 bit as we assign.
                while(*++pTextureFileName)
                    pBmpTextureInfo->mTextureFilePath += (uint8_t)*pTextureFileName;

                mTextureInfoArray.push_back(pBmpTextureInfo);
            }
        }

        sBuffer8.resize(kBufferSize);
    }

    pStream->SetPosition((EA::IO::off_type)streamPositionSaved);

    return true;
}


///////////////////////////////////////////////////////////////////////////////
// ReadBmpTexture
//
bool BmpFont::ReadBmpTexture(IO::IStream* pStream, uint32_t textureIndex)
{
    bool bResult = false;

    EA_ASSERT(textureIndex < mTextureInfoArray.size());

    if(textureIndex < mTextureInfoArray.size())
    {
        BmpTextureInfo* const pBmpTextureInfo = mTextureInfoArray[textureIndex];

        // We set it to be ARGB, though this requires code below to write it as ARGB.
        // The file loading code below will load mPixelData as ARGB, but the glyph cache]
        // code below might load it as 
        // To consider: Support user-specified alternatives to this.
        pBmpTextureInfo->mFormat  = kTextureFormatARGB;
        pBmpTextureInfo->mTexture = 0;

        bResult = true;

        if(mbEnableDirectGlyphCacheUse && mpGlyphCache) // If there is a (usually global) glyph cache that we can write our textures to...
        {
            // Create the texture. Note that pBmpTextureInfo->mpSource points to a file path
            // (or some other kind of asset moniker) and the CreateTexture function should 
            // pay attention to that and load that texture instead of creating a new one 
            // from nothing. User-provided subclasses of GlyphCache can customize the behaviour.
            pBmpTextureInfo->mpStream = pStream;
            pBmpTextureInfo->mTexture = mpGlyphCache->CreateTexture(pBmpTextureInfo); // This will re-write the pBmpTextureInfo->mTexture value to be of the type that was created.
            pBmpTextureInfo->mpStream = NULL;  //We won't be needing this any more, and we don't want to hold onto it after it's released and becomes stale.

            bResult = (pBmpTextureInfo->mTexture != 0);

            if(bResult)
            {
                // To do: In debug build assert that the mTexture pixel data is not blank.
                // If the user's CreateTexture function returns true then mTexture should
                // contain the font image. This isn't necessarily easy because it means
                // reading back a texture image, which isn't even possible on some platforms.

                // To consider: Free pBmpTextureInfo->mPixelData, as it won't be needed any more.
                // pBmpTextureInfo->mPixelData.Clear();
                TextureInfo* pTextureInfo = mpGlyphCache->AddTextureInfo(pBmpTextureInfo, true, true); // Third 'true' parameter means to ignore any size limits the glyph cache may have.

                bResult = (pTextureInfo != NULL);
                EA_ASSERT(bResult);
            }
        }
    }

    return bResult;
}


///////////////////////////////////////////////////////////////////////////////
// GetGlyphMetrics
//
bool BmpFont::GetGlyphMetrics(GlyphId glyphId, GlyphMetrics& glyphMetrics)
{
    const BmpGlyphMetricsMap::const_iterator it = mBmpGlyphMetricsMap.find(glyphId);

    if(it != mBmpGlyphMetricsMap.end()) // If found...
    {
        const BmpGlyphMetrics& glyphMetricsTemp = (*it).second;
        glyphMetrics = glyphMetricsTemp;

        return true;
    }

    return false;
}


///////////////////////////////////////////////////////////////////////////////
// GetGlyphIds
//
uint32_t BmpFont::GetGlyphIds(const Char* pCharArray, uint32_t nCharArrayCount, GlyphId* pGlyphIdArray, 
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


uint32_t BmpFont::GetGlyphIds(const char8_t* pCharArray, uint32_t nCharArrayCount, GlyphId* pGlyphIdArray, 
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


///////////////////////////////////////////////////////////////////////////////
// IsCharSupported
//
bool BmpFont::IsCharSupported(Char c, Script script)
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


///////////////////////////////////////////////////////////////////////////////
// GetSize
//
float BmpFont::GetSize() const
{
    return mFontDescription.mfSize;
}


///////////////////////////////////////////////////////////////////////////////
// SetTransform
//
bool BmpFont::SetTransform(float /*f00*/, float /*f01*/, float /*f10*/, float /*f11*/)
{
    // We don't support dynamic transformation. The user is expected to 
    // implement transformation during bitmap font editing.
    return false;
}


///////////////////////////////////////////////////////////////////////////////
// SetTransform
//
bool BmpFont::SetTransform(float /*fSize*/, float /*fAngle*/, float /*fSkew*/)
{
    // We don't support dynamic transformation. The user is expected to 
    // implement transformation during bitmap font editing.
    return false;
}


///////////////////////////////////////////////////////////////////////////////
// GetGlyphTextureInfo
//
bool BmpFont::GetGlyphTextureInfo(GlyphId glyphId, GlyphTextureInfo& glyphTextureInfo) const
{
    BmpGlyphMetricsMap::const_iterator it = mBmpGlyphMetricsMap.find(glyphId);

    if(it != mBmpGlyphMetricsMap.end()) // If we have this glyph...
    {
        const BmpGlyphMetrics& bmpGlyphMetrics = (*it).second;
        BmpTextureInfo* const pBmpTextureInfo = mTextureInfoArray[(eastl_size_t)bmpGlyphMetrics.mnTextureIndex];

        EA_ASSERT(pBmpTextureInfo->mTexture && 
                  (bmpGlyphMetrics.mnPositionX < (int32_t)pBmpTextureInfo->mnSize) &&  
                  (bmpGlyphMetrics.mnPositionY < (int32_t)pBmpTextureInfo->mnSize));

        const float xSize = 1.0f / (float)pBmpTextureInfo->mnSize;
        const float ySize = xSize; // Assumes square texture  

        // To consider: these could be precalculated and stored in the BmpGlyphMetrics.
        glyphTextureInfo.mpTextureInfo = pBmpTextureInfo;                                                            // The texture this glyph belongs to.
        glyphTextureInfo.mX1           = (bmpGlyphMetrics.mnPositionX + (int)bmpGlyphMetrics.mfHBearingX) * xSize;   // Position of glyph on texture. These are UV positions and so are between 0 and 1.
        glyphTextureInfo.mY1           = (bmpGlyphMetrics.mnPositionY - (int)bmpGlyphMetrics.mfHBearingY) * ySize;   // Position of glyph on texture.
        glyphTextureInfo.mX2           = glyphTextureInfo.mX1 + (bmpGlyphMetrics.mfSizeX) * xSize;                   // Position of glyph on texture.
        glyphTextureInfo.mY2           = glyphTextureInfo.mY1 + (bmpGlyphMetrics.mfSizeY) * ySize;                   // Position of glyph on texture.

        return true;
    }

    return false;
}


///////////////////////////////////////////////////////////////////////////////
// RenderGlyphBitmap
//
// We will return to the user a pointer to our internal mGlyphBitmap. 
//
bool BmpFont::RenderGlyphBitmap(const GlyphBitmap** pGlyphBitmap, GlyphId glyphId, uint32_t /*renderFlags*/, float /*fXFraction*/, float /*fYFraction*/)
{
    // Get the texture file from TextureInfo
    BmpGlyphMetricsMap::const_iterator it = mBmpGlyphMetricsMap.find(glyphId);

    if(it != mBmpGlyphMetricsMap.end()) // If we have this glyph...
    {
        const BmpGlyphMetrics& bmpGlyphMetrics = (*it).second;
        const BmpTextureInfo*  pBmpTextureInfo = mTextureInfoArray[(unsigned)bmpGlyphMetrics.mnTextureIndex];

        EA_ASSERT(!pBmpTextureInfo->mPixelData.empty() && 
                  (bmpGlyphMetrics.mnPositionX < (int32_t)pBmpTextureInfo->mnSize) &&  
                  (bmpGlyphMetrics.mnPositionY < (int32_t)pBmpTextureInfo->mnSize));

        // Copy the GlyphMetrics to the structure that will be returned to the caller.
        GetGlyphMetrics(glyphId, mGlyphBitmap.mGlyphMetrics);

        mGlyphBitmap.mnWidth       = (uint32_t)mGlyphBitmap.mGlyphMetrics.mfSizeX;
        mGlyphBitmap.mnHeight      = (uint32_t)mGlyphBitmap.mGlyphMetrics.mfSizeY;
        mGlyphBitmap.mnStride      = pBmpTextureInfo->mnSize * sizeof(uint32_t);
        mGlyphBitmap.mBitmapFormat = (EA::Text::Font::BitmapFormat)pBmpTextureInfo->mFormat;
        mGlyphBitmap.mpData        = (uint32_t*)pBmpTextureInfo->mPixelData.data() + ((bmpGlyphMetrics.mnPositionY - (int)mGlyphBitmap.mGlyphMetrics.mfHBearingY) * pBmpTextureInfo->mnSize) + (bmpGlyphMetrics.mnPositionX + (int)mGlyphBitmap.mGlyphMetrics.mfHBearingX);

        *pGlyphBitmap = &mGlyphBitmap;

        return true;
    }

    return false;
}


///////////////////////////////////////////////////////////////////////////////
// DoneGlyphBitmap
//
void BmpFont::DoneGlyphBitmap(const GlyphBitmap* pGlyphBitmap)
{
    EA_ASSERT(pGlyphBitmap == &mGlyphBitmap); (void)pGlyphBitmap; // Avoid unused var warnings.

    #ifdef EA_DEBUG
        mGlyphBitmap.mpData = NULL;
    #endif
}


///////////////////////////////////////////////////////////////////////////////
// GetKerning
//
bool BmpFont::GetKerning(GlyphId g1, GlyphId g2, Kerning& kerning, int direction, bool /*bHorizontalLayout*/)
{
    bool bReturnValue = false;

    #if (EATEXT_KERNING_SUPPORT > 0)
        if(direction % 2) // If RTL...
            eastl::swap(g1, g2);

        const GlyphIdPair pair(g1, g2);
        const KerningMap::const_iterator it = mKerningMap.find(pair);

        if(it != mKerningMap.end())
        {
            bReturnValue = true;
            kerning = (*it).second;
        }
    #endif

    if(!bReturnValue)
    {
        kerning.mfKernX = 0.f;
        kerning.mfKernY = 0.f;
    }

    return bReturnValue;
}


///////////////////////////////////////////////////////////////////////////////
// GetBitmapFormat
//
BmpFont::BitmapFormat BmpFont::GetBitmapFormat() const
{
    if(!mTextureInfoArray.empty())
        return (BmpFont::BitmapFormat)mTextureInfoArray[0]->mFormat;

    return kBFARGB; // To consider: Return the default format for the given platform, which may be different.
}


///////////////////////////////////////////////////////////////////////////////
// GetBitmapFormat
//
uint32_t BmpFont::GetTextureCount() const
{
    return (uint32_t)mTextureInfoArray.size();
}


///////////////////////////////////////////////////////////////////////////////
// GetBmpTextureInfo
//
BmpTextureInfo* BmpFont::GetBmpTextureInfo(uint32_t textureIndex)
{
    if(textureIndex < (uint32_t)mTextureInfoArray.size())
        return mTextureInfoArray[(eastl_size_t)textureIndex];

    return NULL;
}


///////////////////////////////////////////////////////////////////////////////
// SetBmpTextureInfo
//
void BmpFont::SetBmpTextureInfo(uint32_t textureIndex, BmpTextureInfo* pBmpTextureInfo)
{
    EA_ASSERT(pBmpTextureInfo && (textureIndex < 1000)); // Sanity checks.

    if(textureIndex >= mTextureInfoArray.size())
    {
        // Note that this will result in one or more NULL pointers being created within 
        // mTextureInfoArray, though the last of them will be set directly below.
        mTextureInfoArray.resize(textureIndex + 1);
    }

    pBmpTextureInfo->AddRef();

    if(mTextureInfoArray[textureIndex])
        mTextureInfoArray[textureIndex]->Release();

    // To consider: We should probably call mpGlyphCache->RemoveTextureInfo if 
    //    pBmpTextureInfo is replacing a texture with a new one.
    mTextureInfoArray[textureIndex] = pBmpTextureInfo;

    if(mpGlyphCache)
    {
        // The GlyphCache will ignore this add if pBmpTextureInfo was already added.
        mpGlyphCache->AddTextureInfo(pBmpTextureInfo, true);
    }
}


///////////////////////////////////////////////////////////////////////////////
// SetBmpTexture
//
void BmpFont::SetBmpTexture(uint32_t textureIndex, uintptr_t texture, uint32_t format)
{
    EA_ASSERT((textureIndex < mTextureInfoArray.size()) && mTextureInfoArray[textureIndex]); // Sanity checks.

    BmpTextureInfo* const pBmpTextureInfo = mTextureInfoArray[textureIndex];

    pBmpTextureInfo->mTexture = texture;
    pBmpTextureInfo->mFormat  = format;

    if(mpGlyphCache)
    {
        // The GlyphCache will ignore this add if pBmpTextureInfo was already added.
        mpGlyphCache->AddTextureInfo(pBmpTextureInfo, true);
    }
}



} // namespace Text

} // namespace EA

















