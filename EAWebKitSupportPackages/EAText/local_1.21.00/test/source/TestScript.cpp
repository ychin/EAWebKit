/////////////////////////////////////////////////////////////////////////////
// TestScript.cpp
//
// Copyright (c) 2004, Electronic Arts Inc. All rights reserved.
// Created by Paul Pedriana
//
/////////////////////////////////////////////////////////////////////////////


#include "EATextTest.h"
#include <EAText/EATextScript.h>
#include <EAStdC/EAString.h>
#include <EATest/EATest.h>


using namespace EA::Text;


int TestScript()
{
	int nErrorCount = 0;

	bool             bResult;
	ScriptProperties sp;
	Script           script;
	const Char*      pLocale;

	// GetScriptProperties
	bResult = GetScriptProperties(kScriptLatin, &sp, false);
	EATEST_VERIFY(bResult);

	// GetScriptFromLocale
	script = GetScriptFromLocale(EATEXT_CHAR("en"),    0);
	EATEST_VERIFY(script == kScriptLatin);
	script = GetScriptFromLocale(EATEXT_CHAR("en-us"), 0);
	EATEST_VERIFY(script == kScriptLatin);
	script = GetScriptFromLocale(EATEXT_CHAR("en-us"), 1);
	EATEST_VERIFY(script == kScriptUnknown);
	script = GetScriptFromLocale(EATEXT_CHAR("en-us"), 2);
	EATEST_VERIFY(script == kScriptUnknown);
	script = GetScriptFromLocale(EATEXT_CHAR("en-us"), 3);
	EATEST_VERIFY(script == kScriptUnknown);
	script = GetScriptFromLocale(EATEXT_CHAR("en-us"), 4);
	EATEST_VERIFY(script == kScriptUnknown);

	// LocaleUsesScript
	bResult = LocaleUsesScript(EATEXT_CHAR("en-us"), kScriptArabic);
	EATEST_VERIFY(!bResult);

	bResult = LocaleUsesScript(EATEXT_CHAR("en-us"), kScriptLatin);
	EATEST_VERIFY(bResult);

	bResult = LocaleUsesScript(EATEXT_CHAR("ar"), kScriptLatin);
	EATEST_VERIFY(!bResult);

	bResult = LocaleUsesScript(EATEXT_CHAR("ar"), kScriptArabic);
	EATEST_VERIFY(bResult);

	// GetScriptFromChar
	script = GetScriptFromChar(EA_CHAR8('a'));
	EATEST_VERIFY(script == kScriptLatin);

	script = GetScriptFromChar(0x0e1c);
	EATEST_VERIFY(script == kScriptThai);

	// GetLocaleFromScript
	pLocale = GetLocaleFromScript(kScriptHangul, false);
	EATEST_VERIFY(EA::StdC::Strcmp(pLocale, EATEXT_CHAR("ko")) == 0);

	return nErrorCount;
}




