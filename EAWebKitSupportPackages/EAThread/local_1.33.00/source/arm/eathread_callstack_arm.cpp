///////////////////////////////////////////////////////////////////////////////
// Copyright (c) Electronic Arts Inc. All rights reserved.
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
// as can be seen from the logic below. For example iPhone probably doesn't need it.
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
		// Can't call GetStackLimit() because doing so would disturb the stack. 
		// As of this writing, we don't have an EAGetStackTop macro which could do this.
		// So we implement it inline here.
		#if   defined(__ARMCC_VERSION)
			pStackBase = (void*)__current_sp();
		#elif defined(__GNUC__) || defined(EA_COMPILER_CLANG)
			pStackBase = __builtin_frame_address(0);
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

	#if   defined(__ARMCC_VERSION)
		void* pStack = (void*)__current_sp();
	#elif defined(__GNUC__) || defined(EA_COMPILER_CLANG)
		void* pStack = __builtin_frame_address(0);
	#else
		void* pStack = NULL;  // TODO:  determine fix.
		pStack = &pStack;
	#endif

	return (void*)((uintptr_t)pStack & ~4095); // Round down to nearest page, as the stack grows downward.
}


} // namespace Thread
} // namespace EA





