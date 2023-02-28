///////////////////////////////////////////////////////////////////////////////
// DescribeCallStack
//
// Copyright (c) 2012, Electronic Arts. All Rights Reserved.
// By Paul Pedriana, Maxis
///////////////////////////////////////////////////////////////////////////////


#include <EABase/eabase.h>
#include <PPMalloc/internal/shared.h>
#include <PPMalloc/internal/callstack.h>
#include <stdio.h>

#include <EACallstack/EACallstack.h>
#include <EACallstack/EAAddressRep.h>

#if PPM_BACKTRACE_AVAILABLE
	#include <execinfo.h>
	#include <string.h>
	#include <stdlib.h>
#endif


namespace EA
{
	namespace Allocator
	{
		#if PPM_USE_EATHREAD_CALLSTACK && PPM_DEBUG_CALLSTACK_AVAILABLE
			PPM_API void InitCallstack()
			{
				EA::Callstack::InitCallstack();
			}

			PPM_API void ShutdownCallstack()
			{
				EA::Callstack::ShutdownCallstack();
			}

			PPM_API size_t GetCallStack(void* pReturnAddressArray[], size_t nReturnAddressArrayLength)
			{
				return EA::Callstack::GetCallstack(pReturnAddressArray, nReturnAddressArrayLength);
			}
		#endif

		#if PPM_BACKTRACE_AVAILABLE
			/// DescribeCallStack
			/// Returns number of bytes written to the string buffer. Note that the buffer written
			/// to is not null-terminated and the return value does not include any such null terminator.
			/// The reason for this is that these call stacks are often concatenated into a stream of 
			/// contiguous data and a null terminator within this stream would detrimentally affect 
			/// data output.
			///
			PPM_API size_t DescribeCallStack(const void* pReturnAddressArray[], size_t nReturnAddressArrayLength, char* pBuffer, size_t nBufferLength, EA::Callstack::IAddressRepLookupBase *)
			{
				char* const pBufferSaved = pBuffer;

				// The symbol names may be unavailable without the use of special linker
				// options. For systems using the GNU linker, it is necessary to use the
				// -rdynamic linker option. Note that names of "static" functions are not
				// exposed, and won't be available in the backtrace.
				// http://linux.die.net/man/3/backtrace_symbols

				char** strings = backtrace_symbols((void**)pReturnAddressArray, (int)nReturnAddressArrayLength);

				if(strings)
				{
					for(size_t i = 0; i < nReturnAddressArrayLength; i++)
					{
						if(i && nBufferLength)
						{
							*pBuffer++ = ' ';
							nBufferLength--;
						}

						const size_t len     = strlen(strings[i]);
						const size_t copyLen = (len < nBufferLength) ? len : nBufferLength;

						for(size_t j = 0; j < copyLen; j++)
							*pBuffer++ = strings[i][j];

						nBufferLength -= copyLen;
					}

					free(strings); // This is from the GCC manual. But don't we need to free each string and not just the strings pointer?
				}

				return (pBuffer - pBufferSaved);
			}

		#elif !defined(EA_PLATFORM_WIN32) || (defined(EA_PLATFORM_WIN32) && !EA_WINAPI_FAMILY_PARTITION(EA_WINAPI_PARTITION_DESKTOP))
			/// DescribeCallStack
			/// Returns number of bytes written to the string buffer. Note that the buffer written
			/// to is not null-terminated and the return value does not include any such null terminator.
			/// The reason for this is that these call stacks are often concatenated into a stream of 
			/// contiguous data and a null terminator within this stream would detrimentally affect 
			/// data output.
			/// 
			/// On console platforms, call stack descriptions are often best done on the host 
			/// debugging computer, which is usually a personal workstation of some sort.
			/// We don't provide that functionality here because this functionality is 
			/// strictly runtime functionality.
			///
			PPM_API size_t DescribeCallStack(const void* pReturnAddressArray[], size_t nReturnAddressArrayLength, char* pBuffer, size_t nBufferLength, EA::Callstack::IAddressRepLookupBase *addressRepLookup)
			{
				using namespace EA::Callstack;

				// We simply convert the addresses to a string.
				char* const pBufferSaved = pBuffer;
				char* const pBufferEnd = pBuffer + nBufferLength;

				for (size_t i = 0; i < nReturnAddressArrayLength; i++)
				{
					if (pBuffer == pBufferEnd)
					{
						break;
					}
					else if (i > 0)
					{
						*pBuffer++ = ' ';
					}

					bool printHexCallstack = true;
					if (addressRepLookup)
					{
						FixedString pStrResults[kARTCount];
						int pIntResults[kARTCount];
						int inputFlags = (kARTFlagFunctionOffset | kARTFlagFileLine);
						int outputFlags = addressRepLookup->GetAddressRep(inputFlags, pReturnAddressArray[i], pStrResults, pIntResults);

						if (pBuffer + 2 >= pBufferEnd)
						{
							break;
						}
						*pBuffer++ = '\n';
						*pBuffer++ = '\t';

						if ((outputFlags & kARTFlagFileLine) && pStrResults[kARTFileLine].size())
						{
							if ((pBuffer + PPM_Snprintf(nullptr, 0, "%s(%d) ", pStrResults[kARTFileLine].c_str(), pIntResults[kARTFileLine])) >= pBufferEnd)
							{
								break;
							}
							else
							{
								// Suppress hexadecimal output
								printHexCallstack = false;
								pBuffer += PPM_Snprintf(pBuffer, pBufferEnd - pBuffer, "%s(%d) ", pStrResults[kARTFileLine].c_str(), pIntResults[kARTFileLine]);
							}
						}
						if ((outputFlags & kARTFlagFunctionOffset))
						{
							if ((pBuffer + PPM_Snprintf(nullptr, 0, "%s (offset %d)", pStrResults[kARTFunctionOffset].c_str(), pIntResults[kARTFunctionOffset])) >= pBufferEnd)
							{
								break;
							}
							else
							{
								// Suppress hexadecimal output
								printHexCallstack = false;
								pBuffer += PPM_Snprintf(pBuffer, pBufferEnd - pBuffer, "%s (offset %d)", pStrResults[kARTFunctionOffset].c_str(), pIntResults[kARTFunctionOffset]);
							}
						}
						
						if (printHexCallstack)
						{
							FixedString str("<Unknown> ");
							if (pBuffer + str.size() >= pBufferEnd)
							{
								break;
							}
							else
							{
								strcpy(pBuffer, str.c_str());
								pBuffer += str.size();
							}
						}
					}
					if (printHexCallstack)
					{
						#if defined(EA_PLATFORM_PTR_SIZE) && (EA_PLATFORM_PTR_SIZE == 8)
							if (pBuffer + 18 >= pBufferEnd)
							{
								break;
							}
							else
							{
								sprintf(pBuffer, "0x%016" PRIx64, (uintptr_t)pReturnAddressArray[i]);
								pBuffer += 18; // The above sprintf writes exactly 18 characters.
							}
						#else
							if (pBuffer + 10 >= pBufferEnd)
							{
								break;
							}
							else
							{
								sprintf(pBuffer, "0x%08x", (unsigned)(uintptr_t)pReturnAddressArray[i]);
								pBuffer += 10; // The above sprintf writes exactly 10 characters.
							}
						#endif
					}
				}

				return static_cast<size_t>(pBuffer - pBufferSaved);
			}
		#endif

	} // namespace Allocator

} // namespace EA


