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
// EATextBidi.cpp
//
// Written and maintained by Paul Pedriana.
//
///////////////////////////////////////////////////////////////////////////////

///////////////////////////////////////////////////////////////////////////////
// This file implements the Unicode 4.0 Bidirectional Algorithm for EAText as 
// per the Unicode Standard and Unicode Standard Annex #9. Some of the 
// implementation is based on the Unicode standard Bidirectional Algorithm
// reference by Asmus Freytag. 
///////////////////////////////////////////////////////////////////////////////


#include <EAText/internal/EATextBidi.h>
#include <EAText/EATextUnicode.h>
#include <EAText/EATextLayout.h>
#include <EAText/EATextTypesetter.h>
#include EA_ASSERT_HEADER



// BidiClass abbreviations
// This allow us to use the BidiClass enumerated values with
// more consise text. We'll use these in some of the code 
// below but not all.
#define N    kBidiClassON
#define ON   kBidiClassON
#define L    kBidiClassL
#define R    kBidiClassR
#define AN   kBidiClassAN
#define EN   kBidiClassEN
#define AL   kBidiClassAL   
#define NSM  kBidiClassNSM
#define CS   kBidiClassCS
#define ES   kBidiClassES
#define ET   kBidiClassET
#define BN   kBidiClassBN
#define S    kBidiClassS
#define WS   kBidiClassWS
#define B    kBidiClassB
#define RLO  kBidiClassRLO
#define RLE  kBidiClassRLE
#define LRO  kBidiClassLRO
#define LRE  kBidiClassLRE
#define PDF  kBidiClassPDF


namespace EA
{

namespace Text
{

namespace
{

    inline int GetGreaterEven(int x)
    {
        return (x + 2) & -2;            // 0 -> 2, 1 -> 2, 2 -> 4, 3 -> 4, 4 -> 6, 5 -> 6
    }


    inline int GetGreaterOdd(int x)
    {
        return ((x + 1) & -2) + 1;      // 0 -> 1, 1 -> 3, 2 -> 3, 3 -> 5, 4 -> 5, 5 -> 7
    }


    inline BidiClass GetEmbeddingDirection(int nEmbeddingLevel)
    {
        EA_COMPILETIME_ASSERT((kBidiClassL == 1) && (kBidiClassR == 2)); 
        return (BidiClass)((nEmbeddingLevel % 2) + 1);  // 0 -> kBidiClassL, 1 -> kBidiClassR, 2 -> kBidiClassL, 3 -> kBidiClassR
    }


    inline void SetBidiClass(AnalysisInfo* pAI, size_t count, BidiClass newBidiClass)
    {
        for(const AnalysisInfo* const pAIEnd = (pAI + count); pAI != pAIEnd; ++pAI)
            pAI->mBidiClass = newBidiClass;
    }



} // namespace




///////////////////////////////////////////////////////////////////////////////
// BidiClassify
//
// Returns true if there are any RTL characters present (i.e. R or AL).
//
bool BidiClassify(const Char* pText, AnalysisInfo* pAnalysisInfoArray, size_t count)
{
    bool bRTLFound = false;

    // Compile-time assert that somebody hasn't messed with the kBidiClass ordering.
    EA_COMPILETIME_ASSERT((N   ==  0) && (ON  ==  0) && (L   ==  1) && (R   ==  2) && (AN  ==  3) && 
                          (EN  ==  4) && (AL  ==  5) && (NSM ==  6) && (CS  ==  7) && (ES  ==  8) && 
                          (ET  ==  9) && (BN  == 10) && (S   == 11) && (WS  == 12) && (B   == 13) && 
                          (RLO == 14) && (RLE == 15) && (LRO == 16) && (LRE == 17) && (PDF == 18));

    for(const Char* const pTextEnd(pText + count); pText != pTextEnd; ++pText, ++pAnalysisInfoArray)
    {
        const BidiClass bc = GetBidiClass(*pText);

        pAnalysisInfoArray->mBidiClass = bc;

        if((bc == kBidiClassR) || (bc == kBidiClassAL)) // Should we also include: kBidiClassRLO, kBidiClassRLE?
            bRTLFound = true;

        // What we do here is convert all neutral types (B, S, WS, ON) to kBidiClassON (other neutrals).
        // This simplifies our bidi processing a little bit, as it turns out that the bidi 
        // algorithm defines various types of neutral characters but doesn't do anything 
        // different depending on them.

        if((unsigned)(bc - kBidiClassS) <= (unsigned)(kBidiClassB - kBidiClassS))
            pAnalysisInfoArray->mBidiClass = kBidiClassON;
    }

    return bRTLFound;
}


///////////////////////////////////////////////////////////////////////////////
// BidiResolveExplicitImpl
//
// directionOverride is one of kBidiClassL, kBidiClassR, or 
// kBidiClassON (left, right, or neutral).
//
// nRecursionCount is the recursion count for this function.
// You should set it to zero when calling this function.
// Recursion count is not the same thing as embedding level,
// though they are related.
//
// To consider: Make this function iterative instead of recursive.
// The version we implement here closely parallels the Unicode standard
// Bidi reference implementation, at least for the time being.
//
size_t BidiResolveExplicitImpl(int nEmbeddingLevel, AnalysisInfo* pAnalysisInfoArray, size_t count,  
                                BidiClass directionOverride, int nRecursionCount)
{
    EA_ASSERT((nEmbeddingLevel >= 0) && (nEmbeddingLevel < kBidiLevelCount));
    EA_ASSERT((nRecursionCount >= 0) && (nRecursionCount < kBidiLevelCount));

    size_t i;
    const int nRecursionCountSaved = nRecursionCount;

    for(i = 0; i < count; i++)
    {
        BidiClass bidiClass = pAnalysisInfoArray[i].mBidiClass;

        switch(bidiClass)
        {
            case kBidiClassLRO: // Left to Right Override
            case kBidiClassLRE: // Left to Right Embedding
            {
                // "X3. With each LRE, compute the least greater even embedding level."
                nRecursionCount++;

                const int nGreaterEven = GetGreaterEven(nEmbeddingLevel);

                if(nGreaterEven <= kBidiLevelMax)    
                {
                    pAnalysisInfoArray[i].mnBidiLevel = nGreaterEven;
                    pAnalysisInfoArray[i].mBidiClass  = kBidiClassBN; // 5.2. Retaining Format Codes

                    const BidiClass newDirectionOverride = (bidiClass == kBidiClassLRE ? kBidiClassON : kBidiClassL);

                    i += BidiResolveExplicitImpl(nGreaterEven, pAnalysisInfoArray + (i + 1), count - (i + 1), newDirectionOverride, nRecursionCount);

                    nRecursionCount--;
                    continue;
                }

                bidiClass = pAnalysisInfoArray[i].mBidiClass = kBidiClassBN; // 5.2. Retaining Format Codes
                break;
            }

            case kBidiClassRLO: // Right to Left Override
            case kBidiClassRLE: // Right to Left Embedding
            {
                // "X2. With each RLE, compute the least greater odd embedding level."
                nRecursionCount++;

                const int nGreaterOdd = GetGreaterOdd(nEmbeddingLevel);

                if(nGreaterOdd <= kBidiLevelMax)  
                {
                    pAnalysisInfoArray[i].mnBidiLevel = nGreaterOdd;
                    pAnalysisInfoArray[i].mBidiClass  = kBidiClassBN; // 5.2. Retaining Format Codes

                    const BidiClass newDirectionOverride = (bidiClass == kBidiClassRLE ? kBidiClassON : kBidiClassR);

                    i += BidiResolveExplicitImpl(nGreaterOdd, pAnalysisInfoArray + (i + 1), count - (i + 1), newDirectionOverride, nRecursionCount);

                    nRecursionCount--;
                    continue;
                }

                bidiClass = pAnalysisInfoArray[i].mBidiClass = kBidiClassBN; // 5.2. Retaining Format Codes
                break;
            }

            case kBidiClassPDF: // Pop Directional Format
            {
                // X7. With each PDF, determine the matching embedding or override code. 
                //     If there was a valid matching code, restore (pop) the last 
                //     remembered (pushed) embedding level and directional override.

                bidiClass = pAnalysisInfoArray[i].mBidiClass = kBidiClassBN;

                if(nRecursionCount > 0) // If there is anything to pop...
                {
                    if(nRecursionCountSaved < nRecursionCount) // If we were able to implement the recursion that PDF is popping...
                        nRecursionCount--;
                    else // Else we were at the max recursion count and we stop processing after this character.
                        count = i;
                }
            }

            case kBidiClassB:
            case kBidiClassWS:
            case kBidiClassS:
            case kBidiClassBN:
            case kBidiClassET:
            case kBidiClassES:
            case kBidiClassCS:
            case kBidiClassNSM:
            case kBidiClassAL:
            case kBidiClassEN:
            case kBidiClassAN:
            case kBidiClassR:
            case kBidiClassL:
            case kBidiClassON:
                break;
        }

        // X6.a For all types besides RLE, LRE, RLO, LRO, and PDF: 
        //      Set the level of the current character to the current embedding level.
        pAnalysisInfoArray[i].mnBidiLevel = nEmbeddingLevel;

        // X6.b. For all types besides RLE, LRE, RLO, LRO, and PDF: 
        //       Whenever the directional override status is not neutral, 
        //       reset the current character type to the directional override status.
        if(directionOverride != kBidiClassON)
            bidiClass = directionOverride;

        if(pAnalysisInfoArray[i].mBidiClass != kBidiClassBN)
            pAnalysisInfoArray[i].mBidiClass = bidiClass;
    }

    // X8. All explicit directional embeddings and overrides are completely 
    //     terminated at the end of each paragraph. Paragraph separators are 
    //     not included in the embedding.

    // Note by Paul Pedriana: It would be hard to implement rule X8 here, as it is
    // our goal to be able to iteratively process a paragraph of text. We may need 
    // to inspect the paragraph after it is done to see if we need to append PDF
    // characters to it to match RLE/LRE/RLO/LRO characters.


    // X9. Remove all RLE, LRE, RLO, LRO, PDF, and kBidiClassBN codes.
    //     Note that an implementation does not have to actually remove 
    //     the codes, it just has to behave as though the codes were not 
    //     present for the remainder of the algorithm. Conformance does 
    //     not require any particular placement of these codes as long as 
    //     all other characters are ordered correctly.

    // Note by Paul Pedriana: X9 doesn't really need to be implemented. All that really
    // matters is that we leave these characters as described in section 5.2 of UAX9.


    // X10. The remaining rules are applied to each run of characters at the same level. 
    //      For each run, determine the start-of-level-run (sor) and end-of-level-run 
    //      (eor) type, either L or R. This depends on the higher of the two levels on 
    //      either side of the boundary (at the start or end of the paragraph, the level 
    //      of the 'other' run is the base embedding level). If the higher level is odd, 
    //      the type is R, otherwise it is L.

    // Note by Paul Pedriana: We don't so much need to implement this as we need to make
    // sure we understand the boundaries when using BidiResolveWeak() (the next step after
    // this function).

    return i;
}


///////////////////////////////////////////////////////////////////////////////
// BidiResolveExplicit
//
void BidiResolveExplicit(int nBaseEmbeddingLevel, AnalysisInfo* pAnalysisInfoArray, size_t count)
{
    // We merely call BidiResolveExplicitImpl with two extra parameters: (kBidiClassON, 0).
    BidiResolveExplicitImpl(nBaseEmbeddingLevel, pAnalysisInfoArray, count, kBidiClassON, 0);
}




namespace 
{

    ///////////////////////////////////////////////////////////////////////////////
    // WeakState
    //
    enum WeakState
    {
        xa,        //  arabic letter            
        xr,        //  right leter              
        xl,        //  left letter              
                                               
        ao,        //  arabic lett. foll by ON  
        ro,        //  right lett. foll by ON      
        lo,        //  left lett. foll by ON      
                                               
        rt,        //  ET following R          
        lt,        //  ET following L          
                                               
        cn,        //  EN, AN following AL      
        ra,        //  arabic number foll R    
        re,        //  european number foll R  
        la,        //  arabic number foll L    
        le,        //  european number foll L  
                                               
        ac,        //  CS following cn          
        rc,        //  CS following ra          
        rs,        //  CS,ES following re       
        lc,        //  CS following la          
        ls,        //  CS,ES following le          

        ret,    //  ET following re
        let,    //  ET following le
    };


    ///////////////////////////////////////////////////////////////////////////////
    // gWeakStateTable
    //
    static const WeakState gWeakStateTable[20][10] =
    {
              //  N,  L,  R  AN, EN, AL,NSM, CS, ES, ET,  
        /*xa*/ { ao, xl, xr, cn, cn, xa, xa, ao, ao, ao },   // arabic letter
        /*xr*/ { ro, xl, xr, ra, re, xa, xr, ro, ro, rt },   // right leter
        /*xl*/ { lo, xl, xr, la, le, xa, xl, lo, lo, lt },   // left letter
                                          
        /*ao*/ { ao, xl, xr, cn, cn, xa, ao, ao, ao, ao },   // arabic lett. foll by ON
        /*ro*/ { ro, xl, xr, ra, re, xa, ro, ro, ro, rt },   // right lett. foll by ON
        /*lo*/ { lo, xl, xr, la, le, xa, lo, lo, lo, lt },   // left lett. foll by ON
                                          
        /*rt*/ { ro, xl, xr, ra, re, xa, rt, ro, ro, rt },   // ET following R
        /*lt*/ { lo, xl, xr, la, le, xa, lt, lo, lo, lt },   // ET following L
                                          
        /*cn*/ { ao, xl, xr, cn, cn, xa, cn, ac, ao, ao },   // EN, AN following AL
        /*ra*/ { ro, xl, xr, ra, re, xa, ra, rc, ro, rt },   // arabic number foll R
        /*re*/ { ro, xl, xr, ra, re, xa, re, rs, rs,ret },   // european number foll R
        /*la*/ { lo, xl, xr, la, le, xa, la, lc, lo, lt },   // arabic number foll L
        /*le*/ { lo, xl, xr, la, le, xa, le, ls, ls,let },   // european number foll L
                                          
        /*ac*/ { ao, xl, xr, cn, cn, xa, ao, ao, ao, ao },   // CS following cn
        /*rc*/ { ro, xl, xr, ra, re, xa, ro, ro, ro, rt },   // CS following ra
        /*rs*/ { ro, xl, xr, ra, re, xa, ro, ro, ro, rt },   // CS,ES following re
        /*lc*/ { lo, xl, xr, la, le, xa, lo, lo, lo, lt },   // CS following la
        /*ls*/ { lo, xl, xr, la, le, xa, lo, lo, lo, lt },   // CS,ES following le

       /*ret*/ { ro, xl, xr, ra, re, xa,ret, ro, ro,ret },  // ET following re
       /*let*/ { lo, xl, xr, la, le, xa,let, lo, lo,let }   // ET following le
    };                                              


    ///////////////////////////////////////////////////////////////////////////////
    // WeakStateAction
    //
    enum WeakStateAction
    {
        // Primitives
        IX  = 0x0100,               // Increment
        XX  = 0x000F,               // No-op

        // Actions
        xxx = (XX << 4) + XX,       // No-op
        xIx = IX + xxx,             // Increment run
        xxN = (XX << 4) + ON,       // Set current to N
        xxE = (XX << 4) + EN,       // Set current to EN
        xxA = (XX << 4) + AN,       // Set current to AN
        xxR = (XX << 4) + R,        // Set current to R
        xxL = (XX << 4) + L,        // Set current to L
        Nxx = (ON << 4) + 0xF,      // Set run to neutral
        Axx = (AN << 4) + 0xF,      // Set run to AN
        ExE = (EN << 4) + EN,       // Set run to EN, set current to EN
        NIx = (ON << 4) + 0xF + IX, // Set run to N, increment
        NxN = (ON << 4) + ON,       // Set run to N, set current to N
        NxR = (ON << 4) + R,        // Set run to N, set current to R
        NxE = (ON << 4) + EN,       // Set run to N, set current to EN

        AxA = (AN << 4) + AN,       // Set run to AN, set current to AN
        NxL = (ON << 4) + L,        // Set run to N, set current to L
        LxL = ( L << 4) + L,        // Set run to L, set current to L
    };


    ///////////////////////////////////////////////////////////////////////////////
    // gWeakStateActionTable
    //
    const WeakStateAction gWeakStateActionTable[20][11] =                          
    {                                               
               //  N,   L,   R   AN,  EN,  AL, NSM,  CS,  ES,  ET,  BN
        /*xa*/ { xxx, xxx, xxx, xxx, xxA, xxR, xxR, xxN, xxN, xxN, xxx },  // arabic letter
        /*xr*/ { xxx, xxx, xxx, xxx, xxE, xxR, xxR, xxN, xxN, xIx, xxx },  // right leter
        /*xl*/ { xxx, xxx, xxx, xxx, xxL, xxR, xxL, xxN, xxN, xIx, xxx },  // left letter

        /*ao*/ { xxx, xxx, xxx, xxx, xxA, xxR, xxN, xxN, xxN, xxN, xxx },  // arabic lett. foll by ON
        /*ro*/ { xxx, xxx, xxx, xxx, xxE, xxR, xxN, xxN, xxN, xIx, xxx },  // right lett. foll by ON
        /*lo*/ { xxx, xxx, xxx, xxx, xxL, xxR, xxN, xxN, xxN, xIx, xxx },  // left lett. foll by ON

        /*rt*/ { Nxx, Nxx, Nxx, Nxx, ExE, NxR, xIx, NxN, NxN, xIx, xxx },  // ET following R
        /*lt*/ { Nxx, Nxx, Nxx, Nxx, LxL, NxR, xIx, NxN, NxN, xIx, xxx },  // ET following L

        /*cn*/ { xxx, xxx, xxx, xxx, xxA, xxR, xxA, xIx, xxN, xxN, xxx },  // EN, AN following  AL
        /*ra*/ { xxx, xxx, xxx, xxx, xxE, xxR, xxA, xIx, xxN, xIx, xxx },  // arabic number foll R
        /*re*/ { xxx, xxx, xxx, xxx, xxE, xxR, xxE, xIx, xIx, xxE, xxx },  // european number foll R
        /*la*/ { xxx, xxx, xxx, xxx, xxL, xxR, xxA, xIx, xxN, xIx, xxx },  // arabic number foll L
        /*le*/ { xxx, xxx, xxx, xxx, xxL, xxR, xxL, xIx, xIx, xxL, xxx },  // european number foll L

        /*ac*/ { Nxx, Nxx, Nxx, Axx, AxA, NxR, NxN, NxN, NxN, NxN, xxx },  // CS following cn
        /*rc*/ { Nxx, Nxx, Nxx, Axx, NxE, NxR, NxN, NxN, NxN, NIx, xxx },  // CS following ra
        /*rs*/ { Nxx, Nxx, Nxx, Nxx, ExE, NxR, NxN, NxN, NxN, NIx, xxx },  // CS,ES following re
        /*lc*/ { Nxx, Nxx, Nxx, Axx, NxL, NxR, NxN, NxN, NxN, NIx, xxx },  // CS following la
        /*ls*/ { Nxx, Nxx, Nxx, Nxx, LxL, NxR, NxN, NxN, NxN, NIx, xxx },  // CS,ES following le

       /*ret*/ { xxx, xxx, xxx, xxx, xxE, xxR, xxE, xxN, xxN, xxE, xxx },  // ET following re
       /*let*/ { xxx, xxx, xxx, xxx, xxL, xxR, xxL, xxN, xxN, xxL, xxx }   // ET following le
    };


    inline BidiClass GetResolvedType(WeakStateAction weakStateAction)
    {
        return (BidiClass)(weakStateAction & 0xf);
    }


    inline BidiClass GetDeferredType(WeakStateAction weakStateAction)
    {
        return (BidiClass)((weakStateAction >> 4) & 0xf);
    }

} // namespace


///////////////////////////////////////////////////////////////////////////////
// BidiResolveWeak
//
void BidiResolveWeak(int nBaseEmbeddingLevel, AnalysisInfo* pAnalysisInfoArray, size_t count)
{
    // Compile-time assert that somebody hasn't messed with the kBidiClass ordering.
    EA_COMPILETIME_ASSERT((N   ==  0) && (ON  ==  0) && (L   ==  1) && (R   ==  2) && (AN  ==  3) && 
                          (EN  ==  4) && (AL  ==  5) && (NSM ==  6) && (CS  ==  7) && (ES  ==  8) && 
                          (ET  ==  9) && (BN  == 10) && (S   == 11) && (WS  == 12) && (B   == 13) && 
                          (RLO == 14) && (RLE == 15) && (LRO == 16) && (LRE == 17) && (PDF == 18));

    int       level = nBaseEmbeddingLevel;
    WeakState weakState = (nBaseEmbeddingLevel & 1) ? xr : xl;
    size_t    nRunLength = 0;
    BidiClass bidiClass;
    size_t    i;

    for(i = 0; i < count; i++)
    {
        // We expect mBidiClass to be one of: ON, L, R, AN, EN, AL, NSM, CS, ES, ET, BN.
        EA_ASSERT(pAnalysisInfoArray[i].mBidiClass <= kBidiClassBN); 

        // Ignore boundary neutrals
        if(pAnalysisInfoArray[i].mBidiClass == kBidiClassBN)
        {
            // must flatten levels unless at a level change;
            pAnalysisInfoArray[i].mnBidiLevel = level;

            // Look ahead for level changes.
            if(((i + 1) == count) && (level != nBaseEmbeddingLevel))
            {
                // Need to fixup last kBidiClassBN before end of the loop, 
                // since its fix-upped value will be needed below the assert.
                pAnalysisInfoArray[i].mBidiClass = GetEmbeddingDirection(level);
            } 
            else if(((i + 1) < count) && (level != pAnalysisInfoArray[i + 1].mnBidiLevel) && 
                     (pAnalysisInfoArray[i + 1].mBidiClass != kBidiClassBN))
            {
                // Fixup the last kBidiClassBN in front / after a level 
                // run to make it act like the SOR/EOR in rule X10.
                int newLevel = pAnalysisInfoArray[i + 1].mnBidiLevel;

                if(level > newLevel)
                    newLevel = level;

                pAnalysisInfoArray[i].mnBidiLevel = newLevel;

                // Must match assigned level.
                pAnalysisInfoArray[i].mBidiClass = GetEmbeddingDirection(newLevel);
                level = pAnalysisInfoArray[i + 1].mnBidiLevel;
            }
            else
            {
                // Don't interrupt runs
                if(nRunLength) 
                    nRunLength++;

                continue;
            }
        }

        // We expect mBidiClass to be one of: ON, L, R, AN, EN, AL, NSM, CS, ES, ET, BN.
        EA_ASSERT(pAnalysisInfoArray[i].mBidiClass <= kBidiClassBN); 
        bidiClass = pAnalysisInfoArray[i].mBidiClass;

        // Set the directionality for deferred runs.
        const WeakStateAction weakStateAction = gWeakStateActionTable[weakState][bidiClass];
        const BidiClass       clsRun          = GetDeferredType(weakStateAction);

        if((uint32_t)clsRun != XX)
        {
            SetBidiClass(pAnalysisInfoArray + (i - nRunLength), nRunLength, clsRun);
            nRunLength = 0;
        }

        // Resolve the directionality class at the current location
        BidiClass clsNew = GetResolvedType(weakStateAction);

        if((uint32_t)clsNew != XX)
            pAnalysisInfoArray[i].mBidiClass = clsNew;

        // Increment a deferred run.
        if(IX & weakStateAction)
            nRunLength++;

        weakState = gWeakStateTable[weakState][bidiClass];
    }
    
    // Resolve any deferred runs. Use the direction of the current 
    // level to emulate PDF (pop directional format).
    bidiClass = GetEmbeddingDirection(level);

    // Resolve the directionality for deferred runs.
    const WeakStateAction weakStateAction = gWeakStateActionTable[weakState][bidiClass];
    const BidiClass       clsRun          = GetDeferredType(weakStateAction);

    if((uint32_t)clsRun != XX) 
        SetBidiClass(pAnalysisInfoArray + (i - nRunLength), nRunLength, clsRun);
}



namespace
{
    enum NeutralState
    {
        r,      // R and characters resolved to R
        l,      // L and characters resolved to L
        rn,     // N preceded by right
        ln,     // N preceded by left
        a,      // AN preceded by left (the abbrev 'la' is used up above)
        na,     // N preceeded by a
    };


    enum NeutralStateAction
    {
        ns0 =  0,           // Placeholder for unused actions.
        nL  =  L,           // Resolve EN to L
        En  =  3 << 4,      // Resolve neutrals run to embedding level direction
        Rn  =  R << 4,      // Resolve neutrals run to strong right
        Ln  =  L << 4,      // Resolved neutrals run to strong left
        In  = (1 << 8),     // Increment count of deferred neutrals
        LnL = (1 << 4) + L, // Set run and EN to L
    };


    const NeutralState gNeutralStateTable[6][5] =
    {
        //  N, L,  R,  AN, EN = bidiClass
                               // state =   
        { rn, l,  r,  r,  r }, // r   right                                        
        { ln, l,  r,  a,  l }, // l   left                                      
                                                                              
        { rn, l,  r,  r,  r }, // rn  N preceded by right                       
        { ln, l,  r,  a,  l }, // ln  N preceded by left                        
                                                                            
        { na, l,  r,  a,  l }, // a  AN preceded by left                       
        { na, l,  r,  a,  l }  // na  N preceded by la                      
    };


    // By rule W7, whenever a EN is dominated by an L (including start 
    // of run with embedding direction = L) it is resolved to, and further  
    // treated as L. This leads to the need for 'a' and 'na' states.

    const NeutralStateAction gNeutralStateActionTable[6][5] =                   
    {
      // N,   L,   R,  AN,  EN = bidiClass
                                     // state =                      
        { In, ns0, ns0, ns0, ns0 },  // r    right                               
        { In, ns0, ns0, ns0,  nL },  // l    left
                   
        { In,  En,  Rn,  Rn,  Rn },  // rn   N preceded by right                           
        { In,  Ln,  En,  En, LnL },  // ln   N preceded by left                            
                                             
        { In, ns0, ns0,  ns0, nL },  // a   AN preceded by left
        { In,  En,  Rn,   Rn, En }   // na   N  preceded by a     
    };


    BidiClass GetDeferredNeutrals(NeutralStateAction action, int level)
    {
        const BidiClass bidiClass = (BidiClass)((action >> 4) & 0xf);

        if(bidiClass == (En >> 4))
            return GetEmbeddingDirection(level);

        return bidiClass;
    }


    BidiClass GetResolvedNeutrals(NeutralStateAction action)
    {
        const BidiClass bidiClass = (BidiClass)(action & 0xf);

        // It's mathematically impossible for the following to happen, since In is greater than 0xf.
        //if((uint32_t)bidiClass == In)
        //    return kBidiClassON;

        return bidiClass;
    }


} // namespace



void BidiResolveNeutral(int nBaseEmbeddingLevel, AnalysisInfo* pAnalysisInfoArray, size_t count)
{   
    int          level = nBaseEmbeddingLevel;
    NeutralState neutralState = (nBaseEmbeddingLevel & 1) ? r : l;
    size_t       nRunLength = 0;
    BidiClass    bidiClass;
    size_t       i;

    for(i = 0; i < count; i++)
    {
        // Ignore boundary neutrals.
        if(pAnalysisInfoArray[i].mBidiClass == BN)
        {
            // Include in the count for a deferred run.
            if(nRunLength)
                nRunLength++;

            // Skip any further processing.
            continue;
        }

        EA_ASSERT(pAnalysisInfoArray[i].mBidiClass <= kBidiClassEN); // Only ON, L, R, AN, EN are allowed.
        bidiClass = pAnalysisInfoArray[i].mBidiClass;

        const NeutralStateAction neutralStateAction = gNeutralStateActionTable[neutralState][bidiClass];

        // resolve the directionality for deferred runs
        const BidiClass clsRun = GetDeferredNeutrals(neutralStateAction, level);

        if(clsRun != N)
        {
            SetBidiClass(pAnalysisInfoArray + (i - nRunLength), nRunLength, clsRun);
            nRunLength = 0;
        }

        // Resolve the directionality class at the current location.
        const BidiClass clsNew = GetResolvedNeutrals(neutralStateAction);

        if(clsNew != N)
            pAnalysisInfoArray[i].mBidiClass = clsNew;

        if(In & neutralStateAction)
            nRunLength++;

        neutralState = gNeutralStateTable[neutralState][bidiClass];
        level = pAnalysisInfoArray[i].mnBidiLevel;
    }
    
    // Resolve any deferred runs
    bidiClass = GetEmbeddingDirection(level); // eor has type of current level

    // Resolve the directionality for deferred runs.
    const BidiClass clsRun = GetDeferredNeutrals(gNeutralStateActionTable[neutralState][bidiClass], level);

    if(clsRun != N) 
        SetBidiClass(pAnalysisInfoArray + (i - nRunLength), nRunLength, clsRun);
}







namespace
{
    const int gAddLevelTable[2][4] =
    {
       // L,  R, AN, EN = BidiClass 
        { 0,  1,  2,  2 }, // level == even
        { 1,  0,  1,  1 }  // level == odd
    };
}


///////////////////////////////////////////////////////////////////////////////
// BidiResolveImplicit
//
void BidiResolveImplicit(AnalysisInfo* pAnalysisInfoArray, size_t count)
{
    for(size_t i = 0; i < count; i++)
    {
        // We cannot resolve kBidiClassBN here, since some kBidiClassBN chars
        // were resolved to strong types in BidiResolveWeak. To remove these 
        // we need the original BidiClass types for the chars.

        if(pAnalysisInfoArray[i].mBidiClass != kBidiClassBN)
        {
            EA_ASSERT((pAnalysisInfoArray[i].mBidiClass > 0) && (pAnalysisInfoArray[i].mBidiClass < 5));

            const int levelAddition = gAddLevelTable[pAnalysisInfoArray[i].mnBidiLevel & 1][pAnalysisInfoArray[i].mBidiClass - 1];

            pAnalysisInfoArray[i].mnBidiLevel += levelAddition;
        }
    }
}


///////////////////////////////////////////////////////////////////////////////
// Bidi
//
void Bidi(int nBaseEmbeddingLevel, const Char* pText, AnalysisInfo* pAnalysisInfoArray, size_t count)
{
    #if EATEXT_BIDI_SUPPORTED
        const bool bRTLPresent = BidiClassify(pText, pAnalysisInfoArray, count);

        if(bRTLPresent) // For all but Hebrew and Arabic script, bRTLPresent will almost always be false.
        {
            BidiResolveExplicit(nBaseEmbeddingLevel, pAnalysisInfoArray, count);
            BidiResolveWeak    (nBaseEmbeddingLevel, pAnalysisInfoArray, count);
            BidiResolveNeutral (nBaseEmbeddingLevel, pAnalysisInfoArray, count);
            BidiResolveImplicit(                     pAnalysisInfoArray, count);
        }
    #else
        (void)nBaseEmbeddingLevel; (void)pText;

        for(const AnalysisInfo* const pEnd = pAnalysisInfoArray + count; pAnalysisInfoArray < pEnd; ++pAnalysisInfoArray)
        {
            pAnalysisInfoArray->mBidiClass  = kBidiClassL;
            pAnalysisInfoArray->mnBidiLevel = 0;
        }
    #endif
}



} // namespace Text

} // namespace EA




// Undefine the above definitions
#undef N
#undef ON
#undef L  
#undef R  
#undef AN 
#undef EN 
#undef AL 
#undef NSM
#undef CS 
#undef ES 
#undef ET 
#undef BN 
#undef S  
#undef WS 
#undef B  
#undef RLO  
#undef RLE  
#undef LRO
#undef LRE
#undef PDF






















