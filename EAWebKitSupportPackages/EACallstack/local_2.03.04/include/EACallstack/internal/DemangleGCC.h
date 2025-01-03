/////////////////////////////////////////////////////////////////////////////
// Copyright (c) Electronic Arts Inc. All rights reserved.
/////////////////////////////////////////////////////////////////////////////


#include <EACallstack/internal/Config.h>

#ifndef EACALLSTACK_INTERNAL_DEMANGLEGCC_H

#if EACALLSTACK_GCC_DEMANGLE_ENABLED

namespace EA
{
namespace Callstack
{
namespace Demangle
{

	size_t UnmangleSymbolGCC(const char* pSymbol, char* buffer, size_t bufferCapacity);

} // namespace Demangle
} // namespace Callstack
} // namespace EA

#endif // EACALLSTACK_GCC_DEMANGLE_ENABLED

#endif // EACALLSTACK_INTERNAL_DEMANGLEGCC_H
