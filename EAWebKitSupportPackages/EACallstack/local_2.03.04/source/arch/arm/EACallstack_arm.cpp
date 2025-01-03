/////////////////////////////////////////////////////////////////////////////
// Copyright (c) Electronic Arts Inc. All rights reserved.
/////////////////////////////////////////////////////////////////////////////

#include <EACallstack/internal/Config.h>
#include <EACallstack/EACallstack.h>
#include <EACallstack/Context.h>

namespace EA
{
namespace Callstack
{

/**
 * Apple uses different convention for arm, see platform/Apple/EACallstack_Apple.cpp
 */
#if !defined(EA_PLATFORM_APPLE)

///////////////////////////////////////////////////////////////////////////////
// GetCallstackContext
//
EACALLSTACK_API void GetCallstackContext(CallstackContext& context, const Context* pContext)
{
#if defined(EA_PROCESSOR_ARM32)

	context.mFP = pContext->mGpr[11];
	context.mSP = pContext->mGpr[13];
	context.mLR = pContext->mGpr[14];
	context.mPC = pContext->mGpr[15];

#elif defined(EA_PROCESSOR_ARM64)

	context.mFP  = pContext->mGpr[29];
	context.mSP  = pContext->mGpr[31];
	context.mLR  = pContext->mGpr[30];
	context.mPC  = pContext->mPC;

#endif
}

#endif

} // namespace Callstack
} // namespace EA
