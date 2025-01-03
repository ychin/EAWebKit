/////////////////////////////////////////////////////////////////////////////
// Allocator.h
//
// Copyright (c) 2007 Criterion Software Limited and its licensors. All Rights Reserved.
// Created by Paul Pedriana
/////////////////////////////////////////////////////////////////////////////


#ifndef EAIO_ALLOCATOR_H
#define EAIO_ALLOCATOR_H


#include <EAIO/internal/Config.h>
#include <coreallocator/icoreallocator_interface.h>


namespace EA
{
	namespace IO
	{
		/// GetAllocator
		/// 
		/// Gets the default EAIO ICoreAllocator set by the SetAllocator function.
		/// If SetAllocator is not called, the ICoreAllocator::GetDefaultAllocator 
		/// allocator is returned.
		///
		EAIO_API Allocator::ICoreAllocator* GetAllocator();


		/// SetAllocator
		/// 
		/// This function lets the user specify the default memory allocator this library will use.
		/// For the most part, this library avoids memory allocations. But there are times 
		/// when allocations are required, especially during startup. 
		///
		EAIO_API void SetAllocator(Allocator::ICoreAllocator* pCoreAllocator);

	}

}

#endif // Header include guard














