///////////////////////////////////////////////////////////////////////////////
// EATextStream.cpp
//
// Copyright (c) 2006 Electronic Arts. All Rights Reserved.
// Written and maintained by Paul Pedriana.
//
///////////////////////////////////////////////////////////////////////////////


#include <EAText/internal/EATextStream.h>
#include <EAText/internal/EATextCoreAllocatorNew.h>
#include <coreallocator/icoreallocator_interface.h>
#include <coreallocator/icoreallocatormacros.h>


namespace EA
{

namespace IO
{

#if EAIO_VERSION_N < 30000
int EATextFileStream::Release()
{
	if(mnRefCount > 1)
		return --mnRefCount;

	if(mpCoreAllocator)
		CORE_DELETE(mpCoreAllocator, this);

	return 0;
}



int EATextMemoryStream::Release()
{
	if(mnRefCount > 1)
		return --mnRefCount;

	if(mpCoreAllocator)
		CORE_DELETE(mpCoreAllocator, this);

	return 0;
}
#endif

} // namespace Text

} // namespace EA






















