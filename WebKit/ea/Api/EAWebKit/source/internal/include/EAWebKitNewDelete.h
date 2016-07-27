/*
Copyright (C) 2010, 2014 Electronic Arts, Inc.  All rights reserved.

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
// EAWebKitNewDelete.h
//
// By Arpit Baldeva (Some implementation details are copied from Paul Pedriana's work in other EA proprietary packages.
///////////////////////////////////////////////////////////////////////////////


#ifndef EAWEBKIT_EAWEBKITNEWDELETE_H
#define EAWEBKIT_EAWEBKITNEWDELETE_H

#include <EAWebKit/EAWebKitConfig.h>

//We keep our new/delete implementation as a simple wrapper around new/delete. We don't implement them in terms of
// templated new etc which results in slightly complex user syntax.

//Since we EAWebKit is always going to be used as a dll, this is pretty effective. 

//Intercept any global usage. However, We should use the macros defined below. So that we can better 
//control the allocation if we need in the future.
void* operator new		(size_t size);
void* operator new[]	(size_t size);
void operator delete	(void *p);
void operator delete[]	(void *p);

//For use with EAWEBKIT_NEW/EAWEBKIT_NEW_ALIGNED macros
void* operator new(size_t size, const char* name, int flags, unsigned debugFlags = 0, const char* file = 0, int line = 0);
void* operator new(size_t size, size_t alignment, size_t alignmentOffset, const char* name, int flags = 0, unsigned debugFlags = 0, const char* file = 0, int line = 0);

//For EASTL
void* operator new[](size_t size, const char* name, int flags, unsigned debugFlags = 0, const char* file = 0, int line = 0);
void* operator new[](size_t size, size_t alignment, size_t alignmentOffset, const char* name, int flags = 0, unsigned debugFlags = 0, const char* file = 0, int line = 0);
//void operator delete[](void *p, const char* /*name*/, int /*flags*/, unsigned /*debugFlags*/, const char* /*file*/, int /*line*/);
//void operator delete[](void *p, size_t /*alignment*/, size_t /*alignmentOffset*/, const char* /*name*/, int /*flags*/, unsigned /*debugFlags*/, const char* /*file*/, int /*line*/);

/*
// The new/delete overrides below are there to deal with an MS VC++ issue.
// When compiling some VC++ code (you especially run into this with MFC), 
// you will find that Microsoft does this in debug builds:
//    #define new new(__FILE__, __LINE__)
// So you have code that uses just new(), it gets silently transformed into a different 
// function call. To make sure you aren't allocating memory from one heap (i.e. MS) 
// and free it with another (i.e. your heap), you need to make sure that you implement 
// that file/line version yourself so that those redefines result in your heap being used.
void* operator new     (size_t n, const char* pFileName, int nLine);
void* operator new[]   (size_t n, const char* pFileName, int nLine);
void  operator delete  (void* p, const char* pFileName, int nLine);
void  operator delete[](void* p, const char* pFileName, int nLine);
*/

//Placement new
//void* operator new(size_t, void* p); 
//void* operator new[](size_t, void* p);




#endif // EAWEBKIT_EAWEBKITNEWDELETE_H
