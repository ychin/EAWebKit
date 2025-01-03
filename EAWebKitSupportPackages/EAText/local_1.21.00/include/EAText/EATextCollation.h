///////////////////////////////////////////////////////////////////////////////
// EATextCollation.h
//
// Copyright (c) 2004 Electronic Arts. All Rights Reserved.
// By Paul Pedriana, Maxis
//
///////////////////////////////////////////////////////////////////////////////


#ifndef EATEXT_EATEXTCOLLATION_H
#define EATEXT_EATEXTCOLLATION_H

#include <EABase/eabase.h>
#include <EAText/EAText.h>
#include <EAText/EATextUnicode.h>
#include <EASTL/fixed_string.h>
#include <EASTL/core_allocator_adapter.h>
#include <coreallocator/icoreallocator_interface.h>
#include <stddef.h>
#include <EAText/internal/EATextCollation.h>
#include <EASTL/hash_map.h>
#include <EASTL/vector.h>
#include <EASTL/internal/config.h>

#ifdef _MSC_VER
	#pragma warning(push)           // We have to be careful about disabling this warning. Sometimes the warning is meaningful; sometimes it isn't.
	#pragma warning(disable: 4251)  // class (some template) needs to have dll-interface to be used by clients.
#endif


namespace EA
{
	namespace Allocator
	{
		class ICoreAllocator;
		typedef CoreAllocatorAdapter<ICoreAllocator> EASTLICoreAllocator;
	}

	namespace Text
	{
		enum NormalizationResultFlags
		{
			kNRFNone,
			kNRFAscii,     // The text was ASCII-only.
			kNRFUnchanged  // The text was already normalized or denormalized.
		};


		/// Normalize
		///
		/// Normalizes a string as per the given normalization type.
		/// Returns the required output strlen, which will be >=
		/// the nTextOutputCapacity value if there wasn't enough space. 
		/// Any UCS-2 Unicode value is valid as input text, including 0.
		/// The input is not expected to be 0-terminated. The output is 
		/// 0-terminated depending on pUnicodeOptions, but defaults to being so.
		/// If nTextInputLength is 0xffffffff, then the length used is the 
		/// Strlen of pTextInput.
		///
		EATEXT_API uint32_t Normalize(const Char* pTextInput, size_t nTextInputLength,
									  Char* pTextOutput, size_t nTextOutputCapacity, 
									  NormalizationType nt, int* pNormalizationResultFlags = NULL,
									  const UnicodeOptions* pUnicodeOptions = NULL);

		/// IsNormalized
		///
		/// Tells if a string is normalized as per the given normalization type.
		/// Note that with a return value of true, the kNRFNormalized flag will
		/// also be set.
		///
		/// This function has the same affect as the following, though it may
		/// possibly not be implemented as such:
		///     int flags;
		///     Normalize(pText, nTextLength, NULL, 0, nt, &flags);
		///     if(pNormalizationResultFlags)
		///         *pNormalizationResultFlags = flags;
		///     return (flags & kNRFUnchanged) != 0;
		///
		EATEXT_API bool IsNormalized(const Char* pText, size_t nTextLength, 
									 NormalizationType nt, int* pNormalizationResultFlags = NULL,
									 const UnicodeOptions* pUnicodeOptions = NULL);


		/// GetCase
		///
		/// Returns the case of the given character.
		/// Returns one of:
		///     kCaseTypeNone
		///     kCaseTypeLower
		///     kCaseTypeUpper
		/// Does not return one of the other CaseType enums (e.g. kCaseTypeReverse)
		/// This function isn't recommended as it doesn't support multiple character 
		/// Unicode sequences. However, as of this writing we don't have an alternative.
		///
		EATEXT_API CaseType GetCase(Char c);

		inline bool IsUpper(Char c)
			{ return GetCase(c) == kCaseTypeUpper; }
		inline bool IsLower(Char c)
			{ return GetCase(c) == kCaseTypeLower; }


		/// ConvertCase
		///
		/// Converts the case of a string.
		/// Returns the required strlen of the destination, which is >= nTextOutputCapacity if 
		/// there wasn't enough destination capacity.
		/// The user-supplied pTextOutput may be the same memory as pTextInput.
		/// The resulting pTextOutput's length may be a different length than pTextInput's length,
		/// as some languages are such that this is so.
		/// The output is always 0-terminated if the output capacity > 0. This means that the output
		/// can be truncated and the return value will be >= nTextOutputCapacity.
		///
		/// Example usage:
		///     Char str[10] = L"abc";
		///     EA::Text::ConvertCase(str, str, 10, EA::Text::kCaseTypeUpper);
		///
		/// Note:
		/// Don't attempt to write code like the following, because ConvertCase may in fact 
		/// generate output that are of a different length than the input.
		///     String str = "abc";
		///     ConvertCase(&str[0], str.length(), &str[0], str.length(), EA::Text::kCaseTypeUpper);
		///
		EATEXT_API uint32_t ConvertCase(const Char* pTextInput,                     Char* pTextOutput, size_t nTextOutputCapacity, CaseType caseType, const UnicodeOptions* pUnicodeOptions = NULL);
		EATEXT_API uint32_t ConvertCase(const Char* pTextInput, size_t nTextLength, Char* pTextOutput, size_t nTextOutputCapacity, CaseType caseType, const UnicodeOptions* pUnicodeOptions = NULL);


		/// ConvertCase
		///
		/// Converts the case of an STL-like string. 
		/// Returns a pointer to the string data.
		///
		/// Example usage:
		///     eastl::string16 s = L"abc";
		///     EA::Text::ConvertCase(s, EA::Text::kCaseTypeLower);
		///
		template <typename String>
		Char* ConvertCase(String& s, CaseType caseType, const UnicodeOptions* pUnicodeOptions = NULL)
		{
			// First try to convert in place without resizing s.
			const uint32_t initialStrlen  = (uint32_t)s.length();
			const uint32_t requiredStrlen = ConvertCase(s.data(), (uint32_t)s.length(), &s[0], (uint32_t)s.length() + 1, caseType, pUnicodeOptions);

			if(requiredStrlen != initialStrlen) // If we need to shrink the string or expand the string...
			{
				s.resize(requiredStrlen);

				if(requiredStrlen > initialStrlen) // Question: Is it a problem if we convert case of a string twice on the same data?
					ConvertCase(s.data(), (uint32_t)s.length(), &s[0], (uint32_t)s.length() + 1, caseType);
			}

			return const_cast<Char*>(s.c_str());
		}

		template <typename String>
		Char* MakeLower(String& s, const UnicodeOptions* pUnicodeOptions = NULL)
			{ return ConvertCase(s, kCaseTypeLower, pUnicodeOptions); }

		template <typename String>
		Char* MakeUpper(String& s, const UnicodeOptions* pUnicodeOptions = NULL)
			{ return ConvertCase(s, kCaseTypeUpper, pUnicodeOptions); }


		/// ToLower
		///
		/// Converts a single character to lower case. 
		/// Note that with some languages and characters it's not possible to 
		/// correctly convert a single character to another case, as combinations
		/// of characters in context are required. It's recommended that you do
		/// string-based conversions with ConvertCase or MakeLower.
		/// This function isn't recommended as it doesn't support multiple character 
		/// Unicode sequences. Instead use the ConvertCase functions.
		/// 
		/// Example usage:
		///     Char c = ToLower('A');
		///
		EATEXT_API Char ToLower(Char c);

		/// ToUpper
		///
		/// Converts a single character to upper case. 
		/// See ToLower for more documentation.
		/// 
		EATEXT_API Char ToUpper(Char c);




		/// Collator
		///
		/// Implements string collation (sorting and comparison).
		/// To implement a string sort, you could use this Collator with 
		/// a conventional sorting algorithm. 
		///
		/// This class design and implementation is based on the Unicode Standard 5.0.
		/// See the Unicode Technical Standard #10 (Collation) for a detailed discussion
		/// of proper collation.
		///
		/// String sort ordering is something that varies per language. By this we mean
		/// that a given language might sort the same strings in a different way from 
		/// another language. So you need to specify the language used by this class.
		/// You should not mix sorting orders produced by a Collator set to use two 
		/// different languages, as they may produce different results with the same 
		/// input.
		///
		/// The Compare function compares two strings, returning one of three values:
		///     < 0  -- p1 is less than p2
		///     = 0  -- p1 == p2
		///     > 0  -- p2 is less than p1
		///
		/// Note that it is possible for two different input strings to compare as equal.
		/// This is possible when one of the strings decomposes into the other. For example,
		/// the a character with an accent built into it decomposes into two chars, the base
		/// char and the accent char. Such a character will compare as equal to its decomposed
		/// equivalent.
		/// 
		/// Example usage:
		///     Collator collator;
		///     result = collator.Compare("0", "1"); // Returns -1.
		///     result = collator.Compare("z", "a"); // Returns +1.
		///     result = collator.Compare("A", "a"); // Returns +1. // Note that this result is different from strcmp, which returns -1. A proper localized Strcmp sets Capitals to sort 'greater' than lower case letters.
		///
		/// Example usage:
		///     Collator collator;
		///     eastl::vector<Char*> stringArray;
		/// 
		///     collator.SetLocale("ko-kr");
		///     collator.SetLocale(Collator::kSOLocaleFirst, 1);
		///     eastl::sort(stringArray.begin(), stringArray.end(), collator);
		///
		class EATEXT_API Collator
		{
		public:
			class CollationMultiElementMap
			{
			public:
				CollationMultiElementMap();
				void LoadDefaultList();

				KeyMap::iterator GetValue(Char c)
				{
					return mRoot.find(c);
				}
			   
				KeyMap mRoot;
			};

			enum SpecialCharOption  // See Unicode Technical Standard #10, section 3.2.2.
			{
				kSCONone,
				kSCONonIgnorable,
				kSCOBlanked,
				kSCOShifted,
				kSCOIgnoreSP,
				kSCOShiftTrimmed
			};

			enum AccentSortOption
			{
				kASONone,
				kASOForward,
				kASOReverse
			};

			enum SortOption
			{
				kSONone,            /// This is a placeholder enum.
				kSOSpecialChar,     /// value is one of SpecialCharOption, with the default being kSCONone. Controls how characters like ' ' and '-' participate (or don't participate) in sorting. Default is kSCOShifted.
				kSOAccentSort,      /// value is one of AccentSortOption, with the default being kASONone. Specifies how to sort accents; allows you to override the default behavior for the current language.
				kSOIgnoreCase,      /// value is True or false (1 or 0), with the default being false.
				kSOLocaleFirst,     /// value is True or false (1 or 0), with the default being false. If enabled, sort strings from the script associated with the locale in front of other scripts. Default is disabled.
				kSODigitsAsNumbers  /// value is True or false (1 or 0), with the default being false. If enabled, numeric sequences [0123456789.,<exponents>] are treated as individual numbers when comparing against other numeric sequences.
			};

		public:
			Collator(bool bEnableCache = false, Allocator::ICoreAllocator* pAllocator = NULL);

			/// Sets a locale as per the ISO-639 and ISO-3166 letter and country codes.
			/// An example locale might be "en-us", "fr-fr", or "ja-jp".
			/// The locale must be set correctly for sorting to occur properly for the given locale.
			void SetLocale(const char* pLocale);

			// To consider: Add a SetUnicodeOptions function. Is it relevant? Its locale duplicates the SetLocale function's purpose.

			/// Enables the sort cache, which causes intermediate calculations to be 
			/// cached for later use. If you will be sorting a large number of strings
			/// then you can speed up the operation by enabling the cache.
			/// If the cache is disabled then any cache memory is immediately freed.
			void EnableCache(bool bEnableCache, Allocator::ICoreAllocator* pAllocator = NULL, uint32_t maxMemoryUsage = 0xffffffff);

			/// Sets one of the options specified by SortOptions.
			void SetSortOption(SortOption option, int value);

			/// Sets one of the options specified by SortOptions.
			void SetSpecialCharacterSortOption(SpecialCharOption option);

			/// The Compare function compares two strings, returning one of three values:
			///     < 0  -- p1 is less than p2
			///     = 0  -- p1 == p2
			///     > 0  -- p2 is less than p1
			/// For example:
			///     Compare("3", "4") returns < 0.
			///     Compare("a", "A") returns > 0.
			///     Compare("A", "A") returns   0.
			/// Note that Unicode Standard-style string compares are different from C strcmp
			/// and wcscmp in some ways. For example, strcmp/wcscmp report capital letters
			/// as being lesser than lower case letters (simply because they have lower 
			/// ascii values). Unicode compares report capital letters as greater.
			int Compare(const Char* p1, const Char* p2);
			int Compare(const Char* p1, size_t p1Length, const Char* p2, size_t p2Length);

			/// STL/EASTL comparison operator.
			/// Returns true if p1 is less than p2. A return value of false means that p2 is >= p1.
			bool operator()(const Char* p1, const Char* p2)
				{ return (Compare(p1, p2) < 0); }

		protected:
			struct CollationElement
			{
				unsigned mPrimary   : 16;
				unsigned mSecondary : 10;
				unsigned mTertiary  :  6;
			};

			static const size_t kFixedLength = 32;

			typedef eastl::fixed_string<Char, kFixedLength, true, EA::Allocator::EASTLICoreAllocator> FixedSortString;
			typedef eastl::fixed_string<Char, kFixedLength, true, EA::Allocator::EASTLICoreAllocator> FixedSortKey;

		protected:
			void BuildSortKey(FixedSortString& s, FixedSortKey& k);

		protected:
			char                     mLocale[8];
			SpecialCharOption        mSpecialCharOption;
			AccentSortOption         mAccentSortOption;
			bool                     mbIgnoreCase;
			bool                     mbLocaleFirst;
			bool                     mbDigitsAsNumbers;
			CollationMultiElementMap mCollationMultiElementMap;
			const CollEntry1*        mCollEntry1Array;
			size_t                   mCollEntry1ArraySize;
			const CollEntry2*        mCollEntry2Array;
			size_t                   mCollEntry2ArraySize;
			const CollEntry3*        mCollEntry3Array;
			size_t                   mCollEntry3ArraySize;

			#ifdef EA_PLATFORM_WINDOWS
				unsigned long mLCID;
			#endif

			// Cache data
			// To to: Make sort keys use machine words instead of bytes.
			// To consider: It might be a little more optimal if we implemented a specialized SortStringSortKey struct.

			// typedef eastl::basic_string<Char, EASTLCoreAllocator>    SortString;
			// typedef eastl::basic_string<uint8_t, EASTLCoreAllocator> SortKey;
			// typedef eastl::hash_map<SortString, SortKey, eastl::string_hash<SortString> >             SortCache;
			// SortCache mSortCache;

		}; // class Collator

	} // namespace Text

} // namespace EA

#ifdef _MSC_VER
	#pragma warning(pop)
#endif


#endif // EATEXT_EATEXTCOLLATION_H











