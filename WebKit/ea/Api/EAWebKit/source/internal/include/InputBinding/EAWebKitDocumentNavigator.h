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
// EAWebKitDocumentNavigator.h
// By Chris Stott
// Maintained by EAWebKit Team
///////////////////////////////////////////////////////////////////////////////

#ifndef EAWEBKIT_EAWEBKITDOCUMENTNAVIGATOR_H
#define EAWEBKIT_EAWEBKITDOCUMENTNAVIGATOR_H

#include <platform/graphics/LayoutPoint.h>
#include <platform/graphics/IntRect.h>
#include <EAWebKit/EAWebKitView.h>
#include <EAWebKit/EAWebkitSTLWrapper.h>
#include <EAWebKit/EAWebKitConfig.h>
#include <internal/include/EAWebKitAssert.h>


namespace WebCore 
{ 
	class Node; 
	class Document;
}

namespace EA
{
	namespace WebKit
	{
		class NodeListContainer;
		class View;

		class DocumentNavigator
		{
		public:
			DocumentNavigator(EA::WebKit::View* view, WebCore::Document* document, EA::WebKit::JumpDirection direction, WebCore::LayoutPoint startingPosition, int previousNodeX, int previousNodeY, int previousNodeWidth, int previousNodeHeight, float theta, bool strictAxesCheck, float maxRadialDistance);
			~DocumentNavigator();
			void FindBestNode(WebCore::Node* rootNode);

			WebCore::Node* GetBestNode() const 
			{ 
				EAW_ASSERT( (mBestNode == 0) || (*(uint32_t*)mBestNode > 10000000u) );
				return mBestNode; 
			}

			float GetBestNodeRadialDistance() const
			{
				return mMinR;
			}

		private:
			EA::WebKit::View* mView;
			EA::WebKit::JumpDirection mDirection;
			WebCore::LayoutPoint mStartingPosition;
			WebCore::IntRect mPreviousNodeRect;

			WebCore::Node* mBestNode;
			WebCore::Document* mDocument;

			float mMinR;
			float mMinThetaRange;
			float mMaxThetaRange;
			bool mStrictAxesCheck;

			bool doAxisCheck(WebCore::LayoutRect rect);
			bool areAnglesInRange(float minTheta, float maxTheta);

		public:
			NodeListContainer*					mNodeListContainer;

			int mAxesX;
			int mAxesY;

		};
	} // namespace WebKit
} // namespace EA

#endif //EAWEBKIT_EAWEBKITDOCUMENTNAVIGATOR_H