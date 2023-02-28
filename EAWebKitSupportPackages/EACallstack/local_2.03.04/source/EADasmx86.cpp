///////////////////////////////////////////////////////////////////////////////
// EADasmx86.cpp
//
// Copyright (c) 2005, Electronic Arts. All rights reserved.
// Maintained by Paul Pedriana.
///////////////////////////////////////////////////////////////////////////////


#include <EACallstack/EADasm.h>

#if EACALLSTACK_DASM_X86_ENABLED

// EACALLSTACK_DASM_X86_USABLE
//
// Defined as 0 or 1.
// Indicates if the current platform can load and execute DasmX86.
// 
#if EA_WINAPI_FAMILY_PARTITION(EA_WINAPI_PARTITION_DESKTOP) // We could conceivably build the DLL for console usage (e.g. XBox One), but DasmX86 is 32 bit only and so is of little use in practice to 64 bit consoles.
	#define EACALLSTACK_DASM_X86_USABLE 1
#else
	#define EACALLSTACK_DASM_X86_USABLE 0
#endif



#include <stdlib.h>
#include <string.h>
#include <stdio.h>
#include <limits.h>



namespace EA
{

namespace Dasm
{




///////////////////////////////////////////////////////////////////////////////
// DisassemblerX86
//
DisassemblerX86::DisassemblerX86()
{
	#if EACALLSTACK_DASM_X86_USABLE
		// no imp
	#endif
}


///////////////////////////////////////////////////////////////////////////////
// ~DisassemblerX86
//
DisassemblerX86::~DisassemblerX86()
{
	#if EACALLSTACK_DASM_X86_USABLE
		// no imp
	#endif
}



///////////////////////////////////////////////////////////////////////////////
// Dasm
//
const void* DisassemblerX86::Dasm(const void* pData, const void* pDataEnd, 
									DasmData& dd, uint32_t optionFlags, uint64_t dataAddress)
{
	dd.ClearLastInstruction();

	if(pData >= pDataEnd) // If there isn't space for one instruction...
	{
		dd.mInstructionFlags = kIFInvalid;
		return pDataEnd;
	}

	dd.mOptionFlags   |= optionFlags;
	dd.mProcessorType  = kProcessorX86;

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

	sprintf(dd.mAddress, "%08x", (unsigned)dd.mInstructionAddress);

	#if EACALLSTACK_DASM_X86_USABLE
		// no imp
	#endif
	
	// Print the instruction (operation and operands)
	sprintf(dd.mOperation, "Disassember not available");
	dd.mOperands[0] = 0;
	dd.mInstructionFlags |= kIFInvalid;

	return pDataEnd;
	
}



} // namespace Dasm

} // namespace EA


#endif // EACALLSTACK_DASM_X86_ENABLED




