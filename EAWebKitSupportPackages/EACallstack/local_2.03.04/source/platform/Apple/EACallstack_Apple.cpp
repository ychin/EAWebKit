///////////////////////////////////////////////////////////////////////////////
// Copyright (c) Electronic Arts Inc. All rights reserved.
///////////////////////////////////////////////////////////////////////////////

#include <EACallstack/internal/Config.h>
#include <EACallstack/internal/Intrin.h>
#include <EACallstack/EACallstack.h>
#include <EACallstack/Context.h>
#include <EACallstack/Apple/EACallstackApple.h>

#include <EAStdC/EAString.h>
#include <EAStdC/EAScanf.h>

#include <EAIO/PathString.h>

#include <stdio.h>
#include <string.h>
#include <pthread.h>
#include <dlfcn.h>

#include <mach/thread_act.h>


#if EACALLSTACK_GLIBC_BACKTRACE_AVAILABLE

	#include "platform/unix/EACallstack_execinfo.cpp"

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


#if defined(EA_PROCESSOR_ARM)

///////////////////////////////////////////////////////////////////////////////
// GetCallstackContext
//
//
EACALLSTACK_API void GetCallstackContext(CallstackContext& context, const Context* pContext)
{
#if defined(EA_PROCESSOR_ARM32)

	context.mFP  = pContext->mGpr[7];   // Apple uses R7 for the frame pointer in both ARM and Thumb CPU modes.
	context.mSP  = pContext->mGpr[13];
	context.mLR  = pContext->mGpr[14];
	context.mPC  = pContext->mGpr[15];

#elif defined(EA_PROCESSOR_ARM64)

	context.mFP  = pContext->mGpr[29];
	context.mSP  = pContext->mGpr[31];
	context.mLR  = pContext->mGpr[30];
	context.mPC  = pContext->mPC;

#endif
}

#endif


///////////////////////////////////////////////////////////////////////////////
// GetCallstack
//
//
// ARM32/64
//      Apple defines a different ABI than the ARM eabi used by Linux and the ABI used
//      by Microsoft. It implements a predictable stack frame layout using r7/r29 as the
//      frame pointer. Documentation:
//          https://developer.apple.com/library/archive/documentation/Xcode/Conceptual/iPhoneOSABIReference/Articles/ARMv6FunctionCallingConventions.html
//          https://developer.apple.com/library/archive/documentation/Xcode/Conceptual/iPhoneOSABIReference/Articles/ARMv7FunctionCallingConventions.html
//          https://developer.apple.com/library/archive/documentation/Xcode/Conceptual/iPhoneOSABIReference/Articles/ARM64FunctionCallingConventions.html
//
//      Basically, Apple uses ARM32/64 r7/r29 as a frame pointer. So for any function you are
//      executing, r7/r29 + 4/8 is the LR passed to us by the caller and is the PC of
//      the parent. And r7/r29 + 0 is a pointer to the parent's r7/r29.
//
//      The ABI is similar except using the different registers from the different CPU.
//
// On All Apple Support Architectures, Frame Pointers are disabled by default
//
EACALLSTACK_API EA_NO_INLINE size_t GetCallstack(void** pReturnAddressArray, size_t nReturnAddressArrayCapacity, const CallstackContext* pContext)
{
/**
 *  x86/x86_64 Stack Frame Layout
 *
 *  ---------------------------------
 *  |  saved EIP/RIP, pushed by CALL |
 *  |                                |
 *  ---------------------------------
 *  | Caller's Saved RBP             |
 *  |                                |  <- Current RBP register points here
 *  ---------------------------------
 */

/**
 *  ARM32/64 Stack Frame Layout
 *
 *  ---------------------------------
 *  |  saved Link Register(LR)       |
 *  |                                |
 *  ---------------------------------
 *  | Caller's Saved R7              |
 *  |                                |  <- Current R7 register points here
 *  ---------------------------------
 */

#if defined(EA_PROCESSOR_ARM) || defined(EA_PROCESSOR_X86) || defined(EA_PROCESSOR_X86_64)

	struct StackFrame
	{
		StackFrame* mpParentStackFrame;
		void* mpReturnPC;
	};

	StackFrame* pStackFrame = nullptr;
	size_t index = 0;

	if (pContext)
	{
		void* pInstruction = nullptr;

#if defined(EA_PROCESSOR_ARM)

		pStackFrame  = reinterpret_cast<StackFrame*>(pContext->mFP);
		pInstruction = reinterpret_cast<void*>(pContext->mPC);

#elif defined(EA_PROCESSOR_X86_64)

		pStackFrame  = reinterpret_cast<StackFrame*>(pContext->mRBP);
		pInstruction = reinterpret_cast<void*>(pContext->mRIP);

#elif defined(EA_PROCESSOR_X86)

		pStackFrame  = reinterpret_cast<StackFrame*>(pContext->mEBP);
		pInstruction = reinterpret_cast<void*>(pContext->mEIP);

#endif

		if (index < (nReturnAddressArrayCapacity - 1))
		{
			pReturnAddressArray[index++] = pInstruction;
		}
	}
	else
	{
		pStackFrame = static_cast<StackFrame*>(__builtin_frame_address(0));
	}

	for (; (index < (nReturnAddressArrayCapacity - 1)) && pStackFrame && pStackFrame->mpReturnPC && pStackFrame->mpParentStackFrame;)
	{
		void* returnInstruction = pStackFrame->mpReturnPC;

		pStackFrame = pStackFrame->mpParentStackFrame;

#if defined(EA_PROCESSOR_ARM32)

		returnInstruction = Detail::RemoveARM32ThumbBit(returnInstruction);

#endif

		pReturnAddressArray[index++] = returnInstruction;
	}

	pReturnAddressArray[index] = nullptr;
	return index;

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
EACALLSTACK_API EA_NO_INLINE bool GetCallstackContext(CallstackContext& context, intptr_t threadId)
{
	bool threadIsSelf = (threadId == (intptr_t)EA::Thread::kThreadIdInvalid) || // Due to a specification mistake, this function
						(threadId == (intptr_t)EA::Thread::kThreadIdCurrent) || // accepts kThreadInvalid to mean current.
						(threadId == (intptr_t)pthread_self());

	if (threadIsSelf)
	{
		#if defined(EA_PROCESSOR_ARM32)

			void* p;
			GetInstructionPointer(p);
			context.mPC = (uint32_t)p;
			context.mFP = (uint32_t)__builtin_frame_address(0);  // This data isn't exactly right. We want to return the registers as they
			context.mSP = (uint32_t)__builtin_frame_address(0);  // are for the caller, not for us. Without doing that we end up reporting
			context.mLR = (uint32_t)__builtin_return_address(0); // an extra frame (this one) on the top of callstacks.

		#elif defined(EA_PROCESSOR_ARM64)

			void* p;
			GetInstructionPointer(p);
			context.mPC = (uint64_t)p;
			context.mFP = (uint64_t)__builtin_frame_address(0);  // This data isn't exactly right. We want to return the registers as they
			context.mSP = (uint64_t)__builtin_frame_address(0);  // are for the caller, not for us. Without doing that we end up reporting
			context.mLR = (uint64_t)__builtin_return_address(0); // an extra frame (this one) on the top of callstacks.

		#elif defined(EA_PROCESSOR_X86_64)

			context.mRIP = (uint64_t)__builtin_return_address(0);
			context.mRSP = 0;
			context.mRBP = (uint64_t)__builtin_frame_address(0);

		#elif defined(EA_PROCESSOR_X86)

			context.mEIP = (uint32_t)__builtin_return_address(0);
			context.mESP = 0;
			context.mEBP = (uint32_t)__builtin_frame_address(0);

		#else

			return false;

		#endif

		return true;
	}
	else
	{
		// Pause the thread, get its state, unpause it.
		//
		// Question: Is it dangerous to suspend an arbitrary thread? Often such a thing is dangerous
		// because that other thread might for example have some kernel mutex locked that we need.
		// We'll have to see, as it's a great benefit for us to be able to read callstack contexts.
		// Another solution would be to inject a signal handler into the thread and signal it and
		// have the handler read context information, if that can be useful. There's example code
		// on the Internet for that.
		// Some documentation:
		//     http://www.linuxselfhelp.com/gnu/machinfo/html_chapter/mach_7.html

		mach_port_t   thread = pthread_mach_thread_np((pthread_t)threadId); // Convert pthread_t to kernel thread id.
		kern_return_t result = thread_suspend(thread);

		if (result == KERN_SUCCESS)
		{
			#if defined(EA_PROCESSOR_ARM32)

				arm_thread_state_t threadState; memset(&threadState, 0, sizeof(threadState));
				mach_msg_type_number_t stateCount = MACHINE_THREAD_STATE_COUNT;
				result = thread_get_state(thread, MACHINE_THREAD_STATE, (natural_t*)(uintptr_t)&threadState, &stateCount);

				context.mFP = threadState.__r[7]; // Apple uses R7 for the frame pointer in both ARM and Thumb CPU modes.
				context.mPC = threadState.__pc;
				context.mSP = threadState.__sp;
				context.mLR = threadState.__lr;

			#elif defined(EA_PROCESSOR_ARM64)

				__darwin_arm_thread_state64 threadState; memset(&threadState, 0, sizeof(threadState));
				mach_msg_type_number_t stateCount = MACHINE_THREAD_STATE_COUNT;
				result = thread_get_state(thread, MACHINE_THREAD_STATE, (natural_t*)(uintptr_t)&threadState, &stateCount);

				context.mFP = threadState.__fp;
				context.mPC = threadState.__pc;
				context.mSP = threadState.__sp;
				context.mLR = threadState.__lr;

			#elif defined(EA_PROCESSOR_X86_64)

				x86_thread_state_t     threadState; memset(&threadState, 0, sizeof(threadState));
				mach_msg_type_number_t stateCount  = MACHINE_THREAD_STATE_COUNT;
				result = thread_get_state(thread, MACHINE_THREAD_STATE, (natural_t*)(uintptr_t)&threadState, &stateCount);

				context.mRIP = threadState.uts.ts64.__rip;
				context.mRSP = threadState.uts.ts64.__rsp;
				context.mRBP = threadState.uts.ts64.__rbp;

			#elif defined(EA_PROCESSOR_X86)

				x86_thread_state_t     threadState; memset(&threadState, 0, sizeof(threadState));
				mach_msg_type_number_t stateCount  = MACHINE_THREAD_STATE_COUNT;
				result = thread_get_state(thread, MACHINE_THREAD_STATE, (natural_t*)(uintptr_t)&threadState, &stateCount);

				context.mEIP = threadState.uts.ts32.__eip;
				context.mESP = threadState.uts.ts32.__esp;
				context.mEBP = threadState.uts.ts32.__ebp;

			#endif

			thread_resume(thread);

			return (result == KERN_SUCCESS);
		}
	}

	memset(&context, 0, sizeof(context));
	return false;
}


///////////////////////////////////////////////////////////////////////////////
// GetCallstackContextSysThreadId
//
EACALLSTACK_API EA_NO_INLINE bool GetCallstackContextSysThreadId(CallstackContext& context, intptr_t sysThreadId)
{
	pthread_t pthread = pthread_from_mach_thread_np((mach_port_t)sysThreadId);

	return GetCallstackContext(context, (intptr_t)pthread);
}

} // namespace Callstack
} // namespace EA
