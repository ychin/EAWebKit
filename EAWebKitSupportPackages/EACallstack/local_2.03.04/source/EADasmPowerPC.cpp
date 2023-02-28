///////////////////////////////////////////////////////////////////////////////
// EADasmPowerPC.cpp
//
// Copyright (c) 2005, Electronic Arts. All rights reserved.
// Maintained by Paul Pedriana.
///////////////////////////////////////////////////////////////////////////////



#include <EACallstack/EADasm.h>

#if EACALLSTACK_DASM_POWERPC_ENABLED

#include <EAStdC/EASprintf.h>
#include <stdlib.h>
#include <string.h>
#include <stdio.h>
#include <limits.h>
#include <EAAssert/eaassert.h>

namespace EA
{

namespace Dasm
{

namespace PowerPC
{


///////////////////////////////////////////////////////////////////////////////
// ppc_word
//
// This is a typedef for a PowerPC instruction word. 
// It allows us to easily tell that a given variable refers to a machine instruction.
//
typedef uint32_t ppc_word;



///////////////////////////////////////////////////////////////////////////////
// Masks
//
// These are various useful masks for use in disassembly
// 
const uint32_t kMaskOpcode          = 0xfc000000;  // 11111100000000000000000000000000 // This field defines the primary mOperation.
const uint32_t kMaskExtendedOpcode  = 0x000007fe;  // 00000000000000000000011111111110 // This field defines the extended mOperation. What bits are used depend on the primary mOperation.
const uint32_t kMaskD               = 0x03e00000;  // 00000011111000000000000000000000 // This field is used to specify a bit in the CR, or in the FPSCR, as the destination of the result of an instruction.
const uint32_t kMaskA               = 0x001f0000;  // 00000000000111110000000000000000 // This field is used to specify a bit in the CR to be used as a source.
const uint32_t kMaskB               = 0x0000f800;  // 00000000000000001111100000000000 // This field is used to specify a bit in the CR to be used as a source.
const uint32_t kMaskRotationB       = 0x000007c0;  // 00000000000000000000011111000000 // These fields are used in rotate instructions to specify a 64-bit mask (32 bits in 32-bit implementations) consisting of 1 bits from bit MB + 32 through bit ME + 32 inclusive, and 0 bits elsewhere, as described in Section 4.2.1.4 Integer Rotate and Shift Instructions.
const uint32_t kMaskRotationE       = 0x0000003e;  // 00000000000000000000000000111110 // " " " 
//const uint32_t kMaskUIMM          = 0x0000ffff;  // 00000000000000001111111111111111 // The field used to specify UIMM or SIMM values (i.e. 16 bit integer constants).
const uint32_t kMaskCRD             = 0x03800000;  // 00000011100000000000000000000000
const uint32_t kMaskCRA             = 0x001c0000;  // 00000000000111000000000000000000
const uint32_t kMaskL               = 0x00600000;  // 00000000011000000000000000000000 // Field used by the synchronize instruction. This field is defined in 64-bit implementations only.
const uint32_t kMaskOE              = 0x00000400;  // 00000000000000000000010000000000 // This field is used for extended arithmetic to enable setting OV and SO (overflow) in the XER.
const uint32_t kMaskDestination     = 0x02000000;  // 00000010000000000000000000000000
const uint32_t kMaskAltivec         = 0x00000400;  // 00000000000000000000010000000000
const uint32_t kMaskAltivecStream   = 0x00600000;  // 00000000011000000000000000000000


inline uint32_t GetOpcode(uint32_t instruction)
	{ return ((instruction & kMaskOpcode) >> 26); }

inline uint32_t GetExtendedOpcode(uint32_t instruction)
	{ return ((instruction & kMaskExtendedOpcode) >> 1); }

inline uint32_t GetD(uint32_t instruction)
	{ return ((instruction & kMaskD) >> 21); }

inline uint32_t GetA(uint32_t instruction)
	{ return ((instruction & kMaskA) >> 16); }

inline uint32_t GetB(uint32_t instruction)
	{ return ((instruction & kMaskB) >> 11); }

inline uint32_t GetRotationB(uint32_t instruction)
	{ return ((instruction & kMaskRotationB) >> 6); }

inline uint32_t GetRotationE(uint32_t instruction)
	{ return ((instruction & kMaskRotationE) >> 1); }

inline uint32_t GetUIMM(uint32_t instruction)
	{ return (uint16_t)instruction; }

inline int32_t GetSIMM(uint32_t instruction)
	{ return (int16_t)(uint16_t)(instruction & 0xffff); }

inline uint32_t GetCRD(uint32_t instruction)
	{ return ((instruction & kMaskCRD) >> 23); }

inline uint32_t GetCRA(uint32_t instruction)
	{ return ((instruction & kMaskCRA) >> 18); }

inline uint32_t GetL(uint32_t instruction)
	{ return ((instruction & kMaskL) >> 21); }

inline uint32_t GetOE(uint32_t instruction)
	{ return ((instruction & kMaskOE) >> 21); }

inline uint32_t GetDestination(uint32_t instruction)
	{ return ((instruction & kMaskDestination) >> 21); }

inline uint32_t GetAltivec(uint32_t instruction)
	{ return ((instruction & kMaskAltivec) >> 21); }

inline uint32_t GetAltivecStream(uint32_t instruction)
	{ return ((instruction & kMaskAltivecStream)  >> 21); }



///////////////////////////////////////////////////////////////////////////////
// Strings
///////////////////////////////////////////////////////////////////////////////


// The full set of trap conditions are (with mnemonically equivalent values
// in parentheses):
//    lt, le (ng), eq, ge (nl), gt, ne, ng, llt, lle (lng), lge (lnl), lgt
const char* kStrTrapCondition[32] = 
{
	NULL, "lgt", "llt",  NULL, "eq", "lge", "lle", NULL,
	"gt",  NULL,  NULL,  NULL, "ge", NULL,  NULL,  NULL,
	"lt",  NULL,  NULL,  NULL, "le", NULL,  NULL,  NULL,
	"ne",  NULL,  NULL,  NULL, NULL, NULL,  NULL,  NULL
};

const char* kStrCondition[8] = 
{
	"ge", "le", "ne", "ns", "lt", "gt", "eq", "so"
};

const char* kStrCompare[4] = 
{
	"cmpw", "cmpd", "cmplw", "cmpld"
};

const char* kStrBranchDetail[4] = 
{
	"", "l", "a", "la"
};

const char* kStrDecrementType[16] = 
{
	"nzf", "zf", NULL, NULL, "nzt", "zt", NULL, NULL,
	"nz",  "z",  NULL, NULL, "nz",  "z",  NULL, NULL
};

const char* kStrImmediateOrRegister[2] = 
{
	"", "r"
};

const char* kStrOverflow[2] = 
{
	"", "o"
};

const char* kStrConditionRegisterModified[2] =
{
	"", "."
};

const char* kStrLoadStore[32] = 
{
	"lwz",  "lwzu", "lbz",  "lbzu", "stw",  "stwu", "stb",  "stbu", 
	"lhz",  "lhzu", "lha",  "lhau", "sth",  "sthu", "lmw",  "stmw",
	"lfs",  "lfsu", "lfd",  "lfdu", "stfs", "stfsu","stfd", "stfdu",
	NULL,   NULL,   NULL,   NULL,   NULL,   NULL,   NULL,   NULL
};

const char* kStrAltivec[32] =
{
	"vmhaddshs", "vmhraddshs", "vmladduhm", NULL,   "vmsumubm",  "vmsummbm", "vmsumuhm",  "vmsumuhs",
	"vmsumshm",  "vmsumshs",   "vsel",      "vperm", "vsldoi",    NULL,      "vmaddfp",   "vnmsubfp", 
	NULL,        NULL,         NULL,        NULL,    NULL,        NULL,      NULL,         NULL, 
	NULL,        NULL,         NULL,        NULL,    NULL,        NULL,      NULL,         NULL
};





///////////////////////////////////////////////////////////////////////////////
// SwapDA
//
// For a given ppc_word instruction, this function swaps the fields in the 
// A and D bitfields. This is simply of utility to us in places.
//
ppc_word SwapDA(ppc_word w)
{
	return ((w & 0xfc00ffff) | ((w & kMaskA) << 5) | ((w & kMaskD) >> 5));
}


///////////////////////////////////////////////////////////////////////////////
// SwapAB
//
// For a given ppc_word instruction, this function swaps the fields in the 
// A and B bitfields. This is simply of utility to us in places.
//
ppc_word SwapAB(ppc_word w)
{
	return ((w & 0xffe007ff) | ((w & kMaskB) << 5) | ((w & kMaskA) >> 5));
}


///////////////////////////////////////////////////////////////////////////////
// WriteInvalidInstruction
//
void WriteInvalidInstruction(DasmData& dd, ppc_word in)
{
	EA::StdC::Snprintf(dd.mOperation, sizeof(dd.mOperation), "(invalid) 0x%08x", (unsigned)in); // e.g. "invalid 0x12345678"
	dd.mOperands[0] = 0;
	dd.mInstructionFlags |= kIFInvalid;
}


///////////////////////////////////////////////////////////////////////////////
// WriteImmediateOperands
//
// For instructions that have immediate operands, we write them here.
//
//   instructionType 0: D-mode, D,A,imm
//   instructionType 1: S-mode, A,S,imm
//   instructionType 2: S/D register is ignored (trap,cmpi)
//   instructionType 3: A register is ignored (li)
//
void WriteImmediateOperands(DasmData& dd, ppc_word in, int uimm, int instructionType)
{
	char*   s = dd.mOperands + strlen(dd.mOperands);
	int32_t i = (int32_t)(in & 0x0000ffff);

	dd.mInstructionType = kITImmediate;

	if(uimm)
		dd.mInstructionFlags |= kIFUnsigned;
	else
	{
		if(i > 0x00007fff)
			i -= 0x00010000;
	}

	dd.mDisplacement = i;

	switch (instructionType)
	{
		case 0: // kOFHex
			EA::StdC::Sprintf(s, "r%u,r%u,%d", (unsigned)GetD(in), (unsigned)GetA(in), (int)i); // e.g. "r4,r6,3256"
			break;

		case 1: // kOFHex
			EA::StdC::Sprintf(s, "r%u,r%u,%d", (unsigned)GetA(in), (unsigned)GetD(in), (int)i); // e.g. "r6,r4,3256"
			break;

		case 2: // kOFHex
			EA::StdC::Sprintf(s, "r%u,%d", (unsigned)GetA(in), (int)i); // e.g. "r4,3256"
			break;

		case 3: // kOFHex
			EA::StdC::Sprintf(s, "r%u,%d", (unsigned)GetD(in), (int)i); // e.g. "r4,3256"
			break;

		default:
			EA::StdC::Sprintf(s, "invalid");
			break;
	}
}


///////////////////////////////////////////////////////////////////////////////
// Write_rArB
//
void Write_rArB(char* s, ppc_word in)
{
	s += strlen(s);
	EA::StdC::Sprintf(s, "r%u,r%u", (unsigned)GetA(in), (unsigned)GetB(in)); // e.g. "r2,r9"
}


///////////////////////////////////////////////////////////////////////////////
// Write_rDrArB
//
char* Write_rDrArB(char* s, ppc_word in, int mask)
{
	s += strlen(s);

	// e.g. "r4", "r4,r5", "r4,r5,r6"
	if(mask)
	{
		if(mask & 4)
			s += EA::StdC::Sprintf(s, "r%u,", (unsigned)GetD(in));
		if(mask & 2)
			s += EA::StdC::Sprintf(s, "r%u,", (unsigned)GetA(in));
		if(mask & 1)
			s += EA::StdC::Sprintf(s, "r%u,", (unsigned)GetB(in));
		--s;
	}

	*s = '\0';

	return s;
}


///////////////////////////////////////////////////////////////////////////////
// Write_fDrArB
//
char* Write_fDrArB(char* s, ppc_word in, int mask)
{
	s += strlen(s);

	// e.g. "f4", "f4,r5", "f4,r5,r6"
	if(mask)
	{
		if(mask & 4)
			s += EA::StdC::Sprintf(s, "f%u,", (unsigned)GetD(in));
		if(mask & 2)
			s += EA::StdC::Sprintf(s, "r%u,", (unsigned)GetA(in));
		if(mask & 1)
			s += EA::StdC::Sprintf(s, "r%u,", (unsigned)GetB(in));
		--s;
	}

	*s = '\0';

	return s;
}


///////////////////////////////////////////////////////////////////////////////
// Write_vDvAvB
//
char* Write_vDvAvB(char* s, ppc_word in, int mask)
{
	if(mask)
	{
		if(mask & 4)
			s += EA::StdC::Sprintf(s, "v%u,", (unsigned)GetD(in));

		if(mask & 2)
		{
			if(mask & 16)  // A = SIMM (signed immediate value)
			{
				const int32_t a = (int32_t)GetA(in);
				s += EA::StdC::Sprintf(s, "%d,", (int)((a > 15) ? (a - 32) : a)); // kOFHex
			}
			else if(mask & 8)  // A = UIMM (unsigned immediate value)
				s += EA::StdC::Sprintf(s, "%u,", (unsigned)GetA(in)); // kOFHex
			else
				s += EA::StdC::Sprintf(s, "v%u,", (unsigned)GetA(in));
		}

		if(mask & 1)
			s += EA::StdC::Sprintf(s, "v%u,", (unsigned)GetB(in));

		--s;
	}

	*s = '\0';

	return s;
}


///////////////////////////////////////////////////////////////////////////////
// WriteTrapi
//
// We write the twi and twd instructions.
// 
void WriteTrapi(DasmData& dd, ppc_word in, InstructionFlags flag)
{
	const uint32_t index = GetD(in);

	if(kStrTrapCondition[index])
	{
		// kOFMnemonics
		dd.mInstructionFlags |= flag;
		EA::StdC::Snprintf(dd.mOperation, sizeof(dd.mOperation), "t%c%s", flag & kIF64Bit ? 'd' : 'w', kStrTrapCondition[index]); // e.g. "twgti"
		WriteImmediateOperands(dd, in, 0, 2);
	}
	else
		WriteInvalidInstruction(dd, in);
}


///////////////////////////////////////////////////////////////////////////////
// WriteTrap
//
void WriteTrap(DasmData& dd, ppc_word in, unsigned char dmode)
{
	const uint32_t to = GetD(in);

	if(kStrTrapCondition[to])
	{
		dd.mInstructionFlags |= dmode;
		EA::StdC::Snprintf(dd.mOperation, sizeof(dd.mOperation), "t%c%s", dmode ? 'd' : 'w', kStrTrapCondition[to]); // e.g. "tdeq", "tdlt"
		Write_rArB(dd.mOperands, in);
	}
	else
	{
		if(to == 31)
		{
			if(dmode)
			{
				dd.mInstructionFlags |= dmode;
				strcpy(dd.mOperation, "td");
				strcpy(dd.mOperands, "31, 0, 0");
			}
			else
				strcpy(dd.mOperation, "trap");
		}
		else
			WriteInvalidInstruction(dd, in);
	}
}


///////////////////////////////////////////////////////////////////////////////
// WriteCmpi
//
void WriteCmpi(DasmData& dd, ppc_word in, int uimm)
{
	uint32_t i = GetL(in); // i will be 0, 1, 2, or 3, but only 0 and 1 are valid.

	if(i < 2)
	{
		if(i == 1)
			dd.mInstructionFlags |= kIF64Bit;

		EA::StdC::Snprintf(dd.mOperation, sizeof(dd.mOperation), "%si", kStrCompare[(uimm * 2) + i]); // e.g. "cmpdi", "cmpwi"

		i = GetCRD(in);

		if(i)
			EA::StdC::Snprintf(dd.mOperands, sizeof(dd.mOperands), "cr%c,", (char)('0' + i)); // e.g.  "cr3,"

		WriteImmediateOperands(dd, in, uimm, 2);
	}
	else
		WriteInvalidInstruction(dd, in);
}


///////////////////////////////////////////////////////////////////////////////
// WriteCmp
//
void WriteCmp(DasmData& dd, ppc_word in)
{
	uint32_t i = GetL(in); // i will be 0, 1, 2, or 3, but only 0 and 1 are valid.

	if(i < 2)
	{
		if(i == 1)
			dd.mInstructionFlags |= kIF64Bit;

		strcpy(dd.mOperation, kStrCompare[((in & kMaskExtendedOpcode) ? 2 : 0) + i]);

		i = GetCRD(in);

		if(i)
			EA::StdC::Snprintf(dd.mOperands, sizeof(dd.mOperands), "cr%c,", (char)('0' + i));

		Write_rArB(dd.mOperands, in);
	}
	else
		WriteInvalidInstruction(dd, in);
}


///////////////////////////////////////////////////////////////////////////////
// WriteAddi
//
void WriteAddi(DasmData& dd, ppc_word in, const char* pSuffix)
{
	if((in & 0x08000000) && (GetA(in) == 0))
	{
		EA::StdC::Snprintf(dd.mOperation, sizeof(dd.mOperation), "l%s", pSuffix);  // e.g. "li", "lis"
		WriteImmediateOperands(dd, in, 0, 3);
	}
	else
	{
		EA::StdC::Snprintf(dd.mOperation, sizeof(dd.mOperation), "%s%s", (in & 0x00008000) ? "sub" : "add", pSuffix); // e.g. "subic", "addis"

		if(in & 0x00008000)
			in = (in ^ 0x0000ffff) + 1;

		WriteImmediateOperands(dd, in, 1, 0);
	}
}


///////////////////////////////////////////////////////////////////////////////
// WriteBranch
//
void WriteBranch(DasmData& dd, ppc_word in, const char* bname, int aform, int displacement)
{
	const uint32_t bo = GetD(in);
	const uint32_t bi = GetA(in);
	char           y  = (char)(bo & 1);
	const char* const pSuffix = kStrBranchDetail[(aform * 2) + (in & 1)];

	if(displacement < 0)
		y ^= 1;

	y = y ? '+' : '-'; // Branch hint.

	if(bo & 4)
	{
		// standard case - no decrement 
		if(bo & 16)
		{
			// branch always 
			if(GetOpcode(in) != 16)
				EA::StdC::Snprintf(dd.mOperation, sizeof(dd.mOperation), "b%s%s", bname, pSuffix);
			else
			{
				EA::StdC::Snprintf(dd.mOperation, sizeof(dd.mOperation), "bc%s", pSuffix); // e.g. "bcla", "bcl"
				EA::StdC::Snprintf(dd.mOperands, sizeof(dd.mOperands), "%u,%u", (unsigned)bo, (unsigned)bi);
			}
		}
		else 
		{
			// branch conditional 
			EA::StdC::Snprintf(dd.mOperation, sizeof(dd.mOperation),"b%s%s%s%c", kStrCondition[((bo & 8) >> 1) + (bi & 3)], bname, pSuffix, y);

			if(bi >= 4)
				EA::StdC::Snprintf(dd.mOperands, sizeof(dd.mOperands), "cr%u", (unsigned)bi >> 2);
		}
	}
	else
	{
		// CTR is decremented and checked 
		EA::StdC::Snprintf(dd.mOperation, sizeof(dd.mOperation), "bd%s%s%s%c", kStrDecrementType[bo >> 1], bname, pSuffix, y);

		if((bo & 16) == 0)
			EA::StdC::Snprintf(dd.mOperands, sizeof(dd.mOperands), "%u", (unsigned)bi);
	}
}


///////////////////////////////////////////////////////////////////////////////
// WriteBranchConditional
//
void WriteBranchConditional(DasmData& dd, ppc_word in)
{
	int32_t d = (int32_t)(in & 0x0000fffc);

	if(d >= 0x00008000)
	   d -= 0x00010000;

	WriteBranch(dd, in, "", (in & 2) ? 1 : 0, d);

	char* s = dd.mOperands + strlen(dd.mOperands);

	if(s > dd.mOperands)
		*s++ = ',';

	if(in & 2)  // If branching to an absolute address...
		EA::StdC::Sprintf(s, "0x%08x", (unsigned)d);
	else
	{
		if(dd.mInstructionAddress > (uint64_t)UINT32_MAX)
			EA::StdC::Sprintf(s, "0x%016" PRIx64, (uint64_t)(dd.mInstructionAddress + d));
		else
			EA::StdC::Sprintf(s, "0x%08x", (unsigned)(dd.mInstructionAddress + d));
	}

	dd.mInstructionType = kITBranch;
	dd.mDisplacement    = d; // Note that if we had an absolute address, this isn't a displacement but an absolute address.
}


///////////////////////////////////////////////////////////////////////////////
// WriteBli
//
void WriteBli(DasmData& dd, ppc_word in)
{
	int d = (int)(in & 0x3fffffc);

	if(d >= 0x02000000)
	   d -= 0x04000000;

	EA::StdC::Snprintf(dd.mOperation, sizeof(dd.mOperation), "b%s", kStrBranchDetail[in & 3]);  // e.g. "bl", "ba", "bla"

	if(in & 2) // If branching to an absolute address...
		EA::StdC::Snprintf(dd.mOperands, sizeof(dd.mOperands), "0x%08x", (unsigned)d);
	else
	{
		if(dd.mInstructionAddress > (uint64_t)UINT32_MAX)
			EA::StdC::Snprintf(dd.mOperands, sizeof(dd.mOperands), "0x%016" PRIx64, (uint64_t)(dd.mInstructionAddress + d));
		else
			EA::StdC::Snprintf(dd.mOperands, sizeof(dd.mOperands), "0x%08x", (unsigned)(dd.mInstructionAddress + d));
	}

	dd.mInstructionType = kITBranch;
	dd.mDisplacement    = d; // Note that if we had an absolute address, this isn't a displacement but an absolute address.
}


///////////////////////////////////////////////////////////////////////////////
// WriteMcrf
//
void WriteMcrf(DasmData& dd, ppc_word in, char c)
{
	if((in & 0x0063f801) == 0)
	{
		EA::StdC::Snprintf(dd.mOperation, sizeof(dd.mOperation), "mcrf%c", c); // e.g. "mcrf", "mcrfs"
		EA::StdC::Snprintf(dd.mOperands, sizeof(dd.mOperands), "cr%u,cr%u", (unsigned)GetCRD(in), (unsigned)GetCRA(in));
	}
	else
		WriteInvalidInstruction(dd, in);
}


///////////////////////////////////////////////////////////////////////////////
// WriteCROperation
//
void WriteCROperation(DasmData& dd, ppc_word in, const char* n1, const char* n2)
{
	const uint32_t crd = GetD(in);
	const uint32_t cra = GetA(in);
	const uint32_t crb = GetB(in);

	if((in & 1) == 0)
	{
		EA::StdC::Snprintf(dd.mOperation, sizeof(dd.mOperation), "cr%s", ((cra == crb) && n2) ? n2 : n1); // e.g. "crnand", "crnor", "crnot"

		if((cra == crb) && n2) // kOFMnemonics
			EA::StdC::Snprintf(dd.mOperands, sizeof(dd.mOperands), "%u,%u", (unsigned)crd, (unsigned)cra); // e.g. "crnand cr2,cr3"
		else
			EA::StdC::Snprintf(dd.mOperands, sizeof(dd.mOperands), "%u,%u,%u", (unsigned)crd, (unsigned)cra, (unsigned)crb); // e.g. "crnand cr2,cr3,cr4"
	}
	else
		WriteInvalidInstruction(dd, in);
}


///////////////////////////////////////////////////////////////////////////////
// WriteNoOperandInstruction
//
// Writes an instruction that has no operands.
//
void WriteNoOperandInstruction(DasmData& dd, ppc_word in, const char* name, unsigned char dmode)
{
	if(in & (kMaskD | kMaskA | kMaskB | 1))
		WriteInvalidInstruction(dd, in);
	else
	{
		dd.mInstructionFlags |= dmode;
		strcpy(dd.mOperation, name);
	}
}


///////////////////////////////////////////////////////////////////////////////
// WriteRlw
//
void WriteRlw(DasmData& dd, ppc_word in, const char* name, int bImmediateOrRegister)
{
	const uint32_t s   = GetD(in);
	const uint32_t a   = GetA(in);
	const uint32_t bsh = GetB(in);
	const uint32_t mb  = GetRotationB(in);
	const uint32_t me  = GetRotationE(in);

	EA::StdC::Snprintf(dd.mOperation, sizeof(dd.mOperation), "rlw%s%c", name, (in & 1) ? '.' : '\0'); // e.g. "rlwimi", "rlwinm."
	EA::StdC::Snprintf(dd.mOperands, sizeof(dd.mOperands), "r%u,r%u,%s%u,%u,%u", (unsigned)a, (unsigned)s, kStrImmediateOrRegister[bImmediateOrRegister & 1], (unsigned)bsh, (unsigned)mb, (unsigned)me);
}


///////////////////////////////////////////////////////////////////////////////
// WriteOri
//
void WriteOri(DasmData& dd, ppc_word in, const char* name)
{
	strcpy(dd.mOperation, name);
	WriteImmediateOperands(dd, in, 1, 1);
}


///////////////////////////////////////////////////////////////////////////////
// WriteRld
//
void WriteRld(DasmData& dd, ppc_word in, const char* name, int bImmediateOrRegister)
{
	uint32_t s   = GetD(in);
	uint32_t a   = GetA(in);
	uint32_t bsh = bImmediateOrRegister ? GetB(in) : (((in & 2) << 4) + GetB(in));
	uint32_t m   = (in & 0x7e0) >> 5;

	dd.mInstructionFlags |= kIF64Bit;
	EA::StdC::Snprintf(dd.mOperation, sizeof(dd.mOperation), "rld%s%c", name, (in & 1) ? '.' : '\0'); // e.g. "rldimi", "rldinm."
	EA::StdC::Snprintf(dd.mOperands, sizeof(dd.mOperands), "r%u,r%u,%s%u,%u", (unsigned)a, (unsigned)s, kStrImmediateOrRegister[bImmediateOrRegister & 1], (unsigned)bsh, (unsigned)m);
}


///////////////////////////////////////////////////////////////////////////////
// WriteThreeRegisterOperation
//
// We write an instruction of the form: <operation> rD,rA,rB
//
void WriteThreeRegisterOperation(DasmData& dd, ppc_word in, const char* name, int mask, int smode, int chkoe, int32_t chkrc, unsigned char dmode)
{
	if((chkrc >= 0) && ((in & 1) != (uint32_t)chkrc))
		WriteInvalidInstruction(dd, in);
	else
	{
		dd.mInstructionFlags |= dmode;

		if(smode)
			in = SwapDA(in);  // rA,rS,rB

		EA::StdC::Snprintf(dd.mOperation, sizeof(dd.mOperation), "%s%s%s", name, kStrOverflow[chkoe && (in & kMaskOE)], kStrConditionRegisterModified[(chkrc < 0) && (in & 1)]);
		Write_rDrArB(dd.mOperands, in, mask);
	}
}


///////////////////////////////////////////////////////////////////////////////
// WriteTwoRegisterOperation
//
// We write an instruction of the form: <operation> rD,rA,NB
//
void WriteTwoRegisterOperation(DasmData& dd, ppc_word in, const char* name, int smode, int chkoe, int32_t chkrc, unsigned char dmode)
{
	if(chkrc >= 0 && (in & 1) != (uint32_t)chkrc)
		WriteInvalidInstruction(dd, in);
	else
	{
		dd.mInstructionFlags |= dmode;

		if(smode)
			in = SwapDA(in);  // rA,rS,NB

		EA::StdC::Snprintf(dd.mOperation, sizeof(dd.mOperation), "%s%s%s", name, kStrOverflow[chkoe && (in & kMaskOE)], kStrConditionRegisterModified[(chkrc < 0) && (in & 1)]);

		char* const s = Write_rDrArB(dd.mOperands, in, 6);
		EA::StdC::Sprintf(s, ",%u", (unsigned)GetB(in));
	}
}


///////////////////////////////////////////////////////////////////////////////
// WriteMtcr
//
void WriteMtcr(DasmData& dd, ppc_word in)
{
	uint32_t s    = GetD(in);
	uint32_t crm  = (in & 0x000ff000) >> 12;
	char*    oper = dd.mOperands;

	if(in & 0x00100801)
		WriteInvalidInstruction(dd, in);
	else
	{
		EA::StdC::Snprintf(dd.mOperation, sizeof(dd.mOperation), "mtcr%c", (crm == 0xff) ? '\0' : 'f'); // e.g. "mtcr", "mtcrf"

		if(crm != 0xff) // kOFMnemonics
			oper += EA::StdC::Sprintf(oper, "0x%02x,", (unsigned)crm); // Might be useful to spell out all the fields. kOFHelp

		EA::StdC::Sprintf(oper, "r%u", (unsigned)s);
	}
}


///////////////////////////////////////////////////////////////////////////////
// WriteMsr
//
void WriteMsr(DasmData& dd, ppc_word in,int smode)
{
	const uint32_t s  = GetD(in);
	const uint32_t sr = (in & 0x000f0000) >> 16;

	if(in & 0x0010f801)
		WriteInvalidInstruction(dd, in);
	else
	{
		dd.mInstructionFlags |= kIFSupervisor;
		EA::StdC::Snprintf(dd.mOperation, sizeof(dd.mOperation), "m%csr", smode ? 't' : 'f'); 

		if(smode)
			EA::StdC::Snprintf(dd.mOperands, sizeof(dd.mOperands), "%u,r%u", (unsigned)sr, (unsigned)s); // Might be useful to spell out all the fields. kOFHelp
		else
			EA::StdC::Snprintf(dd.mOperands, sizeof(dd.mOperands), "r%u,%u", (unsigned)s, (unsigned)sr);
	}
}


///////////////////////////////////////////////////////////////////////////////
// WriteMspr
//
void WriteMspr(DasmData& dd, ppc_word in, int smode)
{
	uint32_t d   = GetD(in);
	uint32_t spr = ((GetB(in) << 5) + GetA(in));
	uint32_t fmt = 0;
	const char* x;

	if(in & 1)
		WriteInvalidInstruction(dd, in);
	else
	{
		if((spr != 1) && (spr != 8) && (spr != 9))
			dd.mInstructionFlags |= kIFSupervisor;

		switch (spr)
		{
			case 1:
				x = "xer";
				break;

			case 8:
				x = "lr";
				break;

			case 9:
				x = "ctr";
				break;

			case 18:
				x = "dsisr";
				break;

			case 19:
				x = "dar";
				break;

			case 22:
				x = "dec";
				break;

			case 25:
				x = "sdr1";
				break;

			case 26:
				x = "srr0";
				break;

			case 27:
				x = "srr1";
				break;

			case 272:
			case 273:
			case 274:
			case 275:
				x    = "sprg";
				spr -= 272;
				fmt  = 1;
				break;

			case 280:
				x = "asr";
				break;

			case 282:
				x = "ear";
				break;

			case 284:
				x = "tbl";
				break;

			case 285:
				x = "tbu";
				break;

			case 528:
			case 530:
			case 532:
			case 534:
				x   = "ibatu";
				spr = (spr - 528) >> 1;
				fmt = 1;
				break;

			case 529:
			case 531:
			case 533:
			case 535:
				x = "ibatl";
				spr = (spr-529)>>1;
				fmt = 1;
				break;

			case 536:
			case 538:
			case 540:
			case 542:
				x = "dbatu";
				spr = (spr - 536) >> 1;
				fmt = 1;
				break;

			case 537:
			case 539:
			case 541:
			case 543:
				x   = "dbatl";
				spr = (spr - 537) >> 1;
				fmt = 1;
				break;

			case 1013:
				x = "dabr";
				break;

			default:
				x   = "spr";
				fmt = 1;
				break;
		}

		EA::StdC::Snprintf(dd.mOperation, sizeof(dd.mOperation), "m%c%s", smode ? 't': 'f', x);

		if(fmt) 
		{
			if(smode)
				EA::StdC::Snprintf(dd.mOperands, sizeof(dd.mOperands),"%u,r%u", (unsigned)spr, (unsigned)d);
			else
				EA::StdC::Snprintf(dd.mOperands, sizeof(dd.mOperands),"r%u,%u", (unsigned)d, (unsigned)spr);
		}
		else
			EA::StdC::Snprintf(dd.mOperands, sizeof(dd.mOperands),"r%u", (unsigned)d);
	}
}


///////////////////////////////////////////////////////////////////////////////
// WriteMtb
//
// We write an instruction like these:
//      "mftbh r8"
//      "mftbl r9"
//      "mftb  r7,270"
// 
void WriteMtb(DasmData& dd, ppc_word in)
{
	if(in & 1)
		WriteInvalidInstruction(dd, in);
	else
	{
		uint32_t destRegister = GetD(in);
		uint32_t timeBaseRegister = (GetB(in) << 5) + GetA(in);
		char*    s = dd.mOperands;
		char     lh; // 'l', 'h', or '\0'

		s += EA::StdC::Snprintf(dd.mOperands, sizeof(dd.mOperands), "r%u", (unsigned)destRegister);

		switch (timeBaseRegister)
		{
			// We have a problem here. If we have a 64 bit PowerPC implementation
			// then the instruction is neither mtfbl or mftbh but simply always
			// just mftb. However, we have no way of telling from the disassembly
			// alone of we are working with a 64 bit PowerPC implementation.
			// We have to use user-supplied flags to tell us so.
			case 268:
				lh = 'l';
				break;

			case 269:
				lh = 'u';
				break;

			default:
				lh = '\0';
				dd.mInstructionFlags |= kIFSupervisor;
				EA::StdC::Sprintf(s, ",%u", (unsigned)timeBaseRegister); // e.g. mOperands => "r7,270"
				break;
		}

		EA::StdC::Snprintf(dd.mOperation, sizeof(dd.mOperation), "mftb%c", lh); // e.g. "mftbl"
	}
}


///////////////////////////////////////////////////////////////////////////////
// WriteSradi
//
void WriteSradi(DasmData& dd, ppc_word in)
{
	const uint32_t s   = GetD(in);
	const uint32_t a   = GetA(in);
	const uint32_t bsh = ((in & 2) << 4) + GetB(in);

	dd.mInstructionFlags |= kIF64Bit;
	EA::StdC::Snprintf(dd.mOperation, sizeof(dd.mOperation), "sradi%c", (in & 1) ? '.' : '\0');                     // e.g. "sradi."
	EA::StdC::Snprintf(dd.mOperands, sizeof(dd.mOperands), "r%u,r%u,%u", (unsigned)a, (unsigned)s, (unsigned)bsh); // e.g. "r8,r7,4"
}


///////////////////////////////////////////////////////////////////////////////
// WriteLdst
//
// We write load and store instructions for both the CPU and the FPU.
//
void WriteLdst(DasmData& dd, ppc_word in, const char* name, char reg, unsigned char dmode)
{
	const uint32_t s = GetD(in);
	const uint32_t a = GetA(in);
	int32_t        d = (int32_t)(uint32_t)(in & 0x0000ffff);

	dd.mInstructionType   = (name[0] == 's') ?  kITStore : kITLoad;
	dd.mInstructionFlags |= dmode;

	if(d >= 0x00008000)
		d -= 0x00010000;

	dd.mDisplacement = d;
	strcpy(dd.mOperation, name);    // e.g. "lwz"
	EA::StdC::Snprintf(dd.mOperands, sizeof(dd.mOperands), "%c%u,%u(r%u)", reg, (unsigned)s, (unsigned)d, (unsigned)a); // e.g. "r3,(16)r24",  "f4,(16)r24"    // kOFHex
}


///////////////////////////////////////////////////////////////////////////////
// WriteFdacb
//
// Write a floating point instruction of the form: <operation> fD,fA,fC,fB
// e.g. "fmadd f4,f5,f6,f7"
// Note that the order is specifically fA, fC, fB and not fA, fB, fC.
//
void WriteFdacb(DasmData& dd, ppc_word in, const char* name, int mask, unsigned char dmode)
{
	char*    s   = dd.mOperands;
	uint32_t err = 0;

	dd.mInstructionFlags |= dmode;
	EA::StdC::Snprintf(dd.mOperation, sizeof(dd.mOperation), "f%s%s", name, kStrConditionRegisterModified[in & 1]);

	s += EA::StdC::Sprintf(s, "f%u,", (unsigned)GetD(in));      // fD

	if(mask & 4)
		s += EA::StdC::Sprintf(s, "f%u,", (unsigned)GetA(in));   // fA
	else
		err |= GetA(in);

	if(mask & 1)
		s += EA::StdC::Sprintf(s, "f%u,", (unsigned)GetRotationB(in)); // fC
	else if((mask & 8) == 0)
		err |= GetRotationB(in);

	if(mask & 2)
		s += EA::StdC::Sprintf(s, "f%u,", (unsigned)GetB(in));  // fB
	else if(GetB(in))
		err |= GetB(in);

	*(s - 1) = '\0'; // '-1' in order to remove the trailing ','.

	if(err)
		WriteInvalidInstruction(dd, in);
}


///////////////////////////////////////////////////////////////////////////////
// WriteFdab
//
// Write indexed float instruction: <operation> fD,rA,rB
// e.g. "lfsx f4,r5,r6"
//
void WriteFdab(DasmData& dd, ppc_word in, const char* name, int mask)
{
	strcpy(dd.mOperation, name);
	Write_fDrArB(dd.mOperands, in, mask);
}


///////////////////////////////////////////////////////////////////////////////
// WriteFcmp
//
void WriteFcmp(DasmData& dd, ppc_word in, char c)
{
	if(in & 0x00600001)
		WriteInvalidInstruction(dd, in);
	else
	{
		EA::StdC::Snprintf(dd.mOperation, sizeof(dd.mOperation), "fcmp%c", c);
		EA::StdC::Snprintf(dd.mOperands, sizeof(dd.mOperands), "cr%u,f%u,f%u", (unsigned)GetCRD(in), (unsigned)GetA(in), (unsigned)GetB(in));
	}
}


///////////////////////////////////////////////////////////////////////////////
// WriteMtfsb
//
// Write the "move to FPSCR bit" instruction.
//
void WriteMtfsb(DasmData& dd, ppc_word in, int n)
{
	if(in & (kMaskA | kMaskB))
		WriteInvalidInstruction(dd, in);
	else
	{
		EA::StdC::Snprintf(dd.mOperation, sizeof(dd.mOperation), "mtfsb%d%s", n, kStrConditionRegisterModified[in & 1]); // e.g "mtfsb0", "mtfsb1."
		EA::StdC::Snprintf(dd.mOperands, sizeof(dd.mOperands), "%u", (unsigned)GetD(in));
	}
}


///////////////////////////////////////////////////////////////////////////////
// WriteVdab
//
// Write a standard AltiVec instruction: <operation> vD,vA,vB 
//
void WriteVdab(DasmData& dd, ppc_word in, const char* name, int mask, int chkrc)
{
	EA::StdC::Snprintf(dd.mOperation, sizeof(dd.mOperation), "%s%s", name, kStrConditionRegisterModified[chkrc && (in & kMaskAltivec)]);
	Write_vDvAvB(dd.mOperands, in, mask);
}


///////////////////////////////////////////////////////////////////////////////
// WriteVdabc
//
// Write a four operand AltiVec instruction: <operation> vD,vA,vB,vC
//
void WriteVdabc(DasmData& dd, ppc_word in)
{
	const ppc_word nameidx = (in & 0x0000003f) - 32; // index is in range of [0,31].

	if(kStrAltivec[nameidx])
	{
		EA::StdC::Snprintf(dd.mOperation, sizeof(dd.mOperation), "%s", kStrAltivec[nameidx]);
		char* s = Write_vDvAvB(dd.mOperands, in, 7);
		EA::StdC::Sprintf(s, (nameidx == 12) ? ",%u" : ",v%u", (unsigned)GetRotationB(in));
	}
	else
		WriteInvalidInstruction(dd, in);
}


///////////////////////////////////////////////////////////////////////////////
// WriteVldst
//
// Altivec load/store.
//
void WriteVldst(DasmData& dd, ppc_word in, const char* name)
{
	if(in & 1)
		WriteInvalidInstruction(dd, in);
	else
	{
		dd.mInstructionType   = (name[0] == 's') ?  kITStore : kITLoad;
		dd.mInstructionFlags |= kIFAltivec;
		strcpy(dd.mOperation, name);
		EA::StdC::Snprintf(dd.mOperands, sizeof(dd.mOperands), "v%u,r%u,r%u", (unsigned)GetD(in), (unsigned)GetA(in), (unsigned)GetB(in));
	}
}


///////////////////////////////////////////////////////////////////////////////
// WriteAltivecDataStream
//
void WriteAltivecDataStream(DasmData& dd, ppc_word in, const char* name)
{
	if(in & 0x01800001)
		WriteInvalidInstruction(dd, in);
	else
	{
		char* s = dd.mOperands;

		if(GetExtendedOpcode(in) == 822)  // dss, dssall
		{
			if(GetA(in) || GetB(in))
			{
				WriteInvalidInstruction(dd, in);
				return;
			}
			EA::StdC::Snprintf(dd.mOperation, sizeof(dd.mOperation), "d%s%s", name, (in & kMaskDestination) ? "all" : "");
		}
		else
		{
			EA::StdC::Snprintf(dd.mOperation, sizeof(dd.mOperation), "d%s%c", name, (in & kMaskDestination) ? 't' : '\0');
			s += EA::StdC::Sprintf(s, "r%u,r%u,", (unsigned)GetA(in), (unsigned)GetB(in));
		}

		EA::StdC::Sprintf(s, "%u", (unsigned)GetAltivecStream(in));
		dd.mInstructionFlags |= kIFAltivec;
	}
}


///////////////////////////////////////////////////////////////////////////////
// WriteInstruction
//
void WriteInstruction(ppc_word in, DasmData& dd)
{
	dd.mInstructionType  = kITMisc;
	dd.mInstructionFlags = kIFNone;

	switch (GetOpcode(in))
	{
		case 2:
			WriteTrapi(dd, in, kIF64Bit);  // tdi 
			break;

		case 3:
			WriteTrapi(dd, in, kIFNone);  // twi 
			break;

		case 4:
		{
			dd.mInstructionFlags |= kIFAltivec;

			if((in & 0x30) == 0x20)
			{
				// vxxx vA,vB,vC,vD  - four mOperands instructions
				WriteVdabc(dd, in);
				break;
			}

			if(in & 1)
			{
				WriteInvalidInstruction(dd, in);
				break;
			}

			const uint32_t extendedOpcode = (GetExtendedOpcode(in) << 1);

			switch (extendedOpcode)
			{
				case 0:
					WriteVdab(dd, in, "vaddubm", 7, 0);
					break;

				case 2:
					WriteVdab(dd, in, "vmaxub", 7, 0);
					break;

				case 4:
					WriteVdab(dd, in, "vrlb", 7, 0);
					break;

				case 6:
				case 6 + kMaskAltivec:
					WriteVdab(dd, in, "vcmpequb", 7, 1);
					break;

				case 8:
					WriteVdab(dd, in, "vmuloub", 7, 0);
					break;

				case 10:
					WriteVdab(dd, in, "vaddfp", 7, 0);
					break;

				case 12:
					WriteVdab(dd, in, "vmrghb", 7, 0);
					break;

				case 14:
					WriteVdab(dd, in, "vpkuhum", 7, 0);
					break;

				case 64:
					WriteVdab(dd, in, "vadduhm", 7, 0);
					break;

				case 66:
					WriteVdab(dd, in, "vmaxuh", 7, 0);
					break;

				case 68:
					WriteVdab(dd, in, "vrlh", 7, 0);
					break;

				case 70:
				case 70 + kMaskAltivec:
					WriteVdab(dd, in, "vcmpequh", 7, 1);
					break;

				case 72:
					WriteVdab(dd, in, "vmulouh", 7, 0);
					break;

				case 74:
					WriteVdab(dd, in, "vsubfp", 7, 0);
					break;

				case 76:
					WriteVdab(dd, in, "vmrghh", 7, 0);
					break;

				case 78:
					WriteVdab(dd, in, "vpkuwum", 7, 0);
					break;

				case 128:
					WriteVdab(dd, in, "vadduwm", 7, 0);
					break;

				case 130:
					WriteVdab(dd, in, "vmaxuw", 7, 0);
					break;

				case 132:
					WriteVdab(dd, in, "vrlw", 7, 0);
					break;

				case 134:
				case 134 + kMaskAltivec:
					WriteVdab(dd, in, "vcmpequw", 7, 1);
					break;

				case 140:
					WriteVdab(dd, in, "vmrghw", 7, 0);
					break;

				case 142:
					WriteVdab(dd, in, "vpkuhus", 7, 0);
					break;

				case 198:
				case 198 + kMaskAltivec:
					WriteVdab(dd, in, "vcmpeqfp", 7, 1);
					break;

				case 206:
					WriteVdab(dd, in, "vpkuwus", 7, 0);
					break;

				case 258:
					WriteVdab(dd, in, "vmaxsb", 7, 0);
					break;

				case 260:
					WriteVdab(dd, in, "vslb", 7, 0);
					break;

				case 264:
					WriteVdab(dd, in, "vmulosb", 7, 0);
					break;

				case 266:
					WriteVdab(dd, in, "vrefp", 5, 0);
					break;

				case 268:
					WriteVdab(dd, in, "vmrglb", 7, 0);
					break;

				case 270:
					WriteVdab(dd, in, "vpkshus", 7, 0);
					break;

				case 322:
					WriteVdab(dd, in, "vmaxsh", 7, 0);
					break;

				case 324:
					WriteVdab(dd, in, "vslh", 7, 0);
					break;

				case 328:
					WriteVdab(dd, in, "vmulosh", 7, 0);
					break;

				case 330:
					WriteVdab(dd, in, "vrsqrtefp", 5, 0);
					break;

				case 332:
					WriteVdab(dd, in, "vmrglh", 7, 0);
					break;

				case 334:
					WriteVdab(dd, in, "vpkswus", 7, 0);
					break;

				case 384:
					WriteVdab(dd, in, "vaddcuw", 7, 0);
					break;

				case 386:
					WriteVdab(dd, in, "vmaxsw", 7, 0);
					break;

				case 388:
					WriteVdab(dd, in, "vslw", 7, 0);
					break;

				case 394:
					WriteVdab(dd, in, "vexptefp", 5, 0);
					break;

				case 396:
					WriteVdab(dd, in, "vmrglw", 7, 0);
					break;

				case 398:
					WriteVdab(dd, in, "vpkshss", 7, 0);
					break;

				case 452:
					WriteVdab(dd, in, "vsl", 7, 0);
					break;

				case 454:
				case 454 + kMaskAltivec:
					WriteVdab(dd, in, "vcmpgefp", 7, 1);
					break;

				case 458:
					WriteVdab(dd, in, "vlogefp", 5, 0);
					break;

				case 462:
					WriteVdab(dd, in, "vpkswss", 7, 0);
					break;

				case 512:
					WriteVdab(dd, in, "vaddubs", 7, 0);
					break;

				case 514:
					WriteVdab(dd, in, "vminub", 7, 0);
					break;

				case 516:
					WriteVdab(dd, in, "vsrb", 7, 0);
					break;

				case 518:
				case 518 + kMaskAltivec:
					WriteVdab(dd, in, "vcmpgtub", 7, 1);
					break;

				case 520:
					WriteVdab(dd, in, "vmuleub", 7, 0);
					break;

				case 522:
					WriteVdab(dd, in, "vrfin", 5, 0);
					break;

				case 524:
					WriteVdab(dd, in, "vspltb", 15, 0);
					break;

				case 526:
					WriteVdab(dd, in, "vupkhsb", 5, 0);
					break;

				case 576:
					WriteVdab(dd, in, "vadduhs", 7, 0);
					break;

				case 578:
					WriteVdab(dd, in, "vminuh", 7, 0);
					break;

				case 580:
					WriteVdab(dd, in, "vsrh", 7, 0);
					break;

				case 582:
				case 582 + kMaskAltivec:
					WriteVdab(dd, in, "vcmpgtuh", 7, 1);
					break;

				case 584:
					WriteVdab(dd, in, "vmuleuh", 7, 0);
					break;

				case 586:
					WriteVdab(dd, in, "vrfiz", 5, 0);
					break;

				case 588:
					WriteVdab(dd, in, "vsplth", 15, 0);
					break;

				case 590:
					WriteVdab(dd, in, "vupkhsh", 5, 0);
					break;

				case 640:
					WriteVdab(dd, in, "vadduws", 7, 0);
					break;

				case 642:
					WriteVdab(dd, in, "vminuw", 7, 0);
					break;

				case 644:
					WriteVdab(dd, in, "vsrw", 7, 0);
					break;

				case 646:
				case 646 + kMaskAltivec:
					WriteVdab(dd, in, "vcmpgtuw", 7, 1);
					break;

				case 650:
					WriteVdab(dd, in, "vrfip", 5, 0);
					break;

				case 652:
					WriteVdab(dd, in, "vspltw", 15, 0);
					break;

				case 654:
					WriteVdab(dd, in, "vupklsb", 5, 0);
					break;

				case 708:
					WriteVdab(dd, in, "vsr", 7, 0);
					break;

				case 710:
				case 710 + kMaskAltivec:
					WriteVdab(dd, in, "vcmpgtfp", 7, 1);
					break;

				case 714:
					WriteVdab(dd, in, "vrfim", 5, 0);
					break;

				case 718:
					WriteVdab(dd, in, "vupklsh", 5, 0);
					break;

				case 768:
					WriteVdab(dd, in, "vaddsbs", 7, 0);
					break;

				case 770:
					WriteVdab(dd, in, "vminsb", 7, 0);
					break;

				case 772:
					WriteVdab(dd, in, "vsrab", 7, 0);
					break;

				case 774:
				case 774 + kMaskAltivec:
					WriteVdab(dd, in, "vcmpgtsb", 7, 1);
					break;

				case 776:
					WriteVdab(dd, in, "vmulesb", 7, 0);
					break;

				case 778:
					WriteVdab(dd, in, "vcfux", 15, 0);
					break;

				case 780:
					WriteVdab(dd, in, "vspltisb", 22, 0);
					break;

				case 782:
					WriteVdab(dd, in, "vpkpx", 7, 0);
					break;

				case 832:
					WriteVdab(dd, in, "vaddshs", 7, 0);
					break;

				case 834:
					WriteVdab(dd, in, "vminsh", 7, 0);
					break;

				case 836:
					WriteVdab(dd, in, "vsrah", 7, 0);
					break;

				case 838:
				case 838 + kMaskAltivec:
					WriteVdab(dd, in, "vcmpgtsh", 7, 1);
					break;

				case 840:
					WriteVdab(dd, in, "vmulesh", 7, 0);
					break;

				case 842:
					WriteVdab(dd, in, "vcfsx", 15, 0);
					break;

				case 844:
					WriteVdab(dd, in, "vspltish",22, 0);
					break;

				case 846:
					WriteVdab(dd, in, "vupkhpx", 5, 0);
					break;

				case 896:
					WriteVdab(dd, in, "vaddsws", 7, 0);
					break;

				case 898:
					WriteVdab(dd, in, "vminsw", 7, 0);
					break;

				case 900:
					WriteVdab(dd, in, "vsraw", 7, 0);
					break;

				case 902:
				case 902 + kMaskAltivec:
					WriteVdab(dd, in, "vcmpgtsw", 7, 1);
					break;

				case 906:
					WriteVdab(dd, in, "vctuxs", 15, 0);
					break;

				case 908:
					WriteVdab(dd, in, "vspltisw", 22, 0);
					break;

				case 966:
				case 966 + kMaskAltivec:
					WriteVdab(dd, in, "vcmpbfp", 7, 1);
					break;

				case 970:
					WriteVdab(dd, in, "vctsxs", 15, 0);
					break;

				case 974:
					WriteVdab(dd, in, "vupklpx", 5, 0);
					break;

				case 1024:
					WriteVdab(dd, in, "vsububm", 7, 0);
					break;

				case 1026:
					WriteVdab(dd, in, "vavgub", 7, 0);
					break;

				case 1028:
					WriteVdab(dd, in, "vand", 7, 0);
					break;

				case 1034:
					WriteVdab(dd, in, "vmaxfp", 7, 0);
					break;

				case 1036:
					WriteVdab(dd, in, "vslo", 7, 0);
					break;

				case 1088:
					WriteVdab(dd, in, "vsubuhm", 7, 0);
					break;

				case 1090:
					WriteVdab(dd, in, "vavguh", 7, 0);
					break;

				case 1092:
					WriteVdab(dd, in, "vandc", 7, 0);
					break;

				case 1098:
					WriteVdab(dd, in, "vminfp", 7, 0);
					break;

				case 1100:
					WriteVdab(dd, in, "vsro", 7, 0);
					break;

				case 1152:
					WriteVdab(dd, in, "vsubuwm", 7, 0);
					break;

				case 1154:
					WriteVdab(dd, in, "vavguw", 7, 0);
					break;

				case 1156:
					WriteVdab(dd, in, "vor", 7, 0);
					break;

				case 1220:
					WriteVdab(dd, in, "vxor", 7, 0);
					break;

				case 1282:
					WriteVdab(dd, in, "vavgsb", 7, 0);
					break;

				case 1284:
					WriteVdab(dd, in, "vnor", 7, 0);
					break;

				case 1346:
					WriteVdab(dd, in, "vavgsh", 7, 0);
					break;

				case 1408:
					WriteVdab(dd, in, "vsubcuw", 7, 0);
					break;

				case 1410:
					WriteVdab(dd, in, "vavgsw", 7, 0);
					break;

				case 1536:
					WriteVdab(dd, in, "vsububs", 7, 0);
					break;

				case 1540:
					WriteVdab(dd, in, "mfvscr", 4, 0);
					break;

				case 1544:
					WriteVdab(dd, in, "vsum4ubs", 7, 0);
					break;

				case 1600:
					WriteVdab(dd, in, "vsubuhs", 7, 0);
					break;

				case 1604:
					WriteVdab(dd, in, "mtvscr", 1, 0);
					break;

				case 1608:
					WriteVdab(dd, in, "vsum4shs", 7, 0);
					break;

				case 1664:
					WriteVdab(dd, in, "vsubuws", 7, 0);
					break;

				case 1672:
					WriteVdab(dd, in, "vsum2sws", 7, 0);
					break;

				case 1792:
					WriteVdab(dd, in, "vsubsbs", 7, 0);
					break;

				case 1800:
					WriteVdab(dd, in, "vsum4sbs", 7, 0);
					break;

				case 1856:
					WriteVdab(dd, in, "vsubshs", 7, 0);
					break;

				case 1920:
					WriteVdab(dd, in, "vsubsws", 7, 0);
					break;

				case 1928:
					WriteVdab(dd, in, "vsumsws", 7, 0);
					break;

				default:
					WriteInvalidInstruction(dd, in);
					break;
			}

			break;
		}

		case 7: 
			strcpy(dd.mOperation, "mulli");
			WriteImmediateOperands(dd, in, 0, 0);
			break;

		case 8:
			strcpy(dd.mOperation, "subfic");
			WriteImmediateOperands(dd, in, 0, 0);
			break;

		case 10:
			WriteCmpi(dd, in, 1);  // cmpli 
			break;

		case 11:
			WriteCmpi(dd, in, 0);  // cmpi 
			break;

		case 12:
			WriteAddi(dd, in, "ic");  // addic 
			break;

		case 13:
			WriteAddi(dd, in, "ic.");  // addic. 
			break;

		case 14:
			WriteAddi(dd, in, "i");  // WriteAddi 
			break;

		case 15:
			WriteAddi(dd, in, "is");  // addis 
			break;

		case 16:
			WriteBranchConditional(dd, in);
			break;

		case 17:
			if((in & ~kMaskOpcode) == 2)
				strcpy(dd.mOperation,"sc");
			else
				WriteInvalidInstruction(dd, in);
			break;

		case 18:
			WriteBli(dd, in);
			break;

		case 19:
			switch (GetExtendedOpcode(in))
			{
				case 0:
					WriteMcrf(dd, in, '\0');  // mcrf 
					break;

				case 16:
					WriteBranch(dd, in, "lr", 0, 0);  // bclr 
					break;

				case 33:
					WriteCROperation(dd, in, "nor", "not");  // crnor 
					break;

				case 50:
					WriteNoOperandInstruction(dd, in, "rfi", kIFSupervisor);
					break;

				case 129:
					WriteCROperation(dd, in, "andc", NULL);  // crandc 
					break;

				case 150:
					WriteNoOperandInstruction(dd, in, "isync", 0);
					break;

				case 193:
					WriteCROperation(dd, in, "xor", "clr");  // crxor 
					break;

				case 225:
					WriteCROperation(dd, in, "nand", NULL);  // crnand 
					break;

				case 257:
					WriteCROperation(dd, in, "and", NULL);  // crand 
					break;

				case 289:
					WriteCROperation(dd, in, "eqv", "set");  // creqv 
					break;

				case 417:
					WriteCROperation(dd, in, "orc", NULL);  // crorc 
					break;

				case 449:
					WriteCROperation(dd, in, "or", "move");  // cror 
					break;

				case 528:
					WriteBranch(dd, in, "ctr", 0, 0);  // bcctr 
					break;

				default:
					WriteInvalidInstruction(dd, in);
					break;
			}
			break;

		case 20:
			WriteRlw(dd, in, "imi", 0);  // rlwimi 
			break;

		case 21:
			WriteRlw(dd, in, "inm", 0);  // rlwinm 
			break;

		case 23:
			WriteRlw(dd, in, "nm", 1);  // rlwnm 
			break;

		case 24:
		{
			// Note: "ori r0,r0,0" is the standard nop, but "ori r1,r1,0" is also a nop.
			const bool bIsNop = ((GetUIMM(in) == 0) && (GetD(in)) == GetA(in)); // Any register ORd against itself is a nop.

			if(bIsNop)
				strcpy(dd.mOperation, "nop");
			else
				WriteOri(dd, in, "ori");
			break;
		}

		case 25:
			WriteOri(dd, in, "oris");
			break;

		case 26:
			WriteOri(dd, in, "xori");
			break;

		case 27:
			WriteOri(dd, in, "xoris");
			break;

		case 28:
			WriteOri(dd, in, "andi.");
			break;

		case 29:
			WriteOri(dd, in, "andis.");
			break;

		case 30:
			switch (in & 0x1c)  // 0x1c == 0x00011100
			{
				case 0:
					WriteRld(dd, in, "icl", 0);  // rldicl 
					break;

				// These commented-out cases are impossible, as 0x1c above don't allow it. Question: is the 0x1c mask above correct? I think it must be wrong.
				//case 1:
				//    WriteRld(dd, in, "icr", 0);  // rldicr 
				//    break;

				//case 2:
				//    WriteRld(dd, in, "ic", 0);   // rldic 
				//    break;

				//case 3:
				//    WriteRld(dd, in, "imi", 0);  // rldimi 
				//    break;

				case 4:
					WriteRld(dd, in, (in & 2) ? "cl" : "cr", 1);  // rldcl, rldcr 
					break;

				default:
					WriteInvalidInstruction(dd, in);
					break;
			}
			break;

		case 31:
			switch (GetExtendedOpcode(in))
			{
				case 0:
				case 32:
					if(in & 1)
						WriteInvalidInstruction(dd, in);
					else
						WriteCmp(dd, in);  // cmp, cmpl 
					break;

				case 4:
					if(in & 1)
						WriteInvalidInstruction(dd, in);
					else
						WriteTrap(dd, in, 0);  // tw 
					break;

				case 6:
					WriteVldst(dd, in, "lvsl");  // AltiVec 
					break;

				case 7:
					WriteVldst(dd, in, "lvebx");  // AltiVec 
					break;

				case 8:
				case (kMaskOE >> 1) + 8:
					WriteThreeRegisterOperation(dd, SwapAB(in), "subc", 7, 0, 1, -1, 0);
					break;

				case 9:
					WriteThreeRegisterOperation(dd, in, "mulhdu", 7, 0, 0, -1, kIF64Bit);
					break;

				case 10:
				case (kMaskOE >> 1) + 10:
					WriteThreeRegisterOperation(dd, in, "addc", 7, 0, 1, -1, 0);
					break;

				case 11:
					WriteThreeRegisterOperation(dd, in, "mulhwu", 7, 0, 0, -1, 0);
					break;

				case 19:
					if(in & (kMaskA | kMaskB))
						WriteInvalidInstruction(dd, in);
					else
						WriteThreeRegisterOperation(dd, in, "mfcr", 4, 0, 0, 0, 0);
					break;

				case 20:
					WriteThreeRegisterOperation(dd, in, "lwarx", 7, 0, 0, 0, 0); // This should probably have kITLoad.
					break;

				case 21:
					WriteThreeRegisterOperation(dd, in, "ldx", 7, 0, 0, 0, kIF64Bit); // This should probably have kITLoad.
					break;

				case 23:
					WriteThreeRegisterOperation(dd, in, "lwzx", 7, 0, 0, 0, 0); // This should probably have kITLoad.
					break;

				case 24:
					WriteThreeRegisterOperation(dd, in, "slw", 7, 1, 0, -1, 0); // This should probably have kITStore.
					break;

				case 26:
					if(in & kMaskB)
						WriteInvalidInstruction(dd, in);
					else
						WriteThreeRegisterOperation(dd, in, "cntlzw", 6, 1, 0, -1, 0);
					break;

				case 27:
					WriteThreeRegisterOperation(dd, in, "sld", 7, 1, 0, -1, kIF64Bit);
					break;

				case 28:
					// To consider: A register ANDed against itself is a nop. May possibly want to label it as so.
					WriteThreeRegisterOperation(dd, in, "and", 7, 1, 0, -1, 0);
					break;

				case 38:
					WriteVldst(dd, in, "lvsr");  // AltiVec 
					break;

				case 39:
					WriteVldst(dd, in, "lvehx");  // AltiVec 
					break;

				case 40:
				case (kMaskOE >> 1) + 40:
					WriteThreeRegisterOperation(dd, SwapAB(in), "sub", 7, 0, 1, -1, 0);
					break;

				case 53:
					WriteThreeRegisterOperation(dd, in, "ldux", 7, 0, 0, 0, kIF64Bit);
					break;

				case 54:
					if(in & kMaskD)
						WriteInvalidInstruction(dd, in);
					else
						WriteThreeRegisterOperation(dd, in, "dcbst", 3, 0, 0, 0, 0);
					break;

				case 55:
					WriteThreeRegisterOperation(dd, in, "lwzux", 7, 0, 0, 0, 0);
					break;

				case 58:
					if(in & kMaskB)
						WriteInvalidInstruction(dd, in);
					else
						WriteThreeRegisterOperation(dd, in, "cntlzd", 6, 1, 0, -1, kIF64Bit);
					break;

				case 60:
					WriteThreeRegisterOperation(dd, in, "andc", 7, 1, 0, -1, 0);
					break;

				case 68:
					WriteTrap(dd, in, kIF64Bit);  // td 
					break;

				case 71:
					WriteVldst(dd, in, "lvewx");  // AltiVec 
					break;

				case 73:
					WriteThreeRegisterOperation(dd, in, "mulhd", 7, 0, 0, -1, kIF64Bit);
					break;

				case 75:
					WriteThreeRegisterOperation(dd, in, "mulhw", 7, 0, 0, -1, 0);
					break;

				case 83:
					if(in & (kMaskA | kMaskB))
						WriteInvalidInstruction(dd, in);
					else
						WriteThreeRegisterOperation(dd, in, "mfmsr", 4, 0, 0, 0, kIFSupervisor);
					break;

				case 84:
					WriteThreeRegisterOperation(dd, in, "ldarx", 7, 0, 0, 0, kIF64Bit); // This should probably have kITLoad.
					break;

				case 86:
					if(in & kMaskD)
						WriteInvalidInstruction(dd, in);
					else
						WriteThreeRegisterOperation(dd, in, "dcbf", 3, 0, 0, 0, 0);
					break;

				case 87:
					WriteThreeRegisterOperation(dd, in, "lbzx", 7, 0, 0, 0, 0); // This should probably have kITLoad.
					break;

				case 103:
					WriteVldst(dd, in, "lvx");  // AltiVec 
					break;

				case 104:
				case (kMaskOE >> 1) + 104:
					if(in & kMaskB)
						WriteInvalidInstruction(dd, in);
					else
						WriteThreeRegisterOperation(dd, in, "neg", 6, 0, 1, -1, 0);
					break;

				case 119:
					WriteThreeRegisterOperation(dd, in, "lbzux", 7, 0, 0, 0, 0);
					break;

				case 124:
					if(GetD(in) == GetB(in))
						WriteThreeRegisterOperation(dd, in, "not", 6, 1, 0, -1, 0);
					else
						WriteThreeRegisterOperation(dd, in, "nor", 7, 1, 0, -1, 0);
					break;

				case 135:
					WriteVldst(dd, in, "stvebx");  // AltiVec 
					break;

				case 136:
				case (kMaskOE >> 1) + 136:
					WriteThreeRegisterOperation(dd, in, "subfe", 7, 0, 1, -1, 0);
					break;

				case 138:
				case (kMaskOE >> 1) + 138:
					WriteThreeRegisterOperation(dd, in, "adde", 7, 0, 1, -1, 0);
					break;

				case 144:
					WriteMtcr(dd, in);
					break;

				case 146:
					if(in & (kMaskA | kMaskB))
						WriteInvalidInstruction(dd, in);
					else
						WriteThreeRegisterOperation(dd, in, "mtmsr", 4, 0, 0, 0, kIFSupervisor);
					break;

				case 149:
					WriteThreeRegisterOperation(dd, in, "stdx", 7, 0, 0, 0, kIF64Bit); // This should probably have kITStore.
					break;

				case 150:
					WriteThreeRegisterOperation(dd, in, "stwcx.", 7, 0, 0, 1, 0); // This should probably have kITStore.
					break;

				case 151:
					WriteThreeRegisterOperation(dd, in, "stwx", 7, 0, 0, 0, 0); // This should probably have kITStore.
					break;

				case 167:
					WriteVldst(dd, in, "stvehx");  // AltiVec 
					break;

				case 181:
					WriteThreeRegisterOperation(dd, in, "stdux", 7, 0, 0, 0, kIF64Bit); // This should probably have kITStore.
					break;

				case 183:
					WriteThreeRegisterOperation(dd, in, "stwux", 7, 0, 0, 0, 0); // This should probably have kITStore.
					break;

				case 199:
					WriteVldst(dd, in, "stvewx");  // AltiVec 
					break;

				case 200:
				case (kMaskOE >> 1) + 200:
					if(in & kMaskB)
						WriteInvalidInstruction(dd, in);
					else
						WriteThreeRegisterOperation(dd, in, "subfze", 6, 0, 1, -1, 0);
					break;

				case 202:
				case (kMaskOE >> 1) + 202:
					if(in & kMaskB)
						WriteInvalidInstruction(dd, in);
					else
						WriteThreeRegisterOperation(dd, in, "addze", 6, 0, 1, -1, 0);
					break;

				case 210:
					WriteMsr(dd, in, 1);  // mfsr 
					break;

				case 214:
					WriteThreeRegisterOperation(dd, in, "stdcx.", 7, 0, 0, 1, kIF64Bit);
					break;

				case 215:
					WriteThreeRegisterOperation(dd, in, "stbx", 7, 0, 0, 0, 0);
					break;

				case 231:
					WriteVldst(dd, in, "stvx");  // AltiVec 
					break;

				case 232:
				case (kMaskOE >> 1) + 232:
					if(in & kMaskB)
						WriteInvalidInstruction(dd, in);
					else
						WriteThreeRegisterOperation(dd, in, "subfme", 6, 0, 1, -1, 0);
					break;

				case 233:
				case (kMaskOE >> 1) + 233:
					WriteThreeRegisterOperation(dd, in, "mulld", 7, 0, 1, -1, kIF64Bit);
					break;

				case 234:
				case (kMaskOE >> 1) + 234:
					if(in & kMaskB)
						WriteInvalidInstruction(dd, in);
					else
						WriteThreeRegisterOperation(dd, in, "addme", 6, 0, 1, -1, 0);
					break;

				case 235:
				case (kMaskOE >> 1) + 235:
					WriteThreeRegisterOperation(dd, in, "mullw", 7, 0, 1, -1, 0);
					break;

				case 242:
					if(in & kMaskA)
						WriteInvalidInstruction(dd, in);
					else
						WriteThreeRegisterOperation(dd, in, "mtsrin", 5, 0, 0, 0, kIFSupervisor);
					break;

				case 246:
					if(in & kMaskD)
						WriteInvalidInstruction(dd, in);
					else
						WriteThreeRegisterOperation(dd, in, "dcbtst", 3, 0, 0, 0, 0);
					break;

				case 247:
					WriteThreeRegisterOperation(dd, in, "stbux", 7, 0, 0, 0, 0);
					break;

				case 266:
				case (kMaskOE >> 1) + 266:
					WriteThreeRegisterOperation(dd, in, "add", 7, 0, 1, -1, 0);
					break;

				case 278:
					if(in & kMaskD)
						WriteInvalidInstruction(dd, in);
					else
						WriteThreeRegisterOperation(dd, in, "dcbt", 3, 0, 0, 0, 0);
					break;

				case 279:
					WriteThreeRegisterOperation(dd, in, "lhzx", 7, 0, 0, 0, 0);
					break;

				case 284:
					WriteThreeRegisterOperation(dd, in, "eqv", 7, 1, 0, -1, 0);
					break;

				case 306:
					if(in & (kMaskD | kMaskA))
						WriteInvalidInstruction(dd, in);
					else
						WriteThreeRegisterOperation(dd, in, "tlbie", 1, 0, 0, 0, kIFSupervisor);
					break;

				case 310:
					WriteThreeRegisterOperation(dd, in, "eciwx", 7, 0, 0, 0, 0);
					break;

				case 311:
					WriteThreeRegisterOperation(dd, in, "lhzux", 7, 0, 0, 0, 0);
					break;

				case 316:
					WriteThreeRegisterOperation(dd, in, "xor", 7, 1, 0, -1, 0);
					break;

				case 339:
					WriteMspr(dd, in, 0);  // mfspr 
					break;

				case 341:
					WriteThreeRegisterOperation(dd, in, "lwax", 7, 0, 0, 0, kIF64Bit);
					break;

				case 342:
					WriteAltivecDataStream(dd, in, "st");  // AltiVec Stream 
					break;

				case 343:
					WriteThreeRegisterOperation(dd, in, "lhax", 7, 0, 0, 0, 0);
					break;

				case 359:
					WriteVldst(dd, in, "lvxl");  // AltiVec 
					break;

				case 370:
					WriteNoOperandInstruction(dd, in, "tlbia", kIFSupervisor);
					break;

				case 371:
					WriteMtb(dd, in);  // mftb 
					break;

				case 373:
					WriteThreeRegisterOperation(dd, in, "lwaux", 7, 0, 0, 0, kIF64Bit);
					break;

				case 374:
					WriteAltivecDataStream(dd, in, "stst");  // AltiVec Stream 
					break;

				case 375:
					WriteThreeRegisterOperation(dd, in, "lhaux", 7, 0, 0, 0, 0);
					break;

				case 407:
					WriteThreeRegisterOperation(dd, in, "sthx", 7, 0, 0, 0, 0);
					break;

				case 412:
					WriteThreeRegisterOperation(dd, in, "orc", 7, 1, 0, -1, 0);
					break;

				case 413:
					WriteSradi(dd, in);  // sradi 
					break;

				case 434:
					if(in & (kMaskD | kMaskA))
						WriteInvalidInstruction(dd, in);
					else
						WriteThreeRegisterOperation(dd, in, "slbie", 1, 0, 0, 0, kIFSupervisor | kIF64Bit);
					break;

				case 438:
					WriteThreeRegisterOperation(dd, in, "ecowx", 7, 0, 0, 0, 0);
					break;

				case 439:
					WriteThreeRegisterOperation(dd, in, "sthux", 7, 0, 0, 0, 0);
					break;

				case 444:
					if(GetD(in) == GetB(in))
					{
						// Detect the PowerPC hyperthreading mnemonics. These aren't documented in the 
						// standard IBM instruction set manual for PowerPC but are documented elsewhere.
						// Nevertheless, they are applicable to the PowerPCs that we use.

						// Mnemonic     Operation           Other mnemonic      Meaning
						// cctpl        or r1, r1, r1       mr r1, r1           Change current thread priority to low.
						// cctpm        or r2, r2, r2       mr r2, r2           Change current thread priority to medium.
						// cctph        or r3, r3, r3       mr r3, r3           Change current thread priority to high.
						// dbcyc8       or r28, r28, r28    mr r28, r28         Cause the current thread to block at dispatch for eight cycles.
						// dbcyc10      or r29, r29, r29    mr r29, r29         Cause the current thread to block at dispatch for 10 cycles.
						// dbcyc12      or r30, r30, r30    mr r30, r30         Cause the current thread to block at dispatch for 12 cycles.
						// dbcyc16      or r31, r31, r31    mr r31, r31         Cause the current thread to block at dispatch for 16 cycles.

						const uint32_t d = GetD(in);
						const uint32_t a = GetA(in);
						const uint32_t b = GetB(in);

						if((d == a) && (a == b)) // If all three registers are the same...
						{
							switch (d)
							{
								case 1:
									strcpy(dd.mOperation, "cctpl (mr r1, r1)");
									break;
								case 2:
									strcpy(dd.mOperation, "cctpm (mr r2, r2)");
									break;
								case 3:
									strcpy(dd.mOperation, "cctph (mr r3, r3)");
									break;
								case 28:
									strcpy(dd.mOperation, "dbcyc8 (mr r28, r28)");
									break;
								case 29:
									strcpy(dd.mOperation, "dbcyc10 (mr r29, r29)");
									break;
								case 30:
									strcpy(dd.mOperation, "dbcyc12 (mr r30, r30)");
									break;
								case 31:
									strcpy(dd.mOperation, "dbcyc16 (mr r31, r31)");
									break;
								default:
									// All other values have no special meaning as of this writing.
									WriteThreeRegisterOperation(dd, in, "mr", 6, 1, 0, -1, 0);
									break;
							}
						}
						else
							WriteThreeRegisterOperation(dd, in, "mr", 6, 1, 0, -1, 0);
					}
					else
						WriteThreeRegisterOperation(dd, in, "or", 7, 1, 0, -1, 0);
					break;

				case 457:
				case (kMaskOE >> 1) + 457:
					WriteThreeRegisterOperation(dd, in, "divdu", 7, 0, 1, -1, kIF64Bit);
					break;

				case 459:
				case (kMaskOE >> 1) + 459:
					WriteThreeRegisterOperation(dd, in, "divwu", 7, 0, 1, -1, 0);
					break;

				case 467:
					WriteMspr(dd, in, 1);  // mtspr
					break;

				case 470:
					if(in & kMaskD)
						WriteInvalidInstruction(dd, in);
					else
						WriteThreeRegisterOperation(dd, in, "dcbi", 3, 0, 0, 0, 0);
					break;

				case 476:
					WriteThreeRegisterOperation(dd, in, "nand", 7, 1, 0, -1, 0);
					break;

				case 487:
					WriteVldst(dd, in, "stvxl");  // AltiVec 
					break;

				case 489:
				case (kMaskOE>>1) + 489:
					WriteThreeRegisterOperation(dd, in, "divd", 7, 0, 1, -1, kIF64Bit);
					break;

				case 491:
				case (kMaskOE >> 1) + 491:
					WriteThreeRegisterOperation(dd, in, "divw", 7, 0, 1, -1, 0);
					break;

				case 498:
					WriteNoOperandInstruction(dd, in, "slbia", kIFSupervisor|kIF64Bit);
					break;

				case 512:
					if(in & 0x007ff801)
						WriteInvalidInstruction(dd, in);
					else 
					{
						strcpy(dd.mOperation,"mcrxr");
						EA::StdC::Snprintf(dd.mOperands, sizeof(dd.mOperands), "cr%u", (unsigned)GetCRD(in));
					}
					break;

				case 533:
					WriteThreeRegisterOperation(dd, in, "lswx", 7, 0, 0, 0, 0);
					break;

				case 534:
					WriteThreeRegisterOperation(dd, in, "lwbrx", 7, 0, 0, 0, 0);
					break;

				case 535:
					WriteFdab(dd, in, "lfsx",7);
					break;

				case 536:
					WriteThreeRegisterOperation(dd, in, "srw", 7, 1, 0, -1, 0);
					break;

				case 539:
					WriteThreeRegisterOperation(dd, in, "srd", 7, 1, 0, -1, kIF64Bit);
					break;

				case 566:
					WriteNoOperandInstruction(dd, in, "tlbsync", kIFSupervisor);
					break;

				case 567:
					WriteFdab(dd, in, "lfsux",7);
					break;

				case 595:
					WriteMsr(dd, in, 0);  // mfsr 
					break;

				case 597:
					WriteTwoRegisterOperation(dd, in, "lswi", 0, 0, 0, 0);
					break;

				case 598:
					WriteNoOperandInstruction(dd, in, "sync", kIFSupervisor);
					break;

				case 599:
					WriteFdab(dd, in, "lfdx",7);
					break;

				case 631:
					WriteFdab(dd, in, "lfdux",7);
					break;

				case 659:
					if(in & kMaskA)
						WriteInvalidInstruction(dd, in);
					else
						WriteThreeRegisterOperation(dd, in, "mfsrin", 5, 0, 0, 0, kIFSupervisor);
					break;

				case 661:
					WriteThreeRegisterOperation(dd, in, "stswx", 7, 0, 0, 0, 0);
					break;

				case 662:
					WriteThreeRegisterOperation(dd, in, "stwbrx", 7, 0, 0, 0, 0);
					break;

				case 663:
					WriteFdab(dd, in, "stfsx",7);
					break;

				case 695:
					WriteFdab(dd, in, "stfsux",7);
					break;

				case 725:
					WriteTwoRegisterOperation(dd, in, "stswi", 0, 0, 0, 0);
					break;

				case 727:
					WriteFdab(dd, in, "stfdx",7);
					break;

				case 759:
					WriteFdab(dd, in, "stfdux",7);
					break;

				case 790:
					WriteThreeRegisterOperation(dd, in, "lhbrx", 7, 0, 0, 0, 0);
					break;

				case 792:
					WriteThreeRegisterOperation(dd, in, "sraw", 7, 1, 0, -1, 0);
					break;

				case 794:
					WriteThreeRegisterOperation(dd, in, "srad", 7, 1, 0, -1, kIF64Bit);
					break;

				case 822:
					WriteAltivecDataStream(dd, in, "ss");  // AltiVec Stream 
					break;

				case 824:
					WriteTwoRegisterOperation(dd, in, "srawi", 1, 0, -1, 0);
					break;

				case 854:
					WriteNoOperandInstruction(dd, in, "eieio", kIFSupervisor);
					break;

				case 918:
					WriteThreeRegisterOperation(dd, in, "sthbrx", 7, 0, 0, 0, 0);
					break;

				case 922:
					if(in & kMaskB)
						WriteInvalidInstruction(dd, in);
					else
						WriteThreeRegisterOperation(dd, in, "extsh", 6, 1, 0, -1, 0);
					break;

				case 954:
					if(in & kMaskB)
						WriteInvalidInstruction(dd, in);
					else
						WriteThreeRegisterOperation(dd, in, "extsb", 6, 1, 0, -1, 0);
					break;

				case 982:
					if(in & kMaskD)
						WriteInvalidInstruction(dd, in);
					else
						WriteThreeRegisterOperation(dd, in, "icbi", 3, 0, 0, 0, 0);
					break;

				case 983:
					WriteFdab(dd, in, "stfiwx",7);
					break;

				case 986:
					if(in & kMaskB)
						WriteInvalidInstruction(dd, in);
					else
						WriteThreeRegisterOperation(dd, in, "extsw", 6, 1, 0, -1, kIF64Bit);
					break;

				case 1014:
					if(in & kMaskD)
						WriteInvalidInstruction(dd, in);
					else
						WriteThreeRegisterOperation(dd, in, "dcbz", 3, 0, 0, 0, 0);
					break;

				default:
					WriteInvalidInstruction(dd, in);
					break;
			}
			break;

		case 32:
		case 33:
		case 34:
		case 35:
		case 36:
		case 37:
		case 38:
		case 39:
		case 40:
		case 41:
		case 42:
		case 43:
		case 44:
		case 45:
		case 46:
		case 47:
			WriteLdst(dd, in, kStrLoadStore[GetOpcode(in) - 32], 'r', 0);
			break;

		case 48:
		case 49:
		case 50:
		case 51:
		case 52:
		case 53:
		case 54:
		case 55:
			WriteLdst(dd, in, kStrLoadStore[GetOpcode(in) - 32], 'f', 0);
			break;

		case 58:
			switch (in & 3)
			{
				case 0:
					WriteLdst(dd, in & ~3, "ld", 'r', kIF64Bit);
					break;

				case 1:
					WriteLdst(dd, in & ~3, "ldu", 'r', kIF64Bit);
					break;

				case 2:
					WriteLdst(dd, in & ~3, "lwa", 'r', kIF64Bit);
					break;

				default:
					WriteInvalidInstruction(dd, in);
					break;
			}
			break;

		case 59:
			switch (in & 0x3e)
			{
				case 36:
					WriteFdacb(dd, in, "divs", 6, 0);
					break;

				case 40:
					WriteFdacb(dd, in, "subs", 6, 0);
					break;

				case 42:
					WriteFdacb(dd, in, "adds", 6, 0);
					break;

				case 44:
					WriteFdacb(dd, in, "sqrts", 2, 0);
					break;

				case 48:
					WriteFdacb(dd, in, "res", 2, 0);
					break;

				case 50:
					WriteFdacb(dd, in, "muls", 5, 0);
					break;

				case 56:
					WriteFdacb(dd, in, "msubs", 7, 0);
					break;

				case 58:
					WriteFdacb(dd, in, "madds", 7, 0);
					break;

				case 60:
					WriteFdacb(dd, in, "nmsubs", 7, 0);
					break;

				case 62:
					WriteFdacb(dd, in, "nmadds", 7, 0);
					break;

				default:
					WriteInvalidInstruction(dd, in);
					break;
			}
			break;

		case 62:
			switch (in & 3)
			{
				case 0:
					WriteLdst(dd, in & ~3, "std", 'r', kIF64Bit);
					break;

				case 1:
					WriteLdst(dd, in & ~3, "stdu", 'r', kIF64Bit);
					break;

				default:
					WriteInvalidInstruction(dd, in);
					break;
			}
			break;

		case 63:
			if(in & 32)
			{
				switch (in & 0x1e)
				{
					case 4:
						WriteFdacb(dd, in, "div", 6, 0);
						break;

					case 8:
						WriteFdacb(dd, in, "sub", 6, 0);
						break;

					case 10:
						WriteFdacb(dd, in, "add", 6, 0);
						break;

					case 12:
						WriteFdacb(dd, in, "sqrt", 2, 0);
						break;

					case 14:
						WriteFdacb(dd, in, "sel", 7, 0);
						break;

					case 18:
						WriteFdacb(dd, in, "mul", 5, 0);
						break;

					case 20:
						WriteFdacb(dd, in, "sqrte", 2, 0);
						break;

					case 24:
						WriteFdacb(dd, in, "msub", 7, 0);
						break;

					case 26:
						WriteFdacb(dd, in, "madd", 7, 0);
						break;

					case 28:
						WriteFdacb(dd, in, "nmsub", 7, 0);
						break;

					case 30:
						WriteFdacb(dd, in, "nmadd", 7, 0);
						break;

					default:
						WriteInvalidInstruction(dd, in);
						break;
				}
			}
			else
			{
				switch (GetExtendedOpcode(in))
				{
					case 0:
						WriteFcmp(dd, in, 'u');
						break;

					case 12:
						WriteFdacb(dd, in, "rsp", 10, 0);
						break;

					case 14:
						WriteFdacb(dd, in, "ctiw", 10, 0);
						break;

					case 15:
						WriteFdacb(dd, in, "ctiwz", 10, 0);
						break;

					case 32:
						WriteFcmp(dd, in,'o');
						break;

					case 38:
						WriteMtfsb(dd, in, 1);
						break;

					case 40:
						WriteFdacb(dd, in, "neg", 10, 0);
						break;

					case 64:
						WriteMcrf(dd, in, 's');  // mcrfs 
						break;

					case 70:
						WriteMtfsb(dd, in, 0);
						break;

					case 72:
						WriteFdacb(dd, in, "mr", 10, 0);
						break;

					case 134:
						if((in & 0x006f0800) == 0)
						{
							EA::StdC::Snprintf(dd.mOperation, sizeof(dd.mOperation),"mtfsfi%s", kStrConditionRegisterModified[in & 1]);
							EA::StdC::Snprintf(dd.mOperands, sizeof(dd.mOperands),"%u,%u", (unsigned)GetCRD(in), (unsigned)(in & 0x0000f000) >> 12);
						}
						else
							WriteInvalidInstruction(dd, in);
						break;

					case 136:
						WriteFdacb(dd, in, "nabs", 10, 0);
						break;

					case 264:
						WriteFdacb(dd, in, "abs", 10, 0);
						break;

					case 583:
						if((in & (kMaskA | kMaskB)) == 0)
						{
							EA::StdC::Snprintf(dd.mOperation, sizeof(dd.mOperation), "mffs%s", kStrConditionRegisterModified[in & 1]);
							EA::StdC::Snprintf(dd.mOperands, sizeof(dd.mOperands), "f%u", (unsigned)GetD(in));
						}
						else
							WriteInvalidInstruction(dd, in);
						break;

					case 711:
						if((in & 0x02010000) == 0)
						{
							EA::StdC::Snprintf(dd.mOperation, sizeof(dd.mOperation), "mtfsf%s", kStrConditionRegisterModified[in & 1]);
							EA::StdC::Snprintf(dd.mOperands, sizeof(dd.mOperands), "%u,f%u", (unsigned)(in & 0x01fe0000) >> 17, (unsigned)GetB(in));
						}
						else
							WriteInvalidInstruction(dd, in);
						break;

					case 814:
						WriteFdacb(dd, in, "fctid", 10, kIF64Bit);
						break;

					case 815:
						WriteFdacb(dd, in, "fctidz", 10, kIF64Bit);
						break;

					case 846:
						WriteFdacb(dd, in, "fcfid", 10, kIF64Bit);
						break;

					default:
						WriteInvalidInstruction(dd, in);
						break;
				}
			}
			break;

		default:
			WriteInvalidInstruction(dd, in);
			break;
	}
}

} // namespace PowerPC



///////////////////////////////////////////////////////////////////////////////
// DisassemblerPowerPC
///////////////////////////////////////////////////////////////////////////////

///////////////////////////////////////////////////////////////////////////////
// DisassemblerPowerPC
//
DisassemblerPowerPC::DisassemblerPowerPC()
{
	// Empty
}


///////////////////////////////////////////////////////////////////////////////
// ~DisassemblerPowerPC
//
DisassemblerPowerPC::~DisassemblerPowerPC()
{
	// Empty
}


///////////////////////////////////////////////////////////////////////////////
// Dasm
//
const void* DisassemblerPowerPC::Dasm(const void* pData, const void* pDataEnd, 
									   DasmData& dd, uint32_t optionFlags, uint64_t dataAddress)
{
	EA_ASSERT(pData != NULL);

	const uint32_t* pData32    = (const uint32_t*)pData;
	const uint32_t* pDataEnd32 = (const uint32_t*)pDataEnd;

	dd.ClearLastInstruction();

	if((pData32 + 1) > pDataEnd32) // If there isn't space for one instruction...
	{
		dd.mInstructionFlags = kIFInvalid;
		return pDataEnd;
	}

	dd.mOptionFlags   |= optionFlags;
	dd.mProcessorType  = kProcessorPowerPC;

	// If this is the first time in use, set the base address.
	if(dd.mBaseAddress == 0)
		dd.mBaseAddress = dataAddress;

	// Print mInstructionAddress
	// mInstructionAddress is the address for this particular instruction. mBaseAddress is
	// the address for the first instruction in the span of instructions we are using DasmData for.
	// So mInstructionAddress is always >= mBaseAddress. 
	//if(!dd.mInstructionAddress) This doesn't work right. // If the user didn't specify it already...
	{
		if(dataAddress)
			dd.mInstructionAddress = dataAddress;
		else
			dd.mInstructionAddress = (uint64_t)(uintptr_t)pData;
	}

	if(dd.mInstructionAddress > (uint64_t)UINT32_MAX)
		EA::StdC::Snprintf(dd.mAddress, sizeof(dd.mAddress), "%016" PRIx64, (uint64_t)dd.mInstructionAddress);
	else
		EA::StdC::Snprintf(dd.mAddress, sizeof(dd.mAddress), "%08x", (unsigned)dd.mInstructionAddress);


	// Read the instruction.
	// This is very simple on PowerC, as instructions are always 32 bits. 
	// We memcpy it instead of assign it because it may necessarily be 32 bit aligned.
	// We use the term 'ppc_word' to refer to an instruction, though it is a uint32_t.
	PowerPC::ppc_word in;
	memcpy(&in, pData32, sizeof(in));

	#ifdef EA_SYSTEM_LITTLE_ENDIAN
		// PowerPC instructions are natively big-endian. So if we are running on 
		// a little endian system then we need to swizzle it.
		in = (uint32_t)((in >> 24)               |
					   ((in << 24) & 0xff000000) |
					   ((in <<  8) & 0x00ff0000) |
					   ((in >>  8) & 0x0000ff00));
	#endif

	// Print the instruction (operation and operands)
	PowerPC::WriteInstruction(in, dd);

	EA::StdC::Snprintf(dd.mInstruction, sizeof(dd.mInstruction), "%08x", *pData32);

	// Regardless of the validity of he instruction, the next instruction is always
	// one ppc_word (uint32_t) later.
	return ++pData32;
}




} // namespace Dasm

} // namespace EA


#endif // EACALLSTACK_DASM_POWERPC_ENABLED




