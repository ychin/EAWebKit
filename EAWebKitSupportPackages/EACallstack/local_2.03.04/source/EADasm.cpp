///////////////////////////////////////////////////////////////////////////////
// Copyright (c) Electronic Arts Inc. All rights reserved.
///////////////////////////////////////////////////////////////////////////////




#include <EACallstack/EADasm.h>
#include <stdlib.h>
#include <string.h>
#include <EAAssert/eaassert.h>


namespace EA
{

namespace Dasm
{


///////////////////////////////////////////////////////////////////////////////
// Data
///////////////////////////////////////////////////////////////////////////////

void Data::AddSpan(uint32_t address, uint32_t size)
{
	uint32_t i;
	bool     bSpanExpanded = false;

	for(i = 0; i < mDataBlockCount; i++)
	{
		MemorySpan& span = mDataBlocks[i];

		if(((address + size) >= span.mAddress) &&
			(address <= (span.mAddress + span.mSize))) // If there is an overlap or direct contiguity...
		{
			uint32_t   low     = ((address < span.mAddress) ? address : span.mAddress);
			uint32_t   high    = (((address + size) > (span.mAddress + span.mSize)) ? (address + size) : (span.mAddress + span.mSize));
			MemorySpan newSpan = { low, high };

			if(memcmp(&newSpan, &span, sizeof(span)) == 0)
			{
				bSpanExpanded = true;
				span = newSpan;
			}
		}
	}

	if(i == mDataBlockCount) // If no match was found....
	{
		if(mDataBlockCount < EAArrayCount(mDataBlocks))
		{
			mDataBlocks[mDataBlockCount].mAddress = address;
			mDataBlocks[mDataBlockCount].mSize    = size;
			mDataBlockCount++;
		}
		// Else we are out of capacity. Maybe we should use dynamic container.
	}
	else if(bSpanExpanded) // else mDataBlocks[i] was expanded.
	{
		// Unfortunately, the above span expansion might have caused a bridge 
		// between it and another span that we didn't see. It turns out we can
		// solve this cleanly by simply calling ourselves recursively with the
		// most recently added span.
		AddSpan(mDataBlocks[i].mAddress, mDataBlocks[i].mSize);
	}
	// Else the input span was entirely within mDataBlocks[i].
}


bool Data::HasSpan(uint32_t address, uint32_t size) const
{
	for(uint32_t i = 0; i < mDataBlockCount; i++)
	{
		const MemorySpan& span = mDataBlocks[i];

		if((address          < (span.mAddress + span.mSize)) &&
		   ((address + size) >  span.mAddress)) // If there is an overlap or direct contiguity...
		{
			return true;
		}
	}

	return false;
}




///////////////////////////////////////////////////////////////////////////////
// DasmData
///////////////////////////////////////////////////////////////////////////////

DasmData::DasmData()
{
	Clear();
}


void DasmData::Clear()
{
	mOptionFlags        = 0;

	mAddress[0]         = 0;
	mOperation[0]       = 0;
	mOperands[0]        = 0;
	mHelp[0]            = 0;
	mProcessorType      = kProcessorNone;
	mInstructionType    = kITNone;
	mInstructionFlags   = 0;
	mInstruction[0]     = 0;
	mInstructionAddress = 0;
	mDisplacement       = 0;

	mBaseAddress        = 0;
	mData.clear();
}


void DasmData::ClearLastInstruction()
{
  //mAddress[0]         = 0; // Not sure we want to clear these.
	mOperation[0]       = 0;
	mOperands[0]        = 0;
	mHelp[0]            = 0;
  //mProcessorType      = kProcessorNone;
	mInstructionType    = kITNone;
	mInstructionFlags   = 0;
  //mInstruction[0]     = 0;
  //mInstructionAddress = 0; // Not cleared because currently the user can purposefully set this before calling Dasm().
	mDisplacement       = 0;
}



///////////////////////////////////////////////////////////////////////////////
// misc
///////////////////////////////////////////////////////////////////////////////

///////////////////////////////////////////////////////////////////////////////
// DetectCurrentProcessor
//
EACALLSTACK_API Processor DetectCurrentProcessor()
{
	// EABase already has code to detect the processor, so we just use it.

	#if defined(EA_PROCESSOR_POWERPC)
		return kProcessorPowerPC;
	#elif defined(EA_PROCESSOR_ARM)
		return kProcessorARM;
	#elif defined(EA_PROCESSOR_X86)
		return kProcessorX86;
	#elif defined(EA_PROCESSOR_X86_64)
		return kProcessorX86_64;
	#else
		return kProcessorNone;
	#endif
}


///////////////////////////////////////////////////////////////////////////////
// DetectProcessor
//
EACALLSTACK_API Processor DetectProcessor(const void* pData, const void* pDataEnd)
{
	Processor   processor                  = kProcessorNone;
	float       fValidInstructionRatio     = 0.f;    EA_UNUSED(fValidInstructionRatio);
	const float kMinValidInstructionRatio  = 0.40f;  // Below this value, we assume the processor identification is incorrect.
	const float kGoodValidInstructionRatio = 0.85f;  // Above this value, we assume the processor identification is correct.

	if(pData < pDataEnd) // If there is anything to read...
	{
		// Limit our reading to only so many bytes...
		const uintptr_t kMaxReadSize = 2048;

		if(((uintptr_t)pDataEnd - (uintptr_t)pData) > kMaxReadSize)
			pDataEnd = (const char*)pData + kMaxReadSize;


		EA_UNUSED(kMinValidInstructionRatio);
		EA_UNUSED(kGoodValidInstructionRatio);

		// Try ARM
		#if EACALLSTACK_DASM_ARM_ENABLED
			if(fValidInstructionRatio < kGoodValidInstructionRatio) // If an earlier test wasn't a perfect match...
			{
				// To do. 
				// First thing is to see if >= 30% of uint32_t's begin with 0xe (four bits). That would indicate 32 bit ARM.
				// How to detect 16 bit ARM Thumb instructions?
			}
		#endif

		// Try PowerPC
		#if EACALLSTACK_DASM_POWERPC_ENABLED
			if(fValidInstructionRatio < kGoodValidInstructionRatio) // If an earlier test wasn't a perfect match...
			{
				DisassemblerPowerPC disassemblerPowerPC;
				DasmData            dd;
				int                 instructionCount = 0;
				int                 validInstructionCount = 0;

				for(const void* pCurrent = pData; pCurrent < pDataEnd; ++instructionCount)
				{
					pCurrent = disassemblerPowerPC.Dasm(pCurrent, pDataEnd, dd);

					if((dd.mInstructionFlags & kIFInvalid) == 0) // If the instruction is valid...
						validInstructionCount++;
				}

				const float fRatio = (float)validInstructionCount / (float)instructionCount;

				if((fRatio > kMinValidInstructionRatio) && (fRatio > fValidInstructionRatio))
				{
					processor = kProcessorPowerPC;
					fValidInstructionRatio = fRatio;
				}
			}
		#endif

		#if EACALLSTACK_DASM_X86_ENABLED
			if(fValidInstructionRatio < kGoodValidInstructionRatio) // If an earlier test wasn't a perfect match...
			{
				DisassemblerX86 disassemblerX86;
				DasmData        dd;
				int             instructionCount = 0;
				int             validInstructionCount = 0;
				
				for(const void* pCurrent = pData; pCurrent < pDataEnd; ++instructionCount)
				{
					pCurrent = disassemblerX86.Dasm(pCurrent, pDataEnd, dd);

					if((dd.mInstructionFlags & kIFInvalid) == 0) // If the instruction is valid...
						validInstructionCount++;
				}

				const float fRatio = (float)validInstructionCount / (float)instructionCount;

				if((fRatio > kMinValidInstructionRatio) && (fRatio > fValidInstructionRatio))
				{
					processor = kProcessorX86;
					fValidInstructionRatio = fRatio;
				}
			}
		#endif
	}

	return processor;
}


} // namespace Dasm

} // namespace EA















