///////////////////////////////////////////////////////////////////////////////
// EATextGlyphCache_gxm.h (PSVita platform)
//
// Copyright (c) 2011 Electronic Arts. All Rights Reserved.
///////////////////////////////////////////////////////////////////////////////


#ifndef EATEXT_EATEXTGLYPHCACHE_GXM_H
#define EATEXT_EATEXTGLYPHCACHE_GXM_H


#ifndef EATEXT_EATEXTCACHE_H
	#include <EAText/EATextCache.h>
#endif


// Forward-declare
struct IDirect3DDevice9;

namespace EA
{
	
	namespace Text
	{
		class EATEXT_API GlyphCache_GXM : public GlyphCache
		{
		public:
			uintptr_t   CreateTexture(TextureInfo* pTextureInfo);
			bool        DestroyTexture(TextureInfo* pTextureInfo);

			bool        BeginUpdate(TextureInfo* pTextureInfo);
			bool        EndUpdate(TextureInfo* pTextureInfo);

		};

		// Gets the primary glyph cache.
		EATEXT_API GlyphCache_GXM* GetGlyphCache_GXM(bool bCreateIfNotFound = true);


		// Sets the primary glyph cache.
		EATEXT_API GlyphCache_GXM* SetGlyphCache_GXM(GlyphCache_GXM* pGlyphCache);

	} // namespace Text

} // namespace EA

#endif
