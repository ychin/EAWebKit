/////////////////////////////////////////////////////////////////////////////
// TestEAStreamCpp.cpp
//
// Copyright (c) 2006, Electronic Arts Inc. All rights reserved.
// Written by Paul Pedriana.
//
/////////////////////////////////////////////////////////////////////////////


#include <EAIO/internal/Config.h>
#include <EAIO/EAStreamCpp.h>
#include <EAStdC/EAString.h>
#include <EAIOTest/EAIO_Test.h>
#include <EATest/EATest.h>

EA_DISABLE_VC_WARNING(4350) // warning C4350: behavior change: 'std::_Wrap_alloc<_Alloc>::_Wrap_alloc(const std::_Wrap_alloc<_Alloc> &) throw()' called instead of 'std::_Wrap_alloc<_Alloc>::_Wrap_alloc<std::_Wrap_alloc<_Alloc>>(_Other &) throw()'
							// happens with vs2012 and winrt.  why?  instantiated std::vector<int>::push_back will cause this
#include <stdio.h>
#if EAIO_CPP_STREAM_ENABLED
	#include <fstream>
#endif
EA_RESTORE_VC_WARNING()

#include EA_ASSERT_HEADER

#if defined(EA_PLATFORM_SONY)
	unsigned int sceLibcHeapExtendedAlloc = 1;  // Switch to dynamic allocation
	size_t sceLibcHeapSize = SCE_LIBC_HEAP_SIZE_EXTENDED_ALLOC_NO_LIMIT; // No upper limit for heap area
#endif

///////////////////////////////////////////////////////////////////////////////
// TestEAStreamCpp
//
int TestEAStreamCpp()
{
	EA::UnitTest::Report("TestEAStreamCpp\n");

	int nErrorCount = 0;

	#if EAIO_CPP_STREAM_ENABLED

	using namespace EA::IO;

	//{ // Test for broken C++ Standard Library in XBox 360.
	//  // It incorrectly prints "good, bad, good, bad", whereas it should always print "bad, bad, bad, bad".
	//    std::fstream f;
	//
	//    f.seekg(1, std::ios::beg);
	//    EA::UnitTest::Report("state is %s\n", (f.rdstate() & (std::ios::failbit | std::ios::badbit)) ? "error" : "good");
	//
	//    f.seekg(1, std::ios::beg);
	//    EA::UnitTest::Report("state is %s\n", (f.rdstate() & (std::ios::failbit | std::ios::badbit)) ? "error" : "good");
	//
	//    f.seekg(1, std::ios::beg);
	//    EA::UnitTest::Report("state is %s\n", (f.rdstate() & (std::ios::failbit | std::ios::badbit)) ? "error" : "good");
	//
	//    f.seekg(1, std::ios::beg);
	//    EA::UnitTest::Report("state is %s\n", (f.rdstate() & (std::ios::failbit | std::ios::badbit)) ? "error" : "good");
	//}


	{ // Tests on an un-open file
		std::fstream      fs;
		EA::IO::StreamCpp s1(&fs, &fs);
		EA::IO::StreamCpp s2(s1);
		EA::IO::StreamCpp s3;

		// StreamCpp& operator=(const StreamCpp& x);
		s1 = s2;

		// int AddRef();
		// int Release();
		s1.AddRef();
		s1.AddRef();
		s1.Release();

		// uint32_t GetType() const;
		uint32_t type = s1.GetType();
		EATEST_VERIFY(type == EA::IO::StreamCpp::kStreamType);

		// int GetAccessFlags() const;
		int flags = s1.GetAccessFlags();
		EATEST_VERIFY(flags == EA::IO::kAccessFlagReadWrite);
		flags = s3.GetAccessFlags();
		EATEST_VERIFY(flags == 0);

		// int GetState() const;
		int state = s1.GetState();
		EATEST_VERIFY(state == EA::IO::kStateSuccess);
		state = s3.GetState();
		EATEST_VERIFY(state == EA::IO::kStateSuccess);

		// bool Close();
		bool bCloseResult = s1.Close();
		EATEST_VERIFY(bCloseResult);
		bCloseResult = s3.Close();
		EATEST_VERIFY(bCloseResult);

		// size_type GetSize() const;
		size_type sizeResult = s1.GetSize();
		EATEST_VERIFY(sizeResult == kSizeTypeError);
		sizeResult = s3.GetSize();
		EATEST_VERIFY(sizeResult == kSizeTypeError);

		// bool SetSize(size_type size);
		bool bResizeResult = s1.SetSize(1); (void)bResizeResult;    // Some older iostream implementatations mistakenly succeed this.
		EATEST_VERIFY(!bResizeResult);
		bResizeResult = s3.SetSize(1);
		EATEST_VERIFY(!bResizeResult);

		// off_type GetPosition(PositionType positionType = kPositionTypeBegin) const;
		off_type getPositionResult = s1.GetPosition();
		EATEST_VERIFY(getPositionResult == (off_type)kSizeTypeError);
		getPositionResult = s3.GetPosition();
		EATEST_VERIFY(getPositionResult == (off_type)kSizeTypeError);

		// This is a workaround that allows VS2008 and XBox360 to work as expected in the code below.
		s1.Clear();
		s3.Clear();

		// bool SetPosition(off_type position, PositionType positionType = kPositionTypeBegin);
		bool bSetPositionResult = s1.SetPosition(1);    // Some older iostream implementatations mistakenly succeed this.
		EATEST_VERIFY(!bSetPositionResult);
		bSetPositionResult = s3.SetPosition(1);
		EATEST_VERIFY(!bSetPositionResult);

		// size_type GetAvailable() const;
		size_type getAvailableResult = s1.GetAvailable();
		EATEST_VERIFY(getAvailableResult == kSizeTypeError);
		getAvailableResult = s1.GetAvailable();
		EATEST_VERIFY(getAvailableResult == kSizeTypeError);

		// size_type Read(void* pData, size_type nSize);
		char readBuffer[32];
		memset(readBuffer, 0, sizeof(readBuffer));
		size_type readResult = s1.Read(readBuffer, 32);
		EATEST_VERIFY(readResult == kSizeTypeError);
		readResult = s3.Read(readBuffer, 32);
		EATEST_VERIFY(readResult == kSizeTypeError);

		// bool Flush();
		bool bFlushResult = s1.Flush();
		EATEST_VERIFY(bFlushResult);
		bFlushResult = s3.Flush();
		EATEST_VERIFY(bFlushResult);

		// bool Write(const void* pData, size_type nSize);
		char writeBuffer[32];
		memset(writeBuffer, 0, sizeof(writeBuffer));
		bool writeResult;

		#if (!defined(__APPLE__) && !defined(EA_PLATFORM_UNIX)) || !defined(__GNUC__) // On Linux/GlibC writeResult => true, though I don't think it should be. But it's internal to the standard library.
			writeResult = s1.Write(writeBuffer, 32);
			EATEST_VERIFY(writeResult == false);
		#endif

		writeResult = s3.Write(writeBuffer, 32);
		EATEST_VERIFY(writeResult == false);
	}

	#endif // EAIO_CPP_STREAM_ENABLED

	return nErrorCount;
}





