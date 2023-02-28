/////////////////////////////////////////////////////////////////////////////
// Copyright (c) Electronic Arts Inc. All rights reserved.
/////////////////////////////////////////////////////////////////////////////

#ifndef EACALLSTACK_INTERNAL_INTRIN_H
#define EACALLSTACK_INTERNAL_INTRIN_H

#include <EACallstack/internal/Config.h>

#if defined(EA_PRAGMA_ONCE_SUPPORTED)
	#pragma once
#endif // EA_PRAGMA_ONCE_SUPPORTED

namespace EA
{
namespace Callstack
{

namespace Detail
{

	EACALLSTACK_API EA_NO_INLINE void GetStackPointer(void*& p);

	EACALLSTACK_LOCAL void* RemoveARM32ThumbBit(void* pCodeAddress);

	inline uintptr_t RemoveARM32ThumbBit(uintptr_t pCodeAddress)
	{
		return reinterpret_cast<uintptr_t>(RemoveARM32ThumbBit(reinterpret_cast<void*>(pCodeAddress)));
	}

} // namespace Detail

} // namespace Callstack
} // namespace EA

#endif // EACALLSTACK_INTERNAL_INTRIN_H
