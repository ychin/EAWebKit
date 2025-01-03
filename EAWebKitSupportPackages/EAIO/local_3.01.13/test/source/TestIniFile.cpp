/////////////////////////////////////////////////////////////////////////////
// TestIniFile.cpp
//
// Copyright (c) 2009, Electronic Arts Inc. All rights reserved.
// Written by Paul Pedriana.
/////////////////////////////////////////////////////////////////////////////


#include <EAIO/internal/Config.h>
#include <EAIO/EAIniFile.h>
#include <EAIO/EAFileUtil.h>
#include <EAIO/EAStreamFixedMemory.h>
#include <EAIO/EAStreamMemory.h>
#include <EAStdC/EAString.h>
#include <EAStdC/EAEndian.h>
#include <EAStdC/EAScanf.h>
#include <EAIOTest/EAIO_Test.h>
#include <EATest/EATest.h>
#include <EASTL/vector.h>
#include <EASTL/string.h>
#include <stdio.h>
#include <iostream>
#include EA_ASSERT_HEADER


#if EAIO_INIFILE_ENABLED
	struct EnumContext
	{
		typedef eastl::pair<EA::IO::StringW, EA::IO::StringW> tDataEntry;
		typedef eastl::vector<tDataEntry>                     tDataEntryArray;

		tDataEntryArray mData;
		bool            mbKeyValueEnumeration;

		void Clear()
			{ mData.clear(); }
	};


	static bool IniFileCallback(const wchar_t* param1, const wchar_t* param2, void* pContext)
	{
		EnumContext* pec = (EnumContext*) pContext;

		if(pec->mbKeyValueEnumeration)
			pec->mData.push_back(EnumContext::tDataEntry(param1, param2)); // within a section    
		else 
			pec->mData.push_back(EnumContext::tDataEntry(param1, param1)); // sections

		return true;
	}
#endif



///////////////////////////////////////////////////////////////////////////////
// TestIniFile
//
int TestIniFile()
{
	#if EAIO_INIFILE_ENABLED
		using namespace EA::IO;
		using namespace EA::UnitTest;

		int nErrorCount = 0;

		{
			const char* name8 = "abc";
			const wchar_t* nameW = L"abc";

			// Test ctor usage.
			IniFile initFile0;
			IniFile initFile1(name8);
			IniFile initFile2(nameW);
			IniFile initFile3(name8, EA::IO::IniFile::kOptionNone, EA::IO::GetAllocator());
			IniFile initFile4(nameW, EA::IO::IniFile::kOptionLeaveFileOpen, EA::Allocator::ICoreAllocator::GetDefaultAllocator());

		#if EA_CHAR16_NATIVE
			const char16_t* name16 = u"abc";
			IniFile initFile5(name16);
			IniFile initFile6(name16, EA::IO::IniFile::kOptionNone, EA::IO::GetAllocator());
		#endif
		}

		// What we have here is a starting ini file image for this test. 
		// 
		const char pIniFileData8[] = 
			"; Comment string\r\n"
			"; Comment string\r\n"
			";Comment string\r\n"
			";Comment string\r\n"
			"[Section 1]\r\n"
			"Key 1 = Value 1\n"
			"Key 2 = Value 2\n"
			"Key 3 = Value 3\n"
			"Key 4 = Value 4\n"
			"Key 5 = Value 5\n"
			"Key 6 = Value 6\n"
			"Key 7 = Value 7\n"
			"Key 8 = Value 8\n"
			"Key 9 = Value 9\n"
			"; Comment string\n"
			"; Comment string\r\n"
			"\r\n"
			"[Section 2]\r\n"
			"Key 1 = Value 1\r\n"
			"Key 2 = Value 2\r\n"
			"Key 3 = Value 3\r\n"
			"Key 4 = Value 4\r\n"
			"; Comment string\r\n"
			"Key 5 = Value 5\r\n"
			"Key 6 = Value 6\r\n"
			"Key 7 = Value 7\r\n"
			"Key 8 = Value 8\r\n"
			"Key 9 = Value 9\r\n"
			"\r\n"
			"\r\n"
			"\r\n"
			"[Section 3]\r\n"
			";Comment string\r\n"
			"Key 1 = Value 1\r\n"
			"Key 2 = Value 2\r\n"
			"Key 3 = Value 3\r\n"
			"Key 4 = Value 4\r\n"
			"Key 5 = Value 5\r\n"
			"Key 6 = Value 6\r\n"
			"Key 7 = Value 7\r\n"
			"Key 8 = Value 8\r\n"
			"Key 9 = Value 9\r\n"
			"; Comment string";

		String8  sIniFileData8  = pIniFileData8;
		String16 sIniFileData16 = EA::StdC::Strlcpy<String16, char>(pIniFileData8);
		String32 sIniFileData32 = EA::StdC::Strlcpy<String32, char>(pIniFileData8);
		StringW  sIniFileDataW  = EA::StdC::Strlcpy<StringW,  char>(pIniFileData8);

		{
			StringW  sEntry;
			String16 sEntry16;
			String8  sEntry8;
			bool     bResult;
			int      nResult;

			for(size_t i = 0; i < 5; i++)
			{
				EA::IO::MemoryStream stream;

				stream.AddRef();
				stream.SetOption(EA::IO::MemoryStream::kOptionResizeEnabled, 1);
				stream.SetPosition(0);

				if(i == 0)
					stream.SetData(&sIniFileData8[0], sIniFileData8.size() * sizeof(char), false, true);
				else if(i == 1)
					stream.SetData(&sIniFileData16[0], sIniFileData16.size() * sizeof(char16_t), false, true);
				else if(i == 2)
					stream.SetData(&sIniFileData32[0], sIniFileData32.size() * sizeof(char32_t), false, true);
				else if(i == 3)
				{
					for(eastl_size_t ii = 0; ii < sIniFileData16.size(); ii++)
						sIniFileData16[ii] = (char16_t)EA::StdC::Swizzle((uint16_t)sIniFileData16[ii]);
					stream.SetData(&sIniFileData16[0], sIniFileData16.size() * sizeof(char16_t), false, true);
				}
				else if(i == 4)
				{
					for(eastl_size_t ii = 0; ii < sIniFileData32.size(); ii++)
						sIniFileData32[ii] = (char32_t)EA::StdC::Swizzle((uint32_t)sIniFileData32[ii]);
					stream.SetData(&sIniFileData32[0], sIniFileData32.size() * sizeof(char32_t), false, true);
				}

				IniFile iniFile(&stream);
				iniFile.Open(EA::IO::kAccessFlagReadWrite);

				// ReadEntry / WriteEntry
				nResult = iniFile.ReadEntry(EA_WCHAR("Graphics"), EA_WCHAR("Default Resolution"), sEntry);
				EATEST_VERIFY(nResult == -1);

				nResult = iniFile.ReadEntry(EA_CHAR16("Graphics"), EA_CHAR16("Default Resolution"), sEntry16);
				EATEST_VERIFY(nResult == -1);

				bResult = iniFile.WriteEntryFormatted(EA_WCHAR("Graphics"), EA_WCHAR("Default Resolution"), EA_WCHAR("%ix%i"), 800, 600); 
				EATEST_VERIFY(bResult);

				nResult = iniFile.ReadEntry(EA_WCHAR("Graphics"), EA_WCHAR("Default Resolution"), sEntry);
				EATEST_VERIFY(nResult > 0 && sEntry == EA_WCHAR("800x600"));

				bResult = iniFile.WriteEntryFormatted(EA_CHAR16("Graphics"), EA_CHAR16("Default Resolution"), EA_CHAR16("%ix%i"), 123, 321);
				EATEST_VERIFY(bResult);

				nResult = iniFile.ReadEntry(EA_CHAR16("Graphics"), EA_CHAR16("Default Resolution"), sEntry16);
				EATEST_VERIFY(nResult > 0 && sEntry16 == EA_CHAR16("123x321"));

				bResult = iniFile.WriteEntryFormatted("Graphics", "Default Resolution", "%ix%i", 3000, 2000); 
				EATEST_VERIFY(bResult);

				nResult = iniFile.ReadEntry("Graphics", "Default Resolution", sEntry8);
				EATEST_VERIFY(nResult > 0 && sEntry8 == "3000x2000");

				nResult = iniFile.ReadEntry(EA_WCHAR("Graphics"), EA_WCHAR("Not Present"), sEntry);
				EATEST_VERIFY(nResult  == -1);

				nResult = iniFile.ReadEntry(EA_WCHAR("Not Present"), EA_WCHAR("Default Resolution"), sEntry);
				EATEST_VERIFY(nResult  == -1);


				// ReadEntry / WriteEntry
				nResult = iniFile.ReadEntry(EA_WCHAR("Section 2"), EA_WCHAR("Key 5"), sEntry);
				EATEST_VERIFY(nResult > 0 && sEntry == EA_WCHAR("Value 5"));

				bResult = iniFile.WriteEntry(EA_WCHAR("Section 2"), EA_WCHAR("Key 5"), EA_WCHAR("Value 55")); 
				EATEST_VERIFY(bResult);

				nResult = iniFile.ReadEntry(EA_WCHAR("Section 2"), EA_WCHAR("Key 5"), sEntry);
				EATEST_VERIFY(nResult > 0 && sEntry == EA_WCHAR("Value 55"));


				// Enumeration
				EnumContext ec;
				ec.mbKeyValueEnumeration = false;
				nResult = iniFile.EnumSections(IniFileCallback, &ec);
				EATEST_VERIFY(nResult == 4);
				EATEST_VERIFY(ec.mData.size() == 4);

				ec.Clear();
				ec.mbKeyValueEnumeration = true;
				nResult = iniFile.EnumEntries(EA_WCHAR("Section 3"), IniFileCallback, &ec);
				EATEST_VERIFY(nResult == 9);
				EATEST_VERIFY(ec.mData.size() == 9);


				// ReadEntry / WriteEntry
				bResult = iniFile.WriteEntry(EA_WCHAR("Section 2"), EA_WCHAR("Key Section 2"), EA_WCHAR("\"Value Section 2\"")); 
				EATEST_VERIFY(bResult);

				bResult = iniFile.WriteEntry(EA_WCHAR("Section 1"), EA_WCHAR("Key Section 1"), EA_WCHAR("\"Value Section 1\"")); 
				EATEST_VERIFY(bResult);

				nResult = iniFile.ReadEntry(EA_WCHAR("Section 2"), EA_WCHAR("Key Section 2"), sEntry);
				EATEST_VERIFY(nResult > 0 && sEntry == EA_WCHAR("\"Value Section 2\""));

				nResult = iniFile.ReadEntry(EA_WCHAR("Section 1"), EA_WCHAR("Key Section 1"), sEntry);
				EATEST_VERIFY(bResult && sEntry == EA_WCHAR("\"Value Section 1\""));


				// ReadEntry / WriteEntry
				bResult = iniFile.WriteEntry(EA_WCHAR("Section Spore 1"), EA_WCHAR("Key Spore 1"), EA_WCHAR("\"Value Spore 1\"")); 
				EATEST_VERIFY(bResult);

				bResult = iniFile.WriteEntry(EA_WCHAR("Section Spore 2"), EA_WCHAR("Key Spore 2"), EA_WCHAR("\"Value Spore 2\"")); 
				EATEST_VERIFY(bResult);

				nResult = iniFile.ReadEntry(EA_WCHAR("Section Spore 2"), EA_WCHAR("Key Spore 2"), sEntry);
				EATEST_VERIFY(nResult > 0 && sEntry == EA_WCHAR("\"Value Spore 2\""));

				nResult = iniFile.ReadEntry(EA_WCHAR("Section Spore 1"), EA_WCHAR("Key Spore 1"), sEntry);
				EATEST_VERIFY(bResult && sEntry == EA_WCHAR("\"Value Spore 1\""));


				// Formatted
				int32_t  inumber;
				float    fnumber;
				wchar_t  c;
				int      hex;
				wchar_t  string_buf[10];

				bResult = iniFile.WriteEntryFormatted("Section Format", "FormatTest", "%i %5.2f%% %c %s %0*x", 777, 3.14, '0', "Hello", 4, 32);     
				EATEST_VERIFY(bResult);

				nResult = iniFile.ReadEntry("Section Format", "FormatTest", sEntry8);
				EATEST_VERIFY((nResult == (int)sEntry8.length()) && (sEntry8 == "777  3.14% 0 Hello 0020"));

				nResult = iniFile.ReadEntryFormatted("Section Format", "FormatTest",  "%i  %f%%  %c  %s %x", &inumber, &fnumber, &c, string_buf, &hex);
				EATEST_VERIFY(nResult == 5);

				bResult = iniFile.WriteEntryFormatted(EA_WCHAR("Section Format"), EA_WCHAR("FormatTest"), EA_WCHAR("%i %5.2f%% %lc %ls %0*x"), 777, 3.14, EA_WCHAR('0'), EA_WCHAR("Hello"), 4, 32);     
				EATEST_VERIFY(bResult);

				nResult = iniFile.ReadEntry(EA_WCHAR("Section Format"), EA_WCHAR("FormatTest"), sEntry);
				EATEST_VERIFY((nResult == (int)sEntry.length()) && (sEntry == EA_WCHAR("777  3.14% 0 Hello 0020")));

				nResult = EA::StdC::Sscanf(sEntry.c_str(), EA_WCHAR("%i  %f%%  %lc  %ls %x"), &inumber, &fnumber, &c, string_buf, &hex);
				EATEST_VERIFY(nResult == 5);

				nResult = iniFile.ReadEntryFormatted(EA_WCHAR("Section Format"), EA_WCHAR("FormatTest"),  EA_WCHAR("%i  %f%%  %lc  %ls %x"), &inumber, &fnumber, &c, string_buf, &hex);
				EATEST_VERIFY(nResult == 5);

				// Binary
				uint8_t bin_buffer[] = "\x1\x2\x3\x4\x5\x6\x7\x8\x9\xa\xb";
				bResult = iniFile.WriteBinary(EA_WCHAR("Section Binary"), EA_WCHAR("Binary"), bin_buffer, sizeof(bin_buffer));     
				EATEST_VERIFY(bResult);

				nResult = iniFile.ReadBinary(EA_WCHAR("Section Binary"), EA_WCHAR("Binary"), bin_buffer, sizeof(bin_buffer));
				EATEST_VERIFY(nResult == 24);


				// Enumeration
				ec.Clear();
				ec.mbKeyValueEnumeration = false;
				nResult = iniFile.EnumSections(IniFileCallback, &ec);
				EATEST_VERIFY(nResult == 8);
				EATEST_VERIFY(ec.mData.size() == 8);


				iniFile.Close();
			}
		}

		return nErrorCount;
	#else
		return 0;
	#endif
}


