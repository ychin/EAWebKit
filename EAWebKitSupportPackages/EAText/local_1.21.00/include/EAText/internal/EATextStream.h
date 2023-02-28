///////////////////////////////////////////////////////////////////////////////
// EATextStream.h
//
// Copyright (c) 2006 Electronic Arts. All Rights Reserved.
// Written and maintained by Paul Pedriana.
//
///////////////////////////////////////////////////////////////////////////////


#ifndef EATEXT_INTERNAL_EATEXTSTREAM_H
#define EATEXT_INTERNAL_EATEXTSTREAM_H


#include <EABase/eabase.h>
#include <EAText/EAText.h>
#include <EAIO/EAStreamMemory.h>
#include <EAIO/EAFileStream.h>


namespace EA
{
	namespace Allocator
	{
		class ICoreAllocator;
	}

	namespace IO // We put this in the IO namespace instead of EAText because some compilers don't work with cross-namespaces very well, and so this is safer.
	{

#if EAIO_VERSION_N >= 30000
		using EATextFileStream = FileStream;
		using EATextMemoryStream = MemoryStream;
#else
		/// EATextFileStream
		///
		/// This class exists for the sole purpose of providing a means to be allocated and freed by 
		/// an ICoreAllocator while supporting reference-counting. 
		///
		class EATextFileStream : public FileStream
		{
		public:
			EATextFileStream(const EATEXT_FILE_PATH_CHAR_TYPE* pPath)
				: FileStream(pPath), mpCoreAllocator(NULL) { }

			int Release();

			EA::Allocator::ICoreAllocator* mpCoreAllocator; // Allocator used to allocate and free this instance.
		};


		/// EATextMemoryStream
		///
		/// This class exists for the sole purpose of providing a means to be allocated and freed by 
		/// an ICoreAllocator while supporting reference-counting. 
		///
		class EATextMemoryStream : public MemoryStream
		{
		public:
			EATextMemoryStream(void* pData, size_type nSize, bool bUsePointer, bool bFreePointer = true, MemoryStream::Allocator* pAllocator = NULL)
				: MemoryStream(pData, nSize, bUsePointer, bFreePointer, pAllocator), mpCoreAllocator(NULL) { }

			int Release();

			EA::Allocator::ICoreAllocator* mpCoreAllocator; // Allocator used to allocate and free this instance.
		};
#endif

	} // namespace IO

} // namespace EA



#endif // Header include guard













