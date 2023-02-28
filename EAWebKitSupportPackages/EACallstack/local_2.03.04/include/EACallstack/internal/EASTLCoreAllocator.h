/////////////////////////////////////////////////////////////////////////////
// Copyright (c) Electronic Arts Inc. All rights reserved.
/////////////////////////////////////////////////////////////////////////////

///////////////////////////////////////////////////////////////////////////////
//
// Implements an EASTL allocator that uses an arbitrary ICoreAllocator.
//
///////////////////////////////////////////////////////////////////////////////


#ifndef EACALLSTACK_INTERNAL_EASTLCOREALLOCATOR_H
#define EACALLSTACK_INTERNAL_EASTLCOREALLOCATOR_H


#include <EACallstack/internal/Config.h>

#if defined(EA_PRAGMA_ONCE_SUPPORTED)
	#pragma once
#endif // EA_PRAGMA_ONCE_SUPPORTED

#include <coreallocator/icoreallocator_interface.h>
#include <EACallstack/Allocator.h>
#include <EASTL/allocator.h>


namespace EA
{
namespace Callstack
{


	/// EASTLCoreAllocator
	///
	/// Implements the EASTL allocator interface.
	/// Allocates memory from an instance of ICoreAllocator.
	///
	/// Example usage:
	///     eastl::list<Widget, EASTLCoreAllocator> widgetList("UI/WidgetList", pSomeCoreAllocator);
	///     widgetList.push_back(Widget());
	///
	/// Example usage:
	///    // Note that the CoreAllocator is declared before and thus destroyed after the widget list.
	///    typedef eastl::list<Widget, EASTLCoreAllocator> WidgetList;
	///    CoreAllocatorFixed<WidgetList::node_type> widgetCoreAllocator(pFixedAllocatorForWidgetListValueType);
	///    WidgetList widgetList(EASTLCoreAllocator("UI/WidgetList", &widgetCoreAllocator));
	///
	class EASTLCoreAllocator
	{
	public:
		EASTLCoreAllocator(const char* pName = EASTL_NAME_VAL(EASTL_ALLOCATOR_DEFAULT_NAME));
		EASTLCoreAllocator(const char* pName, EA::Allocator::ICoreAllocator* pAllocator);
		EASTLCoreAllocator(const EASTLCoreAllocator& x);
		EASTLCoreAllocator(const EASTLCoreAllocator& x, const char* pName);

		EASTLCoreAllocator& operator=(const EASTLCoreAllocator& x);

		void* allocate(size_t n, int flags = 0);
		void* allocate(size_t n, size_t alignment, size_t offset, int flags = 0);
		void  deallocate(void* p, size_t n);

		EA::Allocator::ICoreAllocator* get_allocator() const;
		void                           set_allocator(EA::Allocator::ICoreAllocator* pAllocator);

		int  get_flags() const;
		void set_flags(int flags);

		const char* get_name() const;
		void        set_name(const char* pName);

	protected:
		friend bool operator==(const EASTLCoreAllocator& a, const EASTLCoreAllocator& b);
		friend bool operator!=(const EASTLCoreAllocator& a, const EASTLCoreAllocator& b);

		EA::Allocator::ICoreAllocator* mpCoreAllocator;
		int                            mnFlags;    // Allocation flags. See ICoreAllocator/AllocFlags.

		#if EASTL_NAME_ENABLED
			const char* mpName; // Debug name, used to track memory.
		#endif
	};

	bool operator==(const EASTLCoreAllocator& a, const EASTLCoreAllocator& b);
	bool operator!=(const EASTLCoreAllocator& a, const EASTLCoreAllocator& b);


} // namespace Callstack
} // namespace EA


///////////////////////////////////////////////////////////////////////////////
// Inlines
///////////////////////////////////////////////////////////////////////////////

namespace EA
{
namespace Callstack
{

	inline EASTLCoreAllocator::EASTLCoreAllocator(const char* EASTL_NAME(pName))
	  : mpCoreAllocator(EA::Callstack::GetAllocator())
	  , mnFlags(0)
	{
		#if EASTL_NAME_ENABLED
			mpName = pName ? pName : EASTL_ALLOCATOR_DEFAULT_NAME;
		#endif
	}

	inline EASTLCoreAllocator::EASTLCoreAllocator(const char* EASTL_NAME(pName), EA::Allocator::ICoreAllocator* pCoreAllocator)
	  : mpCoreAllocator(pCoreAllocator)
	  , mnFlags(0)
	{
		#if EASTL_NAME_ENABLED
			mpName = pName ? pName : EASTL_ALLOCATOR_DEFAULT_NAME;
		#endif
	}

	inline EASTLCoreAllocator::EASTLCoreAllocator(const EASTLCoreAllocator& x)
	  : mpCoreAllocator(x.mpCoreAllocator)
	  , mnFlags(x.mnFlags)
	{
		#if EASTL_NAME_ENABLED
			mpName = x.mpName;
		#endif
	}

	inline EASTLCoreAllocator::EASTLCoreAllocator(const EASTLCoreAllocator& x, const char* EASTL_NAME(pName))
	  : mpCoreAllocator(x.mpCoreAllocator)
	  , mnFlags(x.mnFlags)
	{
		#if EASTL_NAME_ENABLED
			mpName = pName ? pName : EASTL_ALLOCATOR_DEFAULT_NAME;
		#endif
	}

	inline EASTLCoreAllocator& EASTLCoreAllocator::operator=(const EASTLCoreAllocator& x)
	{
		// In order to be consistent with EASTL's allocator implementation,
		// we don't copy the name from the source object.
		mpCoreAllocator = x.mpCoreAllocator;
		mnFlags         = x.mnFlags;
		return *this;
	}

	inline void* EASTLCoreAllocator::allocate(size_t n, int /*flags*/)
	{
		// It turns out that EASTL itself doesn't use the flags parameter,
		// whereas the user here might well want to specify a flags
		// parameter. So we use ours instead of the one passed in.
		return mpCoreAllocator->Alloc(n, EASTL_NAME_VAL(mpName), (unsigned)mnFlags);
	}

	inline void* EASTLCoreAllocator::allocate(size_t n, size_t alignment, size_t offset, int /*flags*/)
	{
		// It turns out that EASTL itself doesn't use the flags parameter,
		// whereas the user here might well want to specify a flags
		// parameter. So we use ours instead of the one passed in.
		return mpCoreAllocator->Alloc(n, EASTL_NAME_VAL(mpName), (unsigned)mnFlags, (unsigned)alignment, (unsigned)offset);
	}

	inline void EASTLCoreAllocator::deallocate(void* p, size_t n)
	{
		return mpCoreAllocator->Free(p, n);
	}

	inline EA::Allocator::ICoreAllocator* EASTLCoreAllocator::get_allocator() const
	{
		return mpCoreAllocator;
	}

	inline void EASTLCoreAllocator::set_allocator(EA::Allocator::ICoreAllocator* pAllocator)
	{
		mpCoreAllocator = pAllocator;
	}

	inline int EASTLCoreAllocator::get_flags() const
	{
		return mnFlags;
	}

	inline void EASTLCoreAllocator::set_flags(int flags)
	{
		mnFlags = flags;
	}

	inline const char* EASTLCoreAllocator::get_name() const
	{
		#if EASTL_NAME_ENABLED
			return mpName;
		#else
			return EASTL_ALLOCATOR_DEFAULT_NAME;
		#endif
	}

	inline void EASTLCoreAllocator::set_name(const char* pName)
	{
		#if EASTL_NAME_ENABLED
			mpName = pName;
		#else
			(void)pName;
		#endif
	}



	inline bool operator==(const EASTLCoreAllocator& a, const EASTLCoreAllocator& b)
	{
		return (a.mpCoreAllocator == b.mpCoreAllocator) &&
			   (a.mnFlags         == b.mnFlags);
	}

	inline bool operator!=(const EASTLCoreAllocator& a, const EASTLCoreAllocator& b)
	{
		return (a.mpCoreAllocator != b.mpCoreAllocator) ||
			   (a.mnFlags         != b.mnFlags);
	}


} // namespace Callstack
} // namespace EA


#endif // EACALLSTACK_INTERNAL_EASTLCOREALLOCATOR_H
