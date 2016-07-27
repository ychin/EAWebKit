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
// EAWebKitDocumentNavigator.cpp
// By Chris Stott
// Maintained by EAWebKit Team
///////////////////////////////////////////////////////////////////////////////

//Not fixing any errors in this file as we plan to remove the functionality


#include "config.h"
#include <internal/include/InputBinding/EAWebKitDocumentNavigator.h>
#include <internal/include/InputBinding/EAWebKitDocumentNavigationDelegates.h>
#include <internal/include/InputBinding/EAWebKitPolarRegion.h>
#include <internal/include/InputBinding/EAWebKitUtils.h>

#include <internal/include/EAWebKitAssert.h>
#include <internal/include/EAWebKitEASTLHelpers.h>
#include <internal/include/EAWebkitNodeListContainer.h>
#include <internal/include/EAWebKitNewDelete.h>

#include <NodeList.h>
#include <EventNames.h>
#include <HTMLInputElement.h>
#include <platform/graphics/LayoutRect.h>

#include "RenderStyle.h"
#include "Document.h"
#include "FrameView.h"
#include "NodeTraversal.h"

#include <float.h>
#include "Frame.h"

namespace EA
{
	namespace WebKit
	{
		DocumentNavigator::DocumentNavigator(EA::WebKit::View* view, WebCore::Document* document, EA::WebKit::JumpDirection direction, 
			WebCore::LayoutPoint startingPosition, int previousNodeX, int previousNodeY, int previousNodeWidth, int previousNodeHeight, float theta, 
			bool strictAxesCheck, float maxRadialDistance)
			:	mView(view)
			,	mDirection(direction)
			,	mStartingPosition(startingPosition)
			,	mPreviousNodeRect(previousNodeX,previousNodeY,previousNodeWidth,previousNodeHeight)
			,	mBestNode(0)
			,	mDocument(document)
			,	mMinR(maxRadialDistance)
			,   mStrictAxesCheck(strictAxesCheck)
			,	mNodeListContainer(0)
		{

			mNodeListContainer = new NodeListContainer();//WTF::fastNew<NodeListContainer> ();

			int width = previousNodeWidth;
			int height = previousNodeHeight;

			switch (direction)
			{
			case EA::WebKit::JumpRight:
				mStartingPosition.setX(mStartingPosition.x() + 3*width/8);
				break;

			case EA::WebKit::JumpDown:
				mStartingPosition.setY(mStartingPosition.y() + 3*height/8);
				break;

			case EA::WebKit::JumpLeft:
				mStartingPosition.setX(mStartingPosition.x() - 3*width/8);
				break;

			case EA::WebKit::JumpUp:
				mStartingPosition.setY(mStartingPosition.y() - 3*height/8);
				break;

			default:
				EAW_FAIL_MSG("Should not have got here\n");
			}

			mAxesX = mStartingPosition.x();
			mAxesY = mStartingPosition.y();

			const float PI_4 = 3.14159f / 4.0f;

			switch (direction)
			{
			case EA::WebKit::JumpRight:
				mMinThetaRange = 8*PI_4 - theta;
				mMaxThetaRange = theta;



				//printf("JumpingRight : theta=[%f,%f]\n", mMinThetaRange, mMaxThetaRange);

				break;

			case EA::WebKit::JumpDown:
				mMinThetaRange = 2*PI_4-theta;
				mMaxThetaRange = 2*PI_4+theta;



				//printf("JumpingDown : theta=[%f,%f]\n", mMinThetaRange, mMaxThetaRange);
				break;

			case EA::WebKit::JumpLeft:
				mMinThetaRange = 4*PI_4 - theta;
				mMaxThetaRange = 4*PI_4 + theta;


				//printf("JumpingLeft : theta=[%f,%f]\n", mMinThetaRange, mMaxThetaRange);
				break;

			case EA::WebKit::JumpUp:
				mMinThetaRange = 6*PI_4 - theta;
				mMaxThetaRange = 6*PI_4 + theta;


				//printf("JumpingUp : theta=[%f,%f]\n", mMinThetaRange, mMaxThetaRange);
				break;

			default:
				EAW_FAIL_MSG("Should not have got here\n");
			}


			//printf("atan2(1.0f,0.0f)=%f [+XAXIS]\n",		CalcAngle(1.0f,0.0f) );
			//printf("atan2(0.0f,1.0f)=%f [+YAXIS]\n",		CalcAngle(0.0f,1.0f) );
			//printf("atan2(-1.0f,0.0f)=%f [-XAXIS]\n",		CalcAngle(-1.0f,0.0f) );
			//printf("atan2(0.0f,-1.0f)=%f [-YAXIS]\n",		CalcAngle(0.0f,-1.0f) );

			//printf("atan2(1.0f,1.0f)=%f [DIAGONAL1]\n",	CalcAngle(1.0f,1.0f) );
			//printf("atan2(-1.0f,1.0f)=%f [DIAGONAL2]\n",	CalcAngle(-1.0f,1.0f) );
			//printf("atan2(-1.0f,-1.0f)=%f [DIAGONAL3]\n", CalcAngle(-1.0f,-1.0f) );
			//printf("atan2(1.0f,-1.0f)=%f [DIAGONAL4]\n",	CalcAngle(1.0f,-1.0f) );
		}

		DocumentNavigator::~DocumentNavigator()
		{
			if(mNodeListContainer)
			{
				delete mNodeListContainer;//WTF::fastDelete<NodeListContainer>(mNodeListContainer);
				mNodeListContainer = 0;
			}
		}
		static bool WouldBeTrappedInElement(const WebCore::LayoutRect& rect, const WebCore::LayoutPoint& point, EA::WebKit::JumpDirection direction)
		{
			// If we're not inside, don't worry
			if (rect.contains(point))
			{
				typedef WebCore::LayoutPoint Vector2D;

				const WebCore::LayoutPoint centre = Average( rect.minXMaxYCorner(), rect.maxXMinYCorner() );

				Vector2D pointToCentre = Subtract(centre,point);
				Vector2D forward;

				switch (direction)
				{
				// note these are 'backward
				case EA::WebKit::JumpUp:		forward = Vector2D(0,-100); break;
				case EA::WebKit::JumpDown:		forward = Vector2D(0,100); break;
				case EA::WebKit::JumpLeft:		forward = Vector2D(-100,0); break;
				case EA::WebKit::JumpRight:		forward = Vector2D(100,0); break;
				}

				// Basically, if the centre is behind us, don't jump there
				if (DotProduct(forward,pointToCentre) < 0)
				{
					return false;
				}
				else
				{
					/*printf("(%d,%d) Trapped Inside Element (%d,%d)->(%d,%d): forward=(%d,%d) pointToCentre=(%d,%d) dot=%d\n",
						point.x(),
						point.y(),
						rect.bottomLeft().x(),
						rect.bottomLeft().y(),
						rect.topRight().x(),
						rect.topRight().y(),
						forward.x(),
						forward.y(),
						pointToCentre.x(),
						pointToCentre.y(),
						DotProduct(forward,pointToCentre)
						);*/
					return true;
				}
			}
			else
			{
				return false;
			}
		}

		//////////////////////////////////////////////////////////////////////////
		//
		static bool TryingToDoPerpendicularJump(const WebCore::LayoutRect& rect, const WebCore::LayoutRect& previousRect, EA::WebKit::JumpDirection direction)
		{
			if (direction == EA::WebKit::JumpLeft || direction == EA::WebKit::JumpRight)
			{
				if (rect.maxXMinYCorner().x() <= previousRect.maxXMinYCorner().x() && rect.minXMinYCorner().x() >= previousRect.minXMinYCorner().x())
				{
					return true;
				}
				else
				{
					return false;
				}
			}
			else
			{
				if (rect.maxXMinYCorner().y() <= previousRect.maxXMinYCorner().y() && rect.maxXMaxYCorner().y() >= previousRect.maxXMaxYCorner().y())
				{
					return true;
				}
				else
				{
					return false;
				}
			}
		}

		//////////////////////////////////////////////////////////////////////////
		//
		void DocumentNavigator::FindBestNode(WebCore::Node* rootNode)
		{
			// Note by Arpit Baldeva - Changed the recursive algorithm to an iterative algorithm. This results in 25% to 40% increase in efficiency.
			
			while (rootNode) 
			{
				IsNodeNavigableDelegate nodeNavigableDelegate(mView);
				// As it turns out, getRect on HTMLElement is pretty expensive. So we don't do it inside the delegate as we require getRect here too. We do that check here.
				// It is at least ~15% more efficient and can be up to ~25% more efficient (depends on the page layout and current node you are at).
				nodeNavigableDelegate(rootNode,false);

				if (nodeNavigableDelegate.FoundNode())
				{
					WebCore::HTMLElement* htmlElement = (WebCore::HTMLElement*) rootNode;
					WebCore::LayoutRect rectAbsolute = htmlElement->boundingBox();		
					// Adjust the rectangle position based on the frame offset so that we have absolute geometrical position.
					WebCore::FrameView* pFrameView = htmlElement->document().view(); //Can be NULL
					if(pFrameView)
					{
						rectAbsolute.setX(rectAbsolute.x() + pFrameView->x());
						rectAbsolute.setY(rectAbsolute.y() + pFrameView->y());
					}

					 /* printf("Looking at ELEMENT_NODE : nodeName=%S (%d,%d)->(%d,%d) ThetaRange(%f,%f)\n\n%S\n-----------------------------------\n", 
											htmlElement->tagName().charactersWithNullTermination(),
											rect.topLeft().x(),rect.topLeft().y(),
											rect.bottomRight().x(), rect.bottomRight().y(),
											mMinThetaRange,mMaxThetaRange,
											htmlElement->innerHTML().charactersWithNullTermination()
											);
										*/

					if (!WouldBeTrappedInElement(rectAbsolute,mStartingPosition,mDirection))
					{
						if (!TryingToDoPerpendicularJump(rectAbsolute,mPreviousNodeRect,mDirection))
						{
							if(rectAbsolute.width()>=1 && rectAbsolute.height() >= 1) //Avoid 0 size elements
							{
								if (doAxisCheck(rectAbsolute))
								{
									PolarRegion pr(rectAbsolute, mStartingPosition);

									if (pr.minR < mMinR )
									{
										if (areAnglesInRange(pr.minTheta,pr.maxTheta))
										{
											mMinR = pr.minR;

											EAW_ASSERT( *(uint32_t*)rootNode > 10000000u );

											//mBestNode = rootNode; //We don't assign it here since we do the Z-layer testing later on.
											FoundNodeInfo foundNodeInfo = {rootNode, mMinR};
											mNodeListContainer->mFoundNodes.push_back(foundNodeInfo);
											/*printf("Found ELEMENT_NODE : nodeName=%s (%d,%d)->(%d,%d) polar: R(%f,%f) Theta(%f,%f) ThetaRange(%f,%f)  \n", 
											(char*)htmlElement->nodeName().characters(),
											rect.topLeft().x(),rect.topLeft().y(),
											rect.bottomRight().x(), rect.bottomRight().y(),
											pr.minR,pr.maxR,pr.minTheta,pr.maxTheta,
											mMinThetaRange,mMaxThetaRange
											);*/
											
										} 
										else
										{
											
#if EAWEBKIT_ENABLE_JUMP_NAVIGATION_DEBUGGING
											mNodeListContainer->mRejectedByAngleNodes.push_back(rootNode);
#endif
											/*printf("RejectedA ELEMENT_NODE : nodeName=%s (%d,%d)->(%d,%d) polar: R(%f,%f) Theta(%f,%f) ThetaRange(%f,%f)  \n", 
											(char*)htmlElement->nodeName().characters(),
											rect.topLeft().x(),rect.topLeft().y(),
											rect.bottomRight().x(), rect.bottomRight().y(),
											pr.minR,pr.maxR,pr.minTheta,pr.maxTheta,
											mMinThetaRange,mMaxThetaRange
											);*/
										}
									} 
									else
									{
#if EAWEBKIT_ENABLE_JUMP_NAVIGATION_DEBUGGING
										mNodeListContainer->mRejectedByRadiusNodes.push_back(rootNode);
#endif
										/*printf("RejectedR ELEMENT_NODE : nodeName=%s (%d,%d)->(%d,%d) polar: R(%f,%f) Theta(%f,%f) ThetaRange(%f,%f)  \n", 
										(char*)htmlElement->nodeName().characters(),
										rect.topLeft().x(),rect.topLeft().y(),
										rect.bottomRight().x(), rect.bottomRight().y(),
										pr.minR,pr.maxR,pr.minTheta,pr.maxTheta,
										mMinThetaRange,mMaxThetaRange
										);*/
									}
								}
								else
								{
									//printf(" - failed axis check\n");
								}
							}
							else
							{
								//printf(" - too small\n");
							}
						}
						else 
						{
							//printf(" - perpendicular\n");
						}							
					}
					else
					{
#if EAWEBKIT_ENABLE_JUMP_NAVIGATION_DEBUGGING
						mNodeListContainer->mRejectedWouldBeTrappedNodes.push_back(rootNode);
#endif
					}
				}
				
				rootNode = WebCore::NodeTraversal::next(rootNode);
			}

			// Make sure that this element can be jumped to by passing z-check. This makes sure that we jump only on the element
			// at the top most layer (For example, a CSS+JavaScript pop up).
			// We don't try and check against Z-layer in the loop above as it has significant performance penalty. On an average, it causes traversal to be 50% slower. So what we do 
			// instead is to collect all the nodes and at the end, traverse this list from the end to begining. It is important to traverse from end as that is where the most suited element is 
			// based on the position.

			WebCore::Node* bestNode = NULL;
			float radialDistance = FLT_MAX; // A high value so that the max distance between any two elements in the surface is under it.
			bool matched = false;
			for (WebCoreFoundNodeInfoListReverseIterator rIt = mNodeListContainer->mFoundNodes.rbegin(); rIt != mNodeListContainer->mFoundNodes.rend(); ++rIt)
			{
				bestNode = (*rIt).mFoundNode;
				radialDistance = (*rIt).mRadialDistance;
				WebCore::HTMLElement* element = (WebCore::HTMLElement*)bestNode;

				WebCore::Frame*		frame = element->document().frame();
				WebCore::FrameView* pFrameView = element->document().view(); 

				WebCore::LayoutRect rect = element->boundingBox(); //This list is decently small so we don't worry about caching the rect size.
				// ElementFromPoint expects the point in its own coordinate system so we don't need to adjust the rectangle to its absolute position
				// on screen
				// elementFromPoint API changed compared to 1.x. The simplest thing to do at the moment is to adjust our input.
				int inputX = (rect.x()+rect.width()/2 - pFrameView->scrollX())/frame->pageZoomFactor();
				int inputY = (rect.y()+rect.height()/2 - pFrameView->scrollY())/frame->pageZoomFactor();
				
				WebCore::Node* hitElement = mDocument->elementFromPoint(inputX,inputY);
				while (hitElement)
				{
					if(bestNode == hitElement)
					{
						matched = true;
						break;
					}
					hitElement = hitElement->parentNode();//We need to find the element that responds to the events as that is what we jump to. For example, we don't jump to a "span".
				};

				if(matched)
					break;
			}

			if(matched)
			{
				mBestNode = bestNode;
				mMinR = radialDistance;
			}
			else
			{
				mBestNode = 0; //We didn't match anything based on the Z-layer testing.
				mMinR = FLT_MAX;
			}


			// A way to do Post Order traversal.
			//while (WebCore::Node* firstChild = rootNode->firstChild())
			//	rootNode = firstChild;
			//while(rootNode)
			//{
			//	rootNode = rootNode->traverseNextNodePostOrder();
			//}


/*
			//////////////////////////////////////////////////////////////////////////
			// THEN, FIND THE CHILDREN
			if (rootNode && rootNode->childNodeCount() > 0)
			{
				PassRefPtr<WebCore::NodeList> children = rootNode->childNodes();

				const uint32_t length = children->length();

				for (uint32_t i=0; i < length; ++i)
				{
					WebCore::Node* child = children->item(i);
					if (child)
					{
						FindBestNode(child);
					}
				}
			}
*/		
		}

		bool DocumentNavigator::doAxisCheck(WebCore::LayoutRect rect)
		{
			
			int left = rect.x();
			int right = rect.x() + rect.width();
			int top = rect.y();
			int bottom = rect.y() + rect.height();

			//abaldeva: Feel like only one check is enough?
			//Fixes the navigation to the yahoo.com sports section link
			switch (mDirection)
			{
			case EA::WebKit::JumpRight:
				
				if(mStrictAxesCheck)
					return left >= mStartingPosition.x();
				else
					return !(left<mStartingPosition.x() && right<mStartingPosition.x());

				break;

			case EA::WebKit::JumpDown:
				if(mStrictAxesCheck)
					return top >= mStartingPosition.y();
				else
					return !(top<mStartingPosition.y() && bottom<mStartingPosition.y());
				
				break;

			case EA::WebKit::JumpLeft:
				if(mStrictAxesCheck)
					return right <= mStartingPosition.x();
				else
					return !(left>mStartingPosition.x() && right>mStartingPosition.x());
				
				break;

			case EA::WebKit::JumpUp:
				if(mStrictAxesCheck)
					return bottom <= mStartingPosition.y();
				else
					return !(top>mStartingPosition.y() && bottom>mStartingPosition.y());
				
				break;

			default:
				EAW_FAIL_MSG("Should not have got here\n");
			}

			return false;
		}

		bool DocumentNavigator::areAnglesInRange(float minTheta, float maxTheta)
		{
			float tempMaxThetaRange;
			float tempMinThetaRange;
			float tempMaxTheta;
			float tempMinTheta;

			const float TWO_PI = 2*3.14159f;

			if (mMinThetaRange < mMaxThetaRange)
			{
				// We're in a normal 
				if (minTheta < maxTheta)
				{
					tempMaxThetaRange = mMaxThetaRange;
					tempMinThetaRange = mMinThetaRange;
					tempMaxTheta = maxTheta;
					tempMinTheta = minTheta;
				}
				else
				{
					tempMaxThetaRange = mMaxThetaRange;
					tempMinThetaRange = mMinThetaRange;
					tempMaxTheta = maxTheta + TWO_PI;
					tempMinTheta = minTheta;
				}

			}
			else
			{

				// We're crossing a cut line (at 2pi)
				if (minTheta < maxTheta)
				{
					tempMaxThetaRange = mMaxThetaRange + TWO_PI;
					tempMinThetaRange = mMinThetaRange;
					tempMaxTheta = maxTheta;
					tempMinTheta = minTheta;
				}
				else
				{
					tempMaxThetaRange = mMaxThetaRange + TWO_PI;
					tempMinThetaRange = mMinThetaRange;
					tempMaxTheta = maxTheta + TWO_PI;
					tempMinTheta = minTheta;
				}

			}


			bool onOneSide = tempMinTheta>tempMaxThetaRange && tempMaxTheta>tempMaxThetaRange;
			bool onOtherSide = tempMinTheta<tempMinThetaRange && tempMaxTheta<tempMinThetaRange;

			return !(onOneSide || onOtherSide);

		}


	} // namespace WebKit
} // namespace EA