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
// EATextFontServer.h
//
// By Paul Pedriana - 2004
//
///////////////////////////////////////////////////////////////////////////////


#ifndef EATEXT_EATEXTFONTSERVER_H
#define EATEXT_EATEXTFONTSERVER_H


#include <EABase/eabase.h>
#include <EAText/EAText.h>
#include <EAText/EATextFont.h>
#include <EAText/EATextEffects.h>
#include <EAText/EATextScript.h>
#include <EAText/EATextStyle.h>
#include <EAText/EATextUnicode.h>
#include <EAText/internal/EATextRefCount.h>
#include <EASTL/fixed_set.h>
#include <EASTL/fixed_list.h>
#include <EASTL/fixed_vector.h>
#include <EASTL/fixed_hash_map.h>
#include <EASTL/fixed_hash_set.h>
#include <EASTL/functional.h>
#include <EASTL/utility.h>
#include <EASTL/string.h>
#include <coreallocator/icoreallocator_interface.h>

#ifdef _MSC_VER
    #pragma warning(push)           // We have to be careful about disabling this warning. Sometimes the warning is meaningful; sometimes it isn't.
    #pragma warning(disable: 4251)  // class (some template) needs to have dll-interface to be used by clients.
#endif


namespace EA
{
    namespace Allocator
    {
        class ICoreAllocator;
    }

    namespace IO
    {
        class IStream;
    }

    namespace Text
    {
        class Font;
        class OutlineFont;
        class BmpFont;
        class PolygonFont;
        class GlyphCache;
        class FaceData;
 

        /// FontSelection
        ///
        /// Defines a selection of fonts. This is used by the layout engine to select
        /// fonts which are used for a layout. The fonts are in order from most to 
        /// least significant; thus the first fonts listed are of highest priority.
        /// 
        /// For very basic Western script layout you can get away with just a single
        /// font, but with multi-language text it will usually be the case that a 
        /// single font doesn't suffice and that you need to do the layout with
        /// a selection of fonts that match the required style but each of which 
        /// covers a different set of characters (usually one font per script).
        ///
        /// The FontServer will give you a FontSelection based on input criteria such
        /// as text style and locale. You want to use FontSelections instead of always
        /// going to the FontServer and doing lookups because the lookups are going 
        /// to be somewhat slower, whereas working with a FontSelection is fast because
        /// it is merely an array of pre-selected Fonts.
        ///
        /// A FontSelection AddRefs the Fonts that it contains.
        ///
        typedef EA::Text::AutoRefCount<Font> FontARC;
        typedef eastl::fixed_set<FontARC, 8> FontSelection;



        /// GetSystemFontDirectory
        ///
        /// For systems that support a standardized central font repository, this function
        /// yields the location of that repository. 
        /// The pFontDirectory parameter should hold enough space to store a standard path
        /// for the given operating sytem. If there isn't enough space to store the directory,
        /// or if pFontDirectory is NULL, no characters are copied to pFontDirectory.
        /// The returned directory string will be terminated by a platform-specific path
        /// separator.
        /// The return value is the Strlen of system font directory, regarless of the 
        /// value of the nFontDirectoryCapacity argument. If the system font directory 
        /// is unknown, the return value is zero and pFontDirectory is set to an empty string.
        /// If pFontDirectory is non-NULL, nFontDirectoryCapacity must be at least 1.
        ///
        EATEXT_API uint32_t GetSystemFontDirectory(FilePathChar* pFontDirectory, uint32_t nFontDirectoryCapacity);



        #if EATEXT_FONT_SERVER_ENABLED

        /// FontServer
        ///
        /// Provides a basic generic font server. A font server is an entity that maintains a set of 
        /// fonts which can be requested by the user based on various criteria. For example, a user
        /// might request a size 12 Helvetica Bold Italic font, and the font server will tell if such 
        /// a thing is present and if not it will attempt to find the best match. 
        ///
        ///
        class EATEXT_API FontServer
        {
        public:
            /// MatchOption 
            /// See kOptionRequireFontFamilyMatch and any other match options that may exist.
            enum MatchOption
            {
                kMatchOptionNone   = 0x00,     /// GetFont allows mismatches.
                kMatchOptionAssert = 0x01,     /// GetFont asserts in debug builds that the requested font was in fact found and a substitute was not used. However, a font will be returned nevertheless unless kMatchOptionFail is also specified.
                kMatchOptionFail   = 0x02      /// GetFont function fails and returns NULL when the user-specified font family doesn't match.
            };

            /// Option
            /// Note that a subclass may define additional options beyond options defined here.
            enum Option
            {
                kOptionNone                       = 0,    /// Non-option.

                // Font options (applied to Fonts as they are created). Values for all Font options are: [ -1 (unset), 0 (disabled), >0 ].
                kOptionOpenTypeFeatures           = 1,    /// OpenType feature table support. Default is enabled. If EATEXT_OPENTYPE_ENABLED == 0 then OpenType features are unilaterally disabled.
                kOptionDPI                        = 2,    /// The DPI the font works in. Default is the same as EATEXT_DPI.
                kOptionEnableHinting              = 3,    /// Enable hinting with hinted fonts. Default is enabled.
                kOptionUseAutoHinting             = 4,    /// Use auto-generated hinting, even if the font has intrinsic hinting. Default is disabled.
                kOptionLCD                        = 5,    /// Target LCD displays. Default is disabled.
                kOptionEnableDirectGlyphCacheUse  = 6,    /// Currently used by BmpFonts only. Default is disabled. If enabled then fonts directly store their images as individual glyph cache textures. Otherwise glyphs are copied as-needed from the images to the global (usually singleton) glyph cache texture. With systems like UTFWin you have to disable this option, as UTFWin doesn't support multiple glyph cache textures at a time, as of this writing (2/2011).
                kOptionSystemMemoryCopy           = 7,    /// Currently used by BmpFonts only. Default is enabled. If enabled then fonts keep a copy of their images in system memory and load it on startup.

                // FontServer options.
                kOptionSmartFallback              = 8,    /// Smart fallback means that if the user-specified font specification can't support a given char/script, other registered fonts are considered. Disabled (0) by default; > 0 if enabled.
                kOptionRequireFontFamilyMatch     = 9,    /// One of enum MatchOption. kMatchOptionNone by default. Values are ORd together, so a value of (kMatchOptionAssert | kMatchOptionFail) causes both an assertion and faiure. ther font style verification isn't applied, such as weight, style, pitch.

                kOptionCount                              /// Not an actual option, but rather the count of options.
            };

        public:
            FontServer(Allocator::ICoreAllocator* pCoreAllocator = NULL);
            FontServer(const FontServer&);
            FontServer& operator=(const FontServer&);

            virtual ~FontServer();

            /// SetAllocator
            /// Sets the allocator to use for any memory allocation.
            /// Memory is only allocated by this class during the creation of Fonts. No memory 
            /// is allocated by this class at runtime while serving Fonts, and no memory is 
            /// allocated if the user creates the Fonts himself instead of using the FontServer
            /// to create fonts. 
            /// This function must be called before this class is used and before Init is called.
            void SetAllocator(Allocator::ICoreAllocator* pCoreAllocator);

            /// Init
            /// 
            /// Initialize the font server for use. Should be matched by a call to Shutdown.
            ///
            virtual bool Init();


            /// SetOption
            ///
            virtual void SetOption(int32_t option, int32_t value);

            virtual int32_t GetOption(int32_t option) const;


            /// Shutdown
            ///
            /// Returns the FontServer to a state equivalent to that before Init.
            ///
            virtual bool Shutdown();


            /// SetDefaultGlyphCache
            /// 
            /// Used to set the GlyphCache associated with entities that use GlyphCaches.
            /// You should use this function if you will be using FontServer to create 
            /// BmpFont objects, as BmpFonts want to know what the system GlyphCache is 
            /// in order to store their textures.
            /// If this function is not called, the default GlyphCache is NULL.
            ///
            virtual void SetDefaultGlyphCache(GlyphCache* pGlyphCache);


            /// GetDefaultGlyphCache
            /// 
            /// Returns the GlyphCache set by SetDefaultGlyphCache, else NULL.
            ///
            virtual GlyphCache* GetDefaultGlyphCache();


            /// CreateNewFont
            /// 
            /// This is a factory function used to create font. It does not add the font
            /// to any data structures; it merely creates it.
            /// The fontType argument is usually one of enum FontType, such as kFontTypeOutline
            /// or kFontTypeBitmap. But for user-extended font types this may be a type that
            /// is outside this enumeration.
            /// It is intended that the user be able to override this function in a subclass
            /// of this class.
            /// The returned font is AddRefd for the caller.
            ///
            Font* CreateNewFont(int fontType);


            /// GetFont
            /// 
            /// Gives the user a font, with the choice being dependent on the fontDescription.
            /// Unless there are no fonts present at all, a valid font will always be returned.
            ///
            /// This function accepts a Script specifier so that it can do basic coverage tests 
            /// on the font selection. Note that if you know your locale but not your script, 
            /// you can use GetScriptFromLocale to convert the former to the latter.
            /// For a font to match, it must support the script that it will be used for.
            /// If the user requests a family/face called Mojo with Hangul script but 
            /// the Mojo font doesn't support Hangul, we simply can't use Mojo, as it wouldn't
            /// be able to draw anything but blank characters. The user can use an input script 
            /// kScriptUnknown to specify that script matching should be ignored.
            ///
            /// If pFontArray is non-NULL, then as many as nFontArrayCapacity Fonts are written
            /// to the array. Note that TextStyle allows for the specification of multiple
            /// font families to use for finding candidate fonts.
            ///
            /// If bManaged is true, then the returned Font is also retained by the FontServer for
            /// sharing with other users. If bManaged is false, then the Font is solely owned
            /// by the user and is not shared with other users, even if they request an identical font.
            ///
            /// The matching behavior of this function is controlled by the kOptionRequireFontFamilyMatch
            /// option. In particular this function can return NULL if you specify kMatchOptionFail and 
            /// the family name (e.g. "Arial") you request isn't present in the FontServer.
            ///
            /// There will be one AddRef for each font returned in pFontArray.
            /// The font(s) in pFontArray must be relinquished by the user with a call to Font::Release.
            /// The returned font is equal to the value of the first font in pFontArray.
            ///
            virtual Font* GetFont(const TextStyle* pTextStyle, Font* pFontArray[] = NULL, uint32_t nFontArrayCapacity = 0, 
                                   Char c = kCharInvalid, Script script = kScriptUnknown, bool bManaged = true);


            /// GetFont
            /// 
            /// This version of GetFont is the same as the other but it uses a FontSelection object
            /// instead of a raw array of Font pointers. The FontSelection manages the reference 
            /// counting of Fonts and properly handles other operations as well, such as copying 
            /// FontSelection objects.
            ///
            /// The FontSelection object manages refcounts automatically. Thus unlike the other
            /// GetFont function, the user need not manually Release the fonts in the returned FontSelection.
            /// The returned font is equal to the value of the first font added to the FontSelection.
            ///
            virtual Font* GetFont(const TextStyle* pTextStyle, FontSelection& fontSelection, uint32_t maxCount = 0xffffffff, 
                                  Char c = kCharInvalid, Script script = kScriptUnknown, bool bManaged = true);


            /// AddFont
            /// 
            /// Adds a font to the FontServer. The FontServer will AddRef the Font for its own
            /// purposes and eventually Release the Font when it is done with it. Recall that a
            /// Font is a particular instance of a face. A face refers to, for example, Arial,
            /// whereas a font refers to Arial 12 pt bold.
            /// Returns true if the Font could be added and an equivalent font wasn't already 
            /// previously added.
            /// If pFontDescription is non-NULL, the new or existing Font's FontDescription is written to it.
            /// If the font is invalid, pFontDescription is not written to.
            ///
            virtual bool AddFont(Font* pFont, FontDescription* pFontDescription = NULL);


            /// EnumerateFonts
            /// 
            /// Gives the user information about every usable font. 
            /// At most nCount descriptions will be written to pFontDescriptionArray.
            /// The return value is the total count of descriptions, regardless of the 
            /// size of input nCount. The input pFontDescriptionArray can be NULL.
            /// 
            /// Note that while the returned array is the concrete set of source font 
            /// descriptions, a user request for a font of a FontDescription that 
            /// doesn't match any known type may result in the synthesis of a new
            /// Font which fulfills the user's request. An example of such a font might
            /// be an oblique font which is based on a non-italic version but slants
            /// its characters during glyph rasterization.
            ///
            /// To consider: Make this function so it is optionally compiled.
            ///
            virtual uint32_t EnumerateFonts(FontDescription* pFontDescriptionArray, uint32_t nCount);


            /// ReleaseUnusedFonts
            /// 
            /// Releases (frees) any managed Fonts with a ref-count of one, with one 
            /// indicating that the FontServer is the only remaining user of the Font.
            ///
            virtual uint32_t FreeUnusedFonts();


            /// AddFace
            ///
            /// Adds a font typeface from a stream source or from a font image in memory.
            /// See Font::Open for documentation on these parameters.
            /// Returns the number of FontSources added from the face. FontSources are 
            /// not added if an equivalent FontSource is already present.
            /// If pFontDescription is non-NULL, the the new or existing Font's FontDescription is written to it.
            /// If the font is invalid, pFontDescription is not written to.
            ///
            virtual uint32_t AddFace(IO::IStream* pStream, FontType fontType, FontDescription* pFontDescription = NULL);


            /// AddFace
            ///
            /// Adds a font typeface from a standalone file, such as a .ttf (TrueType Font) file.
            /// Returns the number of FontSources added from the face. FontSources are 
            /// not added if an equivalent FontSource is already present.
            /// If pFontDescription is non-NULL, the the new or existing Font's FontDescription is written to it.
            /// If the font is invalid, pFontDescription is not written to.
            ///
            virtual uint32_t AddFace(const FilePathChar* pFacePath, FontType fontType = kFontTypeUnknown, FontDescription* pFontDescription = NULL);

            /// RemoveFace
            ///
            /// Releases any font faces that belong to the given family. Any Fonts associated with
            /// the faces are released. Since the Fonts are reference decremented and outright 
            /// deleted, existing users of Fonts can continue to use them.
            /// Returns the count of faces removed.
            ///
            virtual uint32_t RemoveFace(const Char* pFamilyName);


            /// AddEffect
            ///  
            /// Registers the given user-defined effect for future use. If the given effectId is 
            /// already registered then this overwrites the effect value with the new data.
            /// The instruction list is copied and thus the supplied pInstructionList does not need 
            /// to have a lifetime beyond a call to this function.
            /// 
            /// The effectId must be a value (typically a GUID) >= 256. This is because lower values
            /// are reserved for predefined effects. The instruction count can be no longer than 
            /// EffectsState::kInstructionListCapacity.
            ///
            /// The instruction list/size can be NULL/0 (usually for the purpose of reserving an id) and 
            /// replaced with a valid instruction list later by a subsequent call to AddEffect with the same id.
            ///
            void AddEffect(uint32_t effectId, const EIWord* pInstructionList, uint32_t instructionCount);


            /// RemoveEffect
            ///
            /// Removes an effect previously added via AddEffect.
            ///
            void RemoveEffect(uint32_t effectId);


            /// GetEffect
            ///
            /// Retrieves an effect previously registered with AddEffect. Returns false if the effect
            /// was not registered and thus is unknown. 
            ///
            bool GetEffect(uint32_t effectId, const EIWord*& pInstructionList, uint32_t& instructionCount) const;


            #if EATEXT_FAMILY_SUBSTITUTION_ENABLED
                /// AddSubstitution
                ///
                /// Allows for the substitution and aliasing of font family names. This is useful for 
                /// providing fallbacks for missing fonts and for allowing the migration of a system
                /// from an old system to a new system. 
                ///
                /// The input pFamily is the name of a family which may not be present. The input
                /// pFamilySubstitution is the nameof a family which is expected to be present.
                /// If pFamily is "a" and pFamilySubstitution is "b", then requests for family "a"
                /// will be converted to requests for family "b".
                ///
                /// Returns true if the substitution could be added. This function could fail if the 
                /// input family strings are longer than kMaxFamilyNameSize.
                ///
                virtual bool AddSubstitution(const Char* pFamily, const Char* pFamilySubstitution);
            #endif

        protected: // Data structures

            /// FontList
            /// List of Font objects. A Font is the most specialized version of a typeface.
            typedef eastl::fixed_list<Font*, 4, true> FontList;


            /// FaceSource
            /// (e.g. a .ttf file such as "courier bold")
            /// We could call this struct FaceFile instead, though it's not necessarily 
            /// a file which provides the binary data for the font implmentation.
            struct FaceSource
            {
                IO::IStream*        mpStream;               /// Font source stream. It is AddRefd once per FaceSource that we use.
                FontType            mFontType;              /// kFontTypeOutline, kFontTypeBmp, etc.
                FontDescription     mFontDescription;       /// For outline fonts, the size field will be zero, indicating that this source works for all sizes.
                uint8_t             mnFaceIndex;            /// Index of which source to use within the font source (usually zero).
                FontList            mFontList;              /// List of fonts which are from this source. Usually you will have one per size.
                FaceData*           mpFaceData;             /// This applies to OutlineFonts only.

                FaceSource();
                FaceSource(const FaceSource& fs);
               ~FaceSource();
                FaceSource& operator=(const FaceSource& fs);
            };

            /// FaceSourceList
            /// List of face sources. Normally you will have at most a few face sources for a Face (e.g normal, bold, italic, bold-italic).
            typedef eastl::fixed_list<FaceSource, 4> FaceSourceList;


            /// Face
            /// (e.g. "Courier", but not anything more specific than that.)
            /// To consider: Remove the mFamily member, as it is already represented in the FaceMap.
            struct Face
            {
                Char           mFamily[kFamilyNameCapacity];    /// Family is stored in lower case.
                FaceSourceList mFaceSourceList;                 /// List of FaceSources -- one per FontDescription (not necessarily one per .ttf file)

                Face();
            };

            /// FontServerString
            /// To do: Convert FontServerString to a fixed_string type or to use our custom allocator..
            typedef eastl::basic_string<Char> FontServerString;

            /// FaceMap
            /// Map of family to Face information.
            typedef eastl::fixed_hash_map<FontServerString, Face, 8, 9, true, eastl::string_hash<FontServerString> > FaceMap;


            /// FaceArray
            /// fixed-size array of Faces.
            typedef eastl::fixed_vector<Face*, kFamilyNameArrayCapacity, true> FaceArray;


            /// EffectData
            struct EffectData
            {
                uint32_t mEffectId; 
                EIWord   mEffectInstructions[kEffectInstructionListCapacity];
                uint32_t mEffectInstructionCount;
            };
            typedef eastl::list<EffectData, EA::Allocator::EASTLICoreAllocator> EffectDataList;

            // CharSet
            struct CharHash { size_t operator()(Char c) const { return static_cast<size_t>(c); } };

            typedef eastl::fixed_hash_set<Char, 41, 41, false, CharHash> CharSet;

            #if EATEXT_FAMILY_SUBSTITUTION_ENABLED
                /// FamilySubstitutionMap
                /// Maps family names to alternatives that are considered equivalent.
                typedef eastl::fixed_hash_map<FontServerString, FontServerString, 4, 5, true, eastl::string_hash<FontServerString> > FamilySubstitutionMap;
            #endif

        protected: // Functions
            virtual Font* CreateNewFont(FaceSource* pFaceSource, const TextStyle& ssCSS, bool bManaged);
            virtual int   GetFontDescriptionScore(const FontDescription& fontDescription, const TextStyle& ssCSS);
            virtual bool  AddFaceSource(FaceSource& faceSource, FontDescription* pFontDescription);

        protected: // Member data
            bool                       mbInitialized;
            int32_t                    mOptions[kOptionCount];
            Allocator::ICoreAllocator* mpCoreAllocator;
            TextStyle                  mTextStyleDefault;
            FaceMap                    mFaceMap;
            GlyphCache*                mpGlyphCacheDefault;
            FaceArray                  mFaceArray;                          // Used as a scratch buffer by GetFont.
            EffectDataList             mEffectDataList;
            int32_t                    mDPI;
            CharSet                    mIgoredFontCharSet;                  // Used to skip lookups of some characters in fonts.

            #if EATEXT_FAMILY_SUBSTITUTION_ENABLED
                FamilySubstitutionMap mFamilySubstitutionMap;
            #endif

        }; // class FontServer


        #endif // EATEXT_FONT_SERVER_ENABLED



    } // namespace Text

} // namespace EA


#ifdef _MSC_VER
    #pragma warning(pop)
#endif


#endif // EATEXT_EATEXTFONTSERVER_H





