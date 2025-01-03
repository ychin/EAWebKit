/*H********************************************************************************/
/*!
    \File voipheadsetxboxone.cpp

    \Description
        VoIP headset manager. Wraps Xbox One Game Chat 2 api.

    \Copyright
        Copyright (c) Electronic Arts 2018. ALL RIGHTS RESERVED.

    \Version 05/24/2013 (mclouatre) First Version
    \Version 11/19/2018 (mclouatre) Rebased from Windows::XBox:Chat to MS game chat 2.
*/
/********************************************************************************H*/

/*** Include files ****************************************************************/
#include "DirtySDK/platform.h"

// system includes
#include <windows.h>
#include <stdio.h>
#include <string>
#include <wrl/client.h>
#include <GameChat2.h>

// disable the warning caused by this include instead of global suppression
#pragma warning(push, 0)
#include <GameChat2Impl.h>
#pragma warning(pop)

#if defined(DIRTYCODE_GDK)
#include <XUser.h>
#endif

#include <xaudio2.h>

// dirtysock includes
#include "DirtySDK/dirtysock.h"
#include "DirtySDK/dirtysock/dirtymem.h"
#include "DirtySDK/dirtysock/dirtyerr.h"
#include "DirtySDK/dirtysock/netconn.h"
#include "DirtySDK/voip/voipdef.h"
#include "DirtySDK/voip/voipnarrate.h"
#include "DirtySDK/voip/voiptranscribe.h"
#include "DirtySDK/voip/voipcodec.h"

// voip includes
#include "voippriv.h"
#include "voipcommon.h"
#include "voippacket.h"
#include "voipmixer.h"
#include "voipconduit.h"
#include "voipheadset.h"

using namespace xbox::services::game_chat_2;
#ifndef DIRTYCODE_GDK
using namespace Windows::Xbox::System;
#endif

/*** Defines **********************************************************************/
#define VOIPHEADSET_DEBUG                   (DIRTYCODE_DEBUG && FALSE)
#define VOIPHEADSET_GAMECHAT2_POLL_RATE     (40)
#define VOIPHEADSET_CONSUMER_POLL_RATE      (20)
#define VOIPHEADSET_CONSUMER_SAMPLE_RATIO   (VOIPHEADSET_GAMECHAT2_POLL_RATE / VOIPHEADSET_CONSUMER_POLL_RATE)
#define VOIPHEADSET_XUID_SAFE_LEN           (64)
#define VOIPHEADSET_BYTESPERSAMPLES         (2)
#define VOIPHEADSET_SAMPLERATE              (16000)
#define VOIPHEADSET_FRAMESAMPLES            (VOIPHEADSET_CONSUMER_POLL_RATE * VOIPHEADSET_SAMPLERATE / 1000) //!< samples per frame (20ms; 16khz=320)
#define VOIPHEADSET_FRAMESIZE               (VOIPHEADSET_FRAMESAMPLES * VOIPHEADSET_BYTESPERSAMPLES)
#define VOIPHEADSET_LOOPBACK_BUFFER_SIZE    (8*1024) //!< 256ms samples for Xaudio @ 16kHz

/*** Macros ***********************************************************************/

/*** Type Definitions *************************************************************/

//! local pre-encoded stream
typedef struct
{
    uint32_t                uCurrentAudioBufferSize;
    uint32_t                uCurrentAudioBufferConsumed;
    uint8_t                 *pCurrentAudioBuffer;
    VoipTranscribeRefT      *pTranscribeRef;
    pre_encode_audio_stream *pAudioStream;
}XONELocalPreEncodedStreamT;

//! local mixer stream
typedef struct 
{
    VoipMixerRefT *pMixer;
    post_decode_audio_sink_stream   *pSinkStream;
} XONELocalMixerStreamT;

//! local user data space
typedef struct
{
    VoipUserT User;                 //!< local account and persona information
    uint64_t uXuid;                 //!< local first party id
    chat_user *pChatUser;           //!< chat user associated with this local user
    uint8_t bParticipating;         //!< TRUE if this user is a participating user FALSE if not
    uint8_t bMic;                   //!< TRUE if this user has a microphone, FALSE if not
    uint8_t uSendSeq;
    uint8_t bStopping;              //!< are we stopping the playing audio?
    VoipNarrateGenderE eGender;     //!< gender setting for text to speech, used outside the context of gamechat

    int32_t iAudioLen;              //!< amount of data written to the audio buffer
    uint8_t aAudioData[VOIPHEADSET_LOOPBACK_BUFFER_SIZE]; //!< buffer up to 4k samples (256ms at 16kbps)

    VoipNarrateRefT *pNarrateRef;   //!< narrate ref for text to speech, used outside the context of gamechat and crossplay
    uint8_t bNarrating;             //!< use to stop users from sending mic data during narration  
    uint8_t bNarrateCbExit;         //!< use to break out of the narrate cb (need so we can send 20ms packets every 20ms)
    uint8_t bMuteListUpdated;       //!< TRUE if the first party mute list has been updated
    uint8_t _pad;

    Microsoft::WRL::ComPtr<IXAudio2> pXaudio2;  //!< pointer to the xaudio2 interface
    IXAudio2MasteringVoice *pMasteringVoice;    //!< pointer to the master voice
    IXAudio2SourceVoice *pSourceVoice;          //!< pointer to the source voice

    uint32_t uPlaybackFlags;                    //!< crossplay mute flag every remote user is 1 bit
} XONELocalVoipUserT;

//! remote user data space
typedef struct
{
    VoipUserT User;         //!< remote user account and persona information
    uint64_t uXuid;         //!< remote user first party id
    uint64_t uConsoleId;    //!< console id;
    chat_user *pChatUser;   //!< chat user associated with this user null if this user is not from xone
    game_chat_communication_relationship_flags aRelFromChannels[VOIP_MAXLOCALUSERS];    //!< muting via first party relationships for channels
    game_chat_communication_relationship_flags aRelFromMuting[VOIP_MAXLOCALUSERS];      //!< muting via first party relationships for mute
} XONERemoteVoipUserT;

//! voipheadset module state data
struct VoipHeadsetRefT
{
    int32_t iMemGroup;                    //!< mem group
    void *pMemGroupUserData;              //!< mem group user data

    //! channel info
    int32_t iMaxChannels;

    //!< game chat 2 thread affinity
    int32_t iThreadAffinity;

    //! user callback data
    VoipHeadsetMicDataCbT *pMicDataCb;          //!< callback invoked when locally acquired voice data is ready to be transmitted over the network
    VoipHeadsetTextDataCbT *pTextDataCb;        //!< callback invoked when transcribed text from a local orginiator is ready to be transmitted over the network
    VoipHeadsetOpaqueDataCbT *pOpaqueDataCb;    //!< callback invoked when locally generated gamechat2 control packet is ready to be transmitted over the network
    VoipHeadsetStatusCbT *pStatusCb;
    void *pCbUserData;

    VoipHeadsetTranscribedTextReceivedCbT *pTranscribedTextReceivedCb; //!< callback invoked when transcribed text from a remote orginator needs to be surfaced up to voipcommon to get displayed locall
    void *pTranscribedTextReceivedCbUserData;

    //! speaker callback data
    VoipSpkrCallbackT *pSpkrDataCb;
    void *pSpkrCbUserData;

#if DIRTYCODE_LOGGING
    int32_t iDebugLevel;            //!< module debuglevel
    uint32_t uLastChatUsersDump;    //!< last time users where dumped 
    uint8_t bDumpChatUsers;         //!< used to trigger a dump from code not protected by pVoipCommon->ThreadCrit, resulting in dump being delayed to VoipHeadsetProcess()
    uint8_t _pad[3];
#endif

    XONELocalVoipUserT aLocalUsers[VOIP_MAXLOCALUSERS];

    wchar_t *pTTSInputText;         //!< TTS input text
    int32_t iTTSInputLen;
    VoipTextToSpeechMetricsT aTTSMetrics[VOIP_MAXLOCALUSERS];
    VoipSpeechToTextMetricsT aSTTMetrics[VOIP_MAXLOCALUSERS];

    // crossplay
    int32_t iCmpFrameSize;      //!< codec frame size in bytes
    VoipConduitRefT *pConduitRef;  //!< voip conduit used for crossplay flow
    XONELocalMixerStreamT aLocalMixerStreams[VOIP_MAXLOCALUSERS_EXTENDED];      //!< number of mixer equals the number of audio devices but might not be equal to the number of local users
    XONELocalPreEncodedStreamT aPreEncodedStreams[VOIP_MAXLOCALUSERS_EXTENDED]; //!< number of pre encoded buffers should match the numbers of mixers
    VoipFirstPartyIdCallbackCbT *pFirstPartyIdCallback;    //!< used to query first party ids for remote users
    void *pFirstPartyIdCbUserData;
    uint8_t uSendSeq;   //!< send sequence tracker
    uint8_t bLoopback;  //!< loopback mode
    uint8_t bCrossplay; //!< TRUE if cross-play is enabled, else FALSE
    uint8_t bVoiceTranscriptionEnabled; //!< TRUE if voice transcription is enabled (in crossplay)

    //! remote user list - must come last in ref as it is variable length
    int32_t iMaxRemoteUsers;
    XONERemoteVoipUserT aRemoteUsers[1];
};

/*** Function Prototypes **********************************************************/
static int32_t _VoipHeadsetCreateNarrate(VoipHeadsetRefT *pHeadset, int32_t iLocalUserIndex);

/*** Variables ********************************************************************/
static int32_t VoipHeadsetXboxone_iMemGroup;
static void *VoipHeadsetXboxone_pMemGroupUserData;


/*** Private Functions ************************************************************/

#if DIRTYCODE_LOGGING
/*F********************************************************************************/
/*!
    \Function _VoipHeadsetGetChatIndicatorString

    \Description
        Return debug string for specified chat indicator value.

    \Input ChatIndicator    - chat indicator value

    \Output
        const char *        - matching chat indicator string

    \Version 11/27/2018 (mclouatre)
*/
/********************************************************************************F*/
static const char * _VoipHeadsetGetChatIndicatorString(game_chat_user_chat_indicator ChatIndicator)
{
    switch (ChatIndicator)
    {
        case game_chat_user_chat_indicator::incoming_communications_muted:
            return("COMM_MUTED");

        case game_chat_user_chat_indicator::local_microphone_muted:
            return("MIC_MUTED ");

        case game_chat_user_chat_indicator::no_chat_focus:
            return("NO_FOCUS  ");

        case game_chat_user_chat_indicator::no_microphone:
            return("NO_MIC    ");

        case game_chat_user_chat_indicator::platform_restricted:
            return("PLAT_RESTR");

        case game_chat_user_chat_indicator::reputation_restricted:
            return("REP_RESTR ");

        case game_chat_user_chat_indicator::silent:
            return("SILENT    ");

        case game_chat_user_chat_indicator::talking:
            return("TALKING   ");

        default:
            break;
    }

    return("INVALID   ");
}

/*F********************************************************************************/
/*!
    \Function _VoipHeadsetGetCommRelAdjusterString

    \Description
        Return debug string for specified communication relationship adjuster

    \Input CommRelAdjuster  - communication relationship adjuster

    \Output
        const char *        - matching adjuster string

    \Version 11/27/2018 (mclouatre)
*/
/********************************************************************************F*/
static const char * _VoipHeadsetGetCommRelAdjusterString(game_chat_communication_relationship_adjuster CommRelAdjuster)
{
    switch (CommRelAdjuster)
    {
        case game_chat_communication_relationship_adjuster::conflict_with_other_user:
            return("CONFLICT ");

        case game_chat_communication_relationship_adjuster::initializing:
            return("INIT     ");

        case game_chat_communication_relationship_adjuster::none:
            return("NONE     ");

        case game_chat_communication_relationship_adjuster::privacy:
            return("PRIVACY  ");

        case game_chat_communication_relationship_adjuster::privilege:
            return("PRIVILEGE");

        case game_chat_communication_relationship_adjuster::reputation:
            return("REPUTATION");

        case game_chat_communication_relationship_adjuster::uninitialized:
            return("UNINIT    ");

        default:
            break;
    }

    return("INVALID   ");
}
#endif

/*F********************************************************************************/
/*!
    \Function _VoipHeadsetGetRemoteUserIndex

    \Description
       Returns the user index of the remote user

    \Input *pHeadset    - pHeadset ref
    \Input *pRemoteUser - remote user

    \Output
        int32_t         - remote user index, negative if user is not found

    \Version 05/28/2019 (tcho)
*/
/********************************************************************************F*/
static int32_t _VoipHeadsetGetRemoteUserIndex(VoipHeadsetRefT *pHeadset, VoipUserT *pRemoteUser)
{
    int32_t iRemoteUserIndex;

    for (iRemoteUserIndex = 0; iRemoteUserIndex < pHeadset->iMaxRemoteUsers; iRemoteUserIndex += 1)
    {
        if (VOIP_SameUser(&pHeadset->aRemoteUsers[iRemoteUserIndex].User, pRemoteUser))
        {
            break;
        }
    }

    return((iRemoteUserIndex != pHeadset->iMaxRemoteUsers) ? iRemoteUserIndex : -1);
}

/*F********************************************************************************/
/*!
    \Function _VoipHeadsetFirstPartyMute

    \Description
        Find out if the given local user is muted with the given remote user.
        First party mute will be combined with other forms of muting to be 
        most restrictive.
        Always return false if the remote user is non xbox.

    \Input *pHeadset         - headset ref
    \Input iLocalUserIndex   - index of local user
    \Input iRemoteUserIndex  - index of remote user

    \Output
        uint8_t             - TRUE if muted, FALSE if not muted

    \Version 6/13/2019 (tcho)
*/
/********************************************************************************F*/
static uint8_t _VoipHeadsetFirstPartyMute(VoipHeadsetRefT *pHeadset, int32_t iLocalUserIndex, int32_t iRemoteUserIndex)
{
    uint8_t bMuted = FALSE;
    chat_user *pRemoteChatUser;
    VoipUserT *pRemoteUser;

    // make sure indices are within bounds
    if ((iLocalUserIndex < 0) || (iRemoteUserIndex < 0) || (iLocalUserIndex >= VOIP_MAXLOCALUSERS) || (iRemoteUserIndex >= pHeadset->iMaxRemoteUsers))
    {
        return(bMuted);
    }

    // if we are in crossplay there is no need to apply first party mute separately because GameChat2 will do it for you
    if (pHeadset->bCrossplay == FALSE)
    {
        return(bMuted);
    }

    pRemoteChatUser = pHeadset->aRemoteUsers[iRemoteUserIndex].pChatUser;
    pRemoteUser = &pHeadset->aRemoteUsers[iRemoteUserIndex].User;

    // is it a valid xbox user?
    if ((pHeadset->aLocalUsers[iLocalUserIndex].pChatUser != NULL) && 
        (pRemoteUser->ePlatform == VOIP_PLATFORM_XBOXONE) &&
        (!VOIP_NullUser(pRemoteUser)) && 
        (pRemoteChatUser != NULL))
    {
        game_chat_communication_relationship_flags communicationFlags;
        game_chat_communication_relationship_adjuster communicationAdjuster;

        pHeadset->aLocalUsers[iLocalUserIndex].pChatUser->local()->get_effective_communication_relationship(pRemoteChatUser, &communicationFlags, &communicationAdjuster);
            
        if ((communicationAdjuster != game_chat_communication_relationship_adjuster::initializing) 
            && (communicationAdjuster != game_chat_communication_relationship_adjuster::uninitialized) 
            && (communicationAdjuster != game_chat_communication_relationship_adjuster::none))
        {
            if ((communicationFlags & game_chat_communication_relationship_flags::receive_audio) != game_chat_communication_relationship_flags::receive_audio)
            {
                bMuted = TRUE;
            }
        }
    }

    return(bMuted);
}


/*F********************************************************************************/
/*!
    \Function _VoipHeadsetPlaybackCallback

    \Description
        Determines if we should be submitting audio to the given mixer

    \Input *pMixer          - mixer ref
    \Input *pRemoteUser     - remote user who sent the mic packets
    \Input *pUserData       - callback user data (VoipHeadsetRefT)

    \Output
        uint8_t             - TRUE for playback, FALSE for not to playback

    \Version 4/15/2019 (tcho)
*/
/********************************************************************************F*/
static uint8_t _VoipHeadsetPlaybackCallback(VoipMixerRefT *pMixer, VoipUserT *pRemoteUser, void *pUserData)
{
    uint8_t bPlayback = TRUE;
    int32_t iMixerStreamIndex, iLocalUserIndex, iRemoteUserIndex;
    uint32_t uUserIndex, uChatUserCount;
    chat_user_array aUsers;
    post_decode_audio_sink_stream  *pSinkStream = NULL;
    VoipHeadsetRefT *pHeadset = (VoipHeadsetRefT *)pUserData;
    
    // find the corresponding audio sink stream
    for (iMixerStreamIndex = 0; iMixerStreamIndex < VOIP_MAXLOCALUSERS_EXTENDED; iMixerStreamIndex += 1)
    {
        if (pHeadset->aLocalMixerStreams[iMixerStreamIndex].pMixer == pMixer)
        {
            pSinkStream = pHeadset->aLocalMixerStreams[iMixerStreamIndex].pSinkStream;
            break;
        }
    }

    if (pSinkStream == NULL)
    {
        NetPrintf(("voipheadsetxboxone: _VoipHeadsetPlaybackCallback() cannot find the sink stream for user iPersonaId: %lld", pRemoteUser->AccountInfo.iPersonaId));
        return(bPlayback);
    }

    // find the remote user index
    iRemoteUserIndex = _VoipHeadsetGetRemoteUserIndex(pHeadset, pRemoteUser);

    if (iRemoteUserIndex < 0)
    {
        NetPrintf(("voipheadsetxboxone: _VoipHeadsetPlaybackCallback() cannot find the remote user iPersonaId: %lld", pRemoteUser->AccountInfo.iPersonaId));
        return(bPlayback);
    }

    // retreive all the users associated with the sink stream
    pSinkStream->get_users(&uChatUserCount, &aUsers);

    // loop through all the users to determine the muting
    for (uUserIndex = 0; uUserIndex < uChatUserCount; uUserIndex += 1)
    {
        uint64_t uXuid = _wtoi64(aUsers[uUserIndex]->xbox_user_id());
        for (iLocalUserIndex = 0; iLocalUserIndex < VOIP_MAXLOCALUSERS; iLocalUserIndex += 1)
        {
            if (pHeadset->aLocalUsers[iLocalUserIndex].uXuid == uXuid)
            {
                // muting from +-pbk selectors
                if ((pHeadset->aLocalUsers[iLocalUserIndex].uPlaybackFlags & (1 << iRemoteUserIndex)) == 0) 
                {
                    bPlayback = FALSE;
                    break;
                }
            }
        }

        // we are done if the anyone of users associated with the VoipMixer is muted with any local user
        if (bPlayback == FALSE)
        {
            break;
        }
    }
    return(bPlayback);
}

/*F********************************************************************************/
/*!
    \Function _VoipHeadsetTranscibeCreate

    \Description
        Create Transcription module for the give local user

    \Input pHeadset            - headset ref
    \Input iLocalUserIndex     - user for which you want to create the transcribe module for

    \Output
       VoipTranscribeRefT *     - trascribe ref of given user

    \Version 4/15/2019 (tcho)
*/
/********************************************************************************F*/
static VoipTranscribeRefT *_VoipHeadsetTranscibeCreate(VoipHeadsetRefT *pHeadset, int32_t iLocalUserIndex)
{

    uint64_t uXuid;

    int32_t iPreEncodeStreamIndex = 0;

    NetConnStatus('xuid', iLocalUserIndex, &uXuid, sizeof(uXuid));
    std::wstring wstrXuid = std::to_wstring(uXuid);

    for (iPreEncodeStreamIndex = 0; iPreEncodeStreamIndex < VOIP_MAXLOCALUSERS; iPreEncodeStreamIndex += 1)
    {
        uint32_t uChatUserCount = 0;
        uint32_t uChatUserIndex = 0;
        chat_user_array aUsersArray;

        if (pHeadset->aPreEncodedStreams[iPreEncodeStreamIndex].pAudioStream != NULL)
        {
            pHeadset->aPreEncodedStreams[iPreEncodeStreamIndex].pAudioStream->get_users(&uChatUserCount, &aUsersArray);
            
            for (uChatUserIndex = 0; uChatUserIndex < uChatUserCount; uChatUserIndex += 1)
            {
                if (wcscmp(aUsersArray[uChatUserIndex]->xbox_user_id(), wstrXuid.c_str()) == 0)
                {
                    break;
                }
            }

            if (uChatUserIndex < uChatUserCount)
            {
                break;
            }
        }
    }

    if (iPreEncodeStreamIndex < VOIP_MAXLOCALUSERS)
    {
        if (pHeadset->aPreEncodedStreams[iPreEncodeStreamIndex].pTranscribeRef == NULL)
        {
            pHeadset->aPreEncodedStreams[iPreEncodeStreamIndex].pTranscribeRef = VoipTranscribeCreate(0);
        }

        return(pHeadset->aPreEncodedStreams[iPreEncodeStreamIndex].pTranscribeRef);
    }
    else
    {
        return(NULL);
    }
}

/*F********************************************************************************/
/*!
    \Function _VoipHeadsetDumpChatUsers

    \Description
        Dump user info for all users registered with the chat manager.

    \Input pHeadset            - headset ref

    \Version 11/27/2018 (mclouatre)
*/
/********************************************************************************F*/
static void _VoipHeadsetDumpChatUsers(VoipHeadsetRefT *pHeadset)
{
    uint32_t uChatUserIndex;
    uint32_t uChatUserCount;
    chat_user_array aChatUsers;
    chat_user *pChatUser = NULL;

    // get collection of chat users from the chat manager
    chat_manager::singleton_instance().get_chat_users(&uChatUserCount, &aChatUsers);

    NetPrintf(("voipheadsetxboxone: CHAT USERS DUMP  --  %d chat user(s) currently registered with the chat manager:\n", uChatUserCount));

    // look for a chat user with the matching XBoxUserId
    for (uChatUserIndex = 0; uChatUserIndex < uChatUserCount; uChatUserIndex += 1)
    {
        pChatUser = aChatUsers[uChatUserIndex];

        NetPrintf(("voipheadsetxboxone: chat user #%d --> local:%s   xuid:%S   chat_indicator:%s\n",
            uChatUserIndex, (pChatUser->local() == NULL) ? "n" : "y", pChatUser->xbox_user_id(),
            _VoipHeadsetGetChatIndicatorString(pChatUser->chat_indicator())));
    }

    #if DIRTYCODE_LOGGING
    pHeadset->uLastChatUsersDump = NetTick();
    #endif
}

/*F********************************************************************************/
/*!
    \Function _VoipHeadsetGetXboxUserIdForVoipUser

    \Description
        Fills the caller's buffer with the XBoxUserId (wchar_t string) for the 
        specified user.

    \Input *pHeadset        - headset ref
    \Input *pUser           - user for which the XBoxUserId is queried
    \Input *pXboxUserId     - [out] pointer to be filled in with the XBoxUserId (wchar_t string)
    \Input iXboxUserIdLen   - size of caller's buffer
     
    \Output
        int32_t             - <0:error, >=0: number of bytes written in caller's buffer

    \Version 11/22/2018 (mclouatre)
*/
/********************************************************************************F*/
static int32_t _VoipHeadsetGetXboxUserIdForVoipUser(VoipHeadsetRefT *pHeadset, VoipUserT *pUser, wchar_t *pXboxUserId, int32_t iXboxUserIdLen)
{
    uint64_t uXuid = 0;
        
    if (pUser->ePlatform == VOIP_PLATFORM_XBOXONE)
    {
        if (pHeadset->pFirstPartyIdCallback != NULL)
        {
            uXuid = pHeadset->pFirstPartyIdCallback(pUser->AccountInfo.iPersonaId, pHeadset->pFirstPartyIdCbUserData);
            return(_snwprintf(pXboxUserId, iXboxUserIdLen, L"%lld", uXuid));
        }
    }
    return(0);
}

/*F********************************************************************************/
/*!
    \Function _VoipHeadsetGetChatUserForXBoxUserId

    \Description
        Returns the chat_user object that matches the specified XUID.

    \Input *wstrXuid    - xuid for which the chat_user is queried
     
    \Output
       chat_user *      - chat_user pointer, NULL if not found

    \Version 11/22/2018 (mclouatre)
*/
/********************************************************************************F*/
static chat_user * _VoipHeadsetGetChatUserForXBoxUserId(const wchar_t *wstrXuid)
{
    uint32_t uChatUserIndex;
    uint32_t uChatUserCount;
    chat_user_array aChatUsers;
    chat_user *pChatUser = NULL;

    // get collection of chat users from the chat manager
    chat_manager::singleton_instance().get_chat_users(&uChatUserCount, &aChatUsers);

    // look for a chat user with the matching Xuid
    for (uChatUserIndex = 0; uChatUserIndex < uChatUserCount; uChatUserIndex += 1)
    {
        pChatUser = aChatUsers[uChatUserIndex];

        if (wcscmp(pChatUser->xbox_user_id(), wstrXuid) == 0)
        {
            // found!
            break;
        }
        else
        {
            pChatUser = NULL;
        }
    }

    return(pChatUser);
}

/*F********************************************************************************/
/*!
    \Function _VoipHeadsetGetChatUserForVoipUser

    \Description
        Returns the chat_user object that matches the specifief VoipUser.

    \Input *pHeadset        - user for which the chat_user is queried
    \Input *pUser           - user for which the chat_user is queried
     
    \Output
       chat_user *          - chat_user pointer, NULL if not found

    \Version 11/22/2018 (mclouatre)
*/
/********************************************************************************F*/
static chat_user * _VoipHeadsetGetChatUserForVoipUser(VoipHeadsetRefT *pHeadset, VoipUserT *pUser)
{
    uint64_t uXuid = 0;

    if (pUser->ePlatform == VOIP_PLATFORM_XBOXONE)
    {
        if (pHeadset->pFirstPartyIdCallback != NULL)
        {
            uXuid = pHeadset->pFirstPartyIdCallback(pUser->AccountInfo.iPersonaId, pHeadset->pFirstPartyIdCbUserData);
        }

        return(_VoipHeadsetGetChatUserForXBoxUserId(std::to_wstring(uXuid).c_str()));
    }

    return(NULL);
}

/*F********************************************************************************/
/*!
    \Function _VoipHeadsetGetVoipUserForChatUser

    \Description
        Fills in the voipuser buffer with the xuid of specified chat user.

    \Input *pHeadset        - pHeadset ref
    \Input *pChatUser       - chat user
    \Input *pVoipUser       - [OUT] VoipUserT to be initialized

    \Version 11/22/2018 (mclouatre)
*/
/********************************************************************************F*/
static void _VoipHeadsetGetVoipUserForChatUser(VoipHeadsetRefT *pHeadset, chat_user *pChatUser, VoipUserT *pVoipUser)
{
    uint64_t uXuid;
    int32_t iUserIndex;

    swscanf(pChatUser->xbox_user_id(), L"%I64d", &uXuid);

    // check local users
    for (iUserIndex = 0; iUserIndex < VOIP_MAXLOCALUSERS; iUserIndex += 1)
    {
        if (pHeadset->aLocalUsers[iUserIndex].uXuid == uXuid)
        {
            VOIP_CopyUser(pVoipUser, &pHeadset->aLocalUsers[iUserIndex].User);
            return;
        }
    }

    // check remote users
    for (iUserIndex = 0; iUserIndex < pHeadset->iMaxRemoteUsers; iUserIndex += 1)
    {
        if (pHeadset->aRemoteUsers[iUserIndex].uXuid == uXuid)
        {
            VOIP_CopyUser(pVoipUser, &pHeadset->aRemoteUsers[iUserIndex].User);
            return;
        }
    }
}

/*F********************************************************************************/
/*!
    \Function _VoipHeadsetAddLocalTalker

    \Description
        Create a game chat participant for this local user and populate the
        aLocalUser array.

    \Input *pHeadset        - headset module
    \Input iLocalUserIndex  - local user index
    \Input *pLocalUser      - local user

    \Output
        int32_t             - negative=error, zero=success

    \Version 05/24/2013 (mclouatre)
*/
/********************************************************************************F*/
static int32_t _VoipHeadsetAddLocalTalker(VoipHeadsetRefT *pHeadset, int32_t iLocalUserIndex, VoipUserT *pLocalUser)
{
    uint64_t uXuid;
    char strGamertag[16];

    if (VOIP_NullUser(pLocalUser))
    {
        NetPrintf(("voipheadsetxboxone: can't add NULL local talker at local user index %d\n", iLocalUserIndex));
        return(-1);
    }

    // make sure there is no other local talker
    if (pHeadset->aLocalUsers[iLocalUserIndex].pChatUser != NULL)
    {
        NetPrintf(("voipheadsetxboxone: can't add multiple local talkers at the same index (%d)\n", iLocalUserIndex));
        return(-2);
    }

    // find the system local user's xuid
    if (NetConnStatus('xuid', iLocalUserIndex, &uXuid, sizeof(uXuid)) < 0)
    {
        NetPrintf(("voipheadsetxboxone: local user at index %d either doesn't exist or has invalid xuid\n", iLocalUserIndex));
        return(-4);
    }

    // find the system local user's gamertag
    if (NetConnStatus('gtag', iLocalUserIndex, &strGamertag, sizeof(strGamertag)) < 0)
    {
        NetPrintf(("voipheadsetxboxone: could not retrieve gamertag for local user at index %d\n", iLocalUserIndex));
        return(-5);
    }

    // see if there is already a matching user registered with the chat manager
    std::wstring wstrXuid = std::to_wstring(uXuid);
    if (_VoipHeadsetGetChatUserForXBoxUserId(wstrXuid.c_str()) == NULL)
    {
        VOIP_CopyUser(&pHeadset->aLocalUsers[iLocalUserIndex].User, pLocalUser);
        pHeadset->aLocalUsers[iLocalUserIndex].uXuid = uXuid;
        pHeadset->aLocalUsers[iLocalUserIndex].pChatUser = chat_manager::singleton_instance().add_local_user(wstrXuid.c_str());
        pHeadset->aLocalUsers[iLocalUserIndex].uPlaybackFlags = 0xffffffff;
        NetPrintf(("voipheadsetxboxone: adding local talker %lld at index %d (gamertag: %s, xuid: %llu)\n",
            pLocalUser->AccountInfo.iPersonaId, iLocalUserIndex, strGamertag, uXuid));
    }
    else
    {
        NetPrintf(("voipheadsetxboxone: local talker %lld at local user index %d is already known by the chat manager\n",
            pLocalUser->AccountInfo.iPersonaId, iLocalUserIndex));
        return(-6);
    }

    _VoipHeadsetDumpChatUsers(pHeadset);

    return(0);
}

/*F********************************************************************************/
/*!
    \Function _VoipHeadsetRemoveLocalTalker

    \Description
        Destroy the game chat participant for this local user and remove the user 
        from the aLocalUser array.

    \Input *pHeadset        - headset module
    \Input iLocalUserIndex  - local index
    \Input *pLocalUser      - local user

    \Output
        int32_t             - negative=error, zero=success

    \Version 01/22/2013 (mclouatre)
*/
/********************************************************************************F*/
static int32_t _VoipHeadsetRemoveLocalTalker(VoipHeadsetRefT *pHeadset, int32_t iLocalUserIndex, VoipUserT *pLocalUser)
{
    int32_t iRetCode = 0;

    if (pHeadset->aLocalUsers[iLocalUserIndex].bParticipating == FALSE)
    {
        if (pHeadset->aLocalUsers[iLocalUserIndex].pChatUser != NULL)
        {
            NetPrintf(("voipheadsetxboxone: deleting local talker (xuid: %S) at index %d\n",
                pHeadset->aLocalUsers[iLocalUserIndex].pChatUser->xbox_user_id(), iLocalUserIndex));

            chat_manager::singleton_instance().remove_user(pHeadset->aLocalUsers[iLocalUserIndex].pChatUser);
            pHeadset->aLocalUsers[iLocalUserIndex].pChatUser = FALSE;
            pHeadset->aLocalUsers[iLocalUserIndex].uXuid = 0;
            VOIP_ClearUser(&pHeadset->aLocalUsers[iLocalUserIndex].User);
            pHeadset->aLocalUsers[iLocalUserIndex].uPlaybackFlags = 0xffffffff;
        }
        else
        {
            NetPrintf(("voipheadsetxboxone: local talker at local user index %d is already deleted\n", iLocalUserIndex));
            iRetCode = -1;
        }
    }
    else
    {
        NetPrintf(("voipheadsetxboxone: cannot delete local talker (xuid: %S) at local user index %d because the user must first be deactivated\n",
            pHeadset->aLocalUsers[iLocalUserIndex].pChatUser->xbox_user_id(), iLocalUserIndex));
        iRetCode = -2;
    }

    _VoipHeadsetDumpChatUsers(pHeadset);

    return(iRetCode);
}

/*F********************************************************************************/
/*!
    \Function _VoipHeadsetActivateLocalTalker

    \Description
        Create a game chat participant for this local user and populate the
        aLocalUser array.

    \Input *pHeadset        - headset module
    \Input iLocalUserIndex  - local user index
    \Input bParticipate     - register flag

    \Output
        int32_t             - negative=error, zero=success

    \Version 05/14/2014 (amakoukji)
*/
/********************************************************************************F*/
static int32_t _VoipHeadsetActivateLocalTalker(VoipHeadsetRefT *pHeadset, int32_t iLocalUserIndex, bool bParticipate)
{
    uint64_t uXuid;

    // find the system local user's xuid 
    if (NetConnStatus('xuid', iLocalUserIndex, &uXuid, sizeof(uXuid)) < 0)
    {
        NetPrintf(("voipheadsetxboxone: local user at index %d either doesn't exist or has an invalid xuid\n", iLocalUserIndex));
        return(-1);
    }

    if ((pHeadset->aLocalUsers[iLocalUserIndex].bParticipating == FALSE) && (bParticipate == FALSE))
    {
        NetPrintf(("voipheadsetxboxone: cannot demote user at local index %d because not already a participating user. Ignoring.\n", iLocalUserIndex));
        return(-2);
    }

    pHeadset->aLocalUsers[iLocalUserIndex].bParticipating = bParticipate;
    NetPrintf(("voipheadsetxboxone: local user (xuid: %S) at index %d is %s participating\n",
        pHeadset->aLocalUsers[iLocalUserIndex].pChatUser->xbox_user_id(), iLocalUserIndex, (bParticipate ? "now" : "no longer")));

    return(0);
}

/*F********************************************************************************/
/*!
    \Function _VoipHeadsetAddRemoteTalker

    \Description
        Create the game chat participant for this remote user.

    \Input *pHeadset    - headset module
    \Input *pRemoteUser - remote user
    \Input uConsoleId   - unique console identifier (local scope only, does not need to be the same on all hosts)

    \Output
        int32_t         - negative=error else returns remote user index

    \Version 07/12/2013 (mclouatre)
*/
/********************************************************************************F*/
static int32_t _VoipHeadsetAddRemoteTalker(VoipHeadsetRefT *pHeadset, VoipUserT *pRemoteUser, uint64_t uConsoleId)
{
    int iRetCode = 0;
    int32_t iRemoteUserIndex;
    XONERemoteVoipUserT *pXoneRemoteUser = NULL;

    // find a duplicate entry
    for (iRemoteUserIndex = 0; iRemoteUserIndex < pHeadset->iMaxRemoteUsers; iRemoteUserIndex += 1)
    {
        if (VOIP_SameUser(&pHeadset->aRemoteUsers[iRemoteUserIndex].User, pRemoteUser))
        {
            pXoneRemoteUser = &pHeadset->aRemoteUsers[iRemoteUserIndex];
            break;
        }
    }

    if (pXoneRemoteUser == NULL)
    {
        // find a empty remote user entry
        for (iRemoteUserIndex = 0; iRemoteUserIndex < pHeadset->iMaxRemoteUsers; iRemoteUserIndex += 1)
        {
            if (VOIP_NullUser(&pHeadset->aRemoteUsers[iRemoteUserIndex].User))
            {
                ds_memclr(&pHeadset->aRemoteUsers[iRemoteUserIndex], sizeof(XONERemoteVoipUserT));
                VOIP_CopyUser(&pHeadset->aRemoteUsers[iRemoteUserIndex].User, pRemoteUser);
                pXoneRemoteUser = &pHeadset->aRemoteUsers[iRemoteUserIndex];
                break;
            }
        }

        if (pXoneRemoteUser != NULL)
        {
            if (pXoneRemoteUser->User.ePlatform == VOIP_PLATFORM_XBOXONE)
            {
                wchar_t wstrXuid[VOIPHEADSET_XUID_SAFE_LEN];
                _VoipHeadsetGetXboxUserIdForVoipUser(pHeadset, &pXoneRemoteUser->User, wstrXuid, sizeof(wstrXuid));
                pXoneRemoteUser->uXuid = _wtoi64(wstrXuid);

                NetPrintf(("voipheadsetxboxone: adding remote talker %lld (xuid: %S, console id: 0x%llx)\n",
                    pRemoteUser->AccountInfo.iPersonaId, wstrXuid, uConsoleId));

                pXoneRemoteUser->pChatUser = chat_manager::singleton_instance().add_remote_user(wstrXuid, uConsoleId);

                if (pXoneRemoteUser->pChatUser != NULL)
                {
                    pXoneRemoteUser->pChatUser->set_custom_user_context((void *)pXoneRemoteUser);

                    if (pHeadset->bCrossplay == TRUE)
                    {
                        int32_t iLocalUserIndex;
                        for (iLocalUserIndex = 0; iLocalUserIndex < VOIP_MAXLOCALUSERS; iLocalUserIndex += 1)
                        {
                            if (pHeadset->aLocalUsers[iLocalUserIndex].pChatUser != NULL)
                            {
                                pHeadset->aLocalUsers[iLocalUserIndex].pChatUser->local()->set_communication_relationship(pXoneRemoteUser->pChatUser, c_communicationRelationshipSendAndReceiveAll);
                            }
                        }
                    }
                }
                else
                {
                    NetPrintf(("voipheadsetxboxone: adding remote talker %lld failed because it chat_matanger::add_remote_user() returned NULL\n", pRemoteUser->AccountInfo.iPersonaId));
                    ds_memclr(&pHeadset->aRemoteUsers[iRemoteUserIndex], sizeof(XONERemoteVoipUserT));
                    iRetCode = -1;
                }
            }

            // register the remote user with all the conduits
            pXoneRemoteUser->uConsoleId = uConsoleId;
            VoipConduitRegisterUser(pHeadset->pConduitRef, &pXoneRemoteUser->User, TRUE);
            iRetCode = iRemoteUserIndex;
        }
        else
        {
            NetPrintf(("voipheadsetxboxone: adding remote talker %lld failed because aRemoteUsers is full\n", pRemoteUser->AccountInfo.iPersonaId));
            iRetCode = -2;
        }
    }
    else
    {
        NetPrintf(("voipheadsetxboxone: adding remote talker %lld failed because it already exists\n", pRemoteUser->AccountInfo.iPersonaId));
        iRetCode = -3;
    }

    #if DIRTYCODE_LOGGING
    pHeadset->bDumpChatUsers = TRUE;
    #endif

    return(iRetCode);
}

/*F********************************************************************************/
/*!
    \Function _VoipHeadsetRemoveRemoteTalker

    \Description
        Remove the specified remote user from the collection of users known by the chat manager.

    \Input *pHeadset    - headset module
    \Input *pUser       - user to be removed

    \Output
        int32_t         - negative=error, zero=success

    \Version 11/22/2018 (mclouatre)
*/
/********************************************************************************F*/
static int32_t _VoipHeadsetRemoveRemoteTalker(VoipHeadsetRefT *pHeadset, VoipUserT *pUser)
{
    int32_t iRetCode = 0;
    int32_t iRemoteUserIndex;
    XONERemoteVoipUserT *pXoneRemoteUser = NULL;
    chat_user *pChatUser = NULL;

    VoipConduitRegisterUser(pHeadset->pConduitRef, pUser, FALSE);
    
    // find the remote user
    for (iRemoteUserIndex = 0; iRemoteUserIndex < pHeadset->iMaxRemoteUsers; iRemoteUserIndex += 1)
    {
        if (VOIP_SameUser(&pHeadset->aRemoteUsers[iRemoteUserIndex].User, pUser))
        {
            pXoneRemoteUser = &pHeadset->aRemoteUsers[iRemoteUserIndex];
            pChatUser = pHeadset->aRemoteUsers[iRemoteUserIndex].pChatUser;
            break;
        }
    }

    if (pXoneRemoteUser != NULL)
    {
        if (pXoneRemoteUser->User.ePlatform == VOIP_PLATFORM_XBOXONE)
        {
            if (pChatUser != NULL)
            {
                NetPrintf(("voipheadsetxboxone: deleting remote talker (xuid: %d)\n", pXoneRemoteUser->uXuid));
                chat_manager::singleton_instance().remove_user(pChatUser);
                pXoneRemoteUser->pChatUser = NULL;
            }
            else
            {
                NetPrintf(("voipheadsetxboxone: deleting remote talker %lld failed because it cannot not find the associated game chat user\n", pUser->AccountInfo.iPersonaId));
                iRetCode = -1;
            }
        }

        if (iRetCode == 0)
        {
            ds_memclr(pXoneRemoteUser, sizeof(XONERemoteVoipUserT));
        }
    }
    else
    {
        NetPrintf(("voipheadsetxboxone: deleting remote talker %lld failed because it does not exists\n", pUser->AccountInfo.iPersonaId));
        iRetCode = -2;
    }

    #if DIRTYCODE_LOGGING
    pHeadset->bDumpChatUsers = TRUE;
    #endif

    return(iRetCode);
}

/*F********************************************************************************/
/*!
    \Function _VoipHeadsetSynthesizeSpeech

    \Description
        Text-to-speech for the specified user.

    \Input *pHeadset        - headset module
    \Input iLocalUserIndex  - local user index (text originator)
    \Input *pText           - text to by synthesized

    \Output
        int32_t             - 0 if success, negative otherwise

    \Version 04/25/2017 (mclouatre)
*/
/********************************************************************************F*/
static int32_t _VoipHeadsetSynthesizeSpeech(VoipHeadsetRefT *pHeadset, int32_t iLocalUserIndex, const char *pText)
{
    int32_t iRetCode = 0; // default to success

    if ((pHeadset->bCrossplay == TRUE) && (pHeadset->aLocalUsers[iLocalUserIndex].bParticipating == TRUE))
    {
        // create the narrate module if needed
        if (_VoipHeadsetCreateNarrate(pHeadset, iLocalUserIndex) == 0)
        {
            VoipNarrateInput(pHeadset->aLocalUsers[iLocalUserIndex].pNarrateRef, iLocalUserIndex, pHeadset->aLocalUsers[iLocalUserIndex].eGender, pText);
        }
    }
    else if ((pHeadset->bCrossplay == FALSE) && (pHeadset->aLocalUsers[iLocalUserIndex].pChatUser != NULL))
    {
        // make sure the user does want TTS
        if (pHeadset->aLocalUsers[iLocalUserIndex].pChatUser->local()->text_to_speech_conversion_preference_enabled() == true)
        {
            // make sure we have a large enough buffer for the text (in the correct format)
            int32_t iSizeNeeded = MultiByteToWideChar(CP_UTF8, 0, pText, -1, NULL, 0);
            if (pHeadset->iTTSInputLen < iSizeNeeded)
            {
                if (pHeadset->pTTSInputText != NULL)
                {
                    DirtyMemFree(pHeadset->pTTSInputText, VOIP_MEMID, pHeadset->iMemGroup, pHeadset->pMemGroupUserData);
                    pHeadset->pTTSInputText = NULL;
                }

                if ((pHeadset->pTTSInputText = (wchar_t *)DirtyMemAlloc((int32_t)(sizeof(wchar_t) * iSizeNeeded), VOIP_MEMID, pHeadset->iMemGroup, pHeadset->pMemGroupUserData)) == NULL)
                {
                    NetPrintf(("voipheadsetxboxone: cannot allocate ttos text!\n"));
                    iRetCode = -1;
                }
                else
                {
                    pHeadset->iTTSInputLen = iSizeNeeded;
                }
            }

            // we do have a large enough buffer, carry on with the conversion and synthesize
            if (iRetCode == 0)
            {
                int32_t iStringLength = MultiByteToWideChar(CP_UTF8, 0, pText, -1, pHeadset->pTTSInputText, iSizeNeeded);
                pHeadset->aLocalUsers[iLocalUserIndex].pChatUser->local()->synthesize_text_to_speech(pHeadset->pTTSInputText);

                // gather metrics, note on xone we have no way of getting; uDelay, uDurationMsRecv, uEmptyResultCount
                pHeadset->aTTSMetrics[iLocalUserIndex].uEventCount += 1;
                pHeadset->aTTSMetrics[iLocalUserIndex].uCharCountSent += iStringLength;
            }
        }
        else
        {
            NetPrintf(("voipheadsetxboxone: game chat 2 will not synthesize speech for local talker at local user index %d because it has TTS disabled\n", iLocalUserIndex));
        }
    }
    else
    {
        NetPrintf(("voipheadsetxboxone: can't synthesize speech for local talker at local user index %d because no matching chat user found\n", iLocalUserIndex));
        iRetCode = -2;
    }

    // note that iRetCode only changes in the "native" code path
    if (iRetCode != 0)
    {
        pHeadset->aTTSMetrics[iLocalUserIndex].uErrorCount += 1;
    }

    return(iRetCode);
}

/*F********************************************************************************/
/*!
    \Function _VoipHeadsetProcessLoopback

    \Description
        Submit the loopback audio to the output device

    \Input *pHeadset    - pointer to headset state
    \Input *pLocalUser  - user for which the audio belongs

    \Version 3/15/2019 (eesponda)
*/
/********************************************************************************F*/
static void _VoipHeadsetProcessLoopback(VoipHeadsetRefT *pHeadset, XONELocalVoipUserT *pLocalUser)
{
    XAUDIO2_VOICE_STATE State;
    pLocalUser->pSourceVoice->GetState(&State);
    if (State.BuffersQueued == 0)
    {
        if (pLocalUser->iAudioLen > 0)
        {
            XAUDIO2_BUFFER Buffer = {};
            Buffer.pAudioData = pLocalUser->aAudioData;
            Buffer.AudioBytes = pLocalUser->iAudioLen;
            pLocalUser->pSourceVoice->SubmitSourceBuffer(&Buffer);
            pLocalUser->iAudioLen = 0;
        }
        else if (pLocalUser->bStopping)
        {
            pLocalUser->pSourceVoice->Stop();
            pLocalUser->bStopping = FALSE;
        }
    }
}

/*F********************************************************************************/
/*!
    \Function _VoipHeadsetUpdateStatus

    \Description
        Headset update function.

    \Input *pHeadset    - pointer to headset state

    \Notes
        The function does not do audioout detection as it is assumed that the speakers
        will always be available if the user is not using a headset.

    \Version 11/23/2018 (mclouatre)
*/
/********************************************************************************F*/
static void _VoipHeadsetUpdateStatus(VoipHeadsetRefT *pHeadset)
{
    int32_t iLocalUserIndex;

    for (iLocalUserIndex = 0; iLocalUserIndex < VOIP_MAXLOCALUSERS; iLocalUserIndex += 1)
    {
        XONELocalVoipUserT *pLocalUser = &pHeadset->aLocalUsers[iLocalUserIndex];

        if (pLocalUser->pChatUser != NULL)
        {
            uint8_t bMicDetected = (pHeadset->aLocalUsers[iLocalUserIndex].pChatUser->chat_indicator() == game_chat_user_chat_indicator::no_microphone) ? FALSE : TRUE;
            
            if (pHeadset->aLocalUsers[iLocalUserIndex].bMic != bMicDetected)
            {
                pHeadset->pStatusCb(iLocalUserIndex, bMicDetected, VOIP_HEADSET_STATUS_INPUT, pHeadset->pCbUserData);
                pHeadset->aLocalUsers[iLocalUserIndex].bMic = bMicDetected;
            }
        }
    }
}

/*F********************************************************************************/
/*!
    \Function _VoipHeadsetProcessDataFrames

    \Description
        Give a chance to game chat 2 to send 'unreliable' and 'relialbe' data
        over the network to the game chat 2 instances at remote end of voip
        connections.

    \Input *pHeadset    - headset module

    \Version 11/22/2018 (mclouatre)
*/
/********************************************************************************F*/
static void _VoipHeadsetProcessDataFrames(VoipHeadsetRefT *pHeadset)
{
    uint32_t uDataFrameIndex;
    uint32_t uDataFrameCount;
    game_chat_data_frame_array aDataFrames;

    chat_manager::singleton_instance().start_processing_data_frames(&uDataFrameCount, &aDataFrames);

    for (uDataFrameIndex = 0; uDataFrameIndex < uDataFrameCount; uDataFrameIndex += 1)
    {
        uint32_t uTargetEndpointIndex;
        uint32_t uSendMask = 0;
        game_chat_data_frame const *pDataFrame = aDataFrames[uDataFrameIndex];
        uint8_t bReliable = (pDataFrame->transport_requirement == game_chat_data_transport_requirement::guaranteed) ? TRUE : FALSE;

        // build send mask from collection of endpoint identifiers
        for (uTargetEndpointIndex = 0; uTargetEndpointIndex < pDataFrame->target_endpoint_identifier_count; uTargetEndpointIndex += 1)
        {
            uSendMask |= (1 << pDataFrame->target_endpoint_identifiers[uTargetEndpointIndex]);
        }

        pHeadset->pOpaqueDataCb(pDataFrame->packet_buffer, pDataFrame->packet_byte_count, uSendMask, bReliable, pHeadset->uSendSeq++, pHeadset->pCbUserData);
    }

    chat_manager::singleton_instance().finish_processing_data_frames(aDataFrames);
}

/*F********************************************************************************/
/*!
    \Function _VoipHeadsetHandlePostDecodeSinkStreamCreated

    \Description
        Process stream notifications game_chat_stream_state_change_type::post_decode_audio_sink_stream_created

    \Input *pHeadset    - headset module
    \Input *pSinkStream - post decode audio sink stream

    \Version 01/30/2019 (tcho)
*/
/********************************************************************************F*/
static void _VoipHeadsetHandlePostDecodeSinkStreamCreated(VoipHeadsetRefT *pHeadset, post_decode_audio_sink_stream *pSinkStream)
{
    int32_t iMixerStreamIndex;
    XONELocalMixerStreamT *pCurMixerStream = NULL;
    game_chat_audio_format gameChatAudioFormat;

    // find an empty spot
    for (iMixerStreamIndex = 0; iMixerStreamIndex < VOIP_MAXLOCALUSERS_EXTENDED; iMixerStreamIndex += 1)
    {
        if (pHeadset->aLocalMixerStreams[iMixerStreamIndex].pMixer == NULL)
        {
            pCurMixerStream = &pHeadset->aLocalMixerStreams[iMixerStreamIndex];
            break;
        }
    }

    if (pCurMixerStream == NULL)
    {
        NetPrintf(("voipheadsetxboxone: no space to add sink stream (ID:%s)\n", pSinkStream->get_device_id()));
        return;
    }

    if ((pCurMixerStream->pMixer = VoipMixerCreate(16, VOIPHEADSET_FRAMESAMPLES)) == NULL)
    {
        NetPrintf(("voipheadsetxboxone: failed to create voip mixer for sink stream (ID:%s)\n", pSinkStream->get_device_id()));
        return;
    }

    gameChatAudioFormat.bits_per_sample = VOIPHEADSET_BYTESPERSAMPLES * 8;
    gameChatAudioFormat.channel_count = 1;
    gameChatAudioFormat.channel_mask = 0;
    gameChatAudioFormat.is_interleaved = false;
    gameChatAudioFormat.sample_rate = VOIPHEADSET_SAMPLERATE;
    gameChatAudioFormat.sample_type = game_chat_sample_type::integer;

    pSinkStream->set_processed_format(gameChatAudioFormat);
    pCurMixerStream->pSinkStream = pSinkStream;

    // add the mixer to all the conduits
    VoipConduitMixerSet(pHeadset->pConduitRef, pCurMixerStream->pMixer);
}

/*F********************************************************************************/
/*!
    \Function _VoipHeadsetHandlePostDecodeSinkStreamClosed

    \Description
        Process stream notifications game_chat_stream_state_change_type::post_decode_audio_sink_stream_closed

    \Input *pHeadset    - headset module
    \Input *pSinkStream - post decode sudio sink stream

    \Version 01/30/2019 (tcho)
*/
/********************************************************************************F*/
static void _VoipHeadsetHandlePostDecodeSinkStreamClosed(VoipHeadsetRefT *pHeadset, post_decode_audio_sink_stream *pSinkStream)
{
    int32_t iMixerStreamIndex;

    // find matching sink stream
    for (iMixerStreamIndex = 0; iMixerStreamIndex < VOIP_MAXLOCALUSERS; iMixerStreamIndex += 1)
    {
        if (pHeadset->aLocalMixerStreams[iMixerStreamIndex].pSinkStream == pSinkStream)
        {
            if (pHeadset->pConduitRef != NULL)
            {
                VoipConduitMixerUnset(pHeadset->pConduitRef, pHeadset->aLocalMixerStreams[iMixerStreamIndex].pMixer);
            }

            VoipMixerDestroy(pHeadset->aLocalMixerStreams[iMixerStreamIndex].pMixer);
            pHeadset->aLocalMixerStreams[iMixerStreamIndex].pMixer = NULL;
            pHeadset->aLocalMixerStreams[iMixerStreamIndex].pSinkStream = NULL;
            break;
        }
    }
}

/*F********************************************************************************/
/*!
    \Function _VoipHeadsetHandlePreEncodeAudioStreamCreated

    \Description
        Process stream notification game_chat_stream_state_change_type::pre_encode_audio_stream_created

    \Input *pHeadset    - headset module
    \Input *pAudioStream - recently created pre encode audio stream

    \Version 03/04/2019 (cvienneau)
*/
/********************************************************************************F*/
static void _VoipHeadsetHandlePreEncodeAudioStreamCreated(VoipHeadsetRefT *pHeadset, pre_encode_audio_stream *pAudioStream)
{
    int32_t iPreEncodedStreamIndex;
    XONELocalPreEncodedStreamT *pCurrentStream = NULL;

    // find an empty spot
    for (iPreEncodedStreamIndex = 0; iPreEncodedStreamIndex < VOIP_MAXLOCALUSERS_EXTENDED; iPreEncodedStreamIndex += 1)
    {
        if (pHeadset->aPreEncodedStreams[iPreEncodedStreamIndex].pAudioStream == NULL)
        {
            pCurrentStream = &pHeadset->aPreEncodedStreams[iPreEncodedStreamIndex];
            break;
        }
    }

    if (pCurrentStream == NULL)
    {
        NetPrintf(("voipheadsetxboxone: no space to add pre-encoded audio streams\n"));
        return;
    }
    // stick the headset as the context of the stream
    pAudioStream->set_custom_stream_context(pHeadset);
    pCurrentStream->pAudioStream = pAudioStream;
}

/*F********************************************************************************/
/*!
    \Function _VoipHeadsetHandlePreEncodeAudioStreamClosed

    \Description
        Process stream notification game_chat_stream_state_change_type::pre_encode_audio_stream_closed

    \Input *pHeadset    - headset module
    \Input *pAudioStream - recently created pre encode audio stream

    \Version 03/04/2019 (cvienneau)
*/
/********************************************************************************F*/
static void _VoipHeadsetHandlePreEncodeAudioStreamClosed(VoipHeadsetRefT *pHeadset, pre_encode_audio_stream *pAudioStream)
{
    int32_t iPreEncodedStreamIndex = 0;
    
    // find the matching audio stream
    for (iPreEncodedStreamIndex = 0; iPreEncodedStreamIndex < VOIP_MAXLOCALUSERS_EXTENDED; iPreEncodedStreamIndex += 1)
    {
        if (pAudioStream == pHeadset->aPreEncodedStreams[iPreEncodedStreamIndex].pAudioStream)
        {
            if (pHeadset->aPreEncodedStreams[iPreEncodedStreamIndex].pTranscribeRef != NULL)
            {
                VoipTranscribeDestroy(pHeadset->aPreEncodedStreams[iPreEncodedStreamIndex].pTranscribeRef);
                pHeadset->aPreEncodedStreams[iPreEncodedStreamIndex].pTranscribeRef = NULL;
            }

            pHeadset->aPreEncodedStreams[iPreEncodedStreamIndex].uCurrentAudioBufferSize = 0;
            pHeadset->aPreEncodedStreams[iPreEncodedStreamIndex].pCurrentAudioBuffer = NULL;
            pHeadset->aPreEncodedStreams[iPreEncodedStreamIndex].pAudioStream = NULL;
            break;
        }
    }
}
/*F********************************************************************************/
/*!
    \Function _VoipHeadsetGetUserIndexofAudioStream

    \Description
        Get the DirtySDK user index of the user associated to the audio stream

    \Input *pHeadset        - headset module
    \Input *pAudioStream    - audio stream to examin
    \Input *pLocalUserIndex - [OUT] local user index of the stream

    \Output
        uint8_t             - return TRUE if the current pre encode audio stream is allow to send packets

    \Version 03/04/2019 (cvienneau)
*/
/********************************************************************************F*/
static uint8_t _VoipHeadsetGetUserIndexofAudioStream(VoipHeadsetRefT *pHeadset, pre_encode_audio_stream *pAudioStream, int32_t *pLocalUserIndex)
{
    uint8_t bSendPacket = TRUE;
    int32_t iLocalUserIndex = -1;
    uint32_t uStreamUserIndex;
    uint32_t uStreamUserCount = 0;

    *pLocalUserIndex = -1;
    xbox::services::game_chat_2::chat_user_array aStreamUsers;
    pAudioStream->get_users(&uStreamUserCount, &aStreamUsers);

    for (uStreamUserIndex = 0; uStreamUserIndex < uStreamUserCount; uStreamUserIndex += 1)
    {
        for (iLocalUserIndex = 0; iLocalUserIndex < VOIP_MAXLOCALUSERS; iLocalUserIndex += 1)
        {
            if (pHeadset->aLocalUsers[iLocalUserIndex].pChatUser != NULL)
            {
                if (wcscmp(aStreamUsers[uStreamUserIndex]->xbox_user_id(), pHeadset->aLocalUsers[iLocalUserIndex].pChatUser->xbox_user_id()) == 0)
                {
                    bSendPacket &= pHeadset->aLocalUsers[iLocalUserIndex].bParticipating;
                    bSendPacket &= !pHeadset->aLocalUsers[iLocalUserIndex].bNarrating;
                    *pLocalUserIndex = iLocalUserIndex;
                }
            }
        }
    }

    // this is a shared user if there is more than one local user associated with the stream
    if (uStreamUserCount > 1)
    {
        *pLocalUserIndex = VOIP_SHARED_USER_INDEX;
    }

    // return false if we can't find a local user index (this should be impossible)
    if (*pLocalUserIndex == -1)
    {
        NetPrintf(("voipheadsetxboxone: stream user index not found.\n"));
        return(FALSE);
    }

    return(bSendPacket);
}


/*F********************************************************************************/
/*!
    \Function _VoipHeadsetConfigureOpus

    \Description
        Configure opus for the data coming from the stream

    \Input *pStream         - the xbox audio stream

    \Output
        uint32_t            - return >= 0 on success

    \Version 03/04/2019 (cvienneau)
*/
/********************************************************************************F*/
static uint32_t _VoipHeadsetConfigureOpus(XONELocalPreEncodedStreamT *pStream)
{
    int32_t iRet = 0;
    // let opus know what the sample rate and data type of the audio, it will convert it as necessary
    if (VoipCodecControl('opus', 'insr', pStream->pAudioStream->get_pre_processed_format().sample_rate, 0, NULL) == 0)
    {
        if (pStream->pAudioStream->get_pre_processed_format().bits_per_sample == 16)
        {
            VoipCodecControl('opus', 'infl', FALSE, 0, NULL);   // restore default of int16
        }
        else if (pStream->pAudioStream->get_pre_processed_format().bits_per_sample == 32)
        {
            if (pStream->pAudioStream->get_pre_processed_format().sample_type == game_chat_sample_type::integer)
            {
                VoipCodecControl('opus', 'inin', TRUE, 0, NULL);// convert int32 to floats
            }
            else if (pStream->pAudioStream->get_pre_processed_format().sample_type == game_chat_sample_type::ieee_float)
            {
                VoipCodecControl('opus', 'infl', TRUE, 0, NULL);// use floats
            }
            else
            {
                NetPrintf(("voipheadsetxboxone: unsupported sample_type from pre-encoded stream.\n"));
                iRet = -1;
            }
        }
        else
        {
            NetPrintf(("voipheadsetxboxone: unsupported bits_per_sample(%d) from pre-encoded stream.\n", pStream->pAudioStream->get_pre_processed_format().bits_per_sample));
            iRet = -2;
        }
    }
    else
    {
        NetPrintf(("voipheadsetxboxone: cannot send mic packet because 'insr' failed.\n"));
        iRet = -3;
    }
    return(iRet);
}

/*F********************************************************************************/
/*!
    \Function _VoipHeadsetProcessPreEncodeAudio

    \Description
        React to audio generated from the mic, when used with cross play we encode the audio
        and send it to all recepients.

    \Input *pHeadset    - headset module

    \Version 03/04/2019 (cvienneau)
*/
/********************************************************************************F*/
static void _VoipHeadsetProcessPreEncodeAudio(VoipHeadsetRefT *pHeadset)
{
    uint32_t uPreEncodeAudioStreamIndex;

    for (uPreEncodeAudioStreamIndex = 0; uPreEncodeAudioStreamIndex < VOIP_MAXLOCALUSERS_EXTENDED; uPreEncodeAudioStreamIndex += 1)
    {
        VoipMicrPacketT voipMicrPacket;
        uint32_t uBytesPerSample, uBytesToSend, uSamplesToSend;
        int32_t iLocalUserIndex, iCompBytes;
        uint8_t bSendPacket;

        XONELocalPreEncodedStreamT *pStream = &(pHeadset->aPreEncodedStreams[uPreEncodeAudioStreamIndex]);
        VoipTranscribeRefT *pTranscribeRef = NULL;
        
        // if this slot isn't associated to a stream right now skip it
        if (pStream->pAudioStream == NULL)
        {
            continue;
        }

        // get the next audio buffer if we are done consuming the one we currently have
        if ((pStream->uCurrentAudioBufferConsumed >= pStream->uCurrentAudioBufferSize) || (pStream->pCurrentAudioBuffer == NULL))
        {
            if (pStream->pCurrentAudioBuffer != NULL)
            {
                pStream->pAudioStream->return_buffer(pStream->pCurrentAudioBuffer);
                pStream->pCurrentAudioBuffer = NULL;
            }

            pStream->pAudioStream->get_next_buffer(&pStream->uCurrentAudioBufferSize, (void**)&pStream->pCurrentAudioBuffer);
            pStream->uCurrentAudioBufferConsumed = 0;
        }
        
        // if there is no audio, skip futher processing
        if (pStream->pCurrentAudioBuffer == NULL)
        {
            continue;
        }

        // calculate how much raw data we would be sending
        bSendPacket = _VoipHeadsetGetUserIndexofAudioStream(pHeadset, pStream->pAudioStream, &iLocalUserIndex);
        uBytesPerSample = pStream->pAudioStream->get_pre_processed_format().bits_per_sample / 8;
        uBytesToSend = DS_MIN(pStream->uCurrentAudioBufferSize - pStream->uCurrentAudioBufferConsumed, pStream->uCurrentAudioBufferSize / VOIPHEADSET_CONSUMER_SAMPLE_RATIO);
        uSamplesToSend = uBytesToSend / uBytesPerSample;

        // if we aren't going to send the data just consume it
        if (!bSendPacket)
        {
            pStream->uCurrentAudioBufferConsumed += uBytesToSend;
            continue;
        }

        // setup opus to compress the raw data we have
        if (_VoipHeadsetConfigureOpus(pStream) != 0)
        {
            continue;
        }

        // create the transcribe ref if needed
        if (pHeadset->bVoiceTranscriptionEnabled == TRUE)
        {
            if (pStream->pTranscribeRef == NULL)
            {
                pStream->pTranscribeRef = VoipTranscribeCreate(0);
            }
            pTranscribeRef = pStream->pTranscribeRef;
        }

        // compress the buffer data, and if there is compressed data pass it to be sent to the other users
        if ((iCompBytes = VoipCodecEncode(voipMicrPacket.aData, (const int16_t*)(pStream->pCurrentAudioBuffer + pStream->uCurrentAudioBufferConsumed), uSamplesToSend, pTranscribeRef)) > 0)
        {
            pHeadset->pMicDataCb(voipMicrPacket.aData, iCompBytes, NULL, 0, iLocalUserIndex, pHeadset->uSendSeq++, pHeadset->pCbUserData);
            pStream->uCurrentAudioBufferConsumed += uBytesToSend;
        }
    }
}


/*F********************************************************************************/
/*!
    \Function _VoipHeadsetProcessStateChanges

    \Description
        Process notifications from game chat 2's chat manager

    \Input *pHeadset    - headset module

    \Version 11/22/2018 (mclouatre)
*/
/********************************************************************************F*/
static void _VoipHeadsetProcessStateChanges(VoipHeadsetRefT *pHeadset)
{
    uint32_t uStateChangeIndex;
    uint32_t uStateChangeCount;
    game_chat_state_change_array aGameChatStateChanges;
    chat_manager::singleton_instance().start_processing_state_changes(&uStateChangeCount, &aGameChatStateChanges);

    for (uStateChangeIndex = 0; uStateChangeIndex < uStateChangeCount; uStateChangeIndex += 1)
    {
        switch (aGameChatStateChanges[uStateChangeIndex]->state_change_type)
        {
            case game_chat_state_change_type::text_chat_received:
            {   
                #if DIRTYCODE_LOGGING
                const game_chat_text_chat_received_state_change *pStateChange = static_cast<const game_chat_text_chat_received_state_change*>(aGameChatStateChanges[uStateChangeIndex]);
                #endif
                NetPrintf(("voipheadsetxboxone: ignoring player-to-player text communication message from user with xuid %S\n", pStateChange->sender->xbox_user_id()));
                break;
            }

            case game_chat_state_change_type::transcribed_chat_received:
            {
                const game_chat_transcribed_chat_received_state_change *pStateChange = static_cast<const game_chat_transcribed_chat_received_state_change*>(aGameChatStateChanges[uStateChangeIndex]);
                VoipUserT VoipUser;
                char strTranscribedTextUtf8[c_maxChatTextMessageLength * sizeof(wchar_t) + 1];
                int32_t iMbStrLenNeeded;

                iMbStrLenNeeded = WideCharToMultiByte(CP_UTF8, 0, pStateChange->message, -1, NULL, 0, NULL, 0);
                if (iMbStrLenNeeded <= sizeof(strTranscribedTextUtf8))
                {
                    int32_t iUserIndex;
                    WideCharToMultiByte(CP_UTF8, 0, pStateChange->message, -1, strTranscribedTextUtf8, sizeof(strTranscribedTextUtf8), NULL, 0);
                    _VoipHeadsetGetVoipUserForChatUser(pHeadset, pStateChange->speaker, &VoipUser);

                    // see if a local user sent this text (if so record some metrics)
                    for (iUserIndex = 0; iUserIndex < VOIP_MAXLOCALUSERS; iUserIndex++)
                    {
                        int64_t iLocalPersonaId;
                        if (NetConnStatus('peid', iUserIndex, &iLocalPersonaId, sizeof(iLocalPersonaId)) == 0)
                        {
                            if (iLocalPersonaId == VoipUser.AccountInfo.iPersonaId)
                            {
                                // note on xbox we don't know; uDelay, uDurationMsSent, uEmptyResultCount, uErrorCount
                                pHeadset->aSTTMetrics[iUserIndex].uEventCount++;
                                pHeadset->aSTTMetrics[iUserIndex].uCharCountRecv += (uint32_t)strlen(strTranscribedTextUtf8);
                                break;
                            }
                        }
                    }

                    // send the text off to be displayed
                    pHeadset->pTranscribedTextReceivedCb(&VoipUser, strTranscribedTextUtf8, pHeadset->pTranscribedTextReceivedCbUserData);
                }
                else
                {
                    NetPrintf(("voipheadsetxboxone: transcribed text unexpectedly does not fit in statically allocated buffer during wcstring mbstring converstion\n"));
                }

                break;
            }

            case game_chat_state_change_type::text_conversion_preference_changed:
            {
                #if DIRTYCODE_LOGGING
                const game_chat_text_conversion_preference_changed_state_change *pStateChange = static_cast<const game_chat_text_conversion_preference_changed_state_change*>(aGameChatStateChanges[uStateChangeIndex]);
                #endif
                NetPrintf(("voipheadsetxboxone: game chat 2 notifies a 'text conversion preference' change for user with xuid %S --> STT %s TTS %s\n",
                    pStateChange->chatUser->xbox_user_id(),
                    (pStateChange->chatUser->local()->speech_to_text_conversion_preference_enabled() ? "enabled" : "disabled"),
                    (pStateChange->chatUser->local()->text_to_speech_conversion_preference_enabled() ? "enabled" : "disabled")));
                break;
            }

            case game_chat_state_change_type::communication_relationship_adjuster_changed:
            {
                game_chat_communication_relationship_adjuster CommRelAdj;
                game_chat_communication_relationship_flags EffCommRelFlags;
                int32_t iLocalUserIndex;
                const game_chat_communication_relationship_adjuster_changed_state_change *pStateChange = static_cast<const game_chat_communication_relationship_adjuster_changed_state_change*>(aGameChatStateChanges[uStateChangeIndex]);

                // set the flag that the first party mute list changed
                const uint64_t uXuid = _wtoi64(pStateChange->local_user->xbox_user_id());
                for (iLocalUserIndex = 0; iLocalUserIndex < VOIP_MAXLOCALUSERS; iLocalUserIndex += 1)
                {
                    if (pHeadset->aLocalUsers[iLocalUserIndex].uXuid == uXuid)
                    {
                        pHeadset->aLocalUsers[iLocalUserIndex].bMuteListUpdated = TRUE;
                        break;
                    }
                }

                pStateChange->local_user->local()->get_effective_communication_relationship(pStateChange->target_user, &EffCommRelFlags, &CommRelAdj);
                NetPrintf(("voipheadsetxboxone: 'comm rel adjuster' event for localXUID %S target XUID %S --> RCV_AUD:%s, RECV_TXT:%s, SND_MIC_AUD:%s, SEND_TEXT:%s, SEND_TTS_AUD:%s | commRelAdj = %s\n",
                    pStateChange->local_user->xbox_user_id(), pStateChange->target_user->xbox_user_id(),
                    ((EffCommRelFlags & game_chat_communication_relationship_flags::receive_audio) == game_chat_communication_relationship_flags::none) ? "n" : "y",
                    ((EffCommRelFlags & game_chat_communication_relationship_flags::receive_text) == game_chat_communication_relationship_flags::none) ? "n" : "y",
                    ((EffCommRelFlags & game_chat_communication_relationship_flags::send_microphone_audio) == game_chat_communication_relationship_flags::none) ? "n" : "y",
                    ((EffCommRelFlags & game_chat_communication_relationship_flags::send_text) == game_chat_communication_relationship_flags::none) ? "n" : "y",
                    ((EffCommRelFlags & game_chat_communication_relationship_flags::send_text_to_speech_audio) == game_chat_communication_relationship_flags::none) ? "n" : "y",
                    _VoipHeadsetGetCommRelAdjusterString(CommRelAdj)));
                break;
            }

            default:
                break;
        }

    }

    chat_manager::singleton_instance().finish_processing_state_changes(aGameChatStateChanges);
}

/*F********************************************************************************/
/*!
    \Function _VoipHeadsetProcessStreamStateChanges

    \Description
        Process stream notifications from game chat 2's chat manager

    \Input *pHeadset    - headset module

    \Version 01/30/2019 (tcho)
*/
/********************************************************************************F*/
static void _VoipHeadsetProcessStreamStateChanges(VoipHeadsetRefT *pHeadset)
{
    uint32_t uStreamStateChangeIndex;
    uint32_t uStreamStateChangeCount;
    game_chat_stream_state_change_array aStreamStateChanges;

    chat_manager::singleton_instance().start_processing_stream_state_changes(&uStreamStateChangeCount, &aStreamStateChanges);

    for (uStreamStateChangeIndex = 0; uStreamStateChangeIndex < uStreamStateChangeCount; uStreamStateChangeIndex += 1)
    {
        switch (aStreamStateChanges[uStreamStateChangeIndex]->state_change_type)
        {
            case game_chat_stream_state_change_type::post_decode_audio_sink_stream_created:
            {
                post_decode_audio_sink_stream *pSinkStream = aStreamStateChanges[uStreamStateChangeIndex]->post_decode_audio_sink_stream;
                _VoipHeadsetHandlePostDecodeSinkStreamCreated(pHeadset, pSinkStream);
                break;
            }

            case game_chat_stream_state_change_type::post_decode_audio_sink_stream_closed:
            {
                post_decode_audio_sink_stream *pSinkStream = aStreamStateChanges[uStreamStateChangeIndex]->post_decode_audio_sink_stream;
                _VoipHeadsetHandlePostDecodeSinkStreamClosed(pHeadset, pSinkStream);
                break;
            }
            
            case game_chat_stream_state_change_type::pre_encode_audio_stream_created:
            {
                pre_encode_audio_stream *pAudioStream = aStreamStateChanges[uStreamStateChangeIndex]->pre_encode_audio_stream;
                _VoipHeadsetHandlePreEncodeAudioStreamCreated(pHeadset, pAudioStream);
                break;
            }

            case game_chat_stream_state_change_type::pre_encode_audio_stream_closed:
            {
                pre_encode_audio_stream *pAudioStream = aStreamStateChanges[uStreamStateChangeIndex]->pre_encode_audio_stream;
                _VoipHeadsetHandlePreEncodeAudioStreamClosed(pHeadset, pAudioStream);
                break;
            }

            default:
                break;
        }
    }

    chat_manager::singleton_instance().finish_processing_stream_state_changes(aStreamStateChanges);
}


/*F********************************************************************************/
/*!
    \Function _VoipHeadsetApplyEffectiveRelationship

    \Description
        Set local player to remote player relationship taking into account the 
        per-connection muting (aRelFromMuting) and the channel config (aRelFromChannels).

    \Input *pHeadset            - pointer to headset state
    \Input iLocalUserIndex      - index of local user
    \Input *pRemoteChatUser     - remote user

    \Version 12/18/2018 (mclouatre)
*/
/********************************************************************************F*/
static void _VoipHeadsetApplyEffectiveRelationship(VoipHeadsetRefT *pHeadset, int32_t iLocalUserIndex, chat_user *pRemoteChatUser)
{
    game_chat_communication_relationship_flags EffCommRelFlags;
    XONERemoteVoipUserT *pXoneRemoteUser = (XONERemoteVoipUserT *)pRemoteChatUser->custom_user_context();

    EffCommRelFlags = pXoneRemoteUser->aRelFromMuting[iLocalUserIndex] & pXoneRemoteUser->aRelFromChannels[iLocalUserIndex];

    pHeadset->aLocalUsers[iLocalUserIndex].pChatUser->local()->set_communication_relationship(pRemoteChatUser, EffCommRelFlags);

    NetPrintf(("voipheadsetxboxone: applying new comm rel for localXUID %S target XUID %S --> RCV_AUD:%s, RECV_TXT:%s, SND_MIC_AUD:%s, SEND_TEXT:%s, SEND_TTS_AUD:%s\n",
        pHeadset->aLocalUsers[iLocalUserIndex].pChatUser->xbox_user_id(), pRemoteChatUser->xbox_user_id(),
        ((EffCommRelFlags & game_chat_communication_relationship_flags::receive_audio) == game_chat_communication_relationship_flags::none) ? "n" : "y",
        ((EffCommRelFlags & game_chat_communication_relationship_flags::receive_text) == game_chat_communication_relationship_flags::none) ? "n" : "y",
        ((EffCommRelFlags & game_chat_communication_relationship_flags::send_microphone_audio) == game_chat_communication_relationship_flags::none) ? "n" : "y",
        ((EffCommRelFlags & game_chat_communication_relationship_flags::send_text) == game_chat_communication_relationship_flags::none) ? "n" : "y",
        ((EffCommRelFlags & game_chat_communication_relationship_flags::send_text_to_speech_audio) == game_chat_communication_relationship_flags::none) ? "n" : "y"));

    return;
}

/*F********************************************************************************/
/*!
    \Function _VoipHeadsetSetRemoteUserVoicePlayback

    \Description
        Helper function to enable or disable playback of voice from a remote user
        for a given local user.

    \Input *pHeadset            - pointer to headset state
    \Input *pRemoteUser         - remote user
    \Input iLocalUserIndex      - local user index
    \Input bEnablePlayback      - TRUE to enable voice playback. FALSE to disable voice playback.

    \Output
        int32_t                 - negative=error, zero=success 

    \Version 12/10/2014 (tcho)
*/
/********************************************************************************F*/
static int32_t _VoipHeadsetSetRemoteUserVoicePlayback(VoipHeadsetRefT *pHeadset, VoipUserT *pRemoteUser, int32_t iLocalUserIndex, uint8_t bEnablePlayback)
{
    chat_user *pLocalChatUser = pHeadset->aLocalUsers[iLocalUserIndex].pChatUser;
    chat_user *pRemoteChatUser = _VoipHeadsetGetChatUserForVoipUser(pHeadset, pRemoteUser);

    if (pLocalChatUser != NULL)
    {
        int32_t iRemoteUserIndex;

        // if the remote is an xboxone user want to apply setting to game chat 2
        if (pRemoteChatUser != NULL)
        {
            XONERemoteVoipUserT *pXoneRemoteUser = (XONERemoteVoipUserT *)pRemoteChatUser->custom_user_context();
            NetPrintf(("voipheadsetxboxone: %s localXUID %S remoteXUID %S\n", (bEnablePlayback ? "+pbk" : "-pbk"), pLocalChatUser->xbox_user_id(), pRemoteChatUser->xbox_user_id()));
            pXoneRemoteUser->aRelFromChannels[iLocalUserIndex] = (bEnablePlayback ? c_communicationRelationshipSendAndReceiveAll : c_communicationRelationshipSendAll);
            _VoipHeadsetApplyEffectiveRelationship(pHeadset, iLocalUserIndex, pRemoteChatUser);
        }

        // find the remote user index
        iRemoteUserIndex = _VoipHeadsetGetRemoteUserIndex(pHeadset, pRemoteUser);

        if (iRemoteUserIndex < 0)
        {
            NetPrintf(("voipheadsetxboxone: failed to change voice playback for remote user can't be found (pLocalChatUser = %p, pRemoteChatUser = %p)\n",
                pLocalChatUser, pRemoteChatUser));
            return(-1);
        }

        if (bEnablePlayback)
        {
            pHeadset->aLocalUsers[iLocalUserIndex].uPlaybackFlags |= (1 << iRemoteUserIndex);
        }
        else
        {
            pHeadset->aLocalUsers[iLocalUserIndex].uPlaybackFlags &= ~(1 << iRemoteUserIndex);
        }
        NetPrintf(("voipheadsetxboxone: local user[%d] playback flags now: %x.\n", iLocalUserIndex, pHeadset->aLocalUsers[iLocalUserIndex].uPlaybackFlags));
    }
    else
    {
        NetPrintf(("voipheadsetxboxone: failed to change voice playback for local user index %d because chat users can't be found (pLocalChatUser = %p, pRemoteChatUser = %p)\n",
            pLocalChatUser, pRemoteChatUser));
        return(-2);
    }

    return(0);
}

/*F********************************************************************************/
/*!
    \Function _VoipHeadsetSetRemoteUserOutboundMute

    \Description
        Helper function to enable or disable outbound mute for a pair of local
        user and remote user.

    \Input *pHeadset            - pointer to headset state
    \Input *pRemoteUser         - remote user
    \Input iLocalUserIndex      - local user index
    \Input bSend                - TRUE to disable outbound mute. FALSE to enable outbound mute.

    \Output
        int32_t                 - negative=error, zero=success 

    \Version 12/18/2018 (mclouatre)
*/
/********************************************************************************F*/
static int32_t _VoipHeadsetSetRemoteUserOutboundMute(VoipHeadsetRefT *pHeadset, VoipUserT *pRemoteUser, int32_t iLocalUserIndex, uint8_t bSend)
{
    chat_user *pLocalChatUser = pHeadset->aLocalUsers[iLocalUserIndex].pChatUser;
    chat_user *pRemoteChatUser = _VoipHeadsetGetChatUserForVoipUser(pHeadset, pRemoteUser);

    if ((pLocalChatUser != NULL) && (pRemoteChatUser != NULL))
    {
        XONERemoteVoipUserT *pXoneRemoteUser = (XONERemoteVoipUserT *)pRemoteChatUser->custom_user_context();

        NetPrintf(("voipheadsetxboxone: %s localXUID %S remoteXUID %S\n", (bSend ? "+snd" : "-snd"), pLocalChatUser->xbox_user_id(), pRemoteChatUser->xbox_user_id()));

        if (pXoneRemoteUser != NULL)
        {
            if (bSend)
            {
                pXoneRemoteUser->aRelFromMuting[iLocalUserIndex] = pXoneRemoteUser->aRelFromMuting[iLocalUserIndex] | c_communicationRelationshipSendAll;
            }
            else
            {
                pXoneRemoteUser->aRelFromMuting[iLocalUserIndex] = pXoneRemoteUser->aRelFromMuting[iLocalUserIndex] & (game_chat_communication_relationship_flags)(~(uint32_t)c_communicationRelationshipSendAll);
            }

          _VoipHeadsetApplyEffectiveRelationship(pHeadset, iLocalUserIndex, pRemoteChatUser);
        }
        else
        {
            NetPrintf(("voipheadsetxboxone: failed to change outbound mute for local user index %d because remote user can’t be found\n", iLocalUserIndex));
            return(-1);
        }
    }
    else
    {
        NetPrintf(("voipheadsetxboxone: failed to change outbound mute for local user index %d because chat users can't be found (pLocalChatUser = %p, pRemoteChatUser = %p)\n",
            iLocalUserIndex, pLocalChatUser, pRemoteChatUser));
        return(-2);
    }

    return(0);
}

/*F********************************************************************************/
/*!
    \Function _VoipHeadsetSetRemoteUserInboundMute

    \Description
        Helper function to enable or disable inbound mute for a pair of local
        user and remote user.

    \Input *pHeadset            - pointer to headset state
    \Input *pRemoteUser         - remote user
    \Input iLocalUserIndex      - local user index
    \Input bRecv                - TRUE to disable inbound mute. FALSE to enable inbbound mute.

    \Output
        int32_t                 - negative=error, zero=success 

    \Version 12/18/2018 (mclouatre)
*/
/********************************************************************************F*/
static int32_t _VoipHeadsetSetRemoteUserInboundMute(VoipHeadsetRefT *pHeadset, VoipUserT *pRemoteUser, int32_t iLocalUserIndex, uint8_t bRecv)
{
    chat_user *pLocalChatUser = pHeadset->aLocalUsers[iLocalUserIndex].pChatUser;
    chat_user *pRemoteChatUser = _VoipHeadsetGetChatUserForVoipUser(pHeadset, pRemoteUser);

    if ((pLocalChatUser != NULL) && (pRemoteChatUser != NULL))
    {
        XONERemoteVoipUserT *pXoneRemoteUser = (XONERemoteVoipUserT *)pRemoteChatUser->custom_user_context();

        NetPrintf(("voipheadsetxboxone: %s localXUID %S remoteXUID %S\n", (bRecv ? "+rcv" : "-rcv"), pLocalChatUser->xbox_user_id(), pRemoteChatUser->xbox_user_id()));

        if (pXoneRemoteUser != NULL)
        {
            if (bRecv)
            {
                pXoneRemoteUser->aRelFromMuting[iLocalUserIndex] = pXoneRemoteUser->aRelFromMuting[iLocalUserIndex] | c_communicationRelationshipReceiveAll;
            }
            else
            {
                pXoneRemoteUser->aRelFromMuting[iLocalUserIndex] = pXoneRemoteUser->aRelFromMuting[iLocalUserIndex] & (game_chat_communication_relationship_flags)(~(uint32_t)c_communicationRelationshipReceiveAll);
            }

            _VoipHeadsetApplyEffectiveRelationship(pHeadset, iLocalUserIndex, pRemoteChatUser);
        }
        else
        {
            NetPrintf(("voipheadsetxboxone: failed to change inbound mute for local user index %d because remote user can’t be found\n", iLocalUserIndex));
            return(-1);
        }
    }
    else
    {
        NetPrintf(("voipheadsetxboxone: failed to change inbound mute for local user index %d because chat users can't be found (pLocalChatUser = %p, pRemoteChatUser = %p)\n",
            iLocalUserIndex, pLocalChatUser, pRemoteChatUser));
        return(-2);
    }

    return(0);
}

/*F********************************************************************************/
/*!
    \Function _VoipHeadsetProcessPlayback

    \Description
        Process playback of data received from the network to the headset earpiece under crossplay.

    \Input *pHeadset        - pointer to headset state

    \Version 29/01/2019 (tcho)
*/
/********************************************************************************F*/
static void _VoipHeadsetProcessPlayback(VoipHeadsetRefT *pHeadset)
{
    uint8_t FrameData[VOIPHEADSET_FRAMESIZE];
    int32_t iSampleBytes;
    int32_t iMixStreamIndex;

    for (iMixStreamIndex = 0; iMixStreamIndex < VOIP_MAXLOCALUSERS_EXTENDED; iMixStreamIndex += 1)
    {
        if ((pHeadset->aLocalMixerStreams[iMixStreamIndex].pMixer != NULL) && (pHeadset->aLocalMixerStreams[iMixStreamIndex].pSinkStream != NULL))
        {
            if ((iSampleBytes = VoipMixerProcess(pHeadset->aLocalMixerStreams[iMixStreamIndex].pMixer, FrameData)) == VOIPHEADSET_FRAMESIZE)
            {
                pHeadset->aLocalMixerStreams[iMixStreamIndex].pSinkStream->submit_mixed_buffer((uint32_t)iSampleBytes, FrameData);
            }
        }
    }
}

/*F********************************************************************************/
/*!
    \Function _VoipHeadsetProcessNarrate

    \Description
        Process voice narration

    \Input pHeadset     - headset ref

    \Version 04/09/2019 (tcho)
*/
/********************************************************************************F*/
static void _VoipHeadsetProcessNarrate(VoipHeadsetRefT *pHeadset)
{
    int32_t iLocalUserIndex;

    for (iLocalUserIndex = 0; iLocalUserIndex < VOIP_MAXLOCALUSERS; iLocalUserIndex += 1)
    {
        XONELocalVoipUserT *pLocalUser = &pHeadset->aLocalUsers[iLocalUserIndex];

        // process narrate
        if (pLocalUser->pNarrateRef != NULL)
        {
            VoipNarrateUpdate(pLocalUser->pNarrateRef);
        }
        if (pLocalUser->pSourceVoice != NULL)
        {
            if (pHeadset->bLoopback)
            {
                _VoipHeadsetProcessLoopback(pHeadset, pLocalUser);
            }
        }
    }
}
/*F********************************************************************************/
/*!
    \Function _VoipHeadsetProcessTranscribe

    \Description
        Process voice transcription

    \Input pHeadset     - headset ref

    \Version 04/09/2019 (tcho)
*/
/********************************************************************************F*/
static void _VoipHeadsetProcessTranscribe(VoipHeadsetRefT *pHeadset)
{
    int32_t iPreEncodedStreamIndex;
    char strTranscribeBuf[VOIPTRANSCRIBE_OUTPUT_MAX];

    // loop through all the PreEncoded Streams
    for (iPreEncodedStreamIndex = 0; iPreEncodedStreamIndex < VOIP_MAXLOCALUSERS; iPreEncodedStreamIndex += 1)
    {
        uint8_t bPreEncodeParticipating = FALSE;
        int32_t iParticipatingIndex = -1;
        uint32_t uUserCount = 0;
        chat_user_array aUsers;

        // make sure we have a valid audio stream
        if (pHeadset->aPreEncodedStreams[iPreEncodedStreamIndex].pAudioStream != NULL)
        {
            uint32_t uUserIndex;
            pHeadset->aPreEncodedStreams[iPreEncodedStreamIndex].pAudioStream->get_users(&uUserCount, &aUsers);
            
            // loop through all the users associated with the audio stream
            // a pre-encoded stream is consider participating if even on of its users is participating
            for (uUserIndex = 0; uUserIndex < uUserCount; uUserIndex += 1)
            {      
                int32_t iLocalUserIndex = NetConnStatus('ixid', 0, (void *)aUsers[uUserIndex]->xbox_user_id(), 0);               
                bPreEncodeParticipating |= pHeadset->aLocalUsers[iLocalUserIndex].bParticipating;

                // the index of the first participating user will be user for sending any transcriptions
                if ((iParticipatingIndex == -1) && (pHeadset->aLocalUsers[iLocalUserIndex].bParticipating == TRUE))
                {
                    iParticipatingIndex = iLocalUserIndex;
                }
            }
        }

        // process transcriptions
        if ((bPreEncodeParticipating == TRUE) && (pHeadset->aPreEncodedStreams[iPreEncodedStreamIndex].pTranscribeRef != NULL))
        {
            VoipTranscribeUpdate(pHeadset->aPreEncodedStreams[iPreEncodedStreamIndex].pTranscribeRef);

            if (VoipTranscribeGet(pHeadset->aPreEncodedStreams[iPreEncodedStreamIndex].pTranscribeRef, strTranscribeBuf, sizeof(strTranscribeBuf)) > 0)
            {
                pHeadset->pTextDataCb(strTranscribeBuf, iParticipatingIndex, pHeadset->pCbUserData);
            }
        }
    }
}

/*F********************************************************************************/
/*!
    \Function _VoipHeadsetAllocateMemory

    \Description
        Memory allocator registered with game chat 2.

    \Input Size             - the size of the allocation to be made. This value will never be zero.
    \Input uMemoryTypeId    - opaque identifier representing the Game Chat 2 internal category of memory being allocated.

    \Output
        void *              - NULL if failure, pointer to newly allocated memory if success

    \Version 29/11/2018 (mclouatre)
*/
/********************************************************************************F*/
static void * _VoipHeadsetAllocateMemory(size_t Size, uint32_t uMemoryTypeId)
{
    void *pAllocatedMemory;

    // allocate requested memory
    if ((pAllocatedMemory = DirtyMemAlloc((int32_t)Size, VOIP_PLATFORM_MEMID, VoipHeadsetXboxone_iMemGroup, VoipHeadsetXboxone_pMemGroupUserData)) == NULL)
    {
        NetPrintf(("voipheadsetxboxone: failed to allocate a buffer of size %s with gamechat2 memory type %s\n", Size, uMemoryTypeId));
    }

    // return the pointer passed the spot reserved to store the memory type id.
    return(pAllocatedMemory);
}


/*F********************************************************************************/
/*!
    \Function _VoipHeadsetFreeMemory

    \Description
        Memory allocator registered with game chat 2.

    \Input *pMemory         - pointer to the memory buffer previously allocated. This value will never be a null pointer
    \Input uMemoryTypeId    - opaque identifier representing the Game chat internal category of memory being freed

    \Version 29/11/2018 (mclouatre)
*/
/********************************************************************************F*/
static void _VoipHeadsetFreeMemory(void *pMemory, uint32_t uMemoryTypeId)
{
    DirtyMemFree(pMemory, VOIP_PLATFORM_MEMID, VoipHeadsetXboxone_iMemGroup, VoipHeadsetXboxone_pMemGroupUserData);
}

/*F********************************************************************************/
/*!
    \Function _VoipHeadsetInitializeGameChat2

    \Description
        Initialize game chat 2

    \Input *pHeadset        - pointer to headset state
    \Input bCrossplay       - TRUE for cross play, FALSE for native mode

    \Version 29/01/2019 (tcho)
*/
/********************************************************************************F*/
static void _VoipHeadsetInitializeGameChat2(VoipHeadsetRefT *pHeadset, int8_t bCrossplay)
{
    // applying custom game chat 2 thread affinity if specified
    if (pHeadset->iThreadAffinity != 0)
    {
        chat_manager::set_thread_processor(game_chat_thread_id::audio, pHeadset->iThreadAffinity & 0xFFFF);
        chat_manager::set_thread_processor(game_chat_thread_id::networking, pHeadset->iThreadAffinity >> 16);
    }

    chat_manager::set_memory_callbacks(
        (game_chat_allocate_memory_callback)_VoipHeadsetAllocateMemory,
        (game_chat_free_memory_callback)_VoipHeadsetFreeMemory);

    chat_manager::singleton_instance().initialize(
        pHeadset->iMaxChannels,
        1.0F,
        c_communicationRelationshipSendAndReceiveAll,
        game_chat_shared_device_communication_relationship_resolution_mode::restrictive,    // aligns with DS default shared channel config behavior
        game_chat_speech_to_text_conversion_mode::automatic,
        bCrossplay ? (game_chat_audio_manipulation_mode_flags::post_decode_stream_manipulation | game_chat_audio_manipulation_mode_flags::pre_encode_stream_manipulation) : game_chat_audio_manipulation_mode_flags::none);

    // set the default bitrate to 'medium/normal' quality
    #if defined(DIRTYCODE_GDK)
    chat_manager::singleton_instance().set_audio_encoding_bitrate(game_chat_audio_encoding_bitrate::kilobits_per_second_16);
    #else
    chat_manager::singleton_instance().set_audio_encoding_type_and_bitrate(game_chat_audio_encoding_type_and_bitrate::silk_16_kilobits_per_second);
    #endif
}

/*F********************************************************************************/
/*!
    \Function   _VoipHeadsetCleanupCrossplayUserResources

    \Description
        Helper function for destroying state associated to a user in crossplay

    \Input *pHeadset    - pointer to headset state

    \Version 04/25/2019 (cvienneau)
*/
/********************************************************************************F*/
static void _VoipHeadsetCleanupCrossplayUserResources(VoipHeadsetRefT *pHeadset)
{
    int32_t iUserIndex;

    // release all user resources
    for (iUserIndex = 0; iUserIndex < VOIP_MAXLOCALUSERS; iUserIndex += 1)
    {
        XONELocalVoipUserT *pLocalUser = &pHeadset->aLocalUsers[iUserIndex];
        XONELocalMixerStreamT *pMixerStream = &pHeadset->aLocalMixerStreams[iUserIndex];
        XONELocalPreEncodedStreamT *pPreEncodeStream = &pHeadset->aPreEncodedStreams[iUserIndex];

        // mixer module
        if (pMixerStream->pMixer != NULL)
        {
            if (pHeadset->pConduitRef != NULL)
            {
                VoipConduitMixerUnset(pHeadset->pConduitRef, pMixerStream->pMixer);
            }
            VoipMixerDestroy(pMixerStream->pMixer);
            pMixerStream->pMixer = NULL;
        }

        // transcribe module
        if (pPreEncodeStream->pTranscribeRef != NULL)
        {
            VoipTranscribeDestroy(pPreEncodeStream->pTranscribeRef);
            pPreEncodeStream->pTranscribeRef = NULL;
        }

        // narrate module
        if (pLocalUser->pNarrateRef != NULL)
        {
            VoipNarrateDestroy(pLocalUser->pNarrateRef);
            pLocalUser->pNarrateRef = NULL;
        }

        // xaudio modules
        if (pLocalUser->pSourceVoice != NULL)
        {
            pLocalUser->pSourceVoice->DestroyVoice();
            pLocalUser->pSourceVoice = NULL;
        }
        if (pLocalUser->pMasteringVoice != NULL)
        {
            pLocalUser->pMasteringVoice->DestroyVoice();
            pLocalUser->pMasteringVoice = NULL;
        }
        pLocalUser->pXaudio2 = nullptr;
    }
    
    // clear all the arrays so they can be added to freshly in the future
    ds_memclr(pHeadset->aPreEncodedStreams, sizeof(pHeadset->aPreEncodedStreams));
    ds_memclr(pHeadset->aLocalMixerStreams, sizeof(pHeadset->aLocalMixerStreams));
    ds_memclr(pHeadset->aLocalUsers, sizeof(pHeadset->aLocalUsers));
}

/*F********************************************************************************/
/*!
    \Function _VoipHeadsetToggleCrossplayMode

    \Description
        Enable or disable crossplay mode.

    \Input *pHeadset        - pointer to headset state
    \Input bCrossplay       - TRUE if we will be supporting crossplay
    
    \Output
        int32_t             - negative=error, zero=success

    \Version 29/01/2018 (tcho)
*/
/********************************************************************************F*/
static int32_t _VoipHeadsetToggleCrossplayMode(VoipHeadsetRefT *pHeadset, uint8_t bCrossplay)
{
    int32_t  iUserIndex = 0;
    uint32_t uChatUserCount;
    uint32_t uChatUserIndex;
    uint32_t uPreEncodeAudioStreamIndex;
    VoipUserT aLocalVoipUsers[VOIP_MAXLOCALUSERS];
    uint8_t   aLocalVoipUsersParticipating[VOIP_MAXLOCALUSERS];
    chat_user_array aChatUsers;

    chat_manager::singleton_instance().get_chat_users(&uChatUserCount, &aChatUsers);
    ds_memclr(aLocalVoipUsers, sizeof(aLocalVoipUsers));
    ds_memclr(aLocalVoipUsersParticipating, sizeof(aLocalVoipUsersParticipating));

    // process the list of current users
    for (uChatUserIndex = 0; uChatUserIndex < uChatUserCount; uChatUserIndex += 1)
    {
        //chat_user::chat_user_local *pLocalUser = aChatUsers[uChatUserIndex]->local();

        // all users
        chat_manager::singleton_instance().remove_user(aChatUsers[uChatUserIndex]);
    }

    // return lingering pre-encoded audio buffers to game chat
    for (uPreEncodeAudioStreamIndex = 0; uPreEncodeAudioStreamIndex < VOIP_MAXLOCALUSERS_EXTENDED; uPreEncodeAudioStreamIndex += 1)
    {
        XONELocalPreEncodedStreamT *pStream = &(pHeadset->aPreEncodedStreams[uPreEncodeAudioStreamIndex]);

        if ((pStream->pAudioStream != NULL) && (pStream->pCurrentAudioBuffer != NULL))
        {
            pStream->pAudioStream->return_buffer(pStream->pCurrentAudioBuffer);
            pStream->pCurrentAudioBuffer = NULL;
        }
    }

    // cleanup game chat 2
    chat_manager::singleton_instance().cleanup();

    // save down all the local users
    for (iUserIndex = 0; iUserIndex < VOIP_MAXLOCALUSERS; iUserIndex += 1)
    {
        if (!VOIP_NullUser(&pHeadset->aLocalUsers[iUserIndex].User))
        {
            ds_memcpy(&aLocalVoipUsers[iUserIndex], &pHeadset->aLocalUsers[iUserIndex].User, sizeof(VoipUserT));
            aLocalVoipUsersParticipating[iUserIndex] = pHeadset->aLocalUsers[iUserIndex].bParticipating;
        }
    }

    // purge the users crossplay state
    _VoipHeadsetCleanupCrossplayUserResources(pHeadset);
    // re-initialize game chat 2
    _VoipHeadsetInitializeGameChat2(pHeadset, bCrossplay);


    // re add all the local voip users to game chat 2
    for (iUserIndex = 0; iUserIndex < VOIP_MAXLOCALUSERS; iUserIndex += 1)
    {
        if (!VOIP_NullUser(&aLocalVoipUsers[iUserIndex]))
        {
            VoipHeadsetControl(pHeadset, 'rloc', iUserIndex, TRUE, &aLocalVoipUsers[iUserIndex]);

            if (aLocalVoipUsersParticipating[iUserIndex] == TRUE)
            {
                VoipHeadsetControl(pHeadset, 'aloc', iUserIndex, TRUE, &aLocalVoipUsers[iUserIndex]);
            }

            NetPrintf(("voipheadsetxboxone: re-adding local talker %lld\n", aLocalVoipUsers[iUserIndex].AccountInfo.iPersonaId));
        }
    }

    // loop through remote users
    for (iUserIndex = 0; iUserIndex < pHeadset->iMaxRemoteUsers; iUserIndex += 1)
    {
        XONERemoteVoipUserT *pRemoteUser = &pHeadset->aRemoteUsers[iUserIndex];

        // we will only need to re add x1 remote players
        // because from native -> crossplay we should only have x1 remote players
        //         from crossplay -> native we should only keep x1 remote players
        if (!VOIP_NullUser(&pRemoteUser->User) && (pRemoteUser->User.ePlatform == VOIP_PLATFORM_XBOXONE))
        {
            XONERemoteVoipUserT TempRemoteUser;
            int32_t iRemoteUserIndex;
            int32_t iLocalUserIndex;

            // save down the remote user and remove him
            ds_memcpy(&TempRemoteUser, pRemoteUser, sizeof(XONERemoteVoipUserT));
            ds_memclr(&pHeadset->aRemoteUsers[iUserIndex], sizeof(XONERemoteVoipUserT));
            
            VoipConduitRegisterUser(pHeadset->pConduitRef, &TempRemoteUser.User, FALSE);
            iRemoteUserIndex = _VoipHeadsetAddRemoteTalker(pHeadset, &TempRemoteUser.User, TempRemoteUser.uConsoleId);

            // re-apply communication flags
            ds_memcpy(pHeadset->aRemoteUsers[iRemoteUserIndex].aRelFromChannels, TempRemoteUser.aRelFromChannels, sizeof(TempRemoteUser.aRelFromChannels));
            ds_memcpy(pHeadset->aRemoteUsers[iRemoteUserIndex].aRelFromMuting, TempRemoteUser.aRelFromMuting, sizeof(TempRemoteUser.aRelFromMuting));

            for (iLocalUserIndex = 0; iLocalUserIndex < VOIP_MAXLOCALUSERS; iLocalUserIndex += 1)
            {
                if (!VOIP_NullUser(&pHeadset->aLocalUsers[iLocalUserIndex].User))
                {
                    _VoipHeadsetApplyEffectiveRelationship(pHeadset, iLocalUserIndex, pHeadset->aRemoteUsers[iRemoteUserIndex].pChatUser);
                }
            }
        }
    }
    return(0);
}

/*F********************************************************************************/
/*!
    \Function _VoipHeadsetNarrateWriteAudio

    \Description
        Write to narrate audio to our audio buffer

    \Input *pLocalUser  - the user that the audio belongs to
    \Input *pBuffer     - audio data we are writing
    \Input iBufLen      - amount of audio data

    \Output
        int32_t         - amount of data written

    \Version 03/15/2019 (eesponda)
*/
/********************************************************************************F*/
static int32_t _VoipHeadsetNarrateWriteAudio(XONELocalVoipUserT *pLocalUser, const uint8_t *pBuffer, int32_t iBufLen)
{
    if ((iBufLen = DS_MIN((signed)sizeof(pLocalUser->aAudioData) - pLocalUser->iAudioLen, iBufLen)) > 0)
    {
        ds_memcpy(pLocalUser->aAudioData+pLocalUser->iAudioLen, pBuffer, iBufLen);
        pLocalUser->iAudioLen += iBufLen;
    }
    return(iBufLen);
}

/*F********************************************************************************/
/*!
    \Function _VoipHeadsetNarrateLoopbackCb

    \Description
        Helper function for narrate callback from the narrate module for the loopback
    
    \Input *pHeadset    - headset ref
    \Input iUserIndex   - local user index
    \Input *pSamples    - audio samples
    \Input iSize        - amount of audio samples

    \Output
        int32_t         - amount of data consumed

    \Version 06/25/2019 (cvienneau)
*/
/********************************************************************************F*/
static int32_t _VoipHeadsetNarrateLoopbackCb(VoipHeadsetRefT *pHeadset, int32_t iUserIndex, const int16_t *pSamples, int32_t iSize)
{
    XONELocalVoipUserT *pLocalUser = &pHeadset->aLocalUsers[iUserIndex];

    // start the playback
    if (iSize == VOIPNARRATE_STREAM_START)
    {
        // if we are currently stopping, just don't stop and don't start since we are already started
        if (pLocalUser->bStopping)
        {
            pLocalUser->bStopping = FALSE;
        }
        else
        {
            pLocalUser->pSourceVoice->Start(0, 0);
        }
    }
    // queue the end of the playback
    else if (iSize == VOIPNARRATE_STREAM_END)
    {
        pLocalUser->bStopping = TRUE;
    }
    // if we have no buffers queue start writing data again
    else
    {
        XAUDIO2_VOICE_STATE State;
        pLocalUser->pSourceVoice->GetState(&State);
        iSize = (State.BuffersQueued == 0) ? _VoipHeadsetNarrateWriteAudio(pLocalUser, (const uint8_t *)pSamples, iSize) : 0;
    }
    return(iSize);
}
/*F********************************************************************************/
/*!
    \Function _VoipHeadsetNarrateCrossplayCb

    \Description
        Helper function for narrate callback from the narrate module for crossplay
    
    \Input *pHeadset    - headset ref
    \Input iUserIndex   - local user index
    \Input *pSamples    - audio samples
    \Input iSize        - amount of audio samples

    \Output
        int32_t         - amount of data consumed

    \Version 03/15/2019 (eesponda)
*/
/********************************************************************************F*/
static int32_t _VoipHeadsetNarrateCrossplayCb(VoipHeadsetRefT *pHeadset, int32_t iUserIndex, const int16_t *pSamples, int32_t iSize)
{
    XONELocalVoipUserT *pLocalUser = &pHeadset->aLocalUsers[iUserIndex];
    VoipMicrPacketT voipMicrPacket;
    VoipTranscribeRefT *pTranscribeRef = NULL;
    int32_t iCompBytes = 0;

    // start the playback
    if (iSize == VOIPNARRATE_STREAM_START)
    {
        pLocalUser->bNarrating = TRUE;
    }
    // queue the end of the playback
    else if (iSize == VOIPNARRATE_STREAM_END)
    {
        pLocalUser->bNarrateCbExit = FALSE;
        pLocalUser->bNarrating = FALSE;
    }
    else
    {
        const int16_t *pPreEncodeSamples = pSamples;

        // we will exit if we have already sent a 20ms packet
        if (pLocalUser->bNarrateCbExit == TRUE)
        {
            pLocalUser->bNarrateCbExit = FALSE;
            iSize = 0;
        }

        // if the the incoming samples is smaller than the framesize pad it with 0s
        if (iSize < VOIPHEADSET_FRAMESIZE)
        {
            pLocalUser->iAudioLen = 0;
            ds_memclr(pLocalUser->aAudioData, VOIPHEADSET_FRAMESIZE);
            _VoipHeadsetNarrateWriteAudio(pLocalUser, (const uint8_t *)pSamples, iSize);
            pPreEncodeSamples = (const int16_t *)pLocalUser->aAudioData;
        }

        // the assumption here is every time voipnarratexboxone will try to give use exactly one frame size of data 
        if (iSize == VOIPHEADSET_FRAMESIZE)
        {
            // create transcribe module if needed
            if ((pHeadset->bVoiceTranscriptionEnabled == TRUE) && (pHeadset->aLocalUsers[iUserIndex].bParticipating == TRUE))
            {
                pTranscribeRef = _VoipHeadsetTranscibeCreate(pHeadset, iUserIndex);
            }

            // let opus know what the sample rate and data type of the audio, it will convert it as necessary
            if (VoipCodecControl('opus', 'insr', VOIPHEADSET_SAMPLERATE, 0, NULL) == 0)
            {
                VoipCodecControl('opus', 'infl', FALSE, 0, NULL);
                if ((iCompBytes = VoipCodecEncode(voipMicrPacket.aData, pPreEncodeSamples, VOIPHEADSET_FRAMESAMPLES, pTranscribeRef)) > 0)
                {
                    pHeadset->pMicDataCb(voipMicrPacket.aData, iCompBytes, NULL, 0, iUserIndex, pHeadset->uSendSeq++, pHeadset->pCbUserData);
                }
            }
            else
            {
                NetPrintf(("voipheadsetxboxone: cannot send narrate mic packet because 'insr' failed .\n"));
            }

            pLocalUser->bNarrateCbExit = TRUE;
            iSize = VOIPHEADSET_FRAMESIZE;
        }
    }
    return(iSize);
}


/*F********************************************************************************/
/*!
    \Function _VoipHeadsetNarrateCb

    \Description
        Voice callback from the narrate module

    \Input *pNarrateRef - narrate module
    \Input iUserIndex   - index this voice data belongs to
    \Input *pSamples    - audio data samples
    \Input iSize        - amount of audio data
    \Input *pUserData   - callback user data

    \Output
        int32_t         - amount of data consumed

    \Version 03/15/2019 (eesponda)
*/
/********************************************************************************F*/
static int32_t _VoipHeadsetNarrateCb(VoipNarrateRefT *pNarrateRef, int32_t iUserIndex, const int16_t *pSamples, int32_t iSize, void *pUserData)
{
    VoipHeadsetRefT *pHeadset = (VoipHeadsetRefT *)pUserData;
    int32_t iRet = 0;

    if (pHeadset->bLoopback == TRUE)
    {
        iRet = _VoipHeadsetNarrateLoopbackCb(pHeadset, iUserIndex, pSamples, iSize);
    }
    else if (pHeadset->bCrossplay == TRUE)
    {
        iRet = _VoipHeadsetNarrateCrossplayCb(pHeadset, iUserIndex, pSamples, iSize);
    }

    return(iRet);
}

#if !defined(DIRTYCODE_GDK)
/*F********************************************************************************/
/*!
    \Function _VoipHeadsetConvertRenderString

    \Description
        Converts the render (output) device id string to one that can be used by xaudio

    \Input *pInput      - the string we are converting
    \Input *pOutput     - [out] the result of the conversion
    \Input iOutputLen   - size of the output

    \Output
        wchar_t *       - result of conversion or NULL if failure

    \Notes
        There are no official docs on this. The knowledge of how this is done was
        taken from research on the Xbox Developer Forums.
        For more information see my post:
            https://forums.xboxlive.com/questions/88438/converting-iaudiodeviceinfo-id-to-device-identifie.html

    \Version 03/15/2019 (eesponda)
*/
/********************************************************************************F*/
static wchar_t *_VoipHeadsetConvertRenderString(const wchar_t *pInput, wchar_t *pOutput, int32_t iOutputLen)
{
    const char *pBegin, *pEnd;
    char strSection[64], strInput[512], strOutput[512] = {0};
    int32_t iOffset = 0, iInputLen;

    // convert to multi-byte for ease of parsing
    iInputLen = (signed)wcstombs(strInput, pInput, sizeof(strInput));

    // attempt to find our controller render device
    if (strstr(strInput, "xboxcontrollerrender") == NULL)
    {
        return(NULL);
    }
    // convert the prefix to the expected format
    iOffset += ds_snzprintf(strOutput+iOffset, sizeof(strOutput)-iOffset, "\\??\\XboxControllerRender#");

    // look for the first delimiter, append to output if found
    if ((pBegin = strchr(strInput, '#')) == NULL)
    {
        return(NULL);
    }
    pEnd = strInput+iInputLen;

    // iterate over each section and process if needed then append to output
    while (ds_strsplit(pBegin, '#', strSection, sizeof(strSection), &pBegin) != 0)
    {
        if ((strstr(strSection, "vid") != NULL) || (strstr(strSection, "pid") != NULL))
        {
            ds_strtoupper(strSection);
        }
        iOffset += ds_snzprintf(strOutput+iOffset, sizeof(strOutput)-iOffset, (pBegin != pEnd) ? "%s#" : "%s", strSection);
    }

    // convert the output to wchar_t and return
    mbstowcs(pOutput, strOutput, iOutputLen);
    return(pOutput);
}
#endif

/*F********************************************************************************/
/*!
    \Function _VoipHeadsetCreateNarrate

    \Description
        Create the narrate module for a user

    \Input *pHeadset        - module state
    \Input iLocalUserIndex  - the index of the user we are creating for

    \Output
        int32_t             - zero=success, negative=failure

    \Version 03/15/2019 (eesponda)
*/
/********************************************************************************F*/
static int32_t _VoipHeadsetCreateNarrate(VoipHeadsetRefT *pHeadset, int32_t iLocalUserIndex)
{
    #if !defined(DIRTYCODE_GDK)
    User ^refUser = nullptr;
    #else
    XUserHandle pUser;
    #endif
    wchar_t wstrDeviceId[512], *pResult = NULL;

    HRESULT hResult;
    WAVEFORMATEX WaveFormat;
    XONELocalVoipUserT *pLocalUser = &pHeadset->aLocalUsers[iLocalUserIndex];

    // check that we have a user logged in to this index and get its reference
    #if !defined(DIRTYCODE_GDK)
    if (NetConnStatus('xusr', iLocalUserIndex, &refUser, sizeof(refUser)) < 0)
    #else
    if (NetConnStatus('xusr', iLocalUserIndex, &pUser, sizeof(pUser)) < 0)
    #endif
    {
        NetPrintf(("voipheadsetxboxone: no valid user at index %d to create narration for\n", iLocalUserIndex));
        return(-1);
    }
    // if narration already created then nothing left to do
    if (pLocalUser->pNarrateRef != NULL)
    {
        return(0);
    }

    // create the narrate module
    if ((pLocalUser->pNarrateRef = VoipNarrateCreate(_VoipHeadsetNarrateCb, pHeadset)) != NULL)
    {
        // since this is being created late, set the debug level to the current setting
        #if DIRTYCODE_LOGGING
        VoipNarrateControl(pLocalUser->pNarrateRef, 'spam', pHeadset->iDebugLevel, 0, NULL);
        #endif
    }
    else
    {
        NetPrintf(("voipheadsetxboxone: failed to start narration module\n"));
        return(-1);
    }
    VoipNarrateStatus(pLocalUser->pNarrateRef, 'wave', 0, &WaveFormat, sizeof(WaveFormat));

    // create xaudio2 for this user
    #if !defined(DIRTYCODE_GDK)
    if (FAILED(hResult = XAudio2Create(pLocalUser->pXaudio2.GetAddressOf(), XAUDIO2_DO_NOT_USE_SHAPE)))
    #else
    if (FAILED(hResult = XAudio2Create(pLocalUser->pXaudio2.GetAddressOf())))
    #endif
    {
        NetPrintf(("voipheadsetxboxone: failed to create xaudio2 (%s)", DirtyErrGetName(hResult)));
        return(-1);
    }

    #if !defined(DIRTYCODE_GDK)
    // find the controller headset if we have one. if not found we will use default render device (speakers usually)
    for (uint32_t uDeviceIndex = 0, uDeviceMax = refUser->AudioDevices->Size; uDeviceIndex < uDeviceMax; uDeviceIndex += 1)
    {
        IAudioDeviceInfo ^refAudioDevice = refUser->AudioDevices->GetAt(uDeviceIndex);
        if ((refAudioDevice->DeviceCategory == AudioDeviceCategory::Communications) &&
            (refAudioDevice->DeviceType == AudioDeviceType::Render) &&
            (refAudioDevice->Sharing == AudioDeviceSharing::Exclusive))
        {
            pResult = _VoipHeadsetConvertRenderString(refAudioDevice->Id->Data(), wstrDeviceId, sizeof(wstrDeviceId) / sizeof(*wstrDeviceId));
            break;
        }
    }
    #else
    XUserLocalId LocalId;
    size_t uResultSize;
    
    // attempt to get local id which is required for the next call
    if (FAILED(hResult = XUserGetLocalId(pUser, &LocalId)))
    {
        NetPrintf(("voipheadsetxboxone: failed to get user's local id (%s)\n", DirtyErrGetName(hResult)));
        pLocalUser->pXaudio2 = nullptr;
        return(-1);
    }

    // get the default audio endpoint
    if (SUCCEEDED(hResult = XUserGetDefaultAudioEndpointUtf16(LocalId, XUserDefaultAudioEndpointKind::CommunicationRender, XUserAudioEndpointMaxUtf16Count, wstrDeviceId, &uResultSize)))
    {
        pResult = wstrDeviceId;
    }
    else
    {
        NetPrintf(("voipheadsetxboxone: failed to get default audio endpoint (%s)\n", DirtyErrGetName(hResult)));
        pLocalUser->pXaudio2 = nullptr;
        return(-1);
    }
    #endif

    // attempt to create mastering voice
    if (FAILED(hResult = pLocalUser->pXaudio2->CreateMasteringVoice(&pLocalUser->pMasteringVoice, XAUDIO2_DEFAULT_CHANNELS, XAUDIO2_DEFAULT_SAMPLERATE, 0, pResult)))
    {
        pLocalUser->pXaudio2 = nullptr;
        NetPrintf(("voipheadsetxboxone: failed to create mastering voice (%s)\n", DirtyErrGetName(hResult)));
        return(-1);
    }
    // attempt to create sourcce voice
    if (FAILED(hResult = pLocalUser->pXaudio2->CreateSourceVoice(&pLocalUser->pSourceVoice, &WaveFormat, 0, XAUDIO2_DEFAULT_FREQ_RATIO)))
    {
        pLocalUser->pMasteringVoice->DestroyVoice();
        pLocalUser->pMasteringVoice = NULL;
        pLocalUser->pXaudio2 = nullptr;
        NetPrintf(("voipheadsetxboxone: failed to create source voice (%s)\n", DirtyErrGetName(hResult)));
        return(-1);
    }
    return(0);
}

/*F********************************************************************************/
/*!
    \Function _VoipHeadsetSynthesizeSpeechLoopback

    \Description
        Text-to-speech for the specified user to be played back locally

    \Input *pHeadset        - headset module
    \Input iLocalUserIndex  - local user index (text originator)
    \Input *pText           - text to by synthesized

    \Output
        int32_t             - 0 if success, negative otherwise

    \Version 03/15/2019 (eesponda)
*/
/********************************************************************************F*/
static int32_t _VoipHeadsetSynthesizeSpeechLoopback(VoipHeadsetRefT *pHeadset, int32_t iLocalUserIndex, const char *pText)
{
    XONELocalVoipUserT *pLocalUser;

    // validate index
    if ((iLocalUserIndex < 0) || (iLocalUserIndex >= VOIP_MAXLOCALUSERS))
    {
        return(-1);
    }
    pLocalUser = &pHeadset->aLocalUsers[iLocalUserIndex];

    // create narration module if needed and submit text to be synthesized
    if (_VoipHeadsetCreateNarrate(pHeadset, iLocalUserIndex) == 0)
    {
        return(VoipNarrateInput(pLocalUser->pNarrateRef, iLocalUserIndex, pLocalUser->eGender, pText));
    }
    else
    {
        NetPrintf(("voipheadsetxboxone: failed to start narration module\n"));
        return(-1);
    }
}

/*** Public functions *************************************************************/

/*F********************************************************************************/
/*!
    \Function   VoipHeadsetCreate

    \Description
        Create the headset manager.

    \Input iMaxChannels     - max number of remote users
    \Input *pMicDataCb      - pointer to user callback to trigger when mic data is ready
    \Input *pTextDataCb     - pointer to user callback to trigger when transcribed text is ready
    \Input *pOpaqueDataCb   - pointer to user callback to trigger when opaque data is ready
    \Input *pStatusCb       - pointer to user callback to trigger when headset status changes
    \Input *pCbUserData     - pointer to user callback data
    \Input iData            - platform-specific - use to specify the game chat 2 threads affinity on XBoxONe

    \Output
        VoipHeadsetRefT *   - pointer to module state, or NULL if an error occured

    \Notes
        \verbatim
            Valid value for iData:
            The higher 16 bit is the CPU affinity value for the network processing. The lower 16 bits is 
            the CPU affinity value for the audio processing.
            For example iDate = (iNetworkCPU << 16) | iAudioCPU.
        \endverbatim

    \Version 05/24/2013 (mclouatre)
*/
/********************************************************************************F*/
VoipHeadsetRefT *VoipHeadsetCreate(int32_t iMaxChannels, VoipHeadsetMicDataCbT *pMicDataCb, VoipHeadsetTextDataCbT *pTextDataCb, VoipHeadsetOpaqueDataCbT *pOpaqueDataCb, VoipHeadsetStatusCbT *pStatusCb, void *pCbUserData, int32_t iData)
{
    VoipHeadsetRefT *pHeadset;
    int32_t iMemGroup;
    int32_t iSize;
    void *pMemGroupUserData;

    // query current mem group data
    VoipCommonMemGroupQuery(&iMemGroup, &pMemGroupUserData);

    // make sure we don't exceed maxchannels
    if (iMaxChannels > VOIP_MAXCONNECT)
    {
        NetPrintf(("voipheadsetxboxone: request for %d channels exceeds max\n", iMaxChannels));
        return(NULL);
    }

    iSize = sizeof(*pHeadset) + (sizeof(XONERemoteVoipUserT) * (iMaxChannels + VOIP_MAX_LOW_LEVEL_CONNS - 1));

    // allocate and clear module state
    if ((pHeadset = (VoipHeadsetRefT *)DirtyMemAlloc(iSize, VOIP_MEMID, iMemGroup, pMemGroupUserData)) == NULL)
    {
        NetPrintf(("voipheadsetxboxone: not enough memory to allocate module state\n"));
        return(NULL);
    }
    ds_memclr(pHeadset, iSize);

    // set default debuglevel
    #if DIRTYCODE_LOGGING
    pHeadset->iDebugLevel = 1;
    #endif

    // save callback info
    pHeadset->pMicDataCb = pMicDataCb;
    pHeadset->pTextDataCb = pTextDataCb;
    pHeadset->pOpaqueDataCb = pOpaqueDataCb;
    pHeadset->pStatusCb = pStatusCb;
    pHeadset->pCbUserData = pCbUserData;
    pHeadset->iMaxChannels = iMaxChannels;
    pHeadset->iMaxRemoteUsers = iMaxChannels + VOIP_MAX_LOW_LEVEL_CONNS;
    pHeadset->iMemGroup = VoipHeadsetXboxone_iMemGroup = iMemGroup;
    pHeadset->pMemGroupUserData = VoipHeadsetXboxone_pMemGroupUserData = pMemGroupUserData;
    pHeadset->iThreadAffinity = iData;

    // create the conduit
    if ((pHeadset->pConduitRef = VoipConduitCreate(pHeadset->iMaxChannels)) == NULL)
    {
        NetPrintf(("voipheadsetxboxone: cannot create voip conduit %d\n"));
        VoipHeadsetDestroy(pHeadset);
        return(NULL);
    }

    VoipConduitRegisterPlaybackCb(pHeadset->pConduitRef, _VoipHeadsetPlaybackCallback, pHeadset);

    // Init game chat 2 in non-crossplay mode by default
    _VoipHeadsetInitializeGameChat2(pHeadset, FALSE);

    // return module ref to caller
    return(pHeadset);
}

/*F********************************************************************************/
/*!
    \Function   VoipHeadsetDestroy

    \Description
        Destroy the headset manager.

    \Input *pHeadset    - pointer to headset state

    \Version 05/24/2013 (mclouatre)
*/
/********************************************************************************F*/
void VoipHeadsetDestroy(VoipHeadsetRefT *pHeadset)
{
    chat_manager::singleton_instance().cleanup();
    // purge the users crossplay state
    _VoipHeadsetCleanupCrossplayUserResources(pHeadset);
    
    if (pHeadset->pConduitRef != NULL)
    {
        VoipConduitDestroy(pHeadset->pConduitRef);
    }

    if (pHeadset->pTTSInputText)
    {
        DirtyMemFree(pHeadset->pTTSInputText, VOIP_MEMID, pHeadset->iMemGroup, pHeadset->pMemGroupUserData);
    }

    // dispose of module memory
    NetPrintf(("voipheadsetxboxone: [%p] destroy complete\n", pHeadset));
    DirtyMemFree(pHeadset, VOIP_MEMID, pHeadset->iMemGroup, pHeadset->pMemGroupUserData);
}

/*F********************************************************************************/
/*!
    \Function   VoipHeadsetReceiveVoiceDataCb

    \Description
        Connectionlist callback to handle receiving an unreliable opaque packet from a remote peer.

    \Input *pRemoteUsers    - user we're receiving the voice data from
    \Input iRemoteUserSize  - pRemoteUsers array size
    \Input iConsoleId       - generic identifier for the console to which the users belong
    \Input *pMicrInfo       - micr info from inbound packet
    \Input *pPacketData     - pointer to beginning of data in packet payload
    \Input *pUserData       - VoipHeadsetT ref

    \Version 05/24/2013 (mclouatre)
*/
/********************************************************************************F*/
void VoipHeadsetReceiveVoiceDataCb(VoipUserT *pRemoteUsers, int32_t iRemoteUserSize, int32_t iConsoleId, VoipMicrInfoT *pMicrInfo, uint8_t *pPacketData, void *pUserData)
{
    uint32_t uRemoteUserIndex = 0;
    uint32_t uMicrPkt;
    uint8_t *pRead = pPacketData;
    VoipHeadsetRefT *pHeadset = (VoipHeadsetRefT *)pUserData;

    if ((pMicrInfo->uSubPacketSize != 0xFF) && (pMicrInfo->uSubPacketSize != pHeadset->iCmpFrameSize))
    {
        NetPrintf(("voipheadsetxone: discarding voice packet with %d voice bundles and mismatched sub-packet size %d (expecting %d)\n",
            pMicrInfo->uNumSubPackets, pMicrInfo->uSubPacketSize, pHeadset->iCmpFrameSize));
        return;
    }

    // if this is the shared user index
    if (pMicrInfo->uUserIndex == VOIP_SHARED_REMOTE_INDEX)
    {
        int32_t iIndex;

        // find the first valid user to playback the audio
        for (iIndex = 0; iIndex < iRemoteUserSize; iIndex += 1)
        {
            if (!VOIP_NullUser(&pRemoteUsers[iIndex]))
            {
                uRemoteUserIndex = iIndex;
                break;
            }
        }
        
        if (iIndex == iRemoteUserSize)
        {
            // didn't find a remote user to play back the shared audio
            NetPrintf(("voipheadsetxone: discarding voice packet from shared user because we cannot find a remote user to play it back as!\n"));
            return;
        }
    }
    else
    {
        uRemoteUserIndex = pMicrInfo->uUserIndex;
    }


    for (uMicrPkt = 0; uMicrPkt < pMicrInfo->uNumSubPackets; uMicrPkt += 1)
    {
        // get the size of the subpacket based on variable or not
        uint32_t uSubPktSize = (pMicrInfo->uSubPacketSize != 0xFF) ? pMicrInfo->uSubPacketSize : *pRead++;

        // loop through all the VoipConduitRefs and pass in each sub packet in crossplay
        if (pHeadset->bCrossplay == TRUE)
        {
            VoipConduitReceiveVoiceData(pHeadset->pConduitRef, &pRemoteUsers[uRemoteUserIndex], pRead, uSubPktSize);        
            pRead += uSubPktSize;
        }
        // each sub-packet is an unreliabe gamechat2 data frame to be delivered to the local instance of gamechat2
        else
        {
            NetPrintfVerbose((pHeadset->iDebugLevel, 2, "voipheadsetxboxone: chat_manager::process_incoming_data(%d) - unreliable data\n", uSubPktSize));
            chat_manager::singleton_instance().process_incoming_data((uint64_t)iConsoleId, uSubPktSize, pRead);
            pRead += uSubPktSize;
        }
    }
}

/*F********************************************************************************/
/*!
    \Function   VoipHeadsetReceiveOpaqueDataCb

    \Description
        Connectionlist callback to handle receiving a reliable opaque packet from a remote peer.

    \Input iConsoleId       - generic identifier for the console to which the users belong
    \Input *pOpaqueData     - pointer to opaque data
    \Input iOpaqueDataSize  - opaque data size
    \Input *pUserData       - VoipHeadsetT ref

    \Version 05/24/2013 (mclouatre)
*/
/********************************************************************************F*/
void VoipHeadsetReceiveOpaqueDataCb(int32_t iConsoleId, const uint8_t *pOpaqueData, int32_t iOpaqueDataSize, void *pUserData)
{
    #if DIRTYCODE_LOGGING
    VoipHeadsetRefT *pHeadset = (VoipHeadsetRefT *)pUserData;

    NetPrintfVerbose((pHeadset->iDebugLevel, 2, "voipheadsetxboxone: chat_manager::process_incoming_data(%d) - reliable data\n", iOpaqueDataSize));
    #endif

    chat_manager::singleton_instance().process_incoming_data((uint64_t)iConsoleId, iOpaqueDataSize, pOpaqueData);
}

/*F********************************************************************************/
/*!
    \Function   VoipHeadsetRegisterUserCb

    \Description
        Connectionlist callback to register a new remote user with the 1st party voice system
        It will also register remote shared users with voipheadset internally

    \Input *pRemoteUser     - user to register
    \Input iConsoleId       - generic identifier for the console to which the user belongs
    \Input bRegister        - true=register, false=unregister
    \Input *pUserData       - voipheadset module ref

    \Version 05/24/2013 (mclouatre)
*/
/********************************************************************************F*/
void VoipHeadsetRegisterUserCb(VoipUserT *pRemoteUser, int32_t iConsoleId, uint32_t bRegister, void *pUserData)
{
    VoipHeadsetRefT *pHeadset = (VoipHeadsetRefT *)pUserData;

    // ignore shared users... this is all taken care of by game chat 2 now
    if ((pRemoteUser->AccountInfo.iPersonaId & VOIP_SHARED_USER_MASK) == VOIP_SHARED_USER_VALUE)
    {
        return;
    }

    if (bRegister)
    {
        _VoipHeadsetAddRemoteTalker(pHeadset, pRemoteUser, (uint64_t)iConsoleId);
    }
    else
    {
        _VoipHeadsetRemoveRemoteTalker(pHeadset, pRemoteUser);
    }

}

/*F********************************************************************************/
/*!
    \Function   VoipHeadsetProcess

    \Description
        Headset process function.

    \Input *pHeadset    - pointer to headset state
    \Input uFrameCount  - current frame count

    \Version 05/24/2013 (mclouatre)
*/
/********************************************************************************F*/
void VoipHeadsetProcess(VoipHeadsetRefT *pHeadset, uint32_t uFrameCount)
{
    static uint8_t bProcessNormalFlow = TRUE;

    // normal processing should happen at 40ms intervals
    if (bProcessNormalFlow == TRUE)
    {
        _VoipHeadsetUpdateStatus(pHeadset);
        _VoipHeadsetProcessDataFrames(pHeadset);
        _VoipHeadsetProcessStateChanges(pHeadset);

        bProcessNormalFlow = FALSE;
    }
    else
    {
        bProcessNormalFlow = TRUE;
    }
    
    // process loopback and crossplay TTS
    _VoipHeadsetProcessNarrate(pHeadset);

    // crossplay related stuff should happen at 20ms intervals
    if (pHeadset->bCrossplay == TRUE)
    {
        _VoipHeadsetProcessStreamStateChanges(pHeadset);
        _VoipHeadsetProcessPreEncodeAudio(pHeadset);
        _VoipHeadsetProcessTranscribe(pHeadset);
        _VoipHeadsetProcessPlayback(pHeadset);
    }

    #if DIRTYCODE_LOGGING
    if ( (pHeadset->iDebugLevel > 3) && 
         ((pHeadset->uLastChatUsersDump == 0) || (NetTickDiff(NetTick(), pHeadset->uLastChatUsersDump) > 5000)))
    {
        pHeadset->bDumpChatUsers = TRUE;
    }

    if (pHeadset->bDumpChatUsers)
    {
        pHeadset->bDumpChatUsers = FALSE;
        _VoipHeadsetDumpChatUsers(pHeadset);
    }
    #endif
}

/*F********************************************************************************/
/*!
    \Function   VoipHeadsetSetVolume

    \Description
        Sets play and record volume.

    \Input *pHeadset    - pointer to headset state
    \Input iPlayVol     - play volume to set
    \Input iRecVol      - record volume to set

    \Notes
        To not set a value, specify it as -1.

    \Version 05/24/2013 (mclouatre)
*/
/********************************************************************************F*/
void VoipHeadsetSetVolume(VoipHeadsetRefT *pHeadset, int32_t iPlayVol, uint32_t iRecVol)
{
    return; // not implemented
}

/*F********************************************************************************/
/*!
    \Function VoipHeadsetControl

    \Description
        Control function.

    \Input *pHeadset    - headset module state
    \Input iControl     - control selector
    \Input iValue       - control value
    \Input iValue2      - control value
    \Input *pValue      - control value

    \Output
        int32_t         - selector specific, or -1 if no such selector

    \Notes
        iControl can be one of the following:

        \verbatim
            '+pbk' - enable voice playback for a given remote user (pValue is the remote user VoipUserT).  - maps to voip per-user channel feature
            '-pbk' - disable voice playback for a given remote user (pValue is the remote user VoipUserT).  - maps to voip per-user channel feature
            'cdec' - create new codec
            'creg' - register codec
            'cstm' - clear speech to text metrics in VoipSpeechToTextMetricsT
            'ctsm' - clear text to speech metrics in VoipTextToSpeechMetricsT
            '+rcv' - enable sending voice to a remote user (pValue is the remote user VoipUserT).  - maps to voip per-connection muting feature
            '-rcv' - disable sending voice to a remote user (pValue is the remote user VoipUserT).  - maps to voip per-connection muting feature
            '+snd' - enable receiving voice from a remote user (pValue is the remote user VoipUserT).  - maps to voip per-connection muting feature
            '-snd' - disable receiving voice from a remote user (pValue is the remote user VoipUserT).  - maps to voip per-connection muting feature
            'aloc' - set local user as participating / not participating
            'loop' - enable/disable loopback mode (NOT SUPPORTED WITH MS GAME CHAT 2)
            'qual' - set encoding quality (iValue must map to one of the enum value of game_chat_audio_encoding_type_and_bitrate (XDK) or game_chat_audio_encoding_bitrate (GDK))
            'rloc' - register/unregister local talker
            'spam' - debug verbosity level
            'tran' - enable/disable local generation of transcribed text for speech produced by local users for crossplay (speech-to-text component)
            'ttos' - text-to-speech: text buffer (wchar_t) received as input is passed to the synthesizer for the specified speech impaired user
            'xply' - enable/disable crossplay 
        \endverbatim

    \Version 05/24/2013 (mclouatre)
*/
/********************************************************************************F*/
int32_t VoipHeadsetControl(VoipHeadsetRefT *pHeadset, int32_t iControl, int32_t iValue, int32_t iValue2, void *pValue)
{
    XONELocalVoipUserT *pLocalUser;

    if ((iControl == '-pbk') || (iControl == '+pbk'))
    {
        uint8_t bVoiceEnable = (iControl == '+pbk') ? TRUE : FALSE;
        int32_t iRetCode = 0; // default to success

        VoipUserT *pRemoteUser;

        if ((pValue != NULL) && (iValue < VOIP_MAXLOCALUSERS_EXTENDED))
        {
            pRemoteUser = (VoipUserT *)pValue;

            // make sure the local user and the remote user are not a shared user (shared user concept not supported on xboxone)
            if ((iValue != VOIP_SHARED_USER_INDEX) && ((pRemoteUser->AccountInfo.iPersonaId & VOIP_SHARED_USER_MASK) != VOIP_SHARED_USER_VALUE))
            {
                _VoipHeadsetSetRemoteUserVoicePlayback(pHeadset, pRemoteUser, iValue, bVoiceEnable);
            }
        }
        else
        {
            NetPrintf(("voipheadsetxboxone: VoipHeadsetControl('%C', %d) invalid arguments\n", iControl, iValue));
            iRetCode = -2;
        }
        return(iRetCode);
    }
    if (iControl == 'cdec')
    {
        int32_t iResult;

        // pass through codec creation request
        if ((iResult = VoipCodecCreate(iValue, pHeadset->iMaxChannels)) < 0)
        {
            return(iResult);
        }

        // query codec output size
        pHeadset->iCmpFrameSize = VoipCodecStatus(iValue, 'fsiz', VOIPHEADSET_FRAMESAMPLES, NULL, 0);

        // return result to caller
        return(iResult);

    }
    if (iControl == 'creg')
    {
        VoipCodecRegister(iValue, (const VoipCodecDefT *)pValue);
        return(0);
    }
    if (iControl == 'cstm')
    {
        ds_memclr(&(pHeadset->aSTTMetrics[iValue]), sizeof(pHeadset->aSTTMetrics[iValue]));
        return(0);
    }
    if (iControl == 'ctsm')
    {
        ds_memclr(&(pHeadset->aTTSMetrics[iValue]), sizeof(pHeadset->aTTSMetrics[iValue]));
        return(0);
    }
    if ((iControl == '-snd') || (iControl == '+snd'))
    {
        uint8_t bSend = (iControl == '+snd') ? TRUE : FALSE;
        int32_t iRetCode = 0; // default to success

        VoipUserT *pRemoteUser;

        if ((pValue != NULL) && (iValue < VOIP_MAXLOCALUSERS_EXTENDED))
        {
            pRemoteUser = (VoipUserT *)pValue;

            // make sure the local user and the remote user are not a shared user (shared user concept not supported on xboxone)
            if ((iValue != VOIP_SHARED_USER_INDEX) && ((pRemoteUser->AccountInfo.iPersonaId & VOIP_SHARED_USER_MASK) != VOIP_SHARED_USER_VALUE))
            {
                _VoipHeadsetSetRemoteUserOutboundMute(pHeadset, pRemoteUser, iValue, bSend);
            }
        }
        else
        {
            NetPrintf(("voipheadsetxboxone: VoipHeadsetControl('%C', %d) invalid arguments\n", iControl, iValue));
            iRetCode = -2;
        }
        return(iRetCode);
    }
    if ((iControl == '-rcv') || (iControl == '+rcv'))
    {
        uint8_t bRcv = (iControl == '+rcv') ? TRUE : FALSE;
        int32_t iRetCode = 0; // default to success

        VoipUserT *pRemoteUser;

        if ((pValue != NULL) && (iValue < VOIP_MAXLOCALUSERS_EXTENDED))
        {
            pRemoteUser = (VoipUserT *)pValue;

            // make sure the local user and the remote user are not a shared user (shared user concept not supported on xboxone)
            if ((iValue != VOIP_SHARED_USER_INDEX) && ((pRemoteUser->AccountInfo.iPersonaId & VOIP_SHARED_USER_MASK) != VOIP_SHARED_USER_VALUE))
            {
                _VoipHeadsetSetRemoteUserInboundMute(pHeadset, pRemoteUser, iValue, bRcv);
            }
        }
        else
        {
            NetPrintf(("voipheadsetxboxone: VoipHeadsetControl('%C', %d) invalid arguments\n", iControl, iValue));
            iRetCode = -2;
        }
        return(iRetCode);
    }
    if (iControl == 'aloc')
    {
        return(_VoipHeadsetActivateLocalTalker(pHeadset, iValue, iValue2 > 0));
    }
    if (iControl == 'loop')
    {
        /* mclouatre dec 2018
           I tried exercising loopback by setting up a comm relationship between a local user and himself.
           Game Chat 2 complained with:
                GC2: Fatal Error: ChatUserSetCommunicationRelationship failed
                Game Chat Fatal Error in xbox::services::game_chat_2::chat_user::chat_user_local::set_communication_relationship - Error code: 0x80070057
                - The setting cannot be configured or retrieved for a chat user with itself as the target input.
        */

        // enable loopback mode which can only be used for local tts (not with gamechat2)
        pHeadset->bLoopback = iValue;
        NetPrintf(("voipheadsetxboxone: loopback mode %s\n", (iValue ? "enabled" : "disabled")));
        return(0);
    }
    if (iControl == 'qual')
    {
        #if defined(DIRTYCODE_GDK)
        chat_manager::singleton_instance().set_audio_encoding_bitrate((game_chat_audio_encoding_bitrate)iValue);
        #else
        chat_manager::singleton_instance().set_audio_encoding_type_and_bitrate((game_chat_audio_encoding_type_and_bitrate)iValue);
        #endif
        return(0);
    }
    if (iControl == 'rloc')
    {
        if (iValue2)
        {
            return(_VoipHeadsetAddLocalTalker(pHeadset, iValue, (VoipUserT *)pValue));
        }
        else
        {
            return(_VoipHeadsetRemoveLocalTalker(pHeadset, iValue, (VoipUserT *)pValue));
        }
    }
    #if DIRTYCODE_LOGGING
    if (iControl == 'spam')
    {
        NetPrintf(("voipheadsetxboxone: setting debuglevel=%d\n", iValue));
        pHeadset->iDebugLevel = iValue;
        return(0);
    }
    #endif
    if (iControl == 'tran')
    {
        // will only matter if we are in crossplay, for native mode transcription settings they are set in the first party ui
        if (pHeadset->bVoiceTranscriptionEnabled != iValue)
        {
            pHeadset->bVoiceTranscriptionEnabled = iValue;
            NetPrintf(("voipheadsetxone %s voice transcription locally for crossplay.\n", pHeadset->bVoiceTranscriptionEnabled ? "enabling" : "disabling"));
        }
        return (0);
    }
    if (iControl == 'ttos')
    {
        if (pHeadset->bLoopback == TRUE)
        {
            return(_VoipHeadsetSynthesizeSpeechLoopback(pHeadset, iValue, (const char *)pValue));
        }
        else
        {
            return(_VoipHeadsetSynthesizeSpeech(pHeadset, iValue, (char *)pValue));
        }
    }
    if ((iControl == 'voic') && (iValue < VOIP_MAXLOCALUSERS) && (pValue != NULL))
    {
        const VoipSynthesizedSpeechCfgT *pCfg = (const VoipSynthesizedSpeechCfgT *)pValue;
        pLocalUser = &pHeadset->aLocalUsers[iValue];
        pLocalUser->eGender = (pCfg->iPersonaGender == 1) ? VOIPNARRATE_GENDER_FEMALE : VOIPNARRATE_GENDER_MALE;

        // create the narrate module
        return(_VoipHeadsetCreateNarrate(pHeadset, iValue));
    }
    if (iControl == 'xply')
    {
        int32_t iRet = 0;
        uint8_t bCrossplay = iValue ? TRUE : FALSE;

        if (pHeadset->bCrossplay != bCrossplay)
        {
            NetPrintf(("voipheadsetxboxone: changing crossplay mode to: %s\n", bCrossplay ? "crossplay" : "native"));
            iRet = _VoipHeadsetToggleCrossplayMode(pHeadset, bCrossplay);
            
            if (iRet >= 0)
            {
                pHeadset->bCrossplay = bCrossplay;
            }
        }

        return(iRet);
    }
    return(-1);
}

/*F********************************************************************************/
/*!
    \Function    VoipHeadsetStatus

    \Description
        Status function.

    \Input *pHeadset    - headset module state
    \Input iSelect      - control selector
    \Input iValue       - selector specific
    \Input *pBuf        - buffer pointer
    \Input iBufSize     - buffer size

    \Output
        int32_t         - selector specific, or -1 if no such selector

    \Notes
        iSelect can be one of the following:

        \verbatim
            'fpml' - return the first party mute setting between a local user (iValue) and a remote user (VoipUserT* in pBuf)
            'fpmu' = return TRUE if the first party mute list has been updated for a local user (index via iValue)
            'ruvu' - return TRUE if the given remote user (pBuf) is registered with voipheadset, FALSE if not.
            'talk' - return TRUE if the specified local talker (iValue) is talking or the specified remote talker (pBuf) is talking, FALSE otherwise
            'sttm' - get the VoipSpeechToTextMetricsT via pBuf, user index is iValue
            'stts' - get the local user's (iValue) STT conversion preference
            'ttsm' - get the VoipTextToSpeechMetricsT via pBuf, user index is iValue
            'ttss' - get the local user's TTS conversion preference 
            'vlen' - TRUE if voice sub-packets have variable length, FALSE otherwise
            'xply' - return status on crossplay
        \endverbatim

        Unhadled selectors are pass through to VoipCodecStatus()

    \Version 05/24/2013 (mclouatre)
*/
/********************************************************************************F*/
int32_t VoipHeadsetStatus(VoipHeadsetRefT *pHeadset, int32_t iSelect, int32_t iValue, void *pBuf, int32_t iBufSize)
{
    if (iSelect == 'fpml')
    {
        return(_VoipHeadsetFirstPartyMute(pHeadset, iValue, _VoipHeadsetGetRemoteUserIndex(pHeadset, (VoipUserT *)pBuf)));
    }
    if ((iSelect == 'fpmu') && (iValue < VOIP_MAXLOCALUSERS))
    {
        if (pHeadset->aLocalUsers[iValue].bMuteListUpdated)
        {
            pHeadset->aLocalUsers[iValue].bMuteListUpdated = FALSE;
            return(TRUE);
        }
        return(FALSE);
    }
    if (iSelect == 'ruvu')
    {
        VoipUserT *pRemoteUser = (VoipUserT *)pBuf;

        // ignore shared users... this is all taken care of by game chat 2 now
        if ((pRemoteUser->AccountInfo.iPersonaId & VOIP_SHARED_USER_MASK) == VOIP_SHARED_USER_VALUE)
        {
            return(FALSE);
        }

        if (pHeadset->bCrossplay)
        {
            int32_t iRemoteUserIndex;

            // if remote user exists return true
            for (iRemoteUserIndex = 0; iRemoteUserIndex < pHeadset->iMaxRemoteUsers; iRemoteUserIndex += 1)
            {
                if (VOIP_SameUser(&pHeadset->aRemoteUsers[iRemoteUserIndex].User, pRemoteUser))
                {
                    return(TRUE);
                }
            }
        }
        else
        {
            chat_user *pChatUser = _VoipHeadsetGetChatUserForVoipUser(pHeadset, pRemoteUser);

            // if the chat user is valid and is a remote user, then return TRUE
            if ((pChatUser != NULL) && (pChatUser->local() == NULL))
            {
                return(TRUE);
            }
        }
        return(FALSE);
    }
    if (iSelect == 'talk')
    {
        uint32_t bTalking = FALSE;  // default to FALSE
        chat_user *pChatUser = NULL;

        // in crossplay mode statues will be handled up in the Voipconnection layer like other platforms
        if (pHeadset->bCrossplay == TRUE)
        {
            return(-1);
        }

        // local user or remote user ?
        if (pBuf == NULL)
        {
            pChatUser = pHeadset->aLocalUsers[iValue].pChatUser;
        }
        else
        {
            VoipUserT *pVoipUser = (VoipUserT *)pBuf;
            pChatUser = _VoipHeadsetGetChatUserForVoipUser(pHeadset, pVoipUser);
        }

        if (pChatUser)
        {
            bTalking = (pChatUser->chat_indicator() == game_chat_user_chat_indicator::talking) ? TRUE : FALSE;
        }

        return(bTalking);
    }
    if (iSelect == 'sttm')
    {
        // note that on xbox we only report metrics for the 'native' stt, not metrics when in crossplay
        if ((pBuf != NULL) && (iBufSize >= sizeof(VoipSpeechToTextMetricsT)))
        {
            ds_memcpy_s(pBuf, iBufSize, &pHeadset->aSTTMetrics[iValue], sizeof(VoipSpeechToTextMetricsT));
            return(0);
        }
        return(-1);
    }
    if (iSelect == 'stts')
    {
        const chat_user *pChatUser;

        if ((iValue < 0) || (iValue >= VOIP_MAXLOCALUSERS))
        {
            return(FALSE);
        }
        return(((pChatUser = pHeadset->aLocalUsers[iValue].pChatUser) != NULL) ? pChatUser->local()->speech_to_text_conversion_preference_enabled() : FALSE);
    }
    if (iSelect == 'ttsm')
    {
        // note that on xbox we only report metrics for the 'native' tts, not metrics when in crossplay
        if ((pBuf != NULL) && (iBufSize >= sizeof(VoipTextToSpeechMetricsT)))
        {
            ds_memcpy_s(pBuf, iBufSize, &pHeadset->aTTSMetrics[iValue], sizeof(VoipTextToSpeechMetricsT));
            return(0);
        }
        return(-1);
    }
    if (iSelect == 'ttss')
    {
        if ((iValue < 0) || (iValue >= VOIP_MAXLOCALUSERS))
        {
            NetPrintf(("voipheadsetxboxone: VoipHeadsetStatus('ttss') invalid agruments.\n"));
            return(FALSE);
        }

        if (pHeadset->aLocalUsers[iValue].pChatUser != NULL)
        {
            chat_user * pChatUser = pHeadset->aLocalUsers[iValue].pChatUser;
            return(pChatUser->local()->text_to_speech_conversion_preference_enabled());
        }

        NetPrintf(("voipheadsetxboxone: VoipHeadsetStatus('ttss') did  not find a valid local user.\n"));
        return(FALSE);
    }
    if (iSelect == 'vlen')
    {
        *(uint8_t *)pBuf = TRUE;
        return(0);
    }
    if (iSelect == 'xply')
    {
        return(pHeadset->bCrossplay);
    }
    
    // unhandled result, fallthrough to active codec
    if (VoipCodecStatus(0, 'coco', 0, NULL, 0) > 0)
    {
        return(VoipCodecStatus(VOIP_CODEC_ACTIVE, iSelect, iValue, pBuf, iBufSize));
    }
    else
    {
        // unhandled selector
        return(-1);
    }
}

/*F********************************************************************************/
/*!
    \Function VoipHeadsetSpkrCallback

    \Description
        Set speaker output callback.

    \Input *pHeadset    - headset module state
    \Input *pCallback   - what to call when output data is available
    \Input *pUserData   - user data for callback

    \Version 05/24/2013 (mclouatre)
*/
/********************************************************************************F*/
void VoipHeadsetSpkrCallback(VoipHeadsetRefT *pHeadset, VoipSpkrCallbackT *pCallback, void *pUserData)
{
    pHeadset->pSpkrDataCb = pCallback;
    pHeadset->pSpkrCbUserData = pUserData;
}

/*F********************************************************************************/
/*!
    \Function VoipHeadsetSetTranscribedTextReceivedCallback

    \Description
        Set callback to be invoked by voipheadsetxboxone when inbound transcribed text
        has passed communication permission check.

    \Input *pHeadset    - headset module state
    \Input *pCallback   - what to call when transcribed text is received from remote player
    \Input *pUserData   - user data for callback

    \Version 11/28/2018 (mclouatre)
*/
/********************************************************************************F*/
void VoipHeadsetSetTranscribedTextReceivedCallback(VoipHeadsetRefT *pHeadset, VoipHeadsetTranscribedTextReceivedCbT *pCallback, void *pUserData)
{
    pHeadset->pTranscribedTextReceivedCb = pCallback;
    pHeadset->pTranscribedTextReceivedCbUserData = pUserData;
}


/*F********************************************************************************/
/*!
    \Function VoipHeadsetSetFirstPartyIdCallback

    \Description
        callback to tell dirtysdk what the first party id is for this user

    \Input *pHeadset    - headset module state
    \Input *pCallback   - what to call when transcribed text is received from remote player
    \Input *pUserData   - user data for callback

    \Version 06/25/2019 (cvienneau)
*/
/********************************************************************************F*/
void VoipHeadsetSetFirstPartyIdCallback(VoipHeadsetRefT *pHeadset, VoipFirstPartyIdCallbackCbT *pCallback, void *pUserData)
{
    pHeadset->pFirstPartyIdCbUserData = pUserData;
    pHeadset->pFirstPartyIdCallback = pCallback;
}

/*F********************************************************************************/
/*!
    \Function VoipHeadsetConfigTranscription

    \Description
        Configure the transcribe module

    \Input *pHeadset    - headset module state
    \Input uProfile     - transcribe profile
    \Input *pUrl        - transcribe provider url
    \Input *pKey        - transcribe key

    \Version 11/06/2018 (tcho)
*/
/********************************************************************************F*/
void VoipHeadsetConfigTranscription(VoipHeadsetRefT *pHeadset, uint32_t uProfile, const char *pUrl, const char *pKey)
{
    VoipTranscribeConfig(uProfile, pUrl, pKey);
}
