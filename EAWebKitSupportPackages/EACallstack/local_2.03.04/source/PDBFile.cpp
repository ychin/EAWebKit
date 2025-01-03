///////////////////////////////////////////////////////////////////////////////
// Copyright (c) Electronic Arts Inc. All rights reserved.
///////////////////////////////////////////////////////////////////////////////

#include <EACallstack/PDBFile.h>

#if EACALLSTACK_PDBFILE_ENABLED

#include <EACallstack/EAAddressRep.h>
#include <EACallstack/Allocator.h>
#include <EAAssert/eaassert.h>
#include <EACallstack/EASymbolUtil.h>

#if EA_WINAPI_FAMILY_PARTITION(EA_WINAPI_PARTITION_DESKTOP)

///////////////////////////////////////////////////////////////////////////////
// PDBFileMS
///////////////////////////////////////////////////////////////////////////////

#include <EAStdC/EAString.h>
#include <EAStdC/EATextUtil.h>
#include <EAIO/EAFileUtil.h>
#include <EAAssert/eaassert.h>


EA_DISABLE_ALL_VC_WARNINGS();

#include <Windows.h>
#include <DbgHelp.h>
#include <Psapi.h>
//#include <cvconst.h> // This header defines enum SymTagEnum, etc. but this header doesn't exist in VC++, at least up till VS2008. It's present in the DIA SDK.

EA_RESTORE_ALL_VC_WARNINGS();

#if defined(_MSC_VER)
	#pragma comment(lib, "dbghelp.lib")
	#pragma comment(lib, "psapi.lib")
#endif


#if defined(EA_DEBUG) && defined(EA_ASSERT_ENABLED) && EA_WINAPI_FAMILY_PARTITION(EA_WINAPI_PARTITION_DESKTOP)
	static void FormatWindowsError(DWORD dwError, char* buffer, size_t capacity)
	{
		char* pError;

		DWORD error = FormatMessageA(FORMAT_MESSAGE_ALLOCATE_BUFFER | FORMAT_MESSAGE_FROM_SYSTEM | FORMAT_MESSAGE_IGNORE_INSERTS, NULL, dwError, MAKELANGID(LANG_NEUTRAL, SUBLANG_DEFAULT), (LPSTR)&pError, 0, NULL);

		if (error != 0)
		{
			EA::StdC::Strlcpy(buffer, pError, capacity);
			LocalFree(pError);
		}
	}
#endif


namespace EA
{
namespace Callstack
{

// It turns out that the Windows Sym functions have a weakness in that they reject the user setting
// the base address to 0, for no apparently outward useful reason. So when we call SymLoadModule64 we
// can't use 0, as the function will fail. To get around that limitation we always use a different
// value (kPDBFileFake0Offset) in place of 0. This is OK as long as all user-supplied addresses
// (which are 0-based) have kPDBFileFake0Offset added to them before calling the SymGet* functions.
// Experimentation has shown that we can't set kPDBFileFake0Offset to a value like 1, as that will
// result in SymLoadModule64 succeeding but SymGetLineFromAddr64 failing. So we pick an alternative
// that's known to work.
static const uint64_t kPDBFileFake0Offset = 0x1000;


///////////////////////////////////////////////////////////////////////////////
// PDBFileMS
//
PDBFileMS::PDBFileMS(EA::Allocator::ICoreAllocator* /*pCoreAllocator*/)
  : mhSymbols(NULL),
	mbOfflineLookup(false),
	mBaseAddress(kBaseAddressUnspecified),
	mSymModuleLoadedBaseAddress(UINT64_MAX),
	msPDBFilePath()
{
}


///////////////////////////////////////////////////////////////////////////////
// ~PDBFileMS
//
PDBFileMS::~PDBFileMS()
{
	Shutdown();
}


///////////////////////////////////////////////////////////////////////////////
// SetAllocator
//
void PDBFileMS::SetAllocator(EA::Allocator::ICoreAllocator* /*pCoreAllocator*/)
{
	// We don't currently allocate any memory.
}


///////////////////////////////////////////////////////////////////////////////
// Init
//
bool PDBFileMS::Init(const wchar_t* pPDBFilePath, bool /*bDelayLoad*/)
{
	// We ignore bDelayLoad; all we do is initialize the DebugHelp library.
	DWORD          dwError;
	DWORD64        dwLoadResult;
	bool           bGuessBaseAddress = false;
	const uint64_t baseAddressGuesses[3] = { 0x82000000, 0x00400000, 0x0000000140000000i64 }; // XBox 360, Win32, Win64/XBox One defaults.

	if(pPDBFilePath)
		msPDBFilePath = pPDBFilePath;

	if(!mhSymbols)
	{
		mhSymbols = CreateEvent(NULL, FALSE, FALSE, NULL);
		EA_ASSERT(mhSymbols != NULL);
	}

	// Convert the input path to UTF8, required by the SymLoadModule64 call.
	char pPath8[IO::kMaxPathLength];
	EA::StdC::Strlcpy(pPath8, msPDBFilePath.c_str(), IO::kMaxPathLength);

	if(mBaseAddress == kBaseAddressUnspecified) // If the base address hasn't been set...
	{
		// Given that the SymLoadModule64 call below requires a base address,
		// it's really best if the user manually supplies the base address
		// instead of us trying to guess it here.

		// We use the base address of the module (i.e. exe or dll) associated with our .pdb file.
		// This can't work if we are reading a symbol file from an application
		// other than the currently executing one (i.e. if we are offline).

		if(!mbOfflineLookup)
		{
			// This should always work as long as the module is valid, as the APIs to get a module's base
			mBaseAddress = Callstack::GetModuleBaseAddress(pPath8);
			if(mBaseAddress == kBaseAddressInvalid)
				mBaseAddress = kBaseAddressUnspecified;
		}

		if(mBaseAddress == kBaseAddressUnspecified) // We don't test mbOfflineLookup because we have no choice but to provide a non-zero base address and unless we guess something then we will fail SymLoadModule64 below.
		{
			// Since we are currently executing on a PC, we're going to first guess that
			// the .pdb file is an XBox PDB file. By default, XBox PDB files use a base
			// address of 0x82000000, while Win32 uses an address of 0x00400000, and
			// Win64 uses an address of 0x0000000140000000.
			bGuessBaseAddress = true;
			mBaseAddress      = baseAddressGuesses[0];
		}
	}

	// Get the size of the database file to be loaded.
	uint32_t pdbFileSize = (uint32_t)IO::File::GetSize(pPDBFilePath);

	// SymSetOptions
	// The function returns the current options mask.
	::SymSetOptions( SYMOPT_LOAD_LINES | SYMOPT_LOAD_ANYTHING); //SYMOPT_UNDNAME removed and done later manually

	// SymInitialize
	// If the function succeeds, the return value is TRUE.
	// If the function fails, the return value is FALSE. To retrieve extended error information, call GetLastError.
	BOOL result = ::SymInitialize(mhSymbols, NULL, FALSE);

	if(!result)
	{
		#if defined(EA_DEBUG) && defined(EA_ASSERT_ENABLED) && EA_WINAPI_FAMILY_PARTITION(EA_WINAPI_PARTITION_DESKTOP)
			char buffer[256];
			FormatWindowsError(GetLastError(), buffer, 256);
			EA_FAIL_MSG(buffer);
		#endif

		return false;
	}

	// SymLoadModule64
	// If the function succeeds, the return value is the base address of the loaded module.
	// If the function fails, the return value is zero. To retrieve extended error information, call GetLastError.
	// If the module is already loaded, the return value is zero and GetLastError returns ERROR_SUCCESS.
	// SymLoadModule64 doesn't allow a base address of 0, but sometimes that's what we want to use because the
	// addresses we will be supplying will be 0-based instead of actual runtime addresses.
	uint64_t baseAddressToUse = (mBaseAddress == 0 ? kPDBFileFake0Offset : mBaseAddress);
	dwLoadResult = ::SymLoadModule64(mhSymbols, NULL, pPath8, NULL,
									 baseAddressToUse,    // Non-zero value required when loading a .pdb file.
									 pdbFileSize);        // Non-zero size required when loading a .pdb file.

	if(dwLoadResult) // If SymLoadModule64 succeeded...
	{
		mSymModuleLoadedBaseAddress = baseAddressToUse;

		// If we are guessing the base address, then do a little test to see if the guess is sane.
		if(bGuessBaseAddress)
		{
			for(int i = 1; (i < 2) && dwLoadResult; i++)
			{
				// To do: Update the following code to use SymFromAddr, as Microsoft has deprecated SymGetSymFromAddr64.
				struct  SymInfo{ IMAGEHLP_SYMBOL64 pSymbol; CHAR namebuf[2048]; };
				SymInfo symInfo;
				DWORD64 displacement;

				symInfo.pSymbol.SizeOfStruct = sizeof(IMAGEHLP_SYMBOL64);
				symInfo.pSymbol.MaxNameLength = 2048;

				if(SymGetSymFromAddr64(mhSymbols, mBaseAddress + 256, &displacement, &symInfo.pSymbol)) // Test an address that is right after the base address.
					break; // Success

				SymUnloadModule64(mhSymbols, mSymModuleLoadedBaseAddress);
				mSymModuleLoadedBaseAddress = UINT64_MAX;

				mBaseAddress     = baseAddressGuesses[i];
				baseAddressToUse = (mBaseAddress == 0 ? kPDBFileFake0Offset : mBaseAddress);
				dwLoadResult     = ::SymLoadModule64(mhSymbols, NULL, pPath8, NULL, mBaseAddress, pdbFileSize);
				if(dwLoadResult)
					mSymModuleLoadedBaseAddress = baseAddressToUse;
			}
		}
	}
	else
	{
		dwError = GetLastError();

		if(dwError == ERROR_SUCCESS)
			mBaseAddress = kPDBFileFake0Offset;
		else
		{
			#if defined(EA_DEBUG) && defined(EA_ASSERT_ENABLED) && EA_WINAPI_FAMILY_PARTITION(EA_WINAPI_PARTITION_DESKTOP)
				char buffer[256];
				FormatWindowsError(dwError, buffer, 256);
				EA_FAIL_MSG(buffer);
			#endif
		}
	}

	return (mBaseAddress != kBaseAddressUnspecified);
}


///////////////////////////////////////////////////////////////////////////////
// Shutdown
//
bool PDBFileMS::Shutdown()
{
	if(mhSymbols)
	{
		if(mSymModuleLoadedBaseAddress != UINT64_MAX)
		{
			SymUnloadModule64(mhSymbols, mSymModuleLoadedBaseAddress);
			mSymModuleLoadedBaseAddress = UINT64_MAX;
		}

		SymCleanup(mhSymbols);
		CloseHandle(mhSymbols);
		mhSymbols = NULL;

		mBaseAddress = 0; // Question: Should we do this or just leave it as-is?
	}

	return true;
}


///////////////////////////////////////////////////////////////////////////////
// AsInterface
//
void* PDBFileMS::AsInterface(int id)
{
	if(id == PDBFileMS::kTypeId)
		return static_cast<PDBFileMS*>(this); // The cast isn't necessary, but makes the intent more clear.
	return NULL;
}


///////////////////////////////////////////////////////////////////////////////
// SetBaseAddress
//
void PDBFileMS::SetBaseAddress(uint64_t baseAddress)
{
	mBaseAddress = baseAddress;
	if(mhSymbols) // If we've already been initialized...
	{
		// We call SymUnloadModule64 then call SymLoadModule64 with a new base address.
		uint64_t baseAddressToUse = (mBaseAddress == 0 ? kPDBFileFake0Offset : mBaseAddress);
		if(baseAddressToUse != mSymModuleLoadedBaseAddress)
		{
			if(mSymModuleLoadedBaseAddress != UINT64_MAX) // If we already have the SymModule loaded (this should usually be true)...
			{
				::SymUnloadModule64(mhSymbols, mSymModuleLoadedBaseAddress);
				mSymModuleLoadedBaseAddress = UINT64_MAX;
			}

			// We need to set up some stuff before calling SymLoadModule64.
			char pPath8[IO::kMaxPathLength];
			EA::StdC::Strlcpy(pPath8, msPDBFilePath.c_str(), IO::kMaxPathLength);

			DWORD    pdbFileSize      = (DWORD)IO::File::GetSize(msPDBFilePath.c_str());
			DWORD64  dwLoadResult     = ::SymLoadModule64(mhSymbols, NULL, pPath8, NULL, baseAddressToUse, pdbFileSize);
			if(dwLoadResult) // If succeeded...
			{
				mSymModuleLoadedBaseAddress = baseAddressToUse;
			}
		}
	}
}


///////////////////////////////////////////////////////////////////////////////
// GetBaseAddress
//
uint64_t PDBFileMS::GetBaseAddress() const
{
	return mBaseAddress;
}


///////////////////////////////////////////////////////////////////////////////
// GetDatabasePath
//
const wchar_t* PDBFileMS::GetDatabasePath() const
{
	return msPDBFilePath.c_str();
}


///////////////////////////////////////////////////////////////////////////////
// SetOption
//
void PDBFileMS::SetOption(int option, int value)
{
	// Most options are not supported, as we are entirely going through the Microsoft debug SDK.

	if(option == kOptionOfflineLookup)
		mbOfflineLookup = (value != 0);
}


///////////////////////////////////////////////////////////////////////////////
// GetAddressRep
//
int PDBFileMS::GetAddressRep(int addressRepTypeFlags, uint64_t nAddress, FixedString* pRepArray, int* pIntValueArray)
{
	int returnFlags = 0;

	// If the base address is 0 then that means the addresses the user is specifying are 0-based.
	// It turns out that the Windows Sym functions have a weakness in that they reject the user setting
	// the base address to 0, for no apparently outward useful reason. So when we earlier called
	// SymLoadModule64 we used kPDBFileFake0Offset instead of 0 in order to get around the Windows API
	// limitation. As a result of doing that we need to add kPDBFileFake0Offset to all addresses we
	// pass to SymGet* functions. See the kPDBFileFake0Offset declaration for more info.
	if(mBaseAddress == 0)
		nAddress += kPDBFileFake0Offset;

	if(addressRepTypeFlags & kARTFlagFileLine)
	{
		IMAGEHLP_LINE64 lineInfo = { sizeof IMAGEHLP_LINE64 };
		DWORD           displacement = 0;
		BOOL            result = SymGetLineFromAddr64(mhSymbols, nAddress, &displacement, &lineInfo);

		if(result)
		{
			pRepArray[kARTFileLine]      = lineInfo.FileName;
			pIntValueArray[kARTFileLine] = (int)lineInfo.LineNumber;
			returnFlags                 |= kARTFlagFileLine;
		}
		else
		{
			#if 0 // defined(EA_DEBUG) && defined(EA_ASSERT_ENABLED) && EA_WINAPI_FAMILY_PARTITION(EA_WINAPI_PARTITION_DESKTOP)
				char  buffer[256];
				DWORD dwError = GetLastError();
				FormatWindowsError(dwError, buffer, 256);
				//EA_FAIL_MSG(buffer);
			#endif
		}
	}

	// To consider: Ignore function/offset if file/line was present.
	if(addressRepTypeFlags & kARTFlagFunctionOffset)
	{
		struct SymbolInfoPlus
		{
			SYMBOL_INFO mSymbolInfo;
			CHAR        mName[2048];
		};

		SymbolInfoPlus symbolInfo;
		DWORD64        displacement = 0;

		memset(&symbolInfo, 0, sizeof(symbolInfo));
		symbolInfo.mSymbolInfo.SizeOfStruct = sizeof(SYMBOL_INFO);
		symbolInfo.mSymbolInfo.MaxNameLen   = sizeof(symbolInfo.mName) + 1;

		BOOL result = SymFromAddr(mhSymbols, nAddress, &displacement, &symbolInfo.mSymbolInfo);

		// local def of enum values to avoid dependency
		// https://msdn.microsoft.com/en-us/library/bkedss5f(v=VS.100).aspx
		static const int kLocalSymTagNull = 0;
		static const int kLocalSymTagFunction = 5;
		static const int kLocalSymTagLabel = 0xa; // DRM technology converts functions to labels

		if(result
			&& ((symbolInfo.mSymbolInfo.Tag == kLocalSymTagNull)
			|| (symbolInfo.mSymbolInfo.Tag == kLocalSymTagFunction
			|| (symbolInfo.mSymbolInfo.Tag == kLocalSymTagLabel))))
		{

			if(IsDRMInternalLabel(symbolInfo.mSymbolInfo.Name))
			{
				// "So in case your callstack (can happen in some cases) shows any “Label_xxxx” just scroll up in disassembly and locate the first “LSIxxxxx__....” label that comes from the game. This will indicate where it crashed."
				return GetAddressRep(addressRepTypeFlags, symbolInfo.mSymbolInfo.Address - 1, pRepArray, pIntValueArray); //note the address - 1 is "up" in the addresses
			}
			else
			{
				static const int MAX_FULL_SYMBOL_NAME = 2048;
				// create a copy so we can strip the DRM symbols
				char     demangledSymbol[MAX_FULL_SYMBOL_NAME + 1];

				 // We unmangle the symbolMangled string into the line buffer. We re-use the line buffer because we don't need it any more.
				UnmangleSymbol(symbolInfo.mSymbolInfo.Name, demangledSymbol, MAX_FULL_SYMBOL_NAME, kCompilerTypeMSVC);

				pRepArray[kARTFunctionOffset]      = demangledSymbol;
				pIntValueArray[kARTFunctionOffset] = (int)displacement;
				returnFlags                       |= kARTFlagFunctionOffset;
			}
		}
		else
		{
			#if 0 // defined(EA_DEBUG) && defined(EA_ASSERT_ENABLED) && EA_WINAPI_FAMILY_PARTITION(EA_WINAPI_PARTITION_DESKTOP)
				char  buffer[256];
				DWORD dwError = GetLastError();
				FormatWindowsError(dwError, buffer, 256);
				//EA_FAIL_MSG(buffer);
			#endif
		}
	}

	return returnFlags;
}

} // namespace Callstack

} // namespace EA

#endif




///////////////////////////////////////////////////////////////////////////////
// PDBFileCustom
///////////////////////////////////////////////////////////////////////////////

#include <EAIO/EAFileStream.h>
#include <EAStdC/EATextUtil.h>
#include <EAStdC/EAString.h>
#include <EACallstack/EASymbolUtil.h>
#include <EASTL/sort.h>
#include <stdlib.h>
#include <stdarg.h>
#include <stdio.h>
#include <time.h>
#include <EAAssert/eaassert.h>





namespace EA
{
namespace Callstack
{
namespace PDB
{
	// Microsoft has so far had three symbol type revisions, and modern pdb files use
	// version three. We don't bother enumerating the old types or their associated structs.
	enum SymbolType
	{
		kCompilandV3            = 0x1101,
		kThunkV3                = 0x1102,
		kBlockV3                = 0x1103,
		kLabelV3                = 0x1105,
		kRegisterV3             = 0x1106,
		kConstantV3             = 0x1107,
		kUdtV3                  = 0x1108,
		kBPRelV3                = 0x110b,
		kLocalDataV3            = 0x110c,
		kGlobalDataV3           = 0x110d,
		kPublicV3               = 0x110e,
		kLocalProcV3            = 0x110f,
		kGlobalProcV3           = 0x1110,
		kBPRelative_V3          = 0x1111,   // It's not clear what this is, though perhaps it's something about the x86 BP register.
		kGlobalDataUnknownV3    = 0x1113,   // Seen on XBox 360. Similar to kGlobalDataV3.
		kToolV3                 = 0x1116,
		kPublicFunc1V3          = 0x1125,
		kPublicFunc2V3          = 0x1127,
		kUnknown1128V3          = 0x1128,   // Seen on XBox 360. Always of size 16.
		kSectionInfoV3          = 0x1136,
		kSubSectionInfoV3       = 0x1137,
		kEntryPointV3           = 0x1138,
		kSecurityCookieV3       = 0x113A,
		kMSTooInfoV3            = 0x113C,
		kMSToolEnvV3            = 0x113D
	};

	enum FileType
	{
		kFileTypeFreePages    =  0,
		kFileTypeRoot         =  1,
		kFileTypeTypes        =  2,
		kFileTypeModuleRecord =  3,
		kFileTypeSectionDir   = 10,
		kFileTypeFiles        = 12
	};

	enum SectionChararacteristics
	{
		kReserved0                          = 0x00000000,
		kReserved1                          = 0x00000001,
		kReserved2                          = 0x00000002,
		kReserved4                          = 0x00000004,
		kIMAGE_SCN_TYPE_NO_PAD              = 0x00000008, // The section should not be padded to the next boundary. This flag is obsolete and is replaced by IMAGE_SCN_ALIGN_1BYTES.
		kReserved10                         = 0x00000010,
		kIMAGE_SCN_CNT_CODE                 = 0x00000020, // The section contains executable code.
		kIMAGE_SCN_CNT_INITIALIZED_DATA     = 0x00000040, // The section contains initialized data.
		kIMAGE_SCN_CNT_UNINITIALIZED_DATA   = 0x00000080, // The section contains uninitialized data.
		kIMAGE_SCN_LNK_OTHER                = 0x00000100, // Reserved.
		kIMAGE_SCN_LNK_INFO                 = 0x00000200, // The section contains comments or other information. This is valid only for object files.
		kReserved400                        = 0x00000400, // Reserved.
		kIMAGE_SCN_LNK_REMOVE               = 0x00000800, // The section will not become part of the image. This is valid only for object files.
		kIMAGE_SCN_LNK_COMDAT               = 0x00001000, // The section contains COMDAT data. This is valid only for object files.
		kReserved2000                       = 0x00002000, // Reserved.
		kIMAGE_SCN_NO_DEFER_SPEC_EXC        = 0x00004000, // Reset speculative exceptions handling bits in the TLB entries for this section.
		kIMAGE_SCN_GPREL                    = 0x00008000, // The section contains data referenced through the global pointer.
		kReserved10000                      = 0x00010000, // Reserved.
		kIMAGE_SCN_MEM_PURGEABLE            = 0x00020000, // Reserved.
		kIMAGE_SCN_MEM_LOCKED               = 0x00040000, // Reserved.
		kIMAGE_SCN_MEM_PRELOAD              = 0x00080000, // Reserved.
		kIMAGE_SCN_ALIGN_1BYTES             = 0x00100000, // Align data on a 1-byte boundary. This is valid only for object files.
		kIMAGE_SCN_ALIGN_2BYTES             = 0x00200000, // Align data on a 2-byte boundary. This is valid only for object files.
		kIMAGE_SCN_ALIGN_4BYTES             = 0x00300000, // Align data on a 4-byte boundary. This is valid only for object files.
		kIMAGE_SCN_ALIGN_8BYTES             = 0x00400000, // Align data on a 8-byte boundary. This is valid only for object files.
		kIMAGE_SCN_ALIGN_16BYTES            = 0x00500000, // Align data on a 16-byte boundary. This is valid only for object files.
		kIMAGE_SCN_ALIGN_32BYTES            = 0x00600000, // Align data on a 32-byte boundary. This is valid only for object files.
		kIMAGE_SCN_ALIGN_64BYTES            = 0x00700000, // Align data on a 64-byte boundary. This is valid only for object files.
		kIMAGE_SCN_ALIGN_128BYTES           = 0x00800000, // Align data on a 128-byte boundary. This is valid only for object files.
		kIMAGE_SCN_ALIGN_256BYTES           = 0x00900000, // Align data on a 256-byte boundary. This is valid only for object files.
		kIMAGE_SCN_ALIGN_512BYTES           = 0x00A00000, // Align data on a 512-byte boundary. This is valid only for object files.
		kIMAGE_SCN_ALIGN_1024BYTES          = 0x00B00000, // Align data on a 1024-byte boundary. This is valid only for object files.
		kIMAGE_SCN_ALIGN_2048BYTES          = 0x00C00000, // Align data on a 2048-byte boundary. This is valid only for object files.
		kIMAGE_SCN_ALIGN_4096BYTES          = 0x00D00000, // Align data on a 4096-byte boundary. This is valid only for object files.
		kIMAGE_SCN_ALIGN_8192BYTES          = 0x00E00000, // Align data on a 8192-byte boundary. This is valid only for object files.
		kIMAGE_SCN_LNK_NRELOC_OVFL          = 0x01000000, // The section contains extended relocations. The count of relocations for the section exceeds the 16 bits that is reserved for it in the section header. If the NumberOfRelocations field in the section header is 0xffff, the actual relocation count is stored in the VirtualAddress field of the first relocation. It is an error if IMAGE_SCN_LNK_NRELOC_OVFL is set and there are fewer than 0xffff relocations in the section.
		kIMAGE_SCN_MEM_DISCARDABLE          = 0x02000000, // The section can be discarded as needed.
		kIMAGE_SCN_MEM_NOT_CACHED           = 0x04000000, // The section cannot be cached.
		kIMAGE_SCN_MEM_NOT_PAGED            = 0x08000000, // The section cannot be paged.
		kIMAGE_SCN_MEM_SHARED               = 0x10000000, // The section can be shared in memory.
		kIMAGE_SCN_MEM_EXECUTE              = 0x20000000, // The section can be executed as code.
		kIMAGE_SCN_MEM_READ                 = 0x40000000, // The section can be read.
		kIMAGE_SCN_MEM_WRITE                = 0x80000000, // The section can be written to.
	};

	struct PascalString
	{
		uint8_t mLength;
		char    mName[1];
	};

	union Symbol
	{
		struct
		{
			uint16_t mSize; // Size of data after mSize. Thus the full size is mSize + 2.
			uint16_t mId;
		} mBase;

		struct
		{
			uint16_t mSize;
			uint16_t mId;
			uint32_t mSymType;
			uint32_t mOffset;
			uint16_t mSegment;
			char     mName[1];
		} mDataV3;

		struct
		{
			uint16_t mSize;
			uint16_t mId;
			uint32_t mParent;
			uint32_t mEnd;
			uint32_t mNext;
			uint32_t mOffset;
			uint16_t mSegment;
			uint16_t thunk_len;
			uint8_t  thtype;
			char     mName[1];
		} mThunkV3;

		struct
		{
			uint16_t mSize;
			uint16_t mId;
			uint32_t mParent;
			uint32_t mEnd;
			uint32_t mNext;
			uint32_t mProcLength;
			uint32_t mDebugStart;
			uint32_t mDebugEnd;
			uint32_t mProcType;
			uint32_t mOffset;
			uint16_t mSegment;
			uint8_t  mFlags;
			char     mName[1];
		} mProcV3;

		struct
		{
			uint16_t mSize;
			uint16_t mId;
			uint32_t mSymType;
			uint32_t mOffset;
			uint16_t mSegment;
			char     mName[1];
		} mPublicV3;

		struct
		{
			uint16_t mSize;           // Total length of this entry
			uint16_t mId;             // Always BPRelV3
			int32_t  mOffset;         // Stack mOffset relative to BP
			uint32_t mSymType;
			char     mName[1];
		} mStackV3;

		struct
		{
			uint16_t mSize;           // Total length of this entry
			uint16_t mId;             // Always RegisterV3
			uint32_t mType;           // check whether type & reg are correct
			uint16_t mRegister;
			char     mName[1];
		} mRegisterV3;

		struct
		{
			uint16_t mSize;
			uint16_t mId;
			uint32_t mParent;
			uint32_t mEnd;
			uint32_t mLength;
			uint32_t mOffset;
			uint16_t mSegment;
			char     mName[1];
		} mBlockV3;

		struct
		{
			uint16_t mSize;
			uint16_t mId;
			uint32_t mOffset;
			uint16_t mSegment;
			uint8_t  mFlags;
			char     mName[1];
		} mLabelV3;

		struct
		{
			uint16_t mSize;
			uint16_t mId;
			uint32_t mType;
			uint16_t mValue;
		  //char     mName[1];
		} mConstantV3;

		struct
		{
			uint16_t mSize;
			uint16_t mId;
			uint32_t mType;
			char     mName[1];
		} mUdtV3;

		struct
		{
			uint16_t mSize;
			uint16_t mId;
			uint32_t mUnknown;
			char     mName[1];
		} mCompilandV3;
	};

} // namespace PDB



static inline uint16_t LE(uint16_t& n16)
{
	#if defined(EA_SYSTEM_BIG_ENDIAN)
		n16 = (uint16_t) ((n16 >> 8) | (n16 << 8));
	#endif

	return n16;
}

static inline uint32_t LE(uint32_t& n32)
{
	#if defined(EA_SYSTEM_BIG_ENDIAN)
		n32 = ((n32 >> 24)               |
			  ((n32 << 24) & 0xff000000) |
			  ((n32 <<  8) & 0x00ff0000) |
			  ((n32 >>  8) & 0x0000ff00));
	#endif

	return n32;
}

static inline void LE(uint32_t* p32, uint32_t n)
{
	#if defined(EA_SYSTEM_BIG_ENDIAN)
		for(uint32_t* p = p32; n; n--, p++)
			*p = LE(*p);
	#else
		(void)p32; (void)n;
	#endif
}



struct SortByOffset
{
	bool operator()(const PDBFileCustom::SymbolInfo& a, const PDBFileCustom::SymbolInfo& b) const
	{
		return a.mAddressOffset < b.mAddressOffset;
	}
};


PDBFileCustom::LookupRequest::LookupRequest()
  : mAddressRepTypeFlags(0),
	mpAddressArray(NULL),
	mAddressArrayCount(0),
	mpFunctionArray(NULL),
	mpOffsetArray(NULL),
	mResultCount(0)
{
}


///////////////////////////////////////////////////////////////////////////////
// PDBFileCustom
//
PDBFileCustom::PDBFileCustom(EA::Allocator::ICoreAllocator* pCoreAllocator)
  : mpCoreAllocator(pCoreAllocator ? pCoreAllocator : EA::Callstack::GetAllocator()),
	msPDBFilePath(),
	mFile(),
	mFileSize(0),
	mbOfflineLookup(false),
	mBaseAddress(kBaseAddressUnspecified),
	mHeader(),
	mpTOC(NULL),
	mRoot(),
	mGlobalSymbolFileIndex(0),
	mGlobalSymbolFileSize(0),
	mGlobalSymbolFileBlockIndexArray(EA::Callstack::EASTLCoreAllocator(EACALLSTACK_ALLOC_PREFIX "EACallstack/PDBFileCustom", mpCoreAllocator)),
	mbBuildSymbolInfoArray(false),
	mSymbolInfoArray(EA::Callstack::EASTLCoreAllocator(EACALLSTACK_ALLOC_PREFIX "EACallstack/PDBFileCustom", mpCoreAllocator))
{
	memset(&mHeader,        0, sizeof(mHeader));
	memset(&mRoot,          0, sizeof(mRoot));
	memset(&mModuleRecord,  0, sizeof(mModuleRecord));
	memset(&mTextSection,   0, sizeof(mTextSection));
}


///////////////////////////////////////////////////////////////////////////////
// ~PDBFileCustom
//
PDBFileCustom::~PDBFileCustom()
{
	Shutdown();
}


///////////////////////////////////////////////////////////////////////////////
// SetAllocator
//
void PDBFileCustom::SetAllocator(EA::Allocator::ICoreAllocator* pCoreAllocator)
{
	mpCoreAllocator = pCoreAllocator;
}


///////////////////////////////////////////////////////////////////////////////
// Init
//
bool PDBFileCustom::Init(const wchar_t* pPDBFilePath, bool bDelayLoad)
{
	bool bResult = true;

	if(pPDBFilePath)
	{
		msPDBFilePath = pPDBFilePath;
		mFile.SetPath(pPDBFilePath);
	}

	if(mBaseAddress == kBaseAddressUnspecified) // If the base address hasn't been set...
	{
		char pPath8[EA::IO::kMaxPathLength];
		EA::StdC::Strlcpy(pPath8, msPDBFilePath.c_str(), EA::IO::kMaxPathLength);

		// We use the base address of the currently executing module.
		// This can't work if we are reading a symbol file from an
		// application other than the currently executing one.

		if(!mbOfflineLookup)
		{
			mBaseAddress = Callstack::GetModuleBaseAddress(pPath8);
			if(mBaseAddress == kBaseAddressInvalid)
				mBaseAddress = kBaseAddressUnspecified;
		}

		if(mBaseAddress == kBaseAddressUnspecified)
		{
			// To do: read the pdb file and if it's XBox 360 then set the base address to 0x82000000.
			// On Win32 the default is 0x00400000. On Win64 the default is 0x0000000140000000i64.

			// We make the assumption that we are working with an XBox 360 PDB file and
			// that the default base address is in use. XBox 360 is a good guess because
			// right now the only platform this PDBFileCustom class is used on is runtime
			// 360 lookups on the 360 of its own addresses. The guess of 0x82000000 works
			// only for the main executable (usually), but can't work for DLLs. But on
			// the other hand, the GetModuleBaseAddress call above really should have found
			// the base address for a DLL associated with this PDB file.

			mBaseAddress = 0x82000000; // XBox 360 default.
		}
	}

	if(!bDelayLoad)
		bResult = LoadSymbolInfo(); // This doesn't load the symbols, it loads the info about the symbols. The actual symbol info could be huge.

	return bResult;
}


///////////////////////////////////////////////////////////////////////////////
// Shutdown
//
bool PDBFileCustom::Shutdown()
{
	if(mpTOC)
	{
		mpCoreAllocator->Free(mpTOC);
		mpTOC = NULL;
	}

	mSymbolInfoArray.clear();

	mGlobalSymbolFileBlockIndexArray.clear();
	mSymbolInfoArray.clear();
	mBaseAddress = kBaseAddressUnspecified;
	mFile.Close();

	return true;
}


///////////////////////////////////////////////////////////////////////////////
// AsInterface
//
void* PDBFileCustom::AsInterface(int id)
{
	if(id == PDBFileCustom::kTypeId)
		return static_cast<PDBFileCustom*>(this); // The cast isn't necessary, but makes the intent more clear.
	return NULL;
}


///////////////////////////////////////////////////////////////////////////////
// SetBaseAddress
//
void PDBFileCustom::SetBaseAddress(uint64_t baseAddress)
{
	mBaseAddress = baseAddress;
}


///////////////////////////////////////////////////////////////////////////////
// GetBaseAddress
//
uint64_t PDBFileCustom::GetBaseAddress() const
{
	return mBaseAddress;
}


///////////////////////////////////////////////////////////////////////////////
// GetDatabasePath
//
const wchar_t* PDBFileCustom::GetDatabasePath() const
{
	return msPDBFilePath.c_str();
}


///////////////////////////////////////////////////////////////////////////////
// SetOption
//
void PDBFileCustom::SetOption(int option, int value)
{
	if(option == kOptionSymbolCacheLevel)
		mbBuildSymbolInfoArray = (value != 0);
	//else if(option == kOptionCopyFileToMemory)
	//{
	//    To consider: Implement this.
	//    There hasn't been a hurry to implement this, because usually this class
	//    is used only on a console, and in that case you don't typically want
	//    to copy the file to memory.
	//}
	else if(option == kOptionOfflineLookup)
		mbOfflineLookup = (value != 0);
}


///////////////////////////////////////////////////////////////////////////////
// GetAddressRep
//
int PDBFileCustom::GetAddressRep(int addressRepTypeFlags, uint64_t nAddress, FixedString* pRepArray, int* pIntValueArray)
{
	int count = 0;

	if(addressRepTypeFlags)
	{
		if(!mGlobalSymbolFileSize)
			LoadSymbolInfo(); // This doesn't load the symbols, it loads the info about the symbols. The actual symbol info could be huge.

		if(mGlobalSymbolFileSize) // If we could load the symbol info...
		{
			if((nAddress <  (mBaseAddress + mTextSection.mVirtualAddress)) ||
			   (nAddress >= (mBaseAddress + mTextSection.mVirtualAddress + mTextSection.mVirtualSize)))
			{
				return 0;
			}

			// We don't yet have the ability to spelunk the .pdb file for kARTFlagFileLine information.
			if(addressRepTypeFlags & kARTFlagFunctionOffset)
			{
				if(mbBuildSymbolInfoArray) // If we have pre-stored symbol information...
				{
					FixedString8 s(EA::Callstack::EASTLCoreAllocator(EACALLSTACK_ALLOC_PREFIX "EACallstack/PDBFileCustom/FixedString", mpCoreAllocator));

					pRepArray[kARTFunctionOffset].clear();
					pIntValueArray[kARTFunctionOffset] = 0; // Sorry, caller, this info is unavailable right now.

					// The relative address is address within the .text section. Even though the absolute address can be a 64 bit value (Win64),
					// all segments are <= 32 bits in size and so relative addresses will always fit in a uint32_t.
					const uint32_t nAddressRelative = (uint32_t)(nAddress - (mBaseAddress + mTextSection.mVirtualAddress));
					const SymbolInfo symbolInfo(nAddressRelative, NULL);

					SymbolInfoArray::iterator it = eastl::lower_bound(mSymbolInfoArray.begin(), mSymbolInfoArray.end(), symbolInfo, SortByOffset());

					if(it != mSymbolInfoArray.end())
					{
						if((it != mSymbolInfoArray.begin()) && (it->mAddressOffset > symbolInfo.mAddressOffset))
							--it;

						const SymbolInfo& si = *it;

						s.resize(s.capacity() - 1);
						s.resize((eastl_size_t)UnmangleSymbol(si.mSymbolText.c_str(), (char*)s.data(), s.size(), kCompilerTypeMSVC));
						SanitizeSymbol(s, kCompilerTypeMSVC);

						pRepArray[kARTFunctionOffset] = s.c_str();
						SanitizeSymbol(pRepArray[kARTFunctionOffset], kCompilerTypeMSVC);
						count++;
					}
				}
				else // Else we need to iterate the symbols as we go.
				{
					// We leave some information for us, then call IterateSymbols, which will pick up this information.
					mLookupRequest.mAddressRepTypeFlags = addressRepTypeFlags;
					mLookupRequest.mpAddressArray       = &nAddress;
					mLookupRequest.mAddressArrayCount   = 1;
					mLookupRequest.mpFunctionArray      = pRepArray + kARTFunctionOffset;
					mLookupRequest.mpOffsetArray        = pIntValueArray + kARTFunctionOffset;
					mLookupRequest.mResultCount         = 0;

					IterateSymbols();

					count = mLookupRequest.mResultCount;

					mLookupRequest.mAddressRepTypeFlags = 0;
				}
			}
		}
	}

	return count;
}


bool PDBFileCustom::ReadPDBFile(void* pDestination, uint32_t destinationCapacity, uint32_t fileSize, uint32_t blockIndexArrayOffset)
{
	bool bReturnValue = false;

	const uint32_t blockCount = (fileSize + mHeader.mBlockSize - 1) / mHeader.mBlockSize; // Round up to the block size.
	uint8_t*       pDest8     = (uint8_t*)pDestination;
	uint8_t* const pDest8End  = pDest8 + eastl::min_alt(destinationCapacity, fileSize);

	if(mFile.SetPosition((int32_t)blockIndexArrayOffset))
	{
		BlockIndexArray blockIndexArray(blockCount, EA::Callstack::EASTLCoreAllocator(EACALLSTACK_ALLOC_PREFIX "EACallstack/PDBFileCustom", mpCoreAllocator));

		if(mFile.Read((uint32_t*)blockIndexArray.data(), blockCount * sizeof(uint32_t)) == (blockCount * sizeof(uint32_t)))
		{
			LE((uint32_t*)blockIndexArray.data(), (uint32_t)blockIndexArray.size());

			bReturnValue = true;

			for(uint32_t i = 0; bReturnValue && (pDest8 < pDest8End) && (i < blockCount); i++, pDest8 += mHeader.mBlockSize)
			{
				const uint32_t nReadSize   = eastl::min_alt(mHeader.mBlockSize, (uint32_t)(pDest8End - pDest8));
				const uint32_t nBlockIndex = blockIndexArray[i];
				const uint32_t nPosition   = nBlockIndex * mHeader.mBlockSize;

				if(mFile.SetPosition((int32_t)nPosition))
				{
					if(mFile.Read(pDest8, nReadSize) != nReadSize)
						bReturnValue = false;
				}
			}
		}
	}

	return bReturnValue;
}


bool PDBFileCustom::LoadSymbolInfo()
{
	bool bReturnValue = false;

	if(!mFile.GetAccessFlags())
		mFile.Open(EA::IO::kAccessFlagRead);

	if(mFile.GetAccessFlags() & EA::IO::kAccessFlagRead)
	{
		mFile.SetPosition(0);

		mFileSize = (uint32_t)mFile.GetSize();

		if(mFile.Read(&mHeader, sizeof(mHeader)) == sizeof(mHeader))
		{
			LE(mHeader.mBlockSize);
			LE(mHeader.mPageCount);
			LE(mHeader.mTOCSize);
			LE(mHeader.mTOCPage);

			const char* const pSignature = "Microsoft C/C++ MSF 7.00";

			if(memcmp(mHeader.mSignature, pSignature, strlen(pSignature)) == 0)
			{
				if(mpTOC)
					mpCoreAllocator->Free(mpTOC);

				const uint32_t nTOCReadSize = mHeader.mTOCSize; // Don't try this until we know it can work: nTOCReadSize = eastl::min_alt(mHeader.mTOCSize, (uint32_t)65536); // The user would have to have a truly huge .pdb file (~1GB) for us to ever need to use more than this.

				mpTOC = (PDB::TOC*)mpCoreAllocator->Alloc(nTOCReadSize, EACALLSTACK_ALLOC_PREFIX "EACallstack/PDBFileCustom", 0);

				if(mpTOC)
				{
					bReturnValue = ReadPDBFile(mpTOC, nTOCReadSize, nTOCReadSize, mHeader.mTOCPage * mHeader.mBlockSize); // destination, destination size, file size, block index offset.

					if(bReturnValue)
					{
						uint32_t  nBlockCount;
						uint32_t* pBlockIndexArray;

						// All of the contents of the TOC are uint32_t, so it's easiest to just convert them all at once here like this.
						LE((uint32_t*)mpTOC, nTOCReadSize / sizeof(uint32_t));

						{
							// Read Root
							pBlockIndexArray = (mpTOC->mFileSizeArray + mpTOC->mFileCount);

							// Move pBlockIndexArray past each file till we get to PDB::kFileTypeModuleRecord. Round up file size to block size.
							for(uint32_t i = 0, iEnd = PDB::kFileTypeRoot; i < iEnd; i++)
							{
								nBlockCount       = (mpTOC->mFileSizeArray[i] + mHeader.mBlockSize - 1) / mHeader.mBlockSize; // Round up mFileSizeArray[i] to block size.
								pBlockIndexArray += nBlockCount;
							}

							bReturnValue = mFile.SetPosition((int32_t)(*pBlockIndexArray * mHeader.mBlockSize)) &&
										   mFile.Read(&mRoot, sizeof(mRoot)) == sizeof(mRoot);

							// Note: We have disabled this because a user reported an mNameCount of 1.5 billion. Maybe we have a bug somewhere,
							//       but until then there isn't much to do outside of disabling this assert.
							// EA_ASSERT(mRoot.mNameCount < sizeof(mRoot.mNames)); // If this fails then we need to make mRoot.mNames larger or just disable this assert.
						}

						{
							// Read the ModuleRecord

							// Set pBlockIndexArray to be after the file size array.
							pBlockIndexArray = (mpTOC->mFileSizeArray + mpTOC->mFileCount);

							// Move pBlockIndexArray past each file till we get to PDB::kFileTypeModuleRecord. Round up file size to block size.
							for(uint32_t i = 0, iEnd = PDB::kFileTypeModuleRecord; i < iEnd; i++)
							{
								nBlockCount       = (mpTOC->mFileSizeArray[i] + mHeader.mBlockSize - 1) / mHeader.mBlockSize; // Round up mFileSizeArray[i] to block size.
								pBlockIndexArray += nBlockCount;
							}

							// pBlockIndexArray now points to the block indexes for kFileTypeModuleRecord. The file is of size mFileSizeArray[kFileTypeModuleRecord].
							bReturnValue = mFile.SetPosition((int32_t)(*pBlockIndexArray * mHeader.mBlockSize)) &&
										   mFile.Read(&mModuleRecord, sizeof(mModuleRecord)) == sizeof(mModuleRecord);
						}

						// Note by Paul Pedriana: mModuleRecord doesn't seem to be read right with VC8/Windows pdb files.
						if(bReturnValue)
						{
							LE(mModuleRecord.mSignature);
							LE(mModuleRecord.mVersion);
							LE(mModuleRecord.mHash1File);
							LE(mModuleRecord.mHash2File);
							LE(mModuleRecord.mGlobalSymbolFile);
							LE(mModuleRecord.mModuleSize);
							LE(mModuleRecord.mOffsetSize);
							LE(mModuleRecord.mHashSize);
							LE(mModuleRecord.mSourceModuleSize);
							LE(mModuleRecord.mPDBImportSize);

							pBlockIndexArray = (mpTOC->mFileSizeArray + mpTOC->mFileCount);

							// Move pBlockIndexArray past each file till we get to mModuleRecord.mGlobalSymbolFile. Round up file size to block size.
							for(uint32_t i = 0; i < mModuleRecord.mGlobalSymbolFile; i++)
							{
								nBlockCount       = (mpTOC->mFileSizeArray[i] + mHeader.mBlockSize - 1) / mHeader.mBlockSize; // Round up mFileSizeArray[i] to block size.
								pBlockIndexArray += nBlockCount;
							}

							// pBlockIndexArray now points to the block indexes for mModuleRecord.mGlobalSymbolFile. The file is of size mFileSizeArray[mModuleRecord.mGlobalSymbolFile].
							mGlobalSymbolFileSize = mpTOC->mFileSizeArray[mModuleRecord.mGlobalSymbolFile];
							nBlockCount = (mGlobalSymbolFileSize + mHeader.mBlockSize - 1) / mHeader.mBlockSize; // Round mFileSizeArray[kFileTypeModuleRecord] up to the block size.
							mGlobalSymbolFileBlockIndexArray.assign(pBlockIndexArray, pBlockIndexArray + nBlockCount + 1);

							if(bReturnValue && mbBuildSymbolInfoArray)
								IterateSymbols();

							if(bReturnValue)
							{
								bReturnValue = false; // False until we find a .text section below.

								// Look for the .text SectionDirectoryEntry.
								// It usually begins at section #7 and its information is repeated up to as much as section #14.
								// We loop until we find the first instance of it. Since we don't know the actual format of the
								// the section entry system in detail, we search the whole TOC space since empircal evidence based searches
								// have been too unreliable and the cost of searching the whole space is relatively minor
								pBlockIndexArray = (mpTOC->mFileSizeArray + mpTOC->mFileCount);

								for(uint32_t i = 0, iLast = mpTOC->mFileCount; i < iLast; i++)
								{
									nBlockCount       = (mpTOC->mFileSizeArray[i] + mHeader.mBlockSize - 1) / mHeader.mBlockSize; // Round up mFileSizeArray[i] to block size.
									pBlockIndexArray += nBlockCount;

									const uint32_t nSectionDirectoryBlockIndex = pBlockIndexArray[0];
									bReturnValue = mFile.SetPosition((int32_t)(nSectionDirectoryBlockIndex * mHeader.mBlockSize)) &&
												   mFile.Read(&mTextSection, sizeof(mTextSection)) == sizeof(mTextSection);

									while(bReturnValue && memcmp(mTextSection.mName, ".text", 6) && (mTextSection.mName[0] == '.'))
										bReturnValue = mFile.Read(&mTextSection, sizeof(mTextSection)) == sizeof(mTextSection);

									bReturnValue = false;

									if(memcmp(mTextSection.mName, ".text", 6) == 0)
									{
										LE(mTextSection.mVirtualSize);          // This appears to be the size of all the .text segment variants (e.g. .text, .text$yc, .text$yd) that you may see in the .map file.
										LE(mTextSection.mVirtualAddress);
										LE(mTextSection.mRawDataSize);
										LE(mTextSection.mRawDataPointer);
										LE(mTextSection.mRelocationsPointer);
										LE(mTextSection.mLineNumbersPointer);
										LE(mTextSection.mRelocationsCount);
										LE(mTextSection.mLineNumbersCount);
										LE(mTextSection.mCharacteristics); // This will typically be at least 0x60000020 (kIMAGE_SCN_CNT_CODE, kIMAGE_SCN_MEM_EXECUTE, kIMAGE_SCN_MEM_READ)

										bReturnValue = true;
										break;
									}
								}

								EA_ASSERT_MSG(bReturnValue, "The .text section should always exist so this assert means the PDB is either malformed or the PDB Reader code above is incorrect.");
							}
						}
					}
				}
			}
		}
	}

	return bReturnValue;
}


bool PDBFileCustom::IterateSymbols()
{
	bool bReturnValue = true;

	// We iterate the symbol data, which may be megabytes in size. In order to avoid allocating
	// so much memory, we implement a sliding window view (symbolBuffer) over the symbol data.
	// Much of the logic below implements the maintenance of this window.
	typedef eastl::basic_string<char, EA::Callstack::EASTLCoreAllocator>                    SymbolBuffer;
	typedef eastl::fixed_vector<uint32_t, 32, true, EA::Callstack::EASTLCoreAllocator>      AddressOffsetArray;
	typedef eastl::fixed_vector<FixedString8, 32, true, EA::Callstack::EASTLCoreAllocator>  SymbolArray;

	SymbolBuffer        symbolBuffer(EA::Callstack::EASTLCoreAllocator(EACALLSTACK_ALLOC_PREFIX "EACallstack/PDBFileCustom/SymbolBuffer", mpCoreAllocator));
	AddressOffsetArray  addressOffsetArray(EA::Callstack::EASTLCoreAllocator(EACALLSTACK_ALLOC_PREFIX "EACallstack/PDBFileCustom/AddressOffsetArray", mpCoreAllocator));   // Holds temporary strings for the current best-match symbol.
	SymbolArray         symbolArray(EA::Callstack::EASTLCoreAllocator(EACALLSTACK_ALLOC_PREFIX "EACallstack/PDBFileCustom/SymbolArray", mpCoreAllocator));          //
	uint32_t            nFilePosition1   = 0; // This is where in the symbol file we are reading from.
	uint32_t            nFilePosition2   = 0; // This is the end of the buffered area in the symbol file.
	uint32_t            nBufferPosition1 = 0; // nBufferPosition2 is not defined because it would be the same as symbolBuffer.size().

	symbolBuffer.reserve(mHeader.mBlockSize * 2); // We need to reserve at least mBlockSize * 2.

	if(mLookupRequest.mAddressRepTypeFlags) // If we will be doing dynamic lookups while iterating symbols...
	{
		// Set our scratch buffer to hold enough entries.
		addressOffsetArray.resize(mLookupRequest.mAddressArrayCount);
		symbolArray.resize(mLookupRequest.mAddressArrayCount);
	}

	while(bReturnValue && (nFilePosition1 < mGlobalSymbolFileSize)) // While we haven't read the entire file...
	{
		while(nBufferPosition1 >= mHeader.mBlockSize)
		{
			symbolBuffer.erase(0, mHeader.mBlockSize);
			nBufferPosition1 -= mHeader.mBlockSize;
		}

		while((symbolBuffer.capacity() - symbolBuffer.size()) >= mHeader.mBlockSize)
		{
			symbolBuffer.resize(symbolBuffer.size() + mHeader.mBlockSize);

			bReturnValue = mFile.SetPosition((int32_t)(mGlobalSymbolFileBlockIndexArray[nFilePosition2 / mHeader.mBlockSize] * mHeader.mBlockSize)) &&
						   mFile.Read(&symbolBuffer[symbolBuffer.size() - mHeader.mBlockSize], mHeader.mBlockSize) == mHeader.mBlockSize;

			nFilePosition2 += mHeader.mBlockSize;
		}

		if(bReturnValue)    // && (nFilePosition1 < mGlobalSymbolFileSize) && ((nBufferPosition1 + sizeof(symbol)) < symbolBuffer.size()))
		{
			PDB::Symbol symbolCurrent;

			memcpy(&symbolCurrent, &symbolBuffer[nBufferPosition1], sizeof(PDB::Symbol));
			LE(symbolCurrent.mBase.mSize);
			LE(symbolCurrent.mBase.mId);
			symbolCurrent.mBase.mSize += 2; // +2 because mSize refers to data following the mSize member.

			switch (symbolCurrent.mBase.mId)
			{
				case PDB::kLocalProcV3:
				case PDB::kGlobalProcV3:
					break;

				case PDB::kGlobalDataUnknownV3:
				case PDB::kUnknown1128V3:
					break;

				case PDB::kPublicV3:
				case PDB::kPublicFunc1V3:
				case PDB::kPublicFunc2V3:
				{
					LE(symbolCurrent.mPublicV3.mSymType);
					LE(symbolCurrent.mPublicV3.mOffset);
					LE(symbolCurrent.mPublicV3.mSegment);

					if(symbolCurrent.mPublicV3.mSymType == 2) // We usually have either 0 (data) or 2 (function)
					{
						// We create pSymbolCurrent because we need to reference the arbitrary length string at the end.
						const PDB::Symbol* const pSymbolCurrent = (PDB::Symbol*)&symbolBuffer[nBufferPosition1];

						if(mbBuildSymbolInfoArray) // If we are to build the symbol information into a data structure...
						{
							// To do: Add in symbol.mPublicV3.mSegment's offset to SymbolInfo.mAddressOffset.
							// It turns out that it is usually zero, but some day will come when it is not.
							SymbolInfo symbolInfoCurrent;
							symbolInfoCurrent.mAddressOffset = symbolCurrent.mPublicV3.mOffset;
							symbolInfoCurrent.mSymbolText.get_allocator().set_allocator(mpCoreAllocator);
							symbolInfoCurrent.mSymbolText = pSymbolCurrent->mPublicV3.mName;

							mSymbolInfoArray.push_back(std::move(symbolInfoCurrent));
						}
						else if(mLookupRequest.mAddressRepTypeFlags & kARTFlagFunctionOffset) // Otherwise if the user wants us to do lookups...
						{
							for(eastl_size_t i = 0; i < (eastl_size_t)mLookupRequest.mAddressArrayCount; i++)
							{
								// Note that this 32 bit address is 64 bit safe (at least in the year 2007).
								// This is because even in a 64 bit VC++ app, the app code itself is limited to 32 bits (4GB).
								const int64_t offsetRelativeToSection = (int64_t)mLookupRequest.mpAddressArray[i] - (int64_t)(mBaseAddress + mTextSection.mVirtualAddress);
								const int64_t diffCurrent             = (int64_t)(offsetRelativeToSection - (int32_t)symbolCurrent.mPublicV3.mOffset);
								const int64_t diffBest                = (int64_t)(offsetRelativeToSection - (int32_t)addressOffsetArray[i]);

								if((diffCurrent >= 0) && (diffCurrent < diffBest))
								{
									addressOffsetArray[i] = symbolCurrent.mPublicV3.mOffset;
									symbolArray[i]        = pSymbolCurrent->mPublicV3.mName;
								}
							}
						}
					}

					break;
				}
			}

			nBufferPosition1 += symbolCurrent.mBase.mSize;
			nFilePosition1   += symbolCurrent.mBase.mSize;
		}
	}

	if(mbBuildSymbolInfoArray)
	{
		// Soert the symbols from lowest address to highest.
		eastl::sort(mSymbolInfoArray.begin(), mSymbolInfoArray.end(), SortByOffset());

		// We add an empty terminating symbol after the last symbol.
		if(!mSymbolInfoArray.empty())
		{
			SymbolInfo& symbolInfoLast = mSymbolInfoArray.back(); // We use 8192 here, but that's just a guess. We could instead track the exact value above, but it's hardly worth the effort.
			SymbolInfo  symbolInfo(symbolInfoLast.mAddressOffset + 8192, symbolInfoLast.mSymbolText);
			mSymbolInfoArray.push_back(std::move(symbolInfo));
		}

		// We simply won't be needing this any more.
		mGlobalSymbolFileBlockIndexArray.set_capacity(0);
	}
	else
	{
		if(mLookupRequest.mAddressRepTypeFlags & kARTFlagFunctionOffset)
		{
			FixedString8 s(EA::Callstack::EASTLCoreAllocator(EACALLSTACK_ALLOC_PREFIX "EACallstack/PDBFileCustom/FixedString", mpCoreAllocator));

			for(eastl_size_t i = 0, iEnd = symbolArray.size(); i < iEnd; i++)
			{
				const FixedString8& sCurrent = symbolArray[i];

				if(sCurrent.empty())
				{
					mLookupRequest.mpFunctionArray[i].clear();
					mLookupRequest.mpOffsetArray[i] = 0;
				}
				else
				{
					mLookupRequest.mResultCount = 1;

					s.resize(s.capacity() - 1);
					s.resize((eastl_size_t)UnmangleSymbol(sCurrent.c_str(), (char*)s.data(), s.size(), kCompilerTypeMSVC));
					SanitizeSymbol(s, kCompilerTypeMSVC);

					const int32_t diffBest = (int32_t)(mLookupRequest.mpAddressArray[i] - (mBaseAddress + mTextSection.mVirtualAddress + addressOffsetArray[i]));
					mLookupRequest.mpFunctionArray[i].assign(s.data(), s.length());
					mLookupRequest.mpOffsetArray[i] = diffBest;
				}
			}
		}
	}

	return bReturnValue;
}


} // namespace Callstack
} // namespace EA


#endif // EACALLSTACK_PDBFILE_ENABLED
