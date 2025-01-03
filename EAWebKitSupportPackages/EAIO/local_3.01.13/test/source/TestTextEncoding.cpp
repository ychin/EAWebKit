///////////////////////////////////////////////////////////////////////////////
// TestTextEncoding.cpp
//
// Copyright (c) 2003 Electronic Arts. All rights reserved.
//
// Created by Paul Pedriana
///////////////////////////////////////////////////////////////////////////////


#include <EABase/eabase.h>
#include <EAIO/FnEncode.h>
#include <EAIOTest/EAIO_Test.h>
#include <EATest/EATest.h>

#ifdef _MSC_VER
	#pragma warning(push, 0)
#endif

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#ifdef EA_PLATFORM_WINDOWS
	#include <Windows.h>
#endif

#ifdef _MSC_VER
	#pragma warning(pop)
#endif



///////////////////////////////////////////////////////////////////////////////
// TestTextEncoding
//
int TestTextEncoding()
{
	using namespace EA::IO;

	EA::UnitTest::Report("TestTextEncoding\n");

	int nErrorCount(0);

	/*

	// StrlcpyXXXToXXX
	{
		char  buffer8[256];
		char16_t buffer16[256];
		size_t   n;

		// size_t StrlcpyUTF16ToUTF8(char* pText8, size_t nLength8, const char16_t* pText16, size_t nLength16 = kLengthNull);
		n = StrlcpyUTF16ToUTF8(buffer8, 256, EA_CHAR16("abc"), kLengthNull);
		EATEST_VERIFY(strlen(buffer8) == 3);
		EATEST_VERIFY(n == 3);

		n = StrlcpyUTF16ToUTF8(buffer8, 256, EA_CHAR16(""), kLengthNull);
		EATEST_VERIFY(strlen(buffer8) == 0);
		EATEST_VERIFY(n == 0);

		n = StrlcpyUTF16ToUTF8(buffer8, 0, EA_CHAR16("abc"), kLengthNull);
		EATEST_VERIFY(strlen(buffer8) == 0);
		EATEST_VERIFY(n == 3);

		n = StrlcpyUTF16ToUTF8(buffer8, 1, EA_CHAR16("abc"), kLengthNull);
		EATEST_VERIFY(strlen(buffer8) == 0);
		EATEST_VERIFY(n == 3);

		buffer8[2] = '?';
		n = StrlcpyUTF16ToUTF8(buffer8, 2, EA_CHAR16("abc"), kLengthNull);
		EATEST_VERIFY(buffer8[0] == 'a');
		EATEST_VERIFY(buffer8[1] == 0);
		EATEST_VERIFY(buffer8[2] == '?');
		EATEST_VERIFY(n == 3);

		buffer8[0] = '?';
		n = StrlcpyUTF16ToUTF8(buffer8, 0, EA_CHAR16("abc"), kLengthNull);
		EATEST_VERIFY(buffer8[0] == '?');
		EATEST_VERIFY(n == 3);

		//size_t StrlcpyUTF8ToUTF16(char16_t* pText16, size_t nLength16, const char* pText8, size_t nLength8 = kLengthNull);
		n = StrlcpyUTF8ToUTF16(buffer16, 256, "abc", kLengthNull);
		EATEST_VERIFY(EAIOStrlen16(buffer16) == 3);
		EATEST_VERIFY(n == 3);

		n = StrlcpyUTF8ToUTF16(buffer16, 256, "", kLengthNull);
		EATEST_VERIFY(EAIOStrlen16(buffer16) == 0);
		EATEST_VERIFY(n == 0);

		n = StrlcpyUTF8ToUTF16(buffer16, 0, "abc", kLengthNull);
		EATEST_VERIFY(EAIOStrlen16(buffer16) == 0);
		EATEST_VERIFY(n == 3);

		n = StrlcpyUTF8ToUTF16(buffer16, 1, "abc", kLengthNull);
		EATEST_VERIFY(EAIOStrlen16(buffer16) == 0);
		EATEST_VERIFY(n == 3);

		buffer16[2] = '?';
		n = StrlcpyUTF8ToUTF16(buffer16, 2, "abc", kLengthNull);
		EATEST_VERIFY(buffer16[0] == 'a');
		EATEST_VERIFY(buffer16[1] == 0);
		EATEST_VERIFY(buffer16[2] == '?');
		EATEST_VERIFY(n == 3);

		buffer16[0] = '?';
		n = StrlcpyUTF8ToUTF16(buffer16, 0, "abc", kLengthNull);
		EATEST_VERIFY(buffer16[0] == '?');
		EATEST_VERIFY(n == 3);

		const char* pUTF8 = "\x43\x3A\x5C\x45\x6C\x65\x63\x74\x72\x6F\x6E\x69\x63\x20\x41\x72\x74\x73\x5C\xE3\x82\xB6\xEF\xBD\xA5\xE3\x82\xB7\xE3\x83\xA0\xE3\x82\xBA\xEF\xBC\x93";
		n = StrlcpyUTF8ToUTF16(buffer16, 256, pUTF8);
		EATEST_VERIFY(n == 25);

		n = StrlcpyUTF8ToUTF16(buffer16, 256, pUTF8, strlen(pUTF8));
		EATEST_VERIFY(n == 25);
	}
	*/

	return nErrorCount;
}

