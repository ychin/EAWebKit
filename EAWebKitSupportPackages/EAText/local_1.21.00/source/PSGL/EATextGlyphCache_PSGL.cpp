///////////////////////////////////////////////////////////////////////////////
// GlyphCache_PSGL.cpp
//
// Copyright (c) 2006 Electronic Arts. All Rights Reserved.
// By Alex Liberman, based on EATextCache_RenderWare.cpp by Paul Pedriana
//
///////////////////////////////////////////////////////////////////////////////


#include <EASTL/algorithm.h>
#include <EAText/PSGL/EATextGlyphCache_PSGL.h>
#include <EAText/EATextFont.h>
#include <EAStdC/EAString.h>
#include <EAIO/EAFileStream.h>
#include EA_ASSERT_HEADER
#include <GLES/gl.h>
#include <PSGL/psgl.h>
#include <math.h>

namespace EA
{
	namespace Text
	{

		///////////////////////////////////////////////////////////////////////////////
		// GlyphCache_PSGL
		//
		static GlyphCache_PSGL* gpGlyphCache_PSGL = NULL;

		EATEXT_API GlyphCache_PSGL* GetGlyphCache_PSGL(bool bCreateIfNotFound)
		{
			if(!gpGlyphCache_PSGL && bCreateIfNotFound)
			{
				static GlyphCache_PSGL sGlyphCache_PSGL; // Possibly this should be startup-initialized to lower memory fragmentation. Usually this wouldn't contribute much or anything to fragmentation, though.
				gpGlyphCache_PSGL = &sGlyphCache_PSGL;
			}
			return gpGlyphCache_PSGL;
		}

		///////////////////////////////////////////////////////////////////////////////
		// SetGlyphCache_PSGL
		//
		EATEXT_API GlyphCache_PSGL* SetGlyphCache_PSGL(GlyphCache_PSGL* pGlyphCache)
		{
			GlyphCache_PSGL* const pSaved = gpGlyphCache_PSGL;
			gpGlyphCache_PSGL = pGlyphCache;
			return pSaved;
		}

		///////////////////////////////////////////////////////////////////////////////
		// CreateTexture
		//
		uintptr_t GlyphCache_PSGL::CreateTexture(TextureInfo* pTextureInfo)
		{
			// Create the texture. Note that pTextureInfo->mpSource points to a file path
			// (or some other kind of asset moniker) and the CreateTexture function should 
			// pay attention to that and load that texture instead of creating a new one 
			// from nothing. User-provided subclasses of GlyphCache can customize the behaviour.
			// As of this writing we haven't yet written support for loading from mpSource.
			// See the EATextRNA package's GlyphCache_RNA::CreateTexture function for an example. 

			pTextureInfo->mFormat = kTextureFormatDXT3;

			uint32_t texSize = pTextureInfo->mnSize;
			uint32_t blocksPerSide = static_cast<uint32_t>(ceil(texSize / 4.0f));

			// Allocate system RAM for mirroring texture data (for future texture writes)
			uint32_t texDataSize = blocksPerSide * blocksPerSide * 16;   // (16 bytes per DXT3 block's row)

			// PSGL Programming guide recommends 16-byte alignment
			uint32_t* const pTextureData = (uint32_t*)mpCoreAllocator->Alloc(texDataSize, EATEXT_ALLOC_PREFIX "Cache/Texture", 0, 16, 0);
			EA_ASSERT_M(pTextureData != NULL, "GlyphCache_PSGL::CreateTexture(): EA_MALLOC_ALIGNED failed");

			// Obtain a texture handle
			GLuint texId;
			glGenTextures(1, &texId);

			// Internal struct for tracking GL texID and system memory associated with this texture
			PSGLTextureInfo* const pTexInfoStruct = (PSGLTextureInfo*)mpCoreAllocator->Alloc(sizeof(PSGLTextureInfo), EATEXT_ALLOC_PREFIX "Cache/TextureStruct", 0);
			pTexInfoStruct->mTextureID      = texId;
			pTexInfoStruct->mpTextureData   = pTextureData;
			
			return (uintptr_t)(void*)pTexInfoStruct;
		}


		///////////////////////////////////////////////////////////////////////////////
		// DestroyTexture
		//
		bool GlyphCache_PSGL::DestroyTexture(TextureInfo* pTextureInfo)
		{
			if(pTextureInfo->mTexture)
			{
				PSGLTextureInfo* const pTexInfoStruct = (PSGLTextureInfo*)(void*)pTextureInfo->mTexture;

				// Free RSX memory associated with the texture
				glDeleteTextures(1, (GLuint*)&(pTexInfoStruct->mTextureID));

				// Free the mirrored texture data in system RAM
				mpCoreAllocator->Free(pTexInfoStruct->mpTextureData);

				// Free the GlyphCace_PSGL texture wrapper
				mpCoreAllocator->Free(pTexInfoStruct);
			}
			return true;
		}


		///////////////////////////////////////////////////////////////////////////////
		// BeginUpdate
		//
		bool GlyphCache_PSGL::BeginUpdate(TextureInfo* pTextureInfo)
		{
			#if EATEXT_THREAD_SAFETY_ENABLED
				EA::Thread::AutoFutex autoMutex(mMutex);
			#endif

			if(!pTextureInfo->mpData) // If not already locked... (to consider: Allow BeginUpdate calls to be additive).
			{
				PSGLTextureInfo* const pTexInfoStruct = (PSGLTextureInfo*)(void*)pTextureInfo->mTexture;
				pTextureInfo->mpData    = (uint8_t*)pTexInfoStruct->mpTextureData;
				pTextureInfo->mnStride  = static_cast<intptr_t>(ceil(pTextureInfo->mnSize / 4.0f) * 16);   // (16 bytes per 4x4 DXT3 block's row)

				#if EATEXT_THREAD_SAFETY_ENABLED
					mMutex.Lock(); // Will be matched by Unlock in EndUpdate.
				#endif

				return true;
			}
			return false;
		}


		///////////////////////////////////////////////////////////////////////////////
		// EndUpdate
		//
		bool GlyphCache_PSGL::EndUpdate(TextureInfo* pTextureInfo)
		{
			EA_ASSERT_M(pTextureInfo->mpData, "GlyphCache_PSGL::EndUpdate: texture is not locked.");

			#if EATEXT_THREAD_SAFETY_ENABLED
				EA::Thread::AutoFutex autoMutex(mMutex);
			#endif

			if(pTextureInfo->mpData) // If already locked...
			{
				const PSGLTextureInfo* const pTexInfoStruct = (PSGLTextureInfo*)(void*)pTextureInfo->mTexture;

				uint32_t texSize = pTextureInfo->mnSize;
				uint32_t blocksPerSide = static_cast<uint32_t>(ceil(texSize / 4.0f));
				uint32_t texDataSize = blocksPerSide * blocksPerSide * 16;   // (16 bytes per DXT3 block's row)

				// Bind texture which we're about to overwrite
				glBindTexture(GL_TEXTURE_2D, pTexInfoStruct->mTextureID);

				// Need to set initial texture filter modes
				glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
				glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
				
				glCompressedTexImage2D(
					GL_TEXTURE_2D,                      // Target
					0,                                  // Disable MIP-mapping
					GL_COMPRESSED_RGBA_S3TC_DXT3_EXT,   // Internal texture format
					texSize,                            // Width    (can be non-power-of-2)
					texSize,                            // Height   (can be non-power-of-2)
					0,                                  // No Border   (PS3 does not support borders)
					texDataSize,                        // Image size in bytes
					pTextureInfo->mpData                // Texture data (equivalent to pTexInfoStruct->mpTextureData)
					);
				GLenum errorCode = glGetError();
				EA_ASSERT_M((errorCode != GL_INVALID_ENUM && errorCode != GL_INVALID_VALUE), "EATextGlyphCache_PSGL::EndUpdate(): glCompressedTexImage2D failed");
				(void)errorCode;

				// Ready to lock again if necessary
				pTextureInfo->mpData = NULL;

				#if EATEXT_THREAD_SAFETY_ENABLED
					mMutex.Unlock(); // Matched by previous Lock in BeginUpdate.
				#endif

				return true;
			}

			return false;
		}

	} // namespace Text
} // namespace EA
