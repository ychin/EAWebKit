/*H*************************************************************************************/
/*!
    \File dirtysessionmanagerps4.c

    \Description
        DirtySessionManagerps4 encapsulates use of the Sce's SessionInvitation API for 
        creating, modifying, inviting and joining.
    
    \Notes
        The Play Together feature is deprecated as of 7.5 SDK

    \Copyright
        Copyright (c) Electronic Arts 20013.  ALL RIGHTS RESERVED.

    \Version 1.0 03/26/2013 (cvienneau)  First Version
*/
/*************************************************************************************H*/


/*** Include files *********************************************************************/
#include <string.h>
#include "DirtySDK/dirtysock.h"
#include "DirtySDK/dirtylang.h"
#include "DirtySDK/dirtysock/dirtymem.h"
#include "DirtySDK/dirtysock/dirtyuser.h"
#include "DirtySDK/dirtysock/dirtysessionmanager.h"
#include "DirtySDK/dirtysock/ps4/dirtyeventmanagerps4.h"
#include "DirtySDK/dirtysock/ps4/dirtywebapips4.h"
#include "DirtySDK/dirtysock/dirtyerr.h"
#include "DirtySDK/dirtysock/netconn.h"
#include "DirtySDK/util/jsonparse.h"

#include <sdk_version.h>
#include <scetypes.h>
#include <net.h>
#include <libhttp.h>
#include <np.h>
#include <invitation_dialog.h>

#if SCE_ORBIS_SDK_VERSION < 0x07508001u
#include <np/np_play_together.h>
#endif

/*** Defines ***************************************************************************/
#define DIRTY_SESSION_DEFAULT_REQUEST_BUFF_SIZE         (12 * 1024)
#define DIRTY_SESSION_DEFAULT_RECIEVE_BUFF_SIZE         (24 * 1024)
#define DIRTY_SESSION_DEFAULT_SEND_INVITE_LIST_SIZE     (2 * 1024)
#define DIRTY_SESSION_MULTI_PART_SEPERATOR              "boundary_parameter"
#define DIRTY_SESSION_WEBAPI_GROUP_SESSION              "sdk:sessionInvitation"

#define DIRTY_SESSION_API_VERSION                               "/v1"
#define DIRTY_SESSION_WEBAPI_PATH_SESSION_CREATE                DIRTY_SESSION_API_VERSION"/sessions"                                // no params
#define DIRTY_SESSION_WEBAPI_PATH_SESSION_UPDATE                DIRTY_SESSION_API_VERSION"/sessions/%s"                             // {0}=sessionid
#define DIRTY_SESSION_WEBAPI_PATH_SESSION_GET_INVITE            DIRTY_SESSION_API_VERSION"/users/me/invitations/%s?fields=@default" // {0}=invitationId
#define DIRTY_SESSION_WEBAPI_PATH_SESSION_USED_INVITE           DIRTY_SESSION_API_VERSION"/users/me/invitations/%s"                 // {0}=invitationId
#define DIRTY_SESSION_WEBAPI_PATH_SESSION_INVITATIONS           DIRTY_SESSION_API_VERSION"/users/me/invitations?fields=@default,sessionId,expired,fromUser"  // no params
#define DIRTY_SESSION_WEBAPI_PATH_SESSION_MEMBER_OF             DIRTY_SESSION_API_VERSION"/users/me/sessions"                       // no params
#define DIRTY_SESSION_WEBAPI_PATH_SESSION_GET                   DIRTY_SESSION_API_VERSION"/sessions/%s"                             // {0}=sessionid
#define DIRTY_SESSION_WEBAPI_PATH_SESSION_GET_DATA              DIRTY_SESSION_API_VERSION"/sessions/%s/sessionData"                 // {0}=sessionid
#define DIRTY_SESSION_WEBAPI_PATH_SESSION_GET_CHANGEABLE_DATA   DIRTY_SESSION_API_VERSION"/sessions/%s/changeableSessionData"       // {0}=sessionid
#define DIRTY_SESSION_WEBAPI_PATH_SESSION_UPDATE_IMAGE          DIRTY_SESSION_API_VERSION"/sessions/%s/sessionImage"                // {0}=sessionid
#define DIRTY_SESSION_WEBAPI_PATH_SESSION_REMOVE_MEMBER         DIRTY_SESSION_API_VERSION"/sessions/%s/members/me"                  // {0}=sessionid 
#define DIRTY_SESSION_WEBAPI_PATH_SESSION_JOIN                  DIRTY_SESSION_API_VERSION"/sessions/%s/members"                     // {0}=sessionid
#define DIRTY_SESSION_WEBAPI_PATH_SESSION_SEND_INVITE           DIRTY_SESSION_API_VERSION"/sessions/%s/invitations"                 // {0}=sessionid

#define DIRTY_SESSION_STATUS_LENGTH 64                                         //refer SCE_TOOLKIT_NP_SESSION_STATUS_MAX_SIZE
#define DIRTY_SESSION_NAME_LENGTH 64                                           //refer to SCE_TOOLKIT_NP_SESSION_NAME_MAX_SIZE
#define DIRTY_SESSION_ID_LENGTH (SCE_NP_SESSION_ID_MAX_SIZE + 1)               //45 for data + 1 for the termination char
#define DIRTY_SESSION_INVITATION_ID_LENGTH (SCE_NP_INVITATION_ID_MAX_SIZE + 1) //60 for data + 1 for the termination char
#define DIRTY_SESSION_MAX_LANGUAGES (10)                                       //from sony doc
#define DIRTY_SESSION_MAX_LOCAL_CODE_LENGTH (8)                                //looks like 5 characters would actually do the job
#define DIRTY_SESSION_MAX_INVITATION_MEMBER_LIST_LENGTH (32)                   //number of npid's that can be populated into the invite dialog
#define DIRTY_SESSION_MAX_INVITATION_RECIEVED_LIST_LENGTH (10)                 //the maximum number of sessions invitations to store at once
#define DIRTY_SESSION_MAX_INVITATION_MESSAGE_LENGTH (512)                      //max length of invitation message from user
#define DIRTY_SESSION_PLAYER_NAME_LENGTH (SCE_NP_ONLINEID_MAX_LENGTH + 1)      //size of username in SceNpOnlineId


/*** Macros ****************************************************************************/


/*** Type Definitions ******************************************************************/

//!< data used to receive an an Invitation
typedef struct DirtySessionRecievedInvitationT
{
    //basic data
    char strSessionID[DIRTY_SESSION_ID_LENGTH];
    char strInvitationID[DIRTY_SESSION_INVITATION_ID_LENGTH];
    SceNpAccountId FromUser;
    //extended meta data
    char strMessage[DIRTY_SESSION_MAX_INVITATION_MESSAGE_LENGTH];
    uint8_t bExpired;
    uint8_t bUsedFlag;
    uint8_t bHasMetaData;
} DirtySessionRecievedInvitationT;

//!< several invitations
typedef struct DirtySessionRecievedInvitationListT
{
    DirtySessionRecievedInvitationT aInvitations[DIRTY_SESSION_MAX_INVITATION_RECIEVED_LIST_LENGTH];
    int32_t iNumInvitations;
    uint8_t bListUpToDate;
} DirtySessionRecievedInvitationListT;

//!< data attributed to each player
typedef struct DirtySessionManagerUserT
{
    uint8_t                 bSessionMember;
    DirtySessionRecievedInvitationListT RecievedInvitations;
} DirtySessionManagerUserT;

//!< possible states of the NP session
typedef enum DirtySessionmanagerStatusE
{
    DIRTY_SESSION_INVALID,
    DIRTY_SESSION_PENDING,
    DIRTY_SESSION_ACTIVE
} DirtySessionmanagerStatusE;

typedef struct DirtySessionManagerLocalizedElementT
{
    char strLanguageCode[DIRTY_SESSION_MAX_LOCAL_CODE_LENGTH];
    char strData[DIRTY_SESSION_NAME_LENGTH];
} DirtySessionManagerLocalizedElementT;

typedef struct DirtySessionManagerLocalizedListT
{
    DirtySessionManagerLocalizedElementT LocalizedElement[DIRTY_SESSION_MAX_LANGUAGES];
    int32_t iNumLanguages;
} DirtySessionManagerLocalizedListT;

//!< struct to mirror the fields of a np session
typedef struct DirtySessionManagerSessionInfoT
{
    DirtySessionManagerLocalizedListT LocalizedSessionStatus;
    DirtySessionManagerLocalizedListT LocalizedSessionNames;
    char strSessionStatus[DIRTY_SESSION_STATUS_LENGTH];
    char strSessionName[DIRTY_SESSION_NAME_LENGTH];
    char strSessionID[DIRTY_SESSION_ID_LENGTH];
    int32_t iMaxUsers;
    int32_t iBinaryDataSize;
    int32_t iChangeableBinaryDataSize;
    int32_t iImageDataSize;
    uint8_t bPrivate;
    uint8_t bClosed;
    uint8_t bSessionLock;
    void *pImageData;
    void *pBinaryData;
    DirtySessionManagerBinaryHeaderT binaryDataHeader;
    void *pChangeableBinaryData;
    DirtySessionManagerChangeableBinaryHeaderT changeableBinaryDataHeader;
}DirtySessionManagerSessionInfoT;

//!< data from a web request response
typedef struct DirtySessionManagerResponseT
{
    const char  *pRespBody;
    void        *pRequestData;
    int32_t     iRespBodyLength;
    uint32_t    iControlCode;
    int32_t     iUserIndex;
    int32_t     iStatusCode;
} DirtySessionManagerResponseT;

//!< data used to create in an Invitation
typedef struct DirtySessionInvitationT
{
    char strMessage[DIRTY_SESSION_MAX_INVITATION_MESSAGE_LENGTH];
    SceNpAccountId aAccountIds[DIRTY_SESSION_MAX_INVITATION_MEMBER_LIST_LENGTH];
    int32_t iMaxUsers;
    bool bEditList;
} DirtySessionInvitationT;

//! internal module state
struct DirtySessionManagerRefT
{
    void                *pMemGroupUserData;     //!< user data associated with mem group
    int32_t             iMemGroup;              //!< module mem group id

    uint8_t             bInitEvents;               //!< determins if webapi events have been setup, they can only be setup after NetConnConnect
    uint8_t             bInvDialog;                //!< indicates if the invite dialog is open
    uint8_t             bInvDialogUserManagedInit; //!< indicates if the sceInvitationDialogInitialize/Term, is managed by the user.
    uint8_t             bPendingNameSet;           //!< indicates if session name is set already
    uint8_t             bPendingStatusSet;         //!< indicates if session status is set already
    uint8_t             bServerDrivenModeEnabled;  //!< indicates if client session operation should be ignored because sessions are now driven by a server
    uint8_t             bPTHostEvent;              //!< indicates if a client has received a Play Together Host event
    uint8_t             _pad;                      //!< padding

    DirtySessionInvitationT InvitationInfo;     //!< data used to create in an Invitation
    char strSelectedSessionID[DIRTY_SESSION_ID_LENGTH];   //!< the session id the user has selected to join
    char strSelectedInviteID[DIRTY_SESSION_INVITATION_ID_LENGTH];    //!< the invitation id the user has selected to join
    SceUserServiceUserId  lastAcceptUserId;     //!< last user that accepted the invite

    DirtyWebApiRefT     *pWebApi; 
    int32_t             iRequestBuffSize;               //!< maximum request size
    NetCritT            crit;                           //!< sychronize shared data between the threads
    DirtySessionManagerResponseT    ResponseData;       //!< data shared between threads contains current web response info
    int32_t                         iActiveRequests;    //!< data shared between threads contains number of outstanding web requests
    int32_t                         iLastStatusCode;    //!< status code of last handled request

    DirtySessionManagerSessionInfoT PendingSession;
    DirtySessionManagerSessionInfoT ActiveSession;
    DirtySessionmanagerStatusE      eSessionStatus;     //!< 3 values; no session, pending, active session
    DirtySessionManagerUserT        aSessionUsers[NETCONN_MAXLOCALUSERS];     //!< state for each user

    #if SCE_ORBIS_SDK_VERSION < 0x07508001u
    int32_t iPTInviteListLen;
    SceUserServiceUserId launchSuid;
    SceNpPlayTogetherInvitee aPTInviteList[SCE_NP_PLAY_TOGETHER_MAX_INVITEE_LIST_NUM];
    #endif
};

//! data passed to the webapi module that we can retrieve in its callback
typedef struct DirtySessionManagerCallbackT
{
    DirtySessionManagerRefT     *pDirtySessionManagerRef;
    void                        *pRequestData;
    uint32_t                    iControlCode;
} DirtySessionManagerCallbackT;


static int32_t _DirtySessionManagerSetupWebRequest(DirtySessionManagerRefT *pRef, int32_t iControlCode, int32_t iUserIndex, int32_t iValue2, const void *pValue, void **pRequestData, char *pApiGroup, char *pApiPath, int32_t iStrSize, SceNpWebApiContentParameter *pContentParameter, SceNpWebApiHttpMethod *pHttpMethod, char *pSendBuff, int32_t iBuffSize);
static int32_t _DirtySessionManagerWebRequestResult(DirtySessionManagerRefT *pRef, int32_t iControlCode, int32_t iUserIndex, void *pRequestData, int32_t iStatus, const char *pReadBuff, int32_t iBuffSize);

/*** Private Functions *****************************************************************/

/*F********************************************************************************/
/*!
    \Function _DirtySessionManagerOnSystemEvent

    \Description
        Handler for when a user accepts an invitation with the system ui.

    \Input *pUserData  - DirtySessionManagerRefT pointer
    \Input *pSystemServiceEvent - the event from the console (session invite acceptance)

    \Version 07/23/2013 (cvienneau)
*/
/********************************************************************************F*/
static void _DirtySessionManagerOnSystemEvent(void *pUserData, const SceSystemServiceEvent *pSystemServiceEvent)
{
    if (pSystemServiceEvent && pUserData)
    {
        DirtySessionManagerRefT *pRef = (DirtySessionManagerRefT*)pUserData;

        if (pSystemServiceEvent->eventType == SCE_SYSTEM_SERVICE_EVENT_SESSION_INVITATION)
        {
            int32_t iUserIndex;
            SceNpSessionInvitationEventParam *pSessionParam =  (SceNpSessionInvitationEventParam *)(pSystemServiceEvent->data.param);
        
            NetPrintf(("dirtysessionmanagerps4: [%p] detected session join event, user would like to join %s.\n", pRef, pSessionParam->sessionId.data));
        
            pRef->lastAcceptUserId = pSessionParam->userId;

            /* When the user selects to join an invite from system software it may be that our internal list of 
               invites is not up to date.  Typically we mark that list as being not up to date initially (on boot), 
               and when we receive webapi event indicating a new invite has arrived.  However the webapi event can be
               missed if the console is put into REST mode.  Since the list is marked as not up to date, any code attempting 
               to join the invite will first fetch the list, such that we have all the appropriate meta data.*/
            for (iUserIndex = 0; iUserIndex < NETCONN_MAXLOCALUSERS; ++iUserIndex)
            {
                if (pSessionParam->userId == NetConnStatus('suid', iUserIndex, NULL, 0))
                {
                    break;
                }
            }
            pRef->aSessionUsers[iUserIndex].RecievedInvitations.bListUpToDate = FALSE;

            // save strSelectedSessionID, and strSelectedInviteID if available, these can be used later to actually do the join 
            ds_strnzcpy(pRef->strSelectedSessionID, pSessionParam->sessionId.data, sizeof(pRef->strSelectedSessionID));
            if (pSessionParam->flag == SCE_NP_SESSION_INVITATION_EVENT_FLAG_INVITATION)
            {
                ds_strnzcpy(pRef->strSelectedInviteID, pSessionParam->invitationId.data, sizeof(pRef->strSelectedInviteID));
            }
            else
            {
                pRef->strSelectedInviteID[0] = '\0';
            }
        }
        #if SCE_ORBIS_SDK_VERSION < 0x07508001u
        else if (pSystemServiceEvent->eventType == SCE_SYSTEM_SERVICE_EVENT_PLAY_TOGETHER_HOST_A)
        {
            SceNpPlayTogetherHostEventParamA *pPlayTogetherHostParam = (SceNpPlayTogetherHostEventParamA *)(pSystemServiceEvent->data.param);
            int32_t iIndex;
            int32_t iUserIndex;

            // look up the ds user index 
            for (iUserIndex = 0; iUserIndex < NETCONN_MAXLOCALUSERS; ++iUserIndex)
            {
                if (pPlayTogetherHostParam->userId == NetConnStatus('suid', iUserIndex, NULL, 0))
                {
                    break;
                }
            }

            NetPrintf(("dirtysessionmanagerps4: [%p] detected play together event, user would like to invite %i players to the game.\n", pRef, pPlayTogetherHostParam->inviteeListLen));

            pRef->iPTInviteListLen = pPlayTogetherHostParam->inviteeListLen;
            pRef->launchSuid = NetConnStatus('suid', iUserIndex, NULL, 0);

            for (iIndex = 0; iIndex < SCE_NP_PLAY_TOGETHER_MAX_INVITEE_LIST_NUM; ++iIndex)
            {
                if ( iIndex < pRef->iPTInviteListLen)
                {
                    ds_memcpy_s(&pRef->aPTInviteList[iIndex], sizeof(SceNpPlayTogetherInvitee), &pPlayTogetherHostParam->inviteeList[iIndex],sizeof(SceNpPlayTogetherInvitee));
                }
                else
                {
                    ds_memclr(&pRef->aPTInviteList[iIndex], sizeof(SceNpPlayTogetherInvitee));
                }
            }

            pRef->bPTHostEvent = TRUE;
        }
        #endif
    }
}

/*F*************************************************************************************/
/*!
    \Function    _DirtySessionManagerOnWebApiSessionEvent

    \Description
        Callback for when session/invite web api events happen.

    \Input userCtxId        - user web api context
    \Input callbackId       - callback id generated at registration
    \Input pNpServiceName   - NP web service generating the event
    \Input npServiceLabel   - more detail on the web service
    \Input pTo              - contains the onlineId of target user
    \Input pFrom            - contains the onlineId of source user
    \Input pDataType        - event type
    \Input pData            - any data coming with the event
    \Input dataLen          - length of data with the event
    \Input pUserArg         - pointer to DirtySessionManagerRefT

    \Version 07/20/2013 (cvienneau)
*/
/*************************************************************************************F*/
static void _DirtySessionManagerOnWebApiSessionEvent(int32_t userCtxId, int32_t callbackId, const char *pNpServiceName, SceNpServiceLabel npServiceLabel, const SceNpPeerAddressA *pTo, const SceNpPeerAddressA *pFrom, const SceNpWebApiPushEventDataType *pDataType, const char *pData, size_t dataLen, void *pUserArg)
{
    if (pUserArg != NULL)
    {
        DirtySessionManagerRefT *pRef = (DirtySessionManagerRefT*)pUserArg;
        uint8_t bFound = FALSE;
        NetPrintf(("dirtysessionmanagerps4: [%p] detected invitation event for account id %llu.\n", pRef, pTo->accountId));

        //find the user index of the person who received the invite
        for (int32_t iUserIndex = 0; iUserIndex < NETCONN_MAXLOCALUSERS; iUserIndex++)
        {
            SceNpAccountId accountId;
            int32_t iRet;
    
            //compare the accountIds to match
            if ((iRet = NetConnStatus('acct', iUserIndex, &accountId, sizeof(accountId))) >= 0) 
            {
                if (accountId == pTo->accountId)
                {
                    NetPrintf(("dirtysessionmanagerps4: [%p] Invite list for player %d:%llu, has changed.\n", pRef, iUserIndex, accountId));

                    //we found the right player, mark their list of invites as beign not up to date
                    //the user will have to decide to fetch it if they want with DirtySessionManagerControl(pRef, 'ginv', iUserIndex, 0, NULL);
                    pRef->aSessionUsers[iUserIndex].RecievedInvitations.bListUpToDate = FALSE;
                    bFound = TRUE;
                    break;
                }
            }
        }

        if (bFound == FALSE)
        {
            NetPrintf(("dirtysessionmanagerps4: [%p] unable to associate invitation to logged in users.\n", pRef));
        }
    }
}

/*F*************************************************************************************/
/*!
    \Function    _DirtySessionManagerInitDialog

    \Description
        Prepares the system to display the invitation dialog

    \Input *pRef                - pointer to module state

    \Output
        int32_t                 - >=0 success, < 0 failure

    \Version 06/03/2013 (cvienneau)
*/
/*************************************************************************************F*/
static int32_t _DirtySessionManagerInitDialog(DirtySessionManagerRefT *pRef)
{
    int32_t iRet;

    //prepare the ability to show the invitation/receive dialog
    if ((iRet = sceCommonDialogInitialize()) != SCE_OK)
    {
        if (iRet != SCE_COMMON_DIALOG_ERROR_ALREADY_SYSTEM_INITIALIZED)
        {
            NetPrintf(("dirtysessionmanagerps4: [%p] sceCommonDialogInitialize() failed, error = %s\n", pRef, DirtyErrGetName(iRet)));
            return(iRet);
        }
    }

    if (pRef->bInvDialogUserManagedInit == FALSE)
    {
        if ((iRet = sceInvitationDialogInitialize()) != SCE_OK)
        {
            if (iRet != SCE_COMMON_DIALOG_ERROR_ALREADY_SYSTEM_INITIALIZED)
            {
                NetPrintf(("dirtysessionmanagerps4: [%p] sceInvitationDialogInitialize() failed, error = %s\n", pRef, DirtyErrGetName(iRet)));
                return(iRet);
            }
        }
    }

    return(1);
}

/*F*************************************************************************************/
/*!
    \Function    _DirtySessionManagerCanShowDialog

    \Description
        Prepares the system to display the invitation dialog, and validates we are in a state to show it

    \Input *pRef                - pointer to module state

    \Output
        int32_t                 - >=0 success, < 0 failure

    \Version 06/03/2013 (cvienneau)
*/
/*************************************************************************************F*/
static int32_t _DirtySessionManagerCanShowDialog(DirtySessionManagerRefT *pRef)
{
    if(pRef->bInvDialog)
    {
        NetPrintf(("dirtysessionmanagerps4: [%p] Can't open invite dialog, we have already opened it.\n", pRef));
        return(-1);
    }

    if(sceCommonDialogIsUsed() && (pRef->bInvDialogUserManagedInit == FALSE))
    {
        NetPrintf(("dirtysessionmanagerps4: [%p] Can't open invite dialog, someone else has a dialog open.\n", pRef));
        return(-1);
    }

    return(_DirtySessionManagerInitDialog(pRef));
}

/*F*************************************************************************************/
/*!
    \Function    _DirtySessionManagerShowInviteDialog

    \Description
        Displays the sony session invitation dialog

    \Input *pRef                - pointer to module state
    \Input iUserIndex           - index of the user sending the invite

    \Output
        int32_t                 - >=0 success, < 0 failure

    \Version 06/03/2013 (cvienneau)
*/
/*************************************************************************************F*/
static int32_t _DirtySessionManagerShowInviteDialog(DirtySessionManagerRefT *pRef, int32_t iUserIndex)
{
    SceInvitationDialogParamA       dialogParam;
    SceInvitationDialogDataParamA   dataParam;
    SceNpSessionId sessionId;
    int32_t iRet = 0;

    if (pRef->eSessionStatus == DIRTY_SESSION_ACTIVE)
    {
        if ((iRet =_DirtySessionManagerCanShowDialog(pRef)) >= 0)
        {

            //prepare dialog paramerts
            ds_memclr(&dataParam, sizeof(SceInvitationDialogDataParamA));
            ds_memclr(&sessionId, sizeof(sessionId));

            //set the message 
            dataParam.SendInfo.userMessage = pRef->InvitationInfo.strMessage;
    
            //session id
            strncpy(sessionId.data, pRef->ActiveSession.strSessionID, sizeof(dataParam.SendInfo.sessionId->data));
            dataParam.SendInfo.sessionId = &sessionId;

            //users
            if (pRef->InvitationInfo.bEditList)
            {
                dataParam.SendInfo.addressParam.addressType = SCE_INVITATION_DIALOG_ADDRESS_TYPE_USERENABLE;
                dataParam.SendInfo.addressParam.addressInfo.UserSelectEnableAddress.userMaxCount = pRef->InvitationInfo.iMaxUsers;
            }
            else
            {
                //count the number of account ids that have been set
                int32_t iAccountCount = 0;
                for (int32_t i = 0; i < DIRTY_SESSION_MAX_INVITATION_MEMBER_LIST_LENGTH; i++)
                {
                    if (pRef->InvitationInfo.aAccountIds[i] == 0)
                    {
                        break;
                    }   
                    iAccountCount++;
                }

                dataParam.SendInfo.addressParam.addressType = SCE_INVITATION_DIALOG_ADDRESS_TYPE_USERDISABLE;   //todo can this be enabled?
                dataParam.SendInfo.addressParam.addressInfo.UserSelectDisableAddress.accountIds = pRef->InvitationInfo.aAccountIds;
                dataParam.SendInfo.addressParam.addressInfo.UserSelectDisableAddress.accountIdsCount = iAccountCount;
            }
    

            // open dialog to send invite
            sceInvitationDialogParamInitializeA( &dialogParam );
            dialogParam.mode = SCE_INVITATION_DIALOG_MODE_SEND;
            dialogParam.callbackArg = pRef;
            dialogParam.dataParam = &dataParam;    
            dialogParam.userId = NetConnStatus('suid', iUserIndex, NULL, 0);
            NetCritEnter(&pRef->crit);
            if ((iRet = sceInvitationDialogOpenA( &dialogParam )) == SCE_OK)
            {
                SceCommonDialogStatus status = sceInvitationDialogUpdateStatus();

                if (status == SCE_COMMON_DIALOG_STATUS_RUNNING)
                {
                    pRef->bInvDialog = TRUE;
                }
                else 
                {
                    NetPrintf(("dirtysessionmanagerps4: [%p] sceInvitationDialogOpen() unexpectd status %d\n", pRef, status));
                }
            }
            else
            {
                NetPrintf(("dirtysessionmanagerps4: [%p] sceInvitationDialogOpen() failed, error = %s\n", pRef, DirtyErrGetName(iRet)));
            }
            NetCritLeave(&pRef->crit);
        } //_DirtySessionManagerCanShowDialog
    }
    else
    {
        NetPrintf(("dirtysessionmanagerps4: [%p] An active session is required to send a session invite.\n", pRef));
        iRet = -1;
    }   
    return (iRet);
}


/*F*************************************************************************************/
/*!
    \Function    _DirtySessionManagerShowReceiveDialog

    \Description
        Displays the sony session invitation dialog

    \Input *pRef                - pointer to module state
    \Input iUserIndex           - index of the user sending the invite

    \Output
        int32_t                 - >=0 success, < 0 failure

    \Version 06/03/2013 (cvienneau)
*/
/*************************************************************************************F*/
static int32_t _DirtySessionManagerShowReceiveDialog(DirtySessionManagerRefT *pRef, int32_t iUserIndex)
{
    SceInvitationDialogParamA       dialogParam;
    SceInvitationDialogDataParamA   dataParam;
    int32_t iRet = 0;

    if ((iRet =_DirtySessionManagerCanShowDialog(pRef)) >= 0)
    {
        //prepare dialog paramerts
        ds_memclr(&dataParam, sizeof(SceInvitationDialogDataParamA));

        // open dialog to receive invite
        sceInvitationDialogParamInitializeA( &dialogParam );
        dialogParam.mode = SCE_INVITATION_DIALOG_MODE_RECV;
        dialogParam.callbackArg = pRef;
        dialogParam.dataParam = &dataParam;    
        dialogParam.userId = NetConnStatus('suid', iUserIndex, NULL, 0);
        NetCritEnter(&pRef->crit);
        if ((iRet = sceInvitationDialogOpenA( &dialogParam )) == SCE_OK)
        {
            SceCommonDialogStatus status = sceInvitationDialogUpdateStatus();

            if (status == SCE_COMMON_DIALOG_STATUS_RUNNING)
            {
                pRef->bInvDialog = TRUE;
            }
            else 
            {
                NetPrintf(("dirtysessionmanagerps4: [%p] sceInvitationDialogOpen() unexpectd status %d\n", pRef, status));
            }
        }
        else
        {
            NetPrintf(("dirtysessionmanagerps4: [%p] sceInvitationDialogOpen() failed, error = %s\n", pRef, DirtyErrGetName(iRet)));
        }
        NetCritLeave(&pRef->crit);
    } //_DirtySessionManagerCanShowDialog

    return (iRet);
}

/*F*************************************************************************************/
/*!
    \Function    _DirtySessionManagerGetSonyLanguageCode

    \Description
        Stores the binary data in the image field

    \Input *pRef                - pointer to module state
    \Input uDirtyLocal          - local as defined by dirtylang.h

    \Output 
        char *              - a sony language code, NULL on error

    \Version 06/03/2013 (cvienneau)
*/
/*************************************************************************************F*/
static const char * _DirtySessionManagerGetSonyLanguageCode(DirtySessionManagerRefT *pRef, uint32_t uDirtyLocal)
{
    uint16_t uLanguage = LOBBYAPI_LocalizerTokenGetLanguage(uDirtyLocal);
    uint16_t uCountry = LOBBYAPI_LocalizerTokenGetCountry(uDirtyLocal);
    const char *pSonyLanguage = NULL;

    if (uLanguage == LOBBYAPI_LANGUAGE_ENGLISH && uCountry == LOBBYAPI_COUNTRY_UNITED_KINGDOM)
    {
        pSonyLanguage = "en-GB";
    }
    else if (uLanguage == LOBBYAPI_LANGUAGE_PORTUGUESE && uCountry == LOBBYAPI_COUNTRY_BRAZIL)
    {
        pSonyLanguage = "pt-BR";
    }
    else if (uLanguage == LOBBYAPI_LANGUAGE_CHINESE)
    {
        pSonyLanguage = "zh-CN";    //todo how do we tell the difference between chinese traditional "zh-TW" and chinese simplified "zh-CN"
    }
    else if (uLanguage == LOBBYAPI_LANGUAGE_JAPANESE)
    {
        pSonyLanguage = "ja";
    }
    else if (uLanguage == LOBBYAPI_LANGUAGE_ENGLISH)
    {
        pSonyLanguage = "en";
    }
    else if (uLanguage == LOBBYAPI_LANGUAGE_FRENCH)
    {
        pSonyLanguage = "fr";
    }
    else if (uLanguage == LOBBYAPI_LANGUAGE_SPANISH)
    {
        pSonyLanguage = "es";
    }
    else if (uLanguage == LOBBYAPI_LANGUAGE_GERMAN)
    {
        pSonyLanguage = "de";
    }
    else if (uLanguage == LOBBYAPI_LANGUAGE_ITALIAN)
    {
        pSonyLanguage = "it";
    }
    else if (uLanguage == LOBBYAPI_LANGUAGE_DUTCH)
    {
        pSonyLanguage = "nl";
    }
    else if (uLanguage == LOBBYAPI_LANGUAGE_PORTUGUESE)
    {
        pSonyLanguage = "pt";
    }
    else if (uLanguage == LOBBYAPI_LANGUAGE_RUSSIAN)
    {
        pSonyLanguage = "ru";
    }
    else if (uLanguage == LOBBYAPI_LANGUAGE_KOREAN)
    {
        pSonyLanguage = "ko";
    }
    else if (uLanguage == LOBBYAPI_LANGUAGE_FINNISH)
    {
        pSonyLanguage = "fi";
    }
    else if (uLanguage == LOBBYAPI_LANGUAGE_SWEDISH)
    {
        pSonyLanguage = "sv";
    }
    else if (uLanguage == LOBBYAPI_LANGUAGE_DANISH)
    {
        pSonyLanguage = "da";
    }
    else if (uLanguage == LOBBYAPI_LANGUAGE_NORWEGIAN)
    {
        pSonyLanguage = "no";
    }
    else if (uLanguage == LOBBYAPI_LANGUAGE_POLISH)
    {
        pSonyLanguage = "pl";
    }
    else if (uLanguage == LOBBYAPI_LANGUAGE_TURKISH)
    {
        pSonyLanguage = "tr";
    }

    if (pSonyLanguage == NULL)
    {
        NetPrintf(("dirtysessionmanagerps4: [%p] _DirtySessionManagerGetSonyLanguageCode, Error converting %C to a valid sony laguage code\n", pRef, uDirtyLocal));
    }

    return(pSonyLanguage);
}

/*F*************************************************************************************/
/*!
    \Function    _DirtySessionManagerSetSessionString

    \Description
        Sets the SessionName or SessionStatus field, localized if local is provided.

    \Input *pRef                - pointer to module state
    \Input *strData             - string data
    \Input uDirtyLocal          - local as defined by dirtylang.h
    \Input bName                - true=set name field, false=set status field

    \Version 04/03/2013 (cvienneau)
*/
/*************************************************************************************F*/
static int32_t _DirtySessionManagerSetSessionString(DirtySessionManagerRefT *pRef, const char * strData, uint32_t uDirtyLocal, uint8_t bName)
{
    int32_t iRet = -1;
    //if we aren't doing localization its simple just write the string into the field
    if (uDirtyLocal == 0)
    {
        if (bName)
        {
            pRef->bPendingNameSet = TRUE;
            ds_strnzcpy(pRef->PendingSession.strSessionName, strData, DIRTY_SESSION_NAME_LENGTH);
        }
        else
        {
            pRef->bPendingStatusSet = TRUE;
            ds_strnzcpy(pRef->PendingSession.strSessionStatus, strData, DIRTY_SESSION_STATUS_LENGTH);
        }
        iRet = 1;
    }
    else
    {
        const char * pLanguageCode = _DirtySessionManagerGetSonyLanguageCode(pRef, uDirtyLocal);
        if (pLanguageCode != NULL)
        {
            //get the right collection
            DirtySessionManagerLocalizedListT *pList;
            DirtySessionManagerLocalizedElementT *pElement = NULL;
            if (bName)
            {
                pList = &pRef->PendingSession.LocalizedSessionNames;
            }
            else
            {
                pList = &pRef->PendingSession.LocalizedSessionStatus;
            }

            //see if there is already a field for this language code, or a blank element to add it to
            for (int32_t i = 0; i < DIRTY_SESSION_MAX_LANGUAGES; i++)
            {
                if (ds_stricmp(pList->LocalizedElement[i].strLanguageCode, pLanguageCode) == 0)
                {
                    pElement = &pList->LocalizedElement[i];
                    break;
                }
                if  (ds_stricmp(pList->LocalizedElement[i].strLanguageCode, "") == 0)
                {
                    pElement = &pList->LocalizedElement[i];
                    pList->iNumLanguages++;
                    break;
                }

            }

            //add the data to the list
            if (pElement != NULL)
            {
                ds_strnzcpy(pElement->strLanguageCode, pLanguageCode, DIRTY_SESSION_MAX_LOCAL_CODE_LENGTH);
                ds_strnzcpy(pElement->strData, strData, DIRTY_SESSION_NAME_LENGTH);
                iRet = 1;
            }
            else
            {
                NetPrintf(("dirtysessionmanagerps4: [%p] _DirtySessionManagerSetSessionString, Could not get a slot for localized session data; iNumLanguages=%d.\n", pRef, pList->iNumLanguages));
            }
        }
    }
    return(iRet);
}

/*F*************************************************************************************/
/*!
    \Function    _DirtySessionManagerGetLocalizedJson

    \Description
        Serializes the localized session data.

    \Input *pRef                - pointer to module state
    \Input *pBuff               - buffer to write data to
    \Input iBuffSize            - sizeof pBuff
    \Input bName                - true=set name field, false=set status field

    \Output
        char*                   - pBuff with data written to it

    \Version 04/03/2013 (cvienneau)
*/
/*************************************************************************************F*/
static char * _DirtySessionManagerGetLocalizedJson(DirtySessionManagerRefT *pRef, char *pBuff, int32_t iBuffSize, uint8_t bName)
{
    int32_t iLength = 0;
    pBuff[0] = 0;
    DirtySessionManagerLocalizedListT *pList;
    const char * strArrayName = "  \"localizedSessionNames\": [\r\n";
    const char * strDataName = "      \"sessionName\": \"%s\"\r\n";
    if (bName)
    {
        pList = &pRef->PendingSession.LocalizedSessionNames;
    }
    else
    {
        pList = &pRef->PendingSession.LocalizedSessionStatus;
        strArrayName = "  \"localizedSessionStatus\": [\r\n";
        strDataName = "      \"sessionStatus\": \"%s\"\r\n";
    }

    if (pList->iNumLanguages > 0)
    {
        iLength += ds_snzprintf(pBuff + iLength, iBuffSize - iLength, strArrayName);
        for (int32_t i = 0; i < pList->iNumLanguages; i++)
        {
            iLength += ds_snzprintf(pBuff + iLength, iBuffSize - iLength, "    {\r\n");
            iLength += ds_snzprintf(pBuff + iLength, iBuffSize - iLength, "      \"npLanguage\": \"%s\",\r\n", pList->LocalizedElement[i].strLanguageCode);
            iLength += ds_snzprintf(pBuff + iLength, iBuffSize - iLength, strDataName, pList->LocalizedElement[i].strData);
            //last element doesn't need a comma
            if (i != (pList->iNumLanguages -1))
            {
                iLength += ds_snzprintf(pBuff + iLength, iBuffSize - iLength, "    },\r\n");
            }
            else
            {
                 iLength += ds_snzprintf(pBuff + iLength, iBuffSize - iLength, "    }\r\n");
           }
        }
        iLength += ds_snzprintf(pBuff + iLength, iBuffSize - iLength, "  ],\r\n");

    }
    return(pBuff);
}

/*F*************************************************************************************/
/*!
    \Function    _DirtySessionManagerSetSessionImage

    \Description
        Stores the binary data in the image field

    \Input *pRef        - pointer to module state
    \Input *pSession    - pointer to session being manipulated
    \Input *pImageData  - pointer to the incomming data
    \Input iDataSize    - number of bytes in pImageData

    \Version 04/03/2013 (cvienneau)
*/
/*************************************************************************************F*/
static void _DirtySessionManagerSetSessionImage(DirtySessionManagerRefT *pRef, DirtySessionManagerSessionInfoT *pSession, const void *pImageData, int32_t iDataSize)
{
    if (pSession->pImageData != pImageData)
    {
        //free any existing data if it exists
        if (pSession->pImageData != NULL)
        {
            DirtyMemFree(pSession->pImageData, DIRTYSESSMGR_MEMID, pRef->iMemGroup, pRef->pMemGroupUserData);
            pSession->pImageData = NULL;
            pSession->iImageDataSize = 0;
        }

        //allocate and copy data, if provided
        if (pImageData != NULL)
        {
            if ((pSession->pImageData = DirtyMemAlloc(iDataSize, DIRTYSESSMGR_MEMID, pRef->iMemGroup, pRef->pMemGroupUserData)) != NULL)
            {
                ds_memcpy(pSession->pImageData, pImageData, iDataSize);
                pSession->iImageDataSize = iDataSize;
            }
            else
            {
                NetPrintf(("dirtysessionmanagerps4: [%p] failed to allocate storage for image data.\n", pRef));
            }
        }
    }
}

/*F*************************************************************************************/
/*!
    \Function    _DirtySessionManagerSetSessionData

    \Description
        Stores the binary data in the data field

    \Input *pRef         - pointer to module state
    \Input *pSession     - pointer to session being manipulated
    \Input *pSessionData - pointer to the incomming data
    \Input iDataSize     - number of bytes in pImageData

    \Version 04/03/2013 (cvienneau)
*/
/*************************************************************************************F*/
static void _DirtySessionManagerSetSessionData(DirtySessionManagerRefT *pRef, DirtySessionManagerSessionInfoT *pSession, const void *pSessionData, int32_t iDataSize)
{
    if (pSession->pBinaryData != pSessionData)
    {
        //free any existing data if it exists
        if (pSession->pBinaryData != NULL)
        {
            DirtyMemFree(pSession->pBinaryData, DIRTYSESSMGR_MEMID, pRef->iMemGroup, pRef->pMemGroupUserData);
            pSession->pBinaryData = NULL;
            pSession->iBinaryDataSize = 0;
        }

        //allocate and copy data, if provided
        if (pSessionData != NULL)
        {
            if ((pSession->pBinaryData = DirtyMemAlloc(iDataSize, DIRTYSESSMGR_MEMID, pRef->iMemGroup, pRef->pMemGroupUserData)) != NULL)
            {
                ds_memcpy(pSession->pBinaryData, pSessionData, iDataSize);
                pSession->iBinaryDataSize = iDataSize;
            }
            else
            {
                NetPrintf(("dirtysessionmanagerps4: [%p] failed to allocate storage for binary session data.\n", pRef));
            }
        }
    }
}

/*F*************************************************************************************/
/*!
    \Function    _DirtySessionManagerSetChangeableSessionData

    \Description
        Stores the changeable binary data in the data field

    \Input *pRef                   - pointer to module state
    \Input *pSession               - pointer to session being manipulated
    \Input *pChangeableSessionData - pointer to the incomming data
    \Input iChangeableDataSize     - number of bytes in pImageData

    \Version 08/19/2015 (mclouatre)
*/
/*************************************************************************************F*/
static void _DirtySessionManagerSetChangeableSessionData(DirtySessionManagerRefT *pRef, DirtySessionManagerSessionInfoT *pSession, const void *pChangeableSessionData, int32_t iChangeableDataSize)
{
    if (pSession->pChangeableBinaryData != pChangeableSessionData)
    {
        // free any existing data if it exists
        if (pSession->pChangeableBinaryData != NULL)
        {
            DirtyMemFree(pSession->pChangeableBinaryData, DIRTYSESSMGR_MEMID, pRef->iMemGroup, pRef->pMemGroupUserData);
            pSession->pChangeableBinaryData = NULL;
            pSession->iChangeableBinaryDataSize = 0;
        }

        // allocate and copy data, if provided
        if (pChangeableSessionData != NULL)
        {
            if ((pSession->pChangeableBinaryData = DirtyMemAlloc(iChangeableDataSize, DIRTYSESSMGR_MEMID, pRef->iMemGroup, pRef->pMemGroupUserData)) != NULL)
            {
                ds_memcpy(pSession->pChangeableBinaryData, pChangeableSessionData, iChangeableDataSize);
                pSession->iChangeableBinaryDataSize = iChangeableDataSize;
            }
            else
            {
                NetPrintf(("dirtysessionmanagerps4: [%p] failed to allocate storage for changeable binary session data.\n", pRef));
            }
        }
    }
}


/*F*************************************************************************************/
/*!
    \Function    _DirtySessionManagerCountSessionMembers

    \Description
        Returns the number of local users currently in the active session

    \Input *pRef        - pointer to module state

    \Output
        int32_t         - 0-NETCONN_MAXLOCALUSERS, number of local users currently in the active session

    \Version 08/25/2014 (cvienneau)
*/
/*************************************************************************************F*/
static int32_t _DirtySessionManagerCountSessionMembers(DirtySessionManagerRefT *pRef)
{
    int32_t iMemberCount = 0;
    for (int32_t iIndex = 0; iIndex < NETCONN_MAXLOCALUSERS; iIndex++)
    {
        if (pRef->aSessionUsers[iIndex].bSessionMember == TRUE)
        {
            iMemberCount++;
        }
    }
    return iMemberCount;
}

/*F*************************************************************************************/
/*!
    \Function    _DirtySessionManagerCleanupSession

    \Description
        Clear state of a session no local users is a member of anymore

    \Input *pRef        - pointer to module state

    \Version 08/25/2014 (cvienneau)
*/
/*************************************************************************************F*/
static void _DirtySessionManagerCleanupSession(DirtySessionManagerRefT *pRef)
{
    // free everything
    _DirtySessionManagerSetChangeableSessionData(pRef, &pRef->ActiveSession, NULL, 0);
    _DirtySessionManagerSetSessionData(pRef, &pRef->ActiveSession, NULL, 0);
    _DirtySessionManagerSetSessionImage(pRef, &pRef->ActiveSession, NULL, 0);
    ds_memclr(&pRef->ActiveSession, sizeof(pRef->ActiveSession));
    pRef->eSessionStatus = DIRTY_SESSION_INVALID;
}

/*F*************************************************************************************/
/*!
    \Function    _DirtySessionManagerUserInvalidated

    \Description
        Mark the specified user as no long being a session member, if there are now no 
        members cleanup the session.

    \Input *pRef        - pointer to module state
    \Input iUserIndex   - user index

    \Version 08/25/2014 (cvienneau)
*/
/*************************************************************************************F*/
static void _DirtySessionManagerUserInvalidated(DirtySessionManagerRefT *pRef, int32_t iUserIndex)
{
    NetPrintf(("dirtysessionmanagerps4: [%p] _DirtySessionManagerUserInvalidated a local user that was expected to be part of the session is no longer online %s.\n", pRef, pRef->ActiveSession.strSessionID));
    pRef->aSessionUsers[iUserIndex].bSessionMember = FALSE;
    if (_DirtySessionManagerCountSessionMembers(pRef) == 0)
    {
        NetPrintf(("dirtysessionmanagerps4: [%p] _DirtySessionManagerUserInvalidated, the last local user has left session %s, cleaning up.\n", pRef, pRef->ActiveSession.strSessionID));
        _DirtySessionManagerCleanupSession(pRef);
    }
}

/*F*************************************************************************************/
/*!
    \Function    _DirtySessionManagerActivatePendingSession

    \Description
        Copies the pending session into the active session space.

    \Input *pRef         - pointer to module state

    \Version 04/03/2013 (cvienneau)
*/
/*************************************************************************************F*/
static void _DirtySessionManagerActivatePendingSession(DirtySessionManagerRefT *pRef)
{
    ds_strnzcpy(pRef->ActiveSession.strSessionID, pRef->PendingSession.strSessionID, sizeof(pRef->ActiveSession.strSessionID));
    ds_strnzcpy(pRef->ActiveSession.strSessionStatus, pRef->PendingSession.strSessionStatus, sizeof(pRef->ActiveSession.strSessionStatus));
    ds_strnzcpy(pRef->ActiveSession.strSessionName, pRef->PendingSession.strSessionName, sizeof(pRef->ActiveSession.strSessionName));
    pRef->ActiveSession.iMaxUsers = pRef->PendingSession.iMaxUsers;
    pRef->ActiveSession.iBinaryDataSize = pRef->PendingSession.iBinaryDataSize;
    pRef->ActiveSession.iImageDataSize = pRef->PendingSession.iImageDataSize;
    pRef->ActiveSession.bPrivate = pRef->PendingSession.bPrivate;
    pRef->ActiveSession.bClosed = pRef->PendingSession.bClosed;
    //we don't provide image getters so once the session is created we can free the image
    _DirtySessionManagerSetSessionImage(pRef, &pRef->PendingSession, NULL, 0);
    _DirtySessionManagerSetSessionData(pRef, &pRef->ActiveSession, pRef->PendingSession.pBinaryData, pRef->PendingSession.iBinaryDataSize);
    pRef->ActiveSession.binaryDataHeader = pRef->PendingSession.binaryDataHeader;
    _DirtySessionManagerSetChangeableSessionData(pRef, &pRef->ActiveSession, pRef->PendingSession.pChangeableBinaryData, pRef->PendingSession.iChangeableBinaryDataSize);
    pRef->ActiveSession.changeableBinaryDataHeader = pRef->PendingSession.changeableBinaryDataHeader;
    pRef->eSessionStatus = DIRTY_SESSION_ACTIVE;
}

/*F********************************************************************************/
/*!
    \Function _DirtySessionManagerUpdate

    \Description
        Update status of DirtySessionManager module.

    \Input *pData   - pointer to DirtySessionManagerRefT module ref
    \Input uData    - unused

    \Version 06/04/2012 (cvienneau)
*/
/********************************************************************************F*/
static void _DirtySessionManagerUpdate(void *pData, uint32_t uData)
{
    DirtySessionManagerRefT *pState = (DirtySessionManagerRefT*)pData;
    DirtySessionManagerUpdate(pState);
}

/*F*************************************************************************************/
/*!
    \Function    _DirtySessionManagerDestroy

    \Description
        Free Dirty Session Manager, only gets called after all session operations completed.

    \Input *pRef    - pointer to module state

    \Version 04/03/2013 (cvienneau)
*/
/*************************************************************************************F*/
static void _DirtySessionManagerDestroy(DirtySessionManagerRefT *pRef)
{
    NetPrintf(("dirtysessionmanagerps4: [%p] _DirtySessionManagerDestroy.\n", pRef));

    //release any image or binary data the session may be holding
    _DirtySessionManagerSetSessionImage(pRef, &pRef->ActiveSession, NULL, 0);
    _DirtySessionManagerSetSessionImage(pRef, &pRef->PendingSession, NULL, 0);
    _DirtySessionManagerSetSessionData(pRef, &pRef->ActiveSession, NULL, 0);
    _DirtySessionManagerSetSessionData(pRef, &pRef->PendingSession, NULL, 0);
    _DirtySessionManagerSetChangeableSessionData(pRef, &pRef->ActiveSession, NULL, 0);
    _DirtySessionManagerSetChangeableSessionData(pRef, &pRef->PendingSession, NULL, 0);

    DirtyWebApiDestroy(pRef->pWebApi);

    DirtyEventManagerUnregisterSystemService(&_DirtySessionManagerOnSystemEvent);

    // remove idle handler
    NetConnIdleDel(_DirtySessionManagerUpdate, pRef);
  
    NetCritKill(&pRef->crit);
    DirtyMemFree(pRef, DIRTYSESSMGR_MEMID, pRef->iMemGroup, pRef->pMemGroupUserData);
}

/*F*************************************************************************************************/
/*!
    \Function _DirtySessionManagerDestroyCallback

    \Description
        Ensure all requests are completed before destroying module state.

    \Input *module   - pointer to query record

    \Output
        int32_t         - zero=success; -1=try again; other negative=error

    \Version 05/24/2013 (cvienneau)
*/
/*************************************************************************************************F*/
static int32_t _DirtySessionManagerDestroyCallback(void *module)
{
    DirtySessionManagerRefT *pRef = (DirtySessionManagerRefT *)module;
    int32_t iActiveRequests = 0;

    //give a chance to clear the requests
    DirtySessionManagerUpdate(pRef);

    NetCritEnter(&pRef->crit);
    iActiveRequests = pRef->iActiveRequests;
    NetCritLeave(&pRef->crit);

    if (iActiveRequests > 0)
    {
        return(-1);
    }
    _DirtySessionManagerDestroy(pRef);
    return(0);
}

/*F*************************************************************************************/
/*!
    \Function    _DirtySessionManagerWebApiWebRequestCallback

    \Description
        DirtyWebApi request completion handler

    \Input iSceError    - error code returned by sony api
    \Input iUserIndex   - user index who issued the command
    \Input iStatusCode  - web status code
    \Input pRespBody    - pointer to response body
    \Input iRespBodyLength  - number of bytes in response body
    \Input pUserData    - a DirtySessionManagerCallbackT for our own use

    \Version 05/23/2013 (cvienneau)
*/
/*************************************************************************************F*/
static void _DirtySessionManagerWebApiWebRequestCallback(int32_t iSceError, int32_t iUserIndex, int32_t iStatusCode, const char *pRespBody, int32_t iRespBodyLength, void *pUserData)
{
    DirtySessionManagerRefT *pRef = ((DirtySessionManagerCallbackT *)pUserData)->pDirtySessionManagerRef;
    int32_t iControl = ((DirtySessionManagerCallbackT *)pUserData)->iControlCode;
    void *pRequestData = ((DirtySessionManagerCallbackT *)pUserData)->pRequestData;
    DirtyMemFree(pUserData, DIRTYSESSMGR_MEMID, pRef->iMemGroup,  pRef->pMemGroupUserData);

    if (iSceError < 0)
    {
        NetPrintf(("dirtysessionmanagerps4: [%p] _DirtySessionManagerWebApiWebRequestCallback user %d, control %C got an sce error %s.\n", pRef, iUserIndex, iControl, DirtyErrGetName(iSceError)));
    }

    //set the main threads response info
    NetCritEnter(&pRef->crit);
    pRef->ResponseData.iControlCode = iControl;
    pRef->ResponseData.iRespBodyLength = iRespBodyLength;
    pRef->ResponseData.iStatusCode = iStatusCode;
    pRef->ResponseData.iUserIndex = iUserIndex;
    pRef->ResponseData.pRespBody = pRespBody;
    pRef->ResponseData.pRequestData = pRequestData;
    pRef->iLastStatusCode = iSceError;
    NetCritLeave(&pRef->crit);

    if (DirtyWebApiStatus(pRef->pWebApi, 'qcal', 0, NULL, 0))
    {
        // do not want to hang up the main thread when webapi queuing is enabled.
        return;
    }

    
    //wait for the response to be processed on the main thread
    while (1)
    {
        NetCritEnter(&pRef->crit);
        iControl = pRef->ResponseData.iControlCode;
        NetCritLeave(&pRef->crit);
        if (iControl == 0)
        {
            return;
        }
        NetConnSleep(1);
    }
}

/*F*************************************************************************************/
/*!
    \Function    _DirtySessionManagerEnqueueControlCode

    \Description
        Add the specified control code to a list of commands to be run by the webapi thread

    \Input *pRef        - pointer to module state
    \Input iControl     - the command id, usually in the form of a four character code 'abcz'
    \Input iUserIndex   - user index
    \Input iValue2      - user data
    \Input pValue       - user data

    \Output
        int32_t         - >=0 success, <0 failure

    \Version 04/03/2013 (cvienneau)
*/
/*************************************************************************************F*/
static int32_t _DirtySessionManagerEnqueueControlCode(DirtySessionManagerRefT *pRef, int32_t iControl, int32_t iUserIndex, int32_t iValue2, const void *pValue)
{
    int32_t iRet = 0;
    SceNpWebApiContentParameter ContentParameter;
    SceNpWebApiHttpMethod HttpMethod;
    const int32_t kstrSize = 256;
    char strApiGroup[kstrSize];
    char strApiPath[kstrSize];
    char *pSendBuff;
    void *pRequestData = NULL;
    
    if ((pSendBuff = (char *)DirtyMemAlloc(pRef->iRequestBuffSize, DIRTYSESSMGR_MEMID, pRef->iMemGroup, pRef->pMemGroupUserData)) != NULL)
    {
        ContentParameter.contentLength = 0;
        if ((iRet = _DirtySessionManagerSetupWebRequest(pRef, iControl, iUserIndex, iValue2, pValue, &pRequestData, strApiGroup, strApiPath, kstrSize, &ContentParameter, &HttpMethod, pSendBuff, pRef->iRequestBuffSize)) >= 0)
        {
            DirtySessionManagerCallbackT *pCallbackData = NULL;
            if ((pCallbackData = (DirtySessionManagerCallbackT*)DirtyMemAlloc(sizeof(DirtySessionManagerCallbackT), DIRTYSESSMGR_MEMID, pRef->iMemGroup, pRef->pMemGroupUserData)) != NULL)
            {
                pCallbackData->iControlCode = iControl;
                pCallbackData->pDirtySessionManagerRef = pRef;
                pCallbackData->pRequestData = pRequestData;
                
                // we only have an valid avctive request if DirtyWebApi successfully creates the request
                if ((iRet = DirtyWebApiRequest(pRef->pWebApi, iUserIndex, strApiGroup, HttpMethod, strApiPath, (const uint8_t *)pSendBuff, ContentParameter.contentLength, ContentParameter.pContentType, _DirtySessionManagerWebApiWebRequestCallback, pCallbackData)) >= 0)
                {
                    NetCritEnter(&pRef->crit);
                    pRef->iActiveRequests++;
                    NetPrintf(("dirtysessionmanagerps4: [%p] _DirtySessionManagerEnqueueControlCode added %C, pending request count = %d\n", pRef, pCallbackData->iControlCode, pRef->iActiveRequests));
                    NetCritLeave(&pRef->crit);
                }
                else
                {
                    NetPrintf(("dirtysessionmanagerps4: [%p] _DirtySessionManagerEnqueueControlCode DirtyWebApu failed to add %C request, pending request count = %d\n" ,pRef, pCallbackData->iControlCode, pRef->iActiveRequests ));
                    iRet = -1;
                }
            }
            else
            {
                NetPrintf(("dirtysessionmanagerps4: [%p] _DirtySessionManagerEnqueueControlCode error allocating DirtySessionManagerCallbackT.\n", pRef));
                iRet = -1;
            }
        }
        DirtyMemFree(pSendBuff, DIRTYSESSMGR_MEMID, pRef->iMemGroup, pRef->pMemGroupUserData);
    }
    else
    {
        NetPrintf(("dirtysessionmanagerps4: [%p] _DirtySessionManagerEnqueueControlCode error allocating pSendBuff.\n", pRef));
    }

    return (iRet);
}

/*F*************************************************************************************/
/*!
    \Function    _DirtySessionManagerSetupLockSession

    \Description
        Locks the current active seesion

    \Input *pRef        - pointer to module state
    \Input iUserIndex   - user index
    \Input pApiGroup    - string to populate indicating which group of sce webapi commands we're using
    \Input pApiPath     - string to populate indicating url path of the command
    \Input iStrSize     - length of the pApiGroup and pApiPath parameters
    \Input pContentParameter     - SceNpWebApiContentParameter, contains total content length and type
    \Input pHttpMethod   - SceNpWebApiHttpMethod, http method type eg. GET, PUT
    \Input pSendBuff    - buffer to accept the http request body
    \Input iBuffSize    - max size of the pSendBuff

    \Output
        int32_t         - >=0 success, <0 failure

    \Version 12/02/2014 (tcho)
*/
/*************************************************************************************F*/
static int32_t _DirtySessionManagerSetupLockSession(DirtySessionManagerRefT *pRef, int32_t iUserIndex, char *pApiGroup, char *pApiPath, int32_t iStrSize, SceNpWebApiContentParameter *pContentParameter, SceNpWebApiHttpMethod *pHttpMethod, char *pSendBuff, int32_t iBuffSize)
{
    ds_memclr(pSendBuff, iBuffSize);
    ds_snzprintf(pApiPath, iStrSize, DIRTY_SESSION_WEBAPI_PATH_SESSION_UPDATE, pRef->PendingSession.strSessionID);
    ds_snzprintf(pApiGroup, iStrSize, DIRTY_SESSION_WEBAPI_GROUP_SESSION);
    *pHttpMethod = SCE_NP_WEBAPI_HTTP_METHOD_PUT;
    pContentParameter->pContentType = "application/json; charset=utf-8";

    // create the Json body
    int32_t iLength = ds_snzprintf(pSendBuff, iBuffSize,
        "{\r\n"
        "  \"sessionLockFlag\":\"%s\"\r\n" 
        "}\r\n",
        pRef->PendingSession.bSessionLock ? "true" : "false"
        );

    pContentParameter->contentLength = iLength;
    return(iLength);
}

/*F*************************************************************************************/
/*!
    \Function    _DirtySessionManagerSetupCreateSession

    \Description
        Prepares a web api request to create and join a sony session.

    \Input *pRef        - pointer to module state
    \Input iUserIndex   - user index
    \Input pApiGroup    - string to populate indicating which group of sce webapi commands we're using
    \Input pApiPath     - string to populate indicating url path of the command
    \Input iStrSize     - length of the pApiGroup and pApiPath parameters
    \Input pContentParameter     - SceNpWebApiContentParameter, contains total content length and type
    \Input pHttpMethod   - SceNpWebApiHttpMethod, http method type eg. GET, PUT
    \Input pSendBuff    - buffer to accept the http request body
    \Input iBuffSize    - max size of the pSendBuff

    \Output
        int32_t         - >=0 success, <0 failure

    \Version 04/03/2013 (cvienneau)
*/
/*************************************************************************************F*/
static int32_t _DirtySessionManagerSetupCreateSession(DirtySessionManagerRefT *pRef, int32_t iUserIndex, char *pApiGroup, char *pApiPath, int32_t iStrSize, SceNpWebApiContentParameter *pContentParameter, SceNpWebApiHttpMethod *pHttpMethod, char *pSendBuff, int32_t iBuffSize)
{
    const int32_t iTempBuffSize = 2048;
    char aTempJson[iTempBuffSize];
    char aTempLocNameJson[iTempBuffSize];
    char aTempLocStatusJson[iTempBuffSize];
    ds_memclr(pSendBuff, iBuffSize);
    ds_memclr(aTempJson, iTempBuffSize);

    if (pRef->eSessionStatus != DIRTY_SESSION_INVALID)
    {
        NetPrintf(("dirtysessionmanagerps4: [%p] 'scre' Expected DIRTY_SESSION_INVALID, might still be processing control queue.\n", pRef));
    }
    pRef->eSessionStatus = DIRTY_SESSION_PENDING;

    ds_snzprintf(pApiPath, iStrSize, DIRTY_SESSION_WEBAPI_PATH_SESSION_CREATE);
    ds_snzprintf(pApiGroup, iStrSize, DIRTY_SESSION_WEBAPI_GROUP_SESSION);
    *pHttpMethod = SCE_NP_WEBAPI_HTTP_METHOD_POST;
    pContentParameter->pContentType = "multipart/mixed; boundary="DIRTY_SESSION_MULTI_PART_SEPERATOR;
    // create the Json body
    int32_t iLength = ds_snzprintf(aTempJson, iTempBuffSize,
        "{\r\n"
        "  \"sessionType\":\"owner-migration\",\r\n"
        "  \"sessionPrivacy\":\"%s\",\r\n"
        "  \"sessionMaxUser\":%d,\r\n"
        "  \"sessionName\":\"%s\",\r\n"
        "  \"sessionStatus\":\"%s\",\r\n"
        "%s%s"//localized data, is optional  
        "  \"availablePlatforms\":[\"PS4\"]\r\n" 
        "}\r\n",
        pRef->PendingSession.bPrivate ? "private" : "public",
        pRef->PendingSession.iMaxUsers,
        pRef->PendingSession.strSessionName,
        pRef->PendingSession.strSessionStatus,
        _DirtySessionManagerGetLocalizedJson(pRef, aTempLocNameJson, iTempBuffSize, TRUE),
        _DirtySessionManagerGetLocalizedJson(pRef, aTempLocStatusJson, iTempBuffSize, FALSE)
        );

    //create the header for json, attach json body, create header for image
    iLength = ds_snzprintf(pSendBuff, iBuffSize, 
        "--"DIRTY_SESSION_MULTI_PART_SEPERATOR"\r\n"
        "Content-Type:application/json; charset=utf-8\r\n"
        "Content-Description:session-request\r\n"
        "Content-Length:%d\r\n"
        "\r\n"
        "%s"
        "\r\n"
        "--"DIRTY_SESSION_MULTI_PART_SEPERATOR"\r\n"
        "Content-Type:image/jpeg\r\n" 
        "Content-Disposition:attachment\r\n"
        "Content-Description:session-image\r\n"
        "Content-Length:%d\r\n"
        "\r\n",
        iLength, aTempJson, pRef->PendingSession.iImageDataSize);

    if ((iLength + pRef->PendingSession.iImageDataSize) <= iBuffSize)
    {
        //copy the image data into the buffer
        ds_memcpy(pSendBuff+ iLength, pRef->PendingSession.pImageData, pRef->PendingSession.iImageDataSize); 
        iLength += pRef->PendingSession.iImageDataSize;

        //continue writing binary blob header
        iLength += ds_snzprintf(pSendBuff + iLength, iBuffSize - iLength, 
            "\r\n"
            "--"DIRTY_SESSION_MULTI_PART_SEPERATOR"\r\n"
            "Content-Type:application/octet-stream\r\n" 
            "Content-Disposition:attachment\r\n"
            "Content-Description:session-data\r\n"
            "Content-Length:%d\r\n"
            "\r\n", pRef->PendingSession.iBinaryDataSize + sizeof(pRef->PendingSession.binaryDataHeader));

        if ((iLength + (int32_t)sizeof(pRef->PendingSession.binaryDataHeader) + pRef->PendingSession.iBinaryDataSize) <= iBuffSize)
        {
            //copy the binary data into the buffer
            ds_memcpy(pSendBuff+ iLength, &pRef->PendingSession.binaryDataHeader, sizeof(pRef->PendingSession.binaryDataHeader)); 
            iLength += sizeof(pRef->PendingSession.binaryDataHeader);
            ds_memcpy(pSendBuff+ iLength, pRef->PendingSession.pBinaryData, pRef->PendingSession.iBinaryDataSize); 
            iLength += pRef->PendingSession.iBinaryDataSize;
        }
        else
        {
            iLength = -1;
            pRef->eSessionStatus = DIRTY_SESSION_INVALID;
            NetPrintf(("dirtysessionmanagerps4: [%p] _DirtySessionManagerSetupCreateSession() failed, insufficient send buffer for binary data.\n", pRef));
        }

        //continue writing changeable binary blob header
        iLength += ds_snzprintf(pSendBuff + iLength, iBuffSize - iLength, 
            "\r\n"
            "--"DIRTY_SESSION_MULTI_PART_SEPERATOR"\r\n"
            "Content-Type:application/octet-stream\r\n" 
            "Content-Disposition:attachment\r\n"
            "Content-Description:changeable-session-data\r\n"
            "Content-Length:%d\r\n"
            "\r\n", pRef->PendingSession.iChangeableBinaryDataSize + sizeof(pRef->PendingSession.changeableBinaryDataHeader));

        if ((iLength + (int32_t)sizeof(pRef->PendingSession.changeableBinaryDataHeader) + pRef->PendingSession.iChangeableBinaryDataSize) <= iBuffSize)
        {
            //copy the changeable binary data into the buffer
            ds_memcpy(pSendBuff+ iLength, &pRef->PendingSession.changeableBinaryDataHeader, sizeof(pRef->PendingSession.changeableBinaryDataHeader)); 
            iLength += sizeof(pRef->PendingSession.changeableBinaryDataHeader);
            ds_memcpy(pSendBuff+ iLength, pRef->PendingSession.pChangeableBinaryData, pRef->PendingSession.iChangeableBinaryDataSize); 
            iLength += pRef->PendingSession.iChangeableBinaryDataSize;
        }
        else
        {
            iLength = -1;
            pRef->eSessionStatus = DIRTY_SESSION_INVALID;
            NetPrintf(("dirtysessionmanagerps4: [%p] _DirtySessionManagerSetupCreateSession() failed, insufficient send buffer for changeable binary data.\n", pRef));
        }

        if (iLength > 0)
        {
            iLength += ds_snzprintf(pSendBuff + iLength, iBuffSize - iLength,
                "\r\n"
                "--"DIRTY_SESSION_MULTI_PART_SEPERATOR"--\r\n"); //end
        }
    }
    else
    {
        iLength = -1;
        pRef->eSessionStatus = DIRTY_SESSION_INVALID;
        NetPrintf(("dirtysessionmanagerps4: [%p] _DirtySessionManagerSetupCreateSession() failed, insufficient send buffer for image data.\n", pRef));
    }
    
    pContentParameter->contentLength = iLength;
    return(iLength);
}

/*F*************************************************************************************/
/*!
    \Function    _DirtySessionManagerPostInvitation

    \Description
        Prepares a web api request to send an invitation to current session without first party dialog.

    \Input *pRef                 - pointer to module state
    \Input iUserIndex            - user index
    \Input pApiGroup             - string to populate indicating which group of sce webapi commands we're using
    \Input pApiPath              - string to populate indicating url path of the command
    \Input iStrSize              - length of the pApiGroup and pApiPath parameters
    \Input pContentParameter     - SceNpWebApiContentParameter, contains total content length and type
    \Input pHttpMethod           - SceNpWebApiHttpMethod, http method type eg. GET, PUT
    \Input pSendBuff             - buffer to accept the http request body
    \Input iBuffSize             - max size of the pSendBuff

    \Output
        int32_t         - >=0 success, <0 failure

    \Notes
        See an example in the Sony docs at 
        https://ps4.scedev.net/resources/documents/WebAPI/1/Session_Invitation_WebAPI-Reference/0009.html


    \Version 11/01/2013 (tcho)
*/
/*************************************************************************************F*/
static int32_t _DirtySessionManagerPostInvitation(DirtySessionManagerRefT *pRef, int32_t iUserIndex, char *pApiGroup, char *pApiPath, int32_t iStrSize, SceNpWebApiContentParameter *pContentParameter, SceNpWebApiHttpMethod *pHttpMethod, char *pSendBuff, int32_t iBuffSize)
{
    const int32_t iTempBuffSize = 8192;
    const int32_t iSendListSize = DIRTY_SESSION_DEFAULT_SEND_INVITE_LIST_SIZE;
    char aTempJson[iTempBuffSize];
    char aTempToList[iSendListSize];
    uint8_t bIsNotListEmpty = 0;

    ds_memclr(aTempToList, iSendListSize);
    ds_memclr(aTempJson, iTempBuffSize);

    if (pRef->eSessionStatus != DIRTY_SESSION_ACTIVE)
    {
        NetPrintf(("dirtysessionmanagerps4: [%p] 'sind' Expected DIRTY_SESSION_ACTIVE, might still be processing control queue.\n", pRef));
    }

    ds_snzprintf(pApiPath, iStrSize, DIRTY_SESSION_WEBAPI_PATH_SESSION_SEND_INVITE, pRef->ActiveSession.strSessionID);
    ds_snzprintf(pApiGroup, iStrSize, DIRTY_SESSION_WEBAPI_GROUP_SESSION);

    *pHttpMethod = SCE_NP_WEBAPI_HTTP_METHOD_POST;
    pContentParameter->pContentType ="multipart/mixed; boundary="DIRTY_SESSION_MULTI_PART_SEPERATOR;

    uint32_t uWritten = 0;
    uWritten = ds_strnzcat(aTempToList, "[\r\n    ", iSendListSize);

    //Building the Json to list
    for(uint32_t index = 0; index < DIRTY_SESSION_MAX_INVITATION_MEMBER_LIST_LENGTH; ++index)
    {
        char strAccountId[20];
        if (pRef->InvitationInfo.aAccountIds[index] == 0)
        {
            break;
        }   

        bIsNotListEmpty = 1;
        ds_snzprintf(strAccountId, (int32_t)sizeof(strAccountId), "%llu", pRef->InvitationInfo.aAccountIds[index]);

        ds_strnzcat(aTempToList, "\"", iSendListSize);
        ds_strnzcat(aTempToList, strAccountId, iSendListSize);
        uWritten = ds_strnzcat(aTempToList, "\",\r\n    ", iSendListSize);
    }

    if (!bIsNotListEmpty)
    {
        NetPrintf(("dirtysessionmanagerps4: [%p] 'sind' Expected Invite List cannot be empty.", pRef));
        return(-1);
    }

    //Remove Extra comma and spaces & terminate the list
    uWritten = uWritten - strlen(",\r\n    ");
    aTempToList[uWritten] = '\0';
    uWritten = ds_strnzcat(aTempToList, "\r\n  ]", iSendListSize);

    int32_t iLength = ds_snzprintf(aTempJson, iTempBuffSize,
        "{\r\n"
        "  \"to\":%s,\r\n"
        "  \"message\":\"%s\"\r\n"
        "}\r\n"
        , aTempToList
        , pRef->InvitationInfo.strMessage);

    iLength = ds_snzprintf(pSendBuff, iBuffSize, 
        "--"DIRTY_SESSION_MULTI_PART_SEPERATOR"\r\n"
        "Content-Type:application/json; charset=utf-8\r\n"
        "Content-Description:invitation-request\r\n"
        "Content-Length:%d\r\n"
        "\r\n"
        "%s"
        "\r\n"
        "--"DIRTY_SESSION_MULTI_PART_SEPERATOR"--\r\n"
        ,iLength
        ,aTempJson);

    pContentParameter->contentLength = iLength;

    return(iLength);
}

/*F*************************************************************************************/
/*!
    \Function    _DirtySessionManagerSetupGetSessionData

    \Description
        Prepares a webapi request to download the binary blob portion of a session.

    \Input *pRef        - pointer to module state
    \Input iUserIndex   - user index
    \Input pApiGroup    - string to populate indicating which group of sce webapi commands we're using
    \Input pApiPath     - string to populate indicating url path of the command
    \Input iStrSize     - length of the pApiGroup and pApiPath parameters
    \Input pContentParameter - SceNpWebApiContentParameter, contains total content length and type
    \Input pHttpMethod  - SceNpWebApiHttpMethod, http method type eg. GET, PUT
    \Input pSendBuff    - buffer to accept the http request body
    \Input iBuffSize    - max size of the pSendBuff

    \Output
        int32_t         - >=0 success, <0 failure

    \Version 04/03/2013 (cvienneau)
*/
/*************************************************************************************F*/
static int32_t _DirtySessionManagerSetupGetSessionData(DirtySessionManagerRefT *pRef, int32_t iUserIndex, char *pApiGroup, char *pApiPath, int32_t iStrSize, SceNpWebApiContentParameter *pContentParameter, SceNpWebApiHttpMethod *pHttpMethod, char *pSendBuff, int32_t iBuffSize)
{
    ds_snzprintf(pApiPath, iStrSize, DIRTY_SESSION_WEBAPI_PATH_SESSION_GET_DATA, pRef->PendingSession.strSessionID);
    ds_snzprintf(pApiGroup, iStrSize, DIRTY_SESSION_WEBAPI_GROUP_SESSION);
    *pHttpMethod = SCE_NP_WEBAPI_HTTP_METHOD_GET;
    return(1);
}

/*F*************************************************************************************/
/*!
    \Function    _DirtySessionManagerSetupGetSessionChangeableData

    \Description
        Prepares a webapi request to download the changeable binary blob portion of a session.

    \Input *pRef        - pointer to module state
    \Input iUserIndex   - user index
    \Input pApiGroup    - string to populate indicating which group of sce webapi commands we're using
    \Input pApiPath     - string to populate indicating url path of the command
    \Input iStrSize     - length of the pApiGroup and pApiPath parameters
    \Input pContentParameter - SceNpWebApiContentParameter, contains total content length and type
    \Input pHttpMethod  - SceNpWebApiHttpMethod, http method type eg. GET, PUT
    \Input pSendBuff    - buffer to accept the http request body
    \Input iBuffSize    - max size of the pSendBuff

    \Output
        int32_t         - >=0 success, <0 failure

    \Version 08/17/2015 (mclouatre)
*/
/*************************************************************************************F*/
static int32_t _DirtySessionManagerSetupGetSessionChangeableData(DirtySessionManagerRefT *pRef, int32_t iUserIndex, char *pApiGroup, char *pApiPath, int32_t iStrSize, SceNpWebApiContentParameter *pContentParameter, SceNpWebApiHttpMethod *pHttpMethod, char *pSendBuff, int32_t iBuffSize)
{
    ds_snzprintf(pApiPath, iStrSize, DIRTY_SESSION_WEBAPI_PATH_SESSION_GET_CHANGEABLE_DATA, pRef->PendingSession.strSessionID);
    ds_snzprintf(pApiGroup, iStrSize, DIRTY_SESSION_WEBAPI_GROUP_SESSION);
    *pHttpMethod = SCE_NP_WEBAPI_HTTP_METHOD_GET;
    return(1);
}

/*F*************************************************************************************/
/*!
    \Function    _DirtySessionManagerSetupGetInvites

    \Description
        Prepares a webapi request to get a list of all invites for this user.

    \Input *pRef        - pointer to module state
    \Input iUserIndex   - user index
    \Input pApiGroup    - string to populate indicating which group of sce webapi commands we're using
    \Input pApiPath     - string to populate indicating url path of the command
    \Input iStrSize     - length of the pApiGroup and pApiPath parameters
    \Input pContentParameter     - SceNpWebApiContentParameter, contains total content length and type
    \Input pHttpMethod   - SceNpWebApiHttpMethod, http method type eg. GET, PUT
    \Input pSendBuff    - buffer to accept the http request body
    \Input iBuffSize    - max size of the pSendBuff

    \Output
        int32_t         - >=0 success, <0 failure

    \Version 04/03/2013 (cvienneau)
*/
/*************************************************************************************F*/
static int32_t _DirtySessionManagerSetupGetInvites(DirtySessionManagerRefT *pRef, int32_t iUserIndex, char *pApiGroup, char *pApiPath, int32_t iStrSize, SceNpWebApiContentParameter *pContentParameter, SceNpWebApiHttpMethod *pHttpMethod, char *pSendBuff, int32_t iBuffSize)
{    
    ds_snzprintf(pApiPath, iStrSize, DIRTY_SESSION_WEBAPI_PATH_SESSION_INVITATIONS);
    ds_snzprintf(pApiGroup, iStrSize, DIRTY_SESSION_WEBAPI_GROUP_SESSION);
    *pHttpMethod = SCE_NP_WEBAPI_HTTP_METHOD_GET;
    return(0);
}

/*F*************************************************************************************/
/*!
    \Function    _DirtySessionManagerSetupGetInviteMetadata

    \Description
        Prepares a webapi request to get a list of all invites for this user.

    \Input *pRef        - pointer to module state
    \Input iUserIndex   - user index
    \Input iInviteIndex - index of invite in pRef->aSessionUsers[iUserIndex].RecievedInvitations.aInvitations[]
    \Input pInviteId    - pointer to location to store the inviteId, so when the results arrive we can determine for which invite
    \Input pApiGroup    - string to populate indicating which group of sce webapi commands we're using
    \Input pApiPath     - string to populate indicating url path of the command
    \Input iStrSize     - length of the pApiGroup and pApiPath parameters
    \Input pContentParameter     - SceNpWebApiContentParameter, contains total content length and type
    \Input pHttpMethod   - SceNpWebApiHttpMethod, http method type eg. GET, PUT
    \Input pSendBuff    - buffer to accept the http request body
    \Input iBuffSize    - max size of the pSendBuff

    \Output
        int32_t         - >=0 success, <0 failure

    \Version 04/03/2013 (cvienneau)
*/
/*************************************************************************************F*/
static int32_t _DirtySessionManagerSetupGetInviteMetadata(DirtySessionManagerRefT *pRef, int32_t iUserIndex, int32_t iInviteIndex, void ** pInviteId, char *pApiGroup, char *pApiPath, int32_t iStrSize, SceNpWebApiContentParameter *pContentParameter, SceNpWebApiHttpMethod *pHttpMethod, char *pSendBuff, int32_t iBuffSize)
{
    int32_t iRet = -1;
    
    //check to be sure there is a valid invite at the index the user specified
    if ((iInviteIndex < pRef->aSessionUsers[iUserIndex].RecievedInvitations.iNumInvitations) && (iInviteIndex >= 0))
    {
        ds_snzprintf(pApiPath, iStrSize, DIRTY_SESSION_WEBAPI_PATH_SESSION_GET_INVITE, pRef->aSessionUsers[iUserIndex].RecievedInvitations.aInvitations[iInviteIndex].strInvitationID);
        ds_snzprintf(pApiGroup, iStrSize, DIRTY_SESSION_WEBAPI_GROUP_SESSION);
        *pHttpMethod = SCE_NP_WEBAPI_HTTP_METHOD_GET;

        //store the inviteId in the void pointer so the response has some idea what invite we're talking about
        *pInviteId = pRef->aSessionUsers[iUserIndex].RecievedInvitations.aInvitations[iInviteIndex].strInvitationID;
    }
    else
    {
        NetPrintf(("dirtysessionmanagerps4: [%p] _DirtySessionManagerSetupGetInviteMetadata() failed, invalid invite index.\n", pRef));
    }
    return(iRet);
}

/*F*************************************************************************************/
/*!
    \Function    _DirtySessionManagerSetupGetMemberOfSessions

    \Description
        Prepares a webapi request to get a list of all sessions for this user.

    \Input *pRef        - pointer to module state
    \Input iUserIndex   - user index
    \Input pApiGroup    - string to populate indicating which group of sce webapi commands we're using
    \Input pApiPath     - string to populate indicating url path of the command
    \Input iStrSize     - length of the pApiGroup and pApiPath parameters
    \Input pContentParameter     - SceNpWebApiContentParameter, contains total content length and type
    \Input pHttpMethod   - SceNpWebApiHttpMethod, http method type eg. GET, PUT
    \Input pSendBuff    - buffer to accept the http request body
    \Input iBuffSize    - max size of the pSendBuff

    \Output
        int32_t         - >=0 success, <0 failure

    \Version 04/03/2013 (cvienneau)
*/
/*************************************************************************************F*/
static int32_t _DirtySessionManagerSetupGetMemberOfSessions(DirtySessionManagerRefT *pRef, int32_t iUserIndex, char *pApiGroup, char *pApiPath, int32_t iStrSize, SceNpWebApiContentParameter *pContentParameter, SceNpWebApiHttpMethod *pHttpMethod, char *pSendBuff, int32_t iBuffSize)
{
    ds_snzprintf(pApiPath, iStrSize, DIRTY_SESSION_WEBAPI_PATH_SESSION_MEMBER_OF);
    ds_snzprintf(pApiGroup, iStrSize, DIRTY_SESSION_WEBAPI_GROUP_SESSION);
    *pHttpMethod = SCE_NP_WEBAPI_HTTP_METHOD_GET;
    return(0);
}

/*F*************************************************************************************/
/*!
    \Function    _DirtySessionManagerSetupGetSession

    \Description
        Prepares a webapi request to get the basic session information.

    \Input *pRef        - pointer to module state
    \Input iUserIndex   - user index
    \Input pApiGroup    - string to populate indicating which group of sce webapi commands we're using
    \Input pApiPath     - string to populate indicating url path of the command
    \Input iStrSize     - length of the pApiGroup and pApiPath parameters
    \Input pContentParameter     - SceNpWebApiContentParameter, contains total content length and type
    \Input pHttpMethod   - SceNpWebApiHttpMethod, http method type eg. GET, PUT
    \Input pSendBuff    - buffer to accept the http request body
    \Input iBuffSize    - max size of the pSendBuff

    \Output
        int32_t         - >=0 success, <0 failure

    \Version 04/03/2013 (cvienneau)
*/
/*************************************************************************************F*/
static int32_t _DirtySessionManagerSetupGetSession(DirtySessionManagerRefT *pRef, int32_t iUserIndex, char *pApiGroup, char *pApiPath, int32_t iStrSize, SceNpWebApiContentParameter *pContentParameter, SceNpWebApiHttpMethod *pHttpMethod, char *pSendBuff, int32_t iBuffSize)
{
    ds_snzprintf(pApiPath, iStrSize, DIRTY_SESSION_WEBAPI_PATH_SESSION_GET, pRef->PendingSession.strSessionID);
    ds_snzprintf(pApiGroup, iStrSize, DIRTY_SESSION_WEBAPI_GROUP_SESSION);
    *pHttpMethod = SCE_NP_WEBAPI_HTTP_METHOD_GET;
    return(1);
}

/*F*************************************************************************************/
/*!
    \Function    _DirtySessionManagerSetupDeleteSession

    \Description
        Prepares a webapi request to remove a player from the session.

    \Input *pRef        - pointer to module state
    \Input iUserIndex   - user index
    \Input pApiGroup    - string to populate indicating which group of sce webapi commands we're using
    \Input pApiPath     - string to populate indicating url path of the command
    \Input iStrSize     - length of the pApiGroup and pApiPath parameters
    \Input pContentParameter     - SceNpWebApiContentParameter, contains total content length and type
    \Input pHttpMethod   - SceNpWebApiHttpMethod, http method type eg. GET, PUT
    \Input pSendBuff    - buffer to accept the http request body
    \Input iBuffSize    - max size of the pSendBuff

    \Output
        int32_t         - >=0 success, <0 failure

    \Version 04/03/2013 (cvienneau)
*/
/*************************************************************************************F*/
static int32_t _DirtySessionManagerSetupDeleteSession(DirtySessionManagerRefT *pRef, int32_t iUserIndex, char *pApiGroup, char *pApiPath, int32_t iStrSize, SceNpWebApiContentParameter *pContentParameter, SceNpWebApiHttpMethod *pHttpMethod, char *pSendBuff, int32_t iBuffSize)
{
    SceNpAccountId accountId;
    int32_t iRet;

    if (pRef->eSessionStatus != DIRTY_SESSION_ACTIVE)
    {
        NetPrintf(("dirtysessionmanagerps4: [%p] 'sdel' Expected DIRTY_SESSION_ACTIVE, might still be processing control queue.\n", pRef));
    }

    //if we succeed or not, still remove the player, a failure probably means the session is gone anyways.
    pRef->aSessionUsers[iUserIndex].bSessionMember = FALSE;
    
    //we only go into the pending state when the last user is leaving the session
    if (_DirtySessionManagerCountSessionMembers(pRef) == 0)
    {
        pRef->eSessionStatus = DIRTY_SESSION_PENDING;
    }

    //get online id
    if ((iRet = NetConnStatus('acct', iUserIndex, &accountId, sizeof(accountId))) >= 0) 
    {
        ds_snzprintf(pApiPath, iStrSize, DIRTY_SESSION_WEBAPI_PATH_SESSION_REMOVE_MEMBER, pRef->ActiveSession.strSessionID);
        ds_snzprintf(pApiGroup, iStrSize, DIRTY_SESSION_WEBAPI_GROUP_SESSION);
        *pHttpMethod = SCE_NP_WEBAPI_HTTP_METHOD_DELETE;

        pRef->bPendingNameSet = false;
        pRef->bPendingStatusSet = false;

        ds_memclr(&(pRef->lastAcceptUserId), sizeof(SceUserServiceUserId));
    }
    else
    {
        NetPrintf(("dirtysessionmanagerps4: [%p] _DirtySessionManagerSetupDeleteSession() failed, couldn't get account id.\n", pRef));
        _DirtySessionManagerUserInvalidated(pRef, iUserIndex);
    }
    return(iRet);
}

/*F*************************************************************************************/
/*!
    \Function    _DirtySessionManagerSetupJoinSession

    \Description
        Prepares a webapi request to join a session.

    \Input *pRef        - pointer to module state
    \Input iUserIndex   - user index
    \Input pApiGroup    - string to populate indicating which group of sce webapi commands we're using
    \Input pApiPath     - string to populate indicating url path of the command
    \Input iStrSize     - length of the pApiGroup and pApiPath parameters
    \Input pContentParameter     - SceNpWebApiContentParameter, contains total content length and type
    \Input pHttpMethod   - SceNpWebApiHttpMethod, http method type eg. GET, PUT
    \Input pSendBuff    - buffer to accept the http request body
    \Input iBuffSize    - max size of the pSendBuff

    \Output
        int32_t         - >=0 success, <0 failure

    \Version 04/03/2013 (cvienneau)
*/
/*************************************************************************************F*/
static int32_t _DirtySessionManagerSetupJoinSession(DirtySessionManagerRefT *pRef, int32_t iUserIndex, char *pApiGroup, char *pApiPath, int32_t iStrSize, SceNpWebApiContentParameter *pContentParameter, SceNpWebApiHttpMethod *pHttpMethod, char *pSendBuff, int32_t iBuffSize)
{
    int32_t iRet  = 1;

    if (pRef->eSessionStatus == DIRTY_SESSION_INVALID)
    {
        pRef->eSessionStatus = DIRTY_SESSION_PENDING;
    }

    if (pRef->eSessionStatus == DIRTY_SESSION_PENDING) //first join, joining from pending session
    {
        ds_snzprintf(pApiPath, iStrSize, DIRTY_SESSION_WEBAPI_PATH_SESSION_JOIN, pRef->PendingSession.strSessionID);
    }
    else if (pRef->eSessionStatus == DIRTY_SESSION_ACTIVE)  //joining into an active session, mlu
    {
        ds_snzprintf(pApiPath, iStrSize, DIRTY_SESSION_WEBAPI_PATH_SESSION_JOIN, pRef->ActiveSession.strSessionID);
    }
    else 
    {
        NetPrintf(("dirtysessionmanagerps4: [%p] _DirtySessionManagerSetupJoinSession unexpected state.\n", pRef));
        iRet = -1;
    }

    ds_snzprintf(pApiGroup, iStrSize, DIRTY_SESSION_WEBAPI_GROUP_SESSION);
    *pHttpMethod = SCE_NP_WEBAPI_HTTP_METHOD_POST;
    pContentParameter->pContentType = "application/json; charset=utf-8";
    return(iRet);
}

/*F*************************************************************************************/
/*!
    \Function    _DirtySessionManagerSetupUpdateSessionMaxUsers

    \Description
        Prepares a webapi request to update the max users attribute of a session.

    \Input *pRef        - pointer to module state
    \Input iUserIndex   - user index
    \Input pApiGroup    - string to populate indicating which group of sce webapi commands we're using
    \Input pApiPath     - string to populate indicating url path of the command
    \Input iStrSize     - length of the pApiGroup and pApiPath parameters
    \Input pContentParameter     - SceNpWebApiContentParameter, contains total content length and type
    \Input pHttpMethod   - SceNpWebApiHttpMethod, http method type eg. GET, PUT
    \Input pSendBuff    - buffer to accept the http request body
    \Input iBuffSize    - max size of the pSendBuff

    \Output
        int32_t         - >=0 success, <0 failure

    \Version 21/09/2013 (mclouatre)
*/
/*************************************************************************************F*/
static int32_t _DirtySessionManagerSetupUpdateSessionMaxUsers(DirtySessionManagerRefT *pRef, int32_t iUserIndex, char *pApiGroup, char *pApiPath, int32_t iStrSize, SceNpWebApiContentParameter *pContentParameter, SceNpWebApiHttpMethod *pHttpMethod, char *pSendBuff, int32_t iBuffSize)
{
    ds_memclr(pSendBuff, iBuffSize);
    ds_snzprintf(pApiPath, iStrSize, DIRTY_SESSION_WEBAPI_PATH_SESSION_UPDATE, pRef->PendingSession.strSessionID);
    ds_snzprintf(pApiGroup, iStrSize, DIRTY_SESSION_WEBAPI_GROUP_SESSION);
    *pHttpMethod = SCE_NP_WEBAPI_HTTP_METHOD_PUT;
    pContentParameter->pContentType = "application/json; charset=utf-8";

    // create the Json body
    int32_t iLength = ds_snzprintf(pSendBuff, iBuffSize,
        "{\r\n"
        "  \"sessionMaxUser\":\"%d\"\r\n" 
        "}\r\n",
        pRef->PendingSession.iMaxUsers
        );

    pContentParameter->contentLength = iLength;
    return(iLength);
}

/*F*************************************************************************************/
/*!
    \Function    _DirtySessionManagerSetupUpdateSessionImage

    \Description
        Prepares a webapi request to update the image portion of a session.

    \Input *pRef        - pointer to module state
    \Input iUserIndex   - user index
    \Input pApiGroup    - string to populate indicating which group of sce webapi commands we're using
    \Input pApiPath     - string to populate indicating url path of the command
    \Input iStrSize     - length of the pApiGroup and pApiPath parameters
    \Input pContentParameter     - SceNpWebApiContentParameter, contains total content length and type
    \Input pHttpMethod   - SceNpWebApiHttpMethod, http method type eg. GET, PUT
    \Input pSendBuff    - buffer to accept the http request body
    \Input iBuffSize    - max size of the pSendBuff

    \Output
        int32_t         - >=0 success, <0 failure

    \Version 04/03/2013 (cvienneau)
*/
/*************************************************************************************F*/
static int32_t _DirtySessionManagerSetupUpdateSessionImage(DirtySessionManagerRefT *pRef, int32_t iUserIndex, char *pApiGroup, char *pApiPath, int32_t iStrSize, SceNpWebApiContentParameter *pContentParameter, SceNpWebApiHttpMethod *pHttpMethod, char *pSendBuff, int32_t iBuffSize)
{
    int32_t iRet;
    if (iBuffSize >= pRef->PendingSession.iImageDataSize)
    {
        iRet = 1;
        pContentParameter->pContentType = "image/jpeg";
        ds_snzprintf(pApiPath, iStrSize, DIRTY_SESSION_WEBAPI_PATH_SESSION_UPDATE_IMAGE, pRef->PendingSession.strSessionID);
        ds_snzprintf(pApiGroup, iStrSize, DIRTY_SESSION_WEBAPI_GROUP_SESSION);
        *pHttpMethod = SCE_NP_WEBAPI_HTTP_METHOD_PUT;
        ds_memcpy(pSendBuff, pRef->PendingSession.pImageData, pRef->PendingSession.iImageDataSize);
        pContentParameter->contentLength = pRef->PendingSession.iImageDataSize;
    }
    else
    {
        iRet = -1;
        NetPrintf(("dirtysessionmanagerps4: [%p] _DirtySessionManagerSetupUpdateSessionImage insufficent send buffer size\n", pRef));
    }

    return(iRet);
}

/*F*************************************************************************************/
/*!
    \Function    _DirtySessionManagerSetupUpdateInvite

    \Description
        Prepares a webapi request to update the invite usedFlag

    \Input *pRef        - pointer to module state
    \Input iUserIndex   - user index
    \Input iInviteIndex - index of invite in pRef->aSessionUsers[iUserIndex].RecievedInvitations.aInvitations[]
    \Input pInviteId    - pointer to location to store the inviteId, so when the results arrive we can determine for which invite
    \Input pApiGroup    - string to populate indicating which group of sce webapi commands we're using
    \Input pApiPath     - string to populate indicating url path of the command
    \Input iStrSize     - length of the pApiGroup and pApiPath parameters
    \Input pContentParameter     - SceNpWebApiContentParameter, contains total content length and type
    \Input pHttpMethod   - SceNpWebApiHttpMethod, http method type eg. GET, PUT
    \Input pSendBuff    - buffer to accept the http request body
    \Input iBuffSize    - max size of the pSendBuff

    \Output
        int32_t         - >=0 success, <0 failure

    \Version 04/03/2013 (cvienneau)
*/
/*************************************************************************************F*/
static int32_t _DirtySessionManagerSetupUpdateInvite(DirtySessionManagerRefT *pRef, int32_t iUserIndex, int32_t iInviteIndex, void ** pInviteId, char *pApiGroup, char *pApiPath, int32_t iStrSize, SceNpWebApiContentParameter *pContentParameter, SceNpWebApiHttpMethod *pHttpMethod, char *pSendBuff, int32_t iBuffSize)
{
    int32_t iRet = -1;
    
    //check to be sure there is a valid invite at the index the user specified
    if ((iInviteIndex < pRef->aSessionUsers[iUserIndex].RecievedInvitations.iNumInvitations) && (iInviteIndex >= 0))
    {
        ds_memclr(pSendBuff, iBuffSize);
        ds_snzprintf(pApiPath, iStrSize, DIRTY_SESSION_WEBAPI_PATH_SESSION_USED_INVITE, pRef->aSessionUsers[iUserIndex].RecievedInvitations.aInvitations[iInviteIndex].strInvitationID);
        ds_snzprintf(pApiGroup, iStrSize, DIRTY_SESSION_WEBAPI_GROUP_SESSION);
        *pHttpMethod = SCE_NP_WEBAPI_HTTP_METHOD_PUT;
        pContentParameter->pContentType = "application/json; charset=utf-8";
        
        // create the Json body
        iRet = ds_snzprintf(pSendBuff, iBuffSize,
            "{\r\n"
            "  \"usedFlag\":\"true\"\r\n"
            "}\r\n");         
        pContentParameter->contentLength = iRet;

        //store the inviteId in the void pointer so the response has some idea what invite we're talking about
        *pInviteId = pRef->aSessionUsers[iUserIndex].RecievedInvitations.aInvitations[iInviteIndex].strInvitationID;
    }
    else
    {
        NetPrintf(("dirtysessionmanagerps4: [%p] _DirtySessionManagerSetupUpdateInvite() failed, invalid invite index.\n", pRef));
    }
    return(iRet);
}

/*F*************************************************************************************/
/*!
    \Function    _DirtySessionManagerSetupUpdateSession

    \Description
        Prepares a webapi request to update the basic session information.

    \Input *pRef        - pointer to module state
    \Input iUserIndex   - user index
    \Input pApiGroup    - string to populate indicating which group of sce webapi commands we're using
    \Input pApiPath     - string to populate indicating url path of the command
    \Input iStrSize     - length of the pApiGroup and pApiPath parameters
    \Input pContentParameter     - SceNpWebApiContentParameter, contains total content length and type
    \Input pHttpMethod   - SceNpWebApiHttpMethod, http method type eg. GET, PUT
    \Input pSendBuff    - buffer to accept the http request body
    \Input iBuffSize    - max size of the pSendBuff

    \Output
        int32_t         - >=0 success, <0 failure

    \Version 04/03/2013 (cvienneau)
*/
/*************************************************************************************F*/
static int32_t _DirtySessionManagerSetupUpdateSession(DirtySessionManagerRefT *pRef, int32_t iUserIndex, char *pApiGroup, char *pApiPath, int32_t iStrSize, SceNpWebApiContentParameter *pContentParameter, SceNpWebApiHttpMethod *pHttpMethod, char *pSendBuff, int32_t iBuffSize)
{
    const int32_t iTempBuffSize = 2048;
    char aTempLocNameJson[iTempBuffSize];
    char aTempLocStatusJson[iTempBuffSize];

    ds_memclr(pSendBuff, iBuffSize);
    ds_snzprintf(pApiPath, iStrSize, DIRTY_SESSION_WEBAPI_PATH_SESSION_UPDATE, pRef->PendingSession.strSessionID);
    ds_snzprintf(pApiGroup, iStrSize, DIRTY_SESSION_WEBAPI_GROUP_SESSION);
    *pHttpMethod = SCE_NP_WEBAPI_HTTP_METHOD_PUT;
    pContentParameter->pContentType = "application/json; charset=utf-8";

    // create the Json body
    int32_t iLength = ds_snzprintf(pSendBuff, iBuffSize,
        "{\r\n"
        "%s%s" //localized data is optional
        "  \"sessionName\":\"%s\",\r\n"
        "  \"sessionStatus\":\"%s\"\r\n" 
        "}\r\n",
        _DirtySessionManagerGetLocalizedJson(pRef, aTempLocNameJson, iTempBuffSize, TRUE),
        _DirtySessionManagerGetLocalizedJson(pRef, aTempLocStatusJson, iTempBuffSize, FALSE),
        pRef->PendingSession.strSessionName,
        pRef->PendingSession.strSessionStatus
        );
    
    pContentParameter->contentLength = iLength;
    return(iLength);
}

/*F*************************************************************************************/
/*!
    \Function    _DirtySessionManagerSetupWebRequest

    \Description
        Generates a web request for the supplied control command.

    \Input *pRef        - pointer to module state
    \Input iControlCode - four letter character code indicating what type of request is to be made
    \Input iUserIndex   - user index
    \Input iValue2      - user data
    \Input pValue       - user data
    \Input pRequestData - output void* which can be fetched during the response
    \Input pApiGroup    - string to populate indicating which group of sce webapi commands we're using
    \Input pApiPath     - string to populate indicating url path of the command
    \Input iStrSize     - length of the pApiGroup and pApiPath parameters
    \Input pContentParameter     - SceNpWebApiContentParameter, contains total content length and type
    \Input pHttpMethod   - SceNpWebApiHttpMethod, http method type eg. GET, PUT
    \Input pSendBuff    - buffer to accept the http request body
    \Input iBuffSize    - max size of the pSendBuff

    \Output
        int32_t         - >=0 success, <0 failure

    \Version 04/03/2013 (cvienneau)
*/
/*************************************************************************************F*/
static int32_t _DirtySessionManagerSetupWebRequest(DirtySessionManagerRefT *pRef, int32_t iControlCode, int32_t iUserIndex, int32_t iValue2, const void *pValue, void **pRequestData, char *pApiGroup, char *pApiPath, int32_t iStrSize, SceNpWebApiContentParameter *pContentParameter, SceNpWebApiHttpMethod *pHttpMethod, char *pSendBuff, int32_t iBuffSize)
{
    if (iControlCode == 'gchg')
    {
        return(_DirtySessionManagerSetupGetSessionChangeableData(pRef, iUserIndex, pApiGroup, pApiPath, iStrSize, pContentParameter, pHttpMethod, pSendBuff, iBuffSize));
    }
    if (iControlCode == 'gdat')
    {
        return(_DirtySessionManagerSetupGetSessionData(pRef, iUserIndex, pApiGroup, pApiPath, iStrSize, pContentParameter, pHttpMethod, pSendBuff, iBuffSize));
    }
    else if (iControlCode == 'ginv')
    {
        return(_DirtySessionManagerSetupGetInvites(pRef, iUserIndex, pApiGroup, pApiPath, iStrSize, pContentParameter, pHttpMethod, pSendBuff, iBuffSize));
    }
    else if (iControlCode == 'gimd')
    {
        return(_DirtySessionManagerSetupGetInviteMetadata(pRef, iUserIndex, iValue2, pRequestData, pApiGroup, pApiPath, iStrSize, pContentParameter, pHttpMethod, pSendBuff, iBuffSize));
    }    
    else if (iControlCode == 'gmse')
    {
        return(_DirtySessionManagerSetupGetMemberOfSessions(pRef, iUserIndex, pApiGroup, pApiPath, iStrSize, pContentParameter, pHttpMethod, pSendBuff, iBuffSize));
    }    
    else if (iControlCode == 'gses')
    {
        return(_DirtySessionManagerSetupGetSession(pRef, iUserIndex, pApiGroup, pApiPath, iStrSize, pContentParameter, pHttpMethod, pSendBuff, iBuffSize));
    }
    else if (iControlCode == 'lock')
    {
        return(_DirtySessionManagerSetupLockSession(pRef, iUserIndex, pApiGroup, pApiPath, iStrSize, pContentParameter, pHttpMethod, pSendBuff, iBuffSize));
    }
    else if (iControlCode == 'scre')
    {
        return(_DirtySessionManagerSetupCreateSession(pRef, iUserIndex, pApiGroup, pApiPath, iStrSize, pContentParameter, pHttpMethod, pSendBuff, iBuffSize));
    }
    else if (iControlCode == 'sdel')
    {
        return(_DirtySessionManagerSetupDeleteSession(pRef, iUserIndex, pApiGroup, pApiPath, iStrSize, pContentParameter, pHttpMethod, pSendBuff, iBuffSize));
    }
    else if (iControlCode == 'sind')
    {
        return(_DirtySessionManagerPostInvitation(pRef, iUserIndex, pApiGroup, pApiPath, iStrSize, pContentParameter, pHttpMethod, pSendBuff, iBuffSize));
    }
    else if (iControlCode == 'sjoi')
    {
        return(_DirtySessionManagerSetupJoinSession(pRef, iUserIndex, pApiGroup, pApiPath, iStrSize, pContentParameter, pHttpMethod, pSendBuff, iBuffSize));
    }
    else if (iControlCode == 'uimg')
    {
        return(_DirtySessionManagerSetupUpdateSessionImage(pRef, iUserIndex, pApiGroup, pApiPath, iStrSize, pContentParameter, pHttpMethod, pSendBuff, iBuffSize));
    }
    else if (iControlCode == 'uinv')
    {
        return(_DirtySessionManagerSetupUpdateInvite(pRef, iUserIndex, iValue2, pRequestData, pApiGroup, pApiPath, iStrSize, pContentParameter, pHttpMethod, pSendBuff, iBuffSize));
    }
    else if (iControlCode == 'umau')
    {
        return(_DirtySessionManagerSetupUpdateSessionMaxUsers(pRef, iUserIndex, pApiGroup, pApiPath, iStrSize, pContentParameter, pHttpMethod, pSendBuff, iBuffSize));
    }
    else if (iControlCode == 'uses')
    {
        return(_DirtySessionManagerSetupUpdateSession(pRef, iUserIndex, pApiGroup, pApiPath, iStrSize, pContentParameter, pHttpMethod, pSendBuff, iBuffSize));
    }   
    return(-1);
}

/*F*************************************************************************************/
/*!
    \Function    _DirtySessionManagerGetSessionChangeableDataResult

    \Description
        Parse response from web request and handle results

    \Input *pRef        - pointer to module state
    \Input iUserIndex   - user index
    \Input iStatus      - http status code of the response
    \Input pReadBuff    - response data to parse
    \Input iBuffSize    - number of bytes in response data

    \Output
        int32_t         - >=0 success, <0 failure

    \Version 08/18/2015 (mclouatre)
*/
/*************************************************************************************F*/
static int32_t _DirtySessionManagerGetSessionChangeableDataResult(DirtySessionManagerRefT *pRef, int32_t iUserIndex, int32_t iStatus, const char *pReadBuff, int32_t iBuffSize)
{
    int32_t iRet = 1;
    if (iStatus == 200)
    {
        if (iBuffSize >= (int32_t)sizeof(pRef->PendingSession.changeableBinaryDataHeader))
        {
            //strip off the header structure
            ds_memcpy(&pRef->PendingSession.changeableBinaryDataHeader, pReadBuff, sizeof(pRef->PendingSession.changeableBinaryDataHeader));

            //allocate a new buffer to store the rest of the data
            _DirtySessionManagerSetChangeableSessionData(pRef, &pRef->PendingSession, pReadBuff + sizeof(pRef->PendingSession.changeableBinaryDataHeader), iBuffSize - sizeof(pRef->PendingSession.changeableBinaryDataHeader));
        }
        else
        {
            NetPrintf(("dirtysessionmanagerps4: [%p] _DirtySessionManagerGetSessionChangeableDataResult data size isn't large enough to contain a DirtySessionManagerChangeableBinaryHeaderT size = %d\n", pRef, iBuffSize));
            iRet = -1;
        }
    }
    else
    {
        NetPrintf(("dirtysessionmanagerps4: [%p] _DirtySessionManagerGetSessionChangeableDataResult failed, status=%d\n", pRef, iStatus));
        iRet = -1;
    }
    return(iRet);
}

/*F*************************************************************************************/
/*!
    \Function    _DirtySessionManagerGetSessionDataResult

    \Description
        Parse response from web request and handle results

    \Input *pRef        - pointer to module state
    \Input iUserIndex   - user index
    \Input iStatus      - http status code of the response
    \Input pReadBuff    - response data to parse
    \Input iBuffSize    - number of bytes in response data

    \Output
        int32_t         - >=0 success, <0 failure

    \Version 04/03/2013 (cvienneau)
*/
/*************************************************************************************F*/
static int32_t _DirtySessionManagerGetSessionDataResult(DirtySessionManagerRefT *pRef, int32_t iUserIndex, int32_t iStatus, const char *pReadBuff, int32_t iBuffSize)
{
    int32_t iRet = 1;
    if (iStatus == 200)
    {
        if (iBuffSize >= (int32_t)sizeof(pRef->PendingSession.binaryDataHeader))
        {
            //strip off the header structure
            ds_memcpy(&pRef->PendingSession.binaryDataHeader, pReadBuff, sizeof(pRef->PendingSession.binaryDataHeader));
            
            //allocate a new buffer to store the rest of the data
            _DirtySessionManagerSetSessionData(pRef, &pRef->PendingSession, pReadBuff + sizeof(pRef->PendingSession.binaryDataHeader), iBuffSize - sizeof(pRef->PendingSession.binaryDataHeader));
        }
        else
        {
            NetPrintf(("dirtysessionmanagerps4: [%p] _DirtySessionManagerGetSessionDataResult data size isn't large enough to contain a DirtySessionManagerBinaryHeaderT size = %d\n", pRef, iBuffSize));
            iRet = -1;
        }
    }
    else
    {
        NetPrintf(("dirtysessionmanagerps4: [%p] _DirtySessionManagerGetSessionDataResult failed, status=%d\n", pRef, iStatus));
        iRet = -1;
    }
    return(iRet);
}

/*F*************************************************************************************/
/*!
    \Function    _DirtySessionManagerGetInvitesResult

    \Description
        Parse response from web request and handle results

    \Input *pRef        - pointer to module state
    \Input iUserIndex   - user index
    \Input iStatus      - http status code of the response
    \Input pReadBuff    - response data to parse
    \Input iBuffSize    - number of bytes in response data

    \Output
        int32_t         - >=0 success, <0 failure

    \Version 04/03/2013 (cvienneau)
*/
/*************************************************************************************F*/
static int32_t _DirtySessionManagerGetInvitesResult(DirtySessionManagerRefT *pRef, int32_t iUserIndex, int32_t iStatus, const char *pReadBuff, int32_t iBuffSize)
{
    NetPrintf(("dirtysessionmanagerps4: [%p] _DirtySessionManagerGetInvitesResult size=%d, data=%s\n", pRef, iBuffSize, pReadBuff));
    int32_t iRet = 0;
    if (iStatus == 200)
    {
        uint16_t aJsonParse[12*1024];
        char strInviteId[DIRTY_SESSION_INVITATION_ID_LENGTH];
        char strSessionId[DIRTY_SESSION_ID_LENGTH];
        char strFrom[20]; // max uint64_t length when a string
        const char *pCurrent = NULL;
        const char *pInviteList = NULL;
        uint8_t bUsedFlag;
        uint8_t bExpired ;
        int32_t iIncommingListIndex = 0;

        JsonParse(aJsonParse, sizeof(aJsonParse)/sizeof(aJsonParse[0]), pReadBuff, iBuffSize);
        ds_memclr(&pRef->aSessionUsers[iUserIndex].RecievedInvitations, sizeof(pRef->aSessionUsers[iUserIndex].RecievedInvitations));

        do
        {
            pInviteList = JsonFind2(aJsonParse, NULL, "invitations[", iIncommingListIndex);
            pCurrent = JsonFind2(aJsonParse, pInviteList, ".invitationId", iIncommingListIndex);
            if (pCurrent != NULL)
            {
                JsonGetString(pCurrent, strInviteId, sizeof(strInviteId), "");
            }
            else
            {
                break;
            }

            pCurrent = JsonFind2(aJsonParse, pInviteList, ".usedFlag", iIncommingListIndex);
            if (pCurrent != NULL)
            {
                bUsedFlag = JsonGetBoolean(pCurrent, TRUE);
            }
            else
            {
                break;
            }

            pCurrent = JsonFind2(aJsonParse, pInviteList, ".sessionId", iIncommingListIndex);
            if (pCurrent != NULL)
            {
                JsonGetString(pCurrent, strSessionId, sizeof(strSessionId), "");
            }
            else
            {
                break;
            }

            pCurrent = JsonFind2(aJsonParse, pInviteList, ".expired", iIncommingListIndex);
            if (pCurrent != NULL)
            {
                bExpired = JsonGetBoolean(pCurrent, TRUE);
            }
            else
            {
                break;
            }

            pCurrent = JsonFind2(aJsonParse, pInviteList, ".fromUser.accountId", iIncommingListIndex);
            if (pCurrent != NULL)
            {
                JsonGetString(pCurrent, strFrom, sizeof(strFrom), "");
            }
            else
            {
                break;
            }

            //this is a useful invite
            if((bUsedFlag == FALSE) && (bExpired == FALSE))
            {
                ds_strnzcpy(pRef->aSessionUsers[iUserIndex].RecievedInvitations.aInvitations[pRef->aSessionUsers[iUserIndex].RecievedInvitations.iNumInvitations].strSessionID, strSessionId, sizeof(pRef->aSessionUsers[iUserIndex].RecievedInvitations.aInvitations[pRef->aSessionUsers[iUserIndex].RecievedInvitations.iNumInvitations].strSessionID));
                pRef->aSessionUsers[iUserIndex].RecievedInvitations.aInvitations[pRef->aSessionUsers[iUserIndex].RecievedInvitations.iNumInvitations].FromUser = ds_strtoull(strFrom, NULL, 10);
                ds_strnzcpy(pRef->aSessionUsers[iUserIndex].RecievedInvitations.aInvitations[pRef->aSessionUsers[iUserIndex].RecievedInvitations.iNumInvitations].strInvitationID, strInviteId, sizeof(pRef->aSessionUsers[iUserIndex].RecievedInvitations.aInvitations[pRef->aSessionUsers[iUserIndex].RecievedInvitations.iNumInvitations].strInvitationID));
                pRef->aSessionUsers[iUserIndex].RecievedInvitations.iNumInvitations++;
            }

            iIncommingListIndex++;
        }
        while(pRef->aSessionUsers[iUserIndex].RecievedInvitations.iNumInvitations < DIRTY_SESSION_MAX_INVITATION_RECIEVED_LIST_LENGTH);
        iRet = pRef->aSessionUsers[iUserIndex].RecievedInvitations.iNumInvitations;
        pRef->aSessionUsers[iUserIndex].RecievedInvitations.bListUpToDate = TRUE;
        NetPrintf(("dirtysessionmanagerps4: [%p] player %d now has %d usable invites.\n", pRef, iUserIndex, iRet));
    }
    else
    {
        NetPrintf(("dirtysessionmanagerps4: [%p] _DirtySessionManagerGetInvitesResult failed, status=%d, response=%s\n", pRef, iStatus, pReadBuff));
        iRet = -1;
    }
    return(iRet);
}

/*F*************************************************************************************/
/*!
    \Function    _DirtySessionManagerGetInviteMetadataResult

    \Description
        Parse response from web request and handle results

    \Input *pRef        - pointer to module state
    \Input iUserIndex   - user index
    \Input pRequestData - pointer to the invitation id we are getting details on
    \Input iStatus      - http status code of the response
    \Input pReadBuff    - response data to parse
    \Input iBuffSize    - number of bytes in response data

    \Output
        int32_t         - >=0 success, <0 failure

    \Version 04/03/2013 (cvienneau)
*/
/*************************************************************************************F*/
static int32_t _DirtySessionManagerGetInviteMetadataResult(DirtySessionManagerRefT *pRef, int32_t iUserIndex, void *pRequestData, int32_t iStatus, const char *pReadBuff, int32_t iBuffSize)
{
    NetPrintf(("dirtysessionmanagerps4: [%p] _DirtySessionManagerGetInviteMetadataResult size=%d, data=%s\n", pRef, iBuffSize, pReadBuff));
    int32_t iRet = 0;
    if (iStatus == 200)
    {
        uint16_t aJsonParse[2048];
        const char *pCurrent = NULL;
        int32_t iInviteIndex;
        uint8_t bValidInvite = FALSE;
        JsonParse(aJsonParse, sizeof(aJsonParse)/sizeof(aJsonParse[0]), pReadBuff, iBuffSize);

        //loop through the invitations we have an find the matching one
        for (iInviteIndex = 0; iInviteIndex < pRef->aSessionUsers[iUserIndex].RecievedInvitations.iNumInvitations; iInviteIndex++)
        {
            if (strcmp(pRef->aSessionUsers[iUserIndex].RecievedInvitations.aInvitations[iInviteIndex].strInvitationID, (const char*)pRequestData) == 0)
            {
                bValidInvite = TRUE;
                break;
            }
        }

        if (bValidInvite == TRUE)
        {
            pCurrent = JsonFind(aJsonParse, "message");
            pRef->aSessionUsers[iUserIndex].RecievedInvitations.aInvitations[iInviteIndex].bHasMetaData = true;
            if (pCurrent != NULL)
            {
                JsonGetString(pCurrent, pRef->aSessionUsers[iUserIndex].RecievedInvitations.aInvitations[iInviteIndex].strMessage, sizeof(pRef->aSessionUsers[iUserIndex].RecievedInvitations.aInvitations[iInviteIndex].strMessage), "");
            }

            pCurrent = JsonFind(aJsonParse, "expired");
            if (pCurrent != NULL)
            {
                pRef->aSessionUsers[iUserIndex].RecievedInvitations.aInvitations[iInviteIndex].bExpired = JsonGetBoolean(pCurrent, false);
            }

            pCurrent = JsonFind(aJsonParse, "usedFlag");
            if (pCurrent != NULL)
            {
                pRef->aSessionUsers[iUserIndex].RecievedInvitations.aInvitations[iInviteIndex].bUsedFlag = JsonGetBoolean(pCurrent, false);
            }   
        
        }
        else
        {
            NetPrintf(("dirtysessionmanagerps4: [%p] _DirtySessionManagerGetInviteMetadataResult failed to find matching invitation %s\n", pRef, (const char*)pRequestData));
        }
    }
    else
    {
        NetPrintf(("dirtysessionmanagerps4: [%p] _DirtySessionManagerGetInviteMetadataResult failed, status=%d, response=%s\n", pRef, iStatus, pReadBuff));
        iRet = -1;
    }
    return(iRet);
}

/*F*************************************************************************************/
/*!
    \Function    _DirtySessionManagerGetMemberOfSessionsResult

    \Description
        Parse response from web request and handle results

    \Input *pRef        - pointer to module state
    \Input iUserIndex   - user index
    \Input iStatus      - http status code of the response
    \Input pReadBuff    - response data to parse
    \Input iBuffSize    - number of bytes in response data

    \Output
        int32_t         - >=0 success, <0 failure

    \Version 04/03/2013 (cvienneau)
*/
/*************************************************************************************F*/
static int32_t _DirtySessionManagerGetMemberOfSessionsResult(DirtySessionManagerRefT *pRef, int32_t iUserIndex, int32_t iStatus, const char *pReadBuff, int32_t iBuffSize)
{
    NetPrintf(("dirtysessionmanagerps4: [%p] _DirtySessionManagerGetMemberOfSessionsResult size=%d, data=%s\n", pRef, iBuffSize, pReadBuff));
    return(1);
}

/*F*************************************************************************************/
/*!
    \Function    _DirtySessionManagerGetSessionResult

    \Description
        Parse response from web request and handle results

    \Input *pRef        - pointer to module state
    \Input iUserIndex   - user index
    \Input iStatus      - http status code of the response
    \Input pReadBuff    - response data to parse
    \Input iBuffSize    - number of bytes in response data

    \Output
        int32_t         - >=0 success, <0 failure

    \Version 04/03/2013 (cvienneau)
*/
/*************************************************************************************F*/
static int32_t _DirtySessionManagerGetSessionResult(DirtySessionManagerRefT *pRef, int32_t iUserIndex, int32_t iStatus, const char *pReadBuff, int32_t iBuffSize)
{
    int32_t iRet = 1;
    if (iStatus == 200)
    {
        uint16_t aJsonParse[1024];
        JsonParse(aJsonParse, sizeof(aJsonParse)/sizeof(aJsonParse[0]), pReadBuff, iBuffSize);
        const char *pCurrent = NULL;

        pCurrent = JsonFind(aJsonParse, "sessionPrivacy");
        if (pCurrent != NULL)
        {
            char strTemp[32];
            JsonGetString(pCurrent, strTemp, sizeof(strTemp), "");
            if (ds_stricmp(strTemp, "private") == 0)
            {
                pRef->PendingSession.bPrivate = TRUE;
            }
            else
            {
                pRef->PendingSession.bPrivate = FALSE;
            }
        }

        pCurrent = JsonFind(aJsonParse, "sessionMaxUser");
        if (pCurrent != NULL)
        {
            pRef->PendingSession.iMaxUsers = JsonGetInteger(pCurrent, 0);
        }

        pCurrent = JsonFind(aJsonParse, "sessionName");
        if (pCurrent != NULL)
        {
            pRef->bPendingNameSet = TRUE;
            JsonGetString(pCurrent, pRef->PendingSession.strSessionName, sizeof(pRef->PendingSession.strSessionName), "");
        }

        pCurrent = JsonFind(aJsonParse, "sessionStatus");
        if (pCurrent != NULL)
        {
            pRef->bPendingStatusSet = TRUE;
            JsonGetString(pCurrent, pRef->PendingSession.strSessionStatus, sizeof(pRef->PendingSession.strSessionStatus), "");
        }

        pCurrent = JsonFind(aJsonParse, "joinableFlag");
        if (pCurrent != NULL)
        {
            pRef->PendingSession.bClosed = !JsonGetBoolean(pCurrent, FALSE);
        }
    }
    else if (iStatus == 404)
    {
        pRef->PendingSession.strSessionID[0] = '\0';
        NetPrintf(("dirtysessionmanagerps4: [%p] _DirtySessionManagerGetSessionResult session not found, status=%d, response=%s\n", pRef, iStatus, pReadBuff));
        iRet = -1;
    }
    else
    {
        NetPrintf(("dirtysessionmanagerps4: [%p] _DirtySessionManagerGetSessionResult failed, status=%d, response=%s\n", pRef, iStatus, pReadBuff));
        iRet = -1;
    }
    return(iRet);
}

/*F*************************************************************************************/
/*!
    \Function    _DirtySessionManagerCreateSessionResult

    \Description
        Parse response from web request and handle results

    \Input *pRef        - pointer to module state
    \Input iUserIndex   - user index
    \Input iStatus      - http status code of the response
    \Input pReadBuff    - response data to parse
    \Input iBuffSize    - number of bytes in response data

    \Output
        int32_t         - >=0 success, <0 failure

    \Version 04/03/2013 (cvienneau)
*/
/*************************************************************************************F*/
static int32_t _DirtySessionManagerCreateSessionResult(DirtySessionManagerRefT *pRef, int32_t iUserIndex, int32_t iStatus, const char *pReadBuff, int32_t iBuffSize)
{
    int32_t iRet = 1;
    if (iStatus == 200)
    {
        uint16_t aJsonParse[512];
        JsonParse(aJsonParse, sizeof(aJsonParse)/sizeof(aJsonParse[0]), pReadBuff, iBuffSize);
        const char *pCurrent = NULL;
        pCurrent = JsonFind(aJsonParse, "sessionId");
        if (pCurrent != NULL)
        {
            JsonGetString(pCurrent, pRef->PendingSession.strSessionID, sizeof(pRef->PendingSession.strSessionID), "");
            //verify we got something "meaningful"
            if (strlen(pRef->PendingSession.strSessionID) > 0)
            {
                _DirtySessionManagerActivatePendingSession(pRef);
                pRef->aSessionUsers[iUserIndex].bSessionMember = TRUE;
                NetPrintf(("dirtysessionmanagerps4: [%p] successfully created session: %s\n", pRef, pRef->ActiveSession.strSessionID));
                #if DIRTYCODE_LOGGING
                    SceNpOnlineId onlineId;
                    NetConnStatus('soid', iUserIndex, &onlineId, sizeof(onlineId));
                    NetPrintf(("dirtysessionmanagerps4: [%p] user %d:%s has successfully joined session: %s\n", pRef, iUserIndex, onlineId.data, pRef->ActiveSession.strSessionID));
                #endif
            }
            else
            {
                pRef->eSessionStatus = DIRTY_SESSION_INVALID;
            }
        }
        else
        {
            pRef->eSessionStatus = DIRTY_SESSION_INVALID;
        }
    }
    else
    {
        pRef->eSessionStatus = DIRTY_SESSION_INVALID;
    }

    if (pRef->eSessionStatus == DIRTY_SESSION_INVALID)
    {
        iRet = -1;
        NetPrintf(("dirtysessionmanagerps4: [%p] _DirtySessionManagerCreateSessionResult failed, status=%d, response=%s\n", pRef, iStatus, pReadBuff));
    }
    return(iRet);
}

/*F*************************************************************************************/
/*!
    \Function    _DirtySessionManagerDeleteSessionResult

    \Description
        Parse response from web request and handle results

    \Input *pRef        - pointer to module state
    \Input iUserIndex   - user index
    \Input iStatus      - http status code of the response
    \Input pReadBuff    - response data to parse
    \Input iBuffSize    - number of bytes in response data

    \Output
        int32_t         - >=0 success, <0 failure

    \Version 04/03/2013 (cvienneau)
*/
/*************************************************************************************F*/
static int32_t _DirtySessionManagerDeleteSessionResult(DirtySessionManagerRefT *pRef, int32_t iUserIndex, int32_t iStatus, const char *pReadBuff, int32_t iBuffSize)
{
    int32_t iRet = 0;

    if (iStatus != 204)
    {
        NetPrintf(("dirtysessionmanagerps4: [%p] _DirtySessionManagerDeleteSessionResult failed, status=%d, response=%s\n", pRef, iStatus, pReadBuff));
        iRet = -1;
    }

    if (_DirtySessionManagerCountSessionMembers(pRef) == 0)
    {
        NetPrintf(("dirtysessionmanagerps4: [%p] _DirtySessionManagerDeleteSessionResult, the last local user has left session %s, cleaning up.\n", pRef, pRef->ActiveSession.strSessionID));
        _DirtySessionManagerCleanupSession(pRef);
    }

    return(iRet);
}

/*F*************************************************************************************/
/*!
    \Function    _DirtySessionManagerJoinSessionResult

    \Description
        Parse response from web request and handle results

    \Input *pRef        - pointer to module state
    \Input iUserIndex   - user index
    \Input iStatus      - http status code of the response
    \Input pReadBuff    - response data to parse
    \Input iBuffSize    - number of bytes in response data

    \Output
        int32_t         - >=0 success, <0 failure

    \Version 04/03/2013 (cvienneau)
*/
/*************************************************************************************F*/
static int32_t _DirtySessionManagerJoinSessionResult(DirtySessionManagerRefT *pRef, int32_t iUserIndex, int32_t iStatus, const char *pReadBuff, int32_t iBuffSize)
{
    int32_t iRet = 1;
    if (iStatus == 204)
    {
        pRef->aSessionUsers[iUserIndex].bSessionMember = TRUE;
        if (pRef->eSessionStatus == DIRTY_SESSION_PENDING)
        {
            _DirtySessionManagerActivatePendingSession(pRef);
        }
        #if DIRTYCODE_LOGGING
            SceNpOnlineId onlineId;
            NetConnStatus('soid', iUserIndex, &onlineId, sizeof(onlineId));
            NetPrintf(("dirtysessionmanagerps4: [%p] user %d:%s has successfully joined session: %s\n", pRef, iUserIndex, onlineId.data, pRef->ActiveSession.strSessionID));
        #endif
    }
    else 
    {
        NetPrintf(("dirtysessionmanagerps4: [%p] _DirtySessionManagerJoinSessionResult failed, status=%d, response=%s\n", pRef, iStatus, pReadBuff));
        pRef->eSessionStatus = DIRTY_SESSION_INVALID;
        iRet = -1;
    }
    return(iRet);
}

/*F*************************************************************************************/
/*!
    \Function    _DirtySessionManagerUpdateLockSessionResult

    \Description
        Parse response from web request and handle results

    \Input *pRef        - pointer to module state
    \Input iUserIndex   - user index
    \Input iStatus      - http status code of the response
    \Input pReadBuff    - response data to parse
    \Input iBuffSize    - number of bytes in response data

    \Output
        int32_t         - >=0 success, <0 failure

    \Version 12/2/2014 (tcho)
*/
/*************************************************************************************F*/
static int32_t _DirtySessionManagerUpdateLockSessionResult(DirtySessionManagerRefT *pRef, int32_t iUserIndex, int32_t iStatus, const char *pReadBuff, int32_t iBuffSize)
{
    int32_t iRet = 1;
    if (iStatus == 204)
    {
        pRef->ActiveSession.bSessionLock = pRef->PendingSession.bSessionLock;
        NetPrintf(("dirtysessionmanagerps4: [%p] session successfully %s", pRef, pRef->ActiveSession.bSessionLock ? "locked" : "unlocked"));
    }
    else
    {
        iRet = -1;
        NetPrintf(("dirtysessionmanagerps4: [%p] _DirtySessionManagerUpdateLockSessionResult failed, status=%d, response=%s\n", pRef, iStatus, pReadBuff));
    }
    return(iRet);
}

/*F*************************************************************************************/
/*!
    \Function    _DirtySessionManagerUpdateSessionMaxUsersResult

    \Description
        Parse response from web request and handle results

    \Input *pRef        - pointer to module state
    \Input iUserIndex   - user index
    \Input iStatus      - http status code of the response
    \Input pReadBuff    - response data to parse
    \Input iBuffSize    - number of bytes in response data

    \Output
        int32_t         - >=0 success, <0 failure

    \Version 09/12/2013 (mclouatre)
*/
/*************************************************************************************F*/
static int32_t _DirtySessionManagerUpdateSessionMaxUsersResult(DirtySessionManagerRefT *pRef, int32_t iUserIndex, int32_t iStatus, const char *pReadBuff, int32_t iBuffSize)
{
    int32_t iRet = 1;
    if (iStatus == 204)
    {
        pRef->ActiveSession.iMaxUsers = pRef->PendingSession.iMaxUsers;
        NetPrintf(("dirtysessionmanagerps4: [%p] session capacity successfully updated to %d\n", pRef, pRef->ActiveSession.iMaxUsers));
    }
    else
    {
        iRet = -1;
        NetPrintf(("dirtysessionmanagerps4: [%p] _DirtySessionManagerUpdateSessionMaxUsersResult failed, status=%d, response=%s\n", pRef, iStatus, pReadBuff));
    }
    return(iRet);
}

/*F*************************************************************************************/
/*!
    \Function    _DirtySessionManagerUpdateSessionImageResult

    \Description
        Parse response from web request and handle results

    \Input *pRef        - pointer to module state
    \Input iUserIndex   - user index
    \Input iStatus      - http status code of the response
    \Input pReadBuff    - response data to parse
    \Input iBuffSize    - number of bytes in response data

    \Output
        int32_t         - >=0 success, <0 failure

    \Version 04/03/2013 (cvienneau)
*/
/*************************************************************************************F*/
static int32_t _DirtySessionManagerUpdateSessionImageResult(DirtySessionManagerRefT *pRef, int32_t iUserIndex, int32_t iStatus, const char *pReadBuff, int32_t iBuffSize)
{
    int32_t iRet = 1;
    if (iStatus == 204)
    {
        //just to clear image memory
        _DirtySessionManagerSetSessionImage(pRef, &pRef->PendingSession, NULL, 0);
    }
    else
    {
        iRet = -1;
        NetPrintf(("dirtysessionmanagerps4: [%p] _DirtySessionManagerUpdateSessionImageResult failed, status=%d, response=%s\n", pRef, iStatus, pReadBuff));
    }
    return(iRet);
}

/*F*************************************************************************************/
/*!
    \Function    _DirtySessionManagerUpdateInviteResult

    \Description
        Parse response from web request and handle results

    \Input *pRef        - pointer to module state
    \Input iUserIndex   - user index
    \Input pRequestData - pointer to the invitation id we have updated
    \Input iStatus      - http status code of the response
    \Input pReadBuff    - response data to parse
    \Input iBuffSize    - number of bytes in response data

    \Output
        int32_t         - >=0 success, <0 failure

    \Version 04/03/2013 (cvienneau)
*/
/*************************************************************************************F*/
static int32_t _DirtySessionManagerUpdateInviteResult(DirtySessionManagerRefT *pRef, int32_t iUserIndex, void *pRequestData, int32_t iStatus, const char *pReadBuff, int32_t iBuffSize)
{
    int32_t iRet = 1;
    if (iStatus == 204)
    {
        //since we know we just set an invite to used, update the in memory representation for it

        int32_t iInviteIndex;
        uint8_t bValidInvite = FALSE;
        //loop through the invitations we have to find the matching one
        for (iInviteIndex = 0; iInviteIndex < pRef->aSessionUsers[iUserIndex].RecievedInvitations.iNumInvitations; iInviteIndex++)
        {
            if (strcmp(pRef->aSessionUsers[iUserIndex].RecievedInvitations.aInvitations[iInviteIndex].strInvitationID, (const char*)pRequestData) == 0)
            {
                bValidInvite = TRUE;
                break;
            }
        }

        if (bValidInvite == TRUE)
        {
            pRef->aSessionUsers[iUserIndex].RecievedInvitations.aInvitations[iInviteIndex].bUsedFlag = TRUE;
        }
        else
        {
            NetPrintf(("dirtysessionmanagerps4: [%p] _DirtySessionManagerUpdateInviteResult failed to find matching invitation %s\n", pRef, (const char*)pRequestData));
        }

    }
    else
    {
        iRet = -1;
        NetPrintf(("dirtysessionmanagerps4: [%p] _DirtySessionManagerUpdateInviteResult failed, status=%d, response=%s\n", pRef, iStatus, pReadBuff));
    }
    return(iRet);
}

/*F*************************************************************************************/
/*!
    \Function    _DirtySessionManagerUpdateSessionResult

    \Description
        Parse response from web request and handle results

    \Input *pRef        - pointer to module state
    \Input iUserIndex   - user index
    \Input iStatus      - http status code of the response
    \Input pReadBuff    - response data to parse
    \Input iBuffSize    - number of bytes in response data

    \Output
        int32_t         - >=0 success, <0 failure

    \Version 04/03/2013 (cvienneau)
*/
/*************************************************************************************F*/
static int32_t _DirtySessionManagerUpdateSessionResult(DirtySessionManagerRefT *pRef, int32_t iUserIndex, int32_t iStatus, const char *pReadBuff, int32_t iBuffSize)
{
    int32_t iRet = 1;
    if (iStatus == 204)
    {
        //to move the changes into the active session
        _DirtySessionManagerActivatePendingSession(pRef);
    }
    else
    {
        iRet = -1;
        NetPrintf(("dirtysessionmanagerps4: [%p] _DirtySessionManagerUpdateSessionResult failed, status=%d, response=%s\n", pRef, iStatus, pReadBuff));
    }
    return(iRet);
}


/*F*************************************************************************************/
/*!
    \Function    _DirtySessionManagerWebRequestResult

    \Description
        Parse response from web request and handle results

    \Input *pRef        - pointer to module state
    \Input iControlCode - the control command that generated this result
    \Input iUserIndex   - user index
    \Input pRequestData - pointer to arbitrary data the request may have stored for the results to referance
    \Input iStatus      - http status code of the response
    \Input pReadBuff    - response data to parse
    \Input iBuffSize    - number of bytes in response data

    \Output
        int32_t         - >=0 success, <0 failure

    \Version 04/03/2013 (cvienneau)
*/
/*************************************************************************************F*/
static int32_t _DirtySessionManagerWebRequestResult(DirtySessionManagerRefT *pRef, int32_t iControlCode, int32_t iUserIndex, void *pRequestData, int32_t iStatus, const char *pReadBuff, int32_t iBuffSize)
{
    NetPrintf(("dirtysessionmanagerps4: [%p] _DirtySessionManagerWebRequestResult('%C'), user: %d, status: %d, bytes: %d\n", pRef, iControlCode, iUserIndex, iStatus, iBuffSize));

    if (iControlCode == 'gchg')
    {
        return(_DirtySessionManagerGetSessionChangeableDataResult(pRef, iUserIndex, iStatus, pReadBuff, iBuffSize));
    }
    if (iControlCode == 'gdat')
    {
        return(_DirtySessionManagerGetSessionDataResult(pRef, iUserIndex, iStatus, pReadBuff, iBuffSize));
    }
    else if (iControlCode == 'ginv')
    {
        return(_DirtySessionManagerGetInvitesResult(pRef, iUserIndex, iStatus, pReadBuff, iBuffSize));
    }      
    else if (iControlCode == 'gimd')
    {
        return(_DirtySessionManagerGetInviteMetadataResult(pRef, iUserIndex, pRequestData, iStatus, pReadBuff, iBuffSize));
    }      
    else if (iControlCode == 'gmse')
    {
        return(_DirtySessionManagerGetMemberOfSessionsResult(pRef, iUserIndex, iStatus, pReadBuff, iBuffSize));
    }      
    else if (iControlCode == 'gses')
    {
        return(_DirtySessionManagerGetSessionResult(pRef, iUserIndex, iStatus, pReadBuff, iBuffSize));
    }
    else if (iControlCode == 'lock')
    {
        return(_DirtySessionManagerUpdateLockSessionResult(pRef, iUserIndex, iStatus, pReadBuff, iBuffSize));
    }
    else if (iControlCode == 'scre')
    {
        return(_DirtySessionManagerCreateSessionResult(pRef, iUserIndex, iStatus, pReadBuff, iBuffSize));
    }
    else if (iControlCode == 'sdel')
    {
        return(_DirtySessionManagerDeleteSessionResult(pRef, iUserIndex, iStatus, pReadBuff, iBuffSize));
    }    
    else if (iControlCode == 'sjoi')
    {
        return(_DirtySessionManagerJoinSessionResult(pRef, iUserIndex, iStatus, pReadBuff, iBuffSize));
    }
    else if (iControlCode == 'uimg')
    {
        return(_DirtySessionManagerUpdateSessionImageResult(pRef, iUserIndex, iStatus, pReadBuff, iBuffSize));
    }    
    else if (iControlCode == 'uinv')
    {
        return(_DirtySessionManagerUpdateInviteResult(pRef, iUserIndex, pRequestData, iStatus, pReadBuff, iBuffSize));
    }
    else if (iControlCode == 'umau')
    {
        return(_DirtySessionManagerUpdateSessionMaxUsersResult(pRef, iUserIndex, iStatus, pReadBuff, iBuffSize));
    }    
    else if (iControlCode == 'uses')
    {
        return(_DirtySessionManagerUpdateSessionResult(pRef, iUserIndex, iStatus, pReadBuff, iBuffSize));
    }
    return(-1);
}

/*** Public Functions ******************************************************************/

/*F*************************************************************************************************/
/*!
    \Function DirtySessionManagerCreateSess

    \Description
        (Unused) Create a PS4 session.

    \Input *pRef        - pointer to module state
    \Input bRanked      - TRUE if ranked, else FALSE
    \Input uUserFlags   - User flags such as slot type, DIRTYSESSIONMANAGER_FLAG_PUBLICSLOT | DIRTYSESSIONMANAGER_FLAG_PRIVATESLOT
    \Input *pSession    - NULL if creating new session, pointer to session string if joining
    \Input *pLocalAddrs - Array of local address to join-in. Controller-index based. 0 if player is not joining

    \Output
        int32_t         - zero=success, negative=failure

    \Version 04/03/2013 (cvienneau)
*/
/*************************************************************************************************F*/
int32_t DirtySessionManagerCreateSess(DirtySessionManagerRefT *pRef, uint32_t bRanked, uint32_t *uUserFlags, const char *pSession, DirtyAddrT *pLocalAddrs)
{
    if (pRef->bServerDrivenModeEnabled)
    {
        NetPrintf(("dirtysessionmanagerps4: [%p] DirtySessionManagerCreateSess() ignored because server-driven operational mode enabled\n", pRef));
        return(-1);
    }

    return(0);
}


/*F*************************************************************************************************/
/*!
    \Function DirtySessionManagerEncodeSession

    \Description
        Encode the pending sessions binary header and data and write it to pBuff if there is sufficient space

    \Input *pBuffer         - [out] pointer to buffer for session string
    \Input iBufSize         - size of output buffer
    \Input *pSessionInfo0   - DirtySessionManagerRefT

    \Version 04/03/2013 (cvienneau)
*/
/*************************************************************************************************F*/
void DirtySessionManagerEncodeSession(char *pBuffer, int32_t iBufSize, const void *pSessionInfo0)
{
    DirtySessionManagerRefT *pRef = (DirtySessionManagerRefT*)pSessionInfo0;
    if (iBufSize >= ((int32_t)sizeof(pRef->PendingSession.binaryDataHeader) + pRef->PendingSession.iBinaryDataSize))
    {
        ds_memcpy(pBuffer, &pRef->PendingSession.binaryDataHeader, sizeof(pRef->PendingSession.binaryDataHeader));
        ds_memcpy(pBuffer + sizeof(pRef->PendingSession.binaryDataHeader), &pRef->PendingSession.pBinaryData, pRef->PendingSession.iBinaryDataSize);
    }
}

/*F*************************************************************************************************/
/*!
    \Function DirtySessionManagerDecodeSession

    \Description
        unused, requires buffer size param to be useful

    \Input *pSessionInfo0   - DirtySessionManagerRefT
    \Input *pBuffer         - pointer to incomming data

    \Version 04/03/2013 (cvienneau)
*/
/*************************************************************************************************F*/
void DirtySessionManagerDecodeSession(void *pSessionInfo0, const char *pBuffer)
{
}

/*F*************************************************************************************************/
/*!
    \Function    DirtySessionManagerUpdate

    \Description
        (Unused) Update module state

    \Input *pRef    - module state

    \Version 04/03/2013 (cvienneau)
*/
/*************************************************************************************************F*/
void DirtySessionManagerUpdate(DirtySessionManagerRefT *pRef)
{
    int32_t iRet = 0;

    // check to see if we're online, then setup the webapi events
    if (pRef->bInitEvents == FALSE)
    {
        int32_t status = NetConnStatus('conn', 0, NULL, 0);
        if (status == '+onl')
        {
            pRef->bInitEvents = TRUE;
            // Register for session webapi events, so we get notified when an invite arrives
            DirtyWebApiAddPushEventListenerEx(pRef->pWebApi, "np:service:invitation", "sessionInvitation", SCE_NP_DEFAULT_SERVICE_LABEL, _DirtySessionManagerOnWebApiSessionEvent, pRef);
        }
    }

    NetCritEnter(&pRef->crit);

    //service web api
    if(pRef->ResponseData.iControlCode != 0)
    {
        _DirtySessionManagerWebRequestResult(pRef, pRef->ResponseData.iControlCode, pRef->ResponseData.iUserIndex, pRef->ResponseData.pRequestData, pRef->ResponseData.iStatusCode, pRef->ResponseData.pRespBody, pRef->ResponseData.iRespBodyLength);
        ds_memclr(&pRef->ResponseData, sizeof(pRef->ResponseData));
        --pRef->iActiveRequests;
    }

    //service invite dialog screens
    if (pRef->bInvDialog)
    {
        SceCommonDialogStatus status = sceInvitationDialogUpdateStatus();

        if (status == SCE_COMMON_DIALOG_STATUS_FINISHED)
        {
            pRef->bInvDialog = FALSE;
            SceInvitationDialogResultA dialogResult;
            SceInvitationDialogUserList sentOnlineIdList;
            ds_memclr(&sentOnlineIdList, sizeof(SceInvitationDialogUserList));
            ds_memclr(&dialogResult, sizeof(SceInvitationDialogResultA) );
            dialogResult.sentUsers = &sentOnlineIdList;

            if ((iRet = sceInvitationDialogGetResultA(&dialogResult)) == SCE_OK)
            {    
                NetPrintf(("dirtysessionmanagerps4: [%p] sceInvitationDialogGetResult result = %s, errorCode = %d, sentUsersCount = %d\n", pRef, DirtyErrGetName(dialogResult.result), dialogResult.errorCode, dialogResult.sentUsers->count));
            
                if (dialogResult.sentUsers->count > 0)
                {
                    for (uint32_t i = 0; i < dialogResult.sentUsers->count; i++)
                    {
                        NetPrintf(("dirtysessionmanagerps4: [%p] Invite AccountId = [%llu]\n", pRef, dialogResult.sentUsers->users->accountId));
                    }
                }                
            }
            else
            {
                NetPrintf(("dirtysessionmanagerps4: [%p] sceInvitationDialogGetResult failed, error = %s\n", pRef, DirtyErrGetName(iRet)));
            }
            if ((pRef->bInvDialogUserManagedInit == FALSE) && (iRet = sceInvitationDialogTerminate()) != SCE_OK)
            {
                NetPrintf(("dirtysessionmanagerps4: [%p] sceInvitationDialogTerm failed, error = %s\n", pRef, DirtyErrGetName(iRet)));
            }
        }
        else if (SCE_COMMON_DIALOG_STATUS_RUNNING != status)
        {
            pRef->bInvDialog = FALSE;
            NetPrintf(("dirtysessionmanagerps4: [%p] sceInvitationDialog closed unexpectedly %d\n", pRef, status));

            if ((pRef->bInvDialogUserManagedInit == FALSE) && (iRet = sceInvitationDialogTerminate()) != SCE_OK)
            {
                NetPrintf(("dirtysessionmanagerps4: [%p] sceInvitationDialogTerm failed, error = %s\n", pRef, DirtyErrGetName(iRet)));
            }
        }

    }
    NetCritLeave(&pRef->crit);
}

/*F*************************************************************************************************/
/*!
    \Function    DirtySessionManagerControl

    \Description
        DirtySessionManager control function.

    \Input *pRef    - module ref
    \Input iControl - control selector
    \Input iValue   - selector specific
    \Input iValue2  - selector specific
    \Input *pValue  - selector specific

    \Output
        int32_t     - 0=success; negative=error

    \Notes
        iControl can be one of the following:

        \verbatim
            'abrt' - abort all active requests
            'decb' - decode the supplied buffer into the pending sessions binary header and data
            'csid' - force close the session invitation dialog
            'cssi' - clear the selected session id from the invite dialog, for after reading the session from 'gssi'
            'gchg' - queue getting the changeable binary data into the pending session
            'gdat' - queue getting the binary data into the pending session
            'ginv' - queue getting of session invitation list for player index iValue 
            'gimd' - queue getting of meta data for invitation for player index iValue, invite index iValue2
            'gmse' - debug to print the sessions this player is a member of for player index iValue 
            'gses' - queue getting the provided session (via pValue=sessionid) into the pending session
            'ianp' - add a user account id to list of ids to invite (only usable when not editable); via pValue
            'icnp' - clear the list of invitation np ids.
            'imus' - set the number of users for the session invite, via iValue
            'imsg' - set the session invitation message; via pValue
            'iued' - set if the invited users are editable once dispalyed; iValue as a bool
            'osid' - open session invitation dialog (using data from 'ianp', 'imus', 'imsg', 'iued', 'inpi'); iValue inviters user index
            'osrd' - open session receive dialog for player index iValue 
            'rbuf' - set the buffer size to receive data into, may need to be large for binary data or large invite lists (sony says 100 max), via iValue
            'rqbs' - set the buffer size to generate requests into (sendbuff), may need to be large for images or binary data, via iValue
            'lock' - locks or unlocks the pending session. Set iValue to TRUE to lock, FALSE to unlock
            'scre' - queue create a new session with the current settings, all users on the console will autoamatically join the same session
            'sdat' - set the pending session's binary data; pValue incomming data, iValue2 data size
            'sdel' - queue remove all players from the current active session
            'serv' - turn on server-driven session - in that mode, this client tech no longer honors any client request for operations that are now owned by the server
            'sess' - set session identifier (server-driven mode only)
            'sidi' - call sceInvitationDialogInitialize, and take responsibility for calling 'sidt'
            'sidt' - call sceInvitationDialogTerminate, and relinquish responsibility for calling 'sidt'
            'sind' - send invitations without first party dialog
            'simg' - set the pending session's image; pValue incomming data, iValue2 data size
            'siud' - set the bool indicating that the invite list is up to date, in case you want to ignore this update but want to detect future ones, for player index iValue 
            'sjoi' - queue to have the user at index (iValue) join the provided session id (pValue)
            'slid' - set the pending session lobby id; int64 via pValue, iValue2 sizeof(int64_t)
            'smau' - set the pending session's maximum number of users; iValue2 is new max 0-256
            'snam' - set the pending session's name; pValue incomming string, iValue2 local code ie 'enUS' 0 for unlocalized
            'spgm' - set the pending session game mode; int32 via pValue, iValue2 sizeof(int32_t)
            'spgt' - set the pending session game type; int64 via pValue, iValue2 sizeof(int64_t)
            'spri' - set the pending session's privacy setting; iValue2 == 1 private (default public) 
            'ssta' - set the pending session's status; pValue incomming string, iValue2 local code ie 'enUS' 0 for unlocalized
            'surm' - remove the user specified by the user index (iValue) from the session
            'uimg' - queue updating the active image data from the pending session
            'uinv' - queue updating the invite usedFlag to TRUE for player index iValue, invite index iValue2
            'umau' - queue updating the active session max user attribute from the pending session
            'uses' - queue updating the active session from pending session
        \endverbatim

    \Version 04/03/2013 (cvienneau)
*/
/*************************************************************************************************F*/
int32_t DirtySessionManagerControl(DirtySessionManagerRefT *pRef, int32_t iControl, int32_t iValue, int32_t iValue2, const void *pValue)
{
    int32_t iRet = -1;
    NetPrintf(("dirtysessionmanagerps4: [%p] DirtySessionManagerControl('%C'), (%d, %d)\n", pRef, iControl, iValue, iValue2));

    //'abrt' - abort all active requests
    if (iControl == 'abrt')
    {
        iRet = DirtyWebApiAbortRequests(pRef->pWebApi, -1);
        NetCritEnter(&pRef->crit);
        pRef->iActiveRequests = 0;
        NetCritLeave(&pRef->crit);
        return(iRet);
    }
    //'decb' - decode the supplied buffer into the pending sessions binary header and data
    else if (iControl == 'decb')
    {
        if (iValue2 >= (int32_t)sizeof(pRef->PendingSession.binaryDataHeader))
        {
            //strip off the header structure
            ds_memcpy(&pRef->PendingSession.binaryDataHeader, pValue, sizeof(pRef->PendingSession.binaryDataHeader));
            
            //allocate a new buffer to store the rest of the data
            _DirtySessionManagerSetSessionData(pRef, &pRef->PendingSession, (char *)pValue + sizeof(pRef->PendingSession.binaryDataHeader), iValue2 - sizeof(pRef->PendingSession.binaryDataHeader));
            iRet = 1;
        }
        else
        {
            NetPrintf(("dirtysessionmanagerps4: [%p] 'decb' data size isn't large enough to contain a DirtySessionManagerBinaryHeaderT size = %d\n", pRef, iValue2));
        }
        return(iRet);
    }
    //'csid' - force close the session invitation dialog
    else if (iControl == 'csid')
    {
        if (pRef->bInvDialog)
        {
            if ((iRet = sceInvitationDialogClose()) != SCE_OK)
            {
                NetPrintf(("dirtysessionmanagerps4: [%p] sceInvitationDialogClose failed, error = %s\n", pRef, DirtyErrGetName(iRet)));
            }
        }
        else
        {
            NetPrintf(("dirtysessionmanagerps4: [%p] 'cind' close invite dialog failed, we haven't opend the invite dialog.\n", pRef));
        }
        return(iRet);
    }
    //'cssi' - clear the selected session id from the invite dialog, for after reading the session from 'gssi'
    else if (iControl == 'cssi')
    {
        pRef->strSelectedSessionID[0] = '\0';
        pRef->strSelectedInviteID[0] = '\0';
        return(1);
    }
    //'gchg' - queue getting the changeable binary data for the pending session
    else if (iControl == 'gchg')
    {
        return(_DirtySessionManagerEnqueueControlCode(pRef, iControl, iValue, iValue2, pValue));
    }
    //'gdat' - queue getting the binary data for the pending session
    else if (iControl == 'gdat')
    {
        return(_DirtySessionManagerEnqueueControlCode(pRef, iControl, iValue, iValue2, pValue));
    }
    //'ginv' - queue getting of session invitation list for player index iValue 
    else if (iControl == 'ginv')
    {
        return(_DirtySessionManagerEnqueueControlCode(pRef, iControl, iValue, iValue2, pValue));
    }
    //'gimd' - queue getting of meta data for invitation for player index iValue, invite index iValue2
    else if (iControl == 'gimd')
    {
        return(_DirtySessionManagerEnqueueControlCode(pRef, iControl, iValue, iValue2, pValue));
    }    
    //'gmse' - debug to print the sessions this player is a member of
    else if (iControl == 'gmse')
    {
        return(_DirtySessionManagerEnqueueControlCode(pRef, iControl, iValue, iValue2, pValue));
    } 
    //'gses' - queue getting the provided session (via pValue=sessionid) into the pending session
    else if (iControl == 'gses')
    {
        if (pValue != NULL && (ds_stricmp((char *)pValue, "INVALID") != 0))
        {
            ds_strnzcpy(pRef->PendingSession.strSessionID, (char *)pValue, sizeof(pRef->PendingSession.strSessionID));
            iRet = _DirtySessionManagerEnqueueControlCode(pRef, iControl, iValue, iValue2, pValue);
        }
        else
        {
            NetPrintf(("dirtysessionmanagerps4: [%p] 'gses' valid session id required.\n", pRef));
        }
        return(iRet);
    }
    //'ianp' - add a user name to list of npids to invite (only usable when not editable)
    else if (iControl == 'ianp')
    {
        int32_t index;
        SceNpAccountId accountId = *((SceNpAccountId*)pValue);
        for (index = 0; index < DIRTY_SESSION_MAX_INVITATION_MEMBER_LIST_LENGTH; index++)
        {
            //if we found the name the user is attempting to add, then ignore the request
            if (pRef->InvitationInfo.aAccountIds[index] == accountId)
            {
                iRet = 0;
                break;
            }

            //if we found a blank spot insert the name into the list
            if (pRef->InvitationInfo.aAccountIds[index] == 0)
            {
                iRet = 1;
                pRef->InvitationInfo.aAccountIds[index] = accountId;
                break;
            }
        }
        if (index == DIRTY_SESSION_MAX_INVITATION_MEMBER_LIST_LENGTH)
        {
            NetPrintf(("dirtysessionmanagerps4: [%p] 'ianp' could not add npid, out of space.\n", pRef));
        }
        return(iRet);
    }
    //'icnp' - clear the list of invitation np ids.
    else if (iControl == 'icnp')
    {    
        ds_memclr(pRef->InvitationInfo.aAccountIds, sizeof(pRef->InvitationInfo.aAccountIds));
        return(1);
    }
    //'imus' - set the number of users for the session
    else if (iControl == 'imus')
    {
        pRef->InvitationInfo.iMaxUsers = iValue;
        return(1);
    }
    //'imsg' - set the session invitation message
    else if (iControl == 'imsg')
    {    
        ds_strnzcpy(pRef->InvitationInfo.strMessage, (const char *)pValue, sizeof(pRef->InvitationInfo.strMessage));
        return(1);
    }
    //'iued' - set if the invited users are editable once displayed; iValue as a bool
    else if (iControl == 'iued')
    {    
        pRef->InvitationInfo.bEditList = iValue;
        return(1);
    }
    //'osid' - open session invitation dialog (using data from 'imus', 'imsg', 'iued')
    else if (iControl == 'osid')
    {
        return(_DirtySessionManagerShowInviteDialog(pRef, iValue));
    }
    //'osrd' - open session receive dialog
    else if (iControl == 'osrd')
    {
        return(_DirtySessionManagerShowReceiveDialog(pRef, iValue));
    }
    //'rbuf' - set the buffer size to receive data into, may need to be large for binary data or large invite lists (sony says 100 max) 
    else if (iControl == 'rbuf')
    {
        DirtyWebApiControl(pRef->pWebApi, 'rbuf', iValue, 0, NULL);
        return(1);
    }
    //'rqbs' - set the buffer size to generate requests into, may need to be large for images or binary data 
    else if (iControl == 'rqbs')
    {
        pRef->iRequestBuffSize = iValue;
        return(1);
    }
    //'serv' - turn on server-driven mode
    else if (iControl == 'serv')
    {
        pRef->bServerDrivenModeEnabled = (uint8_t)iValue;
        NetPrintf(("dirtysessionmanagerps4: [%p] server-driven mode %s\n", pRef, pRef->bServerDrivenModeEnabled ? "enabled" : "disabled"));
        return(1);
    }
    //'sess' - set session id (only for server-driven mode)
    else if (iControl == 'sess')
    {
        if (pRef->bServerDrivenModeEnabled)
        {
            NetPrintf(("dirtysessionmanagerps4: [%p] session ID set to %s while server-driven mode is enabled\n", pRef, pValue));
            ds_strnzcpy(pRef->ActiveSession.strSessionID, (char *)pValue, sizeof(pRef->ActiveSession.strSessionID));
            pRef->eSessionStatus = ((pRef->ActiveSession.strSessionID[0] != '\0')? DIRTY_SESSION_ACTIVE : DIRTY_SESSION_INVALID);
            iRet = 1;
        }
        else
        {
            NetPrintf(("dirtysessionmanagerps4: [%p] DirtySessionManagerControl('sess') can't be used if server-driven mode is not enabled\n", pRef, pValue));
            iRet = -1;
        }
        return(iRet);
    }
    //'sidi' - call sceInvitationDialogInitialize, and take responsibility for calling 'sidt'
    else if (iControl == 'sidi')
    {
        //prepare the ability to show the invitation/receive dialog
        iRet = _DirtySessionManagerInitDialog(pRef);
        pRef->bInvDialogUserManagedInit = TRUE;
        return(iRet);
    }
    //'sind' - send an invitation without first party dialog
    else if (iControl == 'sind')
    {
        return(_DirtySessionManagerEnqueueControlCode(pRef, 'sind', 0, 0, NULL));
    }
    //'sidt' - call sceInvitationDialogTerminate, and relinquish responsibility for calling 'sidt'
    else if (iControl == 'sidt')
    {
        if ((iRet = sceInvitationDialogTerminate()) != SCE_OK)
        {
            NetPrintf(("dirtysessionmanagerps4: [%p] sceInvitationDialogTerm 'sidt' failed, error = %s\n", pRef, DirtyErrGetName(iRet)));
        }
        pRef->bInvDialogUserManagedInit = FALSE;
        return(iRet);
    }
    //'siud' - set the bool indicating that the invite list is up to date, in case you want to ignore this update but want to hear about future ones, for player index iValue 
    else if (iControl == 'siud')
    {
        if ((iValue >= 0) && (iValue < NETCONN_MAXLOCALUSERS))
        {
            iRet = pRef->aSessionUsers[iValue].RecievedInvitations.bListUpToDate = TRUE;
        }
        return(iRet);
    }
    //'uinv' - queue updating the invite usedFlag to TRUE for player index iValue, invite index iValue2
    else if (iControl == 'uinv')
    {
        return(_DirtySessionManagerEnqueueControlCode(pRef, iControl, iValue, iValue2, pValue));
    }

    // make sure that the below controls are only handled when server-driven operation mode is disabled
    if (pRef->bServerDrivenModeEnabled)
    {
        NetPrintf(("dirtysessionmanagerps4: [%p] DirtySessionManagerControl('%C') ignored becaused server-driven operation mode enabled\n", pRef, iControl));
        return(iRet);
    }

    // locks or unlocks the current active session
    if (iControl == 'lock')
    {
        pRef->PendingSession.bSessionLock = iValue ? TRUE : FALSE;
        return(_DirtySessionManagerEnqueueControlCode(pRef, iControl, 0, 0, NULL)); 
    }
    //'scre' - queue create a new session with the current settings
    else if (iControl == 'scre')
    {
        return(_DirtySessionManagerEnqueueControlCode(pRef, iControl, iValue, iValue2, pValue));
    }
    //'sdat' - set the pending session's binary data; pValue incomming data, iValue2 data size
    else if (iControl == 'sdat')
    {
        _DirtySessionManagerSetSessionData(pRef, &pRef->PendingSession, pValue, iValue2);
        return(1);
    }
    //'sdel' - queue remove all players from the current active session
    else if (iControl == 'sdel')
    {
        // remove all players who are part of the session from it
        for (int32_t iIndex = 0; iIndex < NETCONN_MAXLOCALUSERS; iIndex++)
        {
            if (pRef->aSessionUsers[iIndex].bSessionMember)
            {
                if (_DirtySessionManagerEnqueueControlCode(pRef, iControl, iIndex, 0, NULL) < 0)
                {
                    NetPrintf(("dirtysessionmanagerps4: [%p] 'sdel' Warning failed to remove local user at index %i", pRef, iIndex));
                }
            }
        }
        return(1);
    }
    //'simg' - set the pending session's image; pValue incomming data, iValue2 data size
    else if (iControl == 'simg')
    {
        _DirtySessionManagerSetSessionImage(pRef, &pRef->PendingSession, pValue, iValue2);
        return(1);
    }
    //'sjoi' - queue to have the user at index (iValue) join the provided session id (pValue)
    else if (iControl == 'sjoi')
    {
        uint8_t bGetSessionInfo = FALSE;
        iRet = 1;

        if ((pValue != NULL) && (ds_stricmp((char *)pValue, "INVALID") != 0))
        {
            //if the pending session id, matches what has been passed in, we would have already fetched the info
            if (ds_strnicmp(pRef->PendingSession.strSessionID, (char *)pValue, sizeof(pRef->PendingSession.strSessionID)) != 0)
            {
                ds_strnzcpy(pRef->PendingSession.strSessionID, (char *)pValue, sizeof(pRef->PendingSession.strSessionID));
                bGetSessionInfo = TRUE;
            }
        }
        else
        {
            NetPrintf(("dirtysessionmanagerps4: [%p] Skipping join of INVALID session id.\n", pRef));
            iRet = -1;
        }

        if (iRet != -1)
        {
            if ((iValue >= (0)) && (iValue < NETCONN_MAXLOCALUSERS))
            {
               SceNpOnlineId onlineId;
                if ((NetConnStatus('soid', iValue, &onlineId, sizeof(onlineId)) >= 0) && (pRef->aSessionUsers[iValue].bSessionMember == FALSE))
                {
                    if (bGetSessionInfo)
                    {
                       iRet = _DirtySessionManagerEnqueueControlCode(pRef, 'gses', iValue, 0, NULL); //first you need the basic session information
                       iRet = _DirtySessionManagerEnqueueControlCode(pRef, 'gdat', iValue, 0, NULL); //then you need the binary blob
                    }
                    //let the player join
                    iRet = _DirtySessionManagerEnqueueControlCode(pRef, iControl, iValue, 0, NULL);
                }
            }
            else
            {
                NetPrintf(("dirtysessionmanagerps4: [%p] Invalid local user index. Skipping Join", pRef));
                iRet = -2;
            }
        }
        return(iRet);
    }
    //'slid' - set the pending session lobby id; int64 via pValue, iValue2 sizeof(int64_t)
    else if (iControl == 'slid')
    {
        if (iValue2 == sizeof(int64_t) && pValue != NULL)
        {
            ds_memcpy(&pRef->PendingSession.binaryDataHeader.iLobbyId, pValue, sizeof(int64_t));
            iRet = 1;
        }
        else
        {
            iRet = -1;
        }
        return(iRet);
    }
    //'smau' - set the pending session's maximum number of users; iValue2 is new max 0-256
    else if (iControl == 'smau')
    {
        pRef->PendingSession.iMaxUsers = iValue2;
        return(1);
    }
    //'snam' - set the pending session's name; pValue incomming string
    else if (iControl == 'snam')
    {
        return(_DirtySessionManagerSetSessionString(pRef, (const char *)pValue, iValue2, TRUE));
    }
    //'spgm' - set the pending session game mode; int32 via pValue, iValue2 sizeof(int32_t)
    else if (iControl == 'spgm')
    {
        if (pValue != NULL)
        {
            ds_strnzcpy(pRef->PendingSession.changeableBinaryDataHeader.strGameMode, (char *)pValue, sizeof(pRef->PendingSession.changeableBinaryDataHeader.strGameMode));
            NetPrintf(("dirtysessionmanagerps4: [%p] pending session game mode set to %s\n", pRef, pRef->PendingSession.changeableBinaryDataHeader.strGameMode));
            iRet = 1;
        }
        else
        {
            iRet = -1;
        }
        return(iRet);
    }
    //'spgt' - set the pending session game type; int64 via pValue, iValue2 sizeof(int64_t)
    else if (iControl == 'spgt')
    {
        if (iValue2 == sizeof(int32_t) && pValue != NULL)
        {
            ds_memcpy(&pRef->PendingSession.binaryDataHeader.iGameType, pValue, sizeof(int64_t));
            NetPrintf(("dirtysessionmanagerps4: [%p] pending session game type set to %llu\n", pRef, pRef->PendingSession.binaryDataHeader.iGameType));
            iRet = 1;
        }
        else
        {
            iRet = -1;
        }
        return(iRet);
    }
    //'spri' - set the pending session's privacy setting; iValue2 == 1 private (default public) 
    else if (iControl == 'spri')
    {
        pRef->PendingSession.bPrivate = iValue2;
        return(1);
    }
    //'ssta' - set the pending session's status; pValue incomming string
    else if (iControl == 'ssta')
    {
        return(_DirtySessionManagerSetSessionString(pRef, (const char *)pValue, iValue2, FALSE)); 
    }
    //'surm' - remove player specified by index from session
    else if (iControl == 'surm')
    {
        // remove all players who are part of the session from it
        if ((iValue >= 0) && (iValue < NETCONN_MAXLOCALUSERS))
        {
            if (pRef->aSessionUsers[iValue].bSessionMember)
            {
                iRet = _DirtySessionManagerEnqueueControlCode(pRef, 'sdel', iValue, 0, NULL);
            }
            else
            {
                NetPrintf(("dirtysessionmanagerps4: [%p] Skip removing local user from session because %d is not a session member.\n", pRef, iValue));
            }
        }
        else
        {
            NetPrintf(("dirtysessionmanagerps4: [%p] Skip removing local user from session due to invalid user index.\n", pRef));
            iRet = -1;
        }
        return(iRet);
    }
    //'uimg' - queue updating the active image data from the pending session
    else if (iControl == 'uimg')
    {
        return(_DirtySessionManagerEnqueueControlCode(pRef, iControl, iValue, iValue2, pValue));
    }
    //'umau' - queue updating the active session max users attribute from the pending session
    else if (iControl == 'umau')
    {
        return(_DirtySessionManagerEnqueueControlCode(pRef, iControl, iValue, iValue2, pValue));
    }
    //'uses' - queue updating the active session from pending session
    else if (iControl == 'uses')
    {
        return(_DirtySessionManagerEnqueueControlCode(pRef, iControl, iValue, iValue2, pValue));
    }
    return(iRet);
}

/*F*************************************************************************************************/
/*!
    \Function    DirtySessionManagerStatus2

    \Description
        Return module status based on input selector, passes through to DirtySessionManagerStatus.

    \Input *pRef    - module ref
    \Input iSelect  - status selector
    \Input iValue   - selection argument
    \Input iValue2  - selection argument
    \Input iValue3  - selection argument
    \Input *pBuf    - user-provided buffer
    \Input iBufSize - size of user-provided buffer
    
    \Output
        int32_t     - depends on selector, see each code

    \Notes
        iSelect can be one of the following:

        \verbatim
            'giex' - get the invitation expired flag as bool; for the player at iValue index
            'ginv' - get the current count of invitations; for the player at iValue index
            'gimd' - get the invitation meta data has been set as bool; iValue player index, iValue2 invitation index
            'gims' - get the invitation maessage for; iValue player index, iValue2 invitation index, via pbuff as a char[]
            'giii' - get the invitation invite id for; iValue player index, iValue2 invitation index, via pbuff as a char[]
            'gisi' - get the invitation session id for; iValue player index, iValue2 invitation index, via pbuff as a char[]
            'giud' - get the current invitation list is up to date, as a bool; for the player at iValue index
            'giun' - get the invitation user name for; iValue player index, iValue2 invitation index, via pbuff as a DirtyUserT
            'giuf' - get the invitation used flag as bool; for the player at iValue index
        \endverbatim

    \Version 04/03/2013 (cvienneau)
*/
/*************************************************************************************************F*/
int32_t DirtySessionManagerStatus2(DirtySessionManagerRefT *pRef, int32_t iSelect, int32_t iValue, int32_t iValue2, int32_t iValue3, void *pBuf, int32_t iBufSize)
{
    int32_t iRet = -1;
    if (iSelect == 'giex')
    {
        if ((iValue >= 0) && (iValue < NETCONN_MAXLOCALUSERS))
        {
            iRet = pRef->aSessionUsers[iValue].RecievedInvitations.aInvitations[iValue2].bExpired;
        }
        else
        {
            NetPrintf(("dirtysessionmanagerps4: [%p] 'giex' invalid local user index. \n", pRef));
        }
        return (iRet);
    }
    if (iSelect == 'ginv')
    {
        if ((iValue >= 0) && (iValue < NETCONN_MAXLOCALUSERS))
        {
            iRet = pRef->aSessionUsers[iValue].RecievedInvitations.iNumInvitations;
        }
        else
        {
            NetPrintf(("dirtysessionmanagerps4: [%p] 'ginv' invalid local user index. \n", pRef));
        }
        return (iRet);
    }
    if (iSelect == 'gimd')
    {
        if ((iValue >= 0) && (iValue < NETCONN_MAXLOCALUSERS))
        {
            iRet = pRef->aSessionUsers[iValue].RecievedInvitations.aInvitations[iValue2].bHasMetaData;
        }
        else
        {
            NetPrintf(("dirtysessionmanagerps4: [%p] 'gimd' invalid local user index. \n", pRef));
        }
        return (iRet);
    }
    if (iSelect == 'gims')
    {
        if ((iValue >= 0) && (iValue < NETCONN_MAXLOCALUSERS))
        {
            if (iBufSize < ((int32_t)strlen(pRef->aSessionUsers[iValue].RecievedInvitations.aInvitations[iValue2].strMessage) + 1))
            {
                NetPrintf(("dirtysessionmanagerps4: [%p] 'gims' insufficient buffer space \n", pRef));
            }
            ds_strnzcpy((char *)pBuf, pRef->aSessionUsers[iValue].RecievedInvitations.aInvitations[iValue2].strMessage, iBufSize);
            iRet = 1;
        }
        else
        {
            NetPrintf(("dirtysessionmanagerps4: [%p] 'gims' invalid local user index. \n", pRef));
        }
        return (iRet);
    }
    if (iSelect == 'giii')
    {
        if ((iValue >= 0) && (iValue < NETCONN_MAXLOCALUSERS))
        {
            if (iBufSize < ((int32_t)strlen(pRef->aSessionUsers[iValue].RecievedInvitations.aInvitations[iValue2].strInvitationID) + 1))
            {
                NetPrintf(("dirtysessionmanagerps4: [%p] 'giii' insufficient buffer space \n", pRef));
            }
            else
            {
                ds_strnzcpy((char *)pBuf, pRef->aSessionUsers[iValue].RecievedInvitations.aInvitations[iValue2].strInvitationID, iBufSize);
                iRet = 1;
            }
        }
        else
        {
            NetPrintf(("dirtysessionmanagerps4: [%p] 'giii' invalid local user index. \n", pRef));
        }
        return (iRet);
    }
    if (iSelect == 'gisi')
    {
        if ((iValue >= 0) && (iValue < NETCONN_MAXLOCALUSERS))
        {
            if (iBufSize < ((int32_t)strlen(pRef->aSessionUsers[iValue].RecievedInvitations.aInvitations[iValue2].strSessionID) + 1))
            {
                NetPrintf(("dirtysessionmanagerps4: [%p] 'gisi' insufficient buffer space \n", pRef));
            }
            else
            {
                ds_strnzcpy((char *)pBuf, pRef->aSessionUsers[iValue].RecievedInvitations.aInvitations[iValue2].strSessionID, iBufSize);
                iRet = 1;
            }
        }
        else
        {
            NetPrintf(("dirtysessionmanagerps4: [%p] 'gisi' invalid local user index. \n", pRef));
        }
        return (iRet);
    }
    if (iSelect == 'giud')
    {
        if ((iValue >= 0) && (iValue < NETCONN_MAXLOCALUSERS))
        {
            iRet = pRef->aSessionUsers[iValue].RecievedInvitations.bListUpToDate;
        }
        else
        {
            NetPrintf(("dirtysessionmanagerps4: [%p] 'giud' invalid local user index. \n", pRef));
        }
        return (iRet);
    }
    if (iSelect == 'giun')
    {
        if ((iValue >= 0) && (iValue < NETCONN_MAXLOCALUSERS))
        {
            if (iBufSize < (int32_t)sizeof(DirtyUserT))
            {
                NetPrintf(("dirtysessionmanagerps4: [%p] 'giun' insufficient buffer space \n", pRef));
            }
            else
            {
                //create a DirtyUserT with the user name info we have
                DirtyUserT dirtyUser;
                SceNpAccountId accountId;
                accountId = pRef->aSessionUsers[iValue].RecievedInvitations.aInvitations[iValue2].FromUser;
                DirtyUserFromNativeUser(&dirtyUser, &accountId);

                //copy the dirtyuser into the one provided by the user
                ds_memcpy(pBuf, &dirtyUser, sizeof(dirtyUser));
                iRet = 1;
            }
        }
        else
        {
            NetPrintf(("dirtysessionmanagerps4: [%p] 'giun' invalid local user index. \n", pRef));
        }
        return (iRet);
    }
    if (iSelect == 'giuf')
    {
        if ((iValue >= 0) && (iValue < NETCONN_MAXLOCALUSERS))
        {
            iRet = pRef->aSessionUsers[iValue].RecievedInvitations.aInvitations[iValue2].bUsedFlag;
        }
        else
        {
            NetPrintf(("dirtysessionmanagerps4: [%p] 'giuf' invalid local user index. \n", pRef));
        }
        return (iRet);
    }
    return(DirtySessionManagerStatus(pRef, iSelect, pBuf, iBufSize));
}

/*F*************************************************************************************************/
/*!
    \Function    DirtySessionManagerStatus

    \Description
        Return module status based on input selector.

    \Input *pRef    - module ref
    \Input iSelect  - status selector
    \Input *pBuf    - user-provided buffer
    \Input iBufSize - size of user-provided buffer
    
    \Output
        int32_t     - depends on selector, see each code

    \Notes
        iSelect can be one of the following:

        \verbatim
            'bnam' - returns True if pending session name is before
            'bsta' - returns True if pending session status is set before
            'encb' - encode the pending sessions binary header and data and write it to pBuff if there is sufficiet space, returns the number of bytes
            'gclo' - get the active session's closed flag; -1 error, 0 joinable, 1 closed
            'gcre' - checks to see if session creation/join has completed, returns -1 error, 0 in progress, 1 active; and session id if a buffer to receive it (46 bytes) has been provided
            'gprc' - gets the pending request count, will be 0 with the session manager is idel
            'gdat' - get the active session's binary data size; returns -1 error, 0 in progress, > 0 size; pValue if large enough will have the data written to it
            'gpda' - get the pending session's binary data size; returns -1 error, 0 in progress, > 0 size; pValue if large enough will have the data written to it
            'gcda' - get the active session's changeable binary data size; returns -1 error, 0 in progress, > 0 size; pValue if large enough will have the data written to it
            'gpcd' - get the pending session's changeable binary data size; returns -1 error, 0 in progress, > 0 size; pValue if large enough will have the data written to it
            'gali' - get the active binary data lobby id; int64 via pBuf, iBUfSize =  sizeof(int64_t)
            'gpli' - get the pending binary data lobby id; int64 via pValue, iValue2 sizeof(int64_t)
            'gagm' - get the active changeable data game mode; int32 via pBuf, iBufSize =  sizeof(int32_t)
            'gpgm' - get the pending changeable data game mode; int32 via pBuf, iBufSize =  sizeof(int32_t)
            'gagt' - get the active binary data game type; int64 via pBuf, iBufSize =  sizeof(int64_t)
            'gpgt' - get the pending binary data game type; int64 via pBuf, iBufSize =  sizeof(int64_t)
            'gmau' - get the active session's maximum number of users; -1 error, >=0 max users
            'gnam' - get the active session's name; -1 error, pBuf output string
            'gpid' - get the pending session id. -1 error, 1 success, pBuf output string
            'gpri' - get the active session's privacy setting; -1 error, 0 public, 1 private 
            'gsdo' - get if the session dialog is open
            'gsii' - get the selected invite id from the invite dialog, via pBuf
            'gssi' - get the selected session id from the invite dialog, via pBuf
            'gsta' - get the active session's status; -1 error, pBuf output string
            'laid' - get the last user that accepted an invite and returns it via pBuf as a DirtyUserT
            'lock' - get the session lock status of the active session
            'lsts' - get the last request's status code. check <0 for error
            'pthe' - check to see if the client has received a Play Togther Host Event (it will be reset after this call) return true or false.
                     If true the SceUserServiceUserId of the user that launched the game will be return in pBuf.
            'ptil' - returns the Play Together invite list length. The invite list will be return in pBuf if provided.
        \endverbatim

    \Version 04/03/2013 (cvienneau)
*/
/*************************************************************************************************F*/
int32_t DirtySessionManagerStatus(DirtySessionManagerRefT *pRef, int32_t iSelect, void *pBuf, int32_t iBufSize)
{
    int32_t iRet;

    if (iSelect == 'bnam')
    {
        iRet = pRef->bPendingNameSet;
    }

    if (iSelect == 'bsta')
    {
        iRet = pRef->bPendingStatusSet;
    }

    if (iSelect == 'ginv')
    {
        iRet = pRef->aSessionUsers[0].RecievedInvitations.iNumInvitations;
    }
    // 'encb' - encode the pending sessions binary header and data and write it to pBuff if there is sufficiet space, returns the number of bytes
    else if (iSelect == 'encb')
    {
        DirtySessionManagerEncodeSession((char *)pBuf, iBufSize, pRef);
        iRet = sizeof(pRef->PendingSession.binaryDataHeader) + pRef->PendingSession.iBinaryDataSize;
    }
    //'gclo' - get the active session's closed flag; -1 error, 0 joinable, 1 closed
    else if (iSelect == 'gclo')
    {
        if (pRef->eSessionStatus == DIRTY_SESSION_ACTIVE)
        {
            iRet = pRef->ActiveSession.bClosed;
        }
        else
        {
            iRet = -1;
        }
    }
    //'gcre' - checks to see if session creation has completed, 
    //returns -1 error, 0 in progress, 1 active; and session id if a buffer to receive it (46 bytes) has been provided
    if (iSelect == 'gcre')
    {
        //determine if the session is active
        iRet = -1;
        if (pRef->eSessionStatus == DIRTY_SESSION_ACTIVE)
        {
            iRet = 1;
        }
        else if (pRef->eSessionStatus == DIRTY_SESSION_PENDING)
        {
            iRet = 0;
        }

        //copy the session id
        if ((pRef->eSessionStatus == DIRTY_SESSION_ACTIVE) && (pBuf != NULL) && (iBufSize >= (int32_t)sizeof(pRef->ActiveSession.strSessionID)))
        {
            ds_strnzcpy((char *)pBuf, pRef->ActiveSession.strSessionID, iBufSize);
        }
        else if ((pRef->eSessionStatus == DIRTY_SESSION_INVALID) && (pBuf != NULL) && (iBufSize >= (int32_t)sizeof(pRef->ActiveSession.strSessionID)))
        {
            ds_strnzcpy((char *)pBuf, "INVALID", iBufSize);
        }
    }
    //'gprc' - gets the pending request count, will be 0 with the session manager is idel
    else if (iSelect == 'gprc')
    {
        NetCritEnter(&pRef->crit);
        iRet = pRef->iActiveRequests;
        NetCritLeave(&pRef->crit);
    }
    //'gdat' - get the active session's binary data size (binary data returned in pBuf)
    //returns -1 error, 0 in progress, > 0 size; pValue if large enough will have the data written to it
    else if (iSelect == 'gdat')
    {
        iRet = pRef->ActiveSession.iBinaryDataSize;
        if ((iRet > 0) && (pBuf != NULL) && (iBufSize >= pRef->ActiveSession.iBinaryDataSize))
        {
            ds_memcpy(pBuf, pRef->ActiveSession.pBinaryData, pRef->ActiveSession.iBinaryDataSize);
        }
    }
    //'gpda' - get the pending session's binary data size (binary data returned in pBuf)
    //returns -1 error, 0 in progress, > 0 size; pValue if large enough will have the data written to it
    else if (iSelect == 'gpda')
    {
        iRet = pRef->PendingSession.iBinaryDataSize;
        if ((iRet > 0) && (pBuf != NULL) && (iBufSize >= pRef->PendingSession.iBinaryDataSize))
        {
            ds_memcpy(pBuf, pRef->PendingSession.pBinaryData, pRef->PendingSession.iBinaryDataSize);
        }
    }
    //'gcda' - get the active session's changeable binary data size (changeable binary data returned in pBuf)
    //returns -1 error, 0 in progress, > 0 size; pValue if large enough will have the data written to it
    else if (iSelect == 'gcda')
    {
        iRet = pRef->ActiveSession.iChangeableBinaryDataSize;
        if ((iRet > 0) && (pBuf != NULL) && (iBufSize >= pRef->ActiveSession.iChangeableBinaryDataSize))
        {
            ds_memcpy(pBuf, pRef->ActiveSession.pChangeableBinaryData, pRef->ActiveSession.iChangeableBinaryDataSize);
        }
    }
    //'gpcd' - get the pending session's changeable binary data size (changeable binary data returned in pBuf)
    //returns -1 error, 0 in progress, > 0 size; pValue if large enough will have the data written to it
    else if (iSelect == 'gpcd')
    {
        iRet = pRef->PendingSession.iChangeableBinaryDataSize;
        if ((iRet > 0) && (pBuf != NULL) && (iBufSize >= pRef->PendingSession.iChangeableBinaryDataSize))
        {
            ds_memcpy(pBuf, pRef->PendingSession.pChangeableBinaryData, pRef->PendingSession.iChangeableBinaryDataSize);
        }
    }
    //'gali' - get the active binary data lobby id; int64 via pBuf, iBufSize =  sizeof(int64_t)
    else if (iSelect == 'gali')
    {
        if (iBufSize == sizeof(int64_t) && pBuf != NULL)
        {
            ds_memcpy(pBuf, &pRef->ActiveSession.binaryDataHeader.iLobbyId, sizeof(int64_t));
            iRet = 1;
        }
        else
        {
            iRet = -1;
        }
    }
    //'gpli' - get the pending binary data lobby id; int64 via pBuf, iBufSize =  sizeof(int64_t)
    else if (iSelect == 'gpli')
    {
        if (iBufSize == sizeof(int64_t) && pBuf != NULL)
        {
            ds_memcpy(pBuf, &pRef->PendingSession.binaryDataHeader.iLobbyId, sizeof(int64_t));
            iRet = 1;
        }
        else
        {
            iRet = -1;
        }
    }
    //'gagt' - get the active binary data game type; int64 via pBuf, iBufSize =  sizeof(int64_t)
    else if (iSelect == 'gagt')
    {
        if (iBufSize == sizeof(int32_t) && pBuf != NULL)
        {
            ds_memcpy(pBuf, &pRef->ActiveSession.binaryDataHeader.iGameType, sizeof(int64_t));
            iRet = 1;
        }
        else
        {
            iRet = -1;
        }
    }
    //'gpgt' - get the pending binary data game type; int64 via pBuf, iBufSize =  sizeof(int64_t)
    else if (iSelect == 'gpgt')
    {
        if (iBufSize == sizeof(int64_t) && pBuf != NULL)
        {
            ds_memcpy(pBuf, &pRef->PendingSession.binaryDataHeader.iGameType, sizeof(int64_t));
            iRet = 1;
        }
        else
        {
            iRet = -1;
        }
    }
    //'gagm' - get the active changeable data game mode; int32 via pBuf, iBufSize =  sizeof(int32_t)
    else if (iSelect == 'gagm')
    {
        if ((pBuf != NULL) && (iBufSize >= (int32_t)sizeof(pRef->ActiveSession.changeableBinaryDataHeader.strGameMode)))
        {
            if (pRef->ActiveSession.changeableBinaryDataHeader.strGameMode[0] == '\0')
            {
                ((char*)pBuf)[0] = '\0';
            }
            else
            {
                ds_strnzcpy((char *)pBuf, pRef->ActiveSession.changeableBinaryDataHeader.strGameMode, iBufSize);
                iRet = 1;
            }
        }
        else
        {
            iRet = -1;
        }
    }
    //'gpgm' - get the pending changeable data game mode; int32 via pBuf, iBufSize =  sizeof(int32_t)
    else if (iSelect == 'gpgm')
    {
        if ((pBuf != NULL) && (iBufSize >= (int32_t)sizeof(pRef->PendingSession.changeableBinaryDataHeader.strGameMode)))
        {
            if (pRef->PendingSession.changeableBinaryDataHeader.strGameMode[0] == '\0')
            {
                ((char*)pBuf)[0] = '\0';
            }
            else
            {
                ds_strnzcpy((char *)pBuf, pRef->PendingSession.changeableBinaryDataHeader.strGameMode, iBufSize);
                iRet = 1;
            }
        }
        else
        {
            iRet = -1;
        }
    }
    //'gmau' - get the active session's maximum number of users; -1 error, >=0 max users
    else if (iSelect == 'gmau')
    {
        if (pRef->eSessionStatus == DIRTY_SESSION_ACTIVE)
        {
            iRet = pRef->ActiveSession.iMaxUsers;
        }
        else
        {
            iRet = -1;
        }
    }
    //'gnam' - get the active session's name; -1 error, pBuf output string
    else if (iSelect == 'gnam')
    {
        if ((pRef->eSessionStatus == DIRTY_SESSION_ACTIVE) && (pBuf != NULL) && (iBufSize >= (int32_t)sizeof(pRef->ActiveSession.strSessionName)))
        {
            ds_strnzcpy((char *)pBuf, pRef->ActiveSession.strSessionName, iBufSize);
            iRet = 1;
        }
        else
        {
            iRet = -1;
        }
    }
    //'gpri' - get the active session's privacy setting; -1 error, 0 public, 1 private 
    else if (iSelect == 'gpri')
    {
        if (pRef->eSessionStatus == DIRTY_SESSION_ACTIVE)
        {
            iRet = pRef->ActiveSession.bPrivate;
        }
        else
        {
            iRet = -1;
        }
    }
    //'gpid' - get the pending session id. -1 error, 1 success, pBuf output string
    else if (iSelect == 'gpid')
    {
        if ( (pBuf != NULL) && (pRef->PendingSession.strSessionID[0] != '\0') && (iBufSize >= (int32_t)sizeof(pRef->PendingSession.strSessionID)))
        {
            ds_strnzcpy((char *)pBuf, pRef->PendingSession.strSessionID, iBufSize);
            iRet = 1;
        }
        else
        {
            iRet = -1;
        }
    }
    //'gsdo' - get if the session dialog is open
    else if (iSelect == 'gsdo')
    {
        iRet = pRef->bInvDialog;
    }
    //'gsii' - get the selected invite id from the invite dialog, via pBuf
    else if (iSelect == 'gsii')
    {
        if ((pBuf != NULL) && (iBufSize >= (int32_t)sizeof(pRef->strSelectedInviteID)))
        {
            ds_strnzcpy((char *)pBuf, pRef->strSelectedInviteID, iBufSize);
            iRet = 1;
        }
        else
        {
            iRet = -1;
        }
    }    
    //'gssi' - get the selected session id from the invite dialog, via pBuf
    else if (iSelect == 'gssi')
    {
        if ((pBuf != NULL) && (iBufSize >= (int32_t)sizeof(pRef->strSelectedSessionID)))
        {
            ds_strnzcpy((char *)pBuf, pRef->strSelectedSessionID, iBufSize);
            iRet = 1;
        }
        else
        {
            iRet = -1;
        }
    }
    //'gsta' - get the active session's status; -1 error, pBuf output string
    else if (iSelect == 'gsta')
    {
        if ((pRef->eSessionStatus == DIRTY_SESSION_ACTIVE) && (pBuf != NULL) && (iBufSize >= (int32_t)sizeof(pRef->ActiveSession.strSessionStatus)))
        {
            ds_strnzcpy((char *)pBuf, pRef->ActiveSession.strSessionStatus, iBufSize);
            iRet = 1;
        }
        else
        {
            iRet = -1;
        }
    } 
    //'laid' - get last user that accepted the invite
    else if (iSelect == 'laid')
    {
        if ((pBuf != 0) && (iBufSize >= (int32_t)sizeof(DirtyUserT)))
        {
            SceNpAccountId AcountId = 0;
            iRet = sceNpGetAccountIdA(pRef->lastAcceptUserId, &AcountId);
            if (iRet >= 0)
            {
                DirtyUserFromNativeUser((DirtyUserT *)pBuf, &AcountId);
            }
        }
        else
        {
            iRet = -1;
        }
    }
    // 'lock' get the session lock status of the active session
    else if (iSelect == 'lock')
    {
        return(pRef->ActiveSession.bSessionLock);
    }
    // 'lsts' - get the last request's status code
    else if (iSelect == 'lsts')
    {
        return(pRef->iLastStatusCode);
    }
    #if SCE_ORBIS_SDK_VERSION < 0x07508001u
    // 'pthe' - check to see if we received a Play Togther Host event
    else if (iSelect == 'pthe')
    {
        iRet = FALSE;

        if (pRef->bPTHostEvent)
        {
            if ((pBuf != NULL) && (iBufSize >= (int32_t)sizeof(SceUserServiceUserId)))
            {
                ds_memcpy_s(pBuf, iBufSize, &pRef->launchSuid, sizeof(pRef->launchSuid));
            }

            pRef->bPTHostEvent = FALSE;
            iRet = TRUE;
        }
    }
    // 'ptil' - returns the Play Together invite list size. The list itself will be returned in pBuf.
    else if (iSelect == 'ptil')
    {
        iRet = 0;

        if ((pBuf != NULL) && (iBufSize >= (int32_t)(pRef->iPTInviteListLen * sizeof(SceNpPlayTogetherInvitee))))
        {
            int32_t iIndex;
            SceNpPlayTogetherInvitee *pInviteList = (SceNpPlayTogetherInvitee *)pBuf;

            for (iIndex = 0; iIndex < pRef->iPTInviteListLen; ++iIndex)
            {
                ds_memcpy_s(&pInviteList[iIndex], sizeof(SceNpPlayTogetherInvitee), &pRef->aPTInviteList[iIndex], sizeof(SceNpPlayTogetherInvitee));
            }
        }

        iRet = pRef->iPTInviteListLen;
    }
    #endif

    return(iRet);
}

/*F*************************************************************************************************/
/*!
    \Function    DirtySessionManagerConnect

    \Description
        (Unused) Connects one or many player(s) to a given game

    \Input *pRef    - module state
    \Input **pSessID- for Xbox, this is an array of pointers to the peers DirtyAddrT
    \Input pSlot    - slot type to use, i.e. DIRTYSESSIONMANAGER_FLAG_PUBLICSLOT, array: one per player
    \Input uCount   - number of players to join

    \Version 04/03/2013 (cvienneau)
*/
/*************************************************************************************************F*/
void DirtySessionManagerConnect(DirtySessionManagerRefT *pRef, const char **pSessID, uint32_t *pSlot, uint32_t uCount)
{
    if (pRef->bServerDrivenModeEnabled)
    {
        NetPrintf(("dirtysessionmanagerps4: [%p] DirtySessionManagerConnect() ignored because server-driven operational mode enabled\n", pRef));
        return;
    }
}

/*F*************************************************************************************************/
/*!
    \Function    DirtySessionManagerCreate

    \Description
        Allocate module state and prepare for use.

    \Output
        DirtySessionManagerRefT *   - reference pointer (must be passed to all other functions)

    \Version 04/03/2013 (cvienneau)
*/
/*************************************************************************************************F*/
DirtySessionManagerRefT *DirtySessionManagerCreate(void)
{
    DirtySessionManagerRefT *pRef;
    int32_t iMemGroup;
    void *pMemGroupUserData;
    DirtyWebApiCreateParamsT createParams;

    iMemGroup = NetConnStatus('mgrp', 0, &pMemGroupUserData, sizeof(pMemGroupUserData));

    // allocate and init module state
    if ((pRef = DirtyMemAlloc(sizeof(*pRef), DIRTYSESSMGR_MEMID, iMemGroup, pMemGroupUserData)) == NULL)
    {
        NetPrintf(("dirtysessionmanagerps4: [%p] failed to allocate module state.\n", pRef));
        return(NULL);
    }
    ds_memclr(pRef, sizeof(*pRef));
    pRef->iMemGroup = iMemGroup;
    pRef->pMemGroupUserData = pMemGroupUserData;
    pRef->iRequestBuffSize = DIRTY_SESSION_DEFAULT_REQUEST_BUFF_SIZE;
    pRef->bServerDrivenModeEnabled = TRUE;
    NetCritInit(&pRef->crit, "DirtySessionManager");

    //prepare web api
    ds_memclr(&createParams, sizeof(createParams));
    createParams.readBufferSize = DIRTY_SESSION_DEFAULT_RECIEVE_BUFF_SIZE;
    if ((pRef->pWebApi = DirtyWebApiCreate(&createParams)) == NULL)
    {
        NetPrintf(("dirtysessionmanagerps4: [%p] DirtyWebApiCreate(), failed.\n", pRef));
        _DirtySessionManagerDestroy(pRef);
        return(NULL);
    }

    //add idler handler
    NetConnIdleAdd(_DirtySessionManagerUpdate, pRef);

    // Register for system events, to capture selection of invite in system util
    DirtyEventManagerRegisterSystemService(&_DirtySessionManagerOnSystemEvent, pRef);

    NetPrintf(("dirtysessionmanagerps4: [%p] initial creation completed.\n", pRef));

    // return ref to caller
    return(pRef);
}

/*F*************************************************************************************************/
/*!
    \Function    DirtySessionManagerDestroy

    \Description
        Destroy the module and release its state

    \Input *pRef    - reference pointer

    \Version 04/03/2013 (cvienneau)
*/
/*************************************************************************************************F*/
void DirtySessionManagerDestroy(DirtySessionManagerRefT *pRef)
{
    int32_t iActiveRequests = 0;
    NetPrintf(("dirtysessionmanagerps4: [%p] DirtySessionManagerDestroy\n", pRef));
    
    //leave any sessions we may be a part of
    DirtySessionManagerControl(pRef, 'sdel', 0, 0, NULL);
    
    NetCritEnter(&pRef->crit);
    iActiveRequests = pRef->iActiveRequests;
    NetCritLeave(&pRef->crit);
    
    if (iActiveRequests > 0)
    {
        //we need to use external cleanup
        NetPrintf(("dirtysessionmanagerps4: [%p] DirtySessionManagerDestroy waiting on %d requests.\n", pRef, iActiveRequests));
        NetConnControl('recu', 0, 0, (void *)_DirtySessionManagerDestroyCallback, pRef);
    }
    else
    {
        _DirtySessionManagerDestroy(pRef);
    }

}
