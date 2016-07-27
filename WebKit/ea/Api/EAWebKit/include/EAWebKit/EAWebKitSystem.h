/*
Copyright (C) 2011, 2012, 2013 Electronic Arts, Inc.  All rights reserved.

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

////////////////////////////////////////////////////////////////////
// EAWebKitSystem.h
// By Arpit Baldeva
// Maintained by EAWebKit Team
//
// The purpose of this file is to provide a cross-platform abstraction which is free from
// EAWebKit API details. Thus, this file can be included in various WebCore source files without
// violating layer abstraction. You can think of this file as a system header.
///////////////////////////////////////////////////////////////////

#ifndef EAWEBKIT_EAWEBKITSYSTEM_H
#define EAWEBKIT_EAWEBKITSYSTEM_H

namespace EA { namespace WebKit {

// Very often, in our API, we use char8_t and it is not clear to the users if they can actually pass utf8 encoded data or only
// ascii data is accepted. An explicit typedef makes clears up that confusion.
typedef char8_t utf8_t; 

enum ScrollbarPolicy 
{ 
	ScrollbarAuto, 
	ScrollbarAlwaysOff, 
	ScrollbarAlwaysOn 
};

enum ScrollbarOrientation 
{
	ScrollbarHorizontal,
	ScrollbarVertical
};

// Default Viewport size in which the View is rendered
enum ViewDefault
{
	kViewWidthDefault  = 800,
	kViewHeightDefault = 600
};

enum WebAction 
{
	NoWebAction = - 1,
	OpenLink,

	OpenLinkInNewWindow,
	OpenFrameInNewWindow,

	DownloadLinkToDisk,
	CopyLinkToClipboard,

	OpenImageInNewWindow,
	DownloadImageToDisk,
	CopyImageToClipboard,
	Back,
	Forward,
	Stop,
	Reload,

	Cut,
	Copy,
	Paste,
	Undo,
	Redo,
	MoveToNextChar,
	MoveToPreviousChar,
	MoveToNextWord,
	MoveToPreviousWord,
	MoveToNextLine,
	MoveToPreviousLine,
	MoveToStartOfLine,
	MoveToEndOfLine,
	MoveToStartOfBlock,
	MoveToEndOfBlock,
	MoveToStartOfDocument,
	MoveToEndOfDocument,
	SelectNextChar,
	SelectPreviousChar,
	SelectNextWord,
	SelectPreviousWord,
	SelectNextLine,
	SelectPreviousLine,
	SelectStartOfLine,
	SelectEndOfLine,
	SelectStartOfBlock,
	SelectEndOfBlock,
	SelectStartOfDocument,
	SelectEndOfDocument,
	DeleteStartOfWord,
	DeleteEndOfWord,
	SetTextDirectionDefault,
	SetTextDirectionLeftToRight,
	SetTextDirectionRightToLeft,

	ToggleBold,
	ToggleItalic,
	ToggleUnderline,

	InspectElement,
	InsertParagraphSeparator,
	InsertLineSeparator,

	SelectAll,
	ReloadAndBypassCache,
	PasteAndMatchStyle,
	RemoveFormat,

	ToggleStrikethrough,
	ToggleSubscript,
	ToggleSuperscript,
	InsertUnorderedList,
	InsertOrderedList,
	Indent,
	Outdent,

	AlignCenter,
	AlignJustified,
	AlignLeft,
	AlignRight,

	StopScheduledPageRefresh,
	CopyImageUrlToClipboard,

	WebActionCount
};


// Jump directions enumeration when navigating through a controller
enum JumpDirection
{
	JumpLeft=0,
	JumpUp,
	JumpRight,
	JumpDown
};

class IntPoint
{
public:
	IntPoint()
		: mX(0)
		, mY(0)
	{
	}
	IntPoint(int x, int y)
		: mX(x)
		, mY(y)
	{
	}
	int mX, mY;
};

class IntSize
{
public:
	IntSize()
		: mWidth(0)
		, mHeight(0)
	{
	}
	IntSize(int width, int height)
		: mWidth(width)
		, mHeight(height)
	{
	}
	int mWidth, mHeight;
};

class IntRect
{
public:
	IntRect()
		: mLocation()
		, mSize()
	{
	}
	IntRect(IntPoint location, IntSize size)
		: mLocation(location)
		, mSize(size)
	{
	}

	IntRect(int x, int y, int w, int h)
        : mLocation(x, y)
		, mSize(w, h)
	{
	}

	IntPoint mLocation;
	IntSize mSize;
};

class FloatPoint
{
public:
    FloatPoint(void)
    : mX(0)
    , mY(0)
    {
        // Do nothing.
    }

    FloatPoint(float x, float y)
    : mX(x)
    , mY(y)
    {
        // Do nothing.
    }
    float mX, mY;
};

class FloatSize
{
public:
    FloatSize(void)
    : mWidth(0)
    , mHeight(0) 
    {
        // Do nothing.
    }

    FloatSize(float width, float height)
    : mWidth(width)
    , mHeight(height)
    {
        // Do nothing.
    }

    float mWidth, mHeight;
};

class FloatRect
{
public:

    FloatRect(void)
    : mLocation()
    , mSize()
    {
        // Do nothing.
    }

    FloatRect(FloatPoint location, FloatSize size)
    : mLocation(location)
    , mSize(size)
    {
        // Do nothing.
    }

    FloatRect(float x, float y, float w, float h)
    : mLocation(x, y)
    , mSize(w, h)
    {
        // Do nothing.
    }

    FloatPoint mLocation;
    FloatSize mSize;
};

class TransformationMatrix
{
public:
    TransformationMatrix(void)
    {
        mComponents[0][0] = 1.0f;
        mComponents[0][1] = 0.0f;
        mComponents[0][2] = 0.0f;
        mComponents[0][3] = 0.0f;

        mComponents[1][0] = 0.0f;
        mComponents[1][1] = 1.0f;
        mComponents[1][2] = 0.0f;
        mComponents[1][3] = 0.0f;

        mComponents[2][0] = 0.0f;
        mComponents[2][1] = 0.0f;
        mComponents[2][2] = 1.0f;
        mComponents[2][3] = 0.0f;

        mComponents[3][0] = 0.0f;
        mComponents[3][1] = 0.0f;
        mComponents[3][2] = 0.0f;
        mComponents[3][3] = 1.0f;
    }

    float mComponents[4][4];
};

}}

#endif//EAWEBKIT_EAWEBKITSYSTEM_H
