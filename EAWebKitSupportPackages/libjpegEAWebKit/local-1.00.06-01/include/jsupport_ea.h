/*
Copyright (C) 2010 Electronic Arts, Inc.  All rights reserved.

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

#ifndef LIBJEG_JSUPPORT_EA_H
#define LIBJEG_JSUPPORT_EA_H


#if defined(__cplusplus)
    extern "C" {
#endif


/* The following are custom declarations defined so that users can override memory */
/* allocation at runtime as opposed to compile-time. By default these point to     */
/* malloc and free.                                                                */

typedef void* (*jpeg_malloc_func)(size_t size);
typedef void  (*jpeg_free_func)(void* memptr);

extern jpeg_malloc_func gpMalloc;
extern jpeg_free_func   gpFree;

void jpeg_set_malloc_functions(jpeg_malloc_func  pMalloc, jpeg_free_func  pFree);
void jpeg_get_malloc_functions(jpeg_malloc_func* pMalloc, jpeg_free_func* pFree);



/* The following are custom declarations defined so that users can override file   */
/* io at runtime as opposed to compile-time. By default these point to             */
/* fread and fwrite. The expected behaviour is the same as fread/fwrite.           */

typedef size_t (*jpeg_read_func)(void* pFile, void* buffer, size_t capacity);
typedef size_t (*jpeg_write_func)(void* pFile, const void* buffer, size_t size);

extern jpeg_read_func  gpRead;
extern jpeg_write_func gpWrite;

void jpeg_set_io_functions(jpeg_read_func  pRead, jpeg_write_func  pWrite);
void jpeg_get_io_functions(jpeg_read_func* pRead, jpeg_write_func* pWrite);



#if defined(__cplusplus)
    }
#endif


#endif // Header include guard
