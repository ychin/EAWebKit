///////////////////////////////////////////////////////////////////////////////
// GlyphCache_OPENGL_ES.cpp
//
// Copyright (c) 2010 Electronic Arts. All Rights Reserved.
//
///////////////////////////////////////////////////////////////////////////////


#include <EASTL/algorithm.h>
#include <EAText/OpenGL_ES/EATextGlyphCache_OpenGL_ES.h>
#include <EAText/EATextFont.h>
//#include <EAStdC/EAString.h>
#include <EAIO/EAFileStream.h>
#include EA_ASSERT_HEADER
//#include <GLES/gl.h>      // For OpenGL ES 1.x 
//#include <GLES/gl2.h>     // For OpenGL ES 2.x


// ** Note **
// As of this writing in July 2011, this code is not tested nor is there a demo application for it.
// Feel free to ask the maintainer of this package if you need it. In the meantime it serves as a 
// reference but may have a bug or two.

namespace EA
{
	namespace Text
	{

		///////////////////////////////////////////////////////////////////////////////
		// GlyphCache_OPENGL_ES
		//
		static GlyphCache_OPENGL_ES* gpGlyphCache_OPENGL_ES = NULL;

		EATEXT_API GlyphCache_OPENGL_ES* GetGlyphCache_OpenGL_ES(bool bCreateIfNotFound)
		{
			if(!gpGlyphCache_OPENGL_ES && bCreateIfNotFound)
			{
				static GlyphCache_OPENGL_ES sGlyphCache_OpenGL_ES; // Possibly this should be startup-initialized to lower memory fragmentation. Usually this wouldn't contribute much or anything to fragmentation, though.
				gpGlyphCache_OPENGL_ES = &sGlyphCache_OpenGL_ES;
			}
			return gpGlyphCache_OPENGL_ES;
		}


		///////////////////////////////////////////////////////////////////////////////
		// SetGlyphCache_OPENGL_ES
		//
		EATEXT_API GlyphCache_OPENGL_ES* SetGlyphCache_OpenGL_ES(GlyphCache_OPENGL_ES* pGlyphCache)
		{
			GlyphCache_OPENGL_ES* const pSaved = gpGlyphCache_OPENGL_ES;
			gpGlyphCache_OPENGL_ES = pGlyphCache;
			return pSaved;
		}


		///////////////////////////////////////////////////////////////////////////////
		// CreateTexture
		//
		uintptr_t GlyphCache_OPENGL_ES::CreateTexture(TextureInfo* /*pTextureInfo*/)
		{
			/*
			// To do: We need to change this to support kTextureFormat8Bpp, kTextureFormatDXT3, 
			//        kTextureFormatRGBA. DXT isn't necessarily supported by OpenGL ES implementations.
			pTextureInfo->mFormat = kTextureFormatARGB;

			GLuint texId = 0;
			glGenTextures(1, &texId);

			// Internal struct for tracking GL texID and system memory associated with this texture
			GLESTextureInfo* const pTexInfoStruct = (GLESTextureInfo*)mpCoreAllocator->Alloc(sizeof(GLESTextureInfo), EATEXT_ALLOC_PREFIX "Cache/TextureStruct", 0);
			pTexInfoStruct->mTextureID    = texId;
			pTexInfoStruct->mpTextureData = NULL;

			return (uintptr_t)(void*)pTexInfoStruct;
			*/

			return 0;
		}


		///////////////////////////////////////////////////////////////////////////////
		// DestroyTexture
		//
		bool GlyphCache_OPENGL_ES::DestroyTexture(TextureInfo*  /*pTextureInfo*/)
		{
			/*
			if(pTextureInfo->mTexture)
			{
				GLESTextureInfo* const pTexInfoStruct = (GLESTextureInfo*)(void*)pTextureInfo->mTexture;

				glDeleteTextures(1, (GLuint*)&(pTexInfoStruct->mTextureID));
				mpCoreAllocator->Free(pTexInfoStruct);
			}
			*/

			return true;
		}


		///////////////////////////////////////////////////////////////////////////////
		// BeginUpdate
		//
		bool GlyphCache_OPENGL_ES::BeginUpdate(TextureInfo*  /*pTextureInfo*/)
		{
			/*
			#if EATEXT_THREAD_SAFETY_ENABLED
				EA::Thread::AutoFutex autoMutex(mMutex);
			#endif

			if(!pTextureInfo->mpData) // If not already locked... (to consider: Allow BeginUpdate calls to be additive).
			{
				GLESTextureInfo* const pTexInfoStruct = (GLESTextureInfo*)(void*)pTextureInfo->mTexture;

				pTextureInfo->mpData    = (uint8_t*)pTexInfoStruct->mpTextureData;
				pTextureInfo->mnStride  = pTextureInfo->mnSize * 4; // This might be wrong.

				#if EATEXT_THREAD_SAFETY_ENABLED
					mMutex.Lock(); // Will be matched by Unlock in EndUpdate.
				#endif

				return true;
			}
			*/

			return false;
		}


		///////////////////////////////////////////////////////////////////////////////
		// EndUpdate
		//
		bool GlyphCache_OPENGL_ES::EndUpdate(TextureInfo*  /*pTextureInfo*/)
		{
			/*
			EA_ASSERT_M(pTextureInfo->mpData, "GlyphCache_OPENGL_ES::EndUpdate: texture is not locked; it should have been locked by a preceding call to BeginUpdate.");

			#if EATEXT_THREAD_SAFETY_ENABLED
				EA::Thread::AutoFutex autoMutex(mMutex);
			#endif

			if(pTextureInfo->mpData) // If already locked...
			{
				bool mbPointSample = false; // To consider: Make this a member variable.

				const GLESTextureInfo* const pTexInfoStruct = (GLESTextureInfo*)(void*)pTextureInfo->mTexture;

				glBindTexture(GL_TEXTURE_2D, pTexInfoStruct->mTextureID);
				//glTexEnvx(GL_TEXTURE_ENV, GL_TEXTURE_ENV_MODE, GL_MODULATE); // EAMobile did this, though I think a call like this belongs in the draw code and not here.
				glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, mbPointSample ? GL_NEAREST : GL_LINEAR);
				glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, mbPointSample ? GL_NEAREST : GL_LINEAR);

				// Load texture into OpenGL
				glTexImage2D(
					GL_TEXTURE_2D,          // Target
					0,                      // Disable MIP mapping
					GL_RGBA,                // Internal texture format
					pTextureInfo->mnSize,   // Width
					pTextureInfo->mnSize,   // Height
					0,                      // No Border (PS3 does not support borders)
					GL_RGBA,                // Format
					GL_UNSIGNED_BYTE,       // Type
					pTextureInfo->mpData);

				// Ready to lock again if necessary
				pTextureInfo->mpData = NULL;

				#if EATEXT_THREAD_SAFETY_ENABLED
					mMutex.Unlock(); // Matched by previous Lock in BeginUpdate.
				#endif

				return true;
			}
			*/

			return false;
		}

	} // namespace Text
} // namespace EA
