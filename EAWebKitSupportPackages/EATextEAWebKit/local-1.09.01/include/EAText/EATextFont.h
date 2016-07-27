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
// EATextFont.h
//
// By Paul Pedriana - 2004
//
///////////////////////////////////////////////////////////////////////////////


#ifndef EATEXT_EATEXTFONT_H
#define EATEXT_EATEXTFONT_H


#include <EABase/eabase.h>
#include <EAText/EAText.h>
#include <EAText/EATextStyle.h>
#include <EAText/EATextScript.h>
#include <EASTL/utility.h>
#include <EASTL/hash_map.h>
#include <EASTL/bitset.h>
#include <EASTL/core_allocator_adapter.h>
#include <coreallocator/icoreallocator_interface.h>
#include <EAIO/EAStream.h>

#ifdef _MSC_VER
    #pragma warning(push)           // We have to be careful about disabling this warning. Sometimes the warning is meaningful; sometimes it isn't.
    #pragma warning(disable: 4251)  // class (some template) needs to have dll-interface to be used by clients.
#endif


namespace EA
{
    namespace Allocator
    {
        class ICoreAllocator;

        typedef CoreAllocatorAdapter<ICoreAllocator> EASTLICoreAllocator;
    }


    namespace Text
    {
        // Forward declarations
        class OTF; // OpenTypeFont info


        /// FontMetrics
        ///
        /// Font metrics describe aspects of the font as a whole. The same metrics work for 
        /// both vertical and horizontal text layout.
        ///
        struct EATEXT_API FontMetrics
        {
            float mfSize;                   /// The classic font size metric; what you see in the text editor 'size' box.
            Pitch mPitch;                   /// Specifies if the font is of variable or fixed pitch (i.e. monospaced).
            float mfHAdvanceXMax;           /// For fixed pitch fonts, this is the advance for all non-zero-advance glyphs.
            float mfVAdvanceYMax;           /// For fixed pitch fonts, this is the advance for all non-zero-advance glyphs. May be equal to mfLineHeight for fonts without vertical metrics.
            float mfAscent;                 /// Max ascent value for the font. This will normally be >= 0. Covers the distance from the baseline to the top of the highest lower-case glyph (usually 'b').
            float mfDescent;                /// Min descent value for the font. This will normally be <= 0. Covers the distance from the baseline to the bottom of the lowest glyph. Bottoms of characters such as j, p, and q would be in this area.
            float mfLeading;                /// The extra space between the bottom of the descent and the top of the next line of text below the current one. Leading is often zero.
            float mfBaseline;               /// The distance from the top of a line down to the baseline. Equal to mfAscent + mfLeading.
            float mfLineHeight;             /// The height of a line of text. Equal to mfDescent + mfBaseline. Also equal to mfDescent + mfAscent + mfLeading.
            float mfXHeight;                /// The height of lower-case Latin glyphs (usually 'x').
            float mfCapsHeight;             /// The height of top of upper-case Latin glyphs (usually 'H').
            float mfUnderlinePosition;      /// The position is relative to baseline; negative values mean below the baseline. Refers to the center of the line. For a thickness of two, position is the top pixel.
            float mfUnderlineThickness;     /// Thickness of the line. 
            float mfLinethroughPosition;    /// The position is relative to baseline. Refers to the center of the line. For a thickness of two, position is the top pixel.
            float mfLinethroughThickness;   /// Thickness of the line. 
            float mfOverlinePosition;       /// The position is relative to baseline. Refers to the center of the line. For a thickness of two, position is the top pixel.
            float mfOverlineThickness;      /// Thickness of the line.

            FontMetrics() { memset(this, 0, sizeof(*this)); }
        };


        /// GlyphMetrics
        ///
        /// Glyph metrics describe aspects of a particular glyph within a font. 
        /// The metrics use a coordinate system in which the x axis is positive as 
        /// it goes to the right and the y axis is positive as it goes upward. 
        /// This coordinate system can be said to have an origin at the "pen position".
        /// These glyph metrics apply to vertically oriented text as well as horizontally
        /// oriented text. The first line of each description applies to both vertical
        /// and horizontal text. 
        ///
        struct EATEXT_API GlyphMetrics
        {
            float mfSizeX;            /// The width of the glyph.
            float mfSizeY;            /// The height of the glyph.
            float mfHBearingX;        /// The distance from the origin ("pen position") to the left of the glyph. This usually is a value >= 0. May be negative for glyphs that are diacritics which modify the character before them.
            float mfHBearingY;        /// The distance from the origin to the top of the glyph. This is usually a value >= 0. May be negative for glyphs that are entirely below the baseline (horizontal text).
            float mfHAdvanceX;        /// The distance from the origin to the origin of the next glyph. This is usually a value > 0.

            #if EATEXT_VERTICAL_ENABLED
                float mfVBearingX;    /// X bearing for when text is layed out vertically.
                float mfVBearingY;    /// Y bearing for when text is layed out vertically.
                float mfVAdvanceY;    /// Advance for when text is layed out vertically.
            #endif
        };

        typedef eastl::pair<const GlyphId, GlyphMetrics> GlyphMetricsMapPair;

        /// GlyphMetricsMap
        ///
        /// A map of all cached glyphs to their metrics information.
        ///
        typedef eastl::hash_map<GlyphId, GlyphMetrics, eastl::hash<uint32_t>, eastl::equal_to<GlyphId>, EA::Allocator::EASTLICoreAllocator> GlyphMetricsMap;


        /// SupportedScriptSet
        ///
        /// This is a bitset of all supportable scripts. A given font may store
        /// a SupportedScriptSet in order to do quick character coverage lookups.
        ///
        typedef eastl::bitset<64> SupportedScriptSet;


        /// Kerning
        ///
        /// Provides information for the kerning of character pairs. 
        /// A kerning value for a font is an adjustment of the normal advance vector for a 
        /// given glyph to the next glyph. Thus, a zero value means that there is no adjustment 
        /// and the glyphs are regularly drawn; a negative value means the glyphs are made closer 
        /// together than normal, and a positive value means the glyphs are farther apart than normal.
        ///
        struct EATEXT_API Kerning
        {
            float mfKernX;
            float mfKernY;
        };


        /// FontType
        ///
        /// Defines the font's data source type. 
        ///
        enum FontType
        {
            kFontTypeUnknown,   /// The font type is not known.
            kFontTypeBitmap,    /// The glyphs are derived from pregenerated bitmaps.
            kFontTypeOutline,   /// The glyphs are derived from outlines (such as Bezier). TrueType, PostScript, and OpenType fonts are outline fonts, though they allow for embedding of bitmaps. 
            kFontTypeStroke,    /// The glyphs are derived from strokes. This is common for East Asian kanji.
            kFontTypePolygon,   /// The glyphs are derived from polygons.
        };

        EATEXT_API FontType GetFontTypeFromFilePath(const FilePathChar* pFontFilePath);



        /// FontDescription
        ///
        /// Not all aspects of a font are embodied by a FontDescription. For example, a FontDescription includes
        /// the size, style, and weight of a font but doesn't include draw-specific options such as the 
        /// antialiasing of a font, the color of the font, or the alignment of a particular string drawing 
        /// operation. It also doesn't include modifier information such as underlining or linethrough, 
        /// as these properties can be applied to any font as it is used.
        ///
        struct EATEXT_API FontDescription
        {
            Char      mFamily[kFamilyNameCapacity]; /// Family name to which the font belongs. For example, Arial.
            float     mfSize;                       /// Font size. If size is not applicable, the value is 0.
            Style     mStyle;                       /// CSS Style type. For example, kStyleOblique.
            float     mfWeight;                     /// CSS Weight type. For example, kWeightBold.
            float     mfStretch;                    /// Stretch scaling value. 1.0 means neither stretched nor condensed.
            Pitch     mPitch;                       /// Pitch type. Tells if font is variable or fixed pitch. ** Perhaps this should be part of the family instead of the font.
            Variant   mVariant;                     /// CSS Variant type. For example, kVariantSmallCaps.
            Smooth    mSmooth;                      /// CSS Smooth type. For example, kSmoothEnabled.
            uint32_t  mEffect;                      /// To support outline, shadow, raised and depressed 
            float     mfEffectX;
            float     mfEffectY;
            Color     mEffectBaseColor;             /// Color of the text, this needs be here because we are rendering color glyphs
            Color     mEffectColor;                 /// If mEffect is outline, this is outline color, if effect is shadow, this shadow color
            Color     mHighLightColor;              /// used for shadow, rasied and depressed effect

            FontDescription()
              : mfSize(0.f), mStyle(kStyleNormal), mfWeight(kWeightNormal), mfStretch(1.f), 
                mPitch(kPitchVariable), mVariant(kVariantNormal), mSmooth(kSmoothNone), mEffect(kEffectNone),
                mfEffectX(1.f), mfEffectY(1.f), mEffectBaseColor(0xffffffff), mEffectColor(0xff000000), mHighLightColor(0xffffffff) { mFamily[0] = 0; }

            bool operator==(const FontDescription& fd) const;
        };



        /// Font
        ///
        /// Provides a base font definition and interface.
        ///
        class EATEXT_API Font
        {
        public:
            /// BitmapFormat
            /// 
            /// Defines bitmap formats supported by this class.
            /// A given font will generate only a single BitmapFormat.
            /// A different font might generate some other BitmapFormat.
            ///
            enum BitmapFormat
            {
                kBFNone       =  0,     /// No format in particular.
                kBFMonochrome =  1,     /// 1 bit per pixel, no compression aside from the bits packed in the bytes.
                kBFGrayscale  =  8,     /// 8 bits per pixel, no compression. Each pixel represents opacity.
                kBFARGB       = 32,     /// 32 bits per pixel, no compression. Format is 32 bit 0xaarrggbb.
                kBFRGBA       = 33,     /// 32 bits per pixel, no compression. Format is 32 bit 0xrrggbbaa.
                kBFABGR       = 34      /// 32 bits per pixel, no compression. Format is 32 bit 0xaabbggrr.
            };

            /// RenderFlags
            /// 
            /// This is used to control settings for how the RenderGlyphBitmap and RenderGlyphOutline functions work.
            ///
            enum RenderFlags
            {
                kRFNone       = 0,     /// No flag in particular.
                kRFUnicodeId  = 1,     /// The glyphId is to be interpreted as a Unicode character instead of a glyphId. This is slower but OK for testing purposes.
                kRFDefault    = kRFNone
            };

            /// Option
            /// 
            /// Note that a subclass may define additional options beyond options defined here.
            ///
            enum Option
            {
                kOptionNone                       = 0,    /// 
                kOptionOpenTypeFeatures           = 1,    /// OpenType feature table support. Default is enabled. If EATEXT_OPENTYPE_ENABLED == 0 then OpenType features are unilaterally disabled.
                kOptionDPI                        = 2,    /// The DPI the font works in. Default is the same as EATEXT_DPI.
                kOptionEnableHinting              = 3,    /// Enable hinting with hinted fonts. Default is enabled.
                kOptionUseAutoHinting             = 4,    /// Use auto-generated hinting, even if the font has intrinsic hinting. Default is disabled.
                kOptionLCD                        = 5,    /// Target LCD displays. Default is disabled.
                kOptionEnableDirectGlyphCacheUse  = 6,    /// Currently used by BmpFonts only. Default is disabled. If enabled then fonts directly store their images as individual glyph cache textures. Otherwise glyphs are copied as-needed from the images to the global (usually singleton) glyph cache texture. With systems like UTFWin you have to disable this option, as UTFWin doesn't support multiple glyph cache textures at a time, as of this writing (2/2011).
                kOptionSystemMemoryCopy           = 7     /// Currently used by BmpFonts only. Default is enabled. If enabled then fonts keep a copy of their images in system memory and load it on startup.
            };

            /// GlyphRepresentation
            /// 
            /// This structure defines a Glyph representation as rasterized by the RenderGlyph functions.
            /// Previously this was called GlyphBitmap, but it now supports polygonal glyphs as well as
            /// bitmapped glyphs and so GlyphRepresentation is a better name.
            ///
            struct GlyphRepresentation
            {
                uint32_t      mnWidth;          /// Width of the physical glyph, uint32_t version of mGlyphMetrics.mfSizeX. Not the same as advance width. 
                uint32_t      mnHeight;         /// Height of the physical glypyh, uint32_t version of mGlyphMetrics.mfSizeY.
                GlyphMetrics  mGlyphMetrics;    /// GlyphMetrics; includes glyph size, bearing, advance.
                const void*   mpData;           /// The physical glyph representation. The data format depends on the Font type and glyph format. For OutlineFont and BmpFont, this is a bitmap. For PolygonFont, this is a PolygonGlyph (or custom user representation).
                BitmapFormat  mBitmapFormat;    /// Glyph representation format.
                uint32_t      mnStride;         /// For bitmapped glyphs, this is the bitmap stride in bytes.
            };

            //  Backwards compatibility definition:
            struct GlyphBitmap : public GlyphRepresentation { };

        public:
            /// Font
            ///
            /// Constructor. The font constructor merely initializes member data to zero. 
            /// No significant operations are done. 
            ///
            Font(Allocator::ICoreAllocator* pCoreAllocator = NULL);
            Font(const Font&);
            Font& operator=(const Font&);

            /// ~Font
            ///
            EATEXT_VIRTUAL ~Font();


            /// SetAllocator
            ///
            /// Sets the memory allocator that this font uses. The memory allocator 
            /// should be set before the font is used. If and how any memory is allocated
            /// depends on the particular font implementation.
            ///
            EATEXT_VIRTUAL void SetAllocator(Allocator::ICoreAllocator* pCoreAllocator);


            /// AddRef
            ///
            /// Increments the reference count, returns the new reference count.
            ///
            /// Fonts are reference counted items. This allows for the efficient and smart sharing of 
            /// fonts between various subsystems. All AddRef calls must be eventually matched by a 
            /// Release call. Otherwise the system cannot know if the font is needed any longer.
            ///
            /// The font class guarantees that you can use it without reference counting, as long as 
            /// you consistently use reference counting. In other words, once you call AddRef on a 
            /// newly created font, you must use AddRef and Release on the font in the future. 
            ///
            EATEXT_VIRTUAL int AddRef();


            /// Release
            ///
            /// Matches an AddRef call.
            ///
            EATEXT_VIRTUAL int Release();


            /// GetUserData
            ///
            /// Retrieves user data set by SetUserData. See SetUserData for more information.
            ///
            EATEXT_VIRTUAL void* GetUserData() const;


            /// SetUserData
            ///
            /// Allows the user to associate custom data to an instance of this class.
            /// Note that if a font is shared by multiple users that this value is also shared
            /// between those users. Thus the information stored with SetUserData is limited to
            /// certain kinds of data.
            /// The FontServer does not use this field.
            ///
            EATEXT_VIRTUAL void SetUserData(void* pUserData);


            /// GetStream
            ///
            EATEXT_VIRTUAL IO::IStream* GetStream();


            /// Close
            ///
            /// Closes the font and frees any resources allocated by it after being opened.
            /// The font is returned to the same essential state that it was before being Opened.
            /// The font must be opened or the return value will be false.
            /// If the font is not closed before it is destroyed, it will be closed upon its final Release.
            ///
            EATEXT_VIRTUAL bool Close() = 0;


            /// SetOption
            ///
            EATEXT_VIRTUAL void SetOption(int32_t option, int32_t value);


            /// GetBitmapFormat
            ///
            /// Returns the BitmapFormat of the font.
            ///
            EATEXT_VIRTUAL BitmapFormat GetBitmapFormat() const = 0;


            /// GetFontType
            ///
            /// Returns one of enum FontType. 
            /// The font must be opened or the return value will be kFontTypeUnknown.
            /// The returned FontType will be the internal font type, despite that the 
            /// font data may be converted to other forms, as is often the case when
            /// outline fonts are converted from outlines to textures.
            ///
            EATEXT_VIRTUAL FontType GetFontType() = 0;


            /// GetFontDescription
            ///
            /// Fills in a FontDescription for the user.
            /// The font must be opened or the return value will be false.
            ///
            EATEXT_VIRTUAL bool GetFontDescription(FontDescription& fontDescription) = 0;


            /// GetFontMetrics
            ///
            /// Fills in a FontMetrics for the user.
            /// The font must be opened or the return value will be false.
            ///
            EATEXT_VIRTUAL bool GetFontMetrics(FontMetrics& fontMetrics) = 0;


            /// GetGlyphMetrics
            ///
            /// Fills in a GetGlyphMetrics for the user.
            /// The font must be opened or the return value will be false. 
            /// The font sizing must be set or the results are unpredicatble.
            /// Due to the way font hinting and antialiasing occurs, glyph metrics are 
            /// only known after a glyph has been rendered. As a result, the GetGlyphMetrics
            /// function will have to internally do an imaginary rendering of the glyph 
            /// if the glyph metrics of the given glyph are not already known. Glyph metrics
            /// will be cached upon first calculation, and the user may be able to supply
            /// a precalculated set of glyph metrics for a font in order to prevent the need
            /// for calculating them at runtime upon first use.
            ///
            /// This function does not take into account kerning that may be applied to 
            /// glyphs, as kerning requires contextual information that can only be known
            /// at the end of the layout process.
            ///
            EATEXT_VIRTUAL bool GetGlyphMetrics(GlyphId glyphId, GlyphMetrics& glyphMetrics) = 0;


            /// GetGlyphIds
            ///
            /// Function to map code points to glyph indexes.
            /// Similar to Windows ScriptGetCMap function.
            /// Unsupported glyphs are filled in with the replacement glyph if bUseReplacementGlyph is true, 
            /// otherwise kGlyphIdInvalid (if bWriteInvalidGlyphs is true). In the case of bUseReplacementGlyph == true,
            /// the return value of this function will always be equal to nCharArrayCount.
            /// If bWriteInvalidGlyphs is false then kGlyphIdInvalid is never written and the return value may be 
            /// less than nCharArrayCount. bWriteInvalidGlyphs defaults to false for backward compatibility
            /// with older versions of this library. bWriteInvalidGlyphs has meaning only if bUseReplacementGlyph == false.
            /// The nGlyphIdStride parameter allows the user to write to an array of GlyphDisplayEntry (or some other format)
            /// instead of just an array of GlyphId.
            /// Returns the number of glyphs written to pGlyphIdArray.
            /// The font must be opened or the return value will be 0.
            /// If pGlyphIdArray is NULL, the glyphs are not written to pGlyphIdArray.
            /// This function is also useful for telling if a code point is covered by a font.
            ///
            EATEXT_VIRTUAL uint32_t GetGlyphIds(const Char* pCharArray, uint32_t nCharArrayCount, GlyphId* pGlyphIdArray = NULL, 
                                                bool bUseReplacementGlyph = true, const uint32_t nGlyphIdStride = sizeof(GlyphId), bool bWriteInvalidGlyphs = false) = 0;

            EATEXT_VIRTUAL uint32_t GetGlyphIds(const char8_t* pCharArray, uint32_t nCharArrayCount, GlyphId* pGlyphIdArray = NULL, 
                                                bool bUseReplacementGlyph = true, const uint32_t nGlyphIdStride = sizeof(GlyphId), bool bWriteInvalidGlyphs = false) = 0;


            /// IsCharSupported
            ///
            /// Returns true if the font appears to support the given character (or script). 
            /// If you want to know if a given locale is supported, you should use the 
            /// GetScriptFromLocale function to convert your locale to a script usable by 
            /// this function.
            /// If you supply a script other than kScriptUnknown, the script is possibly  
            /// consulted instead of the char. If you can supply a script then this function  
            /// will be faster because a script lookup is significantly faster than a character
            /// lookup. A script value of kScriptCommon will always return true, however, 
            /// as there are many common characters that fall under kScriptCommon.
            /// A script of kScriptUnknown will always result in a full character support lookup.
            ///
            EATEXT_VIRTUAL bool IsCharSupported(Char c, Script script = kScriptUnknown) = 0;


            /// GetKerning
            ///
            /// Gives the user kerning information for a pair of glyph ids.
            /// Some glyph pairs in a font may have custom kerning assigned to them. This kerning 
            /// information can be used to adjust the advance distance of the first glyph to the second. 
            /// The return value is true if this function can return kerning information to the user.
            /// Otherwise the Kerning data will be zero.
            /// The direction argument specifes the glyph layout direction as per standard convention;
            /// even number indicate LTR directions and odd numbers indicate RTL direction.
            /// The font must be opened or the return value will be false.
            ///
            EATEXT_VIRTUAL bool GetKerning(GlyphId g1, GlyphId g2, Kerning& kerning, int direction, bool bHorizontalLayout = true) = 0;


            /// GetSize
            ///
            /// Returns the size of the font. The size is based on the transform.
            /// This function merely returns the FontDescription.mfSize member, which you 
            /// can also retrieve via GetFontDescription().
            ///
            EATEXT_VIRTUAL float GetSize() const = 0;


            /// SetTransform
            ///
            /// Specifies the transform with defines the size, rotation, and skew (slant) of the text. 
            /// These are linear transformations which use a matrix of the form:
            ///     [0,0]   [0,1]
            ///     [1,0]   [1,1]
            ///
            /// Note that we don't specify a translation component. This is because the translation  
            /// usually stays as zero and even when it doesn't stay at zero it is a value that 
            /// will change for every character, due to the need to place each character on a line
            /// with differening fractional pixel positioning.
            /// 
            /// Scaling by a scale s in the x and y dimensions is done with the following matrix:
            ///      sx   0
            ///       0  sy
            ///
            /// Rotation by an angle a in radians is done with the following matrix:
            ///      cos(a)  -sin(a)
            ///      sin(a)   cos(a)
            ///
            /// Skewing by an angle a in radians is done with the following matrix (for horizontal skewing):
            ///      1  sin(a)
            ///      0  1
            ///
            /// You would need to multiple scale, rotation, and skew matrices by each other to get 
            /// combination effects. See the SetTransform(size, angle, skew) function for a shortcut
            /// to achieving such a goal.
            ///
            EATEXT_VIRTUAL bool SetTransform(float f00, float f01, float f10, float f11) = 0;


            /// SetTransform
            ///
            /// This is a high-level function to set the size, angle, and skew of glyphs.
            /// Underneath, this function builds a 2x2 transform matrix and calls the base 
            /// SetTransform function.
            ///
            /// The input fSize refers to the desired size of the font in pixels. Due to the way modern typography
            /// works, the size of a font doesn't may any particular concrete metric. The size is simply something
            /// that happens to be close to the max bounding box of characters in the font. In practice you will
            /// need to experiment to see what size works best for your application.
            /// 
            EATEXT_VIRTUAL bool SetTransform(float fSize, float fAngle = 0.f, float fSkew = 0.f) = 0;


            /// RenderGlyphBitmap
            ///
            /// Renders a single glyph. Upon being done with the bitmap, the DoneGlyphBitmap function must be
            /// called. A second call to RenderGlyphBitmap before DoneGlyphBitmap is called will result in 
            /// an internal call to DoneGlyphBitmap for the first call. Thus there can be only one outstanding
            /// rendered glyph at a time.
            ///
            /// This function is used to convert source font data to bitmapped font data. In the world of 3D
            /// graphics and textured rasterization, the user of a glyph bitmap would usually want to copy the 
            /// bitmap to a free rectangular position on a 2D texture. The GlyphBitmap information will allow
            /// the user to do this appropriately. 
            ///
            /// The render flags specify parameters for how the glyph is to be drawn.
            /// 
            /// Example usage:
            ///    GlyphBitmap* pGlyphBitmap;
            ///    if(pFont->RenderGlyphBitmap(&pGlyphBitmap, 'a', kRFUnicodeId)){
            ///       // Do something with pGlyphBitmap.
            ///       pFont->DoneGlyphBitmap(pGlyphBitmap);
            ///    }
            ///
            EATEXT_VIRTUAL bool RenderGlyphBitmap(const GlyphBitmap** pGlyphBitmap, GlyphId g, 
                                        uint32_t renderFlags = kRFDefault, float fXFraction = 0, float fYFraction = 0) = 0;


            /// DoneGlyphBitmap
            ///
            EATEXT_VIRTUAL void DoneGlyphBitmap(const GlyphBitmap* pGlyphBitmap) = 0;


            /// GetOTF
            ///
            /// Returns the OpenTypeFont object for this Font. OpenType information is going to be 
            /// generally found only outline fonts that have complex scripts such as Thai or Arabic.
            /// This function returns NULL if the Font has no OpenType information. It is possible
            /// that the returned OTF may be empty or may be lacking information for the script 
            /// which you are shaping.
            ///
            EATEXT_VIRTUAL OTF* GetOTF() = 0;

        protected:
            friend class FontServer;

            void SetupSupportedScriptSet();

        protected:
            Allocator::ICoreAllocator* mpCoreAllocator;
            void*                      mpUserData;
            IO::IStream*               mpStream;
            GlyphId                    mReplacementGlyphId;
            SupportedScriptSet         mSupportedScriptSet;

            int mRefCount;

        }; // Font

    } // namespace Text

} // namespace EA


#ifdef _MSC_VER
    #pragma warning(pop)
#endif


#endif // EATEXT_EATEXTFONT_H





