// Copyright 2004-2007 Electronic Arts Inc. All rights reserved.

#ifndef DYNAMICSOA_H
#define DYNAMICSOA_H

#include <PPMalloc/EAStackAllocator.h>


/*    
	Dynamic allocation of small objects. 

	The small object allocator consists of buckets of objects for each size. When a small object is 
	requested, an object is returned by removing it from the corresponding buckets free list. If the bucket 
	is empty, the allocator will request a page from the core allocator, divide the page into objects and 
	place each object into the corresponding buckets free list.

	Each page maintains a structure containing information such as the size and number of the objects stored 
	within that page. A page stores only objects of the same size. Thus, given a pointer to any object, it is 
	possible to calculate the starting address of the corresponding page and therefore the size of the object.

	A BIBOP (Big Bag of Pages) technique is used to encode information of each object. Since all objects on a 
	particular page are of the same size this technique cab be used to avoid allocating object headers. 

	A sample page alloc:
	
		 ---------------------------------------------------
		| P |           |                                   |
		| a | Block     |    D A T A                        |
		| g |  Pointers |     B L O C K S                   |
		| e |           |                                   |
		 ---------------------------------------------------
*/


namespace EA
{
	namespace Allocator
	{
		class DynamicSmallOjbectAllocator
		{
		public:
			static const int kNumBuckets = 5;
			static const int kBlockAlign = 16;
			static const int kMaxBlockSize = 128;
			static const int kPageSize = 4096;

			DynamicSmallOjbectAllocator(EA::Allocator::StackAllocator *allocator = NULL);
			virtual ~DynamicSmallOjbectAllocator();

			void SetAllocator(EA::Allocator::StackAllocator *allocator) { mAllocator = allocator; }

			void Purge();

			void *Alloc(const char *name, size_t size, unsigned int flags, const char *file, int line);
			bool Free(void *ptr);
			bool Contains(void *ptr);

			size_t GetBlockSize(void *ptr);
			
			void Trace();

			size_t GetNumPages();
			size_t GetNumBlocks();
			size_t GetNumBlocksFree();
			size_t GetMemoryUsage();

		protected:
			static void TraceFunction(const char *text, void *context);

		protected:
			struct Page
			{
				Page*    Next;
				size_t    BlockSize;
				size_t  NumBlocks;
				void*    DataPtr;
			};

			struct Block
			{
				Block*    Next;
			};

			Block* mBlockTable[kNumBuckets];
			Page*  mBlockPages;

		private:
			EA::Allocator::StackAllocator *mAllocator;
		};
	}
}

#endif // DYNAMICSOA_H
