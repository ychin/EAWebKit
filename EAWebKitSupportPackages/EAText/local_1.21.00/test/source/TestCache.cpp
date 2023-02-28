/////////////////////////////////////////////////////////////////////////////
// TestCache.cpp
//
// Copyright (c) 2004, Electronic Arts Inc. All rights reserved.
// Created by Paul Pedriana
//
/////////////////////////////////////////////////////////////////////////////


#include "EATextTest.h"
#include <EAText/EATextCache.h>
#include <EAText/EATextFont.h>
#include <EAText/EATextFontServer.h>
#include <EAText/EATextTypesetter.h>
#include <EASTL/hash_set.h>
#include <EASTL/set.h>
#include <EAStdC/EAMemory.h>
#include <EAStdC/EAString.h>
#include <EATest/EATest.h>
#include <eathread/eathread_thread.h>
#include <eathread/eathread_atomic.h>
#include <EAIO/EAFileUtil.h>
#include <EAIO/PathString.h>
#include <EAIO/EAStreamMemory.h>
#include <MemoryMan/MemoryMan.h>
#include <string.h>

#if defined (EA_PLATFORM_IPHONE)
	namespace EA{ namespace IO {
		int appleGetHomeDirectory(char8_t* pDirectory, uint32_t nPathCapacity);
		int appleGetAppName(char8_t* pDirectory, uint32_t nPathCapacity);
	}}
#endif

using namespace EA::Text;

namespace TestCacheInternal
{
	// Here we #include an embedded TrueType font which is located inside 
	// a .inl file and which is declared as a C char array like this:
	//   const uint8_t pPalormSmallCapsTTF[] = { 0x23, 0xa2, ... }; 
	#include "data/ParolmSmallCaps.ttf.inl"
}

// Allocator used for TestCacheAllocator
class MallocAllocator : public EA::Allocator::ICoreAllocator
{
public:
	MallocAllocator(const char* = EASTL_NAME_VAL("MallocAllocator")) : mAllocCount(0), mFreeCount(0), mAllocVolume(0)
		{ }

	MallocAllocator(const MallocAllocator& x)
		: mAllocCount(x.mAllocCount), mFreeCount(x.mFreeCount), mAllocVolume(x.mAllocVolume)
		{ }

	MallocAllocator(const MallocAllocator&, const char*)
		{ }

	MallocAllocator& operator=(const MallocAllocator& x)
		{ mAllocCount = x.mAllocCount; mFreeCount = x.mFreeCount; mAllocVolume = x.mAllocVolume; return *this; }

	void* allocate(size_t n, int = 0)
	{ 
		++mAllocCount; 
		mAllocVolume += n; 
		++mAllocCountAll; 
		mAllocVolumeAll += n; 
		mpLastAllocation = malloc(n); 

		return mpLastAllocation;
	}

	// We don't support alignment, so you can't use this class where alignment is required.
	void* allocate(size_t n, size_t, size_t, int = 0) 
	{ 
		++mAllocCount; 
		mAllocVolume += n;
		++mAllocCountAll; 
		mAllocVolumeAll += n; 
		mpLastAllocation = malloc(n); 

		return mpLastAllocation; 
	}

	void  deallocate(void* p, size_t n)
	{ 
		++mFreeCount;
		mAllocVolume -= n;
		++mFreeCountAll;
		mAllocVolumeAll -= n;

		return free(p);
	}

	const char* get_name() const
		{ return "MallocAllocator"; }

	void set_name(const char*)
		{ }

	static void reset_all()
		{ mAllocCountAll = 0; mFreeCountAll = 0; mAllocVolumeAll = 0; mpLastAllocation = NULL; }

	// ICoreAllocator Functions
	virtual void *Alloc(size_t size, const char*, unsigned int flags) { return allocate(size, flags); }

    virtual void *Alloc(size_t size, const char*, unsigned int flags,
		unsigned int, unsigned int = 0) { return allocate(size, flags); }

	virtual void Free(void *block, size_t size=0) { deallocate(block,size); }

public:
	int    mAllocCount;
	int    mFreeCount;
	size_t mAllocVolume;

	static int    mAllocCountAll;
	static int    mFreeCountAll;
	static size_t mAllocVolumeAll;
	static void*  mpLastAllocation;
};
int    MallocAllocator::mAllocCountAll   = 0;
int    MallocAllocator::mFreeCountAll    = 0;
size_t MallocAllocator::mAllocVolumeAll  = 0;
void*  MallocAllocator::mpLastAllocation = NULL;

// Test TextureInfo that isn't used for anything other than being allocated in the TestCacheAllocator test
class TestTextureInfo : public EA::Text::TextureInfo
{
	virtual int AddRef() { return 0;}
	virtual int Release() { return 0;}
};


// Your application code may have its own equivalent to this or simply get away without needing it.
struct UVRectangle
{
	float mLeft;
	float mTop;
	float mRight;
	float mBottom;

	void  Assign(float l, float t, float r, float b) { mLeft = l; mTop = t; mRight = r; mBottom = b; }
};


// We use a single FontServer for all threads, and thus shared Fonts between threads.
struct GlyphCacheTestData
{
	EA::Text::FontServer    mFontServer;
	volatile bool           mbShouldQuit;
	EA::Thread::AtomicInt32 mnErrorCount;

	GlyphCacheTestData() : mFontServer(), mbShouldQuit(false), mnErrorCount(0){}
};


static void DrawText(EA::Text::GlyphCache_Memory& glyphCache, EA::Text::LineLayout& lineLayout)
{
	// This implementation is hard-coded to a single texture (kTextureIndex). The code here could be expanded to 
	// deal with the case that the required glyphs come from multiple textures. To do that efficiently on the 
	// GPU probably requires generating display lists that are grouped by texture in order to minimize state changes. 
	// That's an exercize for the reader that's outside the scope of this test/demonstration.

	const uint32_t kTextureIndex = 0;
	bool glyphCacheUpdateActive = false;

	retry:
	for(eastl_size_t i = 0, iEnd = lineLayout.mGlyphArray.size(); i != iEnd; i++)
	{
		const GlyphId          glyphId = lineLayout.mGlyphArray[i];
		const GlyphLayoutInfo& gli     = lineLayout.mGlyphLayoutInfoArray[i];

		const EA::Text::Font::GlyphBitmap* pGlyphBitmap;
		EA::Text::GlyphTextureInfo         glyphTextureInfo;
		UVRectangle                        uvRect;

		if(glyphCache.GetGlyphTextureInfo(gli.mpFont, glyphId, glyphTextureInfo)) // If we already have the glyph's cached in glyphCache...
			uvRect.Assign(glyphTextureInfo.mX1, glyphTextureInfo.mY1, glyphTextureInfo.mX2, glyphTextureInfo.mY2); 
		else
		{
			// We need to generate the glyph and add it to the glyph cache.
			if(gli.mpFont->RenderGlyphBitmap(&pGlyphBitmap, glyphId)) // This call locks the Font mutex until DoneGlyphBitmap is called.
			{
				// Copy the glyph to the texture.
				if(!glyphCacheUpdateActive)
				{
					glyphCacheUpdateActive = glyphCache.BeginUpdate(glyphCache.GetTextureInfo(kTextureIndex)); // This locks the glyph cache and leaves it open for writing until EndUpdate is called.

					if(!glyphCacheUpdateActive) // Normally this shouldn't happen, but we put it here for completeness.
						break;  // A given application's version of this may want to handle it differently than we do here.
				}

				bool success = glyphCache.AddGlyphTexture(gli.mpFont, glyphId, pGlyphBitmap->mpData, pGlyphBitmap->mnWidth, pGlyphBitmap->mnHeight, 
															pGlyphBitmap->mnStride, pGlyphBitmap->mBitmapFormat, glyphTextureInfo); 

				gli.mpFont->DoneGlyphBitmap(pGlyphBitmap); // Matches RenderGlyphBitmap above.

				if(success)
					uvRect.Assign(glyphTextureInfo.mX1, glyphTextureInfo.mY1, glyphTextureInfo.mX2, glyphTextureInfo.mY2); // These are UV values and so are in the range of [0.0,1.0].
				else
				{
					// The glyph doesn't fit. Flush the cache and retry the primitive. If you encounter this problem in a production environment then 
					// and you don't support the addition of extra textures, you could have a problem because there may be existing display lists that
					// refer to the glyphs that we are clearing here. You need to go back and redraw those (probably easier said than done) or come 
					// up with some way to handle this situation.
					glyphCache.ClearTexture(glyphCache.GetTextureInfo(kTextureIndex));
					goto retry;
				}
			}
			else // else we had an unexpected error.
			{
				// Normally this shouldn't happen, but we put it here for completeness. The only way it could happen  
				// is if the glyphId wasn't recognized, but that should always be so because previous code ensured so. 
				uvRect.Assign(0.f, 0.f, 0.f, 0.f);
			}
		}

		// We now have a uvRect for the glyph from the textureInfo.mTexture texture. 
		// And we have destination coordinates for the glyph in gli.mfX1, gli.mfY1, etc.
		// Whether the dest coordinates are Y-up or Y-down depends on what you set in LayoutSettings for Typesetter.
		// We are ready to generate a display list for these glyphs. How this is done is typically
		// application or render engine-specific. 
		// The format of the texture is application-specific and you typically won't have used a memory glyph cache
		// like we do here. You will need to set your render state to be a 1:1 pixel mapping orthographic projection
		// (or whatever you need to do to make the UVs map 1:1 to pixels), otherwise the glyphs will have little glitches
		// in them which we have pictures of in the EAText FAQ).
		// You may need to manually apply clipping here, depending on your situation. While EAText can wrap text within
		// a box, it doesn't currently have functionality to clip LineLayout glyph layout info to an arbitrary dest.
		// We have some example clipping code if you need it.

		// Hypothetical function call:
		// const EA::Text::TextureInfo* pTextureInfo = glyphCache.GetTextureInfo(kTextureIndex);
		// someVertexBuffer.AppendQuad(pTextureInfo->mTexture, uvRect.mLeft, uvRect.mTop, uvRect.mRight, uvRect.Bottom, gli.mfX1, gli.mfY1, gli.mfX2, gli.mfY2);
	}

	if(glyphCacheUpdateActive) // If we called glyphCache.BeginUpdate above, we need to eventually match it with this EndUpdate.
		glyphCache.EndUpdate(glyphCache.GetTextureInfo(kTextureIndex));
}


static intptr_t GlyphCacheTestThreadFunction(void* pGlyphCacheTestDataVoid)
{
	GlyphCacheTestData&         testData = *static_cast<GlyphCacheTestData*>(pGlyphCacheTestDataVoid);
	EA::Text::Typesetter        typesetter;  // It's possible for multiple threads to share a Typesetter instance, though that wouldn't gain much and the usage would have to be manually mutex-protected and could block the threads too much.
	EA::Text::GlyphCache_Memory glyphCache;
	int32_t                     nErrorCount = 0;
	EA::Text::LineLayout        lineLayout;
	EA::Text::TextStyle         textStyle(EATEXT_CHAR("Parolm"), 32);
	TestString                  s;

	glyphCache.Init(1, 1);

	while(!testData.mbShouldQuit)
	{
		GenerateRandomString(s, 1, 10, 6);
		lineLayout.Clear();
		typesetter.LayoutParagraph(s.c_str(), s.length(), 0, 0, 133, 10000, &textStyle, lineLayout);
		EATEST_VERIFY(lineLayout.Validate(true));

		DrawText(glyphCache, lineLayout);
	}

	glyphCache.Shutdown();

	testData.mnErrorCount += nErrorCount;
	return 0;
}


/////////////////////////////////////////////////////////////////////////////
// TestCacheAllocator
//
static int TestCacheAllocator()
{
	int nErrorCount = 0;

	// Allocate one extra than the size of the fixed allocator in the glyph cache for holding TextureInfo
	TestTextureInfo* textureInfoList[EA::Text::kTextureSlotCount + 1]; 
	const size_t kTextureSize = 64;
	MallocAllocator testAllocator;

	// This scoping is required so we can test that overflow memory is freed after the GlyphCache has been destroyed
	{
		GlyphCache_Memory testCache;

		// Test setup
		testCache.Init();
		testCache.SetAllocator(&testAllocator);
		EATEST_VERIFY(testAllocator.mAllocCount == 0);

		for(size_t i = 0; i < EAArrayCount(textureInfoList); ++i)
		{
			TestTextureInfo* pTextureInfo = EA_NEW("EATextTest/TestTextureInfo") TestTextureInfo();

			// This allocation needs to be done with our test allocator since we will be destroying the TestTextureInfo
			// in the glyph cache shutdown which will try to clean up this memory with our cache's allocator
			pTextureInfo->mTexture = (uintptr_t) testAllocator.Alloc(kTextureSize, "EATextTest/TestTextureInfo/Texture", EA::Allocator::MEM_TEMP, 0);
			pTextureInfo->mnSize = kTextureSize;

			textureInfoList[i] = pTextureInfo;
		}

		// Verify that we are indeed using our test allocator when we overflow our glyph cache's TextureInfoArray
		int preTestAllocCount = testAllocator.mAllocCount;
		for(size_t i = 0; i < EAArrayCount(textureInfoList); ++i)
		{
			testCache.AddTextureInfo(textureInfoList[i], true, true); 

			if(i == EAArrayCount(textureInfoList) - 1)
			{
				// We've overrun the TextureInfoArray's size so we must have allocated
				EATEST_VERIFY(testAllocator.mAllocCount == preTestAllocCount + 1);
			}
			else
			{
				// We shouldn't see any allocations since the GylphCache's TextureInfoArray is a fixed list
				EATEST_VERIFY(testAllocator.mAllocCount == preTestAllocCount);
			}
		}

		// Test cleanup
		testCache.Shutdown();
		// We should have freed all of the textureInfos created in the setup but not the extra space in the fixed TextureInfoArray
		// that instead will be freed upon destruction of the list
		EATEST_VERIFY(testAllocator.mAllocCount - 1 == testAllocator.mFreeCount);
	}
	// The destructor of the cache has been called so we should have released any overflow memory allocated for the TextureInfoArray
	EATEST_VERIFY(testAllocator.mAllocCount == testAllocator.mFreeCount);

	for(size_t i = 0; i < EAArrayCount(textureInfoList); ++i)
	{
		TestTextureInfo* pTextureInfo = textureInfoList[i];
		
		EA_DELETE(pTextureInfo);
	}

	return nErrorCount;
}


/////////////////////////////////////////////////////////////////////////////
// TestMultipleSimultaneousCaches
//
static int TestMultipleSimultaneousCaches()
{
	using namespace TestCacheInternal;

	int nErrorCount = 0;

	#if EATEXT_THREAD_SAFETY_ENABLED
		// We test using the same Typesetter, FontServer, and Fonts with two GlyphCaches. In a multithreaded environment.
		GlyphCacheTestData testData;

		// Add a single font ("Parolm") to the test data font server.
		EA::IO::MemoryStream* const pMemoryStream = EA_NEW("EAText/MemoryStream") EA::IO::MemoryStream((void*)pPalormSmallCapsTTF, sizeof(pPalormSmallCapsTTF), true, false, GetAllocator());
		testData.mFontServer.AddFace(pMemoryStream, kFontTypeOutline);

		// Create threads
		const int kThreadCount(2);
		EA::Thread::Thread thread[kThreadCount];
		int                i;

		for(i = 0; i < kThreadCount; i++)
			thread[i].Begin(GlyphCacheTestThreadFunction, &testData);

		EA::UnitTest::ThreadSleepRandom(3000, 3000);
		testData.mbShouldQuit = true;

		for(i = 0; i < kThreadCount; i++)
		{
			EA::Thread::Thread::Status status = thread[i].WaitForEnd(EA::Thread::GetThreadTime() + (1000*120));
			EATEST_VERIFY(status == EA::Thread::Thread::kStatusEnded);
		}

		nErrorCount += testData.mnErrorCount.GetValue();
	#else
		EA_UNUSED(&GlyphCacheTestThreadFunction);
	#endif

	return nErrorCount;
}

/////////////////////////////////////////////////////////////////////////////
// VerifyGlyphPadding
//
// This function tests that there is a pixel boarder of 'glyphPadding' pixels around a glyph on its texture atlas

enum GlyphPaddingError
{
	eGPENone   = 0x0,
	eGPELeft   = 0x1,
	eGPERight  = 0x2,
	eGPETop    = 0x4,
	eGPEBottom = 0x8
};

static uint32_t VerifyGlyphPadding(const EA::Text::GlyphTextureInfo* pGlyphTextureInfo, const int32_t glyphPadding)
{
	const EA::Text::TextureInfo* pTextureInfo   = pGlyphTextureInfo->mpTextureInfo;
	const uint32_t* pData                       = (uint32_t*)pTextureInfo->mpData;
	const uint32_t kTextureSize                 = pGlyphTextureInfo->mpTextureInfo->mnSize;
	const uint32_t kGlyphHeight                 = (uint32_t)((pGlyphTextureInfo->mY2 - pGlyphTextureInfo->mY1) * kTextureSize);
	const uint32_t kGlyphWidth                  = (uint32_t)((pGlyphTextureInfo->mX2 - pGlyphTextureInfo->mX1) * kTextureSize);
	uint32_t error = eGPENone;

	if(kGlyphHeight == 0 || kGlyphWidth == 0)
	{
		return error;
	}

	// We have UVs so convert them to indicies in mem
	// Determine starting pixel and end pixel
	const uint32_t X1 = (uint32_t)(pGlyphTextureInfo->mX1 * kTextureSize);
	const uint32_t X2 = X1 + kGlyphWidth - 1; // minus 1 because we don't want the extent of the glyph, we want its last indice
	const uint32_t Y1 = (uint32_t)(pGlyphTextureInfo->mY1 * kTextureSize);
	const uint32_t Y2 = Y1 + kGlyphHeight - 1; // minus 1 because we don't want the extent of the glyph, we want its last indice

	// Since we have padded the Glyph, we should be safe to check glyphPadding number of pixels on either side of the texture
	// check the top and bottom (padding) lines of the glyph
	for(int32_t j = 1; j <= glyphPadding; ++j)
	{
		for(uint32_t i = (X1 - glyphPadding), iEnd = (X2 + glyphPadding); i < iEnd; ++i)
		{
			const uint32_t topPixel     = pData[((Y1 - j) * kTextureSize) + i];
			const uint32_t bottomPixel  = pData[((Y2 + j) * kTextureSize) + i];

			bool topAlphaFound = false;
			bool bottomAlphaFound = false;
			switch(pTextureInfo->mFormat)
			{
				case TextureFormat::kTextureFormatABGR:
				case TextureFormat::kTextureFormatARGB:
				{
					topAlphaFound    = ((topPixel    & 0xFF000000) >> 24) != 0;
					bottomAlphaFound = ((bottomPixel & 0xFF000000) >> 24) != 0;
					break;
				}
				case TextureFormat::kTextureFormatRGBA:
				{
					topAlphaFound    = (topPixel    & 0xFF) != 0;
					bottomAlphaFound = (bottomPixel & 0xFF) != 0;
					break;
				}
				default:
				{
					EA_ASSERT_MSG(false, "Add support for testing this platform's texture format");
				}
			}

			error |= topAlphaFound ? eGPETop : eGPENone;
			error |= bottomAlphaFound ? eGPEBottom : eGPENone;
		}
	}

	// Now check the sides of the glyph between the top and bottom lines of the glyph
	for(uint32_t j = (Y1 - glyphPadding); j <= (Y2 + glyphPadding); ++j)
	{
		const uint32_t yOffset = (j * kTextureSize);
		for(int i = 1; i <= glyphPadding; ++i)
		{
			const uint32_t leftPixel = pData[yOffset  + X1 - i];
			const uint32_t rightPixel = pData[yOffset + X2 + i];

			bool leftAlphaFound = false;
			bool rightAlphaFound = false;
			switch(pTextureInfo->mFormat)
			{
				case TextureFormat::kTextureFormatABGR:
				case TextureFormat::kTextureFormatARGB:
				{
					leftAlphaFound  = ((leftPixel  & 0xFF000000) >> 24) != 0;
					rightAlphaFound = ((rightPixel & 0xFF000000) >> 24) != 0;
					break;
				}
				case TextureFormat::kTextureFormatRGBA:
				{
					leftAlphaFound  = (leftPixel  & 0xFF) != 0;
					rightAlphaFound = (rightPixel & 0xFF) != 0;
					break;
				}
				default:
				{
					EA_ASSERT_MSG(false, "Add support for testing this platform's texture format");
				}
			}

			error |= leftAlphaFound ? eGPELeft : eGPENone;
			error |= rightAlphaFound ? eGPERight : eGPENone;
		}               
	}

	return error;
}

/////////////////////////////////////////////////////////////////////////////
// TestCacheDetail
//
static int TestCacheDetail(EA::Text::GlyphCache& glyphCache, Font* pFont)
{
	int  nErrorCount = 0;
	bool bResult;

	//EATextTest_Rand r(gEATextTest_RandSeed);

	EA::Text::TextureInfo* pTextureInfo = glyphCache.GetTextureInfo(0);
	EATEST_VERIFY((pTextureInfo != NULL) && (pTextureInfo->mTexture != 0));

	EA::Text::GlyphTextureInfo glyphTextureInfo;

	// Create a fake glyph image.
	//char glyphImage[64][64];
	//memset(glyphImage, 0xff, sizeof(glyphImage));

	// Make a set of all glyphs in this font.
	typedef eastl::set<GlyphId> GlyphSet;
	GlyphSet           glyphSet;
	GlyphSet::iterator it;
	uint32_t           i;

	for(i = 32; i < 5000; i++) // This is not the most ideal way to enumerate font glyphs, but it's all we have for now.
	{
		Char    c = (Char)i;
		GlyphId g;

		if(pFont->GetGlyphIds(&c, 1, &g, false))
			glyphSet.insert(g);
	}

	// For the test below we will be intentionally fully filling and clearing the cache.
	glyphCache.SetOption(EA::Text::GlyphCache::kOptionAssertOnFailure, 0);

	// Useful print to display the glyphs in a web browser to help visualize what this test is trying to do. 
	// Dump the output into an html file or here http://www.w3schools.com/graphics/svg_intro.asp via "Try it yourself" button
	#if TEST_DEBUG_DRAW
		const uint32_t kCanvasSize = glyphCache.GetTextureInfo(0)->mnSize;
		const uint32_t kScaleFactor = 6;
		EA::EAMain::Report("\n\n<!DOCTYPE html><html><body><svg height=\"%d\" width=\"%d\" style=\"border: 1px solid black; \">", kCanvasSize * kCanvasSize, kCanvasSize * kCanvasSize);
	#endif

	for(it = glyphSet.begin(); it != glyphSet.end(); ++it)
	{
		GlyphId g = *it;

		// The glyph shouldn't ever be present, as our glyph set is a unique set of glyph ids.
		bResult = glyphCache.GetGlyphTextureInfo(pFont, g, glyphTextureInfo);
		EATEST_VERIFY(!bResult);

		if(!bResult)
		{
			const EA::Text::Font::GlyphBitmap* pGlyphBitmap;
			bResult = pFont->RenderGlyphBitmap(&pGlyphBitmap, g);
			EATEST_VERIFY(bResult);

			if(bResult)
			{
				bResult = glyphCache.BeginUpdate(pTextureInfo);
				EATEST_VERIFY(bResult);

				if(bResult)
				{
					Retry:
					bResult = glyphCache.AddGlyphTexture(pFont, g, pGlyphBitmap->mpData, pGlyphBitmap->mnWidth, 
														 pGlyphBitmap->mnHeight, pGlyphBitmap->mnStride, 
														 pGlyphBitmap->mBitmapFormat, glyphTextureInfo);

					if(!bResult)
					{
						glyphCache.ClearTexture(pTextureInfo);
						goto Retry;
					}

					bResult = glyphCache.EndUpdate(pTextureInfo);
					EATEST_VERIFY(bResult);

					bResult = glyphCache.GetGlyphTextureInfo(pFont, g, glyphTextureInfo);
					EATEST_VERIFY(bResult);
				
					uint32_t glyphPaddingError = VerifyGlyphPadding(&glyphTextureInfo, glyphCache.GetOption(GlyphCache::kOptionGlyphPadding));

					#if TEST_DEBUG_DRAW
						// Don't use EATest macros here since when we fail it will print to stdout muddling our test draw output
						if (glyphPaddingError != eGPENone) ++nErrorCount;

						const uint32_t kTextureSize = glyphTextureInfo.mpTextureInfo->mnSize;
						const uint32_t X1 = (uint32_t)(glyphTextureInfo.mX1 * kTextureSize);
						const uint32_t X2 = (uint32_t)(glyphTextureInfo.mX2 * kTextureSize);
						const uint32_t Y1 = (uint32_t)(glyphTextureInfo.mY1 * kTextureSize);
						const uint32_t Y2 = (uint32_t)(glyphTextureInfo.mY2 * kTextureSize);



						EA::EAMain::Report("<polygon points=\"");
						EA::EAMain::Report("%d,%d %d,%d %d,%d %d,%d",
							kScaleFactor * X1, kScaleFactor * Y2, kScaleFactor * X2, kScaleFactor * Y2,
							kScaleFactor * X2, kScaleFactor * Y1, kScaleFactor * X1, kScaleFactor * Y1);
						EA::EAMain::Report("\" style=\"fill:%s;stroke-width:0;shape-rendering:crispEdges;\" />\n", glyphPaddingError != eGPENone ? "red" : "purple");
					#else	
						EATEST_VERIFY_F(glyphPaddingError == eGPENone, "Padding off on top side of glyph (%d) has error %d", g, glyphPaddingError);
					#endif		
				}

				pFont->DoneGlyphBitmap(pGlyphBitmap);
			}
		}
	}

	#if TEST_DEBUG_DRAW
		EA::EAMain::Report("</svg></body></html>");
	#endif

	return nErrorCount;
}


/////////////////////////////////////////////////////////////////////////////
// TestCache
//
int TestCache()
{
	int  nErrorCount = 0;
	int  nResult;

	nErrorCount += TestCacheAllocator();
	nErrorCount += TestMultipleSimultaneousCaches();

	// Test EATextFixed106ToFloat / EATextFloatToFixed106
	{
		// Do a round trip and make sure the values are preserved.
		float    x = 20.5;
		uint16_t y = EATextFloatToFixed106(x);
		float    z = EATextFixed106ToFloat(y);
		uint16_t w = EATextFloatToFixed106(z);

		EATEST_VERIFY(x == z);
		EATEST_VERIFY(y == w);
	}

	for(int nAlgorithm = 0; nAlgorithm < 2; ++nAlgorithm)
	{
		EA::Text::GlyphCache_Memory glyphCache;

		if (nAlgorithm == 0)
			glyphCache.SetOption(EA::Text::GlyphCache::kOptionUseGlyphBinPacking, 0);
		else if (nAlgorithm == 1)
			glyphCache.SetOption(EA::Text::GlyphCache::kOptionUseGlyphBinPacking, 1);
		else
			EA_ASSERT_MSG(false, "Test needs to be updated for a new algorithm option, or the wrong value is used for iterating above");

		nResult = glyphCache.Init(1, 1); // nMaxTextureCount = 1, nInitialTextureCount = 1
		EATEST_VERIFY(nResult == 1);

		EA::Text::TextureInfo* pTextureInfo = glyphCache.GetTextureInfo(0);
		EATEST_VERIFY((pTextureInfo != NULL) && (pTextureInfo->mTexture != 0));

		EA::StdC::Memset8(pTextureInfo->mpData, 0, pTextureInfo->mnSize * pTextureInfo->mnSize * (eastl::min<size_t>(1, pTextureInfo->mFormat / 8)) );

		if(nResult > 0)
		{
			FontServer* pFontServer = GetFontServer();

			if(pFontServer)
			{
				// wipe out all fonts in the system
				pFontServer->Shutdown();
				pFontServer->Init();
				pFontServer->FreeUnusedFonts();
				EATEST_VERIFY(pFontServer != NULL);

				FilePathChar currDir[EA::IO::kMaxDirectoryLength];
				EATEST_VERIFY(GetTestDataDirectory(currDir, EAArrayCount(currDir)) > 0);

				// Load in a large font which we know will really fill the glyph cache / texture
				#if EATEXT_FILE_PATH_CHAR_TYPE_SIZE == 1
					EA::IO::Path::PathString8 path(currDir);
				#elif EATEXT_FILE_PATH_CHAR_TYPE_SIZE == 2
					EA::IO::Path::PathString16 path(currDir);
				#elif EATEXT_FILE_PATH_CHAR_TYPE_SIZE == 4
					EA::IO::Path::PathString32 path(currDir);
				#endif
				
				EA::IO::Path::Append(path, EATEXT_FILE_PATH_CHAR("arial.ttf")); // This font should have been deployed with the tests
				EA::IO::Path::Normalize(path);
				EATEST_VERIFY(pFontServer->AddFace(path.c_str()) > 0);

				TextStyle textStyle;
				EA::StdC::Strcpy(textStyle.mFamilyNameArray[0], EATEXT_CHAR("Arial")); 
				textStyle.mfSize = 16;

				Font* const pFont = pFontServer->GetFont(&textStyle);
				EATEST_VERIFY(pFont != NULL);

				if(pFont)
				{
					nErrorCount += TestCacheDetail(glyphCache, pFont);

					pFont->Release();
				}
			}

			nResult = glyphCache.Shutdown();
			EATEST_VERIFY(nResult == 0);
		}
	}

	return nErrorCount;
}
