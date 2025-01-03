/////////////////////////////////////////////////////////////////////////////
// TestCollation.cpp
//
// Copyright (c) 2011, Electronic Arts Inc. All rights reserved.
// Created by Paul Pedriana
//
/////////////////////////////////////////////////////////////////////////////


#include "EATextTest.h"
#include <EAText/EATextUnicode.h>
#include <EAText/EATextCollation.h>
#include <EAIO/EAFileStream.h>
#include <EAIO/EAStreamBuffer.h>
#include <EAIO/EAStreamAdapter.h>
#include <EAIO/EAFileDirectory.h>
#include <EAIO/EAFileUtil.h>
#include <EAStdC/EAStopwatch.h>
#include <EAStdC/EAString.h>
#include <EAStdC/EASprintf.h>
#include <EAStdC/EAMemory.h>
#include <EASTL/fixed_string.h>
#include <EATest/EATest.h>
#include EA_ASSERT_HEADER

#if defined EA_PLATFORM_KETTLE
	EA_DISABLE_CLANG_WARNING(-Wmissing-braces)
#endif

#if defined (EA_PLATFORM_IPHONE)
	namespace EA{ namespace IO {
		int appleGetHomeDirectory(char8_t* pDirectory, uint32_t nPathCapacity);
		int appleGetAppName(char8_t* pDirectory, uint32_t nPathCapacity);
	}}
#endif
typedef eastl::fixed_string<EA::Text::Char, 64> CollationTestFixedString;


// EATEXT_TEST_GENERATE_RANDOM_COLLATION_DATA
//
// Defined as 0 or 1. Usually defined as 0, unless we want to generate a new table.
// We generate a bunch of random strings of the locales we support via a known-good 
// implementation (Windows' CompareString), save the results and make sure our own
// implementation yields the same results with the same data.
//
#define EATEXT_TEST_GENERATE_RANDOM_COLLATION_DATA 0


#if EATEXT_TEST_GENERATE_RANDOM_COLLATION_DATA

	static const EA::Text::Char gCollationCharSelection[] = 
	{
		// To do: Expand this once we support more than just ASCII characters in our implementation and tests.
		'!', '\'', '\"', ',', '#', '$', '%', '&', '(', ')', '*', '+', ',', '-', '.', '/', ':', ';', 
		'<', '=', '>', '@', '[', '\\', ']', '^', '_', '`', '{', '|', '}', '~', 
		'a', 'b', 'c', 'd', 'e', 'f', 'g', 'h', 'i', 'j', 'k', 'l', 'm', 'n', 'o', 'p', 'q', 'r', 's', 't', 'u', 'v', 'w', 'x', 'y', 'z',
		'A', 'B', 'C', 'D', 'E', 'F', 'G', 'H', 'I', 'J', 'K', 'L', 'M', 'N', 'O', 'P', 'Q', 'R', 'S', 'T', 'U', 'V', 'W', 'X', 'Y', 'Z',
	};
	uint32_t kSelectionCount = EAArrayCount(gCollationCharSelection);


	static void MakeRandomCollationTestStringPair(CollationTestFixedString& s1, CollationTestFixedString& s2)
	{
		using namespace EA::Text;

		s1.clear();
		s2.clear();

		int32_t s1Length  = gRandom.RandRange(1, 10);
		int32_t s2Length  = eastl::max_alt((int32_t)1, (int32_t)gRandom.RandRange(s1Length - 2, s1Length + 2));
		int32_t maxLength = eastl::max_alt(s1Length, s2Length);

		for(int32_t i = 0; i < maxLength; i++)
		{
			uint32_t n1       = gRandom.RandLimit(kSelectionCount);
			Char     c1       = gCollationCharSelection[n1];
			uint32_t n2       = 0;
			Char     c2       = 0;
			uint32_t strategy = gRandom.RandLimit(3);

			if(strategy == 0) // Pick any character.
			{
				n2 = gRandom.RandLimit(kSelectionCount);
				c2 = gCollationCharSelection[n2];
			}
			else if(strategy == 1) // Make it a near character.
			{
				n2 = (uint32_t)gRandom.RandRange(eastl::max_alt((uint32_t)(n1 - 3), (uint32_t)0), 
												 eastl::min_alt((uint32_t)(n1 + 3), (uint32_t)kSelectionCount));
				c2 = gCollationCharSelection[n2];
			}
			else // Make it a reversed-case letter.
			{
				if(EA::Text::GetCase(c1) == EA::Text::kCaseTypeLower)
					c2 = EA::Text::ToUpper(c1);
				else if(EA::Text::GetCase(c1) == EA::Text::kCaseTypeUpper)
					c2 = EA::Text::ToLower(c1);
				else
					c2 = c1;
			}

			if(i < s1Length)
				s1 += c1;
			if(i < s2Length)
				s2 += c2;
		}
	}

	static void EncodeCString(CollationTestFixedString& s)
	{
		// Currently do nothing, since s is currently guaranteed to be ascii.
		for(eastl_size_t i = 0; i < s.length(); i++)
		{
			if((s[i] == '\\') || (s[i] == '"'))
			{
				s.insert(i, (eastl_size_t)1, (CollationTestFixedString::value_type)'\\');
				i++;
			}

			// To do: Handle Unicode characters beyond ASCII characters.
		}
	}

#endif // EATEXT_TEST_GENERATE_RANDOM_COLLATION_DATA

const EA::Text::Char* gNonIgnorableTestArray[] =
{
	EATEXT_CHAR("de luge"),
	EATEXT_CHAR("de Luge"),
	EATEXT_CHAR("de\x002Dluge"),
	EATEXT_CHAR("de\x002DLuge"),
	EATEXT_CHAR("de\x002Eluge"),
	EATEXT_CHAR("de\x002ELuge"),
	EATEXT_CHAR("death"),
	EATEXT_CHAR("deluge"),
	EATEXT_CHAR("deLuge"),
	EATEXT_CHAR("denmark")
};

const EA::Text::Char* gBlankedTestArray[] =
{
	EATEXT_CHAR("death"),
	EATEXT_CHAR("de luge"),
	EATEXT_CHAR("de\x002Dluge"),
	EATEXT_CHAR("deluge"),
	EATEXT_CHAR("de\x002Eluge"),
	EATEXT_CHAR("de Luge"),
	EATEXT_CHAR("de\x002DLuge"),
	EATEXT_CHAR("deLuge"),
	EATEXT_CHAR("de\x002ELuge"),
	EATEXT_CHAR("denmark")
};

const EA::Text::Char* gShiftedTestArray[] =
{
	EATEXT_CHAR("death"),
	EATEXT_CHAR("de luge"),
	EATEXT_CHAR("de\x002Dluge"),
	EATEXT_CHAR("de\x002Eluge"),
	EATEXT_CHAR("deluge"),
	EATEXT_CHAR("de Luge"),
	EATEXT_CHAR("de\x002DLuge"),
	EATEXT_CHAR("de\x002ELuge"),
	EATEXT_CHAR("deLuge"),
	EATEXT_CHAR("denmark")
};

const EA::Text::Char* gIgnoreSPTestArray[] =
{
	EATEXT_CHAR("death"),
	EATEXT_CHAR("de luge"),
	EATEXT_CHAR("de\x002Dluge"),
	EATEXT_CHAR("de\x002Eluge"),
	EATEXT_CHAR("deluge"),
	EATEXT_CHAR("de Luge"),
	EATEXT_CHAR("de\x002DLuge"),
	EATEXT_CHAR("de\x002ELuge"),
	EATEXT_CHAR("deLuge"),
	EATEXT_CHAR("denmark")
};

const EA::Text::Char* gShiftTrimmedArray[] =
{
	EATEXT_CHAR("death"),
	EATEXT_CHAR("deluge"),
	EATEXT_CHAR("de luge"),
	EATEXT_CHAR("de\x002Dluge"),
	EATEXT_CHAR("de\x002Eluge"),
	EATEXT_CHAR("deLuge"),
	EATEXT_CHAR("de Luge"),
	EATEXT_CHAR("de\x002DLuge"),
	EATEXT_CHAR("de\x002ELuge"),
	EATEXT_CHAR("denmark")
};

struct ConvertTest
{
	const EA::Text::Char* mStr1;
	const EA::Text::Char* mStr2;
};

struct TestCase
{
	EA::Text::Collator::SpecialCharOption   mSCO;
	const EA::Text::Char**                  mTestArray;
	size_t                                  mTestArraySize;
};

ConvertTest gConvertUpperTestArray[] = 
{
	{  // Russian
	   EATEXT_CHAR("\x041A\x0443\x043F\x0438\x0442\x044C\x0020\x0444\x0438\x0448\x043A\x0438\x0020\x0438\x0020\x0437\x043E\x043B\x043E\x0442\x043E"), 
	   EATEXT_CHAR("\x041A\x0423\x041F\x0418\x0422\x042C\x0020\x0424\x0418\x0428\x041A\x0418\x0020\x0418\x0020\x0417\x041E\x041B\x041E\x0422\x041E")
	},

	{  // Russian
	   EATEXT_CHAR("\x0421\x043F\x0438\x0441\x043E\x043A\x0020\x043B\x0438\x0434\x0435\x0440\x043E\x0432"), 
	   EATEXT_CHAR("\x0421\x041F\x0418\x0421\x041E\x041A\x0020\x041B\x0418\x0414\x0415\x0420\x041E\x0412")
	},

	{  // Russian
	   EATEXT_CHAR("\x0418\x0433\x0440\x0430\x0442\x044C"), 
	   EATEXT_CHAR("\x0418\x0413\x0420\x0410\x0422\x042C")
	},
};


int TestConvertCase()
{
	using namespace EA::Text;

	int nErrorCount = 0;

	{
		// template <typename String>
		// Char* ConvertCase(String& s, CaseType caseType, const UnicodeOptions* pUnicodeOptions = NULL)
		eastl::basic_string<Char> s;

		for(size_t i = 0; i < EAArrayCount(gConvertUpperTestArray); i++) // For each test set...
		{
			s = gConvertUpperTestArray[i].mStr1;
			EA::Text::ConvertCase(s, EA::Text::kCaseTypeUpper);

			// s is now an upper-case version of mStr1. It should be equal to mStr2, which is the expected result.
			if(s != gConvertUpperTestArray[i].mStr2) // If there was an unexpected result (i.e an error) ... 
			{
				for(eastl_size_t j = 0, jEnd = s.size(); j < jEnd; j++) // For each character in the unexpected result, print out the different chars.
				{
					Char c1 = s[j];
					Char c2 = gConvertUpperTestArray[i].mStr2[j];

					if(c1 != c2)
					{
						EATEST_VERIFY_F(c1 == c2, "    TestConvertCase for kCaseTypeUpper failed at char index %u for test set %u. Input char: %lc (%04x), expected output: %lc (%04x), actual output: %lc, (%04x)", 
													(unsigned)j, (unsigned)i, gConvertUpperTestArray[i].mStr1[j], gConvertUpperTestArray[i].mStr1[j], 
													(wchar_t)c2, (unsigned)c2, (wchar_t)c1, (unsigned)c1);
						break;
					}
				}
			}
		}
	}

	{
		// uint32_t ConvertCase(const Char* pTextInput, uint32_t nTextLength, Char* pTextOutput, uint32_t nTextOutputCapacity, CaseType caseType, const UnicodeOptions* pUnicodeOptions = NULL);

		eastl::basic_string<Char> s;
		Char     upperCase[256];
		uint32_t requiredStrlen;
		Char     cff; 
		memset(&cff, 0xff, sizeof(cff));

		for(size_t i = 0; i < EAArrayCount(gConvertUpperTestArray); i++) // For each test set...
		{
			s = gConvertUpperTestArray[i].mStr1;

			EA::StdC::Memset8(upperCase, 0xff, sizeof(upperCase));
			requiredStrlen = ConvertCase(s.data(), (uint32_t)s.length(), upperCase, EAArrayCount(upperCase), EA::Text::kCaseTypeUpper, NULL);
			EATEST_VERIFY(s.length() == requiredStrlen);
			EATEST_VERIFY(EA::StdC::Strcmp(upperCase, gConvertUpperTestArray[i].mStr2) == 0);

			EA::StdC::Memset8(upperCase, 0xff, sizeof(upperCase));
			requiredStrlen = ConvertCase(s.data(), (uint32_t)s.length(), upperCase, 1, EA::Text::kCaseTypeUpper, NULL);
			EATEST_VERIFY(requiredStrlen == EA::StdC::Strlen(gConvertUpperTestArray[i].mStr2));
			EATEST_VERIFY((upperCase[0] == 0) && (upperCase[1] == cff));

			UnicodeOptions uc; 
			uc.mbNoNullTerminate = true;
			EA::StdC::Memset8(upperCase, 0xff, sizeof(upperCase));
			requiredStrlen = ConvertCase(s.data(), (uint32_t)s.length(), upperCase, EAArrayCount(upperCase), EA::Text::kCaseTypeUpper, &uc);
			EATEST_VERIFY((upperCase[requiredStrlen - 1] != cff) && (upperCase[requiredStrlen] == cff));
			EATEST_VERIFY(requiredStrlen == EA::StdC::Strlen(gConvertUpperTestArray[i].mStr2));
		}
	}

	return nErrorCount;
}


int TestCompare()
{
	using namespace EA::Text;

	int nErrorCount = 0;

	// Unicode Collation Test
	{
		const EA::Text::FilePathChar* kCollationDataFileNonIgnorable = EATEXT_FILE_PATH_CHAR("CollationTest-NonIgnorable.txt");
		const EA::Text::FilePathChar* kCollationDataFileShifted      = EATEXT_FILE_PATH_CHAR("CollationTest-Shifted.txt");
		const EA::Text::FilePathChar* collationTestFiles[2]          = { kCollationDataFileNonIgnorable, kCollationDataFileShifted };

		const EA::IO::size_type kBufferSize = 128;
		const size_t kEntryWidth = 5; // 4 Hex-digits + 1 white space 
		size_t numLinesRead = 0;

		Collator collator;

		EA::IO::size_type lineSize;
		EA::Text::FilePathChar dataDirPath[EA::IO::kMaxPathLength];
		dataDirPath[0] = 0;

		EATEST_VERIFY(GetTestDataDirectory(dataDirPath, EAArrayCount(dataDirPath)) > 0);

		for(size_t nTestFile = 0; nTestFile < EAArrayCount(collationTestFiles); ++nTestFile)
		{
			if(nTestFile == 0)
			{
				collator.SetSpecialCharacterSortOption(EA::Text::Collator::kSCONonIgnorable);
			}
			else if(nTestFile == 1)
			{
				collator.SetSpecialCharacterSortOption(EA::Text::Collator::kSCOShifted);
			}

			char lineBuffer[kBufferSize] = { '\0' };
			uint32_t n = 0;

			EA::IO::size_type stringLen[2] = { 0 , 0 };
			Char stringBuffer[2][kBufferSize] = { { 0 } };

			#if EATEXT_FILE_PATH_CHAR_TYPE_SIZE == 1
				EA::IO::Path::PathString8 collationDataPath(dataDirPath);
				EA::IO::Path::PathString8 collationDataFile(collationTestFiles[nTestFile]);
			#else
				EA::IO::Path::PathString16 collationDataPath(dataDirPath);
				EA::IO::Path::PathString16 collationDataFile(collationTestFiles[nTestFile]);
			#endif

			EA::IO::Path::Join(collationDataPath, collationDataFile);
			EA::IO::Path::Normalize(collationDataPath);

			EA::IO::FileStream file(collationDataPath.c_str());
			file.AddRef();
			EATEST_VERIFY_F(file.Open(), "Failed to open %I16s", collationDataPath.c_str());
			EA::IO::StreamBuffer bufferedFile(EA::IO::StreamBuffer::kBufferSizeReadDefault, 0, &file);

			EA::StdC::Stopwatch      stopwatch(EA::StdC::Stopwatch::kUnitsMilliseconds, true);
			EA::StdC::LimitStopwatch limitStopwatch(EA::StdC::Stopwatch::kUnitsSeconds, 20, true);

			while ((lineSize = EA::IO::ReadLine(&bufferedFile, lineBuffer, kBufferSize)) < EA::IO::kSizeTypeDone) // While there there are lines to read
			{
				eastl::fixed_string<char, kBufferSize> testLine(lineBuffer, lineSize);         

				numLinesRead++;

				// We print a '.' char every N seconds in order to indicate we are still alive to any listeners of this test.
				if(limitStopwatch.IsTimeUp())
				{
					EA::UnitTest::Report(".\n");
					limitStopwatch.SetTimeLimit(20, true);
				}

				// If this takes longer than N minutes, quit the test. Some testing environments (e.g. those with remote disks) are
				// pathologically slow about reading disk files and automated test runs in such environments can take too long.
				if(stopwatch.GetElapsedTime() > (3 * 60 * 1000))
				{
					break;
				}

				if(testLine[0] == '#') // Comment - simply ignore this line
				{

				}
				else
				{
					unsigned val = 0;
					stringLen[n % 2] = 0;
					for(unsigned int i = 0; i < lineSize; i += kEntryWidth)
					{
						const char* p = testLine.c_str() + i;
						int wasRead = EA::StdC::Sscanf(p, "%X", &val);
						if(!wasRead)
						{
							val = 0;
						}

						stringBuffer[n % 2][stringLen[n % 2]++] = (Char)val;
					}

					// We alternate new/previous every other read as we read in new lines
					eastl::fixed_string<Char, kBufferSize> prevString(stringBuffer[(n + 1) % 2], stringLen[(n + 1) % 2]);
					eastl::fixed_string<Char, kBufferSize> newString(stringBuffer[n % 2], stringLen[n % 2]);
					n++;

					// Compare the new string to the old string and ensure the new string is always greater
					int result = collator.Compare(prevString.c_str(), prevString.size(), newString.c_str(), newString.size());
					EATEST_VERIFY_F(result <= 0, "CollationTest failure: '%I16s' %u '%I16s' %u. Expected/actual: %d/%d", prevString.c_str(), 
						prevString.size(), newString.c_str(), newString.size(), -1, result);

					if(result > 0) // the previous string is greater than the new string
					{
						#if DEBUG_PRINT
							EA::EAMain::Report("\nPrevString: ");
							for(unsigned int i = 0; i < prevString.size(); ++i)
							{
								EA::EAMain::Report("%04X ", prevString[i]);
							}
							EA::EAMain::Report("\nNew String: ");
							for(unsigned int i = 0; i < newString.size(); ++i)
							{
								EA::EAMain::Report("%04X ", newString[i]);
							}
							EA::EAMain::Report("\n");
							collator.Compare(prevString.c_str(), prevString.size(), newString.c_str(), newString.size());
						#endif
						// We failed the test so break out early
						break;
					}
				}
			} // while there are still lines to read

			stopwatch.Stop();
			EA::EAMain::Report("\t%I16s Finished - Elapsed Time: %I64u ms for %zu lines.\n", collationTestFiles[nTestFile], stopwatch.GetElapsedTime(), numLinesRead);
			bufferedFile.Close();
		}
	}

	// Unicode Variable Weighting tests
	{
		using namespace EA::Text;

		const TestCase testCases[] = 
		{
			{ Collator::kSCONonIgnorable, gNonIgnorableTestArray, EAArrayCount(gNonIgnorableTestArray) },
			{ Collator::kSCOShifted,      gShiftedTestArray,      EAArrayCount(gShiftedTestArray) },
			{ Collator::kSCOBlanked,      gBlankedTestArray,      EAArrayCount(gBlankedTestArray) },
			{ Collator::kSCOIgnoreSP,     gIgnoreSPTestArray,     EAArrayCount(gIgnoreSPTestArray) },
			{ Collator::kSCOShiftTrimmed, gShiftTrimmedArray,     EAArrayCount(gShiftTrimmedArray) }
		};

		Collator collator;

		for(size_t i = 0; i < EAArrayCount(testCases); ++i)
		{
			const Char** charArray = testCases[i].mTestArray;
			const size_t charArraySize = testCases[i].mTestArraySize;

			collator.SetSpecialCharacterSortOption(testCases[i].mSCO);

			for(size_t j = 0; j < charArraySize - 1; ++j)
			{
				const Char* s1 = charArray[j];
				const Char* s2 = charArray[j+1];

				int result = collator.Compare(s1, EA::StdC::Strlen(s1), s2, EA::StdC::Strlen(s2));

				// Each char array should be sorted from smallest to largest 
				EATEST_VERIFY_F(result <= 0, "Variable Weighted Character Test failure (SpecialCharOption: %d) (coll: '%I16s' %u '%I16s' %u. Expected/actual: %d/%d",
					testCases[i].mSCO, s1, EA::StdC::Strlen(s1), s2, EA::StdC::Strlen(s2), -1, result);
			}
		}
	}

	return nErrorCount;
}



int TestCollation()
{
	int nErrorCount = 0;

	nErrorCount += TestConvertCase();
	nErrorCount += TestCompare();

	return nErrorCount;
}

#if defined EA_PLATFORM_KETTLE
	EA_RESTORE_CLANG_WARNING() //-Wmissing-braces
#endif
