///////////////////////////////////////////////////////////////////////////////
// EATextGlyphCache_RNA.h
//
// Copyright (c) 2007 Electronic Arts. All Rights Reserved.
// By Alex Liberman, based on EATextCache_RenderWare.h by Paul Pedriana
//
///////////////////////////////////////////////////////////////////////////////

#ifndef EATEXT_EATEXTGLYPHCACHE_RNA_H
#define EATEXT_EATEXTGLYPHCACHE_RNA_H

#ifndef EATEXT_EATEXTCACHE_H
	#include <EAText/EATextCache.h>
#endif

namespace RNA
{
	// Forward declare RNA device
	class DeviceC;
}

namespace EA
{
	
	namespace Text
	{
		/// GlyphCache_RNA
		///
		/// Implements a RNA-savvy glyph cache.
		///
		class EATEXT_API GlyphCache_RNA : public GlyphCache
		{
		public:
			uintptr_t   CreateTexture(TextureInfo* pTextureInfo);
			bool        DestroyTexture(TextureInfo* pTextureInfo);

			bool        BeginUpdate(TextureInfo* pTextureInfo);
			bool        EndUpdate(TextureInfo* pTextureInfo);

			// RNA Extension
			void SetDevice(RNA::DeviceC* pDevice)
			{
				mpDevice = pDevice;
			}

		protected:
			// Graphics device pointer for texture ops
			RNA::DeviceC*   mpDevice;

		}; // class GlyphCache_RNA

		/// GetGlyphCache_RNA
		///
		/// Gets the primary glyph cache.
		///
		EATEXT_API GlyphCache_RNA* GetGlyphCache_RNA(bool bCreateIfNotFound = true);


		/// SetGlyphCache_RNA
		///
		/// Sets the primary glyph cache.
		///
		EATEXT_API GlyphCache_RNA* SetGlyphCache_RNA(GlyphCache_RNA* pGlyphCache);

	} // namespace Text

} // namespace EA

#endif // Header Sentry
