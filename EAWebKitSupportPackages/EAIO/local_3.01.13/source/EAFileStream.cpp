/////////////////////////////////////////////////////////////////////////////
// EAFileStream.cpp
//
// Copyright (c) 2004, Electronic Arts Inc. All rights reserved.
/////////////////////////////////////////////////////////////////////////////


#include <EAIO/internal/Config.h>
#ifndef INCLUDED_eabase_H
   #include "EABase/eabase.h"
#endif

#if defined(EA_PLATFORM_MICROSOFT)
	#include "Win32/EAFileStreamWin32.cpp"
#elif defined(EA_PLATFORM_ANDROID)
	#include "Android/EAFileStreamAndroid.cpp"
#elif defined(EA_PLATFORM_NX)
	#include "NX/EAFileStreamNX.cpp"
#elif EAIO_USE_UNIX_IO
	#include "Unix/EAFileStreamUnix.cpp"
#elif defined(EA_PLATFORM_SONY)
	#include "Sony/EAFileStreamSony.cpp"
#else
	#include "StdC/EAFileStreamStdC.cpp"
#endif


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


		static ErrorResponse ErrorHandlingFunctionDefault(const FileErrorInfo& fileErrorInfo, void*)
		{
			(void)fileErrorInfo; // Suppress possible unused variable warnings.
			return kErrorResponseCancel;
		}


		EAIO_API bool PushFileErrorHandler(ErrorHandlingFunction pErrorHandlingFunction, void* pContext)
		{
			// This code is currently not thread-safe. It currently requires that you coordinate the 
			// registration of error handlers in a thread safe way. Also note that thread safety
			// also has to be coordinated with any code that calls the error handler. Clearly this 
			// is a potentially significant limitation. The likely resolution is to implement the 
			// required functionality with atomic operations.

			if(gErrorHandlingFunctionArraySize < EAArrayCount(gErrorHandlingFunctionArray))
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
				// PushFileErrorHandler guarantees that gErrorHandlingFunctionArraySize is always a valid index.
				EA_ASSERT(gErrorHandlingFunctionArraySize <= EAArrayCount(gErrorHandlingFunctionArray));
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
















