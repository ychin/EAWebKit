///////////////////////////////////////////////////////////////////////////////
// Copyright (c) Electronic Arts Inc. All rights reserved.
///////////////////////////////////////////////////////////////////////////////

#include <EACallstack/internal/Config.h>
#include <EAIO/PathString.h>

#include <nn/diag.h>

namespace EA
{
namespace Callstack
{

namespace Detail
{

static void FillOutModuleInfo(bool emptyFill, ModuleInfo& moduleInfo, const nn::diag::ModuleInfo& nxModule)
{
	if (!emptyFill)
	{
		moduleInfo.mModuleHandle = static_cast<ModuleHandle>(nxModule.baseAddress);

		moduleInfo.mPath = nxModule.path;

		moduleInfo.mName = EA::IO::Path::GetFileName(nxModule.path);

		moduleInfo.mBaseAddress = static_cast<uint64_t>(nxModule.baseAddress);

		moduleInfo.mSize = static_cast<uint64_t>(nxModule.size);
	}
	else
	{
		ClearModuleInfo(moduleInfo);
	}
}

template <typename F>
static bool IterateNXModules(F&& f)
{
	const size_t nxBufferSize = nn::diag::GetRequiredBufferSizeForGetAllModuleInfo();
	EA_ALIGNED(char, nxModuleBuffer[nxBufferSize], 8);

	nn::diag::ModuleInfo* nxModules;
	int ret = nn::diag::GetAllModuleInfo(&nxModules, nxModuleBuffer, nxBufferSize);
	if (ret > 0)
	{
		for (int i = 0; i < ret; ++i)
		{
			const nn::diag::ModuleInfo& nxModule = nxModules[i];

			if (!f(nxModule))
			{
				return true;
			}
		}
	}

	return false;
}

} // namespace Detail

namespace EAModuleImpl
{

///////////////////////////////////////////////////////////////////////////////
// GetModuleFromAddressImpl
//
EACALLSTACK_LOCAL size_t GetModuleFromAddressImpl(const void* pAddress, char* pModuleFileName, size_t moduleNameCapacity)
{
	int result = -1;

	auto moduleHandler = [&](const nn::diag::ModuleInfo& nxModule)
						 {
							 const uint64_t addr = reinterpret_cast<uint64_t>(pAddress);
							 const uint64_t baseAddress = static_cast<uint64_t>(nxModule.baseAddress);
							 const uint64_t endAddress = baseAddress + nxModule.size;

							 if ((baseAddress <= addr) && (addr < endAddress))
							 {
								 const char* filename = EA::IO::Path::GetFileName(nxModule.path);

								 result = EA::StdC::Strlcpy(pModuleFileName, filename, moduleNameCapacity);

								 return false;
							 }

							 return true;
						 };

	if (Detail::IterateNXModules(moduleHandler))
	{
		if (result > 0)
		{
			return static_cast<size_t>(result);
		}
	}


	pModuleFileName[0] = '\0';
	return 0;
}

///////////////////////////////////////////////////////////////////////////////
// GetModuleHandleFromAddressImpl
//
EACALLSTACK_LOCAL ModuleHandle GetModuleHandleFromAddressImpl(const void* pAddress)
{
	ModuleHandle moduleHandle = kModuleHandleInvalid;

	auto moduleHandler = [&](const nn::diag::ModuleInfo& nxModule)
						 {
							 const uint64_t addr = reinterpret_cast<uint64_t>(pAddress);
							 const uint64_t baseAddress = static_cast<uint64_t>(nxModule.baseAddress);
							 const uint64_t endAddress = baseAddress + nxModule.size;

							 if ((baseAddress <= addr) && (addr < endAddress))
							 {
								 moduleHandle = static_cast<ModuleHandle>(nxModule.baseAddress);

								 return false;
							 }

							 return true;
						 };

	Detail::IterateNXModules(moduleHandler);

	return moduleHandle;
}


///////////////////////////////////////////////////////////////////////////////
// GetModuleBaseAddressImpl
//
EACALLSTACK_LOCAL uint64_t GetModuleBaseAddressImpl(const wchar_t* pModuleName)
{
	return TransformToChar(pModuleName,
						   [](const char* moduleFileName) -> uint64_t { return GetModuleBaseAddressImpl(moduleFileName); },
						   kBaseAddressInvalid);
}


///////////////////////////////////////////////////////////////////////////////
// GetModuleBaseAddressImpl
//
EACALLSTACK_LOCAL uint64_t GetModuleBaseAddressImpl(const char* pModuleName)
{
	char moduleFileName[EA::IO::kMaxPathLength];

	Utils::GetFileNameWithoutExt(moduleFileName, EA::IO::kMaxPathLength, pModuleName);

	uint64_t result = kBaseAddressInvalid;

	auto moduleHandler = [&](const nn::diag::ModuleInfo& nxModule)
						 {
							 if (EA::StdC::Stristr(nxModule.path, moduleFileName))
							 {
								 result = static_cast<uint64_t>(nxModule.baseAddress);

								 return false;
							 }

							 return true;
						 };

	Detail::IterateNXModules(moduleHandler);

	return result;
}


///////////////////////////////////////////////////////////////////////////////
// GetModuleBaseAddressByHandleImpl
//
EACALLSTACK_LOCAL uint64_t GetModuleBaseAddressByHandleImpl(ModuleHandle moduleHandle)
{
	return static_cast<uint64_t>(moduleHandle);
}

///////////////////////////////////////////////////////////////////////////////
// GetModuleBaseAddressByAddressImpl
//
EACALLSTACK_LOCAL uint64_t GetModuleBaseAddressByAddressImpl(const void* pCodeAddress)
{
	uint64_t result = kBaseAddressInvalid;

	auto moduleHandler = [&](const nn::diag::ModuleInfo& nxModule)
						 {
							 const uint64_t addr = reinterpret_cast<uint64_t>(pCodeAddress);
							 const uint64_t baseAddress = static_cast<uint64_t>(nxModule.baseAddress);
							 const uint64_t endAddress = baseAddress + nxModule.size;

							 if ((baseAddress <= addr) && (addr < endAddress))
							 {
								 result = static_cast<uint64_t>(nxModule.baseAddress);

								 return false;
							 }

							 return true;
						 };

	Detail::IterateNXModules(moduleHandler);

	return result;
}

///////////////////////////////////////////////////////////////////////////////
/// GetModuleHandleListImpl
///
EACALLSTACK_LOCAL size_t GetModuleHandleListImpl(ModuleHandle* pModuleHandleArray, size_t nModuleArrayCapacity)
{
	size_t moduleCount = 0;

	auto moduleHandler = [&](const nn::diag::ModuleInfo& nxModule)
						 {
							 if (pModuleHandleArray)
							 {
								 if (moduleCount < nModuleArrayCapacity)
								 {
									 pModuleHandleArray[moduleCount] = reinterpret_cast<ModuleHandle>(nxModule.baseAddress);

									 ++moduleCount;
								 }
							 }
							 else
							 {
								 ++moduleCount;
							 }

							 return true;
						 };

	Detail::IterateNXModules(moduleHandler);

	return moduleCount;
}


///////////////////////////////////////////////////////////////////////////////
// GetModuleInfoImpl
//
EACALLSTACK_LOCAL size_t GetModuleInfoImpl(ModuleInfo* pModuleInfoArray, size_t moduleArrayCapacity)
{
	size_t moduleCount = 0;

	auto moduleHandler = [&](const nn::diag::ModuleInfo& nxModule)
						 {
							 if (pModuleInfoArray)
							 {
								 if (moduleCount < moduleArrayCapacity)
								 {
									 ModuleInfo& moduleInfo = pModuleInfoArray[moduleCount];
									 ++moduleCount;

									 Detail::FillOutModuleInfo(false, moduleInfo, nxModule);
								 }
							 }
							 else
							 {
								 ++moduleCount;
							 }

							 return true;
						 };

	Detail::IterateNXModules(moduleHandler);

	return moduleCount;
}

///////////////////////////////////////////////////////////////////////////////
/// GetModuleInfoByHandleImpl
///
EACALLSTACK_LOCAL bool GetModuleInfoByHandleImpl(ModuleHandle moduleHandle, ModuleInfo& moduleInfo)
{
	auto moduleHandler = [&](const nn::diag::ModuleInfo& nxModule)
						 {
							 const ModuleHandle nxHandle = static_cast<ModuleHandle>(nxModule.baseAddress);
							 if (nxHandle == moduleHandle)
							 {
								 Detail::FillOutModuleInfo(false, moduleInfo, nxModule);

								 return false;
							 }

							 return true;
						 };

	if (Detail::IterateNXModules(moduleHandler))
	{
		return true;
	}

	return false;
}

///////////////////////////////////////////////////////////////////////////////
/// GetModuleHandleOfExecImpl
///
EACALLSTACK_LOCAL ModuleHandle GetModuleHandleOfExecImpl()
{
	ModuleHandle moduleHandle = kModuleHandleInvalid;
	int moduleCount = 0;

	/* On NX the exec is always the second image added to the linker list after nnrtld, NX's runtime loader */
	auto moduleHandler = [&](const nn::diag::ModuleInfo& nxModule)
						 {
							 if (moduleCount == 1)
							 {
								 moduleHandle = static_cast<ModuleHandle>(nxModule.baseAddress);

								 return false;
							 }

							 ++moduleCount;

							 return true;
						 };

	Detail::IterateNXModules(moduleHandler);

	return moduleHandle;
}

} // namespace EAModuleImpl


} // namespace Callstack
} // namspace EA
