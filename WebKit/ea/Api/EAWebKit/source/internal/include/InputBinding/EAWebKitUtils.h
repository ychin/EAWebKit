/*
Copyright (C) 2009, 2010, 2011, 2014 Electronic Arts, Inc.  All rights reserved.

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
// EAWebKitUtils.h
// By Chris Stott
// Maintained by EAWebKit Team
///////////////////////////////////////////////////////////////////////////////

#ifndef EAWEBKIT_EAWEBKITUTILS_H
#define EAWEBKIT_EAWEBKITUTILS_H

#include <wtf/MathExtras.h>
#include "LayoutPoint.h"
#include <EAWebKit/EAWebKitConfig.h>


namespace EA
{
	namespace WebKit
	{
		//////////////////////////////////////////////////////////////////////////
		//
		inline float CalcAngle(float x, float y)
		{
			float a = atan2(y, x);

			if (a < 0.0f)
			{
				a += 2*3.14159f;
			}

			return a;
		}

		//////////////////////////////////////////////////////////////////////////
		//
		template <typename T>
		T Min(T a, T b)
		{
			return a>b ? b : a;
		}

		//////////////////////////////////////////////////////////////////////////
		//
		template <typename T>
		T Min(T a, T b, T c, T d)
		{
			return Min(Min(a,b),Min(c,d));
		}

		//////////////////////////////////////////////////////////////////////////
		//
		template <typename T>
		T Max(T a, T b)
		{
			return a>b ? a : b;
		}

		//////////////////////////////////////////////////////////////////////////
		//
		template <typename T>
		T Max(T a, T b, T c, T d)
		{
			return Max(Max(a,b),Max(c,d));
		}

		//////////////////////////////////////////////////////////////////////////
		//
		template <typename T>
		T Clamp(T min, T value, T max)
		{
			return Min(Max(min,value),max);
		}

		//////////////////////////////////////////////////////////////////////////
		//
		template <typename T>
		void Swap(T& a, T& b)
		{
			T temp = a;
			a = b;
			b = temp;
		}

		//////////////////////////////////////////////////////////////////////////
		//
		inline WebCore::LayoutPoint Add(WebCore::LayoutPoint a, WebCore::LayoutPoint b)
		{
			WebCore::LayoutPoint c;
			c.setX(a.x() + b.x());
			c.setY(a.y() + b.y());
			return c;
		}

		//////////////////////////////////////////////////////////////////////////
		//
		inline WebCore::LayoutPoint Subtract(WebCore::LayoutPoint a, WebCore::LayoutPoint b)
		{
			WebCore::LayoutPoint c;
			c.setX(b.x() - a.x());
			c.setY(b.y() - a.y());
			return c;
		}

		//////////////////////////////////////////////////////////////////////////
		//
		inline WebCore::LayoutPoint Divide(WebCore::LayoutPoint a, int f)
		{
			WebCore::LayoutPoint c;
			c.setX(a.x()/f);
			c.setY(a.y()/f);
			return c;
		}

		//////////////////////////////////////////////////////////////////////////
		//
		inline WebCore::LayoutPoint Multiply(WebCore::LayoutPoint a, int f)
		{
			WebCore::LayoutPoint c;
			c.setX(a.x()*f);
			c.setY(a.y()*f);
			return c;
		}

		//////////////////////////////////////////////////////////////////////////
		//
		inline WebCore::LayoutPoint Average(WebCore::LayoutPoint a, WebCore::LayoutPoint b)
		{
			return Divide(Add(a,b),2);
		}

		//////////////////////////////////////////////////////////////////////////
		//
		inline WebCore::LayoutPoint Interpolate(WebCore::LayoutPoint a, WebCore::LayoutPoint b, int n, int total)
		{
			return Divide(Add(Multiply(a,total-n),Multiply(b,n)),total);
		}

		//////////////////////////////////////////////////////////////////////////
		//
		inline int DotProduct(WebCore::LayoutPoint a, WebCore::LayoutPoint b)
		{
			return a.x()*b.x() + a.y()*b.y();
		}
	} // namespace WebKit
} // namespace EA

#endif //EAWEBKIT_EAWEBKITUTILS_H