///////////////////////////////////////////////////////////////////////////////
// EAIOCoreAllocator.cpp
//
// Copyright (c) 2006 Electronic Arts Inc.
// Written and maintained by Paul Pedriana.
///////////////////////////////////////////////////////////////////////////////


#include <EAIO/internal/Config.h>
#include <coreallocator/icoreallocator_interface.h>


#if EAIO_DLL // If this library is built to be a DLL... provide our own default new/delete-based ICoreAllocator.


namespace EA
{
	namespace IO
	{
		class CoreAllocatorMalloc : public EA::Allocator::ICoreAllocator
		{
		public:
			void* Alloc(size_t size, const char* /*name*/, unsigned int /*flags*/)
				{ return new char[size]; }

			void* Alloc(size_t size, const char* /*name*/, unsigned int /*flags*/, unsigned int /*align*/, unsigned int /*alignOffset*/ = 0)
				{ return new char[size]; }

			void  Free(void* p, size_t /*size*/ = 0)
				{ delete[] (char*)p; }
		};

		CoreAllocatorMalloc gCoreAllocatorMalloc;
	}


	#if EAIO_DEFAULT_ALLOCATOR_IMPL_ENABLED
		namespace Allocator
		{
			ICoreAllocator* ICoreAllocator::GetDefaultAllocator()
			{
				return &IO::gCoreAllocatorMalloc;
			}
		}
	#endif

} // namespace EA



#endif // EAIO_DLL


