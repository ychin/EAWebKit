/*H********************************************************************************/
/*!
    \File privilegeapips4.c

    \Description
        Implements the ps4 specific parental control and PlayStation Plus checking.

    \Copyright
        Copyright (c) Electronic Arts 2012

    \Version 09/02/2013 (mcorcoran) First Version
*/
/********************************************************************************H*/

/*** Include files ***********************************************************/
#include <string.h>
#include <np.h>
//#include <np/np_npid.h>
#include <scetypes.h>
#include <np_commerce_dialog.h>
#include <message_dialog.h>


#include "DirtySDK/platform.h"
#include "DirtySDK/misc/privilegeapi.h"

#include "DirtySDK/dirtysock/dirtyerr.h"
#include "DirtySDK/dirtysock/dirtymem.h"
#include "DirtySDK/dirtysock/dirtylib.h"
#include "DirtySDK/dirtysock/netconn.h"

/*** Defines ***************************************************************************/

/*** Macros ****************************************************************************/

/*** Type Definitions ******************************************************************/

typedef enum PrivilegeApiSceStateE
{
    PRIVILEGEAPI_SCE_STATE_GRANTED          = 0x0000,
    PRIVILEGEAPI_SCE_STATE_RESTRICTED       = 0x0001,
    PRIVILEGEAPI_SCE_STATE_IN_PROGRESS      = 0x0002,
    PRIVILEGEAPI_SCE_STATE_DIALOG_PENDING   = 0x0004 | PRIVILEGEAPI_SCE_STATE_IN_PROGRESS,
    PRIVILEGEAPI_SCE_STATE_DIALOG_SHOWING   = 0x0008 | PRIVILEGEAPI_SCE_STATE_IN_PROGRESS
    
} PrivilegeApiSceStateE;

typedef struct PrivilegeApiUserContextT
{
    int32_t iStatus;
    int32_t iSceNpPlusFeatures;
    PrivilegeApiSceStateE eChat;
    PrivilegeApiSceStateE eUgc;
    PrivilegeApiSceStateE eContent;
    PrivilegeApiSceStateE ePlus;
    PrivilegeApiSceStateE iDialogOption;
    uint8_t bIsUnderAge;
} PrivilegeApiUserContextT;

struct PrivilegeApiRefT
{
    int32_t                iMemGroup;                                 //!< dirtymem memory group
    void                  *pMemGroupUserData;                         //!< dirtymem memory group user data

    PrivilegeApiUserContextT aUserContext[NETCONN_MAXLOCALUSERS];

    int8_t bCommerceDialogInitialized;
    int8_t bMsgDialogInitialized;
};

/*** Function Prototypes ***************************************************************/

/*** Variables *************************************************************************/

static uint8_t bPrivApiCommerceCleanupNeeded = FALSE;
static uint8_t bPrivApiMessageCleanupNeeded  = FALSE;

/*** Private functions *****************************************************************/
/*F********************************************************************************/
/*!
    \Function _PrivilegeApiUpdateDialogStatus

    \Description
        Check that message dialog closure is complete and terminate if appropriate

    \Input *pData   - PrivilegeApiSceState
    \Input uTick    - current tick count

    \Version 051/02/2014 (amakoukji)
*/
/********************************************************************************F*/
static void _PrivilegeApiUpdateDialogStatus(void *pData, uint32_t uTick)
{
    SceCommonDialogStatus CommerceDialogStatus = SCE_COMMON_DIALOG_STATUS_NONE;
    SceCommonDialogStatus MsgDialogStatus      = SCE_COMMON_DIALOG_STATUS_NONE;

    PrivilegeApiSceStateE *state = (PrivilegeApiSceStateE*)pData;

    if (*state != PRIVILEGEAPI_SCE_STATE_DIALOG_SHOWING)
    {
        // check commerce dialog
        if (bPrivApiCommerceCleanupNeeded == TRUE)
        {
            // all managed instances destroyed, if dialog is finish or initialized but not in a showing state
            CommerceDialogStatus = sceNpCommerceDialogUpdateStatus();
            if ((CommerceDialogStatus == SCE_COMMON_DIALOG_STATUS_FINISHED) || (CommerceDialogStatus == SCE_COMMON_DIALOG_STATUS_INITIALIZED))
            {
                sceNpCommerceDialogTerminate();
                bPrivApiCommerceCleanupNeeded = FALSE;
            }
        }

        // check message dialog
        if (bPrivApiMessageCleanupNeeded == TRUE)
        {
            // all managed instances destroyed if dialog is finish or initialized but not in a showing state
            MsgDialogStatus = sceMsgDialogUpdateStatus();
            if ((MsgDialogStatus == SCE_COMMON_DIALOG_STATUS_FINISHED) || (MsgDialogStatus == SCE_COMMON_DIALOG_STATUS_INITIALIZED))
            {
                sceMsgDialogTerminate();
                bPrivApiMessageCleanupNeeded = FALSE;
            }
        }
    }

    if (bPrivApiCommerceCleanupNeeded == FALSE && bPrivApiMessageCleanupNeeded == FALSE)
    {
        // we're done with all cleanups, remove the idler
        NetConnIdleDel(_PrivilegeApiUpdateDialogStatus, pData);
    }
}

/*F********************************************************************************/
/*!
    \Function _PrivilegeApiUpdateParentalProgress

    \Description
        Display and monitor the Message Dialog if the state
        is PRIVILEGEAPI_SCE_STATE_DIALOG_PENDING or PRIVILEGEAPI_SCE_STATE_DIALOG_SHOWING.

    \Input  *pRef            - the module reference
    \Input  *pState          - the given state
    \Input  iMsgDialogType   - the sce message dialog type to show
    \Input  iUserIndex       - the current user index
    
    \Output int32_t          - negative=error, zero=success

    \Version 09/02/2013 (mcorcoran)
*/
/********************************************************************************F*/
static int32_t _PrivilegeApiUpdateParentalProgress(PrivilegeApiRefT *pRef, PrivilegeApiSceStateE *pState, int32_t iMsgDialogType, int32_t iUserIndex)
{
    int32_t iRet = 0;

    // check if we should try to show the message dialog
    if (*pState == PRIVILEGEAPI_SCE_STATE_DIALOG_PENDING)
    {
        // make sure the message dialog is initialized
        iRet = sceMsgDialogInitialize();
        if (iRet != SCE_OK)
        {
            if (iRet != SCE_COMMON_DIALOG_ERROR_ALREADY_INITIALIZED && !sceCommonDialogIsUsed())
            {
                NetPrintf(("privilegeapips4: sceInvitationDialogInitialize() failed, error = %s\n", DirtyErrGetName(iRet)));
                return(iRet);
            }
        }
        else
        {
            // track that *we* initialized the dialog, so we can free it on destroy
            pRef->bMsgDialogInitialized = TRUE;
            if (bPrivApiCommerceCleanupNeeded == FALSE && bPrivApiMessageCleanupNeeded == FALSE)
            {
                // we need to manage the cleanup states of these dialogs
                NetConnIdleAdd(_PrivilegeApiUpdateDialogStatus, pState);
            }
            bPrivApiMessageCleanupNeeded = TRUE;
        }

        SceCommonDialogStatus status = sceMsgDialogUpdateStatus();
        if (status != SCE_COMMON_DIALOG_STATUS_RUNNING)
        {
            // as long as the message dialog is not currently being displayed
            // we can go ahead and display our message
            SceMsgDialogParam param;
            SceMsgDialogSystemMessageParam systemMsgParam;

            sceMsgDialogParamInitialize(&param);
            ds_memclr(&systemMsgParam, sizeof(systemMsgParam));
            param.mode = SCE_MSG_DIALOG_MODE_SYSTEM_MSG;
            param.userId = NetConnStatus('suid', iUserIndex, NULL, 0);
            param.sysMsgParam = &systemMsgParam;
            param.sysMsgParam->sysMsgType = iMsgDialogType;

            iRet = sceMsgDialogOpen(&param);
            if (iRet == SCE_OK)
            {
                *pState = PRIVILEGEAPI_SCE_STATE_DIALOG_SHOWING;
            }
            else
            {
                // we shouldn't really ever get here, but if we do, just let the function return, and we'll try again later
                NetPrintf(("privilegeapips4: sceMsgDialogOpen() failed with (%s)\n", DirtyErrGetName(iRet)));
                sceMsgDialogTerminate();
            }
        }

    }
    else if (*pState == PRIVILEGEAPI_SCE_STATE_DIALOG_SHOWING)
    {
        SceCommonDialogStatus status = sceMsgDialogUpdateStatus();
        if (status == SCE_COMMON_DIALOG_STATUS_FINISHED || status ==  SCE_COMMON_DIALOG_STATUS_NONE)
        {
            *pState = PRIVILEGEAPI_SCE_STATE_RESTRICTED;
            sceMsgDialogTerminate();
        }
    }

    return(iRet);
}

/*F********************************************************************************/
/*!
    \Function _PrivilegeApiUpdatePlusProgress

    \Description
        Display and monitor the Commerce Dialog if the state
        is PRIVILEGEAPI_SCE_STATE_DIALOG_PENDING or PRIVILEGEAPI_SCE_STATE_DIALOG_SHOWING.

    \Input  *pRef            - the module reference
    \Input   iUserIndex      - the user index
    
    \Output int32_t          - negative=error, zero=success

    \Version 09/02/2013 (mcorcoran)
*/
/********************************************************************************F*/
static int32_t _PrivilegeApiUpdatePlusProgress(PrivilegeApiRefT *pRef, int32_t iUserIndex)
{
    int32_t iRet = 0;

    if (pRef->aUserContext[iUserIndex].ePlus == PRIVILEGEAPI_SCE_STATE_DIALOG_PENDING)
    {
        iRet = sceNpCommerceDialogInitialize();
        if (iRet != SCE_OK)
        {
            if (iRet != SCE_COMMON_DIALOG_ERROR_ALREADY_INITIALIZED && !sceCommonDialogIsUsed())
            {
                NetPrintf(("privilegeapips4: [%p] sceInvitationDialogInitialize() failed, error = %s\n", pRef, DirtyErrGetName(iRet)));
                return (iRet);
            }
        }
        else
        {
            pRef->bCommerceDialogInitialized = TRUE;
            if (bPrivApiCommerceCleanupNeeded == FALSE && bPrivApiMessageCleanupNeeded == FALSE)
            {
                // we need to manage the cleanup states of these dialogs
                NetConnIdleAdd(_PrivilegeApiUpdateDialogStatus, &pRef->aUserContext[iUserIndex].ePlus);
            }
            bPrivApiCommerceCleanupNeeded = TRUE;
        }

        SceCommonDialogStatus status = sceNpCommerceDialogUpdateStatus();
        if (status != SCE_COMMON_DIALOG_STATUS_RUNNING)
        {
            SceNpCommerceDialogParam param;
            sceNpCommerceDialogParamInitialize(&param);
            #ifdef DIRTYCODE_PS5
            param.mode = SCE_NP_COMMERCE_DIALOG_MODE_PREMIUM;
            #else       
            param.mode = SCE_NP_COMMERCE_DIALOG_MODE_PLUS;
            #endif
            param.features = pRef->aUserContext[iUserIndex].iSceNpPlusFeatures;
            param.userId = NetConnStatus('suid', iUserIndex, NULL, 0);
            param.targets = NULL;
            param.numTargets = 0;

            iRet = sceNpCommerceDialogOpen(&param);
            if (iRet == SCE_OK)
            {
                pRef->aUserContext[iUserIndex].ePlus = PRIVILEGEAPI_SCE_STATE_DIALOG_SHOWING;
            }
            else
            {
                // we shouldn't really ever get here, but if we do, just let the function return and we'll try again later
                NetPrintf(("privilegeapips4: [%p] sceNpCommerceDialogOpen() failed with error (%s)\n", pRef, DirtyErrGetName(iRet)));
                sceNpCommerceDialogTerminate();
            }
        }
    }
    else if (pRef->aUserContext[iUserIndex].ePlus == PRIVILEGEAPI_SCE_STATE_DIALOG_SHOWING)
    {
        sceNpCommerceDialogUpdateStatus();

        SceNpCommerceDialogResult dialogResult;
        ds_memclr(&dialogResult, sizeof(dialogResult));

        iRet = sceNpCommerceDialogGetResult(&dialogResult);
        if (iRet >= 0)
        {
            if (dialogResult.authorized)
            {
                pRef->aUserContext[iUserIndex].ePlus = PRIVILEGEAPI_SCE_STATE_GRANTED;
            }
            else
            {
                pRef->aUserContext[iUserIndex].ePlus = PRIVILEGEAPI_SCE_STATE_RESTRICTED;
                if (dialogResult.result == SCE_COMMON_DIALOG_RESULT_USER_CANCELED)
                {
                    // return as error when users cancel the dialog
                    iRet = -1;
                }
            }

            sceNpCommerceDialogTerminate();
        }
        else if (iRet == SCE_COMMON_DIALOG_ERROR_NOT_FINISHED)
        {
            // not technically an error, still waiting for users to react to dialog
            iRet = 0;
        }
        else
        {
            pRef->aUserContext[iUserIndex].ePlus = PRIVILEGEAPI_SCE_STATE_RESTRICTED;
            NetPrintf(("privilegeapips4: [%p] sceNpCommerceDialogGetResult() failed with error (%s)\n", pRef, DirtyErrGetName(iRet)));
            sceNpCommerceDialogTerminate();
        }
    }

    return(iRet);
}

/*** Public functions ********************************************************/

/*F********************************************************************************/
/*!
    \Function PrivilegeApiCreate

    \Description
        Create the PrivilegeApi

    \Output PrivilegeApiRefT    - the module reference, or NULL on error

    \Version 09/02/2013 (mcorcoran)
*/
/********************************************************************************F*/
PrivilegeApiRefT *PrivilegeApiCreate(void)
{
    PrivilegeApiRefT *pRef;
    int32_t iMemGroup;
    void *pMemGroupUserData;
    int32_t iRet;

    iRet = sceCommonDialogInitialize();
    if ((iRet != SCE_OK) && (iRet != SCE_COMMON_DIALOG_ERROR_ALREADY_SYSTEM_INITIALIZED))
    {
        NetPrintf(("privilegeapips4: sceCommonDialogInitialize() failed, error = %s\n", DirtyErrGetName(iRet)));
        return (NULL);
    }

    DirtyMemGroupQuery(&iMemGroup, &pMemGroupUserData);

    // allocate and init module state
    if ((pRef = (PrivilegeApiRefT*)DirtyMemAlloc(sizeof(*pRef), PRIVILEGEAPI_MEMID, iMemGroup, pMemGroupUserData)) == NULL)
    {
        NetPrintf(("privilegeapips4: [%p] failed to allocate module state.\n", pRef));
        return(NULL);
    }
    ds_memclr(pRef, sizeof(*pRef));
    pRef->iMemGroup = iMemGroup;
    pRef->pMemGroupUserData = pMemGroupUserData;

    return(pRef);
}

/*F********************************************************************************/
/*!
    \Function PrivilegeApiDestroy

    \Description
        Destroy the PrivilegeApi

    \Input  *pRef      - the module reference
    
    \Output int32_t    - <0 = error, >=0 = success

    \Version 09/02/2013 (mcorcoran)
*/
/********************************************************************************F*/
int32_t PrivilegeApiDestroy(PrivilegeApiRefT *pRef)
{
    for (int32_t iUserIndex = 0; iUserIndex < NETCONN_MAXLOCALUSERS; iUserIndex++)
    {
        if (pRef->aUserContext[iUserIndex].iStatus != PRIVILEGEAPI_STATUS_NONE)
        {
            PrivilegeApiAbort(pRef, iUserIndex + 1);
        }
    }

    // DialogTerminate() calls are handled by NetConnIdler in _PrivilegeApiUpdateDialogStatus()
    DirtyMemFree(pRef, PRIVILEGEAPI_MEMID, pRef->iMemGroup, pRef->pMemGroupUserData);

    return(1);
}

/*F********************************************************************************/
/*!
    \Function PrivilegeApiCheckPrivilegesAsync

    \Description
        Check one or more privileges against the system or the first party service for an authorotative
        determination of the current privilege status.  No UI is shown when using this function.

    \Input  *pRef             - the module reference
    \Input   iUserIndex       - user index
    \Input  *pPrivileges      - array of privileges
    \Input   iPrivilegeCount  - the numnber of privileges pointed to by pPrivileges
    \Input  *pHint            - (optional, out) if provided, pHint will be filled with a non-authoratative result for the privilege check.  Unless the return value is zero (0), in which case, this value can be considered autorotative.
    
    \Output int32_t    - <0 = error, 0 = success, use *pHint for the result, >0 = the request id to pass into PrivilegeApiCheckResult()

    \Version 09/02/2013 (mcorcoran)
*/
/********************************************************************************F*/
int32_t PrivilegeApiCheckPrivilegesAsync(PrivilegeApiRefT *pRef, int32_t iUserIndex, const PrivilegeApiPrivE *pPrivileges, int32_t iPrivilegeCount, int32_t *pHint)
{
    return PrivilegeApiCheckPrivilegesAsyncWithUi(pRef, iUserIndex, pPrivileges, iPrivilegeCount, pHint, NULL);
}

/*F********************************************************************************/
/*!
    \Function PrivilegeApiCheckPrivilegesAsyncWithUi

    \Description
        Check one or more privileges against the system or the first party service for an authorotative
        determination of the current privilege status.  This function will show appropriate error messages
        or commerce dialog to upsell product/privileges to the user.

    \Input  *pRef             - the module reference
    \Input   iUserIndex       - user index
    \Input  *pPrivileges      - array of privileges
    \Input   iPrivilegeCount  - the numnber of privileges pointed to by pPrivileges
    \Input  *pHint            - (optional, out) if provided, pHint will be filled with a non-authoratative result for the privilege check.  Unless the return value is zero (0), in which case, this value can be considered autorotative.
    \Input  *pUiMessage       - this parameter should be non-NULL for the UI functionality to be enabled, otherwise, no UI is displayed.  However, the value of the string is not used on PS4.
    
    \Output int32_t    - <0 = error, 0 = success, use *pHint for the result, >0 = the request id to pass into PrivilegeApiCheckResult()

    \Version 09/02/2013 (mcorcoran)
*/
/********************************************************************************F*/
int32_t PrivilegeApiCheckPrivilegesAsyncWithUi(PrivilegeApiRefT *pRef, int32_t iUserIndex, const PrivilegeApiPrivE *pPrivileges, int32_t iPrivilegeCount, int32_t *pHint, const char *pUiMessage)
{
    int32_t iRet = 0;

    if ((pPrivileges == NULL) || (iPrivilegeCount < 0))
    {
        NetPrintf(("privilegeapips4: PrivilegeApiCheckPrivilegesAsyncWithUi() Invalid argument.\n"));
        return(-1);
    }

    if (pRef->aUserContext[iUserIndex].iStatus != PRIVILEGEAPI_STATUS_NONE)
    {
        NetPrintf(("privilegeapips4: The user index (%i) is already busy requesting privileges from first party.\n", iUserIndex));
        return(-3);
    }

    pRef->aUserContext[iUserIndex].iStatus = PRIVILEGEAPI_STATUS_GRANTED;
    pRef->aUserContext[iUserIndex].iDialogOption = (pUiMessage == NULL ? PRIVILEGEAPI_SCE_STATE_RESTRICTED : PRIVILEGEAPI_SCE_STATE_DIALOG_PENDING);

    for (int32_t iIndex = 0; iIndex < iPrivilegeCount; iIndex++)
    {
        switch (pPrivileges[iIndex])
        {
            case PRIVILEGEAPI_PRIV_COMMUNICATION_VOICE_INGAME:
            case PRIVILEGEAPI_PRIV_COMMUNICATION_VOICE_SKYPE:
            case PRIVILEGEAPI_PRIV_VIDEO_COMMUNICATIONS:
            case PRIVILEGEAPI_PRIV_COMMUNICATIONS:
            {
                pRef->aUserContext[iUserIndex].iStatus = PRIVILEGEAPI_STATUS_IN_PROGRESS;
                pRef->aUserContext[iUserIndex].eChat = PRIVILEGEAPI_SCE_STATE_IN_PROGRESS;
                break;
            }
            case PRIVILEGEAPI_PRIV_USER_CREATED_CONTENT:
            {
                pRef->aUserContext[iUserIndex].iStatus = PRIVILEGEAPI_STATUS_IN_PROGRESS;
                pRef->aUserContext[iUserIndex].eUgc = PRIVILEGEAPI_SCE_STATE_IN_PROGRESS;
                break;
            }
            case PRIVILEGEAPI_PRIV_ONLINE_ACCESS:
            {
                pRef->aUserContext[iUserIndex].iStatus = PRIVILEGEAPI_STATUS_IN_PROGRESS;
                pRef->aUserContext[iUserIndex].eContent = PRIVILEGEAPI_SCE_STATE_IN_PROGRESS;
                break;
            }
            case PRIVILEGEAPI_PRIV_MULTIPLAYER_SESSIONS_REALTIME:
            {
                pRef->aUserContext[iUserIndex].iStatus = PRIVILEGEAPI_STATUS_IN_PROGRESS;
                pRef->aUserContext[iUserIndex].ePlus = PRIVILEGEAPI_SCE_STATE_IN_PROGRESS; 
                
                #ifdef DIRTYCODE_PS5
                pRef->aUserContext[iUserIndex].iSceNpPlusFeatures |= SCE_NP_PREMIUM_FEATURE_REALTIME_MULTIPLAY;
                #else   
                pRef->aUserContext[iUserIndex].iSceNpPlusFeatures |= SCE_NP_PLUS_FEATURE_REALTIME_MULTIPLAY;
                #endif

                break;
            }
            #if SCE_ORBIS_SDK_VERSION < 0x03508030u
            case PRIVILEGEAPI_PRIV_MULTIPLAYER_SESSIONS_ASYNC:
            {
                pRef->aUserContext[iUserIndex].iStatus = PRIVILEGEAPI_STATUS_IN_PROGRESS;
                pRef->aUserContext[iUserIndex].ePlus = PRIVILEGEAPI_SCE_STATE_IN_PROGRESS;
                pRef->aUserContext[iUserIndex].iSceNpPlusFeatures |= SCE_NP_PLUS_FEATURE_ASYNC_MULTIPLAY;
                break;
            }
            #endif
            default:
            {
                break;
            }
        }
    }

    if (pRef->aUserContext[iUserIndex].ePlus == PRIVILEGEAPI_SCE_STATE_IN_PROGRESS)
    {
        #ifdef DIRTYCODE_PS5
        NetConnControl('prem', iUserIndex, pRef->aUserContext[iUserIndex].iSceNpPlusFeatures, NULL, NULL);
        #else
        NetConnControl('pspl', iUserIndex, pRef->aUserContext[iUserIndex].iSceNpPlusFeatures, NULL, NULL);
        #endif
    }

    if (pHint != NULL)
    {
        iRet = PrivilegeApiCheckResult(pRef, iUserIndex + 1);
        if (iRet != PRIVILEGEAPI_STATUS_IN_PROGRESS)
        {
            *pHint = iRet;
            PrivilegeApiReleaseRequest(pRef, iUserIndex + 1);
            return(0);
        }

        *pHint = PRIVILEGEAPI_STATUS_GRANTED;
    }

    return(iUserIndex + 1);
}

/*F********************************************************************************/
/*!
    \Function PrivilegeApiCheckResult

    \Description
        Gets the current status of an active privilege check started with one of the
        PrivilegeApiCheckPrivilegesAsync() functions.

    \Input  *pRef       - the module reference
    \Input  iRequestId  - the request id returned from PrivilegeApiCheckPrivilegesAsync() or PrivilegeApiCheckPrivilegesAsyncWithUi()
    
    \Output int32_t    - <0 = error, >=0 = one or more of the PRIVILEGEAPI_STATUS_* flags

    \Version 09/02/2013 (mcorcoran)
*/
/********************************************************************************F*/
int32_t PrivilegeApiCheckResult(PrivilegeApiRefT *pRef, int32_t iRequestId)
{
    int32_t iRet = PRIVILEGEAPI_STATUS_NONE;
    int32_t iUserIndex = iRequestId - 1;

    if ((iUserIndex < 0) || (iUserIndex >= NETCONN_MAXLOCALUSERS) || (pRef->aUserContext[iUserIndex].iStatus == PRIVILEGEAPI_STATUS_NONE))
    {
        NetPrintf(("privilegeapips4: invalid request id (%d)\n", iRequestId));
        return(-1);
    }

    if (pRef->aUserContext[iUserIndex].iStatus == PRIVILEGEAPI_STATUS_IN_PROGRESS)
    {
        bool bAborted = false;
        uint8_t bUgcRestrictionErrorOccurred = FALSE;
        uint8_t bPlusPassErrorOccurred = FALSE;

        // first, check to see if we need to obtain any of the parental control restrictions
        if ((pRef->aUserContext[iUserIndex].eChat == PRIVILEGEAPI_SCE_STATE_IN_PROGRESS) ||
            (pRef->aUserContext[iUserIndex].eUgc == PRIVILEGEAPI_SCE_STATE_IN_PROGRESS) ||
            (pRef->aUserContext[iUserIndex].eContent == PRIVILEGEAPI_SCE_STATE_IN_PROGRESS))
        {
            #ifdef DIRTYCODE_PS5
            uint8_t bRestricted;
            iRet = NetConnStatus('crst', iUserIndex, &bRestricted, sizeof(bRestricted));

            if (iRet > 0)
            {
                pRef->aUserContext[iUserIndex].eUgc = (bRestricted ? pRef->aUserContext[iUserIndex].iDialogOption : PRIVILEGEAPI_SCE_STATE_GRANTED);
                pRef->aUserContext[iUserIndex].eChat = (bRestricted ? PRIVILEGEAPI_SCE_STATE_RESTRICTED : PRIVILEGEAPI_SCE_STATE_GRANTED);
                pRef->aUserContext[iUserIndex].eContent = (bRestricted ? PRIVILEGEAPI_SCE_STATE_RESTRICTED : PRIVILEGEAPI_SCE_STATE_GRANTED);
            }
            #else
            // check to see if we have obtained parental control settings yet
            SceNpParentalControlInfo parentalControlInfo;
            iRet = NetConnStatus('pctl', iUserIndex, &parentalControlInfo, sizeof(parentalControlInfo));

            if (iRet == SCE_NP_ERROR_AGE_RESTRICTION || pRef->aUserContext[iUserIndex].bIsUnderAge)
            {
                pRef->aUserContext[iUserIndex].bIsUnderAge = TRUE;
                if (pRef->aUserContext[iUserIndex].eChat == PRIVILEGEAPI_SCE_STATE_IN_PROGRESS)
                {
                    pRef->aUserContext[iUserIndex].eChat = pRef->aUserContext[iUserIndex].iDialogOption;
                }
                if (pRef->aUserContext[iUserIndex].eUgc == PRIVILEGEAPI_SCE_STATE_IN_PROGRESS)
                {
                    pRef->aUserContext[iUserIndex].eUgc = pRef->aUserContext[iUserIndex].iDialogOption;
                }
                if (pRef->aUserContext[iUserIndex].eContent == PRIVILEGEAPI_SCE_STATE_IN_PROGRESS)
                {
                    pRef->aUserContext[iUserIndex].eContent = PRIVILEGEAPI_SCE_STATE_RESTRICTED;
                }
            }
            else if (iRet > 0)
            {
                // we've obtained the parental control settings, now check them against the selected privileges for this request
                if (pRef->aUserContext[iUserIndex].eChat == PRIVILEGEAPI_SCE_STATE_IN_PROGRESS)
                {
                    pRef->aUserContext[iUserIndex].eChat = (parentalControlInfo.chatRestriction ? pRef->aUserContext[iUserIndex].iDialogOption : PRIVILEGEAPI_SCE_STATE_GRANTED);
                }
                if (pRef->aUserContext[iUserIndex].eUgc == PRIVILEGEAPI_SCE_STATE_IN_PROGRESS)
                {
                    pRef->aUserContext[iUserIndex].eUgc = (parentalControlInfo.ugcRestriction ? pRef->aUserContext[iUserIndex].iDialogOption : PRIVILEGEAPI_SCE_STATE_GRANTED);
                }
                if (pRef->aUserContext[iUserIndex].eContent == PRIVILEGEAPI_SCE_STATE_IN_PROGRESS)
                {
                    pRef->aUserContext[iUserIndex].eContent = (parentalControlInfo.contentRestriction ? PRIVILEGEAPI_SCE_STATE_RESTRICTED : PRIVILEGEAPI_SCE_STATE_GRANTED);
                }
            }
            #endif
            else if (iRet < 0)
            {
                bUgcRestrictionErrorOccurred = TRUE;
            }
        }

        if (pRef->aUserContext[iUserIndex].ePlus == PRIVILEGEAPI_SCE_STATE_IN_PROGRESS)
        {
            bool bAuthorized;
            #ifdef DIRTYCODE_PS5
            iRet = NetConnStatus('prem', iUserIndex, &bAuthorized, sizeof(bAuthorized));
            #else
            iRet = NetConnStatus('pspl', iUserIndex, &bAuthorized, sizeof(bAuthorized));
            #endif

            if (iRet == SCE_NP_ERROR_AGE_RESTRICTION || pRef->aUserContext[iUserIndex].bIsUnderAge == TRUE)
            {
                pRef->aUserContext[iUserIndex].bIsUnderAge = TRUE;
                pRef->aUserContext[iUserIndex].ePlus = PRIVILEGEAPI_SCE_STATE_RESTRICTED;
            }
            else if (iRet > 0)
            {
                pRef->aUserContext[iUserIndex].ePlus = (!bAuthorized ? pRef->aUserContext[iUserIndex].iDialogOption : PRIVILEGEAPI_SCE_STATE_GRANTED);
            }
            else if (iRet < 0)
            {
                // NetConnStatus 'pspl' can exit out with a negative number in the case of an error
                bPlusPassErrorOccurred = TRUE;
            }
        }

        #ifdef DIRTYCODE_PS5
        // display ugc restriction dialog
        if (_PrivilegeApiUpdateParentalProgress(pRef, &pRef->aUserContext[iUserIndex].eUgc, SCE_MSG_DIALOG_SYSMSG_TYPE_PSN_COMMUNICATION_RESTRICTION, iUserIndex) >= 0)
        {
            if ((pRef->aUserContext[iUserIndex].eUgc != PRIVILEGEAPI_SCE_STATE_DIALOG_PENDING) && (pRef->aUserContext[iUserIndex].eUgc != PRIVILEGEAPI_SCE_STATE_DIALOG_SHOWING))
            {
                // display plus dialog
                if (_PrivilegeApiUpdatePlusProgress(pRef, iUserIndex) < 0)
                {
                    // if an error is encounter showing the dialog treat as aborted
                    bAborted = true;
                }
            }
        }     
        #else
        // dsiplay chat restriction dialog
        if (_PrivilegeApiUpdateParentalProgress(pRef, &pRef->aUserContext[iUserIndex].eChat, SCE_MSG_DIALOG_SYSMSG_TYPE_TRC_PSN_CHAT_RESTRICTION, iUserIndex) >= 0)
        {
            if ((pRef->aUserContext[iUserIndex].eChat != PRIVILEGEAPI_SCE_STATE_DIALOG_PENDING) && (pRef->aUserContext[iUserIndex].eChat != PRIVILEGEAPI_SCE_STATE_DIALOG_SHOWING))
            {
                // dsiplay ugc restriction dialog
                if (_PrivilegeApiUpdateParentalProgress(pRef, &pRef->aUserContext[iUserIndex].eUgc, SCE_MSG_DIALOG_SYSMSG_TYPE_TRC_PSN_UGC_RESTRICTION, iUserIndex) >= 0)
                {
                    if ((pRef->aUserContext[iUserIndex].eUgc != PRIVILEGEAPI_SCE_STATE_DIALOG_PENDING) && (pRef->aUserContext[iUserIndex].eUgc != PRIVILEGEAPI_SCE_STATE_DIALOG_SHOWING))
                    {
                        // display plus dialog
                        if (_PrivilegeApiUpdatePlusProgress(pRef, iUserIndex) < 0)
                        {
                            // if an error is encounter showing the dialog treat as aborted
                            bAborted = true;
                        }
                    }
                }
            }
        }
        #endif

        if (!(pRef->aUserContext[iUserIndex].eChat & PRIVILEGEAPI_SCE_STATE_IN_PROGRESS) &&
            !(pRef->aUserContext[iUserIndex].eUgc & PRIVILEGEAPI_SCE_STATE_IN_PROGRESS) &&
            !(pRef->aUserContext[iUserIndex].eContent & PRIVILEGEAPI_SCE_STATE_IN_PROGRESS) &&
            !(pRef->aUserContext[iUserIndex].ePlus & PRIVILEGEAPI_SCE_STATE_IN_PROGRESS))
        {
            if ((pRef->aUserContext[iUserIndex].eChat == PRIVILEGEAPI_SCE_STATE_GRANTED) &&
                (pRef->aUserContext[iUserIndex].eUgc == PRIVILEGEAPI_SCE_STATE_GRANTED) &&
                (pRef->aUserContext[iUserIndex].eContent == PRIVILEGEAPI_SCE_STATE_GRANTED) &&
                (pRef->aUserContext[iUserIndex].ePlus == PRIVILEGEAPI_SCE_STATE_GRANTED))
            {
                pRef->aUserContext[iUserIndex].iStatus = PRIVILEGEAPI_STATUS_GRANTED;
            }
            else
            {
                pRef->aUserContext[iUserIndex].iStatus = PRIVILEGEAPI_STATUS_RESTRICTED;
                if (bAborted)
                {
                    pRef->aUserContext[iUserIndex].iStatus |= PRIVILEGEAPI_STATUS_ABORTED;
                }
            }
        }

        // An error occurred during the plus pass check... prevent a softlock
        if(bPlusPassErrorOccurred || bUgcRestrictionErrorOccurred)
        {
            pRef->aUserContext[iUserIndex].iStatus = PRIVILEGEAPI_STATUS_ERROR;
        }
    }

    return(pRef->aUserContext[iUserIndex].iStatus);
}

/*F********************************************************************************/
/*!
    \Function PrivilegeApiReleaseRequest

    \Description
        Must be called after you are done with a request id returned
        from PrivilegeApiCheckPrivilegesAsync() or PrivilegeApiCheckPrivilegesAsyncWithUi().

    \Input  *pRef       - the module reference
    \Input  iRequestId  - the request id
    
    \Output int32_t     - >=0 = success, <0 = error

    \Version 09/02/2013 (mcorcoran)
*/
/********************************************************************************F*/
int32_t PrivilegeApiReleaseRequest(PrivilegeApiRefT *pRef, int32_t iRequestId)
{
    int32_t iUserIndex = iRequestId - 1;

    if ((iUserIndex < 0) || (iUserIndex >= NETCONN_MAXLOCALUSERS) || (pRef->aUserContext[iUserIndex].iStatus == PRIVILEGEAPI_STATUS_NONE))
    {
        NetPrintf(("privilegeapips4: invalid request id (%d)\n", iRequestId));
        return(-1);
    }

    if (pRef->aUserContext[iUserIndex].iStatus == PRIVILEGEAPI_STATUS_IN_PROGRESS)
    {
        NetPrintf(("privilegeapips4: the specified request id (%d) is currently busy, use PrivilegeApiAbort()\n", iRequestId));
        return(-2);
    }

    ds_memclr(&pRef->aUserContext[iUserIndex], sizeof(PrivilegeApiUserContextT));

    return(0);
}

/*F********************************************************************************/
/*!
    \Function PrivilegeApiAbort

    \Description
        Aborts and releases a request.

    \Input  *pRef       - the module reference
    \Input  iRequestId  - the request id
    
    \Output int32_t     - >=0 = success, <0 = error

    \Version 09/02/2013 (mcorcoran)
*/
/********************************************************************************F*/
int32_t PrivilegeApiAbort(PrivilegeApiRefT *pRef, int32_t iRequestId)
{
    int32_t iUserIndex = iRequestId - 1;

    if ((iUserIndex < 0) || (iUserIndex >= NETCONN_MAXLOCALUSERS) || (pRef->aUserContext[iUserIndex].iStatus == PRIVILEGEAPI_STATUS_NONE))
    {
        NetPrintf(("privilegeapips4: invalid request id (%d)\n", iRequestId));
        return(-1);
    }

    if (pRef->aUserContext[iUserIndex].iStatus == PRIVILEGEAPI_STATUS_IN_PROGRESS)
    {
        if ((pRef->aUserContext[iUserIndex].eChat == PRIVILEGEAPI_SCE_STATE_DIALOG_SHOWING) ||
            (pRef->aUserContext[iUserIndex].eUgc == PRIVILEGEAPI_SCE_STATE_DIALOG_SHOWING))
        {
            sceMsgDialogClose();
            if (pRef->aUserContext[iUserIndex].eChat == PRIVILEGEAPI_SCE_STATE_DIALOG_SHOWING)
            {
                pRef->aUserContext[iUserIndex].eChat = PRIVILEGEAPI_SCE_STATE_RESTRICTED;
            }
            if (pRef->aUserContext[iUserIndex].eUgc == PRIVILEGEAPI_SCE_STATE_DIALOG_SHOWING)
            {
                pRef->aUserContext[iUserIndex].eUgc = PRIVILEGEAPI_SCE_STATE_RESTRICTED;
            }
        }

        if (pRef->aUserContext[iUserIndex].ePlus == PRIVILEGEAPI_SCE_STATE_DIALOG_SHOWING)
        {
            sceNpCommerceDialogClose();
            pRef->aUserContext[iUserIndex].ePlus = PRIVILEGEAPI_SCE_STATE_RESTRICTED;
        }
    }

    PrivilegeApiReleaseRequest(pRef, iRequestId);

    if (pRef->aUserContext[iUserIndex].iStatus == PRIVILEGEAPI_STATUS_IN_PROGRESS)
    {
        pRef->aUserContext[iUserIndex].iStatus = PRIVILEGEAPI_STATUS_NONE;
    }

    return(0);
}
