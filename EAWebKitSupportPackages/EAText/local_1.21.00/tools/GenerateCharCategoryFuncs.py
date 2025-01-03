
# Character categories
kCharCategory_Lu        = 1     # Letter, Uppercase
kCharCategory_Ll        = 2     # Letter, Lowercase
kCharCategory_Lt        = 3     # Letter, Titlecase
kCharCategory_Lm        = 4     # Letter, Modifier
kCharCategory_Lo        = 5     # Letter, Other
kCharCategory_Mn        = 6     # Mark, Non-Spacing
kCharCategory_Mc        = 7     # Mark, Spacing Combining
kCharCategory_Me        = 8     # Mark, Enclosing
kCharCategory_Nd        = 9     # Number, Decimal
kCharCategory_Nl        = 10    # Number, Letter
kCharCategory_No        = 11    # Number, Other
kCharCategory_Pc        = 12    # Punctuation, Connector
kCharCategory_Pd        = 13    # Punctuation, Dash
kCharCategory_Ps        = 14    # Punctuation, Open
kCharCategory_Pe        = 15    # Punctuation, Close
kCharCategory_Pi        = 16    # Punctuation, Initial quote
kCharCategory_Pf        = 17    # Punctuation, Final quote
kCharCategory_Po        = 18    # Punctuation, Other
kCharCategory_Sm        = 19    # Symbol, Math
kCharCategory_Sc        = 20    # Symbol, Currency
kCharCategory_Sk        = 21    # Symbol, Modifier
kCharCategory_So        = 22    # Symbol, Other
kCharCategory_Zs        = 23    # Separator, Space
kCharCategory_Zl        = 24    # Separator, Line
kCharCategory_Zp        = 25    # Separator, Paragraph
kCharCategory_Cc        = 26    # Other, Control
kCharCategory_Cf        = 27    # Other, Format
kCharCategory_Cs        = 28    # Other, Surrogate
kCharCategory_Co        = 29    # Other, Private Use
kCharCategory_Cn        = 30    # Other, Not Assigned

categoryDict = { 
	'Lu' : kCharCategory_Lu, 'Ll' : kCharCategory_Ll, 'Lt' : kCharCategory_Lt,
	'Lm' : kCharCategory_Lm, 'Lo' : kCharCategory_Lo, 'Mn' : kCharCategory_Mn,
	'Mc' : kCharCategory_Mc, 'Me' : kCharCategory_Me, 'Nd' : kCharCategory_Nd,
	'Nl' : kCharCategory_Nl, 'No' : kCharCategory_No, 'Pc' : kCharCategory_Pc,
	'Pd' : kCharCategory_Pd, 'Ps' : kCharCategory_Ps, 'Pe' : kCharCategory_Pe,
	'Pi' : kCharCategory_Pi, 'Pf' : kCharCategory_Pf, 'Po' : kCharCategory_Po,
	'Sm' : kCharCategory_Sm, 'Sc' : kCharCategory_Sc, 'Sk' : kCharCategory_Sk,
	'So' : kCharCategory_So, 'Zs' : kCharCategory_Zs, 'Zl' : kCharCategory_Zl,
	'Zp' : kCharCategory_Zp, 'Cc' : kCharCategory_Cc, 'Cf' : kCharCategory_Cf,
	'Cs' : kCharCategory_Cs, 'Co' : kCharCategory_Co, 'Cn' : kCharCategory_Cn }
		
categoryNameDict = {
	0 : "kCCUnassigned",
	kCharCategory_Lu : "kCCUppercaseLetter",
	kCharCategory_Ll : "kCCLowercaseLetter",
	kCharCategory_Lt : "kCCTitlecaseLetter",
	kCharCategory_Lm : "kCCModifierLetter",
	kCharCategory_Lo : "kCCOtherLetter",
	kCharCategory_Mn : "kCCNonSpacingMark",
	kCharCategory_Mc : "kCCCombiningSpacingMark",
	kCharCategory_Me : "kCCEnclosingMark",
	kCharCategory_Nd : "kCCDecimalDigitNumber",
	kCharCategory_Nl : "kCCLetterNumber",
	kCharCategory_No : "kCCOtherNumber",
	kCharCategory_Pc : "kCCConnectorPunctuation",
	kCharCategory_Pd : "kCCDashPunctuation",
	kCharCategory_Ps : "kCCStartPunctuation",
	kCharCategory_Pe : "kCCEndPunctuation",
	kCharCategory_Pi : "kCCInitialPunctuation",
	kCharCategory_Pf : "kCCFinalPunctuation",
	kCharCategory_Po : "kCCOtherPunctuation",
	kCharCategory_Sm : "kCCMathSuymbol",
	kCharCategory_Sc : "kCCCurrencySymbol",
	kCharCategory_Sk : "kCCModifierSymbol",
	kCharCategory_So : "kCCOtherSymbol",
	kCharCategory_Zs : "kCCSpaceSeparator",
	kCharCategory_Zl : "kCCLineSeparator",
	kCharCategory_Zp : "kCCParagraphSeparator",
	kCharCategory_Cc : "kCCControlChar",
	kCharCategory_Cf : "kCCFormatChar",
	kCharCategory_Cs : "kCCSurrogate",
	kCharCategory_Co : "kCCPrivateUseChar",
	kCharCategory_Cn : "kCCGeneralOtherTypes",
}
	
def is_letter( category ):
	return category == kCharCategory_Lu \
		or category == kCharCategory_Ll \
		or category == kCharCategory_Lt \
		or category == kCharCategory_Lm \
		or category == kCharCategory_Lo

# parse the unicode data
def process():
	category_ranges = []
	prev_code = 0
	prev_category = None
	
	code_first = 0
	dbfile = open( "UnicodeData.txt", "rt" )
	for line in dbfile:
		fields = line.split( ';' )
	
		code = int( fields[ 0 ], 16 )
		name = fields[ 1 ]
		category = categoryDict[ fields[ 2 ] ]
		combiningClass = fields[ 3 ]
		bidiClass = fields[ 4 ]
		decompType = fields[ 5 ]
		numericType = fields[ 6 ]
		
		if code > 0xffff: break
			
		if name.endswith( ", First>" ):
			code_first = code;
			continue

		code_last = code
		if not name.endswith( ", Last>" ):
			code_first = code
			
#        if not is_letter( category ): category = 0
		
		#print code_first, code_last, category, prev_code, prev_category
		
		if code_first > prev_code and prev_category != 0:
			category_ranges.append( ( prev_code, 0 ) )
			prev_code = code_first
			prev_category = 0
			
		if category != prev_category:
			category_ranges.append( ( code_first, category ) )
		
		prev_category = category
		prev_code = code_last + 1
		
	dbfile.close()
	
	if 0x10000 > prev_code and prev_category != 0:
		category_ranges.append( ( prev_code, 0 ) )
	category_ranges.append( ( 0x10000, 0 ) )
	
	numRanges = len( category_ranges ) - 1
	i = 0
	while i < numRanges:
		span_start = i
		span_end = i + 1
		span = []
		mask = 0
		bit = 1
		cat0 = category_ranges[ i ][ 1 ];
		cat1 = category_ranges[ i + 1 ][ 1 ];
		for j in range( i, numRanges - 1 ):
			delta = category_ranges[ j + 1 ][ 0 ] - category_ranges[ j ][ 0 ];
			if category_ranges[ j ][ 1 ] != cat0 and category_ranges[ j ][ 1 ] != cat1:
				span.append( mask )
				break
			if delta > 128:
				span.append( mask )
				break
			for k in range( 0, delta ):
				if category_ranges[ j ][ 1 ] == cat1:
					mask |= bit
				bit <<= 1
				if bit > 0x80:
					span.append( mask )
					mask = 0
					bit = 1
			span_end = j + 1
		if span_end - span_start > 3:
			output_table.append( ( category_ranges[ i ][ 0 ], cat0, cat1, span ) )
			i = span_end
		else:
			output_table.append( ( category_ranges[ i ][ 0 ], cat0, cat0, None ) )
			i += 1
			
def lookup( code ):
	low = 0
	high = len( output_table ) - 1
	
	while (high > low):
		mid = (low + high + 1) / 2
		if code < output_table[ mid ][ 0 ]:
			high = mid - 1
		else:
			low = mid

	cd, cat0, cat1, span = output_table[ low ]
	if span == None:
		return cat0
	code -= cd
	index = code / 8
	bit = code & 7
	mask = 1 << bit
	if span[ index ] & mask:
		return cat1
	else:
		return cat0
			
# Run through the entire unicode file again, and make sure that
# our lookup strategy works
def check():
	code_first = 0
	dbfile = open( "UnicodeData.txt", "rt" )
	for line in dbfile:
		fields = line.split( ';' )
	
		code = int( fields[ 0 ], 16 )
		name = fields[ 1 ]
		category = categoryDict[ fields[ 2 ] ]
		combiningClass = fields[ 3 ]
		bidiClass = fields[ 4 ]
		decompType = fields[ 5 ]
		numericType = fields[ 6 ]
		
		if code > 0xffff: break
			
		if name.endswith( ", First>" ):
			code_first = code;
			continue

		code_last = code
		if not name.endswith( ", Last>" ):
			code_first = code
			
		for i in range( code_first, code_last + 1 ):
			lcat = lookup( i )
			if category != lcat:
				raise "Gah" + " " + str( code ) + " " + str( category ) + " " + str( lcat )
			
	dbfile.close()

def generate():
	outfile = open( "../source/Generated/UnicodeCharClass.cpp", "w" );
	
	outfile.write( """///////////////////////////////////////////////////////////////////////////////
// Name: UnicodeCharClass.cpp
// Purpose: Contains a compressed implementation of the unicode character db.
//
// Generated by GenerateCharacterCategoryFuncs.py.
// Do not edit this file!
//
// Copyright (c) 2005 Electronic Arts Inc.
///////////////////////////////////////////////////////////////////////////////

#include "EAText/EATextUnicode.h"

namespace EA {
namespace Text {

	namespace {
	
		/// Theory of operation: The tables below are generated from the Unicode
		/// character database (see http://http://www.unicode.org/Public/4.1.0/ucd/UnicodeData.txt).
		/// The following data structures represent a compressed version of that table.
		///
		/// The unicode character set is divided into a set of ranges, where each range
		/// is defined as a set of consecutive characters that are either all in the
		/// the same category, or fall into exactly two categories. In the former case,
		/// the the category ID is simply stored directly. In the cases of a range
		/// in which there are two categories (such as for example, the Latin alphabet
		/// characters, which alternates between upper and lower case), both categories
		/// are stored, and a bit-array is used to determine which characters are
		/// from the first category, and which are from the second.

		struct CharCategory {
			uint16_t        mCode;              /// Starting code of the range
			uint8_t         mCharClass0;        /// Character class for 0 bits
			uint8_t         mCharClass1;        /// Character class for 1 bits
			uint8_t         *mpBitMask;         /// Bitmask (can be NULL)
		};
	
""" )

	for entry in output_table:
		code, cat0, cat1, span = entry
		if span != None:
			outfile.write( "        uint8_t cc_bitarray_%04.4x[] = { " % code )
			count = 0
			for mask in span:
				if count > 8:
					outfile.write( "\n                                       " )
					count = 0
				outfile.write( "0x%02.2x," % mask )
				count += 1
				
			outfile.write( "};\n" )
			
	outfile.write( """
		CharCategory gCharCategoryTable[] = {
""" )
			
	for entry in output_table:
		code, cat0, cat1, span = entry
		span_name = "NULL"
		if span != None:
			span_name = "cc_bitarray_%04.4x" % code
		outfile.write( "            ( 0x%04.4x, %s, %s, %s },\n" % ( code, categoryNameDict[ cat0 ], categoryNameDict[ cat1 ], span_name ) )

	outfile.write( """        };

	}

	uint32_t GetUnicodeCharacterCategory( uint16_t nChar ) {
		int low = 0
		int high = sizeof gCharCategoryTable / gCharCategoryTable[ 0 ];
	
		while (high > low) {
			int mid = (low + high + 1) / 2
			if (nChar < gCharCategoryTable[ mid ].mCode) {
				high = mid - 1;
			} else {
				low = mid;
			}
		}
			
		CharCategory &cc = gCharCategoryTable[ low ];
		if (cc.mpBitMask == NULL) return cc.mCharClass0;
		int nBitIndex = nChar - cc.mCode;
		return (cc.mpBitMask[ nBitIndex >> 3 ] & (1 << (nBitIndex & 7)))
			? cc.mCharClass1
			: cc.mCharClass0 ;
	}

}
}
""" )

	outfile.close()
		
output_table = []
process()
check()
generate()
