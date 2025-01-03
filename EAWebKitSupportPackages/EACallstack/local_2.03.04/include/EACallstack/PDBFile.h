/////////////////////////////////////////////////////////////////////////////
// Copyright (c) Electronic Arts Inc. All rights reserved.
/////////////////////////////////////////////////////////////////////////////

#ifndef EACALLSTACK_PDBFILE_H
#define EACALLSTACK_PDBFILE_H


#include <EACallstack/internal/Config.h>

#if defined(EA_PRAGMA_ONCE_SUPPORTED)
	#pragma once
#endif // EA_PRAGMA_ONCE_SUPPORTED

#if EACALLSTACK_PDBFILE_ENABLED

#include <EACallstack/internal/EASTLCoreAllocator.h>
#include <EACallstack/EAAddressRep.h>
#include <EASTL/vector.h>
#include <EASTL/fixed_vector.h>
#include <EASTL/string.h>
#include <EASTL/fixed_string.h>
#include <EAIO/EAFileStream.h>
#include <EAStdC/EAString.h>
#include <coreallocator/icoreallocator_interface.h>


EA_DISABLE_VC_WARNING(4251); // class (some template) needs to have dll-interface to be used by clients.

namespace EA
{

namespace Allocator
{

	class ICoreAllocator;

} // namespace Allocator

namespace Callstack
{

	#if EA_WINAPI_FAMILY_PARTITION(EA_WINAPI_PARTITION_DESKTOP)

		/// PDBFileMS
		///
		/// This class uses the Microsoft Windows DbgHelp facilities to do .pdb file lookups
		/// on Windows platforms. This works with Win32, Win64, and XBox 360 .pdb files.
		/// Microsoft documents the DbgHelp functions as being thread-unsafe, which implies
		/// that PDBFileMS usage is not thread-safe unless the application coordinates usage
		/// of it.
		///
		class EACALLSTACK_API PDBFileMS : public IAddressRepLookup
		{
		public:
			/// kTypeId
			/// Defines what type of IAddressRepLookup this is. This is used in conjunction
			/// with the AsInterface function to safely dynamically cast a base class
			/// such as IAddressRepLookup to a subclass such as PDBFileMS.
			static const int kTypeId = 0x061b65e2; // This is just some unique value.

			PDBFileMS(EA::Allocator::ICoreAllocator* pCoreAllocator = NULL);
		   ~PDBFileMS();

			void SetAllocator(EA::Allocator::ICoreAllocator* pCoreAllocator);

			bool Init(const wchar_t* pPDBFilePath, bool bDelayLoad = true);
			bool Shutdown();

			void*           AsInterface(int typeId);
			void            SetBaseAddress(uint64_t baseAddress);
			uint64_t        GetBaseAddress() const;
			int             GetAddressRep(int addressRepTypeFlags, uint64_t address, FixedString* pRepArray, int* pIntValueArray);
			const wchar_t*  GetDatabasePath() const;
			void            SetOption(int option, int value);

		protected:
			void*         mhSymbols;
			bool          mbOfflineLookup;              /// True if we are to do offline address lookups, instead of lookups for the current app's addresses. Affects how we try to auto-determine module base addresses, which is important for relocatable DLLs on all platforms.
			uint64_t      mBaseAddress;                 /// The load address of the module.
			uint64_t      mSymModuleLoadedBaseAddress;  /// Identifies if we successfully called SymLoadModule64 and what base address was passed to it. UINT64_MAX if not successfully called. If not UINT64_MAX, it should be equal to mBaseAddress.
			FixedStringW  msPDBFilePath;
		};

	#endif


	///////////////////////////////////////////////////////////////////////
	// PDBFileCustom
	///////////////////////////////////////////////////////////////////////

	namespace PDB
	{
		struct Header
		{
			char     mSignature[32];
			uint32_t mBlockSize;
			uint32_t mUnknown1;
			uint32_t mPageCount;
			uint32_t mTOCSize;
			uint32_t mUnknown2;
			uint32_t mTOCPage;
		};

		struct TOC
		{
			uint32_t mFileCount;
			uint32_t mFileSizeArray[1];
		};

		struct Root
		{
			uint32_t mVersion;
			uint32_t mTimeDateStamp;
			uint32_t mAge;
			uint32_t mGuidData1;
			uint16_t mGuidData2;
			uint16_t mGuidData3;
			uint8_t  mGuidData4[8];
			uint32_t mNameCount;    // mNames seems to always be "/names /LinkInfo /src/headerblock"
			char     mNames[256];   // We need to see if this can be larger than 256.
		};

		struct ModuleRecord
		{
			uint32_t mSignature;            //
			uint32_t mVersion;              //
			uint32_t mUnknown;
			uint32_t mHash1File;            //
			uint32_t mHash2File;            //
			uint16_t mGlobalSymbolFile;     //
			uint16_t mUnknown2;
			uint32_t mModuleSize;           //
			uint32_t mOffsetSize;           //
			uint32_t mHashSize;             //
			uint32_t mSourceModuleSize;     //
			uint32_t mPDBImportSize;        //
			uint32_t mReserved[5];
		};

		// This is the same as Microsoft's <winnt.h> IMAGE_SECTION_HEADER struct.
		struct SectionDirectoryEntry
		{
			char     mName[8];              /// An 8-byte, null-padded UTF-8 string. There is no terminating null character if the string is exactly eight characters long. For longer names, this member contains a forward slash (/) followed by an ASCII representation of a decimal number that is an offset into the string table. Executable images do not use a string table and do not support section names longer than eight characters.
			uint32_t mVirtualSize;          /// The total size of the section when loaded into memory, in bytes. If this value is greater than the SizeOfRawData member, the section is filled with zeroes. This field is valid only for executable images and should be set to 0 for object files.
			uint32_t mVirtualAddress;       /// The address of the first byte of the section when loaded into memory, relative to the image base. For object files, this is the address of the first byte before relocation is applied.
			uint32_t mRawDataSize;          /// The size of the initialized data on disk, in bytes. This value must be a multiple of the FileAlignment member of the IMAGE_OPTIONAL_HEADER structure. If this value is less than the VirtualSize member, the remainder of the section is filled with zeroes. If the section contains only uninitialized data, the member is zero.
			uint32_t mRawDataPointer;       /// A file pointer to the first page within the COFF file. This value must be a multiple of the FileAlignment member of the IMAGE_OPTIONAL_HEADER structure. If a section contains only uninitialized data, set this member is zero.
			uint32_t mRelocationsPointer;   /// A file pointer to the beginning of the relocation entries for the section. If there are no relocations, this value is zero.
			uint32_t mLineNumbersPointer;   /// A file pointer to the beginning of the line-number entries for the section. If there are no COFF line numbers, this value is zero.
			uint16_t mRelocationsCount;     /// The number of relocation entries for the section. This value is zero for executable images.
			uint16_t mLineNumbersCount;     /// The number of line-number entries for the section.
			uint32_t mCharacteristics;      /// The characteristics (as flags) of the image. See enum SectionChararacteristics.
		};

	} // namespace PDB


	/// PDBFileCustom
	///
	class EACALLSTACK_API PDBFileCustom : public IAddressRepLookup
	{
	public:
		/// kTypeId
		/// Defines what type of IAddressRepLookup this is. This is used in conjunction
		/// with the AsInterface function to safely dynamically cast a base class
		/// such as IAddressRepLookup to a subclass such as PDBFileCustom.
		static const int kTypeId = 0x061b6714; // This is just some unique value.

		PDBFileCustom(EA::Allocator::ICoreAllocator* pCoreAllocator = NULL);
	   ~PDBFileCustom();

		void SetAllocator(EA::Allocator::ICoreAllocator* pCoreAllocator);

		bool Init(const wchar_t* pPDBFilePath, bool bDelayLoad = true);
		bool Shutdown();

		void*           AsInterface(int typeId);
		void            SetBaseAddress(uint64_t baseAddress);
		uint64_t        GetBaseAddress() const;
		int             GetAddressRep(int addressRepTypeFlags, uint64_t address, FixedString* pRepArray, int* pIntValueArray);
		const wchar_t*  GetDatabasePath() const;
		void            SetOption(int option, int value);

	protected:
		struct SymbolInfo
		{
			uint32_t mAddressOffset; // mBaseAddress + mAddressOffset => RAM address of symbol. Even on 64 bit platforms, addresses are 32 bits, as they are offset at runtime by a 64 bit value.
			CAString8    mSymbolText;

			SymbolInfo(uint32_t addr, const CAString8& str) : mAddressOffset(addr), mSymbolText(str) {}
			SymbolInfo() : mSymbolText(EASTLCoreAllocator(EASTL_NAME_VAL(EACALLSTACK_ALLOC_PREFIX "PDBFileCustom"))) {}
		};

		friend struct SortByOffset;

		struct LookupRequest
		{
			int                 mAddressRepTypeFlags;   //
			const uint64_t*     mpAddressArray;         // Specified as an array because we may want to do multiple lookups in parallel.
			uint32_t            mAddressArrayCount;     // Size of these arrays. It is usually one.
			FixedString*        mpFunctionArray;        // Function name (from kARTFunctionOffset).
			int*                mpOffsetArray;          // Function offset (from kARTFunctionOffset).
			int                 mResultCount;           // Number of AddressRepTypeFlags that could be honored.

			LookupRequest();
		};

		bool LoadSymbolInfo();
		bool ReadPDBFile(void* pDestination, uint32_t destinationCapacity, uint32_t fileSize, uint32_t blockIndexArrayOffset);
		bool IterateSymbols();

		typedef eastl::vector<uint32_t,   EA::Callstack::EASTLCoreAllocator> BlockIndexArray;
		typedef eastl::vector<SymbolInfo, EA::Callstack::EASTLCoreAllocator> SymbolInfoArray;

	protected:
		EA::Allocator::ICoreAllocator* mpCoreAllocator;
		FixedStringW                   msPDBFilePath;
		IO::FileStream                 mFile;
		uint32_t                       mFileSize;
		bool                           mbOfflineLookup;     /// True if we are to do offline address lookups, instead of lookups for the current app's addresses. Affects how we try to auto-determine module base addresses, which is important for relocatable DLLs on all platforms.
		uint64_t                       mBaseAddress;        /// The load address of the module.
		PDB::Header                    mHeader;
		PDB::TOC*                      mpTOC;
		PDB::Root                      mRoot;
		PDB::ModuleRecord              mModuleRecord;
		PDB::SectionDirectoryEntry     mTextSection;
		uint32_t                       mGlobalSymbolFileIndex;
		uint32_t                       mGlobalSymbolFileSize;
		BlockIndexArray                mGlobalSymbolFileBlockIndexArray;    // This is only needed if mSymbolInfoArray is empty, as it provides the location of the SymbolInfoArray data.
		bool                           mbBuildSymbolInfoArray;
		SymbolInfoArray                mSymbolInfoArray;
		LookupRequest                  mLookupRequest;
	};


	#if EACALLSTACK_MS_PDB_READER_ENABLED
		typedef PDBFileMS PDBFile;
	#else
		typedef PDBFileCustom PDBFile;
	#endif


} // namespace Callstack
} // namespace EA


EA_RESTORE_VC_WARNING();

#endif // EACALLSTACK_PDBFILE_ENABLED

#endif // EACALLSTACK_PDBFILE_H
