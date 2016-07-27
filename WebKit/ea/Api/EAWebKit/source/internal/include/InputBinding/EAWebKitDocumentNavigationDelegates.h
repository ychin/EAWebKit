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
// EAWebKitDocumentNavigationDelegates.h
// By Chris Stott
// Maintained by EAWebKit Team
///////////////////////////////////////////////////////////////////////////////

#ifndef EAWEBKIT_EAWEBKITDOCUMENTNAVIGATIONDELEGATES_H
#define EAWEBKIT_EAWEBKITDOCUMENTNAVIGATIONDELEGATES_H

#include <EAWebKit/EAWebKitConfig.h>

namespace WebCore
{
	class Node;
	class Element;
}

namespace EA
{
	
	namespace WebKit
	{
		class View;

		class DelegateBase
		{
		public:
			DelegateBase(EA::WebKit::View* view) : mView(view)
			{
				// empty
			}

		protected:
			bool CanJumpToNode(WebCore::Node* node, bool checkHTMLElementType = true, bool checkSize = true);
			
			const EA::WebKit::View* GetView() const 
			{ 
				return mView; 
			}
			EA::WebKit::View* GetView() 
			{ 
				return mView; 
			}


		private:
			EA::WebKit::View* mView;
		};

		class IsNodeNavigableDelegate : DelegateBase
		{
		public:
			IsNodeNavigableDelegate(EA::WebKit::View* view) : DelegateBase(view), mFoundNode(false) {}

			bool operator()(WebCore::Node* node, bool checkSize = true);
			
			bool FoundNode() const 
			{ 
				return mFoundNode; 
			}

		private:
			bool mFoundNode;
		};

	}
}

#endif //EAWEBKIT_EAWEBKITDOCUMENTNAVIGATIONDELEGATES_H