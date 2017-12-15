/*
Copyright (C) 2005,2009, 2012, 2013 Electronic Arts, Inc.  All rights reserved.

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

/*H********************************************************************************/
/*!
    \File dirtyplatform.h
*/
/********************************************************************************H*/

#ifndef _dirtyplatform_h
#define _dirtyplatform_h

/*** Include files **********************************************************
 ******/

#ifndef DIRTYCODE_UNDEF
#define DIRTYCODE_UNDEF         1   
#endif

#if   defined (_WIN32) || defined(_Win64)
#undef EA_PLATFORM_WINDOWS
#define EA_PLATFORM_WINDOWS 1
#endif

#include <stdarg.h>

    #ifndef __int8_t_defined
        typedef signed char             int8_t;
        typedef signed short            int16_t;
        typedef signed int              int32_t;
        typedef signed long long        int64_t;
        #define __int8_t_defined
    #endif
    #ifndef __uint32_t_defined
        typedef unsigned char           uint8_t;
        typedef unsigned short          uint16_t;
        typedef unsigned int            uint32_t;
        typedef unsigned long long      uint64_t;
        #define __uint32_t_defined
    #endif
    #ifndef _intptr_t_defined
        #if defined(_WIN64)
            typedef signed long long    intptr_t;
        #else
            typedef signed int          intptr_t;
        #endif
        #define _intptr_t_defined
    #endif
    #ifndef _uintptr_t_defined
        #if defined(_WIN64)
            typedef unsigned long long  uintptr_t;
        #else
            typedef unsigned int        uintptr_t;
        #endif
        #define _uintptr_t_defined
    #endif

#include <time.h>

/*** Defines **********************************************************************/

#ifdef  TRUE
#undef  TRUE
#undef  FALSE
#endif

#define FALSE (0)
#define TRUE  (1)

#ifdef  NULL
#undef  NULL
#endif

#ifdef __cplusplus
#define NULL 0
#else
#define NULL ((void *)0)
#endif

#ifndef DIRTYCODE_DEBUG
 #if defined(EA_DEBUG)
  #define DIRTYCODE_DEBUG (1)
 #elif defined(RWDEBUG)
  #define DIRTYCODE_DEBUG (1)
 #else
  #define DIRTYCODE_DEBUG (0)
 #endif
#endif

/*** Macros ***********************************************************************/

#ifdef DS_PLATFORM
#if DIRTYCODE_DEBUG
 #define logprintf(_x) _Platform_pLogPrintf _x
#else
 #define logprintf(_x) { }
#endif

#define vsnzprintf ds_vsnzprintf
#define snzprintf ds_snzprintf
#define strnzcpy ds_strnzcpy
#define strcasecmp ds_stricmp
#define strncasecmp ds_strnicmp
#ifndef stricmp
#define stricmp ds_stricmp
#endif
#ifndef strnicmp
#define strnicmp ds_strnicmp
#endif
#define stristr ds_stristr
#define localtime ds_localtime

#define strsubzcpy ds_strsubzcpy
#define strnzcat ds_strnzcat
#define strsubzcat ds_strsubzcat

#define timezone ds_timezone
#define timetosecs ds_timetosecs
#define secstotime ds_secstotime
#define timeinsecs ds_timeinsecs
#define strtotime ds_strtotime
#endif // DS_PLATFORM

/*** Type Definitions *************************************************************/

/*** Variables ********************************************************************/

#if DIRTYCODE_DEBUG
extern int (*_Platform_pLogPrintf)(const char *pFmt, ...);
#endif

/*** Functions ********************************************************************/

#ifdef __cplusplus
extern "C" {
#endif

uint32_t ds_timeinsecs(void);
int32_t ds_timezone(void);
struct tm *ds_localtime(struct tm *tm, uint32_t elap);
struct tm *ds_secstotime(struct tm *tm, uint32_t elap);
uint32_t ds_timetosecs(const struct tm *tm);
uint32_t ds_strtotime(const char *str);

int32_t ds_vsnzprintf(char *pBuffer, int32_t iLength, const char *pFormat, va_list Args);
int32_t ds_snzprintf(char *pBuffer, int32_t iLength, const char *pFormat, ...);
char *ds_strnzcpy(char *pDest, const char *pSource, int32_t iCount);
int32_t ds_strnzcat(char *pDst, const char *pSrc, int32_t iDstLen);
int32_t ds_stricmp(const char *pString1, const char *pString2);
int32_t ds_strnicmp(const char *pString1, const char *pString2, uint32_t uCount);
char *ds_stristr(const char *pHaystack, const char *pNeedle);

int32_t ds_strsubzcpy(char *pDst, int32_t iDstLen, const char *pSrc, int32_t iSrcLen);
int32_t ds_strsubzcat(char *pDst, int32_t iDstLen, const char *pSrc, int32_t iSrcLen);

#ifdef __cplusplus
}
#endif

#endif // _dirtyplatform_h
