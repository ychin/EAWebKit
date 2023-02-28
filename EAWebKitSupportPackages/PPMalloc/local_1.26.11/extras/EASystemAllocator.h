///////////////////////////////////////////////////////////////////////////////
// EASystemAllocator
//
// Copyright (c) 2004, Electronic Arts. All Rights Reserved.
// By Paul Pedriana, Maxis
//
// Provides a basic allocator for retrieving memory from the "system."
// By the term "system," we mean the operating system or kernel environment
// for the current application. Typically, applications request memory from
// some higher level system and carve it up for their own use. Even the 
// built-in C malloc function does this. What we provide here is a basic
// mechanism for arbitrarily requesting memory from the system. This is 
// useful for implementing your own malloc and using this system memory
// as the pool of core memory for your implementation. System allocators
// are not intended to be high performance allocators.
///////////////////////////////////////////////////////////////////////////////


#ifndef PPMALLOC_EASYSTEMALLOCATOR_H
#define PPMALLOC_EASYSTEMALLOCATOR_H


#include <PPMalloc/internal/config.h>
#include <stddef.h>  // For NULL, size_t


namespace EA
{
	namespace Allocator
	{
		// Some systems need to be initialized with a pre-existing block, some don't.
		void  SystemAllocInit();
		void  SystemAllocShutdown();

		void* SystemAlloc(size_t n);
		void  SystemFree(void* p);
		void* SystemRealloc(void* p, size_t n);


		///////////////////////////////////////////////////////////////////////////////
		/// SystemSTLAllocator
		///
		/// This class is used for creating STL containers that directly use system-provided
		/// memory allocation and don't rely on standard library malloc, free, new, delete.
		/// A primary use of this class is to implement memory management and measurement
		/// itself, without interfering with the memory that is being managed or measured.
		///
		/// Example usage:
		///     SystemAllocInit();
		///     std::vector<int, SystemSTLAllocator<int> > IntArray;
		///     SystemAllocShutdown();
		///
		/// Additional example usage:
		///     typedef vector<int, SystemSTLAllocator<int> >                                         IntArray;
		///     typedef list<int, SystemSTLAllocator<int> >                                           IntList;
		///     typedef deque<int, SystemSTLAllocator<int> >                                          IntDequeue;
		///     typedef queue<int, deque<int, SystemSTLAllocator<int> > >                             IntQueue;
		///     typedef priority_queue<int, vector<int, SystemSTLAllocator<int> > >                   IntPriorityQueue;
		///     typedef stack<int, deque<int, SystemSTLAllocator<int> > >                             IntStack;
		///     typedef map<int, long, less<int>, SystemSTLAllocator<int> >                           IntMap;
		///     typedef multimap<int, long, less<int>, SystemSTLAllocator<int> >                      IntMultiMap;
		///     typedef set<int, less<int>, SystemSTLAllocator<int> >                                 IntSet;
		///     typedef multiset<int, less<int>, SystemSTLAllocator<int> >                            IntMultiSet;
		///     typedef hash_map<int, long, hash<int>, equal_to<int>, SystemSTLAllocator<int> >       IntHashMap;
		///     typedef hash_multimap<int, long, hash<int>, equal_to<int>, SystemSTLAllocator<int> >  IntHashMultiMap;
		///     typedef hash_set<int, hash<int>, equal_to<int>, SystemSTLAllocator<int> >             IntHashSet;
		///     typedef hash_multiset<int, hash<int>, equal_to<int>, SystemSTLAllocator<int> >        IntHashMultiSet;
		///     typedef basic_string<char, char_traits<char>, SystemSTLAllocator<char> >              CharString;
		///
		template <class T>
		class SystemSTLAllocator
		{
		public:
			typedef size_t     size_type;
			typedef ptrdiff_t  difference_type;
			typedef T*         pointer;
			typedef const T*   const_pointer;
			typedef T&         reference;
			typedef const T&   const_reference;
			typedef T          value_type;

			SystemSTLAllocator(){}
			SystemSTLAllocator(const SystemSTLAllocator<T>&) {}
		   ~SystemSTLAllocator() {}
			template <class T1> SystemSTLAllocator(const SystemSTLAllocator<T1>&){}
			template <class T1> struct rebind { typedef SystemSTLAllocator<T1> other; };

			T*          address(T& t) const                       { return &t; }
			const T*  address(const T& t) const                   { return &t; }
			T*          allocate(size_type n, const void* = NULL) { return (T*)SystemAlloc(n * sizeof(T)); }
			void        deallocate(T* p, size_type)               { SystemFree(p); }
			void        construct(T* p, const T& value)           { new(p) T(value); }
			void        destroy(T* p)                             { p->~T(); }
			size_type max_size() const                            { return size_type(-1) / sizeof(T); }
		};

		template<class T>
		inline bool operator==(const SystemSTLAllocator<T>&, const SystemSTLAllocator<T>&) { return true;  }

		template<class T>
		inline bool operator!=(const SystemSTLAllocator<T>&, const SystemSTLAllocator<T>&) { return false; }

		template<> class SystemSTLAllocator<void>
		{
		public:
			typedef size_t      size_type;
			typedef ptrdiff_t   difference_type;
			typedef void*       pointer;
			typedef const void* const_pointer;
			typedef void        value_type;

			template<class T1> struct rebind { typedef SystemSTLAllocator<T1> other; };
		};

		#if defined(_STLPORT_VERSION) && !defined(_STLP_MEMBER_TEMPLATE_CLASSES)
			// The __stl_alloc_rebind and __stl_alloc_create functions below are there to support
			// STLPort under VC++, due to limitations in the VC++ compiler up to and including
			// VC7.1. See the following document for a little information about this:
			//    http://www.codeguru.com/Cpp/Cpp/cpp_mfc/stl/article.php/c4079/
			template <class T1, class T2>
				inline SystemSTLAllocator<T2>& __stl_alloc_rebind(SystemSTLAllocator<T1>& a, const T2*)     { return (SystemSTLAllocator<T2>&)(a); }
			template <class T1, class T2>
				inline SystemSTLAllocator<T2>  __stl_alloc_create(const SystemSTLAllocator<T1>&, const T2*) { return SystemSTLAllocator<T2>(); }
		#endif

	} // namespace Allocator

} // namepspace EA







#if defined(EA_PLATFORM_WINDOWS)
	#pragma warning(push, 0)
	#include <Windows.h>
	#pragma warning(pop)
#else
	#include <stdlib.h>
#endif

namespace EA
{
	namespace Allocator
	{
		#if defined(EA_PLATFORM_WINDOWS)
			// Discussion
			//
			// Consider if these Win32-based platforms should be using VirtualAlloc instead of 
			// HeapAlloc. VirtualAlloc is slower and requires allocations to be in page-sized 
			// increments but has the advantage of being able to use memory mapping.
			// HeapAlloc, on the other hand, is faster, can allocate any sized block, but gets
			// its memory from an existing heap (which itself was allocated by VirtualAlloc).
			// Most likely we are better off using HeapAlloc. The primary advantage of VirtualAlloc
			// would be it direct use of virtual memory. But if HeapAlloc doesn't have space it 
			// too will internally resort to VirtualAlloc to provide more pool space for the allocation.
			// VirtualAlloc is best for standalone large allocations.
			//
			// Another topic of discussion is XBox 'debug memory' availability. XBox and XBox2 have
			// a DmAllocatePool function which is simply a malloc function which gets its memory from
			// the upper extra memory available on XBox development machines. Is there a useful and 
			// clean way to use this function? Or should we just keep the system symmetrical and 
			// ignore it and let the regular allocations use the high memory as they exhaust low
			// memory in debug builds?
			//
			inline void SystemAllocInit()
			{
				// Not needed under this platform.
			}

			inline void SystemAllocShutdown()
			{
				// Not needed under this platform.
			}

			inline void* SystemAlloc(size_t n)
			{
				static HANDLE hProcessHeap = GetProcessHeap();
				return HeapAlloc(hProcessHeap, 0, (DWORD)n);
			}

			inline void SystemFree(void* p)
			{
				static HANDLE hProcessHeap = GetProcessHeap();
				HeapFree(hProcessHeap, 0, p);
			}

			inline void* SystemRealloc(void* pAlloc, size_t nNewBytes)
			{
				static HANDLE hProcessHeap = GetProcessHeap();
				return HeapReAlloc(hProcessHeap, 0, pAlloc, nNewBytes);
			}

		#else
			// Todo: Implement this for other platforms as needed.

			inline void SystemAllocInit()
			{
				// Empty.
			}

			inline void SystemAllocShutdown()
			{
				// Empty.
			}

			inline void* SystemAlloc(size_t n)
			{
				return malloc(n);
			}

			inline void SystemFree(void* p)
			{
				return free(p);
			}

			inline void* SystemRealloc(void* p, size_t nNewBytes)
			{
				return realloc(p, nNewBytes);
			}

		#endif 

	} // namespace Allocator

} // namepspace EA



#endif // PPMALLOC_EASYSTEMALLOCATOR_H














