/*H********************************************************************************/
/*!
    \File voipheadsetstadia.c

    \Description
        Stadia VoIP headset manager

    \Copyright
        Copyright Electronic Arts 2020.

    \Version 03/03/2020 (eesponda)
*/
/********************************************************************************H*/

/*** Include files ****************************************************************/

#include "DirtySDK/platform.h"

EA_DISABLE_CLANG_WARNING(-Wstrict-prototypes)
#include <ggp_c/audio_playback.h>
#include <ggp_c/future.h>
#include <ggp_c/microphone.h>
#include <ggp_c/social.h>
#include <ggp_c/status.h>
EA_RESTORE_CLANG_WARNING()

#include <pulse/pulseaudio.h>

#include "DirtySDK/dirtysock/dirtymem.h"
#include "DirtySDK/dirtysock/dirtylib.h"
#include "DirtySDK/voip/voipdef.h"
#include "DirtySDK/voip/voipcodec.h"
#include "DirtySDK/voip/voipnarrate.h"
#include "DirtySDK/voip/voiptranscribe.h"

#include "voippriv.h"
#include "voipcommon.h"
#include "voipmixer.h"
#include "voipconduit.h"
#include "voipconnection.h"
#include "voippcm.h"
#include "voipheadset.h"

/*** Defines ***********************************************************************/

#define VOIPHEADSET_SAMPLERATE (48000)                                                          //!< sample rate; 48kHz audio
#define VOIPHEADSET_SAMPLEWIDTH (2)                                                             //!< sample size; 16-bit samples
#define VOIPHEADSET_FRAMEDURATION (20)                                                          //!< frame duration in milliseconds; 20ms
#define VOIPHEADSET_FRAMESAMPLES ((VOIPHEADSET_SAMPLERATE * VOIPHEADSET_FRAMEDURATION) / 1000)  //!< samples per frame (20ms; 48hkz=960)
#define VOIPHEADSET_FRAMESIZE (VOIPHEADSET_FRAMESAMPLES * VOIPHEADSET_SAMPLEWIDTH)              //!< frame size in bytes; 1920
#define VOIPHEADSET_BLOCKLIST_INTERVAL (15*1000)                                                //!< STR 137.2 states we need to react to blocking within 30s, use a safe value
#define VOIPHEADSET_BLOCKLIST_MAX (2048)                                                        //!< maximum size blocklist we support; this aligns with the maximums on other platforms

/*** Macros ************************************************************************/

//! calculate the frame samples based on the sample rate
#define VOIPHEADSET_GetFrameSamples(uSampleRate) (((uSampleRate) * VOIPHEADSET_FRAMEDURATION) / 1000)

/*** Type Definitions **************************************************************/

//!< local user data space
typedef struct StadiaLocalVoipUserT
{
    VoipUserT User;             //!< user information
    GgpPlayerId uPlayerId;      //!< platform player id
    uint32_t uPlaybackFlags;    //!< mute flag every remote user is 1 bit
} StadiaLocalVoipUserT;

//! remote user data space
typedef struct StadiaRemoteVoipUserT
{
    VoipUserT User;         //!< user information
    GgpPlayerId uPlayerId;  //!< platform player id
} StadiaRemoteVoipUserT;

typedef struct VoipHeadsetDeviceInfoT
{
    GgpMicrophone Device;       //!< handle to currently open device, or kGgpInvalidReference if no device is open
} VoipHeadsetDeviceInfoT;

typedef struct VoipHeadsetSpeakerInfoT
{
    GgpAudioEndpoint Device;    //!< handle to currently open device or kGgpInvalidReference if no device is open
    pa_context *pAudioContext;  //!< pulseaudio context used for connecting to remote server. cannot be re-used thus tied to the stream
    pa_stream *pStream;         //!< pulse audio stream open for this speaker device
} VoipHeadsetSpeakerInfoT;

//! VOIP module state data
struct VoipHeadsetRefT
{
    int32_t iMemGroup;                      //!< memgroup id
    void *pMemGroupUserData;                //!< memgroup userdata

    VoipSpkrCallbackT *pSpkrDataCb;         //!< speaker callback
    void *pSpkrCbUserData;                  //!< speaker callback user data

    VoipHeadsetMicDataCbT *pMicDataCb;      //!< mic data callback
    VoipHeadsetTextDataCbT *pTextDataCb;    //!< text data callback
    VoipHeadsetStatusCbT *pStatusCb;        //!< headset status callback
    VoipHeadsetOpaqueDataCbT *pOpaqueDataCb;//!< opaque data callback
    void *pCbUserData;                      //!< callback user data

    StadiaLocalVoipUserT aLocalUsers[VOIP_MAXLOCALUSERS]; //!< local users registered with voip
    int32_t iParticipatingUserIndex;        //!< local user who owns voip
    uint8_t bLoopback;                      //!< playing voice back to the user
    int8_t iVerbose;                        //!< verbosity level

    int8_t iMaxConduits;                    //!< number of conduits
    VoipConduitRefT *pConduitRef;           //!< conduit module state
    VoipMixerRefT *pMixerRef;               //!< mixer module state

    uint32_t uSendSeq;                      //!< sequence sent in MIC packets

    GgpFuture BlockListAsyncOp;             //!< holds the result of the async operation
    GgpProfile aBlockList[VOIPHEADSET_BLOCKLIST_MAX]; //!< blocked users
    uint16_t uNumBlocked;                   //!< current number of blocked users
    uint8_t bPendingBlockList;              //!< is the block list request still pending? this protects pulling out of the future multiple times
    uint8_t bMuteListUpdated;               //!< TRUE if the mute list was updated
    uint32_t uNextBlockListCheck;           //!< when should we kick off a block list check

    VoipHeadsetDeviceInfoT MicrInfo;        //!< microphone info
    VoipHeadsetSpeakerInfoT SpkrInfo;       //!< speaker info

    pa_mainloop *pPulseAudio;               //!< main pulseaudio state
    VoipNarrateRefT *pNarrate;              //!< voip narrate module ref
    int16_t *pNarrateBuf;                   //!< allocated narrate buffer based on the sample rate; may differ from system's sample rate
    int32_t iNarrateMax;                    //!< maximum size (in number of samples) of the narrate buffer
    int32_t iNarrateLen;                    //!< current length (in number of samples) of the narrate buffer
    int32_t iNarrateInp;                    //!< write pointer; head of circular buffer
    int32_t iNarrateOut;                    //!< read pointer; tail of circular buffer
    int8_t iNarrating;                      //!< narration status; zero=inactive, one=narrating, negative=done
    uint8_t uNarrationGender;               //!< text-to-speech gender (VOIPNARRATE_GENDER_*); defaults to neutral (usually male-sounding)

    uint8_t bTextChatAccessibility;         //!< TRUE if text chat accessibility features are enabled
    uint8_t bVoiceTranscriptionEnabled;     //!< TRUE if voice transcription is enabled
    VoipTranscribeRefT *pTranscribe;        //!< voip transcription module ref

    GgpEventQueue EventQueue;               //!< used for processing callbacks
    GgpEventHandle uMicHotplug;             //!< handle to the microphone hotplug event
    GgpEventHandle uMicStateChanged;        //!< handle to the microphone state changed event
    GgpEventHandle uMicAudioStateChanged;   //!< handle to the microphone audio state changed event
    GgpEventHandle uAudioEndpointChanged;   //!< handle to the audio endpoint state changed event

    VoipFirstPartyIdCallbackCbT *pFirstPartyIdCb;   //!< function to query the GgpPlayerId for remote users
    void *pFirstPartyIdCbUserData;                  //!< function callback user data

    int32_t iCmpFrameSize;                  //!< codec frame size
    int32_t iNumRemoteUsers;                //!< current number of registered remote users
    int32_t iMaxRemoteUsers;                //!< maximum number of remote users
    StadiaRemoteVoipUserT aRemoteUsers[1];  //!< remote user list - must come last in ref as it is variable length
};

/*** Function Prototypes **********************************************************/

/*** Variables ********************************************************************/

/*** Private Functions ************************************************************/


/*F********************************************************************************/
/*!
    \Function _VoipHeadsetPulseContextStateCb

    \Description
        State change handler for the pulseaudio context

    \Input *pAudioContext   - context whose state has changed
    \Input *pUserData       - callback user data

    \Version 03/03/2020 (eesponda)
*/
/********************************************************************************F*/
static void _VoipHeadsetPulseContextStateCb(pa_context *pAudioContext, void *pUserData)
{
    static const pa_channel_map _ChannelMap =
    {
        .channels = 1,
        .map = { PA_CHANNEL_POSITION_MONO }
    };
    static const pa_sample_spec _SampleSpec =
    {
        .format = PA_SAMPLE_S16LE,
        .rate = VOIPHEADSET_SAMPLERATE,
        .channels = 1
    };
    static const pa_buffer_attr _BufferAttr =
    {
        .fragsize = 0xffffffff,
        .maxlength = 0xffffffff,
        .minreq = 0xffffffff,
        .prebuf = 0xffffffff,
        .tlength = 0
    };

    VoipHeadsetRefT *pHeadset = (VoipHeadsetRefT *)pUserData;
    int32_t iResult;
    char strDeviceName[kGgpMaxPulseAudioDeviceNameSize];

    // ignore the other states
    if (pa_context_get_state(pAudioContext) != PA_CONTEXT_READY)
    {
        return;
    }

    // get the device name
    GgpAudioEndpointGetPulseAudioDeviceName(pHeadset->SpkrInfo.Device, strDeviceName, sizeof(strDeviceName), NULL);

    // create a stream when the context is ready
    if ((pHeadset->SpkrInfo.pStream = pa_stream_new(pAudioContext, "", &_SampleSpec, &_ChannelMap)) == NULL)
    {
        NetPrintf(("voipheadsetstadia: failed to allocate new pulseaudio stream\n"));
        return;
    }
    // connect the stream playback
    if ((iResult = pa_stream_connect_playback(pHeadset->SpkrInfo.pStream, strDeviceName, &_BufferAttr, PA_STREAM_NOFLAGS, NULL, NULL)) != 0)
    {
        NetPrintf(("voipheadsetstadia: pulseaudio stream connect playback failed with %d\n", iResult));
    }
}

/*F********************************************************************************/
/*!
    \Function _VoipHeadsetStreamFlushCb

    \Description
        Callback fired on completion of the pulseaudio stream flush

    \Input *pStream     - the pulseaudio stream
    \Input iSuccess     - success of the flush
    \Input *pUserData   - callback user data

    \Notes
        Within this callback handler we also cleanup the context as it cannot
        be reused when pa_context_disconnect is called.

    \Version 03/03/2020 (eesponda)
*/
/********************************************************************************F*/
static void _VoipHeadsetStreamFlushCb(pa_stream *pStream, int32_t iSuccess, void *pUserData)
{
    VoipHeadsetRefT *pHeadset = (VoipHeadsetRefT *)pUserData;
    NetPrintfVerbose((pHeadset->iVerbose, 0, "voipheadsetstadia: flush of the pulseaudio stream completed with success = %d\n", iSuccess));

    // finish the stream cleanup
    pa_stream_disconnect(pStream);
    pa_stream_unref(pStream);

    // disconnect and unref the context
    pa_context_set_state_callback(pHeadset->SpkrInfo.pAudioContext, NULL, NULL);
    pa_context_disconnect(pHeadset->SpkrInfo.pAudioContext);
    pa_context_unref(pHeadset->SpkrInfo.pAudioContext);
    pHeadset->SpkrInfo.pAudioContext = NULL;
}

/*F********************************************************************************/
/*!
    \Function _VoipHeadsetMicrophoneHotplug

    \Description
        Callback fired when the microphone hotplug state changes

    \Input *pEvent      - the event data
    \Input *pUserData   - callback user data

    \Version 03/03/2020 (eesponda)
*/
/********************************************************************************F*/
static void _VoipHeadsetMicrophoneHotplug(const GgpMicrophoneHotplugEvent *pEvent, void *pUserData)
{
    #if DIRTYCODE_LOGGING
    VoipHeadsetRefT *pHeadset = (VoipHeadsetRefT *)pUserData;
    #endif

    NetPrintfVerbose((pHeadset->iVerbose, 1, "voipheadsetstadia: _VoipHeadsetMicrophoneHotPlug (mic=%llu, connected=%s)\n",
        pEvent->microphone, pEvent->connected ? "true" : "false"));
}

/*F********************************************************************************/
/*!
    \Function _VoipHeadsetMicAudioStateChanged

    \Description
        Callback fired when the microphone audio state changes

    \Input *pEvent      - the event data
    \Input *pUserData   - callback user data

    \Version 03/03/2020 (eesponda)
*/
/********************************************************************************F*/
static void _VoipHeadsetMicAudioStateChanged(const GgpMicAudioStateChangedEvent *pEvent, void *pUserData)
{
    #if DIRTYCODE_LOGGING
    static const char *_strAccessState[] =
    {
        "kGgpMicAudioAccessState_Invalid", "kGgpMicAudioAccessState_Available", "kGgpMicAudioAccessState_Busy"
    };
    static const char *_strMuteState[] =
    {
        "kGgpMicAudioMuteState_Invalid", "kGgpMicAudioMuteState_Muted", "kGgpMicAudioMuteState_Unmuted"
    };
    #endif
    VoipHeadsetRefT *pHeadset = (VoipHeadsetRefT *)pUserData;
    int32_t iLocalUserIndex;

    NetPrintfVerbose((pHeadset->iVerbose, 1, "voipheadsetstadia: _VoipHeadsetMicAudioStateChanged (player_id=%llu, access=%s, mute=%s, listening=%s\n",
        pEvent->player_id, _strAccessState[pEvent->access], _strMuteState[pEvent->mute], pEvent->listening ? "true" : "false"));

    // if the user is found, fire the callback given the current mic audio state
    for (iLocalUserIndex = 0; iLocalUserIndex < VOIP_MAXLOCALUSERS; iLocalUserIndex += 1)
    {
        const StadiaLocalVoipUserT *pLocalUser = &pHeadset->aLocalUsers[iLocalUserIndex];
        if (pLocalUser->uPlayerId == pEvent->player_id)
        {
            pHeadset->pStatusCb(iLocalUserIndex, pEvent->access == kGgpMicAudioAccessState_Available, VOIP_HEADSET_STATUS_INPUT, pHeadset->pCbUserData);
            break;
        }
    }
}

/*F********************************************************************************/
/*!
    \Function _VoipHeadsetMicStateChanged

    \Description
        Callback fired when the microphone state changes

    \Input *pEvent      - the event data
    \Input *pUserData   - callback user data

    \Version 03/03/2020 (eesponda)
*/
/********************************************************************************F*/
static void _VoipHeadsetMicStateChanged(const GgpMicrophoneStateChangedEvent *pEvent, void *pUserData)
{
    VoipHeadsetRefT *pHeadset = (VoipHeadsetRefT *)pUserData;
    NetPrintfVerbose((pHeadset->iVerbose, 1, "voipheadsetstadia: _VoipHeadsetMicStateChanged (mic=%llu, is_live=%s)\n",
        pEvent->microphone, pEvent->is_live ? "true" : "false"));

    // set the microphone device state
    pHeadset->MicrInfo.Device = pEvent->is_live ? pEvent->microphone : kGgpInvalidReference;
}

/*F********************************************************************************/
/*!
    \Function _VoipHeadsetAudioEndpointChanged

    \Description
        Callback fired when the audio endpoint (speaker) state changes

    \Input *pEvent      - the event data
    \Input *pUserData   - callback user data

    \Version 03/03/2020 (eesponda)
*/
/********************************************************************************F*/
static void _VoipHeadsetAudioEndpointChanged(const GgpAudioEndpointChangedEvent *pEvent, void *pUserData)
{
    #if DIRTYCODE_LOGGING
    static const char *_strEndpointState[] =
    {
        "kGgpAudioEndpointState_Invalid", "kGgpAudioEndpointState_Enabled", "kGgpAudioEndpointState_Disabled"
    };
    #endif

    VoipHeadsetRefT *pHeadset = (VoipHeadsetRefT *)pUserData;
    VoipHeadsetSpeakerInfoT *pSpkrInfo = &pHeadset->SpkrInfo;
    int32_t iResult;

    // ignore anything not relating to voice chat
    if (pEvent->endpoint_type != kGgpAudioEndpointType_VoiceChat)
    {
        return;
    }
    NetPrintfVerbose((pHeadset->iVerbose, 1, "voipheadsetstadia: _VoipHeadsetAudioEndpointChanged (audio_endpoint=%llu, endpoint_type=kGgpAudioEndpointType_VoiceChat, endpoint_state=%s\n",
        pEvent->audio_endpoint, _strEndpointState[pEvent->endpoint_state]));

    if (pEvent->endpoint_state == kGgpAudioEndpointState_Enabled)
    {
        // assign the device that was enabled
        pSpkrInfo->Device = pEvent->audio_endpoint;

        // create the pulse audio context
        if ((pHeadset->SpkrInfo.pAudioContext = pa_context_new(pa_mainloop_get_api(pHeadset->pPulseAudio), "")) == NULL)
        {
            NetPrintf(("voipheadsetstadia: failed to allocate the pulse audio context\n"));
            return;
        }
        pa_context_set_state_callback(pHeadset->SpkrInfo.pAudioContext, _VoipHeadsetPulseContextStateCb, pHeadset);

        // start the async connect
        if ((iResult = pa_context_connect(pHeadset->SpkrInfo.pAudioContext, NULL, PA_CONTEXT_NOAUTOSPAWN, NULL)) != 0)
        {
            NetPrintf(("voipheadsetstadia: pulseaudio context connect failed err=%d/%s\n", iResult, pa_strerror(iResult)));
        }
    }
    else if (pEvent->endpoint_state == kGgpAudioEndpointState_Disabled)
    {
        // reset the device to invalid
        pHeadset->SpkrInfo.Device = kGgpInvalidReference;

        // flush the stream, checking the validity as it could have failed to be allocated
        if (pHeadset->SpkrInfo.pStream != NULL)
        {
            pa_stream_flush(pHeadset->SpkrInfo.pStream, _VoipHeadsetStreamFlushCb, pHeadset);
            pHeadset->SpkrInfo.pStream = NULL;
        }
        // otherwise just destroy the context
        else if (pHeadset->SpkrInfo.pAudioContext != NULL)
        {
            pa_context_set_state_callback(pHeadset->SpkrInfo.pAudioContext, NULL, NULL);
            pa_context_disconnect(pHeadset->SpkrInfo.pAudioContext);
            pa_context_unref(pHeadset->SpkrInfo.pAudioContext);
            pHeadset->SpkrInfo.pAudioContext = NULL;
        }
    }
}

/*F********************************************************************************/
/*!
    \Function _VoipHeadsetSendEncodedAudio

    \Description
        Send the compressed audio data

    \Input *pHeadset    - headset state
    \Input *pPacket     - the compressed audio data being sent
    \Input iCompBytes   - number of bytes being sent

    \Version 03/03/2020 (eesponda)
*/
/********************************************************************************F*/
static void _VoipHeadsetSendEncodedAudio(VoipHeadsetRefT *pHeadset, const uint8_t *pPacket, int32_t iCompBytes)
{
    if (!pHeadset->bLoopback)
    {
        pHeadset->pMicDataCb(pPacket, iCompBytes, NULL, 0, pHeadset->iParticipatingUserIndex, pHeadset->uSendSeq++, pHeadset->pCbUserData);
    }
    else
    {
        // setup a null user for loopback
        VoipUserT NullUser;
        ds_memclr(&NullUser, sizeof(NullUser));

        VoipConduitReceiveVoiceData(pHeadset->pConduitRef, &NullUser, pPacket, iCompBytes);
    }
}

/*F********************************************************************************/
/*!
    \Function _VoipHeadsetWriteNarrateInput

    \Description
        Write audio samples to our narration buffer

    \Input *pHeadset    - headset state
    \Input *pSamples    - audio samples
    \Input iNumSamples  - number of audio samples to write

    \Output
        int32_t         - number of samples written to our buffer

    \Version 04/14/2020 (eesponda)
*/
/********************************************************************************F*/
static int32_t _VoipHeadsetWriteNarrateInput(VoipHeadsetRefT *pHeadset, const int16_t *pSamples, int32_t iNumSamples)
{
    int32_t iWriteSize, iWriteSize2;

    // see how much room we have to write
    if ((iWriteSize = pHeadset->iNarrateMax - pHeadset->iNarrateLen) < iNumSamples)
    {
        iNumSamples = iWriteSize;
    }
    // see if we need to wrap the copy
    if ((pHeadset->iNarrateInp + iNumSamples) > pHeadset->iNarrateMax)
    {
        // write to end of buffer
        iWriteSize = pHeadset->iNarrateMax - pHeadset->iNarrateInp;
        ds_memcpy(pHeadset->pNarrateBuf+pHeadset->iNarrateInp, pSamples, sizeof(*pHeadset->pNarrateBuf) * iWriteSize);
        // wrap and write the rest
        iWriteSize2 = iNumSamples - iWriteSize;
        ds_memcpy(pHeadset->pNarrateBuf, pSamples+iWriteSize, sizeof(*pHeadset->pNarrateBuf) * iWriteSize2);
        pHeadset->iNarrateInp = iWriteSize2;
    }
    else
    {
        ds_memcpy(pHeadset->pNarrateBuf+pHeadset->iNarrateInp, pSamples, sizeof(*pHeadset->pNarrateBuf) * iNumSamples);
        pHeadset->iNarrateInp += iNumSamples;
    }
    // add to size
    pHeadset->iNarrateLen += iNumSamples;
    // return samples written
    return(iNumSamples);
}

/*F********************************************************************************/
/*!
    \Function _VoipHeadsetReadNarrateInput

    \Description
        Read audio samples from our narration buffer

    \Input *pHeadset    - headset state
    \Input *pSamples    - [out] audio samples
    \Input iNumSamples  - number of audio samples to read

    \Output
        int32_t         - number of samples read from our buffer

    \Notes
        This function might not read a full frame of samples, thus the data is cleared
        to first to make sure that the codec accepts the data.

    \Version 04/14/2020 (eesponda)
*/
/********************************************************************************F*/
static int32_t _VoipHeadsetReadNarrateInput(VoipHeadsetRefT *pHeadset, int16_t *pSamples, int32_t iNumSamples)
{
    int32_t iReadSize;

    // clear the buffer; we want to ensure for partial reads that the end of the buffer is still zero'd out
    ds_memclr(pSamples, iNumSamples);

    // see how much data is available for reading
    if ((iReadSize = pHeadset->iNarrateLen) == 0)
    {
        // return if there isn't enough data to read
        return(0);
    }
    else if (iReadSize < iNumSamples)
    {
        iNumSamples = iReadSize;
    }

    // handle wrapping copy
    if ((pHeadset->iNarrateOut + iNumSamples) > pHeadset->iNarrateMax)
    {
        // get size to end of buffer
        int32_t iReadSize2 = pHeadset->iNarrateMax - pHeadset->iNarrateOut;
        // get size of the rest of the copy after wrapping
        iReadSize = iNumSamples - iReadSize2;
        // do the copy
        ds_memcpy(pSamples, pHeadset->pNarrateBuf+pHeadset->iNarrateOut, sizeof(*pSamples) * iReadSize2);
        ds_memcpy(pSamples+iReadSize2, pHeadset->pNarrateBuf, sizeof(*pSamples) * iReadSize);
        pHeadset->iNarrateOut = iReadSize;
        pHeadset->iNarrateLen -= iNumSamples;
    }
    // regular non-wrapping copy
    else
    {
        ds_memcpy(pSamples, pHeadset->pNarrateBuf+pHeadset->iNarrateOut, sizeof(*pSamples) * iNumSamples);
        pHeadset->iNarrateOut += iNumSamples;
        pHeadset->iNarrateLen -= iNumSamples;
    }
    return(iNumSamples);
}

/*F********************************************************************************/
/*!
    \Function _VoipHeadsetTranscribeCreate

    \Description
        Create the transcription module if it doesn't exist and returns the module ref

    \Input *pHeadset            - headset module state

    \Output
        VoipTranscribeRefT *    - pointer to the module state or NULL if failure

    \Version 04/16/2020 (eesponda)
*/
/********************************************************************************F*/
static VoipTranscribeRefT *_VoipHeadsetTranscribeCreate(VoipHeadsetRefT *pHeadset)
{
    // create the module if it doesnt exist. using 16k buffer to match protossl for optimal transmission
    if ((pHeadset->pTranscribe == NULL) && ((pHeadset->pTranscribe = VoipTranscribeCreate(16 * 1024)) != NULL))
    {
        VoipTranscribeControl(pHeadset->pTranscribe, 'spam', pHeadset->iVerbose, 0, NULL);
    }
    return(pHeadset->pTranscribe);
}

/*F********************************************************************************/
/*!
    \Function _VoipHeadsetProcessTranscription

    \Description
        Gives time for the transcription module to do its work and pulls out any
        results returned

    \Input *pHeadset    - headset module state

    \Version 04/16/2020 (eesponda)
*/
/********************************************************************************F*/
static void _VoipHeadsetProcessTranscription(VoipHeadsetRefT *pHeadset)
{
    char strTranscribeBuf[VOIPTRANSCRIBE_OUTPUT_MAX];
    if (pHeadset->pTranscribe == NULL)
    {
        return;
    }

    // process transcription
    VoipTranscribeUpdate(pHeadset->pTranscribe);

    // if a transcription is available, send it along
    if (VoipTranscribeGet(pHeadset->pTranscribe, strTranscribeBuf, sizeof(strTranscribeBuf)) <= 0)
    {
        return;
    }

    // if we have an active user, fire the callback
    if (pHeadset->iParticipatingUserIndex != VOIP_INVALID_LOCAL_USER_INDEX)
    {
        pHeadset->pTextDataCb(strTranscribeBuf, pHeadset->iParticipatingUserIndex, pHeadset->pCbUserData);
    }

    // enable short audio discard if not narrating
    if (pHeadset->iNarrating == 0)
    {
        VoipTranscribeControl(pHeadset->pTranscribe, 'vdis', TRUE, 0, NULL);
    }
}

/*F********************************************************************************/
/*!
    \Function _VoipHeadsetProcessRecord

    \Description
        Process pulling data out of the microphone

    \Input *pHeadset    - headset state

    \Version 03/03/2020 (eesponda)
*/
/********************************************************************************F*/
static void _VoipHeadsetProcessRecord(VoipHeadsetRefT *pHeadset)
{
    GgpStatus Status;
    int32_t iNumSamples, iCompBytes;
    uint8_t aPacketData[VOIPHEADSET_FRAMESIZE];
    int16_t aFrameData[VOIPHEADSET_FRAMESAMPLES];
    VoipTranscribeRefT *pTranscribe;
    // make sure we don't do transcription when doing text chat accessiblity while narrating
    const uint8_t bVoiceTranscriptionEnabled = (pHeadset->iNarrating != 0) ? (!pHeadset->bTextChatAccessibility && pHeadset->bVoiceTranscriptionEnabled) : pHeadset->bVoiceTranscriptionEnabled;

    // only record if we have a recording device open and taking input
    if ((pHeadset->MicrInfo.Device == kGgpInvalidReference) || !GgpMicrophoneIsLive(pHeadset->MicrInfo.Device))
    {
        return;
    }

    // validate the number of available samples
    if ((iNumSamples = GgpMicrophoneGetAvailableSampleCount(pHeadset->MicrInfo.Device, &Status)) < 0)
    {
        NetPrintf(("voipheadsetstadia: failed to get available samples from the mic (status_code=%d)\n", Status.status_code));
    }
    else if (iNumSamples == 0)
    {
        return;
    }

    // copy data from the microphone, will be discarded if we are narrating
    if ((iNumSamples = GgpMicrophoneGetAudioData(pHeadset->MicrInfo.Device, aFrameData, VOIPHEADSET_FRAMESAMPLES, &Status)) < 0)
    {
        NetPrintf(("voipheadsetstadia: failed to get audio data from the mic (status_code=%d)\n", Status.status_code));
        return;
    }

    // if we are narrating read the samples out of the buffer. when no samples are available in this case we then early return; ignored if narration inactive
    if ((pHeadset->iNarrating != 0) && (iNumSamples = _VoipHeadsetReadNarrateInput(pHeadset, aFrameData, VOIPHEADSET_GetFrameSamples(VOIPNARRATE_SAMPLERATE))) == 0)
    {
        return;
    }

    // try to create the ref if transcription is enabled, if it is already created the function will return it
    pTranscribe = bVoiceTranscriptionEnabled ? _VoipHeadsetTranscribeCreate(pHeadset) : NULL;

    // compress the audio data and if there is any compressed data send it to the appropriate function
    if (!NetConnStatus('chat', pHeadset->iParticipatingUserIndex, NULL, 0) && (iCompBytes = VoipCodecEncode(aPacketData, aFrameData, iNumSamples, pTranscribe)) > 0)
    {
        // send the encoded audio if we are narrating or not narrating but text chat accessiblity is disabled
        if ((pHeadset->iNarrating != 0) || !pHeadset->bTextChatAccessibility)
        {
            _VoipHeadsetSendEncodedAudio(pHeadset, aPacketData, iCompBytes);
        }
    }

    // if we have completed the narration encoding, set the narration flag back to inactive
    if ((pHeadset->iNarrating < 0) && (pHeadset->iNarrateLen == 0))
    {
        pHeadset->iNarrating = 0;
    }
}

/*F********************************************************************************/
/*!
    \Function _VoipHeadsetProcessPlayback

    \Description
        Process pulling data out of the mixer to write to the speakers

    \Input *pHeadset    - headset state

    \Version 03/03/2020 (eesponda)
*/
/********************************************************************************F*/
static void _VoipHeadsetProcessPlayback(VoipHeadsetRefT *pHeadset)
{
    size_t uCompBytes = VOIPHEADSET_FRAMESIZE;
    uint8_t *pFrameData = NULL;
    int32_t iResult;

    // only play if we have a playback device open
    if ((pHeadset->SpkrInfo.pStream == NULL) || (pa_stream_get_state(pHeadset->SpkrInfo.pStream) != PA_STREAM_READY))
    {
        return;
    }

    // get a pointer to perform the write on
    if ((iResult = pa_stream_begin_write(pHeadset->SpkrInfo.pStream, (void **)&pFrameData, &uCompBytes)) != 0)
    {
        NetPrintf(("voipheadsetstadia: pa_stream_begin_write returned non-zero result (err=%d)\n", iResult));
        return;
    }

    // decode and mix buffered packet data, we try to pull the data out first before checking chat restrictions
    if (((uCompBytes = VoipMixerProcess(pHeadset->pMixerRef, pFrameData)) == VOIPHEADSET_FRAMESIZE) && !NetConnStatus('chat', pHeadset->iParticipatingUserIndex, NULL, 0))
    {
        // forward data to speaker callback, if callback is specified
        if (pHeadset->pSpkrDataCb != NULL)
        {
            pHeadset->pSpkrDataCb((int16_t *)pFrameData, VOIPHEADSET_FRAMESAMPLES, pHeadset->pSpkrCbUserData);
        }

        if ((iResult = pa_stream_write(pHeadset->SpkrInfo.pStream, pFrameData, uCompBytes, NULL, 0, PA_SEEK_RELATIVE)) != 0)
        {
            NetPrintf(("voipheadsetstadia: pa_stream_write returned non-zero result (err=%d)\n", iResult));
        }
    }
    // otherwise cancel the pending write as we will not be writing anything
    else
    {
        pa_stream_cancel_write(pHeadset->SpkrInfo.pStream);
    }
}

/*F********************************************************************************/
/*!
    \Function _VoipHeadsetAddLocalTalker

    \Description
        Add a local talker to be tracked by the headset

    \Input *pHeadset        - headset state
    \Input iLocalUserIndex  - index of the user being added
    \Input *pLocalUser      - local user state

    \Output
        int32_t             - zero=success, negative=failure

    \Version 03/03/2020 (eesponda)
*/
/********************************************************************************F*/
static int32_t _VoipHeadsetAddLocalTalker(VoipHeadsetRefT *pHeadset, int32_t iLocalUserIndex, const VoipUserT *pLocalUser)
{
    GgpPlayerId uPlayerId;

    // validate the input user
    if (VOIP_NullUser(pLocalUser))
    {
        NetPrintf(("voipheadsetstadia: can't add NULL local talker at local user index %d\n", iLocalUserIndex));
        return(-1);
    }
    // make sure there is no other local talker
    if (!VOIP_NullUser(&pHeadset->aLocalUsers[iLocalUserIndex].User))
    {
        NetPrintf(("voipheadsetstadia: can't add multiple local talkers at local user index %d\n", iLocalUserIndex));
        return(-2);
    }
    // find the local user's player id
    if (NetConnStatus('gpid', iLocalUserIndex, &uPlayerId, sizeof(uPlayerId)) < 0)
    {
        NetPrintf(("voipheadsetstadia: failed to get player_id for user at local user index %d\n", iLocalUserIndex));
        return(-3);
    }
    // copy the local user data and initialize
    VOIP_CopyUser(&pHeadset->aLocalUsers[iLocalUserIndex].User, pLocalUser);
    pHeadset->aLocalUsers[iLocalUserIndex].uPlayerId = uPlayerId;
    pHeadset->aLocalUsers[iLocalUserIndex].uPlaybackFlags = 0xffffffff;

    NetPrintf(("voipheadsetstadia: adding local talker %lld at index %d (player_id: %llu)\n",
        pLocalUser->AccountInfo.iPersonaId, iLocalUserIndex, uPlayerId));
    return(0);
}

/*F********************************************************************************/
/*!
    \Function _VoipHeadsetRemoveLocalTalker

    \Description
        Remove a local talker from being tracked by the headset

    \Input *pHeadset        - headset state
    \Input iLocalUserIndex  - index of the user being removed

    \Output
        int32_t             - zero=success, negative=failure

    \Version 03/03/2020 (eesponda)
*/
/********************************************************************************F*/
static int32_t _VoipHeadsetRemoveLocalTalker(VoipHeadsetRefT *pHeadset, int32_t iLocalUserIndex)
{
    StadiaLocalVoipUserT *pLocalUser;

    if (VOIP_NullUser(&pHeadset->aLocalUsers[iLocalUserIndex].User))
    {
        NetPrintf(("voipheadsetstadia: local user at user index %d is already deleted\n", iLocalUserIndex));
        return(-1);
    }
    pLocalUser = &pHeadset->aLocalUsers[iLocalUserIndex];

    if (pHeadset->iParticipatingUserIndex == iLocalUserIndex)
    {
        NetPrintf(("voipheadsetstadia: cannot delete local talker (player_id: %llu) at local user index %d because the user must first be deactivated\n",
            pLocalUser->uPlayerId, iLocalUserIndex));
        return(-2);
    }

    NetPrintf(("voipheadsetstadia: deleting local talker %lld at index %d (player_id: %llu)\n",
        pLocalUser->User.AccountInfo.iPersonaId, iLocalUserIndex, pLocalUser->uPlayerId));

    VOIP_ClearUser(&pLocalUser->User);
    pLocalUser->uPlayerId = 0;
    pLocalUser->uPlaybackFlags = 0xffffffff;
    return(0);
}

/*F********************************************************************************/
/*!
    \Function _VoipHeadsetConduitPlaybackCb

    \Description
        Determines if we should be submitting audio to the given mixer

    \Input *pMixer          - mixer ref
    \Input *pRemoteUser     - remote user who sent the mic packets
    \Input *pUserData       - callback user data

    \Output
        uint8_t             - TRUE for playback, FALSE for not to playback

    \Version 03/03/2020 (eesponda)
*/
/********************************************************************************F*/
static uint8_t _VoipHeadsetConduitPlaybackCb(VoipMixerRefT *pMixer, VoipUserT *pRemoteUser, void *pUserData)
{
    VoipHeadsetRefT *pHeadset = (VoipHeadsetRefT *)pUserData;
    int32_t iRemoteUserIndex, iLocalUserIndex;

    // find the remote user
    for (iRemoteUserIndex = 0; iRemoteUserIndex < pHeadset->iMaxRemoteUsers; iRemoteUserIndex += 1)
    {
        if (VOIP_SameUser(&pHeadset->aRemoteUsers[iRemoteUserIndex].User, pRemoteUser))
        {
            break;
        }
    }

    // validate the index
    if (iRemoteUserIndex == pHeadset->iMaxRemoteUsers)
    {
        NetPrintf(("voipheadsetstadia: cannot find remote user %lld\n", pRemoteUser->AccountInfo.iPersonaId));
        return(FALSE);
    }

    // loop through all local users to determine if ANY of them have this remote user index muted
    for (iLocalUserIndex = 0; iLocalUserIndex < VOIP_MAXLOCALUSERS; iLocalUserIndex += 1)
    {
        // muting from the +pbk/-pbk selectors
        if ((pHeadset->aLocalUsers[iLocalUserIndex].uPlaybackFlags & (1 << iRemoteUserIndex)) == 0)
        {
            return(FALSE);
        }
    }
    return(TRUE);
}

/*F********************************************************************************/
/*!
    \Function _VoipHeadsetNarrateCb

    \Description
        Callback fired when samples are streamed in from the narration module

    \Input *pNarrate    - narrate state
    \Input iUserIndex   - index of the user who requested narration
    \Input *pSamples    - audio samples that we will playback
    \Input iSize        - size of the sample data in bytes
    \Input *pUserData   - callback user data

    \Output
        int32_t         - amount of audio sample data consumed

    \Version 04/14/2020 (eesponda)
*/
/********************************************************************************F*/
static int32_t _VoipHeadsetNarrateCb(VoipNarrateRefT *pNarrate, int32_t iUserIndex, const int16_t *pSamples, int32_t iSize, void *pUserData)
{
    VoipHeadsetRefT *pHeadset = (VoipHeadsetRefT *)pUserData;
    int32_t iNumSamples;

    // check for start/end of stream
    if (iSize == VOIPNARRATE_STREAM_START)
    {
        pHeadset->iNarrating = 1;

        /* attempt to create the transcription module if it doesn't exist to ensure it is configured correctly. the
           decision to use transcription or not will happen later */
        if ((pHeadset->pTranscribe = _VoipHeadsetTranscribeCreate(pHeadset)) != NULL)
        {
            VoipTranscribeControl(pHeadset->pTranscribe, 'vdis', FALSE, 0, NULL);
        }

        return(iSize);
    }
    else if (iSize == VOIPNARRATE_STREAM_END)
    {
        pHeadset->iNarrating = -1;
        return(iSize);
    }
    // convert to samples for input. output returns samples as well but convert to bytes as is expected by narrate
    iNumSamples = iSize / sizeof(*pSamples);
    return(_VoipHeadsetWriteNarrateInput(pHeadset, pSamples, iNumSamples) * sizeof(*pSamples));
}

/*F********************************************************************************/
/*!
    \Function _VoipHeadsetCreateNarrate

    \Description
        Handles creating / configuring the narration module if it doesn't exist

    \Input *pHeadset    - headset state

    \Output
        int32_t         - zero=success, negative=error

    \Version 04/14/2020 (eesponda)
*/
/********************************************************************************F*/
static int32_t _VoipHeadsetCreateNarrate(VoipHeadsetRefT *pHeadset)
{
    // allow queuing up to 200ms of data; 20ms samples * 10
    const int32_t iNarrateMax = VOIPHEADSET_GetFrameSamples(VOIPNARRATE_SAMPLERATE) * 10;

    /* create the narrate ref if it doesn't exist yet. this allows us to create after given a chance for the module to be configured.
       early out if the narration module has already been created */
    if (pHeadset->pNarrate != NULL)
    {
        return(0);
    }

    // allocate the narration buffer
    if ((pHeadset->pNarrateBuf = (int16_t *)DirtyMemAlloc(sizeof(*pHeadset->pNarrateBuf) * iNarrateMax, VOIP_MEMID, pHeadset->iMemGroup, pHeadset->pMemGroupUserData)) == NULL)
    {
        NetPrintf(("voipheadsetstadia: failed to allocate memory for the narration buffer\n"));
        return(-1);
    }
    pHeadset->iNarrateMax = iNarrateMax;

    // create the narration module
    if ((pHeadset->pNarrate = VoipNarrateCreate(_VoipHeadsetNarrateCb, pHeadset)) == NULL)
    {
        DirtyMemFree(pHeadset->pNarrateBuf, VOIP_MEMID, pHeadset->iMemGroup, pHeadset->pMemGroupUserData);
        return(-1);
    }
    VoipNarrateControl(pHeadset->pNarrate, 'spam', pHeadset->iVerbose, 0, NULL);
    return(0);
}

/*F********************************************************************************/
/*!
    \Function _VoipHeadsetProcessBlockList

    \Description
        Initiates request for the local user's blocklist and processes the response

    \Input *pHeadset    - headset state

    \Version 04/23/2020 (eesponda)
*/
/********************************************************************************F*/
static void _VoipHeadsetProcessBlockList(VoipHeadsetRefT *pHeadset)
{
    GgpStatus Status;
    char strError[128];
    GgpProfile aBlockList[VOIPHEADSET_BLOCKLIST_MAX];
    uint16_t uNumBlocked;
    const uint32_t uNetTick = NetTick();

    // check if it is time to request again
    if (!pHeadset->bPendingBlockList)
    {
        if ((pHeadset->iParticipatingUserIndex != VOIP_INVALID_LOCAL_USER_INDEX) && (NetTickDiff(uNetTick, pHeadset->uNextBlockListCheck) > 0))
        {
            pHeadset->bPendingBlockList = TRUE;
            pHeadset->BlockListAsyncOp = GgpListBlockedPlayers(pHeadset->aLocalUsers[pHeadset->iParticipatingUserIndex].uPlayerId);
        }
        return;
    }

    // check if the results are ready
    if (!GgpFutureIsReady(pHeadset->BlockListAsyncOp))
    {
        return;
    }
    pHeadset->bPendingBlockList = FALSE;

    // update the next check regardless of success
    pHeadset->uNextBlockListCheck = uNetTick + VOIPHEADSET_BLOCKLIST_INTERVAL;

    // get the number of results
    uNumBlocked = GgpFutureGetResultCount(pHeadset->BlockListAsyncOp, &Status);
    if (Status.status_code != kGgpStatusCode_Ok)
    {
        GgpGetStatusMessage(Status.message_token, strError, sizeof(strError));
        NetPrintf(("voipheadsetstadia: failed to get the blocked list count (err=%s)\n", strError));
        return;
    }

    // if we cannot fit all the blocked users, log a warning and clamp to maximum
    if (uNumBlocked > VOIPHEADSET_BLOCKLIST_MAX)
    {
        NetPrintf(("voipheadsetstadia: number of users (%d) in the block list exceeds our maximum (%d)\n", pHeadset->uNumBlocked, VOIPHEADSET_BLOCKLIST_MAX));
        uNumBlocked = VOIPHEADSET_BLOCKLIST_MAX;
    }

    // pull out the results into our allocated array and schedule a new check
    ds_memclr(aBlockList, sizeof(aBlockList));
    if (!GgpFutureGetMultipleResult(pHeadset->BlockListAsyncOp, aBlockList, uNumBlocked * sizeof(*aBlockList), VOIPHEADSET_BLOCKLIST_MAX, NULL, &Status))
    {
        GgpGetStatusMessage(Status.message_token, strError, sizeof(strError));
        NetPrintf(("voipheadsetstadia: failed to get the blocked list result (err=%s)\n", strError));
        return;
    }

    // check if the block list has changed to kick off re-applying the mute flags
    if (memcmp(pHeadset->aBlockList, aBlockList, sizeof(pHeadset->aBlockList)) != 0)
    {
        ds_memcpy(pHeadset->aBlockList, aBlockList, sizeof(pHeadset->aBlockList));
        pHeadset->uNumBlocked = uNumBlocked;
        pHeadset->bMuteListUpdated = TRUE;
    }
}

/*** Public functions *************************************************************/


/*F********************************************************************************/
/*!
    \Function VoipHeadsetCreate

    \Description
        Create the headset manager.

    \Input iMaxConduits     - max number of conduits
    \Input *pMicDataCb      - pointer to user callback to trigger when mic data is ready
    \Input *pTextDataCb     - pointer to user callback to trigger when transcribed text is ready
    \Input *pOpaqueDataCb   - pointer to user callback to trigger when opaque data is ready
    \Input *pStatusCb       - pointer to user callback to trigger when headset status changes
    \Input *pCbUserData     - pointer to user callback data
    \Input iData            - platform-specific - unused for stadia

    \Output
        VoipHeadsetRefT *   - pointer to module state, or NULL if an error occured

    \Version 03/03/2020 (eesponda)
*/
/********************************************************************************F*/
VoipHeadsetRefT *VoipHeadsetCreate(int32_t iMaxConduits, VoipHeadsetMicDataCbT *pMicDataCb, VoipHeadsetTextDataCbT *pTextDataCb, VoipHeadsetOpaqueDataCbT *pOpaqueDataCb, VoipHeadsetStatusCbT *pStatusCb, void *pCbUserData, int32_t iData)
{
    VoipHeadsetRefT *pHeadset;
    int32_t iMemGroup, iSize;
    void *pMemGroupUserData;

    // query mem group data
    VoipCommonMemGroupQuery(&iMemGroup, &pMemGroupUserData);

    // make sure we don't exceed maxconduits
    if (iMaxConduits > VOIP_MAXCONNECT)
    {
        NetPrintf(("voipheadsetstadia: request for %d conduits exceeds max\n", iMaxConduits));
        return(NULL);
    }
    iSize = sizeof(*pHeadset) + (sizeof(*pHeadset->aRemoteUsers) * (iMaxConduits + VOIP_MAX_LOW_LEVEL_CONNS - 1));

    // allocate and clear module state
    if ((pHeadset = DirtyMemAlloc(iSize, VOIP_MEMID, iMemGroup, pMemGroupUserData)) == NULL)
    {
        NetPrintf(("voipheadsetstadia: unable to allocate headset state\n"));
        return(NULL);
    }
    ds_memclr(pHeadset, sizeof(*pHeadset));
    pHeadset->iMemGroup = iMemGroup;
    pHeadset->pMemGroupUserData = pMemGroupUserData;
    pHeadset->iParticipatingUserIndex = VOIP_INVALID_LOCAL_USER_INDEX;
    pHeadset->pMicDataCb = pMicDataCb;
    pHeadset->pTextDataCb = pTextDataCb;
    pHeadset->pOpaqueDataCb = pOpaqueDataCb;
    pHeadset->pStatusCb = pStatusCb;
    pHeadset->pCbUserData = pCbUserData;
    pHeadset->iVerbose = 1;

    // allocate the conduit manager
    if ((pHeadset->pConduitRef = VoipConduitCreate(iMaxConduits)) == NULL)
    {
        NetPrintf(("voipheadsetstadia: unable to allocate conduit manager\n"));
        DirtyMemFree(pHeadset, VOIP_MEMID, iMemGroup, pMemGroupUserData);
        return(NULL);
    }
    pHeadset->iMaxConduits = iMaxConduits;
    pHeadset->iMaxRemoteUsers = iMaxConduits + VOIP_MAX_LOW_LEVEL_CONNS;
    VoipConduitRegisterPlaybackCb(pHeadset->pConduitRef, _VoipHeadsetConduitPlaybackCb, pHeadset);

    // allocate the mixer
    if ((pHeadset->pMixerRef = VoipMixerCreate(16, VOIPHEADSET_FRAMESAMPLES)) == NULL)
    {
        NetPrintf(("voipheadsetstadia: unable to allocate mixer\n"));
        VoipConduitDestroy(pHeadset->pConduitRef);
        DirtyMemFree(pHeadset, VOIP_MEMID, iMemGroup, pMemGroupUserData);
        return(NULL);
    }
    VoipConduitMixerSet(pHeadset->pConduitRef, pHeadset->pMixerRef);

    // create the event queue for the microphone event callbacks
    pHeadset->EventQueue = GgpEventQueueCreate();

    // add the hotplug event
    if ((pHeadset->uMicHotplug = GgpAddMicrophoneHotplugHandler(pHeadset->EventQueue, _VoipHeadsetMicrophoneHotplug, pHeadset, NULL, NULL)) == 0)
    {
        NetPrintf(("voipheadsetstadia: failed to add the microphone hotplug handler\n"));
        VoipHeadsetDestroy(pHeadset);
        return(NULL);
    }

    // add the microphone state changed handler
    if ((pHeadset->uMicStateChanged = GgpAddMicrophoneStateChangedHandler(pHeadset->EventQueue, _VoipHeadsetMicStateChanged, pHeadset, NULL, NULL)) == 0)
    {
        NetPrintf(("voipheadsetstadia: failed to add the microphone state changed handler\n"));
        VoipHeadsetDestroy(pHeadset);
        return(NULL);
    }

    // add the microphone audio state changed handler
    if ((pHeadset->uMicAudioStateChanged = GgpAddMicAudioStateChangedHandler(pHeadset->EventQueue, _VoipHeadsetMicAudioStateChanged, pHeadset, NULL, NULL)) == 0)
    {
        NetPrintf(("voipheadsetstadia: failed to add the microphone audio state changed handler\n"));
        VoipHeadsetDestroy(pHeadset);
        return(NULL);
    }

    // add the audio endpoint changed handler
    if ((pHeadset->uAudioEndpointChanged = GgpAddAudioEndpointChangedHandler(pHeadset->EventQueue, _VoipHeadsetAudioEndpointChanged, pHeadset, NULL, NULL)) == 0)
    {
        NetPrintf(("voipheadsetstadia: failed to add the audio endpoint changed handler\n"));
        VoipHeadsetDestroy(pHeadset);
        return(NULL);
    }

    /* setup the single-threaded mainloop to interface into pulseaudio. synchronization is not required as the documentation states: "This implementation is thread
       safe as long as you access the main loop object from a single thread only." Given that we only interface with this API from the VoIP thread this assertion is true.
       there exists a threaded version of the mainloop but we choose not to spawn another thread and using it would increase the complexity of the associated code. */
    if ((pHeadset->pPulseAudio = pa_mainloop_new()) == NULL)
    {
        NetPrintf(("voipheadsetstadia: failed to allocate the pulseaudio state\n"));
        VoipHeadsetDestroy(pHeadset);
        return(NULL);
    }

    // return module ref to caller
    return(pHeadset);
}

/*F********************************************************************************/
/*!
    \Function VoipHeadsetDestroy

    \Description
        Destroy the headset manager.

    \Input *pHeadset    - pointer to headset state

    \Version 03/03/2020 (eesponda)
*/
/********************************************************************************F*/
void VoipHeadsetDestroy(VoipHeadsetRefT *pHeadset)
{
    // cleanup narration module
    if (pHeadset->pNarrate != NULL)
    {
        VoipNarrateDestroy(pHeadset->pNarrate);
        DirtyMemFree(pHeadset->pNarrateBuf, VOIP_MEMID, pHeadset->iMemGroup, pHeadset->pMemGroupUserData);
    }

    // cleanup the pulseaudio module state
    if (pHeadset->pPulseAudio != NULL)
    {
        pa_mainloop_free(pHeadset->pPulseAudio);
    }

    // unregister the handlers
    if (pHeadset->uAudioEndpointChanged != 0)
    {
        GgpRemoveAudioEndpointChangedHandler(pHeadset->uAudioEndpointChanged, NULL);
    }
    if (pHeadset->uMicAudioStateChanged != 0)
    {
        GgpRemoveMicAudioStateChangedHandler(pHeadset->uMicAudioStateChanged, NULL);
    }
    if (pHeadset->uMicStateChanged != 0)
    {
        GgpRemoveMicrophoneStateChangedHandler(pHeadset->uMicStateChanged, NULL);
    }
    if (pHeadset->uMicHotplug != 0)
    {
        GgpRemoveMicrophoneHotplugHandler(pHeadset->uMicHotplug, NULL);
    }

    VoipMixerDestroy(pHeadset->pMixerRef);
    VoipConduitDestroy(pHeadset->pConduitRef);
    VoipCodecDestroy();
    DirtyMemFree(pHeadset, VOIP_MEMID, pHeadset->iMemGroup, pHeadset->pMemGroupUserData);
}

/*F********************************************************************************/
/*!
    \Function VoipHeadsetReceiveVoiceDataCb

    \Description
        Connectionlist callback to handle receiving a voice packet from a remote peer.

    \Input *pRemoteUsers    - user we're receiving the voice data from
    \Input iRemoteUserSize  - pRemoteUsers array size
    \Input iConsoleId       - generic identifier for the console to which the users belong
    \Input *pMicrInfo       - micr info from inbound packet
    \Input *pPacketData     - pointer to beginning of data in packet payload
    \Input *pUserData       - VoipHeadsetT ref

    \Version 03/03/2020 (eesponda)
*/
/********************************************************************************F*/
void VoipHeadsetReceiveVoiceDataCb(VoipUserT *pRemoteUsers, int32_t iRemoteUserSize, int32_t iConsoleId, VoipMicrInfoT *pMicrInfo, uint8_t *pPacketData, void *pUserData)
{
    uint32_t uMicrPkt;
    int32_t iRemoteUserIndex;
    VoipHeadsetRefT *pHeadset = (VoipHeadsetRefT *)pUserData;

    // validate subpacket size if we are dealign with a fixed-length scenario
    if ((pMicrInfo->uSubPacketSize != 0xFF) && (pMicrInfo->uSubPacketSize != pHeadset->iCmpFrameSize))
    {
        NetPrintf(("voipheadsetstadia: discarding voice packet with %d voice bundles and mismatched sub-packet size %d (expecting: %d)\n",
            pMicrInfo->uNumSubPackets, pMicrInfo->uSubPacketSize, pHeadset->iCmpFrameSize));
        return;
    }

    // if this is the shared user index
    if (pMicrInfo->uUserIndex == VOIP_SHARED_REMOTE_INDEX)
    {
        // find the first valid user to playback the audio
        for (iRemoteUserIndex = 0; iRemoteUserIndex < iRemoteUserSize; iRemoteUserIndex += 1)
        {
            if (!VOIP_NullUser(&pRemoteUsers[iRemoteUserIndex]))
            {
                break;
            }
        }

        // didn't find a remote user
        if (iRemoteUserIndex == iRemoteUserSize)
        {
            NetPrintf(("voipheadsetstadia: discarding voice packet from shared user because we cannot find a remote user to play it back as\n"));
            return;
        }
    }
    else
    {
        iRemoteUserIndex = pMicrInfo->uUserIndex;
    }

    // submit voice sub-packets
    for (uMicrPkt = 0; uMicrPkt < pMicrInfo->uNumSubPackets; uMicrPkt += 1)
    {
        // get the size of the sub-packet based on if variable-length or not
        const uint32_t uSubPacketSize = (pMicrInfo->uSubPacketSize != 0xFF) ? pMicrInfo->uSubPacketSize : *pPacketData++;

        // send it to conduit manager
        VoipConduitReceiveVoiceData(pHeadset->pConduitRef, &pRemoteUsers[iRemoteUserIndex], pPacketData, uSubPacketSize);

        // move to the next packet
        pPacketData += uSubPacketSize;
    }
}

/*F********************************************************************************/
/*!
    \Function VoipHeadsetRegisterUserCb

    \Description
        Connectionlist callback to register/unregister a new user with the
        VoipConduit module.

    \Input *pRemoteUser - user to register
    \Input iConsoleId   - generic identifier for the console to which the user belongs (ignored)
    \Input bRegister    - true=register, false=unregister
    \Input *pUserData   - voipheadset module ref

    \Version 03/03/2020 (eesponda)
*/
/********************************************************************************F*/
void VoipHeadsetRegisterUserCb(VoipUserT *pRemoteUser, int32_t iConsoleId, uint32_t bRegister, void *pUserData)
{
    int32_t iRemoteUserIndex;
    VoipHeadsetRefT *pHeadset = (VoipHeadsetRefT *)pUserData;

    if (VOIP_NullUser(pRemoteUser))
    {
        return;
    }

    if (bRegister)
    {
        // make sure we have enough space to add a new user
        if (pHeadset->iNumRemoteUsers == pHeadset->iMaxRemoteUsers)
        {
            NetPrintf(("voipheadsetstadia: could not add remote user %lld as the list if full\n", pRemoteUser->AccountInfo.iPersonaId));
            return;
        }

        // find a duplicate entry
        for (iRemoteUserIndex = 0; iRemoteUserIndex < pHeadset->iMaxRemoteUsers; iRemoteUserIndex += 1)
        {
            if (!VOIP_SameUser(&pHeadset->aRemoteUsers[iRemoteUserIndex].User, pRemoteUser))
            {
                continue;
            }

            // handle this failure case
            NetPrintf(("voipheadsetstadia: could not add remote user %lld as it already exists in the list\n", pRemoteUser->AccountInfo.iPersonaId));
            return;
        }

        // find an empty remote user entry
        for (iRemoteUserIndex = 0; iRemoteUserIndex < pHeadset->iMaxRemoteUsers; iRemoteUserIndex += 1)
        {
            if (!VOIP_NullUser(&pHeadset->aRemoteUsers[iRemoteUserIndex].User))
            {
                continue;
            }

            // make sure the first party id callback is registered
            if (pHeadset->pFirstPartyIdCb == NULL)
            {
                NetPrintf(("voipheadsetstadia: cannot register remote user as first party id callback is not registered which is required for voip to function\n"));
                return;
            }

            // copy the user data
            ds_memcpy(&pHeadset->aRemoteUsers[iRemoteUserIndex].User, pRemoteUser, sizeof(*pRemoteUser));
            pHeadset->aRemoteUsers[iRemoteUserIndex].uPlayerId = pHeadset->pFirstPartyIdCb(pRemoteUser->AccountInfo.iPersonaId, pHeadset->pFirstPartyIdCbUserData);
            // register with the conduit
            VoipConduitRegisterUser(pHeadset->pConduitRef, pRemoteUser, TRUE);
            // increment our count and exit
            pHeadset->iNumRemoteUsers += 1;
            NetPrintf(("voipheadsetstadia: adding remote user %lld at remote user index %d (num users = %d)\n", pRemoteUser->AccountInfo.iPersonaId, iRemoteUserIndex, pHeadset->iNumRemoteUsers));
            break;
        }
    }
    else
    {
        // find the remote user entry
        for (iRemoteUserIndex = 0; iRemoteUserIndex < pHeadset->iMaxRemoteUsers; iRemoteUserIndex += 1)
        {
            if (!VOIP_SameUser(&pHeadset->aRemoteUsers[iRemoteUserIndex].User, pRemoteUser))
            {
                continue;
            }

            // clear the user data
            ds_memclr(&pHeadset->aRemoteUsers[iRemoteUserIndex], sizeof(*pHeadset->aRemoteUsers));
            // unregister with the conduit
            VoipConduitRegisterUser(pHeadset->pConduitRef, pRemoteUser, FALSE);
            // decrement our count and exit
            pHeadset->iNumRemoteUsers -= 1;
            NetPrintf(("voipheadsetstadia: removing remote user %lld at remote user index %d (num users = %d)\n", pRemoteUser->AccountInfo.iPersonaId, iRemoteUserIndex, pHeadset->iNumRemoteUsers));
            break;
        }
    }
}

/*F********************************************************************************/
/*!
    \Function VoipHeadsetProcess

    \Description
        Headset process function.

    \Input *pHeadset    - pointer to headset state
    \Input uFrameCount  - process iteration counter

    \Version 03/03/2020 (eesponda)
*/
/********************************************************************************F*/
void VoipHeadsetProcess(VoipHeadsetRefT *pHeadset, uint32_t uFrameCount)
{
    // process any events that are ready in the queue
    while (GgpEventQueueProcess(pHeadset->EventQueue, 0))
        ;

    // process the audio loop
    pa_mainloop_iterate(pHeadset->pPulseAudio, 0, NULL);

    // process the block list
    _VoipHeadsetProcessBlockList(pHeadset);

    // process narration
    if (pHeadset->pNarrate != NULL)
    {
        VoipNarrateUpdate(pHeadset->pNarrate);
    }

    // process transcription
    _VoipHeadsetProcessTranscription(pHeadset);

    // process the record
    _VoipHeadsetProcessRecord(pHeadset);

    // process the playback
    _VoipHeadsetProcessPlayback(pHeadset);
}

/*F********************************************************************************/
/*!
    \Function VoipHeadsetSetVolume

    \Description
        Sets play and record volume.

    \Input *pHeadset    - pointer to headset state
    \Input iPlayVol     - play volume to set
    \Input iRecVol      - record volume to set

    \Notes
        To not set a value, specify it as -1.

    \Version 03/03/2020 (eesponda)
*/
/********************************************************************************F*/
void VoipHeadsetSetVolume(VoipHeadsetRefT *pHeadset, int32_t iPlayVol, uint32_t iRecVol)
{
}

/*F********************************************************************************/
/*!
    \Function VoipHeadsetConfigNarration

    \Description
        Configure the narrate module

    \Input *pHeadset    - headset module state
    \Input uProvider    - narrate provider
    \Input *pUrl        - narrate url
    \Input *pKey        - narrate key

    \Version 04/07/2020 (eesponda)
*/
/********************************************************************************F*/
void VoipHeadsetConfigNarration(VoipHeadsetRefT *pHeadset, uint32_t uProvider, const char *pUrl, const char *pKey)
{
    VoipNarrateConfig(uProvider, pUrl, pKey);
}

/*F********************************************************************************/
/*!
    \Function VoipHeadsetConfigTranscription

    \Description
        Configure the transcription module

    \Input *pHeadset    - headset module state
    \Input uProvider    - narrate provider
    \Input *pUrl        - narrate url
    \Input *pKey        - narrate key

    \Version 04/16/2020 (eesponda)
*/
/********************************************************************************F*/
void VoipHeadsetConfigTranscription(VoipHeadsetRefT *pHeadset, uint32_t uProvider, const char *pUrl, const char *pKey)
{
    VoipTranscribeConfig(uProvider, pUrl, pKey);
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
        '+pbk' - enable the playback for a remote user (iValue=local user index, pValue=remote user)
        '-pbk' - disable the playback for a remote user (iValue=local user index, pValue=remote user)
        'aloc' - promote/demote to/from participating state
        'cdec' - create a codec based on the identier
        'creg' - register a codec with an identifier
        'cstm' - clear speech to text metrics in VoipSpeechToTextMetricsT
        'ctsm' - clear text to speech metrics in VoipTextToSpeechMetricsT
        'loop' - enable/disable loopback
        'mute' - enable/disable audio input muting
        'rloc' - register/unregister local user
        'txta' - enable/disable text chat accessibility, this also controls loopback in this context
        'tran' - enable/disable local generation of transcribed text for speech produced by local users (speech-to-text component)
        'spam' - set verbosity level (default=1)
        \endverbatim

    \Version 03/03/2020 (eesponda)
*/
/********************************************************************************F*/
int32_t VoipHeadsetControl(VoipHeadsetRefT *pHeadset, int32_t iControl, int32_t iValue, int32_t iValue2, void *pValue)
{
    if ((iControl == '+pbk') || (iControl == '-pbk'))
    {
        int32_t iRemoteUserIndex;
        const VoipUserT *pRemoteUser = (const VoipUserT *)pValue;
        const uint8_t bPlaybackEnabled = (iControl == '+pbk');

        if ((pValue == NULL) || (iValue < 0) || (iValue >= VOIP_MAXLOCALUSERS))
        {
            NetPrintf(("voipheadsetstadia: VoipHeadsetControl('%C') failed due to invalid inputs\n", iControl));
            return(-1);
        }

        // find the remote user index
        for (iRemoteUserIndex = 0; iRemoteUserIndex < pHeadset->iMaxRemoteUsers; iRemoteUserIndex += 1)
        {
            if (VOIP_SameUser(&pHeadset->aRemoteUsers[iRemoteUserIndex].User, pRemoteUser))
            {
                break;
            }
        }

        // validate remote user index
        if (iRemoteUserIndex == pHeadset->iMaxRemoteUsers)
        {
            NetPrintf(("voipheadsetstadia: VoipHeadsetControl('%C') cannot find the remote user %lld\n", iControl, pRemoteUser->AccountInfo.iPersonaId));
            return(-1);
        }

        // modify the playback flags
        if (bPlaybackEnabled)
        {
            pHeadset->aLocalUsers[iValue].uPlaybackFlags |= (1 << iRemoteUserIndex);
        }
        else
        {
            pHeadset->aLocalUsers[iValue].uPlaybackFlags &= ~(1 << iRemoteUserIndex);
        }
        NetPrintfVerbose((pHeadset->iVerbose, 0, "voipheadsetstadia: voice playback %s for remote user index %d, bit field 0x%08x\n",
                bPlaybackEnabled ? "enabled" : "disabled", iRemoteUserIndex, pHeadset->aLocalUsers[iValue].uPlaybackFlags));
        return(0);
    }
    if (iControl == 'aloc')
    {
        int32_t iLocalUserIndex;
        GgpStatus Status;
        char strError[128];

        // set the mic listening state
        GgpEnableVoiceChatEndpoint(iValue2 != 0);
        if (!GgpSetMicAudioListening(pHeadset->aLocalUsers[iValue].uPlayerId, (iValue2 != 0), &Status))
        {
            GgpGetStatusMessage(Status.message_token, strError, sizeof(strError));
            NetPrintf(("voipheadsetstadia: failed to set the mic listening state (code=%d, msg=%s)\n", Status.status_code, strError));
            return(-1);
        }

        pHeadset->iParticipatingUserIndex = (iValue2 == 0) ? VOIP_INVALID_LOCAL_USER_INDEX : iValue;
        NetPrintf(("voipheadsetstadia: user %d, %s the participating state\n", iValue, (iValue2 == 0) ? "exiting" : "entering"));

        // set the time to check the block list as soon as possible
        pHeadset->uNextBlockListCheck = NetTick();

        // whenever active user status changes all mute masks will be re-calculated so lets reset our state
        for (iLocalUserIndex = 0; iLocalUserIndex < VOIP_MAXLOCALUSERS; iLocalUserIndex += 1)
        {
            pHeadset->aLocalUsers[iLocalUserIndex].uPlaybackFlags = 0xFFFFFFFF;
        }

        return(0);
    }
    if (iControl == 'cdec')
    {
        int32_t iResult;

        // pass through codec creation request
        if ((iResult = VoipCodecCreate(iValue, pHeadset->iMaxConduits)) < 0)
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
        return((pHeadset->pTranscribe != NULL) ? VoipTranscribeControl(pHeadset->pTranscribe, iControl, iValue, 0, NULL) : -1);
    }
    if (iControl == 'ctsm')
    {
        return((pHeadset->pNarrate != NULL) ? VoipNarrateControl(pHeadset->pNarrate, iControl, 0, 0, NULL) : -1);
    }
    if (iControl == 'loop')
    {
        pHeadset->bLoopback = iValue;
        NetPrintf(("voipheadsetstadia: loopback mode %s\n", (iValue ? "enabled" : "disabled")));
        return(0);
    }
    if ((iControl == 'mute') && (iValue < VOIP_MAXLOCALUSERS) && (pHeadset->aLocalUsers[iValue].uPlayerId != 0))
    {
        // get the current mute state
        const uint8_t bMuted = GgpGetMicAudioMuteState(pHeadset->aLocalUsers[iValue].uPlayerId, NULL) == kGgpMicAudioMuteState_Muted;

        // return if the state change is no-op or if we are in loopback mode
        if ((bMuted == iValue2) || pHeadset->bLoopback)
        {
            return(0);
        }

        // attempt to set the mute; if it fails we don't want to log to prevent spam
        if (GgpSetMicAudioMute(pHeadset->aLocalUsers[iValue].uPlayerId, iValue2, NULL))
        {
            NetPrintfVerbose((pHeadset->iVerbose, 1, "voipheadsetstadia: mute %s\n", iValue2 ? "enabled" : "disabled"));
            return(0);
        }
        return(-1);
    }
    if (iControl == 'rloc')
    {
        const VoipUserT *pLocalUser = (VoipUserT *)pValue;
        return(iValue2 ? _VoipHeadsetAddLocalTalker(pHeadset, iValue, pLocalUser)
                       : _VoipHeadsetRemoveLocalTalker(pHeadset, iValue));
    }
    if (iControl == 'spam')
    {
        pHeadset->iVerbose = iValue;
        VoipConduitControl(pHeadset->pConduitRef, iControl, iValue, pValue);
        if (pHeadset->pNarrate != NULL)
        {
            VoipNarrateControl(pHeadset->pNarrate, iControl, iValue, 0, NULL);
        }
        return(0);
    }
    if (iControl == 'txta')
    {
        pHeadset->bTextChatAccessibility = pHeadset->bLoopback = iValue;
        NetPrintf(("voipheadsetstadia: text chat accessibility mode %s\n", (iValue ? "enabled" : "disabled")));
        return(0);
    }
    if (iControl == 'tran')
    {
        pHeadset->bVoiceTranscriptionEnabled = iValue;
        NetPrintf(("voipheadsetstadia: %s voice transcription locally\n", iValue ? "enabling" : "disabling"));
        return(0);
    }
    if ((iControl == 'ttos') && (iValue < VOIP_MAXLOCALUSERS))
    {
        return((_VoipHeadsetCreateNarrate(pHeadset) == 0) ? VoipNarrateInput(pHeadset->pNarrate, iValue, pHeadset->uNarrationGender, (const char *)pValue) : -1);
    }
    if ((iControl == 'voic') && (pValue != NULL))
    {
        const VoipSynthesizedSpeechCfgT *pCfg = (const VoipSynthesizedSpeechCfgT *)pValue;
        pHeadset->uNarrationGender = (pCfg->iPersonaGender == 1) ? VOIPNARRATE_GENDER_FEMALE : VOIPNARRATE_GENDER_MALE;
        NetPrintf(("voipnarratestadia: setting tts voice to %s\n", (pHeadset->uNarrationGender == VOIPNARRATE_GENDER_MALE) ? "male" : "female"));
        return(0);
    }
    return(-1);
}

/*F********************************************************************************/
/*!
    \Function VoipHeadsetStatus

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
            'fpml' - return the first party mute setting between a local user (iValue) and a remote user (VoipUserT * in pBuf)
            'fpmu' - return TRUE if the first party mute list was updated
            'ruvu' - always return TRUE (as long as user is active) because MLU is not currently supported on stadia
            'sttm' - get the VoipSpeechToTextMetricsT via pBuf
            'ttos' - returns TRUE if we are currently narrating
            'ttsm' - get the VoipTextToSpeechMetricsT via pBuf
            'xply' - always return FALSE as we don't implement crossplay at this time
        \endverbatim

    \Version 03/03/2020 (eesponda)
*/
/********************************************************************************F*/
int32_t VoipHeadsetStatus(VoipHeadsetRefT *pHeadset, int32_t iSelect, int32_t iValue, void *pBuf, int32_t iBufSize)
{
    if (iSelect == 'fpml')
    {
        int32_t iRemoteUserIndex;
        uint16_t uBlockListIndex;

        // try to find the user in the list of remote users
        for (iRemoteUserIndex = 0; iRemoteUserIndex < pHeadset->iMaxRemoteUsers; iRemoteUserIndex += 1)
        {
            if (VOIP_SameUser(&pHeadset->aRemoteUsers[iRemoteUserIndex].User, (VoipUserT *)pBuf))
            {
                break;
            }
        }
        if (iRemoteUserIndex == pHeadset->iMaxRemoteUsers)
        {
            NetPrintf(("voipheadsetstadia: could not find remote user in list of registered users\n"));
            return(-1);
        }

        // try to find user in list of blocked users
        for (uBlockListIndex = 0; uBlockListIndex < pHeadset->uNumBlocked; uBlockListIndex += 1)
        {
            if (pHeadset->aBlockList[uBlockListIndex].player_id == pHeadset->aRemoteUsers[iRemoteUserIndex].uPlayerId)
            {
                return(TRUE);
            }
        }
        return(FALSE);
    }
    if (iSelect == 'fpmu')
    {
        if (pHeadset->bMuteListUpdated)
        {
            pHeadset->bMuteListUpdated = FALSE;
            return(TRUE);
        }
        return(FALSE);
    }
    if (iSelect == 'ruvu')
    {
        int32_t iRemoteUserSpaceIndex = 0;

        for (iRemoteUserSpaceIndex = 0; iRemoteUserSpaceIndex < pHeadset->iMaxRemoteUsers; iRemoteUserSpaceIndex += 1)
        {
            if (VOIP_SameUser(&pHeadset->aRemoteUsers[iRemoteUserSpaceIndex].User, (VoipUserT *)pBuf))
            {
                // remote user found and it is registered with voipheadset
                return(TRUE);
            }
        }
        return(FALSE);
    }
    if (iSelect == 'sttm')
    {
        return((pHeadset->pTranscribe != NULL) ? VoipTranscribeStatus(pHeadset->pTranscribe, iSelect, iValue, pBuf, iBufSize) : -1);
    }
    if (iSelect == 'ttos')
    {
        return(pHeadset->iNarrating != 0);
    }
    if (iSelect == 'ttsm')
    {
        return((pHeadset->pNarrate != NULL) ? VoipNarrateStatus(pHeadset->pNarrate, iSelect, iValue, pBuf, iBufSize) : -1);
    }
    if (iSelect == 'xply')
    {
        // we don't support crossplay at this time
        return(FALSE);
    }
    // unhandled result, fallthrough to the codec
    return(VoipCodecStatus(VOIP_CODEC_ACTIVE, iSelect, iValue, pBuf, iBufSize));
}

/*F********************************************************************************/
/*!
    \Function VoipHeadsetSpkrCallback

    \Description
        Set speaker output callback.

    \Input *pHeadset    - headset module state
    \Input *pCallback   - what to call when output data is available
    \Input *pUserData   - user data for callback

    \Version 03/03/2020 (eesponda)
*/
/********************************************************************************F*/
void VoipHeadsetSpkrCallback(VoipHeadsetRefT *pHeadset, VoipSpkrCallbackT *pCallback, void *pUserData)
{
    pHeadset->pSpkrDataCb = pCallback;
    pHeadset->pSpkrCbUserData = pUserData;
}

/*F********************************************************************************/
/*!
    \Function VoipHeadsetSetFirstPartyIdCallback

    \Description
        callback to tell dirtysdk what the first party id is for this user

    \Input *pHeadset    - headset module state
    \Input *pCallback   - what to call when transcribed text is received from remote player
    \Input *pUserData   - user data for callback

    \Version 04/22/2020 (eesponda)
*/
/********************************************************************************F*/
void VoipHeadsetSetFirstPartyIdCallback(VoipHeadsetRefT *pHeadset, VoipFirstPartyIdCallbackCbT *pCallback, void *pUserData)
{
    pHeadset->pFirstPartyIdCb = pCallback;
    pHeadset->pFirstPartyIdCbUserData = pUserData;
}
