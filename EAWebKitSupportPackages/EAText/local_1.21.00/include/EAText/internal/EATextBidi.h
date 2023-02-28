///////////////////////////////////////////////////////////////////////////////
// EATextBidi.h
//
// Copyright (c) 2006 Electronic Arts. All Rights Reserved.
// Written and maintained by Paul Pedriana.
//
///////////////////////////////////////////////////////////////////////////////

///////////////////////////////////////////////////////////////////////////////
// This file implements the Unicode 4.0 Bidirectional algorithm for EAText as 
// per the Unicode Standard and Unicode Standard Annex #9. 
//
///////////////////////////////////////////////////////////////////////////////


#ifndef EATEXT_INTERNAL_EATEXTBIDI_H
#define EATEXT_INTERNAL_EATEXTBIDI_H


#include <EABase/eabase.h>
#include <EAText/EAText.h>


namespace EA
{
	namespace Text
	{
		// Forward declarations
		struct AnalysisInfo;

		enum BidiPairedBracketType
		{
			kBPBTNone = 0,      /// A character which has no matching character to make a bracket pair.
			kBPBTOpen,          /// A character which represents the beginning of a bracket pair ( e.g. Left parenthesis '(' )
			kBPBTClose,         /// A character which represents the ending of a bracket pair ( e.g. Left parenthesis ')' )
		};

		/// kBidiLevelCount
		///
		/// The count of bidi levels we support. The primary reason we support
		/// only 16 levels instead of the 125 levels indicated by the Unicode
		/// standard is that 125 would use more memory and that 16 is well beyond
		/// the maximum practical level count (~4) in real word text. 
		///
		const int kBidiLevelCount = 16;


		/// kBidiLevelMax
		const int kBidiLevelMax = (kBidiLevelCount - 1);


		/// BidiClassify
		///
		/// Sets the BidiClass values in the AnalysisInfo based on the input pText.
		/// Returns true if there are any RTL characters present (i.e. R or AL).
		///
		bool BidiClassify(const Char* pText, AnalysisInfo* pAnalysisInfoArray, size_t count);


		/// BidiResolveExplicit
		///
		/// Resolves explicit embedding levels and bidirectional overrides.
		/// Implements Unicode Bidirectional Algorithm UAX 9 rules X1 - X10.
		///
		/// Returns the number of characters processed.
		///
		/// embeddingLevel is an input parameter which is an integer in 
		/// the range of [0, kBidiLevelMax].
		///
		/// This function expects to have a pAnalysisInfo Array which contains
		/// BidiClass values for each character. This function modifies the 
		/// array in-place.
		///
		void BidiResolveExplicit(int nBaseEmbeddingLevel, AnalysisInfo* pAnalysisInfoArray, size_t count);


		/// BidiResolveWeak
		///
		/// Implements rules W1 - W7 of UAX 9 (Unicode Bidirectional Algorithm).
		///
		/// This function is called after BidiResolveExplicit.
		/// This function modifies the array in-place.
		///
		/// nBaseEmbeddingLevel is initial embedding level for the paragraph.
		/// It is set to 0 for LTR scripts such as English and 1 for RTL scripts
		/// such as Hebrew. 
		///
		void BidiResolveWeak(int nBaseEmbeddingLevel, AnalysisInfo* pAnalysisInfoArray, size_t count);


		/// BidiResolveNeutral
		///
		/// Implements rules W7, N1, and N2 of UAX 9 (Unicode Bidirectional Algorithm).
		///
		/// This function is called after BidiResolveNeutral.
		/// This function modifies the array in-place.
		///
		void BidiResolveNeutral(int nBaseEmbeddingLevel, AnalysisInfo* pAnalysisInfoArray, size_t count);


		/// BidiResolveImplicit
		///
		/// Implements rules I1 and I2 of UAX 9 (Unicode Bidirectional Algorithm).
		///
		/// This function is called after BidiResolveNeutral.
		/// This function modifies the array in-place.
		///
		void BidiResolveImplicit(AnalysisInfo* pAnalysisInfoArray, size_t count);


		/// Bidi
		///
		/// High level fucntion that implements the Bidirectional Algorithm.
		/// In many cases, the user need only call this function and let it call the 
		/// Bidi subfunctions.
		///
		/// This function calls the following sub-functions:
		///     BidiClassify
		///     BidiResolveExplicit
		///     BidiResolveWeak
		///     BidiResolveNeutral
		///     BidiResolveImplicit
		///
		/// The ResolveWhitespace function is not called, as that needs to be done after
		/// Bidi, shaping, and line breaking.
		///    
		///
		void Bidi(int nBaseEmbeddingLevel, const Char* pText, AnalysisInfo* pAnalysisInfoArray, size_t count);

		/// GetBidiPairedBracketType
		///
		/// Determines whether the given character is of open, close or non-bracket type
		///
		BidiPairedBracketType GetBidiPairedBracketType(Char c);

	} // namespace Text

} // namespace EA





///////////////////////////////////////////////////////////////////////////////
// Inline functions
///////////////////////////////////////////////////////////////////////////////


#endif // Header include guard













