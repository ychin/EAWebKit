/*
Copyright (C) 2008, 2009, 2010, 2011, 2012, 2014 Electronic Arts, Inc.  All rights reserved.

Redistribution and use in source and binary forms, with or without
modification, are permitted provided that the following conditions
are met:

1.  Redistributions of source code must retain the above copyright
    notice, this list of conditions and the following disclaimer.
2.  Redistributions in binary form must reproduce the above copyright
    notice, this list of conditions and the following disclaimer in the
    documentation and/or other materials provided with the distribution.
3.  Neither the name of Electronic Arts, Inc. ("EA") nor the names of
    its contributors may be used to endorse or promote products derived
    from this software without specific prior written permission.

THIS SOFTWARE IS PROVIDED BY ELECTRONIC ARTS AND ITS CONTRIBUTORS "AS IS" AND ANY
EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE IMPLIED
WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE ARE
DISCLAIMED. IN NO EVENT SHALL ELECTRONIC ARTS OR ITS CONTRIBUTORS BE LIABLE FOR ANY
DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES
(INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES;
LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND
ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT
(INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE OF
THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
*/

///////////////////////////////////////////////////////////////////////////////
// EAWebKitInput.h
// Created by Paul Pedriana
// Maintained by EAWebKit Team
///////////////////////////////////////////////////////////////////////////////


#ifndef EAWEBKIT_EAWEBKITINPUT_H
#define EAWEBKIT_EAWEBKITINPUT_H


#include <EAWebKit/EAWebKitConfig.h>
#include <EABase/eabase.h>


namespace EA
{
namespace WebKit
{
class  View;

struct KeyboardEvent
{
    uint32_t    mId;            /// Windows/InputMan key codes are expected.
	uint32_t	mModifiers;		/// Mask indicating depressed modifier keys.
	bool        mbChar;         /// If true, this is a Unicode char; if false, it is a physical key (e.g. VK_F1).
    bool        mbDepressed;    /// True for key down, false for key up.

	KeyboardEvent(uint32_t id = 0, bool bChar = false, bool bDepressed = false, uint32_t modifiers = 0)
		: mId(id)
		, mModifiers(modifiers)
		, mbChar(bChar)
		, mbDepressed(bDepressed)
		
	{
	}
};

// Mouse button identifiers
const uint32_t kMouseLeft    = 0;  // We have 'mouse' in the identifier name so to not get confused with keyboard left arrow key.
const uint32_t kMouseMiddle  = 1;  // For some mice the mouse wheel acts as a third button.
const uint32_t kMouseRight   = 2;
const uint32_t kMouseX1      = 3;  // This is the button sometimes found on the left side of the mouse, used in web browsers to
const uint32_t kMouseX2      = 4;  // go backward and forward through the page history.
const uint32_t kMouseNoButton = 0xffffffff; // This matches with internal WebCore(name and value).


struct MouseButtonEvent
{
    uint32_t    mId;            /// This is the button, such as kMouseLeft.
	int32_t     mX;             /// Current x position, relative to the View origin.
    int32_t     mY;             /// Current y position, relative to the View origin.
	int32_t     mGlobalX;       /// Current x position, Global(Top left corner of screen(computer monitor) is 0). Currently unused.
	int32_t     mGlobalY;       /// Current y position, Global(Top left corner of screen(computer monitor) is 0). Currently unused.
	uint32_t	mModifiers;		/// Mask indicating depressed modifier keys.
	uint32_t	mClickCount;	/// 1 for single click, 2 for double click.
	bool        mbDepressed;    /// True for button down, false for button up.

	MouseButtonEvent(uint32_t id = kMouseNoButton, bool bDepressed = false, uint32_t clickCount = 1
					, int32_t x = -1, int32_t y = -1, int32_t globalX = -1, int32_t globalY = -1
					, uint32_t modifiers = 0)
					: mId(id)
					, mX(x), mY(y), mGlobalX(globalX), mGlobalY(globalY)
					, mModifiers(modifiers)
					, mClickCount(clickCount), mbDepressed(bDepressed)
	{
	}

};

struct MouseMoveEvent
{
	uint32_t    mId;            /// This is the button, such as kMouseLeft. Use this only if the mouse is moved with this button pressed.
	int32_t     mX;             /// Current x position, relative to the View origin.
	int32_t     mY;             /// Current y position, relative to the View origin.
	int32_t     mGlobalX;       /// Current x position, Global(Top left corner of screen(computer monitor) is 0). Currently unused.
	int32_t     mGlobalY;       /// Current y position, Global(Top left corner of screen(computer monitor) is 0). Currently unused.
	uint32_t	mModifiers;		/// Mask indicating depressed modifier keys.
	MouseMoveEvent(uint32_t id = kMouseNoButton, int32_t x = -1, int32_t y = -1
					, int32_t globalX = -1, int32_t globalY = -1, uint32_t modifiers = 0)
					: mId(id), mX(x), mY(y)
					, mGlobalX(globalX), mGlobalY(globalY), mModifiers(modifiers)
	{

	}

};

struct MouseWheelEvent
{
	// zDelta is the pure, unadulterated, distance moved forward (+ve) or
	// backward (-ve) by the mouse wheel.  For a traditional mouse wheel,
	// this will be in steps of WHEEL_DELTA, or 120, for each notch.
	// Other mouse wheels may have finer resolution, however.
	//
	// Both zDelta and lineDelta are passed because WebKit needs both.
	int32_t     mZDelta;        

	// lineDelta can be obtained from the system settings for the user.
	// Something like following on Windows.
	// UINT scrollLines = 1;
	// SystemParametersInfoA(SPI_GETWHEELSCROLLLINES, 0, &scrollLines, 0);
	// lineDelta = ((zDelta * (int32_t)scrollLines) / (int32_t)WHEEL_DELTA);
	//
    float       mNumLines;     
	int32_t     mX;             /// Current x position, relative to the View origin.
	int32_t     mY;             /// Current y position, relative to the View origin.
	int32_t     mGlobalX;       /// Current x position, Global(Top left corner of screen(computer monitor) is 0). Currently unused.
	int32_t     mGlobalY;       /// Current y position, Global(Top left corner of screen(computer monitor) is 0). Currently unused.
	uint32_t	mModifiers;		/// Mask indicating depressed modifier keys.

	MouseWheelEvent(int32_t zDelta = 0, float numLines = 0.f, int32_t x = -1, int32_t y = -1
					, int32_t globalX = -1, int32_t globalY = -1, uint32_t modifiers = 0)
					: mZDelta(zDelta), mNumLines(numLines), mX(x), mY(y)
					, mGlobalX(globalX), mGlobalY(globalY), mModifiers(modifiers)
	{
	}
};

// We can extend following structs with more information as needed.
struct ButtonEvent
{
	uint32_t	mID;			/// Button id, such as kButton3
	uint16_t	mRepeatCount;	/// Repeat count. Will be in the range of [(uint16_t)-1, UINT16_MAX - 2].

	//Need a Ctor because I want to clear the "mTag" in case no tag is specified and a default needs to be used.
	ButtonEvent(uint32_t id = 0, uint16_t repeatCount = 1)
		: mID(id)
		, mRepeatCount(repeatCount)
	{

	}
};

// Note that the following is a diversion from InputMan. InputMan has a problem that it only deals with 1 axis at a time and associates the id with the stick axis.
// We associate the mID with stick ID(kStick0,...,kStick3).
struct StickEvent
{
	uint32_t	mID;			/// Stick or axis id, such as kStick0.
	float		mXAxis;			/// Float   (-1.0 - +1.0)
	float		mYAxis;			/// Float   (-1.0 - +1.0)
	uint16_t	mRepeatCount;	/// Repeat count. Will be in the range of [(uint16_t)-1, UINT16_MAX - 2]. This can be used to accelerate scroll/cursor movement.

	//Need a Ctor because I want to clear the "mTag" in case no tag is specified and a default needs to be used.
	StickEvent(uint32_t id = 0, float xAxis = 0.0f, float yAxis = 0.0f, uint16_t repeatCount = 1)
		: mID(id)
		, mXAxis(xAxis)
		, mYAxis(yAxis)
		, mRepeatCount(repeatCount)
	{

	}
};






// Keyboard modifier masks
// These are masks that are defined for modifier flags when you might 
// need to 'or' them together. For example, if you have a GUI system and you 
// want to pass around messages that tell the user what modifier keys are currently 
// set, you'd use these. You can always write a system that passes around individual 
// key defines if you want, in which case you'd use the values below.
//
const uint32_t kModifierMaskNone       = 0x0000;  // Empty mask.
const uint32_t kModifierMaskAny        = 0x03ff;  // This is a special value that means the user will accept any modifier mask. 
const uint32_t kModifierMaskShift      = 0x0001;  // Shift key.
const uint32_t kModifierMaskControl    = 0x0002;  // Control key.
const uint32_t kModifierMaskAlt        = 0x0004;  // Alt key.
const uint32_t kModifierMaskOption     = 0x0008;  // Option key (Macintosh). AltGr on European keyboards. 
const uint32_t kModifierMaskOS         = 0x0010;  // Apple key (Macintosh). 
const uint32_t kModifierMaskCapsLock   = 0x0020;  // 'Lock' keys don't have to be physically depressed in 
const uint32_t kModifierMaskNumLock    = 0x0040;  //    order to be enabled. Their enabled state is toggled 
const uint32_t kModifierMaskScrollLock = 0x0080;  //    on and off by pressing the keys.
const uint32_t kModifierMaskCompose    = 0x0100;  //
const uint32_t kModifierMaskKana       = 0x0200;  //


// Modifier keys
// Note that we have a singular version (e.g. kShift) and a specific 
// version (kLShift / kRShift). This is for convenience, though it is 
// somewhat for backwards compatibility as well. The idea is that in most
// of your GUI system you want to be able to tell if a shift key simply
// was pressed. So in this case if any of the shift keys is pressed, you get a 
// message of simply kShift. However, you may want to specifically check for
// a particular shift key itself being depressed. This is uncommon but plausible.
// In this case we provide separate defines for this. So if your system has
// a function called CheckIfKeyIsDown, it can check for the specific exact key.
//
const uint32_t kShift           =  16;  // 0x10     Either of the shift keys.
const uint32_t kLShift          = 160;  // 0xA0     The left shift key in particular.
const uint32_t kRShift          = 161;  // 0xA1     The right shift key in particular.

const uint32_t kControl         =  17;  // 0x11     Either of the control keys.
const uint32_t kLControl        = 162;  // 0xA2     The left control key in particular.
const uint32_t kRControl        = 163;  // 0xA3     The right control key in particular.

const uint32_t kAlt             =  18;  // 0x12     Either of the alt keys.
const uint32_t kLAlt            = 164;  // 0xA4     The left alt key in particular.
const uint32_t kRAlt            = 165;  // 0xA5     The right alt key in particular.

const uint32_t kOption          = 225;  // 0xE1     Either of the (Macintosh) option keys.
const uint32_t kLOption         = 226;  // 0xE2     The left option key in particular.
const uint32_t kROption         = 227;  // 0xE3     The right option key in particular.

const uint32_t kOS              =  91;  // 0x5B     The Windows key on Windows; the Apple key on MacOS.
const uint32_t kLOS             =  91;  // 0x5B     
const uint32_t kROS             =  92;  // 0x5C     

// Toggle keys
// These are keys which have non-transient state associated with them.
// As you know, when CapsLock is enabled, a keyboard light is on and 
// characters are generated differently as a result of key presses.
const uint32_t kCapsLock        =  20;  // 0x14     Caps Lock key.
const uint32_t kNumLock         = 144;  // 0x90     Num Lock key.
const uint32_t kScrollLock      = 145;  // 0x91     Scroll Lock key.

// Digit keys. 
// kDigit0 thru kDigit9 are the same as ASCII '0' thru '9' (0x30 - 0x39)
const uint32_t kDigit0          =  48;  // 0x30     The '0' digit key.
const uint32_t kDigit1          =  49;  // 0x31     The '1' digit key. 
const uint32_t kDigit2          =  50;  // 0x32     The '2' digit key. 
const uint32_t kDigit3          =  51;  // 0x33     The '3' digit key. 
const uint32_t kDigit4          =  52;  // 0x34     The '4' digit key. 
const uint32_t kDigit5          =  53;  // 0x35     The '5' digit key. 
const uint32_t kDigit6          =  54;  // 0x36     The '6' digit key. 
const uint32_t kDigit7          =  55;  // 0x37     The '7' digit key. 
const uint32_t kDigit8          =  56;  // 0x38     The '8' digit key. 
const uint32_t kDigit9          =  57;  // 0x39     The '9' digit key. 

// Letter keys.
// kLetterA thru kLetterZ are the same as ASCII 'A' thru 'Z' (0x41 - 0x5A)
const uint32_t kLetterA         =  65;  // 0x41     The 'A' key.
const uint32_t kLetterB         =  66;  // 0x42     The 'B' key. 
const uint32_t kLetterC         =  67;  // 0x43     The 'C' key. 
const uint32_t kLetterD         =  68;  // 0x44     The 'D' key. 
const uint32_t kLetterE         =  69;  // 0x45     The 'E' key. 
const uint32_t kLetterF         =  70;  // 0x46     The 'F' key. 
const uint32_t kLetterG         =  71;  // 0x47     The 'G' key. 
const uint32_t kLetterH         =  72;  // 0x48     The 'H' key. 
const uint32_t kLetterI         =  73;  // 0x49     The 'I' key. 
const uint32_t kLetterJ         =  74;  // 0x4A     The 'J' key. 
const uint32_t kLetterK         =  75;  // 0x4B     The 'K' key.
const uint32_t kLetterL         =  76;  // 0x4C     The 'L' key. 
const uint32_t kLetterM         =  77;  // 0x4D     The 'M' key. 
const uint32_t kLetterN         =  78;  // 0x4E     The 'N' key. 
const uint32_t kLetterO         =  79;  // 0x4F     The 'O' key. 
const uint32_t kLetterP         =  80;  // 0x50     The 'P' key. 
const uint32_t kLetterQ         =  81;  // 0x51     The 'Q' key. 
const uint32_t kLetterR         =  82;  // 0x52     The 'R' key. 
const uint32_t kLetterS         =  83;  // 0x53     The 'S' key. 
const uint32_t kLetterT         =  84;  // 0x54     The 'T' key. 
const uint32_t kLetterU         =  85;  // 0x55     The 'U' key.
const uint32_t kLetterV         =  86;  // 0x56     The 'V' key. 
const uint32_t kLetterW         =  87;  // 0x57     The 'W' key. 
const uint32_t kLetterX         =  88;  // 0x58     The 'X' key. 
const uint32_t kLetterY         =  89;  // 0x59     The 'Y' key. 
const uint32_t kLetterZ         =  90;  // 0x5A     The 'Z' key. 

// Miscellaneous keys
const uint32_t kInvalid         =   0;  // 0x00     Refers not to a key, but instead acts as a placeholder for an invalid key id.
const uint32_t kBackspace       =   8;  // 0x08     Backspace key.
const uint32_t kTab             =   9;  // 0x09     The tab key.
const uint32_t kNewLine         =  10;  // 0x0A     Some platforms treat this as Enter key on the keyboard.
const uint32_t kCarriageReturn  =  13;  // 0x0D     Some platforms treat this as Enter key on the keyboard.
const uint32_t kPause           =  19;  // 0x13     The pause (a.k.a break) key, as returned by Windows without control depressed.
const uint32_t kBreak           =   3;  // 0x03     The break (a.k.a pause) key, as returned by Windows with control depressed.
const uint32_t kEscape          =  27;  // 0x1B     The esc key.
const uint32_t kSpacebar        =  32;  // 0x20     The spacebar key.
const uint32_t kPageUp          =  33;  // 0x21     The Page up key.
const uint32_t kPageDown        =  34;  // 0x22     The Page down key.
const uint32_t kEnd             =  35;  // 0x23     The End key.
const uint32_t kHome            =  36;  // 0x24     The Home key.
const uint32_t kArrowLeft       =  37;  // 0x25     The left arrow key.
const uint32_t kArrowUp         =  38;  // 0x26     The up arrow key.
const uint32_t kArrowRight      =  39;  // 0x27     The right arrow key.
const uint32_t kArrowDown       =  40;  // 0x28     The down arrow key.
const uint32_t kPrintScreen     =  44;  // 0x2C     The PrintScrn (print screen) key.
const uint32_t kInsert          =  45;  // 0x2D     The insert key.
const uint32_t kDelete          =  46;  // 0x2E     The delete key.
const uint32_t kHelp            =  47;  // 0x2F     The help key, if there is one.
const uint32_t kSleep           =  95;  // 0x5F     The computer sleep key, if it has one.
const uint32_t kApp             =  93;  // 0x5D     Windows-specific: The "applications" key. 

// Numpad keys
const uint32_t kNumpadCenter    =  12;  // 0x0C     The key in the center of the numpad (with '5' on it) when numlock is turned off.
const uint32_t kNumpadEnter     =  14;  // 0x0E     The number pad enter key.
const uint32_t kNumpad0         =  96;  // 0x60     The number pad '0' key.
const uint32_t kNumpad1         =  97;  // 0x61     The number pad '1' key.
const uint32_t kNumpad2         =  98;  // 0x62     The number pad '2' key.
const uint32_t kNumpad3         =  99;  // 0x63     The number pad '3' key.
const uint32_t kNumpad4         = 100;  // 0x64     The number pad '4' key.
const uint32_t kNumpad5         = 101;  // 0x65     The number pad '5' key.
const uint32_t kNumpad6         = 102;  // 0x66     The number pad '6' key.
const uint32_t kNumpad7         = 103;  // 0x67     The number pad '7' key.
const uint32_t kNumpad8         = 104;  // 0x68     The number pad '8' key.
const uint32_t kNumpad9         = 105;  // 0x69     The number pad '9' key.
const uint32_t kNumpadMultiply  = 106;  // 0x6A     The number pad '*' key.
const uint32_t kNumpadPlus      = 107;  // 0x6B     The number pad '+' key.
const uint32_t kNumpadMinus     = 109;  // 0x6D     The number pad '-' key.
const uint32_t kNumpadDecimal   = 110;  // 0x6E     The number pad '.' key.
const uint32_t kNumpadDivide    = 111;  // 0x6F     The number pad '/' key.

const uint32_t kF1              = 112;  // 0x70     The F1 key.
const uint32_t kF2              = 113;  // 0x71     The F2 key.
const uint32_t kF3              = 114;  // 0x72     The F3 key.
const uint32_t kF4              = 115;  // 0x73     The F4 key.
const uint32_t kF5              = 116;  // 0x74     The F5 key.
const uint32_t kF6              = 117;  // 0x75     The F6 key.
const uint32_t kF7              = 118;  // 0x76     The F7 key.
const uint32_t kF8              = 119;  // 0x77     The F8 key.
const uint32_t kF9              = 120;  // 0x78     The F9 key.
const uint32_t kF10             = 121;  // 0x79     The F10 key.
const uint32_t kF11             = 122;  // 0x7A     The F11 key.
const uint32_t kF12             = 123;  // 0x7B     The F12 key.
const uint32_t kF13             = 124;  // 0x7C     The F13 key.
const uint32_t kF14             = 125;  // 0x7D     The F14 key.
const uint32_t kF15             = 126;  // 0x7E     The F15 key.
const uint32_t kF16             = 127;  // 0x7F     The F16 key.
const uint32_t kF17             = 128;  // 0x80     The F17 key.
const uint32_t kF18             = 129;  // 0x81     The F18 key.
const uint32_t kF19             = 130;  // 0x82     The F19 key.
const uint32_t kF20             = 131;  // 0x83     The F20 key.
const uint32_t kF21             = 132;  // 0x84     The F21 key.
const uint32_t kF22             = 133;  // 0x85     The F22 key.
const uint32_t kF23             = 134;  // 0x86     The F23 key.
const uint32_t kF24             = 135;  // 0x87     The F24 key.

// Other miscellaneous keyboard keys.
const uint32_t kSemicolon       = 186;  // 0xBA     The ';' key.             // May be different on non-US keyboards.
const uint32_t kEquals          = 187;  // 0xBB     The '=' key.
const uint32_t kComma           = 188;  // 0xBC     The ',' key.
const uint32_t kMinus           = 189;  // 0xBD     The non-numpad '-' key.
const uint32_t kPeriod          = 190;  // 0xBE     The non-numpad '.' key.
const uint32_t kForwardSlash    = 191;  // 0xBF     The non-numpad '/' key.  // May be different on non-US keyboards.
const uint32_t kBackSlash       = 220;  // 0xDC     The '\' key.             // May be different on non-US keyboards.
const uint32_t kAccent          = 192;  // 0xC0     The '`' key.             // May be different on non-US keyboards.
const uint32_t kOpenBracket     = 219;  // 0xDB     The '[' key.             // May be different on non-US keyboards.
const uint32_t kCloseBracket    = 221;  // 0xDD     The ']' key.             // May be different on non-US keyboards.
const uint32_t kSingleQuote     = 222;  // 0xDE     The ''' key.             // May be different on non-US keyboards.
const uint32_t kSection         = 223;  // 0xDF     The section (§) key on some foreign keyboards, such as French.
const uint32_t kHalf            = 167;  // 0xA7     The half (½) key found on some foreign keyboards, such as Swedish.
const uint32_t kSeparator       = 108;  // 0x6C     This is not actually generated by keyboards

const uint32_t kSelect          =  41;  // 0x29
const uint32_t kPrint           =  42;  // 0x2A
const uint32_t kExecute         =  43;  // 0x2B

const uint32_t kBrowserBack     = 166;  // 0xA6
const uint32_t kBrowserForward  = 167;  // 0xA7
const uint32_t kBrowserRefresh  = 168;  // 0xA8
const uint32_t kBrowserStop     = 169;  // 0xA9
const uint32_t kBrowserSearch   = 170;  // 0xAA
const uint32_t kBrowserFavorite = 171;  // 0xAB
const uint32_t kBrowserHome     = 172;  // 0xAC

const uint32_t kVolumeMute      = 173;  // 0xAD
const uint32_t kVolumeDown      = 174;  // 0xAE
const uint32_t kVolumeUp        = 175;  // 0xAF

const uint32_t kMediaNext       = 176;  // 0xB0
const uint32_t kMediaPrev       = 177;  // 0xB1
const uint32_t kMediaStop       = 178;  // 0xB2
const uint32_t kMediaPause      = 179;  // 0xB3

const uint32_t kLaunchMail      = 180;  // 0xB4
const uint32_t kMediaSelect     = 181;  // 0xB5
const uint32_t kApp1            = 182;  // 0xB6
const uint32_t kApp2            = 183;  // 0xB7

const uint32_t kOEM1            = 186;  // 0xBA
const uint32_t kOEM2            = 191;  // 0xBF
const uint32_t kOEM3            = 192;  // 0xC0
const uint32_t kOEM4            = 219;  // 0xDB
const uint32_t kOEM5            = 220;  // 0xDC
const uint32_t kOEM6            = 221;  // 0xDD
const uint32_t kOEM7            = 222;  // 0xDE
const uint32_t kOEM8            = 223;  // 0xDF

const uint32_t kOEMPlus         = 187;  // 0xBB
const uint32_t kOEMComma        = 188;  // 0xBC
const uint32_t kOEMMinus        = 189;  // 0xBD
const uint32_t kOEMPeriod       = 190;  // 0xBE
const uint32_t kOEMAx           = 225;  // 0xE1
const uint32_t kOEM102          = 226;  // 0xE2
const uint32_t kICOHelp         = 227;  // 0xE3
const uint32_t kICO00           = 228;  // 0xE4
const uint32_t kICOClear        = 230;  // 0xE6
const uint32_t kProcess         = 229;  // 0xE5
const uint32_t kPacket          = 231;  // 0xE7

// IME (input method editor) keys, used for typing in Asian symbolic characters
const uint32_t kIMEKana         =  21;  // 0x15     IME Kana mode            // Generally appears on Asian keyboards only.
const uint32_t kIMEHangul       =  21;  // 0x15     IME Hangul mode          // Generally appears on Asian keyboards only.
const uint32_t kIMEJunja        =  23;  // 0x17     IME Junja mode           // Generally appears on Asian keyboards only.
const uint32_t kIMEFinal        =  24;  // 0x18     IME final mode           // Generally appears on Asian keyboards only.
const uint32_t kIMEHanja        =  25;  // 0x19     IME Hanja mode           // Generally appears on Asian keyboards only.
const uint32_t kIMEKanji        =  25;  // 0x19     IME Kanji mode           // Generally appears on Asian keyboards only.
const uint32_t kIMEConvert      =  28;  // 0x1C     IME convert              // Generally appears on Asian keyboards only.
const uint32_t kIMENonConvert   =  29;  // 0x1D     IME nonconvert           // Generally appears on Asian keyboards only.
const uint32_t kIMEAccept       =  30;  // 0x1E     IME accept               // Generally appears on Asian keyboards only.
const uint32_t kIMEModeChange   =  31;  // 0x1F     IME mode change request  // Generally appears on Asian keyboards only.

// Stick (platform-independent)
// Note that some input devices have the concept of what they call a z axis
// to supplement the x and y axes. This is found on joysticks for flight simulation
// titles where the stick itself provides x and y input and there is a slider or
// trigger which supplies the z axis. We don't define an explicit z axis here but
// instead use the "second" stick x axis to provide z axis functionality. This makes
// some sense if you consider that usually these z axis devices are usually a slider
// that is independent from the physical stick.
//
const uint32_t kStick0X     = 2000;   // X axis of first stick.
const uint32_t kStick0Y     = 2001;   // Y axis of first stick.
const uint32_t kStick1X     = 2002;   // X axis of second stick.
const uint32_t kStick1Y     = 2003;   // Y axis of second stick.
const uint32_t kStick2X     = 2004;   // X axis of third stick.
const uint32_t kStick2Y     = 2005;   // Y axis of third stick.
const uint32_t kStick3X     = 2006;   // X axis of third stick.
const uint32_t kStick3Y     = 2007;   // Y axis of third stick.
const uint32_t kStickCount  = 8;      // We can expand this if necessary.

// Stick identifiers (platform-independent)
const uint32_t kStick0      = 2050;
const uint32_t kStick1      = 2051;
const uint32_t kStick2      = 2052;
const uint32_t kStick3      = 2053;

// Buttons (platform-independent)
const uint32_t kButton0     = 2008;
const uint32_t kButton1     = 2009;
const uint32_t kButton2     = 2010;
const uint32_t kButton3     = 2011;
const uint32_t kButton4     = 2012;
const uint32_t kButton5     = 2013;
const uint32_t kButton6     = 2014;
const uint32_t kButton7     = 2015;
const uint32_t kButton8     = 2016;
const uint32_t kButton9     = 2017;
const uint32_t kButton10    = 2018;
const uint32_t kButton11    = 2019;
const uint32_t kButton12    = 2020;
const uint32_t kButton13    = 2021;
const uint32_t kButton14    = 2022;
const uint32_t kButton15    = 2023;
const uint32_t kButton16    = 2024;

const uint32_t kButtonCount = 16;

// This class created by EA in order to provide a generic means to have some GUI entity
// own the keyboard and mouse focus in a Overlay way. This was initially created due to a
// need for this by the PopupMenu class. WebKit ports such as the Windows, Macintosh, 
// and Gtk ports implement popup menus by directly using operating system windowing 
// services, whereas we don't have such a thing on our platforms. So we emulate that 
// functionality with things like OverlayInputClient and private Raster surfaces like
// that used by PopupMenu.
// A class can get Overlay control of input by inheriting from this class, implementing
// the event functions, and using View::SetOverlayInput.
class IOverlayInputClient
{
	//abaldeva: We should probably have all the APIs return a bool. Currently, only OnButtonEvent returns a boolean as it indicates
	// whether the cursor jumped or not.
public:
	virtual     ~IOverlayInputClient(void) { }
	virtual void OverlayBegin(void) = 0;
	virtual void OverlayEnd(void) = 0;
	virtual void OnKeyboardEvent(const KeyboardEvent& keyboardEvent) = 0;
	virtual void OnMouseMoveEvent(const MouseMoveEvent& mouseMoveEvent) = 0;
	virtual void OnMouseButtonEvent(const MouseButtonEvent& mouseButtonEvent) = 0;
	virtual void OnMouseWheelEvent(const MouseWheelEvent& mouseWheelEvent) = 0;
	virtual bool OnButtonEvent(const ButtonEvent& buttonEvent) = 0;
	virtual void OnFocusChangeEvent(bool hasFocus) = 0;
};

} // namespace WebKit

} // namespace EA





#endif // Header include guard










