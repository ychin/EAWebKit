/*
 *  Copyright (C) 2006 George Staikos <staikos@kde.org>
 *  Copyright (C) 2006 Alexey Proskuryakov <ap@nypop.com>
 *  Copyright (C) 2006, 2007, 2008, 2009 Apple Inc. All rights reserved.
 *  Copyright (C) 2011, 2012, 2013, 2014 Electronic Arts, Inc. All rights reserved.
 *
 *  This library is free software; you can redistribute it and/or
 *  modify it under the terms of the GNU Library General Public
 *  License as published by the Free Software Foundation; either
 *  version 2 of the License, or (at your option) any later version.
 *
 *  This library is distributed in the hope that it will be useful,
 *  but WITHOUT ANY WARRANTY; without even the implied warranty of
 *  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
 *  Library General Public License for more details.
 *
 *  You should have received a copy of the GNU Library General Public License
 *  along with this library; see the file COPYING.LIB.  If not, write to
 *  the Free Software Foundation, Inc., 51 Franklin Street, Fifth Floor,
 *  Boston, MA 02110-1301, USA.
 *
 */

#include "config.h"
//EAWebKitTOD: Trim down the number of ICU headers we need to include
#include "../icu/UnicodeICU.h"
#include <unicode/unorm.h>

#include "../../../../../../WebKit/ea/Api/EAWebKit/include/EAWebKit/EAWebKitTextInterface.h"

#include <WTF/assertions.h>
#include <EABase/eabase.h>
#include EAWEBKIT_PLATFORM_HEADER

namespace EA
{
namespace WebKit
{
	ITextSystem* GetTextSystem();
}
}

COMPILE_ASSERT(sizeof(UChar)	== sizeof(uint16_t), UCharShouldBeTwoBytes);
COMPILE_ASSERT(sizeof(wchar_t)	== 2, wchar_tShouldBeTwoBytes); //We are relying on it in many places.


// Note by Arpit Baldeva:
// We try to implement as much functionality as possible. However, it is non trivial to provide a perfect implementation due to memory cost it ensues.

UBlockCode ublock_getCode(UChar32 character) 
{ 
	// Turns out this code is only being used to detect ARABIC block in WebCore so following implementation is okay.
	// EAWebKitTODO: It'd be great to implement it in terms of EAText and support other blocks but that'd mean we'd need to write a huge
	// case statement or modify the EAText to return unicode enums.
	return (character & ~0xFF) == 0x600 ? UBLOCK_ARABIC : UBLOCK_NO_BLOCK; 
}

int unorm_normalize(const UChar*, int32_t, UNormalizationMode, int32_t, UChar*, int32_t, UErrorCode*)
{
	//EAWebKitTODO: We have not implemented this previously. EAText supports it now (seems like partially). Use it.
	ASSERT_NOT_REACHED();
	return 0;
}

UChar32 u_tolower(UChar32 c)
{
	ASSERT_WITH_MESSAGE(c <= 0xffff || c==0xffffffff, "We don't support chars above 0xffff");
	
	const EA::WebKit::Char source = static_cast<EA::WebKit::Char>(c);
	EA::WebKit::Char result = c; 
	EA::WebKit::GetTextSystem()->ConvertCase(&source, 1, &result, 1, EA::WebKit::kCaseTypeLower,"");
	return static_cast<UChar32>(result); 
}

int u_strToLower(UChar* dest, int destCapacity, const UChar* source, int sourceLength, const char* locale, UErrorCode* status)
{
	//Depending upon locale, the length of a string can change when converting case.
	int destLength = EA::WebKit::GetTextSystem()->ConvertCase((EA::WebKit::Char*)(source), sourceLength, (EA::WebKit::Char*)(dest), destCapacity, EA::WebKit::kCaseTypeLower,locale);
	if(destLength > destCapacity)
		*status = (UErrorCode)(U_ZERO_ERROR+1); // Anything greater than U_ZERO_ERROR is an error but there is no generic error in ICU. So we hack it by simply adding 1.
	else
		*status = U_ZERO_ERROR;
	
	return destLength;
}

UChar32 u_foldCase(UChar32 character, unsigned options)
{ 
	ASSERT_UNUSED(options, options == U_FOLD_CASE_DEFAULT); 
	return u_tolower(character); 
}

int u_strFoldCase(UChar* result, int resultLength, const UChar* source, int sourceLength, unsigned options, UErrorCode* status)
{
	ASSERT_UNUSED(options, options == U_FOLD_CASE_DEFAULT);
	// For most unicode characters, foldCase is same as toLower.
	return u_strToLower(result, resultLength, source, sourceLength, 0, status);
}

UChar32 u_toupper(UChar32 c)
{
	ASSERT_WITH_MESSAGE(c <= 0xffff || c==0xffffffff, "We don't support chars above 0xffff");

	const EA::WebKit::Char source = static_cast<EA::WebKit::Char>(c);
	EA::WebKit::Char result = c; 
	EA::WebKit::GetTextSystem()->ConvertCase(&source, 1, &result, 1, EA::WebKit::kCaseTypeUpper,"");
	return static_cast<UChar32>(result); 
}

UChar32 u_totitle(UChar32 c)
{
	const EA::WebKit::Char source = static_cast<EA::WebKit::Char>(c);
	EA::WebKit::Char result = c; 
	EA::WebKit::GetTextSystem()->ConvertCase(&source, 1, &result, 1, EA::WebKit::kCaseTypeTitle,"");
	return static_cast<UChar32>(result); 
}

int u_strToUpper(UChar* dest, int destCapacity, const UChar* source, int sourceLength, const char* locale, UErrorCode* status)
{
	//Depending upon locale, the length of a string can change when converting case.
	int destLength = EA::WebKit::GetTextSystem()->ConvertCase((EA::WebKit::Char*)(source), sourceLength, (EA::WebKit::Char*)(dest), destCapacity, EA::WebKit::kCaseTypeUpper,locale);
	if(destLength > destCapacity)
		*status = (UErrorCode)(U_ZERO_ERROR+1); // Anything greater than U_ZERO_ERROR is an error but there is no generic error in ICU. So we hack it by simply adding 1.
	else
		*status = U_ZERO_ERROR;
	
	return destLength;
}

int8_t u_charType(UChar32 c)
{
	ASSERT_WITH_MESSAGE(c <= 0xffff || c==0xffffffff, "We don't support chars above 0xffff");
	const EA::WebKit::CharCategory cc = EA::WebKit::GetTextSystem()->GetCharCategory((EA::WebKit::Char)c);

	switch(cc)		
	{
	case EA::WebKit::kCCGeneralOtherTypes:	    return U_GENERAL_OTHER_TYPES; 
	case EA::WebKit::kCCUppercaseLetter:	    return U_UPPERCASE_LETTER; 
	case EA::WebKit::kCCLowercaseLetter:	    return U_LOWERCASE_LETTER; 
	case EA::WebKit::kCCTitlecaseLetter:	    return U_TITLECASE_LETTER; 
	case EA::WebKit::kCCModifierLetter:		    return U_MODIFIER_LETTER; 
	case EA::WebKit::kCCOtherLetter:		    return U_OTHER_LETTER; 


	case EA::WebKit::kCCNonSpacingMark:		    return U_NON_SPACING_MARK; 
	case EA::WebKit::kCCEnclosingMark:		    return U_ENCLOSING_MARK; 
	case EA::WebKit::kCCCombiningSpacingMark:	return U_COMBINING_SPACING_MARK; 


	case EA::WebKit::kCCDecimalDigitNumber:	    return U_DECIMAL_DIGIT_NUMBER; 
	case EA::WebKit::kCCLetterNumber:			return U_LETTER_NUMBER; 
	case EA::WebKit::kCCOtherNumber:			return U_OTHER_NUMBER; 


	case EA::WebKit::kCCSpaceSeparator:		    return U_SPACE_SEPARATOR; 
	case EA::WebKit::kCCLineSeparator:		    return U_LINE_SEPARATOR; 
	case EA::WebKit::kCCParagraphSeparator:	    return U_PARAGRAPH_SEPARATOR; 


	case EA::WebKit::kCCControlChar:			return U_CONTROL_CHAR; 
	case EA::WebKit::kCCFormatChar:			    return U_FORMAT_CHAR; 
	case EA::WebKit::kCCPrivateUseChar:		    return U_PRIVATE_USE_CHAR; 
	case EA::WebKit::kCCSurrogate:			    return U_SURROGATE; 


	case EA::WebKit::kCCDashPunctuation:		return U_DASH_PUNCTUATION; 
	case EA::WebKit::kCCStartPunctuation:		return U_START_PUNCTUATION; 
	case EA::WebKit::kCCEndPunctuation:		    return U_END_PUNCTUATION; 
	case EA::WebKit::kCCConnectorPunctuation:   return U_CONNECTOR_PUNCTUATION; 
	case EA::WebKit::kCCOtherPunctuation:		return U_OTHER_PUNCTUATION; 


	case EA::WebKit::kCCMathSymbol:				return U_MATH_SYMBOL; 
	case EA::WebKit::kCCCurrencySymbol:		    return U_CURRENCY_SYMBOL; 
	case EA::WebKit::kCCModifierSymbol:		    return U_MODIFIER_SYMBOL; 
	case EA::WebKit::kCCOtherSymbol:			return U_OTHER_SYMBOL; 

	case EA::WebKit::kCCInitialPunctuation:	    return U_INITIAL_PUNCTUATION; 
	case EA::WebKit::kCCFinalPunctuation:		return U_FINAL_PUNCTUATION; 
	}
	
	ASSERT_WITH_MESSAGE(false, "some kind of error.");
	return U_CHAR_CATEGORY_COUNT;
}

UBool u_isprint(UChar32 c)
{
	ASSERT_WITH_MESSAGE(c <= 0xffff || c==0xffffffff, "We don't support chars above 0xffff");
	
	EA::WebKit::CharCategory cc = EA::WebKit::GetTextSystem()->GetCharCategory((EA::WebKit::Char)c);
	bool nonPrintable =  ( cc == EA::WebKit::kCCUnassigned
						|| cc == EA::WebKit::kCCControlChar
						|| cc == EA::WebKit::kCCFormatChar
						|| cc == EA::WebKit::kCCPrivateUseChar
						|| cc == EA::WebKit::kCCSurrogate
						|| cc == EA::WebKit::kCCGeneralOtherTypes);

	return !nonPrintable;
}

UBool u_ispunct(UChar32 c)
{
	ASSERT_WITH_MESSAGE(c <= 0xffff || c==0xffffffff, "We don't support chars above 0xffff");
	const EA::WebKit::CharCategory cc = (EA::WebKit::CharCategory) EA::WebKit::GetTextSystem()->GetCharCategory((EA::WebKit::Char)c);
	return (cc == EA::WebKit::kCCDashPunctuation
		|| cc == EA::WebKit::kCCStartPunctuation
		|| cc == EA::WebKit::kCCEndPunctuation
		|| cc == EA::WebKit::kCCConnectorPunctuation
		|| cc == EA::WebKit::kCCOtherPunctuation
		|| cc == EA::WebKit::kCCInitialPunctuation
		|| cc == EA::WebKit::kCCFinalPunctuation);
}

UChar32 u_charMirror(UChar32 c)
{
	ASSERT_WITH_MESSAGE(c <= 0xffff || c==0xffffffff, "We don't support chars above 0xffff");
	return EA::WebKit::GetTextSystem()->GetMirrorChar((EA::WebKit::Char)c);
}

UCharDirection u_charDirection(UChar32 c)
{
	EA::WebKit::BidiClass bidiClass = EA::WebKit::GetTextSystem()->GetBidiClass((EA::WebKit::Char)c);

	// We have to use a switch statement because of the ordering issue.
	switch (bidiClass)
	{
	case EA::WebKit::kBidiClassON:  return U_OTHER_NEUTRAL;
	case EA::WebKit::kBidiClassL:   return U_LEFT_TO_RIGHT;
	case EA::WebKit::kBidiClassR:   return U_RIGHT_TO_LEFT;
	case EA::WebKit::kBidiClassAN:  return U_ARABIC_NUMBER;
	case EA::WebKit::kBidiClassEN:  return U_EUROPEAN_NUMBER;
	case EA::WebKit::kBidiClassAL:  return U_RIGHT_TO_LEFT_ARABIC;
	case EA::WebKit::kBidiClassNSM: return U_DIR_NON_SPACING_MARK;
	case EA::WebKit::kBidiClassCS:  return U_COMMON_NUMBER_SEPARATOR;
	case EA::WebKit::kBidiClassES:  return U_EUROPEAN_NUMBER_SEPARATOR;
	case EA::WebKit::kBidiClassET:  return U_EUROPEAN_NUMBER_TERMINATOR;
	case EA::WebKit::kBidiClassBN:  return U_BOUNDARY_NEUTRAL;
	case EA::WebKit::kBidiClassS:   return U_SEGMENT_SEPARATOR;
	case EA::WebKit::kBidiClassWS:  return U_WHITE_SPACE_NEUTRAL;
	case EA::WebKit::kBidiClassB:   return U_BLOCK_SEPARATOR;
	case EA::WebKit::kBidiClassRLO: return U_RIGHT_TO_LEFT_OVERRIDE;
	case EA::WebKit::kBidiClassRLE: return U_RIGHT_TO_LEFT_EMBEDDING;
	case EA::WebKit::kBidiClassLRO: return U_LEFT_TO_RIGHT_OVERRIDE;
	case EA::WebKit::kBidiClassLRE: return U_LEFT_TO_RIGHT_EMBEDDING;
	case EA::WebKit::kBidiClassPDF: return U_POP_DIRECTIONAL_FORMAT;
	}
	return U_LEFT_TO_RIGHT;
}

uint8_t u_getCombiningClass(UChar32 c)
{
	const int32_t cc = EA::WebKit::GetTextSystem()->GetCombiningClass((EA::WebKit::Char)c);
	return (uint8_t)cc;
}

int u_getIntPropertyValue(UChar32 c, UProperty property)
{
	switch (property) {
	case UCHAR_DECOMPOSITION_TYPE:
		{
			//EAWebKitTODO: Put static_assert for enum values
			EA::WebKit::DecompositionType dType = EA::WebKit::GetTextSystem()->DecompositionType(c);
			return dType;
		}
	case UCHAR_LINE_BREAK:
		//EAWebKitTODO: We need GetCharProperty Support in EAText for this to work
		return U_LB_UNKNOWN;
	default:
		break;
	}

	ASSERT_NOT_REACHED();
	return 0;
}



int u_memcasecmp(const UChar* a, const UChar* b, int length, unsigned options)
{
	for (int i = 0; i < length; ++i) {
		UChar c1 = u_foldCase(a[i], options);
		UChar c2 = u_foldCase(b[i], options);
		if (c1 != c2)
			return c1 - c2;
	}

	return 0;
}

