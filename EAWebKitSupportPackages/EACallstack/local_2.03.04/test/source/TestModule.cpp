///////////////////////////////////////////////////////////////////////////////
// Copyright (c) Electronic Arts Inc. All rights reserved.
///////////////////////////////////////////////////////////////////////////////

#include <EACallstackTest/EACallstackTest.h>

EA_DISABLE_VC_WARNING(4592); // warning C4592: symbol will be dynamically initialized
EA_DISABLE_VC_WARNING(4073); // warning C4073: initializers put in library initialization area
EA_DISABLE_VC_WARNING(4740) // flow in or out of inline asm code suppresses global optimization.

#if defined(EA_COMPILER_MSVC)

#pragma init_seg(user)

#endif

#include <stdio.h>

#if EACALLSTACK_MODULE_INFO_SUPPORTED

TEST(EACallstackTest, TestModuleStaticExecSameBaseAddress)
{
	void* thisIP;
	EA::Callstack::GetInstructionPointer(thisIP);

	uint64_t baseAddress01 = EA::Callstack::GetModuleBaseAddress(L"EACallstackTest");

	uint64_t baseAddress02 = EA::Callstack::GetModuleBaseAddress("EACallstackTest");

	uint64_t baseAddress03 = EA::Callstack::GetModuleBaseAddressByHandle(EA::Callstack::GetModuleHandleFromAddress(thisIP));

	uint64_t baseAddress04 = EA::Callstack::GetModuleBaseAddressByAddress(thisIP);

	/* Android doesn't support static builds, so our main exec isnt't EACallstackTest */
#if !defined(EA_PLATFORM_ANDROID)

	uint64_t baseAddress05 = EA::Callstack::GetModuleBaseAddressByHandle(EA::Callstack::kModuleHandleInvalid);

	uint64_t baseAddress06 = EA::Callstack::GetModuleBaseAddressByHandle();

	uint64_t baseAddress07 = EA::Callstack::GetModuleBaseAddress();

	const char* ptrC = nullptr;
	uint64_t baseAddress08 = EA::Callstack::GetModuleBaseAddress(ptrC);

	const wchar_t* ptrW = nullptr;
	uint64_t baseAddress09 = EA::Callstack::GetModuleBaseAddress(ptrW);

	ptrC = "";
	uint64_t baseAddress10 = EA::Callstack::GetModuleBaseAddress(ptrC);

	ptrW = L"";
	uint64_t baseAddress11 = EA::Callstack::GetModuleBaseAddress(ptrW);

	EXPECT_EQ(baseAddress05, baseAddress06);
	EXPECT_EQ(baseAddress06, baseAddress07);
	EXPECT_EQ(baseAddress07, baseAddress08);
	EXPECT_EQ(baseAddress08, baseAddress09);
	EXPECT_EQ(baseAddress09, baseAddress10);
	EXPECT_EQ(baseAddress10, baseAddress11);
	EXPECT_EQ(baseAddress11, baseAddress05);

#endif // !EA_PLATFORM_ANDROID

	EXPECT_EQ(baseAddress01, baseAddress02);
	EXPECT_EQ(baseAddress02, baseAddress03);
	EXPECT_EQ(baseAddress03, baseAddress04);
	EXPECT_EQ(baseAddress04, baseAddress01);
}

TEST(EACallstackTest, TestModuleStaticExecBaseAddress)
{
	void* thisIP;
	EA::Callstack::GetInstructionPointer(thisIP);

	{
		uint64_t baseAddress = EA::Callstack::GetModuleBaseAddress(L"EACallstackTest");

		EXPECT_NE(baseAddress, 0);
		EXPECT_NE(baseAddress, EA::Callstack::kBaseAddressInvalid);
		EXPECT_NE(baseAddress, EA::Callstack::kBaseAddressUnspecified);
	}

	{
		uint64_t baseAddress = EA::Callstack::GetModuleBaseAddress();

		EXPECT_NE(baseAddress, 0);
		EXPECT_NE(baseAddress, EA::Callstack::kBaseAddressInvalid);
		EXPECT_NE(baseAddress, EA::Callstack::kBaseAddressUnspecified);
	}

	{
		const char* ptr = nullptr;
		uint64_t baseAddress = EA::Callstack::GetModuleBaseAddress(ptr);

		EXPECT_NE(baseAddress, 0);
		EXPECT_NE(baseAddress, EA::Callstack::kBaseAddressInvalid);
		EXPECT_NE(baseAddress, EA::Callstack::kBaseAddressUnspecified);
	}

	{
		const wchar_t* ptr = nullptr;
		uint64_t baseAddress = EA::Callstack::GetModuleBaseAddress(ptr);

		EXPECT_NE(baseAddress, 0);
		EXPECT_NE(baseAddress, EA::Callstack::kBaseAddressInvalid);
		EXPECT_NE(baseAddress, EA::Callstack::kBaseAddressUnspecified);
	}

	{
		uint64_t baseAddress = EA::Callstack::GetModuleBaseAddress("EACallstackTest");

		EXPECT_NE(baseAddress, 0);
		EXPECT_NE(baseAddress, EA::Callstack::kBaseAddressInvalid);
		EXPECT_NE(baseAddress, EA::Callstack::kBaseAddressUnspecified);
	}

	{
		uint64_t baseAddress = EA::Callstack::GetModuleBaseAddressByHandle(EA::Callstack::kModuleHandleInvalid);

		EXPECT_NE(baseAddress, 0);
		EXPECT_NE(baseAddress, EA::Callstack::kBaseAddressInvalid);
		EXPECT_NE(baseAddress, EA::Callstack::kBaseAddressUnspecified);
	}

	{
		uint64_t baseAddress = EA::Callstack::GetModuleBaseAddressByHandle(EA::Callstack::GetModuleHandleFromAddress(thisIP));

		EXPECT_NE(baseAddress, 0);
		EXPECT_NE(baseAddress, EA::Callstack::kBaseAddressInvalid);
		EXPECT_NE(baseAddress, EA::Callstack::kBaseAddressUnspecified);
	}

	{
		uint64_t baseAddress = EA::Callstack::GetModuleBaseAddressByHandle();

		EXPECT_NE(baseAddress, 0);
		EXPECT_NE(baseAddress, EA::Callstack::kBaseAddressInvalid);
		EXPECT_NE(baseAddress, EA::Callstack::kBaseAddressUnspecified);
	}

	{
		uint64_t baseAddress = EA::Callstack::GetModuleBaseAddressByAddress(thisIP);

		EXPECT_NE(baseAddress, 0);
		EXPECT_NE(baseAddress, EA::Callstack::kBaseAddressInvalid);
		EXPECT_NE(baseAddress, EA::Callstack::kBaseAddressUnspecified);
	}

	{
		uint64_t baseAddress = EA::Callstack::GetModuleBaseAddressByAddress(nullptr);

		EXPECT_EQ(baseAddress, EA::Callstack::kBaseAddressInvalid);
	}

	{
		uint64_t baseAddress = EA::Callstack::GetModuleBaseAddressByAddress((void*)(~uintptr_t(0)));

		EXPECT_EQ(baseAddress, EA::Callstack::kBaseAddressInvalid);
	}
}

TEST(EACallstackTest, TestModuleStaticExecGetModuleBaseAddress)
{
	{
		uint64_t baseAddress01 = EA::Callstack::GetModuleBaseAddress(L"EACallstackTest");

#if defined(EA_PLATFORM_WINDOWS)

		uint64_t baseAddressEXE = EA::Callstack::GetModuleBaseAddress(L"EACallstackTest.exe");

		EXPECT_EQ(baseAddress01, baseAddressEXE);

#endif // EA_PLATFORM_WINDOWS

		if (gpApplicationPath)
		{
			wchar_t buffer[EA::IO::kMaxPathLength];
			EA::StdC::Strlcpy(buffer, gpApplicationPath, EA::IO::kMaxPathLength);
			uint64_t baseAddress02 = EA::Callstack::GetModuleBaseAddress(buffer);

			EXPECT_EQ(baseAddress02, baseAddress01);
		}

		EXPECT_NE(baseAddress01, EA::Callstack::kBaseAddressInvalid);
	}

	{
		uint64_t baseAddress01 = EA::Callstack::GetModuleBaseAddress("EACallstackTest");

#if defined(EA_PLATFORM_WINDOWS)

		uint64_t baseAddressEXE = EA::Callstack::GetModuleBaseAddress("EACallstackTest.exe");

		EXPECT_EQ(baseAddress01, baseAddressEXE);

#endif // EA_PLATFORM_WINDOWS

		if (gpApplicationPath)
		{
			uint64_t baseAddress02 = EA::Callstack::GetModuleBaseAddress(gpApplicationPath);

			EXPECT_EQ(baseAddress02, baseAddress01);
		}

		EXPECT_NE(baseAddress01, EA::Callstack::kBaseAddressInvalid);
	}
}

TEST(EACallstackTest, TestModuleStaticExecGetModuleFromAddress)
{
	{
		char buffer[EA::IO::kMaxPathLength];
		void* thisIP;
		EA::Callstack::GetInstructionPointer(thisIP);
		size_t ret = EA::Callstack::GetModuleFromAddress(thisIP, buffer, EA::IO::kMaxPathLength);

		EXPECT_GT(ret, 0);
		if (ret >= EA::IO::kMaxPathLength)
		{
			EXPECT_EQ(buffer[EA::IO::kMaxPathLength - 1], 0);
		}
		else
		{
			EXPECT_EQ(buffer[ret], 0);
		}

		EXPECT_TRUE(EA::StdC::Stristr(buffer, "EACallstackTest") != nullptr);
	}

	{
		char buffer[EA::IO::kMaxPathLength];
		size_t ret = EA::Callstack::GetModuleFromAddress(nullptr, buffer, EA::IO::kMaxPathLength);

		EXPECT_EQ(ret, 0);

		EXPECT_EQ(buffer[0], 0);
	}

	{
		char buffer[EA::IO::kMaxPathLength];
		size_t ret = EA::Callstack::GetModuleFromAddress(((void*)(~uintptr_t(0))), buffer, EA::IO::kMaxPathLength);

		EXPECT_EQ(ret, 0);

		EXPECT_EQ(buffer[0], 0);
	}
}

TEST(EACallstackTest, TestModuleStaticExecGetModuleHandleFromAddress)
{
	{
		void* thisIP;
		EA::Callstack::GetInstructionPointer(thisIP);
		EA::Callstack::ModuleHandle moduleHandle = EA::Callstack::GetModuleHandleFromAddress(thisIP);

		EXPECT_NE(moduleHandle, EA::Callstack::kModuleHandleInvalid);
	}

	{
		EA::Callstack::ModuleHandle moduleHandle = EA::Callstack::GetModuleHandleFromAddress(nullptr);

		EXPECT_EQ(moduleHandle, EA::Callstack::kModuleHandleInvalid);
	}

	{
		EA::Callstack::ModuleHandle moduleHandle = EA::Callstack::GetModuleHandleFromAddress(((void*)(~uintptr_t(0))));

		EXPECT_EQ(moduleHandle, EA::Callstack::kModuleHandleInvalid);
	}
}

TEST(EACallstackTest, TestModuleGetModuleHandleList)
{
	{
		size_t moduleCount = EA::Callstack::GetModuleHandleList(nullptr, 0);

		EXPECT_GE(moduleCount, 1);

		EA::Callstack::ModuleHandle moduleBuffer[1];
		size_t moduleCountCopied = EA::Callstack::GetModuleHandleList(moduleBuffer, 1);

		EXPECT_EQ(moduleCountCopied, 1);
		EXPECT_NE(moduleBuffer[0], EA::Callstack::kModuleHandleInvalid);
	}
}

TEST(EACallstackTest, TestModuleGetModuleInfo)
{
	{
		size_t moduleCount = EA::Callstack::GetModuleInfo(nullptr, 0);

		EXPECT_GE(moduleCount, 1);

		EA::Callstack::ModuleInfo moduleBuffer[1];
		size_t moduleCountCopied = EA::Callstack::GetModuleInfo(moduleBuffer, 1);

		EXPECT_EQ(moduleCountCopied, 1);
		EXPECT_NE(moduleBuffer[0].mModuleHandle, EA::Callstack::kModuleHandleInvalid);
	}

	{
		EA::Callstack::ModuleInfo* moduleBuffer = CORE_NEW_ARRAY(EA::Callstack::GetAllocator(), EA::Callstack::ModuleInfo,
																 EA::Callstack::kModuleMaxCapacity, "EACallstack/moduleBuffer", 0);

		size_t moduleCount = EA::Callstack::GetModuleInfo(moduleBuffer, EA::Callstack::kModuleMaxCapacity);

		EXPECT_GT(moduleCount, 0);

		EA::Callstack::ModuleInfo* modulePtr = nullptr;
		for (size_t i = 0; i < moduleCount; ++i)
		{
			EA::Callstack::ModuleInfo& moduleInfo = moduleBuffer[i];
			if (EA::StdC::Stristr(moduleInfo.mName.c_str(), "EACallstackTest"))
			{
				modulePtr = &moduleInfo;
			}
		}

		EXPECT_NE(modulePtr, nullptr);

		CORE_DELETE_ARRAY(EA::Callstack::GetAllocator(), moduleBuffer);
	}
}

TEST(EACallstackTest, TestModuleGetModuleInfoByHandle)
{
	{
		EA::Callstack::ModuleHandle moduleBuffer[1];
		size_t moduleCount = EA::Callstack::GetModuleHandleList(moduleBuffer, 1);

		EXPECT_EQ(moduleCount, 1);

		EA::Callstack::ModuleInfo moduleInfo;
		bool success = EA::Callstack::GetModuleInfoByHandle(moduleBuffer[0], moduleInfo);

		EXPECT_TRUE(success);
	}
}

TEST(EACallstackTest, TestModuleGetModuleInfoByNameLookup)
{
	{
		EA::Callstack::ModuleInfo* moduleBuffer = CORE_NEW_ARRAY(EA::Callstack::GetAllocator(), EA::Callstack::ModuleInfo,
																 EA::Callstack::kModuleMaxCapacity, "EACallstack/moduleBuffer", 0);

		size_t moduleCountCopied = EA::Callstack::GetModuleInfo(moduleBuffer, EA::Callstack::kModuleMaxCapacity);

		EXPECT_GT(moduleCountCopied, 0);

		EA::Callstack::ModuleInfo outInfo;
		bool ret = EA::Callstack::GetModuleInfoByName(moduleBuffer[0].mName.c_str(), outInfo, moduleBuffer, moduleCountCopied);

		EXPECT_TRUE(ret);
		EXPECT_TRUE(outInfo == moduleBuffer[0]);

		CORE_DELETE_ARRAY(EA::Callstack::GetAllocator(), moduleBuffer);
	}
}

TEST(EACallstackTest, TestModuleGetModuleInfoByHandleLookup)
{
	{
		EA::Callstack::ModuleInfo* moduleBuffer = CORE_NEW_ARRAY(EA::Callstack::GetAllocator(), EA::Callstack::ModuleInfo,
																 EA::Callstack::kModuleMaxCapacity, "EACallstack/moduleBuffer", 0);

		size_t moduleCountCopied = EA::Callstack::GetModuleInfo(moduleBuffer, EA::Callstack::kModuleMaxCapacity);

		EXPECT_GT(moduleCountCopied, 0);

		EA::Callstack::ModuleInfo outInfo;
		bool ret = EA::Callstack::GetModuleInfoByHandle(moduleBuffer[0].mModuleHandle, outInfo, moduleBuffer, moduleCountCopied);

		EXPECT_TRUE(ret);
		EXPECT_TRUE(outInfo == moduleBuffer[0]);

		CORE_DELETE_ARRAY(EA::Callstack::GetAllocator(), moduleBuffer);
	}
}

TEST(EACallstackTest, TestModuleGetModuleInfoByAddressLookup)
{
	{
		EA::Callstack::ModuleInfo* moduleBuffer = CORE_NEW_ARRAY(EA::Callstack::GetAllocator(), EA::Callstack::ModuleInfo,
																 EA::Callstack::kModuleMaxCapacity, "EACallstack/moduleBuffer", 0);

		size_t moduleCountCopied = EA::Callstack::GetModuleInfo(moduleBuffer, EA::Callstack::kModuleMaxCapacity);

		EXPECT_GT(moduleCountCopied, 0);

		EA::Callstack::ModuleInfo outInfo;
		bool ret = EA::Callstack::GetModuleInfoByAddress(reinterpret_cast<void*>(moduleBuffer[0].mBaseAddress), outInfo,
														 moduleBuffer, moduleCountCopied);

		EXPECT_TRUE(ret);
		EXPECT_TRUE(outInfo == moduleBuffer[0]);

		CORE_DELETE_ARRAY(EA::Callstack::GetAllocator(), moduleBuffer);
	}
}

TEST(EACallstackTest, TestModuleGetModuleInfoLookup)
{
	EA::Callstack::ModuleInfo* moduleBuffer = CORE_NEW_ARRAY(EA::Callstack::GetAllocator(), EA::Callstack::ModuleInfo,
															 EA::Callstack::kModuleMaxCapacity, "EACallstack/moduleBuffer", 0);

	size_t moduleCountCopied = EA::Callstack::GetModuleInfo(moduleBuffer, EA::Callstack::kModuleMaxCapacity);

	EXPECT_GT(moduleCountCopied, 0);

	void* thisIP;
	EA::Callstack::GetInstructionPointer(thisIP);
	EA::Callstack::ModuleInfo expectedModuleInfo;
	EA::Callstack::ModuleHandle moduleHandle = EA::Callstack::GetModuleHandleFromAddress(thisIP);
	bool success = EA::Callstack::GetModuleInfoByHandle(moduleHandle, expectedModuleInfo);
	EXPECT_TRUE(success);

	{
		uint64_t baseAddress = EA::Callstack::GetModuleBaseAddress("EACallstackTest");

		EA::Callstack::ModuleInfo outInfo;
		bool ret = EA::Callstack::GetModuleInfoByAddress(reinterpret_cast<void*>(baseAddress), outInfo,
														 moduleBuffer, moduleCountCopied);

		EXPECT_TRUE(ret);
		EXPECT_TRUE(outInfo == expectedModuleInfo);
	}

	{
		EA::Callstack::ModuleInfo outInfo;
		bool ret = EA::Callstack::GetModuleInfoByName("EACallstackTest", outInfo,
													  moduleBuffer, moduleCountCopied);

		EXPECT_TRUE(ret);
		EXPECT_TRUE(outInfo == expectedModuleInfo);
	}

	{
		EA::Callstack::ModuleInfo outInfo;
		bool ret = EA::Callstack::GetModuleInfoByHandle(moduleHandle, outInfo,
														moduleBuffer, moduleCountCopied);

		EXPECT_TRUE(ret);
		EXPECT_TRUE(outInfo == expectedModuleInfo);
	}

	CORE_DELETE_ARRAY(EA::Callstack::GetAllocator(), moduleBuffer);
}

TEST(EACallstackTest, ConstructModuleInfoWithICoreAllocator)
{
	{
		EA::Allocator::ICoreAllocator* allocator = EA::Callstack::GetAllocator();

		EA::Callstack::ModuleInfo moduleInfo{allocator};
	}
}

#endif // EACALLSTACK_MODULE_INFO_SUPPORTED


EA_RESTORE_VC_WARNING();
EA_RESTORE_VC_WARNING();
EA_RESTORE_VC_WARNING();
