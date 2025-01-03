///////////////////////////////////////////////////////////////////////////////
// EATextGlyphCache_D3D.h
//
// Copyright (c) 2006 Electronic Arts. All Rights Reserved.
// By Alex Liberman, based on EATextCache_RenderWare.h by Paul Pedriana
//
///////////////////////////////////////////////////////////////////////////////

#ifndef EATEXT_EATEXTGLYPHCACHE_D3D_H
#define EATEXT_EATEXTGLYPHCACHE_D3D_H

#ifndef EATEXT_EATEXTCACHE_H
	#include <EAText/EATextCache.h>
#endif

// Forward-declare
struct IDirect3DDevice9;

namespace EA
{
	
	namespace Text
	{
		/// GlyphCache_D3D
		///
		/// Implements a D3D-savvy glyph cache.
		///
		class EATEXT_API GlyphCache_D3D : public GlyphCache
		{
		public:
			uintptr_t   CreateTexture(TextureInfo* pTextureInfo);
			bool        DestroyTexture(TextureInfo* pTextureInfo);

			bool        BeginUpdate(TextureInfo* pTextureInfo);
			bool        EndUpdate(TextureInfo* pTextureInfo);

			// D3D Extension
			void SetDevice(IDirect3DDevice9* pDevice)
			{
				mpDevice = pDevice;
			}

		protected:
			// Graphics device pointer for texture ops
			IDirect3DDevice9* mpDevice;

		}; // class GlyphCache_D3D

		/// GetGlyphCache_D3D
		///
		/// Gets the primary glyph cache.
		///
		EATEXT_API GlyphCache_D3D* GetGlyphCache_D3D(bool bCreateIfNotFound = true);


		/// SetGlyphCache_D3D
		///
		/// Sets the primary glyph cache.
		///
		EATEXT_API GlyphCache_D3D* SetGlyphCache_D3D(GlyphCache_D3D* pGlyphCache);

	} // namespace Text

} // namespace EA

#endif // Header Sentry
