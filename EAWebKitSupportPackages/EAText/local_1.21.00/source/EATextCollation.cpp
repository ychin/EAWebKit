///////////////////////////////////////////////////////////////////////////////
// EATextCollation.cpp
//
// Copyright (c) 2004 Electronic Arts. All Rights Reserved.
// By Paul Pedriana, Maxis
//
///////////////////////////////////////////////////////////////////////////////


#include <EABase/eabase.h>
#include <EAText/EATextCollation.h>
#include <EAText/internal/EATextScriptHangul.h>
#include <EASTL/bonus/sort_extra.h>
#include <EASTL/fixed_string.h>
#include <EASTL/fixed_substring.h>
#include <EASTL/fixed_hash_map.h>
#include <EASTL/hash_map.h>
#include <EASTL/vector.h>
#include <EAStdC/EAString.h>
#include <EAStdC/EAMemory.h>
#include <EAText/internal/EATextCollation.h>
#include <coreallocator/icoreallocatormacros.h>
#include "internal/CollationData.inl"


#include EA_ASSERT_HEADER

#ifdef EA_PLATFORM_WINDOWS
	EA_DISABLE_ALL_VC_WARNINGS();
	#include <Windows.h>
	EA_RESTORE_ALL_VC_WARNINGS();
#endif


///////////////////////////////////////////////////////////////////////////////
// EATEXT_OS_COLLATION_ENABLED
// 
// Defined as 0 or 1. 1 is default.
// If enabled then any OS-provided collation functionality is used instead of
// our collation functionality. The OS collation functionality is likely to 
// be more correct than ours, since such functionality is usually from a major
// OS like Windows or Unix. However, you might want to disable it if you want
// it to act the same across all platforms.
#ifndef EATEXT_OS_COLLATION_ENABLED
	#define EATEXT_OS_COLLATION_ENABLED 0
#endif


///////////////////////////////////////////////////////////////////////////////
// EATEXT_OS_COLLATION_AVAILABLE
// 
// Defined as 0 or 1. The value depends on whether the OS has its own collation
// functionality and if EATEXT_OS_COLLATION_ENABLED is enabled.
// This config option is not user-settable. It's informationational and is based
// off of other config options.
//
#if EATEXT_OS_COLLATION_ENABLED
	#if defined(EA_PLATFORM_WINDOWS)
		#define EATEXT_OS_COLLATION_AVAILABLE 1
	#else
		#define EATEXT_OS_COLLATION_AVAILABLE 0
	#endif
#else
	#define EATEXT_OS_COLLATION_AVAILABLE 0
#endif



namespace EA
{
namespace Text
{

typedef eastl::fixed_string<Char, 128, true, EA::Allocator::EASTLICoreAllocator> FixedBufferString;


struct SortByCombiningClass // : public eastl::binary_function<Char, Char, bool>
{
	bool operator()(Char a, Char b) const {
		return (GetCombiningClass(a) < GetCombiningClass(b));
	}
};


#ifdef EA_PLATFORM_WINDOWS

	// This table is borrowed from the EALocale package.
	static const char8_t* sOSLocaleTable[] = 
	{
		// "ISO^Win32LCID
		"en-us\0" "0400",      // Process or User Default Language     
		"sq-al\0" "041c",      // Albanian     
		"ar-sa\0" "0401",      // Arabic (Saudi Arabia)     
		"zh-tw\0" "0404",      // Chinese (Taiwan)     
		"zh-cn\0" "0804",      // Chinese (PRC)     
		"zh-hk\0" "0c04",      // Chinese (Hong Kong SAR, PRC)     
		"zh-sg\0" "1004",      // Chinese (Singapore)     
		"cs-cz\0" "0405",      // Czech     
		"da-dk\0" "0406",      // Danish     
		"nl-nl\0" "0413",      // Dutch (Netherlands)     
		"nl-be\0" "0813",      // Dutch (Belgium)     
		"en-us\0" "0409",      // English United States    
		"en-gb\0" "0809",      // English Great Britain (UK)    
		"fi-fi\0" "040b",      // Finnish     
		"fr-fr\0" "040c",      // French (Standard)     
		"fr-be\0" "080c",      // French (Belgian)     
		"fr-ca\0" "0c0c",      // French (Canadian)     
		"de-de\0" "0407",      // German (Standard)     
		"de-ch\0" "0807",      // German (Switzerland)     
		"el-gr\0" "0408",      // Greek     
		"iw-il\0" "040d",      // Hebrew     
		"hi-in\0" "0439",      // Hindi   
		"hu-hu\0" "040e",      // Hungarian     
		"is-is\0" "040f",      // Icelandic     
		"it-it\0" "0410",      // Italian (Standard)     
		"it-ch\0" "0810",      // Italian (Switzerland)     
		"ja-jp\0" "0411",      // Japanese     
		"ko-kp\0" "0412",      // Korean (North)    
		"ko-kr\0" "0412",      // Korean (South)    
		"no-no\0" "0414",      // Norwegian (Bokmal)     
		"pl-pl\0" "0415",      // Polish     
		"pt-br\0" "0416",      // Portuguese (Brazil)     
		"pt-pt\0" "0816",      // Portuguese (Portugal)     
		"ro-ro\0" "0418",      // Romanian     
		"ru-ru\0" "0419",      // Russian     
		"sk-sk\0" "041b",      // Slovak     
		"es-es\0" "040a",      // Spanish (Spain, Traditional Sort)     
		"es-mx\0" "080a",      // Spanish (Mexican)     
		"es-es\0" "0c0a",      // Spanish (Spain, Modern Sort)     
		"sv-se\0" "041d",      // Swedish     
		"sv-fi\0" "081d",      // Swedish (Finland)
		"th-th\0" "041e",      // Thai     
		"tr-tr\0" "041f"       // Turkish     
	};

#endif


///////////////////////////////////////////////////////////////////////////////
// GetCanonicalDecomposition
//
static const Char* GetCanonicalDecomposition(Char& c, uint32_t& count)
{
	// There are no decomposable chars below 0x00C0, and there are none
	// of interest to us above 0x30FE.

	if((c >= 0x00C0) && (c <= 0x30FE))
	{
		// To consider: Replace this with eastl::lower_bound.

		int nLower = 0;
		int nUpper = (int)(sizeof(gNFDEntryArray) / sizeof(gNFDEntryArray[0]));

		while(nLower <= nUpper)
		{
			const int nMid = (nLower + nUpper) / 2;

			if(c == gNFDEntryArray[nMid].mChar)
			{
				if(gNFDEntryArray[nMid].mD1 != 0)
				{
					count = 2;
				}
				else
				{
					count = 1;
				}
				return &gNFDEntryArray[nMid].mD0; // We should change mD0 to be an array of size 2 instead of being two successive chars.
			}
			else if(c < gNFDEntryArray[nMid].mChar)
				nUpper = nMid - 1;
			else
				nLower = nMid + 1;
		}
	}

	count = 0;
	return &c;
}


///////////////////////////////////////////////////////////////////////////////
// GetCompatibilityDecomposition
//
static const Char* GetCompatibilityDecomposition(Char& c, uint32_t& count)
{
	// There are no decomposable chars below 0x00C0, and there are none
	// of interest to us above 0x30FE.

	if((c >= 0x00A0) && (c <= 0xFFE5))
	{
		// To consider: Replace this with eastl::lower_bound.

		int nLower = 0;
		int nUpper = (int)(sizeof(gNFKDEntryArray) / sizeof(gNFKDEntryArray[0]));

		while(nLower <= nUpper)
		{
			const int nMid = (nLower + nUpper) / 2;

			if(c == gNFKDEntryArray[nMid].mChar)
			{
				if(gNFKDEntryArray[nMid].mD1 != 0)
				{
					count = 2;
				}
				else
				{
					count = 1;
				}
				return &gNFKDEntryArray[nMid].mD0; // We should change mD0 to be an array of size 2 instead of being two successive chars.
			}
			else if(c < gNFKDEntryArray[nMid].mChar)
				nUpper = nMid - 1;
			else
				nLower = nMid + 1;
		}
	}

	count = 0;
	return &c;
}


// We cheat here and instead only make the simplest of Tries which will only store strings
// of length 3. In this instance we store strings which are the compositions we care about from CollationData.inl
// in such an order that the last character is the composite of the two which preceded it, for 
// instance c1 + c2 == c3
class NFCTrie
{
public:
	static const size_t kNFCIndexCount = sizeof(gNFCIndexArray)/sizeof(gNFCIndexArray[0]);

	typedef eastl::hash_map<Char, Char, CharHash, eastl::equal_to<Char>, EA::Allocator::EASTLICoreAllocator> EntryMap;
	typedef eastl::fixed_hash_map<Char, EntryMap, kNFCIndexCount, kNFCIndexCount + 1, false, CharHash, eastl::equal_to<Char>, false, EA::Allocator::EASTLICoreAllocator> KeyMap;

	NFCTrie()
		: mRoot(EA::Allocator::EASTLICoreAllocator(EATEXT_ALLOC_PREFIX "NFCTrie::mRoot", EA::Text::GetAllocator()))
	{
		LoadList();
	}

	void LoadList()
	{
		uint32_t i = 0;
		uint32_t j = 0;
		uint32_t k = 0;

		for(; i < kNFCIndexCount; ++i)
		{
			const NFCIndex& index = gNFCIndexArray[i];
			EntryMap& newMap = mRoot[index.mC0];
			newMap.set_allocator(EA::Allocator::EASTLICoreAllocator(EATEXT_ALLOC_PREFIX "NFCTrie::mRoot", EA::Text::GetAllocator()));
			k = 0;
			
			while(k < index.mCount)
			{
				const NFCEntry& entry = gNFKCEntryArray[j + k++];
				newMap[entry.mC1] = entry.mChar;
			}

			j += index.mCount;
		}
	}

	KeyMap mRoot;
};
static NFCTrie *gpNFCTrie = NULL;


///////////////////////////////////////////////////////////////////////////////
// GetCanonicalComposition
//
static Char GetCanonicalComposition(const Char& c1, const Char& c2)
{
	EA_ASSERT_MSG(gpNFCTrie, "Ensure that EA::Text::Init() has been called before using the library.");
	NFCTrie::KeyMap::const_iterator it = gpNFCTrie->mRoot.find(c1);

	if (it == gpNFCTrie->mRoot.end())
	{
		return 0;
	}

	const NFCTrie::EntryMap& entryMap = it->second;
	NFCTrie::EntryMap::const_iterator jit = entryMap.find(c2);

	return jit == entryMap.end() ? 0 : jit->second;
}


///////////////////////////////////////////////////////////////////////////////
// IsNormalized
//
EATEXT_API bool IsNormalized(const Char* pText, size_t nTextLength, NormalizationType nt, int* pNormalizationResultFlags, const UnicodeOptions* pUnicodeOptions)
{
	// There is a potentially faster way to go about this, which is to use the "quick check" 
	// approach described in UAX #15 and via the DerivedNormalizationProps.txt file published
	// with the Unicode Standard data. 

	const size_t kBufferSize = 256;
	eastl::fixed_string<Char, kBufferSize> tempOutputString;
	if (nTextLength > kBufferSize)
	{
		tempOutputString.reserve((eastl_size_t)nTextLength);
	}
	else
	{
		tempOutputString.reserve(kBufferSize);
	}
	
	int flags;

	Normalize(pText, nTextLength, &tempOutputString[0], tempOutputString.capacity(), nt, &flags, pUnicodeOptions);

	if(pNormalizationResultFlags)
		*pNormalizationResultFlags = flags;

	return !!(flags & kNRFUnchanged);
}


///////////////////////////////////////////////////////////////////////////////
// Normalize
//
// Implement no normalization form. Simply copy the text and return the flags
// associated with it. This function is essentially a fallback that has little
// practical purpose beyond giving the caller the resulting flag information.
//
static uint32_t NormalizeNone(const Char* pTextInput,  size_t nTextInputLength,
							  Char* pTextOutput, size_t nTextOutputCapacity, 
							  NormalizationType /*nt*/, int* pNormalizationResultFlags, 
							  const UnicodeOptions* pUnicodeOptions)
{
	uint32_t i, j;
	int      flags = (kNRFAscii | kNRFUnchanged);
	uint32_t nullTermination = (pUnicodeOptions && pUnicodeOptions->mbNoNullTerminate) ? 0 : 1;

	for(i = 0, j = 0; i < nTextInputLength; ++i)
	{
		if(pTextInput[i] >= 0x0080)
			flags &= ~kNRFAscii;

		if((j + nullTermination) < nTextOutputCapacity)
			pTextOutput[j++] = pTextInput[i];
	}

	if(nullTermination && (j < nTextOutputCapacity))
		pTextOutput[j] = 0;

	if(pNormalizationResultFlags)
	   *pNormalizationResultFlags = flags;

	return i;
}


///////////////////////////////////////////////////////////////////////////////
// NormalizeD
//
// Implement normalization NFD and NFKD (decomposition forms).
//
// As with much of the rest of this Unicode library, we don't support the 
// entire Unicode character space, though we support the large majority that
// we are going to be interested in. There are a large number of characters
// in the Unicode Standard which have non-trivial decompositions but also 
// are unlikely to be seen by our users. Instead of making a huge table of
// decomposition data, we use the Expands_On_NFD section of DerivedNormalizationProps.txt
// to decide what to decompose and write manual decompositions.
//
// If you need a more full treatment of the Unicode standard, please contact
// the maintainer of this package.
//
static uint32_t NormalizeD(const Char* pTextInput,  size_t nTextInputLength,
						   Char* pTextOutput, size_t nTextOutputCapacity, 
						   NormalizationType nt, int* pNormalizationResultFlags,
						   const UnicodeOptions* pUnicodeOptions)
{
	const Char* pTextInputBegin = pTextInput;
	uint32_t i, j;
	uint32_t nullTermination = (pUnicodeOptions && pUnicodeOptions->mbNoNullTerminate) ? 0 : 1;
	int      flags = (kNRFAscii | kNRFUnchanged); // The text is ASCII and denormalized until proven otherwise.

	for(i = 0, j = 0; i < nTextInputLength; ++i, ++pTextInput)
	{
		Char c = *pTextInput;

		if(c >= 0x0080)
			flags &= ~kNRFAscii;

		// Hangul (Korean) decomposition. With Hangul, NFD and NFKD are the same,
		// so both normalization forms can share the same code here.
		if((c - 0xAC00u) <= (0xD7A4u - 0xAC00u)) // If c is composed Hangul...
		{
			c -= kHangulBegin;

			const Char cL = kJamoLBase + (c / kJamoNCount);
			const Char cV = kJamoVBase + (c % kJamoNCount) / kJamoTCount;
			const Char cT = kJamoTBase + (c % kJamoTCount);

			if((j + nullTermination) < nTextOutputCapacity)
				pTextOutput[j++] = cL;

			if((j + nullTermination) < nTextOutputCapacity)
				pTextOutput[j++] = cV;

			if(cT != kJamoTBase)
			{
				if((j + nullTermination) < nTextOutputCapacity)
					pTextOutput[j++] = cT;
			}
		}
		else
		{
			// We need to recursively apply character decomposition. By recursively, we mean that
			// char c may decompose into char c1 and c2, but either of c1 or c2 may itself decompose.
			// In practice such recursive decomposition is rarely required.
			typedef eastl::fixed_string<Char, 16, true, EA::Allocator::EASTLICoreAllocator> DString;

			DString      ds(&c, 1);
			uint32_t     count;
			eastl_size_t k = 0;
			eastl_size_t kEnd;

			while(k < ds.size()) // Size can change so we re-evaluate it every loop iteration.
			{
				const Char* const p = GetCanonicalDecomposition(ds[k], count);

				if (count > 0) // If there is a decomposition...
				{
					ds.replace(k, 1, p, count);
				}
				else
				{
					++k;
				}
			}

			// now we decompose using the compatablitiy character mappings
			if(nt == kNT_NFKD)
			{
				k = 0;
				while(k < ds.size()) // Size can change so we re-evaluate it every loop iteration.
				{
					const Char* const p = GetCompatibilityDecomposition(ds[k], count);

					if (count > 0) // If there is a decomposition...
					{
						ds.replace(k, 1, p, count);
					}
					else
					{
						++k;
					}
				}
			}

			// We need to sort the ds entries by combining class.
			if (ds.size() > 1)
			{
				eastl::bubble_sort(ds.begin(), ds.end(), SortByCombiningClass());
			}

			for(k = 0, kEnd = ds.size(); k < kEnd; k++)
			{
				if((j + nullTermination) < nTextOutputCapacity)
					pTextOutput[j++] = ds[k];
			}
		}
	}

	if(nullTermination && (j < nTextOutputCapacity))
		pTextOutput[j] = 0;

	typedef eastl::fixed_substring<Char> SubString;
	SubString decomposedString(pTextOutput);
	
	// We need to sort the final string by combining class. This is necessary
	// for cases when a series of non-starter characters are in the final string which 
	// were not inserted due to a decomposition
	SubString::iterator begin, it, end;
	for (begin = decomposedString.begin(), it = begin, end = decomposedString.end(); it != end; ++it)
	{
		int32_t cc = GetCombiningClass(*it);
		if (!cc)
		{
			if (it != begin)
			{
				// ++begin, because we don't need to sort the starter character
				eastl::bubble_sort(++begin, it, SortByCombiningClass());
			}
			begin = it;
		}
	}

	if (pNormalizationResultFlags)
	{
		// Decompsed strings usually grow in size so we short circuit this check before doing the 
		// expensive compare.
		// TODO: Implement a less expensive version see "quick check" unicode.org
		if ((nTextInputLength != j) || EA::StdC::Strncmp(pTextInputBegin, pTextOutput, nTextInputLength))
		{
			flags &= ~kNRFUnchanged;
		}

		*pNormalizationResultFlags = flags;
	}

	return j;
}


///////////////////////////////////////////////////////////////////////////////
// NormalizeC
//
// Implement normalization NFC and NFKC (composition forms).
//
// As with much of the rest of this Unicode library, we don't support the 
// entire Unicode character space, though we support the large majority that
// we are going to be interested in. There are a large number of characters
// in the Unicode Standard which have non-trivial compositions but also 
// are unlikely to be seen by our users. 
//
// If you need a more full treatment of the Unicode standard, please contact
// the maintainer of this package.
//
static uint32_t NormalizeC(const Char* pTextInput,  size_t nTextInputLength,
						   Char* pTextOutput, size_t nTextOutputCapacity, 
						   NormalizationType nt, int* pNormalizationResultFlags,
						   const UnicodeOptions* pUnicodeOptions)
{
	// Watch out for this: http://labs.spotify.com/2013/06/18/creative-usernames/

	// We first need to decompose the string
	const uint32_t decomposedLen = NormalizeD(pTextInput, nTextInputLength, pTextOutput, nTextOutputCapacity, nt == kNT_NFKC ? kNT_NFKD : kNT_NFD, pNormalizationResultFlags, pUnicodeOptions);

	uint32_t       lastStarterPos, currentPos, outputStringPos;
	uint32_t       composedStringLen;
	uint32_t       nullTermination = (pUnicodeOptions && pUnicodeOptions->mbNoNullTerminate) ? 0 : 1;
	int            flags = (kNRFAscii | kNRFUnchanged); // The text is ASCII and denormalized until proven otherwise.

	lastStarterPos    = 0;
	currentPos        = 0;
	outputStringPos   = 0;
	composedStringLen = decomposedLen;

	while(currentPos < decomposedLen)
	{      
		// Starting character used while a forming composite character
		const Char c0 = pTextOutput[currentPos++];

		if((c0 - 0x1100u) <= (0x1112u - 0x1100u)) // If c0 and c1 are decomposed Hangul...
		{
			if(currentPos < decomposedLen)
			{
				const Char c1 = pTextOutput[currentPos++];

				// We need to verify the second character is indeed one we can form a composite from
				if((c1 - 0x1161u) <= (0x1175u - 0x1161u))
				{
					const Char cL = (c0 - kJamoLBase) * kJamoNCount;
					const Char cV = (c1 - kJamoVBase) * kJamoTCount;

					if ((outputStringPos + nullTermination) < nTextOutputCapacity)
					{
						pTextOutput[outputStringPos++] = cL + cV + kHangulBegin;

						composedStringLen--;
					}

					if(currentPos < decomposedLen)
					{
						// Don't increment currentPos since if this character isn't part of the 
						// Hangul composite then it might be the starter character for the next composite in the string
						const Char c3 = pTextOutput[currentPos];
						if((c3 - 0x11A8u) <= (0x11C2u - 0x11A8u))
						{
							const Char cT = (c3 - kJamoTBase);
							pTextOutput[outputStringPos-1] += cT; 

							composedStringLen--;
						}
					}
				}
				else
				{
					// We don't have a composition so write out the two candidate characters
					pTextOutput[outputStringPos++] = c0;
					pTextOutput[outputStringPos++] = c1;
				}
			}
			else
			{
				// We don't have a composition so write out the Hangul starter character
				pTextOutput[outputStringPos++] = c0;
			}

			lastStarterPos = currentPos;
		}
		else
		{
			typedef eastl::fixed_string<Char, 16, true, EA::Allocator::EASTLICoreAllocator> CString;

			// step through the string until we find either the end or a the next starter character
			while(currentPos < decomposedLen && GetCombiningClass(pTextOutput[currentPos]) != 0)
			{
				currentPos++;
			}

			// We now have a small set of characters which are [S, ..., NS | S]
			eastl_size_t kEnd;
			eastl_size_t k = currentPos - lastStarterPos; // Initially represents the substring length
			CString cs(&pTextOutput[lastStarterPos], k);

			lastStarterPos = currentPos;

			// Now we iterate backwards to find a composite with the starter.
			while(--k > 0) // We need at least two characters to form a composite
			{
				const Char c1 = cs[k];            
				const Char composite = GetCanonicalComposition(c0, c1);

				if(composite)
				{
					// Ensure this composite is not blocked (ie, the character before
					// c has a combining class lower than its own)
					const int32_t ccc = GetCombiningClass(c1);  
					if(ccc > GetCombiningClass(cs[k-1]))
					{
						// The denomalization process would have set the flags properly for all other characters
						// so we only need to check the new composite character
						if(composite >= 0x0080)
							flags &= ~kNRFAscii;

						cs[0] = composite; 
						cs.erase(k, 1);

						// update the final composed string length
						composedStringLen--;
					}
				}
			}

			// Write out our composed string segment which will either the same size or smaller than the size we started with
			for(k = 0, kEnd = cs.size(); k < kEnd; ++k)
			{
				if ((outputStringPos + nullTermination) < decomposedLen)
				{
					pTextOutput[outputStringPos++] = cs[k];
				}
			}
		}
	}

	if(nullTermination && (composedStringLen < nTextOutputCapacity))
		pTextOutput[composedStringLen] = 0;

	if (pNormalizationResultFlags)
	{
		// Composed strings usually shrink in size so we short circuit this check before doing the 
		// expensive compare.
		// TODO: Implement a less expensive version see "quick check" unicode.org
		if ((nTextInputLength != composedStringLen) || EA::StdC::Strncmp(pTextInput, pTextOutput, nTextInputLength))
		{
			flags &= ~kNRFUnchanged;
		}
		*pNormalizationResultFlags = flags;
	}

	return composedStringLen;
	
}


///////////////////////////////////////////////////////////////////////////////
// Normalize
//
// This is the main entrypoint to Normalize.
//
EATEXT_API uint32_t Normalize(const Char* pTextInput,  size_t nTextInputLength,
							  Char* pTextOutput, size_t nTextOutputCapacity, 
							  NormalizationType nt, int* pNormalizationResultFlags,
							  const UnicodeOptions* pUnicodeOptions)
{
	if(nTextInputLength == 0xffffffff)
		nTextInputLength = (uint32_t)EA::StdC::Strlen(pTextInput);

	if((nt == kNT_NFD) || (nt == kNT_NFKD)) // Decomposition forms
		return NormalizeD(pTextInput, nTextInputLength, pTextOutput, nTextOutputCapacity, nt, pNormalizationResultFlags, pUnicodeOptions);

	if((nt == kNT_NFC) || (nt == kNT_NFKC)) // Composition forms
		return NormalizeC(pTextInput, nTextInputLength, pTextOutput, nTextOutputCapacity, nt, pNormalizationResultFlags, pUnicodeOptions);

	return NormalizeNone(pTextInput, nTextInputLength, pTextOutput, nTextOutputCapacity, nt, pNormalizationResultFlags, pUnicodeOptions);
}


/*
#include <EAIO/EAFileStream.h>
#include <EAIO/EAStreamAdapter.h>
#include <stdio.h>

static void GenerateCaseTableData()
{
	EA::IO::FileStream file(EATEXT_FILE_PATH_CHAR("C:\\temp\\UnicodeCase.txt"));
	file.Open();

	char buffer[512];
	EA::IO::size_type size;

	while((size = EA::IO::ReadLine(&file, buffer, 512)) < EA::IO::kSizeTypeDone) // While there there were more lines...
	{
		eastl::fixed_string<char, 512> s(buffer, size);
		eastl_size_t pos = 0;
		unsigned n;

		//const int kLowerCase = 12;
		const int kUpperCase = 13;
		//const int kTitleCase = 14;

		for(int j = 0; j < kUpperCase; j++)
		{
			pos = s.find(';', pos + 1);
			EA_ASSERT(pos < s.length());
		}

		int wasRead = EA::StdC::Sscanf(s.c_str() + pos + 1, "%X", &n);
		if(!wasRead)
			n = 0;
		s.sprintf("    0x%04x,\n", n);
		OutputDebugString(s.c_str());
	}

	file.Close();
}
*/

///////////////////////////////////////////////////////////////////////////////
// UpperCaseTable
//
const Char gUpperCaseTable[0x0590] = 
{
 // U+0000
 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000,
 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000,
 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000,
 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000,
 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000,
 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000,
 0x0000, 0x0041, 0x0042, 0x0043, 0x0044, 0x0045, 0x0046, 0x0047, 0x0048, 0x0049, 0x004a, 0x004b, 0x004c, 0x004d, 0x004e, 0x004f,
 0x0050, 0x0051, 0x0052, 0x0053, 0x0054, 0x0055, 0x0056, 0x0057, 0x0058, 0x0059, 0x005a, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000,
 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000,
 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000,
 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000,
 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x039c, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000,
 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000,
 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000,
 0x00c0, 0x00c1, 0x00c2, 0x00c3, 0x00c4, 0x00c5, 0x00c6, 0x00c7, 0x00c8, 0x00c9, 0x00ca, 0x00cb, 0x00cc, 0x00cd, 0x00ce, 0x00cf,
 0x00d0, 0x00d1, 0x00d2, 0x00d3, 0x00d4, 0x00d5, 0x00d6, 0x0000, 0x00d8, 0x00d9, 0x00da, 0x00db, 0x00dc, 0x00dd, 0x00de, 0x0178,

 // U+0100
 0x0000, 0x0100, 0x0000, 0x0102, 0x0000, 0x0104, 0x0000, 0x0106, 0x0000, 0x0108, 0x0000, 0x010a, 0x0000, 0x010c, 0x0000, 0x010e,
 0x0000, 0x0110, 0x0000, 0x0112, 0x0000, 0x0114, 0x0000, 0x0116, 0x0000, 0x0118, 0x0000, 0x011a, 0x0000, 0x011c, 0x0000, 0x011e,
 0x0000, 0x0120, 0x0000, 0x0122, 0x0000, 0x0124, 0x0000, 0x0126, 0x0000, 0x0128, 0x0000, 0x012a, 0x0000, 0x012c, 0x0000, 0x012e,
 0x0000, 0x0049, 0x0000, 0x0132, 0x0000, 0x0134, 0x0000, 0x0136, 0x0000, 0x0000, 0x0139, 0x0000, 0x013b, 0x0000, 0x013d, 0x0000,
 0x013f, 0x0000, 0x0141, 0x0000, 0x0143, 0x0000, 0x0145, 0x0000, 0x0147, 0x0000, 0x0000, 0x014a, 0x0000, 0x014c, 0x0000, 0x014e,
 0x0000, 0x0150, 0x0000, 0x0152, 0x0000, 0x0154, 0x0000, 0x0156, 0x0000, 0x0158, 0x0000, 0x015a, 0x0000, 0x015c, 0x0000, 0x015e,
 0x0000, 0x0160, 0x0000, 0x0162, 0x0000, 0x0164, 0x0000, 0x0166, 0x0000, 0x0168, 0x0000, 0x016a, 0x0000, 0x016c, 0x0000, 0x016e,
 0x0000, 0x0170, 0x0000, 0x0172, 0x0000, 0x0174, 0x0000, 0x0176, 0x0000, 0x0000, 0x0179, 0x0000, 0x017b, 0x0000, 0x017d, 0x0053,
 0x0243, 0x0000, 0x0000, 0x0182, 0x0000, 0x0184, 0x0000, 0x0000, 0x0187, 0x0000, 0x0000, 0x0000, 0x018b, 0x0000, 0x0000, 0x0000,
 0x0000, 0x0000, 0x0191, 0x0000, 0x0000, 0x01f6, 0x0000, 0x0000, 0x0000, 0x0198, 0x023d, 0x0000, 0x0000, 0x0000, 0x0220, 0x0000,
 0x0000, 0x01a0, 0x0000, 0x01a2, 0x0000, 0x01a4, 0x0000, 0x0000, 0x01a7, 0x0000, 0x0000, 0x0000, 0x0000, 0x01ac, 0x0000, 0x0000,
 0x01af, 0x0000, 0x0000, 0x0000, 0x01b3, 0x0000, 0x01b5, 0x0000, 0x0000, 0x01b8, 0x0000, 0x0000, 0x0000, 0x01bc, 0x0000, 0x01f7,
 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x01c4, 0x01c4, 0x0000, 0x01c7, 0x01c7, 0x0000, 0x01ca, 0x01ca, 0x0000, 0x01cd, 0x0000,
 0x01cf, 0x0000, 0x01d1, 0x0000, 0x01d3, 0x0000, 0x01d5, 0x0000, 0x01d7, 0x0000, 0x01d9, 0x0000, 0x01db, 0x018e, 0x0000, 0x01de,
 0x0000, 0x01e0, 0x0000, 0x01e2, 0x0000, 0x01e4, 0x0000, 0x01e6, 0x0000, 0x01e8, 0x0000, 0x01ea, 0x0000, 0x01ec, 0x0000, 0x01ee,
 0x0000, 0x0000, 0x01f1, 0x01f1, 0x0000, 0x01f4, 0x0000, 0x0000, 0x0000, 0x01f8, 0x0000, 0x01fa, 0x0000, 0x01fc, 0x0000, 0x01fe,

 // U+0200
 0x0000, 0x0200, 0x0000, 0x0202, 0x0000, 0x0204, 0x0000, 0x0206, 0x0000, 0x0208, 0x0000, 0x020a, 0x0000, 0x020c, 0x0000, 0x020e,
 0x0000, 0x0210, 0x0000, 0x0212, 0x0000, 0x0214, 0x0000, 0x0216, 0x0000, 0x0218, 0x0000, 0x021a, 0x0000, 0x021c, 0x0000, 0x021e,
 0x0000, 0x0000, 0x0000, 0x0222, 0x0000, 0x0224, 0x0000, 0x0226, 0x0000, 0x0228, 0x0000, 0x022a, 0x0000, 0x022c, 0x0000, 0x022e,
 0x0000, 0x0230, 0x0000, 0x0232, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x023b, 0x0000, 0x0000, 0x2c7e,
 0x2c7f, 0x0000, 0x0241, 0x0000, 0x0000, 0x0000, 0x0000, 0x0246, 0x0000, 0x0248, 0x0000, 0x024a, 0x0000, 0x024c, 0x0000, 0x024e,
 0x2c6f, 0x2c6d, 0x2c70, 0x0181, 0x0186, 0x0000, 0x0189, 0x018a, 0x0000, 0x018f, 0x0000, 0x0190, 0x0000, 0x0000, 0x0000, 0x0000,
 0x0193, 0x0000, 0x0000, 0x0194, 0x0000, 0x0000, 0x0000, 0x0000, 0x0197, 0x0196, 0x0000, 0x2c62, 0x0000, 0x0000, 0x0000, 0x019c,
 0x0000, 0x2c6e, 0x019d, 0x0000, 0x0000, 0x019f, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x2c64, 0x0000, 0x0000,
 0x01a6, 0x0000, 0x0000, 0x01a9, 0x0000, 0x0000, 0x0000, 0x0000, 0x01ae, 0x0244, 0x01b1, 0x01b2, 0x0245, 0x0000, 0x0000, 0x0000,
 0x0000, 0x0000, 0x01b7, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000,
 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000,
 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000,
 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000,
 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000,
 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000,
 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000,

 // U+0300
 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000,
 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000,
 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000,
 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000,
 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000,
 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000,
 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000,
 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000,
 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000,
 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000,
 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000,
 0x0390, 0x0391, 0x0392, 0x0393, 0x0394, 0x0395, 0x0396, 0x0397, 0x0398, 0x0399, 0x039a, 0x039b, 0x039c, 0x039d, 0x039e, 0x039f,
 0x03a0, 0x03a1, 0x03a2, 0x03a3, 0x03a4, 0x03a5, 0x03a6, 0x03a7, 0x03a8, 0x03a9, 0x03aa, 0x03ab, 0x038c, 0x038e, 0x038f, 0x0000,
 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x03d8, 0x0000, 0x03da, 0x0000, 0x03dc, 0x0000, 0x03de,
 0x0000, 0x03e0, 0x0000, 0x03e2, 0x0000, 0x03e4, 0x0000, 0x03e6, 0x0000, 0x03e8, 0x0000, 0x03ea, 0x0000, 0x03ec, 0x0000, 0x03ee,
 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x03f7, 0x0000, 0x0000, 0x03fa, 0x0000, 0x0000, 0x0000, 0x0000,

 // U+0400
 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000,
 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000,
 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000,
 0x0410, 0x0411, 0x0412, 0x0413, 0x0414, 0x0415, 0x0416, 0x0417, 0x0418, 0x0419, 0x041a, 0x041b, 0x041c, 0x041d, 0x041e, 0x041f, 
 0x0420, 0x0421, 0x0422, 0x0423, 0x0424, 0x0425, 0x0426, 0x0427, 0x0428, 0x0429, 0x042a, 0x042b, 0x042c, 0x042d, 0x042e, 0x042f,
 0x0400, 0x0401, 0x0402, 0x0403, 0x0404, 0x0405, 0x0406, 0x0407, 0x0408, 0x0409, 0x040a, 0x040b, 0x040c, 0x040d, 0x040e, 0x040f,
 0x0000, 0x0460, 0x0000, 0x0462, 0x0000, 0x0464, 0x0000, 0x0466, 0x0000, 0x0468, 0x0000, 0x046a, 0x0000, 0x046c, 0x0000, 0x046e,
 0x0000, 0x0470, 0x0000, 0x0472, 0x0000, 0x0474, 0x0000, 0x0476, 0x0000, 0x0478, 0x0000, 0x047a, 0x0000, 0x047c, 0x0000, 0x047e,
 0x0000, 0x0480, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x048a, 0x0000, 0x048c, 0x0000, 0x048e,
 0x0000, 0x0490, 0x0000, 0x0492, 0x0000, 0x0494, 0x0000, 0x0496, 0x0000, 0x0498, 0x0000, 0x049a, 0x0000, 0x049c, 0x0000, 0x049e,
 0x0000, 0x04a0, 0x0000, 0x04a2, 0x0000, 0x04a4, 0x0000, 0x04a6, 0x0000, 0x04a8, 0x0000, 0x04aa, 0x0000, 0x04ac, 0x0000, 0x04ae,
 0x0000, 0x04b0, 0x0000, 0x04b2, 0x0000, 0x04b4, 0x0000, 0x04b6, 0x0000, 0x04b8, 0x0000, 0x04ba, 0x0000, 0x04bc, 0x0000, 0x04be,
 0x0000, 0x0000, 0x04c1, 0x0000, 0x04c3, 0x0000, 0x04c5, 0x0000, 0x04c7, 0x0000, 0x04c9, 0x0000, 0x04cb, 0x0000, 0x04cd, 0x04c0,
 0x0000, 0x04d0, 0x0000, 0x04d2, 0x0000, 0x04d4, 0x0000, 0x04d6, 0x0000, 0x04d8, 0x0000, 0x04da, 0x0000, 0x04dc, 0x0000, 0x04de,
 0x0000, 0x04e0, 0x0000, 0x04e2, 0x0000, 0x04e4, 0x0000, 0x04e6, 0x0000, 0x04e8, 0x0000, 0x04ea, 0x0000, 0x04ec, 0x0000, 0x04ee,
 0x0000, 0x04f0, 0x0000, 0x04f2, 0x0000, 0x04f4, 0x0000, 0x04f6, 0x0000, 0x04f8, 0x0000, 0x04fa, 0x0000, 0x04fc, 0x0000, 0x04fe,

 // U+0500
 0x0000, 0x0500, 0x0000, 0x0502, 0x0000, 0x0504, 0x0000, 0x0506, 0x0000, 0x0508, 0x0000, 0x050a, 0x0000, 0x050c, 0x0000, 0x050e, 
 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 
 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 
 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 
 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 
 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 
 0x0000, 0x0531, 0x0532, 0x0533, 0x0534, 0x0535, 0x0536, 0x0537, 0x0538, 0x0539, 0x053a, 0x053b, 0x053c, 0x053d, 0x053e, 0x053f, 
 0x0540, 0x0541, 0x0542, 0x0543, 0x0544, 0x0545, 0x0546, 0x0547, 0x0548, 0x0549, 0x054a, 0x054b, 0x054c, 0x054d, 0x054e, 0x054f, 
 0x0550, 0x0551, 0x0552, 0x0553, 0x0554, 0x0555, 0x0556, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000
};


///////////////////////////////////////////////////////////////////////////////
// LowerCaseTable
//
const Char gLowerCaseTable[0x0590] = 
{
 // U+0000
 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000,
 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000,
 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000,
 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000,
 0x0000, 0x0061, 0x0062, 0x0063, 0x0064, 0x0065, 0x0066, 0x0067, 0x0068, 0x0069, 0x006a, 0x006b, 0x006c, 0x006d, 0x006e, 0x006f,
 0x0070, 0x0071, 0x0072, 0x0073, 0x0074, 0x0075, 0x0076, 0x0077, 0x0078, 0x0079, 0x007a, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000,
 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000,
 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000,
 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000,
 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000,
 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000,
 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000,
 0x00e0, 0x00e1, 0x00e2, 0x00e3, 0x00e4, 0x00e5, 0x00e6, 0x00e7, 0x00e8, 0x00e9, 0x00ea, 0x00eb, 0x00ec, 0x00ed, 0x00ee, 0x00ef,
 0x00f0, 0x00f1, 0x00f2, 0x00f3, 0x00f4, 0x00f5, 0x00f6, 0x0000, 0x00f8, 0x00f9, 0x00fa, 0x00fb, 0x00fc, 0x00fd, 0x00fe, 0x0000,
 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000,
 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000,

 // U+0100
 0x0101, 0x0000, 0x0103, 0x0000, 0x0105, 0x0000, 0x0107, 0x0000, 0x0109, 0x0000, 0x010b, 0x0000, 0x010d, 0x0000, 0x010f, 0x0000,
 0x0111, 0x0000, 0x0113, 0x0000, 0x0115, 0x0000, 0x0117, 0x0000, 0x0119, 0x0000, 0x011b, 0x0000, 0x011d, 0x0000, 0x011f, 0x0000,
 0x0121, 0x0000, 0x0123, 0x0000, 0x0125, 0x0000, 0x0127, 0x0000, 0x0129, 0x0000, 0x012b, 0x0000, 0x012d, 0x0000, 0x012f, 0x0000,
 0x0069, 0x0000, 0x0133, 0x0000, 0x0135, 0x0000, 0x0137, 0x0000, 0x0000, 0x013a, 0x0000, 0x013c, 0x0000, 0x013e, 0x0000, 0x0140,
 0x0000, 0x0142, 0x0000, 0x0144, 0x0000, 0x0146, 0x0000, 0x0148, 0x0000, 0x0000, 0x014b, 0x0000, 0x014d, 0x0000, 0x014f, 0x0000,
 0x0151, 0x0000, 0x0153, 0x0000, 0x0155, 0x0000, 0x0157, 0x0000, 0x0159, 0x0000, 0x015b, 0x0000, 0x015d, 0x0000, 0x015f, 0x0000,
 0x0161, 0x0000, 0x0163, 0x0000, 0x0165, 0x0000, 0x0167, 0x0000, 0x0169, 0x0000, 0x016b, 0x0000, 0x016d, 0x0000, 0x016f, 0x0000,
 0x0171, 0x0000, 0x0173, 0x0000, 0x0175, 0x0000, 0x0177, 0x0000, 0x00ff, 0x017a, 0x0000, 0x017c, 0x0000, 0x017e, 0x0000, 0x0000,
 0x0000, 0x0253, 0x0183, 0x0000, 0x0185, 0x0000, 0x0254, 0x0188, 0x0000, 0x0256, 0x0257, 0x018c, 0x0000, 0x0000, 0x01dd, 0x0259,
 0x025b, 0x0192, 0x0000, 0x0260, 0x0263, 0x0000, 0x0269, 0x0268, 0x0199, 0x0000, 0x0000, 0x0000, 0x026f, 0x0272, 0x0000, 0x0275,
 0x01a1, 0x0000, 0x01a3, 0x0000, 0x01a5, 0x0000, 0x0280, 0x01a8, 0x0000, 0x0283, 0x0000, 0x0000, 0x01ad, 0x0000, 0x0288, 0x01b0,
 0x0000, 0x028a, 0x028b, 0x01b4, 0x0000, 0x01b6, 0x0000, 0x0292, 0x01b9, 0x0000, 0x0000, 0x0000, 0x01bd, 0x0000, 0x0000, 0x0000,
 0x0000, 0x0000, 0x0000, 0x0000, 0x01c6, 0x01c6, 0x0000, 0x01c9, 0x01c9, 0x0000, 0x01cc, 0x01cc, 0x0000, 0x01ce, 0x0000, 0x01d0,
 0x0000, 0x01d2, 0x0000, 0x01d4, 0x0000, 0x01d6, 0x0000, 0x01d8, 0x0000, 0x01da, 0x0000, 0x01dc, 0x0000, 0x0000, 0x01df, 0x0000,
 0x01e1, 0x0000, 0x01e3, 0x0000, 0x01e5, 0x0000, 0x01e7, 0x0000, 0x01e9, 0x0000, 0x01eb, 0x0000, 0x01ed, 0x0000, 0x01ef, 0x0000,
 0x0000, 0x01f3, 0x01f3, 0x0000, 0x01f5, 0x0000, 0x0195, 0x01bf, 0x01f9, 0x0000, 0x01fb, 0x0000, 0x01fd, 0x0000, 0x01ff, 0x0000,

 // U+0200
 0x0201, 0x0000, 0x0203, 0x0000, 0x0205, 0x0000, 0x0207, 0x0000, 0x0209, 0x0000, 0x020b, 0x0000, 0x020d, 0x0000, 0x020f, 0x0000,
 0x0211, 0x0000, 0x0213, 0x0000, 0x0215, 0x0000, 0x0217, 0x0000, 0x0219, 0x0000, 0x021b, 0x0000, 0x021d, 0x0000, 0x021f, 0x0000,
 0x019e, 0x0000, 0x0223, 0x0000, 0x0225, 0x0000, 0x0227, 0x0000, 0x0229, 0x0000, 0x022b, 0x0000, 0x022d, 0x0000, 0x022f, 0x0000,
 0x0231, 0x0000, 0x0233, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x2c65, 0x023c, 0x0000, 0x019a, 0x2c66, 0x0000,
 0x0000, 0x0242, 0x0000, 0x0180, 0x0289, 0x028c, 0x0247, 0x0000, 0x0249, 0x0000, 0x024b, 0x0000, 0x024d, 0x0000, 0x024f, 0x0000,
 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000,
 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000,
 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000,
 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000,
 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000,
 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000,
 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000,
 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000,
 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000,
 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000,
 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000,

 // U+0300 (Greek and Coptic)
 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000,
 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000,
 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000,
 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000,
 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000,
 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000,
 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000,
 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000,
 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x03ac, 0x0000, 0x03ad, 0x03ae, 0x03af, 0x0000, 0x03cc, 0x0000, 0x03cd, 0x03ce,
 0x03b0, 0x03b1, 0x03b2, 0x03b3, 0x03b4, 0x03b5, 0x03b6, 0x03b7, 0x03b8, 0x03b9, 0x03ba, 0x03bb, 0x03bc, 0x03bd, 0x03be, 0x03bf,
 0x03c0, 0x03c1, 0x03c2, 0x03c3, 0x03c4, 0x03c5, 0x03c6, 0x03c7, 0x03c8, 0x03c9, 0x03ca, 0x03cb, 0x0000, 0x0000, 0x0000, 0x0000,
 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000,
 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000,
 0x03d1, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x03d9, 0x0000, 0x03db, 0x0000, 0x03dd, 0x0000, 0x03df, 0x0000,
 0x03e1, 0x0000, 0x03e3, 0x0000, 0x03e5, 0x0000, 0x03e7, 0x0000, 0x03e9, 0x0000, 0x03eb, 0x0000, 0x03ed, 0x0000, 0x03ef, 0x0000,
 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x03f8, 0x0000, 0x0000, 0x03fb, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000,

 // U+0400 (Cyrillic)
 0x0450, 0x0451, 0x0452, 0x0453, 0x0454, 0x0455, 0x0456, 0x0457, 0x0458, 0x0459, 0x045a, 0x045b, 0x045c, 0x045d, 0x045e, 0x045f, 
 0x0430, 0x0431, 0x0432, 0x0433, 0x0434, 0x0435, 0x0436, 0x0437, 0x0438, 0x0439, 0x043a, 0x043b, 0x043c, 0x043d, 0x043e, 0x043f, 
 0x0440, 0x0441, 0x0442, 0x0443, 0x0444, 0x0445, 0x0446, 0x0447, 0x0448, 0x0449, 0x044a, 0x044b, 0x044c, 0x044d, 0x044e, 0x044f, 
 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000,
 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000,
 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000,
 0x0461, 0x0000, 0x0463, 0x0000, 0x0465, 0x0000, 0x0467, 0x0000, 0x0469, 0x0000, 0x046b, 0x0000, 0x046d, 0x0000, 0x046f, 0x0000,
 0x0471, 0x0000, 0x0473, 0x0000, 0x0475, 0x0000, 0x0477, 0x0000, 0x0479, 0x0000, 0x047b, 0x0000, 0x047d, 0x0000, 0x047f, 0x0000,
 0x0481, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x048b, 0x0000, 0x048d, 0x0000, 0x048f, 0x0000,
 0x0491, 0x0000, 0x0493, 0x0000, 0x0495, 0x0000, 0x0497, 0x0000, 0x0499, 0x0000, 0x049b, 0x0000, 0x049d, 0x0000, 0x049f, 0x0000,
 0x04a1, 0x0000, 0x04a3, 0x0000, 0x04a5, 0x0000, 0x04a7, 0x0000, 0x04a9, 0x0000, 0x04ab, 0x0000, 0x04ad, 0x0000, 0x04af, 0x0000,
 0x04b1, 0x0000, 0x04b3, 0x0000, 0x04b5, 0x0000, 0x04b7, 0x0000, 0x04b9, 0x0000, 0x04bb, 0x0000, 0x04bd, 0x0000, 0x04bf, 0x0000,
 0x04cf, 0x04c2, 0x0000, 0x04c4, 0x0000, 0x04c6, 0x0000, 0x04c8, 0x0000, 0x04ca, 0x0000, 0x04cc, 0x0000, 0x04ce, 0x0000, 0x0000,
 0x04d1, 0x0000, 0x04d3, 0x0000, 0x04d5, 0x0000, 0x04d7, 0x0000, 0x04d9, 0x0000, 0x04db, 0x0000, 0x04dd, 0x0000, 0x04df, 0x0000,
 0x04e1, 0x0000, 0x04e3, 0x0000, 0x04e5, 0x0000, 0x04e7, 0x0000, 0x04e9, 0x0000, 0x04eb, 0x0000, 0x04ed, 0x0000, 0x04ef, 0x0000,
 0x04f1, 0x0000, 0x04f3, 0x0000, 0x04f5, 0x0000, 0x04f7, 0x0000, 0x04f9, 0x0000, 0x04fb, 0x0000, 0x04fd, 0x0000, 0x04ff, 0x0000,

 // U+0500
 0x0501, 0x0000, 0x0503, 0x0000, 0x0505, 0x0000, 0x0507, 0x0000, 0x0509, 0x0000, 0x050b, 0x0000, 0x050d, 0x0000, 0x050f, 0x0000,
 0x0511, 0x0000, 0x0513, 0x0000, 0x0515, 0x0000, 0x0517, 0x0000, 0x0519, 0x0000, 0x051b, 0x0000, 0x051d, 0x0000, 0x051f, 0x0000,
 0x0521, 0x0000, 0x0523, 0x0000, 0x0525, 0x0000, 0x0561, 0x0562, 0x0563, 0x0564, 0x0565, 0x0566, 0x0567, 0x0568, 0x0569, 0x056a,
 0x056b, 0x056c, 0x056d, 0x056e, 0x056f, 0x0570, 0x0571, 0x0572, 0x0573, 0x0574, 0x0575, 0x0576, 0x0577, 0x0578, 0x0579, 0x057a,
 0x057b, 0x057c, 0x057d, 0x057e, 0x057f, 0x0580, 0x0581, 0x0582, 0x0583, 0x0584, 0x0585, 0x0586, 0x0000, 0x0000, 0x0000, 0x0000,
 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000,
 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000,
 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000,
 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000
};


///////////////////////////////////////////////////////////////////////////////
// GetCase
//
EATEXT_API CaseType GetCase(Char c)
{
	if(c < 0x0590) // Chars above this have no case that we recognize
	{
		const Char lc = gLowerCaseTable[c];
		if(lc)  // If there is a lower case version of this character...
			return kCaseTypeUpper;

		const Char uc = gUpperCaseTable[c];
		if(uc) // If there is an upper case version of this character...
			return kCaseTypeLower;
	}

	return kCaseTypeNone;
}


///////////////////////////////////////////////////////////////////////////////
// ConvertCaseLower
//
static uint32_t ConvertCaseLower(const Char* pTextInput, size_t nTextLength, 
								 Char* pTextOutput, size_t nTextOutputCapacity, 
								 const UnicodeOptions* pUnicodeOptions)
{
	EA_ASSERT(pTextInput != NULL);
	uint32_t i, j;
	uint32_t nullTermination = (pUnicodeOptions && pUnicodeOptions->mbNoNullTerminate) ? 0 : 1;

	for(i = 0, j = 0; i != nTextLength; i++)
	{
		Char c = pTextInput[i];

		// Write hard-coded logic for the most common case.
		// We have this logic here in order to avoid a possible memory cache hit to the table.
		if(c < 0x005b)
		{
			if(c > 0x0040)
				c += 0x0020;
			// else no change.
		}
		else if(c < 0x0590) // chars above this have no case that we recognize
		{
			if(gLowerCaseTable[c])
				c = gLowerCaseTable[c];
		}
		// else no change.

		if((j + nullTermination) < nTextOutputCapacity) // If there's enough space to write this char plus a 0-terminator at the end...
			pTextOutput[j++] = c;
	}

	if(nullTermination && (j < nTextOutputCapacity))
		pTextOutput[j] = 0;

	return i; // Return the required strlen.
}


///////////////////////////////////////////////////////////////////////////////
// ConvertCaseUpper
//
static uint32_t ConvertCaseUpper(const Char* pTextInput, size_t nTextLength, 
								 Char* pTextOutput, size_t nTextOutputCapacity, 
								 const UnicodeOptions* pUnicodeOptions)
{
	EA_ASSERT(pTextInput != NULL);
	uint32_t i, j;
	uint32_t nullTermination = (pUnicodeOptions && pUnicodeOptions->mbNoNullTerminate) ? 0 : 1;

	for(i = 0, j = 0; i != nTextLength; i++)
	{
		Char c = pTextInput[i];

		// Write hard-coded logic for the most common case.
		// We have this logic here in order to avoid a possible memory cache hit to the table.
		if(c > 0x0060)
		{
			if(c < 0x007b)      // If ASCII, just subtract 0x20.
				c -= 0x0020;
			else if(c < 0x0590) // If within our table lookup...
			{
				if(gUpperCaseTable[c]) // If the table has a conversion...
					c = gUpperCaseTable[c];
			}
			// else no change, as chars above 0x0590 have no case that we recognize.
		}

		if((j + nullTermination) < nTextOutputCapacity) // If there's enough space to write this char plus a 0-terminator at the end...
			pTextOutput[j++] = c;
	}

	if(nullTermination && (j < nTextOutputCapacity))
		pTextOutput[j] = 0;

	return i; // Return the required strlen.
}


///////////////////////////////////////////////////////////////////////////////
// ConvertCaseTitle
//
static uint32_t ConvertCaseTitle(const Char* pTextInput, size_t nTextLength, 
								 Char* pTextOutput, size_t nTextOutputCapacity, 
								 const UnicodeOptions* pUnicodeOptions)
{
	EA_ASSERT(pTextInput != NULL);
	uint32_t i, j;
	uint32_t nullTermination = (pUnicodeOptions && pUnicodeOptions->mbNoNullTerminate) ? 0 : 1;
	bool     prevIsSpace = true;
	bool     currentIsSpace = true;

	for(i = 0, j = 0; i != nTextLength; i++)
	{
		prevIsSpace    = currentIsSpace;
		currentIsSpace = IsSpace(pTextInput[i], kSTAll, false);

		if((j + nullTermination) < nTextOutputCapacity) // If there's enough space to write this char plus a 0-terminator at the end...
		{
			if(prevIsSpace && !currentIsSpace)
				pTextOutput[j++] = ToUpper(pTextInput[i]);
			else
				pTextOutput[j++] = ToLower(pTextInput[i]);
		}
	}

	if(nullTermination && (j < nTextOutputCapacity))
		pTextOutput[j] = 0;

	return i; // Return the required strlen.
}


///////////////////////////////////////////////////////////////////////////////
// ConvertCaseReverse
//
static uint32_t ConvertCaseReverse(const Char* pTextInput, size_t nTextLength, 
								   Char* pTextOutput, size_t nTextOutputCapacity, 
								   const UnicodeOptions* pUnicodeOptions)
{
	EA_ASSERT(pTextInput != NULL);
	uint32_t i, j;
	uint32_t nullTermination = (pUnicodeOptions && pUnicodeOptions->mbNoNullTerminate) ? 0 : 1;

	for(i = 0, j = 0; i != nTextLength; i++)
	{
		if((j + nullTermination) < nTextOutputCapacity)
		{
			CaseType caseType = GetCase(pTextInput[i]);

			if(caseType == kCaseTypeLower)
				pTextOutput[j++] = ToUpper(pTextInput[i]);
			else if(caseType == kCaseTypeUpper)
				pTextOutput[j++] = ToLower(pTextInput[i]);
			else
				pTextOutput[j++] = pTextInput[i];
		}
	}

	if(nullTermination && (j < nTextOutputCapacity))
		pTextOutput[j] = 0;

	return i;
}


EATEXT_API uint32_t ConvertCase(const Char* pTextInput, size_t nTextLength, 
								Char* pTextOutput, size_t nTextOutputCapacity, CaseType caseType, 
								const UnicodeOptions* pUnicodeOptions)
{
	switch (caseType)
	{
		case kCaseTypeLower:
			return ConvertCaseLower(pTextInput, nTextLength, pTextOutput, nTextOutputCapacity, pUnicodeOptions);

		case kCaseTypeUpper:
			return ConvertCaseUpper(pTextInput, nTextLength, pTextOutput, nTextOutputCapacity, pUnicodeOptions);

		case kCaseTypeTitle:
			return ConvertCaseTitle(pTextInput, nTextLength, pTextOutput, nTextOutputCapacity, pUnicodeOptions);

		case kCaseTypeReverse:
			return ConvertCaseReverse(pTextInput, nTextLength, pTextOutput, nTextOutputCapacity, pUnicodeOptions);

		case kCaseTypeNone:
		default:
		{
			uint32_t i, j;
			uint32_t nullTermination = (pUnicodeOptions && pUnicodeOptions->mbNoNullTerminate) ? 0 : 1;

			for(i = 0, j = 0; i != nTextLength; i++)
			{
				if((j + nullTermination) < nTextOutputCapacity)
					pTextOutput[j++] = pTextInput[i];
			}

			if(nullTermination && (j < nTextOutputCapacity))
				pTextOutput[j] = 0;

			return i;
		}
	}
}


EATEXT_API uint32_t ConvertCase(const Char* pTextInput, Char* pTextOutput, size_t nTextOutputCapacity, CaseType caseType, const UnicodeOptions* pUnicodeOptions)
{
	const uint32_t nTextLength = (uint32_t)EA::StdC::Strlen(pTextInput);

	return ConvertCase(pTextInput, nTextLength, pTextOutput, nTextOutputCapacity, caseType, pUnicodeOptions);
}


EATEXT_API Char ToLower(Char c)
{
	Char cResult[4]; // Use [4] in case ConvertCase somehow results in more than one char (couldn't possibly need to be bigger than 4).
	ConvertCase(&c, 1, &cResult[0], 4, kCaseTypeLower);
	return cResult[0]; 
}


EATEXT_API Char ToUpper(Char c)
{
	Char cResult[4]; // Use [4] in case ConvertCase somehow results in more than one char (couldn't possibly need to be bigger than 4).
	ConvertCase(&c, 1, &cResult[0], 4, kCaseTypeUpper);
	return cResult[0]; 
}





///////////////////////////////////////////////////////////////////////////////
// Collator
///////////////////////////////////////////////////////////////////////////////

Collator::Collator(bool /*bEnableCache*/, Allocator::ICoreAllocator* /*pAllocator*/)
  : mSpecialCharOption(kSCONone)
  , mAccentSortOption(kASONone)
  , mbIgnoreCase(false)
  , mbLocaleFirst(false)
  , mbDigitsAsNumbers(false)
  #ifdef EA_PLATFORM_WINDOWS
  , mLCID(LOCALE_USER_DEFAULT)
  #endif
{
	mLocale[0] = 0;
	mCollEntry1Array = gDefaultCollEntry1Array;
	mCollEntry2Array = gDefaultCollEntry2Array;
	mCollEntry3Array = gDefaultCollEntry3Array;
	mCollEntry1ArraySize = EAArrayCount(gDefaultCollEntry1Array);
	mCollEntry2ArraySize = EAArrayCount(gDefaultCollEntry2Array);
	mCollEntry3ArraySize = EAArrayCount(gDefaultCollEntry3Array);
}


void Collator::SetLocale(const char* pLocale)
{
	#ifdef EA_PLATFORM_WINDOWS
		using namespace EA::StdC;

		mLCID = LOCALE_USER_DEFAULT;

		const size_t kTableSize = sizeof(sOSLocaleTable) / sizeof(sOSLocaleTable[0]);
		size_t i = 0;

		for(i = 0; i < kTableSize; ++i)
		{
			if(Stricmp(pLocale, sOSLocaleTable[i]) == 0)
			{
				mLCID = StrtoU32(sOSLocaleTable[i] + 6, NULL, 16);
				break;
			}
		}

		if(i == kTableSize)
		{
			for(i = 0; i < kTableSize; ++i)
			{
				if(Strnicmp(pLocale, sOSLocaleTable[i], 2) == 0)
				{
					mLCID = StrtoU32(sOSLocaleTable[i] + 6, NULL, 16);
					break;
				}
			}
		}
	#else
		EA_UNUSED(pLocale);
	#endif
}


void Collator::EnableCache(bool /*bEnableCache*/, Allocator::ICoreAllocator* /*pAllocator*/, uint32_t /*maxMemoryUsage*/)
{

}


void Collator::SetSortOption(SortOption option, int value)
{
	switch(option)
	{
		case kSONone:
			break;

		case kSOSpecialChar:
			mSpecialCharOption = (SpecialCharOption)value;
			break;

		case kSOAccentSort:
			mAccentSortOption = (AccentSortOption)value;
			break;

		case kSOIgnoreCase:
			mbIgnoreCase = (value != 0);
			break;

		case kSOLocaleFirst:
			mbLocaleFirst = (value != 0);
			break;

		case kSODigitsAsNumbers:
			mbDigitsAsNumbers = (value != 0);
			break;
	}
}

void Collator::SetSpecialCharacterSortOption(SpecialCharOption option)
{
	mSpecialCharOption = option;
}

namespace Internal
{
	int SortKeyCompare(const Char* pString1, const eastl_size_t sLen1, const Char* pString2, const eastl_size_t sLen2)
	{
		Char c1, c2;
		eastl_size_t pos = 0;
		eastl_size_t minLen = sLen1 < sLen2 ? sLen1 : sLen2;

		while((c1 = pString1[pos]) == (c2 = pString2[pos]))
		{
			if(++pos >= minLen)
			{
				if(sLen1 == sLen2)
				{
					return 0;
				}
				else if(sLen1 < sLen2)
				{
					return -1;
				}
				else
				{
					return 1;
				}
			}
		}

		return (c1 - c2);
	}

} // namespace


///////////////////////////////////////////////////////////////////////////////
// FindCollationElements
//
template <class T>
static const T* FindCollationElements(const Char* pC, T* collEntryArray, size_t arraySize)
{
	const Char c = *pC;

	if((c >= 0x0009) && (c <= 0xFFFD))
	{
		// To consider: Replace this with eastl::lower_bound.
		int nLower = 0;
		int nUpper = (int) arraySize;

		while(nLower <= nUpper)
		{
			const int nMid = (nLower + nUpper) / 2;

			if(c == collEntryArray[nMid].mChar)
			{
				return &collEntryArray[nMid];
			}
			else if(c < collEntryArray[nMid].mChar)
				nUpper = nMid - 1;
			else
				nLower = nMid + 1;
		}
	}


	return 0;
}

static bool IsCharacterVariablyWeighted(const Char* pC, VariableWeightedCharacter& vwc)
{
	const Char c = *pC;

	if((c >= 0x0009) && (c <= 0xFFFD))
	{
		// To consider: Replace this with eastl::lower_bound.
		int nLower = 0;
		int nUpper = EAArrayCount(gVariableWeightArray);

		while(nLower <= nUpper)
		{
			const int nMid = (nLower + nUpper) / 2;

			if(c == gVariableWeightArray[nMid].mChar)
			{
				vwc = gVariableWeightArray[nMid];
				return true;
			}
			else if(c < gVariableWeightArray[nMid].mChar)
				nUpper = nMid - 1;
			else
				nLower = nMid + 1;
		}
	}

	return false;
}

Collator::CollationMultiElementMap::CollationMultiElementMap()
{
	LoadDefaultList();
}

void Collator::CollationMultiElementMap::LoadDefaultList()
{
	const size_t kDefaultMultiCollIndexCount = sizeof(gDefaultMultiCollIndexArray)/sizeof(gDefaultMultiCollIndexArray[0]);
	uint32_t i = 0;
	uint32_t j = 0;
	uint32_t k = 0;
		
	for(; i < kDefaultMultiCollIndexCount; ++i)
	{
		const MultiCollIndex& index = gDefaultMultiCollIndexArray[i];
		EntryMap& newMap = mRoot[index.mChar0];
		k = 0;
			
		while(k < index.mCount)
		{
			const MultiCollEntry& entry = gDefaultMultiCollEntryArray[j + k++];
			newMap[entry.mChar1].mCE0 = entry.mCE0;
			newMap[entry.mChar1].mCE1 = entry.mCE1;
		}

		j += index.mCount;
	}
}

///////////////////////////////////////////////////////////////////////////////
// BuildSortKey
//
// This function expects the string argument to be a decomposed string of 
// normalization type NFD (canonical decomposition). 
//
void Collator::BuildSortKey(FixedSortString& s, FixedSortKey& k)
{
	// Produce the collation element array
	uint32_t       currentPos, lastCollElementSize;
	eastl_size_t   stringLen;
	eastl::vector<VariableCollElement> collElemArray;
	VariableWeightedCharacter vwc = {0, 0, 0, 0};
	bool bVariableFound;    // This refers to a variable weighted characters
	bool bLastCharVariable; 

	if(s.empty())
	{
		k.append(4, 0);
		return;
	}

	// Reserve enough space for the average word length x3 (one for each Collation Element array {L1, L2, L3, L4})
	// Based on language distribution (http://www.ravi.io/language-word-lengths) we reserve ceil(9.5)x4 = 40
	collElemArray.reserve(40);
	currentPos          = 0;

	// Used for popping collation elements off from our collElemArray in the case of a previously processed character being combined
	// with one or more characters ahead of it.
	lastCollElementSize = 0;
	stringLen           = s.size();
	bLastCharVariable   = false;

	// Find the longest substring which has a match in the collation element table
	while(currentPos < stringLen)
	{
		bVariableFound = false;
		const Char c = s[currentPos++];
		
		// if our collator allows for variable weights
		if(mSpecialCharOption != kSCONonIgnorable)
		{
			if(mSpecialCharOption == kSCOIgnoreSP)
			{
				const CharCategory cc = GetCharCategory(c);
				if((uint32_t)(cc - kCCSpaceSeparator) <= (uint32_t)(kCCFinalPunctuation - kCCSpaceSeparator))
				{
					bVariableFound = IsCharacterVariablyWeighted(&c, vwc);
				}
			}
			else
			{
				bVariableFound = IsCharacterVariablyWeighted(&c, vwc);
			}

			// if this character is variable weighted 
			if(bVariableFound)
			{
				// this is the kSCOBlanked case - Null initialized
				VariableCollElement& ce0 = collElemArray.push_back();
				EA_ASSERT(ce0.mL1 == 0);
				
				lastCollElementSize = 1;
				
				if(mSpecialCharOption != kSCOBlanked)
				{
					const CollEntry1* pCe1 = FindCollationElements(&c, mCollEntry1Array, mCollEntry1ArraySize);
					if(pCe1 != NULL)
					{
						// There are only two cases in which we change L4 to a different value
						// 1. if L1 != 0 and is variable
						if(pCe1->mCE0.mL1 != 0)
						{
							ce0.mL4 = pCe1->mCE0.mL1;
						}
						else if(pCe1->mCE0.mL1 == 0 && pCe1->mCE0.mL3 != 0 && !bLastCharVariable)
						{
							ce0.mL4 = UINT16_MAX;
						}

						lastCollElementSize = 1;
						bLastCharVariable = true;
					}
					else
					{
						// We know that we only support variable characters with primary or tertiary collation element arrays
						// so we don't even bother checking secondary collation tables
						const CollEntry3* pCe3 = FindCollationElements(&c, mCollEntry3Array, mCollEntry3ArraySize);
						if(pCe3 != NULL)
						{
							if(vwc.mCE0)
							{
								if(pCe3->mCE0.mL1 != 0)
								{
									ce0.mL4 = pCe3->mCE0.mL1;
								}
								else if(pCe3->mCE0.mL1 == 0 && pCe3->mCE0.mL3 != 0 && !bLastCharVariable)
								{
									ce0.mL4 = UINT16_MAX;
								}

								bLastCharVariable = true;
							}
							else
							{
								ce0.mL1 = pCe3->mCE0.mL1;
								ce0.mL2 = pCe3->mCE0.mL2;
								ce0.mL3 = pCe3->mCE0.mL3; 
								ce0.mL4 = ce0.mL1 ? UINT16_MAX : 0;
								bLastCharVariable = false;
							}

							VariableCollElement& ce1 = collElemArray.push_back();
							if(vwc.mCE1)
							{
								if(pCe3->mCE0.mL1 != 0)
								{
									ce1.mL4 = pCe3->mCE0.mL1;
								}
								else if(pCe3->mCE0.mL1 == 0 && pCe3->mCE0.mL3 != 0 && !bLastCharVariable)
								{
									ce1.mL4 = UINT16_MAX;
								}

								bLastCharVariable = true;
							}
							else
							{
								ce1.mL1 = pCe3->mCE1.mL1;
								ce1.mL2 = pCe3->mCE1.mL2;
								ce1.mL3 = pCe3->mCE1.mL3; 
								ce1.mL4 = ce1.mL1 ? UINT16_MAX : 0;
								bLastCharVariable = false;
							}

							VariableCollElement& ce2 = collElemArray.push_back();
							if(vwc.mCE2)
							{
								if(pCe3->mCE0.mL1 != 0)
								{
									ce2.mL4 = pCe3->mCE0.mL1;
								}
								else if(pCe3->mCE0.mL1 == 0 && pCe3->mCE0.mL3 != 0 && !bLastCharVariable)
								{
									ce2.mL4 = UINT16_MAX;
								}

								bLastCharVariable = true;
							}
							else
							{
								ce2.mL1 = pCe3->mCE2.mL1;
								ce2.mL2 = pCe3->mCE2.mL2;
								ce2.mL3 = pCe3->mCE2.mL3; 
								ce2.mL4 = ce2.mL1 ? UINT16_MAX : 0;
								bLastCharVariable = false;
							}

							lastCollElementSize = 3;
						}
						else
						{
							// If this is NULL it means we don't know about the variable character (since all variably 
							// weighted characters we care about only have one collation element array [L1,L2,L3]
							EA_ASSERT(pCe3 != NULL); 
						}
					}
				}
			}
		}

		if(!bVariableFound)
		{    
			const CollEntry1* pCe1 = FindCollationElements(&c, mCollEntry1Array, mCollEntry1ArraySize);
			if(pCe1 == NULL)
			{
				const CollEntry2* pCe2 = FindCollationElements(&c, mCollEntry2Array, mCollEntry2ArraySize);
				if(pCe2 == NULL)
				{
					const CollEntry3* pCe3 = FindCollationElements(&c, mCollEntry3Array, mCollEntry3ArraySize);
					if(pCe3 == NULL)
					{
							// We need to derive the weights since we have no match
							uint16_t base = 0xFBC0;
							if((c - 0x3400u) <= (0x4DB5u - 0x3400u))
							{
								base = 0xFB80;
							} else if((c - 0x4E00u) <= (0x9FCCu - 0x4E00u))
							{
								base = 0xFB40;
							}
						
							uint16_t AAAA = (uint16_t)(base + ((uint32_t)c >> 15));
							uint16_t BBBB = (uint16_t)(((uint32_t) c & 0x7FFF) | 0x8000);

							// We don't check for ignorables here since L1 will never == 0
							VariableCollElement& ce1 = collElemArray.push_back();
							ce1.mL1 = AAAA;
							ce1.mL2 = 0x0020;
							ce1.mL3 = 0x0002;
							ce1.mL4 = UINT16_MAX;
						
							VariableCollElement& ce2 = collElemArray.push_back();
							ce2.mL1 = BBBB;
							ce2.mL2 = 0;
							ce2.mL3 = 0;
							ce2.mL4 = UINT16_MAX;

							lastCollElementSize = 2;
					}
					else
					{
						VariableCollElement& ce0 = collElemArray.push_back();
						ce0.mL1 = pCe3->mCE0.mL1;
						ce0.mL2 = pCe3->mCE0.mL2;
						ce0.mL3 = pCe3->mCE0.mL3; 
						ce0.mL4 = ce0.mL1 ? UINT16_MAX : 0;

						VariableCollElement& ce1 = collElemArray.push_back();
						ce1.mL1 = pCe3->mCE1.mL1;
						ce1.mL2 = pCe3->mCE1.mL2;
						ce1.mL3 = pCe3->mCE1.mL3;
						ce1.mL4 = ce0.mL1 ? UINT16_MAX : 0;

						VariableCollElement& ce2 = collElemArray.push_back();
						ce2.mL1 = pCe3->mCE2.mL1;
						ce2.mL2 = pCe3->mCE2.mL2;
						ce2.mL3 = pCe3->mCE2.mL3;
						ce2.mL4 = ce2.mL1 ? UINT16_MAX : 0;

						lastCollElementSize = 3;
					}
				}
				else // Found a CE2 match
				{
					VariableCollElement& ce0 = collElemArray.push_back();

					ce0.mL1 = pCe2->mCE0.mL1;
					ce0.mL2 = pCe2->mCE0.mL2;
					ce0.mL3 = pCe2->mCE0.mL3;
					ce0.mL4 = ce0.mL1 ? UINT16_MAX : 0;

					VariableCollElement& ce1 = collElemArray.push_back();

					ce1.mL1 = pCe2->mCE1.mL1;
					ce1.mL2 = pCe2->mCE1.mL2;
					ce1.mL3 = pCe2->mCE1.mL3;
					ce1.mL4 = ce1.mL1 ? UINT16_MAX : 0;

					lastCollElementSize = 2;
				}
			}
			else // Found a CE1 match
			{
				lastCollElementSize = 1;

				eastl_size_t kPos = currentPos;
				const CollElement* pColElem0 = &pCe1->mCE0;
				VariableCollElement& ce0 = collElemArray.push_back();
				CollElement2* entry = NULL;

				// Check if the first character has a match in our table of two character indexed
				// collation element arrays
				KeyMap::iterator it = mCollationMultiElementMap.GetValue(c);
				if(it != mCollationMultiElementMap.mRoot.end())
				{
					EntryMap& entryMap = it->second;
					if(kPos < stringLen)
					{
						const Char c1 = s[kPos++];
						EntryMap::iterator jt = entryMap.find(c1);

						if(jt != entryMap.end())
						{
							lastCollElementSize = 1;

							entry = &jt->second;
							pColElem0 = &entry->mCE0;

							if((entry->mCE1.mL1 | entry->mCE1.mL2 | entry->mCE1.mL3) != 0)
							{
								VariableCollElement& ce1 = collElemArray.push_back();

								ce1.mL1 = entry->mCE1.mL1;
								ce1.mL2 = entry->mCE1.mL2;
								ce1.mL3 = entry->mCE1.mL3;
								ce1.mL4 = ce1.mL1 ? UINT16_MAX : 0;

								lastCollElementSize = 2;
							}
						
							stringLen--;
							s.erase(kPos-1, 1);
						}
					}
				}
 
				ce0.mL1 = pColElem0->mL1;
				ce0.mL2 = pColElem0->mL2;
				ce0.mL3 = pColElem0->mL3;
				ce0.mL4 = ce0.mL1 ? UINT16_MAX : 0;
			}

			// Determine if we have a collation element which relies of multiple characters as a key/index
			eastl_size_t kPos = currentPos;
			KeyMap::iterator it = mCollationMultiElementMap.GetValue(c);
			if(it != mCollationMultiElementMap.mRoot.end())
			{
				EntryMap& entryMap = it->second;
				while(kPos < stringLen)
				{
					const Char c1 = s[kPos++];
					int32_t ccc = GetCombiningClass(c1);
					if(ccc == 0 || ccc == GetCombiningClass(s[kPos-2]))
					{
						break;
					}

					EntryMap::iterator jt = entryMap.find(c1);
					if(jt != entryMap.end())
					{
						CollElement2& entry = jt->second;

						for(unsigned int i = 0; i < lastCollElementSize - 1; ++i)
						{
							collElemArray.pop_back();
						}

						VariableCollElement& ce0 = collElemArray.back();
						ce0.mL1 = entry.mCE0.mL1;
						ce0.mL2 = entry.mCE0.mL2;
						ce0.mL3 = entry.mCE0.mL3;
						ce0.mL4 = ce0.mL1 ? UINT16_MAX : 0;

						if((entry.mCE1.mL1 | entry.mCE1.mL2 | entry.mCE1.mL3) != 0)
						{
							VariableCollElement& ce1 = collElemArray.push_back();
							ce1.mL1 = entry.mCE1.mL1;
							ce1.mL2 = entry.mCE1.mL2;
							ce1.mL3 = entry.mCE1.mL3;
							ce1.mL4 = ce1.mL1 ? UINT16_MAX : 0;
						}
						
						s.erase(kPos-1, 1);
						stringLen--;
						break;
					}
				}
			}

			// The elements found here are not variable
			bLastCharVariable = false;
		}
	}
	
	// Now build the sort key
	unsigned int level = 0;
	const uint8_t memberOffset[4] = 
		{ (uint8_t) EA_OFFSETOF(VariableCollElement, mL1)
		, (uint8_t) EA_OFFSETOF(VariableCollElement, mL2)
		, (uint8_t) EA_OFFSETOF(VariableCollElement, mL3)
		, (uint8_t) EA_OFFSETOF(VariableCollElement, mL4)
		};

	#if EABASE_VERSION_N < 20300
		const uint8_t memberSize[4] = 
			{ (uint8_t) sizeof(((VariableCollElement*)0)->mL1)
			, (uint8_t) sizeof(((VariableCollElement*)0)->mL2)
			, (uint8_t) sizeof(((VariableCollElement*)0)->mL3)
			, (uint8_t) sizeof(((VariableCollElement*)0)->mL4)
			};
	#else
		const uint8_t memberSize[4] = 
			{ (uint8_t) EA_SIZEOF_MEMBER(VariableCollElement, mL1)
			, (uint8_t) EA_SIZEOF_MEMBER(VariableCollElement, mL2)
			, (uint8_t) EA_SIZEOF_MEMBER(VariableCollElement, mL3)
			, (uint8_t) EA_SIZEOF_MEMBER(VariableCollElement, mL4)
			};
	#endif

	do
	{
		if(level != 0)
		{
			k.push_back(0x0000);
		}

		for(eastl_size_t j = 0; j < collElemArray.size(); ++j)
		{
			VariableCollElement& ce = collElemArray[j];
			uint16_t lValue = 0;
			EA::StdC::Memcpy(&lValue, (void*)((uintptr_t)&ce + memberOffset[level]), memberSize[level]);

			if(lValue)
			{
				if(mSpecialCharOption != kSCOShiftTrimmed || (mSpecialCharOption == kSCOShiftTrimmed && lValue != UINT16_MAX))
				{
					k.push_back(lValue);
				}
			}
		}
	}while(++level < 4);

}

///////////////////////////////////////////////////////////////////////////////
// Compare
//
// Implement Unicode Technical Standard #10, section 4 (Main Algorithm)
//
// If the two strings are both entirely ASCII and mSpecialCharOption == kSCONonIgnorable, 
// then we can use a simple strcmp comparison. But we need to mind the kSOCapitalsFirst option.
//
// If caching is enabled and either string has a cached sort key, use the key.
//
int Collator::Compare(const Char* p1, const Char* p2)
{
	size_t p1Length = EA::StdC::Strlen(p1);
	size_t p2Length = EA::StdC::Strlen(p2);

	return Compare(p1, p1Length, p2, p2Length);
}

int Collator::Compare(const Char* p1, size_t p1Length, const Char* p2, size_t p2Length)
{
	using namespace Internal;

	int             result = 0;
	const Char*     p[2] = { p1, p2 };
	size_t          l[2] = { p1Length, p2Length };
	FixedSortString s[2];
	FixedSortKey    k[2];
	int             resultFlags[2];
	size_t          i;

	for(i = 0; i < 2; i++)
	{
		s[i].resize(kFixedLength - 1, Char(0)); // To do: Make it so this resize faster, as it currently fills the string with zeroes.

		const uint32_t requiredLength = Normalize(p[i], l[i], &s[i][0], s[i].size(), kNT_NFD, &resultFlags[i]);

		s[i].resize(requiredLength);

		if(requiredLength > s[i].size()) // If there wasn't initially enough room...
			Normalize(p[i], 0xffffffff, &s[i][0], s[i].size(), kNT_NFD, &resultFlags[i]);
	}

	// If we can do a simple strcmp...
	if((mSpecialCharOption == kSCONone) && (resultFlags[0] & kNRFAscii) && (resultFlags[1] & kNRFAscii))
		result = EA::StdC::Strcmp(s[0].c_str(), s[1].c_str());
	else
	{
		BuildSortKey(s[0], k[0]);
		BuildSortKey(s[1], k[1]);

		result = SortKeyCompare(k[0].c_str(), k[0].size(), k[1].c_str(), k[1].size());

		// If caching is enabled, cache the sort keys.
		//   (to do)
	}

	return result;
}

///////////////////////////////////////////////////////////////////////////////
// EATextCollationInit
//
void EATextCollationInit()
{
	EA_ASSERT_MSG(EA::Text::GetAllocator(), "Please ensure EA::Text::SetAllocator() has been called before EA::Text::Init()");
	gpNFCTrie = CORE_NEW(EA::Text::GetAllocator(), EATEXT_ALLOC_PREFIX "NFCTrie", 0) NFCTrie;
}

///////////////////////////////////////////////////////////////////////////////
// EATextCollationShutdown
//
void EATextCollationShutdown()
{
	CORE_DELETE(EA::Text::GetAllocator(), gpNFCTrie);
	gpNFCTrie = NULL;
}

} // namespace Text
} // namespace EA
