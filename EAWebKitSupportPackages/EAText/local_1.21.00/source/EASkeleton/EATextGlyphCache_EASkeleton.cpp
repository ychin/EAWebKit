///////////////////////////////////////////////////////////////////////////////
// Copyright (c) Electronic Arts. All Rights Reserved.
///////////////////////////////////////////////////////////////////////////////


#include <EAText/EASkeleton/EATextGlyphCache_EASkeleton.h>
#include <EAText/EATextFont.h>
#include <EAStdC/EAString.h>
#include <EASTL/algorithm.h>
#include <EAIO/EAFileStream.h>
#include <EASkeleton/Graphics.h>

namespace EA
{
	namespace Text
	{

		///////////////////////////////////////////////////////////////////////////////
		static GlyphCache_EASkeleton* gpGlyphCache_EASkeleton = NULL;

		///////////////////////////////////////////////////////////////////////////////
		EATEXT_API GlyphCache_EASkeleton* GetGlyphCache_EASkeleton(bool bCreateIfNotFound)
		{
			if(!gpGlyphCache_EASkeleton && bCreateIfNotFound)
			{
				static GlyphCache_EASkeleton sGlyphCache_EASkeleton;
				gpGlyphCache_EASkeleton = &sGlyphCache_EASkeleton;
			}
			return gpGlyphCache_EASkeleton;
		}

		///////////////////////////////////////////////////////////////////////////////
		EATEXT_API GlyphCache_EASkeleton* SetGlyphCache_EASkeleton(GlyphCache_EASkeleton* pGlyphCache)
		{
			GlyphCache_EASkeleton* const pSaved = gpGlyphCache_EASkeleton;
			gpGlyphCache_EASkeleton = pGlyphCache;
			return pSaved;
		}

		///////////////////////////////////////////////////////////////////////////////
		GlyphCache_EASkeleton::GlyphCache_EASkeleton()
		{
			SetOption( kOptionDefaultFormat, kTextureFormat32Bpp );
		}

		///////////////////////////////////////////////////////////////////////////////
		uintptr_t GlyphCache_EASkeleton::CreateTexture(TextureInfo* pTextureInfo)
		{
			EA_ASSERT_M(EA::Skeleton::Graphics::IsInitialized(), ("CreateTexture() failed: EA::Skeleton::Graphics has not been initialized yet"));

			EA::Skeleton::Graphics::TextureFormat format;

			if(mnTextureFormatDefault == kTextureFormat32Bpp)
			{
				pTextureInfo->mFormat = kTextureFormat32Bpp;
				format                = EA::Skeleton::Graphics::kR8G8B8A8;
			}
			else if(mnTextureFormatDefault == kTextureFormat8Bpp)
			{
				pTextureInfo->mFormat = kTextureFormat8Bpp;
				format                = EA::Skeleton::Graphics::kL8;
			}
			else
			{
				format                 = EA::Skeleton::Graphics::kR8G8B8A8; // To prevent compiler warning
				EA_FAIL();
			}

			EA::Skeleton::Graphics::Texture pTexture = EA::Skeleton::Graphics::CreateTexture(pTextureInfo->mpData, pTextureInfo->mnSize, pTextureInfo->mnSize, format);

			EA_ASSERT_M((pTexture != NULL), ("CreateTexture() failed"));

			return (uintptr_t)(void*)pTexture;
		}

		///////////////////////////////////////////////////////////////////////////////
		bool GlyphCache_EASkeleton::DestroyTexture(TextureInfo* pTextureInfo)
		{
			EA_ASSERT( pTextureInfo );

			if(pTextureInfo->mTexture)
			{
				EA::Skeleton::Graphics::Texture hTexture = (EA::Skeleton::Graphics::Texture)pTextureInfo->mTexture;
				EA::Skeleton::Graphics::DestroyTexture(hTexture);
			}

			return true;
		}

		///////////////////////////////////////////////////////////////////////////////
		bool GlyphCache_EASkeleton::BeginUpdate(TextureInfo* pTextureInfo)
		{
			#if EATEXT_THREAD_SAFETY_ENABLED
				EA::Thread::AutoFutex autoMutex(mMutex);
			#endif

			EA::Skeleton::Graphics::Texture pTexture = (EA::Skeleton::Graphics::Texture)(void*)pTextureInfo->mTexture;

			if(pTextureInfo->mpData == 0)
			{
				uint8_t* dataBuffer;
				size_t stride;
				if (EA::Skeleton::Graphics::LockTexture((void**)&dataBuffer, &stride, pTexture))
				{
					pTextureInfo->mnStride = static_cast<intptr_t>(stride);
					pTextureInfo->mpData = dataBuffer;

					#if EATEXT_THREAD_SAFETY_ENABLED
						mMutex.Lock(); // Will be matched by Unlock in EndUpdate.
					#endif

					return true;
				}
			}

			return false;
		}

		///////////////////////////////////////////////////////////////////////////////
		bool GlyphCache_EASkeleton::EndUpdate(TextureInfo* pTextureInfo)
		{
			EA_ASSERT_MESSAGE(pTextureInfo->mpData, "GlyphCache_EASkeleton::EndUpdate: texture is not locked.");

			#if EATEXT_THREAD_SAFETY_ENABLED
				EA::Thread::AutoFutex autoMutex(mMutex);
			#endif

			if(pTextureInfo->mpData) // If already locked...
			{
				pTextureInfo->mpData = NULL;

				EA::Skeleton::Graphics::Texture pTexture = (EA::Skeleton::Graphics::Texture)pTextureInfo->mTexture;
				EA::Skeleton::Graphics::UnlockTexture(pTexture);

				#if EATEXT_THREAD_SAFETY_ENABLED
					mMutex.Unlock(); // Matched by previous Lock in BeginUpdate.
				#endif

				return true;
			}

			return false;
		}
	} // namespace Text
} // namespace EA
