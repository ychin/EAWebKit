/*H********************************************************************************/
/*!
    \File voipnarratexboxone.cpp

    \Description
        VoipNarration API wrapping SAPI text to speech APIs via SpeechSynthesizer
        on XboxOne.

    \Copyright
        Copyright 2019 Electronic Arts

    \Version 02/04/2019 (eesponda)
*/
/********************************************************************************H*/

/*** Include Files ****************************************************************/

#include <stdlib.h>
#include <wrl.h>
#include <robuffer.h>
#include <mmreg.h>

#include "DirtySDK/dirtysock/dirtylib.h"
#include "DirtySDK/dirtysock/dirtymem.h"
#include "DirtySDK/dirtysock/netconn.h"
#include "DirtySDK/voip/voipdef.h"
#include "DirtySDK/voip/voipnarrate.h"

/*** Defines **********************************************************************/

#define VOIPNARRATE_SAMPLEWIDTH (2)      //!< sample size; 16-bit samples

/*** Type Definitions *************************************************************/

// using declarations
using Windows::Media::SpeechSynthesis::SpeechSynthesizer;
using Windows::Media::SpeechSynthesis::SpeechSynthesisStream;
using Windows::Storage::Streams::DataReader;
using Windows::Storage::Streams::DataReaderLoadOperation;

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

struct VoipNarrateRequestT
{
    wchar_t wstrText[VOIPNARRATE_INPUT_MAX]; //!< text to synthesize
    VoipNarrateGenderE eGender;         //!< voice we will use for this request
    int32_t iUserIndex;                 //!< user index of requestor
    SpeechSynthesisStream ^refResult;   //!< result of the synthesis
    DataReader ^refReader;              //!< reader we use to pull bytes out of the stream
    DataReaderLoadOperation ^refAsyncOp;//!< async operation for loading data from the reader
    VoipNarrateRequestT *pNext;         //!< next item in the list
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
    SpeechSynthesizer ^refSynth;            //!< used to synthesize audio
    VoipTextToSpeechMetricsT Metrics;       //!< narration metrics
    enum
    {
        VOIPNARRATE_VOICE_READY,            //!< ready to process a new request
        VOIPNARRATE_VOICE_BUSY,             //!< currently processing a request
        VOIPNARRATE_VOICE_TRANSITION        //!< transition to next request
    } eVoiceState;                          //!< current state of the module
    uint32_t uStartTime;                    //!< when the request started
    uint8_t *pData;                         //!< voice data for processing
    int32_t iDataLen;                       //!< length of the data
    int32_t iDataMax;                       //!< maximum amount of data we will pull at once
    int32_t iDataOff;
    VoipNarrateGenderE eGender;             //!< current gender
    NetCritT DataCrit;                      //!< crit to share the data between the lambda
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
    \Function _VoipNarrateStart

    \Description
        Request narration for the request at the front of the queue

    \Input *pVoipNarrate    - module state
    \Input *pRequest        - current request data

    \Version 02/04/2019 (eesponda)
*/
/********************************************************************************F*/
static void _VoipNarrateStart(VoipNarrateRefT *pVoipNarrate, VoipNarrateRequestT *pRequest)
{
    using namespace Windows::Foundation;
    using namespace Windows::Media::SpeechSynthesis;
    using namespace Windows::Storage::Streams;

    // update metrics
    pVoipNarrate->Metrics.uEventCount += 1;
    pVoipNarrate->Metrics.uCharCountSent += (unsigned)wcslen(pRequest->wstrText);

    // announce that the streaming will begin for this request
    pVoipNarrate->pVoiceDataCb(pVoipNarrate, pRequest->iUserIndex, NULL, VOIPNARRATE_STREAM_START, pVoipNarrate->pUserData);

    // convert the text to speech
    pVoipNarrate->uStartTime = NetTick();

    try
    {
        IAsyncOperation<SpeechSynthesisStream ^> ^refAsyncOp = pVoipNarrate->refSynth->SynthesizeTextToStreamAsync(ref new Platform::String(pRequest->wstrText));
        refAsyncOp->Completed = ref new AsyncOperationCompletedHandler<SpeechSynthesisStream ^>(
            [pVoipNarrate, pRequest](IAsyncOperation<SpeechSynthesisStream ^> ^refAsyncOp, Windows::Foundation::AsyncStatus eStatus)
        {
            switch (eStatus)
            {
                case Windows::Foundation::AsyncStatus::Completed:
                {
                    try
                    {
                        pRequest->refResult = refAsyncOp->GetResults();
                        pRequest->refReader = ref new DataReader(pRequest->refResult);
                        pRequest->refReader->UnicodeEncoding = UnicodeEncoding::Utf8;
                        pRequest->refReader->ByteOrder = ByteOrder::LittleEndian;

                        // update metrics
                        pVoipNarrate->Metrics.uDelay += NetTickDiff(NetTick(), pVoipNarrate->uStartTime);

                        NetPrintfVerbose((pVoipNarrate->iVerbose, 1, "voipnarratexboxone: SynthesizeTextToStreamAsync AsyncStatus::Completed (ContentType=%S, Size=%u)\n",
                            pRequest->refResult->ContentType->Data(), pRequest->refResult->Size));
                    }
                    catch (Platform::Exception ^refException)
                    {
                        NetPrintf(("voipnarratexboxone: exception thrown when getting results for SynthezieTextToStreamAsync %S/0x%08x\n",
                            refException->Message->Data(), refException->HResult));
                        pVoipNarrate->Metrics.uErrorCount += 1;
                        pVoipNarrate->eVoiceState = VoipNarrateRefT::VOIPNARRATE_VOICE_TRANSITION;
                    }
                    break;
                }
                case Windows::Foundation::AsyncStatus::Error:
                {
                    NetPrintf(("voipnarratexboxone: SynthesizeTextToStreamAsync AsyncStatus::Error\n"));
                    pVoipNarrate->Metrics.uErrorCount += 1;
                    pVoipNarrate->eVoiceState = VoipNarrateRefT::VOIPNARRATE_VOICE_TRANSITION;
                    break;
                }
                case Windows::Foundation::AsyncStatus::Canceled:
                {
                    NetPrintfVerbose((pVoipNarrate->iVerbose, 1, "voipnarratexboxone: SynthesizeTextToStreamAsync AsyncStatus::Canceled\n"));
                    break;
                }
                case Windows::Foundation::AsyncStatus::Started:
                {
                    NetPrintfVerbose((pVoipNarrate->iVerbose, 1, "voipnarratexboxone: SynthesizeTextToStreamAsync AsyncStatus::Started\n"));
                    break;
                }
            }

            // close the async operation if completed
            if (eStatus != Windows::Foundation::AsyncStatus::Started)
            {
                refAsyncOp->Close();
            }
        });

        pVoipNarrate->eVoiceState = VoipNarrateRefT::VOIPNARRATE_VOICE_BUSY;
    }
    catch (Platform::Exception ^refException)
    {
        NetPrintf(("voipnarratexboxone: SynthesizeTextToStreamAsync threw an exception, msg=%S\n", refException->Message->Data()));
        pVoipNarrate->Metrics.uErrorCount += 1;
        pVoipNarrate->eVoiceState = VoipNarrateRefT::VOIPNARRATE_VOICE_TRANSITION;
    }
}

/*F********************************************************************************/
/*!
    \Function _VoipNarrateGenderChange

    \Description
        Change the gender (voice) for the synth module

    \Input *pVoipNarrate    - module state
    \Input *pRequest        - current request data

    \Notes
        This operation can be expensive, we only want to do it when necessary

    \Version 02/04/2019 (eesponda)
*/
/********************************************************************************F*/
static void _VoipNarrateGenderChange(VoipNarrateRefT *pVoipNarrate, VoipNarrateRequestT *pRequest)
{
    using namespace Windows::Media::SpeechSynthesis;
    using namespace Windows::Foundation::Collections;

    VoiceGender Gender = (pRequest->eGender == VOIPNARRATE_GENDER_FEMALE) ? VoiceGender::Female : VoiceGender::Male;
    IIterator<VoiceInformation ^> ^refIterator;
    for (refIterator = pVoipNarrate->refSynth->AllVoices->First(); refIterator->HasCurrent; refIterator->MoveNext())
    {
        VoiceInformation ^refCurrent = refIterator->Current;
        if ((refCurrent->Gender == Gender) && (refCurrent->Language->Equals(ref new Platform::String(L"en-US"))))
        {
            NetPrintfVerbose((pVoipNarrate->iVerbose, 1, "voipnarratexboxone: selected voice=%S, Description: %S, Gender: %S, Language: %S\n",
                refCurrent->DisplayName->Data(), refCurrent->Description->Data(),
                refCurrent->Gender.ToString()->Data(), refCurrent->Language->ToString()->Data()));
            pVoipNarrate->refSynth->Voice = refCurrent;
            pVoipNarrate->eGender = pRequest->eGender;
            break;
        }
    }
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

    \Version 02/04/2019 (eesponda)
*/
/********************************************************************************F*/
static void _VoipNarrateProcessResult(VoipNarrateRefT *pVoipNarrate, VoipNarrateRequestT *pRequest)
{
    using namespace Windows::Foundation;
    using namespace Windows::Storage::Streams;
    using namespace Microsoft::WRL;

    // check the position before we call load
    const bool bStreamStart = (pRequest->refResult->Position == 0);
    const bool bStreamEnd = (pRequest->refResult->Position == pRequest->refResult->Size);

    // if there is a current async op in progress, then lets wait until the next frame
    if (pRequest->refAsyncOp != nullptr)
    {
        return;
    }

    // if there is data to process do it now
    NetCritEnter(&pVoipNarrate->DataCrit);
    if (pVoipNarrate->iDataLen > 0)
    {
        //int32_t iOffset = 0;

        // update metrics
        pVoipNarrate->Metrics.uDurationMsRecv += ((pVoipNarrate->iDataLen * 1000) / VOIPNARRATE_SAMPLERATE);

        // send the data back
        while (pVoipNarrate->iDataOff < pVoipNarrate->iDataLen)
        {
            const int32_t iLength = DS_MIN(320*2, pVoipNarrate->iDataLen - pVoipNarrate->iDataOff); // 320 samples @ 16kHz for 20ms of audio
            int32_t iRead;

            // invoke the callback with the voice data
            if ((iRead = pVoipNarrate->pVoiceDataCb(pVoipNarrate, pRequest->iUserIndex, (const int16_t *)(pVoipNarrate->pData+pVoipNarrate->iDataOff), iLength, pVoipNarrate->pUserData)) > 0)
            {
                pVoipNarrate->iDataOff += iRead;
            }
            else
            {
                break;
            }
        }
        // complete buffer has been processed
        if (pVoipNarrate->iDataOff >= pVoipNarrate->iDataLen)
        {
            pVoipNarrate->iDataLen = 0;
            pVoipNarrate->iDataOff = 0;
        }
    }
    NetCritLeave(&pVoipNarrate->DataCrit);

    // if the stream has ended then transition
    if ((pVoipNarrate->iDataLen == 0) && bStreamEnd)
    {
        pVoipNarrate->eVoiceState = VoipNarrateRefT::VOIPNARRATE_VOICE_TRANSITION;
        return;
    }
    if (pVoipNarrate->iDataLen > 0)
    {
        return;
    }

    try
    {
        // load up to 1s of audio at a time, first receive pull wave header information
        pRequest->refAsyncOp = pRequest->refReader->LoadAsync(!bStreamStart ? (pVoipNarrate->iDataMax) : (pVoipNarrate->iDataMax + sizeof(WaveHeaderT)));
        pRequest->refAsyncOp->Completed = ref new AsyncOperationCompletedHandler<uint32_t>(
            [pVoipNarrate, pRequest, bStreamStart](IAsyncOperation<uint32_t> ^refAsyncOp, Windows::Foundation::AsyncStatus eStatus)
        {
            bool bError = false;

            switch (eStatus)
            {
                case Windows::Foundation::AsyncStatus::Completed:
                {
                    ComPtr<IBufferByteAccess> pBufferByteAccess;
                    uint8_t *pAudioData;
                    uint32_t uAvailableBytes, uOffset = 0;

                    try
                    {
                        uAvailableBytes = DS_MIN((unsigned)pVoipNarrate->iDataMax-pVoipNarrate->iDataLen, refAsyncOp->GetResults());
                        NetPrintfVerbose((pVoipNarrate->iVerbose, 1, "voipnarratexboxone: LoadAsync AsyncStatus::Completed (Length=%u)\n",
                            uAvailableBytes));
                    }
                    catch (Platform::Exception ^refException)
                    {
                        NetPrintf(("voipnarratexboxone: exception thrown when getting results from LoadAsync %S/0x%08x\n",
                            refException->Message->Data(), refException->HResult));
                        bError = true;
                        break;
                    }

                    // pull out the raw pointer
                    ((IInspectable *)pRequest->refReader->ReadBuffer(uAvailableBytes))->QueryInterface(IID_PPV_ARGS(&pBufferByteAccess));
                    if (FAILED(pBufferByteAccess->Buffer(&pAudioData)))
                    {
                        NetPrintf(("voipnarratexboxone: could not convert to IBufferByteAccess to get raw data\n"));
                        bError = true;
                        break;
                    }

                    // validate the wave header in the first receive
                    if (bStreamStart)
                    {
                        // validate that the format is what we expect, FormatTag is the beginning of the format data
                        if ((uOffset = _VoipNarrateSkipWavHeader(pAudioData, uAvailableBytes)) == 0)
                        {
                            bError = true;
                            break;
                        }
                    }

                    // copy into our buffer to be processed outside of this lambda
                    NetCritEnter(&pVoipNarrate->DataCrit);
                    ds_memcpy_s(pVoipNarrate->pData+pVoipNarrate->iDataLen, pVoipNarrate->iDataMax-pVoipNarrate->iDataLen, pAudioData+uOffset, uAvailableBytes-uOffset);
                    pVoipNarrate->iDataLen += DS_MIN(pVoipNarrate->iDataMax, (signed)(uAvailableBytes-uOffset));
                    NetCritLeave(&pVoipNarrate->DataCrit);

                    break;
                }
                case Windows::Foundation::AsyncStatus::Error:
                {
                    NetPrintf(("voipnarratexboxone: LoadAsync AsyncStatus::Error\n"));
                    bError = true;
                    break;
                }
                case Windows::Foundation::AsyncStatus::Canceled:
                {
                    NetPrintfVerbose((pVoipNarrate->iVerbose, 1, "voipnarratexboxone: LoadAsync AsyncStatus::Canceled\n"));
                    break;
                }
                case Windows::Foundation::AsyncStatus::Started:
                {
                    NetPrintfVerbose((pVoipNarrate->iVerbose, 1, "voipnarratexboxone: LoadAsync AsyncStatus::Started\n"));
                    break;
                }
            }

            // close the async operation if completed
            if (eStatus != Windows::Foundation::AsyncStatus::Started)
            {
                pRequest->refAsyncOp->Close();
                delete pRequest->refAsyncOp;
                pRequest->refAsyncOp = nullptr;
            }

            // if an error occured increment the count and transition
            if (bError)
            {
                pVoipNarrate->Metrics.uErrorCount += 1;
                pVoipNarrate->eVoiceState = VoipNarrateRefT::VOIPNARRATE_VOICE_TRANSITION;
            }
        });
    }
    catch (Platform::Exception ^refException)
    {
        NetPrintf(("voipnarratexboxone: LoadAsync threw an exception, msg=%S\n", refException->Message->Data()));
        pVoipNarrate->Metrics.uErrorCount += 1;
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
    pRequest->refReader = nullptr;
    pRequest->refResult = nullptr;
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
        int32_t             - zero=destroy compelte, negative=try again

    \Version 02/04/2019 (eesponda)
*/
/********************************************************************************F*/
static int32_t _VoipNarrateDestoryInternal(void *pDestroyData)
{
    VoipNarrateRefT *pVoipNarrate = (VoipNarrateRefT *)pDestroyData;

    // update the module to allow it to do work
    VoipNarrateUpdate(pVoipNarrate);

    // if we are in the ready state then we are safe to cleanup
    if (pVoipNarrate->eVoiceState == VoipNarrateRefT::VOIPNARRATE_VOICE_READY)
    {
        // dispose of synth memory
        pVoipNarrate->refSynth = nullptr;

        // clean up the requests
        while (VoipNarrateRequestT *pRequest = pVoipNarrate->pHead)
        {
            _VoipNarrateRequestTransition(pVoipNarrate, pRequest, false);
        }
        pVoipNarrate->pHead = pVoipNarrate->pTail = NULL;

        // dispose of buffer
        if (pVoipNarrate->pData != NULL)
        {
            DirtyMemFree(pVoipNarrate->pData, VOIPNARRATE_MEMID, pVoipNarrate->iMemGroup, pVoipNarrate->pMemGroupUserData);
            pVoipNarrate->pData = NULL;
        }

        // dispose of crit
        if (pVoipNarrate->DataCrit.pData != NULL)
        {
            NetCritKill(&pVoipNarrate->DataCrit);
        }

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
    pVoipNarrate->refSynth = ref new SpeechSynthesizer();

    // allocate response buffer
    pVoipNarrate->iDataMax = VOIPNARRATE_SAMPLERATE * VOIPNARRATE_SAMPLEWIDTH;
    if ((pVoipNarrate->pData = (uint8_t *)DirtyMemAlloc(pVoipNarrate->iDataMax, VOIPNARRATE_MEMID, iMemGroup, pMemGroupUserData)) == NULL)
    {
        NetPrintf(("voipnarratexboxone: could not allocate response buffer\n"));
        VoipNarrateDestroy(pVoipNarrate);
        return(NULL);
    }

    // initialize our crit
    if (NetCritInit(&pVoipNarrate->DataCrit, "data crit") < 0)
    {
        NetPrintf(("voipnarratexboxone: could not initialize crit\n"));
        VoipNarrateDestroy(pVoipNarrate);
        return(NULL);
    }

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
    // transfer voipnarraatexboxone destruction ownership to NetConn
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
    mbstowcs(pRequest->wstrText, pText, sizeof(pRequest->wstrText) / sizeof(pRequest->wstrText[0]));
    pRequest->eGender = eGender;
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
        'ctsm'              - clear text to speech metrics in VoipTextToSpeechMetricsT
        'spam'              - set the module verbosity level per iValue
    \endverbatim

    \Version 02/04/2019 (eesponda)
*/
/********************************************************************************F*/
int32_t VoipNarrateControl(VoipNarrateRefT *pVoipNarrate, int32_t iControl, int32_t iValue, int32_t iValue2, void *pValue)
{
    if (iControl == 'ctsm')
    {
        ds_memclr(&(pVoipNarrate->Metrics), sizeof(pVoipNarrate->Metrics));
        return(0);
    }
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
        if (pRequest->eGender == pVoipNarrate->eGender)
        {
            _VoipNarrateStart(pVoipNarrate, pRequest);
        }
        else
        {
            _VoipNarrateGenderChange(pVoipNarrate, pRequest);
        }
    }
    // if available process the result
    else if ((pVoipNarrate->eVoiceState == VoipNarrateRefT::VOIPNARRATE_VOICE_BUSY) && (pRequest->refResult != nullptr))
    {
        _VoipNarrateProcessResult(pVoipNarrate, pRequest);
    }
    // if request processing complete or error occurred, transition to next request
    else if (pVoipNarrate->eVoiceState == VoipNarrateRefT::VOIPNARRATE_VOICE_TRANSITION)
    {
        _VoipNarrateRequestTransition(pVoipNarrate, pRequest, true);
    }
}
