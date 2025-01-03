/////////////////////////////////////////////////////////////////////////////
// DemangleGCC.cpp
//
// Copyright (c) 2008, Electronic Arts Inc. All rights reserved.
// Created by Paul Pedriana
/////////////////////////////////////////////////////////////////////////////

/////////////////////////////////////////////////////////////////////////////
// The Standard for GCC C++ name mangling is defined at: 
//     http://www.codesourcery.com/public/cxx-abi/abi.html#mangling
//
/////////////////////////////////////////////////////////////////////////////


#include <EACallstack/internal/DemangleGCC.h>

#if EACALLSTACK_GCC_DEMANGLE_ENABLED

#include <EACallstack/EAAddressRep.h>
#include <EASTL/fixed_substring.h>
#include <EAStdC/EAString.h>
#include <ctype.h>



#if EACALLSTACK_LIBSTDCPP_DEMANGLE_AVAILABLE
	#include <cxxabi.h>         // This is compiler-provided header.
#elif EACALLSTACK_LLVM_DEMANGLE_AVAILABLE
	#include "cxxabi.h"         // This header is local to our build.
#endif




namespace EA
{
namespace Callstack
{
namespace Demangle
{

#if EACALLSTACK_LIBSTDCPP_DEMANGLE_AVAILABLE || EACALLSTACK_LLVM_DEMANGLE_AVAILABLE

	/////////////////////////////////////////////////////////////////////////////////////
	// char* abi::__cxa_demangle(const char* mangled_name, char* output_buffer, size_t* length, int* status);
	//
	//     mangled_name  - A NUL-terminated character string containing the name to be demangled.
	//     output_buffer - A region of memory, allocated with malloc, of *length bytes, into which 
	//                     the demangled name is stored. If output_buffer is not long enough, it is 
	//                     expanded using realloc. output_buffer may instead be NULL; in that case, 
	//                     the demangled name is placed in a region of memory allocated with malloc.
	//     length        - If length is non-NULL, the length of the buffer containing the demangled 
	//                     name is placed in *length.
	//     status        - *status is set to one of the following values:
	//                        0: The demangling operation succeeded.
	//                       -1: A memory allocation failiure occurred.
	//                       -2: mangled_name is not a valid name under the C++ ABI mangling rules.
	//                       -3: One of the arguments is invalid.
	//     Returns: A pointer to the start of the NUL-terminated demangled name, or NULL if the 
	//              demangling fails. The caller is responsible for deallocating this memory using free.
	/////////////////////////////////////////////////////////////////////////////////////

	size_t UnmangleSymbolGCC(const char* pSymbol, char* buffer, size_t bufferCapacity)
	{        
		if(pSymbol)
		{
			// To do: Find a way to avoid using malloc/realloc/free. It's a concern because this
			// function may be called while we are in an exception handler and the system is of
			// an unknown state of reliability. One solution is to allocate a single usually-large-enough
			// block ahead of time and pass that in as the output_buffer/length parameters, and 
			// don't free the return value, as we'll use it again later. We'll still want to find a
			// way to free it before a normal app shutdown though.
			int status;

			#if EACALLSTACK_LIBSTDCPP_DEMANGLE_AVAILABLE
			  char* pNameDemangled = abi::__cxa_demangle(pSymbol, NULL, NULL, &status);
			#else
				// cxa_demangle does not support mangled function names beginning with two underscores (found in iOS/Mac map files)
				if( EA::StdC::Strncmp(pSymbol, "__Z", EA::StdC::Strlen("__Z")) == 0)
				{
					pSymbol++; // remove one underscore
				}

				char* pNameDemangled = llvm_cxxabiv1::llvm_cxa_demangle(pSymbol, NULL, NULL, &status);
			#endif

			if(pNameDemangled && (status == 0))
			{
				EA::StdC::Strlcpy(buffer, pNameDemangled, bufferCapacity);
				free(pNameDemangled);
				return EA::StdC::Strlen(buffer);
			}
			// Fall through
		}

		// In case there is an error or if the symbol appears to already be demangled.
		// Just copy the symbol to the buffer as-is.
		EA::StdC::Strlcpy(buffer, pSymbol, bufferCapacity);
		return EA::StdC::Strlen(buffer);
	}

#else

	size_t UnmangleSymbolGCC(const char* pSymbol, char* buffer, size_t bufferCapacity)
	{
		EA::StdC::Strlcpy(buffer, pSymbol, bufferCapacity);
		return EA::StdC::Strlen(buffer);
	}

#endif // EACALLSTACK_LIBSTDCPP_DEMANGLE_AVAILABLE

} // namespace Demangle

} // namespace Callstack

} // namespace EA


#endif // EACALLSTACK_GCC_DEMANGLE_ENABLED

