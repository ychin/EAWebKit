/////////////////////////////////////////////////////////////////////////////
// EATextTextEdit.cpp
//
// Written by Talin and Paul Pedriana.
// Copyright (c) 2005, Electronic Arts Inc. All rights reserved.
/////////////////////////////////////////////////////////////////////////////


#include "EATextTextEdit.h"
#include <EAText/EAText.h>
#include <EAText/EATextBreak.h>
#include <EAText/EATextTypesetter.h>
#include <EAStdC/EAString.h>
#include <EAStdC/EACType.h>
#include <coreallocator/icoreallocatormacros.h>
#include <math.h>   // VS2008 has an acknowledged bug that requires math.h (and possibly also string.h) to be #included before intrin.h, which EAMathHelp.h includes.
#include <EAStdC/EAMathHelp.h>
#include <EASTL/algorithm.h>
#include <UTFDraw2D/DrawContext.h>
#include EA_ASSERT_HEADER
#include <string.h>
#include <math.h>


#ifdef _MSC_VER
	#pragma warning(disable: 4244)  // conversion from 'const ptrdiff_t' to 'int32_t', possible loss of data. This is a bogus warning resulting from a bug in VC++.
#elif defined(__SNC__)
	#pragma control %push diag
	#pragma diag_suppress=178 // variable was declared but never referenced
#endif


namespace EA
{
	namespace Input
	{
		// This code is copied from the InputMan package.

	  //const uint32_t kModifierMaskNone       = 0x0000;  // Empty mask.
	  //const uint32_t kModifierMaskAny        = 0x03ff;  // This is a special value that means the user will accept any modifier mask. 
		const uint32_t kModifierMaskShift      = 0x0001;  // Shift key.
		const uint32_t kModifierMaskControl    = 0x0002;  // Control key.
		const uint32_t kModifierMaskAlt        = 0x0004;  // Alt key.
	  //const uint32_t kModifierMaskOption     = 0x0008;  // Option key (Macintosh). 
	  //const uint32_t kModifierMaskOS         = 0x0010;  // Apple key (Macintosh). 
	  //const uint32_t kModifierMaskCapsLock   = 0x0020;  // 'Lock' keys don't have to be physically depressed in 
	  //const uint32_t kModifierMaskNumLock    = 0x0040;  //    order to be enabled. Their enabled state is toggled 
	  //const uint32_t kModifierMaskScrollLock = 0x0080;  //    on and off by pressing the keys.

	  //const uint32_t kShift           =  16;  // 0x10     Either of the shift keys.
	  //const uint32_t kLShift          = 160;  // 0xA0     The left shift key in particular.
	  //const uint32_t kRShift          = 161;  // 0xA1     The right shift key in particular.

	  //const uint32_t kControl         =  17;  // 0x11     Either of the control keys.
	  //const uint32_t kLControl        = 162;  // 0xA2     The left control key in particular.
	  //const uint32_t kRControl        = 163;  // 0xA3     The right control key in particular.

	  //const uint32_t kAlt             =  18;  // 0x12     Either of the alt keys.
	  //const uint32_t kLAlt            = 164;  // 0xA4     The left alt key in particular.
	  //const uint32_t kRAlt            = 165;  // 0xA5     The right alt key in particular.

	  //const uint32_t kOption          = 225;  // 0xE1     Either of the (Macintosh) option keys.
	  //const uint32_t kLOption         = 226;  // 0xE2     The left option key in particular.
	  //const uint32_t kROption         = 227;  // 0xE3     The right option key in particular.

	  //const uint32_t kOS              =  91;  // 0x5B     The Windows key on Windows; the Apple key on MacOS.
	  //const uint32_t kLOS             =  91;  // 0x5B     
	  //const uint32_t kROS             =  92;  // 0x5C     

		// Toggle keys
		// These are keys which have non-transient state associated with them.
		// As you know, when CapsLock is enabled, a keyboard light is on and 
		// characters are generated differently as a result of key presses.
	  //const uint32_t kCapsLock        =  20;  // 0x14     Caps Lock key.
	  //const uint32_t kNumLock         = 144;  // 0x90     Num Lock key.
	  //const uint32_t kScrollLock      = 145;  // 0x91     Scroll Lock key.

		// Digit keys. 
		// kDigit0 thru kDigit9 are the same as ASCII '0' thru '9' (0x30 - 0x39)
		const uint32_t kDigit0          =  48;  // 0x30     The '0' digit key.
	  //const uint32_t kDigit1          =  49;  // 0x31     The '1' digit key. 
	  //const uint32_t kDigit2          =  50;  // 0x32     The '2' digit key. 
	  //const uint32_t kDigit3          =  51;  // 0x33     The '3' digit key. 
	  //const uint32_t kDigit4          =  52;  // 0x34     The '4' digit key. 
	  //const uint32_t kDigit5          =  53;  // 0x35     The '5' digit key. 
	  //const uint32_t kDigit6          =  54;  // 0x36     The '6' digit key. 
	  //const uint32_t kDigit7          =  55;  // 0x37     The '7' digit key. 
	  //const uint32_t kDigit8          =  56;  // 0x38     The '8' digit key. 
		const uint32_t kDigit9          =  57;  // 0x39     The '9' digit key. 

		// Letter keys.
		// kLetterA thru kLetterZ are the same as ASCII 'A' thru 'Z' (0x41 - 0x5A)
		const uint32_t kLetterA         =  65;  // 0x41     The 'A' key.
	  //const uint32_t kLetterB         =  66;  // 0x42     The 'B' key. 
		const uint32_t kLetterC         =  67;  // 0x43     The 'C' key. 
	  //const uint32_t kLetterD         =  68;  // 0x44     The 'D' key. 
	  //const uint32_t kLetterE         =  69;  // 0x45     The 'E' key. 
	  //const uint32_t kLetterF         =  70;  // 0x46     The 'F' key. 
	  //const uint32_t kLetterG         =  71;  // 0x47     The 'G' key. 
	  //const uint32_t kLetterH         =  72;  // 0x48     The 'H' key. 
	  //const uint32_t kLetterI         =  73;  // 0x49     The 'I' key. 
	  //const uint32_t kLetterJ         =  74;  // 0x4A     The 'J' key. 
	  //const uint32_t kLetterK         =  75;  // 0x4B     The 'K' key.
	  //const uint32_t kLetterL         =  76;  // 0x4C     The 'L' key. 
	  //const uint32_t kLetterM         =  77;  // 0x4D     The 'M' key. 
	  //const uint32_t kLetterN         =  78;  // 0x4E     The 'N' key. 
	  //const uint32_t kLetterO         =  79;  // 0x4F     The 'O' key. 
	  //const uint32_t kLetterP         =  80;  // 0x50     The 'P' key. 
	  //const uint32_t kLetterQ         =  81;  // 0x51     The 'Q' key. 
	  //const uint32_t kLetterR         =  82;  // 0x52     The 'R' key. 
	  //const uint32_t kLetterS         =  83;  // 0x53     The 'S' key. 
	  //const uint32_t kLetterT         =  84;  // 0x54     The 'T' key. 
	  //const uint32_t kLetterU         =  85;  // 0x55     The 'U' key.
		const uint32_t kLetterV         =  86;  // 0x56     The 'V' key. 
	  //const uint32_t kLetterW         =  87;  // 0x57     The 'W' key. 
		const uint32_t kLetterX         =  88;  // 0x58     The 'X' key. 
		const uint32_t kLetterY         =  89;  // 0x59     The 'Y' key. 
		const uint32_t kLetterZ         =  90;  // 0x5A     The 'Z' key. 

		// Miscellaneous keys
	  //const uint32_t kInvalid         =   0;  // 0x00     Refers not to a key, but instead acts as a placeholder for an invalid key id.
		const uint32_t kBackspace       =   8;  // 0x08     Backspace key.
		const uint32_t kTab             =   9;  // 0x09     The tab key.
		const uint32_t kEnter           =  13;  // 0x0D     Either of the return and enter keys.
	  //const uint32_t kPause           =  19;  // 0x13     The pause (a.k.a break) key, as returned by Windows without control depressed.
	  //const uint32_t kBreak           =   3;  // 0x03     The break (a.k.a pause) key, as returned by Windows with control depressed.
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
	  //const uint32_t kPrintScreen     =  44;  // 0x2C     The PrintScrn (print screen) key.
		const uint32_t kInsert          =  45;  // 0x2D     The insert key.
		const uint32_t kDelete          =  46;  // 0x2E     The delete key.
	  //const uint32_t kHelp            =  47;  // 0x2F     The help key, if there is one.
	  //const uint32_t kSleep           =  95;  // 0x5F     The computer sleep key, if it has one.
	  //const uint32_t kApp             =  93;  // 0x5D     Windows-specific: The "applications" key. 

		// Numpad keys
	  //const uint32_t kNumpadCenter    =  12;  // 0x0C     The key in the center of the numpad (with '5' on it) when numlock is turned off.
	  //const uint32_t kNumpadEnter     =  14;  // 0x0C     The number pad enter key.
	  //const uint32_t kNumpad0         =  96;  // 0x60     The number pad '0' key.
	  //const uint32_t kNumpad1         =  97;  // 0x61     The number pad '1' key.
	  //const uint32_t kNumpad2         =  98;  // 0x62     The number pad '2' key.
	  //const uint32_t kNumpad3         =  99;  // 0x63     The number pad '3' key.
	  //const uint32_t kNumpad4         = 100;  // 0x64     The number pad '4' key.
	  //const uint32_t kNumpad5         = 101;  // 0x65     The number pad '5' key.
	  //const uint32_t kNumpad6         = 102;  // 0x66     The number pad '6' key.
	  //const uint32_t kNumpad7         = 103;  // 0x67     The number pad '7' key.
	  //const uint32_t kNumpad8         = 104;  // 0x68     The number pad '8' key.
	  //const uint32_t kNumpad9         = 105;  // 0x69     The number pad '9' key.
		const uint32_t kNumpadMultiply  = 106;  // 0x6A     The number pad '*' key.
		const uint32_t kNumpadPlus      = 107;  // 0x6B     The number pad '+' key.
		const uint32_t kNumpadMinus     = 109;  // 0x6D     The number pad '-' key.
		const uint32_t kNumpadDecimal   = 110;  // 0x6E     The number pad '.' key.
		const uint32_t kNumpadDivide    = 111;  // 0x6F     The number pad '/' key.

	  //const uint32_t kF1              = 112;  // 0x70     The F1 key.
	  //const uint32_t kF2              = 113;  // 0x71     The F2 key.
	  //const uint32_t kF3              = 114;  // 0x72     The F3 key.
	  //const uint32_t kF4              = 115;  // 0x73     The F4 key.
	  //const uint32_t kF5              = 116;  // 0x74     The F5 key.
	  //const uint32_t kF6              = 117;  // 0x75     The F6 key.
	  //const uint32_t kF7              = 118;  // 0x76     The F7 key.
	  //const uint32_t kF8              = 119;  // 0x77     The F8 key.
	  //const uint32_t kF9              = 120;  // 0x78     The F9 key.
	  //const uint32_t kF10             = 121;  // 0x79     The F10 key.
	  //const uint32_t kF11             = 122;  // 0x7A     The F11 key.
	  //const uint32_t kF12             = 123;  // 0x7B     The F12 key.
	  //const uint32_t kF13             = 124;  // 0x7C     The F13 key.
	  //const uint32_t kF14             = 125;  // 0x7D     The F14 key.
	  //const uint32_t kF15             = 126;  // 0x7E     The F15 key.
	  //const uint32_t kF16             = 127;  // 0x7F     The F16 key.
	  //const uint32_t kF17             = 128;  // 0x80     The F17 key.
	  //const uint32_t kF18             = 129;  // 0x81     The F18 key.
	  //const uint32_t kF19             = 130;  // 0x82     The F19 key.
	  //const uint32_t kF20             = 131;  // 0x83     The F20 key.
	  //const uint32_t kF21             = 132;  // 0x84     The F21 key.
	  //const uint32_t kF22             = 133;  // 0x85     The F22 key.
	  //const uint32_t kF23             = 134;  // 0x86     The F23 key.
	  //const uint32_t kF24             = 135;  // 0x87     The F24 key.

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
	  //const uint32_t kCloseBracket    = 221;  // 0xDD     The ']' key.             // May be different on non-US keyboards.
		const uint32_t kSingleQuote     = 222;  // 0xDE     The ''' key.             // May be different on non-US keyboards.
	  //const uint32_t kSection         = 223;  // 0xDF     The section (§) key on some foreign keyboards, such as French.
	  //const uint32_t kHalf            = 167;  // 0xA7     The half (½) key found on some foreign keyboards, such as Swedish.
	  //const uint32_t kSeparator       = 108;  // 0x6C     This is not actually generated by keyboards

	  //const uint32_t kSelect          =  41;  // 0x29
	  //const uint32_t kPrint           =  42;  // 0x2A
	  //const uint32_t kExecute         =  43;  // 0x2B

	} // namespace Input

} // namespace EA




namespace EA
{
namespace Text
{


static const float kDefaultTextEditBorderSize  = 4.0f;
static const float kDefaultTextEditCaretPeriod = 0.25f;


#if defined(EA_PLATFORM_WINDOWS) && EA_WINAPI_FAMILY_PARTITION(EA_WINAPI_PARTITION_DESKTOP)
	extern "C" { extern unsigned int __stdcall GetDoubleClickTime(); }
#endif


/////////////////////////////////////////////////////////////////////////////
// TextEdit
//
TextEdit::TextEdit()
	: mInitialized(false)
	, mId(0)
	, mFlags(0)
	, mTextEditFlags(kEditFlagsDefault)
	, mTextStyle(0)
	, mDirtyFlags(0)
	, mLineWrapMode(kLineWrapModeDefault)
	, mBorders(kDefaultTextEditBorderSize, kDefaultTextEditBorderSize, kDefaultTextEditBorderSize, kDefaultTextEditBorderSize)
	, mPhysicalArea(0, 0, 0, 0)
	, mVisibleArea(0, 0, 0, 0)
	, mSuspendUpdateLevel(0)
  //, mColors()
	, mMaxTextLength(kCharIndexInfinite)
	, mCursorIndex(0)
	, mAnchorIndex(0)
	, mCursorColumn()
	, mCursorParaLine()
	, mCursorSeekPositionX()
	, mpTypesetter(NULL)
	, mpMessageHandler(NULL)
	, mText()
	, mParagraphList(EASTLAllocatorType(EATEXT_ALLOC_PREFIX "TextEdit"))
	, mStyle(0)
	, mActualStyle()
	, mLineHeight()
	, mLineBreaks(EASTLAllocatorType(EATEXT_ALLOC_PREFIX "TextEdit"))
	, mParaBreak(0)
	, mUndoHistorySize(10)
	, mUndoList(EASTLAllocatorType(EATEXT_ALLOC_PREFIX "TextEdit"))
	, mRedoList(EASTLAllocatorType(EATEXT_ALLOC_PREFIX "TextEdit"))
	, mScrollPosition(0.f, 0.f)
	, mTextExtent(0, 0)
	, mAutoScrollTimer(EA::StdC::Stopwatch::kUnitsSeconds)
	, mCaretPeriod(kDefaultTextEditCaretPeriod)
	, mCaretTimer(EA::StdC::Stopwatch::kUnitsSeconds)
	, mCaretRect(0, 0, 0, 0)
	, mCaretVisible(true)
	, mDoubleClickTimer(EA::StdC::Stopwatch::kUnitsSeconds)
	, mDoubleClickInterval(0.5f)
{
	mColors[kColorText]                 = 0xff000000;  // Black
	mColors[kColorBackground]           = 0x00ffffff;  // None
	mColors[kColorTextDisabled]         = 0xff808080;  // Grey
	mColors[kColorBackgroundDisabled]   = 0x00000000;  // None
	mColors[kColorTextSelected]         = 0xff0080ff;  // Blue-green
	mColors[kColorBackgroundSelected]   = 0x00C0C0ff;  // None
	mColors[kColorTextCaret]            = 0xff00ff00;  // Green
	mColors[kColorBackgroundCaret]      = 0xff000000;  // Black

	//mScrollbars[kScrollbarVertical].mDrawable  = NULL;
	//mScrollbars[kScrollbarVertical].mScrollbar = NULL;
	mScrollbars[kScrollbarVertical].mEnabled   = kScrollbarNever;

	//mScrollbars[kScrollbarHorizontal].mDrawable  = NULL;
	//mScrollbars[kScrollbarHorizontal].mScrollbar = NULL;
	mScrollbars[kScrollbarHorizontal].mEnabled   = kScrollbarNever;

	#if defined(EA_PLATFORM_WINDOWS) && EA_WINAPI_FAMILY_PARTITION(EA_WINAPI_PARTITION_DESKTOP)
		mDoubleClickInterval = (float)GetDoubleClickTime() / 1000.0f;
	#endif
}


/////////////////////////////////////////////////////////////////////////////
// ~TextEdit
//
TextEdit::~TextEdit()
{
	EA_ASSERT(!mInitialized);
	Shutdown();
}


////////////////////////////////////////////////////////////////////////////////
// Init
//
bool TextEdit::Init()
{
	if(!mInitialized)
	{
		mInitialized = true;

		mParagraphList.reserve(16);
		mParagraphList.push_back(Paragraph(0, 0)); // First line always starts at position 0

		SetTextStyle(mTextStyle);

		// Recalculate everything.
		mDirtyFlags |= kDirtyFlagLineBreaks;

		ShowCaret(true);
		mCaretTimer.Restart();
	}

	return mInitialized;
}


////////////////////////////////////////////////////////////////////////////////
// Shutdown
//
bool TextEdit::Shutdown()
{
	TrimUndoList(0);
	TrimRedoList(0);

	//mScrollbars[kScrollbarVertical].mScrollbar   = NULL;
	//mScrollbars[kScrollbarVertical].mDrawable    = NULL;
	//mScrollbars[kScrollbarHorizontal].mScrollbar = NULL;
	//mScrollbars[kScrollbarHorizontal].mDrawable  = NULL;

	mInitialized = false;

	return true;
}


/////////////////////////////////////////////////////////////////////////////
// OnAreaChanged
//
void TextEdit::OnAreaChanged(const RectFloat& /*newArea*/)
{
	CreateScrollbars();
	CalculateVisibleArea();
	mDirtyFlags |= kDirtyFlagLineBreaks;
}


/////////////////////////////////////////////////////////////////////////////
// CalculateVisibleArea
//
void TextEdit::CalculateVisibleArea()
{
	// Get the area of this window (relative to the top left of the window.)
	mVisibleArea = RectFloat(0, 0, mPhysicalArea.GetWidth(), mPhysicalArea.GetHeight());

	// Shrink it by the border widths.
	mVisibleArea.mLeft   += mBorders.mLeft;
	mVisibleArea.mTop    += mBorders.mTop;
	mVisibleArea.mRight  -= mBorders.mRight;
	mVisibleArea.mBottom -= mBorders.mBottom;

	// Adjust for scrollbars too
	if(IsScrollbarVisible(kScrollbarVertical))
		mVisibleArea.mRight -= mScrollbars[kScrollbarVertical].mPreferredSize.mX;

	if(IsScrollbarVisible(kScrollbarHorizontal))
		mVisibleArea.mBottom -= mScrollbars[kScrollbarHorizontal].mPreferredSize.mY;

	// Now, make sure its not negative in size. If the rectangle is inside out, 
	// then set it to an empty rect.
	if(!mVisibleArea.IsNormalized())
		mVisibleArea.SetSize(0, 0);
}


/////////////////////////////////////////////////////////////////////////////
// SendMessage
//
void TextEdit::SendMessage(TextEditMessage& msg)
{
	if(mpMessageHandler)
		mpMessageHandler->HandleTextEditMessage(msg);
}


/////////////////////////////////////////////////////////////////////////////
// NotifyTextChanged
//
void TextEdit::NotifyTextChanged()
{
	Message msg(kEditMessageTextChanged);

	msg.mArgs.m1  = mId;
	msg.mArgs.m2  = mId; // Duplicated for historical reasons.
	msg.mArgs.m3  = (int32_t)mText.size();
	msg.mArgs.m4p = &mText[0];

	SendMessage(msg);
}


/////////////////////////////////////////////////////////////////////////////
// NotifyError
//
void TextEdit::NotifyError(tError nError)
{
	Message msg(kEditMessageError);

	msg.mArgs.m1 = mId;
	msg.mArgs.m2 = nError;

	SendMessage(msg);
}


/////////////////////////////////////////////////////////////////////////////
// NotifyCapture
//
bool TextEdit::NotifyCapture(CaptureRequest captureRequest)
{
	CaptureControl captureControl;
	captureControl.mRequest = captureRequest;
	captureControl.mResult  = false;

	Message msg(kEditMessageCaptureRequest);
	msg.mArgs.m1  = mId;
	msg.mArgs.m4p = &captureControl;

	SendMessage(msg);

	return captureControl.mResult;
}


/////////////////////////////////////////////////////////////////////////////
// GetTextStyle
//
uint32_t TextEdit::GetTextStyle()
{
	return mStyle;
}


/////////////////////////////////////////////////////////////////////////////
// GetTypesetter
//
Text::Typesetter* TextEdit::GetTypesetter()
{
	return mpTypesetter;
}


/////////////////////////////////////////////////////////////////////////////
// SetTypesetter
//
void TextEdit::SetTypesetter(Text::Typesetter* pTypesetter)
{
	mpTypesetter = pTypesetter;
}


/////////////////////////////////////////////////////////////////////////////
// SetMessageHandler
//
void TextEdit::SetMessageHandler(ITextEditMessageHandler* pMessageHandler)
{
	mpMessageHandler = pMessageHandler;
}


/////////////////////////////////////////////////////////////////////////////
// SetTextStyle
//
void TextEdit::SetTextStyle(uint32_t nStyleId)
{
	using namespace EA::Text;

	// Note: We intentionally do not do 'smart' checking to see if nStyleID is
	// equal to mTextStyle. We rely on being able to call this to unilaterally
	// trigger a recalculation.

	mTextStyle   = nStyleId;
	mDirtyFlags |= kDirtyFlagLineBreaks;

	StyleManager* const pStyleManager = GetStyleManager(true);

	if(pStyleManager)
	{
		const TextStyle* const pStyle = pStyleManager->GetStyle(mTextStyle);

		if(pStyle)
			mActualStyle = *pStyle;
		else
			mActualStyle = TextStyle(); // Reset to constructed values

		FontServer* const pFS = GetFontServer();

		if(pFS)
		{
			FontSelection fontSelection;

			if(pFS->GetFont(&mActualStyle, fontSelection))
			{
				float fMaxBaseline = 0;
				float fMaxDescent  = 0;

				GetMaxFontMetrics(fontSelection, fMaxBaseline, fMaxDescent);
				mLineHeight = (int32_t)(fMaxBaseline - fMaxDescent);
			}
		}
	}

	if(mInitialized)
		Invalidate();
}


/////////////////////////////////////////////////////////////////////////////
// GetColor
//
uint32_t TextEdit::GetColor(tColorIndex nColorIndex) const
{
	EA_ASSERT(nColorIndex >= 0 && nColorIndex < kColorCount);

	if((nColorIndex >= 0) && (nColorIndex < kColorCount))
		return mColors[nColorIndex];
	return 0;
}


/////////////////////////////////////////////////////////////////////////////
// SetColor
//
void TextEdit::SetColor(tColorIndex nColorIndex, uint32_t nColor)
{
	EA_ASSERT((nColorIndex >= 0) && (nColorIndex < kColorCount));

	if((nColorIndex >= 0) && (nColorIndex < kColorCount) && (mColors[nColorIndex] != nColor))
	{
		mColors[nColorIndex] = nColor;
		Invalidate();
	}
}


/////////////////////////////////////////////////////////////////////////////
// GetArea
//
const RectFloat& TextEdit::GetArea() const
{
	return mPhysicalArea;
}


/////////////////////////////////////////////////////////////////////////////
// SetArea
//
void TextEdit::SetArea(const EA::RectFloat& area)
{
	mPhysicalArea = area;
	OnAreaChanged(area);
}


/////////////////////////////////////////////////////////////////////////////
// GetBorders
//
const RectFloat &TextEdit::GetBorders() const
{
	return mBorders;
}


/////////////////////////////////////////////////////////////////////////////
// SetBorders
//
void TextEdit::SetBorders(const RectFloat& rBorders)
{
	mBorders = rBorders;
	CreateScrollbars();
	CalculateVisibleArea();

	// Recalculate the line breaks.
	mDirtyFlags |= kDirtyFlagLineBreaks;
}


/////////////////////////////////////////////////////////////////////////////
// GetCaretPeriod
//
float TextEdit::GetCaretPeriod() const
{
	return mCaretPeriod;
}


/////////////////////////////////////////////////////////////////////////////
// SetCaretPeriod
//
void TextEdit::SetCaretPeriod(float fSeconds)
{
	mCaretPeriod = fSeconds;
}


/////////////////////////////////////////////////////////////////////////////
// GetDoubleClickInterval
//
float TextEdit::GetDoubleClickInterval() const
{
	return mDoubleClickInterval;
}


/////////////////////////////////////////////////////////////////////////////
// SetDoubleClickInterval
//
void TextEdit::SetDoubleClickInterval(float fSeconds)
{
	mDoubleClickInterval = fSeconds;
}


/////////////////////////////////////////////////////////////////////////////
// GetTextLength
//
uint32_t TextEdit::GetTextLength() const
{
	return (uint32_t)mText.size();
}


/////////////////////////////////////////////////////////////////////////////
// GetText
//
uint32_t TextEdit::GetText(Char* pBuffer, tCharIndex nStart, uint32_t nLength) const
{
	const uint32_t nTextLength = TextEdit::GetTextLength();

	if(nStart < nTextLength) 
	{
		nLength = eastl::min_alt<uint32_t>(nLength, nTextLength - nStart);
		memcpy(pBuffer, &mText[nStart], nLength * sizeof(Char));
		return nLength;
	}

	return 0;
}


/////////////////////////////////////////////////////////////////////////////
// GetText
//
uint32_t TextEdit::GetText(String& sResult, tCharIndex nStart, uint32_t nLength) const
{
	uint32_t nTextLength = TextEdit::GetTextLength();

	if(nStart < nTextLength)
	{
		nLength = eastl::min_alt<uint32_t>(nLength, nTextLength - nStart);
		sResult.assign(&mText[nStart], nLength);
		return nLength;
	}

	return 0;
}


/////////////////////////////////////////////////////////////////////////////
// GetText
//
const Char* TextEdit::GetText()
{
	return mText.c_str();
}


/////////////////////////////////////////////////////////////////////////////
// SetText
//
void TextEdit::SetText(const Char* caption, bool bNotify)
{
	EA_ASSERT(caption != NULL);
	Replace(0, kCharIndexEnd, caption, (tCharIndex)EA::StdC::Strlen(caption), bNotify);
}


/////////////////////////////////////////////////////////////////////////////
// GetMaximumTextLength
//
uint32_t TextEdit::GetMaximumTextLength() const
{
	return mMaxTextLength;
}


/////////////////////////////////////////////////////////////////////////////
// SetMaximumTextLength
//
void TextEdit::SetMaximumTextLength(uint32_t nMaxLength)
{
	if(nMaxLength != mMaxTextLength)
	{
		mMaxTextLength = nMaxLength;

		// Trim any characters beyond that point.
		Replace(nMaxLength, kCharIndexEnd, NULL, 0, true);
	}
}


/////////////////////////////////////////////////////////////////////////////
// Insert
//
bool TextEdit::Insert(const Char* pInsertText, uint32_t nInsertLength, bool bNotify)
{
	return Replace(mCursorIndex, 0, pInsertText, nInsertLength, bNotify);
}


/////////////////////////////////////////////////////////////////////////////
// Delete
//
bool TextEdit::Delete(uint32_t nDeleteLength, bool bNotify)
{
	return Replace(mCursorIndex, nDeleteLength,  NULL, 0, bNotify);
}


/////////////////////////////////////////////////////////////////////////////
// Replace
//
bool TextEdit::ReplaceSelection(const Char* pReplaceText, tCharIndex nReplaceLength, bool bNotify)
{
	tCharIndex nSelStart;
	tCharIndex nSelEnd;

	GetSelectionRange(nSelStart, nSelEnd);

	return Replace(nSelStart, nSelEnd - nSelStart, pReplaceText, nReplaceLength, bNotify);
}


/////////////////////////////////////////////////////////////////////////////
// Replace
//
bool TextEdit::Replace(tCharIndex nOldTextIndex, uint32_t nOldTextLength, 
							const Char* pNewText, uint32_t nNewTextLength, bool bNotify)
{
	const tCharIndex nTextLength = GetTextLength();

	nOldTextIndex  = eastl::min_alt<tCharIndex>(nOldTextIndex, nTextLength);
	nOldTextLength = eastl::min_alt<tCharIndex>(nOldTextLength, nTextLength - nOldTextIndex);

	// This can happen if we attempted to, for example, delete forward when we were already at the end.
	if((nOldTextLength == 0) && (nNewTextLength == 0)) 
		return false;

	// Check to see if we've reached the limit. Note that if the new length is smaller
	// than the existing length, then its OK to go over the limit (the idea being that
	// the old text was over the limit too.)
	tCharIndex nNewTotalLength = ((nTextLength - nOldTextLength) + nNewTextLength);

	if((nNewTotalLength > nTextLength) && (nNewTotalLength > mMaxTextLength))
	{
		NotifyError(kEditErrorCapacityReached);
		return false;
	}

	// Create a new undo action
	UndoAction* const pUndoAction = CORE_NEW(EA::Text::GetAllocator(), EATEXT_ALLOC_PREFIX "TextEdit/UndoAction", 0) UndoAction;
	pUndoAction->mIndex = nOldTextIndex;
	pUndoAction->mOldText.assign(&mText[nOldTextIndex], nOldTextLength);
	pUndoAction->mNewText.assign(pNewText, nNewTextLength);

	// Store the old cursor position
	pUndoAction->mOldCursorIndex = mCursorIndex;
	pUndoAction->mOldAnchorIndex = mAnchorIndex;

	// Calculate the new cursor position.
	if(mCursorIndex >= nOldTextIndex + nOldTextLength)
		pUndoAction->mNewCursorIndex = mCursorIndex - nOldTextLength + nNewTextLength;
	else if(mCursorIndex >= nOldTextIndex)
		pUndoAction->mNewCursorIndex = nOldTextIndex + nNewTextLength;
	else
		pUndoAction->mNewCursorIndex = mCursorIndex;

	// Calculate the new anchor position.
	if(mAnchorIndex >= nOldTextIndex + nOldTextLength)
		pUndoAction->mNewAnchorIndex = mAnchorIndex - nOldTextLength + nNewTextLength;
	else if(mAnchorIndex >= nOldTextIndex)
		pUndoAction->mNewAnchorIndex = nOldTextIndex + nNewTextLength;
	else
		pUndoAction->mNewAnchorIndex = mAnchorIndex;

	// Apply this change to the text
	ApplyRedo(pUndoAction);

	// Call the winproc to validate the change
	if((mSuspendUpdateLevel != 0) || ValidateChange())
	{
		// If we're keeping undo records. (Note: Don't record undo history if we
		// haven't been initialized.)

		if(mInitialized && (mUndoHistorySize > 0))
		{
			// Clear the redo list
			TrimRedoList(0);

			// Queue the undo item
			mUndoList.push_front(pUndoAction);

			// Trim the undo list
			TrimUndoList(mUndoHistorySize);

			if(bNotify)
				NotifyTextChanged();

		}
		else
		{
			// Otherwise, just delete this undo action
			CORE_DELETE(EA::Text::GetAllocator(), pUndoAction);
		}

		// Set the dirty flags for cursor pos and line breaks.
		mDirtyFlags |= (kDirtyFlagCursorPosition | kDirtyFlagLineBreaks | kDirtyFlagSetCursorSeek);

		// Tell the window manager to redraw us.
		Invalidate();

		return true;
	}

	// Otherwise, revert the change we just made.
	ApplyUndo(pUndoAction);
	delete pUndoAction;

	return false;
}


/////////////////////////////////////////////////////////////////////////////
// Clear
//
bool TextEdit::Clear(bool bNotify)
{
	// Clear simply deletes everything.
	return Replace(0, kCharIndexEnd, NULL, 0, bNotify);
}


/////////////////////////////////////////////////////////////////////////////
// Invalidate
//
void TextEdit::Invalidate()
{
	Message msg(kEditMessageRedraw);

	msg.mArgs.m1  = mId;
	msg.mArgs.m2  = 0;
	msg.mArgs.m3  = (int32_t)mText.size();
	msg.mArgs.m4p = &mText[0];

	SendMessage(msg);
}


/////////////////////////////////////////////////////////////////////////////
// Draw
//
bool TextEdit::Draw(void* pDrawContext)
{
	return OnRebuild(pDrawContext);
}


/////////////////////////////////////////////////////////////////////////////
// ValidateChange
//
bool TextEdit::ValidateChange()
{
	TextValidation validationParams;

	validationParams.mText       = &mText[0];
	validationParams.mTextLength = (tCharIndex)mText.size();
	validationParams.mValidated  = true;

	Message validateMessage(kEditMessageValidate);
	validateMessage.mArgs.m4p = (void*)&validationParams;

	SendMessage(validateMessage);

	return validationParams.mValidated;
}


/////////////////////////////////////////////////////////////////////////////
// SuspendUpdates
//
void TextEdit::SuspendUpdates()
{
	mSuspendUpdateLevel++;
}


/////////////////////////////////////////////////////////////////////////////
// ResumeUpdates
//
void TextEdit::ResumeUpdates()
{
	mSuspendUpdateLevel--;
}


/////////////////////////////////////////////////////////////////////////////
// GetCursorIndex
//
TextEdit::tCharIndex TextEdit::GetCursorIndex() const
{
	return mCursorIndex;
}


/////////////////////////////////////////////////////////////////////////////
// SetCursorIndex
//
void TextEdit::SetCursorIndex(tCharIndex nCursorIndex, bool bExtendSelection)
{
	SetCursorAndAnchor(nCursorIndex, bExtendSelection ? mAnchorIndex : nCursorIndex);
}


/////////////////////////////////////////////////////////////////////////////
// GetAnchorIndex
//
TextEdit::tCharIndex TextEdit::GetAnchorIndex() const
{
	return mAnchorIndex;
}


/////////////////////////////////////////////////////////////////////////////
// SetCursorAndAnchor
//
void TextEdit::SetCursorAndAnchor(tCharIndex nCursorIndex, tCharIndex nAnchorIndex)
{
	nCursorIndex = eastl::min_alt<tCharIndex>(nCursorIndex, GetTextLength());
	nAnchorIndex = eastl::min_alt<tCharIndex>(nAnchorIndex, GetTextLength());

	if(nCursorIndex != mCursorIndex || nAnchorIndex != mAnchorIndex)
	{
		mCursorIndex = nCursorIndex;
		mAnchorIndex = nAnchorIndex;

		mDirtyFlags |= kDirtyFlagCursorPosition | kDirtyFlagSetCursorSeek;
		Invalidate();
	}
}


/////////////////////////////////////////////////////////////////////////////
// GetSelectionRange
//
bool TextEdit::GetSelectionRange(tCharIndex& selectionStart, tCharIndex& selectionEnd) const
{
	selectionStart = eastl::min_alt<tCharIndex>(mCursorIndex, mAnchorIndex);
	selectionEnd   = eastl::max_alt<tCharIndex>(mCursorIndex, mAnchorIndex);

	return selectionStart != selectionEnd;
}

/////////////////////////////////////////////////////////////////////////////
// MoveCursor
//
bool TextEdit::MoveCursor(tMoveType nMoveType, int32_t nCount, bool bExtendSelection)
{
	tCharIndex nNewPos = mCursorIndex;

	switch (nMoveType)
	{
		case kMoveByCharacter:
			// Since we're dealing with unsigned numbers here, we can't simply add and clamp
			// because it might wrap around. Instead, we clamp the offset first before the add.
			if(nCount < 0) 
				nNewPos += eastl::max_alt<int32_t>(nCount, -(int32_t)mCursorIndex);
			else 
				nNewPos += eastl::min_alt<int32_t>(nCount, TextEdit::GetTextLength() - mCursorIndex);

			SetCursorIndex(nNewPos, bExtendSelection);
			Invalidate();
			break;

		case kMoveByWord:
			if(nCount > 0)
			{
				while(nCount-- && mCursorIndex < mText.size())
				{
					// Search for next word break;
					SetCursorIndex(FindNextWord(mCursorIndex), bExtendSelection);
				}
			}
			else if(nCount < 0)
			{
				while(nCount++ && mCursorIndex > 0) 
				{
					// Search for previous word break;
					SetCursorIndex(FindPrevWord(mCursorIndex), bExtendSelection);
				}
			}
			break;

		case kMoveByLine:
		{
			using namespace EA::Text;

			if(mDirtyFlags & kDirtyFlagSetCursorSeek)
				RecalculateCursorPosition();

			tParagraphList::const_iterator itPara = FindParagraphByTextIndex(mCursorIndex);
			int32_t nCursorLine = mCursorParaLine + nCount;

			// Seek through paragraphs until cursor index falls within one.
			if(nCursorLine < 0)
			{
				while(nCursorLine < 0)
				{
					if(itPara > mParagraphList.begin()) {
						--itPara;
						nCursorLine += (*itPara).mTextLines;
					}
					else {
						nCursorLine = 0;
						break;
					}
				}

				if(nCursorLine < 0)
					nCursorLine = 0;
			}
			else
			{
				while(nCursorLine >= (*itPara).mTextLines)
				{
					if(itPara < mParagraphList.end() - 1) {
						nCursorLine -= (*itPara).mTextLines;
						itPara++;
					}
					else {
						nCursorLine = (*itPara).mTextLines - 1;
						break;
					}
				}
			}

			// Layout the paragraph so we can determine the cursor position
			Paragraph& para = const_cast<Paragraph&>(*itPara);
			LayoutParagraph(para);

			tCharIndex  nLineStartIndex, nLineLength;
			LineLayout& ll = FillLineLayout(para, nCursorLine, nLineStartIndex, nLineLength);
			GlyphSide   gs;
			tCharIndex  nCursorIndex = (tCharIndex)ll.GetTextPositionFromDisplayPosition(
											mCursorSeekPositionX, 0, false, kPickTypeInsertion, gs, kDirectionDefault);

			// Change the cursor
			SetCursorIndex(para.mTextIndex + nLineStartIndex + nCursorIndex, bExtendSelection);

			// In the case of movement by line, we want to supress the normal behavior
			// which is to set the cursor seek position.
			mDirtyFlags &= ~kDirtyFlagSetCursorSeek;

			break;
		}

		case kMoveBySentance:
			if(nCount > 0)
			{
				while(nCount-- && mCursorIndex < mText.size()) {
					// Search for next sentance
					SetCursorIndex(FindNextSentence(mCursorIndex), bExtendSelection);
				}
			}
			else if(nCount < 0)
			{
				while(nCount++ && mCursorIndex > 0) {
					// Search for previous sentance
					SetCursorIndex(FindPrevSentence(mCursorIndex), bExtendSelection);
				}
			}
			break;

		case kMoveByParagraph:
			if(nCount > 0)
			{
				while(nCount-- && mCursorIndex < mText.size()) {
					// Search for next para;
					SetCursorIndex(FindNextParagraph(mCursorIndex), bExtendSelection);
				}
			}
			else if(nCount < 0)
			{
				while(nCount++ && mCursorIndex > 0) {
					// Search for previous para;
					SetCursorIndex(FindPrevParagraph(mCursorIndex), bExtendSelection);
				}
			}
			break;

		case kMoveToLineBeginning:
			SetCursorIndex(mCursorIndex - mCursorColumn, bExtendSelection);
			break;

		case kMoveToLineEnd:
		{
			tParagraphList::const_iterator itPara = FindParagraphByTextIndex(mCursorIndex);

			// Layout the paragraph so we can determine the cursor position
			Paragraph&para = const_cast<Paragraph&>(*itPara);
			LayoutParagraph(para);

			tCharIndex nLineStartIndex, nLineLength;
			GetLineRange(mCursorParaLine, nLineStartIndex, nLineLength);

			tCharIndex nLineEndIndex = nLineStartIndex + nLineLength;
			if(nLineEndIndex < para.mTextLength) 
				nLineEndIndex--;

			SetCursorIndex(para.mTextIndex + nLineEndIndex, bExtendSelection);

			break;
		}

		case kMoveToBeginning:
			SetCursorIndex(0, bExtendSelection);
			break;

		case kMoveToEnd:
			SetCursorIndex(TextEdit::GetTextLength(), bExtendSelection);
			break;

		case kMoveByPage:
		default:
			return false;
	}

	return true;
}


/////////////////////////////////////////////////////////////////////////////
// GetParagraphCount
//
uint32_t TextEdit::GetParagraphCount() const
{
	if(mDirtyFlags & kDirtyFlagLineBreaks)
		const_cast<TextEdit*>(this)->RecalculateLineBreaks();

	return (uint32_t)mParagraphList.size();
}


/////////////////////////////////////////////////////////////////////////////
// GetParagraphTextIndex
//
TextEdit::tCharIndex TextEdit::GetParagraphTextIndex(uint32_t nParaIndex) const
{
	if(mDirtyFlags & kDirtyFlagLineBreaks)
		const_cast<TextEdit*>(this)->RecalculateLineBreaks();

	const uint32_t nNumParagraphs = (uint32_t)mParagraphList.size();

	if(nParaIndex >= nNumParagraphs) 
		return kInvalidCharIndex;

	return mParagraphList[nParaIndex].mTextIndex;
}


/////////////////////////////////////////////////////////////////////////////
// GetParagraphYPos
//
float TextEdit::GetParagraphYPos(uint32_t nParaIndex) const
{
	if(mDirtyFlags & kDirtyFlagLineBreaks)
		const_cast<TextEdit *>(this)->RecalculateLineBreaks();

	const uint32_t nNumParagraphs = (uint32_t)mParagraphList.size();

	if(nParaIndex >= nNumParagraphs)
		return 0.0f;

	return mParagraphList[nParaIndex].mYPos;
}


/////////////////////////////////////////////////////////////////////////////
// GetParagraphLength
//
uint32_t TextEdit::GetParagraphLength(uint32_t nParaIndex) const
{
	if(mDirtyFlags & kDirtyFlagLineBreaks)
		const_cast<TextEdit*>(this)->RecalculateLineBreaks();

	const uint32_t nNumParagraphs = (uint32_t)mParagraphList.size();

	if(nParaIndex >= nNumParagraphs)
		return kInvalidCharIndex;

	return mParagraphList[nParaIndex].mTextLength;
}


/////////////////////////////////////////////////////////////////////////////
// GetParagraphAtTextIndex
//
uint32_t TextEdit::GetParagraphAtTextIndex(tCharIndex nTextIndex) const
{
	if(mDirtyFlags & kDirtyFlagLineBreaks)
		const_cast<TextEdit*>(this)->RecalculateLineBreaks();

	return (uint32_t)(FindParagraphByTextIndex(nTextIndex) - mParagraphList.begin());
}


/////////////////////////////////////////////////////////////////////////////
// GetParagraphAtPosition
//
uint32_t TextEdit::GetParagraphAtPosition(float fYPos) const
{
	if(mDirtyFlags & kDirtyFlagLineBreaks)
		const_cast<TextEdit *>(this)->RecalculateLineBreaks();

	return (uint32_t)(FindParagraphByPosition(fYPos) - mParagraphList.begin());
}


/////////////////////////////////////////////////////////////////////////////
// GetLineWrapMode
//
TextEdit::tLineWrapMode TextEdit::GetLineWrapMode() const
{
	return (tLineWrapMode)mLineWrapMode;
}


/////////////////////////////////////////////////////////////////////////////
// SetLineWrapMode
//
void TextEdit::SetLineWrapMode(tLineWrapMode mode)
{
	mLineWrapMode = mode;
	mDirtyFlags |= kDirtyFlagLineBreaks;
}


/////////////////////////////////////////////////////////////////////////////
// SetTextEditFlag
//
void TextEdit::SetTextEditFlag(uint32_t flag, bool bSet)
{
	const uint32_t nOldFlags = mTextEditFlags;

	if(bSet)
		mTextEditFlags |= flag;
	else
		mTextEditFlags &= ~flag;

	const uint32_t nChangedFlags = (nOldFlags ^ mTextEditFlags);

	// These are the only flags we care about changing for now
	if(nChangedFlags & (kEditFlagHideCaret|kEditFlagReadOnly))
	{
		if(!(mTextEditFlags & (kEditFlagHideCaret|kEditFlagReadOnly)))
			mCaretTimer.Restart();
	}

	if(nChangedFlags & (kEditFlagOverwrite | kEditFlagHideCaret | kEditFlagHideInactiveSelection))
		Invalidate();
}


/////////////////////////////////////////////////////////////////////////////
// GetTextEditFlag
//
bool TextEdit::GetTextEditFlag(uint32_t flag) const
{
	return (mTextEditFlags & flag) != 0;
}


/////////////////////////////////////////////////////////////////////////////
// GetTextEditFlags
//
int32_t TextEdit::GetTextEditFlags() const
{
	return mTextEditFlags;
}


/////////////////////////////////////////////////////////////////////////////
// GetUndoHistorySize
//
uint32_t TextEdit::GetUndoHistorySize() const
{
	return mUndoHistorySize;
}


/////////////////////////////////////////////////////////////////////////////
// SetUndoHistorySize
//
void TextEdit::SetUndoHistorySize(uint32_t nCount)
{
	mUndoHistorySize = nCount;
	TrimUndoList(mUndoHistorySize);
}


/////////////////////////////////////////////////////////////////////////////
// CanUndo
//
bool TextEdit::CanUndo() const
{
	return !mUndoList.empty();
}


/////////////////////////////////////////////////////////////////////////////
// CanRedo
//
bool TextEdit::CanRedo() const
{
	return !mRedoList.empty();
}


/////////////////////////////////////////////////////////////////////////////
// Undo
//
bool TextEdit::Undo()
{
	if(!mUndoList.empty())
	{
		// Get the head undo action.
		UndoAction* const pAction = mUndoList.front();
		mUndoList.pop_front();

		// Make the change.
		ApplyUndo(pAction);

		// Add to redo list.
		mRedoList.push_front(pAction);

		// Set the dirty flags for cursor pos and line breaks.
		mDirtyFlags |= (kDirtyFlagCursorPosition | kDirtyFlagLineBreaks | kDirtyFlagSetCursorSeek);

		// Tell the window manager to redraw us.
		Invalidate();

		NotifyTextChanged();

		return true;
	}

	return false;
}

/////////////////////////////////////////////////////////////////////////////
// Redo
//
bool TextEdit::Redo()
{
	if(!mRedoList.empty())
	{
		// Get the head redo action
		UndoAction* const pAction = mRedoList.front();
		mRedoList.pop_front();

		// Apply the change
		ApplyRedo(pAction);

		// Add to undo list
		mUndoList.push_front(pAction);

		// Set the dirty flags for cursor pos and line breaks.
		mDirtyFlags |= (kDirtyFlagCursorPosition | kDirtyFlagLineBreaks | kDirtyFlagSetCursorSeek);

		// Tell the window manager to redraw us.
		Invalidate();

		NotifyTextChanged();

		return true;
	}

	return false;
}


/////////////////////////////////////////////////////////////////////////////
// ApplyUndo
//
void TextEdit::ApplyUndo(UndoAction* pAction)
{
	mText.replace(pAction->mIndex, pAction->mNewText.size(), pAction->mOldText);
	mCursorIndex = pAction->mOldCursorIndex;
	mAnchorIndex = pAction->mOldAnchorIndex;
}


/////////////////////////////////////////////////////////////////////////////
// ApplyRedo
//
void TextEdit::ApplyRedo(UndoAction* pAction)
{
	mText.replace(pAction->mIndex, pAction->mOldText.size(), pAction->mNewText);
	mCursorIndex = pAction->mNewCursorIndex;
	mAnchorIndex = pAction->mNewAnchorIndex;
}


/////////////////////////////////////////////////////////////////////////////
// TrimUndoList
//
void TextEdit::TrimUndoList(uint32_t nMaxSize)
{
	uint32_t nSize = (uint32_t)mUndoList.size();

	while(nSize > nMaxSize)
	{
		delete mUndoList.back();
		mUndoList.pop_back();
		nSize--;
	}
}


/////////////////////////////////////////////////////////////////////////////
// TrimRedoList
//
void TextEdit::TrimRedoList(uint32_t nMaxSize)
{
	uint32_t nSize = (uint32_t)mRedoList.size();

	while(nSize > nMaxSize)
	{
		delete mRedoList.back();
		mRedoList.pop_back();
		nSize--;
	}
}


/////////////////////////////////////////////////////////////////////////////
// FindNextWord
//
TextEdit::tCharIndex TextEdit::FindNextWord(tCharIndex nPos) const
{
	// Note by Paul Pedriana: This should be switched to us the 
	// EAText WordBreakIterator, as it will be more accurate.
	size_t nTextLength = mText.size();
	bool   bPrevSpace = false;

	while(nPos < nTextLength)
	{
		const bool bSpace = EA::StdC::Isspace(mText[nPos]) ? true : false;
		if(!bSpace && bPrevSpace)
			break;
		bPrevSpace = bSpace;
		nPos++;
	}
	return nPos;
}


/////////////////////////////////////////////////////////////////////////////
// FindPrevWord
//
TextEdit::tCharIndex TextEdit::FindPrevWord(tCharIndex nPos) const
{
	// Note by Paul Pedriana: This should be switched to us the 
	// EAText WordBreakIterator, as it will be more accurate.
	bool bPrevSpace = true;

	while(nPos > 0)
	{
		--nPos;
		bool bSpace = EA::StdC::Isspace(mText[nPos]) ? true : false;
		if(bSpace && !bPrevSpace)
			return nPos + 1;
		bPrevSpace = bSpace;
	}

	return nPos;
}


/////////////////////////////////////////////////////////////////////////////
// FindNextWord
//
TextEdit::tCharIndex TextEdit::FindNextWordEnd(tCharIndex nPos) const
{
	// Note by Paul Pedriana: This should be switched to us the 
	// EAText WordBreakIterator, as it will be more accurate.
	size_t nTextLength = mText.size();
	bool   bPrevSpace = false;

	while(nPos < nTextLength)
	{
		bool bSpace = EA::StdC::Isspace(mText[nPos]) ? true : false;
		if(bSpace && !bPrevSpace)
			break;
		bPrevSpace = bSpace;
		nPos++;
	}
	return nPos;
}


/////////////////////////////////////////////////////////////////////////////
// FindPrevWord
//
TextEdit::tCharIndex TextEdit::FindPrevWordEnd(tCharIndex nPos) const
{

	// Note by Paul Pedriana: This should be switched to us the 
	// EAText WordBreakIterator, as it will be more accurate.
	bool bPrevSpace = true;

	while(nPos > 0)
	{
		--nPos;
		bool bSpace = EA::StdC::Isspace(mText[nPos]) ? true : false;
		if(!bSpace && bPrevSpace)
			return nPos + 1;
		bPrevSpace = bSpace;
	}

	return nPos;
}


/////////////////////////////////////////////////////////////////////////////
// FindNextSentence
//
TextEdit::tCharIndex TextEdit::FindNextSentence(tCharIndex nPos) const
{
	// Note by Paul Pedriana: This should be switched to us the 
	// EAText SentenceBreakIterator, as it will be more accurate.
	enum {
		kStateDefault,
		kStatePuncWS,
		kStatePunc,
		kStateNL
	};

	size_t   nTextLength = mText.size();
	uint32_t nState = kStateDefault;

	while(nPos < nTextLength)
	{
		const Char nChar = mText[nPos++];

		switch (nChar)
		{
			case '\n': 
			case '\r':
				nState = kStateNL;
				break;

			case '.':
			case '?':
			case '!':
			case ':':
			case ';':
				if(nState == kStateNL)
					return nPos - 1;
				nState = kStatePunc;
				break;

			default:
				if(nState == kStateNL)
					return nPos - 1;
				if(EA::StdC::Isspace(nChar))
				{
					if(nState == kStatePunc) 
						nState = kStatePuncWS;
				}
				else
				{
					if(nState == kStatePuncWS)
						return nPos - 1;
					nState = kStateDefault;
				}

				break;
		}
	}

	return nPos;
}


/////////////////////////////////////////////////////////////////////////////
// FindPrevSentence
//
TextEdit::tCharIndex TextEdit::FindPrevSentence(tCharIndex nPos) const
{
	// Note by Paul Pedriana: This should be switched to us the 
	// EAText SentenceBreakIterator, as it will be more accurate.
	enum
	{
		kStateDefault,
		kStateWS
	};

	uint32_t   nState = kStateDefault;
	tCharIndex nSaved = kInvalidCharIndex;

	while(nPos > 0)
	{
		const Char nChar = mText[--nPos];

		switch (nChar)
		{
			case '\n':
			case '\r':
				if(nSaved != kInvalidCharIndex) 
					return nSaved;
				nState = kStateDefault;
				break;

			case '.':
			case '?':
			case '!':
			case ':':
			case ';':
				if(nSaved != kInvalidCharIndex && nState == kStateWS) 
					return nSaved;
				nState = kStateDefault;
				break;

			default:
				if(EA::StdC::Isspace(nChar))
					nState = kStateWS;
				else
				{
					nState = kStateDefault;
					nSaved = nPos;
				}
				break;
		}
	}

	return nPos;
}


/////////////////////////////////////////////////////////////////////////////
// FindNextParagraph
//
TextEdit::tCharIndex TextEdit::FindNextParagraph(tCharIndex nPos) const
{
	// Note by Paul Pedriana: This should be switched to us the 
	// EAText ParagraphBreakIterator, as it will be more accurate.
	// \r and \n are not the only paragraph delimiters!
	const size_t nTextLength = mText.size();

	while(nPos < nTextLength)
	{
		if(mText[nPos] == '\n') 
			return nPos + 1;

		if(mText[nPos] == '\r')
		{
			if(((nPos + 1) < nTextLength) && (mText[nPos + 1] == '\n')) 
				return nPos + 2;
			return nPos + 1;
		}
		
		nPos++;
	}

	return (tCharIndex)nTextLength;
}


/////////////////////////////////////////////////////////////////////////////
// FindPrevParagraph
//
TextEdit::tCharIndex TextEdit::FindPrevParagraph(tCharIndex nPos) const
{
	// Note by Paul Pedriana: This should be switched to us the 
	// EAText ParagraphBreakIterator, as it will be more accurate.
	// \r and \n are not the only paragraph delimiters!
	if((nPos > 0) && (mText[nPos - 1] == '\n')) 
		nPos--;

	if((nPos > 0) && (mText[nPos - 1] == '\r')) 
		nPos--;

	while(nPos > 0)
	{
		if((nPos > 0) && ((mText[nPos - 1] == '\r') || (mText[nPos - 1] == '\n'))) 
			return nPos;
		if((nPos > 0) && (mText[nPos - 1] == '\r')) 
			nPos--;

		nPos--;
	}

	return 0;
}


/////////////////////////////////////////////////////////////////////////////
// SelectWordAt
//
void TextEdit::SelectWordAt(tCharIndex nPos)
{
	tCharIndex nTextLength = TextEdit::GetTextLength();

	if(nTextLength == 0)
	{
		SetCursorIndex(0, false);
		return;
	}

	if(nPos >= nTextLength)
		nPos = nTextLength - 1;

	tCharIndex  ws = nPos;
	tCharIndex  we = nPos;

	//Char c = mText[nPos];
	if(nPos > 1 && EA::StdC::Isspace(mText[nPos]) && EA::StdC::Isspace(mText[nPos - 1]))
	{
		ws = FindPrevWordEnd(nPos + 1);
		we = FindNextWord(nPos);
	}
	else
	{
		ws = FindPrevWord(nPos + 1);
		we = FindNextWordEnd(nPos);

		if(we < nTextLength && EA::StdC::Isspace(mText[we]))
			we++;
	}

	SetCursorAndAnchor(ws, we);
}


/////////////////////////////////////////////////////////////////////////////
// GetScrollPosition
//
const Point2DFloat &TextEdit::GetScrollPosition() const
{
	return mScrollPosition;
}


/////////////////////////////////////////////////////////////////////////////
// SetScrollPosition
//
void TextEdit::SetScrollPosition(const Point2DFloat& pos)
{
	if(mScrollPosition != pos)
	{
		mScrollPosition = pos;

		//if(mScrollbars[kScrollbarVertical].mScrollbar != NULL)
		//    mScrollbars[kScrollbarVertical].mScrollbar->SetValue((int32_t)pos.mY);

		//if(mScrollbars[kScrollbarHorizontal].mScrollbar != NULL)
		//    mScrollbars[kScrollbarHorizontal].mScrollbar->SetValue((int32_t)pos.mX);

		Invalidate();
	}
}


/////////////////////////////////////////////////////////////////////////////
// ScrollToCursor
//
void TextEdit::ScrollToCursor()
{
	if(mDirtyFlags & (kDirtyFlagCursorPosition|kDirtyFlagLineBreaks))
		RecalculateCursorPosition();

	Point2DFloat newScrollPosition(mScrollPosition);

	newScrollPosition.mX = eastl::min_alt(newScrollPosition.mX, mTextExtent.mX - mVisibleArea.GetWidth() + 3.0f);
	newScrollPosition.mX = eastl::max_alt(newScrollPosition.mX, mCaretRect.mRight - mVisibleArea.GetWidth());
	newScrollPosition.mX = eastl::min_alt(newScrollPosition.mX, mCaretRect.mLeft);
	newScrollPosition.mX = eastl::max_alt(newScrollPosition.mX, 0.f);

	newScrollPosition.mY = eastl::min_alt(newScrollPosition.mY, mTextExtent.mY - mVisibleArea.GetHeight() + 3.0f);
	newScrollPosition.mY = eastl::max_alt(newScrollPosition.mY, mCaretRect.mBottom - mVisibleArea.GetHeight());
	newScrollPosition.mY = eastl::min_alt(newScrollPosition.mY, mCaretRect.mTop);
	newScrollPosition.mY = eastl::max_alt(newScrollPosition.mY, 0.f);

	if(newScrollPosition != mScrollPosition)
		SetScrollPosition(newScrollPosition);
}


/////////////////////////////////////////////////////////////////////////////
// GetTextExtent
//
Point2DFloat TextEdit::GetTextExtent() const
{
	if(mDirtyFlags & kDirtyFlagLineBreaks)
		const_cast<TextEdit*>(this)->RecalculateLineBreaks();

	return mTextExtent;
}


/////////////////////////////////////////////////////////////////////////////
// GetScrollbarDrawable
//
//IScrollbarDrawable* TextEdit::GetScrollbarDrawable(tScrollbar nWhichScrollbar) const
//{
//    EA_ASSERT(nWhichScrollbar >= 0);
//    EA_ASSERT(nWhichScrollbar < kScrollbarCount);
//
//    return mScrollbars[nWhichScrollbar].mDrawable;
//}


/////////////////////////////////////////////////////////////////////////////
// SetScrollbarDrawable
//
//void TextEdit::SetScrollbarDrawable(tScrollbar nWhichScrollbar, IScrollbarDrawable* pScrollbarDrawable)
//{
//    EA_ASSERT(nWhichScrollbar >= 0);
//    EA_ASSERT(nWhichScrollbar < kScrollbarCount);
//
//    mScrollbars[nWhichScrollbar].mDrawable = pScrollbarDrawable;
//    CreateScrollbars();
//}


/////////////////////////////////////////////////////////////////////////////
// GetScrollbarEnabled
//
TextEdit::tScrollbarEnabled TextEdit::GetScrollbarEnabled(tScrollbar nWhichScrollbar) const
{
	return mScrollbars[nWhichScrollbar].mEnabled;
}


/////////////////////////////////////////////////////////////////////////////
// SetScrollbarEnabled
//
void TextEdit::SetScrollbarEnabled(tScrollbar nWhichScrollbar, tScrollbarEnabled nEnabled)
{
	mScrollbars[nWhichScrollbar].mEnabled = nEnabled;
	CreateScrollbars();
}


/////////////////////////////////////////////////////////////////////////////
// GetScrollbarVisible
//
bool TextEdit::IsScrollbarVisible(tScrollbar /*nWhichScrollbar*/) const
{
	//return (mScrollbars[nWhichScrollbar].mScrollbar != NULL) &&
	//       ((mScrollbars[nWhichScrollbar].mScrollbar->AsIWindow()->GetFlags() &  kWinFlagVisible) != 0);

	return false;
}


/////////////////////////////////////////////////////////////////////////////
// RecalculateCursorPosition
//
void TextEdit::RecalculateCursorPosition()
{
	using namespace EA::Text;

	if(mDirtyFlags & kDirtyFlagLineBreaks)
		RecalculateLineBreaks();

	mDirtyFlags &= ~kDirtyFlagCursorPosition;

	tParagraphList::const_iterator itPara = FindParagraphByTextIndex(mCursorIndex);
	Paragraph& para = const_cast<Paragraph&>(*itPara);

	LayoutParagraph(para);

	// Get the line number, given the text index
	tTextIndexArray::const_iterator itLine = eastl::upper_bound(mLineBreaks.begin(), mLineBreaks.end(), mCursorIndex - para.mTextIndex) - 1;
	mCursorParaLine = (uint32_t)(itLine - mLineBreaks.begin());

	tCharIndex  nLineStartIndex, nLineLength;
	LineLayout& ll = FillLineLayout(para, mCursorParaLine, nLineStartIndex, nLineLength);

	mCursorColumn = mCursorIndex - para.mTextIndex - nLineStartIndex;

	if(nLineLength == 0)
		mCaretRect.mLeft = 0;
	else
	{
		const float fDisplayPosition = ll.GetDisplayPositionFromTextPosition(mCursorColumn, kGlyphSideBefore, kDirectionDefault);
		mCaretRect.mLeft = eastl::max_alt(0.f, fDisplayPosition);
	}

	const float fCaretWidth = ((mActualStyle.mfSize < 20) ? 1 : ((mActualStyle.mfSize < 40) ? 2 : mActualStyle.mfSize / 20));

	mCaretRect.mTop    = para.mYPos + (mCursorParaLine * mLineHeight);
	mCaretRect.mBottom = mCaretRect.mTop + mLineHeight;
	mCaretRect.mRight  = mCaretRect.mLeft + fCaretWidth;

	if(mDirtyFlags & kDirtyFlagSetCursorSeek)
	{
		mCursorSeekPositionX = mCaretRect.mLeft;
		mDirtyFlags &= ~kDirtyFlagSetCursorSeek;
	}

	ShowCaret(true);
	mCaretTimer.Restart();
}


/////////////////////////////////////////////////////////////////////////////
// RecalculateLineBreaks
//
void TextEdit::RecalculateLineBreaks()
{
	using namespace EA::Text;

	mDirtyFlags &= ~kDirtyFlagLineBreaks;

	tCharIndex nTextLength = TextEdit::GetTextLength();

	switch (mLineWrapMode)
	{
		case kLineWrapModeSingleLine:
		case kLineWrapModeMultiLine:
			mActualStyle.mTextWrap = kTWNone;
			break;

		case kLineWrapModeMultiLineWordWrap:
			mActualStyle.mTextWrap = kTWDefault;
			break;
	}

	// Create a text run representing the entire text
	TextRun textRun;
	textRun.mpText = &mText[0];
	textRun.mnTextSize = nTextLength;

	// Break up the text into paragraphs
	mParagraphList.clear();
	ParagraphBreakIterator paraIter(&textRun, 1);
	tCharIndex nParaStart = 0;

	while(!paraIter.AtEnd())
	{
		tCharIndex  nParaNext = paraIter.GetNextParagraphBreak();
		mParagraphList.push_back(Paragraph(nParaStart, nParaNext - nParaStart));
		nParaStart = nParaNext;
	}

	// If there was a trailing carriage return on the final paragraph, then
	// add an extra, empty paragrah at the end to hold the insertion point.
	if(mParagraphList.empty() || mParagraphList.back().mTextIndex + mParagraphList.back().mTextLength < nTextLength)
		mParagraphList.push_back(Paragraph(nTextLength, 0));

	Typesetter& typesetter = *GetTypesetter();
	typesetter.Reset();
	typesetter.SetDefaultTextStyle(&mActualStyle);
	mTextExtent.mY = 0.0f;
	mTextExtent.mX = 0.0f;

	for(tParagraphList::iterator it = mParagraphList.begin(), itEnd = mParagraphList.end(); it != itEnd; ++it)
	{
		Paragraph& para = *it;

		para.mYPos = mTextExtent.mY;
		LayoutParagraph(para);

		// Update the widest line value.
		mTextExtent.mX  = eastl::max_alt(mTextExtent.mX, para.mExtent.mX);
		mTextExtent.mY += para.mExtent.mY;
	}
}


/////////////////////////////////////////////////////////////////////////////
// FindParagraphByPosition
//
TextEdit::tParagraphList::const_iterator TextEdit::FindParagraphByPosition(float yPos) const
{
	return eastl::upper_bound(mParagraphList.begin(), mParagraphList.end(), yPos, Paragraph::YPosComparator()) - 1;
}


/////////////////////////////////////////////////////////////////////////////
// FindParagraphByTextIndex
//
TextEdit::tParagraphList::const_iterator TextEdit::FindParagraphByTextIndex(tCharIndex nIndex) const
{
	return eastl::upper_bound(mParagraphList.begin(), mParagraphList.end(), nIndex, Paragraph::TextIndexComparator()) - 1;
}


/////////////////////////////////////////////////////////////////////////////
// Pick
//
TextEdit::tCharIndex TextEdit::Pick(float x, float y, bool bClamp)
{
	if(mDirtyFlags & kDirtyFlagLineBreaks)
		RecalculateLineBreaks();

	// Get our standard layout constants.
	const uint32_t nTextLength = (uint32_t)mText.size();

	// Adjust for scroll position.
	x += mScrollPosition.mX;
	y += mScrollPosition.mY;

	// Find the paragraph that was picked. If its outside the range of valid lines,
	// either clamp it, or return invalid result.
	if(y < 0)
	{
		if(bClamp) 
			return 0;
		else 
			return kInvalidCharIndex;
	}

	if(y >= mTextExtent.mY)
	{
		if(bClamp) 
			return nTextLength;
		else 
			return kInvalidCharIndex;
	}

	// Find which paragraph we want
	tParagraphList::const_iterator it = FindParagraphByPosition(y);

	// Search the paragraph
	LayoutParagraph(const_cast<Paragraph&>(*it));
	tCharIndex nResult = PickParagraph(*it, Point2DFloat(x, y), bClamp) + it->mTextIndex;
	return nResult;
}


/////////////////////////////////////////////////////////////////////////////
// OnMouseDown
//
bool TextEdit::OnMouseDown(float_t x, float_t y, uint32_t /*button*/, uint32_t modifiers)
{
	const bool bReadOnly             = TextEdit::GetTextEditFlag(kEditFlagReadOnly);
	const bool bEnableClipboard      = TextEdit::GetTextEditFlag(kEditFlagEnableClipboard);
	const bool bEnableCursorMovement = bEnableClipboard || !bReadOnly;

	tCharIndex nPickIndex = Pick(x, y, true);

	if(bEnableCursorMovement)
	{
		bool bDoubleClickDetected(false);

		// Detect double-click
		bDoubleClickDetected = (mDoubleClickTimer.IsRunning() && (mDoubleClickTimer.GetElapsedTime() < mDoubleClickInterval));
		mDoubleClickTimer.Restart();

		if(nPickIndex != kInvalidCharIndex)
		{
			if(bDoubleClickDetected && (nPickIndex == mCursorIndex))
				SelectWordAt(nPickIndex);
			else
			{
				SetCursorIndex(nPickIndex, (modifiers & EA::Input::kModifierMaskShift) != 0);
				NotifyCapture(kCaptureRequestAcquire);
			}
		}
	}

	return true;
}


/////////////////////////////////////////////////////////////////////////////
// OnMouseUp
//
bool TextEdit::OnMouseUp(float_t /*x*/, float_t /*y*/, uint32_t /*button*/, uint32_t /*modifiers*/)
{
	if(NotifyCapture(kCaptureRequestQuery)) // If we have capture...
		NotifyCapture(kCaptureRequestRelease); // Release capture

	return true;
}


/////////////////////////////////////////////////////////////////////////////
// OnMouseMove
//
bool TextEdit::OnMouseMove(float_t x, float_t y, uint32_t /*modifiers*/)
{
	if(NotifyCapture(kCaptureRequestQuery)) // Should really be: if(AmDragging())
	{
		const bool bReadOnly             = TextEdit::GetTextEditFlag(kEditFlagReadOnly);
		const bool bEnableClipboard      = TextEdit::GetTextEditFlag(kEditFlagEnableClipboard);
		const bool bEnableCursorMovement = bEnableClipboard || !bReadOnly;

		if(bEnableCursorMovement)
		{
			const tCharIndex nPickIndex = Pick(x, y, true);
			SetCursorIndex(nPickIndex, true);
		}
	}

	return true;
}


/////////////////////////////////////////////////////////////////////////////
// OnMouseWheel
//
bool TextEdit::OnMouseWheel(float_t /*x*/, float_t /*y*/, uint32_t /*modifiers*/, int32_t zDelta)
{
	if(!NotifyCapture(kCaptureRequestQuery)) // If we don't have capture (and thus am not busy drag-highlighting)...
	{
		Point2DFloat newScrollPosition(mScrollPosition);

		newScrollPosition.mY -= (float)(zDelta / 4);

		newScrollPosition.mY = eastl::min_alt(newScrollPosition.mY, (mTextExtent.mY - mVisibleArea.GetHeight()) + 3.0f);
		newScrollPosition.mY = eastl::max_alt(newScrollPosition.mY, 0.f);

		if(newScrollPosition != mScrollPosition)
			SetScrollPosition(newScrollPosition);
	}

	return false;
}


/////////////////////////////////////////////////////////////////////////////
// OnFocusChange
//
bool TextEdit::OnFocusChange(int /*context*/, void* /*pNewFocus*/)
{
	//if(context == IWindowMgr::kICKeyboard)
	{
		ShowCaret(true);
		mCaretTimer.Restart();
	}

	return false;
}


/////////////////////////////////////////////////////////////////////////////
// OnKeyDown
//
//  Return key       Normally creates a new line, but we don't do that here. See below.
//  Backspace key    Deletes the character before the insertion position.
//  Delete key       Deletes the character after the insertion position.
//  Up arrow key     Moves insertion position up one line.
//  Down arrow key   Moves insertion position down one line.
//  Page up key      Moves insertion position (and view) up one page.
//  Page down key    Moves insertion position (and view) down one page.
//  Left arrow key   Moves insertion position left one character index, or one word if control is down, or to line start if alt is down.
//  Right arrow key  Moves insertion position right one character index, or one word if control is down, or to line end if alt is down.
//  Home key         Moves insertion position to beginning of line (or document?).
//  End key          Moves insertion position to end of line (or document?).
//  Insert key       Toggles between insert and overwrite modes. 
//
//  With the following we have some localization issues because 
//  the keys to implement these vary based on locale. 
//
//  Cut key          Control-X 'cuts' the currently selected text to the clipboard.
//  Copy key         Control-C 'copies' the currently selected text to the clipboard.
//  Paste key        Control-V 'pastes' the currently selected text from the clipboard.
//
//  Undo key         Control-Z undoes the last operation.
//  Redo key         Control-Y redoes the last undo operation.
//
bool TextEdit::OnKeyDown(int /*context*/, uint32_t key, uint32_t modifiers)
{
	const bool bShiftDepressed  ((modifiers & EA::Input::kModifierMaskShift) != 0);
	const bool bControlDepressed((modifiers & EA::Input::kModifierMaskControl)  != 0);
	const bool bAltDepressed    ((modifiers & EA::Input::kModifierMaskAlt)   != 0);

	const bool bReadOnly             = TextEdit::GetTextEditFlag(kEditFlagReadOnly);
	const bool bEnableClipboard      = TextEdit::GetTextEditFlag(kEditFlagEnableClipboard);
	const bool bEnableCursorMovement = bEnableClipboard || !bReadOnly;

	switch (key)
	{
		case EA::Input::kEnter:
			if(!(mTextEditFlags & kEditFlagAcceptsReturn))
			{
				// Sending again, this time as a command message.
				//Message msg(Message::kTypeCtrlActivated); 
				//msg.mArgs.m1  = GetCommandId();
				//msg.mArgs.m3  = (int32_t)mText.size();
				//msg.mArgs.m4p = (void *)&mText[0];
				//SendMessage(msg);

				return false;
			}

			break; // Otherwise handle it in OnChar
		
		case EA::Input::kTab:
			// Let the parent window handle it.
			return false;

		case EA::Input::kInsert:
			if(mTextEditFlags & kEditFlagAcceptsInsertKey)
				SetTextEditFlag(kEditFlagOverwrite, !GetTextEditFlag(kEditFlagOverwrite));
			else
				return false; // Let the parent window handle it.
			break;

		case EA::Input::kBackspace:
			if(!bReadOnly)
			{
				if(mCursorIndex == mAnchorIndex)
				{
					if(mCursorIndex > 0)
						Replace(mCursorIndex - 1, 1, NULL, 0, true);  // Cursor will be adjusted automatically
					else
						NotifyError(kEditErrorCursorLimit);         // Report error
				}
				else
					ReplaceSelection(NULL, 0, true);
			}

			break;

		case EA::Input::kDelete:
			if(!bReadOnly)
			{
				if(mCursorIndex == mAnchorIndex)
				{
					if(mCursorIndex < TextEdit::GetTextLength())
						Replace(mCursorIndex, 1, NULL, 0, true); // Cursor will be adjusted automatically
					else
						NotifyError(kEditErrorCursorLimit); // Report error
				}
				else
					ReplaceSelection(NULL, 0, true);
			}
			break;

		case EA::Input::kArrowUp:
			if(mLineWrapMode != kLineWrapModeSingleLine && bEnableCursorMovement)
			{
				if(!MoveCursor(kMoveByLine, -1, bShiftDepressed))
					NotifyError(kEditErrorCursorLimit);
			}
			break;

		case EA::Input::kArrowDown:
			if(mLineWrapMode != kLineWrapModeSingleLine && bEnableCursorMovement)
			{
				if(!MoveCursor(kMoveByLine, 1, bShiftDepressed))
					NotifyError(kEditErrorCursorLimit);
			}
			break;

		case EA::Input::kArrowLeft:
			if(bEnableCursorMovement)
			{
				tMoveType mt = kMoveByCharacter;

				if(bControlDepressed && bAltDepressed)
					mt = kMoveBySentance;
				else if(bControlDepressed)
					mt = kMoveByWord;
				else if(bAltDepressed)
					mt = kMoveByParagraph;

				if(!MoveCursor(mt, -1, bShiftDepressed))
					NotifyError(kEditErrorCursorLimit);
			}
			break;

		case EA::Input::kArrowRight:
			if(bEnableCursorMovement)
			{
				tMoveType mt = kMoveByCharacter;

				if(bControlDepressed && bAltDepressed)
					mt = kMoveBySentance;
				else if(bControlDepressed)
					mt = kMoveByWord;
				else if(bAltDepressed)
					mt = kMoveByParagraph;

				if(!MoveCursor(mt, 1, bShiftDepressed))
					NotifyError(kEditErrorCursorLimit);
			}
			break;

		case EA::Input::kHome:
			if(bAltDepressed || bControlDepressed)
				MoveCursor(kMoveToBeginning, 0, bShiftDepressed);
			else
				MoveCursor(kMoveToLineBeginning, 0, bShiftDepressed);
			break;

		case EA::Input::kEnd:
			if(bAltDepressed || bControlDepressed)
				MoveCursor(kMoveToEnd, 0, bShiftDepressed);
			else
				MoveCursor(kMoveToLineEnd, 0, bShiftDepressed);
			break;

		case EA::Input::kPageUp:
			if(bEnableCursorMovement)
			{
				if(!MoveCursor(kMoveByPage, -1, bShiftDepressed))
					NotifyError(kEditErrorCursorLimit);
			}
			break;

		case EA::Input::kPageDown:
			if(bEnableCursorMovement)
			{
				if(!MoveCursor(kMoveByPage, 1, bShiftDepressed))
					NotifyError(kEditErrorCursorLimit);
			}
			break;

		case EA::Input::kEscape:
			// Escape clears the current selection. Does this by setting the anchor pos to the cursor pos.
			SetCursorIndex(mCursorIndex, true);
			break;

		case EA::Input::kLetterX:
		case EA::Input::kLetterC:
		{
			#if EA_PLATFORM_WINDOWS
				// Control-X means Cut. We need to deal with localization issues with this key.
				// Control-C means Copy. We need to deal with localization issues with this key.
				if(bControlDepressed && !bShiftDepressed && !bAltDepressed)
				{
					const bool bPasswordMode = TextEdit::GetTextEditFlag(kEditFlagPasswordMode);

					if(bEnableClipboard && !bPasswordMode)
					{
						if(!NotifyCapture(kCaptureRequestQuery)) // If we don't have capture (and thus am not busy drag-highlighting)...
						{
							using namespace EA::ClipboardSys;

							// We ignore characters while drag-selecting, like other text editors.
							tCharIndex nSelectionStart, nSelectionEnd;
							Clipboard* const pClipboard = GetClipboard();

							if(pClipboard && TextEdit::GetSelectionRange(nSelectionStart, nSelectionEnd))
							{
								#if (EATEXT_CHAR_TYPE_SIZE == 2)
									DataText16* const pDataText16 = DataText16::CreateDataText16(&mText[nSelectionStart], nSelectionEnd - nSelectionStart);

									if(pDataText16)
									{
										const bool bReturnValue = pClipboard->SetClipboardData(pDataText16, true);

										pDataText16->Release();

										if(!bReturnValue)
											NotifyError(kEditErrorMisc);
									}
								#else
									// To do: Implement support for 32 bit characters.
									NotifyError(kEditErrorMisc);
								#endif
							}

							if(key == EA::Input::kLetterX && !bReadOnly)
								ReplaceSelection(NULL, 0, true);
						}
					}
				}
			#endif
			break;
		}

		case EA::Input::kLetterV:
		{
			#if EA_PLATFORM_WINDOWS
				if(bControlDepressed && !bShiftDepressed && !bAltDepressed)
				{
					// We ignore characters while drag-selecting, like other text editors.
					if(!NotifyCapture(kCaptureRequestQuery)) // If we don't have capture (and thus am not busy drag-highlighting)...
					{
						using namespace EA::ClipboardSys;

						Clipboard* const pClipboard = GetClipboard();

						if(bEnableClipboard && !bReadOnly && pClipboard != NULL)
						{
							IData* pData;

							if(pClipboard->GetClipboardData(kDataTypeText16, &pData)) // AddRefs pData.
							{
								const size_t nClipLength = (pData->GetSize() / sizeof(Char)) - 1;

								ReplaceSelection((Char*)pData->GetData(), (tCharIndex)nClipLength, true);
								pData->Release();
							}
						}
					}
				}
			#endif
			break;
		}

		case EA::Input::kLetterZ:
			if(bControlDepressed && !bAltDepressed)
			{
				if(CanUndo())
					Undo();
				else
					NotifyError(kEditErrorUndoLimitReached);
			}
			break;

		case EA::Input::kLetterY:
			if(bControlDepressed && !bAltDepressed)
			{
				if(CanRedo())
					Redo();
				else
					NotifyError(kEditErrorUndoLimitReached);
			}
			break;

		case EA::Input::kSpacebar:
		case EA::Input::kNumpadMultiply:
		case EA::Input::kNumpadPlus:
		case EA::Input::kNumpadMinus:
		case EA::Input::kNumpadDecimal:
		case EA::Input::kNumpadDivide:
		case EA::Input::kSemicolon:
		case EA::Input::kEquals:
		case EA::Input::kComma:
		case EA::Input::kMinus:
		case EA::Input::kPeriod:
		case EA::Input::kForwardSlash:
		case EA::Input::kBackSlash:
		case EA::Input::kAccent:
		case EA::Input::kOpenBracket:
		case EA::Input::kSingleQuote:
			break;

		default:
			// Since this class handles OnChar message and accepts all characters, it should also 
			// capture OnKeyDown for those characters. 
			if((key >= EA::Input::kLetterA && key <= EA::Input::kLetterZ) ||
			   (key >= EA::Input::kDigit0  && key <= EA::Input::kDigit9))
			{
				return true;
			}

			return false;
	}

	return true;
}


/////////////////////////////////////////////////////////////////////////////
// OnKeyUp
//
bool TextEdit::OnKeyUp(int /*context*/, uint32_t key, uint32_t /*modifiers*/)
{
	switch (key)
	{
		// Since this class handles OnChar message and accepts all characters, 
		// it should also capture OnKeyUp for those characters. 
		case EA::Input::kSpacebar:
		case EA::Input::kNumpadMultiply:
		case EA::Input::kNumpadPlus:
		case EA::Input::kNumpadMinus:
		case EA::Input::kNumpadDecimal:
		case EA::Input::kNumpadDivide:
		case EA::Input::kSemicolon:
		case EA::Input::kEquals:
		case EA::Input::kComma:
		case EA::Input::kMinus:
		case EA::Input::kPeriod:
		case EA::Input::kForwardSlash:
		case EA::Input::kBackSlash:
		case EA::Input::kAccent:
		case EA::Input::kOpenBracket:
		case EA::Input::kSingleQuote:
			break;

		default:
			// Since this class handles OnChar message and accepts all characters, 
			// it should also capture OnKeyUp for those characters. 
			if((key >= EA::Input::kLetterA && key <= EA::Input::kLetterZ) ||
			   (key >= EA::Input::kDigit0  && key <= EA::Input::kDigit9))
			{
				return true;
			}
			return false;
	}
	return true;
}


/////////////////////////////////////////////////////////////////////////////
// OnChar
//
bool TextEdit::OnChar(int /*context*/, Char character)
{
	const bool bReadOnly = TextEdit::GetTextEditFlag(kEditFlagReadOnly);

	if(!bReadOnly)
	{
		if(character < 32)
		{
			switch (character)
			{
				case '\r':
					// Transform CR into NL
					if(!(mTextEditFlags & kEditFlagAcceptsReturn))
						return false;
					character = '\n';
					break;

				case '\n':
					if(!(mTextEditFlags & kEditFlagAcceptsReturn))
						return false;
					break;

				case '\t':
					return false;

				default:
					// We ignore these characters, especially the backspace and delete keys.
					return false;
			}
		}

		// Insert the character into the text
		if(mCursorIndex == mAnchorIndex && TextEdit::GetTextEditFlag(kEditFlagOverwrite))
			Replace(mCursorIndex, 1, &character, 1, true);
		else
			ReplaceSelection(&character, 1, true);
	}

	return true;
}


/////////////////////////////////////////////////////////////////////////////
// OnTick
//
bool TextEdit::OnTick()
{
	// Do recalculations if needed.
	if(mDirtyFlags & (kDirtyFlagCursorPosition | kDirtyFlagLineBreaks))
	{
		if(mSuspendUpdateLevel == 0)
		{
			ScrollToCursor();
			UpdateScrollbars();
		}
	}

	if(mTextEditFlags & (kEditFlagHideCaret | kEditFlagReadOnly)) 
		mCaretVisible = false;
	else
	{
		if(mCaretTimer.GetElapsedTime() > mCaretPeriod)
		{
			ShowCaret(!mCaretVisible);
			mCaretTimer.Restart();
		}
	}

	return true;
}


/////////////////////////////////////////////////////////////////////////////
// ShowCaret
//
void TextEdit::ShowCaret(bool bToggleState)
{
	//if((mFlags & kWinFlagVisible) == 0) 
	//    bToggleState = false;

	if(mTextEditFlags & (kEditFlagHideCaret | kEditFlagReadOnly)) 
		bToggleState = false;

	//if(!HasFocus(IWindowMgr::kICKeyboard)) 
	//    bToggleState = false;

	if(bToggleState != mCaretVisible)
	{
		mCaretVisible = bToggleState;
		Invalidate();
	}
}


/////////////////////////////////////////////////////////////////////////////
// OnRebuild
//
bool TextEdit::OnRebuild(void* pDrawContext)
{
	using namespace EA::Text;
	using namespace EA::UTFDraw2D;

	RectFloat     rArea(0, 0, mPhysicalArea.GetWidth(), mPhysicalArea.GetHeight());
	IDrawContext& d        = *(EA::UTFDraw2D::IDrawContext*)pDrawContext;
	bool          bFocused = true; // HasFocus(IWindowMgr::kICKeyboard);
	bool          bEnabled = true; // (GetFlags() & kWinFlagEnabled) != 0;

	bool bShowSelection = true;
	if(!bFocused && (mTextEditFlags & kEditFlagHideInactiveSelection))
		bShowSelection = false;

	// Recalculate whatever has changed
	if(mDirtyFlags & kDirtyFlagLineBreaks)
		RecalculateLineBreaks();

	// Draw the background.
	//if(mpFillDrawable)
	//{
	//    DrawParams params(kDrawStateNormal, 0, mFillColor, mShadeColor);
	//    bool       bCapture = NotifyCapture(kCaptureRequestQuery);
	//
	//    if(!bEnabled) 
	//        params.mState = kDrawStateDisabled;
	//    else if(bCapture) 
	//        params.mState = kDrawStateActive;
	//
	//    mpFillDrawable->CreateRenderables(rc, rArea, params);
	//}
	//else
	//{
		int nFillColorIndex = kColorBackground;
		if(!bEnabled) 
			nFillColorIndex = kColorBackgroundDisabled;

		if(mColors[nFillColorIndex])
		{
			d.SetColor(mColors[nFillColorIndex]);
			d.Fill(rArea.mLeft, rArea.mTop, rArea.mRight, rArea.mBottom);
		}
	//}

	d.IntersectClip(mVisibleArea);

	tCharIndex nSelStart, nSelEnd;

	// Calculate the selection range
	TextEdit::GetSelectionRange(nSelStart, nSelEnd);
	if(mCursorIndex == mAnchorIndex) 
		nSelEnd += 1;   // Overwrite cursor size; We'll handle insert below

	tParagraphList::const_iterator paraFirst = FindParagraphByPosition(mScrollPosition.mY);
	tParagraphList::const_iterator paraLast  = FindParagraphByPosition(mScrollPosition.mY + mVisibleArea.GetHeight()) + 1;

	Typesetter& typesetter = *GetTypesetter();

	for(tParagraphList::const_iterator it = paraFirst; it != paraLast; ++it)
	{
		Paragraph& para = const_cast<Paragraph&>(*it);

		typesetter.Reset();
		typesetter.SetDefaultTextStyle(&mActualStyle);
		typesetter.AddTextRun(&mText[para.mTextIndex], para.mTextLength);

		LineLayout& llayout = typesetter.GetLineLayout();

		float   fYPos = mVisibleArea.mTop + para.mYPos - mScrollPosition.mY;
		float   fXPos = mVisibleArea.mLeft - mScrollPosition.mX;
		int32_t nLineStartIndex = 0;

		typesetter.SetLayoutSpace(mVisibleArea.GetWidth(), fXPos, fYPos);

		while(typesetter.GetLineState() != Typesetter::kLineStateComplete)
		{
			typesetter.FinalizeLine();

			// Compute the selection range, relative to this line.
			// Note that these need to be signed, since they are relative to the start
			// of the para, and can therefore be negative.
			int32_t     nLineSelStart = nSelStart - nLineStartIndex - para.mTextIndex;
			int32_t     nLineSelEnd = nSelEnd - nLineStartIndex - para.mTextIndex;
			int32_t     nLineLength = (int32_t)llayout.mCharArray.size();
			int32_t     nTextEnd = nLineStartIndex + nLineLength;
			float       fLineTop = fYPos;
			float       fLineBottom = fLineTop + mLineHeight;

			// If the selection overlaps the line (or this is the last line of the last para,
			// and the selection is at the end of the text.)
			if((mCaretVisible || (mCursorIndex != mAnchorIndex && bShowSelection)) &&
			   ((nLineSelStart < nLineLength && nLineSelEnd > 0) ||
				(nLineSelStart == nLineLength && nLineSelStart + nLineStartIndex == (int32_t)para.mTextLength)))
			{
				// The selection overlaps the current line
				if(mCursorIndex == mAnchorIndex && !(mTextEditFlags & kEditFlagOverwrite))
				{
					// Insertion cursor

					// Draw the text line normally
					d.SetColor(mColors[kColorText]);
					d.DrawTextGlyphs(llayout);

					// Draw the caret
					// To consider: We already have mCaretRect and should be able to use that here.
					float fCursorX = fXPos;

					if(nLineLength > 0)
					{
						const float fDisplayPosition = llayout.GetDisplayPositionFromTextPosition(nLineSelStart, kGlyphSideBefore, kDirectionDefault);
						fCursorX = eastl::max_alt(0.f, fDisplayPosition);
					}

					if(nLineSelStart > 0)
						fCursorX -= 1.0f;

					d.SetColor(mColors[kColorBackgroundCaret]);
					d.Fill(fCursorX, fLineTop, fCursorX + mCaretRect.GetWidth(), fLineBottom);
				}
				else if(nLineSelStart <= 0 && nLineSelEnd >= nLineLength && nLineSelEnd != nTextEnd)
				{
					// Entire line is selected.
					d.SetColor(mColors[kColorBackgroundSelected]);
					d.Fill(mVisibleArea.mLeft, fLineTop, mVisibleArea.mLeft + llayout.mLineMetrics.mfSpace, fLineBottom);
					d.SetColor(mColors[kColorTextSelected]);
					d.DrawTextGlyphs(llayout);
				}
				else
				{
					// Only a part of the line is selected.
					// Calculate the selection index relative to the start of the line (clamped)

					nLineSelStart = eastl::max_alt(nLineSelStart, (int32_t)0);
					nLineSelEnd   = eastl::min_alt(nLineSelEnd, nLineLength);

					if(nLineSelStart < nLineSelEnd)
					{
						RectangleSet rects;
						llayout.GetGlyphSelection(nLineSelStart, nLineSelEnd, rects);

						d.SetColor(mColors[kColorBackgroundSelected]);
						for(RectangleSet::iterator rectIt = rects.begin(); rectIt != rects.end(); ++rectIt)
							d.Fill(rectIt->mLeft, fYPos, rectIt->mRight, fYPos + mLineHeight);
					}

					eastl_size_t nGlyphLineStart   = llayout.GetGlyphIndexFromCharIndex(0);
					eastl_size_t nGlyphSelStart    = llayout.GetGlyphIndexFromCharIndex(nLineSelStart);
					eastl_size_t nGlyphSelEnd      = llayout.GetGlyphIndexFromCharIndex(nLineSelEnd);
					eastl_size_t nGlyphLineEnd     = llayout.GetGlyphIndexFromCharIndex(nLineLength);

					// Draw the part before the selection
					if(nGlyphSelStart > nGlyphLineStart)
					{
						d.SetColor(mColors[kColorText]);
						d.DrawTextGlyphs(&llayout.mGlyphArray[nGlyphLineStart],
										 &llayout.mGlyphLayoutInfoArray[nGlyphLineStart],
										 nGlyphSelStart - nGlyphLineStart);
					}

					// Draw the selected part
					if(nGlyphSelEnd > nGlyphSelStart)
					{
						if(mCursorIndex == mAnchorIndex)
						{
							// Special case of overwrite cursor
							d.SetColor(mColors[kColorTextCaret]);
						}
						else
						{
							// Regular selection region
							d.SetColor(mColors[kColorTextSelected]);
						}

						d.DrawTextGlyphs(&llayout.mGlyphArray[nGlyphSelStart],
										 &llayout.mGlyphLayoutInfoArray[nGlyphSelStart],
										 nGlyphSelEnd - nGlyphSelStart);
					}

					// Draw the part after the selection
					if(nGlyphLineEnd > nGlyphSelEnd)
					{
						d.SetColor(mColors[kColorText]);
						d.DrawTextGlyphs(&llayout.mGlyphArray[nGlyphSelEnd],
										 &llayout.mGlyphLayoutInfoArray[nGlyphSelEnd],
										 nGlyphLineEnd - nGlyphSelEnd);
					}
				}
			}
			else
			{
				d.SetColor(mColors[kColorText]);
				d.DrawTextGlyphs(llayout);
			}

			fYPos += mLineHeight;
			nLineStartIndex += nLineLength;
			typesetter.SetLayoutSpace(mVisibleArea.GetWidth(), mVisibleArea.mLeft, fYPos);
			typesetter.NextLine();
		}
	}

	return true;
}


/////////////////////////////////////////////////////////////////////////////
// HandleMessage
//
bool TextEdit::HandleMessage(const TextEditMessage& msg)
{
	switch (msg.mType)
	{
		case Message::kTypeKeyDown:
			return OnKeyDown(msg.mKeyEvent.mContext, msg.mKeyEvent.mKey, msg.mKeyEvent.mModifiers);

		case Message::kTypeKeyUp:
			return OnKeyUp(msg.mKeyEvent.mContext, msg.mKeyEvent.mKey, msg.mKeyEvent.mModifiers);

		case Message::kTypeChar:
			return OnChar(msg.mKeyEvent.mContext, (Char)msg.mKeyEvent.mKey);

		case Message::kTypeMouseDown:
			return OnMouseDown(msg.mMouseEvent.mX, msg.mMouseEvent.mY, msg.mMouseEvent.mButton, msg.mMouseEvent.mModifiers);

		case Message::kTypeMouseUp:
			return OnMouseUp(msg.mMouseEvent.mX, msg.mMouseEvent.mY, msg.mMouseEvent.mButton, msg.mMouseEvent.mModifiers);

		case Message::kTypeMouseMove:
			return OnMouseMove(msg.mMouseEvent.mX, msg.mMouseEvent.mY, msg.mMouseEvent.mModifiers);

		case Message::kTypeMouseWheel:
			return OnMouseWheel(msg.mMouseEvent.mX, msg.mMouseEvent.mY, msg.mMouseEvent.mModifiers, msg.mMouseEvent.mZDelta);

		case Message::kTypeTick:
			return OnTick();

		case Message::kTypeAreaChanged:
			OnAreaChanged(*(static_cast<RectFloat*>(msg.mArgs.m4p)));
			return false;

		case Message::kTypeRebuild:
			return OnRebuild(msg.mRebuildEvent.mpDC);

		//case Message::kTypeFocusChange:
		//    return OnFocusChange(msg.mFocusEvent.mContext, msg.mFocusEvent.mpTarget);

		//case Message::kTypeCalcSizeQuery:
		//    return OnAutoSize((WinSizeQuery)msg.mArgs.m1, *(Point2DFloat *)msg.mArgs.m4p );

		//case Message::kTypeCaptureChange:
		//    return OnCaptureChange(msg.mFocusEvent.mContext, msg.mFocusEvent.mpTarget);

		//case Message::kTypeCtrlActivated:
		//    return OnCtrlActivated(msg.mSrc, msg.mArgs.m1);

		//case Message::kTypeTriggerDown:
		//    return OnTriggerDown(msg.mTriggerEvent.mContext, msg.mTriggerEvent.mTriggerCode);

		//case Message::kTypeTriggerUp:
		//    return OnTriggerUp(msg.mTriggerEvent.mContext, msg.mTriggerEvent.mTriggerCode);

		//case Message::kTypeFocusAcquired:
		//    return OnFocusAcquired(msg.mFocusEvent.mContext, msg.mFocusEvent.mpTarget);

		//case Message::kTypeFocusLost:
		//    return OnFocusLost(msg.mFocusEvent.mContext, msg.mFocusEvent.mpTarget);

		//case Message::kTypeAttach:
		//    OnAttach();
		//    return false;

		//case Message::kTypeDetach:
		//    OnDetach();
		//    return false;

		//case Message::kTypeHitTest:         
		//    if (!OnHitTest(msg.mHitTestEvent.mX, msg.mHitTestEvent.mY, *msg.mHitTestEvent.mpResult))
		//        return Window::HandleMessage(msg);
		//    return false;

		//case Message::kTypeUpdateTransform:
		//    OnUpdateTransform(*(Transform*)msg.mArgs.m4p);
		//    return false;

		// Scrollbar value changed (we really don't care which one.)
		//case IWinScrollbar::kScrollbarMessageValueChanged:
		//{
		//    ScrollbarState& hs = mScrollbars[kScrollbarHorizontal];
		//    ScrollbarState& vs = mScrollbars[kScrollbarVertical];
		//
		//    Point2DFloat newScrollPos;
		//
		//    newScrollPos.mX = hs.mScrollbar != NULL ? (float)hs.mScrollbar->GetValue() : 0.0f;
		//    newScrollPos.mY = vs.mScrollbar != NULL ? (float)vs.mScrollbar->GetValue() : 0.0f;
		//
		//    if(newScrollPos != mScrollPosition)
		//    {
		//        mScrollPosition = newScrollPos;
		//        Invalidate();
		//    }
		//
		//    return true;
		//}

		//default:
		//    return Window::HandleMessage(msg);
	}

	return false;
}


/////////////////////////////////////////////////////////////////////////////
// CreateScrollbar
//
void TextEdit::CreateScrollbar(tScrollbar nWhichScrollbar)
{
	ScrollbarState& scrollState = mScrollbars[nWhichScrollbar];

	// First, make sure that the scrollbar actually exists (or not)
	if(scrollState.mEnabled == kScrollbarNever)
	{
		//if(scrollState.mScrollbar != NULL)
		//{
		//    ChildDelete(scrollState.mScrollbar->AsIWindow());
		//    scrollState.mScrollbar = NULL;
		//}
	}
	else
	{
		//if(scrollState.mScrollbar == NULL)
		//{
		//    scrollState.mScrollbar = UTFWIN_NEW("UTFWin/TextEdit/Scrollbar", WinScrollbar);
		//    scrollState.mScrollbar->AsIWindow()->SetFlag(kWinFlagPrivate, true);
		//
		//    if(nWhichScrollbar == kScrollbarVertical)
		//        scrollState.mScrollbar->SetOrientation(IWinScrollbar::kScrollbarOrientationVertical);
		//    else
		//        scrollState.mScrollbar->SetOrientation(IWinScrollbar::kScrollbarOrientationHorizontal);
		//
		//    ChildAdd(scrollState.mScrollbar->AsIWindow());
		//}

		//if(scrollState.mScrollbar != NULL)
		//{
		//    if(scrollState.mDrawable)
		//        scrollState.mScrollbar->SetDrawable(scrollState.mDrawable);
		//    else
		//        scrollState.mScrollbar->SetDrawable(UTFWIN_NEW("UTFWin/TextEdit/ScrollbarDrawable", ScrollbarDrawable));
		//}

		// Calculate the "client area"
		RectFloat rClientArea(0, 0, mPhysicalArea.GetWidth(), mPhysicalArea.GetHeight());

		rClientArea.mLeft   += mBorders.mLeft;
		rClientArea.mTop    += mBorders.mTop;
		rClientArea.mRight  -= mBorders.mRight;
		rClientArea.mBottom -= mBorders.mBottom;

		scrollState.mPreferredSize.mX = scrollState.mPreferredSize.mY = 20.0f;

		//Message sizeMsg;
		//sizeMsg.mType     = Message::kTypeCalcSizeQuery;
		//sizeMsg.mArgs.m1  = kSizeQueryPreferred;
		//sizeMsg.mArgs.m4p = &scrollState.mPreferredSize;

		// Find out how big it wants to be...
		//if(GetManager())
		//    GetManager()->SendMsg(scrollState.mScrollbar->AsIWindow(), scrollState.mScrollbar->AsIWindow(), sizeMsg, false);
	}
}


/////////////////////////////////////////////////////////////////////////////
// CreateScrollbars
//
void TextEdit::CreateScrollbars()
{
	CreateScrollbar(kScrollbarVertical);
	CreateScrollbar(kScrollbarHorizontal);
}


/////////////////////////////////////////////////////////////////////////////
// SetScrollbarState
//
// The calculation of which scrollbars should be visible is somewhat ambiguous, as the
// presence of a scrollbar can affect the amount of space available for the text, which
// in turn affects whether the scrollbar should be shown or not.
//
// The intent here is that the scrollbar visibility should be stable, in the sense that
// in the case of an ambiguity, the preference should be to change as little as possible.
//
// As an example, imagine a text widget in which the edited text exactly fills the window,
// in other words, there are as many visible lines of text as there are lines total.
// Now imagine, an additional word is appended, so that the wrapped text is now taller
// than the visible area. This will trigger the appearance of the vertical scrollbar.
// The vertical scrollbar will, in turn, cause a narrowing of the visible area, which will
// trigger a repeat of the word-wrap calculation, causing the wrapped text to become taller
// still.
//
// Now, if that additional text is immediately deleted, causing the text to be restored to
// its previous state, the vertical scrollbar will still cause the wrapped text to be
// taller than it was before, which in turn will cause the vertical scrollbar to continue
// to be show. Only if the text is shortened sufficiently enough to be able to fit into
// the now-narrowed area will the scrollbar disappear.
//
void TextEdit::UpdateScrollbars()
{
	//ScrollbarState& hs = mScrollbars[kScrollbarHorizontal];
	//ScrollbarState& vs = mScrollbars[kScrollbarVertical];

	RectFloat rSavedVisibleArea(mVisibleArea);

	// Show / hide the vertical scrollbar
	// bool bVScrollVisible = (vs.mEnabled == kScrollbarAlways) || (vs.mEnabled == kScrollbarAsNeeded && mTextExtent.mY > mVisibleArea.GetHeight());
	//
	//if(vs.mScrollbar != NULL)
	//    vs.mScrollbar->AsIWindow()->SetFlag(kWinFlagVisible, bVScrollVisible);

	/// Calculate the visible area based on the presence or absence of the scrollbar
	CalculateVisibleArea();

	// If the visible area changed
	if(mVisibleArea != rSavedVisibleArea)
	{
		// Have to do this immediately, since we need to know the new text extent
		RecalculateLineBreaks();
	}

	// Show / hide the horizontal scrollbar (based on the updated visible area.)
	// bool bHScrollVisible = (hs.mEnabled == kScrollbarAlways) || (hs.mEnabled == kScrollbarAsNeeded && mTextExtent.mX > mVisibleArea.GetWidth());
	//
	//if(hs.mScrollbar != NULL)
	//    hs.mScrollbar->AsIWindow()->SetFlag(kWinFlagVisible, bHScrollVisible);

	// Do the calculation *again* (its cheap) to account for the horizontal scrollbar.
	CalculateVisibleArea();

	// Calculate the placement area of the scrollbars.
	// If *both* scrollbars are visible, then the scrollbar areas need to be adjusted so that they don't overlap.
	//if(vs.mScrollbar != NULL)
	//{
	//    RectFloat rArea(mPhysicalArea.GetWidth() - vs.mPreferredSize.mX, 0,
	//                    mPhysicalArea.GetWidth(), mPhysicalArea.GetHeight());
	//    if(bHScrollVisible)
	//        rArea.mBottom -= hs.mPreferredSize.mY;
	//
	//    vs.mScrollbar->AsIWindow()->SetArea(rArea);
	//    vs.mScrollbar->SetMaximumValue((int32_t)mTextExtent.mY);
	//    vs.mScrollbar->SetVisibleRange((int32_t)mVisibleArea.GetHeight());
	//    vs.mScrollbar->SetIncrementDelta(mLineHeight);
	//
	//    if(!vs.mScrollbar->AsIWindow()->HasCapture(IWindowMgr::kICMouse))
	//        vs.mScrollbar->SetValue((int32_t)mScrollPosition.mY);
	//}

	//if(hs.mScrollbar != NULL)
	//{    
	//    RectFloat rArea(0, mPhysicalArea.GetHeight() - hs.mPreferredSize.mY, 
	//                    mPhysicalArea.GetWidth(), mPhysicalArea.GetHeight());
	//    if(bVScrollVisible)
	//        rArea.mRight -= vs.mPreferredSize.mX;
	//
	//    hs.mScrollbar->AsIWindow()->SetArea(rArea);
	//    hs.mScrollbar->SetMaximumValue((int32_t)mTextExtent.mX);
	//    hs.mScrollbar->SetVisibleRange((int32_t)mVisibleArea.GetWidth());
	//    hs.mScrollbar->SetIncrementDelta(16);
	//
	//    if(!hs.mScrollbar->AsIWindow()->HasCapture(IWindowMgr::kICMouse))
	//        hs.mScrollbar->SetValue((int32_t)mScrollPosition.mX);
	//}
}


/////////////////////////////////////////////////////////////////////////////
// LayoutParagraph
//
void TextEdit::LayoutParagraph(Paragraph&para)
{
	using namespace EA::Text;

	mLineBreaks.clear();
	para.mExtent.mX = 0;
	para.mExtent.mY = 0;

	Typesetter& typesetter = *GetTypesetter();
	typesetter.Reset();
	typesetter.SetDefaultTextStyle(&mActualStyle);
	typesetter.AddTextRun(&mText[para.mTextIndex], para.mTextLength);
	typesetter.SetLayoutSpace(mVisibleArea.GetWidth(), mVisibleArea.mLeft, 0);
	int32_t nLineStartIndex = 0;

	while(typesetter.GetLineState() != Typesetter::kLineStateComplete)
	{
		typesetter.FinalizeLine();

		LineLayout& llayout = typesetter.GetLineLayout();

		// Compute the selection range, relative to this line.
		// Note that these need to be signed, since they are relative to the start
		// of the para, and can therefore be negative.
		int32_t nLineLength = (int32_t)llayout.mCharArray.size();

		// Add to the height of this paragraph
		para.mExtent.mY += mLineHeight;

		// Update the widest line value.
		para.mExtent.mX = eastl::max_alt(para.mExtent.mX, llayout.mLineMetrics.mfVisibleSpace);

		// Push the linebreak onto the linebreak list
		mLineBreaks.push_back(nLineStartIndex);

		// Update the line start index to the start of the next line.
		nLineStartIndex += nLineLength;
		typesetter.NextLine();
	}

	mParaBreak = nLineStartIndex;
	para.mTextLines = (uint32_t)mLineBreaks.size();
}


/////////////////////////////////////////////////////////////////////////////
// GetLineRange
//
void TextEdit::GetLineRange(size_t nLine, tCharIndex& nStartIndex, tCharIndex& nLength)
{
	if(nLine >= mLineBreaks.size())
	{
		// Special case for an empty paragraph
		nStartIndex = mParaBreak;
		nLength = 0;
	}
	else
	{
		nStartIndex = (tCharIndex)mLineBreaks[(eastl_size_t)nLine];

		if(nLine >= mLineBreaks.size() - 1) 
			nLength = mParaBreak - nStartIndex;
		else 
			nLength = (tCharIndex)mLineBreaks[(eastl_size_t)nLine + 1] - nStartIndex;
	}
}


/////////////////////////////////////////////////////////////////////////////
// FillLineLayout
//
EA::Text::LineLayout & TextEdit::FillLineLayout(const Paragraph& para, int32_t nLineIndex, 
												tCharIndex& nLineStartIndex, tCharIndex& nLineLength)
{
	using namespace EA::Text;

	mLineBreaks.clear();
	nLineLength = 0;

	Typesetter& typesetter = *GetTypesetter();
	typesetter.Reset();
	typesetter.SetDefaultTextStyle(&mActualStyle);
	typesetter.AddTextRun(&mText[para.mTextIndex], para.mTextLength);
	typesetter.SetLayoutSpace(mVisibleArea.GetWidth(), mVisibleArea.mLeft, 0);

	nLineStartIndex = 0;
	LineLayout& llayout = typesetter.GetLineLayout();

	while(typesetter.GetLineState() != Typesetter::kLineStateComplete)
	{
		typesetter.FinalizeLine();
		nLineLength = (tCharIndex)llayout.mCharArray.size();

		if(nLineIndex <= 0)
			break;

		// Update the line start index to the start of the next line.
		nLineStartIndex += nLineLength;
		typesetter.NextLine();
		--nLineIndex;
	}

	return typesetter.GetLineLayout();
}


/////////////////////////////////////////////////////////////////////////////
// PickParagraph
//
TextEdit::tCharIndex TextEdit::PickParagraph(const Paragraph& para, const Point2DFloat& pickPos, bool bClamp)
{
	using namespace EA::Text;

	// Find the line that was picked.
	const uint32_t nPickLine = (uint32_t)floor(pickPos.mY / mLineHeight);

	tCharIndex  nLineStartIndex, nLineLength;
	LineLayout& ll = FillLineLayout(para, nPickLine, nLineStartIndex, nLineLength);

	if(pickPos.mX < 0.0f)
	{
		if(bClamp) 
			return nLineStartIndex;
		else 
			return kInvalidCharIndex;
	}

	if(nLineLength > 0)
	{
		GlyphSide gs;
		return static_cast<uint32_t>(nLineStartIndex + ll.GetTextPositionFromDisplayPosition(pickPos.mX, pickPos.mY, false, kPickTypeInsertion, gs, kDirectionDefault));
	}

	return nLineStartIndex;
}

} // namespace Text

} // namespace EA



#if defined(__SNC__)
	#pragma control %pop diag
#endif














