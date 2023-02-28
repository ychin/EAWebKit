/*H********************************************************************************/
/*!
    \File voipps4.c

    \Description
        Voip library interface.

    \Copyright
        Copyright (c) Electronic Arts 2013. ALL RIGHTS RESERVED.

    \Version 01/16/2013 (mclouatre) First Version
*/
/********************************************************************************H*/

/*** Include files ****************************************************************/

#include <kernel.h>
#include <string.h>
#include <np.h>

// dirtysock includes
#include "DirtySDK/platform.h"
#include "DirtySDK/dirtysock.h"
#include "DirtySDK/dirtysock/netconn.h"
#include "DirtySDK/dirtysock/dirtythread.h"

// voip includes
#include "DirtySDK/voip/voipdef.h"
#include "voippriv.h"
#include "voipcommon.h"

/*** Defines **********************************************************************/
#define VOIP_THREAD_SLEEP_DURATION        (20)

/*** Macros ***********************************************************************/

/*** Type Definitions *************************************************************/

//! VoIP module state
struct VoipRefT
{
    VoipCommonRefT              Common;             //!< cross-platform voip data (must come first!)

    volatile int32_t            iThreadActive;
};

/*** Function Prototypes **********************************************************/

/*** Variables ********************************************************************/

// Private Variables

//! affinity of the voip thread, if unset uses the global DirtySDK thread affinity
static int32_t _Voip_iThreadAffinity = -1;

// Public Variables

//! Global array used to track which local user(s) shall be included in the most restritive voip privileges calculation.
//! This is not tracked in the VoipRef because we want to empower the game code to pass this info (via '+pri' and '-pri' 
//! control selectors) even if voip is not yet started. The info tracked in this array is only used with users that are
//! NOT yet registered with VOIP. When a user is already registered with voip, he is by default included in the most
//! restritive voip privileges calculation
static uint8_t _VoipPs4_MostRestrictiveVoipContributors[NETCONN_MAXLOCALUSERS] = { FALSE, FALSE, FALSE, FALSE };

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

    \Version 01/16/2013 (mclouatre)
*/
/********************************************************************************F*/
static void _VoipStatusCb(int32_t iLocalUserIndex, uint32_t bStatus, VoipHeadsetStatusUpdateE eUpdate, void *pUserData)
{
    VoipRefT *pRef = (VoipRefT *)pUserData;

    if (eUpdate != VOIP_HEADSET_STATUS_INPUT)
    {
        NetPrintf(("voipps4: warning, unexpected headset event for this platform!\n"));
    }

    if (bStatus)
    {
        NetPrintf(("voipps4: [%d] headset active (audio in: on, audio out: on)\n", iLocalUserIndex));
        pRef->Common.uPortHeadsetStatus |= 1 << iLocalUserIndex;
        pRef->Common.Connectionlist.uLocalUserStatus[iLocalUserIndex] |= (VOIP_LOCAL_USER_HEADSETOK|VOIP_LOCAL_USER_INPUTDEVICEOK);
    }
    else
    {
        NetPrintf(("voipps4: [%d] headset inactive (audio in: off, audio out: on)\n", iLocalUserIndex));
        pRef->Common.uPortHeadsetStatus &= ~(1 << iLocalUserIndex);
        pRef->Common.Connectionlist.uLocalUserStatus[iLocalUserIndex] &= ~(VOIP_LOCAL_USER_HEADSETOK|VOIP_LOCAL_USER_INPUTDEVICEOK);
    }
}

/*F********************************************************************************/
/*!
    \Function   _VoipRegisterLocalSharedTalker

    \Description
        Helper function to register/unregister a shared user

    \Input *pVoip           - voip module reference
    \Input bRegister        - TRUE to register user, FALSE to unregister

    \Version 01/12/2014 (tcho)
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
            
            NetPrintf(("voipps4: local shared user added at index %d\n", VOIP_SHARED_USER_INDEX));
        }
    }
    else
    {
        // remove the shared user if there are no more local users left
        if (!VOIP_NullUser((VoipUserT *)(&pVoip->Common.Connectionlist.LocalUsers[VOIP_SHARED_USER_INDEX])))
        {
            ds_memclr(&pVoip->Common.Connectionlist.LocalUsers[VOIP_SHARED_USER_INDEX], sizeof(pVoip->Common.Connectionlist.LocalUsers[VOIP_SHARED_USER_INDEX]));
            pVoip->Common.Connectionlist.aIsParticipating[VOIP_SHARED_USER_INDEX] = FALSE;
            NetPrintf(("voipps4: shared user removed at index %d\n", VOIP_SHARED_USER_INDEX));
        }

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
        int32_t             - TRUE if the local talker was successfully registered, else FALSE

    \Version 02/05/2013 (mclouatre)
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

/*F********************************************************************************/
/*!
    \Function   _VoipActivateLocalTalker

    \Description
        Register the participating talker on the given port.

    \Input *pVoipCommon     - voip common state
    \Input iLocalUserIndex  - local user index associated with the user
    \Input *pVoipUser       - VoipUserT to register
    \Input bActivate        - TRUE to activate, FALSE to deactivate

    \Output
        int32_t         - TRUE if the local talker was successfully registered, else FALSE

    \Version 04/25/2013 (tcho)
*/
/********************************************************************************F*/
static int32_t _VoipActivateLocalTalker(VoipCommonRefT *pVoipCommon, int32_t iLocalUserIndex, VoipUserT *pVoipUser, uint8_t bActivate)
{
    int32_t iResult = -1;

    if (pVoipUser != NULL)
    {
        iResult = VoipHeadsetControl(pVoipCommon->pHeadset, 'aloc', iLocalUserIndex, bActivate, pVoipUser);
        if (iResult >= 0)
        {
            if (bActivate == TRUE)
            {
                // mark local user as participating
                pVoipCommon->Connectionlist.aIsParticipating[iLocalUserIndex] = TRUE;
            }
            else
            {
                // mark local user as not participating
                pVoipCommon->Connectionlist.aIsParticipating[iLocalUserIndex] = FALSE;
            }
        }
        else
        {
            NetPrintf(("voipps4: cannot %s a null user %s participating state (local user index %d)\n",
                (bActivate?"promote":"demote"), (bActivate?"to":"from"), iLocalUserIndex));
        }
    }
    
    return(iResult);
}

/*F********************************************************************************/
/*!
    \Function   _VoipThread

    \Description
        Main Voip thread that updates all active connections

    \Input *pArg    - argument

    \Output
        void *      - NULL

    \Version 01/18/2013 (mclouatre)
*/
/********************************************************************************F*/
static void _VoipThread(void *pArg)
{
    VoipRefT *pVoip = (VoipRefT *)pArg;
    uint32_t uProcessCt = 0, uTtsStatus;
    uint32_t uTime0, uTime1;
    int32_t iLocalUser, iSleep;

    // wait until we're started up
    while(pVoip->iThreadActive == 0)
    {
        NetConnSleep(VOIP_THREAD_SLEEP_DURATION);
    }

    // execute until we're killed
    while(pVoip->iThreadActive == 1)
    {
        uTime0 = NetTick();

        NetCritEnter(&pVoip->Common.ThreadCrit);
        NetCritEnter(&pVoip->Common.Connectionlist.NetCrit);

        // apply the changes if the first party mute list has been updated
        for (iLocalUser = 0; iLocalUser < VOIP_MAXLOCALUSERS; iLocalUser += 1)
        {
            if (VoipHeadsetStatus(pVoip->Common.pHeadset, 'fpmu', iLocalUser, NULL, 0))
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
        for (iLocalUser = 0; iLocalUser < VOIP_MAXLOCALUSERS; iLocalUser += 1)
        {
            VoipHeadsetControl(pVoip->Common.pHeadset, 'mute', iLocalUser, (pVoip->Common.uUserSendMask & (1 << iLocalUser)) == 0, NULL);
        }

        // update headset
        VoipHeadsetProcess(pVoip->Common.pHeadset, uProcessCt++);

        // update ttos status for all local users
        for (iLocalUser = 0, uTtsStatus = 0; iLocalUser < VOIP_MAXLOCALUSERS; iLocalUser += 1)
        {
            uTtsStatus |= VoipHeadsetStatus(pVoip->Common.pHeadset, 'ttos', iLocalUser, NULL, 0) << iLocalUser;
        }
        pVoip->Common.uTtsStatus = uTtsStatus;

        NetCritLeave(&pVoip->Common.ThreadCrit);

        uTime1 = NetTick();

        iSleep = VOIP_THREAD_SLEEP_DURATION - NetTickDiff(uTime1, uTime0);

        // wait for next tick
        if (iSleep >= 0)
        {
            NetConnSleep(iSleep);
        }
        else
        {
            NetPrintf(("voipps4: [WARNING] Overall voip thread update took %d ms\n", NetTickDiff(uTime1, uTime0)));
        }
    }

    // notify main thread we're done
    pVoip->iThreadActive = 0;
}

/*** Public Functions *************************************************************/


/*F********************************************************************************/
/*!
    \Function   VoipStartup

    \Description
        Prepare VoIP for use.

    \Input iMaxPeers    - maximum number of peers supported (up to VOIP_MAXCONNECT)
    \Input iData        - platform-specific - unused for ps4

    \Output
        VoipRefT        - state reference that is passed to all other functions

    \Version 01/16/2013 (mclouatre)
*/
/********************************************************************************F*/
VoipRefT *VoipStartup(int32_t iMaxPeers, int32_t iData)
{
    VoipRefT *pVoip;
    DirtyThreadConfigT ThreadConfig;
    int32_t iResult;

    // common startup
    if ((pVoip = VoipCommonStartup(iMaxPeers, sizeof(*pVoip), _VoipStatusCb, iData)) == NULL)
    {
        return(NULL);
    }

    VoipHeadsetControl(pVoip->Common.pHeadset, 'vpri', 0, 0, (void *)_VoipPs4_MostRestrictiveVoipContributors);

    /* configure thread parameters
       the priority calculation is a bit odd but gets us at the highest priority
       due to the EAThread implementation on PS4 */
    ds_memclr(&ThreadConfig, sizeof(ThreadConfig));
    ThreadConfig.pName = "VoIP";
    ThreadConfig.iAffinity = (_Voip_iThreadAffinity == -1) ? NetConnStatus('affn', 0, NULL, 0) : _Voip_iThreadAffinity;
    ThreadConfig.iPriority = SCE_KERNEL_PRIO_FIFO_DEFAULT - SCE_KERNEL_PRIO_FIFO_HIGHEST;

    // create worker thread
    if ((iResult = DirtyThreadCreate(_VoipThread, pVoip, &ThreadConfig)) == 0)
    {
        // tell thread to start executing
        pVoip->iThreadActive = 1;
    }
    else
    {
        NetPrintf(("voipps4: unable to create voip thread (err=%d)\n", iResult));
        pVoip->iThreadActive = 0;
        VoipShutdown(pVoip, 0);
        return(NULL);
    }

    //permission is handled by the voipheadsetps4 so bPrivileged is always TRUE
    pVoip->Common.bPrivileged = TRUE; 

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

    \Version 01/16/2013 (mclouatre)
*/
/********************************************************************************F*/
void VoipShutdown(VoipRefT *pVoip, uint32_t uShutdownFlags)
{
    // make sure we're really started up
    if (VoipGetRef() == NULL)
    {
        NetPrintf(("voipps4: module shutdown called when not in a started state\n"));
        return;
    }

    // tell thread to shut down and wait for shutdown confirmation (if running)
    if (pVoip->iThreadActive == 1)
    {
        for (pVoip->iThreadActive = 2; pVoip->iThreadActive != 0; )
        {
            NetPrintf(("voipps4: waiting for shutdown\n"));
            NetConnSleep(VOIP_THREAD_SLEEP_DURATION);
        }
    }

    // remove the shared user
    _VoipRegisterLocalSharedTalker(pVoip, FALSE);

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
    \Input bRegister        - true to register user, false to unregister user

    \Version 01/16/2013 (mclouatre)
*/
/********************************************************************************F*/
void VoipSetLocalUser(VoipRefT *pVoip, int32_t iLocalUserIndex, uint32_t bRegister)
{
    if ((iLocalUserIndex < VOIP_MAXLOCALUSERS) && (iLocalUserIndex >= 0))
    {
        NetCritEnter(&pVoip->Common.ThreadCrit);

        if (bRegister)
        {
            if (VOIP_NullUser(&pVoip->Common.Connectionlist.LocalUsers[iLocalUserIndex]))
            {
                VoipUserT voipUser;
                ds_memclr(&voipUser, sizeof(voipUser));

                voipUser.ePlatform = VOIP_PLATFORM_PS4;
                NetConnStatus('acid', iLocalUserIndex, &voipUser.AccountInfo.iAccountId, sizeof(voipUser.AccountInfo.iAccountId));
                NetConnStatus('peid', iLocalUserIndex, &voipUser.AccountInfo.iPersonaId, sizeof(voipUser.AccountInfo.iPersonaId));

                // register the local talker
                NetPrintf(("voipps4: registering local user %lld at index %d\n", voipUser.AccountInfo.iPersonaId, iLocalUserIndex));
                _VoipRegisterLocalTalker(pVoip, iLocalUserIndex, TRUE, &voipUser);
            }
            else
            {
                NetPrintf(("voipps4: warning -- ignoring attempt to register user index %d because user %lld is already registered at that index\n",
                    iLocalUserIndex, pVoip->Common.Connectionlist.LocalUsers[iLocalUserIndex].AccountInfo.iPersonaId));
            }
        }
        else
        {
            if (!VOIP_NullUser(&pVoip->Common.Connectionlist.LocalUsers[iLocalUserIndex]))
            {
                // if a participating user demote him first 
                if (pVoip->Common.Connectionlist.aIsParticipating[iLocalUserIndex] == TRUE)
                {
                    VoipCommonActivateLocalUser(&pVoip->Common, iLocalUserIndex, FALSE);
                }

                NetPrintf(("voipps4: unregistering local user %lld at index %d\n", pVoip->Common.Connectionlist.LocalUsers[iLocalUserIndex].AccountInfo.iPersonaId, iLocalUserIndex));
                _VoipRegisterLocalTalker(pVoip, iLocalUserIndex, FALSE, NULL);
            }
            else
            {
                NetPrintf(("voipps4: warning -- ignoring attempt to unregister local user at index %d because it is no yet registered\n", iLocalUserIndex));
            }
        }

        NetCritLeave(&pVoip->Common.ThreadCrit);
    }
    else
    {
        NetPrintf(("voipps4: VoipSetLocalUser() invoked with an invalid user index %d\n", iLocalUserIndex));
    }
}

/*F********************************************************************************/
/*!
    \Function   VoipCommonActivateLocalUser

    \Description
        Promote/demote specified local user to/from "participating" state

    \Input *pVoipCommon     - voip common state
    \Input iLocalUserIndex  - local user index
    \Input bActivate        - TRUE to Activate False to unActivate

    \Version 04/25/2013 (tcho)
*/
/********************************************************************************F*/
void VoipCommonActivateLocalUser(VoipCommonRefT *pVoipCommon, int32_t iLocalUserIndex, uint8_t bActivate)
{
    VoipConnectionlistT *pConnectionlist = &pVoipCommon->Connectionlist;

    if ((iLocalUserIndex < NETCONN_MAXLOCALUSERS) && (iLocalUserIndex >= 0))
    {
        NetCritEnter(&pVoipCommon->ThreadCrit);

        if (!VOIP_NullUser(&pConnectionlist->LocalUsers[iLocalUserIndex]))
        {
            if (bActivate != FALSE)
            {
                NetPrintf(("voipps4: promoting local user %lld to participating state\n", &pConnectionlist->LocalUsers[iLocalUserIndex].AccountInfo.iPersonaId));
                _VoipActivateLocalTalker(pVoipCommon, iLocalUserIndex, &pConnectionlist->LocalUsers[iLocalUserIndex], TRUE);

                // reapply playback muting config based on the channel configuration only if this is a participating user
                pVoipCommon->bApplyChannelConfig = TRUE;
            }
            else
            {
                NetPrintf(("voipps4: demoting local user %lld from participating state\n", &pConnectionlist->LocalUsers[iLocalUserIndex].AccountInfo.iPersonaId));
                _VoipActivateLocalTalker(pVoipCommon, iLocalUserIndex, &pConnectionlist->LocalUsers[iLocalUserIndex], FALSE);
            }

            // announce jip add or remove event on connection thater are already active
            VoipConnectionReliableBroadcastUser(pConnectionlist, iLocalUserIndex, bActivate);
        }
        else
        {
            NetPrintf(("voipps4: VoipCommonActivateLocalUser() failed because no local user registered at index %d\n", iLocalUserIndex));
        }

        NetCritLeave(&pVoipCommon->ThreadCrit);
    }
    else
    {
        NetPrintf(("voipps4: VoipCommonActivateLocalUser() invoked with an invalid user index %d\n", iLocalUserIndex));
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
            'vpri' - returns whether the specified local user (iValue) is a contributor to most restrictive voip privilege calculation
        \endverbatim

        Unhandled selectors are passed through to VoipCommonStatus(), and
        further to VoipHeadsetStatus() if unhandled there.

    \Version 01/16/2013 (mclouatre)
*/
/********************************************************************************F*/
int32_t VoipStatus(VoipRefT *pVoip, int32_t iSelect, int32_t iValue, void *pBuf, int32_t iBufSize)
{
    // returns whether the specified local user (iValue) is a contributor to most restrictive voip privilege calculation
    if (iSelect == 'vpri')
    {
        return(_VoipPs4_MostRestrictiveVoipContributors[iValue]);
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
             '+pbk' - enable voice playback for a given remote user (pValue is the remote user VoipUserT).
             '-pbk' - disable voice playback for a given remote user (pValue is the remote user VoipUserT).
             '+pri' - signal that local user shall be included when internally calculating most restrictive voip privileges
             '-pri' - signal that local user shall not be included when internally calculating most restrictive voip privileges
        \endverbatim

        Unhandled selectors are passed through to VoipCommonControl(), and
        further to VoipHeadsetControl() if unhandled there.

    \Version 01/16/2013 (mclouatre)
*/
/********************************************************************************F*/
int32_t VoipControl(VoipRefT *pVoip, int32_t iControl, int32_t iValue, void *pValue)
{
    if ('affn' == iControl)
    {
        _Voip_iThreadAffinity = iValue;
        return(0);
    }
    if (iControl == '+pbk')
    {
        return(VoipHeadsetControl(pVoip->Common.pHeadset, '+pbk', iValue, 0, (VoipUserT *)pValue));
    }
    if (iControl == '-pbk')
    {
        return(VoipHeadsetControl(pVoip->Common.pHeadset, '-pbk', iValue, 0, (VoipUserT *)pValue));
    }
    if (iControl == '+pri')
    {
        if (_VoipPs4_MostRestrictiveVoipContributors[iValue] == FALSE)
        {
            NetPrintf(("voipps4: local user at index %d is now an explicit contributor to most restrictive voip privilege determination\n", iValue));
            _VoipPs4_MostRestrictiveVoipContributors[iValue] = TRUE;
        }
        #if DIRTYCODE_LOGGING
        else
        {
            NetPrintf(("voipps4: VoipControl('+pri', %d) ignored because user is already an explicit contributor to most restrictive voip privilege determination\n", iValue));
        }
        #endif
        return(0);
    }
    if (iControl == '-pri')
    {
        if (_VoipPs4_MostRestrictiveVoipContributors[iValue] == TRUE)
        {
            NetPrintf(("voipps4: local user at index %d is no longer an explicit contributor to most restrictive voip privilege determination\n", iValue));
            _VoipPs4_MostRestrictiveVoipContributors[iValue] = FALSE;
        }
        #if DIRTYCODE_LOGGING
        else
        {
            NetPrintf(("voipps4: VoipControl('-pri', %d) ignored because user is already not explicitly contributing to most restrictive voip privilege determination\n", iValue));
        }
        #endif
        return(0);
    }

    return(VoipCommonControl(&pVoip->Common, iControl, iValue, pValue));
}
