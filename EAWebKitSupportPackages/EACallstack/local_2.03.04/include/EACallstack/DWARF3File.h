/////////////////////////////////////////////////////////////////////////////
// Copyright (c) Electronic Arts Inc. All rights reserved.
/////////////////////////////////////////////////////////////////////////////

///////////////////////////////////////////////////////////////////////////////
//
// This a DWARF3 reader designed for gleaning basic symbol information such
// as the function name, line number, and source file information from the
// DWARF3 symbol data stored in an ELF file.
//
// The canonical DWARF format specification can be found here:
//     http://dwarfstd.org/
// Recommended reading:
//     Introduction to the DWARF Debugging Format
//     by Michael J. Eager, Eager Consulting
//     http://www.dwarfstd.org/Debugging%20using%20DWARF.pdf
//
///////////////////////////////////////////////////////////////////////////////


#ifndef EACALLSTACK_DWARF3FILE_H
#define EACALLSTACK_DWARF3FILE_H


#include <EACallstack/internal/Config.h>

#if EACALLSTACK_DWARFFILE_ENABLED

#include <EACallstack/internal/EASTLCoreAllocator.h>
#include <EACallstack/EAAddressRep.h>
#include <EACallstack/ELFFile.h>
#include <EASTL/fixed_string.h>
#include <EASTL/map.h>
#include <EASTL/fixed_vector.h>
#include <EAIO/EAFileStream.h>
#include <EAIO/EAStreamBuffer.h>
#include <EAIO/EAStreamMemory.h>
#include <coreallocator/icoreallocator_interface.h>


EA_DISABLE_VC_WARNING(4251); // class (some template) needs to have dll-interface to be used by clients.


namespace EA
{
namespace Callstack
{
	// Forward declarations
	class DWARF3File;
	class AbbrevSectionEntry;
	class FileLineInfo;



	// enum DwarfLanguage
	// {
	//     DW_LANG_C89             = 0x0001,
	//     DW_LANG_C               = 0x0002,
	//     DW_LANG_Ada83           = 0x0003,
	//     DW_LANG_C_plus_plus     = 0x0004,
	//     DW_LANG_Cobol74         = 0x0005,
	//     DW_LANG_Cobol85         = 0x0006,
	//     DW_LANG_Fortran77       = 0x0007,
	//     DW_LANG_Fortran90       = 0x0008,
	//     DW_LANG_Pascal83        = 0x0009,
	//     DW_LANG_Modula2         = 0x000a,
	//     DW_LANG_Java            = 0x000b,
	//     DW_LANG_C99             = 0x000c,
	//     DW_LANG_Ada95           = 0x000d,
	//     DW_LANG_Fortran95       = 0x000e,
	//     DW_LANG_Mips_Assembler  = 0x8001
	// };


	enum AttributeFlags
	{
		ATTR_NAME             =  1, // 0x01, 1<<0   The DW_AT_NAME attribute is present.
		ATTR_HIGH_PC          =  2, // 0x02, 1<<1   The DW_AT_HIGH_PC attribute is present.
		ATTR_LOW_PC           =  4, // 0x04, 1<<2   The DW_AT_LOW_PC attribute is present.
		ATTR_STMT_LIST        =  8, // 0x08, 1<<3   The DW_AT_STMT_LIST attribute is present.
		ATTR_ABSTRACT_ORIGIN  = 16, // 0x10, 1<<4   The DW_AT_ABSTRACT_ORIGIN attribute is present.
		ATTR_SPECIFICATION    = 32  // 0x20, 1<<5   The DW_AT_SPECIFICATION attribute is present.
	};


	/// DwarfForm
	/// See the DWARF3 Standard, section 7.5.4
	enum DwarfForm
	{
		DW_FORM_NONE			= 0x00,
		DW_FORM_ADDR			= 0x01,
		DW_FORM_BLOCK2			= 0x03,
		DW_FORM_BLOCK4			= 0x04,
		DW_FORM_DATA2			= 0x05,
		DW_FORM_DATA4			= 0x06,
		DW_FORM_DATA8			= 0x07,
		DW_FORM_STRING			= 0x08,       // String represented immediately in the debug information.
		DW_FORM_BLOCK			= 0x09,
		DW_FORM_BLOCK1			= 0x0A,
		DW_FORM_DATA1			= 0x0B,
		DW_FORM_FLAG			= 0x0C,
		DW_FORM_SDATA			= 0x0D,
		DW_FORM_STRP			= 0x0E,       // String at a given offset into a string table contained in the .debug_str section.
		DW_FORM_UDATA			= 0x0F,
		DW_FORM_REF_ADDR		= 0x10,       // Something at a given offset in the .debug_info section.
		DW_FORM_REF1			= 0x11,
		DW_FORM_REF2			= 0x12,
		DW_FORM_REF4			= 0x13,
		DW_FORM_REF8			= 0x14,
		DW_FORM_REF_UDATA		= 0x15,
		DW_FORM_INDIRECT		= 0x16,
		DW_FORM_SEC_OFFSET		= 0x17,     // DWARF 3 starts here
		DW_FORM_EXPRLOC			= 0x18,
		DW_FORM_FLAG_PRESENT	= 0x19,
		DW_FORM_REF_SIG_8		= 0x20,
	};

	enum AbbrevTag
	{   // To do: Convert these to capitals in order to match the rest of this library.
		DW_TAG_padding                  = 0x0000,
		DW_TAG_array_type               = 0x0001,
		DW_TAG_class_type               = 0x0002,
		DW_TAG_entry_point              = 0x0003,
		DW_TAG_enumeration_type         = 0x0004,
		DW_TAG_formal_parameter         = 0x0005,
		DW_TAG_imported_declaration     = 0x0008,
		DW_TAG_label                    = 0x000a,
		DW_TAG_lexical_block            = 0x000b,
		DW_TAG_member                   = 0x000d,
		DW_TAG_pointer_type             = 0x000f,
		DW_TAG_reference_type           = 0x0010,
		DW_TAG_compile_unit             = 0x0011,           // A compiled source file; e.g. .cpp file.
		DW_TAG_string_type              = 0x0012,
		DW_TAG_structure_type           = 0x0013,
		DW_TAG_subroutine_type          = 0x0015,
		DW_TAG_typedef                  = 0x0016,           // Standard C/C++ typedef.
		DW_TAG_union_type               = 0x0017,
		DW_TAG_unspecified_parameters   = 0x0018,
		DW_TAG_variant                  = 0x0019,
		DW_TAG_common_block             = 0x001a,
		DW_TAG_common_inclusion         = 0x001b,
		DW_TAG_inheritance              = 0x001c,
		DW_TAG_inlined_subroutine       = 0x001d,           // A particular inlined instance of a subroutine or function.
		DW_TAG_module                   = 0x001e,
		DW_TAG_ptr_to_member_type       = 0x001f,
		DW_TAG_set_type                 = 0x0020,
		DW_TAG_subrange_type            = 0x0021,
		DW_TAG_with_stmt                = 0x0022,
		DW_TAG_access_declaration       = 0x0023,
		DW_TAG_base_type                = 0x0024,           // A base type is a data type that is not defined in terms of other data types. e.g. int, char, float.
		DW_TAG_catch_block              = 0x0025,
		DW_TAG_const_type               = 0x0026,
		DW_TAG_constant                 = 0x0027,
		DW_TAG_enumerator               = 0x0028,
		DW_TAG_file_type                = 0x0029,
		DW_TAG_friend                   = 0x002a,
		DW_TAG_namelist                 = 0x002b,
		DW_TAG_namelist_item            = 0x002c,
		DW_TAG_packed_type              = 0x002d,
		DW_TAG_subprogram               = 0x002e,           // A global or file-static subroutine or function.
		DW_TAG_template_type_parameter  = 0x002f,
		DW_TAG_template_value_parameter = 0x0030,
		DW_TAG_thrown_type              = 0x0031,
		DW_TAG_try_block                = 0x0032,
		DW_TAG_variant_part             = 0x0033,
		DW_TAG_variable                 = 0x0034,
		DW_TAG_volatile_type            = 0x0035,
		DW_TAG_dwarf_procedure          = 0x0036,
		DW_TAG_restrict_type            = 0x0037,
		DW_TAG_interface_type           = 0x0038,
		DW_TAG_namespace                = 0x0039,
		DW_TAG_imported_module          = 0x003a,
		DW_TAG_unspecified_type         = 0x003b,
		DW_TAG_partial_unit             = 0x003c,
		DW_TAG_imported_unit            = 0x003d,
		DW_TAG_condition                = 0x003f,
		DW_TAG_shared_type              = 0x0040,
		DW_TAG_lo_user                  = 0x4080,
		DW_TAG_hi_user                  = 0xffff
	};

	// DwarfAttribute
	// See the DWARF3 Standard, section 7.5.4 for a table of each of these.
	// See the DWARF3 Standard, section 3.1.1 for a further description of some of these.
	//
	enum DwarfAttribute                         // Form classes                     Description
	{                                           // --------------------------------------------------------------------
		DW_AT_NONE                      = 0x00,
		DW_AT_SIBLING                   = 0x01, // reference                        Location of next sibling, so you can skip past reading children.
		DW_AT_LOCATION                  = 0x02, // block, loclistptr
		DW_AT_NAME                      = 0x03, // string                           Path name of compilation unit source, or name of declaration.
		DW_AT_ORDERING                  = 0x09, // constant
		DW_AT_BYTE_SIZE                 = 0x0b, // block, constant, reference
		DW_AT_BIT_OFFSET                = 0x0c, // block, constant, reference
		DW_AT_BIT_SIZE                  = 0x0d, // block, constant, reference
		DW_AT_STMT_LIST                 = 0x10, // lineptr                          Line number information for the compilation unit.
		DW_AT_LOW_PC                    = 0x11, // address
		DW_AT_HIGH_PC                   = 0x12, // address
		DW_AT_LANGUAGE                  = 0x13, // constant                         Programming language. One of enum DwarfLanguage.
		DW_AT_DISCR                     = 0x15, // reference
		DW_AT_DISCR_VALUE               = 0x16, // constant
		DW_AT_VISIBILITY                = 0x17, // constant
		DW_AT_IMPORT                    = 0x18, // reference
		DW_AT_STRING_LENGTH             = 0x19, // block, loclistptr                String length of string type
		DW_AT_COMMON_REFERENCE          = 0x1a, // reference
		DW_AT_COMP_DIR                  = 0x1b, // string                           Compilation directory
		DW_AT_CONST_VALUE               = 0x1c, // block, constant, string
		DW_AT_CONTAINING_TYPE           = 0x1d, // reference
		DW_AT_DEFAULT_VALUE             = 0x1e, // reference
		DW_AT_INLINE                    = 0x20, // constant
		DW_AT_IS_OPTIONAL               = 0x21, // flag
		DW_AT_LOWER_BOUND               = 0x22, // block, constant, reference
		DW_AT_PRODUCER                  = 0x25, // string                           Compiler identification
		DW_AT_PROTOTYPED                = 0x27, // flag
		DW_AT_RETURN_ADDR               = 0x2a, // block, loclistptr
		DW_AT_START_SCOPE               = 0x2c, // constant
		DW_AT_STRIDE_SIZE               = 0x2e, // constant
		DW_AT_UPPER_BOUND               = 0x2f, // block, constant, reference
		DW_AT_ABSTRACT_ORIGIN           = 0x31, // reference                        Inline instances of inline subprograms; Out-of-line instances of inline subprograms.
		DW_AT_ACCESSIBILITY             = 0x32, // constant
		DW_AT_ADDRESS_CLASS             = 0x33, //
		DW_AT_ARTIFICIAL                = 0x34, // flag
		DW_AT_BASE_TYPES                = 0x35, //
		DW_AT_CALLING_CONVENTION        = 0x36, //
		DW_AT_COUNT                     = 0x37, //
		DW_AT_DATA_MEMBER_LOCATION      = 0x38, // block, constant, loclistptr
		DW_AT_DECL_COLUMN               = 0x39, //
		DW_AT_DECL_FILE                 = 0x3a, // constant                         File containing source declaration.
		DW_AT_DECL_LINE                 = 0x3b, // constant                         Line containing source declaration.
		DW_AT_DECLARATION               = 0x3c, // flag
		DW_AT_DISCR_LIST                = 0x3d, //
		DW_AT_ENCODING                  = 0x3e, // constant
		DW_AT_EXTERNAL                  = 0x3f, // flag
		DW_AT_FRAME_BASE                = 0x40, // block, loclistptr
		DW_AT_FRIEND                    = 0x41, // reference
		DW_AT_IDENTIFIER_CASE           = 0x42, //
		DW_AT_MACRO_INFO                = 0x43, //
		DW_AT_NAMELIST_ITEM             = 0x44, //
		DW_AT_PRIORITY                  = 0x45, //
		DW_AT_SEGMENT                   = 0x46, //
		DW_AT_SPECIFICATION             = 0x47, // reference                        Incomplete, non-defining, or separate declaration corresponding to a declaration.
		DW_AT_STATIC_LINK               = 0x48, //
		DW_AT_TYPE                      = 0x49, // reference                        Type of declaration; type of subroutine return. e.g. uint8_t
		DW_AT_USE_LOCATION              = 0x4a, //
		DW_AT_VARIABLE_PARAMETER        = 0x4b, //
		DW_AT_VIRTUALITY                = 0x4c, // constant
		DW_AT_VTABLE_ELEM_LOCATION      = 0x4d, // block, loclistptr
		DW_AT_ALLOCATED                 = 0x4e, // block, constant, reference
		DW_AT_ASSOCIATED                = 0x4f, // block, constant, reference
		DW_AT_DATA_LOCATION             = 0x50, // block
		DW_AT_BYTE_STRIDE               = 0x51, // block, constant, reference
		DW_AT_ENTRY_PC                  = 0x52, // address                              Entry address of module initialization.
		DW_AT_USE_UTF8                  = 0x53, // flag
		DW_AT_EXTENSION                 = 0x54, // reference
		DW_AT_RANGES                    = 0x55, // rangelistptr
		DW_AT_TRAMPOLINE                = 0x56, // address, flag, reference, string     Target subroutine
		DW_AT_CALL_COLUMN               = 0x57, // constant
		DW_AT_CALL_FILE                 = 0x58, // constant
		DW_AT_CALL_LINE                 = 0x59, // constant
		DW_AT_DESCRIPTION               = 0x5a, // string
		DW_AT_BINARY_SCALE              = 0x5b, // constant
		DW_AT_DECIMAL_SCALE             = 0x5c, // constant
		DW_AT_SMALL                     = 0x5d, // reference
		DW_AT_DECIMAL_SIGN              = 0x5e, // constant
		DW_AT_DIGIT_COUNT               = 0x5f, // constant
		DW_AT_PICTURE_STRING            = 0x60, // string
		DW_AT_MUTABLE                   = 0x61, // flag
		DW_AT_THREADS_SCALED            = 0x62, // flag
		DW_AT_EXPLICIT                  = 0x63, // flag
		DW_AT_OBJECT_POINTER            = 0x64, // reference
		DW_AT_ENDIANITY                 = 0x65, // constant
		DW_AT_ELEMENTAL                 = 0x66, // flag
		DW_AT_PURE                      = 0x67, // flag

		DW_AT_LO_USER                   = 0x00002000,
		DW_AT_HI_USER                   = 0x0003ffff
	};

	enum DwarfLineNumberStandard
	{
		DW_LNS_EXTENDED_OPCODE        = 0x0,
		DW_LNS_COPY                   = 0x1,
		DW_LNS_ADVANCE_PC             = 0x2,
		DW_LNS_ADVANCE_LINE           = 0x3,
		DW_LNS_SET_FILE               = 0x4,
		DW_LNS_SET_COLUMN             = 0x5,
		DW_LNS_NEGATE_STMT            = 0x6,
		DW_LNS_SET_BASIC_BLOCK        = 0x7,
		DW_LNS_CONST_ADD_PC           = 0x8,
		DW_LNS_FIXED_ADVANCE_PC       = 0x9,
		DW_LNS_SET_PROLOGUE_END       = 0xa,  // DWARF 3 starts here
		DW_LNS_SET_EPILOGUE_BEGIN     = 0xb,
		DW_LNS_SET_ISA                = 0xc,
	};

	enum DwarfLineNumberExtended
	{
		DW_LNE_END_SEQUENCE           = 1,
		DW_LNE_SET_ADDRESS            = 2,
		DW_LNE_DEFINE_FILE            = 3,
		DW_LNE_SET_DISCRIMINATOR	  = 4, // dwarf 4 page 122
	};



	/// DWARF3SectionReader
	///
	/// This ia a base class for the individual section readers implemented privately
	/// here. A derived section reader determines its piece of the object file to load,
	/// loads it into the memory stream (this class), and uses the stream API to read it.
	///
	class EACALLSTACK_API DWARF3SectionReader : public IO::MemoryStream
	{
	public:
		DWARF3SectionReader();

		bool ReadSLEB128( int64_t* sleb128Out);
		bool ReadULEB128(uint64_t* uleb128Out);

		#ifdef EA_DEBUG
			bool ReadULEB128(AbbrevTag* pAbbrevTag);
			bool ReadULEB128(DwarfForm* pDwarfForm);
			bool ReadULEB128(DwarfAttribute* pDwarfAttribute);
		#endif

	protected:
		DWARF3File*  mpDWARF3;
		char*        mpStreamMem;
		uint32_t     mnStreamMemSize;
	};


	/// AddressRangeTableHeader
	/// Following this header in the file is an array of pair: { address, length }.
	/// See section 7.20 of the DWARF 2 Standard.
	struct AddressRangeTableHeader
	{
		uint32_t mnLength;                      // 4 byte length containing the length of the set of entries for this compilation unit, not including the length field itself.
		uint16_t mnVersion;                     // 2 byte version identifier containing the value 2 for DWARF Version 2.
		uint32_t mnInfoOffset;                  // 4 byte offset into the .debug_info section.
		uint8_t  mnPointerSize;                 // 1 byte unsigned integer containing the size in bytes of an address (or the offset portion of an address for segmented addressing) on the target system.
		uint8_t  mnSegmentDescriptorSize;       // 1 byte unsigned integer containing the size in bytes of a segment descriptor on the target system.
	};


	/// CompUnitInfo
	/// A compilation unit corresponds to a .obj file (and thus to a .cpp file).
	/// See section 7.5.1 of the DWARF 2 Standard.
	struct CompUnitInfo
	{
		uint32_t    mnLength;                   // 4 byte unsigned integer representing the length of the .debug_info contribution for the compilation unit, not including the length field itself.
		uint16_t    mnVersion;                  // 2 byte unsigned integer representing the version of the DWARF information for that compilation unit. For DWARF Version 2, the value in this field is 2.
		uint32_t    mnAbbrevOffset;             // 4 byte unsigned offset into the .debug_abbrev section. This offset associates the compilation unit with a particular set of debugging information entry abbreviations.
		uint8_t     mnPointerSize;              // 1 byte unsigned integer representing the size in bytes of an address on the target architecture. If the system uses segmented addressing, this value represents the size of the offset portion of an address.
		// Additional data we generate:
		uint32_t    mnPositionInInfoSection;    // This is an absolute ELF file position, but refers to a location within the file's debug info section.
		uint32_t    mnPositionInAbbrevSection;  // This is an absolute ELF file position, but refers to a location within the file's debug abbreviations section.
	};


	/// CompUnitReader
	///
	/// A reader for looking up the compilation unit containing the lookup address
	/// from the ".debug_aranges" section.
	///
	/// See the DWARF3 Standard, section 7.5.1 and 7.20.
	///
	class CompUnitReader : public DWARF3SectionReader
	{
	public:
		static const int kHeaderSize = 12;

		bool Init(DWARF3File*);
		bool FindUnitContainingAddress(uint64_t address, CompUnitInfo* pCompUnitInfoOut);
	};


	/// InfoSectionReader
	///
	/// A reader for reading the attribute data for a compilation unit from the
	/// ".debug_info" section.
	///
	/// See the DWARF3 Standard, section 6.1.2 and 7.5.4.
	///
	class InfoSectionReader : public DWARF3SectionReader
	{
	public:
		bool Init             (DWARF3File*);
		bool ReadAbbrevCode   (uint64_t* pnAbbrevCodeOut);
		bool ReadAddressForm  (uint64_t nFormType, uint64_t* pnFormOut);
		bool ReadConstantForm (uint64_t nFormType, uint64_t* pnFormOut);
		bool ReadReferenceForm(uint64_t nFormType, uint64_t* pnFormOut);
		bool ReadStringForm   (uint64_t nFormType, uint64_t* pnFormOut);
		bool SkipForm         (uint64_t nFormType);




		//(DW_AT_LOW_PC)  (DW_AT_HIGH_PC) or for DWARF3 we have (DW_AT_RANGES)



	/// RangesSectionReader
	///
	/// A reader for reading the attribute data for a compilation unit from the
	/// ".debug_ranges" section.
	///
	/// See the DWARF3 Standard, section 6.1.2 and 7.5.4.
	///
	}; // class InfoSectionReader


	/// AbbrevSectionEntry
	///
	/// These sections are hierarchical. For example, you still often see a hierarchy like so:
	///     DW_TAG_compile_unit
	///         DW_TAG_subprogram
	///             DW_TAG_variable
	///
	/// Each entry has properties, and the properties of interest to us are represented as
	/// variable names of the AbbrevSectionEntry class. It turns out that some properties
	/// are inherited from their parents. For example, a DW_AT_STMT_LIST (file/line info)
	/// property of a compilation unit (e.g. .cpp file) may apply to the DW_TAG_subprogram
	/// (function) and the function doesn't have its own unique DW_AT_STMT_LIST property.
	/// So you at the DW_AT_STMT_LIST of the parent compilation unit (DW_TAG_compile_unit).
	///
	class EACALLSTACK_API AbbrevSectionEntry
	{
	public:
		AbbrevSectionEntry();

		bool ContainsAddress(uint64_t address) const;
		bool HasAnyAttribute(int nAttributes) const { return (mnAttributes & nAttributes) != 0; }
		void Clear();
		void CopyFrom(const AbbrevSectionEntry* pEntry);



	public:
		uint64_t    mnAbbrevCode;           // This is an id assigned by the compiler/linker. It is usually just an ever incrementing number.
		uint32_t    mnAttributes;           // Flags from enum AttributeFlags. e.g. (ATTR_NAME | ATTR_LOW_PC | ATTR_HIGH_PC)
		AbbrevTag   mnTag;                  // Defines the debug info entry type (DIE type). For example, DW_TAG_compile_unit
		uint8_t     mbHasChildren;          // True if this entry has children (as typically a subprogram or namespace would but typedef wouldn't).
		uint64_t    mnNameOffset;           // This is the position within the ELF file of the string. Usually it is within the main debug string set, but sometimes it points to a string elsewhere within the ELF file. (DW_AT_NAME)
		uint64_t    mnStmtList;             // File/line information. Note that zero is a valid value and may be seen for the first compilation unit. This value may be inherited by the parent entry. (DW_AT_STMT_LIST)
		uint64_t    mnLowPC;                // Lowest address that this section of information refers to. (DW_AT_LOW_PC)
		uint64_t    mnHighPC;               // Highest address that this section of information refers to. (DW_AT_HIGH_PC)
		uint64_t    mnAbstractOrigin;       // Inline instances of inline subprograms or out-of-line instances of inline subprograms. (DW_AT_ABSTRACT_ORIGIN)
		uint64_t    mnSpecification;        // The reference given by DW_AT_SPECIFICATION is an offset within the current compilation unit. (DW_AT_SPECIFICATION)

	}; // class AbbrevSectionEntry


	/// AbbrevSectionReader
	///
	/// A reader for reading the abbreviation entries of a compilation unit contained in
	/// the ".debug_abbrev" section. In short, an abbreviation entry decribes how the data
	/// in the ".debug_info" section should be interpreted for a compilation unit. Abbrev
	/// entries can be referenced multiple times, thereby achieving data compression.
	///
	/// See the DWARF3 Standard, sec 7.5.3.
	///
	class EACALLSTACK_API AbbrevSectionReader : public DWARF3SectionReader
	{
	public:
		bool Init(DWARF3File*, InfoSectionReader*, bool bCacheEntries, EA::Allocator::ICoreAllocator* pAllocator);
		bool FindEntryByCode(uint64_t nAbbrevCode, AbbrevSectionEntry* pAbbrevEntryOut);

	protected:
		bool ReadAbbreviationSectionEntry(AbbrevSectionEntry* pAbbrevEntryOut);
		bool SkipAbbreviationSectionEntry();

	protected:
		// Defines a map of  uint64_t abbrevCode to off_type abbrev section position.
		typedef eastl::hash_map<uint64_t, EA::IO::off_type, eastl::hash<uint64_t>,
								eastl::equal_to<uint64_t>, EA::Callstack::EASTLCoreAllocator> EntryCache;

		InfoSectionReader* mpInfoSectionReader;
		EntryCache         mEntryCache;         // Cached entries
		EA::IO::off_type   mFurthestPosition;   // Used for saving our position between FindEntryByCode calls.
		bool               mbCacheEntries;      // Enables caching.

	}; // class AbbrevSectionReader


	/// FileLineInfo
	///
	class FileLineInfo
	{
	public:
		FixedString msSourceFileName;
		FixedString msSourceFileDirectory;
		uint64_t    mnLastModifiedTime;
		uint64_t    mnLengthInBytes;
		uint64_t    mnLine;
	};



	/// LineSectionReader
	///
	/// A reader for looking up the file name and line number for a program address.
	/// Line number information in DWARF3 is conceptually stored in a matrix, with a row
	/// for each emitted instruction and columns for the source file name, line number,
	/// column, etc. In practice, however, the matrix is stored as a "program" with opcodes
	/// describing how to rebuild the matrix.
	///
	/// See the DWARF3 specification, sec 6.2, p 50.
	///
	class EACALLSTACK_API LineSectionReader : public DWARF3SectionReader
	{
	public:
		bool Init(DWARF3File*, AbbrevSectionEntry* pAbbrevSectionEntry);
		bool FindFileLineInfoForAddress(uint64_t address, FileLineInfo* pFileLineInfo);

	protected:
		struct ProgramPrologue
		{
			uint32_t mnTotalLength;
			uint16_t mnVersion;
			uint32_t mnPrologueLength;
			uint8_t  mnMinimumInstructionLength;
			uint8_t  mnMaximumOperationsPerInstruction;
			uint8_t  mnDefaultIsStmt;
			int8_t   mnLineBase;
			uint8_t  mnLineRange;
			uint8_t  mnOpcodeBase;

		};

		struct LineMachineState
		{
			uint64_t    mnAddress;
			uint64_t    mnFile;
			uint64_t    mnLine;
			bool        mbInitialized;
		};

		bool ExecuteLineProgram   (uint64_t address, LineMachineState* pLineMachineStateOut);
		bool ExecuteSpecialOpcode (uint64_t address, uint8_t nSpecialOpcode);
		bool ExecuteExtendedOpcode(uint64_t address, uint8_t nExtendedOpcode, uint64_t nInstructionSize);
		void AppendMatrixRow      (uint64_t address);

	protected:
		ProgramPrologue  mPrologue;
		LineMachineState mCurrState;
		LineMachineState mNextState;
		bool             mbFound;
		int64_t          mnFileNameTableStart;

	}; // class LineSectionReader



	/// DWARF3File
	///
	/// Implements the IAddressRepLookup interface for DWARF3 records in a .elf file.
	/// DWARF3 records are the debug info records for binary executables found in .elf files.
	/// The GCC3 (and later) compilers use DWARF3 information to store debug info.
	///
	class EACALLSTACK_API DWARF3File : public IAddressRepLookup
	{
	public:
		/// kTypeId
		/// Defines what type of IAddressRepLookup this is. This is used in conjunction
		/// with the AsInterface function to safely dynamically cast a base class
		/// such as IAddressRepLookup to a subclass such as DWARF3File.
		static const int kTypeId = 0x061b670f; // This is just some unique value.

		DWARF3File(EA::Allocator::ICoreAllocator* pCoreAllocator = NULL);
	   ~DWARF3File();

		void SetAllocator(EA::Allocator::ICoreAllocator* pCoreAllocator);

		bool Init(const wchar_t* pDWARF3FilePath, bool bDelayLoad = true);
		bool Shutdown();

		void*           AsInterface(int typeId);
		void            SetBaseAddress(uint64_t baseAddress);
		uint64_t        GetBaseAddress() const;
		int             GetAddressRep(int addressRepTypeFlags, uint64_t address, FixedString* pRepArray, int* pIntValueArray);
		const wchar_t*  GetDatabasePath() const;
		void            SetOption(int option, int value);

	public:
		friend class CompUnitReader;
		friend class InfoSectionReader;
		friend class AbbrevSectionReader;
		friend class LineSectionReader;

		// this function expects the Comp unit to be initialized correctly before called
		bool GatherDIEForAbbrevSection(const AbbrevSectionEntry* pAbbrevSectionEntryIn);

		bool FindAbbrevSectionEntry(uint64_t address, AbbrevSectionEntry* pAbbrevSectionEntryOut);
		bool GetFunctionName(const AbbrevSectionEntry* pAbbrevSectionEntry, FixedString* pFunctionNameOut, bool prependNamespace = false);
		bool GetSourceFilePath(const FileLineInfo* pFileLineInfo, FixedString* pSourceFilePathOut);

	protected:
		int GetAddressRepSpawn(int addressRepTypeFlags, uint64_t nAddress, FixedString* pRepArray, int* pIntValueArray);

	protected:
		EA::Allocator::ICoreAllocator* mpCoreAllocator;     ///
		FixedStringW                   msDwarfFilePath;     ///
		EA::IO::FileStream             mFileStream;         /// Disk-based source of file data.
		EA::IO::MemoryStream           mMemoryStream;       /// Memory-based source of file data.
		EA::IO::IStream*               mpStream;            /// Points to either mFileStream or mMemoryStream.
		ELFFile                        mELFFile;            ///
		uint32_t                       mELFOffset;          /// Offset to the ELF information in the stream.
		IO::Endian                     mEndian;             /// The endian-ness of the ELF file, as determined by the EI_DATA field.
		bool                           mbInitialized;       ///
		bool                           mbUseMemoryFile;     /// If true then we attempt to copy the entire .elf file to memory instead of using it on disk. This requires a bit of system memory but can result in significantly faster performance.
		bool                           mbEnableCache;       /// If true then we cache tables as we go. This uses memory be increases performance.
		bool                           mbOfflineLookup;     /// True if we are to do offline address lookups, instead of lookups for the current app's addresses. Affects how we try to auto-determine module base addresses, which is important for relocatable DLLs on all platforms.
		int                            mnDebugArangesStart; /// Position within ELF of .debug_aranges data (address ranges).
		int                            mnDebugArangesEnd;   ///
		int                            mnDebugInfoStart;    /// Position within ELF of .debug_info data.
		int                            mnDebugAbbrevStart;  /// Position within ELF of .debug_abbrev data.
		int                            mnDebugAbbrevEnd;    ///
		int                            mnDebugLineStart;    /// Position within ELF of .debug_line data.
		int                            mnDebugStrStart;     /// Position within ELF of .debug_str data.
		bool                           mbRelocated;         /// True if this module is a relocatable module like a DLL.
		uint64_t                       mBaseAddress;        /// Address that executable is offset to. Usually this is zero unless we are dealing with a dynamic library.
		uint64_t                       mTextSegmentSize;    /// Combined with mBaseAddress, tells you the address range of the code.
		CompUnitReader                 mCompUnitReader;     /// Class scope so that CU reader can be re-used across multiple address lookups
		CompUnitInfo                   mCompUnitInfo;       /// The compilation unit containing the symbol address being looked up

		// DebugInformationEntry, often referred to as DIE.
		// The DWARF data (i.e. the debug data) in an ELF file is a tree
		// of DebugInformationEntries. The tree is layed out on disk in depth-first
		// form. What we do here is define what the current path is from the root
		// to our currently iterated location, that way we can tell what our parents
		// are in our current location in the tree as we search it.
		struct DIE
		{
			DIE(AbbrevTag t = DW_TAG_padding, uint64_t n = 0, uint64_t l = 0, uint64_t h = 0) : mTag(t), mnNameOffset(n), mnLowPC(l), mnHighPC(h) {}

			AbbrevTag mTag;
			uint64_t  mnNameOffset; // To consider: store the actual string here instead.
			uint64_t  mnLowPC;
			uint64_t  mnHighPC;
		};

		eastl::fixed_vector<DIE, 8, true, EA::Callstack::EASTLCoreAllocator> mDIEStack;

	}; // class DWARF3File

} // namespace Callstack
} // namespace EA


EA_RESTORE_VC_WARNING();


#endif // EACALLSTACK_DWARFFILE_ENABLED


#endif // EACALLSTACK_DWARF3FILE_H
