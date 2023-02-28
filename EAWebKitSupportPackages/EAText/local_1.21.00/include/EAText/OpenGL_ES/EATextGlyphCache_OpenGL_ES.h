///////////////////////////////////////////////////////////////////////////////
// EATextGlyphCache_OPENGL_ES.h
//
// Copyright (c) 2006 Electronic Arts. All Rights Reserved.
//
///////////////////////////////////////////////////////////////////////////////


#ifndef EATEXT_EATEXTGLYPHCACHE_OPENGL_ES_H
#define EATEXT_EATEXTGLYPHCACHE_OPENGL_ES_H

#ifndef EATEXT_EATEXTCACHE_H
	#include <EAText/EATextCache.h>
#endif

namespace EA
{
	namespace Text
	{
		/// GlyphCache_OPENGL_ES
		///
		/// Implements a OpenGL ES-savvy glyph cache.
		///
		class EATEXT_API GlyphCache_OPENGL_ES : public GlyphCache
		{
		public:
			//////////////////////////////////////////////////////////////////////////
			// Track texture handle and associated system memory. This struct must 
			// be identical to the one in UTFWin/PSGL/GraphicsDevice_PSGL.h
			struct GLESTextureInfo
			{
				uint32_t* mpTextureData;
				uint32_t  mTextureID;
			};

			uintptr_t   CreateTexture(TextureInfo* pTextureInfo);
			bool        DestroyTexture(TextureInfo* pTextureInfo);

			bool        BeginUpdate(TextureInfo* pTextureInfo);
			bool        EndUpdate(TextureInfo* pTextureInfo);
		};


		/// GetGlyphCache_OPENGL_ES
		///
		/// Gets the primary glyph cache.
		///
		EATEXT_API GlyphCache_OPENGL_ES* GetGlyphCache_OpenGL_ES(bool bCreateIfNotFound = true);


		/// SetGlyphCache_OPENGL_ES
		///
		/// Sets the primary glyph cache.
		///
		EATEXT_API GlyphCache_OPENGL_ES* SetGlyphCache_OpenGL_ES(GlyphCache_OPENGL_ES* pGlyphCache);

	} // namespace Text

} // namespace EA

#endif // Header Sentry
