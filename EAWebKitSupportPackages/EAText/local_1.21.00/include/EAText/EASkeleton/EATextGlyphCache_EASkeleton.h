///////////////////////////////////////////////////////////////////////////////
// EATextGlyphCache_EASkeleton.h
//
// Copyright (c) 2012 Electronic Arts. All Rights Reserved.
//
///////////////////////////////////////////////////////////////////////////////

#ifndef EATEXT_EATEXTGLYPHCACHE_EASKELETON_H
#define EATEXT_EATEXTGLYPHCACHE_EASKELETON_H

#ifndef EATEXT_EATEXTCACHE_H
	#include <EAText/EATextCache.h>
#endif

namespace EA
{
	
	namespace Text
	{
		/// GlyphCache_EASkeleton
		///
		/// Implements a platform-independent glyph cache.
		///
		class EATEXT_API GlyphCache_EASkeleton : public GlyphCache
		{
		public:

			GlyphCache_EASkeleton();

			uintptr_t   CreateTexture(TextureInfo* pTextureInfo);
			bool        DestroyTexture(TextureInfo* pTextureInfo);

			bool        BeginUpdate(TextureInfo* pTextureInfo);
			bool        EndUpdate(TextureInfo* pTextureInfo);

		}; // class GlyphCache_D3D

		/// GetGlyphCache_EASkeleton
		///
		/// Gets the primary glyph cache.
		///
		EATEXT_API GlyphCache_EASkeleton* GetGlyphCache_EASkeleton(bool bCreateIfNotFound = true);


		/// SetGlyphCache_EASkeleton
		///
		/// Sets the primary glyph cache.
		///
		EATEXT_API GlyphCache_EASkeleton* SetGlyphCache_EASkeleton(GlyphCache_EASkeleton* pGlyphCache);

	} // namespace Text

} // namespace EA

#endif // Header Sentry
