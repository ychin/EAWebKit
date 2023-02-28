/////////////////////////////////////////////////////////////////////////////
// Copyright (c) Electronic Arts Inc. All rights reserved.
/////////////////////////////////////////////////////////////////////////////

#ifndef EACALLSTACK_IADDRESSREP_H
#define EACALLSTACK_IADDRESSREP_H

#include <EACallstack/internal/Config.h>

#if defined(EA_PRAGMA_ONCE_SUPPORTED)
	#pragma once
#endif // EA_PRAGMA_ONCE_SUPPORTED

#include <EACallstack/internal/EASTLCoreAllocator.h>
#include <EASTL/string.h>
#include <EASTL/fixed_string.h>


namespace EA
{
namespace Callstack
{

/// Number of lines of context delivered with source code text.
static constexpr int kSourceContextLineCount = 8;

/// Return value from BaseAddress functions when the base address is unknown/the function fails.
static constexpr uint64_t kBaseAddressInvalid = 0;

/// Refers to when the base address for an AddressRep instance hasn't been explicitly specified by the user.
/// For example, in the constructor for the PDBFile class its mBaseAddress will initially be set to kBaseAddressUnspecified.
/// Most of the time users don't need to use this constant, though it can be returned by AddressRep::GetBaseAddress().
static constexpr uint64_t kBaseAddressUnspecified = UINT64_C(0xffffffffffffffff);

/// FixedString
typedef eastl::fixed_string<char,     256, true, EA::Callstack::EASTLCoreAllocator> FixedString;
typedef eastl::fixed_string<char,     256, true, EA::Callstack::EASTLCoreAllocator> FixedString8;
typedef eastl::fixed_string<char16_t, 256, true, EA::Callstack::EASTLCoreAllocator> FixedString16;
typedef eastl::fixed_string<char32_t, 256, true, EA::Callstack::EASTLCoreAllocator> FixedString32;

#if defined(EA_WCHAR_UNIQUE) && EA_WCHAR_UNIQUE
	typedef eastl::fixed_string<wchar_t, 256, true, EA::Callstack::EASTLCoreAllocator> FixedStringW;
#else
	#if (EA_WCHAR_SIZE == 1)
		typedef FixedString8 FixedStringW;
	#elif (EA_WCHAR_SIZE == 2)
		typedef FixedString16 FixedStringW;
	#elif (EA_WCHAR_SIZE == 4)
		typedef FixedString32 FixedStringW;
	#endif
#endif


/// A string that takes an ICoreAllocator. Symbols names can be of highly variable
/// length (from 1 to 100 chars), and we don't want to reserve a large fixed space
/// for each. So we go with an ICoreAllocator.
typedef eastl::basic_string<char, EA::Callstack::EASTLCoreAllocator> CAString8;


/// NOTE(mwinkler): The ordering of the two enums must be kept the same!
/// AddressRepType
///
/// Defines the different text representation types for a program address.
/// If you have an address, 0x12345678, that address can be thought of
/// as referring to a source file/line number combination, a C++ function
/// name and byte offset, a C++ line of source code, or simply as a string
/// of the address value, "0x12345678".
enum AddressRepType
{
	kARTFileLine = 0,             /// Source code file path plus line number in the file, beginning with 1.
	kARTFunctionOffset,           /// Function name plus byte count of instructions into the address.
	kARTSource,                   /// Source code text.
	kARTAddress,                  /// The address as a string hex number.
	kARTCount,                    /// Count of AddressRepTypes
};


/// AddressRepTypeFlag
///
/// Flag versions of AddressRepType.
///
enum AddressRepTypeFlag
{
	kARTFlagNone           =  0,  /// No flag in particular.
	kARTFlagFileLine       =  1,  /// Source code file path plus line number in the file, beginning with 1.
	kARTFlagFunctionOffset =  2,  /// Function name plus byte count of instructions into the address.
	kARTFlagSource         =  4,  /// Source code text.
	kARTFlagAddress        =  8,  /// The address as a hex number.
	kARTFlagAll            = 15,  /// All flags ORd together.
};

/// IAddressRepLookupBase
///
class EACALLSTACK_API IAddressRepLookupBase
{
public:

	IAddressRepLookupBase() = default;
	virtual ~IAddressRepLookupBase() = default;

	/// GetAddressRep
	///
	/// Returns an address representation (e.g. file/line) for a given address.
	///
	virtual int GetAddressRep(int addressRepTypeFlags, uint64_t address, FixedString* pRepArray, int* pIntValueArray) = 0;

	virtual int GetAddressRep(int addressRepTypeFlags, const void* pAddress, FixedString* pRepArray, int* pIntValueArray) final
	{
		return GetAddressRep(addressRepTypeFlags, ((uint64_t)(uintptr_t)pAddress), pRepArray, pIntValueArray);
	}

	/// GetAddressRep
	///
	/// Gets an individual address representation.
	/// This function is an alternative to the GetAddressRep function which takes
	/// addreessRepFlags and returns potentially more than one address rep.
	///
	virtual FixedString GetAddressRep(AddressRepType addressRepType, uint64_t address, int* pIntValue) final
	{
		FixedString stringResults[kARTCount];
		int intResults[kARTCount] = { };

		if (GetAddressRep(1 << addressRepType, address, stringResults, intResults))
		{
			if (pIntValue)
			{
				*pIntValue = intResults[addressRepType];
			}
		}

		return stringResults[addressRepType];
	}

	virtual FixedString GetAddressRep(AddressRepType addressRepType, const void* pAddress, int* pIntValue) final
	{
		return GetAddressRep(addressRepType, ((uint64_t)(uintptr_t)pAddress), pIntValue);
	}

};

/// IAddressRepLookup
///
/// This is the virtual base class for all symbol lookups.
/// We call this AddressRepLookup because it looks up a representation
/// for a process's instruction address, and this doesn't necessarily involve
/// just debug symbols. It can additionally include, for example, source code.
///
/// If you are implementing an instance of this class, your instance usually
/// need only respect the kARTFunctionOffset and kARTFileLine flags.
/// The kARTFlagSource and kARTFlagAddress flags are taken care of automatically
/// in the AddressRepLookupSet class, as they are independant of symbol files.
///
class EACALLSTACK_API IAddressRepLookup : public IAddressRepLookupBase
{
public:
	virtual ~IAddressRepLookup() = default;

	/// AsInterface
	/// Safely and dynamically casts this instance to a parent or child class.
	/// A primary usage of this is to allow code to safely downcast an IAddressRepLookup
	/// to a subclass, usually for the purpose of calling functions specific to that
	/// concrete class.
	///
	/// Example usage:
	///    DWARF3File* pDwarfLookup = (DWARF3File*)pAddressRepLookup->AsInterface(DWARF3File::kTypeId);
	///    if(pDwarfLookup)
	///        pDwarfLookup->Blah();
	virtual void* AsInterface(int typeId) = 0;

	/// SetAllocator
	///
	/// Sets the memory allocator used by an instance of this class. By default
	/// the global EACallstack ICoreAllocator is used (EA::Callstack::GetAllocator()).
	///
	virtual void SetAllocator(EA::Allocator::ICoreAllocator* pCoreAllocator) = 0;

	/// Init
	///
	/// Initializes the given instance with a symbol file path.
	/// If bDelayLoad is true, the instance is expected to delay loading of
	/// symbol information until the GetAddressRep function is called.
	///
	virtual bool Init(const wchar_t* pDatabasePath, bool bDelayLoad = true) = 0;

	/// Shutdown
	///
	/// Returns the instance to a state equivalent to that before Init was called.
	///
	virtual bool Shutdown() = 0;

	/// enum Option
	///
	enum Option
	{
		kOptionCopyFileToMemory = 1,  /// Value is 0 or 1; default is 0. A value of 1 means the symbol file is entirely copied
									  /// to system memory during usage. This results in (potentially much) larger memory usage,
									  /// but faster performance.
									  ///
		kOptionSymbolCacheLevel = 2,  /// Value is >= 0; default is 0. A value of 0 means there is no symbol caching; less memory is used,
									  /// but lookups are significantly slower. A value of 1 enables basic symbol caching.
									  ///
		kOptionLowMemoryUsage   = 3,  /// Value is 0 or 1; default is 0. A value of 1 means that memory usage is reduced, but lookups may be
									  /// significantly slower. This is usually due to reading the symbols as we go instead of loading the entire
									  /// file. This is not the same as kOptionCopyFileToMemory, though can be used in conjunction with it.
									  ///
		kOptionFileBufferSize   = 4,  /// Value is between 64 and 65536. Specifies the size of the file buffer used for file reading. This is
									  /// especially applicable in the case of kOptionLowMemoryUsage being enabled. Default is typically 4096.
									  ///
		kOptionOfflineLookup    = 5,  /// Value is 0 or 1; default is 0. Specifies if lookups will be done for addresses of an offline (remote)
									  /// application, such as a PC analyzing console addresses. A value of 0 means that the addresses correspond
									  /// to the currently running application. This option affects the ability of the DB to dynamically look up
									  /// its base address at runtime in order to support DLLs. With a value of 0 (runtime) DBs for DLLs can guess
									  /// their base address correctly, but with a value of 1 (offline) the user will need to manually supply the
									  /// base addresses to DLLs (PRXs) that have been relocated when the app was run.
	};

	/// SetOption
	///
	/// Options must be set before Init is called.
	/// Options are persistent after Shutdown is called. They are not cleared in the case of Shutdown.
	///
	/// Example usage:
	///    pAddressRepLookup->SetOption(IAddressRepLookup::kOptionSymbolCacheLevel, 1);
	///
	virtual void SetOption(int option, int value) = 0;

	/// SetBaseAddress
	///
	/// Sets the base address of the module.
	/// If the base address is not set, then the GetAddressRep function assumes that
	/// the base address is the base address of the currently executing module.
	/// You should call this function when you are reading symbols for an address
	/// of a process that isn't the one currently running.
	/// This function should be called before Init.
	///
	virtual void SetBaseAddress(uint64_t baseAddress) = 0;

	/// GetBaseAddress
	///
	/// Returns the base address for the module, which is the address set by SetBaseAddress.
	/// If SetBaseAddress was not called then it is the default address for the module or kBaseAddressUnspecified
	/// if the default hasn't been calculated yet.
	virtual uint64_t GetBaseAddress() const = 0;

	/// GetDatabasePath
	///
	/// Returns the path to the database source (usually a disk file) specified
	/// in the Init function.
	virtual const wchar_t* GetDatabasePath() const = 0;


	/// Inherited from the IAddressRepLookupBase base class ///

	///
	/// GetAddressRep
	///
	/// Returns an address representation (e.g. file/line) for a given address.
	///
	virtual int GetAddressRep(int addressRepTypeFlags, uint64_t address, FixedString* pRepArray, int* pIntValueArray) = 0;

};

} // namespace Callstack
} // namespace EA

#endif // EACALLSTACK_IADDRESSREP_H
