///////////////////////////////////////////////////////////////////////////////
// TestFileNotification.cpp
//
// Copyright (c) 2007 Criterion Software Limited and its licensors. All Rights Reserved.
// Created by Paul Pedriana
///////////////////////////////////////////////////////////////////////////////


#include <EABase/eabase.h>
#include <EAIO/internal/Config.h>
#include <EAIO/EAFileStream.h>
#include <EAIO/EAFileDirectory.h>
#include <EAIO/EAFileUtil.h>
#include <EAIO/EAFileNotification.h>
#include <EAIO/PathString.h>
#include <EAIOTest/EAIO_Test.h>
#include <EATest/EATest.h>
#include <EAStdC/EAString.h>
#include <EAStdC/EASprintf.h>
#include <eathread/eathread_thread.h>
#include <coreallocator/icoreallocator_interface.h>
#include EA_ASSERT_HEADER


// Can be used by only one thread at a time.
static const char* GetChangeTypeFlagsDescription(int nChangeTypeFlags)
{
	static char descriptionString[256];

	EA::StdC::Sprintf(descriptionString, "changeTypeFlags: %d:", nChangeTypeFlags);

	if(nChangeTypeFlags == 0)
		EA::StdC::Strlcat(descriptionString, " kChangeTypeFlagNone", EAArrayCount(descriptionString));
	if(nChangeTypeFlags & EA::IO::FileChangeNotification::kChangeTypeFlagFileName)
		EA::StdC::Strlcat(descriptionString, " kChangeTypeFlagFileName", EAArrayCount(descriptionString));
	if(nChangeTypeFlags & EA::IO::FileChangeNotification::kChangeTypeFlagDirectoryName)
		EA::StdC::Strlcat(descriptionString, " kChangeTypeFlagDirectoryName", EAArrayCount(descriptionString));
	if(nChangeTypeFlags & EA::IO::FileChangeNotification::kChangeTypeFlagModified)
		EA::StdC::Strlcat(descriptionString, " kChangeTypeFlagModified", EAArrayCount(descriptionString));
	if(nChangeTypeFlags & EA::IO::FileChangeNotification::kChangeTypeFlagAttributes)
		EA::StdC::Strlcat(descriptionString, " kChangeTypeFlagAttributes", EAArrayCount(descriptionString));
	if(nChangeTypeFlags & EA::IO::FileChangeNotification::kChangeTypeFlagSize)
		EA::StdC::Strlcat(descriptionString, " kChangeTypeFlagSize", EAArrayCount(descriptionString));

	return descriptionString;
}


///////////////////////////////////////////////////////////////////////////////
// TestFileNotificationAllFiles
///////////////////////////////////////////////////////////////////////////////

#if defined(EA_PLATFORM_DESKTOP) && FCN_POLL_ENABLED
	static void FileChangeDetectorAllFiles(EA::IO::FileChangeNotification* /*pFileChangeNotification*/, const wchar_t* /*pDirectoryPath*/,
										   const wchar_t* /*pFileName*/, int /*nChangeTypeFlags*/, void* /*pContext*/)
	{
		// Currently empty
	}
#endif


static int TestFileNotificationAllFiles()
{
	using namespace EA::IO;
	
	int nErrorCount(0);
	
	#if defined(EA_PLATFORM_DESKTOP) && FCN_POLL_ENABLED
		// This is a regression for a Mac OS X crash, which turned out to be due to
		// stack space exhaustion, mostly the fault of FindFileFirst, FindFileNext.
		// Create N threads which implement manual file detection testing 
		// of the entire file system. Let them execute for N seconds for the 
		// purpose of verifying simply that they don't crash or hang.
		const char* kWatchPath = "/Users/PPedriana/Projects/EAOS/";
		
		if(EA::IO::Directory::Exists(kWatchPath))
		{
			const int kFCNCount = 1; // Currently we test just one, as that was enough to test the crash we were looking for when we first wrote this.
			FileChangeNotification fcArray[kFCNCount];
			
			for(int i = 0; i < kFCNCount; i++)
			{
				fcArray[i].Init();
				fcArray[i].SetWatchedDirectory(kWatchPath);
				fcArray[i].SetChangeTypeFlags(FileChangeNotification::kChangeTypeFlagModified | 
											  FileChangeNotification::kChangeTypeFlagFileName | 
											  FileChangeNotification::kChangeTypeFlagDirectoryName);
				fcArray[i].SetOptionFlags(FileChangeNotification::kOptionFlagWatchSubdirectories | 
										  FileChangeNotification::kOptionFlagMultithreaded);
				fcArray[i].SetNotificationCallback(FileChangeDetectorAllFiles, NULL);
				fcArray[i].Start();
			}

			EA::UnitTest::ThreadSleepRandom(100000, 200000, false);

			for(int i = 0; i < kFCNCount; i++)
			{
				fcArray[i].Shutdown();
			}
		}
	#endif

	return nErrorCount;
}



///////////////////////////////////////////////////////////////////////////////
// TestFileNotificationBasic
///////////////////////////////////////////////////////////////////////////////

enum Operation
{
	kOperationNone,
	kOperationFileCreate,
	kOperationFileChange,
	kOperationFileDelete,
	kOperationDirCreate,
	kOperationDirDelete
};


struct WorkData
{
	Operation                       mOperation;
	EA::IO::Path::PathStringW       mDirectoryPath;
	EA::IO::Path::PathStringW       mFileName;
	EA::IO::Path::PathStringW       mFilePath;
	EA::Thread::AtomicInt32         mbDetected;    // True if the execution of this operation has been detected by FileChangeNotification.
	EA::Thread::AtomicInt32         mnErrorCount;
	EA::IO::FileChangeNotification  mFCN;

	WorkData() : mOperation(kOperationNone), 
				 mDirectoryPath(),
				 mFileName(),
				 mFilePath(),
				 mbDetected(0),
				 mnErrorCount(0),
				 mFCN() { }
private:
	WorkData(const WorkData& rhs);
	WorkData& operator=(const WorkData& rhs);
};


static void FileChangeDetectorBasic(EA::IO::FileChangeNotification* /*pFileChangeNotification*/, const wchar_t* pDirectoryPath,
								   const wchar_t* pFileName, int nChangeTypeFlags, void* pContext)
{
	using namespace EA::IO;

	WorkData* const pWorkData = (WorkData*)pContext;

	int  nErrorCount = 0;
	bool bResult;

	pWorkData->mbDetected = 1;

	switch(pWorkData->mOperation)
	{
		case kOperationFileCreate:
			EATEST_VERIFY_F((nChangeTypeFlags & FileChangeNotification::kChangeTypeFlagFileName) != 0, "Expected kOperationFileCreate for %ls%ls. Actual %hs.", pDirectoryPath, pFileName, GetChangeTypeFlagsDescription(nChangeTypeFlags));
			break;

		case kOperationFileChange:
			#if !defined(EA_PLATFORM_NX) // The NX doesn't support reading a file's timestamp
				EATEST_VERIFY_F((nChangeTypeFlags & FileChangeNotification::kChangeTypeFlagModified) != 0, "Expected kChangeTypeFlagModified for %ls%ls. Actual %hs.", pDirectoryPath, pFileName, GetChangeTypeFlagsDescription(nChangeTypeFlags));
			#endif
			break;

		case kOperationFileDelete:
			EATEST_VERIFY_F((nChangeTypeFlags & FileChangeNotification::kChangeTypeFlagFileName) != 0, "Expected kChangeTypeFlagFileName for %ls%ls. Actual %hs.", pDirectoryPath, pFileName, GetChangeTypeFlagsDescription(nChangeTypeFlags));
			break;

		case kOperationDirCreate:
			EATEST_VERIFY_F((nChangeTypeFlags & FileChangeNotification::kChangeTypeFlagDirectoryName) != 0, "Expected kChangeTypeFlagDirectoryName for %ls%ls. Actual %hs.", pDirectoryPath, pFileName, GetChangeTypeFlagsDescription(nChangeTypeFlags));
			break;

		case kOperationDirDelete:
			EATEST_VERIFY_F((nChangeTypeFlags & FileChangeNotification::kChangeTypeFlagDirectoryName) != 0, "Expected kChangeTypeFlagDirectoryName for %ls%ls. Actual %hs.", pDirectoryPath, pFileName, GetChangeTypeFlagsDescription(nChangeTypeFlags));
			break;

		case kOperationNone:
		default:
			EATEST_VERIFY_MSG(false, "Unexpected kOperationNone.");
			break;
	}


	switch(pWorkData->mOperation)
	{
		case kOperationFileCreate:
		case kOperationFileChange:
		case kOperationFileDelete:
		case kOperationDirCreate:
		case kOperationDirDelete:
			bResult = (EA::StdC::Stricmp(pWorkData->mFileName.c_str(), pFileName) == 0);
			EATEST_VERIFY_F(bResult, "File name doesn't match expected in directory %ls. Name: %ls; expected: %ls", pDirectoryPath, pFileName, pWorkData->mFileName.c_str());

			bResult = (EA::StdC::Stricmp(pWorkData->mDirectoryPath.c_str(), pDirectoryPath) == 0);
			EATEST_VERIFY_F(bResult, "Directory path doesn't match expected. path: %ls; expected: %ls", pDirectoryPath, pWorkData->mDirectoryPath.c_str());

		default:
			break;
	}

	pWorkData->mnErrorCount += nErrorCount;
}


static int TestFileNotificationBasic()
{
	using namespace EA::IO;

	int nErrorCount(0);

	const float    fDiskSpeed     = EA::UnitTest::GetSystemSpeed(EA::UnitTest::kSpeedTypeDisk);
	const float    fDiskSpeedUsed = (fDiskSpeed >= 0.2f) ? fDiskSpeed : 0.2f;
	const uint32_t kMinWaitTimeMs = (uint32_t)( 500 / fDiskSpeedUsed);
	const uint32_t kMaxWaitTimeMs = (uint32_t)(1000 / fDiskSpeedUsed);

	Path::PathStringW sTestDir(gRootDirectoryW);
	Path::Append(sTestDir, Path::PathStringW(EA_WCHAR("FileNotificationTest")));
	Path::EnsureTrailingSeparator(sTestDir);

	bool bResult = Directory::EnsureExists(sTestDir.c_str());
	EATEST_VERIFY_F(bResult, "Directory::EnsureExists failure for %ls.", sTestDir.c_str());

	if(bResult)
	{
		EA::UnitTest::ThreadSleepRandom(kMinWaitTimeMs, kMaxWaitTimeMs);

		WorkData workData;

		bResult = workData.mFCN.Init();
		EATEST_VERIFY_MSG(bResult, "FileChangeNotification::Init failed.");

		bResult = workData.mFCN.SetWatchedDirectory(sTestDir.c_str());
		workData.mFCN.SetChangeTypeFlags(0xffff);
		EATEST_VERIFY_F(bResult, "FileChangeNotification::SetWatchedDirectory failed for directory %ls.", sTestDir.c_str());

		workData.mFCN.SetOptionFlags(FileChangeNotification::kOptionFlagWatchSubdirectories); // | kOptionFlagMultithreaded
		workData.mFCN.SetNotificationCallback(FileChangeDetectorBasic, &workData);
		bResult = workData.mFCN.Start();
		EATEST_VERIFY_F(bResult, "FileChangeNotification::Start failed for directory %ls.", sTestDir.c_str());


		// kOperationFileDelete
		EA::UnitTest::ThreadSleepRandom(kMinWaitTimeMs, kMaxWaitTimeMs);
		workData.mOperation     = kOperationFileDelete;
		workData.mDirectoryPath = sTestDir;
		workData.mFileName      = EA_WCHAR("FileA.txt");
		workData.mFilePath      = workData.mDirectoryPath; workData.mFilePath += workData.mFileName;
		workData.mbDetected     = 0;

		if(File::Exists(workData.mFilePath.c_str()))
		{
			bResult = File::Remove(workData.mFilePath.c_str());
			EATEST_VERIFY_F(bResult, "Unable to delete file: %ls", workData.mFilePath.c_str());

			if(bResult)
			{
				EA::UnitTest::ThreadSleepRandom(kMinWaitTimeMs, kMaxWaitTimeMs);
				workData.mFCN.Poll((int)kMaxWaitTimeMs);
				EATEST_VERIFY_F(workData.mbDetected.GetValue() == 1, "File deletion not detected for file: %ls", workData.mFilePath.c_str());
			}
		}


		// kOperationFileCreate
		EA::UnitTest::ThreadSleepRandom(kMinWaitTimeMs, kMaxWaitTimeMs);
		workData.mOperation     = kOperationFileCreate;
		workData.mDirectoryPath = sTestDir;
		workData.mFileName      = EA_WCHAR("FileA.txt");
		workData.mFilePath      = workData.mDirectoryPath; workData.mFilePath += workData.mFileName;
		workData.mbDetected     = 0;

		bResult = File::Create(workData.mFilePath.c_str());
		EATEST_VERIFY_F(bResult, "Unable to create file: %ls", workData.mFilePath.c_str());

		if(bResult)
		{
			EA::UnitTest::ThreadSleepRandom(kMinWaitTimeMs, kMaxWaitTimeMs);
			workData.mFCN.Poll((int)kMaxWaitTimeMs);
			EATEST_VERIFY_F(workData.mbDetected.GetValue() == 1, "File creation not detected for file: %ls", workData.mFilePath.c_str()); // Verify that a change was detected (in this case that a file was created).
		}


		// kOperationDirDelete
		EA::UnitTest::ThreadSleepRandom(kMinWaitTimeMs, kMaxWaitTimeMs);
		workData.mOperation     = kOperationDirDelete;
		workData.mDirectoryPath = sTestDir;
		workData.mFileName      = EA_WCHAR("DirA");
		workData.mFilePath      = workData.mDirectoryPath; workData.mFilePath += workData.mFileName; workData.mFilePath += EA_FILE_PATH_SEPARATOR_16;
		workData.mbDetected     = 0;

		if(Directory::Exists(workData.mFilePath.c_str()))
		{
			bResult = Directory::Remove(workData.mFilePath.c_str());
			EATEST_VERIFY_F(bResult, "Unable to delete directory: %ls", workData.mFilePath.c_str());

			if(bResult)
			{
				EA::UnitTest::ThreadSleepRandom(kMinWaitTimeMs, kMaxWaitTimeMs);
				workData.mFCN.Poll((int)kMaxWaitTimeMs);

				// Currently, the polled version of EAFileNotification doesn't detect directory creation/removal.
				#if !FCN_POLL_ENABLED
					EATEST_VERIFY_F(workData.mbDetected.GetValue() == 1, "Directory deletion not detected for directory: %ls", workData.mFilePath.c_str());
				#endif
			}
		}


		// kOperationDirCreate
		EA::UnitTest::ThreadSleepRandom(kMinWaitTimeMs, kMaxWaitTimeMs);
		workData.mOperation     = kOperationDirCreate;
		workData.mDirectoryPath = sTestDir;
		workData.mFileName      = EA_WCHAR("DirA");
		workData.mFilePath      = workData.mDirectoryPath; workData.mFilePath += workData.mFileName; workData.mFilePath += EA_FILE_PATH_SEPARATOR_16;
		workData.mbDetected     = 0;

		bResult = Directory::Create(workData.mFilePath.c_str());
		EATEST_VERIFY_F(bResult, "Unable to create directory: %ls", workData.mFilePath.c_str());

		if(bResult)
		{
			EA::UnitTest::ThreadSleepRandom(kMinWaitTimeMs, kMaxWaitTimeMs);
			workData.mFCN.Poll((int)kMaxWaitTimeMs);

			// Currently, the console (polled) versio nof EAFileNotification doesn't detect directory creation/removal.
			#if !FCN_POLL_ENABLED
				EATEST_VERIFY_F(workData.mbDetected.GetValue() == 1, "Directory creation not detected for directory: %ls", workData.mFilePath.c_str());
			#endif
		}


		// kOperationFileChange
		EA::UnitTest::ThreadSleepRandom(kMinWaitTimeMs, kMaxWaitTimeMs);
		workData.mOperation     = kOperationFileChange;
		workData.mDirectoryPath = sTestDir;
		workData.mFileName      = EA_WCHAR("FileA.txt");
		workData.mFilePath      = workData.mDirectoryPath; workData.mFilePath += workData.mFileName;
		workData.mbDetected     = 0;

		FileStream fileStream(workData.mFilePath.c_str());
		bResult = fileStream.Open(kAccessFlagReadWrite, kCDOpenAlways);
		EATEST_VERIFY_F(bResult, "Unable to open file for write: %ls", workData.mFilePath.c_str());

		if(bResult)
		{
			char c = 0;
			fileStream.SetPosition(0);
			fileStream.Write(&c, 1);
			fileStream.Close();

			EA::UnitTest::ThreadSleepRandom(kMinWaitTimeMs, kMaxWaitTimeMs);
			workData.mFCN.Poll((int)kMaxWaitTimeMs);
			EATEST_VERIFY_F(workData.mbDetected.GetValue() == 1, "File change not detected for file: %ls", workData.mFilePath.c_str());
		}


		// kOperationFileDelete
		EA::UnitTest::ThreadSleepRandom(kMinWaitTimeMs, kMaxWaitTimeMs);
		workData.mOperation     = kOperationFileDelete;
		workData.mDirectoryPath = sTestDir;
		workData.mFileName      = EA_WCHAR("FileA.txt");
		workData.mFilePath      = workData.mDirectoryPath; workData.mFilePath += workData.mFileName;
		workData.mbDetected     = 0;

		bResult = File::Remove(workData.mFilePath.c_str());
		EATEST_VERIFY_F(bResult, "Unable to delete file: %ls", workData.mFilePath.c_str());

		if(bResult)
		{
			EA::UnitTest::ThreadSleepRandom(kMinWaitTimeMs, kMaxWaitTimeMs);
			workData.mFCN.Poll((int)kMaxWaitTimeMs);
			EATEST_VERIFY_F(workData.mbDetected.GetValue() == 1, "File deletion not detected for file: %ls", workData.mFilePath.c_str());
		}


		// kOperationDirDelete
		EA::UnitTest::ThreadSleepRandom(kMinWaitTimeMs, kMaxWaitTimeMs);
		workData.mOperation     = kOperationDirDelete;
		workData.mDirectoryPath = sTestDir;
		workData.mFileName      = EA_WCHAR("DirA");
		workData.mFilePath      = workData.mDirectoryPath; workData.mFilePath += workData.mFileName; workData.mFilePath += EA_FILE_PATH_SEPARATOR_16;
		workData.mbDetected     = 0;

		bResult = Directory::Remove(workData.mFilePath.c_str());
		EATEST_VERIFY_F(bResult, "Unable to delete directory: %ls", workData.mFilePath.c_str());

		if(bResult)
		{
			EA::UnitTest::ThreadSleepRandom(kMinWaitTimeMs, kMaxWaitTimeMs);
			workData.mFCN.Poll((int)kMaxWaitTimeMs);

			// Currently, the polled version of EAFileNotification doesn't detect directory creation/removal.
			#if !FCN_POLL_ENABLED
				EATEST_VERIFY_F(workData.mbDetected.GetValue() == 1, "Directory deletion not detected for directory: %ls", workData.mFilePath.c_str());
			#endif
		}

		nErrorCount += workData.mnErrorCount.GetValue();
	}

	return nErrorCount;
}


///////////////////////////////////////////////////////////////////////////////
// TestFileNotification
//
int TestFileNotification()
{
	int nErrorCount(0);
	
	nErrorCount += TestFileNotificationAllFiles();
	nErrorCount += TestFileNotificationBasic();
	
	return nErrorCount;
}





