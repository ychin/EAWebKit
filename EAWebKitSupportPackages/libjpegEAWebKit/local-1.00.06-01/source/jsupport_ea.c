/*
Copyright (C) 2009 Electronic Arts, Inc.  All rights reserved.

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


#define JPEG_INTERNALS
#include "jinclude.h"
#include "jpeglib.h"
#include "jmemsys.h"
#include "jsupport_ea.h"
#include <stdlib.h>
#include <stdio.h>


// LIBJPEG_EA_SUPPORT_ENABLED
//
// Defined as 0 or 1
// If defined as 1 then we provide functionality that allows the user to override 
// memory allocation and file IO functions. 
// If not defined or defined as 0 then libjeg is unchanged.
//
#if defined(LIBJPEG_EA_SUPPORT_ENABLED) && LIBJPEG_EA_SUPPORT_ENABLED

    /* In addition to adding this jsupport_ea.h and jsupport_ea.c files,        */
    /* we have also modified jinclude.h to allow for the overriding of file IO. */
    #if !defined(LIBJPEG_EA_JREAD_REDEFINED)
        /* If this fails then somebody has replaced jinclude.h and lost our custom addition to it. */
        #error
    #endif



    /* We have two global function pointers which store the malloc and free functions. */
    /* They default to the Standard C library malloc and free, but can be overridden.  */
    jpeg_malloc_func gpMalloc = malloc;
    jpeg_free_func   gpFree   = free;


    void jpeg_set_malloc_functions(jpeg_malloc_func pMalloc, jpeg_free_func pFree)
    {
        gpMalloc = pMalloc;
        gpFree   = pFree;
    }


    void jpeg_get_malloc_functions(jpeg_malloc_func* pMalloc, jpeg_free_func* pFree)
    {
        *pMalloc = gpMalloc;
        *pFree   = gpFree;
    }



    /* We have two global function pointers which store the read and write functions. */
    /* The default to the Standard C library fread and fwrite, but can be overridden. */
    /* The default will fail or crash if the pFile isn't actually a FILE*.            */
    static size_t fread_ea(void* pFile, void* buffer, size_t capacity)
        { return fread(buffer, 1, capacity, (FILE*)pFile); }
    static size_t fwrite_ea(void* pFile, const void* buffer, size_t size)
        { return fwrite(buffer, 1, size, (FILE*)pFile); }

    jpeg_read_func  gpRead  = fread_ea;
    jpeg_write_func gpWrite = fwrite_ea;


    void jpeg_set_io_functions(jpeg_read_func pRead, jpeg_write_func pWrite)
    {
        gpRead  = pRead;
        gpWrite = pWrite;
    }

    void jpeg_get_io_functions(jpeg_read_func* pRead, jpeg_write_func* pWrite)
    {
        *pRead  = gpRead;
        *pWrite = gpWrite;
    }





    /* ************* */
    /* Internal code */
    /* ************* */

    void* jpeg_get_small(j_common_ptr cinfo, size_t sizeofobject)
    {
        return (*gpMalloc)(sizeofobject);
    }

    void jpeg_free_small(j_common_ptr cinfo, void* object, size_t sizeofobject)
    {
        (*gpFree)(object);
    }



    void* jpeg_get_large(j_common_ptr cinfo, size_t sizeofobject)
    {
        return (*gpMalloc)(sizeofobject);
    }

    void jpeg_free_large(j_common_ptr cinfo, void* object, size_t sizeofobject)
    {
        (*gpFree)(object);
    }


    long jpeg_mem_available(j_common_ptr cinfo, long min_bytes_needed, long max_bytes_needed, long already_allocated)
    {
        return max_bytes_needed;
    }


    static void read_backing_store(j_common_ptr cinfo, backing_store_ptr info, void* buffer_address, long file_offset, long byte_count)
    {
        (void)cinfo; (void)info; (void)file_offset;

        /* Currently disabled because we don't really need this code, */
        /* and we'd have to redirect it to our own file IO functions. */

        /*
        #if defined(EA_PLATFORM_WINDOWS) ||defined(CS_UNDEFINED_STRING) // Add other platforms as necessary.
            if(fseek(info->temp_file, file_offset, SEEK_SET) == 0)
            {
                if(fread(buffer_address, 1, byte_count, info->temp_file) == (size_t)byte_count)
                    return; // success
            }
        #endif
        */

        memset(buffer_address, 0, byte_count);
    }

    static void write_backing_store(j_common_ptr cinfo, backing_store_ptr info, void* buffer_address, long file_offset, long byte_count)
    {
        (void)cinfo; (void)info; (void)buffer_address; (void)file_offset; (void)byte_count;

        /* Currently disabled because we don't really need this code, */
        /* and we'd have to redirect it to our own file IO functions. */

        /*
        #if defined(EA_PLATFORM_WINDOWS) || defined(CS_UNDEFINED_STRING) // Add other platforms as necessary.
            if(fseek(info->temp_file, file_offset, SEEK_SET) == 0)
            {
                if(fwrite(buffer_address, 1, byte_count, info->temp_file) == (size_t)byte_count)
                    return; // success
            }
        #endif
        */
    }

    static void close_backing_store(j_common_ptr cinfo, backing_store_ptr info)
    {
        (void)cinfo; (void)info;

        /*
        #if defined(EA_PLATFORM_WINDOWS) || defined(CS_UNDEFINED_STRING) // Add other platforms as necessary.
            fclose(info->temp_file);
        #else
            // Do nothing.
        #endif
        */
    }


    void jpeg_open_backing_store(j_common_ptr cinfo, backing_store_ptr info, long total_bytes_needed)
    {
        /* Currently disabled because we don't really need this code, */
        /* and we'd have to redirect it to our own file IO functions. */

        /* info->temp_file = tmpfile(); */
        /* if(info->temp_file) */
        {
            info->read_backing_store  = read_backing_store;
            info->write_backing_store = write_backing_store;
            info->close_backing_store = close_backing_store;
        }
    }


    long jpeg_mem_init(j_common_ptr cinfo)
    {
        return 0;
    }

    void jpeg_mem_term(j_common_ptr cinfo)
    {
    }


#endif // #if defined(LIBJPEG_EA_SUPPORT_ENABLED)



