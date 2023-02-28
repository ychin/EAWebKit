/*H********************************************************************************/
/*!
    \File voipxboxone.cpp

    \Description
        Voip library interface.

    \Copyright
        Copyright (c) Electronic Arts 2013. ALL RIGHTS RESERVED.

    \Version 05/24/2013 (mclouatre) First Version
*/
/********************************************************************************H*/

/*** Include files ****************************************************************/

// dirtysock includes
#include "DirtySDK/platform.h"
#include "DirtySDK/dirtysock.h"
#include "DirtySDK/dirtysock/netconn.h"
#include "DirtySDK/dirtysock/dirtyerr.h"
#include "DirtySDK/dirtysock/dirtythread.h"

// voip includes
#include "DirtySDK/voip/voipdef.h"
#include "voippriv.h"
#include "voipcommon.h"

// for PartyChat
#ifndef DIRTYCODE_GDK
using namespace Windows::Xbox::Multiplayer;
#endif

/*** Defines **********************************************************************/

/* https://docs.microsoft.com/en-us/windows/uwp/xbox-live/multiplayer/chat/using-game-chat-2
  The more frequently the data frames are processed, the lower the audio latency apparent to the 
  end user will be. The audio is coalesced into 40 ms data frames; this is the suggested polling period, however
  other platforms operate at 20ms so xone is following that convention here*/
#define VOIP_THREAD_SLEEP_DURATION        (20)

/*** Macros ***********************************************************************/

/*** Type Definitions *************************************************************/

//! VoIP module state
struct VoipRefT
{
    VoipCommonRefT              Common;                 //!< cross-platform voip data (must come first!)

    volatile int32_t            iThreadActive;          //!< control variable used during thread creation and exit
    volatile HANDLE             hThread;                //!< thread handle used to signal shutdown

    uint8_t                     bOnewayBadRepMute;      //!< TRUE if we want one way bad rep auto mute
    uint8_t                     _pad[3];
};

/*** Function Prototypes **********************************************************/

/*** Variables ********************************************************************/

// Private Variables
static int32_t _Voip_iThreadAffinity = -1;
static int32_t _Voip_iThreadPriority = -1;
// Public Variables


/*** Private Functions ************************************************************/


/*F********************************************************************************/
/*!
    \Function _VoipStatusCb

    \Description
        Callback to handle change of headset status.

    \Input iLocalUserIndex - headset that has changed
    \Input bStatus         - if TRUE the headset was inserted, else if FALSE it was removed
    \Input eUpdate         - functionality of the headset (input, output or both)
    \Input *pUserData      - pointer to callback user data

    \Notes
        It is assumed that eUpdate will always be VOIP_HEADSET_STATUS_INPUT for
        this platform and that speakers will always be available as output.

    \Version 05/24/2013 (mclouatre)
*/
/********************************************************************************F*/
static void _VoipStatusCb(int32_t iLocalUserIndex, uint32_t bStatus, VoipHeadsetStatusUpdateE eUpdate, void *pUserData)
{
    VoipRefT *pRef = (VoipRefT *)pUserData;

    if (eUpdate != VOIP_HEADSET_STATUS_INPUT)
    {
        NetPrintf(("voipxboxone: warning, unexpected headset event for this platform!\n"));
    }

    if (bStatus)
    {
        NetPrintf(("voipxboxone: [%d] headset active (audio in: on, audio out: on)\n", iLocalUserIndex));
        pRef->Common.uPortHeadsetStatus |= 1 << iLocalUserIndex;
        pRef->Common.Connectionlist.uLocalUserStatus[iLocalUserIndex] |= (VOIP_LOCAL_USER_HEADSETOK|VOIP_LOCAL_USER_INPUTDEVICEOK);
    }
    else
    {
        NetPrintf(("voipxboxone: [%d] headset inactive (audio in: off, audio out: on)\n", iLocalUserIndex));
        pRef->Common.uPortHeadsetStatus &= ~(1 << iLocalUserIndex);
        pRef->Common.Connectionlist.uLocalUserStatus[iLocalUserIndex] &= ~(VOIP_LOCAL_USER_HEADSETOK|VOIP_LOCAL_USER_INPUTDEVICEOK);
    }
}

/*F********************************************************************************/
/*!
    \Function   _VoipRegisterLocalTalker

    \Description
        Register the local talker on the given port.

    \Input *pVoip           - voip module reference
    \Input iLocalUserIndex  - local user index associated with the user
    \Input bRegister        - TRUE to register user, FALSE to unregister
    \Input *pVoipUser       - VoipUserT to register

    \Output
        int32_t             - 1 if the local talker was successfully registered, else 0

    \Version 04/25/2014 (tcho)
*/
/********************************************************************************F*/
static int32_t _VoipRegisterLocalTalker(VoipRefT *pVoip, int32_t iLocalUserIndex, uint32_t bRegister, VoipUserT *pVoipUser)
{
    int32_t iResult;
    if ((iResult = VoipHeadsetControl(pVoip->Common.pHeadset, 'rloc', iLocalUserIndex, bRegister, pVoipUser)) == 0)
    {
        if (bRegister)
        {
            // add user to local list
            if (pVoipUser != NULL)
            {
                ds_memcpy_s(&pVoip->Common.Connectionlist.LocalUsers[iLocalUserIndex], sizeof(pVoip->Common.Connectionlist.LocalUsers[iLocalUserIndex]), pVoipUser, sizeof(*pVoipUser));
                // set output device to be ok, assuming speakers present
                pVoip->Common.Connectionlist.uLocalUserStatus[iLocalUserIndex] |= VOIP_LOCAL_USER_OUTPUTDEVICEOK;
            }
        }
        else
        {
            pVoip->Common.Connectionlist.uLocalUserStatus[iLocalUserIndex] &= ~(VOIP_LOCAL_USER_HEADSETOK);
            ds_memclr(&pVoip->Common.Connectionlist.LocalUsers[iLocalUserIndex], sizeof(pVoip->Common.Connectionlist.LocalUsers[iLocalUserIndex]));

        }

        pVoip->Common.Connectionlist.bApplyRelFromMuting = TRUE;
    }
    return(iResult);
}

/*F********************************************************************************/
/*!
    \Function   _VoipRegisterLocalSharedTalker

    \Description
        Helper function to register/unregister a shared user

    \Input *pVoip           - voip module reference
    \Input bRegister        - TRUE to register user, FALSE to unregister

    \Version 12/10/2014 (tcho)
*/
/********************************************************************************F*/
static void _VoipRegisterLocalSharedTalker(VoipRefT *pVoip, uint32_t bRegister)
{
    if (bRegister)
    {
        // do we have a shared user? If not add one
        if (VOIP_NullUser((VoipUserT *)(&pVoip->Common.Connectionlist.LocalUsers[VOIP_SHARED_USER_INDEX])))
        {
            VoipUserT voipSharedUser;
            ds_memclr(&voipSharedUser, sizeof(voipSharedUser));
            voipSharedUser.AccountInfo.iPersonaId = ((int64_t)pVoip->Common.Connectionlist.uClientId) | VOIP_SHARED_USER_VALUE;
            ds_memcpy_s(&pVoip->Common.Connectionlist.LocalUsers[VOIP_SHARED_USER_INDEX], sizeof(pVoip->Common.Connectionlist.LocalUsers[VOIP_SHARED_USER_INDEX]), &voipSharedUser, sizeof(voipSharedUser));
            pVoip->Common.Connectionlist.aIsParticipating[VOIP_SHARED_USER_INDEX] = TRUE; // the shared user is always participating

            NetPrintf(("voipxboxone: local shared user added at index %d\n", VOIP_SHARED_USER_INDEX));
        }
    }
    else
    {
        // remove the shared user if there are no more local users left
        if (!VOIP_NullUser((VoipUserT *)(&pVoip->Common.Connectionlist.LocalUsers[VOIP_SHARED_USER_INDEX])))
        {
            ds_memclr(&pVoip->Common.Connectionlist.LocalUsers[VOIP_SHARED_USER_INDEX], sizeof(pVoip->Common.Connectionlist.LocalUsers[VOIP_SHARED_USER_INDEX]));
            pVoip->Common.Connectionlist.aIsParticipating[VOIP_SHARED_USER_INDEX] = FALSE;
            NetPrintf(("voipxboxone: shared user removed at index %d\n", VOIP_SHARED_USER_INDEX));
        }
    }
}

/*F********************************************************************************/
/*!
    \Function   _VoipUpdateLocalStatus

    \Description
        Update status of local users

    \Input *pVoip   - voip module reference

    \Version 12/04/2018 (mclouatre)
*/
/********************************************************************************F*/
static void _VoipUpdateLocalStatus(VoipRefT *pVoip)
{
    int32_t iLocalUserIndex;
    uint32_t uTick = NetTick();

    for (iLocalUserIndex = 0; iLocalUserIndex < VOIP_MAXLOCALUSERS; iLocalUserIndex++)
    {
        // check if we need to time out talking status
        if (VoipHeadsetStatus(pVoip->Common.pHeadset, 'xply', 0, NULL, 0) == TRUE)
        {
            if ((pVoip->Common.Connectionlist.uLocalUserStatus[iLocalUserIndex] & VOIP_LOCAL_USER_TALKING) && (NetTickDiff(uTick, pVoip->Common.Connectionlist.uLastVoiceTime[iLocalUserIndex]) > VOIP_TALKTIMEOUT))
            {
                pVoip->Common.Connectionlist.uLocalUserStatus[iLocalUserIndex] &= ~VOIP_LOCAL_USER_TALKING;
            }
        }
        else
        {
            // on native voip chat status comes from gamechat
            if (VoipHeadsetStatus(pVoip->Common.pHeadset, 'talk', iLocalUserIndex, NULL, 0) == TRUE)
            {
                pVoip->Common.Connectionlist.uLocalUserStatus[iLocalUserIndex] |= VOIP_LOCAL_USER_TALKING;
            }
            else
            {
                pVoip->Common.Connectionlist.uLocalUserStatus[iLocalUserIndex] &= ~VOIP_LOCAL_USER_TALKING;
            }
        }
    }
}

/*F********************************************************************************/
/*!
    \Function   _VoipThread

    \Description
        Main Voip thread that updates all active connections

    \Input *pParam      - pointer to voip module state

    \Output
        DWORD           - return value

    \Version 05/24/2013 (mclouatre)
*/
/********************************************************************************F*/
static void _VoipThread(void *pParam)
{
    VoipRefT *pVoip = (VoipRefT *)pParam;
    uint32_t uProcessCt = 0;
    uint64_t uTime0, uTime1;
    int32_t iLocalUserIndex;
    int64_t iSleep;
    VoipConnectionlistT *pConnectionlist = &pVoip->Common.Connectionlist;

    pVoip->hThread = GetCurrentThread();

    // execute until we're killed AND
    for (pVoip->iThreadActive = 1; pVoip->iThreadActive != 0; )
    {
        uTime0 = NetTickUsec();

        NetCritEnter(&pVoip->Common.ThreadCrit);
        NetCritEnter(&pConnectionlist->NetCrit);

        // apply the changes if the first party mute list has been updated
        for (iLocalUserIndex = 0; iLocalUserIndex < VOIP_MAXLOCALUSERS; iLocalUserIndex += 1)
        {
            if (VoipHeadsetStatus(pVoip->Common.pHeadset, 'fpmu', iLocalUserIndex, NULL, 0))
            {
                pVoip->Common.bApplyChannelConfig = TRUE;
            }
        }

        if (pVoip->Common.bApplyChannelConfig)
        {
            pVoip->Common.bApplyChannelConfig = FALSE;
            VoipCommonApplyChannelConfig((VoipCommonRefT *)pVoip);
        }

        // update status of remote users
        VoipCommonUpdateRemoteStatus(&pVoip->Common);

        /* With XBoxOne, per-connection outbound muting handling is rather deferred to
           VoipHeadsetXboxOne with usage of +snd, -snd. (We still alter
           pConnectionlist->uSendMask but its contents is ignored when sending over the voip
           connection.

           After rebasing VoipHeadsetXboxOne to MS game chat 2, we found out that the integration
           with our per-connection muting here was not behaving properly during unmuting: resuming
           submission of data frames to MS game chat 2 resulted in a ~ 5-sec delay before speech
           would resume. */
        if (pConnectionlist->bApplyRelFromMuting == TRUE)
        {
            int32_t iConnIndex;

            pConnectionlist->bApplyRelFromMuting = FALSE;

            for (iConnIndex = 0; iConnIndex < pConnectionlist->iMaxConnections; iConnIndex++)
            {
                for (iLocalUserIndex = 0; iLocalUserIndex < VOIP_MAXLOCALUSERS; iLocalUserIndex++)
                {
                    if (!VOIP_NullUser(&pConnectionlist->LocalUsers[iLocalUserIndex]))
                    {
                        int32_t iRemoteUserIndex;

                        for (iRemoteUserIndex = 0; iRemoteUserIndex < pConnectionlist->pConnections[iConnIndex].iMaxRemoteUsers; iRemoteUserIndex++)
                        {
                            VoipUserT *pRemoteUser = &pConnectionlist->pConnections[iConnIndex].RemoteUsers[iRemoteUserIndex];

                            if (!VOIP_NullUser(pRemoteUser))
                            {
                                VoipCommonControl((VoipCommonRefT *)VoipGetRef(), ((pConnectionlist->uSendMask & (1 << iConnIndex)) && (pConnectionlist->aIsParticipating[iLocalUserIndex] == TRUE)) ? '+snd' : '-snd', iLocalUserIndex, pRemoteUser);
                                VoipCommonControl((VoipCommonRefT *)VoipGetRef(), ((pConnectionlist->uRecvMask & (1 << iConnIndex)) && (pConnectionlist->aIsParticipating[iLocalUserIndex] == TRUE)) ? '+rcv' : '-rcv', iLocalUserIndex, pRemoteUser);
                            }
                        }
                    }
                }
            }
        }

        // update status of local users
        _VoipUpdateLocalStatus(pVoip);

        NetCritLeave(&pVoip->Common.Connectionlist.NetCrit);

        VoipConnectionUpdate(&pVoip->Common.Connectionlist);

        /*
        For Xbox One, the ConnectionList's critical section (NetCrit) is entered before
        calling VoipHeadsetProcess() to guarantee thread-safety between _VoipHeadsetProcessInboundQueue()
        or _VoipHeadsetUpdateEncodersDecodersForLocalCaptureSources() being executed from the voip thread,
        and _VoipHeadsetEnqueueInboundSubPacket() being executed from the socket recv thread.
        */
        NetCritEnter(&pVoip->Common.Connectionlist.NetCrit);

        VoipHeadsetProcess(pVoip->Common.pHeadset, uProcessCt++);

        NetCritLeave(&pVoip->Common.Connectionlist.NetCrit);

        NetCritLeave(&pVoip->Common.ThreadCrit);

        uTime1 = NetTickUsec();

        iSleep = (VOIP_THREAD_SLEEP_DURATION * 1000) - NetTickDiff(uTime1, uTime0);

        if (iSleep >= 0)
        {
            // converts from usec to msec before calling NetConnSleep
            NetConnSleep(iSleep / 1000);
        }
        else
        {
            NetPrintf(("voipxboxone: [WARNING] Overall voip thread update took %d ms\n", NetTickDiff(uTime1, uTime0)/1000));
        }
    }

    // indicate we're exiting
    pVoip->hThread = NULL;
}

/*F********************************************************************************/
/*!
    \Function   _VoipShutdownInternal

    \Description
        Release all VoIP resources; implemented as a NetConn shutdown function

    \Input *pDestroyData    - voip module reference to shut down

    \Output
        int32_t             - zero if shutdown is complete, else negative

    \Version 07/31/2015 (jbrookes)
*/
/********************************************************************************F*/
static int32_t _VoipShutdownInternal(void *pDestroyData)
{
    VoipRefT *pVoip = (VoipRefT *)pDestroyData;

    // check for thread exit confirmation
    if (pVoip->hThread != NULL)
    {
        // not ready
        return(-1);
    }

    // remove the shared user
    _VoipRegisterLocalSharedTalker(pVoip, FALSE);

    // common shutdown (must come last as this frees the memory)
    VoipCommonShutdown(&pVoip->Common);

    // return success
    return(0);
}


/*** Public Functions *************************************************************/

/*F********************************************************************************/
/*!
    \Function   VoipStartup

    \Description
        Prepare VoIP for use.

    \Input iMaxPeers    - maximum number of peers supported (up to VOIP_MAXCONNECT)
    \Input iData        - platform-specific - unused for xboxone

    \Output
        VoipRefT        - state reference that is passed to all other functions

    \Version 05/24/2013 (mclouatre)
*/
/********************************************************************************F*/
VoipRefT *VoipStartup(int32_t iMaxPeers, int32_t iData)
{
    VoipRefT *pVoip;
    VoipCommonRefT *pVoipCommon;
    DirtyThreadConfigT ThreadConfig;
    int32_t iResult;

    // make sure we're not already started up
    if (VoipGetRef() != NULL)
    {
        NetPrintf(("voipxboxone: module startup called when already in a started state\n"));
        return(NULL);
    }

    // common startup
    if ((pVoip = VoipCommonStartup(iMaxPeers, sizeof(*pVoip), _VoipStatusCb, iData)) == NULL)
    {
        return(NULL);
    }
    pVoipCommon = (VoipCommonRefT *)pVoip;

    // permission is handled by the Game Chat API so bPrivileged is always TRUE
    pVoipCommon->bPrivileged = TRUE;

    // configure thread parameters
    ds_memclr(&ThreadConfig, sizeof(ThreadConfig));
    ThreadConfig.pName = "VoIP";
    
    // set the thread affinity to default if not set
    if (_Voip_iThreadAffinity == -1)
    {
        ThreadConfig.iAffinity = NetConnStatus('affn', 0, NULL, 0);
    }
    else
    {
        ThreadConfig.iAffinity = _Voip_iThreadAffinity;
    }

    // set the thread priority to default if not set
    if (_Voip_iThreadPriority == -1)
    {
        ThreadConfig.iPriority = THREAD_PRIORITY_TIME_CRITICAL;
    }
    else
    {
        ThreadConfig.iPriority = _Voip_iThreadPriority;
    }

    // create our own worker thread
    if ((iResult = DirtyThreadCreate(_VoipThread, pVoip, &ThreadConfig)) == 0)
    {
        // wait for thread startup
        while (pVoip->iThreadActive == 0)
        {
            Sleep(1);
        }
    }
    else
    {
        NetPrintf(("voipxboxone: error creating voip thread (err=%d)\n", iResult));
        pVoip->iThreadActive = 0;
        VoipShutdown(pVoip, 0);
        return(NULL);
    }


    // add a local shared user
    _VoipRegisterLocalSharedTalker(pVoip, TRUE);

    // return to caller
    return(pVoip);
}

/*F********************************************************************************/
/*!
    \Function   VoipShutdown

    \Description
        Release all VoIP resources.

    \Input *pVoip           - module state from VoipStartup
    \Input uShutdownFlags   - NET_SHUTDOWN_* flags

    \Version 05/24/2013 (mclouatre)
*/
/********************************************************************************F*/
void VoipShutdown(VoipRefT *pVoip, uint32_t uShutdownFlags)
{
    // make sure we're really started up
    if (VoipGetRef() == NULL)
    {
        NetPrintf(("voipxboxone: module shutdown called when not in a started state\n"));
        return;
    }

    NetPrintf(("voipxboxone: [%p] shutting down\n", pVoip));

    // tell the thread to exit
    pVoip->iThreadActive = 0;

    // transfer destruction ownership to NetConn
    NetConnControl('recu', 0, 0, (void *)_VoipShutdownInternal, pVoip);
}

/*F********************************************************************************/
/*!
    \Function VoipSetLocalUser

    \Description
        Register/unregister specified local user with the voip sub-system.

    \Input *pVoip           - module state from VoipStartup
    \Input iLocalUserIndex  - controller index of user
    \Input bRegister        - TRUE to register, FALSE to unregister.

    \Version 05/24/2013 (mclouatre)
*/
/********************************************************************************F*/
void VoipSetLocalUser(VoipRefT *pVoip, int32_t iLocalUserIndex, uint32_t bRegister)
{
    if ((iLocalUserIndex < VOIP_MAXLOCALUSERS) && (iLocalUserIndex >= 0))
    {
        NetCritEnter(&pVoip->Common.ThreadCrit);

        if (bRegister)
        {
            // is this slot empty?
            if (VOIP_NullUser((VoipUserT *)(&pVoip->Common.Connectionlist.LocalUsers[iLocalUserIndex])))
            {
                VoipUserT voipUser;
                ds_memclr(&voipUser, sizeof(voipUser));

                voipUser.ePlatform = VOIP_PLATFORM_XBOXONE;
                NetConnStatus('acid', iLocalUserIndex, &voipUser.AccountInfo.iAccountId, sizeof(voipUser.AccountInfo.iAccountId));
                NetConnStatus('peid', iLocalUserIndex, &voipUser.AccountInfo.iPersonaId, sizeof(voipUser.AccountInfo.iPersonaId));

                // register the local talker
                NetPrintf(("voipxboxone: registering local user %lld at index %d\n", voipUser.AccountInfo.iPersonaId, iLocalUserIndex));
                _VoipRegisterLocalTalker(pVoip, iLocalUserIndex, TRUE, &voipUser);
            }
            else
            {
                NetPrintf(("voipxboxone: warning -- ignoring attempt to register user at index %d because user %lld is already registered at that index\n",
                    iLocalUserIndex, ((VoipUserT *)(&pVoip->Common.Connectionlist.LocalUsers[iLocalUserIndex]))->AccountInfo.iPersonaId));
            }
        }
        else
        {
            // is there a user at this slot?
            if (!VOIP_NullUser((VoipUserT *)(&pVoip->Common.Connectionlist.LocalUsers[iLocalUserIndex])))
            {
                // if a participating user demote him first
                if (pVoip->Common.Connectionlist.aIsParticipating[iLocalUserIndex] == TRUE)
                {
                    VoipCommonActivateLocalUser(&pVoip->Common, iLocalUserIndex, FALSE);
                }

                NetPrintf(("voipxboxone: disabling voice for user at index %d\n", iLocalUserIndex));
                _VoipRegisterLocalTalker(pVoip, iLocalUserIndex, FALSE, NULL);
            }
            else
            {
                NetPrintf(("voipxboxone: warning -- ignoring attempt to un-register user at index %d because it is empty\n", iLocalUserIndex));
            }
        }

        NetCritLeave(&pVoip->Common.ThreadCrit);
    }
    else
    {
        NetPrintf(("voipxboxone: VoipSetLocalUser() invoked with an invalid user index %d\n", iLocalUserIndex));
    }
}

/*F********************************************************************************/
/*!
    \Function   VoipCommonActivateLocalUser

    \Description
        Promote/demote specified local user to/from "participating" state

    \Input *pVoipCommon     - voip common state
    \Input iLocalUserIndex  - local user index
    \Input bActivate        - TRUE to activate, FALSE to deactivate

    \Version 04/25/2013 (tcho)
*/
/********************************************************************************F*/
void VoipCommonActivateLocalUser(VoipCommonRefT *pVoipCommon, int32_t iLocalUserIndex, uint8_t bActivate)
{
    int32_t iResult = 0;
    uint64_t uXuid;

    if ((pVoipCommon != NULL) && (iLocalUserIndex >= 0) && (iLocalUserIndex < NETCONN_MAXLOCALUSERS))
    {
        NetCritEnter(&pVoipCommon->ThreadCrit);

        if (!VOIP_NullUser((VoipUserT *)(&pVoipCommon->Connectionlist.LocalUsers[iLocalUserIndex])))
        {
            if (NetConnStatus('xuid', iLocalUserIndex, &uXuid, sizeof(uXuid)) == 0)
            {
                if ((iResult = VoipHeadsetControl(pVoipCommon->pHeadset, 'aloc', iLocalUserIndex, bActivate, NULL)) < 0)
                {
                    NetPrintf(("voipxboxone: cannot %s a null user %s participating state (local user index %d)\n",
                        (bActivate?"promote":"demote"), (bActivate?"to":"from"), iLocalUserIndex));
                }

                pVoipCommon->Connectionlist.aIsParticipating[iLocalUserIndex] = bActivate;

                // reapply playback muting config based on the channel configuration
                pVoipCommon->bApplyChannelConfig = TRUE;

                // reapply player-to-player xone comm relationships
                pVoipCommon->Connectionlist.bApplyRelFromMuting = TRUE;

                // announce jip add or remove event on connection that are already active
                VoipConnectionReliableBroadcastUser(&pVoipCommon->Connectionlist, iLocalUserIndex, bActivate);
            }
            else
            {
                NetPrintf(("voipxboxone: local talker registration aborted - local user at index %d either doesn't exist or has an invalid xuid\n", iLocalUserIndex));
            }
        }
        else
        {
            NetPrintf(("voipxboxone: VoipCommonActivateLocalUser() failed because no local user registered at index %d\n", iLocalUserIndex));
        }

        NetCritLeave(&pVoipCommon->ThreadCrit);
    }
    else
    {
        NetPrintf(("voipxboxone: VoipCommonActivateLocalUser() invoked with an invalid user index %d\n", iLocalUserIndex));
    }
}

/*F********************************************************************************/
/*!
    \Function   VoipStatus

    \Description
        Return status.

    \Input *pVoip   - module state from VoipStartup
    \Input iSelect  - status selector
    \Input iValue   - selector-specific
    \Input *pBuf    - [out] storage for selector-specific output
    \Input iBufSize - size of output buffer

    \Output
        int32_t     - selector-specific data

    \Notes
        iSelect can be one of the following:

        \verbatim
            'hset' - bitfield indicating which ports have headsets
            'pcac' - returns true if party chat is active
            'pcsu' - returns true if party chat is currently suspended
            'thid' - returns handle of _VoipThread
        \endverbatim

        Unhandled selectors are passed through to VoipCommonStatus(), and
        further to VoipHeadsetStatus() if unhandled there.

    \Version 05/24/2013 (mclouatre)
*/
/********************************************************************************F*/
int32_t VoipStatus(VoipRefT *pVoip, int32_t iSelect, int32_t iValue, void *pBuf, int32_t iBufSize)
{
    if (iSelect == 'pcac')
    {
        #ifndef DIRTYCODE_GDK
        //ref: https://forums.xboxlive.com/AnswerPage.aspx?qid=c0bdf120-e54c-49be-8141-8f67d31e8add&tgt=1
        try
        {
            return(PartyChat::IsPartyChatActive);
        }
        catch (Platform::Exception ^ e)
        {
            NetPrintf(("voipxboxone: error, exception thrown when calling get PartyChat::IsPartyChatActive (%S/0x%08x)\n", e->Message->Data(), e->HResult));
            return(-1);
        }
        #endif
    }
    if (iSelect == 'pcsu')
    {
        #ifndef DIRTYCODE_GDK
        //ref: https://forums.xboxlive.com/AnswerPage.aspx?qid=c0bdf120-e54c-49be-8141-8f67d31e8add&tgt=1
        try
        {
            return(PartyChat::IsPartyChatSuppressed);
        }
        catch (Platform::Exception ^ e)
        {
            NetPrintf(("voipxboxone: error, exception thrown when calling get PartyChat::IsPartyChatActive (%S/0x%08x)\n", e->Message->Data(), e->HResult));
            return(-1);
        }
        #endif
    }
    if (iSelect == 'thid')
    {
        *(HANDLE *)pBuf = pVoip->hThread;
        return(0);
    }
    return(VoipCommonStatus(&pVoip->Common, iSelect, iValue, pBuf, iBufSize));
}

/*F********************************************************************************/
/*!
    \Function   VoipControl

    \Description
        Set control options.

    \Input *pVoip   - module state from VoipStartup
    \Input iControl - control selector
    \Input iValue   - selector-specific input
    \Input *pValue  - selector-specific input

    \Output
        int32_t     - selector-specific output

    \Notes
        iControl can be one of the following:

        \verbatim
            'affn' - set the voip thread cpu affinity, please call this before voip startup is called with pVoip being null
            'amow' - enable one way bad rep auto muting
            '+pbk' - enable voice playback for a given remote user (pValue is the remote user VoipUserT).
            '-pbk' - disable voice playback for a given remote user (pValue is the remote user VoipUserT).
            'prio' - set the voip thread priority, please call this before voip startup is called with pVoip being null
            'pcsu' - takes iValue as a bool, TRUE suspend party chat, FALSE resume it.
                In order for VoipControl 'pcsu' to work you must add to the appxmanifest.xml:
                <mx:Extension Category="xbox.multiplayer">
                    <mx:XboxMultiplayer CanSuppressPartyChat="true" />
                </mx:Extension>
        \endverbatim

        Unhandled selectors are passed through to VoipCommonControl(), and
        further to VoipHeadsetControl() if unhandled there.

    \Version 05/24/2013 (mclouatre)
*/
/********************************************************************************F*/
int32_t VoipControl(VoipRefT *pVoip, int32_t iControl, int32_t iValue, void *pValue)
{
    if ('affn' == iControl)
    {
        _Voip_iThreadAffinity = iValue;
        return(0);
    }
    if ('amow' == iControl)
    {
        pVoip->bOnewayBadRepMute = (uint8_t)(iValue);
        return(0);
    }
    if ('+pbk' == iControl)
    {
        return(VoipHeadsetControl(pVoip->Common.pHeadset, '+pbk', iValue, 0, (VoipUserT *)pValue));
    }
    if ('-pbk' == iControl)
    {
        return(VoipHeadsetControl(pVoip->Common.pHeadset, '-pbk', iValue, 0, (VoipUserT *)pValue));
    }
    if ('pcsu' == iControl)
    {
        #ifndef DIRTYCODE_GDK
        NetPrintf(("voipxboxone: IsPartyChatSuppressed=%d", iValue));

        //ref: https://forums.xboxlive.com/AnswerPage.aspx?qid=c0bdf120-e54c-49be-8141-8f67d31e8add&tgt=1
        try
        {
            PartyChat::IsPartyChatSuppressed = (iValue != 0);
        }
        catch (Platform::Exception ^ e)
        {
            NetPrintf(("voipxboxone: error, exception thrown when calling set PartyChat::IsPartyChatSuppressed (%S/0x%08x)\n", e->Message->Data(), e->HResult));
            return(-1);
        }
        #endif
        return(iValue);
    }
    if ('prio' == iControl)
    {
        _Voip_iThreadPriority = iValue;
        return(0);
    }
    return(VoipCommonControl(&pVoip->Common, iControl, iValue, pValue));
}

