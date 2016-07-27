/*
Copyright (C) 2010, 2011, 2012, 2013 Electronic Arts, Inc.  All rights reserved.

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
// EAWebKitFPUPrecision.cpp
// By Arpit Baldeva 
// Part of the implementation/comments are borrowed from Paul Pedriana/other EA packages.
// Maintained by EAWebKit Team
///////////////////////////////////////////////////////////////////////////////

#include "config.h"
#include <internal/include/EAWebKitFPUPrecision.h>
#include <EABase/eabase.h>
#include <internal/include/EAWebKitAssert.h>


#if defined(EA_PLATFORM_WIN32) 

#include <float.h>
namespace EA
{
namespace WebKit
{
AutoFPUPrecision::AutoFPUPrecision(EA::WebKit::FPUPrecision precisionDesired)
{
	unsigned int controlWord = 0;

	switch (precisionDesired)
	{
	case kFPUPrecisionExtended:
		controlWord = _PC_64;
		break;
	case kFPUPrecisionDouble:
		controlWord = _PC_53;
		break;
	case kFPUPrecisionSingle:
		controlWord = _PC_24;
		break;
	default:
		EAW_ASSERT_MSG(false,"Invalid FPUPrecision specified");
		break;
	}
	_controlfp_s(&mSavedControlWord, 0, 0); //Save original Control word
	_controlfp_s(NULL, controlWord, MCW_PC);//Set the higher precision
}

AutoFPUPrecision::~AutoFPUPrecision()
{
	_controlfp_s(NULL, mSavedControlWord, MCW_PC);//restore the original FPU setting
}

}
}


#endif 
