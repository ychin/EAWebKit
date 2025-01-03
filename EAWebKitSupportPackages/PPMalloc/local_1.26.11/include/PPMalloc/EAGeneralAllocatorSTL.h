///////////////////////////////////////////////////////////////////////////////
// EAGeneralAllocatorSTL
//
// Copyright (c) 2004, Electronic Arts. All Rights Reserved.
// By Paul Pedriana, Maxis
//
// This header defines an STL-compatible allocator 
///////////////////////////////////////////////////////////////////////////////


#ifndef PPMALLOC_EAGENERALALLOCATORSTL_H
#define PPMALLOC_EAGENERALALLOCATORSTL_H


#include <PPMalloc/internal/config.h>
#include <PPMalloc/EAGeneralAllocator.h>
#include <stddef.h>  // For NULL, size_t
#include <memory>    // For std::allocator


namespace EA
{
	namespace Allocator
	{
		extern PPM_API GeneralAllocator* gpEAGeneralAllocator;

		///////////////////////////////////////////////////////////////////////////////
		/// GeneralAllocatorSTL
		///
		/// This class is used for creating STL containers that use your own specified.
		/// memory allocation and don't rely on standard library malloc, free, new, delete.
		/// A primary use of this class is to implement memory management and measurement
		/// itself, without interfering with the memory that is being managed or measured.
		///
		/// Example usage:
		///     std::vector<int, GeneralAllocatorSTL<int> > intArray;
		///
		/// Additional example usage:
		///     typedef vector<int, GeneralAllocatorSTL<int> >                                         IntArray;
		///     typedef list<int, GeneralAllocatorSTL<int> >                                           IntList;
		///     typedef deque<int, GeneralAllocatorSTL<int> >                                          IntDequeue;
		///     typedef queue<int, deque<int, GeneralAllocatorSTL<int> > >                             IntQueue;
		///     typedef priority_queue<int, vector<int, GeneralAllocatorSTL<int> > >                   IntPriorityQueue;
		///     typedef stack<int, deque<int, GeneralAllocatorSTL<int> > >                             IntStack;
		///     typedef map<int, long, less<int>, GeneralAllocatorSTL<int> >                           IntMap;
		///     typedef multimap<int, long, less<int>, GeneralAllocatorSTL<int> >                      IntMultiMap;
		///     typedef set<int, less<int>, GeneralAllocatorSTL<int> >                                 IntSet;
		///     typedef multiset<int, less<int>, GeneralAllocatorSTL<int> >                            IntMultiSet;
		///     typedef hash_map<int, long, hash<int>, equal_to<int>, GeneralAllocatorSTL<int> >       IntHashMap;
		///     typedef hash_multimap<int, long, hash<int>, equal_to<int>, GeneralAllocatorSTL<int> >  IntHashMultiMap;
		///     typedef hash_set<int, hash<int>, equal_to<int>, GeneralAllocatorSTL<int> >             IntHashSet;
		///     typedef hash_multiset<int, hash<int>, equal_to<int>, GeneralAllocatorSTL<int> >        IntHashMultiSet;
		///     typedef basic_string<char, char_traits<char>, GeneralAllocatorSTL<char> >              CharString;
		///
		template <class T>
		class GeneralAllocatorSTL
		{
		public:
			typedef size_t     size_type;
			typedef ptrdiff_t  difference_type;
			typedef T*         pointer;
			typedef const T*   const_pointer;
			typedef T&         reference;
			typedef const T&   const_reference;
			typedef T          value_type;

			GeneralAllocatorSTL(){}
			GeneralAllocatorSTL(const GeneralAllocatorSTL<T>&) {}
		   ~GeneralAllocatorSTL() {}
			template <class T1> GeneralAllocatorSTL(const GeneralAllocatorSTL<T1>&){}
			template <class T1> struct rebind { typedef GeneralAllocatorSTL<T1> other; };

			T*        address(T& t) const                       { return &t; }
			const T*  address(const T& t) const                 { return &t; }
			T*        allocate(size_type n, const void* = NULL) { return (T*)gpEAGeneralAllocator->Malloc(n * sizeof(T)); }
			void      deallocate(T* p, size_type)               { gpEAGeneralAllocator->Free(p); }
			void      construct(T* p, const T& value)           { new(p) T(value); }
			void      destroy(T* p)                             { p->~T(); }
			size_type max_size() const                          { return size_type(-1) / sizeof(T); }
		};

		template<class T>
		inline bool operator==(const GeneralAllocatorSTL<T>&, const GeneralAllocatorSTL<T>&) { return true;  }

		template<class T>
		inline bool operator!=(const GeneralAllocatorSTL<T>&, const GeneralAllocatorSTL<T>&) { return false; }

		template<> class GeneralAllocatorSTL<void>
		{
		public:
			typedef size_t      size_type;
			typedef ptrdiff_t   difference_type;
			typedef void*       pointer;
			typedef const void* const_pointer;
			typedef void        value_type;

			template<class T1> struct rebind { typedef GeneralAllocatorSTL<T1> other; };
		};

		#if defined(_STLPORT_VERSION) && !defined(_STLP_MEMBER_TEMPLATE_CLASSES)
			// The __stl_alloc_rebind and __stl_alloc_create functions below are there to support
			// STLPort under VC++, due to limitations in the VC++ compiler up to and including
			// VC7.1. See the following document for a little information about this:
			//    http://www.codeguru.com/Cpp/Cpp/cpp_mfc/stl/article.php/c4079/
			template <class T1, class T2>
				inline GeneralAllocatorSTL<T2>& __stl_alloc_rebind(GeneralAllocatorSTL<T1>& a, const T2*)     { return (GeneralAllocatorSTL<T2>&)(a); }
			template <class T1, class T2>
				inline GeneralAllocatorSTL<T2>  __stl_alloc_create(const GeneralAllocatorSTL<T1>&, const T2*) { return GeneralAllocatorSTL<T2>(); }
		#endif

	} // namespace Allocator

} // namespace EA


#endif // PPMALLOC_EAGENERALALLOCATORSTL_H













