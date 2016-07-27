/*
Copyright (C) 2008, 2009, 2010, 2011, 2012 Electronic Arts, Inc.  All rights reserved.

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

#ifndef EAWEBKIT_EAWEBKITVIEWNAVIGATIONDELEGATE_H
#define EAWEBKIT_EAWEBKITVIEWNAVIGATIONDELEGATE_H

namespace WebCore
{
class Frame;
class Node;
}

#include <EAWebKit/EAWebKitSystem.h>
#include <EAWebKit/EAWebKitView.h>


namespace EA
{
namespace WebKit
{
class ViewNavigationDelegate
{
public:
	ViewNavigationDelegate(EA::WebKit::View* pView)
		: mView(pView)
		, mBestNodeFrame(0)
		, mBestNodeX(0)
		, mBestNodeY(0)
		, mBestNodeWidth(0)
		, mBestNodeHeight(0)
		, mCentreX(0)
		, mCentreY(0)
		, mCachedNavigationUpId()
		, mCachedNavigationDownId()
		, mCachedNavigationLeftId()
		, mCachedNavigationRightId()
	{

	}

	bool JumpToNearestElement(EA::WebKit::JumpDirection direction);
	bool Click();
	void MoveMouseCursorToNode(WebCore::Node* node, bool scrollIfNecessary = true);

	bool JumpToNearestElement(EA::WebKit::JumpDirection direction, bool scrollIfElementNotFound);
	void ScrollOnJump(bool vertical, int zDelta, float numLines);

private:
	EA::WebKit::View*					mView;
	WebCore::Frame*						mBestNodeFrame;//Frame where the last best node was found.
	int									mBestNodeX;
	int									mBestNodeY;
	int									mBestNodeWidth;
	int									mBestNodeHeight;
	int									mCentreX;
	int									mCentreY;
	EASTLFixedString8Wrapper			mCachedNavigationUpId;
	EASTLFixedString8Wrapper			mCachedNavigationDownId;
	EASTLFixedString8Wrapper			mCachedNavigationLeftId;
	EASTLFixedString8Wrapper			mCachedNavigationRightId;
};

}
}

#endif //EAWEBKIT_EAWEBKITVIEWNAVIGATIONDELEGATE_H