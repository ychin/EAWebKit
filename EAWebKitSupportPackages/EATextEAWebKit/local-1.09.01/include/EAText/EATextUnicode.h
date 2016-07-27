/*
Copyright (C) 2004, 2009, 2010, 2012 Electronic Arts, Inc.  All rights reserved.

Redistribution and use in source and binary forms, with or without
modification, are permitted provided that the following conditions
are met:

1.  Redistributions of source code must retain the above copyright
    notice, this list of conditions and the following disclaimer.
2.  Redistributions in binary form must reproduce the above copyright
    notice, this list of conditions and the following disclaimer in the
    documentation and/or other materials provided with the distribution.
3.  Neither the name of Electronic Arts, Inc. ("EA") nor the names of
    its contributors may be used to endorse or promote products derived
    from this software without specific prior written permission.

THIS SOFTWARE IS PROVIDED BY ELECTRONIC ARTS AND ITS CONTRIBUTORS "AS IS" AND ANY
EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE IMPLIED
WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE ARE
DISCLAIMED. IN NO EVENT SHALL ELECTRONIC ARTS OR ITS CONTRIBUTORS BE LIABLE FOR ANY
DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES
(INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES;
LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND
ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT
(INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE OF
THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
*/

///////////////////////////////////////////////////////////////////////////////
// EATextUnicode.h
//
// By Paul Pedriana - 2004
//
///////////////////////////////////////////////////////////////////////////////


#ifndef EATEXT_EATEXTUNICODE_H
#define EATEXT_EATEXTUNICODE_H


#include <EAText/EAText.h>
#include <stddef.h>


namespace EA
{
    namespace Text
    {
        // Special character definitions.
        //
        // Name                   Hex            Decimal    Name
        const Char kCharNUL     = 0x0000;    /// 0          Null.
        const Char kCharBS      = 0x0008;    /// 8          Backspace
        const Char kCharHT      = 0x0009;    /// 9          Horizontal Tab
        const Char kCharLF      = 0x000A;    /// 10         Linefeed
        const Char kCharVT      = 0x000B;    /// 11         Vertical Tab
        const Char kCharFF      = 0x000C;    /// 12         Form Feed
        const Char kCharCR      = 0x000D;    /// 13         Carriage Return
        const Char kCharESC     = 0x001B;    /// 27         Escape
        const Char kCharSpace   = 0x0020;    /// 32         Space
        const Char kCharDEL     = 0x007F;    /// 127        Delete (Rubout)
        const Char kCharNEL     = 0x0085;    /// 133        Next Line (intended to replace /r/n, etc.)
        const Char kCharNBSP    = 0x00A0;    /// 160        No-Break Space
        const Char kCharCGJ     = 0x034F;    /// 847        Combining Grapheme Joiner.
        const Char kCharNQSP    = 0x2000;    /// 8192       En Quad
        const Char kCharMQSP    = 0x2001;    /// 8193       Em Quad
        const Char kCharENSP    = 0x2002;    /// 8194       En Space
        const Char kCharEMSP    = 0x2003;    /// 8195       Em Space
        const Char kChar3MSP    = 0x2004;    /// 8196       3-Em Space
        const Char kChar4MSP    = 0x2005;    /// 8197       4-Em Space
        const Char kChar6MSP    = 0x2006;    /// 8198       6-Em Space
        const Char kCharFSP     = 0x2007;    /// 8199       Figure Space (Equivalent to the digit width of fonts with fixed-width digits).
        const Char kCharPSP     = 0x2008;    /// 8200       Punctuation Space (Equavalent to the width of narrow punctuation of a font).
        const Char kCharTHSP    = 0x2009;    /// 8201       Thin Space
        const Char kCharHSP     = 0x200A;    /// 8202       Hair Space
        const Char kCharZWSP    = 0x200B;    /// 8203       Zero-Width Space
        const Char kCharZWNJ    = 0x200C;    /// 8204       Zero-Width Non-Joiner
        const Char kCharZWJ     = 0x200D;    /// 8205       Zero-Width Joiner
        const Char kCharLRM     = 0x200E;    /// 8206       Left-to-Right Mark. See Unicode Standard Annex #9.
        const Char kCharRLM     = 0x200F;    /// 8207       Right-to-Left Mark. See Unicode Standard Annex #9.
        const Char kCharHyphen  = 0x2010;    /// 8208       Hyphen.
        const Char kCharNBHyphen= 0x2011;    /// 8209       No-Break hyphen.
        const Char kCharEllipsis= 0x2026;    /// 8230       Ellipsis.
        const Char kCharLSEP    = 0x2028;    /// 8232       Line Separator. See section 15.2 of the Unicode Standard 4.0.
        const Char kCharPSEP    = 0x2029;    /// 8233       Paragraph Separator. See section 15.2 of the Unicode Standard 4.0.
        const Char kCharLRE     = 0x202A;    /// 8234       Left-to-Right Embedding. See Unicode Standard Annex #9.
        const Char kCharRLE     = 0x202B;    /// 8235       Right-to-Left Embedding. See Unicode Standard Annex #9.
        const Char kCharPDF     = 0x202C;    /// 8236       Pop Directional Formatting. Used to override text directionality. See Unicode Standard Annex #9.
        const Char kCharLRO     = 0x202D;    /// 8237       Left-to-Right Override. Used to override text directionality. See Unicode Standard Annex #9.
        const Char kCharRLO     = 0x202E;    /// 8238       Right-to-Left Override. Used to override text directionality. See Unicode Standard Annex #9.
        const Char kCharWJoin   = 0x2060;    /// 8288       Word Joiner, acts the same as Zero-Width No-Break Space.
        const Char kCharISS     = 0x206A;    /// 8298       Inhibit Symmetric Swapping
        const Char kCharASS     = 0x206B;    /// 8299       Activate Symmetric Swapping
        const Char kCharIAFS    = 0x206C;    /// 8300       Inhibit Arabic Form Shaping
        const Char kCharAAFS    = 0x206D;    /// 8301       Activate Arabic Form Shaping
        const Char kCharNADS    = 0x206E;    /// 8302       National Digit Shapes
        const Char kCharNODS    = 0x206F;    /// 8303       Nominal Digit Shapes
        const Char kCharDCIR    = 0x25CC;    /// 9676       Dotted Circle. Used when text has a decoration glyph but no (preceding) base glyph to go with it.
        const Char kCharPASS    = 0x25CF;    /// 9679       Black Circle, but more often used as the password char.
        const Char kCharIDSP    = 0x3000;    /// 12288      Ideographic Space
        const Char kCharOBJ     = 0xFFFC;    /// 65532      Object replacement character
        const Char kCharREPL    = 0xFFFD;    /// 65533      Replacement character
        const Char kCharZWNBSP  = 0xFEFF;    /// 65279      Zero-Width No-Break Space. Best to use kCharWJoin instead.
        const Char kCharBOM     = 0xFEFF;    /// 65279      Byte Order Mark. Same as kCharZWNBSP.
        const Char kCharBOM_R   = 0xFFFE;    /// 65534      Byte Order Mark, endian-reversed
        const Char kCharInvalid = 0xFFFF;    /// 65535      .


        // Byte order marks
        // If a text file begins with any of these sequences, it is probably a 
        // Unicode text file of the given type. 
        //
        #ifndef kBOM_UTF8
            #define kBOM_UTF8     "\xEF\xBB\xBF"        // UTF8 Unicode
            #define kBOM_UTF16_BE "\xFE\xFF"            // UTF16 Unicode, big-endian encoded.
            #define kBOM_UTF16_LE "\xFF\xFE"            // UTF16 Unicode, little-endian encoded.
            #define kBOM_UTF32_BE "\x00\x00\xFE\xFF"    // UTF32 Unicode, big-endian encoded.
            #define kBOM_UTF32_LE "\xFF\xFE\x00\x00"    // UTF32 Unicode, little-endian encoded.
        #endif


        /// SpaceType
        ///
        /// This enumeration describes various properties of space characters. 
        ///
        /// In the world of advanced text processing, the concept of space or whitespace 
        /// is a bit more complicated than can be seen with the C isspace function. 
        /// An example of this is how some locales use spaces instead of commas to 
        /// designate a numerical thousands separator. However, this space is not a 
        /// standard space but must be a non-breaking space so that numbers with such 
        /// spaces don't get split across lines of text.
        ///
        enum SpaceType
        {
            kSTNone      = 0x00,    /// Space of any type, including non-space.
            kSTWidth     = 0x01,    /// Space that takes up width (e.g. ' '), as opposed to space that has no width (e.g. ZWSP)
            kSTZeroWidth = 0x02,    /// Space that takes up no width (e.g. ZWNBSP), as opposed to space that has width (e.g. TAB)
            kSTBreak     = 0x04,    /// Space that allows word breaks on either side of it (e.g. ' '), as opposed to space doesn't allow word breaks (e.g. NBSP) 
            kSTNoBreak   = 0x08,    /// Space that doesn't allow word breaks on either side of it (e.g. ZWNBSP), as opposed to space does allow word breaks (e.g. ZWSP).
            kSTControl   = 0x10,    /// Space that is from control characters, such as \t, \r, \n.
            kSTAll       = 0xff     /// All space types or'd together.
        };


        /// Direction
        ///
        /// Refers to text layout direction within a given orientation.
        /// The values for kDirectionLTR and kDirectionRTL correspond to the 
        /// standard convention that LTR text has an even embedding level
        /// value while RTL text has an odd embedding level.
        ///
        enum Direction
        {
            kDirectionLTR     = 0,      /// Left to right (e.g. English). For vertical orientation, refers to top to bottom.
            kDirectionRTL     = 1,      /// Right to left (e.g. Arabic). For vertical orientation, refers to bottom to top.
            kDirectionNeutral = 2,      /// No specific direction.
            kDirectionDefault = kDirectionLTR
        };


        /// Orientation
        ///
        /// Refers to text layout orientation. 
        ///
        enum Orientation
        {
            kOrientationHorizontal,    /// Horizontal orientation, used by Latin, Greek, Hebrew, Arabic, Chinese, etc.
            kOrientationVertical,      /// Vertical orientation, used by Chinese, Japanese, Korean, etc.
            kOrientationDefault = kOrientationHorizontal
        };


        /// EastAsianWidth
        /// 
        /// Defines the type of width associated with an East-Asian character.
        /// East-Asian scripts such as Japanese classify characters graphically as being
        /// half width, full width, etc. This is because such scripts often view text as
        /// being laid out in an organized grid of rows and columns. 
        ///
        enum EastAsianWidth 
        {
            kEAWNeutral,         /// N
            kEAWAmbiguous,       /// A
            kEAWHalfWidth,       /// H
            kEAWFullWidth,       /// F
            kEAWNarrow,          /// Na
            kEAWWide             /// W
        };


        /// DecompositionType
        /// 
        /// Defines character decomposition types. 
        /// Decomposition refers to the process of breaking a composed or shaped character 
        /// down into its basic parts, often for the purpose of doing proper searching 
        /// and comparison. For example, if you search for the digit 1 in a paragraph of
        /// text and there is a 1 which is in superscript form, it should be found. 
        /// Similarly, a superscript 1 should compare as equal to a regular 1 in terms 
        /// of sorting. Other kinds of composition exist as well, such as the application 
        /// of diacriticals to base characters.
        /// 
        /// Todo: Consider removing this declaration once it is has been source controlled, 
        ///       so that it may be resurrected if and when needed at a later time.
        ///
        enum DecompositionType
        {
            kDTNone,            /// none
            kDTCanonical,       /// can
            kDTCompatible,      /// com
            kDTCircle,          /// enc
            kDTFinal,           /// fin
            kDTFont,            /// font
            kDTFraction,        /// fra
            kDTInitial,         /// init
            kDTIsolated,        /// iso
            kDTMedial,          /// med
            kDTNarrow,          /// nar
            kDTNoBreak,         /// nb
            kDTSmall,           /// sml
            kDTSquare,          /// sqr
            kDTSubscript,       /// sub
            kDTSuperscript,     /// sup
            kDTVertical,        /// vert
            kDTWide             /// wide
        };


        /// ArabicJoiningClass
        /// 
        /// Defines how joins occur with scripts that do glyph joining. An example of such scripts
        /// are cursive scripts such as those in the Arabic family. Joining types are identified
        /// for various characters of arabic script at http://www.unicode.org/Public/UNIDATA/ArabicShaping.txt.
        /// See section 8.2 of the Unicode standard for a description of these joining classes.
        ///
        enum ArabicJoiningClass
        {
            kAJCNonJoining,      /// U  Zero-width non-joiner and all spacing characters, with some exceptions.
            kAJCJoinCausing,     /// C  Zero-width joiners and the Arabic Tatweel Char (0x0640).
            kAJCDualJoining,     /// D  Beh, Teh, Theh, etc.
            kAJCLeftJoining,     /// L  By the Standard, there are no Chars in this class.
            kAJCRightJoining,    /// R  Alef, Dal, Thal, etc.
            kAJCTransparent      /// T  All non-spacing marks.
        };


        /// ArabicJoiningGroup
        /// 
        /// Defines joining groups for Arabic text.
        ///
        enum JoiningGroup
        {
            kAJGNone,
            kAJGAin,
            kAJGAlaph,
            kAJGAlef,
            kAJGBeh,
            kAJGBeth,
            kAJGDal,
            kAJGDalathRish,
            kAJGE,
            kAJGFeh,
            kAJGFinalSemkath,
            kAJGGaf,
            kAJGGamal,
            kAJGHah,
            kAJGHamzaOnHehGoal,
            kAJGHe,
            kAJGHeh,
            kAJGHehGoal,
            kAJGHeth,
            kAJGKAf,
            kAJGKAph,
            kAJGKnottedHeh,
            kAJGLam,
            kAJGLamadh,
            kAJGMeem,
            kAJGMim,
            kAJGNoon,
            kAJGNun,
            kAJGPp,
            kAJGQAf,
            kAJGQAph,
            kAJGReh,
            kAJGReversedPe,
            kAJGSad,
            kAJGSadHe,
            kAJGSeen,
            kAJGSemkath,
            kAJGShin,
            kAJGSwashKaf,
            kAJGSyriacWaw,
            kAJGTah,
            kAJGTaw,
            kAJGTehMarbuta,
            kAJGTeth,
            kAJGWaw,
            kAJGYeh,
            kAJGYehBarre,
            kAJGYehWithTail,
            kAJGYudh,
            kAJGYudhHe,
            kAJGZain,
            kAJGFe,
            kAJGKhaph,
            kAJGZhain
        };


        /// GlyphJustificationClass
        ///
        /// Defines a classification that applies to any given glyph and which is used to tell how to 
        /// implement justification with respect to this glyph.
        ///
        /// To consider: Consider changing this to be CharJustificationClass. Hard to say which is 
        ///              more correct, as Char seems more clean but Glyph is actually what's dealt with.
        ///
        enum GlyphJustificationClass
        {
            kGJCNone = 0,       /// Justification can't be applied at this glyph.
            kGJCBlank,          /// This glyph represents a blank outside an Arabic run.
            kGJCArabicBlank,    /// This glyph represents a blank in an Arabic run.
            kGJCCharacter,      /// Inter-character justification point follows this glyph.
            kGJCArabicNormal,   /// Normal middle of word glyph that connects to the right.
            kGJCArabicKashida,  /// Kashida (U+0640) in middle of word.
            kGJCArabicAlef,     /// Final form of Alef-like (U+0623, U+0625, U+0627, U+0632).
            kGJCArabicHeh,      /// Final form of Heh (U+0647).
            kGJCArabicReh,      /// Final form of Reh (U+0631).
            kGJCArabicBeh,      /// Middle of word form of Beh (U+0628).
            kGJCArabicBehReh,   /// Ligature of alike (U+0628, U+0631).
            kGJCArabicSeen,     /// Initial shape of Seen (U+0633).
        };


        /// Block
        ///
        /// Defines a set of contiguous related Unicode code points. 
        /// Blocks aren't the same as Scripts, though the two are related. A given script
        /// will often use only characters from a given Unicode block. However, a given
        /// Unicode block can be used by multiple scripts. Also, a given script can use
        /// multiple blocks.
        ///
        /// If some of the names below seem a little long, it's because these are the 
        /// standard Unicode names and we don't want to have any ambiguity about them.
        ///
        enum Block
        {
            kBlockInvalid = -1,
            kBlockNone = 0,
            kBlockBasicLatin,                           /// Begins at U+0000
            kBlockLatin1Supplement,                     /// Begins at U+0080
            kBlockLatinExtendedA,                       /// Begins at U+0100
            kBlockLatinExtendedB,                       /// Begins at U+0180
            kBlockIPAExtensions,                        /// Begins at U+0250
            kBlockSpacingModifierLetters,               /// Begins at U+02B0
            kBlockCombiningDiacriticalMarks,            /// Begins at U+0300
            kBlockGreekAndCoptic,                       /// Begins at U+0370
            kBlockCyrillic,                             /// Begins at U+0400
            kBlockCyrillicSupplement,                   /// Begins at U+0500
            kBlockArmenian,                             /// Begins at U+0530
            kBlockHebrew,                               /// Begins at U+0590
            kBlockArabic,                               /// Begins at U+0600
            kBlockSyriac,                               /// Begins at U+0700
            kBlockThaana,                               /// Begins at U+0780
            kBlockDevanagari,                           /// Begins at U+0900
            kBlockBengali,                              /// Begins at U+0980
            kBlockGurmukhi,                             /// Begins at U+0A00
            kBlockGujarati,                             /// Begins at U+0A80
            kBlockOriya,                                /// Begins at U+0B00
            kBlockTamil,                                /// Begins at U+0B80
            kBlockTelugu,                               /// Begins at U+0C00
            kBlockKannada,                              /// Begins at U+0C80
            kBlockMalayalam,                            /// Begins at U+0D00
            kBlockSinhala,                              /// Begins at U+0D80
            kBlockThai,                                 /// Begins at U+0E00
            kBlockLao,                                  /// Begins at U+0E80
            kBlockTibetan,                              /// Begins at U+0F00
            kBlockMyanmar,                              /// Begins at U+1000
            kBlockGeorgian,                             /// Begins at U+10A0
            kBlockHangulJamo,                           /// Begins at U+1100
            kBlockEthiopic,                             /// Begins at U+1200
            kBlockCherokee,                             /// Begins at U+13A0
            kBlockUnifiedCanadianAboriginalSyllabics,   /// Begins at U+1400
            kBlockOgham,                                /// Begins at U+1680
            kBlockRunic,                                /// Begins at U+16A0
            kBlockTagalog,                              /// Begins at U+1700
            kBlockHanunoo,                              /// Begins at U+1720
            kBlockBuhid,                                /// Begins at U+1740
            kBlockTagbanwa,                             /// Begins at U+1760
            kBlockKhmer,                                /// Begins at U+1780
            kBlockMongolian,                            /// Begins at U+1800
            kBlockLimbu,                                /// Begins at U+1900
            kBlockTaiLe,                                /// Begins at U+1950
            kBlockKhmerSymbols,                         /// Begins at U+19E0
            kBlockPhoneticExtensions,                   /// Begins at U+1D00
            kBlockLatinExtendedAdditional,              /// Begins at U+1E00
            kBlockGreekExtended,                        /// Begins at U+1F00
            kBlockGeneralPunctuation,                   /// Begins at U+2000
            kBlockSuperscriptsAndSubscripts,            /// Begins at U+2070
            kBlockCurrencySymbols,                      /// Begins at U+20A0
            kBlockCombiningDiacriticalMarksForSymbols,  /// Begins at U+20D0
            kBlockLetterLikeSymbols,                    /// Begins at U+2100
            kBlockNumberForms,                          /// Begins at U+2150
            kBlockArrows,                               /// Begins at U+2190
            kBlockMathematicalOperators,                /// Begins at U+2200
            kBlockMiscellaneousTechnical,               /// Begins at U+2300
            kBlockControlPictures,                      /// Begins at U+2400
            kBlockOpticalCharacterRecognition,          /// Begins at U+2440
            kBlockEnclosedAlphanumerics,                /// Begins at U+2460
            kBlockBoxDrawing,                           /// Begins at U+2500
            kBlockBlockElements,                        /// Begins at U+2580
            kBlockGeometricShapes,                      /// Begins at U+25A0
            kBlockMiscellaneousSymbols,                 /// Begins at U+2600
            kBlockDingbats,                             /// Begins at U+2700
            kBlockMiscellaneousMathematicalSymbolsA,    /// Begins at U+27C0
            kBlockSupplementalArrowsA,                  /// Begins at U+27F0
            kBlockBraillePatterns,                      /// Begins at U+2800
            kBlockSupplementalArrowsB,                  /// Begins at U+2900
            kBlockMiscellaneousMathematicalSymbolsB,    /// Begins at U+2980
            kBlockSupplementalMathematicalOperators,    /// Begins at U+2A00
            kBlockMiscellaneousSymbolsandArrows,        /// Begins at U+2B00
            kBlockCJKRadicalsSupplement,                /// Begins at U+2E80
            kBlockKangxiRadicals,                       /// Begins at U+2F00
            kBlockIdeographicDescriptionCharacters,     /// Begins at U+2FF0
            kBlockCJKSymbolsAndPunctuation,             /// Begins at U+3000
            kBlockHiragana,                             /// Begins at U+3040
            kBlockKatakana,                             /// Begins at U+30A0
            kBlockBopomofo,                             /// Begins at U+3100
            kBlockHangulCompatibilityJamo,              /// Begins at U+3130
            kBlockKanbun,                               /// Begins at U+3190
            kBlockBopomofoExtended,                     /// Begins at U+31A0
            kBlockKatakanaPhoneticExtensions,           /// Begins at U+31F0
            kBlockEnclosedCJKLettersAndMonths,          /// Begins at U+3200
            kBlockCJKCompatibility,                     /// Begins at U+3300
            kBlockCJKUnifiedIdeographsExtensionA,       /// Begins at U+3400
            kBlockYijingHexagramSymbols,                /// Begins at U+4DC0
            kBlockCJKUnifiedIdeographs,                 /// Begins at U+4E00
            kBlockYiSyllables,                          /// Begins at U+A000
            kBlockYiRadicals,                           /// Begins at U+A490
            kBlockHangulSyllables,                      /// Begins at U+AC00
            kBlockHighSurrogates,                       /// Begins at U+D800
            kBlockHighPrivateUseSurrogates,             /// Begins at U+DB80
            kBlockLowSurrogates,                        /// Begins at U+DC00
            kBlockPrivateUseArea,                       /// Begins at U+E000
            kBlockCJKCompatibilityIdeographs,           /// Begins at U+F900
            kBlockAlphabeticPresentationForms,          /// Begins at U+FB00
            kBlockArabicPresentationFormsA,             /// Begins at U+FB50
            kBlockVariationSelectors,                   /// Begins at U+FE00
            kBlockCombiningHalfMarks,                   /// Begins at U+FE20
            kBlockCJKCompatibilityForms,                /// Begins at U+FE30
            kBlockSmallFormVariants,                    /// Begins at U+FE50
            kBlockArabicPresentationFormsB,             /// Begins at U+FE70
            kBlockHalfWidthAndFullWidthForms,           /// Begins at U+FF00
            kBlockSpecials                              /// Begins at U+FFF0
        };


        /// LineBreakCategory
        ///
        /// Identifies the type of line breaking type or category a character is associated with.
        /// These types are as per the Unicode standard. See http://www.unicode.org/reports/tr7/
        ///
        enum LineBreakCategory
        {
            // Note that as of this writing, the order of these is significant, 
            // as it is used in the line break calculation code.
            kLBCOpenPunctuation,    /// OP
            kLBCClosePunctuation,   /// CL
            kLBCQuotation,          /// QU
            kLBCGlue,               /// GL
            kLBCNonStarter,         /// NS
            kLBCExclamation,        /// EX
            kLBCBreakSymbols,       /// SY
            kLBCInfixNumeric,       /// IS
            kLBCPrefixNumeric,      /// PR
            kLBCPostfixNumeric,     /// PO
            kLBCNumeric,            /// NU
            kLBCAlphabetic,         /// AL
            kLBCIdeographic,        /// ID
            kLBCInseperable,        /// IN
            kLBCHyphen,             /// HY
            kLBCBreakAfter,         /// BA
            kLBCBreakBefore,        /// BB
            kLBCBreakBoth,          /// B2
            kLBCZWSpace,            /// ZW
            kLBCCombiningMark,      /// CM
            kLBCWordJoiner,         /// WJ
            kLBCHangulJamo2,        /// H2
            kLBCHangulJamo3,        /// H3
            kLBCHangulJamoLeading,  /// JL
            kLBCHangulJamoVowel,    /// JV
            kLBCHangulJamoTrailing, /// JT
            kLBCSoutheastAsian,     /// SA
            kLBCSpace,              /// SP
            kLBCParagraphSeparator, /// PS
            kLBCMandatoryBreak,     /// BK
            kLBCCarriageReturn,     /// CR
            kLBCLineFeed,           /// LF
            kLBCNextLine,           /// NL
            kLBCContingentBreak,    /// CB
            kLBCSurrogate,          /// SG
            kLBCAmbiguous,          /// AI
            kLBCUnknown,            /// XX
        };


        /// BidiClass
        ///
        /// A background in the general topic of directional text can be found here:
        ///    http://en.wikipedia.org/wiki/Bi-directional_text
        /// The foundation of understanding computer processing of directional text
        /// can be found here:
        ///    http://www.unicode.org/reports/tr9/
        ///
        /// Note that the ordering of classes below is very different from that given 
        /// in the Unicode standard and UAX 9 Table 3.7. This is because the ordering 
        /// below is what allows the bidirectional algorithm implementation to work
        /// more smoothly. For the most part, the BidiClass enumeration is useful only
        /// to the bidirection algorithm anyway and so using the ordering given in 
        /// the Unicode standard texts doesn't buy much beyond taxonomic readability.
        ///
        enum BidiClass
        {
            kBidiClassON = 0,   /// Other Neutrals. Includes all other characters, including OBJECT REPLACEMENT CHARACTER.
            kBidiClassL,        /// Left-to-Right. Includes LRM, Most alphabetic, syllabic, Han ideographic characters, digits that are neither European nor Arabic, ...
            kBidiClassR,        /// Right-to-Left. Includes RLM, Hebrew alphabet, most punctuation specific to that script, ...
            kBidiClassAN,       /// Arabic Number. Includes Arabic-Indic digits, Arabic decimal & thousands separators, ...
            kBidiClassEN,       /// European Number. Includes European digits, Eastern Arabic-Indic digits, ...
            kBidiClassAL,       /// Right-to-Left Arabic. Includes Arabic, Thaana, and Syriac alphabets, most punctuation specific to those scripts, ...
            kBidiClassNSM,      /// Non-Spacing Mark. Includes characters marked Mn (Non-Spacing Mark) and Me (Enclosing Mark) in the Unicode Character Database.
            kBidiClassCS,       /// Common Number Separator. Includes colon, comma, full stop (period), non-breaking space, ...
            kBidiClassES,       /// European Number Separator. Includes plus sign, minus sign.
            kBidiClassET,       /// European Number Terminator. Includes degree, currency symbols, ...
            kBidiClassBN,       /// Boundary Neutral. Includes most formatting and control characters, other than those explicitly given types above.
            kBidiClassS,        /// Segment Separator. Includes tab.
            kBidiClassWS,       /// Whitespace. Includes space, figure space, line separator, form feed, general punctuation spaces, ...
            kBidiClassB,        /// Paragraph Separator. Includes paragraph separator, appropriate newline functions, higher-protocol paragraph determination.
            kBidiClassRLO,      /// Right-to-Left Override.
            kBidiClassRLE,      /// Right-to-Left Embedding.
            kBidiClassLRO,      /// Left-to-Right Override.
            kBidiClassLRE,      /// Left-to-Right Embedding.
            kBidiClassPDF,      /// Pop Directional Format.
        };


        /// CaseType
        ///
        /// Defines the types of case recognized by the Unicode standard.
        ///
        enum CaseType
        {
            kCaseTypeNone,      /// No case.
            kCaseTypeLower,     /// Lower case.
            kCaseTypeUpper,     /// Upper case.
            kCaseTypeTitle,     /// Title case, which refers to chars in some language which combine an upper and lower character, such as Dz.
            kCaseTypeReverse    /// The reverse of the current case. Useful for flipping string sort order between upper and lower case strings.
        };


        /// HangulSyllableType
        ///
        /// Defines the recognized Hangul (Korean) syllable types.
        ///
        enum HangulSyllableType
        {                       /// Also known in the Unicode standard as:
            kHSTNone,           /// NA
            kHSTLeadingJamo,    /// L
            kHSTVowelJamo,      /// V
            kHSTTrailingJamo,   /// T
            kHSTLVSyllable,     /// LV
            kHSTLVTSyllable,    /// LVT
        };


        /// NormalizationType
        ///
        /// See Unicode TF 15 (http://www.unicode.org/reports/tr15/).
        ///
        enum NormalizationType
        {
            kNT_None,  /// 
            kNT_NFC,   /// Normalization Form C (composition).
            kNT_NFKC,  /// Normalization Form Compatibility C.
            kNT_NFD,   /// Normalization Form D (decomposition).
            kNT_NFKD
        };


        /// CharProperty
        ///
        /// Defines Unicode code point (character) properties as per the Unicode standard.
        /// Note that individual characters may have more than one of these properties.
        /// The list below is somewhat comprehensive, but while we define all these properties,
        /// we don't necessarily store information for all of them. A full heavyweight Unicode
        /// library might store a full database of character information, but we in practice
        /// pick and choose only what we need. For those items which aren't intially supported,
        /// these values serve as a placeholder for the day when such items may be needed.
        /// We keep the unused properties defined so that databases which use the properties 
        /// don't need to be recreated when enumerated values change.
        ///
        /// For information about Unicode character properties, see http://www.unicode.org/ucd/.
        /// Each of the properties is described there and in many cases the explanation is not
        /// very hard to understand for most programmers.
        ///
        enum CharProperty
        {                                           /// Supported?   Description
            kCPInvalid = -1,                        /// (NA)         Represents a nonexistent or invalid property or property value.

            // Boolean properties
            kCPAlphabetic = 0,                      /// +            Boolean property Alphabetic. Lu + Ll + Lt + Lm + Lo + Nl + Other_Alphabetic
            kCPBooleanBegin = kCPAlphabetic,        /// (NA)         Start of boolean properties.
            kCPASCIIHexDigit,                       /// -            Boolean property ASCII_Hex_Digit. 0-9 A-F a-f
            kCPBidiControl,                         /// -            Boolean property Bidi_Control. Format controls which have specific functions in the Bidi Algorithm.
            kCPBidiMirrored,                        /// +            Boolean property Bidi_Mirrored. Characters that may change display in RTL text.
            kCPDash,                                /// -            Boolean property Dash. Variations of dashes.
            kCPDefaultIgnorableCodePoint,           /// -            Boolean property Default_Ignorable_Code_Point. Ignorable in most processing. <2060 - 206F, FFF0 - FFFB, E0000 - E0FFF> + Other_Default_Ignorable_Code_Point + (Cf + Cc + Cs - White_Space)
            kCPDeprecated,                          /// -            Boolean property Deprecated. The use of deprecated characters is discouraged.
            kCPDiacritic,                           /// -            Boolean property Diacritic. Characters that linguistically modify the meaning of another character to which they apply.
            kCPExtender,                            /// -            Boolean property Extender. Extend the value or shape of a preceding alphabetic character, e.g., length and iteration marks.
            kCPFullCompositionExclusion,            /// -            Boolean property Full_Composition_Exclusion. CompositionExclusions.txt + Singleton Decompositions+ Non-Starter Decompositions.
            kCPGraphemeBase,                        /// -            Boolean property Grapheme_Base. For programmatic determination of grapheme cluster boundaries. [0 - 10FFFF] - Cc - Cf - Cs - Co - Cn - Zl - Zp - Grapheme_Link - Grapheme_Extend - CGJ
            kCPGraphemeExtend,                      /// +            Boolean property Grapheme_Extend. For programmatic determination of grapheme cluster boundaries. Me + Mn + Mc + Other_Grapheme_Extend - Grapheme_Link - CGJ
            kCPGraphemeLink,                        /// -            Boolean property Grapheme_Link. For programmatic determination of grapheme cluster boundaries.
            kCPHexDigit,                            /// -            Boolean property Hex_Digit. Characters commonly used for hexadecimal numbers. Includes hex digits from other Unicode blocks than just ASCII.
            kCPHyphen,                              /// -            Boolean property Hyphen. Dashes used to mark connections between pieces of words, plus the Katakana middle dot.
            kCPIDContinue,                          /// -            Boolean property ID_Continue. Characters that can continue an identifier. ID_Start + Mn + Mc + Nd + Pc.
            kCPIDStart,                             /// -            Boolean property ID_Start. Characters that can start an identifier. Lu + Ll + Lt + Lm + Lo + Nl.
            kCPIdeographic,                         /// +            Boolean property Ideographic. CJKV ideographs.
            kCPIDSBooleanOperator,                  /// -            Boolean property IDS_Boolean_Operator. For programmatic determination of Ideographic Description Sequences.
            kCPIDSTrinaryOperator,                  /// -            Boolean property IDS_Trinary_Operator. For programmatic determination of Ideographic Description Sequences.
            kCPJoinControl,                         /// +            Boolean property Join_Control. Format controls for cursive joining and ligation.
            kCPLogicalOrderException,               /// -            Boolean property Logical_Order_Exception. Characters that do not use logical order and require special handling in most processing.
            kCPLowercase,                           /// -            Boolean property Lowercase. Ll + Other_Lowercase.
            kCPMath,                                /// -            Boolean property Math. Sm + Other_Math.
            kCPNonCharacterCodePoint,               /// -            Boolean property Noncharacter_Code_Point. Code points that are explicitly defined as illegal for the encoding of characters.
            kCPQuotationMark,                       /// -            Boolean property Quotation_Mark.
            kCPRadical,                             /// -            Boolean property Radical. For programmatic determination of ideographic description sequences.
            kCPSoftDotted,                          /// -            Boolean property Soft_Dotted. Characters with a "soft dot", like i or j. An accent placed on these characters causes the dot to disappear.
            kCPTerminalPunctuation,                 /// -            Boolean property Terminal_Punctuation. Punctuation characters that generally mark the end of textual units.
            kCPUnifiedIdeograph,                    /// -            Boolean property Unified_Ideograph. For programmatic determination of ideographic description sequences.
            kCPUppercase,                           /// -            Boolean property Uppercase. Lu + Other_Uppercase
            kCPWhiteSpace,                          /// +            Boolean property White_Space. Space characters + TAB + CR + LF - ZWSP - ZWNBSP
            kCPXIDContinue,                         /// -            Boolean property XID_Continue. ID_Continue modified to allow closure under normalization forms NFKC and NFKD.
            kCPXIDStart,                            /// -            Boolean property XID_Start. ID_Start modified to allow closure under normalization forms NFKC and NFKD.
            kCPCaseSensitive,                       /// -            Boolean property Case_Sensitive. Either the source of a case mapping or -in- the target of a case mapping. Not the same as the general category Cased_Letter.
            kCPSTerm,                               /// +            Boolean property STerm. Sentence Terminal. Used in UAX #29: Text Boundaries (http://www.unicode.org/reports/tr29/)
            kCPVariationSelector,                   /// -            Boolean property Variation_Selector. Indicates all those characters that qualify as Variation Selectors. For details on the behavior of these characters, see Unicode's StandardizedVariants.html and 15.6 Variation Selectors.
            kCPSegmentStarter,                      /// -            Boolean Property Segment_Starter.
            kCPBooleanEnd,                          /// (NA)         One more than the last constant for boolean Unicode properties.

            // Integer properties
            kCPBidiClass = 0x1000,                  /// +            Enumerated Unicode property Bidi_Class. Returns enum BidiClass values.
            kCPIntegerBegin = kCPBidiClass,         /// (NA)         First constant for enumerated/integer Unicode properties.
            kCPBlock,                               /// +            Enumerated Unicode property Block. Returns enum Block values.
            kCPScript,                              /// -            Enumerated Unicode property Script. Returns enum Script values.
            kCPCanonicalCombiningClass,             /// +            Enumerated Unicode property Canonical_Combining_Class. Returns 8-bit numeric values.
            kCPDecompositionType,                   /// -            Enumerated Unicode property Decomposition_Type. Returns enum DecompositionType values.
            kCPEastAsianWidth,                      /// -            Enumerated Unicode property East_Asian_Width. See http://www.unicode.org/reports/tr11/. Returns enum EastAsianWidth values.
            kCPGeneralCategory,                     /// -            Enumerated Unicode property General_Category. Returns enum CharCategory values.
            kCPJoiningGroup,                        /// -            Enumerated Unicode property Joining_Group. Returns enum JoiningGroup values.
            kCPJoiningType,                         /// -            Enumerated Unicode property Joining_Type. Returns enum JoiningType values.
            kCPSpaceType,                           /// +            Not a formal Unicode property. Returns enum SpaceType values. 
            kCPLineBreak,                           /// +            Enumerated Unicode property Line_Break. Returns enum LineBreakCategory values.
            kCPNumericType,                         /// -            Enumerated Unicode property Numeric_Type. Returns enum NumericType values.
            kCPHangulSyllableType,                  /// +            Enumerated Unicode property Hangul_Syllable_Type. Returns enum HangulSyllableType values.
            kCPLeadCanonicalCombiningClass,         /// -            Enumerated Unicode property Lead_Canonical_Combining_Class. Returns 8-bit numeric values like kCPCanonicalCombiningClass. See http://www.unicode.org/notes/tn5/#FCD
            kCPTrailCanonicalCombiningClass,        /// -            Enumerated Unicode property Trail_Canonical_Combining_Class. Returns 8-bit numeric values like kCPCanonicalCombiningClass. See http://www.unicode.org/notes/tn5/#FCD
            kCPIntegerEnd,                          /// (NA)         One more than the last constant for enumerated/integer Unicode properties.

            // Mask (bitfield) properties
            kCPGeneralCategoryMask = 0x2000,        /// -            Bitmask property General_Category_Mask. This is the General_Category property returned as a bit mask.
            kCPMaskBegin = kCPGeneralCategoryMask,  /// (NA)         First constant for bit-mask Unicode properties.
            kCPMaskEnd,                             /// (NA)         One more than the last constant for bit-mask Unicode properties.

            // Floating point properties
            kCPNumericValue = 0x3000,               /// -            Float property Numeric_Value.
            kCPFloatBegin = kCPNumericValue,        /// (NA)         First constant for float Unicode properties.
            kCPFloatEnd,                            /// (NA)         One more than the last constant for floating point Unicode properties.

            // String properties
            kCPAge = 0x4000,                        /// -            String property Age.
            kCPStringBegin = kCPAge,                /// (NA)         First constant for string Unicode properties.
            kCPBidiMirroringGlyph,                  /// +            String property Bidi_Mirroring_Glyph.
            kCPCaseFolding,                         /// -            String property Case_Folding.
            kCPISOComment,                          /// -            String property ISO_Comment.
            kCPLowercaseMapping,                    /// -            String property Lowercase_Mapping.
            kCPName,                                /// +            String property Name.
            kCPSimpleCaseFolding,                   /// -            String property Simple_Case_Folding.
            kCPSimpleLowercaseMapping,              /// -            String property Simple_Lowercase_Mapping.
            kCPSimpleTitlecaseMapping,              /// -            String property Simple_Titlecase_Mapping.
            kCPSimpleUppercaseMapping,              /// -            String property Simple_Uppercase_Mapping.
            kCPTitlecaseMappiong,                   /// -            String property Titlecase_Mapping.
            kCPUnicode1Name,                        /// -            String property Unicode_1_Name.
            kCPUppercaseMapping,                    /// -            String property Uppercase_Mapping.
            kCPStringEnd,                           /// (NA)         One more than the last constant for string Unicode properties.
        };


        /// CharCategory
        ///
        /// The Unicode standard assigns every code point (character) a single category.
        /// These categories are useful for broad categorization of individual characters
        /// during text processing. Character categories are different from character 
        /// properties (enum CharProperty). CharProperties are more useful in practice,
        /// but character categories have their uses.
        /// 
        /// These definitions are derived from the Unicode standard at
        /// http://www.unicode.org/Public/UNIDATA/UnicodeData.html.
        //
        enum CharCategory
        {
            kCCUnassigned             =  0, /// Non-category for unassigned and non-character code points.
            kCCGeneralOtherTypes      =  0, /// Cn other, not assigned. Same as kCCUnassigned. Note that no characters in UnicodeData.txt have this property. 
            kCCControlChar            =  1, /// Cc
            kCCFormatChar             =  2, /// Cf -- includes special characters to control formatting, such as kCharLRE, kCharPDF, kCharZWJ
            kCCPrivateUseChar         =  3, /// Co
            kCCSurrogate              =  4, /// Cs
            kCCUppercaseLetter        =  5, /// Lu
            kCCLowercaseLetter        =  6, /// Ll
            kCCTitlecaseLetter        =  7, /// Lt
            kCCModifierLetter         =  8, /// Lm
            kCCOtherLetter            =  9, /// Lo
            kCCNonSpacingMark         = 10, /// Mn
            kCCEnclosingMark          = 11, /// Me
            kCCCombiningSpacingMark   = 12, /// Mc
            kCCDecimalDigitNumber     = 13, /// Nd
            kCCLetterNumber           = 14, /// Nl
            kCCOtherNumber            = 15, /// No
            kCCSpaceSeparator         = 16, /// Zs
            kCCLineSeparator          = 17, /// Zl
            kCCParagraphSeparator     = 18, /// Zp
            kCCDashPunctuation        = 19, /// Pd
            kCCStartPunctuation       = 20, /// Ps
            kCCEndPunctuation         = 21, /// Pe
            kCCConnectorPunctuation   = 22, /// Pc
            kCCOtherPunctuation       = 23, /// Po
            kCCInitialPunctuation     = 24, /// Pi
            kCCFinalPunctuation       = 25, /// Pf
            kCCMathSymbol             = 26, /// Sm
            kCCCurrencySymbol         = 27, /// Sc
            kCCModifierSymbol         = 28, /// Sk
            kCCOtherSymbol            = 29  /// So
        };


        /// CharCategoryFlags
        ///
        /// Defines flags for enum CharCategory. 
        ///
        enum CharCategoryFlags
        {
            kCCFlagUnassigned             = 1 <<  0, /// Non-category for unassigned and non-character code points.
            kCCFlagGeneralOtherTypes      = 1 <<  0, /// Cn other, not assigned. Same as kCCFlagUnassigned. Note that no characters in UnicodeData.txt have this property. 
            kCCFlagControlChar            = 1 <<  1, /// Cc
            kCCFlagFormatChar             = 1 <<  2, /// Cf     http://www.fileformat.info/info/unicode/category/Cf/list.htm
            kCCFlagPrivateUseChar         = 1 <<  3, /// Co
            kCCFlagSurrogate              = 1 <<  4, /// Cs
            kCCFlagUppercaseLetter        = 1 <<  5, /// Lu
            kCCFlagLowercaseLetter        = 1 <<  6, /// Ll
            kCCFlagTitlecaseLetter        = 1 <<  7, /// Lt
            kCCFlagModifierLetter         = 1 <<  8, /// Lm
            kCCFlagOtherLetter            = 1 <<  9, /// Lo
            kCCFlagNonSpacingMark         = 1 << 10, /// Mn
            kCCFlagEnclosingMark          = 1 << 11, /// Me
            kCCFlagCombiningSpacingMark   = 1 << 12, /// Mc
            kCCFlagDecimalDigitNumber     = 1 << 13, /// Nd     http://www.fileformat.info/info/unicode/category/Nd/list.htm
            kCCFlagLetterNumber           = 1 << 14, /// Nl
            kCCFlagOtherNumber            = 1 << 15, /// No
            kCCFlagSpaceSeparator         = 1 << 16, /// Zs
            kCCFlagLineSeparator          = 1 << 17, /// Zl
            kCCFlagParagraphSeparator     = 1 << 18, /// Zp
            kCCFlagDashPunctuation        = 1 << 19, /// Pd
            kCCFlagStartPunctuation       = 1 << 20, /// Ps
            kCCFlagEndPunctuation         = 1 << 21, /// Pe
            kCCFlagConnectorPunctuation   = 1 << 22, /// Pc     http://www.fileformat.info/info/unicode/category/Pc/list.htm
            kCCFlagOtherPunctuation       = 1 << 23, /// Po
            kCCFlagInitialPunctuation     = 1 << 24, /// Pi
            kCCFlagFinalPunctuation       = 1 << 25, /// Pf
            kCCFlagMathSymbol             = 1 << 26, /// Sm
            kCCFlagCurrencySymbol         = 1 << 27, /// Sc
            kCCFlagModifierSymbol         = 1 << 28, /// Sk
            kCCFlagOtherSymbol            = 1 << 29, /// So     http://www.fileformat.info/info/unicode/category/So/list.htm

            kCCFlagLetter       = kCCFlagUppercaseLetter | kCCFlagLowercaseLetter | kCCFlagTitlecaseLetter | kCCFlagModifierLetter | kCCFlagOtherLetter,
            kCCFlagCaseLetter   = kCCFlagUppercaseLetter | kCCFlagLowercaseLetter | kCCFlagTitlecaseLetter,
            kCCFlagMark         = kCCFlagNonSpacingMark | kCCFlagEnclosingMark | kCCFlagCombiningSpacingMark,
            kCCFlagNumber       = kCCFlagDecimalDigitNumber | kCCFlagLetterNumber | kCCFlagOtherNumber,
            kCCFlagSeparator    = kCCFlagSpaceSeparator | kCCFlagLineSeparator | kCCFlagParagraphSeparator,
            kCCFlagOther        = kCCFlagGeneralOtherTypes | kCCFlagControlChar | kCCFlagFormatChar | kCCFlagPrivateUseChar | kCCFlagSurrogate,
            kCCFlagPunctuation  = kCCFlagDashPunctuation | kCCFlagStartPunctuation | kCCFlagEndPunctuation | kCCFlagConnectorPunctuation | kCCFlagOtherPunctuation | kCCFlagInitialPunctuation | kCCFlagFinalPunctuation,
            kCCFlagSymbol       = kCCFlagMathSymbol | kCCFlagCurrencySymbol | kCCFlagModifierSymbol | kCCFlagOtherSymbol
        };


        /// IsSpace
        ///
        /// Tells you if a character is a space character of a given type. 
        ///
        /// This is a generic function that can often tell you what you need to know 
        /// about a space character. In working with text processing you may very well
        /// not want to use the C isspace function, if for no othe reason than because
        /// it doesn't support Unicode.
        ///
        /// The return value is true if the spaceTypeFlags apply to the given character.
        /// If bRequireAllFlags is true, then all spaceTypeFlags must apply, otherwise 
        /// any of the spaceTypeFlags can apply for the return value to be true.
        ///
        /// If you want to tell of a character is a no-break space, you would do this:
        ///    IsSpace(c, kSTNoBreak);
        ///
        /// If you want to tell of a character is a zero-width breakable space, you would do this:
        ///    IsSpace(c, kSTZeroWidth | kSTBreak);
        ///
        /// The C isspace function is essentially equivalent to using:
        ///    IsSpace(c, kSTWidth | kSTControl);
        ///
        /// If you want to tell if a character is not a space of any type:
        ///    IsSpace(c, kSTAll, false);
        ///
        EATEXT_API bool IsSpace(Char c, int spaceTypeFlags, bool bRequireAllFlags = true);


        /// IsCharZeroWidth
        ///
        /// Returns true if the Char by definition is of zero width. Some characters
        /// may or may not be of zero width based on a given font, while others are 
        /// defined by the Unicode standard to be of zero width. An example of a 
        /// character that is of zero width is 0x200B (ZERO WIDTH SPACE).
        ///
        EATEXT_API bool IsCharZeroWidth(Char c);


        /// GetCharBlock
        ///
        /// Returns the block that the character comes from, or returns kBlockNone
        /// if the block is not known.
        ///
        /// GetCharBlock(c) is the same as HasCharProperty(c, kCPBlock);
        ///
        EATEXT_API Block GetCharBlock(Char c);


        /// GetCharCategory
        ///
        /// Returns the category of the given character. Every character is assigned one 
        /// category by the Unicode standard. 
        ///
        EATEXT_API CharCategory GetCharCategory(Char c);


        /// GetCharCategory
        ///
        /// Returns true if the character is of one of the given categories specified by the
        /// input flags.
        ///
        EATEXT_API bool IsCharCategory(Char c, int nCharCategoryFlags);


        /// GetCharName
        ///
        /// Gets the Unicode 4.0 name for the given Unicode code point. 
        /// The name is copied to pName and is null-terminated. 
        ///
        /// The return value is the Strlen of the name of the character.
        /// If nNameLength is not large enough to hold the name, the name
        /// is not copied to pName and the return value is >= nNameLength.
        /// The required length of pName will never be greater than 96.
        ///
        /// This function is not implemented for all platforms and in general
        /// is likely to be mostly used for tools rather than runtimes.
        ///
        EATEXT_API uint32_t GetCharName(Char c, Char* pName, uint32_t nNameCapacity);


        /// GetMirrorChar
        ///
        /// In working with bidirectional text shaping and layout, some characters
        /// need to be mirrored in order to display properly in reverse direction.
        /// An example is the '(' character, which needs to be displayed as ')'
        /// when it is reversed. The Unicode standard specifies which characters    
        /// are mirrorable. See UAX #9: Bidirectional Algorithm, at 
        ///    http://www.unicode.org/unicode/reports/tr9/
        /// 
        /// Return value is the mirror character of the input character, or the 
        /// character value itself if there is no mirror. Most characters in 
        /// Unicode space don't have the 'mirrorable' property.  
        /// 
        /// GetMirrorChar(c) is the same as GetCharProperty(c, kCPBidiMirrored);
        ///
        EATEXT_API Char GetMirrorChar(Char c);


        /// IsCharAlphabetic
        ///
        /// IsCharAlphabetic(c) is the same as HasCharProperty(c, kCPAlphabetic);
        ///
        EATEXT_API bool IsCharAlphabetic(Char c);


        /// IsCharIdeographic
        ///
        /// IsCharIdeographic(c) is the same as HasCharProperty(c, kCPIdeographic);
        ///
        EATEXT_API bool IsCharIdeographic(Char c);


        /// IsCharIdeographic
        ///
        /// IsCharIdeographic(c) is the same as HasCharProperty(c, kCPJoinControl);
        ///
        EATEXT_API bool IsCharJoinControl(Char c);


        /// IsCharWhiteSpace
        ///
        /// Unicode white space is not exactly the same as the C isspace function.
        /// IsCharIdeographic(c) is the same as HasCharProperty(c, kCPWhiteSpace);
        ///
        EATEXT_API bool IsCharWhiteSpace(Char c);


        /// IsCharDigit
        ///
        /// Yields the digit value of a character.
        /// The return value is true if the input Char corresponds to a digit 
        /// char, and false if not. If true then the output 'digit' contains
        /// the integer value of the digit. Otherwise 'digit is untouched. 
        /// This function is like the C isdigit function except it also returns
        /// the digit value and works for non-ASCII locales.
        ///
        /// Example usage:
        //      int digit;
        ///     bool isDigit = GetDigit('7', d);  // Returns true and sets 'd' to 7.
        ///
        EATEXT_API bool IsCharDigit(Char c, int& digit);


        /// IsCharSTerm
        ///
        /// Sentence terminator.
        /// IsCharSTerm(c) is the same as HasCharProperty(c, kCPSTerm);
        ///
        EATEXT_API bool IsCharSTerm(Char c);


        /// GetBidiClass
        ///
        /// GetBidiClass(c) is the same as GetCharProperty(c, kCPBidiClass);
        ///
        EATEXT_API BidiClass GetBidiClass(Char c);


        /// IsCharHyphen
        ///
        /// IsCharHyphen(c) is the same as HasCharProperty(c, kCPHyphen);
        ///
        EATEXT_API bool IsCharHyphen(Char c);


        /// IsCharUppercase
        ///
        /// IsCharUppercase(c) is the same as HasCharProperty(c, kCPUppercase);
        ///
        EATEXT_API bool IsCharUppercase(Char c);


        /// IsGraphemeExtend
        ///
        /// IsGraphemeExtend(c) is the same as HasCharProperty(c, kCPGraphemeExtend);
        ///
        EATEXT_API bool IsGraphemeExtend(Char c);


        /// GetHangulSyllableType
        ///
        /// GetHangulSyllableType(c) is the same as GetCharProperty(c, kCPHangulSyllableType);
        ///
        EATEXT_API HangulSyllableType GetHangulSyllableType(Char c);

        
        /// GetLineBreakCategory
        ///
        /// GetLineBreakCategory(c) is the same as GetCharProperty(c, kCPLineBreak);
        ///
        EATEXT_API LineBreakCategory GetLineBreakCategory(Char c);


        /// GetDirection
        ///
        /// Determines the default direction (LTR or RTL) of a block of text.
        /// Returns one of enum Direction. 
        ///
        /// If you have a paragraph of text that is stored as multiple segments
        /// of text instead of a single string of text, you can call this function
        /// on each successive segment until you get an answer that's not
        /// equal to kDirectionNeutral.
        ///
        /// This function follows the standard specified by Unicode TR9
        /// (a.k.a. UAX 9), rules P2 and P3. However, when writing a text display
        /// engine, it is best if you know up front what base direction your 
        /// paragraphs should have. Usually this is based on the locale the 
        /// application is running in. See EATextScript.h/cpp for how to 
        /// convert a locale to a direction.
        ///
        EATEXT_API int GetDirection(const Char* pText, uint32_t nTextLength);


        /// GetCombiningClass
        ///
        /// A combining class is a somewhat arbitrary number which has no
        /// mathematical meaning beyond comparison with another combining class.
        /// Base characters (e.g. all Latin characters) have a combining class
        /// of zero, whereas accents and other decoration characters have combining
        /// classes of non-zero. From a typographical standpoint, combining characters
        /// of an equivalent class visually go in the same place on the character,
        /// whereas combining characters of different classes decorate characters
        /// in different places. Combining classes are important for doing locale-
        /// proper sorting and searching of text.
        ///
        /// GetCombiningClass(c) is the same as GetCharProperty(c, kCPCanonicalCombiningClass);
        ///
        EATEXT_API int32_t GetCombiningClass(Char c);


        /// GetDecompositionType
        ///
        /// Returns one of enum DecompositionType.
        ///
        EATEXT_API DecompositionType GetDecompositionType(Char c);


        /// ASCIICompatibleEncoding
        ///
        /// Returns the required strlen of the destination.
        ///
        /// http://tools.ietf.org/html/rfc3492
        /// http://tools.ietf.org/html/rfc5890, http://tools.ietf.org/html/rfc5891
        /// https://developer.mozilla.org/en/Internationalized_Domain_Names_%28IDN%29_Support_in_Mozilla_Browsers#ASCII-compatible_encoding_%28ACE%29
        ///
        EATEXT_API uint32_t ASCIICompatibleEncoding(const Char* pTextInput, uint32_t nTextLength, char8_t* pTextOutput, uint32_t nTextOutputCapacity);

    } // namespace Text

} // namespace EA





///////////////////////////////////////////////////////////////////////////////
// Inline functions
///////////////////////////////////////////////////////////////////////////////

inline bool EA::Text::IsCharZeroWidth(Char c)
{
    // The standard zero width chars are:
    //     00AD  SOFT HYPHEN
    //     034F  COMBINING GRAPHEME JOINER
    //     200B  ZERO WIDTH SPACE
    //     200C  ZERO WIDTH NON-JOINER
    //     200D  ZERO WIDTH JOINER
    //     200E  LEFT-TO-RIGHT MARK
    //     200F  RIGHT-TO-LEFT MARK
    //     2028  LINE SEPARATOR
    //     202A  LEFT-TO-RIGHT EMBEDDING
    //     202B  RIGHT-TO-LEFT EMBEDDING
    //     202C  POP DIRECTIONAL FORMATTING
    //     202D  LEFT-TO-RIGHT OVERRIDE
    //     202E  RIGHT-TO-LEFT OVERRIDE
    //     2060  WORD JOINER
    //     2061  FUNCTION APPLICATION
    //     2062  INVISIBLE TIMES
    //     2063  INVISIBLE SEPARATOR
    //     FEFF  ZERO WIDTH NO-BREAK SPACE
    return EA_UNLIKELY(((((unsigned)c & ~0x007Fu) == 0x2000) &&               // This section tests to see if the char is 
                       ((((unsigned)c - 0x200Bu) <= (0x200Fu - 0x200Bu)) ||   // one of 0x20?? for a quick reject and then 
                        (((unsigned)c - 0x202Au) <= (0x202Eu - 0x202Au)) ||   // if so checks to see if it's one that we 
                        (((unsigned)c - 0x2060u) <= (0x2063u - 0x2060u)) ||   // are interested in.
                        (c == 0x2028)))                                       //

                    || ((c == 0x00AD) || (c == 0x034F) || (c == 0xFEFF)));    // This section does direct comparisons to a few Unicode values.
}



inline bool EA::Text::IsCharCategory(Char c, int nCharCategoryFlags)
{
    const int flags = (1 << GetCharCategory(c));
    return ((flags & nCharCategoryFlags) != 0);
}


inline bool EA::Text::IsCharJoinControl(Char c)
{
    // There are only two join control chars: ZWNJ (0x200C) and ZWJ (0x200D).
    return ((c - 0x200Cu) <= (0x200Du - 0x200Cu)); 
}


inline bool EA::Text::IsCharUppercase(Char c)
{
    return IsCharCategory(c, kCCFlagUppercaseLetter | kCCFlagTitlecaseLetter);
            
    // Also, strictly speaking, 0x2160 - 0x216F (Roman numerals) and
    // 0x24B6 - 0x24CF (Latin letters with circles around them) are upper case.
    // We'll ignore them for now.
    // 2160..216F ; Other_Uppercase # Nl  [16] ROMAN NUMERAL ONE..ROMAN NUMERAL ONE THOUSAND
    // 24B6..24CF ; Other_Uppercase # So  [26] CIRCLED LATIN CAPITAL LETTER A..CIRCLED LATIN CAPITAL LETTER Z
}


#endif // EATEXT_EATEXTUNICODE_H











