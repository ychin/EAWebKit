///////////////////////////////////////////////////////////////////////////////
// GlyphCache_GXM.cpp (PSVita platform)
//
// Copyright (c) 2011 Electronic Arts. All Rights Reserved.
///////////////////////////////////////////////////////////////////////////////


#include <EAText/GXM/EATextGlyphCache_GXM.h>
#include <EAText/EATextFont.h>
#include <EAStdC/EAString.h>
#include <gxm.h>
#include <gxm/texture.h>
#include EA_ASSERT_HEADER


// Helper macro to align a value
#define ALIGN(x, a) (((x) + ((a) - 1)) & ~((a) - 1))


namespace EA
{

	namespace Text
	{
		class DeviceTexture
		{
		public:
			SceGxmTexture gxmTexture;
			SceUID memoryUID;
			
			mutable const DeviceTexture *freeListNext;  // This pointer is used to create a free list of textures for deferred deletion.
		};

		static void *graphicsAlloc(SceKernelMemBlockType type, uint32_t size, uint32_t alignment, uint32_t attribs, SceUID *uid)
		{
			int err = SCE_OK;
			(void)(err);

			/*	Since we are using sceKernelAllocMemBlock directly, we cannot directly
				use the alignment parameter.  Instead, we must allocate the size to the
				minimum for this memblock type, and just assert that this will cover
				our desired alignment.

				Developers using their own heaps should be able to use the alignment
				parameter directly for more minimal padding.
			*/
			if (type == SCE_KERNEL_MEMBLOCK_TYPE_USER_CDRAM_RWDATA) {
				// CDRAM memblocks must be 256KiB aligned
				EA_ASSERT(alignment <= 256*1024);
				size = ALIGN(size, 256*1024);
			} else {
				// LPDDR memblocks must be 4KiB aligned
				EA_ASSERT(alignment <= 4*1024);
				size = ALIGN(size, 4*1024);
			}
			(void)(alignment);

			// allocate some memory
			*uid = sceKernelAllocMemBlock("basic", type, size, NULL);
			EA_ASSERT(*uid >= SCE_OK);

			// grab the base address
			void *mem = NULL;
			err = sceKernelGetMemBlockBase(*uid, &mem);
			EA_ASSERT(err == SCE_OK);

			// map for the GPU
			err = sceGxmMapMemory(mem, size, attribs);
			EA_ASSERT(err == SCE_OK);

			// done
			return mem;
		}

		static void graphicsFree(SceUID uid)
		{
			int err = SCE_OK;
			(void)(err);

			// grab the base address
			void *mem = NULL;
			err = sceKernelGetMemBlockBase(uid, &mem);
			EA_ASSERT(err == SCE_OK);

			// unmap memory
			err = sceGxmUnmapMemory(mem);
			EA_ASSERT(err == SCE_OK);

			// free the memory block
			err = sceKernelFreeMemBlock(uid);
			EA_ASSERT(err == SCE_OK);
		}

		static const DeviceTexture* CreateTextureARGB(unsigned int width, unsigned int height)
		{
			DeviceTexture* texture = new DeviceTexture();
			const size_t textureSizeInBytes = 4*width*height;
			
			void *textureData = graphicsAlloc(
				SCE_KERNEL_MEMBLOCK_TYPE_USER_RWDATA_UNCACHE,
				textureSizeInBytes,
				SCE_GXM_TEXTURE_ALIGNMENT,
				SCE_GXM_MEMORY_ATTRIB_READ,
				&texture->memoryUID);

			SceGxmTexture *gxmTexture = &texture->gxmTexture;
			int err = sceGxmTextureInitLinear(gxmTexture,
				textureData,
				SCE_GXM_TEXTURE_FORMAT_A8R8G8B8,
				width,
				height,
				1);
			EA_ASSERT(err == SCE_OK);
			sceGxmTextureSetMinFilter(gxmTexture, SCE_GXM_TEXTURE_FILTER_LINEAR);
			sceGxmTextureSetMagFilter(gxmTexture, SCE_GXM_TEXTURE_FILTER_LINEAR);
			sceGxmTextureSetMipFilter(gxmTexture, SCE_GXM_TEXTURE_MIP_FILTER_ENABLED);
			sceGxmTextureSetUAddrMode(gxmTexture, SCE_GXM_TEXTURE_ADDR_REPEAT);
			sceGxmTextureSetVAddrMode(gxmTexture, SCE_GXM_TEXTURE_ADDR_REPEAT);

			return texture;
		}

		static void DestroyTexture(const DeviceTexture* tex)
		{
			graphicsFree(tex->memoryUID);
			delete tex;
		}



		///////////////////////////////////////////////////////////////////////////////
		// GlyphCache_GXM
		//
		static GlyphCache_GXM* gpGlyphCache_GXM = NULL;

		EATEXT_API GlyphCache_GXM* GetGlyphCache_GXM(bool bCreateIfNotFound)
		{
			if(!gpGlyphCache_GXM && bCreateIfNotFound)
			{
				static GlyphCache_GXM sGlyphCache_GXM; // Possibly this should be startup-initialized to lower memory fragmentation. Usually this wouldn't contribute much or anything to fragmentation, though.
				gpGlyphCache_GXM = &sGlyphCache_GXM;
			}
			return gpGlyphCache_GXM;
		}

		///////////////////////////////////////////////////////////////////////////////
		// SetGlyphCache_GXM
		//
		EATEXT_API GlyphCache_GXM* SetGlyphCache_GXM(GlyphCache_GXM* pGlyphCache)
		{
			GlyphCache_GXM* const pSaved = gpGlyphCache_GXM;
			gpGlyphCache_GXM = pGlyphCache;
			return pSaved;
		}


		///////////////////////////////////////////////////////////////////////////////
		// CreateTexture
		//
		uintptr_t GlyphCache_GXM::CreateTexture(TextureInfo* pTextureInfo)
		{
			pTextureInfo->mFormat = kTextureFormatARGB;
			return (uintptr_t)(void*)CreateTextureARGB(pTextureInfo->mnSize, pTextureInfo->mnSize);
		}


		///////////////////////////////////////////////////////////////////////////////
		// DestroyTexture
		//
		bool GlyphCache_GXM::DestroyTexture(TextureInfo* pTextureInfo)
		{
			if(pTextureInfo->mTexture)
			{
				EA::Text::DestroyTexture((DeviceTexture*)pTextureInfo->mTexture);
			}
			return true;
		}


		///////////////////////////////////////////////////////////////////////////////
		// BeginUpdate
		//
		bool GlyphCache_GXM::BeginUpdate(TextureInfo* pTextureInfo)
		{
			#if EATEXT_THREAD_SAFETY_ENABLED
				EA::Thread::AutoFutex autoMutex(mMutex);
			#endif
			
			DeviceTexture* const texture = (DeviceTexture*)(void*)pTextureInfo->mTexture;
			
			pTextureInfo->mpData = (uint8_t*)sceGxmTextureGetData(&texture->gxmTexture);
			pTextureInfo->mnStride = pTextureInfo->mnSize * sizeof(unsigned int);
			#if EATEXT_THREAD_SAFETY_ENABLED
				mMutex.Lock(); // Will be matched by Unlock in EndUpdate.
			#endif

			return true;
		}


		///////////////////////////////////////////////////////////////////////////////
		// EndUpdate
		//
		bool GlyphCache_GXM::EndUpdate(TextureInfo* pTextureInfo)
		{
			EA_ASSERT_MESSAGE(pTextureInfo->mpData, "GlyphCache_GXM::EndUpdate: texture is not locked.");

			#if EATEXT_THREAD_SAFETY_ENABLED
				EA::Thread::AutoFutex autoMutex(mMutex);
			#endif

			pTextureInfo->mpData = NULL;
			
			#if EATEXT_THREAD_SAFETY_ENABLED
				mMutex.Unlock(); // Matched by previous Lock in BeginUpdate.
			#endif

			return true;
		}

	} // namespace Text

} // namespace EA
