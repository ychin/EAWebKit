/////////////////////////////////////////////////////////////////////////////
// Copyright (c) Electronic Arts Inc. All rights reserved.
/////////////////////////////////////////////////////////////////////////////


#ifndef EACALLSTACK_MAPFILE_H
#define EACALLSTACK_MAPFILE_H


#include <EACallstack/internal/Config.h>
#include <EACallstack/internal/EASTLCoreAllocator.h>
#include <EACallstack/EAAddressRep.h>
#include <EASTL/fixed_string.h>
#include <EASTL/list.h>
#include <EASTL/map.h>
#include <EASTL/vector.h>
#include <coreallocator/icoreallocator_interface.h>
#include <EAIO/EAFileStream.h>
#include <EAIO/EAStreamBuffer.h>

#if defined(EA_PRAGMA_ONCE_SUPPORTED)
	#pragma once
#endif // EA_PRAGMA_ONCE_SUPPORTED

EA_DISABLE_VC_WARNING(4251); // class (some template) needs to have dll-interface to be used by clients.


namespace EA
{
namespace Callstack
{
	#if EACALLSTACK_MSVC_MAPFILE_ENABLED

		/// MapFileMSVC
		///
		/// Make sure the application is built with the linker set to generate .map files and
		/// also that the .map files contain line information. These are separate settings in VC++.
		/// Note that in VC++ line number information is not generated in .map files if incremental
		/// linking is enabled, regardless of whether the line number setting is explicitly specified.
		///
		class EACALLSTACK_API MapFileMSVC : public IAddressRepLookup
		{
		public:
			/// kTypeId
			/// Defines what type of IAddressRepLookup this is. This is used in conjunction
			/// with the AsInterface function to safely dynamically cast a base class
			/// such as IAddressRepLookup to a subclass such as MapFileMSVC.
			static const int kTypeId = 0x061b6705; // This is just some unique value.

			MapFileMSVC(EA::Allocator::ICoreAllocator* pCoreAllocator = NULL);
		   ~MapFileMSVC();

			void SetAllocator(EA::Allocator::ICoreAllocator* pCoreAllocator);

			bool Init(const wchar_t* pMapFilePath, bool bDelayLoad = true);
			bool Shutdown();

			void*           AsInterface(int typeId);
			void            SetBaseAddress(uint64_t baseAddress);
			uint64_t        GetBaseAddress() const;
			int             GetAddressRep(int addressRepTypeFlags, uint64_t address, FixedString* pRepArray, int* pIntValueArray);
			const wchar_t*  GetDatabasePath() const;
			void            SetOption(int option, int value);

		protected:
			struct Symbol
			{
				uint32_t    mSegment;
				uint32_t    mOffset;
				CAString8   mSymbol;
				uint64_t    mRVA;        // Relative virtual address. http://www.windowsitlibrary.com/Content/356/11/1.html
			  //CAString8   mSourceObj;  // This is not currently needed.

				Symbol() : mSymbol(EASTLCoreAllocator(EASTL_NAME_VAL(EACALLSTACK_ALLOC_PREFIX "MapFileMSVC"))) {}
			};

			struct Group
			{
				uint32_t mSegment;  //
				uint32_t mOffset;   //
			  //uint64_t mRVA;      // May be useful to calculate and store this.
				uint32_t mLength;   //
			  //char     mClass[5]; // "TEXT", "DATA". It may be useful to store this value.
			};

			enum Phase
			{
				kPhaseIdle,
				kPhaseCapturingGroups,
				kPhaseCapturingSymbols,
				kPhaseCapturingLineNumbers,
				kPhaseCount
			};

			// Used in low memory usage mode, so we can more quickly tell where a symbols is within
			// a file, as opposed to reading every line of the file every time. We don't typically
			// encode every rva to file position, but just every N of them.
			struct RVAToFilePos
			{
				uint64_t mRVA;      // Address.
				uint64_t mFilePos;  // Position in the file where this line begins.
			};

			struct RVAToFilePosCompare
			{
				bool operator()(const RVAToFilePos& a, const RVAToFilePos& b)
					{ return a.mRVA < b.mRVA; }
			};

			bool Load();
			bool FillRVAToFilePosArray(uint64_t nAddress);                 // Used when mbLowMemoryUsage == true.
			bool FindRVAToFilePosArray(uint64_t nAddress, Symbol& symbol); // Used when mbLowMemoryUsage == true.

		protected:
			EA::Allocator::ICoreAllocator* mpCoreAllocator;             //
			bool                           mbLoadAttempted;             // True if we have tried to load the map file.
			bool                           mbLowMemoryUsage;            // True if we read the file iteratively instead of compiling it into our data structures.
			bool                           mbOfflineLookup;             // True if we are to do offline address lookups, instead of lookups for the current app's addresses. Affects how we try to auto-determine module base addresses, which is important for relocatable DLLs on all platforms.
			bool                           mbSymbolEndFound;            // True if during load we have encountered the end of .text symbols. Used for the case of mbLowMemoryUsage = true.
			FixedStringW                   msMapFilePath;               //
			uint64_t                       mBaseAddress;                // The load address of the module. This may be a value which is lower than the lowest symbol in the text segment on some platforms.
			uint64_t                       mMapFileBaseAddress;         // Called "Preferred load address" in the map file.
			uint32_t                       mFileBufferSize;             // Size for file buffering.
			IO::FileStream                 mFile;                       // Used for reading the file. In low memory mode, it is read periodically as we need to see more of the symbol lines.
			IO::StreamBuffer               mFileBuffer;                 // Used for reading the file. In low memory mode, it is read periodically as we need to see more of the symbol lines.
			IO::FileStream                 mFileLowMemoryUsage;         // Used in low memory mode to read lines dynamically, independently of mFile/mFileBuffer.
			IO::StreamBuffer               mFileBufferLowMemoryUsage;   // Used in low memory mode to read lines dynamically, independently of mFile/mFileBuffer.
			uint64_t                       mFileSize;                   // Size of map file.
			Phase                          mPhase;                      //
			uint32_t                       mSymbolsPerLowMemSample;     // When mbLowMemoryUsage is true, we read only every N lines from the .text symbols section. That way we use less memory, but have to read as many as N lines from the file on every lookup instead of just reading our (potentially large) data structures. See mRVAToFilePosArray.

			typedef eastl::map<uint64_t, Symbol, eastl::less<uint64_t>, EA::Callstack::EASTLCoreAllocator> AddressToSymbolMap;
			AddressToSymbolMap mAddressToSymbolMap;

			typedef eastl::list<CAString8, EA::Callstack::EASTLCoreAllocator> LineFilenameList;
			LineFilenameList mLineFilenameList;             // Since VS2005 and later no longer creates line info in map files, this isn't used.

			typedef eastl::map<uint64_t, eastl::pair<CAString8*, int>, eastl::less<uint64_t>, EA::Callstack::EASTLCoreAllocator> AddressToFileLineMap;
			AddressToFileLineMap mAddressToFileLineMap;     // Since VS2005 and later no longer creates line info in map files, this isn't used.

			typedef eastl::vector<uint32_t, EA::Callstack::EASTLCoreAllocator> SectionArray;
			SectionArray mSectionArray;                     // Section to offset, i.e. 0001:00203872 -> 00401000 + 00203872

			typedef eastl::vector<Group, EA::Callstack::EASTLCoreAllocator> GroupArray;
			GroupArray mCodeGroupArray;

			typedef eastl::vector<RVAToFilePos, EA::Callstack::EASTLCoreAllocator> RVAToFilePosArray;
			RVAToFilePosArray mRVAToFilePosArray;           // Used when mbLowMemoryUsage is true.

		}; // MapFileMSVC

	#endif // EACALLSTACK_MSVC_MAPFILE_ENABLED


	#if EACALLSTACK_GCC_MAPFILE_ENABLED

		/// MapFileGCC3
		///
		/// This class supports GCC v3.x and v4.x map files. GCC 2.x and earlier used a different map file format.
		/// You need to link your app with the -Map linker argument in order to generate a GCC .map file.
		/// At least for the PS3 platform, SetBaseAddress will need to be called for PRXs (PS3 DLLs) but not ELFs (main executables).
		///
		class EACALLSTACK_API MapFileGCC3 : public IAddressRepLookup
		{
		public:
			/// kTypeId
			/// Defines what type of IAddressRepLookup this is. This is used in conjunction
			/// with the AsInterface function to safely dynamically cast a base class
			/// such as IAddressRepLookup to a subclass such as MapFileGCC3.
			static const int kTypeId = 0x061b66fe; // This is just some unique value.

			MapFileGCC3(EA::Allocator::ICoreAllocator* pCoreAllocator = NULL);
		   ~MapFileGCC3();

			void SetAllocator(EA::Allocator::ICoreAllocator* pCoreAllocator);

			bool Init(const wchar_t* pMapFilePath, bool bDelayLoad = true);
			bool Shutdown();

			void*           AsInterface(int typeId);
			void            SetBaseAddress(uint64_t baseAddress);
			uint64_t        GetBaseAddress() const;
			int             GetAddressRep(int addressRepTypeFlags, uint64_t address, FixedString* pRepArray, int* pIntValueArray);
			const wchar_t*  GetDatabasePath() const;
			void            SetOption(int option, int value);

		protected:
			struct EntryPair
			{
				EntryPair() : mSymbol(EASTLCoreAllocator(EASTL_NAME_VAL(EACALLSTACK_ALLOC_PREFIX "MapFileGCC3"))) {}

				uint64_t    mAddress; // 64 bits so that we can read .map files from both 32 and 64 bit platforms.
				CAString8 mSymbol;
			};

			typedef eastl::vector<EntryPair, EA::Callstack::EASTLCoreAllocator> EntryPairArray;

			struct EntryPairCompare
			{
				bool operator()(const EntryPair& a, const EntryPair& b)
					{ return a.mAddress < b.mAddress; }
			};

			bool Load();

		protected:
			EA::Allocator::ICoreAllocator* mpCoreAllocator;
			bool                           mbLoadAttempted;  /// True if we have tried to load the map file.
			bool                           mbLowMemoryUsage; /// True if we read the file iteratively instead of compiling it into our data structures.
			bool                           mbOfflineLookup;  /// True if we are to do offline address lookups, instead of lookups for the current app's addresses. Affects how we try to auto-determine module base addresses, which is important for relocatable DLLs on all platforms.
			FixedStringW                   msMapFilePath;
			EntryPairArray                 mEntryPairArray;  /// This holds pointers into mStackAllocator's space.
			uint64_t                       mBaseAddress;     /// The load address of the module.
			uint32_t                       mFileBufferSize;  /// Size for file buffering.

		}; // MapFileGCC3

	#endif // EACALLSTACK_GCC_MAPFILE_ENABLED


	#if EACALLSTACK_APPLE_MAPFILE_ENABLED

		/// MapFileApple
		///
		/// This class supports Apple GCC map files generated for iOS and OS X.
		/// You need to link your app with the -Map linker argument in order to generate these .map files.
		/// XCode has a GUI setting for this.
		///
		class EACALLSTACK_API MapFileApple : public IAddressRepLookup
		{
		public:
			/// kTypeId
			/// Defines what type of IAddressRepLookup this is. This is used in conjunction
			/// with the AsInterface function to safely dynamically cast a base class
			/// such as IAddressRepLookup to a subclass such as MapFileApple.
			static const int kTypeId = 0x061b66ef; // This is just some unique value.

			MapFileApple(EA::Allocator::ICoreAllocator* pCoreAllocator = NULL);
		   ~MapFileApple();

			void SetAllocator(EA::Allocator::ICoreAllocator* pCoreAllocator);

			bool Init(const wchar_t* pMapFilePath, bool bDelayLoad = true);
			bool Shutdown();

			void*           AsInterface(int typeId);
			void            SetBaseAddress(uint64_t baseAddress);
			uint64_t        GetBaseAddress() const;
			int             GetAddressRep(int addressRepTypeFlags, uint64_t address, FixedString* pRepArray, int* pIntValueArray);
			const wchar_t*  GetDatabasePath() const;
			void            SetOption(int option, int value);

		protected:
			struct EntryPair
			{
				EntryPair() : mSymbol(EASTLCoreAllocator(EASTL_NAME_VAL(EACALLSTACK_ALLOC_PREFIX "MapFileApple"))) {}

				uint64_t    mAddress; // 64 bits so that we can read .map files from both 32 and 64 bit platforms.
				CAString8 mSymbol;
			};

			typedef eastl::vector<EntryPair, EA::Callstack::EASTLCoreAllocator> EntryPairArray;

			struct EntryPairCompare
			{
				bool operator()(const EntryPair& a, const EntryPair& b)
					{ return a.mAddress < b.mAddress; }
			};

			bool Load();

		protected:
			EA::Allocator::ICoreAllocator* mpCoreAllocator;
			bool                           mbLoadAttempted;  /// True if we have tried to load the map file.
			bool                           mbLowMemoryUsage; /// True if we read the file iteratively instead of compiling it into our data structures.
			bool                           mbOfflineLookup;  /// True if we are to do offline address lookups, instead of lookups for the current app's addresses. Affects how we try to auto-determine module base addresses, which is important for relocatable DLLs on all platforms.
			FixedStringW                   msMapFilePath;
			EntryPairArray                 mEntryPairArray;  /// This holds pointers into mStackAllocator's space.
			uint64_t                       mBaseAddress;     /// The load address of the module.
			uint32_t                       mFileBufferSize;  /// Size for file buffering.

		}; // MapFileApple

	#endif // EACALLSTACK_APPLE_MAPFILE_ENABLED


	#if EACALLSTACK_SN_MAPFILE_ENABLED

		/// MapFileSN
		///
		/// You need to link your app with the -Map linker argument in order to generate an SN .map file.
		/// At least for the PS3 platform, SetBaseAddress will need to be called for PRXs (PS3 DLLs) but not ELFs (main executables).
		///
		class EACALLSTACK_API MapFileSN : public IAddressRepLookup
		{
		public:
			/// kTypeId
			/// Defines what type of IAddressRepLookup this is. This is used in conjunction
			/// with the AsInterface function to safely dynamically cast a base class
			/// such as IAddressRepLookup to a subclass such as MapFileSN.
			static const int kTypeId = 0x068472b3; // This is just some unique value.

			MapFileSN(EA::Allocator::ICoreAllocator* pCoreAllocator = NULL);
		   ~MapFileSN();

			void SetAllocator(EA::Allocator::ICoreAllocator* pCoreAllocator);

			bool Init(const wchar_t* pMapFilePath, bool bDelayLoad = true);
			bool Shutdown();

			void*           AsInterface(int typeId);
			void            SetBaseAddress(uint64_t baseAddress);
			uint64_t        GetBaseAddress() const;
			int             GetAddressRep(int addressRepTypeFlags, uint64_t address, FixedString* pRepArray, int* pIntValueArray);
			const wchar_t*  GetDatabasePath() const;
			void            SetOption(int option, int value);

		protected:
			struct EntryPair
			{
				EntryPair() : mSymbol(EASTLCoreAllocator(EASTL_NAME_VAL(EACALLSTACK_ALLOC_PREFIX "MapFileSN"))) {}

				uint64_t    mAddress; // 64 bits so that we can read .map files from both 32 and 64 bit platforms.
				CAString8 mSymbol;
			};

			typedef eastl::vector<EntryPair, EA::Callstack::EASTLCoreAllocator> EntryPairArray;

			struct EntryPairCompare
			{
				bool operator()(const EntryPair& a, const EntryPair& b)
					{ return a.mAddress < b.mAddress; }
			};

			// Used in low memory usage mode, so we can more quickly tell where a symbols is within
			// a file, as opposed to reading every line of the file every time. We don't typically
			// encode every address to file position, but just one for every module.
			struct AddressToFilePos
			{
				uint64_t mAddress;  // To consider: store these as uint32_t for 32 bit platforms. We don't store a whole lot of these so this isn't as important as it might first seem.
				uint64_t mFilePos;  // Position in the file where this line begins.
			};

			typedef eastl::vector<AddressToFilePos, EA::Callstack::EASTLCoreAllocator> AddressToFilePosArray;

			struct AddressToFilePosCompare
			{
				bool operator()(const AddressToFilePos& a, const AddressToFilePos& b)
					{ return a.mAddress < b.mAddress; }
			};

			bool Load();
			bool FillAddressToFilePosArray(uint64_t nAddressRelative);                              // Used when mbLowMemoryUsage == true.
			bool FindAddressToFilePosArray(uint64_t nAddressRelative, CAString8& symbol);           // Used when mbLowMemoryUsage == true.

			//                                             "Address  Size     Align Out     In      File    Symbol
			//                                             "=================================================================
			static const size_t kOutMinLength    = 25;  // "00010230 0008f5d8     8 .text"
			static const size_t kInMinLength     = 33;  // "0009f684 0000006c     1         .gnu.linkonce.t.blah"
			static const size_t kFileMinLength   = 41;  // "0009f684 0000006c     1                 C:\Projects\EAOS\UTF\build\EASTL\dev\ps3-sn-dev-debug\lib\libEASTL.a
			static const size_t kSymbolMinLength = 49;  // "000105c0 00000124     0                         EA::UnitTest::Test::Test(char const*, void (*)(char const*))

		protected:
			EA::Allocator::ICoreAllocator* mpCoreAllocator;             //
			bool                           mbLoadAttempted;             // True if we have tried to load the map file.
			bool                           mbLowMemoryUsage;            // True if we read the file iteratively instead of compiling it into our data structures.
			bool                           mbOfflineLookup;             // True if we are to do offline address lookups, instead of lookups for the current app's addresses. Affects how we try to auto-determine module base addresses, which is important for relocatable DLLs on all platforms.
			bool                           mbSymbolEndFound;            // True if during load we have encountered the end of .text symbols. Used for the case of mbLowMemoryUsage = true.
			bool						   mbImageBaseIsZero;			// True if we detect that .interp has address zero... this corresponds to ASLR map file generation
			FixedStringW                   msMapFilePath;               //
			EntryPairArray                 mEntryPairArray;             // This holds pointers into mStackAllocator's space.
			uint64_t                       mBaseAddress;                // The load address of the module.
			uint32_t                       mFileBufferSize;             // Size for file buffering.
			IO::FileStream                 mFile;                       // Used for reading the file. In low memory mode, it is read periodically as we need to see more of the symbol lines.
			IO::StreamBuffer               mFileBuffer;                 // Used for reading the file. In low memory mode, it is read periodically as we need to see more of the symbol lines.
			IO::FileStream                 mFileLowMemoryUsage;         // Used in low memory mode to read lines dynamically, independently of mFile/mFileBuffer.
			IO::StreamBuffer               mFileBufferLowMemoryUsage;   // Used in low memory mode to read lines dynamically, independently of mFile/mFileBuffer.
			AddressToFilePosArray          mAddressToFilePosArray;      // Used when mbLowMemoryUsage is true.
			uint64_t                       mFileSize;                   // Size of map file.

		}; // MapFileSN

	#endif // EACALLSTACK_SN_MAPFILE_ENABLED


} // namespace Callstack
} // namespace EA


EA_RESTORE_VC_WARNING();


#endif // EACALLSTACK_MAPFILE_H
