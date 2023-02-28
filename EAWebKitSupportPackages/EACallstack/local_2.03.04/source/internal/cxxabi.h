//===--------------------------- cxxabi.h ---------------------------------===//
//
//                     The LLVM Compiler Infrastructure
//
// This file is dual licensed under the MIT and the University of Illinois Open
// Source Licenses. See LICENSE.TXT for details.
//
//===----------------------------------------------------------------------===//

#ifndef __CXXABI_H
#define __CXXABI_H 

/*
 * This header provides the interface to the C++ ABI as defined at:
 *       http://www.codesourcery.com/cxx-abi/
 */

#include <stddef.h>
#if !defined(_MSC_VER) || (_MSC_VER >= 1600)
	#include <stdint.h>
#else
	#include <EABase/eabase.h>
#endif

#define _LIBCPPABI_VERSION 1001

#if defined(__GNUC__) || defined(__clang__)
	#define LIBCXXABI_NORETURN  __attribute__((noreturn))
#else
	#define LIBCXXABI_NORETURN
#endif


#ifdef __cplusplus

namespace std {
	class type_info; // forward declaration
}


// runtime routines use C calling conventions, but are in __cxxabiv1 namespace.
// For portability, we name it llvm_cxxabiv1 within our libraries, in order to avoid collisions.
namespace llvm_cxxabiv1 {  
  extern "C"  {
		// 3.4 Demangler API
		extern char* llvm_cxa_demangle(const char* mangled_name, 
									  char*       output_buffer,
									  size_t*     length, 
									  int*        status);
  } // extern "C"
} // namespace llvm_cxxabiv1

#endif // __cplusplus

#endif // __CXXABI_H 
