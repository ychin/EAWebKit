/////////////////////////////////////////////////////////////////////////////
// Copyright (c) Electronic Arts Inc. All rights reserved.
/////////////////////////////////////////////////////////////////////////////

///////////////////////////////////////////////////////////////////////////////
//
// This module implements platform independent disassembly of machine instructions
// across multiple platforms/processors.
//
///////////////////////////////////////////////////////////////////////////////


#ifndef EACALLSTACK_EADASM_H
#define EACALLSTACK_EADASM_H


#include <EABase/eabase.h>
#include <EACallstack/internal/Config.h>
#include <EAStdC/EASprintf.h>
#include <stdio.h>


namespace EA
{
namespace Dasm
{

	enum OptionFlags
	{
		kOFNone      = 0x0000,   /// Placeholder for zero flags.
		kOFMnemonics = 0x0001,   /// Enable the reporting of instruction mnemonics, which are simplified forms of instructions.
		kOFHex       = 0x0002,   /// Display integers as hex.
		kOFHelp      = 0x0004,   /// Appends a small explanation to each instruction.
		kOFSelf      = 0x0100,   /// The program is disassembling itself. This allows for additional actions such as reading memory while disassembling. Disabled by default, as there's no way for a piece of code to know for certain that it's disassembling itself.
		kOFARM       = 0x1000,   /// ARM (as opposed to ARM Thumb 16 bit instructions).
		kOFThumb     = 0x2000    /// ARM Thumb (as opposed to ARM standard 32 bit instructions).
	};


	enum Processor
	{
		kProcessorNone,         /// No identified processor.
		kProcessorX86,          /// Intel 32 bit x86 processor family.
		kProcessorX86_64,       /// AMD/Intel 64 bit x86-64 processor family.
		kProcessorARM,          /// ARM processor family.
		kProcessorPowerPC,      /// PowerPC 32 bit and 64 bit processor family.
		kProcessorMIPS          /// MIPS processor family.
	};


	enum InstructionType
	{
		kITNone,        /// No instruction in particular.
		kITInvalid,     /// Not a valid instruction.
		kITBranch,      /// Branch instruction, result of if/for/do/while statements in C/C++.
		kITLoad,        /// Read of memory into a register.
		kITStore,       /// Write of register to memory.
		kITImmediate,   /// Read of or use of an immediate (integer literal) into a register.
		kITMisc         /// All other instructions.
	};


	enum InstructionFlags
	{
		kIFNone       =   0,    /// Placeholder for zero flags.
		kIFInvalid    =   1,    /// Not a valid instruction.
		kIFFPU        =   2,    /// FPU instruction.
		kIFUnsigned   =   4,    /// Unsigned integer as opposed to signed.
		kIFSupervisor =   8,    /// Priviledged.
		kIFARM        =   8,    /// ARM standard instructions.
		kIFThumb      =  16,    /// ARM Thumb instructions.
		kIFAltivec    =  16,    /// PowerPC Altivec.
		kIFSSE        =  32,    /// x86 SSE
		kIFDebug      =  64,    /// Debug instruction.
		kIF64Bit      = 128,    /// 64 bit instruction.
		kIFMicrocode  = 256,    /// Instruction is microcoded.
		kIFNop        = 512     /// Instruction is a no-op.
	};


	// Internal struct
	struct MemorySpan
	{
		uint32_t mAddress;
		uint32_t mSize;
	};

	// Internal struct
	struct EACALLSTACK_API Data
	{
		MemorySpan mDataBlocks[6];
		uint32_t   mDataBlockCount;

		Data() : mDataBlockCount(0){}
		void clear() { mDataBlockCount = 0; }
		void AddSpan(uint32_t address, uint32_t size);
		void AddUint32(uint32_t address) { AddSpan(address, 4); }
		void AddUint16(uint32_t address) { AddSpan(address, 2); }
		void AddUint8(uint32_t address) { AddSpan(address, 1); }
		bool HasSpan(uint32_t address, uint32_t size) const;
		bool HasUint32(uint32_t address) const { return HasSpan(address, 4); }
		bool HasUint16(uint32_t address) const { return HasSpan(address, 2); }
		bool HasUint8(uint32_t address) const { return HasSpan(address, 1); }
	};


	/// DasmData
	///
	/// Stores the disassembled instruction data.
	///
	struct EACALLSTACK_API DasmData
	{
		// Input data
		uint32_t        mOptionFlags;           /// See enum OptionFlags.

		// Output data
		char            mAddress[17];           /// The address of the instruction; string version of mInstructionAddress.
		char            mOperation[32];         /// The instruction as a string.
		char            mOperands[96];          /// The instruction arguments as a string.
		char            mHelp[256];             /// Help for the instruction, if the kOFHelp option is enabled.
		Processor       mProcessorType;         /// See enum Processor.
		InstructionType mInstructionType;       /// See enum InstructionType.
		uint32_t        mInstructionFlags;      /// See enum InstructionFlags.
		char            mInstruction[20];       /// The binary instruction value as a string as would be displayed by a disassembler.
		uint64_t        mInstructionAddress;    /// Address of the instruction. Defines as a uint64_t so that it can work for both 32 and 64 bit platforms.
		int32_t         mDisplacement;          /// Branch, load, or store displacement.

		// State data
		uint64_t        mBaseAddress;           /// The base address for the first instruction. This field is const and not modified by the disassembler.
		Data            mData;                  /// Provides a list of code spans that are known to be data as opposed to instructions.

		// Functions
		DasmData();
		void Clear();
		void ClearLastInstruction();
	};



	/// DetectCurrentProcessor
	///
	/// Detects the current processor type.
	///
	EACALLSTACK_API Processor DetectCurrentProcessor();


	/// DetectProcessor
	///
	/// Detects the processor type from a stream of machine instructions.
	///
	EACALLSTACK_API Processor DetectProcessor(const void* pData, const void* pDataEnd);



	/// Disassembler
	///
	/// Provides dasm (disassembler) functionality.
	///
	class EACALLSTACK_API Disassembler
	{
	public:
		virtual ~Disassembler() {}

		/// Dasm
		///
		/// Disassembles the next instruction, given a processor and pointer to instruction data.
		/// Returns a pointer to the next instruction. If the instruction is invalid the return
		/// value is a pointer to where the next instruction should be read, even though that
		/// instruction may be invalid as well. On RISC platforms, this will always be four bytes
		/// later. On CISC platforms, this will be the minimum instruction size for the platform,
		/// which is usually one byte.
		///
		/// This function works somewhat like an STL iterator; you pass in a pointer to a begin
		/// iterator (pData) and an end iterator (pDataEnd) and it returns the iterator to use
		/// for the next instruction.
		///
		/// The dataAddress argument is the address which pData refers to. If pData refers to actual
		/// instructions in memory on a machine at their current location, then dataAddress should
		/// be the same as pData. However, if pData is pointing to memory that is coming from a
		/// disk file or was malloc'd and filled with instructions, then you would want to set
		/// dataAddress to the actual address that the instructions originally resided at in the
		/// running program (if you happen to know what that address is). Sometimes this address
		/// is referred to as "rva" (relative virtual address).
		///
		/// Note: optionFlags and dataAddress are deprecated. Instead you should set the DasmData
		///       mOptionFlags and mInstructionAddressInput fields respectively.
		///
		/// Example usage:
		///     char                buffer[1024]; // Let's assume this is filled with 1024 bytes of instructions.
		///     DisassemblerPowerPC dasm;
		///
		///     for(const void* pCurrent = buffer; pCurrent < (buffer + 1024); )
		///     {
		///         DasmData dd;
		///         pCurrent = dasm.Dasm(pCurrent, buffer + 1024, dd, kOFHex | kOFMnemonics);
		///         printf("%s %s %s\n", dd.mAddress, dd.mOperation, dd.mOperands);
		///     }
		///
		virtual const void* Dasm(const void* pData, const void* pDataEnd, DasmData& dasmData,
								  uint32_t optionFlagsDeprecated = 0, uint64_t dataAddressDeprecated = 0) = 0;
	};


	/// DasmBlock
	///
	/// Disassembles a block of memory.
	///
	/// Example usage:
	///     char                buffer[1024]; // Let's assume this is filled with 1024 bytes of instructions.
	///     DisassemblerPowerPC dasm;
	//      DasmData            dd;
	///     eastl::string8      s;
	///
	///     dd.mOptionFlags = kOFHex | kOFHelp;
	///     Dasm(dasm, s, buffer, buffer + 1024, dd, 0x87655678);
	///
	template <typename String>
	bool DasmBlock(Disassembler& d, String& s, const void* pData, const void* pDataEnd,
					uint32_t optionFlags, uint64_t dataAddress)
	{
		DasmData dd;
		const void* pDataCurrent = pData;

		s.clear();
		dd.mOptionFlags |= optionFlags;
		dd.mBaseAddress  = dataAddress;

		while(pDataCurrent < pDataEnd)
		{
			pDataCurrent = d.Dasm(pDataCurrent, pDataEnd, dd, 0, dataAddress + ((uintptr_t)pDataCurrent - (uintptr_t)pData));

			if(pDataCurrent > pData)
			{
				char instructionBuffer[64];
				EA::StdC::Snprintf(instructionBuffer, 64, "%s %s", dd.mOperation, dd.mOperands);

				char buffer[256];
				EA::StdC::Snprintf(buffer, 256, "%s: %-12s %-36s", dd.mAddress, dd.mInstruction, instructionBuffer);
				s += buffer;

				if((dd.mOptionFlags & kOFHelp) && dd.mHelp[0])
				{
					s += " | ";
					s += dd.mHelp;
				}

				s += '\n';

				// The following relateds to ARM, but it has no effect for other targets.
				if(dd.mInstructionFlags & kIFARM)
					dd.mOptionFlags = kOFARM;           // This allows us to skip executing instruction detection code next time through the loop.
				else if(dd.mInstructionFlags & kIFARM)
					dd.mOptionFlags = kOFThumb;
			}
			else // Else something went wrong and we protect against a possible infinite loop.
				break;
		}

		return true;
	}



	#if EACALLSTACK_DASM_POWERPC_ENABLED
		///////////////////////////////////////////////////////////////////////
		/// DisassemblerPowerPC
		///
		class EACALLSTACK_API DisassemblerPowerPC : public Disassembler
		{
		public:
			DisassemblerPowerPC();
			virtual ~DisassemblerPowerPC();

			const void* Dasm(const void* pData, const void* pDataEnd, DasmData& dasmData,
								uint32_t optionFlagsDeprecated = 0, uint64_t dataAddressDeprecated = 0);
		};
	#endif


	#if EACALLSTACK_DASM_X86_ENABLED
		///////////////////////////////////////////////////////////////////////
		/// DisassemblerX86
		///
		///
		class EACALLSTACK_API DisassemblerX86 : public Disassembler
		{
		public:
			DisassemblerX86();
			virtual ~DisassemblerX86();

			const void* Dasm(const void* pData, const void* pDataEnd, DasmData& dasmData,
								uint32_t optionFlagsDeprecated = 0, uint64_t dataAddressDeprecated = 0);
		};
	#endif


	#if EACALLSTACK_DASM_ARM_ENABLED
		///////////////////////////////////////////////////////////////////////
		/// DisassemblerARM
		///
		class EACALLSTACK_API DisassemblerARM : public Disassembler
		{
		public:
			DisassemblerARM();
			virtual ~DisassemblerARM();

			const void* Dasm(const void* pData, const void* pDataEnd, DasmData& dasmData,
								uint32_t optionFlagsDeprecated = 0, uint64_t dataAddressDeprecated = 0);
		};
	#endif


} // namespace Dasm
} // namespace EA


#endif // EACALLSTACK_EADASM_H
