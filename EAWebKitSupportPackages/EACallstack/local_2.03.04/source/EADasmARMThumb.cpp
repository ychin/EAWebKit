///////////////////////////////////////////////////////////////////////////////
// EADasmARMThumb.cpp
//
// Copyright (c) 2011, Electronic Arts. All rights reserved.
// Maintained by Paul Pedriana.
///////////////////////////////////////////////////////////////////////////////


#include <EACallstack/EADasm.h>

#if EACALLSTACK_DASM_ARM_ENABLED

#include <EACallstack/internal/EADasmARM.h>


namespace EA
{

namespace Dasm
{

namespace ARM
{



///////////////////////////////////////////////////////////////////////////////
// SignExtend
//
static inline int SignExtend(int n32, int highBit)
{
	n32 <<= (31 - highBit);
	n32 >>= (31 - highBit);    // This assumes that bits are propogated, but they always are on our CPUs.

	return n32;
}


///////////////////////////////////////////////////////////////////////////////
// GetBitRangeSigned
//
// Example:
//     GetBitRangeSigned(0b0000111100001111, 2, 4)
//     returns 0b11 treated as signed two bits, which happens to be -1.
//
static inline int GetBitRangeSigned(uint16_t n16, int lowBit, int highBit)
{
	// This is a little tricky. We need to treat a sub-range of bits within n16 as signed. 
	// The C/C++ language doesn't directly support doing this, but we can accomplish it anyway.
	int16_t s16 = (int16_t)n16;
	s16 <<= (15 - highBit);
	s16 >>= (15 - highBit) + lowBit;    // This shift assumes that the compiler does an "arithmetic" (a.k.a. signed) right shift, which propogates a top-most 1 bit downward if present. All modern compiler/CPU combinations do so.

	return s16;
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
static inline int GetBitRangeUnsigned(uint16_t n16, int lowBit, int highBit)
{
	n16 <<= (15 - highBit);
	n16 >>= (15 - highBit) + lowBit;

	return n16;
}


// Returns the bit as 0 or 1.
static inline int GetBit(uint16_t n16, int bit)
{
	const int value = (n16 & (1 << bit)) >> bit;

	return value;
}


// "raw" because it isn't shifted down to make a numerical value. 
static inline int GetBitRaw(uint16_t n16, int bit)
{
	return (n16 & (1 << bit));
}

// Returns true or false.
static inline bool GetBitBool(uint16_t n16, int bit)
{
	const int value = (n16 & (1 << bit));

	return (value != 0);
}



///////////////////////////////////////////////////////////////////////////////
// WriteThumbInstruction (no arguments)
//
// Writes an instruction such as:
//    nop
//
static void WriteThumbInstruction(DasmData& dd, InstructionType it, uint16_t /*in*/, const char* pInstructionName)
{
	using namespace EA::StdC;

	dd.mInstructionType = it;
	Strcpy(dd.mOperation, pInstructionName);
	dd.mOperands[0] = 0;
}


///////////////////////////////////////////////////////////////////////////////
// WriteThumbInstructionI (Immediate)
//
// Writes an instruction such as:
//    swi 28
//    bkpt 2
//
static void WriteThumbInstructionI(DasmData& dd, InstructionType it, uint16_t in, const char* pInstructionName, 
								 int& immediate, int immediateLowBit, int immediateHighBit)
{
	using namespace EA::StdC;

	dd.mInstructionType = it;
	Strcpy(dd.mOperation, pInstructionName);

	immediate = GetBitRangeUnsigned(in, immediateLowBit, immediateHighBit); // Immediate is some constant integer, usually an offset or a numerical shift/add value.
	Snprintf(dd.mOperands, EAArrayCount(dd.mOperands), "%d", immediate);
}


///////////////////////////////////////////////////////////////////////////////
// WriteThumbInstructionB (Branch)
//
// Writes an instruction such as:
//    bne #28 (back to address 04387008)
//
static void WriteThumbInstructionB(DasmData& dd, InstructionType it, uint16_t in, const char* pInstructionName, 
								 int& offset, int offsetLowBit, int offsetHighBit)
{
	using namespace EA::StdC;

	dd.mInstructionType = it;
	Strcpy(dd.mOperation, pInstructionName);

	offset = (GetBitRangeSigned(in, offsetLowBit, offsetHighBit) << 1);  // Shift by 1 because values are uint16_t-aligned. 
	int32_t pcOffset = 4 + offset;                                       // Add 4 because all PC-relative offsets are PC+4, because the way the ARM CPU works, by the time this instruction is actually executing, the PC has moved to two instuctions later due to pipeline pre-fetch.
	Snprintf(dd.mOperands, EAArrayCount(dd.mOperands), "#%d (%s to %08x)", offset, (pcOffset < 0) ? "back" : "forward", (uint32_t)((int64_t)dd.mInstructionAddress + pcOffset));
}




///////////////////////////////////////////////////////////////////////////////
// WriteThumbInstructionXI (string, Immediate)
//
// Writes an instruction such as:
//    add sp, #48
//
static void WriteThumbInstructionXI(DasmData& dd, InstructionType it, uint16_t in, const char* pInstructionName, 
								 const char* pRegisterName,  
								 int& immediate, int immediateLowBit, int immediateHighBit,
								 int immediateShift)    // Sometimes the encoded opvalue is shifted to the right because its low bits are unimportant. This arg is so we can unshift it.
{
	using namespace EA::StdC;

	dd.mInstructionType = it;
	Strcpy(dd.mOperation, pInstructionName);

	immediate = GetBitRangeUnsigned(in, immediateLowBit, immediateHighBit); // Immediate is some constant integer, usually an offset or a numerical shift/add value.
	Snprintf(dd.mOperands, EAArrayCount(dd.mOperands), "%s, #%d", pRegisterName, immediate << immediateShift);
}


///////////////////////////////////////////////////////////////////////////////
// WriteThumbInstructionRI (Register, Immediate)
//
// Writes an instruction such as:
//    mov r4, #86
//
static void WriteThumbInstructionRI(DasmData& dd, InstructionType it, uint16_t in, const char* pInstructionName, 
								 int& regDest, int regDestLowBit, int regDestHighBit, 
								 int& immediate, int immediateLowBit, int immediateHighBit,
								 int immediateShift)    // Sometimes the encoded opvalue is shifted to the right because its low bits are unimportant. This arg is so we can unshift it.
{
	using namespace EA::StdC;

	dd.mInstructionType = it;
	Strcpy(dd.mOperation, pInstructionName);

	regDest = GetBitRangeUnsigned(in, regDestLowBit, regDestHighBit);
	char regDestName[12];
	GetARMRegisterName(regDestName, regDest);

	immediate = GetBitRangeUnsigned(in, immediateLowBit, immediateHighBit); // Immediate is some constant integer, usually an offset or a numerical shift/add value.
	Snprintf(dd.mOperands, EAArrayCount(dd.mOperands), "%s, #%d", regDestName, immediate << immediateShift);
}


///////////////////////////////////////////////////////////////////////////////
// WriteThumbInstructionRXI (Register, string, Immediate)
//
// Writes an instruction such as:
//    ldr r2, [pc, #8]
//    add r4, pc, #24
//
static void WriteThumbInstructionRXI(DasmData& dd, InstructionType it, uint16_t in, const char* pInstructionName, 
								 int& regDest, int regDestLowBit, int regDestHighBit,
								 const char* pRegisterName,  
								 int& immediate, int immediateLowBit, int immediateHighBit,
								 int immediateShift, bool bPrintBraces)    // Sometimes the encoded opvalue is shifted to the right because its low bits are unimportant. This arg is so we can unshift it.
{
	using namespace EA::StdC;

	dd.mInstructionType = it;
	Strcpy(dd.mOperation, pInstructionName);

	regDest = GetBitRangeUnsigned(in, regDestLowBit, regDestHighBit);
	char regDestName[12];
	GetARMRegisterName(regDestName, regDest);

	immediate = GetBitRangeUnsigned(in, immediateLowBit, immediateHighBit); // Immediate is some constant integer, usually an offset or a numerical shift/add value.
	if(bPrintBraces) // Braces means that the value in the braces is a pointer which is dereferenced.
		Snprintf(dd.mOperands, EAArrayCount(dd.mOperands), "%s, [%s, #%d]", regDestName, pRegisterName, immediate << immediateShift);
	else
		Snprintf(dd.mOperands, EAArrayCount(dd.mOperands), "%s, %s, #%d", regDestName, pRegisterName, immediate << immediateShift);
}


///////////////////////////////////////////////////////////////////////////////
// WriteThumbInstructionRRI (Register, Register, Immediate)
//
// Writes an instruction such as:
//    lsl r7, r6, #12
//
static void WriteThumbInstructionRRI(DasmData& dd, InstructionType it, uint16_t in, const char* pInstructionName, 
								 bool bBracketsAroundLastTwoArgs,                               // The brackets mean that the two items are added together to form a value.
								 int& regDest, int regDestLowBit, int regDestHighBit, 
								 int& regSource, int regSourceLowBit, int regSourceHighBit,
								 int& immediate, int immediateLowBit, int immediateHighBit,
								 int immediateShift, int immediateMultiplier) // immediateMultiplier is -1 or 1.
{
	using namespace EA::StdC;

	dd.mInstructionType = it;
	Strcpy(dd.mOperation, pInstructionName);

	regDest = GetBitRangeUnsigned(in, regDestLowBit, regDestHighBit);
	char regDestName[12];
	GetARMRegisterName(regDestName, regDest);

	regSource = GetBitRangeUnsigned(in, regSourceLowBit, regSourceHighBit);
	char regSourceName[12];
	GetARMRegisterName(regSourceName, regSource);

	immediate = GetBitRangeUnsigned(in, immediateLowBit, immediateHighBit); // Immediate is some constant integer, usually an offset or a numerical shift/add value.
	Snprintf(dd.mOperands, EAArrayCount(dd.mOperands), bBracketsAroundLastTwoArgs ? "%s, [%s, #%d]" : "%s, %s, #%d", regDestName, regSourceName, (immediate << immediateShift) * immediateMultiplier);
}


///////////////////////////////////////////////////////////////////////////////
// WriteThumbInstructionR (Register)
//
// Writes an instruction such as:
//    bx r6
//    bx lr
//
static void WriteThumbInstructionR(DasmData& dd, InstructionType it, uint16_t in, const char* pInstructionName, 
								int& regDest, int regDestLowBit, int regDestHighBit, 
								int regDestAddition) // regDestAddition will be either 0 or 8. Some of the instructions mean that a 0-3 encoding doesn't mean the usual register 0-7 but register 8-15.
{
	using namespace EA::StdC;

	dd.mInstructionType = it;
	Strcpy(dd.mOperation, pInstructionName);

	char regDestName[12];
	regDest = GetBitRangeUnsigned(in, regDestLowBit, regDestHighBit);
	GetARMRegisterName(regDestName, regDest + regDestAddition);

	Snprintf(dd.mOperands, EAArrayCount(dd.mOperands), "%s", regDestName);
}


///////////////////////////////////////////////////////////////////////////////
// WriteThumbInstructionRR (Register, Register)
//
// Writes an instruction such as:
//    add r5, r6
//
static void WriteThumbInstructionRR(DasmData& dd, InstructionType it, uint16_t in, const char* pInstructionName, 
								 int& regDest, int regDestLowBit, int regDestHighBit, 
								 int& regArg, int regArgLowBit, int regArgHighBit, 
								 int regDestAddition, int regArgAddition)
{
	using namespace EA::StdC;

	dd.mInstructionType = it;
	Strcpy(dd.mOperation, pInstructionName);

	char regDestName[12];
	regDest = GetBitRangeUnsigned(in, regDestLowBit, regDestHighBit) + regDestAddition;
	GetARMRegisterName(regDestName, regDest);

	char regArgName[12];
	regArg = GetBitRangeUnsigned(in, regArgLowBit, regArgHighBit) + regArgAddition;
	GetARMRegisterName(regArgName, regArg);

	Snprintf(dd.mOperands, EAArrayCount(dd.mOperands), "%s, %s", regDestName, regArgName);
}


///////////////////////////////////////////////////////////////////////////////
// WriteThumbInstructionRRR (Register, Register, Register)
//
// Writes an instruction such as:
//    sub r5, r6, r5
//
static void WriteThumbInstructionRRR(DasmData& dd, InstructionType it, uint16_t in, const char* pInstructionName, 
								 bool bBracketsAroundLastTwoArgs,                               // The brackets mean that the two items are added together to form a value.
								 int& regDest, int regDestLowBit, int regDestHighBit, 
								 int& regSource, int regSourceLowBit, int regSourceHighBit,
								 int& regArg, int regArgLowBit, int regArgHighBit)
{
	using namespace EA::StdC;

	dd.mInstructionType = it;
	Strcpy(dd.mOperation, pInstructionName);

	char regDestName[12];
	regDest = GetBitRangeUnsigned(in, regDestLowBit, regDestHighBit);
	GetARMRegisterName(regDestName, regDest);

	char regSourceName[12];
	regSource = GetBitRangeUnsigned(in, regSourceLowBit, regSourceHighBit);
	GetARMRegisterName(regSourceName, regSource);

	char regArgName[12];
	regArg = GetBitRangeUnsigned(in, regArgLowBit, regArgHighBit);
	GetARMRegisterName(regArgName, regArg);

	Snprintf(dd.mOperands, EAArrayCount(dd.mOperands), bBracketsAroundLastTwoArgs ? "%s, [%s, %s]" : "%s, %s, %s", regDestName, regSourceName, regArgName);
}


///////////////////////////////////////////////////////////////////////////////
// GetARMRegisterNameList
//
// Writes a name list such as:
//    { r1, r3, r6 }
//    { r6, r8, r6, lr }
//
// regNameList must hold at least 64 chars
//
void GetARMRegisterNameList(char* regNameList, uint16_t in, int regListLowBit, int regListHighBit, const char* pAdditionalRegister)
{
	// To do: The ARM assembler recognizes registers specified in the form 
	//        of ranges, like { r3-r7 }. We should detect this and print like that.
	using namespace EA::StdC;

	Strcpy(regNameList, "{ ");

	int listCount = 0;
	for(int bit = regListLowBit; bit <= regListHighBit; bit++)
	{
		if(in & (1 << bit))
		{
			char regName[8];
			if(listCount > 0) // If this not the first one...
				Strcat(regNameList, ", ");
			GetARMRegisterName(regName, bit - regListLowBit);
			Strcat(regNameList, regName);
			listCount++;
		}
	}

	if(pAdditionalRegister)
	{
		if(listCount > 0) // If this not the first one...
			Strcat(regNameList, ", ");
		Strcat(regNameList, pAdditionalRegister); // pAdditionalRegister is usually "lr" or "pc"
	}

	Strcat(regNameList, " }");
}


///////////////////////////////////////////////////////////////////////////////
// WriteThumbInstructionRlist (Register list)
//
// Writes an instruction such as:
//    push { r4, r5, r8 }
//    pop { r6, r8, r6, pc }
//
static void WriteThumbInstructionRlist(DasmData& dd, InstructionType it, uint16_t in, const char* pInstructionName, 
									 int regListLowBit, int regListHighBit, const char* pAdditionalRegister) // pAdditionalRegister is always NULL, "PC", or "LR".
{
	using namespace EA::StdC;

	dd.mInstructionType = it;
	Strcpy(dd.mOperation, pInstructionName);

	char regNameList[64];
	GetARMRegisterNameList(regNameList, in, regListLowBit, regListHighBit, pAdditionalRegister);
	Strcpy(dd.mOperands, regNameList);
}


///////////////////////////////////////////////////////////////////////////////
// WriteThumbInstructionMia (special case for stmia/ldmia)
//
// Writes an instruction such as:
//    stmia r3! {r5, r6, r7 }
//    ldmia r3! {r5, r6, r7 }
//
static void WriteThumbInstructionMia(DasmData& dd, InstructionType it, uint16_t in, const char* pInstructionName,
									int& regArg, int regArgLowBit, int regArgHighBit, int regListLowBit, int regListHighBit)
{
	using namespace EA::StdC;

	dd.mInstructionType = it;
	Strcpy(dd.mOperation, pInstructionName);

	char regArgName[12];
	regArg = GetBitRangeUnsigned(in, regArgLowBit, regArgHighBit);
	GetARMRegisterName(regArgName, regArg);

	char regNameList[64];
	GetARMRegisterNameList(regNameList, in, regListLowBit, regListHighBit, NULL);
	Snprintf(dd.mOperands, EAArrayCount(dd.mOperands), "%s!, %s", regArgName, regNameList); // The exclamation point means that this register points to the beginning of an array.
}



///////////////////////////////////////////////////////////////////////////////
// DasmThumb32
//
// You need to read the ARM architecture manual "32-bit Thumb instruction 
// encoding" section to grok this.
//
static void DasmThumb32(thumb_word in, thumb_word in2, DasmData& dd)
{
	using namespace EA::StdC;

	int bits11_12 = GetBitRangeUnsigned(in, 11, 12);
	int bits4_10  = GetBitRangeUnsigned(in, 4, 10);

	//(bits11_12 == 0) is a 16 bit instruction and so handled by DasmThumb().
	if(bits11_12 == 1)
	{
		if((bits4_10 & 0x64) == 0x00) // Bit pattern 00xx0xx -- Load Multiple and Store Multiple
		{
			Sprintf(dd.mHelp, "EADasmArmThumb.cpp line %d", __LINE__);
		}
		else if((bits4_10 & 0x64) == 0x04) // Bit pattern 00xx1xx - Load/store dual or exclusive, table branch
		{
			Sprintf(dd.mHelp, "EADasmArmThumb.cpp line %d", __LINE__);
		}
		else if((bits4_10 & 0x60) == 0x20) // Bit pattern 01xxxxx - Data processing (shifted register)
		{
			Sprintf(dd.mHelp, "EADasmArmThumb.cpp line %d", __LINE__);
		}
		else // Bit pattern 1xxxxxx - Coprocessor instructions
		{
			Sprintf(dd.mHelp, "EADasmArmThumb.cpp line %d", __LINE__);
		}
		// The above should cover all cases.
	}
	else if(bits11_12 == 2)
	{
		int op = GetBit(in2, 15);  // The ARM docs call this bit "op".

		if(((bits4_10 & 0x20) == 0x00) && (op == 0)) // Bit pattern x0xxxxx -- Data processing (modified immediate)
		{
			Sprintf(dd.mHelp, "EADasmArmThumb.cpp line %d", __LINE__);
		}
		else if(((bits4_10 & 0x20) == 0x20) && (op == 0)) // Bit pattern x1xxxxx - Data processing (plain binary immediate)
		{
			Sprintf(dd.mHelp, "EADasmArmThumb.cpp line %d", __LINE__);
		}
		else if(op == 1)  // Bit pattern xxxxxxx - Branches and miscellaneous control
		{
			int bits2_12_14 = GetBitRangeUnsigned(in2, 12, 14);

			if((bits2_12_14 & 0x05) == 0x00)
			{
				switch (bits4_10)
				{
					case 0x38: // 0111000 Move to Special Register, application level / system level - MSR (register)
					case 0x39: // 0111001
					{
						int bits2_8_11 = GetBitRangeUnsigned(in2, 8, 11);

						if((bits2_8_11 & 0x03) == 0x00) // xx00
						{
							// Move to Special Register, application level
							// MSR<c> <spec_reg>,<Rn>
							// page A8-210
							Sprintf(dd.mHelp, "EADasmArmThumb.cpp line %d", __LINE__);
						}
						else // xx01 / xx1x
						{
							// Move to Special Register, system level
							// MSR<c> <spec_reg>,<Rn>
							// page B6-14
							Sprintf(dd.mHelp, "EADasmArmThumb.cpp line %d", __LINE__);
						}
						break;
					}

					case 0x3a: // 0111010 Change Processor State, and hints
					{
						int bits2_8_10 = GetBitRangeUnsigned(in2, 8, 10);

						if(bits2_8_10 != 0x00) // Change Processor State CPS
						{
							// CPS<effect> <iflags>{,#<mode>}
							// CPS #<mode>
							// See B1.3.1 ARM processor modes
							WriteARMInstruction(dd, kITMisc, "cps ...");
							Sprintf(dd.mHelp, "EADasmArm.cpp line %d", __LINE__);
						}
						else
						{
							int bits0_7 = GetBitRangeUnsigned(in, 0, 7);
							WriteARMHintInstruction(dd, kCondAl, bits0_7);
						}

						break;
					}

					case 0x3b: // 0111011 Miscellaneous control instructions
					{
						int bits2_4_7 = GetBitRangeUnsigned(in2, 4, 7);

						switch (bits2_4_7)
						{
							case 0x00: // Leave ThumbEE state a ENTERX, LEAVEX
							case 0x01: // Enter ThumbEE state ENTERX, LEAVEX
								WriteARMInstruction(dd, kITMisc, GetBitRaw(in2, 4) ? "enterx" : "leavex");
								Strcpy(dd.mHelp, "Enters or leaves ThumbEE state.");
								break;

							case 0x02: // Clear-Exclusive CLREX
								WriteARMInstruction(dd, kITMisc, "clrex");
								Strcpy(dd.mHelp, "Clears the local record of the executing processor that an address has had a request for an exclusive access.");
								break;

							case 0x04: // Data Synchronization Barrier DSB
							case 0x05: // Data Memory Barrier DMB
							case 0x06: // Instruction Synchronization Barrier ISB
							{
								int option = GetBitRangeUnsigned(in2, 0, 3);
								WriteArmBarrierInstruction(dd, bits2_4_7, option);
								break;
							}
						}
					}

					case 0x3c: // 0111100 Branch and Exchange Jazelle BXJ
					{
						// BXJ <Rm>
						WriteARMInstruction(dd, kITMisc, "bxj");
						Snprintf(dd.mOperands, EAArrayCount(dd.mOperands), "%s", GetARMRegisterName(in, 0, 3));
						Strcpy(dd.mHelp, "Changes to Jazelle state. If that fails, it branches to an address and instruction set specified by the register as though it were a BX instruction.");
						break;                                     
					}

					case 0x3d: // 0111101 Exception Return SUBS PC, LR and related instructions
					{
						// SUBS<c> PC,LR,#<imm8>
						WriteARMInstruction(dd, kITMisc, "subs ...");
						Sprintf(dd.mHelp, "EADasmArm.cpp line %d", __LINE__);
						break;
					}

					case 0x3e: // 0111110 Move from Special Register, application level - MRS
					case 0x3f: // 0111111 Move from Special Register, system level - MRS
					{
						WriteARMInstruction(dd, kITMisc, "mrs ...");
						Sprintf(dd.mHelp, "EADasmArm.cpp line %d", __LINE__);
						break;
					}

					default:   // not x111xxx - Conditional branch - B
					{                       
						// Encoding T3
						// B<c>.W <label> Not permitted in IT (if/then) block.
						// if cond<3:1> == '111' then SEE "Related encodings";
						// imm32 = SignExtend(S:J2:J1:imm6:imm11:'0', 32);
						int condition  = GetBitRangeUnsigned(in, 6, 9);
						int bits2_0_10 = GetBitRangeUnsigned(in2, 0, 10);
						int bits0_5    = GetBitRangeUnsigned(in, 0, 5);     // The calculation below is one of the most ridiculous I've had to do in a while. ARM isn't always simple.
						int imm32      = (GetBit(in, 10) << 20) | (GetBit(in, 11) << 19) | (GetBit(in, 13) << 18) | (bits0_5 << 12) | (bits2_0_10 << 1);  // 21 bits of total information. –1048576 to 1048574
						imm32 = SignExtend(imm32, 20) + 4; // Add 4 because ARM PC-relative values are always +4 due to how the pipeline reads PC as it goes.

						dd.mInstructionType = kITBranch;
						Sprintf(dd.mOperation, "b%s", kConditionStr[condition]);
						Sprintf(dd.mOperands, "#%d (%s to %08x)", imm32, (imm32 < 0) ? "back" : "forward", (uint32_t)((int64_t)dd.mInstructionAddress + imm32));
						Sprintf(dd.mHelp, "Branch to [pc + 4 + %d]", imm32 - 4);

						break;
					}
				}
			}
			else if(bits2_12_14 == 0x00)
			{
				// Secure Monitor Call SMC (previously SMI)
				WriteARMInstruction(dd, kITMisc, "smc ...");
				Sprintf(dd.mHelp, "EADasmArm.cpp line %d", __LINE__);
			}
			else if(bits2_12_14 == 0x02)
			{
				// Permanently UNDEFINED. This space will not be allocated in future.
			}

			// Branch - B
			// Branch with Link and Exchange BL, BLX (bit 12 == 0)
			// Branch with Link                      (bit 12 == 1)
			else if(((bits2_12_14 & 0x05) == 0x01) || (bits2_12_14 & 0x04))
			{
				int S     = GetBit(in, 10);
				int J1    = GetBit(in2, 13);
				int I1    = !(J1 ^ S);          // This is strange, but it's what the architecture docs say to do.
				int J2    = GetBit(in2, 11);
				int I2    = !(J2 ^ S);
				int imm10 = GetBitRangeUnsigned(in, 0, 9);
				int imm11 = GetBitRangeUnsigned(in2, 0, 10);
				int imm32 = (S << 24) | (I2 << 23) | (I1 << 22) | (imm10 << 12) | (imm11 << 1);    // 25 bits of total information. –16777216 to 16777214
				imm32 = SignExtend(imm32, 24) + 4; // Add 4 because ARM PC-relative values are always +4 due to how the pipeline reads PC as it goes.

				dd.mInstructionType = kITBranch;

				if((bits2_12_14 & 0x05) == 0x01)
				{
					// Encoding T4
					// B<c>.W <label> Outside or last in IT (if/then) block
					// I1 = NOT(J1 EOR S); I2 = NOT(J2 EOR S);
					// imm32 = SignExtend(S:I1:I2:imm10:imm11:’0’, 32);
					Sprintf(dd.mOperation, "b.n");
					Sprintf(dd.mHelp, "Branch to [pc + 4 + %d]. This is just a jump and not a function call.", imm32 - 4);
				}
				else if(GetBitRaw(in2, 12))
				{
					// Encoding T1
					// BL<c> <label>
					// I1 = NOT(J1 EOR S); I2 = NOT(J2 EOR S);
					// imm32 = SignExtend(S:I1:I2:imm10:imm11:’0’, 32);
					// Branch with Link calls a subroutine at a PC-relative address.
					// Functions are usually retuned by the caller with "BX LR instruction" or similar.
					Sprintf(dd.mOperation, "bl");
					Sprintf(dd.mHelp, "Call subroutine at [pc + 4 + %d]. lr is set to instruction after this one.", imm32 - 4);
				}
				else
				{
					// Encoding T2
					// BLX<c> <label>
					// I1 = NOT(J1 EOR S); I2 = NOT(J2 EOR S);
					// imm32 = SignExtend(S:I1:I2:imm10H:imm10L:’00’, 32);
					// Branch with Link and Exchange Instruction Sets (immediate) calls a subroutine at a PC-relative address, and changes instruction set from ARM to Thumb, or from Thumb to ARM.
					imm32 &= ~3; // Needed because imm32 is slightly different here than the above case.
					Sprintf(dd.mOperation, "blx");
					Sprintf(dd.mHelp, "Call subroutine at [pc + 4 + %d]. Switch from Thumb to ARM state. lr is set to instruction after this one.", imm32 - 4);
				}

				Sprintf(dd.mOperands, "#%d (call func at %08x)", imm32, (uint32_t)((int64_t)dd.mInstructionAddress + imm32)); // We have to put this after the above code because imm32 can be modified above.
			}
		}
		// Fall through to error if the above didn't catch the bit pattern as a valid instruction.
	}
	else if(bits11_12 == 3)
	{
		if((bits4_10 & 0x71) == 0x00) // Bit pattern 000xxx0 -- Store single data item
		{
			Sprintf(dd.mHelp, "EADasmArmThumb.cpp line %d", __LINE__);
		}
		else if((bits4_10 & 0x67) == 0x01) // Bit pattern 00xx001 - Load byte, memory hints
		{
			Sprintf(dd.mHelp, "EADasmArmThumb.cpp line %d", __LINE__);
		}
		else if((bits4_10 & 0x67) == 0x03) // Bit pattern 00xx011 - Load halfword, memory hints
		{
			Sprintf(dd.mHelp, "EADasmArmThumb.cpp line %d", __LINE__);
		}
		else if((bits4_10 & 0x67) == 0x05) // Bit pattern 00xx101 - Load word
		{
			Sprintf(dd.mHelp, "EADasmArmThumb.cpp line %d", __LINE__);
		}
		else if((bits4_10 & 0x67) == 0x07) // Bit pattern 00xx111 - Undefined
		{
			// Undefined
		}
		else if((bits4_10 & 0x70) == 0x20) // Bit pattern 010xxxx - Data processing (register)
		{
			Sprintf(dd.mHelp, "EADasmArmThumb.cpp line %d", __LINE__);
		}
		else if((bits4_10 & 0x78) == 0x30) // Bit pattern 0110xxx - Multiply, multiply accumulate, and absolute difference
		{
			Sprintf(dd.mHelp, "EADasmArmThumb.cpp line %d", __LINE__);
		}
		else if((bits4_10 & 0x78) == 0x38) // Bit pattern 0111xxx - Long multiply, long multiply accumulate, and divide
		{
			Sprintf(dd.mHelp, "EADasmArmThumb.cpp line %d", __LINE__);
		}
		else if((bits4_10 & 0x40) == 0x40) // Bit pattern 1xxxxxx - Coprocessor instructions
		{
			int coprocessorOp1 = GetBitRangeUnsigned(in, 4, 9);
			int coprocessorOp  = GetBit(in2, 4);

			if(coprocessorOp1 == 0x04) // 000100 - Move to Coprocessor from two ARM core registers
			{
				Sprintf(dd.mHelp, "EADasmArmThumb.cpp line %d", __LINE__);
			}
			else if(coprocessorOp1 == 0x05) // 000101 - Move to two ARM core registers from Coprocessor
			{
				Sprintf(dd.mHelp, "EADasmArmThumb.cpp line %d", __LINE__);
			}
			else if((coprocessorOp1 & 0x21) == 0) // 0xxxx0 - Store Coprocessor
			{
				Sprintf(dd.mHelp, "EADasmArmThumb.cpp line %d", __LINE__);
			}
			else if(coprocessorOp) // 10xxxx -- Coprocessor data operations
			{
				Sprintf(dd.mHelp, "EADasmArmThumb.cpp line %d", __LINE__);
			}
			else if(coprocessorOp) // 10xxx0 - Move to Coprocessor from ARM core register
			{
				Sprintf(dd.mHelp, "EADasmArmThumb.cpp line %d", __LINE__);
			}
			else if(coprocessorOp) // 10xxx1 - Move to ARM core register from Coprocessor
			{
				Sprintf(dd.mHelp, "EADasmArmThumb.cpp line %d", __LINE__);
			}
			// Fall through to error if the above didn't catch the bit pattern as a valid instruction.
		}
		// Fall through to error if the above didn't catch the bit pattern as a valid instruction.
	}
}


///////////////////////////////////////////////////////////////////////////////
// DasmThumb
//
// Returns the number of instructions read, which will be 1 or 2.
//
// This was originally based off the information in the ARM document: 
//     ARM7-TDMI-manual-pt3.pdf, table 5-1
//     ARM Architecture v7m Reference Manual
//     ARMv7-AR Architecture Reference Manual (this is a superset of all documents)
// 
// See one of these ARM documents for explanations of terms below, such as "format 1".
// If you want to understand this code, it's pretty easy if you have table 5-1.
// The documents above are all redundant, but some provide information in an
// easier to read way than others.
//
int DasmThumb(thumb_word in, thumb_word in2, DasmData& dd)
{
	using namespace EA::StdC;

	int instructionCount = 1; // This is 1 unless this happens to be a 32 bit instruction, in which case it will be 2.

	dd.mInstructionType  = kITInvalid;
	dd.mInstructionFlags = kIFThumb;
	dd.mOperation[0]     = 0;
	dd.mOperands[0]      = 0;

	int  regSource = 0;
	int  regDest   = 0;
	int  regArg    = 0;
	int  offset    = 0;
	int  value     = 0;
	int  bits13_15 = GetBitRangeUnsigned(in, 13, 15); // Top three bits.

	// The method we use is to do switches on bit groups starting from highest to lowest.
	// We could achieve a more compact implementation by using some fairly complex tables and 
	// function pointers, but the implementation would be much harder for a reader to understand.
	switch(bits13_15)
	{
		case 0:             // Move shifted register | add/subtract
		{
			int bits11_12 = GetBitRangeUnsigned(in, 11, 12);

			switch(bits11_12)
			{                   
				case 0:         // Format 1: move shifted register. LSL Rd, Rs, #Offset5. Shift Rs left by a 5-bit immediate value and store the result in Rd.
					WriteThumbInstructionRRI(dd, kITImmediate, in, "lsls", false, regDest, 0, 2, regSource, 3, 5, offset, 6, 10, 0, 1);
					if(offset == 0)
						Snprintf(dd.mHelp, EAArrayCount(dd.mHelp), "(mov r%d r%d). r%d = r%d << %d.", regDest, regSource, regDest, regSource, offset);
					else
						Snprintf(dd.mHelp, EAArrayCount(dd.mHelp), "r%d = r%d << %d. r%d = r%d * %d", regDest, regSource, offset, regDest, regSource, 1 << offset);
					break;

				case 1:         // Format 1: move shifted register. LSR Rd, Rs, #Offset5. Perform logical shift right on Rs by a 5-bit immediate value and store the result in Rd.
					WriteThumbInstructionRRI(dd, kITImmediate, in, "lsr", false, regDest, 0, 2, regSource, 3, 5, offset, 6, 10, 0, 1);
					if(offset == 0)
						Snprintf(dd.mHelp, EAArrayCount(dd.mHelp), "(mov r%d r%d). r%d = r%d >> %d.", regDest, regSource, regDest, regSource, offset);
					else
						Snprintf(dd.mHelp, EAArrayCount(dd.mHelp), "r%d = r%d >> %d. r%d = r%d / %d. Unsigned shift right / divide by power of 2.", regDest, regSource, offset, regDest, regSource, 1 << offset);
					break;

				case 2:         // Format 1: move shifted register
					WriteThumbInstructionRRI(dd, kITImmediate, in, "asrs", false, regDest, 0, 2, regSource, 3, 5, offset, 6, 10, 0, 1);
					if(offset == 0)
						Snprintf(dd.mHelp, EAArrayCount(dd.mHelp), "(mov r%d r%d). r%d = r%d >> %d.", regDest, regSource, regDest, regSource, offset);
					else
						Snprintf(dd.mHelp, EAArrayCount(dd.mHelp), "r%d = r%d >> %d. r%d = r%d / %d. Signed shift right / divide by power of 2.", regDest, regSource, offset, regDest, regSource, 1 << offset);
					break;

				case 3:         // Format 2: add/subtract
				{
					bool bRegister = GetBitBool(in, 10); // register (0), or immediate (1).
					bool bAdd      = GetBitBool(in, 9);  // add (0), or subtract (1).

					if(!bRegister)
					{
						if(!bAdd)
						{
							// ADD Rd, Rs, Rn. Add contents of Rn to contents of Rs. Place result in Rd.
							WriteThumbInstructionRRR(dd, kITImmediate, in, "adds", false, regDest, 0, 2, regSource, 3, 5, regArg, 6, 8);
							Snprintf(dd.mHelp, EAArrayCount(dd.mHelp), "r%d = r%d + r%d", regDest, regSource, regArg);
						}
						else
						{
							// SUB Rd, Rs, Rn. Subtract contents of Rn from contents of Rs. Place result in Rd.
							WriteThumbInstructionRRR(dd, kITImmediate, in, "subs", false, regDest, 0, 2, regSource, 3, 5, regArg, 6, 8);
							Snprintf(dd.mHelp, EAArrayCount(dd.mHelp), "r%d = r%d - r%d", regDest, regSource, regArg);
						}
					}
					else // else immediate
					{
						if(!bAdd)
						{
							// ADD Rd, Rs, #Offset3. Add 3-bit immediate value to contents of Rs. Place result in Rd.
							WriteThumbInstructionRRI(dd, kITImmediate, in, "adds", false, regDest, 0, 2, regSource, 3, 5, value, 6, 8, 0, 1);
							if(value == 0)
								Snprintf(dd.mHelp, EAArrayCount(dd.mHelp), "(mov r%d r%d). r%d = r%d + %d", regDest, regSource, regDest, regSource, value);
							else
								Snprintf(dd.mHelp, EAArrayCount(dd.mHelp), "r%d = r%d + %d", regDest, regSource, value);
						}
						else
						{
							// SUB Rd, Rs, #Offset3. Subtract 3-bit immediate value from contents of Rs. Place result in Rd.
							WriteThumbInstructionRRI(dd, kITImmediate, in, "adds", false, regDest, 0, 2, regSource, 3, 5, value, 6, 8, 0, -1);
							if(value == 0)
								Snprintf(dd.mHelp, EAArrayCount(dd.mHelp), "(mov r%d r%d). r%d = r%d - %d", regDest, regSource, regDest, regSource, value);
							else
								Snprintf(dd.mHelp, EAArrayCount(dd.mHelp), " r%d = r%d - %d", regDest, regSource, value);
						}
					}
					break;
				}
			}

			break;
		}
		case 1:             // Move / compare / add / subtract immediate
		{
			int operation = GetBitRangeUnsigned(in, 11, 12);

			switch (operation)
			{
				case 0:         // MOV Rd, #Offset8. Move 8-bit immediate value into Rd.
					WriteThumbInstructionRI(dd, kITImmediate, in, "movs", regDest, 8, 10, value, 0, 7, 0);
					Snprintf(dd.mHelp, EAArrayCount(dd.mHelp), "r%d = %d", regDest, value);
					break;

				case 1:         // CMP Rd, #Offset8. Compare contents of Rd with 8-bit immediate value.
					WriteThumbInstructionRI(dd, kITImmediate, in, "cmp", regDest, 8, 10, value, 0, 7, 0);
					Snprintf(dd.mHelp, EAArrayCount(dd.mHelp), "Compare r%d to %d. Set cspr register with results.", regDest, value);
					break;

				case 2:         // ADD Rd, #Offset8. Add 8-bit immediate value to contents of Rd and place the result in Rd.
					WriteThumbInstructionRI(dd, kITImmediate, in, "adds", regDest, 8, 10, value, 0, 7, 0);
					Snprintf(dd.mHelp, EAArrayCount(dd.mHelp), "r%d += %d.", regDest, value);
					break;

				case 3:         // SUB Rd, #Offset8. Subtract 8-bit immediate value from contents of Rd and place the result in Rd.
					WriteThumbInstructionRI(dd, kITImmediate, in, "subs", regDest, 8, 10, value, 0, 7, 0);
					Snprintf(dd.mHelp, EAArrayCount(dd.mHelp), "r%d -= %d.", regDest, value);
					break;
			}

			break;
		}
		case 2:             // ALU operations | Hi register operations/branch exchange | PC-relative load | Load/store with register offset | Load/store sign-extended byte/halfword
		{
			int bits10_12 = GetBitRangeUnsigned(in, 10, 12);
			int bits11_12 = GetBitRangeUnsigned(in, 11, 12);

			if(bits10_12 == 0) // ALU operations
			{
				int         operation = GetBitRangeUnsigned(in, 6, 9);
				const char* pOperationNames[16] = { "ands", "eors", "lsls", "lsr", "asrs", "adcs", "sbcs", "rors", "tst", "negs", "cmp", "cmn", "orrs", "muls", "bics", "mvns" }; 
				const char* pOperationHelpFormat[16] = { "r%d = r%d & r%d", "r%d = r%d ^ r%d", "r%d = r%d << r%d", "r%d = r%d >> r%d (unsigned shift)", "r%d = r%d >> r%d (signed shift)", "r%d = r%d + r%d + C-bit", "r%d = r%d - r%d - NOT C-bit", "r%d = r%d ROR r%d (rotate right)", 
														 "Set condition codes on r%d AND r%d", "r%d = -r%d", "Set condition codes on r%d - r%d", "Set condition codes on r%d + r%d", 
														 "r%d = r%d | r%d", "r%d = r%d * r%d", "r%d = r%d AND NOT r%d", "r%d = NOT r%d" }; 

				WriteThumbInstructionRR(dd, kITImmediate, in, pOperationNames[operation], regDest, 0, 2, regSource, 3, 5, 0, 0);

				if((operation >= 8) && (operation <= 11))
					Snprintf(dd.mHelp, EAArrayCount(dd.mHelp), pOperationHelpFormat[operation], regDest, regSource);
				else
					Snprintf(dd.mHelp, EAArrayCount(dd.mHelp), pOperationHelpFormat[operation], regDest, regDest, regSource);
			}
			else if(bits10_12 == 1) // Hi register operations/branch exchange
			{
				int operation = GetBitRangeUnsigned(in, 6, 9);

				switch (operation)
				{
					case 0:
						// Invalid. Fall through to invalid handling below.
						break;

					case 1:             // ADD Rd, Hs. Add a register in the range r8-r15 to a register in the range r0-r7. If either or both of the operands is R8-R15, ADD not ADDS.
						WriteThumbInstructionRR(dd, kITImmediate, in, "add", regDest, 0, 2, regSource, 3, 5, 0, 8);
						Snprintf(dd.mHelp, EAArrayCount(dd.mHelp), "r%d = r%d + r%d", regDest, regDest, regSource );
						break;

					case 2:             // ADD Hd, Rs. Add a register in the range r0-r7 to a register in the range r8-r15. If either or both of the operands is R8-R15, ADD not ADDS.
						WriteThumbInstructionRR(dd, kITImmediate, in, "add", regDest, 0, 2, regSource, 3, 5, 8, 0);
						Snprintf(dd.mHelp, EAArrayCount(dd.mHelp), "r%d = r%d + r%d", regDest, regDest, regSource);
						break;

					case 3:             // ADD Hd, Hs. Add two registers in the range r8-r15. If either or both of the operands is R8-R15, ADD not ADDS.
						WriteThumbInstructionRR(dd, kITImmediate, in, "add", regDest, 0, 2, regSource, 3, 5, 8, 8);
						Snprintf(dd.mHelp, EAArrayCount(dd.mHelp), "r%d = r%d + r%d", regDest, regDest, regSource);
						break;

					case 4:
						// Invalid. Fall through to invalid handling below.
						break;

					case 5:             // CMP Rd, Hs. Compare a register in the range 0-7 with a register in the range 8-15. Set the condition code flags on the result.
						WriteThumbInstructionRR(dd, kITImmediate, in, "cmp", regDest, 0, 2, regSource, 3, 5, 0, 8);
						Snprintf(dd.mHelp, EAArrayCount(dd.mHelp), "Compare r%d to r%d. Set condition flags on the result.", regDest, regSource);
						break;

					case 6:             // CMP Hd, Rs. Compare a register in the range 8-15 with a register in the range 0-7. Set the condition code flags on the result.
						WriteThumbInstructionRR(dd, kITImmediate, in, "cmp", regDest, 0, 2, regSource, 3, 5, 8, 0);
						Snprintf(dd.mHelp, EAArrayCount(dd.mHelp), "Compare r%d to r%d. Set the condition code flags on the result.", regDest, regSource);
						break;

					case 7:             // CMP Hd, Hs. Compare two registers in the range 8-15. Set the condition code flags on the result.
						WriteThumbInstructionRR(dd, kITImmediate, in, "cmp", regDest, 0, 2, regSource, 3, 5, 8, 8);
						Snprintf(dd.mHelp, EAArrayCount(dd.mHelp), "Compare r%d to r%d. Set the condition code flags on the result.", regDest, regSource);
						break;

					case 8:
						// Invalid. Fall through to invalid handling below.
						break;

					case 9:             // MOV Rd, Hs. Move a value from a register in the range 8-15 to a register in the range 0-7. If either or both of the operands is R8-R15, MOV <Rd>,<Rn> not ADDS <Rd>,<Rn>,#0.
						WriteThumbInstructionRR(dd, kITImmediate, in, "mov", regDest, 0, 2, regSource, 3, 5, 0, 8);
						Snprintf(dd.mHelp, EAArrayCount(dd.mHelp), "r%d = r%d", regDest, regSource);
						break;

					case 10:            // MOV Hd, Rs. Move a value from a register in the range 0-7 to a register in the range 8-15. If either or both of the operands is R8-R15, MOV <Rd>,<Rn> not ADDS <Rd>,<Rn>,#0.
						WriteThumbInstructionRR(dd, kITImmediate, in, "mov", regDest, 0, 2, regSource, 3, 5, 8, 0);
						Snprintf(dd.mHelp, EAArrayCount(dd.mHelp), "r%d = r%d", regDest, regSource);
						break;

					case 11:            // MOV Hd, Hs. Move a value between two registers in the range 8-15. If either or both of the operands is R8-R15, MOV <Rd>,<Rn> not ADDS <Rd>,<Rn>,#0.
						WriteThumbInstructionRR(dd, kITImmediate, in, "mov", regDest, 0, 2, regSource, 3, 5, 8, 8);

						if(regDest == regSource) // If this actually is a no-op (which GCC often does)...
						{
							Snprintf(dd.mHelp, EAArrayCount(dd.mHelp), "Encoded as: %s %s", dd.mOperation, dd.mOperands);
							Strcpy(dd.mOperation, "nop");
							dd.mOperands[0] = 0;
						}
						else
							Snprintf(dd.mHelp, EAArrayCount(dd.mHelp), "r%d = r%d", regDest, regSource);
						break;

					case 12:            // BX Rs. Perform branch (plus optional state change) to address in a register in the range 0-7.
					case 13:            // BX Hs. Perform branch (plus optional state change) to address in a register in the range 8-15.
						WriteThumbInstructionR(dd, kITBranch, in, "bx", regDest, 3, 6, 0);
						Snprintf(dd.mHelp, EAArrayCount(dd.mHelp), "Branch (usually function return) to address in register r%d. If bit of the address 0 is 0, destination is ARM code, else Thumb code.", regDest);
						break;

					case 14:
						// Invalid. Fall through to invalid handling below.
						break;

					case 15:
						// Invalid. Fall through to invalid handling below.
						break;
				}
			}
			else if(bits11_12 == 1) // PC-relative load
			{
				// LDR Rd, [PC, #Imm]. Add unsigned offset (255 words, 1020 bytes) in Imm to the current value of the PC. Load the aligned word from the resulting address into Rd.
				WriteThumbInstructionRXI(dd, kITLoad, in, "ldr", regDest, 8, 10, "pc", value, 0, 7, 2, true);
				uint32_t codeAddress = (uint32_t)((int64_t)dd.mInstructionAddress + ((int64_t)value << INT64_C(2)) + 4) & ~3;         // We will be loading a uint32_t from codeAddress.

				if(dd.mOptionFlags & kOFSelf) // If we are disassembling ourselves while we are running...
				{
					uint32_t loadedValue = *(uint32_t*)(uintptr_t)codeAddress;                                         // This is the uint32_t.
					Sprintf(dd.mOperands + Strlen(dd.mOperands), " (%d from %08x)", (int)loadedValue, codeAddress);    // Write it as signed int because that's how it's usually used.
					Snprintf(dd.mHelp, EAArrayCount(dd.mHelp), "r%d = %d. Read aligned int32 (unsigned %u) from [pc + 4 + %d]. Load of large const stored in code.", regDest, (int)loadedValue, loadedValue, value << 2);
				}
				else
				{
					Sprintf(dd.mOperands + Strlen(dd.mOperands), " (from %08x)", codeAddress);
					Snprintf(dd.mHelp, EAArrayCount(dd.mHelp), "r%d = *(%08x). Read aligned int32 from [pc + 4 + %d]. Load of large const stored in code.", regDest, codeAddress, value << 2);
				}

				// We save in dd that the given address refers to data instead of to instructions.
				// This allows us to describe it as such when we get to disassembling that address.
				dd.mData.AddUint32(codeAddress);
			}
			else
			{
				int bits9_11 = GetBitRangeUnsigned(in, 9, 11);

				switch (bits9_11)
				{
					case 0:
						// STR  Rd, [Rb, Ro]. Pre-indexed word store: Calculate the target address by adding together the value in Rb and the value in Ro. Store the contents of Rd at the address.
						WriteThumbInstructionRRR(dd, kITStore, in, "str", true, regDest, 0, 2, regSource, 3, 5, regArg, 6, 8);
						Snprintf(dd.mHelp, EAArrayCount(dd.mHelp), "*(r%d + r%d) = r%d. Write a uint32 to memory.", regSource, regArg, regDest);
						break;

					case 1:
						// STRH Rd, [Rb, Ro]. Store halfword: Add Ro to base address in Rb. Store bits 0-15 of Rd at the resulting address.
						WriteThumbInstructionRRR(dd, kITStore, in, "strh", true, regDest, 0, 2, regSource, 3, 5, regArg, 6, 8);
						Snprintf(dd.mHelp, EAArrayCount(dd.mHelp), "*(r%d + r%d) = (uint16_t)r%d. Write a uint16 to memory.", regSource, regArg, regDest);
						break;

					case 2:
						// STRB Rd, [Rb, Ro]. Pre-indexed byte store: Calculate the target address by adding together the value in Rb and the value in Ro. Store the byte value in Rd at the resulting address.
						WriteThumbInstructionRRR(dd, kITStore, in, "strb", true, regDest, 0, 2, regSource, 3, 5, regArg, 6, 8);
						Snprintf(dd.mHelp, EAArrayCount(dd.mHelp), "*(r%d + r%d) = (uint8_t)r%d. Write a uint8 to memory.", regSource, regArg, regDest);
						break;

					case 3:
						// LDRSB Rd, [Rb, Ro]. Load sign-extended byte: Add Ro to base address in Rb. Load bits 0-7 of Rd from the resulting address, and set bits 8-31 of Rd to bit 7.
						WriteThumbInstructionRRR(dd, kITLoad, in, "ldrsb", true, regDest, 0, 2, regSource, 3, 5, regArg, 6, 8);
						Snprintf(dd.mHelp, EAArrayCount(dd.mHelp), "r%d = *(int8_t*)(r%d + r%d). Sign extend r%d to 32 bits. Read int8 from memory.", regDest, regSource, regArg, regDest);
						break;

					case 4:
						// LDR  Rd, [Rb, Ro]. Pre-indexed word load: Calculate the source address by adding together the value in Rb and the value in Ro. Load the contents of the address into Rd.
						WriteThumbInstructionRRR(dd, kITLoad, in, "ldr", true, regDest, 0, 2, regSource, 3, 5, regArg, 6, 8);
						Snprintf(dd.mHelp, EAArrayCount(dd.mHelp), "r%d = *(uint32_t*)(r%d + r%d). Read uint32 from memory.", regDest, regSource, regArg);
						break;

					case 5:
						// LDRH Rd, [Rb, Ro]. Load halfword: Add Ro to base address in Rb. Load bits 0-15 of Rd from the resulting address, and set bits 16-31 of Rd to 0.
						WriteThumbInstructionRRR(dd, kITLoad, in, "ldrh", true, regDest, 0, 2, regSource, 3, 5, regArg, 6, 8);
						Snprintf(dd.mHelp, EAArrayCount(dd.mHelp), "r%d = *(uint16_t*)(r%d + r%d). Read uint16 from memory.", regDest, regSource, regArg);
						break;

					case 6:
						// LDRB Rd, [Rb, Ro]. Pre-indexed byte load: Calculate the source address by adding together the value in Rb and the value in Ro. Load the byte value at the resulting address.
						WriteThumbInstructionRRR(dd, kITLoad, in, "ldrb", true, regDest, 0, 2, regSource, 3, 5, regArg, 6, 8);
						Snprintf(dd.mHelp, EAArrayCount(dd.mHelp), "r%d = *(uint8_t*)(r%d + r%d). Read uint8 from memory.", regDest, regSource, regArg);
						break;

					case 7:
						// LDRSH Rd, [Rb, Ro]. Load sign-extended halfword: Add Ro to base address in Rb. Load bits 0-15 of Rd from the resulting address, and set bits 16-31 of Rd to bit 15.
						WriteThumbInstructionRRR(dd, kITLoad, in, "ldrsh", true, regDest, 0, 2, regSource, 3, 5, regArg, 6, 8);
						Snprintf(dd.mHelp, EAArrayCount(dd.mHelp), "r%d = *(int16_t*)(r%d + r%d). Sign extend r%d to 32 bits. Read int16 from memory.", regDest, regSource, regArg, regDest);
						break;
				}
			}

			break;
		}
		case 3:             // Load/store with immediate offset
		{
			int bits11_12 = GetBitRangeUnsigned(in, 11, 12);

			if(bits11_12 == 0)
			{
				// STR Rd, [Rb, #Imm]. Calculate the target address by adding together the value in Rb and Imm. Store the contents of Rd at the address.
				WriteThumbInstructionRRI(dd, kITStore, in, "str", true, regDest, 0, 2, regSource, 3, 5, offset, 6, 10, 2, 1);
				Snprintf(dd.mHelp, EAArrayCount(dd.mHelp), "*(r%d + %d) = r%d. Write a uint32 to memory.", regSource, offset, regDest);
			}
			else if(bits11_12 == 1)
			{
				// LDR Rd, [Rb, #Imm]. Calculate the source address by adding together the value in Rb and Imm. Load Rd from the address.
				WriteThumbInstructionRRI(dd, kITLoad, in, "ldr", true, regDest, 0, 2, regSource, 3, 5, offset, 6, 10, 2, 1);
				Snprintf(dd.mHelp, EAArrayCount(dd.mHelp), "r%d = *(uint32_t*)(r%d + %d). Read uint32 from memory.", regDest, regSource, offset);
			}
			else if(bits11_12 == 2)
			{
				// STRB Rd, [Rb, #Imm]. Calculate the target address by adding together the value in Rb and Imm. Store the byte value in Rd at the address.
				WriteThumbInstructionRRI(dd, kITStore, in, "strb", true, regDest, 0, 2, regSource, 3, 5, offset, 6, 10, 0, 1);
				Snprintf(dd.mHelp, EAArrayCount(dd.mHelp), "*(r%d + %d) = (uint8_t)r%d. Write a uint8 to memory.", regSource, offset, regDest);
			}
			else if(bits11_12 == 3)
			{
				// LDRB Rd, [Rb, #Imm]. Calculate source address by adding together the value in Rb and Imm. Load the byte value at the address into Rd.
				WriteThumbInstructionRRI(dd, kITLoad, in, "ldrb", true, regDest, 0, 2, regSource, 3, 5, offset, 6, 10, 0, 1);
				Snprintf(dd.mHelp, EAArrayCount(dd.mHelp), "r%d = *(uint8_t*)(r%d + %d). Read uint8 from memory.", regDest, regSource, offset);
			}

			break;
		}
		case 4:             // Load/store halfword | SP-relative load/store
		{
			int bits11_12 = GetBitRangeUnsigned(in, 11, 12);

			if(bits11_12 == 0)
			{
				// STRH Rd, [Rb, #Imm]. Add #Imm to base address in Rb and store bits 0-15 of Rd at the resulting address.
				WriteThumbInstructionRRI(dd, kITStore, in, "strh", true, regDest, 0, 2, regSource, 3, 5, offset, 6, 10, 1, 1);
				Snprintf(dd.mHelp, EAArrayCount(dd.mHelp), "*(r%d + %d) = (uint16_t)r%d. Write a uint16 to memory.", regSource, offset, regDest);
			}
			else if(bits11_12 == 1)
			{
				// LDRH Rd, [Rb, #Imm]. Add #Imm to base address in Rb. Load bits 0-15 from the resulting address into Rd and set bits 16-31 to zero.
				WriteThumbInstructionRRI(dd, kITLoad, in, "ldrh", true, regDest, 0, 2, regSource, 3, 5, offset, 6, 10, 1, 1);
				Snprintf(dd.mHelp, EAArrayCount(dd.mHelp), "r%d = *(uint16_t*)(r%d + %d). Read uint16 from memory. Top bits of r%d become 0", regDest, regSource, offset, regDest);
			}
			else if(bits11_12 == 2)
			{
				// STR Rd, [SP, #Imm]. Add unsigned offset (255 words, 1020 bytes) in Imm to the current value of the SP (R7). Store the contents of Rd at the resulting address.
				// To consider: write custom output here which uses "sp" in the arguments.
				WriteThumbInstructionRXI(dd, kITStore, in, "str", regDest, 8, 10, "sp", value, 0, 7, 2, true);
				Snprintf(dd.mHelp, EAArrayCount(dd.mHelp), "*(sp + %d) = r%d. Write a uint32 to memory. For writing a local variable or temporarily saving a register's value.", value, regDest);
			}
			else if(bits11_12 == 3)
			{
				// LDR Rd, [SP, #Imm]. Add unsigned offset (255 words, 1020 bytes) in Imm to the current value of the SP (R7). Load the word from the resulting address into Rd.
				// To consider: write custom output here which uses "sp" in the arguments.
				WriteThumbInstructionRXI(dd, kITLoad, in, "ldr", regDest, 8, 10, "sp", value, 0, 7, 2, true);
				Snprintf(dd.mHelp, EAArrayCount(dd.mHelp), "r%d = *(sp + %d). For restoring a temporarily saved register back.", regDest, value << 2);                
			}

			break;
		}
		case 5:             // Load address | Add offset to stack pointer | Push/pop registers | Misc instructions.
		{
			int bit12 = GetBit(in, 12);

			if(bit12 == 0)  // Load address
			{
				int bit11 = GetBit(in, 11); // 0 -> PC, 1 -> SP

				if(bit11 == 0)
				{
					// ADD Rd, PC, #Imm. Add #Imm to the current value of the program counter (PC) and load the result into Rd. If either or both of the operands is R8-R15, ADD not ADDS.
					WriteThumbInstructionRXI(dd, kITImmediate, in, "add", regDest, 8, 10, "pc", offset, 0, 7, 2, false);
					uint32_t codeAddress = (uint32_t)((int64_t)dd.mInstructionAddress + ((int64_t)offset << INT64_C(2)) + 4) & ~3;
					Sprintf(dd.mOperands + Strlen(dd.mOperands), " (%08x)", codeAddress);
					Snprintf(dd.mHelp, EAArrayCount(dd.mHelp), "r%d = 0x%08x. r%d = pc + 4 + %d. Used for reading the address of some code, usually for a future branch to it.", regDest, codeAddress, regDest, offset);
				}
				else
				{
					// ADD Rd, SP, #Imm. Add #Imm to the current value of the stack pointer (SP) and load the result into Rd. If either or both of the operands is R8-R15, ADD not ADDS.
					WriteThumbInstructionRXI(dd, kITImmediate, in, "add", regDest, 8, 10, "sp", offset, 0, 7, 2, false);
					Snprintf(dd.mHelp, EAArrayCount(dd.mHelp), "r%d = sp + %d. Used for reading the address of some variable on the stack.", regDest, offset);
				}
			}
			else
			{
				// This set of instructions (top four bits of the instruction are 1101) are the most diverse and require the most logic to handle.
				// See table A5.2.5 of the ARMv7-M Technical Reference Manual (Misc Thumb instructions). It might be a different table depending on the doc version.
				int bit10 = GetBit(in, 10);

				if(bit10 == 0)  // Add offset to stack pointer | Sign/zero extend | Compare and branch on non-zero.
				{
					int bits8_9 = GetBitRangeUnsigned(in, 8, 9);

					if(bits8_9 == 0) // Add offset to stack pointer
					{
						int bit7 = GetBit(in, 7); // 0 -> offset is positive, 1 -> offset is negative

						if(bit7 == 0)
						{
							// ADD SP, #Imm. Add #Imm to the stack pointer (SP). If either or both of the operands is R8-R15, ADD not ADDS.
							WriteThumbInstructionXI(dd, kITImmediate, in, "add", "sp", value, 0, 6, 2);
							Snprintf(dd.mHelp, EAArrayCount(dd.mHelp), "sp += %d. Undoes the previous creation of local stack space. Usually before returning from a subroutine.", value << 2, value << 2);
						}
						else
						{
							// ADD SP, #-Imm. Add #-Imm to the stack pointer (SP). If the operand register is SP, SUB not SUBS.
							WriteThumbInstructionXI(dd, kITImmediate, in, "sub", "sp", value, 0, 6, 2);
							Snprintf(dd.mHelp, EAArrayCount(dd.mHelp), "sp -= %d. Creates %d bytes of space for this subroutine's local variables. Usually at the start of a subroutine.", value << 2, value << 2);
						}
					}
					else if(bits8_9 == 2) // Sign/zero extend
					{
						// Signed Extend Halfword | Signed Extend Byte | Unsigned Extend Halfword | Unsigned Extend Byte
						int         operation = GetBitRangeUnsigned(in, 6, 7);
						const char* pOperationNames[4] = { "sxth", "sxtb", "uxth", "uxtb" }; 
						const char* pOperationHelpFormat[4]  = 
						{ 
							"Read low uint16 from r%d, sign-extend it to 32 bits, write result to r%d.",
							"Read low uint8 from r%d, sign-extend it to 32 bits, write result to r%d.",
							"Read low uint16 from r%d, 0-extend it to 32 bits, write result to r%d.",
							"Read low uint8 from r%d, 0-extend it to 32 bits, write result to r%d."
						}; 

						WriteThumbInstructionRR(dd, kITImmediate, in, pOperationNames[operation], regDest, 0, 2, regSource, 3, 5, 0, 0);
						Snprintf(dd.mHelp, EAArrayCount(dd.mHelp), pOperationHelpFormat[operation], regSource, regDest);
					}
					else // Compare and branch on non-zero.
					{
						// CBZ <Rn>,<label>
						// This instruction appears to use bit3 3-7 and 9 to form a 6 bit unsigned integer. 
						// This 6 bit value is then shifted left by one because the offset is specified in 
						// 16 bit instructions as opposed to bytes. Offset is always positive.
						dd.mInstructionType = kITBranch;
						Strcpy(dd.mOperation, "cbz");

						regDest = GetBitRangeUnsigned(in, 0, 2);
						offset = (GetBitRangeUnsigned(in, 3, 7) << 1);  // Shift by 1 because values are uint16_t-aligned. 
						if(GetBit(in, 9))
							offset |= 0x40; // add in 0b01000000
						int32_t pcOffset = 4 + offset;                // Add 4 because all PC-relative offsets are PC+4, because the way the ARM CPU works, by the time this instruction is actually executing, the PC has moved to two instuctions later due to pipeline pre-fetch.
						Snprintf(dd.mOperands, EAArrayCount(dd.mOperands), "#%d (forward to %08x)", offset, (uint32_t)((int64_t)dd.mInstructionAddress + pcOffset));

						Snprintf(dd.mHelp, EAArrayCount(dd.mHelp), "Compare value in r%d with zero, and conditionally branch forward to [pc + 4 + %d)", regDest, offset);
					}
				}
				else    // Push/pop registers / (unpredictable) / Change processor state / reverse bytes / software breakpoint / if-then instructions / NOP-compatible hints
				{
					int bits9_10 = GetBitRangeUnsigned(in, 9, 10);

					if(bits9_10 == 2) // Push/pop registers
					{
						int bit11 = GetBit(in, 11); // 0 -> store to memory (push), 1 -> load from memory (pop).

						if(bit11 == 0)
						{
							int bit8  = GetBit(in, 8);  // 0 -> don't push LR, 1 -> push LR.

							if(bit8 == 0)
							{
								// PUSH { Rlist }. Push the registers specified by Rlist onto the stack. Update the stack pointer.
								WriteThumbInstructionRlist(dd, kITStore, in, "push", 0, 7, NULL);
								Strcpy(dd.mHelp, "Push registers onto the stack and update the stack pointer.");
							}
							else
							{
								// PUSH { Rlist, LR }. Push the Link Register and the registers specified by Rlist (if any) onto the stack. Update the stack pointer.
								WriteThumbInstructionRlist(dd, kITStore, in, "push", 0, 7, "lr");
								Strcpy(dd.mHelp, "Push registers (including LR) onto the stack and update the stack pointer. Usually as a function prologue.");
							}
						}
						else
						{
							int bit8  = GetBit(in, 8);  // 0 -> don't pop PC, 1 -> pop PC.

							if(bit8 == 0)
							{
								// POP { Rlist }. Pop values off the stack into the registers specified by Rlist. Update the stack pointer.
								WriteThumbInstructionRlist(dd, kITLoad, in, "pop", 0, 7, NULL);
								Strcpy(dd.mHelp, "Pop registers from the stack and update the stack pointer.");
							}
							else
							{
								// POP { Rlist, PC }. Pop values off the stack and load into the registers specified by Rlist. Pop the PC off the stack. Update the stack pointer.
								WriteThumbInstructionRlist(dd, kITLoad, in, "pop", 0, 7, "pc");
								Strcpy(dd.mHelp, "Pop registers (including PC) from the stack and update the stack pointer. Usually for a function return.");
							}
						}
					}
					else if(bits9_10 == 1) // reverse bytes
					{
						int operation = GetBitRangeUnsigned(in, 6, 7);

						if(operation == 0)
						{
							WriteThumbInstructionRR(dd, kITImmediate, in, "rev", regDest, 0, 2, regSource, 3, 5, 0, 0);
							Snprintf(dd.mHelp, EAArrayCount(dd.mHelp), "Byte-reverse (endian swap) the entire word (uint32_t) in r%d to r%d.", regSource, regDest);
						}
						else if(operation == 1)
						{
							WriteThumbInstructionRR(dd, kITImmediate, in, "rev", regDest, 0, 2, regSource, 3, 5, 0, 0);
							Snprintf(dd.mHelp, EAArrayCount(dd.mHelp), "Byte-reverse the two individual uint16_t's in r%d to r%d. This is an uncommon instruction.", regSource, regDest);
						}
						else if(operation == 2)
						{
							// Invalid. Fall through to invalid handling below.
						}
						else if(operation == 3)
						{
							WriteThumbInstructionRR(dd, kITImmediate, in, "rev", regDest, 0, 2, regSource, 3, 5, 0, 0);
							Snprintf(dd.mHelp, EAArrayCount(dd.mHelp), "Byte-reverse (endian swap) signed half word (int16_t) in r%d to r%d.", regSource, regDest);
						}
					}
					else
					{
						int bits8_11 = GetBitRangeUnsigned(in, 8, 11);

						if(bits8_11 == 14) // 0x1110 // Software breakpoint
						{
							// BKPT #<imm8>
							WriteThumbInstructionI(dd, kITMisc, in, "bkpt", value, 0, 7);
							Snprintf(dd.mHelp, EAArrayCount(dd.mHelp), "Causes a debug break to occur (assuming debugger support). value %d is used by handler.", value);
						}
						else if(bits8_11 == 15) // 0x1111 // if-then instructions / NOP-compatible hints
						{
							int bits0_3 = GetBitRangeUnsigned(in, 0, 3);

							if(bits0_3 == 0) // NOP-compatible hints
							{
								int bits4_7 = GetBitRangeUnsigned(in, 4, 7);

								if(bits4_7 == 0) // nop
								{
									WriteThumbInstruction(dd, kITMisc, in, "nop");
									Strcpy(dd.mHelp, "Does nothing (no operation).");
								}
								else if(bits4_7 == 1) // yield
								{
									WriteThumbInstruction(dd, kITMisc, in, "yield");
									Strcpy(dd.mHelp, "Yields the CPU. Otherwise does nothing.");
								}
								else if(bits4_7 == 2) // wfe
								{
									WriteThumbInstruction(dd, kITMisc, in, "wfe");
									Strcpy(dd.mHelp, "Wait for event. Otherwise does nothing.");
								}
								else if(bits4_7 == 3) // wfe
								{
									WriteThumbInstruction(dd, kITMisc, in, "wfi");
									Strcpy(dd.mHelp, "Wait for interrupt. Otherwise does nothing.");
								}
								else if(bits4_7 == 4) // sev
								{
									WriteThumbInstruction(dd, kITMisc, in, "sev");
									Strcpy(dd.mHelp, "Send event. Otherwise does nothing.");
								}
								else
								{
									// Invalid. Fall through to invalid handling below.
								}
							}
							else // if-then instructions
							{
								int mask = GetBitRangeUnsigned(in, 0, 3);
								int firstCondition = GetBitRangeUnsigned(in, 4, 7);

								// This is a complicated instruction. For the time being we just print "it <mask> <firstcondition>"
								dd.mInstructionType = kITBranch;
								Strcpy(dd.mOperation, "it");
								Snprintf(dd.mOperands, EAArrayCount(dd.mOperands), "%04x, %04x", mask, firstCondition);
								Strcpy(dd.mHelp, "If/Then instruction. This disassembly is not a true disassembly of this instruction.");
								// To do: Implement the full disassembly of this instruction.
								// To do: Some of the possible bit patterns here for mask and firstCondition are invalid.
							}
						}
						else if((bits8_11 == 6) && (GetBit(in, 3) == 0)) // Change processor state
						{
							// CPS<effect><q> <iflags>
							// cpsie.n i
							// cpsid.w f
							// The instruction modifies the PRIMASK and FAULTMASK special-purpose register values.

							// This is a complicated instruction. For the time being we just print "it <mask> <firstcondition>"
							dd.mInstructionFlags |= kIFSupervisor;
							dd.mInstructionType = kITMisc;
							if(GetBit(in, 4) == 0)
								Strcpy(dd.mOperation, "cpsie");
							else
								Strcpy(dd.mOperation, "cpsid");
							Snprintf(dd.mOperands, EAArrayCount(dd.mOperands), "%d", GetBitRangeUnsigned(in, 0, 1));
							Strcpy(dd.mHelp, "Enable or disable interrupts. Privileged instruction.");
							// To do: Implement the full disassembly of this instruction.
						}
						// Else invalid. Fall through to invalid handling below.
					}
				}
			}

			break;
		}
		case 6:             // Multiple load/store | Conditional branch | Software Interrupt
		{
			int bit12 = GetBit(in, 12);

			if(bit12 == 0)  // Multiple load/store
			{
				int bit11 = GetBit(in, 11);

				if(bit11 == 0)  // store
				{
					// STMIA Rb!, { Rlist }. Store the registers specified by Rlist, starting at the base address in Rb. Write back the new base address.
					WriteThumbInstructionMia(dd, kITStore, in, "stmia", regArg, 8, 10, 0, 7);
					Snprintf(dd.mHelp, EAArrayCount(dd.mHelp), "Store the registers specified by the {} list in ascending order, starting at the base address in r%d. Write back the new base address to r%d.", regArg, regArg);
				}
				else    // load
				{
					// LDMIA Rb!, { Rlist }. Load the registers specified by Rlist, starting at the base address in Rb. Write back the new base address.
					WriteThumbInstructionMia(dd, kITStore, in, "ldmia", regArg, 8, 10, 0, 7);
					Snprintf(dd.mHelp, EAArrayCount(dd.mHelp), "Load the registers specified by the {} list in ascending order, starting at the base address in r%d. Write back the new base address to r%d.", regArg, regArg);
				}
			}
			else
			{
				int condition = GetBitRangeUnsigned(in, 8, 11);

				if(condition != 15)  // Conditional branch. A value of 15 (1111) indicates another instruction: execute software interrupt.
				{
					// BEQ label. Branch if Z set (equal).
					// ...
					const char* pOperationNames[15] = { "beq.n", "bne.n", "bcs.n", "bcc.n", "bmi.n", "bpl.n", "bvs.n", "bvc.n", "bhi.n", "bls.n", "bge.n", "blt.n", "bgt.n", "ble.n", "<invalid>" };
					const char* pOperationHelpFormat[15] = 
					{
						"Branch by offset of 4 + %d if CPSR register's Z is set (last compare was equal).",
						"Branch by offset of 4 + %d if CPSR register's Z is clear (last compare was not equal).",
						"Branch by offset of 4 + %d if CPSR register's C is set (unsigned higher or same).",
						"Branch by offset of 4 + %d if CPSR register's C is clear (unsigned lower).",
						"Branch by offset of 4 + %d if CPSR register's N is set (negative).",
						"Branch by offset of 4 + %d if CPSR register's N is clear (positive or zero).",
						"Branch by offset of 4 + %d if CPSR register's V is set (overflow.)",
						"Branch by offset of 4 + %d if CPSR register's V is clear (no overflow).",
						"Branch by offset of 4 + %d if CPSR register's C is set and Z clear (unsigned higher).",
						"Branch by offset of 4 + %d if CPSR register's C is clear or Z set (unsigned lower or same).",
						"Branch by offset of 4 + %d if CPSR register's N is set and V set, or N clear and V clear (greater or equal).",
						"Branch by offset of 4 + %d if CPSR register's N is set and V clear, or N clear and V set (less than).",
						"Branch by offset of 4 + %d if CPSR register's Z is clear, and either N set and V set or N clear and V clear (greater than).",
						"Branch by offset of 4 + %d if CPSR register's Z is set, or N set and V clear, or N clear and V set (less than or equal).", 
						"Invalid instruction" // 1110 is an invalid compare operation.
					}; 

					WriteThumbInstructionB(dd, kITBranch, in, pOperationNames[condition], value, 0, 7);
					Snprintf(dd.mHelp, EAArrayCount(dd.mHelp), pOperationHelpFormat[condition], value);
				}
				else    // Software Interrupt
				{
					// SWI Value8. Perform Software Interrupt: Move the address of the next instruction into LR, move CPSR to SPSR, load the software interrupt vector address (0x8) into the PC. Switch to ARM state and enter supervisor mode.
					WriteThumbInstructionI(dd, kITMisc, in, "swi", value, 0, 7);
					Snprintf(dd.mHelp, EAArrayCount(dd.mHelp), "Interrupt: Move address of next instruction to LR, CPSR to SPSR, handler address (0x8) to PC. Switch to ARM state and supervisor mode. value %d is used by handler.", value);
				}
			}

			break;
		}
		case 7:             // Unconditional branch | Long branch with link
		{
			int bits11_12 = GetBitRangeUnsigned(in, 11, 12);

			if(bits11_12 == 0)  // Unconditional branch
			{
				// B label. Branch PC relative +/- Offset11 << 1, where label is PC +/- 2048 bytes.
				WriteThumbInstructionB(dd, kITBranch, in, "b.n", value, 0, 10);
				Snprintf(dd.mHelp, EAArrayCount(dd.mHelp), "Branch unconditionally by offset of 4 + %d.", value);
			}
			else // Else we have a bit pattern of 1111 10xx xxxx xxxx / 1111 1xxx xxxx xxxx / 1110 1xxx xxxx xxxx
			{
				// This case is special, as it's the entryway to 32 bit Thumb-2 instructions.
				// In original Thumb, this was an instruction called "Long branch with link (bl)" that didn't have a direct equivalent in ARM.
				// See the ARM architecture manual for a description of the 32 bit Thumb format.
				DasmThumb32(in, in2, dd);
				instructionCount = 2; // Even if the 32 bit instruction is some invalid combination, we still assume that we are dealing with a 32 bit instruction instead of a 16 bit one.
			}

			break;
		}
	}

	if((dd.mOperation[0] == 0) || Stristr(dd.mOperation, "invalid"))
	{
		dd.mInstructionType   = kITInvalid;
		dd.mInstructionFlags |= kIFInvalid;

		if(Strstr(dd.mHelp, "EADasm")) // If this is a case of an unfinished instruction parsing on our side...
			Strcpy(dd.mOperation, "(to-do)");
		else
		{
			Strcpy(dd.mOperation, "<invalid>");
			Strcpy(dd.mHelp, "Invalid instruction as of ARM7.");
		}
	}

	return instructionCount;
}


} // namespace ARM

} // namespace Dasm

} // namespace EA


#endif // EACALLSTACK_DASM_ARM_ENABLED





