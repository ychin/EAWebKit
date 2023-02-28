/////////////////////////////////////////////////////////////////////////////
// Copyright (c) Electronic Arts Inc. All rights reserved.
/////////////////////////////////////////////////////////////////////////////


#ifndef EACALLSTACK_ALLOCATOR_H
#define EACALLSTACK_ALLOCATOR_H


#include <EACallstack/internal/Config.h>
#include <coreallocator/icoreallocator_interface.h>


namespace EA
{
namespace Callstack
{

	/// GetAllocator
	///
	/// Gets the default ICoreAllocator set by the SetAllocator function.
	///
	/// @return: Returns the default allocator
	///
	EACALLSTACK_API EA::Allocator::ICoreAllocator* GetAllocator();


	/// SetAllocator
	///
	/// This function lets the user specify the default memory allocator this library will use.
	///
	/// @param pCoreAllocator: Set the default allocator to the given allocator
	///
	EACALLSTACK_API void SetAllocator(EA::Allocator::ICoreAllocator* pCoreAllocator);

} // namespace Callstack
} // namespace EA

#endif // EACALLSTACK_ALLOCATOR_H
