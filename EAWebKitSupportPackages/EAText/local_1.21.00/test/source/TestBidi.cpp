/////////////////////////////////////////////////////////////////////////////
// TestUnicode.cpp
//
// Copyright (c) 2004, Electronic Arts Inc. All rights reserved.
// Created by Paul Pedriana
//
/////////////////////////////////////////////////////////////////////////////


#include "EATextTest.h"
#include <EAText/internal/EATextBidi.h>
#include <EAText/EATextTypesetter.h>
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


#if EATEXT_DLL
int TestBidi()
{
		// Bidi functions are internal and not exported through Dll interface
		return 0;
}

#else

int TestBidi()
{
	using namespace EA::Text;

	int nErrorCount = 0;

	{   

		//////////////////////////////////////////////////////////////////////////////////////////////////////////
		// Read over the test document and run our paired bracket tests.
		// Each line is either a comment (prefixed with '#')
		// or a line formatted as 5 fields: <SRC>;<PDIR>;<RPLEV>;<RLEV>;<VINDEX>
		// where SRC is a list of hex values for the source string;
		// fields with a range from 0 - 2 or less, 1 means the SRC string is RTL, otherwise LT;
		// field 4 is a list of embedding levels per character in the SRC string
		// field 5 (ignored) is the index for each character as the character would appear visually when rendered 
		//////////////////////////////////////////////////////////////////////////////////////////////////////////
		const EA::Text::FilePathChar* kPairedBracketsDataFile = EATEXT_FILE_PATH_CHAR("PairedBracketTest.txt");
		const EA::IO::size_type kBufferSize = 256;
		const size_t kNumCols = 4; // 5; // we ignore the last column since we don't care about visual display order in this test
		const size_t kEntryWidth = 5;

		typedef EATEXT_CHAR_TYPE Char;
		typedef eastl::fixed_string<Char, kBufferSize> TestCharString;
		typedef eastl::vector<int> LevelList;

		struct TestData
		{
			TestCharString mSRCString;          // SRC    - Source string to typeset
			Direction  mParagraphDir;           // PDIR   - Paragraph's direction
			Direction  mResolvedParagraphDir;   // RPLEV  - Resolved Paragraph embedding level
			LevelList  mResolvedLevelList;      // RLEV   - Resolved levels per character
			//LevelList  mVisualCharOrderList;  // VINDEX - Visual index for each character in SRC (ignored)
		};
	   
		size_t numLinesRead = 0;
		size_t testNum = 0;
		EA::IO::size_type lineSize;
		char lineBuffer[kBufferSize];
		Char bufferArray[kBufferSize];
		EA::Text::FilePathChar dataDirPath[EA::IO::kMaxPathLength];
		EATEST_VERIFY(GetTestDataDirectory(dataDirPath, EAArrayCount(dataDirPath)) > 0);

		#if EATEXT_FILE_PATH_CHAR_TYPE_SIZE == 1
			EA::IO::Path::PathString8 pairedBracketsDataPath(dataDirPath);
			EA::IO::Path::PathString8 pairedBracketsDataFile(kPairedBracketsDataFile);
		#else
			EA::IO::Path::PathString16 pairedBracketsDataPath(dataDirPath);
			EA::IO::Path::PathString16 pairedBracketsDataFile(kPairedBracketsDataFile);
		#endif

		EA::IO::Path::Join(pairedBracketsDataPath, pairedBracketsDataFile);
		EA::IO::Path::Normalize(pairedBracketsDataPath);

		EA::IO::FileStream file(pairedBracketsDataPath.c_str());
		file.AddRef();
		EATEST_VERIFY_F(file.Open(), "Failed to open %I8s", pairedBracketsDataPath.c_str());
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

			if(testLine[0] == '#') 
			{
				// Comment - simply ignore this line
			}
			else if(testLine.length() > 0)
			{
				TestData testData;
				testNum++;

				for(size_t j = 0; j < kNumCols; ++j)
				{
					pos = testLine.find(';', prevPos);
					EA_ASSERT(pos < testLine.length());
					switch(j)
					{
						case 0:
						{
							testData.mSRCString = TestCharString(bufferArray, ((pos - prevPos) + 1)/kEntryWidth);
							int charIndex = 0;
							for(size_t i = prevPos; i < pos; i += kEntryWidth)
							{
								const char* p = testLine.c_str() + i;
								int wasRead = EA::StdC::Sscanf(p, "%X", &n);
								EA_UNUSED(wasRead);
								EA_ASSERT_FORMATTED(wasRead, ("Failed to parse line %d :'%s'", numLinesRead, p));

								testData.mSRCString[charIndex++] = (Char)n;
							}
							testData.mSRCString[charIndex] = 0;
							break;
						}
						case 1: // PDIR
						{
							const char* p = testLine.c_str() + prevPos;
							int wasRead = EA::StdC::Sscanf(p, "%d", &n);
							EA_UNUSED(wasRead);
							EA_ASSERT_FORMATTED(wasRead, ("Failed to parse line %d :'%s'", numLinesRead, p));

							testData.mResolvedParagraphDir = n & 1 ? kDirectionRTL : kDirectionLTR;
							break;
						}
						case 2: // RPLEV
						{
							const char* p = testLine.c_str() + prevPos;
							int wasRead = EA::StdC::Sscanf(p, "%d", &n);
							EA_UNUSED(wasRead);
							EA_ASSERT_FORMATTED(wasRead, ("Failed to parse line %d :'%s'", numLinesRead, p));

							testData.mParagraphDir = n & 1 ? kDirectionRTL : kDirectionLTR;
							break;
						}
						case 3: // RLEV
						{
							size_t diff = 0;
							for(size_t i = prevPos; i < pos; i += diff)
							{
								const char* p = testLine.c_str() + i;
								int wasRead = EA::StdC::Sscanf(p, "%d", &n);
								EA_UNUSED(wasRead);
								EA_ASSERT_FORMATTED(wasRead, ("Failed to parse line %d :'%s'", numLinesRead, p));

								if(n > 9)
								{
									diff = 3;
								}
								else
								{
									diff = 2;
								}

								testData.mResolvedLevelList.push_back(n);
							}
							break;
						}
					}
					prevPos = pos + 1; // skip the ';'
				}
				
				Typesetter typesetter;
				EA::Text::LayoutSettings settings;
				settings.mDirection = testData.mParagraphDir;
				typesetter.SetLayoutSettings(settings);
				typesetter.SetLayoutSpace(255.0f, 0.0f, 0.0f);

				typesetter.AddTextRun(&testData.mSRCString[0], testData.mSRCString.length());
				typesetter.FinalizeLine();

				LineLayout& lineLayout = typesetter.GetLineLayout();

				// We can't even bother running some tests unless we actually run the bidi algorithm which is conditional on there being
				// any RTL characters in the string. While technically not correct, this is an optimization we can live with until 
				// there is a strong need for this kind of feature (e.g. webkit desiring to use <rtl> attributes)
				if(BidiClassify(testData.mSRCString.c_str(), &lineLayout.mAnalysisInfoArray[0], testData.mSRCString.length()))
				{
					bool failure = false;
					for(TestString::size_type j = 0 ; j < testData.mSRCString.length() && !failure; ++j)
					{
						failure = (lineLayout.mAnalysisInfoArray[j].mnBidiLevel) != (testData.mResolvedLevelList[j]);
					}
					if(failure)
					{
						EATEST_VERIFY(false);
						#if EATEXT_DEBUG
							EA::EAMain::Report("\n::Test %d Failed::", testNum);
							EA::EAMain::Report("\nSRC: ");
							for(TestString::size_type j = 0; j < testData.mSRCString.length(); ++j)
							{
								EA::EAMain::Report("%04X ", testData.mSRCString[j]);
							}
							EA::EAMain::Report("\nLTR: %s", testData.mParagraphDir == kDirectionLTR ? "Yes" : "No");
							EA::EAMain::Report("\nExpected:\t");
							for(TestString::size_type j = 0 ; j < testData.mSRCString.length(); ++j)
							{
								EA::EAMain::Report("%d ", testData.mResolvedLevelList[j]);                        
							}
							EA::EAMain::Report("\nActual:\t\t");
							for(TestString::size_type j = 0 ; j < testData.mSRCString.length(); ++j)
							{
								EA::EAMain::Report("%d ", lineLayout.mAnalysisInfoArray[j].mnBidiLevel);
							}
						#endif
					}
				}
			}
		} // while there are still lines to read

		stopwatch.Stop();
		EA::EAMain::Report("\tElapsed Time: %I64u ms for %zu lines, %zu test cases.\n", stopwatch.GetElapsedTime(), numLinesRead, testNum);
		bufferedFile.Close();
	}

	return nErrorCount;
}
#endif