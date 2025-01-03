// (c) 2008 Electronic Arts Inc.

#include <EABase/eabase.h>

EA_DISABLE_ALL_VC_WARNINGS()
EA_DISABLE_VC_WARNING(4548)
#include <stdlib.h>
#include <assert.h>
#include <stdio.h>
#if defined(EA_PLATFORM_MICROSOFT)
	#include <crtdbg.h>
	#include <malloc.h>
#endif
EA_RESTORE_VC_WARNING()
EA_RESTORE_ALL_VC_WARNINGS()

#include <coreallocator/icoreallocatormacros.h>
#include <EAMain/EAEntryPointMain.inl>
#include <EATest/EASTLNewOperatorGuard.inl>


class TestAllocator : public EA::Allocator::ICoreAllocator
{
public:
	virtual void *Alloc(size_t size, const char *name, unsigned int flags)
	{
		#if defined(EA_PLATFORM_WINDOWS)
			flags |= _NORMAL_BLOCK;
			return _malloc_dbg(size, static_cast<int>(flags), NULL, 0);
		#else
			return malloc(size);
		#endif
	}

	virtual void *Alloc(size_t size, const char *name, unsigned int flags, unsigned int align, unsigned int alignoffset)
	{
		#if defined(EA_PLATFORM_WINDOWS)
			flags |= _NORMAL_BLOCK;
			return _malloc_dbg(size, static_cast<int>(flags), NULL, 0);
		#else
			return malloc(size);
		#endif
	}

	virtual void *AllocDebug(size_t size, const DebugParams debugParams, unsigned int flags)
	{
		#if defined(EA_PLATFORM_WINDOWS)
			flags |= _NORMAL_BLOCK;
			return _malloc_dbg(size, static_cast<int>(flags),
				debugParams.mFileName, debugParams.mLineNum);
		#else
			return malloc(size);
		#endif
	}

	virtual void *AllocDebug(size_t size, const DebugParams debugParams, unsigned int flags, unsigned int align, unsigned int alignOffset)
	{
		#if defined(EA_PLATFORM_WINDOWS)
			flags |= _NORMAL_BLOCK;
			return _malloc_dbg(size, static_cast<int>(flags),
				debugParams.mFileName, debugParams.mLineNum);
		#else
			return malloc(size);
		#endif
	}

	virtual void Free(void *ptr, size_t)
	{
		free(ptr);
	}
};

TestAllocator gTestAllocator;

struct TestStruct
{
	// default ctor required to avoid warning on POD types
	TestStruct() {}

	int m;
};

struct TestStructNoDefaultCtor
{
	int m;
};


// Class that has a private constructor and destructor.
// For testing CORE_NEW_ARRAY_PRIVATE and CORE_DELETE_ARRAY_PRIVATE.
class TestStructPrivate
{
public:
	static TestStructPrivate* CreateArray(size_t n){ TestStructPrivate* pArray; CORE_NEW_ARRAY_PRIVATE(pArray, &gTestAllocator, TestStructPrivate, n, "TestStructPrivate", 0); return pArray; }
	static void               DestroyArray(TestStructPrivate* pArray){ CORE_DELETE_ARRAY_PRIVATE(&gTestAllocator, TestStructPrivate, pArray); }
	static int ctorCount;
	static int dtorCount;

private:
	TestStructPrivate(){ ctorCount++; }
   ~TestStructPrivate(){ dtorCount++; }
};

int TestStructPrivate::ctorCount = 0;
int TestStructPrivate::dtorCount = 0;



int EAMain(int, char**)
{
	TestStruct *p = NULL;
	TestStructNoDefaultCtor *q = NULL;

	p = CORE_NEW(&gTestAllocator, "Block1", 0) TestStruct;
	assert(p);
	CORE_DELETE(&gTestAllocator, p);

	p = CORE_NEW_ALIGN(&gTestAllocator, "Block1", 0, 16) TestStruct;
	assert(p);
	CORE_DELETE(&gTestAllocator, p);

	p = CORE_NEW_ARRAY(&gTestAllocator, TestStruct, 10, "Block1", 0);
	assert(p);
	CORE_DELETE_ARRAY(&gTestAllocator, p);

	q = CORE_NEW_ARRAY_UNINITIALIZED(&gTestAllocator, TestStructNoDefaultCtor, 10, "Block1", 0);
	assert(q);
	CORE_DELETE_ARRAY(&gTestAllocator, q);

	p = CORE_NEW_ARRAY_ALIGN(&gTestAllocator, TestStruct, 10, "Block1", 0, 64);
	assert(p);
	CORE_DELETE_ARRAY(&gTestAllocator, p);

	q = CORE_NEW_ARRAY_ALIGN_UNINITIALIZED(&gTestAllocator, TestStructNoDefaultCtor, 10, "Block1", 0, 64);
	assert(q);
	CORE_DELETE_ARRAY(&gTestAllocator, q);

	EA::Allocator::ICoreAllocator *allocator = &gTestAllocator;
	p = (TestStruct*)CORE_ALLOC(allocator,
		sizeof(TestStruct), "Block1", 0);
	assert(p);
	CORE_FREE(allocator, p);

	p = (TestStruct*)CORE_ALLOC_ALIGN(allocator, sizeof(TestStruct), "Block1", 0, 16, 0);
	assert(p);
	CORE_FREE_SIZE(allocator, p, sizeof(TestStruct));

	p = (TestStruct*)CORE_ALLOC(allocator, sizeof(TestStruct), "Block1", 0);
	assert(p);
	CORE_FREE(allocator, p);

	p = (TestStruct*)CORE_ALLOC_ALIGN(allocator, sizeof(TestStruct), "Block1", 0, 16, 0);
	assert(p);
	CORE_FREE_SIZE(allocator, p, sizeof(TestStruct));


	// Test CORE_NEW_ARRAY_PRIVATE and CORE_DELETE_PRIVATE
	TestStructPrivate* pArray = TestStructPrivate::CreateArray(3);
	assert(pArray);
	TestStructPrivate::DestroyArray(pArray);
	assert(TestStructPrivate::ctorCount == 3);
	assert(TestStructPrivate::dtorCount == 3);


	EA::EAMain::Report("done.");
	
	return 0;
}
