/*
Copyright (C) 2013 Electronic Arts, Inc.  All rights reserved.

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
// EAWebKitDll.h
// By Arpit Baldeva
// Maintained by EAWebKit Team
//
// The purpose of this file is isolate dll export related code.
///////////////////////////////////////////////////////////////////

#ifndef EAWEBKIT_EAWEBKITDLL_H
#define EAWEBKIT_EAWEBKITDLL_H

#include "DLLInterface.h"

#if BUILDING_EAWEBKIT_DLL
	#define DLL_INTERFACE PLATFORM_DLL_EXPORT_INTERFACE
#else
	#define DLL_INTERFACE PLATFORM_DLL_IMPORT_INTERFACE
#endif

namespace EA
{
namespace WebKit
{
class EAWebKitLib;
}
}

#ifndef PLATFORM_DLL_EXPORT_FUNC_DECLARE
	#if defined(__cplusplus)
		#define PLATFORM_DLL_EXPORT_FUNC_DECLARE(funcname,rettype,interfacetype) extern "C" interfacetype rettype funcname(); 
	#else
		#define PLATFORM_DLL_EXPORT_FUNC_DECLARE(funcname,rettype,interfacetype) interfacetype rettype funcname(); 
	#endif
#endif

PLATFORM_DLL_EXPORT_FUNC_DECLARE(CreateEAWebkitInstance,EA::WebKit::EAWebKitLib*,DLL_INTERFACE);

#endif//EAWEBKIT_EAWEBKITDLL_H
