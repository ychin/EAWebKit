/////////////////////////////////////////////////////////////////////////////
// Copyright (c) Electronic Arts Inc. All rights reserved.
/////////////////////////////////////////////////////////////////////////////

#include <EACallstack/internal/Config.h>
#include <EACallstack/Context.h>

#include <string.h>


/**
 * NOTE(mwinkler): backtrace() requires no linking of external libraries on linux as it is provided by glibc
 *                 backtrace() on other platforms such as FreeBSD require libexecinfo
 *                 backtrace() on Linux/glibc delegates to GCCs runtime library, libgcc_s.so.1, which conforms to the
 *                 Linux Standard Base Specification and provides the generic functions for Unwinding, like _Unwind_Backtrace
 */

#include <execinfo.h>


namespace EA
{
namespace Callstack
{
namespace Detail
{

EA_DISABLE_CLANG_WARNING(-Wunused-function);

///////////////////////////////////////////////////////////////////////////////
// GetCallstackExecinfoBacktrace
//
// Local Backtrace of the thread calling this function only
EA_NO_INLINE static size_t GetCallstackExecinfoBacktrace(void** pReturnAddressArray, size_t nReturnAddressArrayCapacity, size_t nSkip = 0)
{
	/**
	 * The backtrace(void** addrlist, int len) function places into the array pointed by addrlist the
	 * array of the values of the program counter for each frame called up to
	 * len frames. The number of frames found (which can be fewer than len) is
	 * returned.
	 */

	size_t framesFound = 0;

#if defined(EA_PLATFORM_FREEBSD)

	framesFound = static_cast<size_t>(backtrace(pReturnAddressArray, nReturnAddressArrayCapacity));

#else

	framesFound = static_cast<size_t>(backtrace(pReturnAddressArray, static_cast<int>(nReturnAddressArrayCapacity)));

#endif

	if (framesFound > 0)
	{
		++nSkip; // Ensure we skip ourself, GetCallstackExecinfoBacktrace()
		framesFound = (nSkip > framesFound) ? 0 : (framesFound - nSkip);
		if (framesFound > 0)
		{
			memmove(pReturnAddressArray, pReturnAddressArray + nSkip, framesFound * sizeof(void*));
		}
	}

	return framesFound;
}

EA_RESTORE_CLANG_WARNING();

} // namespace Detail
} // namespace Callstack
} // namespace EA
