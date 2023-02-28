// (c) 2007 Electronic Arts Inc.

#ifndef EA_ALLOCATOR_ICOREALLOCATORMACROS_H
#define EA_ALLOCATOR_ICOREALLOCATORMACROS_H

#ifdef _MSC_VER
#pragma once
#endif

/// @brief This header file defines macros for accessing ICoreAllocator.

#include "coreallocator/icoreallocator_interface.h"

/// @remarks
/// CORE_ALLOCATOR_DEBUGPARAMS_LEVEL controls what debug information is passed
/// through to ICoreAllocator Alloc methods by the CORE_NEW & CORE_ALLOC macros.
/// This value may be defined by the user ... if not it will default to 1 for
/// EA_DEBUG builds, otherwise 0.
///
/// 0 - no debug information is passed through to CORE_NEW/CORE_ALLOC calls.
/// 1 - 'name' is passed through to CORE_NEW/CORE_ALLOC calls.
/// >1 - 'name', __FILE__, and __LINE__ are passed through to CORE_NEW/CORE_ALLOC calls.
///
/// CORE_ALLOCATOR_DEBUGPARAMS_LEVEL can be redefined as needed in different builds
/// or sections of code (or more likely different libraries) within the same build
/// (thus some code can pass debug parameters while other code does not).
/// This define should not be used to alter function signatures, object sizes or
/// anything else that would make code compiled with it on incompatible with code
/// compiled with it off.

#if !defined(CORE_ALLOCATOR_DEBUGPARAMS_LEVEL) || (CORE_ALLOCATOR_DEBUGPARAMS_LEVEL < 0)
   #if defined(EA_DEBUG)
	  #define CORE_ALLOCATOR_DEBUGPARAMS_LEVEL (1)
   #else
	  #define CORE_ALLOCATOR_DEBUGPARAMS_LEVEL (0)
   #endif
#endif

/// @remarks
/// Helper macro for passing debug params to Alloc methods only if desired.
/// See CORE_ALLOCATOR_DEBUGPARAMS_LEVEL for details.

#if CORE_ALLOCATOR_DEBUGPARAMS_LEVEL > 0
	#if CORE_ALLOCATOR_DEBUGPARAMS_LEVEL > 1
		#define CORE_ALLOCATOR_DEBUGPARAMS(allocname_)    EA::Allocator::ICoreAllocator::DebugParams((allocname_), __FILE__, __LINE__)
		#define CORE_ALLOCATOR_DEBUGPARAMS_ENABLED        1
	#else
		#define CORE_ALLOCATOR_DEBUGPARAMS(allocname_)    (allocname_)
		#define CORE_ALLOCATOR_DEBUGPARAMS_ENABLED        0
	#endif
#else
	#define CORE_ALLOCATOR_DEBUGPARAMS(allocname_)        NULL
	#define CORE_ALLOCATOR_DEBUGPARAMS_ENABLED            0
#endif



namespace EA
{
	namespace Allocator
	{
		namespace detail
		{
			// kOffset is the number of bytes from the beginning of the allocation to the user data for array allocations.
			const size_t kOffset = 16;
		}
	}
}


#include "coreallocator/detail/macro_impl.h"

/// @param allocator_   allocator_ must implement the EA::Allocator::ICoreAllocator interface
/// @param allocname_   string to give allocation name
/// @param flags_:      additional information on how to allocate the block (MEM_PERM or MEM_TEMP)
/// @return             Allocated and constructed object
///
/// Example usage:
///    Widget* widget = CORE_NEW(allocator, "UI/Widget", EA::Allocator::MEM_TEMP) Widget;
///    CORE_DELETE(allocator,widget);
///
/// @remarks
/// It is NOT valid to create an array using this macro, use CORE_NEW_ARRAY instead.
/// If you attempt to create an array (by putting brackets after the class name) a link
/// error will be generated.
///
/// It is not possible to have this functionality via an override of operator new,
/// as operator new/delete only understand custom implementations on a class-by-class
/// basis and not on an allocator instance basis. The C++ language provides no way to
/// call operator delete with an allocator parameter.

#define CORE_NEW(allocator_, allocname_, flags_) \
	new(EA::Allocator::detail::opNonArrayNew(), (allocator_), CORE_ALLOCATOR_DEBUGPARAMS((allocname_)), (flags_))

#define CORE_GLOBAL_NEW(allocator_, allocname_, flags_) ::CORE_NEW(allocator_, allocname_, flags_)


/// @param allocator_   allocator_ must implement the EA::Allocator::ICoreAllocator interface
/// @param allocname_   string to give allocation name
/// @param flags_       allocation flags, see EA::Allocator::ICoreAllocator for a description.
/// @param align_       Memory alignment to give (must be power of 2)
/// @return             Allocated and constructed object
///
/// Example usage: //allocate a widget aligned to 32 bytes, then delete it
///    Widget* widget = CORE_NEW_ALIGN(allocator, "UI/Widget", EA::Allocator::MEM_TEMP, 32) Widget;
///    CORE_DELETE(allocator,widget);

#define CORE_NEW_ALIGN(allocator_, allocname_, flags_, align_) \
	new(EA::Allocator::detail::opNonArrayNew(), (allocator_), CORE_ALLOCATOR_DEBUGPARAMS((allocname_)), (flags_), (align_))

#define CORE_GLOBAL_NEW_ALIGN(allocator_, allocname_, flags_, align_) ::CORE_NEW_ALIGN(allocator_, allocname_, flags_, align_)


/// @param allocator_   allocator_ must implement the EA::Allocator::ICoreAllocator interface
/// @param ptr_         pointer to the block previously allocated with CORE_NEW or CORE_NEW_ALIGN

#define CORE_DELETE(allocator_, ptr_) EA::Allocator::detail::DeleteObject((allocator_),(ptr_))


/// @param allocator_   allocator must implement the EA::Allocator::ICoreAllocator interface
/// @param type_        Type of the object being deleted.
/// @param ptr_         pointer to the block previously allocated with CORE_NEW or CORE_NEW_ALIGN
///
/// @remarks
/// This method may be needed when a class with a private destructor needs to delete an instance of
/// the class. Using CORE_DELETE in this situation may cause a compile error since CORE_DELETE calls a
/// helper method that will attempt to call the private destructor. This version of the macro avoids
/// this problem but the user has to supply the type to be deleted.
/// We create a saved temporary allocator, as the allocator member argument may be a member of the 
/// class instance being deleted. See CORE_NEW_ARRAY_PRIVATE for a related version of operator new.

#define CORE_DELETE_PRIVATE(allocator_, type_, ptr_)       \
	EA_DISABLE_VC_WARNING(4127)                            \
	do                                                     \
	{                                                      \
		EA::Allocator::ICoreAllocator* saved = allocator_; \
		if(ptr_)                                           \
		{                                                  \
			ptr_->~type_();                                \
			saved->Free(ptr_);                             \
		}                                                  \
	} while (false)                                        \
	EA_RESTORE_VC_WARNING()


#define CORE_DELETE_ARRAY_PRIVATE(allocator_, type_, ptr_)                      \
	EA_DISABLE_VC_WARNING(4127)                                                 \
	do                                                                          \
	{                                                                           \
		if(ptr_)                                                                \
		{                                                                       \
			EA::Allocator::ICoreAllocator* saved = allocator_;                  \
			type_* arrayEnd = ptr_ + ((size_t*)((uintptr_t)ptr_-EA::Allocator::detail::kOffset))[0];   \
			while(arrayEnd-- != ptr_)                                           \
				arrayEnd->~type_();                                             \
			saved->Free(reinterpret_cast<void*>((uintptr_t)ptr_-EA::Allocator::detail::kOffset));      \
		}                                                                       \
	} while (false)                                                             \
	EA_RESTORE_VC_WARNING()


/// @param allocator_   allocator_ must implement the EA::Allocator::ICoreAllocator interface
/// @param objtype_     Type of object to allocate
/// @param count_       number of objects to allocate
/// @param allocname_   string to give allocation name
/// @param flags_       allocation flags, see EA::Allocator::ICoreAllocator for a description.
/// @return             Allocated and constructed objects
///
/// @remarks
/// Constructs object with their default constructor (). See CORE_NEW_ARRAY_UNINITIALIZED for an alternative.
/// Example usage: //Allocates an array of 42 widgets, then destroys it
///    Widget* widgetArray = CORE_NEW_ARRAY(allocator, Widget, 42, "42 Widgets", EA::Allocator::MEM_TEMP);
///    CORE_DELETE_ARRAY(allocator,widgetArray);

#define CORE_NEW_ARRAY(allocator_, objtype_, count_, allocname_, flags_)   \
	EA::Allocator::detail::CreateArray<objtype_>((allocator_), (count_), CORE_ALLOCATOR_DEBUGPARAMS((allocname_)), (flags_))

/// @param allocator_   allocator_ must implement the EA::Allocator::ICoreAllocator interface
/// @param objtype_     Type of object to allocate
/// @param count_       number of objects to allocate
/// @param allocname_   string to give allocation name
/// @param flags_       allocation flags, see EA::Allocator::ICoreAllocator for a description.
/// @return             Allocated and constructed objects
///
/// @remarks
/// Constructs object without their default constructor. See CORE_NEW_ARRAY for an alternative.
/// Example usage: //Allocates an array of 42 widgets, then destroys it
///    Widget* widgetArray = CORE_NEW_ARRAY(allocator, Widget, 42, "42 Widgets", EA::Allocator::MEM_TEMP);
///    CORE_DELETE_ARRAY(allocator,widgetArray);

#define CORE_NEW_ARRAY_UNINITIALIZED(allocator_, objtype_, count_, allocname_, flags_)   \
	EA::Allocator::detail::CreateArrayUninitialized<objtype_>((allocator_), (count_), CORE_ALLOCATOR_DEBUGPARAMS((allocname_)), (flags_))

/// @param allocator_   allocator_ must implement the EA::Allocator::ICoreAllocator interface
/// @param objtype_     Type of object to allocate
/// @param count_       number of objects to allocate.
/// @param allocname_   string to give allocation name
/// @param flags_       allocation flags, see EA::Allocator::ICoreAllocator for a description.
/// @param align_       Memory alignment to give (must be power of 2)
/// @return             Allocated and constructed objects
///
/// @remarks
/// Constructs object with their default constructor (). See CORE_NEW_ARRAY_UNINITIALIZED for an alternative.

#define CORE_NEW_ARRAY_ALIGN(allocator_, objtype_, count_, allocname_, flags_, align_) \
	EA::Allocator::detail::CreateArray<objtype_>((allocator_), (count_), CORE_ALLOCATOR_DEBUGPARAMS((allocname_)), (flags_), (align_))

/// @param allocator_   allocator_ must implement the EA::Allocator::ICoreAllocator interface
/// @param ptr_         pointer to the array previously allocated with CORE_NEW_ARRAY or CORE_NEW_ARRAY_ALIGN
///
/// @remarks
/// Constructs object without their default constructor. See CORE_NEW_ARRAY for an alternative.

#define CORE_NEW_ARRAY_ALIGN_UNINITIALIZED(allocator_, objtype_, count_, allocname_, flags_, align_) \
	EA::Allocator::detail::CreateArrayUninitialized<objtype_>((allocator_), (count_), CORE_ALLOCATOR_DEBUGPARAMS((allocname_)), (flags_), (align_))

/// @param ptr_         pointer to the array previously allocated with CORE_NEW_ARRAY or CORE_NEW_ARRAY_ALIGN
/// @param allocator_   allocator_ must implement the EA::Allocator::ICoreAllocator interface
/// @param objtype_     Type of object to allocate
/// @param count_       number of objects to allocate.
/// @param allocname_   string to give allocation name
/// @param flags_       allocation flags, see EA::Allocator::ICoreAllocator for a description.
/// @return             Allocated and constructed objects
/// 
/// @remarks
/// Implements array new for a class that has a protected or private constructor. 
/// Each element is constructed with its default constructor ().
/// This isn't possible via EA::Allocator::detail::CreateArray because it would need to be given 
/// friend access to whatever classes it allocates. So instead we implement it inline here.
/// There doesn't need to be a non-array version (e.g. CORE_NEW_PRIVATE) because CORE_NEW works
/// as-is. See CORE_DELETE_PRIVATE for a related version of operator delete.
///
/// Example usage:
///    class Widget {
///      public:
///        static Widget* CreateWidgetArray(size_t n){ Widget* pArray; CORE_NEW_ARRAY_PRIVATE(pArray, pAllocator, Widget, n, "Widget", 0); return pArray; }
///        static void    DestroyWidgetArray(Widget* pArray){ CORE_DELETE_PRIVATE(pAllocator, Widget, pArray); }
///      private:
///        Widget(){}
///       ~Widget(){}
///    };

#define CORE_NEW_ARRAY_PRIVATE(ptr_, allocator_, objtype_, count_, allocname_, flags_)                                                  \
  {                                                                                                                                     \
	size_t* p_ = (size_t*)(allocator_)->Alloc(EA::Allocator::detail::kOffset + (sizeof(objtype_) * (count_)), (allocname_), (flags_));  \
	*p_ = count_;                                                                                                                       \
	objtype_* first_ = reinterpret_cast<objtype_*>((uintptr_t)p_+EA::Allocator::detail::kOffset);                                       \
	objtype_* end_ = first_ + (count_);                                                                                                 \
	for (objtype_* _cur = first_; _cur < end_; _cur++)                                                                                  \
		new(_cur) objtype_();                                                                                                           \
	ptr_ = first_;                                                                                                                      \
  } 


/// @param allocator_   allocator_ must implement the EA::Allocator::ICoreAllocator interface
/// @param ptr_         pointer to the array previously allocated with CORE_NEW_ARRAY or CORE_NEW_ARRAY_ALIGN

#define CORE_DELETE_ARRAY(allocator_, ptr_) EA::Allocator::detail::DeleteArray((allocator_),(ptr_))

/// @param allocator_   allocator_ must implement the EA::Allocator::ICoreAllocator interface
/// @param size_        size of allocation
/// @param name_        string to give allocation name
/// @param flags_       allocation flags, see EA::Allocator::ICoreAllocator for a description.
/// @return             Allocated memory
#if CORE_ALLOCATOR_DEBUGPARAMS_ENABLED
	#define CORE_ALLOC(allocator_, size_, name_, flags_) ((allocator_)->AllocDebug((size_), CORE_ALLOCATOR_DEBUGPARAMS((name_)), (flags_)))
#else
	#define CORE_ALLOC(allocator_, size_, name_, flags_) ((allocator_)->Alloc((size_), CORE_ALLOCATOR_DEBUGPARAMS((name_)), (flags_)))
#endif

/// @param allocator_   allocator_ must implement the EA::Allocator::ICoreAllocator interface
/// @param size_        size of allocation
/// @param name_        string to give allocation name
/// @param flags_       allocation flags, see EA::Allocator::ICoreAllocator for a description.
/// @param align_       desired alignment
/// @param alignoffset_ desired offset that requires align_
/// @return             Allocated memory
#if CORE_ALLOCATOR_DEBUGPARAMS_ENABLED
	#define CORE_ALLOC_ALIGN(allocator_, size_, name_, flags_, align_, alignoffset_) ((allocator_)->AllocDebug((size_), CORE_ALLOCATOR_DEBUGPARAMS((name_)), (flags_), (align_), (alignoffset_)))
#else
	#define CORE_ALLOC_ALIGN(allocator_, size_, name_, flags_, align_, alignoffset_) ((allocator_)->Alloc((size_), CORE_ALLOCATOR_DEBUGPARAMS((name_)), (flags_), (align_), (alignoffset_)))
#endif

/// @param allocator_:      allocator_ must implement the EA::Allocator::ICoreAllocator interface
/// @param block_:           pointer to the block to be freed.

#define CORE_FREE(allocator_, block_) ((allocator_)->Free((block_),0))

/// @param allocator_:      allocator_ must implement the EA::Allocator::ICoreAllocator interface
/// @param block_:           pointer to the block to be freed.
/// @param size_:            size of allocation may be used to find the pool

#define CORE_FREE_SIZE(allocator_, block_, size_) ((allocator_)->Free((block_),(size_)))

#endif

