///////////////////////////////////////////////////////////////////////////////
// DemoTypesetter.cpp
//
// Copyright (c) 2007 Criterion Software Limited and its licensors. All Rights Reserved.
// By Paul Pedriana
///////////////////////////////////////////////////////////////////////////////


#include "Demo.h"
#include <EAText/EATextStyle.h>
#include <EAText/EATextCache.h>
#include <EAText/EATextOutlineFont.h>
#include <EAText/EATextTypesetter.h>
#include <EAIO/EAStreamMemory.h>
#include <EAStdC/EAString.h>
#include <coreallocator/icoreallocator_interface.h>
#include <MemoryMan/MemoryMan.h>
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
// MainDemoTypesetter
//
// Does some basic demonstration of Typesetter. There are a lot of ways to use
// Typesetter and it would take a chapter of a book to describe them all.
// So we stick with just the basics here.
// 
int MainDemoTypesetter(int, char**)
{
	using namespace EA::Text;

	// Need to tell EAText how to allocate memory. Any ICoreAllocator will do. 
	CoreAllocatorMalloc coreAllocator;
	EA::Text::SetAllocator(&coreAllocator);

	// EAText main init.
	EA::Text::Init();

	// Create a GlyphCache for the rest of this app. Usually you create one of 
	// these on startup and use it for the entire app.
	// Usually you will create a GlyphCache that is specialized to your 
	// graphics system. But EAText provides a default memory cache that is 
	// useful for testing and no-op graphics drivers.
	GlyphCache* const pGlyphCache = new GlyphCache_Memory;
	pGlyphCache->Init(8, 0);


	// Create a FontServer for the rest of this app. Usually you create one of 
	// these on startup and use it for the entire app.
	FontServer fontServer;
	fontServer.Init();
	EA::Text::SetFontServer(&fontServer);

	// Ensure there is at least one face available in the font server
	EA::IO::MemoryStream* pMemoryStream = EA_NEW("EAText/MemoryStream") EA::IO::MemoryStream((void*)gTrueTypeFont, kTrueTypeFontSize, true, false, GetAllocator());
	fontServer.AddFace(pMemoryStream, kFontTypeOutline);

	// You will probably want to tell the FontServer what the GlyphCache is if you
	// are going to be creating bitmapped fonts (BmpFont objects).
	fontServer.SetDefaultGlyphCache(pGlyphCache);

	// Add a Font directly to the FontServer.
	OutlineFont* pOutlineFont = new OutlineFont; 
	pOutlineFont->AddRef();
	pOutlineFont->Open(gTrueTypeFont, kTrueTypeFontSize);
	pOutlineFont->SetTransform(16);
	fontServer.AddFont(pOutlineFont);
	pOutlineFont->Release();


	// Demonstrate LayoutSimple, which is useful for drawing (non-localized) debug text.
	// Note that we don't use the FontServer for this, but directly use a Font.
	{
		// Create a font.
		OutlineFont font; 
		font.AddRef(); // We're using a refcounted object on the stack. So AddRef it.
		font.Open(gTrueTypeFont, kTrueTypeFontSize);
		font.SetTransform(12);

		// Layout some text.
		LineLayout lineLayout;
		LayoutSimple(EATEXT_CHAR("Hello world"), 11, 0, 0, &font, lineLayout);

		// Now you would take the LineLayout and write it to your vertex buffer system.
		// This is much like how the GlyphMesh class does things in the EATextRNA package.
	}


	// Demonstrate the high level layout functionality
	{
		Typesetter  ts;
		LineLayout& lineLayout = ts.GetLineLayout();
		TextStyle   textStyle;

		EA::StdC::Strcpy(textStyle.mFamilyNameArray[1], EATEXT_CHAR("Arial"));
		textStyle.mfSize = 12;

		ts.LayoutLine(EATEXT_CHAR("Hello World"), 11, 150, 150, &textStyle);
		// Use lineLayout. Possibly give it to a GlyphMesh class which reads LineLayout info.
		// lineLayout has glyph info and metrics info.

		ts.LayoutParagraph(EATEXT_CHAR("Four score and seven years ago our forefathers ..."), 11, 150, 150, 200, 200, &textStyle, lineLayout);
		// Use lineLayout. Possibly give it to a GlyphMesh class which reads LineLayout info.
	}

	fontServer.Shutdown();
	
	pGlyphCache->Shutdown();
	delete pGlyphCache;

	// EAText main shutdown.
	EA::Text::Shutdown();

	return 0;
}


