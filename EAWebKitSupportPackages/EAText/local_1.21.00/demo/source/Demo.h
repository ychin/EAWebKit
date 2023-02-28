///////////////////////////////////////////////////////////////////////////////
// Demo.h
//
// Copyright (c) 2007 Criterion Software Limited and its licensors. All Rights Reserved.
// By Paul Pedriana
///////////////////////////////////////////////////////////////////////////////


#include <stddef.h>
#include <EASTL/string.h>
#include <EAText/EAText.h>


/// TestString
///
/// An eastl string of EA::Text::Char.
///
typedef eastl::basic_string<EA::Text::Char> TestString;


// The following is a .ttf font image as a C char array. 
// This allows for simple embedding of a .ttf file into a C++ program.
const size_t kTrueTypeFontSize = 26560;
extern const unsigned char gTrueTypeFont[kTrueTypeFontSize];


// Each of these acts like a standalone application. It's simpler for us 
// to implement them as part of a single app, for project management.
int MainDemoBaseline(int argc, char** argv);
int MainDemoCompression(int argc, char** argv);
int MainDemoFontServer(int argc, char** argv);
int MainDemoStyleManager(int argc, char** argv);
int MainDemoTextBreak(int argc, char** argv);
int MainDemoTypesetter(int argc, char** argv);
int MainDemoUnicode(int argc, char** argv);
int MainDemoTextEdit(int argc, char** argv);
int MainDemoEffects(int argc, char** argv);






