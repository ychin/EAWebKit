///////////////////////////////////////////////////////////////////////////////
// GlyphCache_Xenon.cpp
//
// Copyright (c) 2006 Electronic Arts. All Rights Reserved.
// By Alex Liberman, based on EATextCache_RenderWare.cpp by Paul Pedriana
//
///////////////////////////////////////////////////////////////////////////////


#include <EASTL/algorithm.h>
#include <EAText/Xenon/EATextGlyphCache_Xenon.h>
#include <EAText/EATextFont.h>
#include <EAStdC/EAString.h>
#include <EAIO/EAFileStream.h>
#include EA_ASSERT_HEADER
#include <xtl.h>
#include <d3d9.h>

namespace EA
{

	namespace Text
	{

		///////////////////////////////////////////////////////////////////////////////
		// GlyphCache_Xenon
		//
		static GlyphCache_Xenon* gpGlyphCache_Xenon = NULL;

		EATEXT_API GlyphCache_Xenon* GetGlyphCache_Xenon(bool bCreateIfNotFound)
		{
			if(!gpGlyphCache_Xenon && bCreateIfNotFound)
			{
				static GlyphCache_Xenon sGlyphCache_Xenon; // Possibly this should be startup-initialized to lower memory fragmentation. Usually this wouldn't contribute much or anything to fragmentation, though.
				gpGlyphCache_Xenon = &sGlyphCache_Xenon;
			}
			return gpGlyphCache_Xenon;
		}

		///////////////////////////////////////////////////////////////////////////////
		// SetGlyphCache_Xenon
		//
		EATEXT_API GlyphCache_Xenon* SetGlyphCache_Xenon(GlyphCache_Xenon* pGlyphCache)
		{
			GlyphCache_Xenon* const pSaved = gpGlyphCache_Xenon;
			gpGlyphCache_Xenon = pGlyphCache;
			return pSaved;
		}

		///////////////////////////////////////////////////////////////////////////////
		// CreateTexture
		//
		uintptr_t GlyphCache_Xenon::CreateTexture(TextureInfo* pTextureInfo)
		{
			// Create the texture. Note that pTextureInfo->mpSource points to a file path
			// (or some other kind of asset moniker) and the CreateTexture function should 
			// pay attention to that and load that texture instead of creating a new one 
			// from nothing. User-provided subclasses of GlyphCache can customize the behaviour.
			// As of this writing we haven't yet written support for loading from mpSource.
			// See the EATextRNA package's GlyphCache_RNA::CreateTexture function for an example. 

			EA_ASSERT(mpDevice);

			pTextureInfo->mFormat = kTextureFormatDXT3;

			IDirect3DTexture9* pTexture = NULL;
			mpDevice->CreateTexture(pTextureInfo->mnSize, pTextureInfo->mnSize, 1, 0, D3DFMT_LIN_DXT3, 0, &pTexture, NULL);

			EA_ASSERT_M((pTexture != NULL), ("CreateTexture() failed"));

			return (uintptr_t)(void*)pTexture;
		}


		///////////////////////////////////////////////////////////////////////////////
		// DestroyTexture
		//
		bool GlyphCache_Xenon::DestroyTexture(TextureInfo* pTextureInfo)
		{
			if(pTextureInfo->mTexture)
			{
				IDirect3DTexture9* pTexture = (IDirect3DTexture9*)(void*)pTextureInfo->mTexture;
				pTexture->Release();
			}
			return true;
		}


		///////////////////////////////////////////////////////////////////////////////
		// BeginUpdate
		//
		bool GlyphCache_Xenon::BeginUpdate(TextureInfo* pTextureInfo)
		{
			#if EATEXT_THREAD_SAFETY_ENABLED
				EA::Thread::AutoFutex autoMutex(mMutex);
			#endif

			EA_COMPILETIME_ASSERT(sizeof(pTextureInfo->mLockInformation) >= sizeof(D3DLOCKED_RECT));

			IDirect3DTexture9* const pTexture = (IDirect3DTexture9*)(void*)pTextureInfo->mTexture;
			D3DLOCKED_RECT*    const pLocked  = (D3DLOCKED_RECT*)(void*)pTextureInfo->mLockInformation;

			if(!pTextureInfo->mpData) // If not already locked... (to consider: Allow BeginUpdate calls to be additive).
			{
				if (SUCCEEDED(pTexture->LockRect(0, pLocked, NULL, NULL)))
				{
					pTextureInfo->mpData   = static_cast<uint8_t*>(pLocked->pBits);
					pTextureInfo->mnStride = static_cast<uint32_t>(pLocked->Pitch);

					#if EATEXT_THREAD_SAFETY_ENABLED
						mMutex.Lock(); // Will be matched by Unlock in EndUpdate.
					#endif

					return true;
				}
			}

			return false;
		}


		///////////////////////////////////////////////////////////////////////////////
		// EndUpdate
		//
		bool GlyphCache_Xenon::EndUpdate(TextureInfo* pTextureInfo)
		{
			EA_ASSERT_MESSAGE(pTextureInfo->mpData, "GlyphCache_Xenon::EndUpdate: texture is not locked.");

			#if EATEXT_THREAD_SAFETY_ENABLED
				EA::Thread::AutoFutex autoMutex(mMutex);
			#endif

			if(pTextureInfo->mpData) // If already locked...
			{
				pTextureInfo->mpData = NULL;

				IDirect3DTexture9* const pTexture = (IDirect3DTexture9*)(void*)pTextureInfo->mTexture;
				pTexture->UnlockRect(0);   // Unlock top-most MIP map level

				#if EATEXT_THREAD_SAFETY_ENABLED
					mMutex.Unlock(); // Matched by previous Lock in BeginUpdate.
				#endif

				return true;
			}

			return false;
		}

	} // namespace Text
} // namespace EA
