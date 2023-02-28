/*H********************************************************************************/
/*!
    \File privilegeapixboxone.cpp

    \Description
        Implements the xboxone specific privilege checking via Product::CheckPrivilegesAsync().

    \Copyright
        Copyright (c) Electronic Arts 2012

    \Version 09/02/2013 (mcorcoran) First Version
*/
/********************************************************************************H*/

/*** Include files ***********************************************************/
#include "DirtySDK/platform.h"

#include <xdk.h>
#include <string.h>
#include <collection.h>

#include "DirtySDK/misc/privilegeapi.h"

#include "DirtySDK/dirtysock/dirtymem.h"
#include "DirtySDK/dirtysock/dirtylib.h"
#include "DirtySDK/dirtysock/netconn.h"

using namespace Platform;
using namespace Windows::Data;
using namespace Windows::Foundation;
using namespace Windows::Foundation::Collections;
using namespace Windows::Xbox::ApplicationModel::Store;
using namespace Windows::Xbox::System;

/*** Defines ***************************************************************************/
#define PRIVILEGEAPI_MAX_ASYNC_REQUESTS (50)

/*** Macros ****************************************************************************/

/*** Type Definitions ******************************************************************/

typedef struct PrivilegeApiAsyncRequestT
{
    IAsyncOperation<Windows::Xbox::ApplicationModel::Store::PrivilegeCheckResult> ^refAsyncOp;
    int32_t bAdditionalChecks;
    int32_t iStatus;
} PrivilegeApiAsyncRequestT;

struct PrivilegeApiRefT
{
    int32_t                iMemGroup;                                 //!< dirtymem memory group
    void                  *pMemGroupUserData;                         //!< dirtymem memory group user data

    PrivilegeApiAsyncRequestT aAsyncRequests[PRIVILEGEAPI_MAX_ASYNC_REQUESTS];
};

/*** Function Prototypes ***************************************************************/

/*** Variables *************************************************************************/

/*** Private functions ******************************************************/

/*F********************************************************************************/
/*!
    \Function _PrivilegeApiGetAvailableSlot

    \Description
        Gets an available index in the pRef->aAsyncRequests array.

    \Input  *pRef       - the module reference
    
    \Output int32_t     - >=0 = the slot index, <0 = no slots available

    \Version 09/02/2013 (mcorcoran)
*/
/********************************************************************************F*/
static int32_t _PrivilegeApiGetAvailableSlot(PrivilegeApiRefT *pRef)
{
    int32_t iResult;

    for (iResult = 0; iResult < PRIVILEGEAPI_MAX_ASYNC_REQUESTS; iResult++)
    {
        if (pRef->aAsyncRequests[iResult].iStatus == PRIVILEGEAPI_STATUS_NONE)
        {
            pRef->aAsyncRequests[iResult].iStatus = PRIVILEGEAPI_STATUS_IN_PROGRESS;
            break;
        }
    }

    if (iResult == PRIVILEGEAPI_MAX_ASYNC_REQUESTS)
    {
        iResult = -1;
    }

    return(iResult);
}

/*F********************************************************************************/
/*!
    \Function _PrivilegeApiConvertToNativePriv

    \Description
        Converts a privilege to a native privilege.

    \Input  iPrivilege                 - the privilege id as defined by the PrivilegeApi
    \Input *pNativePrivilege           - gets set to the XPRIVILEGE matching the iPrivilege value, if there is a direct match
    \Input *pAdditionalChecksRequired  - gets set to TRUE if the privilege will require additional privacy checks by the user
    
    \Output int32_t     - TRUE if there was a matching native privilege, otherwise FALSE

    \Version 09/02/2013 (mcorcoran)
*/
/********************************************************************************F*/
static int32_t _PrivilegeApiConvertToNativePriv(PrivilegeApiPrivE iPrivilege, uint32_t *pNativePrivilege, int32_t *pAdditionalChecksRequired)
{
    int32_t bIsNativePriv = TRUE;
    uint32_t iNativePrivilege;
    switch (iPrivilege)
    {
        case PRIVILEGEAPI_PRIV_COMMUNICATION_VOICE_INGAME: iNativePrivilege = (uint32_t)Windows::Xbox::ApplicationModel::Store::KnownPrivileges::XPRIVILEGE_COMMUNICATION_VOICE_INGAME; break;
        case PRIVILEGEAPI_PRIV_COMMUNICATION_VOICE_SKYPE: iNativePrivilege = (uint32_t)Windows::Xbox::ApplicationModel::Store::KnownPrivileges::XPRIVILEGE_COMMUNICATION_VOICE_SKYPE; break;
        case PRIVILEGEAPI_PRIV_VIDEO_COMMUNICATIONS: iNativePrivilege = (uint32_t)Windows::Xbox::ApplicationModel::Store::KnownPrivileges::XPRIVILEGE_VIDEO_COMMUNICATIONS; break;
        case PRIVILEGEAPI_PRIV_COMMUNICATIONS: iNativePrivilege = (uint32_t)Windows::Xbox::ApplicationModel::Store::KnownPrivileges::XPRIVILEGE_COMMUNICATIONS; break;
        case PRIVILEGEAPI_PRIV_USER_CREATED_CONTENT: iNativePrivilege = (uint32_t)Windows::Xbox::ApplicationModel::Store::KnownPrivileges::XPRIVILEGE_USER_CREATED_CONTENT; break;
        case PRIVILEGEAPI_PRIV_MULTIPLAYER_SESSIONS_REALTIME: iNativePrivilege = (uint32_t)Windows::Xbox::ApplicationModel::Store::KnownPrivileges::XPRIVILEGE_MULTIPLAYER_SESSIONS; break;
        case PRIVILEGEAPI_PRIV_DOWNLOAD_FREE_CONTENT: iNativePrivilege = (uint32_t)193; break; //Windows::Xbox::ApplicationModel::Store::KnownPrivileges::XPRIVILEGE_DOWNLOAD_FREE_CONTENT; break;
        case PRIVILEGEAPI_PRIV_FITNESS_UPLOAD: iNativePrivilege = (uint32_t)195; break; //Windows::Xbox::ApplicationModel::Store::KnownPrivileges::XPRIVILEGE_FITNESS_UPLOAD; break;
        case PRIVILEGEAPI_PRIV_VIEW_FRIENDS_LIST: iNativePrivilege = (uint32_t)Windows::Xbox::ApplicationModel::Store::KnownPrivileges::XPRIVILEGE_VIEW_FRIENDS_LIST; break;
        case PRIVILEGEAPI_PRIV_SHARE_KINECT_CONTENT: iNativePrivilege = (uint32_t)Windows::Xbox::ApplicationModel::Store::KnownPrivileges::XPRIVILEGE_SHARE_KINECT_CONTENT; break;
        case PRIVILEGEAPI_PRIV_MULTIPLAYER_PARTIES: iNativePrivilege = (uint32_t)Windows::Xbox::ApplicationModel::Store::KnownPrivileges::XPRIVILEGE_MULTIPLAYER_PARTIES; break;
        case PRIVILEGEAPI_PRIV_CLOUD_GAMING_MANAGE_SESSION: iNativePrivilege = (uint32_t)Windows::Xbox::ApplicationModel::Store::KnownPrivileges::XPRIVILEGE_CLOUD_GAMING_MANAGE_SESSION; break;
        case PRIVILEGEAPI_PRIV_CLOUD_GAMING_JOIN_SESSION: iNativePrivilege = (uint32_t)Windows::Xbox::ApplicationModel::Store::KnownPrivileges::XPRIVILEGE_CLOUD_GAMING_JOIN_SESSION; break;
        case PRIVILEGEAPI_PRIV_CLOUD_SAVED_GAMES: iNativePrivilege = (uint32_t)Windows::Xbox::ApplicationModel::Store::KnownPrivileges::XPRIVILEGE_CLOUD_SAVED_GAMES; break;
        case PRIVILEGEAPI_PRIV_PREMIUM_CONTENT: iNativePrivilege = (uint32_t)Windows::Xbox::ApplicationModel::Store::KnownPrivileges::XPRIVILEGE_PREMIUM_CONTENT; break;
        case PRIVILEGEAPI_PRIV_INTERNET_BROWSER: iNativePrivilege = (uint32_t)217; break; //Windows::Xbox::ApplicationModel::Store::KnownPrivileges::XPRIVILEGE_INTERNET_BROWSER; break;
        case PRIVILEGEAPI_PRIV_SUBSCRIPTION_CONTENT: iNativePrivilege = (uint32_t)Windows::Xbox::ApplicationModel::Store::KnownPrivileges::XPRIVILEGE_SUBSCRIPTION_CONTENT; break;
        case PRIVILEGEAPI_PRIV_PREMIUM_VIDEO: iNativePrivilege = (uint32_t)Windows::Xbox::ApplicationModel::Store::KnownPrivileges::XPRIVILEGE_PREMIUM_VIDEO; break;
        case PRIVILEGEAPI_PRIV_GAME_DVR: iNativePrivilege = (uint32_t)Windows::Xbox::ApplicationModel::Store::KnownPrivileges::XPRIVILEGE_GAME_DVR; break;
        case PRIVILEGEAPI_PRIV_SOCIAL_NETWORK_SHARING: iNativePrivilege = (uint32_t)Windows::Xbox::ApplicationModel::Store::KnownPrivileges::XPRIVILEGE_SOCIAL_NETWORK_SHARING; break;
        case PRIVILEGEAPI_PRIV_PURCHASE_CONTENT: iNativePrivilege = (uint32_t)Windows::Xbox::ApplicationModel::Store::KnownPrivileges::XPRIVILEGE_PURCHASE_CONTENT; break;
        case PRIVILEGEAPI_PRIV_PROFILE_VIEWING: iNativePrivilege = (uint32_t)Windows::Xbox::ApplicationModel::Store::KnownPrivileges::XPRIVILEGE_PROFILE_VIEWING; break;
        case PRIVILEGEAPI_PRIV_PRESENCE:
        case PRIVILEGEAPI_PRIV_CONTENT_AUTHOR:
        case PRIVILEGEAPI_PRIV_UNSAFE_PROGRAMMING:
        case PRIVILEGEAPI_PRIV_MULTIPLAYER_SESSIONS_ASYNC:
        case PRIVILEGEAPI_PRIV_ONLINE_ACCESS:
        {
            bIsNativePriv = FALSE;
            iNativePrivilege = 0;
            break;
        }
        default:
        {
            iNativePrivilege = (uint32_t)iPrivilege;
            break;
        }
    }

    if (pNativePrivilege != NULL)
    {
        *pNativePrivilege = iNativePrivilege;
    }

    if (pAdditionalChecksRequired != NULL)
    {
        switch (iNativePrivilege)
        {
            case (uint32_t)Windows::Xbox::ApplicationModel::Store::KnownPrivileges::XPRIVILEGE_GAME_DVR:
            case (uint32_t)Windows::Xbox::ApplicationModel::Store::KnownPrivileges::XPRIVILEGE_SHARE_KINECT_CONTENT:
            case (uint32_t)Windows::Xbox::ApplicationModel::Store::KnownPrivileges::XPRIVILEGE_COMMUNICATION_VOICE_INGAME:
            case (uint32_t)Windows::Xbox::ApplicationModel::Store::KnownPrivileges::XPRIVILEGE_VIDEO_COMMUNICATIONS:
            case (uint32_t)Windows::Xbox::ApplicationModel::Store::KnownPrivileges::XPRIVILEGE_PROFILE_VIEWING:
            case (uint32_t)Windows::Xbox::ApplicationModel::Store::KnownPrivileges::XPRIVILEGE_COMMUNICATIONS:
            {
                *pAdditionalChecksRequired = TRUE;
                break;
            }
            default:
            {
                *pAdditionalChecksRequired = FALSE;
                break;
            }
        }
    }

    return bIsNativePriv;
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
    for (int32_t iSlotIndex = 0; iSlotIndex < PRIVILEGEAPI_MAX_ASYNC_REQUESTS; iSlotIndex++)
    {
        if (pRef->aAsyncRequests[iSlotIndex].iStatus != PRIVILEGEAPI_STATUS_NONE)
        {
            PrivilegeApiAbort(pRef, iSlotIndex + 1);
        }
    }

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

    \Input  *pRef               - the module reference
    \Input   iUserIndex         - user index
    \Input  *pPrivileges        - array of privileges
    \Input   iPrivilegeCount    - the number of privileges pointed to by pPrivileges
    \Input  *pHint              - (optional, out) if provided, pHint will be filled with a non-authoritative result for the privilege check.  Unless the return value is zero (0), in which case, this value can be considered authoritative.
    \Input  *pUiMessage         - this parameter should be non-NULL for the UI functionality to be enabled, otherwise, no UI is displayed.  However, the value of the string is not used on PS4.
    
    \Output int32_t             - <0 = error, 0 = success, use *pHint for the result, >0 = the request id to pass into PrivilegeApiCheckResult()

    \Version 09/02/2013 (mcorcoran)
*/
/********************************************************************************F*/
int32_t PrivilegeApiCheckPrivilegesAsyncWithUi(PrivilegeApiRefT *pRef, int32_t iUserIndex, const PrivilegeApiPrivE *pPrivileges, int32_t iPrivilegeCount, int32_t *pHint, const char *pUiMessage)
{
    int32_t iSlotIndex;
    User ^refXboxUser = nullptr;
    if (NetConnStatus('xusr', iUserIndex, &refXboxUser, sizeof(refXboxUser)) < 0)
    {
        NetPrintf(("privilegeapixboxone: invalid user index (%i)\n", iUserIndex));
        return(-1);
    }

    iSlotIndex = _PrivilegeApiGetAvailableSlot(pRef);
    if (iSlotIndex < 0)
    {
        NetPrintf(("privilegeapixboxone: too many async requests pending\n"));
        return(-2);
    }

    if (pHint != NULL)
    {
        *pHint = PRIVILEGEAPI_STATUS_GRANTED;
    }

    Platform::Collections::Vector<uint32_t> ^refPrivileges = ref new Platform::Collections::Vector<uint32_t>();

    for (int32_t iIndex = 0; iIndex < iPrivilegeCount; iIndex++)
    {
        uint32_t iNativePriv;
        int32_t bIsNativePriv = _PrivilegeApiConvertToNativePriv(pPrivileges[iIndex], &iNativePriv, (pRef->aAsyncRequests[iSlotIndex].bAdditionalChecks ? NULL : &pRef->aAsyncRequests[iSlotIndex].bAdditionalChecks));
        if (pPrivileges[iIndex] == PRIVILEGEAPI_PRIV_INVALID)
        {
            // no op
        }
        else if (bIsNativePriv)
        {
            refPrivileges->Append(iNativePriv);

            if ((pHint != NULL) && (*pHint == PRIVILEGEAPI_STATUS_GRANTED))
            {
                uint32_t iNativeIndex = 0;

                try
                {
                    if (!refXboxUser->DisplayInfo->Privileges->IndexOf(iNativePriv, &iNativeIndex))
                    {
                        *pHint = PRIVILEGEAPI_STATUS_RESTRICTED;
                    }
                }
                catch (::Platform::COMException^ e)
                {
                    NetPrintf(("privilegeapixboxone: exception thrown while accessing user's privileges (%S/0x%08x)\n", e->Message->Data(), e->HResult));
                    return(-3);
                }
            }
        }
        // else, the passed in privilege id might be a custom privilege.
        // custom privileges always have >0 id values.
        else if (pPrivileges[iIndex] > 0)
        {
            refPrivileges->Append((uint32_t)pPrivileges[iIndex]);
        }
    }

    if ((pHint != NULL) && pRef->aAsyncRequests[iSlotIndex].bAdditionalChecks)
    {
        *pHint |= PRIVILEGEAPI_STATUS_ADDITIONAL_CHECKS_REQUIRED;
    }

    if (refPrivileges->Size != 0)
    {
        try
        {
            if (pUiMessage != NULL)
            {
                if (*pUiMessage)
                {
                    int32_t iBufSize = MultiByteToWideChar(CP_UTF8, 0, pUiMessage, -1, NULL, 0) * sizeof(wchar_t);
                    if (iBufSize > 0)
                    {
                        wchar_t *wstrUiMessage;
                        if ((wstrUiMessage = (wchar_t*)DirtyMemAlloc(iBufSize, PRIVILEGEAPI_MEMID, pRef->iMemGroup, pRef->pMemGroupUserData)) == NULL)
                        {
                            PrivilegeApiAbort(pRef, iSlotIndex);
                            NetPrintf(("privilegeapixboxone: failed to allocate memory for wide-character ui message string.\n"));
                            return(-4);
                        }

                        MultiByteToWideChar(CP_UTF8, 0, pUiMessage, -1, wstrUiMessage, iBufSize/sizeof(wstrUiMessage[0]));
                        Platform::String ^refUiMessage = ref new Platform::String(wstrUiMessage);

                        pRef->aAsyncRequests[iSlotIndex].refAsyncOp = Product::CheckPrivilegesAsync(refXboxUser, refPrivileges->GetView(), true, refUiMessage);

                        DirtyMemFree(wstrUiMessage, PRIVILEGEAPI_MEMID, pRef->iMemGroup, pRef->pMemGroupUserData);
                    }
                }
                else
                {
                    pRef->aAsyncRequests[iSlotIndex].refAsyncOp = Product::CheckPrivilegesAsync(refXboxUser, refPrivileges->GetView(), true, "");
                }
            }
            else
            {
                pRef->aAsyncRequests[iSlotIndex].refAsyncOp = Product::CheckPrivilegesAsync(refXboxUser, refPrivileges->GetView(), false, "");
            }
        }
        catch (::Platform::COMException^ e) // CheckPrivilegeAsync can throw ERROR_NOT_SUPPORTED (0x80070032) if the user is not logged in or is only partially logged in somehow
        {
            NetPrintf(("privilegeapixboxone: exception thrown while attempting to check privileges (%S/0x%08x)\n", e->Message->Data(), e->HResult));
            pRef->aAsyncRequests[iSlotIndex].refAsyncOp = nullptr;
            PrivilegeApiAbort(pRef, iSlotIndex);
            return(-5);
        }
    }
    else if (pHint != NULL)
    {
        PrivilegeApiAbort(pRef, iSlotIndex);
        return(0);
    }
    else
    {
        pRef->aAsyncRequests[iSlotIndex].iStatus = PRIVILEGEAPI_STATUS_GRANTED;
        if (pRef->aAsyncRequests[iSlotIndex].bAdditionalChecks)
        {
            pRef->aAsyncRequests[iSlotIndex].iStatus |= PRIVILEGEAPI_STATUS_ADDITIONAL_CHECKS_REQUIRED;
        }
    }

    return(iSlotIndex + 1);
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
    int32_t iSlotIndex = iRequestId - 1;

    if ((iRequestId <= 0) || (iRequestId > PRIVILEGEAPI_MAX_ASYNC_REQUESTS) || (pRef->aAsyncRequests[iSlotIndex].iStatus == PRIVILEGEAPI_STATUS_NONE))
    {
        NetPrintf(("privilegeapixboxone: invalid request id (%d)\n", iRequestId));
        return(-1);
    }

    if ((pRef->aAsyncRequests[iSlotIndex].refAsyncOp != nullptr) &&
        (pRef->aAsyncRequests[iSlotIndex].refAsyncOp->Status != Windows::Foundation::AsyncStatus::Started))
    {
        switch (pRef->aAsyncRequests[iSlotIndex].refAsyncOp->Status)
        {
            case Windows::Foundation::AsyncStatus::Completed:
            {
                Windows::Xbox::ApplicationModel::Store::PrivilegeCheckResult result;
                try
                {
                    result = pRef->aAsyncRequests[iSlotIndex].refAsyncOp->GetResults();
                }
                catch (Platform::Exception ^e)
                {
                    pRef->aAsyncRequests[iSlotIndex].iStatus = PRIVILEGEAPI_STATUS_ERROR;
                    NetPrintf(("privilegeapixboxone: PrivilegeApiCheckPrivilegesAsync() Exception thrown (%S/0x%08x)\n", e->Message->Data(), e->HResult));
                    break;
                }

                if (result == Windows::Xbox::ApplicationModel::Store::PrivilegeCheckResult::NoIssue)
                {
                    pRef->aAsyncRequests[iSlotIndex].iStatus = PRIVILEGEAPI_STATUS_GRANTED;
                }
                else
                {
                    pRef->aAsyncRequests[iSlotIndex].iStatus = 0;
                    if ((int32_t)result & (int32_t)Windows::Xbox::ApplicationModel::Store::PrivilegeCheckResult::Restricted)
                    {
                        pRef->aAsyncRequests[iSlotIndex].iStatus |= PRIVILEGEAPI_STATUS_RESTRICTED;
                    }
                    if ((int32_t)result & (int32_t)Windows::Xbox::ApplicationModel::Store::PrivilegeCheckResult::Aborted)
                    {
                        pRef->aAsyncRequests[iSlotIndex].iStatus |= PRIVILEGEAPI_STATUS_ABORTED;
                    }
                    if ((int32_t)result & (int32_t)Windows::Xbox::ApplicationModel::Store::PrivilegeCheckResult::Banned)
                    {
                        pRef->aAsyncRequests[iSlotIndex].iStatus |= PRIVILEGEAPI_STATUS_BANNED;
                    }
                    if ((int32_t)result & (int32_t)Windows::Xbox::ApplicationModel::Store::PrivilegeCheckResult::PurchaseRequired)
                    {
                        pRef->aAsyncRequests[iSlotIndex].iStatus |= PRIVILEGEAPI_STATUS_PURCHASE_REQUIRED;
                    }
                }

                if (pRef->aAsyncRequests[iSlotIndex].bAdditionalChecks)
                {
                    pRef->aAsyncRequests[iSlotIndex].iStatus |= PRIVILEGEAPI_STATUS_ADDITIONAL_CHECKS_REQUIRED;
                }
                break;
            }
            case Windows::Foundation::AsyncStatus::Error:
            {
                pRef->aAsyncRequests[iSlotIndex].iStatus = PRIVILEGEAPI_STATUS_ERROR;
                NetPrintf(("privilegeapixboxone: PrivilegeApiCheckPrivilegesAsync() An error occured in call to CheckPrivilegesAsync(), ErrorCode=%S (0x%08x)\n",
                    pRef->aAsyncRequests[iSlotIndex].refAsyncOp->ErrorCode.ToString()->Data(), pRef->aAsyncRequests[iSlotIndex].refAsyncOp->ErrorCode));
                break;
            }
            case Windows::Foundation::AsyncStatus::Canceled:
            {
                pRef->aAsyncRequests[iSlotIndex].iStatus = PRIVILEGEAPI_STATUS_ABORTED;
                NetPrintf(("privilegeapixboxone: PrivilegeApiCheckPrivilegesAsync() The async operation was cancelled\n"));
                break;
            }
        }
    }

    return(pRef->aAsyncRequests[iSlotIndex].iStatus);
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
    int32_t iSlotIndex = iRequestId - 1;

    if ((iRequestId <= 0) || (iRequestId > PRIVILEGEAPI_MAX_ASYNC_REQUESTS) || (pRef->aAsyncRequests[iSlotIndex].iStatus == PRIVILEGEAPI_STATUS_NONE))
    {
        NetPrintf(("privilegeapixboxone: invalid request id (%i)\n", iRequestId));
        return(-1);
    }

    if (pRef->aAsyncRequests[iSlotIndex].iStatus == PRIVILEGEAPI_STATUS_IN_PROGRESS)
    {
        NetPrintf(("privilegeapixboxone: the request id (%i) is currently in progress, use PrivilegeApiAbort()\n", iRequestId));
        return(-1);
    }

    if (pRef->aAsyncRequests[iSlotIndex].refAsyncOp != nullptr)
    {
        pRef->aAsyncRequests[iSlotIndex].refAsyncOp->Close();
        pRef->aAsyncRequests[iSlotIndex].refAsyncOp = nullptr;
    }
    pRef->aAsyncRequests[iSlotIndex].bAdditionalChecks = FALSE;
    pRef->aAsyncRequests[iSlotIndex].iStatus = PRIVILEGEAPI_STATUS_NONE;

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
    int32_t iSlotIndex = iRequestId - 1;

    if ((iRequestId <= 0) || (iRequestId > PRIVILEGEAPI_MAX_ASYNC_REQUESTS) || (pRef->aAsyncRequests[iSlotIndex].iStatus == PRIVILEGEAPI_STATUS_NONE))
    {
        NetPrintf(("privilegeapixboxone: invalid request id (%i)\n", iRequestId));
        return(-1);
    }

    if (pRef->aAsyncRequests[iSlotIndex].refAsyncOp != nullptr)
    {
        pRef->aAsyncRequests[iSlotIndex].refAsyncOp->Cancel();
    }
    PrivilegeApiCheckResult(pRef, iRequestId);
    PrivilegeApiReleaseRequest(pRef, iRequestId);

    return(0);
}
