///////////////////////////////////////////////////////////////////////////////
// Copyright (c) Electronic Arts Inc. All rights reserved.
///////////////////////////////////////////////////////////////////////////////


#include <coreallocator/icoreallocatormacros.h>
#include <EACallstack/internal/Config.h>
#include <EACallstack/internal/EACallstackUtils.h>
#include <EACallstack/EAAddressRep.h>
#include <EACallstack/Allocator.h>
#include <EACallstack/MapFile.h>
#include <EACallstack/PDBFile.h>
#include <EACallstack/DWARF3File.h>
#include <EACallstack/EACallstack.h>
#include <EACallstack/EASymbolUtil.h>
#include <EACallstack/internal/ELF.h>
#include <EAStdC/EAString.h>
#include <EAStdC/EASprintf.h>
#include <EAStdC/EAScanf.h>
#include <EAStdC/EAMemory.h>
#include <EAStdC/EATextUtil.h>
#include <EAStdC/EAProcess.h>
#include <EASTL/fixed_substring.h>
#include <EAIO/PathString.h>
#include <EAIO/EAFileStream.h>
#include <EAIO/EAStreamAdapter.h>
#include <EAIO/EAFileUtil.h>
#include <EAIO/EAFileDirectory.h>
#include <EAAssert/eaassert.h>
#include <stdio.h>
#include <new>


namespace EA
{
namespace Callstack
{

///////////////////////////////////////////////////////////////////////////////
// AddressToString
//
EACALLSTACK_API CAString8& AddressToString(uint64_t address, CAString8& s, bool bLeading0x)
{
	const char* const pFormat64 = "0x%016" PRIx64;
	const char* const pFormat32 = "0x%08" PRIx64;
	const char*       pFormat   = (address > UINT32_MAX) ? pFormat64 : pFormat32;

	return static_cast<CAString8&>(s.sprintf(bLeading0x ? pFormat : pFormat + 2, address));
}


///////////////////////////////////////////////////////////////////////////////
// AddressToString
//
EACALLSTACK_API FixedString& AddressToString(uint64_t address, FixedString& s, bool bLeading0x)
{
	const char* const pFormat64 = "0x%016" PRIx64;
	const char* const pFormat32 = "0x%08" PRIx64;
	const char*       pFormat   = (address > UINT32_MAX) ? pFormat64 : pFormat32;

	return static_cast<FixedString&>(s.sprintf(bLeading0x ? pFormat : pFormat + 2, address));
}


///////////////////////////////////////////////////////////////////////////////
// GetSymbolInfoTypeFromDatabase
//
EACALLSTACK_API SymbolInfoType GetSymbolInfoTypeFromDatabase(const wchar_t* pDatabaseFilePath, TargetOS* pTargetOS, TargetProcessor* pTargetProcessor)
{
	IO::FileStream  fileStream(pDatabaseFilePath);
	SymbolInfoType  result          = kSymbolInfoTypeNone;
	TargetOS        targetOS        = kTargetOSNone;
	TargetProcessor targetProcessor = kTargetProcessorNone;

	fileStream.AddRef();

	if(fileStream.Open(IO::kAccessFlagRead))
	{
		char buffer[4096];
		EA::StdC::Memset8(buffer, 0, sizeof(buffer));

		if(fileStream.Read(buffer, sizeof(buffer) - 1) != IO::kSizeTypeError)
		{
			const wchar_t* const pExtension = IO::Path::GetFileExtension(pDatabaseFilePath);

			// Check for a Sony .self header.
			if((buffer[0] ==  'S') && (buffer[1] ==  'C') && (buffer[2] ==  'E') && (buffer[3] ==  0))
			{
				// A SELF file contains an ELF file. We find that ELF file within the SELF file and
				// read it into our buffer, then just fall through and let the code detect an ELF file.
				uint64_t nELFLocation = 0;
				fileStream.SetPosition(16);
				EA::IO::ReadUint64(&fileStream, nELFLocation, EA::IO::kEndianBig);
				fileStream.SetPosition((EA::IO::off_type)nELFLocation);
				fileStream.Read(buffer, sizeof(buffer) - 1);
				// Fall through to .elf checking
			}

			// Check for an .elf header or a Sony .self header.
			if((buffer[0] == 0x7f) && (buffer[1] ==  'E') && (buffer[2] ==  'L') && (buffer[3] == 'F'))
			{
				using namespace EA::Callstack::ELF;

				result = kSymbolInfoTypeDWARF3;

			  //const bool b64BitELF        = (buffer[EI_CLASS] == ELFCLASS64);
				const EA::IO::Endian endian = (buffer[EI_DATA]  == ELFDATA2MSB) ? EA::IO::kEndianBig : EA::IO::kEndianLittle;

				// Detect the processor first.
				uint16_t elfProcessor;
				EA::StdC::Memcpy(&elfProcessor, buffer + 18, sizeof(uint16_t));
				if(endian != EA::IO::kEndianLocal)
					elfProcessor = Utils::SwizzleUint16(elfProcessor);

				if(elfProcessor == EM_X86_64)
					targetProcessor = kTargetProcessorX64;
				else if((elfProcessor == EM_386) || (elfProcessor == EM_486))
					targetProcessor = kTargetProcessorX86;
				else if(elfProcessor == EM_PPC)
					targetProcessor = kTargetProcessorPowerPC32;
				else if(elfProcessor == EM_PPC64)
					targetProcessor = kTargetProcessorPowerPC64;
				else if(elfProcessor == EM_ARM)
					targetProcessor = kTargetProcessorARM;

				// Detect the target OS.
				if(buffer[EI_OSABI] == ELFOSABI_LINUX) // Actually, Linux usually uses ELFOSABI_NONE, but we'll catch that below.
					targetOS = kTargetOSLinux;
				else if(buffer[EI_OSABI] == ELFOSABI_NONE)
				{
					// It turns out that a number of platforms use ELFOSABI_NONE. In this case the only correct solution
					// is to iterate the sections and look for OS-specific info.
					targetOS = kTargetOSLinux; // or kTargetOSOSX
				}
			}
			else if((EA::StdC::Stricmp(pExtension, L".map") == 0) ||
					(EA::StdC::Stricmp(pExtension, L".txt") == 0))
			{
				if(EA::StdC::Strstr(buffer, "Timestamp is"))
				{
					result = kSymbolInfoTypeMapVC;

					if(EA::StdC::Strstr(buffer, "Rva+Base               Lib:Object") || EA::StdC::Strstr(buffer, "load address is 00000"))
					{
						targetOS        = kTargetOSWin64;
						targetProcessor = kTargetProcessorX64;
					}
					else
					{
						targetOS        = kTargetOSWin32;
						targetProcessor = kTargetProcessorX86;
					}
				}

				// SN map files have a header that look like this:
				// Address  Size     Align Out     In      File    Symbol
				// =================================================================
				else if(EA::StdC::Strstr(buffer, "Address  Size     Align Out     In      File    Symbol"))
				{
					result = kSymbolInfoTypeMapSN;
				}

				// Apple's map files have a header that look like this:
				// # Address Size        File  Name
				// =================================================================
				else if(EA::StdC::Strstr(buffer, "# Path:") &&
						EA::StdC::Strstr(buffer, "# Arch:"))
				{
					result = kSymbolInfoTypeMapApple;

					if(EA::StdC::Strstr(buffer, "# Arch: x86_64"))
					{
						targetOS        = kTargetOSOSX;
						targetProcessor = kTargetProcessorX64;
					}
					if(EA::StdC::Strstr(buffer, "# Arch: i386"))
					{
						targetOS        = kTargetOSOSX;         // It could instead be iOS emulation on OSX, though actually that would still indicate an OSX application.
						targetProcessor = kTargetProcessorX86;
					}
					else if(EA::StdC::Strstr(buffer, "# Arch: ppc"))
					{
						targetOS        = kTargetOSOSX;
						targetProcessor = kTargetProcessorPowerPC32; // Could also be 64.
					}
					else
					{
						targetOS        = kTargetOSIOS;
						targetProcessor = kTargetProcessorARM;
					}
				}

				// GCC map files aren't highly distinctive. It's harder to tell by looking
				// at a given text file and programmatically tell if it's a GCC .map file.
				// So for now we just always say that it is.
				else
				{
					result = kSymbolInfoTypeMapGCC;

					if(EA::StdC::Stristr(buffer, "android"))
					{
						targetOS        = kTargetOSAndroid;
						targetProcessor = kTargetProcessorARM;
					}
					else if(EA::StdC::Strstr(buffer, "/usr/lib"))
					{
						if(EA::StdC::Strstr(buffer, "linux"))
							targetOS = kTargetOSLinux;
						else if(EA::StdC::Strstr(buffer, "apple"))
							targetOS = kTargetOSOSX;

						// The following assumes the target processor is x86 or x64, but it doesn't work right for apple (Mac OS X), because it uses a single /lib directory for everything.
						if(EA::StdC::Strstr(buffer, "/lib64") || EA::StdC::Strstr(buffer, "/lib/64"))
							targetProcessor = kTargetProcessorX64;
						else
							targetProcessor = kTargetProcessorX86;
					}
				}
			}

			else if(EA::StdC::Stricmp(pExtension, L".pdb") == 0)
			{
				if(EA::StdC::Strstr(buffer, "Microsoft C/C++ MSF 7.00") == buffer)
					result = kSymbolInfoTypePDB7;

				else if(EA::StdC::Strstr(buffer, "Microsoft C/C++ MSF 8.00") == buffer)
					result = kSymbolInfoTypePDB8;

				// To do: Find out how to tell these correctly.
				targetOS        = kTargetOSWin64;           // kTargetOSWin32, kTargetOSWin64
				targetProcessor = kTargetProcessorX64;		// kTargetProcessorX64
			}
		}
	}

	if(pTargetOS)
		*pTargetOS = targetOS;

	if(pTargetProcessor)
		*pTargetProcessor = targetProcessor;

	return result;
}


///////////////////////////////////////////////////////////////////////////////
// GetCurrentOS
//
TargetOS GetCurrentOS()
{
	#if defined(EA_PLATFORM_IPHONE)
		return kTargetOSIOS;
	#elif defined(EA_PLATFORM_ANDROID)
		return kTargetOSAndroid;
	#elif defined(EA_PLATFORM_OSX)
		return kTargetOSOSX;
	#elif defined(EA_PLATFORM_LINUX)
		return kTargetOSLinux;
	#elif defined(EA_PLATFORM_WIN32)
		return kTargetOSWin32;             // It's possible that we are in fact running under Win64, though within the Win32 WoW of Win64.
	#elif defined(EA_PLATFORM_WIN64)
		return kTargetOSWin64;
	#elif defined(EA_PLATFORM_MICROSOFT) && defined(EA_PLATFORM_MOBILE)
		return kTargetOSMicrosoftMobile;
	#elif defined(EA_PLATFORM_MICROSOFT) && defined(EA_PLATFORM_CONSOLE)
		return kTargetOSMicrosoftConsole;
	#else
		return kTargetOSNone;
	#endif
}


///////////////////////////////////////////////////////////////////////////////
// GetCurrentProcessor
//
TargetProcessor GetCurrentProcessor()
{
	#if defined(EA_PROCESSOR_POWERPC)
		return kTargetProcessorPowerPC64;
	#elif defined(EA_PROCESSOR_X86)
		return kTargetProcessorX86;
	#elif defined(EA_PROCESSOR_X86_64)
		return kTargetProcessorX64;
	#elif defined(EA_PROCESSOR_ARM)
		return kTargetProcessorARM;
	#else
		return kTargetProcessorNone;
	#endif
}


///////////////////////////////////////////////////////////////////////////////
// MakeAddressRepLookupFromDatabase
//
EACALLSTACK_API IAddressRepLookup* MakeAddressRepLookupFromDatabase(const wchar_t* pDatabaseFilePath,
																	EA::Allocator::ICoreAllocator* pCoreAllocator,
																	SymbolInfoType symbolInfoType)
{
	if(!pCoreAllocator)
	{
		pCoreAllocator = EA::Callstack::GetAllocator();
		EA_ASSERT(pCoreAllocator != NULL); // If this fails, did you forget to call EA::Callstack::SetAllocator?
	}

	if(symbolInfoType == kSymbolInfoTypeNone)
		symbolInfoType = GetSymbolInfoTypeFromDatabase(pDatabaseFilePath);

	if(symbolInfoType != kSymbolInfoTypeNone)
	{
		void* pMemory;

		switch(symbolInfoType)
		{
			case kSymbolInfoTypeMapVC:
			{
				#if EACALLSTACK_MSVC_MAPFILE_ENABLED
					pMemory = pCoreAllocator->Alloc(sizeof(MapFileMSVC), EACALLSTACK_ALLOC_PREFIX "EACallstack/MapFileMSVC", 0);

					if(pMemory)
					{
						MapFileMSVC* const pMapFileMSVC = new(pMemory) MapFileMSVC(pCoreAllocator);
						return pMapFileMSVC;
					}
				#endif

				break;
			}

			case kSymbolInfoTypeMapGCC:
			{
				#if EACALLSTACK_GCC_MAPFILE_ENABLED
					pMemory = pCoreAllocator->Alloc(sizeof(MapFileGCC3), EACALLSTACK_ALLOC_PREFIX "EACallstack/MapFileGCC3", 0);

					if(pMemory)
					{
						MapFileGCC3* const pMapFileGCC3 = new(pMemory) MapFileGCC3(pCoreAllocator);
						return pMapFileGCC3;
					}
				#endif

				break;
			}

			case kSymbolInfoTypeMapSN:
			{
				#if EACALLSTACK_SN_MAPFILE_ENABLED
					pMemory = pCoreAllocator->Alloc(sizeof(MapFileSN), EACALLSTACK_ALLOC_PREFIX "EACallstack/MapFileSN", 0);

					if(pMemory)
					{
						MapFileSN* const pMapFileSN = new(pMemory) MapFileSN(pCoreAllocator);
						return pMapFileSN;
					}
				#endif

				break;
			}

			case kSymbolInfoTypePDB7:
			case kSymbolInfoTypePDB8:
			{
				#if EACALLSTACK_PDBFILE_ENABLED
					pMemory = pCoreAllocator->Alloc(sizeof(PDBFile), EACALLSTACK_ALLOC_PREFIX "EACallstack/PDBFile", 0);

					if(pMemory)
					{
						PDBFile* const pPDBFile = new(pMemory) PDBFile(pCoreAllocator);
						return pPDBFile;
					}
				#endif

				break;
			}

			case kSymbolInfoTypeDWARF3:
			{
				#if EACALLSTACK_DWARFFILE_ENABLED
					pMemory = pCoreAllocator->Alloc(sizeof(DWARF3File), EACALLSTACK_ALLOC_PREFIX "EACallstack/DWARF3File", 0);

					if(pMemory)
					{
						DWARF3File* const pDWARF3File = new(pMemory) DWARF3File(pCoreAllocator);
						return pDWARF3File;
					}
				#endif

				break;
			}

			case kSymbolInfoTypeMapApple:
			{
				#if EACALLSTACK_APPLE_MAPFILE_ENABLED
					pMemory = pCoreAllocator->Alloc(sizeof(MapFileApple), EACALLSTACK_ALLOC_PREFIX "EACallstack/MapFileApple", 0);

					if(pMemory)
					{
						MapFileApple* const pMapFileApple = new(pMemory) MapFileApple(pCoreAllocator);
						return pMapFileApple;
					}
				#endif

				break;
			}

			// Not currently supported.
			case kSymbolInfoTypeNone:
				break;
		}
	}

	return NULL;
}


EACALLSTACK_API void DestroyAddressRepLookup(IAddressRepLookup* pLookup, EA::Allocator::ICoreAllocator* pCoreAllocator)
{
	if(pLookup)
	{
		if(!pCoreAllocator)
		{
			pCoreAllocator = EA::Callstack::GetAllocator();
			EA_ASSERT(pCoreAllocator != NULL); // If this fails, did you forget to call EA::Callstack::SetAllocator?
		}

		pLookup->~IAddressRepLookup();
		pCoreAllocator->Free(pLookup);
	}
}


} // namespace Callstack
} // namespace EA
