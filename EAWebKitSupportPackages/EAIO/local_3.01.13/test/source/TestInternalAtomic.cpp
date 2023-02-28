///////////////////////////////////////////////////////////////////////////////
//
// Copyright (c) 2019 Electronic Arts. All rights reserved.
///////////////////////////////////////////////////////////////////////////////


#include <coreallocator/icoreallocator_interface.h>
#include <EABase/eabase.h>
#include <EAIO/EAAsyncStream.h>
#include <EAIO/EAFileStream.h>
#include <EAIO/EAStreamMemory.h>
#include <EAIOTest/EAIO_Test.h>
#include <EAStdC/EAMemory.h>
#include <EASTL/vector.h>
#include <EATest/EATest.h>
#include <eathread/eathread_atomic.h>
#include <eathread/eathread_sync.h>
#include <eathread/eathread_thread.h>
#include <MemoryMan/CoreAllocatorNew.h>

#include <EAIO/internal/EAAsyncExecutorImpl.h>

int TestTaggedPointer()
{
	using namespace EA::IO;

	int nErrorCount = 0;

	int i = 1;
	int* ptr = &i;

	using TaggedPointerType = Internal::TaggedPointer<alignof(int)>;

	{
		TaggedPointerType tp(ptr, 0);

		{
			uint32_t ret = tp.GetCounterRaw();

			EATEST_VERIFY(ret == 0);

			ret = tp.GetCounterIncrementedRaw();

			EATEST_VERIFY(ret == 1);

			tp.SetPointerAndCounter(ptr, ret);

			ret = tp.GetCounterRaw();

			EATEST_VERIFY(ret == 1);

			ret = TaggedPointerType::ExtractCounter(tp.GetValueRaw());

			EATEST_VERIFY(ret == 1);

			ret = TaggedPointerType::ExtractCounter(tp.GetValue());

			EATEST_VERIFY(ret == 1);
		}
	}

	// On 32-bit we use 32bits for tags, this tests will take ages to complete.
#if EA_PLATFORM_WORD_SIZE == 8
	{
		TaggedPointerType tp(ptr, 0);

		{
			uint32_t count = 0;
			do
			{
				uint32_t ret = tp.GetCounterRaw();

				EATEST_VERIFY(ret == count);

				++count;

				ret = tp.GetCounterIncrementedRaw();

				tp.SetPointerAndCounter(ptr, ret);

				if (tp.GetCounterRaw() == 0)
				{
					// Overflowed

					EATEST_VERIFY(tp.GetPointerRaw() == ptr);

					break;
				}

				EATEST_VERIFY(tp.GetPointerRaw() == ptr);

			} while (true);
		}
	}
#endif

	{
		TaggedPointerType tp(ptr, 0);

		{
			uint32_t ret = 32;

			tp.SetPointerAndCounter(ptr, ret);

			ret = tp.GetCounterRaw();

			EATEST_VERIFY(ret == 32);

			ret = TaggedPointerType::ExtractCounter(tp.GetValueRaw());

			EATEST_VERIFY(ret == 32);
		}

		{
			uint32_t ret = 31;

			tp.SetPointerAndCounter(ptr, ret);

			ret = tp.GetCounterRaw();

			EATEST_VERIFY(ret == 31);

			ret = TaggedPointerType::ExtractCounter(tp.GetValueRaw());

			EATEST_VERIFY(ret == 31);
		}

		{
			uint32_t ret = 34;

			tp.SetPointerAndCounter(ptr, ret);

			ret = tp.GetCounterRaw();

			EATEST_VERIFY(ret == 34);

			ret = TaggedPointerType::ExtractCounter(tp.GetValueRaw());

			EATEST_VERIFY(ret == 34);
		}
	}

	{
		TaggedPointerType tp(ptr, 0);

		{
			uint32_t ret = tp.GetCounterRaw();

			EATEST_VERIFY(ret == 0);

			ret = tp.GetCounterIncrementedRaw();

			EATEST_VERIFY(ret == 1);

			tp.SetPointerAndCounter(ptr, ret);

			ret = tp.GetCounterRaw();

			EATEST_VERIFY(ret == 1);
		}
	}

	{
		TaggedPointerType tp(ptr, 0);

		{
			void* p = tp.GetPointer();

			EATEST_VERIFY(p == static_cast<void*>(ptr));

			p = tp.GetPointerRaw();

			EATEST_VERIFY(p == static_cast<void*>(ptr));

			p = TaggedPointerType::ExtractPointer(tp.GetValueRaw());

			EATEST_VERIFY(p == static_cast<void*>(ptr));

			p = TaggedPointerType::ExtractPointer(tp.GetValue());

			EATEST_VERIFY(p == static_cast<void*>(ptr));
		}
	}

	{
		TaggedPointerType tp(0, 0);

		{
			void* p = tp.GetPointer();

			EATEST_VERIFY(p == static_cast<void*>(nullptr));

			p = tp.GetPointerRaw();

			EATEST_VERIFY(p == static_cast<void*>(nullptr));

			p = TaggedPointerType::ExtractPointer(tp.GetValueRaw());

			EATEST_VERIFY(p == static_cast<void*>(nullptr));

			p = TaggedPointerType::ExtractPointer(tp.GetValue());

			EATEST_VERIFY(p == static_cast<void*>(nullptr));
		}
	}

	{
		{
			TaggedPointerType tp(ptr, 0);

			tp.SetValueRaw(0);

			EATEST_VERIFY(0 == tp.GetValueRaw());
		}

		{
			TaggedPointerType tp(ptr, 0);

			tp.SetValue(0);

			EATEST_VERIFY(0 == tp.GetValue());
		}
	}

	{
		TaggedPointerType tp(ptr, 0);

		{
			tp.SetPointerAndCounter(ptr, 0);

			tp.SetPointerAndCounter(0, 0);

			EATEST_VERIFY(tp.GetPointer() == 0);

			tp.SetPointerAndCounter(ptr, 0);

			tp.SetPointerAndCounter(ptr, 2);

			EATEST_VERIFY(tp.GetCounter() == 2);
		}
	}

	{
		TaggedPointerType tp(ptr, 0);

		{
			tp.SetPointerAndCounterRaw(ptr, 0);

			tp.SetPointerAndCounterRaw(0, 0);

			EATEST_VERIFY(tp.GetPointerRaw() == 0);

			tp.SetPointerAndCounterRaw(ptr, 0);

			tp.SetPointerAndCounterRaw(ptr, 2);

			EATEST_VERIFY(tp.GetCounterRaw() == 2);
		}
	}

	{
		TaggedPointerType tp(0, 0);

		{
			tp.SetValueConditional(2, 0);

			EATEST_VERIFY(tp.GetValueRaw() == 2);
		}
	}

	{
		TaggedPointerType tp(ptr, 0);

		{
			uint64_t ret = TaggedPointerType::MakeTagged(ptr, 0);

			EATEST_VERIFY(ret == tp.GetValueRaw());
		}
	}

	return nErrorCount;
}

int TestAtomicIntrusiveStack()
{
	using namespace EA::IO;

	int nErrorCount = 0;

	struct TestAtomicNode
	{
		TestAtomicNode* next = nullptr;
	};

	{
		Internal::AtomicIntrusiveStack<TestAtomicNode, 0> atomicStack;

		TestAtomicNode* ret = atomicStack.Pop();

		EATEST_VERIFY(ret == nullptr);
	}

	{
		TestAtomicNode node;
		Internal::AtomicIntrusiveStack<TestAtomicNode, 0> atomicStack;

		atomicStack.Push(&node);

		TestAtomicNode* ret = atomicStack.Pop();

		EATEST_VERIFY(ret == &node);

		ret = atomicStack.Pop();

		EATEST_VERIFY(ret == nullptr);
	}

	{
		TestAtomicNode node;
		TestAtomicNode node2;
		Internal::AtomicIntrusiveStack<TestAtomicNode, 0> atomicStack;

		atomicStack.Push(&node);
		atomicStack.Push(&node2);

		TestAtomicNode* ret = atomicStack.Pop();

		EATEST_VERIFY(ret == &node2);

		ret = atomicStack.Pop();

		EATEST_VERIFY(ret == &node);
	}

	{
		TestAtomicNode node;
		TestAtomicNode node2;
		TestAtomicNode node3;
		TestAtomicNode node4;
		Internal::AtomicIntrusiveStack<TestAtomicNode, 0> atomicStack;

		atomicStack.Push(&node);
		atomicStack.Push(&node2);
		atomicStack.Push(&node3);
		atomicStack.Push(&node4);

		TestAtomicNode* ret = atomicStack.PeelOff();

		for (volatile TestAtomicNode* head = ret; head; head = head->next)
		{
		}

		EATEST_VERIFY(ret == &node4);
		EATEST_VERIFY(ret->next == &node3);
		EATEST_VERIFY(ret->next->next == &node2);
		EATEST_VERIFY(ret->next->next->next == &node);
	}

	return nErrorCount;
}

int TestInternalAtomic()
{
	using namespace EA::IO;

	int nErrorCount = 0;

	nErrorCount += TestTaggedPointer();

	nErrorCount += TestAtomicIntrusiveStack();

	return nErrorCount;
}
