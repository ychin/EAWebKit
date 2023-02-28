///////////////////////////////////////////////////////////////////////////////
// DemoFontServer.cpp
//
// Copyright (c) 2007 Criterion Software Limited and its licensors. All Rights Reserved.
// By Paul Pedriana
///////////////////////////////////////////////////////////////////////////////


#include "Demo.h"
#include <EAText/EATextFontServer.h>
#include <EAText/EATextStyle.h>
#include <EAText/EATextOutlineFont.h>
#include <EAStdC/EAString.h>
#include <coreallocator/icoreallocator_interface.h>
#include <EAIO/EAStreamMemory.h>
#include <EAIO/EAFileStream.h>
#include <stdlib.h>

namespace
{
	class CoreAllocatorMalloc : public EA::Allocator::ICoreAllocator 
	{
	public:
		void* Alloc(size_t size, const char* /*name*/, unsigned /*flags*/)
		{
			return new char[size];
		}

		void* Alloc(size_t size, const char* /*name*/, unsigned /*flags*/, unsigned /*align*/, unsigned /*alignOffset*/)
		{
			return new char[size]; // Actually you need to have this call an aligned allocation function.
		}

		void Free(void* ptr, size_t /*size*/)
		{
			delete[] (char*)ptr;
		}
	};

} // namespace


///////////////////////////////////////////////////////////////////////////////
// MainDemoFontServer
//
// The purpose of a FontServer is manage individual fonts and faces. It allows
// you to do lookups of fonts based on style and provides shared storage of 
// fonts so that memory is used efficiently and not duplicated.
// You don't need this FontServer in order to use EAText.
//
int MainDemoFontServer(int, char**)
{
	using namespace EA::Text;
	using namespace EA::StdC;

	// Need to tell EAText how to allocate memory. Any ICoreAllocator will do. 
	CoreAllocatorMalloc coreAllocator;
	EA::Text::SetAllocator(&coreAllocator);

	// EAText main init.
	EA::Text::Init();

	{
		// Create and init a FontServer. Usually you would have one of these for an app.
		// Set the global font server to our FontServer, so anybody that wants to 
		// find it can get it from a central location.
		FontServer fontServer;
		fontServer.Init();
		EA::Text::SetFontServer(&fontServer);


		// You will probably want to tell the FontServer what the GlyphCache is if you
		// are going to be creating bitmapped fonts (BmpFont objects).
		// fontServer.SetDefaultGlyphCache(pSomeGlyphCache);


		// Add fonts to the FontServer.
		#if defined(EA_PLATFORM_WINDOWS)
			// Add some fonts from disk file sources.
			FilePathChar pFontDirectory[EA::IO::kMaxPathLength];
			FilePathChar pFontPath[EA::IO::kMaxPathLength];

			EA::Text::GetSystemFontDirectory(pFontDirectory, EA::IO::kMaxPathLength);

			// Add Arial normal
			Strcpy(pFontPath, pFontDirectory);
			Strcat(pFontPath, EATEXT_FILE_PATH_CHAR("arial.ttf"));
			fontServer.AddFace(pFontPath);

			// Add Arial bold
			Strcpy(pFontPath, pFontDirectory);
			Strcat(pFontPath, EATEXT_FILE_PATH_CHAR("arialb.ttf"));
			fontServer.AddFace(pFontPath);

			// Add Courier New normal
			Strcpy(pFontPath, pFontDirectory);
			Strcat(pFontPath, EATEXT_FILE_PATH_CHAR("cour.ttf"));
			fontServer.AddFace(pFontPath);
		#endif

		// Add a font face from a memory image.
		EA::IO::MemoryStream* const pMemoryStream = new EA::IO::MemoryStream(const_cast<unsigned char*>(gTrueTypeFont), kTrueTypeFontSize, true, false);
		pMemoryStream->AddRef();
		fontServer.AddFace(pMemoryStream, kFontTypeOutline);
		pMemoryStream->Release();

		// Add a Font directly to the FontServer.
		OutlineFont* pOutlineFont = new OutlineFont; 
		pOutlineFont->AddRef();
		pOutlineFont->Open(gTrueTypeFont, kTrueTypeFontSize);
		pOutlineFont->SetTransform(16);
		fontServer.AddFont(pOutlineFont);
		pOutlineFont->Release();


		// Enumerate the existing font faces registered with the FontServer.
		FontDescription fd[16];
		const uint32_t faceCount = fontServer.EnumerateFonts(fd, 16);
		(void)faceCount;
		// Do something with the FontDescription items.


		// Do a simple FontServer lookup.
		TextStyle ts;
		Strcpy(ts.mFamilyNameArray[0], EATEXT_CHAR("Arial"));
		ts.mfSize = 16;
		ts.mStyle = kStyleItalic;

		Font* pFont = fontServer.GetFont(&ts, NULL); // GetFont AddRefs any fonts it returns.
		// Use pFont here.
		pFont->Release();

		// Do a multi-font lookup, whereby the best matches are given.
		FontSelection fontSelection;
		Strcpy(ts.mFamilyNameArray[0], EATEXT_CHAR("Arial"));
		Strcpy(ts.mFamilyNameArray[1], EATEXT_CHAR("Courier New"));

		fontServer.GetFont(&ts, fontSelection);
		// There will be two fonts in fontSelection.
		// fontSelection will auto-release its fonts.

		// Shutdown
		EA::Text::SetFontServer(NULL);
		fontServer.Shutdown();
	}

	// EAText main shutdown.
	EA::Text::Shutdown();

	return 0;
}





