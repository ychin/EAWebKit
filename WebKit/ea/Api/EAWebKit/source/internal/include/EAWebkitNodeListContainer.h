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
// EAWebKitNodeListContainer.h
// By Arpit Baldeva 
// Maintained by EAWebKit Team
///////////////////////////////////////////////////////////////////////////////


#ifndef EAWEBKIT_EAWEBKITNODELISTCONTAINER_H
#define EAWEBKIT_EAWEBKITNODELISTCONTAINER_H


///////////////////////////////////////////////////////////////////////
// The purpose of this file is to contain the nodelist so that it does not expand in the main app.
// This is required for LGPL compliance.
///////////////////////////////////////////////////////////////////////

#include <EABase/eabase.h>
#include <EAWebKit/EAWebKitView.h>
#include <EASTL/list.h>
#include <EAWebKit/EAWebkitAllocator.h> //For EASTLAllocator

namespace WebCore
{
	class Node;
}

namespace EA
{
    namespace WebKit
    {
		struct FoundNodeInfo
		{
			WebCore::Node*	mFoundNode;
			float			mRadialDistance; //Distance from the search point
		};

		typedef eastl::list<WebCore::Node*,EASTLAllocator> WebCoreNodeList;
		typedef eastl::list<WebCore::Node*,EASTLAllocator>::iterator WebCoreNodeListIterator;
		typedef eastl::list<WebCore::Node*,EASTLAllocator>::reverse_iterator WebCoreNodeListReverseIterator;

		typedef eastl::list<FoundNodeInfo,EASTLAllocator> WebCoreFoundNodeInfoList;
		typedef eastl::list<FoundNodeInfo,EASTLAllocator>::iterator WebCoreFoundNodeInfoListIterator;
		typedef eastl::list<FoundNodeInfo,EASTLAllocator>::reverse_iterator WebCoreFoundNodeInfoListReverseIterator;


		class NodeListContainer
		{
			friend class View;
			friend class DocumentNavigator;
		private:
			WebCoreFoundNodeInfoList	mFoundNodes;
			WebCoreNodeList				mRejectedByHitTestNodes;
			WebCoreNodeList				mRejectedByAngleNodes;
			WebCoreNodeList				mRejectedByRadiusNodes;
			WebCoreNodeList 			mRejectedWouldBeTrappedNodes;
		};
    }
}

#endif // Header include guard
