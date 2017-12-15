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
// EATextFontServer.cpp
//
// By Paul Pedriana - 2004
//
///////////////////////////////////////////////////////////////////////////////


#include <EAText/EATextFontServer.h>
#include <EAText/EATextOutlineFont.h>
#include <EAText/EATextBmpFont.h>
#include <EAText/EATextPolygonFont.h>
#include <EAText/EATextScript.h>
#include <EAText/internal/EATextStream.h>
#include <EAText/internal/StdC.h>
#include <coreallocator/icoreallocatormacros.h>
#include <EAIO/EAStream.h>
#include <EAIO/PathString.h>
#include <EAIO/EAFileBase.h>
#include <EAIO/EAFileDirectory.h>
#include <EAIO/EAFileStream.h>
#include <EAIO/EAStreamMemory.h>
#include <EAIO/FnEncode.h>
#include <EASTL/fixed_string.h>
#include <EASTL/algorithm.h>
#include EA_ASSERT_HEADER
#include <stddef.h>
#include <string.h>
#include <math.h>
    #pragma warning(push, 0)
    #include <Windows.h>
    #pragma warning(pop)

#if EATEXT_USE_FREETYPE
    #include <ft2build.h>
    #include FT_FREETYPE_H  // #include <freetype/freetype.h>
    #include FT_SIZES_H     // #include <freetype/ftsizes.h>
#endif



namespace EA
{
namespace Text
{


#if EATEXT_USE_FREETYPE
    extern volatile FT_Library gFTLibrary;
#endif

#if EATEXT_EMBEDDED_BACKUP_FONT_ENABLED
    // Here we #include an embedded TrueType font which is located inside 
    // a .inl file and which is declared as a C char array like this:
    //   const unsigned char pTrueTypeFont[] = { 0x23, 0xa2, ... }; 
    #include "internal/EmbeddedFont.inl"
#endif



EATEXT_API uint32_t GetSystemFontDirectory(FilePathChar* pFontDirectory, uint32_t nFontDirectoryCapacity)
{
        FilePathChar        pFontDirectoryTemp[EA::IO::kMaxPathLength];
        UINT                nStrlen = GetWindowsDirectoryW(pFontDirectoryTemp, (UINT)_MAX_PATH); // GetWindowsDirectoryW: If the function fails, the return value is zero. To get extended error information, call GetLastError. 
        const FilePathChar* pFonts = EA_CHAR16("Fonts\\");
        const uint32_t      kFontsLength = 6;

        if(nStrlen > 0)
        {
            // Build the path
            if((pFontDirectoryTemp[nStrlen - 1] != '\\') && (pFontDirectoryTemp[nStrlen - 1] != '/'))
                pFontDirectoryTemp[nStrlen++] = '\\';
            wcscpy(pFontDirectoryTemp + nStrlen, pFonts);
            nStrlen += kFontsLength;

            // Possibly copy the path
            if(pFontDirectory && (nFontDirectoryCapacity > nStrlen))
                wcscpy(pFontDirectory, pFontDirectoryTemp);
        }

        return (uint32_t)nStrlen;
}


EATEXT_API FontType GetFontTypeFromFilePath(const FilePathChar* pFontFilePath)
{
    const FilePathChar* const pExtension = EA::IO::Path::GetFileExtension(pFontFilePath);

    if(Stricmp(pExtension, EATEXT_FILE_PATH_CHAR(".bmpFont")) == 0)
        return kFontTypeBitmap;

    if(Stricmp(pExtension, EATEXT_FILE_PATH_CHAR(".polygonFont")) == 0)
        return kFontTypePolygon;

    // We could conceivably check for specific types such as .tt?, .otf.
    return kFontTypeOutline;
}




#ifdef EATEXT_FONT_SERVER_ENABLED

///////////////////////////////////////////////////////////////////////////////
// FontServer
///////////////////////////////////////////////////////////////////////////////

///////////////////////////////////////////////////////////////////////////////
// FontServer::FaceSource
//
FontServer::FaceSource::FaceSource()
  : mpStream(NULL),
    mFontType(kFontTypeUnknown),
    mFontDescription(),
    mnFaceIndex(0),
    mFontList(),
    mpFaceData(NULL)
{
}


///////////////////////////////////////////////////////////////////////////////
// FontServer::FaceSource
//
FontServer::FaceSource::FaceSource(const FaceSource& fs)
{
    mpStream = NULL;
    mpFaceData = NULL;

    operator=(fs);
}


///////////////////////////////////////////////////////////////////////////////
// FontServer::~FaceSource
//
FontServer::FaceSource::~FaceSource()
{
    for(FontList::iterator itF = mFontList.begin(); itF != mFontList.end(); ++itF)
    {
        Font* const pFont = *itF;
        pFont->Release();
    }

    if(mpStream)
        mpStream->Release();

    if(mpFaceData)
        mpFaceData->Release();
}


///////////////////////////////////////////////////////////////////////////////
// FontServer::operator=
//
FontServer::FaceSource& FontServer::FaceSource::operator=(const FaceSource& fs)
{
    if(&fs != this)
    {
        mFontType        = fs.mFontType;
        mFontDescription = fs.mFontDescription;
        mnFaceIndex      = fs.mnFaceIndex;
        mFontList        = fs.mFontList;

        for(FontList::iterator itF = mFontList.begin(); itF != mFontList.end(); ++itF)
        {
            Font* const pFont = *itF;
            pFont->AddRef();
        }

        if(fs.mpStream)
            fs.mpStream->AddRef();
        if(mpStream)
            mpStream->Release();
        mpStream = fs.mpStream;

        if(fs.mpFaceData)
            fs.mpFaceData->AddRef();
        if(mpFaceData)
            mpFaceData->Release();
        mpFaceData = fs.mpFaceData;
    }

    return *this;
}


///////////////////////////////////////////////////////////////////////////////
// FontServer::Face
//
FontServer::Face::Face() 
    : mFaceSourceList()
{
    memset(mFamily, 0, sizeof(mFamily));
}


///////////////////////////////////////////////////////////////////////////////
// FontServer
//
FontServer::FontServer(Allocator::ICoreAllocator* pCoreAllocator)
    : mbInitialized(false),
      mpCoreAllocator(pCoreAllocator ? pCoreAllocator : EA::Text::GetAllocator()),
      mTextStyleDefault(),
      mFaceMap(),
      mpGlyphCacheDefault(),
      mFaceArray(),
      mEffectDataList(EA::Allocator::EASTLICoreAllocator(EATEXT_ALLOC_PREFIX "FontServer/EffectDataList", mpCoreAllocator)),
      mDPI(EATEXT_DPI),
      mIgoredFontCharSet()
    #if EATEXT_FAMILY_SUBSTITUTION_ENABLED
    , mFamilySubstitutionMap()
    #endif
{
    mOptions[kOptionOpenTypeFeatures]            = -1;
    mOptions[kOptionDPI]                         = -1;
    mOptions[kOptionEnableHinting]               = -1;
    mOptions[kOptionUseAutoHinting]              = -1;
    mOptions[kOptionLCD]                         = -1;
    mOptions[kOptionEnableDirectGlyphCacheUse]   = -1;
    mOptions[kOptionSystemMemoryCopy]            = -1;
    mOptions[kOptionSmartFallback]               =  0;
    mOptions[kOptionRequireFontFamilyMatch]      =  kMatchOptionNone;

    // These are Chars that we pretend are present in all fonts, as they are not really characters but
    // rather instructions. Thus it's pointless to use font matching for these characters, as it would
    // make for in efficient layout.
    const Char gIgnoredFontCharArray[] = 
    {
        kCharNUL, kCharBS, kCharHT, kCharLF, kCharVT, kCharFF, kCharCR, kCharESC, kCharNEL, kCharNBSP,
        kCharCGJ, kCharZWSP, kCharZWNJ, kCharZWJ, kCharLRM, kCharRLM, kCharLSEP, kCharPSEP, kCharLRE,
        kCharRLE, kCharPDF, kCharLRO, kCharRLO, kCharWJoin, kCharISS, kCharASS, kCharIAFS, kCharAAFS,
        kCharNADS, kCharNODS, kCharOBJ, kCharREPL, kCharZWNBSP, kCharBOM, kCharBOM_R, kCharInvalid,
    };

    for(eastl_size_t i = 0; i < EAArrayCount(gIgnoredFontCharArray); i++)
        mIgoredFontCharSet.insert(gIgnoredFontCharArray[i]);
}


///////////////////////////////////////////////////////////////////////////////
// ~FontServer
//
FontServer::~FontServer()
{
    // Assert that Shutdown was called.
    EA_ASSERT_MESSAGE(!mbInitialized, "FontServer::~FontServer: Shutdown needs to be called.");

    FontServer::Shutdown(); // Preface call with FontServer:: because we are in a destructor and virtual functions don't work in destructors and the compiler would issue a warning.
}


///////////////////////////////////////////////////////////////////////////////
// SetAllocator
//
void FontServer::SetAllocator(Allocator::ICoreAllocator* pCoreAllocator)
{
    mpCoreAllocator = pCoreAllocator;
    mEffectDataList.get_allocator().set_allocator(pCoreAllocator);
}


///////////////////////////////////////////////////////////////////////////////
// Init
//
bool FontServer::Init()
{
    EA_ASSERT_MESSAGE(mpCoreAllocator, "FontServer::mpCoreAllocator is NULL. You need to call EA::Text::SetAllocator on startup, and FontServer will pick up the value during its construction. If you call EA::Text::SetAllocator after the Font Server is constructed, then you will need to call FontServer::SetAllocator with the same value before using the Font Server.");

    if(!mbInitialized)
    {
        mbInitialized = true;

        #if EATEXT_EMBEDDED_BACKUP_FONT_ENABLED
            EA::IO::EATextMemoryStream* const pMemoryStream = CORE_NEW(mpCoreAllocator, EATEXT_ALLOC_PREFIX "MemoryStream", 0)
                                                                 EA::IO::EATextMemoryStream((void*)pTrueTypeFont, sizeof(pTrueTypeFont), true, false, mpCoreAllocator);
            if(pMemoryStream)
            {
                pMemoryStream->AddRef();
                pMemoryStream->mpCoreAllocator = mpCoreAllocator;
                AddFace(pMemoryStream, kFontTypeOutline);
                pMemoryStream->Release();
            }
        #endif
    }

    return true;
}


///////////////////////////////////////////////////////////////////////////////
// Shutdown
//
bool FontServer::Shutdown()
{
    mFaceMap.clear(); // This clear will take care of releasing any allocated font resources.
    mEffectDataList.clear(); 

    mbInitialized = false;

    return true;
}


///////////////////////////////////////////////////////////////////////////////
// SetOption
//
void FontServer::SetOption(int32_t option, int32_t value)
{
    EA_ASSERT((option != kOptionDPI) || (value > 0));  // kOptionDPI value must be > 0

    if(option < kOptionCount)
        mOptions[option] = value;
}


///////////////////////////////////////////////////////////////////////////////
// SetOption
//
int32_t FontServer::GetOption(int32_t option) const
{
    if(option < kOptionCount)
        return mOptions[option];

    return 0;
}


///////////////////////////////////////////////////////////////////////////////
// SetDefaultGlyphCache
//
void FontServer::SetDefaultGlyphCache(GlyphCache* pGlyphCache)
{
    mpGlyphCacheDefault = pGlyphCache;
}


///////////////////////////////////////////////////////////////////////////////
// GetDefaultGlyphCache
//
GlyphCache* FontServer::GetDefaultGlyphCache()
{
    return mpGlyphCacheDefault;
}


///////////////////////////////////////////////////////////////////////////////
// GetFont
//
Font* FontServer::GetFont(const TextStyle* pTextStyle, FontSelection& fontSelection, 
                            uint32_t maxCount, Char c, Script script, bool bManaged)
{
    const uint32_t kFontArraySize = 32;
    Font*          pFontArray[kFontArraySize];
    uint32_t       nFontArrayCapacity = (maxCount < kFontArraySize ? maxCount : kFontArraySize);

    // The GetFont call will AddRef the fonts written to pFontArray.
    Font* const pPrimaryFont = GetFont(pTextStyle, pFontArray, nFontArrayCapacity, c, script, bManaged);

    // Transfer the fonts into the FontSelection.
    for(uint32_t i = 0; (i < nFontArrayCapacity) && pFontArray[i]; i++)
    {
        fontSelection.insert(pFontArray[i]); // This will AddRef the Font for the caller.
        pFontArray[i]->Release();            // Matches AddRef from the GetFont above. Since the font is AddRefd for fontSelection, this Release will not destroy the Font.
    }

    return pPrimaryFont;
}


///////////////////////////////////////////////////////////////////////////////
// GetFont
//
// The CSS standard specifies an algorithm for scoring font descriptions.
// As of this writing, this standard is documented at http://www.w3.org/TR/REC-CSS2/fonts.html#algorithm
// for CSS2 and http://www.w3.org/TR/2002/WD-css3-fonts-20020802/ for CSS3.
//
// To do: Have the last N lookups be cached, allowing the lookup to be 
//        immediate if the textStyle matches the cached one. 
//
Font* FontServer::GetFont(const TextStyle* pTextStyle, Font* pFontArray[], uint32_t nFontArrayCapacity, 
                            Char c, Script script, bool bManaged)
{
    Font*           pPrimaryFont = NULL;    // This is the first font returned and is the "best match" of the fonts matching the requested style.
    Char            familyNameArray[kFamilyNameArrayCapacity][kFamilyNameCapacity];
    uint32_t        nFamilyNameCount = 0;
    const uint32_t  kFaceSourceCount = 32;
    int             nScoreArray[kFaceSourceCount];
    FaceSource*     pFaceSourceArray[kFaceSourceCount];
    uint32_t        nFaceSourceArraySize = 0;
    uint32_t        nMaxFaceSourceArraySize = kFaceSourceCount;
    uint32_t        i, iEnd;
    bool            bSmartFallbackEnabled = (mOptions[kOptionSmartFallback] > 0);

    if(nFontArrayCapacity > kFaceSourceCount)
        nFontArrayCapacity = kFaceSourceCount;

    if(!pTextStyle)
        pTextStyle = &mTextStyleDefault;

    if(nFontArrayCapacity)
        memset(pFontArray, 0, sizeof(Font*) * nFontArrayCapacity);

    mFaceArray.clear();

    // Do checks for user errors.
    EA_ASSERT_MESSAGE(!mFaceMap.empty(), "The FontServer has no fonts to serve.");
    EA_ASSERT_MESSAGE(pTextStyle->mfSize > 0.f, "The style specification requires a size > 0.");


    // Copy the input style specification and make it lower-case while doing so.
    for(i = 0; (i < kFamilyNameArrayCapacity) && pTextStyle->mFamilyNameArray[i][0]; i++)
    {
        Strcpy(familyNameArray[i], pTextStyle->mFamilyNameArray[i]);
        Strlwr(familyNameArray[i]);
        ++nFamilyNameCount;
    }

    // There are some characters that we ignore for the purpose of font lookups (e.g. zero-width non-breaking space).
    // We want to pretend that every Font works for such characters, even if those characters aren't represented in
    // the font. This situation comes about because text is a string of characters and it uses the same number space
    // to encode regular characters (e.g. 'a') but also to encode instructions (e.g. start a new line, reverse display direction).
    bool bIgnoreChar;

    if(c == kCharInvalid)   // We are commonly called with c == kCharInvalid, so do a quick check here in order to avoid the hash table lookup two lines below.
        bIgnoreChar = true;
    else
        bIgnoreChar = (mIgoredFontCharSet.find(c) != mIgoredFontCharSet.end());


    #if EATEXT_FAMILY_SUBSTITUTION_ENABLED
        // Test for font substitutions. These are font names that are to be checked if the original name doesn't exist.
        // What we do here is possibly modify familyNameArray entries to use substituted values.
        // If you are using family substitution, it may be a good idea that the user set up the 
        // CSS generic family names (e.g. "serif") as substitution values. See enum Family.
        if(!mFamilySubstitutionMap.empty())
        {
            for(int n = (int)nFamilyNameCount - 1; n >= 0; --n)
            {
                Char* const pFamily = familyNameArray[n];

                FamilySubstitutionMap::iterator it = mFamilySubstitutionMap.find_as(pFamily);
                if(it != mFamilySubstitutionMap.end())
                {
                    const FontServerString& sFamilySubstitution = (*it).second;

                    EA_ASSERT(sFamilySubstitution.length() < kFamilyNameCapacity);
                    Strncpy(pFamily, sFamilySubstitution.c_str(), kFamilyNameCapacity);
                    pFamily[kFamilyNameCapacity - 1] = 0;
                }
            }
        }
    #endif // EATEXT_FAMILY_SUBSTITUTION_ENABLED


    // Find the FaceMap entries that match familyNameArray.
    for(i = 0; i < nFamilyNameCount; i++)
    {
        const FaceMap::iterator it = mFaceMap.find_as(familyNameArray[i]);

        if(it != mFaceMap.end())
        {
            Face& face = (*it).second;
            mFaceArray.push_back(&face);
        }
    }

    // At this point, hopefully we have some candidate FaceArray entries to work with.
    // We work with these entities now and no longer need the input family name information.
    // We go through each entry and search for matches to the input TextStyle information.
    TryFaceArray:
    for(i = 0, iEnd = (uint32_t)mFaceArray.size(); (i < iEnd) && (nFaceSourceArraySize < nMaxFaceSourceArraySize); i++)
    {
        Face* const pFace                = mFaceArray[i];
        FaceSource* pFaceSourceBestMatch = NULL;
        int         nScoreBestMatch      = -1; // Higher scores are better.
        int         nCharSupported       = -1; // -1 means not tried, 0 means tried and failed, 1 means tried and succeeded.

        for(FaceSourceList::iterator it = pFace->mFaceSourceList.begin(); (it != pFace->mFaceSourceList.end()) && (nCharSupported != 0); ++it)
        {
            FaceSource& faceSource = *it;

            if((nCharSupported == -1) && !faceSource.mFontList.empty()) // If we don't know if the char/script is supported but we can find out because we have a Font.
            {
                Font* const pFont = faceSource.mFontList.front();
                nCharSupported    = ((bIgnoreChar || pFont->IsCharSupported(c, script)) ? 1 : 0);
            }

            // If the char/script is known to be unsupported, we might as well give up on this FaceSource.
            if(nCharSupported != 0) // If the char is supported by this face...
            {
                const int nScore = GetFontDescriptionScore(faceSource.mFontDescription, *pTextStyle);

                if(nScore > nScoreBestMatch)
                {
                    pFaceSourceBestMatch = &faceSource;
                    nScoreBestMatch      = nScore;
                }
            }
        }

        if(pFaceSourceBestMatch)
        {
            if(nCharSupported == -1) // If we have no idea if the char/script is supported (because this face has no fonts open)...
            {
                // We can open a font and check the char/script or we can just assume the face supports the char/script.
                // In the case of Eastern chars/scripts we would need to check and in the case of English chars/scripts we probably don't need to.
                if((c < 0x0080) || ((c == kCharInvalid) && (script == kScriptUnknown)))  // To consider: We could possibly also assume that all chars of kScriptLatin will also be supported. That would however require all fonts to have the entire Latin set if they have any Latin characters.
                    nCharSupported = 1;
                else
                {
                    // We go ahead and make an instance of the font. We will be wasting some memory if the 
                    // font doesn't turn out to support the char/script requirements. On the other hand, 
                    // the code wouldn't get to here unless the user specifically asked to consider a 
                    // font of this given family in the user's style specification.
                    Font* const pFont = CreateNewFont(pFaceSourceBestMatch, *pTextStyle, true); // This AddRefs pFont for us.

                    if(pFont)
                    {
                        nCharSupported = ((bIgnoreChar || pFont->IsCharSupported(c, script)) ? 1 : 0);
                        pFont->Release(); // Match the AddRef from CreateNewFont above. But the font will stay alive because CreateNewFont will have AddRef'd the font for future use.
                    }
                }
            }

            if(nCharSupported == 1) // If the char/script is known to be supported...
            {
                // 2/19/09 Change by Chris Sidhall to implement duplication testing. 
                // The list is usually pretty small so a full search is usually OK here.
                bool duplicate = false;

                for(uint32_t j = 0; j < nFaceSourceArraySize; j++)
                {
                    if(pFaceSourceArray[j] == pFaceSourceBestMatch)    
                    {
                        duplicate = true;        
                        break;
                    }
                }

                if(!duplicate)
                {
                    pFaceSourceArray[nFaceSourceArraySize] = pFaceSourceBestMatch;
                    nScoreArray[nFaceSourceArraySize]      = nScoreBestMatch;
                    ++nFaceSourceArraySize;
                }
            }
        }
    }


    // Safety fallback. Just use the first font source that is usable or if not then just use the first font source there is.
    if(!nFaceSourceArraySize) // If no match was found...
    {
        if(bSmartFallbackEnabled)
        {
            // What we do now is check all typefaces for support of the given char/script.
            bSmartFallbackEnabled   = false; // Set this to false in order to prevent infinite recursion.
            nMaxFaceSourceArraySize = 1;     // Stop at the first match, no need to collect extra fonts and pick the best. To consider: Provide an advanced configurable option to in fact look for the best fallback match.
            mFaceArray.clear();

            for(FaceMap::iterator itFM = mFaceMap.begin(); itFM != mFaceMap.end(); ++itFM)
            {
                Face& face = (*itFM).second;
                mFaceArray.push_back(&face); // To consider: Might it be slightly more optimal if we only push_back faces that weren't checked already?
            }

            goto TryFaceArray; // We go back above and try matching faces again, but this time our selection includes our entire face set and not just the ones the user passed in.
        }
        else
        {
            // Here we simply pick the first typeface we have. 
            // In general it is a bad thing to be executing this code, as it almost always
            // is going to result in some font that is different from what was intended and 
            // may not support the required characters or may even be fixed at a given size.
            // Problem: If our typeface list includes fonts of alternative types such as 
            // polygonal fonts, then the returned font could be completely unusable.
            for(FaceMap::iterator itFM(mFaceMap.begin()); itFM != mFaceMap.end(); ++itFM)
            {
                FaceSourceList& faceSourceList = (*itFM).second.mFaceSourceList;

                EA_ASSERT(!faceSourceList.empty()); // We don't allow Faces without sources (i.e. with an empty mFaceSourceList).
                if(!faceSourceList.empty())
                {
                    FaceSource& faceSource = *faceSourceList.begin();
                    pFaceSourceArray[nFaceSourceArraySize] = &faceSource;
                    nScoreArray[nFaceSourceArraySize]      = 0;
                    nFaceSourceArraySize++; // Note that nFaceSourceArraySize is known to be zero here.
                    break;
                }
            }
        }
    }


    // If there are more than one face sources, make the highest scoring one be listed first.
    for(uint32_t j = 1; j < nFaceSourceArraySize; j++)
    {
        if(nScoreArray[j] > nScoreArray[0])
        {
            eastl::swap(pFaceSourceArray[j], pFaceSourceArray[0]);
            eastl::swap(nScoreArray[j],      nScoreArray[0]);
        }
    }


    // We (probably) have a FaceSource to work with. Now let's search for an existing 
    // Font or create one if there isn't an existing match.
    FontDescription fontDescription;

    for(uint32_t k = 0; k < nFaceSourceArraySize; k++)
    {
        Font* pFontCurrent = NULL;

        // We only check our list of pre-made fonts if the user requests a managed font.
        if(bManaged) 
        {
            for(FontList::iterator it = pFaceSourceArray[k]->mFontList.begin(); it != pFaceSourceArray[k]->mFontList.end(); ++it)
            {
                pFontCurrent = *it;

                EA_ASSERT(pFontCurrent);
                pFontCurrent->GetFontDescription(fontDescription);

                // We have a problem here. We probably need to have a more detailed match
                // check than that done here. Currently we check only a few properties, 
                // whereas there are other important characteristics of a font.
                if((fabs(fontDescription.mfSize - pTextStyle->mfSize) < 0.05f) && // If there is a font match...
                    (fontDescription.mSmooth == pTextStyle->mSmooth) &&
                    (fontDescription.mEffect == pTextStyle->mEffect))
                {
                    pFontCurrent->AddRef(); // AddRef it once for the user. The ref count on the font will now be at least 2 (once for our container, and once for the user here).
                    break;
                }
                else
                    pFontCurrent = NULL;
            }
        }

        if(!pFontCurrent) // If no size matched or (bManaged == false), we create the font ourselves to match the size (and style).
        {
            if(!pPrimaryFont || (k < nFontArrayCapacity))
            {
                pFontCurrent = CreateNewFont(pFaceSourceArray[k], *pTextStyle, bManaged);
                EA_ASSERT(pFontCurrent);
                // Pass on the AddRef that CreateNewFont did to the caller. The ref count on the font will now be at least 2 (once for our container, and once for the user here).
            }
        }

        if(pFontCurrent)
        {
            if(!pPrimaryFont)
                pPrimaryFont = pFontCurrent;

            if(k < nFontArrayCapacity)
                pFontArray[k] = pFontCurrent;
            else if(pFontCurrent != pPrimaryFont) // If we won't be returning this font to the user...
                pFontCurrent->Release(); // Matches the AddRef above for the user. Since we won't be returning this font to the user, we let go of the reference.
        }
    } // for()


    // The only way we should not have at least some kind of font to offer is if we simply have no fonts.
    EA_ASSERT(pPrimaryFont || mFaceMap.empty());

    if(pPrimaryFont && (mOptions[kOptionRequireFontFamilyMatch] > 0)) // If we should handle font name matching failures...
    {
        // If using the EAAssert package and assertions are enabled, or if using the EATrace package and assertions are enabled...
        #if ((defined(EAASSERT_EAASSERT_H) && defined(EA_ASSERT_ENABLED)) || (defined(EATRACE_EATRACE_H) && EA_TRACE_ENABLED))
            bool shouldDoMatch = true;
        #else
            bool shouldDoMatch = ((mOptions[kOptionRequireFontFamilyMatch] & kMatchOptionFail) != 0);
        #endif

        if(shouldDoMatch)
        {
            FontDescription fontDescription;
            bool            bFamilyNameMatch = false;

            // We test only the name of the primary font, as that one at least must match.
            pPrimaryFont->GetFontDescription(fontDescription);

            for(uint32_t i = 0; i < nFamilyNameCount; i++) // For each of the family name options in the list the user gave us...
            {
                if(Stricmp(fontDescription.mFamily, familyNameArray[i]) == 0)
                {
                    bFamilyNameMatch = true;
                    break;
                }
            }

            if(!bFamilyNameMatch) // If the above check found that pPrimaryFont's name doesn't match any of the 1 or more names requested by the user...
            {
                if(mOptions[kOptionRequireFontFamilyMatch] & kMatchOptionAssert)
                {
                    EA_FAIL_MESSAGE("FontServer::GetFont: Failed to find a matching font by name.");
                }

                if(mOptions[kOptionRequireFontFamilyMatch] & kMatchOptionFail)
                {
                    // Release all the fonts and return NULL.
                    if(pFontArray)
                    {
                        for(uint32_t f = 0; pFontArray[f] && (f < nFontArrayCapacity); f++)
                            pFontArray[f]->Release();
                    }

                    pPrimaryFont->Release();
                    pPrimaryFont = NULL;
                }
            }
        }
    }

    return pPrimaryFont; // This font will be AddRefd for the caller. Any *other* fonts in the pFontArray argument will also be AddRef'd. 
}


///////////////////////////////////////////////////////////////////////////////
// GetFontDescriptionScore
//
int FontServer::GetFontDescriptionScore(const FontDescription& fd, const TextStyle& ssCSS)
{
    int nScore = 1000; // We start with a perfect score and subtract as we find mismatches.

    // We subtract a lot of points if there is no matching family name.
    // We subtract a few points if the name matches but it isn't the first in user-requested list.
    bool bFamilyNameMatch = false;
    int  nFamilyNameMatch = 0;

    for(int i = 0; !bFamilyNameMatch && (i < (int)kFamilyNameCapacity) && ssCSS.mFamilyNameArray[i][0]; i++)
    {
        if(Stricmp(fd.mFamily, ssCSS.mFamilyNameArray[i]) == 0)
        {
            bFamilyNameMatch = true;
            nFamilyNameMatch = i;
            break;
        }
    }

    if(bFamilyNameMatch)
        nScore -= (2 * nFamilyNameMatch);
    else
        nScore -= 100;

    // A FontDescription whose mfSize matches the ssCSS mfSize loses no points.
    // A FontDescription whose mfSize mismatches the ssCSS mfSize loses a lot of points.
    // A zero value for mfSize in the fontDescription means the font source can 
    // supply any sized font. Such fonts lose only a few points. 
    if(fd.mfSize == 0)
        nScore -= 10;
    else
    {
        const float fDifference = fabsf(ssCSS.mfSize - fd.mfSize);
        nScore -= (int)(20 * fDifference);
    }

    // Compare style.
    if(fd.mStyle != ssCSS.mStyle)
    {
        if((fd.mStyle == kStyleItalic    || fd.mStyle == kStyleOblique) && 
           (ssCSS.mStyle == kStyleItalic || ssCSS.mStyle == kStyleOblique))
            nScore -= 20; // Oblique and italic appear similar to most people.
        else
            nScore -= 100;
    }

    // Compare weight
    nScore -= (int)(fabs(fd.mfWeight - ssCSS.mfWeight) / 4);

    // Compare stretch
    if(fd.mfStretch != ssCSS.mfStretch)
        nScore -= (int)(20 + (20 * fabs(fd.mfStretch - ssCSS.mfStretch))); // Not being a perfect match subtracts a fixed amount. Then extra is added depending on the distance.

    // Compare pitch
    if(fd.mPitch != ssCSS.mPitch)
        nScore -= 150;

    // Compare variant
    if(fd.mVariant != ssCSS.mVariant)
        nScore -= 50;

    // Compare smoothing
    if(fd.mSmooth != ssCSS.mSmooth)
    {
        // Possibly we want to do something to make it so that if the user wants 
        // a differently smoothed version of a font from the same face, we can 
        // deal with that. Perhaps we can return 0 if we know that there is a
        // matching face available which can do the desired kind of smoothing.
        //
        // To consider: Add a configuration setting which makes it so that a 
        // request for a different smoothing of an existing font always returns
        // simply the existing font with its existing smoothing. This is useful 
        // for saving memory and preventing mistakes. Actually, having a small
        // score penalty here has virtually the same effect but lets you override
        // it if desired.
        nScore -= 5;
    }

    // Other aspects of the ssCSS are not part of a FontDescription but rather are 
    // about layout, decoration, or rendering characteristics.

    return nScore;
}


///////////////////////////////////////////////////////////////////////////////
// AddFaceSource
//
// Returns true if the faceSource was added (i.e. wasn't already present).
//
bool FontServer::AddFaceSource(FaceSource& faceSource, FontDescription* pFontDescription)
{
    bool bReturnValue = false;

    eastl::fixed_string<Char, 48> sFamily(faceSource.mFontDescription.mFamily);

    EA_ASSERT_FORMATTED(sFamily.length() < kFamilyNameCapacity, ("FontServer: Family name (%ls) is too long; max supported length is %d.", faceSource.mFontDescription.mFamily, (int)kFamilyNameCapacity - 1));
    if(sFamily.length() < kFamilyNameCapacity) // If the name is not too long...
    {
        sFamily.make_lower();
        FaceMap::iterator it = mFaceMap.find_as(sFamily.c_str());

        if(it == mFaceMap.end()) // If this family doesn't already have an entry... create one.
        {
            Face face;
            Strlcpy(face.mFamily, sFamily.c_str(), kFamilyNameCapacity);  // It's not a critical problem if there wasn't enough capacity for the full string.
            const FaceMap::value_type entry(face.mFamily, face);
            mFaceMap.insert(entry);
            it = mFaceMap.find(entry.first);

            #if EASTL_NAME_ENABLED // There isn't an easy way around using const_cast here, due to the way STL maps work.
                FontServerString& sFamilyMutable = const_cast<FontServerString&>(it->first);
                sFamilyMutable.get_allocator().set_name(EATEXT_ALLOC_PREFIX "FontServer/FaceMap");
            #endif
        }

        // If the face isn't already in the list, add it.
        bool        bFaceAlreadyPresent = false;
        Face* const pFace = &(*it).second;

        for(FaceSourceList::const_iterator itFs = pFace->mFaceSourceList.begin(); !bFaceAlreadyPresent && (itFs != pFace->mFaceSourceList.end()); ++itFs)
        {
            const FaceSource& fs = *itFs;

            if(faceSource.mFontDescription == fs.mFontDescription)
            {
                bFaceAlreadyPresent = true;
                if(pFontDescription)
                    *pFontDescription = fs.mFontDescription;
            }
        }

        if(!bFaceAlreadyPresent)
        {
            bReturnValue = true;

            pFace->mFaceSourceList.push_back(faceSource);
            if(pFontDescription)
                *pFontDescription = faceSource.mFontDescription;
        }
    }
    // Else pFontDescription remains unchanged.

    return bReturnValue;
}


///////////////////////////////////////////////////////////////////////////////
// AddFont
//
bool FontServer::AddFont(Font* pFont, FontDescription* pFontDescription)
{
    FontDescription fd;

    if(pFont && pFont->GetFontDescription(fd))  // We assume that a 'true' return value from GetFontDescription means that the font is OK.
    {
        FaceSource faceSource;

        faceSource.mpStream         = pFont->GetStream();
        faceSource.mFontType        = pFont->GetFontType();
        faceSource.mFontDescription = fd;
        faceSource.mnFaceIndex      = 0;

        if(faceSource.mpStream)
            faceSource.mpStream->AddRef(); // AddRef it for FaceSource, which will release it in its dtor.

        pFont->AddRef(); // AddRef it for our usage of it. We will Release it when we are eventually done with it.
        faceSource.mFontList.push_back(pFont);

        return AddFaceSource(faceSource, pFontDescription);
    }

    return false;
}


///////////////////////////////////////////////////////////////////////////////
// EnumerateFonts
//
uint32_t FontServer::EnumerateFonts(FontDescription* pFontDescriptionArray, uint32_t nCount)
{
    uint32_t nDescriptionCount = 0;
    uint32_t nCountWritten = 0;

    for(FaceMap::const_iterator itFM = mFaceMap.begin(); itFM != mFaceMap.end(); ++itFM)
    {
        const FaceMap::value_type& fmPair = *itFM; // Create an alias so it is easier to read.
        const Face& face = fmPair.second;

        for(FaceSourceList::const_iterator itFS = face.mFaceSourceList.begin(); itFS != face.mFaceSourceList.end(); ++itFS)
        {
            const FaceSource& faceSource = *itFS;

            if(nCountWritten < nCount)
                pFontDescriptionArray[nCountWritten++] = faceSource.mFontDescription;
            nDescriptionCount++;
        }
    }

    return nDescriptionCount;
}


///////////////////////////////////////////////////////////////////////////////
// FreeUnusedFonts
//
uint32_t FontServer::FreeUnusedFonts()
{
    uint32_t count = 0;

    for(FaceMap::iterator it = mFaceMap.begin(); it != mFaceMap.end(); ++it) // For each face family we manage (e.g. Arial)...
    {
        Face& face = (*it).second;

        for(FaceSourceList::iterator itFSL = face.mFaceSourceList.begin(), itFSLEnd = face.mFaceSourceList.end(); // For each specific face we manage (e.g. Arial Bold)...
            itFSL != itFSLEnd;
            ++itFSL)
        {
            FaceSource& faceSource = *itFSL;

            for(FontList::iterator itFL = faceSource.mFontList.begin(), itFLEnd = faceSource.mFontList.end(); // For each existing Font we are managing for the given face (e.g. Font of Arial 12)...
                itFL != itFLEnd; )
            {
                Font* pFont = *itFL;
                int   refCount = pFont->AddRef();

                EA_ASSERT(refCount>= 1); // There should always be a value of at least one (our AddRef on it).

                if(refCount == 2) // If the previous RefCount was 1 and thus if we were the only one using it...
                {
                    if(mpGlyphCacheDefault)
                        mpGlyphCacheDefault->RemoveTextureInfo(pFont);

                    pFont->Release();   // Matches above AddRef.
                    pFont->Release();   // Matches original AddRef for the mFontList container.

                    itFL = faceSource.mFontList.erase(itFL);
                    count++;
                }
                else // Else there were more than two, meaning there are additional uses of it outside of our own.
                {
                    pFont->Release();   // Matches above AddRef.
                    ++itFL;
                }
            }
        }
    }

    return count;
}


///////////////////////////////////////////////////////////////////////////////
// AddFace
//
uint32_t FontServer::AddFace(IO::IStream* pStream, FontType fontType, FontDescription* pFontDescription)
{
    uint32_t nFaceSourceCount = 0; // This is the number of successfully added faces for the Font. Usually this value is 0 (on error) or 1.

    EA_ASSERT(pStream);
    pStream->AddRef(); // AddRef for this function.

    FaceSource faceSource;
    faceSource.mpStream = pStream;
    faceSource.mpStream->AddRef(); // AddRef it for FaceSource, which will release it in its dtor.
    faceSource.mFontType = fontType;
    faceSource.mnFaceIndex = 0;

    if(fontType == kFontTypeOutline)
    {
        // In the case of outline (e.g. .ttf) fonts, we have the possibility that multiple
        // font images are stored inside a particular font file. .ttc files are .ttf files
        // that specifically hold collections of TrueType fonts.
        int nFaceSourceCountMax = 32;

        for(int faceIndex = 0; faceIndex < nFaceSourceCountMax; faceIndex++)
        {
            faceSource.mnFaceIndex = (uint8_t)faceIndex;

            // What we do here is create a dummy Font whose purpose is to tell us about
            // the font information. We won't keep this font around, but instead we'll 
            // create specific instances (e.g. per-size) of it later as-needed.
            OutlineFont font;

            font.AddRef(); // AddRef it for its place on the stack here.
            font.SetAllocator(mpCoreAllocator);

            if(mOptions[kOptionOpenTypeFeatures] != -1)
                font.SetOption(Font::kOptionOpenTypeFeatures, mOptions[kOptionOpenTypeFeatures]);
            if(mOptions[kOptionDPI] != -1)
                font.SetOption(Font::kOptionDPI, mOptions[kOptionDPI]);
            if(mOptions[kOptionEnableHinting] != -1)
                font.SetOption(Font::kOptionEnableHinting, mOptions[kOptionEnableHinting]);
            if(mOptions[kOptionUseAutoHinting] != -1)
                font.SetOption(Font::kOptionUseAutoHinting, mOptions[kOptionUseAutoHinting]);
            if(mOptions[kOptionLCD] != -1)
                font.SetOption(Font::kOptionLCD, mOptions[kOptionLCD]);

            // We are using the faceSource variable as a basis for possibly 
            // multiple (but usually just one) FaceSource objects. These FaceSource
            // objects will have identical information except for their mpFaceData.
            // So we vary the mpFaceData for each faceIndex.
            EA_ASSERT(faceSource.mpFaceData == NULL);
            faceSource.mpFaceData = OutlineFont::CreateFaceData(mpCoreAllocator, pStream, NULL, 0, faceIndex);

            if(faceSource.mpFaceData)
            {
                font.SetFaceData(faceSource.mpFaceData);

                if(font.Open(pStream, faceIndex))
                {
                    if(font.GetFontDescription(faceSource.mFontDescription))
                    {
                        if(AddFaceSource(faceSource, pFontDescription))
                            nFaceSourceCount++;

                        // The first time we read a font, we adjust the source count number. 
                         // This is not just an optimization but can fix certain bugs. 
                        if(faceIndex == 0)
                        {
                            #if EATEXT_USE_FREETYPE
                                EA_ASSERT(gFTLibrary && faceSource.mpFaceData && faceSource.mpFaceData->mpFTFace);
                                nFaceSourceCountMax = (int)faceSource.mpFaceData->mpFTFace->num_faces;
                            #endif
                        }
                    }
                    else
                        nFaceSourceCountMax = 0; // Discontinue the loop.
                }
                else
                    nFaceSourceCountMax = 0; // Discontinue the loop.

                faceSource.mpFaceData->Release();
                faceSource.mpFaceData = NULL;
            }
            else
                nFaceSourceCountMax = 0; // Discontinue the loop.
        }
    }
    else if(fontType == kFontTypeBitmap)
    {
        // There can only ever be a single font represented by a .bmpFont file. There can't be multiple faces like with outline fonts.
        // We create temporary dummy font here for the purposes of reading the .bmpFont info and
        // giving us a FontDescription. We'll use that FontDescription later for picking fonts
        // based on a user specification.
        BmpFont font;
        font.AddRef(); // AddRef it for its place on the stack here.
        font.SetAllocator(mpCoreAllocator);

        if(mOptions[kOptionEnableDirectGlyphCacheUse] != -1)
            font.SetOption(Font::kOptionEnableDirectGlyphCacheUse, mOptions[kOptionEnableDirectGlyphCacheUse]);
        if(mOptions[kOptionSystemMemoryCopy] != -1)
            font.SetOption(Font::kOptionSystemMemoryCopy, mOptions[kOptionSystemMemoryCopy]);

        if(font.Open(&pStream, 1)) // Open it with just the .bmpFont stream.
        {
            if(font.GetFontDescription(faceSource.mFontDescription))
            {
                if(AddFaceSource(faceSource, pFontDescription))
                    nFaceSourceCount++;
            }
        }
    }
    else if(fontType == kFontTypePolygon)
    {
        // There can only ever be a single font represented by a .polygonFont file. There can't be multiple faces like with outline fonts.
        // We create temporary dummy font here for the purposes of reading the .polygonFont info and
        // giving us a FontDescription. We'll use that FontDescription later for picking fonts
        // based on a user specification.
        PolygonFont font;
        font.AddRef(); // AddRef it for its place on the stack here.
        font.SetAllocator(mpCoreAllocator);

        if(font.Open(pStream)) // Open it with just the .bmpFont stream.
        {
            if(font.GetFontDescription(faceSource.mFontDescription))
            {
                if(AddFaceSource(faceSource, pFontDescription))
                    nFaceSourceCount++;
            }
        }
    }

    pStream->Release(); // Match the AddRef above for this function.

    return nFaceSourceCount;
}


///////////////////////////////////////////////////////////////////////////////
// AddFace
//
uint32_t FontServer::AddFace(const FilePathChar* pFacePath, FontType fontType, FontDescription* pFontDescription)
{
    #ifdef EA_DEBUG
        const FilePathChar* const pExtension = EA::IO::Path::GetFileExtension(pFacePath); 
        EA_UNUSED(pExtension);

        #if EATEXT_USE_FREETYPE
            // To consider: Add some asserts here.
        #endif
    #endif

    // Thread safey not required here, as we aren't accessing member data.
    uint32_t nFaceSourceCount = 0;

    if(fontType == kFontTypeUnknown)
        fontType = GetFontTypeFromFilePath(pFacePath);

    if(fontType != kFontTypeUnknown)
    {
        EA::IO::EATextFileStream* const pFileStream = CORE_NEW(mpCoreAllocator, EATEXT_ALLOC_PREFIX "FileStream", 0) EA::IO::EATextFileStream(pFacePath);

        if(pFileStream)
        {
            pFileStream->AddRef();
            pFileStream->mpCoreAllocator = mpCoreAllocator;

            if(pFileStream->Open(IO::kAccessFlagRead))
                nFaceSourceCount = AddFace(pFileStream, fontType, pFontDescription);

            pFileStream->Release();
        }
    }

    return nFaceSourceCount;
}


///////////////////////////////////////////////////////////////////////////////
// CreateNewFont
//
Font* FontServer::CreateNewFont(int fontType)
{
    Font* pFont = NULL;

    switch (fontType)
    {
        case kFontTypeOutline:
            pFont = CORE_NEW(mpCoreAllocator, EATEXT_ALLOC_PREFIX "OutlineFont", 0) OutlineFont(mpCoreAllocator); // This is like saying: pFont = new OutlineFont(mpCoreAllocator);
            break;

        case kFontTypeBitmap:
            pFont = CORE_NEW(mpCoreAllocator, EATEXT_ALLOC_PREFIX "BmpFont", 0) BmpFont(mpCoreAllocator);
            break;

        case kFontTypePolygon:
            pFont = CORE_NEW(mpCoreAllocator, EATEXT_ALLOC_PREFIX "PolygonFont", 0) PolygonFont(mpCoreAllocator);
            break;
    }

    if(pFont)
        pFont->AddRef();

    return pFont;
}


///////////////////////////////////////////////////////////////////////////////
// CreateNewFont
//
// This function AddRefs the returned Font for the caller, regardless of the value of bManaged. 
// This function AddRefs the Font internally for internal listkeeping.
//
Font* FontServer::CreateNewFont(FaceSource* pFaceSource, const TextStyle& ssCSS, bool bManaged)
{
    bool  bResult = false;
    Font* pFont   = NULL;

    if(pFaceSource->mFontType == kFontTypeOutline)
    {
        OutlineFont* const pOutlineFont = (OutlineFont*)CreateNewFont(kFontTypeOutline);

        if(pOutlineFont)
        {
            EA_ASSERT((mpCoreAllocator != NULL) && (pFaceSource->mpFaceData != NULL));
            pOutlineFont->SetFaceData(pFaceSource->mpFaceData);

            if(mOptions[kOptionOpenTypeFeatures] != -1)
                pOutlineFont->SetOption(Font::kOptionOpenTypeFeatures, mOptions[kOptionOpenTypeFeatures]);
            if(mOptions[kOptionDPI] != -1)
                pOutlineFont->SetOption(Font::kOptionDPI, mOptions[kOptionDPI]);
            if(mOptions[kOptionEnableHinting] != -1)
                pOutlineFont->SetOption(Font::kOptionEnableHinting, mOptions[kOptionEnableHinting]);
            if(mOptions[kOptionUseAutoHinting] != -1)
                pOutlineFont->SetOption(Font::kOptionUseAutoHinting, mOptions[kOptionUseAutoHinting]);
            if(mOptions[kOptionLCD] != -1)
                pOutlineFont->SetOption(Font::kOptionLCD, mOptions[kOptionLCD]);

            bResult = pOutlineFont->Open(pFaceSource->mpStream, pFaceSource->mnFaceIndex);

            if(bResult)
            {
                EA_ASSERT(ssCSS.mfSize > 0);
                pOutlineFont->SetTransform(ssCSS.mfSize, 0.f, 0.f);
                pOutlineFont->SetSmoothing(ssCSS.mSmooth);

                if(ssCSS.mEffect != kEffectNone)
                    pOutlineFont->SetEffect(ssCSS.mEffect, ssCSS.mfEffectX, ssCSS.mfEffectY, ssCSS.mEffectBaseColor, ssCSS.mEffectColor, ssCSS.mHighLightColor);
            }

            pFont = pOutlineFont;
        }
    }
    else if(pFaceSource->mFontType == kFontTypeBitmap)
    {
        BmpFont* const pBmpFont = (BmpFont*)CreateNewFont(kFontTypeBitmap);

        if(pBmpFont)
        {
            if(mOptions[kOptionEnableDirectGlyphCacheUse] != -1)
                pBmpFont->SetOption(Font::kOptionEnableDirectGlyphCacheUse, mOptions[kOptionEnableDirectGlyphCacheUse]);
            if(mOptions[kOptionSystemMemoryCopy] != -1)
                pBmpFont->SetOption(Font::kOptionSystemMemoryCopy, mOptions[kOptionSystemMemoryCopy]);

            // We set pBmpFont to use the global GlyphCache, but unilaterally doing this is not
            // how we would like our system to work. The user should have higher level control
            // over the association (or lack thereof) of glyph caches with BmpFonts. 
            GlyphCache* const pGlyphCache = pBmpFont->GetGlyphCache();
            if(!pGlyphCache)
                pBmpFont->SetGlyphCache(mpGlyphCacheDefault);

            bResult = pBmpFont->Open(&pFaceSource->mpStream, 1);

            // Without understand the given application's storage system, we don't know how to 
            // read named streams except when they are native system disk file paths. The user
            // can override this class to provide extended behaviour. 

            if(bResult)
            {
                EATextPathString sDirectory;
                BmpTextureInfo*  pBmpTextureInfo = pBmpFont->GetBitmapTextureInfo(0);
                const uint32_t   streamType = pFaceSource->mpStream->GetType();

                if(pBmpTextureInfo && (streamType == EA::IO::FileStream::kTypeFileStream) && // If the name of the BmpFont's (first) texture is just a file name and not a path (as is typically the case)...
                   (EA::IO::Path::GetFileName(pBmpTextureInfo->mTextureFilePath.c_str()) == pBmpTextureInfo->mTextureFilePath.c_str()))
                {
                    // We assume that the Bmp textures are in the same directory as the main Bmp font file.
                    EA::IO::FileStream* const pFileStream = static_cast<EA::IO::FileStream*>(pFaceSource->mpStream);
                    sDirectory.resize(IO::kMaxDirectoryLength);
                    pFileStream->GetPath(&sDirectory[0], IO::kMaxDirectoryLength);
                    sDirectory.resize((eastl_size_t)(EA::IO::Path::GetFileName(sDirectory.c_str()) - sDirectory.c_str()));
                }

                // Here we load the BmpFont textures. At this time we only know how to read
                // the textures from file streams (i.e. disk files). However, the user can 
                // override this function to provide a different interpretation of the font source name.
                const uint32_t count = pBmpFont->GetTextureCount();

                for(uint32_t i = 0; i < count; i++)
                {
                    pBmpTextureInfo = pBmpFont->GetBitmapTextureInfo(i);

                    EA_ASSERT(pBmpTextureInfo);
                    if(pBmpTextureInfo)
                    {
                        pBmpTextureInfo->mTextureFilePath.insert(0, sDirectory);

                        EA::IO::EATextFileStream* const pTextureStream = CORE_NEW(mpCoreAllocator, EATEXT_ALLOC_PREFIX "BmpFont", 0) EA::IO::EATextFileStream(pBmpTextureInfo->mTextureFilePath.c_str());

                        if(pTextureStream)
                        {
                            pTextureStream->AddRef(); // AddRef it for this function.
                            pTextureStream->mpCoreAllocator = mpCoreAllocator;

                            if(pTextureStream->Open(IO::kAccessFlagRead))
                                pBmpFont->ReadBmpTexture(pTextureStream, i);

                            pTextureStream->Release(); // Release it for this function.
                        }
                    }
                }
            }

            pFont = pBmpFont;
        }
    }
    else if(pFaceSource->mFontType == kFontTypePolygon)
    {
        PolygonFont* const pPolygonFont = (PolygonFont*)CreateNewFont(kFontTypePolygon);

        if(pPolygonFont)
        {
            bResult = pPolygonFont->Open(pFaceSource->mpStream);

            if(bResult)
                pFont = pPolygonFont; 
        }
    }

    if(bResult)
    {
        pFont->AddRef(); // AddRef it for the caller.

        if(bManaged)
        {
            pFont->AddRef(); // AddRef it for our usage of it. We will Release it when it is removed from pFaceSource->mFontList.
            pFaceSource->mFontList.push_back(pFont);
        }
    }

    if(pFont)
        pFont->Release(); // Matches the AddRef above for this function.

    return pFont;
}

///////////////////////////////////////////////////////////////////////////////
// RemoveFace
//
uint32_t FontServer::RemoveFace(const Char* pFamilyName)
{
    uint32_t count = 0;

    for(FaceMap::iterator it = mFaceMap.begin(); it != mFaceMap.end(); ) // For each face family we manage (e.g. Arial)...
    {
        const Face& face = (*it).second;

        if(Stricmp(face.mFamily, pFamilyName) == 0) // If this face matches the user-specified face by name...
        {
            // Here we iterate each Font associated with the given Face and clear if from the GlyphCache.
            // Usually there are at most about five Fonts to deal with, and rarely are there more than ten or so.

            if(mpGlyphCacheDefault)
            {
                for(FaceSourceList::const_iterator itFSL = face.mFaceSourceList.begin(), itFSLEnd = face.mFaceSourceList.end();// For each specific face we manage (e.g. Arial Bold)...
                    itFSL != itFSLEnd;
                    ++itFSL)
                {
                    const FaceSource& faceSource = *itFSL;

                    for(FontList::const_iterator itFL = faceSource.mFontList.begin(), itFLEnd = faceSource.mFontList.end(); // For each existing Font we are managing for the given face (e.g. Font of Arial 12)...
                        itFL != itFLEnd;
                        ++itFL)
                    {
                        const Font* pFont = *itFL;
                    
                        mpGlyphCacheDefault->RemoveTextureInfo(pFont);
                    }
                }
            }

            it = mFaceMap.erase(it);
            count++;
        }
        else
            ++it;
    }

    return count;
}


///////////////////////////////////////////////////////////////////////////////
// AddEffect
//
void FontServer::AddEffect(uint32_t effectId, const EIWord* pInstructionList, uint32_t instructionCount)
{
    EA_ASSERT(instructionCount <= kEffectInstructionListCapacity);

    RemoveEffect(effectId);

    mEffectDataList.push_back();
    EffectData& effectData = mEffectDataList.back();
    effectData.mEffectId = effectId;
    memcpy(effectData.mEffectInstructions, pInstructionList, instructionCount * sizeof(uint32_t));
    effectData.mEffectInstructionCount = instructionCount;
}


///////////////////////////////////////////////////////////////////////////////
// RemoveEffect
//
void FontServer::RemoveEffect(uint32_t effectId)
{
    for(EffectDataList::iterator it = mEffectDataList.begin(); it != mEffectDataList.end(); ++it)
    {
        const uint32_t id = it->mEffectId;

        if(effectId == id)
        {
            mEffectDataList.erase(it);
            break;
        }
    }
}


///////////////////////////////////////////////////////////////////////////////
// GetEffect
//
bool FontServer::GetEffect(uint32_t effectId, const EIWord*& pInstructionList, uint32_t& instructionCount) const
{
    for(EffectDataList::const_iterator it = mEffectDataList.begin(), itEnd = mEffectDataList.end(); it != itEnd; ++it)
    {
        const EffectData& effectData = *it;

        if(effectData.mEffectId == effectId)
        {
            pInstructionList = effectData.mEffectInstructions;
            instructionCount = effectData.mEffectInstructionCount;
            return true;
        }
    }

    pInstructionList = NULL;
    instructionCount = 0;
    return false;
}


///////////////////////////////////////////////////////////////////////////////
// AddSubstitution
//
#if EATEXT_FAMILY_SUBSTITUTION_ENABLED

    bool FontServer::AddSubstitution(const Char* pFamily, const Char* pFamilySubstitution)
    {
        FontServerString sFamilyLowerCase(pFamily);
        sFamilyLowerCase.make_lower();

        EA_ASSERT(sFamilyLowerCase.length() <= kMaxFamilyNameSize);
        if(sFamilyLowerCase.length() <= kMaxFamilyNameSize)
        {
            FontServerString sSubstitutionLowerCase(pFamilySubstitution);
            sSubstitutionLowerCase.make_lower();

            EA_ASSERT(sSubstitutionLowerCase.length() <= kMaxFamilyNameSize);
            if(sSubstitutionLowerCase.length() <= kMaxFamilyNameSize)
            {
                const FamilySubstitutionMap::const_iterator it = mFamilySubstitutionMap.find(sFamilyLowerCase);

                if(it == mFamilySubstitutionMap.end())
                {
                    const FamilySubstitutionMap::value_type entry(sFamilyLowerCase, sSubstitutionLowerCase);
                    mFamilySubstitutionMap.insert(entry);
                }
            }
        }

        return false;
    }

#endif // EATEXT_FAMILY_SUBSTITUTION_ENABLED




#endif // EATEXT_FONT_SERVER_ENABLED




} // namespace Text

} // namespace EA











