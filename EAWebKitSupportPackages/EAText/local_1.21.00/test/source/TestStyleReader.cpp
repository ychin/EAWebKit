/////////////////////////////////////////////////////////////////////////////
// TestStyleReader.cpp
//
// Copyright (c) 2006, Electronic Arts Inc. All rights reserved.
// Created by Talin. Maintained by Paul Pedriana
//
/////////////////////////////////////////////////////////////////////////////


#include <EATest/EATest.h>
#include <EAText/EATextStyleReader.h>
#include <EAText/EATextEffects.h>
#include <EAText/EATextFontServer.h>
#include <EAStdC/EAString.h>
#include <EAStdC/EAHashString.h>
#include <EATest/EATest.h>



// This CommandCallback exists for the purpose of helping to parse effects. 
// We need to find a way to provide a default version of this in EAText.
// pContext is pointer to FontServer.
static void CommandCallback(int32_t argc, const EA::Text::Char** argv, void* pContext)
{
	using namespace EA::StdC;
	using namespace EA::Text;

	if((Stricmp(argv[0], EATEXT_CHAR("effect")) == 0) && (argc >= 3))
	{
		// argv[1] is an effect id, such as 0x12345678
		// argv[2] is an effect definition string, such as "SetGlyphColor 0xff061FB5\tSetGlyphSmooth 1..."

		uint32_t     effectID = StrtoU32(argv[1], NULL, 16);
		EffectsState es;
		int32_t      errorLine;
		char         buffer8[1024];
		char8_t*     p8;
		const Char*  p16;

		if((argv[1][0] == '0') && (argv[1][1] == 'x'))
			effectID = StrtoU32(argv[1], NULL, 16);
		else // Else using "effectName" or the effect ID is a string.
		{
			Strlcpy(buffer8, argv[1], sizeof(buffer8));             // The only reason we copy this to 8 bit is because currently we have only one FNV1 function and it 
			effectID = EA::StdC::FNV1(buffer8, Strlen(buffer8));    // works only for 8 bit text. But the EAStdC package has proper 8 and 16 bit FNV1 functions that obviate the need to do this copying.
		}

		// Convert the 16 bit buffer to 8 bit, and convert \t to \n while at it.
		for(p16 = argv[2], p8 = buffer8; *p16 && (p8 < (buffer8 + 1023)); ++p16, ++p8)
		{
			if(*p16 == '\t')
				*p8 = '\n';
			else
				*p8 = (char8_t)(uint8_t)*p16;
		}
		*p8 = 0;

		if(CreateInstructionList(buffer8, es, &errorLine))
		{
			FontServer* const pFontServer = (FontServer*)pContext;
			pFontServer->AddEffect(effectID, es.mInstructionList, es.mInstructionListSize);
		}
	}

	if((Stricmp(argv[0], EATEXT_CHAR("font")) == 0) && (argc >= 2))
	{
		// argv[1] is a font file name, such as arial.ttf.
		// In an application we would load the given font file here.
	}
}


int TestStyleReader()
{
	using namespace EA::Text;

	int nErrorCount = 0;

	
	/* Disabled until we can make the failures silent. They currently bring 
	   up dialog boxes and are inappropriate for automated testing.
	{ 
		// A style with lots of errors
		const char* pStyle =
			"   // Comment                                              \n"
			"   // MainStyle(0x00000000){                               \n"
			"                                                           \n"
			"   MainStyle(0x0134ad5f){                                  \n"
			"      font-family : \"Courier\",     , ,\"\",, arial   ;   \n"     // Error
			"      font-size: 100.f;                                    \n"     // Error
			"      font-size: 101.0px;                                  \n"     // OK
			"      font-size: 102.0pt;                                  \n"     // OK
			"      font-variant: trash;                                 \n"     // Error
			"   }                                                       \n"
			"                                                           \n"
			"// Comment                                                 \n";

		StyleManager    styleManager;
		TextStyleReader reader(&styleManager, pStyle);

		EATEST_VERIFY(reader.ReadStyles() );
		EATEST_VERIFY(reader.GetWarningCount() >= 3);
	}
	*/

	{
		const char* pStyle = 
			"   // Comment                          \n"
			"   // MainStyle(0x00000000){           \n"
			"                                       \n"
			"   MainStyle(0x0134ad5f){              \n"
			"      font-family : \"Courier\";       \n"
			"      font-size: 102.0px;              \n"
			"      font-style: italic;              \n"
			"      background-color : #11223344;    \n"
			"      color : #1234;                   \n"
			"   }                                   \n"
			"                                       \n"
			"// Comment                             \n";

		StyleManager    styleManager;
		TextStyleReader reader( &styleManager, pStyle );
		uint32_t        nStyleId;
		TextStyle       style;
		char            styleName[ 32 ];

		EATEST_VERIFY( reader.ReadStyle( styleName, 32, nStyleId, style ) );
		EATEST_VERIFY( reader.GetWarningCount() == 0 );
		EATEST_VERIFY( EA::StdC::Strcmp( &style.mFamilyNameArray[ 0 ][ 0 ], EATEXT_CHAR("Courier")) == 0 );
		EATEST_VERIFY( style.mfSize == 102.0f ); 
		EATEST_VERIFY( style.mStyle == kStyleItalic ); 
		EATEST_VERIFY( style.mColorBackground == 0x11223344 ); 
		EATEST_VERIFY( style.mColor == 0x11223344 ); 
	}


	{ 
		const char* pStyle =
			"   MainStyle (0x0134ad5f) {                    \n"
			"      font-family : arial;                     \n"
			"      font-size: 55px;                         \n"
			"      font-style: italic;                      \n"
			"      background-color : #223344;              \n"
			"      color : #234;                            \n"
			"   }                                           \n"
			"                                               \n"
			"   DialogStyle (0xacbf4564) : MainStyle {      \n"
			"   }                                           \n";

		StyleManager    styleManager;
		TextStyleReader reader(&styleManager, pStyle);

		EATEST_VERIFY(reader.ReadStyles());
		EATEST_VERIFY(reader.GetWarningCount() == 0);

		const TextStyle* pMainStyle = styleManager.GetStyle(0x0134ad5f);
		const TextStyle* pDialogStyle = styleManager.GetStyle(0xacbf4564);

		EATEST_VERIFY(pMainStyle != NULL);
		EATEST_VERIFY(pDialogStyle != NULL);
		EATEST_VERIFY(pMainStyle->mfSize == 55.0f);
		EATEST_VERIFY(pDialogStyle->mfSize == 55.0f);
	}


	{
		//////////////////////////////////////////////////////////////////////////
		// font-effect: none | effectID (<hex guid id> | <name string which is hashed to guid by FNV1>) | [[ outline | shadow | raised | depressed ] xval yval base_color background_color [highlight_color]]
		//////////////////////////////////////////////////////////////////////////

		const char* pStyle =
			" @font \"arial*.ttf\";                           \n"
			" @effect \"0x22222222\"  \"SetGlyphColor 0xff061FB5\tSetGlyphSmooth 1\tSetGlyphBrush 0 5 1 .8\tDrawGlyphSmearOutline\tMerge\tClear\tSetGlyphColor 0xff5ECDF6\tDrawGlyph\tMerge\tSetGlyphHSpace 2 1\";\n"
			" @effect \"SuperShadow\" \"SetGlyphColor 0xff061FB5\tSetGlyphSmooth 1\tSetGlyphBrush 0 5 1 .8\tDrawGlyphSmearOutline\tMerge\tClear\tSetGlyphColor 0xff5ECDF6\tDrawGlyph\tMerge\tSetGlyphHSpace 2 1\";\n"
			"                                                 \n"
			" EffectStyle1 (0x11111111) {                     \n"
			"    font-family : arial;                         \n"
			"    font-size: 16px;                             \n"
			"    font-effect: none;                           \n"
			" }                                               \n"
			"                                                 \n"
			" EffectStyle2 (0x22222222) : EffectStyle1 {      \n"
			"    font-effect: effectID 0x22222222;            \n"     // The effectID can be the same value as the effect style ID.
			" }                                               \n"
			"                                                 \n"
			" EffectStyle3 (0x33333333) : EffectStyle1 {      \n"
			"    font-effect: effectID \"Shadowed\";          \n"
			" }                                               \n"
			"                                                 \n"
			" EffectStyle4 (0x44444444) : EffectStyle1 {      \n"
			"    font-effect: shadow 1 1 #010203 #020304;     \n"
			" }                                               \n"
			"                                                 \n";

		StyleManager    styleManager;
		TextStyleReader reader(&styleManager, pStyle);
		FontServer*     pFontServer = new FontServer; // Allocate with new instead of on the stack, as it may be too big for the stack on some platforms.

		reader.SetCommandCallback(CommandCallback, pFontServer);

		EATEST_VERIFY(reader.ReadStyles());
		EATEST_VERIFY(reader.GetWarningCount() == 0);

		const TextStyle* pEffectStyle1 = styleManager.GetStyle(0x11111111);
		const TextStyle* pEffectStyle2 = styleManager.GetStyle(0x22222222);
		const TextStyle* pEffectStyle3 = styleManager.GetStyle(0x33333333);
		const TextStyle* pEffectStyle4 = styleManager.GetStyle(0x44444444);

		EATEST_VERIFY((pEffectStyle1 != NULL) && (pEffectStyle2 != NULL) && (pEffectStyle3 != NULL) && (pEffectStyle4 != NULL));

		EATEST_VERIFY(pEffectStyle1->mEffect == 0);
		EATEST_VERIFY(pEffectStyle2->mEffect == 0x22222222);
		EATEST_VERIFY(pEffectStyle3->mEffect == EA::StdC::FNV1("Shadowed", strlen("Shadowed")));
		EATEST_VERIFY(pEffectStyle4->mEffect == kEffectShadow);
		EATEST_VERIFY(pEffectStyle4->mfEffectX == 1);
		EATEST_VERIFY(pEffectStyle4->mfEffectY == 1);
		EATEST_VERIFY(pEffectStyle4->mEffectBaseColor == 0xff010203);
		EATEST_VERIFY(pEffectStyle4->mEffectColor == 0xff020304);

		#if (EASTDC_VERSION_N >= 11600) // EAStdC had a bug prior to v1.16.00 with regards to Atoi32.
			const EIWord* pInstructionList;
			uint32_t      instructionCount;
			EATEST_VERIFY(pFontServer->GetEffect(0x22222222, pInstructionList, instructionCount));
			EATEST_VERIFY(pFontServer->GetEffect(EA::StdC::FNV1("SuperShadow", strlen("SuperShadow")), pInstructionList, instructionCount));
		#endif

		delete pFontServer;
	}


	{
		// Test user-reported bug for the case of font effects styles being ignored or causing other styles to be ignored.

		const char* pStyle =
			"Tag1 (0x00000001) {                                    \n"
			"    font-effect:effectID 0x00010001;                   \n"     // This gets ignored.
			"    font-size:10px;                                    \n"     // This gets ignored.
			"    font-family:\"Arial\";                             \n"     // This gets ignored.
			"}                                                      \n"
			"                                                       \n"
			"Tag2 (0x00000002) {                                    \n"
			"    font-size:20px;                                    \n"
			"    font-effect:effectID 0x00010002;                   \n"     // This gets ignored.
			"    font-family:\"Arial\";                             \n"     // This gets ignored.
			"}                                                      \n"
			"                                                       \n";
			//"Tag3 (0x00000003) {                                    \n"
			//"    font-size:30px;                                    \n"
			//"    font-effect: effectName Blah;                      \n"     // This gets ignored.
			//"    font-family:\"Arial\";                             \n"     // This gets ignored.
			//"}                                                      \n";

		StyleManager    styleManager;
		TextStyleReader reader(&styleManager, pStyle);
		FontServer*     pFontServer = new FontServer; // Allocate with new instead of on the stack, as it may be too big for the stack on some platforms.

		reader.SetCommandCallback(CommandCallback, pFontServer);

		pFontServer->AddEffect(0x00010001, NULL, 0);
		pFontServer->AddEffect(0x00010002, NULL, 0);
	  //pFontServer->AddEffect(EA::StdC::FNV1("Blah", EA::StdC::Strlen("Blah")), NULL, 0);

		EATEST_VERIFY(reader.ReadStyles());
		EATEST_VERIFY(reader.GetWarningCount() == 0);

		const TextStyle* pEffectStyle1 = styleManager.GetStyle(0x00000001);
		const TextStyle* pEffectStyle2 = styleManager.GetStyle(0x00000002);
	  //const TextStyle* pEffectStyle3 = styleManager.GetStyle(0x00000003);

		// Verify that the effect style was read OK.
		EATEST_VERIFY(pEffectStyle1 && pEffectStyle2);
		EATEST_VERIFY(pEffectStyle1->mEffect == 0x00010001);
		EATEST_VERIFY(pEffectStyle2->mEffect == 0x00010002);

		// Verify that the registered styles are as expected.
		const TextStyle* pTag1Style = styleManager.GetStyle(0x00000001);
		EATEST_VERIFY(pTag1Style != NULL);
		//EATEST_VERIFY(pTag1Style->mfSize == 55.0f);

		const TextStyle* pTag2Style = styleManager.GetStyle(0x00000002);
		EATEST_VERIFY(pTag2Style != NULL);

		//const TextStyle* pTag3Style = styleManager.GetStyle(0x00000003);
		//EATEST_VERIFY(pTag3Style != NULL);
		//EATEST_VERIFY(pTag3Style->mfSize == 55.0f);

		delete pFontServer;
	}

	{  // Test fix for bug that a style specification that ended without whitespace would cause kWarningInvalidOrMissingPropertyValue.
		const char* pStyle = 
			"   TestStyle(0x10000000){          \n"
			"      font-family : \"Helvetica\"; \n"
			"   }"; // Note: nothing after the '}' char.

		StyleManager    styleManager;
		TextStyleReader reader(&styleManager, pStyle);
		uint32_t        nStyleId;
		TextStyle       style;
		char            styleName[16];

		EATEST_VERIFY(reader.ReadStyle(styleName, EAArrayCount(styleName), nStyleId, style));
		EATEST_VERIFY(reader.GetWarningCount() == 0);
		EATEST_VERIFY(EA::StdC::Strcmp(styleName, "TestStyle") == 0);
		EATEST_VERIFY(nStyleId == 0x10000000);
		EATEST_VERIFY(EA::StdC::Strcmp(style.mFamilyNameArray[0], EATEXT_CHAR("Helvetica")) == 0);
	}

	return nErrorCount;
}
















