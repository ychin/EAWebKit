///////////////////////////////////////////////////////////////////////////////
// Copyright (c) Electronic Arts Inc. All rights reserved.
///////////////////////////////////////////////////////////////////////////////

#include <EACallstack/internal/Config.h>
#include <EACallstack/EACallstack.h>
#include <EACallstack/Context.h>

#include <eathread/eathread.h>

#include <string.h>
#include <sys/signal.h>
#include <machine/signal.h>
#include <sdk_version.h>
#include <unistd.h>


/**
 * NOTE:
 *
 * Use of unwind.h is disabled on PS4 due to syscall hangs in the kernel
 * experienced by Frostbite when overloadiing user_malloc to generate a
 * callstack. In addition, Sony recommends the use of the clang builtins, __builtin_frame_address
 * / __builtin_return_address over _Unwind_Backtrace as it is more performant
 * due to the frame pointers being included by default in all builds on Kettle.
 */
#if !defined(EACALLSTACK_SONY_NO_UNWIND)

	#define EACALLSTACK_SONY_NO_UNWIND 1

#endif


namespace EA
{
namespace Callstack
{


///////////////////////////////////////////////////////////////////////////////
// InitCallstack
//
EACALLSTACK_API void InitCallstack()
{
}


///////////////////////////////////////////////////////////////////////////////
// ShutdownCallstack
//
EACALLSTACK_API void ShutdownCallstack()
{
}


///////////////////////////////////////////////////////////////////////////////
// GetCallstack
//
EACALLSTACK_API EA_NO_INLINE size_t GetCallstack(void** pReturnAddressArray, size_t nReturnAddressArrayCapacity, const CallstackContext* pContext)
{
	/**
	 * On kettle it's not recommended to omit the frame pointer
	 * it could have been omitted. From Sony Docs:
	 * "[omit frame pointer] will inhibit unwinding and ... the option may also increase code size since the
	 * encoding for stack-based addressing is often 1 byte longer then RBP-based (frame pointer) addressing.
	 * With PlayStation®4 Clang, frame pointer omission may not lead to improved performance.
	 * Performance analysis and code  profiling are recommended before using this option"
	 */
	if (pContext == nullptr)
	{
		CallstackContext context;
		context.mRBP = reinterpret_cast<uint64_t>(__builtin_frame_address(0));

		size_t ret = Detail::GetCallstackx64FramePtrWalk(pReturnAddressArray, nReturnAddressArrayCapacity - 1, &context);

		pReturnAddressArray[ret] = nullptr;
		return ret;
	}
	else
	{
		pReturnAddressArray[0] = reinterpret_cast<void*>(pContext->mRIP);

		size_t ret = Detail::GetCallstackx64FramePtrWalk(pReturnAddressArray + 1, nReturnAddressArrayCapacity - 2, pContext);
		++ret;

		pReturnAddressArray[ret] = nullptr;
		return ret;
	}
}


///////////////////////////////////////////////////////////////////////////////
// GetCallstackContext
//
EACALLSTACK_API EA_NO_INLINE bool GetCallstackContext(CallstackContext& context, intptr_t threadId)
{
	ScePthread self         = scePthreadSelf();
	ScePthread pthread_Id   = (ScePthread)threadId; // Requires that ScePthread is a pointer or integral type.

	if (scePthreadEqual(pthread_Id, self))
	{
		void* IP;
		GetInstructionPointer(IP);
		context.mRIP = (uint64_t)IP;
		context.mRSP = (uint64_t)__builtin_frame_address(1);
		context.mRBP = 0;
	}
	else
	{
		/* There is currently no way to do this. */
		memset(&context, 0, sizeof(context));
		return false;
	}

	return true;
}


///////////////////////////////////////////////////////////////////////////////
// GetCallstackContextSysThreadId
//
EACALLSTACK_API EA_NO_INLINE bool GetCallstackContextSysThreadId(CallstackContext& context, intptr_t sysThreadId)
{
	return GetCallstackContext(context, sysThreadId);
}


} // namespace Callstack
} // namespace EA
