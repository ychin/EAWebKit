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
// EATextCache.h
// By Paul Pedriana - 2004
//
///////////////////////////////////////////////////////////////////////////////

///////////////////////////////////////////////////////////////////////////////
// GlyphCache Texture Mechanism
//
// A cache texture is divided into a number of predefined vertical bins (columns).
// Also, there is an open area on the right side of the texture which handles
// overflow from the columns on the left side. The left-most columns are 
// narrow and handle thin glyphs such as the ones shown below. As we move
// rightward, successive columns are wider. Glyphs are added to each column
// until the column runs out of space, at which point the glyph is tried in 
// the next column, and so on.
//
// Each column is on a four pixel x boundary. And each character vertically
// within a column begins on a four pixel y boundary. There is at least one
// blank pixel between each glyph. This is done because some texture sampling
// will read outside of the glyph, and we don't want to read pixels from 
// other glyphs.
// 
// ------------------------------------------------------------------
// | i  |   b   |   W   |       | D3TPZ+?M                          |
// | l  |   c   |   X   |       |                                   |
// | .  |   e   |   Q   |       |                                   |
// | :  |   d   |   H   |       |                                   |
// | ;  |   r   |       |       |           (open area)             |
// | ,  |       |       |       |                                   |
// |    |       |       |       |                                   |
// |    |       |       |       |                                   |
// |    |       |       |       |                                   |
// |    |       |       |       |                                   |
// |    |       |       |       |                                   |
// |    |       |       |       |                                   |
// |    |       |       |       |                                   |
// |    |       |       |       |                                   |
// |    |       |       |       |                                   |
// |    |       |       |       |                                   |
// |    |       |       |       |                                   |
// |    |       |       |       |                                   |
// |    |       |       |       |                                   |
// |    |       |       |       |                                   |
// |    |       |       |       |                                   |
// |    |       |       |       |                                   |
// |    |       |       |       |                                   |
// ------------------------------------------------------------------
///////////////////////////////////////////////////////////////////////////////


#ifndef EATEXT_EATEXTCACHE_H
#define EATEXT_EATEXTCACHE_H


#include <EAText/EAText.h>
#include <EASTL/hash_map.h>
#include <EASTL/fixed_vector.h>
#include <EASTL/core_allocator_adapter.h>
#include <coreallocator/icoreallocator_interface.h>
#include <stddef.h>
#include <string.h>

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

    namespace IO
    {
        class IStream;
    }

    namespace Text
    {
        // Defaults
        const int      kTextureSlotCount         = EA_TEXT_CACHE_TEXTURE_SLOT_COUNT;
        const int      kTextureSizeDefault       = EA_TEXT_CACHE_TEXTURE_SIZE;
        const int      kTextureColumnCountMax    = EA_TEXT_CACHE_TEXTURE_COLUMN_COUNT_MAX;
        const uint32_t kTextureMinSize           = 64;
        const bool     kTextureAutoCreateDefault = false;


        /// TextureFormat
        ///
        /// Defines the format/encoding of the texture.
        ///
        enum TextureFormat
        {
            kTextureFormatNone  =  0,                   // Same as Font::kBFNone.
            kTextureFormat1Bpp  =  1,                   // Same as Font::kBFMonochrome.
            kTextureFormat8Bpp  =  8,                   // Same as Font::kBFGrayscale.
            kTextureFormatARGB  = 32,                   // Same as Font::kBFARGB.
            kTextureFormatRGBA  = 33,                   // Same as Font::kBFRGBA.
            kTextureFormatABGR  = 34,                   // Same as Font::kBFABGR.
            kTextureFormatDXT3  =  3,                   // DXT3 encoding/compression. DXT3 can store ARGB in a compressed but lossy way.
            kTextureFormat32Bpp = kTextureFormatARGB    // For backwards compatibility.
        };

        /// PixelData
        typedef eastl::vector<uint32_t, EA::Allocator::EASTLICoreAllocator> PixelData;

        /// TextureInfo
        /// Stores information about a given texture's space for new glyphs.
        struct EATEXT_API TextureInfo
        {
            EA_PREFIX_ALIGN(8)
            uint8_t             mLockInformation[40] EA_POSTFIX_ALIGN(8);   /// Used by renderer for lock context.
            const FilePathChar* mpSource;                                   /// Non-null if this texture comes from some source (e.g. file path or asset name), usually in pre-made form such as a BmpFont.
            EA::IO::IStream*    mpStream;                                   /// Non-null if this texture comes from some stream source, usually in pre-made form such as a BmpFont.
            PixelData           mPixelData;                                 /// This is a system memory image of the texture; it may be empty (particularly if BmpFont::mbSystemMemoryCopy is false). Used by bmpFont textures that are stored as system-memory images.
            uintptr_t           mTexture;                                   /// This is either a straight integer id (e.g. OpenGL texture id) or this is a pointer to a texture object (e.g. DirectX IDirect3DTexture). Depends on the system.
            uint8_t*            mpData;                                     /// Only valid when the the texture is locked for read or write access.
            intptr_t            mnStride;                                   /// Only valid when the the texture is locked for read or write access.
            float               mfSizeInverse;                              /// 1/size. Used for scaling u/v values.
            uint32_t            mFormat;                                    /// Texture format plus color depth. One of enum TextureFormat unless the user is using private extended values.
            uint32_t            mnSize;                                     /// Size of the texture, which is 2D and of even powers of two in both dimensions.
            uint32_t            mnGeneration;                               /// Used to tell if any references to data in this texture are stale. The texture may be purged if it runs out of room.
            uint32_t            mnColumnHeights[kTextureColumnCountMax];    /// 
            uint32_t            mnOpenAreaX;                                /// 
            uint32_t            mnOpenAreaY;                                /// 
            uint32_t            mnOpenAreaLineH;                            /// 
            uint8_t             mnColumnCount;                              /// 
            uint8_t             mnColumnWidths[kTextureColumnCountMax];     /// 
            bool                mbWritable;                                 /// If true, this texture can be written to. If false, it is read-only.
            int mRefCount;

            Allocator::ICoreAllocator* mpCoreAllocator;               /// The allocator used to allocate this TextureInfo. Null if the TextureInfo isn't allocated from any heap.

        public:
            TextureInfo();
            virtual ~TextureInfo();

            virtual int AddRef();
            virtual int Release();
        };



        /// GlyphTextureInfo
        /// Used for bitmapped glyphs (as opposed to polygonal glyphs).
        struct EATEXT_API GlyphTextureInfo
        {
            TextureInfo* mpTextureInfo; /// The texture this glyph belongs to.
            float        mX1;           /// Position of glyph on texture. These are UV positions and so are between 0 and 1.
            float        mY1;           /// Position of glyph on texture.
            float        mX2;           /// Position of glyph on texture.
            float        mY2;           /// Position of glyph on texture.
        };


        /// GlyphCache
        ///
        /// A GlyphCache is a cache of precalculated font glyph information. Without a cache, 
        /// drawing individual glyphs would be much slower, especially for glyphs that are 
        /// derived from outline curves (i.e. TrueType fonts).
        ///
        class EATEXT_API GlyphCache
        {
        public:
            /// GlyphCache
            ///
            /// Constructs up the GlyphCache with basic parameters.
            ///
            GlyphCache(Allocator::ICoreAllocator* pCoreAllocator = NULL);
            GlyphCache(const GlyphCache&);
            GlyphCache& operator=(const GlyphCache&);

            /// ~GlyphCache
            ///
            EATEXT_VIRTUAL ~GlyphCache();


            /// SetAllocator
            ///
            /// Sets the memory allocator used by this class. By default the EAText allocator
            /// is used. You can use this function to set a specific allocator used just
            /// by this class. This function must be called before Init is called.
            ///
            EATEXT_VIRTUAL void SetAllocator(Allocator::ICoreAllocator* pCoreAllocator);

            /// Init
            ///
            /// This function doesn't necessarily need to be subclassed, but it 
            /// can be done. A subclassed version should call this version.
            /// Note that some application graphics layers can handle only a 
            /// single global glyph texture. UTFWin is such an example.
            ///
            EATEXT_VIRTUAL int Init(uint32_t nMaxTextureCount = 1, uint32_t nInitialTextureCount = 0);


            /// Shutdown
            ///
            /// This function doesn't necessarily need to be subclassed, but it 
            /// can be done. A subclassed version should call this version.
            ///
            EATEXT_VIRTUAL int Shutdown();


            enum Option
            {
                kOptionDefaultSize          =   0,    /// One of 128, 256, 512, 1024, etc. Individual platforms will have min/max limits on these. Default is 256. Minimum is kTextureMinSize.
                kOptionDefaultFormat        =   1,    /// One of enum TextureFormat. Default is the most compressed color-supporting format for the given platform (usually DXT3).
                kOptionAutoTextureCreate    =   2,    /// If enabled, we auto-create new textures when we run out of space. Default is false.
                kOptionDoubleBuffer         =   3,    /// If enabled, then textures are double-buffered, which means that textures are in pairs: one used by the hardware and one used to update glyphs. This uses more memory but reduces texture contention. Default is disabled.
                kOptionGlyphPadding         =   4,    /// Value >= 0; default is 1. Specifies the amount of empty space around glyphs. A value of 1 means that there is 1 empty pixel on all four sides of glyphs. It is useful to have extra space around glyphs when implementing shadow multi-sampling.
                kOptionMaxTextureCount      =   5,    /// The maximum number of textures to create.
                kOptionColumnWidthScale     =   6,    /// Specifies how much to scale the glyph texture column sizes, in 16:16 fixed point. Default is 1. Fixed point is used because SetOption uses int32_t and not float. Useful for handling high resolution displays more efficiently, where your font sizes are all much bigger than normal. If you use this to double the scale, you may need to make sure the texture size is big enough, lest it assert than the column size/positions don't fit.
                kOptionColumnCount          =  99,    /// Must be set before using this class.
                kOptionColumnValueBase      = 100     /// Must be set before using this class.
            };


            /// GetOption
            ///
            EATEXT_VIRTUAL int32_t GetOption(int32_t option) const;

            /// SetOption
            ///
            EATEXT_VIRTUAL void SetOption(int32_t option, int32_t value);


            /// GetGlyphTextureInfo
            ///
            /// This function generally doesn't need to be subclassed.
            ///
            EATEXT_VIRTUAL bool GetGlyphTextureInfo(Font* pFont, GlyphId glyphId, 
                                        GlyphTextureInfo& glyphTextureInfo) const;


            /// GetGlyphTextureInfo
            ///
            /// This is an alternative version that takes scaling information for the glyph.
            ///
            /// This function generally doesn't need to be subclassed.
            ///
            EATEXT_VIRTUAL bool GetGlyphTextureInfo(Font* pFont, GlyphId glyphId, float fScaleX, float fScaleY,
                                        GlyphTextureInfo& glyphTextureInfo) const;


            /// SetGlyphTextureInfo
            ///
            /// This function is useful for subclasses to call and for pre-generated
            /// BmpFont systems to use to seed the glyph data. Normally only EAText-related
            /// code needs to call this function.
            ///
            /// This function generally doesn't need to be subclassed.
            ///
            EATEXT_VIRTUAL bool SetGlyphTextureInfo(Font* pFont, GlyphId glyphId, 
                                        const GlyphTextureInfo& glyphTextureInfo);


            /// SetGlyphTextureInfo
            ///
            /// This is an alternative version that takes scaling information for the glyph.
            ///
            /// This function generally doesn't need to be subclassed.
            ///
            EATEXT_VIRTUAL bool SetGlyphTextureInfo(Font* pFont, GlyphId glyphId, float fScaleX, float fScaleY,
                                        const GlyphTextureInfo& glyphTextureInfo);

            /// RemoveTextureInfo
            ///
            /// Removes the texture info for the given Font. This is useful for the 
            /// case that you want to destroy the Font and make both it and its
            /// cached glyphs go away.
            ///
            /// This function generally doesn't need to be subclassed.
            /// 
            EATEXT_VIRTUAL void RemoveTextureInfo(const Font* pFont);



            /// AddGlyphTexture
            ///
            /// Adds a new glyph to the glyph cache, based on a pixel map from a Font.
            /// This function must be called between calls to BeginUpdate and 
            /// EndUpdate. You can (and should when possible) make multiple 
            /// calls to this function after a call to BeginUpdate and before
            /// a call to EndUpdate. This function internally calls WriteTextureArea.
            ///
            /// This function generally doesn't need to be subclassed.
            ///
            /// This function is misnamed, it should be called AddGlyph instead of AddGlyphTexture.
            ///
            EATEXT_VIRTUAL bool AddGlyphTexture(Font* pFont, GlyphId glyphId, 
                                                const void* pSourceData, uint32_t nSourceSizeX, uint32_t nSourceSizeY, 
                                                uint32_t nSourceStride, uint32_t nSourceFormat,
                                                EA::Text::GlyphTextureInfo& glyphTextureInfo);

            /// AddGlyphTexture
            ///
            /// This is an alternative version that takes scaling information for the glyph.
            ///
            /// This function generally doesn't need to be subclassed.
            ///
            /// This function is misnamed, it should be called AddGlyph instead of AddGlyphTexture.
            ///
            EATEXT_VIRTUAL bool AddGlyphTexture(Font* pFont, GlyphId glyphId, float fScaleX, float fScaleY,
                                                const void* pSourceData, uint32_t nSourceSizeX, uint32_t nSourceSizeY, 
                                                uint32_t nSourceStride, uint32_t nSourceFormat,
                                                EA::Text::GlyphTextureInfo& glyphTextureInfo);

            /// CreateTexture
            ///
            /// Creates an empty texture of the given input size. 
            /// The input size must be zero or a power of two or the value will be rounded up 
            /// to the nearest power of two.
            /// A 'size' value of zero means to use the default size.
            /// The return value is a graphics-system-specific texture identifier.
            /// This function needs to be overridden by a subclass which understands
            /// the given textures at hand.
            ///
            EATEXT_VIRTUAL uintptr_t CreateTexture(TextureInfo* pTextureInfo) = 0;


            /// DestroyTexture
            ///
            /// Clears the texture of its glyphs and destroys it.
            /// This function needs to be overridden by a subclass which understands
            /// the given textures at hand.
            ///
            EATEXT_VIRTUAL bool DestroyTexture(TextureInfo* pTextureInfo) = 0;


            /// GetTextureInfo
            /// 
            /// This function generally doesn't need to be subclassed.
            /// The returned TextureInfo* is not AddRefd for the user.
            ///
            EATEXT_VIRTUAL TextureInfo* GetTextureInfo(uint32_t nTextureIndex);


            /// GetTextureInfoCount
            ///
            /// This function generally doesn't need to be subclassed.
            /// 
            EATEXT_VIRTUAL uint32_t GetTextureInfoCount() const;


            /// AddTextureInfo
            ///
            /// Allows manual addition of TextureInfo.
            /// If the pTextureInfo argument is non-NULL, it is AddRefd and stored
            /// internally from that point on. If pTextureInfo is NULL, a new TextureInfo
            /// is created and stored internally.
            /// If bInitialized is true, then it is assumed that all information
            /// in the TextureInfo struct is complete and correct. If it is false,
            /// then AddTextureInfo will create the texture if TextureInfo::mTexture
            /// is zero, and will initialize other data members in any case.
            /// The returned TextureInfo* is not AddRefd for the user.
            ///
            /// This function generally doesn't need to be subclassed.
            ///
            EATEXT_VIRTUAL TextureInfo* AddTextureInfo(TextureInfo* pTextureInfo = NULL, bool bInitialized = false, bool bIgnoreSizeLimits = false);


            /// ClearTexture
            ///
            /// Clears the texture of its glyphs. This would be done when there is no more
            /// space on any of our textures and we cannot allocate new textures. In this case
            /// we must clear an existing texture of its glyphs to make room.
            ///
            /// This function generally doesn't need to be subclassed.
            ///
            EATEXT_VIRTUAL bool ClearTexture(TextureInfo* pTextureInfo);


            /// ClearTextureImage
            ///
            /// Clears the given texture's image, but not any data associated with it.
            /// The destination size refers to texels and not bytes.
            /// This function will usually be called internally by ClearTexture.
            ///
            /// This function generally doesn't need to be subclassed.
            /// 
            EATEXT_VIRTUAL void ClearTextureImage(void* pDest, uint32_t nDestSize, uint32_t nDestStride, uint32_t nDestFormat);


            /// AllocateTextureArea
            ///
            /// Finds space for a glyph of the given size. This may result in creating new 
            /// textures or clearing an existing texture, depending on our settings and state.
            ///
            /// This function generally doesn't need to be subclassed. It would only need to 
            /// be subclassed when the texture encoding is a format that is unique.
            ///
            /// The returned TextureInfo* is not AddRefd for the user.
            /// 
            EATEXT_VIRTUAL TextureInfo* AllocateTextureArea(uint32_t xSize, uint32_t ySize, 
                                                    uint32_t& xPosition, uint32_t& yPosition);

            /// BeginUpdate
            ///
            /// This function is called by the user before doing writes to the texture cache.
            /// This function must be matched by a call to EndUpdate as soon as possible.
            /// BeginUpdate is currently not recursive: If you call it a second time before 
            /// calling EndUpdate it will return false. You can check if it's already been called
            /// by calling IsUpdating, though that has limitations documented below. 
            /// To consider: Make BeginUpdate support recursivity.
            ///
            /// This function needs to be overridden by a subclass which understands
            /// the given textures at hand.
            /// 
            EATEXT_VIRTUAL bool BeginUpdate(TextureInfo* pTextureInfo) = 0;


            /// IsUpdating
            ///
            /// Returns true if IsUpdating is active.
            /// This function is subject to thread race problems if textures could be modified
            /// by threads other than the current thread.
            ///
            EATEXT_VIRTUAL bool IsUpdating(TextureInfo* pTextureInfo) const
                { return (pTextureInfo && pTextureInfo->mpData != NULL); }


            /// EndUpdate
            ///
            /// This function is called by the user before doing writes to the texture cache.
            ///
            /// This function needs to be overridden by a subclass which understands
            /// the given textures at hand.
            /// 
            EATEXT_VIRTUAL bool EndUpdate(TextureInfo* pTextureInfo) = 0;


            /// WriteTextureArea
            ///
            /// Writes to the given texture. 
            /// This function should be called between calls to BeginUpdate and EndUpdate.
            /// The TextureInfo should have valid mpData and mStride values, as that's 
            /// what BeginUpdate should set.
            /// This function is called internally by AddGlyphTexture, so you don't need
            /// to call this function if you are calling AddGlyphTexture.
            ///
            /// This function generally doesn't need to be subclassed.
            /// 
            EATEXT_VIRTUAL bool WriteTextureArea(TextureInfo* pTextureInfo, uint32_t nDestPositionX, uint32_t nDestPositionY,
                                                 const void* pSourceData, uint32_t nSourceSizeX, uint32_t nSourceSizeY,    
                                                 uint32_t sourceStride, uint32_t nSourceFormat);

            /// WriteTextureArea
            ///
            /// Writes to the given destination block of memory.
            ///
            /// This function generally doesn't need to be subclassed, but you may want to
            /// subclass it (override it) if you have a unique way that you want to write texture data.
            /// 
            EATEXT_VIRTUAL void WriteTextureArea(void* pDest, uint32_t nDestPositionX, uint32_t nDestPositionY, 
                                                 uint32_t nDestStride, uint32_t nDestFormat,
                                                 const void* pSourceData, uint32_t nSourceSizeX, uint32_t nSourceSizeY,
                                                 uint32_t nSourceStride, uint32_t nSourceFormat);

            /// GetGlyphTextureData
            ///
            /// Returns the texture cache as a block of data which can be stored on disk and later retrieved.
            /// The return value is the required size of the glyph data. If the input size is not large enough
            /// then no data is written.
            ///
            /// This function generally doesn't need to be subclassed.
            /// 
            EATEXT_VIRTUAL uint32_t GetGlyphTextureData(void* pGlyphData, uint32_t nGlyphDataSize, bool bPortable = true);


            /// SetGlyphTextureData
            ///
            /// Sets a batch of glyph textures. This is useful for the loading of precalculated information
            /// on application startup. The format of the data is internal and the data itself is generated
            /// by calls to GetGlyphTextureData. Any existing data is replaced.
            ///
            /// This function generally doesn't need to be subclassed.
            /// 
            EATEXT_VIRTUAL bool SetGlyphTextureData(const void* pGlyphData, uint32_t nGlyphDataSize);

        protected:
            #define EATextFixed106ToFloat(a)  (((float)a) / 64.f)
            #define EATextFloatToFixed106(a)  ((uint16_t)((a) * 64.f))

            struct GlyphInfo
            {
                Font*    mpFont;
                GlyphId  mGlyphId;   // Usually 16 bits.
                uint16_t muScaleX;   // In 10:6 fixed point.
                uint16_t muScaleY;   // In 10:6 fixed point.

                GlyphInfo(Font* pFont = NULL, GlyphId glyphId = 0, float fScaleX = 0.0f, float fScaleY = 0.0f)
                    : mpFont(pFont), mGlyphId(glyphId), muScaleX(EATextFloatToFixed106(fScaleX)), muScaleY(EATextFloatToFixed106(fScaleY)) { }

                bool operator==(const GlyphInfo& gi) const
                    { return (mpFont == gi.mpFont) && (mGlyphId == gi.mGlyphId) && (muScaleX == gi.muScaleX) && (muScaleY == gi.muScaleY); }
            };

            struct GlyphInfoHash
            {
                uint32_t operator()(const GlyphInfo& gi) const 
                    { return (uint32_t)((uintptr_t(gi.mpFont) << 16) + ((gi.mGlyphId * gi.muScaleX * gi.muScaleY) >> 8)); }
            };

            /// TextureInfoArray
            /// Entries are AddRefd.
            typedef eastl::fixed_vector<TextureInfo*, kTextureSlotCount, true> TextureInfoArray;

            /// GlyphTextureMap
            /// Map of all Glyph/Font to GlyphTextureInfo.
            typedef eastl::hash_map<GlyphInfo, GlyphTextureInfo, GlyphInfoHash, 
                                    eastl::equal_to<GlyphInfo>, EA::Allocator::EASTLICoreAllocator> GlyphTextureMap;

        protected:
            #ifdef EA_DEBUG
                bool ValidateGlyphTextureMap() const;
                bool ValidateGlyphInfo(const GlyphInfo& glyphInfo) const;
                void EnableAutoTextureWrite(const char* pDirectory, bool bEnabled);
                bool WritePPMFile(const char* pPath, uint32_t pixelFormat, const void* pData, uint32_t width, 
                                    uint32_t height, uint32_t strideBytes, bool bAlphaOnly = true, uint32_t backgroundColorRGB = 0xffffffff);
            #endif

        protected:
            /// TryAllocateTextureArea
            ///
            /// Tries to allocate space from a specific texture.
            /// See the implementation for an explanation and diagram of how it works.
            ///
            EATEXT_VIRTUAL bool TryAllocateTextureArea(uint32_t xSize, uint32_t ySize, 
                                        TextureInfo& textureInfo, uint32_t& xPosition, uint32_t& yPosition);

            /// ClearTextureInternal
            ///
            /// Clears the texture associated with TextureInfo. The caller of this
            /// function will not call BeginUpdate/EndUpdate before and after calling
            /// this function and so the subclassed version of this function must
            /// do such a thing itself. The reason for this is that it sometimes 
            /// happens that the fastest way to clear a texture is not to lock it
            /// and write data to it but to do other operations instead and which
            /// only the given implementation can know about.
            ///
            /// The caller will lock the mMutex member variable before calling this function.
            ///
            /// This function needs to be overridden by a subclass which understands
            /// the given textures at hand.
            /// 
            EATEXT_VIRTUAL void ClearTextureInternal(TextureInfo* pTextureInfo);

        protected:
            Allocator::ICoreAllocator* mpCoreAllocator;                                /// Memory allocator.
            GlyphTextureMap            mGlyphTextureMap;                               /// A map of all cached glyphs to glyph texture information.
            TextureInfoArray           mTextureInfoArray;                              /// Array of AddRefd TextureInfo.
            uint32_t                   mnTextureInfoCountMax;                          /// Max count of textures to maintain. Defaults to 1.
            uint32_t                   mnTextureSizeDefault;                           /// Default size of textures.
            uint32_t                   mnTextureFormatDefault;                         /// One of enum TextureFormat. Defaults to kTextureFormatNone (back-end decides the best format).
            float                      mOptionColumnWidthScale;                        ///
            uint32_t                   mnColumnCountDefault;                           ///
            uint32_t                   mnColumnWidthsDefault[kTextureColumnCountMax];  /// 
            bool                       mbAutoTextureCreate;                            /// If true, we auto-create new textures when we run out of space. Defaults to false.
            bool                       mbDoubleBuffer;                                 /// If true, dynamically written textures are double-buffered.
            int32_t                    mnGlyphPadding;                                 /// Specifies extra space around glyphs in the glyph texture. A value of 1 means that there is 1 empty pixel on all four sides of glyphs.
            int8_t                     mRecursionCounter;                              /// Used to prevent infinite recursion in self-calling functions.

            #ifdef EA_DEBUG
                bool                   mbDebugBitmapEnable;
                uint32_t               mDebugBitmapCounter;
                char                   mDebugBitmapDirectory[256];
            #endif

            int                       mnInitCount;
        };



        /// GlyphCache_Memory
        ///
        /// Implements a generic memory-based GlyphCache, whereby the texture(s) used
        /// to store glyphs is a simple memory RAM image and not a hardware texture.
        /// A memory GlyphCache is useful for implementing a backing store for hardware
        /// images and is useful for implementing reference or testing behaviour.
        ///
        class EATEXT_API GlyphCache_Memory : public GlyphCache
        {
        public:
            GlyphCache_Memory(TextureFormat textureFormat = kTextureFormatARGB)
                : mTextureFormat(textureFormat) { }

            uintptr_t CreateTexture(TextureInfo* pTextureInfo);
            bool      DestroyTexture(TextureInfo* pTextureInfo);

            bool BeginUpdate(TextureInfo* pTextureInfo);
            bool EndUpdate(TextureInfo* pTextureInfo);

        protected:
            TextureFormat mTextureFormat;
        };



        /// TextureRowIterator
        ///
        /// Iterates rows of an arbitrary memory block of a given pitch.
        ///
        class EATEXT_API TextureIterator
        {
        public:
            TextureIterator(void* pData = NULL, ptrdiff_t nPitch = 0) : mpData((char*)pData), mPitch(nPitch) { }

            operator void*    () const { return            mpData; }
            operator uint8_t* () const { return (uint8_t*) mpData; }
            operator uint16_t*() const { return (uint16_t*)mpData; }
            operator uint32_t*() const { return (uint32_t*)mpData; }

            char* operator--()    { mpData -= mPitch; return mpData; }
            char* operator++()    { mpData += mPitch; return mpData; }
            char* operator--(int) { char* const p = mpData; mpData -= mPitch; return p; }
            char* operator++(int) { char* const p = mpData; mpData += mPitch; return p; }

            TextureIterator& operator-=(int n) { mpData -= mPitch * n; return *this; }
            TextureIterator& operator+=(int n) { mpData += mPitch * n; return *this; }

            void Offset(int x, int y) { mpData += ((mPitch * y) + x); }

        protected:
            char*     mpData;
            ptrdiff_t mPitch;
        };

    } // namespace Text

} // namespace EA


#ifdef _MSC_VER
    #pragma warning(pop)
#endif


#endif // EATEXT_EATEXTCACHE_H





