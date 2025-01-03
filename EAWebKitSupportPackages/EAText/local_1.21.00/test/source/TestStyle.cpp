/////////////////////////////////////////////////////////////////////////////
// TestStyle.cpp
//
// Copyright (c) 2004, Electronic Arts Inc. All rights reserved.
// Created by Paul Pedriana
//
/////////////////////////////////////////////////////////////////////////////


#include "EATextTest.h"
#include <EAText/EATextStyle.h>
#include <EATest/EATest.h>


using namespace EA::Text;


const uint32_t kStyleTest1 = 1;
const uint32_t kStyleTest2 = 2;


int TestStyle()
{
	int nErrorCount = 0;

	{ // GetStyleId
		uint32_t id;

		id = GetStyleId(NULL);
		EATEST_VERIFY(id == 0);

		id = GetStyleId("");
		EATEST_VERIFY(id == 0);

		id = GetStyleId("Hello World");
		EATEST_VERIFY(id != 0); // We happen to know that this string doesn't have a zero id.

		id = GetStyleId("\1"); // Test a non-ascii value.
		EATEST_VERIFY(id != 0); // We happen to know that this string doesn't have a zero id.
	}


	{ // ParseStyleText
		const char* pMainStyle = {
			"   // Comment \n"
			"   // MainStyle(0x00000000){ \n"
			"\n"
			"   MainStyle(0x134ad5f){                      \n"
			"      font-family : \"Courier\",     , ,\"\",, arial   \n"
			"      font-size: 100.f   \n"
			"      font-size: 101.0px   \n"
			"      font-size: 102.0pt   \n"
			"      font-style: italic   \n"
			"      font-weight: 100\n"
			"      font-variant: trash   \n"
			"      font-variant: small-caps   \n"
			"      font-variant: normal   \n"
			"      font-variant: trash   \n"
			"      font-pitch: variable\n"
			"      text-decoration    :overline, underline, line-through, underline, underline, trash   \n"
			"      font-smooth: never   \n"
			"      background-color : #11223344   \n"
			"      background-color : 11223344   \n"
			"      line-spacing :  1   \n"
			"      overflow xxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxx :  xxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxx \n"
			"\n"
			"      letter-spacing: +3.7   \n"
			"      word-spacing: -.25   \n"
			"      font-stretch : 5.5   \n"
			"      font-emphasize-style: none  \n"
			"      font-emphasize-position :before   \n"
			"      \n"
			"   }  \n"
			"\n"
			"// Comment \n"
		};

		const char* pDialogStyle = {
			"   DialogStyle ( 0xacbf4564) : MainFont{       \n"
			"      font-size: 14px; font-style : italic;    \n"
			"      font-size: 10px; font-stretch : 5.5      \n"
			"   }                                           \n"
		};

		const char* pMultiStyle = {
			"   Style1(0x00000001){                         \n"
			"      font-family: \"Helvetica Neue\", arial;  \n"
			"      font-size: 10px;                         \n"
			"   }                                           \n"
			"   Style2{                                     \n"
			"      font-family: \"Helvetica Neue\", arial;  \n"
			"      font-size: 10px;                         \n"
			"   }                                           \n"
			"                                               \n"
			"     // Comment \n"
			"     \n"
			"   Style3(0x00000003) : Style1{                \n"
			"      font-family: \"Helvetica Neue\", arial;  \n"
			"      font-size: 10px;                         \n"
			"   }                                           \n"
			"   Style4 : Style2{                            \n"
			"   }"
		};

		size_t       nStyleCount;
		char         pStyleName[24][32];
		uint32_t     pIdArray[24];
		TextStyle    ssCSSArray[24];
		StyleManager styleManager;
		
		memset(pStyleName, 0, sizeof(pStyleName));
		memset(pIdArray,   0, sizeof(pIdArray));
		nStyleCount = ParseStyleText(pMainStyle, pStyleName, pIdArray, ssCSSArray, 24, NULL); 
		EATEST_VERIFY(nStyleCount > 0);

		memset(pStyleName, 0, sizeof(pStyleName));
		memset(pIdArray,   0, sizeof(pIdArray));
		nStyleCount = ParseStyleText(pDialogStyle, pStyleName, pIdArray, ssCSSArray, 24, &styleManager); 
		EATEST_VERIFY(nStyleCount > 0);

		memset(pStyleName, 0, sizeof(pStyleName));
		memset(pIdArray,   0, sizeof(pIdArray));
		nStyleCount = ParseStyleText(pMultiStyle, pStyleName, pIdArray, ssCSSArray, 24, &styleManager); 
		EATEST_VERIFY(nStyleCount > 0);
	}


	{ // StyleManager
		StyleManager     styleManager;
		const TextStyle* pCSS;
		TextStyle        css;

		// Verify that getting a non-existant style pointer works as expected.
		pCSS = styleManager.GetStyle(kStyleTest1, NULL);
		EATEST_VERIFY(pCSS == NULL);

		// Verify that getting a non-existant style copy works as expected.
		pCSS = styleManager.GetStyle(kStyleTest1, &css);
		EATEST_VERIFY(pCSS == NULL);

		// Verify that removing a non-existant style doesn't cause a crash.
		styleManager.RemoveStyle(kStyleTest1);

		// Verify that getting an existing style pointer works as expected.
		css.mfSize = 1;
		styleManager.AddStyle(kStyleTest1, css);
		pCSS = styleManager.GetStyle(kStyleTest1);
		EATEST_VERIFY(pCSS && (pCSS->mfSize == 1));

		// Verify that getting an existing style pointer works as expected.
		css.mfSize = 2;
		styleManager.AddStyle(kStyleTest2, css);
		pCSS = styleManager.GetStyle(kStyleTest2);
		EATEST_VERIFY(pCSS && (pCSS->mfSize == 2));

		// Verify that getting an existing style copy works as expected.
		pCSS = styleManager.GetStyle(kStyleTest1, &css);
		EATEST_VERIFY(pCSS && (pCSS == &css) && (css.mfSize == 1));

		// Verify that getting a recently removed style works as expected.
		styleManager.RemoveStyle(kStyleTest1);
		pCSS = styleManager.GetStyle(kStyleTest1);
		EATEST_VERIFY(pCSS == NULL);

		// Verify that RemoveStyle(kStyleAll) works as expected.
		for(uint32_t i = 1000; i < 1100; i++)
			styleManager.AddStyle(i, css);
		styleManager.RemoveStyle(kStyleIdAll);
		for(uint32_t j = 1000; j < 1100; j++)
		{
			pCSS = styleManager.GetStyle(j);
			EATEST_VERIFY(pCSS == NULL);
		}
	}

	return nErrorCount;
}





