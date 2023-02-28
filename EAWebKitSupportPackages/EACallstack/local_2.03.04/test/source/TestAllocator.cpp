///////////////////////////////////////////////////////////////////////////////
// Copyright (c) Electronic Arts Inc. All rights reserved.
///////////////////////////////////////////////////////////////////////////////

#include <EACallstackTest/EACallstackTest.h>

EA_DISABLE_VC_WARNING(4592); // warning C4592: symbol will be dynamically initialized
EA_DISABLE_VC_WARNING(4073); // warning C4073: initializers put in library initialization area

#if defined(EA_COMPILER_MSVC)

#pragma init_seg(user)

#endif

TEST(EACallstackTest, TestAllocatorGet)
{
	/* Allocator is initialy set from Main */

	EA::Allocator::ICoreAllocator* allocator = EA::Callstack::GetAllocator();

	EXPECT_EQ(allocator, EA::Allocator::ICoreAllocator::GetDefaultAllocator());
}

TEST(EACallstackTest, TestAllocatorSet)
{
	EA::Allocator::ICoreAllocator* allocatorOld = EA::Callstack::GetAllocator();

	EA::Callstack::SetAllocator(&gCustomCoreAllocator);

	EA::Allocator::ICoreAllocator* allocatorNew = EA::Callstack::GetAllocator();

	EA::Callstack::SetAllocator(allocatorOld);

	EXPECT_EQ(allocatorNew, &gCustomCoreAllocator);
}

EA_RESTORE_VC_WARNING();
EA_RESTORE_VC_WARNING();
