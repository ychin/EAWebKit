/*
Copyright (C) 2009, 2010, 2011, 2012 Electronic Arts, Inc.  All rights reserved.

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
// EAWebKitString.cpp
//
// (5/7/09 CSidhall - Note this code was extracted from EATEXT StdC.cpp): 
// By Paul Pedriana
///////////////////////////////////////////////////////////////////////////////

#include "config.h"
#include <internal/include/EAWebKitString.h>
#include <EAIO/FnEncode.h> 

namespace EA
{
namespace Internal
{
int Strlcpy(char8_t*  pDest, const char16_t* pSource, size_t nDestCapacity, size_t nSourceLength)
{
	return EA::IO::StrlcpyUTF16ToUTF8(pDest, nDestCapacity, pSource, nSourceLength);
}

int Strlcpy(char16_t* pDest, const char8_t*  pSource, size_t nDestCapacity, size_t nSourceLength)
{
	return EA::IO::StrlcpyUTF8ToUTF16(pDest, nDestCapacity, pSource, nSourceLength);
}

int Strlcpy(char16_t* pDestination, const char16_t* pSource, size_t nDestCapacity)
{
	const char16_t* s = pSource;
	size_t          n = nDestCapacity;

	if(n && --n)
	{
		do{
			if((*pDestination++ = *s++) == 0)
				break;
		} while(--n);
	}

	if(!n)
	{
		if(nDestCapacity)
			*pDestination = 0;
		while(*s++)
		{ }
	}

	return (size_t)(s - pSource - 1);
}

inline char16_t Tolower(char16_t c)
{
    if(c >= 'A' && c <= 'Z')
        return (char16_t)(c + ('a' - 'A'));
    return c;
}
int Stricmp(const char16_t* pString1, const char16_t* pString2)
{
	char16_t c1, c2;

	// PowerPC has lhzu and sthu instructions which make pre-increment 
	// updates faster for the PowerPC's load and store architecture.
#ifdef EA_PROCESSOR_POWERPC
	--pString1;
	--pString2;
	while((c1 = Tolower(*++pString1)) == (c2 = Tolower(*++pString2)))
	{
		if(c1 == 0)
			return 0;
	}
#else
	while((c1 = Tolower(*pString1++)) == (c2 = Tolower(*pString2++)))
	{
		if(c1 == 0)
			return 0;
	}
#endif

	return ((uint16_t)c1 - (uint16_t)c2);
}

inline char8_t Tolower(char8_t c)
{
    if(c >= 'A' && c <= 'Z')
        return (char8_t)(c + ('a' - 'A'));
    return c;
}

int Stricmp(const char8_t* pString1, const char8_t* pString2)
{
	char8_t c1, c2;

	// PowerPC has lhzu and sthu instructions which make pre-increment 
	// updates faster for the PowerPC's load and store architecture.
#ifdef EA_PROCESSOR_POWERPC
	--pString1;
	--pString2;
	while((c1 = Tolower(*++pString1)) == (c2 = Tolower(*++pString2)))
	{
		if(c1 == 0)
			return 0;
	}
#else
	while((c1 = Tolower(*pString1++)) == (c2 = Tolower(*pString2++)))
	{
		if(c1 == 0)
			return 0;
	}
#endif

	return ((uint8_t)c1 - (uint8_t)c2);
}

char8_t* Stristr(const char8_t* s1, const char8_t* s2)
{
    const char8_t* cp = s1;

    if(!*s2)
        return (char8_t*)s1;

    while(*cp)
    {
        const char8_t* s = cp;
        const char8_t* t = s2;

        while(*s && *t && (Tolower(*s) == Tolower(*t)))
            ++s, ++t;

        if(*t == 0)
            return (char8_t*)cp;
        ++cp;
    }

    return 0;
}

char16_t* Stristr(const char16_t* s1, const char16_t* s2)
{
	const char16_t* cp = s1;

	if(!*s2)
		return (char16_t*)s1;

	while(*cp)
	{
		const char16_t* s = cp;
		const char16_t* t = s2;

		while(*s && *t && (Tolower(*s) == Tolower(*t)))
			++s, ++t;

		if(*t == 0)
			return (char16_t*)cp;
		++cp;
	}

	return 0;
}

size_t Strlen(const char8_t* pString)
{
	ssize_t nLength = (size_t)-1; // EABase 1.0.14 and later recognize ssize_t for all platforms.

	// PowerPC has lhzu and sthu instructions which make pre-increment 
	// updates faster for the PowerPC's load and store architecture.
#ifdef EA_PROCESSOR_POWERPC
	const char8_t* p = pString - 1;
	do{
		++nLength;
	}while(*++p);
#else
	do{
		++nLength;
	}while(*pString++);

#endif

	return (size_t)nLength;
}

size_t Strlen(const char16_t* pString)
{
	size_t nLength = (size_t)-1;

	// PowerPC has lhzu and sthu instructions which make pre-increment 
	// updates faster for the PowerPC's load and store architecture.
#ifdef EA_PROCESSOR_POWERPC
	const char16_t* p = pString - 1;
	do{
		++nLength;
	}while(*++p);
#else
	do{
		++nLength;
	}while(*pString++);

#endif

	return nLength;
}

char16_t* Strcpy(char16_t* pDestination, const char16_t* pSource)
{
	// PowerPC has lhzu and sthu instructions which make pre-increment 
	// updates faster for the PowerPC's load and store architecture.
#ifdef EA_PROCESSOR_POWERPC
	const char16_t* s = (char16_t*)pSource - 1;
	char16_t*       d = (char16_t*)pDestination - 1;

	while((*++d = *++s) != 0)
	{} // Do nothing.
#else
	const char16_t* s = pSource;
	char16_t*       d = pDestination;

	while((*d++ = *s++) != 0)
	{} // Do nothing.
#endif

	return pDestination;
}
 
char16_t* Strncpy(char16_t* pDestination, const char16_t* pSource, size_t n)
{
	// PowerPC has lhzu and sthu instructions which make pre-increment 
	// updates faster for the PowerPC's load and store architecture.
#ifdef EA_PROCESSOR_POWERPC
	const char16_t* s = pSource - 1;
	char16_t*       d = pDestination - 1;

	n++;
	while(--n)
	{
		if((*++d = *++s) == 0)
		{
			while(--n)
				*++d = 0;
			break;
		}
	}
#else
	const char16_t* s = pSource;
	char16_t*       d = pDestination;

	n++;
	while(--n)
	{
		if((*d++ = *s++) == 0)
		{
			while(--n)
				*d++ = 0;
			break;
		}
	}
#endif

	return pDestination;
}

char16_t* Strcat(char16_t* pDestination, const char16_t* pSource)
{
	// PowerPC has lhzu and sthu instructions which make pre-increment 
	// updates faster for the PowerPC's load and store architecture.
#ifdef EA_PROCESSOR_POWERPC
	const char16_t* s = pSource - 1;
	char16_t*       d = pDestination - 1;

	while(*++d){}          // Do nothing.
	--d;
	while((*++d = *++s) != 0)
	{} // Do nothing.
#else
	const char16_t* s = pSource;
	char16_t*       d = pDestination;

	while(*d++){}          // Do nothing.
	--d;
	while((*d++ = *s++) != 0)
	{} // Do nothing.
#endif

	return pDestination;
}

} // namespace Internal

} // namespace EA







