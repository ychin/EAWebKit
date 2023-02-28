/////////////////////////////////////////////////////////////////////////////
// Copyright (c) Electronic Arts Inc. All rights reserved.
/////////////////////////////////////////////////////////////////////////////


#include <EACallstack/internal/Config.h>
#include <EACallstack/Context.h>

#include <string.h>


namespace EA
{
namespace Callstack
{

///////////////////////////////////////////////////////////////////////////////
// GetCallstackContext
//
EACALLSTACK_API void GetCallstackContext(CallstackContext& context, const Context* pContext)
{
	context.mEIP = pContext->Eip;
	context.mESP = pContext->Esp;
	context.mEBP = pContext->Ebp;
}

namespace Detail
{

EA_DISABLE_CLANG_WARNING(-Wunused-function);
EA_DISABLE_VC_WARNING(4505); // unreferenced local function has been removed

///////////////////////////////////////////////////////////////////////////////
// GetCallstackx86FramePtrWalk
//
// This Function returns the callstack by walking the stack through the use of frame pointers.
// This Function intentionally skips the first stack frame of the provided context, pContext.
EA_NO_INLINE static size_t GetCallstackx86FramePtrWalk(void* pReturnAddressArray[], size_t nReturnAddressArrayCapacity, const CallstackContext* pContext)
{
	/**
	 *  ------------------------------
	 *  |  saved EIP, pushed by CALL |
	 *  |                            |
	 *  ------------------------------
	 *  | Caller's Saved EBP         |
	 *  |                            |  <- Current EBP register points here
	 *  ------------------------------
	 */

	size_t nEntryIndex = 0;

	uintptr_t currentEBP;
	uintptr_t prevEBP;
	uintptr_t returnEIP;

	currentEBP = pContext->mEBP;

	/* NOTE(mwinkler): We receive a context, so pContext never points to our stack frame */
	for (; (nEntryIndex < nReturnAddressArrayCapacity) && currentEBP;)
	{
		returnEIP = *(reinterpret_cast<uintptr_t*>(currentEBP + 4));
		if (!returnEIP)
		{
			break;
		}

		prevEBP = currentEBP;
		currentEBP = *(reinterpret_cast<uintptr_t*>(currentEBP));

		pReturnAddressArray[nEntryIndex++] = reinterpret_cast<void*>(returnEIP);
	}

	return nEntryIndex;
}

EA_RESTORE_VC_WARNING();
EA_RESTORE_CLANG_WARNING();

} // namespace Detail

} // namespace Callstack
} // namespace EA
