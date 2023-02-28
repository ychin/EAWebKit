///////////////////////////////////////////////////////////////////////////////
// Copyright (c) Electronic Arts Inc. All rights reserved.
///////////////////////////////////////////////////////////////////////////////


EA_DISABLE_ALL_VC_WARNINGS();

#include <Windows.h>

#if EA_WINAPI_FAMILY_PARTITION(EA_WINAPI_PARTITION_DESKTOP) || EA_WINAPI_FAMILY_PARTITION(EA_WINAPI_PARTITION_GAMES)

	#include <process.h>
	#include <Psapi.h>
	#include <ShellAPI.h>

	#ifdef EA_COMPILER_MSVC

		#pragma comment(lib, "dbghelp.lib")
		#pragma comment(lib, "psapi.lib")
		#pragma comment(lib, "shell32.lib") // Required for shellapi calls.

	#endif // EA_COMPILER_MSVC

#endif // EA_WINAPI_PARTITION_DESKTOP || EA_WINAPI_PARTITION_GAMES

/**
 * Define the GMI, GetModuleInformation, module and functions in a partition independent way.
 * All Ascii variants of the Windows API, GetModuleFileNameA, allocate memory and call the wchar_t variant.
 * We intentionally use the wchar_t variant so that memory is not allocated. All conversions from UTF8 to UCS4
 * are done on a stack allocated buffer if needed.
 */
#if EA_WINAPI_FAMILY_PARTITION(EA_WINAPI_PARTITION_TV_TITLE)

	#define EAGMI_DLL_NAME       L"toolhelpx.dll"
	#define EAGMI_ENUM_FUNC_NAME  "K32EnumProcessModules"
	#define EAGMI_GMI_FUNC_NAME   "K32GetModuleInformation"
	#define EAGMI_GMBN_FUNC_NAME  "K32GetModuleBaseNameW"

	typedef struct _MODULEINFO { // This is normally defined in psapi.h for Windows
		LPVOID lpBaseOfDll;
		DWORD  SizeOfImage;
		LPVOID EntryPoint;
	} MODULEINFO, *LPMODULEINFO;

#elif EA_WINAPI_FAMILY_PARTITION(EA_WINAPI_PARTITION_GAMES)

	#define EAGMI_DLL_NAME       L"KernelBase.dll"
	#define EAGMI_ENUM_FUNC_NAME  "K32EnumProcessModules"
	#define EAGMI_GMI_FUNC_NAME   "K32GetModuleInformation"
	#define EAGMI_GMBN_FUNC_NAME  "K32GetModuleBaseNameW"

#elif EA_WINAPI_FAMILY_PARTITION(EA_WINAPI_PARTITION_DESKTOP)

	#define EAGMI_DLL_NAME       L"psapi.dll"
	#define EAGMI_ENUM_FUNC_NAME  "EnumProcessModules"
	#define EAGMI_GMI_FUNC_NAME   "GetModuleInformation"
	#define EAGMI_GMBN_FUNC_NAME  "GetModuleBaseNameW"

#endif

typedef BOOL  (__stdcall *ENUMPROCESSMODULES) (HANDLE hProcess, HMODULE* phModule, DWORD cb, LPDWORD lpcbNeeded);
typedef DWORD (__stdcall *GETMODULEBASENAME) (HANDLE hProcess, HMODULE hModule, LPWSTR lpFilename, DWORD nSize);
typedef BOOL  (__stdcall *GETMODULEINFORMATION) (HANDLE hProcess, HMODULE hModule, MODULEINFO* pmi, DWORD nSize);

EA_RESTORE_ALL_VC_WARNINGS();

namespace EA
{
namespace Callstack
{

namespace Detail
{


struct GMIHelp
{
	HMODULE mhdll = nullptr;
	ENUMPROCESSMODULES mpEnumProcessModules = nullptr;
	GETMODULEBASENAME mpGetModuleBaseName = nullptr;
	GETMODULEINFORMATION mpGetModuleInformation = nullptr;
	bool mbInitialized = false;

	GMIHelp()
	{
		mhdll = LoadLibraryW(EAGMI_DLL_NAME);

		if (mhdll)
		{
			*(reinterpret_cast<void**>(&mpEnumProcessModules)) = reinterpret_cast<void*>(GetProcAddress(mhdll, EAGMI_ENUM_FUNC_NAME));
			*(reinterpret_cast<void**>(&mpGetModuleBaseName)) = reinterpret_cast<void*>(GetProcAddress(mhdll, EAGMI_GMBN_FUNC_NAME));
			*(reinterpret_cast<void**>(&mpGetModuleInformation)) = reinterpret_cast<void*>(GetProcAddress(mhdll, EAGMI_GMI_FUNC_NAME));

			if (mpEnumProcessModules && mpGetModuleBaseName && mpGetModuleInformation)
			{
				mbInitialized = true;
			}
		}
	}

	~GMIHelp()
	{
		if (mhdll)
		{
			FreeLibrary(mhdll);

			mpEnumProcessModules = nullptr;
			mpGetModuleBaseName = nullptr;
			mpGetModuleInformation = nullptr;

			mbInitialized = false;
		}
	}

	bool IsInitialized() const
	{
		return mbInitialized;
	}

	static const GMIHelp& GetGMIHelp()
	{
		static GMIHelp sGMIHelp{};
		return sGMIHelp;
	}
};

static size_t ComputeHMODULECount(DWORD cbNeeded, size_t moduleCapacity)
{
	return ((cbNeeded / sizeof(HMODULE)) < moduleCapacity) ? (cbNeeded / sizeof(HMODULE)) : moduleCapacity;
}

static size_t GetModuleFileNameNullTerminated(HMODULE module, wchar_t* moduleFileNameW, size_t moduleNameCapacity)
{
	DWORD ret = GetModuleFileNameW(module, moduleFileNameW, static_cast<DWORD>(moduleNameCapacity));
	if (ret >= moduleNameCapacity)
	{
		moduleFileNameW[moduleNameCapacity - 1] = '\0';
	}

	return static_cast<size_t>(ret);
}

static void FillOutModuleInfo(bool emptyFill, ModuleInfo& moduleInfo, HMODULE module, const MODULEINFO& mi)
{
	if (!emptyFill)
	{
		wchar_t pathW[EA::IO::kMaxPathLength];
		pathW[0] = '\0';
		char path8[EA::IO::kMaxPathLength];
		path8[0] = '\0';

		Detail::GetModuleFileNameNullTerminated(module, pathW, EA::IO::kMaxPathLength);
		EA::StdC::Strlcpy(path8, pathW, EA::IO::kMaxPathLength);
		moduleInfo.mPath = path8;

		moduleInfo.mModuleHandle = static_cast<ModuleHandle>(module);

		moduleInfo.mName = EA::IO::Path::GetFileName(path8);

		moduleInfo.mBaseAddress = reinterpret_cast<uint64_t>(mi.lpBaseOfDll);

		moduleInfo.mSize = static_cast<uint64_t>(mi.SizeOfImage);
	}
	else
	{
		ClearModuleInfo(moduleInfo);
	}
}

} // namespace Detail

namespace EAModuleImpl
{

///////////////////////////////////////////////////////////////////////////////
// GetModuleFromAddressImpl
//
EACALLSTACK_LOCAL size_t GetModuleFromAddressImpl(const void* address, char* pModuleName, size_t moduleNameCapacity)
{
	MEMORY_BASIC_INFORMATION mbi;

	if (VirtualQuery(address, &mbi, sizeof(mbi)))
	{
		HMODULE hModule = reinterpret_cast<HMODULE>(mbi.AllocationBase);

		if ((mbi.State != MEM_FREE) && (hModule))
		{
			wchar_t moduleFileNameW[EA::IO::kMaxPathLength];
			moduleFileNameW[0] = '\0';

			Detail::GetModuleFileNameNullTerminated(hModule, moduleFileNameW, EA::IO::kMaxPathLength);

			int result = EA::StdC::Strlcpy(pModuleName, moduleFileNameW, moduleNameCapacity);

			return static_cast<size_t>(result);
		}
	}

	pModuleName[0] = '\0';
	return 0;
}


///////////////////////////////////////////////////////////////////////////////
// GetModuleHandleFromAddressImpl
//
EACALLSTACK_LOCAL ModuleHandle GetModuleHandleFromAddressImpl(const void* pAddress)
{
	MEMORY_BASIC_INFORMATION mbi;

	if (VirtualQuery(pAddress, &mbi, sizeof(mbi)))
	{
		if (mbi.State == MEM_FREE)
		{
			return kModuleHandleInvalid;
		}

		return reinterpret_cast<ModuleHandle>(mbi.AllocationBase);
	}

	return kModuleHandleInvalid;
}


///////////////////////////////////////////////////////////////////////////////
// GetModuleBaseAddressImpl
//
EACALLSTACK_LOCAL uint64_t GetModuleBaseAddressImpl(const wchar_t* pModuleName)
{
	const Detail::GMIHelp& gmiHelp = Detail::GMIHelp::GetGMIHelp();

	if (gmiHelp.IsInitialized())
	{
		HANDLE hProcess = GetCurrentProcess();

		ENUMPROCESSMODULES pEnumProcessModules = gmiHelp.mpEnumProcessModules;
		GETMODULEBASENAME pGetModuleBaseName = gmiHelp.mpGetModuleBaseName;
		GETMODULEINFORMATION pGetModuleInformation = gmiHelp.mpGetModuleInformation;

		HMODULE hModuleBuffer[EA::Callstack::kModuleMaxCapacity];
		MODULEINFO mi;
		DWORD cbNeeded = 0;
		wchar_t pBaseName[EA::IO::kMaxPathLength];
		pBaseName[0] = '\0';
		wchar_t pFullName[EA::IO::kMaxPathLength];
		pFullName[0] = '\0';

		if (pEnumProcessModules(hProcess, hModuleBuffer, sizeof(hModuleBuffer), &cbNeeded))
		{
			const size_t moduleCount = Detail::ComputeHMODULECount(cbNeeded, EA::Callstack::kModuleMaxCapacity);

			// Run the first loop checking for perfect full path matches
			for (size_t i = 0; i < moduleCount; ++i)
			{
				EA::StdC::Memset8(&mi, 0, sizeof(mi));

				pGetModuleInformation(hProcess, hModuleBuffer[i], &mi, sizeof(mi));
				pGetModuleBaseName(hProcess, hModuleBuffer[i], pBaseName, EA::IO::kMaxPathLength);
				Detail::GetModuleFileNameNullTerminated(hModuleBuffer[i], pFullName, EA::IO::kMaxPathLength);

				if (Utils::FileNamesMatch(pFullName, pModuleName))
				{
					return reinterpret_cast<uint64_t>(mi.lpBaseOfDll);
				}
			}

			// Run the second loop checking for just file name matches.
			for (size_t i = 0; i < moduleCount; ++i)
			{
				EA::StdC::Memset8(&mi, 0, sizeof(mi));

				pGetModuleInformation(hProcess, hModuleBuffer[i], &mi, sizeof(mi));
				pGetModuleBaseName(hProcess, hModuleBuffer[i], pBaseName, EA::IO::kMaxPathLength);
				Detail::GetModuleFileNameNullTerminated(hModuleBuffer[i], pFullName, EA::IO::kMaxPathLength);

				if (Utils::FileNamesMatch(pBaseName, pModuleName))
				{
					return reinterpret_cast<uint64_t>(mi.lpBaseOfDll);
				}

				if (Utils::FileNamesMatch(pFullName, pModuleName))
				{
					return reinterpret_cast<uint64_t>(mi.lpBaseOfDll);
				}
			}

			return kBaseAddressInvalid;
		}
	}

	return kBaseAddressInvalid;
}

///////////////////////////////////////////////////////////////////////////////
// GetModuleBaseAddressImpl
//
EACALLSTACK_LOCAL uint64_t GetModuleBaseAddressImpl(const char* pModuleName)
{
	return TransformToWChar(pModuleName,
							[](const wchar_t* moduleFileNameW) -> uint64_t { return GetModuleBaseAddressImpl(moduleFileNameW); },
							kBaseAddressInvalid);
}


///////////////////////////////////////////////////////////////////////////////
// GetModuleBaseAddressByHandleImpl
//
EACALLSTACK_LOCAL uint64_t GetModuleBaseAddressByHandleImpl(ModuleHandle moduleHandle)
{
	const Detail::GMIHelp& gmiHelp = Detail::GMIHelp::GetGMIHelp();

	if (gmiHelp.IsInitialized())
	{
		GETMODULEINFORMATION pGetModuleInformation = gmiHelp.mpGetModuleInformation;

		MODULEINFO moduleInfo;
		pGetModuleInformation(GetCurrentProcess(), static_cast<HMODULE>(moduleHandle), &moduleInfo, sizeof(moduleInfo));

		return reinterpret_cast<uint64_t>(moduleInfo.lpBaseOfDll);
	}

	return kBaseAddressInvalid;
}

///////////////////////////////////////////////////////////////////////////////
// GetModuleHandleOfExecImpl
//
EACALLSTACK_LOCAL ModuleHandle GetModuleHandleOfExecImpl()
{
	HMODULE nativeModuleHandle = GetModuleHandle(nullptr);

	return static_cast<ModuleHandle>(nativeModuleHandle);
}

///////////////////////////////////////////////////////////////////////////////
// GetModuleBaseAddressByAddressImpl
//
EACALLSTACK_LOCAL uint64_t GetModuleBaseAddressByAddressImpl(const void* pCodeAddress)
{
	MEMORY_BASIC_INFORMATION mbi;

	if (VirtualQuery(pCodeAddress, &mbi, sizeof(mbi)))
	{
		if (mbi.State == MEM_FREE)
		{
			return kBaseAddressInvalid;
		}

		return reinterpret_cast<uint64_t>(mbi.AllocationBase);
	}

	return kBaseAddressInvalid;
}


///////////////////////////////////////////////////////////////////////////////
/// GetModuleHandleListImpl
///
EACALLSTACK_LOCAL size_t GetModuleHandleListImpl(ModuleHandle* pModuleHandleArray, size_t nModuleArrayCapacity)
{
	size_t moduleCount = 0;

	const Detail::GMIHelp& gmiHelp = Detail::GMIHelp::GetGMIHelp();

	if (gmiHelp.IsInitialized())
	{
		HANDLE hProcess = GetCurrentProcess();
		ENUMPROCESSMODULES pEnumProcessModules = gmiHelp.mpEnumProcessModules;

		HMODULE hModuleBuffer[EA::Callstack::kModuleMaxCapacity];
		DWORD cbNeeded = 0;

		if (pEnumProcessModules(hProcess, hModuleBuffer, sizeof(hModuleBuffer), &cbNeeded))
		{
			moduleCount = Detail::ComputeHMODULECount(cbNeeded, EA::Callstack::kModuleMaxCapacity);

			if (pModuleHandleArray)
			{
				moduleCount = eastl::min_alt(moduleCount, nModuleArrayCapacity);

				EA::StdC::Memcpy(pModuleHandleArray, hModuleBuffer, sizeof(ModuleHandle) * moduleCount);
			}
		}
	}

	return moduleCount;
}


///////////////////////////////////////////////////////////////////////////////
// GetModuleInfoImpl
//
EACALLSTACK_LOCAL size_t GetModuleInfoImpl(ModuleInfo* pModuleInfoArray, size_t moduleArrayCapacity)
{
	size_t moduleCount = 0;
	HANDLE hProcess = GetCurrentProcess();

	const Detail::GMIHelp& gmiHelp = Detail::GMIHelp::GetGMIHelp();

	if (gmiHelp.IsInitialized())
	{
		ENUMPROCESSMODULES pEnumProcessModules = gmiHelp.mpEnumProcessModules;
		GETMODULEINFORMATION pGetModuleInformation = gmiHelp.mpGetModuleInformation;

		HMODULE hModuleBuffer[EA::Callstack::kModuleMaxCapacity];
		DWORD cbNeeded = 0;
		MODULEINFO mi;

		if (pEnumProcessModules(hProcess, hModuleBuffer, sizeof(hModuleBuffer), &cbNeeded))
		{
			moduleCount = Detail::ComputeHMODULECount(cbNeeded, EA::Callstack::kModuleMaxCapacity);

			if (pModuleInfoArray)
			{
				moduleCount = eastl::min_alt(moduleCount, moduleArrayCapacity);

				for (size_t i = 0; i < moduleCount; ++i)
				{
					ModuleInfo& moduleInfo = pModuleInfoArray[i];

					EA::StdC::Memset8(&mi, 0, sizeof(mi));
					BOOL result = pGetModuleInformation(hProcess, hModuleBuffer[i], &mi, sizeof(mi));

					Detail::FillOutModuleInfo(!result, moduleInfo, hModuleBuffer[i], mi);
				}
			}
		}
	}

	return moduleCount;
}

///////////////////////////////////////////////////////////////////////////////
/// GetModuleInfoByHandleImpl
///
EACALLSTACK_LOCAL bool GetModuleInfoByHandleImpl(ModuleHandle moduleHandle, ModuleInfo& moduleInfo)
{
	HANDLE hProcess = GetCurrentProcess();
	HMODULE nativeModuleHandle = static_cast<HMODULE>(moduleHandle);

	const Detail::GMIHelp& gmiHelp = Detail::GMIHelp::GetGMIHelp();

	if (gmiHelp.IsInitialized())
	{
		GETMODULEINFORMATION pGetModuleInformation = gmiHelp.mpGetModuleInformation;

		MODULEINFO mi;
		EA::StdC::Memset8(&mi, 0, sizeof(mi));

		BOOL result = pGetModuleInformation(hProcess, nativeModuleHandle, &mi, sizeof(mi));

		Detail::FillOutModuleInfo(!result, moduleInfo, nativeModuleHandle, mi);

		return result;
	}

	return false;
}

} // namespace EAModuleImpl

} // namespace Callstack
} // namespace EA
