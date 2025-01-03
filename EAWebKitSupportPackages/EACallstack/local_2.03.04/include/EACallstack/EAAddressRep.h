/////////////////////////////////////////////////////////////////////////////
// Copyright (c) Electronic Arts Inc. All rights reserved.
/////////////////////////////////////////////////////////////////////////////

///////////////////////////////////////////////////////////////////////////////
// EAAddressRep is the symbol lookup and caching engine of EACallstack.
// It allows for symbol lookups at runtime for a given platform and for
// cross-platform symbols lookups outside of a running application.
//
// The class users should be most concerned with here is AddressRepCache,
// as it is a high-level accessor to the rest of the EAAddressRep library.
// Normally you want to do all your operations through AddressRepCache,
// though you may be interested in doing uncached lookups through the
// AddressRepLookupSet class.
//
// A typical high level function you might want to use is this:
//
//     bool GetAddressRepFromAddress(uint64_t address, eastl::string8& sAddressRep)
//     {
//         const char* pStrResults[Callstack::kARTCount];
//         int         pIntResults[Callstack::kARTCount];
//
//         sAddressRep.clear();
//
//         const int outputFlags = gAddressRepCache.GetAddressRep(mAddressRepTypeFlags, address, pStrResults, pIntResults);
//
//         if(outputFlags & kARTFlagAddress)
//             sAddressRep.append_sprintf("%s\r\n", pStrResults[kARTSource]);
//
//         if(outputFlags & kARTFlagFileLine)
//             sAddressRep.append_sprintf("%s(%d)\r\n", pStrResults[kARTFileLine], pIntResults[kARTFileLine]);
//
//         if(outputFlags & kARTFlagFunctionOffset)
//             sAddressRep.append_sprintf("%s + %d\r\n", pStrResults[kARTFunctionOffset], pIntResults[kARTFunctionOffset]);
//
//         if(outputFlags & kARTFlagSource)
//             sAddressRep.append_sprintf("%s\r\n", pStrResults[kARTSource]);
//     }
///////////////////////////////////////////////////////////////////////////////


#ifndef EACALLSTACK_EAADDRESSREP_H
#define EACALLSTACK_EAADDRESSREP_H


#include <EACallstack/internal/Config.h>

#if defined(EA_PRAGMA_ONCE_SUPPORTED)
	#pragma once
#endif // EA_PRAGMA_ONCE_SUPPORTED

#include <coreallocator/icoreallocator_interface.h>
#include <EACallstack/internal/EASTLCoreAllocator.h>
#include <EACallstack/IAddressRep.h>
#include <EACallstack/EAModule.h>
#include <EACallstack/Allocator.h>
#include <EASTL/hash_map.h>
#include <EASTL/list.h>


EA_DISABLE_VC_WARNING(4251); // class (some template) needs to have dll-interface to be used by clients.


namespace EA
{
namespace Callstack
{


/// AddressToString
///
/// Converts an address to a string. Deals with platform differences.
/// The address string will padded with zeroes on the left. It will
/// be prefixed by 0x if the bLeading0x parameter is true.
///
EACALLSTACK_API FixedString& AddressToString(uint64_t address, FixedString& s, bool bLeading0x = true);
EACALLSTACK_API CAString8&   AddressToString(uint64_t address, CAString8& s, bool bLeading0x = true);

inline FixedString& AddressToString(const void* pAddress, FixedString& s, bool bLeading0x = true)
{
	return AddressToString(((uint64_t)(uintptr_t)pAddress), s, bLeading0x);
}
inline CAString8& AddressToString(const void* pAddress, CAString8& s, bool bLeading0x = true)
{
	return AddressToString(((uint64_t)(uintptr_t)pAddress), s, bLeading0x);
}


/// SymbolInfoType
///
/// Defines a type of symbol database. A Microsoft .pdb file is a symbol
/// database, for example. This enumeration identifies a specific format
/// of a database, and thus a .pdb from VC7 might be different from a
/// .pdb from VC8.
///
enum SymbolInfoType
{
	kSymbolInfoTypeNone = 0,    /// No type in particular.
	kSymbolInfoTypeMapVC,       /// Used by VC++ (all versions).
	kSymbolInfoTypeMapGCC,      /// Used by GCC 3.x and later. The format varies a bit with successive GCC versions,
								/// and some GCCs (e.g. Apple) use an entirely different map file format.
	kSymbolInfoTypeMapSN,       /// Used by SN linker.
	kSymbolInfoTypeMapApple,    /// Used by Apple's GCC-based linker.
	kSymbolInfoTypePDB7,        /// Used by VC7 (VS 2003). PDB (program database) files.
	kSymbolInfoTypePDB8,        /// Used by VC8 (VS 2005). PDB (program database) files.
	kSymbolInfoTypeDWARF3,      /// Used by GCC 3.x and later. Embedded in .elf files.
};

/// TargetOS
///
/// Identifies an executable target OS.
///
enum TargetOS
{
	kTargetOSNone,
	kTargetOSWin32,
	kTargetOSWin64,
	kTargetOSMicrosoftMobile,
	kTargetOSMicrosoftConsole,
	kTargetOSLinux,
	kTargetOSOSX,           // Apple desktop OS
	kTargetOSIOS,           // Apple mobile OS (e.g iPhone)
	kTargetOSAndroid,
	kTargetOSNew,           // Identifies an arbitrary new OS that exists but isn't necessarily identifiable.
};

/// TargetProcessor
///
/// Identifies an executable target processor.
///
enum TargetProcessor
{
	kTargetProcessorNone,
	kTargetProcessorPowerPC32,
	kTargetProcessorPowerPC64,
	kTargetProcessorX86,
	kTargetProcessorX64,
	kTargetProcessorARM,
};


/// GetSymbolInfoTypeFromDatabase
///
/// Given a symbol database file, determine what kind of SymbolInfoType goes with it.
/// Note that the term "symbol database" refers to a .pdb file, .map file, or
/// .elf file with debug information.
///
EACALLSTACK_API SymbolInfoType GetSymbolInfoTypeFromDatabase(const wchar_t* pDatabaseFilePath,
															 TargetOS* pTargetOS = nullptr,
															 TargetProcessor* pTargetProcessor = NULL);
/// GetCurrentOS
///
/// Returns the OS that the current application was compiled for. EABase's EA_PLATFORM_XXX
/// It's possible that the application might be running in some kind of emulation
/// mode under another OS, though. The most common example is a Win32 app under Win64.
/// This is useful for using in conjunction with GetSymbolInfoTypeFromDatabase.
///
TargetOS GetCurrentOS();


/// GetCurrentProcessor
///
/// Returns the processor that the current application was compiled for. EABase's EA_PROCESSOR_XXX
/// This is useful for using in conjunction with GetSymbolInfoTypeFromDatabase.
///
TargetProcessor GetCurrentProcessor();


/// MakeAddressRepLookupFromDatabase
///
/// Returns a new appropriate IAddressRepLookup object based on the the given
/// database file path. Returns NULL if the type could not be determined.
/// The return value is allocated via global new and the call assumes ownership
/// of the pointer. This function does not call Init on the resulting instance;
/// it is merely a factory for the instance.
/// The returned IAddressRepLookup* should be destroyed with DestoryAddressRepLookup
/// called with the same ICoreAllocator as was passed to this function.
///
EACALLSTACK_API IAddressRepLookup* MakeAddressRepLookupFromDatabase(const wchar_t* pDatabaseFilePath,
																	EA::Allocator::ICoreAllocator* pCoreAllocator,
																	SymbolInfoType symbolInfoType = kSymbolInfoTypeNone);
/// DestroyAddressRepLookup
///
/// Matches MakeAddressRepLookupFromDatabase
///
EACALLSTACK_API void DestroyAddressRepLookup(IAddressRepLookup* pLookup, EA::Allocator::ICoreAllocator* pCoreAllocator);


/// AddressRepLookupSet
///
/// Implements a container of IAddressRepLookup objects, presenting them as a single
/// IAddressRepLookup interface. It may happen that you have multiple symbol files
/// (e.g. .pdb files) to work with. This class manages the set of them and makes it
/// so you can work with a single interface to all of them.
///
/// This class additionally has the ability to implement kARTSource lookups using
/// the information from kARTFileLine lookups.
///
/// Normally you will want to use the AddressRepCache class instead of this, as
/// that class owns an instance of this class but will cache lookup results it
/// gets from this class and will thus run much faster under most practical
/// circumstances, though at the cost of the cache memory.
///
class EACALLSTACK_API AddressRepLookupSet : public IAddressRepLookupBase
{
public:
	using IAddressRepLookupBase::GetAddressRep;

	AddressRepLookupSet(EA::Allocator::ICoreAllocator* pCoreAllocator = nullptr);
	~AddressRepLookupSet();

	/// SetAllocator
	///
	/// Sets the memory allocator used by an instance of this class. By default
	/// the global EACallstack ICoreAllocator is used (EA::Callstack::GetAllocator()).
	///
	void SetAllocator(EA::Allocator::ICoreAllocator* pCoreAllocator);


	/// Init
	///
	/// Initializes the class for use after construction. Init is the first function
	/// that may possibly allocate memory or resources needed for this class to work.
	///
	bool Init();


	/// Shutdown
	///
	/// Shuts down an instance of this class to match a call to Init. Upon Shutdown,
	/// any resources allocated during or after Init will be freed.
	///
	bool Shutdown();


	/// GetLookupsAreLocal
	///
	/// Indicates whether lookups are being done refer to the current process, as opposed to
	/// some other process. There are some advantages that can be had when looking up symbols
	/// for the current process, such as using some kinds of system-provided lookup functionality.
	///
	bool GetLookupsAreLocal() const        { return mbLookupsAreLocal;   }
	void SetLookupsAreLocal(bool bLocal)   { mbLookupsAreLocal = bLocal; }


	/// AddDatabaseFile
	///
	/// Adds a symbol database for use when looking up symbols for a program address.
	/// A database is a .pdb file, .map file, or .elf file, for example.
	/// A variety of symbol database formats are supported, including PDB, DWARF3 (contained
	/// in ELF/SELF executables), and Map files. Currently the type of database is
	/// determined by the file extension in the path provided.
	/// The return value indicates whether the given is registered upon return.
	/// Doesn't re-register a database file if it's already registered.
	///
	/// This function is a higher level alternative to the AddAddressRepLookup function.
	/// It creates the appropriate IAddressRepLookup class from the given symbol
	/// file and then calls AddAddressRepLookup.
	///
	/// The pCoreAllocator argument specifies the ICoreAllocator that the database file
	/// object (an instance of IAddressRepLookup) will use. If pCoreAllocator is NULL,
	/// our own allocator will be used.
	///
	bool AddDatabaseFile(const wchar_t* pDatabaseFilePath, uint64_t baseAddress = kBaseAddressUnspecified,
						 bool bDelayLoad = true, EA::Allocator::ICoreAllocator* pCoreAllocator = nullptr);

	/// Same as AddDatabaseFile excepts adds two bool arguments.
	/// If bDBPreviouslyPresent, bBaseAddressAlreadySet both return as true then
	/// this call had no effect, but the return value will still be true.
	/// The return value indicates whether the given is registered upon return.
	bool AddDatabaseFileEx(const wchar_t* pDatabaseFilePath, uint64_t baseAddress,
						   bool bDelayLoad, EA::Allocator::ICoreAllocator* pCoreAllocator,
						   bool& bDBPreviouslyPresent, bool& bBaseAddressAlreadySet);

	/// RemoveDatabaseFile
	///
	/// Removes a database file specified by AddDatabaseFile.
	/// Returns true if a the database file was found, in which case it can always
	/// be removed.
	///
	bool RemoveDatabaseFile(const wchar_t* pDatabaseFilePath);


	/// GetDatabaseFileCount
	///
	/// Returns the number of databases successfully added via AddDatabaseFile.
	///
	size_t GetDatabaseFileCount() const;


	/// GetDatabaseFile
	///
	/// Returns the Nth IAddressRepLookup which was added via AddDatabaseFile.
	/// Returns NULL if the given index is >= the database file count.
	/// The return value is valid only as long as RemoveDatabaseFile isn't called.
	///
	IAddressRepLookup* GetDatabaseFile(size_t index);


	/// GetAddressRepLookup
	///
	/// Returns the IAddressRepLookup for a database file previously registered via AddDatabaseFile.
	/// Returns NULL if no database file for the given path was added, or if one was added by removed via RemoveDatabaseFile.
	/// The user can use AsInterface to cast the return value to a specific type.
	///
	IAddressRepLookup* GetAddressRepLookup(const wchar_t* pDatabaseFilePath);


	/// AddAddressRepLookup
	///
	/// Adds an IAddressRepLookup object to the managed set.
	/// The added object is added to the back of the queue of IAddressRepLookup
	/// objects that are consulted in the GetAddressRep function.
	/// It is expected that the Init function has already been called on the
	/// pAddressRepLookup instance.
	///
	/// This function is a lower level alternative to the AddDatabaseFile function
	/// and is preferred when you want to have more control over the AddressRepLookup
	/// object, such as specifying a custom memory allocator.
	///
	void AddAddressRepLookup(IAddressRepLookup* pAddressRepLookup);


	/// AddSourceCodeDirectory
	///
	/// Adds a file system directory to the set we use to find file source code.
	/// If a given directory is the root of a tree of source directories, you need only
	/// supply the root directory. Since most symbol databases store full paths
	/// to source files, often only the home or root directory location of the source
	/// code is required. The source directory supplied here need not be the same
	/// directory path as the one that the symbol files were built with.
	///
	void AddSourceCodeDirectory(const wchar_t* pSourceDirectory);


	/// GetAddressRep
	///
	/// Looks up one or more of the given AddressRepType values. We have a single function for
	/// all three types because it is much faster to do lookups of multiple types at the same time.
	/// Returns flags indicating which of the AddressRepType values were successfully looked up.
	/// The input pRepArray and pIntValueArray need to have a capacity for all AddressRepTypes
	/// (i.e. kARTCount), as each result is written to its position in the array.
	/// Unused results are set to empty strings.
	/// The kARTAddress AddressRepType always succeeds, as it is merely a translation
	/// of a pointer to a hex number.
	///
	/// Example usage:
	///     FixedString pStrResults[kARTCount];
	///     int         pIntResults[kARTCount];
	///     int         inputFlags = (kARTFlagFunctionOffset | kARTFlagFileLine | kARTFlagSource);
	///     int         outputFlags;
	///
	///     outputFlags = addressRepLookup.GetAddressRep(inputFlags, pSomeAddress, pStrResults, pIntResults);
	///
	///     if(outputFlags & kARTFlagFileLine)
	///         printf("File: %s, line: %d", pStrResults[kARTFileLine].c_str(), pIntResults[kARTFileLine]);
	///
	virtual int GetAddressRep(int addressRepTypeFlags, uint64_t address, FixedString* pRepArray, int* pIntValueArray) override;


	/// EnableAutoDatabaseFind
	///
	/// Enables or disables the ability to automatically find the associated symbol file
	/// for the currently running application. This is simply a matter of looking for files
	/// with the same name as the executable but with a different extension, such as .map.
	/// This functionality is only useful when an application needs to do symbol lookups
	/// on itself, and is not useful for when you are doing symbol lookups for some other
	/// application. Auto-find only occurs if the user hasn't added any IAddressRepLookup
	/// objects. By default auto-database-find is enabled.
	///
	/// If you expect to use this class within an executing exception handler, you might
	/// want to disable auto-database-find, at least with some platforms. For example, the
	/// PS3 platform exception handling system gives you few resources to work with and
	/// users have seen crashes when trying to auto-load DBs during exception handling.
	///
	void EnableAutoDatabaseFind(bool bEnable);


	/// SetSourceCodeLineCount
	///
	/// Sets the number of lines shown for the given address.
	/// The default value is defined by kSourceContextLineCount.
	///
	void     SetSourceCodeLineCount(unsigned lineCount);
	unsigned GetSourceCodeLineCount() const { return mnSourceContextLineCount; }

protected:
	/// AutoDatabaseAdd
	void AutoDatabaseAdd();

	/// Looks up the AddressRep in our mAddressRepLookupList.
	int GetAddressRepFromSet(int addressRepTypeFlags, uint64_t address, FixedString* pRepArray, int* pIntValueArray);

	/// Gets a kARTAddress for the given pAddress. This is a simple operation,
	/// as it merely calls the AddressToString function, which itself merely
	/// takes a numeric address and converts it to a string.
	int GetAddressRepAddress(uint64_t address, FixedString* pRepArray, int* pIntValueArray);

	/// Attempts to get a kARTSource for the given pAddress.
	int GetAddressRepSource(FixedString* pRepArray, int* pIntValueArray);

protected:
	typedef eastl::list<FixedStringW,       EA::Callstack::EASTLCoreAllocator> FixedStringWList;
	typedef eastl::list<IAddressRepLookup*, EA::Callstack::EASTLCoreAllocator> AddressRepLookupList;

	EA::Allocator::ICoreAllocator* mpCoreAllocator;
	bool                           mbLookupsAreLocal;         /// If true, then the lookups we are doing refer to the current process. True by default.
	bool                           mbEnableAutoDatabaseFind;  /// If true, look for DB files in our home directory if mAddressRepLookupList is empty.
	bool                           mbLookedForDBFiles;        /// If true, then our file lists are empty and we already looked for them in our home directory.
	AddressRepLookupList           mAddressRepLookupList;
	FixedStringWList               mSourceCodeDirectoryList;
	int                            mnSourceCodeFailureCount;
	unsigned                       mnSourceContextLineCount;
};


/// AddressRepEntry
///
/// Defines a representation of what an address refers to. If you have a call
/// stack that refers to a given line of source code, that line can be represented
/// as a file/line pair, a function/offset pair, or simply as the source code
/// text itself.
///
struct EACALLSTACK_API AddressRepEntry
{
	CAString8	 mAddressRep[kARTCount];
	uint32_t     mARTCheckFlags;
	uint32_t     mFunctionOffset;
	uint32_t     mFileOffset;

	AddressRepEntry(EA::Allocator::ICoreAllocator* pCoreAllocator = nullptr);

	~AddressRepEntry() = default;

	void SetAllocator(EA::Allocator::ICoreAllocator* pCoreAllocator);
};

/// AddressRepCache
///
/// Implements a cache of AddressRepEntry values.
///
/// It is expensive to do lookups of AddressRepEntry information out of source code
/// map or database files (e.g. VC++ .pdb files). So we implement a cache of
/// results so that database lookups for a given address need only occur once.
///
/// Example usage:
///     AddressRepCache arc;
///     arc.AddDatabaseFile(L"D:\application.pdb");
///
///     int   line;
///     char* pFile = arc.GetAddressRep(kARTFlagFileLine, 0x12345678, &line);
///
///     if(pFile)
///         printf("Address: 0x%08x -> File: %s, line: %d", 0x12345678, pFile, line);
///
class EACALLSTACK_API AddressRepCache : public IAddressRepLookupBase
{
public:

	using IAddressRepLookupBase::GetAddressRep;

	AddressRepCache(EA::Allocator::ICoreAllocator* pCoreAllocator = nullptr);
	~AddressRepCache() = default;


	/// SetAllocator
	///
	/// Sets the memory allocator used by an instance of this class. By default
	/// the global EACallstack ICoreAllocator is used (EA::Callstack::GetAllocator()).
	///
	void SetAllocator(EA::Allocator::ICoreAllocator* pCoreAllocator);


	/// GetLookupsAreLocal
	///
	/// Indicates whether lookups are being done refer to the current process, as opposed to
	/// some other process. There are some advantages that can be had when looking up symbols
	/// for the current process, such as using some kinds of system-provided lookup functionality.
	///
	bool GetLookupsAreLocal() const
	{
		return mAddressRepLookupSet.GetLookupsAreLocal();
	}

	void SetLookupsAreLocal(bool bLocal)
	{
		mAddressRepLookupSet.SetLookupsAreLocal(bLocal);
	}


	/// AddDatabaseFile
	///
	/// Adds a symbol database for use when looking up symbols for a program address.
	/// A database is a .pdb file, .map file, or .elf file, for example.
	/// A variety of symbol database formats are supported, including PDB, DWARF3 (contained
	/// in ELF/SELF executables), and Map files. Currently the type of database is
	/// determined by the file extension in the path provided.
	/// Doesn't re-register a database file if it's already registered.
	///
	/// The pCoreAllocator argument specifies the ICoreAllocator that the database file
	/// object (an instance of IAddressRepLookup) will use. If pCoreAllocator is NULL,
	/// our own allocator will be used.
	///
	/// See AddressRepLookupSet::AddDatabaseFile.
	///
	bool AddDatabaseFile(const wchar_t* pDatabaseFilePath, uint64_t baseAddress = kBaseAddressUnspecified,
						 bool bDelayLoad = true, EA::Allocator::ICoreAllocator* pCoreAllocator = nullptr)
	{
		return mAddressRepLookupSet.AddDatabaseFile(pDatabaseFilePath, baseAddress, bDelayLoad, pCoreAllocator);
	}

	/// See AddressRepLookupSet::AddDatabaseFileEx.
	bool AddDatabaseFileEx(const wchar_t* pDatabaseFilePath, uint64_t baseAddress,
						   bool bDelayLoad, EA::Allocator::ICoreAllocator* pCoreAllocator,
						   bool& bDBPreviouslyPresent, bool& bBaseAddressAlreadySet)
	{
		return mAddressRepLookupSet.AddDatabaseFileEx(pDatabaseFilePath, baseAddress, bDelayLoad, pCoreAllocator,
													  bDBPreviouslyPresent, bBaseAddressAlreadySet);
	}


	/// RemoveDatabaseFile
	///
	/// Removes a database file specified by AddDatabaseFile.
	/// Returns true if a the database file was found, in which case it can always
	/// be removed.
	/// This function does not clear any cache entries that derived from the
	/// given database file.
	///
	bool RemoveDatabaseFile(const wchar_t* pDatabaseFilePath)
	{
		return mAddressRepLookupSet.RemoveDatabaseFile(pDatabaseFilePath);
	}

	/// GetDatabaseFileCount
	///
	/// See IAddresssRepLookupSet::GetDatabaseFileCount
	///
	size_t GetDatabaseFileCount() const
	{
		return mAddressRepLookupSet.GetDatabaseFileCount();
	}


	/// GetDatabaseFile
	///
	/// See IAddresssRepLookupSet::GetDatabaseFile
	///
	IAddressRepLookup* GetDatabaseFile(size_t index)
	{
		return mAddressRepLookupSet.GetDatabaseFile(index);
	}


	/// GetAddressRepLookupSet
	///
	/// Returns the underlying AddressRepLookupSet used for lookups.
	///
	AddressRepLookupSet& GetAddressRepLookupSet()
	{
		return mAddressRepLookupSet;
	}


	/// GetAddressRepLookup
	///
	/// The user can use AsInterface to cast the return value to a specific type.
	///
	IAddressRepLookup* GetAddressRepLookup(const wchar_t* pDatabaseFilePath)
	{
		return mAddressRepLookupSet.GetAddressRepLookup(pDatabaseFilePath);
	}


	/// AddAddressRepLookup
	///
	/// Adds an IAddressRepLookup object to the managed set.
	/// The added object is added to the back of the queue of IAddressRepLookup
	/// objects that are consulted in the GetAddressRep function.
	/// It is expected that the Init function has already been called on the
	/// pAddressRepLookup instance.
	///
	/// This function is a lower level alternative to the AddDatabaseFile function
	/// and is preferred when you want to have more control over the AddressRepLookup
	/// object, such as specifying a custom memory allocator.
	///
	void AddAddressRepLookup(IAddressRepLookup* pAddressRepLookup)
	{
		return mAddressRepLookupSet.AddAddressRepLookup(pAddressRepLookup);
	}


	/// AddSourceCodeDirectory
	///
	/// Adds a file system directory to the set we use to find file source code.
	/// If a given directory is the root of a tree of source directories, you need only
	/// supply the root directory. Since most symbol databases store full paths
	/// to source files, often only the home directory or root drive location of
	/// the source code is required when loading source text from the host PC.
	///
	void AddSourceCodeDirectory(const wchar_t* pSourceDirectory)
	{
		mAddressRepLookupSet.AddSourceCodeDirectory(pSourceDirectory);
	}


	/// SetSourceCodeLineCount
	///
	/// Sets the number of lines shown for the given address.
	/// The default value is defined by kSourceContextLineCount.
	///
	void SetSourceCodeLineCount(unsigned lineCount)
	{
		mAddressRepLookupSet.SetSourceCodeLineCount(lineCount);
	}

	unsigned GetSourceCodeLineCount() const
	{
		return mAddressRepLookupSet.GetSourceCodeLineCount();
	}


	/// GetAddressRep
	///
	/// Inherited from the IAddressRepLookupBase base class.
	/// Returns an address representation (e.g. file/line) for a given address.
	/// See AddressRepLookupSet for example usage.
	///
	virtual int GetAddressRep(int addressRepTypeFlags, uint64_t address, FixedString* pRepArray, int* pIntValueArray) override;


	/// NOTE(mwinkler):
	/// The following GetAddressRep() API returns const char* that point to strings directly in the cache,
	/// instead of copying the strings out as with the FixedString GetAddressRep() API.

	/// GetAddressRep
	///
	/// Gets an individual address representation.
	/// This function is an alternative to the GetAddressRep function which takes
	/// addreessRepFlags and returns potentially more than one address rep.
	///
	const char* GetAddressRep(AddressRepType addressRepType, uint64_t address, int* pIntValue, bool bLookupIfMissing);

	const char* GetAddressRep(AddressRepType addressRepType, const void* pAddress, int* pIntValue, bool bLookupIfMissing)
	{
		return GetAddressRep(addressRepType, ((uint64_t)(uintptr_t)pAddress), pIntValue, bLookupIfMissing);
	}


	/// GetAddressRep
	///
	/// Looks up one or more of the given AddressRepType values. We have a single function for
	/// all three types because it is much faster to do lookups of multiple types at the same time.
	/// Returns flags indicating which of the AddressRepType values were successfully looked up.
	/// The input pRepArray and pIntValueArray need to have a capacity for all AddressRepTypes
	/// (i.e. kARTCount), as each result is written to its position in the array.
	/// Unused results are set to empty strings.
	/// The kARTAddress AddressRepType always succeeds, as it is merely a translation
	/// of a pointer to a hex number.
	///
	/// Example usage:
	///     const char* pStrResults[kARTCount];
	///     int         pIntResults[kARTCount];
	///     int         inputFlags = (kARTFlagFunctionOffset | kARTFlagFileLine | kARTFlagSource);
	///     int         outputFlags;
	///
	///     outputFlags = addressRepLookup.GetAddressRep(inputFlags, pSomeAddress, pStrResults, pIntResults);
	///
	///     if(outputFlags & kARTFlagFileLine)
	///         printf("File: %s, line: %d", pStrResults[kARTFileLine], pIntResults[kARTFileLine]);
	///
	int GetAddressRep(int addressRepTypeFlags, uint64_t address, const char** pRepArray, int* pIntValueArray, bool bLookupIfMissing = true);

	int GetAddressRep(int addressRepTypeFlags, const void* pAddress, const char** pRepArray, int* pIntValueArray, bool bLookupIfMissing = true)
	{
		return GetAddressRep(addressRepTypeFlags, ((uint64_t)(uintptr_t)pAddress), pRepArray, pIntValueArray, bLookupIfMissing);
	}


	/// SetAddressRep
	///
	/// Sets the AddressRepEntry associated with the program address to the given data.
	/// Typically this is only used internally by the AddressRepCache class.
	///
	void SetAddressRep(AddressRepType addressRepType, uint64_t address, const char* pRep, size_t nRepLength, int intValue);

	void SetAddressRep(AddressRepType addressRepType, const void* pAddress, const char* pRep, size_t nRepLength, int intValue)
	{
		return SetAddressRep(addressRepType, ((uint64_t)(uintptr_t)pAddress), pRep, nRepLength, intValue);
	}


	/// EnableAutoDatabaseFind
	///
	void EnableAutoDatabaseFind(bool bEnable)
	{
		mAddressRepLookupSet.EnableAutoDatabaseFind(bEnable);
	}


	/// PurgeCache
	///
	/// Purges entries from the cache. How much to purge is determined by the newCacheSize
	/// argument. The newCacheSize argument specifies the desired size of the cache after
	/// the purge. The newCacheSize argument is a hint and the actual amount of remaining
	/// memory may be slightly different from newCacheSize, due to the size of cache entries.
	///
	void PurgeCache(size_t newCacheSize = 0);

protected:

	size_t CalculateMemoryUsage(bool bNewNode, size_t nRepLength);

	void SetAddressRep(AddressRepEntry& ar, AddressRepType addressRepType, uint64_t address, const char* pRep, size_t nRepLength, int intValue);

protected:

	typedef eastl::hash_map<uint64_t, AddressRepEntry, eastl::hash<uint64_t>,
							eastl::equal_to<uint64_t>, EA::Callstack::EASTLCoreAllocator> AddressRepMap;

	EA::Allocator::ICoreAllocator* mpCoreAllocator;        /// The memory allocator we use.
	AddressRepMap                  mAddressRepMap;         /// The address representation cache.
	AddressRepLookupSet            mAddressRepLookupSet;   /// The address representation lookup.
	size_t                         mCacheSize;             /// The current estimated cache size.
	size_t                         mMaxCacheSize;          /// The max size in bytes that the cache (mAddressRepMap) uses.

};


/// GetAddressRepCache
///
/// Retrieves the default global AddressRepCache object.
///
EACALLSTACK_API AddressRepCache* GetAddressRepCache();


/// SetAddressRepCache
///
/// Sets the default global AddressRepCache object.
/// Returns the old AddressRepCache object.
/// The old AddressRepCache is not deleted, as this function is
/// merely an accessor modifier.
///
EACALLSTACK_API AddressRepCache* SetAddressRepCache(AddressRepCache* pAddressRepCache);


} // namespace Callstack
} // namespace EA


EA_RESTORE_VC_WARNING();


#endif // EACALLSTACK_EAADDRESSREP_H
