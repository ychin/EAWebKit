///////////////////////////////////////////////////////////////////////////////
// Copyright (c) Electronic Arts Inc. All rights reserved.
///////////////////////////////////////////////////////////////////////////////

#include <EACallstackTest/EACallstackTest.h>

EA_DISABLE_VC_WARNING(4740) // flow in or out of inline asm code suppresses global optimization.
EA_DISABLE_VC_WARNING(4592); // warning C4592: symbol will be dynamically initialized
EA_DISABLE_VC_WARNING(4073); // warning C4073: initializers put in library initialization area

#if defined(EA_COMPILER_MSVC)

#pragma init_seg(user)

#endif


union variant
{
	uint32_t m32;
	uint16_t m16[2];
	uint8_t  m8[4];
};



///////////////////////////////////////////////////////////////////////////////
// ConvertDasmFileToBinary
//
// We expect the file to look like this (arm-thumb):
//     0004e2dc <_ZN19EAThreadDynamicDataD1Ev>:
//        4e2dc:	b510      	push	{r4, lr}
//        4e2de:	1c04      	adds	r4, r0, #0
//        4e2e0:	f7ba ef38 	blx	9154 <_Z18androidGetFileRootv-0xc4>
//
// or this (arm):
//     00094610 <_ZNK2EA2IO10FileStream7GetTypeEv>:
//        94610:	e52db004 	push	{fp}		; (str fp, [sp, #-4]!)
//        94614:	e28db000 	add	fp, sp, #0	; 0x0
//
// or this:
//     (to do: describe others)
//
// We convert the disassembly to nothing more than binary instructions.
//
EA::Dasm::Processor ConvertDasmFileToBinary(const wchar_t* pFilePath, eastl::vector<uint8_t>& data, uint64_t& startingAddress)
{
	EA::Dasm::Processor processor = EA::Dasm::kProcessorNone;
	EA::IO::FileStream  file(pFilePath);
	file.AddRef();
	const bool          bGCC     = (EA::StdC::Stristr(pFilePath, EA_WCHAR("gcc-")) != NULL);
  //const bool          bVC      = (EA::StdC::Stristr(pFilePath, EA_WCHAR("vc-"))  != NULL);
	const bool          bARM     = (EA::StdC::Stristr(pFilePath, EA_WCHAR("arm-")) != NULL);
  //const bool          bPowerPC = (EA::StdC::Stristr(pFilePath, EA_WCHAR("ppc-")) != NULL);

	data.clear();

	if(file.Open())
	{
		char line[256];
		char fileBuffer[2048];
		EA::IO::size_type size;
		int lineCount = 0;

		// This buffered stream greatly improves performance on systems where single byte file IO is slow.
		EA::IO::StreamBuffer bufferedStream(fileBuffer, sizeof(fileBuffer), NULL, 0, &file);
		while((size = EA::IO::ReadLine(&bufferedStream, line, 256)) < EA::IO::kSizeTypeDone) // While there were more lines...
		{
			if(size && bGCC && bARM)
			{
				processor = EA::Dasm::kProcessorARM;

				// Surely this code is a bit fragile.
				char*   pInstructionBegin = EA::StdC::Strstr(line, ":\t");
				char*   p;
				variant v;

				if(pInstructionBegin) // If it looks like this line has an instruction...
				{
					if(lineCount == 0) // If this is the first instruction line...
						EA::StdC::Sscanf(line, "%I64x", &startingAddress);

					pInstructionBegin += 2; // It now points to something like: "f7ba ef38 	blx	9154 <_Z18androidGetFileRootv-0xc4>"

					for(p = pInstructionBegin; *p; p++)
					{
						if(*p == '\t')
							*p = ' ';
					}

					p = EA::StdC::Strstr(pInstructionBegin, "  "); // We assume that >= two whitespace chars is the space between instructions and the asm text.
					if(p)
						*p = 0; // It now points to something like: "f7ba ef38"

					v.m32 = EA::StdC::StrtoU32(pInstructionBegin, &p, 16); // Read an up to 32 bit hex (i.e. base 16) number.

					if(p < (pInstructionBegin + 8)) // If it appears to be a 16 bit instruction (i.e. thumb)...
					{
						// ARM Thumb instructions are 16 bit, but there's a 32 bit Thumb extension which emits pairs of 16 bit instructions.
						v.m16[0] = (uint16_t)v.m32;
						v.m16[0] = EA::StdC::ToLittleEndian(v.m16[0]);
						data.insert(data.end(), v.m8, v.m8 + 2);

						if(EA::StdC::Strlen(p) >= 4)
						{
							v.m16[0] = (uint16_t)EA::StdC::StrtoU32(p, &p, 16); // Will there ever be more than two instructions?
							v.m16[0] = EA::StdC::ToLittleEndian(v.m16[0]);
							data.insert(data.end(), v.m8, v.m8 + 2);
						}
					}
					else
					{
						v.m32 = EA::StdC::ToLittleEndian(v.m32);
						data.insert(data.end(), v.m8, v.m8 + 4);
					}

					lineCount++;
				}
			}
		}
	}

	return data.empty() ? EA::Dasm::kProcessorNone : processor;
}


///////////////////////////////////////////////////////////////////////////////
// TestDasm
//
int TestDasm()
{
	EA::UnitTest::ReportVerbosity(1, "\nTestDasm\n");

	int nErrorCount = 0;

	// Get the instruction pointer. We'll disassemble this function
	// with the code from this function.
	void* pInstruction;
	EAGetInstructionPointer(pInstruction);

	EA::Dasm::Processor currentProcessor = EA::Dasm::kProcessorNone;
	currentProcessor = EA::Dasm::DetectCurrentProcessor();


	{
		// Do file-based dasm testing
		for(eastl::list<eastl::wstring>::iterator it = gTestFileList.begin(); it != gTestFileList.end(); ++it)
		{
			const eastl::wstring& sFilePath = *it;

			if(sFilePath.find(EA_WCHAR(".dasm")) < sFilePath.size()) // For each .dasm file...
			{
				eastl::vector<uint8_t> data;
				uint64_t               dasmBaseAddress = 0;

				EA::Dasm::Processor processor = ConvertDasmFileToBinary(sFilePath.c_str(), data, dasmBaseAddress);

				switch((int)processor) // Cast to int to avoid compiler warnings about non-handled enums.
				{
					case EA::Dasm::kProcessorARM:
					{
						#if EACALLSTACK_DASM_ARM_ENABLED
							EA::Dasm::DisassemblerARM dasm;
							uint32_t                  optionFlags = EA::Dasm::kOFHex | EA::Dasm::kOFHelp;
							eastl::string             s;

							if(EA::StdC::Stristr(sFilePath.c_str(), EA_WCHAR("thumb")))
								optionFlags |= EA::Dasm::kOFThumb;
							else
								optionFlags |= EA::Dasm::kOFARM;

							EA::Dasm::DasmBlock(dasm, s, data.begin(), data.end(), optionFlags, dasmBaseAddress);

							EA::UnitTest::ReportVerbosity(1, "%ls\n", sFilePath.c_str());
							EA::UnitTest::ReportVerbosity(1, "%s", s.c_str());
							EA::UnitTest::ReportVerbosity(1, "\n\n"); // This is on its own line because the above might be so long that it gets chopped before it gets fully written on some systems.
						#endif
					}
				}
			}
		}
	}


	{
		// Read a block of instructions into data.
		uint8_t data[512];

		memcpy(data, pInstruction, sizeof(data)); // Fill in the data with the code for this function.

		const void* pCurrent = data;
		const void* pEnd     = data + sizeof(data);

		// Verify that DetectProcessor works as expected.
		EA::Dasm::Processor processor = EA::Dasm::DetectProcessor(pCurrent, pEnd);

		if(processor == currentProcessor)
		{
			if(processor == EA::Dasm::kProcessorPowerPC)
			{
				#if EACALLSTACK_DASM_POWERPC_ENABLED
					EA::Dasm::DisassemblerPowerPC dPowerPC;
					EA::Dasm::DasmData dd;
					dd.mOptionFlags = EA::Dasm::kOFHex | EA::Dasm::kOFMnemonics;

					while(pCurrent != pEnd)
					{
						pCurrent = dPowerPC.Dasm(pCurrent, pEnd, dd);

						if((dd.mInstructionFlags & EA::Dasm::kIFInvalid) == 0) // If this is a real instruction...
						{
							// EA::UnitTest::ReportVerbosity(1, "%s %s %s\n", dd.mAddress, dd.mOperation, dd.mOperands);
						}
					}

					// template <typename String>
					// bool Dasm(Disassembler& d, String& s, const void* pData, const void* pDataEnd, uint32_t optionFlags = 0, uint64_t dataAddress = 0)
					eastl::string s8;
					uint8_t optionFlags = (EA::Dasm::kOFHex | EA::Dasm::kOFMnemonics | EA::Dasm::kOFHelp);
					EA::Dasm::DasmBlock(dPowerPC, s8, data, data + sizeof(data), optionFlags, 0);
					EATEST_VERIFY(!s8.empty()); // Not much of a test, really.
				#endif
			}
			else if(processor == EA::Dasm::kProcessorX86)
			{
				#if EACALLSTACK_DASM_X86_ENABLED
					EA::Dasm::DisassemblerX86 dX86;
					EA::Dasm::DasmData dd;
					dd.mOptionFlags = EA::Dasm::kOFHex | EA::Dasm::kOFMnemonics;

					while(pCurrent != pEnd)
					{
						pCurrent = dX86.Dasm(pCurrent, pEnd, dd);

						if((dd.mInstructionFlags & EA::Dasm::kIFInvalid) == 0) // If this is a real instruction...
						{
							// EA::UnitTest::ReportVerbosity(1, "%s %s %s\n", dd.mAddress, dd.mOperation, dd.mOperands);
						}
					}
				#endif
			}
			else if(processor == EA::Dasm::kProcessorARM)
			{
				#if EACALLSTACK_DASM_ARM_ENABLED
					EA::Dasm::DisassemblerARM dARM;
					EA::Dasm::DasmData dd;
					dd.mOptionFlags = EA::Dasm::kOFHex | EA::Dasm::kOFMnemonics;

					while(pCurrent != pEnd)
					{
						pCurrent = dARM.Dasm(pCurrent, pEnd, dd);

						if((dd.mInstructionFlags & EA::Dasm::kIFInvalid) == 0) // If this is a real instruction...
						{
							// EA::UnitTest::ReportVerbosity(1, "%s %s %s\n", dd.mAddress, dd.mOperation, dd.mOperands);
						}
					}
				#endif
			}
		}
	}


	{
		// Here we test random instruction patterns on RISC architectures.
		// We want to make sure the disassembler is robust enough to deal with
		// invalid instruction sequences.

		#if EACALLSTACK_DASM_POWERPC_ENABLED
		{
			EA::UnitTest::Rand rng(EA::UnitTest::GetRandSeed());
			EA::Dasm::DasmData dd;

			#if defined(EA_PLATFORM_DESKTOP)
				const int kCount = 1000000;
			#else
				const int kCount =   10000;
			#endif

			for(int i = 0; i < kCount; i++)
			{
				uint32_t instruction = rng.RandValue();
				EA::Dasm::DisassemblerPowerPC dPowerPC;

				const void* p = dPowerPC.Dasm(&instruction, &instruction + 1, dd);

				EATEST_VERIFY((p >= &instruction) && (p <= (&instruction + 1)));
				EATEST_VERIFY(strlen(dd.mOperation) < sizeof(dd.mOperation));
				EATEST_VERIFY(strlen(dd.mOperands)  < sizeof(dd.mOperands));
			}
		}
		#endif

		#if EACALLSTACK_DASM_ARM_ENABLED
		{
			EA::UnitTest::Rand        rng(EA::UnitTest::GetRandSeed());
			EA::Dasm::DisassemblerARM dARM;
			EA::Dasm::DasmData        dd;

			#if defined(EA_PLATFORM_DESKTOP)
				const int kCount = 2000000;
			#else
				const int kCount =   10000;
			#endif

			for(int t = 0; t < 2; t++)
			{
				if(t == 0)
					dd.mOptionFlags |= EA::Dasm::kOFARM;    // Force-interpret it as 32 bit ARM code.
				else
					dd.mOptionFlags |= EA::Dasm::kOFThumb;  // Force-interpret it as Thumb code.

				for(int i = 0; i < kCount; i++)
				{
					uint32_t instruction = rng.RandValue();
					dARM.Dasm(&instruction, &instruction + sizeof(instruction), dd);

					EATEST_VERIFY(strlen(dd.mOperation) < sizeof(dd.mOperation));
					EATEST_VERIFY(strlen(dd.mOperands)  < sizeof(dd.mOperands));
				}
			}
		}
		#endif
	}

	return nErrorCount;
}

/* the test is disabled on PS5 because the hardware doesn't allow you to read from the executable's memory */
#if !defined(EA_PLATFORM_PS5)

TEST(EACallstackTest, TestDasmDetectCurrentProcessor)
{
	EA::Dasm::Processor currentProcessor = EA::Dasm::kProcessorNone;

	currentProcessor = EA::Dasm::DetectCurrentProcessor();

	EXPECT_NE(currentProcessor, EA::Dasm::kProcessorNone);
}

TEST(EACallstackTest, TestDasm)
{
	int nErrorCount = TestDasm();

	EXPECT_EQ(nErrorCount, 0);
}

#endif // !EA_PLATFORM_PS5


EA_RESTORE_VC_WARNING();
EA_RESTORE_VC_WARNING();
EA_RESTORE_VC_WARNING();
