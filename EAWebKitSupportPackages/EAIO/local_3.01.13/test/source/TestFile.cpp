///////////////////////////////////////////////////////////////////////////////
// TestFile.cpp
//
// Copyright (c) 2003 Electronic Arts. All rights reserved.
//
// Created by Paul Pedriana
///////////////////////////////////////////////////////////////////////////////


#include <EABase/eabase.h>

#include <EAIO/internal/Config.h>
#include <EAIO/EAStream.h>
#include <EAIO/EAFileStream.h>
#include <EAIO/EAFileDirectory.h>
#include <EAIO/EAFileUtil.h>
#include <EAIO/FnEncode.h>
#include <EAIO/PathString.h>
#include <EASTL/string.h>
#include <EAStdC/EAString.h>
#include <EAStdC/EASprintf.h>
#include <EAStdC/EADateTime.h>
#include <EAIOTest/EAIO_Test.h>
#include <EATest/EATest.h>
#include <coreallocator/icoreallocator_interface.h>
#include EA_ASSERT_HEADER


#ifdef _MSC_VER
	#pragma warning(push, 0)
#endif

#include <stdio.h>
#include <limits.h>
#if defined(EA_PLATFORM_MICROSOFT)
	#include <crtdbg.h>
#endif

#if defined(EA_PLATFORM_MICROSOFT)
	#if !(defined(EA_PLATFORM_WINDOWS) && !EA_WINAPI_FAMILY_PARTITION(EA_WINAPI_PARTITION_DESKTOP))
		#include <WinSock2.h>
	#endif
	#include <windows.h>
#endif

#ifdef _MSC_VER
	#pragma warning(pop)
#endif

#if defined(EA_PLATFORM_SONY)
	#define TIME_TEST_SKIP_HOST_FILE_SYS L"/app0/"
#endif

#if defined(EA_PLATFORM_NX)
	// The NX requires a mount volume in the path or else it will throw exceptions and call abort
	#define TEST_MOUNT "tmp:/temp/" // temp dir specified in EAIO_Test.cpp
#else
	#define TEST_MOUNT ""
#endif

namespace TestLocal
{
	/// is_filled
	///
	/// Returns true if all elements in the range [first, last] are equal to value.
	///
	template <typename ForwardIterator, typename T>
	bool is_filled(ForwardIterator first, ForwardIterator last, const T& value)
	{
		while((first != last) && (*first == value))
			++first;

		return (first == last);
	}


	// Writes the time_t value into pBuffer as local time.
	// pBuffer must hold at least 64 characters.
	// This function is thread-safe only if you pass in your own pBuffer.
	char* FormatTimeAsString(time_t t, char* pBuffer = NULL)
	{
		static char buffer[64];

		if(!pBuffer)
			pBuffer = buffer;

		struct tm* const pTime = localtime(&t);
		sprintf(pBuffer, "%u/%u/%u %u:%u:%u", (uint32_t)pTime->tm_year + 1900, (uint32_t)pTime->tm_mon + 1, (uint32_t)pTime->tm_mday, (uint32_t)pTime->tm_hour, (uint32_t)pTime->tm_min, (uint32_t)pTime->tm_sec);

		return pBuffer;
	}


	// Can't use regular abs, because it may not support time_t.
	time_t time_t_abs(time_t t)
	{
		if(t < 0)
			t = (time_t)-t;
		return t;
	}

} // namespace



///////////////////////////////////////////////////////////////////////////////
// CreateExpectedFile
//
// This function creates an 'expected' file, which is a file that has a certain
// pattern of bytes. The reason for this pattern of bytes is that we want to verify
// copying, renaming, and moving of files and a way to do this is to verify that
// the destination file is the right size and byte pattern.
//
// The byte pattern we use is simply:
//     0x00 0x01 0x02 0x03 0x04... 0xEE 0xEF 0x00 0x01 0x02...
//
static bool CreateExpectedFile(const wchar_t* pFilePath, EA::IO::size_type expectedFileSize)
{
	bool bReturnValue = false;
	EA::IO::FileStream fileStream(pFilePath);

	uint8_t* buffer = (uint8_t*)EA::Allocator::ICoreAllocator::GetDefaultAllocator()->Alloc(expectedFileSize, "CreateExpectedFileBuffer", EA::Allocator::MEM_TEMP, 0);

	if(fileStream.Open(EA::IO::kAccessFlagWrite, EA::IO::kCDCreateAlways, 0, 0)) // Create if non-existent; resize to zero if existent.
	{
		bReturnValue = true;

		for(EA::IO::size_type i = 0, iWrite = 0; i < expectedFileSize; i++, iWrite++)
		{
			if(iWrite == (0xEF + 1))
				iWrite = 0;

			buffer[i] = (uint8_t)iWrite;
		}

		bReturnValue = fileStream.Write(buffer, expectedFileSize);
		fileStream.Close();
	}
	
	EA::Allocator::ICoreAllocator::GetDefaultAllocator()->Free(buffer, expectedFileSize);

	return bReturnValue;
}


///////////////////////////////////////////////////////////////////////////////
// ValidateExpectedFile
//
// This function verifies an 'expected' file of the given size. See the
// CreateExpectedFile function for an explanation of an 'expected' file.
//
static bool ValidateExpectedFile(const wchar_t* pFilePath, EA::IO::size_type expectedFileSize)
{
	bool bReturnValue = false;
	EA::IO::FileStream fileStream(pFilePath);
	uint8_t* expectedBuffer = (uint8_t*)EA::Allocator::ICoreAllocator::GetDefaultAllocator()->Alloc(expectedFileSize, "ValidateExpectedFileExpectedBuffer", EA::Allocator::MEM_TEMP, 0);
	uint8_t* readBuffer = (uint8_t*)EA::Allocator::ICoreAllocator::GetDefaultAllocator()->Alloc(expectedFileSize, "ValidateExpectedFileReadBuffer", EA::Allocator::MEM_TEMP, 0);

	if(fileStream.Open(EA::IO::kAccessFlagRead, EA::IO::kCDOpenExisting, 0, 0))
	{
		bReturnValue = true;

		for(EA::IO::size_type i = 0, iExpected = 0; i < expectedFileSize; i++, iExpected++)
		{
			if(iExpected == (0xEF + 1))
				iExpected = 0;

			expectedBuffer[i] = (uint8_t) iExpected;
		}

		bReturnValue = fileStream.Read(readBuffer, expectedFileSize) == expectedFileSize;

		for(EA::IO::size_type i = 0; i < expectedFileSize && bReturnValue; i++)
		{
			bReturnValue = expectedBuffer[i] == readBuffer[i];
		}

		fileStream.Close();
	}

	EA::Allocator::ICoreAllocator::GetDefaultAllocator()->Free(expectedBuffer, expectedFileSize);
	EA::Allocator::ICoreAllocator::GetDefaultAllocator()->Free(readBuffer, expectedFileSize);

	return bReturnValue;
}



///////////////////////////////////////////////////////////////////////////////
// TestFileUtil
//
static int TestFileUtil()
{
	using namespace EA::IO;

	EA::UnitTest::Report("    TestFileUtil\n");

	int             nErrorCount(0);
	wchar_t         pDirectoryW[kMaxDirectoryLength]; pDirectoryW[0] = L'\0';
	char         pDirectory8[kMaxDirectoryLength]; pDirectory8[0] = '\0';
	eastl::wstring  sTempW;
	eastl::string8  sTemp8;
	eastl::wstring  sLongFileNameW;
	eastl::string8  sLongFileName8;
	size_type       n;
	int             i;
	uint64_t        g = 0;
	bool            b;
	#if !defined(EA_PLATFORM_NX) // There is no way to get file time on NX (other than a no-ship, debug only function)
		time_t          t; 
	#endif
	const float     fDiskSpeed = EA::UnitTest::GetSystemSpeed(EA::UnitTest::kSpeedTypeDisk);

	{
		sLongFileNameW.resize(kMaxPathLength * 4);
		sLongFileName8.resize(kMaxPathLength * 4);

		for(eastl_size_t f = 0, fEnd = sLongFileNameW.length(); f < fEnd; f++)
		{
			sLongFileNameW[f] = 'a';
			sLongFileName8[f] = 'a';
		}
	}

	{
		// bool EA::IO::File::Exists(const char* pPath);
		// bool EA::IO::File::Exists(const char16_t* pPath);
		// bool EA::IO::File::Exists(const char32_t* pPath);

		// bool EA::IO::File::PatternExists(const char* pPathPattern); 
		// bool EA::IO::File::PatternExists(const char16_t* pPathPattern); 
		// bool EA::IO::File::PatternExists(const char32_t* pPathPattern); 

		// bool EA::IO::File::Create(const char* pPath, bool bTruncate = false);
		// bool EA::IO::File::Create(const char16_t* pPath, bool bTruncate = false);
		// bool EA::IO::File::Create(const char32_t* pPath, bool bTruncate = false);

		// bool EA::IO::File::Remove(const char* pPath);
		// bool EA::IO::File::Remove(const char16_t* pPath);
		// bool EA::IO::File::Remove(const char32_t* pPath);

		// Test empty paths
		b = File::Exists(EA_WCHAR(""));
		EATEST_VERIFY(!b);
		b = File::Exists("");
		EATEST_VERIFY(!b);

		#if EAIOTEST_APPLICATION_BUNDLES_SUPPORTED
			if(gbVerboseOutput)
				EA::UnitTest::Report("Testing appbundle exists\n");
			b = File::Exists(EA_WCHAR("appbundle:/filea.txt"));
			EATEST_VERIFY(b);
			b = File::Exists("appbundle:/filea.txt");
			EATEST_VERIFY(b);
			b = File::Exists(EA_WCHAR("appbundle:/missing_file.txt"));
			EATEST_VERIFY(!b);
			b = File::Exists("appbundle:/missing_file.txt");
			EATEST_VERIFY(!b);
			
			b = Directory::Exists(EA_WCHAR("appbundle:/subfolder/"));
			EATEST_VERIFY(b);
			b = Directory::Exists("appbundle:/subfolder/");
			EATEST_VERIFY(b);
			b = Directory::Exists("appbundle:/non_existant_folder/");
			EATEST_VERIFY(!b);

			b = File::Exists("appbundle:/subfolder/../filea.txt");
			EATEST_VERIFY(b);

			b = File::Exists("appbundle:/subfolder/.././filea.txt");
			EATEST_VERIFY(b);
		#endif

		b = File::PatternExists(EA_WCHAR("")); 
		EATEST_VERIFY(!b);
		b = File::PatternExists(""); 
		EATEST_VERIFY(!b);

		b = File::Create(EA_WCHAR(""), true);
		EATEST_VERIFY(!b);
		b = File::Create("", true);
		EATEST_VERIFY(!b);

		b = File::Create(EA_WCHAR(""), false);
		EATEST_VERIFY(!b);
		b = File::Create("", false);
		EATEST_VERIFY(!b);

		b = File::Remove(EA_WCHAR(""));
		EATEST_VERIFY(!b);
		b = File::Remove("");
		EATEST_VERIFY(!b);

		// Test known-nonexistant paths
		b = File::Exists(EA_WCHAR(TEST_MOUNT "asdfasdfasdf.asdf"));
		EATEST_VERIFY(!b);
		b = File::Exists(TEST_MOUNT "asdfasdfasdf.asdf");
		EATEST_VERIFY(!b);

		b = File::PatternExists(EA_WCHAR(TEST_MOUNT "asdfasdfasdf.asdf"));
		EATEST_VERIFY(!b);
		b = File::PatternExists(TEST_MOUNT "asdfasdfasdf.asdf");
		EATEST_VERIFY(!b);

		b = File::Remove(EA_WCHAR(TEST_MOUNT "asdfasdfasdf.asdf"));
		EATEST_VERIFY(!b);
		b = File::Remove(TEST_MOUNT "asdfasdfasdf.asdf");
		EATEST_VERIFY(!b);


		// Test paths that are too long to be valid.
		#if !defined(EA_PLATFORM_NX) // The NX will abort if a path name is too long when calling fs functions.
			sTempW = gRootDirectoryW; sTempW += sLongFileNameW;
			sTemp8  = gRootDirectory8;  sTemp8  += sLongFileName8;

			b = File::Exists(sTempW.c_str());
			EATEST_VERIFY(!b);
			b = File::Exists(sTemp8.c_str());
			EATEST_VERIFY(!b);

			b = File::PatternExists(sTempW.c_str()); 
			EATEST_VERIFY(!b);
			b = File::PatternExists(sTemp8.c_str()); 
			EATEST_VERIFY(!b);

			b = File::Create(sTempW.c_str(), true);
			EATEST_VERIFY(!b);
			b = File::Create(sTemp8.c_str(), true);
			EATEST_VERIFY(!b);

			b = File::Create(sTempW.c_str(), false);
			EATEST_VERIFY(!b);
			b = File::Create(sTemp8.c_str(), false);
			EATEST_VERIFY(!b);

			b = File::Remove(sTempW.c_str());
			EATEST_VERIFY(!b);
			b = File::Remove(sTemp8.c_str());
			EATEST_VERIFY(!b);
		#endif

		// Test some file that is known to exist.
		sTempW = gRootDirectoryW; sTempW += EA_WCHAR("EAIO test file16.txt");  // The code below depends on this being at least a certain length.
		sTemp8  = gRootDirectory8;  sTemp8  +=  "EAIO test file8.txt";

		if(File::Create(sTempW.c_str(), true) && 
		   File::Create(sTemp8.c_str(), true))
		{
			b = File::Exists(sTempW.c_str());
			EATEST_VERIFY(b);
			b = File::Exists(sTemp8.c_str());
			EATEST_VERIFY(b);
	
			int attributes = File::GetAttributes(sTempW.c_str());
			EATEST_VERIFY((attributes & (kAttributeReadable | kAttributeWritable)) == (kAttributeReadable | kAttributeWritable));
			
			attributes = File::GetAttributes(sTemp8.c_str());
			EATEST_VERIFY((attributes & (kAttributeReadable | kAttributeWritable)) == (kAttributeReadable | kAttributeWritable));

			b = File::PatternExists(sTempW.c_str()); 
			EATEST_VERIFY(b);
			b = File::PatternExists(sTemp8.c_str()); 
			EATEST_VERIFY(b);

			eastl::wstring sTempPatternW(sTempW); sTempPatternW.resize(sTempW.size() - 8); sTempPatternW += '*';
			eastl::string8 sTempPattern8(sTemp8); sTempPattern8.resize(sTemp8.size() - 8); sTempPattern8 += '*';

			b = File::PatternExists(sTempPatternW.c_str()); 
			EATEST_VERIFY(b);
			b = File::PatternExists(sTempPattern8.c_str()); 
			EATEST_VERIFY(b);
		}

		if(File::Exists(sTempW.c_str()))
		{
			b = File::Remove(sTempW.c_str());
			EATEST_VERIFY(b);
		}

		if(File::Exists(sTemp8.c_str()))
		{
			b = File::Remove(sTemp8.c_str());
			EATEST_VERIFY(b);
		}
	}

	{
		// bool EA::IO::File::Move(const char*  pPathSource,  const char*  pPathDestination, bool bOverwriteIfPresent = true);
		// bool EA::IO::File::Move(const char16_t* pPathSource,  const char16_t* pPathDestination, bool bOverwriteIfPresent = true);
		// bool EA::IO::File::Move(const char32_t* pPathSource,  const char16_t* pPathDestination, bool bOverwriteIfPresent = true);

		// bool EA::IO::File::Rename(const char*  pPathSource,  const char*  pPathDestination, bool bOverwriteIfPresent = true);
		// bool EA::IO::File::Rename(const char16_t* pPathSource,  const char16_t* pPathDestination, bool bOverwriteIfPresent = true);
		// bool EA::IO::File::Rename(const char32_t* pPathSource,  const char32_t* pPathDestination, bool bOverwriteIfPresent = true);

		// bool EA::IO::File::Copy(const char*  pPathSource,  const char*  pPathDestination, bool bOverwriteIfPresent = true);
		// bool EA::IO::File::Copy(const char16_t* pPathSource,  const char16_t* pPathDestination, bool bOverwriteIfPresent = true);
		// bool EA::IO::File::Copy(const char32_t* pPathSource,  const char32_t* pPathDestination, bool bOverwriteIfPresent = true);

		// Test empty file paths
		b = File::Move(EA_WCHAR(TEST_MOUNT ""), EA_WCHAR(TEST_MOUNT ""), false);
		EATEST_VERIFY(!b);

		b = File::Rename(EA_WCHAR(TEST_MOUNT ""), EA_WCHAR(TEST_MOUNT ""), false);
		EATEST_VERIFY(!b);

		b = File::Copy(EA_WCHAR(TEST_MOUNT ""), EA_WCHAR(TEST_MOUNT ""), false);
		EATEST_VERIFY(!b);


		// Test non-existant source file paths.
		sTempW = gRootDirectoryW; sTempW += EA_WCHAR("EAIO test file16.txt");  // The code below depends on this being at least a certain length.
		sTemp8  = gRootDirectory8;  sTemp8  +=  "EAIO test file8.txt";

		sLongFileNameW = sTempW + 'x';
		sLongFileName8  = sTemp8 + 'x';

		File::Remove(sTempW.c_str());          // Make sure they don't exist.
		File::Remove(sTemp8.c_str());
		File::Remove(sLongFileNameW.c_str());
		File::Remove(sLongFileName8.c_str());

		EATEST_VERIFY(!File::Exists(sTempW.c_str()));
		EATEST_VERIFY(!File::Exists(sTemp8.c_str()));
		EATEST_VERIFY(!File::Exists(sLongFileNameW.c_str()));
		EATEST_VERIFY(!File::Exists(sLongFileName8.c_str()));

		b = File::Copy(sTempW.c_str(), sLongFileNameW.c_str(), true);
		EATEST_VERIFY(!b);
		b = File::Copy(sTemp8.c_str(),  sLongFileName8.c_str(),  true);
		EATEST_VERIFY(!b);

		// To do: Verify that the copied contents are equivalent, and do this for multiple file sizes.

		b = File::Move(sTempW.c_str(), sLongFileNameW.c_str(), true);
		EATEST_VERIFY(!b);
		b = File::Move(sTemp8.c_str(),  sLongFileName8.c_str(),  true);
		EATEST_VERIFY(!b);

		// To do: Verify that the moved contents are equivalent, and do this for multiple file sizes.

		b = File::Rename(sTempW.c_str(), sLongFileNameW.c_str(), true);
		EATEST_VERIFY(!b);
		b = File::Rename(sTemp8.c_str(),  sLongFileName8.c_str(),  true);
		EATEST_VERIFY(!b);

		// Do some tests of 'expected' file content.
		const EA::IO::size_type sizeArray[] = { 1, 40, 256, 1000, 2700 };

		for(EA::IO::size_type j = 0; j < EAArrayCount(sizeArray); j++)
		{
			if((sizeArray[j] > 100) && (fDiskSpeed < 0.1f)) // If this test will take too long due to terrible debug disk speed...
				continue;

			wchar_t filePath1[EA::IO::kMaxPathLength];
			wchar_t filePath2[EA::IO::kMaxPathLength];

			EA::StdC::Strcpy(filePath1, gRootDirectoryW);
			EA::StdC::Strcat(filePath1, EA_WCHAR("EAIO expected file 1.txt"));
			EA::StdC::Strcpy(filePath2, gRootDirectoryW);
			EA::StdC::Strcat(filePath2, EA_WCHAR("EAIO expected file 2.txt"));

			b = CreateExpectedFile(filePath1, sizeArray[j]);
			EATEST_VERIFY(b);
			b = ValidateExpectedFile(filePath1, sizeArray[j]);
			EATEST_VERIFY(b);
			b = File::Copy(filePath1, filePath2, true);
			EATEST_VERIFY(b);
			b = ValidateExpectedFile(filePath2, sizeArray[j]);
			EATEST_VERIFY(b);
			File::Remove(filePath2);

			b = File::Move(filePath1, filePath2, true);
			EATEST_VERIFY(b);
			b = ValidateExpectedFile(filePath2, sizeArray[j]);
			EATEST_VERIFY(b);

			b = File::Rename(filePath2, filePath1, true);
			EATEST_VERIFY(b);
			b = ValidateExpectedFile(filePath1, sizeArray[j]);
			EATEST_VERIFY(b);
			File::Remove(filePath1);
		}

		// Test valid file paths.
		if(File::Create(sTempW.c_str(), true) && 
		   File::Create(sTemp8.c_str(), true))
		{
			// There is no way to get file time on NX (other than a no-ship, debug only function)
			#if !defined(EA_PLATFORM_NX)
			{
				// time_t File::GetTime(const char16_t* pPath, FileTimeType timeType);
				// time_t File::GetTime(const char* pPath, FileTimeType timeType);
				// bool   File::SetTime(const char16_t* pPath, int nFileTimeTypeFlags, time_t nTime);
				// bool   File::SetTime(const char* pPath, int nFileTimeTypeFlags, time_t nTime);
				// bool   File::Stat(const char*  pPath, StatData& statData);
				// bool   File::Stat(const char16_t* pPath, StatData& statData);

				time_t       timeNow      = (time_t)(EA::StdC::GetTime() / UINT64_C(1000000000));
				time_t       timeLastWeek = timeNow - 20000;
				StatData     statData;
				const time_t maxDifference = 60;

				t = File::GetTime(sTempW.c_str(), EA::IO::kFileTimeTypeLastModification);
				time_t difference = TestLocal::time_t_abs(t - timeNow);
				EATEST_VERIFY_F(difference < maxDifference, "TestFileUtil/File::GetTime(kFileTimeTypeLastModification): Difference was %d and we expected less than %d", (int)difference, (int)maxDifference);

				if(File::Stat(sTempW.c_str(), statData))
				{
					difference = TestLocal::time_t_abs(statData.mModificationTime - timeNow);
					EATEST_VERIFY_F(difference < maxDifference, "TestFileUtil/File::Stat: Difference was %d and we expected less than %d", (int)difference, (int)maxDifference);
				}
				if(File::SetTime(sTempW.c_str(), EA::IO::kFileTimeTypeLastModification, timeLastWeek)) // Some platforms don't support this.
				{
					t = File::GetTime(sTempW.c_str(), EA::IO::kFileTimeTypeLastModification);
					difference = TestLocal::time_t_abs(t - timeLastWeek);
					EATEST_VERIFY_F(difference < maxDifference, "TestFileUtil/File::SetTime: Difference was %d and we expected less than %d", (int)difference, (int)maxDifference);
				}


				t = File::GetTime(sTemp8.c_str(), EA::IO::kFileTimeTypeLastModification);
				difference = TestLocal::time_t_abs(t - timeNow);
				EATEST_VERIFY_F(difference < maxDifference, "TestFileUtil/File::GetTime(kFileTimeTypeLastModification): Difference was %d and we expected less than %d", (int)difference, (int)maxDifference);

				if(File::Stat(sTemp8.c_str(), statData))
				{
					difference = TestLocal::time_t_abs(statData.mModificationTime - timeNow);
					EATEST_VERIFY_F(difference < maxDifference, "TestFileUtil/File::Stat: Difference was %d and we expected less than %d", (int)difference, (int)maxDifference);
				}
				if(File::SetTime(sTemp8.c_str(), EA::IO::kFileTimeTypeLastModification, timeLastWeek)) // Some platforms don't support this.
				{
					t = File::GetTime(sTemp8.c_str(), EA::IO::kFileTimeTypeLastModification);
					difference = TestLocal::time_t_abs(t - timeLastWeek);
					EATEST_VERIFY_F(difference < maxDifference, "TestFileUtil/File::SetTime: Difference was %d and we expected less than %d", (int)difference, (int)maxDifference);
				}
			}
			#endif

			eastl::wstring sMoveDestinationDirW(gRootDirectoryW); sMoveDestinationDirW += EA_WCHAR("DirW"); sMoveDestinationDirW += EA_FILE_PATH_SEPARATOR_16;
			eastl::string8 sMoveDestinationDir8(gRootDirectory8); sMoveDestinationDir8 +=          "Dir";  sMoveDestinationDir8 += EA_FILE_PATH_SEPARATOR_8;

			if(Directory::Create(sMoveDestinationDirW.c_str()) && 
			   Directory::Create(sMoveDestinationDir8.c_str()))
			{
				eastl::wstring sMoveDestinationW(sMoveDestinationDirW); sMoveDestinationW += EA_WCHAR("Test16.txt"); 
				eastl::string8 sMoveDestination8(sMoveDestinationDir8); sMoveDestination8 +=          "Test8.txt"; 

				// Move it to a subdirectory.
				b = File::Move(sTempW.c_str(), sMoveDestinationW.c_str(), true);
				EATEST_VERIFY(b);
				b = File::Move(sTemp8.c_str(), sMoveDestination8.c_str(), true);
				EATEST_VERIFY(b);

				// Make sure the old version is gone.
				b = File::Exists(sTempW.c_str());
				EATEST_VERIFY(!b);
				b = File::Exists(sTemp8.c_str());
				EATEST_VERIFY(!b);

				// Make sure the new version is present.
				b = File::Exists(sMoveDestinationW.c_str());
				EATEST_VERIFY(b);
				b = File::Exists(sMoveDestination8.c_str());
				EATEST_VERIFY(b);

				// Rename it.
				eastl::wstring sRenamedW(sMoveDestinationW); sRenamedW += EA_WCHAR(".xyz");
				eastl::string8 sRenamed8(sMoveDestination8); sRenamed8 +=  ".xyz";

				b = File::Rename(sMoveDestinationW.c_str(), sRenamedW.c_str(), true);
				EATEST_VERIFY(b);
				b = File::Rename(sMoveDestination8.c_str(), sRenamed8.c_str(), true);
				EATEST_VERIFY(b);

				// Make sure the new version is present.
				b = File::Exists(sRenamedW.c_str());
				EATEST_VERIFY(b);
				b = File::Exists(sRenamed8.c_str());
				EATEST_VERIFY(b);

				// Copy it back to the original location with the original name.
				b = File::Copy(sRenamedW.c_str(), sTempW.c_str(), true);
				EATEST_VERIFY(b);
				b = File::Copy(sRenamed8.c_str(), sTemp8.c_str(), true);
				EATEST_VERIFY(b);

				// Delete the renamed source file.
				b = File::Remove(sRenamedW.c_str());
				EATEST_VERIFY(b);
				b = File::Remove(sRenamed8.c_str());
				EATEST_VERIFY(b);

				// Make sure the removed files are removed.
				b = File::Exists(sRenamedW.c_str());
				EATEST_VERIFY(!b);
				b = File::Exists(sRenamed8.c_str());
				EATEST_VERIFY(!b);
			}
		}

		if(File::Exists(sTempW.c_str()))
		{
			b = File::Remove(sTempW.c_str());
			EATEST_VERIFY(b);
		}

		if(File::Exists(sTemp8.c_str()))
		{
			b = File::Remove(sTemp8.c_str());
			EATEST_VERIFY(b);
		}
	}

	{
		// size_type EA::IO::File::GetSize(const char16_t* pPath);
		// bool EA::IO::File::IsWritable(const char16_t* pPath);
		// int EA::IO::File::GetAttributes(const char16_t* pPath);
		// bool EA::IO::File::SetAttributes(const char16_t* pPath, int nAttributeMask, bool bEnable);
		// time_t EA::IO::File::GetTime(const char16_t* pPath, FileTimeType timeType);
		// bool EA::IO::File::SetTime(const char16_t* pPath, int nFileTimeTypeFlags, time_t nTime); 

		// size_type EA::IO::File::GetSize(const char* pPath);
		// bool EA::IO::File::IsWritable(const char* pPath);
		// int EA::IO::File::GetAttributes(const char* pPath);
		// bool EA::IO::File::SetAttributes(const char* pPath, int nAttributeMask, bool bEnable);
		// time_t EA::IO::File::GetTime(const char* pPath, FileTimeType timeType);
		// bool EA::IO::File::SetTime(const char* pPath, int nFileTimeTypeFlags, time_t nTime); 

		// Test empty paths.
		n = File::GetSize(EA_WCHAR(""));
		EATEST_VERIFY(n == EA::IO::kSizeTypeError);

		b = File::IsWritable(EA_WCHAR(""));
		EATEST_VERIFY(!b);

		i = File::GetAttributes(EA_WCHAR(""));
		EATEST_VERIFY(i == 0);

		b = File::SetAttributes(EA_WCHAR(""), 0, true);
		EATEST_VERIFY(!b);

		#if !defined(EA_PLATFORM_NX) // There is no way to get file time on NX (other than a no-ship, debug only function)
			t = File::GetTime(EA_WCHAR(""), EA::IO::kFileTimeTypeNone);
			EATEST_VERIFY(t == 0);
		#endif

		b = File::SetTime(EA_WCHAR(""), 0, 0); 
		EATEST_VERIFY(!b);

		#if EAIOTEST_APPLICATION_BUNDLES_SUPPORTED
			b = (File::GetSize("appbundle:/subfolder/../filea.txt") ==  File::GetSize("appbundle:/filea.txt")) && (File::GetSize("appbundle:/subfolder/../filea.txt") != 0);
			EATEST_VERIFY(b);
		#endif
	}

	{
		// ResolveAliasResult EA::IO::File::ResolveAlias(const char16_t* pPathSource, char16_t* pPathDestination);
		// bool EA::IO::File::CreateAlias(const char16_t* pDestinationPath, const char16_t* pShortcutPath, 
		//                               const char16_t* pShortcutDescription, const char16_t* pShortcutArguments);

		// ResolveAliasResult EA::IO::File::ResolveAlias(const char* pPathSource, char* pPathDestination);
		// bool EA::IO::File::CreateAlias(const char* pDestinationPath, const char* pShortcutPath, 
		//                               const char* pShortcutDescription, const char* pShortcutArguments);

		// Test empty paths.
		EA::IO::File::ResolveAliasResult r = File::ResolveAlias(EA_WCHAR(""), pDirectoryW, kMaxDirectoryLength);
		EATEST_VERIFY(r == EA::IO::File::kRARInvalid); // kRARNone, kRARAlias

		b = File::CreateAlias(EA_WCHAR(""), EA_WCHAR(""), EA_WCHAR(""), EA_WCHAR(""));
		EATEST_VERIFY(!b);

		// Windows supports explicit aliases (.lnk files) and implicit aliases (junctions).
		// Unix supports implicit aliases (symbolic links and hard links).
		// Mac supports implicit aliases (symbolic links and hard links) and smart aliases (called simply "aliases" by Apple).
		// Console platforms support no kind of file system aliases.
		//
		// As of this writing, we have support only for Windows explicit aliases.
		// 
		#ifdef EA_PLATFORM_WINDOWS
			// To do.
		#endif
	}

	{
		// bool EA::IO::Directory::Exists(const char16_t* pDirectoryW);
		// bool EA::IO::Directory::EnsureExists(const char16_t* pDirectoryW);
		// bool EA::IO::Directory::Create(const char16_t* pDirectoryW);
		// bool EA::IO::Directory::Remove(const char16_t* pDirectoryW, bool bAllowRecursiveRemoval = true);
		// bool EA::IO::Directory::Rename(const char16_t* pDirectoryOld, const char16_t* pDirectoryNew);

		// bool EA::IO::Directory::Exists(const char* pDirectoryW);
		// bool EA::IO::Directory::EnsureExists(const char* pDirectoryW);
		// bool EA::IO::Directory::Create(const char* pDirectoryW);
		// bool EA::IO::Directory::Remove(const char* pDirectoryW, bool bAllowRecursiveRemoval = true);
		// bool EA::IO::Directory::Rename(const char* pDirectoryOld, const char* pDirectoryNew);

		// Test empty paths.
		b = Directory::Exists(EA_WCHAR(""));
		EATEST_VERIFY(!b);

		b = Directory::EnsureExists(EA_WCHAR(""));
		EATEST_VERIFY(!b);

		b = Directory::Create(EA_WCHAR(""));
		EATEST_VERIFY(!b);

		b = Directory::Remove(EA_WCHAR(""), true);
		EATEST_VERIFY(!b);

		b = Directory::Rename(EA_WCHAR(""), EA_WCHAR(""));
		EATEST_VERIFY(!b);

		if(EA::IO::IsFilePathSeparator('/'))
		{
			char root[] = "/";
			const char* result = EA::IO::Path::GetPathComponentEnd(root, root + 1, 1);
			EATEST_VERIFY(result == (root + 1));
		}

		#if EAIOTEST_APPLICATION_BUNDLES_SUPPORTED
			b = Directory::EnsureExists("appbundle:/./subfolder");
			EATEST_VERIFY(b);
		#endif
	}

	{
		using namespace EA::IO::Path;
		// bool EA::IO::Directory::Copy(const char16_t* pDirectorySource, const char16_t* pDirectoryDestination, bool bRecursive, bool bOverwriteIfAlreadyPresent = true);
		// bool EA::IO::Directory::Copy(const char* pDirectorySource, const char* pDirectoryDestination, bool bRecursive, bool bOverwriteIfAlreadyPresent = true);
		PathStringW pSrcDirW(gRootDirectoryW);
		EnsureTrailingSeparator(pSrcDirW);
		EA::IO::Path::Append(pSrcDirW, PathStringW(EA_WCHAR("CopyTestSrc")));

		PathStringW pCopyDirW(gRootDirectoryW);
		EnsureTrailingSeparator(pCopyDirW);

		PathStringW pInnerDirW(pSrcDirW);
		EnsureTrailingSeparator(pInnerDirW);
		EA::IO::Path::Append(pInnerDirW, PathStringW(EA_WCHAR("InnerDir")));
		
		PathStringW pFileW(pInnerDirW);
		EnsureTrailingSeparator(pFileW);
		EA::IO::Path::Append(pFileW, PathStringW(EA_WCHAR("Batman.txt")));

		EA::IO::Path::Append(pCopyDirW, PathStringW(EA_WCHAR("CopyTestDest")));

		PathString8 pSrcDir8;
		PathString8 pCopyDir8;
		PathString8 pInnerDir8;
		PathString8 pFile8;
		ConvertPath(pSrcDir8, pSrcDirW.c_str());
		ConvertPath(pCopyDir8, pCopyDirW.c_str());
		ConvertPath(pInnerDir8, pInnerDirW.c_str());
		ConvertPath(pFile8, pFileW.c_str());

		// Test empty paths.
		b = Directory::Copy(EA_WCHAR(""), EA_WCHAR(""), true, false);
		EATEST_VERIFY(!b);

		//Test copy, nonrecursive
		b = Directory::EnsureExists(pSrcDirW.c_str());
		EATEST_VERIFY(b);

		if(Directory::Exists(pCopyDirW.c_str()))
			Directory::Remove(pCopyDirW.c_str(), true);

		b = Directory::Copy(pSrcDirW.c_str(), pCopyDirW.c_str(), false, false);
		EATEST_VERIFY(b);
		EATEST_VERIFY(Directory::Exists(pCopyDirW.c_str()));

		b = Directory::Copy(pSrcDirW.c_str(), pCopyDirW.c_str(), false, true);
		EATEST_VERIFY(b);

		//Test copy, recursive

		//Create something to be copied recursively
		Directory::Create(pInnerDirW.c_str());
		File::Create(pFileW.c_str());

		if(Directory::Exists(pCopyDirW.c_str()))
			Directory::Remove(pCopyDirW.c_str(), true);

		b = Directory::Copy(pSrcDirW.c_str(), pCopyDirW.c_str(), true, false);
		EATEST_VERIFY(b);

		Directory::Remove(pCopyDirW.c_str(), true);
		b = Directory::Copy(pSrcDirW.c_str(), pCopyDirW.c_str(), false, false);
		EATEST_VERIFY(b);

		b = Directory::Copy(pSrcDirW.c_str(), pCopyDirW.c_str(), true, true);
		EATEST_VERIFY(b);

		//char
		 // Test empty paths.
		b = Directory::Copy("", "", true, false);
		EATEST_VERIFY(!b);

		//Test copy, nonrecursive
		b = Directory::EnsureExists(pSrcDir8.c_str());
		EATEST_VERIFY(b);

		if(Directory::Exists(pCopyDir8.c_str()))
			Directory::Remove(pCopyDir8.c_str(), true);

		b = Directory::Copy(pSrcDir8.c_str(), pCopyDir8.c_str(), false, false);
		EATEST_VERIFY(b);
		EATEST_VERIFY(Directory::Exists(pCopyDir8.c_str()));

		b = Directory::Copy(pSrcDir8.c_str(), pCopyDir8.c_str(), false, true);
		EATEST_VERIFY(b);

		//Test copy, recursive

		//Create something to be copied recursively
		Directory::Create(pInnerDir8.c_str());
		File::Create(pFile8.c_str());

		if(Directory::Exists(pCopyDir8.c_str()))
			Directory::Remove(pCopyDir8.c_str(), true);

		b = Directory::Copy(pSrcDir8.c_str(), pCopyDir8.c_str(), true, false);
		EATEST_VERIFY(b);

		Directory::Remove(pCopyDir8.c_str(), true);
		b = Directory::Copy(pSrcDir8.c_str(), pCopyDir8.c_str(), false, false);
		EATEST_VERIFY(b);

		b = Directory::Copy(pSrcDir8.c_str(), pCopyDir8.c_str(), true, true);
		EATEST_VERIFY(b);
	}

	{
		// int EA::IO::Directory::GetAttributes(const char16_t* pPath);
		// bool EA::IO::Directory::SetAttributes(const char16_t* pPath, int nAttributeMask, bool bEnable);
		// bool EA::IO::Directory::SetAttributes(const char16_t* pBaseDirectory, int nAttributeMask, bool bEnable, bool bRecursive, bool bIncludeBaseDirectory = true);
		// time_t EA::IO::File::GetTime(const char16_t* pPath, FileTimeType timeType);
		// bool EA::IO::File::SetTime(const char16_t* pPath, int nFileTimeTypeFlags, time_t nTime); 

		// int EA::IO::Directory::GetAttributes(const char* pPath);
		// bool EA::IO::Directory::SetAttributes(const char* pPath, int nAttributeMask, bool bEnable);
		// bool EA::IO::Directory::SetAttributes(const char* pBaseDirectory, int nAttributeMask, bool bEnable, bool bRecursive, bool bIncludeBaseDirectory = true);
		// time_t EA::IO::File::GetTime(const char* pPath, FileTimeType timeType);
		// bool EA::IO::File::SetTime(const char* pPath, int nFileTimeTypeFlags, time_t nTime); 

		// Test empty paths.
		i = Directory::GetAttributes(EA_WCHAR(""));
		EATEST_VERIFY(i == 0);

		b = Directory::SetAttributes(EA_WCHAR(""), 0, false);
		EATEST_VERIFY(!b);

		b = Directory::SetAttributes(EA_WCHAR(""), 0, false, false, true);
		EATEST_VERIFY(!b);

		#if !defined(EA_PLATFORM_NX) // There is no way to get file time on NX (other than a no-ship, debug only function)
			t = File::GetTime(EA_WCHAR(""), EA::IO::kFileTimeTypeNone);
			EATEST_VERIFY(t == 0);
		#endif

		b = File::SetTime(EA_WCHAR(""), 0, 0);
		EATEST_VERIFY(!b);
	}

	{
		// int  EA::IO::Directory::GetCurrentWorkingDirectory(char16_t* pDirectoryW);
		// bool EA::IO::Directory::SetCurrentWorkingDirectory(const char16_t* pDirectoryW);

		// int  EA::IO::Directory::GetCurrentWorkingDirectory(char* pDirectoryW);
		// bool EA::IO::Directory::SetCurrentWorkingDirectory(const char* pDirectoryW);

		// Test empty paths.
		Directory::GetCurrentWorkingDirectory(pDirectoryW);
		Directory::SetCurrentWorkingDirectory(pDirectoryW);

		// Test user-reported bug
		char bufferBefore[64];                       memset(bufferBefore,  '!',  sizeof(bufferBefore));
		char inputDir[EA::IO::kMaxPathLength + 1];   memset(inputDir,     0xcc,  sizeof(inputDir));
		char bufferAfter[64];                        memset(bufferAfter,   '!',  sizeof(bufferAfter));

		inputDir[EA::IO::kMaxPathLength] = '\0'; // Initialize to empty string.
		EA::IO::Directory::GetCurrentWorkingDirectory(inputDir, EA::IO::kMaxPathLength);

		EATEST_VERIFY(TestLocal::is_filled(bufferBefore, bufferBefore + 64, '!'));
		EATEST_VERIFY(TestLocal::is_filled(bufferAfter,  bufferAfter  + 64, '!'));

		eastl::fill(inputDir, inputDir + EA::IO::kMaxPathLength, ' '); // Initialize to really long string.
		inputDir[EA::IO::kMaxPathLength] = '\0';
		EA::IO::Directory::GetCurrentWorkingDirectory(inputDir, EA::IO::kMaxPathLength);

		EATEST_VERIFY(TestLocal::is_filled(bufferBefore, bufferBefore + 64, '!'));
		EATEST_VERIFY(TestLocal::is_filled(bufferAfter,  bufferAfter  + 64, '!'));        
	}


	{ 
		// bool MakeTempPathName(char16_t* pDestPath, const char16_t* pDirectoryW = NULL, const char16_t* pFileName = NULL, const char16_t* pExtension = NULL, uint32_t nDestPathLength = kMaxPathLength);
		// bool MakeTempPathName(char* pDestPath, const char* pDirectoryW = NULL, const char* pFileName = NULL, const char* pExtension = NULL, uint32_t nDestPathLength = kMaxPathLength);

		// int GetTempDirectory(char16_t* pDirectoryW, uint32_t nPathLength = kMaxPathLength);
		// int GetTempDirectory(char* pDirectoryW, uint32_t nPathLength = kMaxPathLength);

		// bool SetTempDirectory(const char16_t* pDirectoryW);
		// bool SetTempDirectory(const char* pDirectoryW);

		// Temp file functionality
		{
			i = GetTempDirectory(pDirectoryW);
			EATEST_VERIFY((i > 0) && (*pDirectoryW != 0));

			#if defined(EA_PLATFORM_WINDOWS) || defined(EA_PLATFORM_UNIX)
				b = SetTempDirectory(pDirectoryW);
				EATEST_VERIFY(b);
			#endif

			#if !defined(EA_PLATFORM_IPHONE)// These platforms have standard file systems that are read-only.
				wchar_t pTempPath[kMaxPathLength];

				b = MakeTempPathName(pTempPath);
				EATEST_VERIFY(b && pTempPath[0]);
				if(b)
					File::Remove(pTempPath);
		
				b = MakeTempPathName(pTempPath, pDirectoryW);
				EATEST_VERIFY(b && pTempPath[0]);
				if(b)
					File::Remove(pTempPath);
		
				b = MakeTempPathName(pTempPath, NULL, EA_WCHAR("abc"));
				EATEST_VERIFY(b && pTempPath[0] && EA::StdC::Strstr(pTempPath, EA_WCHAR("abc")));
				if(b)
					File::Remove(pTempPath);
		
				b = MakeTempPathName(pTempPath, NULL, EA_WCHAR("abc"), EA_WCHAR(".def"));
				EATEST_VERIFY(b && pTempPath[0] && EA::StdC::Strstr(pTempPath, EA_WCHAR("abc")) && EA::StdC::Strstr(pTempPath, EA_WCHAR("def")));
				if(b)
					File::Remove(pTempPath);

				// Test the creation of many temp paths in a short period of time.
				const size_t kCount = 100;
				eastl::wstring pathArray[kCount];

				for(n = 0; n < kCount; n++)
				{
					b = MakeTempPathName(pTempPath);
					pathArray[n] = pTempPath;
					EATEST_VERIFY(b && pTempPath[0]);
				}
				for(n = 0; n < kCount; n++)
				   File::Remove(pathArray[n].c_str());
			#endif
		}

		{
			i = GetTempDirectory(pDirectory8);
			EATEST_VERIFY((i > 0) && (*pDirectory8 != 0));

			#if defined(EA_PLATFORM_WINDOWS) || defined(EA_PLATFORM_UNIX)
				b = SetTempDirectory(pDirectory8);
				EATEST_VERIFY(b);
			#endif

			#if !defined(EA_PLATFORM_IPHONE)// These platforms have standard file systems that are read-only.
				char pTempPath[kMaxPathLength];

				b = MakeTempPathName(pTempPath);
				EATEST_VERIFY(b && pTempPath[0]);
				if(b)
					File::Remove(pTempPath);
		
				b = MakeTempPathName(pTempPath, pDirectory8);
				EATEST_VERIFY(b && pTempPath[0]);
				if(b)
					File::Remove(pTempPath);
		
				b = MakeTempPathName(pTempPath, NULL, "abc");
				EATEST_VERIFY(b && pTempPath[0] && EA::StdC::Strstr(pTempPath, "abc"));
				if(b)
					File::Remove(pTempPath);
		
				b = MakeTempPathName(pTempPath, NULL, "abc", ".def");
				EATEST_VERIFY(b && pTempPath[0] && EA::StdC::Strstr(pTempPath, "abc") && EA::StdC::Strstr(pTempPath, "def"));
				if(b)
					File::Remove(pTempPath);

				// Test the creation of many temp paths in a short period of time.
				const size_t kCount = 100;
				eastl::string8 pathArray[kCount];

				for(n = 0; n < kCount; n++)
				{
					b = MakeTempPathName(pTempPath);
					pathArray[n] = pTempPath;
					EATEST_VERIFY(b && pTempPath[0]);
				}
				for(n = 0; n < kCount; n++)
				   File::Remove(pathArray[n].c_str());
			#endif
		}
	}

	{
		// uint64_t GetDriveFreeSpace(const char32_t* pPath);
		// uint64_t GetDriveFreeSpace(const char16_t* pPath);
		// uint64_t GetDriveFreeSpace(const char* pPath);

		// int GetDriveName(const char32_t* pPath, char32_t* pName);
		// int GetDriveName(const char16_t* pPath, char16_t* pName);
		// int GetDriveName(const char* pPath, char* pName);

		// int GetDriveSerialNumber(const char32_t* pPath, char32_t* pSerialNumber);
		// int GetDriveSerialNumber(const char16_t* pPath, char16_t* pSerialNumber);
		// int GetDriveSerialNumber(const char* pPath, char* pSerialNumber);

		// DriveType GetDriveTypeValue(const char32_t* pPath);
		// DriveType GetDriveTypeValue(const char16_t* pPath);
		// DriveType GetDriveTypeValue(const char* pPath);

		// bool IsVolumeAvailable(const char32_t* pPath, int timeoutMS = 2000);
		// bool IsVolumeAvailable(const char16_t* pPath, int timeoutMS = 2000);
		// bool IsVolumeAvailable(const char* pPath, int timeoutMS = 2000);
	}

	{
		// size_t GetDriveInfo(DriveInfo16* pDriveInfoArray = NULL, size_t nDriveInfoArrayLength = 0);
		// size_t GetDriveInfo(DriveInfo8* pDriveInfoArray = NULL, size_t nDriveInfoArrayLength = 0);

		DriveInfoW driveInfoArray[12];
		n = EA::IO::GetDriveInfo(driveInfoArray, 12);
		
		#if EA_WINAPI_FAMILY_PARTITION(EA_WINAPI_PARTITION_DESKTOP) // We don't currently require that other platforms enumerate drives
			EATEST_VERIFY(n > 0);
		#endif

		for(size_t j = 0; j < n; j++)
		{
			EA::IO::DriveInfoW::DriveInfoName driveInfoNameTemp(driveInfoArray[j].mName);

			// EATEST_VERIFY(driveInfoNameTemp.length() > 0);  // This isn't necessarily so. 
			// const wchar_t c = driveInfoNameTemp.back();
			// EATEST_VERIFY((c == ':') || !IsFilePathSeparator(c)); // This isn't necessarily so. 

			// We'll use mName with a '/' appended from here on.
			driveInfoNameTemp += EA_FILE_PATH_SEPARATOR_8;
			sTempW = driveInfoNameTemp.c_str();
			sTempW += EA_WCHAR("abc");
			sTempW += EA_FILE_PATH_SEPARATOR_W;
			sTempW += EA_WCHAR("def");

			// DriveType GetDriveTypeValue(const wchar_t* pPath);
			DriveType driveType = GetDriveTypeValue(driveInfoNameTemp.c_str());
			EATEST_VERIFY(driveType != kDriveTypeUnknown);
		
			if(driveInfoArray[j].mType == kDriveTypeFixed) // Any other drive will likely be absent and give error values.
			{
				// int EA::IO::GetDriveName(const wchar_t* pPath, wchar_t* pName);
				wchar_t pName[kMaxVolumeNameLength];
				i = GetDriveName(sTempW.c_str(), pName);

				#if defined(EA_PLATFORM_MICROSOFT)
					// Windows sometimes fails us; it enumerates a drive like Z: and then when we ask 
					// Windows about it we get an error saying it's invalid. So we have little choice
					// here but to cripple this unit test.
					if((driveInfoNameTemp[0] == 'c') || (driveInfoNameTemp[0] == 'C')) // Verify the result for C: and no others.
						EATEST_VERIFY_F(i >= 0, "GetDriveName failed. Return value: %d, Type: kDriveTypeFixed, Name: %ls, GetLastError: 0x%08x (%u)", i, sTempW.c_str(), GetLastError(), GetLastError());
				#else
					EATEST_VERIFY_F(i >= 0, "GetDriveName failed. Return value: %d, Type: kDriveTypeFixed, Name: %ls", i, sTempW.c_str());
				#endif

				#if defined(EA_PLATFORM_WINDOWS) // So far only implemented on these platforms.
					// uint64_t EA::IO::GetDriveFreeSpace(const wchar_t* pPath);
					g = GetDriveFreeSpace(driveInfoNameTemp.c_str());
					EATEST_VERIFY(g != UINT64_C(0xffffffffffffffff));

					g = GetDriveFreeSpace(sTempW.c_str());
					EATEST_VERIFY(g != UINT64_C(0xffffffffffffffff));

					#if defined(EA_PLATFORM_WINDOWS)
						// int EA::IO::GetDriveSerialNumber(const wchar_t* pPath, wchar_t* pSerialNumber);
						wchar_t pSerialNumber[kMaxVolumeSerialNumberLength];
						i = GetDriveSerialNumber(driveInfoNameTemp.c_str(), pSerialNumber);
						if((driveInfoNameTemp[0] == 'c') || (driveInfoNameTemp[0] == 'C')) // Verify the result for C: and no others.
							EATEST_VERIFY((i > 0) && (*pSerialNumber != 0));
					#endif
				#else
					EATEST_VERIFY(g == 0);
				#endif
			}
		}
	}


	{
		// int GetSpecialDirectory(SpecialDirectory specialDirectory, wchar_t* pDirectoryW, bool bEnsureDirectoryExistence = false, uint32_t nPathLength = kMaxDirectoryLength);
		// int GetSpecialDirectory(SpecialDirectory specialDirectory, char* pDirectoryW, bool bEnsureDirectoryExistence = false, uint32_t nPathLength = kMaxDirectoryLength);

		using namespace EA::IO::Path;

		i = GetSpecialDirectory(kSpecialDirectoryTemp, pDirectoryW, false);
		EATEST_VERIFY(i > 0);
		EATEST_VERIFY(GetHasTrailingSeparator(pDirectoryW)); 

		i = GetSpecialDirectory(kSpecialDirectoryOperatingSystem, pDirectoryW, false);
		EATEST_VERIFY(i > 0);
		EATEST_VERIFY(GetHasTrailingSeparator(pDirectoryW)); 

		// Microsoft appears to have deprecated this functionality on their systems.
		//i = GetSpecialDirectory(kSpecialDirectoryOperatingSystemTrash, pDirectoryW, false);
		//EATEST_VERIFY(i > 0);
		//EATEST_VERIFY(GetHasTrailingSeparator(pDirectoryW)); 

		i = GetSpecialDirectory(kSpecialDirectoryOperatingSystemFonts, pDirectoryW, false);
		EATEST_VERIFY(i > 0);
		EATEST_VERIFY(GetHasTrailingSeparator(pDirectoryW)); 

		#if !defined(EAIO_USE_UNIX_IO)
			i = GetSpecialDirectory(kSpecialDirectoryCurrentApplication, pDirectoryW, false);
			EATEST_VERIFY(i > 0);
			EATEST_VERIFY(GetHasTrailingSeparator(pDirectoryW)); 
		#endif

		i = GetSpecialDirectory(kSpecialDirectoryUserDesktop, pDirectoryW, false);
		EATEST_VERIFY(i > 0);
		EATEST_VERIFY(GetHasTrailingSeparator(pDirectoryW)); 

		i = GetSpecialDirectory(kSpecialDirectoryCommonDesktop, pDirectoryW, false);
		EATEST_VERIFY(i > 0);
		EATEST_VERIFY(GetHasTrailingSeparator(pDirectoryW)); 

		i = GetSpecialDirectory(kSpecialDirectoryUserApplicationData, pDirectoryW, false);
		EATEST_VERIFY(i > 0);
		EATEST_VERIFY(GetHasTrailingSeparator(pDirectoryW)); 

		i = GetSpecialDirectory(kSpecialDirectoryCommonApplicationData, pDirectoryW, false);
		EATEST_VERIFY(i > 0);
		EATEST_VERIFY(GetHasTrailingSeparator(pDirectoryW)); 

		i = GetSpecialDirectory(kSpecialDirectoryUserDocuments, pDirectoryW, false);
		EATEST_VERIFY(i > 0);
		EATEST_VERIFY(GetHasTrailingSeparator(pDirectoryW)); 

		i = GetSpecialDirectory(kSpecialDirectoryCommonDocuments, pDirectoryW, false);
		EATEST_VERIFY(i > 0);
		EATEST_VERIFY(GetHasTrailingSeparator(pDirectoryW)); 

		i = GetSpecialDirectory(kSpecialDirectoryUserMusic, pDirectoryW, false);
		EATEST_VERIFY(i > 0);
		EATEST_VERIFY(GetHasTrailingSeparator(pDirectoryW)); 

		i = GetSpecialDirectory(kSpecialDirectoryCommonMusic, pDirectoryW, false);
		EATEST_VERIFY(i > 0);
		EATEST_VERIFY(GetHasTrailingSeparator(pDirectoryW)); 

		i = GetSpecialDirectory(kSpecialDirectoryUserProgramsShortcuts, pDirectoryW, false);
		EATEST_VERIFY(i > 0);
		EATEST_VERIFY(GetHasTrailingSeparator(pDirectoryW)); 

		i = GetSpecialDirectory(kSpecialDirectoryCommonProgramsShortcuts, pDirectoryW, false);
		EATEST_VERIFY(i > 0);
		EATEST_VERIFY(GetHasTrailingSeparator(pDirectoryW)); 
	}

	return nErrorCount;
}

///////////////////////////////////////////////////////////////////////////////
// TestFileDirectory
//
static int TestFileDirectory()
{
	int nErrorCount(0);

	EA::UnitTest::Report("    TestFileDirectory\n");

	using namespace EA::IO;

	#if EAIOTEST_APPLICATION_BUNDLES_SUPPORTED
		if(gbVerboseOutput)
			EA::UnitTest::Report("\nAppBundle EntryFindFirst / EntryFindNext test\n");

	{
		EntryFindData* p = EntryFindFirst(EA_WCHAR("appbundle:/"), NULL);
		if(p)
		{
			do
			{
				if(gbVerboseOutput)
				{
					EA::UnitTest::Report("*****Entry: %ls\n", p->mName);
				}

				// 'subfolder' was a folder created for the tests assets explicitly.
				if(EA::StdC::Strcmp(p->mName, EA_WCHAR("subfolder/")) == 0)
				{
					EATEST_VERIFY(p->mbIsDirectory);
				}
				// The remaining folders are created automaticaly on some (but not all) Android
				// devices even though they don't exist in the .apk file.
				// See https://annoyingtechnicaldetails.wordpress.com/2012/08/09/fighting-to-simplify-androids-assetmanager/
				else if( EA::StdC::Strcmp(p->mName, EA_WCHAR("webkit/"))    == 0 ||   
						 EA::StdC::Strcmp(p->mName, EA_WCHAR("images/"))    == 0 || 
						 EA::StdC::Strcmp(p->mName, EA_WCHAR("kioskmode/")) == 0 || 
						 EA::StdC::Strcmp(p->mName, EA_WCHAR("databases/")) == 0 || 
						 EA::StdC::Strcmp(p->mName, EA_WCHAR("sounds/"))    == 0)
				{
					EATEST_VERIFY(p->mbIsDirectory);
				}
				else
				{
					EATEST_VERIFY(!p->mbIsDirectory);
				}

			} while(EntryFindNext(p));

			EntryFindFinish(p);
		}
	}

	{
		int numMatches = 0;
		EntryFindData* p = EntryFindFirst(EA_WCHAR("appbundle:/"), EA_WCHAR("*.txt"));
		if(p)
		{
			do{
				if(gbVerboseOutput)
					EA::UnitTest::Report("text file:, %ls\n", p->mName);
				numMatches++;
			} while(EntryFindNext(p));

			EntryFindFinish(p);

		}
		// We expect 2 text files in the application bundle
		EATEST_VERIFY(numMatches == 2);
	}

	{
		int numMatches = 0;
		EntryFindData* p = EntryFindFirst(EA_WCHAR("appbundle:/subfolder/"), EA_WCHAR("*.txt"));
		if(p)
		{
			do{
				if(gbVerboseOutput)
					EA::UnitTest::Report("text file:, %ls\n", p->mName);
				numMatches++;
			} while(EntryFindNext(p));
			EntryFindFinish(p);
		}
		// We expect 1 text file in the application bundle subfolder named 'subfolder'
		EATEST_VERIFY(numMatches == 1);
	}
	#endif // #if EAIOTEST_APPLICATION_BUNDLES_SUPPORTED

	{
		if(gbVerboseOutput)
			EA::UnitTest::Report("\nEntryFindFirst / EntryFindNext test #1\n");

		bool result = Directory::Exists(gRootDirectoryW);
		EATEST_VERIFY(result);

		EntryFindData* p = EntryFindFirst(gRootDirectoryW, NULL);

		if(p)
		{
			do{
				if(gbVerboseOutput)
					EA::UnitTest::Report("%ls\n", p->mName);
			} while(EntryFindNext(p));

			EntryFindFinish(p);
		}
	}

	{
		if(gbVerboseOutput)
			EA::UnitTest::Report("\nEntryFindFirst / EntryFindNext test #2\n");

		bool result = Directory::Exists(gRootDirectoryW);
		EATEST_VERIFY(result);

		{
			timeval tv;
			EA::StdC::GetTimeOfDay(&tv, NULL, true);    // We use EAStdC time functionality instead of built-in standard C time because the time function is broken on some platforms.
			if(gbVerboseOutput)
				EA::UnitTest::Report("date now: %s\n", TestLocal::FormatTimeAsString(tv.tv_sec));

			EntryFindData efd;

			if(EntryFindFirst(gRootDirectoryW, EA_WCHAR("*"), &efd))
			{
				do{
					// Avoid peeking at modified times of things outside the test folder as this can be problematic in rare cases
					// e.g. the system time on the target may have been reset resulting in some items having creation times in the future
					if (EA::StdC::Strcmp(efd.mName, EA_WCHAR("..\\")) == 0 || EA::StdC::Strcmp(efd.mName, EA_WCHAR("../")) == 0)
						continue;

					// wchar_t efd.mName[kMaxPathLength];
					if(gbVerboseOutput)
						EA::UnitTest::Report("%ls size: %I64u date modified: %s\n", efd.mName, efd.mSize, TestLocal::FormatTimeAsString(efd.mModificationTime));

					// bool efd.mbIsDirectory;
					EA::IO::Path::PathStringW sPath(efd.mDirectoryPath);
					sPath += efd.mName;

					if(efd.mbIsDirectory)
					{
						EATEST_VERIFY(EA::IO::Directory::Exists(sPath.c_str()));
					}
					else
						EATEST_VERIFY(EA::IO::File::Exists(sPath.c_str()));

					// The following time tests can fail if the file system is on another computer
					// and that computer has a different date setting than the currently running 
					// computer. An example of this is debug testing on a console platform where
					// the time() function returns the currently executing console's time, but the 
					// files themselves reside on a test PC which is set to a different date.
					// Skip this test on platforms that have this problem:
				#if defined(TIME_TEST_SKIP_HOST_FILE_SYS)
					if (EA::StdC::Strncmp(sPath.c_str(), TIME_TEST_SKIP_HOST_FILE_SYS, EA::StdC::Strlen(TIME_TEST_SKIP_HOST_FILE_SYS)) != 0)
				#endif
					{
						const time_t expectedTime = (time_t)(tv.tv_sec + 86400);

						EATEST_VERIFY_F(efd.mCreationTime < expectedTime, "EntryFindFirst/Next creation time test failed for path %ls. Expected: %I64u, actual: %I64u.", sPath.c_str(), (uint64_t)expectedTime, (uint64_t)efd.mCreationTime);              // This can fail if the file system was messed with. We use 86400 because that's the number of seconds in a day, and some file systems write file times in local time instead of UTC.
						EATEST_VERIFY_F(efd.mModificationTime < expectedTime, "EntryFindFirst/Next modification time test failed for path %ls. Expected: %I64u, actual: %I64u.", sPath.c_str(), (uint64_t)expectedTime, (uint64_t)efd.mModificationTime);  // This can fail if the file system was messed with.
					}

					// uint64_t efd.mSize;
					EATEST_VERIFY(efd.mSize < UINT64_C(10000000000));   // Sanity check

					// wchar_t efd.mDirectoryPath[kMaxPathLength];
					EATEST_VERIFY(EA::StdC::Strcmp(efd.mDirectoryPath, gRootDirectoryW) == 0);

					// wchar_t efd.mEntryFilterPattern[kMaxPathLength];
					EATEST_VERIFY(EA::StdC::Strcmp(efd.mEntryFilterPattern, EA_WCHAR("*")) == 0);

				} while(EntryFindNext(&efd));

				EntryFindFinish(&efd);
			}
		}
	}


	{
		wchar_t kDirSeparator[2] = { EA_FILE_PATH_SEPARATOR_8, 0 };

		if(gbVerboseOutput)
			EA::UnitTest::Report("\nDirectoryIterator tests\n");

		// Make some files and directories if they don't already exist.
		wchar_t pTempDir[kMaxPathLength];
		EA::StdC::Strcpy(pTempDir, gRootDirectoryW);
		EA::StdC::Strcat(pTempDir, EA_WCHAR("temp"));
		EA::StdC::Strcat(pTempDir, kDirSeparator);

		Directory::Remove(pTempDir);
		if(!Directory::Exists(pTempDir))
			Directory::Create(pTempDir);

		if(Directory::Exists(pTempDir))
		{
			wchar_t pTempDirA[kMaxPathLength];
			EA::StdC::Strcpy(pTempDirA, pTempDir);
			EA::StdC::Strcat(pTempDirA, EA_WCHAR("A"));
			EA::StdC::Strcat(pTempDirA, kDirSeparator);

			Directory::Remove(pTempDirA);
			if(!Directory::Exists(pTempDirA))
				Directory::Create(pTempDirA);

			wchar_t pTempFile1[kMaxPathLength];
			EA::StdC::Strcpy(pTempFile1, pTempDir);
			EA::StdC::Strcat(pTempFile1, EA_WCHAR("1.txt"));

			wchar_t pTempFile2[kMaxPathLength];
			EA::StdC::Strcpy(pTempFile2, pTempDir);
			EA::StdC::Strcat(pTempFile2, EA_WCHAR("2.doc"));

			wchar_t pTempFileA1[kMaxPathLength];
			EA::StdC::Strcpy(pTempFileA1, pTempDirA);
			EA::StdC::Strcat(pTempFileA1, EA_WCHAR("A1.txt"));

			wchar_t pTempFileA2[kMaxPathLength];
			EA::StdC::Strcpy(pTempFileA2, pTempDirA);
			EA::StdC::Strcat(pTempFileA2, EA_WCHAR("A2.doc"));

			if(!File::Exists(pTempFile1))
				if(!File::Create(pTempFile1))
					EATEST_VERIFY(false);

			if(!File::Exists(pTempFile2))
				if(!File::Create(pTempFile2))
					EATEST_VERIFY(false);

			if(!File::Exists(pTempFileA1))
				if(!File::Create(pTempFileA1))
					EATEST_VERIFY(false);

			if(!File::Exists(pTempFileA2))
				if(!File::Create(pTempFileA2))
					EATEST_VERIFY(false);


			// Try iterating the above
			DirectoryIterator dirIterator;
			DirectoryIterator::EntryList entryList;
			size_t result;

			// Test Read of a directory.
			entryList.clear();
			result = dirIterator.Read(pTempDirA, entryList, NULL, kDirectoryEntryFile, DirectoryIterator::kMaxEntryCountDefault, false);
			EATEST_VERIFY(result == 2);
			EATEST_VERIFY(entryList.size() == 2);
			for(DirectoryIterator::EntryList::iterator it = entryList.begin(); it != entryList.end(); ++it)
			{
				const DirectoryIterator::Entry& entry = *it;
				if(gbVerboseOutput)
					EA::UnitTest::Report("%ls%ls\n", pTempDirA, entry.msName.c_str());
			}
			if(gbVerboseOutput)
				EA::UnitTest::Report("\n");


			// Test Read of a directory with count limit.
			entryList.clear();
			result = dirIterator.Read(pTempDirA, entryList, NULL, kDirectoryEntryFile, 1, false);
			EATEST_VERIFY(result == 1);
			EATEST_VERIFY(entryList.size() == 1);
			for(DirectoryIterator::EntryList::iterator it = entryList.begin(); it != entryList.end(); ++it)
			{
				const DirectoryIterator::Entry& entry = *it;

				// wchar_t entry.mName[kMaxPathLength];
				if(gbVerboseOutput)
					EA::UnitTest::Report("%ls%ls\n", pTempDirA, entry.msName.c_str());
			}
			if(gbVerboseOutput)
				EA::UnitTest::Report("\n");

			{
				// Test ReadRecursive of a directory.
				timeval tv;
				EA::StdC::GetTimeOfDay(&tv, NULL, true);    // We use EAStdC time functionality instead of built-in standard C time because the time function is broken on some platforms.
				if(gbVerboseOutput)
					EA::UnitTest::Report("date now: %s\n", TestLocal::FormatTimeAsString(tv.tv_sec));

				entryList.clear();
				result = dirIterator.ReadRecursive(pTempDir, entryList, EA_WCHAR("*"), kDirectoryEntryFile | kDirectoryEntryDirectory, true, true, DirectoryIterator::kMaxEntryCountDefault, true);
				EATEST_VERIFY(result == 5);
				EATEST_VERIFY(entryList.size() == 5);
				for(DirectoryIterator::EntryList::iterator it = entryList.begin(); it != entryList.end(); ++it)
				{
					const DirectoryIterator::Entry& entry = *it;

					// wchar_t entry.mName[kMaxPathLength];
					if(gbVerboseOutput)
						EA::UnitTest::Report("%ls size: %I64u\n date modified: %s", entry.msName.c_str(), entry.mSize, TestLocal::FormatTimeAsString(entry.mModificationTime));

					// The following time tests can fail if the file system is on another computer
					// and that computer has a different date setting than the currently running 
					// computer. An example of this is debug testing on a console platform where
					// the time() function returns the currently executing console's time, but the 
					// files themselves reside on a test PC which is set to a different date.
					// Skip this test on platforms that have this problem:
				#if defined(TIME_TEST_SKIP_HOST_FILE_SYS)
					if (EA::StdC::Strncmp(entry.msName.c_str(),TIME_TEST_SKIP_HOST_FILE_SYS, EA::StdC::Strlen(TIME_TEST_SKIP_HOST_FILE_SYS)) != 0)
				#endif
					{    
						const time_t expectedTime = (time_t)(tv.tv_sec + 86400);

						EATEST_VERIFY_F(entry.mCreationTime < expectedTime, "DirectoryIterator creation time test failed for path %ls. Expected: %I64u, actual: %I64u.", entry.msName.c_str(), (uint64_t)expectedTime, (uint64_t)entry.mCreationTime);              // This can fail if the file system was messed with. We use 86400 because that's the number of seconds in a day, and some file systems write file times in local time instead of UTC.
						EATEST_VERIFY_F(entry.mModificationTime < expectedTime, "DirectoryIterator modification time test failed for path %ls. Expected: %I64u, actual: %I64u.", entry.msName.c_str(), (uint64_t)expectedTime, (uint64_t)entry.mModificationTime);  // This can fail if the file system was messed with.
					}

					EATEST_VERIFY(entry.mSize < UINT64_C(10000000000));   // Sanity check
				}
				if(gbVerboseOutput)
					EA::UnitTest::Report("\n");
			}


			// Test ReadRecursive of a directory with filter.
			entryList.clear();
			result = dirIterator.ReadRecursive(pTempDir, entryList, EA_WCHAR("*.doc"), kDirectoryEntryFile | kDirectoryEntryDirectory, true, true, DirectoryIterator::kMaxEntryCountDefault, false);
			EATEST_VERIFY(result == 2);
			EATEST_VERIFY(entryList.size() == 2);
			for(DirectoryIterator::EntryList::iterator it = entryList.begin(); it != entryList.end(); ++it)
			{
				const DirectoryIterator::Entry& entry = *it;
				if(gbVerboseOutput)
					EA::UnitTest::Report("%ls\n", entry.msName.c_str());
			}
			if(gbVerboseOutput)
				EA::UnitTest::Report("\n");


			// Test ReadRecursive of a directory with count limit.
			entryList.clear();
			result = dirIterator.ReadRecursive(pTempDir, entryList, EA_WCHAR("*"), kDirectoryEntryFile | kDirectoryEntryDirectory, true, true, 3, false);
			EATEST_VERIFY(result == 3);
			EATEST_VERIFY(entryList.size() == 3);
			for(DirectoryIterator::EntryList::iterator it = entryList.begin(); it != entryList.end(); ++it)
			{
				const DirectoryIterator::Entry& entry = *it;
				if(gbVerboseOutput)
					EA::UnitTest::Report("%ls\n", entry.msName.c_str());
			}
			if(gbVerboseOutput)
				EA::UnitTest::Report("\n");


			// Test ReadRecursive of a directory with partial paths.
			entryList.clear();
			result = dirIterator.ReadRecursive(pTempDir, entryList, EA_WCHAR("*"), kDirectoryEntryFile | kDirectoryEntryDirectory, true, false, DirectoryIterator::kMaxEntryCountDefault, false);
			EATEST_VERIFY(result == 5);
			EATEST_VERIFY(entryList.size() == 5);
			for(DirectoryIterator::EntryList::iterator it = entryList.begin(); it != entryList.end(); ++it)
			{
				const DirectoryIterator::Entry& entry = *it;
				if(gbVerboseOutput)
					EA::UnitTest::Report("%ls\n", entry.msName.c_str());
			}
			if(gbVerboseOutput)
				EA::UnitTest::Report("\n");


			// Test a filter that should produce no results.
			entryList.clear();
			result = dirIterator.ReadRecursive(pTempDir, entryList, EA_WCHAR("*.NonExistingFile"), kDirectoryEntryFile | kDirectoryEntryDirectory, true, false, DirectoryIterator::kMaxEntryCountDefault, false);
			EATEST_VERIFY(result == 0);
			EATEST_VERIFY(entryList.size() == 0);
		}
	}

	return nErrorCount;
}


///////////////////////////////////////////////////////////////////////////////
// TestFileStream0
//
static int TestFileStream0()
{
	int nErrorCount(0);

	EA::UnitTest::Report("    TestFileStream0\n");

	using namespace EA::IO;

	{   // FileStream path tests
		eastl::string8 path8(gRootDirectory8);
		eastl::wstring pathW(gRootDirectoryW);
		wchar_t kDirSeparator[2] = { EA_FILE_PATH_SEPARATOR_8, 0 };

		path8  +=  "temp";
		path8  +=  EA_FILE_PATH_SEPARATOR_STRING_8;
		path8  +=  "hello world.txt";

		pathW += EA_WCHAR("temp");
		pathW += kDirSeparator;
		pathW += EA_WCHAR("hello world.txt");

		EA::IO::FileStream fileStream8(path8.c_str());
		EA::IO::FileStream fileStream16(pathW.c_str());

		char pPath8[EA::IO::kMaxPathLength];
		wchar_t pPathW[EA::IO::kMaxPathLength];

		fileStream8.GetPath(pPath8, EA::IO::kMaxPathLength);
		EATEST_VERIFY(path8 == pPath8);

		fileStream8.GetPath(pPathW, EA::IO::kMaxPathLength);
		EATEST_VERIFY(pathW == pPathW);

		fileStream16.GetPath(pPath8, EA::IO::kMaxPathLength);
		EATEST_VERIFY(path8 == pPath8);

		fileStream16.GetPath(pPathW, EA::IO::kMaxPathLength);
		EATEST_VERIFY(pathW == pPathW);
	}


	{   // FileStream read/write, CreationDisposition, sharing
		// 0
		// kCDOpenExisting
		// kCDCreateNew
		// kCDCreateAlways
		// kCDOpenAlways
		// kCDTruncateExisting
	}



	{   // FileStream read/write tests.
		EA::IO::off_type  nPosition;
		EA::IO::size_type nSize;
		bool              bResult;
		int               nResult;
		eastl::string8    path8(gRootDirectory8); path8 += "hello world.txt";

		// Remove this test file if it already exists.
		if(File::Exists(path8.c_str()))
			File::Remove(path8.c_str());

		bResult = File::Exists(path8.c_str());
		EATEST_VERIFY_MSG(!bResult, "Unable to delete test file.");

		EA::IO::FileStream fileStream(path8.c_str());

		nResult = fileStream.GetAccessFlags();
		EATEST_VERIFY(nResult == 0);

		nResult = fileStream.GetState();
		EATEST_VERIFY(nResult == kStateNotOpen);

		nSize = fileStream.GetSize();
		EATEST_VERIFY(nSize == EA::IO::kSizeTypeError);

		bResult = fileStream.Open(kAccessFlagReadWrite, kCDCreateAlways, 0, 0);
		EATEST_VERIFY_MSG(bResult, "Unable to open test file.");

		if(bResult)
		{
			nResult = fileStream.GetAccessFlags();
			EATEST_VERIFY(nResult == kAccessFlagReadWrite);

			nResult = fileStream.GetState();
			EATEST_VERIFY(nResult == kStateSuccess);

			nSize = fileStream.GetSize();
			EATEST_VERIFY(nSize == 0);

			nPosition = fileStream.GetPosition();
			EATEST_VERIFY(nPosition == 0);

			bResult = fileStream.SetSize(5555);
			EATEST_VERIFY(bResult);

			nSize = fileStream.GetSize();
			EATEST_VERIFY(nSize == 5555);

			nSize = fileStream.GetAvailable();
			EATEST_VERIFY(nSize == 5555);

			nPosition = fileStream.GetPosition();
			EATEST_VERIFY(nPosition == 0);

			nPosition = fileStream.GetPosition(EA::IO::kPositionTypeEnd);
			EATEST_VERIFY(nPosition == (EA::IO::off_type)-5555);

			bResult = fileStream.SetPosition(200);
			EATEST_VERIFY(bResult);

			nPosition = fileStream.GetPosition(EA::IO::kPositionTypeBegin);
			EATEST_VERIFY(nPosition == 200);

			nPosition = fileStream.GetPosition(EA::IO::kPositionTypeCurrent);
			EATEST_VERIFY(nPosition == 0);

			nPosition = fileStream.GetPosition(EA::IO::kPositionTypeEnd);
			EATEST_VERIFY(nPosition == (EA::IO::off_type)(-5555 + 200));

			nSize = fileStream.GetAvailable();
			EATEST_VERIFY(nSize == (5555 - 200));

			bResult = fileStream.SetPosition(100, EA::IO::kPositionTypeCurrent);
			EATEST_VERIFY(bResult);

			nPosition = fileStream.GetPosition(EA::IO::kPositionTypeBegin);
			EATEST_VERIFY(nPosition == 300);

			bResult = fileStream.Write(path8.data(), 4);
			EATEST_VERIFY(bResult);

			nPosition = fileStream.GetPosition(EA::IO::kPositionTypeBegin);
			EATEST_VERIFY(nPosition == 304);

			bResult = fileStream.Flush();
			EATEST_VERIFY(bResult);

			#if EAIO_64_BIT_SIZE_ENABLED
				#if defined(EA_PLATFORM_NX) 
					// We use the tmp partition for writing our test data and that is capped at 4GB
					// so while the spirit of this test is not met, it is still useful to run as it has caught errors
					const EA::IO::size_type kLargeFileSize = UINT64_C(0xA0000000); // This is > 3GB
				#else
					const EA::IO::size_type kLargeFileSize = UINT64_C(0x100001010); // This is > 4GB
				#endif

				bResult = fileStream.SetSize(kLargeFileSize);

				#if defined(EA_PLATFORM_MICROSOFT) && !EA_WINAPI_FAMILY_PARTITION(EA_WINAPI_PARTITION_DESKTOP)
					if(!bResult && (fileStream.GetState() == kFSErrorDiskFull))
						EA::UnitTest::Report("Cannot create > 4GB file on the kit. Skipping test.\n");
					else
						EATEST_VERIFY(bResult);
				#else
					EATEST_VERIFY_F(bResult, "FileStream::SetSize error: %d\n", fileStream.GetState());
				#endif

				if(bResult)
				{
					nSize = fileStream.GetSize();
					//EA::UnitTest::Report("%llu %lld\n", (int64_t)nSize, kLargeFileSize);
					EATEST_VERIFY(nSize == kLargeFileSize);

					nPosition = fileStream.GetPosition(EA::IO::kPositionTypeBegin);
					EATEST_VERIFY(nPosition == 304);

					bResult = fileStream.SetPosition(0, EA::IO::kPositionTypeEnd);
					EATEST_VERIFY(bResult);

					nPosition = fileStream.GetPosition(EA::IO::kPositionTypeBegin);
					//EA::UnitTest::Report("%lld %llu %lld\n", (int64_t)nPosition, kLargeFileSize, (EA::IO::off_type)kLargeFileSize);
					EATEST_VERIFY(nPosition == (EA::IO::off_type)kLargeFileSize);

					bResult = fileStream.SetPosition(-4, EA::IO::kPositionTypeEnd);
					EATEST_VERIFY(bResult);

					// On the PS4 platform the sceKernelWrite() function behind the Write call can timeout.
					// We could ask Sony for a solution but it is questionable if we really want to fix
					// this as it might not be practical to write 4GB of data in one write operation.
					// We currently enable this test in interactive mode only because it takes a long time 
					// to complete on some platforms.
					if((EA::UnitTest::GetTestLevel() >= EA::UnitTest::kTestLevelHigh) || EA::UnitTest::GetInteractive())
					{
						bResult = fileStream.Write(path8.data(), 4); // This write takes a long time to execute on some platforms, as it is filling in a lot of new file space.
						EATEST_VERIFY(bResult);

						bResult = fileStream.SetPosition(-4, EA::IO::kPositionTypeCurrent);
						EATEST_VERIFY(bResult);

						char buffer[5] = { 0, 0, 0, 0, 0 };
						nSize = fileStream.Read(buffer, 4);
						EATEST_VERIFY(nSize == 4);
						EATEST_VERIFY(memcmp(buffer, path8.c_str(), 4) == 0);
					}
				}
                #if defined(EA_PLATFORM_APPLE)
                // On OSX file will be zero filled for security reasons when pointer is moved past the end of the file
                fileStream.SetSize(0);
                #endif
			#endif

			fileStream.Close();

			File::Remove(path8.c_str());
		}
	}

	return nErrorCount;
}


///////////////////////////////////////////////////////////////////////////////
// TestFileStream1
//
// Tests reading of a Uint8.txt file. This is a file which is simply 256
// bytes long and each byte has the value 0, 1, 2, etc. We do random reads
// from that file into a buffer and verify that read proceeded as expected.
//
static int TestFileStream1(EA::IO::FileStream* pFileStream = NULL)
{
	using namespace EA;
	using namespace EA::IO;

	EA::UnitTest::Report("    TestFileStream1\n");

	int         nErrorCount = 0;
	const float fDiskSpeed  = EA::UnitTest::GetSystemSpeed(EA::UnitTest::kSpeedTypeDisk);

	if(fDiskSpeed > 0.1f)
	{
		bool              bResult = true;
		wchar_t           pFilePath[kMaxPathLength];
		EA::IO::size_type n;
		const uint32_t    kTest1WriteCount = 256;

		bResult = MakeTempPathName(pFilePath, gRootDirectoryW, NULL, EA_WCHAR(".temp"));
		EATEST_VERIFY(bResult);

		if(bResult)
		{
			FileStream file;

			if(!pFileStream)
				pFileStream = &file;

			pFileStream->SetPath(pFilePath);

			if(File::Exists(pFilePath))
				File::Remove(pFilePath);

			// Write the test file.
			bResult = pFileStream->Open(kAccessFlagWrite, kCDOpenAlways);
			EATEST_VERIFY(bResult);

			if(bResult)
			{
				for(uint32_t i = 0; i < kTest1WriteCount; i++)
				{
					uint8_t i8 = (uint8_t)i;
					bResult = pFileStream->Write(&i8, sizeof(i8));
					EATEST_VERIFY(bResult);
				}

				pFileStream->Close();
			}

			if(bResult)
			{
				// Read the test file back.
				if(pFileStream->Open(kAccessFlagRead))
				{
					const uint8_t  kUnusedValue = 0x01;
					UnitTest::Rand rng(UnitTest::GetRandSeed());
					uint32_t       j;

					uint8_t* const pBuffer = new uint8_t[kTest1WriteCount * 2];

					// Set the first kTest1WriteCount bytes to the same value we wrote into the file.
					for(j = 0; j < kTest1WriteCount; j++)
						pBuffer[j] = (uint8_t)j;

					// Set the next kTest1WriteCount bytes to some other value.
					for(; j < kTest1WriteCount * 2; j++)
						pBuffer[j] = kUnusedValue;

					#if defined(EA_PLATFORM_DESKTOP)
						int kTest1LoopCount = 1000;
					#else
						int kTest1LoopCount = 200;
					#endif

					#if EATEST_VERSION_N >= 20500
						if(EA::UnitTest::GetTestLevel() <= EA::UnitTest::kTestLevelDefault)
							kTest1LoopCount = 10;
					#endif

					for(int i = 0; i < kTest1LoopCount; i++)
					{
						const uint32_t nReadCount    = rng.RandLimit(35);
						const uint32_t nFilePosition = rng.RandLimit(kTest1WriteCount - nReadCount);

						bResult = pFileStream->SetPosition((off_type)(int32_t)nFilePosition);
						EATEST_VERIFY(bResult);

						n = pFileStream->Read(pBuffer + nFilePosition, nReadCount);
						EATEST_VERIFY(n == (size_type)nReadCount);

						n = pFileStream->Read(pBuffer + nFilePosition + nReadCount, nReadCount);
						EATEST_VERIFY(n != kSizeTypeError); // This (intentionally) might go past the end of the file.

						for(j = 0; j < kTest1WriteCount; j++)
							EATEST_VERIFY(pBuffer[j] == (uint8_t)j);

						for(; j < kTest1WriteCount * 2; j++)
							EATEST_VERIFY(pBuffer[j] == kUnusedValue);
					}

					pFileStream->Close();

					delete[] pBuffer;
				}
			}

			if(File::Exists(pFilePath))
				File::Remove(pFilePath);
		}
	}

	return nErrorCount;
}


///////////////////////////////////////////////////////////////////////////////
// TestFileStream2
//
// Tests reading of a uint16_t temp file. This is a file which is simply 65536
// bytes long and each byte has the value 0, 1, 2, etc. We do random reads
// from that file into a buffer and verify that read proceeded as expected.
//
static int TestFileStream2(EA::IO::FileStream* pFileStream = NULL)
{
	using namespace EA;
	using namespace EA::IO;

	EA::UnitTest::Report("    TestFileStream2\n");

	int         nErrorCount = 0;
	const float fDiskSpeed  = EA::UnitTest::GetSystemSpeed(EA::UnitTest::kSpeedTypeDisk);

	if(fDiskSpeed > 0.1f)
	{
		bool              bResult = true;
		wchar_t           pFilePath[kMaxPathLength];
		EA::IO::size_type n;
		const uint32_t    kTest2WriteCount = 2048;

		bResult = MakeTempPathName(pFilePath, gRootDirectoryW, NULL, EA_WCHAR(".temp"));
		EATEST_VERIFY(bResult);

		if(bResult)
		{
			FileStream file;

			if(!pFileStream)
				pFileStream = &file;

			pFileStream->SetPath(pFilePath);

			if(File::Exists(pFilePath))
				File::Remove(pFilePath);

			// Write the test file.
			bResult = pFileStream->Open(kAccessFlagWrite, kCDOpenAlways);
			EATEST_VERIFY(bResult);

			if(bResult)
			{
				for(uint32_t i = 0; i < kTest2WriteCount; i++)
				{
					uint16_t i16 = (uint16_t)i;
					bResult = pFileStream->Write(&i16, sizeof(i16));
					EATEST_VERIFY(bResult);
				}

				pFileStream->Close();
			}

			if(bResult)
			{
				// Read the test file back.
				if(pFileStream->Open(kAccessFlagRead))
				{
					const uint16_t kUnusedValue = 0x0001;
					UnitTest::Rand rng(UnitTest::GetRandSeed());
					uint32_t       j;

					uint16_t* const pBuffer = new uint16_t[kTest2WriteCount * 2];

					// Set the first kTest2WriteCount bytes to the same value we wrote into the file.
					for(j = 0; j < kTest2WriteCount; j++)
						pBuffer[j] = (uint16_t)j;

					// Set the next kTest2WriteCount bytes to some other value.
					for(; j < kTest2WriteCount * 2; j++)
						pBuffer[j] = kUnusedValue;

					#if defined(EA_PLATFORM_DESKTOP)
						int kTest2LoopCount = 1000;
					#else
						int kTest2LoopCount = 200;
					#endif

					#if EATEST_VERSION_N >= 20500
						if(EA::UnitTest::GetTestLevel() <= EA::UnitTest::kTestLevelDefault)
							kTest2LoopCount = 10;
					#endif

					for(int i = 0; i < kTest2LoopCount; i++)
					{
						const uint32_t nReadCount    = rng.RandLimit(1500);
						const uint32_t nFilePosition = rng.RandLimit(kTest2WriteCount - nReadCount);

						bResult = pFileStream->SetPosition((off_type)(nFilePosition * sizeof(uint16_t)));
						EATEST_VERIFY(bResult);

						n = pFileStream->Read(pBuffer + nFilePosition, nReadCount * sizeof(uint16_t));
						EATEST_VERIFY(n == (size_type)(nReadCount * sizeof(uint16_t)));

						n = pFileStream->Read(pBuffer + nFilePosition + nReadCount, nReadCount * sizeof(uint16_t));
						EATEST_VERIFY(n != kSizeTypeError); // This (intentionally) might go past the end of the file.

						for(j = 0; j < kTest2WriteCount; j++)
							EATEST_VERIFY(pBuffer[j] == (uint16_t)j);

						for(; j < kTest2WriteCount * 2; j++)
							EATEST_VERIFY(pBuffer[j] == kUnusedValue);
					}

					pFileStream->Close();

					delete[] pBuffer;
				}
			}

			if(File::Exists(pFilePath))
				File::Remove(pFilePath);
		}
	}

	return nErrorCount;
}


///////////////////////////////////////////////////////////////////////////////
// TestFileStream3
//
// Tests buffered reading and writing to a uint16_t temp file. This is
// the most stringent test of out test suite here. We do random seeks within
// a uint16_t-setup file and randomly read or write chunks of data that should
// match the expected data in the file. After doing this many many time, we
// re-read the entire file and verify that it contains the expected data. Also,
// we manually inspect the file to make sure it looks proper.
// 
static int TestFileStream3(EA::IO::FileStream* pFileStream = NULL)
{
	using namespace EA;
	using namespace EA::IO;

	EA::UnitTest::Report("    TestFileStream3\n");

	int         nErrorCount = 0;
	const float fDiskSpeed  = EA::UnitTest::GetSystemSpeed(EA::UnitTest::kSpeedTypeDisk);

	if(fDiskSpeed > 0.1f)
	{
		bool              bResult = true;
		wchar_t           pFilePath[kMaxPathLength];
		EA::IO::size_type n;
		uint32_t          kTest3WriteSize = 4096;

		bResult = MakeTempPathName(pFilePath, gRootDirectoryW, NULL, EA_WCHAR(".temp"));
		EATEST_VERIFY(bResult);

		if(bResult)
		{
			FileStream file;

			if(!pFileStream)
				pFileStream = &file;

			pFileStream->SetPath(pFilePath);

			if(File::Exists(pFilePath))
				File::Remove(pFilePath);

			bResult = pFileStream->Open(kAccessFlagReadWrite, kCDOpenAlways);
			EATEST_VERIFY(bResult);

			if(bResult)
			{
				const uint16_t kUnusedValue = 0x0001;
				UnitTest::Rand rng(UnitTest::GetRandSeed());
				uint32_t       i;

				uint16_t* const pBuffer = new uint16_t[kTest3WriteSize * 2];

				// Setup
				for(i = 0; i < kTest3WriteSize; i++)
					pBuffer[i] = (uint16_t)i;

				for(; i < kTest3WriteSize * 2; i++)
					pBuffer[i] = kUnusedValue;


				// Test a simple sequential write.
				for(i = 0; i < kTest3WriteSize; i++)
				{
					uint16_t i16 = (uint16_t)i;
					pFileStream->Write(&i16, sizeof(i16));
				}

				pFileStream->SetPosition(0);

				for(i = 0; i < kTest3WriteSize; i++)
				{
					uint16_t i16;
					pFileStream->Read(&i16, sizeof(i16));

					EATEST_VERIFY(i16 == (uint16_t)i);
				}

				for(i = kTest3WriteSize; i < kTest3WriteSize * 2; i++)
				{
					EATEST_VERIFY(pBuffer[i] == kUnusedValue);
				}


				// Arbitrary random read/write tests.
				uint32_t iterationCount = (uint32_t)(10000 * EA::UnitTest::GetSystemSpeed(EA::UnitTest::kSpeedTypeDisk));

				#if EATEST_VERSION_N >= 20500
					if(EA::UnitTest::GetTestLevel() <= EA::UnitTest::kTestLevelDefault)
						iterationCount /= 10;
				#endif

				for(i = 0; i < iterationCount; i++)
				{
					uint32_t nFilePosition(rng.RandLimit((kTest3WriteSize / sizeof(uint16_t)) - 32) * sizeof(uint16_t));
					uint16_t buffer[48];
					int      nBlockSize(rng.RandRange(2, 48));

					bResult = pFileStream->SetPosition((EA::IO::off_type)nFilePosition); // nFilePosition is an even value from 0-131070.
					EATEST_VERIFY(bResult);

					if(rng.RandLimit(2) == 0) // RandLimit(2) is the same thing as a RandBool.
					{
						// We write values into the file with the same value they were originally, 
						// so it's easy to tell if things went correctly.
						for(uint16_t j = 0; j < nBlockSize; j++)
							buffer[j] = (uint16_t)((nFilePosition / sizeof(uint16_t)) + j);

						bResult = pFileStream->Write(buffer, nBlockSize * sizeof(uint16_t));
						EATEST_VERIFY(bResult);
					}
					else
					{
						// Test reading a segment of the file, verifying that it is what we expect.
						n = pFileStream->Read(buffer, nBlockSize * sizeof(uint16_t));
						EATEST_VERIFY(n == (nBlockSize * sizeof(uint16_t)));

						for(uint16_t x = 0; x < nBlockSize; x++)
							EATEST_VERIFY(buffer[x] == (uint16_t)((nFilePosition / sizeof(uint16_t)) + x));
					}
				}

				// Do final pass at verifying the file is as we expect.
				bResult = pFileStream->SetPosition(0);
				EATEST_VERIFY(bResult);

				for(uint32_t y = 0; y < kTest3WriteSize; y++)
				{
					uint16_t y16;
					pFileStream->Read(&y16, sizeof(y16));

					EATEST_VERIFY(y16 == (uint16_t)y);
				}

				pFileStream->Close();

				delete[] pBuffer;
			}
		}
	}

	return nErrorCount;
}


///////////////////////////////////////////////////////////////////////////////
// TestFile
//
int TestFile()
{
	int nErrorCount(0);

	nErrorCount += TestFileUtil();  
	nErrorCount += TestFileDirectory();
	nErrorCount += TestFileStream0();
	nErrorCount += TestFileStream1();
	nErrorCount += TestFileStream2();
	nErrorCount += TestFileStream3();


	return nErrorCount;
}

