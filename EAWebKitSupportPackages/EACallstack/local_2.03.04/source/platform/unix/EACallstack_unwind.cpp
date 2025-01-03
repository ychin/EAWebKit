/////////////////////////////////////////////////////////////////////////////
// Copyright (c) Electronic Arts Inc. All rights reserved.
/////////////////////////////////////////////////////////////////////////////

#include <EACallstack/internal/Config.h>
#include <EACallstack/internal/Intrin.h>
#include <EACallstack/Context.h>

#include <unwind.h>

#if defined(EA_PLATFORM_LINUX) && !defined(EA_PLATFORM_ANDROID)

#if !defined(_GNU_SOURCE)

	#define _GNU_SOURCE

#endif

	#include <dlfcn.h>

#endif

/**
 * NOTE(mwinkler): On Unix platforms DWARF debug CFI information was reused to be able to implement exception handling in C++.
 *                 .debug_frame and .eh_frame sections in the .elf both contain CFI information for stack unwinding,
 *                 one contains information for all functions where as the other only contains information for functions
 *                 that can throw/catch exceptions and if exceptions are enabled.
 *                 Linux Standard Base, LSB, Specification defines the interface for an Unwinding Implementation that a runtime
 *                 must expose to provide a platform independant way to implement exceptions and stack unwinding. This interface
 *                 became the defacto standard for Unwinding on unix based systems. There are two main unwinding libraries that are
 *                 by the two main compilers, clang and gcc. libgcc_s is gcc's runtime library that provides the unwind routines
 *                 and clang's libunwind is their implementation.
 *
 *                 http://refspecs.linuxbase.org/LSB_4.1.0/LSB-Core-generic/LSB-Core-generic/libgcc-s.html
 */

namespace EA
{
namespace Callstack
{
namespace Detail
{

EA_DISABLE_CLANG_WARNING(-Wunused-function);

#if defined(EA_PLATFORM_LINUX) && !defined(EA_PLATFORM_ANDROID)

	/**
	 * NOTE(mwinkler): libgcc_s.so.1 may not be loaded because exceptions are disabled, as one example.
	 *                 Just manually load the .so to be safe that we indeed have these functions in all cases.
	 *                 Clang on platforms where GCC is the prominent compiler, Linux, default to linking against libgcc as the compiler runtime
	 *                 library, and --rtlib=compiler-rt must be passed to use Clang's own runtime library.
	 *                 Loading two different runtime libraries is perfectly fine as well
	 *                 and on Linux we can assume a sane distro has GCC by default.
	 */

	struct GCCRuntimeLibrary
	{
		typedef _Unwind_Reason_Code (*UnwindBacktraceFuncPtr)(_Unwind_Trace_Fn, void*);
		typedef _Unwind_Ptr (*UnwindGetIPFuncPtr)(struct _Unwind_Context*);

		UnwindBacktraceFuncPtr unwind_backtrace_func = nullptr;
		UnwindGetIPFuncPtr unwind_getip_func = nullptr;
		void* libgcc_so = nullptr;
		bool initialized = false;

		GCCRuntimeLibrary()
		{
			libgcc_so = dlopen("libgcc_s.so.1", RTLD_NOW);
			if (libgcc_so)
			{
				*(reinterpret_cast<void**>(&unwind_backtrace_func)) = dlsym(libgcc_so, "_Unwind_Backtrace");
				*(reinterpret_cast<void**>(&unwind_getip_func)) = dlsym(libgcc_so, "_Unwind_GetIP");

				if (unwind_backtrace_func && unwind_getip_func)
				{
					initialized = true;
				}
			}
		}

		~GCCRuntimeLibrary()
		{
			if (libgcc_so)
			{
				dlclose(libgcc_so);

				libgcc_so = nullptr;
				unwind_backtrace_func = nullptr;
				unwind_getip_func = nullptr;

				initialized = false;
			}
		}

		bool IsInitialized() const
		{
			return initialized;
		}

		static const GCCRuntimeLibrary& GetGCCRuntimeLibrary()
		{
			static GCCRuntimeLibrary sGCCRuntimeLib{};
			return sGCCRuntimeLib;
		}
	};

	#define InternalUnwindBacktrace (GCCRuntimeLibrary::GetGCCRuntimeLibrary().unwind_backtrace_func)
	#define InternalUnwindGetIP (GCCRuntimeLibrary::GetGCCRuntimeLibrary().unwind_getip_func)
	#define InternalUnwindInitialized() (GCCRuntimeLibrary::GetGCCRuntimeLibrary().IsInitialized())

#else

	#define InternalUnwindBacktrace _Unwind_Backtrace
	#define InternalUnwindGetIP _Unwind_GetIP
	#define InternalUnwindInitialized() (true)

#endif

static void InitUnwind()
{
#if defined(EA_PLATFORM_LINUX) && !defined(EA_PLATFORM_ANDROID)

	(void)GCCRuntimeLibrary::GetGCCRuntimeLibrary();

#endif
}

static void ShutdownUnwind()
{
}

struct CallstackState
{
	void** arrayCurrent;
	void** arrayEnd;
	size_t skipFrames;
};

/* Any return value other than _URC_NO_REASON, stops the unwinding */
static _Unwind_Reason_Code UnwindFrameCallback(struct _Unwind_Context* context, void* callbackArg)
{
	CallstackState* state = static_cast<CallstackState*>(callbackArg);
	if (state->skipFrames == 0)
	{
		uintptr_t pc = reinterpret_cast<uintptr_t>(InternalUnwindGetIP(context));

#if defined(EA_PROCESSOR_ARM32)

		pc = Detail::RemoveARM32ThumbBit(pc);

#endif

		if (pc)
		{
			if (state->arrayCurrent == state->arrayEnd)
			{
				return _URC_END_OF_STACK;
			}
			else
			{
				*(state->arrayCurrent++) = reinterpret_cast<void*>(pc);
			}
		}
		else
		{
			return _URC_END_OF_STACK;
		}
	}
	else
	{
		--state->skipFrames;
	}

	return _URC_NO_REASON;
}

///////////////////////////////////////////////////////////////////////////////
// GetCallstackUnwind
//
// Get a backtrace of the local thread
EA_NO_INLINE static size_t GetCallstackUnwind(void** pReturnAddressArray, size_t nReturnAddressArrayCapacity, size_t nSkip = 0)
{
	if (!InternalUnwindInitialized())
	{
		return 0;
	}

	size_t numFrames = 0;

	++nSkip; /* Ensure we skip ourself, GetCallstackUnwind() */

	CallstackState state{ pReturnAddressArray, pReturnAddressArray + nReturnAddressArrayCapacity, nSkip };

	InternalUnwindBacktrace(UnwindFrameCallback, static_cast<void*>(&state));

	numFrames = static_cast<size_t>(state.arrayCurrent - pReturnAddressArray);

	return numFrames;
}

EA_RESTORE_CLANG_WARNING();

} // namespace Detail
} // namespace Callstack
} // namespace EA
