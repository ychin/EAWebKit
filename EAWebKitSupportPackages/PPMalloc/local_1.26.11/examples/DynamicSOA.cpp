// Copyright 2004-2007 Electronic Arts Inc. All rights reserved.

#include "DynamicSOA.h"

namespace EA
{
	namespace Allocator
	{
		static const int kBucketBlockSize[DynamicSmallOjbectAllocator::kNumBuckets] = { 
			16, 32, 48, 64, 128
		};

		static int kBucketBlockIndexTable[9] = { 
			0, 0,                    // 16 bytes
			1,                        // 32 bytes
			2,                        // 48 bytes
			3,                         // 64 bytes
			4, 4, 4, 4                // 128 bytes
		};

		DynamicSmallOjbectAllocator::DynamicSmallOjbectAllocator(EA::Allocator::StackAllocator *allocator /*=NULL*/) :
			mAllocator(allocator),
			mBlockPages(NULL)
		{
			for(int i=0; i<kNumBuckets; i++)
				mBlockTable[i] = NULL;
		}

		DynamicSmallOjbectAllocator::~DynamicSmallOjbectAllocator()
		{
			dbAssert(mBlockPages);
		}

		void DynamicSmallOjbectAllocator::Purge()
		{
			Page *page = mBlockPages;
			while(page)
			{
				mAllocator->Free(page);
				page = page->Next;
			}
		}

		void *DynamicSmallOjbectAllocator::Alloc(const char *name, size_t size, unsigned int flags, const char *file, int line)
		{
			if (!mAllocator)
				return NULL;

			if (size == 0) 
				return NULL;

			size = ((size + (kBlockAlign - 1)) & ~(kBlockAlign - 1));
			
			if (size > kMaxBlockSize)
				return NULL;
			
			int i = kBucketBlockIndexTable[ (int)stmath::fpu::Ceil((float)size / (float)kBlockAlign) ];
			Block *block = mBlockTable[i];
			char *dataptr = NULL;

			if (block == NULL)
			{
				// number of possible aligned blocks that can be allocated from a single page
				size_t blocksize = ((kBucketBlockSize[i] + (kBlockAlign - 1)) & ~(kBlockAlign - 1));
				int numblocks = (kPageSize - sizeof(Page)) / (blocksize + sizeof(Block));
				dbAssert(numblocks);

				// page alloc
				Page *page = (Page *)( mAllocator->MallocAligned(kPageSize, kPageSize, 0, false) );
				if (page)
				{
					page->BlockSize = kBucketBlockSize[i];
					page->NumBlocks = numblocks;
					page->DataPtr = (void *)(((uintptr_t)page + sizeof(Page) + (numblocks * sizeof(Block)) + (kBlockAlign - 1)) & ~(kBlockAlign - 1));
					page->Next = mBlockPages;
					mBlockPages = page;

					// block table free list pointer
					block = (Block *)((char *)page + sizeof(Page) + sizeof(Block));
					mBlockTable[i] = block;

					// ith blocks
					for(int k=0; k<numblocks-2; k++)
					{
						Block *next = (Block *)((char *)block + sizeof(Block));
						block->Next = next;
						block = next;
					}

					// nth and 1st blocks
					block->Next = NULL;
					block = (Block *)((char *)page + sizeof(Page));
					block->Next = NULL;

					// data ptr
					dataptr = (char *)page->DataPtr;
				}
				else
				{
					dbPrintf("DynamicSmallOjbectAllocator -- Stack Overflow\n");
				}
			}
			else
			{
				// block page ptr
				Page *page = (Page *)((((uintptr_t)block + (kPageSize - 1)) & ~(kPageSize - 1)) - kPageSize);

				// block index
				int index = ((uintptr_t)block - ((uintptr_t)page + sizeof(Page))) / sizeof(Block);

				// block data ptr
				dataptr = (char *)((char *)page->DataPtr + (index * page->BlockSize));

				// update free list
				mBlockTable[i] = block->Next;
				block->Next = NULL;
			}

			return dataptr;
		}

		bool DynamicSmallOjbectAllocator::Free(void *ptr)
		{
			if (ptr == NULL)
				return false;

			// pointer to page alloc
			Page *page = (Page *)((((uintptr_t)ptr + (kPageSize - 1)) & ~(kPageSize - 1)) - kPageSize);

			// validate range
			if (ptr < ((char *)page) || ptr >= ((char *)page + kPageSize))
				return false;

			// data index
			int index = (((uintptr_t)ptr - (uintptr_t)page->DataPtr) / page->BlockSize);

			// block pointer
			Block *block = (Block *)((char *)page + sizeof(Page) + (index * sizeof(Block)));

			// update free list
			int i = kBucketBlockIndexTable[ (int)stmath::fpu::Ceil((float)page->BlockSize / (float)kBlockAlign) ];
			block->Next = mBlockTable[i];
			mBlockTable[i] = block;

			return true;
		}

		size_t DynamicSmallOjbectAllocator::GetBlockSize(void *p)
		{
			if (p == NULL) 
				return 0;

			// pointer to page alloc
			Page *page = (Page *)((((uintptr_t)p + (kPageSize - 1)) & ~(kPageSize - 1)) - kPageSize);

			if (p < ((char *)page) || p >= ((char *)page + kPageSize))
				return 0;

			return page->BlockSize;
		}

		bool DynamicSmallOjbectAllocator::Contains(void *ptr)
		{
			if (ptr == NULL)
				return false;

			Page *page = mBlockPages;
			while(page)
			{
				if (ptr >= ((char *)page) && ptr < ((char *)page + kPageSize))
					return true;
				page = page->Next;
			}
			return false;
		}

		/* static */ void DynamicSmallOjbectAllocator::TraceFunction(const char *text, void *context)
		{
			dbPrintf("%s", text);
		}

		void DynamicSmallOjbectAllocator::Trace()
		{
			Page *p = mBlockPages;
			while (p)
			{
				char buffer[2048];
				rw::core::stdc::MemFill32(buffer, 0, 2048);
				
				rw::core::stdc::Sprintf(buffer, "addr: 0x%08x\tpage:\t%10u\tblock:\t%8u\tcount:\t%6u\n", 
					p->DataPtr, 
					kPageSize,
					p->BlockSize,
					p->NumBlocks);

				TraceFunction(buffer, NULL);

				p = p->Next;
			}
		}

		size_t DynamicSmallOjbectAllocator::GetNumPages()
		{
			size_t n = 0;

			Page *p = mBlockPages;
			while (p)
			{
				++n;
				p = p->Next;
			}

			return n;
		}

		size_t DynamicSmallOjbectAllocator::GetNumBlocks()
		{
			size_t n = 0;

			Page *p = mBlockPages;
			while (p)
			{
				n += p->NumBlocks;
				p = p->Next;
			}

			return n;
		}

		size_t DynamicSmallOjbectAllocator::GetNumBlocksFree()
		{
			size_t n = 0;
			Block *b = 0;

			for (int i=0; i<kNumBuckets; i++)
			{
				b = mBlockTable[i];
				while (b)
				{
					++n;
					b = b->Next;
				}
			}
			return n;
		}

		size_t DynamicSmallOjbectAllocator::GetMemoryUsage()
		{
			return (GetNumPages() * kPageSize);
		}
	}
}
