///////////////////////////////////////////////////////////////////////////////
// Copyright (c) Electronic Arts Inc. All rights reserved.
///////////////////////////////////////////////////////////////////////////////

#include <EACallstack/internal/Config.h>
#include <EACallstack/internal/Intrin.h>
#include <EACallstack/EACallstack.h>
#include <EACallstack/Context.h>
#include <EAStdC/EAMemory.h>
#include <nn/diag.h>

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
	size_t ret = 0;

	if ((nReturnAddressArrayCapacity - 1) > 0)
	{
		if (pContext == nullptr)
		{
			ret = nn::diag::GetBacktrace(reinterpret_cast<uintptr_t*>(pReturnAddressArray), nReturnAddressArrayCapacity - 1);
			if (ret > 0)
			{
				ret = (1 > ret) ? 0 : (ret - 1);
				if (ret > 0)
				{
					EA::StdC::Memmove(pReturnAddressArray, pReturnAddressArray + 1, ret * sizeof(void*));
				}
			}
		}
		else
		{
			ret = nn::diag::GetBacktrace(reinterpret_cast<uintptr_t*>(pReturnAddressArray), nReturnAddressArrayCapacity - 1,
										 pContext->mFP, pContext->mSP, pContext->mPC);
		}
	}

#if defined(EA_PROCESSOR_ARM32)

	for (int i = 0; i < ret; ++i)
	{
		pReturnAddressArray[i] = Detail::RemoveARM32ThumbBit(pReturnAddressArray[i]);
	}

#endif

	pReturnAddressArray[ret] = nullptr;
	return ret;
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
