/////////////////////////////////////////////////////////////////////////////
// Copyright (c) Electronic Arts Inc. All rights reserved.
/////////////////////////////////////////////////////////////////////////////

#ifndef EACALLSTACK_INTERNAL_UTILS_H
#define EACALLSTACK_INTERNAL_UTILS_H

#include <EACallstack/internal/Config.h>

#if defined(EA_PRAGMA_ONCE_SUPPORTED)
	#pragma once
#endif // EA_PRAGMA_ONCE_SUPPORTED


namespace EA
{
namespace Callstack
{

namespace Utils
{

EACALLSTACK_LOCAL uint16_t SwizzleUint16(uint16_t x);

EACALLSTACK_LOCAL bool FileNamesMatch(const wchar_t* pFirst, const wchar_t* pSecond);

EACALLSTACK_LOCAL size_t GetFileName(char* buffer, size_t bufferSize, const char* filepath);

EACALLSTACK_LOCAL size_t GetFileNameWithoutExt(char* buffer, size_t bufferSize, const char* filepath);

} // namespace Utils


} // namespace Callstack
} // namespace EA

#endif // EACALLSTACK_INTERNAL_UTILS_H
