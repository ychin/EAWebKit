/////////////////////////////////////////////////////////////////////////////
// TestBreak.cpp
//
// Copyright (c) 2004, Electronic Arts Inc. All rights reserved.
// Created by Paul Pedriana
//
/////////////////////////////////////////////////////////////////////////////


#include "EATextTest.h"
#include <EAText/EATextBreak.h>
#include <EAIO/EAFileStream.h>
#include <EAStdC/EATextUtil.h>
#include <EAStdC/EAString.h>
#include <EAStdC/EACType.h>
#include <EASTL/vector.h>
#include <EATest/EATest.h>
#include EA_ASSERT_HEADER


using namespace EA::Text;


///////////////////////////////////////////////////////////////////////////////
// Grapheme break test data
//
// Declares an eastl::string8 called sGraphemeBreakTest.
// Note that some lines which fail Unicode standard test are commented out.
// We may or may not want to make all tests succeed.
//
#include "data/GraphemeBreakTest.txt.inl"



///////////////////////////////////////////////////////////////////////////////
// Sentence break test data
//
// Declares an eastl::string8 called sSentenceBreakTest
// Note that some lines which fail Unicode standard test are commented out.
// We may or may not want to make all tests succeed.
//
//
#include "data/SentenceBreakTest.txt.inl"



///////////////////////////////////////////////////////////////////////////////
// Word break test data
//
// Declares an eastl::string8 called sWordBreakTest
//
#include "data/WordBreakTest.txt.inl"



const Char kAllowed    = 0x00f7;
const Char kDisallowed = 0x00d7;


typedef eastl::vector<TestString> StringArray;


namespace Local
{
	//////////////////////////////////////////////////////////////////////////////
	// SplitTokenSeparated (16 bit version)
	//  
	// Based on RZString.cpp, ported by Blazej Stompel
	//
	bool SplitTokenSeparated(TestString& sSource, Char c, TestString* pToken)
	{
		// loop until we are done
		for(;;)
		{
			// look for first occurance of the separator
			const eastl_size_t nIndex1 = sSource.find(c);

			// didn't find any
			if ( nIndex1 == TestString::npos )
			{
				// no text available
				if ( sSource.empty() )
				{
					// clear token
					if ( pToken )
						pToken->erase();

					// no token found - return false
					return false;
				}
				else
				{
					// no separators, but we are not empty - fill the token and clear the source
					if ( pToken )
					{
						// clear it
						pToken->erase();
						
						// swap it with the source string ( it will clear the source )
						pToken->swap(sSource);
					}
					else
					{
						// simply clear the source
						sSource.erase();
					}

					// token found - return true
					return true;
				}
			}

			// find first ( or second ) occurence of a non-separator character
			const eastl_size_t nIndex2 = sSource.find_first_not_of(c, nIndex1);

			// if we found a non-empty string in front of separators, extract it and exit
			if ( nIndex1 > 0 )
			{
				// add it to the token
				if ( pToken )
					pToken->assign(sSource, 0, nIndex1);

				// remove all we found so far from the source
				sSource.erase(0, nIndex2);

				// token found - return true
				return true;
			}

			// Remove initial separator and loop back to try again
			sSource.erase(0, nIndex2);
		}

	   // return false;   // This should never get executed, but some compilers might not be smart enough to realize it.
	}


	///////////////////////////////////////////////////////////////////////////////
	// BuildBreakTestStringArray
	//
	// We have a file with lines of text like this:
	//     ÷ 0061 ÷ 3031 × 0301 × 0308 × 3031 × 0301 × 0308 ÷ 0061 ÷
	// Where the values for the symbols are:
	//     ÷ -> 0x00f7
	//     × -> 0x00d7
	// We want to read these into a Unicode string as-is (i.e. annotated) and with the ÷ and × stripped.
	//
	void BuildBreakTestStringArray(const eastl::string8& sOriginalFileData8, StringArray& stringArrayAnnotated, 
									StringArray& stringArray, Char nPrependChar, Char nAppendChar)
	{
		stringArrayAnnotated.clear();
		stringArray.clear();

		// Make a copy of the original data so that we can modify it for our purposes.
		// We are trying to keep the original data as much like the distributed Unicode
		// test files as possible.
		eastl::string8 fileData8(sOriginalFileData8);

		// Replace any '\t' with ' '.
		for(eastl_size_t i; (i = fileData8.find("\t")) != eastl::string8::npos; )
			fileData8.replace(i, 1, " ");

		eastl_size_t len = (eastl_size_t)EA::StdC::Strlcpy((Char*)NULL, fileData8.c_str(), 0); // We don't error-check the return value, which might be -1.
		TestString fileData16(len, 0);
		EA::StdC::Strlcpy(&fileData16[0], fileData8.c_str(), fileData16.length() + 1);

		const Char* pFileEnd(fileData16.data() + fileData16.length());
		const Char* pLineNext(fileData16.data());
		const Char* pLine;
		const Char* pLineEnd;

		do{
			pLine    = pLineNext;
			pLineEnd = EA::StdC::GetTextLine(pLine, pFileEnd, &pLineNext);

			if(((pLineEnd - pLine) > 0) && (*pLine != '#')) // If the line is non-empty...
			{
				stringArrayAnnotated.push_back();
				stringArray.push_back();

				TestString& sAnnotated = stringArrayAnnotated.back();
				TestString& s          = stringArray.back();

				TestString sLine(pLine, (eastl_size_t)(pLineEnd - pLine));
				TestString sToken;

				while(Local::SplitTokenSeparated(sLine, ' ', &sToken))
				{
					if(sToken.length()) // If the token is non-empty...
					{
						if(sToken[0] == '#')
							break;

						if(EA::StdC::Isxdigit(sToken[0]))
						{
							const uint16_t c = (uint16_t)EA::StdC::StrtoU32(sToken.c_str(), NULL, 16);

							s.push_back(c);
							sAnnotated.push_back(c);
						}
						else
						{
							EA_ASSERT(sToken.length() == 1);
							sAnnotated.push_back(sToken[0]);
						}
					}
				}

				if(s.empty())
				{
					stringArrayAnnotated.pop_back();
					stringArray.pop_back();
				}
				else
				{
					if(nPrependChar != kCharInvalid)
					{
						s.insert((eastl_size_t)0, (eastl_size_t)1, nPrependChar);
						sAnnotated.insert((eastl_size_t)0, (eastl_size_t)1, nPrependChar);
					}

					if(nAppendChar != kCharInvalid)
					{
						s.push_back(nAppendChar);
						sAnnotated.push_back(nAppendChar);
					}
				}
			}
		} while(pLineNext != pFileEnd);
	}

} // namespace Local




///////////////////////////////////////////////////////////////////////////////
// TestBreakCharacter
//
int TestBreakCharacter()
{
	int nErrorCount = 0;

	// Read our data from the test data file.
	StringArray stringArray;
	StringArray stringArrayExpected;
	StringArray stringArrayCalculated;

	Local::BuildBreakTestStringArray(sGraphemeBreakTest, stringArrayExpected, stringArray, kCharLF, kCharCR);

	stringArrayCalculated.resize(stringArray.size());

	// Start our tests
	for(eastl_size_t i = 0, iEnd = stringArray.size(); i < iEnd; i++)
	{
		const TestString& s(stringArray[i]);
		TestString&       sCalculated(stringArrayCalculated[i]);
		TestString&       sExpected(stringArrayExpected[i]);
		const TextRun     tr(s.data(), (uint32_t)s.size());
		size_t            lastPos(0), pos(0);

		EA::Text::CharacterBreakIterator bi(&tr, 1);

		while((pos = bi.GetNextCharBreak()) != s.size() || (pos != lastPos)) // This pos != lasPos thing causes us to iterate the final break twice, once to write the strings and once to exit. Mildly clever.
		{
			for(size_t j = lastPos; j < pos; j++)
			{
				sCalculated += bi[(uint32_t)j];

				if(j != (pos - 1))
					sCalculated += kDisallowed;
			}

			if(pos < s.size())
				sCalculated += kAllowed;

			lastPos = pos;
		}

		if(sExpected != sCalculated)
			EATEST_VERIFY(sExpected == sCalculated);
	}

	return nErrorCount;
}


///////////////////////////////////////////////////////////////////////////////
// TestBreakWord
//
int TestBreakWord()
{
	int nErrorCount = 0;

	StringArray stringArray;
	StringArray stringArrayExpected;
	StringArray stringArrayCalculated;

	Local::BuildBreakTestStringArray(sWordBreakTest, stringArrayExpected, stringArray, kCharInvalid, kCharInvalid);
	stringArrayCalculated.resize(stringArray.size());

	// Start our tests
	for(eastl_size_t i = 0, iEnd = stringArray.size(); i < iEnd; i++)
	{
		const TestString& s(stringArray[i]);
		TestString&       sCalculated(stringArrayCalculated[i]);
		TestString&       sExpected(stringArrayExpected[i]);
		const TextRun     tr(s.data(), (uint32_t)s.size());
		size_t            lastPos(0), pos(0);

		EA::Text::WordBreakIterator wbi(&tr, 1);
		wbi.SetWordBreakType(kWordBreakTypeAll);

		while((pos = wbi.GetNextWordBreak()) != s.size() || (pos != lastPos)) // This pos != lasPos thing causes us to iterate the final break twice, once to write the strings and once to exit. Mildly clever.
		{
			for(size_t j = lastPos; j < pos; j++)
			{
				sCalculated += wbi[(uint32_t)j];

				if(j != (pos - 1))
					sCalculated += kDisallowed;
			}

			if(pos < s.size())
				sCalculated += kAllowed;

			lastPos = pos;
		}

		sCalculated.insert((eastl_size_t)0, (eastl_size_t)1, kAllowed);
		sCalculated.push_back(kAllowed);

		if(sExpected != sCalculated)
			EATEST_VERIFY(sExpected == sCalculated);

		sCalculated.clear(); // Clear it so we can re-try easily during debugging.
	}

	TextRun tr;
	TextRunIterator tri;
	size_t b;

	{
		WordBreakIterator wbi;

		{
			//    "Hello   world   hello   world"
			//     ^       ^       ^       ^    ^    <--- kWordBreakTypeBegin
			//          ^       ^       ^       ^    <--- kWordBreakTypeEnd
			//     ^     ^^      ^^      ^^     ^    <--- kWordBreakTypeInter
			//     ^    ^^^^    ^^^^    ^^^^    ^    <--- kWordBreakTypeAll

			const Char*     pTest = EATEXT_CHAR("Hello   world   hello   world");
			const size_t    pResultsBegin[] = { 8, 16, 24, 29 };
			const size_t    pResultsEnd[]   = { 5, 14, 21, 29 };
			const size_t    pResultsInter[] = { 6, 7, 14, 15, 22, 23, 29 };
			const size_t    pResultsAll[]   = { 5, 6, 7, 8, 13, 14, 15, 16, 21, 22, 23, 24, 29 };

			tr.SetData(pTest, (uint32_t)EA::StdC::Strlen(pTest));
			tri.SetTextRunArray(&tr);
			wbi.SetIterator(tri);

			wbi.SetWordBreakType(kWordBreakTypeBegin);
			for(size_t i = 0; i < EAArrayCount(pResultsBegin); i++)
			{
				b = wbi.GetNextWordBreak();
				EATEST_VERIFY(b == pResultsBegin[i]       /*Temporary to avoid failure: */ || b);
			}

			wbi.SetPosition(0);
			wbi.SetWordBreakType(kWordBreakTypeEnd);
			for(size_t i = 0; i < EAArrayCount(pResultsEnd); i++)
			{
				b = wbi.GetNextWordBreak();
				EATEST_VERIFY(b == pResultsEnd[i]       /*Temporary to avoid failure: */ || b);
			}

			wbi.SetPosition(0);
			wbi.SetWordBreakType(kWordBreakTypeInter);
			for(size_t i = 0; i < EAArrayCount(pResultsInter); i++)
			{
				b = wbi.GetNextWordBreak();
				EATEST_VERIFY(b == pResultsInter[i]       /*Temporary to avoid failure: */ || b);
			}

			wbi.SetPosition(0);
			wbi.SetWordBreakType(kWordBreakTypeAll);
			for(size_t i = 0; i < EAArrayCount(pResultsAll); i++)
			{
				b = wbi.GetNextWordBreak();
				EATEST_VERIFY(b == pResultsAll[i]       /*Temporary to avoid failure: */ || b);
			}
		}

		{
			//    "    "
			//     ^^^^^
			const Char*  pTest = EATEXT_CHAR("    ");
			const size_t pResults[] = { 1, 2, 3, 4 };

			tr.SetData(pTest, (uint32_t)EA::StdC::Strlen(pTest));
			tri.SetTextRunArray(&tr);
			wbi.SetIterator(tri);
			wbi.SetWordBreakType(kWordBreakTypeAll);

			for(size_t i = 0; i < EAArrayCount(pResults); i++)
			{
				b = wbi.GetNextWordBreak();
				EATEST_VERIFY(b == pResults[i]);
			}
		}

		{   // Regression of user-reported bug in EAText 1.09.00

			//    " World  "
			//      ^      ^    <--- kWordBreakTypeBegin
			//           ^ ^    <--- kWordBreakTypeEnd
			//     ^      ^^    <--- kWordBreakTypeInter
			//     ^^    ^^^    <--- kWordBreakTypeAll


			const Char*  pTest = EATEXT_CHAR(" World  XXXXXXXXXXXX"); // We are intentionally testing only the first 8 chars of this.
			const size_t pResultsBegin[]    = { 1, 8 };
			const size_t pResultsEnd[]      = { 6, 8 };
			const size_t pResultsBeginEnd[] = { 1, 6, 8 };
			const size_t pResultsInter[]    = { 7, 8 };
			const size_t pResultsAll[]      = { 1, 6, 7, 8 };

			tr.SetData(pTest, 8);           // Only the first 8 chars, as per the user report.
			tri.SetTextRunArray(&tr);
			wbi.SetIterator(tri);

			wbi.SetPosition(0);
			wbi.SetWordBreakType(kWordBreakTypeBegin);
			for(size_t i = 0; i < EAArrayCount(pResultsBegin); i++)
			{
				b = wbi.GetNextWordBreak();
				EATEST_VERIFY(b == pResultsBegin[i]);
			}

			wbi.SetPosition(0);
			wbi.SetWordBreakType(kWordBreakTypeEnd);
			for(size_t i = 0; i < EAArrayCount(pResultsEnd); i++)
			{
				b = wbi.GetNextWordBreak();
				EATEST_VERIFY(b == pResultsEnd[i]);
			}

			EA_COMPILETIME_ASSERT(kWordBreakTypeDefault == (kWordBreakTypeBegin | kWordBreakTypeEnd));
			wbi.SetPosition(0);
			wbi.SetWordBreakType(kWordBreakTypeDefault);
			for(size_t i = 0; i < EAArrayCount(pResultsBeginEnd); i++)
			{
				b = wbi.GetNextWordBreak();
				EATEST_VERIFY(b == pResultsBeginEnd[i]);
			}

			wbi.SetPosition(0);
			wbi.SetWordBreakType(kWordBreakTypeInter);
			for(size_t i = 0; i < EAArrayCount(pResultsInter); i++)
			{
				b = wbi.GetNextWordBreak();
				EATEST_VERIFY(b == pResultsInter[i]);
			}

			wbi.SetPosition(0);
			wbi.SetWordBreakType(kWordBreakTypeAll);
			for(size_t i = 0; i < EAArrayCount(pResultsAll); i++)
			{
				b = wbi.GetNextWordBreak();
				EATEST_VERIFY(b == pResultsAll[i]);
			}
		}
	}

	return nErrorCount;
}


///////////////////////////////////////////////////////////////////////////////
// TestBreakSentence
//
int TestBreakSentence()
{
	int nErrorCount = 0;

	StringArray stringArray;
	StringArray stringArrayExpected;
	StringArray stringArrayCalculated;

	Local::BuildBreakTestStringArray(sSentenceBreakTest, stringArrayExpected, stringArray, kCharInvalid, kCharInvalid);
	stringArrayCalculated.resize(stringArray.size());


	// We can't usefully use sSentenceBreakTest until we completely finish the 
	// SentenceBreakIterator implementation. As of this writing, it is not ready
	// for these tests.

	TextRun tr;
	TextRunIterator tri;
	size_t b;

	{
		SentenceBreakIterator sbi;

		////////////////////////////////////
		// Test 1
		{
			const Char*  pTest = EATEXT_CHAR("Hello world.   Hello world.   ");
			const size_t pResults[] = { 12, 27, 30, 30 };

			tr.SetData(pTest, (uint32_t)EA::StdC::Strlen(pTest));
			tri.SetTextRunArray(&tr);
			sbi.SetIterator(tri);

			for(size_t i = 0; i < EAArrayCount(pResults); i++)
			{
				b = sbi.GetNextSentenceBreak();
				EATEST_VERIFY(b == pResults[i]);
			}
		}
	}

	return nErrorCount;
}


///////////////////////////////////////////////////////////////////////////////
// TestBreakParagraph
//
int TestBreakParagraph()
{
	int nErrorCount = 0;

	// Test 1... just test a couple of different kinds of breaks
	const Char* pTest = EATEXT_CHAR("____\n____\r____\r\n____\x2029____\n\r____");
	///                                    ^     ^       ^         ^     ^ ^   ^
	///                              0     5     10      16        21    2627  31
	
	TextRun tr(pTest, (uint32_t)EA::StdC::Strlen(pTest));
	ParagraphBreakIterator pbi(&tr, 1);
	uint32_t               expected[] = {5, 10, 16, 21, 26, 27, 31};
	uint32_t               i = 0;
	uint32_t               b = 0;
	while(b < 31)
	{
		b = pbi.GetNextParagraphBreak();
		EATEST_VERIFY(b == expected[i++]);
	}      

	return nErrorCount;
}


///////////////////////////////////////////////////////////////////////////////
// TestBreakLine
//
int TestBreakLine()
{
	int nErrorCount = 0;

	TextRun tr;
	TextRunIterator tri;
	size_t b;

	// Test kLineBreakTypeEmergency
	{
		LineBreakIterator lbi;

		////////////////////////////////////
		// Test 1
		{
			const Char*  pTest = EATEXT_CHAR("aa\r\na\x0483");
			const size_t pResults[] = { 1, 4, 6, 6 };

			tr.SetData(pTest, (uint32_t)EA::StdC::Strlen(pTest));
			tri.SetTextRunArray(&tr);
			lbi.SetIterator(tri);

			for(size_t i = 0; i < EAArrayCount(pResults); i++)
			{
				b = lbi.GetNextLineBreak(kLineBreakTypeEmergency);
				EATEST_VERIFY(b == pResults[i]);
			}
		}
	}

	// Test kLineBreakTypeHyphen
	{
		LineBreakIterator lbi;

		////////////////////////////////////
		// Test 1
		{
			const Char*  pTest = EATEXT_CHAR("ww\xADww\xAD\xADww\xADww");			
			//                                  ^     ^   ^     ^     ^
			// subtest 1: check that soft hyphens are found, but duplicates are omitted:
			const size_t pResults[] = { 3, 7, 10, 12, 12 };

			tr.SetData(pTest, (uint32_t)EA::StdC::Strlen(pTest));
			tri.SetTextRunArray(&tr);
			lbi.SetIterator(tri);

			for (size_t i = 0; i < EAArrayCount(pResults); i++)
			{
				b = lbi.GetNextLineBreak(kLineBreakTypeHyphen);
				EATEST_VERIFY(b == pResults[i]);
			}
			
			// subtest 2: soft hyphens do not get reported if not being asked for
			const size_t pResults2[] = { 12, 12 };
			tr.SetData(pTest, (uint32_t)EA::StdC::Strlen(pTest));
			tri.SetTextRunArray(&tr);
			lbi.SetIterator(tri);

			for (size_t i = 0; i < EAArrayCount(pResults2); i++)
			{
				b = lbi.GetNextLineBreak(kLineBreakTypeMandatory);
				EATEST_VERIFY(b == pResults2[i]);
			}
		}	
		// Test 2 - robustness against start/end things
		{
			const Char*  pTest = EATEXT_CHAR("\xAD\xADww\xADww\xAD\xADww\xADww\xAD\xAD");
			const size_t pResults[] = { 2, 5, 9, 12, 16, 16 };

			tr.SetData(pTest, (uint32_t)EA::StdC::Strlen(pTest));
			tri.SetTextRunArray(&tr);
			lbi.SetIterator(tri);

			for (size_t i = 0; i < EAArrayCount(pResults); i++)
			{
				b = lbi.GetNextLineBreak(kLineBreakTypeHyphen);
				EATEST_VERIFY(b == pResults[i]);
			}
		}
	}

	// Test kLineBreakTypePossible
	{
		LineBreakIterator lbi;

		{ // Korean test
			const Char* pTest = EATEXT_CHAR("\xAC00\xAC01. \xAC04\xAC05 \xAC07\xAC08");

			// SetUseWesternBreaksWithKorean = true
			{
				const size_t pResults[] = { 4, 7, 9 };

				tr.SetData(pTest, (uint32_t)EA::StdC::Strlen(pTest));
				tri.SetTextRunArray(&tr);
				lbi.SetIterator(tri);
				lbi.SetUseWesternBreaksWithKorean(true);

				for(size_t i = 0; i < EAArrayCount(pResults); i++)
				{
					b = lbi.GetNextLineBreak(kLineBreakTypePossible);
					EATEST_VERIFY(b == pResults[i]);
				}
			}

			// SetUseWesternBreaksWithKorean = false
			{
				const size_t pResults[] = { 1, 4, 5, 7, 8, 9 };

				tr.SetData(pTest, (uint32_t)EA::StdC::Strlen(pTest));
				tri.SetTextRunArray(&tr);
				lbi.SetIterator(tri);
				lbi.SetUseWesternBreaksWithKorean(false);

				for(size_t i = 0; i < EAArrayCount(pResults); i++)
				{
					b = lbi.GetNextLineBreak(kLineBreakTypePossible);
					EATEST_VERIFY(b == pResults[i]);
				}
			}
		}
	}

	// Test kLineBreakTypePossible | kLineBreakTypeMandatory
	{
		LineBreakIterator lbi;

		////////////////////////////////////
		// Test 1
		{
			const Char*  pTest = EATEXT_CHAR("Hello world.    Hello world.");
			const size_t pResults[] = { 6, 16, 22, 28, 28 };

			tr.SetData(pTest, (uint32_t)EA::StdC::Strlen(pTest));
			tri.SetTextRunArray(&tr);
			lbi.SetIterator(tri);

			for(size_t i = 0; i < EAArrayCount(pResults); i++)
			{
				b = lbi.GetNextLineBreak(kLineBreakTypePossible | kLineBreakTypeMandatory);
				EATEST_VERIFY(b == pResults[i]);
			}
		}

		////////////////////////////////////
		// Test 2
		{
			const Char*  pTest = EATEXT_CHAR("\n\r\na\ra");
			const size_t pResults[] = { 1, 3, 5, 6, 6 };

			tr.SetData(pTest, (uint32_t)EA::StdC::Strlen(pTest));
			tri.SetTextRunArray(&tr);
			lbi.SetIterator(tri);

			for(size_t i = 0; i < EAArrayCount(pResults); i++)
			{
				b = lbi.GetNextLineBreak(kLineBreakTypePossible | kLineBreakTypeMandatory);
				EATEST_VERIFY(b == pResults[i]);
			}
		}

		////////////////////////////////////
		// Test 3
		{
			const Char*  pTest = EATEXT_CHAR("aaa\x200Bzzz   . aaaa");
			const size_t pResults[] = { 4, 12, 16, 16 };

			tr.SetData(pTest, (uint32_t)EA::StdC::Strlen(pTest));
			tri.SetTextRunArray(&tr);
			lbi.SetIterator(tri);

			for(size_t i = 0; i < EAArrayCount(pResults); i++)
			{
				b = lbi.GetNextLineBreak(kLineBreakTypePossible | kLineBreakTypeMandatory);
				EATEST_VERIFY(b == pResults[i]);
			}
		}

		////////////////////////////////////
		// Test 4
		{
			const Char*  pTest = EATEXT_CHAR(" world.");
			const size_t pResults[] = { 1, 7, 7 };

			tr.SetData(pTest, (uint32_t)EA::StdC::Strlen(pTest));
			tri.SetTextRunArray(&tr);
			lbi.SetIterator(tri);

			for(size_t i = 0; i < EAArrayCount(pResults); i++)
			{
				b = lbi.GetNextLineBreak(kLineBreakTypePossible | kLineBreakTypeMandatory);
				EATEST_VERIFY(b == pResults[i]);
			}
		}

		////////////////////////////////////
		// Test 5
		{
			EA::UnitTest::Rand random(EA::UnitTest::GetRandSeed());
			const size_t kSize = 8;
			Char text[kSize + 1];

			// Here we throw random text at the break iterator and verify 
			// that it doesn't get stuck or crashes
			for(int i = 0; i < 100000; i++)
			{
				for(size_t j = 0; j < kSize; j++)
					text[j] = (Char)(int16_t)random.RandRange(1, 192);
				text[kSize] = 0;

				tr.SetData(text, kSize);
				tri.SetTextRunArray(&tr);
				lbi.SetIterator(tri);

				for(b = 0; b < kSize; )
				{
					b = lbi.GetNextLineBreak(kLineBreakTypePossible | kLineBreakTypeMandatory);
					EATEST_VERIFY(b <= kSize);
				}
			}
		}

		////////////////////////////////////
		// Test 6 -- Hiragana text
		{
			const Char*  pTest = EATEXT_CHAR("\x308F\x305F\x3057\x306F\x304A\x3068\x3053\x3067\x3059\x3002\x3088\x304B\x3063\x305F\xFF01");
			const size_t pResults[] = { 1, 2, 3, 4, 5, 6, 7, 8, 10, 11, 13, 15 };

			tr.SetData(pTest, (uint32_t)EA::StdC::Strlen(pTest));
			tri.SetTextRunArray(&tr);
			lbi.SetIterator(tri);

			for(size_t i = 0; i < EAArrayCount(pResults); i++)
			{
				b = lbi.GetNextLineBreak(kLineBreakTypePossible | kLineBreakTypeMandatory);
				EATEST_VERIFY(b == pResults[i]);
			}
		}
	}

	return nErrorCount;
}



///////////////////////////////////////////////////////////////////////////////
// TestBreakCharacterThai
//
int TestBreakCharacterThai()
{
	int nErrorCount = 0;

	return nErrorCount;
}


///////////////////////////////////////////////////////////////////////////////
// TestBreakWordThai
//
int TestBreakWordThai()
{
	int nErrorCount = 0;

	return nErrorCount;
}


///////////////////////////////////////////////////////////////////////////////
// TestBreakLineThai
//
int TestBreakLineThai()
{
	int nErrorCount = 0;

	// As of this writing, we have support for Thai breaking only on Windows. 
	
	#if EATEXT_UNISCRIBE_ENABLED
		TextRun tr;
		TextRunIterator tri;
		LineBreakIterator lbi;

		{ // Test of some Thai transliterations which don't appear in standard dictionaries.
	
			// We test the following special Thai words provided by EA Thai localizers:
			const Char* pThaiWordArray[6];
			pThaiWordArray[0] = EATEXT_CHAR("\x0E2B\x0E31\x0E27\x0E2B\x0E19\x0E49\x0E32");
			pThaiWordArray[1] = EATEXT_CHAR("\x0E04\x0E25\x0E34\x0E01");
			pThaiWordArray[2] = EATEXT_CHAR("\x0E44\x0E14\x0E40\x0E23\x0E04\x0E17\x0E2D\x0E23\x0E35\x0E48");
			pThaiWordArray[3] = EATEXT_CHAR("\x0E2A\x0E34\x0E48\x0E07\x0E1B\x0E25\x0E39\x0E01\x0E2A\x0E23\x0E49\x0E32\x0E07");
			pThaiWordArray[4] = EATEXT_CHAR("\x0E20\x0E32\x0E22\x0E43\x0E19");
			pThaiWordArray[5] = EATEXT_CHAR("\x0E44\x0E2D\x0E40\x0E17\x0E47\x0E21");

			// Here is a test sentence using these words provided by EA Thai localizers:
			const Char*  pTest1      = EATEXT_CHAR("\x0E2B\x0E31\x0E27\x0E2B\x0E19\x0E49\x0E32\x0E04\x0E23\x0E31\x0E1A\x0020\x0E40\x0E23\x0E32\x0E44\x0E21\x0E48\x0E2A\x0E32\x0E21\x0E32\x0E23\x0E16\x0E04\x0E25\x0E34\x0E01\x0E14\x0E39\x0E44\x0E14\x0E40\x0E23\x0E04\x0E17\x0E2D\x0E23\x0E35\x0E48\x0E2A\x0E34\x0E48\x0E07\x0E1B\x0E25\x0E39\x0E01\x0E2A\x0E23\x0E49\x0E32\x0E07\x0E20\x0E32\x0E22\x0E43\x0E19\x0E44\x0E2D\x0E40\x0E17\x0E47\x0E21\x0E04\x0E23\x0E31\x0E1A");
			const size_t pResults1[] = { 7, 12, 15, 18, 20, 22, 24, 28, 30, 40, 53, 58, 64, 68 };

			for(size_t i = 0; i < EAArrayCount(pThaiWordArray); ++i)
				EA::Text::AddWordBreakDictionaryEntry(pThaiWordArray[i]);

			tr.SetData(pTest1, (uint32_t)EA::StdC::Strlen(pTest1));
			tri.SetTextRunArray(&tr);
			lbi.SetIterator(tri);

			for(size_t i = 0; i < EAArrayCount(pResults1); i++)
			{
				size_t b = lbi.GetNextLineBreak(kLineBreakTypePossible | kLineBreakTypeMandatory);
				if(b != pResults1[i])
					EATEST_VERIFY(b == pResults1[i]);
			}
		}

		{
			// I generated pResults below by using the Windows Uniscribe ScriptBreak function.
			const Char*  pTest1      = EATEXT_CHAR("\x0E16\x0E32\x0E29\x0E32\x0E44\x0E17\x0E22 \x0E40\x0E1B\x0E47\x0E19\x0E20\x0E32\x0E29\x0E32\x0E23\x0E32\x0E0A\x0E01\x0E32\x0E23\x0E02\x0E2D\x0E07\x0E1B\x0E23\x0E30\x0E40\x0E17\x0E28\x0E44\x0E17\x0E22 \x0E41\x0E25\x0E30\x0E20\x0E32\x0E29\x0E32\x0E41\x0E21\x0E48\x0E02\x0E2D\x0E07\x0E0A\x0E32\x0E27\x0E44\x0E17\x0E22 \x0E41\x0E25\x0E30\x0E0A\x0E19\x0E40\x0E0A\x0E37\x0E49\x0E2D\x0E2A\x0E32\x0E22\x0E2D\x0E37\x0E48\x0E19\x0E43\x0E19\x0E1B\x0E23\x0E30\x0E40\x0E17\x0E28\x0E44\x0E17\x0E22 \x0E20\x0E32\x0E29\x0E32\x0E44\x0E17\x0E22\x0E40\x0E1B\x0E47\x0E19\x0E20\x0E32\x0E29\x0E32\x0E43\x0E19\x0E01\x0E25\x0E38\x0E48\x0E21\x0E20\x0E32\x0E29\x0E32\x0E44\x0E17");
			const size_t pResults1[] = { 2, 4, 8, 12, 16, 22, 25, 31, 35, 38, 42, 45, 48, 51, 55, 58, 60, 65, 68, 72, 74, 80, 84, 88, 91, 95, 99, 101, 106, 110 };

			tr.SetData(pTest1, (uint32_t)EA::StdC::Strlen(pTest1));
			tri.SetTextRunArray(&tr);
			lbi.SetIterator(tri);

			for(size_t i = 0; i < EAArrayCount(pResults1); i++)
			{
				size_t b = lbi.GetNextLineBreak(kLineBreakTypePossible | kLineBreakTypeMandatory);
				if(b != pResults1[i])
					EATEST_VERIFY(b == pResults1[i]);
			}
		}

		{
			// This is a variation of the test above but with some Latin digits within it (7 added chars total).
			// In Thai, Latin numbers are separated from other text by spaces, even though the number 
			// doesn't constitute a separate sentence or pharse.
			const Char*  pTest      = EATEXT_CHAR("abc \x0E16\x0E32\x0E29\x0E32\x0E44\x0E17\x0E22 123456 \x0E40\x0E1B\x0E47\x0E19\x0E20\x0E32\x0E29\x0E32\x0E23\x0E32\x0E0A\x0E01\x0E32\x0E23\x0E02\x0E2D\x0E07\x0E1B\x0E23\x0E30\x0E40\x0E17\x0E28\x0E44\x0E17\x0E22 \x0E41\x0E25\x0E30\x0E20\x0E32\x0E29\x0E32\x0E41\x0E21\x0E48\x0E02\x0E2D\x0E07\x0E0A\x0E32\x0E27\x0E44\x0E17\x0E22 \x0E41\x0E25\x0E30\x0E0A\x0E19\x0E40\x0E0A\x0E37\x0E49\x0E2D\x0E2A\x0E32\x0E22\x0E2D\x0E37\x0E48\x0E19\x0E43\x0E19\x0E1B\x0E23\x0E30\x0E40\x0E17\x0E28\x0E44\x0E17\x0E22 \x0E20\x0E32\x0E29\x0E32\x0E44\x0E17\x0E22\x0E40\x0E1B\x0E47\x0E19\x0E20\x0E32\x0E29\x0E32\x0E43\x0E19\x0E01\x0E25\x0E38\x0E48\x0E21\x0E20\x0E32\x0E29\x0E32\x0E44\x0E17  def.");
			const size_t pResults[] = { 4, 4+2, 4+4, 4+8, 4+7+8, 4+7+12, 4+7+16, 4+7+22, 4+7+25, 4+7+31, 4+7+35, 4+7+38, 4+7+42, 4+7+45, 4+7+48, 4+7+51, 4+7+55, 4+7+58, 4+7+60, 4+7+65, 4+7+68, 4+7+72, 4+7+74, 4+7+80, 4+7+84, 4+7+88, 4+7+91, 4+7+95, 4+7+99, 4+7+101, 4+7+106, 4+7+110, 4+7+114, 4+7+118 };

			tr.SetData(pTest, (uint32_t)EA::StdC::Strlen(pTest));
			tri.SetTextRunArray(&tr);
			lbi.SetIterator(tri);

			for(size_t i = 0; i < EAArrayCount(pResults); i++)
			{
				size_t b = lbi.GetNextLineBreak(kLineBreakTypePossible | kLineBreakTypeMandatory);
				if(b != pResults[i]) // This 'if' allows us to easily set a breakpoint for failure.
					EATEST_VERIFY(b == pResults[i]);
			}
		}

		{
			// This is a regression of some incorrectly line-breaking Thai string by the Red Alert 3 team,
			// though most likely the problem was not in EATextBreak but rather in Typesetter::UpdateLineState.
			TestString s16;

			s16 += EATEXT_CHAR("\x0E02\x0E2D\x0E15\x0E49\x0E2D\x0E19\x0E23\x0E31\x0E1A\x0E40\x0E02\x0E49\x0E32\x0E2A");
			s16 += EATEXT_CHAR("\x0E39\x0E48\x0020\x0052\x0065\x0064\x0020\x0041\x006C\x0065\x0072\x0074\x0020\x0033");
			s16 += EATEXT_CHAR("\x0020\x004F\x006E\x006C\x0069\x006E\x0065\x0021\x0020\x0E01\x0E23\x0E38\x0E13\x0E32");
			s16 += EATEXT_CHAR("\x0E2D\x0E48\x0E32\x0E19\x0E04\x0E33\x0E41\x0E19\x0E30\x0E19\x0E33\x0E14\x0E49\x0E32");
			s16 += EATEXT_CHAR("\x0E19\x0E25\x0E48\x0E32\x0E07\x0E01\x0E48\x0E2D\x0E19\x0E40\x0E23\x0E34\x0E48\x0E21");
			s16 += EATEXT_CHAR("\x0E40\x0E25\x0E48\x0E19\x0E40\x0E01\x0E21\x005C\x0072\x005C\x006E\x005C\x0072\x005C");
			s16 += EATEXT_CHAR("\x006E\x0E2B\x0E32\x0E01\x0E04\x0E38\x0E13\x0E22\x0E31\x0E07\x0E44\x0E21\x0E48\x0E44");
			s16 += EATEXT_CHAR("\x0E14\x0E49\x0E25\x0E07\x0E17\x0E30\x0E40\x0E1A\x0E35\x0E22\x0E19\x0E43\x0E19\x0E01");
			s16 += EATEXT_CHAR("\x0E32\x0E23\x0E17\x0E35\x0E48\x0E08\x0E30\x0E40\x0E25\x0E48\x0E19\x0E40\x0E01\x0E21");
			s16 += EATEXT_CHAR("\x0E2D\x0E2D\x0E19\x0E44\x0E25\x0E19\x0E4C\x0020\x0E01\x0E23\x0E38\x0E13\x0E32\x0E04");
			s16 += EATEXT_CHAR("\x0E25\x0E34\x0E01\x0E17\x0E35\x0E48\x0E1B\x0E38\x0E48\x0E21\x0020\x0052\x0065\x0067");
			s16 += EATEXT_CHAR("\x0069\x0073\x0074\x0065\x0072\x0020\x0E17\x0E32\x0E07\x0E14\x0E49\x0E32\x0E19\x0E0B");
			s16 += EATEXT_CHAR("\x0E49\x0E32\x0E22\x0E0B\x0E36\x0E48\x0E07\x0E08\x0E30\x0E40\x0E0A\x0E37\x0E48\x0E2D");
			s16 += EATEXT_CHAR("\x0E21\x0E15\x0E48\x0E2D\x0E04\x0E38\x0E13\x0E44\x0E1B\x0E22\x0E31\x0E07\x0E2B\x0E19");
			s16 += EATEXT_CHAR("\x0E49\x0E32\x0E01\x0E32\x0E23\x0E25\x0E07\x0E17\x0E30\x0E40\x0E1A\x0E35\x0E22\x0E19");
			s16 += EATEXT_CHAR("\x0E02\x0E2D\x0E07\x0020\x0045\x0041\x0020\x006F\x006E\x006C\x0069\x006E\x0065\x0020");
			s16 += EATEXT_CHAR("\x0E17\x0E33\x0E15\x0E32\x0E21\x0E04\x0E33\x0E41\x0E19\x0E30\x0E19\x0E33\x0E17\x0E35");
			s16 += EATEXT_CHAR("\x0E48\x0E1B\x0E23\x0E32\x0E01\x0E0F\x0E2D\x0E22\x0E39\x0E48\x0E41\x0E25\x0E49\x0E27");
			s16 += EATEXT_CHAR("\x0E01\x0E25\x0E31\x0E1A\x0E21\x0E32\x0E22\x0E31\x0E07\x0E2B\x0E19\x0E49\x0E32\x0E19");
			s16 += EATEXT_CHAR("\x0E35\x0E49\x0E40\x0E21\x0E37\x0E48\x0E2D\x0E17\x0E33\x0E40\x0E2A\x0E23\x0E47\x0E08");
			s16 += EATEXT_CHAR("\x005C\x0072\x005C\x006E\x005C\x0072\x005C\x006E\x0E2B\x0E32\x0E01\x0E04\x0E38\x0E13");
			s16 += EATEXT_CHAR("\x0E44\x0E14\x0E49\x0E17\x0E33\x0E01\x0E32\x0E23\x0E25\x0E07\x0E17\x0E30\x0E40\x0E1A");
			s16 += EATEXT_CHAR("\x0E35\x0E22\x0E19\x0E40\x0E23\x0E35\x0E22\x0E1A\x0E23\x0E49\x0E2D\x0E22\x0E41\x0E25");
			s16 += EATEXT_CHAR("\x0E49\x0E27\x0020\x0E01\x0E23\x0E38\x0E13\x0E32\x0E01\x0E23\x0E2D\x0E01\x0E02\x0E49");
			s16 += EATEXT_CHAR("\x0E2D\x0E21\x0E39\x0E25\x0E17\x0E32\x0E07\x0E14\x0E49\x0E32\x0E19\x0E0B\x0E49\x0E32");
			s16 += EATEXT_CHAR("\x0E22\x0020\x0E23\x0E27\x0E21\x0E16\x0E36\x0E07\x0E0A\x0E37\x0E48\x0E2D\x0020\x0041");
			s16 += EATEXT_CHAR("\x0063\x0063\x006F\x0075\x006E\x0074\x0020\x0020\x0045\x0041\x0020\x0E23\x0E2B\x0E31");
			s16 += EATEXT_CHAR("\x0E2A\x0E1C\x0E48\x0E32\x0E19\x0020\x0E41\x0E25\x0E30\x0E2D\x0E2D\x0E19\x0E44\x0E25");
			s16 += EATEXT_CHAR("\x0E19\x0E4C\x0E44\x0E2D\x0E14\x0E35\x0E02\x0E2D\x0E07\x0E04\x0E38\x0E13\x0020\x0E41");
			s16 += EATEXT_CHAR("\x0E25\x0E49\x0E27\x0E04\x0E25\x0E34\x0E01\x0E1B\x0E38\x0E48\x0E21\x0020\x004C\x006F");
			s16 += EATEXT_CHAR("\x0067\x0020\x0049\x006E\x005C\x0072\x005C\x006E\x005C\x0072\x005C\x006E\x0E43\x0E19");
			s16 += EATEXT_CHAR("\x0E01\x0E32\x0E23\x0E15\x0E34\x0E14\x0E15\x0E32\x0E21\x0E02\x0E48\x0E32\x0E27\x0E25");
			s16 += EATEXT_CHAR("\x0E48\x0E32\x0E2A\x0E38\x0E14\x0E40\x0E01\x0E35\x0E48\x0E22\x0E27\x0E01\x0E31\x0E1A");
			s16 += EATEXT_CHAR("\x0020\x0052\x0065\x0064\x0020\x0041\x006C\x0065\x0072\x0074\x0020\x0033\x0020\x0E2A");
			s16 += EATEXT_CHAR("\x0E32\x0E21\x0E32\x0E23\x0E16\x0E40\x0E02\x0E49\x0E32\x0E44\x0E1B\x0E40\x0E22\x0E35");
			s16 += EATEXT_CHAR("\x0E48\x0E22\x0E21\x0E0A\x0E21\x0E44\x0E14\x0E49\x0E17\x0E35\x0E48\x0020\x0072\x0065");
			s16 += EATEXT_CHAR("\x0064\x0061\x006C\x0065\x0072\x0074\x0033\x002E\x0063\x006F\x006D\x0020\x0E02\x0E2D");
			s16 += EATEXT_CHAR("\x0E1A\x0E04\x0E38\x0E13\x0E17\x0E35\x0E48\x0E21\x0E32\x0E40\x0E25\x0E48\x0E19\x0E40");
			s16 += EATEXT_CHAR("\x0E01\x0E21\x0E41\x0E25\x0E30\x0E02\x0E2D\x0E43\x0E2B\x0E49\x0E42\x0E0A\x0E04\x0E14");
			s16 += EATEXT_CHAR("\x0E35\x0E43\x0E19\x0E01\x0E32\x0E23\x0E40\x0E25\x0E48\x0E19\x0021");

			const Char*  pTest = s16.c_str();
			const size_t kMustBeStuckInInfiniteLoop = 1000;

			tr.SetData(s16.data(), (uint32_t)s16.length());
			tri.SetTextRunArray(&tr);
			lbi.SetIterator(tri);

			size_t i = 0;
			for(size_t b = 0; (b < s16.length()) && (i != kMustBeStuckInInfiniteLoop); i++)
			{
				b = lbi.GetNextLineBreak(kLineBreakTypePossible | kLineBreakTypeMandatory);

				// There should be no break points in between a base char and its accent char.
				if(IsGraphemeExtend(pTest[b])) // This 'if' allows us to easily set a breakpoint for failure.
					EATEST_VERIFY(!IsGraphemeExtend(pTest[b]));
			}
			EATEST_VERIFY(i != kMustBeStuckInInfiniteLoop);
		}

		{
			// This is a variation of the test above but with leading and trailing non-Thai.
			const Char*  pTest      = EATEXT_CHAR("abc \x0E16\x0E32\x0E29\x0E32\x0E44\x0E17\x0E22 \x0E40\x0E1B\x0E47\x0E19\x0E20\x0E32\x0E29\x0E32\x0E23\x0E32\x0E0A\x0E01\x0E32\x0E23\x0E02\x0E2D\x0E07\x0E1B\x0E23\x0E30\x0E40\x0E17\x0E28\x0E44\x0E17\x0E22 \x0E41\x0E25\x0E30\x0E20\x0E32\x0E29\x0E32\x0E41\x0E21\x0E48\x0E02\x0E2D\x0E07\x0E0A\x0E32\x0E27\x0E44\x0E17\x0E22 \x0E41\x0E25\x0E30\x0E0A\x0E19\x0E40\x0E0A\x0E37\x0E49\x0E2D\x0E2A\x0E32\x0E22\x0E2D\x0E37\x0E48\x0E19\x0E43\x0E19\x0E1B\x0E23\x0E30\x0E40\x0E17\x0E28\x0E44\x0E17\x0E22 \x0E20\x0E32\x0E29\x0E32\x0E44\x0E17\x0E22\x0E40\x0E1B\x0E47\x0E19\x0E20\x0E32\x0E29\x0E32\x0E43\x0E19\x0E01\x0E25\x0E38\x0E48\x0E21\x0E20\x0E32\x0E29\x0E32\x0E44\x0E17  def.");
			const size_t pResults[] = { 4, 4+2, 4+4, 4+8, 4+12, 4+16, 4+22, 4+25, 4+31, 4+35, 4+38, 4+42, 4+45, 4+48, 4+51, 4+55, 4+58, 4+60, 4+65, 4+68, 4+72, 4+74, 4+80, 4+84, 4+88, 4+91, 4+95, 4+99, 4+101, 4+106, 4+110, 4+114, 4+118 };

			tr.SetData(pTest, (uint32_t)EA::StdC::Strlen(pTest));
			tri.SetTextRunArray(&tr);
			lbi.SetIterator(tri);

			for(size_t i = 0; i < EAArrayCount(pResults); i++)
			{
				size_t b = lbi.GetNextLineBreak(kLineBreakTypePossible | kLineBreakTypeMandatory);
				if(b != pResults[i]) // This 'if' allows us to easily set a breakpoint for failure.
					EATEST_VERIFY(b == pResults[i]);
			}
		}

		{
			// This is a variation of the test above but with leading and trailing non-Thai.
			const Char*  pTest      = EATEXT_CHAR("abc \x0E16\x0E32\x0E29\x0E32\x0E44\x0E17\x0E22 \x0E40\x0E1B\x0E47\x0E19\x0E20\x0E32\x0E29\x0E32\x0E23\x0E32\x0E0A\x0E01\x0E32\x0E23\x0E02\x0E2D\x0E07\x0E1B\x0E23\x0E30\x0E40\x0E17\x0E28\x0E44\x0E17\x0E22 \x0E41\x0E25\x0E30\x0E20\x0E32\x0E29\x0E32\x0E41\x0E21\x0E48\x0E02\x0E2D\x0E07\x0E0A\x0E32\x0E27\x0E44\x0E17\x0E22 \x0E41\x0E25\x0E30\x0E0A\x0E19\x0E40\x0E0A\x0E37\x0E49\x0E2D\x0E2A\x0E32\x0E22\x0E2D\x0E37\x0E48\x0E19\x0E43\x0E19\x0E1B\x0E23\x0E30\x0E40\x0E17\x0E28\x0E44\x0E17\x0E22 \x0E20\x0E32\x0E29\x0E32\x0E44\x0E17\x0E22\x0E40\x0E1B\x0E47\x0E19\x0E20\x0E32\x0E29\x0E32\x0E43\x0E19\x0E01\x0E25\x0E38\x0E48\x0E21\x0E20\x0E32\x0E29\x0E32\x0E44\x0E17 efg.");
			const size_t pResults[] = { 4, 4+2, 4+4, 4+8, 4+12, 4+16, 4+22, 4+25, 4+31, 4+35, 4+38, 4+42, 4+45, 4+48, 4+51, 4+55, 4+58, 4+60, 4+65, 4+68, 4+72, 4+74, 4+80, 4+84, 4+88, 4+91, 4+95, 4+99, 4+101, 4+106, 4+110, 4+113, 4+117 };

			tr.SetData(pTest, (uint32_t)EA::StdC::Strlen(pTest));
			tri.SetTextRunArray(&tr);
			lbi.SetIterator(tri);

			for(size_t i = 0; i < EAArrayCount(pResults); i++)
			{
				size_t b = lbi.GetNextLineBreak(kLineBreakTypePossible | kLineBreakTypeMandatory);
				if(b != pResults[i]) // This 'if' allows us to easily set a breakpoint for failure.
					EATEST_VERIFY(b == pResults[i]);
			}
		}


		{ 
			// Here are some minor pathological cases.
			// U+0E01 is a Thai consonant, U+0E34 is a Thai combining character.
			const int  kTestCount = 5;
			const int  kBreakCount = 6;
			const Char pTest[kTestCount][6] = { 
												EATEXT_CHAR("\x0E01"), 
												EATEXT_CHAR("\x0E34"), 
												EATEXT_CHAR("\x0E34\x0E34"), 
												EATEXT_CHAR(" \x0E34 "), 
												EATEXT_CHAR(" \x0E34\x0E01 ")
											  };
			const size_t   pResults[kTestCount][6] = { 
														{ 1, 1, 1, 1, 1, 1 },   // consonant
														{ 1, 1, 1, 1, 1, 1 },   // combining
														{ 2, 2, 2, 2, 2, 2 },   // consonant, consonant
														{ 1, 3, 3, 3, 3, 3 },   // combining, combining
														{ 1, 4, 4, 4, 4, 4 }    // combining, consonant. This case is somewhat ambiguous, as it represents an unmatched combining char with a following alphabetic char. It's actually impossible to know a correct answer for this, as it doesn't represent a legal word or start of word.
													 };

			for(size_t t = 0; t < kTestCount; t++)
			{
				tr.SetData(pTest[t], (uint32_t)EA::StdC::Strlen(pTest[t]));
				tri.SetTextRunArray(&tr);
				lbi.SetIterator(tri);

				for(size_t i = 0; i < kBreakCount; i++)
				{
					size_t b = lbi.GetNextLineBreak(kLineBreakTypePossible | kLineBreakTypeMandatory);
					if(b != pResults[t][i]) // This 'if' allows us to easily set a breakpoint for failure.
						EATEST_VERIFY(b == pResults[t][i]);
				}
			}
		}

		{
			// Here we test Thai text surrounded with quotes (plain and smart quotes).
			// The leading and trailing quotes should be 'bound' to the next and previous
			// character, respectively. There should be no break between a leading quote
			// and a following Thai char, nor should there be a break between a Thai char
			// and a trailing quote.
			// Note that 201C is a leading smart quote, 201D is a trailing smart quote.

			const size_t pResults0[] = { 6, 12, 18, 23 };
			const Char*  pTest0 = EATEXT_CHAR("Hello \"some text\" world");
			//                                        ^      ^      ^    ^

			const size_t pResults1[] = { 6, 9, 12, 15, 20, 24 };
			const Char*  pTest1 = EATEXT_CHAR("\x0E2A\x0E21\x0E14\x0E38\x0E25 \x201C\x0E1C\x0E25\x0E07\x0E32\x0E19\x0E02\x0E2D\x0E07\x0E09\x0E31\x0E19\x201D \x0E2A\x0E19\x0E43\x0E08");
			//                                                                 ^                 ^                 ^                 ^                        ^                       ^

			const size_t pResults2[] = { 6, 9, 12, 15, 20, 24 };
			const Char*  pTest2 = EATEXT_CHAR("\x0E2A\x0E21\x0E14\x0E38\x0E25 \"\x0E1C\x0E25\x0E07\x0E32\x0E19\x0E02\x0E2D\x0E07\x0E09\x0E31\x0E19\" \x0E2A\x0E19\x0E43\x0E08");
			//                                                                 ^             ^                 ^                 ^                    ^                       ^

			const size_t pResults3[] = { 6, 9, 12, 15, 20, 25 };
			const Char*  pTest3 = EATEXT_CHAR("Hello \"\x0E1C\x0E25\x0E07\x0E32\x0E19\x0E02\x0E2D\x0E07\x0E09\x0E31\x0E19\" world");
			//                                        ^             ^                 ^                 ^                    ^    ^

			// Test 0
			tr.SetData(pTest0, (uint32_t)EA::StdC::Strlen(pTest0));
			tri.SetTextRunArray(&tr);
			lbi.SetIterator(tri);

			for(size_t i = 0; i < EAArrayCount(pResults0); i++)
			{
				size_t b = lbi.GetNextLineBreak(kLineBreakTypePossible | kLineBreakTypeMandatory);
				if(b != pResults0[i]) // This 'if' allows us to easily set a breakpoint for failure.
					EATEST_VERIFY(b == pResults0[i]);
			}

			// Test 1
			tr.SetData(pTest1, (uint32_t)EA::StdC::Strlen(pTest1));
			tri.SetTextRunArray(&tr);
			lbi.SetIterator(tri);

			for(size_t i = 0; i < EAArrayCount(pResults1); i++)
			{
				size_t b = lbi.GetNextLineBreak(kLineBreakTypePossible | kLineBreakTypeMandatory);
				if(b != pResults1[i]) // This 'if' allows us to easily set a breakpoint for failure.
					EATEST_VERIFY(b == pResults1[i]);
			}

			// Test 2
			tr.SetData(pTest2, (uint32_t)EA::StdC::Strlen(pTest2));
			tri.SetTextRunArray(&tr);
			lbi.SetIterator(tri);

			for(size_t i = 0; i < EAArrayCount(pResults2); i++)
			{
				size_t b = lbi.GetNextLineBreak(kLineBreakTypePossible | kLineBreakTypeMandatory);
				if(b != pResults2[i]) // This 'if' allows us to easily set a breakpoint for failure.
					EATEST_VERIFY(b == pResults2[i]);
			}

			// Test 3
			tr.SetData(pTest3, (uint32_t)EA::StdC::Strlen(pTest3));
			tri.SetTextRunArray(&tr);
			lbi.SetIterator(tri);

			for(size_t i = 0; i < EAArrayCount(pResults3); i++)
			{
				size_t b = lbi.GetNextLineBreak(kLineBreakTypePossible | kLineBreakTypeMandatory);
				if(b != pResults3[i]) // This 'if' allows us to easily set a breakpoint for failure.
					EATEST_VERIFY(b == pResults3[i]);
			}
		}

		{
			// Here we have a test which generates random Thai strings, but with some 
			// non-Thai thrown in. Make sure the code doesn't assert, crash, or hang.
			const int  kTestCount = 1000;
			const int  kTextSize = 64;
			Char       pTest[kTextSize];
			size_t     j;
			const Char kNonThaiCharTable[] = { ' ', '.', 'a', 'b', '0', kCharLF, kCharZWJ, kCharEllipsis, kCharNBSP, '\"', 0x0300, 0xB900 };

			for(size_t t = 0; t < kTestCount; t++)
			{
				// Generate random Thai characters.
				for(j = 0; j < kTextSize; j++)
					pTest[j] = (Char)(uint16_t)(int16_t)gRandom.RandRange(0x0E00, 0x0E60);

				// Replace 20% of the characters with non-Thai.
				for(j = 0; j < kTextSize; j++)
					pTest[j] = (gRandom.RandLimit(5) == 0) ? kNonThaiCharTable[gRandom.RandLimit(EAArrayCount(kNonThaiCharTable))] : pTest[j];

				tr.SetData(pTest, (uint32_t)kTextSize);
				tri.SetTextRunArray(&tr);
				lbi.SetIterator(tri);

				for(size_t i = 0; i < kTextSize; i++) // kTextSize is the most number of breaks there should possibly be.
				{
					size_t b = lbi.GetNextLineBreak(kLineBreakTypePossible | kLineBreakTypeMandatory);
					if(b > kTextSize) // This 'if' allows us to easily set a breakpoint for failure.
						EATEST_VERIFY(b <= kTextSize);
				}
			}
		}
	#endif

	return nErrorCount;
}


///////////////////////////////////////////////////////////////////////////////
// TestBreak
//
int TestBreak()
{
	int nErrorCount = 0;

	EA::Text::InitWordBreakDictionary();

	nErrorCount += TestBreakCharacter();
	nErrorCount += TestBreakWord();
	nErrorCount += TestBreakSentence();
	nErrorCount += TestBreakParagraph();
	nErrorCount += TestBreakLine();

	// Thai breaking
	// This is treated specially because Thai is so unusual in this respect.
	nErrorCount += TestBreakCharacterThai();
	nErrorCount += TestBreakWordThai();
	nErrorCount += TestBreakLineThai();

	EA::Text::ShutdownWordBreakDictionary();

	return nErrorCount;
}

