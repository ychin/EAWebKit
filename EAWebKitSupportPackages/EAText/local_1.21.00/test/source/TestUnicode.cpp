/////////////////////////////////////////////////////////////////////////////
// TestUnicode.cpp
//
// Copyright (c) 2004, Electronic Arts Inc. All rights reserved.
// Created by Paul Pedriana
//
/////////////////////////////////////////////////////////////////////////////


#include "EATextTest.h"
#include <EAText/EATextUnicode.h>
#include <EAText/EATextCollation.h>
#include <EAStdC/EADateTime.h>
#include <EAStdC/EAString.h>
#include <EAStdC/EAScanf.h>
#include <EAStdC/EAMemory.h>
#include <EAStdC/EAStopwatch.h>

#include <EAIO/EAStreamBuffer.h>
#include <EAIO/EAFileStream.h>
#include <EAIO/EAStreamAdapter.h>
#include <EAIO/EAFileDirectory.h>
#include <EAIO/EAFileUtil.h>
#include <EAMain/EAMain.h>
#include <EATest/EATest.h>
#include EA_ASSERT_HEADER


#if defined (EA_PLATFORM_IPHONE)
	namespace EA{ namespace IO {
		int appleGetHomeDirectory(char8_t* pDirectory, uint32_t nPathCapacity);
		int appleGetAppName(char8_t* pDirectory, uint32_t nPathCapacity);
	}}
#endif


///////////////////////////////////////////////////////////////////////////////
/// EATRACE_TRUE / EATRACE_FALSE
///
/// These expressions are replacements for 'true' and 'false' but which aren't
/// seen as so by the compiler but only at runtime. Compilers such as VC++
/// will generate warnings (such as C4127: conditional expression is constant)
/// when you try to use expressions that it sees as being constant but are 
/// intentionally so. 
///
/// Given that EA_TRUE results in code being generated instead of code being
/// compiled away, you generally only want to use these expressions in code
/// that you don't need to be optimized, such as debug code and test code.
/// For high performance code, you are better off disabling the warning via
/// compiler directives.
///
/// Example usage:
///     #define MY_TRACE(str) do{ puts(str); } while(EATRACE_FALSE)
///
inline int EATrueDummyFunction() { return 1; }

#ifndef EATRACE_TRUE
	#define EATRACE_TRUE  (EATrueDummyFunction())
#endif

#ifndef EATRACE_FALSE
	#define EATRACE_FALSE (!EATrueDummyFunction())
#endif



int TestUnicode()
{
	using namespace EA::Text;

	int nErrorCount = 0;

	{   // IsSpace
		bool bResult;

		// kSTNone
		bResult = IsSpace(kCharZWNBSP, kSTNone);
		EATEST_VERIFY(bResult);

		bResult = IsSpace(kCharSpace, kSTNone);
		EATEST_VERIFY(bResult);

		bResult = IsSpace(kCharNUL, kSTNone);
		EATEST_VERIFY(bResult);

		// kSTWidth
		bResult = IsSpace(kCharENSP, kSTWidth);
		EATEST_VERIFY(bResult);

		bResult = IsSpace(kCharZWNBSP, kSTWidth);
		EATEST_VERIFY(!bResult);

		bResult = IsSpace(kCharSpace, kSTWidth);
		EATEST_VERIFY(bResult);

		bResult = IsSpace(kCharNUL, kSTWidth);
		EATEST_VERIFY(!bResult);

		// kSTZeroWidth
		bResult = IsSpace(kCharENSP, kSTZeroWidth);
		EATEST_VERIFY(!bResult);

		bResult = IsSpace(kCharZWNBSP, kSTZeroWidth);
		EATEST_VERIFY(bResult);

		bResult = IsSpace(kCharSpace, kSTZeroWidth);
		EATEST_VERIFY(!bResult);

		bResult = IsSpace(kCharNUL, kSTZeroWidth);
		EATEST_VERIFY(!bResult);

		// kSTBreak
		bResult = IsSpace(kCharZWNBSP, kSTBreak);
		EATEST_VERIFY(!bResult);

		bResult = IsSpace(kCharSpace, kSTBreak);
		EATEST_VERIFY(bResult);

		bResult = IsSpace(kCharNUL, kSTBreak);
		EATEST_VERIFY(!bResult);

		// kSTNoBreak
		bResult = IsSpace(kCharZWNBSP, kSTNoBreak);
		EATEST_VERIFY(bResult);

		bResult = IsSpace(kCharSpace, kSTNoBreak);
		EATEST_VERIFY(!bResult);

		bResult = IsSpace(kCharNUL, kSTNoBreak);
		EATEST_VERIFY(!bResult);

		// kSTControl
		bResult = IsSpace(kCharZWNBSP, kSTControl);
		EATEST_VERIFY(!bResult);

		bResult = IsSpace(kCharSpace, kSTControl);
		EATEST_VERIFY(!bResult);

		bResult = IsSpace(kCharCR, kSTControl);
		EATEST_VERIFY(bResult);

		bResult = IsSpace(kCharNUL, kSTControl);
		EATEST_VERIFY(!bResult);

		// kSTAll
		bResult = IsSpace(kCharZWNBSP, kSTAll, false);
		EATEST_VERIFY(bResult);

		bResult = IsSpace(kCharSpace, kSTAll, false);
		EATEST_VERIFY(bResult);

		bResult = IsSpace(kCharNUL, kSTAll, false);
		EATEST_VERIFY(!bResult);

		// kSTZeroWidth | kSTBreak
		bResult = IsSpace(kCharZWNBSP, kSTZeroWidth | kSTBreak);
		EATEST_VERIFY(!bResult);

		bResult = IsSpace(kCharSpace, kSTZeroWidth | kSTBreak);
		EATEST_VERIFY(!bResult);

		bResult = IsSpace(kCharZWSP, kSTZeroWidth | kSTBreak);
		EATEST_VERIFY(bResult);

		bResult = IsSpace(kCharNUL, kSTZeroWidth | kSTBreak);
		EATEST_VERIFY(!bResult);

		// kSTWidth | kSTNoBreak
		bResult = IsSpace(kCharZWNBSP, kSTWidth | kSTNoBreak);
		EATEST_VERIFY(!bResult);

		bResult = IsSpace(kCharSpace, kSTWidth | kSTNoBreak);
		EATEST_VERIFY(!bResult);

		bResult = IsSpace(kCharNBSP, kSTWidth | kSTNoBreak);
		EATEST_VERIFY(bResult);

		bResult = IsSpace(kCharNUL, kSTWidth | kSTNoBreak);
		EATEST_VERIFY(!bResult);

		// Lastly we test all possible Chars to make sure we don't crash.
		for(uint32_t i = 0; i < 65536; i++)
		{
			const Char c = (Char)i;

			for(int f = 0; f <= 0x1f; f++) // 0x1f happens to represent all recognized flags.
			{
				bResult = IsSpace(c, f, true);
				EATEST_VERIFY(bResult || EATRACE_TRUE);

				bResult = IsSpace(c, f, false);
				EATEST_VERIFY(bResult || EATRACE_TRUE);
			}
		}

	} // IsSpace tests


	{
		Block block;

		// Do some specific tests here.
		block = GetCharBlock('A');
		EATEST_VERIFY(block == kBlockBasicLatin);

		block = GetCharBlock(0x0590);
		EATEST_VERIFY(block == kBlockHebrew);

		block = GetCharBlock(0x0900);
		EATEST_VERIFY(block == kBlockDevanagari);

		// Test all possible Chars to make sure we don't crash.
		for(uint32_t i = 0; i < 65536; i++)
		{
			const Char c = (Char)i;

			for(int f = 0; f <= 0x1f; f++) // 0x1f happens to represent all recognized flags.
			{
				block = GetCharBlock(c);
				EATEST_VERIFY(((int)block >= 0) && ((int)block <= kBlockSpecials));
			}
		}
	}


	{
		bool  bResult;
		CharCategory cc;

		// Do some specific tests here.
		// (To do)

		// Test all possible Chars to make sure we don't crash.
		for(uint32_t i = 0; i < 65536; i++)
		{
			const Char c = (Char)i;

			for(int f = 0; f <= 0x1f; f++) // 0x1f happens to represent all recognized flags.
			{
				cc = GetCharCategory(c);
				EATEST_VERIFY(((int)cc >= 0) && ((int)cc <= kCCOtherSymbol));

				bResult = IsCharCategory(c, kCCFlagUppercaseLetter);
				EATEST_VERIFY(bResult || EATRACE_TRUE);

				bResult = IsCharCategory(c, kCCFlagMathSymbol);
				EATEST_VERIFY(bResult || EATRACE_TRUE);
			}
		}
	}


	{   // GetCharName
		Char   name[256];
		size_t nStrlen;
		EA::UnitTest::Rand rand((uint32_t)EA::StdC::GetTime());

		#if EA_WINAPI_FAMILY_PARTITION(EA_WINAPI_PARTITION_DESKTOP) // As of this writing only this mode has full support.
			nStrlen = GetCharName(0x02a5, name, 256);

			EATEST_VERIFY((nStrlen != 0) && (nStrlen == EA::StdC::Strlen(name)));
			EATEST_VERIFY(EA::StdC::Stristr(name, EATEXT_CHAR("dz"))      != NULL);
			EATEST_VERIFY(EA::StdC::Stristr(name, EATEXT_CHAR("digraph")) != NULL);
			EATEST_VERIFY(EA::StdC::Stristr(name, EATEXT_CHAR("curl"))    != NULL);
		#endif

		// Do some random conversions just to make sure it produces something and doesn't crash.
		for(uint32_t i = 0; i < 500; i++)
		{
			Char c = (Char)rand.RandRange(0, 65536);
			nStrlen = GetCharName(c, name, 256);

			EATEST_VERIFY(nStrlen != 0);
		}
	}


	{   // GetMirrorChar
		Char c;

		// There are a lot of mirror chars to test.
		c = GetMirrorChar('}');
		EATEST_VERIFY(c == '{');

		c = GetMirrorChar('>');
		EATEST_VERIFY(c == '<');

		c = GetMirrorChar('1');
		EATEST_VERIFY(c == '1');
	}


	{   // Misc categorization functions.
		bool  bResult;

		// Do some specific tests here.
		// (To do)

		// Test all possible Chars to make sure we don't crash.
		for(uint32_t i = 0; i < 65536; i++)
		{
			const Char c = (Char)i;

			for(int f = 0; f <= 0x1f; f++) // 0x1f happens to represent all recognized flags.
			{
				bResult = IsCharAlphabetic(c);
				EATEST_VERIFY(bResult || EATRACE_TRUE);

				bResult = IsCharIdeographic(c);
				EATEST_VERIFY(bResult || EATRACE_TRUE);

				bResult = IsCharWhiteSpace(c);
				EATEST_VERIFY(bResult || EATRACE_TRUE);

				bResult = IsCharSTerm(c);
				EATEST_VERIFY(bResult || EATRACE_TRUE);

				bResult = IsCharHyphen(c);
				EATEST_VERIFY(bResult || EATRACE_TRUE);

				bResult = IsCharUppercase(c);
				EATEST_VERIFY(bResult || EATRACE_TRUE);

				bResult = IsGraphemeExtend(c);
				EATEST_VERIFY(bResult || EATRACE_TRUE);

				HangulSyllableType hst = GetHangulSyllableType(c);
				EATEST_VERIFY(((int)hst >= 0) && ((int)hst <= kHSTLVTSyllable));

				LineBreakCategory lbc = GetLineBreakCategory(c);
				EATEST_VERIFY(((int)lbc >= 0) && ((int)lbc <= kLBCUnknown));
			}
		}
	}

	// Normalization Smoke Test
	// Testing simple ASCII which should remain unchanged via all normalization forms
	{
		const EA::IO::size_type kBufferSize = 256;
		typedef EATEXT_CHAR_TYPE Char;
		typedef eastl::fixed_string<Char, kBufferSize> FixedString;

		const Char* simpleText = EATEXT_CHAR("ABCDEFG");
		FixedString srcString;
		FixedString resString;
		
		srcString = simpleText;
		resString.reserve(srcString.length());

		EATEST_VERIFY(!!EA::Text::Normalize(&srcString[0], srcString.length(), &resString[0], resString.capacity(), EA::Text::NormalizationType::kNT_NFC));
		EATEST_VERIFY(EA::StdC::Strncmp(srcString.c_str(), resString.c_str(), resString.length()) == 0);
		EATEST_VERIFY(!!EA::Text::Normalize(&srcString[0], srcString.length(), &resString[0], resString.capacity(), EA::Text::NormalizationType::kNT_NFD));
		EATEST_VERIFY(EA::StdC::Strncmp(srcString.c_str(), resString.c_str(), resString.length()) == 0);
		EATEST_VERIFY(!!EA::Text::Normalize(&srcString[0], srcString.length(), &resString[0], resString.capacity(), EA::Text::NormalizationType::kNT_NFKC));
		EATEST_VERIFY(EA::StdC::Strncmp(srcString.c_str(), resString.c_str(), resString.length()) == 0);
		EATEST_VERIFY(!!EA::Text::Normalize(&srcString[0], srcString.length(), &resString[0], resString.capacity(), EA::Text::NormalizationType::kNT_NFKD));
		EATEST_VERIFY(EA::StdC::Strncmp(srcString.c_str(), resString.c_str(), resString.length()) == 0);
	}

	{   // Normalization functions
		// bool IsNormalized(const Char* pText, uint32_t nTextLength, NormalizationType nt);

		// uint32_t Normalize(const Char* pTextInput, uint32_t nTextInputLength,
		//                    Char* pTextOutput, uint32_t nTextOutputCapacity, NormalizationType nt);

		/////////////////////////////////////////////////////////////////////////////////////////////
		// Read over the test document and run our normalization tests.
		// Each line is either a comment (prefixed with '#'), a test label (prefixed with '@')
		// or a line formatted as <SRC>;<NFC>;<NFD>;<NFKC>;<NFKD>
		/////////////////////////////////////////////////////////////////////////////////////////////
		typedef EATEXT_CHAR_TYPE Char;

		const EA::Text::FilePathChar* kNormalizationDataFile = EATEXT_FILE_PATH_CHAR("NormalizationTest.txt");
		const EA::IO::size_type kBufferSize = 256;
		const size_t kNumCols = 5;
		const size_t kEntryWidth = 5;
		size_t numLinesRead = 0;

		EA::IO::size_type lineSize;
		char lineBuffer[kBufferSize];
		Char bufferArray[kNumCols][kBufferSize];
		eastl::fixed_string<Char, kBufferSize> stringArray[kNumCols];

		Char normalizedBuffer[kBufferSize];
		
		// For convenience and clarity when reading the identities in the tests
		#define SRC  stringArray[0]
		#define NFC  stringArray[1]
		#define NFD  stringArray[2]
		#define NFKC stringArray[3]
		#define NFKD stringArray[4]       

		EA::Text::FilePathChar dataDirPath[EA::IO::kMaxPathLength];
		EATEST_VERIFY(GetTestDataDirectory(dataDirPath, EAArrayCount(dataDirPath)) > 0);

		#if EATEXT_FILE_PATH_CHAR_TYPE_SIZE == 1
			EA::IO::Path::PathString8 normalizationDataPath(dataDirPath);
			EA::IO::Path::PathString8 normalizationDataFile(kNormalizationDataFile);
		#else
			EA::IO::Path::PathString16 normalizationDataPath(dataDirPath);
			EA::IO::Path::PathString16 normalizationDataFile(kNormalizationDataFile);
		#endif

		EA::IO::Path::Join(normalizationDataPath, normalizationDataFile);
		EA::IO::Path::Normalize(normalizationDataPath);

		EA::IO::FileStream file(normalizationDataPath.c_str());
		file.AddRef();
		EATEST_VERIFY_F(file.Open(), "Failed to open %I8s", normalizationDataPath.c_str());
		EA::IO::StreamBuffer bufferedFile(EA::IO::StreamBuffer::kBufferSizeReadDefault, 0, &file);

		EA::StdC::Stopwatch      stopwatch(EA::StdC::Stopwatch::kUnitsMilliseconds, true);
		EA::StdC::LimitStopwatch limitStopwatch(EA::StdC::Stopwatch::kUnitsSeconds, 20, true);

		while((lineSize = EA::IO::ReadLine(&bufferedFile, lineBuffer, kBufferSize)) < EA::IO::kSizeTypeDone) // While there there are lines to read
		{
			eastl::fixed_string<char, kBufferSize> testLine(lineBuffer, lineSize);            
			eastl_size_t pos = 0;
			eastl_size_t prevPos = 0;
			unsigned n;

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
			else if(testLine[0] == '@') // Test label
			{
				EA::EAMain::Report("\t%s\n", testLine.c_str());
			}
			else
			{
				// Build test data for comparisons by filling our SRC, NFD, NFC, NFKD and NFCK strings
				for(size_t j = 0; j < kNumCols; ++j)
				{
					pos = testLine.find(';', prevPos);
					EA_ASSERT(pos < testLine.length());

					stringArray[j] = eastl::fixed_string<Char, kBufferSize>(bufferArray[j], (pos - prevPos + 1)/kEntryWidth);

					int charIndex = 0;
					for(size_t i = prevPos; i < pos; i += kEntryWidth)
					{
						const char* p = testLine.c_str() + i;
						int wasRead = EA::StdC::Sscanf(p, "%X", &n);
						if(!wasRead)
						{
							n = 0;
						}

						stringArray[j][charIndex++] = (Char)n;
					}

					prevPos = pos + 1;
				}

				//    NFD Test Identities
				//
				//    NFD  ==  toNFD(SRC)   ==  toNFD(NFC)   ==  toNFD(NFD)
				//    NFKD ==  toNFD(NFKC)  ==  toNFD(NFKD)
				/////////////////////////////////////////////////////////////////////////////////////////
				//*

				Normalize(SRC.c_str(), SRC.size(), normalizedBuffer, kBufferSize, kNT_NFD);
				EATEST_VERIFY(EA::StdC::Strncmp(normalizedBuffer, NFD.data(), NFD.size()) == 0);
				Normalize(NFC.c_str(), NFC.size(), normalizedBuffer, kBufferSize, kNT_NFD);
				EATEST_VERIFY(EA::StdC::Strncmp(normalizedBuffer, NFD.data(), NFD.size()) == 0);
				Normalize(NFD.c_str(), NFD.size(), normalizedBuffer, kBufferSize, kNT_NFD);
				EATEST_VERIFY(EA::StdC::Strncmp(normalizedBuffer, NFD.data(), NFD.size()) == 0);
				Normalize(NFKC.c_str(), NFKC.size(), normalizedBuffer, kBufferSize, kNT_NFD);
				EATEST_VERIFY(EA::StdC::Strncmp(normalizedBuffer, NFKD.data(), NFKD.size()) == 0);
				Normalize(NFKD.c_str(), NFKD.size(), normalizedBuffer, kBufferSize, kNT_NFD);
				EATEST_VERIFY(EA::StdC::Strncmp(normalizedBuffer, NFKD.data(), NFKD.size()) == 0);

				if (EA::StdC::Strncmp(SRC.c_str(), NFD.c_str(), SRC.length()) != 0)
				{
					EATEST_VERIFY(!IsNormalized(SRC.c_str(), SRC.size(), kNT_NFD));
					EATEST_VERIFY(IsNormalized(NFD.c_str(), NFD.size(), kNT_NFD));
				}

				//*/
				//    NFC Test Identities
				//
				//    NFC ==  toNFC(SRC) ==  toNFC(NFC) ==  toNFC(NFD)
				//    NFKC ==  toNFC(NFKC) ==  toNFC(NFKD)
				//////////////////////////////////////////////////////////////////////////////////////////
				//*

				Normalize(SRC.c_str(), SRC.length(), normalizedBuffer, kBufferSize, kNT_NFC);
				bool result = EA::StdC::Strncmp(normalizedBuffer, NFC.data(), NFC.length()) == 0;
				EATEST_VERIFY(result);
				if (!result)
				{
					EA::EAMain::Report("\nSRC: ");
					for (unsigned int i = 0; i < SRC.length(); ++i)
					{
						EA::EAMain::Report("%04X ", SRC[i]);
					}
					EA::EAMain::Report("\nNFC: ");
					for (unsigned int i = 0; i < NFC.length(); ++i)
					{
						EA::EAMain::Report("%04X ", NFC[i]);
					}
					EA::EAMain::Report("\nResult: ");
					for (unsigned int i = 0; i < kBufferSize; ++i)
					{
						EA::EAMain::Report("%04X ", normalizedBuffer[i]);
					}
					EA::EAMain::Report("\n");
					Normalize(SRC.c_str(), SRC.length(), normalizedBuffer, kBufferSize, kNT_NFC);
				}

				Normalize(NFC.c_str(), NFC.length(), normalizedBuffer, kBufferSize, kNT_NFC);
				EATEST_VERIFY(EA::StdC::Strncmp(normalizedBuffer, NFC.data(), NFC.length()) == 0);
				Normalize(NFD.c_str(), NFD.length(), normalizedBuffer, kBufferSize, kNT_NFC);
				EATEST_VERIFY(EA::StdC::Strncmp(normalizedBuffer, NFC.data(), NFC.length()) == 0);
				Normalize(NFKC.c_str(), NFKC.length(), normalizedBuffer, kBufferSize, kNT_NFC);
				EATEST_VERIFY(EA::StdC::Strncmp(normalizedBuffer, NFKC.data(), NFKC.length()) == 0);
				Normalize(NFKD.c_str(), NFKD.length(), normalizedBuffer, kBufferSize, kNT_NFC);
				EATEST_VERIFY(EA::StdC::Strncmp(normalizedBuffer, NFKC.data(), NFKC.length()) == 0);

				if (EA::StdC::Strncmp(SRC.c_str(), NFC.c_str(), SRC.length()) != 0)
				{
					EATEST_VERIFY(!IsNormalized(SRC.c_str(), SRC.length(), kNT_NFC));
					EATEST_VERIFY(IsNormalized(NFC.c_str(), NFC.length(), kNT_NFC));
				}

				//*/
				//    NFKD Test Identities
				//
				//    NFKD == toNFKD(SRC) == toNFKD(NFC) == toNFKD(NFD) == toNFKD(NFKC) == toNFKD(NFKD)
				//////////////////////////////////////////////////////////////////////////////////////////
				//*

				Normalize(SRC.c_str(), SRC.size(), normalizedBuffer, kBufferSize, kNT_NFKD);
				EATEST_VERIFY(EA::StdC::Strncmp(normalizedBuffer, NFKD.data(), NFKD.size()) == 0);
				Normalize(NFC.c_str(), NFC.size(), normalizedBuffer, kBufferSize, kNT_NFKD);
				EATEST_VERIFY(EA::StdC::Strncmp(normalizedBuffer, NFKD.data(), NFKD.size()) == 0);
				Normalize(NFD.c_str(), NFD.size(), normalizedBuffer, kBufferSize, kNT_NFKD);
				EATEST_VERIFY(EA::StdC::Strncmp(normalizedBuffer, NFKD.data(), NFKD.size()) == 0);
				Normalize(NFKC.c_str(), NFKC.size(), normalizedBuffer, kBufferSize, kNT_NFKD);
				EATEST_VERIFY(EA::StdC::Strncmp(normalizedBuffer, NFKD.data(), NFKD.size()) == 0);
				Normalize(NFKD.c_str(), NFKD.size(), normalizedBuffer, kBufferSize, kNT_NFKD);
				EATEST_VERIFY(EA::StdC::Strncmp(normalizedBuffer, NFKD.data(), NFKD.size()) == 0);

				if (EA::StdC::Strncmp(SRC.c_str(), NFKD.c_str(), SRC.length()) != 0)
				{
					EATEST_VERIFY(!IsNormalized(SRC.c_str(), SRC.size(), kNT_NFKD));
					EATEST_VERIFY(IsNormalized(NFKD.c_str(), NFKD.size(), kNT_NFKD));
				}

				//*/
				//    NFKC Test Identities
				//
				//    NFKC == toNFKC(SRC) == toNFKC(NFC) == toNFKC(NFD) == toNFKC(NFKC) == toNFKC(NFKD)
				//////////////////////////////////////////////////////////////////////////////////////////
				//*
				
				Normalize(SRC.c_str(), SRC.length(), normalizedBuffer, kBufferSize, kNT_NFKC);
				EATEST_VERIFY(EA::StdC::Strncmp(normalizedBuffer, NFKC.data(), NFKC.length()) == 0);
				Normalize(NFC.c_str(), NFC.length(), normalizedBuffer, kBufferSize, kNT_NFKC);
				EATEST_VERIFY(EA::StdC::Strncmp(normalizedBuffer, NFKC.data(), NFKC.length()) == 0);
				Normalize(NFD.c_str(), NFD.length(), normalizedBuffer, kBufferSize, kNT_NFKC);
				EATEST_VERIFY(EA::StdC::Strncmp(normalizedBuffer, NFKC.data(), NFKC.length()) == 0);
				Normalize(NFKC.c_str(), NFKC.length(), normalizedBuffer, kBufferSize, kNT_NFKC);
				EATEST_VERIFY(EA::StdC::Strncmp(normalizedBuffer, NFKC.data(), NFKC.length()) == 0);
				Normalize(NFKD.c_str(), NFKD.length(), normalizedBuffer, kBufferSize, kNT_NFKC);
				EATEST_VERIFY(EA::StdC::Strncmp(normalizedBuffer, NFKC.data(), NFKC.length()) == 0);

				if (EA::StdC::Strncmp(SRC.c_str(), NFKC.c_str(), SRC.length()) != 0)
				{
					EATEST_VERIFY(!IsNormalized(SRC.c_str(), SRC.length(), kNT_NFKC));
					EATEST_VERIFY(IsNormalized(NFKC.c_str(), NFKC.length(), kNT_NFKC));
				}

				//*/
				// End Normalization tests
			}
		} // while there are still lines to read

		stopwatch.Stop();
		EA::EAMain::Report("Elapsed Time: %I64u ms for %zu lines.\n", stopwatch.GetElapsedTime(), numLinesRead);
		bufferedFile.Close();
	}

	
	{   // Case
		{
			EA::Text::CaseType ct;

			ct = GetCase(0);
			EATEST_VERIFY(ct == kCaseTypeNone);

			ct = GetCase('a');
			EATEST_VERIFY(ct == kCaseTypeLower);

			ct = GetCase('A');
			EATEST_VERIFY(ct == kCaseTypeUpper);

			ct = GetCase(0x0158);
			EATEST_VERIFY(ct == kCaseTypeUpper);

			ct = GetCase(0x0159);
			EATEST_VERIFY(ct == kCaseTypeLower);

			ct = GetCase(0x3000);
			EATEST_VERIFY(ct == kCaseTypeNone);
		}

		{
			const size_t   kSize = 10;
			EA::Text::Char strIn[kSize] = { 0x0001, 'a', 'A', 'z', 'Z', 0x0158, 0x0159, 0x3000, '_', 0 };
			EA::Text::Char strOut[kSize];
			uint32_t       n;

			n = ConvertCase(strIn, strOut, sizeof(strIn)/sizeof(strIn[0]), EA::Text::kCaseTypeNone);
			EATEST_VERIFY(n == (kSize - 1));
			EATEST_VERIFY((strOut[0] == 0x0001) && 
				   (strOut[1] == 'a')    && (strOut[2] == 'A')    && 
				   (strOut[3] == 'z')    && (strOut[4] == 'Z')    && 
				   (strOut[5] == 0x0158) && (strOut[6] == 0x0159) && 
				   (strOut[7] == 0x3000) && (strOut[8] == '_') && (strOut[9] == 0));


			n = ConvertCase(strIn, strOut, sizeof(strIn)/sizeof(strIn[0]), EA::Text::kCaseTypeLower);
			EATEST_VERIFY(n == (kSize - 1));
			EATEST_VERIFY((strOut[0] == 0x0001) && 
				   (strOut[1] == 'a')    && (strOut[2] == 'a')    && 
				   (strOut[3] == 'z')    && (strOut[4] == 'z')    && 
				   (strOut[5] == 0x0159) && (strOut[6] == 0x0159) && 
				   (strOut[7] == 0x3000) && (strOut[8] == '_') && (strOut[9] == 0));

			n = ConvertCase(strIn, strOut, sizeof(strIn)/sizeof(strIn[0]), EA::Text::kCaseTypeUpper);
			EATEST_VERIFY(n == (kSize - 1));
			EATEST_VERIFY((strOut[0] == 0x0001) && 
				   (strOut[1] == 'A')    && (strOut[2] == 'A') && 
				   (strOut[3] == 'Z')    && (strOut[4] == 'Z') && 
				   (strOut[5] == 0x0158) && (strOut[6] == 0x0158) && 
				   (strOut[7] == 0x3000) && (strOut[8] == '_') && (strOut[9] == 0));

			Char cff;
			memset(&cff, 0xff, sizeof(cff)); // cff may be 16 or 32 bits.

			memset(strIn, 0xff, kSize); strIn[kSize-1] = 0;
			memset(strOut, 0xff, kSize); strOut[kSize-1] = 0;
			n = ConvertCase(strIn, strOut, 0, EA::Text::kCaseTypeNone);
			EATEST_VERIFY((n == kSize - 1) && (strOut[0] == cff));

			memset(strIn,  0xff, kSize); strIn[kSize-1] = 0;
			memset(strOut, 0xff, kSize); strOut[kSize-1] = 0;
			n = ConvertCase(strIn, strOut, 1, EA::Text::kCaseTypeNone);
			EATEST_VERIFY((n == kSize - 1) && (strOut[0] == 0x00) && (strOut[1] == cff));

			memset(strIn, 0xff, kSize); strIn[kSize-1] = 0;
			memset(strOut, 0xff, kSize); strOut[kSize-1] = 0;
			n = ConvertCase(strIn, strOut, 0, EA::Text::kCaseTypeLower);
			EATEST_VERIFY((n == kSize - 1) && (strOut[0] == cff));

			memset(strIn, 0xff, kSize); strIn[kSize-1] = 0;
			memset(strOut, 0xff, kSize); strOut[kSize-1] = 0;
			n = ConvertCase(strIn, strOut, 1, EA::Text::kCaseTypeLower);
			EATEST_VERIFY((n == kSize - 1) && (strOut[0] == 0x00) && (strOut[1] == cff));

			memset(strIn, 0xff, kSize); strIn[kSize-1] = 0;
			memset(strOut, 0xff, kSize); strOut[kSize-1] = 0;
			n = ConvertCase(strIn, strOut, 0, EA::Text::kCaseTypeUpper);
			EATEST_VERIFY((n == kSize - 1) && (strOut[0] == cff));

			memset(strIn, 0xff, kSize); strIn[kSize-1] = 0;
			memset(strOut, 0xff, kSize); strOut[kSize-1] = 0;
			n = ConvertCase(strIn, strOut, 1, EA::Text::kCaseTypeUpper);
			EATEST_VERIFY((n == kSize - 1) && (strOut[0] == 0x00) && (strOut[1] == cff));


			TestString s16 = EATEXT_CHAR("AaZz$");
			MakeLower(s16);
			EATEST_VERIFY(s16 == EATEXT_CHAR("aazz$"));

			s16 = EATEXT_CHAR("AaZz$");
			MakeUpper(s16);
			EATEST_VERIFY(s16 == EATEXT_CHAR("AAZZ$"));
		}
	}

	{
		// uint32_t ASCIICompatibleEncoding(const Char* pTextInput, uint32_t nTextLength, 
		//                                  char8_t* pTextOutput, uint32_t nTextOutputCapacity, 
		//                                  const UnicodeOptions* pUnicodeOptions = NULL);

		EA::UnitTest::Rand rand((uint32_t)EA::StdC::GetTime());

		// Do some random conversions just to make sure it produces something and doesn't crash.
		for(uint32_t i = 0; i < 500; i++)
		{
			const size_t kMaxInputStrlen = 20;
			Char         buffer[kMaxInputStrlen + 1];
			uint32_t     j, jEnd;

			for(j = 0, jEnd = rand.RandRange(0, kMaxInputStrlen); j < jEnd; j++)
				buffer[j] = (Char)rand.RandRange(0, 65536);
			buffer[j] = 0;

			// Test this call a couple ways.
			char8_t  output8[kMaxInputStrlen * 8];
			uint32_t result = ASCIICompatibleEncoding(buffer, j, output8, EAArrayCount(output8), NULL);
			EATEST_VERIFY((result >= j) && (result < EAArrayCount(output8))); // ASCIICompatibleEncoding returns a string that is at least as long as the input string.

			UnicodeOptions uc; 
			uc.mbNoNullTerminate = true;
			result = ASCIICompatibleEncoding(buffer, j, output8, EAArrayCount(output8), &uc);
			EATEST_VERIFY((result >= j) && (result < EAArrayCount(output8))); // ASCIICompatibleEncoding returns a string that is at least as long as the input string.
		}
	}

	return nErrorCount;
}
