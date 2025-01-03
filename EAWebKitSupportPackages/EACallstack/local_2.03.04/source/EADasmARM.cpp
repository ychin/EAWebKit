///////////////////////////////////////////////////////////////////////////////
// EADasmARM.cpp
//
// Copyright (c) 2011, Electronic Arts. All rights reserved.
// Maintained by Paul Pedriana.
///////////////////////////////////////////////////////////////////////////////

///////////////////////////////////////////////////////////////////////////////
// The easiest way to understand and maintain the code here is to have a recent
// copy of the ARM Technical Reference Manual. There are online ARM manuals
// in web pages at arm.com, but to get the reference manuals that have 
// instruction descriptions and opcodes you need to sign up as an ARM developer. 
// A key manual is the ARMv7-AR Architecture Reference Manual.
//     http://infocenter.arm.com/help/index.jsp?topic=/com.arm.doc.ddi0406b/index.html
// 
// While there are many variations of ARM CPUs, the core ARM register set and 
// instruction set is fairly stable. Auxiliary functionality such as floating
// point and vector math is a case where things diverge a bit. 
//
// The primary instruction set is the 32 bit ARM instruction set, which is a 
// RISC design with each instruction being 32 bits. ARM also has a reduced 
// instruction set called Thumb, in which each instruction is 16 bits. Each 
// 16 bit Thumb instruction matches a specific 32 bit ARM instruction, but 
// reduces the available functionality (which is why it can fit in 16 bits).
// Recent ARM chips support Thumb-2, which is a superset of Thumb which provides
// for supporting all 32 bit ARM instruction functionality via some double 
// instructions (two 16 bit instructions) and via taking more instructions to 
// do the same thing (e.g. conditional execution). Still any set of Thumb or
// Thumb-2 instructions can be directly converted to canonical 32 bit ARM 
// instructions. And in fact that's how ARM CPUs work at runtime.
//
///////////////////////////////////////////////////////////////////////////////


#include <EACallstack/EADasm.h>

#if EACALLSTACK_DASM_ARM_ENABLED

#include <EACallstack/internal/EADasmARM.h>
#include <EAAssert/eaassert.h>

namespace EA
{

namespace Dasm
{

namespace ARM
{


///////////////////////////////////////////////////////////////////////////////
// GetBitRangeSigned
//
// Example:
//     GetBitRangeSigned(0b0000111100001111, 2, 4)
//     returns 0b11 treated as signed two bits, which happens to be -1.
//
static int GetBitRangeSigned(uint32_t n32, int lowBit, int highBit)
{
	// This is a little tricky. We need to treat a sub-range of bits within n16 as signed. 
	// The C/C++ language doesn't directly support doing this, but we can accomplish it anyway.
	int32_t s32 = (int32_t)n32;
	s32 <<= (31 - highBit);
	s32 >>= (31 - highBit) + lowBit;    // This shift assumes that the compiler does an "arithmetic" (a.k.a. signed) right shift, which propogates a top-most 1 bit downward if present. All modern compiler/CPU combinations do so.

	return s32;
}


///////////////////////////////////////////////////////////////////////////////
// GetBitRangeUnsigned
//
// Converts a dynamically specified unsigned bitfield to an integer.
// It doesn't matter that the return value is signed int as opposed to 
// unsigned int, as the return value is always positive. It's convenient
// for us to use signed int in order to avoid a bunch of casts in the code.
//
// Example:
//     GetBitRangeUnsigned(0b0000111100001111, 2, 10)
//     returns 0b111000011
//
static inline int GetBitRangeUnsigned(uint32_t n32, int lowBit, int highBit)
{
	n32 <<= (31 - highBit);
	n32 >>= (31 - highBit) + lowBit;

	return (int)n32;
}

// Returns the bit as 0 or 1.
static inline int GetBit(uint32_t n32, int bit)
{
	const uint32_t value = (n32 & (1 << bit)) >> bit;

	return (int)value;
}

// "raw" because it isn't shifted down to make a numerical value. 
static inline int GetBitRaw(uint32_t n32, int bit)
{
	return (int)(n32 & (1 << bit));
}

// Returns true or false.
static inline bool GetBitBool(uint32_t n32, int bit)
{
	const uint32_t value = (n32 & (1 << bit));

	return (value != 0);
}


const char* kSignString[3] = 
{
	"", 
	"+",
	"-"
};


static inline Sign GetSign(uint32_t in, int signBit)
{
	// If the user passes in a negative sign bit value, we assume the user doesn't want sign.
	return (signBit >= 0) ? (GetBit(in, signBit) ? kSignPositive : kSignNegative) : kSignNone;
}


// Returns "-" or ""
static inline const char* GetSignStringNatural(uint32_t in, int signBit)
{
	return (GetSign(in, signBit) == kSignNegative) ? "-" : "";
}


// Returns "-", "+", or ""
static inline const char* GetSignString(uint32_t in, int signBit)
{
	return kSignString[GetSign(in, signBit)];
}


// Returns "-", or "+"
static inline const char* GetSignOperationString(uint32_t in, int signBit)
{
	Sign sign = GetSign(in, signBit);

	return (sign == kSignNegative) ? "-" : "+";
}


const char* kRegisterName[16] = 
{
	"r0",   // a.k.a. a1 (argument 1)
	"r1",   // a2
	"r2",   // a3
	"r3",   // a4
	"r4",   // a.k.a. v1  (variable 1)
	"r5",   // v2
	"r6",   // v3
	"r7",   // v4 frame pointer for Apple ARM, or Thumb 16 bit instructions for Linux ARM.
	"r8",   // v5
	"r9",   // or v6 or sb. static base in RWPI variants."
	"sl",   // or v7. stack limit in stack-checked variants" GCC disassemblers seem to use "sl" instead of "r10"
	"fp",   // or v8. frame pointer for Linux ARM 32 bit instructions.
	"r12",  // or ip (intra-procedure-call scratch register)
	"sp",   // stack pointer
	"lr",   // link register (holds the address that a subroutine returns to when done).
	"pc"    // program counter (holds the currently executing instruction address).
};


///////////////////////////////////////////////////////////////////////////////
// GetARMRegisterName
//
void GetARMRegisterName(char* sRegister, int regIndex)
{
	EA::StdC::Strcpy(sRegister, kRegisterName[regIndex]);
}


///////////////////////////////////////////////////////////////////////////////
// GetARMRegisterName
//
const char* GetARMRegisterName(int regIndex)
{
	return kRegisterName[regIndex];
}


///////////////////////////////////////////////////////////////////////////////
// GetARMRegisterName
//
const char* GetARMRegisterName(uint32_t in, int rStartBit, int rEndBit)
{
	int regIndex = GetBitRangeUnsigned(in, rStartBit, rEndBit);
	return kRegisterName[regIndex];
}


///////////////////////////////////////////////////////////////////////////////
// GetARMRegisterName
//
// regName must have a capacity of at least 4 bytes.
//
int GetARMRegisterName(uint32_t in, char* sRegister, int rStartBit, int rEndBit)
{
	int regIndex = GetBitRangeUnsigned(in, rStartBit, rEndBit);
	EA::StdC::Strcpy(sRegister, kRegisterName[regIndex]);
	return regIndex;
}


///////////////////////////////////////////////////////////////////////////////
// GetARMValue
//
// sValue must have a capacity of at least 16 bytes.
//
static int GetARMValue(uint32_t in, char* sOffset, int signBit, int startBit, int endBit)
{
	using namespace EA::StdC;

	int         offset      = GetBitRangeUnsigned(in, startBit, endBit);
	Sign        sign        = GetSign(in, signBit);
	const char* pSignString = kSignString[sign];

	Snprintf(sOffset, 16, "#%s%d", pSignString, offset);

	return (sign == kSignNegative) ? -offset : offset;
}


static void GetShiftStr(uint32_t in, 
					   int typeBitBegin, int typeBitEnd, 
					   ShiftType& shiftType, int& shiftValue, char* sShift, char* sShiftHelp)
{
	using namespace EA::StdC;

	shiftType  = (ShiftType)GetBitRangeUnsigned(in, typeBitBegin, typeBitEnd);
	shiftValue = GetBitRangeUnsigned(in, typeBitBegin, typeBitEnd);

	switch (shiftType)
	{
		default: // The default should be logically impossible, but we include it to avoid compiler warnings.
		case kSRType_LSL:
			Snprintf(sShift, 16, "lsl");
			Snprintf(sShiftHelp, 32, "shift left");
			break;

		case kSRType_LSR:
			Snprintf(sShift, 16, "lsr");
			Snprintf(sShiftHelp, 32, "logical (unsigned) right");
			break;

		case kSRType_ASR:
			Snprintf(sShift, 16, "asr");
			Snprintf(sShiftHelp, 32, "shift arithmetic (signed) right");
			break;

		case kSRType_ROR:
			Snprintf(sShift, 16, "ror");
			Snprintf(sShiftHelp, 32, "rotated right");
			break;
	}
}


// WriteARMImmediateShift
//
// Shift asm strings are written like this:
//      lsl #<n>        Logical shift left <n> bits. 1 <= <n> <= 31.
//      lsr #<n>        Logical shift right <n> bits. 1 <= <n> <= 32.
//      asr #<n>        Arithmetic shift right <n> bits. 1 <= <n> <= 32.
//      ror #<n>        Rotate right <n> bits. 1 <= <n> <= 31.
//      rrx             Rotate right one bit, with extend. Bit [0] is written to shifter_carry_out, bits [31:1] are shifted right one bit, and the Carry Flag is shifted into bit [31].
//
// Example sShiftHelp:
//   bSingleRegisterHelp = true:
//      [r4, -r2, lsl #3]          // Use the address formed by [r4 - (r2 shifted left by 3)]
//   bSingleRegisterHelp = false:
//      [r4, lsl #3]               // Use the address formed by [r2 shifted left by 3]
//
// sShift must have a capacity of at least 16 characters.
// sShiftHelp must have a capacity of at least 32 characters.
// The logic below is basically copied from the ARM "DecodeImmShift" documentation for.
//
static int WriteARMImmediateShift(uint32_t in, 
								   bool bSingleRegisterHelp,
								   int rnStartBit, int rnEndBit,
								   int rmStartBit, int rmEndBit,
								   int signBit,
								   int typeBitBegin, int typeBitEnd, 
								   ShiftType& shiftType, int& shiftValue, char* sShift, char* sShiftHelp, int immediateBitBegin, int immediateBitEnd)
{
	using namespace EA::StdC;

	char sRn[8]; GetARMRegisterName(in, sRn, rnStartBit, rnEndBit);
	char sRm[8]; GetARMRegisterName(in, sRm, rmStartBit, rmEndBit);
	int  imm  =  GetBitRangeUnsigned(in, immediateBitBegin, immediateBitEnd);

	shiftType =  (ShiftType)GetBitRangeUnsigned(in, typeBitBegin, typeBitEnd);

	switch(shiftType)
	{
		default: // The default should be logically impossible, but we include it to avoid compiler warnings.
		case kSRType_LSL:
			shiftValue = imm;
			Snprintf(sShift, 16, "lsl #%d", shiftValue);
			if(bSingleRegisterHelp)
				Snprintf(sShiftHelp, 32, "%s shifted left by %d]", sRm, shiftValue); // Example output: [sp shifted left by 2]
			else
				Snprintf(sShiftHelp, 32, "[%s %s (%s shifted left by %d)]", sRn, GetSignOperationString(in, signBit), sRm, shiftValue); // Example output: [r2 - (sp shifted left by 2)]
			break;

		case kSRType_LSR:
			if(imm == 0)
				shiftValue = 32;
			else
				shiftValue = imm;
			Snprintf(sShift, 16, "lsr #%d", shiftValue);
			if(bSingleRegisterHelp)
				Snprintf(sShiftHelp, 32, "%s shifted logical (unsigned) right by %d]", sRm, shiftValue);
			else
				Snprintf(sShiftHelp, 32, "[%s %s (%s shifted logical (unsigned) right by %d)]", sRn, GetSignOperationString(in, signBit), sRm, shiftValue);
			break;

		case kSRType_ASR:
			if(imm == 0)
				shiftValue = 32;
			else
				shiftValue = imm;
			Snprintf(sShift, 16, "asr #%d", shiftValue);
			if(bSingleRegisterHelp)
				Snprintf(sShiftHelp, 32, "%s shifted arithmetic (signed) right by %d]", sRm, shiftValue);
			else
				Snprintf(sShiftHelp, 32, "[%s %s (%s shifted arithmetic (signed) right by %d)]", sRn, GetSignOperationString(in, signBit), sRm, shiftValue);
			break;

		case kSRType_ROR:
			if(imm == 0)
			{
				shiftType  = kSRType_RRX;
				shiftValue = 1;
				Snprintf(sShift, 16, "rrx", shiftValue);
				if(bSingleRegisterHelp)
					Snprintf(sShiftHelp, 32, "%s rotated right by 1, with extend]", sRm);
				else
					Snprintf(sShiftHelp, 32, "[%s %s (%s rotated right by 1, with extend)]", sRn, GetSignOperationString(in, signBit), sRm);
			}
			else
			{
				shiftValue = imm;
				Snprintf(sShift, 16, "ror #%d", shiftValue);
				if(bSingleRegisterHelp)
					Snprintf(sShiftHelp, 32, "%s rotated right by %d]", sRm, shiftValue);
				else
					Snprintf(sShiftHelp, 32, "[%s %s (%s rotated right by %d)]", sRn, GetSignOperationString(in, signBit), sRm, shiftValue);
			}
			break;
	}

	return shiftValue;
}


const char* kAModeStr[4] = 
{
	"da",
	"ia",
	"db",
	"ib"
};

static const char* GetAModeStr(uint32_t in, int amodeBitBegin)
{
	AMode aMode = (AMode)GetBitRangeUnsigned(in, amodeBitBegin, amodeBitBegin + 1);
	return kAModeStr[aMode];
}




const char* kConditionStr[16] = 
{
	"eq", // 0000
	"ne", // 0001
	"cs", // 0010
	"cc", // 0011
	"mi", // 0100
	"pl", // 0101
	"vs", // 0110
	"vc", // 0111
	"hi", // 1000
	"ls", // 1001
	"ge", // 1010
	"lt", // 1011
	"gt", // 1100
	"le", // 1101
	"",   // 1110 Same as "al" for always, but don't show it on diassembly.
	"",   // 1111 invalid for use as a condition.
};

const char* GetConditionStr(uint32_t in)
{
	const int condition = GetBitRangeUnsigned(in, 28, 31);
	return kConditionStr[condition];
}


const char* kConditionIntegerHelpStr[16] = 
{
	"Condition: Equal; Executed if Z flag == 1.",
	"Condition: Not equal; Executed if Z flag == 0.",
	"Condition: Carry set (there was a bit carry); Executed if C flag == 1.",
	"Condition: Carry clear (no bit carry); Executed if C flag == 0.",
	"Condition: Minus, negative; Executed if N flag == 1.",
	"Condition: Plus, positive or zero; Executed if N flag == 0.",
	"Condition: Overflow; Executed if V flag == 1.",
	"Condition: No overflow; Executed if V flag == 0.",
	"Condition: Unsigned higher; Executed if flags: C == 1 and Z == 0.",
	"Condition: Unsigned lower or same; Executed if flags: C == 0 or Z == 1.",
	"Condition: Signed greater than or equal; flags: N == V.",
	"Condition: Signed less than; Executed if flags: N != V.",
	"Condition: Signed greater than; Executed if flags: Z == 0 and N == V.",
	"Condition: Signed less than or equal; Executed if flags: Z == 1 or N != V",
	"", // Unconditional
	""  // Invalid
};

const char* GetConditionHelpStrInt(uint32_t in)
{
	const int condition = GetBitRangeUnsigned(in, 28, 31);
	return kConditionIntegerHelpStr[condition];
}

/* Not yet in use.
static const char* kConditionFloatHelpStr[16] = 
{
	"Equal; Executed if Z flag == 1.",
	"Not equal, or unordered; Executed if Z flag == 0.",
	"Greater than, equal, or unordered; Executed if C flag == 1.",
	"Less than; Executed if C flag == 0.",
	"Less than; Executed if N flag == 1.",
	"Greater than, equal, or unordered; Executed if N flag == 0.",
	"Unordered; Executed if V flag == 1.",
	"Not unordered; Executed if V flag == 0.",
	"Greater than, or unordered; Executed if flags: C == 1 and Z == 0.",
	"Less than or equal; Executed if flags: C == 0 or Z == 1.",
	"Greater than or equal; flags: N == V.",
	"Less than, or unordered; Executed if flags: N != V.",
	"Greater than; Executed if flags: Z == 0 and N == V.",
	"Less than, equal, or unordered; Executed if flags: Z == 1 or N != V",
	"", // Unconditional
	"" // Invalid
};


static const char* GetConditionHelpStrFloat(uint32_t in, 28, 31)
{
	const int condition = GetBitRangeUnsigned();
	return kConditionFloatHelpStr[condition];
}
*/


///////////////////////////////////////////////////////////////////////////////
// WriteARMInstruction
//
// Writes a standalone instruction such as:
//    nop
//
void WriteARMInstruction(DasmData& dd, InstructionType it, const char* pInstructionName)
{
	using namespace EA::StdC;

	dd.mInstructionType = it;
	Snprintf(dd.mOperation, EAArrayCount(dd.mOperation), "%s", pInstructionName);
	dd.mOperands[0] = 0;
	dd.mHelp[0] = 0;
}


///////////////////////////////////////////////////////////////////////////////
// WriteARMInstructionRO (Register, offset)
//
// Writes an instruction such as:
//    pli [r4, #-12]
//
static void WriteARMInstructionRO(DasmData& dd, InstructionType it, uint32_t in, const char* pInstructionName, 
								  int& r, char* sR, int rStartBit, int rEndBit, 
								  int& offset, char* sOffset, int oSignBit, int oStartBit, int oEndBit)
{
	using namespace EA::StdC;

	dd.mInstructionType = it;
	Strcpy(dd.mOperation, pInstructionName);

	r = GetARMRegisterName(in, sR, rStartBit, rEndBit);
	offset = GetARMValue(in, sOffset, oSignBit, oStartBit, oEndBit);
	if(r == kRegPC) // If it's the PC (program counter)...
		offset += 8; // PC offsets are always two instructions more, given how the PC changes while executing instructions.

	Snprintf(dd.mOperands, EAArrayCount(dd.mOperands), "[%s, %s]", sR, sOffset);
}


///////////////////////////////////////////////////////////////////////////////
// WriteARMInstructionRRSo (Register, shifted offset)
//
// Writes an instruction such as:
//    pldw [r2, -r4, lsl #4]
//    pldw [r2, -r4]           (shift value of 0)
//
static void WriteARMInstructionRRSo(DasmData& dd, InstructionType it, uint32_t in, const char* pInstructionName, 
								   int rnStartBit, int rnEndBit,
								   int rmStartBit, int rmEndBit,
								   int signBit,
								   int typeBitBegin, int typeBitEnd, 
								   char* sShiftHelp, int immediateBitBegin, int immediateBitEnd)
{
	using namespace EA::StdC;

	dd.mInstructionType = it;
	Strcpy(dd.mOperation, pInstructionName);

	char      sRn[8];  GetARMRegisterName(in, sRn, rnStartBit, rnEndBit);
	char      sRm[8];  GetARMRegisterName(in, sRm, rmStartBit, rmEndBit);
	ShiftType shiftType;
	int       shiftValue;
	char      sShift[16];

	WriteARMImmediateShift(in, false, rnStartBit, rnEndBit, rmStartBit, rmEndBit, signBit, typeBitBegin, typeBitEnd, shiftType, shiftValue, sShift, sShiftHelp, immediateBitBegin, immediateBitEnd);

	if(shiftValue == 0)
		Snprintf(dd.mOperands, EAArrayCount(dd.mOperands), "[%s, %s%s]", sRn, GetSignString(in, signBit), sRm);
	else
		Snprintf(dd.mOperands, EAArrayCount(dd.mOperands), "[%s, %s%s, %s]", sRn, GetSignString(in, signBit), sRm, sShift);
}


///////////////////////////////////////////////////////////////////////////////
// WriteARMInstructionCRRS (Condition, Register, Register, Shift)
//
// Writes an instruction such as:
//    tst r2, r4, lsl #4
//    tst r2, r4               (shift value of 0)
//
static void WriteARMInstructionCRRS(DasmData& dd, InstructionType it, uint32_t in, const char* pInstructionName, 
								   char* sRn, int rnStartBit, int rnEndBit,
								   char* sRm, int rmStartBit, int rmEndBit,
								   int typeBitBegin, int typeBitEnd, 
								   char* sShiftHelp, int immediateBitBegin, int immediateBitEnd)
{
	using namespace EA::StdC;

	dd.mInstructionType = it;
	Strcpy(dd.mOperation, pInstructionName);
	Strcat(dd.mOperation, GetConditionStr(in));

	ShiftType shiftType;
	int       shiftValue;
	char      sShift[16];

	GetARMRegisterName(in, sRn, rnStartBit, rnEndBit);
	GetARMRegisterName(in, sRm, rmStartBit, rmEndBit);

	WriteARMImmediateShift(in, true, rnStartBit, rnEndBit, rmStartBit, rmEndBit, -1, typeBitBegin, typeBitEnd, shiftType, shiftValue, sShift, sShiftHelp, immediateBitBegin, immediateBitEnd);

	if(shiftValue == 0) // If no shift... then we just have a mov.
		Snprintf(dd.mOperands, EAArrayCount(dd.mOperands), "%s, %s", sRn, sRm);
	else
		Snprintf(dd.mOperands, EAArrayCount(dd.mOperands), "%s, %s, %s", sRn, sRm, sShift);
}



///////////////////////////////////////////////////////////////////////////////
// WriteARMInstructionCRRRS (Condition, Register, Register, Register, Shift)
//
// Writes an instruction such as:
//    ands r2, r4, r5, lsl #4
//    ands r2, r4, r5          (shift value of 0)
//
static void WriteARMInstructionCRRRS(DasmData& dd, InstructionType it, uint32_t in, const char* pInstructionName, 
								   char* sRd, int rdStartBit, int rdEndBit,
								   char* sRn, int rnStartBit, int rnEndBit,
								   char* sRm, int rmStartBit, int rmEndBit,
								   int typeBitBegin, int typeBitEnd, 
								   char* sShiftHelp, int immediateBitBegin, int immediateBitEnd)
{
	using namespace EA::StdC;

	dd.mInstructionType = it;
	Strcpy(dd.mOperation, pInstructionName);
	Strcat(dd.mOperation, GetConditionStr(in));

	ShiftType shiftType;
	int       shiftValue;
	char      sShift[16];

	GetARMRegisterName(in, sRd, rdStartBit, rdEndBit);
	GetARMRegisterName(in, sRn, rnStartBit, rnEndBit);
	GetARMRegisterName(in, sRm, rmStartBit, rmEndBit);

	WriteARMImmediateShift(in, true, rnStartBit, rnEndBit, rmStartBit, rmEndBit, -1, typeBitBegin, typeBitEnd, shiftType, shiftValue, sShift, sShiftHelp, immediateBitBegin, immediateBitEnd);

	if(shiftValue == 0) // If no shift... then we just have a mov.
		Snprintf(dd.mOperands, EAArrayCount(dd.mOperands), "%s, %s, %s", sRd, sRn, sRm);
	else
		Snprintf(dd.mOperands, EAArrayCount(dd.mOperands), "%s, %s, %s, %s", sRd, sRn, sRm, sShift);
}



///////////////////////////////////////////////////////////////////////////////
// WriteARMInstructionCRRSr (Condition, Register, Register, Shifted Register)
//
// Writes an instruction such as:
//    sub r2, r4, lsl r5
//
static void WriteARMInstructionCRRSr(DasmData& dd, InstructionType it, uint32_t in, const char* pInstructionName, 
								   char* sRn, int rnStartBit, int rnEndBit,
								   char* sRm, int rmStartBit, int rmEndBit,
								   int typeBitBegin, int typeBitEnd, 
								   char* sShiftHelp, char* sRs, int rsStartBit, int rsEndBit)
{
	using namespace EA::StdC;

	dd.mInstructionType = it;
	Strcpy(dd.mOperation, pInstructionName);
	Strcat(dd.mOperation, GetConditionStr(in));

	ShiftType shiftType;
	int       shiftValue;
	char      sShift[16];

	GetARMRegisterName(in, sRn, rnStartBit, rnEndBit);
	GetARMRegisterName(in, sRm, rmStartBit, rmEndBit);
	GetARMRegisterName(in, sRs, rsStartBit, rsEndBit);
	GetShiftStr(in, typeBitBegin, typeBitEnd, shiftType, shiftValue, sShift, sShiftHelp);

	Snprintf(dd.mOperands, EAArrayCount(dd.mOperands), "%s, %s, %s %s", sRn, sRm, sShift, sRs);
}


///////////////////////////////////////////////////////////////////////////////
// WriteARMInstructionCRRRSr (Condition, Register, Register, Register, Shifted Register)
//
// Writes an instruction such as:
//    tst r2, r4, r3, lsl r5
//
static void WriteARMInstructionCRRRSr(DasmData& dd, InstructionType it, uint32_t in, const char* pInstructionName, 
									  char* sRd, int rdStartBit, int rdEndBit,
									  char* sRn, int rnStartBit, int rnEndBit,
									  char* sRm, int rmStartBit, int rmEndBit,
									  int typeBitBegin, int typeBitEnd, 
									  char* sShiftHelp, char* sRs, int rsStartBit, int rsEndBit)
{
	using namespace EA::StdC;

	dd.mInstructionType = it;
	Strcpy(dd.mOperation, pInstructionName);
	Strcat(dd.mOperation, GetConditionStr(in));

	ShiftType shiftType;
	int       shiftValue;
	char      sShift[16];

	GetARMRegisterName(in, sRd, rdStartBit, rdEndBit);
	GetARMRegisterName(in, sRn, rnStartBit, rnEndBit);
	GetARMRegisterName(in, sRm, rmStartBit, rmEndBit);
	GetARMRegisterName(in, sRs, rsStartBit, rsEndBit);
	GetShiftStr(in, typeBitBegin, typeBitEnd, shiftType, shiftValue, sShift, sShiftHelp);

	Snprintf(dd.mOperands, EAArrayCount(dd.mOperands), "%s, %s, %s, %s %s", sRd, sRn, sRm, sShift, sRs);
}



///////////////////////////////////////////////////////////////////////////////
// WriteARMInstructionRRIo() (Register, Register, Immediate Offset)
//
// Writes an instruction such as:
//     str r4, [r6]             // Equivalent to: str r4, [r6, #0]
//     str r4, [r6, #-8]
//     str r4, [r6, #-8]!
//     str r4, [r6], #-8
// 
static void WriteARMInstructionRRIo(DasmData& dd, InstructionType it, uint32_t in, const char* pInstructionName, 
									  char* sRt, int rtStartBit, int rtEndBit,
									  char* sRn, int rnStartBit, int rnEndBit,
									  int& value, int signBit, int immediateStartBit, int immmediateEndBit,
									  int indexedBit, int writeBackBit,
									  char* sOffsetHelp)
{
	using namespace EA::StdC;

	dd.mInstructionType = it;
	Strcpy(dd.mOperation, pInstructionName);
	Strcat(dd.mOperation, GetConditionStr(in));

	// ARM has this concept of three types of register-immediate operations: "offset", "pre-indexed", and "post-indexed". The two bools here detect which of the modes is in effect.
	bool        bIndexed     = GetBitBool(in, indexedBit);
	bool        bWriteBack   = GetBitBool(in, writeBackBit);
	int         immediate    = GetBitRangeUnsigned(in, immediateStartBit, immmediateEndBit);
	const char* pSignNatural = GetSignStringNatural(in, signBit);

	GetARMRegisterName(in, sRt, rtStartBit, rtEndBit);
	GetARMRegisterName(in, sRn, rnStartBit, rnEndBit);

	value = immediate;
	if(*pSignNatural == '-')
		value = -value;

	if(bIndexed && !bWriteBack)
	{
		if(immediate)
			Snprintf(dd.mOperands, EAArrayCount(dd.mOperands), "str %s, [%s, #%s%d]", sRt, sRn, pSignNatural, immediate);
		else
			Snprintf(dd.mOperands, EAArrayCount(dd.mOperands), "str %s, [%s]", sRt, sRn);

		if(value >= 0)
			Snprintf(sOffsetHelp, 32, "[%s + %d]", sRn, value);
		else
			Snprintf(sOffsetHelp, 32, "[%s - %d]", sRn, -value);
	}
	else if(bIndexed && bWriteBack)
	{
		Snprintf(dd.mOperands, EAArrayCount(dd.mOperands), "str %s, [%s, #%s%d]!", sRt, sRn, pSignNatural, immediate);

		if(value >= 0)
			Snprintf(sOffsetHelp, 32, "[%s += %d]. Pre-increment.", sRn, value);
		else
			Snprintf(sOffsetHelp, 32, "[%s -= %d]. Pre-increment.", sRn, -value);
	}
	else if(!bIndexed && !bWriteBack)
	{
		Snprintf(dd.mOperands, EAArrayCount(dd.mOperands), "str %s, [%s], #%s%d", sRt, sRn, pSignNatural, immediate);

		if(value >= 0)
			Snprintf(sOffsetHelp, 32, "[%s]; %s += %d. Post-increment.", sRn, sRn, value);
		else
			Snprintf(sOffsetHelp, 32, "[%s]; %s -= %d. Post-increment.", sRn, sRn, -value);
	}
	else
	{
		// !bIndexed && bWriteBack
		// This is a fourth mode that is handled by alternative instructions.
	}
}


///////////////////////////////////////////////////////////////////////////////
// WriteARMHintInstruction
//
// Writes one of the hint instructions (nop, wfe, yield, etc.)
//
void WriteARMHintInstruction(DasmData& dd, int condition, int identifier)
{
	using namespace EA::StdC;

	dd.mInstructionType = kITInvalid;

	switch (identifier)
	{
		case 0: // NOP<c>
			WriteARMInstruction(dd, kITMisc, "nop");
			Snprintf(dd.mHelp, EAArrayCount(dd.mHelp), "No operation. Used for biding time or for aligning executed instructions. ");
			break;

		case 1: // YIELD<c>
			WriteARMInstruction(dd, kITMisc, "yield");
			Snprintf(dd.mHelp, EAArrayCount(dd.mHelp), "Hint instruction telling CPU that other threads should execute. ");
			break;

		case 2: // WFE<c>
			WriteARMInstruction(dd, kITMisc, "wfe");
			Snprintf(dd.mHelp, EAArrayCount(dd.mHelp), "Hint instruction telling CPU to go to low power until an event occurs. ");
			break;

		case 3: // WFI<c>
			WriteARMInstruction(dd, kITMisc, "wfi");
			Snprintf(dd.mHelp, EAArrayCount(dd.mHelp), "Hint instruction telling CPU to go to low power until an interrupt occurs. ");
			break;

		case 4: // SEV<c>
			WriteARMInstruction(dd, kITMisc, "sev");
			Snprintf(dd.mHelp, EAArrayCount(dd.mHelp), "Hint instruction telling CPU an event has occurred. ");
			break;

		default:
			if((identifier & 0xf0) == 0xf0)
			{
				// DBG<c> #<option>
				dd.mInstructionType = kITMisc;
				Strcpy(dd.mOperation, "dbg");
				Sprintf(dd.mOperands, "%x", identifier & 0x0f);
				Sprintf(dd.mHelp, "Generates debug hint %d (%x). ", identifier & 0x0f, identifier & 0x0f);
			} 
			// Else invalid
			break;
	}

	if(dd.mInstructionType != kITInvalid)
	{
		Strcat(dd.mOperation, kConditionStr[condition]);
		Strcat(dd.mHelp, kConditionIntegerHelpStr[condition]);
	}
}


///////////////////////////////////////////////////////////////////////////////
// WriteArmBarrierInstruction
//
// Writes one of the barrier instructions (dsb, dmb, isb)
//
void WriteArmBarrierInstruction(DasmData& dd, int instruction, int option)
{
	using namespace EA::StdC;

	const char* pName   = ((instruction == 4) ? "dsb" : ((instruction == 5) ? "dmb" : "isb"));
	const char* pOption = NULL;
	const char* pHelp   = NULL;

	switch (option)
	{
		// sy is the only valid option for isb.
		case 0x0f:              // SY (full barrier)=1111
			pOption = "sy";
			pHelp   = "full system, read and write access.";
			break;
		case 0x0e:              // ST=1110
			pOption = "st";
			pHelp   = "full system, write access.";
			break;
		case 0x0b:              // ISH=1011
			pOption = "ish";
			pHelp   = "inner Shareable, read and write access.";
			break;
		case 0x0a:              // ISHST=1010
			pOption = "ishst";
			pHelp   = "inner Shareable, write access.";
			break;
		case 0x07:              // NSH=0111
			pOption = "nsh";
			pHelp   = "non-shareable, read and write access.";
			break;
		case 0x06:              // NSHST=0110
			pOption = "nshst";
			pHelp   = "non-shareable, write access.";
			break;
		case 0x03:              // OSH=0011
			pOption = "osh";
			pHelp   = "outer Shareable, read and write access.";
			break;
		case 0x02:              // OSHST=0010
			pOption = "oshst";
			pHelp   = "outer Shareable, write access.";
			break;
		default:
			pOption = "<invalid>";
			pHelp   = "";
			break;
	}

	dd.mInstructionType = kITMisc;
	Strcpy(dd.mOperation, pName);
	Snprintf(dd.mOperands, EAArrayCount(dd.mOperands), "%s", pOption);
	Snprintf(dd.mHelp, EAArrayCount(dd.mHelp), "Sync barrier: %s", pHelp);
}


///////////////////////////////////////////////////////////////////////////////
// ThumbInstructionIs32Bit
//
// Returns true if the input instruction indicates that it's the first of a pair.
// Most Thumb instructions are 16 bit. But a few of them are double instructions
// where the first uint16_t has part of the instruction and the next uint16_t 
// has the rest of it. It's a bit like UTF8 string encoding where a few special
// uint8_t chars are designated to mean that it's a multi-byte sequence.
//
// Example:
//   4e2dc:	b510      	push	{r4, lr}
//   4e2de:	1c04      	adds	r4, r0, #0
//   4e2e0:	f7ba ef38 	blx	9154 <_Z18androidGetFileRootv-0xc4>
//   4e2e4:	2300      	movs	r3, #0
//   4e2e6:	6003      	str	r3, [r0, #0]
//
static inline bool ThumbInstructionIs32Bit(uint16_t in16)
{
	return ((in16 & 0xe000) == 0xe000) && ((in16 & 0x1800) != 0x0000);  // If the top three bits are set, and the next two are anything but zero... it's a 32 bit instruction.
}





///////////////////////////////////////////////////////////////////////////////
// DasmARMUnconditional
//
int DasmARMUnconditional(uint32_t in, DasmData& dd)
{
	using namespace EA::StdC;

	int  Rn;
	char sRn[8];
	int  offset;
	char sOffset[16];
	char sShiftHelp[32];
	int  bits20_27 = GetBitRangeUnsigned(in, 20, 27);
	int  bits16_19 = GetBitRangeUnsigned(in, 16, 19);

	// 0xxxxxxx - Miscellaneous instructions, memory hints, and Advanced SIMD instructions
	if(GetBit(in, 27) == 0)
	{
		int bits4_7 = GetBitRangeUnsigned(in, 4, 7);

		// 0010000 xx0x xxx0 Change Processor State
		if((bits20_27 == 0x10) && !(bits4_7 & 2) && !(bits16_19 & 1))
		{
			// CPS<effect> <iflags>{,#<mode>}
			// CPS #<mode>
			// See B1.3.1 ARM processor modes
			WriteARMInstruction(dd, kITMisc, "cps ...");
			Sprintf(dd.mHelp, "Change processor state (interrupts and cpsr mode bits). Privileged instruction");
		}

		// 0010000 0000 xxx1 Set Endianness
		else if((bits20_27 == 0x10) && (bits4_7 == 0) && (bits16_19 & 1))
		{
			// SETEND <endian_specifier>
			const bool  bBigEndian  = GetBitBool(in, 9);
			const char* pBigEndian1 = bBigEndian ? "be" : "le";
			const char* pBigEndian2 = bBigEndian ? "big" : "little";

			dd.mInstructionType = kITMisc;
			Strcpy(dd.mOperation, "setend");
			Snprintf(dd.mOperands, EAArrayCount(dd.mOperands), "%s", pBigEndian1);
			Snprintf(dd.mHelp, EAArrayCount(dd.mHelp), "Set endian-ness to %s.", pBigEndian2);
		}

		// 01xxxxx - - See Advanced SIMD data-processing instructions (section A7.4)
		else if((bits20_27 & 0x60) == 0x20)
		{
			// V____ instructions
			int bits4_7_19_23 = GetBitRangeUnsigned(in, 4, 7) | (GetBitRangeUnsigned(in, 19, 23) << 4);

			if(GetBitRaw(in, 23) == 0) // Three registers of the same length
			{
				int bits8_11  = GetBitRangeUnsigned(in, 8, 11);
				int bit4      = GetBit(in, 4);
				int bit24     = GetBit(in, 24);
				int bits20_21 = GetBitRangeUnsigned(in, 20, 21);

				switch (bits8_11)
				{
					case 0:
						if(bit4 == 0) // VHADD, VHSUB on page A8-600
						{
							
						}
						else // VQADD on page A8-700
						{
							
						}
						break;

					case 1:
						if(bit4 == 0) // VRHADD on page A8-734
						{
							
						}
						else
						{
							if(bit24 == 0)
							{
								switch (bits20_21)
								{
									case 0: // VAND (register) on page A8-544
										break;
									case 1: // VBIC (register) on page A8-548
										break;
									case 2: // VORR (register) on page A8-680  
											// VMOV (register) on page A8-642 
										break;
									case 3: // VORN (register) on page A8-676
										break;
								}
							}
							else
							{
								switch (bits20_21)
								{
									case 0: // VEOR on page A8-596
										break;
									case 1: // VBIF, VBIT, VBSL on page A8-550  
										break;
									case 2: // VBIF, VBIT, VBSL on page A8-550
										break;
									case 3: // VBIF, VBIT, VBSL on page A8-550
										break;
								}
							}
						}
						break;

					case 2:
						if(bit4 == 0) // VHADD, VHSUB on page A8-600
						{
						}
						else // VQSUB on page A8-724
						{
						}
						break;

					case 3:
						if(bit4 == 0) // VCGT (register) on page A8-560
						{
						}
						else // VCGE (register) on page A8-556
						{
						}
						break;

					case 4:
						if(bit4 == 0) // VSHL (register) on page A8-752
						{
						}
						else // VQSHL (register) on page A8-718
						{
						}
						break;

					case 5:
						if(bit4 == 0) // VRSHL on page A8-736
						{
						}
						else // VQRSHL on page A8-714
						{
						}
						break;

					case 6: // VMAX, VMIN (integer) on page A8-630
						break;

					case 7:
						if(bit4 == 0) // VABD, VABDL (integer) on page A8-528
						{
						}
						else // VABA, VABAL on page A8-526
						{
						}

						break;

					case 8:
						if((bit4 == 0) && (bit24 == 0)) // VADD (integer) on page A8-536
						{
						}
						else if((bit4 == 0) && (bit24 == 1)) // VSUB (integer) on page A8-788
						{
						}
						else if((bit4 == 1) && (bit24 == 0)) // VTST on page A8-802
						{
						}
						else if((bit4 == 1) && (bit24 == 1)) // VCEQ (register) on page A8-552
						{
						}

						break;

					case 9: 
						if(bit4 == 0) // VMLA, VMLAL, VMLS, VMLSL (integer) on page A8-634
						{
						}
						else // VMUL, VMULL (integer and polynomial) on page A8-662
						{
						}
						break;

					case 10: // VPMAX, VPMIN (integer) on page A8-690
						break;

					case 11:
						if((bit4 == 0) && (bit24 == 0)) // VQDMULH on page A8-704
						{
						}
						else if((bit4 == 0) && (bit24 == 1)) // VQRDMULH on page A8-712
						{
						}
						else if((bit4 == 1) && (bit24 == 0)) // VPADD (integer) on page A8-684
						{
						}
						else if((bit4 == 1) && (bit24 == 1))
						{
							// Not represented
						}
						break;

					case 12:            // (not represented)
						break;

					case 13:
						if((bit4 == 0) && (bit24 == 0)) 
						{
							// VADD (floating-point) on page A8-538
							// VSUB (floating-point) on page A8-790
						}
						else if((bit4 == 0) && (bit24 == 1)) 
						{
							// VPADD (floating-point) on page A8-686
							// VABD (floating-point) on page A8-530
						}
						else if((bit4 == 1) && (bit24 == 0)) 
						{
							// VMLA, VMLS (floating-point) on page A8-636
						}
						else if((bit4 == 1) && (bit24 == 1))
						{
							// VMUL (floating-point) on page A8-664
						}
						break;

					case 14:
						if((bit4 == 0) && (bit24 == 0)) 
						{
							// VCEQ (register) on page A8-552
							
						}
						else if((bit4 == 0) && (bit24 == 1)) 
						{
							// VCGE (register) on page A8-556
							// VCGT (register) on page A8-560
						}
						else if((bit4 == 1) && (bit24 == 0)) 
						{
							// Not represented.
						}
						else if((bit4 == 1) && (bit24 == 1))
						{
							// VACGE, VACGT, VACLE,VACLT on page A8-534
						}

						break;

					case 15:
						if((bit4 == 0) && (bit24 == 0)) 
						{
							// VMAX, VMIN (floating-point) on page A8-632
						}
						else if((bit4 == 0) && (bit24 == 1)) 
						{
							// VPMAX, VPMIN (floating-point) on page A8-692
						}
						else if((bit4 == 1) && (bit24 == 0)) 
						{
							// VRECPS on page A8-730
							// VRSQRTS on page A8-744
						}
						else if((bit4 == 1) && (bit24 == 1))
						{
							// Not represented.
						}
						break;
				}
			}

			else if((bits4_7_19_23 & 0x179) == 0x101) // One register and a modified immediate value
			{
				int bits5_5_8_11 = GetBitRangeUnsigned(in, 5, 5) | (GetBitRangeUnsigned(in, 8, 11) << 1);

				if((bits5_5_8_11 & 0x19) == 0x0)
				{
					// 00xx0 Vector Move VMOV (immediate) on page A8-640
				}
				else if((bits5_5_8_11 & 0x19) == 0x1)
				{
					// 00xx1 Vector Bitwise OR VORR (immediate) on page A8-678
				}
				else if((bits5_5_8_11 & 0x1d) == 0x8)
				{
					// 010x0 Vector Move VMOV (immediate) on page A8-640
				}
				else if((bits5_5_8_11 & 0x1d) == 0x9)
				{
					// 010x1 Vector Bitwise OR VORR (immediate) on page A8-678
				}
				else if((bits5_5_8_11 & 0x1c) == 0xc)
				{
					// 011xx Vector Move VMOV (immediate) on page A8-640
				}
				else if((bits5_5_8_11 & 0x19) == 0x10)
				{
					// 10xx0 Vector Bitwise NOT VMVN (immediate) on page A8-668
				}
				else if((bits5_5_8_11 & 0x19) == 0x11)
				{
					// 10xx1 Vector Bit Clear VBIC (immediate) on page A8-546
				}
				else if((bits5_5_8_11 & 0x1d) == 0x18)
				{
					// 110x0 Vector Bitwise NOT VMVN (immediate) on page A8-668
				}
				else if((bits5_5_8_11 & 0x1d) == 0x19)
				{
					// 110x1 Vector Bit Clear VBIC (immediate) on page A8-546
				}
				else if((bits5_5_8_11 & 0x1e) == 0x1c)
				{
					// 1110x Vector Bitwise NOT VMVN (immediate) on page A8-668
				}
				else if((bits5_5_8_11 & 0x1f) == 0x1e)
				{
					// 11110 Vector Move VMOV (immediate) on page A8-640
				}
				else
				{
					// 11111 UNDEFINED
				}
			}

			else if(((bits4_7_19_23 & 0x179) == 0x111) || // Two registers and a shift amount
					((bits4_7_19_23 & 0x169) == 0x121) || 
					((bits4_7_19_23 & 0x149) == 0x141) || 
					((bits4_7_19_23 & 0x109) == 0x109))
			{
			}

			else if(((bits4_7_19_23 & 0x145) == 0x100) || // Three registers of different lengths
					((bits4_7_19_23 & 0x165) == 0x140))      
			{
			}

			else if(((bits4_7_19_23 & 0x145) == 0x104) ||  // Two registers and a scalar
					((bits4_7_19_23 & 0x165) == 0x144)) 
			{
			}

			else if((GetBitRaw(in, 24) == 0) && ((bits4_7_19_23 & 0x161) == 0x160)) // Vector Extract, VEXT
			{ 
			}

			else if(GetBitRaw(in, 24) == 1)
			{
				int bits4_11_19_23 = GetBitRangeUnsigned(in, 4, 11) | (GetBitRangeUnsigned(in, 19, 23) << 8);

				if((bits4_11_19_23 & 0x1681) == 0x1600) // Two registers, miscellaneous
				{ 
				}

				else if((bits4_11_19_23 & 0x16c1) == 0x1680) // Vector Table Lookup, VTBL, VTBX 
				{    
				}

				else if((bits4_11_19_23 & 0x16f9) == 0x16c0) // Vector Duplicate, VDUP (scalar)
				{ 
				}
			}

			WriteARMInstruction(dd, kITMisc, "v___ ...");
			Snprintf(dd.mHelp, EAArrayCount(dd.mHelp), "%s", "Advanced SIMD data-processing instruction.");
		}

		// 100xxx0 - - See Advanced SIMD element or structure load/store instructions (section A7.7)
		else if((bits20_27 & 0x71) == 0x40)
		{
			// V____ instructions
			WriteARMInstruction(dd, kITMisc, "v___ ...");
			Snprintf(dd.mHelp, EAArrayCount(dd.mHelp), "%s", "Advanced SIMD element or structure load/store instruction.");

			//const bool bLoad = (GetBitRaw(in, 21) != 0); // (bit 21 is the "L" bit (load vs store))

		}

		// 100x001 - Unallocated memory hint (treat as NOP)
		else if((bits20_27 & 0x77) == 0x41)
		{
			// NOP
			WriteARMInstruction(dd, kITMisc, "(nop) ...");
			Strcpy(dd.mHelp, "Unallocated memory hint, treated as NOP (no operation).");
		}

		// 100x101 - Preload Instruction PLI (immediate, literal)
		else if((bits20_27 & 0x77) == 0x45)
		{
			// PLI [<Rn>,#+/-<imm12>]
			// PLI <label>
			// PLI [PC,#-0]
			WriteARMInstructionRO(dd, kITMisc, in, "pli", Rn, sRn, 16, 19, offset, sOffset, 23, 0, 11);
			Strcpy(dd.mHelp, "Signals the memory system that instruction memory accesses from a specified address are likely in the near future.");
		}

		// 101x001 - not 1111 Preload Data with intent to Write. 
		//               1111 UNPREDICTABLE
		else if((bits20_27 & 0x77) == 0x51)
		{
			// PLD{W} [<Rn>,#+/-<imm12>]
			// Unpredictable
			if(bits16_19 != 0x0f)
			{
				const char* pName = (GetBit(in, 22) ? "pld" : "pldw");
				WriteARMInstructionRO(dd, kITMisc, in, pName, Rn, sRn, 16, 19, offset, sOffset, 23, 0, 11);
				Strcpy(dd.mHelp, "Preload data at the specified address with intent to write.");
			}
			else
			{
				const char* pName = (GetBit(in, 22) ? "pld (unpredictable)" : "pldw (unpredictable)");
				WriteARMInstructionRO(dd, kITMisc, in, pName, Rn, sRn, 16, 19, offset, sOffset, 23, 0, 11);
				Strcpy(dd.mHelp, "Preload data at the specified address with intent to write.");
			}
		}

		// 101x101 - not 1111 Preload Data - PLD, PLDW (immediate).
		//               1111 Preload Data - PLD (literal)
		else if((bits20_27 & 0x77) == 0x55)
		{
			// PLD{W} [<Rn>,#+/-<imm12>]
			// PLD <label>
			// PLD [PC,#-0]
			const bool bBit22 = GetBitBool(in, 22);
			const char* pName = (bBit22 ? "pld" : "pldw");
			WriteARMInstructionRO(dd, kITMisc, in, pName, Rn, sRn, 16, 19, offset, sOffset, 23, 0, 11);
			Strcpy(dd.mHelp, "Preload data at the specified address with intent to write.");
		}

		// 1010111 0001 - Clear-Exclusive CLREX, 0100 - Data Synchronization Barrier, 0101 - Data Memory Barrier, 0110 - Instruction Synchronization Barrier
		else if(bits20_27 == 0x57)
		{
			switch(bits4_7)
			{
				case 1: // CLREX
					WriteARMInstruction(dd, kITMisc, "clrex");
					Strcpy(dd.mHelp, "Clears the local record of the executing processor that an address has had a request for an exclusive access.");
					break;

				case 4: // DSB <option>, SY (full barrier)=1111, ST=1110, ISH=1011, ISHST=1010, NSH=0111, NSHST=0110, OSH=0011, OSHST=0110, invalid
				case 5: // DMB #<option>
				case 6: // ISB #<option>
				{
					int option = GetBitRangeUnsigned(in, 0, 3);
					WriteArmBarrierInstruction(dd, bits4_7, option);
					break;
				}

				default:
					//invalid
					break;
			}
		}

		// UNPREDICTABLE except as shown above
		else if((bits20_27 & 0x63) == 0x43)
		{
			// Invalid / unpredictable
		}

		// 110x001 xxx0 - Unallocated memory hint (treat as NOP)
		else if(((bits20_27 & 0x77) == 0x61) && ((bits4_7 & 1) == 0))
		{
			// NOP
			WriteARMInstruction(dd, kITMisc, "(nop)");
			Strcpy(dd.mHelp, "Unallocated memory hint, treated as NOP (no operation).");
		}

		// 110x101 xxx0 - Preload Instruction
		else if(((bits20_27 & 0x77) == 0x65) && ((bits4_7 & 1) == 0))
		{
			// PLI [<Rn>,+/-<Rm>{, <shift>}]
			WriteARMInstructionRRSo(dd, kITMisc, in, "pli", 16, 19, 0, 3, 23, 5, 6, sShiftHelp, 7, 11);
			Snprintf(dd.mHelp, EAArrayCount(dd.mHelp), "Hints that instructions may be soon read at %s", sShiftHelp);
		}

		// 111x001 xxx0 - Preload Data with intent to Write
		// 111x101 xxx0 - Preload Data
		else if((((bits20_27 & 0x77) == 0x71) && ((bits4_7 & 1) == 0)) ||
				(((bits20_27 & 0x77) == 0x75) && ((bits4_7 & 1) == 0)))
		{
			// PLD{W}<c> [<Rn>,+/-<Rm>{, <shift>}]
			const bool  bRead = GetBitBool(in, 22);
			const char* pName = bRead ? "pld" : "pldw";

			WriteARMInstructionRRSo(dd, kITMisc, in, pName, 16, 19, 0, 3, 23, 5, 6, sShiftHelp, 7, 11);
			Snprintf(dd.mHelp, EAArrayCount(dd.mHelp), "Hints that data may be soon %s %s", sShiftHelp, bRead ? "read from" : "written to");
		}

		// 11xxx11 xxx0 - UNPREDICTABLE
		else if(((bits20_27 & 0x63) == 0x63) && ((bits4_7 & 1) == 0))
		{
			// (invalid)
		}
	}

	// 100xx1x0 - Store Return State
	else if((bits20_27 & 0xe5) == 0x84)
	{
		// SRS{<amode>} SP{!},#<mode>
		WriteARMInstruction(dd, kITMisc, "srs ...");
		Snprintf(dd.mHelp, EAArrayCount(dd.mHelp), "Store the LR and SPSR of the current mode to the stack of a specified mode (%s).", GetAModeStr(in, 23));
	}

	// 100xx0x1 - Return From Exception
	else if((bits20_27 & 0xe5) == 0x81)
	{
		// RFE{<amode>} <Rn>{!}
		WriteARMInstruction(dd, kITMisc, "rfe ...");
		Strcpy(dd.mHelp, "Loads the PC and the CPSR from the word at the specified address and the following word respectively.");
	}

	// 101xxxxx - Branch with Link and Exchange - BL, BLX (immediate)
	else if((bits20_27 & 0xe0) == 0xa0)
	{
		// BLX <label>
		dd.mInstructionType = kITBranch;
		Strcpy(dd.mOperation, "blx");

		offset = (GetBitRangeUnsigned(in, 0, 23) << 1) | GetBit(in, 24); // It seems that the bits are used in an ugly way by ARM here.
		offset = GetBitRangeSigned((uint32_t)offset, 0, 24) * 2;         // *2 because thumb addresses are 16 bit aligned, and so the offset is specified in uint16's.
		Snprintf(dd.mOperands, EAArrayCount(dd.mOperands), "#%d", offset);

		Snprintf(dd.mHelp, EAArrayCount(dd.mHelp), "Branch from ARM code to Thumb code at the address [pc + 8 + %d], usually as a function call.", offset);
	}

	// 11000x11 / not 1111 Load Coprocessor (immediate)
	else if(((bits20_27 & 0xfb) == 0xc3) && (bits16_19 != 0x0f))
	{
		// Unconditional:
		// LDC2{L}<c> <coproc>,<CRd>,[<Rn>,#+/-<imm>]{!}
		// LDC2{L}<c> <coproc>,<CRd>,[<Rn>],#+/-<imm>
		// LDC2{L}<c> <coproc>,<CRd>,[<Rn>],<option>

		WriteARMInstruction(dd, kITMisc, "ldc2 ...");
		Snprintf(dd.mHelp, EAArrayCount(dd.mHelp), "Load memory data from consecutive memory addresses to a coprocessor. Immediate version.");
	}

	// 11001xx1 / 1111 , 1101xxx1 / 1111 - Load Coprocessor (literal)
	else if((((bits20_27 & 0xf9) == 0xc9) && (bits16_19 == 0x0f)) ||
			(((bits20_27 & 0xf1) == 0xd1) && (bits16_19 == 0x0f)))
	{
		// LDC2{L}<c> <coproc>,<CRd>,<label>
		// LDC2{L}<c> <coproc>,<CRd>,[PC,#-0]
		// LDC2{L}<c> <coproc>,<CRd>,[PC],<option>
		WriteARMInstruction(dd, kITMisc, "ldc2 ..."); // To do: complete the decoding of this and other "..." instructions.
		Snprintf(dd.mHelp, EAArrayCount(dd.mHelp), "Load memory data from consecutive memory addresses to a coprocessor. Literal version.");
	}

	// 11000x10, 11001xx0, 1101xxx0 - Store Coprocessor
	else if(((bits20_27 & 0xfb) == 0xc2) ||
			((bits20_27 & 0xf9) == 0xc8) ||
			((bits20_27 & 0xf1) == 0xd0))
	{
		// STC2{L}<c> <coproc>,<CRd>,[<Rn>,#+/-<imm>]{!}
		// STC2{L}<c> <coproc>,<CRd>,[<Rn>],#+/-<imm>
		// STC2{L}<c> <coproc>,<CRd>,[<Rn>],<option>
		WriteARMInstruction(dd, kITMisc, "stc2 ...");
		Snprintf(dd.mHelp, EAArrayCount(dd.mHelp), "Store data from a coprocessor to a sequence of consecutive memory addresses.");
	}

	// 11000100 - Move to Coprocessor from two ARM core registers
	else if(bits20_27 == 0xc4)
	{
		// MCRR2<c> <coproc>,<opc1>,<Rt>,<Rt2>,<CRm>
		WriteARMInstruction(dd, kITMisc, "mcrr2 ...");
		Snprintf(dd.mHelp, EAArrayCount(dd.mHelp), "(Move to Coprocessor from 2 ARM registers). Pass the values of two ARM core registers to a coprocessor.");
	}

	// 11000101 - Move to two ARM core registers from Coprocessor
	else if(bits20_27 == 0xc5)
	{
		// MRRC2<c> <coproc>,<opc>,<Rt>,<Rt2>,<CRm>
		WriteARMInstruction(dd, kITMisc, "mcrr2 ...");
		Snprintf(dd.mHelp, EAArrayCount(dd.mHelp), "(Move to 2 ARM registers from Coprocessor). Causes a coprocessor to transfer values to two ARM registers.");
	}

	// 1110xxxx 0 - Coprocessor data operations
	// 1110xxx0 1 - Move to Coprocessor from ARM core register
	// 1110xxx1 1 - Move to ARM core register from Coprocessor
	else if((bits20_27 & 0xf0) == 0xe0)
	{
		int bit4 = GetBit(in, 4);

		// 1110xxxx 0 - Coprocessor data operations
		if(bit4 == 0)
		{
			// CDP2<c> <coproc>,<opc1>,<CRd>,<CRn>,<CRm>,<opc2>
			WriteARMInstruction(dd, kITMisc, "cdp2 ...");
			Snprintf(dd.mHelp, EAArrayCount(dd.mHelp), "Tells a coprocessor to perform an operation that is independent of ARM core registers and memory.");
		}

		// 1110xxx0 1 - Move to Coprocessor from ARM core register
		else if((bits20_27 & 0x1) == 0x01)
		{
			// MCR2<c> <coproc>,<opc1>,<Rt>,<CRn>,<CRm>{,<opc2>}
			WriteARMInstruction(dd, kITMisc, "mcr2 ...");
			Snprintf(dd.mHelp, EAArrayCount(dd.mHelp), "Pass the value of an ARM register to a coprocessor.");
		}

		// 1110xxx1 1 - Move to ARM core register from Coprocessor
		else
		{
			// MRC2<c> <coproc>,<opc1>,<Rt>,<CRn>,<CRm>{,<opc2>}
			WriteARMInstruction(dd, kITMisc, "mrc2 ...");
			Snprintf(dd.mHelp, EAArrayCount(dd.mHelp), "Pass the value of a coprocessor register to an ARM register.");
		}
	}

	return 1;
}


///////////////////////////////////////////////////////////////////////////////
// DasmARMConditional
//
int DasmARMConditional(uint32_t in, DasmData& dd)
{
	using namespace EA::StdC;

	int  Rd;
	int  Rn;
	int  Rm;
	char sRd[8];
	char sRn[8];
	char sRm[8];
	char sRs[8];
	char sRt[8];
	char sShiftHelp[32];
	char sOffsetHelp[32];
	int  value;
	int  condition = GetBitRangeUnsigned(in, 28, 31);
	int  bits25_27 = GetBitRangeUnsigned(in, 25, 27);
	int  bits20_24 = GetBitRangeUnsigned(in, 20, 24);

	// 00x - Data-processing and miscellaneous instructions
	if((bits25_27 & 6) == 0)
	{
		if((bits25_27 & 0x01) == 0)
		{
			int bits4_7 = GetBitRangeUnsigned(in, 4, 7);    // The docs call bits 4-7 "op2" but aren't used in the actual instruction opcode with that name.

			// Unless we were going to take a different approach to disassembling (e.g. bit tables), the 
			// complexity and seemingly arbitrary complications here make us write some ugly bit picking code.

			// 0, not 10xx0 xxx0 Data-processing (register), 0xx1 Data-processing (register-shifted register)
			if(((bits20_24 & 0x19) != 0x10) && (((bits4_7 & 0x01) == 0x00) || ((bits4_7 & 0x09) == 0x01)))
			{
				const bool bReg     = ((bits4_7 & 0x01) == 0x00);
				const bool bRegSReg = ((bits4_7 & 0x09) == 0x01);

				if(bReg || bRegSReg)
				{
					// Data-processing (register)
					if(bits20_24 == 0x11)
					{
						// bReg:     TST<c> <Rn>,<Rm>{,<shift>}
						// bRegSReg: TST<c> <Rn>,<Rm>,<type> <Rs>  // Shift Rm by the amount specified by type/Rs
						if(bReg)
						{
							WriteARMInstructionCRRS(dd, kITMisc, in, "tst", sRn, 16, 19, sRm, 0, 3, 5, 6, sShiftHelp, 7, 11);
							Snprintf(dd.mHelp, EAArrayCount(dd.mHelp), "Generic test. Does a AND on %s and %s and updates flags. %s", sRn, sShiftHelp, GetConditionHelpStrInt(in));
						}
						else
						{
							WriteARMInstructionCRRSr(dd, kITMisc, in, "tst", sRn, 16, 19, sRm, 0, 3, 5, 6, sShiftHelp, sRs, 8, 11);
							Snprintf(dd.mHelp, EAArrayCount(dd.mHelp), "Generic test. Does a AND on %s and %s shifted with (%s %s) and updates flags. %s", sRn, sRm, sShiftHelp, sRs, GetConditionHelpStrInt(in));
						}
					}

					else if(bits20_24 == 0x3)
					{
						// TEQ<c> <Rn>,<Rm>{,<shift>}
						// TEQ<c> <Rn>,<Rm>,<type> <Rs>
						if(bReg)
						{
							WriteARMInstructionCRRS(dd, kITMisc, in, "teq", sRn, 16, 19, sRm, 0, 3, 5, 6, sShiftHelp, 7, 11);
							Snprintf(dd.mHelp, EAArrayCount(dd.mHelp), "Test equivalence. Does an XOR on %s and %s and updates flags. %s", sRn, sShiftHelp, GetConditionHelpStrInt(in));
						}
						else
						{
							WriteARMInstructionCRRSr(dd, kITMisc, in, "teq", sRn, 16, 19, sRm, 0, 3, 5, 6, sShiftHelp, sRs, 8, 11);
							Snprintf(dd.mHelp, EAArrayCount(dd.mHelp), "Test equivalence. Does an XOR on %s and %s shifted with (%s %s) and updates flags. %s", sRn, sRm, sShiftHelp, sRs, GetConditionHelpStrInt(in));
						}
					}

					else if(bits20_24 == 0x15)
					{
						// CMP<c> <Rn>,<Rm>{,<shift>}
						// CMP<c> <Rn>,<Rm>,<type> <Rs>
						if(bReg)
						{
							WriteARMInstructionCRRS(dd, kITMisc, in, "cmp", sRn, 16, 19, sRm, 0, 3, 5, 6, sShiftHelp, 7, 11);
							Snprintf(dd.mHelp, EAArrayCount(dd.mHelp), "Compare. Does %s minus %s and updates flags. %s", sRn, sShiftHelp, GetConditionHelpStrInt(in));
						}
						else
						{
							WriteARMInstructionCRRSr(dd, kITMisc, in, "cmp", sRn, 16, 19, sRm, 0, 3, 5, 6, sShiftHelp, sRs, 8, 11);
							Snprintf(dd.mHelp, EAArrayCount(dd.mHelp), "Compare. Does %s minus %s shifted with (%s %s) and updates flags. %s", sRn, sRm, sShiftHelp, sRs, GetConditionHelpStrInt(in));
						}
					}

					else if(bits20_24 == 0x17)
					{
						// CMN<c> <Rn>,<Rm>{,<shift>}
						// CMN<c> <Rn>,<Rm>,<type> <Rs>
						if(bReg)
						{
							WriteARMInstructionCRRS(dd, kITMisc, in, "cmn", sRn, 16, 19, sRm, 0, 3, 5, 6, sShiftHelp, 7, 11);
							Snprintf(dd.mHelp, EAArrayCount(dd.mHelp), "Compare negative. Does %s plus %s and updates flags. %s", sRn, sShiftHelp, GetConditionHelpStrInt(in));
						}
						else
						{
							WriteARMInstructionCRRSr(dd, kITMisc, in, "cmn", sRn, 16, 19, sRm, 0, 3, 5, 6, sShiftHelp, sRs, 8, 11);
							Snprintf(dd.mHelp, EAArrayCount(dd.mHelp), "Compare negative. Does %s plus %s shifted with (%s %s) and updates flags. %s", sRn, sRm, sShiftHelp, sRs, GetConditionHelpStrInt(in));
						}
					}

					const bool bS = GetBitBool(in, 20);

					switch (bits20_24 >> 1) // Shift away the S bit.
					{
						// Note: This could be table-driven, since the values are incremental.
						case  0:
						{
							// AND{S}<c> <Rd>,<Rn>,<Rm>{,<shift>}
							// AND{S}<c> <Rd>,<Rn>,<Rm>,<type> <Rs>
							const char* pName = bS ? "ands" : "and";

							if(bReg)
							{
								WriteARMInstructionCRRRS(dd, kITMisc, in, pName, sRd, 12, 15, sRn, 16, 19, sRm, 0, 3, 5, 6, sShiftHelp, 7, 11);
								Snprintf(dd.mHelp, EAArrayCount(dd.mHelp), "%s = %s AND %s (shifted with %s). %s", sRd, sRn, sRm, sShiftHelp, GetConditionHelpStrInt(in));
							}
							else
							{
								WriteARMInstructionCRRRSr(dd, kITMisc, in, pName, sRd, 12, 15, sRn, 16, 19, sRm, 0, 3, 5, 6, sShiftHelp, sRs, 8, 11);
								Snprintf(dd.mHelp, EAArrayCount(dd.mHelp), "%s = %s AND %s (shifted with (%s %s)). %s", sRd, sRn, sRm, sShiftHelp, sRs, GetConditionHelpStrInt(in));
							}

							break;
						}

						case  1:
						{
							// EOR{S}<c> <Rd>,<Rn>,<Rm>{,<shift>}
							// EOR{S}<c> <Rd>,<Rn>,<Rm>,<type> <Rs>

							const char* pName = bS ? "eors" : "eor";

							if(bReg)
							{
								WriteARMInstructionCRRRS(dd, kITMisc, in, pName, sRd, 12, 15, sRn, 16, 19, sRm, 0, 3, 5, 6, sShiftHelp, 7, 11);
								Snprintf(dd.mHelp, EAArrayCount(dd.mHelp), "%s = %s XOR %s (shifted with %s). %s", sRd, sRn, sRm, sShiftHelp, GetConditionHelpStrInt(in));
							}
							else
							{
								WriteARMInstructionCRRRSr(dd, kITMisc, in, pName, sRd, 12, 15, sRn, 16, 19, sRm, 0, 3, 5, 6, sShiftHelp, sRs, 8, 11);
								Snprintf(dd.mHelp, EAArrayCount(dd.mHelp), "%s = %s XOR %s (shifted with (%s %s)). %s", sRd, sRn, sRm, sShiftHelp, sRs, GetConditionHelpStrInt(in));
							}

							break;
						}

						case  2:
						{
							// SUB{S}<c> <Rd>,<Rn>,<Rm>{,<shift>}
							// SUB{S}<c> <Rd>,<Rn>,<Rm>,<type> <Rs>
							const char* pName = bS ? "subs" : "sub";

							if(bReg)
							{
								WriteARMInstructionCRRRS(dd, kITMisc, in, pName, sRd, 12, 15, sRn, 16, 19, sRm, 0, 3, 5, 6, sShiftHelp, 7, 11);
								Snprintf(dd.mHelp, EAArrayCount(dd.mHelp), "%s = %s - %s (shifted with %s). %s", sRd, sRn, sRm, sShiftHelp, GetConditionHelpStrInt(in));
							}
							else
							{
								WriteARMInstructionCRRRSr(dd, kITMisc, in, pName, sRd, 12, 15, sRn, 16, 19, sRm, 0, 3, 5, 6, sShiftHelp, sRs, 8, 11);
								Snprintf(dd.mHelp, EAArrayCount(dd.mHelp), "%s = %s - %s (shifted with (%s %s)). %s", sRd, sRn, sRm, sShiftHelp, sRs, GetConditionHelpStrInt(in));
							}

							break;
						}

						case  3:
						{
							// RSB{S}<c> <Rd>,<Rn>,<Rm>{,<shift>}
							// RSB{S}<c> <Rd>,<Rn>,<Rm>,<type> <Rs>
							const char* pName = bS ? "rsbs" : "rsb";

							if(bReg)
							{
								WriteARMInstructionCRRRS(dd, kITMisc, in, pName, sRd, 12, 15, sRn, 16, 19, sRm, 0, 3, 5, 6, sShiftHelp, 7, 11);
								Snprintf(dd.mHelp, EAArrayCount(dd.mHelp), "%s = %s (shifted with %s) - %s. %s", sRd, sRn, sRm, sShiftHelp, GetConditionHelpStrInt(in));
							}
							else
							{
								WriteARMInstructionCRRRSr(dd, kITMisc, in, pName, sRd, 12, 15, sRn, 16, 19, sRm, 0, 3, 5, 6, sShiftHelp, sRs, 8, 11);
								Snprintf(dd.mHelp, EAArrayCount(dd.mHelp), "%s = %s (shifted with (%s %s)) - %s. %s", sRd, sRn, sRm, sShiftHelp, sRs, GetConditionHelpStrInt(in));
							}

							break;
						}

						case  4:
						{
							// ADD{S}<c> <Rd>,<Rn>,<Rm>{,<shift>}
							// ADD{S}<c> <Rd>,<Rn>,<Rm>,<type> <Rs>
							const char* pName = bS ? "adds" : "add";

							if(bReg)
							{
								WriteARMInstructionCRRRS(dd, kITMisc, in, pName, sRd, 12, 15, sRn, 16, 19, sRm, 0, 3, 5, 6, sShiftHelp, 7, 11);
								Snprintf(dd.mHelp, EAArrayCount(dd.mHelp), "%s = %s + %s (shifted with %s). %s", sRd, sRn, sRm, sShiftHelp, GetConditionHelpStrInt(in));
							}
							else
							{
								WriteARMInstructionCRRRSr(dd, kITMisc, in, pName, sRd, 12, 15, sRn, 16, 19, sRm, 0, 3, 5, 6, sShiftHelp, sRs, 8, 11);
								Snprintf(dd.mHelp, EAArrayCount(dd.mHelp), "%s = %s + %s (shifted with (%s %s)). %s", sRd, sRn, sRm, sShiftHelp, sRs, GetConditionHelpStrInt(in));
							}

							break;
						}

						case  5:
						{
							// ADC{S}<c> <Rd>,<Rn>,<Rm>{,<shift>}
							// ADC{S}<c> <Rd>,<Rn>,<Rm>,<type> <Rs>
							const char* pName = bS ? "adcs" : "adc";

							if(bReg)
							{
								WriteARMInstructionCRRRS(dd, kITMisc, in, pName, sRd, 12, 15, sRn, 16, 19, sRm, 0, 3, 5, 6, sShiftHelp, 7, 11);
								Snprintf(dd.mHelp, EAArrayCount(dd.mHelp), "%s = %s + %s (shifted with %s), with carry. %s", sRd, sRn, sRm, sShiftHelp, GetConditionHelpStrInt(in));
							}
							else
							{
								WriteARMInstructionCRRRSr(dd, kITMisc, in, pName, sRd, 12, 15, sRn, 16, 19, sRm, 0, 3, 5, 6, sShiftHelp, sRs, 8, 11);
								Snprintf(dd.mHelp, EAArrayCount(dd.mHelp), "%s = %s + %s (shifted with (%s %s)), with carry. %s", sRd, sRn, sRm, sShiftHelp, sRs, GetConditionHelpStrInt(in));
							}

							break;
						}

						case  6:
						{
							// SBC{S}<c> <Rd>,<Rn>,<Rm>{,<shift>}
							// SBC{S}<c> <Rd>,<Rn>,<Rm>,<type> <Rs>
							const char* pName = bS ? "sbcs" : "sbc";

							if(bReg)
							{
								WriteARMInstructionCRRRS(dd, kITMisc, in, pName, sRd, 12, 15, sRn, 16, 19, sRm, 0, 3, 5, 6, sShiftHelp, 7, 11);
								Snprintf(dd.mHelp, EAArrayCount(dd.mHelp), "%s = %s - %s (shifted with %s), with carry. %s", sRd, sRn, sRm, sShiftHelp, GetConditionHelpStrInt(in));
							}
							else
							{
								WriteARMInstructionCRRRSr(dd, kITMisc, in, pName, sRd, 12, 15, sRn, 16, 19, sRm, 0, 3, 5, 6, sShiftHelp, sRs, 8, 11);
								Snprintf(dd.mHelp, EAArrayCount(dd.mHelp), "%s = %s - %s (shifted with (%s %s)), with carry. %s", sRd, sRn, sRm, sShiftHelp, sRs, GetConditionHelpStrInt(in));
							}

							break;
						}

						case  7:
						{
							// RSC{S}<c> <Rd>,<Rn>,<Rm>{,<shift>}
							// RSC{S}<c> <Rd>,<Rn>,<Rm>,<type> <Rs>
							const char* pName = bS ? "rscs" : "rsc";

							if(bReg)
							{
								WriteARMInstructionCRRRS(dd, kITMisc, in, pName, sRd, 12, 15, sRn, 16, 19, sRm, 0, 3, 5, 6, sShiftHelp, 7, 11);
								Snprintf(dd.mHelp, EAArrayCount(dd.mHelp), "%s = %s (shifted with %s) - %s, with carry. %s", sRd, sRn, sRm, sShiftHelp, GetConditionHelpStrInt(in));
							}
							else
							{
								WriteARMInstructionCRRRSr(dd, kITMisc, in, pName, sRd, 12, 15, sRn, 16, 19, sRm, 0, 3, 5, 6, sShiftHelp, sRs, 8, 11);
								Snprintf(dd.mHelp, EAArrayCount(dd.mHelp), "%s = %s (shifted with (%s %s)) - %s, with carry. %s", sRd, sRn, sRm, sShiftHelp, sRs, GetConditionHelpStrInt(in));
							}

							break;
						}

						case  8: // TST
						case  9: // TEQ
						case 10: // CMP
						case 11: // CMN
							// These are handled above before the switch statement.
							break;

						case 12:
						{
							// ORR{S}<c> <Rd>,<Rn>,<Rm>{,<shift>}
							// ORR{S}<c> <Rd>,<Rn>,<Rm>,<type> <Rs>
							const char* pName = bS ? "orrs" : "orr";

							if(bReg)
							{
								WriteARMInstructionCRRS(dd, kITMisc, in, pName, sRn, 16, 19, sRm, 0, 3, 5, 6, sShiftHelp, 7, 11);
								Snprintf(dd.mHelp, EAArrayCount(dd.mHelp), "Bitwise OR. Does %s OR %s%s. %s", sRn, sShiftHelp, bS ? " and updates flags" : "", GetConditionHelpStrInt(in));
							}
							else
							{
								WriteARMInstructionCRRSr(dd, kITMisc, in, pName, sRn, 16, 19, sRm, 0, 3, 5, 6, sShiftHelp, sRs, 8, 11);
								Snprintf(dd.mHelp, EAArrayCount(dd.mHelp), "Bitwise OR. Does %s OR %s shifted with (%s %s). %s", sRn, sRm, sShiftHelp, sRs, bS ? " and updates flags" : "", GetConditionHelpStrInt(in));
							}
							break;
						}

						case 13:
						{
							ShiftType shiftType = (ShiftType)GetBitRangeUnsigned(in, 5, 6); 

							switch ((int)shiftType)
							{
								case kSRType_LSL: // kSRType_MOV
								{
									// MOV{S}<c> <Rd>,<Rm>
									// LSL{S}<c> <Rd>,<Rn>,<Rm>
									// LSL{S}<c> <Rd>,<Rm>,#<imm5>
									int bits4_11 = GetBitRangeUnsigned(in, 4, 11);

									if(bits4_11 == 0) 
									{
										// MOV{S}<c> <Rd>,<Rm>
										const char* pName = bS ? "movs" : "mov";
										WriteARMInstructionCRRS(dd, kITImmediate, in, pName, sRd, 12, 15, sRm, 0, 3, 4, 4, sShiftHelp, 4, 11);

										if(Strcmp(sRd, sRm) == 0) // If the registers are equal and thus this actually is a no-op (which GCC often does)...
										{
											Snprintf(dd.mHelp, EAArrayCount(dd.mHelp), "Encoded as: %s %s", dd.mOperation, dd.mOperands);
											Strcpy(dd.mOperation, "nop");
											dd.mOperands[0] = 0;
										}
										else
											Snprintf(dd.mHelp, EAArrayCount(dd.mHelp), "%s = %s", sRd, sRm);

									}
									else if(GetBitRaw(in, 4))
									{
										// LSL{S}<c> <Rd>,<Rn>,<Rm>
										WriteARMInstruction(dd, kITMisc, "lsl ...");
										Sprintf(dd.mHelp, "EADasmArm.cpp line %d", __LINE__);
									}
									else
									{
										// LSL{S}<c> <Rd>,<Rm>,#<imm5>
										WriteARMInstruction(dd, kITMisc, "lsl ...");
										Sprintf(dd.mHelp, "EADasmArm.cpp line %d", __LINE__);
									}
									break;
								}

								case kSRType_LSR:
									// LSR{S}<c> <Rd>,<Rm>,#<imm>
									// LSR{S}<c> <Rd>,<Rn>,<Rm>
									WriteARMInstruction(dd, kITMisc, "lsr ...");
									Sprintf(dd.mHelp, "EADasmArm.cpp line %d", __LINE__);
									break;

								case kSRType_ASR:
									// ASR{S}<c> <Rd>,<Rm>,#<imm>
									// ASR{S}<c> <Rd>,<Rn>,<Rm>
									WriteARMInstruction(dd, kITMisc, "asr ...");
									Sprintf(dd.mHelp, "EADasmArm.cpp line %d", __LINE__);
									break;

								case kSRType_ROR: // kSRType_RRX
									// RRX{S}<c> <Rd>,<Rm>
									// ROR{S}<c> <Rd>,<Rm>,#<imm>
									// ROR{S}<c> <Rd>,<Rn>,<Rm>
									WriteARMInstruction(dd, kITMisc, "ror/rrx ...");
									Sprintf(dd.mHelp, "EADasmArm.cpp line %d", __LINE__);
									break;
							}
							break;
						}

						case 14:
						{
							// BIC{S}<c> <Rd>,<Rn>,<Rm>{,<shift>}
							const char* pName = bS ? "bics" : "bic";
							WriteARMInstructionCRRRS(dd, kITMisc, in, pName, sRd, 12, 15, sRn, 16, 19, sRm, 0, 3, 5, 6, sShiftHelp, 7, 11);
							Snprintf(dd.mHelp, EAArrayCount(dd.mHelp), "%s = %s AND ~%s shifted with %s%s. %s", sRd, sRn, sRm, sShiftHelp, bS ? " and updates flags" : "", GetConditionHelpStrInt(in));
							break;
						}

						case 15:
						{
							// MVN{S}<c> <Rd>,<Rm>{,<shift>}
							const char* pName = bS ? "mvns" : "mvn";
							WriteARMInstructionCRRS(dd, kITMisc, in, pName, sRd, 12, 15, sRm, 0, 3, 5, 6, sShiftHelp, 7, 11);
							Snprintf(dd.mHelp, EAArrayCount(dd.mHelp), "Bitwise NOT (register). %s = ~(%s shifted with %s)%s. %s", sRd, sRn, sShiftHelp, bS ? " and updates flags" : "", GetConditionHelpStrInt(in));
							break;
						}
					}
				}
			}

			// 0, 10xx0 0xxx Miscellaneous instructions, 1xx0 Halfword multiply and multiply-accumulate
			else if(((bits20_24 & 0x19) == 0x10) && (((bits4_7 & 0x08) == 0x00) || ((bits4_7 & 0x09) == 0x08)))
			{
				int bits21_22 = GetBitRangeUnsigned(in, 21, 22);

				// 10xx0 0xxx Miscellaneous instructions
				if(((bits4_7 & 0x08) == 0x00))
				{
					switch(bits4_7)
					{
						case 0:
						{
							// Move Special Register to Register / Move to Special Register, application level / Move to Special Register, system level
							if(bits21_22 == 0x03)
							{
								// Move to Special Register, system level
								// MSR<c> <spec_reg>,<Rn>
								WriteARMInstruction(dd, kITMisc, "msr ...");
								Sprintf(dd.mHelp, "EADasmArm.cpp line %d", __LINE__);
							}
							else if(bits21_22 == 0x01)
							{
								// Move to Special Register, application level
								// MSR<c> <spec_reg>,<Rn>
								WriteARMInstruction(dd, kITMisc, "msr ...");
								Sprintf(dd.mHelp, "EADasmArm.cpp line %d", __LINE__);
							}
							else 
							{
								// Move Special Register to Register
								// MRS<c> <Rd>,<spec_reg>
								// Two variations of this?
								WriteARMInstruction(dd, kITMisc, "mrs ...");
								Sprintf(dd.mHelp, "EADasmArm.cpp line %d", __LINE__);
							}
							break;
						}

						case 1:
						{
							// Branch and Exchange / Count Leading Zeros
							if(bits21_22 == 0x01)
							{
								// BX<c> Rm
								WriteARMInstruction(dd, kITMisc, "bx ...");
								Sprintf(dd.mHelp, "EADasmArm.cpp line %d", __LINE__);
							}
							else if(bits21_22 == 0x03) 
							{
								// CLZ<c> <Rd>,<Rm>
								WriteARMInstruction(dd, kITMisc, "clz ...");
								Sprintf(dd.mHelp, "EADasmArm.cpp line %d", __LINE__);
							}
							break;
						}

						case 2:
							// Branch and Exchange Jazelle
							// BXJ<c> <Rm>
							WriteARMInstruction(dd, kITMisc, "bxj ...");
							Sprintf(dd.mHelp, "EADasmArm.cpp line %d", __LINE__);
							break;

						case 3:
							// Branch with Link and Exchange
							// BLX<c> <Rm>
							WriteARMInstruction(dd, kITMisc, "blx ...");
							Sprintf(dd.mHelp, "EADasmArm.cpp line %d", __LINE__);
							break;

						case 5:
						{
							// Saturating addition and subtraction
							switch (bits21_22)
							{
								case 0:
									// QADD<c> <Rd>,<Rm>,<Rn>
									WriteARMInstruction(dd, kITMisc, "qadd ...");
									Sprintf(dd.mHelp, "EADasmArm.cpp line %d", __LINE__);
									break;
								case 1:
									// QSUB<c> <Rd>,<Rm>,<Rn>
									WriteARMInstruction(dd, kITMisc, "qsub ...");
									Sprintf(dd.mHelp, "EADasmArm.cpp line %d", __LINE__);
									break;
								case 2:
									// QDADD<c> <Rd>,<Rm>,<Rn>
									WriteARMInstruction(dd, kITMisc, "qdadd ...");
									Sprintf(dd.mHelp, "EADasmArm.cpp line %d", __LINE__);
									break;
								case 3:
									// QDSUB<c> <Rd>,<Rm>,<Rn>
									WriteARMInstruction(dd, kITMisc, "qdsub ...");
									Sprintf(dd.mHelp, "EADasmArm.cpp line %d", __LINE__);
									break;
							}

							break;
						}

						case 7:
						{
							// Breakpoint / Secure Monitor Call
							if(bits21_22 == 0x01)
							{
								// BKPT #<imm16>
								WriteARMInstruction(dd, kITMisc, "bkpt ...");
								Sprintf(dd.mHelp, "EADasmArm.cpp line %d", __LINE__);
							}
							else if(bits21_22 == 0x03) 
							{
								// SMC<c> #<imm4> (priviliged)
								WriteARMInstruction(dd, kITMisc, "smc ...");
								Sprintf(dd.mHelp, "EADasmArm.cpp line %d", __LINE__);
							}
							break;
						}

						// Any others, fall through to invalid handling.
					}
				}

				// Halfword multiply and multiply-accumulate
				else
				{
					switch (bits21_22)
					{
						case 0: // Signed 16-bit multiply, 32-bit accumulate
						{
							// SMLA<x><y><c> <Rd>,<Rn>,<Rm>,<Ra>
							WriteARMInstruction(dd, kITMisc, "smla ...");
							Sprintf(dd.mHelp, "EADasmArm.cpp line %d", __LINE__);
							break;
						}
						case 1: // Signed 16-bit x 32-bit multiply, 32-bit accumulate / Signed 16-bit x 32-bit multiply, 32-bit result
						{
							// SMLAW<y><c> <Rd>,<Rn>,<Rm>,<Ra>
							// SMULW<y><c> <Rd>,<Rn>,<Rm>
							WriteARMInstruction(dd, kITMisc, "smlaw/smulw ...");
							Sprintf(dd.mHelp, "EADasmArm.cpp line %d", __LINE__);
							break;
						}
						case 2: // Signed 16-bit multiply, 64-bit accumulate
						{
							// SMLAL<x><y><c> <RdLo>,<RdHi>,<Rn>,<Rm>
							WriteARMInstruction(dd, kITMisc, "smlal ...");
							Sprintf(dd.mHelp, "EADasmArm.cpp line %d", __LINE__);
							break;
						}
						case 3: // Signed 16-bit multiply, 32-bit result
						{
							// SMUL<x><y><c> <Rd>,<Rn>,<Rm>
							WriteARMInstruction(dd, kITMisc, "smul ...");
							Sprintf(dd.mHelp, "EADasmArm.cpp line %d", __LINE__);
							break;
						}
					}
				}
			}

			// 0, 0xxxx 1001 Multiply and multiply-accumulate
			else if(((bits20_24 & 0x10) == 0x00) && (bits4_7 == 0x09))
			{
				int bits19_23 = GetBitRangeUnsigned(in, 19, 23);

				switch (bits19_23)
				{
					case 0: // MUL{S}<c> <Rd>,<Rn>,<Rm>
						WriteARMInstruction(dd, kITMisc, "mul ...");
						Sprintf(dd.mHelp, "EADasmArm.cpp line %d", __LINE__);
						break;
					case 1: // MLA{S}<c> <Rd>,<Rn>,<Rm>,<Ra>
						WriteARMInstruction(dd, kITMisc, "mla ...");
						Sprintf(dd.mHelp, "EADasmArm.cpp line %d", __LINE__);
						break;
					case 2: // UMAAL<c> <RdLo>,<RdHi>,<Rn>,<Rm> / UNDEFINED (if S bit is 1)
						WriteARMInstruction(dd, kITMisc, "umall ...");
						Sprintf(dd.mHelp, "EADasmArm.cpp line %d", __LINE__);
						break;
					case 3: // MLS<c> <Rd>,<Rn>,<Rm>,<Ra> / UNDEFINED (if S bit is 1)
						WriteARMInstruction(dd, kITMisc, "mls ...");
						Sprintf(dd.mHelp, "EADasmArm.cpp line %d", __LINE__);
						break;
					case 4: // UMULL{S}<c> <RdLo>,<RdHi>,<Rn>,<Rm>
						WriteARMInstruction(dd, kITMisc, "umull ...");
						Sprintf(dd.mHelp, "EADasmArm.cpp line %d", __LINE__);
						break;
					case 5: // UMLAL{S}<c> <RdLo>,<RdHi>,<Rn>,<Rm>
						WriteARMInstruction(dd, kITMisc, "umlal ...");
						Sprintf(dd.mHelp, "EADasmArm.cpp line %d", __LINE__);
						break;
					case 6: // SMULL{S}<c> <RdLo>,<RdHi>,<Rn>,<Rm>
						WriteARMInstruction(dd, kITMisc, "smull ...");
						Sprintf(dd.mHelp, "EADasmArm.cpp line %d", __LINE__);
						break;
					case 7: // SMLAL{S}<c> <RdLo>,<RdHi>,<Rn>,<Rm>
						WriteARMInstruction(dd, kITMisc, "smlal ...");
						Sprintf(dd.mHelp, "EADasmArm.cpp line %d", __LINE__);
						break;
				}
			}

			// 0, 1xxxx 1001 Synchronization primitives
			else if(((bits20_24 & 0x10) == 0x10) && (bits4_7 == 0x09))
			{
				if(!GetBitBool(in, 23))
				{
					// SWP{B}<c> <Rt>,<Rt2>,[<Rn>]
					// The use of SWP is deprecated, and new code should use LDREX/STREX in preference to using SWP
					// The use of SWPB is deprecated, and new code should use LDREXB/STREXB in preference to using SWPB.
					WriteARMInstruction(dd, kITMisc, "swp ...");     // To do: Improve this.
					Sprintf(dd.mHelp, "Swap register with memory");
				}
				else
				{
					int bits20_22 = GetBitRangeUnsigned(in, 20, 22);

					switch (bits20_22)
					{
						case 0: // STREX<c> <Rd>,<Rt>,[<Rn>]
							WriteARMInstruction(dd, kITMisc, "strex ...");  // To do: Improve this.
							Sprintf(dd.mHelp, "32 bit load reserved (for atomic ops)");
							break;
						case 1: // LDREX<c> <Rt>,[<Rn>]
							WriteARMInstruction(dd, kITMisc, "ldrex ...");
							Sprintf(dd.mHelp, "32 bit atomic store conditional (for atomic ops)");
							break;
						case 2: // STREXD<c> <Rd>,<Rt>,<Rt2>,[<Rn>]
							WriteARMInstruction(dd, kITMisc, "strexd ...");
							Sprintf(dd.mHelp, "64 bit load reserved (for atomic ops)");
							break;
						case 3: // LDREXD<c> <Rt>,<Rt2>,[<Rn>]
							WriteARMInstruction(dd, kITMisc, "ldrexd ...");
							Sprintf(dd.mHelp, "64 bit atomic store conditional (for atomic ops)");
							break;
						case 4: // STREXB<c> <Rd>,<Rt>,[<Rn>]
							WriteARMInstruction(dd, kITMisc, "strexb ...");
							Sprintf(dd.mHelp, "8 bit load reserved (for atomic ops)");
							break;
						case 5: // LDREXB<c> <Rt>, [<Rn>]
							WriteARMInstruction(dd, kITMisc, "ldrexb ...");
							Sprintf(dd.mHelp, "8 bit atomic store conditional (for atomic ops)");
							break;
						case 6: // STREXH<c> <Rd>,<Rt>,[<Rn>]
							WriteARMInstruction(dd, kITMisc, "strexh ...");
							Sprintf(dd.mHelp, "16 bit load reserved (for atomic ops)");
							break;
						case 7: // LDREXH<c> <Rt>, [<Rn>]
							WriteARMInstruction(dd, kITMisc, "ldrexh ...");
							Sprintf(dd.mHelp, "16 bit atomic store conditional (for atomic ops)");
							break;
					}
				}
			}

			// 0, not 0xx1x 1011 Extra load/store instructions, 11x1 Extra load/store instructions
			if(((bits20_24 & 0x12) != 0x10) && ((bits4_7 == 0x0b) || ((bits4_7 & 0x0d) == 0x0d)))
			{
				int bits5_6  = GetBitRangeUnsigned(in, 5, 6);

				if(bits5_6 == 0x01)
				{
					if((bits20_24 & 0x05) == 0x00)
					{
						// STRH<c> <Rt>,[<Rn>,+/-<Rm>]{!}
						// STRH<c> <Rt>,[<Rn>],+/-<Rm>
						WriteARMInstruction(dd, kITMisc, "strh ...");
						Sprintf(dd.mHelp, "EADasmArm.cpp line %d", __LINE__);
					}
					else if((bits20_24 & 0x05) == 0x01)
					{
						// LDRH<c> <Rt>,[<Rn>,+/-<Rm>]{!}
						// LDRH<c> <Rt>,[<Rn>],+/-<Rm>
						WriteARMInstruction(dd, kITMisc, "ldrh ...");
						Sprintf(dd.mHelp, "EADasmArm.cpp line %d", __LINE__);
					}
					else if((bits20_24 & 0x05) == 0x04)
					{
						// STRH<c> <Rt>,[<Rn>{,#+/-<imm8>}]
						// STRH<c> <Rt>,[<Rn>],#+/-<imm8>
						// STRH<c> <Rt>,[<Rn>,#+/-<imm8>]!
						WriteARMInstruction(dd, kITMisc, "strh ...");
						Sprintf(dd.mHelp, "EADasmArm.cpp line %d", __LINE__);
					}
					else if((bits20_24 & 0x05) == 0x05)
					{
						// LDRH<c> <Rt>,[<Rn>{,#+/-<imm8>}]
						// LDRH<c> <Rt>,[<Rn>],#+/-<imm8>
						// LDRH<c> <Rt>,[<Rn>,#+/-<imm8>]!
						// Rn == 1111 =>
						// LDRH<c> <Rt>,<label>
						// LDRH<c> <Rt>,[PC,#-0]
						WriteARMInstruction(dd, kITMisc, "ldrh ...");
						Sprintf(dd.mHelp, "EADasmArm.cpp line %d", __LINE__);
					}
				}
				else if(bits5_6 == 2)
				{
					if((bits20_24 & 0x05) == 0x00)
					{
						// LDRD<c> <Rt>,<Rt2>,[<Rn>,+/-<Rm>]{!}
						// LDRD<c> <Rt>,<Rt2>,[<Rn>],+/-<Rm>
						WriteARMInstruction(dd, kITMisc, "ldrd ...");
						Sprintf(dd.mHelp, "EADasmArm.cpp line %d", __LINE__);
					}
					else if((bits20_24 & 0x05) == 0x01)
					{
						// LDRSB<c> <Rt>,[<Rn>,+/-<Rm>]{!}
						// LDRSB<c> <Rt>,[<Rn>],+/-<Rm>
						WriteARMInstruction(dd, kITMisc, "ldrsb ...");
						Sprintf(dd.mHelp, "EADasmArm.cpp line %d", __LINE__);
					}
					else if((bits20_24 & 0x05) == 0x04)
					{
						// LDRD<c> <Rt>,<Rt2>,[<Rn>{,#+/-<imm8>}]
						// LDRD<c> <Rt>,<Rt2>,[<Rn>],#+/-<imm8>
						// LDRD<c> <Rt>,<Rt2>,[<Rn>,#+/-<imm8>]!
						// Rn == 1111 =>
						// LDRD<c> <Rt>,<Rt2>,<label>
						// LDRD<c> <Rt>,<Rt2>,[PC,#-0]
						WriteARMInstruction(dd, kITMisc, "ldrd ...");
						Sprintf(dd.mHelp, "EADasmArm.cpp line %d", __LINE__);
					}
					else if((bits20_24 & 0x05) == 0x05)
					{
						// LDRSB<c> <Rt>,[<Rn>{,#+/-<imm8>}]
						// LDRSB<c> <Rt>,[<Rn>],#+/-<imm8>
						// LDRSB<c> <Rt>,[<Rn>,#+/-<imm8>]!
						// Rn == 1111 =>
						// LDRSB<c> <Rt>,<label>
						// LDRSB<c> <Rt>,[PC,#-0]
						WriteARMInstruction(dd, kITMisc, "ldrsb ...");
						Sprintf(dd.mHelp, "EADasmArm.cpp line %d", __LINE__);
					}
				}
				else if(bits5_6 == 3)
				{
					if((bits20_24 & 0x05) == 0x00)
					{
						// STRD<c> <Rt>,<Rt2>,[<Rn>,+/-<Rm>]{!}
						// STRD<c> <Rt>,<Rt2>,[<Rn>],+/-<Rm>
						WriteARMInstruction(dd, kITMisc, "strd ...");
						Sprintf(dd.mHelp, "EADasmArm.cpp line %d", __LINE__);
					}
					else if((bits20_24 & 0x05) == 0x01)
					{
						// LDRSH<c> <Rt>,[<Rn>,+/-<Rm>]{!}
						// LDRSH<c> <Rt>,[<Rn>],+/-<Rm>
						WriteARMInstruction(dd, kITMisc, "ldrsh ...");
						Sprintf(dd.mHelp, "EADasmArm.cpp line %d", __LINE__);
					}
					else if((bits20_24 & 0x05) == 0x04)
					{
						// STRD<c> <Rt>,<Rt2>,[<Rn>{,#+/-<imm8>}]
						// STRD<c> <Rt>,<Rt2>,[<Rn>],#+/-<imm8>
						// STRD<c> <Rt>,<Rt2>,[<Rn>,#+/-<imm8>]!
						WriteARMInstruction(dd, kITMisc, "strd ...");
						Sprintf(dd.mHelp, "EADasmArm.cpp line %d", __LINE__);
					}
					else if((bits20_24 & 0x05) == 0x05)
					{
						// LDRSH<c> <Rt>,[<Rn>{,#+/-<imm8>}]
						// LDRSH<c> <Rt>,[<Rn>],#+/-<imm8>
						// LDRSH<c> <Rt>,[<Rn>,#+/-<imm8>]!
						// Rn == 1111 =>
						// LDRSH<c> <Rt>,<label>
						// LDRSH<c> <Rt>,[PC,#-0]
						WriteARMInstruction(dd, kITMisc, "ldrsh ...");
						Sprintf(dd.mHelp, "EADasmArm.cpp line %d", __LINE__);
					}
				}
			}

			// 0, 0xx1x 1011 Extra load/store instructions (unprivileged), 11x1 Extra load/store instructions
			if(((bits20_24 & 0x12) == 0x10) && ((bits4_7 == 0x0b) || ((bits4_7 & 0x0d) == 0x0d)))
			{
				int  bits5_6 = GetBitRangeUnsigned(in, 5, 6);
				bool bit20   = GetBitBool(in, 20);

				if(bits5_6 == 0x01)
				{
					if(bit20)
					{
						// LDRHT<c> <Rt>, [<Rn>] {, #+/-<imm8>}
						// LDRHT<c> <Rt>, [<Rn>], +/-<Rm>
						WriteARMInstruction(dd, kITMisc, "ldrht ...");
						Sprintf(dd.mHelp, "EADasmArm.cpp line %d", __LINE__);
					}
					else
					{
						// STRHT<c> <Rt>, [<Rn>] {, #+/-<imm8>}
						// STRHT<c> <Rt>, [<Rn>], +/-<Rm>
						WriteARMInstruction(dd, kITMisc, "strht ...");
						Sprintf(dd.mHelp, "EADasmArm.cpp line %d", __LINE__);
					}
				}
				else if(bits5_6 == 2)
				{
					if(bit20)
					{
						// LDRSBT<c> <Rt>, [<Rn>] {, #+/-<imm8>}
						// LDRSBT<c> <Rt>, [<Rn>], +/-<Rm>
						WriteARMInstruction(dd, kITMisc, "ldrsbt ...");
						Sprintf(dd.mHelp, "EADasmArm.cpp line %d", __LINE__);
					}
					else
					{
						// Unpredictable / Undefined
					}
				}
				else if(bits5_6 == 3)
				{
					if(bit20)
					{
						// LDRSHT<c> <Rt>, [<Rn>] {, #+/-<imm8>}
						// LDRSHT<c> <Rt>, [<Rn>], +/-<Rm>
						WriteARMInstruction(dd, kITMisc, "ldrsht ...");
						Sprintf(dd.mHelp, "EADasmArm.cpp line %d", __LINE__);
					}
					else
					{
						// Unpredictable / Undefined
					}
				}
			}
		}
		else
		{
			// 1, 10000 - 16-bit immediate load (MOV (immediate)
			if(bits20_24 == 0x10)
			{
				// MOV{S}<c> <Rd>,#<const>
				// MOVW<c> <Rd>,#<imm16>
				WriteARMInstruction(dd, kITMisc, "mov/movw ...");
				Sprintf(dd.mHelp, "EADasmArm.cpp line %d", __LINE__);
			}

			// 1, 10100 - High halfword 16-bit immediate load
			else if(bits20_24 == 0x14)
			{
				// MOVT<c> <Rd>,#<imm16>
				WriteARMInstruction(dd, kITMisc, "movt ...");
				Sprintf(dd.mHelp, "EADasmArm.cpp line %d", __LINE__);
			}

			// 1, 10x10 - MSR (immediate), and hints
			else if((bits20_24 & 0x1b) == 0x12)
			{
				// MSR (immediate), and hints on page A5-17
				const bool bit22 = GetBitBool(in, 22);

				if(!bit22)
				{
					int bits16_19 = GetBitRangeUnsigned(in, 16, 19);

					if(bits16_19 == 0) // If hints...
					{
						int bits0_7 = GetBitRangeUnsigned(in, 0, 7);
						WriteARMHintInstruction(dd, condition, bits0_7);
					}
					else // Else MSR (immediate)...
					{
						if((bits16_19 == 0x04) || ((bits16_19 & 0x0b) == 0x08))
						{
							// MSR<c><q> <spec_reg>, #<imm>
							WriteARMInstruction(dd, kITMisc, "msr ...");
							Sprintf(dd.mHelp, "EADasmArm.cpp line %d", __LINE__);
						}
						else if(((bits16_19 & 3) == 0x01) || (bits16_19 & 2))
						{
							// MSR<c> <spec_reg>,#<const>
							// Same as a few lines below.
							WriteARMInstruction(dd, kITMisc, "msr ...");
							Sprintf(dd.mHelp, "EADasmArm.cpp line %d", __LINE__);
						}
						// Else invalid
					}
				}
				else
				{
					// MSR<c> <spec_reg>,#<const>
					// Same as a few lines above.
					WriteARMInstruction(dd, kITMisc, "msr ...");
					Sprintf(dd.mHelp, "EADasmArm.cpp line %d", __LINE__);
				}
			}

			// 1, not 10xx0 - Data-processing (immediate)
			else
			{
				// The following is about 80% similar to the Data-processing (register) opcodes handled above. Perhaps the logic can be merged.
				// Data-processing (register)
				if(bits20_24 == 0x11)
				{
					// TST<c> <Rn>,#<const>
					WriteARMInstruction(dd, kITMisc, "tst ...");
					Sprintf(dd.mHelp, "EADasmArm.cpp line %d", __LINE__);
				}
				if(bits20_24 == 0x13)
				{
					// TEQ<c> <Rn>,#<const>
					WriteARMInstruction(dd, kITMisc, "teq ...");
					Sprintf(dd.mHelp, "EADasmArm.cpp line %d", __LINE__);
				}
				if(bits20_24 == 0x15)
				{
					// CMP<c> <Rn>,#<const>
					WriteARMInstruction(dd, kITMisc, "cmp ...");
					Sprintf(dd.mHelp, "EADasmArm.cpp line %d", __LINE__);
				}
				if(bits20_24 == 0x17)
				{
					// CMN<c> <Rn>,#<const>
					WriteARMInstruction(dd, kITMisc, "cmn ...");
					Sprintf(dd.mHelp, "EADasmArm.cpp line %d", __LINE__);
				}

				switch (bits20_24 >> 1) // Shift away the S bit.
				{
					// Note: This could be table-driven, since the values are incremental.
					case  0:
						// AND{S}<c> <Rd>,<Rn>,#<const>
						WriteARMInstruction(dd, kITMisc, "and ...");
						Sprintf(dd.mHelp, "EADasmArm.cpp line %d", __LINE__);
						break;

					case  1:
						// EOR{S}<c> <Rd>,<Rn>,#<const>
						WriteARMInstruction(dd, kITMisc, "eor ...");
						break;

					case  3:
						// RSB{S}<c> <Rd>,<Rn>,#<const>
						WriteARMInstruction(dd, kITMisc, "rsb ...");
						Sprintf(dd.mHelp, "EADasmArm.cpp line %d", __LINE__);
						break;

					case  2:
						// SUB{S}<c> <Rd>,<Rn>,#<const>
						// Rn = 1111 =>
						//   ADR<c> <Rd>,<label>   (label after current instruction)
						//   ADR<c> <Rd>,<label>   (label before current instruction)
						//   SUB <Rd>,PC,#0        (subtraction of 0)

						// fall through

					case  4:
					{
						// ADD{S}<c> <Rd>,<Rn>,#<const>
						// Rn = 1111 =>
						//   ADR<c> <Rd>,<label> (<label> after current instruction)
						//   ADR<c> <Rd>,<label> (<label> before current instruction)
						//   SUB <Rd>,PC,#0 (Special case for subtraction of zero)
						// if Rn == ‘1101’ then SEE ADD (SP plus immediate);
						Rn = GetBitRangeUnsigned(in, 16, 19);
						int S = GetBitRaw(in, 20);

						if(Rn == kRegPC)
						{
							if(S)
							{
								// ADR<c> <Rd>,<label> (<label> after current instruction)
								// ADR<c> <Rd>,<label> (<label> before current instruction)
								WriteARMInstruction(dd, kITMisc, "adr ...");
								Sprintf(dd.mHelp, "EADasmArm.cpp line %d", __LINE__);
							}
							else
							{
								// SUB <Rd>,PC,#0 (Special case for subtraction of zero)
								WriteARMInstruction(dd, kITMisc, "sub ...");
								Sprintf(dd.mHelp, "EADasmArm.cpp line %d", __LINE__);
							}
						}
						else
						{
							// ADD{S}<c> <Rd>,<Rn>,#<const>
							const char* pName = ((bits20_24 >> 1) == 2) ? (S ? "sub" : "subs") : (S ? "add" : "adds");
							WriteARMInstruction(dd, kITMisc, pName);
							//WriteARMInstructionRRMic();
							// If Rd = Rn = SP, then this is usually the creation and cleanup of a stack frame.
							Sprintf(dd.mHelp, "EADasmArm.cpp line %d", __LINE__);
						}

						break;
					}

					case  5:
						// ADC{S}<c> <Rd>,<Rn>,#<const>
						WriteARMInstruction(dd, kITMisc, "adc ...");
						Sprintf(dd.mHelp, "EADasmArm.cpp line %d", __LINE__);
						break;

					case  6:
						// SBC{S}<c> <Rd>,<Rn>,#<const>
						WriteARMInstruction(dd, kITMisc, "sbc ...");
						Sprintf(dd.mHelp, "EADasmArm.cpp line %d", __LINE__);
						break;

					case  7:
						// RSC{S}<c> <Rd>,<Rn>,#<const>
						WriteARMInstruction(dd, kITMisc, "rsc ...");
						Sprintf(dd.mHelp, "EADasmArm.cpp line %d", __LINE__);
						break;

					case  8: // TST
					case  9: // TEQ
					case 10: // CMP
					case 11: // CMN
						// These are handled above before the switch statement.
						break;

					case 12:
						// ORR{S}<c> <Rd>,<Rn>,#<const>
						WriteARMInstruction(dd, kITMisc, "orr ...");
						Sprintf(dd.mHelp, "EADasmArm.cpp line %d", __LINE__);
						break;

					case 13:
					{
						// MOV{S}<c> <Rd>,#<const>    (12 bits)
						// MOVW<c> <Rd>,#<imm16>      (16 bits)
						WriteARMInstruction(dd, kITMisc, "mov/movw ...");
						Sprintf(dd.mHelp, "EADasmArm.cpp line %d", __LINE__);
						break;
					}

					case 14:
						// BIC{S}<c> <Rd>,<Rn>,#<const>
						WriteARMInstruction(dd, kITMisc, "bic ...");
						Sprintf(dd.mHelp, "EADasmArm.cpp line %d", __LINE__);
						break;

					case 15:
						// MVN{S}<c> <Rd>,#<const>
						WriteARMInstruction(dd, kITMisc, "mvn ...");
						Sprintf(dd.mHelp, "EADasmArm.cpp line %d", __LINE__);
						break;
				}
			}
		}
	}

	// 010   - Load/store word and unsigned byte
	// 011 0 - Load/store word and unsigned byte
	else if((bits25_27 == 2) || ((bits25_27 == 3) && !GetBitBool(in, 4)))
	{
		bool A   = GetBitBool(in, 25);
		bool B   = GetBitBool(in, 4);

		if(A && !B)
		{
			// 1 0x010 0 - Store Register Unprivileged
			if((bits20_24 & 0x17) == 0x02)
			{
				// STRT<c> <Rt>, [<Rn>] {, +/-<imm12>}
				// STRT<c> <Rt>,[<Rn>],+/-<Rm>{, <shift>}
				WriteARMInstruction(dd, kITMisc, "strt ...");
				Sprintf(dd.mHelp, "EADasmArm.cpp line %d", __LINE__);
			}

			// 1 xx0x0 not 0x010 0 - Store Register
			else if((bits20_24 & 0x05) == 0x00)
			{
				// STR<c> <Rt>,[<Rn>,+/-<Rm>{, <shift>}]{!}
				// STR<c> <Rt>,[<Rn>],+/-<Rm>{, <shift>}
				WriteARMInstruction(dd, kITMisc, "str ...");
				Sprintf(dd.mHelp, "EADasmArm.cpp line %d", __LINE__);
			}

			// 1 0x011 0 - Load Register Unprivileged
			else if((bits20_24 & 0x17) == 0x03)
			{
				// LDRT<c> <Rt>, [<Rn>] {, #+/-<imm12>}
				// LDRT<c> <Rt>,[<Rn>],+/-<Rm>{, <shift>}
				WriteARMInstruction(dd, kITMisc, "ldrt ...");
				Sprintf(dd.mHelp, "EADasmArm.cpp line %d", __LINE__);
			}

			// 1 xx0x1 not 0x011 0 - Load Register
			else if((bits20_24 & 0x05) == 0x01)
			{
				// LDR<c> <Rt>,[<Rn>,+/-<Rm>{, <shift>}]{!}
				// LDR<c> <Rt>,[<Rn>],+/-<Rm>{, <shift>}
				WriteARMInstruction(dd, kITMisc, "ldr ...");
				Sprintf(dd.mHelp, "EADasmArm.cpp line %d", __LINE__);
			}

			// 1 0x110 0 - Store Register Byte Unprivileged
			else if((bits20_24 & 0x17) == 0x06)
			{
				// STRBT<c> <Rt>,[<Rn>],#+/-<imm12>
				// STRBT<c> <Rt>,[<Rn>],+/-<Rm>{, <shift>}
				// Any connection with the strbt below?
				WriteARMInstruction(dd, kITMisc, "strbt ...");
				Sprintf(dd.mHelp, "EADasmArm.cpp line %d", __LINE__);
			}

			// 1 xx1x0 not 0x110 0 - Store Register Byte (register)
			if((bits20_24 & 0x17) == 0x06)
			{
				// STRB<c> <Rt>,[<Rn>,+/-<Rm>{, <shift>}]{!}
				// STRB<c> <Rt>,[<Rn>],+/-<Rm>{, <shift>}
				WriteARMInstruction(dd, kITMisc, "strb ...");
				Sprintf(dd.mHelp, "EADasmArm.cpp line %d", __LINE__);
			}

			// 1 0x111 0 - Load Register Byte Unprivileged
			else if((bits20_24 & 0x17) == 0x07)
			{
				// LDRBT<c> <Rt>,[<Rn>],#+/-<imm12>
				// LDRBT<c> <Rt>,[<Rn>],+/-<Rm>{, <shift>}
				WriteARMInstruction(dd, kITMisc, "ldrbt ...");
				Sprintf(dd.mHelp, "EADasmArm.cpp line %d", __LINE__);
			}

			// 1 xx1x1 not 0x111 0 - Load Register Byte (register)
			if((bits20_24 & 0x05) == 0x05)
			{
				// LDRB<c> <Rt>,[<Rn>,+/-<Rm>{, <shift>}]{!}
				// LDRB<c> <Rt>,[<Rn>],+/-<Rm>{, <shift>}
				WriteARMInstruction(dd, kITMisc, "ldrb ...");
				Sprintf(dd.mHelp, "EADasmArm.cpp line %d", __LINE__);
			}
		}
		else if(!A)
		{
			Rn  = GetBitRangeUnsigned(in, 16, 19);

			// 0 0x010 - Store Register Unprivileged
			if((bits20_24 & 0x17) == 0x02)
			{
				// STRT<c> <Rt>, [<Rn>] {, +/-<imm12>}
				// STRT<c> <Rt>,[<Rn>],+/-<Rm>{, <shift>}
				WriteARMInstruction(dd, kITMisc, "strt ...");
				Sprintf(dd.mHelp, "EADasmArm.cpp line %d", __LINE__);
			}

			// 0 xx0x0 not 0x010 - - Store Register
			else if((bits20_24 & 0x05) == 0x00)
			{
				// STR<c> <Rt>,[<Rn>{,#+/-<imm12>}]     // Offset addressing.       Address = (Rn + immediate).
				// STR<c> <Rt>,[<Rn>,#+/-<imm12>]!      // Pre-indexed addressing.  Address = (Rn += immediate).
				// STR<c> <Rt>,[<Rn>],#+/-<imm12>       // Post-indexed addressing. Address = Rn; Rn += immediate.
				// "if Rn == ‘1101’ && P == ‘1’ && U == ‘0’ && W == ‘1’ && imm12 == ‘000000000100’ then SEE PUSH;"
				Rn    = GetBitRangeUnsigned(in, 16, 19);
				value = GetBitRangeUnsigned(in, 0, 11);

				if((Rn == kRegSP) && (value == 0x04) && GetBitRaw(in, 24) && !GetBitRaw(in, 23) && GetBitRaw(in, 21)) // If this is effectively "push { <register> }"
				{
					dd.mInstructionType = kITStore;
					Strcpy(dd.mOperation, "push");

					GetARMRegisterName(in, sRt, 12, 15);
					Snprintf(dd.mOperands, EAArrayCount(dd.mOperands), "{ %s }", sRt);

					Snprintf(dd.mHelp, EAArrayCount(dd.mHelp), "Push %s onto the stack.", sRt);
				}
				else    
				{
					WriteARMInstructionRRIo(dd, kITStore, in, "str", sRt, 12, 15, sRn, 16, 19, value, 23, 0, 11, 24, 21, sOffsetHelp);
					Snprintf(dd.mHelp, EAArrayCount(dd.mHelp), "Write register to memory. %s = %s", sRt, sOffsetHelp);
				}
			}

			// 0 0x011 - Load Register Unprivileged
			else if((bits20_24 & 0x17) == 0x03)
			{
				// LDRT<c> <Rt>, [<Rn>] {, #+/-<imm12>}
				// LDRT<c> <Rt>,[<Rn>],+/-<Rm>{, <shift>}
				WriteARMInstruction(dd, kITMisc, "ldrt ...");
				Sprintf(dd.mHelp, "EADasmArm.cpp line %d", __LINE__);
			}

			// 0 xx0x1 not 0x011 - not 1111 Load Register (immediate)
			// 0 xx0x1 not 0x011 -     1111 Load Register (literal)
			else if((bits20_24 & 0x05) == 0x01)
			{
				if(Rn == 0x0f)
				{
					// LDR<c> <Rt>,<label>
					// LDR<c> <Rt>,[PC,#-0]
					WriteARMInstruction(dd, kITMisc, "ldr ...");
					Sprintf(dd.mHelp, "EADasmArm.cpp line %d", __LINE__);
				}
				else
				{
					// LDR<c> <Rt>,[<Rn>{,#+/-<imm12>}]
					// LDR<c> <Rt>,[<Rn>],#+/-<imm12>
					// LDR<c> <Rt>,[<Rn>,#+/-<imm12>]!
					WriteARMInstruction(dd, kITMisc, "ldr ...");
					Sprintf(dd.mHelp, "EADasmArm.cpp line %d", __LINE__);
				}
			}

			// 0 0x110 - Store Register Byte Unprivileged
			else if((bits20_24 & 0x17) == 0x06)
			{
				// STRBT<c> <Rt>,[<Rn>],#+/-<imm12>
				// STRBT<c> <Rt>,[<Rn>],+/-<Rm>{, <shift>}
				// Any connection with the strbt above?
				WriteARMInstruction(dd, kITMisc, "strbt ...");
				Sprintf(dd.mHelp, "EADasmArm.cpp line %d", __LINE__);
			}

			// 0 xx1x0 not 0x110 - Store Register Byte (immediate)
			if((bits20_24 & 0x17) == 0x06)
			{
				// STRB<c> <Rt>,[<Rn>{,#+/-<imm12>}]
				// STRB<c> <Rt>,[<Rn>],#+/-<imm12>
				// STRB<c> <Rt>,[<Rn>,#+/-<imm12>]!
				WriteARMInstruction(dd, kITMisc, "strb ...");
				Sprintf(dd.mHelp, "EADasmArm.cpp line %d", __LINE__);
			}

			// 0 0x111 - Load Register Byte Unprivileged
			else if((bits20_24 & 0x17) == 0x07)
			{
				// LDRBT<c> <Rt>,[<Rn>],#+/-<imm12>
				// LDRBT<c> <Rt>,[<Rn>],+/-<Rm>{, <shift>}
				WriteARMInstruction(dd, kITMisc, "ldrbt ...");
				Sprintf(dd.mHelp, "EADasmArm.cpp line %d", __LINE__);
			}

			// 0 xx1x1 not 0x111 - not 1111 Load Register Byte (immediate)
			// 0 xx1x1 not 0x111 -     1111 Load Register Byte (literal)
			if((bits20_24 & 0x05) == 0x05)
			{
				if(Rn == 0x0f)
				{
					// LDRB<c> <Rt>,<label>
					// LDRB<c> <Rt>,[PC,#-0]
					WriteARMInstruction(dd, kITMisc, "ldrb ...");
					Sprintf(dd.mHelp, "EADasmArm.cpp line %d", __LINE__);
				}
				else
				{
					// LDRB<c> <Rt>,[<Rn>{,#+/-<imm12>}]
					// LDRB<c> <Rt>,[<Rn>],#+/-<imm12>
					// LDRB<c> <Rt>,[<Rn>,#+/-<imm12>]!
					WriteARMInstruction(dd, kITMisc, "ldrb ...");
					Sprintf(dd.mHelp, "EADasmArm.cpp line %d", __LINE__);
				}
			}
		}
	}

	// 011 1 - Media instructions
	else if(bits25_27 == 3) // && GetBitBool(in, 4) -> don't need to test for this because it's done above.
	{
		int bits5_7 = GetBitRangeUnsigned(in, 5, 7);

		Rm = GetBitRangeUnsigned(in, 0, 3);
		Rd = GetBitRangeUnsigned(in, 12, 15);
		Rn = GetBitRangeUnsigned(in, 16, 19);

		EA_UNUSED(Rm);
		EA_UNUSED(Rd);
		EA_UNUSED(Rn);

		// 000xx - - - - Parallel addition and subtraction, signed
		if((bits20_24 & 0x1c) == 0x00)
		{
			const char* pNameArray[24] = {  "sadd16",  "sasx",  "ssax",  "ssub16",  "sadd8",  "", "", "ssub8", 
											"qadd16",  "qasx",  "qsax",  "qsub16",  "qadd8",  "", "", "qsub8", 
											"shadd16", "shasx", "shsax", "shsub16", "shadd8", "", "", "shsub8" };
			int bits5_7_20_21 = (GetBitRangeUnsigned(in, 20, 21) << 3) | bits5_7; // Two ranges as one integer. It just happens to be useful to do this due to the opcode bit encoding here.
			int index         = (bits5_7_20_21 - 0x08);

			if((index >= 0) && (index <= (int)EAArrayCount(pNameArray)))
			{
				const char* pName = pNameArray[bits5_7_20_21 - 0x08];
				WriteARMInstruction(dd, kITMisc, pName); // "pName ..."
				Sprintf(dd.mHelp, "EADasmArm.cpp line %d", __LINE__);
			}
		}

		// 001xx - - - - Parallel addition and subtraction, unsigned
		else if((bits20_24 & 0x1c) == 0x04)
		{
			const char* pNameArray[24] = {  "uadd16",  "uasx",  "usax",  "usub16",  "uadd8",  "", "", "usub8", 
											"uqadd16", "uqasx", "uqsax", "uqsub16", "uqadd8", "", "", "uqsub8", 
											"uhadd16", "uhasx", "uhsax", "uhsub16", "uhadd8", "", "", "uhsub8" };
			int bits5_7_20_21 = (GetBitRangeUnsigned(in, 20, 21) << 3) | bits5_7; // Two ranges as one integer. It just happens to be useful to do this due to the opcode bit encoding here.
			int index         = (bits5_7_20_21 - 0x08);

			if((index >= 0) && (index <= (int)EAArrayCount(pNameArray)))
			{
				const char* pName = pNameArray[bits5_7_20_21 - 0x08];
				WriteARMInstruction(dd, kITMisc, pName); // "pName ..."
				Sprintf(dd.mHelp, "EADasmArm.cpp line %d", __LINE__);
			}
		}

		// 01xxx - - - - Packing, unpacking, saturation, and reversal
		else if((bits20_24 & 0x18) == 0x08)
		{
			int bits5_7_20_22 = (GetBitRangeUnsigned(in, 20, 22) << 3) | bits5_7; // Two ranges as one integer. It just happens to be useful to do this due to the opcode bit encoding here.
			int bits16_19     = GetBitRangeUnsigned(in, 16, 19);

			if((bits5_7_20_22 & 0x39) == 0x00)
			{
				// PKHBT<c> <Rd>,<Rn>,<Rm>{,LSL #<imm>}
				// PKHTB<c> <Rd>,<Rn>,<Rm>{,ASR #<imm>}
				WriteARMInstruction(dd, kITMisc, "pkhbt/pkhtb ...");
				Sprintf(dd.mHelp, "EADasmArm.cpp line %d", __LINE__);
			}

			if((bits5_7_20_22 & 0x31) == 0x10)
			{
				// SSAT<c> <Rd>,#<imm>,<Rn>{,<shift>}
				WriteARMInstruction(dd, kITMisc, "ssat ...");
				Sprintf(dd.mHelp, "EADasmArm.cpp line %d", __LINE__);
			}

			if((bits5_7_20_22 & 0x31) == 0x30)
			{
				// USAT<c> <Rd>,#<imm5>,<Rn>{,<shift>}
				WriteARMInstruction(dd, kITMisc, "usat ...");
				Sprintf(dd.mHelp, "EADasmArm.cpp line %d", __LINE__);
			}

			else if(bits5_7_20_22 == 0x03)
			{
				if(bits16_19 != 0x0f)
				{
					// SXTAB16<c> <Rd>,<Rn>,<Rm>{,<rotation>}
					WriteARMInstruction(dd, kITMisc, "sxtab16 ...");
					Sprintf(dd.mHelp, "EADasmArm.cpp line %d", __LINE__);
				}
				else
				{
					// SXTB16<c> <Rd>,<Rm>{,<rotation>}
					WriteARMInstruction(dd, kITMisc, "sxtb16 ...");
					Sprintf(dd.mHelp, "EADasmArm.cpp line %d", __LINE__);
				}
			}

			else if(bits5_7_20_22 == 0x05)
			{
				// SEL<c> <Rd>,<Rn>,<Rm>
				WriteARMInstruction(dd, kITMisc, "sel ...");
				Sprintf(dd.mHelp, "EADasmArm.cpp line %d", __LINE__);
			}

			else if(bits5_7_20_22 == 0x11)
			{
				// SSAT16<c> <Rd>,#<imm>,<Rn>
				WriteARMInstruction(dd, kITMisc, "ssat16 ...");
				Sprintf(dd.mHelp, "EADasmArm.cpp line %d", __LINE__);
			}

			else if(bits5_7_20_22 == 0x13)
			{
				if(bits16_19 != 0x0f)
				{
					// SXTAB<c> <Rd>,<Rn>,<Rm>{,<rotation>}
					WriteARMInstruction(dd, kITMisc, "sxtab ...");
					Sprintf(dd.mHelp, "EADasmArm.cpp line %d", __LINE__);
				}
				else
				{
					// SXTB<c> <Rd>,<Rm>{,<rotation>}
					WriteARMInstruction(dd, kITMisc, "sxtb ...");
					Sprintf(dd.mHelp, "EADasmArm.cpp line %d", __LINE__);
				}
			}

			else if(bits5_7_20_22 == 0x19)
			{
				// REV<c> <Rd>,<Rm>
				WriteARMInstruction(dd, kITMisc, "rev ...");
				Sprintf(dd.mHelp, "EADasmArm.cpp line %d", __LINE__);
			}

			else if(bits5_7_20_22 == 0x1b)
			{
				if(bits16_19 != 0x0f)
				{
					// SXTAH<c> <Rd>,<Rn>,<Rm>{,<rotation>}
					WriteARMInstruction(dd, kITMisc, "sxtah ...");
					Sprintf(dd.mHelp, "EADasmArm.cpp line %d", __LINE__);
				}
				else
				{
					// SXTH<c> <Rd>,<Rm>{,<rotation>}
					WriteARMInstruction(dd, kITMisc, "sxth ...");
					Sprintf(dd.mHelp, "EADasmArm.cpp line %d", __LINE__);
				}
			}

			else if(bits5_7_20_22 == 0x1d)
			{
				// REV16<c> <Rd>,<Rm>
				WriteARMInstruction(dd, kITMisc, "rev16 ...");
				Sprintf(dd.mHelp, "EADasmArm.cpp line %d", __LINE__);
			}

			else if(bits5_7_20_22 == 0x23)
			{
				if(bits16_19 != 0x0f)
				{
					// UXTAB16<c> <Rd>,<Rn>,<Rm>{,<rotation>}
					WriteARMInstruction(dd, kITMisc, "uxtab16 ...");
					Sprintf(dd.mHelp, "EADasmArm.cpp line %d", __LINE__);
				}
				else
				{
					// UXTB16<c> <Rd>,<Rm>{,<rotation>}
					WriteARMInstruction(dd, kITMisc, "uxtb16 ...");
					Sprintf(dd.mHelp, "EADasmArm.cpp line %d", __LINE__);
				}
			}

			else if(bits5_7_20_22 == 0x31)
			{
				// USAT16<c> <Rd>,#<imm4>,<Rn>
				WriteARMInstruction(dd, kITMisc, "usat16 ...");
				Sprintf(dd.mHelp, "EADasmArm.cpp line %d", __LINE__);
			}

			else if(bits5_7_20_22 == 0x33)
			{
				if(bits16_19 != 0x0f)
				{
					// UXTAB<c> <Rd>,<Rn>,<Rm>{,<rotation>} 
					WriteARMInstruction(dd, kITMisc, "uxtab ...");
					Sprintf(dd.mHelp, "EADasmArm.cpp line %d", __LINE__);
				}
				else
				{
					// UXTB<c> <Rd>,<Rm>{,<rotation>}
					WriteARMInstruction(dd, kITMisc, "uxtb ...");
					Sprintf(dd.mHelp, "EADasmArm.cpp line %d", __LINE__);
				}
			}

			else if(bits5_7_20_22 == 0x39)
			{
				// RBIT<c> <Rd>,<Rm>
				WriteARMInstruction(dd, kITMisc, "rbit ...");
				Sprintf(dd.mHelp, "EADasmArm.cpp line %d", __LINE__);
			}

			else if(bits5_7_20_22 == 0x3b)
			{
				if(bits16_19 != 0x0f)
				{
					// UXTAH<c> <Rd>,<Rn>,<Rm>{,<rotation>}
					WriteARMInstruction(dd, kITMisc, "uxtah ...");
					Sprintf(dd.mHelp, "EADasmArm.cpp line %d", __LINE__);
				}
				else
				{
					// UXTH<c> <Rd>,<Rm>{,<rotation>}
					WriteARMInstruction(dd, kITMisc, "uxth ...");
					Sprintf(dd.mHelp, "EADasmArm.cpp line %d", __LINE__);
				}
			}

			else if(bits5_7_20_22 == 0x3d)
			{
				// REVSH<c> <Rd>,<Rm>
				WriteARMInstruction(dd, kITMisc, "revsh ...");
				Sprintf(dd.mHelp, "EADasmArm.cpp line %d", __LINE__);
			}
		}

		// 10xxx - - - - Signed multiplies
		else if((bits20_24 & 0x18) == 0x10)
		{
			int bits5_7_20_22 = (GetBitRangeUnsigned(in, 20, 22) << 3) | bits5_7; // Two ranges as one integer. It just happens to be useful to do this due to the opcode bit encoding here.
			int bits12_15     = GetBitRangeUnsigned(in, 12, 15);

			if((bits5_7_20_22 & 0x3e) == 0x00)
			{
				if(bits12_15 != 0x0f)
				{
					// SMLAD{X}<c> <Rd>,<Rn>,<Rm>,<Ra>
					WriteARMInstruction(dd, kITMisc, "smlad ...");
					Sprintf(dd.mHelp, "EADasmArm.cpp line %d", __LINE__);
				}
				else
				{
					// SMUAD{X}<c> <Rd>,<Rn>,<Rm>
					WriteARMInstruction(dd, kITMisc, "smuad ...");
					Sprintf(dd.mHelp, "EADasmArm.cpp line %d", __LINE__);
				}
			}

			else if((bits5_7_20_22 & 0x3e) == 0x02)
			{
				if(bits12_15 != 0x0f)
				{
					// SMLSD{X}<c> <Rd>,<Rn>,<Rm>,<Ra>
					WriteARMInstruction(dd, kITMisc, "smlsd ...");
					Sprintf(dd.mHelp, "EADasmArm.cpp line %d", __LINE__);
				}
				else
				{
					// SMUSD{X}<c> <Rd>,<Rn>,<Rm>
					WriteARMInstruction(dd, kITMisc, "smusd ...");
					Sprintf(dd.mHelp, "EADasmArm.cpp line %d", __LINE__);
				}
			}

			else if((bits5_7_20_22 & 0x3e) == 0x20)
			{
				// SMLALD{X}<c> <RdLo>,<RdHi>,<Rn>,<Rm>
				WriteARMInstruction(dd, kITMisc, "smlald ...");
				Sprintf(dd.mHelp, "EADasmArm.cpp line %d", __LINE__);
			}

			else if((bits5_7_20_22 & 0x3e) == 0x22)
			{
				// SMLSLD{X}<c> <RdLo>,<RdHi>,<Rn>,<Rm>
				WriteARMInstruction(dd, kITMisc, "smlsld ...");
				Sprintf(dd.mHelp, "EADasmArm.cpp line %d", __LINE__);
			}

			else if((bits5_7_20_22 & 0x3e) == 0x28)
			{
				if(bits12_15 != 0x0f)
				{
					// SMMLA{R}<c> <Rd>,<Rn>,<Rm>,<Ra>
					WriteARMInstruction(dd, kITMisc, "smmla ...");
					Sprintf(dd.mHelp, "EADasmArm.cpp line %d", __LINE__);
				}
				else
				{
					// SMMUL{R}<c> <Rd>,<Rn>,<Rm>
					WriteARMInstruction(dd, kITMisc, "smmul ...");
					Sprintf(dd.mHelp, "EADasmArm.cpp line %d", __LINE__);
				}
			}

			else if((bits5_7_20_22 & 0x3e) == 0x2e)
			{
				// SMMLS{R}<c> <Rd>,<Rn>,<Rm>,<Ra>
				WriteARMInstruction(dd, kITMisc, "smmls ...");
				Sprintf(dd.mHelp, "EADasmArm.cpp line %d", __LINE__);
			}
		}

		// 11000 000     1111 - Unsigned Sum of Absolute Differences
		// 11000 000 not 1111 - Unsigned Sum of Absolute Differences and Accumulate
		else if((bits20_24 == 0x18) && (bits5_7 == 0))
		{
			int bits12_15 = GetBitRangeUnsigned(in, 12, 15);

			if(bits12_15 == 0x0f)
			{
				// USAD8<c> <Rd>,<Rn>,<Rm>
				WriteARMInstruction(dd, kITMisc, "usad8 ...");
				Sprintf(dd.mHelp, "EADasmArm.cpp line %d", __LINE__);
			}
			else
			{
				// USADA8<c> <Rd>,<Rn>,<Rm>,<Ra>
				WriteARMInstruction(dd, kITMisc, "usada8 ...");
				Sprintf(dd.mHelp, "EADasmArm.cpp line %d", __LINE__);
			}
		}

		// 1101x x10 - - Signed Bit Field Extract
		else if(((bits20_24 & 0x1e) == 0x1a) && ((bits5_7 & 0x03) == 0x02))
		{
			// SBFX<c> <Rd>,<Rn>,#<lsb>,#<width>
			WriteARMInstruction(dd, kITMisc, "sbfx ...");
			Sprintf(dd.mHelp, "EADasmArm.cpp line %d", __LINE__);
		}

		// 1110x x00 - 1111 Bit Field Clear
		// 1110x x00 - not 1111 Bit Field Insert
		else if(((bits20_24 & 0x1e) == 0x1c) && ((bits5_7 & 0x03) == 0x00))
		{
			int bits0_3 = GetBitRangeUnsigned(in, 0, 3);

			if(bits0_3 == 0x0f)
			{
				// BFC<c> <Rd>,#<lsb>,#<width>
				WriteARMInstruction(dd, kITMisc, "bfc ...");
				Sprintf(dd.mHelp, "EADasmArm.cpp line %d", __LINE__);
			}
			else
			{
				// BFI<c> <Rd>,<Rn>,#<lsb>,#<width>
				WriteARMInstruction(dd, kITMisc, "bfi ...");
				Sprintf(dd.mHelp, "EADasmArm.cpp line %d", __LINE__);
			}
		}

		// 1111x x10 - - Unsigned Bit Field Extract
		else if(((bits20_24 & 0x1e) == 0x1e) && ((bits5_7 & 0x03) == 0x02))
		{
			// UBFX<c> <Rd>,<Rn>,#<lsb>,#<width>
			WriteARMInstruction(dd, kITMisc, "ubfx ...");
			Sprintf(dd.mHelp, "EADasmArm.cpp line %d", __LINE__);
		}

		// 11111 111 - - Permanently UNDEFINED
		else if((bits20_24 == 0x1f) && (bits5_7 == 0x07))
		{
			WriteARMInstruction(dd, kITMisc, "<undefined>");
			Sprintf(dd.mHelp, "EADasmArm.cpp line %d", __LINE__);
		}
	}

	// 10x - Branch, branch with link, and block data transfer
	else if((bits25_27 & 6) == 4)
	{
		int bits20_25 = GetBitRangeUnsigned(in, 20, 25);

		// 0000x0 - Store Multiple Decrement After
		if((bits20_25 & 0x3d) == 0x00)
		{
			// STMDA<c> <Rn>{!},<registers>
			WriteARMInstruction(dd, kITMisc, "stmda ...");
			Sprintf(dd.mHelp, "EADasmArm.cpp line %d", __LINE__);
		}

		// 0000x1 - Load Multiple Decrement After
		else if((bits20_25 & 0x3d) == 0x01)
		{
			// LDMDA<c> <Rn>{!},<registers>
			WriteARMInstruction(dd, kITMisc, "ldmda ...");
			Sprintf(dd.mHelp, "EADasmArm.cpp line %d", __LINE__);
		}

		// 0010x0 - Store Multiple (Increment After)
		else if((bits20_25 & 0x3d) == 0x08)
		{
			// STM<c> <Rn>{!},<registers>
			WriteARMInstruction(dd, kITMisc, "stm ...");
			Sprintf(dd.mHelp, "EADasmArm.cpp line %d", __LINE__);
		}

		// 001001 - Load Multiple (Increment After)
		else if(bits20_25 == 0x09)
		{
			// LDM<c> <Rn>{!},<registers>
			WriteARMInstruction(dd, kITMisc, "ldm ...");
			Sprintf(dd.mHelp, "EADasmArm.cpp line %d", __LINE__);
		}

		// 001011 not 1101 - Load Multiple (Increment After)
		// 001011     1101 - Pop multiple registers
		else if(bits20_25 == 0x0b)
		{
			int bits16_19 = GetBitRangeUnsigned(in, 16, 19);

			if(bits16_19 == 0x0d)   
			{
				// Pop multiple registers
				// POP<c> <registers>
				// This is merely a specialization of ldm with Rn = sp and W bit being 1.
				WriteARMInstruction(dd, kITMisc, "pop ...");
				Sprintf(dd.mHelp, "EADasmArm.cpp line %d", __LINE__);
			}
			else
			{
				// Load Multiple (Increment After)
				// LDM<c> <Rn>{!},<registers>
				// Same as above.
				WriteARMInstruction(dd, kITMisc, "ldm ...");
				Sprintf(dd.mHelp, "EADasmArm.cpp line %d", __LINE__);
			}
		}

		// 010000 - Store Multiple Decrement Before
		else if(bits20_25 == 0x10)
		{
			// STMDB<c> <Rn>{!},<registers>
			WriteARMInstruction(dd, kITMisc, "stmdb ...");
			Sprintf(dd.mHelp, "EADasmArm.cpp line %d", __LINE__);
		}

		// 010010 - not 1101 - Store Multiple Decrement Before
		// 010010 -     1101 - Push multiple registers
		else if(bits20_25 == 0x12)
		{
			int bits16_19 = GetBitRangeUnsigned(in, 16, 19);

			if(bits16_19 == 0x0d)   
			{
				// Push multiple registers
				// PUSH<c> <registers>
				// PUSH<c> <register> (different opcode encoding)
				WriteARMInstruction(dd, kITMisc, "push ...");
				Sprintf(dd.mHelp, "EADasmArm.cpp line %d", __LINE__);
			}
			else
			{
				// Store Multiple Decrement Before
				// Same as above.
				WriteARMInstruction(dd, kITMisc, "stmdb ...");
				Sprintf(dd.mHelp, "EADasmArm.cpp line %d", __LINE__);
			}
		}

		// 0100x1 - Load Multiple Decrement Before
		else if((bits20_25 & 0x3d) == 0x11)
		{
			// LDMDB<c> <Rn>{!},<registers>
			WriteARMInstruction(dd, kITMisc, "ldmdb ...");
			Sprintf(dd.mHelp, "EADasmArm.cpp line %d", __LINE__);
		}

		// 0110x0 - Store Multiple Increment Before
		else if((bits20_25 & 0x3d) == 0x18)
		{
			// STMIB<c> <Rn>{!},<registers>
			WriteARMInstruction(dd, kITMisc, "stmib ...");
			Sprintf(dd.mHelp, "EADasmArm.cpp line %d", __LINE__);
		}

		// 0110x1 - Load Multiple Increment Before
		else if((bits20_25 & 0x3d) == 0x19)
		{
			// LDMIB<c> <Rn>{!},<registers>
			WriteARMInstruction(dd, kITMisc, "ldmib ...");
			Sprintf(dd.mHelp, "EADasmArm.cpp line %d", __LINE__);
		}

		// 0xx1x0 - Store Multiple (user registers)
		else if((bits20_25 & 0x25) == 0x04)
		{
			// STM{amode}<c> <Rn>,<registers>
			WriteARMInstruction(dd, kITMisc, "stm ...");
			Sprintf(dd.mHelp, "EADasmArm.cpp line %d", __LINE__);
		}

		// 0xx1x1 0 Load Multiple (user registers)
		// 0xx1x1 1 Load Multiple (exception return)
		else if((bits20_25 & 0x25) == 0x05)
		{
			if(!GetBitBool(in, 15))
			{
				// LDM{<amode>}<c> <Rn>,<registers_without_pc>
				// Load Multiple (user registers) is UNPREDICTABLE in User or System modes. It's for exception modes.
				WriteARMInstruction(dd, kITMisc, "ldm (exception mode) ...");
				Sprintf(dd.mHelp, "EADasmArm.cpp line %d", __LINE__);
			}
			else
			{
				// LDM{<amode>}<c> <Rn>{!},<registers_with_pc>
				// Load Multiple (exception return) loads multiple registers from consecutive memory locations using an address from a base register.
				WriteARMInstruction(dd, kITMisc, "ldm (exception return) ...");
				Sprintf(dd.mHelp, "EADasmArm.cpp line %d", __LINE__);
			}
		}

		// 10xxxx - Branch
		else if((bits20_25 & 0x30) == 0x20)
		{
			// B<c> <label>
			WriteARMInstruction(dd, kITMisc, "b ...");
			Sprintf(dd.mHelp, "EADasmArm.cpp line %d", __LINE__);
		}

		// 11xxxx - Branch with Link
		else if((bits20_25 & 0x30) == 0x30)
		{
			// BL<c> <label>
			// BLX <label>
			if(condition != 0x0f)
			{
				WriteARMInstruction(dd, kITMisc, "bl ...");
				Sprintf(dd.mHelp, "EADasmArm.cpp line %d", __LINE__);
			}
			else
			{
				WriteARMInstruction(dd, kITMisc, "blx ...");
				Sprintf(dd.mHelp, "EADasmArm.cpp line %d", __LINE__);
			}
		}
	}

	// 11x - Supervisor Call, and coprocessor instructions
	else if((bits25_27 & 6) == 6)
	{
		int   bits20_25 = GetBitRangeUnsigned(in, 20, 25);
		int   bits8_11  = GetBitRangeUnsigned(in, 8, 11);
		bool  b101x     = ((bits8_11 & 0x0e) == 0x0a);    // See the docs for this "101x" pattern.
		bool  b000x0x   = ((bits20_25 & 0x3a) == 0x00);   // See the docs. Actually we could make the logic below work without calculating this. But then the logic wouldn't follow the same top-down flow as the documentation.
		int   bit4Raw   = GetBitRaw(in, 4);               // "raw" because it isn't shifted down to make a numerical value.

		// 0xxxxx (but not 000x0x) - 101x - Advanced SIMD, VFP
		if(((bits20_25 & 0x20) == 0x00) && !b000x0x && b101x)
		{
			// Advanced SIMD, VFP Extension register load/store instructions. Section A7-26.
			WriteARMInstruction(dd, kITMisc, "simd/vfp ...");
			Sprintf(dd.mHelp, "EADasmArm.cpp line %d", __LINE__);
		}

		// 0xxxx0 (but not 000x0x) - not 101x - Store Coprocessor
		else if(((bits20_25 & 0x20) == 0x00) && !b000x0x && !b101x)
		{
			// Store Coprocessor STC, STC2
			// STC{L}<c> <coproc>,<CRd>,[<Rn>,#+/-<imm>]{!}
			// STC{L}<c> <coproc>,<CRd>,[<Rn>],#+/-<imm>
			// STC{L}<c> <coproc>,<CRd>,[<Rn>],<option>
			WriteARMInstruction(dd, kITMisc, "stc/stc2 ...");
			Sprintf(dd.mHelp, "EADasmArm.cpp line %d", __LINE__);
		}

		// 0xxxx1 (but not 000x0x) - not 101x not 1111 Load Coprocessor LDC, LDC2 (immediate)
		// 0xxxx1 (but not 000x0x) - not 101x     1111 Load Coprocessor LDC, LDC2 (literal)
		else if(((bits20_25 & 0x21) == 0x00) && !b000x0x && !b101x)
		{
			int  bits16_19 = GetBitRangeUnsigned(in, 16, 19);

			if(bits16_19 != 0x0f)
			{
				// Load Coprocessor LDC, LDC2 (immediate)
				// LDC{L}<c> <coproc>,<CRd>,[<Rn>,#+/-<imm>]{!}
				// LDC{L}<c> <coproc>,<CRd>,[<Rn>],#+/-<imm>
				// LDC{L}<c> <coproc>,<CRd>,[<Rn>],<option>
				WriteARMInstruction(dd, kITMisc, "ldc/ldc2 ...");
				Sprintf(dd.mHelp, "EADasmArm.cpp line %d", __LINE__);
			}
			else
			{
				// Load Coprocessor LDC, LDC2 (literal)
				// LDC{L}<c> <coproc>,<CRd>,<label>
				// LDC{L}<c> <coproc>,<CRd>,[PC,#-0] Special case
				// LDC{L}<c> <coproc>,<CRd>,[PC],<option>
				WriteARMInstruction(dd, kITMisc, "ldc/ldc2 ...");
				Sprintf(dd.mHelp, "EADasmArm.cpp line %d", __LINE__);
			}
		}

		// 00000x - - - UNDEFINED
		else if((bits20_25 & 0x3e) == 0x3e)
		{
			// Undefined
			//WriteARMInstruction(dd, kITMisc, "<undefined>");
			//Sprintf(dd.mHelp, "EADasmArm.cpp line %d", __LINE__);
		}

		// 00010x - 101x - Advanced SIMD, VFP. 64-bit transfers between ARM core and extension registers
		else if(((bits20_25 & 0x3e) == 0x04) && b101x)
		{
			// (this is a set of a number of V functions). Section A7-32.
			WriteARMInstruction(dd, kITMisc, "simd/vfp ...");
			Sprintf(dd.mHelp, "EADasmArm.cpp line %d", __LINE__);
		}

		// 000100 - not 101x - Move to Coprocessor from two ARM core registers
		else if((bits20_25 == 0x04) && !b101x)
		{
			// MCRR<c> <coproc>,<opc1>,<Rt>,<Rt2>,<CRm>
			WriteARMInstruction(dd, kITMisc, "mcrr/mcrr2 ...");
			Sprintf(dd.mHelp, "EADasmArm.cpp line %d", __LINE__);
		}

		// 000101 - not 101x - Move to two ARM core registers from Coprocessor
		else if((bits20_25 == 0x05) && !b101x)
		{
			// MRRC<c> <coproc>,<opc>,<Rt>,<Rt2>,<CRm>
			WriteARMInstruction(dd, kITMisc, "mrrc/mrrc2 ...");
			Sprintf(dd.mHelp, "EADasmArm.cpp line %d", __LINE__);
		}

		// 10xxxx 0 101x - - VFP data-processing instructions
		else if(((bits20_25 & 0x30) == 0x20) && b101x)
		{
			// (this is a set of a number of V functions)
			WriteARMInstruction(dd, kITMisc, "vfp ...");
			Sprintf(dd.mHelp, "EADasmArm.cpp line %d", __LINE__);
		}

		// 10xxxx 0 not 101x - Coprocessor data operations
		else if(((bits20_25 & 0x30) == 0x20) && !bit4Raw && !b101x)
		{
			// CDP<c> <coproc>,<opc1>,<CRd>,<CRn>,<CRm>,<opc2>
			WriteARMInstruction(dd, kITMisc, "cdp/cdp2 ...");
			Sprintf(dd.mHelp, "EADasmArm.cpp line %d", __LINE__);
		}

		// 10xxxx 1 101x - Advanced SIMD, VFP
		else if(((bits20_25 & 0x30) == 0x20) && bit4Raw && b101x)
		{
			// 8, 16, and 32-bit transfer between ARM core and extension registers. Section A7-31.
			// (this is a set of a number of V functions)
			WriteARMInstruction(dd, kITMisc, "simd/vfp ...");
			Sprintf(dd.mHelp, "EADasmArm.cpp line %d", __LINE__);
		}

		// 10xxx0 1 not 101x - Move to Coprocessor from ARM core register
		else if(((bits20_25 & 0x31) == 0x20) && bit4Raw && !b101x)
		{
			// MCR<c> <coproc>,<opc1>,<Rt>,<CRn>,<CRm>{,<opc2>}
			WriteARMInstruction(dd, kITMisc, "mcr/mcr2 ...");
			Sprintf(dd.mHelp, "EADasmArm.cpp line %d", __LINE__);
		}

		// 10xxx1 1 not 101x - Move to ARM core register from Coprocessor
		else if(((bits20_25 & 0x31) == 0x21) && bit4Raw && !b101x)
		{
			// MRC<c> <coproc>,<opc1>,<Rt>,<CRn>,<CRm>{,<opc2>}
			WriteARMInstruction(dd, kITMisc, "mrc/mrc2 ...");
			Sprintf(dd.mHelp, "EADasmArm.cpp line %d", __LINE__);
		}

		// 11xxxx - - - Supervisor Call
		else if((bits20_25 & 0x30) == 0x30)
		{
			// Supervisor Call SVC
			// SVC<c> #<imm24>
			WriteARMInstruction(dd, kITMisc, "svc ...");
			Sprintf(dd.mHelp, "EADasmArm.cpp line %d", __LINE__);
		}
	}

	return 1; // One 32 bit instruction read.
}



///////////////////////////////////////////////////////////////////////////////
// DasmARM
//
int DasmARM(uint32_t in, DasmData& dd)
{
	using namespace EA::StdC;

	dd.mInstructionType  = kITInvalid;
	dd.mInstructionFlags = kIFARM;
	dd.mOperation[0]     = 0;
	dd.mOperands[0]      = 0;

	int condition = GetBitRangeUnsigned(in, 28, 31); // Top four bits.

	if(condition == 0xf) // 1111 - Unconditional instructions
		DasmARMUnconditional(in, dd);
	else
		DasmARMConditional(in, dd);

	if((dd.mOperation[0] == 0) || Stristr(dd.mOperation, "invalid"))
	{
		dd.mInstructionType   = kITInvalid;
		dd.mInstructionFlags |= kIFInvalid;
		Strcpy(dd.mOperation, "<invalid>");
		Strcpy(dd.mHelp, "Invalid instruction as of ARM7.");
	}

	return 1;  //one 32 bit instruction was read.
}



///////////////////////////////////////////////////////////////////////////////
// DetectThumbInstructions
//
// Returns true if instructions appear to be ARM Thumb instructions (16 bit 
// compressed ARM instruction set) as opposed to regular 32 bit ARM instructions.
//
static bool DetectThumbInstructions(const void* pData, const void* pDataEnd)
{
	// I think the biggest giveaway to 32 bit ARM instructions is that 
	// a large number of them have the bit pattern 1110 (i.e. 0xe) as
	// their top four bits. This is because those bits are the condition
	// bits and most instruction execution is unconditional and bit pattern
	// 1110 means 'execute this instruction unconditionally'.
	// One case where this will break down is a string of a large number of
	// vector math opcodes which don't use the conditional bits.

	const uint32_t* pData32    = (const uint32_t*)pData;
	const uint32_t* pDataEnd32 = (const uint32_t*)pDataEnd;

	if((pData32 + 4) < pDataEnd32) // If we have at least a few instructions to sample...
	{
		const size_t kMaxCount = 64;

		if(pDataEnd32 > (pData32 + kMaxCount))
		   pDataEnd32 = (pData32 + kMaxCount);

		DasmData     dd;
		const size_t instructionCount = (size_t)(pDataEnd32 - pData32);
		size_t       foundCount = 0; // Count of uint32_t values with 0xe as their top half byte (four bits).

		while(pData32 < pDataEnd32)
		{
			if((*pData32 & 0xf0000000) == 0xe0000000) // If the top four bits are 1110
				foundCount++;
			pData32++;
		}

		return (foundCount < (instructionCount / 4));
	}

	return true;
}


} // namespace ARM






///////////////////////////////////////////////////////////////////////////////
// DisassemblerARM
///////////////////////////////////////////////////////////////////////////////

///////////////////////////////////////////////////////////////////////////////
// DisassemblerARM
//
DisassemblerARM::DisassemblerARM()
{
	// Empty
}


///////////////////////////////////////////////////////////////////////////////
// ~DisassemblerARM
//
DisassemblerARM::~DisassemblerARM()
{
	// Empty
}


///////////////////////////////////////////////////////////////////////////////
// Dasm
//
const void* DisassemblerARM::Dasm(const void* pData, const void* pDataEnd, 
								  DasmData& dd, uint32_t optionFlags, uint64_t dataAddress)
{
	using namespace EA::StdC;

	EA_ASSERT(pData != NULL);

	dd.ClearLastInstruction();

	dd.mOptionFlags   |= optionFlags;
	dd.mProcessorType  = kProcessorARM;

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
		EA::StdC::Sprintf(dd.mAddress, "%016I64x", (uint64_t)dd.mInstructionAddress);
	else
		EA::StdC::Sprintf(dd.mAddress, "%08I32x", (uint32_t)dd.mInstructionAddress);

	// If the current address has been previously logged as being data within code...
	if(dd.mData.HasUint32((uint32_t)dd.mInstructionAddress)) // If this was previously flagged as 
	{
		uint32_t valueUint32 = *(uint32_t*)pData;

		Sprintf(dd.mInstruction, "%08x", (uint32_t)(uintptr_t)pData);
		Sprintf(dd.mOperation, "word");
		if(valueUint32 > 0x00100000) // If it's probably a negative value...
			Sprintf(dd.mOperands, "%d", (int32_t)valueUint32);
		else
			Sprintf(dd.mOperands, "%u", valueUint32);
		Sprintf(dd.mHelp, "This is a numerical const stored in code, used by earlier code.");

		const uint32_t* pData32 = (const uint32_t*)pData + 1;
		pData = pData32;
	}
	else
	{
		// Print the instruction (operation and operands).
		bool bThumbInstructions;

		if(dd.mOptionFlags & kOFThumb)        // If it's to be forced as ARM Thumb code...
			bThumbInstructions = true;
		else if(dd.mOptionFlags & kOFARM)     // If it's to be forced as ARM code...
			bThumbInstructions = false;
		else
			bThumbInstructions = ARM::DetectThumbInstructions(pData, pDataEnd);

		if(bThumbInstructions)
		{
			const uint16_t* pData16    = (const uint16_t*)pData;
			const uint16_t* pDataEnd16 = (const uint16_t*)pDataEnd;

			if((pData16 + 1) > pDataEnd16) // If there isn't space for one instruction...
			{
				dd.mInstructionFlags = kIFInvalid;
				return pDataEnd;
			}

			uint16_t in16, in16_2;
			memcpy(&in16, pData16, sizeof(in16)); // memcpy just in case the data isn't aligned.
			if((pData16 + 1) < pDataEnd16)
				memcpy(&in16_2, pData16 + 1, sizeof(in16));
			else
			{   // We have only one uint16.
				// If in16 is a 32 bit instruction, then we have an error because we need more data than provided.
				if(ARM::ThumbInstructionIs32Bit(in16))
				{
					dd.mInstructionFlags = kIFInvalid;
					return pDataEnd;
				}

				in16_2 = 0;
			}

			#ifdef EA_SYSTEM_BIG_ENDIAN
				// ARM instructions are natively little-endian. So if we are running on 
				// a big endian system then we need to byte-swap it.
				in16 = (uint16_t)((in16 << 8) | (in16 >> 8));
			#endif

			int instructionCount = ARM::DasmThumb(in16, in16_2, dd);

			if(instructionCount == 1)
				Sprintf(dd.mInstruction, "%04x", pData16[0]);
			else
				Sprintf(dd.mInstruction, "%04x %04x", pData16[0], pData16[1]);

			pData16 += instructionCount;
			pData = pData16;
		}
		else
		{
			const uint32_t* pData32    = (const uint32_t*)pData;
			const uint32_t* pDataEnd32 = (const uint32_t*)pDataEnd;

			if((pData32 + 1) > pDataEnd32) // If there isn't space for one instruction...
			{
				dd.mInstructionFlags = kIFInvalid;
				return pDataEnd;
			}

			uint32_t in32;
			memcpy(&in32, pData32, sizeof(in32));

			#ifdef EA_SYSTEM_BIG_ENDIAN
				// ARM instructions are natively little-endian. So if we are running on 
				// a big endian system then we need to byte-swap it.
				in32 = (uint32_t)((in32 >> 24)               |
								 ((in32 << 24) & 0xff000000) |
								 ((in32 <<  8) & 0x00ff0000) |
								 ((in32 >>  8) & 0x0000ff00));
			#endif

			int instructionCount = ARM::DasmARM(in32, dd);

			Sprintf(dd.mInstruction, "%08x", *pData32);

			pData32 += instructionCount;
			pData = pData32;
		}
	}

	// If the given instuction appears to be a function prologue, clear dd.mData. 
	// This is because usually code data for a function is stored at the end of
	// the function. So a new function would not need the data from the old function.
	// Prologue usually has this: "add sp, ..." and/or "push { ...lr }"
	if(((Strcmp(dd.mOperation, "add") == 0) && (Strstr(dd.mOperands, "sp, ") == dd.mOperands)) ||
	   ((Strcmp(dd.mOperation, "push") == 0) && Strstr(dd.mOperands, "lr }")))
	{
		dd.mData.clear();
	}

	return pData;
}



} // namespace Dasm

} // namespace EA


#endif // EACALLSTACK_DASM_ARM_ENABLED

