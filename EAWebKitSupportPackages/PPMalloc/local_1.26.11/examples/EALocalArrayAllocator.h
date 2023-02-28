///////////////////////////////////////////////////////////////////////////////
// EALocalArrayAllocator
//
// Copyright (c) 2004, Electronic Arts Inc. All rights reserved.
// By Paul Pedriana, Maxis
//
// Implements a portable allocator for memory on the C++ stack. Similar to 
// alloca but portable and with no limits on allocation size. However, 
// the alloca function is more efficient and thus is superior in cases 
// where it can be safely used.
///////////////////////////////////////////////////////////////////////////////


#ifndef PPMALLOC_EALOCALARRAYALLOCATOR_H
#define PPMALLOC_EALOCALARRAYALLOCATOR_H


#include <new>


namespace EA
{
   namespace Allocator
   {
	  ///////////////////////////////////////////////////////////////////////////////
	  /// class LocalArrayAllocator
	  ///
	  /// Brief
	  /// Allocates an array of objects quickly for cases when you don't know the size
	  /// of the array beforehand. This is similar to the alloca function but has no
	  /// limit on the maximum amount of memory allocated.
	  ///
	  /// Detail
	  /// This templated class allows you to allocate a dynamically sized temporary
	  /// block of memory local to a function in a way that causes it to use some
	  /// stack space instead of using the (very slow) malloc function. You don't
	  /// want to call malloc/new to allocate temporary blocks of memory when speed
	  /// is important. This class is only practical if you are allocating arrays of
	  /// objects (especially smaller objects). If you have a class whose size is
	  /// 40K, you aren't going to be able to put even one of them on the stack.
	  /// If you don't need an array but instead just need a single object, then you
	  /// can simply create your object on the stack and be done with it. However,
	  /// there are some times when you need a temporary array of objects (or just
	  /// an array of chars or pointers) but don't know the size beforehand. That's
	  /// where this class is useful.
	  ///
	  /// Notes
	  /// An alternative implementation would use macros and the nonportable but 
	  /// often found 'alloca' function. You can't use alloca with a template like
	  /// below because unless the template was guaranteed to be inline the memory
	  /// would become 'freed' before the allocation function returned.
	  ///
	  /// Parameters:
	  ///    The class takes three templated parameters: T, nStackCount, bConstruct.
	  ///      T           -- The type of object being allocated. Examples include char,
	  ///                     std::string, int*, Point3D*.
	  ///      nStackCount -- How many items to allocate on the stack before switching
	  ///                     to a heap allocation. Note that if you want to write
	  ///                     portable code, you need to use no more than about 2K
	  ///                     of stack space. So make sure nStackCount*sizeof(T) < 2048.
	  ///      bConstruct  -- This is normally 1, but setting it to 0 is a hint to the
	  ///                     compiler that you are constructing a type that has no
	  ///                     constructor. Testing showed that the VC++ compiler wasn't
	  ///                     completely able to optimize on its own. In particular, it
	  ///                     was smart enough to remove the while loop, but not smart
	  ///                     enough to remore the pTArrayEnd assignment.
	  ///
	  /// Benchmarks:
	  ///    Testing one one machine showed that allocating and freeing a block of
	  ///    2048 bytes via malloc/free took 8700 and 8400 clock ticks respectively.
	  ///    Using this stack allocator to do the same thing took 450 and 375 clock
	  ///    ticks respectively.
	  ///
	  /// Usage and examples:
	  ///    Since we are using a local allocator and not overloading global or class
	  ///    operator new, you have to directly call LocalArrayAllocator::New instead
	  ///    of operator new. So instead of saying:
	  ///       SomeStruct* pStructArray = new SomeStruct[13];
	  ///    you say:
	  ///       SomeStruct* pStructArray = stackAllocator.New(13);
	  ///
	  ///    void Example(int nSize){
	  ///       LocalArrayAllocator<char, 512> stackAllocator;           // Create an instance
	  ///       char* pArray = LocalArrayAllocator.New(nSize);           // Allocate memory, it will auto-freed.
	  ///       memset(pArray, 0, nSize*sizeof(char));                   // Do something with the memory.
	  ///    }
	  ///
	  ///    void Example(int nSize){
	  ///       LocalArrayAllocator<int*, 512, 0> LocalArrayAllocator;   // Create an instance. We use the 'construct' hint feature here.
	  ///       int** pArray = LocalArrayAllocator.New(nSize);           // Allocate memory.
	  ///       memset(pArray, 0, nSize*sizeof(int*));                   // Do something with the memory.
	  ///       LocalArrayAllocator.Delete(pArray);                      // In this example, we explicity free the memory.
	  ///    }
	  ///
	  ///    void Example(int nSize){
	  ///       LocalArrayAllocator<Point3D, 200> LocalArrayAllocator;   // Create an instance
	  ///       Point3D* pPointArray = LocalArrayAllocator.New(nSize);   // Allocate memory, it will auto-freed.
	  ///       pRectArray[0].SetRect(0,0,1,1);                          // Do something with the memory.
	  ///    }
	  ///
	  ///    void Example(int nSize){
	  ///       LocalArrayAllocator<char, 200> LocalArrayAllocator;      // Create an instance
	  ///       char* pArray  = LocalArrayAllocator.New(nSize);          // Allocate memory.
	  ///       char* pArray2 = LocalArrayAllocator.New(nSize);          // Allocate some additional memory.
	  ///       memset(pArray, 0, nSize*sizeof(char));                   // Do something with the memory.
	  ///       LocalArrayAllocator.Delete(pArray);                      // Delete the memory
	  ///       pArray = LocalArrayAllocator.New(nSize*2);               // Allocate some new memory. We'll let the allocator free it.
	  ///       memset(pArray, 0, nSize*2*sizeof(char));                 // Do something with the new memory.
	  ///       LocalArrayAllocator.Delete(pArray2);                     // Delete the additional memory.
	  ///    }
	  ///////////////////////////////////////////////////////////////////////////////

	  template<class T, int nStackCount, int bConstruct = 1>
	  class LocalArrayAllocator
	  {
	  public:
		 LocalArrayAllocator() 
			: mnAllocCount(-1), mpTHeap(NULL){}

		 /// ~LocalArrayAllocator
		 /// Frees any memory that may have been allocated.
		~LocalArrayAllocator(){
			if(mnAllocCount != -1){ //If there is anything to do...
			   if(mpTHeap)
				  delete mpTHeap;
			   else{
				  if(bConstruct){ // Since this constant, the comparison gets optimized away.
					 T* pTArray = (T*)mTArray;
					 const T* const pTArrayEnd = pTArray + mnAllocCount;
					 while(pTArray < pTArrayEnd){
						pTArray->~T();
						++pTArray;
					 }
				  }
			   }
			}
		 }

		 /// New
		 /// @brief Acts like C++ array operator new.
		 /// Returns nCount constructed items of type T. The input nCount can 
		 /// be a value greater than template parameter nStackCount, though 
		 /// performance is highest when nCount is <= nStackCount.
		 T* New(unsigned nCount){
			if(mnAllocCount == -1){
			   mnAllocCount = nCount;
			   if(nCount < nStackCount){ // If the request is small enough to come from the stack...
				  // We call the constructors of all the objects here.
				  if(bConstruct){ //Since this constant, the comparison gets optimized away.
					 T* pTArray = (T*)mTArray;
					 const T* const pTArrayEnd = pTArray + nCount;
					 while(pTArray < pTArrayEnd){
						new(pTArray)T; // Use the placement operator new. This simply calls the constructor
						++pTArray;     // of T with 'this' set to the input address. Note that we don't put
					 }                 // a '()' after the T this is because () causes trivial types like int
				  }                    // and class* to be assigned zero/NULL. We don't want that.
				  return (T*)mTArray;
			   } // Else the request is too big. So let's use (the slower) operator new.
			   return (mpTHeap = new T[nCount]); //The compiler will call the constructors here.
			} // Else we are being used. Let's be nice and allocate something anyway.
			return new T[nCount];
		 }

		 /// Delete
		 /// @brief Acts like C++ array operator delete.
		 /// Frees memory allocated with operator new.
		 void Delete(T* pT){
			if(pT == (T*)mTArray){  // If the allocation came from our stack...
			   if(bConstruct){      // Since this constant, the comparison gets optimized away.
				  T* pTArray = (T*)mTArray;
				  const T* const pTArrayEnd = pTArray + mnAllocCount;
				  while(pTArray < pTArrayEnd){
					 pTArray->~T(); // Call the destructor on the object directly.
					 ++pTArray;
				  }
			   }
			   mnAllocCount = -1;
			}
			else if(pT == mpTHeap){ // If the allocation came from our heap...
			   delete[] mpTHeap;    // The compiler will call the destructors here.
			   mpTHeap      = NULL; // We clear these out so that we can possibly
			   mnAllocCount = -1;   //  use the allocator again.
			}
			else // Else the allocation came from the external heap.
			   delete[] pT;
		 }

	  protected:
		 int  mnAllocCount;                     /// Count of objects allocated. -1 means that nothing is allocated. We don't use zero because zero is a legal allocation count in C++.
		 T*   mpTHeap;                          /// This is normally NULL, but gets used of the allocation request is too high.
		 char mTArray[nStackCount * sizeof(T)]; /// This is our stack memory.
	  };

   } // namespace Allocator

} // namespace EA



#endif // PPMALLOC_EALOCALARRAYALLOCATOR_H















