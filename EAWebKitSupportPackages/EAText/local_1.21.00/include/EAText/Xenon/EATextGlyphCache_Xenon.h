///////////////////////////////////////////////////////////////////////////////
// EATextGlyphCache_XENON.h
//
// Copyright (c) 2006 Electronic Arts. All Rights Reserved.
// By Alex Liberman, based on EATextCache_RenderWare.h by Paul Pedriana
//
///////////////////////////////////////////////////////////////////////////////

#ifndef EATEXT_EATEXTGLYPHCACHE_XENON_H
#define EATEXT_EATEXTGLYPHCACHE_XENON_H

#ifndef EATEXT_EATEXTCACHE_H
	#include <EAText/EATextCache.h>
#endif
#include <xtl.h>
#include <d3d9.h>

namespace EA
{
	
	namespace Text
	{
		/// GlyphCache_Xenon
		///
		/// Implements a Xenon-savvy glyph cache.
		///
		class EATEXT_API GlyphCache_Xenon : public GlyphCache
		{
		public:
			uintptr_t   CreateTexture(TextureInfo* pTextureInfo);
			bool        DestroyTexture(TextureInfo* pTextureInfo);

			bool        BeginUpdate(TextureInfo* pTextureInfo);
			bool        EndUpdate(TextureInfo* pTextureInfo);

			// Xenon Extension
			void SetDevice(IDirect3DDevice9* pDevice)
			{
				mpDevice = pDevice;
			}

		protected:
			// Graphics device pointer for texture ops
			IDirect3DDevice9* mpDevice;

		}; // class GlyphCache_Xenon

		/// GetGlyphCache_Xenon
		///
		/// Gets the primary glyph cache.
		///
		EATEXT_API GlyphCache_Xenon* GetGlyphCache_Xenon(bool bCreateIfNotFound = true);


		/// SetGlyphCache_Xenon
		///
		/// Sets the primary glyph cache.
		///
		EATEXT_API GlyphCache_Xenon* SetGlyphCache_Xenon(GlyphCache_Xenon* pGlyphCache);

	} // namespace Text

} // namespace EA

#endif // Header Sentry

