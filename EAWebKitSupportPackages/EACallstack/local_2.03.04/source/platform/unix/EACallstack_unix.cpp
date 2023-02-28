/////////////////////////////////////////////////////////////////////////////
// Copyright (c) Electronic Arts Inc. All rights reserved.
/////////////////////////////////////////////////////////////////////////////

#include <EACallstack/internal/Config.h>
#include <EACallstack/EACallstack.h>
#include <EACallstack/Context.h>

#include <EAStdC/EAString.h>


/**
 * Include Various Implementations that we can use under unix
 */
#if EACALLSTACK_GLIBC_BACKTRACE_AVAILABLE

	#include "platform/unix/EACallstack_execinfo.cpp"

#elif defined(EA_PLATFORM_ANDROID) || defined(EA_PLATFORM_LINUX) || defined(EA_PLATFORM_FREEBSD)

	#include "platform/unix/EACallstack_unwind.cpp"

	#define EACALLSTACK_UNIX_UNWIND 1

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
#if defined(EACALLSTACK_UNIX_UNWIND)

	Detail::InitUnwind();

#endif
}


///////////////////////////////////////////////////////////////////////////////
// ShutdownCallstack
//
EACALLSTACK_API void ShutdownCallstack()
{
#if defined(EACALLSTACK_UNIX_UNWIND)

	Detail::ShutdownUnwind();

#endif
}


///////////////////////////////////////////////////////////////////////////////
// GetCallstack
//
EACALLSTACK_API EA_NO_INLINE size_t GetCallstack(void** pReturnAddressArray, size_t nReturnAddressArrayCapacity, const CallstackContext* pContext)
{
#if defined(EACALLSTACK_FRAME_POINTERS_ENABLED)

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

#elif defined(EACALLSTACK_UNIX_UNWIND)

	size_t ret = 0;

	if (pContext == nullptr)
	{
		ret = Detail::GetCallstackUnwind(pReturnAddressArray, nReturnAddressArrayCapacity - 1, 1);
	}

	pReturnAddressArray[ret] = nullptr;
	return ret;

#elif EACALLSTACK_GLIBC_BACKTRACE_AVAILABLE

	size_t ret = 0;

	if (pContext == nullptr)
	{
		ret = Detail::GetCallstackExecinfoBacktrace(pReturnAddressArray, nReturnAddressArrayCapacity - 1, 1);
	}

	pReturnAddressArray[ret] = nullptr;
	return ret;

#else

	EA_UNUSED(pReturnAddressArray);
	EA_UNUSED(nReturnAddressArrayCapacity);
	EA_UNUSED(pContext);

	return 0;

#endif
}


///////////////////////////////////////////////////////////////////////////////
// GetCallstackContext
//
EACALLSTACK_API EA_NO_INLINE bool GetCallstackContext(CallstackContext&, intptr_t)
{
	return false;
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
