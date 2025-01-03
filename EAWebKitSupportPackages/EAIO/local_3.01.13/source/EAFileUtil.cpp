/////////////////////////////////////////////////////////////////////////////
// EAFileUtil.cpp
// 
// Copyright (c) 2007, Electronic Arts. All Rights Reserved.
// Created by Paul Pedriana, Maxis
//
// Implements various file and directory manipulation functions.
/////////////////////////////////////////////////////////////////////////////


#include <EAIO/internal/Config.h>
#include <EAIO/EAFileBase.h>
#include <EAIO/EAFileDirectory.h>
#include <EAIO/EAFileStream.h>
#include <EAIO/EAFileUtil.h>
#include <EAIO/PathString.h>
#include <EAIO/FnEncode.h>
#include <EAStdC/EAString.h>
#include <EAStdC/EAProcess.h>
#include <EAStdC/EASprintf.h>
#include <EAStdC/EAMemory.h>
#include <string.h>
#include <time.h>
#include <eathread/eathread.h>
#include EA_ASSERT_HEADER


#if defined(EA_PLATFORM_MICROSOFT)
	#if defined(_MSC_VER)
		#pragma warning(push, 0)
	#endif

	#if (defined(_MSC_VER) && (_MSC_VER < 1500)) && !defined(_WIN32_WINNT)
		#define _WIN32_WINNT 0x0500 // We use GetVolumePathName, which is a Windows 2000 API.
	#endif
	#include <Windows.h>
	#include <direct.h>
	#include <sys/stat.h>
	#include <sys/utime.h>

	#if EA_WINAPI_FAMILY_PARTITION(EA_WINAPI_PARTITION_DESKTOP)
		#include <ShlObj.h>
		#include <ObjBase.h>

		#ifdef _MSC_VER
			#pragma comment(lib, "ole32.lib")    // Required for CoInitialize, etc.
			#pragma comment(lib, "shell32.lib") // Required for SHGetFolderPath, etc.
		#endif
	#elif EAIO_WINDOWS8_EMULATION_ENABLED && (_WIN32_WINNT >= 0x0602)
		HANDLE CreateFile2EAIO(LPCWSTR lpFileName, DWORD dwDesiredAccess, DWORD dwShareMode, DWORD dwCreationDisposition, LPCREATEFILE2_EXTENDED_PARAMETERS pCreateExParams);
	#endif

	#if defined(_MSC_VER)
		#pragma warning(pop)
	#endif

#elif defined(EA_PLATFORM_APPLE) // OS X, iPhone, etc.

	#include <stdio.h>
	#include <time.h>
	#include <errno.h>
	#include <fcntl.h>
	#include <unistd.h>
	#include <sys/stat.h>
	#include <sys/types.h>
	#include <sys/statvfs.h>
	#include <utime.h>        // Some versions may require <sys/utime.h>

	#if EAIO_APPLE_USE_UNIX_IO
		namespace EA { namespace IO {
			int appleGetTempDirectory(char*  pDirectory, uint32_t nPathCapacity);
			int appleGetTempDirectory(char16_t* pDirectory, uint32_t nPathCapacity);
			int appleGetTempDirectory(char32_t* pDirectory, uint32_t nPathCapacity);

			int appleGetHomeDirectory(char*  pDirectory, uint32_t nPathCapacity);
			int appleGetHomeDirectory(char16_t* pDirectory, uint32_t nPathCapacity);      
			int appleGetHomeDirectory(char32_t* pDirectory, uint32_t nPathCapacity);      
			
			int appleGetAppName(char*  pAppName, uint32_t nPathCapacity);
			int appleGetAppName(char16_t* pAppName, uint32_t nPathCapacity);
			int appleGetAppName(char32_t* pAppName, uint32_t nPathCapacity);
		}}
	#endif

#elif EAIO_USE_UNIX_IO
	#include <stdio.h>
	#include <errno.h>
	#include <fcntl.h>
	#include <unistd.h>
	#if defined(EA_HAVE_SYS_STAT_H)
		#include <sys/stat.h>
	#endif
	#include <sys/types.h>
	#if defined(EA_HAVE_UTIME_H)
		#include <utime.h>        // Some systems may require <sys/utime.h>
	#endif
	#if defined(EA_HAVE_STATVFS_H)
		#include <sys/statvfs.h>
	#endif
	#ifdef EA_PLATFORM_ANDROID
		#include <sys/statfs.h>

		namespace EA{
			namespace IO{
				// Include this within the EA::IO namespace.
				#ifndef ASSETMANAGER_INIT_FAILURE_MESSAGE
					#define ASSETMANAGER_INIT_FAILURE_MESSAGE "Note: EAIO.Startup(activityInstance) must be called from Java prior to accessing appbundle files."
				#endif
				#include "Android/assetmanagerjni.h"
			}
		}
	#endif
	#if defined(EA_PLATFORM_NX)
		#include <EAStdC/EACType.h>
		#include <nn/fs.h>
		#include <nn/nn_Result.h>
		#include <nn/fs/fs_Rom.h>
		#include <nn/fs/fs_Mount.h>
		#include <nn/fs/fs_RomForDebug.h>
	#endif

	// Some libraries don't define all access identifiers.
	#ifndef S_IRUSR
		#define S_IRWXU 00700
		#define S_IRUSR 00400
		#define S_IWUSR 00200
		#define S_IXUSR 00100
	#endif
	#ifndef S_IRGRP
		#define S_IRWXG 00070
		#define S_IRGRP 00040
		#define S_IWGRP 00020
		#define S_IXGRP 00010
	#endif
	#ifndef S_IROTH
		#define S_IRWXO 00007
		#define S_IROTH 00004
		#define S_IWOTH 00002
		#define S_IXOTH 00001
	#endif
#elif defined(EA_PLATFORM_SONY)
	#include <scebase_common.h>
	#include <app_content.h>
	#include <kernel.h>
	#include <sdk_version.h>
#endif


///////////////////////////////////////////////////////////////////////////////
// EAIO_MKSTEMPS_AVAILABLE
//
// http://linux.die.net/man/3/mkstemp
//
#if (defined(EA_PLATFORM_APPLE) || defined(EA_PLATFORM_BSD) || (defined(EA_PLATFORM_LINUX) && (defined(_BSD_SOURCE) || defined(_SVID_SOURCE)))) && (!defined(EA_COMPILER_GNUC) || (__GLIBC__ >= 2 && __GLIBC_MINOR__ >= 11 ))
	#define EAIO_MKSTEMPS_AVAILABLE 1
#else
	#define EAIO_MKSTEMPS_AVAILABLE 0
#endif


#ifdef _MSC_VER
	#pragma warning(disable: 4127) // conditional expression is constant
#endif


namespace
{
	#if (EAIO_MKSTEMPS_AVAILABLE == 0) // We use EAIOTime and EAIORand in the case that we can't use mkstemps.

		// Returns seconds, though unlike the C time() function this function doesn't 
		// need to return time based off of 1/1/1970.
		time_t EAIOTime()
		{
			time_t nTime = time(NULL);
			return nTime;
		}

		uint32_t EAIORand()
		{
			// This randomization code is borrowed from the EAStdC package.
			static uint32_t seed = 0;

			if(seed == 0)
			{
				seed = (uint32_t)EAIOTime();
				if(seed == 0)
					seed = 0x13572468;
			}

			const uint64_t nResult64 = (seed * (uint64_t)1103515245) + 12345;
			seed = (uint32_t)nResult64;

			return (uint32_t)(nResult64 >> 16);
		}
		
	#endif
} // namespace

#if (EASTDC_VERSION_N < 10601) // Newer versions of EAStdC have this built-in.
	namespace EA
	{
		namespace StdC
		{
			namespace Internal
			{
				template <typename Dest, typename DestCharType, typename Source>
				struct Strlcpy2Class
				{
					static bool Strlcpy2Impl(Dest& d, const Source* pSource, size_t sourceLength)
					{
						const int requiredStrlen = EA::StdC::Strlcpy(&d[0], pSource, 0, sourceLength);

						if(requiredStrlen >= 0)
						{
							d.resize((typename Dest::size_type)requiredStrlen);
							EA::StdC::Strlcpy(&d[0], pSource, d.length() + 1, sourceLength);

							return true;
						}

						d.clear();
						return false;
					}
				};

				template <typename Dest, typename CharType> // Specialization for the case that source and dest are equal.
				struct Strlcpy2Class<Dest, CharType, CharType>
				{
					static bool Strlcpy2Impl(Dest& d, const CharType* pSource, size_t sourceLength)
						{ d.assign(pSource, sourceLength); return true; }
				};
			}

			template <typename Dest, typename Source>
			inline bool Strlcpy(Dest& d, const Source* pSource, size_t sourceLength = (size_t)~0)
			{
				if(sourceLength == (size_t)~0)
					sourceLength = Strlen(pSource);

				return Internal::Strlcpy2Class<Dest, typename Dest::value_type, Source>::Strlcpy2Impl(d, pSource, sourceLength);
			}
		}
	}
#endif



#if defined(EA_PLATFORM_MICROSOFT)
	namespace EA
	{
		namespace IO
		{
			namespace Internal
			{
				// To do: Move these functions to EAStdC/EADateTime
				void TimeTToFILETIME(const time_t& t, FILETIME& fileTime)
				{
					 // Note that LONGLONG is a 64-bit value
					 const uint64_t u64 = ((uint64_t)t * 10000000) + UINT64_C(116444736000000000);
					 fileTime.dwLowDateTime  = (DWORD)(u64);
					 fileTime.dwHighDateTime = (DWORD)(u64 >> 32);
				}


				void FILETIMEToTimeT(const FILETIME& fileTime, time_t& t)
				{
					const uint64_t u64 = ((uint64_t)fileTime.dwHighDateTime << 32) + fileTime.dwLowDateTime;
					t = (time_t)((u64 - UINT64_C(116444736000000000)) / 10000000);
				}
			}
		}
	}
#endif


#if defined(EA_PLATFORM_NX)
	// We do simple checks here to make sure the path is valid.  If we send an invalid path to the NX libs,
	// they will assert and bring down the application.
	/*
		From NX Docs:
		It begins with[mount_name + ":/"] or [mount_name + ":\"]
		The mount name is a valid mount name.
		The mount name is one that is already mounted.
		The directory delimiter is a forward slash(/) or backslash(\).
		Entry names do not use the symbols < > * ? : or |.
		The string is no longer than 260 characters.
		It must be a string from one to nn::fs::EntryNameLengthMax bytes, not including the terminating null character.
	*/
	template<typename T>
	static bool IsPathValid(T path)
	{
		if (!path || path[0] == '\0')
			return false;

		size_t strLen = EA::StdC::Strlen(path);
		if (!(strLen >= 1 && strLen < EA::IO::kMaxPathLength))
			return false;

		if (!EA::StdC::Isalpha(path[0]))
			return false;

		auto driveSeperator = EA::StdC::Strchr(path, ':');
		if (driveSeperator == nullptr)
			return false;

		auto pathSeperator = path[(driveSeperator - path) + 1];
		if (!(pathSeperator == '\\' || pathSeperator == '/'))
			return false;

		return true;
	}

#endif

namespace EA
{

namespace IO
{

////////////////////////////////////////////////////////////////////////////
// gpTempDirectory
//
// Temporary directory that the user can specify to take precedence over 
// a system default temp directory. The user can use the GetTempDirectory
// and SetTempDirectory functions to get and set this value.
//
const uint32_t kTempDirectoryLength             = kMaxDirectoryLength + 1;
char16_t gpTempDirectory[kTempDirectoryLength]  = { 0 };



////////////////////////////////////////////////////////////////////////////
// gpCurrentWorkingDirectory
//
// This holds the current working directory. On platforms whereby the system
// supports the concept of current working directory, this value is not defined.
// On platforms whereby the the system does not support the concept of a 
// current working directory, this value is defined. The user can use 
// GetCurrentWorkingDirectory and SetCurrentWorkingDirectory to get and set
// this value.
//
// Note: A weakness of this variable is that system file io code is not aware
// of it and cannot use it. It can only be of use if the user intervenes and 
// reads this value before calling system file io functions.
//
#if EA_WINAPI_FAMILY_PARTITION(EA_WINAPI_PARTITION_DESKTOP) || EAIO_USE_UNIX_IO
	// Define nothing, as the system supports this.
#else
	const uint32_t kCWDLength                       = kMaxDirectoryLength + 1;
	char16_t gpCurrentWorkingDirectory[kCWDLength]  = { 0 };
	bool     gbCurrentWorkingDirectoryInitialized   = false;
#endif

////////////////////////////////////////////////////////////////////////////
// Sony functionality
//
#if defined(EA_PLATFORM_SONY)
	#include <app_content.h>    // Need to link libSceAppContent_stub_weak.a
	#include <libsysmodule.h>   // Need to link libSceSysmodule_stub_weak.a

	// Note: https://ps4.scedev.net/technotes/view/134/1 The code below may want to use the new function when it's available.
	// Note: https://ps4.scedev.net/support/issue/5862/_Meeting_followup_-_Is_there_a_cached_partition_and_what_is_its_cost

	EAIO_API int PS4GetTemporaryDataMount(char* pDirectory, size_t nDirectoryPathCapacity, bool registerAsEAIOTempDirectory)
	{
		SceAppContentInitParam initParam;
		memset(&initParam, 0, sizeof(SceAppContentInitParam));

		SceAppContentBootParam bootParam;
		memset(&bootParam, 0, sizeof(SceAppContentBootParam));

		sceSysmoduleLoadModule(SCE_SYSMODULE_APP_CONTENT);

		int result = sceAppContentInitialize(&initParam, &bootParam);

		if((result == SCE_OK) || (result == SCE_APP_CONTENT_ERROR_BUSY)) // If OK or if already initialized...
		{
			SceAppContentMountPoint mountPoint;

			#if (SCE_ORBIS_SDK_VERSION >= 0x01500111u) // Another option is SCE_APP_CONTENT_TEMPORARY_DATA_OPTION_FORMAT, which force-formats the data mount.
				result = sceAppContentTemporaryDataMount2(SCE_APP_CONTENT_TEMPORARY_DATA_OPTION_NONE, &mountPoint);
			#else
				result = sceAppContentTemporaryDataMount(&mountPoint); // Removed in SDK 1.5+ because of https://ps4.scedev.net/technotes/view/134
			#endif

			// As of SDK 1.050 (October 2013), Sony has stated that returned path isn't guaranteed
			// to be "/temp0". Thus if result is SCE_APP_CONTENT_ERROR_BUSY (already called) then we 
			// cannot simply assume that when somebody else in our process previously called it the 
			// returned value was "/temp0". Sony will be revisiting this in SDK 1.500 with this and 
			// other issues in mind.
 
			if(result == SCE_OK) // If OK...
			{
				// If pDirectory is NULL then registerAsEAIOTempDirectory must be true, else the 
				// registered mount point will be lost.
				EA_ASSERT(pDirectory || registerAsEAIOTempDirectory);

				if(pDirectory)
				{
					result = EA::StdC::Strlcpy(pDirectory, mountPoint.data, nDirectoryPathCapacity);
						
					if(result < (int)nDirectoryPathCapacity) // If there was enough capacity...
						result = SCE_OK;
					else
						result = SCE_APP_CONTENT_ERROR_PARAMETER;
				}

				if(registerAsEAIOTempDirectory && (result == SCE_OK))
					EA::IO::SetTempDirectory(mountPoint.data);
			} // Else the result is probably SCE_APP_CONTENT_ERROR_BUSY, which means somebody else 
		}     // in our process already did this, and we can't know what the resulting path was.

		// Don't do this because it would cause the temp dir to be unmounted.
		//sceSysmoduleUnloadModule(SCE_SYSMODULE_APP_CONTENT);

		return result;
	}
#endif


////////////////////////////////////////////////////////////////////////////
// DriveInfo32::DriveInfo32
//
DriveInfo32::DriveInfo32(const char32_t* pName, DriveType driveType)
	: mType(driveType)
{
	if(pName)
		mName.assign(pName);
}

////////////////////////////////////////////////////////////////////////////
// DriveInfo16::DriveInfo16
//
DriveInfo16::DriveInfo16(const char16_t* pName, DriveType driveType)
	: mType(driveType)
{
	if(pName)
		mName.assign(pName);
}

////////////////////////////////////////////////////////////////////////////
// DriveInfo8::DriveInfo8
//
DriveInfo8::DriveInfo8(const char* pName, DriveType driveType)
	: mType(driveType)
{
	if(pName)
		mName.assign(pName);
}


#if EA_WCHAR_UNIQUE
	////////////////////////////////////////////////////////////////////////////
	// DriveInfoW::DriveInfoW
	//
	DriveInfoW::DriveInfoW(const wchar_t* pName, DriveType driveType)
		: mType(driveType)
	{
		if(pName)
			mName.assign(pName);
	}
#endif


////////////////////////////////////////////////////////////////////////////
// File::Create
//
EAIO_API bool File::Create(const char32_t* pPath, bool bTruncate)
{
	Path::PathString8 path8;
	ConvertPath(path8, pPath);
	return File::Create(path8.c_str(), bTruncate);
}

////////////////////////////////////////////////////////////////////////////
// File::Create
//
EAIO_API bool File::Create(const char16_t* pPath, bool bTruncate)
{
	if(!pPath || !(*pPath))
		return false;

	#if defined(EA_PLATFORM_MICROSOFT)

		HANDLE hFile = INVALID_HANDLE_VALUE;

		#if EA_WINAPI_FAMILY_PARTITION(EA_WINAPI_PARTITION_DESKTOP | EA_WINAPI_PARTITION_TV_TITLE | EA_WINAPI_PARTITION_GAMES)

			SECURITY_ATTRIBUTES tempSECURITY_ATTRIBUTES;
			tempSECURITY_ATTRIBUTES.nLength              = sizeof(tempSECURITY_ATTRIBUTES);
			tempSECURITY_ATTRIBUTES.lpSecurityDescriptor = NULL;
			tempSECURITY_ATTRIBUTES.bInheritHandle       = FALSE;

			// We have a problem here if the given file exists and is open by another 
			// entity but in write mode with no ability for us to read the file.
			// In this case we will probably want to change this code to try to test
			// for the existence of the file before trying to create it.
			hFile = CreateFileW(reinterpret_cast<LPCWSTR>(pPath), GENERIC_READ, FILE_SHARE_READ | FILE_SHARE_WRITE, 
								&tempSECURITY_ATTRIBUTES, OPEN_ALWAYS, FILE_ATTRIBUTE_NORMAL, NULL);

		#else

			 CREATEFILE2_EXTENDED_PARAMETERS temp_CREATEFILE2_EXTENDED_PARAMETERS;
			temp_CREATEFILE2_EXTENDED_PARAMETERS.dwSize               = sizeof(temp_CREATEFILE2_EXTENDED_PARAMETERS);
			temp_CREATEFILE2_EXTENDED_PARAMETERS.dwFileAttributes     = FILE_ATTRIBUTE_NORMAL;
			temp_CREATEFILE2_EXTENDED_PARAMETERS.dwFileFlags          = 0;
			temp_CREATEFILE2_EXTENDED_PARAMETERS.dwSecurityQosFlags   = SECURITY_ANONYMOUS;
			temp_CREATEFILE2_EXTENDED_PARAMETERS.lpSecurityAttributes = NULL;
			temp_CREATEFILE2_EXTENDED_PARAMETERS.hTemplateFile        = NULL;

			#if EAIO_WINDOWS8_EMULATION_ENABLED
				hFile = CreateFile2EAIO(pPath, GENERIC_READ, FILE_SHARE_READ | FILE_SHARE_WRITE, 
										OPEN_ALWAYS, &temp_CREATEFILE2_EXTENDED_PARAMETERS);
			#else
				// Microsoft has declared that we should enable at least read sharing whenever CreateFile2 is used for this platform.
				hFile = CreateFile2(pPath, GENERIC_READ, FILE_SHARE_READ | FILE_SHARE_WRITE, 
									OPEN_ALWAYS, &temp_CREATEFILE2_EXTENDED_PARAMETERS);
			#endif
		#endif

		if(hFile != INVALID_HANDLE_VALUE)
		{
			if(bTruncate)
			{
				LARGE_INTEGER distToMove;
				LARGE_INTEGER newFilePointer;
				distToMove.QuadPart = 0;
				SetFilePointerEx(hFile, distToMove, &newFilePointer, FILE_BEGIN);
				SetEndOfFile(hFile);
			}

			CloseHandle(hFile);
			return true;
		}

		return false;

	#elif EAIO_USE_UNIX_IO

		char path8[kMaxPathLength];
		size_t n = EA::StdC::Strlcpy(path8, pPath, kMaxPathLength);

		if(n < (size_t)kMaxPathLength)
		{
			// We have a problem here if the given file exists and is open by another 
			// entity but in write mode with no ability for us to read the file.
			const int fileHandle = open(path8, O_RDWR | O_CREAT | (bTruncate ? O_TRUNC : 0), 0777);

			if(fileHandle >= 0)
			{
				close(fileHandle);
				return true;
			}
		}

		return false;

	#elif defined(EA_PLATFORM_SONY)

		char path8[kMaxPathLength];
		size_t n = EA::StdC::Strlcpy(path8, pPath, kMaxPathLength);

		if (n < (size_t)kMaxPathLength)
		{
			SceKernelMode accessMode = SCE_KERNEL_S_IRWXU;
			const int fileHandle = sceKernelOpen(path8, SCE_KERNEL_O_RDWR | SCE_KERNEL_O_CREAT | (bTruncate ? SCE_KERNEL_O_TRUNC : 0), accessMode);

			if (fileHandle >= 0)
			{
				sceKernelClose(fileHandle);
				return true;
			}
		}

		return false;

	#else
		EA_FAIL_M("Unimplemented");

		// Bug Paul Pedriana to finish this for the given platform.
		EA_UNUSED(pPath); EA_UNUSED(bTruncate);        
		return false;

	#endif
}

EAIO_API bool File::Create(const char* pPath, bool bTruncate)
{
	#if EAIO_USE_UNIX_IO // Special case for Unix because Unix paths aren't necessarily UTF8.
		if(pPath && pPath[0])
		{
			// We have a problem here if the given file exists and is open by another 
			// entity but in write mode with no ability for us to read the file.


			const int fileHandle = open(pPath, O_RDWR | O_CREAT | (bTruncate ? O_TRUNC : 0), 0777);

			if(fileHandle >= 0)
			{
				close(fileHandle);
				return true;
			}
		}

		return false;

	#else
		using namespace Path;

		PathString16 path16;
		ConvertPath(path16, pPath);

		return File::Create(path16.c_str(), bTruncate);
	#endif
}




////////////////////////////////////////////////////////////////////////////
// File::Exists
//
EAIO_API bool File::Exists(const char32_t* pPath)
{
	Path::PathString8 path8;
	ConvertPath(path8, pPath);
	return File::Exists(path8.c_str());
}

EAIO_API bool File::Exists(const char16_t* pPath)
{
	if(pPath && *pPath)
	{
		#if defined(EA_PLATFORM_MICROSOFT)
			WIN32_FILE_ATTRIBUTE_DATA fileAttr;

			if (GetFileAttributesExW(reinterpret_cast<LPCWSTR>(pPath), GetFileExInfoStandard, &fileAttr) == FALSE)
				return false;

			return ((fileAttr.dwFileAttributes != INVALID_FILE_ATTRIBUTES) && ((fileAttr.dwFileAttributes & FILE_ATTRIBUTE_DIRECTORY) == 0));
		#elif defined(EA_PLATFORM_ANDROID)
			char path8[kMaxPathLength];
			size_t n = EA::StdC::Strlcpy(path8, pPath, kMaxPathLength);

			if(n < (size_t)kMaxPathLength) // If the string could be fully copied... (Question: what's the max path lenght of appbundle file paths?
			{
				if(strstr(path8, APPBUNDLE_PATH_PREFIX) == path8) // If the path begins with the APPBUNDLE_PATH_PREFIX prefix..
				{
					AutoJNIEnv autoEnv;

					EA::IO::Path::PathString8 pathString = path8 + APPBUNDLE_PATH_PREFIX_LENGTH;
					EA::IO::Path::Simplify(pathString);

					bool result = AssetManagerJNI::Exists(autoEnv, pathString.c_str());

					// To consider: We would probably need to add the following if the above Exists returned true for directory paths.
					//if(result)
					//    result = !DirectoryExists(path8);

					return result;
				}
				else
				{
					struct stat tempStat;
					const int result = stat(path8, &tempStat);

					if(result == 0)
					{
						if(S_ISREG(tempStat.st_mode))
							return true;

						#if defined(S_ISWHT) // http://free-it.de/archiv/talks_2005/paper-11254/paper-11254.html#N10174
							if(S_ISWHT(tempStat.st_mode) && !S_ISDIR(tempStat.st_mode))
								return true;
						#endif
					}
				}
			}
		
		#elif defined(EA_PLATFORM_NX)
			if(!::IsPathValid(pPath))
				return false;

			char path8[kMaxPathLength];
			EA::StdC::Strlcpy(path8, pPath, kMaxPathLength);

			nn::fs::DirectoryEntryType dt;
			if (nn::fs::GetEntryType(&dt, path8).IsSuccess() && dt == nn::fs::DirectoryEntryType_File)
				return true;

			return false;

		#elif EAIO_USE_UNIX_IO && defined(EA_HAVE_SYS_STAT_H)

			char path8[kMaxPathLength];
			size_t n = EA::StdC::Strlcpy(path8, pPath, kMaxPathLength);

			if(n < (size_t)kMaxPathLength) // If the string could be fully copied...
			{
				struct stat tempStat;
				const int result = stat(path8, &tempStat);

				if(result == 0)
				{
					if(S_ISREG(tempStat.st_mode))
						return true;

					#if defined(S_ISWHT) // http://free-it.de/archiv/talks_2005/paper-11254/paper-11254.html#N10174
						if(S_ISWHT(tempStat.st_mode) && !S_ISDIR(tempStat.st_mode))
							return true;
					#endif
				}
			}

		#elif defined(EA_PLATFORM_SONY)

			char path8[kMaxPathLength];
			size_t n = EA::StdC::Strlcpy(path8, pPath, kMaxPathLength);

			if (n < (size_t)kMaxPathLength) // If the string could be fully copied...
			{
				SceKernelStat tempStat;
				const int result = sceKernelStat(path8, &tempStat);

				if (result == SCE_OK)
					return SCE_KERNEL_S_ISREG(tempStat.st_mode) != 0;
			}

		#else
			EA_FAIL_M("Unimplemented");
			// Bug Paul Pedriana to finish this for the given platform.
		#endif
	}

	return false;
}

EAIO_API bool File::Exists(const char* pPath)
{
	#if EAIO_USE_UNIX_IO // Special case for Unix because Unix paths aren't necessarily UTF8.
		#if defined(EA_PLATFORM_NX)
			if (!::IsPathValid(pPath))
				return false;

			nn::fs::DirectoryEntryType dt;
			if (nn::fs::GetEntryType(&dt, pPath).IsSuccess() && dt == nn::fs::DirectoryEntryType_File)
				return true;
			return false;
		#elif defined(EA_HAVE_SYS_STAT_H)
			if(pPath && pPath[0])
			{
				#if defined(EA_PLATFORM_ANDROID)
					if(strstr(pPath, APPBUNDLE_PATH_PREFIX) == pPath) // If the path begins with the APPBUNDLE_PATH_PREFIX prefix..
					{
						AutoJNIEnv autoEnv;
						
						EA::IO::Path::PathString8 pathString = pPath + APPBUNDLE_PATH_PREFIX_LENGTH;
						EA::IO::Path::Simplify(pathString);

						bool result = AssetManagerJNI::Exists(autoEnv, pathString.c_str());

						// To consider: We would probably need to add the following if the above Exists returned true for directory paths.
						//if(result)
						//    result = !DirectoryExists(pPath);

						return result;
					}
				#endif

				struct stat tempStat;
				const int result = stat(pPath, &tempStat);

				if(result == 0)
				{
					if(S_ISREG(tempStat.st_mode))
						return true;

					#if defined(S_ISWHT) // http://free-it.de/archiv/talks_2005/paper-11254/paper-11254.html#N10174
						if(S_ISWHT(tempStat.st_mode) && !S_ISDIR(tempStat.st_mode))
							return true;
					#endif
				}
			}

			return false;
		#else
			return false;
		#endif
	#elif defined(EA_PLATFORM_SONY)
		SceKernelStat tempStat;
		const int     result = sceKernelStat(pPath, &tempStat);

		if(result == SCE_OK)
			return SCE_KERNEL_S_ISREG(tempStat.st_mode) != 0;

		return false;

	#else
		using namespace Path;

		PathString16 path16;
		ConvertPath(path16, pPath);

		return File::Exists(path16.c_str());
	#endif
}



namespace
{
	// This function uses wchar_t instead of char16_t and char32_t because the DirectoryIterator
	// class is currently defined in terms of wchar_t, and we use it below.
	bool PatternExistsW(const wchar_t* pPathPattern)
	{
		using namespace Path;

		// The EntryFindFirst function requires a valid directory path.
		// We must split pPathPattern into a directory and file component.
		const wchar_t* const pFileNamePattern = Path::GetFileName(pPathPattern);
		PathStringW directoryW(pPathPattern, pFileNamePattern);

		EntryFindData entryFindData;
		EntryFindData* const pEntryFindData = EntryFindFirst(directoryW.c_str(), pFileNamePattern, &entryFindData);

		if(pEntryFindData)
			EntryFindFinish(pEntryFindData);

		return (pEntryFindData != NULL);
	}
}

////////////////////////////////////////////////////////////////////////////
// File::PatternExists
//
EAIO_API bool File::PatternExists(const char32_t* pPathPattern)
{
	#if !EA_WCHAR_UNIQUE && (EA_WCHAR_SIZE == 4) // if wchar_t == char32_t
		return PatternExistsW(pPathPattern);
	#else
		EA::IO::Path::PathStringW patternW;
		ConvertPath(patternW, pPathPattern);

		return PatternExistsW(patternW.c_str());
	#endif
}

EAIO_API bool File::PatternExists(const char16_t* pPathPattern)
{
	#if !EA_WCHAR_UNIQUE && (EA_WCHAR_SIZE == 2) // if wchar_t == char16_t
		return PatternExistsW(pPathPattern);
	#else
		EA::IO::Path::PathStringW patternW;
		ConvertPath(patternW, pPathPattern);

		return PatternExistsW(patternW.c_str());
	#endif
}

EAIO_API bool File::PatternExists(const char* pPathPattern)
{
	// We have a limitation here: The input file pattern has to be UTF8, even on 
	// platforms that don't have explicitly UTF8 file systems (e.g. Unix). This limitation
	// exists because currently our EntryFindFirst/EntryFindNext functionality is wchar_t only.

	#if(EA_WCHAR_SIZE == 1) // if wchar_t == char
		return PatternExistsW(pPathPattern);
	#else
		EA::IO::Path::PathStringW patternW;
		ConvertPath(patternW, pPathPattern);

		return PatternExistsW(patternW.c_str());
	#endif
}




////////////////////////////////////////////////////////////////////////////
// File::IsWritable
//
// Todo: Complete this function properly. The current implementation
//       of reading the attribute isn't enough for a rigorous test.

EAIO_API bool File::IsWritable(const char32_t* pPath)
{
	return (GetAttributes(pPath) & kAttributeWritable) != 0;
}

EAIO_API bool File::IsWritable(const char16_t* pPath)
{
	return (GetAttributes(pPath) & kAttributeWritable) != 0;
}

EAIO_API bool File::IsWritable(const char* pPath)
{
	return (GetAttributes(pPath) & kAttributeWritable) != 0;
}



////////////////////////////////////////////////////////////////////////////
// File::Remove
//
EAIO_API bool File::Remove(const char32_t* pPath)
{
	Path::PathString8 path8;
	ConvertPath(path8, pPath);
	return File::Remove(path8.c_str());
}

////////////////////////////////////////////////////////////////////////////
// File::Remove
//
EAIO_API bool File::Remove(const char16_t* pPath)
{
	if(!pPath || !(*pPath))
		return false;

	#if defined(EA_PLATFORM_MICROSOFT)

		const BOOL bResult = ::DeleteFileW(reinterpret_cast<LPCWSTR>(pPath));
		// Possibly save off an error value here.
		return (bResult != 0);

	#elif EAIO_USE_UNIX_IO

		char path8[kMaxPathLength];
		EA::StdC::Strlcpy(path8, pPath, kMaxPathLength);

		const int result = unlink(path8);
		return (result == 0);

	#elif defined(EA_PLATFORM_SONY)
	
		char path8[kMaxPathLength];
		size_t n = EA::StdC::Strlcpy(path8, pPath, kMaxPathLength);

		if (n < (size_t)kMaxPathLength)
		{
			return (sceKernelUnlink(path8) == SCE_OK);
		}

		return false;

	#else
		EA_FAIL_M("Unimplemented");

		// Bug Paul Pedriana to finish this for the given platform.
		EA_UNUSED(pPath);
		return false;

	#endif
}

EAIO_API bool File::Remove(const char* pPath)
{
	#if EAIO_USE_UNIX_IO // Special case for Unix because Unix paths aren't necessarily UTF8.
		if(pPath && pPath[0])
		{
			const int result = unlink(pPath);
			return (result == 0);
		}

		return false;

	#else
		using namespace Path;

		PathString16 path16;
		ConvertPath(path16, pPath);

		return File::Remove(path16.c_str());
	#endif
}




////////////////////////////////////////////////////////////////////////////
// File::Move
//
EAIO_API bool File::Move(const char32_t* pPathSource, const char32_t* pPathDestination, bool bOverwriteIfPresent)
{
	Path::PathString8 pathSource8, pathDest8;
	ConvertPath(pathSource8, pPathSource);
	ConvertPath(pathDest8, pPathDestination);

	return File::Move(pathSource8.c_str(), pathDest8.c_str(), bOverwriteIfPresent);
}

EAIO_API bool File::Move(const char16_t* pPathSource, const char16_t* pPathDestination, bool bOverwriteIfPresent)
{
	using namespace EA::IO::Path;

	// Check for NULL or empty path strings.  If so, just return false.
	if(!pPathSource || !(*pPathSource) || !pPathDestination || !(*pPathDestination))
		return false;

	#if defined(EA_PLATFORM_MICROSOFT)

		if(bOverwriteIfPresent || !Exists(pPathDestination))
			return (::MoveFileExW(reinterpret_cast<LPCWSTR>(pPathSource), reinterpret_cast<LPCWSTR>(pPathDestination), MOVEFILE_REPLACE_EXISTING | MOVEFILE_COPY_ALLOWED) != 0);
		return false;

	#elif EAIO_USE_UNIX_IO
		
		char srcPath8[kMaxPathLength];
		EA::StdC::Strlcpy(srcPath8, pPathSource, kMaxPathLength);

		char dstPath8[kMaxPathLength];
		EA::StdC::Strlcpy(dstPath8, pPathDestination, kMaxPathLength);
		return Move(srcPath8, dstPath8, bOverwriteIfPresent);

	#elif defined(EA_PLATFORM_SONY)

		char srcPath8[kMaxPathLength];
		size_t n = EA::StdC::Strlcpy(srcPath8, pPathSource, kMaxPathLength);
		if (n >= (size_t)kMaxPathLength)
			return false;

		char dstPath8[kMaxPathLength];
		n = EA::StdC::Strlcpy(dstPath8, pPathDestination, kMaxPathLength);
		if (n >= (size_t)kMaxPathLength)
			return false;

		if (!bOverwriteIfPresent && Exists(dstPath8))
			return false;

		if (sceKernelRename(srcPath8, dstPath8) == SCE_OK)
			return true;

		// If we failed, it might be because we tried to move between two different filesystems.
		// In which case we must manually copy.
		if (Copy(pPathSource, pPathDestination, bOverwriteIfPresent))
		{
			Remove(pPathSource); // To consider: Back out of the copy if the remove fails.
			return true;
		}

		return false;

	#else
		EA_FAIL_M("Unimplemented");

		// Bug Paul Pedriana to finish this for the given platform.
		EA_UNUSED(pPathSource); EA_UNUSED(pPathDestination); EA_UNUSED(bOverwriteIfPresent);
		return false;

	#endif
}

EAIO_API bool File::Move(const char* pPathSource, const char* pPathDestination, bool bOverwriteIfPresent)
{
	#if EAIO_USE_UNIX_IO // Special case for Unix because Unix paths aren't necessarily UTF8.
		if(!bOverwriteIfPresent && Exists(pPathDestination))
			return false;
		
		// The NX has a non-standard rename implementation so we must use the nn version
		#if defined(EA_PLATFORM_NX)
			if (!::IsPathValid(pPathSource) || !::IsPathValid(pPathDestination))
				return false;

			if (nn::fs::RenameFile(pPathSource, pPathDestination).IsSuccess())
				return true;
		#else
			if(rename(pPathSource, pPathDestination) == 0)
				return true;
		#endif

		// If we failed, it might be because we tried to move between two different filesystems.
		// In which case we must manually copy.
		if(Copy(pPathSource, pPathDestination, bOverwriteIfPresent))
		{
			Remove(pPathSource); // To consider: Back out of the copy if the remove fails.
			return true;
		}

		return false;

	#else
		using namespace Path;

		PathString16 pathSrc16;
		ConvertPath(pathSrc16, pPathSource);

		PathString16 pathDst16;
		ConvertPath(pathDst16, pPathDestination);

		return File::Move(pathSrc16.c_str(), pathDst16.c_str(), bOverwriteIfPresent);
	#endif
}




////////////////////////////////////////////////////////////////////////////
// File::Copy
//
EAIO_API bool File::Copy(const char32_t* pPathSource, const char32_t* pPathDestination, bool bOverwriteIfPresent)
{
	Path::PathString8 pathSource8, pathDest8;
	ConvertPath(pathSource8, pPathSource);
	ConvertPath(pathDest8, pPathDestination);

	return File::Copy(pathSource8.c_str(), pathDest8.c_str(), bOverwriteIfPresent);
}

///////////////////////////////////////////////////////////////////////////////
// File::Copy
//
EAIO_API bool File::Copy(const char16_t* pPathSource, const char16_t* pPathDestination, bool bOverwriteIfPresent)
{
	if(!(bOverwriteIfPresent || !Exists(pPathDestination)))
		return false;

	EA::IO::FileStream srcStream(pPathSource);
	bool result = srcStream.Open(EA::IO::kAccessFlagRead, kCDOpenExisting, EA::IO::FileStream::kShareRead, EA::IO::FileStream::kUsageHintNone);

	if(!result)
		return false;

	EA::IO::FileStream dstStream(pPathDestination);
	result = dstStream.Open(EA::IO::kAccessFlagWrite, kCDCreateAlways, EA::IO::FileStream::kShareWrite, EA::IO::FileStream::kUsageHintNone);

	if(!result)
	{
		srcStream.Close();
		return false;
	}

	const uint64_t kBufferSize(1024);
	char           pBuffer[kBufferSize];
	size_type      readCount = 0;

	do
	{
		readCount = srcStream.Read(pBuffer, kBufferSize);

		if(readCount > 0)
		{    
			size_type writeCount      = 0;
			size_type writtenCount    = 0;
			off_type previousPosition = dstStream.GetPosition(kPositionTypeBegin);

			do 
			{
				result = dstStream.Write(pBuffer + writeCount, readCount - writeCount);
				writtenCount = dstStream.GetPosition(kPositionTypeBegin) - previousPosition;
			} while((result) && ((writeCount += writtenCount) < readCount));
		}
	}while((result) && (readCount > 0));

	dstStream.Close();

	return result;
}

EAIO_API bool File::Copy(const char* pPathSource, const char* pPathDestination, bool bOverwriteIfPresent)
{
	#if EAIO_USE_UNIX_IO // Special case for Unix because Unix paths aren't necessarily UTF8.
		bool bResult = false;

		if(bOverwriteIfPresent || !Exists(pPathDestination))
		{
			EA::IO::FileStream fileSource(pPathSource);

			if(fileSource.Open(kAccessFlagRead))
			{
				EA::IO::FileStream fileDest(pPathDestination);

				if(fileDest.Open(kAccessFlagReadWrite, kCDCreateAlways))
				{
					const size_type kBufferSize(2048);
					char            pBuffer[kBufferSize];
					size_type       readCount = 0;

					bResult = true; // True until proven false below.

					do {
						readCount = fileSource.Read(pBuffer, kBufferSize);

						if(readCount == kSizeTypeError)
							bResult = false;
						else if(readCount > 0)
							bResult = fileDest.Write(pBuffer, readCount);
					} while (bResult && (readCount > 0)); // While no error and while we aren't done yet.
				}
			}
		}

		return bResult;
	#else
		char16_t srcPath16[kMaxPathLength];
		EA::StdC::Strlcpy(srcPath16, pPathSource, kMaxPathLength);

		char16_t dstPath16[kMaxPathLength];
		EA::StdC::Strlcpy(dstPath16, pPathDestination, kMaxPathLength);

		return File::Copy(srcPath16, dstPath16, bOverwriteIfPresent);
	#endif
}



////////////////////////////////////////////////////////////////////////////
// File::Stat
//
EAIO_API bool File::Stat(const char32_t* pPath, StatData& statData)
{
	Path::PathString8 path8;
	ConvertPath(path8, pPath);
	return File::Stat(path8.c_str(), statData);
}

EAIO_API bool File::Stat(const char16_t* pPath, StatData& statData)
{
	// For Windows and some other supporting Microsoft platforms we need to use wchar_t paths.
	#if defined(EA_PLATFORM_MICROSOFT)
		// Microsoft provides a stat/_wstat function for getting the time of a file, 
		// but their implementation is non-conforming. In particular the Microsoft stat 
		// function implementations convert file time to local time:
		//      http://support.microsoft.com/kb/190315.
		// Unix-based stat usage is OK, as Unix stat doesn't adjust time values and 
		// instead uses UTC values without modification.
		WIN32_FIND_DATAW win32FindData;

		#if EA_WINAPI_FAMILY_PARTITION(EA_WINAPI_PARTITION_DESKTOP | EA_WINAPI_PARTITION_TV_TITLE | EA_WINAPI_PARTITION_GAMES)
			HANDLE hFindFile = FindFirstFileW(reinterpret_cast<LPCWSTR>(pPath), &win32FindData);
		#else
			HANDLE hFindFile = FindFirstFileExW(pPath, FindExInfoBasic, &win32FindData, FindExSearchNameMatch, NULL, 0);
		#endif

		if(hFindFile != INVALID_HANDLE_VALUE)
		{
			statData.mFlags = kStatFlagAll;
			statData.mSize  = ((uint64_t)win32FindData.nFileSizeHigh << 32) + win32FindData.nFileSizeLow;
			Internal::FILETIMEToTimeT(win32FindData.ftCreationTime, statData.mCreationTime); // Some implementations of stat define ctime to be something other than creation time.
			Internal::FILETIMEToTimeT(win32FindData.ftLastWriteTime, statData.mModificationTime);
			Internal::FILETIMEToTimeT(win32FindData.ftLastAccessTime, statData.mAccessTime);

			FindClose(hFindFile);
			return true;
		}

		statData.mFlags = 0;
		return false;
	#else
		Path::PathString8 path8;
		ConvertPath(path8, pPath);
		return File::Stat(path8.c_str(), statData);
	#endif
}

EAIO_API bool File::Stat(const char* pPath, StatData& statData)
{
	#if defined(EA_PLATFORM_NX)
		nn::fs::FileHandle fh;
		if (::IsPathValid(pPath) && 
			nn::fs::OpenFile(&fh, pPath, nn::fs::OpenMode_Read).IsSuccess())
		{
			int64_t fs;
			nn::fs::GetFileSize(&fs, fh);

			statData.mFlags = kStatFlagSize;
			statData.mCreationTime = 0;
			statData.mModificationTime = 0;
			statData.mAccessTime = 0;
			statData.mSize = (uint64_t)fs;

			nn::fs::CloseFile(fh);

			return true;
		}
		return false;
	#elif (EAIO_USE_UNIX_IO && defined(EA_HAVE_SYS_STAT_H)) || defined(EA_PLATFORM_SONY)
		#if defined(EA_PLATFORM_SONY)

			SceKernelStat tempStat;
			const int result = (sceKernelStat(pPath, &tempStat) == SCE_OK ? 0 : -1);

		#elif EAIO_USE_UNIX_IO // Special case for Unix because Unix paths aren't necessarily UTF8.
			// Some platforms (especially mobile) provide an alternative read-only file system
			// embedded within the executable application/package. We have standardized on the 
			// convention that paths to this file system are prefixed with "appbundle:/"

			#if defined(EA_PLATFORM_ANDROID)
				if(strstr(pPath, APPBUNDLE_PATH_PREFIX) == pPath) // If the path begins with the APPBUNDLE_PATH_PREFIX prefix..
				{
					AutoJNIEnv autoEnv;
					EA::IO::Path::PathString8 pathString = pPath + APPBUNDLE_PATH_PREFIX_LENGTH;
					EA::IO::Path::Simplify(pathString);
					bool result = AssetManagerJNI::Exists(autoEnv, pathString.c_str());

					return (result ? 1319738365 : 0);  // 1319738365 is October 27, 2011. To do: Have this return the application executable time.
				}
			#endif

			struct stat tempStat;
			const int result = stat(pPath, &tempStat);
		#else
			const int result = -1;
		#endif

		if(result == 0)
		{
			statData.mFlags            = kStatFlagAll;
			statData.mCreationTime     = (time_t)tempStat.st_ctime;
			statData.mModificationTime = (time_t)tempStat.st_mtime;
			statData.mAccessTime       = (time_t)tempStat.st_atime;
			statData.mSize             = (uint64_t)tempStat.st_size;
			return true;
		}

		statData.mFlags = 0;
		return false;

	#elif defined(EA_PLATFORM_MICROSOFT)

		Path::PathString16 path16;
		ConvertPath(path16, pPath);
		return File::Stat(path16.c_str(), statData);

	#else
		// To do: Make this more efficient by making a system-specific call.
		statData.mFlags            = kStatFlagAll;
		statData.mCreationTime     = File::GetTime(pPath, kFileTimeTypeCreation);
		statData.mModificationTime = File::GetTime(pPath, kFileTimeTypeLastModification);
		statData.mAccessTime       = File::GetTime(pPath, kFileTimeTypeLastAccess);
		statData.mSize             = File::GetSize(pPath);
		return true;
	#endif
}




////////////////////////////////////////////////////////////////////////////
// File::GetSize
//
EAIO_API size_type File::GetSize(const char32_t* pPath)
{
	Path::PathString8 path8;
	ConvertPath(path8, pPath);
	return File::GetSize(path8.c_str());
}

///////////////////////////////////////////////////////////////////////////////
// File::GetSize
//
// Returns the file size in bytes.
//
EAIO_API size_type File::GetSize(const char16_t* pPath)
{
	size_type nReturnValue = kSizeTypeError;

	#if defined(EA_PLATFORM_MICROSOFT)

		WIN32_FIND_DATAW win32FindDataW;
		#if EA_WINAPI_FAMILY_PARTITION(EA_WINAPI_PARTITION_DESKTOP)
			const HANDLE hFindFile = FindFirstFileW(reinterpret_cast<LPCWSTR>(pPath), &win32FindDataW);
		#else
			const HANDLE hFindFile = FindFirstFileExW(reinterpret_cast<LPCWSTR>(pPath), FindExInfoBasic, &win32FindDataW, FindExSearchNameMatch, NULL, 0);
		#endif

		if(hFindFile != INVALID_HANDLE_VALUE)
		{
			nReturnValue = (size_type)win32FindDataW.nFileSizeLow;
			if(sizeof(size_type) >= sizeof(uint64_t))
				nReturnValue |= (size_type)((uint64_t)win32FindDataW.nFileSizeHigh << (uint64_t)32);
			FindClose(hFindFile);
		}

	#elif EAIO_USE_UNIX_IO

		Path::PathString8 path8;
		ConvertPath(path8, pPath);

		return File::GetSize(path8.c_str());
			
	#elif defined(EA_PLATFORM_SONY)

		char path8[kMaxPathLength];
		size_t n = EA::StdC::Strlcpy(path8, pPath, kMaxPathLength);

		if (n < (size_t)kMaxPathLength)
		{
			SceKernelStat st;
			if (sceKernelStat(path8, &st) == SCE_OK)
				nReturnValue = st.st_size;
		}
		
	#else
		EA_FAIL_M("Unimplemented");

		// Bug Paul Pedriana to finish this for the given platform.
		EA_UNUSED(pPath);

	#endif

	return nReturnValue;
}

EAIO_API size_type File::GetSize(const char* pPath)
{
	#if defined(EA_PLATFORM_NX)
		nn::fs::FileHandle fh;
		if (::IsPathValid(pPath) && 
			nn::fs::OpenFile(&fh, pPath, nn::fs::OpenMode_Read).IsSuccess())
		{
			int64_t fs;
			nn::fs::GetFileSize(&fs, fh);
			nn::fs::CloseFile(fh);

			return (size_type)fs;
		}
		return kSizeTypeError;
	#elif EAIO_USE_UNIX_IO // Special case for Unix because Unix paths aren't necessarily UTF8.
		#if defined(EA_HAVE_SYS_STAT_H) 
			// Some platforms (especially mobile) provide an alternative read-only file system
			// embedded within the executable application/package. We have standardized on the 
			// convention that paths to this file system are prefixed with "appbundle:/"
			#if defined(APPBUNDLE_PATH_PREFIX)
				if(strstr(pPath, APPBUNDLE_PATH_PREFIX) == pPath) // If the path begins with the APPBUNDLE_PATH_PREFIX prefix..
				{
					FileStream fileStream(pPath);

					if(fileStream.Open(kAccessFlagRead))
						return fileStream.GetSize();
					return kSizeTypeError;
				}
			#endif

			struct stat tempStat;
			const int result = stat(pPath, &tempStat);
			if(result == 0)
				return (size_type)tempStat.st_size;
			return kSizeTypeError;
		#else
			return kSizeTypeError;
		#endif
	#else
		using namespace EA::IO::Path;

		PathString16 path16;
		ConvertPath(path16, pPath);

		return File::GetSize(path16.c_str());
	#endif
}



////////////////////////////////////////////////////////////////////////////
// File::GetAttributes
//
EAIO_API int File::GetAttributes(const char32_t* pPath)
{
	Path::PathString8 path8;
	ConvertPath(path8, pPath);
	return File::GetAttributes(path8.c_str());
}

///////////////////////////////////////////////////////////////////////////////
// File::GetAttributes
//
EAIO_API int File::GetAttributes(const char16_t* pPath)
{
	int nAttributes = 0;

	#if defined(EA_PLATFORM_MICROSOFT)

		WIN32_FILE_ATTRIBUTE_DATA fileAttr; 
		if(GetFileAttributesExW(reinterpret_cast<LPCWSTR>(pPath), GetFileExInfoStandard, &fileAttr) == FALSE)
			return 0;

		const DWORD dwCurrentAttributes = fileAttr.dwFileAttributes;
		if(dwCurrentAttributes == INVALID_FILE_ATTRIBUTES)
			return 0;

		// Under Win32, all files effectively have the 'readable' flag set.
		// As such, we do nothing here as this test can never fail.
		nAttributes |= kAttributeReadable;

		// Win32 doesn't have a Unix-like writability mask, 
		// but instead has a read-only mask.
		if((dwCurrentAttributes & FILE_ATTRIBUTE_READONLY) == 0)
			nAttributes |= kAttributeWritable;

		// Win32 doesn't have a Unix-like executable mask. As such, all
		// files effectively have the 'executable' flag set.
		nAttributes |= kAttributeExecutable;

		if(dwCurrentAttributes & FILE_ATTRIBUTE_DIRECTORY)
			nAttributes |= kAttributeDirectory;

		if(dwCurrentAttributes & FILE_ATTRIBUTE_ARCHIVE)
			nAttributes |= kAttributeArchive;

		if(dwCurrentAttributes & FILE_ATTRIBUTE_HIDDEN)
			nAttributes |= kAttributeHidden;

		if(dwCurrentAttributes & FILE_ATTRIBUTE_SYSTEM)
			nAttributes |= kAttributeSystem;
	
	#elif EAIO_USE_UNIX_IO

		if(pPath)
		{
			Path::PathString8 path8;
			ConvertPath(path8, pPath);

			return File::GetAttributes(path8.c_str());
		}
	#elif defined(EA_PLATFORM_SONY)

		if (pPath)
		{
			char path8[kMaxPathLength];
			size_t n = EA::StdC::Strlcpy(path8, pPath, kMaxPathLength);

			if (n < (size_t)kMaxPathLength)
			{
				SceKernelStat st;
				if (sceKernelStat(path8, &st) == SCE_OK)
				{
					if(st.st_mode & SCE_KERNEL_S_IRUSR)
						nAttributes |= kAttributeReadable;
					if(st.st_mode & SCE_KERNEL_S_IWUSR)
						nAttributes |= kAttributeWritable;
					if(st.st_mode & SCE_KERNEL_S_IXUSR)
						nAttributes |= kAttributeExecutable;
					if(SCE_KERNEL_S_ISDIR(st.st_mode))
						nAttributes |= kAttributeDirectory;
				}
			}
		}
		
	#else
		EA_FAIL_M("Unimplemented");

		// Bug Paul Pedriana to finish this for the given platform.
		EA_UNUSED(pPath);

	#endif

	return nAttributes;
}

EAIO_API int File::GetAttributes(const char* pPath)
{
	#if defined(EA_PLATFORM_NX)
		int nAttributes = 0;
		
		// We use nn::fs::GetEntryType() and not stat() here because it is more reliable on NX
		nn::fs::DirectoryEntryType dt;		
		if (::IsPathValid(pPath) && 
			nn::fs::GetEntryType(&dt, pPath).IsSuccess())
		{
			// Per the NX documentation, when calling stat on NX, the st_mode is fixed to 0666
			// so the following values are hard coded
			nAttributes = kAttributeWritable | kAttributeReadable;
			if (dt == nn::fs::DirectoryEntryType_Directory)
				nAttributes |= kAttributeDirectory;
		}
		return nAttributes;
	#elif EAIO_USE_UNIX_IO // Special case for Unix because Unix paths aren't necessarily UTF8.
		#if defined(EA_HAVE_SYS_STAT_H)
			int nAttributes = 0;

			// Some platforms (especially mobile) provide an alternative read-only file system
			// embedded within the executable application/package. We have standardized on the 
			// convention that paths to this file system are prefixed with "appbundle:/"
			#if defined(APPBUNDLE_PATH_PREFIX)
				if(strstr(pPath, APPBUNDLE_PATH_PREFIX) == pPath) // If the path begins with the APPBUNDLE_PATH_PREFIX prefix..
				{
					nAttributes |= kAttributeReadable;

					if(Directory::Exists(pPath))
						nAttributes |= kAttributeDirectory;

					return nAttributes;
				}
			#endif

			struct stat tempStat;
			const int result = stat(pPath, &tempStat);
			if(result == 0)
			{
				if(tempStat.st_mode & S_IRUSR)
					nAttributes |= kAttributeReadable;
				if(tempStat.st_mode & S_IWUSR)
					nAttributes |= kAttributeWritable;
				if(tempStat.st_mode & S_IXUSR)
					nAttributes |= kAttributeExecutable;

				if((tempStat.st_mode & S_IFDIR) == S_IFDIR)
					nAttributes |= kAttributeDirectory;
				if((tempStat.st_mode & S_IFLNK) == S_IFLNK)
					nAttributes |= kAttributeAlias;
			}

			return nAttributes;
		#else
			return 0;
		#endif
	#else
		using namespace EA::IO::Path;

		PathString16 path16;
		ConvertPath(path16, pPath);

		return File::GetAttributes(path16.c_str());
	#endif
}




////////////////////////////////////////////////////////////////////////////
// File::SetAttributes
//
EAIO_API bool File::SetAttributes(const char32_t* pPath, int nAttributeMask, bool bEnable)
{
	Path::PathString8 path8;
	ConvertPath(path8, pPath);
	return File::SetAttributes(path8.c_str(), nAttributeMask, bEnable);
}

///////////////////////////////////////////////////////////////////////////////
// File::SetAttributes
//
EAIO_API bool File::SetAttributes(const char16_t* pPath, int nAttributeMask, bool bEnable)
{
	if(!pPath || !pPath[0])
		return false;

	#if defined(EA_PLATFORM_MICROSOFT)

		WIN32_FILE_ATTRIBUTE_DATA fileAttr; 
		if(GetFileAttributesExW(reinterpret_cast<LPCWSTR>(pPath), GetFileExInfoStandard, &fileAttr) == FALSE)
			return 0;

		const DWORD dwCurrentAttributes = fileAttr.dwFileAttributes;

		#ifndef INVALID_FILE_ATTRIBUTES // Windows SDK defines this, XBox SDK does not.
			#define INVALID_FILE_ATTRIBUTES ((DWORD)-1)
		#endif

		if(dwCurrentAttributes != INVALID_FILE_ATTRIBUTES)
		{
			DWORD dwNewAttributes = dwCurrentAttributes;

			if(nAttributeMask & kAttributeReadable){
				// Under Win32, all files effectively have the 'readable' flag set.
				// If the caller tries to set this flag, we do nothing.
			}

			if(nAttributeMask & kAttributeWritable){
				// Win32 doesn't have a Unix-like writability mask, but instead has a read-only mask.
				if(bEnable)
					dwNewAttributes &= ~FILE_ATTRIBUTE_READONLY; // Turn off this flag.
				else
					dwNewAttributes |=  FILE_ATTRIBUTE_READONLY; // Turn on this flag.
			}

			if(nAttributeMask & kAttributeExecutable){
				// Win32 doesn't have a Unix-like executable mask. As such, all
				// files effectively have the 'executable' flag set. If the
				// caller tries to set this flag, we do nothing.
			}

			if(nAttributeMask & kAttributeDirectory){
				// You cannot set this attribute. Perhaps some unusual OS will allow you
				// to do this, but we always disallow it, for consistency.
				// EA_FAIL_MESSAGE("SetFileAttributes: kAttributeDirectory can't be altered.\n\n");
				return false;
			}

			if(nAttributeMask & kAttributeAlias){
				// You cannot set this attribute. Perhaps some unusual OS will allow 
				// you to do this, but we always disallow it, for consistency.
				// EA_FAIL_MESSAGE("SetFileAttributes: kAttributeAlias can't be altered.\n\n");
				return false;
			}

			if(nAttributeMask & kAttributeArchive){
				if(bEnable)
					dwNewAttributes |=  FILE_ATTRIBUTE_ARCHIVE;
				else
					dwNewAttributes &= ~FILE_ATTRIBUTE_ARCHIVE;
			}

			if(nAttributeMask & kAttributeHidden){
				if(bEnable)
					dwNewAttributes |=  FILE_ATTRIBUTE_HIDDEN;
				else
					dwNewAttributes &= ~FILE_ATTRIBUTE_HIDDEN;
			}

			if(nAttributeMask & kAttributeSystem){
				if(bEnable)
					dwNewAttributes |=  FILE_ATTRIBUTE_SYSTEM;
				else
					dwNewAttributes &= ~FILE_ATTRIBUTE_SYSTEM;
			}

			if(dwNewAttributes == dwCurrentAttributes)
				return true;

			return (SetFileAttributesW(reinterpret_cast<LPCWSTR>(pPath), dwNewAttributes) != 0);
		}

		return false;
		
	#elif EAIO_USE_UNIX_IO

		Path::PathString8 path8;
		ConvertPath(path8, pPath);

		return File::SetAttributes(path8.c_str(), nAttributeMask, bEnable);

	#elif defined(EA_PLATFORM_SONY)

		char path8[kMaxPathLength];
		size_t n = EA::StdC::Strlcpy(path8, pPath, kMaxPathLength);

		if (n < (size_t)kMaxPathLength)
		{
			SceKernelMode modeChange = 0;

			if (nAttributeMask & kAttributeReadable)
				modeChange |= SCE_KERNEL_S_IRUSR;
			if (nAttributeMask & kAttributeWritable)
				modeChange |= SCE_KERNEL_S_IWUSR;
			if (nAttributeMask & kAttributeExecutable)
				modeChange |= SCE_KERNEL_S_IXUSR;

			if (modeChange == 0)
				return true;

			SceKernelStat st;
			if (sceKernelStat(path8, &st) != SCE_OK)
				return false;

			SceKernelMode newMode;
			if (bEnable)
				newMode = st.st_mode | modeChange;
			else
				newMode = st.st_mode & ~modeChange;

			return sceKernelChmod(path8, newMode) == SCE_OK;
		}

		return false;

	#else
		EA_FAIL_M("Unimplemented");

		// Bug Paul Pedriana to finish this for the given platform.
		EA_UNUSED(pPath); EA_UNUSED(nAttributeMask); EA_UNUSED(bEnable);
			
		return false;

	#endif
}

EAIO_API bool File::SetAttributes(const char* pPath, int nAttributeMask, bool bEnable)
{
	#if EAIO_USE_UNIX_IO // Special case for Unix because Unix paths aren't necessarily UTF8.
		#if defined(EA_HAVE_SYS_STAT_H) && !defined(EA_PLATFORM_NX)
			// To consider: Write code to handle APPBUNDLE_PATH_PREFIX. It might be pointless to do
			// so because the return value is always false, which the code below will already produce.
			if(pPath && pPath[0]) // Some Unixes (e.g. OSX) stat will return 0 (success) on an empty path, so we add this check.
			{
				struct stat tempStat;
				if (stat(pPath, &tempStat) != 0)
					return false;

				mode_t modeChange = 0;

				if (nAttributeMask & kAttributeReadable)
					modeChange |= S_IRUSR;
				if (nAttributeMask & kAttributeWritable)
					modeChange |= S_IWUSR;
				if (nAttributeMask & kAttributeExecutable)
					modeChange |= S_IXUSR;

				if (modeChange == 0)
					return true;

				mode_t newMode;
				if (bEnable)
					newMode = tempStat.st_mode | modeChange;
				else
					newMode = tempStat.st_mode & ~modeChange;

				if (chmod(pPath, newMode) == 0)
					return true;
			}

			return false;
		#else
			return false;
		#endif
	#else
		using namespace EA::IO::Path;

		PathString16 path16;
		ConvertPath(path16, pPath);

		return File::SetAttributes(path16.c_str(), nAttributeMask, bEnable);
	#endif
}

////////////////////////////////////////////////////////////////////////////
// File::GetTime
//
EAIO_API time_t File::GetTime(const char32_t* pPath, FileTimeType timeType)
{
	Path::PathString8 path8;
	ConvertPath(path8, pPath);
	return File::GetTime(path8.c_str(), timeType);
}

///////////////////////////////////////////////////////////////////////////////
// File::GetTime
//
// Microsoft provides a stat/_wstat function for getting the time of a file, 
// but their implementation is non-conforming. In particular the Microsoft stat 
// function implementations convert file time to local time:
//      http://support.microsoft.com/kb/190315.
// Unix-based stat usage is OK, as Unix stat doesn't adjust time values and 
// instead uses UTC values without modification.
//
EAIO_API time_t File::GetTime(const char16_t* pPath, FileTimeType timeType)
{
	#if defined(EA_PLATFORM_MICROSOFT)

		time_t returnValue = 0;
		HANDLE hFindFile = INVALID_HANDLE_VALUE;

		WIN32_FIND_DATAW win32FindData;
		#if EA_WINAPI_FAMILY_PARTITION(EA_WINAPI_PARTITION_DESKTOP)
			hFindFile = FindFirstFileW(reinterpret_cast<LPCWSTR>(pPath), &win32FindData);
		#else
			hFindFile = FindFirstFileExW(reinterpret_cast<LPCWSTR>(pPath), FindExInfoBasic, &win32FindData, FindExSearchNameMatch, NULL, 0);
		#endif

		if(hFindFile != INVALID_HANDLE_VALUE)
		{
			if(timeType == kFileTimeTypeCreation)
				Internal::FILETIMEToTimeT(win32FindData.ftCreationTime, returnValue); // Some implementations of stat define ctime to be something other than creation time.
			else if(timeType == kFileTimeTypeLastModification)
				Internal::FILETIMEToTimeT(win32FindData.ftLastWriteTime, returnValue);
			else if(timeType == kFileTimeTypeLastAccess)
				Internal::FILETIMEToTimeT(win32FindData.ftLastAccessTime, returnValue);

			FindClose(hFindFile);
		}

		return returnValue;

	#elif defined(EA_PLATFORM_NX) // There is no way to get file time on NX (other than a no-ship, debug only function)

		return 0;

	#elif EAIO_USE_UNIX_IO

		Path::PathString8 path8;
		ConvertPath(path8, pPath);
		return File::GetTime(path8.c_str(), timeType);

	#elif defined(EA_PLATFORM_SONY)

		char path8[kMaxPathLength];
		size_t n = EA::StdC::Strlcpy(path8, pPath, kMaxPathLength);

		if (n < (size_t)kMaxPathLength)
		{
			SceKernelStat st;

			if(sceKernelStat(path8, &st) == SCE_OK)
			{
				if (timeType == kFileTimeTypeCreation)
					return st.st_ctim.tv_sec;
				else if (timeType == kFileTimeTypeLastModification)
					return st.st_mtim.tv_sec;
				else if (timeType == kFileTimeTypeLastAccess)
					return st.st_atim.tv_sec;
			}
		}

		return 0;

	#else
		EA_FAIL_M("Unimplemented");

		// No way to get the file time for the given platform.
		// Or possibly the platform isn't supported yet.
		EA_UNUSED(pPath); EA_UNUSED(timeType);
		return 0;

	#endif
}

EAIO_API time_t File::GetTime(const char* pPath, FileTimeType timeType)
{
	// Special case for Unix because Unix paths aren't necessarily UTF8.
	#if defined(EA_PLATFORM_NX) // There is no way to get file time on NX (other than a no-ship, debug only function)
		return 0;
	#elif EAIO_USE_UNIX_IO
		#if defined(EA_HAVE_SYS_STAT_H)
			// Some platforms (especially mobile) provide an alternative read-only file system
			// embedded within the executable application/package. We have standardized on the 
			// convention that paths to this file system are prefixed with "appbundle:/"

			#if defined(EA_PLATFORM_ANDROID)
				if(strstr(pPath, APPBUNDLE_PATH_PREFIX) == pPath) // If the path begins with the APPBUNDLE_PATH_PREFIX prefix..
				{
					AutoJNIEnv autoEnv;
					EA::IO::Path::PathString8 pathString = pPath + APPBUNDLE_PATH_PREFIX_LENGTH;
					EA::IO::Path::Simplify(pathString);
					bool result = AssetManagerJNI::Exists(autoEnv, pathString.c_str());

					return (result ? 1319738365 : 0);  // 1319738365 is October 27, 2011. To do: Have this return the application executable time.
				}
			#endif

			struct stat tempStat;
			const int result = stat(pPath, &tempStat);

			if(result == 0)
			{
				if(timeType == kFileTimeTypeCreation)
					return tempStat.st_ctime;
				else if(timeType == kFileTimeTypeLastModification)
					return tempStat.st_mtime;
				else if(timeType == kFileTimeTypeLastAccess)
					return tempStat.st_atime;
			}

			return 0;
		#else
			return 0;
		#endif

	#elif defined(EA_PLATFORM_SONY)
		SceKernelStat st;

		if(sceKernelStat(pPath, &st) == SCE_OK)
		{
			if(timeType == kFileTimeTypeCreation)
				return st.st_ctim.tv_sec;
			else if (timeType == kFileTimeTypeLastModification)
				return st.st_mtim.tv_sec;
			else if (timeType == kFileTimeTypeLastAccess)
				return st.st_atim.tv_sec;
		}

		return 0;
		
	#else
		using namespace EA::IO::Path;

		PathString16 path16;
		ConvertPath(path16, pPath);

		return File::GetTime(path16.c_str(), timeType);
	#endif
}


////////////////////////////////////////////////////////////////////////////
// File::SetTime
//
EAIO_API bool File::SetTime(const char32_t* pPath, int nFileTimeTypeFlags, time_t nTime)
{
	Path::PathString8 path8;
	ConvertPath(path8, pPath);
	return File::SetTime(path8.c_str(), nFileTimeTypeFlags, nTime);
}

///////////////////////////////////////////////////////////////////////////////
// File::SetTime
//
EAIO_API bool File::SetTime(const char16_t* pPath, int nFileTimeTypeFlags, time_t nTime)
{
	using namespace EA::IO::Path;

	#if defined(EA_PLATFORM_MICROSOFT) || (EAIO_USE_UNIX_IO && EAIO_UTIME_PRESENT)

		#if defined(EA_PLATFORM_MICROSOFT)
			_utimbuf timbuf;
		#elif EAIO_USE_UNIX_IO
			 utimbuf timbuf;
		#endif
	
		// Not supported by most platforms:
		//if(nFileTimeTypeFlags & kFileTimeTypeCreation)
		//    timbuf.modtime = nTime;
		//else
		//    timbuf.modtime = GetFileTime(pPath, kFileTimeTypeCreation);

		if(nFileTimeTypeFlags & kFileTimeTypeLastModification)
			timbuf.modtime = nTime;
		else
			timbuf.modtime = File::GetTime(pPath, kFileTimeTypeLastModification);
		if(nFileTimeTypeFlags & kFileTimeTypeLastAccess)
			timbuf.actime  = nTime;
		else
			timbuf.actime = File::GetTime(pPath, kFileTimeTypeLastAccess);

		//We convert here and not above to avoid the extra conversion 
		//calls associated with 8 bit chars in FileUtil
		PathString8 path8;
		ConvertPath(path8, pPath);

		#if defined(EA_PLATFORM_MICROSOFT)
			return (_utime(path8.c_str(), &timbuf) == 0);
		#elif EAIO_USE_UNIX_IO
			return (utime(path8.c_str(), &timbuf) == 0);
		#endif

	#elif defined(EA_PLATFORM_SONY)

		timeval timbuf[2];

		// timebuf[0] is access time
		// timebuf[1] is modification/creation time (creation time will be set only if older than the value set)
		timbuf[0].tv_usec = 0;
		timbuf[1].tv_usec = 0;

		if (nFileTimeTypeFlags & kFileTimeTypeLastModification)
			timbuf[1].tv_sec = nTime;
		else
			timbuf[1].tv_sec = File::GetTime(pPath, kFileTimeTypeLastModification);

		if (nFileTimeTypeFlags & kFileTimeTypeLastAccess)
			timbuf[0].tv_sec  = nTime;
		else
			timbuf[0].tv_sec = File::GetTime(pPath, kFileTimeTypeLastAccess);

		// We convert here and not above to avoid the extra conversion 
		// calls associated with 8 bit chars in GetTime
		char path8[kMaxPathLength];
		size_t n = EA::StdC::Strlcpy(path8, pPath, kMaxPathLength);

		if (n < (size_t)kMaxPathLength)
		{
			return sceKernelUtimes(path8, timbuf) == SCE_OK;
		}
		return false;

	#else

		// Other platforms are not known to support this functionality.
		EA_UNUSED(pPath); EA_UNUSED(nFileTimeTypeFlags); EA_UNUSED(nTime);
		return false;
	#endif
}

EAIO_API bool File::SetTime(const char* pPath, int nFileTimeTypeFlags, time_t nTime)
{
	#if EAIO_USE_UNIX_IO && EAIO_UTIME_PRESENT // Special case for Unix because Unix paths aren't necessarily UTF8.
		utimbuf timbuf;
	
		if(nFileTimeTypeFlags & kFileTimeTypeLastModification)
			timbuf.modtime = nTime;
		else
			timbuf.modtime = File::GetTime(pPath, kFileTimeTypeLastModification);
		if(nFileTimeTypeFlags & kFileTimeTypeLastAccess)
			timbuf.actime  = nTime;
		else
			timbuf.actime = File::GetTime(pPath, kFileTimeTypeLastAccess);

		return (utime(pPath, &timbuf) == 0);
	#else
		using namespace EA::IO::Path;

		PathString16 path16;
		ConvertPath(path16, pPath);

		return File::SetTime(path16.c_str(), nFileTimeTypeFlags, nTime);
	#endif
}




////////////////////////////////////////////////////////////////////////////
// File::ResolveAlias
//
EAIO_API File::ResolveAliasResult File::ResolveAlias(const char32_t* pPathSource, char32_t* pPathDestination, 
													 uint32_t nDestPathCapacity)
{
	Path::PathString16 pathSource16;
	ConvertPath(pathSource16, pPathSource);

	char16_t pPathDest16[EA::IO::kMaxPathLength];

	File::ResolveAliasResult result = File::ResolveAlias(pathSource16.c_str(), pPathDest16, EA::IO::kMaxPathLength);
	EA::StdC::Strlcpy(pPathDestination, pPathDest16, nDestPathCapacity, EA::StdC::Strlen(pPathDest16));

	return result;
}

///////////////////////////////////////////////////////////////////////////////
// File::ResolveAlias
//
EAIO_API File::ResolveAliasResult File::ResolveAlias(const char16_t* pPathSource, char16_t* pPathDestination, 
													 uint32_t nDestPathCapacity)
{
	ResolveAliasResult result = kRARInvalid;

	if(File::Exists(pPathSource))
	{
		result = kRARNone;

		#if EA_WINAPI_FAMILY_PARTITION(EA_WINAPI_PARTITION_DESKTOP)
			// Try to ignore these ugly typedefs. They come right from Microsoft's 'objbase.h' header.
			typedef HRESULT(STDAPICALLTYPE *CoInitializeFunctionType)(LPVOID);
			typedef void   (STDAPICALLTYPE *CoUninitializeFunctionType)(void);
			typedef HRESULT(STDAPICALLTYPE *CoCreateInstanceFunctionType)(REFCLSID, LPUNKNOWN, DWORD, REFIID, LPVOID FAR*);

			const size_t nStrlen = EA::StdC::Strlen(pPathSource);

			if(nStrlen > 0)
			{
				size_t posExt = nStrlen - 1;

				while(posExt > 0)
				{
					if(pPathSource[posExt] == '.')
					{
						if(!StrEq(pPathSource + posExt, EA_CHAR16(".lnk")))
							posExt = 0;
						break;
					}
					posExt--;
				}

				if(posExt > 0) // If the file name ends with ".lnk"...
				{
					WIN32_FIND_DATAW fd;
					IPersistFile*    pPersistFile = NULL;
					IShellLinkW*     pShellLink = NULL;
					HRESULT          hResult;
					WCHAR            szPathW[kMaxPathLength + 1];

					fd.cFileName[0] = 0;
					szPathW[0]      = 0;

					HINSTANCE hOle32Library = LoadLibraryA("Ole32.dll");

					if(hOle32Library)
					{
						CoInitializeFunctionType     CoInitializeFunction     = (CoInitializeFunctionType)    (void*)::GetProcAddress(hOle32Library, "CoInitialize");
						CoUninitializeFunctionType   CoUninitializeFunction   = (CoUninitializeFunctionType)  (void*)::GetProcAddress(hOle32Library, "CoUninitialize");
						CoCreateInstanceFunctionType CoCreateInstanceFunction = (CoCreateInstanceFunctionType)(void*)::GetProcAddress(hOle32Library, "CoCreateInstance");

						// Initialize COM
						if(CoInitializeFunction && CoUninitializeFunction && CoCreateInstanceFunction)
						{
							hResult = CoInitializeFunction(NULL);
							if(SUCCEEDED(hResult))
							{
								// Create an instance of shell link interface
								hResult = CoCreateInstanceFunction(CLSID_ShellLink, NULL, CLSCTX_INPROC_SERVER, IID_IShellLinkW, (void**)&pShellLink);
								if(SUCCEEDED(hResult))
								{
									// Get the persist file interface
									hResult = pShellLink->QueryInterface(IID_IPersistFile,(void**)&pPersistFile);
									if(SUCCEEDED(hResult))
									{
										// Load the persist file with the link
										hResult = pPersistFile->Load((LPCWSTR)pPathSource, 0);
										if(SUCCEEDED(hResult))
										{
											// Resolve the link
											hResult = pShellLink->Resolve(NULL, SLR_NO_UI);
											if(SUCCEEDED(hResult))
											{
												// Get the path from the link
												hResult = pShellLink->GetPath(szPathW, sizeof(szPathW)/sizeof(szPathW[0]), &fd, 0);
												if(hResult == S_OK)
												{
													// Copy to destination
													if(EA::StdC::Strlen(szPathW) < kMaxPathLength)
													{
														EA::StdC::Strlcpy(pPathDestination, (char16_t*)szPathW, nDestPathCapacity);
														result = kRARAlias;
													}
												}
											}
										}
										pPersistFile->Release();
										pPersistFile = NULL;
									}
									pShellLink->Release();
									pShellLink = NULL;
								}
								CoUninitializeFunction();
							}
						}
						::FreeLibrary(hOle32Library);
					}
				}
			}
		#endif // EA_PLATFORM_WINDOWS
	}

	if(result != kRARAlias) // If an alias was not found... copy the source to the dest.
	{
		if(pPathDestination && (pPathSource != pPathDestination)) // If the user provided a dest and it wasn't the same memory as the source...
			EA::StdC::Strlcpy(pPathDestination, pPathSource, nDestPathCapacity);
	}
	
	return result;
}

EAIO_API File::ResolveAliasResult File::ResolveAlias(const char* pPathSource, char* pPathDestination,
													 uint32_t nDestCapacity)
{
	using namespace EA::IO::Path;

	// This function currently doesn't have a pathway for non-UTF8 char strings.
	// As it currently stands, that doesn't matter much because the implementation
	// currently is implemented only for Windows, with which we require all file 
	// paths to be Unicode.

	// We do back and forth conversions, but Windows applications (which would be using this 
	// function the most) are often going to be using 16 bit strings for UI and file systems anyway.

	PathString16 pathSource16;
	ConvertPath(pathSource16, pPathSource);

	char16_t pDirectory16[kMaxDirectoryLength];
	const File::ResolveAliasResult result = File::ResolveAlias(&pathSource16[0], pDirectory16, kMaxDirectoryLength);

	EA::StdC::Strlcpy(pPathDestination, pDirectory16, nDestCapacity);

	return result;
}



////////////////////////////////////////////////////////////////////////////
// File::CreateAlias
//
EAIO_API bool File::CreateAlias(const char32_t* pDestinationPath, const char32_t* pShortcutPath, 
								const char32_t* pShortcutDescription, const char32_t* pShortcutArguments)
{
	Path::PathString8 pathDest8, pathShortcut8, pathDescription8, pathArgs8;
	ConvertPath(pathDest8,        pDestinationPath);
	ConvertPath(pathShortcut8,    pShortcutPath);
	ConvertPath(pathDescription8, pShortcutDescription);
	ConvertPath(pathArgs8,        pShortcutArguments);

	return File::CreateAlias(pathDest8.c_str(), pathShortcut8.c_str(), pathDescription8.c_str(), pathArgs8.c_str());
}

///////////////////////////////////////////////////////////////////////////////
// File::CreateAlias
//
EAIO_API bool File::CreateAlias(const char16_t* pDestinationPath, const char16_t* pShortcutPath, 
								const char16_t* pShortcutDescription, const char16_t* pShortcutArguments)
{
	using namespace EA::IO::Path;

	if(pDestinationPath[0])
	{
		#if EA_WINAPI_FAMILY_PARTITION(EA_WINAPI_PARTITION_DESKTOP)

			HRESULT      hResult;
			IShellLinkW* pShellLink;
			
			hResult = ::CoInitialize(NULL); // Or ::CoInitializeEx(NULL, COINIT_MULTITHREADED);
	 
			if(SUCCEEDED(hResult))
			{
				// Get a pointer to the IShellLink interface.
				hResult = ::CoCreateInstance(CLSID_ShellLink, NULL, CLSCTX_INPROC_SERVER, IID_IShellLinkW, (VOID**)&pShellLink);

				if(SUCCEEDED(hResult))
				{
					// Set the shortcut parameters
					pShellLink->SetPath(reinterpret_cast<LPCWSTR>(pDestinationPath));

					if(pShortcutDescription && pShortcutDescription[0])
						pShellLink->SetDescription(reinterpret_cast<LPCWSTR>(pShortcutDescription));

					if(pShortcutArguments && pShortcutArguments[0])
						pShellLink->SetArguments(reinterpret_cast<LPCWSTR>(pShortcutArguments));

					// Query IShellLink for the IPersistFile interface for saving the
					// shortcut in persistent storage.
					IPersistFile* pPersistFile; 
					hResult = pShellLink->QueryInterface(IID_IPersistFile, (VOID**)&pPersistFile);
			   
					if(SUCCEEDED(hResult))
					{
						PathString16 path16(pShortcutPath);

						// Make sure the shortcut ends with .lnk. It won't work unless it has this extension.
						if(!StrEq(Path::GetFileExtension(path16), EA_CHAR16(".lnk")))
							path16 += EA_CHAR16(".lnk");

						// Remove the file if it exists.
						File::Remove(path16.c_str());

						// Save the link by calling IPersistFile::Save.
						hResult = pPersistFile->Save(reinterpret_cast<LPCOLESTR>(path16.c_str()), TRUE);
						pPersistFile->Release();
					}

					pShellLink->Release();
				}

				::CoUninitialize();
			}

			return SUCCEEDED(hResult);

		#else
			EA_UNUSED(pDestinationPath);
			EA_UNUSED(pShortcutPath);
			EA_UNUSED(pShortcutDescription);
			EA_UNUSED(pShortcutArguments);
		#endif
	}

	return false;
}

EAIO_API bool File::CreateAlias(const char* pDestinationPath,
								const char* pShortcutPath, 
								const char* pShortcutDescription,
								const char* pShortcutArguments)
{
	// This function currently doesn't have a pathway for non-UTF8 char strings.
	// As it currently stands, that doesn't matter much because the implementation
	// currently is implemented only for Windows, with which we require all file 
	// paths to be Unicode.

	using namespace EA::IO::Path;

	PathString16 pathDest16;
	PathString16 pathShortcut16;
	PathString16 shortcutDescription16;
	PathString16 shortcutArguments16;

	ConvertPath(pathDest16,            pDestinationPath);
	ConvertPath(pathShortcut16,        pShortcutPath);
	ConvertPath(shortcutDescription16, pShortcutDescription);
	ConvertPath(shortcutArguments16,   pShortcutArguments);

	return File::CreateAlias(pathDest16.c_str(),
							 pathShortcut16.c_str(), 
							 shortcutDescription16.c_str(),
							 shortcutArguments16.c_str());
}



////////////////////////////////////////////////////////////////////////////
// File::IsVolumeAvailable
//
EAIO_API bool IsVolumeAvailable(const char32_t* pPath, int timeoutMS)
{
	Path::PathString8 path8;
	ConvertPath(path8, pPath);
	return IsVolumeAvailable(path8.c_str(), timeoutMS);
}

EAIO_API bool IsVolumeAvailable(const char16_t* pPath, int timeoutMS)
{
	Path::PathString8 path8;
	ConvertPath(path8, pPath);
	return IsVolumeAvailable(path8.c_str(), timeoutMS);
}

EAIO_API bool IsVolumeAvailable(const char* pPath, int timeoutMS)
{
	// Complete this for other platforms as required.
	EA_UNUSED(pPath); EA_UNUSED(timeoutMS);
	return true;
}

///////////////////////////////////////////////////////////////////////////////
// InitializeFileSystem
//
EAIO_API bool InitializeFileSystem(bool)
{
	return true;
}

///////////////////////////////////////////////////////////////////////////////
// MakeTempPathName
//
EAIO_API bool MakeTempPathName(char32_t* pPath, const char32_t* pDirectory, const char32_t* pFileName, const char32_t* pExtension, uint32_t nDestPathCapacity)
{
	char path8[kMaxPathLength];      path8[0] = 0;
	char directory8[kMaxPathLength]; directory8[0] = 0;
	char fileName8[kMaxPathLength];  fileName8[0] = 0;
	char extension8[kMaxPathLength]; extension8[0] = 0;

	if(pDirectory)
		EA::StdC::Strlcpy(directory8, pDirectory, kMaxPathLength);
	if(pFileName)
		EA::StdC::Strlcpy(fileName8, pFileName, kMaxPathLength);
	if(pExtension)
		EA::StdC::Strlcpy(extension8, pExtension, kMaxPathLength);

	bool result = MakeTempPathName(path8, pDirectory ? directory8 : NULL, 
										  pFileName  ? fileName8  : NULL, 
										  pExtension ? extension8 : NULL, 
								   kMaxPathLength);

	if(result) // If succeeded...
	{
		int strlcpyResult = EA::StdC::Strlcpy(pPath, path8, nDestPathCapacity); // Copy to the user-supplied path.

		if((strlcpyResult > 0) && (strlcpyResult < (int)nDestPathCapacity)) // If there wasn't an error and if there was enough space...
			return true;
	}

	return false;
}

EAIO_API bool MakeTempPathName(char16_t* pPath, const char16_t* pDirectory, const char16_t* pFileName, const char16_t* pExtension, uint32_t nDestPathCapacity)
{
	#if EAIO_MKSTEMPS_AVAILABLE
		// We do character arrays instead of string literals because string literals won't be possible if wchar_t != char16_t.
		const char16_t tmp[]    = { '/', 't', 'm', 'p', '/', 0 };
		const char16_t temp[]   = { 't', 'm', 'p', 0 };
		const char16_t XXXXXX[] = { 'X', 'X', 'X', 'X', 'X', 'X', 0}; // Some mkstemps implementations (e.g. Linux) require that there be six Xs and no less, while others don't (BSD).
		const char16_t empty[]  = { 0 };

		if(!pDirectory)
			pDirectory = tmp;

		if(!pFileName)
			pFileName = temp;
		
		if(!pExtension)
			pExtension = empty;
		
		char path8[kMaxPathLength];

		size_t  nLength = EA::StdC::Strlcpy(path8,           pDirectory,         kMaxPathLength,           EA::StdC::Strlen(pDirectory));
		nLength +=        EA::StdC::Strlcpy(path8 + nLength, pFileName,          kMaxPathLength - nLength, EA::StdC::Strlen(pFileName));
		nLength +=        EA::StdC::Strlcpy(path8 + nLength, XXXXXX,             kMaxPathLength - nLength, EA::StdC::Strlen(XXXXXX));
		nLength +=        EA::StdC::Strlcpy(path8 + nLength, pExtension,         kMaxPathLength - nLength, EA::StdC::Strlen(pExtension));
		
		// We have a path8 like so: /UserSpecifiedDirectory/UserSpecifiedFileNameXXXXXX.userSpecfiedExtension
		int fd = mkstemps(path8, (int)EA::StdC::Strlen(pExtension));
		if(fd == -1)
			return false;
		close(fd);
		
		EA::StdC::Strlcpy(pPath, path8, nDestPathCapacity);
		return true;

	#else

		using namespace EA::IO::Path;
		char path8[kMaxPathLength];
		char directory8[kMaxPathLength];
		char fileName8[kMaxPathLength];
		char extension8[kMaxPathLength];
		char *pFileName8 = NULL, *pDirectory8 = NULL, *pExtension8 = NULL;

		if (pDirectory != NULL)
		{
			EA::StdC::Strlcpy(directory8, pDirectory, sizeof directory8);
			pDirectory8 = directory8;
		}

		if (pFileName != NULL)
		{
			EA::StdC::Strlcpy(fileName8, pFileName, sizeof fileName8);
			pFileName8 = fileName8;
		}

		if (pExtension != NULL)
		{
			EA::StdC::Strlcpy(extension8, pExtension, sizeof extension8);
			pExtension8 = extension8;
		}

		bool result = MakeTempPathName(path8, pDirectory8, pFileName8, pExtension8, kMaxPathLength);
		if (result)
		{
			const int requiredLength = EA::StdC::Strlcpy(pPath, path8, nDestPathCapacity);
			if ((requiredLength < 0) || requiredLength > static_cast<int>(nDestPathCapacity))
				result = false;
		}

		return result;

	#endif
}


EAIO_API bool MakeTempPathName(char* pPath, const char* pDirectory, const char* pFileName, const char* pExtension, uint32_t nDestPathCapacity)
{
   #if EAIO_MKSTEMPS_AVAILABLE

		if(!pDirectory)
			pDirectory = "/tmp/";
		
		if(!pFileName)
			pFileName = "temp";
		
		if(!pExtension)
			pExtension = "";
		
		// Some mkstemps implementations (e.g. Linux) require that there be six Xs and no less, while others don't (BSD).
		if(EA::StdC::Snprintf(pPath, nDestPathCapacity, "%s%sXXXXXX%s", pDirectory, pFileName, pExtension) >= (int)nDestPathCapacity)
			return false;
	
		// Apple seems to be the only Unix variant we use which supports mkstemps. GCC under Linux supports only mkstemp as of GCC 4.3.
		int fd = mkstemps(pPath, (int)EA::StdC::Strlen(pExtension));
		if(fd == -1)
			return false;
		close(fd);
		
		return true;

	#else

		// User must allocate space for the resulting temp path.
		if(pPath)
		{
			static const char pFileNameDefault[]  = { 't', 'e', 'm', 'p', 0 };
			static const char pExtensionDefault[] = { '.', 't', 'm', 'p', 0 };

			const uint32_t nTime = (uint32_t)EAIOTime();
			uint32_t       nRand = EAIORand();
			char        tempPath[kMaxPathLength]; tempPath[0] = '\0';

			if(!pFileName)
				pFileName = pFileNameDefault;

			if(!pExtension)
				pExtension = pExtensionDefault;

			if(!pDirectory)
			{
				if(!GetTempDirectory(tempPath))
					return false;
				pDirectory = tempPath;
			}

			for(size_t i = 0; i < 100000; i++, nRand = EAIORand())
			{
				char buffer[20];

				Path::PathString8 tempFilePath(pDirectory);
				Path::Append(tempFilePath, pFileName);

				tempFilePath.operator+=(EA::StdC::U32toa(nTime ^ nRand, buffer, 10));
				tempFilePath.operator+=(pExtension);

				uint32_t nSrcPathLength = (uint32_t)tempFilePath.length();
				if(nSrcPathLength > nDestPathCapacity)
					break;

				EA::StdC::Strlcpy(pPath, tempFilePath.c_str(), nDestPathCapacity);

				FileStream fileStream(pPath);
				if(fileStream.Open(kAccessFlagReadWrite, kCDCreateNew))
				{
					fileStream.Close();
					return true;
				}
				else if(fileStream.GetState() == kFSErrorWriteProtect) // If the directory cannot be written to...
					break;
			}
		}

		return false;

	#endif
}



///////////////////////////////////////////////////////////////////////////////
// GetTempDirectory
//
EAIO_API int GetTempDirectory(char32_t* pDirectory, uint32_t nPathCapacity)
{
	char pPathDest8[EA::IO::kMaxPathLength]; pPathDest8[0] = '\0';

	int length = GetTempDirectory(pPathDest8, EA::IO::kMaxPathLength);

	if(length >= 0)
	{
		int newLength = EA::StdC::Strlcpy(pDirectory, pPathDest8, nPathCapacity, (size_t)length);
			
		if(newLength < (int)nPathCapacity)
			length = newLength;
		else
			length = -1;
	}

	return length;
}

///////////////////////////////////////////////////////////////////////////////
// GetTempDirectory
//
EAIO_API int GetTempDirectory(char16_t* pDirectory, uint32_t nPathCapacity)
{
	// Check to see if the user manually specified a temp directory.
	if(gpTempDirectory[0])
	{
		// To do: Call Directory::EnsureExists and check its return value.
		EA::StdC::Strlcpy(pDirectory, gpTempDirectory, nPathCapacity);
		return (int)EA::StdC::Strlen(pDirectory);
	}

	char directory8[kMaxDirectoryLength]; directory8[0] = '\0';
	int result = GetTempDirectory(directory8, kMaxDirectoryLength);
	if (result >= 0)
	{
		const int requiredLength = EA::StdC::Strlcpy(pDirectory, directory8, nPathCapacity);
		if ((requiredLength < 0) || (requiredLength > static_cast<int>(nPathCapacity)))
			result = -1;
	}

	return result;
}

EAIO_API int GetTempDirectory(char* pDirectory, uint32_t nPathCapacity)
{
	// Check to see if the user manually specified a temp directory.
	if(gpTempDirectory[0])
	{
		// To do: Call Directory::EnsureExists and check its return value.
		return (int)(unsigned)EA::StdC::Strlcpy(pDirectory, gpTempDirectory, nPathCapacity);
	}

	#if defined(EA_PLATFORM_MICROSOFT)

		#if EA_WINAPI_FAMILY_PARTITION(EA_WINAPI_PARTITION_DESKTOP)

			wchar_t pPathW[kMaxPathLength];
		
			DWORD dwResult = GetTempPathW(kMaxDirectoryLength, pPathW);
			if(dwResult) // If succeeded...
			{
				// According to the docs, these arguments can overlap like this.
				GetLongPathNameW(pPathW, pPathW, kMaxDirectoryLength);
				return (int)(unsigned)EA::StdC::Strlcpy(pDirectory, pPathW, nPathCapacity);
			}

			return -1;

		#elif defined(EA_PLATFORM_WINDOWS) && !EA_WINAPI_FAMILY_PARTITION(EA_WINAPI_PARTITION_DESKTOP)
			Platform::String^ tempfolder = Windows::Storage::ApplicationData::Current->TemporaryFolder->Path;
			EA::StdC::Strlcpy(pDirectory, tempfolder->Data(), nPathCapacity);
			uint32_t nLength = (uint32_t)EA::StdC::Strlcat(pDirectory, "\\", nPathCapacity); // must have trailing slash
			Directory::EnsureExists(pDirectory);

			return (int)(unsigned)nLength;
		#elif defined(EA_PLATFORM_XBOXONE) || defined(EA_PLATFORM_XBSX)
			const char* tempfolder = "T:";
			EA::StdC::Strlcpy(pDirectory, tempfolder, nPathCapacity);
			uint32_t nLength = (uint32_t)EA::StdC::Strlcat(pDirectory, "\\", nPathCapacity); // must have trailing slash
			Directory::EnsureExists(pDirectory);
			return (int)(unsigned)nLength;

		#elif defined(EA_PLATFORM_WINDOWS_PHONE)
			Platform::String^ tempfolder = Windows::Storage::ApplicationData::Current->LocalFolder->Path;
			EA::StdC::Strlcpy(pDirectory, tempfolder->Data(), nPathCapacity);
			uint32_t nLength = (uint32_t)EA::StdC::Strlcat(pDirectory, "\\", nPathCapacity); // must have trailing slash
			Directory::EnsureExists(pDirectory);
			return (int)(unsigned)nLength;

		#else
			uint32_t nLength = (uint32_t)EA::StdC::Strlcpy(pDirectory, "c:\\Windows\\Temp\\", nPathCapacity);
			Directory::EnsureExists(pDirectory);
			return (int)(unsigned)nLength;

		#endif

	#elif defined(EA_PLATFORM_SONY)
		// https://ps4.scedev.net/docs/ps4-en,AppContent-Overview-orbis,Using_the_Library/1#Procedure_to_Access_Temporary_Data
		// Sony has documented a /temp0/ mount point but states as of SDK 1.021 that it is not supported, though after talking
		// to them they say this is meant to refer to the Temporary Data mount mentioned above.
		//

		/*
		This is currently disabled while we resolve a sceAppContentTemporaryDataMount problem with Sony: https://ps4.scedev.net/forums/thread/20610/?page=1#n135890

		if(!gpTempDirectory[0]) // If we haven't mounted the temp directory yet...
		{
			SceAppContentInitParam initParam;
			memset(&initParam, 0, sizeof(SceAppContentInitParam));

			SceAppContentBootParam bootParam;
			memset(&bootParam, 0, sizeof(SceAppContentBootParam));

			int result = sceAppContentInitialize(&initParam, &bootParam);

			if((result == SCE_OK) ||( result == SCE_APP_CONTENT_ERROR_BUSY)) // If OK or if somebody else didn't already call this...
			{
				SceAppContentMountPoint mountPoint;
				result = sceAppContentTemporaryDataMount(&mountPoint);

				if((result == SCE_OK) || (result == SCE_APP_CONTENT_ERROR_BUSY)) // If OK or if somebody else didn't already call this...
				{
					result = EA::StdC::Strlcpy(gpTempDirectory, mountPoint.data, EAArrayCount(gpTempDirectory));
						
					if(result < EAArrayCount(gpTempDirectory)) // If there was enough capacity...
						return result;
					else
						gpTempDirectory[0] = 0;
				}
			}
		}
		*/

		// For the time being, for retail (shipping) applications, you need to set the requested space in /download0 via the 
		// application's param.sfo file, which is edited with the orbis-pub-sfo.exe (possibly to be renamed 
		// in the future by Sony) GUI applet.
		EA::StdC::Strlcpy(pDirectory, "/download0/temp/", nPathCapacity);
		Directory::EnsureExists(pDirectory);
		return EAIOStrlen8(pDirectory);

	#elif defined(EA_PLATFORM_ANDROID)

		size_t nLength = EA::StdC::Strlen(AssetManagerJNI::GetTempRoot());
		EA::StdC::Strlcpy(pDirectory, AssetManagerJNI::GetTempRoot(), nPathCapacity);
		Path::EnsureTrailingSeparator(pDirectory, nPathCapacity);
		return nLength;

	 #elif defined(EA_PLATFORM_APPLE) && !EAIO_APPLE_USE_UNIX_IO // OS X, iPhone, etc.

		return appleGetTempDirectory(pDirectory, nPathCapacity);
		
	#elif defined(EA_PLATFORM_NX)

		EA::StdC::Strlcpy(pDirectory, "tmp:/temp/", nPathCapacity);
		Directory::EnsureExists(pDirectory);
		return EA::StdC::Strlen(pDirectory);

	#elif EAIO_USE_UNIX_IO

		const char* tmp = getenv("TMPDIR");

		if(!tmp)
			tmp = "/tmp/";        

		size_t nLength = EA::StdC::Strlcpy(pDirectory, tmp, nPathCapacity);

		if((nLength == 0) || (pDirectory[nLength - 1] != '/')) // Some Unix-like platforms don't append a / char by themselves.
		{
			pDirectory[nLength++] = '/';
			pDirectory[nLength]   = 0;
		}

		return (int)nLength;
		
	#elif defined(EA_PLATFORM_SONY)

		Directory::GetCurrentWorkingDirectory(pDirectory, nPathCapacity);
		EA::IO::Path::EnsureTrailingSeparator(pDirectory,nPathCapacity);
		const size_t nLength = EAIOStrlen8(pDirectory);
		EA::StdC::Strlcpy(pDirectory + nLength, "tmp/", nPathCapacity - nLength);  // We don't have a strlcat to use, so we use strlcpy.
		Directory::EnsureExists(pDirectory);
		return EAIOStrlen8(pDirectory);

	#else

		// Bug Paul Pedriana to finish this for the given platform.
		return -1;

	#endif
}



////////////////////////////////////////////////////////////////////////////
// File::SetTempDirectory
//
EAIO_API bool SetTempDirectory(const char32_t* pDirectory)
{
	Path::PathString8 path8;
	ConvertPath(path8, pDirectory);
	return SetTempDirectory(path8.c_str());
}

///////////////////////////////////////////////////////////////////////////////
// SetTempDirectory
//
EAIO_API bool SetTempDirectory(const char16_t* pDirectory)
{
	using namespace EA::IO::Path;

	const size_t nDirectoryLength = EA::StdC::Strlen(pDirectory);

	// We use +2 because we need room for trailing nul char and for a possible appending
	// of a path separator. Strictly speaking, we should make the code a little smarter
	// about the required length and the trailing path separator, but we are simple for now.
	if((nDirectoryLength + 2) <= kTempDirectoryLength)
	{
		if(pDirectory[0])
		{
			EA::StdC::Strlcpy(gpTempDirectory, pDirectory, kTempDirectoryLength);
			Path::EnsureTrailingSeparator(gpTempDirectory, kTempDirectoryLength);

			if(!Directory::Exists(gpTempDirectory))
				return Directory::Create(gpTempDirectory);
		}
		else
			gpTempDirectory[0] = 0;

		return true;
	}

	return false;
}

EAIO_API bool SetTempDirectory(const char* pDirectory)
{
	// This function currently requires UTF8 path strings.

	EA::IO::Path::PathString16 path16;
	ConvertPath(path16, pDirectory);

	return SetTempDirectory(path16.c_str());
}




////////////////////////////////////////////////////////////////////////////
// GetDriveFreeSpace
//
EAIO_API uint64_t GetDriveFreeSpace(const char32_t* pPath)
{
	Path::PathString8 path8;
	ConvertPath(path8, pPath);
	return GetDriveFreeSpace(path8.c_str());
}


#if EA_WCHAR_UNIQUE
////////////////////////////////////////////////////////////////////////////
// GetDriveFreeSpace
//
EAIO_API uint64_t GetDriveFreeSpace(const wchar_t* pPath)
{
	Path::PathString8 path8;
	ConvertPath(path8, pPath);
	return GetDriveFreeSpace(path8.c_str());
}
#endif


///////////////////////////////////////////////////////////////////////////////
// GetDriveFreeSpace
//
EAIO_API uint64_t GetDriveFreeSpace(const char16_t* pPath)
{
	#if defined(EA_PLATFORM_MICROSOFT)
		wchar_t pPathResult[kMaxDriveLength];

		// Convert "vol:\abc\def\ghi" to "vol:\"
		// Convert "abc/def/ghi" to "/abc/"
		wchar_t* p0 = pPathResult;

		for(const char16_t* p1 = pPath; *p1 && ((*p1 == ':') || !IsFilePathSeparator(*p1)); ++p0, ++p1)
			*p0 = *p1;

		*p0++ = EA_FILE_PATH_SEPARATOR_16;
		*p0   = 0;

		ULARGE_INTEGER freeBytesAvailableToCaller;
		ULARGE_INTEGER totalBytes;
		ULARGE_INTEGER totalFreeBytes;

		// Windows requires there to be a trailing path separator if the path is a UNC path.
		Path::EnsureTrailingSeparator(pPathResult, kMaxDriveLength);

		if(GetDiskFreeSpaceExW(pPathResult, &freeBytesAvailableToCaller, &totalBytes, &totalFreeBytes))
			return freeBytesAvailableToCaller.QuadPart;
		// Fall through
	
	#elif EAIO_USE_UNIX_IO && defined(EA_HAVE_STATVFS_H)
		// http://www.opengroup.org/onlinepubs/009695399/basedefs/sys/statvfs.h.html
		// int statvfs(const char *restrict, struct statvfs *restrict);
		// We need to use f_frsize instead of f_bsize, as f_bsize is actually not the block size but rather 
		// is the size the OS likes to use for writes, which will typically be >= f_frsize.

		char path8[kMaxPathLength];
		EA::StdC::Strlcpy(path8, pPath, kMaxPathLength);

		struct statvfs info;
		const int result = statvfs(path8, &info);

		if(result == 0)
			return ((uint64_t)info.f_frsize * (uint64_t)info.f_bfree); // The data type of f_frsize and f_bfree may differ between Unix systems.

	#elif defined(EA_PLATFORM_ANDROID)
		char path8[kMaxPathLength];
		EA::StdC::Strlcpy(path8, pPath, kMaxPathLength);

		if(Directory::Exists(path8) || File::Exists(path8))
		{
			if(strstr(path8, APPBUNDLE_PATH_PREFIX) == path8) // If the path begins with the APPBUNDLE_PATH_PREFIX prefix..
				return 0; // App bundles are read-only at runtime.
			else
			{
				struct statfs info;
				const int result = statfs(path8, &info);

				if(result == 0)
					return ((uint64_t)info.f_frsize * (uint64_t)info.f_bfree); // The data type of f_frsize and f_bfree may differ between Unix systems.
			}
		}
		// Else fall through and return an error.

	#elif defined(EA_PLATFORM_SONY)
		
		// For Kettle we dont have currently any way to determine the free space on a drive
		// but according to Sony this will be possible in the future:
		// https://orbis.scedev.net/support/issue/958/_cellFsGetFreeSize%28%29_-_statvfs%28%29_equivalent 
		return UINT64_C(0xffffffffffffffff);

	#else
		EA_FAIL_M("Unimplemented");
		// Bug Paul Pedriana for this when you need it.
		EA_UNUSED(pPath);

	#endif

	return UINT64_C(0xffffffffffffffff);
}

EAIO_API uint64_t GetDriveFreeSpace(const char* pPath)
{
	EA::IO::Path::PathString16 path16;
	ConvertPath(path16, pPath);

	return GetDriveFreeSpace(path16.c_str());
}

///////////////////////////////////////////////////////////////////////////////
// GetDriveName
//
EAIO_API int GetDriveName(const char32_t* pPath, char32_t* pName)
{
	EA::IO::Path::PathString16 path16;
	ConvertPath(path16, pPath);
	
	char16_t driveName[EA::IO::kMaxVolumeNameLength];

	const int result = GetDriveName(path16.c_str(), driveName);

	if(result > 0)
		EA::StdC::Strlcpy(pName, driveName, kMaxVolumeNameLength);
	else
		pName[0] = 0;

	return result;
}

EAIO_API int GetDriveName(const char16_t* pPath, char16_t* pName)
{
	EA_ASSERT(*pPath); // Assert that the input is non-empty.

	#if EA_WINAPI_FAMILY_PARTITION(EA_WINAPI_PARTITION_DESKTOP)
		WCHAR root[kMaxPathLength + 1];

		if(GetVolumePathNameW((const WCHAR*)pPath, root, kMaxPathLength)) // Convert "C:\abc\def" to (e.g.) "C:\"
		{
			DWORD  fsFlags = 0;
			size_t length  = EA::StdC::Strlen(root);
			DWORD  maxComponentLength = 0;

			if((length == 0) || (root[length - 1] != L'\\'))
				EA::StdC::Strcat(root, L"\\");

			// A trailing backslash is required for 'root'.
			if(GetVolumeInformationW(root, (WCHAR*)pName, (DWORD)kMaxVolumeNameLength, NULL, &maxComponentLength, &fsFlags, NULL, 0))
				return (int)EA::StdC::Strlen(pName);
		}

		pName[0] = 0;
		return -1;

	#elif EAIO_USE_UNIX_IO || defined(EA_PLATFORM_SONY)

		int i;

		// Copy the first component of pPath into pName. Thus if pPath is /abc/def/ghi,
		// pName will be set to /abc (no trailing separator, as it is a volume name).
		for(i = 1, pName[0] = pPath[0]; pPath[i] != '/'; ++i)
			pName[i] = pPath[i];
		pName[i] = 0;

		return i;

	#else
		// Bug Paul Pedriana for this if you need it.
		// Perhaps for mountable file systems we can get the file system name.
		// This might work on Unix systems.
		EA_UNUSED(pPath);
		pName[0] = 0;
		return -1;
	#endif
}

EAIO_API int GetDriveName(const char* pPath, char* pName)
{
	// This function currently requires UTF8 path strings.

	EA::IO::Path::PathString16 path16;
	ConvertPath(path16, pPath);
	
	char16_t driveName[EA::IO::kMaxVolumeNameLength];

	const int result = GetDriveName(path16.c_str(), driveName);

	if(result > 0)
		EA::StdC::Strlcpy(pName, driveName, kMaxVolumeNameLength);
	else
		pName[0] = 0;

	return result;
}



///////////////////////////////////////////////////////////////////////////////
// GetDriveSerialNumber
//
EAIO_API int GetDriveSerialNumber(const char32_t* pPath, char32_t* pSerialNumber)
{
	char16_t path16[kMaxPathLength];
	char16_t serialNumber16[kMaxPathLength];
	EA::StdC::Strlcpy(path16, pPath, kMaxPathLength);
	int ret = GetDriveSerialNumber(path16, serialNumber16); 
	EA::StdC::Strlcpy(pSerialNumber, path16, kMaxPathLength);
	return ret;
}

///////////////////////////////////////////////////////////////////////////////
// GetDriveSerialNumber
//
EAIO_API int GetDriveSerialNumber(const char16_t* pPath, char16_t* pSerialNumber)
{
	#if EA_WINAPI_FAMILY_PARTITION(EA_WINAPI_PARTITION_DESKTOP)
		WCHAR root[kMaxPathLength];

		if(GetVolumePathNameW((const WCHAR*)pPath, root, kMaxPathLength))
		{
			DWORD fsFlags;
			DWORD dwSerialNumber;

			if(GetVolumeInformationW(root, NULL, 0, &dwSerialNumber, NULL, &fsFlags, NULL, 0))
			{
				wsprintfW(reinterpret_cast<LPWSTR>(pSerialNumber), EA_WCHAR("%04X-%04X"), dwSerialNumber >> 16, dwSerialNumber & 0xffff);
				return 10;
			}
		}

		pSerialNumber[0] = 0;
		return 0;

	#else
		// Bug Paul Pedriana for this if you need it.
		EA_UNUSED(pPath);
		pSerialNumber[0] = 0;
		return 0;
	#endif
}

EAIO_API int GetDriveSerialNumber(const char* pPath, char* pSerialNumber)
{
	// This function currently requires UTF8 path strings.

	char16_t pPath16[kMaxPathLength];
	char16_t pSerialNumber16[kMaxVolumeSerialNumberLength];

	EA::StdC::Strlcpy(pPath16, pPath, kMaxPathLength);
	const int result = GetDriveSerialNumber(pPath16, pSerialNumber16);
	if(result > 0)
		EA::StdC::Strlcpy(pSerialNumber, pSerialNumber16, kMaxVolumeSerialNumberLength);
	else
		pSerialNumber[0] = 0;
	return result;
}




////////////////////////////////////////////////////////////////////////////
// GetDriveTypeValue
//
EAIO_API DriveType GetDriveTypeValue(const char32_t* pPath)
{
	Path::PathString8 path8;
	ConvertPath(path8, pPath);
	return GetDriveTypeValue(path8.c_str());
}

///////////////////////////////////////////////////////////////////////////////
// GetDriveTypeValue
//
EAIO_API DriveType GetDriveTypeValue(const char16_t* pPath)
{
	#if EA_WINAPI_FAMILY_PARTITION(EA_WINAPI_PARTITION_DESKTOP)

		const uint32_t nDriveType = GetDriveTypeW(reinterpret_cast<LPCWSTR>(pPath));

		switch (nDriveType)
		{
			case DRIVE_FIXED:     return kDriveTypeFixed;
			case DRIVE_REMOVABLE: return kDriveTypeRemovable;
			case DRIVE_REMOTE:    return kDriveTypeRemote;
			case DRIVE_CDROM:     return kDriveTypeCD;
			case DRIVE_RAMDISK:   return kDriveTypeRAM;
		}

		return kDriveTypeUnknown;

	#elif EAIO_USE_UNIX_IO
		EA_UNUSED(pPath); // Prevent possible compiler warnings related to disuse.
		return kDriveTypeFixed;

	#else
		EA_UNUSED(pPath); // Prevent possible compiler warnings related to disuse.
		return kDriveTypeUnknown;

	#endif
}

EAIO_API DriveType GetDriveTypeValue(const char* pPath)
{
	// This function currently requires UTF8 path strings.
	// To do: Flip the 16 and 8 bit versions so the 8 bit version does all the work.

	EA::IO::Path::PathString16 path16;
	ConvertPath(path16, pPath);

	return GetDriveTypeValue(path16.c_str());
}



///////////////////////////////////////////////////////////////////////////////
// GetDriveInfo
//
EAIO_API size_t GetDriveInfo(DriveInfo32* pDriveInfoArray, size_t nDriveInfoArrayCapacity)
{
	DriveInfo8 driveInfo8Array[10];
	size_t     count = GetDriveInfo(driveInfo8Array, EAArrayCount(driveInfo8Array));
	
	if(count > nDriveInfoArrayCapacity)
		count = nDriveInfoArrayCapacity;

	// Copy to user-supplied drive info, converting to char32_t while doing so.
	for(size_t i = 0; i < count; i++)
	{
		pDriveInfoArray[i].mType = driveInfo8Array[i].mType;
		EA::StdC::Strlcpy(pDriveInfoArray[i].mName, driveInfo8Array[i].mName);
	}

	return count;
}

///////////////////////////////////////////////////////////////////////////////
// GetDriveInfo
//
EAIO_API size_t GetDriveInfo(DriveInfo16* pDriveInfoArray, size_t nDriveInfoArrayCapacity)
{
	#if EA_WINAPI_FAMILY_PARTITION(EA_WINAPI_PARTITION_DESKTOP)

		size_t nCount = 0;

		// Find all logical drives
		const size_t kArraySize = 2048;
		wchar_t     pDriveStringArray[kArraySize];
		const size_t nStringLength = GetLogicalDriveStringsW(kArraySize - 1, pDriveStringArray);

		// Parse the resulting string in the form of "c:\<nul>d:\<nul>e:\<nul><nul>"
		wchar_t* pCurrent = pDriveStringArray;
		wchar_t* pEnd     = pDriveStringArray + nStringLength;

		while(pCurrent != pEnd) // Walk through (e.g.) "c:\<nul>d:\<nul>e:\<nul><nul>"
		{
			if(*pCurrent) // If we have a non-null character (and thus the beginning of a new drive)...
			{
				// It seems that sometimes Windows can return a drive that's invalid. We test that 
				// here by calling GetVolumeInformation on it and verifying that it is seen as valid.
				DWORD fsFlags;

				if(GetVolumeInformationW(pCurrent, NULL, 0, NULL, NULL, &fsFlags, NULL, 0))
				{
					nCount++;

					if(pDriveInfoArray && (nCount <= nDriveInfoArrayCapacity))  // If there is more capacity in the user-provided array...
					{
						// Since the name is null-terminated we can just assign it
						pDriveInfoArray->mName.assign(reinterpret_cast<char16_t*>(pCurrent));    // pCurrent => "c:\<nul>d:\<nul>e:\<nul><nul>", mName => "c:\"

						// Find out the drive type
						const uint32_t nDriveType = GetDriveTypeW(reinterpret_cast<LPCWSTR>(pDriveInfoArray->mName.c_str()));

						switch (nDriveType)
						{
							case DRIVE_FIXED:     pDriveInfoArray->mType = kDriveTypeFixed;     break;
							case DRIVE_REMOVABLE: pDriveInfoArray->mType = kDriveTypeRemovable; break;
							case DRIVE_REMOTE:    pDriveInfoArray->mType = kDriveTypeRemote;    break;
							case DRIVE_CDROM:     pDriveInfoArray->mType = kDriveTypeCD;        break;
							case DRIVE_RAMDISK:   pDriveInfoArray->mType = kDriveTypeRAM;       break;
							default:              pDriveInfoArray->mType = kDriveTypeUnknown;   break;
						}

						// Strip any trailing separators
						const eastl_size_t nTrailingPosition = (pDriveInfoArray->mName.length() - 1);

						if(IsFilePathSeparator(pDriveInfoArray->mName[nTrailingPosition]))
							pDriveInfoArray->mName.resize(nTrailingPosition); // mName => "c:"

						pDriveInfoArray++;

						// Can't fit anymore in... we have to break
						if (nCount == nDriveInfoArrayCapacity)
							break;
					}
				}

				// Advance past the name by moving pCurrent to the next null value.
				while(*pCurrent && (pCurrent != pEnd))
					pCurrent++;

				// pCurrent => "<nul>d:\<nul>e:\<nul><nul>"
			}
			else
				pCurrent++; // pCurrent => "d:\<nul>e:\<nul><nul>"
		}

		return nCount;

	#else

		DriveInfo8 *scratchSpace = static_cast<DriveInfo8 *>(EAAlloca(sizeof(DriveInfo8) * nDriveInfoArrayCapacity));
		for (size_t i = 0; i < nDriveInfoArrayCapacity; ++i)
			new (scratchSpace + i) DriveInfo8();

		const size_t totalFound = GetDriveInfo(scratchSpace,nDriveInfoArrayCapacity);
		const size_t totalWritten = (totalFound < nDriveInfoArrayCapacity) ? totalFound : nDriveInfoArrayCapacity;

		char16_t convertBuffer[kMaxPathLength];

		for (size_t i = 0; i < totalWritten; ++i)
		{
			StrlcpyUTF8ToUTF16(convertBuffer,kMaxPathLength,scratchSpace[i].mName.c_str());
			pDriveInfoArray[i].mName.assign(convertBuffer);
			pDriveInfoArray[i].mType = scratchSpace[i].mType;
		}

		return totalFound;

	#endif
}


EAIO_API size_t GetDriveInfo(DriveInfo8* pDriveInfoArray, size_t nDriveInfoArrayCapacity)
{
	#if EA_WINAPI_FAMILY_PARTITION(EA_WINAPI_PARTITION_DESKTOP)
	
		DriveInfo16 *scratchSpace = static_cast<DriveInfo16 *>(EAAlloca(sizeof(DriveInfo16) * nDriveInfoArrayCapacity));
		for (size_t i = 0; i < nDriveInfoArrayCapacity; ++i)
			new (scratchSpace + i) DriveInfo16();

		const size_t totalFound = GetDriveInfo(scratchSpace, nDriveInfoArrayCapacity);

		char convertBuffer[kMaxPathLength];

		for (size_t i = 0; i < totalFound; ++i)
		{
			StrlcpyUTF16ToUTF8(convertBuffer,kMaxPathLength,scratchSpace[i].mName.c_str());
			pDriveInfoArray[i].mName.assign(convertBuffer);
			pDriveInfoArray[i].mType = scratchSpace[i].mType;
		}

		return totalFound;

	#else

		// Bug Paul Pedriana to finish this for the given platform.
		EA_UNUSED(pDriveInfoArray); EA_UNUSED(nDriveInfoArrayCapacity);
		return 0;

	#endif
}



///////////////////////////////////////////////////////////////////////////////
// Directory::Exists
//
EAIO_API bool Directory::Exists(const char32_t* pDirectory)
{
	Path::PathString8 path8;
	ConvertPath(path8, pDirectory);
	return Directory::Exists(path8.c_str());
}

EAIO_API bool Directory::Exists(const char16_t* pDirectory)
{
	if(pDirectory && *pDirectory)
	{
		#if defined(EA_PLATFORM_MICROSOFT)

			WIN32_FILE_ATTRIBUTE_DATA fileAttr;
			if (GetFileAttributesExW(reinterpret_cast<LPCWSTR>(pDirectory), GetFileExInfoStandard, &fileAttr) == FALSE)
			{
				return false;
			}
			return ((fileAttr.dwFileAttributes != INVALID_FILE_ATTRIBUTES) && ((fileAttr.dwFileAttributes & FILE_ATTRIBUTE_DIRECTORY) != 0));

		#elif defined(EA_PLATFORM_ANDROID)
			char directory8[kMaxPathLength];
			size_t n = EA::StdC::Strlcpy(directory8, pDirectory, kMaxPathLength);

			if(n < (size_t)kMaxPathLength) // If the string could be fully copied... (Question: what's the max path length of appbundle file paths?
			{
				if(strstr(directory8, APPBUNDLE_PATH_PREFIX) == directory8) // If the path begins with the APPBUNDLE_PATH_PREFIX prefix..
				{
					// We can't use the AssetManagerJNI::Exists function, because that works only for files and not 
					// directories. Making it work for directories would significantly complicate and slow it down,
					// so we don't want to do that. Instead we use the directory iteration functionality to search
					// for a directory entry that matches pDirectory.

					DirectoryIterator            iterator;
					DirectoryIterator::EntryList entryList;                         // We don't do anything with this.
					Path::PathStringW            directoryW;

					#if (EA_WCHAR_SIZE == 2) // if (wchar_t == char16_t)
						directoryW = reinterpret_cast<const wchar_t*>(pDirectory);  // Make a copy. e.g. "/abc/def"
					#else
					ConvertPath(directoryW, pDirectory);
					#endif

					Path::EnsureTrailingSeparator(directoryW);                                   // e.g. "/abc/def/"
					const wchar_t* pLeafDirectoryW = Path::FindComponentRvs(directoryW.c_str()); // e.g. "def/"
					Path::PathStringW leafDirectoryW(pLeafDirectoryW);
					directoryW.erase(const_cast<wchar_t*>(pLeafDirectoryW), directoryW.end());   // e.g. "/abc/"
					EA::IO::Path::Simplify(directoryW);
					EA::IO::Path::Simplify(leafDirectoryW);

					size_t entryCount = iterator.Read(directoryW.c_str(), entryList, leafDirectoryW.c_str(), kDirectoryEntryDirectory, 1); // e.g. look in directory "/abc/" for a directory entry named "def/"

					return (entryCount != 0);
				}
				else
				{
					struct stat tempStat;
					const int result = stat(directory8, &tempStat);

					if(result == 0)
						return S_ISDIR(tempStat.st_mode) != 0;
				}
			}
		#elif defined(EA_PLATFORM_NX)
			if (!::IsPathValid(pDirectory))
				return false;

			char directory8[kMaxPathLength];
			size_t n = EA::StdC::Strlcpy(directory8, pDirectory, kMaxPathLength);

			if (n < (size_t)kMaxPathLength) // If the string could be fully copied...
			{
				nn::fs::DirectoryEntryType dt;
				if (nn::fs::GetEntryType(&dt, directory8).IsSuccess() && dt == nn::fs::DirectoryEntryType_Directory)
					return true;
			}
			// else fallthrough
		#elif EAIO_USE_UNIX_IO && defined(EA_HAVE_SYS_STAT_H)

			char directory8[kMaxPathLength];
			size_t n = EA::StdC::Strlcpy(directory8, pDirectory, kMaxPathLength);

			if(n < (size_t)kMaxPathLength) // If the string could be fully copied...
			{
				struct stat tempStat;
				const int result = stat(directory8, &tempStat);

				if(result == 0)
					return S_ISDIR(tempStat.st_mode) != 0;
			}

			// Fall through to return false.

		#elif defined(EA_PLATFORM_SONY)

			char directory8[kMaxDirectoryLength];
			size_t n = EA::StdC::Strlcpy(directory8, pDirectory, kMaxDirectoryLength);

			if (n < (size_t)kMaxDirectoryLength)
			{
				SceKernelStat tempStat;

				if (sceKernelStat(directory8, &tempStat) == SCE_OK)
					return SCE_KERNEL_S_ISDIR(tempStat.st_mode) != 0;
			}

			return false;

		#else
			EA_FAIL_M("Unimplemented");
			// Bug Paul Pedriana to finish this for the given platform.
		#endif
	}

	return false;
}

EAIO_API bool Directory::Exists(const char* pDirectory)
{
	#if EAIO_USE_UNIX_IO // Special case for Unix because Unix paths aren't necessarily UTF8.
		#if defined(EA_PLATFORM_NX)
			if (pDirectory && *pDirectory)
			{
				nn::fs::DirectoryEntryType dt;
				if (::IsPathValid(pDirectory) && 
					nn::fs::GetEntryType(&dt, pDirectory).IsSuccess() && dt == nn::fs::DirectoryEntryType_Directory)
					return true;
			}
		#elif defined(EA_HAVE_SYS_STAT_H)
			if(pDirectory && *pDirectory)
			{
				struct stat tempStat;
				int result;

				#if defined(EA_PLATFORM_ANDROID)
					if(EA::StdC::Strstr(pDirectory, APPBUNDLE_PATH_PREFIX) == pDirectory) // If the path begins with the APPBUNDLE_PATH_PREFIX prefix..
					{
						// See Directory::Exists(char16_t*) for documentation on why we do it this way.
						EA::IO::Path::PathString16 path16;
						ConvertPath(path16, pDirectory);
						EA::IO::Path::Simplify(path16);

						return Directory::Exists(path16.c_str());
					}

					result = stat(pDirectory, &tempStat);

				if(result == 0)
					return S_ISDIR(tempStat.st_mode) != 0;
				#endif

				result = stat(pDirectory, &tempStat);

				if(result == 0)
					return S_ISDIR(tempStat.st_mode) != 0;
			}
		#endif

		return false;

	#elif defined(EA_PLATFORM_SONY)
		SceKernelStat tempStat;
		const int     result = sceKernelStat(pDirectory, &tempStat);

		if(result == SCE_OK)
			return SCE_KERNEL_S_ISDIR(tempStat.st_mode) != 0;

		return false;

	#else
		EA::IO::Path::PathString16 directory16;
		ConvertPath(directory16, pDirectory);

		return Directory::Exists(directory16.c_str());
	#endif
}




///////////////////////////////////////////////////////////////////////////////
// Directory::EnsureExists
//
EAIO_API bool Directory::EnsureExists(const char32_t* pDirectory)
{
	if(pDirectory && *pDirectory)
	{
		if(Directory::Exists(pDirectory))
			return true;
		return Directory::Create(pDirectory);    
	}
	return false;
}

EAIO_API bool Directory::EnsureExists(const char16_t* pDirectory)
{
	if(pDirectory && *pDirectory)
	{
		if(Directory::Exists(pDirectory))
			return true;
		return Directory::Create(pDirectory);    
	}
	return false;
}

EAIO_API bool Directory::EnsureExists(const char* pDirectory)
{
	if(pDirectory && *pDirectory)
	{
		if(Directory::Exists(pDirectory))
			return true;
		return Directory::Create(pDirectory);    
	}
	return false;
}



////////////////////////////////////////////////////////////////////////////
// Directory::Create
//
//  This function actually builds a path or file. Note that if the directory
//  didn't exist previously, this function makes it. This function leaves the
//  system as is if not able to create the entry. Thus, this function is not
//  the same as the C runtime 'mkdir' function or similar OS-supplied functions,
//  as they will only create a directory if the parent directory already exists.
//
//  Win32 example:
//      C:\blah1\blah2\blah3\        Directory
//      C:\blah1\                    Part1
//      C:\blah1\blah2\              Part2
//      C:\blah1\blah2\blah3\        Part3
//
//  Unix example:
//      /blah1/blah2/blah3/          Directory
//      /blah1/                      Part1
//      /blah1/blah2/                Part2
//      /blah1/blah2/blah3/          Part3
//
//  Win32 UNC Example:
//      \\SomeServer\SomeVolume\blah2\blah3\     // Win32
//
namespace
{
	bool CreateDirectoryInternal(const char16_t* pDirectory)
	{
		// To consider: Perhaps this function should not worry about trailing separators -- 
		// it is currently called only from from Create and Create is taking care of them.
		#if defined(EA_PLATFORM_MICROSOFT)
			if(pDirectory[0]) // Our code below requires at least one char in the string.
			{
				char16_t path[kMaxPathLength];

				const size_t nStrlen = EA::StdC::Strlen(pDirectory);

				// Windows doesn't like it when the directory path ends with a 
				// separator (e.g. '\') character, so we remove it if needed.
				if(IsFilePathSeparator(pDirectory[nStrlen - 1]))  // If the path ends with a '\' char...
				{
					EA::StdC::Strlcpy(path, pDirectory, nStrlen);   // Force 0-terminator in place of directory separator.
					pDirectory = path;
				}

				const BOOL bResult = CreateDirectoryW(reinterpret_cast<LPCWSTR>(pDirectory), NULL);
				return bResult || (GetLastError() == ERROR_ALREADY_EXISTS);
			}

			return false;

		#elif EAIO_USE_UNIX_IO || defined(EA_PLATFORM_SONY)

			if(pDirectory[0]) // Our code below requires at least one char in the string.
			{
				Path::PathString8 directory8;
				uint32_t nLength = ConvertPath(directory8, pDirectory);

				Path::StripTrailingSeparator(&directory8[0], nLength);

				#if EAIO_USE_UNIX_IO
					#if EAIO_MKDIR_PRESENT
						const int result = mkdir(directory8.c_str(), 0777);
						return ((result == 0) || (errno == EEXIST));
					#else
						EA_FAIL();
					#endif
				#elif defined(EA_PLATFORM_SONY)
					return (sceKernelMkdir(directory8.c_str(), SCE_KERNEL_S_IRWXU) == SCE_OK);
				#else
					const BOOL bResult = CreateDirectoryA(directory8.c_str(), NULL);
					return bResult || (GetLastError() == ERROR_ALREADY_EXISTS);

				#endif
			}

			return false;

		#else
			EA_FAIL_M("Unimplemented");
			// Bug Paul Pedriana for this when you need it.
			EA_UNUSED(pDirectory);
			return false;
		#endif
	}
}




///////////////////////////////////////////////////////////////////////////////
// Create
//
// This function does a smart directory create. It doesn't require that the 
// parent directory exist beforehand as with conventional directory creation
// API functions.
// 
EAIO_API bool Directory::Create(const char32_t* pDirectory)
{
	Path::PathString16 path16;
	ConvertPath(path16, pDirectory);
	return Directory::Create(path16.c_str());
}

EAIO_API bool Directory::Create(const char16_t* pDirectory)
{
	using namespace Path;

	bool success(false);

	if(pDirectory[0])
	{
		PathString16 fullDirPath(pDirectory);
		Path::EnsureTrailingSeparator(fullDirPath);
		
		// Path for Scratchwork
		char16_t scratchPath[kMaxPathLength];

		success = true;

		PathString16::iterator it = GetPathComponentEnd(fullDirPath.begin(), fullDirPath.end(), 1);
		PathString16::iterator firstNewDirIt = NULL; // Mark the top-most directory that we created; if we need to do a rollback, we recursively remove this directory.

		do //Use do/while because even on a simple fullDirPath of "/" there will be an initial dir of "/".
		{
			// Example of first iteration:
			//  fullDirPath = "/usr/foo/bar/"
			//           it =     "/foo/bar/"
			// it - fullDirPath.begin() = "/usr"   // We reserve +1 for NULL terminator (don't want path separator for directory creation).
			uint32_t nLength = (uint32_t)(it - fullDirPath.begin() + 1);
			if(nLength >= (uint32_t)kMaxDirectoryLength)
			{
				success = false;
				break;
			}

			// If necessary, create the next component in our path
			EA::StdC::Strlcpy(scratchPath, fullDirPath.c_str(), nLength);
			if(!Directory::Exists(scratchPath))
			{
				if(!CreateDirectoryInternal(scratchPath))
				{
					success = false;
					break;
				}
				else if(!firstNewDirIt)
					firstNewDirIt = it;
			}

			// Advance forward by one component from our current component
			it = GetPathComponentEnd(it, fullDirPath.end(), 1);
		}
		while(it != fullDirPath.end());

		// An error occurred mid-way through, remove any directories that we've created thus far.
		if(!success && firstNewDirIt)
		{
			// We can do a recursive wipe here because we know that the directory hierarchy we're 
			// about to remove was been created solely by us.
			EA::StdC::Strlcpy(scratchPath, fullDirPath.c_str(), firstNewDirIt - fullDirPath.begin() + 1);
			Directory::Remove(scratchPath);
		}
	}

	return success;
}

EAIO_API bool Directory::Create(const char* pDirectory)
{
	// This function currently requires UTF8 path strings.

	EA::IO::Path::PathString16 path16;
	ConvertPath(path16, pDirectory);

	return Directory::Create(path16.c_str());
}


namespace
{
	// This function uses wchar_t instead of char16_t and char32_t because the DirectoryIterator
	// class is currently defined in terms of wchar_t, and we use it below.
	// The logic in this module, for efficiency purposes, requires that pDirectory is non-const and 
	// that at least one character can be appended to it.
	bool RemoveDirectoryRecursiveInternalW(wchar_t* pDirectory, size_t pathLen)
	{
		bool success = false;

		if(pDirectory[0])
		{
			DirectoryIterator            di;
			DirectoryIterator::EntryList entryList(DirectoryIterator::EntryList::allocator_type(EASTL_NAME_VAL(EAIO_ALLOC_PREFIX "FileUtil")));
			size_t                       namePos = pathLen;

			success = true; // True until found to be false.
			
			if(di.Read(pDirectory, entryList, 0, kDirectoryEntryFile | kDirectoryEntryDirectory))
			{
				// Make sure the directory ends in a path separator. Note: If we have a
				// relative drive specification (c:), we don't want to add a backslash to it!
				if(pathLen)
				{
					wchar_t lastChar = pDirectory[pathLen - 1];

					if(!IsFilePathSeparator(lastChar) && (lastChar != EA_FILE_PATH_DRIVE_SEPARATOR_16) && (pathLen < (kMaxPathLength - 1)))
						pDirectory[namePos++] = EA_FILE_PATH_SEPARATOR_8;
				}

				for(DirectoryIterator::EntryList::const_iterator it(entryList.begin()), itEnd(entryList.end()); it != itEnd; ++it)
				{
					const DirectoryIterator::Entry& ent = *it;
					const eastl_size_t nameLen = ent.msName.length();

					if((nameLen + pathLen) > (kMaxPathLength - 1))
						success = false;
					else
					{
						ent.msName.copy(pDirectory + namePos, nameLen);
						pDirectory[namePos + nameLen] = 0;

						if(ent.mType == kDirectoryEntryDirectory)
						{
							if(!RemoveDirectoryRecursiveInternalW(pDirectory, namePos + nameLen))
								success = false;
						}
						else
						{
							if(!File::Remove(pDirectory))
								success = false;
						}
					}
				}
			}

			pDirectory[pathLen] = 0;
			
			if(!Directory::Remove(pDirectory, false))
				success = false;
		}

		return success;
	}

	bool RemoveDirectoryRecursiveInternal(char16_t* pDirectory, size_t pathLen) 
	{
		#if (EA_WCHAR_SIZE == 1)
			Path::PathString16 path16(pDirectory, (eastl_size_t)pathLen);
			char path8[kMaxPathLength];
			int nStrlen = EA::StdC::Strlcpy(path8, path16.c_str(), kMaxPathLength, pathLen);
			return RemoveDirectoryRecursiveInternalW((wchar_t*)path8, nStrlen);

		#elif(EA_WCHAR_SIZE == 2) // if wchar_t == char16_t
			return RemoveDirectoryRecursiveInternalW((wchar_t*)pDirectory, pathLen);

		#else
			Path::PathString16 path16(pDirectory, (eastl_size_t)pathLen);
			char32_t path32[kMaxPathLength];
			int nStrlen = EA::StdC::Strlcpy(path32, path16.c_str(), kMaxPathLength, pathLen);
			return RemoveDirectoryRecursiveInternalW((wchar_t*)path32, nStrlen);
		#endif
	}

} // namespace



///////////////////////////////////////////////////////////////////////////////
// Remove
//
EAIO_API bool Directory::Remove(const char32_t* pDirectory, bool bAllowRecursiveRemoval)
{
	Path::PathString16 path16;
	ConvertPath(path16, pDirectory);
	return Directory::Remove(path16.c_str(), bAllowRecursiveRemoval);
}

EAIO_API bool Directory::Remove(const char16_t* pDirectory, bool bAllowRecursiveRemoval)
{
	if(bAllowRecursiveRemoval)
	{
		// Create a mutable version of the path.
		char16_t path[kMaxPathLength];
		EA::StdC::Strlcpy(path, pDirectory, kMaxPathLength);

		return RemoveDirectoryRecursiveInternal(path, EA::StdC::Strlen(path));
	}
	else // Non-recursive 
	{
		#if defined(EA_PLATFORM_MICROSOFT)
			// Windows doesn't like it when the directory path ends with a 
			// separator (e.g. '\') character, so we correct for this if needed.
			const size_t nStrlen = EA::StdC::Strlen(pDirectory);

			if(!IsFilePathSeparator(pDirectory[nStrlen - 1]))
				return 0 != RemoveDirectoryW(reinterpret_cast<LPCWSTR>(pDirectory));

			// Else we need to remove the separator.
			char16_t path[kMaxPathLength];
			EA::StdC::Strlcpy(path, pDirectory, nStrlen); // Force NULL terminator in place of directory separator
			return 0 != RemoveDirectoryW(reinterpret_cast<LPCWSTR>(pDirectory));

		#elif EAIO_USE_UNIX_IO || defined(EA_PLATFORM_SONY)

			Path::PathString8 directory8;
			uint32_t nLength = ConvertPath(directory8, pDirectory);

			Path::StripTrailingSeparator(&directory8[0], nLength);

			#if EAIO_USE_UNIX_IO
				return (rmdir(directory8.c_str()) == 0);
			#elif defined(EA_PLATFORM_SONY)
				return (sceKernelRmdir(directory8.c_str()) == SCE_OK);
			#else
				return (RemoveDirectoryA(directory8.c_str()) != 0);
			#endif

		#else
			EA_FAIL_M("Unimplemented");
			// Bug Paul Pedriana for this when you need it.
			return false;
		#endif
	}
}

EAIO_API bool Directory::Remove(const char* pDirectory, bool bAllowRecursiveRemoval)
{
	#if EAIO_USE_UNIX_IO // Special case for Unix because Unix paths aren't necessarily UTF8.
		if(bAllowRecursiveRemoval)
		{
			// This pathway currently requires UTF8 path strings.
			EA::IO::Path::PathString16 path16;
			ConvertPath(path16, pDirectory);

			return Directory::Remove(path16.c_str(), bAllowRecursiveRemoval);
		}
		else
		{
			Path::PathString8 directory8(pDirectory);
			Path::StripTrailingSeparator(&directory8[0], directory8.length());

			return (rmdir(directory8.c_str()) == 0);
		}
	#else
		EA::IO::Path::PathString16 path16;
		ConvertPath(path16, pDirectory);

		return Directory::Remove(path16.c_str(), bAllowRecursiveRemoval);
	#endif
}



///////////////////////////////////////////////////////////////////////////////
// Move
//
// This approach of using File::Move may not work on all platforms. If you need it to work on some 
// platform where it doesn't work, contact the maintainers of this package to have it implemented.

EAIO_API bool Move(const char32_t* pDirectorySource, const char32_t* pDirectoryDestination, bool bOverwriteIfPresent)
{
	if(Directory::Exists(pDirectorySource)) // This doubles as verifying that pDirectorySource is a directory.
		return File::Move(pDirectorySource, pDirectoryDestination, bOverwriteIfPresent);
	return false;
}

EAIO_API bool Move(const char16_t* pDirectorySource, const char16_t* pDirectoryDestination, bool bOverwriteIfPresent)
{
	if(Directory::Exists(pDirectorySource)) // This doubles as verifying that pDirectorySource is a directory.
		return File::Move(pDirectorySource, pDirectoryDestination, bOverwriteIfPresent);
	return false;
}

EAIO_API bool Move(const char* pDirectorySource, const char* pDirectoryDestination, bool bOverwriteIfPresent)
{
	if(Directory::Exists(pDirectorySource))
		return File::Move(pDirectorySource, pDirectoryDestination, bOverwriteIfPresent);
	return false;
}



///////////////////////////////////////////////////////////////////////////////
// Rename
//
EAIO_API bool Directory::Rename(const char32_t* pDirectoryOld, const char32_t* pDirectoryNew)
{
	Path::PathString8 pathOld8, pathNew8;
	ConvertPath(pathOld8, pDirectoryOld);
	ConvertPath(pathNew8, pDirectoryNew);

	return File::Rename(pathOld8.c_str(), pathNew8.c_str());
}

EAIO_API bool Directory::Rename(const char16_t* pDirectoryOld, const char16_t* pDirectoryNew)
{
	#if defined(EA_PLATFORM_MICROSOFT)
		// Under Win32 you can rename a directory by using the file move API.
		return (File::Move(pDirectoryOld, pDirectoryNew) != 0);

	#elif EAIO_USE_UNIX_IO
		char pPathSource8[kMaxPathLength];
		EA::StdC::Strlcpy(pPathSource8, pDirectoryOld, kMaxPathLength);

		char pPathDestination8[kMaxPathLength];
		EA::StdC::Strlcpy(pPathDestination8, pDirectoryNew, kMaxPathLength);

		// The NX has a non-standard rename implementation so we must use the nn version
		#if defined(EA_PLATFORM_NX)
			return Rename(pPathSource8, pPathDestination8);
		#else
			return rename(pPathSource8, pPathDestination8) == 0;
		#endif

	#elif defined(EA_PLATFORM_SONY)
		char pathSource8[kMaxDirectoryLength];
		size_t n = EA::StdC::Strlcpy(pathSource8, pDirectoryOld, kMaxDirectoryLength);
		if (n < (size_t)kMaxDirectoryLength)
			return false;

		char pathDestination8[kMaxDirectoryLength];
		n = EA::StdC::Strlcpy(pathDestination8, pDirectoryNew, kMaxDirectoryLength);
		if (n < (size_t)kMaxDirectoryLength)
			return false;

		return (sceKernelRename(pathSource8, pathDestination8) == SCE_OK);

	#else
		EA_FAIL_M("Unimplemented");
		// Bug Paul Pedriana for this when you need it.
		// This isn't necessarily supported on all platforms.
		// On the other hand, a brute force version which copies contents
		// to a new location then deletes the old location may be possible.
		
		EA_UNUSED(pDirectoryOld); EA_UNUSED(pDirectoryNew);
		return false;
	#endif
}

EAIO_API bool Directory::Rename(const char* pDirectoryOld, const char* pDirectoryNew)
{
	#if EAIO_USE_UNIX_IO
		// The NX has a non-standard rename implementation so we must use the nn version
		#if defined(EA_PLATFORM_NX)
			if (!::IsPathValid(pDirectoryOld) || !::IsPathValid(pDirectoryNew))
				return false;

			return nn::fs::RenameDirectory(pDirectoryOld, pDirectoryNew).IsSuccess();
		#else
			// Question: Does this function call require that path separators be absent from the end of the dir path?
			return rename(pDirectoryOld, pDirectoryNew) == 0;
		#endif
	#else        
		EA::IO::Path::PathString16 oldPath16;
		ConvertPath(oldPath16, pDirectoryOld);

		EA::IO::Path::PathString16 newPath16;
		ConvertPath(newPath16, pDirectoryNew);

		return Directory::Rename(oldPath16.c_str(), newPath16.c_str());
	#endif
}




///////////////////////////////////////////////////////////////////////////////
// Copy
//
namespace
{
	// This is defined in terms of wchar_t because it uses DirectoryIterator, 
	// which itself is wchar_t.
	bool CopyW(const wchar_t* pDirectorySource, const wchar_t* pDirectoryDestination, bool bRecursive, bool bOverwriteIfAlreadyPresent)
	{
		using namespace EA::IO::Path;

		bool bResult;

		PathStringW sourceW(pDirectorySource);
		Path::Normalize(sourceW);
		PathStringW destW(pDirectoryDestination);
		Path::Normalize(destW);

		bResult = Directory::Exists(sourceW.c_str());

		if(bResult)
		{
			bResult = Directory::EnsureExists(destW.c_str());

			if(bResult)
			{
				DirectoryIterator            directoryIterator;
				DirectoryIterator::EntryList entryList(DirectoryIterator::EntryList::allocator_type(EASTL_NAME_VAL(EAIO_ALLOC_PREFIX "EAFileNotification/FSEntry")));

				directoryIterator.Read(sourceW.c_str(), entryList, NULL, bRecursive ? kDirectoryEntryDirectory | kDirectoryEntryFile : kDirectoryEntryFile);

				for(DirectoryIterator::EntryList::iterator it(entryList.begin()); it != entryList.end(); ++it)
				{
					DirectoryIterator::Entry& entry = *it;

					PathStringW sourcePathW(sourceW);
					PathStringW destPathW(destW);

					Path::Join(sourcePathW, entry.msName.c_str());
					Path::Join(destPathW, entry.msName.c_str());

					if((entry.mType == kDirectoryEntryDirectory) && bRecursive)
					{
						if(!Directory::Copy(sourcePathW.c_str(), destPathW.c_str(), bOverwriteIfAlreadyPresent))
							bResult = false;
					}
					else if(entry.mType == kDirectoryEntryFile)
					{
						if(!File::Copy(sourcePathW.c_str(), destPathW.c_str(), bOverwriteIfAlreadyPresent))
							bResult = false;
					}
				}
			}
		}

		return bResult;
	}
}


EAIO_API bool Directory::Copy(const char32_t* pDirectorySource, const char32_t* pDirectoryDestination, bool bRecursive, bool bOverwriteIfAlreadyPresent)
{
	#if !EA_WCHAR_UNIQUE && (EA_WCHAR_SIZE == 4) // if wchar_t == char32_t
		return CopyW(pDirectorySource, pDirectoryDestination, bRecursive, bOverwriteIfAlreadyPresent);
	#else
		EA::IO::Path::PathStringW directorySourceW;
		ConvertPath(directorySourceW, pDirectorySource);

		EA::IO::Path::PathStringW directoryDestinationW;
		ConvertPath(directoryDestinationW, pDirectoryDestination);

		return CopyW(directorySourceW.c_str(), directoryDestinationW.c_str(), bRecursive, bOverwriteIfAlreadyPresent);
	#endif
}

EAIO_API bool Directory::Copy(const char16_t* pDirectorySource, const char16_t* pDirectoryDestination, bool bRecursive, bool bOverwriteIfAlreadyPresent)
{
	#if !EA_WCHAR_UNIQUE && (EA_WCHAR_SIZE == 2) // if wchar_t == char16_t
		return CopyW(pDirectorySource, pDirectoryDestination, bRecursive, bOverwriteIfAlreadyPresent);
	#else
		EA::IO::Path::PathStringW directorySourceW;
		ConvertPath(directorySourceW, pDirectorySource);

		EA::IO::Path::PathStringW directoryDestinationW;
		ConvertPath(directoryDestinationW, pDirectoryDestination);

		return CopyW(directorySourceW.c_str(), directoryDestinationW.c_str(), bRecursive, bOverwriteIfAlreadyPresent);
	#endif
}

EAIO_API bool Directory::Copy(const char* pDirectorySource, const char* pDirectoryDestination, bool bRecursive, bool bOverwriteIfAlreadyPresent)
{
	// This function currently requires UTF8 path strings.

	#if(EA_WCHAR_SIZE == 1) // if wchar_t == char
		return CopyW(pDirectorySource, pDirectoryDestination, bRecursive, bOverwriteIfAlreadyPresent);
	#else
		EA::IO::Path::PathStringW directorySourceW;
		ConvertPath(directorySourceW, pDirectorySource);

		EA::IO::Path::PathStringW directoryDestinationW;
		ConvertPath(directoryDestinationW, pDirectoryDestination);

		return CopyW(directorySourceW.c_str(), directoryDestinationW.c_str(), bRecursive, bOverwriteIfAlreadyPresent);
	#endif
}




///////////////////////////////////////////////////////////////////////////////
// SetAttributes
//
static bool SetAttributesW(const wchar_t* pPath, int nAttributeMask, bool bEnable, bool bRecursive, bool bIncludeBaseDirectory)
{
	using namespace EA::IO::Path;

	bool bResult = false;

	if(*pPath)
	{
		bResult = true;

		PathStringW baseW(pPath);
		Path::Normalize(baseW);

		if(bIncludeBaseDirectory)
			bResult = Directory::SetAttributes(baseW.c_str(), nAttributeMask, bEnable);

		if(bResult)
		{
			DirectoryIterator            directoryIterator;
			DirectoryIterator::EntryList entryList(DirectoryIterator::EntryList::allocator_type(EASTL_NAME_VAL(EAIO_ALLOC_PREFIX "EAFileNotification/FSEntry")));

			bResult = (directoryIterator.Read(baseW.c_str(), entryList, NULL, bRecursive ? kDirectoryEntryDirectory | kDirectoryEntryFile : kDirectoryEntryFile) != 0);

			if(bResult)
			{
				for(DirectoryIterator::EntryList::iterator it(entryList.begin()); it != entryList.end(); ++it)
				{
					DirectoryIterator::Entry& entry = *it;

					PathStringW pathW(baseW);
					Path::Join(pathW, entry.msName.c_str());

					if((entry.mType == kDirectoryEntryDirectory) && bRecursive)
					{
						if(!Directory::SetAttributes(pathW.c_str(), nAttributeMask, bEnable, true, true))
							bResult = false;
					}
					else if(entry.mType == kDirectoryEntryFile)
					{
						if(!File::SetAttributes(pathW.c_str(), nAttributeMask, bEnable))
							bResult = false;
					}
				}
			}
		}
	}

	return bResult;
}

EAIO_API bool Directory::SetAttributes(const char32_t* pPath, int nAttributeMask, bool bEnable, bool bRecursive, bool bIncludeBaseDirectory)
{
	#if !EA_WCHAR_UNIQUE && (EA_WCHAR_SIZE == 4)
		return SetAttributesW(pPath, nAttributeMask, bEnable, bRecursive, bIncludeBaseDirectory);
	#else
		EA::IO::Path::PathStringW pathW;
		ConvertPath(pathW, pPath);

		return SetAttributesW(pathW.c_str(), nAttributeMask, bEnable, bRecursive, bIncludeBaseDirectory);
	#endif
}

EAIO_API bool Directory::SetAttributes(const char16_t* pPath, int nAttributeMask, bool bEnable, bool bRecursive, bool bIncludeBaseDirectory)
{
	#if !EA_WCHAR_UNIQUE && (EA_WCHAR_SIZE == 2) // if wchar_t == char16_t
		return SetAttributesW(pPath, nAttributeMask, bEnable, bRecursive, bIncludeBaseDirectory);
	#else
		EA::IO::Path::PathStringW pathW;
		ConvertPath(pathW, pPath);

		return SetAttributesW(pathW.c_str(), nAttributeMask, bEnable, bRecursive, bIncludeBaseDirectory);
	#endif
}

EAIO_API bool Directory::SetAttributes(const char* pPath, int nAttributeMask, bool bEnable, bool bRecursive, bool bIncludeBaseDirectory)
{
	// This function currently requires UTF8 path strings.
	#if (EA_WCHAR_SIZE == 1)
		return SetAttributesW(pPath, nAttributeMask, bEnable, bRecursive, bIncludeBaseDirectory);
	#else
		EA::IO::Path::PathStringW pathW;
		ConvertPath(pathW, pPath);

		return SetAttributes(pathW.c_str(), nAttributeMask, bEnable, bRecursive, bIncludeBaseDirectory);
	#endif
}



///////////////////////////////////////////////////////////////////////////////
// GetTime
//
EAIO_API time_t Directory::GetTime(const char32_t* pPath, FileTimeType timeType)
{
	return File::GetTime(pPath, timeType);
}

EAIO_API time_t Directory::GetTime(const char16_t* pPath, FileTimeType timeType)
{
	return File::GetTime(pPath, timeType);
}

EAIO_API time_t Directory::GetTime(const char* pPath, FileTimeType timeType)
{
	return File::GetTime(pPath, timeType);
}



///////////////////////////////////////////////////////////////////////////////
// SetTime
//
EAIO_API bool Directory::SetTime(const char32_t* pPath, int nFileTimeTypeFlags, time_t nTime)
{
	return File::SetTime(pPath, nFileTimeTypeFlags, nTime);
}

EAIO_API bool Directory::SetTime(const char16_t* pPath, int nFileTimeTypeFlags, time_t nTime)
{
	return File::SetTime(pPath, nFileTimeTypeFlags, nTime);
}

EAIO_API bool Directory::SetTime(const char* pPath, int nFileTimeTypeFlags, time_t nTime)
{
	return File::SetTime(pPath, nFileTimeTypeFlags, nTime);
}
 


///////////////////////////////////////////////////////////////////////////////
// GetCurrentWorkingDirectory
//
EAIO_API int Directory::GetCurrentWorkingDirectory(char32_t* pDirectory, uint32_t nPathCapacity)
{
	char directory8[kMaxPathLength];
	int     result = Directory::GetCurrentWorkingDirectory(directory8, kMaxPathLength);

	if(result > 0) // If succeeded...
	{
		result = EA::StdC::Strlcpy(pDirectory, directory8, nPathCapacity); // Copy to the user-supplied pDirectory.

		if((result > 0) && (result < (int)nPathCapacity)) // If there wasn't an error and if there was enough space...
			return result;
	}

	return 0;
}

EAIO_API int Directory::GetCurrentWorkingDirectory(char16_t* pDirectory, uint32_t nPathCapacity)
{
	#if defined(EA_PLATFORM_MICROSOFT) && EA_WINAPI_FAMILY_PARTITION(EA_WINAPI_PARTITION_DESKTOP)

		// Windows has a GetCurrentDirectory function, but for symmetry with 
		// our SetCurrentWorkingDirectory function, we use _wgetcwd().
		const wchar_t* const pResult = _wgetcwd(reinterpret_cast<wchar_t *>(pDirectory), kMaxDirectoryLength - 1); // '- 1' so that a '/' can possibly be appended.

		if(pResult)
		{
			Path::EnsureTrailingSeparator(pDirectory, nPathCapacity);
			return (int)wcslen(reinterpret_cast<LPCWSTR>(pDirectory));
		}

		pDirectory[0] = 0;
		return 0;

	#elif EAIO_USE_UNIX_IO

		char pPath8[kMaxDirectoryLength];
		const char* const pResult = getcwd(pPath8, kMaxDirectoryLength - 6); // '- 6' so that a '/' can possibly be appended. UTF8 can burn 6 bytes for a single character encoding.

		if(pResult)
		{
			size_t nStrlen = EA::StdC::Strlcpy(pDirectory, (char*)pPath8, kMaxDirectoryLength - 2, kLengthNull); // -2 so we have space for possible trailing '\' and '\0'.

			if(Path::EnsureTrailingSeparator(pDirectory, nPathCapacity))
				nStrlen++;
			return (int)nStrlen;
		}

		pDirectory[0] = 0;
		return 0;

	#else

		if(!gbCurrentWorkingDirectoryInitialized) // If we haven't set the default value of the current working directory...
		{
			// We set the value to be the application directory.
			gbCurrentWorkingDirectoryInitialized = true;

			#if defined(EA_PLATFORM_MICROSOFT)
				#if EASTDC_VERSION_N >= 10901
					EA::StdC::GetCurrentProcessDirectory(gpCurrentWorkingDirectory, EAArrayCount(gpCurrentWorkingDirectory));
				#else
					EA::StdC::GetCurrentProcessDirectory(gpCurrentWorkingDirectory);
				#endif
			#elif defined(EA_PLATFORM_SONY)
				EA::StdC::Strlcpy(gpCurrentWorkingDirectory, EA_CHAR16("/app0/"), kTempDirectoryLength);
			#else
				// To do: Other platforms here.
			#endif
		}

		return (int)EA::StdC::Strlcpy(pDirectory, gpCurrentWorkingDirectory, nPathCapacity);

	#endif
}

EAIO_API int Directory::GetCurrentWorkingDirectory(char* pDirectory, uint32_t nPathCapacity)
{
	#if EAIO_USE_UNIX_IO // Special case for Unix because Unix paths aren't necessarily UTF8.
		const char* const pResult = getcwd(pDirectory, nPathCapacity);

		if(pResult)
		{
			Path::EnsureTrailingSeparator(pDirectory, nPathCapacity);
			return (int)EA::StdC::Strlen(pDirectory);
		}

		pDirectory[0] = 0;
		return 0;
	#else
		char16_t pDirectory16[kMaxPathLength];

		Directory::GetCurrentWorkingDirectory(pDirectory16, kMaxPathLength);

		return (int)(unsigned)EA::StdC::Strlcpy(pDirectory, pDirectory16, nPathCapacity);
	#endif
}




///////////////////////////////////////////////////////////////////////////////
// SetCurrentWorkingDirectory
//
EAIO_API bool Directory::SetCurrentWorkingDirectory(const char32_t* pDirectory)
{
	char directory8[kMaxPathLength];
	int     result = EA::StdC::Strlcpy(directory8, pDirectory, kMaxPathLength);

	if((result > 0) && (result < kMaxPathLength)) // If succeeded...
		return Directory::SetCurrentWorkingDirectory(directory8);

	return false;
}

EAIO_API bool Directory::SetCurrentWorkingDirectory(const char16_t* pDirectory)
{
	#if EA_WINAPI_FAMILY_PARTITION(EA_WINAPI_PARTITION_DESKTOP)
		// Windows has a SetCurrentDirectory function, but it doesn't change any settings
		// in the C runtime. _wchdir calls the OS SetCurrentDirectory and sets the C runtime values.
		return _wchdir(reinterpret_cast<const wchar_t *>(pDirectory)) == 0;

	#elif EAIO_USE_UNIX_IO

		char directory8[kMaxPathLength];
		EA::StdC::Strlcpy(directory8, pDirectory, kMaxPathLength);

		return chdir(directory8) == 0;

	#else
		bool bResult = false;

		size_t nDirectoryLength = EA::StdC::Strlen(pDirectory);

		// We use +2 because we need room for trailing nul char and for a possible appending
		// of a path separator. Strictly speaking, we should make the code a little smarter
		// about the required length and the trailing path separator, but we are simple for now.

		if((nDirectoryLength + 2) < (sizeof(gpCurrentWorkingDirectory) / sizeof(gpCurrentWorkingDirectory[0])))
		{
			bResult = true;

			if(pDirectory[0])
			{
				EA::StdC::Strlcpy(gpCurrentWorkingDirectory, pDirectory, kCWDLength);
				Path::EnsureTrailingSeparator(gpCurrentWorkingDirectory, kMaxDirectoryLength);

				if(!Directory::Exists(gpCurrentWorkingDirectory))
					bResult = Directory::Create(gpCurrentWorkingDirectory);
			}
			else
				gpCurrentWorkingDirectory[0] = 0;


		}

		return bResult;

	#endif
}

EAIO_API bool Directory::SetCurrentWorkingDirectory(const char* pDirectory)
{
	#if EAIO_USE_UNIX_IO // Special case for Unix because Unix paths aren't necessarily UTF8.
		// Question: Does this function call require that path separators be absent from the end of the dir path?
		return chdir(pDirectory) == 0;
	#else
		EA::IO::Path::PathString16 path16;
		ConvertPath(path16, pDirectory);

		return Directory::SetCurrentWorkingDirectory(path16.c_str());
	#endif
}




static bool IsDirectoryEmptyW(const wchar_t* pDirectory, int nDirectoryEntryFlags, bool bRecursive)
{
	EA::IO::DirectoryIterator::EntryList entryList;
	EA::IO::DirectoryIterator it;

	if(bRecursive)
		return it.ReadRecursive(pDirectory, entryList, NULL, nDirectoryEntryFlags, true, true, 1, false) == 0;

	return it.Read(pDirectory, entryList, NULL, nDirectoryEntryFlags, 1, false) == 0;
}

EAIO_API bool Directory::IsDirectoryEmpty(const char32_t* pDirectory, int nDirectoryEntryFlags, bool bRecursive)
{
	#if !EA_WCHAR_UNIQUE && (EA_WCHAR_SIZE == 4)
		return IsDirectoryEmptyW(pDirectory, nDirectoryEntryFlags, bRecursive);
	#else
		EA::IO::Path::PathStringW directoryW;
		ConvertPath(directoryW, pDirectory);

		return IsDirectoryEmptyW(directoryW.c_str(), nDirectoryEntryFlags, bRecursive);
	#endif
}

EAIO_API bool Directory::IsDirectoryEmpty(const char16_t* pDirectory, int nDirectoryEntryFlags, bool bRecursive)
{
	#if !EA_WCHAR_UNIQUE && (EA_WCHAR_SIZE == 2)                // if wchar_t == char16_t
		return IsDirectoryEmptyW(pDirectory, nDirectoryEntryFlags, bRecursive);
	#else
		EA::IO::Path::PathStringW directoryW;
		ConvertPath(directoryW, pDirectory);

		return IsDirectoryEmptyW(directoryW.c_str(), nDirectoryEntryFlags, bRecursive);
	#endif
}

EAIO_API bool Directory::IsDirectoryEmpty(const char* pDirectory, int nDirectoryEntryFlags, bool bRecursive)
{
	// This function currently requires UTF8 path strings.
	#if (EA_WCHAR_SIZE == 1)
		return IsDirectoryEmptyW(pDirectory, nDirectoryEntryFlags, bRecursive);
	#else
		EA::IO::Path::PathStringW directoryW;
		ConvertPath(directoryW, pDirectory);

		return IsDirectoryEmptyW(directoryW.c_str(), nDirectoryEntryFlags, bRecursive);
	#endif
}




///////////////////////////////////////////////////////////////////////////////
// GetSpecialDirectory
//
EAIO_API int GetSpecialDirectory(SpecialDirectory specialDirectory, char32_t* pDirectory, 
								 bool bEnsureDirectoryExistence, uint32_t nPathCapacity)
{
	char directory8[kMaxPathLength];

	if(GetSpecialDirectory(specialDirectory, directory8, bEnsureDirectoryExistence, kMaxPathLength) > 0) // If succeeded...
	{
		const int intendedStrlen = EA::StdC::Strlcpy(pDirectory, directory8, nPathCapacity); // Copy to the user-supplied pDirectory.

		if((intendedStrlen > 0) && (intendedStrlen < (int)nPathCapacity)) // If there wasn't an error and if there was enough space...
			return intendedStrlen;
	}

	return -1;
}

EAIO_API int GetSpecialDirectory(SpecialDirectory specialDirectory, char16_t* pDirectory, 
								 bool bEnsureDirectoryExistence, uint32_t nPathCapacity)
{
	pDirectory[0] = 0;

	#if EA_WINAPI_FAMILY_PARTITION(EA_WINAPI_PARTITION_DESKTOP)

		// The first column is the first type we try, while the second column is the second type.
		static const int kFolderTable[][2] = {
			0,                              0,                              // kSpecialDirectoryNone
			0,                              0,                              // kSpecialDirectoryTemp
			0,                              0,                              // kSpecialDirectoryOperatingSystem
			CSIDL_BITBUCKET,                0,                              // kSpecialDirectoryOperatingSystemTrash
			CSIDL_FONTS,                    0,                              // kSpecialDirectoryOperatingSystemFonts
			0,                              0,                              // kSpecialDirectoryCurrentApplication
			CSIDL_DESKTOPDIRECTORY,         CSIDL_COMMON_DESKTOPDIRECTORY,  // kSpecialDirectoryUserDesktop
			CSIDL_COMMON_DESKTOPDIRECTORY,  CSIDL_DESKTOPDIRECTORY,         // kSpecialDirectoryCommonDesktop
			CSIDL_APPDATA,                  CSIDL_COMMON_APPDATA,           // kSpecialDirectoryUserApplicationData
			CSIDL_COMMON_APPDATA,           CSIDL_APPDATA,                  // kSpecialDirectoryCommonApplicationData
			CSIDL_PERSONAL,                 CSIDL_COMMON_DOCUMENTS,         // kSpecialDirectoryUserDocuments
			CSIDL_COMMON_DOCUMENTS,         CSIDL_PERSONAL,                 // kSpecialDirectoryCommonDocuments
			CSIDL_MYMUSIC,                  CSIDL_COMMON_MUSIC,             // kSpecialDirectoryUserMusic
			CSIDL_COMMON_MUSIC,             CSIDL_MYMUSIC,                  // kSpecialDirectoryCommonMusic
			CSIDL_PROGRAMS,                 CSIDL_COMMON_PROGRAMS,          // kSpecialDirectoryUserProgramsShortcuts
			CSIDL_COMMON_PROGRAMS,          CSIDL_PROGRAMS,                 // kSpecialDirectoryCommonProgramsShortcuts
			CSIDL_MYPICTURES,               CSIDL_COMMON_PICTURES,          // kSpecialDirectoryUserPictures
			CSIDL_MYVIDEO,                  CSIDL_COMMON_VIDEO              // kSpecialDirectoryUserMovies
		};

		// Add some assert to help test if the user has modified the SpecialDirectory enumeration.
		EA_COMPILETIME_ASSERT(kSpecialDirectoryOperatingSystemTrash == 3);
		EA_COMPILETIME_ASSERT(kSpecialDirectoryUserMovies == 17);

		bool success = false;

		switch(specialDirectory)
		{
			case kSpecialDirectoryDevScratch:
			case kSpecialDirectoryTemp:
				success = (GetTempPathW(nPathCapacity, reinterpret_cast<LPWSTR>(pDirectory)) != 0);
				if(success)
					Path::EnsureTrailingSeparator(pDirectory, nPathCapacity);
				break;

			case kSpecialDirectoryOperatingSystem:
				success = (GetWindowsDirectoryW(reinterpret_cast<LPWSTR>(pDirectory), nPathCapacity) != 0);
				if(success)
					Path::EnsureTrailingSeparator(pDirectory, nPathCapacity);
				break;

			case kSpecialDirectoryCurrentApplication:
			{
				char buffer[EA::IO::kMaxDirectoryLength];

				#if EASTDC_VERSION_N >= 10901
					if(EA::StdC::GetCurrentProcessDirectory(buffer, EAArrayCount(buffer)) < (size_t)nPathCapacity)
					{
						EA::StdC::Strlcpy(pDirectory, buffer, nPathCapacity);
						success = true;
					}
				#else
					if(EA::StdC::GetCurrentProcessDirectory(buffer) < (size_t)nPathCapacity)
					{
						EA::StdC::Strlcpy(pDirectory, buffer, nPathCapacity);
						success = true;
					}
				#endif
				break;
			}

			case kSpecialDirectoryDownloadData:
				specialDirectory = kSpecialDirectoryUserDocuments;
				// Fall through

			case kSpecialDirectoryOperatingSystemTrash:
			case kSpecialDirectoryOperatingSystemFonts:
			case kSpecialDirectoryUserDesktop:
			case kSpecialDirectoryCommonDesktop:
			case kSpecialDirectoryUserApplicationData:
			case kSpecialDirectoryCommonApplicationData:
			case kSpecialDirectoryUserDocuments:
			case kSpecialDirectoryCommonDocuments:
			case kSpecialDirectoryUserMusic:
			case kSpecialDirectoryCommonMusic:
			case kSpecialDirectoryUserProgramsShortcuts:
			case kSpecialDirectoryCommonProgramsShortcuts:
			case kSpecialDirectoryUserPictures:
			case kSpecialDirectoryUserMovies:
			case kSpecialDirectoryNone:
			default:
			{
				EA_ASSERT((specialDirectory >= 0) && (specialDirectory < EAArrayCount(kFolderTable)));
				if((specialDirectory >= 0) && (specialDirectory < EAArrayCount(kFolderTable)))
				{
					const int typeFlags = (specialDirectory == kSpecialDirectoryOperatingSystemTrash) ? SHGFP_TYPE_DEFAULT : SHGFP_TYPE_CURRENT;
					const int idFlags   = bEnsureDirectoryExistence ? CSIDL_FLAG_CREATE : 0;
					int       id        = kFolderTable[specialDirectory][0];

					if(id && SUCCEEDED(SHGetFolderPathW(NULL, id | idFlags, NULL, typeFlags, reinterpret_cast<LPWSTR>(pDirectory))))
						success = true;
					else
					{
						id = kFolderTable[specialDirectory][1];

						if(id && SUCCEEDED(SHGetFolderPathW(NULL, id | idFlags, NULL, typeFlags, reinterpret_cast<LPWSTR>(pDirectory))))
							success = true;
					}

					if(success)
					{
						const size_t nLength = wcslen(reinterpret_cast<const wchar_t*>(pDirectory));

						// Check if the path ends in a separator and add it if not.
						if(nLength && !IsFilePathSeparator(pDirectory[nLength - 1]))
						{
							if(nLength >= (kMaxPathLength - 1))
								success = false; // We don't have enough room to add the path separator, so fail the call.
							else
							{
								pDirectory[nLength]     = EA_FILE_PATH_SEPARATOR_16;
								pDirectory[nLength + 1] = 0;
							}
						}
					}
				}
			}
			break;
		}

		// To do: We have a small API compliance issue here in that as of this writing,     
		//        the function is documented to not modify pDirectory if the function fails.
		//        However, that is necessarily not the case here.
		if(success && bEnsureDirectoryExistence && !Directory::Exists(pDirectory)) // If it succeeded but the directory couldn't be made...
			success = false;
		if(success)
			return (int)EA::StdC::Strlen(pDirectory);
		return -1;

	#elif defined(EA_PLATFORM_XBOXONE) || defined(EA_PLATFORM_XBSX)
		bool success = false;

		switch(specialDirectory)
		{
			case kSpecialDirectoryDevScratch: // This is usable only in dev kits and not retail kits.
				// We have the option of returning D:\ or G:\ here. The D drive is called the "scratch drive" by Microsoft 
				// and is available only to dev kits. It's read-write and doesn't have size limitations. The G drive is 
				// the application drive and is read-write to dev kits but read-only to retail kits. Since both of these
				// drives are writable only to dev kits, it's somewhat arbitrary which use use here.
				success = ((uint32_t)EA::StdC::Strlcpy(pDirectory, "D:\\", nPathCapacity) < nPathCapacity);
				break;

			case kSpecialDirectoryDownloadData: // Fall through. Currently download data and temp are the same.
			case kSpecialDirectoryTemp:
				success = ((uint32_t)EA::StdC::Strlcpy(pDirectory, "T:\\", nPathCapacity) < nPathCapacity);
				break;

			case kSpecialDirectoryCurrentApplication:
			{
				char buffer[EA::IO::kMaxDirectoryLength];

				#if EASTDC_VERSION_N >= 10901
					if(EA::StdC::GetCurrentProcessDirectory(buffer, EAArrayCount(buffer)) < (size_t)nPathCapacity)
					{
						EA::StdC::Strlcpy(pDirectory, buffer, nPathCapacity);
						success = true;
					}
				#else
					if(EA::StdC::GetCurrentProcessDirectory(buffer) < (size_t)nPathCapacity)
					{
						EA::StdC::Strlcpy(pDirectory, buffer, nPathCapacity);
						success = true;
					}
				#endif
				break;
			}

			case kSpecialDirectoryNone:
			case kSpecialDirectoryOperatingSystem:
			case kSpecialDirectoryOperatingSystemTrash:
			case kSpecialDirectoryOperatingSystemFonts:
			case kSpecialDirectoryUserDesktop:
			case kSpecialDirectoryCommonDesktop:
			case kSpecialDirectoryUserApplicationData:
			case kSpecialDirectoryCommonApplicationData:
			case kSpecialDirectoryUserDocuments:
			case kSpecialDirectoryCommonDocuments:
			case kSpecialDirectoryUserMusic:
			case kSpecialDirectoryCommonMusic:
			case kSpecialDirectoryUserProgramsShortcuts:
			case kSpecialDirectoryCommonProgramsShortcuts:
			case kSpecialDirectoryUserPictures:
			case kSpecialDirectoryUserMovies:
			default:    // We provide a default value.
				success = ((uint32_t)EA::StdC::Strlcpy(pDirectory, "G:\\", nPathCapacity) < nPathCapacity);
				break;
		}

		if(success && bEnsureDirectoryExistence && !Directory::Exists(pDirectory)) // If it succeeded but the directory couldn't be made...
			success = false;

		if(success)
			return (int)EA::StdC::Strlen(pDirectory);
		return -1;

	#elif EAIO_USE_UNIX_IO || defined(EA_PLATFORM_APPLE) || defined(EA_PLATFORM_SONY)
		// better just call 8-bit version and convert, as we don't have a 
		// 16-bit non-eastl::string sprintf available any more.
		
		char directory8[kMaxDirectoryLength];

		if(GetSpecialDirectory(specialDirectory, directory8, bEnsureDirectoryExistence, kMaxDirectoryLength) > 0)
		{
			const int intendedStrlen = EA::StdC::Strlcpy(pDirectory, directory8, nPathCapacity);

			if((intendedStrlen >= 0) && (intendedStrlen < (int)nPathCapacity))
				return intendedStrlen;
		}

		pDirectory[0] = 0;
		return -1;

	#else

		// Bug Frostbite Foundation for this when you need it for another platform.
		// Much of this functionality is not possible on console platforms but 
		// is possible on desktop platforms such as Windows, MacOS, Linux.
		// We can possibly allow the user to set these directories and then this
		// function could return what the user set.
		EA_UNUSED(specialDirectory);
		EA_UNUSED(bEnsureDirectoryExistence);
		EA_UNUSED(nPathCapacity);

		return GetTempDirectory(pDirectory);

	#endif
}

EAIO_API int GetSpecialDirectory(SpecialDirectory specialDirectory, char* pDirectory, 
								 bool bEnsureDirectoryExistence, uint32_t nPathCapacity)
{
	#if defined(EA_PLATFORM_NX)
		bool success = false;

		switch(specialDirectory)
		{
			case kSpecialDirectoryDevScratch:			// host:/ is not usable in retail titles.
			case kSpecialDirectoryCurrentApplication:
				success = (EA::StdC::Strlcpy(pDirectory, "host:/", nPathCapacity) < nPathCapacity);
				break;

			case kSpecialDirectoryDownloadData:
				success = (EA::StdC::Strlcpy(pDirectory, "download:/", nPathCapacity) < nPathCapacity);
				break;

			case kSpecialDirectoryTemp:
				success = (EA::StdC::Strlcpy(pDirectory, "tmp:/", nPathCapacity) < nPathCapacity);
				break;

			case kSpecialDirectoryUserApplicationData:
			case kSpecialDirectoryCommonApplicationData:
			case kSpecialDirectoryNone:
			case kSpecialDirectoryOperatingSystem:
			case kSpecialDirectoryOperatingSystemTrash:
			case kSpecialDirectoryOperatingSystemFonts:
			case kSpecialDirectoryUserDesktop:
			case kSpecialDirectoryCommonDesktop:
			case kSpecialDirectoryUserDocuments:
			case kSpecialDirectoryCommonDocuments:
			case kSpecialDirectoryUserMusic:
			case kSpecialDirectoryCommonMusic:
			case kSpecialDirectoryUserProgramsShortcuts:
			case kSpecialDirectoryCommonProgramsShortcuts:
			case kSpecialDirectoryUserPictures:
			case kSpecialDirectoryUserMovies:
			default:    // We provide a default value.
				success = (EA::StdC::Strlcpy(pDirectory, "rom:/", nPathCapacity) < nPathCapacity);
				break;
		}

		if(success && bEnsureDirectoryExistence && !Directory::Exists(pDirectory)) // If it succeeded but the directory couldn't be made...
			success = false;

		if(success)
			return (int)EA::StdC::Strlen(pDirectory);
		return -1;
	#elif EAIO_USE_UNIX_IO || defined(EA_PLATFORM_APPLE)
		#if defined(EA_PLATFORM_APPLE)
			// To consider: Use the following instead of some the code below, as the following directly uses iOS APIs.
			//int appleGetTempDirectory(char16_t* pDirectory, uint32_t nPathCapacity);
			//int appleGetTempDirectory(char*  pDirectory, uint32_t nPathCapacity);
			//int appleGetHomeDirectory(char16_t* pDirectory, uint32_t nPathCapacity);      
			//int appleGetHomeDirectory(char*  pDirectory, uint32_t nPathCapacity);
    
            if(specialDirectory == kSpecialDirectoryUserApplicationData)
            {
                if (int len = appleGetHomeDirectory(pDirectory, nPathCapacity))
                {
                    if (EA::IO::Path::GetHasTrailingSeparator(pDirectory, len))
                        return len;
                    else if(len < nPathCapacity)
                    {
                        pDirectory[len] = '/';
                        return len + 1;
                    }
                }
            }
            else if(specialDirectory == kSpecialDirectoryTemp)
            {
                if (int len = appleGetTempDirectory(pDirectory, nPathCapacity))
                {
                    if (EA::IO::Path::GetHasTrailingSeparator(pDirectory, len))
                        return len;
                    else if(len < nPathCapacity)
                    {
                        pDirectory[len] = '/';
                        return len + 1;
                    }
                }
            }
		#elif defined(EA_PLATFORM_ANDROID)
			// Java getExternalStorageDirectory()
			// Java getExternalStoragePublicDirectory()
			// Java getExternalFilesDir()
			// http://developer.android.com/guide/topics/data/data-storage.html#filesExternal
			// AssetManagerJNI::GetDataRoot();
			
			// The fileroot allows one to write files belonging to the application which will be removed upon uninstall
			// This is not the same thing as the application directory which is where the apk would live nor is it the same
			// as the read only asset directory deployed with an apk since this directory is writeable
			if(specialDirectory == kSpecialDirectoryUserApplicationData)
			{
				const char* appFileRoot = AssetManagerJNI::GetFileRoot();
				if (appFileRoot)
				{
					if (EA::IO::Path::GetHasTrailingSeparator(appFileRoot, EA::StdC::Strlen(appFileRoot)))
						return (int)EA::StdC::Strlcpy(pDirectory, appFileRoot, nPathCapacity);
					else
						return EA::StdC::Snprintf(pDirectory, nPathCapacity, "%s/", appFileRoot);
				}
			}
		#endif

		char        buffer[EA::IO::kMaxDirectoryLength];
		const char* home    = getenv("HOME");
		const char* shared  = "/Users/Shared";  // TODO: this is OSX-specific
		const char* tmp     = getenv("TMPDIR");

		if(!tmp || tmp[0] == '\0')
			tmp = "/tmp/";
		if(!home || home[0] == '\0')
			home = tmp;

		switch (specialDirectory)
		{
			case kSpecialDirectoryNone:
				return -1;
				
			case kSpecialDirectoryDevScratch:
			case kSpecialDirectoryTemp:
				if(EA::IO::Path::GetHasTrailingSeparator(tmp, EA::StdC::Strlen(tmp)))
					return (int)EA::StdC::Strlcpy(pDirectory, tmp, nPathCapacity);
				else
					return EA::StdC::Snprintf(pDirectory, nPathCapacity, "%s/", tmp);
				
			case kSpecialDirectoryOperatingSystem:
				return (int)EA::StdC::Strlcpy(pDirectory, "/System/", nPathCapacity);

			case kSpecialDirectoryOperatingSystemTrash:
				// Return per-user trash.
				return EA::StdC::Snprintf(pDirectory, nPathCapacity, "%s/.Trash/", home);

			case kSpecialDirectoryOperatingSystemFonts:
				// there should really be kSpecialDirectoryUserFonts too.
				return (int)EA::StdC::Strlcpy(pDirectory, "/Library/Fonts/", nPathCapacity);
			
			case kSpecialDirectoryCurrentApplication:
				#if EASTDC_VERSION_N >= 10901
					if(EA::StdC::GetCurrentProcessDirectory(buffer, EAArrayCount(buffer)) < (size_t)nPathCapacity)
						return (int)EA::StdC::Strlcpy(pDirectory, buffer, nPathCapacity);
				#else
					if(EA::StdC::GetCurrentProcessDirectory(buffer) < (size_t)nPathCapacity)
						return (int)EA::StdC::Strlcpy(pDirectory, buffer, nPathCapacity);
				#endif
				// Else fall through to failure.

			case kSpecialDirectoryUserDesktop:
				return EA::StdC::Snprintf(pDirectory, nPathCapacity, "%s/Desktop/", home);

			case kSpecialDirectoryCommonDesktop:
				return EA::StdC::Snprintf(pDirectory, nPathCapacity, "%s/Desktop/", shared);

			case kSpecialDirectoryUserApplicationData:
				return EA::StdC::Snprintf(pDirectory, nPathCapacity, "%s/Library/", home);

			case kSpecialDirectoryCommonApplicationData:
				return EA::StdC::Snprintf(pDirectory, nPathCapacity, "%s/Library/", shared);

			case kSpecialDirectoryDownloadData:
			case kSpecialDirectoryUserDocuments:
				return EA::StdC::Snprintf(pDirectory, nPathCapacity, "%s/Documents/", home);

			case kSpecialDirectoryCommonDocuments:
				return EA::StdC::Snprintf(pDirectory, nPathCapacity, "%s/Documents/", shared);

			case kSpecialDirectoryUserMusic:
				return EA::StdC::Snprintf(pDirectory, nPathCapacity, "%s/Music/", home);

			case kSpecialDirectoryCommonMusic:
				return EA::StdC::Snprintf(pDirectory, nPathCapacity, "%s/Music/", shared);

			case kSpecialDirectoryUserProgramsShortcuts:    // shortcuts is a windows-ism, assume they mean the apps directory
				return EA::StdC::Snprintf(pDirectory, nPathCapacity, "%s/Applications/", home);

			case kSpecialDirectoryCommonProgramsShortcuts:
				return (int)EA::StdC::Strlcpy(pDirectory, "/Applications/", nPathCapacity);

			case kSpecialDirectoryUserPictures:
				return EA::StdC::Snprintf(pDirectory, nPathCapacity, "%s/Pictures/", home);

			case kSpecialDirectoryUserMovies:
				return EA::StdC::Snprintf(pDirectory, nPathCapacity, "%s/Movies/", home);
		}

		EA_UNUSED(bEnsureDirectoryExistence); // Avoid compiler warnings.

		return -1;

	#elif defined(EA_PLATFORM_SONY)
		bool success = false;

		switch(specialDirectory)
		{
			case kSpecialDirectoryDevScratch: // /hostapp/ and /host/ are not usable in retail titles.
#if defined(EA_PLATFORM_PS4)
				success = (EA::StdC::Strlcpy(pDirectory, "/hostapp/", nPathCapacity) < nPathCapacity);
#elif defined(EA_PLATFORM_PS5)
				success = (EA::StdC::Strlcpy(pDirectory, "/host/", nPathCapacity) < nPathCapacity);
#endif
				break;

			case kSpecialDirectoryDownloadData:
				success = (EA::StdC::Strlcpy(pDirectory, "/download0/", nPathCapacity) < nPathCapacity);
				break;

			case kSpecialDirectoryTemp:
				success = (GetTempDirectory(pDirectory, nPathCapacity) < nPathCapacity);
				break;

			case kSpecialDirectoryCurrentApplication: // For the case of running a devkit app from a host PC, /app0 is the working directory and not necessary the directory the app is in.
				success = (EA::StdC::Strlcpy(pDirectory, "/app0/", nPathCapacity) < nPathCapacity);
				break;

			case kSpecialDirectoryUserApplicationData:
			case kSpecialDirectoryCommonApplicationData: // Sony states that /data/ and /devlog/app/ are supported by development kits only and not retail titles.
#if defined(EA_PLATFORM_PS4)
				success = (EA::StdC::Strlcpy(pDirectory, "/data/", nPathCapacity) < nPathCapacity);
#elif defined(EA_PLATFORM_PS5)
				success = (EA::StdC::Strlcpy(pDirectory, "/devlog/app/", nPathCapacity) < nPathCapacity);
#endif
				break;

			case kSpecialDirectoryNone:
			case kSpecialDirectoryOperatingSystem:
			case kSpecialDirectoryOperatingSystemTrash:
			case kSpecialDirectoryOperatingSystemFonts:
			case kSpecialDirectoryUserDesktop:
			case kSpecialDirectoryCommonDesktop:
			case kSpecialDirectoryUserDocuments:
			case kSpecialDirectoryCommonDocuments:
			case kSpecialDirectoryUserMusic:
			case kSpecialDirectoryCommonMusic:
			case kSpecialDirectoryUserProgramsShortcuts:
			case kSpecialDirectoryCommonProgramsShortcuts:
			case kSpecialDirectoryUserPictures:
			case kSpecialDirectoryUserMovies:
			default:    // We provide a default value.
				success = (EA::StdC::Strlcpy(pDirectory, "/app0/", nPathCapacity) < nPathCapacity);
				break;
		}

		if(success && bEnsureDirectoryExistence && !Directory::Exists(pDirectory)) // If it succeeded but the directory couldn't be made...
			success = false;

		if(success)
			return (int)EA::StdC::Strlen(pDirectory);
		return -1;

	#else
		// Call the 16 bit version.
		EA::IO::Path::PathString16 path16;
		path16.resize(nPathCapacity);

		if(GetSpecialDirectory(specialDirectory, &path16[0], bEnsureDirectoryExistence, nPathCapacity) > 0)
		{
			const int intendedStrlen = EA::StdC::Strlcpy(pDirectory, path16.c_str(), nPathCapacity);

			if((intendedStrlen >= 0) && (intendedStrlen < (int)nPathCapacity))
				return intendedStrlen;
			// Else fall through to failure.
		}
		
		return -1;
	#endif
}



///////////////////////////////////////////////////////////////////////////////
// Reserved Windows path names
//
// These reserved words (alone or with any extension) cannot be used 
// as the name of a file, directory, server, or volume. We define these
// outside #ifdef EA_PLATFORM_WINDOWS because this function is meant to 
// be able to test Win32 paths while running under Unix, etc.

static const char16_t kCON[]   = { 'c', 'o', 'n', 0 };
static const char16_t kPRN[]   = { 'p', 'r', 'n', 0 };
static const char16_t kAUX[]   = { 'a', 'u', 'x', 0 };
static const char16_t kCLOCK[] = { 'c', 'l', 'o', 'c', 'k', '$', 0 };
static const char16_t kNUL[]   = { 'n', 'u', 'l', 0 };
static const char16_t kCOM1[]  = { 'c', 'o', 'm', '1', 0 };
static const char16_t kCOM2[]  = { 'c', 'o', 'm', '2', 0 };
static const char16_t kCOM3[]  = { 'c', 'o', 'm', '3', 0 };
static const char16_t kCOM4[]  = { 'c', 'o', 'm', '4', 0 };
static const char16_t kCOM5[]  = { 'c', 'o', 'm', '5', 0 };
static const char16_t kCOM6[]  = { 'c', 'o', 'm', '6', 0 };
static const char16_t kCOM7[]  = { 'c', 'o', 'm', '7', 0 };
static const char16_t kCOM8[]  = { 'c', 'o', 'm', '8', 0 };
static const char16_t kCOM9[]  = { 'c', 'o', 'm', '9', 0 };
static const char16_t kLPT1[]  = { 'l', 'p', 't', '1', 0 };
static const char16_t kLPT2[]  = { 'l', 'p', 't', '2', 0 };
static const char16_t kLPT3[]  = { 'l', 'p', 't', '3', 0 };
static const char16_t kLPT4[]  = { 'l', 'p', 't', '4', 0 };
static const char16_t kLPT5[]  = { 'l', 'p', 't', '5', 0 };
static const char16_t kLPT6[]  = { 'l', 'p', 't', '6', 0 };
static const char16_t kLPT7[]  = { 'l', 'p', 't', '7', 0 };
static const char16_t kLPT8[]  = { 'l', 'p', 't', '8', 0 };
static const char16_t kLPT9[]  = { 'l', 'p', 't', '9', 0 };

static const char16_t* kReservedNamesWin32[] =
{
	kCON,  kPRN,  kAUX,  kCLOCK, kNUL,
	kCOM1, kCOM2, kCOM3, kCOM4,  kCOM5,
	kCOM6, kCOM7, kCOM8, kCOM9,  kLPT1,
	kLPT2, kLPT3, kLPT4, kLPT5,  kLPT6,
	kLPT7, kLPT8, kLPT9,
};

// Returns true if Filename is a reserved name
static bool IsFileNameReserved(const EA::IO::Path::PathString16& FileName)
{
	// Lower case
	EA::IO::Path::PathString16 sCompareName(FileName);
	sCompareName.make_lower();

	// Trim off any spaces
	sCompareName.rtrim();

	// Trim any extension
	const eastl_size_t nDotPosition(sCompareName.find('.'));
	if (nDotPosition != EA::IO::Path::PathString16::npos)
		sCompareName.erase(nDotPosition, (sCompareName.length() - nDotPosition));

	for (auto& reservedName : kReservedNamesWin32)
	{
		if (sCompareName == reservedName)
			return true;
	}

	return false;
}

// Returns a string (sCurrentComponent) of the characters from current position up to but not including the next path separator char.
// If there is no '/' (or platform-appropriate path separator), returns the rest of the string.
// Sets nCurrentPosition to be one past the '/' char or one past the last char in the string.
// May possibly return an empty string, in the case of two successive '/' chars.
// Returns true if there were any charaters left to read.
// Returns false if the input position is last the end of the string.
//
static bool GetNextPathComponent(const EA::IO::Path::PathString16& sInputPath, EA::IO::Path::PathString16& sCurrentComponent, eastl_size_t& nCurrentPosition)
{
	const eastl_size_t nInputPathLength = sInputPath.length();

	if(nCurrentPosition < nInputPathLength)
	{
		eastl_size_t i;

		for(i = nCurrentPosition; i < nInputPathLength; i++)
		{
			if(IsFilePathSeparator(sInputPath[i]))
			{
				sCurrentComponent.assign(sInputPath, nCurrentPosition, i - nCurrentPosition);
				nCurrentPosition = i+1;
				return true;
			}
		}

		sCurrentComponent.assign(sInputPath, nCurrentPosition, i-nCurrentPosition);
		nCurrentPosition = i;
		return true;
   }

   return false;
}

inline static bool IsFileNameCharValidImpl(char32_t c)
{
	// Character restrictions are enforced for all FileSystems

	// Windows documentation says that chars 0-31 are invalid, but curiously, if you
	// type such characters into the Windows 2000 file browser for a name, it accepts them.
	// Don't be fooled. It actually isn't accepting them, but is instead converting them
	// to alternative Unicode equivalents for the same glyphs before accepting them.

	return ((c != '<') && (c != '>') && (c != ':') && (c != '\\') &&
			(c != '/') && (c != '\"') && (c != '|') && (c != '*') && (c != '?') && (c > 31));
}

///////////////////////////////////////////////////////////////////////////////
// IsFileNameCharValid
//
// Returns true if the input character can always be used in a path file name.
// You cannot use this function one-by-one on a file name to determine if the file
// name is valid, as there are special combination cases that make this so.
//
bool IsFileNameCharValid(char32_t c, FileSystem fileSystemType)
{
	// Character restrictions are enforced for all FileSystems
	
	switch (fileSystemType)
	{
		case kFileSystemUnix:       // General Unix paths
		case kFileSystemISO9660:
		case kFileSystemJoliet:
		case kFileSystemUDF:
		case kFileSystemNone:
			return (IsFileNameCharValidImpl(c));

		case kFileSystemWindows:
		case kFileSystemFAT:
		case kFileSystemFATX:
		case kFileSystemFAT32:
		case kFileSystemNTFS:
		case kFileSystemUDFX:
		default:
			return (IsFileNameCharValidImpl(c));
	}

	// return true;  // The above switch handles all possibilities.
}



///////////////////////////////////////////////////////////////////////////////
// IsFileNameStringValid
//
// Returns true if the input file name is valid for the file system.
//
bool IsFileNameStringValid(const char32_t* pName, FileSystem fileSystemType)
{
	char16_t name16[kMaxPathLength];
	EA::StdC::Strlcpy(name16, pName, kMaxPathLength);
	return IsFileNameStringValid(name16, fileSystemType); 
}

bool IsFileNameStringValid(const char* pName, FileSystem fileSystemType)
{
	char16_t name16[kMaxPathLength];
	EA::StdC::Strlcpy(name16, pName, kMaxPathLength);
	return IsFileNameStringValid(name16, fileSystemType); 
}

bool IsFileNameStringValid(const char16_t* pName, FileSystem fileSystemType)
{
	using namespace EA::IO::Path;

	PathString16 sName(pName);

	if(sName.length() < (eastl_size_t)kMaxFileNameLength) // Problem: This value is tied to the compilation target and not the fileSystemType.
	{
		//Check for bad characters. This is an OK technique even for multi-byte locales.
		for(eastl_size_t i(0), iEnd(sName.length()); i < iEnd; i++)
		{
			const char16_t c = sName[i];  //We don't have to worry about multi-byte text because we use UTF8 which is not hostile to what we do here.

			if(!IsFileNameCharValid(c, fileSystemType))
				return false;
		}

		// Do reserved word tests
		if((fileSystemType == kFileSystemWindows) || 
		   (fileSystemType == kFileSystemFAT)     ||
		   (fileSystemType == kFileSystemFAT32)   ||
		   (fileSystemType == kFileSystemNTFS))
		{ 
			PathString16 sCurrentComponentLowerCaseMinusExtension(sName);
			sCurrentComponentLowerCaseMinusExtension.make_lower();

			// Remove anything past a dot because that's insignificant to Windows when testing reserved words.
			const eastl_size_t nDotPosition(sCurrentComponentLowerCaseMinusExtension.find('.'));
			if(nDotPosition != PathString16::npos)
				sCurrentComponentLowerCaseMinusExtension.erase(nDotPosition);

			// Also trim off any spaces at the end because those are insignificant to Windows when testing reserved words.
			sCurrentComponentLowerCaseMinusExtension.rtrim();

			for(eastl_size_t i(0); i < (sizeof(kReservedNamesWin32) / sizeof(kReservedNamesWin32[0])); i++)
			{
				if(sCurrentComponentLowerCaseMinusExtension == kReservedNamesWin32[i])
					return false;
			}
		}
		else if (fileSystemType == kFileSystemUnix)
		{
			// Reserved word tests
			if (IsFileNameReserved(sName))
				return false;
		}

		// Components that consist of just '.' and/or ' ' chars are illegal.
		// The Win32 documentation doesn't specifically state that these combinations
		// are illegal, but in practice they are. Thus, "...", "   ", and ". ." are
		// all invalid.
		const char16_t pDotSpace[] = { '.', ' ', 0 };
		if(sName.find_first_not_of(pDotSpace) == PathString16::npos)
			return false;

		return true;
	}

	return false;
}

bool IsDirectoryNameStringValid(const char* pName, FileSystem fileSystemType)
{
	return IsFileNameStringValid(pName, fileSystemType); // This works for all currently supported platforms.
}

bool IsDirectoryNameStringValid(const char32_t* pName, FileSystem fileSystemType)
{
	return IsFileNameStringValid(pName, fileSystemType); // This works for all currently supported platforms.
}

bool IsDirectoryNameStringValid(const char16_t* pName, FileSystem fileSystemType)
{
	return IsFileNameStringValid(pName, fileSystemType); // This works for all currently supported platforms.
}


///////////////////////////////////////////////////////////////////////////////
// IsFilePathStringValid
//
// Here are some helper URLs from Microsoft, though they are actually not complete:
//    http://msdn.microsoft.com/library/default.asp?url=/library/en-us/dnfiles/html/msdn_longfile.asp
//    http://msdn.microsoft.com/library/default.asp?url=/library/en-us/fileio/storage_7mn9.asp
//    http://msdn.microsoft.com/library/default.asp?url=/library/en-us/fileio/volref_6wfi.asp
//    http://msdn.microsoft.com/library/default.asp?url=/library/en-us/vccore98/HTML/_core_device_names_for_windows.asp
//
// Recognized file system types include:
//     Unix:      UNIX, 
//     Macintosh: HFS, 
//     Windows:   FAT (same as VFAT for Windows 95 and later), HPFS, NTFS 
//     CDROM:     UDF, CDFS (Windows), Joliet, Rockridge (Unix compatible)

bool IsFilePathStringValid(const char32_t* pPath, FileSystem fileSystemType)
{
	char16_t path16[kMaxPathLength];
	EA::StdC::Strlcpy(path16, pPath, kMaxPathLength);
	return IsFilePathStringValid(path16, fileSystemType);
}

bool IsFilePathStringValid(const char* pPath, FileSystem fileSystemType)
{
	char16_t path16[kMaxPathLength];
	EA::StdC::Strlcpy(path16, pPath, kMaxPathLength);
	return IsFilePathStringValid(path16, fileSystemType);
}


bool IsFilePathStringValid(const char16_t* pPath, FileSystem fileSystemType)
{
	using namespace EA::IO::Path;

	bool           bReturnValue = false;
	PathString16   sPath(pPath);

	// Do path length tests
	#if defined(EA_PLATFORM_WINDOWS)
		if(sPath.length() < 3)                  // A length less than 3 chars cannot be a full path, as Windows requires at least a drive or UNC prefix.
			return false;

		if(sPath.find(EA_CHAR16("\\\\?\\UNC\\")) == 0)    // Under Windows, if a path is prefixed with `\\?\UNC\` then it has no length limit.
			sPath.erase(2, 6);                            // Convert (e.g.) `\\?\UNC\server\volume\Temp\temp.txt` to `\\server\volume\Temp\temp.txt`.
		else if(sPath.find(EA_CHAR16("\\\\?\\")) == 0)    // Under Windows, if a path is prefixed with `\\?\` then it has no length limit.
			sPath.erase(0, 4);                            // Convert (e.g.) `\\?\C:\Temp\temp.txt` to `C:\Temp\temp.txt`.
		else if(sPath.length() > kMaxPathLength)
			return false;
	#else
		if(sPath.length() < 1)                  // We treat a path of "/" as valid, as it is a valid directory path.
			return false;

		if(sPath.length() > (eastl_size_t)kMaxPathLength)     // Strictly speaking, some Unix systems allow more than this many characters.
			return false;
	#endif

	// Do validation per file system
	if((fileSystemType == kFileSystemWindows) || 
	   (fileSystemType == kFileSystemFAT)     ||
	   (fileSystemType == kFileSystemFAT32)   ||
	   (fileSystemType == kFileSystemNTFS))
	{ 
		const char16_t kReservedCharactersWin32[] = { '<', '>', ':', '\"', '|', '*', '?', 0 }; // We don't include '/' and '\' because we deal with them separately.

		// Set the return value to true until proven false for the rest of this section.
		eastl_size_t nPosition = 0;
		bReturnValue = true;

		//Go through each directory and the file name in the path and see if 
		//it has any of the above reserved names. Note that directory names
		//can't be "com1.txt" any more than file names can't be "com1.txt".
		const char16_t kBackSlashBackSlash[] = { '\\', '\\', 0 };

		if(sPath.find(kBackSlashBackSlash) == 0) // If it is a UNC path... (\\<server>\<volume>\<dir path>). '/' is not accepted as part of the UNC path prefix.
		{
			const eastl_size_t nPosition2 = 1;
			const eastl_size_t nPosition3 = sPath.find('\\', 2);
			const eastl_size_t nPosition4 = sPath.find('\\', nPosition3 + 1); // We know from earlier that there are at least 4 '\' chars.

			if((nPosition3 > (nPosition2 + 1)) && (nPosition4 > (nPosition3 + 1)))
			{
				const PathString16 sServer(sPath, nPosition2 + 1, nPosition3 - nPosition2);
				const PathString16 sVolume(sPath, nPosition3 + 1, nPosition4 - nPosition3);

				// We're a little conservative with the server and volume name checks here.
				// Their limitations don't appear to be well documented, so we err on acceptance.
				if(sServer.find_first_of(kReservedCharactersWin32) != PathString16::npos)
					bReturnValue = false;
				else if(sVolume.find_first_of(kReservedCharactersWin32) != PathString16::npos)
					bReturnValue = false;
				else
				{
					PathString16 sServerLowerCaseMinusExtension(sServer);
					PathString16 sVolumeLowerCaseMinusExtension(sVolume);

					sServerLowerCaseMinusExtension.make_lower();
					const eastl_size_t nServerDotPosition(sServerLowerCaseMinusExtension.find('.'));    
					if(nServerDotPosition != PathString16::npos)
						sServerLowerCaseMinusExtension.erase(nServerDotPosition);

					sVolumeLowerCaseMinusExtension.make_lower();
					const eastl_size_t nVolumeDotPosition(sVolumeLowerCaseMinusExtension.find('.'));    
					if(nVolumeDotPosition != PathString16::npos)
						sVolumeLowerCaseMinusExtension.erase(nVolumeDotPosition);

					for(size_t i(0); i < sizeof(kReservedNamesWin32) / sizeof(kReservedNamesWin32[0]); i++)
					{
						if(sServerLowerCaseMinusExtension == kReservedNamesWin32[i])
						{
							bReturnValue = false;
							break;
						}

						if(sVolumeLowerCaseMinusExtension == kReservedNamesWin32[i])
						{
							bReturnValue = false;
							break;
						}
					}
					nPosition = nPosition4 + 1;
				}
			}
			else
				bReturnValue = false;
		}
		else
		{
			if((sPath.length() >= 3) &&     // Similar to iswalpha(sPath[0])
			   ((sPath[0] >= 'a' && sPath[0] <= 'z') || (sPath[0] >= 'A' && sPath[0] <= 'Z')) && // If is a Windows drive letter...
			   (sPath[1] == ':') &&
			   (sPath[2] == '\\'))       // '/' (as opposed to '\') is allowed as directory separators but not after "C:". That's Microsoft for ya.
			{
				nPosition = 3;
			}
		}

		if(bReturnValue) // If the code directly above found everything OK so far...
		{
			PathString16 sCurrentComponent;

			while(bReturnValue && GetNextPathComponent(sPath, sCurrentComponent, nPosition))
			{
				if(bReturnValue)
				{
					if(sCurrentComponent.empty() || (sCurrentComponent.length() > (eastl_size_t)kMaxFileNameLength))
					{
						bReturnValue = false;
						break;
					}
				}

				// Do reserved word tests
				if(bReturnValue)
				{
					PathString16 sCurrentComponentLowerCaseMinusExtension(sCurrentComponent);

					sCurrentComponentLowerCaseMinusExtension.make_lower();
					const eastl_size_t nDotPosition(sCurrentComponentLowerCaseMinusExtension.find('.'));

					if(nDotPosition != PathString16::npos)
						sCurrentComponentLowerCaseMinusExtension.erase(nDotPosition);

					for(size_t i(0); i < sizeof(kReservedNamesWin32) / sizeof(kReservedNamesWin32[0]); i++)
					{
						if(sCurrentComponentLowerCaseMinusExtension == kReservedNamesWin32[i])
						{
							bReturnValue = false;
							break;
						}
					}

					if(bReturnValue)
					{
						// Components that consist of just '.' and/or ' ' chars are illegal.
						// The Win32 documentation doesn't specifically state that these combinations
						// are illegal, but in practice they are. Thus, "...", "    ", and ". ." are
						// all invalid.
						char16_t kDotSpace[] = { '.', ' ', 0 };

						if(sCurrentComponent.find_first_not_of(kDotSpace) == PathString16::npos)
							bReturnValue = false;      
					}
				}

				// Do reserved character tests
				if(bReturnValue)
				{
					// Windows documentation says that chars 0-31 are invalid, but curiously, if you
					// type such characters into the Windows 2000 file browser for a name, it accepts them.
					// Don't be fooled. It actually isn't accepting them, but is instead converting them
					// to alternative Unicode equivalents for the same glyphs before accepting them. 

					for(char16_t c(0); c <= 31; c++)
					{
						if(sCurrentComponent.find(c) != PathString16::npos)
							bReturnValue = false;
					}

					if(bReturnValue)
					{
						if(sCurrentComponent.find_first_of(kReservedCharactersWin32) != PathString16::npos)
							bReturnValue = false;
					}
				}
			}
		}
	}
	else if (fileSystemType == kFileSystemUnix)
	{
		// Extract path components and validate

		bReturnValue = true;
		auto endIter = sPath.end();
		do
		{
			// Extract Component
			const auto pathIter = FindComponentRvs(sPath.begin(), endIter);
			PathString16 tmpName(pathIter, endIter);

			if (!GetHasTrailingSeparator(tmpName))
			{
				// File Name
				const bool bValidFileName = IsFileNameStringValid(tmpName.begin(), fileSystemType);

				if (!bValidFileName)
				{
					bReturnValue = false;
					break;
				}
			}
			else
			{
				// Directory
				if (tmpName.length() > 1)				// Skip if root directory seperator "/"
				{
					tmpName.pop_back();					// Remove the seperator
					EA_ASSERT(GetHasTrailingSeparator(tmpName) == false);	//Not expecting another seperator at this point

					const bool bValidDirName = IsDirectoryNameStringValid(tmpName.begin(), fileSystemType);

					if (!bValidDirName)
					{
						bReturnValue = false;
						break;
					}
				}
			}

			endIter = pathIter;

		} while (endIter != sPath.begin());
	}
	else
	{
		EA_FAIL_MESSAGE("IsFilePathStringValid: Not implemented for the given file system.");
		return true; // Play it safe and return true. If we were using this function to implement in a security-conscious way, we might instead choose to return false.
	}

   return bReturnValue;
}

#if EA_WCHAR_UNIQUE

	#ifdef EAIO_UNICODE_CONST_CHAR_PTR_CAST
	#undef EAIO_UNICODE_CONST_CHAR_PTR_CAST
	#endif
	
	#ifdef EAIO_UNICODE_CHAR_PTR_CAST
	#undef EAIO_UNICODE_CHAR_PTR_CAST
	#endif

	#if (EA_WCHAR_SIZE == 2)
		#define EAIO_UNICODE_CONST_CHAR_PTR_CAST(x) reinterpret_cast<const char16_t *>(x)
		#define EAIO_UNICODE_CHAR_PTR_CAST(x) reinterpret_cast<char16_t *>(x)
	#else
		#define EAIO_UNICODE_CONST_CHAR_PTR_CAST(x) reinterpret_cast<const char32_t *>(x)
		#define EAIO_UNICODE_CHAR_PTR_CAST(x) reinterpret_cast<char32_t *>(x)
	#endif

	namespace File
	{
		EAIO_API bool Exists(const wchar_t* pPath)
		{
			return Exists(EAIO_UNICODE_CONST_CHAR_PTR_CAST(pPath));
		}

		EAIO_API bool PatternExists(const wchar_t* pPathPattern)
		{
			return PatternExists(EAIO_UNICODE_CONST_CHAR_PTR_CAST(pPathPattern));
		}

		EAIO_API bool Create(const wchar_t* pPath, bool bTruncate)
		{
			return Create(EAIO_UNICODE_CONST_CHAR_PTR_CAST(pPath), bTruncate);
		}

		EAIO_API bool Remove(const wchar_t* pPath)
		{
			return Remove(EAIO_UNICODE_CONST_CHAR_PTR_CAST(pPath));
		}

		EAIO_API bool Move(const wchar_t* pPathSource, const wchar_t* pPathDestination, bool bOverwriteIfPresent)
		{
			return Move(EAIO_UNICODE_CONST_CHAR_PTR_CAST(pPathSource), EAIO_UNICODE_CONST_CHAR_PTR_CAST(pPathDestination), bOverwriteIfPresent);
		}

		EAIO_API bool Copy(const wchar_t* pPathSource, const wchar_t* pPathDestination, bool bOverwriteIfPresent)
		{
			return Copy(EAIO_UNICODE_CONST_CHAR_PTR_CAST(pPathSource), EAIO_UNICODE_CONST_CHAR_PTR_CAST(pPathDestination), bOverwriteIfPresent);
		}

		EAIO_API bool Stat(const wchar_t* pPath, StatData& statData)
		{
			return Stat(EAIO_UNICODE_CONST_CHAR_PTR_CAST(pPath), statData);
		}

		EAIO_API size_type GetSize(const wchar_t* pPath)
		{
			return GetSize(EAIO_UNICODE_CONST_CHAR_PTR_CAST(pPath));
		}

		EAIO_API bool IsWritable(const wchar_t* pPath)
		{
			return IsWritable(EAIO_UNICODE_CONST_CHAR_PTR_CAST(pPath));
		}

		EAIO_API int GetAttributes(const wchar_t* pPath)
		{
			return GetAttributes(EAIO_UNICODE_CONST_CHAR_PTR_CAST(pPath));
		}

		EAIO_API bool SetAttributes(const wchar_t* pPath, int nAttributeMask, bool bEnable)
		{
			return SetAttributes(EAIO_UNICODE_CONST_CHAR_PTR_CAST(pPath), nAttributeMask, bEnable);
		}

		EAIO_API time_t GetTime(const wchar_t* pPath, FileTimeType timeType)
		{
			return GetTime(EAIO_UNICODE_CONST_CHAR_PTR_CAST(pPath), timeType);
		}

		EAIO_API bool SetTime(const wchar_t* pPath, int nFileTimeTypeFlags, time_t nTime)
		{
			return SetTime(EAIO_UNICODE_CONST_CHAR_PTR_CAST(pPath), nFileTimeTypeFlags, nTime);
		}

		EAIO_API ResolveAliasResult ResolveAlias(const wchar_t* pPathSource, wchar_t* pPathDestination, uint32_t nDestPathCapacity)
		{
			return ResolveAlias(EAIO_UNICODE_CONST_CHAR_PTR_CAST(pPathSource), EAIO_UNICODE_CHAR_PTR_CAST(pPathDestination), nDestPathCapacity);
		}

		EAIO_API bool CreateAlias(const wchar_t* pDestinationPath, const wchar_t* pShortcutPath, 
								  const wchar_t* pShortcutDescription, const wchar_t* pShortcutArguments)
		{
			return CreateAlias(EAIO_UNICODE_CONST_CHAR_PTR_CAST(pDestinationPath), EAIO_UNICODE_CONST_CHAR_PTR_CAST(pShortcutPath), EAIO_UNICODE_CONST_CHAR_PTR_CAST(pShortcutDescription), EAIO_UNICODE_CONST_CHAR_PTR_CAST(pShortcutArguments));
		}
	}

	namespace Directory
	{
		EAIO_API bool Exists(const wchar_t* pDirectory)
		{
			return Exists(EAIO_UNICODE_CONST_CHAR_PTR_CAST(pDirectory));
		}

		EAIO_API bool EnsureExists(const wchar_t* pDirectory)
		{
			return EnsureExists(EAIO_UNICODE_CONST_CHAR_PTR_CAST(pDirectory));
		}

		EAIO_API bool Create(const wchar_t* pDirectory)
		{
			return Create(EAIO_UNICODE_CONST_CHAR_PTR_CAST(pDirectory));
		}

		EAIO_API bool Remove(const wchar_t* pDirectory, bool bAllowRecursiveRemoval)
		{
			return Remove(EAIO_UNICODE_CONST_CHAR_PTR_CAST(pDirectory), bAllowRecursiveRemoval);
		}

		EAIO_API bool Rename(const wchar_t* pDirectoryOld, const wchar_t* pDirectoryNew)
		{
			return Rename(EAIO_UNICODE_CONST_CHAR_PTR_CAST(pDirectoryOld), EAIO_UNICODE_CONST_CHAR_PTR_CAST(pDirectoryNew));
		}

		EAIO_API bool Copy(const wchar_t* pDirectorySource, const wchar_t* pDirectoryDestination, bool bRecursive, bool bOverwriteIfAlreadyPresent)
		{
			return Copy(EAIO_UNICODE_CONST_CHAR_PTR_CAST(pDirectorySource), EAIO_UNICODE_CONST_CHAR_PTR_CAST(pDirectoryDestination), bRecursive, bOverwriteIfAlreadyPresent);
		}

		EAIO_API bool SetAttributes(const wchar_t* pBaseDirectory, int nAttributeMask, bool bEnable, bool bRecursive, bool bIncludeBaseDirectory)
		{
			return SetAttributes(EAIO_UNICODE_CONST_CHAR_PTR_CAST(pBaseDirectory), nAttributeMask, bEnable, bRecursive, bIncludeBaseDirectory);
		}

		EAIO_API time_t GetTime(const wchar_t* pPath, FileTimeType timeType)
		{
			return GetTime(EAIO_UNICODE_CONST_CHAR_PTR_CAST(pPath), timeType);
		}

		EAIO_API bool SetTime(const wchar_t* pPath, int nFileTimeTypeFlags, time_t nTime)
		{
			return SetTime(EAIO_UNICODE_CONST_CHAR_PTR_CAST(pPath), nFileTimeTypeFlags, nTime);
		}

		EAIO_API int GetCurrentWorkingDirectory(wchar_t* pDirectory, uint32_t nPathCapacity)
		{
			return GetCurrentWorkingDirectory(EAIO_UNICODE_CHAR_PTR_CAST(pDirectory), nPathCapacity);
		}

		EAIO_API bool SetCurrentWorkingDirectory(const wchar_t* pDirectory)
		{
			return SetCurrentWorkingDirectory(EAIO_UNICODE_CONST_CHAR_PTR_CAST(pDirectory));
		}

		EAIO_API bool IsDirectoryEmpty(const wchar_t* pDirectory, int nDirectoryEntryFlags, bool bRecursive)
		{
			return IsDirectoryEmpty(EAIO_UNICODE_CONST_CHAR_PTR_CAST(pDirectory), nDirectoryEntryFlags, bRecursive);
		}
	}

	EAIO_API bool MakeTempPathName(wchar_t* pDestPath, const wchar_t* pDirectory, const wchar_t* pFileName, const wchar_t* pExtension, uint32_t nDestPathCapacity)
	{
		return MakeTempPathName(EAIO_UNICODE_CHAR_PTR_CAST(pDestPath), EAIO_UNICODE_CONST_CHAR_PTR_CAST(pDirectory), EAIO_UNICODE_CONST_CHAR_PTR_CAST(pFileName), EAIO_UNICODE_CONST_CHAR_PTR_CAST(pExtension), nDestPathCapacity);
	}

	EAIO_API int GetTempDirectory(wchar_t* pDirectory, uint32_t nPathCapacity)
	{
		return GetTempDirectory(EAIO_UNICODE_CHAR_PTR_CAST(pDirectory), nPathCapacity);
	}

	EAIO_API bool SetTempDirectory(const wchar_t* pDirectory)
	{
		return SetTempDirectory(EAIO_UNICODE_CONST_CHAR_PTR_CAST(pDirectory));
	}

	EAIO_API int GetDriveName(const wchar_t* pPath, wchar_t* pName)
	{
		return GetDriveName(EAIO_UNICODE_CONST_CHAR_PTR_CAST(pPath), EAIO_UNICODE_CHAR_PTR_CAST(pName));
	}

	EAIO_API int GetDriveSerialNumber(const wchar_t* pPath, wchar_t* pSerialNumber)
	{
		return GetDriveSerialNumber(EAIO_UNICODE_CONST_CHAR_PTR_CAST(pPath), EAIO_UNICODE_CHAR_PTR_CAST(pSerialNumber));
	}

	EAIO_API DriveType GetDriveTypeValue(const wchar_t* pPath)
	{
		return GetDriveTypeValue(EAIO_UNICODE_CONST_CHAR_PTR_CAST(pPath));
	}

	EAIO_API size_t GetDriveInfo(DriveInfoW* pDriveInfoArray, size_t nDriveInfoArrayCapacity)
	{
		DriveInfo8 driveInfo8Array[10];
		size_t     count = GetDriveInfo(driveInfo8Array, EAArrayCount(driveInfo8Array));
		
		if(count > nDriveInfoArrayCapacity)
			count = nDriveInfoArrayCapacity;

		// Copy to user-supplied drive info, converting to char32_t while doing so.
		for(size_t i = 0; i < count; i++)
		{
			pDriveInfoArray[i].mType = driveInfo8Array[i].mType;
			EA::StdC::Strlcpy(pDriveInfoArray[i].mName, driveInfo8Array[i].mName);
		}

		return count;
	}

	EAIO_API bool IsVolumeAvailable(const wchar_t* pPath, int timeoutMS)
	{
		return IsVolumeAvailable(EAIO_UNICODE_CONST_CHAR_PTR_CAST(pPath), timeoutMS);
	}

	EAIO_API int GetSpecialDirectory(SpecialDirectory specialDirectory, wchar_t* pDirectory, bool bEnsureDirectoryExistence, uint32_t nPathCapacity)
	{
		return GetSpecialDirectory(specialDirectory, EAIO_UNICODE_CHAR_PTR_CAST(pDirectory), bEnsureDirectoryExistence, nPathCapacity);
	}

	bool IsFileNameStringValid(const wchar_t* pName, FileSystem fileSystemType)
	{
		return IsFileNameStringValid(EAIO_UNICODE_CONST_CHAR_PTR_CAST(pName), fileSystemType);
	}

	bool IsDirectoryNameStringValid(const wchar_t* pName, FileSystem fileSystemType)
	{
		return IsDirectoryNameStringValid(EAIO_UNICODE_CONST_CHAR_PTR_CAST(pName), fileSystemType);
	}

	bool IsFilePathStringValid(const wchar_t* pPath, FileSystem fileSystemType)
	{
		return IsFilePathStringValid(EAIO_UNICODE_CONST_CHAR_PTR_CAST(pPath), fileSystemType);
	}


#endif

} // namespace IO

} // namespace EA






