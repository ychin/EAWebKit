/////////////////////////////////////////////////////////////////////////////
// Copyright (c) Electronic Arts Inc. All rights reserved.
/////////////////////////////////////////////////////////////////////////////

#include <EACallstack/internal/Config.h>
#include <EACallstack/Allocator.h>
#include <coreallocator/icoreallocator_interface.h>


namespace EA
{
namespace Callstack
{


static EA::Allocator::ICoreAllocator* gpCoreAllocator = nullptr;


EACALLSTACK_API EA::Allocator::ICoreAllocator* GetAllocator()
{
	#if EACALLSTACK_DEFAULT_ALLOCATOR_ENABLED
		if (!gpCoreAllocator)
		{
			gpCoreAllocator = EA::Allocator::ICoreAllocator::GetDefaultAllocator();
		}
	#endif

	return gpCoreAllocator;
}


EACALLSTACK_API void SetAllocator(EA::Allocator::ICoreAllocator* pCoreAllocator)
{
	gpCoreAllocator = pCoreAllocator;
}


} // namespace Callstack
} // namespace EA
