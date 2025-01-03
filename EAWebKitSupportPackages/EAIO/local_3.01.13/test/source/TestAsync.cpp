///////////////////////////////////////////////////////////////////////////////
// TestAsync.cpp
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

#ifdef _MSC_VER
	#pragma warning(push, 0)
#endif

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#ifdef EA_PLATFORM_WINDOWS
	#include <Windows.h>
#elif defined(EA_PLATFORM_PS5)
	#include <ampr/ampr.h>
#endif

#ifdef _MSC_VER
	#pragma warning(pop)
#endif

uint8_t GetPositionHash(uint32_t position)
{
	uint32_t result = 0x1;
	uint32_t acc = 0x45;
	for (uint32_t bits = position; bits; bits >>= 1)
	{
		if (bits & 1)
		{
			result = result ? (result * acc) : acc;
			while (result > 255)
				result = (result & 255) ^ (result >> 8);
		}
		acc *= acc;
		while (acc > 255)
			acc = (acc & 255) ^ (acc >> 8);
	}
	return uint8_t(result);
}

void FillWriteBuffer(eastl::vector<uint8_t>& buffer, uint32_t position, uint32_t size)
{
	buffer.resize(size);
	for (uint32_t i = 0; i < size; ++i)
		buffer[i] = GetPositionHash(i + position);
}

bool ValidateReadBuffer(const uint8_t* buffer, uint32_t position, uint32_t size)
{
	for (uint32_t i = 0; i < size; ++i)
		if (buffer[i] != GetPositionHash(i + position))
			return false;

	return true;
}

int TestAsyncStartupShutdown()
{
	using namespace EA::IO;

	AsyncExecutor::Init();
	AsyncExecutor::Shutdown();
	return 0;
}

int TestAsyncStartupShutdownWithExecutor(EA::IO::AsyncExecutorType type)
{
	using namespace EA::IO;

	AsyncExecutor::Init();
	AsyncExecutor::StartExecutor(type);
	AsyncExecutor::Shutdown();
	return 0;
}

int TestAsyncFunctionTask(EA::IO::AsyncExecutorType type)
{
	using namespace EA::IO;

	int nErrorCount = 0;

	int value = 0;

	AsyncExecutor::Init();
	AsyncExecutor* exec = AsyncExecutor::StartExecutor(type);

	AsyncFunction(exec, [&value]() { EA::Thread::AtomicFetchIncrement(&value); });
	for (;;)
	{
		if (EA::Thread::AtomicFetchAdd(&value, 0))
			break;
		EA::Thread::ThreadSleep(1);
	}
	AsyncExecutor::Shutdown();
	return nErrorCount;
}

int TestAsyncMultiFunctionTask(EA::IO::AsyncExecutorType type)
{
	using namespace EA::IO;

	int nErrorCount = 0;

	constexpr int kCount = 32;
	uint32_t value = 0;

	AsyncExecutor::Init();
	AsyncExecutor* exec = AsyncExecutor::StartExecutor(type);

	for (int i = 0; i < kCount; ++i)
		AsyncFunction(exec, [&value]() { EA::Thread::AtomicFetchIncrement(&value); });

	for (;;)
	{
		if (EA::Thread::AtomicFetchAdd(&value, 0) == kCount)
			break;
		EA::Thread::ThreadSleep(1);
	}
	AsyncExecutor::Shutdown();
	return nErrorCount;
}

int TestAsyncReadWrite(EA::IO::AsyncExecutorType type, EA::IO::IStream* stream)
{
	using namespace EA::IO;

	int nErrorCount = 0;

	int value = 0;

	AsyncExecutor::Init();
	AsyncExecutor* exec = AsyncExecutor::StartExecutor(type);

	eastl::vector<uint8_t> writeBuffer;
	FillWriteBuffer(writeBuffer, 0, 1024);
	eastl::vector<uint8_t> readBuffer(writeBuffer.size(), 0);

	AsyncIoFunctionCallback reader = [&value, &readBuffer, &writeBuffer, &nErrorCount](AsyncResult result, const AsyncErrorCode& ec, size_type bytesRead)
	{ 
		if (readBuffer != writeBuffer)
			EA::Thread::AtomicFetchIncrement(&nErrorCount);
		EA::Thread::AtomicFetchIncrement(&value);
	};

	AsyncIoFunctionCallback writer = [exec, &stream, &readBuffer, &reader](AsyncResult result, const AsyncErrorCode& ec, size_type bytesWritten)
	{
		stream->AsyncRead(*exec, 0, readBuffer.data(), readBuffer.size(), reader);
	};

	stream->AsyncWrite(*exec, 0, writeBuffer.data(), writeBuffer.size(), writer);

	for (;;)
	{
		if (EA::Thread::AtomicFetchAdd(&value, 0))
			break;
		EA::Thread::ThreadSleep(1);
	}

	AsyncExecutor::Shutdown();
	return nErrorCount;
}

int TestAsyncReadWriteWithStream(EA::IO::IStream* stream)
{
	using namespace EA::IO;

	AsyncExecutor::Init();

	int nErrorCount = 0;

	{
		stream->AddRef();

		int value = 0;

		eastl::vector<uint8_t> writeBuffer;
		FillWriteBuffer(writeBuffer, 0, 1024);
		eastl::vector<uint8_t> readBuffer(writeBuffer.size(), 0);

		AsyncIoFunctionCallback reader = [&value, &readBuffer, &writeBuffer, &nErrorCount](AsyncResult result, const AsyncErrorCode& ec, size_type bytesRead)
		{
			if (readBuffer != writeBuffer)
				EA::Thread::AtomicFetchIncrement(&nErrorCount);
			EA::Thread::AtomicFetchIncrement(&value);
		};

		AsyncIoFunctionCallback writer = [stream, &readBuffer, &reader](AsyncResult result, const AsyncErrorCode& ec, size_type bytesWritten)
		{
			stream->AsyncRead(0, readBuffer.data(), readBuffer.size(), reader);
		};

		stream->AsyncWrite(0, writeBuffer.data(), writeBuffer.size(), writer);

		for (;;)
		{
			if (EA::Thread::AtomicFetchAdd(&value, 0))
				break;
			EA::Thread::ThreadSleep(1);
		}

		stream->Release();
	}

	AsyncExecutor::Shutdown();

	return nErrorCount;
}

int TestAsyncWriteSoakWithStreamNoExec(EA::IO::IStream* stream, uint32_t blockSize, uint32_t blockCount, uint32_t readCount)
{
	using namespace EA::IO;

	int nErrorCount = 0;

	eastl::vector<uint8_t> writeBuffer;
	struct WriteContext
	{
		EA::IO::IStream* stream;
		uint32_t blockSize;
		uint32_t blockCount;
		uint32_t block;
	};
	WriteContext ctx{ stream, blockSize, blockCount, 0 };

	AsyncIoFunctionCallback writer;
	writer = [&ctx, &writeBuffer, &writer, &nErrorCount]
	(AsyncResult result, const AsyncErrorCode& ec, size_type bytesWritten)
	{
		if (result != AsyncResult::Success)
		{
			EA::Thread::AtomicFetchIncrement(&nErrorCount);
			return;
		}

		EA::Thread::AtomicFetchIncrement(&ctx.block);
		if (ctx.block == ctx.blockCount)
			return;

		FillWriteBuffer(writeBuffer, ctx.block * ctx.blockSize, ctx.blockSize);
		ctx.stream->AsyncWrite(ctx.block * ctx.blockSize, writeBuffer.data(), writeBuffer.size(), writer);
	};

	FillWriteBuffer(writeBuffer, ctx.block * blockSize, blockSize);
	stream->AsyncWrite(ctx.block * blockSize, writeBuffer.data(), writeBuffer.size(), writer);

	for (;;)
	{
		if (EA::Thread::AtomicFetchAdd(&ctx.block, 0) == blockCount)
			break;
		EA::Thread::ThreadSleep(1);
	}

	return nErrorCount;
}

int TestAsyncReadSoakWithStreamNoExec(EA::IO::IStream* stream, uint32_t blockSize, uint32_t blockCount, uint32_t readCount)
{
	using namespace EA::IO;

	int nErrorCount = 0;

	eastl::vector<uint8_t> readBuffer(blockSize * blockCount, 0);

	for (uint32_t iter = 0; iter < readCount; ++iter)
	{
		uint32_t block = 0;
		for (uint32_t i = 0; i < blockCount; ++i)
		{
			stream->AsyncRead(i * blockSize, readBuffer.data() + (i * blockSize), blockSize,
				[&nErrorCount, &block](AsyncResult result, const AsyncErrorCode& ec, size_type bytesWritten)
			{
				if (result != AsyncResult::Success)
				{
					EA::Thread::AtomicFetchIncrement(&nErrorCount);
					EA::Thread::AtomicFetchIncrement(&block);
					return;
				}

				//if (!ValidateReadBuffer(readBuffer.data() + (i * blockSize), i * blockSize, blockSize))
				//	++nErrorCount;

				EA::Thread::AtomicFetchIncrement(&block);
			});
		}

		for (;;)
		{
			if (EA::Thread::AtomicFetchAdd(&block, 0) == blockCount)
				break;
			EA::Thread::ThreadSleep(1);
		}
	}
	return nErrorCount;
}

int TestAsyncReadWriteSoakWithStreamNoExec(EA::IO::IStream* stream, uint32_t blockSize, uint32_t blockCount, uint32_t readCount)
{
	using namespace EA::IO;

	int nErrorCount = 0;

	nErrorCount += TestAsyncWriteSoakWithStreamNoExec(stream, blockSize, blockCount, readCount);
	nErrorCount += TestAsyncReadSoakWithStreamNoExec(stream, blockSize, blockCount, readCount);

	return nErrorCount;
}


int TestAsyncReadWriteSoakWithStream(EA::IO::IStream* stream)
{
	using namespace EA::IO;

	if (stream == nullptr)
		return 1;

	AsyncExecutor::Init();

	stream->AddRef();

	static constexpr uint32_t blockSize = 1024;
	static constexpr uint32_t blockCount = 1024;
	static constexpr uint32_t readCount = 1;

	int nErrorCount = TestAsyncReadWriteSoakWithStreamNoExec(stream, blockSize, blockCount, readCount);

	stream->Close();
	stream->Release();

	AsyncExecutor::Shutdown();

	return nErrorCount;
}

EA::IO::IStream* TestAsyncCreateMemoryStream()
{
	using namespace EA::IO;

	EA::Allocator::ICoreAllocator* allocator = EA::Allocator::ICoreAllocator::GetDefaultAllocator();

	const EA::IO::size_type kBufferSize(100);
	MemoryStream* stream = EA::Allocator::create_object<MemoryStream>(allocator, "TestStream", new SharedPointer(kBufferSize), kBufferSize);
	stream->SetReleaseAllocator(allocator);
	stream->SetOption(MemoryStream::kOptionResizeEnabled, true);
	return stream;
}

void TestAsyncGetFileName(eastl::string8& out)
{
	out = gRootDirectory8;
	out += "EAIOTempFile.dat";
	return;
}

EA::IO::IStream* TestAsyncCreateFileStream(bool sync)
{
	using namespace EA::IO;

	eastl::string8 sTemp8;
	TestAsyncGetFileName(sTemp8);

	EA::Allocator::ICoreAllocator* allocator = EA::Allocator::ICoreAllocator::GetDefaultAllocator();

	FileStream* stream = EA::Allocator::create_object<FileStream>(allocator, "TestStream", sTemp8.c_str());
	int usageHints = sync ? FileStream::kUsageHintNone : FileStream::kUsageHintAsync;
	if (!stream->Open(kAccessFlagReadWrite, kCDCreateAlways, FileStream::kShareRead, usageHints))
	{
		EA::UnitTest::Report("Error opening temporary file!!!!");
		return nullptr;
	}
	return stream;
}

int TestAsyncSoak(EA::IO::IStream* stream, const char* what, uint32_t blockSize, uint32_t blockCount, uint32_t readCount)
{
	if (stream == nullptr)
		return 1;

	int nErrorCount{ 0 };

	EA::IO::AsyncExecutor::Init();

	stream->AddRef();

	EA::UnitTest::Report("    Stress of %d blockSize, %d blockCount, %d readCount (%s)\n", blockSize, blockCount, readCount, what);
	nErrorCount += TestAsyncWriteSoakWithStreamNoExec(stream, blockSize, blockCount, readCount);
	uint64_t startTime = EA::UnitTest::GetSystemTimeMicroseconds();
	nErrorCount += TestAsyncReadSoakWithStreamNoExec(stream, blockSize, blockCount, readCount);
	uint64_t endTime = EA::UnitTest::GetSystemTimeMicroseconds();
	uint32_t totalTime = uint32_t(endTime - startTime);
	uint32_t mb = uint32_t(uint64_t(blockSize) * blockCount * readCount / 1024 / 1024);
	EA::UnitTest::Report("    Total time (microseconds): %u for %u total MB at %0.3f MB/sec\n", totalTime, mb, (double)mb / totalTime * 1e6);

	stream->Close();
	stream->Release();

	EA::IO::AsyncExecutor::Shutdown();

	return nErrorCount;
}

#ifdef EA_PLATFORM_PS5
int TestAsyncReadSoakPlatform(EA::IO::AsyncExecutor* exec, uint32_t blockSize, uint32_t blockCount, uint32_t readCount)
{
	using namespace EA::IO;

	int nErrorCount = 0;

	eastl::vector<uint8_t> readBuffer(blockSize * blockCount, 0);

	eastl::string8 sTemp8;
	TestAsyncGetFileName(sTemp8);

	const char* file[1] = { sTemp8.c_str() };
	SceAprFileId id[1];
	uint32_t errorIndex;
	int result = sce::Ampr::Apr::resolveFilepathsToIds(file, 1, id, &errorIndex);
	if (result < 0)
	{
		++nErrorCount;
		return nErrorCount;
	}

	for (uint32_t iter = 0; iter < readCount; ++iter)
	{
		uint32_t block = 0;
		for (uint32_t i = 0; i < blockCount; ++i)
		{
			EA::IO::AmprSubmit(*exec, 
			[&nErrorCount, &block](AsyncResult result, const AsyncErrorCode& ec)
			{
				if (result != AsyncResult::Success)
				{
					EA::Thread::AtomicFetchIncrement(&nErrorCount);
					EA::Thread::AtomicFetchIncrement(&block);
					return;
				}

				//if (!ValidateReadBuffer(readBuffer.data() + (i * blockSize), i * blockSize, blockSize))
				//	++nErrorCount;

				EA::Thread::AtomicFetchIncrement(&block);
			},
			[i, blockSize, data = readBuffer.data(), fileId = id[0]](AprBuilder& builder, AmprBuilderPhase phase)
			{
				builder.Read(data + (i * blockSize), fileId, (i * blockSize), blockSize);
			}
			);
		}

		for (;;)
		{
			if (EA::Thread::AtomicFetchAdd(&block, 0) == blockCount)
				break;
			EA::Thread::ThreadSleep(1);
		}
	}
	return nErrorCount;
}

int TestAsyncPackageSoak(EA::IO::IStream* stream, const char* what, uint32_t blockSize, uint32_t blockCount, uint32_t readCount)
{
	int nErrorCount{ 0 };

	EA::IO::AsyncExecutor::Init();

	EA::IO::AsyncExecutor* exec = stream->GetAsyncExecutor();

	stream->AddRef();
	EA::UnitTest::Report("    Stress of %d blockSize, %d blockCount, %d readCount (%s)\n", blockSize, blockCount, readCount, what);
	nErrorCount += TestAsyncWriteSoakWithStreamNoExec(stream, blockSize, blockCount, readCount);
	stream->Close();
	stream->Release();

	uint64_t startTime = EA::UnitTest::GetSystemTimeMicroseconds();
	nErrorCount += TestAsyncReadSoakPlatform(exec, blockSize, blockCount, readCount);
	uint64_t endTime = EA::UnitTest::GetSystemTimeMicroseconds();
	uint32_t totalTime = uint32_t(endTime - startTime);
	uint32_t mb = uint32_t(uint64_t(blockSize) * blockCount * readCount / 1024 / 1024);
	EA::UnitTest::Report("    Total time (microseconds): %u for %u total MB at %0.3f MB/sec\n", totalTime, mb, (double)mb / totalTime * 1e6);

	EA::IO::AsyncExecutor::Shutdown();

	return nErrorCount;
}
#endif

int TestAsyncSoak(uint32_t blockSize, uint32_t blockCount, uint32_t readCount)
{
	int nErrorCount{ 0 };

#if defined(EA_PLATFORM_PS5)
	// Currently we can't read a temp file in AMPR.  So until we figure out some way to get a file into app0, we can't really test
	//nErrorCount += TestAsyncPackageSoak(TestAsyncCreateFileStream(false), "AMPR - Async", blockSize, blockCount, readCount);
	//nErrorCount += TestAsyncPackageSoak(TestAsyncCreateFileStream(true), "AMPR - Sync", blockSize, blockCount, readCount);
#endif

	nErrorCount += TestAsyncSoak(TestAsyncCreateFileStream(false), "File - Async", blockSize, blockCount, readCount);
	nErrorCount += TestAsyncSoak(TestAsyncCreateFileStream(true), "File - Sync", blockSize, blockCount, readCount);
	nErrorCount += TestAsyncSoak(TestAsyncCreateMemoryStream(), "Memory", blockSize, blockCount, readCount);

	return nErrorCount;
}

int TestAsyncSuite(EA::IO::AsyncExecutorType type)
{
	const char* desc = type == EA::IO::AsyncExecutorType::Sequential ? "Sequintial" : "Concurrent";

	int nErrorCount{ 0 };

	EA::UnitTest::Report("    TestAsyncStartupShutdown (%s)\n", desc);
	nErrorCount += TestAsyncStartupShutdown();
	nErrorCount += TestAsyncStartupShutdownWithExecutor(type);

	EA::UnitTest::Report("    TestAsyncFunctions (%s)\n", desc);
	nErrorCount += TestAsyncFunctionTask(type);
	nErrorCount += TestAsyncMultiFunctionTask(type);

	EA::UnitTest::Report("    TestAsyncIO (%s)\n", desc);
	nErrorCount += TestAsyncReadWrite(type, TestAsyncCreateMemoryStream());
	nErrorCount += TestAsyncReadWrite(type, TestAsyncCreateFileStream(true));
	nErrorCount += TestAsyncReadWrite(type, TestAsyncCreateFileStream(false));

	return nErrorCount;
}

///////////////////////////////////////////////////////////////////////////////
// TestAsync
//
int TestAsync()
{
	using namespace EA::IO;

	int nErrorCount{ 0 };

	nErrorCount += TestAsyncSuite(AsyncExecutorType::Sequential);
	nErrorCount += TestAsyncSuite(AsyncExecutorType::Concurrent);

	EA::UnitTest::Report("    TestAsyncIO (no explicit executor)\n");
	nErrorCount += TestAsyncReadWriteWithStream(TestAsyncCreateMemoryStream());
	nErrorCount += TestAsyncReadWriteWithStream(TestAsyncCreateFileStream(true));
	nErrorCount += TestAsyncReadWriteWithStream(TestAsyncCreateFileStream(false));

	//nErrorCount += TestAsyncSoak(1024, 1024, 4);
	//nErrorCount += TestAsyncSoak(1024 * 128, 1024, 4);
	nErrorCount += TestAsyncSoak(1024, 1024, 128);
	//nErrorCount += TestAsyncSoak(1024 * 128, 1024, 128);

	return nErrorCount;
}

