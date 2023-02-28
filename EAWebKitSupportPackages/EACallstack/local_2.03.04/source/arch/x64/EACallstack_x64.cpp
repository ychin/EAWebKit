///////////////////////////////////////////////////////////////////////////////
// Copyright (c) Electronic Arts Inc. All rights reserved.
///////////////////////////////////////////////////////////////////////////////

#include <EACallstack/internal/Config.h>
#include <EACallstack/EACallstack.h>
#include <EACallstack/Context.h>
#include <eathread/eathread_storage.h>


namespace EA
{
namespace Callstack
{
namespace Detail
{

EA_DISABLE_CLANG_WARNING(-Wunused-function);
EA_DISABLE_VC_WARNING(4505); // unreferenced local function has been removed

EA_THREAD_LOCAL uintptr_t sStackBase = 0;
EA_THREAD_LOCAL uintptr_t sStackLimit = 0;

///////////////////////////////////////////////////////////////////////////////
// GetCallstackx64FramePtrWalk
//
// This Function returns the callstack by walking the stack through the use of frame pointers.
// This Function intentionally skips the first stack frame of the provided context, pContext.
EA_NO_INLINE static size_t GetCallstackx64FramePtrWalk(void** pReturnAddressArray, size_t nReturnAddressArrayCapacity,
													   const CallstackContext* pContext)
{
	/**
	 *  ------------------------------
	 *  |  saved RIP, pushed by CALL |
	 *  |                            |
	 *  ------------------------------
	 *  | Caller's Saved RBP         |
	 *  |                            |  <- Current RBP register points here
	 *  ------------------------------
	 */

	size_t nEntryIndex = 0;

	if (sStackLimit == 0)
		sStackLimit = reinterpret_cast<uintptr_t>(EA::Thread::GetStackLimit());
	if (sStackBase == 0)
		sStackBase = reinterpret_cast<uintptr_t>(EA::Thread::GetStackBase());

	uintptr_t currentRBP;
	uintptr_t prevRBP;
	uintptr_t returnRIP;

	currentRBP = pContext->mRBP;

	/* NOTE(mwinkler): We receive a context, so pContext never points to our stack frame */
	for (; (nEntryIndex < nReturnAddressArrayCapacity) && currentRBP && currentRBP < sStackBase && currentRBP > sStackLimit;)
	{
		returnRIP = *(reinterpret_cast<uintptr_t*>(currentRBP + 8));
		if (!returnRIP)
			break;

		prevRBP = currentRBP;
		currentRBP = *(reinterpret_cast<uintptr_t*>(currentRBP));

		pReturnAddressArray[nEntryIndex++] = reinterpret_cast<void*>(returnRIP);
	}

	return nEntryIndex;
}

EA_RESTORE_VC_WARNING();
EA_RESTORE_CLANG_WARNING();

} // namespace Detail

///////////////////////////////////////////////////////////////////////////////
// GetCallstackContext
//
EACALLSTACK_API void GetCallstackContext(CallstackContext& context, const Context* pContext)
{
	context.mRIP = pContext->Rip;
	context.mRSP = pContext->Rsp;
	context.mRBP = pContext->Rbp;
}

} // namespace Callstack
} // namespace EA
