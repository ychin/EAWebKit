/*
Copyright (C) 2006, 2009, 2010, 2012 Electronic Arts, Inc.  All rights reserved.

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
// EATextBmpFont.h
//
// By Sumedha Kshirsagar and Paul Pedriana.
///////////////////////////////////////////////////////////////////////////////


#ifndef EATEXT_EATEXTBMPFONT_H
#define EATEXT_EATEXTBMPFONT_H


#include <EAText/EAText.h>
#include <EAText/EATextFont.h>
#include <EAText/EATextCache.h>
#include <EAIO/EAStream.h>
#include <EAIO/EAStreamAdapter.h>
#include <EASTL/map.h>
#include <EASTL/vector.h>
#include <EASTL/fixed_vector.h>
#include <EASTL/fixed_string.h>


namespace EA
{
    namespace IO
    {
        class IStream;
    }

    namespace Text
    {
        /// PathString
        /// 
        /// A generic fixed-size path string.
        ///
        const size_t kPathStringLength = 128;
        typedef eastl::fixed_string<FilePathChar, kPathStringLength + 1, true> EATextPathString;


        /// BitmapFont specific data that has info about the texture and glyphs on it.
        struct EATEXT_API BmpGlyphMetrics : public GlyphMetrics
        {
            int mnTextureIndex :  8;  /// Which bitmap/texture this glyph is on.
            int mnPositionX    : 12;  /// The coordinate origin of this bitmap on the texture.
            int mnPositionY    : 12;  /// The coordinate origin of this bitmap on the texture.
        };


        /// A bitmap font may have more than one textures and this structure 
        /// stores information about the texture.
        struct EATEXT_API BmpTextureInfo : public TextureInfo
        {
            EATextPathString mTextureFilePath;   /// This may be a disk file path, but it can be any string which identifies the name of the file data or texture.
            uint32_t         mnTextureFileSize;  /// Size of the file data stream. May be zero if it is not used or needed.
          //PixelData        mPixelData;         /// Moved to the base class (TextureInfo).

            BmpTextureInfo();
           ~BmpTextureInfo();
        };


        /// BmpFont
        ///
        /// This BmpFont class represents a bitmapped font, as stored in a bmpFont file. 
        /// Thus BmpFont is an interface to a bitmapped font and has functionality needed to load that font 
        /// and support the reading of glyphs from that font. 
        ///
        /// Currently, caching is not implemented within the class, though it probably maked more sense
        /// to do so, for unlike outlined fonts, the bitmapped font inherently has the "rendered" glyph data.
        /// 
        class EATEXT_API BmpFont : public Font
        {
        public:
            /// BmpFont
            ///
            /// Constructor. The Font constructor merely initializes 
            /// member data to zero. No significant operations are done. 
            ///
            BmpFont(Allocator::ICoreAllocator* pCoreAllocator = NULL);
            BmpFont(const BmpFont&);
            BmpFont& operator=(const BmpFont&);

            EATEXT_VIRTUAL ~BmpFont();


            /// SetAllocator
            EATEXT_VIRTUAL void SetAllocator(Allocator::ICoreAllocator* pCoreAllocator);


            /// Open
            /// 
            /// Initializes a BmpFont for usage with data supplied by the 
            /// stream functions. Returns true if the data source could be 
            /// read and the Font could be initialized for usage. The Streams 
            /// are AddRefd by the font for its usage and is Released upon 
            /// completion of usage. Upon failure, no streams will be AddRefd.
            ///
            /// pStreamArray[0] must refer to the .bmpFont file itself.
            /// pStreamArray[1+] refers to any texture/bitmap files for the font.
            /// The user can call this function with multiple streams or the 
            /// user can call this function with streamCount = 1 and then call
            /// SetBmpTexture or ReadBmpTexture manually thereafter. If you 
            /// use more than one stream (the .bmpFont file), then Open will call
            /// its member ReadBmpTexture function (which you can override) once 
            /// for each stream (presumed to be texture streams) after the first.
            ///
            EATEXT_VIRTUAL bool Open(IO::IStream* pStreamArray[], uint32_t streamCount);


            /// Close
            ///
            /// Closes the font and frees any resources allocated by it 
            /// after being opened. The Font is returned to the same essential 
            /// state that it was before being Opened. The font must be opened 
            /// or the return value will be false. If the font is not closed 
            /// before it is destroyed, it will be closed upon its final Release.
            ///
            EATEXT_VIRTUAL bool Close();


            /// SetOption
            ///
            /// kOptionOpenTypeFeatures must be set before the font is opened.
            /// kOptionDPI must be set before the font is opened.
            /// See enum Font::Option for more details about available options.
            ///
            EATEXT_VIRTUAL void SetOption(int32_t option, int32_t value);


            /// GetFontType
            ///
            /// Returns kFontTypeBitmap if font is opened, else kFontTypeUnknown. 
            ///
            EATEXT_VIRTUAL FontType GetFontType();


            /// GetGlyphCache
            ///
            /// Returns the GlyphCache associated with the BmpFont, which may be 
            /// NULL if this BmpFont hasn't been assigned a GlyphCache or is intended
            /// to be used standalone without one.
            ///
            EATEXT_VIRTUAL GlyphCache* GetGlyphCache() const;


            /// SetGlyphCache
            ///
            /// Sets the GlyphCache used by this BmpFont. This function must be called
            /// before this font is Opened. When a GlyphCache is associated with a BmpFont
            /// the BmpFont puts its textures and glyph info in the GlyphCache (in read-only form)
            /// so that the BmpFont can be used transparently alongside other fonts such 
            /// as outline fonts.
            ///
            EATEXT_VIRTUAL void SetGlyphCache(GlyphCache* pGlyphCache);


            /// GetFontDescription
            ///
            /// Fills in a FontDescription for the user.
            /// The font must be opened or the return value will be false.
            ///
            EATEXT_VIRTUAL bool GetFontDescription(FontDescription& fontDescription);


            /// GetFontMetrics
            ///
            /// Fills in a FontMetrics for the user.
            /// The font must be opened or the return value will be false.
            ///
            EATEXT_VIRTUAL bool GetFontMetrics(FontMetrics& fontMetrics);


            /// GetBitmapFormat
            ///
            /// Returns the BitmapFormat of the font.
            ///
            EATEXT_VIRTUAL BitmapFormat GetBitmapFormat() const;


            /// GetTextureCount
            ///
            /// Returns the number of textures this font has.
            ///
            EATEXT_VIRTUAL uint32_t GetTextureCount() const;


            /// GetBmpTextureInfo
            ///
            /// The returned TextureInfo* is not AddRefd for the user.
            ///
            EATEXT_VIRTUAL BmpTextureInfo* GetBmpTextureInfo(uint32_t textureIndex);
            #define GetBitmapTextureInfo GetBmpTextureInfo  // Backwards compatibility, to be removed some time in the year 2008.


            /// SetBmpTextureInfo
            ///
            /// Sets or replaces the BmpTextureInfo at the index given by textureIndex.
            /// If pBmpTextureInfo already is set at the given textureIndex, this function does nothing.
            ///
            EATEXT_VIRTUAL void SetBmpTextureInfo(uint32_t textureIndex, BmpTextureInfo* pBmpTextureInfo);


            /// SetBmpTexture
            ///
            /// This function lets you poke textures into BmpFonts. This is useful when
            /// you want to handle the reading of BmpFont textures manually as opposed to 
            /// having it happen within the BmpFont class. It may be useful to use the
            /// GetBmpTextureInfo function to retrieve information about the textures 
            /// before manually loading them and calling this SetBmpTexture function.
            /// 
            /// The textureIndex parameter is a 0-based index that must refer to a valid
            /// texture index. Note that the valid texture indexes are determined in the 
            /// Open function while reading the .bmpFont data.
            ///
            /// The texture and format parameters correspond to the TextureInfo::mTexture and 
            /// TextureInfo::mFormat variables, respectively.
            ///
            EATEXT_VIRTUAL void SetBmpTexture(uint32_t textureIndex, uintptr_t texture, uint32_t format);


            /// ReadBmpTexture
            ///
            /// Adds a Bmp texture to the font dynamically. 
            /// This function is automatically called by the Open function if you specify
            /// more than one stream to the Open function and will be called with the 
            /// second stream passed to Open with textureIndex == 0, then the third stream
            /// passed to Open with textureIndex == 1, etc. 
            /// You can also call this function manually instead of via Open if you want.
            /// You may want or need to override this function in a subclass in order to 
            /// provide a custom mechanism for how to read a texture from a stream.
            /// You can dispense with the need for this function altogether if you instead
            /// use the SetBmpTexture function. 
            ///
            EATEXT_VIRTUAL bool ReadBmpTexture(IO::IStream* pStream, uint32_t textureIndex);


            /// GetGlyphMetrics
            ///
            /// Fills in a GetGlyphMetrics for the user.
            /// The font must be opened or the return value will be false. 
            ///
            EATEXT_VIRTUAL bool GetGlyphMetrics(GlyphId glyphId, GlyphMetrics& glyphMetrics);


            /// GetGlyphIds
            ///
            /// Function to map code points to glyph indexes.
            /// Similar to Windows ScriptGetCMap function.
            /// Unsupported glyphs are filled in with the replacement glyph if bUseReplacementGlyph is true, otherwise (GlyphId)-1.
            /// The nGlyphIdStride parameter allows the user to write to an array of GlyphDisplayEntry (or some other format)
            /// instead of just an array of GlyphId.
            /// Returns the number of glyphs written to pGlyphIdArray.
            /// The font must be opened or the return value will be 0.
            /// If pGlyphIdArray is NULL, the glyphs are not written to pGlyphIdArray.
            /// This function is also useful for telling if a code point is covered by a font.
            ///
            EATEXT_VIRTUAL uint32_t GetGlyphIds(const Char* pCharArray, uint32_t nCharArrayCount, GlyphId* pGlyphIdArray = NULL, 
                                                bool bUseReplacementGlyph = true, const uint32_t nGlyphIdStride = sizeof(GlyphId), bool bWriteInvalidGlyphs = false);

            EATEXT_VIRTUAL uint32_t GetGlyphIds(const char8_t* pCharArray, uint32_t nCharArrayCount, GlyphId* pGlyphIdArray = NULL, 
                                                bool bUseReplacementGlyph = true, const uint32_t nGlyphIdStride = sizeof(GlyphId), bool bWriteInvalidGlyphs = false);


            /// GetSize
            ///
            /// Returns the size (pixel height) of the font. The size is based on the transform.
            /// This function merely returns the FontDescription.mfSize member, which you 
            /// can also retrieve via GetFontDescription().
            ///
            EATEXT_VIRTUAL float GetSize() const;


            /// SetTransform
            /// 
            /// SetTransform is not needed for bitmapped fonts, but is present 
            /// in order to override the base class versions.
            /// This implementation always returns false.
            ///
            EATEXT_VIRTUAL bool SetTransform(float f00, float f01, float f10, float f11);
            EATEXT_VIRTUAL bool SetTransform(float fSize, float fAngle = 0.f, float fSkew = 0.f);


            /// GetGlyphTextureInfo
            ///
            /// Gets the GlyphTextureInfo for the given GlyphId. 
            /// BmpFonts, unlike OutlineFonts, are completely pregenerated and you should never
            /// need to render them (you should never need to call RenderGlyphBitmap).
            /// Note that GlyphId is not the same thing as a Unicode Char.
            ///
            EATEXT_VIRTUAL bool GetGlyphTextureInfo(GlyphId glyphId, GlyphTextureInfo& glyphTextureInfo) const;


            /// RenderGlyphBitmap
            ///
            /// You shouldn't normally need to call this function, as BitmapFonts by definition
            /// are pre-rendered. However, a subclass of BmpFont may want to override this 
            /// function and use it as a hint that a given glyph is needed and to dynamically 
            /// load the texture that contains it. This function fills in the GlyphBitmap with
            /// the BmpTextureInfo::PixelData (a system memory copy of the texture), but does 
            /// nothing and returns false if this data is empty, which could well be the case 
            /// if you are using only a pre-specified GPU textures (e.g. via SetBmpTexture).
            ///
            EATEXT_VIRTUAL bool RenderGlyphBitmap(const GlyphBitmap** pGlyphBitmap, GlyphId g, 
                                                  uint32_t renderFlags = kRFDefault, float fXFraction = 0, float fYFraction = 0);


            /// DoneGlyphBitmap
            ///
            /// You must call this function after calling RenderGlyphBitmap.
            ///
            EATEXT_VIRTUAL void DoneGlyphBitmap(const GlyphBitmap* pGlyphBitmap);


            /// GetOTF
            ///
            /// Returns the OpenTypeFont information for this Font, if any is available.
            /// OpenType font information is what's used to layout text in complex scripts
            /// such as Thai or Arabic. 
            ///
            EATEXT_VIRTUAL OTF* GetOTF()
                { return NULL; }


            /// IsCharSupported
            ///
            EATEXT_VIRTUAL bool IsCharSupported(Char c, Script script);


            /// GetKerning
            ///
            /// Retrieve custom kerning information for the given glyph pair.
            ///
            EATEXT_VIRTUAL bool GetKerning(GlyphId g1, GlyphId g2, Kerning& kerning, int direction, bool bHorizontalLayout = true);

        protected:
            typedef eastl::hash_map<GlyphId, BmpGlyphMetrics, eastl::hash<uint32_t>,
                                 eastl::equal_to<GlyphId>, EA::Allocator::EASTLICoreAllocator> BmpGlyphMetricsMap;

            typedef eastl::pair<GlyphId, GlyphId> GlyphIdPair;

            typedef eastl::map<GlyphIdPair, Kerning, eastl::less<GlyphIdPair>, 
                                            EA::Allocator::EASTLICoreAllocator> KerningMap;

            typedef eastl::fixed_vector<BmpTextureInfo*, 4, true> TextureInfoArray;

            typedef eastl::map<Char, GlyphId, eastl::less<Char>, 
                                EA::Allocator::EASTLICoreAllocator> CharGlyphMap;
        
        protected:
            bool ReadBmpFontInfo(IO::IStream* pStream);
            void RegisterGlyphMetrics();

        protected:
            FontDescription     mFontDescription;
            FontMetrics         mFontMetrics;
            GlyphBitmap         mGlyphBitmap;
            BmpGlyphMetricsMap  mBmpGlyphMetricsMap;
            KerningMap          mKerningMap;
            TextureInfoArray    mTextureInfoArray;           // AddRefd pointers to BmpTextureInfo.
            CharGlyphMap        mCharGlyphMap;
            GlyphCache*         mpGlyphCache;                // GlyphCache to be associated with. If NULL then we don't store our textures in a GlyphCache. 
            bool                mbEnableDirectGlyphCacheUse; // If true, we use our image directly as a glyph cache texture as opposed to using our image as a source of glyphs which get copied to the global glyph cache texture system.
            bool                mbSystemMemoryCopy;          // If true, we keep a copy of the texture(s) in system memory and RenderGlyphBitmap returns a pointer to the system memory.

        }; // class BmpFont

    } // namespace Text

} // namespace EA




#endif // Header include guard

















