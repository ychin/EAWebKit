///////////////////////////////////////////////////////////////////////////////
// Allocator.cpp
// 
// Copyright (c) 2007 Criterion Software Limited and its licensors. All Rights Reserved.
// Created by Paul Pedriana
///////////////////////////////////////////////////////////////////////////////


#include <EAIO/internal/Config.h>
#include <EAIO/Allocator.h>
#include <coreallocator/icoreallocator_interface.h>


namespace EA
{
namespace IO
{



EA::Allocator::ICoreAllocator* gpCoreAllocator = NULL;


EAIO_API Allocator::ICoreAllocator* GetAllocator()
{
	#if EAIO_DEFAULT_ALLOCATOR_ENABLED
		if(!gpCoreAllocator)
			gpCoreAllocator = EA::Allocator::ICoreAllocator::GetDefaultAllocator();
	#endif

	return gpCoreAllocator;
}


EAIO_API void SetAllocator(Allocator::ICoreAllocator* pCoreAllocator)
{
	gpCoreAllocator = pCoreAllocator;
}




} // namespace IO

} // namespace EA












