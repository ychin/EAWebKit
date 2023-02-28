///////////////////////////////////////////////////////////////////////////////
// TestStream.cpp
//
// Copyright (c) 2003 Electronic Arts. All rights reserved.
// Created by Paul Pedriana
///////////////////////////////////////////////////////////////////////////////


#include <EABase/eabase.h>
#include <EAIO/EAFileBase.h>
#include <EAIO/EAIniFile.h>
#include <EAIO/EAFileStream.h>
#include <EAIO/EAFileUtil.h>
#include <EAIO/EAStream.h>
#include <EAIO/EAStreamAdapter.h>
#include <EAIO/EAStreamChild.h>
#include <EAIO/EAStreamMemory.h>
#include <EAIO/EAStreamFixedMemory.h>
#include <EAIO/EAStreamBuffer.h>
#include <EAStdC/EAString.h>
#include <EAStdC/EAStopwatch.h>
#include <EASTL/fixed_string.h>
#include <EASTL/vector.h>
#include <EAIOTest/EAIO_Test.h>
#include <EATest/EATest.h>
#include <eathread/eathread_thread.h>
#include EA_ASSERT_HEADER

#ifdef _MSC_VER
	#pragma warning(push, 0)
#endif

#include <stdio.h>
#include <limits.h>
#if defined(EA_PLATFORM_MICROSOFT)
	#include <crtdbg.h>
#endif

#ifdef _MSC_VER
	#pragma warning(pop)
#endif



static int TestMemoryStream()
{
	using namespace EA::IO;

	EA::UnitTest::Report("    TestMemoryStream\n");

	int nErrorCount(0);

	// Test fixed-size MemoryStream
	{
		const EA::IO::size_type kBufferSize(1000);
		uint8_t                 buffer[kBufferSize];
		MemoryStream            memoryStream(new SharedPointer(kBufferSize), kBufferSize);

		EATEST_VERIFY(memoryStream.GetData() != NULL);
		EATEST_VERIFY(!memoryStream.GetOption(MemoryStream::kOptionResizeEnabled));
		EATEST_VERIFY(memoryStream.GetSize() == kBufferSize);
		EATEST_VERIFY(!memoryStream.SetSize(500));
		EATEST_VERIFY(memoryStream.GetAccessFlags() == kAccessFlagReadWrite);
		EATEST_VERIFY(memoryStream.GetState() == kStateSuccess);
		EATEST_VERIFY(memoryStream.GetPosition() == 0);
		EATEST_VERIFY(memoryStream.GetPosition(kPositionTypeCurrent) == 0);
		EATEST_VERIFY(memoryStream.GetPosition(kPositionTypeEnd) == -(signed)memoryStream.GetSize());

		// Write data.
		uint8_t c;
		for(size_type i(0), iEnd(memoryStream.GetSize()); i < iEnd; i++)
		{
			c = (uint8_t)i;
			EATEST_VERIFY(memoryStream.Write(&c, 1));
		}

		EATEST_VERIFY(memoryStream.GetPosition() == (EA::IO::off_type)memoryStream.GetSize());

		EATEST_VERIFY(memoryStream.GetPosition(kPositionTypeCurrent) == 0);
		EATEST_VERIFY(memoryStream.GetPosition(kPositionTypeEnd) == 0);
		EATEST_VERIFY(!memoryStream.Write(buffer, 1));
		EATEST_VERIFY(memoryStream.SetPosition(0));

		// Read data back.
		EATEST_VERIFY(memoryStream.Read(buffer, 20) != kSizeTypeError);

		for(EA::IO::size_type i(0); i < 20; i++)
		{
			EATEST_VERIFY(buffer[i] == (uint8_t)i);
		}

		EATEST_VERIFY(memoryStream.GetPosition() == 20);

		// Close the stream
		EATEST_VERIFY(memoryStream.Close());
		EATEST_VERIFY(memoryStream.GetData() == NULL);
	}



	// Test resizeable MemoryStream
	{
		const EA::IO::size_type kBufferSize(1000);
		uint8_t                 buffer[kBufferSize];
		MemoryStream            memoryStream(new SharedPointer(kBufferSize), kBufferSize);
		char                    cTest = 0x37; // Arbitrary value.
		size_type               i;

		memoryStream.SetOption(MemoryStream::kOptionResizeEnabled, 1);

		EATEST_VERIFY(memoryStream.GetData() != NULL);
		EATEST_VERIFY(memoryStream.GetOption(MemoryStream::kOptionResizeEnabled) != 0.f);
		EATEST_VERIFY(memoryStream.GetSize() == kBufferSize);
		EATEST_VERIFY(memoryStream.SetSize(2000));
		EATEST_VERIFY(memoryStream.GetSize() == 2000);
		EATEST_VERIFY(memoryStream.GetAccessFlags() == kAccessFlagReadWrite);
		EATEST_VERIFY(memoryStream.GetState() == kStateSuccess);
		EATEST_VERIFY(memoryStream.GetPosition() == 0);
		EATEST_VERIFY(memoryStream.GetPosition(kPositionTypeCurrent) == 0);
		EATEST_VERIFY(memoryStream.GetPosition(kPositionTypeEnd) == (EA::IO::off_type)(memoryStream.GetPosition() - memoryStream.GetSize()));

		// Write data.
		uint8_t c;
		const size_type kNewSize(memoryStream.GetSize() * 2);
		for(i = 0; i < kNewSize; i++)
		{
			c = (uint8_t)i;
			EATEST_VERIFY(memoryStream.Write(&c, 1));
		}

		EATEST_VERIFY(memoryStream.GetSize() >= kNewSize);
		EATEST_VERIFY(memoryStream.GetPosition() == (EA::IO::off_type)kNewSize);
		EATEST_VERIFY(memoryStream.GetPosition(kPositionTypeCurrent) == 0);
		EATEST_VERIFY(memoryStream.GetPosition(kPositionTypeEnd) == (EA::IO::off_type)(memoryStream.GetPosition() - memoryStream.GetSize()));
		EATEST_VERIFY(memoryStream.SetPosition(0));

		// Read data back.
		EATEST_VERIFY(memoryStream.Read(buffer, 20) != kSizeTypeError);

		for(i = 0; i < 20; i++)
			EATEST_VERIFY(buffer[i] == (uint8_t)i);

		EATEST_VERIFY(memoryStream.GetPosition() == 20);

		// Truncate the stream.
		EATEST_VERIFY(memoryStream.SetSize(0));
		EATEST_VERIFY(memoryStream.SetPosition(0));

		// Make sure that capacity and size are distinct.
		for(i = 0; i < 1000; ++i)
		{
			EATEST_VERIFY(memoryStream.Write(&cTest, 1));
			EATEST_VERIFY(memoryStream.GetSize() == (i + 1));
		}

		// Test resizing the stream via SetPosition.
		// In this first case, the newSize happens to fit within our existing capacity.
		const size_type currentSize = memoryStream.GetSize();
		const size_type newSize     = currentSize * 2;
		EATEST_VERIFY(memoryStream.SetPosition((off_type)newSize - 1));
		EATEST_VERIFY(memoryStream.Write(&cTest, 1));
		EATEST_VERIFY(memoryStream.GetSize() == newSize);
		EATEST_VERIFY(memoryStream.SetPosition((off_type)newSize - 1));
		EATEST_VERIFY(memoryStream.Read(&c, 1) == 1);
		EATEST_VERIFY(c == cTest);

		// In this first case, the newSize happens to require an increase in capacity.
		const size_type newSize2 = newSize + 100000;
		EATEST_VERIFY(memoryStream.SetPosition((off_type)newSize2 - 1));
		EATEST_VERIFY(memoryStream.Write(&cTest, 1));
		EATEST_VERIFY(memoryStream.GetSize() == newSize2);
		EATEST_VERIFY(memoryStream.SetPosition((off_type)newSize2 - 1));
		EATEST_VERIFY(memoryStream.Read(&c, 1) == 1);
		EATEST_VERIFY(c == cTest);

		// Close the stream
		EATEST_VERIFY(memoryStream.Close());
		EATEST_VERIFY(memoryStream.GetData() == NULL);
	}



	// Test MemoryStream allocator
	{
		const EA::IO::size_type kBufferSize(1000);
		uint8_t                 buffer[kBufferSize];
		CoreAllocatorMalloc     coreAllocatorMalloc;
		MemoryStream            memoryStream;
		char                    cTest = 0x37; // Arbitrary value.
		size_type               i;

		memoryStream.SetAllocator(&coreAllocatorMalloc);
		memoryStream.SetOption(MemoryStream::kOptionResizeEnabled, 1);

		EATEST_VERIFY(memoryStream.GetOption(MemoryStream::kOptionResizeEnabled) != 0.f);
		EATEST_VERIFY(memoryStream.SetSize(2000));
		EATEST_VERIFY(memoryStream.GetSize() == 2000);
		EATEST_VERIFY(memoryStream.GetAccessFlags() == kAccessFlagReadWrite);
		EATEST_VERIFY(memoryStream.GetState() == kStateSuccess);
		EATEST_VERIFY(memoryStream.GetPosition() == 0);
		EATEST_VERIFY(memoryStream.GetPosition(kPositionTypeCurrent) == 0);
		EATEST_VERIFY(memoryStream.GetPosition(kPositionTypeEnd) == (EA::IO::off_type)(memoryStream.GetPosition() - memoryStream.GetSize()));

		// Write data.
		uint8_t c;
		const size_type kNewSize(memoryStream.GetSize() * 2);
		for(i = 0; i < kNewSize; i++)
		{
			c = (uint8_t)i;
			EATEST_VERIFY(memoryStream.Write(&c, 1));
		}

		EATEST_VERIFY(memoryStream.GetSize() >= kNewSize);
		EATEST_VERIFY(memoryStream.GetPosition() == (EA::IO::off_type)kNewSize);
		EATEST_VERIFY(memoryStream.GetPosition(kPositionTypeCurrent) == 0);
		EATEST_VERIFY(memoryStream.GetPosition(kPositionTypeEnd) == (EA::IO::off_type)(memoryStream.GetPosition() - memoryStream.GetSize()));
		EATEST_VERIFY(memoryStream.SetPosition(0));

		// Read data back.
		EATEST_VERIFY(memoryStream.Read(buffer, 20) != kSizeTypeError);

		for(i = 0; i < 20; i++)
			EATEST_VERIFY(buffer[i] == (uint8_t)i);

		EATEST_VERIFY(memoryStream.GetPosition() == 20);

		// Truncate the stream.
		EATEST_VERIFY(memoryStream.SetSize(0));
		EATEST_VERIFY(memoryStream.SetPosition(0));

		// Make sure that capacity and size are distinct.
		for(i = 0; i < 1000; ++i)
		{
			EATEST_VERIFY(memoryStream.Write(&cTest, 1));
			EATEST_VERIFY(memoryStream.GetSize() == (i + 1));
		}

		// Test resizing the stream via SetPosition.
		// In this first case, the newSize happens to fit within our existing capacity.
		const size_type currentSize = memoryStream.GetSize();
		const size_type newSize     = currentSize * 2;
		EATEST_VERIFY(memoryStream.SetPosition((off_type)newSize - 1));
		EATEST_VERIFY(memoryStream.Write(&cTest, 1));
		EATEST_VERIFY(memoryStream.GetSize() == newSize);
		EATEST_VERIFY(memoryStream.SetPosition((off_type)newSize - 1));
		EATEST_VERIFY(memoryStream.Read(&c, 1) == 1);
		EATEST_VERIFY(c == cTest);

		// In this first case, the newSize happens to require an increase in capacity.
		const size_type newSize2 = newSize + 100000;
		EATEST_VERIFY(memoryStream.SetPosition((off_type)newSize2 - 1));
		EATEST_VERIFY(memoryStream.Write(&cTest, 1));
		EATEST_VERIFY(memoryStream.GetSize() == newSize2);
		EATEST_VERIFY(memoryStream.SetPosition((off_type)newSize2 - 1));
		EATEST_VERIFY(memoryStream.Read(&c, 1) == 1);
		EATEST_VERIFY(c == cTest);

		// Close the stream
		EATEST_VERIFY(memoryStream.Close());
		EATEST_VERIFY(memoryStream.GetData() == NULL);

		// Verify that coreAllocatorMalloc was used and that all memory is freed.
		EATEST_VERIFY(coreAllocatorMalloc.mAllocationCount > 0);
		EATEST_VERIFY(coreAllocatorMalloc.mAllocationCount == coreAllocatorMalloc.mFreeCount);
	}

	// Test copying the MemoryStream
	{
		const EA::IO::size_type kBufferSize(1000);
		uint8_t                 buffer[kBufferSize];
		CoreAllocatorMalloc     coreAllocatorMalloc;
		MemoryStream*           pMemoryStream = NULL;
		size_type               i;

		pMemoryStream = new(static_cast<MemoryStream*>(coreAllocatorMalloc.Alloc(sizeof(MemoryStream), "EAIOTest/MemoryStream", 0)))MemoryStream();
		pMemoryStream->AddRef(); // for this scope

		pMemoryStream->SetAllocator(&coreAllocatorMalloc);
		 // We heap allocated our memory stream so we want to test that the release allocator is working
		pMemoryStream->SetReleaseAllocator(&coreAllocatorMalloc);
		pMemoryStream->SetOption(MemoryStream::kOptionResizeEnabled, 1);
		EATEST_VERIFY(pMemoryStream->SetSize(2000));

		// Write data.
		uint8_t c;
		const size_type kNewSize(pMemoryStream->GetSize() * 2);
		for(i = 0; i < kNewSize; i++)
		{
			c = (uint8_t)i;
			EATEST_VERIFY(pMemoryStream->Write(&c, 1));
		}

		// Read data back.
		EATEST_VERIFY(pMemoryStream->SetPosition(0));
		EATEST_VERIFY(pMemoryStream->Read(buffer, 20) != kSizeTypeError);

		for(i = 0; i < 20; i++)
			EATEST_VERIFY(buffer[i] == (uint8_t)i);

		EATEST_VERIFY(pMemoryStream->GetPosition() == 20);

		// This scoping is important to make sure our copied memory stream is destroyed in a controllable manner
		{
			MemoryStream* pMemoryStreamCopy = new(static_cast<MemoryStream*>(coreAllocatorMalloc.Alloc(sizeof(MemoryStream), "EAIOTest/MemoryStream", 0)))MemoryStream(*pMemoryStream);
			pMemoryStreamCopy->AddRef(); // for this scope
			// NOTE: We are not calling SetReleaseAllocator() here on purpose since it should have been copied in the copy constructor above

			// Verifying that we are properly copying the original MemoryStream
			EATEST_VERIFY(pMemoryStreamCopy->GetPosition()									== pMemoryStream->GetPosition());
			EATEST_VERIFY(pMemoryStreamCopy->GetData()										== pMemoryStream->GetData());
			EATEST_VERIFY(pMemoryStreamCopy->GetOption(MemoryStream::kOptionResizeEnabled)	== pMemoryStream->GetOption(MemoryStream::kOptionResizeEnabled));
			EATEST_VERIFY(pMemoryStreamCopy->GetSize()										== pMemoryStream->GetSize());
			EATEST_VERIFY(pMemoryStreamCopy->GetAccessFlags()								== pMemoryStream->GetAccessFlags());
			EATEST_VERIFY(pMemoryStreamCopy->GetState()										== pMemoryStream->GetState());
			EATEST_VERIFY(pMemoryStreamCopy->GetPosition()									== pMemoryStream->GetPosition());

			
			// Read data back that we wrote previously with the other memory stream
			EATEST_VERIFY(pMemoryStreamCopy->SetPosition(0));
			EATEST_VERIFY(pMemoryStreamCopy->Read(buffer, 20) != kSizeTypeError);

			for(i = 0; i < 20; i++)
				EATEST_VERIFY(buffer[i] == (uint8_t)i);

			pMemoryStreamCopy->Release(); // for the addref above in this scope
		}

		// Read data back again - simply making sure we haven't destroyed our memorystream
		EATEST_VERIFY(pMemoryStream->SetPosition(0));
		EATEST_VERIFY(pMemoryStream->Read(buffer, 20) != kSizeTypeError);

		for(i = 0; i < 20; i++)
			EATEST_VERIFY(buffer[i] == (uint8_t)i);

		// Close the stream
		EATEST_VERIFY(pMemoryStream->Close());
		EATEST_VERIFY(pMemoryStream->GetData() == NULL);

		pMemoryStream->Release(); // Release this stream for this scope. This will invoke the release allocator

		// Verify that coreAllocatorMalloc was used and that all memory is freed.
		EATEST_VERIFY(coreAllocatorMalloc.mAllocationCount > 0);
		EATEST_VERIFY(coreAllocatorMalloc.mAllocationCount == coreAllocatorMalloc.mFreeCount);
	}

	return nErrorCount;
}


static int TestFixedMemoryStream()
{
	using namespace EA::IO;

	EA::UnitTest::Report("    TestFixedMemoryStream\n");

	int nErrorCount(0);

	// Test fixed-size MemoryStream
	{
		const EA::IO::size_type kBufferSize(1000);
		uint8_t                 buffer[kBufferSize];
		FixedMemoryStream       memoryStream(buffer, kBufferSize);

		EATEST_VERIFY(memoryStream.GetData() == buffer);
		EATEST_VERIFY(memoryStream.GetSize() == kBufferSize);
		EATEST_VERIFY(!memoryStream.SetSize(5000));
		EATEST_VERIFY(memoryStream.GetAccessFlags() == kAccessFlagReadWrite);
		EATEST_VERIFY(memoryStream.GetState() == kStateSuccess);
		EATEST_VERIFY(memoryStream.GetPosition() == 0);
		EATEST_VERIFY(memoryStream.GetPosition(kPositionTypeCurrent) == 0);
		EATEST_VERIFY(memoryStream.GetPosition(kPositionTypeEnd) == -(signed)memoryStream.GetSize());

		// Write data.
		uint8_t c;
		for(size_type i(0), iEnd(memoryStream.GetSize()); i < iEnd; i++)
		{
			c = (uint8_t)i;
			EATEST_VERIFY(memoryStream.Write(&c, 1));
		}

		EATEST_VERIFY(memoryStream.GetPosition() == (EA::IO::off_type)memoryStream.GetSize());

		EATEST_VERIFY(memoryStream.GetPosition(kPositionTypeCurrent) == 0);
		EATEST_VERIFY(memoryStream.GetPosition(kPositionTypeEnd) == 0);
		EATEST_VERIFY(!memoryStream.Write(buffer, 1));
		EATEST_VERIFY(memoryStream.SetPosition(0));

		// Read data back.
		EATEST_VERIFY(memoryStream.Read(buffer, 20) != kSizeTypeError);

		for(EA::IO::size_type i(0); i < 20; i++)
		{
			EATEST_VERIFY(buffer[i] == (uint8_t)i);
		}

		EATEST_VERIFY(memoryStream.GetPosition() == 20);

		// Close the stream
		EATEST_VERIFY(memoryStream.Close());
		//EATEST_VERIFY(memoryStream.GetData() == NULL); // This is currently not guaranteed by FixedMemoryStream.
	}

	return nErrorCount;
}

///////////////////////////////////////////////////////////////////////////////
// TestStreamBuffer
//
// Tests buffered reading and writing to a file called uint16_tWrite.txt. This is
// the most stringent test of out test suite here. We do random seeks within
// a uint16_t-setup file and randomly read or write chunks of data that should
// match the expected data in the file. After doing this many many time, we
// re-read the entire file and verify that it contains the expected data. Also,
// we manually inspect the file to make sure it looks proper.
// 
static int TestStreamBufferHelper(EA::IO::IStream* pStream)
{
	using namespace EA::IO;


	int nErrorCount(0);

	const float fDiskSpeed = EA::UnitTest::GetSystemSpeed(EA::UnitTest::kSpeedTypeDisk);

	if(fDiskSpeed > 0.1f)
	{
		const uint16_t     kUnusedValue = 0x0001;
		EA::UnitTest::Rand rng(EA::UnitTest::GetRandSeed());
		uint32_t           i;
		uint16_t* const    pBuffer = new uint16_t[65536 * 2];
		bool               bResult;

		// Setup
		for(i = 0; i < 65536; i++)
			pBuffer[i] = (uint16_t)i;

		for(; i < 65536 * 2; i++)
			pBuffer[i] = kUnusedValue;


		// Test a simple sequential write.
		for(i = 0; i < 65536; i++)
		{
			uint16_t i16 = (uint16_t)i;
			pStream->Write(&i16, sizeof(i16));
		}

		pStream->SetPosition(0, EA::IO::kPositionTypeBegin);

		for(i = 0; i < 65536; i++)
		{
			uint16_t i16;
			pStream->Read(&i16, sizeof(i16));

			EATEST_VERIFY(i16 == (uint16_t)i);
		}

		for(i = 65536; i < 65536 * 2; i++)
		{
			EATEST_VERIFY(pBuffer[i] == kUnusedValue);
		}


		// Arbitrary random read/write tests.
		const uint32_t kMaxReadWriteCount = (uint32_t)(10000 * fDiskSpeed);

		for(i = 0; i < kMaxReadWriteCount; i++)
		{
			uint32_t nFilePosition(rng.RandLimit((65536 / sizeof(uint16_t)) - 32) * sizeof(uint16_t));
			uint16_t buffer[48];
			int      nBlockSize(rng.RandRange(2, 48));

			bResult = pStream->SetPosition((EA::IO::off_type)nFilePosition, EA::IO::kPositionTypeBegin); // nFilePosition is an even value from 0-131070.
			EATEST_VERIFY(bResult);

			if(rng.RandLimit(2) == 0) // RandLimit(2) is the same thing as a RandBool.
			{
				// We write values into the file with the same value they were originally, 
				// so it's easy to tell if things went correctly.
				for(uint16_t j = 0; j < nBlockSize; j++)
					buffer[j] = (uint16_t)((nFilePosition / sizeof(uint16_t)) + j);

				bResult = pStream->Write(buffer, nBlockSize * sizeof(uint16_t));
				EATEST_VERIFY(bResult);
			}
			else
			{
				// Test reading a segment of the file, verifying that it is what we expect.
				EA::IO::size_type n = pStream->Read(buffer, nBlockSize * sizeof(uint16_t));
				EATEST_VERIFY(n == (nBlockSize * sizeof(uint16_t)));

				for(uint16_t x = 0; x < nBlockSize; x++)
					EATEST_VERIFY(buffer[x] == (uint16_t)((nFilePosition / sizeof(uint16_t)) + x));
			}
		}

		// Do final pass at verifying the file is as we expect.
		bResult = pStream->SetPosition(0, EA::IO::kPositionTypeBegin);
		EATEST_VERIFY(bResult);

		for(uint32_t y = 0; y < 65536; y++)
		{
			uint16_t i16;
			pStream->Read(&i16, sizeof(i16));

			EATEST_VERIFY(i16 == (uint16_t)y);
		}

		delete[] pBuffer;
	}

	return nErrorCount;
}


static int TestStreamBuffer()
{
	using namespace EA::IO;

	EA::UnitTest::Report("    TestStreamBuffer\n");

	int     nErrorCount(0);
	bool    bResult = true;
	wchar_t pFilePath[kMaxPathLength];

	bResult = MakeTempPathName(pFilePath, gRootDirectoryW, NULL, L".temp");

	EATEST_VERIFY(bResult);
 
	// Test dynamically allocated buffer space.
	if(bResult)
	{
		FileStream   file;
		StreamBuffer fileBuffer(4000, 4000, &file);

		file.AddRef();
		file.SetPath(pFilePath);

		fileBuffer.AddRef();

		if(File::Exists(pFilePath))
			File::Remove(pFilePath);

		bResult = file.Open(kAccessFlagReadWrite, kCDOpenAlways);
		EATEST_VERIFY(bResult);

		if(bResult)
		{
			nErrorCount += TestStreamBufferHelper(&fileBuffer);

			// Now switch over to manually provided space:
			uint8_t readBuffer[512];
			uint8_t writeBuffer[512];

			fileBuffer.SetBuffers(readBuffer, sizeof(readBuffer), writeBuffer, sizeof(writeBuffer));

			nErrorCount += TestStreamBufferHelper(&fileBuffer);
		}
	}

	// Test manually provided buffer space.
	if(bResult)
	{
		FileStream   file;
		StreamBuffer fileBuffer;
		uint8_t      readBuffer[2000];
		uint8_t      writeBuffer[2000];

		file.AddRef();
		file.SetPath(pFilePath);

		fileBuffer.AddRef();
		fileBuffer.SetStream(&file);
		fileBuffer.SetBuffers(readBuffer, (size_type)sizeof(readBuffer), writeBuffer, (size_type)sizeof(writeBuffer));

		if(File::Exists(pFilePath))
			File::Remove(pFilePath);

		bResult = file.Open(kAccessFlagReadWrite, kCDOpenAlways);
		EATEST_VERIFY(bResult);

		if(bResult)
		{
			nErrorCount += TestStreamBufferHelper(&fileBuffer);

			// Now switch over to non-manually provided space:
			fileBuffer.SetAllocator(EA::IO::GetAllocator());
			fileBuffer.SetBufferSizes(512, 512);

			nErrorCount += TestStreamBufferHelper(&fileBuffer);
		}
	}

	return nErrorCount;
}


// On Windows, I'm getting results like this in opt builds:
//     FileStream (unbuffered): 421 ms
//     StreamBuffer: 51 ms
//     stdio fread: 300 ms

static int BenchmarkStreamBuffer()
{
	int nErrorCount(0);

	// We want to verify that StreamBuffer results in buffered IO that's at least as fast as the IO with fopen/fread/fwrite.
	{
		#if 0 // Disabled until we would like to run it again.   #if defined(EA_PLATFORM_WINDOWS) // For the time being, we test this only on Windows.
			struct ReadSegment
			{
				uint32_t mPosition;
				uint32_t mLength;
			};
			typedef eastl::vector<ReadSegment> ReadSegmentArray;

			uint32_t           fileSize = 16 * 1024 * 1024;  // 16 MB file
			uint32_t           maxSegmentSize = 64 * 1024;
			const uint32_t     maxReadSize = 512;
			uint32_t           segmentCount = 2000;
			ReadSegment        readSegmentTemp;
			ReadSegment        readSegmentSegment;
			ReadSegmentArray   readSegmentArray;
			EA::UnitTest::Rand rand(1234567);
			
			for(uint32_t i = 0; i < segmentCount; i++)
			{
				if(rand.RandLimit(10) == 0)
					readSegmentTemp.mPosition = rand.RandLimit(fileSize - maxSegmentSize); // To do: Make this position be near the previous readSegmentTemp.
				else
					readSegmentTemp.mPosition = rand.RandLimit(fileSize - maxSegmentSize);
				readSegmentTemp.mLength   = rand.RandLimit(maxSegmentSize);

				//EA::UnitTest::Report("BenchmarkStreamBuffer: Segment: ");

				for(uint32_t j = readSegmentTemp.mPosition; j < (readSegmentTemp.mPosition + readSegmentTemp.mLength); )
				{
					readSegmentSegment.mPosition = j;
					readSegmentSegment.mLength   = rand.RandRange(1, maxReadSize);
					j += readSegmentSegment.mLength;

					readSegmentArray.push_back(readSegmentSegment);
					//EA::UnitTest::Report("%I32u/%I32u,", readSegmentSegment.mPosition, readSegmentSegment.mLength);
				}

				//EA::UnitTest::Report("\n");
			}

			// Now we have a readSegmentArray which is a list of reads we are going to do. Most of the time the reads are contiguous.
			char filePath[EA::IO::kMaxPathLength];

			if(EA::IO::MakeTempPathName(filePath, NULL, NULL, ".txt", EAArrayCount(filePath)))
			{
				EA::IO::FileStream fileStream(filePath);
				fileStream.AddRef();

				if(fileStream.Open(EA::IO::kAccessFlagWrite))
				{
					fileStream.SetSize(fileSize);
					fileStream.SetPosition(fileSize - 1);
					fileStream.Write(&fileSize, 1);
					fileStream.Close();

					// Now that we have a file, let's benchmark the reading of segments from it.
					char     readBuffer[maxReadSize];
					uint32_t lastReadPosition = 0;


					{   // Benchmark EA::IO::FileStream. We expect this to be slow because it is unbuffered.
						EA::StdC::Stopwatch  stopwatch(EA::StdC::Stopwatch::kUnitsMilliseconds);
						fileStream.Open(EA::IO::kAccessFlagRead);
						stopwatch.Start();
						for(eastl_size_t i = 0, iEnd = readSegmentArray.size(); i != iEnd; i++)
						{
							const ReadSegment& rs = readSegmentArray[i];

							if(rs.mPosition != lastReadPosition) // If the next read is not sequential after the last one...
								fileStream.SetPosition(rs.mPosition);
							fileStream.Read(readBuffer, rs.mLength);
							lastReadPosition = (rs.mPosition + rs.mLength);
						}
						stopwatch.Stop();
						fileStream.Close();
						EA::UnitTest::Report("BenchmarkStreamBuffer: FileStream (unbuffered): %I64u ms.\n", stopwatch.GetElapsedTime());
					}

					{   // Benchmark EA::IO::StreamBuffer.
						EA::StdC::Stopwatch  stopwatch(EA::StdC::Stopwatch::kUnitsMilliseconds);
						EA::IO::StreamBuffer streamBuffer(maxSegmentSize, 0, &fileStream);
						fileStream.Open(EA::IO::kAccessFlagRead);
						stopwatch.Start();
						for(eastl_size_t i = 0, iEnd = readSegmentArray.size(); i != iEnd; i++)
						{
							const ReadSegment& rs = readSegmentArray[i];

							if(rs.mPosition != lastReadPosition) // If the next read is not sequential after the last one...
								streamBuffer.SetPosition(rs.mPosition);
							streamBuffer.Read(readBuffer, rs.mLength);
							lastReadPosition = (rs.mPosition + rs.mLength);
						}
						stopwatch.Stop();
						fileStream.Close();
						EA::UnitTest::Report("BenchmarkStreamBuffer: StreamBuffer: %I64u ms.\n", stopwatch.GetElapsedTime());
					}

					{   // Benchmark stdio fread.
						EA::StdC::Stopwatch  stopwatch(EA::StdC::Stopwatch::kUnitsMilliseconds);
						EA::IO::StreamBuffer streamBuffer(maxSegmentSize, 0, &fileStream);
						FILE* pFile = fopen(filePath, "r");
						setvbuf(pFile, NULL, _IOFBF, maxSegmentSize);

						stopwatch.Start();
						for(eastl_size_t i = 0, iEnd = readSegmentArray.size(); i != iEnd; i++)
						{
							const ReadSegment& rs = readSegmentArray[i];

							if(rs.mPosition != lastReadPosition) // If the next read is not sequential after the last one...
								streamBuffer.SetPosition(rs.mPosition);
							fread(readBuffer, rs.mLength, 1, pFile);
							lastReadPosition = (rs.mPosition + rs.mLength);
						}
						stopwatch.Stop();
						fileStream.Close();
						EA::UnitTest::Report("BenchmarkStreamBuffer: stdio fread: %I64u ms.\n", stopwatch.GetElapsedTime());
					}
				}

				EA::IO::File::Remove(filePath);
			}
			
		#endif
	}

	return nErrorCount;
}


static int TestStreamChild()
{
	using namespace EA::IO;

	EA::UnitTest::Report("    TestStreamChild\n");

	int nErrorCount(0);

	const uint32_t kMaxReadWriteCount = (uint32_t)(10000 * EA::UnitTest::GetSystemSpeed(EA::UnitTest::kSpeedTypeDisk));

	{
		const EA::IO::size_type kMemorySize(1000);
		uint8_t                 memory[kMemorySize];
		FixedMemoryStream       memoryStream(memory, kMemorySize);
		memoryStream.AddRef();

		const EA::IO::size_type kChildBegin(100);
		const EA::IO::size_type kChildSize(500);
		StreamChild             childStream(&memoryStream, kChildBegin, kChildSize);
		childStream.AddRef();

		EATEST_VERIFY(childStream.GetSize() == kChildSize);
		EATEST_VERIFY((childStream.GetAccessFlags() & kAccessFlagRead) != 0);
		EATEST_VERIFY(childStream.GetState() == kStateSuccess);
		EATEST_VERIFY(childStream.GetPosition() == 0);
		EATEST_VERIFY(childStream.GetPosition(kPositionTypeCurrent) == 0);
		EATEST_VERIFY(childStream.GetPosition(kPositionTypeEnd) == -(signed)childStream.GetSize());

		// Write to memoryStream (not childStream).
		for(size_type i(0), iEnd(memoryStream.GetSize()); i < iEnd; i++)
		{
			uint8_t c = (uint8_t)i;
			EATEST_VERIFY(memoryStream.Write(&c, 1));
		}

		// Read data back.
		uint8_t            buffer[kChildSize];
		EA::UnitTest::Rand rng(EA::UnitTest::GetRandSeed());
		EA::IO::size_type  n;

		// Arbitrary random read tests.
		for(uint32_t i = 0; i < kMaxReadWriteCount; i++)
		{
			int32_t nFilePosition = rng.RandRange(0, kChildSize);
			int32_t nBlockSize    = rng.RandRange(0, kChildSize);

			bool bResult = childStream.SetPosition((EA::IO::off_type)nFilePosition);
			EATEST_VERIFY(bResult);
			EATEST_VERIFY(childStream.GetPosition() == (EA::IO::off_type)nFilePosition);

			// Disabled while StreamChild doesn't support writing.
			//if(rng.RandLimit(2) == 0) // RandLimit(2) is the same thing as a RandBool.
			//{
			//    // We write values into the file with the same value they were originally, 
			//    // so it's easy to tell if things went correctly.
			//    for(uint32_t j = 0; j < ____; j++)
			//        buffer[j] = ____;
			//
			//    bool bResult = childStream.Write(buffer, ____);
			//    EATEST_VERIFY(bResult);
			//}
			//else
			{
				// Test reading a segment of the file, verifying that it is what we expect.
				n = childStream.Read(buffer, (EA::IO::size_type)nBlockSize);

				if((nFilePosition + nBlockSize) <= (int32_t)kChildSize)
					EATEST_VERIFY(n == (EA::IO::size_type)nBlockSize);
				else
					EATEST_VERIFY(n == (kChildSize - nFilePosition));

				for(EA::IO::size_type x = 0; x < n; x++)
					EATEST_VERIFY(buffer[x] == (uint8_t)(kChildBegin + nFilePosition + x));
			}
		}
	}

	return nErrorCount;
}



static int TestStreamAdapter()
{
	using namespace EA::IO;

	EA::UnitTest::Report("    TestStreamAdapter\n");

	int nErrorCount(0);

	const EA::IO::size_type kBufferSize(1000);
	uint8_t                 buffer[kBufferSize];
	FixedMemoryStream       memoryStream(buffer, kBufferSize);
	StreamAdapter           streamAdapter;
	EA::IO::IStream*        pStream;
	EA::IO::Endian          endianType;
	bool                    bResult;
	size_t                  nResult;
	bool                    b8;
	bool                    b8Array[5];
	uint8_t                 u8;
	uint8_t                 u8Array[5];
	uint16_t                u16;
	uint16_t                u16Array[5];
	uint32_t                u32;
	uint32_t                u32Array[5];
	uint64_t                u64;
	uint64_t                u64Array[5];
	float                   f32;
	float                   f32Array[5];
	double                  d64;
	double                  d64Array[5];
	String8                 str8;
	String16                str16;

	memset(buffer, 0, sizeof(buffer));

	// void SetValid(bool success);
	// bool IsValid() const;
	// operator bool() const;
	// bool operator !() const;
	streamAdapter.SetValid(false);
	EATEST_VERIFY(!streamAdapter.IsValid());
	EATEST_VERIFY(streamAdapter == false);
	EATEST_VERIFY(!streamAdapter);

	streamAdapter.SetValid(true);
	EATEST_VERIFY(streamAdapter.IsValid());
	EATEST_VERIFY(streamAdapter == true);
	EATEST_VERIFY(!!streamAdapter);

	// EA::IO::IStream* GetStream() const;
	// void SetStream(EA::IO::IStream* pStream);
	pStream = streamAdapter.GetStream();
	EATEST_VERIFY(pStream == NULL);

	streamAdapter.SetStream(&memoryStream); 
	pStream = streamAdapter.GetStream();
	EATEST_VERIFY(pStream == &memoryStream);

	// EA::IO::Endian GetEndianType() const;
	// void SetEndianType(EA::IO::Endian endianType);
	endianType = streamAdapter.GetEndianType();
	EATEST_VERIFY(endianType == EA::IO::kEndianBig);

	streamAdapter.SetEndianType(EA::IO::kEndianLittle);
	endianType = streamAdapter.GetEndianType();
	EATEST_VERIFY(endianType == EA::IO::kEndianLittle);

	streamAdapter.SetEndianType(EA::IO::kEndianBig);

	// bool VerifyArraySize(uint32_t elementCount, uint32_t elementSize);
	bResult = streamAdapter.VerifyArraySize(0, 1); 
	EATEST_VERIFY(bResult);

	bResult = streamAdapter.VerifyArraySize(1000, 8); 
	EATEST_VERIFY(!bResult);

	streamAdapter.SetValid(true);

	// void WriteBool8(bool v);
	// void WriteUint8(uint8_t v);
	// void WriteInt8(int8_t v);
	// void WriteUint16(uint16_t v);
	// void WriteInt16(int16_t v);
	// void WriteUint32(uint32_t v);
	// void WriteInt32(int32_t v);
	// void WriteUint64(uint64_t v);
	// void WriteInt64(int64_t v);
	// void WriteFloat(float v);
	// void WriteDouble(double v);

	streamAdapter.WriteBool8(true);
	streamAdapter.WriteUint8(8);
	streamAdapter.WriteUint16(16);
	streamAdapter.WriteUint32(32);
	streamAdapter.WriteUint64(64);
	streamAdapter.WriteFloat(32.f);
	streamAdapter.WriteDouble(64.0);

	EATEST_VERIFY(streamAdapter.IsValid());

	// void WriteBool8(const bool* v, uint32_t count);
	// void WriteUint8(const uint8_t* v, uint32_t count);
	// void WriteInt8(const int8_t* v, uint32_t count);
	// void WriteUint16(const uint16_t* v, uint32_t count);
	// void WriteInt16(const int16_t* v, uint32_t count);
	// void WriteUint32(const uint32_t* v, uint32_t count);
	// void WriteInt32(const int32_t* v, uint32_t count);
	// void WriteUint64(const uint64_t* v, uint32_t count);
	// void WriteInt64(const int64_t* v, uint32_t count);
	// void WriteFloat(const float* v, uint32_t count);
	// void WriteDouble(const double* v, uint32_t count);
	b8Array[0] = true;
	b8Array[4] = true;
	streamAdapter.WriteBool8(b8Array, 5);

	u8Array[0] = 8;
	u8Array[4] = 9;
	streamAdapter.WriteUint8(u8Array, 5);

	u16Array[0] = 16;
	u16Array[4] = 17;
	streamAdapter.WriteUint16(u16Array, 5);

	u32Array[0] = 32;
	u32Array[4] = 33;
	streamAdapter.WriteUint32(u32Array, 5);

	u64Array[0] = 64;
	u64Array[4] = 65;
	streamAdapter.WriteUint64(u64Array, 5);

	f32Array[0] = 32;
	f32Array[4] = 33;
	streamAdapter.WriteFloat(f32Array, 5);

	d64Array[0] = 64;
	d64Array[4] = 65;
	streamAdapter.WriteDouble(d64Array, 5);

	EATEST_VERIFY(streamAdapter.IsValid());

	// void WriteString8(const char* string, uint32_t length);
	// template <typename String8>
	// void WriteString8(const String8& string);
	// void WriteString16(const char16_t* string, uint32_t length);
	// template <typename String16>
	// void WriteString16(const String16& string);
	str8 = "abc";
	streamAdapter.WriteString8(str8.data(), static_cast<uint32_t>(str8.length()));
	streamAdapter.WriteString8(str8);

	str16 = EA_CHAR16("abc");
	streamAdapter.WriteString16(str16.data(), static_cast<uint32_t>(str16.length()));
	streamAdapter.WriteString16(str16);

	EATEST_VERIFY(streamAdapter.IsValid());

	// void ReadBool8(bool& v);
	// void ReadUint8(uint8_t& v);
	// void ReadInt8(int8_t& v);
	// void ReadUint16(uint16_t& v);
	// void ReadInt16(int16_t& v);
	// void ReadUint32(uint32_t& v);
	// void ReadInt32(int32_t& v);
	// void ReadUint64(uint64_t& v);
	// void ReadInt64(int64_t& v);
	// void ReadFloat(float& v);
	// void ReadDouble(double& v);
	memoryStream.SetPosition(0);    // We will be reading back what we wrote, so reset the stream back to the beginning.

	streamAdapter.ReadBool8(b8);
	streamAdapter.ReadUint8(u8);
	streamAdapter.ReadUint16(u16);
	streamAdapter.ReadUint32(u32);
	streamAdapter.ReadUint64(u64);
	streamAdapter.ReadFloat(f32);
	streamAdapter.ReadDouble(d64);

	EATEST_VERIFY((b8 == true) && (u8 == 8) && (u16 == 16) && (u32 == 32) && (u64 == 64) && (f32 == 32) && (d64 == 64));
	EATEST_VERIFY(streamAdapter.IsValid());

	// void ReadBool8(bool* v, uint32_t count);
	// void ReadUint8(uint8_t* v, uint32_t count);
	// void ReadUint16(uint16_t* v, uint32_t count);
	// void ReadUint32(uint32_t* v, uint32_t count);
	// void ReadUint64(uint64_t* v, uint32_t count);
	// void ReadInt8(int8_t* v, uint32_t count);
	// void ReadInt16(int16_t* v, uint32_t count);
	// void ReadInt32(int32_t* v, uint32_t count);
	// void ReadInt64(int64_t* v, uint32_t count);
	// void ReadFloat(float* v, uint32_t count);
	// void ReadDouble(double* v, uint32_t count);

	memset(b8Array, 0, sizeof(b8Array));
	streamAdapter.ReadBool8(b8Array, 5);
	EATEST_VERIFY((b8Array[0] == true) && (b8Array[4] == true));

	memset(u8Array, 0, sizeof(u8Array));
	streamAdapter.ReadUint8(u8Array, 5);
	EATEST_VERIFY((u8Array[0] == 8) && (u8Array[4] == 9));

	memset(u16Array, 0, sizeof(u16Array));
	streamAdapter.ReadUint16(u16Array, 5);
	EATEST_VERIFY((u16Array[0] == 16) && (u16Array[4] == 17));

	memset(u32Array, 0, sizeof(u32Array));
	streamAdapter.ReadUint32(u32Array, 5);
	EATEST_VERIFY((u32Array[0] == 32) && (u32Array[4] == 33));

	memset(u64Array, 0, sizeof(u64Array));
	streamAdapter.ReadUint64(u64Array, 5);
	EATEST_VERIFY((u64Array[0] == 64) && (u64Array[4] == 65));

	memset(f32Array, 0, sizeof(f32Array));
	streamAdapter.ReadFloat(f32Array, 5);
	EATEST_VERIFY((f32Array[0] == 32) && (f32Array[4] == 33));

	memset(d64Array, 0, sizeof(d64Array));
	streamAdapter.ReadDouble(d64Array, 5);
	EATEST_VERIFY((d64Array[0] == 64) && (d64Array[4] == 65));

	// size_t ReadString8(char* string, uint32_t capacity);
	// template <typename String8>
	// void ReadString8(String8& string);
	// size_t ReadString16(char16_t* string, uint32_t capacity);
	// template <typename String16>
	// void ReadString16(String16& string);

	str8.clear();
	str8.resize(3);
	nResult = streamAdapter.ReadString8(&str8[0], 4);
	EATEST_VERIFY((nResult == 3) && (str8 == "abc"));

	str8.clear();
	str8.resize(3);
	streamAdapter.ReadString8(str8);
	EATEST_VERIFY(str8 == "abc");

	str16.clear();
	str16.resize(3);
	nResult = streamAdapter.ReadString16(&str16[0], 4);
	EATEST_VERIFY((nResult == 3) && (str16 == EA_CHAR16("abc")));

	str8.clear();
	str8.resize(3);
	streamAdapter.ReadString16(str16);
	EATEST_VERIFY(str16 == EA_CHAR16("abc"));


	return nErrorCount;
}


static int TestStreamNumericalAdapters()
{
	using namespace EA::IO;

	EA::UnitTest::Report("    TestStreamNumericalAdapters\n");

	int nErrorCount(0);

	// Test numerical stream adapters
	{
		const EA::IO::size_type kBufferSize(1000);
		MemoryStream            memoryStream(new SharedPointer(kBufferSize), kBufferSize);

		// Write data.
		for(EA::IO::size_type i(0); i < 10; i++)
		{
			bool n8 = (i != 0);
			EATEST_VERIFY(WriteBool8(&memoryStream, n8));

			uint16_t n16 = (uint16_t)i;
			EATEST_VERIFY(WriteUint16(&memoryStream, n16));

			uint32_t n32 = (uint32_t)i;
			EATEST_VERIFY(WriteUint32(&memoryStream, n32));

			uint64_t n64 = (uint64_t)i;
			EATEST_VERIFY(WriteUint64(&memoryStream, n64));

			float f32 = (float)i;
			EATEST_VERIFY(WriteFloat(&memoryStream, f32));

			double f64 = (double)i;
			EATEST_VERIFY(WriteDouble(&memoryStream, f64));
		}

		// Read data.
		memoryStream.SetPosition(0);
		for(EA::IO::size_type i(0); i < 10; i++)
		{
			bool b8;
			EATEST_VERIFY(ReadBool8(&memoryStream, b8) && (b8 == (i != 0)));

			uint16_t n16;
			EATEST_VERIFY(ReadUint16(&memoryStream, n16) && (n16 == (uint16_t)i));

			uint32_t n32;
			EATEST_VERIFY(ReadUint32(&memoryStream, n32) && (n32 == (uint32_t)i));

			uint64_t n64;
			EATEST_VERIFY(ReadUint64(&memoryStream, n64) && (n64 == (uint64_t)i));

			float f32;
			EATEST_VERIFY(ReadFloat(&memoryStream, f32) && (f32 == (float)i));

			double f64;
			EATEST_VERIFY(ReadDouble(&memoryStream, f64) && (f64 == (double)i));
		}
	}

	return nErrorCount;
}


static int TestStreamStringAdapters()
{
	using namespace EA::IO;

	EA::UnitTest::Report("    TestStreamStringAdapters\n");

	int nErrorCount(0);

	// Test string stream adapters (8-bit)
	{
		const EA::IO::size_type  kBufferSize(20);
		MemoryStream             memoryStream(new SharedPointer(kBufferSize), kBufferSize);

		memoryStream.SetOption(MemoryStream::kOptionResizeEnabled, true);
		const char*  pString1 = "Past is prologue";
		const size_t    nString1 = EA::StdC::Strlen(pString1);
		const char*  pString2 = "The best way around is always through.";
		const size_t    nString2 = EA::StdC::Strlen(pString2);

		EATEST_VERIFY(WriteString(&memoryStream, pString1, (size_t)kLengthNull));
		EATEST_VERIFY(WriteString(&memoryStream, pString2, (size_t)nString2));

		memoryStream.SetPosition(0);
		char buffer[128];
		EATEST_VERIFY(ReadString(&memoryStream, (char *)NULL, 0) == nString1);
		EATEST_VERIFY(ReadString(&memoryStream, buffer, nString1 + 1) != kSizeTypeError); // +1 for NUL
		EATEST_VERIFY(EA::StdC::Strcmp(buffer, pString1) == 0);
		EATEST_VERIFY(ReadString(&memoryStream, buffer, sizeof(buffer)/sizeof(buffer[0])) != kSizeTypeError);
		EATEST_VERIFY(EA::StdC::Strcmp(buffer, pString2) == 0);
	}

	// Test string stream adapters (16-bit)
	{
		const EA::IO::size_type  kBufferSize(100);
		MemoryStream             memoryStream(new SharedPointer(kBufferSize), kBufferSize);

		memoryStream.SetOption(MemoryStream::kOptionResizeEnabled, true);
		const char16_t* pString1 = EA_CHAR16("She sells seashells by the seashore.");
		const size_t    nString1 = EA::StdC::Strlen(pString1);
		const char16_t* pString2 = EA_CHAR16("The quick brown fox quickly jumped over the lazy dog's back.");
		const size_t    nString2 = EA::StdC::Strlen(pString2);

		EATEST_VERIFY(WriteString(&memoryStream, pString1, kLengthNull));
		EATEST_VERIFY(WriteString(&memoryStream, pString2, nString2));

		memoryStream.SetPosition(0);
		char16_t buffer[128];
		EATEST_VERIFY(ReadString(&memoryStream, (char16_t *)NULL, 0) == nString1);
		EATEST_VERIFY(ReadString(&memoryStream, buffer, nString1 + 1) != kSizeTypeError);  // +1 for NUL
		EATEST_VERIFY(EA::StdC::Strcmp(buffer, pString1) == 0);
		EATEST_VERIFY(ReadString(&memoryStream, buffer, sizeof(buffer)/sizeof(buffer[0])) != kSizeTypeError);
		EATEST_VERIFY(EA::StdC::Strcmp(buffer, pString2) == 0);
	}

	return nErrorCount;
}


static int TestStreamLineAdapters()
{
	using namespace EA::IO;

	EA::UnitTest::Report("    TestStreamLineAdapters\n");

	int nErrorCount(0);

	{
		const EA::IO::size_type  kBufferSize(20);
		MemoryStream             memoryStream(new SharedPointer(kBufferSize), kBufferSize);

		memoryStream.SetOption(MemoryStream::kOptionResizeEnabled, true);
		const char*  pString1 = "When, in the course of human events, it becomes necessary";
		const char*  pString2 = "for one people to dissolve the political bands which have";
		const char*  pString3 = "connected them with another, and to assume,";
		const char*  pString4 = "among the Powers of the earth,";

		EATEST_VERIFY(WriteLine(&memoryStream, pString1, kLengthNull, kLineEndUnix));
		EATEST_VERIFY(WriteLine(&memoryStream, pString2, kLengthNull, kLineEndWindows));
		EATEST_VERIFY(WriteLine(&memoryStream, pString3, kLengthNull, kLineEndNewline));
		EATEST_VERIFY(WriteLine(&memoryStream, pString4, kLengthNull, kLineEndNone));

		memoryStream.SetPosition(0);
		char buffer[128];

		EATEST_VERIFY(ReadLine(&memoryStream, buffer, sizeof(buffer)/sizeof(buffer[0])) != kSizeTypeError);
		EATEST_VERIFY(strncmp(buffer, pString1, EA::StdC::Strlen(pString1)) == 0);
		EATEST_VERIFY(ReadLine(&memoryStream, buffer, sizeof(buffer)/sizeof(buffer[0])) != kSizeTypeError);
		EATEST_VERIFY(strncmp(buffer, pString2, EA::StdC::Strlen(pString2)) == 0);
		EATEST_VERIFY(ReadLine(&memoryStream, buffer, sizeof(buffer)/sizeof(buffer[0])) != kSizeTypeError);
		EATEST_VERIFY(strncmp(buffer, pString3, EA::StdC::Strlen(pString3)) == 0);
		EATEST_VERIFY(ReadLine(&memoryStream, buffer, sizeof(buffer)/sizeof(buffer[0])) != kSizeTypeError);
		EATEST_VERIFY(strncmp(buffer, pString4, EA::StdC::Strlen(pString4)) == 0);

		//template <typename String>
		//size_type ReadLine(IStream* pIS, String& s)
		eastl::string8 s8 = "hello\nworld\r\n\n";       // Three lines: "hello", "world", and ""
		memoryStream.SetData(&s8[0], s8.size() * sizeof(char), false, true);
		s8.clear();
		size_type n = ReadLine(&memoryStream, s8);
		EATEST_VERIFY((n == EA::StdC::Strlen("hello")) && (s8 == "hello"));
		n = ReadLine(&memoryStream, s8);
		EATEST_VERIFY((n == EA::StdC::Strlen("world")) && (s8 == "world"));
		n = ReadLine(&memoryStream, s8);
		EATEST_VERIFY((n == EA::StdC::Strlen("")) && (s8 == ""));

		eastl::string16 s16 = EA_CHAR16("hello\nworld\r\n\n");
		memoryStream.SetData(&s16[0], s16.size() * sizeof(char16_t), false, true);
		s16.clear();
		n = ReadLine(&memoryStream, s16, EA::IO::kEndianLocal);
		EATEST_VERIFY((n == EA::StdC::Strlen(EA_CHAR16("hello"))) && (s16 == EA_CHAR16("hello")));
		n = ReadLine(&memoryStream, s16, EA::IO::kEndianLocal);
		EATEST_VERIFY((n == EA::StdC::Strlen(EA_CHAR16("world"))) && (s16 == EA_CHAR16("world")));
		n = ReadLine(&memoryStream, s16, EA::IO::kEndianLocal);
		EATEST_VERIFY((n == EA::StdC::Strlen(EA_CHAR16(""))) && (s16 == EA_CHAR16("")));

		eastl::string32 s32 = EA_CHAR32("hello\nworld\r\n\n");
		memoryStream.SetData(&s32[0], s32.size() * sizeof(char32_t), false, true);
		s32.clear();
		n = ReadLine(&memoryStream, s32, EA::IO::kEndianLocal);
		EATEST_VERIFY((n == EA::StdC::Strlen(EA_CHAR32("hello"))) && (s32 == EA_CHAR32("hello")));
		n = ReadLine(&memoryStream, s32, EA::IO::kEndianLocal);
		EATEST_VERIFY((n == EA::StdC::Strlen(EA_CHAR32("world"))) && (s32 == EA_CHAR32("world")));
		n = ReadLine(&memoryStream, s32, EA::IO::kEndianLocal);
		EATEST_VERIFY((n == EA::StdC::Strlen(EA_CHAR32(""))) && (s32 == EA_CHAR32("")));
	}

	{
		#if defined(EA_PLATFORM_WINDOWS)

			// We write a large text file and read it line by line and verify that it isn't slow.
			const size_t       kFileSize = 200000;
			const size_t       kMaxLineSize = 100;
			char16_t           pPath[EA::IO::kMaxPathLength];
			EA::IO::size_type  size;

			EA::IO::MakeTempPathName(pPath, NULL, EA_CHAR16("FoundationTest"), EA_CHAR16(".txt"));
			EA::IO::FileStream fileStream(pPath);

			if(fileStream.Open(EA::IO::kAccessFlagReadWrite, EA::IO::kCDOpenAlways))
			{
				char buffer[kMaxLineSize];
				memset(buffer, 'a', kMaxLineSize);

				EA::UnitTest::Rand rng(EA::UnitTest::GetRandSeed());

				// Write the file.
				for(size_t n = 0, k = (uint32_t)rng.RandRange(0, kMaxLineSize); n < kFileSize; n += k)
				{
					EA::IO::WriteLine(&fileStream, buffer, k);
					k = (uint32_t)rng.RandRange(0, kMaxLineSize);
				}

				// Read the file.
				fileStream.SetPosition(0);

				while((size = EA::IO::ReadLine(&fileStream, buffer, kMaxLineSize)) < kSizeTypeDone) // While there there are more lines...
				{
					if(size > 0)
					{
						EATEST_VERIFY((buffer[0] == 'a') || (buffer[size - 1] == 'a'));
					}
				}

				fileStream.Close();
			}
			else
				nErrorCount++;

			EA::IO::File::Remove(pPath);

		#endif
	}

	return nErrorCount;
}


static int TestStreamEncoding()
{
	using namespace EA::IO;

	EA::UnitTest::Report("    TestStreamEncoding\n");

	int nErrorCount(0);

	{   // Standalone functions
		/*
		size_t ReadEncoded(IStream* pSource, char* pDestText, size_type nDestReadCapacity, EA::Text::Encoding srcEncoding, EA::Text::Encoding destEncoding);
		size_t ReadEncoded(IStream* pSource, char16_t* pDestText, size_type nDestReadCapacity, EA::Text::Encoding destEncoding);
		size_t ReadLineEncoded(IStream* pSource, char* pLine, size_type nLineCapacity, EA::Text::Encoding srcEncoding, EA::Text::Encoding destEncoding);
		size_t ReadLineEncoded(IStream* pSource, char16_t* pLine, size_type nLineCapacity, EA::Text::Encoding srcEncoding);
		bool   WriteEncoded(IStream* pDestination, const char* pSourceText, size_type nSourceWriteCount, EA::Text::Encoding srcEncoding, EA::Text::Encoding destEncoding);
		bool   WriteEncoded(IStream* pDestination, const char16_t* pSourceText, size_type nSourceWriteCount, EA::Text::Encoding destEncoding);
		bool   WriteLineEncoded(IStream* pDestination, const char* pSourceLine, size_type nSourceLineLength, EA::Text::Encoding srcEncoding, EA::Text::Encoding destEncoding, LineEnd lineEndToUse = kLineEndAuto);
		bool   WriteLineEncoded(IStream* pDestination, const char16_t* pSourceLine, size_type nSourceLineLength, EA::Text::Encoding destEncoding, LineEnd lineEndToUse = kLineEndAuto);
		*/
	}


	{   // class StreamEncoding
		// EA::COM::AutoRefCount<StreamEncoding> pStream = new StreamEncoding;
	}

	return nErrorCount;
}

#if EAIOTEST_APPLICATION_BUNDLES_SUPPORTED && defined(EA_PLATFORM_ANDROID)
	JavaVM *gJavaVM;

	// http://developer.android.com/training/articles/perf-jni.html#native_libraries
	extern "C" JNIEXPORT jint 
	JNI_OnLoad (JavaVM * vm, void * reserved)
	{
		gJavaVM = vm;
		return JNI_VERSION_1_6;
	}

	int gThreadTestErrorCount = 0;
	intptr_t AppBundleThreadTestFunction(void *attachThreadVoid)
	{
		using namespace EA::IO;
		char buffer[128];
		int nErrorCount(0);

		bool attachThread = *(bool*)attachThreadVoid;
		JNIEnv *env;
		if(attachThread)
		{
			gJavaVM->AttachCurrentThread(&env, NULL);
		}


		{
			FileStream fileStream("appbundle:/filea.txt");
			bool result = fileStream.Open();
			EATEST_VERIFY(result);
			int read = fileStream.Read(buffer, 50);
			EATEST_VERIFY(read == 8);
			fileStream.Close();
		}

		{
			FileStream fileStream("appbundle:/./filea.txt");
			bool result = fileStream.Open();
			EATEST_VERIFY(result);
			int read = fileStream.Read(buffer, 50);
			EATEST_VERIFY(read == 8);
			fileStream.Close();
		}

		{
			FileStream fileStream("appbundle:/subfolder/../filea.txt");
			bool result = fileStream.Open();
			EATEST_VERIFY(result);
			int read = fileStream.Read(buffer, 50);
			EATEST_VERIFY(read == 8);
			fileStream.Close();
		}

		if(attachThread)
		{
			gJavaVM->DetachCurrentThread();
		}

		gThreadTestErrorCount += nErrorCount;
		return 0;
	}
#endif


static int TestStreamApplicationBundle()
{
	using namespace EA::IO;

	EA::UnitTest::Report("    TestStreamApplicationBundle\n");

	int nErrorCount(0);

	#if EAIOTEST_APPLICATION_BUNDLES_SUPPORTED
		char buffer[128];

		{
			FileStream fileStream("appbundle:/fileb.txt");
			bool result = fileStream.Open();
			EATEST_VERIFY(result);
			int read = fileStream.Read(buffer, 50);
			EATEST_VERIFY(read == 8);
			fileStream.Close();
		}

		{
			FileStream fileStream("appbundle:/harry.mp3");
			bool result = fileStream.Open(kAccessFlagWrite);
			EATEST_VERIFY(!result);
		}

		{
			FileStream fileStream("appbundle:/harry.snr");
			bool result = fileStream.Open(kAccessFlagRead);
			EATEST_VERIFY(result);
			size_type fileSize = fileStream.GetSize();
			EATEST_VERIFY(fileSize == 67220);
		}

		{
			FileStream fileStream("appbundle:/harry.mp3");
			bool result = fileStream.Open();
			EATEST_VERIFY(result);
			size_type fileSize = fileStream.GetSize();
			EATEST_VERIFY(fileSize == 67220);
			int read;

			result = fileStream.SetPosition(6, kPositionTypeBegin);
			EATEST_VERIFY( fileStream.GetPosition(kPositionTypeBegin) == 6 );
			EATEST_VERIFY(result);
			read = fileStream.Read(buffer, 1);
			EATEST_VERIFY(read == 1);
			EATEST_VERIFY(buffer[0] == (char)0x83);

			result = fileStream.SetPosition(7993, kPositionTypeCurrent);
			EATEST_VERIFY( fileStream.GetPosition(kPositionTypeBegin) == 8000 );
			EATEST_VERIFY(result);
			read = fileStream.Read(buffer, 1);
			EATEST_VERIFY(read == 1);
			EATEST_VERIFY(buffer[0] == (char)1);

			result = fileStream.SetPosition(100, kPositionTypeBegin);
			EATEST_VERIFY(result);
			read = fileStream.Read(buffer, 1);
			EATEST_VERIFY(read == 1);
			EATEST_VERIFY(buffer[0] == (char)0xff);

			fileStream.Close();
		}

		{
			for(int i=0;i<512;i++)
			{
				FileStream fileStream("appbundle:/harry.snr");
				bool result = fileStream.Open();
				EATEST_VERIFY(result);
				fileStream.Close();
			}
		}

		{
			bool result = false;
			for(int i=0;i<512;i++)
			{
				FileStream fileStream("appbundle:/file_does_not_exist.txt");
				result |= fileStream.Open();
				fileStream.Close();
			}
			EATEST_VERIFY(!result);
		}

		{
			FileStream fileStream("appbundle:/subfolder/filec.txt");
			bool result = fileStream.Open();
			EATEST_VERIFY(result);
			int read = fileStream.Read(buffer, 50);
			EATEST_VERIFY(read == 20);
			fileStream.Close();
		}

		#ifdef EA_PLATFORM_ANDROID
			{
				bool attachThread = false;
				EA::Thread::Thread myThread;
				myThread.Begin(AppBundleThreadTestFunction, &attachThread);
				myThread.WaitForEnd();
			}

			{
				bool attachThread = true;
				EA::Thread::Thread myThread;
				myThread.Begin(AppBundleThreadTestFunction, &attachThread);
				myThread.WaitForEnd();
			}
			nErrorCount += gThreadTestErrorCount;
		#endif
	#endif

	return nErrorCount;
}



int TestStream()
{
	int nErrorCount(0);

	nErrorCount += TestMemoryStream();
	nErrorCount += TestFixedMemoryStream();
	nErrorCount += TestStreamBuffer();
	nErrorCount += BenchmarkStreamBuffer();
	nErrorCount += TestStreamChild();
	nErrorCount += TestStreamAdapter();
	nErrorCount += TestStreamNumericalAdapters();
	nErrorCount += TestStreamStringAdapters();
	nErrorCount += TestStreamLineAdapters();
	nErrorCount += TestStreamEncoding();
	nErrorCount += TestStreamApplicationBundle();

	return nErrorCount;
}



















