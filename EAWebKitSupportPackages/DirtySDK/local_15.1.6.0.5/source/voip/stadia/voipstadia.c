/*H********************************************************************************/
/*!
    \File voipstadia.c

    \Description
        voip interface

    \Copyright
        Copyright (c) Electronic Arts 2020. ALL RIGHTS RESERVED.

    \Version 02/28/2020 (eesponda)
*/
/********************************************************************************H*/

/*** Include files ****************************************************************/

// dirtysock includes
#include "DirtySDK/platform.h"
#include "DirtySDK/dirtysock.h"
#include "DirtySDK/dirtysock/netconn.h"
#include "DirtySDK/dirtysock/dirtylib.h"
#include "DirtySDK/dirtysock/dirtymem.h"
#include "DirtySDK/dirtysock/dirtythread.h"

// voip includes
#include "DirtySDK/voip/voipdef.h"
#include "voippriv.h"
#include "voipcommon.h"
#include "DirtySDK/voip/voip.h"

/*** Defines **********************************************************************/

#define VOIP_THREAD_SLEEP_DURATION        (10)

/*** Macros ***********************************************************************/

/*** Type Definitions *************************************************************/

//! VoIP module state
struct VoipRefT
{
    VoipCommonRefT  Common;             //!< cross-platform voip data (must come first!)
    volatile int32_t iThreadState;      //!< 0=dead, 1=alive, 2=pending shutdown
};

/*** Function Prototypes **********************************************************/

/*** Variables ********************************************************************/

//! affinity of the voip thread, if unset uses the global DirtySDK thread affinity
static int32_t _Voip_iThreadAffinity = -1;

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
        this platform.

    \Version 02/28/2020 (eesponda)
*/
/********************************************************************************F*/
static void _VoipStatusCb(int32_t iLocalUserIndex, uint32_t bStatus, VoipHeadsetStatusUpdateE eUpdate, void *pUserData)
{
    VoipRefT *pRef = (VoipRefT *)pUserData;

    if (eUpdate != VOIP_HEADSET_STATUS_INPUT)
    {
        NetPrintf(("voipstadia: warning, unexpected headset event for this platform!\n"));
    }

    if (bStatus)
    {
        NetPrintf(("voipstadia: [%d] headset active (audio in: on, audio out: on)\n", iLocalUserIndex));
        pRef->Common.uPortHeadsetStatus |= 1 << iLocalUserIndex;
        pRef->Common.Connectionlist.uLocalUserStatus[iLocalUserIndex] |= (VOIP_LOCAL_USER_HEADSETOK|VOIP_LOCAL_USER_INPUTDEVICEOK|VOIP_LOCAL_USER_OUTPUTDEVICEOK);
    }
    else
    {
        NetPrintf(("voipstadia: [%d] headset inactive (audio in: off, audio out: on)\n", iLocalUserIndex));
        pRef->Common.uPortHeadsetStatus &= ~(1 << iLocalUserIndex);
        pRef->Common.Connectionlist.uLocalUserStatus[iLocalUserIndex] &= ~(VOIP_LOCAL_USER_HEADSETOK|VOIP_LOCAL_USER_INPUTDEVICEOK|VOIP_LOCAL_USER_OUTPUTDEVICEOK);
    }
}

/*F********************************************************************************/
/*!
    \Function   _VoipThread

    \Description
        Updates the voip connection and does other voip related processing

    \Input *pParam    - voip ref

    \Version 02/28/2020 (eesponda)
*/
/********************************************************************************F*/
static void _VoipThread(void *pParam)
{
    VoipRefT *pVoip = (VoipRefT *)pParam;
    uint32_t uTime0, uTime1;
    int32_t iSleep;

    // wait until we're started up
    if (pVoip->iThreadState == 0)
    {
        NetConnSleep(VOIP_THREAD_SLEEP_DURATION);
    }

    // execute until we've killed
    while (pVoip->iThreadState == 1)
    {
        int32_t iLocalUserIndex;
        uint32_t uProcessCt = 0, uTtsStatus;

        uTime0 = NetTick();

        NetCritEnter(&pVoip->Common.ThreadCrit);
        NetCritEnter(&pVoip->Common.Connectionlist.NetCrit);

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
        NetCritLeave(&pVoip->Common.Connectionlist.NetCrit);

        // update connections
        VoipConnectionUpdate(&pVoip->Common.Connectionlist);

        // set headset mute status based on connectionlist send mask
        for (iLocalUserIndex = 0; iLocalUserIndex < VOIP_MAXLOCALUSERS; iLocalUserIndex += 1)
        {
            VoipHeadsetControl(pVoip->Common.pHeadset, 'mute', iLocalUserIndex, (pVoip->Common.uUserSendMask & (1 << iLocalUserIndex)) == 0, NULL);
        }

        // update headset
        VoipHeadsetProcess(pVoip->Common.pHeadset, uProcessCt++);

        // update ttos status
        for (iLocalUserIndex = 0, uTtsStatus = 0; iLocalUserIndex < VOIP_MAXLOCALUSERS; iLocalUserIndex += 1)
        {
            uTtsStatus |= VoipHeadsetStatus(pVoip->Common.pHeadset, 'ttos', iLocalUserIndex, NULL, 0) << iLocalUserIndex;
        }
        pVoip->Common.uTtsStatus = uTtsStatus;

        NetCritLeave(&pVoip->Common.ThreadCrit);

        uTime1 = NetTick();

        // wait for next tick
        if ((iSleep = VOIP_THREAD_SLEEP_DURATION - NetTickDiff(uTime1, uTime0)) >= 0)
        {
            NetConnSleep(iSleep);
        }
        else
        {
            NetPrintf(("voipunix: [WARNING] Overall voip update took %d ms\n", NetTickDiff(uTime1, uTime0)));
        }
    }

    // notify main thread we'are done
    pVoip->iThreadState = 0;
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
        int32_t             - TRUE if the local talker was successfully registered, else FALSE

    \Version 02/28/2020 (eesponda)
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
            pVoip->Common.Connectionlist.uLocalUserStatus[iLocalUserIndex] &= ~(VOIP_LOCAL_USER_HEADSETOK|VOIP_LOCAL_USER_INPUTDEVICEOK|VOIP_LOCAL_USER_OUTPUTDEVICEOK);
            ds_memclr(&pVoip->Common.Connectionlist.LocalUsers[iLocalUserIndex], sizeof(pVoip->Common.Connectionlist.LocalUsers[iLocalUserIndex]));
        }
    }
    return(iResult);
}

/*** Public Functions *************************************************************/

/*F********************************************************************************/
/*!
    \Function   VoipStartup

    \Description
        Prepare VoIP for use.

    \Input iMaxPeers    - maximum number of peers supported (up to VOIP_MAXCONNECT)
    \Input iData        - platform-specific - unused on Unix

    \Output
        VoipRefT        - state reference that is passed to all other functions

    \Version 02/28/2020 (eesponda)
*/
/********************************************************************************F*/
VoipRefT *VoipStartup(int32_t iMaxPeers, int32_t iData)
{
    VoipRefT *pVoip;
    DirtyThreadConfigT ThreadConfig;
    int32_t iResult;

    // make sure we're not already started
    if (VoipGetRef() != NULL)
    {
        NetPrintf(("voipstadia: module startup called when already in a started state\n"));
        return(NULL);
    }

    // common startup
    if ((pVoip = VoipCommonStartup(iMaxPeers, sizeof(*pVoip), _VoipStatusCb, iData)) == NULL)
    {
        return(NULL);
    }

    // configure the thread parameters
    ds_memclr(&ThreadConfig, sizeof(ThreadConfig));
    ThreadConfig.pName = "VoIP";
    ThreadConfig.iAffinity = (_Voip_iThreadAffinity == -1) ? NetConnStatus('affn', 0, NULL, 0) : _Voip_iThreadAffinity;
    ThreadConfig.iPriority = 0;

    // create worker thread
    if ((iResult = DirtyThreadCreate(_VoipThread, pVoip, &ThreadConfig)) == 0)
    {
        pVoip->iThreadState = 1;
    }
    else
    {
        NetPrintf(("voipstadia: unable to create voip thread (err=%d)\n", iResult));
        pVoip->iThreadState = 0;
        VoipShutdown(pVoip, 0);
        return(NULL);
    }

    // bPrivileged is always TRUE on stadia
    pVoip->Common.bPrivileged = TRUE;

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

    \Version 02/28/2020 (eesponda)
*/
/********************************************************************************F*/
void VoipShutdown(VoipRefT *pVoip, uint32_t uShutdownFlags)
{
    // make sure we're really started up
    if (VoipGetRef() == NULL)
    {
        NetPrintf(("voipstadia: module shutdown called when not in a started state\n"));
        return;
    }

    // tell thread to shut down and wait for shutdown confirmation (if running)
    if (pVoip->iThreadState == 1)
    {
        pVoip->iThreadState = 2;
        while (pVoip->iThreadState != 0)
        {
            NetConnSleep(VOIP_THREAD_SLEEP_DURATION);
        }
    }

    // common shutdown (must come last as this frees the memory)
    VoipCommonShutdown(&pVoip->Common);
}

/*F********************************************************************************/
/*!
    \Function   VoipSetLocalUser

    \Description
        Register/unregister specified local user with the voip sub-system.

    \Input *pVoip           - module state from VoipStartup
    \Input iLocalUserIndex  - local user index
    \Input bRegister        - if we are registering or unregistering

    \Version 02/28/2020 (eesponda)
*/
/********************************************************************************F*/
void VoipSetLocalUser(VoipRefT *pVoip, int32_t iLocalUserIndex, uint32_t bRegister)
{
    if (bRegister)
    {
        if (VOIP_NullUser(&pVoip->Common.Connectionlist.LocalUsers[iLocalUserIndex]))
        {
            VoipUserT VoipUser;
            ds_memclr(&VoipUser, sizeof(VoipUser));

            VoipUser.ePlatform = VOIP_PLATFORM_LINUX;
            NetConnStatus('acid', iLocalUserIndex, &VoipUser.AccountInfo.iAccountId, sizeof(VoipUser.AccountInfo.iAccountId));
            NetConnStatus('peid', iLocalUserIndex, &VoipUser.AccountInfo.iPersonaId, sizeof(VoipUser.AccountInfo.iPersonaId));

            NetPrintf(("voipstadia: registering a local user %llx at index %d\n", VoipUser.AccountInfo.iPersonaId, iLocalUserIndex));
            _VoipRegisterLocalTalker(pVoip, iLocalUserIndex, TRUE, &VoipUser);
        }
        else
        {
            NetPrintf(("voipstadia: warning -- ignoring attempt to register local user because user %llx is already registered\n",
                pVoip->Common.Connectionlist.LocalUsers[iLocalUserIndex].AccountInfo.iPersonaId));
        }
    }
    else
    {
        if (!VOIP_NullUser(&pVoip->Common.Connectionlist.LocalUsers[iLocalUserIndex]))
        {
            // if a participating user demote him first
            if (pVoip->Common.Connectionlist.aIsParticipating[iLocalUserIndex] == TRUE)
            {
                VoipCommonActivateLocalUser(&pVoip->Common, 0, FALSE);
            }

            NetPrintf(("voipstadia: unregistering local user %llx\n", pVoip->Common.Connectionlist.LocalUsers[iLocalUserIndex].AccountInfo.iPersonaId));
            _VoipRegisterLocalTalker(pVoip, iLocalUserIndex, FALSE, NULL);
        }
        else
        {
            NetPrintf(("voipstadia: warning -- ignoring attempt to unregister local user because it is not yet registered\n"));
        }
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

    \Version 02/28/2020 (eesponda)
*/
/********************************************************************************F*/
void VoipCommonActivateLocalUser(VoipCommonRefT *pVoipCommon, int32_t iLocalUserIndex, uint8_t bActivate)
{
    if ((iLocalUserIndex < 0) || (iLocalUserIndex >= VOIP_MAXLOCALUSERS))
    {
        NetPrintf(("voipstadia: VoipCommonActivateLocalUser() involved with an invalid user index %d\n", iLocalUserIndex));
        return;
    }

    NetCritEnter(&pVoipCommon->ThreadCrit);

    if (!VOIP_NullUser(&pVoipCommon->Connectionlist.LocalUsers[iLocalUserIndex]))
    {
        NetPrintf(("voipstadia: %s participating state (index %d, id %llx)\n", bActivate ? "promoting user to" : "demoting user from",
            iLocalUserIndex, pVoipCommon->Connectionlist.LocalUsers[iLocalUserIndex].AccountInfo.iPersonaId));

        VoipHeadsetControl(pVoipCommon->pHeadset, 'aloc', 0, bActivate, &pVoipCommon->Connectionlist.LocalUsers[iLocalUserIndex]);

        // mark local user as participating, or not
        pVoipCommon->Connectionlist.aIsParticipating[iLocalUserIndex] = bActivate;

        // reapply playback muting config based on the channel configuration only if this is a participating user
        pVoipCommon->bApplyChannelConfig = TRUE;

        // announce add or remove event on connection
        VoipConnectionReliableBroadcastUser(&pVoipCommon->Connectionlist, iLocalUserIndex, bActivate);
    }
    else
    {
        NetPrintf(("voipstadia: VoipCommonActivateLocalUser() failed because no local user registered\n"));
    }

    NetCritLeave(&pVoipCommon->ThreadCrit);
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

    \Version 06/12/2017 (mclouatre)
*/
/********************************************************************************F*/
int32_t VoipStatus(VoipRefT *pVoip, int32_t iSelect, int32_t iValue, void *pBuf, int32_t iBufSize)
{
    return(VoipCommonStatus(&pVoip->Common, iSelect, iValue, pBuf, iBufSize));
}

/*F********************************************************************************/
/*!
    \Function   VoipControl

    \Description
        Set control options.

    \Input *pVoip       - module state from VoipStartup
    \Input iControl     - control selector
    \Input iValue       - selector-specific
    \Input *pValue      - selector-specific

    \Notes
        iControl can be one of the following:

        \verbatim
        '+pbk' - enable the playback for a remote user (iValue=local user index, pValue=remote user)
        '-pbk' - disable the playback for a remote user (iValue=local user index, pValue=remote user)
        'affn' - the affinity for the voip thread, must be called before VoipStartup
        \endverbatim

        Unhandled selectors are passed through to VoipCommonControl(), and
        further to VoipHeadsetControl() if unhandled there.

    \Version 06/12/2017 (mclouatre)
*/
/********************************************************************************F*/
int32_t VoipControl(VoipRefT *pVoip, int32_t iControl, int32_t iValue, void *pValue)
{
    if ((iControl == '+pbk') || (iControl == '-pbk'))
    {
        return(VoipHeadsetControl(pVoip->Common.pHeadset, iControl, iValue, 0, pValue));
    }
    if (iControl == 'affn')
    {
        _Voip_iThreadAffinity = iValue;
        return(0);
    }
    return(VoipCommonControl(&pVoip->Common, iControl, iValue, pValue));
}


