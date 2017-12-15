/*
Copyright (C) 2009,2010,2012 Electronic Arts, Inc.  All rights reserved.

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
// EAFileStream.h
//
// Copyright (c) 2003, Electronic Arts Inc. All rights reserved.
// Created by Paul Pedriana
/////////////////////////////////////////////////////////////////////////////


#ifndef EAIO_EAFILESTREAM_H
#define EAIO_EAFILESTREAM_H


#ifndef INCLUDED_eabase_H
    #include <EABase/eabase.h>
#endif
#include <EAIO/internal/Config.h>
#ifndef EAIO_EASTREAM_H
   #include <EAIO/EAStream.h>
#endif
#ifndef EAIO_EAFILEBASE_H
   #include <EAIO/EAFileBase.h>
#endif


namespace EA
{
    namespace IO
    {
        // Forward declarations
        class FileStream;


        /// IOResultCode
        /// File stream-specific errors.
        /// The enumerated set for some platforms is defined in the platform-specific FileStream header and
        /// directly match the values from the system headers.
        /// These represent the most common file system errors. However, there are additional errors 
        /// which may be returned by the system APIs which are different from these. You should be 
        /// prepared to receive any value for an error code. 

        /// FileOperation
        /// Defines an operation that can be done on a file.
        enum FileOperation
        {
            kFileOperationNone,
            kFileOperationRead,   /// Includes Read, GetPosition, GetSize.
            kFileOperationWrite,  /// Includes Write, SetSize.
            kFileOperationSeek,   /// Includes SetPosition.
            kFileOperationOpen,   /// Includes Open.
            kFileOperationClose   /// Includes Close.
        };


        /// ErrorResponse
        /// Used by ErrorHandlingFunction to define what to do in case of a file error.
        /// On some platforms, the application is required to infinitely
        /// retry file operations that fail due to drive-not-ready errors. However, the 
        /// user has the option of doing such retries outside of the error callback, 
        /// and so even on such platforms the user may want to return kErrorResponseCancel.
        enum ErrorResponse
        {
            kErrorResponseCancel = 0,    /// Cancel the operation that failed and return an error to the caller of the failed function. The caller may choose to retry the operation himself.
            kErrorResponseRetry  = 1     /// Retry the operation that failed.
        };


        /// FileErrorInfo
        /// Provides information about the file error.
        struct FileErrorInfo
        {
            FileStream*    mpFileStream;                // Will be NULL for a non-FileStream-related operation.
            FileOperation  mFileOperation;              // The operation being attempted.
            DriveType      mDriveType;                  // The type of drive the operation failed on.
            char16_t       mFilePath16[kMaxPathLength]; // The file path associated with the error.
        };

        /// ErrorHandlingFunction
        /// Defines a user-specified function which is called during file errors.
        /// The pContext argument is the arbitrary value the user specified to the
        /// PushFileErrorHandler function. See PushFileErrorHandler and PopFileErrorHandler.
        typedef ErrorResponse (*ErrorHandlingFunction)(const FileErrorInfo& fileErrorInfo, void* pContext);


        /// PushFileErrorHandler
        /// Adds a user-specified error handling function to the top of the stack. 
        /// The specified function will take over handling of error callbacks until
        /// the RemoveFileErrorHandler function is called to remove it. Only the most
        /// most recently pushed error handler will be called. Every call to 
        /// PushFileErrorHandler must be eventually matched with a call to RemoveFileErrorHandler.
        /// However, the removes do not need to occur in reverse order of the pushes.
        /// The pContext parameter is an arbitrary user-specified value which will be 
        /// given back to the user in the error callback.
        ///
        /// The kinds of errors which result in an error handler callback currently 
        /// consist only of errors which are device-level errors.
        /// If the user never calls PushFileErrorHandler, a default error handler is 
        /// called. The default error handler's behaviour is platform and device-dependent,
        /// as it must be due to platform specific requirements.
        /// Returns true if the error handler could be added -- if there was any more room.
        /// This function is not thread-safe.
        ///
        /// It should be noted that a error handler might be called multiple times 
        /// simultaneously by separate threads. This could happen if two threads are trying
        /// to read from a device which develops a media error. Both threads will call the
        /// user-specified error handling function.
        ///
        /// Example usage:
        ///     class SomeClass {
        ///         static ErrorResponse ErrorHandlingFunctionDefault(const FileErrorInfo& fileErrorInfo, void* pContext) {
        ///             SomeClass* pSomeClass = (SomeClass*)pContext; // Possibly use this.
        ///
        ///             #ifdef EA_PLATFORM_PS3
        ///                 if(fileErrorInfo.mDriveType == kDriveTypeDVD)
        ///                     return kErrorResponseRetry;
        ///             #endif
        ///     
        ///             return kErrorResponseCancel;
        ///         }
        ///     };
        ///
        ///     SomeClass gSomeClass;
        ///     PushFileErrorHandler(&SomeClass::ErrorHandlingFunctionDefault, &gSomeClass);
        ///
        EAIO_API bool PushFileErrorHandler(ErrorHandlingFunction pErrorHandlingFunction, void* pContext);


        /// RemoveFileErrorHandler
        /// Matches a previous call to PushFileErrorHandler.
        /// Returns true if the error handler was found and thus successfully removed.
        /// The function and context both must match what was set in PushFileErrorHandler.
        /// This function is not thread-safe.
        EAIO_API bool RemoveFileErrorHandler(ErrorHandlingFunction pErrorHandlingFunction, void* pContext);


        /// GetFileErrorHandler
        /// Gets the top-most error handler. 
        /// Always returns an error handler, which might be the default handler if the user
        /// did not specify a custom handler.
        EAIO_API void GetFileErrorHandler(ErrorHandlingFunction& pErrorHandlingFunction, void*& pContext);


    } // namespace IO

} // namespace EA


   #include <EAIO/Win32/EAFileStreamWin32.h>



#endif // Header include guard














