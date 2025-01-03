// (c) 2007 Electronic Arts Inc.

#ifndef EA_ALLOCATOR_ICOREALLOCATORMACROHELPERS_H
#define EA_ALLOCATOR_ICOREALLOCATORMACROHELPERS_H

#ifdef _MSC_VER
#pragma once
#endif

/// @brief This header file defines helper methods and functions needed by icoreallocatormacros

#include <EABase/eabase.h>
EA_DISABLE_VC_WARNING(4836 4574)
#include <stddef.h>     // size_t definition
#include <new>          // for placement new and array placement new
EA_RESTORE_VC_WARNING()

namespace EA
{
	namespace Allocator
	{
		namespace detail
		{
			/// @remarks
			/// Distinguisher for our new overload. It's called opNonArrayNew because only the non-array versions of our
			/// new/delete overloads are valid. For arrays users should be using CORE_NEW_ARRAY/CORE_DELETE_ARRAY.

			struct opNonArrayNew { int v; };

			/// @remarks
			/// Destructs and deletes an object created by by an ICoreAllocator operator new overload (or CORE_NEW)
			/// Use CORE_DELETE instead of calling this directly.

			template <typename T>
			inline void DeleteObject(Allocator::ICoreAllocator* allocator, T* object)
			{
				if(object) // As per the C++ standard, deletion of NULL results in a no-op.
				{
					object->~T();
					allocator->Free(object);
				}
			}

			/// @remarks
			/// Creates an array of objects of type T.
			/// Use CORE_NEW_ARRAY instead of calling this directly.

			template <typename T>
			inline T* CreateArray(Allocator::ICoreAllocator* allocator, size_t count, const char* name, unsigned int flags)
			{
				// To do: Use type traits to detect that T is a built-in type and not generate a size_t prefix if so.

				// Make the allocation with kOffset in front to allow us to store count
				size_t* p = (size_t*)allocator->Alloc(kOffset + (sizeof(T) * count), name, flags);

				// put the size_t count in front of the array allocation.
				*p = count;

				//use placement new to construct each object
				T* first = reinterpret_cast<T*>((uintptr_t)p+kOffset);
				T* end = first+count;
				for (T* cur = first; cur < end; cur++)
				{
					new(cur)T();
				}
				return first;
			}

			/// @remarks
			/// Creates an array of objects of type T.
			/// Use CORE_NEW_ARRAY instead of calling this directly.

			template <typename T>
			inline T* CreateArray(Allocator::ICoreAllocator* allocator, size_t count, const ICoreAllocator::DebugParams debugParams, unsigned int flags)
			{
				// To do: Use type traits to detect that T is a built-in type and not generate a size_t prefix if so.

				// Make the allocation with kOffset in front to allow us to store count
				size_t* p = (size_t*)allocator->AllocDebug(kOffset + (sizeof(T) * count), debugParams, flags);

				// put the size_t count in front of the array allocation.
				*p = count;

				//use placement new to construct each object
				T* first = reinterpret_cast<T*>((uintptr_t)p+kOffset);
				T* end = first+count;
				for (T* cur = first; cur < end; cur++)
				{
					new(cur)T();
				}
				return first;
			}

			/// @remarks
			/// Creates an array of objects of type T.
			/// Use CORE_NEW_ARRAY_UNINITIALIZED instead of calling this directly.

			template <typename T>
			inline T* CreateArrayUninitialized(Allocator::ICoreAllocator* allocator, size_t count, const char* name, unsigned int flags)
			{
				// To do: Use type traits to detect that T is a built-in type and not generate a size_t prefix if so.

				// Make the allocation with kOffset in front to allow us to store count
				size_t* p = (size_t*)allocator->Alloc(kOffset + (sizeof(T) * count), name, flags);

				// put the size_t count in front of the array allocation.
				*p = count;

				//use placement new to construct each object
				T* first = reinterpret_cast<T*>((uintptr_t)p+kOffset);
				T* end = first+count;
				for (T* cur = first; cur < end; cur++)
				{
					new(cur)T;
				}
				return first;
			}

			/// @remarks
			/// Creates an array of objects of type T.
			/// Use CORE_NEW_ARRAY_UNINITIALIZED instead of calling this directly.

			template <typename T>
			inline T* CreateArrayUninitialized(Allocator::ICoreAllocator* allocator, size_t count, const ICoreAllocator::DebugParams debugParams, unsigned int flags)
			{
				// To do: Use type traits to detect that T is a built-in type and not generate a size_t prefix if so.

				// Make the allocation with kOffset in front to allow us to store count
				size_t* p = (size_t*)allocator->AllocDebug(kOffset + (sizeof(T) * count), debugParams, flags);

				// put the size_t count in front of the array allocation.
				*p = count;

				//use placement new to construct each object
				T* first = reinterpret_cast<T*>((uintptr_t)p+kOffset);
				T* end = first+count;
				for (T* cur = first; cur < end; cur++)
				{
					new(cur)T;
				}
				return first;
			}

			/// @remarks
			/// Creates an array of objects of type T.
			/// Use CORE_NEW_ARRAY_ALIGN instead of calling this directly

			template <typename T>
			inline T* CreateArray(Allocator::ICoreAllocator* allocator, size_t count, const char* name, unsigned int flags, size_t align)
			{
				// To do: Use type traits to detect that T is a built-in type and not generate a size_t prefix if so.

				// Make the allocation with kOffset in front to allow us to store count
				size_t* p = (size_t*)allocator->Alloc(kOffset + (sizeof(T) * count), name, flags, (unsigned)align, kOffset);

				// put the size_t count in front of the array allocation.
				*p = count;

				//use placement new to construct each object
				T* first = reinterpret_cast<T*>((uintptr_t)p+kOffset);
				T* end = first+count;
				for (T* cur = first; cur < end; cur++)
				{
					new(cur)T();
				}
				return first;
			}

			/// @remarks
			/// Creates an array of objects of type T.
			/// Use CORE_NEW_ARRAY_ALIGN instead of calling this directly

			template <typename T>
			inline T* CreateArray(Allocator::ICoreAllocator* allocator, size_t count, const ICoreAllocator::DebugParams debugParams, unsigned int flags, size_t align)
			{
				// To do: Use type traits to detect that T is a built-in type and not generate a size_t prefix if so.

				// Make the allocation with kOffset in front to allow us to store count
				size_t* p = (size_t*)allocator->AllocDebug(kOffset + (sizeof(T) * count), debugParams, flags, (unsigned)align, kOffset);

				// put the size_t count in front of the array allocation.
				*p = count;

				//use placement new to construct each object
				T* first = reinterpret_cast<T*>((uintptr_t)p+kOffset);
				T* end = first+count;
				for (T* cur = first; cur < end; cur++)
				{
					new(cur)T();
				}
				return first;
			}

			/// @remarks
			/// Creates an array of objects of type T.
			/// Use CORE_NEW_ARRAY_ALIGN_UNINITIALIZED instead of calling this directly

			template <typename T>
			inline T* CreateArrayUninitialized(Allocator::ICoreAllocator* allocator, size_t count, const char* name, unsigned int flags, size_t align)
			{
				// To do: Use type traits to detect that T is a built-in type and not generate a size_t prefix if so.

				// Make the allocation with kOffset in front to allow us to store count
				size_t* p = (size_t*)allocator->Alloc(kOffset + (sizeof(T) * count), name, flags, (unsigned)align, kOffset);

				// put the size_t count in front of the array allocation.
				*p = count;

				//use placement new to construct each object
				T* first = reinterpret_cast<T*>((uintptr_t)p+kOffset);
				T* end = first+count;
				for (T* cur = first; cur < end; cur++)
				{
					new(cur)T;
				}
				return first;
			}

			/// @remarks
			/// Creates an array of objects of type T.
			/// Use CORE_NEW_ARRAY_ALIGN_UNINITIALIZED instead of calling this directly

			template <typename T>
			inline T* CreateArrayUninitialized(Allocator::ICoreAllocator* allocator, size_t count, const ICoreAllocator::DebugParams debugParams, unsigned int flags, size_t align)
			{
				// To do: Use type traits to detect that T is a built-in type and not generate a size_t prefix if so.

				// Make the allocation with kOffset in front to allow us to store count
				size_t* p = (size_t*)allocator->AllocDebug(kOffset + (sizeof(T) * count), debugParams, flags, (unsigned)align, kOffset);

				// put the size_t count in front of the array allocation.
				*p = count;

				//use placement new to construct each object
				T* first = reinterpret_cast<T*>((uintptr_t)p+kOffset);
				T* end = first+count;
				for (T* cur = first; cur < end; cur++)
				{
					new(cur)T;
				}
				return first;
			}


			/// @remarks
			/// Destructs and Deletes an array created by CreateArray.
			/// Use CORE_DELETE_ARRAY instead of calling this directly.

			template <typename T>
			void DeleteArray(Allocator::ICoreAllocator* allocator, T* pArray)
			{
				if(pArray) // As per the C++ standard, deletion of NULL results in a no-op.
				{
					// Destruct objects in reverse order.
					T* arrayEnd = pArray + ((size_t*)((uintptr_t)pArray-kOffset))[0];
					while(arrayEnd-- != pArray)
						arrayEnd->~T();

					allocator->Free(reinterpret_cast<void*>((uintptr_t)pArray-kOffset));
				}
			}
		} // namespace detail
	} // namespace Allocator
} // namespace EA


/// @remarks
/// This overload is provided for use by CORE_NEW* macros, there should be no need for a user to call this overload directly.

inline void* operator new(size_t size, EA::Allocator::detail::opNonArrayNew, EA::Allocator::ICoreAllocator* allocator, const char* name, unsigned int flags)
{
	return allocator->Alloc(size, name, flags);
}

/// @remarks
/// This overload is provided for use by CORE_NEW* macros, there should be no need for a user to call this overload directly.

inline void* operator new(size_t size, EA::Allocator::detail::opNonArrayNew, EA::Allocator::ICoreAllocator* allocator, const EA::Allocator::ICoreAllocator::DebugParams debugParams, unsigned int flags)
{
	return allocator->AllocDebug(size, debugParams, flags);
}

/// @remarks
/// This overload is provided for use by CORE_NEW* macros, there should be no need for a user to call this overload directly.

inline void* operator new(size_t size, EA::Allocator::detail::opNonArrayNew, EA::Allocator::ICoreAllocator* allocator, const char* name, unsigned int flags, size_t align, size_t alignOffset = 0)
{
	return allocator->Alloc(size, name, flags, (unsigned)align, (unsigned)alignOffset);
}

/// @remarks
/// This overload is provided for use by CORE_NEW* macros, there should be no need for a user to call this overload directly.
/// The second size_t argument is compiler specific (for ps3 currently). It represents the compiler derived alignment for alignments greater than ABI specified 16-byte.
/// Future: Should we introduce an assert checking if the compiler derived alignment matches the user specified alignment (7th argument)?

inline void* operator new(size_t size, size_t, EA::Allocator::detail::opNonArrayNew, EA::Allocator::ICoreAllocator* allocator, const char* name, unsigned int flags)
{
	return allocator->Alloc(size, name, flags);
}


inline void* operator new(size_t size, size_t, EA::Allocator::detail::opNonArrayNew, EA::Allocator::ICoreAllocator* allocator, const char* name, unsigned int flags, size_t align, size_t alignOffset = 0)
{
	return allocator->Alloc(size, name, flags, (unsigned)align, (unsigned)alignOffset);
}

/// @remarks
/// This overload is provided for use by CORE_NEW* macros, there should be no need for a user to call this overload directly.

inline void* operator new(size_t size, EA::Allocator::detail::opNonArrayNew, EA::Allocator::ICoreAllocator* allocator, const EA::Allocator::ICoreAllocator::DebugParams debugParams, unsigned int flags, size_t align, size_t alignOffset = 0)
{
	return allocator->AllocDebug(size, debugParams, flags, (unsigned)align, (unsigned)alignOffset);
}

/// @remarks
/// This overload is provided for use by CORE_NEW* macros, there should be no need for a user to call this overload directly.
/// The second size_t argument is compiler specific (for ps3 currently). It represents the compiler derived alignment for alignments greater than ABI specified 16-byte.
/// Future: Should we introduce an assert checking if the compiler derived alignment matches the user specified alignment (7th argument)?

inline void* operator new(size_t size, size_t, EA::Allocator::detail::opNonArrayNew, EA::Allocator::ICoreAllocator* allocator, const EA::Allocator::ICoreAllocator::DebugParams debugParams, unsigned int flags, size_t align, size_t alignOffset = 0)
{
	return allocator->AllocDebug(size, debugParams, flags, (unsigned)align, (unsigned)alignOffset);
}

/// @remarks
/// This overload is provided in case the compiler needs to generate an operator delete call to match our operator new overload due to an exception. There should be no need for a user to call this overload directly.

inline void operator delete(void* p, EA::Allocator::detail::opNonArrayNew, EA::Allocator::ICoreAllocator* allocator, const char* /*name*/, unsigned int /*flags*/)
{
	return allocator->Free(p);
}

/// @remarks
/// This overload is provided in case the compiler needs to generate an operator delete call to match our operator new overload due to an exception. There should be no need for a user to call this overload directly.

inline void operator delete(void* p, EA::Allocator::detail::opNonArrayNew, EA::Allocator::ICoreAllocator* allocator, const EA::Allocator::ICoreAllocator::DebugParams, unsigned int /*flags*/)
{
	return allocator->Free(p);
}

/// @remarks
/// This overload is provided in case the compiler needs to generate an operator delete call to match our operator new overload due to an exception. There should be no need for a user to call this overload directly.

inline void operator delete(void* p, EA::Allocator::detail::opNonArrayNew, EA::Allocator::ICoreAllocator* allocator, const char* /*name*/, unsigned int /*flags*/, size_t /*align*/, size_t /*alignOffset*/)
{
	return allocator->Free(p);
}

/// @remarks
/// This overload is provided in case the compiler needs to generate an operator delete call to match our operator new overload due to an exception. There should be no need for a user to call this overload directly.

inline void operator delete(void* p, EA::Allocator::detail::opNonArrayNew, EA::Allocator::ICoreAllocator* allocator, const EA::Allocator::ICoreAllocator::DebugParams, unsigned int /*flags*/, size_t /*align*/, size_t /*alignOffset*/)
{
	return allocator->Free(p);
}

/// @remarks
/// DO NOT CALL THIS METHOD or try to use CORE_NEW to create an array (such as 'widgetArray = CORE_NEW(allocator, "myarray", 0) Widget[5];').
/// Doing so will cause a link error. Use CORE_NEW_ARRAY and CORE_DELETE_ARRAY instead.

void* operator new[](size_t size, EA::Allocator::detail::opNonArrayNew, EA::Allocator::ICoreAllocator* allocator, const char* name, unsigned int flags);

/// @remarks
/// DO NOT CALL THIS METHOD or try to use CORE_NEW to create an array (such as 'widgetArray = CORE_NEW(allocator, "myarray", 0) Widget[5];').
/// Doing so will cause a link error. Use CORE_NEW_ARRAY and CORE_DELETE_ARRAY instead.

void* operator new[](size_t size, EA::Allocator::detail::opNonArrayNew, EA::Allocator::ICoreAllocator* allocator, const EA::Allocator::ICoreAllocator::DebugParams debugParams, unsigned int flags);

/// @remarks
/// DO NOT CALL THIS METHOD or try to use CORE_NEW to create an array (such as 'widgetArray = CORE_NEW(allocator, "myarray", 0) Widget[5];').
/// Doing so will cause a link error. Use CORE_NEW_ARRAY and CORE_DELETE_ARRAY instead.

void* operator new[](size_t size, EA::Allocator::detail::opNonArrayNew, EA::Allocator::ICoreAllocator* allocator, const char* name, unsigned int flags, size_t align, size_t alignOffset = 0);

/// @remarks
/// DO NOT CALL THIS METHOD or try to use CORE_NEW to create an array (such as 'widgetArray = CORE_NEW(allocator, "myarray", 0) Widget[5];').
/// Doing so will cause a link error. Use CORE_NEW_ARRAY and CORE_DELETE_ARRAY instead.

void* operator new[](size_t size, EA::Allocator::detail::opNonArrayNew, EA::Allocator::ICoreAllocator* allocator, const EA::Allocator::ICoreAllocator::DebugParams debugParams, unsigned int flags, size_t align, size_t alignOffset = 0);

/// @remarks
/// DO NOT CALL THIS METHOD, doing so will cause a link error. Use CORE_DELETE_ARRAY instead.

void operator delete[](void* p, EA::Allocator::detail::opNonArrayNew, EA::Allocator::ICoreAllocator* allocator, const char* name, unsigned int flags);

/// @remarks
/// DO NOT CALL THIS METHOD, doing so will cause a link error. Use CORE_DELETE_ARRAY instead.

void operator delete[](void* p, EA::Allocator::detail::opNonArrayNew, EA::Allocator::ICoreAllocator* allocator, const EA::Allocator::ICoreAllocator::DebugParams, unsigned int flags);

/// @remarks
/// DO NOT CALL THIS METHOD, doing so will cause a link error. Use CORE_DELETE_ARRAY instead.

void operator delete[](void* p, EA::Allocator::detail::opNonArrayNew, EA::Allocator::ICoreAllocator* allocator, const char* name, unsigned int flags, size_t align, size_t alignOffset);

/// @remarks
/// DO NOT CALL THIS METHOD, doing so will cause a link error. Use CORE_DELETE_ARRAY instead.

void operator delete[](void* p, EA::Allocator::detail::opNonArrayNew, EA::Allocator::ICoreAllocator* allocator, const EA::Allocator::ICoreAllocator::DebugParams, unsigned int flags, size_t align, size_t alignOffset);


#endif

