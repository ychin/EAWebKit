///////////////////////////////////////////////////////////////////////////////
// DWARF3File.cpp
//
// Copyright (c) 2006 Electronic Arts Inc.
// Created by Paul Schlegel, developed and maintained by Paul Pedriana.
//
// This a DWARF3 reader designed for gleaning basic symbol information such
// as the function name, line number, and source file information from the
// DWARF3 symbol data stored in an ELF file.
//
// The canonical DWARF3 format specification can be found here:
//     http://dwarf.freestandards.org/
// Recommended reading:
//     Introduction to the DWARF Debugging Format
//     by Michael J. Eager, Eager Consulting
//     http://www.dwarfstd.org/Debugging%20using%20DWARF.pdf
//
///////////////////////////////////////////////////////////////////////////////


#include <EACallstack/DWARF3File.h>

#if EACALLSTACK_DWARFFILE_ENABLED

#include <EACallstack/EAAddressRep.h>
#include <EACallstack/EASymbolUtil.h>
#include <EACallstack/Allocator.h>
#include <EAIO/EAStreamAdapter.h>
#include <EAIO/EAFileUtil.h>
#include <EAIO/FnEncode.h>
#include <EAStdC/EASprintf.h>
#include <EAStdC/EAString.h>


#if defined(EA_PLATFORM_WINDOWS) && EACALLSTACK_ENABLE_ELF_SPAWN
	#include <EACallstack/internal/CabReader.h>
	#include <EACallstack/internal/Win32Runner.h>
	#include <EACallstack/internal/addr2line.exe.inl>
	#include <EAIO/EAFileStream.h>
	#include <EAIO/EAStreamBuffer.h>
	#include <EAIO/EAStreamChild.h>
	#include <EAIO/PathString.h>
#endif

namespace EA
{
namespace Callstack
{

namespace Local
{
	#if defined(EA_PLATFORM_WINDOWS) && EACALLSTACK_ENABLE_ELF_SPAWN
		using namespace EA::IO;

		// Avery Lee's suggestions for fast file copies:
		// On Windows XP, Explorer memory maps the source and does a WriteFile() off of that. 
		// Vista and up will sometimes use FILE_FLAG_NO_BUFFERING instead.
		// If you are copying from different physical disks, then buffered WriteFile() is 
		// probably about as fast as you can get. You want to open in FILE_FLAG_SEQUENTIAL_SCAN 
		// mode and copy at least 64K at a time so that you get full 192K of read-ahead. 
		// If you're copying within the same disk, then FILE_FLAG_NO_BUFFERING is faster.
		// You should try preallocating the destination. Changing the end-of-file pointer is 
		// a serializing operation in NTFS.

		// This is the same as the CopyStream function in EAStreamAdapter.h/cpp except that
		// it uses a larger buffer and it uses the *remaining* source size as opposed to the entire source size.
		size_type CopyStream(IStream* pSource, EA::IO::IStream* pDestination, size_type nSize)
		{
			const size_t    kBufferSize = 262144;
			char*           buffer = new char[kBufferSize];
			size_type       nCurrentCount, nRemaining;
			const size_type nSourceSize = pSource->GetAvailable();

			if(nSourceSize == kSizeTypeError) // If the source size is of undetermined size...
				nSize = kLengthNull;          // Read all of the source.
			else if(nSize > nSourceSize)      // If the user size is too high...
				nSize = nSourceSize;          // Reduce the user size to be the source size.

			for(nRemaining = nSize; nRemaining != 0; nRemaining -= nCurrentCount)
			{
				nCurrentCount = ((nRemaining >= kBufferSize) ? kBufferSize : nRemaining);
				nCurrentCount = pSource->Read(buffer, nCurrentCount);

				if((nCurrentCount == kSizeTypeError) || !pDestination->Write(buffer, nCurrentCount))
					return kSizeTypeError;

				if(nCurrentCount == 0) // If we have read the entire source...
					break;
			}

			delete[] buffer;
			return (nSize - nRemaining); // Return the number of bytes copied. Note that nRemaining might be non-zero.
		}
	#endif

	/*
	size_type CopyFile(const char* pSourcePath, const char* pDestPath)
	{
		FILE* pSource = fopen(pSourcePath, "rb");
		FILE* pDest   = fopen(pDestPath, "wb");

		char            buffer[16384];
		size_type       nCurrentCount, nRemaining;
		size_type       nSize = 175811972;

		for(nRemaining = nSize; nRemaining != 0; nRemaining -= nCurrentCount)
		{
			nCurrentCount = ((nRemaining >= sizeof(buffer)) ? sizeof(buffer) : nRemaining);
			nCurrentCount = fread(buffer, 1, nCurrentCount, pSource); // pSource->Read(buffer, nCurrentCount);

			fwrite(buffer, 1, nCurrentCount, pDest);

			if(nCurrentCount == 0) // If we have read the entire source...
				break;
		}

		fclose(pSource);
		fclose(pDest);

		return (nSize - nRemaining); // Return the number of bytes copied. Note that nRemaining might be non-zero.
	}
	*/

} // namespace Local


///////////////////////////////////////////////////////////////////////////////
// DWARF3File
//
DWARF3File::DWARF3File(EA::Allocator::ICoreAllocator* pCoreAllocator)
  : mpCoreAllocator(pCoreAllocator ? pCoreAllocator : EA::Callstack::GetAllocator()),
	msDwarfFilePath(),
	mFileStream(),
	mMemoryStream(),
	mpStream(NULL),
	mELFFile(),
	mELFOffset(0),
	mEndian(IO::kEndianBig),
	mbInitialized(false),
	mbUseMemoryFile(false),
	mbEnableCache(true),
	mbOfflineLookup(false),
	mnDebugArangesStart(0),
	mnDebugArangesEnd(0),
	mnDebugInfoStart(0),
	mnDebugAbbrevStart(0),
	mnDebugAbbrevEnd(0),
	mnDebugLineStart(0),
	mnDebugStrStart(0),
	mbRelocated(false),
	mBaseAddress(kBaseAddressUnspecified),
	mTextSegmentSize(0),
	mCompUnitReader(),
	mCompUnitInfo()
{
	// AddRef these members so they are reference counted away.
	mFileStream.AddRef();
	mMemoryStream.AddRef();
}


///////////////////////////////////////////////////////////////////////////////
// ~DWARF3File
//
DWARF3File::~DWARF3File()
{
	Shutdown();
}

// simple local helper function
static bool ReadString(EA::IO::IStream* pStream, FixedString* stringOut)
{
	uint8_t nTemp8;
	do
	{
		if(!IO::ReadUint8(pStream, nTemp8))
			return false;
		if(nTemp8)
			*stringOut += (int8_t)nTemp8; // Append to the string.
	}
	while(nTemp8);

	return true;
}

///////////////////////////////////////////////////////////////////////////////
// SetAllocator
//
void DWARF3File::SetAllocator(EA::Allocator::ICoreAllocator* pCoreAllocator)
{
	mpCoreAllocator = pCoreAllocator;
	// If there are any members that need to see this, tell them here.
}


///////////////////////////////////////////////////////////////////////////////
// Init
//
bool DWARF3File::Init(const wchar_t* pDWARF3FilePath, bool /*bDelayLoad*/)
{
	if(!mbInitialized)
	{
		if(pDWARF3FilePath)
		{
			msDwarfFilePath = pDWARF3FilePath;
			mFileStream.SetPath(pDWARF3FilePath);

			if(mFileStream.Open(IO::kAccessFlagRead, IO::kCDOpenExisting, IO::FileStream::kShareRead))
			{
				mpStream = &mFileStream;

				if(mbUseMemoryFile)
				{
					const EA::IO::size_type size = mFileStream.GetSize();

					if(mMemoryStream.SetData(NULL, size, false, true, mpCoreAllocator)) // This allocates the appropriate space.
					{
						const EA::IO::size_type copySize = EA::IO::CopyStream(&mFileStream, &mMemoryStream, size);

						mFileStream.Close(); // One way or another, we won't be needing mFileStream any more.

						if(copySize == size)
							mpStream = &mMemoryStream;
						else
							mpStream = NULL;
					}
				}

				if(mELFFile.Init(mpStream))
				{
					mELFOffset = mELFFile.mELFOffset;
					mEndian    = mELFFile.mEndian;

					// Gather the position information of the DWARF3 data from the ELF
					// section headers that DWARF3 defines.
					ELF::ELF64_Shdr textShdr;
					ELF::ELF64_Shdr debugArangesShdr;
					ELF::ELF64_Shdr debugInfoShdr;
					ELF::ELF64_Shdr debugAbbrevShdr;
					ELF::ELF64_Shdr debugLineShdr;
					ELF::ELF64_Shdr debugStrShdr;

					if(mELFFile.FindSectionHeaderByName(".text", &textShdr))
					{
						if(    textShdr.sh_addr        == 0x0		// the .text segment reports as starting 0x0 when the image is relocatable -  http://www.skyfree.org/linux/references/ELF_Format.pdf
							|| mELFFile.mHeader.e_type == 0xfe10	// PS4's ASLR OS-specific type flag ET_SCE_DYNEXEC -  https://ps4.siedev.net/support/issue/129510/_.text_section_s_sh_addr_address_is_non-zero_for_relocated_section#n1708837
							|| mELFFile.mHeader.e_type == 0xfe18)	// PS4/PS5 OS-specific flag used for PRX files (determined using "prospero-readelf.exe -h" on a prx file)
						{
							mbRelocated = true;

							//mBaseAddress is assumed to be passed in as the relocated address
							if(mBaseAddress == kBaseAddressUnspecified)
							{
							#if defined(EA_PLATFORM_WINDOWS) && EACALLSTACK_ENABLE_ELF_SPAWN
								// if we are going to use an offline tool then reset the base address
								// and continue with initialization
								mBaseAddress = 0;
							#endif
							}
						}
						else
						{
							mbRelocated      = false;
							mBaseAddress     = textShdr.sh_addr;
						}

						mTextSegmentSize = textShdr.sh_size;
					}



					if(mELFFile.FindSectionHeaderByName(".debug_aranges", &debugArangesShdr) &&
					   mELFFile.FindSectionHeaderByName(".debug_info",    &debugInfoShdr) &&
					   mELFFile.FindSectionHeaderByName(".debug_abbrev",  &debugAbbrevShdr) &&
					   mELFFile.FindSectionHeaderByName(".debug_line",    &debugLineShdr) &&
					   mELFFile.FindSectionHeaderByName(".debug_str",     &debugStrShdr))
					{
						// the .debug_ranges section is only required if the Codens in the compiled units refer to DW_AT_ranges
							


						mnDebugArangesStart = (int)debugArangesShdr.sh_offset;
						mnDebugArangesEnd   = mnDebugArangesStart + (int)debugArangesShdr.sh_size;
						mnDebugInfoStart    = (int)debugInfoShdr.sh_offset;
						mnDebugAbbrevStart  = (int)debugAbbrevShdr.sh_offset;
						mnDebugAbbrevEnd    = mnDebugAbbrevStart + (int)debugAbbrevShdr.sh_size;
						mnDebugLineStart    = (int)debugLineShdr.sh_offset;
						mnDebugStrStart     = (int)debugStrShdr.sh_offset;

						mCompUnitReader.Init(this);

						mbInitialized = true;
					}
				}
			}
		}
	}

	#if defined(EA_PLATFORM_WINDOWS) && EACALLSTACK_ENABLE_ELF_SPAWN
		// To consider: Do the file spawn here instead of in the GetAddressRepSpawn function.
		// Unilaterally set mbInitialized to true, as the code above doesn't really matter.
		mbInitialized = true;
	#endif

	if(mbInitialized)
	{
		// Actually, mBaseAddress should always have been set above during the map file reading.
		if(((mBaseAddress == kBaseAddressUnspecified) || (mBaseAddress == 0)) && !mbOfflineLookup) // If the base address hasn't been set or was set to 0 above...
		{
			char pPath8[IO::kMaxPathLength];
			EA::StdC::Strlcpy(pPath8, pDWARF3FilePath, IO::kMaxPathLength);

			mBaseAddress = Callstack::GetModuleBaseAddress(pPath8);
			if(mBaseAddress == kBaseAddressInvalid)
				mBaseAddress = kBaseAddressUnspecified;
		}
	}

	return mbInitialized;
}


///////////////////////////////////////////////////////////////////////////////
// Shutdown
//
bool DWARF3File::Shutdown()
{
	if(mbInitialized)
	{
		mFileStream.Close();
		mMemoryStream.Close();

		mbInitialized = false;
	}

	return true;
}


///////////////////////////////////////////////////////////////////////////////
// AsInterface
//
void* DWARF3File::AsInterface(int id)
{
	if(id == DWARF3File::kTypeId)
		return static_cast<DWARF3File*>(this); // The cast isn't necessary, but makes the intent more clear.
	return NULL;
}


///////////////////////////////////////////////////////////////////////////////
// SetBaseAddress
//
void DWARF3File::SetBaseAddress(uint64_t baseAddress)
{
	mBaseAddress = baseAddress;
}


///////////////////////////////////////////////////////////////////////////////
// GetBaseAddress
//
uint64_t DWARF3File::GetBaseAddress() const
{
	return mBaseAddress;
}


///////////////////////////////////////////////////////////////////////////////
// GetDatabasePath
//
const wchar_t* DWARF3File::GetDatabasePath() const
{
	return msDwarfFilePath.c_str();
}


///////////////////////////////////////////////////////////////////////////////
// SetOption
//
void DWARF3File::SetOption(int option, int value)
{
	if(option == kOptionCopyFileToMemory)
		mbUseMemoryFile = (value != 0);
	else if(option == kOptionSymbolCacheLevel)
		mbEnableCache = (value != 0);
	else if(option == kOptionOfflineLookup)
		mbOfflineLookup = (value != 0);
}


///////////////////////////////////////////////////////////////////////////////
// GetAddressRep
//
int DWARF3File::GetAddressRep(int addressRepTypeFlags, uint64_t nAddress, FixedString* pRepArray, int* pIntValueArray)
{
	int returnFlags = 0;

	if(mbInitialized)
	{
		const uint64_t baseAddressToUse = ((mBaseAddress == kBaseAddressUnspecified) ? 0 : mBaseAddress);
		
		// mTextSegmentSize should have been set in our Init function. 
		// During debugging of ARM Android .so ELFs I (Paul Pedriana) found that there is something
		// wrong with the ELF reader that is tripped up by this kind of ELF but not the ones we've
		// worked with in the past. I need to look into this as part of our ELF/DWARF reader rework.
		if((nAddress < baseAddressToUse) || (nAddress >= (baseAddressToUse + mTextSegmentSize)))
		{
			#if defined(EA_PLATFORM_WINDOWS) && EACALLSTACK_ENABLE_ELF_SPAWN
				return GetAddressRepSpawn(addressRepTypeFlags, nAddress, pRepArray, pIntValueArray);
			#else
				return 0;
			#endif
		}

		// We don't want to adjust the address downward unless the module is relocatable (i.e. .dll, .so, PS3 .prx).
		// The reason is that nonrelocatable ELFs have the .text segment base address already accounted for in 
		// their map file information. 
		if(mbRelocated)
			nAddress -= baseAddressToUse;

		#if defined(EA_PLATFORM_WINDOWS) && EACALLSTACK_ENABLE_ELF_SPAWN
			returnFlags = GetAddressRepSpawn(addressRepTypeFlags, nAddress, pRepArray, pIntValueArray);
		#else
			AbbrevSectionEntry abbrevSectionEntry;


			if(FindAbbrevSectionEntry(nAddress, &abbrevSectionEntry)) // This used to be here but it doesn't seem to be correct: (&& abbrevSectionEntry.mnStmtList)
			{
				if(addressRepTypeFlags & (1 << kARTFileLine))
				{
					if(abbrevSectionEntry.mnAttributes & ATTR_STMT_LIST) // If we have the info for file/line translation.
					{
						LineSectionReader lineSectionReader;

						if(lineSectionReader.Init(this, &abbrevSectionEntry))
						{
							FileLineInfo fileLineInfo;

							if(lineSectionReader.FindFileLineInfoForAddress(nAddress, &fileLineInfo))
							{
								pRepArray[kARTFileLine].clear();
								pIntValueArray[kARTFileLine] = (int)fileLineInfo.mnLine;

								if(GetSourceFilePath(&fileLineInfo, &pRepArray[kARTFileLine]))
									returnFlags |= (1 << kARTFileLine);
							}
						}
					}
				}

				// To consider: Ignore function/offset if file/line was present.
				if(addressRepTypeFlags & (1 << kARTFunctionOffset))
				{
					pRepArray[kARTFunctionOffset].clear();
					pIntValueArray[kARTFunctionOffset] = (int)(unsigned)(nAddress - abbrevSectionEntry.mnLowPC);
					static const bool bResolveNamespace = true;
					if(GetFunctionName(&abbrevSectionEntry, &pRepArray[kARTFunctionOffset], bResolveNamespace))
					{
						char   buffer[256];
						size_t nStrlen = UnmangleSymbol(pRepArray[kARTFunctionOffset].c_str(), buffer, sizeof(buffer), kCompilerTypeGCC);

						if(nStrlen)
							pRepArray[kARTFunctionOffset].assign(buffer, (eastl_size_t)nStrlen);

						returnFlags |= (1 << kARTFunctionOffset);
					}
				}
			}
		#endif
	}

	return returnFlags;
}

///////////////////////////////////////////////////////////////////////////////
// GetFunctionName
//
bool DWARF3File::GetFunctionName(const AbbrevSectionEntry* pAbbrevSectionEntry, FixedString* pFunctionNameOut, bool prependNamespace)
{
	// The problem with this is that it just reads the name and not the parent name(s).
	// So if you want function names to be qualified with their parent classes and namespace scopes,
	// you need to do DIE tree lookups for them as well, though you can typically do that while
	// looking for the function name. 
	
	// This was resolved later by doing a post DIE gather to find the actual subprogram AbbrevSection 
	// with the correct namespace. (

	if(prependNamespace)
	{
		// in the case where we only have a specification (not the real subprogram) we need to regather the DIE by a linear search
		if( (pAbbrevSectionEntry->mnAttributes & ATTR_SPECIFICATION) && GatherDIEForAbbrevSection(pAbbrevSectionEntry))
		{
			for(auto iter =  mDIEStack.begin() ; iter != mDIEStack.end(); iter++)
			{
				if(iter->mTag == DW_TAG_namespace || iter->mTag == DW_TAG_class_type) 
				{
					//FixedString tempString;
					const IO::off_type pos = (IO::off_type)(mELFOffset + iter->mnNameOffset);

					if(mpStream->SetPosition(pos))
					{
						if(!ReadString(mpStream, pFunctionNameOut))
							return false;
					}

					const char kNameSpaceSeperator[] = "::";
					*pFunctionNameOut += kNameSpaceSeperator;
				}
			}
		}
	}

	if(pAbbrevSectionEntry->HasAnyAttribute(ATTR_NAME))
	{
		const IO::off_type pos = (IO::off_type)(mELFOffset + pAbbrevSectionEntry->mnNameOffset);

		if(mpStream->SetPosition(pos))
		{
			if(!ReadString(mpStream, pFunctionNameOut))
				return false;

			return true;
		}
	}

	return false;
}


///////////////////////////////////////////////////////////////////////////////
// GetSourceFilePath
//
bool DWARF3File::GetSourceFilePath(const FileLineInfo* pFileLineInfo, FixedString* pSourceFilePathOut)
{
	// Probably should use EAFilePath functions instead...
	pSourceFilePathOut->assign(pFileLineInfo->msSourceFileDirectory);
	pSourceFilePathOut->append("/");
	pSourceFilePathOut->append(pFileLineInfo->msSourceFileName);
	return true;
}

 //#define DWARF3_DEBUG
// #define DWARF3_DEBUG_2

#if defined(DWARF3_DEBUG_2)
	void TraceAbbrevSectionEntry(const AbbrevSectionEntry* pAbbrevSectionEntry)
	{
		const char* pTag = "Unknown";
		#define XYZ(t) case t: pTag = #t; break;

		switch(pAbbrevSectionEntry->mnTag)
		{
			XYZ(DW_TAG_padding   )           
			XYZ(DW_TAG_array_type  )             
			XYZ(DW_TAG_class_type  )             
			XYZ(DW_TAG_entry_point )             
			XYZ(DW_TAG_enumeration_type   )      
			XYZ(DW_TAG_formal_parameter     )    
			XYZ(DW_TAG_imported_declaration   )  
			XYZ(DW_TAG_label           )         
			XYZ(DW_TAG_lexical_block   )         
			XYZ(DW_TAG_member          )         
			XYZ(DW_TAG_pointer_type    )         
			XYZ(DW_TAG_reference_type   )        
			XYZ(DW_TAG_compile_unit     )        
			XYZ(DW_TAG_string_type     )         
			XYZ(DW_TAG_structure_type   )        
			XYZ(DW_TAG_subroutine_type  )        
			XYZ(DW_TAG_typedef        )          
			XYZ(DW_TAG_union_type    )           
			XYZ(DW_TAG_unspecified_parameters   )
			XYZ(DW_TAG_variant           )       
			XYZ(DW_TAG_common_block      )       
			XYZ(DW_TAG_common_inclusion    )     
			XYZ(DW_TAG_inheritance        )      
			XYZ(DW_TAG_inlined_subroutine   )    
			XYZ(DW_TAG_module               )    
			XYZ(DW_TAG_ptr_to_member_type   )    
			XYZ(DW_TAG_set_type          )       
			XYZ(DW_TAG_subrange_type      )      
			XYZ(DW_TAG_with_stmt         )       
			XYZ(DW_TAG_access_declaration  )     
			XYZ(DW_TAG_base_type        )        
			XYZ(DW_TAG_catch_block    )          
			XYZ(DW_TAG_const_type   )            
			XYZ(DW_TAG_constant     )            
			XYZ(DW_TAG_enumerator  )             
			XYZ(DW_TAG_file_type    )            
			XYZ(DW_TAG_friend      )             
			XYZ(DW_TAG_namelist     )            
			XYZ(DW_TAG_namelist_item )           
			XYZ(DW_TAG_packed_type  )            
			XYZ(DW_TAG_subprogram               )
			XYZ(DW_TAG_template_type_parameter )
			XYZ(DW_TAG_template_value_parameter )
			XYZ(DW_TAG_thrown_type             ) 
			XYZ(DW_TAG_try_block       )         
			XYZ(DW_TAG_variant_part    )         
			XYZ(DW_TAG_variable        )         
			XYZ(DW_TAG_volatile_type  )          
			XYZ(DW_TAG_dwarf_procedure )         
			XYZ(DW_TAG_restrict_type   )         
			XYZ(DW_TAG_interface_type  )         
			XYZ(DW_TAG_namespace      )          
			XYZ(DW_TAG_imported_module  )        
			XYZ(DW_TAG_unspecified_type   )      
			XYZ(DW_TAG_partial_unit )            
			XYZ(DW_TAG_imported_unit)            
			XYZ(DW_TAG_condition)                
			XYZ(DW_TAG_shared_type)              
			XYZ(DW_TAG_lo_user)                  
			XYZ(DW_TAG_hi_user)                  
			default:
				break;
		}

		printf("%s\n", pTag);
	}
#endif


bool DWARF3File::GatherDIEForAbbrevSection(const AbbrevSectionEntry* pAbbrevSectionEntry)
{
	if(!(pAbbrevSectionEntry->mnAttributes & ATTR_SPECIFICATION)) 
		return false;

	// The reference given by DW_AT_SPECIFICATION is an offset within the current 
	// compilation unit. But mCompUnitInfo's buffer doesn't include its first 11 
	// bytes, and pAbbrevSectionEntryOut->mnSpecification has mnPositionInInfoSection
	// added to it. So we undo these offsets by subtracting them away.
	IO::off_type infoSectionReaderOffsetOfSpecification = (IO::off_type)(int64_t)((pAbbrevSectionEntry->mnSpecification - mCompUnitInfo.mnPositionInInfoSection) - 11);

	
	InfoSectionReader infoSectionReader;
	if(!infoSectionReader.Init(this))
		return false;


	AbbrevSectionReader abbrevSectionReader;
	if(!abbrevSectionReader.Init(this, &infoSectionReader, mbEnableCache, mpCoreAllocator))
		return false;

	int bFound = 0; // Defined as int instead of bool so we can do certain kinds of debugging.

	mDIEStack.clear();

	// Look up each abbreviation entry for the compilation unit, examining each
	// for attribute types that we are interested in and accumulating them.
	const IO::off_type size = (IO::off_type)infoSectionReader.GetSize();

	AbbrevSectionEntry tempSection;
	while(!bFound && (infoSectionReader.GetPosition() < size))
	{
		uint64_t nAbbrevCode; // As far as I (Paul Pedriana) know, abbreviation codes have no predefined meaning. They are just ids assigned by the compiler/linker.
		bool isLocationOfSpecification =  infoSectionReaderOffsetOfSpecification == infoSectionReader.GetPosition();

		// Get the next abbreviation code.
		// According to 7.5.3, a zero ("null") tag signifies the end of a sibling chain.
		if(!infoSectionReader.ReadAbbrevCode(&nAbbrevCode))
			break;

		if(nAbbrevCode)
		{
			// Find the abbreviation in the ".debug_abbrev" section.
			tempSection.Clear();

			if(!abbrevSectionReader.FindEntryByCode(nAbbrevCode, &tempSection))
				break;

			// perhaps we should be able to find or check for the correct section better than the conditional code below ?
			if(tempSection.mnAbbrevCode == pAbbrevSectionEntry->mnAbbrevCode && isLocationOfSpecification)
				return true;

			if(tempSection.mbHasChildren)
			{
				mDIEStack.push_back(DIE(tempSection.mnTag, tempSection.mnNameOffset, 
										tempSection.mnLowPC, tempSection.mnHighPC));
			}
		}
		else // Else this is the end of a sibling chain.
		{
			if(!mDIEStack.empty())
				mDIEStack.pop_back();
		}
	}

	return false;
}

///////////////////////////////////////////////////////////////////////////////
// FindAbbrevSectionEntry
//
bool DWARF3File::FindAbbrevSectionEntry(uint64_t address, AbbrevSectionEntry* pAbbrevSectionEntryOut)
{
	// Find the compilation unit containing the target address.
	if(!mCompUnitReader.FindUnitContainingAddress(address, &mCompUnitInfo))
		return false;

	InfoSectionReader infoSectionReader;
	if(!infoSectionReader.Init(this))
		return false;

	AbbrevSectionReader abbrevSectionReader;
	if(!abbrevSectionReader.Init(this, &infoSectionReader, mbEnableCache, mpCoreAllocator))
		return false;

	int bFound = 0; // Defined as int instead of bool so we can do certain kinds of debugging.

	#ifdef EA_DEBUG
		mDIEStack.clear();
	#endif

	// Look up each abbreviation entry for the compilation unit, examining each
	// for attribute types that we are interested in and accumulating them.
	const IO::off_type size = (IO::off_type)infoSectionReader.GetSize();

	while(!bFound && (infoSectionReader.GetPosition() < size))
	{
		uint64_t nAbbrevCode; // As far as I (Paul Pedriana) know, abbreviation codes have no predefined meaning. They are just ids assigned by the compiler/linker.

		// Get the next abbreviation code.
		// According to 7.5.3, a zero ("null") tag signifies the end of a sibling chain.
		if(!infoSectionReader.ReadAbbrevCode(&nAbbrevCode))
			break;

		if(nAbbrevCode)
		{
			// Find the abbreviation in the ".debug_abbrev" section.
			pAbbrevSectionEntryOut->Clear();

			if(!abbrevSectionReader.FindEntryByCode(nAbbrevCode, pAbbrevSectionEntryOut))
				break;

			#if defined(DWARF3_DEBUG_2)
				 TraceAbbrevSectionEntry(pAbbrevSectionEntryOut);
			#endif
			

			// Check if the target address has been reached.
			if(pAbbrevSectionEntryOut->ContainsAddress(address))
			{
				++bFound;

				// We expect that the item either has a name or it is a reference to a specification which itself has a name.
				// Question: What do we do if it has both a name and a specification? It would seem that the name would be enough.

				if(!(pAbbrevSectionEntryOut->mnAttributes & ATTR_NAME) && // If the name is missing, but there is a specification...
					(pAbbrevSectionEntryOut->mnAttributes & ATTR_SPECIFICATION))
				{
					// Check out http://article.gmane.org/gmane.comp.monitoring.frysk.general/584 for 
					// a diagram of somebody trying to solve the same problem we are here.

					// The reference given by DW_AT_SPECIFICATION is an offset within the current 
					// compilation unit. But mCompUnitInfo's buffer doesn't include its first 11 
					// bytes, and pAbbrevSectionEntryOut->mnSpecification has mnPositionInInfoSection
					// added to it. So we undo these offsets by subtracting them away.
					IO::off_type pos = (IO::off_type)(int64_t)((pAbbrevSectionEntryOut->mnSpecification - mCompUnitInfo.mnPositionInInfoSection) - 11);

					infoSectionReader.SetPosition(pos);
					infoSectionReader.ReadAbbrevCode(&nAbbrevCode);
					abbrevSectionReader.FindEntryByCode(nAbbrevCode, pAbbrevSectionEntryOut);
				}
			}

			#ifdef EA_DEBUG
				if(pAbbrevSectionEntryOut->mbHasChildren)
					mDIEStack.push_back(DIE(pAbbrevSectionEntryOut->mnTag, pAbbrevSectionEntryOut->mnNameOffset, 
											pAbbrevSectionEntryOut->mnLowPC, pAbbrevSectionEntryOut->mnHighPC));
			#endif
		}
		else // Else this is the end of a sibling chain.
		{
			#ifdef EA_DEBUG
				if(!mDIEStack.empty())
					mDIEStack.pop_back();
			#endif
		}
	}

	return (bFound != 0);
}


///////////////////////////////////////////////////////////////////////////////
// GetAddressRepSpawn
//
// It is expected that the input nAddress has already been adjusted by 
// mBaseAddress by the caller.
//
int DWARF3File::GetAddressRepSpawn(int addressRepTypeFlags, uint64_t nAddress, FixedString* pRepArray, int* pIntValueArray)
{
	int returnFlags = 0;

	#if defined(EA_PLATFORM_WINDOWS) && EACALLSTACK_ENABLE_ELF_SPAWN
		// We have a Win32 utility applet stored as a C++ array in gAddr2LineExeData.
		// Here we unpack that utility to the temp directory execute it with our 
		// Win32Runner code and read the output.
		enum Addr2LineType
		{
			kAddr2LineTypePS4,      // PS4 addr2line.exe path
			kAddr2LineTypeGeneric,  // Generic addr2line.exe path
			kAddr2LineTypeCount
		};

		wchar_t elfReaderCabFileDir [EA::IO::kMaxPathLength];
		elfReaderCabFileDir[0] = 0;

		wchar_t elfReaderCabFilePath[EA::IO::kMaxPathLength];
		elfReaderCabFilePath[0] = 0;

		wchar_t elfReaderFilePath[kAddr2LineTypeCount][EA::IO::kMaxPathLength]; 
		for(int j = 0; j < kAddr2LineTypeCount; j++)
			elfReaderFilePath[j][0] = 0;

		int length = EA::IO::GetTempDirectory(elfReaderCabFileDir, EA::IO::kMaxPathLength);
		wcscat(elfReaderCabFileDir, L"EACallstack\\");
		length += (int)(unsigned)wcslen(L"EACallstack\\");
		EA::IO::Directory::EnsureExists(elfReaderCabFileDir);

		wcscpy(elfReaderCabFilePath, elfReaderCabFileDir);
		wcscpy(elfReaderCabFilePath + length, L"Addr2LineExe.cab");

		wcscpy(elfReaderFilePath[kAddr2LineTypeGeneric], elfReaderCabFileDir);
		wcscpy(elfReaderFilePath[kAddr2LineTypeGeneric] + length, L"Addr2Line.exe");

		// See if the user has PS4 tools installed on their system. Nearly all PS4 programmers do.
		//    SCE_ORBIS_SDK_DIR=C:\Program Files (x86)\SCE\ORBIS SDKs\1.020
		//    C:\Program Files (x86)\SCE\ORBIS SDKs\1.020\host_tools\bin\orbis-addr2line.exe
		const char* pPS4RootPath = getenv("SCE_ORBIS_SDK_DIR"); 
		
		// orbis-bin uses a different output format than addr2line so we need to know if we will be using it
		bool useOrbisBin = false;
		if(pPS4RootPath)
		{
			length = EA::StdC::Strlcpy(elfReaderFilePath[kAddr2LineTypePS4], pPS4RootPath, EA::IO::kMaxPathLength);
			eastl::replace(elfReaderFilePath[kAddr2LineTypePS4], elfReaderFilePath[kAddr2LineTypePS4] + length, (wchar_t)'/', (wchar_t)'\\');
			if(elfReaderFilePath[kAddr2LineTypePS4][length] != '\\')
			{
				elfReaderFilePath[kAddr2LineTypePS4][length++] = '\\';
				elfReaderFilePath[kAddr2LineTypePS4][length]   = '\0';
			}
			// We cannot use orbis-addr2line since Sony by default enables ASLR which addr2line does not support
			// But we can get the correct information out of orbis-bin
			EA::StdC::Strlcat(elfReaderFilePath[kAddr2LineTypePS4], L"host_tools\\bin\\orbis-bin.exe", EA::IO::kMaxPathLength);
			useOrbisBin = true;
		}

		if(!EA::IO::File::Exists(elfReaderFilePath[kAddr2LineTypeGeneric]))
		{
			EA::IO::FileStream cabFile(elfReaderFilePath[kAddr2LineTypeGeneric]);

			if(cabFile.Open(EA::IO::kAccessFlagWrite, EA::IO::kCDCreateAlways))
			{
				const bool bCabFileOK = cabFile.Write(gAddr2LineExeData, sizeof(gAddr2LineExeData));
				cabFile.Close();

				if(bCabFileOK)
				{
					CabReader cabReader;

					if(cabReader.Init())
					{
						// Decompress Addr2LineExe.cab to Addr2Line.exe.
						char elfReaderCabFilePath8[EA::IO::kMaxPathLength];
						char elfReaderCabFileDir8[EA::IO::kMaxPathLength];

						EA::StdC::Strlcpy(elfReaderCabFilePath8, elfReaderFilePath[kAddr2LineTypeGeneric], EA::IO::kMaxPathLength);
						EA::StdC::Strlcpy(elfReaderCabFileDir8,  elfReaderCabFileDir,                      EA::IO::kMaxPathLength);

						cabReader.Copy(elfReaderCabFilePath8, elfReaderCabFileDir8, NULL);

						cabReader.Shutdown();
					}
				}
			}
		}

		for(int i = 0; i < kAddr2LineTypeCount; i++)
		{
			if(!EA::IO::File::Exists(elfReaderFilePath[i]))
				elfReaderFilePath[i][0] = 0;
		}

		// At this point we have Addr2Line.exe in our temp directory. We use our Win32Runner
		// to run it with appropriate arguments, and look at the results. The app runs like
		// addr2line.exe and to get file/offset and file/line info we run it like so:
		//     Addr2Line.exe -f -e <elf path> [0xAddr [0xAddr] ...] 
		// It prints output like so:
		//    EA::Messaging::Server::ProcessQueue()
		//    C:\Packages\EAMessage\dev\source\/EAMessage.cpp:790
		// If it can't succeed, it prints output like so:
		//    ??
		//    ??:0
		// or possibly like so:
		//    app.self: File format not recognized

		EA::Callstack::Win32Runner runner;
		int result;

		if(runner.Init())
		{
			EA::Callstack::CAString8 sOutput;
			sOutput.get_allocator().set_allocator(EA::Callstack::GetAllocator());
			sOutput.get_allocator().set_name(EACALLSTACK_ALLOC_PREFIX "EACallstack/DWARF3File");

			const size_t kArgStringLength = 512;
			wchar_t argumentString[kArgStringLength];
			EA::Callstack::FixedStringW sDatabasePath = GetDatabasePath();

			// The ELF reader can't directly read Sony PS3 .self files, so if we are working with
			// one of those then we need to convert it back to a .elf file and use that.
			if((mELFOffset != 0) ||                                                // If we have an SCE header (signed elf)...
			   (sDatabasePath.rfind(L".self") == (sDatabasePath.length() - 5)) ||  // If sDatabasePath ends with .self (and somehow didn't already detect mELFOffset != 0)
			   (sDatabasePath.rfind(L".sprx") == (sDatabasePath.length() - 5)))    // or .sprx (and somehow didn't already detect mELFOffset != 0)
			{
				EA::Callstack::FixedStringW sDatabasePathNew(elfReaderCabFileDir);
				sDatabasePathNew += EA::IO::Path::GetFileName(sDatabasePath.c_str());
				sDatabasePathNew.resize(sDatabasePathNew.length() - 5); // Remove the ".self" suffix
				sDatabasePathNew += L".elf";

				// If there is already a file with the given name, we check to see how old it is. If it's 
				// less than N seconds old, we assume it is the same as what we want to use. This is reasonable
				// because to build and run a .self alone takes on the order of at least minutes.

				const time_t fileTime       = EA::IO::File::GetTime(sDatabasePathNew.c_str(), EA::IO::kFileTimeTypeLastModification); // Will be zero if file doesn't exist.
				const time_t currentTime    = time(NULL);

				#if defined(EA_DEBUG)  && defined(AUTHOR_PPEDRIANA)
				const time_t maxElapsedTime = (600000);
				#else
				const time_t maxElapsedTime = (600);
				#endif

				if((currentTime - fileTime) > maxElapsedTime) // If the file is "old" or if it doesn't exist...
				{
					// We don't really need to do this because we open it with kCDCreateAlways below.
					// EA::IO::File::Remove(sDatabasePathNew.c_str()); // Remove it if it exists.

					// Copy the .self file to the temp location, and convert it to .elf while doing so.
					EA::IO::FileStream fsSource(sDatabasePath.c_str());
					fsSource.AddRef();

					if(fsSource.Open())
					{
						// Make a buffer for the source stream. This will speed copying.
						//EA::IO::StreamBuffer fsSourceB(262144, 0, &fsSource, mpCoreAllocator);
						//fsSourceB.AddRef();

						// Get the location within the .self file where the actual .elf that we want begins.
						uint64_t nELFLocation = 0;
						fsSource.SetPosition(16);
						EA::IO::ReadUint64(&fsSource, nELFLocation, EA::IO::kEndianBig);
						fsSource.SetPosition((EA::IO::off_type)nELFLocation);

						// Open the destination 
						EA::IO::FileStream fsDest(sDatabasePathNew.c_str());
						fsDest.AddRef();

						if(fsDest.Open(IO::kAccessFlagWrite, EA::IO::kCDCreateAlways))
						{
							EA::Callstack::Local::CopyStream(&fsSource, &fsDest, fsSource.GetAvailable());
							fsDest.Close();
						}

						fsSource.Close();
					}
				}

				sDatabasePath = sDatabasePathNew;
			}

			if (useOrbisBin)
			{
				// e.g. args: -i C:\tmp\mygame.elf -a2l 0x0079DD3B
				if (nAddress <= 0xffffffff)
					EA::StdC::Sprintf(argumentString, L" -i \"%ls\" -a2l 0x%08x", sDatabasePath.c_str(), (uint32_t)nAddress);
				else
					EA::StdC::Sprintf(argumentString, L" -i \"%ls\" -a2l 0x%016I64x", sDatabasePath.c_str(), nAddress);
			}
			else
			{
				if (nAddress <= 0xffffffff)
					EA::StdC::Sprintf(argumentString, L" -f -e \"%ls\" 0x%08x", sDatabasePath.c_str(), (uint32_t)nAddress);
				else
					EA::StdC::Sprintf(argumentString, L" -f -e \"%ls\" 0x%016I64x", sDatabasePath.c_str(), nAddress);
			}

			char16_t argString16[kArgStringLength];
			EA::StdC::Strlcpy(argString16, argumentString, kArgStringLength);
			for(int i = 0; i < kAddr2LineTypeCount; i++)
			{
				char16_t path16[EA::IO::kMaxPathLength];
				EA::StdC::Strlcpy(path16, elfReaderFilePath[i], EA::IO::kMaxPathLength);		

				runner.SetTargetProcessPath(path16);
				runner.SetArgumentString(argString16);
				runner.SetResponseTimeout(1500);
				runner.SetUnilateralTimeout(3000);
				result = runner.Run();
				sOutput.clear();
				runner.GetOutput(sOutput); 
				runner.Shutdown();

				if(result != 0)
					result = 0; // What can we do better than this?

				bool elfReadSuccessful = true;
				if (useOrbisBin)
				{
					const char* errorStringArray[] =
					{
						"Invalid user input",
						"ERROR: "
					};

					for (size_t e = 0; (e < EAArrayCount(errorStringArray)) && elfReadSuccessful; e++)
					{
						if (sOutput.find(errorStringArray[e]) != EA::Callstack::CAString8::npos)
							elfReadSuccessful = false;
					}
				}
				else
				{
					const char* errorStringArray[] =
					{
						"File format not recognized",
						": Bad value",
						"error: ",
						"Error: "
					};

					for (size_t e = 0; (e < EAArrayCount(errorStringArray)) && elfReadSuccessful; e++)
					{
						if (sOutput.find(errorStringArray[e]) != EA::Callstack::CAString8::npos)
							elfReadSuccessful = false;
					}
				}

				if((result == 0) && elfReadSuccessful)
				{
					if (useOrbisBin)
					{
						// Process the output. We are given output formatted as such:
						// Address:       0x0079DD3B
						// Directory:     C:/p4/UFC/DL_GP3/samplegame/gameplay/private/ufcantplugins/dev/source/sceneops/runtime/cmn
						// File Name:     healthstaminasceneopinstance.cpp
						// Line Number:   26
						// Symbol:        EA::Ant::UFC::HealthStaminaSceneOpInstance::HealthStaminaSceneOpInstance(EA::Ant::UFC::HealthStaminaSceneOp const*, unsigned int)
						eastl_size_t ii, iColon;

						// Replace mistaken "\/" sequences with just "\"
						while ((ii = sOutput.find("\\/")) != EA::Callstack::CAString8::npos)
							sOutput.replace(ii, 2, "\\");
						// Replace forward slashes with Windows path separators since we know we are running on windows
						while ((ii = sOutput.find("/")) != EA::Callstack::CAString8::npos)
							sOutput.replace(ii, 1, "\\");


						// Reset our pointer to the beginning of the output
						ii = 0;

						// Get Address
						pRepArray[kARTAddress].clear();
						pIntValueArray[kARTAddress] = 0;
						for (; sOutput[ii] && (sOutput[ii] != '\r') && (sOutput[ii] != '\n'); ii++)
							pRepArray[kARTAddress] += sOutput[ii];
						iColon = pRepArray[kARTAddress].rfind(':');
						if (iColon == FixedString::npos)
							iColon = ii;
						if (pRepArray[kARTAddress].size() > iColon) // Sanity check.
						{
							pRepArray[kARTAddress].erase(0, iColon + 1);
							pRepArray[kARTAddress].trim();
							pIntValueArray[kARTAddress] = (int)EA::StdC::StrtoI32(pRepArray[kARTAddress].c_str(), nullptr, 16);

							if (!pRepArray[kARTAddress].empty())
								returnFlags |= (1 << kARTAddress);
						}
						else
						{
							pRepArray[kARTAddress].clear();
						}
						
						// Move to the next line of sOutput.
						while ((sOutput[ii] == '\r') || (sOutput[ii] == '\n'))
							ii++;

						// Get File/Line
						// We will combine the filename and the directory output here as a convenience to the user
						FixedString directoryStr;

						directoryStr.clear();
						for (; sOutput[ii] && (sOutput[ii] != '\r') && (sOutput[ii] != '\n'); ii++)
							directoryStr += sOutput[ii];
						iColon = directoryStr.find(':');
						if (iColon == FixedString::npos)
							iColon = ii;
						if (directoryStr.size() > iColon) // Sanity check.
						{
							directoryStr.erase(0, iColon + 1);
							directoryStr.trim();
						}
						else
						{
							directoryStr.clear();
						}

						// We now have a directory (or an empty directory) so now fine the file path
						while ((sOutput[ii] == '\r') || (sOutput[ii] == '\n'))
							ii++;

						pRepArray[kARTFileLine].clear();
						for (; sOutput[ii] && (sOutput[ii] != '\r') && (sOutput[ii] != '\n'); ii++)
							pRepArray[kARTFileLine] += sOutput[ii];
						iColon = pRepArray[kARTFileLine].rfind(':');
						if (iColon == FixedString::npos)
							iColon = ii;
						if (pRepArray[kARTFileLine].size() > iColon) // Sanity check.
						{
							pRepArray[kARTFileLine].erase(0, iColon + 1);
							pRepArray[kARTFileLine].trim();

							pRepArray[kARTFileLine] = directoryStr + "\\" + pRepArray[kARTFileLine];
						}
						else
						{
							pRepArray[kARTFileLine].clear();
						}

						while ((sOutput[ii] == '\r') || (sOutput[ii] == '\n'))
							ii++;

						// Get the FileLine number
						FixedString lineNumberStr;

						lineNumberStr.clear();
						for (; sOutput[ii] && (sOutput[ii] != '\r') && (sOutput[ii] != '\n'); ii++)
							lineNumberStr += sOutput[ii];
						iColon = lineNumberStr.rfind(':');
						if (iColon == FixedString::npos)
							iColon = ii;
						if (lineNumberStr.size() > iColon) // Sanity check.
						{
							lineNumberStr.erase(0, iColon + 1);
							lineNumberStr.trim();
							pIntValueArray[kARTFileLine] = (int)EA::StdC::AtoI32(lineNumberStr.c_str());

							if (!pRepArray[kARTFileLine].empty() && pIntValueArray[kARTFileLine])
								returnFlags |= (1 << kARTFileLine);
						}

						// Move to the next line of sOutput.
						while ((sOutput[ii] == '\r') || (sOutput[ii] == '\n'))
							ii++;

						// Function/Offset,                   
						pRepArray[kARTFunctionOffset].clear();
						pIntValueArray[kARTFunctionOffset] = 0;
						for (; sOutput[ii] && (sOutput[ii] != '\r') && (sOutput[ii] != '\n'); ii++)
							pRepArray[kARTFunctionOffset] += sOutput[ii];
						iColon = pRepArray[kARTFunctionOffset].find(':');
						if (iColon == FixedString::npos)
							iColon = ii;
						if (pRepArray[kARTFunctionOffset].size() > iColon) // Sanity check.
						{
							pRepArray[kARTFunctionOffset].erase(0, iColon + 1);
							pRepArray[kARTFunctionOffset].trim();

							if (!pRepArray[kARTFunctionOffset].empty())
								returnFlags |= (1 << kARTFunctionOffset);
						}
						else
						{
							pRepArray[kARTFunctionOffset].clear();
						}

						// Move to the next line of sOutput.
						while ((sOutput[ii] == '\r') || (sOutput[ii] == '\n'))
							ii++;

						// Get kARTSource
						pRepArray[kARTSource].clear();
						pIntValueArray[kARTSource] = 0; // This information is not provided by orbis-bin

						if (!pRepArray[kARTSource].empty() && pIntValueArray[kARTSource])
							returnFlags |= (1 << kARTSource);

						break; // This invocation of orbis-bin succeeded.
					}
					else
					{
						// Process the output
						eastl_size_t ii, iColon;

						// Replace mistaken "\/" sequences with just "\"
						while ((ii = sOutput.find("\\/")) != EA::Callstack::CAString8::npos)
							sOutput.replace(ii, 2, "\\");

						// If the output begins with a '.', simply remove it. Having it will prevent demangling of the following text.
						if (sOutput[0] == '.')
							sOutput.erase(0, 1);

						// Get Function/Offset
						pRepArray[kARTFunctionOffset].clear();
						pIntValueArray[kARTFunctionOffset] = 0; // We don't seem to be able to get this.
						for (ii = 0; sOutput[ii] && (sOutput[ii] != '\r') && (sOutput[ii] != '\n'); ii++)
							pRepArray[kARTFunctionOffset] += sOutput[ii];

						if (pRepArray[kARTFunctionOffset][0] && (pRepArray[kARTFunctionOffset][0] != '?'))
							returnFlags |= (1 << kARTFunctionOffset);

						// Move to the next line of sOutput.
						while ((sOutput[ii] == '\r') || (sOutput[ii] == '\n'))
							ii++;

						// Get File/Line
						pRepArray[kARTFileLine].clear();
						pIntValueArray[kARTFileLine] = 0;
						for (; sOutput[ii] && (sOutput[ii] != '\r') && (sOutput[ii] != '\n'); ii++)
							pRepArray[kARTFileLine] += sOutput[ii];
						iColon = pRepArray[kARTFileLine].rfind(':');
						if (iColon == FixedString::npos)
							iColon = ii;
						if (pRepArray[kARTFileLine].size() > iColon) // Sanity check.
							pIntValueArray[kARTFileLine] = (int)EA::StdC::AtoI32(&pRepArray[kARTFileLine][iColon + 1]);
						pRepArray[kARTFileLine].resize(iColon);

						if (pRepArray[kARTFileLine][0] && (pRepArray[kARTFileLine][0] != '?'))
							returnFlags |= (1 << kARTFileLine);

						break; // This invocation of addr2line succeeded.
					}
				}
			}
		}

	#else
		(void)addressRepTypeFlags;
		(void)nAddress;
		(void)pRepArray;
		(void)pIntValueArray;
	#endif

	return returnFlags;
}



///////////////////////////////////////////////////////////////////////////////
// DWARF3SectionReader
///////////////////////////////////////////////////////////////////////////////

///////////////////////////////////////////////////////////////////////////////
// DWARF3SectionReader::DWARF3SectionReader
//
DWARF3SectionReader::DWARF3SectionReader()
	: IO::MemoryStream(),
	  mpDWARF3(NULL),
	  mpStreamMem(NULL),
	  mnStreamMemSize(0)
{
}


///////////////////////////////////////////////////////////////////////////////
// DWARF3SectionReader::ReadULEB128
//
// Read an unsigned LEB128 ("Little Endian Base 128"), an integer compression
// scheme that saves space by not storing the topmost bits (the "big" end) of
// the integer that are all zeroes or sign extension bits.
//
// See the DWARF3 specification section 7.6.
//
bool DWARF3SectionReader::ReadULEB128(uint64_t* pnULEB128Out)
{
	// This function is a major bottleneck on large .elf files. We gain 
	// significant speedups (~3 faster lookups) by doing direct access to 
	// the read buffer and by avoiding some checking in release builds.

	uint8_t  nTemp8;
	uint32_t nShift = 0;


	uint64_t nResult = 0;


	#if !defined(EA_DEBUG)
		const uint8_t* const pDataBegin = (const uint8_t*)mpSharedPointer->GetPointer() + mnPosition;
		const uint8_t* pData = pDataBegin;
	#endif

	do
	{
		#if defined(EA_DEBUG)
			if(!Read(&nTemp8, 1)) // Alternatively: if(!IO::ReadUint8(this, nTemp8))
				return false;
		#else
			nTemp8 = *pData++;
		#endif


			
		#if defined(EA_DEBUG)
			if(nShift > (7 * 9)) // When encoded, an LEB128 integer uses 9 bytes max
				return false;
		#endif

		nResult  = nResult | (static_cast<uint64_t>(nTemp8 & 0x7F) << static_cast<uint64_t>(nShift));

		nShift  += 7;

	}
	while(nTemp8 & 0x80);

	*pnULEB128Out = nResult;

	#if !defined(EA_DEBUG)
		mnPosition += (pData - pDataBegin);
	#endif

	return true;

	// //Reference implementation:
	//
	// *pnULEB128Out = 0;
	// 
	// int     nShift = 0;
	// uint8_t nTemp8;
	// 
	// do
	// {
	//     if(!IO::ReadUint8(this, nTemp8))
	//         return false;
	//     if(nShift > (7*9))   //            When encoded, an LEB128 integer uses 9 bytes max
	//        return false;
	// 
	//     *pnULEB128Out |= ((uint64_t)(nTemp8 & 0x7F)) << nShift;
	// 
	//     nShift += 7;
	// 

	// }
	// while(nTemp8 & 0x80);
	// 
	// return true;
}


///////////////////////////////////////////////////////////////////////////////
// DWARF3SectionReader::ReadSLEB128
//
// Read a signed LEB128 ("Little Endian Base 128"), an integer compression
// scheme that saves space by not storing the topmost bits (the "big" end) 
// of the integer that are all zeroes or sign extension bits.
//
// See the DWARF3 specification section 7.6.
//
bool DWARF3SectionReader::ReadSLEB128(int64_t* pnSLEB128Out)
{
	// This function is a major bottleneck on large .elf files. We gain 
	// significant speedups (~3 faster lookups) by doing direct access to 
	// the read buffer and by avoiding some checking in release builds.

	uint8_t  nTemp8;
	uint32_t nShift = 0;


	uint64_t nResult = 0;


	#if !defined(EA_DEBUG)
		const uint8_t* const pDataBegin = (const uint8_t*)mpSharedPointer->GetPointer() + mnPosition;
		const uint8_t* pData = pDataBegin;
	#endif

	do
	{
		#if defined(EA_DEBUG)
			if(!Read(&nTemp8, 1)) // Alternatively: if(!IO::ReadUint8(this, nTemp8))
				return false;
		#else
			nTemp8 = *pData++;
		#endif

		#if defined(EA_DEBUG)
			if(nShift > (7 * 9)) // When encoded, an LEB128 integer uses 9 bytes max
			   return false;
		#endif


		nResult  = nResult | (static_cast<uint64_t>(nTemp8 & 0x7F) << static_cast<uint64_t>(nShift));

		nShift  += 7;


	}
	while(nTemp8 & 0x80);

	*pnSLEB128Out = (int64_t)nResult;

	if(nTemp8 & 0x40)                    // Sign bit of byte is 2nd high order bit (0x40)
		*pnSLEB128Out |= -(1 << nShift); // Sign extend

	#if !defined(EA_DEBUG)
		mnPosition += (pData - pDataBegin);
	#endif

	return true;

	// //Reference implementation:
	//
	// *pnSLEB128Out = 0;
	// 
	// int     nShift = 0;
	// uint8_t nTemp8;
	// 
	// do
	// {
	//     if(!IO::ReadUint8(this, nTemp8))
	//         return false;
	//     if(nShift > (7*9))              // When encoded, an LEB128 integer uses 9 bytes max
	//        return false;
	// 
	//     *pnSLEB128Out |= ((uint64_t)(nTemp8 & 0x7F)) << nShift;
	// 
	//     nShift += 7;
	// 

	// }
	// while(nTemp8 & 0x80);
	// 
	// if(nTemp8 & 0x40)                    // Sign bit of byte is 2nd high order bit (0x40)
	//     *pnSLEB128Out |= -(1 << nShift); // Sign extend
	// 
	// return true;
}


///////////////////////////////////////////////////////////////////////////////
// ReadULEB128
//
// This exists for tracing / debugging convenience.
//
#if defined(EA_DEBUG)
	bool DWARF3SectionReader::ReadULEB128(AbbrevTag* pAbbrevTag)
	{
		uint64_t nValue64;

		if(ReadULEB128(&nValue64))
		{
			*pAbbrevTag = (AbbrevTag)(uint32_t)nValue64;
			return true;
		}

		return false;
	}
#endif


///////////////////////////////////////////////////////////////////////////////
// ReadULEB128
//
// This exists for tracing / debugging convenience.
//
#if defined(EA_DEBUG)
	bool DWARF3SectionReader::ReadULEB128(DwarfForm* pDwarfForm)
	{
		uint64_t nValue64;

		if(ReadULEB128(&nValue64))
		{
			*pDwarfForm = (DwarfForm)(uint32_t)nValue64;
			return true;
		}

		return false;
	}
#endif


///////////////////////////////////////////////////////////////////////////////
// ReadULEB128
//
// This exists for tracing / debugging convenience.
//
#if defined(EA_DEBUG)
	bool DWARF3SectionReader::ReadULEB128(DwarfAttribute* pDwarfAttribute)
	{
		uint64_t nValue64;

		if(ReadULEB128(&nValue64))
		{
			*pDwarfAttribute = (DwarfAttribute)(uint32_t)nValue64;
			return true;
		}

		return false;
	}
#endif


///////////////////////////////////////////////////////////////////////////////
// CompUnitReader::Init
//
bool CompUnitReader::Init(DWARF3File* pDWARF3)
{
	mpDWARF3 = pDWARF3;

	// Read the aranges section into memory.
	int nArangesSectionStart = mpDWARF3->mnDebugArangesStart;
	int nArangesSectionEnd   = mpDWARF3->mnDebugArangesEnd;

	mnStreamMemSize = (uint32_t)(unsigned)(nArangesSectionEnd - nArangesSectionStart);
	mpStreamMem     = (char*)mpDWARF3->mpCoreAllocator->Alloc(mnStreamMemSize, EACALLSTACK_ALLOC_PREFIX "EACallstack/DWARF3File", 0);

	if(mpStreamMem)
	{
		const IO::off_type pos = (IO::off_type)(int32_t)pDWARF3->mELFOffset + (IO::off_type)nArangesSectionStart;

		if(pDWARF3->mpStream->SetPosition(pos) &&
		   pDWARF3->mpStream->Read(mpStreamMem, mnStreamMemSize) == mnStreamMemSize)
		{
			SetData(mpStreamMem, mnStreamMemSize, true, true, mpDWARF3->mpCoreAllocator); // Have the stream take over the pointer. It will free it on stream destruction.
			return true;
		}
	}

	return false;
}


///////////////////////////////////////////////////////////////////////////////
// CompUnitReader::FindUnitContainingAddress
//
bool CompUnitReader::FindUnitContainingAddress(uint64_t address, CompUnitInfo* pCompUnitInfoOut)
{
	// Address range table scan; look for the compilation unit containing
	// the specified address. See section 7.20 of the DWARF3 specification.
	bool bFound = false;

	if(SetPosition(0))
	{
		const IO::off_type      streamSize = (IO::off_type)GetSize();
		uint32_t                outerLoopCount = 0;  // For sanity checking, infinite loop prevention.
		AddressRangeTableHeader artHeader = { 0, 0, 0, 0, 0 };

		while(!bFound && (GetPosition() < streamSize) && (++outerLoopCount < 100000000))
		{
			// Read the current compilation unit header.
			if(!IO::ReadUint32(this, artHeader.mnLength, mpDWARF3->mEndian)) // In 64 bit DWARF3 format (not necessarily meaning a 64 bit platform), this is 0xffffffff, followed by a uint64_t which is the real length.
				return false;
			if(!IO::ReadUint16(this, artHeader.mnVersion, mpDWARF3->mEndian))
				return false;
			if(artHeader.mnVersion < 2) // If not DWARF version 2 or 3 (also this provides something of a sanity check)...
				return false;
			if(!IO::ReadUint32(this, artHeader.mnInfoOffset, mpDWARF3->mEndian))
				return false;
			if(!IO::ReadUint8(this, artHeader.mnPointerSize)) // This is sizeof(void*), typically 4 or 8.
				return false;
			if(!IO::ReadUint8(this, artHeader.mnSegmentDescriptorSize))
				return false;

			// The header is followed by a series of tuples. Each tuple consists of an 
			// address and a length, each in the size appropriate for an address on the
			// target architecture. The first tuple following the header in each set 
			// begins at an offset that is a multiple of the size of a single tuple 
			// (that is, twice the size of an address). The header is padded, if necessary, 
			// to the appropriate boundary. Each set of tuples is terminated by a 0 for 
			// the address and 0 for the length.

			const uint32_t nTupleSize   = (uint32_t)artHeader.mnPointerSize * 2;
			uint32_t       nTupleOffset = nTupleSize - (kHeaderSize % nTupleSize);

			// Seek past the header padding to the (aligned) address/length tuples.
			if(!SetPosition((int32_t)nTupleOffset, IO::kPositionTypeCurrent))
				return false;

			// Scan the address/length tuples in the compilation unit checking for
			// a range that contains the target address.
			nTupleOffset += kHeaderSize;

			uint32_t innerLoopCount = 0; // For sanity checking, infinite loop prevention.

			while((nTupleOffset < (artHeader.mnLength + 4)) && (++innerLoopCount < 100000000)) // 4-byte length field not included in size
			{
				// Read an address/length tuple.
				uint32_t nAddressStart32;
				uint32_t nAddressLength32;

				uint64_t nAddressStart64;
				uint64_t nAddressLength64;

				switch(artHeader.mnPointerSize)
				{
					case 4:
						if(!IO::ReadUint32(this, nAddressStart32, mpDWARF3->mEndian))
							return false;
						if(!IO::ReadUint32(this, nAddressLength32, mpDWARF3->mEndian))
							return false;

						nAddressStart64  = nAddressStart32;
						nAddressLength64 = nAddressLength32;
						break;

					case 8:
						if(!IO::ReadUint64(this, nAddressStart64, mpDWARF3->mEndian))
							return false;
						if(!IO::ReadUint64(this, nAddressLength64, mpDWARF3->mEndian))
							return false;
						break;

					default:
						// Unhandled ptr size.
						return false;
				}

				if((nAddressStart64 <= address) && (address < (nAddressStart64 + nAddressLength64)))
				{
					bFound = true;
					break;
				}

				nTupleOffset += nTupleSize;
			}

			// Seek to the end of the compilation unit.
			if(!bFound && !SetPosition((int32_t)(artHeader.mnLength - nTupleOffset + 4), IO::kPositionTypeCurrent))
				return false;
		}

		if(bFound)
		{
			// Read the header. See section 7.5.1 of the DWARF3 standard.
			if(!mpDWARF3->mpStream->SetPosition((int32_t)(mpDWARF3->mELFOffset + mpDWARF3->mnDebugInfoStart + artHeader.mnInfoOffset)))
				return false;
			if(!IO::ReadUint32(mpDWARF3->mpStream, pCompUnitInfoOut->mnLength, mpDWARF3->mEndian))
				return false;
			if(!IO::ReadUint16(mpDWARF3->mpStream, pCompUnitInfoOut->mnVersion, mpDWARF3->mEndian))
				return false;
			if(pCompUnitInfoOut->mnVersion < 2) // Verify DWARF version 2 or later. This check also acts as a sanity check.
				return false;
			if(!IO::ReadUint32(mpDWARF3->mpStream, pCompUnitInfoOut->mnAbbrevOffset, mpDWARF3->mEndian))
				return false;
			if(!IO::ReadUint8(mpDWARF3->mpStream, pCompUnitInfoOut->mnPointerSize))
				return false;

			pCompUnitInfoOut->mnPositionInInfoSection   = mpDWARF3->mnDebugInfoStart   + artHeader.mnInfoOffset;
			pCompUnitInfoOut->mnPositionInAbbrevSection = mpDWARF3->mnDebugAbbrevStart + pCompUnitInfoOut->mnAbbrevOffset;
		}
	}

	return bFound;
}


///////////////////////////////////////////////////////////////////////////////
// InfoSectionReader::Init
//
bool InfoSectionReader::Init(DWARF3File* pDWARF3)
{
	mpDWARF3 = pDWARF3;

	// Read the portion of the compilation unit info section that contains data related to 
	// the compilation unit containing our target address. See the DWARF3 Standard, sec 6.1.2.
	const int nInfoSectionStart = (int)(mpDWARF3->mCompUnitInfo.mnPositionInInfoSection + 11);      // Skip 11 byte header (size of the CompUnitInfo struct data) 
	const int nInfoSectionEnd   = (int)(nInfoSectionStart + mpDWARF3->mCompUnitInfo.mnLength + 4);  // Length field (4 bytes) not included in size

	mnStreamMemSize = (uint32_t)(nInfoSectionEnd - nInfoSectionStart);
	mpStreamMem     = (char*)mpDWARF3->mpCoreAllocator->Alloc(mnStreamMemSize, EACALLSTACK_ALLOC_PREFIX "EACallstack/DWARF3File", 0);

	if(mpStreamMem)
	{
		const IO::off_type pos = (IO::off_type)pDWARF3->mELFOffset + (IO::off_type)nInfoSectionStart;

		if(pDWARF3->mpStream->SetPosition(pos) &&
		   pDWARF3->mpStream->Read(mpStreamMem, mnStreamMemSize) == mnStreamMemSize)
		{
			// Set the buffer as our memory stream.
			SetData(mpStreamMem, mnStreamMemSize, true, true, mpDWARF3->mpCoreAllocator); // Have the stream take over the pointer. It will free it on stream destruction.
			return true;
		}
	}

	return false;
}


///////////////////////////////////////////////////////////////////////////////
// InfoSectionReader::Init
//

	// Read the portion of the compilation unit info section that contains data related to 
	// the compilation unit containing our target address. See the DWARF3 Standard, sec 6.1.2.



			// Set the buffer as our memory stream.




	// seek from start to the desired offset in .debug_ranges









///////////////////////////////////////////////////////////////////////////////
// InfoSectionReader::ReadAbbrevCode
//
bool InfoSectionReader::ReadAbbrevCode(uint64_t* pnAbbrevCodeOut)
{
	return ReadULEB128(pnAbbrevCodeOut);
}


///////////////////////////////////////////////////////////////////////////////
// DWARF3SectionReader::ReadAddressForm
//
bool InfoSectionReader::ReadAddressForm(uint64_t /*nFormType*/, uint64_t* pnFormOut)
{
	if(mpDWARF3->mCompUnitInfo.mnPointerSize == 4)
	{
		uint32_t value;
		const bool result = ReadUint32(this, value, mpDWARF3->mEndian);
		*pnFormOut = value;
		return result;
	}

	return IO::ReadUint64(this, *pnFormOut, mpDWARF3->mEndian);
}


///////////////////////////////////////////////////////////////////////////////
// InfoSectionReader::ReadConstantForm
//
bool InfoSectionReader::ReadConstantForm(uint64_t nFormType, uint64_t* pnFormOut)
{
	uint8_t  nTemp8;
	uint16_t nTemp16;
	uint32_t nTemp32;
	uint64_t nTemp64U;
	int64_t  nTemp64S;

	switch(nFormType)
	{
		case DW_FORM_DATA1:
			if(!IO::ReadUint8(this, nTemp8))
				return false;
			*pnFormOut = nTemp8;
			break;

		case DW_FORM_DATA2:
			if(!IO::ReadUint16(this, nTemp16, mpDWARF3->mEndian))
				return false;
			*pnFormOut = nTemp16;
			break;

		case DW_FORM_DATA4:
			if(!IO::ReadUint32(this, nTemp32, mpDWARF3->mEndian))
				return false;
			*pnFormOut = nTemp32;
			break;

		case DW_FORM_DATA8:
			if(!IO::ReadUint64(this, nTemp64U, mpDWARF3->mEndian))
				return false;
			*pnFormOut = nTemp64U;
			break;

		case DW_FORM_SDATA:
			if(!ReadSLEB128(&nTemp64S))
				return false;
			*pnFormOut = (uint64_t)nTemp64S;
			break;

		case DW_FORM_UDATA:
			if(!ReadULEB128(&nTemp64U))
				return false;
			*pnFormOut = nTemp64U;
			break;
		 case DW_FORM_SEC_OFFSET:
			if(!IO::ReadUint32(this, nTemp32, mpDWARF3->mEndian))
				return false;
			*pnFormOut = nTemp32;
			break;
		default:
			return false;
	}

	return true;
}


///////////////////////////////////////////////////////////////////////////////
// InfoSectionReader::ReadReferenceForm
//
bool InfoSectionReader::ReadReferenceForm(uint64_t nFormType, uint64_t* pnFormOut)
{
	if(nFormType == DW_FORM_REF_ADDR)
	{
		// This type of reference can identify any debugging information entry within 
		// a program; in particular, it may refer to an entry in a different compilation 
		// unit from the unit containing the reference, and may refer to an entry in a 
		// different shared object. This type of reference (DW_FORM_ref_addr) is an offset 
		// from the beginning of the .debug_info section of the target executable or 
		// shared object; it is relocatable in a relocatable object file and frequently 
		// relocated in an executable file or shared object. For references from one shared 
		// object or static executable file to another, the relocation and identification 
		// of the target object must be performed by the consumer.

		if(mpDWARF3->mCompUnitInfo.mnPointerSize == 4)
		{
			uint32_t value;
			const bool result = IO::ReadUint32(this, value, mpDWARF3->mEndian);
			*pnFormOut = value;
			return result;
		}
		else // (pointer size == 8)
			return IO::ReadUint64(this, *pnFormOut, mpDWARF3->mEndian);
	}
	else
	{
		// This type of reference can identify any debugging information entry within the 
		// containing compilation unit. It is an offset from the first byte of the compilation  
		// header for the compilation unit containing the reference. There are five forms for  
		// this type of reference. There are fixed length forms for one, two, four and eight byte 
		// offsets (respectively, DW_FORM_ref1, DW_FORM_ref2, DW_FORM_ref4, and DW_FORM_ref8). 
		// There is also an unsigned variable length offset encoded form that uses LEB128 
		// numbers (DW_FORM_ref_udata). Because this type of reference is within the containing 
		// compilation unit no relocation of the value is required.

		*pnFormOut = mpDWARF3->mCompUnitInfo.mnPositionInInfoSection;

		uint8_t  nTemp8;
		uint16_t nTemp16;
		uint32_t nTemp32;
		uint64_t nTemp64;

		switch(nFormType)
		{
			case DW_FORM_REF1:
				if(!IO::ReadUint8(this, nTemp8))
					return false;
				nTemp64 = nTemp8;
				break;

			case DW_FORM_REF2:
				if(!IO::ReadUint16(this, nTemp16, mpDWARF3->mEndian))
					return false;
				nTemp64 = nTemp16;
				break;

			case DW_FORM_REF4: // This is the most common of the DW_FORM_REF types. 
				if(!IO::ReadUint32(this, nTemp32, mpDWARF3->mEndian))
					return false;
				nTemp64 = nTemp32;
				break;

			case DW_FORM_REF8:
				if(!IO::ReadUint64(this, nTemp64, mpDWARF3->mEndian))
					return false;
				break;

			case DW_FORM_REF_UDATA:
				if(!ReadULEB128(&nTemp64))
					return false;
				break;

			default:
				return false;
		}

		*pnFormOut += nTemp64;
	}

	return true;
}


///////////////////////////////////////////////////////////////////////////////
// InfoSectionReader::ReadStringForm
//
bool InfoSectionReader::ReadStringForm(uint64_t nFormType, uint64_t* pnFormOut)
{
	if(nFormType == DW_FORM_STRING)
	{
		uint8_t nTemp8;

		// Return the string's offset from the beginning of the file.
		*pnFormOut = (uint64_t)(int64_t)GetPosition() + mpDWARF3->mCompUnitInfo.mnPositionInInfoSection + 11;

		do
		{
			if(!IO::ReadUint8(this, nTemp8))
				return false;
		}
		while(nTemp8);

		return true;
	}
	else if(nFormType == DW_FORM_STRP)
	{
		uint32_t nTemp32;

		if(!IO::ReadUint32(this, nTemp32, mpDWARF3->mEndian))
			return false;

		// Return the string's offset from the beginning of the file.
		*pnFormOut  = (unsigned)mpDWARF3->mnDebugStrStart;
		*pnFormOut += nTemp32;

		return true;
	}

	return false;
}


///////////////////////////////////////////////////////////////////////////////
// InfoSectionReader::SkipForm
//
bool InfoSectionReader::SkipForm(uint64_t nFormType)
{
	uint8_t  nTemp8;
	uint16_t nTemp16;
	uint32_t nTemp32;
	uint64_t nTemp64U;
	int64_t  nTemp64S;

	switch(nFormType)
	{
		case DW_FORM_ADDR:
			if(!SetPosition(mpDWARF3->mCompUnitInfo.mnPointerSize, IO::kPositionTypeCurrent))
				return false;
			break;

		case DW_FORM_BLOCK:
			if(!ReadULEB128(&nTemp64U))
				return false;
			if(!SetPosition((IO::off_type)nTemp64U, IO::kPositionTypeCurrent))
				return false;
			break;

		case DW_FORM_BLOCK1:
			if(!IO::ReadUint8(this, nTemp8))
				return false;
			if(!SetPosition(nTemp8, IO::kPositionTypeCurrent))
				return false;
			break;

		case DW_FORM_BLOCK2:
			if(!IO::ReadUint16(this, nTemp16, mpDWARF3->mEndian))
				return false;
			if(!SetPosition(nTemp16, IO::kPositionTypeCurrent))
				return false;
			break;

		case DW_FORM_BLOCK4:
			if(!IO::ReadUint32(this, nTemp32, mpDWARF3->mEndian))
				return false;
			if(!SetPosition((EA::IO::off_type)nTemp32, IO::kPositionTypeCurrent))
				return false;
			break;

		case DW_FORM_DATA1:
			if(!SetPosition(1, IO::kPositionTypeCurrent))
				return false;
			break;

		case DW_FORM_DATA2:
			if(!SetPosition(2, IO::kPositionTypeCurrent))
				return false;
			break;

		case DW_FORM_DATA4:
			if(!SetPosition(4, IO::kPositionTypeCurrent))
				return false;
			break;

		case DW_FORM_DATA8:
			if(!SetPosition(8, IO::kPositionTypeCurrent))
				return false;
			break;

		case DW_FORM_SDATA:
			if(!ReadSLEB128(&nTemp64S))
				return false;
			break;

		case DW_FORM_UDATA:
			if(!ReadULEB128(&nTemp64U))
				return false;
			break;

		case DW_FORM_STRP:
			if(!SetPosition(4, IO::kPositionTypeCurrent))
				return false;
			break;

		case DW_FORM_STRING:
			do
			{
				if(!IO::ReadUint8(this, nTemp8))
					return false;
			}
			while(nTemp8);
			break;

		case DW_FORM_FLAG:
			if(!SetPosition(1, IO::kPositionTypeCurrent))
				return false;
			break;

		case DW_FORM_REF_ADDR:
			if(!SetPosition(mpDWARF3->mCompUnitInfo.mnPointerSize, IO::kPositionTypeCurrent))
				return false;
			break;

		case DW_FORM_REF1:
			if(!SetPosition(1, IO::kPositionTypeCurrent))
				return false;
			break;

		case DW_FORM_REF2:
			if(!SetPosition(2, IO::kPositionTypeCurrent))
				return false;
			break;

		case DW_FORM_REF4:
			if(!SetPosition(4, IO::kPositionTypeCurrent))
				return false;
			break;

		case DW_FORM_REF8:
			if(!SetPosition(8, IO::kPositionTypeCurrent))
				return false;
			break;

		case DW_FORM_REF_UDATA:
			if(!ReadULEB128(&nTemp64U))
				return false;
			break;
		case DW_FORM_SEC_OFFSET:
			if(!IO::ReadUint32(this, nTemp32, mpDWARF3->mEndian))
				return false;
			break;
		case DW_FORM_FLAG_PRESENT:
			// as flag so NO additional read is required
			break;
		case DW_FORM_EXPRLOC:
			if(!ReadULEB128(&nTemp64U)) // dwarf4 page 148
				return false;
			if(!SetPosition((IO::off_type)nTemp64U, IO::kPositionTypeCurrent)) // skip information we don't care about
				return false;
			break;
		default:
			return false;
	}

	return true;
}


///////////////////////////////////////////////////////////////////////////////
// AbbrevSectionEntry::AbbrevSectionEntry
//
AbbrevSectionEntry::AbbrevSectionEntry()
  : mnAbbrevCode(0),
	mnAttributes(0),
	mnTag(DW_TAG_padding),
	mbHasChildren(false),
	mnNameOffset(0),
	mnStmtList(0),
	mnLowPC(0),
	mnHighPC(0),
	mnAbstractOrigin(0),
	mnSpecification(0)
{
}


///////////////////////////////////////////////////////////////////////////////
// AbbrevSectionEntry::ContainsAddress
//
bool AbbrevSectionEntry::ContainsAddress(uint64_t address) const
{
	if(mnAttributes & (ATTR_HIGH_PC | ATTR_LOW_PC))
	{
		if((mnLowPC <= address) && (address < mnHighPC))
				return true;

	}
 
	return false;
}


///////////////////////////////////////////////////////////////////////////////
// Clear
//
void AbbrevSectionEntry::Clear()
{
	// We clear all properties except mnStmtList. 
	const uint32_t nAttributes    = mnAttributes;
	const uint64_t nStmtListSaved = mnStmtList;


	mnAbbrevCode = 0;
	mnAttributes = 0;
	mnTag = DW_TAG_padding; // padding is zero and acts like the null tag
	mbHasChildren = 0;
	mnNameOffset = 0;
	mnStmtList = 0;
	mnHighPC = 0;
	mnLowPC = 0;
	mnAbstractOrigin = 0;
	mnSpecification = 0;


	if(nAttributes & ATTR_STMT_LIST)
	{
		mnAttributes |= ATTR_STMT_LIST;
		mnStmtList    = nStmtListSaved;
	}
}


///////////////////////////////////////////////////////////////////////////////
// CopyFrom
//
// We don't do a memcpy because pEntry may be a child of 'this' which 
// inherits data from it. So we copy on the data from pEntry that is set.
//
void AbbrevSectionEntry::CopyFrom(const AbbrevSectionEntry* pEntry)
{
	mnAbbrevCode  = pEntry->mnAbbrevCode;
	mnTag         = pEntry->mnTag;
	mbHasChildren = pEntry->mbHasChildren;

	if(pEntry->mnAttributes & ATTR_NAME)
		mnNameOffset = pEntry->mnNameOffset;

	if(pEntry->mnAttributes & ATTR_STMT_LIST)
		mnStmtList = pEntry->mnStmtList;

	if(pEntry->mnAttributes & ATTR_LOW_PC)
		mnLowPC = pEntry->mnLowPC;
	if(pEntry->mnAttributes & ATTR_HIGH_PC)
		mnHighPC = pEntry->mnHighPC;

	if(pEntry->mnAttributes & ATTR_SPECIFICATION)
		mnSpecification = pEntry->mnSpecification;

	if(pEntry->mnAttributes & ATTR_ABSTRACT_ORIGIN)
		mnAbstractOrigin = pEntry->mnAbstractOrigin;
}


///////////////////////////////////////////////////////////////////////////////
// AbbrevSectionReader::Init
//
bool AbbrevSectionReader::Init(DWARF3File* pDWARF3, InfoSectionReader* pInfoSectionReader, bool bCacheEntries, EA::Allocator::ICoreAllocator* /*pAllocator*/)
{
	mpDWARF3            = pDWARF3;
	mpInfoSectionReader = pInfoSectionReader;
	mbCacheEntries      = bCacheEntries;
	mFurthestPosition   = 0;

	// Read a portion of the abbrev section into memory, starting from the
	// compilation unit we are interested in, to the end of the section.

	// assert(mpDWARF3->mnDebugAbbrevStart                      <= mpDWARF3->mCompUnitInfo.mnPositionInAbbrevSection);
	// assert(mpDWARF3->mCompUnitInfo.mnPositionInAbbrevSection <  mpDWARF3->mnDebugAbbrevEnd);
	int nAbbrevSectionStart = (int)mpDWARF3->mCompUnitInfo.mnPositionInAbbrevSection;
	int nAbbrevSectionEnd   = (int)mpDWARF3->mnDebugAbbrevEnd;

	mnStreamMemSize = (uint32_t)(nAbbrevSectionEnd - nAbbrevSectionStart);
	mpStreamMem     = (char*)mpDWARF3->mpCoreAllocator->Alloc(mnStreamMemSize, EACALLSTACK_ALLOC_PREFIX "EACallstack/DWARF3File", 0);

	if(mpStreamMem)
	{
		const IO::off_type pos = (IO::off_type)pDWARF3->mELFOffset + (IO::off_type)nAbbrevSectionStart;

		if(pDWARF3->mpStream->SetPosition(pos) &&
		   pDWARF3->mpStream->Read(mpStreamMem, mnStreamMemSize) == mnStreamMemSize)
		{
			// Set the buffer as our memory stream.
			SetData(mpStreamMem, mnStreamMemSize, true, true, mpDWARF3->mpCoreAllocator); // Have the stream take over the pointer. It will free it on stream destruction.
			return true;
		}
	}

	

	return false;
}


#if defined(DWARF3_DEBUG)
	void TraceAbbrevSectionEntry(uint64_t nTempName, uint64_t /*nTempForm*/, DWARF3File* pFile, const AbbrevSectionEntry* pAbbrevSectionEntry)
	{
		FixedString fs;

		printf("   ");

		switch(nTempName)
		{
			// These are a number of the common attributes that we see. 
			// We have them here for debug tracing.
			case DW_AT_SIBLING:
				printf("DW_AT_SIBLING\n"); break;
			case DW_AT_LOCATION:
				printf("DW_AT_LOCATION\n"); break;
			case DW_AT_NAME:
				pFile->GetFunctionName(pAbbrevSectionEntry, &fs);
				printf("DW_AT_NAME                %s\n", fs.c_str());
				break;
			case DW_AT_BYTE_SIZE:
				printf("DW_AT_BYTE_SIZE\n"); break;
			case DW_AT_STMT_LIST:
				printf("DW_AT_STMT_LIST\n"); break;
			case DW_AT_LOW_PC:
				printf("DW_AT_LOW_PC              0x%08x\n", (uint32_t)pAbbrevSectionEntry->mnLowPC);
				break;
			case DW_AT_HIGH_PC:
				printf("DW_AT_HIGH_PC             0x%08x\n", (uint32_t)pAbbrevSectionEntry->mnHighPC);
				break;

			case DW_AT_LANGUAGE:
				printf("DW_AT_LANGUAGE\n"); break;
			case DW_AT_IMPORT:
				printf("DW_AT_IMPORT\n"); break;
			case DW_AT_COMP_DIR:
				printf("DW_AT_COMP_DIR\n"); break;
			case DW_AT_CONST_VALUE:
				printf("DW_AT_CONST_VALUE\n"); break;
			case DW_AT_CONTAINING_TYPE:
				printf("DW_AT_CONTAINING_TYPE\n"); break;
			case DW_AT_INLINE:
				printf("DW_AT_INLINE\n"); break;
			case DW_AT_PRODUCER:
				printf("DW_AT_PRODUCER\n"); break;
			case DW_AT_UPPER_BOUND:
				printf("DW_AT_UPPER_BOUND\n"); break;
			case DW_AT_ABSTRACT_ORIGIN:
				printf("DW_AT_ABSTRACT_ORIGIN     0x%08x\n", (uint32_t)pAbbrevSectionEntry->mnAbstractOrigin);
				break;
			case DW_AT_ACCESSIBILITY:
				printf("DW_AT_ACCESSIBILITY\n"); break;
			case DW_AT_ARTIFICIAL:
				printf("DW_AT_ARTIFICIAL\n"); break;
			case DW_AT_DATA_MEMBER_LOCATION:
				printf("DW_AT_DATA_MEMBER_LOCATION\n"); break;
			case DW_AT_DECL_COLUMN:
				printf("DW_AT_DECL_COLUMN\n"); break;
			case DW_AT_DECL_FILE:
				printf("DW_AT_DECL_FILE\n"); break;
			case DW_AT_DECL_LINE:
				printf("DW_AT_DECL_LINE\n"); break;
			case DW_AT_DECLARATION:
				printf("DW_AT_DECLARATION\n"); break;
			case DW_AT_ENCODING:
				printf("DW_AT_ENCODING\n"); break;
			case DW_AT_EXTERNAL:
				printf("DW_AT_EXTERNAL\n"); break;
			case DW_AT_FRAME_BASE:
				printf("DW_AT_FRAME_BASE\n"); break;
			case DW_AT_SPECIFICATION:
				printf("DW_AT_SPECIFICATION\n"); break;
			case DW_AT_TYPE:
				printf("DW_AT_TYPE\n"); break;
			case DW_AT_VIRTUALITY:
				printf("DW_AT_VIRTUALITY\n"); break;
			case DW_AT_VTABLE_ELEM_LOCATION:
				printf("DW_AT_VTABLE_ELEM_LOCATION\n"); break;
			default:
				printf("DW_AT_0x%08x\n", (unsigned)nTempName); break;
		}
	}
#endif



///////////////////////////////////////////////////////////////////////////////
// AbbrevSectionReader::FindEntryByCode
//
bool AbbrevSectionReader::FindEntryByCode(uint64_t nAbbrevCode, AbbrevSectionEntry* pAbbrevSectionEntryOut)
{
	EA::IO::off_type startPosition = mFurthestPosition;
	EA::IO::off_type currentPosition;

	if(mbCacheEntries)
	{
		EntryCache::const_iterator it = mEntryCache.find(nAbbrevCode);

		if(it != mEntryCache.end())
		{
			// We know where the starting position in the abbrev section is for the given nAbbrevCode.
			// In this case we will seek right to it below instead of walking linearly through the 
			// abbrev section info.
			startPosition    = it->second;
		}
	}

	bool               bFound = false;
	uint64_t           nTempCode;
	uint8_t            bHasChildren;
	const IO::off_type streamSize = (IO::off_type)GetSize();

	#ifdef EA_DEBUG
		AbbrevTag      nTempTag = DW_TAG_padding;
	#else
		uint64_t       nTempTag = 0;
	#endif

	// Search for the abbreviation entry in the abbrev section by its code.
	// The abbrev table consists a sets of { abbrevCode, AbbrevTag, childrenExistBool, name, form, name, form, ..., 0, 0 }
	// See section 7.5.3 of the DWARF3 Standard.
	SetPosition(startPosition); // If caching is disabled then startPosition will always be zero.

	while(!bFound && ((currentPosition = GetPosition()) < streamSize))
	{
		if(!ReadULEB128(&nTempCode))               // Read abbrevCode
			break;
		if(!nTempCode)  // If we are at the end of the abbrev table...
			break;
		if(!ReadULEB128(&nTempTag))                // Read AbbrevTag
			break;
		if(!IO::ReadUint8(this, bHasChildren))     // Read childrenExistBool
			break;

		if(mbCacheEntries) // If we have caching enabled...
			mEntryCache.insert(EntryCache::value_type(nTempCode, currentPosition)); // Save the location of the abbrev section for nAbbrevCode for a future lookup.

		if(nAbbrevCode == nTempCode)
		{
			pAbbrevSectionEntryOut->mnAbbrevCode  = nTempCode;
			pAbbrevSectionEntryOut->mnTag         = (AbbrevTag)nTempTag;
			pAbbrevSectionEntryOut->mbHasChildren = bHasChildren;

			if(ReadAbbreviationSectionEntry(pAbbrevSectionEntryOut))
				bFound = true;
			else
				break; // Error condition.
		}
		else
			SkipAbbreviationSectionEntry();
	}

	if(mbCacheEntries)
	{
		currentPosition = GetPosition();

		if(currentPosition > mFurthestPosition)
		   mFurthestPosition = currentPosition;
	}

	return bFound;
}


///////////////////////////////////////////////////////////////////////////////
// AbbrevSectionReader::ReadAbbreviationSectionEntry
//
bool AbbrevSectionReader::ReadAbbreviationSectionEntry(AbbrevSectionEntry* pAbbrevSectionEntryOut)
{
	#ifdef EA_DEBUG
		DwarfAttribute nTempName = DW_AT_NONE;
		DwarfForm      nTempForm = DW_FORM_NONE;
	#else
		uint64_t       nTempName = 0;
		uint64_t       nTempForm = 0;
	#endif

	// Read the abbrev entry.
	if(!ReadULEB128(&nTempName))
		return false;
	if(!ReadULEB128(&nTempForm))
		return false;

	while(nTempName && nTempForm) // Read all the attributes of the current entry (AbbrevTag).
	{
		if(nTempForm == DW_FORM_INDIRECT)
		{
			// Special case for DW_FORM_INDIRECT; read and continue.
			if(!mpInfoSectionReader->ReadULEB128(&nTempForm))
				return false;
		}
		else
		{
			switch(nTempName)
			{
				// case DW_AT_SIBLING:
				//    // To consider: take advantage of the DW_AT_SIBLING atttibute to allow us to jump directly to the next sibling for a speed improvement.
				//    break;

				case DW_AT_NAME:
					if(!mpInfoSectionReader->ReadStringForm(nTempForm, &pAbbrevSectionEntryOut->mnNameOffset))
						return false;
					pAbbrevSectionEntryOut->mnAttributes |= ATTR_NAME;
					break;

				case DW_AT_STMT_LIST:
					if(!mpInfoSectionReader->ReadConstantForm(nTempForm, &pAbbrevSectionEntryOut->mnStmtList))
						return false;
					pAbbrevSectionEntryOut->mnAttributes |= ATTR_STMT_LIST;
					break;

				case DW_AT_LOW_PC:
						if(!mpInfoSectionReader->ReadAddressForm(nTempForm, &pAbbrevSectionEntryOut->mnLowPC))
							return false;
					pAbbrevSectionEntryOut->mnAttributes |= ATTR_LOW_PC;
					break;

				case DW_AT_HIGH_PC:
					if(nTempForm == DW_FORM_ADDR)
					{
						if(!mpInfoSectionReader->ReadAddressForm(nTempForm, &pAbbrevSectionEntryOut->mnHighPC))
							return false;
					}
					else
					{
						// need to have already read the low pc
						if((pAbbrevSectionEntryOut->mnAttributes & ATTR_LOW_PC) == 0)
							return false;

						// dwarf4 page 155, this could also be a constant form now
						uint64_t offsetVal;
						if(!mpInfoSectionReader->ReadConstantForm(nTempForm, &offsetVal))
							return false;

						// this constant is really an offset from the low
						pAbbrevSectionEntryOut->mnHighPC = pAbbrevSectionEntryOut->mnLowPC + offsetVal;
					}
					pAbbrevSectionEntryOut->mnAttributes |= ATTR_HIGH_PC;
					break;

				case DW_AT_SPECIFICATION:
					if(!mpInfoSectionReader->ReadReferenceForm(nTempForm, &pAbbrevSectionEntryOut->mnSpecification))
						return false;
					pAbbrevSectionEntryOut->mnAttributes |= ATTR_SPECIFICATION;
					break;

				case DW_AT_ABSTRACT_ORIGIN:
					if(!mpInfoSectionReader->ReadReferenceForm(nTempForm, &pAbbrevSectionEntryOut->mnAbstractOrigin))
						return false;
					pAbbrevSectionEntryOut->mnAttributes |= ATTR_ABSTRACT_ORIGIN;
					break;

					
				case DW_AT_RANGES:
					uint32_t rangesOffset;  // we won't use ranges but we at least recognize it and read the stream
					if(!IO::ReadUint32(mpInfoSectionReader, rangesOffset, mpDWARF3->mEndian))
						return false;
					break;
				
				default:
					if(!mpInfoSectionReader->SkipForm(nTempForm))
						return false;
			}

			#if defined(DWARF3_DEBUG)
				TraceAbbrevSectionEntry(nTempName, nTempForm, mpDWARF3, pAbbrevSectionEntryOut);
			#endif

			if(!ReadULEB128(&nTempName))
				return false;
			if(!ReadULEB128(&nTempForm))
				return false;
		}
	}

	#if defined(DWARF3_DEBUG)
		printf("End entry.\n");
	#endif

	return true;
}


///////////////////////////////////////////////////////////////////////////////
// AbbrevSectionReader::SkipAbbreviationSectionEntry
//
bool AbbrevSectionReader::SkipAbbreviationSectionEntry()
{
	#ifdef EA_DEBUG
		DwarfAttribute nTempName = DW_AT_NONE;
		DwarfForm      nTempForm = DW_FORM_NONE;
	#else
		uint64_t       nTempName = 0;
		uint64_t       nTempForm = 0;
	#endif

	do
	{
		if(!ReadULEB128(&nTempName))
			return false;
		if(!ReadULEB128(&nTempForm))
			return false;
	}
	while(nTempName && nTempForm);

	return true;
}


///////////////////////////////////////////////////////////////////////////////
// LineSectionReader::Init
//
bool LineSectionReader::Init(DWARF3File* pDWARF3, AbbrevSectionEntry* pAbbrevSectionEntry)
{
	mpDWARF3 = pDWARF3;

	if(!pAbbrevSectionEntry->HasAnyAttribute(ATTR_STMT_LIST))
		return false;

	// Read a portion of the line section into memory, starting from the line
	// program we are interested in, to the end of the program for the target
	// compilation unit.
	const int nLineSectionStart = mpDWARF3->mnDebugLineStart + (int)(unsigned)pAbbrevSectionEntry->mnStmtList;

	// Determining the length of the line program requires loading the length
	// field from the program prologue.
	const IO::off_type pos = (IO::off_type)pDWARF3->mELFOffset + (IO::off_type)nLineSectionStart;

	if(!pDWARF3->mpStream->SetPosition(pos))
		return false;
	if(!IO::ReadUint32(pDWARF3->mpStream, mPrologue.mnTotalLength, mpDWARF3->mEndian))
		return false;

	mnStreamMemSize = mPrologue.mnTotalLength;
	mpStreamMem     = (char*)mpDWARF3->mpCoreAllocator->Alloc(mnStreamMemSize, EACALLSTACK_ALLOC_PREFIX "EACallstack/DWARF3File", 0);

	if(mpStreamMem)
	{
		if(pDWARF3->mpStream->Read(mpStreamMem, mnStreamMemSize) == mnStreamMemSize)
		{
			// Set the buffer as our memory stream.
			SetData(mpStreamMem, mnStreamMemSize, true, true, mpDWARF3->mpCoreAllocator); // Have the stream take over the pointer. It will free it on stream destruction.
			return true;
		}
	}

	return false;
}


///////////////////////////////////////////////////////////////////////////////
// LineSectionReader::FindFileLineInfoForAddress
//
bool LineSectionReader::FindFileLineInfoForAddress(uint64_t address, FileLineInfo* pFileLineInfoOut)
{
	pFileLineInfoOut->msSourceFileName.clear();
	pFileLineInfoOut->msSourceFileDirectory.clear();
	pFileLineInfoOut->mnLastModifiedTime = 0;
	pFileLineInfoOut->mnLengthInBytes = 0;
	pFileLineInfoOut->mnLine = 0;

	if(!SetPosition(0))
		return false;

	// Read the line program prologue.
	if(!IO::ReadUint16(this, mPrologue.mnVersion, mpDWARF3->mEndian))
		return false;
	if(mPrologue.mnVersion != 3 && mPrologue.mnVersion != 4) // assume dwarf 3 or 4 prologue supported
		return false;
	if(!IO::ReadUint32(this, mPrologue.mnPrologueLength, mpDWARF3->mEndian))
		return false;
	if(!IO::ReadUint8(this, mPrologue.mnMinimumInstructionLength))
		return false;
	
	// dwarf4 introduced a max ops per instruction pg 113 dwarf4	
	if(mPrologue.mnVersion == 4) 
	{
		if(!IO::ReadUint8(this, mPrologue.mnMaximumOperationsPerInstruction))
			return false;

		if( mPrologue.mnMaximumOperationsPerInstruction != 1)
			return false; // the rest of our code assumes non VLIW  :(
	}
		
	if(!IO::ReadUint8(this, mPrologue.mnDefaultIsStmt))
		return false;
	if(!IO::ReadInt8(this, mPrologue.mnLineBase))
		return false;
	if(!IO::ReadUint8(this, mPrologue.mnLineRange))
		return false;
	if(!IO::ReadUint8(this, mPrologue.mnOpcodeBase))
		return false;

	// Skip the standard opcode lengths array.
	if(!SetPosition(mPrologue.mnOpcodeBase - 1, IO::kPositionTypeCurrent))
		return false;

	uint8_t  nTemp8;
	uint64_t nTemp64;

	// Skip the include directories array.
	if(!IO::ReadUint8(this, nTemp8))
		return false;

	while(nTemp8)
	{
		while(nTemp8)
		{
			if(!IO::ReadUint8(this, nTemp8))
				return false;
		}

		if(!IO::ReadUint8(this, nTemp8))
			return false;
	}

	// Record the stream position. This is where the file name table begins.
	mnFileNameTableStart = (int64_t)GetPosition();

	LineMachineState lineMachineState;

	// Execute the line statement list.
	if(!ExecuteLineProgram(address, &lineMachineState))
		return false;

	// Read the file info from the file name table.
	if(!SetPosition((IO::off_type)mnFileNameTableStart))
		return false;

	uint64_t nFileIndex = lineMachineState.mnFile;
	uint64_t nDirectoryIndex = 0;

	for(unsigned int i = 0; i < nFileIndex; ++i)
	{
		pFileLineInfoOut->msSourceFileName.clear();


		if(!ReadString(this, &pFileLineInfoOut->msSourceFileName))
			return false;


		if(!ReadULEB128(&nTemp64))                              // Index into include directories array.
			return false;
		if(!ReadULEB128(&pFileLineInfoOut->mnLastModifiedTime)) // Last modification time for file.
			return false;
		if(!ReadULEB128(&pFileLineInfoOut->mnLengthInBytes))    // Length of file in bytes.
			return false;

		nDirectoryIndex = nTemp64;
	}

	// Read the directory from the include directories table.
	if(!SetPosition(11 + mPrologue.mnOpcodeBase - 1))
		return false;

	for(unsigned int i = 0; i < nDirectoryIndex; ++i)
	{
		pFileLineInfoOut->msSourceFileDirectory.clear();
		
		if(!ReadString(this, &pFileLineInfoOut->msSourceFileDirectory))
			return false;
	}

	pFileLineInfoOut->mnLine = lineMachineState.mnLine;
	return true;
}


///////////////////////////////////////////////////////////////////////////////
// LineSectionReader::ExecuteLineProgram
//
bool LineSectionReader::ExecuteLineProgram(uint64_t address, LineMachineState* pLineMachineStateOut)
{
	if(!SetPosition((EA::IO::off_type)(mPrologue.mnPrologueLength + 6)))
		return false;

	// these are not actually all the states of the line program
	mCurrState.mnAddress = 0;
	mCurrState.mnFile = 1;
	mCurrState.mnLine = 1;
	mCurrState.mbInitialized = false;

	mNextState.mnAddress = 0;
	mNextState.mnFile = 1;
	mNextState.mnLine = 1;
	mNextState.mbInitialized = true;

	mbFound = false;

	// Execute the line program until the target address is reached.
	const IO::off_type streamSize = (IO::off_type)GetSize();


	while(GetPosition() < streamSize)
	{
		uint8_t nOpcode;

		if(!IO::ReadUint8(this, nOpcode))
			return false;

		uint16_t nOperandU16;
		uint64_t nOperandU64;
		int64_t  nOperandS64;


		if(nOpcode >= mPrologue.mnOpcodeBase)
		{
			if(!ExecuteSpecialOpcode(address, nOpcode))
				return false;
		}
		else
		{
			switch(nOpcode)
			{
				case DW_LNS_EXTENDED_OPCODE:
					if(!ReadULEB128(&nOperandU64))     // Instruction size
						return false;
					if(!IO::ReadUint8(this, nOpcode))  // Extended opcode
						return false;
					if(!ExecuteExtendedOpcode(address, nOpcode, nOperandU64))
						return false;
					break;

				case DW_LNS_COPY:
					AppendMatrixRow(address);
					break;

				case DW_LNS_ADVANCE_PC:
					if(!ReadULEB128(&nOperandU64))
						return false;
					mNextState.mnAddress += (nOperandU64 * mPrologue.mnMinimumInstructionLength);
					break;

				case DW_LNS_ADVANCE_LINE:
					if(!ReadSLEB128(&nOperandS64))
						return false;
					mNextState.mnLine += nOperandS64;
					break;

				case DW_LNS_SET_FILE:
					if(!ReadULEB128(&nOperandU64))
						return false;
					mNextState.mnFile = nOperandU64;
					break;

				case DW_LNS_SET_COLUMN:
					if(!ReadULEB128(&nOperandU64))
						return false;
					break;

				case DW_LNS_NEGATE_STMT:
				case DW_LNS_SET_BASIC_BLOCK:
					break;

				case DW_LNS_CONST_ADD_PC:
					mNextState.mnAddress += (255 - mPrologue.mnOpcodeBase) / mPrologue.mnLineRange * mPrologue.mnMinimumInstructionLength;
					break;

				case DW_LNS_FIXED_ADVANCE_PC:
					if(!IO::ReadUint16(this, nOperandU16, mpDWARF3->mEndian))
						return false;
					mNextState.mnAddress += nOperandU16;
					break;

				 case DW_LNS_SET_PROLOGUE_END:
				 case DW_LNS_SET_EPILOGUE_BEGIN:
					 break;
				 case DW_LNS_SET_ISA:
					if(!ReadULEB128(&nOperandU64))
						return false;
					// information not used 
					break;

				default:
					// should never happen as special opcodes are handled above because of the complexities of opcode base
					// see page 114 dwarf4 "opcode_base"
					return false;
			}
		}

		if(mbFound)
		{
			*pLineMachineStateOut = mCurrState;
			return true;
		}
	}

	return false;
}

///////////////////////////////////////////////////////////////////////////////
// LineSectionReader::ExecuteSpecialOpcode
//
bool LineSectionReader::ExecuteSpecialOpcode(uint64_t address, uint8_t nSpecialOpcode)
{
	if(nSpecialOpcode < mPrologue.mnOpcodeBase) 
	{
		// Based on the DWARF standard (as of v4) this above condition will never be hit
		// and this code will never run.
		// The definition for the code below has no explanation in the dwarf format
		
		
		 
		// Handle opcode extensions; look up the operand count and skip it.
		uint8_t      nOperandCount;
		IO::off_type nSavedStreamPosition = GetPosition();

		//   0              : Seek to beginning of stream where prologue header resides
		// + 11             : Plus offset of opcode length (ubyte) array
		// + nSpecialOpcode : Plus offset into opcode length array
		// - 1              : Less 1 because first element of opcode length array corresponds to opcode whose value is 1
		IO::off_type nOperandCountStreamPosition = (0 + 11 + nSpecialOpcode - 1);

		if(!SetPosition(nOperandCountStreamPosition, IO::kPositionTypeCurrent))
			return false;
		if(!IO::ReadUint8(this, nOperandCount))
			return false;
		if(!SetPosition(nSavedStreamPosition))
			return false;

		uint64_t nTemp64;

		// Skip the operands for the opcode.
		while(nOperandCount--)
		{
			if(!ReadULEB128(&nTemp64))
				return false;
		}
	}
	else
	{
		// See DWARF3 specification section 6.2.5.1, p. 54.
		// our code has the assumption throughout that the
		// maximum_operations_per_instruction is always 1 which means op_index is always zero
		uint8_t adjustedOpCode = (uint8_t)(unsigned)(nSpecialOpcode - mPrologue.mnOpcodeBase);
		uint8_t opAdvance = adjustedOpCode / mPrologue.mnLineRange;

		mNextState.mnAddress += (opAdvance) * mPrologue.mnMinimumInstructionLength;
		mNextState.mnLine    += mPrologue.mnLineBase + (adjustedOpCode % mPrologue.mnLineRange);

		AppendMatrixRow(address);
	}

	return true;
}


///////////////////////////////////////////////////////////////////////////////
// LineSectionReader::ExecuteExtendedOpcode
//
bool LineSectionReader::ExecuteExtendedOpcode(uint64_t address, uint8_t nExtendedOpcode, uint64_t nInstructionSize)
{
	uint8_t  nTemp8;
	uint32_t nTemp32;
	uint64_t nTemp64;

	switch(nExtendedOpcode)
	{
		case DW_LNE_END_SEQUENCE:
			AppendMatrixRow(address);

			// Reset line machine to initial state.
			mNextState.mnAddress = 0;
			mNextState.mnFile = 1;
			mNextState.mnLine = 1;
			break;

		case DW_LNE_SET_ADDRESS:
			switch(nInstructionSize - 1)
			{
				case 4:
					if(!IO::ReadUint32(this, nTemp32, mpDWARF3->mEndian))
						return false;
					nTemp64 = nTemp32;
					break;

				case 8:
					if(!IO::ReadUint64(this, nTemp64, mpDWARF3->mEndian))
						return false;
					break;

				default:
					// Unhandled ptr size.
					return false;
			}

			mNextState.mnAddress = nTemp64;
			break;

		case DW_LNE_DEFINE_FILE:
			// Skip the file name operand.
			do
			{
				if(!IO::ReadUint8(this, nTemp8))
					return false;
			}
			while(nTemp8);

			if(!ReadULEB128(&nTemp64)) // Index into include directories array.
				return false;
			if(!ReadULEB128(&nTemp64)) // Last modification time for file.
				return false;
			if(!ReadULEB128(&nTemp64)) // Length of file in bytes.
				return false;
			break;
		case DW_LNE_SET_DISCRIMINATOR:
			if(!ReadULEB128(&nTemp64)) // (not used)
				return false;
			break;

		default:
			// I dont know what the point of the code below was but isnt refered to in the dwarf standard
			if(!SetPosition((IO::off_type)nInstructionSize, IO::kPositionTypeCurrent))
				return false;
			break;
	}

	return true;
}


///////////////////////////////////////////////////////////////////////////////
// LineSectionReader::AppendMatrixRow
//
void LineSectionReader::AppendMatrixRow(uint64_t address)
{
	if(address == mNextState.mnAddress)
	{
		// Exact match.
		mbFound = true;
		mCurrState = mNextState;
	}
	else if(mCurrState.mbInitialized &&
			address >= mCurrState.mnAddress &&
			address <  mNextState.mnAddress)
	{
		// Target address is in the range of this instruction sequence;
		// the machine state is not updated so that the state recorded
		// at the beginning of the sequence is returned.
		mbFound = true;
	}
	else
		mCurrState = mNextState;
}



} // namespace Callstack
} // namespace EA


#endif // EACALLSTACK_DWARFFILE_ENABLED





