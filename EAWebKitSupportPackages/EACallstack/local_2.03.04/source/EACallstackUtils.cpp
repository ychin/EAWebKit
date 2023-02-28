/////////////////////////////////////////////////////////////////////////////
// Copyright (c) Electronic Arts Inc. All rights reserved.
/////////////////////////////////////////////////////////////////////////////

#include <EACallstack/internal/Config.h>
#include <EACallstack/internal/EACallstackUtils.h>
#include <EACallstack/internal/Intrin.h>

#include <EAIO/EAFileBase.h>
#include <EAIO/PathString.h>

namespace EA
{
namespace Callstack
{

namespace Detail
{

/**
 * ARM32 has two Intrsuction Sets, Thumb and AArch32
 * Thumb is 16-bit fixed width, AArch32 is 32-bit fixed width
 * The last bit of the instruction pointer determines if the instruction is a Thumb or AArch32 instruction
 */
EACALLSTACK_LOCAL void* RemoveARM32ThumbBit(void* pCodeAddress)
{
	uintptr_t ip = reinterpret_cast<uintptr_t>(pCodeAddress);

	ip = ip & ~(uintptr_t(1));

	return reinterpret_cast<void*>(ip);
}

} // namespace Detail


namespace Utils
{

EACALLSTACK_LOCAL uint16_t SwizzleUint16(uint16_t x)
{
	return (uint16_t) ((x >> 8) | (x << 8));
}

EACALLSTACK_LOCAL bool FileNamesMatch(const wchar_t* pFirst, const wchar_t* pSecond)
{
	// TODO(mwinkler): Look At EA::IO::GetFileNameString
	EA::IO::Path::PathStringW sFirst  = EA::IO::Path::GetFileName(pFirst);
	EA::IO::Path::PathStringW sSecond = EA::IO::Path::GetFileName(pSecond);

#if (EA_FILE_SYSTEM_CASE_SENSITIVE == 0)

	sFirst.make_lower();
	sSecond.make_lower();

#endif

	sFirst.resize((eastl_size_t)(EA::IO::Path::GetFileExtension(sFirst) - sFirst.begin()));
	sSecond.resize((eastl_size_t)(EA::IO::Path::GetFileExtension(sSecond) - sSecond.begin()));

	return (sFirst == sSecond);
}

EACALLSTACK_LOCAL size_t GetFileName(char* buffer, size_t bufferSize, const char* filepath)
{
	const char* filename = EA::IO::Path::GetFileName(filepath);

	if (buffer)
	{
		size_t ret = EA::StdC::Strlcpy(buffer, filename, bufferSize);
		if (ret < bufferSize)
		{
			return ret;
		}

		buffer[0] = '\0';
		return 0;
	}
	else
	{
		const size_t charactersToCopy = static_cast<size_t>(EA::StdC::Strend(filepath) - filename);
		return charactersToCopy;
	}
}

EACALLSTACK_LOCAL size_t GetFileNameWithoutExt(char* buffer, size_t bufferSize, const char* filepath)
{
	char pModuleFileName[EA::IO::kMaxPathLength];

	Utils::GetFileName(pModuleFileName, EA::IO::kMaxPathLength, filepath);

	if (buffer)
	{
		*EA::IO::Path::GetFileExtension(pModuleFileName) = '\0';

		size_t ret = EA::StdC::Strlcpy(buffer, pModuleFileName, bufferSize);
		if (ret < bufferSize)
		{
			return ret;
		}

		buffer[0] = '\0';
		return 0;
	}
	else
	{
		const char* fileExt = EA::IO::Path::GetFileExtension(pModuleFileName);
		return static_cast<size_t>(fileExt - pModuleFileName);
	}
}

} // namespace Utils

} // namespace Callstack
} // namespace EA
