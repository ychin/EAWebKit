///////////////////////////////////////////////////////////////////////////////
// DemoCompression.cpp
//
// Copyright (c) 2007 Criterion Software Limited and its licensors. All Rights Reserved.
// By Paul Pedriana
///////////////////////////////////////////////////////////////////////////////


#include "Demo.h"
#include <EAText/EATextCompression.h>
#include <EASTL/string.h>


int MainDemoCompression(int, char**)
{
	using namespace EA::Text;


	{
		// Demo SCSU compression
		TestString sOriginal(EATEXT_CHAR("abcdefg xyzpdq four score and seven years ago save the cheerleader save the world"));
		TestString sCompressed;
		TestString sDecompressed;

		const uint32_t compressedSize = CompressSCSU(sOriginal.data(), (uint32_t)sOriginal.length(), NULL, 0);
		sCompressed.resize(compressedSize);
		CompressSCSU(sOriginal.data(), (uint32_t)sOriginal.length(), &sCompressed[0], (uint32_t)sCompressed.length());

		sDecompressed.resize(sOriginal.size());
		DecompressSCSU(sCompressed.data(), (uint32_t)sCompressed.length(), &sDecompressed[0], (uint32_t)sDecompressed.length());
	}


	{
		// Demo BOCU compression
		TestString sOriginal(EATEXT_CHAR("abcdefg xyzpdq four score and seven years ago save the cheerleader save the world"));
		TestString sCompressed;
		TestString sDecompressed;

		const uint32_t compressedSize = CompressBOCU(sOriginal.data(), (uint32_t)sOriginal.length(), NULL, 0);
		sCompressed.resize(compressedSize);
		CompressBOCU(sOriginal.data(), (uint32_t)sOriginal.length(), &sCompressed[0], (uint32_t)sCompressed.length());

		sDecompressed.resize(sOriginal.size());
		DecompressBOCU(sCompressed.data(), (uint32_t)sCompressed.length(), &sDecompressed[0], (uint32_t)sDecompressed.length());
	}

	return 0;
}


