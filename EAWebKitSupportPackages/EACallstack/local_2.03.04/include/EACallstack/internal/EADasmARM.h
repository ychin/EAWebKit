/////////////////////////////////////////////////////////////////////////////
// Copyright (c) Electronic Arts Inc. All rights reserved.
/////////////////////////////////////////////////////////////////////////////

#ifndef EACALLSTACK_EADASMARM_H
#define EACALLSTACK_EADASMARM_H


#include <EAStdC/EAString.h>
#include <EAStdC/EASprintf.h>
#include <stdlib.h>
#include <string.h>
#include <stdio.h>
#include <limits.h>


namespace EA
{
namespace Dasm
{
namespace ARM
{


// Typedefs, forward declarations
typedef uint32_t arm_word;
typedef uint16_t thumb_word;



enum Sign
{
	kSignNone     = 0,
	kSignPositive = 1,
	kSignNegative = 2
};


///////////////////////////////////////////////////////////////////////////////
// Register
//
enum Register
{
	kReg0,
	kReg1,
	kReg2,
	kReg3,
	kReg4,
	kReg5,
	kReg6,
	kReg7,
	kReg8,
	kReg9,
	kReg10,
	kRegSL = kReg10,
	kReg11,
	kRegFP = kReg11,
	kReg12,
	kRegIP = kReg12,
	kReg13,
	kRegSP = kReg13,
	kReg14,
	kRegLR = kReg14,
	kReg15,
	kRegPC = kReg15
};


enum Condition
{
	kCondEq, // 0000    Equal; Executed if Z flag == 1.
	kCondNe, // 0001    Not equal; Executed if Z flag == 0.
	kCondCs, // 0010    Carry set (there was a bit carry); Executed if C flag == 1.
	kCondCc, // 0011    Carry clear (no bit carry); Executed if C flag == 0.
	kCondMi, // 0100    Minus, negative; Executed if N flag == 1.
	kCondPl, // 0101    Plus, positive or zero; Executed if N flag == 0.
	kCondVs, // 0110    Overflow; Executed if V flag == 1.
	kCondVc, // 0111    No overflow; Executed if V flag == 0.
	kCondHi, // 1000    Unsigned higher; Executed if flags: C == 1 and Z == 0.
	kCondLs, // 1001    Unsigned lower or same; Executed if flags: C == 0 or Z == 1.
	kCondGe, // 1010    Signed greater than or equal; flags: N == V.
	kCondLt, // 1011    Signed less than; Executed if flags: N != V.
	kCondGt, // 1100    Signed greater than; Executed if flags: Z == 0 and N == V.
	kCondLe, // 1101    Signed less than or equal; Executed if flags: Z == 1 or N != V
	kCondAl, // 1110    Always; unconditional
	kCondIn  // 1111    invalid for use as a condition.
};


enum ShiftType
{
	kSRType_LSL = 0,    // Logical shift left by N bits.     Encoding is 0b00.
	kSRType_LSR = 1,    // Logical shift right by N bits.    Encoding is 0b01.
	kSRType_ASR = 2,    // Arithmetic shift right by N bits. Encoding is 0b10.
	kSRType_ROR = 3,    // Rotate right by N bits.           Encoding is 0b11.

	kSRType_MOV = 10,   // Logical shift left by N bits.     Encoding is 0b00.
	kSRType_RRX = 13    // Rotate right "with extend". Bit [0] is written to shifter_carry_out, bits [31:1] are shifted right one bit, and the Carry Flag is shifted into bit [31]. Encoding is 0b11 and immediate is 0.
};


enum AMode
{
	kAModeDA = 0x00, // DA Decrement After. ARM code only. The consecutive memory addresses end at the address in the base register. Encoded as P = 0, U = 0 in encoding A1.
	kAModeIA = 0x01, // The consecutive memory addresses start at the address in the base register. This is the default, and is normally omitted. Encoding T2, or encoding A1 with P = 0, U = 1.
	kAModeDB = 0x02, // The consecutive memory addresses end one word below the address in the base register. Encoding T1, or encoding A1 with P = 1, U = 0.
	kAModeIB = 0x03  // Increment Before. ARM code only. The consecutive memory addresses start one word above the address in the base register. Encoded as P = 1, U = 1 in encoding A1.
};


// Shared data
extern const char* kConditionStr[16];
extern const char* kConditionIntegerHelpStr[16];
extern const char* kRegisterName[16];



// Shared functions
int  DasmARM(uint32_t in, DasmData& dd);
int  DasmThumb(uint16_t in, uint16_t in2, DasmData& dd);

void WriteARMInstruction(DasmData& dd, InstructionType it, const char* pInstructionName);
void WriteARMHintInstruction(DasmData& dd, int condition, int identifier);
void WriteArmBarrierInstruction(DasmData& dd, int instruction, int option);

const char* GetARMRegisterName(int regIndex);
const char* GetARMRegisterName(uint32_t in, int rStartBit, int rEndBit);
void        GetARMRegisterName(char* sRegister, int regIndex);
int         GetARMRegisterName(uint32_t in, char* sRegister, int rStartBit, int rEndBit);
const char* GetConditionStr(uint32_t in);
const char* GetConditionHelpStrInt(uint32_t in);


} // namespace ARM
} // namespace Dasm
} // namespace EA




#endif // EACALLSTACK_EADASMARM_H
