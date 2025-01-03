/////////////////////////////////////////////////////////////////////////////
// Copyright (c) Electronic Arts Inc. All rights reserved.
/////////////////////////////////////////////////////////////////////////////

#include <EACallstack/internal/Config.h>
#include <EACallstack/internal/Intrin.h>
#include <intrin.h>

namespace EA
{
namespace Callstack
{

namespace Detail
{

///////////////////////////////////////////////////////////////////////////////
// GetStackPointer
//
#if defined(EA_PROCESSOR_X86)

EACALLSTACK_API EA_NO_INLINE void GetStackPointer(void*& p)
{
	uintptr_t ara = reinterpret_cast<uintptr_t>(_AddressOfReturnAddress());
	p = reinterpret_cast<void*>(ara + 4);
}

#elif defined(EA_PROCESSOR_X86_64)

EACALLSTACK_API EA_NO_INLINE void GetStackPointer(void*& p)
{
	uintptr_t ara = reinterpret_cast<uintptr_t>(_AddressOfReturnAddress());
	p = reinterpret_cast<void*>(ara + 8);
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
	pInstruction = _ReturnAddress();
}


} // namespace Callstack
} // namespace EA
