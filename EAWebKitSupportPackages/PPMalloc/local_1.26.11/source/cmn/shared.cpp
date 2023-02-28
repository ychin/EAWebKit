///////////////////////////////////////////////////////////////////////////////
// shared.cpp
//
// Copyright (c) 2007, Electronic Arts. All Rights Reserved.
// By Paul Pedriana, Maxis
///////////////////////////////////////////////////////////////////////////////


#include <PPMalloc/internal/shared.h>
#include <string.h>
#include <stdio.h>
#if   defined(EA_PLATFORM_MICROSOFT)
	#pragma warning(push, 0)
	#include <Windows.h>
	#include <time.h>
	#pragma warning(pop)
#elif defined(EA_PLATFORM_ANDROID)
	#include <unistd.h>
	#include <android/log.h>
	#include <time.h>
#elif defined(EA_PROCESSOR_ARM) && defined(__APPLE__)
	#include <signal.h>
	#include <unistd.h>
	#include <time.h>
#elif defined(EA_PLATFORM_UNIX) || defined(__APPLE__)
	#include <unistd.h>      // sbrk(), getpagesize(), mmap, munmap, etc.
	#include <time.h>
#elif defined(EA_PLATFORM_SONY)
	#include <time.h>
	#include <sys/dmem.h>
#else
	#include <time.h>
#endif


namespace EA
{
namespace Allocator
{


const char* kGAErrorText[kGAErrorCount] = 
{
	"Unknown error.",
	"Operating system error.",
	"Internal error.",
	"Heap is unintialized.",
	"Heap structures are corrupted.",
	"Chunk is corrupted.",
	"Chunk has fill error.",
	"Chunk doesn't belong to heap.",
	"Report context is invalid.",
	"Snapshot memory exhaustion.",
	"Trace memory exhaustion.",
	"Requested size is too large.",
	"Requested alignment is invalid.",
	"Supplied core is invalid.",
	"Bad user pointer.",
	"Invalid input.",
	"Memory exhaustion."
};








// Returns elapsed time in seconds.
// The time doesn't need to be rooted at any particular real world time.
unsigned GetPPMallocDebugTime()
{
	#if defined(EA_PLATFORM_MICROSOFT)
		#if EA_WINAPI_FAMILY_PARTITION(EA_WINAPI_PARTITION_DESKTOP)
			// GetTickCount() about 23 days worth of milliseconds. However, if the computer
			// has been running for 22 days, then the return value will turn over within
			// only 24 hours. This may be a small problem for our uses, but not deadly,
			// as this is debug code only.
			return (unsigned)(GetTickCount() / 1000);
		#else
			return static_cast<unsigned>(GetTickCount64() / 1000);
		#endif
	#else
		return (unsigned)(clock() / CLOCKS_PER_SEC);
	#endif
}


// Returns elapsed time in microseconds.
// The time doesn't need to be rooted at any particular real world time.
uint64_t GetPPMallocDebugTimeUs()
{
	#if defined(EA_PLATFORM_WINDOWS)
		uint64_t nTime;
		GetSystemTimeAsFileTime((FILETIME*)&nTime); //lint !e740 (Unusual pointer cast)
		nTime /= 10; // Convert from 100ns to 1 us.
		return nTime;
	#else
		return (uint64_t)((uint64_t)clock() * 100000 / CLOCKS_PER_SEC);
	#endif
}


void PPM_DebugBreak()
{
	#if defined(_MSC_VER) && (_MSC_VER >= 1300)
		__debugbreak();
	#elif defined(EA_PROCESSOR_ARM) && defined(__APPLE__)
		kill( getpid(), SIGINT );
	#elif defined(EA_PROCESSOR_ARM64) && defined(__GNUC__)
		asm("brk 10");
	#elif defined(EA_PROCESSOR_ARM) && defined(__GNUC__)
		asm("BKPT 10");     // The 10 is arbitrary. It's just a unique id.
	#elif defined(EA_PROCESSOR_ARM) && defined(__ARMCC_VERSION)
		__breakpoint(10);
	#elif defined(EA_PLATFORM_NX)
		__builtin_debugtrap();
	#elif defined(EA_PROCESSOR_POWERPC)
		asm(".long 0");
	#elif defined(EA_PROCESSOR_X86) && defined(EA_ASM_STYLE_INTEL)
		{ __asm int 3 }
	#elif (defined(EA_PROCESSOR_X86) || defined(EA_PROCESSOR_X86_64)) && defined(__GNUC__)
		asm("int3");
	#endif
}


DataPreviewType GetDataPreview(const void* pDataVoid, size_t nDataSize, char* pBuffer, wchar_t* pBufferW, size_t nBufferLength)
{
	// Make sure have some room to print to.
	if(nBufferLength < 5) // If there isn't enough room to draw anything... (minimal draw is "48 H" (see above hello world example))
	{
		if(nBufferLength)
		{
			if(pBuffer)
				pBuffer[0] = 0;
			if(pBufferW)
				pBufferW[0] = 0;
		}
		return kDataPreviewTypeBinary;
	}

	// In order to preserve some cache coherency, we implement binary/hex conversions in place.
	const char  hexDigits[16] = { '0', '1', '2', '3', '4', '5', '6', '7', '8', '9', 'a', 'b', 'c', 'd', 'e', 'f' };
	const char* pData = (const char*)pDataVoid;

	// Get the sizes we are going to work with.
	size_t nPreviewSize = (nBufferLength - 2) / 4;  // Have a separating ' ' and a terminating null, print 4 characters per byte (e.g. "48 H" for Hello World example above).
	if(nPreviewSize > nDataSize)
		nPreviewSize = nDataSize; // Can't preview more bytes than there are.

	// Clear the ouput buffer(s).
	if(pBuffer)
	{
		memset(pBuffer, ' ', nBufferLength * sizeof(char));
		pBuffer[nBufferLength - 1] = 0;
	}
	if(pBufferW)
	{
		memset(pBufferW, ' ', nBufferLength * sizeof(wchar_t));
		pBufferW[nBufferLength - 1] = 0;
	}

	// Add a separating tab. This is the tab that separates the hex digits 
	// on the left from the ASCII text on the right.
	if(pBuffer)
		pBuffer[(nPreviewSize * 3) - 1] = '\t';
	if(pBufferW)
		pBufferW[(nPreviewSize * 3) - 1] = '\t';

	// Add each byte to the output buffer, first as ASCII, then as hex.
	for(size_t j = 0; j < nPreviewSize; j++)
	{
		// Ouput a single hex entry in the form of "00 ".
		const char hex0  = hexDigits[(pData[j] & 0xF0) >> 4];
		const char hex1  = hexDigits[(pData[j] & 0x0F)];

		if(pBuffer)
		{
			pBuffer[(j * 3)    ] = hex0;
			pBuffer[(j * 3) + 1] = hex1;
			if((pData[j] < 32) || (pData[j] >= 127) || (pData[j] == '"') || (pData[j] == '\''))
				pBuffer[(nPreviewSize * 3) + j] = '.';
			else
				pBuffer[(nPreviewSize * 3) + j] = pData[j];
		}
		if(pBufferW)
		{
			pBufferW[(j * 3)    ] = (wchar_t)hex0;
			pBufferW[(j * 3) + 1] = (wchar_t)hex1;
			if((pData[j] < 32) || (pData[j] == '"') || (pData[j] == '\''))
				pBufferW[(nPreviewSize * 3) + j] = '.';
			else
				pBufferW[(nPreviewSize * 3) + j] = (wchar_t)pData[j];
		}
	}

	return kDataPreviewTypeBinary;
}


void TraceFunctionDefault(const char* pTraceText, void* /*pContext*/)
{
	#if PPM_DEBUG || !defined(EA_PLATFORM_WINDOWS_PHONE)  // Explicitly disable this in Windows phone non-debug builds, as it is a TRC violation to link to OutputDebugStringA.
		// We cannot use puts() because it appends a newline.
		// We cannot use printf(pTraceText) because pTraceText might have formatting statements.
		if(pTraceText)
		{
			#if defined(EA_PLATFORM_MICROSOFT)
				OutputDebugStringA(pTraceText);
			#elif defined(EA_PLATFORM_ANDROID)
				__android_log_print(ANDROID_LOG_INFO, "PRINTF", "%s\n", pTraceText);
			#else
				printf("%s\n", pTraceText); // Write the message to stdout, which happens to be the trace view for many console debug machines.
			#endif
		}
	#else
	   EA_UNUSED(pTraceText); 
	#endif
}


unsigned GetPageSize()
{
	#if defined(EA_SYSTEM_PAGE_SIZE) // This is something the user would manually define in the build as an override to the values below.
		return EA_SYSTEM_PAGE_SIZE;
	#else
		#if defined(EA_PLATFORM_WINDOWS) && EA_WINAPI_FAMILY_PARTITION(EA_WINAPI_PARTITION_DESKTOP)
			SYSTEM_INFO system_info;
			GetSystemInfo(&system_info);
			return (unsigned)system_info.dwPageSize;
		#elif defined(EA_PLATFORM_UNIX) || defined(__APPLE__)
			return getpagesize();
		#elif defined(EA_PLATFORM_SONY)
			return SCE_KERNEL_PAGE_SIZE;
		#else
			return kDefaultPageSize; // For non-virtual memory systems, this number doesn't matter much.
		#endif
	#endif
}



// To consider: Have this return a bad memory location or NULL if OK.
bool MemoryFillCheck(const unsigned char* pData, size_t nDataSize, unsigned char c)
{
	const unsigned char*  pDataEnd   = pData + nDataSize;
	const uint32_t* const pDataEnd32 = (const uint32_t*)pData + (nDataSize / sizeof(uint32_t));

	if((nDataSize >= sizeof(uint32_t)) && (((uintptr_t)pData % sizeof(uint32_t)) == 0)) // If the data is 32 bits or longer and is 32 bit aligned...
	{
		// 32 bit check
		const uint32_t  nData32 = (uint32_t)((c << 24) | (c << 16) | (c << 8) | c);
		const uint32_t* pData32 = (const uint32_t*)pData; 

		while(pData32 < pDataEnd32)
		{
			if(*pData32++ != nData32)
				return false;
		}

		pData = (const unsigned char*)pData32;
	}

	// 8 bit check
	while(pData < pDataEnd)
	{
		if(*pData++ != c)
			return false;
	}

	return true;
}



#if PPM_DEBUG_FILL
	// To consider: Have this return a bad memory location or NULL if OK.
	bool DebugMemCheck(const void* pData, size_t n, unsigned char c)
	{
		bool bReturnValue;

		// Debug build allocations and frees can take a long time 
		// if you enable full fill checks via PPM_DEBUG > 1 or 2.
		#if (PPM_DEBUG >= 3)     // Developer builds only.
			const size_t kMax = 1000000000;
		#elif (PPM_DEBUG >= 2)  // Extended user debug builds.
			const size_t kMax = 1024;        
		#else 
			const size_t kMax = 16;
		#endif

		if(n < kMax)
			bReturnValue = EA::Allocator::MemoryFillCheck((const unsigned char*)pData, n, c);
		else
		{
			// In case n was too large, we just test the some of the 
			// first part and some of the last part of the Chunk.
			bReturnValue = DebugMemCheck(pData, kMax / 2, c);
			if(bReturnValue)
				bReturnValue = DebugMemCheck((const char*)pData + n - (kMax / 2), kMax / 2, c);
			// We don't need to assert here because the recursed call will do it.
		}
		return bReturnValue;
	}
#endif




} // namespace Allocator
} // namespace EA

