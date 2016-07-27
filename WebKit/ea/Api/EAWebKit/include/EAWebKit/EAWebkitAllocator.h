/*
Copyright (C) 2009, 2010, 2011, 2012, 2013 Electronic Arts, Inc.  All rights reserved.

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

///////////////////////////////////////////////////////////////////////////////
// EAWebkitAllocator.h
// By Arpit Baldeva (Isolated Paul Pedriana's code in EAWebkit.h)
// Maintained by EAWebKit Team
///////////////////////////////////////////////////////////////////////////////

#ifndef EAWEBKIT_EAWEBKITALLOCATOR_H
#define EAWEBKIT_EAWEBKITALLOCATOR_H

#include <EABase/eabase.h>
#include "EAWebKitConfig.h"

namespace EA
{
namespace WebKit
{

// The users of this library can supply an allocator that derives from the following interface in the EA::WebKit::Init() 
// function.
//
// We would prefer to use ICoreAllocator, but ICoreAllocator doesn't support Realloc.
// WebKit requires realloc, and it's impossible to implement realloc on top of just
// malloc and free; you need to know the underlying heap. 

class Allocator
{
public:
	virtual ~Allocator() { }

	virtual void* Malloc(size_t size, int flags, const char* pName) = 0;
	virtual void* MallocAligned(size_t size, size_t alignment, size_t offset, int flags, const char* pName) = 0;
	virtual void  Free(void* p, size_t size) = 0;
	virtual void* Realloc(void* p, size_t size, int flags) = 0;
	virtual void  ValidateHeap() //Useful for tracking memory corruption
	{

	}

	// 11/22/2011 - Added following interface to let the allocator control allocation of On Demand(reserve/commit) and executable memory.
	// This is completely optional interface in order to be backward compatible. If you do not want to inherit complexity of this management,
	// simply don't implement any of the following. 
	
	// Note that following is only applicable on Windows platform currently. This can be extended to other platforms in the future which have
	// reserve/commit/decommit/release capability. Take a look at DefaultAllocator implementation
	// in the EAWebKitAllocator.cpp.
	
	
	
	// Returns false by default. If you return true, make sure to implement rest of the functions below. 
	virtual bool SupportsOSMemoryManagement()
	{
		return false;
	}
	
	virtual size_t SystemPageSize()
	{
		return 4 * 1024; //4K
	}
	
	virtual void* ReserveUncommitted(size_t bytes, bool writable, bool executable)
	{
		(void) bytes;
		(void) writable;
		(void) executable;

		return 0;
	}
	
	virtual void* ReserveAndCommit(size_t bytes, bool writable, bool executable)
	{
		(void) bytes;
		(void) writable;
		(void) executable;

		return 0;
	}

	// The way code is structured in the EAWebKit currently, bytes parameter should be ignored. Simply release decommitted memory located at address
	// parameter. Note that on Windows, releasing memory on Windows automatically decommits it.
	virtual void ReleaseDecommitted(void* address, size_t bytes)
	{
		(void) address;
		(void) bytes;
	}

	// Return value is aligned Address
	virtual void* ReserveAndCommitAligned(size_t bytes, size_t alignment, void*& reserveBase, size_t& reserveSize, bool writable, bool executable)
	{
		(void) bytes;
		(void) alignment;
		(void) reserveBase;
		(void) reserveSize;
		(void) writable;
		(void) executable;

		return 0;
	}
	
	virtual void ReleaseDecommittedAligned(void* reserveBase, size_t reserveSize, size_t alignment)
    {
        (void) reserveBase;
        (void) reserveSize;
        (void) alignment;
    }
	
	virtual void Commit(void* address, size_t bytes, bool writable, bool executable)
	{
		(void) address;
		(void) bytes;
		(void) writable;
		(void) executable;

	}
	virtual void Decommit(void* address, size_t bytes)
	{
		(void) address;
		(void) bytes;
	}
	
};

Allocator* GetAllocator();
void SetAllocator(Allocator*);

}
}

namespace EA
{
namespace WebKit
{
// Following EASTLAllocator is used by EAWebKit internally. The users of this library do not need to create or supply one.
// Example usage:
//  eastl::vector<int, EA::WebKit::EASTLAllocator> intVector;

class EASTLAllocator
{
public:
	explicit EASTLAllocator(const char* pName = NULL);
	EASTLAllocator(const EASTLAllocator& x);
	EASTLAllocator(const EASTLAllocator& x, const char* pName);
	EASTLAllocator& operator=(const EASTLAllocator&);

	void* allocate(size_t n, int flags = 0);
	void* allocate(size_t n, size_t alignment, size_t offset, int flags = 0);
	void  deallocate(void* p, size_t n);

	const char* get_name() const;
	void        set_name(const char* pName);
};

bool operator==(const EASTLAllocator& a, const EASTLAllocator& b);
bool operator!=(const EASTLAllocator& a, const EASTLAllocator& b);

inline EASTLAllocator::EASTLAllocator(const char*)
{ }

inline EASTLAllocator::EASTLAllocator(const EASTLAllocator&)
{ }

inline EASTLAllocator::EASTLAllocator(const EASTLAllocator&, const char*)
{ }

inline EASTLAllocator& EASTLAllocator::operator=(const EASTLAllocator&)
{ return *this; }

inline void* EASTLAllocator::allocate(size_t n, int flags)
{ 
	return EA::WebKit::GetAllocator()->Malloc(n, flags, "EA::WebKit::EASTLAllocator");
}

inline void* EASTLAllocator::allocate(size_t n, size_t alignment, size_t offset, int flags)
{
	return EA::WebKit::GetAllocator()->MallocAligned(n, alignment, offset, flags, "EA::WebKit::EASTLAllocator");
}

inline void EASTLAllocator::deallocate(void* p, size_t n)
{ 
	return EA::WebKit::GetAllocator()->Free(p, n); 
}

inline const char* EASTLAllocator::get_name() const
{ 
	return "EA::WebKit::EASTLAllocator"; 
}

inline void EASTLAllocator::set_name(const char*)
{ }

inline bool operator==(const EASTLAllocator&, const EASTLAllocator&)
{ return true; }  // All instances are equal.

inline bool operator!=(const EASTLAllocator& a, const EASTLAllocator&)
{ return false; } // All instances are equal.


}
}


#endif // EAWEBKIT_EAWEBKITALLOCATOR_H
