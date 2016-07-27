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
// EATextPolygon.h
//
// By Paul Pedriana - 2004
//
// Implements a Font based on polygons as opposed to texture impostors.
//
///////////////////////////////////////////////////////////////////////////////


#ifndef EATEXT_EATEXTPOLYGON_H
#define EATEXT_EATEXTPOLYGON_H


#include <EABase/eabase.h>
#include <EAText/EAText.h>
#include <EAText/EATextFont.h>
#include <EASTL/vector.h>
#include <EASTL/hash_map.h>
#include <EASTL/map.h>


#ifdef _MSC_VER
    #pragma warning(push)           // We have to be careful about disabling this warning. Sometimes the warning is meaningful; sometimes it isn't.
    #pragma warning(disable: 4251)  // class (some template) needs to have dll-interface to be used by clients.
#endif


namespace EA
{
    namespace IO
    {
        class IStream;
    }

    namespace Text
    {
        class  Font;
        struct FontDescription;


        /// Vertex3D
        /// A simple vertex (and vector) description.
        /// This is the vertex type used by PolygonGlyph, which is the optional built-in 
        /// platform-neutral Glyph representation. 
        struct Vertex3D
        {
            float mX,  mY,  mZ;     // Vertex
            float mNX, mNY, mNZ;    // Normal vector
        };

        typedef eastl::vector<Vertex3D, EA::Allocator::EASTLICoreAllocator> VertexArray;
        typedef eastl::vector<uint16_t, EA::Allocator::EASTLICoreAllocator> IndexArray;


        /// PolygonGlyph
        ///
        /// This is the built-in platform-neutral glyph representation (mesh).
        /// PolygonFont can use this format or the user can set glyphs to use
        /// a user-defined representation that is custom to their rendering system.
        /// If you use this PolygonGlyph, then you'll probably need to translate
        /// this glyph's vertex format to your rendering system's vertex or mesh format.
        ///
        /// This glyph is stored as an indexed vertex buffer, with the vertices 
        /// defined in mVertexArray and the indexes defined in mIndexArray.
        /// See PolygonFont for how to use your own custom polygon glyph format.
        ///
        /// Each glyph's vertices are in a coordinate system with the origin at the 
        /// (left side) pen position. For Right-to-left glyphs (e.g. Hebrew), the origin
        /// is at the (left side) advance position.
        ///       
        ///     Y ^
        ///       |    Z     *
        ///       |   /     * *
        ///       |  /     *   *
        ///       | /     *******
        ///       |/     *       *
        ///       o-----*---------*----> 
        ///                        X
        struct EATEXT_API PolygonGlyph
        {
            uint16_t        mnBackFaceVertexStart;    /// First index for the back face vertices. If there is no back face, then mnBackFaceVertexStart == mVertextArray.size(). The back face index count is equal to the front face index count. The side vertices (for triangles that are along the sides of an extruded glyph) come from the front and back vertices; there are no new such vertices.
            uint16_t        mnBackFaceIndexStart;     /// First index for the back face vertex indexes. The beginning of the side vertices (for triangles that are along the sides of an extruded glyph) are at (2 * mnBackFaceIndexStart).
            VertexArray     mVertexArray;             /// All the vertices in this glyph.
            IndexArray      mIndexArray;              /// All the indexes to this glyphs vertices. If mIndexArray.size == 0, then vertices are not indexed but are triangles. Indexes are ordered as front, back, side. If the index array size is > (mnBackFaceIndexStart * 2), then there are side vertices.
        
            PolygonGlyph();
        };

        union PolygonGlyphPtr
        {
            PolygonGlyph* mpPolygonGlyph;   /// Generic platform-neutral polygon glyph format.
            void*         mpUserGlyph;      /// User-defined, supplied, and managed mesh. 
        };


        /// PolygonFont
        ///
        /// Implements a polygonal glyph, which is a glyph defined by a 2D or 3D mesh instead of 
        /// a glyph defined by a 2D texture. This has the advantage of being scalable and having 
        /// 3D depth. The primary disadvantage is that it can't represent small glyphs as well
        /// as a bitmapped glyph from OutlineFont or BitmapFont.
        ///
        /// This class provides a default glyph mesh format (PolygonGlyph) that is platform-neutral.
        /// However, the user can instead disable the default glyph mesh format and instead provide
        /// a format of the user's own design. See kOptionUserGlyphInfo and SetUserGlyphInfo for 
        /// more information about this. 
        ///
        /// This class allocates no memory at runtime and changes no data (i.e. is const) once 
        /// the font is opened. Thus this class is inherently thead-safe once it is set up.
        /// 
        class EATEXT_API PolygonFont : public Font
        {
        public:
            PolygonFont(Allocator::ICoreAllocator* pCoreAllocator = NULL);
            PolygonFont(const PolygonFont&);
            PolygonFont& operator=(const PolygonFont&);

            EATEXT_VIRTUAL ~PolygonFont();


            /// SetAllocator
            EATEXT_VIRTUAL void SetAllocator(Allocator::ICoreAllocator* pCoreAllocator);


            /// Option
            ///
            /// Defines options implemented by this class.
            ///
            enum Option
            {
                kOptionUserGlyphInfo = 301  /// 1 or 0; default is 0. If enabled, glyphs are expected to be user-defined (PolygonGlyphPtr::mpUserGlyph). Must be called before the font is opened. If the font has no glyph information then this option can be set at any time.
            };

            /// SetOption
            ///
            EATEXT_VIRTUAL void SetOption(int32_t option, int32_t value);


            /// Open
            /// 
            /// Initializes a PolygonFont for usage with data supplied by the stream functions.
            /// Returns true if the data source could be read and the PolygonFont could be initialized for usage.
            /// The Stream is AddRefd by the font for its usage and is Released upon completion of usage.
            ///
            /// The input nFaceIndex refers to the 0-based index of a particular font face within a font file.
            /// Some font formats (e.g. TrueType) allow for multiple fonts to be stored in a single TrueType
            /// font file. The face index allows you to refer to a specific font within the file. If the given
            /// index is not present, the return value is false. You can count the faces in a font file by 
            /// simply calling Open with progressively increasing face indexes until the return value is false.
            ///
            EATEXT_VIRTUAL bool Open(IO::IStream* pStream);


            /// Open
            ///
            /// Initializes a PolygonFont from a complete font memory image. The memory is read by the PolygonFont but
            /// is not written to. The source data is not freed by the PolygonFont when usage is complete.
            ///
            EATEXT_VIRTUAL bool Open(const void* pSourceData, uint32_t nSourceSize);


            /// Close
            ///
            /// Closes the font and frees any resources allocated by it 
            /// after being opened. The Font is returned to the same essential 
            /// state that it was before being Opened. The font must be opened 
            /// or the return value will be false. If the font is not closed 
            /// before it is destroyed, it will be closed upon its final Release.
            ///
            EATEXT_VIRTUAL bool Close();


            /// GetBitmapFormat
            ///
            EATEXT_VIRTUAL BitmapFormat GetBitmapFormat() const;


            /// GetFontType
            ///
            /// Returns kFontTypePolygon if font is opened, else kFontTypeUnknown. 
            ///
            EATEXT_VIRTUAL FontType GetFontType();


            /// GetOTF
            ///
            /// Returns the OpenTypeFont object for this Font. OpenType information is going to be 
            /// generally found only outline fonts that have complex scripts such as Thai or Arabic.
            /// This function returns NULL if the Font has no OpenType information. It is possible
            /// that the returned OTF may be empty or may be lacking information for the script 
            /// which you are shaping.
            ///
            EATEXT_VIRTUAL OTF* GetOTF();


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
            /// Returns the size of the font. The size is based on the transform.
            /// This function merely returns the FontDescription.mfSize member, which you 
            /// can also retrieve via GetFontDescription().
            ///
            EATEXT_VIRTUAL float GetSize() const;


            /// SetTransform
            /// 
            EATEXT_VIRTUAL bool SetTransform(float f00, float f01, float f10, float f11);
            EATEXT_VIRTUAL bool SetTransform(float fSize, float fAngle = 0.f, float fSkew = 0.f);


            /// SetUserGlyphInfo
            ///
            /// This function allows the user to set the glyph mesh assigned to glyphs.
            /// This allows for the application to use its own glyph mesh format.
            /// Assumes that kOptionUserGlyphInfo is enabled. 
            /// It turns out that the user doesn't need to actually set the glyph mesh 
            /// info if the user has some other means of getting the glyph mesh for a
            /// given GlyphId at render time. This PolygonFont class doesn't itself use
            /// the glyph mesh info, nor indeed does any other part of the EAText package.
            /// However, it is important that user-supplied glyph meshes have metrics that
            /// correspond to the GlyphMetrics this PolygonFont has for each glyph.
            /// EAText depends on accurate and consistent glyph metrics to do layout.
            /// 
            EATEXT_VIRTUAL void SetUserGlyphInfo(GlyphId glyphId, void* pUserInfo);


            /// RenderGlyphBitmap
            ///
            /// GlyphBitmap::mpData is a PolygonGlyphPtr. 
            ///
            /// Example usage:
            ///    const GlyphBitmap* pGlyphBitmap;
            ///    pPolygonFont->RenderGlyphBitmap(&pGlyphBitmap, 37);
            ///    PolygonGlyph* pPolygonGlyph = (PolygonGlyph*)pGlyphBitmap->mpData;
            ///    (use pPolygonGlyph->mVertexArray)
            ///    pPolygonFont->DoneGlyphBitmap();
            /// 
            /// Example usage:
            ///    const GlyphBitmap* pGlyphBitmap;
            ///    pPolygonFont->RenderGlyphBitmap(&pGlyphBitmap, 37);
            ///    MyMeshFormat* pMesh = (MyMeshFormat*)pGlyphBitmap->mpData;
            ///    (use pMesh)
            ///    pPolygonFont->DoneGlyphBitmap();
            /// 
            EATEXT_VIRTUAL bool RenderGlyphBitmap(const GlyphBitmap** pGlyphBitmap, GlyphId g, 
                                                    uint32_t renderFlags = kRFDefault, float fXFraction = 0, float fYFraction = 0);

            /// DoneGlyphBitmap
            ///
            /// This function doesn't need to be called if the user is managing glyph information.
            ///
            EATEXT_VIRTUAL void DoneGlyphBitmap(const GlyphBitmap* pGlyphBitmap);


            /// IsCharSupported
            ///
            EATEXT_VIRTUAL bool IsCharSupported(Char c, Script script);


            /// GetKerning
            ///
            /// Retrieve custom kerning information for the given glyph pair.
            ///
            EATEXT_VIRTUAL bool GetKerning(GlyphId g1, GlyphId g2, Kerning& kerning, int direction, bool bHorizontalLayout = true);

        protected:
            // To consider: We may want to forego storing font information here and 
            // instead keep a pointer to an associated OutlineFont object. That way we 
            // can much more easily support advanced Font functionality such as custom
            // kerning and OpenType information required for complex scripts.

            typedef eastl::hash_map<GlyphId, PolygonGlyphPtr, eastl::hash<uint32_t>,
                                    eastl::equal_to<GlyphId>, EA::Allocator::EASTLICoreAllocator> PolygonGlyphMap;

            typedef eastl::map<Char, GlyphId, eastl::less<Char>, 
                                EA::Allocator::EASTLICoreAllocator> CharGlyphMap;

        protected:
            bool                mbUserGlyphInfo;    /// If true, then the Glyph mesh is user-defined and PolygonGlyph is not used. 
            FontDescription     mFontDescription;   /// Font description (name, style, etc.).
            FontMetrics         mFontMetrics;       /// Font-wide metrics (ascent, descent, etc.).
            GlyphBitmap         mGlyphBitmap;       /// This is temporary struct given to the user.
            GlyphMetricsMap     mGlyphMetricsMap;   /// The glyph metrics.
            PolygonGlyphMap     mPolygonGlyphMap;   /// Map of glyphs to their glyph info (metrics and mesh). May be empty if the user is entirely supplying glyph meshes on the user side.
            CharGlyphMap        mCharGlyphMap;      /// Map of characters to glyphs.

        }; // class PolygonFont

    } // namespace Text

} // namespace EA


#ifdef _MSC_VER
    #pragma warning(pop)
#endif


#endif // Header include guard



