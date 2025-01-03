///////////////////////////////////////////////////////////////////////////////
// ELFFile.cpp
//
// Copyright (c) 2006 Electronic Arts Inc.
// Created by Paul Schlegel, developed and maintained by Paul Pedriana.
//
// This is a minimal implementation of an ELF32/64 reader with just enough
// functionality for obtaining section information, the minimum functionality
// required by a DWARF3 reader.
//
// A good source of information about the ELF format can be found in the
// FreeBSD file formats manual:
//
//     http://www.freebsd.org/cgi/man.cgi?query=elf
//
///////////////////////////////////////////////////////////////////////////////


#include <EACallstack/ELFFile.h>
#include <EAIO/EAStreamAdapter.h>
#include <EAStdC/EAString.h>
#include <EAStdC/EAEndian.h>

namespace EA
{
namespace Callstack
{


///////////////////////////////////////////////////////////////////////////////
// ELFFile
//
ELFFile::ELFFile()
  : mbInitialized(false),
	mnClass(0),
	mEndian(EA::IO::kEndianBig),
	mpStream(NULL),
	mELFOffset(0),
	mHeader()
{
}


///////////////////////////////////////////////////////////////////////////////
// Init
//
bool ELFFile::Init(IO::IStream* pStream)
{
	if(!mbInitialized)
	{
		mpStream = pStream;

		if(pStream)
			mbInitialized = ReadFileHeader(&mHeader);
	}

	return mbInitialized;
}


///////////////////////////////////////////////////////////////////////////////
// GetSectionCount
//
int ELFFile::GetSectionCount()
{
	if(mbInitialized && mHeader.e_shoff && mHeader.e_shentsize)
		return mHeader.e_shnum;

	return 0;
}


///////////////////////////////////////////////////////////////////////////////
// FindSectionHeaderByIndex
//
bool ELFFile::FindSectionHeaderByIndex(int nSectionIndex, ELF::ELF_Shdr* pSectionHeaderOut)
{
	const int nSectionCount = GetSectionCount(); // Will return zero if !mbInitialized

	if(nSectionIndex < nSectionCount)
	{
		const IO::off_type pos = (IO::off_type)(mELFOffset + mHeader.e_shoff + (mHeader.e_shentsize * nSectionIndex));

		if(mpStream->SetPosition(pos))
			return ReadSectionHeader(pSectionHeaderOut);
	}

	return false;
}


///////////////////////////////////////////////////////////////////////////////
// FindSectionHeaderByName
//
bool ELFFile::FindSectionHeaderByName(const char* pSectionName, ELF::ELF_Shdr* pSectionHeaderOut)
{
	using namespace ELF;

	if(mbInitialized)
	{
		int      nNameSectionIndex;
		ELF_Shdr nameSectionHeader;

		// Look up the index of the section containing all section names.
		if(mHeader.e_shstrndx < SHN_LORESERVE)
		{
			nNameSectionIndex = mHeader.e_shstrndx;
		}
		else if(mHeader.e_shstrndx == SHN_XINDEX)
		{
			if(!FindSectionHeaderByIndex(0, pSectionHeaderOut))
				return false;

			nNameSectionIndex = (int)(int32_t)pSectionHeaderOut->sh_link;
		}
		else
			return false;

		// Get the name section header.
		if(FindSectionHeaderByIndex(nNameSectionIndex, &nameSectionHeader))
		{
			if(nameSectionHeader.sh_type != SHT_NOBITS)
			{
				// Find the requested section.
				const int nSectionCount = GetSectionCount();

				for(int i = 0; i < nSectionCount; ++i)
				{
					if(FindSectionHeaderByIndex(i, pSectionHeaderOut))
					{
						const IO::off_type pos = (IO::off_type)(mELFOffset + nameSectionHeader.sh_offset + pSectionHeaderOut->sh_name);

						if(mpStream->SetPosition(pos))
						{
							if(SectionNameMatches(pSectionName))
								return true;
						}
					}
				}
			}
		}
	}

	return false;
}


///////////////////////////////////////////////////////////////////////////////
// ReadFileHeader
//
bool ELFFile::ReadFileHeader(ELF::ELF_Ehdr* pFileHeaderOut)
{
	bool bReturnValue = false;

	using namespace EA::IO;
	using namespace ELF;

	if(mpStream && mpStream->SetPosition(0))
	{
		if(mpStream->Read(pFileHeaderOut->e_ident, sizeof(pFileHeaderOut->e_ident)) == sizeof(pFileHeaderOut->e_ident))
		{
			// Sony .self files (starting with the 190 SDK?) begin with a section that begins with 'SCE\0'.
			// Note that a .self file is a "signed ELF" file; it's used for security/DRM on the Playstation 3.
			if((pFileHeaderOut->e_ident[EI_MAG0] == 'S') &&
			   (pFileHeaderOut->e_ident[EI_MAG1] == 'C') &&
			   (pFileHeaderOut->e_ident[EI_MAG2] == 'E') &&
			   (pFileHeaderOut->e_ident[EI_MAG3] ==  0))
			{
				// See the SCE_Hdr struct.
				uint64_t nSCEHeaderSize = 0;
				ReadUint64(mpStream, nSCEHeaderSize, mEndian);
				mELFOffset = (uint32_t)nSCEHeaderSize; // nSCEHeaderSize is a uint64_t but it will always be a small value.
				mpStream->SetPosition((IO::off_type)mELFOffset);
				mpStream->Read(pFileHeaderOut->e_ident, sizeof(pFileHeaderOut->e_ident));
			}

			// Some basic sanity checking of the file header.
			if((pFileHeaderOut->e_ident[EI_MAG0] == 0x7F) &&
			   (pFileHeaderOut->e_ident[EI_MAG1] ==  'E') &&
			   (pFileHeaderOut->e_ident[EI_MAG2] ==  'L') &&
			   (pFileHeaderOut->e_ident[EI_MAG3] ==  'F'))
			{
				// Set the ELF class, 32- or 64-bit.
				mnClass = pFileHeaderOut->e_ident[EI_CLASS];
				mEndian = (pFileHeaderOut->e_ident[EI_DATA] == ELFDATA2MSB) ? kEndianBig : kEndianLittle;

				// EA_ASSERT((mnClass == ELFCLASS32) || (mnClass == ELFCLASS64));
				if(mnClass == ELFCLASS32)
				{
					ELF32_Ehdr elf32Ehdr;
					const uint32_t identSize = sizeof(elf32Ehdr.e_ident);
					mpStream->Read(((char*)&elf32Ehdr) + identSize, sizeof(ELF32_Ehdr) - identSize);

					// Coerce the 32-bit section header into a 64-bit one.
					if (mEndian != IO::kEndianLocal)
					{
						pFileHeaderOut->e_type = EA::StdC::Swizzle(elf32Ehdr.e_type);
						pFileHeaderOut->e_machine = EA::StdC::Swizzle(elf32Ehdr.e_machine);
						pFileHeaderOut->e_version = EA::StdC::Swizzle(elf32Ehdr.e_version);
						pFileHeaderOut->e_entry = EA::StdC::Swizzle(elf32Ehdr.e_entry);
						pFileHeaderOut->e_phoff = EA::StdC::Swizzle(elf32Ehdr.e_phoff);
						pFileHeaderOut->e_shoff = EA::StdC::Swizzle(elf32Ehdr.e_shoff);
						pFileHeaderOut->e_flags = EA::StdC::Swizzle(elf32Ehdr.e_flags);
						pFileHeaderOut->e_ehsize = EA::StdC::Swizzle(elf32Ehdr.e_ehsize);
						pFileHeaderOut->e_phentsize = EA::StdC::Swizzle(elf32Ehdr.e_phentsize);
						pFileHeaderOut->e_phnum = EA::StdC::Swizzle(elf32Ehdr.e_phnum);
						pFileHeaderOut->e_shentsize = EA::StdC::Swizzle(elf32Ehdr.e_shentsize);
						pFileHeaderOut->e_shnum = EA::StdC::Swizzle(elf32Ehdr.e_shnum);
						pFileHeaderOut->e_shstrndx = EA::StdC::Swizzle(elf32Ehdr.e_shstrndx);
					}
					else
					{
						pFileHeaderOut->e_type = elf32Ehdr.e_type;
						pFileHeaderOut->e_machine = elf32Ehdr.e_machine;
						pFileHeaderOut->e_version = elf32Ehdr.e_version;
						pFileHeaderOut->e_entry = elf32Ehdr.e_entry;
						pFileHeaderOut->e_phoff = elf32Ehdr.e_phoff;
						pFileHeaderOut->e_shoff = elf32Ehdr.e_shoff;
						pFileHeaderOut->e_flags = elf32Ehdr.e_flags;
						pFileHeaderOut->e_ehsize = elf32Ehdr.e_ehsize;
						pFileHeaderOut->e_phentsize = elf32Ehdr.e_phentsize;
						pFileHeaderOut->e_phnum = elf32Ehdr.e_phnum;
						pFileHeaderOut->e_shentsize = elf32Ehdr.e_shentsize;
						pFileHeaderOut->e_shnum = elf32Ehdr.e_shnum;
						pFileHeaderOut->e_shstrndx = elf32Ehdr.e_shstrndx;
					}

					bReturnValue = (mpStream->GetState() == kStateSuccess);
				}
				else if(mnClass == ELFCLASS64)
				{
					const uint32_t identSize = sizeof(pFileHeaderOut->e_ident);
					mpStream->Read((char*)pFileHeaderOut + identSize, sizeof(ELF64_Ehdr) - identSize);

					if (mEndian != IO::kEndianLocal)
					{
						pFileHeaderOut->e_type = EA::StdC::Swizzle(pFileHeaderOut->e_type);
						pFileHeaderOut->e_machine = EA::StdC::Swizzle(pFileHeaderOut->e_machine);
						pFileHeaderOut->e_version = EA::StdC::Swizzle(pFileHeaderOut->e_version);
						pFileHeaderOut->e_entry = EA::StdC::Swizzle(pFileHeaderOut->e_entry);
						pFileHeaderOut->e_phoff = EA::StdC::Swizzle(pFileHeaderOut->e_phoff);
						pFileHeaderOut->e_shoff = EA::StdC::Swizzle(pFileHeaderOut->e_shoff);
						pFileHeaderOut->e_flags = EA::StdC::Swizzle(pFileHeaderOut->e_flags);
						pFileHeaderOut->e_ehsize = EA::StdC::Swizzle(pFileHeaderOut->e_ehsize);
						pFileHeaderOut->e_phentsize = EA::StdC::Swizzle(pFileHeaderOut->e_phentsize);
						pFileHeaderOut->e_phnum = EA::StdC::Swizzle(pFileHeaderOut->e_phnum);
						pFileHeaderOut->e_shentsize = EA::StdC::Swizzle(pFileHeaderOut->e_shentsize);
						pFileHeaderOut->e_shnum = EA::StdC::Swizzle(pFileHeaderOut->e_shnum);
						pFileHeaderOut->e_shstrndx = EA::StdC::Swizzle(pFileHeaderOut->e_shstrndx);
					}

					bReturnValue = (mpStream->GetState() == kStateSuccess);
				}
			}
		}
	}

	return bReturnValue;
}


///////////////////////////////////////////////////////////////////////////////
// ReadSectionHeader
//
bool ELFFile::ReadSectionHeader(ELF::ELF_Shdr* pSectionHeaderOut)
{
	using namespace ELF;

	bool bReturnValue = false;

	if(mbInitialized)
	{
		if (mnClass == ELFCLASS32)
		{
			ELF32_Shdr elf32Shdr;
			mpStream->Read(&elf32Shdr, sizeof(ELF32_Shdr));

			// Coerce the 32-bit section header into a 64-bit one.
			if (mEndian != IO::kEndianLocal)
			{
				pSectionHeaderOut->sh_name = EA::StdC::Swizzle(elf32Shdr.sh_name);
				pSectionHeaderOut->sh_type = EA::StdC::Swizzle(elf32Shdr.sh_type);
				pSectionHeaderOut->sh_flags = EA::StdC::Swizzle(elf32Shdr.sh_flags);
				pSectionHeaderOut->sh_addr = EA::StdC::Swizzle(elf32Shdr.sh_addr);
				pSectionHeaderOut->sh_offset = EA::StdC::Swizzle(elf32Shdr.sh_offset);
				pSectionHeaderOut->sh_size = EA::StdC::Swizzle(elf32Shdr.sh_size);
				pSectionHeaderOut->sh_link = EA::StdC::Swizzle(elf32Shdr.sh_link);
				pSectionHeaderOut->sh_info = EA::StdC::Swizzle(elf32Shdr.sh_info);
				pSectionHeaderOut->sh_addralign = EA::StdC::Swizzle(elf32Shdr.sh_addralign);
				pSectionHeaderOut->sh_entsize = EA::StdC::Swizzle(elf32Shdr.sh_entsize);
			}
			else
			{
				pSectionHeaderOut->sh_name = elf32Shdr.sh_name;
				pSectionHeaderOut->sh_type = elf32Shdr.sh_type;
				pSectionHeaderOut->sh_flags = elf32Shdr.sh_flags;
				pSectionHeaderOut->sh_addr = elf32Shdr.sh_addr;
				pSectionHeaderOut->sh_offset = elf32Shdr.sh_offset;
				pSectionHeaderOut->sh_size = elf32Shdr.sh_size;
				pSectionHeaderOut->sh_link = elf32Shdr.sh_link;
				pSectionHeaderOut->sh_info = elf32Shdr.sh_info;
				pSectionHeaderOut->sh_addralign = elf32Shdr.sh_addralign;
				pSectionHeaderOut->sh_entsize = elf32Shdr.sh_entsize;
			}

			bReturnValue = (mpStream->GetState() == IO::kStateSuccess);
		}
		else if (mnClass == ELFCLASS64)
		{
			mpStream->Read(pSectionHeaderOut, sizeof(ELF64_Phdr));

			if (mEndian != IO::kEndianLocal)
			{
				pSectionHeaderOut->sh_name = EA::StdC::Swizzle(pSectionHeaderOut->sh_name);
				pSectionHeaderOut->sh_type = EA::StdC::Swizzle(pSectionHeaderOut->sh_type);
				pSectionHeaderOut->sh_flags = EA::StdC::Swizzle(pSectionHeaderOut->sh_flags);
				pSectionHeaderOut->sh_addr = EA::StdC::Swizzle(pSectionHeaderOut->sh_addr);
				pSectionHeaderOut->sh_offset = EA::StdC::Swizzle(pSectionHeaderOut->sh_offset);
				pSectionHeaderOut->sh_size = EA::StdC::Swizzle(pSectionHeaderOut->sh_size);
				pSectionHeaderOut->sh_link = EA::StdC::Swizzle(pSectionHeaderOut->sh_link);
				pSectionHeaderOut->sh_info = EA::StdC::Swizzle(pSectionHeaderOut->sh_info);
				pSectionHeaderOut->sh_addralign = EA::StdC::Swizzle(pSectionHeaderOut->sh_addralign);
				pSectionHeaderOut->sh_entsize = EA::StdC::Swizzle(pSectionHeaderOut->sh_entsize);
			}

			bReturnValue = (mpStream->GetState() == IO::kStateSuccess);
		}
	}

	return bReturnValue;
}


///////////////////////////////////////////////////////////////////////////////
// SectionNameMatches
//
bool ELFFile::SectionNameMatches(const char* pSectionName)
{
	#ifdef EA_DEBUG
		char  sectionName[128]; sectionName[0] = 0;
		char* p    = sectionName;
		char* pEnd = p + 127;

		while((p < pEnd) && mpStream->Read(p, 1) && *p)
			++p;
		*p = 0;

		return (EA::StdC::Strcmp(sectionName, pSectionName) == 0);
	#else
	  char chIn;

	  for(mpStream->Read(&chIn, 1); 
			chIn && *pSectionName; 
			mpStream->Read(&chIn, 1), pSectionName++)
		{
			if(chIn != *pSectionName) 
				return false;
		}

		return (chIn == *pSectionName);
	#endif
}



} // namespace Callstack
} // namespace EA