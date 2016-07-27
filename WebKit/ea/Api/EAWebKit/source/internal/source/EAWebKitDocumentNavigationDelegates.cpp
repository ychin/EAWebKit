/*
Copyright (C) 2009, 2010, 2011, 2012, 2014 Electronic Arts, Inc.  All rights reserved.

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
// EAWebKitDocumentNavigationDelegates.cpp
// By Chris Stott
// Maintained by EAWebKit Team
///////////////////////////////////////////////////////////////////////////////

#include "config.h"
#include <internal/include/InputBinding/EAWebKitDocumentNavigationDelegates.h>
#include <EAWebKit/EAWebKitView.h>

#include "Node.h"
#include "HTMLElement.h"
#include "HTMLNames.h"
#include "IntRect.h"
#include "RenderStyle.h"
#include <wtf/Assertions.h>

namespace EA
{
	namespace WebKit
	{
		bool DelegateBase::CanJumpToNode(WebCore::Node* node, bool checkHTMLElementType, bool checkSize)
		{
			if ((node->nodeType() == WebCore::Node::ELEMENT_NODE) && node->isHTMLElement())
			{
				WebCore::HTMLElement* htmlElement = (WebCore::HTMLElement*)node;
				if(checkHTMLElementType)
				{
					// To consider: Allow following to be included in the navigation check. Consider exposing it in the
					// JumpParams. 
					// bool htmlElementClickable = htmlElement->getAttributeEventListener(WebCore::eventNames().clickEvent); 
					//Following overhead is negligible
					if(!(htmlElement->hasTagName(WebCore::HTMLNames::aTag) || htmlElement->hasTagName(WebCore::HTMLNames::inputTag) || htmlElement->hasTagName(WebCore::HTMLNames::textareaTag)
						|| htmlElement->hasTagName(WebCore::HTMLNames::buttonTag) || htmlElement->hasTagName(WebCore::HTMLNames::selectTag)
						|| (htmlElement->hasClass() && htmlElement->classNames().contains("navigation_include"))
						/* ||htmlElementClickable*/
						))
					{
						return false;
					}
				}

				// We don't check against size unless specified. It is pretty expensive to call getRect and caller may already be doing that.
				if(checkSize)
				{
					WebCore::LayoutRect rect = htmlElement->boundingBox();					
					bool bigEnough = (rect.width() >= 1 && rect.height() >= 1);// Avoid 0 size elements 
					if(!bigEnough)
					{
						return false;
					}
				}
				
				if (htmlElement->computedStyle()->visibility()==WebCore::VISIBLE && htmlElement->computedStyle()->display()!=WebCore::NONE)
				{
					// If the element does not have class or if it does, it is not supposed to be ignored.
					if (!htmlElement->hasClass() || !htmlElement->classNames().contains("navigation_ignore")) 
					{	
						return true;						
					}
				}
			}

			return false;
		}

		
		//
		// IsNodeNavigableDelegate
		//
		bool IsNodeNavigableDelegate::operator ()(WebCore::Node* node, bool checkSize)
		{
			bool canJump = CanJumpToNode(node, true, checkSize);
			if (canJump)
			{
				mFoundNode = true;
			}

			// by returning false we'll taking the domwalker that we've found the node we want, and to stop.
			return !canJump;
		}

	}
}
