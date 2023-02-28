///////////////////////////////////////////////////////////////////////////////
// EATextStyle.cpp
//
// Copyright (c) 2004 Electronic Arts. All Rights Reserved.
// By Paul Pedriana, Maxis
//
///////////////////////////////////////////////////////////////////////////////


#include <EAText/EAText.h>
#include <EAText/EATextStyle.h>
#include <EAStdC/EAString.h>
#include <EAStdC/EATextUtil.h>
#include <EAStdC/EASprintf.h>
#include <EAStdC/EAHashString.h>
#include <coreallocator/icoreallocator_interface.h>
#include <coreallocator/icoreallocatormacros.h>
#include EA_ASSERT_HEADER
#include <ctype.h>
#include <stdio.h>
#include <string.h>
#include <stdlib.h>


namespace EA
{
	namespace StdC
	{
		#if (EASTDC_VERSION_N < 11500) // Older versions of EAStdC don't provide a wchar_t version ParseDelimitedText.
			#if defined(EA_WCHAR_UNIQUE) && EA_WCHAR_UNIQUE
				inline bool ParseDelimitedText(const wchar_t* pText, const wchar_t* pTextEnd, wchar_t cDelimiter, 
											   const wchar_t*& pToken, const wchar_t*& pTokenEnd, const wchar_t** ppNewText)
				{
					#if (EA_WCHAR_SIZE == 2)
						return EA::StdC::ParseDelimitedText(reinterpret_cast<const char16_t*>(pText), reinterpret_cast<const char16_t*>(pTextEnd), (char16_t)cDelimiter, 
															reinterpret_cast<const char16_t*&>(pToken),  reinterpret_cast<const char16_t*&>(pTokenEnd), reinterpret_cast<const char16_t**>(ppNewText));
					#elif (EA_WCHAR_SIZE == 4)
						return EA::StdC::ParseDelimitedText(reinterpret_cast<const char32_t*>(pText), reinterpret_cast<const char32_t*>(pTextEnd), (char32_t)cDelimiter, 
															reinterpret_cast<const char32_t*&>(pToken),  reinterpret_cast<const char32_t*&>(pTokenEnd), reinterpret_cast<const char32_t**>(ppNewText));
					#endif
				}
			#endif
		#endif
	}

namespace Text
{


	FontStyle::FontStyle()
	{
		Reset();
	}


	FontStyle::FontStyle(const Char* pFamilyNameSpec, float fSize, Style style, float weight)
	{
		Reset();

		Char familyNameSpec[256]; // Make a copy so we can 0-terminate chars in it.
		EA::StdC::Strlcpy(familyNameSpec, pFamilyNameSpec, EAArrayCount(familyNameSpec));
		pFamilyNameSpec = familyNameSpec;
		const Char* pTextEnd = EA::StdC::Strend(familyNameSpec);
		const Char* pName    = NULL;
		const Char* pNameEnd = NULL;

		for(uint32_t i = 0; i < kFamilyNameArrayCapacity; i++)
		{
			if(EA::StdC::ParseDelimitedText(pFamilyNameSpec, pTextEnd, (Char)';', pName, pNameEnd, &pFamilyNameSpec))
			{
				*const_cast<Char*>(pNameEnd) = (Char)'\0';
				EA::StdC::Strlcpy(mFamilyNameArray[i], pName, EAArrayCount(mFamilyNameArray[i]));
				pFamilyNameSpec++;
			}
			else
				break;
		}

		if(fSize > 0.f) // else leave as default.
			mfSize = fSize;
		mStyle   = style;
		mfWeight = weight;
	}


	void FontStyle::Reset()
	{
		for(uint32_t i = 0; i < kFamilyNameArrayCapacity; i++)
			mFamilyNameArray[i][0] = 0; 

		mfSize      = 12.f;
		mStyle      = kStyleDefault;
		mfWeight    = kWeightDefault;
		mVariant    = kVariantDefault;
		mPitch      = kPitchDefault;
		mSmooth     = kSmoothDefault;
		mEffect     = kEffectNone;
		mfEffectX   = 1.f;
		mfEffectY   = 1.f;
	}

	FontStyle::FontStyle(const FontStyle& fs)
	{
		operator=(fs);
	}


	FontStyle& FontStyle::operator=(const FontStyle& fs)
	{
		if(&fs != this)
		{
			mfSize      = fs.mfSize;
			mStyle      = fs.mStyle;
			mfWeight    = fs.mfWeight; 
			mVariant    = fs.mVariant;
			mPitch      = fs.mPitch;
			mSmooth     = fs.mSmooth;
			mEffect     = fs.mEffect;
			mfEffectX   = fs.mfEffectX;
			mfEffectY   = fs.mfEffectY;

			for(uint32_t i = 0; i < kFamilyNameArrayCapacity; i++)
				EA::StdC::Strlcpy(mFamilyNameArray[i], fs.mFamilyNameArray[i], kFamilyNameCapacity); 
		}

		return *this;
	}


	EATEXT_API bool operator==(const FontStyle& a, const FontStyle& b)
	{
		// Compare built-in types.
		if((a.mfSize     == b.mfSize)    &&
		   (a.mStyle     == b.mStyle)    &&
		   (a.mfWeight   == b.mfWeight)  &&
		   (a.mVariant   == b.mVariant)  &&
		   (a.mPitch     == b.mPitch)    &&
		   (a.mSmooth    == b.mSmooth)   &&
		   (a.mEffect    == b.mEffect)   &&
		   (a.mfEffectX  == b.mfEffectX) &&
		   (a.mfEffectY  == b.mfEffectY))
		{
			// Compare family name.
			for(uint32_t i = 0; i < kFamilyNameArrayCapacity; i++)
			{
				if(EA::StdC::Strcmp(a.mFamilyNameArray[i], b.mFamilyNameArray[i]) != 0)
					return false;
			}

			return true;
		}

		return false;
	}


	///////////////////////////////////////////////////////////////////////////
	// TextStyle
	///////////////////////////////////////////////////////////////////////////

	TextStyle::TextStyle()
	{
		Initialize();
	}


	TextStyle::TextStyle(const Char* pFamilyNameSpec, float fSize, Style style, float weight)
		: FontStyle(pFamilyNameSpec, fSize, style, weight)
	{
		// Initialize() over Reset(). When we call Reset, we also want to reset the 
		// FontStyle values. However, that just got set.
		Initialize();
	}


	TextStyle::TextStyle(const TextStyle& ts)
	  : FontStyle(NoInit())
	{
		operator=(ts);
	}


	void TextStyle::Initialize()
	{
		mStyleName[0] = 0;
		mParentId = 0;
		mColor = 0xffffffff;
		mColorBackground = 0x00000000;
		mEffectBaseColor = 0xffffffff;
		mEffectColor = 0xffffffff;
		mHighLightColor = 0x00000000;
		mnDecorationFlags = kDFDefault;
		mDirection = kDirectionDefault;
		mOrientation = kOrientationDefault;
		mHAlignment = kHADefault;
		mVAlignment = kVADefault;
		mJustify = kJustifyDefault;
		mTextWrap = kTWDefault;
		mTextOverflow = kTODefault;
		mEmphasisStyle = kESDefault;
		mEmphasisPosition = kEPDefault;
		mDigitSubstitution = kDSDefault;
		mPasswordMode = kPMDefault;
		mfStretch = 1.f;
		mfLetterSpacing = 0.f;
		mfWordSpacing = 0.f;
		mfLineSpace = 0.f;
	}


	void TextStyle::Reset()
	{
		FontStyle::Reset();
		Initialize();
	}


	TextStyle& TextStyle::operator=(const TextStyle& ts)
	{
		if (&ts != this)
		{
			// Explicit member-by-member init, required if this class is not a POD.
			FontStyle::operator=(ts);

			memcpy(mStyleName, ts.mStyleName, sizeof(mStyleName));
			mParentId = ts.mParentId;
			mColor = ts.mColor;
			mColorBackground = ts.mColorBackground;
			mEffectBaseColor = ts.mEffectBaseColor;
			mEffectColor = ts.mEffectColor;
			mHighLightColor = ts.mHighLightColor;
			mnDecorationFlags = ts.mnDecorationFlags;
			mDirection = ts.mDirection;
			mOrientation = ts.mOrientation;
			mHAlignment = ts.mHAlignment;
			mVAlignment = ts.mVAlignment;
			mJustify = ts.mJustify;
			mTextWrap = ts.mTextWrap;
			mTextOverflow = ts.mTextOverflow;
			mEmphasisStyle = ts.mEmphasisStyle;
			mEmphasisPosition = ts.mEmphasisPosition;
			mDigitSubstitution = ts.mDigitSubstitution;
			mPasswordMode = ts.mPasswordMode;
			mfStretch = ts.mfStretch;
			mfLetterSpacing = ts.mfLetterSpacing;
			mfWordSpacing = ts.mfWordSpacing;
			mfLineSpace = ts.mfLineSpace;
		}

		return *this;
	}


	bool operator==(const TextStyle& a, const TextStyle& b)
	{
		if(operator==((FontStyle&)a, (const FontStyle&)b)) // If the base class compares as equal...
		{
			return ((EA::StdC::Strcmp(a.mStyleName, b.mStyleName) == 0) &&
					(a.mParentId          == b.mParentId) &&
					(a.mColor             == b.mColor) &&
					(a.mColorBackground   == b.mColorBackground) &&
					(a.mnDecorationFlags  == b.mnDecorationFlags) &&
					(a.mDirection         == b.mDirection) &&
					(a.mOrientation       == b.mOrientation) &&
					(a.mHAlignment        == b.mHAlignment) &&
					(a.mVAlignment        == b.mVAlignment) &&
					(a.mJustify           == b.mJustify) &&
					(a.mTextWrap          == b.mTextWrap) &&
					(a.mTextOverflow      == b.mTextOverflow) &&
					(a.mEmphasisStyle     == b.mEmphasisStyle) &&
					(a.mEmphasisPosition  == b.mEmphasisPosition) &&
					(a.mDigitSubstitution == b.mDigitSubstitution) &&
					(a.mPasswordMode      == b.mPasswordMode) &&
					(a.mfStretch          == b.mfStretch) &&
					(a.mfLetterSpacing    == b.mfLetterSpacing) &&
					(a.mfWordSpacing      == b.mfWordSpacing) &&
					(a.mfLineSpace        == b.mfLineSpace) &&
					(a.mEffectColor       == b.mEffectColor) &&
					(a.mHighLightColor    == b.mHighLightColor));
		}

		return false;
	}

	// Compute the hash of a text style. We do this by creating a temporary digest and then
	// hashing the digest. Floats a cast to uint8_t because we don't want to hash the
	// least-significant bits. Enums are also cast to uint8_t because most of them have
	// less than 256 unique values.
	size_t TextStyleHash::operator()(const TextStyle& style) const
	{
		const size_t kMaxDigestSize = 384;
		uint8_t  digest[kMaxDigestSize];
		uint8_t* pNext = digest;

		// Copy the family name as the first part of the digest
		for(const Char* pFamily = &style.mFamilyNameArray[0][0]; *pFamily; ++pFamily) 
			*pNext++ = (uint8_t)*pFamily; // This isn't perfect, as it loses high bits from the name. In practice the name is nearly always ASCII, though.

		// Copy the numerical values. Floats and enums get converted to uint8_t because
		// we don't care about the low / high bits.
		for(const Char* pName = style.mStyleName; *pName; ++pName) 
			*pNext++ = (uint8_t)*pName; // This isn't perfect, as it loses high bits from the name. In practice the name is nearly always ASCII, though.

		*pNext++ = (uint8_t)style.mfSize;
		*pNext++ = (uint8_t)style.mStyle;
		*pNext++ = (uint8_t)style.mfWeight;
		*pNext++ = (uint8_t)style.mVariant;
		*pNext++ = (uint8_t)style.mPitch;
		*pNext++ = (uint8_t)style.mSmooth;
		memcpy(pNext, &style.mParentId, sizeof(style.mParentId));
		pNext += sizeof(style.mParentId);
		memcpy(pNext, &style.mColor, sizeof(style.mColor));
		pNext += sizeof(style.mColor);
		memcpy(pNext, &style.mColorBackground, sizeof(style.mColorBackground));
		pNext += sizeof(style.mColorBackground);
		*pNext++ = (uint8_t)style.mnDecorationFlags;
		*pNext++ = (uint8_t)style.mDirection;
		*pNext++ = (uint8_t)style.mOrientation;
		*pNext++ = (uint8_t)style.mHAlignment;
		*pNext++ = (uint8_t)style.mVAlignment;
		*pNext++ = (uint8_t)style.mJustify;
		*pNext++ = (uint8_t)style.mTextWrap;
		*pNext++ = (uint8_t)style.mTextOverflow;
		*pNext++ = (uint8_t)style.mEmphasisStyle;
		*pNext++ = (uint8_t)style.mEmphasisPosition;
		*pNext++ = (uint8_t)style.mDigitSubstitution;
		*pNext++ = (uint8_t)style.mPasswordMode;
		*pNext++ = (uint8_t)style.mfStretch;
		*pNext++ = (uint8_t)style.mfLetterSpacing;
		*pNext++ = (uint8_t)style.mfWordSpacing;
		*pNext++ = (uint8_t)style.mfLineSpace;
		EA_ASSERT((size_t)(pNext - digest) < kMaxDigestSize); // Assert that we didn't blow overflow.

		return EA::StdC::FNV1(digest, pNext - digest);
	}


#if EATEXT_STYLE_MANAGER_ENABLED


	///////////////////////////////////////////////////////////////////////////
	// StyleManager
	///////////////////////////////////////////////////////////////////////////


	StyleManager::StyleManager(Allocator::ICoreAllocator* pCoreAllocator)
	  : mpCoreAllocator(pCoreAllocator ? pCoreAllocator : EA::Text::GetAllocator()),
		mStyleMap(),
		mnNextStyleId(1)
		#if EATEXT_THREAD_SAFETY_ENABLED
	  , mMutex()
		#endif
	{
	}


	StyleManager::~StyleManager()
	{
		for (StyleMap::iterator it = mStyleMap.begin(); it != mStyleMap.end(); ++it)
		{
			TextStyleEx* const pTS = it->second;

			if(--pTS->mnRefCount == 0)
				CORE_DELETE(mpCoreAllocator, pTS);

			it->second = NULL;
		}
	}


	void StyleManager::SetAllocator(Allocator::ICoreAllocator* pCoreAllocator)
	{
		mpCoreAllocator = pCoreAllocator;
	}


	void StyleManager::Shutdown()
	{
		#if EATEXT_THREAD_SAFETY_ENABLED
			EA_ASSERT(mMutex.GetLockCount() == 0);
		#endif
		//mpCoreAllocator; // Leave mpCoreAllocator as-is.
		RemoveStyle(kStyleIdAll);
		mnNextStyleId = 1;
		EA_ASSERT(mStyleMap.empty());
	}


	const TextStyle* StyleManager::GetStyle(uint32_t nStyleId, TextStyle* pTS) const
	{
		#if EATEXT_THREAD_SAFETY_ENABLED
			EA::Thread::AutoFutex autoMutex(mMutex);
		#endif

		const StyleMap::const_iterator it = mStyleMap.find(nStyleId);

		if(it == mStyleMap.end())
			pTS = NULL;
		else
		{
			TextStyle* const pTSTemp = (*it).second;

			if(pTS)
			   *pTS = *pTSTemp; // Copy the style
			else
				pTS =  pTSTemp; // Return pointer to the style.
		}

		return pTS;
	}


	uint32_t StyleManager::AddStyle(uint32_t nStyleId, const TextStyle& ts)
	{
		#if EATEXT_THREAD_SAFETY_ENABLED
			EA::Thread::AutoFutex autoMutex(mMutex);
		#endif

		StyleMap::iterator itEnd(mStyleMap.end());
		StyleMap::iterator it(itEnd);
		TextStyleEx * pTS = NULL;  // Style with same value

		// If nStyleId is kStyleIdUnknown, then we don't even try to search mStyleMap for it; we know it is not present.
		if(nStyleId != kStyleIdUnknown)
			it = mStyleMap.find(nStyleId);

		// If the style ID doesn't already exist, or if we are changing the value of the style
		if (it == itEnd || !(*it->second == ts))
		{
			// Search for a style entry that matches the input ts.
			// Without resorting to adding data structures, we have no choice but to linearly search mStyleMap.
			for(StyleMap::iterator i2 = mStyleMap.begin(); i2 != itEnd; ++i2)
			{
				TextStyleEx* const pTSTemp = (*i2).second;

				if(*pTSTemp == ts) // If this style matches the user's style...
				{
					pTS = pTSTemp;
					if(nStyleId == kStyleIdUnknown)
						nStyleId = (*i2).first;
					break;
				}
			}
		}

		// If no matching style was found, then create a new one.
		if (pTS == NULL)
			pTS = CORE_NEW(mpCoreAllocator, EATEXT_ALLOC_PREFIX "TextStyle", 0) TextStyleEx(ts);

		if (it == itEnd)   // If there was no matching style ID
		{
			if(nStyleId == kStyleIdUnknown)
				nStyleId = GetUniqueStyleId();

			++pTS->mnRefCount;

			const StyleMap::value_type value(nStyleId, pTS);
			mStyleMap.insert(value);
		}
		else
		{
			TextStyleEx* const pTSTemp = it->second;
			++pTS->mnRefCount;
			it->second = pTS;
			if(--pTSTemp->mnRefCount == 0)
				CORE_DELETE(mpCoreAllocator, pTSTemp);
		}

		return nStyleId;
	}


	void StyleManager::RemoveStyle(uint32_t nStyleId)
	{
		#if EATEXT_THREAD_SAFETY_ENABLED
			EA::Thread::AutoFutex autoMutex(mMutex);
		#endif

		if(nStyleId == kStyleIdAll)
		{
			// To consider: We may want to kill this kStyleIdAll functionality, 
			// as it doesn't go well with our reference counting mechanism.
			for(StyleMap::iterator it(mStyleMap.begin()); it != mStyleMap.end(); ++it)
			{
				TextStyleEx* const pTS = (*it).second;

				if(--pTS->mnRefCount == 0)
					CORE_DELETE(mpCoreAllocator, pTS);
			}

			mStyleMap.clear(true);
		}
		else
		{
			const StyleMap::iterator it = mStyleMap.find(nStyleId);

			if(it != mStyleMap.end()) // If found...
			{
				TextStyleEx* const pTS = (*it).second;

				if(--pTS->mnRefCount == 0)
					CORE_DELETE(mpCoreAllocator, pTS);

				mStyleMap.erase(it);
			}
		}
	}


	uint32_t StyleManager::EnumerateStyles(uint32_t* pStyleIdArray, uint32_t nStyleIdArrayCapacity) const
	{
		#if EATEXT_THREAD_SAFETY_ENABLED
			EA::Thread::AutoFutex autoMutex(mMutex);
		#endif

		const uint32_t nStyleCount = (uint32_t)mStyleMap.size();

		if(pStyleIdArray)
		{
			if(nStyleIdArrayCapacity > nStyleCount)
				nStyleIdArrayCapacity = nStyleCount;

			for(StyleMap::const_iterator it = mStyleMap.begin(); (it != mStyleMap.end()) && (nStyleIdArrayCapacity > 0); ++it, --nStyleIdArrayCapacity)
			{
				const uint32_t nStyleId = (*it).first;
				*pStyleIdArray++ = nStyleId;
			}
		}

		return nStyleCount;
	}


	uint32_t StyleManager::GetUniqueStyleId()
	{
		// This is an imperfect system for preventing conflicts, 
		// as it can prevent current conflicts but not future conflicts.
		while(mStyleMap.find(mnNextStyleId) != mStyleMap.end())
			++mnNextStyleId;
		return mnNextStyleId++;
	}


	///////////////////////////////////////////////////////////////////////////
	// GetStyleId
	///////////////////////////////////////////////////////////////////////////

	EATEXT_API uint32_t GetStyleId(const char* pStyleName)
	{
		if(pStyleName && *pStyleName)
		{
			uint32_t c, nHash = UINT32_C(2166136261);
			while((c = (uint8_t)*pStyleName++) != 0)   // We implement an FNV1 hash here.
				nHash = (nHash * 16777619) ^ c;
			return nHash;
		}

		return 0;
	}



	///////////////////////////////////////////////////////////////////////////////
	// Deprecated functions. Superceded by TextStyleReader.
	///////////////////////////////////////////////////////////////////////////////

	namespace Parse
	{
		const uint32_t kBufferSize = 256;

		enum ParseState
		{
			kParseStateGetName,
			kParseStateGetProperties
		};

		/// GetTextLine
		/// Reads the next line of non-empty text. 
		bool GetTextLine(const char*& pText, char* pLine)
		{
			EA_ASSERT(pText && pLine);

			char*       pLineCur = pLine;
			char* const pLineEnd = pLine + kBufferSize;

			while(*pText)
			{
				if((*pText == '\n') || (*pText == '\r'))
				{
					while((*pText == '\n') || (*pText == '\r'))
						pText++;
					break;
				}
				
				if(pLineCur < (pLineEnd - 1))
					*pLineCur++ = *pText;

				pText++;
			}

			*pLineCur = 0;

			return (*pText != 0) || (*pLine != 0);
		}

		/// IsUsableLine
		/// Returns true if line is non-empty and isn't a comment line.
		bool IsUsableLine(const char* pText)
		{
			EA_ASSERT(pText);

			if(pText[0] == '\0')
				return false;
			if((pText[0] == '/') && (pText[1] == '/'))
				return false;
			return true;
		}

		/// StripSpace
		/// Removes leading and trailing space from a string.
		bool StripSpace(char* pText)
		{
			EA_ASSERT(pText);

			unsigned char* pTextBegin = (unsigned char*)pText;
			while(*pTextBegin && isspace(*pTextBegin))
				pTextBegin++;

			const size_t nStrlen = strlen((const char*)pTextBegin);
			if(pTextBegin > (unsigned char*)pText)
				memmove(pText, pTextBegin, nStrlen + 1);

			unsigned char* pTextEnd = (unsigned char*)pText + nStrlen - 1;
			while((pTextEnd >= pTextBegin) && isspace(*pTextEnd))
				*pTextEnd-- = 0;

			return (*pText != 0);
		}

		/// StripQuotes
		/// Removes leading and trailing quote marks from a string.
		bool StripQuotes(char* pText)
		{
			EA_ASSERT(pText);

			size_t nStrlen = strlen(pText);

			if(nStrlen && pText[nStrlen - 1] == '"')
				pText[--nStrlen] = 0;
			if(pText[0] == '"')
				memmove(pText, pText + 1, nStrlen * sizeof(char));

			return (*pText != 0);
		}


		/// GetProperty
		/// Expects a string like this: "font-size : 37".
		/// or this: "font-size : 37"; font-style : italic.
		/// and returns the property and value of "font-size" and "37".
		bool GetProperty(const char*& pText, char* pProperty, char* pValue)
		{
			EA_ASSERT(pText && pProperty && pValue);

			char* const pDivider = strchr((char *)pText, ':');

			if(pDivider)
			{
				size_t nLength = (size_t)(pDivider - pText);
				if(nLength >= kBufferSize)
					nLength = kBufferSize - 1;
				EA::StdC::Strncpy(pProperty, pText, nLength);
				pProperty[nLength] = 0;
				StripSpace(pProperty);

				char* const pSemicolon = strchr(pDivider, ';');

				if(pSemicolon)
				{
					nLength = (size_t)(pSemicolon - pDivider);
					if(nLength >= kBufferSize)
						nLength = kBufferSize - 1;
					EA::StdC::Strncpy(pValue, pDivider + 1, nLength);
					pValue[nLength - 1] = 0; // - 1 here because we want to get rid of the trailing ';' char.
					pText = pSemicolon + 1;
				}
				else
				{
					EA::StdC::Strncpy(pValue, pDivider + 1, kBufferSize - 1);
					pValue[kBufferSize - 1] = 0;
					pText += strlen(pText);
				}

				StripSpace(pValue);

				return true;
			}

			return false;
		}

		/// GetNameAndId
		/// Expects a string line this: "Main Menu"
		/// or this: "Main Menu (0x12345678)"
		/// and returns the name and id from it (e.g. "Main Menu" and 0x12345678)
		bool GetNameAndId(const char* pText, char* pParentName, uint32_t& parentId)
		{
			EA_ASSERT(pText && pParentName);

			uint32_t    nId = 0;
			size_t      nLength;
			char* const pId = strchr((char *)pText, '(');

			if(pId)
			{
				nId = static_cast<uint32_t>(strtoul(pId + 1, NULL, 16));
				nLength = (size_t)(pId - pText);
			}
			else
				nLength = strlen(pText);

			if(nLength >= kBufferSize)
				nLength = kBufferSize - 1;
			EA::StdC::Strncpy(pParentName, pText, nLength);
			pParentName[nLength] = 0;
			StripSpace(pParentName);

			if(*pParentName || nId)
			{
				if(!pParentName)
					EA::StdC::Sprintf(pParentName, "0x%08x", (unsigned)nId);
				else if(nId == 0)
					nId = GetStyleId(pParentName);
				parentId = nId;
				return true;
			}
			return false;
		}

		bool GetStyle(uint32_t nStyleId, const uint32_t pIdArray[], const TextStyle pTSCSSArray[], uint32_t nSize, const StyleManager* pStyleManager, TextStyle& ssCSS)
		{
			for(uint32_t i = 0; i < nSize; i++)
			{
				if(pIdArray[i] == nStyleId)
				{
					ssCSS = pTSCSSArray[i];
					return true;
				}
			}

			if(pStyleManager)
				return (pStyleManager->GetStyle(nStyleId, &ssCSS) != NULL);

			return false;
		}

		void Strncpy8ToChar(Char* pDestination, const char8_t* pSource, uint32_t n)
		{
			EA_ASSERT(pDestination && pSource);

			const char8_t* s = pSource - 1;
			Char*          d = pDestination - 1;

			n++;
			while(--n)
			{
				if((*++d = (uint8_t)*++s) == 0)
				{
					while(--n)
						*++d = 0;
					break;
				}
			}
		}

	} // namespace Parse


	using namespace Parse;

	///////////////////////////////////////////////////////////////////////////////
	// ParseStyleText
	//
	// We implement this in as modular and simple way as we can. If this 
	// implementation grows beyond some size then we may need to make it act like 
	// a more generic parsing engine.
	//
	EATEXT_API bool ParseStyleText(const char*& pCSSText, char* pStyleName, uint32_t& pId, TextStyle& pTSCSSStyle, 
								   GetStyleFunc pFunc, void* pContext, AtCommandCallback pAtCallback, void* pAtContext)
	{
		//using namespace EA::StdC;

		char       currentLine[kBufferSize];
		char       currentPropertyName[kBufferSize];
		char       currentPropertyValue[kBufferSize];
		TextStyle  currentStyle;
		ParseState currentParseState = kParseStateGetName;

		while(GetTextLine(pCSSText, currentLine))
		{
			StripSpace(currentLine);

			// An @command. If a line begins with '@' then we call the user callback
			// and otherwise ignore the line. The primary use for this is so that the 
			// user can specify font files to load which happen to support the given
			// styles specified in the file.
			if(currentLine[0] == '@')
			{
				if(pAtCallback)
					pAtCallback(currentLine + 1, pAtContext);
				continue;
			}

			if(IsUsableLine(currentLine)) // See if the line is empty or is a comment line.
			{
				switch (currentParseState)
				{
					case kParseStateGetName:
					{
						// To do: Put some debug-time code in here to verify the format.
						char* const pEnd = strchr(currentLine, '{');

						if(pEnd)
						{
							*pEnd = 0;

							char* pParent = strchr(currentLine, ':');

							if(pParent)
							{
								uint32_t parentId;
								char     parentName[256];

								*pParent++ = 0;
								if(GetNameAndId(pParent, parentName, parentId) && pFunc)
									(*pFunc)(parentId, currentStyle, pContext);
							}

							if(GetNameAndId(currentLine, pStyleName, pId))
								currentParseState = kParseStateGetProperties;
						}
						break;
					}

					case kParseStateGetProperties:
					{
						if(currentLine[0] == '}') // If we have reached the end of the current style...
						{
							pTSCSSStyle = currentStyle;
							return true;
						}
						else
						{
							const char* pText = currentLine;

							while(GetProperty(pText, currentPropertyName, currentPropertyValue))
							{
								char* pValue = currentPropertyValue;

								if(EA::StdC::Strcmp(currentPropertyName, "font-family") == 0) // (font-family: "Helvetica Neue", arial)
								{
									for(uint32_t i = 0; i < kFamilyNameArrayCapacity;)
									{
										char* const pComma = strchr(pValue, ',');
										if(pComma)
											*pComma = 0;
										if(StripSpace(pValue))
										{
											if(StripQuotes(pValue))
											{
												Strncpy8ToChar(currentStyle.mFamilyNameArray[i], pValue, kFamilyNameCapacity - 1);
												currentStyle.mFamilyNameArray[i][kFamilyNameCapacity - 1] = 0;
												i++;
											}
										}
										if(pComma)
											pValue = pComma + 1;
										else
											break;
									}
								}
								else if(EA::StdC::Strcmp(currentPropertyName, "font-size") == 0) // Format = Npx or Npt. (font-size : 12px) (font-size : 12pt)
								{
									// We aren't currently supporting named sizes.
									currentStyle.mfSize = (float)atof(pValue);
									EA_ASSERT(currentStyle.mfSize > 0.f);
								}
								else if(EA::StdC::Strcmp(currentPropertyName, "font-style") == 0)
								{
									if(EA::StdC::Strcmp(pValue, "normal") == 0)
										currentStyle.mStyle = kStyleNormal;
									else if(EA::StdC::Strcmp(pValue, "italic") == 0)
										currentStyle.mStyle = kStyleItalic;
									else if(EA::StdC::Strcmp(pValue, "oblique") == 0)
										currentStyle.mStyle = kStyleOblique;
								}
								else if(EA::StdC::Strcmp(currentPropertyName, "font-weight") == 0)
								{
									if(EA::StdC::Strcmp(pValue, "normal") == 0)
										currentStyle.mfWeight = 400;
									else if(EA::StdC::Strcmp(pValue, "bold") == 0)
										currentStyle.mfWeight = 700;
									else // We aren't currently supporting other named weights.
										currentStyle.mfWeight = (float)atof(pValue);
								}
								else if(EA::StdC::Strcmp(currentPropertyName, "font-variant") == 0)
								{
									if(EA::StdC::Strcmp(pValue, "normal") == 0)
										currentStyle.mVariant = kVariantNormal;
									else if(EA::StdC::Strcmp(pValue, "small-caps") == 0)
										currentStyle.mVariant = kVariantSmallCaps;
								}
								else if(EA::StdC::Strcmp(currentPropertyName, "font-pitch") == 0)
								{
									if(EA::StdC::Strcmp(pValue, "variable") == 0)
										currentStyle.mPitch = kPitchVariable;
									else if(EA::StdC::Strcmp(pValue, "fixed") == 0)
										currentStyle.mPitch = kPitchFixed;
								}
								else if(EA::StdC::Strcmp(currentPropertyName, "font-smooth") == 0)
								{
									// currentStyle.mSmooth
									if(EA::StdC::Strcmp(pValue, "auto") == 0)
										currentStyle.mSmooth = kSmoothDefault;
									else if(EA::StdC::Strcmp(pValue, "never") == 0)
										currentStyle.mSmooth = kSmoothNone;
									else if(EA::StdC::Strcmp(pValue, "always") == 0)
										currentStyle.mSmooth = kSmoothEnabled;
									//else
									//{   // CSS smoothing seems screwy to me. I think smoothing should occur below a given size, not above it.
									//    const float fSize = (float)atof(pValue);
									//    if(fSize > 0)
									//        currentStyle.mSmooth = (currentStyle.mfSize >= fSize) ? kSmoothEnabled : kSmoothNone;
									//}
								}
								else if(EA::StdC::Strcmp(currentPropertyName, "color") == 0) // Format = #RGB or #RGBA. (color : #cc2255) (color : #cc2255ff)
								{
									if(*pText == '#')
										pText++;

									currentStyle.mColor = 0xff000000 | EA::StdC::StrtoU32(pText, NULL, 16);
								}
								else if(EA::StdC::Strcmp(currentPropertyName, "background-color") == 0)
								{
									if(*pText == '#')
										pText++;

									currentStyle.mColorBackground = 0xff000000 | EA::StdC::StrtoU32(pText, NULL, 16);
								}
								else if(EA::StdC::Strcmp(currentPropertyName, "text-decoration") == 0) //  (text-decoration: underline, line-through)
								{
									currentStyle.mnDecorationFlags = kDFNone; // If there is a text-decoration property, then we erase any inherited properties.

									for(;;) // use a for loop to avoid compiler warnings that you'd get with while loop.
									{
										char* const pComma = strchr(pValue, ',');
										if(pComma)
											*pComma = 0;
										if(StripSpace(pValue))
										{
											if(EA::StdC::Strcmp(pValue, "underline") == 0)
												currentStyle.mnDecorationFlags |= kDFUnderline;
											else if(EA::StdC::Strcmp(pValue, "overline") == 0)
												currentStyle.mnDecorationFlags |= kDFOverline;
											else if(EA::StdC::Strcmp(pValue, "line-through") == 0)
												currentStyle.mnDecorationFlags |= kDFLineThrough;
										}
										if(pComma)
											pValue = pComma + 1;
										else
											break;
									}
								}
								else if(EA::StdC::Strcmp(currentPropertyName, "writing-mode") == 0) // lr-tb | rl-tb | tb-rl | tb-lr
								{
									// To do.
									currentStyle.mDirection   = kDirectionDefault;
									currentStyle.mOrientation = kOrientationDefault;
								}
								else if(EA::StdC::Strcmp(currentPropertyName, "text-align") == 0) // left | center | right | justify
								{
									if(EA::StdC::Strcmp(pValue, "left") == 0)
										currentStyle.mHAlignment = kHALeft;
									else if(EA::StdC::Strcmp(pValue, "center") == 0)
										currentStyle.mHAlignment = kHACenter;
									else if(EA::StdC::Strcmp(pValue, "right") == 0)
										currentStyle.mHAlignment = kHARight;
									else if(EA::StdC::Strcmp(pValue, "justify") == 0)
										currentStyle.mHAlignment = kHAJustify;
								}
								else if(EA::StdC::Strcmp(currentPropertyName, "text-valign") == 0) // top, middle, bottom
								{
									// As of this writing (2/2005), this is not a standard CSS style.
									if(EA::StdC::Strcmp(pValue, "top") == 0)
										currentStyle.mVAlignment = kVATop;
									else if(EA::StdC::Strcmp(pValue, "middle") == 0) // HTML uses "middle" instead of center.
										currentStyle.mVAlignment = kVACenter;
									else if(EA::StdC::Strcmp(pValue, "bottom") == 0)
										currentStyle.mVAlignment = kVABottom;
								}
								else if(EA::StdC::Strcmp(currentPropertyName, "text-justify") == 0) // inter-word | inter-ideograph | distribute | newspaper | inter-cluster | kashida
								{
									if(EA::StdC::Strcmp(pValue, "inter-word") == 0)
										currentStyle.mJustify = kJustifyInterWord;
									else if(EA::StdC::Strcmp(pValue, "inter-ideograph") == 0)
										currentStyle.mJustify = kJustifyInterIdeograph;
									else if(EA::StdC::Strcmp(pValue, "distribute") == 0)
										currentStyle.mJustify = kJustifyDistribute;
									else if(EA::StdC::Strcmp(pValue, "newspaper") == 0)
										currentStyle.mJustify = kJustifyNewspaper;
									else if(EA::StdC::Strcmp(pValue, "inter-cluster") == 0)
										currentStyle.mJustify = kJustifyInterCluster;
									else if(EA::StdC::Strcmp(pValue, "kashida") == 0)
										currentStyle.mJustify = kJustifyKashida;
								}
								else if(EA::StdC::Strcmp(currentPropertyName, "wrap-option") == 0) // wrap | no-wrap | soft-wrap | hard-wrap | emergency
								{
									if(EA::StdC::Strcmp(pValue, "wrap") == 0)
										currentStyle.mTextWrap = kTWWrap;
									else if(EA::StdC::Strcmp(pValue, "no-wrap") == 0)
										currentStyle.mTextWrap = kTWNone;
									else if(EA::StdC::Strcmp(pValue, "soft-wrap") == 0)
										currentStyle.mTextWrap = kTWSoft;
									else if(EA::StdC::Strcmp(pValue, "hard-wrap") == 0)
										currentStyle.mTextWrap = kTWHard;
									else if(EA::StdC::Strcmp(pValue, "emergency") == 0)
										currentStyle.mTextWrap = kTWEmergency;
								}
								else if(EA::StdC::Strcmp(currentPropertyName, "text-overflow-mode") == 0) // none | clip | ellipsis | ellipsis-word
								{
									if(EA::StdC::Strcmp(pValue, "none") == 0)
										currentStyle.mTextOverflow = kTONone;
									else if(EA::StdC::Strcmp(pValue, "clip") == 0)
										currentStyle.mTextOverflow = kTOClip;
									else if(EA::StdC::Strcmp(pValue, "ellipsis") == 0)
										currentStyle.mTextOverflow = kTOEllipsis;
								}
								else if(EA::StdC::Strcmp(currentPropertyName, "font-emphasize-style") == 0)
								{
									if(EA::StdC::Strcmp(pValue, "none") == 0)
										currentStyle.mEmphasisStyle = kESNone;
									else if(EA::StdC::Strcmp(pValue, "accent") == 0)
										currentStyle.mEmphasisStyle = kESAccent;
									else if(EA::StdC::Strcmp(pValue, "dot") == 0)
										currentStyle.mEmphasisStyle = kESDot;
									else if(EA::StdC::Strcmp(pValue, "circle") == 0)
										currentStyle.mEmphasisStyle = kESCircle;
									else if(EA::StdC::Strcmp(pValue, "disc") == 0)
										currentStyle.mEmphasisStyle = kESDisc;
								}
								else if(EA::StdC::Strcmp(currentPropertyName, "font-emphasize-position") == 0)
								{
									if(EA::StdC::Strcmp(pValue, "before") == 0)
										currentStyle.mEmphasisPosition = kEPBefore;
									else if(EA::StdC::Strcmp(pValue, "after") == 0)
										currentStyle.mEmphasisPosition = kEPAfter;
								}
								else if(EA::StdC::Strcmp(currentPropertyName, "digit-substitution") == 0) // none | context | western | national | traditional
								{
									// As of this writing (2/2005), this is not a standard CSS style.
									if(EA::StdC::Strcmp(pValue, "none") == 0)
										currentStyle.mDigitSubstitution = kDSNone;
									else if(EA::StdC::Strcmp(pValue, "context") == 0)
										currentStyle.mDigitSubstitution = kDSContext;
									else if(EA::StdC::Strcmp(pValue, "western") == 0)
										currentStyle.mDigitSubstitution = kDSWestern;
									else if(EA::StdC::Strcmp(pValue, "national") == 0)
										currentStyle.mDigitSubstitution = kDSNational;
									else if(EA::StdC::Strcmp(pValue, "traditional") == 0)
										currentStyle.mDigitSubstitution = kDSTraditional;
								}
								else if(EA::StdC::Strcmp(currentPropertyName, "password-mode") == 0) // none | password
								{
									// As of this writing (2/2005), this is not a standard CSS style.
									if(EA::StdC::Strcmp(pValue, "none") == 0)
										currentStyle.mPasswordMode = kPMNone;
									else if(EA::StdC::Strcmp(pValue, "password") == 0)
										currentStyle.mPasswordMode = kPMPassword;
								}
								else if(EA::StdC::Strcmp(currentPropertyName, "font-stretch") == 0)
								{
									// font-stretch is normally specified by "condensed", "semi-condensed", "normal", etc.
									// In practice it is often more useful for us to use floating point stretching.
									// We may want to establish a mapping between these names and stretch values.
									currentStyle.mfStretch = (float)atof(pValue);
								}
								else if(EA::StdC::Strcmp(currentPropertyName, "letter-spacing") == 0) // (letter-spacing: 12px) (letter-spacing: -0.5px)
								{
									currentStyle.mfLetterSpacing = (float)atof(pValue);
								}
								else if(EA::StdC::Strcmp(currentPropertyName, "word-spacing") == 0)
								{
									currentStyle.mfWordSpacing = (float)atof(pValue);
								}
								else if(EA::StdC::Strcmp(currentPropertyName, "line-spacing") == 0)
								{
									currentStyle.mfLineSpace = (float)atof(pValue);
									EA_ASSERT(currentStyle.mfLineSpace > 0);
								}
							}
						}

						break;
					}
				}
			}
		}
		return false;

	} // ParseStyleText()


	struct StyleCallbackContext
	{
		const StyleManager* mStyleManager;
		const uint32_t*     mIdArray;
		const TextStyle*    mTSCSSArray;
		uint32_t            mCount;
	};
	
	static bool GetStyleCallback(uint32_t nStyleId, TextStyle& result, void* pContext)
	{
		StyleCallbackContext* pStContext = (StyleCallbackContext*)pContext;

		for(uint32_t i = 0; i < pStContext->mCount; i++)
		{
			if(pStContext->mIdArray[i] == nStyleId)
			{
				result = pStContext->mTSCSSArray[i];
				return true;
			}
		}

		if(pStContext->mStyleManager)
		{
			const TextStyle* pReturnTextStyle = pStContext->mStyleManager->GetStyle(nStyleId, &result);
			return (pReturnTextStyle != NULL);
		}

		return false;
	}


	///////////////////////////////////////////////////////////////////////////////
	// ParseStyleText
	//
	// We implement this in as modular and simple way as we can. If this 
	// implementation grows beyond some size then we may need to make it act like 
	// a more generic parsing engine.
	//
	EATEXT_API uint32_t ParseStyleText(const char* pCSSText, char pStyleNameArray[][32], uint32_t pIdArray[], 
									   TextStyle pTSCSSArray[], uint32_t nArrayCapacity, const StyleManager* pStyleManager)
	{
		bool bNotDone = false;
		StyleCallbackContext context = { pStyleManager, pIdArray, pTSCSSArray, 0 };

		while (context.mCount < nArrayCapacity)
		{
			bNotDone = ParseStyleText(pCSSText, &pStyleNameArray[context.mCount][0], pIdArray[context.mCount],
										pTSCSSArray[context.mCount], GetStyleCallback, &context);
			if (!bNotDone) 
				return context.mCount;
			context.mCount++;
		}

		do
		{
			char      dummyStyleName[32];
			uint32_t  dummyStyleId;
			TextStyle dummyStyle;

			bNotDone = ParseStyleText(pCSSText, &dummyStyleName[0], dummyStyleId, dummyStyle, GetStyleCallback, &context);

		} while (bNotDone);

		return context.mCount;

	} // ParseStyleText()

#endif // #endif // EATEXT_STYLE_MANAGER_ENABLED



} // namespace Text

} // namespace EA











