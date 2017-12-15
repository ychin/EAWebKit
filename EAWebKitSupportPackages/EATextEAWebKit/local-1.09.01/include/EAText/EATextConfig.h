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
// EATextConfig.h
//
// By Paul Pedriana - 2004
//
// This file allows customization of the EAText library.
//
///////////////////////////////////////////////////////////////////////////////


#ifndef EATEXT_EATEXTCONFIG_H
#define EATEXT_EATEXTCONFIG_H


#include <EABase/eabase.h>


///////////////////////////////////////////////////////////////////////////////
// EATEXT_VERSION
//
// We more or less follow the conventional EA packaging approach to versioning 
// here. A primary distinction here is that minor versions are defined as two
// digit entities (e.g. .03") instead of minimal digit entities ".3"). The logic
// here is that the value is a counter and not a floating point fraction.
// Note that the major version doesn't have leading zeros.
//
// Example version strings:
//      "0.91.00"   // Major version 0, minor version 91, patch version 0. 
//      "1.00.00"   // Major version 1, minor and patch version 0.
//      "3.10.02"   // Major version 3, minor version 10, patch version 02.
//     "12.03.01"   // Major version 12, minor version 03, patch version 
//
// Example usage:
//     printf("EATEXT version: %s", EATEXT_VERSION);
//     printf("EATEXT version: %d.%d.%d", EATEXT_VERSION_N / 10000 % 100, EATEXT_VERSION_N / 100 % 100, EATEXT_VERSION_N % 100);
//
///////////////////////////////////////////////////////////////////////////////

#ifndef EATEXT_VERSION
    #define EATEXT_VERSION   "1.09.01"
    #define EATEXT_VERSION_N  10901
#endif


///////////////////////////////////////////////////////////////////////////////
// EA_PLATFORM_MICROSOFT
//
// Defined as 1 or undefined.
// Implements support for the definition of EA_PLATFORM_MICROSOFT for the case
// of using EABase versions prior to the addition of its EA_PLATFORM_MICROSOFT support.
//


///////////////////////////////////////////////////////////////////////////////
// EATEXT_DLL
//
// Defined as 0 or 1. The default is dependent on the definition of EA_DLL.
// If EA_DLL is defined, then EATEXT_DLL is 1, else EATEXT_DLL is 0.
// EA_DLL is a define that controls DLL builds within the eaconfig build system. 
// EATEXT_DLL controls whether EAText is built and used as a DLL. 
// Normally you wouldn't do such a thing, but there are use cases for such
// a thing, particularly in the case of embedding C++ into C# applications.
//
#ifndef EATEXT_DLL
    #if defined(EA_DLL)
        #define EATEXT_DLL 1
    #else
        #define EATEXT_DLL 0
    #endif
#endif


///////////////////////////////////////////////////////////////////////////////
// EATEXT_API
//
// This is used to label functions as DLL exports under Microsoft platforms.
// If EA_DLL is defined, then the user is building EAText as a DLL and EAText's
// non-templated functions will be exported. EAText template functions are not
// labeled as EATEXT_API (and are thus not exported in a DLL build). This is 
// because it's not possible (or at least unsafe) to implement inline templated 
// functions in a DLL.
//
// Example usage of EATEXT_API:
//    EATEXT_API int someVariable = 10;         // Export someVariable in a DLL build.
//
//    struct EATEXT_API SomeClass{              // Export SomeClass and its member functions in a DLL build.
//        EATEXT_LOCAL void PrivateMethod();    // Not exported.
//    };
//
//    EATEXT_API void SomeFunction();           // Export SomeFunction in a DLL build.
//
// For GCC, see http://gcc.gnu.org/wiki/Visibility
//
#ifndef EATEXT_API // If the build file hasn't already defined this to be dllexport...
    #if EATEXT_DLL 
        #if defined(_MSC_VER)
            #define EATEXT_API      __declspec(dllimport)
            #define EATEXT_LOCAL
        #elif defined(__CYGWIN__)
            #define EATEXT_API      __attribute__((dllimport))
            #define EATEXT_LOCAL
        #elif (defined(__GNUC__) && (__GNUC__ >= 4))
            #define EATEXT_API      __attribute__ ((visibility("default")))
            #define EATEXT_LOCAL    __attribute__ ((visibility("hidden")))
        #else
            #define EATEXT_API
            #define EATEXT_LOCAL
        #endif
    #else
        #define EATEXT_API
        #define EATEXT_LOCAL
    #endif
#endif


///////////////////////////////////////////////////////////////////////////////
// EA_XBDM_ENABLED
//
// Defined as 0 or 1, with 1 being the default for debug builds.
// This controls whether xbdm library usage is enabled on XBox 360. This library
// allows for runtime debug functionality. But shipping applications are not
// allowed to use xbdm. 
//
#if !defined(EA_XBDM_ENABLED)
    #if defined(EA_DEBUG)
        #define EA_XBDM_ENABLED 1
    #else
        #define EA_XBDM_ENABLED 0
    #endif
#endif


///////////////////////////////////////////////////////////////////////////////
// EATEXT_DEBUG
//
// Defined as 0, 1, 2, or 3. Default is to be 1 under EA_DEBUG, else 0. 
// Higher levels mean more debugging. When defined as non-zero, EAText enables debugging 
// functionality. This functionality includes things like internal diagnostics, 
// input validation, and named memory allocations. When defined as <= zero, all debug 
// functionality is #defined away.
//
#ifndef EATEXT_DEBUG
    #if defined(EA_DEBUG) || defined(_DEBUG)
        #define EATEXT_DEBUG 1
    #else
        #define EATEXT_DEBUG 0
    #endif
#endif



///////////////////////////////////////////////////////////////////////////////
// EATEXT_ALLOC_PREFIX
//
// Defined as a string literal. Defaults to this package's name.
// Can be overridden by the user by predefining it or by editing this file.
// This define is used as the default name used by this package for naming
// memory allocations and memory allocators.
//
// All allocations names follow the same naming pattern:
//     <package>/<module>[/<specific usage>]
// 
// Example usage:
//     void* p = pCoreAllocator->Alloc(37, EATEXT_ALLOC_PREFIX, 0);
//
// Example usage:
//     gMessageServer.GetMessageQueue().get_allocator().set_name(EATEXT_ALLOC_PREFIX "MessageSystem/Queue");
//
#ifndef EATEXT_ALLOC_PREFIX
    #define EATEXT_ALLOC_PREFIX "EAText/"
#endif



///////////////////////////////////////////////////////////////////////////////
// UTF_USE_EATRACE    Defined as 0 or 1. Default is 1 but may be overridden by your project file.
// UTF_USE_EAASSERT   Defined as 0 or 1. Default is 0.
//
// Defines whether this package uses the EATrace or EAAssert package to do assertions.
// EAAssert is a basic lightweight package for portable asserting.
// EATrace is a larger "industrial strength" package for assert/trace/log.
// If both UTF_USE_EATRACE and UTF_USE_EAASSERT are defined as 1, then UTF_USE_EATRACE
// overrides UTF_USE_EAASSERT.
//
#if defined(UTF_USE_EATRACE) && UTF_USE_EATRACE
    #undef  UTF_USE_EAASSERT
    #define UTF_USE_EAASSERT 0
#elif !defined(UTF_USE_EAASSERT)
    #define UTF_USE_EAASSERT 0
#endif

#undef UTF_USE_EATRACE
#if defined(UTF_USE_EAASSERT) && UTF_USE_EAASSERT
    #define UTF_USE_EATRACE 0
#else
    #define UTF_USE_EATRACE 1
#endif

#if !defined(EA_ASSERT_HEADER)
    #if defined(UTF_USE_EAASSERT) && UTF_USE_EAASSERT
        #define EA_ASSERT_HEADER <EAAssert/eaassert.h>
    #else
        #define EA_ASSERT_HEADER <EATrace/EATrace.h>
    #endif
#endif


///////////////////////////////////////////////////////////////////////////////
// EATEXT_CHAR_TYPE
//
// Defines the data type used to store a character (a.k.a. code point). 
// Note that as of this writing the char type must not be a multibyte encoded
// type. Multibyte encodings include UTF8 Unicode and DBCS/MBCS. The reason for
// this limitation is multibyte encodings bring complications to text processing
// and in general should be avoided when possible. 
//
#ifndef EATEXT_CHAR_TYPE
    #if EA_CHAR16_NATIVE
        #define EATEXT_CHAR_TYPE char16_t
        #define EATEXT_CHAR_TYPE_SIZE 2
    #else
        #define EATEXT_CHAR_TYPE wchar_t
        #define EATEXT_CHAR_TYPE_SIZE EA_WCHAR_SIZE // We can't use 'sizeof(Char)' because we need to be able to use
                                                    // EATEXT_CHAR_TYPE in preprocessor statements and thus they can't
                                                    // have C code in them. It must be an integral constant.
    #endif

    #if defined(EA_CHAR16)
        #define EATEXT_CHAR(x) EA_CHAR16(x)
    #else
        #define EATEXT_CHAR(x) EA_CHAR32(x)
    #endif
#endif



///////////////////////////////////////////////////////////////////////////////
// EATEXT_FILE_PATH_CHAR_TYPE
//
// Defines the data type used to represent file paths. 
// It should be a wide character type or represent UTF8 file path names.
//
#ifndef EATEXT_FILE_PATH_CHAR_TYPE
    #if EA_CHAR16_NATIVE
        #define EATEXT_FILE_PATH_CHAR_TYPE char16_t
        #define EATEXT_FILE_PATH_CHAR_TYPE_SIZE 2
    #else
        #define EATEXT_FILE_PATH_CHAR_TYPE wchar_t
        #define EATEXT_FILE_PATH_CHAR_TYPE_SIZE EA_WCHAR_SIZE
    #endif

    #if defined(EA_CHAR16)
        #define EATEXT_FILE_PATH_CHAR(x) EA_CHAR16(x)
    #else
        #define EATEXT_FILE_PATH_CHAR(x) EA_CHAR32(x)
    #endif
#endif



///////////////////////////////////////////////////////////////////////////////
// EATEXT_GLYPH_TYPE
//
// Defines the data type used to store a GlyphId. 
// This type doesn't need to match the EATEXT_CHAR_TYPE and is not related to
// the system wchar_t definition. GlyphIds are numerical indexes of glyphs 
// within fonts. It normally should be a 16 bit type, as space conservation is
// much more important than than the ability to support gigantic fonts which
// are nearly nonexistant anyway.
//
#ifndef EATEXT_GLYPH_TYPE
    #define EATEXT_GLYPH_TYPE      uint16_t // Usually a 16 bit integer type.
    #define EATEXT_GLYPH_TYPE_SIZE 2
#endif



///////////////////////////////////////////////////////////////////////////////
// EA_TEXT_REPLACEMENT_CHAR
//
// Defines the char to use when a glyph for the existing char cannot be found.
//
#ifndef EA_TEXT_REPLACEMENT_CHAR
    #define EA_TEXT_REPLACEMENT_CHAR '_'
#endif



///////////////////////////////////////////////////////////////////////////////
// EATEXT_USE_FREETYPE
//
// Defined as 0 or 1.
// 1 means use FreeType.
//
#ifndef EATEXT_USE_FREETYPE
    #define EATEXT_USE_FREETYPE 1
#endif

///////////////////////////////////////////////////////////////////////////////
// EATEXT_MIRROR_CHAR_SUPPORT 
//
// Defined as 0, 1, 2, or 3. 
// Specifies the level of built-in mirror character support. A value of <= 0 means that 
// there is no built-in mirror character support or the user either must supply a 
// mirror character table manually. Mirrored characters are generally only of use in 
// displaying right-to-left scripts.
//
#ifndef EATEXT_MIRROR_CHAR_SUPPORT
    #define EATEXT_MIRROR_CHAR_SUPPORT 1
#endif



///////////////////////////////////////////////////////////////////////////////
// EATEXT_KERNING_SUPPORT 
//
// Defined as 0, 1, 2, or 3. 
// Specifies what level of custom kerning EAText supports. Higher levels result in
// more precise kerning are slower at text layout. A value of <= 0 means that there 
// is no kerning support and that all glyphs are assumed to have no custom kerning.
//
// A value of 1 causes custom kerning to be used, but only for the most common Latin
// character combinations, such as "AV" and "TA". This is fast because it obviates
// the need to do kerning lookups at runtime. Values higher than 1 cause kerning to 
// be done fully during layout. 
//
#ifndef EATEXT_KERNING_SUPPORT
    #define EATEXT_KERNING_SUPPORT 1
#endif



///////////////////////////////////////////////////////////////////////////////
// EATEXT_LIGATURE_SUPPORT 
//
// Defined as 0, 1, or 2. 
// Specifies what level of custom ligature substitution EAText supports. Higher levels 
// result in deeper attempts at implementing ligatures. A value of <= 0 means that there 
// is no ligature support. However, note that some complex scripts such as Devanagari
// and Arabic require a certain amount of ligature-like substitution and are not affected
// by this setting. 
//
// A value of 1 causes custom ligatures to be used only when there are specific 
// Unicode-assigned ligature glyphs available in the given font (e.g. "tt").
// This level of support has little overhead in terms of speed and memory usage.
// A value of 2 causes the shaping engine to look into the font's custom ligature
// tables (OpenType feature) for ligature opportunities. This level of support 
// has some speed cost and memory cost, as an additional table per font must be 
// loaded and consulted.
//
#ifndef EATEXT_LIGATURE_SUPPORT
    #define EATEXT_LIGATURE_SUPPORT 0
#endif



///////////////////////////////////////////////////////////////////////////////
// EATEXT_SCRIPT_PROPERTY_SUPPORT
//
// Defined as 0, 1, 2, or 3.
// Defines the level of script property support as provided by the GetScriptProperties fucntion.
//
#ifndef EATEXT_SCRIPT_PROPERTY_SUPPORT
    #define EATEXT_SCRIPT_PROPERTY_SUPPORT 1
#endif



///////////////////////////////////////////////////////////////////////////////
// EATEXT_SCRIPT_LOCALE_SUPPORT 
//
// Defined as 0, 1, 2, or 3. 
// Specifies the extent of script support provided by the GetScriptFromLocale function.
//
#ifndef EATEXT_SCRIPT_LOCALE_SUPPORT
    #define EATEXT_SCRIPT_LOCALE_SUPPORT 1
#endif



///////////////////////////////////////////////////////////////////////////////
// EATEXT_SCRIPT_CHAR_SUPPORT
//
// Defined as 0, 1, 2, or 3. 
// Specifies the extent of script support provided by the GetScriptFromChar function.
//
#ifndef EATEXT_SCRIPT_CHAR_SUPPORT
    #define EATEXT_SCRIPT_CHAR_SUPPORT 1
#endif



///////////////////////////////////////////////////////////////////////////////
// EATEXT_LOCALE_SCRIPT_SUPPORT
//
// Defined as 0, 1, 2, or 3. 
// Specifies the extent of script support provided by the LocaleUsesScript function.
//
#ifndef EATEXT_LOCALE_SCRIPT_SUPPORT
    #define EATEXT_LOCALE_SCRIPT_SUPPORT 1
#endif



///////////////////////////////////////////////////////////////////////////////
// EATEXT_BLOCK_INFO_SUPPORT
//
// Defined as 0, 1, 2, or 3.
// Specifies the extent of block support provided by the GetBlockFromChar function.
//
#ifndef EATEXT_BLOCK_INFO_SUPPORT
    #define EATEXT_BLOCK_INFO_SUPPORT 2
#endif



///////////////////////////////////////////////////////////////////////////////
// EA_TEXT_CACHE_TEXTURE_SLOT_COUNT
//
// Defined as a value >= 1.
// Specifies the max number of textures a cache can have. Usually you want this
// value to be something between 1 and 8, depending on the size of the textures
// you use and the number of glyphs you want to cache at a time.
//
#ifndef EA_TEXT_CACHE_TEXTURE_SLOT_COUNT
    #define EA_TEXT_CACHE_TEXTURE_SLOT_COUNT 4
#endif



///////////////////////////////////////////////////////////////////////////////
// EATEXT_FAMILY_NAME_ARRAY_CAPACITY
//
// Defines as an integer >= 1. Default is the value assigned below.
// Specifies the number of family names a FontStyle can hold.
//
#ifndef EATEXT_FAMILY_NAME_ARRAY_CAPACITY
        #define EATEXT_FAMILY_NAME_ARRAY_CAPACITY 8
#endif



///////////////////////////////////////////////////////////////////////////////
// EA_TEXT_CACHE_TEXTURE_SIZE
//
// Defined as a power of two >= kTextureMinSize.
// Specifies the default size for cached glyph textures. Usually you want this
// value to be one of 128, 256, 512, or 1024.
//
#ifndef EA_TEXT_CACHE_TEXTURE_SIZE
    #define EA_TEXT_CACHE_TEXTURE_SIZE 512
#endif



///////////////////////////////////////////////////////////////////////////////
// EA_TEXT_CACHE_TEXTURE_COLUMN_COUNT_MAX
//
// Defined as a value >= 8.
// Specifies how many columns a glyph cache texture has. See the cache documentation
// to better understand what this is about.
//
#ifndef EA_TEXT_CACHE_TEXTURE_COLUMN_COUNT_MAX
    #define EA_TEXT_CACHE_TEXTURE_COLUMN_COUNT_MAX 32
#endif



///////////////////////////////////////////////////////////////////////////////
// EATEXT_DPI
//
// Defined as a value >= 1.
// Font sizes are traditionally specified in points, which are defined to be 
// roughly 1/72 of an inch. But computer screens are made up of pixels and 
// all drawing is done in terms of pixels. The system software doesn't always
// know how to scale text so that it has the same physical height on the screen
// across monitors. So we define a value which says how many dots per inch 
// the user's screen is assumed to have. Back in the early 1990s this value 
// was often 72 dpi, but most modern high resolution monitors use higher values.
// What's important here is that we pick a value and simply stick with it.
// Windows defines font points relative to a standardized 96 DPI system. 
// Thus if we use 96 here, then our 12 point fonts will be the same pixel height as Windows.
//
#ifndef EATEXT_DPI
    #define EATEXT_DPI 96
#endif



///////////////////////////////////////////////////////////////////////////////
// EATEXT_VIRTUAL_ENABLED
//
// Defined as 0 or 1
// However, some interfaces have no choice but to be virtual, and so 
// EATEXT_VIRTUAL_ENABLED has no effect on them.
//
#ifndef EATEXT_VIRTUAL_ENABLED
    #define EATEXT_VIRTUAL_ENABLED 1
#endif

// Do not modify this:
#if EATEXT_VIRTUAL_ENABLED
    #define EATEXT_VIRTUAL virtual
#else
    #define EATEXT_VIRTUAL
#endif


///////////////////////////////////////////////////////////////////////////////
// EATEXT_LAYOUT_ENABLED
//
// Defined as 0 or 1.
// In the long run we want to define it as 0 and eventually delete the Layout code.
//
#ifndef EATEXT_LAYOUT_ENABLED
    #define EATEXT_LAYOUT_ENABLED 1  // For now.
#endif


///////////////////////////////////////////////////////////////////////////////
// EATEXT_DLL_ENABLED
//
// Defined as 0 or 1.
// If 1, then classes and functions are exported and imported appropriately on 
// platforms which support the concept of DLLs, such as Microsoft-derived platforms.
//
#ifndef EATEXT_DLL_ENABLED
    #define EATEXT_DLL_ENABLED 1
#endif



///////////////////////////////////////////////////////////////////////////////
// EATEXT_VERTICAL_ENABLED
//
// Defined as 0 or 1.
// Enables vertical text orientation. Enabling this adds some extra code and increases
// the size of some data structures by a small amount. Vertical text is useful for 
// classic East Asian script layout and display.
//
#ifndef EATEXT_VERTICAL_ENABLED
    #define EATEXT_VERTICAL_ENABLED 0
#endif



///////////////////////////////////////////////////////////////////////////////
// EATEXT_EFFECTS_ENABLED
//
// Defined as 0 or 1.
// Enables the use of dynamically generated text effects such as outlining and
// shadowing. 
//
#ifndef EATEXT_EFFECTS_ENABLED
    #define EATEXT_EFFECTS_ENABLED 1
#endif



///////////////////////////////////////////////////////////////////////////////
// EATEXT_BITMAP_GLYPHS_ENABLED
//
// Defined as 0 or 1.
// Note that if this setting is disabled then users can still use bitmapped
// fonts by simply creating a subclass of GlyphCache which knows how to read
// font texture formats.
//
#ifndef EATEXT_BITMAP_GLYPHS_ENABLED
    #define EATEXT_BITMAP_GLYPHS_ENABLED 1
#endif



///////////////////////////////////////////////////////////////////////////////
// EATEXT_POLYGONAL_GLYPHS_ENABLED
//
// Defined as 0 or 1.
// Enables the handling of polygonal glyphs. Polygonal glyphs are glyphs defined by a 
// set of polygons instead of a set of bitmaps or textures. They have the advantage of
// allowing large glyphs (e.g. 100+ pixels high) without using large amounts of texture
// space to achieve satisfactorily smooth edges.
//
#ifndef EATEXT_POLYGONAL_GLYPHS_ENABLED
    #define EATEXT_POLYGONAL_GLYPHS_ENABLED 1
#endif



///////////////////////////////////////////////////////////////////////////////
// EATEXT_GLYPH_PIXEL_FIXING_ENABLED
//
// Defined as 0 or 1. Default is 1 on PC plaform, 0 otherwise.
// However, occasionally the glyphs can have tiny artifacts
// whereby a pixel is colored when it would look better if it wasn't. 
// For tiny fonts these pixels can make the font look bad. This config
// option enables the dynamic correction of many of these glyphs at the 
// cost of some memory usage.
//
#ifndef EATEXT_GLYPH_PIXEL_FIXING_ENABLED
        #define EATEXT_GLYPH_PIXEL_FIXING_ENABLED 1
#endif



///////////////////////////////////////////////////////////////////////////////
// EATEXT_WEAK_HINTING_DETECTION_ENABLED
//
// Defined as 0 or 1. 
// Determines whether EAText automatically does runtime tests to tell if 
// font hinting is poor and would result in worse rendering than if hinting
// wasn't used.
//
#ifndef EATEXT_WEAK_HINTING_DETECTION_ENABLED
    #define EATEXT_WEAK_HINTING_DETECTION_ENABLED 0
#endif



///////////////////////////////////////////////////////////////////////////////
// EATEXT_DIRECTX_VERSION
//
// Defined as 0, 9 or 10 or possibly some later version. Default is 9.
// Defines which version of DirectX to target in DirectX for Windows.
//
#ifndef EATEXT_DIRECTX_VERSION
        #define EATEXT_DIRECTX_VERSION 9
#endif



///////////////////////////////////////////////////////////////////////////////
// EATEXT_FONT_SERVER_ENABLED
//
// Defined as 0 or 1.
// Enables the availability of the FontServer. The FontServer is useful for environments
// whereby the usage of text, especially styled text, is not fully known at runtime or not
// completely known. Without the font server, font usage needs to be hard-wired into the 
// code that uses fonts. Such hard-wiring is normally not a good thing, regardless of the 
// platform and usage, as changing fonts requires modifying source code and hoping you 
// didn't miss anything. A font server, on the other hand, allows you to identify fonts
// via abstract names or GUIDs.
//
#ifndef EATEXT_FONT_SERVER_ENABLED
    #define EATEXT_FONT_SERVER_ENABLED 1
#endif



///////////////////////////////////////////////////////////////////////////////
// EATEXT_STYLE_MANAGER_ENABLED
//
// Defined as 0 or 1.
// Enables the availability of the StyleManager. The StyleManager is a C++ class and a 
// couple associated utility functions to implement CSS text/font styles. It is useful 
// for implementing application-wide or system-wide style specification. The advantages
// are similar to the advantages gained by writing documents with styles instead of 
// hard-coding font usage in the source code.
//
#ifndef EATEXT_STYLE_MANAGER_ENABLED
    #define EATEXT_STYLE_MANAGER_ENABLED 1
#endif



///////////////////////////////////////////////////////////////////////////////
// EATEXT_FAMILY_SUBSTITUTION_ENABLED
//
// Defined as 0 or 1. Default depends on the platform.
// Enables font substitution. This is useful for applications that are using system 
// fonts instead of a fixed distributed set of fonts or for applications that don't 
// know what kind of font faces it will be working with at compile time.
//
#ifndef EATEXT_FAMILY_SUBSTITUTION_ENABLED
        #define EATEXT_FAMILY_SUBSTITUTION_ENABLED 1
#endif



///////////////////////////////////////////////////////////////////////////////
// EATEXT_PS3_RGBA_COMPAT
//
// Defined as 0 or 1. Default is 0.
// Enables interpreting ARGB formats as RGBA when running on PS3. 
// Currently we define kTextureFormatARGB and kTextureFormatRGBA, but previously
// there was only kTextureFormat32Bpp, which referred to whatever the native 
// platform format was (ARGB or RGBA). However, it became evident that we 
// needed to make explicit kTextureFormatARGB and kTextureFormatRGBA types.
// This is fine but there is existing code which depends on the default format
// ARGB as being instead RGBA. This #define enables that backward compatibilty.
//
#ifndef EATEXT_PS3_RGBA_COMPAT
        #define EATEXT_PS3_RGBA_COMPAT 0
#endif



///////////////////////////////////////////////////////////////////////////////
// EATEXT_XXX_SCRIPT_SUPPORTED
//
// Defined as 0 or 1; can be not defined.
// Allows the user to enable support for individual scripts on a case by case basis.
// Some complex scripts require more code and/or more table space.
//
//    EATEXT_ALL_SCRIPTS_SUPPORTED
//    EATEXT_ARABIC_SCRIPT_SUPPORTED
//    EATEXT_HEBREW_SCRIPT_SUPPORTED
//    EATEXT_GREEK_SCRIPT_SUPPORTED
//    EATEXT_EAST_ASIAN_SCRIPT_SUPPORTED
//    EATEXT_THAI_SCRIPT_SUPPORTED
//    ...
//    See enum Script from EATextScript.h for more scripts
//
#if !defined(EATEXT_ALL_SCRIPTS_SUPPORTED) || EATEXT_ALL_SCRIPTS_SUPPORTED
    #undef  EATEXT_ALL_SCRIPTS_SUPPORTED
    #define EATEXT_ALL_SCRIPTS_SUPPORTED        1

    #define EATEXT_ARABIC_SCRIPT_SUPPORTED      1   // Arabic, Persian, Urdu
    #define EATEXT_CYRILLIC_SCRIPT_SUPPORTED    1   // Russian
    #define EATEXT_DEVANAGARI_SCRIPT_SUPPORTED  1   // Hindi
    #define EATEXT_GREEK_SCRIPT_SUPPORTED       1   // Greek
    #define EATEXT_HAN_SCRIPT_SUPPORTED         1   // Chinese and Japanese symbols (Kanji)
    #define EATEXT_HANGUL_SCRIPT_SUPPORTED      1   // Korean
    #define EATEXT_HEBREW_SCRIPT_SUPPORTED      1   // Hebrew
    #define EATEXT_HIRAGANA_SCRIPT_SUPPORTED    1   // Japanese syllabary
    #define EATEXT_KATAKANA_SCRIPT_SUPPORTED    1   // Japanese
    #define EATEXT_LAO_SCRIPT_SUPPORTED         1   // Lao (this is a rather uncommon script, but you practically get it for free with Thai).
    #define EATEXT_LATIN_SCRIPT_SUPPORTED       1   // English, Spanish, French, German, etc.
    #define EATEXT_THAI_SCRIPT_SUPPORTED        1   // Thai
#endif



///////////////////////////////////////////////////////////////////////////////
// EATEXT_BIDI_SUPPORTED
//
// Defined as 0 or 1. Its value depends on the EATEXT_XXX_SCRIPT_SUPPORTED defines.
// This define should not be set by the user; it is calculated from other defines.
//
#if EATEXT_ARABIC_SCRIPT_SUPPORTED || EATEXT_HEBREW_SCRIPT_SUPPORTED
    #define EATEXT_BIDI_SUPPORTED 1
#else
    #define EATEXT_BIDI_SUPPORTED 0
#endif

#if EATEXT_BIDI_SUPPORTED
    #if !EATEXT_MIRROR_CHAR_SUPPORT
        #undef  EATEXT_MIRROR_CHAR_SUPPORT
        #define EATEXT_MIRROR_CHAR_SUPPORT 1
    #endif
#endif



///////////////////////////////////////////////////////////////////////////////
// EATEXT_OPENTYPE_ENABLED
//
// Defined as 0 or 1. Its default value depends on the EATEXT_XXX_SCRIPT_SUPPORTED defines.
// This define should not be set by the user; it is calculated from other defines.
// OpenType is really needed only for Arabic and Hindi (Devenagari). OpenType is 
// also useful for Thai, as it allows Thai decoration glyphs to be positioned 
// precisely. However, EAText has basic Thai decoration positioning functionality
// in the absence of OpenType and which is good enough in practice for most uses.
//
#if !defined(EATEXT_OPENTYPE_ENABLED)
    #if EATEXT_ARABIC_SCRIPT_SUPPORTED || EATEXT_DEVANAGARI_SCRIPT_SUPPORTED
        #define EATEXT_OPENTYPE_ENABLED 1
    #else
        #define EATEXT_OPENTYPE_ENABLED 0 // We don't need OpenType for any other scripts.
    #endif
#endif



///////////////////////////////////////////////////////////////////////////////
// EATEXT_EMBEDDED_BACKUP_FONT_ENABLED
//
// Defined as 0 or 1. Default is same as !EA_FINAL.
// Enables the embedding of a minimal TrueType font into the source code into the
// text library. This allows users to do prototyping on console platforms with
// a minimum of effort in a debug environment.
//
#ifndef EATEXT_EMBEDDED_BACKUP_FONT_ENABLED
    #ifdef EA_FINAL
        #define EATEXT_EMBEDDED_BACKUP_FONT_ENABLED 0
    #else
        #define EATEXT_EMBEDDED_BACKUP_FONT_ENABLED 0
    #endif
#endif


///////////////////////////////////////////////////////////////////////////////
// EATEXT_SHARED_FF_SFNT_ENABLED
//
// Defined as 0 or 1. Default is 0. 
//
#ifndef EATEXT_SHARED_FF_SFNT_ENABLED
    #define EATEXT_SHARED_FF_SFNT_ENABLED 0
#endif


///////////////////////////////////////////////////////////////////////////////
// EA_UNUSED
// 
// Makes compiler warnings about unused variables go away.
//
// Example usage:
//    void Function(int x)
//    {
//        int y;
//        EA_UNUSED(x);
//        EA_UNUSED(y);
//    }
//
#ifndef EA_UNUSED 
    // The EDG solution below is pretty weak and needs to be augmented or replaced.
    // It can't handle the C language, is limited to places where template declarations
    // can be used, and requires the type x to be usable as a functions reference argument. 
    #if defined(__cplusplus) && defined(__EDG__) 
        template <typename T>
        inline void EABaseUnused(T const volatile & x) { (void)x; }
        #define EA_UNUSED(x) EABaseUnused(x)
    #else
        #define EA_UNUSED(x) (void)x
    #endif
#endif





#endif // Header include guard













