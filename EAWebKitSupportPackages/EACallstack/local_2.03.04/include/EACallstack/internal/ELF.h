/////////////////////////////////////////////////////////////////////////////
// Copyright (c) Electronic Arts Inc. All rights reserved.
/////////////////////////////////////////////////////////////////////////////

///////////////////////////////////////////////////////////////////////////////
//
// This is a minimal implementation of an ELF32/64 reader with just enough
// functionality for obtaining section information, the minimum functionality
// required by a DWARF3 reader.
//
// A good source of information about the ELF format can be found in the
// FreeBSD file formats manual:
//     http://www.koders.com/cpp/fid58F621524F2DADC04E2712E00982A6A062214164.aspx
//     http://www.freebsd.org/cgi/man.cgi?query=elf
//
///////////////////////////////////////////////////////////////////////////////


#ifndef EACALLSTACK_INTERNAL_ELF_H
#define EACALLSTACK_INTERNAL_ELF_H


#include <EACallstack/internal/Config.h>
#include <EAIO/EAStream.h>


namespace EA
{
namespace Callstack
{
namespace ELF
{

// ELF32 primitive types
typedef uint32_t ELF32_Addr;
typedef uint16_t ELF32_Half;
typedef uint32_t ELF32_Off;
typedef int32_t  ELF32_Sword;
typedef uint32_t ELF32_Word;
typedef uint32_t ELF32_Size;
typedef uint8_t  ELF32_Quarter;


// ELF64 primitive types
typedef uint64_t ELF64_Addr;
typedef uint32_t ELF64_Half;
typedef uint64_t ELF64_Off;
typedef int64_t  ELF64_Sword;
typedef uint64_t ELF64_Word;
typedef uint64_t ELF64_Size;
typedef uint16_t ELF64_Quarter;
typedef uint64_t ELF64_Xword;
typedef int64_t  ELF64_Sxword;


// Defines flags for the ELF32_Shdr sh_flags field.
enum SectionHeaderFlags
{
	SHF_WRITE            = 0x00000001,  // Writable data during execution.
	SHF_ALLOC            = 0x00000002,  // Occupies memory during execution.
	SHF_EXECINSTR        = 0x00000004,  // Executable machine instructions.
	SHF_MERGE            = 0x00000010,  // Data in this section can be merged.
	SHF_STRINGS          = 0x00000020,  // Contains null terminated character strings.
	SHF_INFO_LINK        = 0x00000040,  // sh_info holds section header table index.
	SHF_LINK_ORDER       = 0x00000080,  // Preserve section ordering when linking.
	SHF_OS_NONCONFORMING = 0x00000100,  // OS specific processing required.
	SHF_GROUP            = 0x00000200,  // Member of a section group.
	SHF_TLS              = 0x00000400,  // Thread local storage section.
	SHF_MASKOS           = 0x0FF00000,  //
	SHF_MASKPROC         = 0xF0000000   // Processor-specific semantics.
};

// Identification entries
enum Ident
{
	EI_MAG0           = 0,
	EI_MAG1           = 1,
	EI_MAG2           = 2,
	EI_MAG3           = 3,
	EI_CLASS          = 4,
	EI_DATA           = 5,
	EI_VERSION        = 6,
	EI_OSABI          = 7,
	EI_ABIVERSION     = 8,
	EI_PAD            = 9,
	EI_NIDENT         = 16
};

// Class entries
enum Class
{
	ELFCLASS32        = 1,
	ELFCLASS64        = 2
};

enum Encoding
{
	ELFDATANONE       = 0,  /// Invalid data encoding.
	ELFDATA2LSB       = 1,  /// 2's complement, LSB at lowest byte address (Litte Endian).
	ELFDATA2MSB       = 2   /// 2's complement, MSB at lowest byte address (Big Endian).
};

enum OSABI
{
	ELFOSABI_NONE         =   0,    // UNIX System V ABI (Linux).

	ELFOSABI_LINUX        =   3,    // Linux. Not always used, but ELFOSABI_NONE is used instead.
	ELFOSABI_FREEBSD      =   9,    // FreeBSD.
	ELFOSABI_OPENBSD      =  12,    // OpenBSD.
	ELFOSABI_ARM          =  97,    // ARM
	ELFOSABI_STANDALONE   = 255     // Standalone (embedded) application
};

// Section indices
enum SectionIndex
{
	SHN_UNDEF         = 0,
	SHN_LORESERVE     = 0xFF00,
	SHN_LOPROC        = 0xFF00,
	SHN_HIPROC        = 0xFF1F,
	SHN_LOOS          = 0xFF20,
	SHN_HIOS          = 0xFF3F,
	SHN_ABS           = 0xFFF1,
	SHN_COMMON        = 0xFFF2,
	SHN_XINDEX        = 0xFFFF,
	SHN_HIRESERVE     = 0xFFFF
};

// Section types
enum SectionType
{
	SHT_NULL          = 0,
	SHT_PROGBITS      = 1,
	SHT_SYMTAB        = 2,
	SHT_STRTAB        = 3,
	SHT_RELA          = 4,
	SHT_HASH          = 5,
	SHT_DYNAMIC       = 6,
	SHT_NOTE          = 7,
	SHT_NOBITS        = 8,
	SHT_REL           = 9,
	SHT_SHLIB         = 10,
	SHT_DYNSYM        = 11,
	SHT_INIT_ARRAY    = 14,
	SHT_FINI_ARRAY    = 15,
	SHT_PREINIT_ARRAY = 16,
	SHT_GROUP         = 17,
	SHT_SYMTAB_SHNDX  = 18,
	SHT_LOOS          = 0x60000000,
	SHT_HIOS          = 0x6FFFFFFF,
	SHT_LOPROC        = 0x70000000,
	SHT_HIPROC        = 0x7FFFFFFF,
	SHT_LOUSER        = 0x80000000,
	SHT_HIUSER        = 0xFFFFFFFF
};


// ELF_Ehdr e_type values. See ELF32_Ehdr / ELF64_Ehdr.
enum ObjectFileType
{
	ET_NONE   = 0x0000,    // No file type
	ET_REL    = 0x0001,    // Relocatable file
	ET_EXEC   = 0x0002,    // Executable file
	ET_DYN    = 0x0003,    // Shared object file
	ET_CORE   = 0x0004,    // Core file
	ET_LOOS   = 0xFE00,    // Operating system-specific
	ET_HIOS   = 0xFEFF,    // Operating system-specific
	ET_LOPROC = 0xFF00,    // Processor-specific
	ET_HIPROC = 0xFFFF     // Processor-specific
};


// ELF_Ehdr e_machine values. There are numerous values defined but
// we only present the primary currently relevant ones to us here.
enum MachineType
{
	EM_NONE     =  0,    // No machine
	EM_386      =  3,    // Intel 80386
	EM_486      =  6,    // Intel 80486
	EM_MIPS     =  8,    // MIPS R3000 big-endian
	EM_PPC      = 20,    // PowerPC
	EM_PPC64    = 21,    // 64-bit PowerPC
	EM_SPU      = 23,    // Sony SPU
	EM_ARM      = 40,    // ARM
	EM_IA_64    = 50,    // Intel IA-64 Processor
	EM_X86_64   = 62     // Advanced Micro Devices X86-64 processor
};


// ELF64_Phdr::p_type values.
enum ProgramSegmentType
{
	PT_NULL         = 0,            // Program header table entry unused.
	PT_LOAD         = 1,            // Loadable program segment.
	PT_DYNAMIC      = 2,            // Dynamic linking information.
	PT_INTERP       = 3,            // Program interpreter.
	PT_NOTE         = 4,            // Auxiliary information.
	PT_SHLIB        = 5,            // Reserved.
	PT_PHDR         = 6,            // Entry for header table itself.
	PT_TLS          = 7,            // Thread-local storage segment.
	PT_NUM          = 8,            // Number of defined types.
	PT_LOOS         = 0x60000000,   // Start of OS-specific.
	PT_GNU_EH_FRAME = 0x6474e550,   // GCC .eh_frame_hdr segment.
	PT_GNU_STACK    = 0x6474e551,   // Indicates stack executability.
	PT_HIOS         = 0x6fffffff,   // End of OS-specific.
	PT_LOPROC       = 0x70000000,   // Start of processor-specific.
	PT_HIPROC       = 0x7fffffff    // End of processor-specific.
};


// Sony prepends a header to the .elf info in .self (signed elf) files.
struct SCE_Hdr
{
	unsigned char e_ident[4];
	uint32_t      e_unknown0;
	uint32_t      e_unknown1;
	uint32_t      e_unknown2;
	uint64_t      e_hdrsize;
}; // size = 24


// ELF32 header format
struct ELF32_Ehdr
{
	unsigned char e_ident[EI_NIDENT]; // ELF magic number (\x7f E L F).
	uint16_t      e_type;             // Identifies object file type. See enum ObjectFileType.
	uint16_t      e_machine;          // Specifies required architecture. See enum MachineType.
	uint32_t      e_version;          // Identifies object file version.
	uint32_t      e_entry;            // Entry point virtual address.         Field size differs from 64-bit
	uint32_t      e_phoff;            // Program header table file offset.    Field size differs from 64-bit
	uint32_t      e_shoff;            // Section header table file offset.    Field size differs from 64-bit
	uint32_t      e_flags;            // Processor-specific flags.
	uint16_t      e_ehsize;           // ELF header size in bytes.
	uint16_t      e_phentsize;        // Program header table entry size.
	uint16_t      e_phnum;            // Section header table entry size.
	uint16_t      e_shentsize;        // Section header table entry size.
	uint16_t      e_shnum;            // Section header table entry count.
	uint16_t      e_shstrndx;         // Section header string table index.
}; // size = 52

// ELF64 header format
// http://www.tachyonsoft.com/elf.txt
struct ELF64_Ehdr
{
	unsigned char e_ident[EI_NIDENT]; // ELF magic number (\x7f E L F).
	uint16_t      e_type;             // Identifies object file type. See enum ObjectFileType.
	uint16_t      e_machine;          // Specifies required architecture. See enum MachineType.
	uint32_t      e_version;          // Identifies object file version.
	uint64_t      e_entry;            // This member gives the virtual address to which the system first transfers control, thus starting the process. If the file has no associated entry point, this member holds zero. Field size differs from 32-bit
	uint64_t      e_phoff;            // This member holds the program header table's file offset in bytes. If the file has no program header table, this member holds zero. Field size differs from 32-bit
	uint64_t      e_shoff;            // This member holds the section header table's file off­ set in bytes. If the file has no section header table this member holds zero. Field size differs from 32-bit
	uint32_t      e_flags;            // This member holds processor-specific flags associated with the file. Flag names take the form EF_<machine_flag>.
	uint16_t      e_ehsize;           // This member holds the ELF header's size in bytes.
	uint16_t      e_phentsize;        // This member holds the size in bytes of one entry in the file's program header table; all entries are the same size.
	uint16_t      e_phnum;            // This member holds the number of entries in the program header table. Thus the product of e_phentsize and e_phnum gives the table's size in bytes. If a file has no program header, e_phnum holds the value zero.
	uint16_t      e_shentsize;        // This member holds a sections header's size in bytes. A section header is one entry in the section header table; all entries are the same size.
	uint16_t      e_shnum;            // This member holds the number of entries in the section header table. Thus the product of e_shentsize and e_shnum gives the section header table's size in bytes. If a file has no section header table, e_shnumholds the value of zero.
	uint16_t      e_shstrndx;         // This member holds the section header table index of the entry associated with the section name string table. If the file has no section name string table, this member holds the value SHN_UNDEF.
}; // size = 64

typedef ELF64_Ehdr ELF_Ehdr;

// ELF32 section header format
struct ELF32_Shdr
{
	uint32_t  sh_name;        // Section name, index in string table.
	uint32_t  sh_type;        // Type of section.
	uint32_t  sh_flags;       // Miscellaneous section attributes.    Field size differs from 64-bit
	uint32_t  sh_addr;        // Section virtual addr at execution.   Field size differs from 64-bit
	uint32_t  sh_offset;      // Section file offset.                 Field size differs from 64-bit
	uint32_t  sh_size;        // Size of section in bytes.            Field size differs from 64-bit
	uint32_t  sh_link;        // Index of another section.
	uint32_t  sh_info;        // Additional section information.
	uint32_t  sh_addralign;   // Section alignment.                   Field size differs from 64-bit
	uint32_t  sh_entsize;     // Entry size if section holds table.   Field size differs from 64-bit
}; // size = 40

// ELF64 section header format
struct ELF64_Shdr
{
	uint32_t  sh_name;        // Section name, index in string table.
	uint32_t  sh_type;        // Type of section.
	uint64_t  sh_flags;       // Miscellaneous section attributes.    Field size differs from 64-bit
	uint64_t  sh_addr;        // Section virtual addr at execution.   Field size differs from 64-bit
	uint64_t  sh_offset;      // Section file offset.                 Field size differs from 64-bit
	uint64_t  sh_size;        // Size of section in bytes.            Field size differs from 64-bit
	uint32_t  sh_link;        // Index of another section.
	uint32_t  sh_info;        // Additional section information.
	uint64_t  sh_addralign;   // Section alignment.                   Field size differs from 64-bit
	uint64_t  sh_entsize;     // Entry size if section holds table.   Field size differs from 64-bit
}; // size = 64

// The ELF file and section headers used by this library are actually
// the 64-bit versions, even when reading 32-bit ELF files.
// The 32-bit versions are coerced to the 64-bit versions,
// although the caller doesn't really need to know about this.
typedef ELF64_Shdr ELF_Shdr;

// ELF32 program header
struct ELF32_Phdr
{
	uint32_t p_type;
	uint32_t p_offset;
	uint32_t p_vaddr;
	uint32_t p_paddr;
	uint32_t p_filesz;
	uint32_t p_memsz;
	uint32_t p_flags;
	uint32_t p_align;
};  // Size = 32

// ELF64 program header
struct ELF64_Phdr
{
	uint32_t  p_type;             // See enum ProgramSegmentType.
	uint32_t  p_flags;            //
	uint64_t  p_offset;           // Segment file offset.
	uint64_t  p_vaddr;            // Segment virtual address.
	uint64_t  p_paddr;            // Segment physical address.
	uint64_t  p_filesz;           // Segment size in file.
	uint64_t  p_memsz;            // Segment size in memory.
	uint64_t  p_align;            // Segment alignment, file & memory.
}; // Size = 56



} // namespace ELF
} // namespace Callstack
} // namespace EA


#endif // EACALLSTACK_INTERNAL_ELF_H
