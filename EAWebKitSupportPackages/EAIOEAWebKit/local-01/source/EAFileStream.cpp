/*
Copyright (C) 2009-2010, 2013 Electronic Arts, Inc.  All rights reserved.

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

/////////////////////////////////////////////////////////////////////////////
// EAFileStream.cpp
//
// Copyright (c) 2004, Electronic Arts Inc. All rights reserved.
/////////////////////////////////////////////////////////////////////////////


#include <EAIO/internal/Config.h>
#ifndef INCLUDED_eabase_H
   #include "EABase/eabase.h"
#endif

   #include "Win32/EAFileStreamWin32.cpp"


namespace EA
{
    namespace IO
    {
        // Forward declarations
        EAIO_API bool PushFileErrorHandler(ErrorHandlingFunction pErrorHandlingFunction, void* pContext);
        EAIO_API bool RemoveFileErrorHandler(ErrorHandlingFunction pErrorHandlingFunction, void* pContext);
        EAIO_API void GetFileErrorHandler(ErrorHandlingFunction& pErrorHandlingFunction, void*& pContext);


        struct ErrorHandlingFunctionEntry
        {
            ErrorHandlingFunction mpErrorHandlingFunction;
            void*                 mpContext;
        };

        ErrorHandlingFunctionEntry gErrorHandlingFunctionArray[8];
        size_t                     gErrorHandlingFunctionArraySize = 0;


        static ErrorResponse ErrorHandlingFunctionDefault(const FileErrorInfo&, void*)
        {
            return kErrorResponseCancel;
        }


        EAIO_API bool PushFileErrorHandler(ErrorHandlingFunction pErrorHandlingFunction, void* pContext)
        {
            // This code is currently not thread-safe. It currently requires that you coordinate the 
            // registration of error handlers in a thread safe way. Also note that thread safety
            // also has to be coordinated with any code that calls the error handler. Clearly this 
            // is a potentially significant limitation. The likely resolution is to implement the 
            // required functionality with atomic operations.

            if(gErrorHandlingFunctionArraySize < (sizeof(gErrorHandlingFunctionArray) / sizeof(gErrorHandlingFunctionArray[0])))
            {
                gErrorHandlingFunctionArray[gErrorHandlingFunctionArraySize].mpErrorHandlingFunction = pErrorHandlingFunction;
                gErrorHandlingFunctionArray[gErrorHandlingFunctionArraySize].mpContext = pContext;
                gErrorHandlingFunctionArraySize++;
                return true;
            }

            return false;
        }


        EAIO_API bool RemoveFileErrorHandler(ErrorHandlingFunction pErrorHandlingFunction, void* pContext)
        {
            // See comments in PushFileErrorHandler regarding lack of thread safety in this code.

            for(size_t i = 0; i < gErrorHandlingFunctionArraySize; i++)
            {
                if((gErrorHandlingFunctionArray[i].mpErrorHandlingFunction == pErrorHandlingFunction) && 
                   (gErrorHandlingFunctionArray[i].mpContext == pContext))
                {
                    memcpy(gErrorHandlingFunctionArray + i, 
                           gErrorHandlingFunctionArray + i + 1, 
                           ((gErrorHandlingFunctionArraySize - (i + 1))) * sizeof(ErrorHandlingFunctionEntry));
                    gErrorHandlingFunctionArraySize--;
                    return true;
                }
            }

            return false;
        }


        EAIO_API void GetFileErrorHandler(ErrorHandlingFunction& pErrorHandlingFunction, void*& pContext)
        {
            // See comments in PushFileErrorHandler regarding lack of thread safety in this code.

            if(gErrorHandlingFunctionArraySize)
            {
                pErrorHandlingFunction = gErrorHandlingFunctionArray[gErrorHandlingFunctionArraySize - 1].mpErrorHandlingFunction;
                pContext               = gErrorHandlingFunctionArray[gErrorHandlingFunctionArraySize - 1].mpContext;
            }
            else
            {
                pErrorHandlingFunction = ErrorHandlingFunctionDefault;
                pContext               = NULL;
            }
        }

    } // namespace IO

} // namespace EA
















