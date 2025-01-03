///////////////////////////////////////////////////////////////////////////////
// EATextTextEdit.h
//
// Written by Talin and Paul Pedriana.
// Copyright (c) 2005, Electronic Arts Inc. All rights reserved.
///////////////////////////////////////////////////////////////////////////////

///////////////////////////////////////////////////////////////////////////////
// This module implements a generic text display view and text editor via EAText. 
// The editor is similar to the Windows notepad.exe editor in functionality but
// has some additional features.
///////////////////////////////////////////////////////////////////////////////


#ifndef EATEXT_TEXTEDIT_H
#define EATEXT_TEXTEDIT_H


#include <EAPlatform/internal/Config.h>
#include <EAPlatform/EAClipboard.h>
#include <EAText/EATextStyle.h>
#include <EASTL/vector.h>
#include <EASTL/deque.h>
#include <EASTL/string.h>
#include <UTFDraw2D/EAPoint.h>
#include <UTFDraw2D/EARect.h>
#include <EAStdC/EAStopwatch.h>

#ifdef _MSC_VER
	#pragma warning(push)           // We have to be careful about disabling this warning. Sometimes the warning is meaningful; sometimes it isn't.
	#pragma warning(disable: 4251)  // class (some template) needs to have dll-interface to be used by clients.
#endif

namespace EA
{
	namespace Text
	{
		// Forward declarations.
		class LineLayout;
		class Typesetter;
		class IWindow;          // Currently this is an arbitrary type.
		class TextEditMessage;


		/// ITextEditMessageHandler
		///
		class EATEXT_API ITextEditMessageHandler
		{
		public:
			virtual ~ITextEditMessageHandler() {}
			virtual bool HandleTextEditMessage(const TextEditMessage& msg) = 0;
		};


		/// TextEditMessage
		///
		/// These are messages sent to the TextEdit via its HandleMessage.
		/// 
		class TextEditMessage
		{
		public:
			enum Type {
				kTypeNone,              // Arg1 | Arg2 | Arg3 | Arg4 |
				kTypeKeyDown,           // ctx  | -    | key  | mods | sent second if PreKeyDown is not handled; child has priority over parents
				kTypeKeyUp,             // ctx  | -    | key  | mods | sent second if PreKeyUp is not handled; child has priority over parents
				kTypeChar,              // ctx  | char | -    | -    |
				kTypeMouseDown,         // x    | y    | key  | mods |
				kTypeMouseUp,           // x    | y    | key  | mods |
				kTypeMouseMove,         // x    | y    | -    | mods |
				kTypeMouseWheel,        // x    | y    | dz   | mods |
				kTypeFocusChange,       // ctx  | -    | -    |target| sent to both old and new window
				kTypeCaptureChange,     // ctx  | -    | -    |target| sent to both old and new window
				kTypeTick,              // -    | -    | -    | -    |
				kTypeRebuild,           // -    | -    | -    | RC   |
				kTypeAreaChanged        // -    | -    | -    | rect*| sent to support layout and effects
			};

			IWindow*  mSrc; // IWindow is an arbitrary type, but represents an input viewport.
			IWindow*  mDst;
			uint32_t  mType;

			union {
				struct {
					union {
						int32_t m1;
						float_t m1f;
					};

					union {
						int32_t m2;
						float_t m2f;
					};

					union {
						int32_t m3;
						float_t m3f;
					};

					union {
						int32_t m4;
						void*   m4p;
					};
				} mArgs;

				struct {
					int32_t     mContext;       ///< Input context for key/button event (kICKeyboard or kICController_).
					uint32_t    mKey;           ///< The button pressed. This is an InputMan button ID.
					uint32_t    mModifiers;     ///< Mask indicating depressed modifier keys.
				} mKeyEvent;

				struct {
					float_t     mX;             ///< Viewport X coordinate.
					float_t     mY;             ///< Viewport Y coordinate.
					uint32_t    mModifiers;     ///< Mask indicating depressed modifier keys/buttons.
					union {
						int32_t mButton;        ///< Button ID of press.
						int32_t mZDelta;        ///< Mouse wheel delta.
					};
				} mMouseEvent;

				struct {
					int32_t     mContext;       ///< Input context of focus change.
					int32_t     m2;
					int32_t     m3;
					IWindow*    mpTarget;      ///< Window receiving focus.
				} mFocusEvent;

				struct {
					int32_t      m1;
					int32_t      m2;
					int32_t      m3;
					void*        mpDC;        ///< EA::UTFDraw2D::IDrawContext context to use for rebuilding visual.
				} mRebuildEvent;
			};

			TextEditMessage() {}
			TextEditMessage(uint32_t type) : mType(type) {}
		};


		/// TextEdit
		///
		/// Implements a fairly robust text editor suitable for both static text 
		/// display and text editing on any platform and any significant language. 
		///
		class EATEXT_API TextEdit
		{
		public:
			typedef eastl::basic_string<Char> String;

			/// Type representing a text index
			typedef uint32_t tCharIndex;

			/// Invalid text index, used as a return code to indicate no valid position.
			static const tCharIndex kInvalidCharIndex = (tCharIndex)-1;

			/// Used to indicate an infinitely high text index.
			static const tCharIndex kCharIndexInfinite = (tCharIndex)-1;

			/// Used to indicate the end text index.
			static const tCharIndex kCharIndexEnd = (tCharIndex)-1;

			//////////////////////////////////////////////////////////////////////////
			/// TextEdit flags
			///
			enum tFlags
			{
				kEditFlagsNone                 = 0x0000,
				kEditFlagReadOnly              = 0x0001,   /// If true, the text cannot be changed.
				kEditFlagAcceptsReturn         = 0x0002,   /// If true allows return to insert a new line.
														   /// If false, it causes the return key event to be forwarded to the parent.
				kEditFlagAcceptsInsertKey      = 0x0004,   /// If true, allow using the insert key to toggle insert vs. overwrite mode.
				kEditFlagOverwrite             = 0x0008,   /// If true, causes newly entered characters to overwrite existing characters.
														   /// If false, uses the normal insert behavior.
				kEditFlagHideInactiveSelection = 0x0010,   /// If true, the select will be hidden if we don't have focus.
				kEditFlagHideCaret             = 0x0020,   /// If true, the flashing caret is hidden.
				kEditFlagEnableClipboard       = 0x0040,   /// If true, allows copy and paste to and from clipboard.
				kEditFlagPasswordMode          = 0x0080,   /// If true, the displayed text is obscured, and won't copy to clipboard.
				kEditFlagsDefault              = kEditFlagHideInactiveSelection | kEditFlagEnableClipboard | kEditFlagAcceptsInsertKey
			};

			//////////////////////////////////////////////////////////////////////////
			/// Cursor movement types
			///
			enum tLineWrapMode
			{
				kLineWrapModeSingleLine = 0,                /// Single line only
				kLineWrapModeMultiLine,                     /// Multiline with hard linebreaks only
				kLineWrapModeMultiLineWordWrap,             /// Multiline with soft wordwrap
				kLineWrapModeDefault = kLineWrapModeSingleLine 
			};

			//////////////////////////////////////////////////////////////////////////
			/// Cursor movement types
			///
			enum tMoveType
			{
				kMoveByCharacter = 0,       /// Move by specified number of characters
				kMoveByWord,                /// Move by specified number of words
				kMoveByLine,                /// Move by specified number of lines
				kMoveBySentance,            /// Move by specified number of sentences
				kMoveByParagraph,           /// Move by specified number of paragraphs
				kMoveByPage,                /// Move by specified number of pages
				kMoveToLineBeginning,       /// Move to beginning of current line
				kMoveToLineEnd,             /// Move to end of current line
				kMoveToBeginning,           /// Move to beginning of buffer
				kMoveToEnd                  /// Move to end of buffer
			};

			//////////////////////////////////////////////////////////////////////////
			/// Selects one of the two scrollbars.
			///
			enum tScrollbar
			{
				kScrollbarHorizontal = 0,
				kScrollbarVertical,
				kScrollbarCount
			};

			//////////////////////////////////////////////////////////////////////////
			/// Used to enable / disable scrollbars
			///
			enum tScrollbarEnabled
			{
				kScrollbarNever = 0,                    /// Scrollbar is never shown
				kScrollbarAsNeeded,                     /// Scrollbar is shown if needed
				kScrollbarAlways                        /// Scrollbar is always shown
			};

			//////////////////////////////////////////////////////////////////////////
			/// Error codes for error message
			///
			enum tError
			{
				kEditErrorInvalidKey = 1,               /// Invalid keystroke
				kEditErroreValidateFailed,              /// Text validation failed
				kEditErrorCapacityReached,              /// Send if the capacity was reached
				kEditErrorCursorLimit,                  /// Attempt to move the cursor outside of the valid range
				kEditErrorUndoLimitReached,             /// Nothing to undo / redo
				kEditErrorMisc                          /// Misc, such as copy to clipboard failed
			};

			//////////////////////////////////////////////////////////////////////////
			/// Messages 
			///
			/// These are messages sent by this class to its registered handler.
			///
			enum tMessages
			{             
				kEditMessageRedraw         = 0x03b654aa,   /// Sent when any kind of redraw is needed.
				kEditMessageTextChanged    = 0x03b654af,   /// Sent after the text has been changed. 
				kEditMessageCaptureRequest = 0x03b6751e,   /// Sent to acquire capture, release capture, query capture. msg.mArgs.m4p => CaptureControl*.
				kEditMessageValidate       = 0x03b654b1,   /// Sent before the text is changed. msg.mArgs.m4p => TextValidation*.
				kEditMessageError          = 0x03b654b4    /// Sent if there was an error. msg.mArgs.m2 => errorId.
			};

			//////////////////////////////////////////////////////////////////////////
			/// Color indexes for use with SetColor.
			/// Colors are in ARGB format.
			///
			enum tColorIndex
			{
				kColorText,                             /// Color of the text
				kColorBackground,                       /// Color of background (if there's no drawable)
				kColorTextDisabled,                     /// Color of text when disabled
				kColorBackgroundDisabled,               /// Color of background when disabled
				kColorTextSelected,                     /// Color of text when selected
				kColorBackgroundSelected,               /// Color of background when selected
				kColorTextCaret,                        /// Color of text under the caret (overwrite mode)
				kColorBackgroundCaret,                  /// Color of the caret
				kColorCount 
			};

			//////////////////////////////////////////////////////////////////////////
			/// Struct which is passed during validation (as message param 4).
			/// The owner of this editor can use this to callback to accept or reject
			/// changes to the text.
			///
			struct TextValidation
			{
				const Char* mText;              /// The contents of the text buffer after the change.
				tCharIndex  mTextLength;        /// The length of the buffer.
				bool        mValidated;         /// Set this to false to reject the change.
			};

			//////////////////////////////////////////////////////////////////////////
			// This is used by TextEdit to acquire capture, release capture, 
			// or query for whether this widget has cursor capture. 
			enum CaptureRequest
			{
				kCaptureRequestAcquire,     // If set, we are requesting capture. Set mResult to true if we acquire it or already have it.
				kCaptureRequestRelease,     // If set, we are requesting release of capture. Set mResult to true if we release it or didn't have it.
				kCaptureRequestQuery        // Set mResult to true if we have capture, else false.
			};

			struct CaptureControl
			{
				CaptureRequest mRequest;   // The type of action the user is requesting.
				bool           mResult;    // The result of the request is stored here.
			};

		public:
			// TextEdit
			TextEdit();
			virtual ~TextEdit();

			// Setup
			virtual Text::Typesetter* GetTypesetter();
			virtual void              SetTypesetter(Text::Typesetter* pTypesetter);

			virtual void SetMessageHandler(ITextEditMessageHandler* pMessageHandler);

			// Appearance
			virtual uint32_t GetTextStyle();
			virtual void     SetTextStyle(uint32_t nStyleId);

			virtual uint32_t GetColor(tColorIndex nColorIndex) const;             
			virtual void     SetColor(tColorIndex nColorIndex, uint32_t nColor); // Color is in ARGB format.

			virtual const RectFloat& GetArea() const;
			virtual void             SetArea(const EA::RectFloat& area);

			virtual const RectFloat& GetBorders() const;
			virtual void             SetBorders(const EA::RectFloat& rBorders);

			virtual float GetCaretPeriod() const;
			virtual void  SetCaretPeriod(float fSeconds);

			virtual float GetDoubleClickInterval() const;
			virtual void  SetDoubleClickInterval(float fSeconds);

			/// Behavior settings
			virtual tLineWrapMode GetLineWrapMode() const;
			virtual void          SetLineWrapMode(tLineWrapMode mode);

			virtual void    SetTextEditFlag(uint32_t flag, bool bSet); 
			virtual bool    GetTextEditFlag(uint32_t flag) const;
			virtual int32_t GetTextEditFlags() const;

			// Text buffer
			virtual uint32_t    GetTextLength() const;
			virtual uint32_t    GetText(Char* pBuffer, tCharIndex nStart, uint32_t nLength) const;
			virtual uint32_t    GetText(String& sResult, tCharIndex nStart, uint32_t nLength) const;
			virtual const Char* GetText();
			virtual void        SetText(const Char* pText, bool bNotify = true);

			virtual uint32_t GetMaximumTextLength() const;
			virtual void     SetMaximumTextLength(uint32_t nMaxLength);

			virtual bool Insert(const Char* pInsertText, tCharIndex nInsertLength, bool bNotify = false);
			virtual bool Delete(tCharIndex nDeleteLength, bool bNotify);

			virtual bool ReplaceSelection(const Char* pReplaceText, tCharIndex nReplaceLength, bool bNotify = false);
			virtual bool Replace(tCharIndex nOldTextIndex, tCharIndex nOldTextLength, const Char* pNewText, 
									tCharIndex nNewTextLength, bool bNotify = false);
			virtual bool Clear(bool bNotify = false);

			virtual void SuspendUpdates();
			virtual void ResumeUpdates();

			// Cursor movement
			virtual tCharIndex GetCursorIndex() const;
			virtual void       SetCursorIndex(tCharIndex nCursorIndex, bool bExtendSelection);
			virtual tCharIndex GetAnchorIndex() const;
			virtual void       SetCursorAndAnchor(tCharIndex nCursorIndex, tCharIndex nAnchorIndex);
			virtual bool       GetSelectionRange(tCharIndex& selectionStart, tCharIndex& selectionEnd) const;
			virtual bool       MoveCursor(tMoveType nMoveType, int32_t nCount, bool bExtendSelection);

			// Paragraph calculations
			virtual uint32_t   GetParagraphCount() const;
			virtual tCharIndex GetParagraphTextIndex(uint32_t nParaIndex) const;
			virtual float      GetParagraphYPos(uint32_t nParaIndex) const;
			virtual uint32_t   GetParagraphLength(uint32_t nParaIndex) const;
			virtual uint32_t   GetParagraphAtTextIndex(tCharIndex nTextIndex) const ;
			virtual uint32_t   GetParagraphAtPosition(float fYPos) const;

			// Undo/redo
			virtual uint32_t GetUndoHistorySize() const;
			virtual void     SetUndoHistorySize(uint32_t nCount);
			virtual bool     CanUndo() const;
			virtual bool     CanRedo() const;
			virtual bool     Undo();
			virtual bool     Redo();

			// Scrolling
			virtual const Point2DFloat& GetScrollPosition() const;
			virtual void                SetScrollPosition(const Point2DFloat& pos);
			virtual void                ScrollToCursor();
			virtual Point2DFloat        GetTextExtent() const;

			// Scrollbars
			//virtual IScrollbarDrawable* GetScrollbarDrawable(tScrollbar nWhichScrollbar) const;
			//virtual void                SetScrollbarDrawable(tScrollbar nWhichScrollbar, IScrollbarDrawable* pScrollbarDrawable);
			virtual tScrollbarEnabled   GetScrollbarEnabled(tScrollbar nWhichScrollbar) const;
			virtual void                SetScrollbarEnabled(tScrollbar nWhichScrollbar, tScrollbarEnabled nEnabled);
			virtual bool                IsScrollbarVisible(tScrollbar nWhichScrollbar) const;

			// Window
			virtual bool  Init();
			virtual bool  Shutdown();
			virtual void  OnAreaChanged(const EA::RectFloat& newArea);
			virtual bool  HandleMessage(const TextEditMessage& msg);
			virtual void  SendMessage(TextEditMessage& msg);
			virtual void  Invalidate();
			virtual bool  Draw(void* pDrawContext);

		public:
			virtual bool OnMouseDown  (float_t x, float_t y, uint32_t button, uint32_t modifiers);
			virtual bool OnMouseUp    (float_t x, float_t y, uint32_t button, uint32_t modifiers);
			virtual bool OnMouseMove  (float_t x, float_t y, uint32_t modifiers);
			virtual bool OnMouseWheel (float_t x, float_t y, uint32_t modifiers, int32_t zDelta);
			virtual bool OnFocusChange(int context, void* pNewFocus);
			virtual bool OnKeyDown    (int context, uint32_t key, uint32_t modifiers);
			virtual bool OnKeyUp      (int context, uint32_t key, uint32_t modifiers);
			virtual bool OnChar       (int context, Char character);
			virtual bool OnRebuild    (void* pDrawContext);
			virtual bool OnTick       ();

		protected:
			//////////////////////////////////////////////////////////////////////////
			/// Internal constants
			//////////////////////////////////////////////////////////////////////////

			enum tDirtyFlags
			{
				kDirtyFlagCursorPosition  = 0x01,       /// The cursor index changed
				kDirtyFlagSetCursorSeek   = 0x02,       /// Indicates we want to set the cursor seek pos
				kDirtyFlagLineBreaks      = 0x04        /// The text changed (and poss. line count)
			};

		protected:
			//////////////////////////////////////////////////////////////////////////
			// Types
			//////////////////////////////////////////////////////////////////////////

			typedef TextEditMessage             Message;
			typedef EA::ClipboardSys::Clipboard Clipboard;
			typedef eastl::vector<tCharIndex>   CharIndexArray;

			// Paragraphs
			struct Paragraph
			{
				Paragraph(tCharIndex nTextIndex, tCharIndex nTextLength)
					: mTextIndex(nTextIndex), mTextLength(nTextLength) { }

				tCharIndex    mTextIndex;             /// Starting index of paragraph
				tCharIndex    mTextLength;            /// Length of paragraph
				int32_t       mTextLines;             /// Number of lines in paragraph
				Point2DFloat  mExtent;                /// Width and height in pixels
				float         mYPos;                  /// Y position of this para

				// Compare paragraph by Y position
				struct YPosComparator {
					bool operator()(float yPos, const Paragraph& para) {
						return yPos < para.mYPos;
					}
					bool operator()(const Paragraph& para, float yPos) {
						return para.mYPos < yPos;
					}
				};

				// Compare paragraph by Y position
				struct TextIndexComparator {
					bool operator()(tCharIndex nTextIndex, const Paragraph& para) {
						return nTextIndex < para.mTextIndex;
					}
					bool operator()(const Paragraph& para, tCharIndex nTextIndex) {
						return  para.mTextIndex < nTextIndex;
					}
				};
			};

			/// STL vector of paragraphs.
			typedef eastl::vector<Paragraph> tParagraphList;

			// Scrollbars
			struct ScrollbarState
			{
				//EA::Text::AutoRefCount<IWinScrollbar>      mScrollbar;
				//EA::Text::AutoRefCount<IScrollbarDrawable> mDrawable;
				tScrollbarEnabled                    mEnabled;
				Point2DFloat                         mPreferredSize;
			};

			// Undo list
			struct UndoAction
			{
				tCharIndex   mIndex;          /// Index of this change.
				String       mOldText;        /// Old text, might be empty
				String       mNewText;        /// New text, might be empty
				tCharIndex   mOldCursorIndex; /// Old cursor index
				tCharIndex   mNewCursorIndex; /// New cursor index
				tCharIndex   mOldAnchorIndex; /// Old cursor index
				tCharIndex   mNewAnchorIndex; /// New cursor index
			};

			typedef eastl::deque<UndoAction*> tUndoList;
			typedef eastl::vector<tCharIndex> tTextIndexArray;

		protected:
			//////////////////////////////////////////////////////////////////////////
			// Internal functions
			//////////////////////////////////////////////////////////////////////////

			// Recalculation methods
			void CalculateVisibleArea();
			void RecalculateCursorPosition();
			void RecalculateLineBreaks();
			void CreateScrollbars();
			void CreateScrollbar(tScrollbar nWhichScrollbar);
			void UpdateScrollbars();

			// Text picking methods
			tCharIndex Pick(float x, float y, bool bClamp);
			tParagraphList::const_iterator FindParagraphByPosition(float yPos) const;
			tParagraphList::const_iterator FindParagraphByTextIndex(tCharIndex nIndex) const;

			// Undo methods
			void TrimUndoList(uint32_t nMaxSize);
			void TrimRedoList(uint32_t nMaxSize);
			void ApplyUndo(UndoAction* pAction);
			void ApplyRedo(UndoAction* pAction);

			// Text search
			tCharIndex FindNextWord(tCharIndex nPos) const;
			tCharIndex FindPrevWord(tCharIndex nPos) const;
			tCharIndex FindNextSentence(tCharIndex nPos) const;
			tCharIndex FindPrevSentence(tCharIndex nPos) const;
			tCharIndex FindNextParagraph(tCharIndex nPos) const;
			tCharIndex FindPrevParagraph(tCharIndex nPos) const;
			tCharIndex FindNextWordEnd(tCharIndex nPos) const;
			tCharIndex FindPrevWordEnd(tCharIndex nPos) const;
			void       SelectWordAt(tCharIndex nPos);

			// Layout methods
			void                  LayoutParagraph(Paragraph& para);
			void                  GetLineRange(size_t nLine, tCharIndex& nStartIndex, tCharIndex& nLength);
			EA::Text::LineLayout& FillLineLayout(const Paragraph& para, int32_t nLineIndex, tCharIndex& nLineStartIndex, tCharIndex& nLineLength);
			tCharIndex            PickParagraph(const Paragraph& para, const Point2DFloat& pickPos, bool bClamp);

			/// Given an index into a line, find the corresponding X position.
			/// LayoutLine must have been called first.
			float PositionFromIndex(tCharIndex nIndex);

			// Misc internal methods
			virtual bool ValidateChange();
			virtual void ShowCaret(bool bToggleState);
			virtual void NotifyTextChanged();
			virtual void NotifyError(tError nError);
			virtual bool NotifyCapture(CaptureRequest captureRequest);

		protected:
			//////////////////////////////////////////////////////////////////////////
			// Member variables
			//////////////////////////////////////////////////////////////////////////
			bool                mInitialized;           /// Are we initialized?
			uint32_t            mId;                    /// Our unique id. Somewhat like a widet id.
			uint32_t            mFlags;                 /// Generic view flags.
			uint32_t            mTextEditFlags;         /// Edit Flags.
			uint32_t            mTextStyle;             /// EA::Text::TextStyle id.
			uint32_t            mDirtyFlags;            /// Flags for lazy recalc.
			tLineWrapMode       mLineWrapMode;          /// Line wrap mode.
			RectFloat           mBorders;               /// Borders.
		  //RectFloat           mArea;                  /// The logical area in parent window coordinates. The logical area of the window is usually simply the window left, top, right, bottom coordinates in its parent. However, it might instead have something like a 'magic' mLeft value of -1 to indicate that it should be aligned to the left of its parent. The winproc defines the meaning of this. mPhysicalArea defines the actual final window extents.
			RectFloat           mPhysicalArea;          /// The area in parent coordinates, after any transformations on mArea. Usually mPhysicalArea == mArea, but if mArea contains 'magic values', then the winproc might transform them to something else in mPhysicalArea.
			RectFloat           mVisibleArea;           /// Subrectangle of mPhysicalArea; equal to mPhysicalArea minus borders and scrollbars. This is the area where the glyphs are layed out.
			int32_t             mSuspendUpdateLevel;    /// Suspend update nesting count.
			uint32_t            mColors[kColorCount];   /// Various colors.

			tCharIndex          mMaxTextLength;         /// Maximum text length.
			tCharIndex          mCursorIndex;           /// Current cursor index.
			tCharIndex          mAnchorIndex;           /// Current anchor index.
			uint32_t            mCursorColumn;          /// Column index of the cursor.
			uint32_t            mCursorParaLine;        /// Cursor line in paragraph.

			/// Preferred X coordinate of cursor when using up/down arrow keys
			float               mCursorSeekPositionX;

			// Layout variables (global)
			Text::Typesetter*         mpTypesetter;           /// The Typesetter we use to do layout.
			ITextEditMessageHandler*  mpMessageHandler;       /// 
			String                    mText;                  /// Caption text for the window. Not necessarily used by the window.
			tParagraphList            mParagraphList;         /// List of paragraphs.
			uint32_t                  mStyle;                 /// Text style id.
			Text::TextStyle           mActualStyle;           /// Text style.
			int32_t                   mLineHeight;            /// Line height.

			// Layout variables (paragraph)
			CharIndexArray      mLineBreaks;            /// List of line breaks in current para.
			tCharIndex          mParaBreak;             /// Para break for current para.

			// Undo variables
			uint32_t            mUndoHistorySize;       /// Number of undo levels to keep.
			tUndoList           mUndoList;              /// List of undo actions.
			tUndoList           mRedoList;              /// List of redo actions.

			// Scrolling variables
			ScrollbarState      mScrollbars[kScrollbarCount];   /// Holds the state of the scrollbars
			Point2DFloat        mScrollPosition;                /// Current scroll position
			Point2DFloat        mTextExtent;                    /// Total area of text
			EA::StdC::Stopwatch mAutoScrollTimer;               /// Auto-scroll timer

			// Caret variables
			float               mCaretPeriod;           /// Blink time for caret (seconds).
			EA::StdC::Stopwatch mCaretTimer;            /// Used to time flashing cursor
			RectFloat           mCaretRect;             /// Placement of caret
			bool                mCaretVisible;          /// If caret is shown...

			// Double-click vars
			EA::StdC::Stopwatch mDoubleClickTimer;      /// Double-click timer
			float               mDoubleClickInterval;   /// Double-click window duration
   
		}; // class TextEdit

	} // namespace Text

} // namespace EA

#ifdef _MSC_VER
	#pragma warning(pop)
#endif

#endif // Header include guard












