/*H********************************************************************************/
/*!
    \File voipheadsetps4.c

    \Description
        VoIP headset manager.

    \Copyright
        Copyright (c) Electronic Arts 2013. ALL RIGHTS RESERVED.

    \Version 01/16/2013 (mclouatre) First Version
*/
/********************************************************************************H*/

/*** Include files ****************************************************************/

#include <string.h>
#include <stdlib.h>

#include <scebase_common.h>
#include <voice.h>
#include <audioin.h>    // for SCE_AUDIO_IN_TYPE_VOICE
#include <audioout.h>   // for SCE_AUDIO_OUT_PORT_TYPE_VOICE
#include <np/np_common.h>
#include <user_service.h>

// dirtysock includes
#include "DirtySDK/platform.h"

/* internal API exposed for us by Sony, in PlayStation4AudioInEx package
| [Description]
| This is a function to know whether the device associated with the specified handle is a shared device or not.
| Specifically, function that returns whether the device at the port is a Camera Microphone or not.
| The return value is TRUE (= 1) or FALSE (= 0), If it is an invalid handle value, an error (negative value) returned. */
#ifndef DIRTYCODE_PS5
extern int32_t sceAudioInIsSharedDevice(int32_t handle);
#endif

#include "DirtySDK/dirtysock.h"
#include "DirtySDK/dirtysock/dirtymem.h"
#include "DirtySDK/dirtysock/dirtyerr.h"
#include "DirtySDK/dirtysock/netconn.h"
#include "DirtySDK/dirtysock/dirtythread.h"
#include "DirtySDK/misc/userlistapi.h"

// voip includes
#include "DirtySDK/voip/voipdef.h"
#include "DirtySDK/voip/voipcodec.h"
#include "DirtySDK/voip/voipnarrate.h"
#include "DirtySDK/voip/voiptranscribe.h"
#include "voippriv.h"
#include "voipcommon.h"
#include "voippacket.h"
#include "voipheadset.h"

/*** Defines **********************************************************************/

#define VOIPHEADSET_INVALID_INDEX                       (-1)
#define VOIPHEADSET_DECODER_OWNERSHIP_AGING             (1 * 1000)  // user-to-decoder owner relationship broken after 1 sec of inactivity
#define VOIPHEADSET_DEBUG                               (DIRTYCODE_DEBUG && FALSE)

#define VOIPHEADSET_SAMPLERATE                          (SCE_VOICE_BITRATE_16000)                                   //!< sample rate; 16khz audio
#define VOIPHEADSET_SAMPLEWIDTH                         (2)                                                         //!< sample size; 16-bit samples
#define VOIPHEADSET_FRAMEDURATION                       (20)                                                        //!< frame duration in milliseconds; 20ms
#define VOIPHEADSET_FRAMESAMPLES                        ((VOIPHEADSET_SAMPLERATE*VOIPHEADSET_FRAMEDURATION)/1000)   //!< samples per frame (20ms; 8khz=160, 11.025khz=220.5, 16khz=320)
#define VOIPHEADSET_FRAMESIZE                           (VOIPHEADSET_FRAMESAMPLES*VOIPHEADSET_SAMPLEWIDTH)          //!< frame size in bytes; 640

#define VOIPHEADSET_BLOCKLIST_INTERVAL                  (15*1000)                                                   //!< TRCs don't specify how long we need to handle this in
#define VOIPHEADSET_BLOCKLIST_MAX                       (2000)                                                      //!< maximum size blocklist we support; this aligns with the maximums on other platforms

/*** Macros ***********************************************************************/

/*** Type Definitions *************************************************************/

//! narrate callback type
typedef struct VoipHeadsetNarrateCbDataT
{
    VoipHeadsetRefT *pHeadset;
    int32_t iLocalUserIndex;
} VoipHeadsetNarrateCbDataT;

//! decoder data space (used to wrap a lib voice IN_VOICE port)
typedef struct
{
    uint32_t uDecoderPortId;        //!< id of the IN_VOICE port wrapped by this entry
    uint32_t uPcmPortId;            //!< id of the IN_PCMAUDIO port connected to decoder when employing non-native voip codec
    uint32_t uLastUsage;            //!< tick for last usage of the IN_VOICE port by the owner
    uint32_t uConnectedSpkrPort;    //!< keep tracks of which local user's speaker port is connected (1 user per bit)
    int32_t iOwnerIndex;            //!< local user index or remote user space index  (discriminated with bOwnerRemote;  VOIPHEADSET_INVALID_INDEX if not used)
    uint8_t bOwnerRemote;           //!< whether the owner is a remote user or a local user
    uint8_t bValid;                 //!< whether the other fields in this structure are valid or not
    uint8_t _pad[2];
} PS4DecoderT;

//! local user data space
typedef struct
{
    VoipUserT user;                 //!< voip user identifier
    SceUserServiceUserId sceUserId; //!< sce user identifier
    VoipTranscribeRefT *pTranscribeRef;     //!< voip transcription module ref
    VoipNarrateRefT *pNarrateRef;           //!< voip narration module ref
    int32_t iLocalUserIndex;        //!< local user index; matches index in local user array

    int32_t iAudioThreadId;         //!< audio thread identifier
    volatile uint32_t uMicPortId;   //!< id of the IN_DEVICE port associated with this local user; must be volatile due to threaded access
    uint32_t uPcmPortId;            //!< id of the IN_PCMDEVICE port associated with this local user, connected to encoder
    uint32_t uEncoderPortId;        //!< id of the OUT_VOICE port associated with this local user
    uint32_t uSpkrPortId;           //!< id of the OUT_DEVICE port associated with this local user 
    int32_t iDecoderIndex;          //!< index of decoder used in the pool of decoders  (VOIPHEADSET_INVALID_INDEX if no such association exists)
    int32_t iVadSilenceSamples;     //!< number of consecutive samples of silence recorded from this user

    uint8_t bParticipating;         //!< TRUE if this user is a participating user, FALSE if not
    int8_t iNarrating;              //!< narration status; zero=inactive, one=narrating, negative=done
    uint8_t uGender;                //!< text-to-speech gender (VOIPNARRATE_GENDER_*); defaults to neutral (usually male-sounding)
    uint8_t bMuted;                 //!< TRUE if muted (e.g. push-to-talk), else FALSE

    int32_t iEncAudioSamples;       //!< tracks audio samples submitted to voipcodec encoder since last silence
    int32_t uEncAudioTick;          //!< tick when audio started

    SceNpAccountId aBlockList[VOIPHEADSET_BLOCKLIST_MAX]; //!< blocked users
    SceNpAccountId aPendingBlockList[VOIPHEADSET_BLOCKLIST_MAX]; //!< blocked users (staging point while we pull all the data)
    int16_t iNumBlocked;            //!< current number of blocked users
    int16_t iPendingNumBlocked;     //!< current number of blocked users in our pending list
    uint8_t bPendingBlockList;      //!< is the block list request still pending?
    uint8_t bMuteListUpdated;       //!< TRUE if the mute list has been updated
    uint8_t _pad[2];
    uint32_t uNextBlockListCheck;   //!< when should we kick off a block list check

    int32_t iAudioLen;              //!< amount of data in audio buffer
    int32_t iAudioInp;              //!< write pointer; head of circular buffer
    int32_t iAudioOut;              //!< read pointer; tail of circular buffer
    uint8_t aAudioData[8*1024];     //!< buffer up to 4k samples (~256ms at 16khz)  //$$todo - this can probably be reduced
} PS4LocalVoipUserT;

//! remote user data space
typedef struct
{
    VoipUserT user;
    int32_t iDecoderIndex;          //!< index of decoder used in the pool of decoders  (VOIPHEADSET_INVALID_INDEX if no such association exists)
    uint32_t uEnablePlayback;       //!< bit field use to disable or enable playback of voice for a specific remote user (1 user per bit)
    SceNpAccountId uAccountId;      //!< account id for remote sony platforms
} PS4RemoteVoipUserT;

//! voipheadset module state data
struct VoipHeadsetRefT
{
    void *pMemGroupUserData;
    int32_t iMemGroup;

    // microphone status
    uint32_t uLastMicStatus;        //!< bit field capturing last reported microphone status (on/off) for each local player

    //! channel info
    int32_t iMaxChannels;

    //! maximum number of remote users
    int32_t iMaxRemoteUsers;
   
    //! encoding config (bps)
    int32_t iEncoderBps;

    //! user callback data
    VoipHeadsetMicDataCbT *pMicDataCb;
    VoipHeadsetTextDataCbT *pTextDataCb;
    VoipHeadsetStatusCbT *pStatusCb;
    void *pCbUserData;

    VoipHeadsetTranscribedTextReceivedCbT *pTranscribedTextReceivedCb; //!< callback invoked when transcribed text from a remote orginator needs to be surfaced up to voipcommon to get displayed locall
    void *pTranscribedTextReceivedCbUserData;

    //! speaker callback data
    VoipSpkrCallbackT *pSpkrDataCb;
    void *pSpkrCbUserData;

    //! pool of decoders
    PS4DecoderT *pDecoders;                 //!< array of decoder entries
    int32_t iDecoderPoolSize;               //!< number of entries in the array

    // silence (vad) detection parameters
    int32_t iVadPowerThreshold;             //!< power threshold between silence and not
    int32_t iVadSilenceThreshold;           //!< number of consecutive samples of silence before we enable vad

    uint8_t uSendSeq;                       //!< send sequence tracker
    uint8_t bLoopback;                      //!< loopback mode
    
    uint8_t bDecoderConnUpdateNeeded;       //!< tracks decoder connection updates
    uint8_t bDecoderConnUpdateInProgress;   //!< tracks decoder connection updates

    uint8_t bSharedPrivilege;               //!< tracks shared privileges
    uint8_t bVoiceLibInit;                  //!< TRUE if voice lib was initialized
    uint8_t bVoiceLibStart;                 //!< TRUE if voice lib was started
    uint8_t bVoiceTranscriptionEnabled;     //!< TRUE if voice transcription is enabled

    uint8_t bCrossPlay;                     //!< TRUE if cross-play is enabled, else FALSE
    int8_t iDebugLevel;                     //!< module debuglevel
    uint8_t bTextChatAccessibility;         //!< TRUE if text chat accessibility features are enabled
    uint8_t _pad[2];

    //! mem pool and init struct required when calling SceVoiceStart()
    SceVoiceStartParam sceStartParam;
    uint8_t aMemPool[SCE_VOICE_MEMORY_CONTAINER_SIZE];

    PS4LocalVoipUserT aLocalUsers[VOIP_MAXLOCALUSERS];

    //! pointer to global array tracking most restrictive voip contributors
    uint8_t *pVoipMostRestrictiveVoipContributors;

    UserListApiRefT *pUserListApi;          //!< userlist module to check for blocked users

    VoipFirstPartyIdCallbackCbT *pFirstPartyIdCb;   //!< function to query the account id for remote users
    void *pFirstPartyIdCbUserData;                  //!< function callback user data

    //! remote user list - must come last in ref as it is variable length
    PS4RemoteVoipUserT aRemoteUsers[1];
};

/*** Function Prototypes **********************************************************/

/*** Variables ********************************************************************/

/*** Private Functions ************************************************************/

/*F********************************************************************************/
/*!
    \Function _VoipHeadsetSilence

    \Description
        Determine if input buffer of audio data should be considered silence or not.
        32 consecutive 20ms frames of silence must be detected before we consider 
        further input silence.

    \Input *pHeadset    - headset module state
    \Input *pLocalUser  - pointer to local user state
    \Input *pSamples    - signed 16bit sample data
    \Input iFrameBytes  - bytes of data in frame

    \Output
        int32_t         - TRUE if silence, else FALSE

    \Version 11/13/2018 (jbrookes)
*/
/********************************************************************************F*/
static int32_t _VoipHeadsetSilence(VoipHeadsetRefT *pHeadset, PS4LocalVoipUserT *pLocalUser, const int16_t *pSamples, int32_t iFrameBytes)
{
    int32_t iSample, iValue, iNumSamples=iFrameBytes/2;
    int64_t iPowerSum;

    if (iNumSamples > 0)
    {
        // calculate average of sum of squares, which is proportional to power
        for (iSample = 0, iPowerSum = 0; iSample < iNumSamples; iSample += 1)
        {
            iValue = pSamples[iSample];
            iPowerSum += iValue*iValue;
        }
        iPowerSum /= iNumSamples;

        // if silence, accumulate silence samples; if not, clear silence sample counter
        if (iPowerSum < pHeadset->iVadPowerThreshold)
        {
            NetPrintfVerbose(((pLocalUser->iVadSilenceSamples < pHeadset->iVadSilenceThreshold && (pLocalUser->iVadSilenceSamples+iNumSamples >= pHeadset->iVadSilenceThreshold)) ? pHeadset->iDebugLevel : 0, 1, "voipheadsetps4: silence detected\n"));
            pLocalUser->iVadSilenceSamples += iNumSamples;
        }
        else
        {
            NetPrintfVerbose(((pLocalUser->iVadSilenceSamples >= pHeadset->iVadSilenceThreshold) ? pHeadset->iDebugLevel : 0, 1, "voipheadsetps4: voice detected\n"));
            pLocalUser->iVadSilenceSamples = 0;
        }
    }
    // if silence sample count is greater than threshold, return silence detected
    return(pLocalUser->iVadSilenceSamples >= pHeadset->iVadSilenceThreshold);
}

/*F********************************************************************************/
/*!
    \Function _VoipHeadsetWriteAudioInput

    \Description
        Write audio input to local user buffer

    \Input *pLocalUser  - local user we're writing audio input from
    \Input *pBuffer     - audio input we're writing
    \Input iBufLen      - size of audio we're writing

    \Output
        int32_t         - number of bytes written

    \Notes
        The audio buffer is a circular queue.  This function deliberately modifies
        only the length and inp fields so as to avoid the need for a critical section
        with the reader. $$note - could remove len field if we prevent the queue
        from filling completely, as the len is only required to distinguish a full
        queue from an empty queue (head==tail).  This might make the case for
        thread safety simpler to prove?

    \Version 09/21/2018 (jbrookes)
*/
/********************************************************************************F*/
static int32_t _VoipHeadsetWriteAudioInput(PS4LocalVoipUserT *pLocalUser, const uint8_t *pBuffer, int32_t iBufLen)
{
    int32_t iWriteSize, iWriteSize2;
    uint8_t bWriteWrap = FALSE;

    // see how much room we have to write
    if ((iWriteSize = sizeof(pLocalUser->aAudioData) - pLocalUser->iAudioLen) < iBufLen)
    {
        iBufLen = iWriteSize;
    }
    // see if we're wrapping
    if ((pLocalUser->iAudioInp+iBufLen) > (signed)sizeof(pLocalUser->aAudioData))
    {
        bWriteWrap = TRUE;
    }

    // see if we need to wrap the copy
    if (bWriteWrap)
    {
        // write to end of buffer
        iWriteSize = sizeof(pLocalUser->aAudioData) - pLocalUser->iAudioInp;
        ds_memcpy(pLocalUser->aAudioData+pLocalUser->iAudioInp, pBuffer, iWriteSize);
        // wrap and write the rest
        iWriteSize2 = iBufLen - iWriteSize;
        ds_memcpy(pLocalUser->aAudioData, pBuffer+iWriteSize, iWriteSize2);
        pLocalUser->iAudioInp = iWriteSize2;
    }
    else
    {
        ds_memcpy(pLocalUser->aAudioData+pLocalUser->iAudioInp, pBuffer, iBufLen);
        pLocalUser->iAudioInp += iBufLen;
    }

    // add to size
    pLocalUser->iAudioLen += iBufLen;
    // return size written
    return(iBufLen);
}

/*F********************************************************************************/
/*!
    \Function _VoipHeadsetReadAudioInput

    \Description
        Read audio input from local user buffer.  If pBuffer is not null, audio
        is copied to the output buffer but the input buffer paramters are not updated
        (i.e. the data is not consumed).  If pBuffer is null, audio is not copied
        and the buffer paramters are updated.  This two-stage approach allows us
        to consume only the amount of data that is actually used in the subsequent
        write to the PCM/encoder port.      

    \Input *pLocalUser  - local user we're reading audio input from
    \Input *pBuffer     - [out] buffer we're writing audio to
    \Input iBufRead     - amount of audio data we want

    \Output
        int32_t         - size of data read, or zero if not enough data is available

    \Notes
        The audio buffer is a circular queue.  This function deliberately modifies
        only the length and out fields so as to avoid the need for a critical section
        with the writer.

    \Version 09/21/2018 (jbrookes)
*/
/********************************************************************************F*/
static int32_t _VoipHeadsetReadAudioInput(PS4LocalVoipUserT *pLocalUser, uint8_t *pBuffer, int32_t iBufRead)
{
    int32_t iReadSize;
    uint8_t bReadWrap = FALSE;

    // see how much data is available for reading
    if ((iReadSize = pLocalUser->iAudioLen) < 0)
    {
        // return if there isn't enough data to read
        return(0);
    }
    else if (iReadSize < iBufRead)
    {
        iBufRead = iReadSize;
    }
    // see if we're wrapping
    if ((pLocalUser->iAudioOut+iBufRead) > (signed)sizeof(pLocalUser->aAudioData))
    {
        bReadWrap = TRUE;
    }

    // handle wrapping copy
    if (bReadWrap)
    {
        // get size to end of buffer
        int32_t iReadSize2 = sizeof(pLocalUser->aAudioData) - pLocalUser->iAudioOut;
        // get size of the rest of the copy after wrapping
        iReadSize = iBufRead - iReadSize2;
        // do the copy
        if (pBuffer != NULL)
        {
            ds_memcpy(pBuffer, pLocalUser->aAudioData+pLocalUser->iAudioOut, iReadSize2);
            ds_memcpy(pBuffer+iReadSize2, pLocalUser->aAudioData, iReadSize);
        }
        else
        {
            pLocalUser->iAudioOut = iReadSize;
            pLocalUser->iAudioLen -= iBufRead;
        }
    }
    else // regular non-wrapping copy
    {
        if (pBuffer != NULL)
        {
            ds_memcpy(pBuffer, pLocalUser->aAudioData+pLocalUser->iAudioOut, iBufRead);
        }
        else
        {
            pLocalUser->iAudioOut += iBufRead;
            pLocalUser->iAudioLen -= iBufRead;
        }
    }
    return(iBufRead);
}

/*F********************************************************************************/
/*!
    \Function _VoipAudioInputThread

    \Description
        Manage audio input for the voipheadset module; including opening the
        input port, reading input audio, and writing it to the local user buffer.
        The port is closed on thread exit.  Synchronization with the main thread
        on startup and shutdown is through the uMicPortId field.

    \Input *pArg    - argument

    \Notes
        The read/swap/write ordering in the process loop follows Sony example code

    \Version 09/21/2018 (jbrookes)
*/
/********************************************************************************F*/
static void _VoipAudioInputThread(void *pArg)
{
    PS4LocalVoipUserT *pLocalUser = (PS4LocalVoipUserT *)pArg;
    uint16_t aAudioBuffer[2][SCE_AUDIO_IN_GRAIN_DEFAULT];
    int32_t iBuffer, iResult;
    uint32_t uMicPortId;

    // open audio port
    if ((iResult = sceAudioInOpen(pLocalUser->sceUserId, SCE_AUDIO_IN_TYPE_VOICE_CHAT, 0, SCE_AUDIO_IN_GRAIN_DEFAULT, SCE_AUDIO_IN_FREQ_DEFAULT, SCE_AUDIO_IN_PARAM_FORMAT_S16_MONO)) > 0)
    {
        pLocalUser->uMicPortId = iResult;
        NetPrintf(("voipheadsetps4: created AudioIn port %d for user index %d (user ID = %d/0x%08x)\n", pLocalUser->uMicPortId, pLocalUser->iLocalUserIndex, pLocalUser->sceUserId, pLocalUser->sceUserId));
    }
    else
    {
        NetPrintf(("voipheadsetps4: sceAudioInOpen(%d) failed (err=%s)\n", pLocalUser->iLocalUserIndex, DirtyErrGetName(iResult)));
    }

    // save uMicPortId for later close; main thread will zero it to let us know when we should exit
    uMicPortId = pLocalUser->uMicPortId;

    // process audio input loop
    for (iBuffer = 0; pLocalUser->uMicPortId != 0; )
    {
        // blocking read of audio data to local double-buffer
        if ((iResult = sceAudioInInput(pLocalUser->uMicPortId, aAudioBuffer[iBuffer])) < 0)
        {
            NetPrintf(("voipheadsetps4: sceAudioInInput(%d) failed (err=%s)\n", pLocalUser->iLocalUserIndex, DirtyErrGetName(iResult)));
            break;
        }

        // switch the buffer
        iBuffer = !iBuffer;

        // write previous audio to local user buffer if narration is not active
        if (pLocalUser->iNarrating == 0)
        {
            _VoipHeadsetWriteAudioInput(pLocalUser, (const uint8_t *)aAudioBuffer[iBuffer], sizeof(aAudioBuffer[iBuffer]));
        }
    }

    // close audio port
    if ((iResult = sceAudioInClose(uMicPortId)) != SCE_OK)
    {
        NetPrintf(("voipheadsetps4: sceAudioInOpen(%d) failed (err=%s)\n", pLocalUser->iLocalUserIndex, DirtyErrGetName(iResult)));
    }
    // let the main thread know we're done
    pLocalUser->uMicPortId = 0xffffffff;
    // done
    NetPrintf(("voipheadsetps4: audioin thread exiting\n"));
}

/*F********************************************************************************/
/*!
    \Function _VoipHeadsetAudioThreadCreate

    \Description
        Create thread that will manage audio input for us

    \Input *pLocalUser  - local user we're reading audio for

    \Output
        int32_t         - zero=success, negative=failure

    \Version 09/21/2018 (jbrookes)
*/
/********************************************************************************F*/
static int32_t _VoipHeadsetAudioThreadCreate(PS4LocalVoipUserT *pLocalUser)
{
    DirtyThreadConfigT ThreadConfig;
    int32_t iResult;

    /* configure thread parameters
       the priority calculation is a bit odd but gets us at the highest priority
       due to the EAThread implementation on PS4 */
    ds_memclr(&ThreadConfig, sizeof(ThreadConfig));
    ThreadConfig.pName = "VoIPAudioIn";
    ThreadConfig.iAffinity = NetConnStatus('affn', 0, NULL, 0);
    ThreadConfig.iPriority = SCE_KERNEL_PRIO_FIFO_DEFAULT - SCE_KERNEL_PRIO_FIFO_HIGHEST;

    // init mic port identifier to startup value
    pLocalUser->uMicPortId = 0;

    // create worker thread
    if ((iResult = DirtyThreadCreate(_VoipAudioInputThread, pLocalUser, &ThreadConfig)) == 0)
    {
        NetPrintf(("voipheadsetps4: audioin thread created (err=%d)\n", iResult));
    }
    else
    {
        NetPrintf(("voipheadsetps4: audioin thread creation failed (err=%d)\n", iResult));
    }

    // wait until it's started up?

    return(iResult);
}

/*F********************************************************************************/
/*!
    \Function _VoipHeadsetTranscribeCheckCreate

    \Description
        Create a Transcribe ref for this local user if it hasn't already been created

    \Input *pHeadset    - headset state
    \Input *pLocalUser  - local user we're checking/creating transcription for

    \Output
        VoipTranscribeRefT * - transcribe ref, or null if we couldn't create it

    \Version 04/02/2019 (jbrookes)
*/
/********************************************************************************F*/
static VoipTranscribeRefT *_VoipHeadsetTranscribeCheckCreate(VoipHeadsetRefT *pHeadset, PS4LocalVoipUserT *pLocalUser)
{
    // if ref doesn't exist yet, create it
    if ((pLocalUser->pTranscribeRef == NULL) && ((pLocalUser->pTranscribeRef = VoipTranscribeCreate(0)) != NULL))
    {
        // set the debug level to the current setting
        #if DIRTYCODE_LOGGING
        VoipTranscribeControl(pLocalUser->pTranscribeRef, 'spam', pHeadset->iDebugLevel, 0, NULL);
        #endif
    }
    return(pLocalUser->pTranscribeRef);
}

/*F********************************************************************************/
/*!
    \Function _VoipHeadsetNarrateCb

    \Description
        VoipNarrate callback handler

    \Input *pNarrateRef - narration module state
    \Input iUserIndex   - local user index of user who requested narration
    \Input *pSamples    - sample data, or NULL if no data
    \Input iSize        - size of sample data in bytes, or zero if no data
    \Input *pUserData   - callback user data (headset module ref)

    \Version 10/25/2018 (jbrookes)
*/
/********************************************************************************F*/
static int32_t _VoipHeadsetNarrateCb(VoipNarrateRefT *pNarrateRef, int32_t iUserIndex, const int16_t *pSamples, int32_t iSize, void *pUserData)
{
    VoipHeadsetRefT *pHeadset = (VoipHeadsetRefT *)pUserData;
    PS4LocalVoipUserT *pLocalUser = &pHeadset->aLocalUsers[iUserIndex]; //$$todo do we need to validate index?

    if (iSize > 0)
    {
        iSize = _VoipHeadsetWriteAudioInput(pLocalUser, (const uint8_t *)pSamples, iSize);
    }
    else if (iSize == VOIPNARRATE_STREAM_START)
    {
        // see if we need to create voiptranscribe
        if (_VoipHeadsetTranscribeCheckCreate(pHeadset, pLocalUser) != NULL)
        {
            // disable short audio discard during narration
            VoipTranscribeControl(pLocalUser->pTranscribeRef, 'vdis', FALSE, 0, NULL);
        }
        // set narrating flag
        pLocalUser->iNarrating = 1;
    }
    else if (iSize == VOIPNARRATE_STREAM_END)
    {
        // set narrating flag to indicate tts audio streaming is done
        pLocalUser->iNarrating = -1;
    }

    return(iSize);
}

/*F********************************************************************************/
/*!
    \Function _VoipHeadsetVoiceEventCb

    \Description
        Event handler registered with the Voice Library

    \Input *pEventData  - event data

    \Version 01/21/2013 (mclouatre)
*/
/********************************************************************************F*/
static void _VoipHeadsetVoiceEventCb(SceVoiceEventData *pEventData)
{
    if (pEventData->eventType == SCE_VOICE_EVENT_AUDIOINPUT_CONTROL_STATUS)
    {
        NetPrintf(("voipheadsetps4: SCE_VOICE_EVENT_AUDIOINPUT_CONTROL_STATUS event\n"));
    }
    else if (pEventData->eventType != SCE_VOICE_EVENT_DATA_READY)
    {
        NetPrintf(("voipheadsetps4: unknown event\n"));
    }
}

/*F********************************************************************************/
/*!
    \Function _VoipHeadsetVoiceLibInit

    \Description
        Initialize the Sony Voice library.

    \Input *pHeadset    - module reference

    \Output
        int32_t         - negative=error, zero=success

    \Version 01/17/2013 (mclouatre)
*/
/********************************************************************************F*/
static int32_t _VoipHeadsetVoiceLibInit(VoipHeadsetRefT *pHeadset)
{
    SceVoiceInitParam sceVoiceInitParams;
    SceVoiceResourceInfo sceVoiceResourceInfo;
    int32_t iResult;
    int32_t iRetCode = 0; // default to success

    ds_memclr(&sceVoiceInitParams, sizeof(sceVoiceInitParams));
    sceVoiceInitParams.appType = SCE_VOICE_APPTYPE_GAME;
    sceVoiceInitParams.onEvent = _VoipHeadsetVoiceEventCb;
    sceVoiceInitParams.pUserData = pHeadset;

    if ((iResult = sceVoiceInit(&sceVoiceInitParams , SCE_VOICE_VERSION_100)) == SCE_OK)
    {
        NetPrintf(("voipheadsetps4: successfully initialized Sony Voice Lib\n", DirtyErrGetName(iResult)));
    }
    else
    {
        NetPrintf(("voipheadsetps4: sceVoiceInit() failed (err=%s)\n", DirtyErrGetName(iResult)));
        iRetCode = -1;  // signal failure
    }

    if ((iResult = sceVoiceGetResourceInfo(&sceVoiceResourceInfo)) == SCE_OK)
    {
        pHeadset->iDecoderPoolSize = sceVoiceResourceInfo.maxInVoicePort;
        NetPrintf(("voipheadsetps4: max IN_VOICE ports supported by the Voice Lib =        %d\n", sceVoiceResourceInfo.maxInVoicePort));
        NetPrintf(("voipheadsetps4: max OUT_VOICE ports supported by the Voice Lib =       %d\n", sceVoiceResourceInfo.maxOutVoicePort));
        NetPrintf(("voipheadsetps4: max IN_DEVICE ports supported by the Voice Lib =       %d\n", sceVoiceResourceInfo.maxInDevicePort));
        NetPrintf(("voipheadsetps4: max OUT_DEVICE ports supported by the Voice Lib =      %d\n", sceVoiceResourceInfo.maxOutDevicePort));
    }
    else
    {
        NetPrintf(("voipheadsetps4: sceVoiceGetResourceInfo() failed (err=%s)\n", DirtyErrGetName(iResult)));
    }

    return(iRetCode);
}

/*F********************************************************************************/
/*!
    \Function _VoipHeadsetVoiceLibShutdown

    \Description
        Shut down the sony Voice library.

    \Output
        int32_t         - negative=error, zero=success

    \Version 01/17/2013 (mclouatre)
*/
/********************************************************************************F*/
static int32_t _VoipHeadsetVoiceLibShutdown(void)
{
    int32_t iResult;
    int32_t iRetCode = 0; // default to success

    if ((iResult = sceVoiceEnd()) == SCE_OK)
    {
        NetPrintf(("voipheadsetps4: successfully shut down Sony Voice Lib\n"));
    }
    else
    {
        NetPrintf(("voipheadsetps4: sceVoiceEnd() failed (err=%s)\n", DirtyErrGetName(iResult)));
        iRetCode = -1;  // signal failure
    }

    return(iRetCode);
}

/*F********************************************************************************/
/*!
    \Function _VoipHeadsetVoiceSetThreadsParams

    \Description
        Sets the internal voice and audio device threads.

    \Input *pHeadset        - module reference
    \Input cpuAffinity       - CPU affinity
    \Input iSchedPriority   - thread priority

    \Output
        int32_t         - negative=error, zero=success

    \Version 11/01/2016 (mclouatre)
*/
/********************************************************************************F*/
static int32_t _VoipHeadsetVoiceSetThreadsParams(VoipHeadsetRefT *pHeadset, SceKernelCpumask cpuAffinity, int32_t iSchedPriority)
{
    SceVoiceThreadsParams sceVoiceThreadsParams;
    int32_t iResult;
    int32_t iRetCode = 0; // default to success

    sceVoiceThreadsParams.paramVersion = 1;
    sceVoiceThreadsParams.audioInThreadAffinityMask = cpuAffinity;
    sceVoiceThreadsParams.audioInThreadPriority = iSchedPriority;
    sceVoiceThreadsParams.audioOutThreadAffinityMask = cpuAffinity;
    sceVoiceThreadsParams.audioOutThreadPriority = iSchedPriority;
    sceVoiceThreadsParams.voiceThreadAffinityMask = cpuAffinity;
    sceVoiceThreadsParams.voiceThreadPriority = iSchedPriority;

    if ((iResult = sceVoiceSetThreadsParams(&sceVoiceThreadsParams)) == SCE_OK)
    {
        NetPrintf(("voipheadsetps4: successfully modified attributes of Sony voip threads - affinity is 0x%02x and priority is %d\n", cpuAffinity, iSchedPriority));
    }
    else
    {
        NetPrintf(("voipheadsetps4: sceVoiceSetThreadsParams() failed (err=%s)\n", DirtyErrGetName(iResult)));
        iRetCode = -1;  // signal failure
    }

    return(iRetCode);
}

/*F********************************************************************************/
/*!
    \Function _VoipHeadsetVoiceLibStart

    \Description
        Start the Sony Voice library.

    \Input *pHeadset    - module reference

    \Output
        int32_t         - negative=error, zero=success

    \Version 01/17/2013 (mclouatre)
*/
/********************************************************************************F*/
static int32_t _VoipHeadsetVoiceLibStart(VoipHeadsetRefT *pHeadset)
{
    int32_t iResult;
    int32_t iRetCode = 0; // default to success

    pHeadset->sceStartParam.container = &pHeadset->aMemPool;
    pHeadset->sceStartParam.memSize = sizeof(pHeadset->aMemPool);

    if ((iResult = sceVoiceStart(&pHeadset->sceStartParam)) == SCE_OK)
    {
        NetPrintf(("voipheadsetps4: successfully started Sony Voice Lib\n"));
    }
    else
    {
        NetPrintf(("voipheadsetps4: sceVoiceStart() failed (err=%s)\n", DirtyErrGetName(iResult)));
        iRetCode = -1;  // signal failure
    }

    return(iRetCode);
}

/*F********************************************************************************/
/*!
    \Function _VoipHeadsetVoiceLibStop

    \Description
        Stop the Sony Voice library.

    \Output
        int32_t         - negative=error, zero=success

    \Version 01/17/2013 (mclouatre)
*/
/********************************************************************************F*/
static int32_t _VoipHeadsetVoiceLibStop(void)
{
    int32_t iResult;
    int32_t iRetCode = 0; // default to success

    if ((iResult = sceVoiceStop()) == SCE_OK)
    {
        NetPrintf(("voipheadsetps4: successfully stopped Sony Voice Lib\n"));
    }
    else
    {
        NetPrintf(("voipheadsetps4: sceVoiceStop() failed (err=%s)\n", DirtyErrGetName(iResult)));
        iRetCode = -1;  // signal failure
    }

    return(iRetCode);
}

/*F********************************************************************************/
/*!
    \Function _VoipHeadsetPortDelete

    \Description
        Delete port.

    \Input uPortId      - port id

    \Output
        int32_t         - negative=error, zero=success

    \Version 01/21/2013 (mclouatre)
*/
/********************************************************************************F*/
static int32_t _VoipHeadsetPortDelete(uint32_t uPortId)
{
    int32_t iResult;
    int32_t iRetCode = 0; // default to success

    if ((iResult = sceVoiceDeletePort(uPortId)) == SCE_OK)
    {
        NetPrintf(("voipheadsetps4: sceVoiceDeletePort(%d) succeeded\n", uPortId));
    }
    else
    {
        NetPrintf(("voipheadsetps4: sceVoiceDeletePort(%d) failed (err=%s)\n", uPortId, DirtyErrGetName(iResult)));
        iRetCode = -1;  // signal failure
    }

    return(iRetCode);
}

/*F********************************************************************************/
/*!
    \Function _VoipHeadsetMicPortCreate

    \Description
        Starts a thread to create and manage AudioIn port used to acquire voice
        from microphone.

    \Input *pLocalUser  - local user to create mic port for

    \Output
        int32_t         - negative=error, zero=success

    \Version 09/21/2018 (mclouatre)
*/
/********************************************************************************F*/
static int32_t _VoipHeadsetMicPortCreate(PS4LocalVoipUserT *pLocalUser)
{
    return(_VoipHeadsetAudioThreadCreate(pLocalUser));
}

/*F********************************************************************************/
/*!
    \Function _VoipHeadsetPcmPortCreate

    \Description
        Create IN_PCMAUDIO port used to push audio into the encoder port

    \Input *pPcmPortId  - [out] storage for portid on success
    \Input iBufSize     - buffer size to allocate
    \Input iThreshold   - port threshold

    \Output
        int32_t         - negative=error, zero/SCE_OK=success

    \Version 09/20/2018 (jbrookes)
*/
/********************************************************************************F*/
static int32_t _VoipHeadsetPcmPortCreate(uint32_t *pPcmPortId, int32_t iBufSize, int32_t iThreshold)
{
    SceVoicePortParam sceVoicePortParam;

    sceVoicePortParam.portType = SCE_VOICE_PORTTYPE_IN_PCMAUDIO;
    sceVoicePortParam.threshold = 10;
    sceVoicePortParam.bMute = false;
    sceVoicePortParam.volume = 1.0f;
    sceVoicePortParam.pcmaudio.format.dataType = SCE_VOICE_PCM_SHORT_LITTLE_ENDIAN;
    sceVoicePortParam.pcmaudio.format.sampleRate = SCE_VOICE_SAMPLINGRATE_16000;
    sceVoicePortParam.pcmaudio.bufSize = 4*1024;

    return(sceVoiceCreatePort(pPcmPortId, &sceVoicePortParam));
}

/*F********************************************************************************/
/*!
    \Function _VoipHeadsetSpkrPortCreate

    \Description
        Create OUT_DEVICE port used to submit voice to speakers.

    \Input iUserIndex   - local user index
    \Input *pPortId     - [OUT] variable to be filled with port id upon success

    \Output
        int32_t         - negative=error, zero=success

    \Version 01/21/2013 (mclouatre)
*/
/********************************************************************************F*/
static int32_t _VoipHeadsetSpkrPortCreate(int32_t iUserIndex, uint32_t *pPortId)
{
    SceVoicePortParam sceVoicePortParam;
    int32_t iResult;
    int32_t iRetCode = 0; // default to success
    SceUserServiceUserId sceUserId = NetConnStatus('suid', iUserIndex, NULL, 0);

    sceVoicePortParam.portType = SCE_VOICE_PORTTYPE_OUT_DEVICE;
    sceVoicePortParam.bMute = false;
    sceVoicePortParam.threshold = 0;    // not applicable for OUT port
    sceVoicePortParam.volume = 1.0f;
    sceVoicePortParam.device.userId = sceUserId;
    sceVoicePortParam.device.type = SCE_AUDIO_OUT_PORT_TYPE_VOICE;
    sceVoicePortParam.device.index = 0; // index when a single user uses multiple virtual device

    if ((iResult = sceVoiceCreatePort(pPortId, &sceVoicePortParam)) == SCE_OK)
    {
        NetPrintf(("voipheadsetps4: created OUT_DEVICE port %d for user index %d (user ID = %d/0x%08x)\n", *pPortId, iUserIndex, sceUserId, sceUserId));
    }
    else
    {
        NetPrintf(("voipheadsetps4: sceVoiceCreatePort(OUT_DEVICE, %d) failed (err=%s)\n", iUserIndex, DirtyErrGetName(iResult)));
        iRetCode = -1;  // signal failure
    }

    return(iRetCode);
}

/*F********************************************************************************/
/*!
    \Function _VoipHeadsetEncoderPortCreate

    \Description
        Create OUT_VOICE port used to encode voice from local user.

    \Input iBitRate - selected bit rate
    \Input *pPortId - [OUT] variable to be filled with port id upon success

    \Output
        int32_t         - negative=error, zero=success

    \Version 01/21/2013 (mclouatre)
*/
/********************************************************************************F*/
static int32_t _VoipHeadsetEncoderPortCreate(int32_t iBitRate, uint32_t *pPortId)
{
    SceVoicePortParam sceVoicePortParam;
    int32_t iResult;
    int32_t iRetCode = 0; // default to success

    sceVoicePortParam.portType = SCE_VOICE_PORTTYPE_OUT_VOICE;
    sceVoicePortParam.bMute = false;
    sceVoicePortParam.threshold = 0; // not applicable for OUT port
    sceVoicePortParam.volume = 1.0f;
    sceVoicePortParam.voice.bitrate = iBitRate;

    if ((iResult = sceVoiceCreatePort(pPortId, &sceVoicePortParam)) == SCE_OK)
    {
        NetPrintf(("voipheadsetps4: created OUT_VOICE port %d\n", *pPortId));
    }
    else
    {
        NetPrintf(("voipheadsetps4: sceVoiceCreatePort(OUT_VOICE) failed (err=%s)\n", DirtyErrGetName(iResult)));
        iRetCode = -1;  // signal failure
    }

    return(iRetCode);
}

/*F********************************************************************************/
/*!
    \Function _VoipHeadsetDecoderPortCreate

    \Description
        Create IN_VOICE port used to decode voice to be played back locally.

    \Input iBitRate     - selected bit rate
    \Input *pPortId     - [OUT] variable to be filled with port id upon success

    \Output
        int32_t         - negative=error, zero=success

    \Version 01/21/2013 (mclouatre)
*/
/********************************************************************************F*/
static int32_t _VoipHeadsetDecoderPortCreate(int32_t iBitRate, uint32_t *pPortId)
{
    SceVoicePortParam sceVoicePortParam;
    int32_t iResult;
    int32_t iRetCode = 0; // default to success

    sceVoicePortParam.portType = SCE_VOICE_PORTTYPE_IN_VOICE;
    sceVoicePortParam.bMute = false;
    sceVoicePortParam.threshold = 100; // compensate network jitter
    sceVoicePortParam.volume = 1.0f;
    sceVoicePortParam.voice.bitrate = iBitRate;

    if ((iResult = sceVoiceCreatePort(pPortId, &sceVoicePortParam)) == SCE_OK)
    {
        NetPrintf(("voipheadsetps4: created IN_VOICE port %d\n", *pPortId));
    }
    else
    {
        NetPrintf(("voipheadsetps4: sceVoiceCreatePort(IN_VOICE) failed (err=%s)\n", DirtyErrGetName(iResult)));
        iRetCode = -1;  // signal failure
    }

    return(iRetCode);
}

/*F********************************************************************************/
/*!
    \Function _VoipHeadsetConnectPorts

    \Description
        Connect the specified input port to the specified output port.

    \Input uInPortId    - id of input port to be connected
    \Input uOutPortId   - id of output port to be connected

    \Output
        int32_t         - negative=error, zero=success

    \Version 01/21/2013 (mclouatre)
*/
/********************************************************************************F*/
static int32_t _VoipHeadsetConnectPorts(uint32_t uInPortId, uint32_t uOutPortId)
{
    int32_t iResult;
    if ((iResult = sceVoiceConnectIPortToOPort(uInPortId, uOutPortId)) == SCE_OK)
    {
        NetPrintf(("voipheadsetps4: successfully connected IN port %d to OUT port %d\n", uInPortId, uOutPortId));
    }
    else
    {
        NetPrintf(("voipheadsetps4: sceVoiceCConnectIPortToOport(%d, %d) failed (err=%s)\n", uInPortId, uOutPortId, DirtyErrGetName(iResult)));
    }
    return(iResult);
}

/*F********************************************************************************/
/*!
    \Function _VoipHeadsetDisconnectPorts

    \Description
        Disconnect the specified input port from the specified output port.

    \Input uInPortId  - id of input port to be disconnected
    \Input uOutPortId - id of output port to be disconnected

    \Output
        int32_t         - negative=error, zero=success

    \Version 01/21/2013 (mclouatre)
*/
/********************************************************************************F*/
static int32_t _VoipHeadsetDisconnectPorts(uint32_t uInPortId, uint32_t uOutPortId)
{
    int32_t iResult;
    if ((iResult = sceVoiceDisconnectIPortFromOPort(uInPortId, uOutPortId)) == SCE_OK)
    {
        NetPrintf(("voipheadsetps4: successfully disconnected IN port %d from OUT port %d\n", uInPortId, uOutPortId));
    }
    else
    {
        NetPrintf(("voipheadsetps4: sceVoiceDisconnectIPortFromOport(%d, %d) failed (err=%s)\n", uInPortId, uOutPortId, DirtyErrGetName(iResult)));
    }
    return(iResult);
}

/*F********************************************************************************/
/*!
    \Function _VoipHeadsetWriteToPort

    \Description
        Write to IN_VOICE port.

    \Input uInPortId    - id of input port to be written to
    \Input *pFrame      - pointer to encoded voice frame
    \Input uFrameSize   - frame size (in bytes)

    \Output
        int32_t         - negative=error, else number of bytes written

    \Version 01/21/2013 (mclouatre)
*/
/********************************************************************************F*/
static int32_t _VoipHeadsetWriteToPort(uint32_t uInPortId, const uint8_t *pFrame, uint32_t uFrameSize)
{
    uint32_t uBytesInOut = uFrameSize;
    int32_t iResult;

    if ((iResult = sceVoiceWriteToIPort(uInPortId, (void *)pFrame, &uBytesInOut, 0)) == SCE_OK)
    {
        iResult = uBytesInOut;
    }
    else
    {
        NetPrintf(("voipheadsetps4: sceVoiceWriteToIPort(%d, %d) failed (err=%s)\n", uInPortId, uFrameSize, DirtyErrGetName(iResult)));
        iResult = -1;
    }

    return(iResult);
}

/*F********************************************************************************/
/*!
    \Function _VoipHeadsetReadFromPort

    \Description
        Read from OUT_VOICE port.

    \Input uOutPortId   - id of output port to be read from
    \Input *pFrame      - pointer to buffer to be filled with encoded voice frame
    \Input *pFrameSize  - pointer to be filled with frame size (number of bytes read)

    \Output
        int32_t         - negative=error, zero=success

    \Version 01/21/2013 (mclouatre)
*/
/********************************************************************************F*/
static int32_t _VoipHeadsetReadFromPort(uint32_t uOutPortId, const uint8_t *pFrame, uint32_t *pFrameSize)
{
    int32_t iResult;
    if ((iResult = sceVoiceReadFromOPort(uOutPortId, (void *)pFrame, pFrameSize)) != SCE_OK)
    {
        NetPrintf(("voipheadsetps4: sceVoiceReadFromOPort(%d) failed (err=%s)\n", uOutPortId,  DirtyErrGetName(iResult)));
    }
    return(iResult);
}

/*F********************************************************************************/
/*!
    \Function _VoipHeadsetIsLocalVoiceFrameReady

    \Description
        Read from OUT_VOICE port.

    \Input uOutPortId   - id of output port to be read from

    \Output
        int32_t         - frame size if frame is ready; 0 if frame is not ready; negative if error

    \Version 01/21/2013 (mclouatre)
*/
/********************************************************************************F*/
static int32_t _VoipHeadsetIsLocalVoiceFrameReady(uint32_t uOutPortId)
{
    SceVoiceBasePortInfo sceVoiceBasePortInfo;
    sceVoiceBasePortInfo.pEdge = NULL;  // to avoid time-consuming process within voice lib (port's connection info)
    int32_t iResult;

    if ((iResult = sceVoiceGetPortInfo(uOutPortId, &sceVoiceBasePortInfo)) == SCE_OK)
    {
        if (sceVoiceBasePortInfo.portType != SCE_VOICE_PORTTYPE_OUT_VOICE)
        {
            NetPrintf(("voipheadsetps4: sceVoiceGetPortInfo(%d) called on port of wrong type (%d)\n", uOutPortId, sceVoiceBasePortInfo.portType));
            return(-1);
        }
        if (sceVoiceBasePortInfo.numByte >= sceVoiceBasePortInfo.frameSize)
        {
            iResult = sceVoiceBasePortInfo.frameSize;
        }
    }
    else
    {
        NetPrintf(("voipheadsetps4: sceVoiceGetPortInfo(%d) failed (err=%s)\n", uOutPortId, DirtyErrGetName(iResult)));
    }

    return(iResult);
}

/*F********************************************************************************/
/*!
    \Function _VoipHeadsetIsMicDetected

    \Description
        Check if microphone is connected.

    \Input uInPortId            - id of input port to be checked for headset
    \Input *pMicDetected        - [OUT] pointer to boolean var to be filled with headset status

    \Output
        int32_t                 - zero=success, negative for error

    \Version 09/21/2018 (jbrookes)
*/
/********************************************************************************F*/
static int32_t _VoipHeadsetIsMicDetected(uint32_t uInPortId, uint8_t *pMicDetected)
{
    int32_t iResult;

    if ((iResult = sceAudioInGetSilentState(uInPortId)) >= 0)
    {
        // consider microphone detected if data is available, or if the device reports as available but muted
        *pMicDetected = ((iResult == 0) || (iResult == SCE_AUDIO_IN_SILENT_STATE_USER_SETTING)) ? TRUE : FALSE;
        iResult = 0;
    }
    else
    {
        NetPrintf(("voipheadsetps4: sceAudioInGetSilentState(%d) failed (err=%s)\n", uInPortId, DirtyErrGetName(iResult)));
    }

    return(iResult);
}

/*F********************************************************************************/
/*!
    \Function _VoipHeadsetDecoderConnect

    \Description
        Connect decoder to speaker port

    \Input *pHeadset        - pointer to headset state
    \Input *pLocalUser      - local user to connect
    \Input *pDecoder        - decoder to connect

    \Version 01/08/2019 (jbrookes) Refactored from _VoipHeadsetUpdateDecoderConnections
*/
/********************************************************************************F*/
static void _VoipHeadsetDecoderConnect(VoipHeadsetRefT *pHeadset, PS4LocalVoipUserT *pLocalUser, PS4DecoderT *pDecoder)
{
    if (_VoipHeadsetConnectPorts(pHeadset->bCrossPlay ? pDecoder->uPcmPortId : pDecoder->uDecoderPortId, pLocalUser->uSpkrPortId) == 0)
    {
        pDecoder->uConnectedSpkrPort |= (1 << pLocalUser->iLocalUserIndex);
    }
}

/*F********************************************************************************/
/*!
    \Function _VoipHeadsetDecoderDisconnect

    \Description
        Disconnect decoder from speaker port

    \Input *pHeadset        - pointer to headset state
    \Input *pLocalUser      - local user to disconnect
    \Input *pDecoder        - decoder to disconnect

    \Output
        int32_t             - result of port disconnect operation

    \Version 01/08/2019 (jbrookes) Refactored from _VoipHeadsetUpdateDecoderConnections
*/
/********************************************************************************F*/
static int32_t _VoipHeadsetDecoderDisconnect(VoipHeadsetRefT *pHeadset, PS4LocalVoipUserT *pLocalUser, PS4DecoderT *pDecoder)
{
    int32_t iResult;
    uint32_t *pDecoderPortId = pHeadset->bCrossPlay ? &pDecoder->uPcmPortId : &pDecoder->uDecoderPortId;
    if ((iResult = _VoipHeadsetDisconnectPorts(*pDecoderPortId, pLocalUser->uSpkrPortId)) == SCE_OK)
    {
        pDecoder->uConnectedSpkrPort &= ~(1 << pLocalUser->iLocalUserIndex);

        if (pDecoder->uConnectedSpkrPort == 0)
        {
            if ((iResult = sceVoiceResetPort(*pDecoderPortId)) != SCE_OK)
            {
                NetPrintf(("voipheadsetps4: failed to reset decoder port %d (err=%s)\n", *pDecoderPortId, DirtyErrGetName(iResult)));
            }
        }
    }
    return(iResult);
}

/*F********************************************************************************/
/*!
    \Function _VoipHeadsetUpdateDecoderConnections

    \Description
        Make sure all decoders are connected to all local user speaker ports

    \Input *pHeadset        - pointer to headset state

    \Version 04/29/2014 (mclouatre)
*/
/********************************************************************************F*/
static void _VoipHeadsetUpdateDecoderConnections(VoipHeadsetRefT *pHeadset)
{
    PS4LocalVoipUserT *pLocalUser;
    PS4DecoderT *pDecoder;
    int32_t iLocalUserIndex;
    int32_t iDecoderIndex;

    for (iDecoderIndex = 0; iDecoderIndex < pHeadset->iDecoderPoolSize; iDecoderIndex++)
    {
        pDecoder = &pHeadset->pDecoders[iDecoderIndex];
        // only update valid decoders
        if (!pDecoder->bValid || (pDecoder->iOwnerIndex == VOIPHEADSET_INVALID_INDEX))
        {
            continue;
        }

        // update all local users
        for (iLocalUserIndex = 0; iLocalUserIndex < VOIP_MAXLOCALUSERS; iLocalUserIndex++)
        {
            pLocalUser = &pHeadset->aLocalUsers[iLocalUserIndex];
            // exclude empty or non-participating users
            if (VOIP_NullUser(&pLocalUser->user) || !pLocalUser->bParticipating)
            {
                continue;
            }

            // case for normal voip between remote user
            if (pDecoder->bOwnerRemote)
            {
                int32_t iOutput = VoipHeadsetStatus(pHeadset, 'hpao', iLocalUserIndex, NULL, 0);
                int32_t iPlaybackFlagIndex;

                /* we allow the decoder to connect to the speaker port when the audio output device is unknown because according to sony ticket 
                   https://ps4.scedev.net/support/issue/92213/_sceVoiceGetPortAttr_SCE_VOICE_ATTR_AUDIOOUTPUT_DESTINATION_returns_an_Output_of_0_when_I_have_restricted_(underage_sub_account)_login#n1262954
                   the audio output attribute will only be valid if data is first written to the output port. */
                iPlaybackFlagIndex = ((iOutput & SCE_AUDIO_OUT_STATE_OUTPUT_CONNECTED_HEADPHONE) || (iOutput == SCE_AUDIO_OUT_STATE_OUTPUT_UNKNOWN)) ? iLocalUserIndex : VOIP_SHARED_USER_INDEX;

                /* unconditionally (i.e. ignore headsets or TV/cam_mic being used) apply most restrictive voip settings to comply with Sony TR 4061 when there are
                   multiple local users interacting with a game (note: that's different from EA MLU which implies multiple users authenticated with Blaze).
                   The TR itself is not super clear aobut this, but we got some clear guidance from Sony about this here:
                   https://ps4.scedev.net/support/issue/131717/_Need_help_with_interpretation_of_TR_4061. The argument is that there is no reliable way to make
                   sure that the restricted user is not hearing the voice even if headsets are detected because what is detected as a headset connected to a
                   controller can rather be a speaker. */

                // if connected, disconnect if necessary
                if (pDecoder->uConnectedSpkrPort & (1 << iLocalUserIndex))
                {
                    // proceed with disconnect if voip privileges are restrictive or if playback is disabled for the user
                    if (!pHeadset->bSharedPrivilege || !(pHeadset->aRemoteUsers[pDecoder->iOwnerIndex].uEnablePlayback & (1 << iPlaybackFlagIndex)))
                    {
                        _VoipHeadsetDecoderDisconnect(pHeadset, pLocalUser, pDecoder);
                    }
                }
                else // if not connected, connect if necessary
                {
                    // only proceed with connect if voip privileges allow it and if playback is enabled for the user
                    if (pHeadset->bSharedPrivilege && pHeadset->aRemoteUsers[pDecoder->iOwnerIndex].uEnablePlayback & (1 << iPlaybackFlagIndex))
                    {
                        _VoipHeadsetDecoderConnect(pHeadset, pLocalUser, pDecoder);
                    }
                }
            }
            else // loopback
            {
                // connect decoder to its own speaker port for loopback
                if ((pDecoder->iOwnerIndex == iLocalUserIndex) && !(pDecoder->uConnectedSpkrPort & (1 << iLocalUserIndex)))
                {
                    _VoipHeadsetDecoderConnect(pHeadset, pLocalUser, pDecoder);
                }
                // disconnect decoder from other local speaker port not own by the current local user
                else if ((pDecoder->iOwnerIndex != iLocalUserIndex) && (pDecoder->uConnectedSpkrPort & (1 << iLocalUserIndex)))
                {
                    _VoipHeadsetDecoderDisconnect(pHeadset, pLocalUser, pDecoder);
                }
            } 
        }
    }
}

/*F********************************************************************************/
/*!
    \Function _VoipHeadsetCreateDecoder

    \Description
        Create a PS4DecoderT entry in the pool of decoders.

    \Input *pHeadset    - pointer to headset state

    \Output
        int32_t         - negative=error, zero=success

    \Version 09/18/2013 (mclouatre)
*/
/********************************************************************************F*/
static int32_t _VoipHeadsetCreateDecoder(VoipHeadsetRefT *pHeadset)
{
    int32_t iRetCode = 0; // default to success
    int32_t iDecoderIndex;

    // check if there is a free spot in the pool of decoders
    for (iDecoderIndex = 0; iDecoderIndex < pHeadset->iDecoderPoolSize; iDecoderIndex++)
    {
        if (!pHeadset->pDecoders[iDecoderIndex].bValid)
        {
            break;
        }
    }

    // did we find a free spot?
    if (iDecoderIndex < pHeadset->iDecoderPoolSize)
    {
        PS4DecoderT *pDecoder = &pHeadset->pDecoders[iDecoderIndex];
        int32_t iResult;

        if ((iResult = _VoipHeadsetDecoderPortCreate(pHeadset->iEncoderBps, &pDecoder->uDecoderPortId)) == 0)
        {
            NetPrintf(("voipheadsetps4: added IN_VOICE port (%d) at index %d in pool of decoders\n", pDecoder->uDecoderPortId, iDecoderIndex));
            pDecoder->bValid = TRUE;
            pDecoder->iOwnerIndex = VOIPHEADSET_INVALID_INDEX;
            pDecoder->uConnectedSpkrPort = 0;
        }
        else
        {
            NetPrintf(("voipheadsetps4: failed to create a new decoder port)\n"));
            iRetCode = -1;  // signal failure
        }

        /* create PCM port for non-native voip codec; 4k for output gives us ~120ms of buffering at 16khz/16bit, and is
           required to cover a ~100ms voice bundle coming in all at once plus posible jitter.  a threshold of 100ms gives
           us our network jitter buffer */
        if ((iResult = _VoipHeadsetPcmPortCreate(&pDecoder->uPcmPortId, 4*1024, 100)) != SCE_OK)
        {
            NetPrintf(("voipheadsetps4: failed to create pcm port for non-native codec\n"));
        }
    }
    else
    {
        NetPrintf(("voipheadsetps4: IN_VOICE port creation ignored because pool of decoders is already full\n"));
        iRetCode = -2;  // signal failure
    }

    return(iRetCode);
}

/*F********************************************************************************/
/*!
    \Function _VoipHeadsetReleaseDecoderOwnership

    \Description
        Break the ownership association between the specified PS4DecoderT entry
        and its owner.

    \Input *pHeadset        - pointer to headset state
    \Input iDecoderIndex    - index of decoder entry in pool of decoders

    \Output
        int32_t             - negative=error, zero=success

    \Version 09/18/2013 (mclouatre)
*/
/********************************************************************************F*/
static int32_t _VoipHeadsetReleaseDecoderOwnership(VoipHeadsetRefT *pHeadset, int32_t iDecoderIndex)
{
    int32_t iRetCode = -1;  // default to failure
    PS4DecoderT *pDecoder = &pHeadset->pDecoders[iDecoderIndex];

    if (pDecoder->bValid)
    {
        if (pDecoder->iOwnerIndex != VOIPHEADSET_INVALID_INDEX)
        {
            if (pDecoder->bOwnerRemote)
            {
                NetPrintfVerbose((pHeadset->iDebugLevel, 1, "voipheadsetps4: remote user %d released ownership of decoder entry %d\n", pDecoder->iOwnerIndex, iDecoderIndex));
                pHeadset->aRemoteUsers[pDecoder->iOwnerIndex].iDecoderIndex = VOIPHEADSET_INVALID_INDEX;
            }
            else
            {
                NetPrintfVerbose((pHeadset->iDebugLevel, 1, "voipheadsetps4: local user %d released ownership of decoder entry %d\n", pDecoder->iOwnerIndex, iDecoderIndex));
                pHeadset->aLocalUsers[pDecoder->iOwnerIndex].iDecoderIndex = VOIPHEADSET_INVALID_INDEX;
            }

            pDecoder->uLastUsage = 0;
            pDecoder->iOwnerIndex = VOIPHEADSET_INVALID_INDEX;
        }
        else
        {
            NetPrintf(("voipheadsetps4: can't release ownership on a decoder entry with no owner (index = %d)\n", iDecoderIndex));
        }
    }
    else
    {
        NetPrintf(("voipheadsetps4: can't release ownership on an invalid decoder entry (index = %d)\n", iDecoderIndex));
    }

    return(iRetCode);
}

/*F********************************************************************************/
/*!
    \Function _VoipHeadsetTakeDecoderOwnership

    \Description
        Create the ownership association between the specified PS4DecoderT entry
        and a user.

    \Input *pHeadset        - pointer to headset state
    \Input iDecoderIndex    - index of decoder entry in pool of decoders
    \Input iUserIndex       - local user index or remote user space index  (discriminated with bRemote)
    \Input bRemote          - whether the owner is a remote user or a local user

    \Output
        int32_t             - negative=error, zero=success

    \Version 09/18/2013 (mclouatre)
*/
/********************************************************************************F*/
static int32_t _VoipHeadsetTakeDecoderOwnership(VoipHeadsetRefT *pHeadset, int32_t iDecoderIndex, int32_t iUserIndex, uint8_t bRemote)
{
    PS4DecoderT *pDecoder = &pHeadset->pDecoders[iDecoderIndex];
    int32_t iRetCode = -1;  // default to failure

    if (pDecoder->bValid)
    {
        if (pDecoder->iOwnerIndex == VOIPHEADSET_INVALID_INDEX)
        {
            pDecoder->iOwnerIndex = iUserIndex;
            pDecoder->bOwnerRemote = bRemote;

            if (bRemote)
            {
                NetPrintfVerbose((pHeadset->iDebugLevel, 1, "voipheadsetps4: remote user %d took ownership of decoder entry %d\n", iUserIndex, iDecoderIndex));
                pHeadset->aRemoteUsers[iUserIndex].iDecoderIndex = iDecoderIndex;
            }
            else
            {
                NetPrintfVerbose((pHeadset->iDebugLevel, 1, "voipheadsetps4: local user %d took ownership of decoder entry %d\n", iUserIndex, iDecoderIndex));
                pHeadset->aLocalUsers[iUserIndex].iDecoderIndex = iDecoderIndex;
            }

            // refresh decoder connections
            pHeadset->bDecoderConnUpdateNeeded = TRUE;
        }
        else
        {
            NetPrintf(("voipheadsetps4: can't take ownership on a decoder entry that already has an owner (index = %d)\n", iDecoderIndex));
        }
    }
    else
    {
        NetPrintf(("voipheadsetps4: can't take ownership on an invalid decoder entry (index = %d)\n", iDecoderIndex));
    }

    return(iRetCode);
}

/*F********************************************************************************/
/*!
    \Function _VoipHeadsetFindFreeDecoder

    \Description
        Identify a free decoder.

    \Input *pHeadset        - pointer to headset state

    \Output
        int32_t             - VOIPHEADSET_INVALID_INDEX for no decoder found; decoder index otherwise

    \Version 09/17/2013 (mclouatre)
*/
/********************************************************************************F*/
static int32_t _VoipHeadsetFindFreeDecoder(VoipHeadsetRefT *pHeadset)
{
    int32_t iFoundDecoderIndex = VOIPHEADSET_INVALID_INDEX;  // default to failure
    int32_t iDecoderIndex;

    for (iDecoderIndex = 0; iDecoderIndex < pHeadset->iDecoderPoolSize; iDecoderIndex++)
    {
        if (pHeadset->pDecoders[iDecoderIndex].bValid)
        {
            if (pHeadset->pDecoders[iDecoderIndex].iOwnerIndex == VOIPHEADSET_INVALID_INDEX)
            {
                iFoundDecoderIndex = iDecoderIndex;
                break;
            }
        }
    }

    return(iFoundDecoderIndex);
}

/*F********************************************************************************/
/*!
    \Function _VoipHeadsetUpdateDecodersOwnership

    \Description
        Check if the decoders owner are still active. If not, break the
        ownership association.

    \Input *pHeadset        - pointer to headset state

    \Version 09/17/2013 (mclouatre)
*/
/********************************************************************************F*/
static void _VoipHeadsetUpdateDecodersOwnership(VoipHeadsetRefT *pHeadset)
{
    int32_t iDecoderIndex;

    for (iDecoderIndex = 0; iDecoderIndex < pHeadset->iDecoderPoolSize; iDecoderIndex++)
    {
        if (pHeadset->pDecoders[iDecoderIndex].bValid)
        {
            if (pHeadset->pDecoders[iDecoderIndex].iOwnerIndex != VOIPHEADSET_INVALID_INDEX)
            {
                if (NetTickDiff(NetTick(), pHeadset->pDecoders[iDecoderIndex].uLastUsage) > VOIPHEADSET_DECODER_OWNERSHIP_AGING)
                {
                    _VoipHeadsetReleaseDecoderOwnership(pHeadset, iDecoderIndex);
                }
            }
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

    \Version 01/21/2013 (mclouatre)
*/
/********************************************************************************F*/
static void _VoipHeadsetUpdateStatus(VoipHeadsetRefT *pHeadset)
{
    PS4LocalVoipUserT *pLocalUser;
    int32_t iLocalUserIndex;

    for (iLocalUserIndex = 0; iLocalUserIndex < VOIP_MAXLOCALUSERS; iLocalUserIndex++)
    {
        pLocalUser = &pHeadset->aLocalUsers[iLocalUserIndex];
        if (!VOIP_NullUser(&pLocalUser->user))
        {
            int32_t iResult;
            uint8_t bMicDetected = FALSE;

            if ((iResult = _VoipHeadsetIsMicDetected(pLocalUser->uMicPortId, &bMicDetected)) == 0)
            {
                uint32_t uUserMicStatus = (bMicDetected ? 1 : 0) << iLocalUserIndex;
                uint32_t uLastUserMicStatus = pHeadset->uLastMicStatus & (1 << iLocalUserIndex);

                if (uUserMicStatus != uLastUserMicStatus)
                {
                    NetPrintf(("voipheadsetps4: microphone %s for local user index %d\n", (bMicDetected ? "inserted" : "removed"), iLocalUserIndex));

                    // trigger callback
                    pHeadset->pStatusCb(iLocalUserIndex, bMicDetected, VOIP_HEADSET_STATUS_INPUT, pHeadset->pCbUserData);

                    // update last microphone status
                    pHeadset->uLastMicStatus &= ~(1 << iLocalUserIndex);  // clear bit first
                    pHeadset->uLastMicStatus |= uUserMicStatus;           // now set bit status if applicable
                }
            }
        }
    }
}

/*F********************************************************************************/
/*!
    \Function _VoipHeadsetDeleteLocalTalker

    \Description
        Disconnect the "IN_DEVICE" port from the "OUT_VOICE" port associated with the
        local talker. Then delete both ports.

        Also disconnect all decoder ports from the speaker port of this local user.

    \Input *pHeadset        - headset module
    \Input iLocalUserIndex  - local user index
    \Input *pVoipUser       - user identifier (optional)

    \Output
        int32_t             - negative=error, zero=success

    \Version 01/22/2013 (mclouatre)
*/
/********************************************************************************F*/
static int32_t _VoipHeadsetDeleteLocalTalker(VoipHeadsetRefT *pHeadset, int32_t iLocalUserIndex, VoipUserT *pVoipUser)
{
    PS4LocalVoipUserT *pLocalUser = &pHeadset->aLocalUsers[iLocalUserIndex];
    int32_t iRetCode = 0; // default to success

    if (pVoipUser)
    {
        if (pLocalUser->bParticipating != FALSE)
        {
            return(-1);
        }

        if (!VOIP_SameUser(pVoipUser, &pLocalUser->user))
        {
            NetPrintf(("voipheadsetps4: deleting local talker %lld failed because it does not match user at index %d (%x)\n",
                pVoipUser->AccountInfo.iPersonaId, iLocalUserIndex, pLocalUser->user.AccountInfo.iPersonaId));
            return(-1);
        }
    }

    NetPrintf(("voipheadsetps4: deleting local talker %lld at local user index %d\n", pLocalUser->user.AccountInfo.iPersonaId, iLocalUserIndex));

    // tell the voip audio input thread for this user to terminate; and wait for it to finish
    for (pLocalUser->uMicPortId = 0; pLocalUser->uMicPortId == 0; )
    {
        sceKernelUsleep(1000);
    }

    // disconnect input and output ports
    if (_VoipHeadsetDisconnectPorts(pLocalUser->uPcmPortId, pLocalUser->uEncoderPortId) < 0)
    {
        iRetCode = -1;
    }
    // delete encoder port
    if (_VoipHeadsetPortDelete(pLocalUser->uEncoderPortId) < 0)
    {
        iRetCode = -2;
    }
    // delete input port
    if (_VoipHeadsetPortDelete(pLocalUser->uPcmPortId) < 0)
    {
        iRetCode = -3;
    }

    // clear last mic status
    pHeadset->uLastMicStatus &= ~(1 << iLocalUserIndex);
    VOIP_ClearUser(&pLocalUser->user);

    return(iRetCode);
}

/*F********************************************************************************/
/*!
    \Function _VoipHeadsetAddLocalTalker

    \Description
        Create a Mic Port for User (needed for headsetstatus)

    \Input *pHeadset        - headset module
    \Input iLocalUserIndex  - local user index
    \Input *pVoipUser       - local user

    \Output
        int32_t         - negative=error, zero=success

    \Version 05/06/2014 (tcho)
*/
/********************************************************************************F*/
static int32_t _VoipHeadsetAddLocalTalker(VoipHeadsetRefT *pHeadset, int32_t iLocalUserIndex, VoipUserT *pVoipUser)
{
    PS4LocalVoipUserT *pLocalUser = &pHeadset->aLocalUsers[iLocalUserIndex];
    int32_t iResult;

    if (VOIP_NullUser(pVoipUser))
    {
        NetPrintf(("voipheadsetps4: can't add NULL local voip user at local user index %d\n", iLocalUserIndex));
        return(-1);
    }
    NetPrintf(("voipheadsetps4: adding voip local user %lld at local user index %d\n", pVoipUser->AccountInfo.iPersonaId, iLocalUserIndex));

    // init local user state
    VOIP_CopyUser(&pLocalUser->user, pVoipUser);
    pLocalUser->iLocalUserIndex = iLocalUserIndex;
    pLocalUser->iDecoderIndex = VOIPHEADSET_INVALID_INDEX;
    pLocalUser->sceUserId = NetConnStatus('suid', iLocalUserIndex, NULL, 0);
    pLocalUser->uGender = VOIPNARRATE_GENDER_NEUTRAL;

    // start audio input thread, create audio in port
    if ((iResult =_VoipHeadsetMicPortCreate(pLocalUser)) < 0)
    {
        return(iResult);
    }

    // create pcm port that we will push mic audio into for encoding and sending
    if ((iResult = _VoipHeadsetPcmPortCreate(&pLocalUser->uPcmPortId, 2*1024, 10)) == SCE_OK)
    {
        NetPrintf(("voipheadsetps4: created IN_PCMAUDIO port %d for user index %d (user ID = %d/0x%08x)\n", pLocalUser->uPcmPortId, iLocalUserIndex, pLocalUser->sceUserId, pLocalUser->sceUserId));
    }
    else
    {
        NetPrintf(("voipheadsetps4: sceVoiceCreatePort(IN_PCMAUDIO, %d) failed (err=%s)\n", iLocalUserIndex, DirtyErrGetName(iResult)));
    }
    // create encoder port that is linked to pcm port
    _VoipHeadsetEncoderPortCreate(pHeadset->iEncoderBps, &pLocalUser->uEncoderPortId);
    // connect pcm to encoder
    if ((iResult = _VoipHeadsetConnectPorts(pLocalUser->uPcmPortId, pLocalUser->uEncoderPortId)) != SCE_OK)
    {
        // clean up if we had a failure
        _VoipHeadsetDeleteLocalTalker(pHeadset, iLocalUserIndex, pVoipUser);
    }
    return(iResult);
}

/*F********************************************************************************/
/*!
    \Function _VoipHeadsetDeactivateLocalTalker

    \Description
        Disconnect the "IN_DEVICE" port from the "OUT_VOICE" port associated with the
        local talker. Then delete both ports.

        Also disconnect all decoder ports from the speaker port of this local user.

    \Input *pHeadset        - headset module
    \Input iLocalUserIndex  - local index

    \Output
        int32_t             - negative=error, zero=success

    \Version 01/22/2013 (mclouatre)
*/
/********************************************************************************F*/
static int32_t _VoipHeadsetDeactivateLocalTalker(VoipHeadsetRefT *pHeadset, int32_t iLocalUserIndex)
{
    PS4LocalVoipUserT *pLocalUser = &pHeadset->aLocalUsers[iLocalUserIndex];
    int32_t iRetCode = 0; // default to success
    int32_t iDecoderIndex;

    if (pLocalUser->bParticipating != FALSE)
    {
        pLocalUser->bParticipating = FALSE;

        // release decoder  (the one being fed back into during loopback mode)
        if (pLocalUser->iDecoderIndex != VOIPHEADSET_INVALID_INDEX)
        {
            _VoipHeadsetReleaseDecoderOwnership(pHeadset, pLocalUser->iDecoderIndex);
        }

        // disconnect all decoder ports from the speaker port of this local user
        for (iDecoderIndex = 0; iDecoderIndex < pHeadset->iDecoderPoolSize; iDecoderIndex++)
        {
            if ((pHeadset->pDecoders[iDecoderIndex].bValid) && (pHeadset->pDecoders[iDecoderIndex].uConnectedSpkrPort & (1 << iLocalUserIndex)))
            {
                iRetCode = _VoipHeadsetDecoderDisconnect(pHeadset, pLocalUser, &pHeadset->pDecoders[iDecoderIndex]);
            }
        }

        if (_VoipHeadsetPortDelete(pLocalUser->uSpkrPortId) < 0)
        {
            iRetCode = -1; // signal failure
        }

        #if DIRTYCODE_LOGGING
        if (iRetCode == 0)
        {
            NetPrintf(("voipheadsetps4: local user %lld at local user index %d is no longer participating\n", pLocalUser->user.AccountInfo.iPersonaId, iLocalUserIndex));
        }
        #endif
    }

    return(iRetCode);
}

/*F********************************************************************************/
/*!
    \Function _VoipHeadsetDisconnectLocalTalkers

    \Description
        Disconnects the "IN_DEVICE" port from the "OUT_VOICE" port associated with the
        local talker.  Also disconnects all decoder ports from the speaker port of
        all local users.

    \Input *pHeadset        - headset module

    \Version 01/11/2018 (jbrookes)
*/
/********************************************************************************F*/
static void _VoipHeadsetDisconnectLocalTalkers(VoipHeadsetRefT *pHeadset)
{
    int32_t iLocalUserIndex, iDecoderIndex;
    PS4LocalVoipUserT *pLocalUser;
    PS4DecoderT *pDecoder;

    for (iLocalUserIndex = 0; iLocalUserIndex < VOIP_MAXLOCALUSERS; iLocalUserIndex += 1)
    {
        pLocalUser = &pHeadset->aLocalUsers[iLocalUserIndex];
        // skip empty/non-participating users
        if (VOIP_NullUser(&pLocalUser->user) || !pLocalUser->bParticipating)
        {
            continue;
        }
        // disconnect from loopback decoder, if set
        if (pLocalUser->iDecoderIndex != VOIPHEADSET_INVALID_INDEX)
        {
            _VoipHeadsetDecoderDisconnect(pHeadset, pLocalUser, &pHeadset->pDecoders[pLocalUser->iDecoderIndex]);
        }
        // disconnect all decoder ports from the speaker port of this local user
        for (iDecoderIndex = 0; iDecoderIndex < pHeadset->iDecoderPoolSize; iDecoderIndex += 1)
        {
            pDecoder = &pHeadset->pDecoders[iDecoderIndex];
            // skip invalid decoders and decoders this user is not connected to
            if ((pHeadset->pDecoders[iDecoderIndex].bValid) || !(pDecoder->uConnectedSpkrPort & (1 << iLocalUserIndex)))
            {
                continue;
            }
            // disconnect decoder
            _VoipHeadsetDecoderDisconnect(pHeadset, pLocalUser, pDecoder);
        }
    }
}

/*F********************************************************************************/
/*!
    \Function _VoipHeadsetActivateLocalTalker

    \Description
        Prepare a user for full voip functionality

    \Input *pHeadset        - headset module
    \Input iLocalUserIndex  - local user index
    \Input *pVoipUser       - local voip user

    \Output
        int32_t         - negative=error, zero=success

    \Version 05/06/2014 (tcho)
*/
/********************************************************************************F*/
static int32_t _VoipHeadsetActivateLocalTalker(VoipHeadsetRefT *pHeadset, int32_t iLocalUserIndex, VoipUserT *pVoipUser)
{
    PS4LocalVoipUserT *pLocalUser = &pHeadset->aLocalUsers[iLocalUserIndex];
    int32_t iRetCode = -1;

    if (VOIP_NullUser(pVoipUser))
    {
        NetPrintf(("voipheadsetps4: cannot promote a null user to a participating state (local user index %d)\n", iLocalUserIndex));
        return(iRetCode);
    }

    if (pLocalUser->bParticipating == FALSE)
    {
        if (_VoipHeadsetSpkrPortCreate(iLocalUserIndex, &pLocalUser->uSpkrPortId) == 0)
        {
            pLocalUser->bParticipating = TRUE;
            pLocalUser->uNextBlockListCheck = NetTick();

            // attempt to create an additional decoder in the pool (if there is still room for it)
            _VoipHeadsetCreateDecoder(pHeadset);

            // make sure all decoders are connected to all speaker ports of all participating users
            pHeadset->bDecoderConnUpdateNeeded = TRUE;

            iRetCode = 0; // signal success

            NetPrintf(("voipheadsetps4: local user %lld at local user index %d is now participating\n", pLocalUser->user.AccountInfo.iPersonaId, iLocalUserIndex));
        }
        else
        {
            _VoipHeadsetDeactivateLocalTalker(pHeadset, iLocalUserIndex);
        }
    }
    else
    {
        NetPrintf(("voipheadsetps4: local user %lld at local user index %d is already participating\n", pLocalUser->user.AccountInfo.iPersonaId, iLocalUserIndex));
    }

    return(iRetCode);
}

/*F********************************************************************************/
/*!
    \Function _VoipHeadsetCodecEncode

    \Description
        Encode raw audio with VoipCodec, limiting input rate to 16khz for situations
        where the input rate is not already 16khz (narration).

    \Input *pLocalUser      - local user to encode voice for
    \Input *pEncAudioBuffer - [out] encoded audio data
    \Input *pRawAudioBuffer - audio to encode
    \Input iFrameSize       - size of data to encode
    \Input *pTranscribeRef  - transcribe state, or NULL if transcription not enabled

    \Output
        int32_t             - size of encoded data

    \Version 01/08/2018 (jbrookes)
*/
/********************************************************************************F*/
static int32_t _VoipHeadsetCodecEncode(PS4LocalVoipUserT *pLocalUser, uint8_t *pEncAudioBuffer, const uint8_t *pRawAudioBuffer, int32_t iFrameSize, VoipTranscribeRefT *pTranscribeRef)
{
    int32_t iEncAudioTime, iEncAudioRate;
    // reset encoded audio timestamp if at start of phrase
    if ((pLocalUser->iEncAudioSamples == 0) && (pLocalUser->uEncAudioTick == 0))
    {
        pLocalUser->uEncAudioTick = NetTick();
    }
    // calculate elapsed time for phrase and rate we've submitted audio so far
    iEncAudioTime = NetTickDiff(NetTick(), pLocalUser->uEncAudioTick)+1;
    iEncAudioRate = (pLocalUser->iEncAudioSamples*1000)/iEncAudioTime;
    // throttle rate to 16khz - we need to do this for audio that is not produced by a 16khz source
    if (iEncAudioRate > VOIPHEADSET_SAMPLERATE)
    {
        return(0);
    }
    // update sample tracker and encode
    pLocalUser->iEncAudioSamples += iFrameSize/2;
    return(VoipCodecEncode(pEncAudioBuffer, (const int16_t *)pRawAudioBuffer, iFrameSize/2, pTranscribeRef));
}

/*F********************************************************************************/
/*!
    \Function _VoipHeadsetCodecDecode

    \Description
        Decode VoipCodec-encoded data

    \Input *pOutBuf         - [out] buffer to hold decoded audio
    \Input iOutSize         - size of output buffer
    \Input *pEncAudioData   - encoded audio data
    \Input iAudioSize       - size of audio
    \Input iDecoderIndex    - index of decoder

    \Output
        int32_t             - size of decoded data

    \Version 01/08/2018 (jbrookes)
*/
/********************************************************************************F*/
static int32_t _VoipHeadsetCodecDecode(int16_t *pOutBuf, int32_t iOutSize, const uint8_t *pEncAudioData, int32_t iAudioSize, int32_t iDecoderIndex)
{
    int32_t aDecodeBuf[VOIPHEADSET_FRAMESAMPLES], iSample;
    ds_memclr(aDecodeBuf, sizeof(aDecodeBuf));
    VoipCodecDecode(aDecodeBuf, pEncAudioData, iAudioSize, iDecoderIndex);
    for (iSample = 0; iSample < VOIPHEADSET_FRAMESAMPLES; iSample += 1)
    {
        pOutBuf[iSample] = (int16_t)aDecodeBuf[iSample];
    }
    return(VOIPHEADSET_FRAMESIZE);
}

/*F********************************************************************************/
/*!
    \Function _VoipHeadsetGetDecoder

    \Description
        Gets decoder to use for decoding

    \Input *pHeadset        - pointer to headset state
    \Input iUserIndex       - user index
    \Input iDecoderIndex    - current decoder index
    \Input bRemote          - TRUE if user is remote, else FALSE (loopback)

    \Version 01/09/2018 (jbrookes)
*/
/********************************************************************************F*/
static PS4DecoderT *_VoipHeadsetGetDecoder(VoipHeadsetRefT *pHeadset, int32_t iUserIndex, int32_t iDecoderIndex, uint8_t bRemote)
{
    PS4DecoderT *pDecoder=NULL;
    // get decoder (acquire one if necessary)
    if (iDecoderIndex == VOIPHEADSET_INVALID_INDEX)
    {
        if ((iDecoderIndex = _VoipHeadsetFindFreeDecoder(pHeadset)) != VOIPHEADSET_INVALID_INDEX)
        {
            _VoipHeadsetTakeDecoderOwnership(pHeadset, iDecoderIndex, iUserIndex, bRemote);
        }
    }
    // reference and return decoder
    if (iDecoderIndex >= 0)
    {
        pDecoder = &pHeadset->pDecoders[iDecoderIndex];
    }
    return(pDecoder);
}

/*F********************************************************************************/
/*!
    \Function _VoipHeadsetPlayEncodedAudio

    \Description
        Send encoded data to the mic callback, or play it back locally if loopback

    \Input *pHeadset        - pointer to headset state
    \Input *pDecoder        - decoder to use to play encoded audio
    \Input iDecoderIndex    - index of our decoder
    \Input *pEncAudioData   - encoded audio data
    \Input iAudioSize       - size of audio

    \Version 01/08/2018 (jbrookes)
*/
/********************************************************************************F*/
static void _VoipHeadsetPlayEncodedAudio(VoipHeadsetRefT *pHeadset, PS4DecoderT *pDecoder, int32_t iDecoderIndex, const uint8_t *pEncAudioData, int32_t iAudioSize)
{
    int16_t aOutputBuf[VOIPHEADSET_FRAMESAMPLES];
    uint32_t uPortId;
    const uint8_t *pAudioData;

    if (pHeadset->bCrossPlay)
    {
        iAudioSize = _VoipHeadsetCodecDecode(aOutputBuf, sizeof(aOutputBuf), pEncAudioData, iAudioSize, iDecoderIndex);
        uPortId = pDecoder->uPcmPortId;
        pAudioData = (uint8_t *)aOutputBuf;
        iAudioSize = VOIPHEADSET_FRAMESIZE;
    }
    else
    {
        uPortId = pDecoder->uDecoderPortId;
        pAudioData = pEncAudioData;
    }

    // write the audio
    _VoipHeadsetWriteToPort(uPortId, pAudioData, iAudioSize);
    pDecoder->uLastUsage = NetTick();
}

/*F********************************************************************************/
/*!
    \Function _VoipHeadsetSendEncodedAudio

    \Description
        Send encoded data to the mic callback, or play it back locally if loopback

    \Input *pHeadset        - pointer to headset state
    \Input iLocalUserIndex  - local user index
    \Input *pEncAudioData   - encoded audio data
    \Input iAudioSize       - size of audio
    \Input bCameraMic       - TRUE if audio came from the camera mic, else FALSE

    \Version 01/08/2018 (jbrookes)
*/
/********************************************************************************F*/
static void _VoipHeadsetSendEncodedAudio(VoipHeadsetRefT *pHeadset, int32_t iLocalUserIndex, const uint8_t *pEncAudioData, int32_t iAudioSize, uint8_t bCameraMic)
{
    if (!pHeadset->bLoopback)
    {
        /* unconditionally (i.e. ignore headsets or TV/cam_mic being used) apply most restrictive voip settings to comply with Sony TR 4061 when there are
           multiple local users interacting with a game (note: that's different from EA MLU which implies multiple users authenticated with Blaze).
           The TR itself is not super clear aobut this, but we got some clear guidance from Sony about this here:
           https://ps4.scedev.net/support/issue/131717/_Need_help_with_interpretation_of_TR_4061. The argument is that there is no reliable way to make
           sure that the restricted user is not hearing the voice even if headsets are detected because what is detected as a headset connected to a
           controller can rather be a speaker. */
        if (pHeadset->bSharedPrivilege)
        {
            // send to mic data callback
            pHeadset->pMicDataCb(pEncAudioData, iAudioSize, NULL, 0, bCameraMic ? VOIP_SHARED_USER_INDEX : iLocalUserIndex, pHeadset->uSendSeq++, pHeadset->pCbUserData);
        }
    }
    else
    {
        PS4LocalVoipUserT *pLocalUser = &pHeadset->aLocalUsers[iLocalUserIndex];
        PS4DecoderT *pDecoder;

        _VoipHeadsetUpdateDecodersOwnership(pHeadset);

        // get local decoder and play back the audio
        if ((pDecoder = _VoipHeadsetGetDecoder(pHeadset, iLocalUserIndex, pLocalUser->iDecoderIndex, FALSE)) != NULL)
        {
            _VoipHeadsetPlayEncodedAudio(pHeadset, pDecoder, pLocalUser->iDecoderIndex, pEncAudioData, iAudioSize);
        }
        else
        {
            NetPrintfVerbose((pHeadset->iDebugLevel, 1, "voipheadsetps4: dropping chat data for local user index %d because no decoder available\n", iLocalUserIndex));
        }
    }
}

/*F********************************************************************************/
/*!
    \Function _VoipHeadsetEncode

    \Description
        Encode mic audio, using native or voipcodec encoder

    \Input *pHeadset        - pointer to headset state
    \Input *pLocalUser      - local user state
    \Input iLocalUserIndex  - local user index
    \Input *pRawAudioBuffer - audio to encode
    \Input iFrameBytes      - size of audio
    \Input bCameraMic       - TRUE if audio came from the camera mic, else FALSE

    \Output
        int32_t             - number of bytes of input audio that were encoded (used)

    \Version 01/11/2019 (jbrookes)
*/
/********************************************************************************F*/
static int32_t _VoipHeadsetEncode(VoipHeadsetRefT *pHeadset, PS4LocalVoipUserT *pLocalUser, int32_t iLocalUserIndex, const uint8_t *pRawAudioBuffer, int32_t iFrameBytes, uint8_t bCameraMic)
{
    VoipTranscribeRefT *pTranscribeRef = pLocalUser->pTranscribeRef;
    int32_t iFrameBytesEncoded=0;

    // the first time the transcription ref is used we will create it, then it will persist till headset destruction
    if ((pTranscribeRef == NULL) && pHeadset->bVoiceTranscriptionEnabled)
    {
        pTranscribeRef = _VoipHeadsetTranscribeCheckCreate(pHeadset, pLocalUser);
    }

    if (!pHeadset->bCrossPlay)
    {
        // feed audio input to the native encoder, unless we're doing text chat accessibility and not narrating
        if (!pHeadset->bTextChatAccessibility || (pLocalUser->iNarrating != 0))
        {
            iFrameBytesEncoded = _VoipHeadsetWriteToPort(pLocalUser->uPcmPortId, pRawAudioBuffer, iFrameBytes);
        }

        // transcribe audio if enabled
        if (pTranscribeRef != NULL)
        {
            uint32_t bDoTranscription = FALSE;

            // if we are doing text chat accessibility don't transcribe narrated audio
            if (pHeadset->bTextChatAccessibility)
            {
                if (pLocalUser->iNarrating == 0)
                { 
                    bDoTranscription = TRUE;
                }
            }
            else
            {
                /* unconditionally (i.e. ignore headsets or TV/cam_mic being used) apply most restrictive voip settings to comply with Sony TR 4061 when there are
                   multiple local users interacting with a game (note: that's different from EA MLU which implies multiple users authenticated with Blaze).
                   The TR itself is not super clear about this, but we got some clear guidance from Sony about this here:
                   https://ps4.scedev.net/support/issue/131717/_Need_help_with_interpretation_of_TR_4061. The argument is that there is no reliable way to make
                   sure that the restricted user is not hearing the voice even if headsets are detected because what is detected as a headset connected to a
                   controller can rather be a speaker. */
                if (pHeadset->bSharedPrivilege)
                {
                    bDoTranscription = TRUE;
                }
            }

            if (bDoTranscription)
            {

                VoipTranscribeSubmit(pTranscribeRef, pRawAudioBuffer, iFrameBytesEncoded);
            }
        }
    }
    else if (iFrameBytes >= VOIPHEADSET_FRAMESIZE)
    {
        // use cross-platform voipcodec encoder
        uint8_t aEncAudioBuffer[VOIP_MAXMICRPKTSIZE]; //$$todo - this should be much smaller, but voipopus requires this size for now
        int32_t iEncAudioSize = _VoipHeadsetCodecEncode(pLocalUser, aEncAudioBuffer, pRawAudioBuffer, VOIPHEADSET_FRAMESIZE, pTranscribeRef);
        if (iEncAudioSize > 0)
        {
            _VoipHeadsetSendEncodedAudio(pHeadset, iLocalUserIndex, aEncAudioBuffer, iEncAudioSize, bCameraMic);
            iFrameBytesEncoded = VOIPHEADSET_FRAMESIZE;
        }
    }

    return(iFrameBytesEncoded);
}

/*F********************************************************************************/
/*!
    \Function _VoipHeadsetPollForVoiceData

    \Description
        Poll to see if voice data is available.

    \Input *pHeadset    - pointer to headset state

    \Version 01/21/2013 (mclouatre)
*/
/********************************************************************************F*/
static void _VoipHeadsetPollForVoiceData(VoipHeadsetRefT *pHeadset)
{
    uint8_t aRawAudioBuffer[VOIPHEADSET_FRAMESIZE], aEncAudioBuffer[128], bCameraMic;
    int32_t iLocalUserIndex, iFrameBytes, iFrameBytesUsed;
    PS4LocalVoipUserT *pLocalUser;

    // see if any participating user is using a camera
    bCameraMic = VoipHeadsetStatus(pHeadset, 'camr', 0, NULL, 0);

    // loop through all local users, polling for audio
    for (iLocalUserIndex = 0; iLocalUserIndex < VOIP_MAXLOCALUSERS; iLocalUserIndex += 1)
    {
        // reference local user
        pLocalUser = &pHeadset->aLocalUsers[iLocalUserIndex];
        // skip empty/non-participating users
        if (VOIP_NullUser(&pLocalUser->user) || (pLocalUser->bParticipating != TRUE))
        {
            continue;
        }

        // update voip narration; this feeds into the audio input buffer when producing voice
        if (pLocalUser->pNarrateRef != NULL)
        {
            VoipNarrateUpdate(pLocalUser->pNarrateRef);
        }

        // read unencoded audio from audio input buffer (mic/narration audio)
        while ((iFrameBytes = iFrameBytesUsed = _VoipHeadsetReadAudioInput(pLocalUser, aRawAudioBuffer, VOIPHEADSET_FRAMESIZE)) > 0)
        {
            // process if frame is not silent, and we're not muted or are in loopback mode
            if (!_VoipHeadsetSilence(pHeadset, pLocalUser, (const int16_t *)aRawAudioBuffer, iFrameBytes) && (!pLocalUser->bMuted || pHeadset->bLoopback))
            {
                // encode the audio, and transcribe it if transcription is enabled
                iFrameBytesUsed = _VoipHeadsetEncode(pHeadset, pLocalUser, iLocalUserIndex, aRawAudioBuffer, iFrameBytes, bCameraMic);
            }
            else
            {
                // clear encoded audio rate tracker on silence
                pLocalUser->iEncAudioSamples = 0;
                pLocalUser->uEncAudioTick = 0;
            }
            // update the audio buffer with the number of bytes used
            _VoipHeadsetReadAudioInput(pLocalUser, NULL, iFrameBytesUsed);
            // check for end of narration audio; we need to make sure the narration is complete and all audio data consumed before we switch back to mic audio
            if ((pLocalUser->iNarrating == -1) && (pLocalUser->iAudioLen == 0))
            {
                pLocalUser->iAudioInp = pLocalUser->iAudioOut = 0;
                pLocalUser->iNarrating = 0;
            }
            // if not all of the audio was used, break out of the read loop as the encoder is full
            if (iFrameBytesUsed != iFrameBytes)
            {
                break;
            }
        }

        // if not cross-play/using voipcodec, we need to read async encoded data from the native encoder
        if (!pHeadset->bCrossPlay)
        {
            // poll for audio output by the encoder; note if we are using the camera mic we only need to read the voice packet from one of the players
            while ((iFrameBytes = _VoipHeadsetIsLocalVoiceFrameReady(pLocalUser->uEncoderPortId)) > 0)
            {
                //$$todo - do we really need this check?
                if (iFrameBytes >= (signed)sizeof(aEncAudioBuffer))
                {
                    NetPrintf(("voipheadsetps4: static buffer not big enough to read inbound voice frame on port %d for local user index %d\n",
                        pLocalUser->uEncoderPortId, iLocalUserIndex));
                    break;
                }

                // read the encoded audio
                if (_VoipHeadsetReadFromPort(pLocalUser->uEncoderPortId, aEncAudioBuffer, (uint32_t *)&iFrameBytes) == 0)
                {
                    _VoipHeadsetSendEncodedAudio(pHeadset, iLocalUserIndex, aEncAudioBuffer, iFrameBytes, bCameraMic);
                }
            }
        }

        // if the we are using the camera mic we only need to read the voice packet from one of the players
        if (bCameraMic)
        {
            break;
        }
    }
}

/*F********************************************************************************/
/*!
    \Function _VoipHeadsetProcessVoiceData

    \Description
        Process received voice data

    \Input *pHeadset            - pointer to headset state 
    \Input *pPassInRemoteUser   - remote user
    \Input *pMicrInfo           - micr info from inbound packet
    \Input *pPacketData         - pointer to beginning of data in packet payload

    \Version 01/09/2018 (jbrookes)
*/
/********************************************************************************F*/
static void _VoipHeadsetProcessVoiceData(VoipHeadsetRefT *pHeadset, VoipUserT *pPassInRemoteUser, VoipMicrInfoT *pMicrInfo, const uint8_t *pPacketData)
{
    PS4RemoteVoipUserT *pRemoteUser;
    PS4DecoderT *pDecoder;
    const uint8_t *pSubPacket;
    uint32_t uMicrPkt, uSubPacketSize;
    int32_t iRemoteUserSpaceIndex;

    // submit voice sub-packets
    for (uMicrPkt=0, pSubPacket=pPacketData; uMicrPkt < pMicrInfo->uNumSubPackets; uMicrPkt += 1, pSubPacket += uSubPacketSize)
    {
        // get the size of the subpacket based on variable or not
        uSubPacketSize = (pMicrInfo->uSubPacketSize != 0xFF) ? pMicrInfo->uSubPacketSize : *pSubPacket++;

        #if VOIPHEADSET_DEBUG
        NetPrintf(("voipheadsetps4: submitting chat data for remote user %lld at index %d\n", pRemoteUsers[pMicrInfo->uUserIndex].iPersonaId, pMicrInfo->uUserIndex));
        #endif

        // find user this packet is intended for
        for (iRemoteUserSpaceIndex = 0; iRemoteUserSpaceIndex < pHeadset->iMaxRemoteUsers; iRemoteUserSpaceIndex++)
        {
            pRemoteUser = &pHeadset->aRemoteUsers[iRemoteUserSpaceIndex];
            if (!VOIP_SameUser(&pRemoteUser->user, pPassInRemoteUser))
            {
                continue;
            }
            // get decoder (acquire one if necessary)
            if ((pDecoder = _VoipHeadsetGetDecoder(pHeadset, iRemoteUserSpaceIndex, pRemoteUser->iDecoderIndex, TRUE)) == NULL)
            {
                NetPrintfVerbose((pHeadset->iDebugLevel, 0, "voipheadsetps4: dropping chat data for remote user %lld at index %d because no decoder available\n",
                    pPassInRemoteUser->AccountInfo.iPersonaId, pMicrInfo->uUserIndex));
                break;
            }
            // make sure we're connected
            if (pDecoder->uConnectedSpkrPort == 0)
            {
                NetPrintfVerbose((pHeadset->iDebugLevel, 0, "voipheadsetps4: dropping chat data for remote user %lld at index %d because decoder has no connection\n",
                    pPassInRemoteUser->AccountInfo.iPersonaId, pMicrInfo->uUserIndex));
                break;
            }
            // decode and play the encoded audio
            _VoipHeadsetPlayEncodedAudio(pHeadset, pDecoder, pRemoteUser->iDecoderIndex, pSubPacket, uSubPacketSize);
        }
    }
}

/*F********************************************************************************/
/*!
    \Function _VoipHeadsetUpdateChatPrivileges

    \Description
        Updates Chat Privileges for all the local users

    \Input *pHeadset    - pointer to headset state

    \Version 01/15/2015 (tcho)
*/
/********************************************************************************F*/
static void _VoipHeadsetUpdateChatPrivileges(VoipHeadsetRefT *pHeadset)
{
    uint32_t uMask = NetConnStatus('mask', 0 , NULL, 0);
    uint32_t bOldSharedPrivilege = pHeadset->bSharedPrivilege;
    int32_t iLocalUserIndex;

    pHeadset->bSharedPrivilege = TRUE;
    for (iLocalUserIndex = 0; iLocalUserIndex < VOIP_MAXLOCALUSERS; ++iLocalUserIndex)
    {
        // make sure user is signed in with PSN
        if (uMask & (1 << iLocalUserIndex))
        {
            // query whether user can do voip or not
            uint8_t bPrivilege = !NetConnStatus('chat', iLocalUserIndex, NULL, 0);

            /* the shared privilege is set to be the most restrictive one from the set of "participating" users
               as defined by Sony. (Reference:  see R4061G from PS4-TestCase_for_TRC1.5_e.pdf)  That matches any
               user that is registered with DirtySDK voip OR any user that was identified as a contributor to
               the most restrictive voip privilege restriction by the game code via usage of '+pri' selector.
               Note: That however does not imply that the user has the bParticipating flag set to TRUE because
               that rather means the user is actively sending/receiving voip in the context of a blaze mesh. */
            if (!VOIP_NullUser(&pHeadset->aLocalUsers[iLocalUserIndex].user) || pHeadset->pVoipMostRestrictiveVoipContributors[iLocalUserIndex])
            {
                pHeadset->bSharedPrivilege = (bPrivilege ? pHeadset->bSharedPrivilege : FALSE);
            }
        }
    }

    // if the chat privileges changed, update the decoder connections accordingly
    if (pHeadset->bSharedPrivilege != bOldSharedPrivilege)
    {
        NetPrintf(("voipheadsetps4: decoder-to-speaker connections update scheduled because of change in shared user's chat privilege (%s vs %s)\n",
            (bOldSharedPrivilege?"TRUE":"FALSE"), (pHeadset->bSharedPrivilege?"TRUE":"FALSE")));
        pHeadset->bDecoderConnUpdateNeeded = TRUE;
    }
}

/*F********************************************************************************/
/*!
    \Function _VoipHeadsetChangeEncoderBps

    \Description
        Change bitrate configuration of all encoder and decoder ports.

    \Input *pHeadset    - pointer to headset state
    \Input iBitRate     - new bit rate

    \Output
        int32_t         - 0 for success; negative for failure

    \Version 05/14/2013 (mclouatre)
*/
/********************************************************************************F*/
static int32_t _VoipHeadsetChangeEncoderBps(VoipHeadsetRefT *pHeadset, int32_t iBitRate)
{
    int32_t iUserIndex;
    int32_t iDecoderIndex;
    int32_t iResult;
    int32_t iRetCode = 0; // default to success

    if (iBitRate == SCE_VOICE_BITRATE_3850 || iBitRate == SCE_VOICE_BITRATE_4650 || iBitRate == SCE_VOICE_BITRATE_5700 || iBitRate == SCE_VOICE_BITRATE_7300 ||
        iBitRate == SCE_VOICE_BITRATE_14400 || iBitRate == SCE_VOICE_BITRATE_16000 || iBitRate == SCE_VOICE_BITRATE_22533)
    {
        NetPrintf(("voipheadsetps4: encoding configuration changed from %d bps to %d bps\n", pHeadset->iEncoderBps, iBitRate));
        pHeadset->iEncoderBps = iBitRate;

        // modify bit rate of all encoder ports
        for (iUserIndex = 0; iUserIndex < VOIP_MAXLOCALUSERS; iUserIndex++)
        {
            if (!VOIP_NullUser(&pHeadset->aLocalUsers[iUserIndex].user))
            {
                if ((iResult = sceVoiceSetBitRate(pHeadset->aLocalUsers[iUserIndex].uEncoderPortId, pHeadset->iEncoderBps)) == SCE_OK)
                {
                    NetPrintf(("voipheadsetps4: successfully changed bitrate for encoder port %d belonging to local user index %d\n",
                        pHeadset->aLocalUsers[iUserIndex].uEncoderPortId, iUserIndex));
                }
                else
                {
                    NetPrintf(("voipheadsetps4: sceVoiceSetBitRate(%d) for local user index %d failed (err=%s)\n",
                        pHeadset->aLocalUsers[iUserIndex].uEncoderPortId, iUserIndex, DirtyErrGetName(iResult)));
                }
            }
        }

        // modify bit rate of all decoder ports
        for (iDecoderIndex = 0; iDecoderIndex < pHeadset->iDecoderPoolSize; iDecoderIndex++)
        {
            if (pHeadset->pDecoders[iDecoderIndex].bValid)
            {
                if ((iResult = sceVoiceSetBitRate(pHeadset->pDecoders[iDecoderIndex].uDecoderPortId, pHeadset->iEncoderBps)) == SCE_OK)
                {
                    NetPrintf(("voipheadsetps4: successfully changed bitrate for decoder port %d\n",
                        pHeadset->pDecoders[iDecoderIndex].uDecoderPortId));
                }
                else
                {
                    NetPrintf(("voipheadsetps4: sceVoiceSetBitRate(%d) failed (err=%s)\n",
                        pHeadset->pDecoders[iDecoderIndex].uDecoderPortId, DirtyErrGetName(iResult)));
                }
            }
        }
    }
    else
    {
        NetPrintf(("voipheadsetps4: %d bps is not a supported encoding configuration\n", iBitRate));
        iRetCode = -1;
    }

    return(iRetCode);
}

/*F********************************************************************************/
/*!
    \Function _VoipHeadsetBlockListCallback

    \Description
        Callback received from the UserList when data is received or the download is
        complete

    \Input *pRef            - userlist state
    \Input eResponseType    - type of response (TYPE_USER_DATA or TYPE_LIST_END)
    \Input *pEventData      - the data associated with the response type
    \Input *pUserData       - the userdata for this callback (PS4LocalUserT *)

    \Version 04/23/2020 (eesponda)
*/
/********************************************************************************F*/
static void _VoipHeadsetBlockListCallback(UserListApiRefT *pRef, UserListApiReturnTypeE eResponseType, UserListApiEventDataT *pEventData, void *pUserData)
{
    PS4LocalVoipUserT *pLocalUser = (PS4LocalVoipUserT *)pUserData;

    // if this is data write it to our local copy
    if (eResponseType == TYPE_USER_DATA)
    {
        DirtyUserToNativeUser(&pLocalUser->aPendingBlockList[pLocalUser->iPendingNumBlocked++], sizeof(pLocalUser->aPendingBlockList[0]), &pEventData->UserData.DirtyUser);
    }
    // handle end of the list
    else if (eResponseType == TYPE_LIST_END)
    {
        pLocalUser->bPendingBlockList = FALSE;
        pLocalUser->uNextBlockListCheck = NetTick() + VOIPHEADSET_BLOCKLIST_INTERVAL;

        // if we didn't get an error and the list differs commit and apply the changes
        if ((pEventData->ListEndData.Error == 0) && (memcmp(pLocalUser->aBlockList, pLocalUser->aPendingBlockList, sizeof(pLocalUser->aBlockList)) != 0))
        {
            ds_memcpy(pLocalUser->aBlockList, pLocalUser->aPendingBlockList, sizeof(pLocalUser->aBlockList));
            pLocalUser->iNumBlocked = pLocalUser->iPendingNumBlocked;
            pLocalUser->bMuteListUpdated = TRUE;
        }
        ds_memclr(pLocalUser->aPendingBlockList, sizeof(pLocalUser->aPendingBlockList));
        pLocalUser->iPendingNumBlocked = 0;
    }
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
    const uint32_t uNetTick = NetTick();
    int32_t iLocalUserIndex;

    for (iLocalUserIndex = 0; iLocalUserIndex < VOIP_MAXLOCALUSERS; iLocalUserIndex += 1)
    {
        PS4LocalVoipUserT *pLocalUser = &pHeadset->aLocalUsers[iLocalUserIndex];
        if (VOIP_NullUser(&pLocalUser->user))
        {
            continue;
        }

        /* check if it is time to request again. if we do not have a pending request, the user is participating and either we need to pull the next page or it is time to kick off a fresh request again
           make the request */
        if (!pLocalUser->bPendingBlockList && pLocalUser->bParticipating && (NetTickDiff(uNetTick, pLocalUser->uNextBlockListCheck) > 0))
        {
            if (UserListApiGetListAsync(pHeadset->pUserListApi, iLocalUserIndex, USERLISTAPI_TYPE_BLOCKED, VOIPHEADSET_BLOCKLIST_MAX, 0, NULL, _VoipHeadsetBlockListCallback, 0, pLocalUser) >= 0)
            {
                pLocalUser->bPendingBlockList = TRUE;
            }
            else
            {
                pLocalUser->uNextBlockListCheck = uNetTick + VOIPHEADSET_BLOCKLIST_INTERVAL;
            }
        }
    }

    UserListApiUpdate(pHeadset->pUserListApi);
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
    \Input *pOpaqueDataCb   - pointer to user callback to trigger when opaque data is ready (ignored)
    \Input *pStatusCb       - pointer to user callback to trigger when headset status changes
    \Input *pCbUserData     - pointer to user callback data
    \Input iAttr            - platform-specific - use to specify the voice lib thread attributes on PS4 

    \Output
        VoipHeadsetRefT *   - pointer to module state, or NULL if an error occured
    
    \Notes
        \verbatim
            Valid value for iAttr:
            The higher 16 bit is the CPU Affinity value. The lower 16 bits is the scheduling priority.
            For example iAttr = (SCE_KERNEL_CPUMASK_6CPU_ALL << 16) | SCE_KERNEL_PRIO_FIFO_HIGHEST.
        \endverbatim

    \Version 16/01/2013 (mclouatre)
*/
/********************************************************************************F*/
VoipHeadsetRefT *VoipHeadsetCreate(int32_t iMaxChannels, VoipHeadsetMicDataCbT *pMicDataCb, VoipHeadsetTextDataCbT *pTextDataCb, VoipHeadsetOpaqueDataCbT *pOpaqueDataCb, VoipHeadsetStatusCbT *pStatusCb, void *pCbUserData, int32_t iAttr)
{
    int32_t iSize, iSchedPriority;
    SceKernelCpumask cpuAffinity; 
    VoipHeadsetRefT *pHeadset;
    void *pMemGroupUserData;
    int32_t iMemGroup;

    // query current mem group data
    VoipCommonMemGroupQuery(&iMemGroup, &pMemGroupUserData);

    // make sure we don't exceed maxchannels
    if (iMaxChannels > VOIP_MAXCONNECT)
    {
        NetPrintf(("voipheadsetps4: request for %d channels exceeds max\n", iMaxChannels));
        return(NULL);
    }

    // calculate size of module state
    // the size of the remote user array is the max number of remote peers + the max number of remote shared users
    iSize = sizeof(*pHeadset) + (sizeof(PS4RemoteVoipUserT) * (iMaxChannels + VOIP_MAX_LOW_LEVEL_CONNS - 1));

    // allocate and clear module state
    if ((pHeadset = DirtyMemAlloc(iSize, VOIP_MEMID, iMemGroup, pMemGroupUserData)) == NULL)
    {
        NetPrintf(("voipheadsetps4: not enough memory to allocate module state\n"));
        return(NULL);
    }
    ds_memclr(pHeadset, iSize);

    pHeadset->iMemGroup = iMemGroup;
    pHeadset->pMemGroupUserData = pMemGroupUserData;
    pHeadset->iMaxRemoteUsers = iMaxChannels + VOIP_MAX_LOW_LEVEL_CONNS;
    pHeadset->iEncoderBps = VOIPHEADSET_SAMPLERATE;
    // init silence (vad) detection parameters
    pHeadset->iVadPowerThreshold = (32768*32768)/6666; // 32k squared is max amplitude squared, 1/6666 gives us the 0.00015f we use in voipcodec
    pHeadset->iVadSilenceThreshold = 32*VOIPHEADSET_FRAMESAMPLES; // 32 consecutive 20ms frames of silence (.64s) before declaring voice stream silent

    // default voice lib thread attribute
    if ((cpuAffinity = NetConnStatus('affn', 0, NULL, 0)) <= 0)
    {
        cpuAffinity = SCE_KERNEL_CPUMASK_6CPU_ALL;
    }
    iSchedPriority = SCE_KERNEL_PRIO_FIFO_HIGHEST;

    // applying custom voice lib thread attributes if specified
    if (iAttr != 0)
    {
        iSchedPriority = iAttr & 0xFFFF;
        cpuAffinity = (SceKernelCpumask)(iAttr >> 16);
    }

    // set default debuglevel
    pHeadset->iDebugLevel = 1;

    // save callback info
    pHeadset->pMicDataCb = pMicDataCb;
    pHeadset->pTextDataCb = pTextDataCb;
    pHeadset->pStatusCb = pStatusCb;
    pHeadset->pCbUserData = pCbUserData;
    pHeadset->iMaxChannels = iMaxChannels;

    // create the userlist module
    if ((pHeadset->pUserListApi = UserListApiCreate(0)) == NULL)
    {
        NetPrintf(("voipheadsetps4: unable to create the userlist module required for blocked user lookups\n"));
        VoipHeadsetDestroy(pHeadset);
        return(NULL);
    }

    // init sony voice lib (audio I/O systems)
    if (_VoipHeadsetVoiceLibInit(pHeadset) < 0)
    {
        NetPrintf(("voipheadsetps4: failed to initialize voice lib\n"));
        VoipHeadsetDestroy(pHeadset);
        return(NULL);
    }
    pHeadset->bVoiceLibInit = TRUE;

    // allocate and init pool of decoders
    // (needs to be performed after _VoipHeadsetVoiceLibInit() which initializes pHeadset->iDecoderPoolSize)
    if ((pHeadset->pDecoders = (PS4DecoderT *)DirtyMemAlloc(sizeof(PS4DecoderT) * pHeadset->iDecoderPoolSize, VOIP_MEMID, iMemGroup, pMemGroupUserData)) == NULL)
    {
        NetPrintf(("voipheadsetps4: not enough memory to allocate pool of decoders\n"));
        VoipHeadsetDestroy(pHeadset);
        return(NULL);
    }
    ds_memclr(pHeadset->pDecoders, sizeof(PS4DecoderT) * pHeadset->iDecoderPoolSize);

    // set priority and affinity of internal voice and audio ports threads
    if (_VoipHeadsetVoiceSetThreadsParams(pHeadset, cpuAffinity, iSchedPriority) < 0)
    {
        NetPrintf(("voipheadsetps4: failed to modify attributes of internal voice trheads\n"));
        VoipHeadsetDestroy(pHeadset);
        return(NULL);
    }

    // start sony voice lib
    if (_VoipHeadsetVoiceLibStart(pHeadset) < 0)
    {
        NetPrintf(("voipheadsetps4: failed to start voice lib\n"));
        VoipHeadsetDestroy(pHeadset);
        return(NULL);
    }
    pHeadset->bVoiceLibStart = TRUE;

    // return module ref to caller
    return(pHeadset);
}

/*F********************************************************************************/
/*!
    \Function   VoipHeadsetDestroy

    \Description
        Destroy the headset manager.

    \Input *pHeadset    - pointer to headset state

    \Version 16/01/2013 (mclouatre)
*/
/********************************************************************************F*/
void VoipHeadsetDestroy(VoipHeadsetRefT *pHeadset)
{
    int32_t iDecoderIndex, iLocalUserIndex;
    PS4LocalVoipUserT *pLocalUser;

    // release all user resources
    for (iLocalUserIndex = 0; iLocalUserIndex < VOIP_MAXLOCALUSERS; ++iLocalUserIndex)
    {
        pLocalUser = &pHeadset->aLocalUsers[iLocalUserIndex];

        // narrate
        if (pLocalUser->pNarrateRef != NULL)
        {
            VoipNarrateDestroy(pLocalUser->pNarrateRef);
        }
        // transcription
        if (pLocalUser->pTranscribeRef != NULL)
        {
            VoipTranscribeDestroy(pLocalUser->pTranscribeRef);
        }    
        // ps4 user
        if (!VOIP_NullUser(&pLocalUser->user))
        {
            if (pLocalUser->bParticipating)
            {
                _VoipHeadsetDeactivateLocalTalker(pHeadset, iLocalUserIndex);
            }
            _VoipHeadsetDeleteLocalTalker(pHeadset, iLocalUserIndex, &pLocalUser->user);
        }
    }

    // stop sony voice lib
    if (pHeadset->bVoiceLibStart)
    {
        _VoipHeadsetVoiceLibStop();
    }

    // release all decoder resources
    if (pHeadset->pDecoders != NULL)
    {
        for (iDecoderIndex = 0; iDecoderIndex < pHeadset->iDecoderPoolSize; iDecoderIndex++)
        {
            if (pHeadset->pDecoders[iDecoderIndex].bValid)
            {
                _VoipHeadsetPortDelete(pHeadset->pDecoders[iDecoderIndex].uDecoderPortId);
                _VoipHeadsetPortDelete(pHeadset->pDecoders[iDecoderIndex].uPcmPortId);
            }
        }
        DirtyMemFree(pHeadset->pDecoders, VOIP_MEMID, pHeadset->iMemGroup, pHeadset->pMemGroupUserData);
    }

    // free active codec
    VoipCodecDestroy();

    // shutdown sony voice lib (audio I/O systems)
    if (pHeadset->bVoiceLibInit)
    {
        _VoipHeadsetVoiceLibShutdown();
    }

    // release the userlist module
    if (pHeadset->pUserListApi != NULL)
    {
        UserListApiDestroy(pHeadset->pUserListApi);
        pHeadset->pUserListApi = NULL;
    }

    // dispose of module memory and pool of decoders
    DirtyMemFree(pHeadset, VOIP_MEMID, pHeadset->iMemGroup, pHeadset->pMemGroupUserData);
}

/*F********************************************************************************/
/*!
    \Function   VoipHeadsetReceiveVoiceDataCb

    \Description
        Connectionlist callback to handle receiving a voice packet from a remote peer.

    \Input *pRemoteUsers - user we're receiving the voice data from
    \Input iRemoteUserSize  - pRemoteUsers array size
    \Input iConsoleId    - generic identifier for the console to which the users belong
    \Input *pMicrInfo    - micr info from inbound packet
    \Input *pPacketData  - pointer to beginning of data in packet payload
    \Input *pUserData    - VoipHeadsetT ref

    \Version 16/01/2013 (mclouatre)
*/
/********************************************************************************F*/
void VoipHeadsetReceiveVoiceDataCb(VoipUserT *pRemoteUsers, int32_t iRemoteUserSize, int32_t iConsoleId, VoipMicrInfoT *pMicrInfo, uint8_t *pPacketData, void *pUserData)
{
    VoipHeadsetRefT *pHeadset = (VoipHeadsetRefT *)pUserData;
    uint32_t uRemoteUserIndex = 0;

    // update decoder ownership
    _VoipHeadsetUpdateDecodersOwnership(pHeadset);

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
            NetPrintf(("voipheadsetps4: discarding voice packet from shared user because we cannot find a remote user to play it back as!\n"));
            return;
        }
    }
    else
    {
        uRemoteUserIndex = pMicrInfo->uUserIndex;
    }

    // process mic data
    _VoipHeadsetProcessVoiceData(pHeadset, &pRemoteUsers[uRemoteUserIndex], pMicrInfo, pPacketData);
}

/*F********************************************************************************/
/*!
    \Function   VoipHeadsetRegisterUserCb

    \Description
        Connectionlist callback to register a new remote user with the 1st party voice system

    \Input *pRemoteUser     - user to register
    \Input iConsoleId       - generic identifier for the console to which the user belongs (ignored)
    \Input bRegister        - true=register, false=unregister
    \Input *pUserData       - voipheadset module ref

    \Version 16/01/2013 (mclouatre)
*/
/********************************************************************************F*/
void VoipHeadsetRegisterUserCb(VoipUserT *pRemoteUser, int32_t iConsoleId, uint32_t bRegister, void *pUserData)
{
    VoipHeadsetRefT *pHeadset = (VoipHeadsetRefT *)pUserData;
    int32_t iRemoteUserSpaceIndex;

    // early exit if invalid remote talker
    if (VOIP_NullUser(pRemoteUser))
    {
        NetPrintf(("voipheadsetps4: can't %s NULL remote talker\n", (bRegister?"register":"unregister")));
        return;
    }

    if (bRegister)
    {
        // find free remote user space
        for (iRemoteUserSpaceIndex = 0; iRemoteUserSpaceIndex < pHeadset->iMaxRemoteUsers; iRemoteUserSpaceIndex++)
        {
            if (VOIP_NullUser(&pHeadset->aRemoteUsers[iRemoteUserSpaceIndex].user))
            {
                // validate that the first party id callback has been registered
                if (pHeadset->pFirstPartyIdCb == NULL)
                {
                    NetPrintf(("voipheadsetps4: cannot add remote talker if the first party id callback is not registered\n"));
                    return;
                }
                // if the user is another sony device, retrieve the account id
                pHeadset->aRemoteUsers[iRemoteUserSpaceIndex].uAccountId = (pRemoteUser->ePlatform == VOIP_PLATFORM_PS4) ? pHeadset->pFirstPartyIdCb(pRemoteUser->AccountInfo.iPersonaId, pHeadset->pFirstPartyIdCbUserData) : 0;

                VOIP_CopyUser(&pHeadset->aRemoteUsers[iRemoteUserSpaceIndex].user, pRemoteUser);
                pHeadset->aRemoteUsers[iRemoteUserSpaceIndex].iDecoderIndex = VOIPHEADSET_INVALID_INDEX;
                pHeadset->aRemoteUsers[iRemoteUserSpaceIndex].uEnablePlayback = -1; // enable playback by default
                NetPrintf(("voipheadsetps4: registered remote talker %lld at remote user index %d\n", pRemoteUser->AccountInfo.iPersonaId, iRemoteUserSpaceIndex));
                break;
            }
        }

        if (iRemoteUserSpaceIndex < pHeadset->iMaxRemoteUsers)
        {
            // attempt to create an additional decoder in the pool (if there is still room for it)
            if (_VoipHeadsetCreateDecoder(pHeadset) == 0)
            {
                // make sure all decoders are connected to all speaker ports of all participating users
                pHeadset->bDecoderConnUpdateNeeded = TRUE;
            }
        }
        else
        {
            NetPrintf(("voipheadsetps4: failed to register remote talker %lld, max number of channels reached\n", pRemoteUser->AccountInfo.iPersonaId));
        }
    }
    else // unregister
    {
        // find matching user 
        for (iRemoteUserSpaceIndex = 0; iRemoteUserSpaceIndex < pHeadset->iMaxRemoteUsers; iRemoteUserSpaceIndex++)
        {
            if (VOIP_SameUser(&pHeadset->aRemoteUsers[iRemoteUserSpaceIndex].user, pRemoteUser))
            {
                // release decoder if needed
                if (pHeadset->aRemoteUsers[iRemoteUserSpaceIndex].iDecoderIndex != VOIPHEADSET_INVALID_INDEX)
                {
                    _VoipHeadsetReleaseDecoderOwnership(pHeadset, pHeadset->aRemoteUsers[iRemoteUserSpaceIndex].iDecoderIndex);
                }

                pHeadset->aRemoteUsers[iRemoteUserSpaceIndex].uEnablePlayback = -1; // clear playback bit field by default
                NetPrintf(("voipheadsetps4: unregistered remote talker %lld at remote user index %d\n", pRemoteUser->AccountInfo.iPersonaId, iRemoteUserSpaceIndex));

                VOIP_ClearUser(&pHeadset->aRemoteUsers[iRemoteUserSpaceIndex].user);
                break;
            }
        }
    }
}

/*F********************************************************************************/
/*!
    \Function _VoipHeadsetProcessTranscription

    \Description
        Process voice transcription

    \Input pHeadset     - headset ref

    \Version 09/07/2018 (tcho)
*/
/********************************************************************************F*/
static void _VoipHeadsetProcessTranscription(VoipHeadsetRefT *pHeadset)
{
    char strTranscribeBuf[VOIPTRANSCRIBE_OUTPUT_MAX];
    PS4LocalVoipUserT *pLocalUser;
    int32_t iLocalUserIndex;

    // loop through all local users
    for (iLocalUserIndex = 0; iLocalUserIndex < VOIP_MAXLOCALUSERS; iLocalUserIndex += 1)
    {
        // reference local user
        pLocalUser = &pHeadset->aLocalUsers[iLocalUserIndex];

        // skip empty/non-participating users
        if (VOIP_NullUser(&pLocalUser->user) || (pLocalUser->bParticipating != TRUE))
        {
            continue;
        }
        // skip users without a transcription ref
        if (pLocalUser->pTranscribeRef == NULL)
        {
            continue;
        }

        // process transcription
        VoipTranscribeUpdate(pLocalUser->pTranscribeRef);

        // if a transcription is available, send it along
        if (VoipTranscribeGet(pLocalUser->pTranscribeRef, strTranscribeBuf, sizeof(strTranscribeBuf)) > 0)
        {
            pHeadset->pTextDataCb(strTranscribeBuf, iLocalUserIndex, pHeadset->pCbUserData);
        }

        // enable short audio discard if not narrating
        if (pLocalUser->iNarrating == 0)
        {
            VoipTranscribeControl(pLocalUser->pTranscribeRef, 'vdis', TRUE, 0, NULL);
        }
    }
}

/*F********************************************************************************/
/*!
    \Function   VoipHeadsetProcess

    \Description
        Headset process function.

    \Input *pHeadset    - pointer to headset state
    \Input uFrameCount  - current frame count

    \Version 16/01/2013 (mclouatre)
*/
/********************************************************************************F*/
void VoipHeadsetProcess(VoipHeadsetRefT *pHeadset, uint32_t uFrameCount)
{
    _VoipHeadsetUpdateChatPrivileges(pHeadset);
    _VoipHeadsetProcessBlockList(pHeadset);

    if (pHeadset->bDecoderConnUpdateNeeded == TRUE)
    {
        if (pHeadset->bDecoderConnUpdateInProgress == FALSE)
        {
           pHeadset->bDecoderConnUpdateInProgress = TRUE;
           pHeadset->bDecoderConnUpdateNeeded = FALSE;

           _VoipHeadsetUpdateDecoderConnections(pHeadset);

           pHeadset->bDecoderConnUpdateInProgress = FALSE;
        }
    }
    
    _VoipHeadsetUpdateStatus(pHeadset);
    _VoipHeadsetPollForVoiceData(pHeadset);

    _VoipHeadsetProcessTranscription(pHeadset);
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

    \Version 16/01/2013 (mclouatre)
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
            'aloc' - activate/deactivate local user (enter/exit "participating" state)
            'cdec' - create new codec
            'creg' - register codec
            'cstm' - clear speech to text metrics in VoipSpeechToTextMetricsT
            'ctsm' - clear text to speech metrics in VoipTextToSpeechMetricsT
            'epbs' - modified encoding bps generation configuration (native/non-crossplay codec)
            'loop' - enable/disable loopback
            'mute' - enable/disable audio input muting
            '+pbk' - enable voice playback for a given remote user (pValue is the remote user VoipUserT).
            '-pbk' - disable voice playback for a given remote user (pValue is the remote user VoipUserT).
            'rloc' - register/unregister local user
            'spam' - debug verbosity level
            'txta' - enable/disable text chat accessibility, this also controls loopback in this context
            'tran' - enable/disable local generation of transcribed text for speech produced by local users (speech-to-text component)
            'ttos' - request voice narration; iValue=local user index, pValue=utf8-encoded text
            'voic' - set voice synthesized speech config for a local user; iValue=local user index, pValue=pointer to VoipSynthesizedSpeechCfgT
            'vpri' - used by the voip layer to pass pointer to array of contributors to voip most restrictive privilege
            'xply' - enable/disable crossplay
        \endverbatim

    \Version 16/01/2013 (mclouatre)
*/
/********************************************************************************F*/
int32_t VoipHeadsetControl(VoipHeadsetRefT *pHeadset, int32_t iControl, int32_t iValue, int32_t iValue2, void *pValue)
{
    PS4LocalVoipUserT *pLocalUser;

    if (iControl == 'aloc')
    {
        return(iValue2 ? _VoipHeadsetActivateLocalTalker(pHeadset, iValue, (VoipUserT *)pValue) : _VoipHeadsetDeactivateLocalTalker(pHeadset, iValue));
    }
    if (iControl == 'cdec')
    {
        return(VoipCodecCreate(iValue, pHeadset->iDecoderPoolSize));
    }
    if (iControl == 'creg')
    {
        VoipCodecRegister(iValue, pValue);
        return(0);
    }
    if (iControl == 'cstm')
    {
        VoipTranscribeRefT *pTranscribeRef = pHeadset->aLocalUsers[iValue].pTranscribeRef;
        if (pTranscribeRef != NULL)
        {
            return(VoipTranscribeControl(pTranscribeRef, iControl, iValue, 0, NULL));
        }
        else
        {
            // pTranscribeRef == NULL
            return(-1);
        }
    }
    if (iControl == 'ctsm')
    {
        VoipNarrateRefT *pNarrateRef = pHeadset->aLocalUsers[iValue].pNarrateRef;
        if (pNarrateRef != NULL)
        {
            return(VoipNarrateControl(pNarrateRef, iControl, iValue, 0, NULL));
        }
        else
        {
            // pNarrateRef == NULL
            return(-1);
        }
    }
    if (iControl == 'ebps')
    {
        return(_VoipHeadsetChangeEncoderBps(pHeadset, iValue));
    }
    if (iControl == 'loop')
    {
        pHeadset->bLoopback = iValue;
        return(0);
    }
    if ((iControl == 'mute') && (iValue < VOIP_MAXLOCALUSERS))
    {
        uint8_t bMuted = iValue2 ? TRUE : FALSE;
        pLocalUser = &pHeadset->aLocalUsers[iValue];
        if (pLocalUser->bMuted != bMuted)
        {
            NetPrintfVerbose((pHeadset->iDebugLevel, 1, "voipheadsetps4: mute %s\n", (pLocalUser->bMuted ? "enabled" : "disabled")));
            pLocalUser->bMuted = bMuted;
        }
        return(0);
    }
    if ((iControl == '-pbk') || (iControl == '+pbk' ))
    {
        int8_t bVoiceEnable = (iControl == '+pbk')? TRUE: FALSE; 
        int32_t iIndex;
        VoipUserT *pRemoteUser;

        if ((pValue != NULL) && (iValue < VOIP_MAXLOCALUSERS_EXTENDED))
        {
            pRemoteUser = (VoipUserT *)pValue;

            // look up the remote user and set playback
            for (iIndex = 0; iIndex < pHeadset->iMaxRemoteUsers; ++iIndex)
            {
                if (VOIP_SameUser(&pHeadset->aRemoteUsers[iIndex].user, pRemoteUser))
                {
                    if (bVoiceEnable == TRUE)
                    {
                         pHeadset->aRemoteUsers[iIndex].uEnablePlayback |= (1 << iValue);
                         NetPrintf(("voipheadsetps4:VoipHeadsetControl(+pbk) voice playback enabled for remote User Index %i, bPlayback bit field 0x%08X\n", iIndex, pHeadset->aRemoteUsers[iIndex].uEnablePlayback));
                    }
                    else
                    {
                         pHeadset->aRemoteUsers[iIndex].uEnablePlayback &= ~(1 << iValue);
                         NetPrintf(("voipheadsetps4:VoipHeadsetControl(-pbk) voice playback disabled for remote User Index %i, bPlayback bit field 0x%08X\n", iIndex, pHeadset->aRemoteUsers[iIndex].uEnablePlayback));
                    }

                    pHeadset->bDecoderConnUpdateNeeded =TRUE;
                    return(0);
                }
            }
            
            NetPrintf(("voipheadsetps4: VoipHeadsetControl('%s') no remote user was found. User: %lld\n", bVoiceEnable? "+pbk":"-pbk", pRemoteUser->AccountInfo.iPersonaId));
            return(-2);
        }
        else
        {
            NetPrintf(("voipheadsetps4: VoipHeadsetControl('%s') invalid arguments\n", bVoiceEnable? "+pbk":"-pbk"));
            return(-1);
        }
    }
    if (iControl == 'rloc')
    {
        return(iValue2 ? _VoipHeadsetAddLocalTalker(pHeadset, iValue, (VoipUserT *)pValue) : _VoipHeadsetDeleteLocalTalker(pHeadset, iValue, (VoipUserT *)pValue));
    }
    #if DIRTYCODE_LOGGING
    if (iControl == 'spam')
    {
        int32_t iLocalUserIndex;
        // set headset debug level
        NetPrintf(("voipheadsetps4: setting debuglevel=%d\n", iValue));
        pHeadset->iDebugLevel = iValue;
        // pass debug level down to transcribe/narrate refs
        for (iLocalUserIndex = 0; iLocalUserIndex < VOIP_MAXLOCALUSERS; iLocalUserIndex += 1)
        {
            pLocalUser = &pHeadset->aLocalUsers[iLocalUserIndex];
            if (pLocalUser->pTranscribeRef != NULL)
            {
                VoipTranscribeControl(pLocalUser->pTranscribeRef, iControl, iValue, 0, NULL);
            }
            if (pLocalUser->pNarrateRef != NULL)
            {
                VoipNarrateControl(pLocalUser->pNarrateRef, iControl, iValue, 0, NULL);
            }
        }
        return(0);
    }
    #endif
    if (iControl == 'txta')
    {
        pHeadset->bTextChatAccessibility = pHeadset->bLoopback = iValue;
        NetPrintf(("voipheadsetps4: text chat accessibility mode %s\n", (iValue ? "enabled" : "disabled")));
        return(0);
    }
    if (iControl == 'tran')
    {
        if (pHeadset->bVoiceTranscriptionEnabled != iValue)
        {
            pHeadset->bVoiceTranscriptionEnabled = iValue;
            NetPrintf(("voipheadsetps4: %s voice transcription locally\n", pHeadset->bVoiceTranscriptionEnabled ? "enabling" : "disabling"));
        }
        return (0);
    }
    if ((iControl == 'ttos') && (iValue < VOIP_MAXLOCALUSERS))
    {
        pLocalUser = &pHeadset->aLocalUsers[iValue];
        if (pLocalUser->pNarrateRef == NULL)
        {
            if ((pLocalUser->pNarrateRef = VoipNarrateCreate(_VoipHeadsetNarrateCb, pHeadset)) != NULL)
            {
                // set the debug level to the current setting
                #if DIRTYCODE_LOGGING
                VoipNarrateControl(pLocalUser->pNarrateRef, 'spam', pHeadset->iDebugLevel, 0, NULL);
                #endif            
            }
        }
        if (pLocalUser->pNarrateRef != NULL)
        {
            return(VoipNarrateInput(pLocalUser->pNarrateRef, iValue, pLocalUser->uGender, (const char *)pValue));
        }
        else
        {
            NetPrintf(("voipheadsetps4: failed to start narration module\n"));
            return(-1);
        }
    }
    if (iControl == 'voic')
    {
        if ((iValue < VOIP_MAXLOCALUSERS) && (pValue != NULL))
        {
            const VoipSynthesizedSpeechCfgT *pCfg = (const VoipSynthesizedSpeechCfgT *)pValue;
            pLocalUser = &pHeadset->aLocalUsers[iValue];
            pLocalUser->uGender = pCfg->iPersonaGender == 1 ? VOIPNARRATE_GENDER_FEMALE : VOIPNARRATE_GENDER_MALE;
            NetPrintf(("voipheadsetps4: setting tts voice to %s\n", (pLocalUser->uGender == VOIPNARRATE_GENDER_MALE) ? "male" : "female"));
            return(0);
        }
        else
        {
            NetPrintf(("voipheadsetps4: VoipHeadsetControl('voic') invalid parameters\n"));
            return(-1);
        }
    }
    if (iControl == 'vpri')
    {
        pHeadset->pVoipMostRestrictiveVoipContributors = (uint8_t *) pValue;
        return(0);
    }
    if (iControl == 'xply')
    {
        uint8_t bCrossPlay = iValue ? TRUE : FALSE;

        if (pHeadset->bCrossPlay != bCrossPlay)
        {
            // disconnect speaker ports with current settings
            _VoipHeadsetDisconnectLocalTalkers(pHeadset);
            // update crossplay status
            pHeadset->bCrossPlay = bCrossPlay;
            NetPrintf(("voipheadsetps4: cross-play %s\n", pHeadset->bCrossPlay ? "enabled" : "disabled"));
            // reconnect speaker ports with updated settings
            _VoipHeadsetUpdateDecoderConnections(pHeadset);
        }

        return(0);
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
            'camr' - return TRUE is the PS Camera Mic is in use.
            'fpml' - return the first party mute setting between a local user (iValue) and a remote user (VoipUserT * in pBuf); only applies if users are of the same platform
            'fpmu' - return TRUE if the firsty party mute list has been updated for a local user (index via iValue)
            'mute' - get muted status
            'ruvu' - return TRUE if the given remote user (pBuf) is registered with voipheadset, FALSE if not.
            'sact' - return TRUE if shared device (PS Camera) is active
            'hpao' - return the audio device output flag for a given user index (iValue). IMPORTANT: value is not valid until something has been written to the outpout port
            'sttm' - get the VoipSpeechToTextMetricsT via pBuf, user index is iValue
            'ttos' - returns TRUE if TTS is active, else FALSE
            'ttsm' - get the VoipTextToSpeechMetricsT via pBuf, user index is iValue
        \endverbatim

        Unhandled selectors are pass through to VoipCodecStatus()

    \Version 16/01/2013 (mclouatre)
*/
/********************************************************************************F*/
int32_t VoipHeadsetStatus(VoipHeadsetRefT *pHeadset, int32_t iSelect, int32_t iValue, void *pBuf, int32_t iBufSize)
{
    if (iSelect == 'camr')
    {
        #ifndef DIRTYCODE_PS5
        int32_t iLocalUserIndex;
        uint8_t bCameraFound = FALSE;

        for (iLocalUserIndex = 0; iLocalUserIndex < VOIP_MAXLOCALUSERS; ++iLocalUserIndex)
        {
            if (!VOIP_NullUser(&pHeadset->aLocalUsers[iLocalUserIndex].user) && (pHeadset->aLocalUsers[iLocalUserIndex].bParticipating == TRUE))
            {
                if (sceAudioInIsSharedDevice(pHeadset->aLocalUsers[iLocalUserIndex].uMicPortId) == TRUE)
                {
                    bCameraFound = TRUE;
                    break;
                }
            }
        }
        return (bCameraFound);
        #else   
        // as per https://p.siedev.net/support/issue/3511/_Need_AudioIn_03_for_PS5 there are no shared audio inputs on PS4
        return(FALSE);
        #endif
    }
    if (iSelect == 'fpml')
    {
        int32_t iRemoteUserIndex, iBlockListIndex, iLocalUserIndex;

        // try to find the user in the list of remote users
        for (iRemoteUserIndex = 0; iRemoteUserIndex < pHeadset->iMaxRemoteUsers; iRemoteUserIndex += 1)
        {
            if (VOIP_SameUser(&pHeadset->aRemoteUsers[iRemoteUserIndex].user, (VoipUserT *)pBuf))
            {
                break;
            }
        }

        // validate the inputs
        if ((iValue < 0) || (iValue >= VOIP_MAXLOCALUSERS_EXTENDED) || (iRemoteUserIndex == pHeadset->iMaxRemoteUsers))
        {
            return(FALSE);
        }

        // if the user is not another sony device return not muted
        if (pHeadset->aRemoteUsers[iRemoteUserIndex].user.ePlatform != VOIP_PLATFORM_PS4)
        {
            return(FALSE);
        }

        // try to find user in list of blocked users (shared local user case)
        if (iValue == VOIP_SHARED_USER_INDEX)
        {
            for (iLocalUserIndex = 0; iLocalUserIndex < VOIP_MAXLOCALUSERS; iLocalUserIndex += 1)
            {
                for (iBlockListIndex = 0; iBlockListIndex < pHeadset->aLocalUsers[iLocalUserIndex].iNumBlocked; iBlockListIndex += 1)
                {
                    if (pHeadset->aLocalUsers[iLocalUserIndex].aBlockList[iBlockListIndex] == pHeadset->aRemoteUsers[iRemoteUserIndex].uAccountId)
                    {
                        return(TRUE);
                    }
                }
            }
            return(FALSE);
        }

        // check that we have a valid local user in this case
        if (VOIP_NullUser(&pHeadset->aLocalUsers[iValue].user))
        {
            return(FALSE);
        }

        // try to find user in user in list of block users (non-shared local user case)
        for (iBlockListIndex = 0; iBlockListIndex < pHeadset->aLocalUsers[iValue].iNumBlocked; iBlockListIndex += 1)
        {
            if (pHeadset->aLocalUsers[iValue].aBlockList[iBlockListIndex] == pHeadset->aRemoteUsers[iRemoteUserIndex].uAccountId)
            {
                return(TRUE);
            }
        }
        return(FALSE);
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
    if ((iSelect == 'mute') && (iValue < VOIP_MAXLOCALUSERS))
    {
        return(pHeadset->aLocalUsers[iValue].bMuted);
    }
    if (iSelect == 'ruvu')
    {
        int32_t iRemoteUserSpaceIndex = 0;

        for (iRemoteUserSpaceIndex = 0; iRemoteUserSpaceIndex < pHeadset->iMaxRemoteUsers; ++iRemoteUserSpaceIndex)
        {
            if (VOIP_SameUser(&pHeadset->aRemoteUsers[iRemoteUserSpaceIndex].user, (VoipUserT *)pBuf))
            {
                // remote user found and it is registered with voipheadset
                return (TRUE);
            }
        }

        return(FALSE);
    }
    if (iSelect == 'sact')
    {
        return(VoipHeadsetStatus(pHeadset, 'camr', 0, NULL, 0) && (pHeadset->uLastMicStatus == 0));
    }
    if (iSelect == 'hpao')
    {
        int32_t iResult;
        int16_t iOutput = 0;

        if (!VOIP_NullUser(&pHeadset->aLocalUsers[iValue].user) && (pHeadset->aLocalUsers[iValue].bParticipating == TRUE))
        {
            if ((iResult = sceVoiceGetPortAttr(pHeadset->aLocalUsers[iValue].uSpkrPortId, SCE_VOICE_ATTR_AUDIOOUTPUT_DESTINATION , &iOutput, sizeof(iOutput))) == SCE_OK)
            {
                return(iOutput);
            }
        }
    }
    if (iSelect == 'sttm')
    {
        VoipTranscribeRefT *pTranscribeRef = pHeadset->aLocalUsers[iValue].pTranscribeRef;
        if (pTranscribeRef != NULL)
        {
            return(VoipTranscribeStatus(pTranscribeRef, iSelect, iValue, pBuf, iBufSize));
        }
        else
        {
            // pTranscribeRef == NULL
            return(-1);
        }
    }
    if ((iSelect == 'ttos') && (iValue < VOIP_MAXLOCALUSERS))
    {
        return(pHeadset->aLocalUsers[iValue].iNarrating != 0);
    }
    if (iSelect == 'ttsm')
    {
        VoipNarrateRefT *pNarrateRef = pHeadset->aLocalUsers[iValue].pNarrateRef;
        if (pNarrateRef != NULL)
        {
            return(VoipNarrateStatus(pNarrateRef, iSelect, iValue, pBuf, iBufSize));
        }
        else
        {
            // pNarrateRef == NULL
            return(-1);
        }
    }
    if (iSelect == 'xply')
    {
        return(pHeadset->bCrossPlay);
    }

    // unhandled result, fall through to active codec
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

    \Version 16/01/2013 (mclouatre)
*/
/********************************************************************************F*/
void VoipHeadsetSpkrCallback(VoipHeadsetRefT *pHeadset, VoipSpkrCallbackT *pCallback, void *pUserData)
{
    pHeadset->pSpkrDataCb = pCallback;
    pHeadset->pSpkrCbUserData = pUserData;
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

/*F********************************************************************************/
/*!
    \Function VoipHeadsetConfigNarration

    \Description
        Configure the narrate module

    \Input *pHeadset    - headset module state
    \Input uProvider    - narrate provider
    \Input *pUrl        - narrate url
    \Input *pKey        - narrate key

    \Version 11/08/2018 (tcho)
*/
/********************************************************************************F*/
void VoipHeadsetConfigNarration(VoipHeadsetRefT *pHeadset, uint32_t uProvider, const char *pUrl, const char *pKey)
{
    VoipNarrateConfig(uProvider, pUrl, pKey);
}

/*F********************************************************************************/
/*!
    \Function VoipHeadsetTranscribedTextPermissionCheck

    \Description
        Make sure inbound transcribed text can be displayed to local users.

    \Input *pHeadset    - headset module state
    \Input *pOriginator - originator of the message
    \Input *pStrUtf8    - message to be displayed

    \Version 04/04/2019 (mclouatre)
*/
/********************************************************************************F*/
void VoipHeadsetTranscribedTextPermissionCheck(VoipHeadsetRefT *pHeadset, VoipUserT *pOriginator, const char *pStrUtf8)
{
    int32_t iRemoteUserSpaceIndex;

    for (iRemoteUserSpaceIndex = 0; iRemoteUserSpaceIndex < pHeadset->iMaxRemoteUsers; iRemoteUserSpaceIndex++)
    {
        if (VOIP_SameUser(&pHeadset->aRemoteUsers[iRemoteUserSpaceIndex].user, pOriginator))
        {
            // are there communication restrictions?
            if (pHeadset->bSharedPrivilege)
            {
                pHeadset->pTranscribedTextReceivedCb(pOriginator, pStrUtf8, pHeadset->pTranscribedTextReceivedCbUserData);
            }
            else
            {
                NetPrintf(("voipheadsetps4: transcribed text from %lld dropped because chat is restricted -- text was: %s\n",
                    pOriginator->AccountInfo.iPersonaId, pStrUtf8));
            }
        }
    }
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

    \Version 04/04/2019 (mclouatre)
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
        Callback to tell dirtysdk what the first party id is for this user

    \Input *pHeadset    - headset module state
    \Input *pCallback   - what to call when transcribed text is received from remote player
    \Input *pUserData   - user data for callback

    \Version 04/28/2020 (eesponda)
*/
/********************************************************************************F*/
void VoipHeadsetSetFirstPartyIdCallback(VoipHeadsetRefT *pHeadset, VoipFirstPartyIdCallbackCbT *pCallback, void *pUserData)
{
    pHeadset->pFirstPartyIdCb = pCallback;
    pHeadset->pFirstPartyIdCbUserData = pUserData;
}
