///////////////////////////////////////////////////////////////////////////////
// GlyphCache_RNA.cpp
//
// Copyright (c) 2007 Electronic Arts. All Rights Reserved.
// By Alex Liberman, based on EATextCache_RenderWare.cpp by Paul Pedriana
//
///////////////////////////////////////////////////////////////////////////////


#include <EASTL/algorithm.h>
#include <EAText/RNA/EATextGlyphCache_RNA.h>
#include <EAText/EATextFont.h>
#include <EAText/internal/StdC.h>
#include <EAIO/EAFileStream.h>
#include <eathread/eathread_futex.h>
#include EA_ASSERT_HEADER

#include <RNA/RNA.h>

#if defined(EA_PLATFORM_WINDOWS)
	#include <d3d9.h>
#endif

namespace EA
{

	namespace Text
	{

		///////////////////////////////////////////////////////////////////////////////
		// GlyphCache_RNA
		//
		static GlyphCache_RNA* gpGlyphCache_RNA = NULL;

		EATEXT_API GlyphCache_RNA* GetGlyphCache_RNA(bool bCreateIfNotFound)
		{
			if(!gpGlyphCache_RNA && bCreateIfNotFound)
			{
				static GlyphCache_RNA sGlyphCache_RNA; // Possibly this should be startup-initialized to lower memory fragmentation. Usually this wouldn't contribute much or anything to fragmentation, though.
				gpGlyphCache_RNA = &sGlyphCache_RNA;
			}
			return gpGlyphCache_RNA;
		}

		///////////////////////////////////////////////////////////////////////////////
		// SetGlyphCache_RNA
		//
		EATEXT_API GlyphCache_RNA* SetGlyphCache_RNA(GlyphCache_RNA* pGlyphCache)
		{
			GlyphCache_RNA* const pSaved = gpGlyphCache_RNA;
			gpGlyphCache_RNA = pGlyphCache;
			return pSaved;
		}

		///////////////////////////////////////////////////////////////////////////////
		// CreateTexture
		//
		uintptr_t GlyphCache_RNA::CreateTexture(TextureInfo* pTextureInfo)
		{
			// Create the texture. Note that pTextureInfo->mpSource points to a file path
			// (or some other kind of asset moniker) and the CreateTexture function should 
			// pay attention to that and load that texture instead of creating a new one 
			// from nothing. User-provided subclasses of GlyphCache can customize the behaviour.
			// As of this writing we haven't yet written support for loading from mpSource.
			// See the EATextRNA package's GlyphCache_RNA::CreateTexture function for an example. 

			EA_ASSERT(mpDevice);

			pTextureInfo->mFormat = kTextureFormatDXT3;

			RNA::TexturePlatC* pTexture = RNA_NEW(EATEXT_ALLOC_PREFIX "TexturePlatC") RNA::TexturePlatC();
			EA_ASSERT_M((pTexture != NULL), ("GlyphCache_RNA::CreateTexture(): RNA CreateTexture failed"));
			pTexture->AddRef();

			bool texCreated = pTexture->Create(
				mpDevice, 
				RNA::TEXTURE_TYPE_2D, 
				pTextureInfo->mnSize, 
				pTextureInfo->mnSize,
				1,                              // 1 depth level
			#if defined(EA_PLATFORM_XENON)
				RNA::SURFACE_FORMAT_LIN_DXT3,   // Don't use tiled texture format for the glyph cache
			#else
				RNA::SURFACE_FORMAT_DXT3,
			#endif
				1,                              // Number of layers?
				1,                              // MIP-map levels (don't create any)
				RNA::SURFACE_FLAG_COLOR | RNA::SURFACE_FLAG_ALPHA | RNA::SURFACE_FLAG_MANAGED   // Only D3DPOOL_MANAGED textures can be Lock()ed (Ignored on Xenon and PS3) // TODO: Ideally this should be a pair of textures in D3DPOOL_DEFAULT and D3DPOOL_SYSTEMMEM
				);

			(void)texCreated;
			EA_ASSERT_M(texCreated, ("GlyphCache_RNA::CreateTexture(): RNA::Texture::Create failed"));

			return (uintptr_t)(void*)pTexture;
		}


		///////////////////////////////////////////////////////////////////////////////
		// DestroyTexture
		//
		bool GlyphCache_RNA::DestroyTexture(TextureInfo* pTextureInfo)
		{
			if(pTextureInfo->mTexture)
			{
				RNA::TexturePlatC* pTexture = (RNA::TexturePlatC*)(void*)pTextureInfo->mTexture;

				EA_ASSERT(pTexture);

				pTexture->Release();
			}
			return true;
		}


		///////////////////////////////////////////////////////////////////////////////
		// BeginUpdate
		//
		bool GlyphCache_RNA::BeginUpdate(TextureInfo* pTextureInfo)
		{
			#if EATEXT_THREAD_SAFETY_ENABLED
				EA::Thread::AutoFutex autoFutex(mMutex);
			#endif

			if (!pTextureInfo->mpData) // If not already locked... (to consider: Allow BeginUpdate calls to be additive).
			{
				RNA::TexturePlatC*  const pTexture = (RNA::TexturePlatC*)(void*)pTextureInfo->mTexture;
				
				EA_ASSERT_M(pTexture, ("GlyphCache_RNA::BeginUpdate: Cache texture not allocated."));
				//EA_COMPILETIME_ASSERT(sizeof(pTextureInfo->mLockInformation) >= sizeof(D3DLOCKED_RECT));

				RNA::SurfaceC** ppSurface = reinterpret_cast<RNA::SurfaceC**>(&pTextureInfo->mLockInformation);
				*ppSurface = pTexture->AcquireSurface(mpDevice, 0, 0);

				uint32_t pitch(0);
				uint32_t flags(0);

				pTextureInfo->mpData    = (uint8_t*)(*ppSurface)->Lock(pitch, flags);
				pTextureInfo->mnStride  = (intptr_t)pitch;
			#if defined(EA_PLATFORM_PS3)
				// GCM has a convention whereby DXTn textures have a pitch of 0.
				// We explicitly query RNA for the texture's pitch.
				pTextureInfo->mnStride  = (intptr_t)RNA::SurfaceFormatGetPitch(pTexture->GetFormat(), pTexture->GetWidth());
			#endif

			#if EATEXT_THREAD_SAFETY_ENABLED
				mMutex.Lock(); // Will be matched by Unlock in EndUpdate.
			#endif

				if(pTextureInfo->mpData != NULL)
					return true;
			}

			return false;
		}


		///////////////////////////////////////////////////////////////////////////////
		// EndUpdate
		//
		bool GlyphCache_RNA::EndUpdate(TextureInfo* pTextureInfo)
		{
			EA_ASSERT_MESSAGE(pTextureInfo->mpData, "GlyphCache_RNA::EndUpdate: texture is not locked.");

			#if EATEXT_THREAD_SAFETY_ENABLED
				EA::Thread::AutoFutex autoFutex(mMutex);
			#endif

			if(pTextureInfo->mpData) // If already locked...
			{
				RNA::TexturePlatC* const    pTexture = (RNA::TexturePlatC*)(void*)pTextureInfo->mTexture;
				RNA::SurfaceC*     const    pSurface = *(reinterpret_cast<RNA::SurfaceC**>(pTextureInfo->mLockInformation));
				
				pSurface->Unlock(pTextureInfo->mpData);
				pTextureInfo->mpData = NULL;

				pTexture->ReleaseSurface(mpDevice, pSurface);

				#if EATEXT_THREAD_SAFETY_ENABLED
					mMutex.Unlock(); // Matched by previous Lock in BeginUpdate.
				#endif

				return true;
			}

			return false;
		}

	} // namespace Text
} // namespace EA
