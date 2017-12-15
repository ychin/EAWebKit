/*
Copyright (C) 2011, 2012, 2013, 2014 Electronic Arts, Inc.  All rights reserved.

Redistribution and use in source and binary forms, with or without
modification, are permitted provided that the following conditions
are met:

1.  Redistributions of source code must retain the above copyright
    notice, this list of conditions and the following disclaimer.
2.  Redistributions in binary form must reproduce the above copyright
    notice, this list of conditions and the following disclaimer in the
    documentation and/or other materials provided with the distribution.
3.  Neither the name of Electronic Arts, Inc. ("EA") nor the names of
    its contributors may be used to endorse or promote products derived
    from this software without specific prior written permission.

THIS SOFTWARE IS PROVIDED BY ELECTRONIC ARTS AND ITS CONTRIBUTORS "AS IS" AND ANY
EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE IMPLIED
WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE ARE
DISCLAIMED. IN NO EVENT SHALL ELECTRONIC ARTS OR ITS CONTRIBUTORS BE LIABLE FOR ANY
DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES
(INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES;
LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND
ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT
(INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE OF
THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
*/


#include "config.h"
#include <stdlib.h>
#include <string.h>
#include <stdio.h>

#include <EAWebKit/EAWebkitAllocator.h>
#include <internal/include/EAWebKitAssert.h>
#include <wtf/Assertions.h>
#include <EAIO/Allocator.h>

#if ENABLE(SQL_DATABASE)
#include "sqlite3.h"
#endif

// Note by Arpit Baldeva: Following define makes sure that no memory allocation happens in EAWebKit before EA::WebKit::Init()
// is called. If you comment out following, the use of default allocator would result in a crash.
// Not enabled by default as an integrator of the library may not give an allocator during the initial integration.
//#define NO_DEFAULT_ALLOCATOR 1

#include <windows.h>
namespace 
{
inline DWORD protection(bool writable, bool executable)
{
	return (DWORD) (executable ?
		(writable ? PAGE_EXECUTE_READWRITE : PAGE_EXECUTE_READ) :
		(writable ? PAGE_READWRITE : PAGE_READONLY));
}
}

namespace EA
{
namespace WebKit
{

///////////////////////////////////////////////////////////////////////
// Allocator
///////////////////////////////////////////////////////////////////////

// DefaultAllocator is a basic malloc-based allocator.
// Many applications will want to use their own allocator.
// The allocator needs to be set when the EAWebKit module is initialized.

//Note by Arpit Baldeva: Microsoft _aligned_malloc needs to be coupled with _aligned_free. We have Malloc and MallocAligned and Free but not FreeAligned. 
// To tackle the problem, I can think of 2 approaches. One is to allocate all the memory aligned internally which is what we do below. 
// 2nd approach would be to stick a 4 byte magic number with each allocation to figure out if it was aligned or normal allocation.

class DefaultAllocator : public Allocator
{
	void* Malloc(size_t size, int /*flags*/, const char* /*pName*/)
	{
#if defined(NO_DEFAULT_ALLOCATOR)
		ASSERT_NOT_REACHED();
#endif
		#if defined(_MSC_VER)
			#if (EA_PLATFORM_PTR_SIZE == 8)
				return	_aligned_malloc(size, 16); //16 byte alignment on 64 bit 
			#else
				return	_aligned_malloc(size, 8); //8 byte alignment on 32 bit
			#endif
		#elif defined(__GNUC__)
			return malloc(size);
		#endif

	}

	void* MallocAligned(size_t size, size_t alignment, size_t offset, int /*flags*/, const char* /*pName*/)
	{
		(void) offset;
		#if defined(_MSC_VER)
			return _aligned_malloc(size, alignment);
		#elif defined(__GNUC__) 
                return memalign(alignment, size);
		#endif
	}

	void Free(void* p, size_t /*size*/)
	{
		#if defined(_MSC_VER)
			_aligned_free(p);
		#elif defined(__GNUC__)
			free(p);
		#endif
	}

	void* Realloc(void* p, size_t size, int /*flags*/)
	{
		#if defined(_MSC_VER)
			return _aligned_realloc(p, size, 1);
		#elif defined(__GNUC__)
			return realloc(p, size);
		#endif

	}

	// OS memory management API.
	bool SupportsOSMemoryManagement()
	{
		return true;
	}

	size_t SystemPageSize()
	{
		size_t size = 0;
		SYSTEM_INFO system_info;
		GetSystemInfo(&system_info);
		size = system_info.dwPageSize;
		return size;
	}

	void* ReserveUncommitted(size_t bytes, bool writable, bool executable)
	{
		void* result = VirtualAlloc(0, bytes, MEM_RESERVE, protection(writable, executable));
 		EAW_ASSERT_MSG(result, "VirtualAlloc failed");
		return result;
	}
	
	
	void* ReserveAndCommit(size_t bytes, bool writable, bool executable)
	{
		void* result = VirtualAlloc(0, bytes, MEM_RESERVE | MEM_COMMIT, protection(writable, executable));
		EAW_ASSERT_MSG(result, "VirtualAlloc failed");
		return result;
	}
	
	void ReleaseDecommitted(void* address, size_t bytes)
	{
		// According to http://msdn.microsoft.com/en-us/library/aa366892(VS.85).aspx,
		// dwSize must be 0 if dwFreeType is MEM_RELEASE.
		bool result = VirtualFree(address, 0, MEM_RELEASE);
		(void)result;
		EAW_ASSERT_MSG(result, "VirtualFree failed");
	}

	virtual void* ReserveAndCommitAligned(size_t bytes, size_t alignment, void*& reserveBase, size_t& reserveSize, bool writable, bool executable)
	{
		size_t alignmentMask = alignment - 1;
		(void)alignmentMask;

		//VirtualAlloc does not take alignment so we reserve extra memory to be able to commit required amount of memory on alignment requested.
		reserveSize = bytes + alignment; 
		reserveBase = VirtualAlloc(0, reserveSize, MEM_RESERVE, protection(writable, executable));

		// Select an aligned region within the reservation and commit.
		void* alignedBase = reinterpret_cast<uintptr_t>(reserveBase) & alignmentMask
			? reinterpret_cast<void*>((reinterpret_cast<uintptr_t>(reserveBase) & ~alignmentMask) + alignment)
			: reserveBase;
		
		bool isAligned = !(reinterpret_cast<intptr_t>(alignedBase) & (alignment - 1));
		EAW_ASSERT_MSG(isAligned,"Not aligned");
		return VirtualAlloc(alignedBase, bytes, MEM_COMMIT, protection(writable, executable));

	}

	virtual void ReleaseDecommittedAligned(void* reserveBase, size_t reserveSize, size_t alignment)
	{
		// According to http://msdn.microsoft.com/en-us/library/aa366892(VS.85).aspx,
		// dwSize must be 0 if dwFreeType is MEM_RELEASE.
		bool result = VirtualFree(reserveBase, 0, MEM_RELEASE);
		(void)result;
		EAW_ASSERT_MSG(result, "VirtualFree failed");
	}

	void Commit(void* address, size_t bytes, bool writable, bool executable)
	{
		void* result = VirtualAlloc(address, bytes, MEM_COMMIT, protection(writable, executable));
		(void) result;
		EAW_ASSERT_MSG(result, "VirtualAlloc failed");
	}
	
	void Decommit(void* address, size_t bytes)
	{
		bool result = VirtualFree(address, bytes, MEM_DECOMMIT);
		(void) result;
		EAW_ASSERT_MSG(result, "VirtualFree failed");
	}
	
	
};


Allocator* spEAWebKitAllocator = 0;

void SetAllocator(EA::WebKit::Allocator* pAllocator)
{
    EAW_ASSERT_MSG(!spEAWebKitAllocator, "Setting an allocator while one already exists. It is also advisable to not call SetAllocator(0) on Shutdown since some memory might free up as part of dll unload.");
	spEAWebKitAllocator = pAllocator;
}

Allocator* GetAllocator()
{
	if(!spEAWebKitAllocator)
	{
		void* allocMem = malloc(sizeof(DefaultAllocator));
		spEAWebKitAllocator = new(allocMem) DefaultAllocator();
		// Order is important here. We assert assert after the allocator is established otherwise
		// we end up in an endless loop since our logger uses eastl::string which tries to allocate
		// memory as well.
		EAW_ASSERT_MSG(false, "Using default allocator"); 
	}
	return spEAWebKitAllocator;
}

}// namespace WebKit

}//namespace EA

#include <wtf/FastMalloc.h>
namespace WTF 
{
void* fastMalloc(size_t n) 
{
	void* p = EA::WebKit::spEAWebKitAllocator->Malloc(n, 0, 0);
	return p;
}

void* fastZeroedMalloc(size_t n)
{
	void* p = fastMalloc(n); 
	memset(p, 0, n);
	return p;
}

void* fastCalloc(size_t count, size_t n)
{
	void* p = EA::WebKit::spEAWebKitAllocator->Malloc(count * n, 0, 0);
	memset(p, 0, count * n);
	return p;
}

void* fastRealloc(void* p, size_t n)
{
	return EA::WebKit::spEAWebKitAllocator->Realloc(p, n, 0);
}

char* fastStrDup(const char* src)
{
	size_t len = strlen(src) + 1;
	char* dup = static_cast<char*>(fastMalloc(len));
	memcpy(dup, src, len);
	return dup;
}

size_t fastMallocSize(const void* p)
{
	// The sole use of this function is to assist in debugging by querying the system if the pointer is valid.
	// We simply return pointer as valid because our allocators are usually well equipped to detect stomps. 
	return 1;
}

size_t fastMallocGoodSize(size_t bytes)
{
	return bytes;
}

TryMallocReturnValue tryFastMalloc(size_t n)
{
	return fastMalloc(n);
}

TryMallocReturnValue tryFastZeroedMalloc(size_t n)
{
	return fastZeroedMalloc(n);
}

TryMallocReturnValue tryFastCalloc(size_t n_elements, size_t element_size)
{
	return fastCalloc(n_elements, element_size);
}

TryMallocReturnValue tryFastRealloc(void* p, size_t n)
{
	return fastRealloc(p,n);
}

void fastFree(void* p)
{
	return EA::WebKit::spEAWebKitAllocator->Free(p, 0);
}

void releaseFastMallocFreeMemory() 
{

}

FastMallocStatistics fastMallocStatistics()
{
	FastMallocStatistics statistics = { 0, 0, 0 };
	return statistics;
}

#ifndef NDEBUG    
//EAWebKitTODO: Implement these properly using TLS and use them in the functions above. They assist in debugging. 
void fastMallocForbid()
{

}
void fastMallocAllow()
{

}
#endif


void* fastMallocAligned(size_t n, size_t alignment, size_t offset, int flags, const char* pName) 
{
	return EA::WebKit::spEAWebKitAllocator->MallocAligned(n, alignment, offset, flags, pName);

}

} // namespace WTF

#if (BUILDING_EAWEBKIT_DLL)
#include <coreallocator/icoreallocator_interface.h>
namespace EA
{
namespace WebKit
{
class CoreAllocatorWebkitImplementation : public EA::Allocator::ICoreAllocator
{
public:
	void* Alloc(size_t size, const char* name, unsigned int flags)
	{ 
		return spEAWebKitAllocator->Malloc(size, flags, name); 
	}
	void* Alloc(size_t size, const char* name, unsigned int flags, unsigned int alignment, unsigned int alignmentOffset = 0)
	{ 
		return spEAWebKitAllocator->MallocAligned(size, alignment, alignmentOffset, flags,name);
	}

	void  Free(void* p, size_t /*size*/ = 0)
	{ 
		spEAWebKitAllocator->Free(p,0);
	}
};
}

namespace Allocator
{
ICoreAllocator* ICoreAllocator::GetDefaultAllocator()
{
	static EA::WebKit::CoreAllocatorWebkitImplementation sCoreAllocatorWebkitImplementation;
	return &sCoreAllocatorWebkitImplementation;
}
}
}



// DirtySDK package ///////////////////////////////////////////////////////////////

extern "C" void* DirtyMemAlloc(int32_t iSize, int32_t /*iMemModule*/, int32_t /*iMemGroup*/)
{
	return EA::WebKit::spEAWebKitAllocator->Malloc(static_cast<size_t>(iSize), 0, "EAWebKit/DirtyMemAlloc");
}

extern "C" void DirtyMemFree(void* p, int32_t /*iMemModule*/, int32_t /*iMemGroup*/)
{
	return EA::WebKit::spEAWebKitAllocator->Free(p, 0); 
}

// libjpeg package  ///////////////////////////////////////////////////////////

// libjpeg expects the user to implement memory overrides by writing a 
// custom version of jmemansi.c. Note that libjpeg already has some 
// custom versions of thus, such as jmemdos.c, jmemmac.c etc.
// What we do is simply don't include any of those jmem*.c files in
// the libjpeg build and instead provide the implementation here.

extern "C" 
{
#include <jpeglib.h> // libjpeg requires you to #include its headers from C++ via extern "C".
#include <jmemsys.h>

void* jpeg_get_small(j_common_ptr /*cinfo*/, size_t n)
{
	return EA::WebKit::spEAWebKitAllocator->Malloc(n, 0, "EAWebKit/LibJpeg");
}

void jpeg_free_small(j_common_ptr /*cinfo*/, void* p, size_t /*n*/)
{
	return EA::WebKit::spEAWebKitAllocator->Free(p, 0);
}

void* jpeg_get_large(j_common_ptr /*cinfo*/, size_t n)
{
	return EA::WebKit::spEAWebKitAllocator->Malloc(n, 0, "EAWebKit/LibJpeg");
}

void jpeg_free_large(j_common_ptr /*cinfo*/, void* p, size_t /*n*/)
{
	return EA::WebKit::spEAWebKitAllocator->Free(p, 0);
}

long jpeg_mem_available(j_common_ptr /*cinfo*/, long /*min_bytes_needed*/, long max_bytes_needed, long /*already_allocated*/)
{
	return max_bytes_needed;
}

static void read_backing_store(j_common_ptr cinfo, backing_store_ptr info, void* buffer_address, long file_offset, long byte_count)
{
	if(fseek(info->temp_file, file_offset, SEEK_SET) == 0)
	{
		if(fread(buffer_address, 1, byte_count, info->temp_file) == (size_t)byte_count)
			return; // success
	}

	(void)cinfo; (void)info; (void)file_offset;
	memset(buffer_address, 0, byte_count);
}

static void write_backing_store(j_common_ptr cinfo, backing_store_ptr info, void* buffer_address, long file_offset, long byte_count)
{
	if(fseek(info->temp_file, file_offset, SEEK_SET) == 0)
	{
		if(fwrite(buffer_address, 1, byte_count, info->temp_file) == (size_t)byte_count)
			return; // success
	}

	(void)cinfo; (void)info; (void)buffer_address; (void)file_offset; (void)byte_count;
}

static void close_backing_store(j_common_ptr cinfo, backing_store_ptr info)
{
	fclose(info->temp_file);
}

void jpeg_open_backing_store(j_common_ptr /*cinfo*/, backing_store_ptr info, long /*total_bytes_needed*/)
{
	info->read_backing_store  = read_backing_store;
	info->write_backing_store = write_backing_store;
	info->close_backing_store = close_backing_store;
}

long jpeg_mem_init(j_common_ptr /*cinfo*/)
{
	return 0;
}

void jpeg_mem_term(j_common_ptr /*cinfo*/)
{
	// Nothing to do.
}

} // extern "C"


// libpng package  ///////////////////////////////////////////////////////////

// The original libpng package doesn't allow you to specify a global memory 
// allocation default, but instead requires that the user pass a malloc/free
// function for each use of the library. This is fine but the lack of a global
// memory allocation default creates a big problem: we can't control third 
// party code that fails to pass a malloc/free function and thus third party
// code will use malloc/free and there isn't anything we can do about it.
// Well there is something we can do about it and that is to modify libpng
// to support the concept of a user-settable global memory allocator instead
// of the currently hard-coded call to malloc. So I did that and implemented
// the png_set_default_mem_fn function.

#include <png.h>

//extern "C" {

void* png_malloc(size_t n)
{
	// 05/03/12
	// abaldeva: There is no png_malloc_aligned but png_create_struct_2 needs an aligned malloc call to 16 byte boundary
	// on x64 Windows. We do a 16 byte alignment on all x64(including platforms other than Windows)
#if (EA_PLATFORM_PTR_SIZE == 8)
	return EA::WebKit::spEAWebKitAllocator->MallocAligned(n,16,0,0,"EAWebKit/LibPng");
#else
	return EA::WebKit::spEAWebKitAllocator->Malloc(n, 0, "EAWebKit/LibPng");
#endif
}

void png_free(void* p)
{
	return EA::WebKit::spEAWebKitAllocator->Free(p, 0);
}

// } // extern "C"




// libxml package  ///////////////////////////////////////////////////////////
// libxml has been set up to require you to provide the functions below.
#include <libxml/xmlmemory.h>

extern "C" 
{

void* xmlMallocCustom(size_t n)
{
	return EA::WebKit::spEAWebKitAllocator->Malloc(n, 0, "EAWebKit/LibXml");
}

void xmlFreeCustom(void* p)
{
	return EA::WebKit::spEAWebKitAllocator->Free(p, 0);
}

void* xmlReallocCustom(void* p, size_t n)
{
	return EA::WebKit::spEAWebKitAllocator->Realloc(p, n, 0);
}

char* xmlStrdupCustom(const char* p)
{
	const size_t n = strlen(p) + 1;

	char* const pNew = (char*)EA::WebKit::spEAWebKitAllocator->Malloc(n, 0, "EAWebKit/LibXml");

	if(pNew)
		strcpy(pNew, p);

	return pNew;
}

} // extern "C"

// pixman package  /////////////////////////////////////////////////////
// pixman has been set up to require you to provide the functions below.
extern "C" 
{

void *pixman_malloc(size_t size)
{
    return EA::WebKit::spEAWebKitAllocator->Malloc(size, 0, "EAWebKit/Pixman");
}

void *pixman_calloc(size_t num, size_t size)
{
	void* p = pixman_malloc(num * size);
	if(p)
		memset(p, 0, num * size);

	return p;
}

void* pixman_realloc(void* p, size_t size)
{
	return EA::WebKit::spEAWebKitAllocator->Realloc(p,size,0);
}
void pixman_free(void *p)
{
    EA::WebKit::spEAWebKitAllocator->Free(p, 0);
}

}

// cairo package  //////////////////////////////////////////////////////
// cairo has been set up to require you to provide the functions below.
extern "C" 
{
void *cairo_malloc(size_t size)
{
    return EA::WebKit::spEAWebKitAllocator->Malloc(size, 0, "EAWebKit/Cairo");
}

void *cairo_calloc(size_t num, size_t size)
{
	void* p = cairo_malloc(num * size);
	if(p)
		memset(p, 0, num * size);

	return p;
}

void *cairo_realloc(void *p, size_t size)
{
    return EA::WebKit::spEAWebKitAllocator->Realloc(p, size, 0);
}

void cairo_free(void *p)
{
    EA::WebKit::spEAWebKitAllocator->Free(p, 0);
}
}

#if ENABLE(SQL_DATABASE)
// sqlite3 package /////////////////////////////////////////////////////
// Allocations are padded with 4 bytes so that we can store the size
// of the allocation before the returned address. We do this because
// SQLite needs us to be able to tell it the size of allocations it
// makes.
// Note - There are still some allocations done by SQLite directly using malloc/free on Windows. If it becomes a problem, we'll need to fix that.
extern "C"
{
    void *sqlite_malloc(int size)
    {
        int *alloc = (int*)EA::WebKit::spEAWebKitAllocator->Malloc(size + sizeof(int), 0, "EAWebKit/SQLite3");
        *alloc = size;
        
        return alloc + 1;
    }

    void *sqlite_realloc(void *p, int size)
    {
        if(p)
		{
			int *base = (int*)p - 1;
			base = (int*)EA::WebKit::spEAWebKitAllocator->Realloc(base, size + sizeof(int), 0);
			*base = size;

			return base + 1;
		}
		else // possible to realloc a null pointer
		{
			int *base = (int*)EA::WebKit::spEAWebKitAllocator->Realloc(NULL, size + sizeof(int), 0);
			*base = size;

			return base + 1;
		}
    }

    void sqlite_free(void *p)
    {
        if(p)
		{
			int *base = (int*)p - 1;
			EA::WebKit::spEAWebKitAllocator->Free(base, *base + sizeof(int));
		}
    }

    int sqlite_init(void *)
    {
        // Do nothing.
        return SQLITE_OK;
    }

    void sqlite_shutdown(void *)
    {
        // Do nothing.
    }

    int sqlite_roundup(int size)
    {
        return size;
    }

    int sqlite_size(void *p)
    {
        if(p)
			return *((int*)p - 1);

		return 0;
    }

};
#endif

#include <zlib.h>
extern "C" 
{
void* zcalloc(void* opaque, unsigned items, unsigned size)
{
	void* p = EA::WebKit::spEAWebKitAllocator->Malloc(items * size,0,0);
	if(p)
		memset(p, 0, items * size);
	return (void*)p;
}

void zcfree(void* opaque, void* ptr)
{
	(void)opaque;
	EA::WebKit::spEAWebKitAllocator->Free(ptr,0);
}
}
namespace EA
{
namespace WebKit
{
void SetPackageAllocators()
{
	EA::IO::SetAllocator(EA::Allocator::ICoreAllocator::GetDefaultAllocator());
	
	/////////////////////////////////////////////////////////////////////////
	// Setup open source package memory allocation.
	// These callback functions in turn will call the user-specified allocator.

	// Nothing to do for libjpeg (jpeg_get_small, etc.)
	png_set_default_mem_fn(png_malloc, png_free);

#if ENABLE(SQL_DATABASE)
    sqlite3_mem_methods sqliteMemConfig = {0};
    sqliteMemConfig.xMalloc = sqlite_malloc;
    sqliteMemConfig.xFree = sqlite_free;
    sqliteMemConfig.xRealloc = sqlite_realloc;
    sqliteMemConfig.xInit = sqlite_init;
    sqliteMemConfig.xShutdown = sqlite_shutdown;
    sqliteMemConfig.xRoundup = sqlite_roundup;
    sqliteMemConfig.xSize = sqlite_size;
    sqlite3_config(SQLITE_CONFIG_MALLOC, &sqliteMemConfig);
#endif

	xmlMemSetup(xmlFreeCustom, xmlMallocCustom, xmlReallocCustom, xmlStrdupCustom);
	
	// Nothing to do for sdl (SDL_malloc, etc.).
	// Nothing to do for zlib (zcalloc, etc.).
	// Nothing to do for WTF/WebKit (fastMalloc, etc.)
	/////////////////////////////////////////////////////////////////////////
}

}
}
#endif //BUILDING_EAWEBKIT_DLL