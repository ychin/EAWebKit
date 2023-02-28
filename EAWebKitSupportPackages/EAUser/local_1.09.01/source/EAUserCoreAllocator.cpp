///////////////////////////////////////////////////////////////////////////////
// EAJsonCoreAllocator.cpp
//
// Copyright (c) 2010 Electronic Arts Inc.
// Written and maintained by Paul Pedriana.
///////////////////////////////////////////////////////////////////////////////


#include <EAUser/internal/Config.h>
#include <coreallocator/icoreallocator_interface.h>


#if EAUSER_DLL // If this library is built to be a DLL... provide our own default new/delete-based ICoreAllocator.


namespace EA
{
	namespace User
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


	namespace Allocator
	{
		ICoreAllocator* ICoreAllocator::GetDefaultAllocator()
		{
			return &User::gCoreAllocatorMalloc;
		}
	}

} // namespace EA

#endif // EAUSER_DLL
