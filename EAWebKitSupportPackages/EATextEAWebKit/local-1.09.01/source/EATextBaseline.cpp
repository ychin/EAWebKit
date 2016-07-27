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
// EATextBaseline.cpp
//
// Maintained by Paul Pedriana and Talin.
//
///////////////////////////////////////////////////////////////////////////////


#include <EAText/EATextBaseline.h>
#include <string.h>
#include <math.h>
#include EA_ASSERT_HEADER


namespace EA
{

namespace Text
{

namespace Bezier
{
    float GetDistance(const Point& pt0, const Point& pt1)
    {
        const float fLengthX = (pt1.mX - pt0.mX);
        const float fLengthY = (pt1.mY - pt0.mY);

        return sqrtf((fLengthX * fLengthX) + (fLengthY * fLengthY));
    }

    // Returns the cartesian angle formed by the line segment from pt0 to pt1.
    // If pt1 is just to the right of pt0, then the angle is zero. If pt1 is
    // directly above pt0 then the angle is pi/2 radians (90 degrees), assuming
    // you have a y-up coordinate system. 
    inline float GetAngle(const Point& pt0, const Point& pt1)
    {
        return atan2f(pt1.mY - pt0.mY, pt1.mX - pt0.mX);
    }

} // namespace Bezier



///////////////////////////////////////////////////////////////////////////////
// RotatePoint2D
//
// Rotates the given point about the given origin at a given angle in radians.
// The formula for rotating a point (x0,y0) about the origin at a given angle is:
//     x1 = x0*cos(fAngle) - y0*sin(fAngle);
//     y1 = x0*sin(fAngle) + y0*cos(fAngle);
//
// The result point can also be the input point.
//
static void RotatePoint2D(const float ptOrigin[2], const float ptPoint[2], float fAngle, float ptResult[2])
{
    float ptResultTemp[2] =
    {
        ptPoint[0] - ptOrigin[0],  // Make the point be relative to the 0,0 origin.
        ptPoint[1] - ptOrigin[1]
    }; 

    // Rotate in XY plane.
    const float fCos(cosf(fAngle));
    const float fSin(sinf(fAngle));
    const float xTemp(ptResultTemp[0]);
    const float yTemp(ptResultTemp[1]);

    ptResultTemp[0] = (xTemp * fCos) - (yTemp * fSin);
    ptResultTemp[1] = (xTemp * fSin) + (yTemp * fCos);

    // Re-translate to origin point.
    ptResultTemp[0] += ptOrigin[0];
    ptResultTemp[1] += ptOrigin[1];

    // Copy into destination
    ptResult[0] = ptResultTemp[0];
    ptResult[1] = ptResultTemp[1];
}

///////////////////////////////////////////////////////////////////////////
// TransformGlyph
//
// To consider: This function could be directly improved via VMX/SSE instructions.
//
static void TransformGlyph(const GlyphLayoutInfo& gliPrev, GlyphLayoutInfoEx& gliNew, 
                           const Point& ptGlyphCenter, float fGlyphAngle, float fBaseline)
{
    // Algorithm:
    // Translate the glyph to be horizontally centered on ptGlyphCenter.
    // Then rotate the glyph around ptGlyphCenter by fGlyphAngle.

    const float prevCenter[2]  = { (gliPrev.mfX1 + gliPrev.mfX2) / 2, fBaseline };
    const float translation[2] = { ptGlyphCenter.mX - prevCenter[0], ptGlyphCenter.mY - prevCenter[1] };

    // Translate
    gliNew.mfX1 = gliPrev.mfX1 + translation[0];
    gliNew.mfY1 = gliPrev.mfY1 + translation[1];
    gliNew.mfX2 = gliPrev.mfX2 + translation[0];
    gliNew.mfY2 = gliPrev.mfY1 + translation[1];
    gliNew.mfX3 = gliPrev.mfX2 + translation[0];
    gliNew.mfY3 = gliPrev.mfY2 + translation[1];
    gliNew.mfX4 = gliPrev.mfX1 + translation[0];
    gliNew.mfY4 = gliPrev.mfY2 + translation[1];

    // Rotate
    RotatePoint2D(&ptGlyphCenter.mX, &gliNew.mfX1, fGlyphAngle, &gliNew.mfX1);
    RotatePoint2D(&ptGlyphCenter.mX, &gliNew.mfX2, fGlyphAngle, &gliNew.mfX2);
    RotatePoint2D(&ptGlyphCenter.mX, &gliNew.mfX3, fGlyphAngle, &gliNew.mfX3);
    RotatePoint2D(&ptGlyphCenter.mX, &gliNew.mfX4, fGlyphAngle, &gliNew.mfX4);
}


///////////////////////////////////////////////////////////////////////////
// LinesIntersect2D
//
// This function determines if two line segments intersect. 
// Note that we mean line segments and not infinite lines. 
//
static bool LinesIntersect2D(const float ptStart0[2], const float ptEnd0[2],
                             const float ptStart1[2], const float ptEnd1[2])
{
    const float d = (ptEnd0[0]   - ptStart0[0]) * (ptStart1[1] - ptEnd1[1]  ) - 
                    (ptStart1[0] - ptEnd1[0]  ) * (ptEnd0[1]   - ptStart0[1]);

    if(fabsf(d) < 0.01f) // 0.01f is an epsilon value.
        return false; //The lines are parallel.

    const float d0        = (ptStart1[0] - ptStart0[0]) * (ptStart1[1] - ptEnd1[1]  ) - (ptStart1[0]-ptEnd1[0]  ) * (ptStart1[1] - ptStart0[1]);
    const float d1        = (ptEnd0[0]   - ptStart0[0]) * (ptStart1[1] - ptStart0[1]) - (ptStart1[0]-ptStart0[0]) * (ptEnd0[1]   - ptStart0[1]);
    const float kOneOverD = 1 / d;
    const float t0        = d0 * kOneOverD;
    const float t1        = d1 * kOneOverD;

    return ((t0 >= 0.f) && (t0 <= 1.f) && 
            (t1 >= 0.f) && (t1 <= 1.f));
}


///////////////////////////////////////////////////////////////////////////////
// GlyphsOverlap
//
// Tells if two glyphs overlap each other.
// It turns out that we don't need an arbitrary quadrangle/quadrangle intersection
// algorithm, because we happen to know that the two input glyphs will already 
// satisfy some constraints that allow us to greatly simplify the test. 
// In particular, we know that glyph2 is along a baseline right next to glyph1
// and that glyph2 will leaning towards or away from glyph1 at an angle of about
// +/- 20 degrees at most. In the picture below we have an H and M along a baseline
// that is beginning to curve upward, with the M at an angle leaning towards H.
// 
//   *    *      *
//   *    * *    **
//   ******  ** *  *  
//   *    *   *     *
//   *    *    *
// 
// We can do a line-line intersection between the right wide of the H with the left
// side of the M and that should be sufficient for all non-pathological baseline
// curvatures. 
//
static bool GlyphsOverlap(const GlyphLayoutInfoEx& g1, const GlyphLayoutInfoEx& g2)
{
    // The use of x2/x3 x1/x4 here assumes that g2 is to the right of g1. We would 
    // need to reverse this if we want to deal with the case of g2 being to the 
    // left of g1 (right-to-left layout).

    // To do: Implement LinesIntersect2D functionality directly within this function.

    return LinesIntersect2D(&g1.mfX2, &g1.mfX3, &g2.mfX1, &g2.mfX4);
}






///////////////////////////////////////////////////////////////////////////////
// Bezier2D
///////////////////////////////////////////////////////////////////////////////

Bezier2D::Bezier2D()
{
    #ifdef EA_DEBUG
        memset(this, 0, sizeof(Bezier2D));
    #endif
}


void Bezier2D::Set(const Point* pControlPointArray)
{
    memcpy(mControlPoints, pControlPointArray, sizeof(mControlPoints));
    mCurveLength = 0.f;

    float fDistanceMin = 1000000.f;
    float fDistanceMax = 0.f;
    Point pt0;
    Point pt1;

    GetPoint(0.f, pt0);

    // Sample the distance along the curve. Note that we move in increments  
    // that are power-of-two divisions of 1.0. 0.015625 is 1.0/64.0.
    // To do: Make the constants here configurable.

    for(float t = 0.015625f; t < 1.010f; t += 0.015625f, pt0 = pt1)
    {
        GetPoint(t, pt1);

        const float fDistance = Bezier::GetDistance(pt0, pt1);

        mCurveLength += fDistance;

        if(fDistance < fDistanceMin)
            fDistanceMin = fDistance;
        if(fDistance > fDistanceMax)
            fDistanceMax = fDistance;
    }

    // This calculation doesn't work well at all. We really need to find  
    // something that works better. This can't be very hard to do.
    mbLinearMovement = ((fabsf(fDistanceMin - fDistanceMax) / fDistanceMin) < 0.5f);
}


void Bezier2D::GetPoint(float t, Point& pt)
{
    const float tm1  = 1 - t;
    const float tm13 = tm1 * tm1 * tm1;
    const float t3   = t * t * t;
    const float a    = 3 * t * tm1 * tm1;
    const float b    = 3 * t * t * tm1;

    pt.mX = ((tm13 * mControlPoints[0].mX) + (a * mControlPoints[1].mX) + (b * mControlPoints[2].mX) + (t3 * mControlPoints[3].mX));
    pt.mY = ((tm13 * mControlPoints[0].mY) + (a * mControlPoints[1].mY) + (b * mControlPoints[2].mY) + (t3 * mControlPoints[3].mY));
}



///////////////////////////////////////////////////////////////////////////////
// Bezier2DIterator
///////////////////////////////////////////////////////////////////////////////


Bezier2DIterator::Bezier2DIterator()
  : mBezier(),
    mT(0),
    mDistance(0),
    mPoint(),
    mTangentAngle()
{
}


void Bezier2DIterator::SetBezier(const Bezier2D& bezier2D)
{
    mBezier       = bezier2D;
    mT            = 0.f;
    mDistance     = 0.f;
    mPoint        = bezier2D.mControlPoints[0];
    mTangentAngle = 0.f;
}


float Bezier2DIterator::GetT() const
{
    return mT;
}


void Bezier2DIterator::SetT(float t)
{
    mTangentAngle = 999.f;
    mT = t;
}


float Bezier2DIterator::GetDistance() const
{
    return mDistance;
}


void Bezier2DIterator::SetDistance(float fDistance)
{
    float t;

    if(mBezier.mbLinearMovement)
        t = fDistance / mBezier.mCurveLength;
    else
    {
        // This implementation is not as smart as it could be, as it always 
        // starts from the beginning of the curve and along it. A smarter 
        // version would do a more precise measurement of only a small section.
        float fCurrentDistance = 0;
        float fNextDistance;
        Point pt0;
        Point pt1;
        float tNext;

        mBezier.GetPoint(0.f, pt0);

        // To do: This calculation is too slow. It is doing potentially hundreds of 
        // loops through this calculation and it needs to be smarter. You need to 
        // be accurate to within about 1/500 of the curve length to avoid bad positioning.
        for(t = 0, tNext = 0.0015625f; tNext < 1.0010f; t = tNext, tNext += 0.0015625f, pt0 = pt1, fCurrentDistance += fNextDistance)
        {
            mBezier.GetPoint(t, pt1);

            fNextDistance = Bezier::GetDistance(pt0, pt1);

            if((fCurrentDistance + fNextDistance) > fDistance)
            {
                // We take the average of the two, but it would be better if we narrowed down the t value.
                t += (tNext - t) * 0.5f;
                break;
            }
        }
    }

    SetT(t);
}


void Bezier2DIterator::MoveDistance(float fDistance)
{
    SetDistance(mDistance + fDistance);
}


float Bezier2DIterator::GetBaselineLength() const
{
    return mBezier.mCurveLength;
}


void Bezier2DIterator::GetPosition(Point& pt, float& fTangentAngle)
{
    if(mTangentAngle == 999.f) // if the tangent angle is 'dirty' (if we need to recalculate tangent/position)...
    {
        mBezier.GetPoint(mT, mPoint);

        // Need to calculate tangent angle. The only practical way for us to do this is
        // to sample the curve a point near the current mT and get its angle.
        Point ptSample;

        mBezier.GetPoint(mT + 0.02f, ptSample);
        mTangentAngle = Bezier::GetAngle(mPoint, ptSample);
    }

    pt            = mPoint;
    fTangentAngle = mTangentAngle;
}


void Bezier2DIterator::MakeLine(const Point& ptBegin, const Point& ptEnd)
{
    Point pointArray[4];

    // We create a 2D line Bezier, with the inner two control points one-third of 
    // the way between the outer points. This results in linear 't' movement.
    pointArray[0] = ptBegin;
    pointArray[3] = ptEnd;

    pointArray[1].mX = ptBegin.mX + (0.333333f * (ptEnd.mX - ptBegin.mX));
    pointArray[1].mY = ptBegin.mY + (0.333333f * (ptEnd.mY - ptBegin.mY));

    pointArray[2].mX = ptBegin.mX + (0.666666f * (ptEnd.mX - ptBegin.mX));
    pointArray[2].mY = ptBegin.mY + (0.666666f * (ptEnd.mY - ptBegin.mY));

    mBezier.Set(pointArray);
    mBezier.mbLinearMovement = true;
}


void Bezier2DIterator::MakeSemicircle(const Point& ptCenter, float fRadius, float fAngle, bool bClockwise)
{
    // For a brief discussion on drawing circles with Bezier curves, see http://www.whizkidtech.redprince.net/mBezier/circle/.
    // If you want to know how to make other shapes with Bezier curves, see http://www.tinaja.com/cubic01.asp.
    // You can't draw a circle with a four-control-point Bezier, but you can draw a quarter-circle very closely and 
    // a semi-circle fairly closely. For anything more than a semi-circle, you really need to use a Bezier with more
    // than four control points.

    Point pointArray[4];

    const float fXOffset = bClockwise ? fRadius : -fRadius;

    pointArray[0].mX = ptCenter.mX - fXOffset;
    pointArray[0].mY = ptCenter.mY;

    pointArray[3].mX = ptCenter.mX + fXOffset;
    pointArray[3].mY = ptCenter.mY;

    pointArray[1].mX = pointArray[0].mX;
    pointArray[1].mY = ptCenter.mY + (fRadius * 1.333333f);

    pointArray[2].mX = pointArray[3].mX;
    pointArray[2].mY = pointArray[1].mY;

    if(fAngle != 0.f)
    {
        RotatePoint2D(&ptCenter.mX, &pointArray[0].mX, fAngle, &pointArray[0].mX);
        RotatePoint2D(&ptCenter.mX, &pointArray[1].mX, fAngle, &pointArray[1].mX);
        RotatePoint2D(&ptCenter.mX, &pointArray[2].mX, fAngle, &pointArray[2].mX);
        RotatePoint2D(&ptCenter.mX, &pointArray[3].mX, fAngle, &pointArray[3].mX);
    }

    mBezier.Set(pointArray);
    mBezier.mbLinearMovement = false;
}





///////////////////////////////////////////////////////////////////////////////
// BaselineLayout
///////////////////////////////////////////////////////////////////////////////


BaselineLayout::BaselineLayout()
  : mpLineLayout(NULL),
    mpBaselineIterator(NULL)
{
}


///////////////////////////////////////////////////////////////////////////////
// FollowBaseline
//
// If the current glyph overlaps with the previous glyph, then we try moving the 
// current glyph a little farther along the curve. But after a certain distance 
// we simply give up and assume the curve is too sharp to prevent overlap.
//
void BaselineLayout::FollowBaseline(const LineLayout* pLineLayout, GlyphLayoutInfoEx* pGlyphLayoutInfoArray,
                                    BaselineIterator* pBaselineIterator, float fBaseline, float fIntitialAdvance,
                                    HAlignment alignment, Direction direction)
{
    const float fBaselineLength   = pBaselineIterator->GetBaselineLength();
    float       fBaselineDistance = 0.f;

    if(EA_UNLIKELY(direction == kDirectionRTL))
    {
        if(alignment == kHALeft)    
            alignment = kHARight;
        else if(alignment == kHARight)
            alignment = kHALeft;
    }

    switch(alignment)
    {
        case kHARight:
            fBaselineDistance = (fBaselineLength - pLineLayout->mLineMetrics.mfSpace);
            break;

        case kHACenter: // We have a problem in that we are centering the text here but a curvy baseline might cause us to have to push the characters rightward and make them non-centered.
            fBaselineDistance = (fBaselineLength - pLineLayout->mLineMetrics.mfSpace) / 2;
            break;
    
        case kHALeft:
        case kHAJustify:
        case kHAInherit:
        default:
            break;
    }

    fBaselineDistance += fIntitialAdvance;

    // Place each glyph.
    for(eastl_size_t i = 0, iEnd = pLineLayout->mGlyphLayoutInfoArray.size(); i < iEnd; i++)
    {
        const GlyphLayoutInfo& gli    = pLineLayout->mGlyphLayoutInfoArray[i];
        GlyphLayoutInfoEx&     gliNew = pGlyphLayoutInfoArray[i];
        Point ptGlyphCenter;
        float fGlyphAngle;

        gliNew.mpFont = gli.mpFont;

        const float fGlyphWidth  = gli.mfX2 - gli.mfX1;
        const float fGlyphCenter = gli.mfX1 + (fGlyphWidth / 2) - gli.mfPenX;
        const float fGlyphHeight = fabsf(gli.mfY1 - gli.mfPenY);
        const float fMaxDistance = fBaselineDistance + (fGlyphHeight * 2) + 1.f; // Add +1 in case fGlyphHeight == 0.

        // Try to place the current glyph, possibly re-trying if there is overlap.
        do {
            pBaselineIterator->SetDistance(fBaselineDistance + fGlyphCenter);
            pBaselineIterator->GetPosition(ptGlyphCenter, fGlyphAngle);
            TransformGlyph(gli, gliNew, ptGlyphCenter, fGlyphAngle, fBaseline);

            if((i == 0) || !GlyphsOverlap(pGlyphLayoutInfoArray[i - 1], gliNew))
                break;

            fBaselineDistance += (fGlyphHeight / 32);  // This constant here is somewhat arbitrary. It should be a value between ~4 and 128.
        }
        while(fBaselineDistance < fMaxDistance);

      //gliNew.mfPenX    = fBaselineDistance - fIntitialAdvance;  // mfPenX is a zero-based value along the line from the start of the text. We probably want to change this.
        gliNew.mfAdvance = gli.mfAdvance;

        fBaselineDistance += gli.mfAdvance;
    }
}



} // namespace Text

} // namespace EA















