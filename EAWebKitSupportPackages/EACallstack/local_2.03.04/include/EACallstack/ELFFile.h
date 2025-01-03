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
//     http://www.freebsd.org/cgi/man.cgi?query=elf
//
///////////////////////////////////////////////////////////////////////////////


#ifndef EACALLSTACK_ELFFILE_H
#define EACALLSTACK_ELFFILE_H


#include <EACallstack/internal/Config.h>
#include <EACallstack/internal/ELF.h>
#include <EAIO/EAStream.h>


namespace EA
{
namespace Callstack
{
	class EACALLSTACK_API ELFFile
	{
	public:
		/// ELFFile
		///
		ELFFile();

		/// Init
		///
		/// Get the ELF file header and initialize section reading. The
		/// IStream should be opened with the ELF file to be parsed.
		///
		bool Init(IO::IStream*);

		/// GetSectionCount
		///
		/// Get the total number of section in the file.
		///
		int  GetSectionCount();

		/// FindSectionHeaderByIndex
		///
		/// Look up the section, specified by index, and return its info
		/// in the given section header object.
		///
		bool FindSectionHeaderByIndex(int nSectionIndex, ELF::ELF_Shdr* pSectionHeaderOut);

		/// FindSectionHeaderByName
		///
		/// Look up the section, specified by name, and return its info
		/// in the given section header object.
		///
		bool FindSectionHeaderByName(const char* pSectionName, ELF::ELF_Shdr* pSectionHeaderOut);

	public:
		bool ReadFileHeader(ELF::ELF_Ehdr* pFileHeaderOut);
		bool ReadSectionHeader(ELF::ELF_Shdr* pSectionHeaderOut);
		bool SectionNameMatches(const char* pSectionName);

	public:
		bool            mbInitialized;
		int             mnClass;
		IO::Endian      mEndian;        /// The endian-ness of the ELF file, as determined by the EI_DATA field.
		IO::IStream*    mpStream;       /// The file stream for the ELF file. This is set externally; we do not allocate it.
		uint32_t        mELFOffset;     /// Offset to the ELF information in the stream.
		ELF::ELF_Ehdr   mHeader;        /// Note that this is a 64 bit header, and we use it for 32 bit ELFs as well.

	}; // class ELFFile

} // namespace Callstack
} // namespace EA


#endif // EACALLSTACK_ELFFILE_H
