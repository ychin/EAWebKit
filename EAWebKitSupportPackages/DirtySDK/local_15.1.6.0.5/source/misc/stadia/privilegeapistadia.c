/*H*************************************************************************************/
/*!
    \File privilegeapistadia.c

    \Description
        Implements the Stadia specific privilege checking via feature restrictions API

    \Copyright
        Copyright (c) Electronic Arts 2020

    \Version 04/20/2020 (eesponda)
*/
/********************************************************************************H******/

/*** Include files *********************************************************************/

#include "DirtySDK/platform.h"

EA_DISABLE_CLANG_WARNING(-Wstrict-prototypes)
#include <ggp_c/feature_restriction.h>
#include <ggp_c/status.h>
EA_RESTORE_CLANG_WARNING()

#include "DirtySDK/dirtysock/dirtyerr.h"
#include "DirtySDK/dirtysock/dirtymem.h"
#include "DirtySDK/dirtysock/dirtylib.h"
#include "DirtySDK/dirtysock/netconn.h"
#include "DirtySDK/misc/privilegeapi.h"

/*** Defines ***************************************************************************/

#define PRIVILEGEAPI_MAX_PRIVILEGES (8)

/*** Macros ****************************************************************************/

/*** Type Definitions ******************************************************************/

typedef struct PrivilegeApiRequestT
{
    int32_t iResult;                //!< result of the privilege checks
    uint8_t bUseUi;                 //!< should a UI be displayed to resolve issues
    uint8_t _pad[3];

    GgpPlayerId uUser;              //!< user we are performing checks on
    GgpGamerFeature aPrivileges[PRIVILEGEAPI_MAX_PRIVILEGES];   //!< privileges we are checking
    int32_t iPrivilegeCount;        //!< number of privileges to check
    int32_t iCurrentPrivilege;      //!< current privilege we are checking
} PrivilegeApiRequestT;

struct PrivilegeApiRefT
{
    int32_t iMemGroup;          //!< dirtymem memory group
    void *pMemGroupUserData;    //!< dirtymem memory group user data
    PrivilegeApiRequestT aUserContexts[NETCONN_MAXLOCALUSERS]; //!< requests one for each user
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

    \Version 04/20/2020 (eesponda)
*/
/*************************************************************************************F*/
static uint8_t _PrivilegeApiConvertToNativePriv(PrivilegeApiPrivE ePrivilege, GgpGamerFeature *pNativePrivilege)
{
    uint8_t bIsNativePriv = TRUE;

    switch (ePrivilege)
    {
        case PRIVILEGEAPI_PRIV_COMMUNICATION_VOICE_INGAME:
        case PRIVILEGEAPI_PRIV_COMMUNICATION_VOICE_SKYPE:
        {
            *pNativePrivilege = kGgpGamerFeature_VoiceChat;
            break;
        }
        case PRIVILEGEAPI_PRIV_MULTIPLAYER_SESSIONS_REALTIME:
        {
            *pNativePrivilege = kGgpGamerFeature_Multiplayer;
            break;
        }
        default:
        {
            bIsNativePriv = FALSE;
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

    \Version 04/20/2020 (eesponda)
*/
/*************************************************************************************F*/
PrivilegeApiRefT *PrivilegeApiCreate(void)
{
    PrivilegeApiRefT *pRef;
    int32_t iMemGroup;
    void *pMemGroupUserData;

    DirtyMemGroupQuery(&iMemGroup, &pMemGroupUserData);

    // allocate and init module state
    if ((pRef = (PrivilegeApiRefT *)DirtyMemAlloc(sizeof(*pRef), PRIVILEGEAPI_MEMID, iMemGroup, pMemGroupUserData)) == NULL)
    {
        NetPrintf(("privilegeapistadia: failed to allocate module state.\n"));
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

    \Version 04/20/2020 (eesponda)
*/
/*************************************************************************************F*/
int32_t PrivilegeApiDestroy(PrivilegeApiRefT *pRef)
{
    DirtyMemFree(pRef, PRIVILEGEAPI_MEMID, pRef->iMemGroup, pRef->pMemGroupUserData);
    return(0);
}

/*F*************************************************************************************/
/*!
    \Function PrivilegeApiCheckPrivilegesAsync

    \Description
        Check one or more privileges against the system or the first party service for an authoritative
        determination of the current privilege status.  No UI is shown when using this function.

    \Input *pRef            - the module reference
    \Input  iUserIndex      - user index
    \Input *pPrivileges     - array of privileges
    \Input  iPrivilegeCount - the numnber of privileges pointed to by pPrivileges
    \Input *pHint           - unused

    \Output
        int32_t             - negative=error, positive=returns request id to pass into PrivilegeApiCheckResult()

    \Version 04/20/2020 (eesponda)
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
        Check one or more privileges against the system or the first party service for an authoritative
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

    \Version 04/20/2020 (eesponda)
*/
/*************************************************************************************F*/
int32_t PrivilegeApiCheckPrivilegesAsyncWithUi(PrivilegeApiRefT *pRef, int32_t iUserIndex, const PrivilegeApiPrivE *pPrivileges, int32_t iPrivilegeCount, int32_t *pHint, const char *pUiMessage)
{
    int32_t iPrivilege;
    PrivilegeApiRequestT *pRequest = &pRef->aUserContexts[iUserIndex];

    // check if we can hold a request currently
    if (pRequest->uUser != 0)
    {
        NetPrintf(("privilegeapistadia: [%p] request for user index %d, currently in-progress and cannot queue another\n", pRef, iUserIndex));
        return(-1);
    }
    // grab the user to make the request
    if (NetConnStatus('gpid', iUserIndex, &pRequest->uUser, sizeof(pRequest->uUser)) < 0)
    {
        NetPrintf(("privilegeapigdk: [%p] invalid user index %u\n", pRef, iUserIndex));
        return(-2);
    }

    // convert the privileges
    for (iPrivilege = 0; (iPrivilege < iPrivilegeCount) && (pRequest->iPrivilegeCount < PRIVILEGEAPI_MAX_PRIVILEGES); iPrivilege += 1)
    {
        if (_PrivilegeApiConvertToNativePriv(pPrivileges[iPrivilege], &pRequest->aPrivileges[pRequest->iPrivilegeCount]))
        {
            pRequest->iPrivilegeCount += 1;
        }
        else
        {
            NetPrintf(("privilegeapistadia: [%p] could not convert privilege %d to native privilege to check privilege, it is unsupported on this platform\n", pRef, pPrivileges[iPrivilege]));
            ds_memclr(pRequest, sizeof(*pRequest));
            return(-3);
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

    \Version 04/20/2020 (eesponda)
*/
/*************************************************************************************F*/
int32_t PrivilegeApiCheckResult(PrivilegeApiRefT *pRef, int32_t iRequestId)
{
    PrivilegeApiRequestT *pRequest;
    uint8_t bHasPrivilege;
    GgpStatus Status;
    char strError[256];

    // check to make this is a valid request
    if ((iRequestId <= 0) || (iRequestId > NETCONN_MAXLOCALUSERS))
    {
        NetPrintf(("privilegeapistadia: [%p] invalid request id %d specified (invalid index)\n", pRef, iRequestId));
        return(-1);
    }
    if (pRef->aUserContexts[iRequestId-1].uUser == 0)
    {
        NetPrintf(("privilegeapistadia: [%p] invalid request id %d specified (request not in progress)\n", pRef, iRequestId));
        return(-1);
    }
    pRequest = &pRef->aUserContexts[iRequestId-1];

    // if all requests are complete return the result
    if (pRequest->iCurrentPrivilege == pRequest->iPrivilegeCount)
    {
        return(pRequest->iResult);
    }

    // check the current privilege
    bHasPrivilege = !GgpIsFeatureRestricted(pRequest->uUser, pRequest->aPrivileges[pRequest->iCurrentPrivilege], &Status);

    // set the appropriate result
    if (bHasPrivilege)
    {
        pRequest->iResult = PRIVILEGEAPI_STATUS_GRANTED;
    }
    else if (Status.status_code == kGgpStatusCode_Ok)
    {
        pRequest->iResult = PRIVILEGEAPI_STATUS_RESTRICTED;
    }
    else
    {
        GgpGetStatusMessage(Status.message_token, strError, sizeof(strError));
        NetPrintf(("privilegeapistadia: [%p] failed to check privileges (err=%s)\n", pRef, strError));
        return(PRIVILEGEAPI_STATUS_ERROR);
    }

    // if the privilege check failed and we should use ui, show the overlay
    if (!bHasPrivilege && pRequest->bUseUi && !GgpShowFeatureRestrictionOverlay(pRequest->uUser, pRequest->aPrivileges[pRequest->iCurrentPrivilege], &Status))
    {
        GgpGetStatusMessage(Status.message_token, strError, sizeof(strError));
        NetPrintf(("privilegeapistadia: [%p] failed to launch overlay for resolving privileges (err=%s)\n", pRef, strError));
        return(PRIVILEGEAPI_STATUS_ERROR);
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

    \Version 04/20/2020 (eesponda)
*/
/*************************************************************************************F*/
int32_t PrivilegeApiReleaseRequest(PrivilegeApiRefT *pRef, int32_t iRequestId)
{
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

    \Version 04/20/2020 (eesponda)
*/
/*************************************************************************************F*/
int32_t PrivilegeApiAbort(PrivilegeApiRefT *pRef, int32_t iRequestId)
{
    PrivilegeApiReleaseRequest(pRef, iRequestId);
    return(0);
}
