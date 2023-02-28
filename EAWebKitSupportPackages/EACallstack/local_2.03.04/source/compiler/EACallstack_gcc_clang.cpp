/////////////////////////////////////////////////////////////////////////////
// Copyright (c) Electronic Arts Inc. All rights reserved.
/////////////////////////////////////////////////////////////////////////////

#include <EACallstack/internal/Config.h>
#include <EACallstack/internal/Intrin.h>

namespace EA
{
namespace Callstack
{

namespace Detail
{

///////////////////////////////////////////////////////////////////////////////
// GetStackPointer
//
#if defined(EA_PROCESSOR_X86) || defined(EA_PROCESSOR_ARM32)

EACALLSTACK_API EA_NO_INLINE void GetStackPointer(void*& p)
{
	uintptr_t ara = reinterpret_cast<uintptr_t>(__builtin_frame_address(0));
	p = reinterpret_cast<void*>(ara + 8);
}

#elif defined(EA_PROCESSOR_X86_64) || defined(EA_PROCESSOR_ARM64)

EACALLSTACK_API EA_NO_INLINE void GetStackPointer(void*& p)
{
	uintptr_t ara = reinterpret_cast<uintptr_t>(__builtin_frame_address(0));
	p = reinterpret_cast<void*>(ara + 16);
}

#else

EACALLSTACK_API EA_NO_INLINE void GetStackPointer(void*& p)
{
	p = 0;
}

#endif

} // namespace Detail


///////////////////////////////////////////////////////////////////////////////
// GetInstructionPointer
//
EACALLSTACK_API EA_NO_INLINE void GetInstructionPointer(void*& pInstruction)
{
	pInstruction = __builtin_return_address(0);
}


} // namespace Callstack
} // namespace EA
