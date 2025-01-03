/*H*************************************************************************************/
/*!
    \File privilegeapigdk.cpp

    \Description
        Implements the GDK specific privilege checking via XUser

    \Copyright
        Copyright (c) Electronic Arts 2020

    \Version 01/27/2020 (eesponda)
*/
/********************************************************************************H******/

/*** Include files *********************************************************************/

#include "DirtySDK/platform.h"

#define WIN32_LEAN_AND_MEAN 1
#include <Windows.h>
#include <string.h>
#include <XUser.h>

#include "DirtySDK/dirtysock/dirtyerr.h"
#include "DirtySDK/dirtysock/dirtymem.h"
#include "DirtySDK/dirtysock/dirtylib.h"
#include "DirtySDK/dirtysock/netconn.h"
#include "DirtySDK/misc/privilegeapi.h"

/*** Defines ***************************************************************************/

#define PRIVILEGEAPI_MAX_PRIVILEGES (32) //!< max amount of privileges that we support

/*** Macros ****************************************************************************/

/*** Type Definitions ******************************************************************/

typedef struct PrivilegeApiRequestT
{
    XAsyncBlock AsyncBlock;         //!< async block for processing any async operations
    int32_t iResult;                //!< result of the privilege checks
    uint8_t bUseUi;                 //!< should a UI be displayed to resolve issues
    uint8_t _pad[3];

    XUserHandle pUser;              //!< user handle for performing checks
    XUserPrivilege aPrivileges[PRIVILEGEAPI_MAX_PRIVILEGES];    //!< privileges we are checking
    int32_t iPrivilegeCount;        //!< number of privileges to check
    int32_t iCurrentPrivilege;      //!< current privilege we are checking
} PrivilegeApiRequestT;

struct PrivilegeApiRefT
{
    int32_t iMemGroup;          //!< dirtymem memory group
    void *pMemGroupUserData;    //!< dirtymem memory group user data
    PrivilegeApiRequestT aUserContexts[NETCONN_MAXLOCALUSERS];
};

/*** Function Prototypes ***************************************************************/

/*** Variables *************************************************************************/

/*** Private functions *****************************************************************/

/*F*************************************************************************************/
/*!
    \Function _PrivilegeApiConvertToNativePriv

    \Description
        Converts a privilege to a native privilege.

    \Input iPrivilege           - the privilege id as defined by the PrivilegeApi
    \Input *pNativePrivilege    - gets set to the XPRIVILEGE matching the iPrivilege value, if there is a direct match

    \Output
        bool                    - true if there was a matching native privilege, otherwise false

    \Version 01/27/2020 (eesponda)
*/
/*************************************************************************************F*/
static bool _PrivilegeApiConvertToNativePriv(PrivilegeApiPrivE ePrivilege, XUserPrivilege *pNativePrivilege)
{
    bool bIsNativePriv = true;

    switch (ePrivilege)
    {
        case PRIVILEGEAPI_PRIV_COMMUNICATION_VOICE_INGAME:
        case PRIVILEGEAPI_PRIV_COMMUNICATION_VOICE_SKYPE:
        case PRIVILEGEAPI_PRIV_VIDEO_COMMUNICATIONS:
        case PRIVILEGEAPI_PRIV_COMMUNICATIONS:
        {
            *pNativePrivilege = XUserPrivilege::Communications;
            break;
        }
        case PRIVILEGEAPI_PRIV_USER_CREATED_CONTENT:
        {
            *pNativePrivilege = XUserPrivilege::UserGeneratedContent;
            break;
        }
        case PRIVILEGEAPI_PRIV_MULTIPLAYER_SESSIONS_REALTIME:
        {
            *pNativePrivilege = XUserPrivilege::Multiplayer;
            break;
        }
        case PRIVILEGEAPI_PRIV_MULTIPLAYER_PARTIES:
        {
            *pNativePrivilege = XUserPrivilege::MultiplayerParties;
            break;
        }
        case PRIVILEGEAPI_PRIV_CLOUD_GAMING_MANAGE_SESSION:
        {
            *pNativePrivilege = XUserPrivilege::CloudManageSession;
            break;
        }
        case PRIVILEGEAPI_PRIV_CLOUD_GAMING_JOIN_SESSION:
        {
            *pNativePrivilege = XUserPrivilege::CloudJoinSession;
            break;
        }
        case PRIVILEGEAPI_PRIV_CLOUD_SAVED_GAMES:
        {
            *pNativePrivilege = XUserPrivilege::CloudSavedGames;
            break;
        }
        case PRIVILEGEAPI_PRIV_GAME_DVR:
        {
            *pNativePrivilege = XUserPrivilege::GameDvr;
            break;
        }
        case PRIVILEGEAPI_PRIV_SOCIAL_NETWORK_SHARING:
        {
            *pNativePrivilege = XUserPrivilege::SocialNetworkSharing;
            break;
        }
        case PRIVILEGEAPI_PRIV_MULTIPLAYER_SESSIONS_ASYNC:
        {
            *pNativePrivilege = XUserPrivilege::Sessions;
            break;
        }
        default:
        {
            bIsNativePriv = false;
            break;
        }
    }

    return(bIsNativePriv);
}

/*** Public functions ******************************************************************/

/*F*************************************************************************************/
/*!
    \Function PrivilegeApiCreate

    \Description
        Create the PrivilegeApi

    \Output
        PrivilegeApiRefT *  - the module reference, or NULL on error

    \Version 01/27/2020 (eesponda)
*/
/*************************************************************************************F*/
PrivilegeApiRefT *PrivilegeApiCreate(void)
{
    PrivilegeApiRefT *pRef;
    int32_t iMemGroup;
    void *pMemGroupUserData;

    DirtyMemGroupQuery(&iMemGroup, &pMemGroupUserData);

    // allocate and init module state
    if ((pRef = (PrivilegeApiRefT*)DirtyMemAlloc(sizeof(*pRef), PRIVILEGEAPI_MEMID, iMemGroup, pMemGroupUserData)) == NULL)
    {
        NetPrintf(("privilegeapixboxone: failed to allocate module state.\n"));
        return(NULL);
    }
    ds_memclr(pRef, sizeof(*pRef));
    pRef->iMemGroup = iMemGroup;
    pRef->pMemGroupUserData = pMemGroupUserData;

    return(pRef);
}

/*F*************************************************************************************/
/*!
    \Function PrivilegeApiDestroy

    \Description
        Destroy the PrivilegeApi

    \Input *pRef    - the module reference

    \Output
        int32_t     - always returns zero

    \Version 01/27/2020 (eesponda)
*/
/*************************************************************************************F*/
int32_t PrivilegeApiDestroy(PrivilegeApiRefT *pRef)
{
    for (int32_t iUserIndex = 0; iUserIndex < NETCONN_MAXLOCALUSERS; iUserIndex += 1)
    {
        XAsyncCancel(&pRef->aUserContexts[iUserIndex].AsyncBlock);
    }

    DirtyMemFree(pRef, PRIVILEGEAPI_MEMID, pRef->iMemGroup, pRef->pMemGroupUserData);
    return(0);
}

/*F*************************************************************************************/
/*!
    \Function PrivilegeApiCheckPrivilegesAsync

    \Description
        Check one or more privileges against the system or the first party service for an authorotative
        determination of the current privilege status.  No UI is shown when using this function.

    \Input *pRef            - the module reference
    \Input  iUserIndex      - user index
    \Input *pPrivileges     - array of privileges
    \Input  iPrivilegeCount - the numnber of privileges pointed to by pPrivileges
    \Input *pHint           - unused

    \Output
        int32_t             - negative=error, positive=returns request id to pass into PrivilegeApiCheckResult()

    \Version 01/27/2020 (eesponda)
*/
/*************************************************************************************F*/
int32_t PrivilegeApiCheckPrivilegesAsync(PrivilegeApiRefT *pRef, int32_t iUserIndex, const PrivilegeApiPrivE *pPrivileges, int32_t iPrivilegeCount, int32_t *pHint)
{
    return(PrivilegeApiCheckPrivilegesAsyncWithUi(pRef, iUserIndex, pPrivileges, iPrivilegeCount, pHint, NULL));
}

/*F*************************************************************************************/
/*!
    \Function PrivilegeApiCheckPrivilegesAsyncWithUi

    \Description
        Check one or more privileges against the system or the first party service for an authorotative
        determination of the current privilege status.  This function will show appropriate error messages
        or commerce dialog to upsell product/privileges to the user.

    \Input *pRef            - the module reference
    \Input iUserIndex       - user index
    \Input *pPrivileges     - array of privileges
    \Input iPrivilegeCount  - the number of privileges pointed to by pPrivileges
    \Input *pHint           - unused
    \Input *pUiMessage      - this parameter should be non-NULL for the UI functionality to be enabled, otherwise, no UI is displayed.  However, the value of the string is not used on GDK.

    \Output
        int32_t             - negative=error, positive=returns request id to pass into PrivilegeApiCheckResult()

    \Version 01/27/2020 (eesponda)
*/
/*************************************************************************************F*/
int32_t PrivilegeApiCheckPrivilegesAsyncWithUi(PrivilegeApiRefT *pRef, int32_t iUserIndex, const PrivilegeApiPrivE *pPrivileges, int32_t iPrivilegeCount, int32_t *pHint, const char *pUiMessage)
{
    PrivilegeApiRequestT *pRequest = &pRef->aUserContexts[iUserIndex];

    // check if we can hold a request currently
    if (pRequest->pUser != NULL)
    {
        NetPrintf(("privilegeapigdk: [%p] request for user index %d, currently in-progress and cannot queue another\n", pRef, iUserIndex));
        return(-1);
    }
    // grab the user to make the request
    if (NetConnStatus('xusr', iUserIndex, &pRequest->pUser, sizeof(pRequest->pUser)) < 0)
    {
        NetPrintf(("privilegeapigdk: [%p] invalid user index %d\n", pRef, iUserIndex));
        return(-3);
    }

    // convert the privileges
    for (int32_t iPrivilege = 0; (iPrivilege < iPrivilegeCount) && (pRequest->iPrivilegeCount < PRIVILEGEAPI_MAX_PRIVILEGES); iPrivilege += 1)
    {
        if (_PrivilegeApiConvertToNativePriv(pPrivileges[iPrivilege], &pRequest->aPrivileges[pRequest->iPrivilegeCount]))
        {
            pRequest->iPrivilegeCount += 1;
        }
        else
        {
            NetPrintf(("privilegeapigdk: [%p] could not convert privilege %d to native privilege to check privilege, it is unsupported on this platform\n", pRef, pPrivileges[iPrivilege]));
            return(-4);
        }
    }
    pRequest->bUseUi = (pUiMessage != NULL);

    // convert user index to request id
    return(iUserIndex+1);
}

/*F*************************************************************************************/
/*!
    \Function PrivilegeApiCheckResult

    \Description
        Gets the current status of an active privilege check started with one of the
        PrivilegeApiCheckPrivilegesAsync() functions.

    \Input *pRef       - the module reference
    \Input iRequestId  - the request id returned from PrivilegeApiCheckPrivilegesAsync() or PrivilegeApiCheckPrivilegesAsyncWithUi()

    \Output
        int32_t         - negative=error, positive=PRIVILEGEAPI_STATUS_* flags

    \Version 01/27/2020 (eesponda)
*/
/*************************************************************************************F*/
int32_t PrivilegeApiCheckResult(PrivilegeApiRefT *pRef, int32_t iRequestId)
{
    PrivilegeApiRequestT *pRequest = &pRef->aUserContexts[iRequestId-1];
    HRESULT hResult;
    bool bHasPrivilege;
    XUserPrivilegeDenyReason eDenyReason;

    // check to make this is a valid request
    if (pRequest->pUser == NULL)
    {
        NetPrintf(("pritivlegeapigdk: [%p] invalid request id %d specified\n", pRef, iRequestId));
        return(-1);
    }

    // check the async operation, if one is pending
    if ((hResult = XAsyncGetStatus(&pRequest->AsyncBlock, false)) == E_PENDING)
    {
        return(PRIVILEGEAPI_STATUS_IN_PROGRESS);
    }
    else if (FAILED(hResult))
    {
        NetPrintf(("privilegeapigdk: [%p] resolve privilege with ui failed (err=%s)\n", pRef, DirtyErrGetName(hResult)));
        return(PRIVILEGEAPI_STATUS_ERROR);
    }

    // if all requests are complete return the result
    if (pRequest->iCurrentPrivilege == pRequest->iPrivilegeCount)
    {
        return(pRequest->iResult);
    }

    // check the current privilege
    if (FAILED(hResult = XUserCheckPrivilege(pRequest->pUser, XUserPrivilegeOptions::None, pRequest->aPrivileges[pRequest->iCurrentPrivilege], &bHasPrivilege, &eDenyReason)))
    {
        NetPrintf(("privilegeapigdk: [%p] failed to check privileges (err=%s)\n", pRef, DirtyErrGetName(hResult)));
        return(-4);
    }

    // handle the results and kick off new requests if necessary
    if (bHasPrivilege)
    {
        pRequest->iResult = PRIVILEGEAPI_STATUS_GRANTED;
    }
    else
    {
        pRequest->iResult = PRIVILEGEAPI_STATUS_RESTRICTED;
        if (eDenyReason == XUserPrivilegeDenyReason::PurchaseRequired)
        {
            pRequest->iResult |= PRIVILEGEAPI_STATUS_PURCHASE_REQUIRED;
        }
        else if (eDenyReason == XUserPrivilegeDenyReason::Banned)
        {
            pRequest->iResult |= PRIVILEGEAPI_STATUS_BANNED;
        }
    }

    // if the privilege check failed and we shoud use ui, kick off the async request
    if (!bHasPrivilege && pRequest->bUseUi)
    {
        if (FAILED(hResult = XUserResolvePrivilegeWithUiAsync(pRequest->pUser, XUserPrivilegeOptions::None, pRequest->aPrivileges[pRequest->iCurrentPrivilege], &pRequest->AsyncBlock)))
        {
            NetPrintf(("privilegeapigdk: [%p] failed to launch ui for resolve privileges (err=%s)\n", pRef, DirtyErrGetName(hResult)));
            return(PRIVILEGEAPI_STATUS_ERROR);
        }
    }
    pRequest->iCurrentPrivilege += 1;
    return(PRIVILEGEAPI_STATUS_IN_PROGRESS);
}

/*F*************************************************************************************/
/*!
    \Function PrivilegeApiReleaseRequest

    \Description
        Must be called after you are done with a request id returned
        from PrivilegeApiCheckPrivilegesAsync() or PrivilegeApiCheckPrivilegesAsyncWithUi().

    \Input *pRef        - the module reference
    \Input iRequestId   - the request id

    \Output
        int32_t         - zero=success, negative=error

    \Version 01/27/2020 (eespond)
*/
/*************************************************************************************F*/
int32_t PrivilegeApiReleaseRequest(PrivilegeApiRefT *pRef, int32_t iRequestId)
{
    PrivilegeApiRequestT *pRequest = &pRef->aUserContexts[iRequestId-1];
    if (pRequest->pUser == NULL)
    {
        NetPrintf(("privilegeapigdk: [%p] attempting to release an invalid request %d\n", pRef, iRequestId));
        return(-1);
    }
    ds_memclr(&pRef->aUserContexts[iRequestId-1], sizeof(pRef->aUserContexts[iRequestId-1]));
    return(0);
}

/*F*************************************************************************************/
/*!
    \Function PrivilegeApiAbort

    \Description
        Aborts and releases a request.

    \Input *pRef        - the module reference
    \Input iRequestId   - the request id

    \Output
        int32_t         - zero=success, negative=error

    \Version 01/27/2020 (eesponda)
*/
/*************************************************************************************F*/
int32_t PrivilegeApiAbort(PrivilegeApiRefT *pRef, int32_t iRequestId)
{
    PrivilegeApiRequestT *pRequest = &pRef->aUserContexts[iRequestId-1];
    if (pRequest->pUser == NULL)
    {
        NetPrintf(("privilegeapigdk: [%p] attempting to abort an invalid request %d\n", pRef, iRequestId));
        return(-1);
    }
    XAsyncCancel(&pRef->aUserContexts[iRequestId-1].AsyncBlock);
    PrivilegeApiReleaseRequest(pRef, iRequestId);
    return(0);
}
