///////////////////////////////////////////////////////////////////////////////
// DemoBaseline.cpp
//
// Copyright (c) 2007 Criterion Software Limited and its licensors. All Rights Reserved.
// By Paul Pedriana
///////////////////////////////////////////////////////////////////////////////


#include "Demo.h"
#include <EAText/EATextOutlineFont.h>
#include <EAText/EATextTypesetter.h>
#include <EAText/EATextBaseline.h>
#include <coreallocator/icoreallocator_interface.h>
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



int MainDemoBaseline(int, char**)
{
	using namespace EA::Text;

	// Need to tell EAText how to allocate memory. Any ICoreAllocator will do. 
	CoreAllocatorMalloc coreAllocator;
	EA::Text::SetAllocator(&coreAllocator);

	// EAText main init.
	EA::Text::Init();

	// Demonstrate a line of text following a Bezier curve.
	{
		// Create a font.
		OutlineFont font; 
		font.AddRef(); // We're using a refcounted object on the stack. So AddRef it.
		font.Open(gTrueTypeFont, kTrueTypeFontSize);
		font.SetTransform(12);

		// Layout some text.
		LineLayout lineLayout;
		LayoutSimple(EATEXT_CHAR("Hello world"), 11, 0, 0, &font, lineLayout);

		// Curve the text.
		Bezier2DIterator  it;
		BaselineLayout    baselineLayout;
		GlyphLayoutInfoEx gliArray[16];

		it.MakeSemicircle(Point(0, 0), 100);
		baselineLayout.FollowBaseline(&lineLayout, gliArray, &it, 0.f, 0.f, kHALeft, kDirectionLTR);

		// Now you would take the lineLayout and gliArray and write it to your 
		// vertex buffer system. The EATextRNA package has an example of this.
	}

	// EAText main shutdown.
	EA::Text::Shutdown();

	return 0;
}


