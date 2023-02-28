/////////////////////////////////////////////////////////////////////////////
// Copyright (c) Electronic Arts Inc. All rights reserved.
/////////////////////////////////////////////////////////////////////////////

#ifndef EACALLSTACK_EACALLSTACK_H
#define EACALLSTACK_EACALLSTACK_H

#include <eathread/eathread.h>
#include <eathread/eathread_callstack.h>

#include <EACallstack/internal/Config.h>
#include <EACallstack/Context.h>
#include <EACallstack/EAModule.h>

#if defined(EA_PRAGMA_ONCE_SUPPORTED)
	#pragma once
#endif // EA_PRAGMA_ONCE_SUPPORTED


namespace EA
{
namespace Callstack
{

	///////////////////////////////////////////////////////////////////////////////
	/// InitCallstack
	///
	/// Allows the user to explicitly initialize the callstack mechanism.
	/// This must be called before using any Callstack Functions.
	/// Only the first call to InitCallstack will have effect. Calls to
	/// InitCallstack must be matched by calls to ShutdownCallstack.
	///
	/// Safety: MT-Safe
	EACALLSTACK_API void InitCallstack();


	///////////////////////////////////////////////////////////////////////////////
	/// ShutdownCallstack
	///
	/// Allows the user to explicitly shutdown the callstack mechanism.
	/// Calls to InitCallstack must be matched by calls to ShutdownCallstack.
	/// The last call to ShutdownCallstack will shutdown and free the callstack mechanism.
	///
	/// Safety: MT-Safe
	EACALLSTACK_API void ShutdownCallstack();


	///////////////////////////////////////////////////////////////////////////////
	/// GetCallstack
	///
	/// Gets the addresses of the calling instructions of a callstack.
	/// The return value is the number of entries written to the callstack array.
	/// The item at callstack[0] is from the function calling the GetCallstack function.
	/// For most platforms the addresses reported are the addresses of the instruction
	/// that will next be executed upon returning from the function it is calling.
	///
	/// @param pContext: !nullptr = that execution context is used
	///                   nullptr = the current execution context is used
	///                   If passing a context of the current thread and the function returns 0, try passing nullptr next
	///
	/// @param pReturnAddressArray: Return addresses for each corresponding stack frame. The array will be NULL terminated.
	///
	/// @param nReturnAddressArrayCapacity: The number of entries in the pReturnAddressArray buffer, must be at least one for
    ///                                     the terminating NULL entry
	///
	/// @return: Returns the number of entries written into the buffer. A return value of 0 means nothing was written to the buffer
	///          so the function was unable to get a Callstack
	///
	/// Safety: MT-Safe
	EACALLSTACK_API EA_NO_INLINE size_t GetCallstack(void** pReturnAddressArray, size_t nReturnAddressArrayCapacity,
													 const CallstackContext* pContext = nullptr);


	///////////////////////////////////////////////////////////////////////////////
	/// GetCallstackContext
	///
	/// Gets the CallstackContext associated with the given thread.
	/// The thread must be in a non-running state.
	/// If the threadID is EAThread::kThreadIdInvalid, the current thread context is retrieved.
	/// However, it's of little use to get the context of the current thread, since upon return
	/// from the GetCallstackContext the data will not apply to the current thread any more;
	/// thus this information is probably useful only for diagnostic purposes.
	/// The threadId parameter is the same type as an EAThread ThreadId. It is important to
	/// note that an EAThread ThreadId under Microsoft platforms is a thread handle and not what
	/// Microsoft calls a thread id. This is by design as Microsoft thread ids are second class
	/// citizens and likely wouldn't exist if it not were for quirks in the Windows API evolution.
	///
	/// Note that threadId is the same as EA::Callstack::ThreadId and is a Microsoft thread HANDLE.
	/// This is not the same as a Microsoft DWORD thread id which is the same as EA::Callstack::SysThreadId.
	///
	/// EACallstack has a general struct for each CPU type called Context, defined in EACallstack/Context.h.
	/// The Context struct contains the entire CPU register context information. In order to walk a thread's
	/// callstack, you really need only two or three of the register values from the Context. So there is a
	/// mini struct called CallstackContext which is just those registers needed to read a thread's callstack.
	///
	/// @param thradId: The thread to get the context from
	///
	/// @param context: The output register context of the thread, threadId
	///
	/// @return: Returns true if the context was able to be retrieved, false otherwise.
	///
	/// Safety: MT-Safe
	#if EA_USE_CPP11_CONCURRENCY
		EACALLSTACK_API EA_NO_INLINE bool GetCallstackContext(CallstackContext& context, EA::Thread::ThreadId threadId);
	#else
		EACALLSTACK_API EA_NO_INLINE bool GetCallstackContext(CallstackContext& context, intptr_t threadId = 0);
	#endif


	///////////////////////////////////////////////////////////////////////////////
	/// GetCallstackContext
	///
	/// Gets the CallstackContext from a full Context struct. Note that the Context struct
	/// defines the entire machine context, whereas the CallstackContext is a tiny struct
	/// with just a couple integer members and is all that's needed to describe a callstack.
	///
	/// @param context: The output register context
	///
	/// @param pContext: The full context to generate the output context from
	///
	/// Safety: MT-Safe
	EACALLSTACK_API void GetCallstackContext(CallstackContext& context, const Context* pContext);


	///////////////////////////////////////////////////////////////////////////////
	/// GetCallstackContextSysThreadId
	///
	/// This is the same as GetCallstackContext, except it uses what EAThread calls SysThreadId.
	/// On Microsoft platforms a SysThreadId is a "thread id" whereas ThreadId is "thread handle."
	/// On non-Microsoft platforms a SysThreadId is defined to be the same as ThreadId and is often
	/// just an integer or opaque identifier (e.g. pthread).
	/// This function exists because it may be more convenient to work with SysThreadIds in some cases.
	/// You can convert from a ThreadId (Microsoft thread handle) to a SysThreadId (Microsoft thread id)
	/// with the GetThreadIdFromThreadHandle function.
	///
	/// Safety: MT-Safe
	EACALLSTACK_API EA_NO_INLINE bool GetCallstackContextSysThreadId(CallstackContext& context, intptr_t sysThreadId = 0);


	///////////////////////////////////////////////////////////////////////////////
	/// GetInstructionPointer
	///
	/// Returns the current instruction pointer (a.k.a. program counter).
	///
	/// These functions return NULL/0 on error or if getting the value is not possible
	///
	/// Example usage:
	///    void* pInstruction;
	///    GetInstructionPointer(pInstruction);
	///
	/// Safety: MT-Safe
	#if defined(EA_COMPILER_MSVC)

		EACALLSTACK_API EA_NO_INLINE void GetInstructionPointer(void*& pInstruction);

	#elif defined(EA_COMPILER_GNUC) || defined(EA_COMPILER_CLANG)

		EACALLSTACK_API EA_NO_INLINE void GetInstructionPointer(void*& pInstruction);

	#elif defined(EA_COMPILER_ARM) || defined(EA_COMPILER_RVCT)

		EA_FORCE_INLINE void GetInstructionPointer(void*& p)
		{
			p = static_cast<void*>(__current_pc());
		}

	#else

		#error "EA::Callstack::GetInstructionPointer() Not Supported On The Given Platform/Compiler!!!"

	#endif

} // namespace Callstack
} // namespace EA

#endif // EACALLSTACK_EACALLSTACK_H
