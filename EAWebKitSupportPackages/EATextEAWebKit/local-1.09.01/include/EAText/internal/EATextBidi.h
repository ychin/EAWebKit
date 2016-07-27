/*
Copyright (C) 2006, 2009, 2010, 2012 Electronic Arts, Inc.  All rights reserved.

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
// EATextBidi.h
//
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


        /// kBidiLevelCount
        ///
        /// The count of bidi levels we support. The primary reason we support
        /// only 16 levels instead of the 61 levels indicated by the Unicode
        /// standard is that 61 would use more memory and that 16 is well beyond
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


    } // namespace Text

} // namespace EA





///////////////////////////////////////////////////////////////////////////////
// Inline functions
///////////////////////////////////////////////////////////////////////////////


#endif // Header include guard













