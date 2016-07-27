/*
Copyright (C) 2009, 2010, 2011 Electronic Arts, Inc.  All rights reserved.

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
// EAWebKitLocalizedStringsEnum.h
// By Paul Pedriana
// Maintained by EAWebKit Team
///////////////////////////////////////////////////////////////////////////////


#ifndef EAWEBKIT_EAWEBKITLOCALIZEDSTRINGS_ENUM_H
#define EAWEBKIT_EAWEBKITLOCALIZEDSTRINGS_ENUM_H

namespace EA 
{
namespace WebKit
{
enum LocalizedStringType
{
	kLSinputElementAltText,
	kLSresetButtonDefaultLabel,
	kLSsearchableIndexIntroduction,
	kLSsubmitButtonDefaultLabel,
	kLSfileButtonChooseFileLabel,
	kLSfileButtonNoFileSelectedLabel,
	kLScopyImageUnknownFileLabel,
	kLScontextMenuItemTagOpenLinkInNewWindow,
	kLScontextMenuItemTagDownloadLinkToDisk,
	kLScontextMenuItemTagCopyLinkToClipboard,
	kLScontextMenuItemTagOpenImageInNewWindow,
	kLScontextMenuItemTagDownloadImageToDisk,
	kLScontextMenuItemTagCopyImageToClipboard,
	kLScontextMenuItemTagOpenFrameInNewWindow,
	kLScontextMenuItemTagCopy,
	kLScontextMenuItemTagGoBack,
	kLScontextMenuItemTagGoForward,
	kLScontextMenuItemTagStop,
	kLScontextMenuItemTagReload,
	kLScontextMenuItemTagCut,
	kLScontextMenuItemTagPaste,
	kLScontextMenuItemTagDelete,
	kLScontextMenuItemTagSelectAll,
	kLScontextMenuItemTagInputMethods,
	kLScontextMenuItemTagUnicode,
	kLScontextMenuItemTagNoGuessesFound,
	kLScontextMenuItemTagIgnoreSpelling,
	kLScontextMenuItemTagLearnSpelling,
	kLScontextMenuItemTagSearchWeb,
	kLScontextMenuItemTagLookUpInDictionary,
	kLScontextMenuItemTagOpenLink,
	kLScontextMenuItemTagIgnoreGrammar,
	kLScontextMenuItemTagSpellingMenu,
	kLScontextMenuItemTagShowSpellingMenu,
	kLScontextMenuItemTagHideSpellingMenu,
	kLScontextMenuItemTagCheckSpelling,
	kLScontextMenuItemTagCheckSpellingWhileTyping,
	kLScontextMenuItemTagCheckGrammarWithSpelling,
	kLScontextMenuItemTagFontMenu,
	kLScontextMenuItemTagBold,
	kLScontextMenuItemTagItalic,
	kLScontextMenuItemTagUnderline,
	kLScontextMenuItemTagOutline,
	kLScontextMenuItemTagWritingDirectionMenu,
	kLScontextMenuItemTagTextDirectionMenu,
	kLScontextMenuItemTagDefaultDirection,
	kLScontextMenuItemTagLeftToRight,
	kLScontextMenuItemTagRightToLeft,
	kLScontextMenuItemTagOpenVideoInNewWindow,
	kLScontextMenuItemTagOpenAudioInNewWindow,
	kLScontextMenuItemTagCopyVideoLinkToClipboard,
	kLScontextMenuItemTagCopyAudioLinkToClipboard,
	kLScontextMenuItemTagToggleMediaControls,
	kLScontextMenuItemTagToggleMediaLoop,
	kLScontextMenuItemTagEnterVideoFullscreen,
	kLScontextMenuItemTagMediaPlay,
	kLScontextMenuItemTagMediaPause,
	kLScontextMenuItemTagMediaMute,


	kLScontextMenuItemTagInspectElement,

	kLSsearchMenuNoRecentSearchesText,
	kLSsearchMenuRecentSearchesText,
	kLSsearchMenuClearRecentSearchesText,

	kLSAXWebAreaText,
	kLSAXLinkText,
	kLSAXListMarkerText,
	kLSAXImageMapText,
	kLSAXHeadingText,

	kLSAXButtonActionVerb,
	kLSAXRadioButtonActionVerb,
	kLSAXTextFieldActionVerb,
	kLSAXCheckedCheckBoxActionVerb,
	kLSAXUncheckedCheckBoxActionVerb,
	kLSAXLinkActionVerb,

	kLSMissingPluginText,
	kLSunknownFileSizeText,
	kLSuploadFileText,
	kLSallFilesText,

	kLSMediaElementLoadingStateText,
	kLSMediaElementLiveBroadcastStateText,

	kLSAudioElement,
	kLSVideoElement,
	kLSMuteButton,
	kLSUnMuteButton,
	kLSUnPlayButton,
	kLSUnPauseButton,
	kLSSlider,
	kLSSliderThumb,
	kLSRewindButton,
	kLSReturnToRealTimeButton,
	kLSCurrentTimeDisplay,
	kLSTimeRemainingDisplay,
	kLSStatusDisplay,
	KLSFullScreenButton,
	KLSSeekForwardButton,
	kLSSeekBackButton,

	kLSAudioElementHelp,
	kLSVideoElementHelp,
	kLSMuteButtonHelp,
	kLSUnMuteButtonHelp,
	kLSUnPlayButtonHelp,
	kLSUnPauseButtonHelp,
	kLSSliderHelp,
	kLSSliderThumbHelp,
	kLSRewindButtonHelp,
	kLSReturnToRealTimeButtonHelp,
	kLSCurrentTimeDisplayHelp,
	kLSTimeRemainingDisplayHelp,
	kLSStatusDisplayHelp,
	KLSFullScreenButtonHelp,
	KLSSeekForwardButtonHelp,
	kLSSeekBackButtonHelp,

    kLSFileButtonChooseMultipleFilesLabel,
    kLSDefaultDetailsSummaryText,
    kLScontextMenuItemTagCopyImageUrlToClipboard,

	kLSIndefiniteTime,

};
}
} 

#endif //EAWEBKIT_EAWEBKITLOCALIZEDSTRINGS_ENUM_H
