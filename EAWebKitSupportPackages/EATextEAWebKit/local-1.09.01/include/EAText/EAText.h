/*
Copyright (C) 2004,2009, 2010, 2012 Electronic Arts, Inc.  All rights reserved.

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
// EAText.h
// By Paul Pedriana - 2004
//
// This file defines the primary interfaces for the EA::Text namespace.
//
///////////////////////////////////////////////////////////////////////////////


///////////////////////////////////////////////////////////////////////////////
// Design Philosophy
//
// The design for this system is influenced primarily by two things:
//    1 The set of requirements that we have identified.
//        * These requirements are identified in the documentation for this system.
//    2 Existing standards and wisdom related to text processing and display.
//        a The Unicode Standard -- http://www.unicode.org
//        b HTML (Hypertext Markup Language) -- http://www.w3.org/MarkUp/
//        c CSS (Cascading Style Sheets) -- http://www.w3.org/Style/CSS
//        f OpenGL and DirectX graphics
//        d Uniscribe -- http://www.microsoft.com/typography/developers/uniscribe/default.htm
//        e The TrueType font standard -- http://en.wikipedia.org/wiki/Truetype
//
///////////////////////////////////////////////////////////////////////////////


#ifndef EATEXT_EATEXT_H
#define EATEXT_EATEXT_H


#include <EABase/eabase.h>
#include <EAText/EATextConfig.h>
#include <stddef.h>


namespace EA
{
    namespace Allocator
    {
        class ICoreAllocator;
    }

    namespace Text
    {
        // Forward declarations
        class Font;
        class FontServer;
        class StyleManager;
        class PolygonServer;


        /// EATEXT_DEFAULT_LOCALE
        ///
        /// Defines the default locale that EAText uses when no locale is supplied.
        ///
        #define EATEXT_DEFAULT_LOCALE EATEXT_CHAR("en-us")


        /// Char
        ///
        /// Defines the type of character used in this system. Most signficantly, this is 
        /// the data type used to specify text to layout and/or draw. By default, Char is 
        /// char16_t (char16_t is defined in EABase). You can redefine the Char type by 
        /// defining EATEXT_CHAR_TYPE as desired. Unix-based systems which use a 32 bit 
        /// wchar_t may want to define Char as wchar_t instead of char16_t.
        ///
        /// The Char type must be compatible with UCS Unicode. Thus all Chars must be UCS Unicode encoded and 
        /// multibyte encodings such as Chinese Big5, Windows MBCS, and UTF8 Unicode are not recognized. 
        /// You can convert multibyte encodings to UCS Unicode with utility functions if needed. The reason for
        /// this requirement is that it is much simpler to work with conventional Unicode characters than with
        /// multibyte characters. This library would need to be larger and more complicated if it had to support
        /// such characters. Despite Unix's reluctance to support UCS Unicode, the rest of the world has been 
        /// moving in that direction for some time.
        ///
        /// An invalid Char value is identified by kCharInvalid, which is defined 
        /// as (0xffff)in EATextUnicode.h, along with other special Char values. 
        /// A Char of zero is a valid Char value.
        ///
        typedef EATEXT_CHAR_TYPE Char;


        /// FilePathChar
        ///
        /// Defines the data type used to represent file paths. 
        /// It should be a wide character type or represent UTF8 file path names.
        typedef EATEXT_FILE_PATH_CHAR_TYPE FilePathChar;


        /// GlyphId
        ///
        /// Defines the type of glyph id used in this system. 
        /// Normally, GlyphId should be an unsigned type of at least 16 bits in size.
        /// You can redefine the GlyphId type by defining EATEXT_GLYPH_TYPE as desired.
        ///
        /// An invalid GlyphId value is identified by kGlyphIdInvalid, which is defined 
        /// below. There are no special GlyphId values other than kGlyphIdInvalid, and 
        /// a GlyphId of zero is a valid GlyphId as it merely refers to an index within
        /// a font table.
        ///
        typedef EATEXT_GLYPH_TYPE GlyphId;


        /// kGlyphIdInvalid
        ///
        /// Identifies an invalid GlyphId.
        ///
        static const GlyphId kGlyphIdInvalid = 0xffff;

        /// kGlyphIdZeroWidth
        ///
        /// Idenfies a glyph which is invisible, has no width, and has no advance.
        /// This glyph can be special-cased by layout and rendering systems.
        /// There exist multiple zero-width Unicode characters, though most fonts
        /// don't create an explicit glyph for any of them.
        ///
        static const GlyphId kGlyphIdZeroWidth = 0xfffe;


        /// Color
        ///
        /// Defines the standard representation of a color for our interfaces. 
        /// Internal color storage may be different and possibly less precise.
        /// Color is defined as a packed 32-bit integer (8 bits each of ARGB).
        ///
        typedef uint32_t Color;

        /// Point
        struct Point
        {
            float mX;
            float mY;

            Point(float x = 0, float y = 0) : mX(x), mY(y) {}
        };

        EATEXT_API bool operator<(const Point& pt1, const Point& pt2);


        /// Rectangle
        struct Rectangle
        {
            float mLeft;
            float mTop;
            float mRight;
            float mBottom;

            Rectangle(float left = 0, float top = 0, float right = 0, float bottom = 0) 
                : mLeft(left), mTop(top), mRight(right), mBottom(bottom) {}
        };

        EATEXT_API bool operator<(const Rectangle& rect1, const Rectangle& rect2);


        /// GetAllocator
        /// 
        /// Gets the default EAText ICoreAllocator set by the SetAllocator function.
        /// If SetAllocator is not called, the ICoreAllocator::GetDefaultAllocator 
        /// allocator is returned.
        ///
        EATEXT_API Allocator::ICoreAllocator* GetAllocator();


        /// SetAllocator
        /// 
        /// This function lets the user specify the default memory allocator this library will use.
        /// For the most part, this library avoids memory allocations. But there are times 
        /// when allocations are required, especially during startup.
        EATEXT_API void SetAllocator(Allocator::ICoreAllocator* pCoreAllocator);


        /// SetAssertionFailureFunction
        ///
        /// Allows the user to specify a callback function to trap assertion failures.
        /// You can use this to glue your own assertion system into this system.
        ///
        typedef void (*AssertionFailureFunction)(const char* pExpression, void* pContext);
        EATEXT_API void SetAssertionFailureFunction(AssertionFailureFunction pAssertionFailureFunction, void* pContext);


        /// AssertionFailure
        ///
        /// Triggers an assertion failure. This function is generally intended for internal
        /// use but is available so that related code can use the same system.
        ///
        EATEXT_API void AssertionFailure(const char* pExpression);


        /// Init / Shutdown
        ///
        /// EAText init / shutdown
        ///
        EATEXT_API void Init();
        EATEXT_API void Shutdown();


        #if EATEXT_FONT_SERVER_ENABLED
            /// GetFontServer
            ///
            /// Gets the primary font server.
            ///
            EATEXT_API FontServer* GetFontServer(bool bCreateIfNotFound = true);

            /// SetFontServer
            ///
            /// Sets the primary font server. Returns the previous one, which may be NULL.
            ///
            EATEXT_API FontServer* SetFontServer(FontServer* pFontServer);
        #endif



        #if EATEXT_LAYOUT_ENABLED
            class Layout;

            /// GetDefaultLayout
            ///
            /// Returns the default global layout object. Always returns a valid pointer.
            /// You do not want to modify this layout object, as it is only meant for cloning.
            /// Thus the default Layout object pointer is returned as const.
            ///
            EATEXT_API const Layout* GetDefaultLayout(bool bCreateIfNotFound = true);

            /// SetDefaultLayout
            ///
            /// Sets the default Layout object. Returns the previous one.
            /// Sets the default Layout, which is done by making a copy of the passed in Layout.
            ///
            EATEXT_API const Layout* SetDefaultLayout(const Layout* pLayout);
        #endif



        #if EATEXT_STYLE_MANAGER_ENABLED
            /// GetStyleManager
            ///
            /// Gets the default style manager. Always returns a valid pointer; an empty style
            /// manager will be initialized and returned if one wasn't specificaly set by SetStyleManager.
            ///
            EATEXT_API StyleManager* GetStyleManager(bool bCreateIfNotFound = true);

            /// SetStyleManager
            ///
            /// Sets the default style manager. Returns the previous StyleManager, which may have been NULL.
            ///
            EATEXT_API StyleManager* SetStyleManager(StyleManager* pStyleManager);

        #endif // EATEXT_STYLE_MANAGER_ENABLED


    } // namespace Text

} // namespace EA



#endif // EATEXT_EATEXT_H

















