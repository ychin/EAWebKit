///////////////////////////////////////////////////////////////////////////////
// Copyright (c) Electronic Arts Inc. All rights reserved.
///////////////////////////////////////////////////////////////////////////////

#include <coreallocator/icoreallocatormacros.h>
#include <EACallstack/internal/Config.h>
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

#if EACALLSTACK_GLIBC_BACKTRACE_AVAILABLE

	#include <dlfcn.h>
	#include <execinfo.h>

#endif

namespace EA
{
namespace Callstack
{

static constexpr size_t kAddressRepLookupMaxModules = 1024;

///////////////////////////////////////////////////////////////////////////////
// AddressRepLookupSet
///////////////////////////////////////////////////////////////////////////////

///////////////////////////////////////////////////////////////////////////////
// AddressRepLookupSet
//
AddressRepLookupSet::AddressRepLookupSet(EA::Allocator::ICoreAllocator* pCoreAllocator)
  : mpCoreAllocator(pCoreAllocator ? pCoreAllocator : EA::Callstack::GetAllocator()),
	mbLookupsAreLocal(true),
	mbEnableAutoDatabaseFind(true),
	mbLookedForDBFiles(false),
	mAddressRepLookupList(EA::Callstack::EASTLCoreAllocator(EACALLSTACK_ALLOC_PREFIX "EACallstack/AddressRepLookupList", mpCoreAllocator)),
	mSourceCodeDirectoryList(EA::Callstack::EASTLCoreAllocator(EACALLSTACK_ALLOC_PREFIX "EACallstack/SourceCodeDirectoryList", mpCoreAllocator)),
	mnSourceCodeFailureCount(0),
	mnSourceContextLineCount(kSourceContextLineCount)
{
}


///////////////////////////////////////////////////////////////////////////////
// ~AddressRepLookupSet
//
AddressRepLookupSet::~AddressRepLookupSet()
{
	AddressRepLookupSet::Shutdown();
}


///////////////////////////////////////////////////////////////////////////////
// SetAllocator
//
void AddressRepLookupSet::SetAllocator(EA::Allocator::ICoreAllocator* pCoreAllocator)
{
	mpCoreAllocator = pCoreAllocator;

	mAddressRepLookupList   .get_allocator().set_allocator(pCoreAllocator);
	mSourceCodeDirectoryList.get_allocator().set_allocator(pCoreAllocator);
}


///////////////////////////////////////////////////////////////////////////////
// Init
//
bool AddressRepLookupSet::Init()
{
	return true;
}


///////////////////////////////////////////////////////////////////////////////
// Shutdown
//
bool AddressRepLookupSet::Shutdown()
{
	for(AddressRepLookupList::iterator it = mAddressRepLookupList.begin(); it != mAddressRepLookupList.end(); ++it)
	{
		IAddressRepLookup* const pARL = *it;

		pARL->Shutdown();
		pARL->~IAddressRepLookup();
		mpCoreAllocator->Free(pARL);
	}

	mAddressRepLookupList.clear();
	mnSourceCodeFailureCount = 0;

	return true;
}


///////////////////////////////////////////////////////////////////////////////
// EnableAutoDatabaseFind
//
void AddressRepLookupSet::EnableAutoDatabaseFind(bool bEnable)
{
	mbEnableAutoDatabaseFind = bEnable;
}


///////////////////////////////////////////////////////////////////////////////
// SetSourceCodeLineCount
//
void AddressRepLookupSet::SetSourceCodeLineCount(unsigned int lineCount)
{
	mnSourceContextLineCount = lineCount;
}


///////////////////////////////////////////////////////////////////////////////
// AddDatabaseFileEx
//
bool AddressRepLookupSet::AddDatabaseFileEx(const wchar_t* pDatabaseFilePath, uint64_t baseAddress,
											bool bDelayLoad, EA::Allocator::ICoreAllocator* pCoreAllocator,
											bool& bDBPreviouslyPresent, bool& bBaseAddressAlreadySet)
{
	// Check to see if the DB is already registered.
	IAddressRepLookup* pARL = AddressRepLookupSet::GetAddressRepLookup(pDatabaseFilePath);

	if(pARL)
	{
		uint64_t existingDBBaseAddress = pARL->GetBaseAddress();

		// this conditional code is suspect, but was not changed to reduce knock-on behavior
		// Specifically why are we checking for kBaseAddressUnspecified when this is the default ? Shouldnt this already be set?
		if((existingDBBaseAddress == baseAddress) && (existingDBBaseAddress != kBaseAddressUnspecified))
			bBaseAddressAlreadySet = true;
		else
		{
			pARL->SetBaseAddress(baseAddress); // Overwrite the base address.
			bBaseAddressAlreadySet = false;
		}

		bDBPreviouslyPresent = true;

		return true;
	}

	bDBPreviouslyPresent  = false;
	bBaseAddressAlreadySet = false;

	if(!mpCoreAllocator)
	{
		mpCoreAllocator = EA::Callstack::GetAllocator();
		EA_ASSERT(mpCoreAllocator != NULL); // If this fails, did you forget to call EA::Callstack::SetAllocator or did you call it after creating this AddressRepLookupSet instance?
	}

	if(!pCoreAllocator)
		pCoreAllocator = mpCoreAllocator;

	// Note that we create the ARL object with our allocator and not the pCoreAllocatorArgument.
	// The argument pCoreAllocator is used by the created ARL object. Of course, in the large majority
	// of cases these will be the same ICoreAllocator.

	pARL = MakeAddressRepLookupFromDatabase(pDatabaseFilePath, mpCoreAllocator);

	if(pARL)
	{
		pARL->SetBaseAddress(baseAddress); // baseAddress can conceivably be 0 or kBaseAddressUnspecified.
		pARL->SetAllocator(pCoreAllocator);

		#if !defined(EA_PLATFORM_DESKTOP)
			// To do: Make this an option that can be set by the user.
			pARL->SetOption(IAddressRepLookup::kOptionLowMemoryUsage, 1);
		#endif

		if(pARL->Init(pDatabaseFilePath, bDelayLoad))
		{
			AddAddressRepLookup(pARL);
			return true;
		}


		// The above Init failed, so we free the ARL.
		pARL->~IAddressRepLookup();
		mpCoreAllocator->Free(pARL);
	}

	return false;
}


///////////////////////////////////////////////////////////////////////////////
// AddDatabaseFile
//
bool AddressRepLookupSet::AddDatabaseFile(const wchar_t* pDatabaseFilePath, uint64_t baseAddress,
											bool bDelayLoad, EA::Allocator::ICoreAllocator* pCoreAllocator)
{
	bool bDBPreviouslyPresent;
	bool bBaseAddressAlreadySet;

	return AddDatabaseFileEx(pDatabaseFilePath, baseAddress, bDelayLoad, pCoreAllocator, bDBPreviouslyPresent, bBaseAddressAlreadySet);
}


///////////////////////////////////////////////////////////////////////////////
// RemoveDatabaseFile
//
bool AddressRepLookupSet::RemoveDatabaseFile(const wchar_t* pDatabaseFilePath)
{
	for(AddressRepLookupList::iterator it = mAddressRepLookupList.begin(); it != mAddressRepLookupList.end(); ++it)
	{
		IAddressRepLookup* const pARL = *it;

		const wchar_t* pExistingDatabaseFilePath = pARL->GetDatabasePath();

		if(EA::StdC::Stricmp(pDatabaseFilePath, pExistingDatabaseFilePath) == 0)
		{
			mAddressRepLookupList.erase(it);
			delete pARL;
			return true;
		}
	}

	return false;
}


///////////////////////////////////////////////////////////////////////////////
// GetAddressRepLookup
//
IAddressRepLookup* AddressRepLookupSet::GetAddressRepLookup(const wchar_t* pDatabaseFilePath)
{
	for(AddressRepLookupList::iterator it = mAddressRepLookupList.begin(); it != mAddressRepLookupList.end(); ++it)
	{
		IAddressRepLookup* const pARL = *it;

		const wchar_t* pExistingDatabaseFilePath = pARL->GetDatabasePath();

		if(EA::StdC::Stricmp(pDatabaseFilePath, pExistingDatabaseFilePath) == 0)
			return pARL;
	}

	return NULL;
}


///////////////////////////////////////////////////////////////////////////////
// GetDatabaseFileCount
//
size_t AddressRepLookupSet::GetDatabaseFileCount() const
{
	return (size_t)mAddressRepLookupList.size();
}


///////////////////////////////////////////////////////////////////////////////
// GetDatabaseFile
//
IAddressRepLookup* AddressRepLookupSet::GetDatabaseFile(size_t index)
{
	if(index < (size_t)mAddressRepLookupList.size())
	{
		AddressRepLookupList::iterator it = mAddressRepLookupList.begin();
		eastl::advance(it, index);
		return *it;
	}

	return NULL;
}


///////////////////////////////////////////////////////////////////////////////
// AddAddressRepLookup
//
void AddressRepLookupSet::AddAddressRepLookup(IAddressRepLookup* pAddressRepLookup)
{
	// It is assumed that the pAddressRepLookup instance is already initialized.
	mAddressRepLookupList.push_back(pAddressRepLookup);
}


///////////////////////////////////////////////////////////////////////////////
// AddSourceCodeDirectory
//
void AddressRepLookupSet::AddSourceCodeDirectory(const wchar_t* pSourceDirectory)
{
	mSourceCodeDirectoryList.push_back();
	mSourceCodeDirectoryList.back() = pSourceDirectory;
}


///////////////////////////////////////////////////////////////////////////////
// GetAddressRep
//
int AddressRepLookupSet::GetAddressRep(int addressRepTypeFlags, uint64_t address, FixedString* pRepArray, int* pIntValueArray)
{
	// It turns out that we need to do a file/line lookup in order to do
	// a source lookup.
	if(addressRepTypeFlags & kARTFlagSource)
		addressRepTypeFlags |= kARTFlagFileLine;

	// Possibly go and auto-find symbol file sources.
	if(mAddressRepLookupList.empty() && mbEnableAutoDatabaseFind && !mbLookedForDBFiles)
		AutoDatabaseAdd();

	// Look at our set of symbol file sources.
	int returnFlags = GetAddressRepFromSet(addressRepTypeFlags, address, pRepArray, pIntValueArray);

	// Implement kARTFlagAddress.
	if(addressRepTypeFlags & kARTFlagAddress)
		returnFlags |= GetAddressRepAddress(address, pRepArray, pIntValueArray);

	// Implement kARTFlagSource.
	if((addressRepTypeFlags & kARTFlagSource) && !pRepArray[kARTFileLine].empty())
		returnFlags |= GetAddressRepSource(pRepArray, pIntValueArray);

	return returnFlags;
}


///////////////////////////////////////////////////////////////////////////////
// AutoDatabaseAdd
//
void AddressRepLookupSet::AutoDatabaseAdd()
{
	if(!mbLookedForDBFiles)
	{
		mbLookedForDBFiles = true; // Set this to true so we don't keep trying this if it fails.

		// Walk the loaded modules and see if any of them has a corresponding .map/.pdb/.elf file that we can add.
		if(mpCoreAllocator) // To consider: Support using a local array of N ModuleInfos if mpCoreAllocator is NULL or fails to allocate.
		{
			size_t moduleCount = EA::Callstack::GetModuleInfo(NULL, 0);

			if((moduleCount > 0) && (moduleCount <= kAddressRepLookupMaxModules))
			{
				EA::Callstack::ModuleInfo* pModuleInfoArray = CORE_NEW_ARRAY(mpCoreAllocator, ModuleInfo, moduleCount, "ModuleInfo", EA::Allocator::MEM_TEMP);

				if(pModuleInfoArray)
				{
					wchar_t processDirectory[IO::kMaxPathLength]; // e.g. /abc/def/
					size_t processDirectoryLength = EA::StdC::GetCurrentProcessDirectory(processDirectory, EAArrayCount(processDirectory));

					// Until some day that we fix the .elf (DWARF) parsing code, we aren't able to parse .elf/.self files directly.
					// The problem is that at some point DWARF-generating compilers revised their code to write debug info in a way
					// that we can't understand. Also, we have some prototype code to read .pdb files natively but it turns out
					// that doesn't work any more either, due to Microsoft changes.
					#if EA_WINAPI_FAMILY_PARTITION(EA_WINAPI_PARTITION_DESKTOP) // We can read .pdb files directly on Windows because we can use a Microsoft API.
						const wchar_t* kExtensionTypes[] = { L".pdb", L".map" };
					#else
						const wchar_t* kExtensionTypes[] = { L".map" };
					#endif

					moduleCount = EA::Callstack::GetModuleInfo(pModuleInfoArray, moduleCount);

					// Walk through all the modules and see if the module appears to have a corresponding .map/.pdb, etc file we can load.
					for(size_t i = 0; i < moduleCount; i++)
					{
						// Some platforms (e.g. Microsoft) provide a path, some don't (e.g. Sony).
						wchar_t pathW[EA::IO::kMaxPathLength];

						if(EA::IO::Path::IsRelative(pModuleInfoArray[i].mPath.c_str())) // If mPath is only a relative path...
						{
							// We try making a full path relative to our process directory. A lot of the time this will result
							// in invalid paths because it will generate system library paths in our process directory. That's fine,
							// and we'll just let it fail for those cases.
							EA::StdC::Strlcpy(pModuleInfoArray[i].mPath, processDirectory, processDirectoryLength); // Newer versions of EASTL can do this directly instead of using Strlcpy.
							pModuleInfoArray[i].mPath += pModuleInfoArray[i].mName;
						}

						// First try to add a database for the module itself (e.g. some modules contain debug information .elf/.so/.prx/etc.)
						EA::StdC::Strlcpy(pathW, pModuleInfoArray[i].mPath.c_str(), EAArrayCount(pathW));
						if (IO::File::Exists(pathW) && !GetAddressRepLookup(pathW)) // If C:\blah\file.map exists and if it's not already registered... add it.
							AddDatabaseFile(pathW, pModuleInfoArray[i].mBaseAddress, true, mpCoreAllocator);

						// Second try variations of the module path
						FixedString8 pathVariations[] =
						{
							pModuleInfoArray[i].mPath,				// this variation modifies the existing extension (e.g. .exe -> .map)
							pModuleInfoArray[i].mPath + ".___"      // this variation appends to the existing extension (e.g. .exe -> .exe.map)
						};
						for (auto& variation : pathVariations)
						{
							// Now that we have a full module path, see if there are .map analogs for it.
							EA::StdC::Strlcpy(pathW, variation.c_str(), EAArrayCount(pathW));
							wchar_t* pExtension = IO::Path::GetFileExtension(pathW);
							
							if (*pExtension) // If there is an extension
							{
								for (size_t j = 0; j < EAArrayCount(kExtensionTypes); j++)
								{
									const wchar_t* newExt = kExtensionTypes[j];
									if ((pExtension + EA::StdC::Strlen(newExt)) < (pathW + EAArrayCount(pathW))) // Check if there is enough space for the new extension
									{
										EA::StdC::Strcpy(pExtension, newExt);
											if (IO::File::Exists(pathW) && !GetAddressRepLookup(pathW)) // If C:\blah\file.map exists and if it's not already registered... add it.
												AddDatabaseFile(pathW, pModuleInfoArray[i].mBaseAddress, true, mpCoreAllocator);
									}
								}
							}
						}
					}

					CORE_DELETE_ARRAY(mpCoreAllocator, pModuleInfoArray);
				}
			}
		}


		// Fallback basic functionality for the case that module iteration above isn't possible.
		// In this case we just directly look for a corresponding symbol file for the application.
		{
			wchar_t dbPath[IO::kMaxPathLength];          // e.g. /abc/def/someapp.elf
			EA::StdC::GetCurrentProcessPath(dbPath, EAArrayCount(dbPath));

			// Under GCC, the application file (ELF file) also contains the debug information.
			// As of this writing, the following probably won't work in practice because our .elf (DWARF) parsing code doesn't
			// work with recent platform .elf files due to compiler/linker changes that have happened in the last few years.
			#if defined(__GNUC__) || defined(__clang__)
				if(IO::File::Exists(dbPath) && !GetAddressRepLookup(dbPath)) // If C:\blah\file.map exists and if it's not already registered... add it.
				{
					eastl::fixed_substring<wchar_t> modulePath(dbPath);
					const wchar_t* moduleName = modulePath.c_str();

					#if defined(EA_PLATFORM_SONY)
						// the /host/ dir might be used which will mix and match unix and windows
						// path names so we simplify the path string since it can confuse EAIO
						EA::IO::Path::PathStringW::size_type slashPos = modulePath.find_last_of(L'\\');
						if (slashPos < modulePath.length())
						{
							moduleName = modulePath.data() + slashPos + 1;
						}
					#endif

					uint64_t baseAddress = EA::Callstack::GetModuleBaseAddress(moduleName);

					AddDatabaseFile(dbPath, baseAddress, true, mpCoreAllocator);
				}
			#endif

			wchar_t* pExtension = IO::Path::GetFileExtension(dbPath);

			if((pExtension + 4) < (dbPath + EAArrayCount(dbPath))) // If there's enough space...
			{
				// See if <app>.map exists.
				EA::StdC::Strcpy(pExtension, L".map");
				if(IO::File::Exists(dbPath) && !GetAddressRepLookup(dbPath)) // If C:\blah\file.map exists and if it's not already registered... add it.
					AddDatabaseFile(dbPath, kBaseAddressUnspecified, true, mpCoreAllocator);

				// See if <app>.pdb exists.
				EA::StdC::Strcpy(pExtension, L".pdb");
				if(IO::File::Exists(dbPath) && !GetAddressRepLookup(dbPath)) // If C:\blah\file.pdb exists and if it's not already registered... add it.
					AddDatabaseFile(dbPath, kBaseAddressUnspecified, true, mpCoreAllocator);
			}
		}
	}
}


///////////////////////////////////////////////////////////////////////////////
// GetAddressRepFromSet
//
int AddressRepLookupSet::GetAddressRepFromSet(int addressRepTypeFlags, uint64_t address, FixedString* pRepArray, int* pIntValueArray)
{
	int returnFlags = 0;

	// Don't search for kARTAddress and kARTSource. We search for them separately.
	addressRepTypeFlags &= ~(kARTFlagAddress | kARTFlagSource);
	int returnRepTypeFlags = 1; // must be non-zero

	// We try each AddressRepSource until all requested addressRepTypeFlags are found.
	// Of course we might try all of them without all addressRepTypeFlags being found.
	for(AddressRepLookupList::iterator it = mAddressRepLookupList.begin();
		returnRepTypeFlags && (it != mAddressRepLookupList.end()); ++it)
	{
		IAddressRepLookup* const pARL = *it;

		returnFlags |= pARL->GetAddressRep(addressRepTypeFlags, address, pRepArray, pIntValueArray);

		// If we find anything then don't try to look for it any more.
		returnRepTypeFlags &= ~returnFlags;
	}

	// To consider: Move the following processing to a more logical/sensible place. One thought is to
	//              create a special IAdressRepLookup class soley for "local process" lookups which
	//              takes advantage of system-provided symbol information when present.
	if(returnFlags == 0) // If nothing useful was found
	{
		if(GetLookupsAreLocal()) // If the data we are analyzing is from our own process...
		{
			#if EACALLSTACK_GLIBC_BACKTRACE_AVAILABLE
				// The symbol names may be unavailable without the use of special linker
				// options. For systems using the GNU linker, it is necessary to use the
				// -rdynamic linker option. Note that names of "static" functions are not
				// exposed and won't be available .
				// http://linux.die.net/man/3/backtrace_symbols
				void*  pAddress = reinterpret_cast<void*>(static_cast<uintptr_t>(address));
				char** strings  = backtrace_symbols(&pAddress, 1);

				if(strings)
				{
					FixedString strUnmangled(EA::Callstack::EASTLCoreAllocator(EACALLSTACK_ALLOC_PREFIX "EACallstack/AddressRepLookupSet", mpCoreAllocator));
					char        moduleName[256];      moduleName[0] = 0;
					char        functionMangled[512]; functionMangled[0] = 0;
					int         offset;

					//     "0   ExceptionHandlerTest                0x000e2dcc _ZN2EA5Debug16ExceptionHandler25SimulateExceptionHandlingEi + 60"
					auto parsed = EA::StdC::Sscanf(strings[0], "%*s %256s %*s %512s + %d", moduleName, functionMangled, &offset);
					if(parsed != 3)
					{
						#define EACallStack_min(x,y)  ((x)>(y)?(y):(x))

						//     "ExceptionHandlerTest(_ZN2EA5Debug16ExceptionHandler25SimulateExceptionHandlingEi+0x60)"
						size_t length;
						const char* token = EA::StdC::Strtok2(strings[0], "(", &length, true);
						if (token != nullptr)
						{
							int strLen = EACallStack_min(length, sizeof(moduleName) - 1);
							EA::StdC::Strncpy(moduleName, token, strLen);
							moduleName[strLen] = 0;

							token = EA::StdC::Strtok2(token + length + 1, "+", &length, true);
							if (token != NULL)
							{
								strLen = EACallStack_min(length, sizeof(functionMangled) - 1);
								EA::StdC::Strncpy(functionMangled, token, strLen);
								functionMangled[strLen] = 0;

								token = EA::StdC::Strtok2(token + length + 1, ")", &length, true);
								if (token != nullptr)
								{
									char strOffset[6];
									strLen = EACallStack_min(length, sizeof(strOffset) - 1);
									EA::StdC::Strncpy(strOffset, token, strLen);
									strOffset[strLen] = 0;

									offset = EA::StdC::StrtoU32(strOffset, nullptr, 0);
								}
							}
						}

						#undef EACallStack_min
					}
					
					UnmangleSymbol(functionMangled, strUnmangled, kCompilerTypeGCC);

					// To consider: include moduleName in the representation.
					pRepArray[kARTFunctionOffset]      = strUnmangled.c_str();
					pIntValueArray[kARTFunctionOffset] = offset;
					addressRepTypeFlags |= kARTFlagFunctionOffset;

					free(strings); // Apple's documentation for backtrace_symbols specifically states that we don't need to free the individual strings.
				}
			#endif
		}
	}

	return returnFlags;
}


///////////////////////////////////////////////////////////////////////////////
// GetAddressRepAddress
//
int AddressRepLookupSet::GetAddressRepAddress(uint64_t address, FixedString* pRepArray, int* /*pIntValueArray*/)
{
	AddressToString(address, pRepArray[kARTAddress], true);
	return kARTFlagAddress;
}


///////////////////////////////////////////////////////////////////////////////
// GetAddressRepSource
//
int AddressRepLookupSet::GetAddressRepSource(FixedString* pRepArray, int* pIntValueArray)
{
	int returnFlags = 0;

	if(mnSourceCodeFailureCount < 64)
	{
		// Convert char path to wchar_t, as our file path functions are wchar_t.
		wchar_t pPathW[IO::kMaxPathLength];
		EA::StdC::Strlcpy(pPathW, pRepArray[kARTFileLine].c_str(), IO::kMaxPathLength);

		// If the given path exists outright, we can use it as-is and don't need to do
		// any searches through our mSourceCodeDirectoryList.
		bool bFound = IO::File::Exists(pPathW);

		if(!bFound)
		{
			const size_t pathLength = EA::StdC::Strlen(pPathW);

			// Perhaps the file path was not a full path or referred to a hard drive that
			// is on another computer, such as the host computer of a console machine.
			// What we want to do here is match the file name/path in pFilePath with
			// the directory root(s) specified in mSourceCodeDirectoryList. This is not
			// trivial, the first part of pFilePath may not match the first part of an
			// mSourceCodeDirectoryList entry, but the file name may nevertheless match
			// a file name that is a child of an mSourceCodeDirectoryList entry.
			// For example, pFilePath might be C:\Baseball\game\source\ai\ai.cpp, while
			// the source code directory list may contain the directory /host/game/source/
			// of which /host/game/source/ai/ai.cpp is a file.

			// Loop through all the source code directories that have been added via AddSourceCodeDirectory.
			for(FixedStringWList::iterator it(mSourceCodeDirectoryList.begin()); !bFound && (it != mSourceCodeDirectoryList.end()); ++it)
			{
				const FixedStringW& sSourceDirectory = *it;

				// Try joining the base source code directory with pFilePath (which came
				// from the symbol database). If the resulting file doesn't exist, remove
				// the root-most path component from pFilePath and try again.
				for(IO::Path::PathStringW::iterator itPath(pPathW), itPathEnd(pPathW + pathLength);
					itPath != itPathEnd;
					itPath = IO::Path::FindComponentFwd(itPath))
				{
					IO::Path::PathStringW sTestPath(sSourceDirectory.c_str());
					IO::Path::Append(sTestPath, itPath);
					IO::Path::Canonicalize(sTestPath);

					if(IO::File::Exists(sTestPath.c_str()))
					{
						bFound = true;

						EA::StdC::Strncpy(pPathW, sTestPath.c_str(), IO::kMaxPathLength);
						pPathW[IO::kMaxPathLength - 1] = 0;

						break;
					}
				}
			}

			// To consider: Take the mSourceCodeDirectoryList entry list that successfully contained the
			// source code and if it isn't already at the front of the list, put it there so it will be
			// searched first next time around.
		}

		if(bFound)
		{
			// Copy the source code to the user supplied output string.
			IO::FileStream fileStream(pPathW);

			if(fileStream.Open(IO::kAccessFlagRead))
			{
				const size_t fileSize  = (size_t)fileStream.GetSize();
				const char   zeroValue = 0;

				eastl::vector<char, EA::Callstack::EASTLCoreAllocator> fileData((eastl_size_t)fileSize, zeroValue, EA::Callstack::EASTLCoreAllocator(EACALLSTACK_ALLOC_PREFIX "EACallstack/AddressRepLookupSet", mpCoreAllocator));

				fileStream.Read(&fileData[0], fileSize);
				fileStream.Close();

				const char* pLineNext   = fileData.data();
				const char* pFileEnd    = fileData.data() + fileSize;
				const int      lineMatch   = pIntValueArray[kARTFileLine];
				const int      lineRangeLo = (int)(lineMatch - (mnSourceContextLineCount / 2));
				const int      lineRangeHi = (int)(lineMatch + (mnSourceContextLineCount / 2));

				// Scan every line of source code up to the end of the desired line range.
				// To consider: This would be faster if we simply counted \n characters
				// instead of copying lines that we don't use.
				for(int lineCount = 1; (pLineNext < pFileEnd) && (lineCount <= lineRangeHi); ++lineCount)
				{
					const char* const pLine    = pLineNext;
					const char* const pLineEnd = EA::StdC::GetTextLine(pLine, pFileEnd, &pLineNext);

					if((lineCount >= lineRangeLo) && (lineCount <= lineRangeHi))
					{
						// We have a source code line in the desired range. Store it.
						*const_cast<char*>(pLineEnd) = 0;

						if(lineCount == lineMatch)
							pRepArray[kARTSource] += "> => ";
						else
							pRepArray[kARTSource] += ">    ";
						pRepArray[kARTSource] += pLine;
						pRepArray[kARTSource] += "\r\n";  // To consider: make this newline type configurable to \r\n or \n.

						returnFlags |= kARTFlagSource;
					}
				}
			}
		}
	} // Else we gave up trying to convert source code and we're just thrashing.

	if(!returnFlags)
		++mnSourceCodeFailureCount;

	return returnFlags;
}

} // namespace Callstack
} // namespace EA
