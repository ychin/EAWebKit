///////////////////////////////////////////////////////////////////////////////
// EAGeneralAllocatorDebugWin32
//
// Copyright (c) 2004, Electronic Arts. All Rights Reserved.
// By Paul Pedriana, Maxis
///////////////////////////////////////////////////////////////////////////////

#include <PPMalloc/internal/config.h>
#include <PPMalloc/internal/shared.h>
#include <PPMalloc/internal/callstack.h>

#include <EABase/eabase.h>
#include <PPMalloc/internal/dllinfo.h>
#include <stdio.h>

#if defined(EA_PLATFORM_WINDOWS)
	EA_DISABLE_ALL_VC_WARNINGS()
	#include <Windows.h>
	#include <DbgHelp.h>
	EA_RESTORE_ALL_VC_WARNINGS()
#endif

#if defined(EA_COMPILER_MSVC)
	#pragma warning(push, 4)
	#pragma warning(disable: 4748)     // warning: /GS can not protect parameters and local variables from local buffer overrun because optimizations are disabled in function
	#if _MSC_VER >= 1400               // VC2005 or later.
		#pragma warning(disable: 4740) // warning: flow in or out of inline asm code suppresses global optimization
	#endif
#endif



#if EA_WINAPI_FAMILY_PARTITION(EA_WINAPI_PARTITION_DESKTOP) // As it currently stands, this file will not work on XBox or other Win32-derived platforms.

#if defined(_WIN64)
	#error This file works for Win32 and not for Win64. Use EAGeneralAllocatorDebugWin64.cpp for Win64 support.
#endif


#pragma warning(disable: 4191)

typedef BOOL   (__stdcall *SYMINITIALIZE)(HANDLE, LPSTR, BOOL);
typedef BOOL   (__stdcall *SYMCLEANUP)(HANDLE);
typedef BOOL   (__stdcall *STACKWALK)(DWORD, HANDLE, HANDLE, LPSTACKFRAME, LPVOID,PREAD_PROCESS_MEMORY_ROUTINE, PFUNCTION_TABLE_ACCESS_ROUTINE,PGET_MODULE_BASE_ROUTINE, PTRANSLATE_ADDRESS_ROUTINE);
typedef LPVOID (__stdcall *SYMFUNCTIONTABLEACCESS)(HANDLE, DWORD);
typedef DWORD  (__stdcall *SYMGETMODULEBASE)(HANDLE, DWORD);
typedef BOOL   (__stdcall *SYMGETSYMFROMADDR)(HANDLE, DWORD, PDWORD, PIMAGEHLP_SYMBOL);
typedef BOOL   (__stdcall *SYMGETLINEFROMADDR)(HANDLE, DWORD, PDWORD, PIMAGEHLP_LINE);

const size_t kBufferSize = 3096;

namespace // We construct an anonymous namespace because doing so keeps the definitions within it local to this module.
{  

	struct Win32DbgHelp
	{
		HMODULE                mhDbgHelp;
		bool                   mbSymInitialized;
		SYMINITIALIZE          mpSymInitialize;
		SYMCLEANUP             mpSymCleanup;
		STACKWALK              mpStackWalk;
		SYMFUNCTIONTABLEACCESS mpSymFunctionTableAccess;
		SYMGETMODULEBASE       mpSymGetModuleBase;
		SYMGETSYMFROMADDR      mpSymGetSymFromAddr;
		SYMGETLINEFROMADDR     mpSymGetLineFromAddr;
		HANDLE                 mhCurrentProcess;

		Win32DbgHelp() : mhDbgHelp(0), mbSymInitialized(false), mpSymInitialize(0), 
						 mpSymCleanup(0), mpStackWalk(0), mpSymFunctionTableAccess(0), 
						 mpSymGetModuleBase(0), mpSymGetSymFromAddr(0), mpSymGetLineFromAddr(0), mhCurrentProcess(0)
		{
			// Empty. The initialization is done externally, due to tricky startup/shutdown ordering issues.
		}

		~Win32DbgHelp()
		{
			// Empty. The shutdown is done externally, due to tricky startup/shutdown ordering issues.
		}

		void Init()
		{
			if(!mhCurrentProcess)
			{
				// GetCurrentProcess returns a "pseudo-handle" but DbgHelp seems to prefer using a real handle.
				HANDLE hCurrentProcess = ::GetCurrentProcess();
				::DuplicateHandle(hCurrentProcess, hCurrentProcess, hCurrentProcess, &mhCurrentProcess, 0, TRUE, DUPLICATE_SAME_ACCESS);
			}

			if(!mhDbgHelp)
			{
				mhDbgHelp = ::LoadLibraryA("DbgHelp.dll");

				if(mhDbgHelp)
				{
					mpSymInitialize          = (SYMINITIALIZE)         ::GetProcAddress(mhDbgHelp, "SymInitialize");
					mpSymCleanup             = (SYMCLEANUP)            ::GetProcAddress(mhDbgHelp, "SymCleanup");
					mpStackWalk              = (STACKWALK)             ::GetProcAddress(mhDbgHelp, "StackWalk");
					mpSymFunctionTableAccess = (SYMFUNCTIONTABLEACCESS)::GetProcAddress(mhDbgHelp, "SymFunctionTableAccess");
					mpSymGetModuleBase       = (SYMGETMODULEBASE)      ::GetProcAddress(mhDbgHelp, "SymGetModuleBase");
					mpSymGetSymFromAddr      = (SYMGETSYMFROMADDR)     ::GetProcAddress(mhDbgHelp, "SymGetSymFromAddr");
					mpSymGetLineFromAddr     = (SYMGETLINEFROMADDR)    ::GetProcAddress(mhDbgHelp, "SymGetLineFromAddr");
				}
			}
		}

		void Shutdown()
		{
			if(mhDbgHelp)
			{
				if(mbSymInitialized && mpSymCleanup)
				{
					mpSymCleanup(mhCurrentProcess);

					mpSymInitialize          = NULL;
					mpSymCleanup             = NULL;
					mpStackWalk              = NULL;
					mpSymFunctionTableAccess = NULL;
					mpSymGetModuleBase       = NULL;
					mpSymGetSymFromAddr      = NULL;
					mpSymGetLineFromAddr     = NULL;
				}

				::FreeLibrary(mhDbgHelp);
			}

			if(mhCurrentProcess)
			{
				::CloseHandle(mhCurrentProcess);
				mhCurrentProcess = 0;
			}
		}

		bool LookupSymbolByAddress(void* pAddress, char* pBuffer, size_t nBufferLength)
		{
			if(!mhDbgHelp)
				Init();

			if(!mbSymInitialized && mpSymInitialize)
			{
				if(mpSymInitialize(mhCurrentProcess, 0, TRUE)) // TRUE here means to load the modules' symbol information now.
					mbSymInitialized = true;
			}

			BYTE symbolBuffer[sizeof(IMAGEHLP_SYMBOL) + kBufferSize];
			IMAGEHLP_SYMBOL* pSymbol = (PIMAGEHLP_SYMBOL)symbolBuffer;
			pSymbol->SizeOfStruct    = sizeof(symbolBuffer);
			pSymbol->MaxNameLength   = kBufferSize;
			DWORD symDisplacement    = 0;  // Displacement of the input address, relative to the start of the symbol

			if(mpSymGetSymFromAddr && mpSymGetSymFromAddr(mhCurrentProcess, (DWORD)((uintptr_t)pAddress), &symDisplacement, pSymbol))
			{
				pSymbol->Name[pSymbol->MaxNameLength - 1] = 0;
				strncpy(pBuffer, pSymbol->Name, nBufferLength);
				return true;
			}
			return false;
		}
	};

	static int          sInitCount = 0;
	static Win32DbgHelp sWin32DbgHelp;
}



namespace EA {

namespace Allocator {

///////////////////////////////////////////////////////////////////////////////
// DescribeCallStack
//
PPM_API size_t DescribeCallStack(const void* ppReturnAddressArray[], size_t nReturnAddressArrayCount, char* pBuffer, size_t nBufferLength, EA::Callstack::IAddressRepLookupBase*)
{
	char* pBufferCurrent = pBuffer;
	pBufferCurrent[0] = 0;
	nBufferLength--; // Subtract one for 0 terminator.

	if(!sWin32DbgHelp.mhDbgHelp)
		sWin32DbgHelp.Init(); // Note that we intentionally don't call InitCallstack().

	if(!sWin32DbgHelp.mbSymInitialized && sWin32DbgHelp.mpSymInitialize)
	{
		if(sWin32DbgHelp.mpSymInitialize(sWin32DbgHelp.mhCurrentProcess, 0, TRUE)) // TRUE here means to load the modules' symbol information now.
			sWin32DbgHelp.mbSymInitialized = true;
		else
		{
			// SymInitialize failed
			//const DWORD error = GetLastError();
			//EA_UNUSED(error);
		}
	}

	if(sWin32DbgHelp.mbSymInitialized)
	{
		for(size_t i(0); i < nReturnAddressArrayCount; i++)
		{
			const void* const pAddress = ppReturnAddressArray[i];

			if((i > 0) && (nBufferLength > 0))
			{
				*pBufferCurrent++ = ' ';
				nBufferLength -= 1;
			}

			// IMAGEHLP is wacky, and requires you to pass in a pointer to an
			// IMAGEHLP_SYMBOL structure. The problem is that this structure is
			// variable length. That is, you determine how big the structure is
			// at runtime. This means that you can't use sizeof(struct).
			// So...make a buffer that's big enough, and make a pointer
			// to the buffer. We also need to initialize not one, but TWO
			// members of the structure before it can be used.

			BYTE symbolBuffer[sizeof(IMAGEHLP_SYMBOL) + kBufferSize];
			IMAGEHLP_SYMBOL* pSymbol = (PIMAGEHLP_SYMBOL)symbolBuffer;
			pSymbol->SizeOfStruct    = sizeof(symbolBuffer);
			pSymbol->MaxNameLength   = kBufferSize;
			DWORD symDisplacement    = 0;  // Displacement of the input address, relative to the start of the symbol

			if(sWin32DbgHelp.mpSymGetSymFromAddr && sWin32DbgHelp.mpSymGetSymFromAddr(sWin32DbgHelp.mhCurrentProcess, (DWORD)((uintptr_t)pAddress), &symDisplacement, pSymbol))
			{
				char pSymbolText[kBufferSize + 64];

				pSymbol->Name[pSymbol->MaxNameLength - 1] = 0;

				size_t nStrlen = (size_t)(unsigned)sprintf(pSymbolText, "%hs() + %lu", pSymbol->Name, symDisplacement); // Possibly take the pains to get this to work with the various snprintf incarnations available.

				if(nStrlen < nBufferLength)
				{
					strncpy(pBufferCurrent, pSymbolText, nStrlen);

					pBufferCurrent += nStrlen;
					nBufferLength  -= nStrlen;
				}

				if(sWin32DbgHelp.mpSymGetLineFromAddr)
				{
					IMAGEHLP_LINE imageHelpLine;
					imageHelpLine.SizeOfStruct = sizeof(IMAGEHLP_LINE);

					if(sWin32DbgHelp.mpSymGetLineFromAddr(sWin32DbgHelp.mhCurrentProcess, (DWORD)((uintptr_t)pAddress), &symDisplacement, &imageHelpLine))
					{
						nStrlen = (size_t)(unsigned)sprintf(pSymbolText, " \"%hs\", line %lu", imageHelpLine.FileName, imageHelpLine.LineNumber);

						if(nStrlen < nBufferLength)
						{
							strncpy(pBufferCurrent, pSymbolText, nStrlen);

							pBufferCurrent += nStrlen;
							nBufferLength  -= nStrlen;
						}
					}
				}
			}
			else // Else just print the address.
			{
				// SymGetSymFromAddr failed.
				//const DWORD error = GetLastError();
				//EA_UNUSED(error);

				if(nBufferLength >= ((sizeof(void*) * 2) + 2)) // The second '2' is for the "0x" prefix.
				{
					char buffer[32];

					#if defined(EA_PLATFORM_PTR_SIZE) && (EA_PLATFORM_PTR_SIZE == 8)
						const size_t nStrlen = (size_t)(unsigned)sprintf(buffer, "0x%016" PRIx64, (uintptr_t)pAddress);
					#else
						const size_t nStrlen = (size_t)(unsigned)sprintf(buffer, "0x%08x", (uintptr_t)pAddress);
					#endif

					strcpy(pBufferCurrent, buffer); // This will always have enough space.

					pBufferCurrent += nStrlen;
					nBufferLength  -= nStrlen;
				}
			}
		}
	}

	return (size_t)(pBufferCurrent - pBuffer);
}



} // namespace Allocator


} // namespace EA


#endif // EA_PLATFORM_WINDOWS


#ifdef _MSC_VER
	#pragma warning(pop)
#endif

