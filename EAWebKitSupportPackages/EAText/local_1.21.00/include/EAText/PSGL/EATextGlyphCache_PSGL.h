///////////////////////////////////////////////////////////////////////////////
// EATextGlyphCache_PSGL.h
//
// Copyright (c) 2006 Electronic Arts. All Rights Reserved.
// By Alex Liberman, based on EATextCache_RenderWare.h by Paul Pedriana
//
///////////////////////////////////////////////////////////////////////////////


#ifndef EATEXT_EATEXTGLYPHCACHE_PSGL_H
#define EATEXT_EATEXTGLYPHCACHE_PSGL_H

#ifndef EATEXT_EATEXTCACHE_H
	#include <EAText/EATextCache.h>
#endif

// Forward-declare
struct PSGLdevice;

namespace EA
{
	namespace Text
	{
		/// GlyphCache_PSGL
		///
		/// Implements a PSGL-savvy glyph cache.
		///
		class EATEXT_API GlyphCache_PSGL : public GlyphCache
		{
		public:
			//////////////////////////////////////////////////////////////////////////
			// Track texture handle and associated system memory. This struct must 
			// be identical to the one in UTFWin/PSGL/GraphicsDevice_PSGL.h
			struct PSGLTextureInfo
			{
				uint32_t*   mpTextureData;
				uint32_t    mTextureID;
			};

			uintptr_t   CreateTexture(TextureInfo* pTextureInfo);
			bool        DestroyTexture(TextureInfo* pTextureInfo);

			bool        BeginUpdate(TextureInfo* pTextureInfo);
			bool        EndUpdate(TextureInfo* pTextureInfo);
		}; // class GlyphCache_PSGL

		/// GetGlyphCache_PSGL
		///
		/// Gets the primary glyph cache.
		///
		EATEXT_API GlyphCache_PSGL* GetGlyphCache_PSGL(bool bCreateIfNotFound = true);


		/// SetGlyphCache_PSGL
		///
		/// Sets the primary glyph cache.
		///
		EATEXT_API GlyphCache_PSGL* SetGlyphCache_PSGL(GlyphCache_PSGL* pGlyphCache);

	} // namespace Text

} // namespace EA

#endif // Header Sentry
