///////////////////////////////////////////////////////////////////////////////
// EATextCache.cpp
//
// Copyright (c) 2004 Electronic Arts. All Rights Reserved.
// By Paul Pedriana, Maxis
//
///////////////////////////////////////////////////////////////////////////////


#include <EAText/EATextCache.h>
#include <EAText/EATextFont.h>
#include <EAStdC/EAString.h>
#include <EAStdC/EASprintf.h>
#include <EAStdC/EAAlignment.h>
#include <EAText/internal/EATextSquish.h>
#include <coreallocator/icoreallocatormacros.h>
#include <stdio.h>
#include EA_ASSERT_HEADER


namespace EA
{

namespace Text
{


///////////////////////////////////////////////////////////////////////////////
// TextureInfo
///////////////////////////////////////////////////////////////////////////////

TextureInfo::TextureInfo() : 
  //mLockInformation[]
	mpSource(NULL),
	mpStream(NULL),
	mPixelData(EA::Allocator::EASTLICoreAllocator(EASTL_NAME(EATEXT_ALLOC_PREFIX "TextureInfo/mPixelData"))), // Initialize with no particular allocator. We'll set it when we first use mPixelData.
	mTexture(0),
	mpData(NULL),
	mnStride(0),
	mfSizeInverse(0),
	mFormat(0),
	mnSize(0),
	mnGeneration(0),
  //mnColumnHeights[]
	mnOpenAreaX(0),
	mnOpenAreaY(0),
	mnOpenAreaLineH(0),
	mnColumnCount(0),
  //mnColumnWidths[kTextureColumnCountMax];
	mbWritable(true),
	mRefCount(0),
	mpCoreAllocator(NULL)
{
	memset(mLockInformation, 0, sizeof(mLockInformation));
	memset(mnColumnHeights, 0, sizeof(mnColumnHeights));
	memset(mnColumnWidths, 0, sizeof(mnColumnWidths));
}

TextureInfo::~TextureInfo()
{
}

int TextureInfo::AddRef()
{
	EA_ASSERT(mRefCount < 50000); // Sanity check.
	return ++mRefCount;
}

int TextureInfo::Release()
{
	#if EATEXT_THREAD_SAFETY_ENABLED
		int32_t rc = mRefCount.Decrement();
		if(rc)
			return rc;
		mRefCount.SetValue(1); // Prevent double destroy if AddRef/Release is called while in destructor. This can happen if the destructor does things like debug traces.
	#else
		if(mRefCount > 1)
			return --mRefCount;
	#endif

	if(mpCoreAllocator)
		CORE_DELETE(mpCoreAllocator, this);

	return 0;
}



///////////////////////////////////////////////////////////////////////////////
// GlyphCache
///////////////////////////////////////////////////////////////////////////////

///////////////////////////////////////////////////////////////////////////////
// GlyphCache
//
GlyphCache::GlyphCache(Allocator::ICoreAllocator* pCoreAllocator)
  : mpCoreAllocator(pCoreAllocator ? pCoreAllocator : EA::Text::GetAllocator()),
	mGlyphTextureMap(EA::Allocator::EASTLICoreAllocator(EATEXT_ALLOC_PREFIX "GlyphTextureMap", mpCoreAllocator)),
	mTextureInfoArray(EA::Allocator::EASTLICoreAllocator(EATEXT_ALLOC_PREFIX "TextureInfoArray", mpCoreAllocator)),
	mnTextureInfoCountMax(kTextureSlotCount),
	mnTextureSizeDefault(kTextureSizeDefault),
	mnTextureFormatDefault(kTextureFormatDXT3),
	mOptionColumnWidthScale(1.f),
	mnColumnCountDefault(0), 
	mbAutoTextureCreate(kTextureAutoCreateDefault),
	mbDoubleBuffer(false),
	mbAssertOnFailure(true),
	mbPremultipliedOutput(false),
	mbUseBinPack(true),
	mnGlyphPadding(1),
	mRecursionCounter(0),
	mnInitCount(0)
{
	// For some textures, such as DXT, we may want to make sure 
	// the columns are on 4 pixel boundaries.
	mnColumnWidthsDefault[0] =  4;
	mnColumnWidthsDefault[1] =  8;
	mnColumnWidthsDefault[2] = 12;
	mnColumnWidthsDefault[3] = 12;
	mnColumnWidthsDefault[4] = 16;
	mnColumnWidthsDefault[5] = 16;
	mnColumnWidthsDefault[6] = 20;
	mnColumnWidthsDefault[7] = 20;
	mnColumnCountDefault     =  8;

	#ifdef EA_DEBUG
		mbDebugBitmapEnable      = false;
		mDebugBitmapCounter      = 0;
		mDebugBitmapDirectory[0] = 0;
	#endif
}


GlyphCache::GlyphCache(const GlyphCache& glyphCache)
{
	operator=(glyphCache);
}


GlyphCache& GlyphCache::operator=(const GlyphCache&)
{
	EA_FAIL(); // This is not yet supported.
	return *this;
}


///////////////////////////////////////////////////////////////////////////////
// ~GlyphCache
//
GlyphCache::~GlyphCache()
{
	// Force a call to Shutdown, regardless of the init state.
	mnInitCount = 1; // Set to 1 so Shutdown unilaterally executes the shutdown code.
	Shutdown();      // It turns out that this Shutdown call can crash if there are outstanding textures, due to a virtual call to DestroyTexture. Solution: Call Shutdown manually before destructing this class.
}


///////////////////////////////////////////////////////////////////////////////
// SetAllocator
//
void GlyphCache::SetAllocator(Allocator::ICoreAllocator* pCoreAllocator)
{
	mpCoreAllocator = pCoreAllocator;
	mGlyphTextureMap.get_allocator().set_allocator(pCoreAllocator);
	mTextureInfoArray.get_allocator().set_overflow_allocator(EA::Allocator::EASTLICoreAllocator(EATEXT_ALLOC_PREFIX "TextureInfoArray", pCoreAllocator));
}


///////////////////////////////////////////////////////////////////////////////
// Init
//
int GlyphCache::Init(uint32_t nMaxTextureCount, uint32_t nInitialTextureCount)
{
	if(mnInitCount++ == 0) // Only pay attention to these parameters the first time this is called. If somebody tries to call this function again, ignore them.
	{
		mnTextureInfoCountMax = nMaxTextureCount;

		EA_ASSERT(nInitialTextureCount <= nMaxTextureCount);

		for(uint32_t i = 0; i < nInitialTextureCount; i++)
			AddTextureInfo(NULL, false, false);
	}

	return mnInitCount;
}


///////////////////////////////////////////////////////////////////////////////
// Shutdown
//
int GlyphCache::Shutdown()
{
	int result = 0;

	EA_ASSERT(mnInitCount > 0);
	if(mnInitCount > 0)
	{
		result = --mnInitCount;

		if(result == 0) // If this is the last Shutdown...
		{
			mGlyphTextureMap.clear();

			for(eastl_size_t i = 0; i < mTextureInfoArray.size(); i++)
			{
				TextureInfo* const pTextureInfo = mTextureInfoArray[i];

				if(pTextureInfo)
				{
					DestroyTexture(pTextureInfo);
					pTextureInfo->Release();
				}
			}

			mTextureInfoArray.clear();
		}

	}
	else
		result = 0;

	return result;
}


///////////////////////////////////////////////////////////////////////////////
// GetOption
//
int32_t GlyphCache::GetOption(int32_t option) const
{
	switch (option)
	{
		case kOptionMaxTextureCount:
			return (int32_t)mnTextureInfoCountMax;

		case kOptionDefaultSize:
			return (int32_t)mnTextureSizeDefault;

		case kOptionDefaultFormat:
			return (int32_t)mnTextureFormatDefault ;

		case kOptionAutoTextureCreate:
			return (int32_t)(mbAutoTextureCreate  ? 1 : 0);

		case kOptionDoubleBuffer:
			return (int32_t)(mbDoubleBuffer  ? 1 : 0);

		case kOptionGlyphPadding:
			return (int32_t)mnGlyphPadding;

		case kOptionColumnCount:
			return (int32_t)mnColumnCountDefault;

		case kOptionColumnWidthScale:
			return (int32_t)(mOptionColumnWidthScale * 65536.f); // Convert from float to 16:16 fixed point.
	}

	if((option >= kOptionColumnValueBase) && (option < (kOptionColumnValueBase + 100)))
		return (int32_t)mnColumnWidthsDefault[option - kOptionColumnValueBase];
	else 
		return 0;
}

///////////////////////////////////////////////////////////////////////////////
// SetOption
//
void GlyphCache::SetOption(int32_t option, int32_t value)
{
	switch (option)
	{
		// To consider: Add this option.
		//case kOptionMaxTextureCount:
		//    mnTextureInfoCountMax = (uint32_t)value;
		//    return;

		case kOptionDefaultSize:
			mnTextureSizeDefault = (uint32_t)value;
			return;

		case kOptionDefaultFormat:
			mnTextureFormatDefault = (uint32_t)value;
			return;

		case kOptionAutoTextureCreate:
			mbAutoTextureCreate = (value != 0);
			return;

		case kOptionDoubleBuffer:
			mbDoubleBuffer = (value != 0);
			return;

		case kOptionGlyphPadding:
			mnGlyphPadding = eastl::max_alt<int32_t>(0, value);
			return;

		case kOptionColumnCount:
			mnColumnCountDefault = (uint32_t)value;
			return;

		case kOptionColumnWidthScale:
			mOptionColumnWidthScale = (value / 65536.f); // Convert to 16:16 fixed point.
			return;

		case kOptionAssertOnFailure:
			mbAssertOnFailure = (value != 0);
			return;

		case kOptionPremultipliedOutput:
			mbPremultipliedOutput = (value != 0);
			return;

		case kOptionUseGlyphBinPacking:
			mbUseBinPack = (value != 0);
	}

	if((option >= kOptionColumnValueBase) && (option < (kOptionColumnValueBase + 100)))
		mnColumnWidthsDefault[option - kOptionColumnValueBase] = (uint32_t)value;
}


///////////////////////////////////////////////////////////////////////////////
// GetGlyphTextureInfo
//
bool GlyphCache::GetGlyphTextureInfo(Font* pFont, GlyphId glyphId, GlyphTextureInfo& glyphTextureInfo) const
{
	return GetGlyphTextureInfo(pFont, glyphId, 1.0f, 1.0f, glyphTextureInfo);
}

bool GlyphCache::GetGlyphTextureInfo(Font* pFont, GlyphId glyphId, float fScaleX, float fScaleY, GlyphTextureInfo& glyphTextureInfo) const
{
	#if EATEXT_THREAD_SAFETY_ENABLED
		EA::Thread::AutoFutex autoMutex(mMutex);
	#endif

	const GlyphTextureMap::const_iterator it = mGlyphTextureMap.find(GlyphInfo(pFont, glyphId, fScaleX, fScaleY));

	if(it != mGlyphTextureMap.end())
	{
		const GlyphTextureInfo& gti = (*it).second;
		glyphTextureInfo = gti;
		return true;
	}

	return false;
}


///////////////////////////////////////////////////////////////////////////////
// SetGlyphTextureInfo
//
bool GlyphCache::SetGlyphTextureInfo(Font* pFont, GlyphId glyphId, const GlyphTextureInfo& glyphTextureInfo)
{
	return SetGlyphTextureInfo(pFont, glyphId, 1.0f, 1.0f, glyphTextureInfo);
}

bool GlyphCache::SetGlyphTextureInfo(Font* pFont, GlyphId glyphId, float fScaleX, float fScaleY, const GlyphTextureInfo& glyphTextureInfo)
{
	const GlyphInfo glyphInfo(pFont, glyphId, fScaleX, fScaleY);

	mGlyphTextureMap[glyphInfo] = glyphTextureInfo; // This will auto-create the entry if not present.

	return true;
}


///////////////////////////////////////////////////////////////////////////////
// RemoveTextureInfo
//
void GlyphCache::RemoveTextureInfo(const Font* pFont)
{
	#if EATEXT_THREAD_SAFETY_ENABLED
		EA::Thread::AutoFutex autoMutex(mMutex);
	#endif

	for(GlyphTextureMap::iterator it = mGlyphTextureMap.begin(); it != mGlyphTextureMap.end(); )
	{
		const GlyphInfo& glyphInfo = it->first;

		if(glyphInfo.mpFont == pFont)
			it = mGlyphTextureMap.erase(it);
		else
			++it;
	}
}


///////////////////////////////////////////////////////////////////////////////
// ValidateGlyphTextureMap
//
#ifdef EA_DEBUG
	template <typename T>
	inline bool EATextCache_IsPowerOf2(T x){
		return (x & (x - 1)) == 0;
	}

	bool GlyphCache::ValidateGlyphTextureMap() const
	{
		// Test all glyphs by calling this function recursively.
		// This is an O(n^2) algorithm and is going to be pretty slow.
		for(GlyphTextureMap::const_iterator it = mGlyphTextureMap.begin(), itEnd = mGlyphTextureMap.end(); it != itEnd; ++it)
		{
			const GlyphTextureMap::value_type& value = *it;

			EA_ASSERT(value.first.mGlyphId != kGlyphIdInvalid);
			if(value.first.mGlyphId == kGlyphIdInvalid)
				return false;

			if(!ValidateGlyphInfo(value.first))
				return false;
		}

		return true;
	}

	bool GlyphCache::ValidateGlyphInfo(const GlyphInfo& glyphInfo) const
	{
		// Verify that the glyph's GlyphInfo and associated GlyphTextureInfo are sane.
		bool            bResult(true);

		GlyphTextureMap::const_iterator itGlyph = mGlyphTextureMap.find(glyphInfo);
		EA_ASSERT(itGlyph != mGlyphTextureMap.end());
		if(itGlyph == mGlyphTextureMap.end())
			return false;

		GlyphMetrics glyphMetrics;
		bResult = glyphInfo.mpFont && glyphInfo.mpFont->GetGlyphMetrics(glyphInfo.mGlyphId, glyphMetrics);
		EA_ASSERT_MESSAGE(bResult, "ValidateGlyphTextureMap: Glyph doesn't belong to Font.");
		if(!bResult)
			return false;

		const GlyphTextureInfo& gTI = itGlyph->second;
		bResult = gTI.mpTextureInfo && EATextCache_IsPowerOf2(gTI.mpTextureInfo->mnSize);
		EA_ASSERT_MESSAGE(bResult, "ValidateGlyphTextureMap: Invalid TextureInfo");
		if(!bResult)
			return false;

		bResult = (gTI.mX1 <  gTI.mpTextureInfo->mnSize) &&
				  (gTI.mY1 <  gTI.mpTextureInfo->mnSize) &&
				  (gTI.mX2 <= gTI.mpTextureInfo->mnSize) &&
				  (gTI.mY2 <= gTI.mpTextureInfo->mnSize) &&
				  (gTI.mX1 <= gTI.mX2) &&
				  (gTI.mY1 <= gTI.mY2);
		EA_ASSERT_MESSAGE(bResult, "ValidateGlyphTextureMap: Invalid TextureInfo");
		if(!bResult)
			return false;

		// Go through all other glyphs and verify that they don't overlap the given glyph.
		// There was a bug related to this and so this test checks for that.
		for(GlyphTextureMap::const_iterator it = mGlyphTextureMap.begin(), itEnd = mGlyphTextureMap.end(); it != itEnd; ++it)
		{
			const GlyphTextureMap::value_type& value  = *it;
			const GlyphInfo&                   gI2    = value.first;
			const GlyphTextureInfo&            gTI2   = value.second;

			if(!(gI2 == glyphInfo)) // If the currently iterated glyph is different from the input glyphId...
			{
				EA_ASSERT_MESSAGE(gTI2.mpTextureInfo, "ValidateGlyphTextureMap: Bad texture info.");
				if(!gTI2.mpTextureInfo)
					return false;

				if(gTI2.mpTextureInfo->mTexture == gTI.mpTextureInfo->mTexture) // If these two glyphs are on the same texture...
				{
					bResult = (gTI.mX2 <= gTI2.mX1) || (gTI.mX1 >= gTI2.mX2) ||
							  (gTI.mY2 <= gTI2.mY1) || (gTI.mY1 >= gTI2.mY2);

					EA_ASSERT_MESSAGE(bResult, "ValidateGlyphTextureMap: Overlapping glyph.");
					if(!bResult)
						return false;
				}
			}
		}

		return true;
	}
#endif // EA_DEBUG


///////////////////////////////////////////////////////////////////////////////
// AddGlyphTexture
//
bool GlyphCache::AddGlyphTexture(Font* pFont, GlyphId glyphId, const void* pSourceData, uint32_t nSourceSizeX, uint32_t nSourceSizeY, 
								  uint32_t nSourceStride, uint32_t nSourceFormat, EA::Text::GlyphTextureInfo& glyphTextureInfo)
{
	return AddGlyphTexture(pFont, glyphId, 1.0f, 1.0f, pSourceData, nSourceSizeX, nSourceSizeY, nSourceStride, nSourceFormat, glyphTextureInfo);
}


bool GlyphCache::AddGlyphTexture(Font* pFont, GlyphId glyphId, float fScaleX, float fScaleY, const void* pSourceData, uint32_t nSourceSizeX,
								 uint32_t nSourceSizeY, uint32_t nSourceStride, uint32_t nSourceFormat, EA::Text::GlyphTextureInfo& glyphTextureInfo)
{
	#if EATEXT_THREAD_SAFETY_ENABLED
		EA::Thread::AutoFutex autoMutex(mMutex);
	#endif

	EA_ASSERT_MESSAGE(mGlyphTextureMap.find(GlyphInfo(pFont, glyphId, fScaleX, fScaleY)) == mGlyphTextureMap.end(), "GlyphCache: Glyph already present.");

	uint32_t xPosition, yPosition;

	TextureInfo* pTextureInfo = AllocateTextureArea(nSourceSizeX, nSourceSizeY, xPosition, yPosition);

	if(pTextureInfo)
	{
		// If the texture isn't already locked, lock it now. The user will still have to manually  
		// unlock it with a call to EndUpdate sometime after this function has returned.
		if(!pTextureInfo->mpData)
		{
			if(!BeginUpdate(pTextureInfo))
			{
				// Note that the texture area allocated above is lost. It cannot   
				// be made available again until the entire texture is cleared.
				EA_ASSERT_MESSAGE(!pTextureInfo, "GlyphCache: Unable to lock texture.");
				pTextureInfo = NULL;
			}
		}

		const bool bResult = WriteTextureArea(pTextureInfo, xPosition, yPosition,
											   pSourceData, nSourceSizeX, nSourceSizeY, 
											   nSourceStride, nSourceFormat);
		EA_ASSERT(bResult);
		if(bResult)
		{
			// This operation will create the GlyphTextureInfo entry if not already present.
			// If it's already present, then this will overwrite the existing entry.
			GlyphTextureInfo& gti = mGlyphTextureMap[GlyphInfo(pFont, glyphId, fScaleX, fScaleY)];

			gti.mpTextureInfo = pTextureInfo;
			gti.mX1           = xPosition * pTextureInfo->mfSizeInverse;
			gti.mY1           = yPosition * pTextureInfo->mfSizeInverse;
			gti.mX2           = (xPosition + nSourceSizeX) * pTextureInfo->mfSizeInverse;
			gti.mY2           = (yPosition + nSourceSizeY) * pTextureInfo->mfSizeInverse;

			glyphTextureInfo = gti;

			#if defined(EA_DEBUG) && defined(AUTHOR_PPEDRIANA)
				ValidateGlyphTextureMap(pFont, glyphId);
			#endif

			return true;
		}
	}

	return false;
}


///////////////////////////////////////////////////////////////////////////////
// GetTextureInfo
//
TextureInfo* GlyphCache::GetTextureInfo(uint32_t nTextureIndex)
{
	if(nTextureIndex < (uint32_t)mTextureInfoArray.size())
		return mTextureInfoArray[(eastl_size_t)nTextureIndex];

	return NULL;
}


///////////////////////////////////////////////////////////////////////////////
// GetTextureInfoCount
//
uint32_t GlyphCache::GetTextureInfoCount() const
{
	return (uint32_t)mTextureInfoArray.size();
}


///////////////////////////////////////////////////////////////////////////////
// AddTextureInfo
//
TextureInfo* GlyphCache::AddTextureInfo(TextureInfo* pTextureInfo, bool bInitialized, bool bIgnoreSizeLimits)
{
	#if EATEXT_THREAD_SAFETY_ENABLED
		EA::Thread::AutoFutex autoMutex(mMutex);
	#endif

	// If already present then just return OK.
	if(eastl::find(mTextureInfoArray.begin(), mTextureInfoArray.end(), pTextureInfo) != mTextureInfoArray.end())
		return pTextureInfo;

	if(bIgnoreSizeLimits || (mTextureInfoArray.size() < mnTextureInfoCountMax))
	{
		if(pTextureInfo)
			pTextureInfo->AddRef(); // AddRef for this function.
		else
		{
			pTextureInfo = CORE_NEW(mpCoreAllocator, EATEXT_ALLOC_PREFIX "TextureInfo", 0) TextureInfo;
			pTextureInfo->AddRef(); // AddRef for this function.
			pTextureInfo->mpCoreAllocator = mpCoreAllocator; 
			pTextureInfo->mBinPack.mpCoreAllocator = mpCoreAllocator;
			bInitialized = false;
		}

		if(!pTextureInfo->mTexture) // If the user didn't supply the actual texture...
		{
			if(pTextureInfo->mnSize == 0)
				pTextureInfo->mnSize = mnTextureSizeDefault;
			else if(pTextureInfo->mnSize < kTextureMinSize)
				pTextureInfo->mnSize = kTextureMinSize;

			if(pTextureInfo->mFormat == kTextureFormatNone)
				pTextureInfo->mFormat = mnTextureFormatDefault;

			pTextureInfo->mTexture = CreateTexture(pTextureInfo);
		}

		if(pTextureInfo->mTexture) // If we now have a texture to use...
		{
			EA_ASSERT(pTextureInfo->mnSize > 0); // If this fails then the user passed in invalid information.

			pTextureInfo->mfSizeInverse = (float)(1.f / pTextureInfo->mnSize);

			if(!bInitialized)
			{
				pTextureInfo->mnGeneration    = 1;
				pTextureInfo->mnColumnCount   = (uint8_t)mnColumnCountDefault;
				pTextureInfo->mbWritable      = true;
				pTextureInfo->mnOpenAreaX     = 0;
				pTextureInfo->mnOpenAreaY     = 0;
				pTextureInfo->mnOpenAreaLineH = 0;

				for(uint32_t j = 0; j < mnColumnCountDefault; j++)
				{
					pTextureInfo->mnColumnWidths[j]  = EA::StdC::AlignUp<uint8_t, 4>((uint8_t)((mnColumnWidthsDefault[j] + mnGlyphPadding) * mOptionColumnWidthScale));
					pTextureInfo->mnColumnHeights[j] = 0;
					pTextureInfo->mnOpenAreaX        = pTextureInfo->mnOpenAreaX + pTextureInfo->mnColumnWidths[j];
				}

				if (mbUseBinPack)
					pTextureInfo->mBinPack.Init(pTextureInfo->mnSize, pTextureInfo->mnSize, mnGlyphPadding);

				// To consider: Do this clear in debug builds only.
				ClearTextureInternal(pTextureInfo);
			}

			// We transfer the AddRef for this function to mTextureInfoArray.
			mTextureInfoArray.push_back(pTextureInfo);

			return pTextureInfo;
		}

		pTextureInfo->Release(); // Matches the AddRef for this function.
	}

	return NULL;
}


///////////////////////////////////////////////////////////////////////////////
// ClearTexture
//
bool GlyphCache::ClearTexture(TextureInfo* pTextureInfo)
{
	#if EATEXT_THREAD_SAFETY_ENABLED
		EA::Thread::AutoFutex autoMutex(mMutex);
	#endif

	if (mbUseBinPack)
		pTextureInfo->mBinPack.Reset();

	// To consider: Do this clear in debug builds only.
	ClearTextureInternal(pTextureInfo);

	// Remove all glyph info that corresponds to the given texture.
	for(GlyphTextureMap::iterator it(mGlyphTextureMap.begin()); it != mGlyphTextureMap.end();)
	{
		const GlyphTextureInfo& gti = (*it).second;

		if(gti.mpTextureInfo == pTextureInfo)
		{
			GlyphTextureMap::iterator itSaved = it++;
			mGlyphTextureMap.erase(itSaved);
		}
		else
			++it;
	}

	// Clear the texture position allocation information.
	pTextureInfo->mnOpenAreaX     = 0; // We will set adjust it below.
	pTextureInfo->mnOpenAreaY     = 0; 
	pTextureInfo->mnOpenAreaLineH = 0;

	for(uint32_t i = 0; i < pTextureInfo->mnColumnCount; i++)
	{
		pTextureInfo->mnOpenAreaX        = (pTextureInfo->mnOpenAreaX + pTextureInfo->mnColumnWidths[i]);
		pTextureInfo->mnColumnHeights[i] = 0;
	}

	// Mark the texture as having been changed. Any future references that 
	// refer to a previous generation number will be known to be invalid.
	pTextureInfo->mnGeneration++;

	return true;
}


///////////////////////////////////////////////////////////////////////////////
// TryAllocateTextureArea
//
// Tries to allocate space from a specific texture.
//
// The texture is divided into a number of predefined vertical bins (columns).
// Also, there is an open area on the right side of the texture which handles
// overflow from the columns on the left side. The left-most columns are 
// narrow and handle thin glyphs such as the ones shown below. As we move
// rightward, successive columns are wider. Glyphs are added to each column
// until the column runs out of space, at which point the glyph is tried in 
// the next column, and so on.
//
// Each column is on a four pixel x boundary. And each character vertically
// within a column begins on a four pixel y boundary. There is at least one
// blank pixel between each glyph. This is done because some texture sampling
// will read outside of the glyph, and we don't want to read pixels from 
// other glyphs.
// 
// ------------------------------------------------------------------
// | i  |   b   |   W   |       | D3TPZ+?M                          |
// | l  |   c   |   X   |       |                                   |
// | .  |   e   |   Q   |       |                                   |
// | :  |   d   |   H   |       |                                   |
// | ;  |   r   |       |       |           (open area)             |
// | ,  |       |       |       |                                   |
// |    |       |       |       |                                   |
// |    |       |       |       |                                   |
// |    |       |       |       |                                   |
// |    |       |       |       |                                   |
// |    |       |       |       |                                   |
// |    |       |       |       |                                   |
// |    |       |       |       |                                   |
// |    |       |       |       |                                   |
// |    |       |       |       |                                   |
// |    |       |       |       |                                   |
// |    |       |       |       |                                   |
// |    |       |       |       |                                   |
// |    |       |       |       |                                   |
// |    |       |       |       |                                   |
// |    |       |       |       |                                   |
// |    |       |       |       |                                   |
// |    |       |       |       |                                   |
// ------------------------------------------------------------------
//
bool GlyphCache::TryAllocateTextureArea(uint32_t xSize, uint32_t ySize, TextureInfo& ti, uint32_t& xPosition, uint32_t& yPosition)
{
	// Disabled until the Futex::HasLock function is published with EAThread.
	//#if EATEXT_THREAD_SAFETY_ENABLED
	//    EA_ASSERT(mMutex.HasLock()); // This is an internal function, so mutex should be locked.
	//#endif

	// If we have very large glyphs then we have a problem. In reality, if the user is 
	// trying to draw very large glyphs then we really ought to be using very large 
	// textures or the user might want to use polygon glyphs instead.
	EA_ASSERT((xSize < ti.mnSize) && (ySize < ti.mnSize));

	uint32_t x = 0;

	// First try the columns on the left side of the texture.
	for(uint32_t j = 0; j < ti.mnColumnCount; j++)
	{
		EA_ASSERT((x % 4) == 0); // Assert that each column begins on a 4 pixel boundary.

		const uint32_t tmpX = EA::StdC::AlignUp<uint32_t, 4>(x + mnGlyphPadding);
		const uint32_t tmpY = EA::StdC::AlignUp<uint32_t, 4>(ti.mnColumnHeights[j] + mnGlyphPadding);

		if (tmpX + xSize - x < ti.mnColumnWidths[j] && tmpY + ySize + mnGlyphPadding < ti.mnSize)
		{
			xPosition = tmpX;
			yPosition = tmpY;
			ti.mnColumnHeights[j] = tmpY + ySize;

			EA_ASSERT(ti.mnColumnHeights[j] <= ti.mnSize);
			EA_ASSERT(((xPosition % 4) == 0) && ((yPosition % 4) == 0));
			EA_ASSERT(((xPosition + xSize) < ti.mnSize) && ((yPosition + ySize) < ti.mnSize));
			return true;
		}  

		x += ti.mnColumnWidths[j];
	}

	// At this point none of the above columns had space. 
	// So we try to use the open space on the right side of the texture.
	EA_ASSERT((xSize <= (ti.mnSize - (mnGlyphPadding*2))) && (ySize <= (ti.mnSize - (mnGlyphPadding*2)))); // Assert that this glyph could ever fit into the space of the texture. 

	if((x < ti.mnSize) && (xSize <= (ti.mnSize - x))) // If there is an open area to the right of these columns and if this glyph could possibly ever fit into it...
	{
		EA_ASSERT(ti.mnOpenAreaX >= x); // The current free spot should always be to the right of any columns.

		Retry:
		const uint32_t tmpY = EA::StdC::AlignUp<uint32_t, 4>(ti.mnOpenAreaY + mnGlyphPadding);
		
		bool bGlyphFitsY = ((tmpY + ySize + mnGlyphPadding) < ti.mnSize); // We add mnGlyphPadding here to ensure there is room for padding on both sides
		if(bGlyphFitsY)
		{
			const uint32_t tmpX = EA::StdC::AlignUp<uint32_t, 4>(ti.mnOpenAreaX + mnGlyphPadding);
			
			bool bGlyphFitsX = ((tmpX + xSize + mnGlyphPadding) < ti.mnSize); // We add mnGlyphPadding here to ensure there is room for padding on both sides
			if(bGlyphFitsX) // Much of the time this will be true.
			{
				xPosition = tmpX;
				yPosition = tmpY;

				ti.mnOpenAreaX = EA::StdC::AlignUp<uint32_t, 4>(tmpX + xSize); // bump to 4 pixel increment.

				if(ti.mnOpenAreaLineH < ySize)
					ti.mnOpenAreaLineH = ySize;

				EA_ASSERT(((ti.mnOpenAreaX % 4) == 0) && ((ti.mnOpenAreaY % 4) == 0));
				EA_ASSERT(((xPosition % 4) == 0) && ((yPosition % 4) == 0));
				EA_ASSERT(((xPosition + xSize) < ti.mnSize) && ((yPosition + ySize) < ti.mnSize));

				return true;
			}
			else
			{
				// Reset mnOpenAreaX to the left side of the open area. We know from above that the glyph will fit in the x direction.
				// Move mnOpenAreaY down by one line. The + mnGlyphPadding separates each line by at least mnGlyphPadding.
				// Reset mnOpenAreaLineH to zero; we will grow it anew as we add glyphs to this line.

				ti.mnOpenAreaX = x;  
				ti.mnOpenAreaY = EA::StdC::AlignUp<uint32_t, 4>(ti.mnOpenAreaY + ti.mnOpenAreaLineH + mnGlyphPadding);  // bump to 4 pixel increment.
				ti.mnOpenAreaLineH = 0;

				goto Retry;                     
			}
		}
		else
		{
			// If the glyph doesn't fit in the y direction, then it has no chance of fitting
			// in the open space unless we clear the entire texture or try a different texture.
		}
	}

	// No more space.
	return false;
}


bool GlyphCache::TryAllocateTextureAreaBin(uint32_t xSize, uint32_t ySize, TextureInfo& ti, uint32_t& xPosition, uint32_t& yPosition)
{
	// If we have very large glyphs then we have a problem. In reality, if the user is 
	// trying to draw very large glyphs then we really ought to be using very large 
	// textures or the user might want to use polygon glyphs instead.
	EA_ASSERT((xSize < ti.mnSize) && (ySize < ti.mnSize));

	RectangleBinPack::Node *node = ti.mBinPack.Insert(xSize, ySize);
	if (node)
	{
		xPosition = node->x + mnGlyphPadding;
		yPosition = node->y + mnGlyphPadding;
		return true;
	}
	return false;
}


///////////////////////////////////////////////////////////////////////////////
// AllocateTextureArea
//
// Tries to allocate space from a texture. First try for free space on 
// existing textures, then try to free space on existing textures, 
// then try to create a new texture.
//
TextureInfo* GlyphCache::AllocateTextureArea(uint32_t xSize, uint32_t ySize, uint32_t& xPosition, uint32_t& yPosition)
{
	#if EATEXT_THREAD_SAFETY_ENABLED
		EA::Thread::AutoFutex autoMutex(mMutex);
	#endif

	TextureInfoArray::size_type i, iEnd;
	TextureInfo* pTextureInfo = NULL;

	// Try allocating from space in existing textures.
	for(i = 0, iEnd = mTextureInfoArray.size(); i < iEnd; i++)
	{
		TextureInfo* const pTITemp = mTextureInfoArray[i];

		if(pTITemp->mbWritable)
		{
			bool success = false;
			if (mbUseBinPack)
				success = TryAllocateTextureAreaBin(xSize, ySize, *pTITemp, xPosition, yPosition);
			else
				success = TryAllocateTextureArea(xSize, ySize, *pTITemp, xPosition, yPosition);

			if (success)
			{
				pTextureInfo = pTITemp;
				break;
			}
		}
	}

	if(!pTextureInfo)
	{
		// At this point, no texture could be cleared or could serve the request. We possibly try to create a new texture.
		if(mbAutoTextureCreate && (mTextureInfoArray.size() < mnTextureInfoCountMax) && (mRecursionCounter == 0))
		{
			if(AddTextureInfo(NULL, false, false))
			{
				// Here we call ourself recursively. We use the recursion counter to make 
				// sure we don't get into an infinite loop of failure and texture allocation.
				mRecursionCounter++;
				pTextureInfo = AllocateTextureArea(xSize, ySize, xPosition, yPosition);
				mRecursionCounter--;
			}
		}

		// If this fails then mbAutoTextureCreate is false or mnTextureInfoCountMax needs to be increased, 
		// or text usage needs to be reduced. If you know your app well then you may be able to manually 
		// clear glyph textures at known application points. It's impossible for us to automatically clear 
		// textures here because we don't know if the user has any outstanding text usage that's referring 
		// to glyphs and their positions in this cache. We would need to add an elaborate reference counting
		// scheme (or equivalent) in order to accomplish that.
		#if defined(EA_ASSERT_ENABLED) || (defined(EA_TRACE_ENABLED) && EA_TRACE_ENABLED)
			if(mbAssertOnFailure)
			{
				EA_ASSERT_MSG(pTextureInfo != NULL, "GlyphCache::AllocateTextureArea: Failure to expand texture space. Glyphs may be graphically corrupted.");
			}
		#endif
	}

	#ifdef EA_DEBUG
		if(!pTextureInfo)
		{
			xPosition = UINT32_MAX;
			yPosition = UINT32_MAX;
		}
	#endif

	return pTextureInfo;
}


///////////////////////////////////////////////////////////////////////////////
// WriteTextureArea
//
bool GlyphCache::WriteTextureArea(TextureInfo* pTextureInfo, uint32_t nDestPositionX, uint32_t nDestPositionY,
								  const void* pSourceData, uint32_t nSourceSizeX, uint32_t nSourceSizeY,    
								  uint32_t nSourceStride, uint32_t nSourceFormat)
{
	#if EATEXT_THREAD_SAFETY_ENABLED
		EA::Thread::AutoFutex autoMutex(mMutex);
	#endif

	EA_ASSERT_MESSAGE(pTextureInfo->mpData != NULL, "GlyphCache::WriteTextureArea: Texture is not locked.");

	if(pTextureInfo->mpData) // If locked...
	{
		#ifdef EA_DEBUG
			if(mbDebugBitmapEnable)
			{
				const size_t dirPathLength = strlen(mDebugBitmapDirectory);

				if(dirPathLength && (dirPathLength < EAArrayCount(mDebugBitmapDirectory)))
				{
					mDebugBitmapCounter++;

					EA::StdC::Sprintf(mDebugBitmapDirectory + dirPathLength, "glyph%d.ppm", (int)mDebugBitmapCounter);
					WritePPMFile(mDebugBitmapDirectory, nSourceFormat, pSourceData, nSourceSizeX, nSourceSizeY, nSourceStride);

					EA::StdC::Sprintf(mDebugBitmapDirectory + dirPathLength, "cache%d.ppm", (int)mDebugBitmapCounter);
				  //WritePPMFile(mDebugBitmapDirectory, nSourceFormat, pTextureInfo->mpData, pTextureInfo->mnSize, pTextureInfo->mnSize, (uint32_t)pTextureInfo->mnStride);
					WritePPMFile(mDebugBitmapDirectory, pTextureInfo->mFormat, pTextureInfo->mpData, pTextureInfo->mnSize, pTextureInfo->mnSize, (uint32_t)pTextureInfo->mnStride);

					mDebugBitmapDirectory[dirPathLength] = 0;
				}
			}
		#endif

		WriteTextureArea(pTextureInfo->mpData, nDestPositionX, nDestPositionY, 
						 (uint32_t)pTextureInfo->mnStride, pTextureInfo->mFormat,
						 pSourceData, nSourceSizeX, nSourceSizeY, nSourceStride, nSourceFormat);
		return true;
	}

	return false;
}


///////////////////////////////////////////////////////////////////////////////
// WriteTextureArea
//
void GlyphCache::WriteTextureArea(void* pDest, uint32_t nDestPositionX, uint32_t nDestPositionY, 
								  uint32_t nDestStride, uint32_t nDestFormat, 
								  const void* pSourceData, uint32_t nSourceSizeX, uint32_t nSourceSizeY,
								  uint32_t nSourceStride, uint32_t nSourceFormat)
{
	// To do: Make a generic plug-in blitting system. This would allow 
	// users to use somewhat arbitrary destination texture formats.
	// We started some work on this with EATextBlit already. This may 
	// not be as important these days, as video cards are fairly flexible.

	#if EATEXT_THREAD_SAFETY_ENABLED
		EA::Thread::AutoFutex autoMutex(mMutex);
	#endif

	if(nDestFormat == kTextureFormatDXT3)
	{
		// DXT1: http://msdn.microsoft.com/en-us/library/bb147243(VS.85).aspx
		// DXT3 (based on DXT1): http://msdn.microsoft.com/en-us/library/bb206238.aspx

		if((nSourceFormat == kTextureFormatARGB) || (nSourceFormat == kTextureFormatRGBA))
		{
			TextureIterator dstRow(pDest, nDestStride);
			dstRow.Offset(nDestPositionX * 4, nDestPositionY / 4);

			for(uint32_t y = 0; y < nSourceSizeY; y += 4, ++dstRow) // For each band of four rows...
			{
				uint8_t*       pDestination = (uint8_t*)dstRow; // Pointer to the current 128 bit (16 byte) output block.
				const uint32_t blockHeight  = eastl::min_alt<uint32_t>(nSourceSizeY - y, 4); 

				for(uint32_t x = 0; x < nSourceSizeX; x += 4, pDestination += 16) // For band of four columns...
				{
					uint32_t* const pSource     = (uint32_t*)pSourceData + (y * (nSourceStride / 4)) + x; // We need to use (sourceStride / 4) here instead of nSourceSizeX.
					uint32_t*       pSourceTemp = pSource;
					uint8_t*        pRowAlpha   = (uint8_t*)pDestination;

					// Write the alpha values the current 4x4 block (pSource/pDestination).
					for(uint32_t row = 0; row < blockHeight; ++row, pSourceTemp += (nSourceStride / 4), pRowAlpha += 2)
					{
						// The DXT documentation doesn't make this clear, but we need to write four nibbles 
						// in order of source 3, source 2, source 1, source 0. On a big endian system we can
						// do this by writing two bytes (uint16_t): first a byte for source 3 and 2, then a 
						// byte for source 1 and 0. On a little endian system, a uint16_t is byte-swapped, 
						// so we need to write first a byte for source 1 and 0, then a byte for source 3 and 2.
						if(nSourceFormat == kTextureFormatARGB)
						{
							#if defined(EA_PLATFORM_PS3)
								pRowAlpha[0] = (uint8_t)(((pSourceTemp[1] >> 24) & 0xf0) | (pSourceTemp[0] >> 28));
								pRowAlpha[1] = (uint8_t)(((pSourceTemp[3] >> 24) & 0xf0) | (pSourceTemp[2] >> 28));
							#elif defined(EA_SYSTEM_BIG_ENDIAN)
								pRowAlpha[0] = (uint8_t)(((pSourceTemp[3] >> 24) & 0xf0) | (pSourceTemp[2] >> 28));
								pRowAlpha[1] = (uint8_t)(((pSourceTemp[1] >> 24) & 0xf0) | (pSourceTemp[0] >> 28));
							#else
								pRowAlpha[0] = (uint8_t)(((pSourceTemp[1] >> 24) & 0xf0) | (pSourceTemp[0] >> 28));
								pRowAlpha[1] = (uint8_t)(((pSourceTemp[3] >> 24) & 0xf0) | (pSourceTemp[2] >> 28));
							#endif
						}
						else  // else RGBA
						{
							#if defined(EA_PLATFORM_PS3)
								pRowAlpha[0] = (uint8_t)(((pSourceTemp[1] >> 0) & 0xf0) | ((pSourceTemp[0] >> 4) & 0x0f));
								pRowAlpha[1] = (uint8_t)(((pSourceTemp[3] >> 0) & 0xf0) | ((pSourceTemp[2] >> 4) & 0x0f));
							#elif defined(EA_SYSTEM_BIG_ENDIAN)
								pRowAlpha[0] = (uint8_t)(((pSourceTemp[3] >> 0) & 0xf0) | ((pSourceTemp[2] >> 4) & 0x0f));
								pRowAlpha[1] = (uint8_t)(((pSourceTemp[1] >> 0) & 0xf0) | ((pSourceTemp[0] >> 4) & 0x0f));
							#else
								pRowAlpha[0] = (uint8_t)(((pSourceTemp[1] >> 0) & 0xf0) | ((pSourceTemp[0] >> 4) & 0x0f));
								pRowAlpha[1] = (uint8_t)(((pSourceTemp[3] >> 0) & 0xf0) | ((pSourceTemp[2] >> 4) & 0x0f));
							#endif
						}
					}
		
					// Compress the current 4x4 block (pSource/pDestination).
					EATextSquish::Compress((uint8_t*)pSource, pDestination, nSourceStride, nSourceSizeY * nSourceStride);
				}
			}
		}
		else // (nSourceFormat == Font::kBFMonochrome) || (nSourceFormat == Font::kBFGrayscale)
		{
			EA_ASSERT((nDestPositionX & 3) == 0); // Make sure we are on 4 texel alignment.
			EA_ASSERT((nSourceFormat == Font::kBFMonochrome) || (nSourceFormat == Font::kBFGrayscale));

			TextureIterator dstRow(pDest, nDestStride);

			int y      = nDestPositionY;
			int yLimit = y + nSourceSizeY;

			// We implement a DXT3 texture write.
			int xtiles = nSourceSizeX / 4;
			int xextra = nSourceSizeX % 4 ;

			// DXT3 maps 4 pixels to a tile row
			dstRow.Offset(nDestPositionX * 4, nDestPositionY >> 2);

			while(y < yLimit)
			{
				// set 'dst' to a line within the 4x4 tile
				uint16_t* dst = (uint16_t*)dstRow + (y & 3);

				const uint8_t* src2 = (const uint8_t*)pSourceData;
				uint32_t nSourceMask = 0x80; // Binary 10000000

				// load whole tiles
				if(nSourceFormat == Font::kBFMonochrome)
				{
					for(int xt = 0; xt < xtiles; ++xt)
					{
						uint32_t alphaMask = 0;

						for(int i = 0, v = 0x000f; i < 4; i++, v <<= 4, nSourceMask >>= 1)
						{
							if(*src2 & nSourceMask)
								alphaMask += v;
						}

						if(nSourceMask == 0)
						{
							src2++;
							nSourceMask = 0x80;
						}

						#ifdef EA_PLATFORM_PS3
							// For PS3, we swap the high and low bytes of 'alphaMask'
							*dst = ((uint16_t)alphaMask >> 8) | ((uint16_t)alphaMask << 8);
						#else
							*dst = (uint16_t)alphaMask;
						#endif

						dst += 8;
					}
				}
				else
				{
					for(int xt = 0; xt < xtiles; ++xt)
					{
						// We take the 4 MSBs of alpha from each of the 4 pixels, 
						// and pack them into the lower 16 bits of 'alphaMask'
						uint32_t alphaMask = (((uint32_t)src2[3] & 0xf0) << 8) + 
											 (((uint32_t)src2[2] & 0xf0) << 4) + 
											  ((uint32_t)src2[1] & 0xf0) + 
											  ((uint32_t)src2[0] >> 4);
						src2 += 4;

						#ifdef EA_PLATFORM_PS3
							// For PS3, we swap the high and low bytes of 'alphaMask'
							*dst = (uint16_t)alphaMask >> 8 | (uint16_t)alphaMask << 8;
						#else
							*dst = (uint16_t)alphaMask;
						#endif

						dst += 8;
					}
				}
	 
				// load partial tile
				if(xextra) // xextra will be a value between 0 and 3.
				{
					uint32_t alphaMask = 0;

					if(nSourceFormat == Font::kBFMonochrome)
					{
						for(int i = 0, v = 0x000f; i < xextra; i++, v <<= 4, nSourceMask >>= 1)
						{
							if(*src2 & nSourceMask)
								alphaMask += v;
						}
					} 
					else
					{
						for(int x2 = xextra - 1; x2 >= 0; --x2)
							alphaMask = (alphaMask << 4) + (src2[x2] >> 4);
					}

					#ifdef EA_PLATFORM_PS3 // For PS3, we swap the high and low bytes of 'alphaMask'
						*dst = ((uint16_t)alphaMask >> 8) | ((uint16_t)alphaMask << 8);
					#else
						*dst = (uint16_t)alphaMask;
					#endif
				}

				// Check if we're crossing a tile boundary vertically.
				if((++y & 3) == 0)
					++dstRow;

				pSourceData = (const uint8_t*)pSourceData + nSourceStride;
			}
		}
	}
	else if((nDestFormat == kTextureFormatARGB) || (nDestFormat == kTextureFormatRGBA) || (nDestFormat == kTextureFormatABGR))
	{
		if(nSourceFormat == Font::kBFGrayscale)
		{
			// We blit the source to the destination by setting up pointers and     
			// walking the source and destination in memory. This requires more
			// lines of C++ code than using array operators, but is a little faster.

			const uint8_t* pSource8Begin  = (const uint8_t*)pSourceData;
			const uint8_t* pSource8End    = pSource8Begin + (nSourceSizeY * nSourceStride);
			uint32_t*      pDest32Begin   = (uint32_t*)pDest + (nDestPositionY * nDestStride / sizeof(uint32_t)) + nDestPositionX;

			if (mbPremultipliedOutput)
			{
				while (pSource8Begin < pSource8End)
				{
					const uint8_t* pSource8Row = pSource8Begin;
					const uint8_t* pSource8RowEnd = pSource8Begin + nSourceSizeX;
					uint32_t*      pDest32Row = pDest32Begin;

					while (pSource8Row < pSource8RowEnd)
					{
						*pDest32Row = (uint32_t)((*pSource8Row << 24) + (*pSource8Row << 16) + (*pSource8Row << 8) + (*pSource8Row << 0));
						pSource8Row++;
						pDest32Row++;
					}

					pSource8Begin = pSource8Begin + nSourceStride;
					pDest32Begin = (uint32_t*)((uint8_t*)pDest32Begin + nDestStride);
				}
			}
			else
			{
				while(pSource8Begin < pSource8End)
				{
					const uint8_t* pSource8Row    = pSource8Begin;
					const uint8_t* pSource8RowEnd = pSource8Begin + nSourceSizeX;
					uint32_t*      pDest32Row     = pDest32Begin;

					if(nDestFormat == kTextureFormatARGB || nDestFormat == kTextureFormatABGR)
					{
						while(pSource8Row < pSource8RowEnd)
						{
							#if EATEXT_PS3_RGBA_COMPAT  // Treat ARGB as RGBA. This is for backward compatibility for before we had kTextureFormatRGBA 
							   *pDest32Row = (uint32_t)(0xffffff00 + (*pSource8Row << 0));
							#else
							   *pDest32Row = (uint32_t)(0x00ffffff + (*pSource8Row << 24));
							#endif
							pSource8Row++;
							pDest32Row++;
						}
					}
					else // RGBA
					{
						while(pSource8Row < pSource8RowEnd)
						{
							*pDest32Row = (uint32_t)(0xffffff00 + (*pSource8Row << 0));
							pSource8Row++;
							pDest32Row++;
						}
					}

					pSource8Begin = pSource8Begin + nSourceStride;
					pDest32Begin  = (uint32_t*)((uint8_t*)pDest32Begin + nDestStride);
				}
			}
		}
		else if(nSourceFormat == Font::kBFMonochrome)
		{
			const uint8_t* pSource8Begin  = (const uint8_t*)pSourceData;
			const uint8_t* pSource8End    = pSource8Begin + (nSourceSizeY * nSourceStride);
			uint32_t*      pDest32Begin   = (uint32_t*)pDest + (nDestPositionY * nDestStride / sizeof(uint32_t)) + nDestPositionX;

			while(pSource8Begin < pSource8End)
			{
				uint32_t              nSourceMask   = 0x00000080;
				const uint8_t*        pSource8Row   = pSource8Begin;
				uint32_t*             pDest32Row    = pDest32Begin;
				const uint32_t* const pDest32RowEnd = pDest32Begin + nSourceSizeX;

				while(pDest32Row < pDest32RowEnd)
				{
					if(*pSource8Row & nSourceMask) // To consider: Make a branchless version of this logic.
						*pDest32Row = 0xffffffff;
					else
					{
						#if EATEXT_PS3_RGBA_COMPAT  // If RGBA instead of ARGB...
							*pDest32Row = 0xffffff00;
						#else
							*pDest32Row = 0x00ffffff;
						#endif
					}

					nSourceMask >>= 1;

					if(nSourceMask == 0)
					{
						pSource8Row++;
						nSourceMask = 0x00000080;
					}

					pDest32Row++;
				}

				pSource8Begin = pSource8Begin + nSourceStride;
				pDest32Begin  = (uint32_t*)((uint8_t*)pDest32Begin + nDestStride);
			}
		}
		else if(nSourceFormat == Font::kBFARGB)
		{
			// We blit the source to the destination by setting up pointers and     
			// walking the source and destination in memory. This requires more
			// lines of C++ code than using array operators, but is a little faster.
			const uint32_t* pSource32Begin = (const uint32_t*)pSourceData;
			const uint32_t* pSource32End   = pSource32Begin + (nSourceSizeY * nSourceStride / sizeof(uint32_t));
			uint32_t*       pDest32Begin   = (uint32_t*)pDest + (nDestPositionY * nDestStride / sizeof(uint32_t)) + nDestPositionX;

			if(nDestFormat == kTextureFormatARGB)
			{
				while(pSource32Begin < pSource32End)  // For each row...
				{
					#if EATEXT_PS3_RGBA_COMPAT  // If RGBA instead of ARGB...
						// Copy ARGB to RGBA
						const uint32_t* pSource32       = pSource32Begin;
						const uint32_t* pSource32RowEnd = pSource32Begin + nSourceSizeX; 
						uint32_t*       pDest32         = pDest32Begin;

						while(pSource32 < pSource32RowEnd)
						{
							const uint32_t color = *pSource32++;
							*pDest32++ = ((color & 0x00ff0000) << 8) | ((color & 0x0000ff00) << 8) | ((color & 0x000000ff) << 8) | ((color & 0xff000000) >> 24);
						}
					#else
						memcpy(pDest32Begin, pSource32Begin, nSourceSizeX * sizeof(uint32_t)); // Copy ARGB to ARGB
					#endif

					pSource32Begin = pSource32Begin + (nSourceStride / sizeof(uint32_t));
					pDest32Begin   = pDest32Begin   + (nDestStride   / sizeof(uint32_t));
				}
			}
			else if(nDestFormat == kTextureFormatABGR)
			{
				if (mbPremultipliedOutput)
				{
					while (pSource32Begin < pSource32End)  // For each row...
					{
						// Copy ARGB to ABGR, premultiplying in the process
						const uint32_t* pSource32 = pSource32Begin;
						const uint32_t* pSource32RowEnd = pSource32Begin + nSourceSizeX;
						uint32_t*       pDest32 = pDest32Begin;

						while (pSource32 < pSource32RowEnd)
						{
							const uint32_t color = *pSource32++;
							// See CombineColor in EATextEffects for details of the fixed-point math happening here
							uint32_t alpha = (color & 0xff000000) >> 24;
							uint32_t red = ((color & 0x00ff0000) >> 16) * alpha + 1;
							uint32_t green = ((color & 0x0000ff00) >> 8) * alpha + 1;
							uint32_t blue = (color & 0x000000ff) * alpha + 1;
							red = ((red + (red >> 8)) >> 8);
							green = ((green + (green >> 8)) >> 8);
							blue = ((blue + (blue >> 8)) >> 8);
							*pDest32++ = ((((color & 0xff000000) | red) | (green << 8)) | (blue << 16));
						}

						pSource32Begin = pSource32Begin + (nSourceStride / sizeof(uint32_t));
						pDest32Begin = pDest32Begin + (nDestStride / sizeof(uint32_t));
					}
				}
				else
				{
					while (pSource32Begin < pSource32End)  // For each row...
					{
						// Copy ARGB to ABGR
						const uint32_t* pSource32 = pSource32Begin;
						const uint32_t* pSource32RowEnd = pSource32Begin + nSourceSizeX;
						uint32_t*       pDest32 = pDest32Begin;

						while (pSource32 < pSource32RowEnd)
						{
							const uint32_t color = *pSource32++;
							*pDest32++ = ((color & 0xff00ff00) | ((color & 0x00ff0000) >> 16) | ((color & 0x000000ff) << 16));
						}

						pSource32Begin = pSource32Begin + (nSourceStride / sizeof(uint32_t));
						pDest32Begin = pDest32Begin + (nDestStride / sizeof(uint32_t));
					}
				}
			}
			else // RGBA
			{
				while(pSource32Begin < pSource32End)  // For each row...
				{
					// Copy ARGB to RGBA
					const uint32_t* pSource32       = pSource32Begin;
					const uint32_t* pSource32RowEnd = pSource32Begin + nSourceSizeX; 
					uint32_t*       pDest32         = pDest32Begin;

					while(pSource32 < pSource32RowEnd)
					{
						const uint32_t color = *pSource32++;
						*pDest32++ = ((color & 0x00ff0000) << 8) | ((color & 0x0000ff00) << 8) | ((color & 0x000000ff) << 8) | ((color & 0xff000000) >> 24);
					}

					pSource32Begin = pSource32Begin + (nSourceStride / sizeof(uint32_t));
					pDest32Begin   = pDest32Begin   + (nDestStride   / sizeof(uint32_t));
				}
			}
		}
		else if(nSourceFormat == Font::kBFRGBHSubpixelAA)  // 24 bit source R,G,B where each value isn't a red pixel but rather the opacity in the red channel of a pixel.
		{
			const uint8_t* pSource8Begin = (const uint8_t*)pSourceData;
			const uint8_t* pSource8End   = pSource8Begin + (nSourceSizeY * nSourceStride);

			if(nDestFormat == kTextureFormatARGB)
			{
				uint32_t* pDest32Begin  = (uint32_t*)pDest + (nDestPositionY * nDestStride / sizeof(uint32_t)) + nDestPositionX;

				// We copy R,G,B to RGBA.
				// Since we need to set the A channel to something, we set it to the average of the three channels.
				// However, you could alternatively have the fragment shader do the alpha calculation itself as-needed.
				while(pSource8Begin < pSource8End)  // For each row...
				{
					const uint8_t* pSource8       = pSource8Begin;
					const uint8_t* pSource8RowEnd = pSource8Begin + (nSourceSizeX * 3); 
					uint32_t*      pDest32        = pDest32Begin;

					while(pSource8 < pSource8RowEnd)
					{
						// We create an ARGB/RGBA value from the R,G,B triplet. We use the average of the three values 
						// as an alpha value, as these aren't really R,G,B values but rather opacity in the R,G,B channels.
						#if EATEXT_PS3_RGBA_COMPAT  // If dest is RGBA instead of ARGB...
							*pDest32++ = (pSource8[0] << 24) | (pSource8[1] << 16) | (pSource8[2] << 8) | ((pSource8[0] + pSource8[1] + pSource8[2]) / 3);
						#else // ARGB
							*pDest32++ = (pSource8[0] << 16) | (pSource8[1] << 8)  | (pSource8[2] << 0) | ((pSource8[0] + pSource8[1] + pSource8[2]) / 3) << 24;
						#endif

						pSource8 += 3;
					}

					pSource8Begin = pSource8Begin + nSourceStride;
					pDest32Begin  = pDest32Begin  + (nDestStride / sizeof(uint32_t));
				}
			}
			else if(nDestFormat == kTextureFormatRGB)
			{
				uint8_t* pDest8Begin  = (uint8_t*)pDest + (nDestPositionY * nDestStride) + nDestPositionX;

				// Copy R,G,B to RGB
				// In this case the texture is 24 bit and has no alpha channel of its own, and each texel is the packed value 
				// of an R,G,B triplet we read from the source. This R,G,B triplet is really an opacity of each of the three 
				// channels in a pixel, and the fragment shader needs to calculate an opacity for it, such as by averaging the 
				// opacity of the three values.
				// This dest format is generally better to use than kTextureFormatARGB because it saves memory for an alpha channel
				// and instead lets the fragment shader calculate the alpha at runtime, which is probably fast on today's GPUs.
				// Question: Is there any byte swapping that needs to be done?
				while(pSource8Begin < pSource8End)  // For each row...
				{
					const uint8_t* pSource8       = pSource8Begin;
					const uint8_t* pSource8RowEnd = pSource8Begin + (nSourceSizeX * 3); 

					while(pSource8 < pSource8RowEnd)
						memcpy(pDest8Begin, pSource8Begin, nSourceSizeX * 3);

					pSource8Begin = pSource8Begin + nSourceStride;
					pDest8Begin   = pDest8Begin + nDestStride;
				}
			}
			else if(nDestFormat == kTextureFormatRGB565)
			{
				uint16_t* pDest16Begin  = (uint16_t*)pDest + (nDestPositionY * nDestStride / sizeof(uint16_t)) + nDestPositionX;

				// We copy R,G,B to RGB565.
				// There is no alpha channel, so the fragment shader will need to set the alpha based on 
				// the average of the r,g,b channel values or something similar.
				while(pSource8Begin < pSource8End)  // For each row...
				{
					const uint8_t* pSource8       = pSource8Begin;
					const uint8_t* pSource8RowEnd = pSource8Begin + (nSourceSizeX * 3); 
					uint16_t*      pDest16        = pDest16Begin;

					while(pSource8 < pSource8RowEnd)
					{
						*pDest16++ = ((pSource8[0] & 0x1f) << 11) | ((pSource8[1] & 0x3f) << 5)  | (pSource8[2] & 0x1f);
						pSource8 += 3;
					}

					pSource8Begin = pSource8Begin + nSourceStride;
					pDest16Begin  = pDest16Begin  + (nDestStride / sizeof(uint16_t));
				}
			}
			else if(nDestFormat == kTextureFormatABGR)
			{
				// To do if needed.
			}
			else // RGBA
			{
				// To do if needed.
			}
		}
		else
		{
			// We can implement this when we need it fairly easily.
			EA_FAIL_MESSAGE("GlyphCache::WriteTextureArea: Unsupported source texture format.");
		}
	}
	else if(nDestFormat == kTextureFormat8Bpp)
	{
		if(nSourceFormat == Font::kBFGrayscale)
		{
			// We have 8 bit to 8 bit, so we just memcpy each row.
			const uint8_t* pSource8Begin  = (const uint8_t*)pSourceData;
			const uint8_t* pSource8End    = pSource8Begin + (nSourceSizeY * nSourceStride);
			uint8_t*       pDest8Begin    = (uint8_t*)pDest + (nDestPositionY * nDestStride / sizeof(uint8_t)) + nDestPositionX;

			while(pSource8Begin < pSource8End)
			{
				memcpy(pDest8Begin, pSource8Begin, nSourceSizeX);

				pSource8Begin = pSource8Begin + nSourceStride;
				pDest8Begin   = (uint8_t*)((uint8_t*)pDest8Begin + nDestStride);
			}
		}
		else if(nSourceFormat == Font::kBFMonochrome)
		{
			const uint8_t* pSource8Begin  = (const uint8_t*)pSourceData;
			const uint8_t* pSource8End    = pSource8Begin + (nSourceSizeY * nSourceStride);
			uint8_t*       pDest8Begin    = (uint8_t*)pDest + (nDestPositionY * nDestStride / sizeof(uint8_t)) + nDestPositionX;

			while(pSource8Begin < pSource8End)
			{
				uint32_t             nSourceMask  = 0x00000080;
				const uint8_t*       pSource8Row  = pSource8Begin;
				uint8_t*             pDest8Row    = pDest8Begin;
				const uint8_t* const pDest8RowEnd = pDest8Begin + nSourceSizeX;

				while(pDest8Row < pDest8RowEnd)
				{
					if(*pSource8Row & nSourceMask) // To consider: Make a branchless version of this logic.
						*pDest8Row = 0xff;
					else
						*pDest8Row = 0x00;

					nSourceMask >>= 1;

					if(nSourceMask == 0)
					{
						pSource8Row++;
						nSourceMask = 0x00000080;
					}

					pDest8Row++;
				}

				pSource8Begin = pSource8Begin + nSourceStride;
				pDest8Begin   = (uint8_t*)((uint8_t*)pDest8Begin + nDestStride);
			}
		}
		else
		{
			// We can implement this when we need it fairly easily.
			EA_FAIL_MESSAGE("GlyphCache::WriteTextureArea: Unsupported source texture format.");
		}
	}
	else
	{
		// We can implement this when we need it fairly easily.
		EA_FAIL_MESSAGE("GlyphCache::WriteTextureArea: Unsupported destination texture format.");
	}
}


///////////////////////////////////////////////////////////////////////////////
// ClearTextureImage
//
// Generic texture clear function.
//
void GlyphCache::ClearTextureImage(void* pDest, uint32_t nDestSize, uint32_t nDestStride, uint32_t nDestFormat)
{
	#if EATEXT_THREAD_SAFETY_ENABLED
		EA::Thread::AutoFutex autoMutex(mMutex);
	#endif

	TextureIterator dstRow(pDest, nDestStride);

	if(nDestFormat == kTextureFormatDXT3)
	{
		const int tileW = ((nDestSize + 3) / 4);
		const int tileH = ((nDestSize + 3) / 4);

		for(int y = 0; y < tileH; ++y)
		{
			uint32_t* pDestRow = (uint32_t*)dstRow++;

			// DXT1: http://msdn.microsoft.com/archive/default.asp?url=/archive/en-us/directx9_c_Feb_2006/Opaque_and_1_Bit_Alpha_Textures.asp
			// DXT3 (based on DXT1): http://msdn.microsoft.com/archive/default.asp?url=/archive/en-us/directx9_c_Feb_2006/Textures_with_Alpha_Channels.asp
			// init DXT3 tile, 16 pixels/tile
			for(int x = 0; x < tileW; ++x)
			{
				// WriteTextureArea() fills dst[0] and dst[1] with explicit 4-bit alpha (per pixel)
				pDestRow[0] = 0x00000000;    // alpha block, row 0/1 masks
				pDestRow[1] = 0x00000000;    // alpha block, row 2/3 masks
				// Set both colors to white (no color interpolation)
				pDestRow[2] = 0xffffffff;    // color block, lo/hi colors
				// Set all 2-bit bitmasks to '00', which is hi color
				pDestRow[3] = 0x00000000;    // color block, selectors
				pDestRow += 4;
			}
		}
	}
	else if(nDestFormat == kTextureFormat8Bpp)
	{
		for(uint32_t y = 0; y < nDestSize; ++y)
		{
			uint8_t* const pDestRow = (uint8_t*)dstRow++;
			memset(pDestRow, 0, nDestSize * sizeof(uint8_t));
		}
	}
	else if((nDestFormat == kTextureFormatARGB) || (nDestFormat == kTextureFormatRGBA) || (nDestFormat == kTextureFormatABGR))
	{
		for(uint32_t y = 0; y < nDestSize; ++y)
		{
			uint32_t* const pDestRow = (uint32_t*)dstRow++;
			memset(pDestRow, 0, nDestSize * sizeof(uint32_t));
		}
	}
	else
	{
		// We can implement this when we need it fairly easily.
		EA_FAIL_MESSAGE("GlyphCache::ClearTextureImage: Unsupported destination texture format.");
	}
}


///////////////////////////////////////////////////////////////////////////////
// ClearTextureInternal
//
void GlyphCache::ClearTextureInternal(TextureInfo* pTextureInfo)
{
	// We do not need to lock the mutex; the caller does this for us.
	// Disabled until the Futex::HasLock function is published with EAThread.
	//#if EATEXT_THREAD_SAFETY_ENABLED
	//    EA_ASSERT(mMutex.HasLock()); // This is an internal function, so mutex should be locked.
	//#endif

	bool bLocked = BeginUpdate(pTextureInfo); // If the return value is false, it's possible the texture was already locked, as we don't currently have support for re-entrant updates.

	if(IsUpdating(pTextureInfo)) // If BeginUpdate above returned true or if it returned false only because it was already locked...
		ClearTextureImage(pTextureInfo->mpData, pTextureInfo->mnSize, (uint32_t)pTextureInfo->mnStride, pTextureInfo->mFormat);

	if(bLocked) // If we need to match BeginUpdate with EndUpdate...
		EndUpdate(pTextureInfo);
}


///////////////////////////////////////////////////////////////////////////////
// GetGlyphTextureData
//
uint32_t GlyphCache::GetGlyphTextureData(void* /*pGlyphData*/, uint32_t /*nGlyphDataSize*/, bool /*bPortable*/)
{
	EA_FAIL_MESSAGE("GlyphCache::GetGlyphTextureData(): not implemented yet.");

	return 0;
}


///////////////////////////////////////////////////////////////////////////////
// SetGlyphTextureData
//
bool GlyphCache::SetGlyphTextureData(const void* /*pGlyphData*/, uint32_t /*nGlyphDataSize*/)
{
	EA_FAIL_MESSAGE("GlyphCache::SetGlyphTextureData(): not implemented yet.");

	return true;
}



#ifdef EA_DEBUG

	inline uint32_t Multiply(uint32_t a, uint32_t b)
	{
		uint32_t c = ((a * b) + 1);     // Multiply glyph opacity * brush opacity. +1 to deal with roundoff error.
		return ((c + (c >> 8)) >> 8);   // Divide by 255, fixed point. This is a trick shifting version of this.
	}


	void GlyphCache::EnableAutoTextureWrite(const char* pDirectory, bool bEnabled)
	{
		mbDebugBitmapEnable = bEnabled;
		EA::StdC::Strncpy(mDebugBitmapDirectory, pDirectory, sizeof(mDebugBitmapDirectory));
		mDebugBitmapDirectory[sizeof(mDebugBitmapDirectory) - 1] = 0;
	}


	bool GlyphCache::WritePPMFile(const char* pPath, uint32_t pixelFormat, const void* pData, uint32_t width, 
									uint32_t height, uint32_t strideBytes, bool bAlphaOnly, uint32_t backgroundColorRGB)
	{
		FILE* const fp = fopen(pPath, "w");

		if(fp)
		{
			const unsigned backgroundR = (backgroundColorRGB >> 16) & 0xff;
			const unsigned backgroundG = (backgroundColorRGB >>  8) & 0xff;
			const unsigned backgroundB = (backgroundColorRGB >>  0) & 0xff;

			fprintf(fp, "P3\n");
			fprintf(fp, "# %s\n", pPath);
			fprintf(fp, "%u %u\n", (unsigned)width, (unsigned)height);
			fprintf(fp, "%d\n", 255);

			for(uint32_t j = 0; j < height; j++)
			{
				for(uint32_t i = 0; i < width; i++)
				{
					switch (pixelFormat)
					{
						case kTextureFormat1Bpp:
						case kTextureFormatDXT3:
							// We don't currently have a way to write these formats.
							break;

						case kTextureFormat8Bpp:
						{
							const uint8_t* pData8 = (const uint8_t*)pData;
							const unsigned a      = *(pData8 + (j * strideBytes) + i);

							fprintf(fp, "%03u %03u %03u \t", a, a, a);

							break;
						}

						case kTextureFormatARGB:
						case kTextureFormatRGBA:
						case kTextureFormatABGR:
						{
							// We print only alpha channel.
							const uint32_t* pData32 = (const uint32_t*)pData;
							const uint32_t  pix = *(pData32 + (j * (strideBytes/4)) + i);
							unsigned        a, r, g, b;

							if(pixelFormat == kTextureFormatARGB)
							{
								a = (unsigned)((pix >> 24) & 0xff);
								r = (unsigned)((pix >> 16) & 0xff);
								g = (unsigned)((pix >>  8) & 0xff);
								b = (unsigned)((pix >>  0) & 0xff);
							}
							else if(pixelFormat == kTextureFormatABGR)
							{
								a = (unsigned)((pix >> 24) & 0xff);
								b = (unsigned)((pix >> 16) & 0xff);
								g = (unsigned)((pix >>  8) & 0xff);
								r = (unsigned)((pix >>  0) & 0xff);
							}
							else
							{
								r = (unsigned)((pix >> 24) & 0xff);
								g = (unsigned)((pix >> 16) & 0xff);
								b = (unsigned)((pix >>  8) & 0xff);
								a = (unsigned)((pix >>  0) & 0xff);
							}

							if(bAlphaOnly)
							   fprintf(fp, "%03u %03u %03u \t", a, a, a);
							else
							{
								if(a != 255) // If the pixel has any transparency...
								{
									r = Multiply(r, a);
									g = Multiply(g, a);
									b = Multiply(b, a);

									if(backgroundColorRGB != 0x00000000)
									{
										r += Multiply(backgroundR, 255 - a);
										g += Multiply(backgroundG, 255 - a);
										b += Multiply(backgroundB, 255 - a);
									}
								}

								fprintf(fp, "%03u %03u %03u \t", r, g, b);
							}

							break;
						}
					}
				}

				fprintf(fp, "\n");
			}

			fprintf(fp, "\n");
			fclose(fp);

			return true;
		}

		return false;
	}

#endif


///////////////////////////////////////////////////////////////////////////////
// GlyphCache_Memory
///////////////////////////////////////////////////////////////////////////////


GlyphCache_Memory::GlyphCache_Memory(TextureFormat textureFormat)
  : mTextureFormat(textureFormat)
{
}


///////////////////////////////////////////////////////////////////////////////
// CreateTexture
//
uintptr_t GlyphCache_Memory::CreateTexture(TextureInfo* pTextureInfo)
{
	// Create the texture. Note that pTextureInfo->mpSource points to a file path
	// (or some other kind of asset moniker) and the CreateTexture function should 
	// pay attention to that and load that texture instead of creating a new one 
	// from nothing. User-provided subclasses of GlyphCache can customize the behaviour.
	// As of this writing we haven't yet written support for loading from mpSource.
	// See the EATextRNA package's GlyphCache_RNA::CreateTexture function for an example. 

	const uint32_t bytesPerPixel = (((mTextureFormat == kTextureFormatARGB) || (mTextureFormat == kTextureFormatRGBA) || (mTextureFormat == kTextureFormatABGR)) ? 4 : 1);
	const uint32_t nTextureArea  = (pTextureInfo->mnSize * pTextureInfo->mnSize) * bytesPerPixel;

	uint8_t* const pMemory = (uint8_t*)mpCoreAllocator->Alloc(nTextureArea, EATEXT_ALLOC_PREFIX "Cache/Texture", 0);

	#if EATEXT_DEBUG
		memset(pMemory, 0, nTextureArea);
	#endif
 
	// We intentionally ignore pTextureInfo->mFormat, though if somebody 
	// needs it we can pay attention to it.
	pTextureInfo->mFormat  = mTextureFormat;
	pTextureInfo->mpData   = pMemory;
	pTextureInfo->mnStride = pTextureInfo->mnSize * bytesPerPixel;

	return (uintptr_t)pMemory;
}


bool GlyphCache_Memory::DestroyTexture(TextureInfo* pTextureInfo)
{
	if(pTextureInfo->mTexture)
	{
		const uint32_t bytesPerPixel = (((mTextureFormat == kTextureFormatARGB) || (mTextureFormat == kTextureFormatRGBA) || (mTextureFormat == kTextureFormatABGR)) ? 4 : 1);
		const uint32_t nTextureArea  = (pTextureInfo->mnSize * pTextureInfo->mnSize) * bytesPerPixel;

		mpCoreAllocator->Free((void*)pTextureInfo->mTexture, nTextureArea); 
	}

	return true;
}


bool GlyphCache_Memory::BeginUpdate(TextureInfo* /*pTextureInfo*/)
{
	// pTextureInfo->mpData and pTextureInfo->mnStride should be set already, as they never change.
	return true;
}


bool GlyphCache_Memory::EndUpdate(TextureInfo* /*pTextureInfo*/)
{
	return true;
}


void RectangleBinPack::DeleteTree(Node *node)
{
	if (node->left)
	{
		DeleteTree(node->left);
		CORE_DELETE(mpCoreAllocator, node->left);
		node->left = nullptr;
	}
	if (node->right)
	{
		DeleteTree(node->right);
		CORE_DELETE(mpCoreAllocator, node->right);
		node->right = nullptr;
	}
}


RectangleBinPack::RectangleBinPack()
{
	mRoot.left = mRoot.right = nullptr;
	mRoot.x = mRoot.y = 0;
	mnGlyphPadding = 1;
}


RectangleBinPack::~RectangleBinPack()
{
	DeleteTree(&mRoot);
}


void RectangleBinPack::Init(int width, int height, int padding)
{
	// Remove padding from the initial size so we don't push glyphs 
	// up against the texture boundary
	mBinWidth = width - padding;
	mBinHeight = height - padding;
	mnGlyphPadding = padding;
	Reset();
}


void RectangleBinPack::Reset()
{
	DeleteTree(&mRoot);
	mRoot.x = mRoot.y = 0;
	mRoot.width = mBinWidth;
	mRoot.height = mBinHeight;
}


RectangleBinPack::Node* RectangleBinPack::Insert(int width, int height)
{
	width += mnGlyphPadding;
	height += mnGlyphPadding;

	return Insert(&mRoot, width, height);
}


/// Inserts a new rectangle in the subtree rooted at the given node.
RectangleBinPack::Node* RectangleBinPack::Insert(Node *node, int width, int height)
{
	// If this node is an internal node, try both leaves for possible space.
	// (The rectangle in an internal node stores used space, the leaves store free space)
	if (node->left || node->right)
	{
		if (node->left)
		{
			Node *newNode = Insert(node->left, width, height);
			if (newNode)
				return newNode;
		}
		if (node->right)
		{
			Node *newNode = Insert(node->right, width, height);
			if (newNode)
				return newNode;
		}
		return nullptr; // Didn't fit into either subtree!
	}

	// This node is a leaf, but can we fit the new rectangle here?
	if (width > node->width || height > node->height)
		return nullptr; // Too bad, no space.

	// The new cell will fit, split the remaining space along the shorter axis,
	// that is probably more optimal.
	int w = node->width - width;
	int h = node->height - height;
	node->left = CORE_NEW(mpCoreAllocator, EATEXT_ALLOC_PREFIX "GlyphCacheNode", 0) Node;
	node->right = CORE_NEW(mpCoreAllocator, EATEXT_ALLOC_PREFIX "GlyphCacheNode", 0) Node;
	if (w <= h) // Split the remaining space in horizontal direction.
	{
		node->left->x = node->x + width;
		node->left->y = node->y;
		node->left->width = w;
		node->left->height = height;

		node->right->x = node->x;
		node->right->y = node->y + height;
		node->right->width = node->width;
		node->right->height = h;
	}
	else // Split the remaining space in vertical direction.
	{
		node->left->x = node->x;
		node->left->y = node->y + height;
		node->left->width = width;
		node->left->height = h;

		node->right->x = node->x + width;
		node->right->y = node->y;
		node->right->width = w;
		node->right->height = node->height;
	}

	// Note that as a result of the above, it can happen that node->left or node->right
	// is now a degenerate (zero area) rectangle. No need to do anything about it,
	// like remove the nodes as "unnecessary" since they need to exist as children of
	// this node (this node can't be a leaf anymore).

	// This node is now a non-leaf, so shrink its area - it now denotes
	// *occupied* space instead of free space. Its children spawn the resulting
	// area of free space.
	node->width = width;
	node->height = height;
	return node;
}


} // namespace Text

} // namespace EA




















