///////////////////////////////////////////////////////////////////////////////
// Copyright (c) Electronic Arts Inc. All rights reserved.
///////////////////////////////////////////////////////////////////////////////

#include <EACallstack/internal/Config.h>

EA_DISABLE_ALL_VC_WARNINGS();

#include <EACallstack/EACallstack.h>
#include <EACallstack/Context.h>

#include <eathread/eathread.h>

#include <Windows.h>
#include <DbgHelp.h>
#include <stdio.h>

EA_RESTORE_ALL_VC_WARNINGS();

EA_DISABLE_VC_WARNING(4740); // flow in or out of inline asm code suppresses global optimization

#if defined(EA_COMPILER_MSVC)

	#pragma comment(lib, "dbghelp.lib")
	#pragma comment(lib, "psapi.lib")

#endif


namespace EA
{
namespace Callstack
{
namespace Detail
{

typedef BOOL   (__stdcall *STACKWALK)(DWORD, HANDLE, HANDLE, LPSTACKFRAME, LPVOID, PREAD_PROCESS_MEMORY_ROUTINE,
									  PFUNCTION_TABLE_ACCESS_ROUTINE, PGET_MODULE_BASE_ROUTINE, PTRANSLATE_ADDRESS_ROUTINE);
typedef LPVOID (__stdcall *SYMFUNCTIONTABLEACCESS)(HANDLE, DWORD);
typedef DWORD  (__stdcall *SYMGETMODULEBASE)(HANDLE, DWORD);

struct Win32DbgHelp
{
	HMODULE                mhDbgHelp = nullptr;
	STACKWALK              mpStackWalk = nullptr;
	SYMFUNCTIONTABLEACCESS mpSymFunctionTableAccess = nullptr;
	SYMGETMODULEBASE       mpSymGetModuleBase = nullptr;
	bool                   mbInitialized = false;

	Win32DbgHelp()
	{
		mhDbgHelp = LoadLibraryA("DbgHelp.dll");

		if (mhDbgHelp)
		{
			*(reinterpret_cast<void**>(&mpStackWalk)) = reinterpret_cast<void*>(GetProcAddress(mhDbgHelp, "StackWalk"));
			*(reinterpret_cast<void**>(&mpSymFunctionTableAccess)) = reinterpret_cast<void*>(GetProcAddress(mhDbgHelp,
																											"SymFunctionTableAccess"));
			*(reinterpret_cast<void**>(&mpSymGetModuleBase)) = reinterpret_cast<void*>(GetProcAddress(mhDbgHelp, "SymGetModuleBase"));

			if (mpStackWalk && mpSymFunctionTableAccess && mpSymGetModuleBase)
			{
				mbInitialized = true;
			}
		}
	}

	~Win32DbgHelp()
	{
		if (mhDbgHelp)
		{
			FreeLibrary(mhDbgHelp);

			mhDbgHelp = nullptr;
			mpStackWalk = nullptr;
			mpSymFunctionTableAccess = nullptr;
			mpSymGetModuleBase = nullptr;

			mbInitialized = false;
		}
	}

	bool IsInitialized() const
	{
		return mbInitialized;
	}

	static const Win32DbgHelp& GetWin32DbgHelp()
	{
		static Win32DbgHelp sWin32DbgHelp{};
		return sWin32DbgHelp;
	}
};

} // namespace Detail
} // namespace Callstack
} // namespace EA


namespace EA
{
namespace Callstack
{


///////////////////////////////////////////////////////////////////////////////
// InitCallstack
//
EACALLSTACK_API void InitCallstack()
{
	(void)Detail::Win32DbgHelp::GetWin32DbgHelp();
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
	if (!Detail::Win32DbgHelp::GetWin32DbgHelp().IsInitialized())
	{
		return 0;
	}

	size_t nEntryIndex = 0;
	const Detail::Win32DbgHelp& win32DbgHelp = Detail::Win32DbgHelp::GetWin32DbgHelp();

	CONTEXT context;
	memset(&context, 0, sizeof(context));
	context.ContextFlags = CONTEXT_CONTROL;

	if (pContext)
	{
		context.Eip = pContext->mEIP;
		context.Esp = pContext->mESP;
		context.Ebp = pContext->mEBP;
	}
	else
	{
		__asm {
			mov context.Ebp, EBP
			mov context.Esp, ESP
		}

		void* IP;
		GetInstructionPointer(IP);
		context.Eip = reinterpret_cast<uint32_t>(IP);
	}

	/**
	 * Initialize the STACKFRAME structure for the first call.
	 * The first call will fail if not.
	 */
	STACKFRAME sf;
	memset(&sf, 0, sizeof(sf));
	sf.AddrPC.Offset     = context.Eip;
	sf.AddrPC.Mode       = AddrModeFlat;
	sf.AddrStack.Offset  = context.Esp;
	sf.AddrStack.Mode    = AddrModeFlat;
	sf.AddrFrame.Offset  = context.Ebp;
	sf.AddrFrame.Mode    = AddrModeFlat;

	const HANDLE hCurrentProcess = GetCurrentProcess();
	const HANDLE hCurrentThread  = GetCurrentThread();

	for (size_t nStackIndex = 0; nEntryIndex < (nReturnAddressArrayCapacity - 1); ++nStackIndex)
	{
		if (!win32DbgHelp.mpStackWalk(IMAGE_FILE_MACHINE_I386, hCurrentProcess, hCurrentThread,
									  &sf, &context, NULL, win32DbgHelp.mpSymFunctionTableAccess,
									  win32DbgHelp.mpSymGetModuleBase, NULL))
		{
			break;
		}

		/* Basic sanity check to make sure the frame is OK. Bail if not. */
		if (sf.AddrFrame.Offset == 0)
		{
			break;
		}

		/**
		 * If using the current execution context, then we ignore the first stack frame
		 * because it is the one that is our stack walk function itself, i.e. GetCallstack().
		 */
		if (pContext || (nStackIndex > 0))
		{
			pReturnAddressArray[nEntryIndex++] = ((void*)(uintptr_t)sf.AddrPC.Offset);
		}
	}

	pReturnAddressArray[nEntryIndex] = nullptr;
	return nEntryIndex;
}


///////////////////////////////////////////////////////////////////////////////
// GetCallstackContext
//
// The threadId is the same thing as the Windows' HANDLE GetCurrentThread() function
// and not the same thing as Windows' GetCurrentThreadId function. See the
// GetCallstackContextSysThreadId for the latter.
//
EACALLSTACK_API EA_NO_INLINE bool GetCallstackContext(CallstackContext& context, intptr_t threadId)
{
	if ((threadId == static_cast<intptr_t>(0)) ||
		(threadId == (intptr_t)EA::Thread::kThreadIdInvalid) ||
		(threadId == (intptr_t)EA::Thread::kThreadIdCurrent))
	{
		threadId = (intptr_t)::GetCurrentThread(); // GetCurrentThread returns a thread 'pseudohandle' and not a real thread handle.
	}

	const DWORD sysThreadId = ::GetThreadId(((HANDLE)threadId));

	return GetCallstackContextSysThreadId(context, sysThreadId);
}

EA_DISABLE_VC_WARNING(4701); // potentially uninitialized local variable

///////////////////////////////////////////////////////////////////////////////
// GetCallstackContextSysThreadId
//
// A sysThreadId is a Microsoft DWORD thread id, which can be obtained from
// the currently running thread via GetCurrentThreadId. It can be obtained from
// a Microsoft thread HANDLE via EA::Callstack::GetThreadIdFromThreadHandle();
// A DWORD thread id can be converted to a thread HANDLE via the Microsoft OpenThread
// system function.
//
EACALLSTACK_API EA_NO_INLINE bool GetCallstackContextSysThreadId(CallstackContext& context, intptr_t sysThreadId)
{
	const DWORD sysThreadIdCurrent = ::GetCurrentThreadId();
	CONTEXT     win32CONTEXT;

	if (sysThreadIdCurrent == (DWORD)sysThreadId)
	{
		NT_TIB*     pTib;

		__asm {
			mov win32CONTEXT.Ebp, EBP
			mov win32CONTEXT.Esp, ESP
		}

		void* IP;
		GetInstructionPointer(IP);
		win32CONTEXT.Eip = reinterpret_cast<uint32_t>(IP);

		/**
		 * Offset 0x18 from the FS segment register gives a pointer to
		 * the thread information block for the current thread
		 * https://en.wikipedia.org/wiki/Win32_Thread_Information_Block
		 */
		__asm {
			mov eax, fs:[18h]
			mov pTib, eax
		}

		context.mStackBase    = ((uintptr_t)pTib->StackBase);
		context.mStackLimit   = ((uintptr_t)pTib->StackLimit);
	}
	else
	{
		/**
		 * In this case we are working with a separate thread, so we suspend it
		 * and read information about it and then resume it.
		 */
		HANDLE threadSuspendHandle = ::OpenThread(THREAD_SUSPEND_RESUME | THREAD_GET_CONTEXT, TRUE, static_cast<DWORD>(sysThreadId));
		BOOL result = false;

		if (threadSuspendHandle)
		{
			if (::SuspendThread(threadSuspendHandle) != ((DWORD)-1))
			{
				win32CONTEXT.ContextFlags = CONTEXT_CONTROL | CONTEXT_INTEGER;
				result = ::GetThreadContext(threadSuspendHandle, &win32CONTEXT);
				::ResumeThread(threadSuspendHandle);
			}

			::CloseHandle(threadSuspendHandle);
		}

		if (!result)
		{
			win32CONTEXT.Ebp = 0;
			win32CONTEXT.Esp = 0;
			win32CONTEXT.Eip = 0;
		}
	}

	context.mEBP = ((uint32_t)win32CONTEXT.Ebp);
	context.mESP = ((uint32_t)win32CONTEXT.Esp);
	context.mEIP = ((uint32_t)win32CONTEXT.Eip);
	context.mStackPointer = ((uintptr_t)win32CONTEXT.Esp);

	return (context.mEIP != 0);
}

EA_RESTORE_VC_WARNING();

} // namespace Callstack
} // namespace EA

EA_RESTORE_VC_WARNING();
