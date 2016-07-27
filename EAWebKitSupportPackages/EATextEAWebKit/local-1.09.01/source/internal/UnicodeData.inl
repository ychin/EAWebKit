/*
Copyright (C) 2009, 2012 Electronic Arts, Inc.  All rights reserved.

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

// UnicodeData.inl


namespace EA
{

namespace Text
{


///////////////////////////////////////////////////////////////////////////////
// MirrorPair
//
struct MirrorPair
{
    Char mFirst;
    Char mSecond;
};


#if (EATEXT_MIRROR_CHAR_SUPPORT >= 2)
    // This is the full Unicode 4.0 mirroring set.
    // This is only compiled if EATEXT_MIRROR_CHAR_SUPPORT is of a sufficiently high value.

    const MirrorPair gMirrorPairArray[] = 
    {
        { 0x0028, 0x0029 },   // LEFT PARENTHESIS
        { 0x0029, 0x0028 },   // RIGHT PARENTHESIS
        { 0x003C, 0x003E },   // LESS-THAN SIGN
        { 0x003E, 0x003C },   // GREATER-THAN SIGN
        { 0x005B, 0x005D },   // LEFT SQUARE BRACKET
        { 0x005D, 0x005B },   // RIGHT SQUARE BRACKET
        { 0x007B, 0x007D },   // LEFT CURLY BRACKET
        { 0x007D, 0x007B },   // RIGHT CURLY BRACKET
        { 0x00AB, 0x00BB },   // LEFT-POINTING DOUBLE ANGLE QUOTATION MARK
        { 0x00BB, 0x00AB },   // RIGHT-POINTING DOUBLE ANGLE QUOTATION MARK
        { 0x2039, 0x203A },   // SINGLE LEFT-POINTING ANGLE QUOTATION MARK
        { 0x203A, 0x2039 },   // SINGLE RIGHT-POINTING ANGLE QUOTATION MARK
        { 0x2045, 0x2046 },   // LEFT SQUARE BRACKET WITH QUILL
        { 0x2046, 0x2045 },   // RIGHT SQUARE BRACKET WITH QUILL
        { 0x207D, 0x207E },   // SUPERSCRIPT LEFT PARENTHESIS
        { 0x207E, 0x207D },   // SUPERSCRIPT RIGHT PARENTHESIS
        { 0x208D, 0x208E },   // SUBSCRIPT LEFT PARENTHESIS
        { 0x208E, 0x208D },   // SUBSCRIPT RIGHT PARENTHESIS
        { 0x2208, 0x220B },   // ELEMENT OF
        { 0x2209, 0x220C },   // NOT AN ELEMENT OF
        { 0x220A, 0x220D },   // SMALL ELEMENT OF
        { 0x220B, 0x2208 },   // CONTAINS AS MEMBER
        { 0x220C, 0x2209 },   // DOES NOT CONTAIN AS MEMBER
        { 0x220D, 0x220A },   // SMALL CONTAINS AS MEMBER
        { 0x2215, 0x29F5 },   // DIVISION SLASH
        { 0x223C, 0x223D },   // TILDE OPERATOR
        { 0x223D, 0x223C },   // REVERSED TILDE
        { 0x2243, 0x22CD },   // ASYMPTOTICALLY EQUAL TO
        { 0x2252, 0x2253 },   // APPROXIMATELY EQUAL TO OR THE IMAGE OF
        { 0x2253, 0x2252 },   // IMAGE OF OR APPROXIMATELY EQUAL TO
        { 0x2254, 0x2255 },   // COLON EQUALS
        { 0x2255, 0x2254 },   // EQUALS COLON
        { 0x2264, 0x2265 },   // LESS-THAN OR EQUAL TO
        { 0x2265, 0x2264 },   // GREATER-THAN OR EQUAL TO
        { 0x2266, 0x2267 },   // LESS-THAN OVER EQUAL TO
        { 0x2267, 0x2266 },   // GREATER-THAN OVER EQUAL TO
        { 0x2268, 0x2269 },   // [BEST FIT] LESS-THAN BUT NOT EQUAL TO
        { 0x2269, 0x2268 },   // [BEST FIT] GREATER-THAN BUT NOT EQUAL TO
        { 0x226A, 0x226B },   // MUCH LESS-THAN
        { 0x226B, 0x226A },   // MUCH GREATER-THAN
        { 0x226E, 0x226F },   // [BEST FIT] NOT LESS-THAN
        { 0x226F, 0x226E },   // [BEST FIT] NOT GREATER-THAN
        { 0x2270, 0x2271 },   // [BEST FIT] NEITHER LESS-THAN NOR EQUAL TO
        { 0x2271, 0x2270 },   // [BEST FIT] NEITHER GREATER-THAN NOR EQUAL TO
        { 0x2272, 0x2273 },   // [BEST FIT] LESS-THAN OR EQUIVALENT TO
        { 0x2273, 0x2272 },   // [BEST FIT] GREATER-THAN OR EQUIVALENT TO
        { 0x2274, 0x2275 },   // [BEST FIT] NEITHER LESS-THAN NOR EQUIVALENT TO
        { 0x2275, 0x2274 },   // [BEST FIT] NEITHER GREATER-THAN NOR EQUIVALENT TO
        { 0x2276, 0x2277 },   // LESS-THAN OR GREATER-THAN
        { 0x2277, 0x2276 },   // GREATER-THAN OR LESS-THAN
        { 0x2278, 0x2279 },   // NEITHER LESS-THAN NOR GREATER-THAN
        { 0x2279, 0x2278 },   // NEITHER GREATER-THAN NOR LESS-THAN
        { 0x227A, 0x227B },   // PRECEDES
        { 0x227B, 0x227A },   // SUCCEEDS
        { 0x227C, 0x227D },   // PRECEDES OR EQUAL TO
        { 0x227D, 0x227C },   // SUCCEEDS OR EQUAL TO
        { 0x227E, 0x227F },   // [BEST FIT] PRECEDES OR EQUIVALENT TO
        { 0x227F, 0x227E },   // [BEST FIT] SUCCEEDS OR EQUIVALENT TO
        { 0x2280, 0x2281 },   // [BEST FIT] DOES NOT PRECEDE
        { 0x2281, 0x2280 },   // [BEST FIT] DOES NOT SUCCEED
        { 0x2282, 0x2283 },   // SUBSET OF
        { 0x2283, 0x2282 },   // SUPERSET OF
        { 0x2284, 0x2285 },   // [BEST FIT] NOT A SUBSET OF
        { 0x2285, 0x2284 },   // [BEST FIT] NOT A SUPERSET OF
        { 0x2286, 0x2287 },   // SUBSET OF OR EQUAL TO
        { 0x2287, 0x2286 },   // SUPERSET OF OR EQUAL TO
        { 0x2288, 0x2289 },   // [BEST FIT] NEITHER A SUBSET OF NOR EQUAL TO
        { 0x2289, 0x2288 },   // [BEST FIT] NEITHER A SUPERSET OF NOR EQUAL TO
        { 0x228A, 0x228B },   // [BEST FIT] SUBSET OF WITH NOT EQUAL TO
        { 0x228B, 0x228A },   // [BEST FIT] SUPERSET OF WITH NOT EQUAL TO
        { 0x228F, 0x2290 },   // SQUARE IMAGE OF
        { 0x2290, 0x228F },   // SQUARE ORIGINAL OF
        { 0x2291, 0x2292 },   // SQUARE IMAGE OF OR EQUAL TO
        { 0x2292, 0x2291 },   // SQUARE ORIGINAL OF OR EQUAL TO
        { 0x2298, 0x29B8 },   // CIRCLED DIVISION SLASH
        { 0x22A2, 0x22A3 },   // RIGHT TACK
        { 0x22A3, 0x22A2 },   // LEFT TACK
        { 0x22A6, 0x2ADE },   // ASSERTION
        { 0x22A8, 0x2AE4 },   // TRUE
        { 0x22A9, 0x2AE3 },   // FORCES
        { 0x22AB, 0x2AE5 },   // DOUBLE VERTICAL BAR DOUBLE RIGHT TURNSTILE
        { 0x22B0, 0x22B1 },   // PRECEDES UNDER RELATION
        { 0x22B1, 0x22B0 },   // SUCCEEDS UNDER RELATION
        { 0x22B2, 0x22B3 },   // NORMAL SUBGROUP OF
        { 0x22B3, 0x22B2 },   // CONTAINS AS NORMAL SUBGROUP
        { 0x22B4, 0x22B5 },   // NORMAL SUBGROUP OF OR EQUAL TO
        { 0x22B5, 0x22B4 },   // CONTAINS AS NORMAL SUBGROUP OR EQUAL TO
        { 0x22B6, 0x22B7 },   // ORIGINAL OF
        { 0x22B7, 0x22B6 },   // IMAGE OF
        { 0x22C9, 0x22CA },   // LEFT NORMAL FACTOR SEMIDIRECT PRODUCT
        { 0x22CA, 0x22C9 },   // RIGHT NORMAL FACTOR SEMIDIRECT PRODUCT
        { 0x22CB, 0x22CC },   // LEFT SEMIDIRECT PRODUCT
        { 0x22CC, 0x22CB },   // RIGHT SEMIDIRECT PRODUCT
        { 0x22CD, 0x2243 },   // REVERSED TILDE EQUALS
        { 0x22D0, 0x22D1 },   // DOUBLE SUBSET
        { 0x22D1, 0x22D0 },   // DOUBLE SUPERSET
        { 0x22D6, 0x22D7 },   // LESS-THAN WITH DOT
        { 0x22D7, 0x22D6 },   // GREATER-THAN WITH DOT
        { 0x22D8, 0x22D9 },   // VERY MUCH LESS-THAN
        { 0x22D9, 0x22D8 },   // VERY MUCH GREATER-THAN
        { 0x22DA, 0x22DB },   // LESS-THAN EQUAL TO OR GREATER-THAN
        { 0x22DB, 0x22DA },   // GREATER-THAN EQUAL TO OR LESS-THAN
        { 0x22DC, 0x22DD },   // EQUAL TO OR LESS-THAN
        { 0x22DD, 0x22DC },   // EQUAL TO OR GREATER-THAN
        { 0x22DE, 0x22DF },   // EQUAL TO OR PRECEDES
        { 0x22DF, 0x22DE },   // EQUAL TO OR SUCCEEDS
        { 0x22E0, 0x22E1 },   // [BEST FIT] DOES NOT PRECEDE OR EQUAL
        { 0x22E1, 0x22E0 },   // [BEST FIT] DOES NOT SUCCEED OR EQUAL
        { 0x22E2, 0x22E3 },   // [BEST FIT] NOT SQUARE IMAGE OF OR EQUAL TO
        { 0x22E3, 0x22E2 },   // [BEST FIT] NOT SQUARE ORIGINAL OF OR EQUAL TO
        { 0x22E4, 0x22E5 },   // [BEST FIT] SQUARE IMAGE OF OR NOT EQUAL TO
        { 0x22E5, 0x22E4 },   // [BEST FIT] SQUARE ORIGINAL OF OR NOT EQUAL TO
        { 0x22E6, 0x22E7 },   // [BEST FIT] LESS-THAN BUT NOT EQUIVALENT TO
        { 0x22E7, 0x22E6 },   // [BEST FIT] GREATER-THAN BUT NOT EQUIVALENT TO
        { 0x22E8, 0x22E9 },   // [BEST FIT] PRECEDES BUT NOT EQUIVALENT TO
        { 0x22E9, 0x22E8 },   // [BEST FIT] SUCCEEDS BUT NOT EQUIVALENT TO
        { 0x22EA, 0x22EB },   // [BEST FIT] NOT NORMAL SUBGROUP OF
        { 0x22EB, 0x22EA },   // [BEST FIT] DOES NOT CONTAIN AS NORMAL SUBGROUP
        { 0x22EC, 0x22ED },   // [BEST FIT] NOT NORMAL SUBGROUP OF OR EQUAL TO
        { 0x22ED, 0x22EC },   // [BEST FIT] DOES NOT CONTAIN AS NORMAL SUBGROUP OR EQUAL
        { 0x22F0, 0x22F1 },   // UP RIGHT DIAGONAL ELLIPSIS
        { 0x22F1, 0x22F0 },   // DOWN RIGHT DIAGONAL ELLIPSIS
        { 0x22F2, 0x22FA },   // ELEMENT OF WITH LONG HORIZONTAL STROKE
        { 0x22F3, 0x22FB },   // ELEMENT OF WITH VERTICAL BAR AT END OF HORIZONTAL STROKE
        { 0x22F4, 0x22FC },   // SMALL ELEMENT OF WITH VERTICAL BAR AT END OF HORIZONTAL STROKE
        { 0x22F6, 0x22FD },   // ELEMENT OF WITH OVERBAR
        { 0x22F7, 0x22FE },   // SMALL ELEMENT OF WITH OVERBAR
        { 0x22FA, 0x22F2 },   // CONTAINS WITH LONG HORIZONTAL STROKE
        { 0x22FB, 0x22F3 },   // CONTAINS WITH VERTICAL BAR AT END OF HORIZONTAL STROKE
        { 0x22FC, 0x22F4 },   // SMALL CONTAINS WITH VERTICAL BAR AT END OF HORIZONTAL STROKE
        { 0x22FD, 0x22F6 },   // CONTAINS WITH OVERBAR
        { 0x22FE, 0x22F7 },   // SMALL CONTAINS WITH OVERBAR
        { 0x2308, 0x2309 },   // LEFT CEILING
        { 0x2309, 0x2308 },   // RIGHT CEILING
        { 0x230A, 0x230B },   // LEFT FLOOR
        { 0x230B, 0x230A },   // RIGHT FLOOR
        { 0x2329, 0x232A },   // LEFT-POINTING ANGLE BRACKET
        { 0x232A, 0x2329 },   // RIGHT-POINTING ANGLE BRACKET
        { 0x2768, 0x2769 },   // MEDIUM LEFT PARENTHESIS ORNAMENT
        { 0x2769, 0x2768 },   // MEDIUM RIGHT PARENTHESIS ORNAMENT
        { 0x276A, 0x276B },   // MEDIUM FLATTENED LEFT PARENTHESIS ORNAMENT
        { 0x276B, 0x276A },   // MEDIUM FLATTENED RIGHT PARENTHESIS ORNAMENT
        { 0x276C, 0x276D },   // MEDIUM LEFT-POINTING ANGLE BRACKET ORNAMENT
        { 0x276D, 0x276C },   // MEDIUM RIGHT-POINTING ANGLE BRACKET ORNAMENT
        { 0x276E, 0x276F },   // HEAVY LEFT-POINTING ANGLE QUOTATION MARK ORNAMENT
        { 0x276F, 0x276E },   // HEAVY RIGHT-POINTING ANGLE QUOTATION MARK ORNAMENT
        { 0x2770, 0x2771 },   // HEAVY LEFT-POINTING ANGLE BRACKET ORNAMENT
        { 0x2771, 0x2770 },   // HEAVY RIGHT-POINTING ANGLE BRACKET ORNAMENT
        { 0x2772, 0x2773 },   // LIGHT LEFT TORTOISE SHELL BRACKET
        { 0x2773, 0x2772 },   // LIGHT RIGHT TORTOISE SHELL BRACKET
        { 0x2774, 0x2775 },   // MEDIUM LEFT CURLY BRACKET ORNAMENT
        { 0x2775, 0x2774 },   // MEDIUM RIGHT CURLY BRACKET ORNAMENT
        { 0x27D5, 0x27D6 },   // LEFT OUTER JOIN
        { 0x27D6, 0x27D5 },   // RIGHT OUTER JOIN
        { 0x27DD, 0x27DE },   // LONG RIGHT TACK
        { 0x27DE, 0x27DD },   // LONG LEFT TACK
        { 0x27E2, 0x27E3 },   // WHITE CONCAVE-SIDED DIAMOND WITH LEFTWARDS TICK
        { 0x27E3, 0x27E2 },   // WHITE CONCAVE-SIDED DIAMOND WITH RIGHTWARDS TICK
        { 0x27E4, 0x27E5 },   // WHITE SQUARE WITH LEFTWARDS TICK
        { 0x27E5, 0x27E4 },   // WHITE SQUARE WITH RIGHTWARDS TICK
        { 0x27E6, 0x27E7 },   // MATHEMATICAL LEFT WHITE SQUARE BRACKET
        { 0x27E7, 0x27E6 },   // MATHEMATICAL RIGHT WHITE SQUARE BRACKET
        { 0x27E8, 0x27E9 },   // MATHEMATICAL LEFT ANGLE BRACKET
        { 0x27E9, 0x27E8 },   // MATHEMATICAL RIGHT ANGLE BRACKET
        { 0x27EA, 0x27EB },   // MATHEMATICAL LEFT DOUBLE ANGLE BRACKET
        { 0x27EB, 0x27EA },   // MATHEMATICAL RIGHT DOUBLE ANGLE BRACKET
        { 0x2983, 0x2984 },   // LEFT WHITE CURLY BRACKET
        { 0x2984, 0x2983 },   // RIGHT WHITE CURLY BRACKET
        { 0x2985, 0x2986 },   // LEFT WHITE PARENTHESIS
        { 0x2986, 0x2985 },   // RIGHT WHITE PARENTHESIS
        { 0x2987, 0x2988 },   // Z NOTATION LEFT IMAGE BRACKET
        { 0x2988, 0x2987 },   // Z NOTATION RIGHT IMAGE BRACKET
        { 0x2989, 0x298A },   // Z NOTATION LEFT BINDING BRACKET
        { 0x298A, 0x2989 },   // Z NOTATION RIGHT BINDING BRACKET
        { 0x298B, 0x298C },   // LEFT SQUARE BRACKET WITH UNDERBAR
        { 0x298C, 0x298B },   // RIGHT SQUARE BRACKET WITH UNDERBAR
        { 0x298D, 0x2990 },   // LEFT SQUARE BRACKET WITH TICK IN TOP CORNER
        { 0x298E, 0x298F },   // RIGHT SQUARE BRACKET WITH TICK IN BOTTOM CORNER
        { 0x298F, 0x298E },   // LEFT SQUARE BRACKET WITH TICK IN BOTTOM CORNER
        { 0x2990, 0x298D },   // RIGHT SQUARE BRACKET WITH TICK IN TOP CORNER
        { 0x2991, 0x2992 },   // LEFT ANGLE BRACKET WITH DOT
        { 0x2992, 0x2991 },   // RIGHT ANGLE BRACKET WITH DOT
        { 0x2993, 0x2994 },   // LEFT ARC LESS-THAN BRACKET
        { 0x2994, 0x2993 },   // RIGHT ARC GREATER-THAN BRACKET
        { 0x2995, 0x2996 },   // DOUBLE LEFT ARC GREATER-THAN BRACKET
        { 0x2996, 0x2995 },   // DOUBLE RIGHT ARC LESS-THAN BRACKET
        { 0x2997, 0x2998 },   // LEFT BLACK TORTOISE SHELL BRACKET
        { 0x2998, 0x2997 },   // RIGHT BLACK TORTOISE SHELL BRACKET
        { 0x29B8, 0x2298 },   // CIRCLED REVERSE SOLIDUS
        { 0x29C0, 0x29C1 },   // CIRCLED LESS-THAN
        { 0x29C1, 0x29C0 },   // CIRCLED GREATER-THAN
        { 0x29C4, 0x29C5 },   // SQUARED RISING DIAGONAL SLASH
        { 0x29C5, 0x29C4 },   // SQUARED FALLING DIAGONAL SLASH
        { 0x29CF, 0x29D0 },   // LEFT TRIANGLE BESIDE VERTICAL BAR
        { 0x29D0, 0x29CF },   // VERTICAL BAR BESIDE RIGHT TRIANGLE
        { 0x29D1, 0x29D2 },   // BOWTIE WITH LEFT HALF BLACK
        { 0x29D2, 0x29D1 },   // BOWTIE WITH RIGHT HALF BLACK
        { 0x29D4, 0x29D5 },   // TIMES WITH LEFT HALF BLACK
        { 0x29D5, 0x29D4 },   // TIMES WITH RIGHT HALF BLACK
        { 0x29D8, 0x29D9 },   // LEFT WIGGLY FENCE
        { 0x29D9, 0x29D8 },   // RIGHT WIGGLY FENCE
        { 0x29DA, 0x29DB },   // LEFT DOUBLE WIGGLY FENCE
        { 0x29DB, 0x29DA },   // RIGHT DOUBLE WIGGLY FENCE
        { 0x29F5, 0x2215 },   // REVERSE SOLIDUS OPERATOR
        { 0x29F8, 0x29F9 },   // BIG SOLIDUS
        { 0x29F9, 0x29F8 },   // BIG REVERSE SOLIDUS
        { 0x29FC, 0x29FD },   // LEFT-POINTING CURVED ANGLE BRACKET
        { 0x29FD, 0x29FC },   // RIGHT-POINTING CURVED ANGLE BRACKET
        { 0x2A2B, 0x2A2C },   // MINUS SIGN WITH FALLING DOTS
        { 0x2A2C, 0x2A2B },   // MINUS SIGN WITH RISING DOTS
        { 0x2A2D, 0x2A2C },   // PLUS SIGN IN LEFT HALF CIRCLE
        { 0x2A2E, 0x2A2D },   // PLUS SIGN IN RIGHT HALF CIRCLE
        { 0x2A34, 0x2A35 },   // MULTIPLICATION SIGN IN LEFT HALF CIRCLE
        { 0x2A35, 0x2A34 },   // MULTIPLICATION SIGN IN RIGHT HALF CIRCLE
        { 0x2A3C, 0x2A3D },   // INTERIOR PRODUCT
        { 0x2A3D, 0x2A3C },   // RIGHTHAND INTERIOR PRODUCT
        { 0x2A64, 0x2A65 },   // Z NOTATION DOMAIN ANTIRESTRICTION
        { 0x2A65, 0x2A64 },   // Z NOTATION RANGE ANTIRESTRICTION
        { 0x2A79, 0x2A7A },   // LESS-THAN WITH CIRCLE INSIDE
        { 0x2A7A, 0x2A79 },   // GREATER-THAN WITH CIRCLE INSIDE
        { 0x2A7D, 0x2A7E },   // LESS-THAN OR SLANTED EQUAL TO
        { 0x2A7E, 0x2A7D },   // GREATER-THAN OR SLANTED EQUAL TO
        { 0x2A7F, 0x2A80 },   // LESS-THAN OR SLANTED EQUAL TO WITH DOT INSIDE
        { 0x2A80, 0x2A7F },   // GREATER-THAN OR SLANTED EQUAL TO WITH DOT INSIDE
        { 0x2A81, 0x2A82 },   // LESS-THAN OR SLANTED EQUAL TO WITH DOT ABOVE
        { 0x2A82, 0x2A81 },   // GREATER-THAN OR SLANTED EQUAL TO WITH DOT ABOVE
        { 0x2A83, 0x2A84 },   // LESS-THAN OR SLANTED EQUAL TO WITH DOT ABOVE RIGHT
        { 0x2A84, 0x2A83 },   // GREATER-THAN OR SLANTED EQUAL TO WITH DOT ABOVE LEFT
        { 0x2A8B, 0x2A8C },   // LESS-THAN ABOVE DOUBLE-LINE EQUAL ABOVE GREATER-THAN
        { 0x2A8C, 0x2A8B },   // GREATER-THAN ABOVE DOUBLE-LINE EQUAL ABOVE LESS-THAN
        { 0x2A91, 0x2A92 },   // LESS-THAN ABOVE GREATER-THAN ABOVE DOUBLE-LINE EQUAL
        { 0x2A92, 0x2A91 },   // GREATER-THAN ABOVE LESS-THAN ABOVE DOUBLE-LINE EQUAL
        { 0x2A93, 0x2A94 },   // LESS-THAN ABOVE SLANTED EQUAL ABOVE GREATER-THAN ABOVE SLANTED EQUAL
        { 0x2A94, 0x2A93 },   // GREATER-THAN ABOVE SLANTED EQUAL ABOVE LESS-THAN ABOVE SLANTED EQUAL
        { 0x2A95, 0x2A96 },   // SLANTED EQUAL TO OR LESS-THAN
        { 0x2A96, 0x2A95 },   // SLANTED EQUAL TO OR GREATER-THAN
        { 0x2A97, 0x2A98 },   // SLANTED EQUAL TO OR LESS-THAN WITH DOT INSIDE
        { 0x2A98, 0x2A97 },   // SLANTED EQUAL TO OR GREATER-THAN WITH DOT INSIDE
        { 0x2A99, 0x2A9A },   // DOUBLE-LINE EQUAL TO OR LESS-THAN
        { 0x2A9A, 0x2A99 },   // DOUBLE-LINE EQUAL TO OR GREATER-THAN
        { 0x2A9B, 0x2A9C },   // DOUBLE-LINE SLANTED EQUAL TO OR LESS-THAN
        { 0x2A9C, 0x2A9B },   // DOUBLE-LINE SLANTED EQUAL TO OR GREATER-THAN
        { 0x2AA1, 0x2AA2 },   // DOUBLE NESTED LESS-THAN
        { 0x2AA2, 0x2AA1 },   // DOUBLE NESTED GREATER-THAN
        { 0x2AA6, 0x2AA7 },   // LESS-THAN CLOSED BY CURVE
        { 0x2AA7, 0x2AA6 },   // GREATER-THAN CLOSED BY CURVE
        { 0x2AA8, 0x2AA9 },   // LESS-THAN CLOSED BY CURVE ABOVE SLANTED EQUAL
        { 0x2AA9, 0x2AA8 },   // GREATER-THAN CLOSED BY CURVE ABOVE SLANTED EQUAL
        { 0x2AAA, 0x2AAB },   // SMALLER THAN
        { 0x2AAB, 0x2AAA },   // LARGER THAN
        { 0x2AAC, 0x2AAD },   // SMALLER THAN OR EQUAL TO
        { 0x2AAD, 0x2AAC },   // LARGER THAN OR EQUAL TO
        { 0x2AAF, 0x2AB0 },   // PRECEDES ABOVE SINGLE-LINE EQUALS SIGN
        { 0x2AB0, 0x2AAF },   // SUCCEEDS ABOVE SINGLE-LINE EQUALS SIGN
        { 0x2AB3, 0x2AB4 },   // PRECEDES ABOVE EQUALS SIGN
        { 0x2AB4, 0x2AB3 },   // SUCCEEDS ABOVE EQUALS SIGN
        { 0x2ABB, 0x2ABC },   // DOUBLE PRECEDES
        { 0x2ABC, 0x2ABB },   // DOUBLE SUCCEEDS
        { 0x2ABD, 0x2ABE },   // SUBSET WITH DOT
        { 0x2ABE, 0x2ABD },   // SUPERSET WITH DOT
        { 0x2ABF, 0x2AC0 },   // SUBSET WITH PLUS SIGN BELOW
        { 0x2AC0, 0x2ABF },   // SUPERSET WITH PLUS SIGN BELOW
        { 0x2AC1, 0x2AC2 },   // SUBSET WITH MULTIPLICATION SIGN BELOW
        { 0x2AC2, 0x2AC1 },   // SUPERSET WITH MULTIPLICATION SIGN BELOW
        { 0x2AC3, 0x2AC4 },   // SUBSET OF OR EQUAL TO WITH DOT ABOVE
        { 0x2AC4, 0x2AC3 },   // SUPERSET OF OR EQUAL TO WITH DOT ABOVE
        { 0x2AC5, 0x2AC6 },   // SUBSET OF ABOVE EQUALS SIGN
        { 0x2AC6, 0x2AC5 },   // SUPERSET OF ABOVE EQUALS SIGN
        { 0x2ACD, 0x2ACE },   // SQUARE LEFT OPEN BOX OPERATOR
        { 0x2ACE, 0x2ACD },   // SQUARE RIGHT OPEN BOX OPERATOR
        { 0x2ACF, 0x2AD0 },   // CLOSED SUBSET
        { 0x2AD0, 0x2ACF },   // CLOSED SUPERSET
        { 0x2AD1, 0x2AD2 },   // CLOSED SUBSET OR EQUAL TO
        { 0x2AD2, 0x2AD1 },   // CLOSED SUPERSET OR EQUAL TO
        { 0x2AD3, 0x2AD4 },   // SUBSET ABOVE SUPERSET
        { 0x2AD4, 0x2AD3 },   // SUPERSET ABOVE SUBSET
        { 0x2AD5, 0x2AD6 },   // SUBSET ABOVE SUBSET
        { 0x2AD6, 0x2AD5 },   // SUPERSET ABOVE SUPERSET
        { 0x2ADE, 0x22A6 },   // SHORT LEFT TACK
        { 0x2AE3, 0x22A9 },   // DOUBLE VERTICAL BAR LEFT TURNSTILE
        { 0x2AE4, 0x22A8 },   // VERTICAL BAR DOUBLE LEFT TURNSTILE
        { 0x2AE5, 0x22AB },   // DOUBLE VERTICAL BAR DOUBLE LEFT TURNSTILE
        { 0x2AEC, 0x2AED },   // DOUBLE STROKE NOT SIGN
        { 0x2AED, 0x2AEC },   // REVERSED DOUBLE STROKE NOT SIGN
        { 0x2AF7, 0x2AF8 },   // TRIPLE NESTED LESS-THAN
        { 0x2AF8, 0x2AF7 },   // TRIPLE NESTED GREATER-THAN
        { 0x2AF9, 0x2AFA },   // DOUBLE-LINE SLANTED LESS-THAN OR EQUAL TO
        { 0x2AFA, 0x2AF9 },   // DOUBLE-LINE SLANTED GREATER-THAN OR EQUAL TO
        { 0x3008, 0x3009 },   // LEFT ANGLE BRACKET
        { 0x3009, 0x3008 },   // RIGHT ANGLE BRACKET
        { 0x300A, 0x300B },   // LEFT DOUBLE ANGLE BRACKET
        { 0x300B, 0x300A },   // RIGHT DOUBLE ANGLE BRACKET
        { 0x300C, 0x300D },   // [BEST FIT] LEFT CORNER BRACKET
        { 0x300D, 0x300C },   // [BEST FIT] RIGHT CORNER BRACKET
        { 0x300E, 0x300F },   // [BEST FIT] LEFT WHITE CORNER BRACKET
        { 0x300F, 0x300E },   // [BEST FIT] RIGHT WHITE CORNER BRACKET
        { 0x3010, 0x3011 },   // LEFT BLACK LENTICULAR BRACKET
        { 0x3011, 0x3010 },   // RIGHT BLACK LENTICULAR BRACKET
        { 0x3014, 0x3015 },   // LEFT TORTOISE SHELL BRACKET
        { 0x3015, 0x3014 },   // RIGHT TORTOISE SHELL BRACKET
        { 0x3016, 0x3017 },   // LEFT WHITE LENTICULAR BRACKET
        { 0x3017, 0x3016 },   // RIGHT WHITE LENTICULAR BRACKET
        { 0x3018, 0x3019 },   // LEFT WHITE TORTOISE SHELL BRACKET
        { 0x3019, 0x3018 },   // RIGHT WHITE TORTOISE SHELL BRACKET
        { 0x301A, 0x301B },   // LEFT WHITE SQUARE BRACKET
        { 0x301B, 0x301A },   // RIGHT WHITE SQUARE BRACKET
        { 0xFF08, 0xFF09 },   // FULLWIDTH LEFT PARENTHESIS
        { 0xFF09, 0xFF08 },   // FULLWIDTH RIGHT PARENTHESIS
        { 0xFF1C, 0xFF1E },   // FULLWIDTH LESS-THAN SIGN
        { 0xFF1E, 0xFF1C },   // FULLWIDTH GREATER-THAN SIGN
        { 0xFF3B, 0xFF3D },   // FULLWIDTH LEFT SQUARE BRACKET
        { 0xFF3D, 0xFF3B },   // FULLWIDTH RIGHT SQUARE BRACKET
        { 0xFF5B, 0xFF5D },   // FULLWIDTH LEFT CURLY BRACKET
        { 0xFF5D, 0xFF5B },   // FULLWIDTH RIGHT CURLY BRACKET
        { 0xFF5F, 0xFF60 },   // FULLWIDTH LEFT WHITE PARENTHESIS
        { 0xFF60, 0xFF5F },   // FULLWIDTH RIGHT WHITE PARENTHESIS
        { 0xFF62, 0xFF63 },   // [BEST FIT] HALFWIDTH LEFT CORNER BRACKET
        { 0xFF63, 0xFF62 },   // [BEST FIT] HALFWIDTH RIGHT CORNER BRACKET
    };

    const MirrorPair* gpMirrorPairArray      = gMirrorPairArray;
    uint32_t          gnMirrorPairArrayCount = sizeof(gMirrorPairArray) / sizeof(gMirrorPairArray[0]);

#elif (EATEXT_MIRROR_CHAR_SUPPORT >= 1)
    // This is the more condensed mirroring table.
    // It includes the most common mirror characters likely to be encountered 
    // by text this library is expected to see in practice.
    const MirrorPair gMirrorPairArray[] = 
    {
        { 0x0028, 0x0029 },   // LEFT PARENTHESIS
        { 0x0029, 0x0028 },   // RIGHT PARENTHESIS
        { 0x003C, 0x003E },   // LESS-THAN SIGN
        { 0x003E, 0x003C },   // GREATER-THAN SIGN
        { 0x005B, 0x005D },   // LEFT SQUARE BRACKET
        { 0x005D, 0x005B },   // RIGHT SQUARE BRACKET
        { 0x007B, 0x007D },   // LEFT CURLY BRACKET
        { 0x007D, 0x007B },   // RIGHT CURLY BRACKET
        { 0x00AB, 0x00BB },   // LEFT-POINTING DOUBLE ANGLE QUOTATION MARK
        { 0x00BB, 0x00AB },   // RIGHT-POINTING DOUBLE ANGLE QUOTATION MARK
        { 0x2264, 0x2265 },   // LESS-THAN OR EQUAL TO
        { 0x2265, 0x2264 },   // GREATER-THAN OR EQUAL TO
        { 0x2266, 0x2267 },   // LESS-THAN OVER EQUAL TO
        { 0x2267, 0x2266 },   // GREATER-THAN OVER EQUAL TO
        { 0x3008, 0x3009 },   // LEFT ANGLE BRACKET
        { 0x3009, 0x3008 },   // RIGHT ANGLE BRACKET
        { 0x300A, 0x300B },   // LEFT DOUBLE ANGLE BRACKET
        { 0x300B, 0x300A },   // RIGHT DOUBLE ANGLE BRACKET
        { 0x3010, 0x3011 },   // LEFT BLACK LENTICULAR BRACKET
        { 0x3011, 0x3010 },   // RIGHT BLACK LENTICULAR BRACKET
        { 0x3014, 0x3015 },   // LEFT TORTOISE SHELL BRACKET
        { 0x3015, 0x3014 },   // RIGHT TORTOISE SHELL BRACKET
        { 0x3016, 0x3017 },   // LEFT WHITE LENTICULAR BRACKET
        { 0x3017, 0x3016 },   // RIGHT WHITE LENTICULAR BRACKET
        { 0x3018, 0x3019 },   // LEFT WHITE TORTOISE SHELL BRACKET
        { 0x3019, 0x3018 },   // RIGHT WHITE TORTOISE SHELL BRACKET
        { 0x301A, 0x301B },   // LEFT WHITE SQUARE BRACKET
        { 0x301B, 0x301A },   // RIGHT WHITE SQUARE BRACKET
        { 0xFF08, 0xFF09 },   // FULLWIDTH LEFT PARENTHESIS
        { 0xFF09, 0xFF08 },   // FULLWIDTH RIGHT PARENTHESIS
        { 0xFF1C, 0xFF1E },   // FULLWIDTH LESS-THAN SIGN
        { 0xFF1E, 0xFF1C },   // FULLWIDTH GREATER-THAN SIGN
        { 0xFF3B, 0xFF3D },   // FULLWIDTH LEFT SQUARE BRACKET
        { 0xFF3D, 0xFF3B },   // FULLWIDTH RIGHT SQUARE BRACKET
        { 0xFF5B, 0xFF5D },   // FULLWIDTH LEFT CURLY BRACKET
        { 0xFF5D, 0xFF5B },   // FULLWIDTH RIGHT CURLY BRACKET
        { 0xFF5F, 0xFF60 },   // FULLWIDTH LEFT WHITE PARENTHESIS
        { 0xFF60, 0xFF5F },   // FULLWIDTH RIGHT WHITE PARENTHESIS
    };

    const MirrorPair* gpMirrorPairArray      = gMirrorPairArray;
    uint32_t          gnMirrorPairArrayCount = sizeof(gMirrorPairArray) / sizeof(gMirrorPairArray[0]);

#else // EATEXT_MIRROR_CHAR_SUPPORT

    // In this case the mirror table is empty and not allocated 
    // unless the user sets one. We at least define a single empty
    // entry so that the GetMirrorChar doesn't crash.
    const MirrorPair mp = { kCharInvalid, kCharInvalid };
    const MirrorPair* gpMirrorPairArray      = &mp;
    uint32_t          gnMirrorPairArrayCount = 1;

#endif



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
#define NS   kBidiClassNSM
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


///////////////////////////////////////////////////////////////////////////////
// gBidiClassTable256
//
// To consider: We actually need only about 4 bits per entry. We can possibly 
// pack nibbles. The effort to read them may not be worth it, though.
//
const uint8_t gBidiClassTable256[256] =
{
    BN, BN, BN, BN, BN, BN, BN, BN, BN,  S,  B,  S, WS,  B, BN, BN,  // 0000
    BN, BN, BN, BN, BN, BN, BN, BN, BN, BN, BN, BN,  B,  B,  B,  S,  // 0010
    WS, ON, ON, ET, ET, ET, ON, ON, ON, ON, ON, ES, CS, ES, CS, CS,  // 0020
    EN, EN, EN, EN, EN, EN, EN, EN, EN, EN, CS, ON, ON, ON, ON, ON,  // 0030
    ON,  L,  L,  L,  L,  L,  L,  L,  L,  L,  L,  L,  L,  L,  L,  L,  // 0040
     L,  L,  L,  L,  L,  L,  L,  L,  L,  L,  L, ON, ON, ON, ON, ON,  // 0050
    ON,  L,  L,  L,  L,  L,  L,  L,  L,  L,  L,  L,  L,  L,  L,  L,  // 0060
     L,  L,  L,  L,  L,  L,  L,  L,  L,  L,  L, ON, ON, ON, ON, BN,  // 0070
    BN, BN, BN, BN, BN,  B, BN, BN, BN, BN, BN, BN, BN, BN, BN, BN,  // 0080
    BN, BN, BN, BN, BN, BN, BN, BN, BN, BN, BN, BN, BN, BN, BN, BN,  // 0090
    CS, ON, ET, ET, ET, ET, ON, ON, ON, ON,  L, ON, ON, BN, ON, ON,  // 00A0
    ET, ET, EN, EN, ON,  L, ON, ON, ON, EN,  L, ON, ON, ON, ON, ON,  // 00B0
     L,  L,  L,  L,  L,  L,  L,  L,  L,  L,  L,  L,  L,  L,  L,  L,  // 00C0
     L,  L,  L,  L,  L,  L,  L, ON,  L,  L,  L,  L,  L,  L,  L,  L,  // 00D0
     L,  L,  L,  L,  L,  L,  L,  L,  L,  L,  L,  L,  L,  L,  L,  L,  // 00E0 
     L,  L,  L,  L,  L,  L,  L, ON,  L,  L,  L,  L,  L,  L,  L,  L   // 00F0
};


///////////////////////////////////////////////////////////////////////////////
// gArabicBidiClassTable256
//
#if EATEXT_ARABIC_SCRIPT_SUPPORTED

const uint8_t gArabicBidiClassTable256[256] =
{
    AL, AL, AL, AL, AL, AL, AL, AL, AL, AL, AL, AL, CS, AL, ON, ON,  // 0600
    NS, NS, NS, NS, NS, NS, AL, AL, AL, AL, AL, AL, AL, AL, AL, AL,  // 0610
    AL, AL, AL, AL, AL, AL, AL, AL, AL, AL, AL, AL, AL, AL, AL, AL,  // 0620
    AL, AL, AL, AL, AL, AL, AL, AL, AL, AL, AL, AL, AL, AL, AL, AL,  // 0630
    AL, AL, AL, AL, AL, AL, AL, AL, AL, AL, AL, NS, NS, NS, NS, NS,  // 0640
    NS, NS, NS, NS, NS, NS, NS, NS, NS, NS, NS, NS, NS, NS, NS, AL,  // 0650
    AN, AN, AN, AN, AN, AN, AN, AN, AN, AN, ET, AN, AN, AL, AL, AL,  // 0660
    NS, AL, AL, AL, AL, AL, AL, AL, AL, AL, AL, AL, AL, AL, AL, AL,  // 0670
    AL, AL, AL, AL, AL, AL, AL, AL, AL, AL, AL, AL, AL, AL, AL, AL,  // 0680
    AL, AL, AL, AL, AL, AL, AL, AL, AL, AL, AL, AL, AL, AL, AL, AL,  // 0690
    AL, AL, AL, AL, AL, AL, AL, AL, AL, AL, AL, AL, AL, AL, AL, AL,  // 06A0
    AL, AL, AL, AL, AL, AL, AL, AL, AL, AL, AL, AL, AL, AL, AL, AL,  // 06B0
    AL, AL, AL, AL, AL, AL, AL, AL, AL, AL, AL, AL, AL, AL, AL, AL,  // 06C0
    AL, AL, AL, AL, AL, AL, NS, NS, NS, NS, NS, NS, NS, AL, NS, NS,  // 06D0
    NS, NS, NS, NS, NS, AL, AL, NS, NS, ON, NS, NS, NS, NS, AL, AL,  // 06E0 
    EN, EN, EN, EN, EN, EN, EN, EN, EN, EN, AL, AL, AL, AL, AL, AL   // 06F0
};

#endif


// Undefine the above definitions
#undef N
#undef ON
#undef L  
#undef R  
#undef AN 
#undef EN 
#undef AL 
#undef NS
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


///////////////////////////////////////////////////////////////////////////////
// BlockTableEntry
//
struct BlockTableEntry
{
    uint16_t mnBegin;   /// We don't expect to support Chars with values > 65536, even if Char is a 32 bit type.
    uint16_t mnCount;   /// We don't expect to support Chars with values > 65536, even if Char is a 32 bit type.
    int8_t   mnBlock;   /// One of enum Block.
};

#if (EATEXT_BLOCK_INFO_SUPPORT > 1)
    /// Todo: Convert this structure into one that can be searched faster.
    BlockTableEntry gBlockTableEntryArray[] = 
    { 
        { 0x0000, 0x007F - 0x0000 + 1, kBlockBasicLatin },
        { 0x0080, 0x00FF - 0x0080 + 1, kBlockLatin1Supplement },
        { 0x0100, 0x017F - 0x0100 + 1, kBlockLatinExtendedA },
        { 0x0180, 0x024F - 0x0180 + 1, kBlockLatinExtendedB },
        { 0x0250, 0x02AF - 0x0250 + 1, kBlockIPAExtensions },
        { 0x02B0, 0x02FF - 0x02B0 + 1, kBlockSpacingModifierLetters },
        { 0x0300, 0x036F - 0x0300 + 1, kBlockCombiningDiacriticalMarks },
        { 0x0370, 0x03FF - 0x0370 + 1, kBlockGreekAndCoptic },
        { 0x0400, 0x04FF - 0x0400 + 1, kBlockCyrillic },
        { 0x0500, 0x052F - 0x0500 + 1, kBlockCyrillicSupplement },
        { 0x0530, 0x058F - 0x0530 + 1, kBlockArmenian },
        { 0x0590, 0x05FF - 0x0590 + 1, kBlockHebrew },
        { 0x0600, 0x06FF - 0x0600 + 1, kBlockArabic },
        { 0x0700, 0x074F - 0x0700 + 1, kBlockSyriac },
        { 0x0780, 0x07BF - 0x0780 + 1, kBlockThaana },
        { 0x0900, 0x097F - 0x0900 + 1, kBlockDevanagari },
        { 0x0980, 0x09FF - 0x0980 + 1, kBlockBengali },
        { 0x0A00, 0x0A7F - 0x0A00 + 1, kBlockGurmukhi },
        { 0x0A80, 0x0AFF - 0x0A80 + 1, kBlockGujarati },
        { 0x0B00, 0x0B7F - 0x0B00 + 1, kBlockOriya },
        { 0x0B80, 0x0BFF - 0x0B80 + 1, kBlockTamil },
        { 0x0C00, 0x0C7F - 0x0C00 + 1, kBlockTelugu },
        { 0x0C80, 0x0CFF - 0x0C80 + 1, kBlockKannada },
        { 0x0D00, 0x0D7F - 0x0D00 + 1, kBlockMalayalam },
        { 0x0D80, 0x0DFF - 0x0D80 + 1, kBlockSinhala },
        { 0x0E00, 0x0E7F - 0x0E00 + 1, kBlockThai },
        { 0x0E80, 0x0EFF - 0x0E80 + 1, kBlockLao },
        { 0x0F00, 0x0FFF - 0x0F00 + 1, kBlockTibetan },
        { 0x1000, 0x109F - 0x1000 + 1, kBlockMyanmar },
        { 0x10A0, 0x10FF - 0x10A0 + 1, kBlockGeorgian },
        { 0x1100, 0x11FF - 0x1100 + 1, kBlockHangulJamo },
        { 0x1200, 0x137F - 0x1200 + 1, kBlockEthiopic },
        { 0x13A0, 0x13FF - 0x13A0 + 1, kBlockCherokee },
        { 0x1400, 0x167F - 0x1400 + 1, kBlockUnifiedCanadianAboriginalSyllabics },
        { 0x1680, 0x169F - 0x1680 + 1, kBlockOgham },
        { 0x16A0, 0x16FF - 0x16A0 + 1, kBlockRunic },
        { 0x1700, 0x171F - 0x1700 + 1, kBlockTagalog },
        { 0x1720, 0x173F - 0x1720 + 1, kBlockHanunoo },
        { 0x1740, 0x175F - 0x1740 + 1, kBlockBuhid },
        { 0x1760, 0x177F - 0x1760 + 1, kBlockTagbanwa },
        { 0x1780, 0x17FF - 0x1780 + 1, kBlockKhmer },
        { 0x1800, 0x18AF - 0x1800 + 1, kBlockMongolian },
        { 0x1900, 0x194F - 0x1900 + 1, kBlockLimbu },
        { 0x1950, 0x197F - 0x1950 + 1, kBlockTaiLe },
        { 0x19E0, 0x19FF - 0x19E0 + 1, kBlockKhmerSymbols },
        { 0x1D00, 0x1D7F - 0x1D00 + 1, kBlockPhoneticExtensions },
        { 0x1E00, 0x1EFF - 0x1E00 + 1, kBlockLatinExtendedAdditional },
        { 0x1F00, 0x1FFF - 0x1F00 + 1, kBlockGreekExtended },
        { 0x2000, 0x206F - 0x2000 + 1, kBlockGeneralPunctuation },
        { 0x2070, 0x209F - 0x2070 + 1, kBlockSuperscriptsAndSubscripts },
        { 0x20A0, 0x20CF - 0x20A0 + 1, kBlockCurrencySymbols },
        { 0x20D0, 0x20FF - 0x20D0 + 1, kBlockCombiningDiacriticalMarksForSymbols },
        { 0x2100, 0x214F - 0x2100 + 1, kBlockLetterLikeSymbols },
        { 0x2150, 0x218F - 0x2150 + 1, kBlockNumberForms },
        { 0x2190, 0x21FF - 0x2190 + 1, kBlockArrows },
        { 0x2200, 0x22FF - 0x2200 + 1, kBlockMathematicalOperators },
        { 0x2300, 0x23FF - 0x2300 + 1, kBlockMiscellaneousTechnical },
        { 0x2400, 0x243F - 0x2400 + 1, kBlockControlPictures },
        { 0x2440, 0x245F - 0x2440 + 1, kBlockOpticalCharacterRecognition },
        { 0x2460, 0x24FF - 0x2460 + 1, kBlockEnclosedAlphanumerics },
        { 0x2500, 0x257F - 0x2500 + 1, kBlockBoxDrawing },
        { 0x2580, 0x259F - 0x2580 + 1, kBlockBlockElements },
        { 0x25A0, 0x25FF - 0x25A0 + 1, kBlockGeometricShapes },
        { 0x2600, 0x26FF - 0x2600 + 1, kBlockMiscellaneousSymbols },
        { 0x2700, 0x27BF - 0x2700 + 1, kBlockDingbats },
        { 0x27C0, 0x27EF - 0x27C0 + 1, kBlockMiscellaneousMathematicalSymbolsA },
        { 0x27F0, 0x27FF - 0x27F0 + 1, kBlockSupplementalArrowsA },
        { 0x2800, 0x28FF - 0x2800 + 1, kBlockBraillePatterns },
        { 0x2900, 0x297F - 0x2900 + 1, kBlockSupplementalArrowsB },
        { 0x2980, 0x29FF - 0x2980 + 1, kBlockMiscellaneousMathematicalSymbolsB },
        { 0x2A00, 0x2AFF - 0x2A00 + 1, kBlockSupplementalMathematicalOperators },
        { 0x2B00, 0x2BFF - 0x2B00 + 1, kBlockMiscellaneousSymbolsandArrows },
        { 0x2E80, 0x2EFF - 0x2E80 + 1, kBlockCJKRadicalsSupplement },
        { 0x2F00, 0x2FDF - 0x2F00 + 1, kBlockKangxiRadicals },
        { 0x2FF0, 0x2FFF - 0x2FF0 + 1, kBlockIdeographicDescriptionCharacters },
        { 0x3000, 0x303F - 0x3000 + 1, kBlockCJKSymbolsAndPunctuation },
        { 0x3040, 0x309F - 0x3040 + 1, kBlockHiragana },
        { 0x30A0, 0x30FF - 0x30A0 + 1, kBlockKatakana },
        { 0x3100, 0x312F - 0x3100 + 1, kBlockBopomofo },
        { 0x3130, 0x318F - 0x3130 + 1, kBlockHangulCompatibilityJamo },
        { 0x3190, 0x319F - 0x3190 + 1, kBlockKanbun },
        { 0x31A0, 0x31BF - 0x31A0 + 1, kBlockBopomofoExtended },
        { 0x31F0, 0x31FF - 0x31F0 + 1, kBlockKatakanaPhoneticExtensions},
        { 0x3200, 0x32FF - 0x3200 + 1, kBlockEnclosedCJKLettersAndMonths },
        { 0x3300, 0x33FF - 0x3300 + 1, kBlockCJKCompatibility },
        { 0x3400, 0x4DBF - 0x3400 + 1, kBlockCJKUnifiedIdeographsExtensionA },
        { 0x4DC0, 0x4DFF - 0x4DC0 + 1, kBlockYijingHexagramSymbols },
        { 0x4E00, 0x9FFF - 0x4E00 + 1, kBlockCJKUnifiedIdeographs },
        { 0xA000, 0xA48F - 0xA000 + 1, kBlockYiSyllables },
        { 0xA490, 0xA4CF - 0xA490 + 1, kBlockYiRadicals },
        { 0xAC00, 0xD7AF - 0xAC00 + 1, kBlockHangulSyllables },
        { 0xD800, 0xDB7F - 0xD800 + 1, kBlockHighSurrogates },
        { 0xDB80, 0xDBFF - 0xDB80 + 1, kBlockHighPrivateUseSurrogates },
        { 0xDC00, 0xDFFF - 0xDC00 + 1, kBlockLowSurrogates },
        { 0xE000, 0xF8FF - 0xE000 + 1, kBlockPrivateUseArea },
        { 0xF900, 0xFAFF - 0xF900 + 1, kBlockCJKCompatibilityIdeographs },
        { 0xFB00, 0xFB4F - 0xFB00 + 1, kBlockAlphabeticPresentationForms },
        { 0xFB50, 0xFDFF - 0xFB50 + 1, kBlockArabicPresentationFormsA },
        { 0xFE00, 0xFE0F - 0xFE00 + 1, kBlockVariationSelectors },
        { 0xFE20, 0xFE2F - 0xFE20 + 1, kBlockCombiningHalfMarks },
        { 0xFE30, 0xFE4F - 0xFE30 + 1, kBlockCJKCompatibilityForms },
        { 0xFE50, 0xFE6F - 0xFE50 + 1, kBlockSmallFormVariants },
        { 0xFE70, 0xFEFF - 0xFE70 + 1, kBlockArabicPresentationFormsB },
        { 0xFF00, 0xFFEF - 0xFF00 + 1, kBlockHalfWidthAndFullWidthForms },
        { 0xFFF0, 0xFFFF - 0xFFF0 + 1, kBlockSpecials }
    };

#else
    BlockTableEntry gBlockTableEntryArray[] = 
    { 
        { 0x0000, 0x007F - 0x0000 + 1, kBlockBasicLatin },
        { 0x0080, 0x00FF - 0x0080 + 1, kBlockLatin1Supplement },
        { 0x0100, 0x017F - 0x0100 + 1, kBlockLatinExtendedA },
        { 0x0180, 0x024F - 0x0180 + 1, kBlockLatinExtendedB },
        { 0x02B0, 0x02FF - 0x02B0 + 1, kBlockSpacingModifierLetters },
        { 0x0300, 0x036F - 0x0300 + 1, kBlockCombiningDiacriticalMarks },
        { 0x0370, 0x03FF - 0x0370 + 1, kBlockGreekAndCoptic },
        { 0x0400, 0x04FF - 0x0400 + 1, kBlockCyrillic },
        { 0x0590, 0x05FF - 0x0590 + 1, kBlockHebrew },
        { 0x0600, 0x06FF - 0x0600 + 1, kBlockArabic },
        { 0x0E00, 0x0E7F - 0x0E00 + 1, kBlockThai },
        { 0x1100, 0x11FF - 0x1100 + 1, kBlockHangulJamo },
        { 0x20A0, 0x20CF - 0x20A0 + 1, kBlockCurrencySymbols },
        { 0x3000, 0x303F - 0x3000 + 1, kBlockCJKSymbolsAndPunctuation },
        { 0x3040, 0x309F - 0x3040 + 1, kBlockHiragana },
        { 0x30A0, 0x30FF - 0x30A0 + 1, kBlockKatakana },
        { 0x4E00, 0x9FFF - 0x4E00 + 1, kBlockCJKUnifiedIdeographs },
        { 0xAC00, 0xD7AF - 0xAC00 + 1, kBlockHangulSyllables },
        { 0xE000, 0xF8FF - 0xE000 + 1, kBlockPrivateUseArea },
        { 0xFF00, 0xFFEF - 0xFF00 + 1, kBlockHalfWidthAndFullWidthForms }
    };

#endif

const BlockTableEntry* gpBlockTableEntryArray      = gBlockTableEntryArray;
uint32_t               gnBlockTableEntryArrayCount = sizeof(gBlockTableEntryArray) / sizeof(gBlockTableEntryArray[0]);



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

struct CharCategoryInfo
{
    uint16_t mCode;         /// Starting code of the range
    uint8_t  mCharClass0;   /// Character class for 0 bits
    uint8_t  mCharClass1;   /// Character class for 1 bits
    uint8_t* mpBitMask;     /// Bitmask (can be NULL)
};

uint8_t cc_bitarray_00f8[] = { 0x00, 0x55, 0x55, 0x55, 0x55, 0x55, 0x55, 0x55, 0xaa,
                               0xaa, 0x54, 0x55, 0x55, 0x55, 0x55, 0x55, 0x2b, 0xd6,
                               0xce, 0xdb, 0xb1, 0xd5, 0xd2, 0xae, 0x01 };
uint8_t cc_bitarray_01cc[] = { 0xaa, 0xaa, 0x54, 0x55, 0x25 };
uint8_t cc_bitarray_01f3[] = { 0xba, 0xaa, 0xaa, 0xaa, 0xaa, 0xaa, 0xaa, 0xaa, 0x00 };
uint8_t cc_bitarray_02b0[] = { 0x00, 0x00, 0x3c, 0x00, 0xfc, 0xff, 0xe0, 0xbf, 0xff,
                               0xff, 0x00 };
uint8_t cc_bitarray_0300[] = { 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
                               0x00, 0x00, 0x1f, 0x00, 0x00, 0x0f };
uint8_t cc_bitarray_0388[] = { 0x28, 0x00 };
uint8_t cc_bitarray_03d0[] = { 0x1c, 0x55, 0x55, 0x55, 0x10 };
uint8_t cc_bitarray_03f7[] = { 0x12 };
uint8_t cc_bitarray_0400[] = { 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0xff, 0xff, 0xff,
                               0xff, 0xff, 0xff, 0xaa, 0xaa, 0xaa, 0xaa, 0x02 };
uint8_t cc_bitarray_048a[] = { 0xaa, 0xaa, 0xaa, 0xaa, 0xaa, 0xaa, 0x2a, 0x55, 0x15 };
uint8_t cc_bitarray_04d0[] = { 0xaa, 0xaa, 0xaa, 0xaa, 0x2a };
uint8_t cc_bitarray_0500[] = { 0xaa, 0xaa, 0x00 };
uint8_t cc_bitarray_058b[] = { 0xc0, 0xff, 0x7f, 0xff, 0xff, 0x7f, 0x07 };
uint8_t cc_bitarray_05be[] = { 0x5a };
uint8_t cc_bitarray_05c5[] = { 0x00, 0xf8, 0xff, 0xff, 0x3f, 0x38 };
uint8_t cc_bitarray_0616[] = { 0x20, 0x02 };
uint8_t cc_bitarray_0710[] = { 0x02, 0x00, 0x00, 0x00, 0xff, 0xff, 0xff, 0x07 };
uint8_t cc_bitarray_074b[] = { 0x1c, 0x00, 0x00, 0x00, 0x00, 0x00, 0xe0, 0xff, 0xff,
                               0xff, 0xff, 0x07 };
uint8_t cc_bitarray_093e[] = { 0xf8, 0x87, 0x00 };
uint8_t cc_bitarray_0984[] = { 0xfe, 0x99, 0xff, 0xff, 0xdf, 0x5f, 0x3c, 0x00 };
uint8_t cc_bitarray_09c5[] = { 0xcc, 0x00 };
uint8_t cc_bitarray_09d8[] = { 0xb0, 0x03 };
uint8_t cc_bitarray_0a04[] = { 0x7e, 0x98, 0xff, 0xff, 0xdf, 0xdf, 0x36, 0x00 };
uint8_t cc_bitarray_0a41[] = { 0x3c, 0xe3, 0xff, 0x00 };
uint8_t cc_bitarray_0a59[] = { 0xd0, 0x1f };
uint8_t cc_bitarray_0a84[] = { 0xfe, 0xbb, 0xff, 0xff, 0xdf, 0xdf, 0x3e, 0x00 };
uint8_t cc_bitarray_0ace[] = { 0x04, 0x00, 0x0c };
uint8_t cc_bitarray_0b04[] = { 0xfe, 0x99, 0xff, 0xff, 0xdf, 0xdf, 0x3e, 0x00 };
uint8_t cc_bitarray_0b3e[] = { 0x3a };
uint8_t cc_bitarray_0b44[] = { 0x98, 0x01 };
uint8_t cc_bitarray_0b58[] = { 0xb0, 0x03 };
uint8_t cc_bitarray_0b83[] = { 0x02, 0x47, 0x38, 0xe5, 0x1c, 0x07, 0x88, 0x07 };
uint8_t cc_bitarray_0bc1[] = { 0x1c, 0x01 };
uint8_t cc_bitarray_0c04[] = { 0xfe, 0xdd, 0xff, 0xff, 0xdf, 0xff, 0x3e, 0x00 };
uint8_t cc_bitarray_0c45[] = { 0xee, 0x01, 0x03, 0x00 };
uint8_t cc_bitarray_0c84[] = { 0xfe, 0xdd, 0xff, 0xff, 0xdf, 0xff, 0x3e, 0x00 };
uint8_t cc_bitarray_0cd7[] = { 0x80, 0x06 };
uint8_t cc_bitarray_0d04[] = { 0xfe, 0xdd, 0xff, 0xff, 0xdf, 0xff, 0x3f, 0x00 };
uint8_t cc_bitarray_0d44[] = { 0xdc, 0x01 };
uint8_t cc_bitarray_0d84[] = { 0xfe, 0xff, 0xc7, 0xff, 0xff, 0xbf, 0xff, 0xf2, 0x07 };
uint8_t cc_bitarray_0dd2[] = { 0x28 };
uint8_t cc_bitarray_0e01[] = { 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0xf9, 0x03 };
uint8_t cc_bitarray_0e5c[] = { 0x00, 0x00, 0x00, 0x00, 0x60, 0x59, 0x02, 0xef, 0xef,
                               0xca, 0x1e };
uint8_t cc_bitarray_0ebd[] = { 0x06, 0x01 };
uint8_t cc_bitarray_0eda[] = { 0x0c, 0x00, 0x00, 0x00, 0x40 };
uint8_t cc_bitarray_0f34[] = { 0x2a };
uint8_t cc_bitarray_0f3a[] = { 0x0a };
uint8_t cc_bitarray_0f40[] = { 0x00, 0x01, 0x00, 0x00, 0x00, 0xf8, 0x01 };
uint8_t cc_bitarray_0f8c[] = { 0xf0, 0xef, 0xff, 0xff, 0xff, 0xff, 0x01 };
uint8_t cc_bitarray_0fc7[] = { 0xc0, 0xfe, 0xff, 0xff, 0xff, 0xff, 0xff, 0x01 };
uint8_t cc_bitarray_1000[] = { 0x00, 0x00, 0x00, 0x00, 0x04, 0x09 };
uint8_t cc_bitarray_102c[] = { 0x5e };
uint8_t cc_bitarray_10fc[] = { 0xf0, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff,
                               0xff, 0xff, 0x3f, 0xf8, 0xff, 0xff, 0xff, 0xff, 0xff,
                               0xff, 0xff, 0x7f, 0xf0, 0xff, 0xff, 0xff, 0xff, 0xff,
                               0xff, 0xff, 0xff, 0xff, 0x3f, 0xf0, 0xf7, 0xff, 0xff,
                               0xff, 0xff, 0xff, 0xff, 0xff, 0xd7, 0xf3, 0xd7, 0xf3,
                               0xff, 0xff, 0xff, 0xff, 0xd7, 0xf3, 0xff, 0xff, 0xff,
                               0xd7, 0xf3, 0xd7, 0xf3, 0xf7, 0xf7, 0xff, 0xff, 0xf7,
                               0xff, 0xff, 0xff, 0xd7, 0xf3, 0xf7, 0xff, 0xff, 0xff,
                               0xff, 0xf7, 0xff, 0x7f, 0x00 };
uint8_t cc_bitarray_16f1[] = { 0x00, 0x80, 0xff, 0xef, 0x01 };
uint8_t cc_bitarray_1754[] = { 0x00, 0xf0, 0xff, 0x1d };
uint8_t cc_bitarray_17b6[] = { 0xfe, 0x00, 0xf9, 0x3f };
uint8_t cc_bitarray_1920[] = { 0x78, 0x0e };
uint8_t cc_bitarray_1930[] = { 0x04, 0x0e };
uint8_t cc_bitarray_1950[] = { 0x00, 0x00, 0x00, 0xc0, 0xe0, 0xff, 0xff, 0xff, 0xff,
                               0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff,
                               0x00 };
uint8_t cc_bitarray_1e00[] = { 0xaa, 0xaa, 0xaa, 0xaa, 0xaa, 0xaa, 0xaa, 0xaa, 0xaa,
                               0xaa, 0xaa, 0xaa, 0xaa, 0xaa, 0xaa, 0xaa, 0xaa, 0xaa,
                               0xea, 0x0f };
uint8_t cc_bitarray_1ea0[] = { 0xaa, 0xaa, 0xaa, 0xaa, 0xaa, 0xaa, 0xaa, 0xaa, 0xaa,
                               0xaa, 0xaa, 0x02 };
uint8_t cc_bitarray_1f20[] = { 0x00, 0xff, 0x00, 0xff, 0x00 };
uint8_t cc_bitarray_1f58[] = { 0xaa, 0x00 };
uint8_t cc_bitarray_1f80[] = { 0x00, 0xff, 0x00, 0xff, 0x00, 0xff, 0x00 };
uint8_t cc_bitarray_1fbd[] = { 0xe2, 0x00 };
uint8_t cc_bitarray_1ff0[] = { 0xdc, 0x00 };
uint8_t cc_bitarray_20d0[] = { 0x00, 0xe0, 0x1d, 0x00 };
uint8_t cc_bitarray_2100[] = { 0x84, 0x00 };
uint8_t cc_bitarray_210a[] = { 0xce, 0x01 };
uint8_t cc_bitarray_2114[] = { 0xe2, 0x03, 0xd5, 0x03 };
uint8_t cc_bitarray_2190[] = { 0xe0, 0xf3, 0xb6, 0xbf, 0xff, 0xff, 0xff, 0x3f, 0xeb,
                               0xff, 0xff, 0xff, 0x0f };
uint8_t cc_bitarray_2300[] = { 0x00, 0x0f, 0x00, 0x00, 0x03, 0x00 };
uint8_t cc_bitarray_232b[] = { 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
                               0x00, 0x02, 0x00, 0x00, 0x00, 0xff, 0xff, 0xff, 0x01 };
uint8_t cc_bitarray_23b7[] = { 0x00, 0x00, 0x00, 0xfc, 0xff, 0xff, 0xff, 0xff, 0xff,
                               0x01, 0x00, 0x00, 0x00, 0x00, 0xff, 0xff, 0xff, 0x01,
                               0xf0, 0xff, 0xff, 0x01 };
uint8_t cc_bitarray_25b7[] = { 0xfe, 0xfb, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0x01,
                               0xfe, 0xff, 0xff, 0x01 };
uint8_t cc_bitarray_2670[] = { 0x00, 0xc0, 0x00, 0x00, 0xfc, 0xff, 0xfc, 0xff, 0xff,
                               0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff,
                               0x21, 0x0c, 0x00, 0x00, 0x00, 0x01, 0x00, 0x00, 0x00,
                               0x50, 0xb8, 0x80, 0x01, 0x00 };
uint8_t cc_bitarray_2768[] = { 0xaa, 0x2a };
uint8_t cc_bitarray_2794[] = { 0x0e, 0x00, 0x00, 0x10, 0x00, 0xf8, 0xff, 0x0f };
uint8_t cc_bitarray_27e6[] = { 0x2a };
uint8_t cc_bitarray_2983[] = { 0xaa, 0xaa, 0x2a };
uint8_t cc_bitarray_29d8[] = { 0x0a };
uint8_t cc_bitarray_2e80[] = { 0x00, 0x00, 0x00, 0x04, 0x00, 0x00, 0x00, 0x00, 0x00,
                               0x00, 0x00, 0x00, 0x00, 0x00, 0xf0, 0xff, 0x00 };
uint8_t cc_bitarray_3008[] = { 0xaa, 0x02 };
uint8_t cc_bitarray_3014[] = { 0xaa, 0x00 };
uint8_t cc_bitarray_30ff[] = { 0x3e, 0x00, 0x00, 0x00, 0x00, 0xc0, 0x03, 0x00, 0x00,
                               0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
                               0x01 };
uint8_t cc_bitarray_327f[] = { 0xfe, 0x07, 0x00, 0x00, 0x00, 0x00, 0xfc, 0xff, 0x01,
                               0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00 };
uint8_t cc_bitarray_fb00[] = { 0x80, 0xff, 0x07, 0x1f };
uint8_t cc_bitarray_fb2a[] = { 0x00, 0x20, 0x28, 0x09, 0x00, 0x00, 0x00, 0x00, 0x00,
                               0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0xff,
                               0xff, 0xff, 0xff, 0x01 };
uint8_t cc_bitarray_fd40[] = { 0x00, 0x00, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff,
                               0xff, 0xfc, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0x00,
                               0x00, 0x00, 0x00, 0x00, 0xff, 0x0f };
uint8_t cc_bitarray_fdfe[] = { 0xfc, 0xff, 0x03, 0x00, 0x3c, 0x00, 0x00 };
uint8_t cc_bitarray_fe35[] = { 0xaa, 0xaa, 0x00 };
uint8_t cc_bitarray_fe59[] = { 0x2a };
uint8_t cc_bitarray_ff66[] = { 0x00, 0x04, 0x00, 0x00, 0x00, 0x00, 0x00, 0x03, 0x00,
                               0x00, 0x00, 0x00 };
uint8_t cc_bitarray_ffbf[] = { 0xf8, 0xf9, 0xf9, 0x39, 0x00 };

CharCategoryInfo gCharCategoryTable[] = {
    { 0x0000, kCCControlChar,           kCCControlChar,             NULL },
    { 0x0020, kCCSpaceSeparator,        kCCSpaceSeparator,          NULL },
    { 0x0021, kCCOtherPunctuation,      kCCOtherPunctuation,        NULL },
    { 0x0024, kCCCurrencySymbol,        kCCCurrencySymbol,          NULL },
    { 0x0025, kCCOtherPunctuation,      kCCOtherPunctuation,        NULL },
    { 0x0028, kCCStartPunctuation,      kCCStartPunctuation,        NULL },
    { 0x0029, kCCEndPunctuation,        kCCEndPunctuation,          NULL },
    { 0x002a, kCCOtherPunctuation,      kCCOtherPunctuation,        NULL },
    { 0x002b, kCCMathSymbol,            kCCMathSymbol,              NULL },
    { 0x002c, kCCOtherPunctuation,      kCCOtherPunctuation,        NULL },
    { 0x002d, kCCDashPunctuation,       kCCDashPunctuation,         NULL },
    { 0x002e, kCCOtherPunctuation,      kCCOtherPunctuation,        NULL },
    { 0x0030, kCCDecimalDigitNumber,    kCCDecimalDigitNumber,      NULL },
    { 0x003a, kCCOtherPunctuation,      kCCOtherPunctuation,        NULL },
    { 0x003c, kCCMathSymbol,            kCCMathSymbol,              NULL },
    { 0x003f, kCCOtherPunctuation,      kCCOtherPunctuation,        NULL },
    { 0x0041, kCCUppercaseLetter,       kCCUppercaseLetter,         NULL },
    { 0x005b, kCCStartPunctuation,      kCCStartPunctuation,        NULL },
    { 0x005c, kCCOtherPunctuation,      kCCOtherPunctuation,        NULL },
    { 0x005d, kCCEndPunctuation,        kCCEndPunctuation,          NULL },
    { 0x005e, kCCModifierSymbol,        kCCModifierSymbol,          NULL },
    { 0x005f, kCCConnectorPunctuation,  kCCConnectorPunctuation,    NULL },
    { 0x0060, kCCModifierSymbol,        kCCModifierSymbol,          NULL },
    { 0x0061, kCCLowercaseLetter,       kCCLowercaseLetter,         NULL },
    { 0x007b, kCCStartPunctuation,      kCCStartPunctuation,        NULL },
    { 0x007c, kCCMathSymbol,            kCCMathSymbol,              NULL },
    { 0x007d, kCCEndPunctuation,        kCCEndPunctuation,          NULL },
    { 0x007e, kCCMathSymbol,            kCCMathSymbol,              NULL },
    { 0x007f, kCCControlChar,           kCCControlChar,             NULL },
    { 0x00a0, kCCSpaceSeparator,        kCCSpaceSeparator,          NULL },
    { 0x00a1, kCCOtherPunctuation,      kCCOtherPunctuation,        NULL },
    { 0x00a2, kCCCurrencySymbol,        kCCCurrencySymbol,          NULL },
    { 0x00a6, kCCOtherSymbol,           kCCOtherSymbol,             NULL },
    { 0x00a8, kCCModifierSymbol,        kCCModifierSymbol,          NULL },
    { 0x00a9, kCCOtherSymbol,           kCCOtherSymbol,             NULL },
    { 0x00aa, kCCLowercaseLetter,       kCCLowercaseLetter,         NULL },
    { 0x00ab, kCCInitialPunctuation,    kCCInitialPunctuation,      NULL },
    { 0x00ac, kCCMathSymbol,            kCCMathSymbol,              NULL },
    { 0x00ad, kCCFormatChar,            kCCFormatChar,              NULL },
    { 0x00ae, kCCOtherSymbol,           kCCOtherSymbol,             NULL },
    { 0x00af, kCCModifierSymbol,        kCCModifierSymbol,          NULL },
    { 0x00b0, kCCOtherSymbol,           kCCOtherSymbol,             NULL },
    { 0x00b1, kCCMathSymbol,            kCCMathSymbol,              NULL },
    { 0x00b2, kCCOtherNumber,           kCCOtherNumber,             NULL },
    { 0x00b4, kCCModifierSymbol,        kCCModifierSymbol,          NULL },
    { 0x00b5, kCCLowercaseLetter,       kCCLowercaseLetter,         NULL },
    { 0x00b6, kCCOtherSymbol,           kCCOtherSymbol,             NULL },
    { 0x00b7, kCCOtherPunctuation,      kCCOtherPunctuation,        NULL },
    { 0x00b8, kCCModifierSymbol,        kCCModifierSymbol,          NULL },
    { 0x00b9, kCCOtherNumber,           kCCOtherNumber,             NULL },
    { 0x00ba, kCCLowercaseLetter,       kCCLowercaseLetter,         NULL },
    { 0x00bb, kCCFinalPunctuation,      kCCFinalPunctuation,        NULL },
    { 0x00bc, kCCOtherNumber,           kCCOtherNumber,             NULL },
    { 0x00bf, kCCOtherPunctuation,      kCCOtherPunctuation,        NULL },
    { 0x00c0, kCCUppercaseLetter,       kCCUppercaseLetter,         NULL },
    { 0x00d7, kCCMathSymbol,            kCCMathSymbol,              NULL },
    { 0x00d8, kCCUppercaseLetter,       kCCUppercaseLetter,         NULL },
    { 0x00df, kCCLowercaseLetter,       kCCLowercaseLetter,         NULL },
    { 0x00f7, kCCMathSymbol,            kCCMathSymbol,              NULL },
    { 0x00f8, kCCLowercaseLetter,       kCCUppercaseLetter,         cc_bitarray_00f8 },
    { 0x01bb, kCCOtherLetter,           kCCOtherLetter,             NULL },
    { 0x01bc, kCCUppercaseLetter,       kCCUppercaseLetter,         NULL },
    { 0x01bd, kCCLowercaseLetter,       kCCLowercaseLetter,         NULL },
    { 0x01c0, kCCOtherLetter,           kCCOtherLetter,             NULL },
    { 0x01c4, kCCUppercaseLetter,       kCCUppercaseLetter,         NULL },
    { 0x01c5, kCCTitlecaseLetter,       kCCTitlecaseLetter,         NULL },
    { 0x01c6, kCCLowercaseLetter,       kCCLowercaseLetter,         NULL },
    { 0x01c7, kCCUppercaseLetter,       kCCUppercaseLetter,         NULL },
    { 0x01c8, kCCTitlecaseLetter,       kCCTitlecaseLetter,         NULL },
    { 0x01c9, kCCLowercaseLetter,       kCCLowercaseLetter,         NULL },
    { 0x01ca, kCCUppercaseLetter,       kCCUppercaseLetter,         NULL },
    { 0x01cb, kCCTitlecaseLetter,       kCCTitlecaseLetter,         NULL },
    { 0x01cc, kCCLowercaseLetter,       kCCUppercaseLetter,         cc_bitarray_01cc },
    { 0x01f2, kCCTitlecaseLetter,       kCCTitlecaseLetter,         NULL },
    { 0x01f3, kCCLowercaseLetter,       kCCUppercaseLetter,         cc_bitarray_01f3 },
    { 0x0237, kCCUnassigned,            kCCUnassigned,              NULL },
    { 0x0250, kCCLowercaseLetter,       kCCLowercaseLetter,         NULL },
    { 0x02b0, kCCModifierLetter,        kCCModifierSymbol,          cc_bitarray_02b0 },
    { 0x0300, kCCNonSpacingMark,        kCCUnassigned,              cc_bitarray_0300 },
    { 0x0374, kCCModifierSymbol,        kCCModifierSymbol,          NULL },
    { 0x0376, kCCUnassigned,            kCCUnassigned,              NULL },
    { 0x037a, kCCModifierLetter,        kCCModifierLetter,          NULL },
    { 0x037b, kCCUnassigned,            kCCUnassigned,              NULL },
    { 0x037e, kCCOtherPunctuation,      kCCOtherPunctuation,        NULL },
    { 0x037f, kCCUnassigned,            kCCUnassigned,              NULL },
    { 0x0384, kCCModifierSymbol,        kCCModifierSymbol,          NULL },
    { 0x0386, kCCUppercaseLetter,       kCCUppercaseLetter,         NULL },
    { 0x0387, kCCOtherPunctuation,      kCCOtherPunctuation,        NULL },
    { 0x0388, kCCUppercaseLetter,       kCCUnassigned,              cc_bitarray_0388 },
    { 0x0390, kCCLowercaseLetter,       kCCLowercaseLetter,         NULL },
    { 0x0391, kCCUppercaseLetter,       kCCUppercaseLetter,         NULL },
    { 0x03a2, kCCUnassigned,            kCCUnassigned,              NULL },
    { 0x03a3, kCCUppercaseLetter,       kCCUppercaseLetter,         NULL },
    { 0x03ac, kCCLowercaseLetter,       kCCLowercaseLetter,         NULL },
    { 0x03cf, kCCUnassigned,            kCCUnassigned,              NULL },
    { 0x03d0, kCCLowercaseLetter,       kCCUppercaseLetter,         cc_bitarray_03d0 },
    { 0x03f6, kCCMathSymbol,            kCCMathSymbol,              NULL },
    { 0x03f7, kCCUppercaseLetter,       kCCLowercaseLetter,         cc_bitarray_03f7 },
    { 0x03fc, kCCUnassigned,            kCCUnassigned,              NULL },
    { 0x0400, kCCUppercaseLetter,       kCCLowercaseLetter,         cc_bitarray_0400 },
    { 0x0482, kCCOtherSymbol,           kCCOtherSymbol,             NULL },
    { 0x0483, kCCNonSpacingMark,        kCCNonSpacingMark,          NULL },
    { 0x0487, kCCUnassigned,            kCCUnassigned,              NULL },
    { 0x0488, kCCEnclosingMark,         kCCEnclosingMark,           NULL },
    { 0x048a, kCCUppercaseLetter,       kCCLowercaseLetter,         cc_bitarray_048a },
    { 0x04cf, kCCUnassigned,            kCCUnassigned,              NULL },
    { 0x04d0, kCCUppercaseLetter,       kCCLowercaseLetter,         cc_bitarray_04d0 },
    { 0x04f6, kCCUnassigned,            kCCUnassigned,              NULL },
    { 0x04f8, kCCUppercaseLetter,       kCCUppercaseLetter,         NULL },
    { 0x04f9, kCCLowercaseLetter,       kCCLowercaseLetter,         NULL },
    { 0x04fa, kCCUnassigned,            kCCUnassigned,              NULL },
    { 0x0500, kCCUppercaseLetter,       kCCLowercaseLetter,         cc_bitarray_0500 },
    { 0x0510, kCCUnassigned,            kCCUnassigned,              NULL },
    { 0x0531, kCCUppercaseLetter,       kCCUppercaseLetter,         NULL },
    { 0x0557, kCCUnassigned,            kCCUnassigned,              NULL },
    { 0x0559, kCCModifierLetter,        kCCModifierLetter,          NULL },
    { 0x055a, kCCOtherPunctuation,      kCCOtherPunctuation,        NULL },
    { 0x0560, kCCUnassigned,            kCCUnassigned,              NULL },
    { 0x0561, kCCLowercaseLetter,       kCCLowercaseLetter,         NULL },
    { 0x0588, kCCUnassigned,            kCCUnassigned,              NULL },
    { 0x0589, kCCOtherPunctuation,      kCCOtherPunctuation,        NULL },
    { 0x058a, kCCDashPunctuation,       kCCDashPunctuation,         NULL },
    { 0x058b, kCCUnassigned,            kCCNonSpacingMark,          cc_bitarray_058b },
    { 0x05be, kCCOtherPunctuation,      kCCNonSpacingMark,          cc_bitarray_05be },
    { 0x05c5, kCCUnassigned,            kCCOtherLetter,             cc_bitarray_05c5 },
    { 0x05f3, kCCOtherPunctuation,      kCCOtherPunctuation,        NULL },
    { 0x05f5, kCCUnassigned,            kCCUnassigned,              NULL },
    { 0x0600, kCCFormatChar,            kCCFormatChar,              NULL },
    { 0x0604, kCCUnassigned,            kCCUnassigned,              NULL },
    { 0x060c, kCCOtherPunctuation,      kCCOtherPunctuation,        NULL },
    { 0x060e, kCCOtherSymbol,           kCCOtherSymbol,             NULL },
    { 0x0610, kCCNonSpacingMark,        kCCNonSpacingMark,          NULL },
    { 0x0616, kCCUnassigned,            kCCOtherPunctuation,        cc_bitarray_0616 },
    { 0x0621, kCCOtherLetter,           kCCOtherLetter,             NULL },
    { 0x063b, kCCUnassigned,            kCCUnassigned,              NULL },
    { 0x0640, kCCModifierLetter,        kCCModifierLetter,          NULL },
    { 0x0641, kCCOtherLetter,           kCCOtherLetter,             NULL },
    { 0x064b, kCCNonSpacingMark,        kCCNonSpacingMark,          NULL },
    { 0x0659, kCCUnassigned,            kCCUnassigned,              NULL },
    { 0x0660, kCCDecimalDigitNumber,    kCCDecimalDigitNumber,      NULL },
    { 0x066a, kCCOtherPunctuation,      kCCOtherPunctuation,        NULL },
    { 0x066e, kCCOtherLetter,           kCCOtherLetter,             NULL },
    { 0x0670, kCCNonSpacingMark,        kCCNonSpacingMark,          NULL },
    { 0x0671, kCCOtherLetter,           kCCOtherLetter,             NULL },
    { 0x06d4, kCCOtherPunctuation,      kCCOtherPunctuation,        NULL },
    { 0x06d5, kCCOtherLetter,           kCCOtherLetter,             NULL },
    { 0x06d6, kCCNonSpacingMark,        kCCNonSpacingMark,          NULL },
    { 0x06dd, kCCFormatChar,            kCCFormatChar,              NULL },
    { 0x06de, kCCEnclosingMark,         kCCEnclosingMark,           NULL },
    { 0x06df, kCCNonSpacingMark,        kCCNonSpacingMark,          NULL },
    { 0x06e5, kCCModifierLetter,        kCCModifierLetter,          NULL },
    { 0x06e7, kCCNonSpacingMark,        kCCNonSpacingMark,          NULL },
    { 0x06e9, kCCOtherSymbol,           kCCOtherSymbol,             NULL },
    { 0x06ea, kCCNonSpacingMark,        kCCNonSpacingMark,          NULL },
    { 0x06ee, kCCOtherLetter,           kCCOtherLetter,             NULL },
    { 0x06f0, kCCDecimalDigitNumber,    kCCDecimalDigitNumber,      NULL },
    { 0x06fa, kCCOtherLetter,           kCCOtherLetter,             NULL },
    { 0x06fd, kCCOtherSymbol,           kCCOtherSymbol,             NULL },
    { 0x06ff, kCCOtherLetter,           kCCOtherLetter,             NULL },
    { 0x0700, kCCOtherPunctuation,      kCCOtherPunctuation,        NULL },
    { 0x070e, kCCUnassigned,            kCCUnassigned,              NULL },
    { 0x070f, kCCFormatChar,            kCCFormatChar,              NULL },
    { 0x0710, kCCOtherLetter,           kCCNonSpacingMark,          cc_bitarray_0710 },
    { 0x074b, kCCUnassigned,            kCCOtherLetter,             cc_bitarray_074b },
    { 0x07a6, kCCNonSpacingMark,        kCCNonSpacingMark,          NULL },
    { 0x07b1, kCCOtherLetter,           kCCOtherLetter,             NULL },
    { 0x07b2, kCCUnassigned,            kCCUnassigned,              NULL },
    { 0x0901, kCCNonSpacingMark,        kCCNonSpacingMark,          NULL },
    { 0x0903, kCCCombiningSpacingMark,  kCCCombiningSpacingMark,    NULL },
    { 0x0904, kCCOtherLetter,           kCCOtherLetter,             NULL },
    { 0x093a, kCCUnassigned,            kCCUnassigned,              NULL },
    { 0x093c, kCCNonSpacingMark,        kCCNonSpacingMark,          NULL },
    { 0x093d, kCCOtherLetter,           kCCOtherLetter,             NULL },
    { 0x093e, kCCCombiningSpacingMark,  kCCNonSpacingMark,          cc_bitarray_093e },
    { 0x094e, kCCUnassigned,            kCCUnassigned,              NULL },
    { 0x0950, kCCOtherLetter,           kCCOtherLetter,             NULL },
    { 0x0951, kCCNonSpacingMark,        kCCNonSpacingMark,          NULL },
    { 0x0955, kCCUnassigned,            kCCUnassigned,              NULL },
    { 0x0958, kCCOtherLetter,           kCCOtherLetter,             NULL },
    { 0x0962, kCCNonSpacingMark,        kCCNonSpacingMark,          NULL },
    { 0x0964, kCCOtherPunctuation,      kCCOtherPunctuation,        NULL },
    { 0x0966, kCCDecimalDigitNumber,    kCCDecimalDigitNumber,      NULL },
    { 0x0970, kCCOtherPunctuation,      kCCOtherPunctuation,        NULL },
    { 0x0971, kCCUnassigned,            kCCUnassigned,              NULL },
    { 0x0981, kCCNonSpacingMark,        kCCNonSpacingMark,          NULL },
    { 0x0982, kCCCombiningSpacingMark,  kCCCombiningSpacingMark,    NULL },
    { 0x0984, kCCUnassigned,            kCCOtherLetter,             cc_bitarray_0984 },
    { 0x09bc, kCCNonSpacingMark,        kCCNonSpacingMark,          NULL },
    { 0x09bd, kCCOtherLetter,           kCCOtherLetter,             NULL },
    { 0x09be, kCCCombiningSpacingMark,  kCCCombiningSpacingMark,    NULL },
    { 0x09c1, kCCNonSpacingMark,        kCCNonSpacingMark,          NULL },
    { 0x09c5, kCCUnassigned,            kCCCombiningSpacingMark,    cc_bitarray_09c5 },
    { 0x09cd, kCCNonSpacingMark,        kCCNonSpacingMark,          NULL },
    { 0x09ce, kCCUnassigned,            kCCUnassigned,              NULL },
    { 0x09d7, kCCCombiningSpacingMark,  kCCCombiningSpacingMark,    NULL },
    { 0x09d8, kCCUnassigned,            kCCOtherLetter,             cc_bitarray_09d8 },
    { 0x09e2, kCCNonSpacingMark,        kCCNonSpacingMark,          NULL },
    { 0x09e4, kCCUnassigned,            kCCUnassigned,              NULL },
    { 0x09e6, kCCDecimalDigitNumber,    kCCDecimalDigitNumber,      NULL },
    { 0x09f0, kCCOtherLetter,           kCCOtherLetter,             NULL },
    { 0x09f2, kCCCurrencySymbol,        kCCCurrencySymbol,          NULL },
    { 0x09f4, kCCOtherNumber,           kCCOtherNumber,             NULL },
    { 0x09fa, kCCOtherSymbol,           kCCOtherSymbol,             NULL },
    { 0x09fb, kCCUnassigned,            kCCUnassigned,              NULL },
    { 0x0a01, kCCNonSpacingMark,        kCCNonSpacingMark,          NULL },
    { 0x0a03, kCCCombiningSpacingMark,  kCCCombiningSpacingMark,    NULL },
    { 0x0a04, kCCUnassigned,            kCCOtherLetter,             cc_bitarray_0a04 },
    { 0x0a3c, kCCNonSpacingMark,        kCCNonSpacingMark,          NULL },
    { 0x0a3d, kCCUnassigned,            kCCUnassigned,              NULL },
    { 0x0a3e, kCCCombiningSpacingMark,  kCCCombiningSpacingMark,    NULL },
    { 0x0a41, kCCNonSpacingMark,        kCCUnassigned,              cc_bitarray_0a41 },
    { 0x0a59, kCCOtherLetter,           kCCUnassigned,              cc_bitarray_0a59 },
    { 0x0a66, kCCDecimalDigitNumber,    kCCDecimalDigitNumber,      NULL },
    { 0x0a70, kCCNonSpacingMark,        kCCNonSpacingMark,          NULL },
    { 0x0a72, kCCOtherLetter,           kCCOtherLetter,             NULL },
    { 0x0a75, kCCUnassigned,            kCCUnassigned,              NULL },
    { 0x0a81, kCCNonSpacingMark,        kCCNonSpacingMark,          NULL },
    { 0x0a83, kCCCombiningSpacingMark,  kCCCombiningSpacingMark,    NULL },
    { 0x0a84, kCCUnassigned,            kCCOtherLetter,             cc_bitarray_0a84 },
    { 0x0abc, kCCNonSpacingMark,        kCCNonSpacingMark,          NULL },
    { 0x0abd, kCCOtherLetter,           kCCOtherLetter,             NULL },
    { 0x0abe, kCCCombiningSpacingMark,  kCCCombiningSpacingMark,    NULL },
    { 0x0ac1, kCCNonSpacingMark,        kCCNonSpacingMark,          NULL },
    { 0x0ac6, kCCUnassigned,            kCCUnassigned,              NULL },
    { 0x0ac7, kCCNonSpacingMark,        kCCNonSpacingMark,          NULL },
    { 0x0ac9, kCCCombiningSpacingMark,  kCCCombiningSpacingMark,    NULL },
    { 0x0aca, kCCUnassigned,            kCCUnassigned,              NULL },
    { 0x0acb, kCCCombiningSpacingMark,  kCCCombiningSpacingMark,    NULL },
    { 0x0acd, kCCNonSpacingMark,        kCCNonSpacingMark,          NULL },
    { 0x0ace, kCCUnassigned,            kCCOtherLetter,             cc_bitarray_0ace },
    { 0x0ae2, kCCNonSpacingMark,        kCCNonSpacingMark,          NULL },
    { 0x0ae4, kCCUnassigned,            kCCUnassigned,              NULL },
    { 0x0ae6, kCCDecimalDigitNumber,    kCCDecimalDigitNumber,      NULL },
    { 0x0af0, kCCUnassigned,            kCCUnassigned,              NULL },
    { 0x0af1, kCCCurrencySymbol,        kCCCurrencySymbol,          NULL },
    { 0x0af2, kCCUnassigned,            kCCUnassigned,              NULL },
    { 0x0b01, kCCNonSpacingMark,        kCCNonSpacingMark,          NULL },
    { 0x0b02, kCCCombiningSpacingMark,  kCCCombiningSpacingMark,    NULL },
    { 0x0b04, kCCUnassigned,            kCCOtherLetter,             cc_bitarray_0b04 },
    { 0x0b3c, kCCNonSpacingMark,        kCCNonSpacingMark,          NULL },
    { 0x0b3d, kCCOtherLetter,           kCCOtherLetter,             NULL },
    { 0x0b3e, kCCCombiningSpacingMark,  kCCNonSpacingMark,          cc_bitarray_0b3e },
    { 0x0b44, kCCUnassigned,            kCCCombiningSpacingMark,    cc_bitarray_0b44 },
    { 0x0b4d, kCCNonSpacingMark,        kCCNonSpacingMark,          NULL },
    { 0x0b4e, kCCUnassigned,            kCCUnassigned,              NULL },
    { 0x0b56, kCCNonSpacingMark,        kCCNonSpacingMark,          NULL },
    { 0x0b57, kCCCombiningSpacingMark,  kCCCombiningSpacingMark,    NULL },
    { 0x0b58, kCCUnassigned,            kCCOtherLetter,             cc_bitarray_0b58 },
    { 0x0b66, kCCDecimalDigitNumber,    kCCDecimalDigitNumber,      NULL },
    { 0x0b70, kCCOtherSymbol,           kCCOtherSymbol,             NULL },
    { 0x0b71, kCCOtherLetter,           kCCOtherLetter,             NULL },
    { 0x0b72, kCCUnassigned,            kCCUnassigned,              NULL },
    { 0x0b82, kCCNonSpacingMark,        kCCNonSpacingMark,          NULL },
    { 0x0b83, kCCOtherLetter,           kCCUnassigned,              cc_bitarray_0b83 },
    { 0x0bbe, kCCCombiningSpacingMark,  kCCCombiningSpacingMark,    NULL },
    { 0x0bc0, kCCNonSpacingMark,        kCCNonSpacingMark,          NULL },
    { 0x0bc1, kCCCombiningSpacingMark,  kCCUnassigned,              cc_bitarray_0bc1 },
    { 0x0bcd, kCCNonSpacingMark,        kCCNonSpacingMark,          NULL },
    { 0x0bce, kCCUnassigned,            kCCUnassigned,              NULL },
    { 0x0bd7, kCCCombiningSpacingMark,  kCCCombiningSpacingMark,    NULL },
    { 0x0bd8, kCCUnassigned,            kCCUnassigned,              NULL },
    { 0x0be7, kCCDecimalDigitNumber,    kCCDecimalDigitNumber,      NULL },
    { 0x0bf0, kCCOtherNumber,           kCCOtherNumber,             NULL },
    { 0x0bf3, kCCOtherSymbol,           kCCOtherSymbol,             NULL },
    { 0x0bf9, kCCCurrencySymbol,        kCCCurrencySymbol,          NULL },
    { 0x0bfa, kCCOtherSymbol,           kCCOtherSymbol,             NULL },
    { 0x0bfb, kCCUnassigned,            kCCUnassigned,              NULL },
    { 0x0c01, kCCCombiningSpacingMark,  kCCCombiningSpacingMark,    NULL },
    { 0x0c04, kCCUnassigned,            kCCOtherLetter,             cc_bitarray_0c04 },
    { 0x0c3e, kCCNonSpacingMark,        kCCNonSpacingMark,          NULL },
    { 0x0c41, kCCCombiningSpacingMark,  kCCCombiningSpacingMark,    NULL },
    { 0x0c45, kCCUnassigned,            kCCNonSpacingMark,          cc_bitarray_0c45 },
    { 0x0c60, kCCOtherLetter,           kCCOtherLetter,             NULL },
    { 0x0c62, kCCUnassigned,            kCCUnassigned,              NULL },
    { 0x0c66, kCCDecimalDigitNumber,    kCCDecimalDigitNumber,      NULL },
    { 0x0c70, kCCUnassigned,            kCCUnassigned,              NULL },
    { 0x0c82, kCCCombiningSpacingMark,  kCCCombiningSpacingMark,    NULL },
    { 0x0c84, kCCUnassigned,            kCCOtherLetter,             cc_bitarray_0c84 },
    { 0x0cbc, kCCNonSpacingMark,        kCCNonSpacingMark,          NULL },
    { 0x0cbd, kCCOtherLetter,           kCCOtherLetter,             NULL },
    { 0x0cbe, kCCCombiningSpacingMark,  kCCCombiningSpacingMark,    NULL },
    { 0x0cbf, kCCNonSpacingMark,        kCCNonSpacingMark,          NULL },
    { 0x0cc0, kCCCombiningSpacingMark,  kCCCombiningSpacingMark,    NULL },
    { 0x0cc5, kCCUnassigned,            kCCUnassigned,              NULL },
    { 0x0cc6, kCCNonSpacingMark,        kCCNonSpacingMark,          NULL },
    { 0x0cc7, kCCCombiningSpacingMark,  kCCCombiningSpacingMark,    NULL },
    { 0x0cc9, kCCUnassigned,            kCCUnassigned,              NULL },
    { 0x0cca, kCCCombiningSpacingMark,  kCCCombiningSpacingMark,    NULL },
    { 0x0ccc, kCCNonSpacingMark,        kCCNonSpacingMark,          NULL },
    { 0x0cce, kCCUnassigned,            kCCUnassigned,              NULL },
    { 0x0cd5, kCCCombiningSpacingMark,  kCCCombiningSpacingMark,    NULL },
    { 0x0cd7, kCCUnassigned,            kCCOtherLetter,             cc_bitarray_0cd7 },
    { 0x0ce6, kCCDecimalDigitNumber,    kCCDecimalDigitNumber,      NULL },
    { 0x0cf0, kCCUnassigned,            kCCUnassigned,              NULL },
    { 0x0d02, kCCCombiningSpacingMark,  kCCCombiningSpacingMark,    NULL },
    { 0x0d04, kCCUnassigned,            kCCOtherLetter,             cc_bitarray_0d04 },
    { 0x0d3e, kCCCombiningSpacingMark,  kCCCombiningSpacingMark,    NULL },
    { 0x0d41, kCCNonSpacingMark,        kCCNonSpacingMark,          NULL },
    { 0x0d44, kCCUnassigned,            kCCCombiningSpacingMark,    cc_bitarray_0d44 },
    { 0x0d4d, kCCNonSpacingMark,        kCCNonSpacingMark,          NULL },
    { 0x0d4e, kCCUnassigned,            kCCUnassigned,              NULL },
    { 0x0d57, kCCCombiningSpacingMark,  kCCCombiningSpacingMark,    NULL },
    { 0x0d58, kCCUnassigned,            kCCUnassigned,              NULL },
    { 0x0d60, kCCOtherLetter,           kCCOtherLetter,             NULL },
    { 0x0d62, kCCUnassigned,            kCCUnassigned,              NULL },
    { 0x0d66, kCCDecimalDigitNumber,    kCCDecimalDigitNumber,      NULL },
    { 0x0d70, kCCUnassigned,            kCCUnassigned,              NULL },
    { 0x0d82, kCCCombiningSpacingMark,  kCCCombiningSpacingMark,    NULL },
    { 0x0d84, kCCUnassigned,            kCCOtherLetter,             cc_bitarray_0d84 },
    { 0x0dca, kCCNonSpacingMark,        kCCNonSpacingMark,          NULL },
    { 0x0dcb, kCCUnassigned,            kCCUnassigned,              NULL },
    { 0x0dcf, kCCCombiningSpacingMark,  kCCCombiningSpacingMark,    NULL },
    { 0x0dd2, kCCNonSpacingMark,        kCCUnassigned,              cc_bitarray_0dd2 },
    { 0x0dd8, kCCCombiningSpacingMark,  kCCCombiningSpacingMark,    NULL },
    { 0x0de0, kCCUnassigned,            kCCUnassigned,              NULL },
    { 0x0df2, kCCCombiningSpacingMark,  kCCCombiningSpacingMark,    NULL },
    { 0x0df4, kCCOtherPunctuation,      kCCOtherPunctuation,        NULL },
    { 0x0df5, kCCUnassigned,            kCCUnassigned,              NULL },
    { 0x0e01, kCCOtherLetter,           kCCNonSpacingMark,          cc_bitarray_0e01 },
    { 0x0e3b, kCCUnassigned,            kCCUnassigned,              NULL },
    { 0x0e3f, kCCCurrencySymbol,        kCCCurrencySymbol,          NULL },
    { 0x0e40, kCCOtherLetter,           kCCOtherLetter,             NULL },
    { 0x0e46, kCCModifierLetter,        kCCModifierLetter,          NULL },
    { 0x0e47, kCCNonSpacingMark,        kCCNonSpacingMark,          NULL },
    { 0x0e4f, kCCOtherPunctuation,      kCCOtherPunctuation,        NULL },
    { 0x0e50, kCCDecimalDigitNumber,    kCCDecimalDigitNumber,      NULL },
    { 0x0e5a, kCCOtherPunctuation,      kCCOtherPunctuation,        NULL },
    { 0x0e5c, kCCUnassigned,            kCCOtherLetter,             cc_bitarray_0e5c },
    { 0x0eb1, kCCNonSpacingMark,        kCCNonSpacingMark,          NULL },
    { 0x0eb2, kCCOtherLetter,           kCCOtherLetter,             NULL },
    { 0x0eb4, kCCNonSpacingMark,        kCCNonSpacingMark,          NULL },
    { 0x0eba, kCCUnassigned,            kCCUnassigned,              NULL },
    { 0x0ebb, kCCNonSpacingMark,        kCCNonSpacingMark,          NULL },
    { 0x0ebd, kCCOtherLetter,           kCCUnassigned,              cc_bitarray_0ebd },
    { 0x0ec6, kCCModifierLetter,        kCCModifierLetter,          NULL },
    { 0x0ec7, kCCUnassigned,            kCCUnassigned,              NULL },
    { 0x0ec8, kCCNonSpacingMark,        kCCNonSpacingMark,          NULL },
    { 0x0ece, kCCUnassigned,            kCCUnassigned,              NULL },
    { 0x0ed0, kCCDecimalDigitNumber,    kCCDecimalDigitNumber,      NULL },
    { 0x0eda, kCCUnassigned,            kCCOtherLetter,             cc_bitarray_0eda },
    { 0x0f01, kCCOtherSymbol,           kCCOtherSymbol,             NULL },
    { 0x0f04, kCCOtherPunctuation,      kCCOtherPunctuation,        NULL },
    { 0x0f13, kCCOtherSymbol,           kCCOtherSymbol,             NULL },
    { 0x0f18, kCCNonSpacingMark,        kCCNonSpacingMark,          NULL },
    { 0x0f1a, kCCOtherSymbol,           kCCOtherSymbol,             NULL },
    { 0x0f20, kCCDecimalDigitNumber,    kCCDecimalDigitNumber,      NULL },
    { 0x0f2a, kCCOtherNumber,           kCCOtherNumber,             NULL },
    { 0x0f34, kCCOtherSymbol,           kCCNonSpacingMark,          cc_bitarray_0f34 },
    { 0x0f3a, kCCStartPunctuation,      kCCEndPunctuation,          cc_bitarray_0f3a },
    { 0x0f3e, kCCCombiningSpacingMark,  kCCCombiningSpacingMark,    NULL },
    { 0x0f40, kCCOtherLetter,           kCCUnassigned,              cc_bitarray_0f40 },
    { 0x0f71, kCCNonSpacingMark,        kCCNonSpacingMark,          NULL },
    { 0x0f7f, kCCCombiningSpacingMark,  kCCCombiningSpacingMark,    NULL },
    { 0x0f80, kCCNonSpacingMark,        kCCNonSpacingMark,          NULL },
    { 0x0f85, kCCOtherPunctuation,      kCCOtherPunctuation,        NULL },
    { 0x0f86, kCCNonSpacingMark,        kCCNonSpacingMark,          NULL },
    { 0x0f88, kCCOtherLetter,           kCCOtherLetter,             NULL },
    { 0x0f8c, kCCUnassigned,            kCCNonSpacingMark,          cc_bitarray_0f8c },
    { 0x0fbe, kCCOtherSymbol,           kCCOtherSymbol,             NULL },
    { 0x0fc6, kCCNonSpacingMark,        kCCNonSpacingMark,          NULL },
    { 0x0fc7, kCCOtherSymbol,           kCCUnassigned,              cc_bitarray_0fc7 },
    { 0x1000, kCCOtherLetter,           kCCUnassigned,              cc_bitarray_1000 },
    { 0x102c, kCCCombiningSpacingMark,  kCCNonSpacingMark,          cc_bitarray_102c },
    { 0x1033, kCCUnassigned,            kCCUnassigned,              NULL },
    { 0x1036, kCCNonSpacingMark,        kCCNonSpacingMark,          NULL },
    { 0x1038, kCCCombiningSpacingMark,  kCCCombiningSpacingMark,    NULL },
    { 0x1039, kCCNonSpacingMark,        kCCNonSpacingMark,          NULL },
    { 0x103a, kCCUnassigned,            kCCUnassigned,              NULL },
    { 0x1040, kCCDecimalDigitNumber,    kCCDecimalDigitNumber,      NULL },
    { 0x104a, kCCOtherPunctuation,      kCCOtherPunctuation,        NULL },
    { 0x1050, kCCOtherLetter,           kCCOtherLetter,             NULL },
    { 0x1056, kCCCombiningSpacingMark,  kCCCombiningSpacingMark,    NULL },
    { 0x1058, kCCNonSpacingMark,        kCCNonSpacingMark,          NULL },
    { 0x105a, kCCUnassigned,            kCCUnassigned,              NULL },
    { 0x10a0, kCCUppercaseLetter,       kCCUppercaseLetter,         NULL },
    { 0x10c6, kCCUnassigned,            kCCUnassigned,              NULL },
    { 0x10d0, kCCOtherLetter,           kCCOtherLetter,             NULL },
    { 0x10f9, kCCUnassigned,            kCCUnassigned,              NULL },
    { 0x10fb, kCCOtherPunctuation,      kCCOtherPunctuation,        NULL },
    { 0x10fc, kCCUnassigned,            kCCOtherLetter,             cc_bitarray_10fc },
    { 0x1361, kCCOtherPunctuation,      kCCOtherPunctuation,        NULL },
    { 0x1369, kCCDecimalDigitNumber,    kCCDecimalDigitNumber,      NULL },
    { 0x1372, kCCOtherNumber,           kCCOtherNumber,             NULL },
    { 0x137d, kCCUnassigned,            kCCUnassigned,              NULL },
    { 0x13a0, kCCOtherLetter,           kCCOtherLetter,             NULL },
    { 0x13f5, kCCUnassigned,            kCCUnassigned,              NULL },
    { 0x1401, kCCOtherLetter,           kCCOtherLetter,             NULL },
    { 0x166d, kCCOtherPunctuation,      kCCOtherPunctuation,        NULL },
    { 0x166f, kCCOtherLetter,           kCCOtherLetter,             NULL },
    { 0x1677, kCCUnassigned,            kCCUnassigned,              NULL },
    { 0x1680, kCCSpaceSeparator,        kCCSpaceSeparator,          NULL },
    { 0x1681, kCCOtherLetter,           kCCOtherLetter,             NULL },
    { 0x169b, kCCStartPunctuation,      kCCStartPunctuation,        NULL },
    { 0x169c, kCCEndPunctuation,        kCCEndPunctuation,          NULL },
    { 0x169d, kCCUnassigned,            kCCUnassigned,              NULL },
    { 0x16a0, kCCOtherLetter,           kCCOtherLetter,             NULL },
    { 0x16eb, kCCOtherPunctuation,      kCCOtherPunctuation,        NULL },
    { 0x16ee, kCCLetterNumber,          kCCLetterNumber,            NULL },
    { 0x16f1, kCCUnassigned,            kCCOtherLetter,             cc_bitarray_16f1 },
    { 0x1712, kCCNonSpacingMark,        kCCNonSpacingMark,          NULL },
    { 0x1715, kCCUnassigned,            kCCUnassigned,              NULL },
    { 0x1720, kCCOtherLetter,           kCCOtherLetter,             NULL },
    { 0x1732, kCCNonSpacingMark,        kCCNonSpacingMark,          NULL },
    { 0x1735, kCCOtherPunctuation,      kCCOtherPunctuation,        NULL },
    { 0x1737, kCCUnassigned,            kCCUnassigned,              NULL },
    { 0x1740, kCCOtherLetter,           kCCOtherLetter,             NULL },
    { 0x1752, kCCNonSpacingMark,        kCCNonSpacingMark,          NULL },
    { 0x1754, kCCUnassigned,            kCCOtherLetter,             cc_bitarray_1754 },
    { 0x1772, kCCNonSpacingMark,        kCCNonSpacingMark,          NULL },
    { 0x1774, kCCUnassigned,            kCCUnassigned,              NULL },
    { 0x1780, kCCOtherLetter,           kCCOtherLetter,             NULL },
    { 0x17b4, kCCFormatChar,            kCCFormatChar,              NULL },
    { 0x17b6, kCCCombiningSpacingMark,  kCCNonSpacingMark,          cc_bitarray_17b6 },
    { 0x17d4, kCCOtherPunctuation,      kCCOtherPunctuation,        NULL },
    { 0x17d7, kCCModifierLetter,        kCCModifierLetter,          NULL },
    { 0x17d8, kCCOtherPunctuation,      kCCOtherPunctuation,        NULL },
    { 0x17db, kCCCurrencySymbol,        kCCCurrencySymbol,          NULL },
    { 0x17dc, kCCOtherLetter,           kCCOtherLetter,             NULL },
    { 0x17dd, kCCNonSpacingMark,        kCCNonSpacingMark,          NULL },
    { 0x17de, kCCUnassigned,            kCCUnassigned,              NULL },
    { 0x17e0, kCCDecimalDigitNumber,    kCCDecimalDigitNumber,      NULL },
    { 0x17ea, kCCUnassigned,            kCCUnassigned,              NULL },
    { 0x17f0, kCCOtherNumber,           kCCOtherNumber,             NULL },
    { 0x17fa, kCCUnassigned,            kCCUnassigned,              NULL },
    { 0x1800, kCCOtherPunctuation,      kCCOtherPunctuation,        NULL },
    { 0x1806, kCCDashPunctuation,       kCCDashPunctuation,         NULL },
    { 0x1807, kCCOtherPunctuation,      kCCOtherPunctuation,        NULL },
    { 0x180b, kCCNonSpacingMark,        kCCNonSpacingMark,          NULL },
    { 0x180e, kCCSpaceSeparator,        kCCSpaceSeparator,          NULL },
    { 0x180f, kCCUnassigned,            kCCUnassigned,              NULL },
    { 0x1810, kCCDecimalDigitNumber,    kCCDecimalDigitNumber,      NULL },
    { 0x181a, kCCUnassigned,            kCCUnassigned,              NULL },
    { 0x1820, kCCOtherLetter,           kCCOtherLetter,             NULL },
    { 0x1843, kCCModifierLetter,        kCCModifierLetter,          NULL },
    { 0x1844, kCCOtherLetter,           kCCOtherLetter,             NULL },
    { 0x1878, kCCUnassigned,            kCCUnassigned,              NULL },
    { 0x1880, kCCOtherLetter,           kCCOtherLetter,             NULL },
    { 0x18a9, kCCNonSpacingMark,        kCCNonSpacingMark,          NULL },
    { 0x18aa, kCCUnassigned,            kCCUnassigned,              NULL },
    { 0x1900, kCCOtherLetter,           kCCOtherLetter,             NULL },
    { 0x191d, kCCUnassigned,            kCCUnassigned,              NULL },
    { 0x1920, kCCNonSpacingMark,        kCCCombiningSpacingMark,    cc_bitarray_1920 },
    { 0x192c, kCCUnassigned,            kCCUnassigned,              NULL },
    { 0x1930, kCCCombiningSpacingMark,  kCCNonSpacingMark,          cc_bitarray_1930 },
    { 0x193c, kCCUnassigned,            kCCUnassigned,              NULL },
    { 0x1940, kCCOtherSymbol,           kCCOtherSymbol,             NULL },
    { 0x1941, kCCUnassigned,            kCCUnassigned,              NULL },
    { 0x1944, kCCOtherPunctuation,      kCCOtherPunctuation,        NULL },
    { 0x1946, kCCDecimalDigitNumber,    kCCDecimalDigitNumber,      NULL },
    { 0x1950, kCCOtherLetter,           kCCUnassigned,              cc_bitarray_1950 },
    { 0x19e0, kCCOtherSymbol,           kCCOtherSymbol,             NULL },
    { 0x1a00, kCCUnassigned,            kCCUnassigned,              NULL },
    { 0x1d00, kCCLowercaseLetter,       kCCLowercaseLetter,         NULL },
    { 0x1d2c, kCCModifierLetter,        kCCModifierLetter,          NULL },
    { 0x1d62, kCCLowercaseLetter,       kCCLowercaseLetter,         NULL },
    { 0x1d6c, kCCUnassigned,            kCCUnassigned,              NULL },
    { 0x1e00, kCCUppercaseLetter,       kCCLowercaseLetter,         cc_bitarray_1e00 },
    { 0x1e9c, kCCUnassigned,            kCCUnassigned,              NULL },
    { 0x1ea0, kCCUppercaseLetter,       kCCLowercaseLetter,         cc_bitarray_1ea0 },
    { 0x1efa, kCCUnassigned,            kCCUnassigned,              NULL },
    { 0x1f00, kCCLowercaseLetter,       kCCLowercaseLetter,         NULL },
    { 0x1f08, kCCUppercaseLetter,       kCCUppercaseLetter,         NULL },
    { 0x1f10, kCCLowercaseLetter,       kCCLowercaseLetter,         NULL },
    { 0x1f16, kCCUnassigned,            kCCUnassigned,              NULL },
    { 0x1f18, kCCUppercaseLetter,       kCCUppercaseLetter,         NULL },
    { 0x1f1e, kCCUnassigned,            kCCUnassigned,              NULL },
    { 0x1f20, kCCLowercaseLetter,       kCCUppercaseLetter,         cc_bitarray_1f20 },
    { 0x1f46, kCCUnassigned,            kCCUnassigned,              NULL },
    { 0x1f48, kCCUppercaseLetter,       kCCUppercaseLetter,         NULL },
    { 0x1f4e, kCCUnassigned,            kCCUnassigned,              NULL },
    { 0x1f50, kCCLowercaseLetter,       kCCLowercaseLetter,         NULL },
    { 0x1f58, kCCUnassigned,            kCCUppercaseLetter,         cc_bitarray_1f58 },
    { 0x1f60, kCCLowercaseLetter,       kCCLowercaseLetter,         NULL },
    { 0x1f68, kCCUppercaseLetter,       kCCUppercaseLetter,         NULL },
    { 0x1f70, kCCLowercaseLetter,       kCCLowercaseLetter,         NULL },
    { 0x1f7e, kCCUnassigned,            kCCUnassigned,              NULL },
    { 0x1f80, kCCLowercaseLetter,       kCCTitlecaseLetter,         cc_bitarray_1f80 },
    { 0x1fb5, kCCUnassigned,            kCCUnassigned,              NULL },
    { 0x1fb6, kCCLowercaseLetter,       kCCLowercaseLetter,         NULL },
    { 0x1fb8, kCCUppercaseLetter,       kCCUppercaseLetter,         NULL },
    { 0x1fbc, kCCTitlecaseLetter,       kCCTitlecaseLetter,         NULL },
    { 0x1fbd, kCCModifierSymbol,        kCCLowercaseLetter,         cc_bitarray_1fbd },
    { 0x1fc5, kCCUnassigned,            kCCUnassigned,              NULL },
    { 0x1fc6, kCCLowercaseLetter,       kCCLowercaseLetter,         NULL },
    { 0x1fc8, kCCUppercaseLetter,       kCCUppercaseLetter,         NULL },
    { 0x1fcc, kCCTitlecaseLetter,       kCCTitlecaseLetter,         NULL },
    { 0x1fcd, kCCModifierSymbol,        kCCModifierSymbol,          NULL },
    { 0x1fd0, kCCLowercaseLetter,       kCCLowercaseLetter,         NULL },
    { 0x1fd4, kCCUnassigned,            kCCUnassigned,              NULL },
    { 0x1fd6, kCCLowercaseLetter,       kCCLowercaseLetter,         NULL },
    { 0x1fd8, kCCUppercaseLetter,       kCCUppercaseLetter,         NULL },
    { 0x1fdc, kCCUnassigned,            kCCUnassigned,              NULL },
    { 0x1fdd, kCCModifierSymbol,        kCCModifierSymbol,          NULL },
    { 0x1fe0, kCCLowercaseLetter,       kCCLowercaseLetter,         NULL },
    { 0x1fe8, kCCUppercaseLetter,       kCCUppercaseLetter,         NULL },
    { 0x1fed, kCCModifierSymbol,        kCCModifierSymbol,          NULL },
    { 0x1ff0, kCCUnassigned,            kCCLowercaseLetter,         cc_bitarray_1ff0 },
    { 0x1ff8, kCCUppercaseLetter,       kCCUppercaseLetter,         NULL },
    { 0x1ffc, kCCTitlecaseLetter,       kCCTitlecaseLetter,         NULL },
    { 0x1ffd, kCCModifierSymbol,        kCCModifierSymbol,          NULL },
    { 0x1fff, kCCUnassigned,            kCCUnassigned,              NULL },
    { 0x2000, kCCSpaceSeparator,        kCCSpaceSeparator,          NULL },
    { 0x200c, kCCFormatChar,            kCCFormatChar,              NULL },
    { 0x2010, kCCDashPunctuation,       kCCDashPunctuation,         NULL },
    { 0x2016, kCCOtherPunctuation,      kCCOtherPunctuation,        NULL },
    { 0x2018, kCCInitialPunctuation,    kCCInitialPunctuation,      NULL },
    { 0x2019, kCCFinalPunctuation,      kCCFinalPunctuation,        NULL },
    { 0x201a, kCCStartPunctuation,      kCCStartPunctuation,        NULL },
    { 0x201b, kCCInitialPunctuation,    kCCInitialPunctuation,      NULL },
    { 0x201d, kCCFinalPunctuation,      kCCFinalPunctuation,        NULL },
    { 0x201e, kCCStartPunctuation,      kCCStartPunctuation,        NULL },
    { 0x201f, kCCInitialPunctuation,    kCCInitialPunctuation,      NULL },
    { 0x2020, kCCOtherPunctuation,      kCCOtherPunctuation,        NULL },
    { 0x2028, kCCLineSeparator,         kCCLineSeparator,           NULL },
    { 0x2029, kCCParagraphSeparator,    kCCParagraphSeparator,      NULL },
    { 0x202a, kCCFormatChar,            kCCFormatChar,              NULL },
    { 0x202f, kCCSpaceSeparator,        kCCSpaceSeparator,          NULL },
    { 0x2030, kCCOtherPunctuation,      kCCOtherPunctuation,        NULL },
    { 0x2039, kCCInitialPunctuation,    kCCInitialPunctuation,      NULL },
    { 0x203a, kCCFinalPunctuation,      kCCFinalPunctuation,        NULL },
    { 0x203b, kCCOtherPunctuation,      kCCOtherPunctuation,        NULL },
    { 0x203f, kCCConnectorPunctuation,  kCCConnectorPunctuation,    NULL },
    { 0x2041, kCCOtherPunctuation,      kCCOtherPunctuation,        NULL },
    { 0x2044, kCCMathSymbol,            kCCMathSymbol,              NULL },
    { 0x2045, kCCStartPunctuation,      kCCStartPunctuation,        NULL },
    { 0x2046, kCCEndPunctuation,        kCCEndPunctuation,          NULL },
    { 0x2047, kCCOtherPunctuation,      kCCOtherPunctuation,        NULL },
    { 0x2052, kCCMathSymbol,            kCCMathSymbol,              NULL },
    { 0x2053, kCCOtherPunctuation,      kCCOtherPunctuation,        NULL },
    { 0x2054, kCCConnectorPunctuation,  kCCConnectorPunctuation,    NULL },
    { 0x2055, kCCUnassigned,            kCCUnassigned,              NULL },
    { 0x2057, kCCOtherPunctuation,      kCCOtherPunctuation,        NULL },
    { 0x2058, kCCUnassigned,            kCCUnassigned,              NULL },
    { 0x205f, kCCSpaceSeparator,        kCCSpaceSeparator,          NULL },
    { 0x2060, kCCFormatChar,            kCCFormatChar,              NULL },
    { 0x2064, kCCUnassigned,            kCCUnassigned,              NULL },
    { 0x206a, kCCFormatChar,            kCCFormatChar,              NULL },
    { 0x2070, kCCOtherNumber,           kCCOtherNumber,             NULL },
    { 0x2071, kCCLowercaseLetter,       kCCLowercaseLetter,         NULL },
    { 0x2072, kCCUnassigned,            kCCUnassigned,              NULL },
    { 0x2074, kCCOtherNumber,           kCCOtherNumber,             NULL },
    { 0x207a, kCCMathSymbol,            kCCMathSymbol,              NULL },
    { 0x207d, kCCStartPunctuation,      kCCStartPunctuation,        NULL },
    { 0x207e, kCCEndPunctuation,        kCCEndPunctuation,          NULL },
    { 0x207f, kCCLowercaseLetter,       kCCLowercaseLetter,         NULL },
    { 0x2080, kCCOtherNumber,           kCCOtherNumber,             NULL },
    { 0x208a, kCCMathSymbol,            kCCMathSymbol,              NULL },
    { 0x208d, kCCStartPunctuation,      kCCStartPunctuation,        NULL },
    { 0x208e, kCCEndPunctuation,        kCCEndPunctuation,          NULL },
    { 0x208f, kCCUnassigned,            kCCUnassigned,              NULL },
    { 0x20a0, kCCCurrencySymbol,        kCCCurrencySymbol,          NULL },
    { 0x20b2, kCCUnassigned,            kCCUnassigned,              NULL },
    { 0x20d0, kCCNonSpacingMark,        kCCEnclosingMark,           cc_bitarray_20d0 },
    { 0x20eb, kCCUnassigned,            kCCUnassigned,              NULL },
    { 0x2100, kCCOtherSymbol,           kCCUppercaseLetter,         cc_bitarray_2100 },
    { 0x210a, kCCLowercaseLetter,       kCCUppercaseLetter,         cc_bitarray_210a },
    { 0x2114, kCCOtherSymbol,           kCCUppercaseLetter,         cc_bitarray_2114 },
    { 0x212f, kCCLowercaseLetter,       kCCLowercaseLetter,         NULL },
    { 0x2130, kCCUppercaseLetter,       kCCUppercaseLetter,         NULL },
    { 0x2132, kCCOtherSymbol,           kCCOtherSymbol,             NULL },
    { 0x2133, kCCUppercaseLetter,       kCCUppercaseLetter,         NULL },
    { 0x2134, kCCLowercaseLetter,       kCCLowercaseLetter,         NULL },
    { 0x2135, kCCOtherLetter,           kCCOtherLetter,             NULL },
    { 0x2139, kCCLowercaseLetter,       kCCLowercaseLetter,         NULL },
    { 0x213a, kCCOtherSymbol,           kCCOtherSymbol,             NULL },
    { 0x213c, kCCUnassigned,            kCCUnassigned,              NULL },
    { 0x213d, kCCLowercaseLetter,       kCCLowercaseLetter,         NULL },
    { 0x213e, kCCUppercaseLetter,       kCCUppercaseLetter,         NULL },
    { 0x2140, kCCMathSymbol,            kCCMathSymbol,              NULL },
    { 0x2145, kCCUppercaseLetter,       kCCUppercaseLetter,         NULL },
    { 0x2146, kCCLowercaseLetter,       kCCLowercaseLetter,         NULL },
    { 0x214a, kCCOtherSymbol,           kCCOtherSymbol,             NULL },
    { 0x214b, kCCMathSymbol,            kCCMathSymbol,              NULL },
    { 0x214c, kCCUnassigned,            kCCUnassigned,              NULL },
    { 0x2153, kCCOtherNumber,           kCCOtherNumber,             NULL },
    { 0x2160, kCCLetterNumber,          kCCLetterNumber,            NULL },
    { 0x2184, kCCUnassigned,            kCCUnassigned,              NULL },
    { 0x2190, kCCMathSymbol,           kCCOtherSymbol,             cc_bitarray_2190 },
    { 0x21f4, kCCMathSymbol,            kCCMathSymbol,              NULL },
    { 0x2300, kCCOtherSymbol,            kCCMathSymbol,              cc_bitarray_2300 },
    { 0x2329, kCCStartPunctuation,      kCCStartPunctuation,        NULL },
    { 0x232a, kCCEndPunctuation,        kCCEndPunctuation,          NULL },
    { 0x232b, kCCOtherSymbol,            kCCMathSymbol,              cc_bitarray_232b },
    { 0x23b4, kCCStartPunctuation,      kCCStartPunctuation,        NULL },
    { 0x23b5, kCCEndPunctuation,        kCCEndPunctuation,          NULL },
    { 0x23b6, kCCOtherPunctuation,      kCCOtherPunctuation,        NULL },
    { 0x23b7, kCCOtherSymbol,           kCCUnassigned,              cc_bitarray_23b7 },
    { 0x2460, kCCOtherNumber,           kCCOtherNumber,             NULL },
    { 0x249c, kCCOtherSymbol,           kCCOtherSymbol,             NULL },
    { 0x24ea, kCCOtherNumber,           kCCOtherNumber,             NULL },
    { 0x2500, kCCOtherSymbol,           kCCOtherSymbol,             NULL },
    { 0x25b7, kCCMathSymbol,           kCCOtherSymbol,             cc_bitarray_25b7 },
    { 0x2618, kCCUnassigned,            kCCUnassigned,              NULL },
    { 0x2619, kCCOtherSymbol,           kCCOtherSymbol,             NULL },
    { 0x266f, kCCMathSymbol,            kCCMathSymbol,              NULL },
    { 0x2670, kCCOtherSymbol,           kCCUnassigned,              cc_bitarray_2670 },
    { 0x2768, kCCStartPunctuation,      kCCEndPunctuation,          cc_bitarray_2768 },
    { 0x2776, kCCOtherNumber,           kCCOtherNumber,             NULL },
    { 0x2794, kCCOtherSymbol,           kCCUnassigned,              cc_bitarray_2794 },
    { 0x27d0, kCCMathSymbol,            kCCMathSymbol,              NULL },
    { 0x27e6, kCCStartPunctuation,      kCCEndPunctuation,          cc_bitarray_27e6 },
    { 0x27ec, kCCUnassigned,            kCCUnassigned,              NULL },
    { 0x27f0, kCCMathSymbol,            kCCMathSymbol,              NULL },
    { 0x2800, kCCOtherSymbol,           kCCOtherSymbol,             NULL },
    { 0x2900, kCCMathSymbol,            kCCMathSymbol,              NULL },
    { 0x2983, kCCStartPunctuation,      kCCEndPunctuation,          cc_bitarray_2983 },
    { 0x2999, kCCMathSymbol,            kCCMathSymbol,              NULL },
    { 0x29d8, kCCStartPunctuation,      kCCEndPunctuation,          cc_bitarray_29d8 },
    { 0x29dc, kCCMathSymbol,            kCCMathSymbol,              NULL },
    { 0x29fc, kCCStartPunctuation,      kCCStartPunctuation,        NULL },
    { 0x29fd, kCCEndPunctuation,        kCCEndPunctuation,          NULL },
    { 0x29fe, kCCMathSymbol,            kCCMathSymbol,              NULL },
    { 0x2b00, kCCOtherSymbol,           kCCOtherSymbol,             NULL },
    { 0x2b0e, kCCUnassigned,            kCCUnassigned,              NULL },
    { 0x2e80, kCCOtherSymbol,           kCCUnassigned,              cc_bitarray_2e80 },
    { 0x2f00, kCCOtherSymbol,           kCCOtherSymbol,             NULL },
    { 0x2fd6, kCCUnassigned,            kCCUnassigned,              NULL },
    { 0x2ff0, kCCOtherSymbol,           kCCOtherSymbol,             NULL },
    { 0x2ffc, kCCUnassigned,            kCCUnassigned,              NULL },
    { 0x3000, kCCSpaceSeparator,        kCCSpaceSeparator,          NULL },
    { 0x3001, kCCOtherPunctuation,      kCCOtherPunctuation,        NULL },
    { 0x3004, kCCOtherSymbol,           kCCOtherSymbol,             NULL },
    { 0x3005, kCCModifierLetter,        kCCModifierLetter,          NULL },
    { 0x3006, kCCOtherLetter,           kCCOtherLetter,             NULL },
    { 0x3007, kCCLetterNumber,          kCCLetterNumber,            NULL },
    { 0x3008, kCCStartPunctuation,      kCCEndPunctuation,          cc_bitarray_3008 },
    { 0x3012, kCCOtherSymbol,           kCCOtherSymbol,             NULL },
    { 0x3014, kCCStartPunctuation,      kCCEndPunctuation,          cc_bitarray_3014 },
    { 0x301c, kCCDashPunctuation,       kCCDashPunctuation,         NULL },
    { 0x301d, kCCStartPunctuation,      kCCStartPunctuation,        NULL },
    { 0x301e, kCCEndPunctuation,        kCCEndPunctuation,          NULL },
    { 0x3020, kCCOtherSymbol,           kCCOtherSymbol,             NULL },
    { 0x3021, kCCLetterNumber,          kCCLetterNumber,            NULL },
    { 0x302a, kCCNonSpacingMark,        kCCNonSpacingMark,          NULL },
    { 0x3030, kCCDashPunctuation,       kCCDashPunctuation,         NULL },
    { 0x3031, kCCModifierLetter,        kCCModifierLetter,          NULL },
    { 0x3036, kCCOtherSymbol,           kCCOtherSymbol,             NULL },
    { 0x3038, kCCLetterNumber,          kCCLetterNumber,            NULL },
    { 0x303b, kCCModifierLetter,        kCCModifierLetter,          NULL },
    { 0x303c, kCCOtherLetter,           kCCOtherLetter,             NULL },
    { 0x303d, kCCOtherPunctuation,      kCCOtherPunctuation,        NULL },
    { 0x303e, kCCOtherSymbol,           kCCOtherSymbol,             NULL },
    { 0x3040, kCCUnassigned,            kCCUnassigned,              NULL },
    { 0x3041, kCCOtherLetter,           kCCOtherLetter,             NULL },
    { 0x3097, kCCUnassigned,            kCCUnassigned,              NULL },
    { 0x3099, kCCNonSpacingMark,        kCCNonSpacingMark,          NULL },
    { 0x309b, kCCModifierSymbol,        kCCModifierSymbol,          NULL },
    { 0x309d, kCCModifierLetter,        kCCModifierLetter,          NULL },
    { 0x309f, kCCOtherLetter,           kCCOtherLetter,             NULL },
    { 0x30a0, kCCDashPunctuation,       kCCDashPunctuation,         NULL },
    { 0x30a1, kCCOtherLetter,           kCCOtherLetter,             NULL },
    { 0x30fb, kCCConnectorPunctuation,  kCCConnectorPunctuation,    NULL },
    { 0x30fc, kCCModifierLetter,        kCCModifierLetter,          NULL },
    { 0x30ff, kCCOtherLetter,           kCCUnassigned,              cc_bitarray_30ff },
    { 0x3190, kCCOtherSymbol,           kCCOtherSymbol,             NULL },
    { 0x3192, kCCOtherNumber,           kCCOtherNumber,             NULL },
    { 0x3196, kCCOtherSymbol,           kCCOtherSymbol,             NULL },
    { 0x31a0, kCCOtherLetter,           kCCOtherLetter,             NULL },
    { 0x31b8, kCCUnassigned,            kCCUnassigned,              NULL },
    { 0x31f0, kCCOtherLetter,           kCCOtherLetter,             NULL },
    { 0x3200, kCCOtherSymbol,           kCCOtherSymbol,             NULL },
    { 0x321f, kCCUnassigned,            kCCUnassigned,              NULL },
    { 0x3220, kCCOtherNumber,           kCCOtherNumber,             NULL },
    { 0x322a, kCCOtherSymbol,           kCCOtherSymbol,             NULL },
    { 0x3244, kCCUnassigned,            kCCUnassigned,              NULL },
    { 0x3250, kCCOtherSymbol,           kCCOtherSymbol,             NULL },
    { 0x3251, kCCOtherNumber,           kCCOtherNumber,             NULL },
    { 0x3260, kCCOtherSymbol,           kCCOtherSymbol,             NULL },
    { 0x327e, kCCUnassigned,            kCCUnassigned,              NULL },
    { 0x327f, kCCOtherSymbol,           kCCOtherNumber,             cc_bitarray_327f },
    { 0x32ff, kCCUnassigned,            kCCUnassigned,              NULL },
    { 0x3300, kCCOtherSymbol,           kCCOtherSymbol,             NULL },
    { 0x3400, kCCOtherLetter,           kCCOtherLetter,             NULL },
    { 0x4db6, kCCUnassigned,            kCCUnassigned,              NULL },
    { 0x4dc0, kCCOtherSymbol,           kCCOtherSymbol,             NULL },
    { 0x4e00, kCCOtherLetter,           kCCOtherLetter,             NULL },
    { 0x9fa6, kCCUnassigned,            kCCUnassigned,              NULL },
    { 0xa000, kCCOtherLetter,           kCCOtherLetter,             NULL },
    { 0xa48d, kCCUnassigned,            kCCUnassigned,              NULL },
    { 0xa490, kCCOtherSymbol,           kCCOtherSymbol,             NULL },
    { 0xa4c7, kCCUnassigned,            kCCUnassigned,              NULL },
    { 0xac00, kCCOtherLetter,           kCCOtherLetter,             NULL },
    { 0xd7a4, kCCUnassigned,            kCCUnassigned,              NULL },
    { 0xd800, kCCSurrogate,             kCCSurrogate,               NULL },
    { 0xe000, kCCPrivateUseChar,        kCCPrivateUseChar,          NULL },
    { 0xf900, kCCOtherLetter,           kCCOtherLetter,             NULL },
    { 0xfa2e, kCCUnassigned,            kCCUnassigned,              NULL },
    { 0xfa30, kCCOtherLetter,           kCCOtherLetter,             NULL },
    { 0xfa6b, kCCUnassigned,            kCCUnassigned,              NULL },
    { 0xfb00, kCCLowercaseLetter,       kCCUnassigned,              cc_bitarray_fb00 },
    { 0xfb1d, kCCOtherLetter,           kCCOtherLetter,             NULL },
    { 0xfb1e, kCCNonSpacingMark,        kCCNonSpacingMark,          NULL },
    { 0xfb1f, kCCOtherLetter,           kCCOtherLetter,             NULL },
    { 0xfb29, kCCMathSymbol,            kCCMathSymbol,              NULL },
    { 0xfb2a, kCCOtherLetter,           kCCUnassigned,              cc_bitarray_fb2a },
    { 0xfbd3, kCCOtherLetter,           kCCOtherLetter,             NULL },
    { 0xfd3e, kCCStartPunctuation,      kCCStartPunctuation,        NULL },
    { 0xfd3f, kCCEndPunctuation,        kCCEndPunctuation,          NULL },
    { 0xfd40, kCCUnassigned,            kCCOtherLetter,             cc_bitarray_fd40 },
    { 0xfdfc, kCCCurrencySymbol,        kCCCurrencySymbol,          NULL },
    { 0xfdfd, kCCOtherSymbol,           kCCOtherSymbol,             NULL },
    { 0xfdfe, kCCUnassigned,            kCCNonSpacingMark,          cc_bitarray_fdfe },
    { 0xfe30, kCCOtherPunctuation,      kCCOtherPunctuation,        NULL },
    { 0xfe31, kCCDashPunctuation,       kCCDashPunctuation,         NULL },
    { 0xfe33, kCCConnectorPunctuation,  kCCConnectorPunctuation,    NULL },
    { 0xfe35, kCCStartPunctuation,      kCCEndPunctuation,          cc_bitarray_fe35 },
    { 0xfe45, kCCOtherPunctuation,      kCCOtherPunctuation,        NULL },
    { 0xfe47, kCCStartPunctuation,      kCCStartPunctuation,        NULL },
    { 0xfe48, kCCEndPunctuation,        kCCEndPunctuation,          NULL },
    { 0xfe49, kCCOtherPunctuation,      kCCOtherPunctuation,        NULL },
    { 0xfe4d, kCCConnectorPunctuation,  kCCConnectorPunctuation,    NULL },
    { 0xfe50, kCCOtherPunctuation,      kCCOtherPunctuation,        NULL },
    { 0xfe53, kCCUnassigned,            kCCUnassigned,              NULL },
    { 0xfe54, kCCOtherPunctuation,      kCCOtherPunctuation,        NULL },
    { 0xfe58, kCCDashPunctuation,       kCCDashPunctuation,         NULL },
    { 0xfe59, kCCStartPunctuation,      kCCEndPunctuation,          cc_bitarray_fe59 },
    { 0xfe5f, kCCOtherPunctuation,      kCCOtherPunctuation,        NULL },
    { 0xfe62, kCCMathSymbol,            kCCMathSymbol,              NULL },
    { 0xfe63, kCCDashPunctuation,       kCCDashPunctuation,         NULL },
    { 0xfe64, kCCMathSymbol,            kCCMathSymbol,              NULL },
    { 0xfe67, kCCUnassigned,            kCCUnassigned,              NULL },
    { 0xfe68, kCCOtherPunctuation,      kCCOtherPunctuation,        NULL },
    { 0xfe69, kCCCurrencySymbol,        kCCCurrencySymbol,          NULL },
    { 0xfe6a, kCCOtherPunctuation,      kCCOtherPunctuation,        NULL },
    { 0xfe6c, kCCUnassigned,            kCCUnassigned,              NULL },
    { 0xfe70, kCCOtherLetter,           kCCOtherLetter,             NULL },
    { 0xfe75, kCCUnassigned,            kCCUnassigned,              NULL },
    { 0xfe76, kCCOtherLetter,           kCCOtherLetter,             NULL },
    { 0xfefd, kCCUnassigned,            kCCUnassigned,              NULL },
    { 0xfeff, kCCFormatChar,            kCCFormatChar,              NULL },
    { 0xff00, kCCUnassigned,            kCCUnassigned,              NULL },
    { 0xff01, kCCOtherPunctuation,      kCCOtherPunctuation,        NULL },
    { 0xff04, kCCCurrencySymbol,        kCCCurrencySymbol,          NULL },
    { 0xff05, kCCOtherPunctuation,      kCCOtherPunctuation,        NULL },
    { 0xff08, kCCStartPunctuation,      kCCStartPunctuation,        NULL },
    { 0xff09, kCCEndPunctuation,        kCCEndPunctuation,          NULL },
    { 0xff0a, kCCOtherPunctuation,      kCCOtherPunctuation,        NULL },
    { 0xff0b, kCCMathSymbol,            kCCMathSymbol,              NULL },
    { 0xff0c, kCCOtherPunctuation,      kCCOtherPunctuation,        NULL },
    { 0xff0d, kCCDashPunctuation,       kCCDashPunctuation,         NULL },
    { 0xff0e, kCCOtherPunctuation,      kCCOtherPunctuation,        NULL },
    { 0xff10, kCCDecimalDigitNumber,    kCCDecimalDigitNumber,      NULL },
    { 0xff1a, kCCOtherPunctuation,      kCCOtherPunctuation,        NULL },
    { 0xff1c, kCCMathSymbol,            kCCMathSymbol,              NULL },
    { 0xff1f, kCCOtherPunctuation,      kCCOtherPunctuation,        NULL },
    { 0xff21, kCCUppercaseLetter,       kCCUppercaseLetter,         NULL },
    { 0xff3b, kCCStartPunctuation,      kCCStartPunctuation,        NULL },
    { 0xff3c, kCCOtherPunctuation,      kCCOtherPunctuation,        NULL },
    { 0xff3d, kCCEndPunctuation,        kCCEndPunctuation,          NULL },
    { 0xff3e, kCCModifierSymbol,        kCCModifierSymbol,          NULL },
    { 0xff3f, kCCConnectorPunctuation,  kCCConnectorPunctuation,    NULL },
    { 0xff40, kCCModifierSymbol,        kCCModifierSymbol,          NULL },
    { 0xff41, kCCLowercaseLetter,       kCCLowercaseLetter,         NULL },
    { 0xff5b, kCCStartPunctuation,      kCCStartPunctuation,        NULL },
    { 0xff5c, kCCMathSymbol,            kCCMathSymbol,              NULL },
    { 0xff5d, kCCEndPunctuation,        kCCEndPunctuation,          NULL },
    { 0xff5e, kCCMathSymbol,            kCCMathSymbol,              NULL },
    { 0xff5f, kCCStartPunctuation,      kCCStartPunctuation,        NULL },
    { 0xff60, kCCEndPunctuation,        kCCEndPunctuation,          NULL },
    { 0xff61, kCCOtherPunctuation,      kCCOtherPunctuation,        NULL },
    { 0xff62, kCCStartPunctuation,      kCCStartPunctuation,        NULL },
    { 0xff63, kCCEndPunctuation,        kCCEndPunctuation,          NULL },
    { 0xff64, kCCOtherPunctuation,      kCCOtherPunctuation,        NULL },
    { 0xff65, kCCConnectorPunctuation,  kCCConnectorPunctuation,    NULL },
    { 0xff66, kCCOtherLetter,           kCCModifierLetter,          cc_bitarray_ff66 },
    { 0xffbf, kCCUnassigned,            kCCOtherLetter,             cc_bitarray_ffbf },
    { 0xffe0, kCCCurrencySymbol,        kCCCurrencySymbol,          NULL },
    { 0xffe2, kCCMathSymbol,            kCCMathSymbol,              NULL },
    { 0xffe3, kCCModifierSymbol,        kCCModifierSymbol,          NULL },
    { 0xffe4, kCCOtherSymbol,           kCCOtherSymbol,             NULL },
    { 0xffe5, kCCCurrencySymbol,        kCCCurrencySymbol,          NULL },
    { 0xffe7, kCCUnassigned,            kCCUnassigned,              NULL },
    { 0xffe8, kCCOtherSymbol,           kCCOtherSymbol,             NULL },
    { 0xffe9, kCCMathSymbol,            kCCMathSymbol,              NULL },
    { 0xffed, kCCOtherSymbol,           kCCOtherSymbol,             NULL },
    { 0xffef, kCCUnassigned,            kCCUnassigned,              NULL },
    { 0xfff9, kCCFormatChar,            kCCFormatChar,              NULL },
    { 0xfffc, kCCOtherSymbol,           kCCOtherSymbol,             NULL },
    { 0xfffe, kCCUnassigned,            kCCUnassigned,              NULL                                                                            }

}; // gCharCategoryTable


// These define the line break properties for the first 256 characters.
const char gLineBreakCategoryTable256[0x0100] = 
{
    kLBCCombiningMark,    kLBCCombiningMark,    kLBCCombiningMark,    kLBCCombiningMark,
    kLBCCombiningMark,    kLBCCombiningMark,    kLBCCombiningMark,    kLBCCombiningMark,                  
    kLBCCombiningMark,    kLBCBreakAfter,       kLBCLineFeed,         kLBCCombiningMark,                  
    kLBCMandatoryBreak,   kLBCCarriageReturn,   kLBCCombiningMark,    kLBCCombiningMark,
    kLBCCombiningMark,    kLBCCombiningMark,    kLBCCombiningMark,    kLBCCombiningMark,
    kLBCCombiningMark,    kLBCCombiningMark,    kLBCCombiningMark,    kLBCCombiningMark,
    kLBCCombiningMark,    kLBCCombiningMark,    kLBCCombiningMark,    kLBCCombiningMark,
    kLBCCombiningMark,    kLBCCombiningMark,    kLBCCombiningMark,    kLBCCombiningMark,
    kLBCSpace,            kLBCExclamation,      kLBCQuotation,        kLBCAlphabetic,                  
    kLBCPrefixNumeric,    kLBCPostfixNumeric,   kLBCAlphabetic,       kLBCQuotation,                  
    kLBCOpenPunctuation,  kLBCClosePunctuation, kLBCAlphabetic,       kLBCPrefixNumeric,                  
    kLBCInfixNumeric,     kLBCHyphen,           kLBCInfixNumeric,     kLBCBreakSymbols,
    kLBCNumeric,          kLBCNumeric,          kLBCNumeric,          kLBCNumeric,
    kLBCNumeric,          kLBCNumeric,          kLBCNumeric,          kLBCNumeric,
    kLBCNumeric,          kLBCNumeric,          kLBCInfixNumeric,     kLBCInfixNumeric,
    kLBCAlphabetic,       kLBCAlphabetic,       kLBCAlphabetic,       kLBCExclamation,
    kLBCAlphabetic,       kLBCAlphabetic,       kLBCAlphabetic,       kLBCAlphabetic,
    kLBCAlphabetic,       kLBCAlphabetic,       kLBCAlphabetic,       kLBCAlphabetic,
    kLBCAlphabetic,       kLBCAlphabetic,       kLBCAlphabetic,       kLBCAlphabetic,
    kLBCAlphabetic,       kLBCAlphabetic,       kLBCAlphabetic,       kLBCAlphabetic,
    kLBCAlphabetic,       kLBCAlphabetic,       kLBCAlphabetic,       kLBCAlphabetic,
    kLBCAlphabetic,       kLBCAlphabetic,       kLBCAlphabetic,       kLBCAlphabetic,
    kLBCAlphabetic,       kLBCAlphabetic,       kLBCAlphabetic,       kLBCOpenPunctuation,
    kLBCPrefixNumeric,    kLBCClosePunctuation, kLBCAlphabetic,       kLBCAlphabetic,
    kLBCAlphabetic,       kLBCAlphabetic,       kLBCAlphabetic,       kLBCAlphabetic,
    kLBCAlphabetic,       kLBCAlphabetic,       kLBCAlphabetic,       kLBCAlphabetic,
    kLBCAlphabetic,       kLBCAlphabetic,       kLBCAlphabetic,       kLBCAlphabetic,
    kLBCAlphabetic,       kLBCAlphabetic,       kLBCAlphabetic,       kLBCAlphabetic,
    kLBCAlphabetic,       kLBCAlphabetic,       kLBCAlphabetic,       kLBCAlphabetic,
    kLBCAlphabetic,       kLBCAlphabetic,       kLBCAlphabetic,       kLBCAlphabetic,
    kLBCAlphabetic,       kLBCAlphabetic,       kLBCAlphabetic,       kLBCOpenPunctuation,
    kLBCBreakAfter,       kLBCClosePunctuation, kLBCAlphabetic,       kLBCCombiningMark,
    kLBCCombiningMark,    kLBCCombiningMark,    kLBCCombiningMark,    kLBCCombiningMark,
    kLBCCombiningMark,    kLBCNextLine,         kLBCCombiningMark,    kLBCCombiningMark,
    kLBCCombiningMark,    kLBCCombiningMark,    kLBCCombiningMark,    kLBCCombiningMark,
    kLBCCombiningMark,    kLBCCombiningMark,    kLBCCombiningMark,    kLBCCombiningMark,
    kLBCCombiningMark,    kLBCCombiningMark,    kLBCCombiningMark,    kLBCCombiningMark,
    kLBCCombiningMark,    kLBCCombiningMark,    kLBCCombiningMark,    kLBCCombiningMark,
    kLBCCombiningMark,    kLBCCombiningMark,    kLBCCombiningMark,    kLBCCombiningMark,
    kLBCCombiningMark,    kLBCCombiningMark,    kLBCCombiningMark,    kLBCCombiningMark,
    kLBCGlue,             kLBCAmbiguous,        kLBCPostfixNumeric,   kLBCPrefixNumeric,
    kLBCPrefixNumeric,    kLBCPrefixNumeric,    kLBCAlphabetic,       kLBCAmbiguous,
    kLBCAmbiguous,        kLBCAlphabetic,       kLBCAmbiguous,        kLBCQuotation,
    kLBCAlphabetic,       kLBCBreakAfter,       kLBCAlphabetic,       kLBCAlphabetic,
    kLBCPostfixNumeric,   kLBCPrefixNumeric,    kLBCAmbiguous,        kLBCAmbiguous,
    kLBCBreakBefore,      kLBCAlphabetic,       kLBCAmbiguous,        kLBCAmbiguous,
    kLBCAmbiguous,        kLBCAmbiguous,        kLBCAmbiguous,        kLBCQuotation,
    kLBCAmbiguous,        kLBCAmbiguous,        kLBCAmbiguous,        kLBCAmbiguous,
    kLBCAlphabetic,       kLBCAlphabetic,       kLBCAlphabetic,       kLBCAlphabetic,
    kLBCAlphabetic,       kLBCAlphabetic,       kLBCAlphabetic,       kLBCAlphabetic,
    kLBCAlphabetic,       kLBCAlphabetic,       kLBCAlphabetic,       kLBCAlphabetic,
    kLBCAlphabetic,       kLBCAlphabetic,       kLBCAlphabetic,       kLBCAlphabetic,
    kLBCAlphabetic,       kLBCAlphabetic,       kLBCAlphabetic,       kLBCAlphabetic,
    kLBCAlphabetic,       kLBCAlphabetic,       kLBCAlphabetic,       kLBCAmbiguous,
    kLBCAlphabetic,       kLBCAlphabetic,       kLBCAlphabetic,       kLBCAlphabetic,
    kLBCAlphabetic,       kLBCAlphabetic,       kLBCAlphabetic,       kLBCAlphabetic,
    kLBCAlphabetic,       kLBCAlphabetic,       kLBCAlphabetic,       kLBCAlphabetic,
    kLBCAlphabetic,       kLBCAlphabetic,       kLBCAlphabetic,       kLBCAlphabetic,
    kLBCAlphabetic,       kLBCAlphabetic,       kLBCAlphabetic,       kLBCAlphabetic,
    kLBCAlphabetic,       kLBCAlphabetic,       kLBCAlphabetic,       kLBCAlphabetic,
    kLBCAlphabetic,       kLBCAlphabetic,       kLBCAlphabetic,       kLBCAlphabetic,
    kLBCAlphabetic,       kLBCAlphabetic,       kLBCAlphabetic,       kLBCAmbiguous,
    kLBCAlphabetic,       kLBCAlphabetic,       kLBCAlphabetic,       kLBCAlphabetic,
    kLBCAlphabetic,       kLBCAlphabetic,       kLBCAlphabetic,       kLBCAlphabetic
};


#if (defined(EATEXT_HIRAGANA_SCRIPT_SUPPORTED) && EATEXT_HIRAGANA_SCRIPT_SUPPORTED) || \
    (defined(EATEXT_KATAKANA_SCRIPT_SUPPORTED) && EATEXT_KATAKANA_SCRIPT_SUPPORTED)

    // These define the line break categories of the characters in the range of [0x3000, 0x3100).
    // These characters consist primarily of the Japanese Hirigana and Katakana characters.
    const char gLineBreakCategoryTableHK[0x0100] = 
    {
        kLBCIdeographic,      kLBCClosePunctuation, kLBCClosePunctuation, kLBCIdeographic, 
        kLBCIdeographic,      kLBCNonStarter,       kLBCIdeographic,      kLBCIdeographic, 
        kLBCOpenPunctuation,  kLBCClosePunctuation, kLBCOpenPunctuation,  kLBCClosePunctuation, 
        kLBCOpenPunctuation,  kLBCClosePunctuation, kLBCOpenPunctuation,  kLBCClosePunctuation, 
        kLBCOpenPunctuation,  kLBCClosePunctuation, kLBCIdeographic,      kLBCIdeographic, 
        kLBCOpenPunctuation,  kLBCClosePunctuation, kLBCOpenPunctuation,  kLBCClosePunctuation, 
        kLBCOpenPunctuation,  kLBCClosePunctuation, kLBCOpenPunctuation,  kLBCClosePunctuation, 
        kLBCNonStarter,       kLBCOpenPunctuation,  kLBCClosePunctuation, kLBCClosePunctuation, 
        kLBCIdeographic,      kLBCIdeographic,      kLBCIdeographic,      kLBCIdeographic,
        kLBCIdeographic,      kLBCIdeographic,      kLBCIdeographic,      kLBCIdeographic,
        kLBCIdeographic,      kLBCIdeographic,      kLBCCombiningMark,    kLBCCombiningMark,
        kLBCCombiningMark,    kLBCCombiningMark,    kLBCCombiningMark,    kLBCCombiningMark,
        kLBCIdeographic,      kLBCIdeographic,      kLBCIdeographic,      kLBCIdeographic,
        kLBCIdeographic,      kLBCIdeographic,      kLBCIdeographic,      kLBCIdeographic,
        kLBCIdeographic,      kLBCIdeographic,      kLBCIdeographic,      kLBCNonStarter ,
        kLBCNonStarter,       kLBCIdeographic,      kLBCIdeographic,      kLBCIdeographic,
        kLBCAlphabetic,       kLBCNonStarter,       kLBCIdeographic,      kLBCNonStarter ,
        kLBCIdeographic,      kLBCNonStarter,       kLBCIdeographic,      kLBCNonStarter,
        kLBCIdeographic,      kLBCNonStarter,       kLBCIdeographic,      kLBCIdeographic,
        kLBCIdeographic,      kLBCIdeographic,      kLBCIdeographic,      kLBCIdeographic,
        kLBCIdeographic,      kLBCIdeographic,      kLBCIdeographic,      kLBCIdeographic,
        kLBCIdeographic,      kLBCIdeographic,      kLBCIdeographic,      kLBCIdeographic,
        kLBCIdeographic,      kLBCIdeographic,      kLBCIdeographic,      kLBCIdeographic,
        kLBCIdeographic,      kLBCIdeographic,      kLBCIdeographic,      kLBCIdeographic,
        kLBCIdeographic,      kLBCIdeographic,      kLBCIdeographic,      kLBCNonStarter,
        kLBCIdeographic,      kLBCIdeographic,      kLBCIdeographic,      kLBCIdeographic,
        kLBCIdeographic,      kLBCIdeographic,      kLBCIdeographic,      kLBCIdeographic,
        kLBCIdeographic,      kLBCIdeographic,      kLBCIdeographic,      kLBCIdeographic,
        kLBCIdeographic,      kLBCIdeographic,      kLBCIdeographic,      kLBCIdeographic,
        kLBCIdeographic,      kLBCIdeographic,      kLBCIdeographic,      kLBCIdeographic,
        kLBCIdeographic,      kLBCIdeographic,      kLBCIdeographic,      kLBCIdeographic,
        kLBCIdeographic,      kLBCIdeographic,      kLBCIdeographic,      kLBCIdeographic,
        kLBCIdeographic,      kLBCIdeographic,      kLBCIdeographic,      kLBCNonStarter,
        kLBCIdeographic,      kLBCNonStarter,       kLBCIdeographic,      kLBCNonStarter,
        kLBCIdeographic,      kLBCIdeographic,      kLBCIdeographic,      kLBCIdeographic,
        kLBCIdeographic,      kLBCIdeographic,      kLBCNonStarter,       kLBCIdeographic,
        kLBCIdeographic,      kLBCIdeographic,      kLBCIdeographic,      kLBCIdeographic,
        kLBCIdeographic,      kLBCNonStarter,       kLBCNonStarter,       kLBCAlphabetic,
        kLBCAlphabetic,       kLBCCombiningMark,    kLBCCombiningMark,    kLBCNonStarter,
        kLBCNonStarter,       kLBCNonStarter,       kLBCNonStarter,       kLBCIdeographic,
        kLBCNonStarter,       kLBCNonStarter,       kLBCIdeographic,      kLBCNonStarter,
        kLBCIdeographic,      kLBCNonStarter,       kLBCIdeographic,      kLBCNonStarter,
        kLBCIdeographic,      kLBCNonStarter,       kLBCIdeographic,      kLBCIdeographic,
        kLBCIdeographic,      kLBCIdeographic,      kLBCIdeographic,      kLBCIdeographic,
        kLBCIdeographic,      kLBCIdeographic,      kLBCIdeographic,      kLBCIdeographic,
        kLBCIdeographic,      kLBCIdeographic,      kLBCIdeographic,      kLBCIdeographic,
        kLBCIdeographic,      kLBCIdeographic,      kLBCIdeographic,      kLBCIdeographic,
        kLBCIdeographic,      kLBCIdeographic,      kLBCIdeographic,      kLBCIdeographic,
        kLBCIdeographic,      kLBCIdeographic,      kLBCIdeographic,      kLBCNonStarter,
        kLBCIdeographic,      kLBCIdeographic,      kLBCIdeographic,      kLBCIdeographic,
        kLBCIdeographic,      kLBCIdeographic,      kLBCIdeographic,      kLBCIdeographic,
        kLBCIdeographic,      kLBCIdeographic,      kLBCIdeographic,      kLBCIdeographic,
        kLBCIdeographic,      kLBCIdeographic,      kLBCIdeographic,      kLBCIdeographic,
        kLBCIdeographic,      kLBCIdeographic,      kLBCIdeographic,      kLBCIdeographic,
        kLBCIdeographic,      kLBCIdeographic,      kLBCIdeographic,      kLBCIdeographic,
        kLBCIdeographic,      kLBCIdeographic,      kLBCIdeographic,      kLBCIdeographic,
        kLBCIdeographic,      kLBCIdeographic,      kLBCIdeographic,      kLBCNonStarter,
        kLBCIdeographic,      kLBCNonStarter ,      kLBCIdeographic,      kLBCNonStarter,
        kLBCIdeographic,      kLBCIdeographic,      kLBCIdeographic,      kLBCIdeographic,
        kLBCIdeographic,      kLBCIdeographic,      kLBCNonStarter,       kLBCIdeographic,
        kLBCIdeographic,      kLBCIdeographic,      kLBCIdeographic,      kLBCIdeographic,
        kLBCIdeographic,      kLBCNonStarter,       kLBCNonStarter ,      kLBCIdeographic,
        kLBCIdeographic,      kLBCIdeographic,      kLBCIdeographic,      kLBCNonStarter,
        kLBCNonStarter,       kLBCNonStarter,       kLBCNonStarter,       kLBCIdeographic
    };

    // These define the line break categories of the characters in the range of [0xFF00, 0xFFFF].
    // These characters consist of (Japanese) full-width (ASCII) and half-width (Hirigana, Katakana) forms of characters.
    const char gLineBreakCategoryTableHF[0x0100] = 
    {
        kLBCIdeographic,      kLBCExclamation,      kLBCIdeographic,      kLBCIdeographic,      
        kLBCPrefixNumeric,    kLBCPostfixNumeric,   kLBCIdeographic,      kLBCIdeographic,      
        kLBCOpenPunctuation,  kLBCClosePunctuation, kLBCIdeographic,      kLBCIdeographic,      
        kLBCClosePunctuation, kLBCIdeographic,      kLBCClosePunctuation, kLBCIdeographic,      
        kLBCIdeographic,      kLBCIdeographic,      kLBCIdeographic,      kLBCIdeographic,      
        kLBCIdeographic,      kLBCIdeographic,      kLBCIdeographic,      kLBCIdeographic,      
        kLBCIdeographic,      kLBCIdeographic,      kLBCNonStarter,       kLBCNonStarter,       
        kLBCIdeographic,      kLBCIdeographic,      kLBCIdeographic,      kLBCExclamation,      
        kLBCIdeographic,      kLBCIdeographic,      kLBCIdeographic,      kLBCIdeographic,      
        kLBCIdeographic,      kLBCIdeographic,      kLBCIdeographic,      kLBCIdeographic,      
        kLBCIdeographic,      kLBCIdeographic,      kLBCIdeographic,      kLBCIdeographic,      
        kLBCIdeographic,      kLBCIdeographic,      kLBCIdeographic,      kLBCIdeographic,      
        kLBCIdeographic,      kLBCIdeographic,      kLBCIdeographic,      kLBCIdeographic,      
        kLBCIdeographic,      kLBCIdeographic,      kLBCIdeographic,      kLBCIdeographic,      
        kLBCIdeographic,      kLBCIdeographic,      kLBCIdeographic,      kLBCOpenPunctuation,  
        kLBCIdeographic,      kLBCClosePunctuation, kLBCIdeographic,      kLBCIdeographic,      
        kLBCIdeographic,      kLBCIdeographic,      kLBCIdeographic,      kLBCIdeographic,      
        kLBCIdeographic,      kLBCIdeographic,      kLBCIdeographic,      kLBCIdeographic,      
        kLBCIdeographic,      kLBCIdeographic,      kLBCIdeographic,      kLBCIdeographic,      
        kLBCIdeographic,      kLBCIdeographic,      kLBCIdeographic,      kLBCIdeographic,      
        kLBCIdeographic,      kLBCIdeographic,      kLBCIdeographic,      kLBCIdeographic,      
        kLBCIdeographic,      kLBCIdeographic,      kLBCIdeographic,      kLBCIdeographic,      
        kLBCIdeographic,      kLBCIdeographic,      kLBCIdeographic,      kLBCOpenPunctuation,  
        kLBCIdeographic,      kLBCClosePunctuation, kLBCIdeographic,      kLBCOpenPunctuation,  
        kLBCClosePunctuation, kLBCClosePunctuation, kLBCOpenPunctuation,  kLBCClosePunctuation, 
        kLBCClosePunctuation, kLBCNonStarter,       kLBCAlphabetic,       kLBCNonStarter,       
        kLBCNonStarter,       kLBCNonStarter,       kLBCNonStarter,       kLBCNonStarter,       
        kLBCNonStarter,       kLBCNonStarter,       kLBCNonStarter,       kLBCNonStarter,       
        kLBCNonStarter,       kLBCAlphabetic,       kLBCAlphabetic,       kLBCAlphabetic,       
        kLBCAlphabetic,       kLBCAlphabetic,       kLBCAlphabetic,       kLBCAlphabetic,       
        kLBCAlphabetic,       kLBCAlphabetic,       kLBCAlphabetic,       kLBCAlphabetic,       
        kLBCAlphabetic,       kLBCAlphabetic,       kLBCAlphabetic,       kLBCAlphabetic,       
        kLBCAlphabetic,       kLBCAlphabetic,       kLBCAlphabetic,       kLBCAlphabetic,       
        kLBCAlphabetic,       kLBCAlphabetic,       kLBCAlphabetic,       kLBCAlphabetic,       
        kLBCAlphabetic,       kLBCAlphabetic,       kLBCAlphabetic,       kLBCAlphabetic,       
        kLBCAlphabetic,       kLBCAlphabetic,       kLBCAlphabetic,       kLBCAlphabetic,       
        kLBCAlphabetic,       kLBCAlphabetic,       kLBCAlphabetic,       kLBCAlphabetic,       
        kLBCAlphabetic,       kLBCAlphabetic,       kLBCAlphabetic,       kLBCAlphabetic,       
        kLBCAlphabetic,       kLBCAlphabetic,       kLBCAlphabetic,       kLBCAlphabetic,       
        kLBCAlphabetic,       kLBCAlphabetic,       kLBCNonStarter,       kLBCNonStarter,       
        kLBCAlphabetic,       kLBCAlphabetic,       kLBCAlphabetic,       kLBCAlphabetic,       
        kLBCAlphabetic,       kLBCAlphabetic,       kLBCAlphabetic,       kLBCAlphabetic,       
        kLBCAlphabetic,       kLBCAlphabetic,       kLBCAlphabetic,       kLBCAlphabetic,       
        kLBCAlphabetic,       kLBCAlphabetic,       kLBCAlphabetic,       kLBCAlphabetic,       
        kLBCAlphabetic,       kLBCAlphabetic,       kLBCAlphabetic,       kLBCAlphabetic,       
        kLBCAlphabetic,       kLBCAlphabetic,       kLBCAlphabetic,       kLBCAlphabetic,       
        kLBCAlphabetic,       kLBCAlphabetic,       kLBCAlphabetic,       kLBCAlphabetic,       
        kLBCAlphabetic,       kLBCAlphabetic,       kLBCAlphabetic,       kLBCIdeographic,      
        kLBCIdeographic,      kLBCIdeographic,      kLBCAlphabetic,       kLBCAlphabetic,       
        kLBCAlphabetic,       kLBCAlphabetic,       kLBCAlphabetic,       kLBCAlphabetic,       
        kLBCIdeographic,      kLBCIdeographic,      kLBCAlphabetic,       kLBCAlphabetic,       
        kLBCAlphabetic,       kLBCAlphabetic,       kLBCAlphabetic,       kLBCAlphabetic,       
        kLBCIdeographic,      kLBCIdeographic,      kLBCAlphabetic,       kLBCAlphabetic,       
        kLBCAlphabetic,       kLBCAlphabetic,       kLBCAlphabetic,       kLBCAlphabetic,       
        kLBCIdeographic,      kLBCIdeographic,      kLBCAlphabetic,       kLBCAlphabetic,       
        kLBCAlphabetic,       kLBCIdeographic,      kLBCIdeographic,      kLBCIdeographic,      
        kLBCPostfixNumeric,   kLBCPrefixNumeric,    kLBCIdeographic,      kLBCIdeographic,      
        kLBCIdeographic,      kLBCPrefixNumeric,    kLBCPrefixNumeric,    kLBCIdeographic,      
        kLBCAlphabetic,       kLBCAlphabetic,       kLBCAlphabetic,       kLBCAlphabetic,       
        kLBCAlphabetic,       kLBCAlphabetic,       kLBCAlphabetic,       kLBCIdeographic,      
        kLBCIdeographic,      kLBCIdeographic,      kLBCIdeographic,      kLBCIdeographic,      
        kLBCIdeographic,      kLBCIdeographic,      kLBCIdeographic,      kLBCIdeographic,      
        kLBCIdeographic,      kLBCCombiningMark,    kLBCCombiningMark,    kLBCCombiningMark,    
        kLBCContingentBreak,  kLBCAmbiguous,        kLBCZWSpace,          kLBCZWSpace
    };

#endif


// These define the line break categories of the characters in the range of [0x2000u, 0x2061u].
//
const char gLineBreakCategoryTableGP[0x0062] = 
{
    kLBCBreakAfter,       kLBCBreakAfter,       kLBCBreakAfter,       kLBCBreakAfter,       
    kLBCBreakAfter,       kLBCBreakAfter,       kLBCBreakAfter,       kLBCGlue,             
    kLBCBreakAfter,       kLBCBreakAfter,       kLBCBreakAfter,       kLBCZWSpace,          
    kLBCCombiningMark,    kLBCCombiningMark,    kLBCCombiningMark,    kLBCCombiningMark,    
    kLBCBreakAfter,       kLBCGlue,             kLBCBreakAfter,       kLBCBreakAfter,       
    kLBCBreakBoth,        kLBCAmbiguous,        kLBCAmbiguous,        kLBCAlphabetic,       
    kLBCQuotation,        kLBCQuotation,        kLBCOpenPunctuation,  kLBCQuotation,        
    kLBCQuotation,        kLBCQuotation,        kLBCOpenPunctuation,  kLBCQuotation,        
    kLBCAmbiguous,        kLBCAmbiguous,        kLBCAlphabetic,       kLBCAlphabetic,       
    kLBCInseperable,      kLBCInseperable,      kLBCInseperable,      kLBCBreakAfter,       
    kLBCMandatoryBreak,   kLBCMandatoryBreak,   kLBCCombiningMark,    kLBCCombiningMark,    
    kLBCCombiningMark,    kLBCCombiningMark,    kLBCCombiningMark,    kLBCGlue,             
    kLBCPostfixNumeric,   kLBCPostfixNumeric,   kLBCPostfixNumeric,   kLBCPostfixNumeric,   
    kLBCPostfixNumeric,   kLBCPostfixNumeric,   kLBCPostfixNumeric,   kLBCPostfixNumeric,   
    kLBCAlphabetic,       kLBCQuotation,        kLBCQuotation,        kLBCAmbiguous,        
    kLBCNonStarter,       kLBCAlphabetic,       kLBCAlphabetic,       kLBCAlphabetic,       
    kLBCAlphabetic,       kLBCAlphabetic,       kLBCAlphabetic,       kLBCAlphabetic,       
    kLBCInfixNumeric,     kLBCOpenPunctuation,  kLBCClosePunctuation, kLBCAlphabetic,       
    kLBCAlphabetic,       kLBCAlphabetic,       kLBCAlphabetic,       kLBCAlphabetic,       
    kLBCAlphabetic,       kLBCAlphabetic,       kLBCAlphabetic,       kLBCAlphabetic,       
    kLBCAlphabetic,       kLBCAlphabetic,       kLBCAlphabetic,       kLBCAlphabetic,       
    kLBCAlphabetic,       kLBCAlphabetic,       kLBCBreakAfter,       kLBCAlphabetic,       
    kLBCBreakAfter,       kLBCBreakAfter,       kLBCBreakAfter,       kLBCBreakAfter,       
    kLBCAlphabetic,       kLBCBreakAfter,       kLBCBreakAfter,       kLBCBreakAfter,       
    kLBCWordJoiner,       kLBCAlphabetic,       
};


#if defined(EATEXT_THAI_SCRIPT_SUPPORTED) && EATEXT_THAI_SCRIPT_SUPPORTED
    // These define the line break categories of the characters in the range of [0x0E00, 0x0E5C).
    const char gLineBreakCategoryTableThai[0x005C] = 
    {
        kLBCSoutheastAsian,  kLBCSoutheastAsian,  kLBCSoutheastAsian,  kLBCSoutheastAsian,  // 0x0E00
        kLBCSoutheastAsian,  kLBCSoutheastAsian,  kLBCSoutheastAsian,  kLBCSoutheastAsian,  // 0x0E04
        kLBCSoutheastAsian,  kLBCSoutheastAsian,  kLBCSoutheastAsian,  kLBCSoutheastAsian,  // 0x0E08
        kLBCSoutheastAsian,  kLBCSoutheastAsian,  kLBCSoutheastAsian,  kLBCSoutheastAsian,  // 0x0E0C
        kLBCSoutheastAsian,  kLBCSoutheastAsian,  kLBCSoutheastAsian,  kLBCSoutheastAsian,  // 0x0E10
        kLBCSoutheastAsian,  kLBCSoutheastAsian,  kLBCSoutheastAsian,  kLBCSoutheastAsian,  // 0x0E14
        kLBCSoutheastAsian,  kLBCSoutheastAsian,  kLBCSoutheastAsian,  kLBCSoutheastAsian,  // 0x0E18
        kLBCSoutheastAsian,  kLBCSoutheastAsian,  kLBCSoutheastAsian,  kLBCSoutheastAsian,  // 0x0E1C
        kLBCSoutheastAsian,  kLBCSoutheastAsian,  kLBCSoutheastAsian,  kLBCSoutheastAsian,  // 0x0E20
        kLBCSoutheastAsian,  kLBCSoutheastAsian,  kLBCSoutheastAsian,  kLBCSoutheastAsian,  // 0x0E24
        kLBCSoutheastAsian,  kLBCSoutheastAsian,  kLBCSoutheastAsian,  kLBCSoutheastAsian,  // 0x0E28
        kLBCSoutheastAsian,  kLBCSoutheastAsian,  kLBCSoutheastAsian,  kLBCSoutheastAsian,  // 0x0E2C
        kLBCSoutheastAsian,  kLBCCombiningMark,   kLBCSoutheastAsian,  kLBCSoutheastAsian,  // 0x0E30
        kLBCCombiningMark,   kLBCCombiningMark,   kLBCCombiningMark,   kLBCCombiningMark,   // 0x0E34
        kLBCCombiningMark,   kLBCCombiningMark,   kLBCCombiningMark,   kLBCSoutheastAsian,  // 0x0E38
        kLBCSoutheastAsian,  kLBCSoutheastAsian,  kLBCSoutheastAsian,  kLBCPrefixNumeric,   // 0x0E3C
        kLBCSoutheastAsian,  kLBCSoutheastAsian,  kLBCSoutheastAsian,  kLBCSoutheastAsian,  // 0x0E40
        kLBCSoutheastAsian,  kLBCSoutheastAsian,  kLBCSoutheastAsian,  kLBCCombiningMark,   // 0x0E44
        kLBCCombiningMark,   kLBCCombiningMark,   kLBCCombiningMark,   kLBCCombiningMark,   // 0x0E48
        kLBCCombiningMark,   kLBCCombiningMark,   kLBCCombiningMark,   kLBCAlphabetic,      // 0x0E4C
        kLBCNumeric,         kLBCNumeric,         kLBCNumeric,         kLBCNumeric,         // 0x0E50
        kLBCNumeric,         kLBCNumeric,         kLBCNumeric,         kLBCNumeric,         // 0x0E54
        kLBCNumeric,         kLBCNumeric,         kLBCBreakAfter,      kLBCBreakAfter       // 0x0E58
    };
#endif


#if defined(EATEXT_DEVANAGARI_SCRIPT_SUPPORTED) && EATEXT_DEVANAGARI_SCRIPT_SUPPORTED
    // These define the line break categories of the characters in the range of [0x0900, 0x097e).
    const char gLineBreakCategoryTableDevanagari[0x007e] = 
    {
        kLBCCombiningMark,  kLBCCombiningMark,  kLBCCombiningMark,  kLBCCombiningMark, 
        kLBCAlphabetic,     kLBCAlphabetic,     kLBCAlphabetic,     kLBCAlphabetic,    
        kLBCAlphabetic,     kLBCAlphabetic,     kLBCAlphabetic,     kLBCAlphabetic,    
        kLBCAlphabetic,     kLBCAlphabetic,     kLBCAlphabetic,     kLBCAlphabetic,    
        kLBCAlphabetic,     kLBCAlphabetic,     kLBCAlphabetic,     kLBCAlphabetic,    
        kLBCAlphabetic,     kLBCAlphabetic,     kLBCAlphabetic,     kLBCAlphabetic,    
        kLBCAlphabetic,     kLBCAlphabetic,     kLBCAlphabetic,     kLBCAlphabetic,    
        kLBCAlphabetic,     kLBCAlphabetic,     kLBCAlphabetic,     kLBCAlphabetic,    
        kLBCAlphabetic,     kLBCAlphabetic,     kLBCAlphabetic,     kLBCAlphabetic,    
        kLBCAlphabetic,     kLBCAlphabetic,     kLBCAlphabetic,     kLBCAlphabetic,    
        kLBCAlphabetic,     kLBCAlphabetic,     kLBCAlphabetic,     kLBCAlphabetic,    
        kLBCAlphabetic,     kLBCAlphabetic,     kLBCAlphabetic,     kLBCAlphabetic,    
        kLBCAlphabetic,     kLBCAlphabetic,     kLBCAlphabetic,     kLBCAlphabetic,    
        kLBCAlphabetic,     kLBCAlphabetic,     kLBCAlphabetic,     kLBCAlphabetic,    
        kLBCAlphabetic,     kLBCAlphabetic,     kLBCAlphabetic,     kLBCAlphabetic,    
        kLBCCombiningMark,  kLBCAlphabetic,     kLBCCombiningMark,  kLBCCombiningMark, 
        kLBCCombiningMark,  kLBCCombiningMark,  kLBCCombiningMark,  kLBCCombiningMark, 
        kLBCCombiningMark,  kLBCCombiningMark,  kLBCCombiningMark,  kLBCCombiningMark, 
        kLBCCombiningMark,  kLBCCombiningMark,  kLBCCombiningMark,  kLBCCombiningMark, 
        kLBCCombiningMark,  kLBCCombiningMark,  kLBCAlphabetic,     kLBCAlphabetic,    
        kLBCAlphabetic,     kLBCCombiningMark,  kLBCCombiningMark,  kLBCCombiningMark, 
        kLBCCombiningMark,  kLBCAlphabetic,     kLBCAlphabetic,     kLBCAlphabetic,    
        kLBCAlphabetic,     kLBCAlphabetic,     kLBCAlphabetic,     kLBCAlphabetic,    
        kLBCAlphabetic,     kLBCAlphabetic,     kLBCAlphabetic,     kLBCAlphabetic,    
        kLBCAlphabetic,     kLBCAlphabetic,     kLBCCombiningMark,  kLBCCombiningMark, 
        kLBCBreakAfter,     kLBCBreakAfter,     kLBCNumeric,        kLBCNumeric,       
        kLBCNumeric,        kLBCNumeric,        kLBCNumeric,        kLBCNumeric,       
        kLBCNumeric,        kLBCNumeric,        kLBCNumeric,        kLBCNumeric,       
        kLBCAlphabetic,     kLBCAlphabetic,     kLBCAlphabetic,     kLBCAlphabetic,    
        kLBCAlphabetic,     kLBCAlphabetic,     kLBCAlphabetic,     kLBCAlphabetic,    
        kLBCAlphabetic,     kLBCAlphabetic,     kLBCAlphabetic,     kLBCAlphabetic,    
        kLBCAlphabetic,     kLBCAlphabetic
    };
#endif


struct CombiningClassEntry
{
    uint16_t mnBegin;
    uint8_t  mnCount;
    uint8_t  mnClass;
};

CombiningClassEntry gCombiningClassEntryArray[] = 
{ 
    // Char  count  class
    { 0x0300, 21,  230 }, // Canonical_Combining_Class=Above
    { 0x0315,  1,  232 }, // Canonical_Combining_Class=Above_Right
    { 0x0316,  4,  220 }, // Canonical_Combining_Class=Below
    { 0x031A,  1,  232 },
    { 0x031B,  1,  216 },
    { 0x031C,  5,  220 },
    { 0x0321,  2,  202 },
    { 0x0323,  4,  220 },
    { 0x0327,  2,  202 },
    { 0x0329, 11,  220 },
    { 0x0334,  5,    1 }, // Canonical_Combining_Class=Overlay
    { 0x0339,  4,  220 },
    { 0x033D,  8,  230 },
    { 0x0345,  1,  240 }, // Canonical_Combining_Class=Iota_Subscript
    { 0x0346,  1,  230 },
    { 0x0347,  3,  220 },
    { 0x034A,  3,  230 },
    { 0x034D,  2,  220 },
    { 0x0350,  3,  230 },
    { 0x0353,  4,  220 },
    { 0x0357,  1,  230 },
    { 0x0358,  1,  232 },
    { 0x0359,  2,  220 },
    { 0x035B,  1,  230 },
    { 0x035C,  1,  233 }, // Canonical_Combining_Class=Double_Below
    { 0x035D,  2,  234 }, // Canonical_Combining_Class=Double_Above
    { 0x035F,  1,  233 },
    { 0x0360,  2,  234 },
    { 0x0362,  1,  233 },
    { 0x0363, 13,  230 },

    { 0x0483,  4,  230 },

    { 0x0591,  1,  220 },
    { 0x0592,  4,  230 },
    { 0x0596,  1,  220 },
    { 0x0597,  3,  230 },
    { 0x059A,  1,  222 }, // Canonical_Combining_Class=Below_Right
    { 0x059B,  1,  220 },
    { 0x059C,  6,  230 },
    { 0x05A2,  6,  220 },
    { 0x05A8,  2,  230 },
    { 0x05AA,  1,  220 },
    { 0x05AB,  2,  230 },
    { 0x05AD,  1,  222 },
    { 0x05AE,  1,  228 }, // Canonical_Combining_Class=Above_Left
    { 0x05AF,  1,  230 },
    { 0x05B0,  1,   10 },
    { 0x05B1,  1,   11 },
    { 0x05B2,  1,   12 },
    { 0x05B3,  1,   13 },
    { 0x05B4,  1,   14 },
    { 0x05B5,  1,   15 },
    { 0x05B6,  1,   16 },
    { 0x05B7,  1,   17 },
    { 0x05B8,  1,   18 },
    { 0x05B9,  1,   19 },
    { 0x05BB,  1,   20 },
    { 0x05BC,  1,   21 },
    { 0x05BD,  1,   22 },
    { 0x05BF,  1,   23 },
    { 0x05C1,  1,   24 },
    { 0x05C2,  1,   25 },
    { 0x05C4,  1,  230 },
    { 0x05C5,  1,  220 },
    { 0x05C7,  1,   18 },

    { 0x0610,  6,  230 },
    { 0x064B,  1,   27 },
    { 0x064C,  1,   28 },
    { 0x064D,  1,   29 },
    { 0x064E,  1,   30 },
    { 0x064F,  1,   31 },
    { 0x0650,  1,   32 },
    { 0x0651,  1,   33 },
    { 0x0652,  1,   34 },
    { 0x0653,  2,  230 },
    { 0x0655,  2,  220 },
    { 0x0657,  5,  230 },
    { 0x065C,  1,  220 },
    { 0x065D,  2,  230 },
    { 0x0670,  1,   35 },
    { 0x06D6,  7,  230 },
    { 0x06DF,  4,  230 },
    { 0x06E3,  1,  220 },
    { 0x06E4,  1,  230 },
    { 0x06E7,  2,  230 },
    { 0x06EA,  1,  220 },
    { 0x06EB,  2,  230 },
    { 0x06ED,  1,  220 },

    { 0x093C,  1,    7 }, // Canonical_Combining_Class=Nukta
    { 0x094D,  1,    9 }, // Canonical_Combining_Class=Virama
    { 0x0951,  1,  230 },
    { 0x0952,  1,  220 },
    { 0x0953,  2,  230 },

    { 0x0E38,  2,  103 },
    { 0x0E48,  4,  107 },

    { 0x1DC0,  2,  230 },
    { 0x1DC2,  1,  220 },
    { 0x1DC3,  1,  230 },

    { 0x20D0,  2,  230 },
    { 0x20D2,  2,    1 },
    { 0x20D4,  4,  230 },
    { 0x20D8,  3,    1 },
    { 0x20DB,  2,  230 },
    { 0x20E1,  1,  230 },
    { 0x20E5,  2,    1 },
    { 0x20E7,  1,  230 },
    { 0x20E8,  1,  220 },
    { 0x20E9,  1,  230 },
    { 0x20EA,  2,    1 },

    { 0x302A,  1,  218 },
    { 0x302B,  1,  228 },
    { 0x302C,  1,  232 },
    { 0x302D,  1,  222 },
    { 0x302E,  2,  224 }, // Canonical_Combining_Class=Left
    { 0x3099,  2,    8 }, // Canonical_Combining_Class=Kana_Voicing
};





} // namespace Text

} // namespace EA









