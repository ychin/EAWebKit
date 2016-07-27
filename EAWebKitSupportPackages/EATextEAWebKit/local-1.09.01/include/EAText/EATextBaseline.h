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
// EATextBaseline.h
//
// Maintained by Paul Pedriana and Talin.
//
///////////////////////////////////////////////////////////////////////////////

///////////////////////////////////////////////////////////////////////////////
// This module implements an adapter to Typesetter which causes text to 
// follow an arbitrary baseline path as opposed to a horizontal line.
///////////////////////////////////////////////////////////////////////////////


#ifndef EATEXT_EATEXTBASELINE_H
#define EATEXT_EATEXTBASELINE_H


#include <EABase/eabase.h>
#include <EAText/EAText.h>
#include <EAText/EATextTypesetter.h>


namespace EA
{
    namespace Text
    {
        /// GlyphLayoutInfoEx
        ///
        /// Provides additional information for non-axis-aligned glyphs in 2D.
        /// This is useful for drawing non-uniformly rotated glyphs in a way that
        /// can't be solved with a single tranform for a glyph array. An example
        /// of this is a line of text that follows a curve of some sort.
        ///
        /// We follow the following conventions, demonstrated with a tilted M glyph.
        ///
        ///             [x2,y2]
        /// [x1,y1]    *
        ///       *    **
        ///        ** *  *  
        ///         *     * [x3,y3]
        ///          *
        ///        [x4,y4]
        ///
        struct EATEXT_API GlyphLayoutInfoEx : public GlyphLayoutInfo // 48 bytes
        {
            float mfX3;
            float mfY3;
            float mfX4;
            float mfY4;
        };


        /// BaselineIterator
        ///
        /// Generic baseline position iterator.
        /// It is used by the BaselineLayout class to set glyph positions.
        ///
        /// The 'T' value of a line/curve refers to a value in the range
        /// of [0,1] and which defines a position along the curve from its
        /// beginning to its end.
        ///
        class EATEXT_API BaselineIterator
        {
        public:
            virtual ~BaselineIterator() { }

            virtual float GetT() const = 0;
            virtual void  SetT(float fT) = 0;

            virtual float GetDistance() const = 0;
            virtual void  SetDistance(float fDistance) = 0;
            virtual void  MoveDistance(float fDistance) = 0;

            virtual float GetBaselineLength() const = 0;
            virtual void  GetPosition(Point& pPoint, float& fTangentAngle) = 0;
        };



        /// Bezier2D
        ///
        /// Implements a basic 2D four control point Bezier segment.
        /// You can have text follow just about any basic curve with a Bezier curve.
        /// Note that complex curves cannot be done with a single four point curve.
        /// To make complex curves you need to either string together individual
        /// curves (like drawing programs do) or you need to use a curve that has
        /// more than four control points. Feel free to contact the author of this
        /// package if you want code that does either of these.
        ///
        class EATEXT_API Bezier2D
        {
        public:
            Bezier2D();

            /// Sets the Bezier curve parameters.
            void Set(const Point* pControlPointArray);

            /// Returns the point associated with the given t value, where t is a 
            /// value in the range of [0,1] and refers to a position along the curve
            /// from the first point to the last point.
            void GetPoint(float t, Point& pt);

        public:
            Point mControlPoints[4];    // The standard Bezier control points. 
            float mCurveLength;         // Cached curve length. Allows for fast positioning along the curve.
            bool  mbLinearMovement;     // If true, then changes in T are directly proportional to changes in distance along the entire curve. This allows for faster calculations, but requires the user to supply a symmetrically placed set of control points.
        };



        /// Bezier2DIterator
        ///
        /// Iterates positions along a Bezier curve.
        ///
        class EATEXT_API Bezier2DIterator : public BaselineIterator
        {
        public:
            Bezier2DIterator();

            void  SetBezier(const Bezier2D& bezier2D);

            float GetT() const;
            void  SetT(float t);

            float GetDistance() const;
            void  SetDistance(float fDistance);
            void  MoveDistance(float fDistance);

            float GetBaselineLength() const;
            void  GetPosition(Point& pt, float& fTangentAngle);

            // Makes a straight line of an arbitrary angle. If the line goes backwards
            // then the result will be text that is upside-down and goes from right-to-left
            // underneath the line.
            void MakeLine(const Point& ptBegin, const Point& ptEnd);

            // Makes a upward-bowed semicircle whereby text follows the outside of the 
            // crescent from left to right like a sports player's jersey. The angle 
            // specifies where the top of the semicircle is. An angle of zero means
            // the top points directly along the positive y axis. Angles are in radians.
            void MakeSemicircle(const Point& ptCenter, float fRadius, float fAngle = 0.f, bool bClockwise = true);

        public:
            Bezier2D  mBezier;
            float     mT;
            float     mDistance;
            Point     mPoint;
            float     mTangentAngle;
        };



        /// BaselineLayout
        ///
        /// Modifies glyphs along a horizontal baseline to follow an arbitrary 
        /// alternate baseline such as a curve or slope. Such baseline curvature 
        /// can in practice work only on text that composed of independent glyphs
        /// and not text that is cursive (i.e. like handwriting). Arabic text is
        /// unilaterally cursive and cannot in any practicality be successfully
        /// transformed by this kind of operation. It would be simply too hard
        /// to make the glyph-based cursive script follow the baseline smoothly.
        ///
        /// This class requires the LineLayout glyphs to be in visual order as 
        /// opposed to logical (typewritten) order. Unless you have bidirectional
        /// text such as Hebrew or Arabic, visual order will be the same as 
        /// logical order. There is a utility function to convert a LineLayout to
        /// visual order if you do in fact have bidirectional text.
        ///
        /// Example usage:
        ///     Bezier2DIterator it;
        ///     BaselineLayout   baselineLayout;
        ///
        ///     it.MakeLine(Point(10, 10), Point(200, 200));
        ///     baselineLayout.ModifyBaseline(pSomeLineLayout, pGlyphLayoutInfoArray, &it, 0.f, kHALeft);
        ///
        class EATEXT_API BaselineLayout
        {
        public:
            BaselineLayout();

            /// Modifies the existing glyph set in the LineLayout to follow the baseline
            /// controlled by the BaselineIterator. The initial LineLayout is expected
            /// to be horizontal and with an initial pen position of [0,0] and thus not 
            /// previously modified.
            /// The fBaseline argument specifies how the glyph string should follow the 
            /// baseline. A value of 0 means the glyph baseline should follow the baseline.
            /// A value of font-height means the top of the glyph string should follow
            /// the baseline. The former is best for text following the top of circular
            /// curve, while the latter is best for following the bottom of a circular curve.
            /// The HAlignment specifies the desired alignment along the baseline.
            /// The Direction specifies the base direction of the text, which is going
            /// to be left-to-right for most languages and right-to-left for languages
            /// such as Hebrew and Arabic. If Alignment is kHACenter, then the Direction
            /// doesn't matter. 
            void FollowBaseline(const LineLayout* pLineLayout, GlyphLayoutInfoEx* pGlyphLayoutInfoArray,
                                BaselineIterator* pBaselineIterator, float fBaseline, float fIntitialAdvance,
                                HAlignment alignment, Direction direction = kDirectionDefault);

        protected:
            LineLayout*       mpLineLayout;
            BaselineIterator* mpBaselineIterator;
        };


    } // namespace Text

} // namespace EA




#endif // Header include guard













