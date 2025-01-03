///////////////////////////////////////////////////////////////////////////////
// Copyright (c) Electronic Arts Inc. All rights reserved.
///////////////////////////////////////////////////////////////////////////////

///////////////////////////////////////////////////////////////////////////////
// This file implements a manual callstack reader for the x86 platform.
// It is an alternative to EACallstack_Win32.cpp, which implements
// callstack reading by standard Microsoft debug functions. This file
// on the other hand avoids the use of Microsoft calls and instead
// directly walks the callstack via reading the registers and x86
// instructions. This file can also be used to read x86 callstacks on
// non-Microsoft x86 platforms such as Linux.
///////////////////////////////////////////////////////////////////////////////


#include <eathread/eathread_callstack.h>
#include <eathread/eathread_callstack_context.h>
#include <eathread/eathread_storage.h>
#include <string.h>

namespace EA
{
namespace Thread
{

// To do: Remove the usage of sStackBase for the platforms that it's not needed,
// as can be seen from the logic below. For example Linux in general doesn't need it.
EA::Thread::ThreadLocalStorage sStackBase;

///////////////////////////////////////////////////////////////////////////////
// SetStackBase
//
EATHREADLIB_API void SetStackBase(void* pStackBase)
{
	if(pStackBase)
		sStackBase.SetValue(pStackBase);
	else
	{
		#if defined(EA_COMPILER_MSVC) && defined(EA_PROCESSOR_X86)
			__asm mov pStackBase, ESP
		#elif defined(EA_COMPILER_MSVC) && defined(EA_PROCESSOR_X86_64)
			pStackBase = _AddressOfReturnAddress();   // Need to #include <intrin.h> for this.
		#elif (defined(EA_COMPILER_GNUC) && defined(EA_PROCESSOR_X86)) || (defined(EA_COMPILER_CLANG) && defined(EA_PROCESSOR_X86))
			asm volatile("mov %%esp, %0" : "=g"(pStackBase));
		#elif (defined(EA_COMPILER_GNUC) && defined(EA_PROCESSOR_X86_64)) || (defined(EA_COMPILER_CLANG) && defined(EA_PROCESSOR_X86_64))
			asm volatile("mov %%rsp, %0" : "=g"(pStackBase));
		#else
			#error Unsupported compiler/processor combination.
		#endif

		if(pStackBase)
			SetStackBase(pStackBase);
		// Else failure; do nothing.
	}
}


///////////////////////////////////////////////////////////////////////////////
// GetStackBase
//
EATHREADLIB_API void* GetStackBase()
{
	#if defined(EA_PLATFORM_UNIX)
		void* pBase;
		if(GetPthreadStackInfo(&pBase, NULL))
			return pBase;
	#endif

	// Else we require the user to have set this previously, usually via a call
	// to SetStackBase() in the start function of this currently executing
	// thread (or main for the main thread).
	return sStackBase.GetValue();
}


///////////////////////////////////////////////////////////////////////////////
// GetStackLimit
//
EATHREADLIB_API void* GetStackLimit()
{
	#if defined(EA_PLATFORM_UNIX)
		void* pLimit;
		if(GetPthreadStackInfo(NULL, &pLimit))
			return pLimit;
	#endif

	// We call strcat in order to create a stack frame here and to avoid a
	// compiler warning about returning a local variable in the case that we
	// simply returned &stackLocation itself.
	char stackLocation = 0;
	char* pStack = strcat(&stackLocation, "");
	return (void*)((uintptr_t)pStack & ~4095); // Round down to nearest page, as the stack grows downward.
}


} // namespace Callstack
} // namespace EA
