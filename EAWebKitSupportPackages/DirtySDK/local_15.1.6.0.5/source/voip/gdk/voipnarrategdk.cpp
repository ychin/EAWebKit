/*H********************************************************************************/
/*!
    \File voipnarrategdk.cpp

    \Description
        VoipNarration API wrapping SAPI text to speech APIs via XSpeechSynthesizer
        on GDK.

    \Copyright
        Copyright 2020 Electronic Arts

    \Version 01/22/2020 (eesponda)
*/
/********************************************************************************H*/

/*** Include Files ****************************************************************/

#define WIN32_LEAN_AND_MEAN 1
#include <windows.h>
#include <stdlib.h>
#include <mmreg.h>

#include <XAsync.h>
#include <XAsyncProvider.h>
#include <XSpeechSynthesizer.h>

#include "DirtySDK/dirtysock/dirtyerr.h"
#include "DirtySDK/dirtysock/dirtylib.h"
#include "DirtySDK/dirtysock/dirtymem.h"
#include "DirtySDK/dirtysock/netconn.h"
#include "DirtySDK/voip/voipdef.h"
#include "DirtySDK/voip/voipnarrate.h"

/*** Defines **********************************************************************/

#define VOIPNARRATE_SAMPLEWIDTH (2)      //!< sample size; 16-bit samples

/*** Type Definitions *************************************************************/

//! this is the format of wave header used by this module, use bytes to avoid padding caused by CbSize
struct WaveHeaderT
{
    uint8_t Riff[4];            //!< 'RIFF'
    uint8_t FileSize[4];        //!< total size of the file
    uint8_t Wave[4];            //!< 'WAVE'
    uint8_t Fmt[4];             //!< 'fmt '
    uint8_t FmtLen[4];          //!< size of the wave format below
    uint8_t FormatTag[2];       //!< format tag (1=PCM)
    uint8_t Channels[2];        //!< channels (1=mono, 2=stereo)
    uint8_t SamplesPerSec[4];   //!< sample rate
    uint8_t AvgBytesPerSec[4];  //!< bytes rate
    uint8_t BlockAlign[2];      //!< bytes per sample
    uint8_t BitsPerSample[2];   //!< bits per sample
    uint8_t CbSize[2];          //!< cbsize of WAVEFORMATEX
    uint8_t Data[4];            //!< 'data'
    uint8_t DataSize[4];        //!< size of sample data
};

//! request data common between the module and async requests
struct VoipNarrateRequestDataT
{
    char strText[VOIPNARRATE_INPUT_MAX];//!< text to synthesize
    VoipNarrateGenderE eGender;         //!< voice we will use for this request
};

//! request data using for processing our incoming requests
struct VoipNarrateRequestT
{
    VoipNarrateRequestDataT RequestData;    //!< what we are requesting to be narrated
    int32_t iUserIndex;                     //!< user index of requestor
    uint8_t *pData;                         //!< voice data for processing
    size_t uResultSize;                     //!< amount of data to read from the stream
    size_t uResultPos;                      //!< position in the stream
    XAsyncBlock AsyncBlock;                 //!< the block to check the current status and get result of the operation
    VoipNarrateRequestT *pNext;             //!< next item in the list
};

//! request data only used in the async operation handling
struct VoipNarrateRequestAsyncT
{
    VoipNarrateRequestDataT RequestData;    //!< what we are requesting to be narrated
    XSpeechSynthesizerHandle pSynth;        //!< handle to synth module
    XSpeechSynthesizerStreamHandle pResult; //!< result of the synthesis
    int32_t iMemGroup;                      //!< memory group
    void *pMemGroupUserData;                //!< memory group user data
};

struct VoipNarrateRefT
{
    VoipNarrateVoiceDataCbT *pVoiceDataCb;  //!< callback to provide voice samples with
    void *pUserData;                        //!< user data to provide along with the callback
    int32_t iVerbose;                       //!< logging verbosity
    int32_t iMemGroup;                      //!< memory group
    void *pMemGroupUserData;                //!< memory group user data
    VoipNarrateRequestT *pHead;             //!< front of the queue
    VoipNarrateRequestT *pTail;             //!< back of the queue

    VoipTextToSpeechMetricsT Metrics;       //!< narration metrics
    enum
    {
        VOIPNARRATE_VOICE_READY,            //!< ready to process a new request
        VOIPNARRATE_VOICE_BUSY,             //!< currently processing a request
        VOIPNARRATE_VOICE_TRANSITION        //!< transition to next request
    } eVoiceState;                          //!< current state of the module
    uint32_t uStartTime;                    //!< when the request started
    int32_t iDataMax;                       //!< maximum amount of data we will pull at once
};

/*** Variables ********************************************************************/

//! supported format: mono channel, 16kbps
static const WAVEFORMATEX _VoipNarrateWaveFormat =
{
    WAVE_FORMAT_PCM, 1, VOIPNARRATE_SAMPLERATE, VOIPNARRATE_SAMPLERATE * VOIPNARRATE_SAMPLEWIDTH, VOIPNARRATE_SAMPLEWIDTH, VOIPNARRATE_SAMPLEWIDTH * 8, 0
};

/*** Private Functions ************************************************************/

/*F********************************************************************************/
/*!
    \Function _VoipNarrateInstalledVoiceCb

    \Description
        Callback fired to go through the installed voices

    \Input *pInformation    - the current voice
    \Input *pContext        - callback user data

    \Output
        bool                - true=continue, false=stop

    \Version 01/22/2020 (eesponda)
*/
/********************************************************************************F*/
static bool CALLBACK _VoipNarrateInstalledVoiceCb(const XSpeechSynthesizerVoiceInformation *pInformation, void *pContext)
{
    VoipNarrateRequestAsyncT *pRequest = (VoipNarrateRequestAsyncT *)pContext;
    XSpeechSynthesizerVoiceGender eGender = (pRequest->RequestData.eGender == VOIPNARRATE_GENDER_FEMALE) ? XSpeechSynthesizerVoiceGender::Female : XSpeechSynthesizerVoiceGender::Male;

    // check to make sure it is the correct voice we are looking for
    if ((pInformation->Gender == eGender) && (ds_stricmp(pInformation->Language, "en-US") == 0))
    {
        XSpeechSynthesizerSetCustomVoice(pRequest->pSynth, pInformation->VoiceId);
        return(false);
    }
    return(true);
}

/*F********************************************************************************/
/*!
    \Function _VoipNarrateAsync

    \Description
        Does the synchronous work asynchronously via this callback

    \Input eOp      - the state of the async operation
    \Input *pData   - data used to process the async operation

    \Output
        HRESULT     - result of this callback

    \Version 01/22/2020 (eesponda)
*/
/********************************************************************************F*/
static HRESULT CALLBACK _VoipNarrateAsync(XAsyncOp eOp, const XAsyncProviderData *pData)
{
    VoipNarrateRequestAsyncT *pRequest = (VoipNarrateRequestAsyncT *)pData->context;
    HRESULT hResult;
    size_t uResultSize;

    switch (eOp)
    {
        /* An async provider is invoked with this opcode during XAsyncBegin or XAsyncBeginAlloc.
           If the provider implements this op code, they should start their asynchronous task,
           either by calling XAsyncSchedule or through exterior means.  This callback is
           called synchronously in the XAsyncBegin call chain, so it should never block. */
        case XAsyncOp::Begin:
        {
            break;
        }
        /* An async provider is invoked with this opcode when XAsyncSchedule has been called to
           schedule work. Implementations should perform their asynchronous work and then call
           XAsyncComplete with the data payload size. If additional work needs to be done they
           can schedule it and return E_PENDING. */
        case XAsyncOp::DoWork:
        {
            // create the synthesizer
            if (FAILED(hResult = XSpeechSynthesizerCreate(&pRequest->pSynth)))
            {
                NetPrintf(("voipnarrategdk: failed to create synth module (err=%s)\n", DirtyErrGetName(hResult)));
                XAsyncComplete(pData->async, hResult, 0);
                return(hResult);
            }
            // change the voice if necessary
            if ((pRequest->RequestData.eGender != VOIPNARRATE_GENDER_MALE) && (FAILED(hResult = XSpeechSynthesizerEnumerateInstalledVoices(pRequest, _VoipNarrateInstalledVoiceCb))))
            {
                NetPrintf(("voipnarrategdk: failed to enumerate the voices (err=%s)\n", DirtyErrGetName(hResult)));
                XAsyncComplete(pData->async, hResult, 0);
                return(hResult);
            }
            // convert the text to speech and create a stream
            if (FAILED(hResult = XSpeechSynthesizerCreateStreamFromText(pRequest->pSynth, pRequest->RequestData.strText, &pRequest->pResult)))
            {
                NetPrintf(("voipnarrategdk: converting text to speech failed (err=%s)\n", DirtyErrGetName(hResult)));
                XAsyncComplete(pData->async, hResult, 0);
                return(hResult);
            }
            // get the size of the stream data to return to the caller
            if (SUCCEEDED(hResult = XSpeechSynthesizerGetStreamDataSize(pRequest->pResult, &uResultSize)))
            {
                XAsyncComplete(pData->async, hResult, uResultSize);
            }
            else
            {
                NetPrintf(("voipnarrategdk: failed to get the size of the stream (err=%s)\n", DirtyErrGetName(hResult)));
            }
            break;
        }
        /* An async provider is invoked with this opcode after an async call completes and the
           user needs to get the resulting data payload. The buffer and bufferSize have
           been arg checked. */
        case XAsyncOp::GetResult:
        {
            if (FAILED(hResult = XSpeechSynthesizerGetStreamData(pRequest->pResult, pData->bufferSize, pData->buffer, &uResultSize)))
            {
                NetPrintf(("voipnarrategdk: failed to stream data (err=%s)\n", DirtyErrGetName(hResult)));
                return(hResult);
            }
            break;
        }
        /* An async provider is invoked with this opcode when the async work should be canceled. If
           you can cancel your work you should call XAsyncComplete with an error code of E_ABORT when
           the work has been canceled. */
        case XAsyncOp::Cancel:
        {
            XAsyncComplete(pData->async, E_ABORT, 0);
            break;
        }
        /* An async provider is invoked with this opcode when the async call is over and
           data in the context can be cleaned up. */
        case XAsyncOp::Cleanup:
        {
            if (pRequest->pResult != NULL)
            {
                XSpeechSynthesizerCloseStreamHandle(pRequest->pResult);
            }
            if (pRequest->pSynth != NULL)
            {
                XSpeechSynthesizerCloseHandle(pRequest->pSynth);
            }
            DirtyMemFree(pRequest, VOIPNARRATE_MEMID, pRequest->iMemGroup, pRequest->pMemGroupUserData);
            break;
        }
    }

    return(S_OK);
}

/*F********************************************************************************/
/*!
    \Function _VoipNarrateStart

    \Description
        Request narration for the request at the front of the queue

    \Input *pVoipNarrate    - module state
    \Input *pRequest        - current request data

    \Version 01/22/2020 (eesponda)
*/
/********************************************************************************F*/
static void _VoipNarrateStart(VoipNarrateRefT *pVoipNarrate, VoipNarrateRequestT *pRequest)
{
    HRESULT hResult;
    VoipNarrateRequestAsyncT *pAsyncRequest;

    // update metrics
    pVoipNarrate->Metrics.uEventCount += 1;
    pVoipNarrate->Metrics.uCharCountSent += (unsigned)strlen(pRequest->RequestData.strText);

    // announce that the streaming will being for this request
    pVoipNarrate->pVoiceDataCb(pVoipNarrate, pRequest->iUserIndex, NULL, VOIPNARRATE_STREAM_START, pVoipNarrate->pUserData);

    // convert the text to speech
    pVoipNarrate->uStartTime = NetTick();

    // allocate memory for the async operation
    if ((pAsyncRequest = (VoipNarrateRequestAsyncT *)DirtyMemAlloc(sizeof(*pAsyncRequest), VOIPNARRATE_MEMID, pVoipNarrate->iMemGroup, pVoipNarrate->pMemGroupUserData)) == NULL)
    {
        NetPrintf(("voipnarrategdk: failed to allocate memory for the async request\n"));
        pVoipNarrate->Metrics.uErrorCount += 1;
        pVoipNarrate->eVoiceState = VoipNarrateRefT::VOIPNARRATE_VOICE_TRANSITION;
        return;
    }
    ds_memclr(pAsyncRequest, sizeof(*pAsyncRequest));
    ds_memcpy(&pAsyncRequest->RequestData, &pRequest->RequestData, sizeof(pRequest->RequestData));
    pAsyncRequest->iMemGroup = pVoipNarrate->iMemGroup;
    pAsyncRequest->pMemGroupUserData = pVoipNarrate->pMemGroupUserData;

    // schedule the work
    if (FAILED(hResult = XAsyncBegin(&pRequest->AsyncBlock, pAsyncRequest, _VoipNarrateAsync, __FUNCTION__, _VoipNarrateAsync)))
    {
        NetPrintf(("voipnarrategdk: failed to start the async narration operation (err=%s)\n", DirtyErrGetName(hResult)));
        pVoipNarrate->Metrics.uErrorCount += 1;
        pVoipNarrate->eVoiceState = VoipNarrateRefT::VOIPNARRATE_VOICE_TRANSITION;
        return;
    }
    if (FAILED(hResult = XAsyncSchedule(&pRequest->AsyncBlock, 0)))
    {
        NetPrintf(("voipnarrategdk: failed to schedule the async narration operation (err=%s)\n", DirtyErrGetName(hResult)));
        pVoipNarrate->Metrics.uErrorCount += 1;
        pVoipNarrate->eVoiceState = VoipNarrateRefT::VOIPNARRATE_VOICE_TRANSITION;
        return;
    }
    pVoipNarrate->eVoiceState = VoipNarrateRefT::VOIPNARRATE_VOICE_BUSY;
}

/*F********************************************************************************/
/*!
    \Function _VoipNarrateSkipValidateWavHeader

    \Description
        Return offset past WAV header in input data after validation is successful

    \Input *pData           - pointer to wav header
    \Input iDataLen         - length of data

    \Output
        int32_t             - offset past WAV header, or zero

    \Notes
        Given that we cannot specify our own waveformat we need to make sure what
        we get back is what we expect.

    \Version 11/06/2018 (jbrookes)
*/
/********************************************************************************F*/
static int32_t _VoipNarrateSkipWavHeader(const uint8_t *pData, int32_t iDataLen)
{
    int32_t iOffset = 0, iChkLen;
    uint8_t bFoundData;

    // validate and skip RIFF/WAVE header
    if ((iDataLen < 12) || ds_strnicmp((const char *)pData, "RIFF", 4) || ds_strnicmp((const char *)pData+8, "WAVE", 4))
    {
        return(0);
    }
    iOffset += 12;

    // process chunks
    for (bFoundData = FALSE; iOffset < (iDataLen+12); iOffset += iChkLen+8)
    {
        // get chunk length
        iChkLen  = pData[iOffset+4];
        iChkLen |= pData[iOffset+5]<<8;
        iChkLen |= pData[iOffset+6]<<16;
        iChkLen |= pData[iOffset+7]<<24;

        // look for fmt chunk for validation
        if (!ds_strnicmp((const char *)pData+iOffset, "fmt ", 4))
        {
            if (memcmp(pData+iOffset+8, &_VoipNarrateWaveFormat, sizeof(_VoipNarrateWaveFormat)) != 0)
            {
                NetPrintf(("voipnarratexboxone: received wave format does not match what we support in this module\n"));
                break;
            }
        }

        // look for data chunk
        if (!ds_strnicmp((const char *)pData+iOffset, "data", 4))
        {
            bFoundData = TRUE;
            iOffset += 8;
            break;
        }
    }
    return(bFoundData ? iOffset : 0);
}

/*F********************************************************************************/
/*!
    \Function _VoipNarrateProcessResult

    \Description
        After the request has succeeded start reading data from the stream

    \Input *pVoipNarrate    - module state
    \Input *pRequest        - current request data

    \Version 01/22/2020 (eesponda)
*/
/********************************************************************************F*/
static void _VoipNarrateProcessResult(VoipNarrateRefT *pVoipNarrate, VoipNarrateRequestT *pRequest)
{
    int32_t iDataOff = 0, iDataLen;
    HRESULT hResult;

    if ((hResult = XAsyncGetStatus(&pRequest->AsyncBlock, false)) == E_PENDING)
    {
        return;
    }

    // check if the operation failed to prevent having to process further
    if (FAILED(hResult))
    {
        pVoipNarrate->Metrics.uErrorCount += 1;
        pVoipNarrate->eVoiceState = VoipNarrateRefT::VOIPNARRATE_VOICE_TRANSITION;
        return;
    }

    // if we have retrieved the result of the operation then do it now
    if (pRequest->uResultSize == 0)
    {
        if (FAILED(hResult = XAsyncGetResultSize(&pRequest->AsyncBlock, &pRequest->uResultSize)))
        {
            NetPrintf(("voipnarrategdk: failed to size of the audio data (err=%s)\n", DirtyErrGetName(hResult)));
            pVoipNarrate->Metrics.uErrorCount += 1;
            pVoipNarrate->eVoiceState = VoipNarrateRefT::VOIPNARRATE_VOICE_TRANSITION;
            return;
        }
        if ((pRequest->pData = (uint8_t *)DirtyMemAlloc((signed)pRequest->uResultSize, VOIPNARRATE_MEMID, pVoipNarrate->iMemGroup, pVoipNarrate->pMemGroupUserData)) == NULL)
        {
            NetPrintf(("voipnarrategdk: failed to allocate the memory for the audio data\n"));
            pVoipNarrate->Metrics.uErrorCount += 1;
            pVoipNarrate->eVoiceState = VoipNarrateRefT::VOIPNARRATE_VOICE_TRANSITION;
            return;
        }
        if (FAILED(hResult = XAsyncGetResult(&pRequest->AsyncBlock, _VoipNarrateAsync, pRequest->uResultSize, pRequest->pData, &pRequest->uResultSize)))
        {
            NetPrintf(("voipnarrategdk: failed to get the audio data result (err=%s)\n", DirtyErrGetName(hResult)));
            pVoipNarrate->Metrics.uErrorCount += 1;
            pVoipNarrate->eVoiceState = VoipNarrateRefT::VOIPNARRATE_VOICE_TRANSITION;
            return;
        }

        // update metrics
        pVoipNarrate->Metrics.uDelay += NetTickDiff(NetTick(), pVoipNarrate->uStartTime);
        NetPrintfVerbose((pVoipNarrate->iVerbose, 1, "voipnarrategdk: XSpeechSynthesizerCreateStreamFromText Completed (size=%u)\n", pRequest->uResultSize));
    }

    // validate the wave header in the first receive
    if (pRequest->uResultPos == 0)
    {
        uint32_t uOffset;

        // validate that the format is what we expect, FormatTag is the beginning of the format data
        if ((uOffset = _VoipNarrateSkipWavHeader(pRequest->pData, (signed)pRequest->uResultSize)) == 0)
        {
            pVoipNarrate->Metrics.uErrorCount += 1;
            pVoipNarrate->eVoiceState = VoipNarrateRefT::VOIPNARRATE_VOICE_TRANSITION;
            return;
        }
        pRequest->uResultPos += uOffset;
    }

    // if there is data to process do it now
    if ((iDataLen = DS_MIN(pVoipNarrate->iDataMax, (signed)(pRequest->uResultSize - pRequest->uResultPos))) > 0)
    {
        // update metrics
        pVoipNarrate->Metrics.uDurationMsRecv += ((iDataLen * 1000) / VOIPNARRATE_SAMPLERATE);

        const uint8_t *pData = pRequest->pData + pRequest->uResultPos;

        // send the data back
        while (iDataOff < iDataLen)
        {
            // 320 samples @ 16kHz for 20ms of audio
            const int32_t iLength = DS_MIN(320*2, iDataLen - iDataOff);
            int32_t iRead;

            // invoke the callback with the voice data
            if ((iRead = pVoipNarrate->pVoiceDataCb(pVoipNarrate, pRequest->iUserIndex, (const int16_t *)(pData + iDataOff), iLength, pVoipNarrate->pUserData)) > 0)
            {
                iDataOff += iRead;
            }
            else
            {
                break;
            }
        }
        // increment the buffer by the amount of data we read
        pRequest->uResultPos += iDataOff;
    }

    // if the stream has ended then transition
    if (pRequest->uResultPos == pRequest->uResultSize)
    {
        pVoipNarrate->eVoiceState = VoipNarrateRefT::VOIPNARRATE_VOICE_TRANSITION;
    }
}

/*F********************************************************************************/
/*!
    \Function _VoipnarrateRequestTransition

    \Description
        Cleanup current request and transition to the next one

    \Input *pVoipNarrate    - module state
    \Input *pRequest        - current request data
    \Input bAnnounceEnd     - if we should announce the end of the stream

    \Version 02/04/2019 (eesponda)
*/
/********************************************************************************F*/
static void _VoipNarrateRequestTransition(VoipNarrateRefT *pVoipNarrate, VoipNarrateRequestT *pRequest, bool bAnnounceEnd)
{
    // announce the stream has ended
    if (bAnnounceEnd)
    {
        pVoipNarrate->pVoiceDataCb(pVoipNarrate, pRequest->iUserIndex, NULL, VOIPNARRATE_STREAM_END, pVoipNarrate->pUserData);
    }
    // if we are the only entry in the list just remove ourselves
    if (pVoipNarrate->pHead == pVoipNarrate->pTail)
    {
        pVoipNarrate->pHead = pVoipNarrate->pTail = NULL;
    }
    // otherwise pop request off the front
    else
    {
        pVoipNarrate->pHead = pRequest->pNext;
    }
    // clean up the old request
    DirtyMemFree(pRequest, VOIPNARRATE_MEMID, pVoipNarrate->iMemGroup, pVoipNarrate->pMemGroupUserData);

    // set ourselves to read again
    pVoipNarrate->eVoiceState = VoipNarrateRefT::VOIPNARRATE_VOICE_READY;
}

/*F********************************************************************************/
/*!
    \Function _VoipNarrateDestroyInternal

    \Description
        Called by netconn for external cleanup to allow us to wait for our requests
        to complete

    \Input *pDestroyData    - module state

    \Output
        int32_t             - zero=destroy complete, negative=try again

    \Version 02/04/2019 (eesponda)
*/
/********************************************************************************F*/
static int32_t _VoipNarrateDestoryInternal(void *pDestroyData)
{
    VoipNarrateRefT *pVoipNarrate = (VoipNarrateRefT *)pDestroyData;

    // if we are in the ready state we don't want to kick off any new requests by calling update
    if (pVoipNarrate->eVoiceState != VoipNarrateRefT::VOIPNARRATE_VOICE_READY)
    {
        // update the module to allow it to do work
        VoipNarrateUpdate(pVoipNarrate);
    }

    // if we are in the ready state then we are safe to cleanup
    if (pVoipNarrate->eVoiceState == VoipNarrateRefT::VOIPNARRATE_VOICE_READY)
    {
        // clean up the requests
        while (VoipNarrateRequestT *pRequest = pVoipNarrate->pHead)
        {
            _VoipNarrateRequestTransition(pVoipNarrate, pRequest, false);
        }
        pVoipNarrate->pHead = pVoipNarrate->pTail = NULL;

        // dispose of module memory
        DirtyMemFree(pVoipNarrate, VOIPNARRATE_MEMID, pVoipNarrate->iMemGroup, pVoipNarrate->pMemGroupUserData);
        return(0);
    }
    return(-1);
}

/*** Public Functions *************************************************************/

/*F********************************************************************************/
/*!
    \Function VoipNarrateCreate

    \Description
        Create the voipnarrate module

    \Input *pVoiceDataCb    - callback fired with voice samples
    \Input *pUserData       - callback data passed along in every callback

    \Output
        VoipNarrateRefT *   - valid module ref or NULL on error

    \Version 02/04/2019 (eesponda)
*/
/********************************************************************************F*/
VoipNarrateRefT *VoipNarrateCreate(VoipNarrateVoiceDataCbT *pVoiceDataCb, void *pUserData)
{
    VoipNarrateRefT *pVoipNarrate;
    int32_t iMemGroup;
    void *pMemGroupUserData;

    // query memgroup info
    DirtyMemGroupQuery(&iMemGroup, &pMemGroupUserData);

    // validate callback
    if (pVoiceDataCb == NULL)
    {
        NetPrintf(("voipnarratexboxone: could not create module with null callback\n"));
        return(NULL);
    }

    // allocate module ref
    if ((pVoipNarrate = (VoipNarrateRefT *)DirtyMemAlloc(sizeof(*pVoipNarrate), VOIPNARRATE_MEMID, iMemGroup, pMemGroupUserData)) == NULL)
    {
        NetPrintf(("voipnarratexboxone: could not allocate module ref\n"));
        return(NULL);
    }
    ds_memclr(pVoipNarrate, sizeof(*pVoipNarrate));
    pVoipNarrate->pVoiceDataCb = pVoiceDataCb;
    pVoipNarrate->pUserData = pUserData;
    pVoipNarrate->iMemGroup = iMemGroup;
    pVoipNarrate->pMemGroupUserData = pMemGroupUserData;
    pVoipNarrate->iVerbose = 1;

    // allocate response buffer
    pVoipNarrate->iDataMax = VOIPNARRATE_SAMPLERATE * VOIPNARRATE_SAMPLEWIDTH;

    return(pVoipNarrate);
}

/*F********************************************************************************/
/*!
    \Function VoipNarrateConfig

    \Description
        Configure the voipnarrate module

    \Input eProvider    - narration provider
    \Input *pUrl        - provider url
    \Input *pKey        - provider key

    \Notes
        Unimplemented in this version of voipnarrate

    \Version 02/04/2019 (eesponda)
*/
/********************************************************************************F*/
void VoipNarrateConfig(VoipNarrateProviderE eProvider, const char *pUrl, const char *pKey)
{
}

/*F********************************************************************************/
/*!
    \Function VoipNarrateDestroy

    \Description
        Destroy the narrate module

    \Input *pVoipNarrate    - module state

    \Version 02/04/2019 (eesponda)
*/
/********************************************************************************F*/
void VoipNarrateDestroy(VoipNarrateRefT *pVoipNarrate)
{
    // cancel the requests
    for (VoipNarrateRequestT *pRequest = pVoipNarrate->pHead; pRequest != NULL; pRequest = pRequest->pNext)
    {
        XAsyncCancel(&pRequest->AsyncBlock);
    }

    // transfer voipnarrategdk destruction ownership to NetConn
    NetConnControl('recu', 0, 0, (void *)_VoipNarrateDestoryInternal, pVoipNarrate);
}

/*F********************************************************************************/
/*!
    \Function VoipNarrateInput

    \Description
        Add a new request to the queue

    \Input *pVoipNarrate    - module state
    \Input iUserIndex       - index of requestor
    \Input eGender          - gender of narrated voice
    \Input *pText           - text to narrate

    \Output
        int32_t             - zero=success, negative=failure

    \Version 02/04/2019 (eesponda)
*/
/********************************************************************************F*/
int32_t VoipNarrateInput(VoipNarrateRefT *pVoipNarrate, int32_t iUserIndex, VoipNarrateGenderE eGender, const char *pText)
{
    VoipNarrateRequestT *pRequest;

    // allocate the new request
    if ((pRequest = (VoipNarrateRequestT *)DirtyMemAlloc(sizeof(*pRequest), VOIPNARRATE_MEMID, pVoipNarrate->iMemGroup, pVoipNarrate->pMemGroupUserData)) == NULL)
    {
        NetPrintf(("voipnarratexboxone: could not allocate new request\n"));
        pVoipNarrate->Metrics.uErrorCount += 1;
        return(-1);
    }
    ds_memclr(pRequest, sizeof(*pRequest));

    // copy the request data
    ds_strnzcpy(pRequest->RequestData.strText, pText, sizeof(pRequest->RequestData.strText));
    pRequest->RequestData.eGender = eGender;
    pRequest->iUserIndex = iUserIndex;

    // push request to back
    if (pVoipNarrate->pHead != NULL)
    {
        pVoipNarrate->pTail->pNext = pRequest;
    }
    else
    {
        pVoipNarrate->pHead = pRequest;
    }
    pVoipNarrate->pTail = pRequest;

    // return success
    return(0);
}

/*F********************************************************************************/
/*!
    \Function VoipNarrateStatus

    \Description
        Get module status.

    \Input *pVoipNarrate    - module state
    \Input iStatus          - status selector
    \Input iValue           - selector specific
    \Input *pBuffer         - selector specific
    \Input iBufSize         - selector specific

    \Output
        int32_t             - selector specific

    \verbatim
        'ttsm'              - get the TTS metrics via pBuffer
        'wave'              - get the wave format the audio is in
    \endverbatim

    \Version 02/04/2019 (eesponda)
*/
/********************************************************************************F*/
int32_t VoipNarrateStatus(VoipNarrateRefT *pVoipNarrate, int32_t iStatus, int32_t iValue, void *pBuffer, int32_t iBufSize)
{
    // get metrics
    if (iStatus == 'ttsm')
    {
        if ((pBuffer != NULL) && (iBufSize >= (signed)sizeof(pVoipNarrate->Metrics)))
        {
            ds_memcpy(pBuffer, &pVoipNarrate->Metrics, sizeof(pVoipNarrate->Metrics));
            return(0);
        }
        return(-1);
    }
    if ((iStatus == 'wave') && (pBuffer != NULL) && (iBufSize >= (signed)sizeof(_VoipNarrateWaveFormat)))
    {
        ds_memcpy(pBuffer, &_VoipNarrateWaveFormat, iBufSize);
        return(0);
    }
    // unhandled
    return(-1);
}

/*F********************************************************************************/
/*!
    \Function VoipNarrateControl

    \Description
        Set control options

    \Input *pVoipNarrate    - module state
    \Input iControl         - control selector
    \Input iValue           - control specific
    \Input iValue2          - control specific
    \Input *pValue          - control specific

    \Output
        int32_t             - control specific

    \verbatim
        'spam'              - set the module verbosity level per iValue
    \endverbatim

    \Version 02/04/2019 (eesponda)
*/
/********************************************************************************F*/
int32_t VoipNarrateControl(VoipNarrateRefT *pVoipNarrate, int32_t iControl, int32_t iValue, int32_t iValue2, void *pValue)
{
    // update the logging verbosity
    if (iControl == 'spam')
    {
        pVoipNarrate->iVerbose = iValue;
        return(0);
    }
    // unhandled
    return(-1);
}

/*F********************************************************************************/
/*!
    \Function VoipNarrateUpdate

    \Description
        Update the module to give it time to process

    \Input *pVoipNarrate    - module state

    \Version 02/04/2019 (eesponda)
*/
/********************************************************************************F*/
void VoipNarrateUpdate(VoipNarrateRefT *pVoipNarrate)
{
    VoipNarrateRequestT *pRequest = pVoipNarrate->pHead;

    // see if we need to start a queued narration request
    if ((pRequest != NULL) && (pVoipNarrate->eVoiceState == VoipNarrateRefT::VOIPNARRATE_VOICE_READY))
    {
        _VoipNarrateStart(pVoipNarrate, pRequest);
    }
    // if available process the result
    else if (pVoipNarrate->eVoiceState == VoipNarrateRefT::VOIPNARRATE_VOICE_BUSY)
    {
        _VoipNarrateProcessResult(pVoipNarrate, pRequest);
    }
    // if request processing complete or error occurred, transition to next request
    else if (pVoipNarrate->eVoiceState == VoipNarrateRefT::VOIPNARRATE_VOICE_TRANSITION)
    {
        _VoipNarrateRequestTransition(pVoipNarrate, pRequest, true);
    }
}
