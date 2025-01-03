///////////////////////////////////////////////////////////////////////////////
// Copyright (c) Electronic Arts Inc. All rights reserved.
///////////////////////////////////////////////////////////////////////////////

#include <EACallstack/internal/Config.h>
#include <EACallstack/EACallstack.h>
#include <EACallstack/Context.h>

namespace EA
{
namespace Callstack
{

EACALLSTACK_API void InitCallstack()
{
}

EACALLSTACK_API void ShutdownCallstack()
{
}

EACALLSTACK_API EA_NO_INLINE size_t GetCallstack(void*[], size_t, const CallstackContext*)
{
	return 0;
}

EACALLSTACK_API EA_NO_INLINE bool GetCallstackContext(CallstackContext&, intptr_t)
{
	return false;
}

EACALLSTACK_API EA_NO_INLINE bool GetCallstackContextSysThreadId(CallstackContext&, intptr_t)
{
	return false;
}

EACALLSTACK_API void GetCallstackContext(CallstackContext&, const Context*)
{
}

} // namespace Callstack
} // namespace EA
