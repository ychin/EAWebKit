///////////////////////////////////////////////////////////////////////////////
// DemoStyleManager.cpp
//
// Copyright (c) 2007 Criterion Software Limited and its licensors. All Rights Reserved.
// By Paul Pedriana
///////////////////////////////////////////////////////////////////////////////


#include "Demo.h"
#include <EAText/EATextStyle.h>
#include <EAStdC/EAString.h>
#include <coreallocator/icoreallocator_interface.h>


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
// MainDemoStyleManager
//
// The purpose of a StyleManager is to allow the management of how fonts look
// within an application via style definition instead of hard-coded font
// specifications. This is how professional word processing and publishing
// works and makes the maintenance of application fonts easier and more flexible.
// EAText provides a style management system that is much aligned with the 
// CSS standard. You don't need this StyleManager in order to use EAText.
//
int MainDemoStyleManager(int, char**)
{
	using namespace EA::Text;
	using namespace EA::StdC;

	// Need to tell EAText how to allocate memory. Any ICoreAllocator will do. 
	CoreAllocatorMalloc coreAllocator;
	EA::Text::SetAllocator(&coreAllocator);


	StyleManager styleManager;
	TextStyle    ts;

	// Create an Arial Italic 16, and give it a style id of 0x11111111
	Strcpy(ts.mFamilyNameArray[0], EATEXT_CHAR("Arial"));
	ts.mfSize  = 16;
	ts.mStyle  = kStyleItalic;
	ts.mSmooth = kSmoothNone;
	styleManager.AddStyle(0x11111111, ts);


	// Create an Arial 10, and give it a style id of 0x22222222
	Strcpy(ts.mFamilyNameArray[0], EATEXT_CHAR("Arial"));
	ts.mfSize  = 10;
	ts.mStyle  = kStyleNormal;
	ts.mSmooth = kSmoothNone;
	styleManager.AddStyle(0x22222222, ts);


	// Create an Russell Square 20 smoothed, and give it a style id of 0x33333333.
	// Make it so that Arial Unicode MS is a backup font for this style so that 
	// Asian characters which are supported by Russell Square can be supported by Arial Unicode MS.
	Strcpy(ts.mFamilyNameArray[0], EATEXT_CHAR("Russell Square"));
	Strcpy(ts.mFamilyNameArray[1], EATEXT_CHAR("Arial Unicode MS"));
	ts.mfSize  = 20;
	ts.mStyle  = kStyleNormal;
	ts.mSmooth = kSmoothEnabled;
	styleManager.AddStyle(0x33333333, ts);


	// Create and register styles from a text description (usually part of game resource).
	const char* pMultiStyle =
		"   Helv10(0x00000001){                         \n"
		"      font-family: \"Helvetica Neue\", arial;  \n"
		"      font-size: 10px;                         \n"
		"   }                                           \n"
		"                                               \n"
		"   Helv12{                                     \n"
		"      font-family: \"Helvetica Neue\", arial;  \n"
		"      font-size: 12px;                         \n"
		"   }                                           \n"
		"                                               \n"
		"   Helv14(0x00000003) : Style1{                \n"
		"      font-family: \"Helvetica Neue\", arial;  \n"
		"      font-size: 14px;                         \n"
		"   }                                           \n";

	char      pStyleName[24][32];
	uint32_t  pIdArray[24];
	TextStyle ssCSSArray[24];

	memset(pStyleName, 0, sizeof(pStyleName));
	memset(pIdArray,   0, sizeof(pIdArray));
	EA::Text::ParseStyleText(pMultiStyle, pStyleName, pIdArray, ssCSSArray, 24, &styleManager); 


	// Retrieve a style based on its id.
	const TextStyle* pStyle = styleManager.GetStyle(0x22222222);
	(void)pStyle;


	return 0;
}


