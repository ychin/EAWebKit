/*H*************************************************************************************/
/*!
    \File userlistapistadia.c

    \Description
        See userlistapi.h

    \Copyright
        Copyright (c) Electronic Arts 2020

    \Version 01/10/2020 (eesponda)
*/
/*************************************************************************************H*/

/*** Include files *********************************************************************/

#include "DirtySDK/platform.h"

EA_DISABLE_CLANG_WARNING(-Wstrict-prototypes)
#include <ggp_c/status.h>
#include <ggp_c/presence.h>
#include <ggp_c/social.h>
#include <ggp_c/version.h>
EA_RESTORE_CLANG_WARNING()

#include <stdio.h>
#include <stdlib.h>

#include "DirtySDK/misc/userlistapi.h"

#include "DirtySDK/dirtysock.h"
#include "DirtySDK/dirtysock/dirtyerr.h"
#include "DirtySDK/dirtysock/dirtylib.h"
#include "DirtySDK/dirtysock/dirtymem.h"
#include "DirtySDK/dirtysock/netconn.h"

/*** Defines ***************************************************************************/

/*** Macros ****************************************************************************/

//! macro that wraps the call to DirtyErrGetNameList for the callback status
#define USERLISTAPI_GetGgpStatus(iStatus) DirtyErrGetNameList((iStatus), _UserListApi_aGgpStatus)

/*** Type Definitions ******************************************************************/

//! structure to store in the queue of requests
typedef struct UserListApiRequestT
{
    GgpFuture AsyncBlock;               //!< main async block used for asynchronous handling
    GgpFuture PresenceBlock;            //!< presence async block if required
    uint32_t uUserIndex;                //!< local index of the user requesting
    UserListApiTypeE eType;             //!< type of request
    int32_t iLimit;                     //!< request limit (maximum results)
    int32_t iOffset;                    //!< offsets to start
    UserListApiCallbackT *pNotifyCb;    //!< callback function pointer
    void *pUserData;                    //!< user data for the callback
    uint32_t uTypeMask;                 //!< mask for the request
    GgpProfile *pProfiles;              //!< profiles for the social group
    int32_t iNumProfiles;               //!< number of profiles
    GgpPresence *pPresenceRecords;      //!< presence data for the social group (if enabled)
    int32_t iNumPresenceRecords;        //!< number of presence records returned
} UserListApiRequestT;

//! module state
struct UserListApiRefT
{
    int32_t iMemGroup;                                      //!< module mem group id
    void *pMemGroupUserData;                                //!< user data associated with mem group
    UserListApiRequestT aRequests[NETCONN_MAXLOCALUSERS];   //!< request data for each user
};

/*** Function Prototypes ***************************************************************/

/*** Variables *************************************************************************/

#if DIRTYCODE_LOGGING
//! the different statuses returned by the ggp api functions
static const DirtyErrT _UserListApi_aGgpStatus[] =
{
    DIRTYSOCK_ErrorName(kGgpStatusCode_Ok),                 // 0 Operation successful.
    DIRTYSOCK_ErrorName(kGgpStatusCode_Cancelled),          // 1 Operation was cancelled due to anouther call from the game.
    DIRTYSOCK_ErrorName(kGgpStatusCode_InvalidArgument),    // 3 Argument to the function was invalid.
    DIRTYSOCK_ErrorName(kGgpStatusCode_DeadlineExceeded),   // 4 Operation timed out before completing.
    DIRTYSOCK_ErrorName(kGgpStatusCode_NotFound),           // 5 Requested resource doesn't exist.
    DIRTYSOCK_ErrorName(kGgpStatusCode_AlreadyExists),      // 6 Requested resource already exists.
    DIRTYSOCK_ErrorName(kGgpStatusCode_PermissionDenied),   // 7 Game does not have rights to do operation.
    DIRTYSOCK_ErrorName(kGgpStatusCode_ResourceExhausted),  // 8 No more of the resource could be allocated.
    DIRTYSOCK_ErrorName(kGgpStatusCode_FailedPrecondition), // 9 Operation preconditions were not met.
    DIRTYSOCK_ErrorName(kGgpStatusCode_Aborted),            // 10 Operation was aborted, but could be retried.
    DIRTYSOCK_ErrorName(kGgpStatusCode_Unimplemented),      // 12 Feature not yet implemented.
    DIRTYSOCK_ErrorName(kGgpStatusCode_Internal),           // 13 An internal error has occured.
    DIRTYSOCK_ErrorName(kGgpStatusCode_Unavailable),        // 14 Service or resource is not currently available, but may be later.
    DIRTYSOCK_ListEnd()
};
#endif

/*** Private functions *****************************************************************/

/*F*************************************************************************************/
/*!
    \Function _UserListApiHandleListError

    \Description
        Fires the end callback with the error information

    \Input *pRef        - module ref
    \Input *pRequest    - user index specific request we are handling
    \Input iError       - error the occurred

    \Version  03/05/2020 (eesponda)
*/
/*************************************************************************************F*/
static void _UserListApiHandleListError(UserListApiRefT *pRef, UserListApiRequestT *pRequest, int32_t iError)
{
    UserListApiEventDataT EventData;
    ds_memclr(&EventData, sizeof(EventData));

    // fill in the information
    EventData.ListEndData.eRequestType = pRequest->eType;
    EventData.ListEndData.pUserListFilters = NULL;
    EventData.ListEndData.uUserIndex = pRequest->uUserIndex;
    EventData.ListEndData.iLimit = pRequest->iLimit;
    EventData.ListEndData.iOffset = pRequest->iOffset;
    EventData.ListEndData.iTotalFriendCount = -1;
    EventData.ListEndData.Error = iError;

    // fire the callback
    pRequest->pNotifyCb(pRef, TYPE_LIST_END, &EventData, pRequest->pUserData);
}

/*F*************************************************************************************/
/*!
    \Function   _UserListApiHandleProfileResponse

    \Description
        Handles the presence response by pulling it out of the ggp future

    \Input *pRef        - module state
    \Input *pRequest    - request data

    \Version 03/05/2020 (eesponda)
*/
/*************************************************************************************F*/
static int32_t _UserListApiHandleProfileResponse(UserListApiRefT *pRef, UserListApiRequestT *pRequest)
{
    GgpStatus Status;
    char strError[256];
    int32_t iProfileSize;

    // get the number of results
    pRequest->iNumProfiles = GgpFutureGetResultCount(pRequest->AsyncBlock, &Status);
    if (Status.status_code != kGgpStatusCode_Ok)
    {
        GgpGetStatusMessage(Status.message_token, strError, sizeof(strError));
        NetPrintf(("userlistapistadia: [%p] failed to get the friends list result count (err=%s/%s)\n", pRef, USERLISTAPI_GetGgpStatus(Status.status_code), strError));
        _UserListApiHandleListError(pRef, pRequest, Status.status_code);
        return(-1);
    }
    iProfileSize = sizeof(*pRequest->pProfiles) * pRequest->iNumProfiles;
    // allocate a buffer for the result
    if ((pRequest->pProfiles = (GgpProfile *)DirtyMemAlloc(iProfileSize, USERLISTAPI_MEMID, pRef->iMemGroup, pRef->pMemGroupUserData)) == NULL)
    {
        NetPrintf(("userlistapistadia: [%p] failed to allocate memory for the result\n", pRef));
        _UserListApiHandleListError(pRef, pRequest, -1);
        return(-1);
    }
    // get the results
    if (!GgpFutureGetMultipleResult(pRequest->AsyncBlock, pRequest->pProfiles, iProfileSize, pRequest->iNumProfiles, NULL, &Status))
    {
        GgpGetStatusMessage(Status.message_token, strError, sizeof(strError));
        NetPrintf(("userlistapistadia: [%p] failed to get the friends list result (err=%s/%s)\n", pRef, USERLISTAPI_GetGgpStatus(Status.status_code), strError));
        _UserListApiHandleListError(pRef, pRequest, Status.status_code);
        return(-1);
    }

    return(0);
}

/*F*************************************************************************************/
/*!
    \Function   _UserListApiHandlePresenceResponse

    \Description
        Handles the presence response by pulling it out of the ggp future

    \Input *pRef        - module state
    \Input *pRequest    - request data

    \Version 03/05/2020 (eesponda)
*/
/*************************************************************************************F*/
static int32_t _UserListApiHandlePresenceResponse(UserListApiRefT *pRef, UserListApiRequestT *pRequest)
{
    GgpStatus Status;
    char strError[256];
    int32_t iPresenceSize;

    // get the number of records returned
    pRequest->iNumPresenceRecords = GgpFutureGetResultCount(pRequest->PresenceBlock, &Status);
    if (Status.status_code != kGgpStatusCode_Ok)
    {
        GgpGetStatusMessage(Status.message_token, strError, sizeof(strError));
        NetPrintf(("userlistapistadia: [%p] failed to get the friends list presence result count (err=%s/%s)\n", pRef, USERLISTAPI_GetGgpStatus(Status.status_code), strError));
        _UserListApiHandleListError(pRef, pRequest, Status.status_code);
        return(-1);
    }
    iPresenceSize = sizeof(*pRequest->pPresenceRecords) * pRequest->iNumPresenceRecords;
    // allocate a buffer for the result
    if ((pRequest->pPresenceRecords = (GgpPresence *)DirtyMemAlloc(iPresenceSize, USERLISTAPI_MEMID, pRef->iMemGroup, pRef->pMemGroupUserData)) == NULL)
    {
        NetPrintf(("userlistapistadia: [%p] failed to allocate memory for the result\n", pRef));
        _UserListApiHandleListError(pRef, pRequest, -1);
        return(-1);
    }
    // get the results
    if (!GgpFutureGetMultipleResult(pRequest->PresenceBlock, pRequest->pPresenceRecords, iPresenceSize, pRequest->iNumPresenceRecords, NULL, &Status))
    {
        GgpGetStatusMessage(Status.message_token, strError, sizeof(strError));
        NetPrintf(("userlistapistadia: [%p] failed to get the friends list presence result (err=%s/%s)\n", pRef, USERLISTAPI_GetGgpStatus(Status.status_code), strError));
        _UserListApiHandleListError(pRef, pRequest, Status.status_code);
        return(-1);
    }
    return(0);
}

/*F*************************************************************************************/
/*!
    \Function   _UserListApiFilloutPresence

    \Description
        Fills out the presence data part of the callback events

    \Input *pRef        - module state
    \Input *pRequest    - request data
    \Input *pEventData  - the callback data we are filling out
    \Input uPlayerId    - the id of the user in the list

    \Output
        int32_t         - 0=success, negative=failure

    \Version 03/05/2020 (eesponda)
*/
/*************************************************************************************F*/
static int32_t _UserListApiFilloutPresence(UserListApiRefT *pRef, UserListApiRequestT *pRequest, UserApiUserDataT *pEventData, GgpPlayerId uPlayerId)
{
    const GgpPresence *pPresence = NULL;
    int32_t iRecord;

    // search for the record we are interested in
    for (iRecord = 0; iRecord < pRequest->iNumPresenceRecords; iRecord += 1)
    {
        if (pRequest->pPresenceRecords[iRecord].player_id != uPlayerId)
        {
            continue;
        }
        pPresence = &pRequest->pPresenceRecords[iRecord];
        break;
    }

    // if we did not find any records then return an error
    if (pPresence == NULL)
    {
        return(-1);
    }

    // set the flags for the callback
    pEventData->uUserDataMask |= (USERAPI_MASK_PRESENCE);

    // get the presence state of the user
    switch (pPresence->availability)
    {
        case kGgpPresenceAvailability_LookingForParty:
        case kGgpPresenceAvailability_Online:
        {
            pEventData->Presence.ePresenceStatus = USERAPI_PRESENCE_ONLINE;
            break;
        }
        case kGgpPresenceAvailability_Busy:
        case kGgpPresenceAvailability_Away:
        {
            pEventData->Presence.ePresenceStatus = USERAPI_PRESENCE_AWAY;
            break;
        }
        case kGgpPresenceAvailability_Offline:
        {
            pEventData->Presence.ePresenceStatus = USERAPI_PRESENCE_OFFLINE;
            break;
        }
        default:
        {
            pEventData->Presence.ePresenceStatus = USERAPI_PRESENCE_UNKNOWN;
            break;
        }
    }

    pEventData->Presence.pRawData = (void *)pPresence;

    return(0);
}

/*F*************************************************************************************/
/*!
    \Function _UserListApiHandleFriendsListResponse

    \Description
        Handles the friend's list response

    \Input *pRef        - module ref
    \Input *pRequest    - user index specific request we are handling

    \Output
        int32_t         - 0=success and can clear request data, 1=pending

    \Version  03/05/2020 (eesponda)
*/
/*************************************************************************************F*/
static int32_t _UserListApiHandleFriendsListResponse(UserListApiRefT *pRef, UserListApiRequestT *pRequest)
{
    int32_t iProfile;
    UserListApiEventDataT EventData;

    // if we haven't pulled the profile result yet do that now
    if (pRequest->pProfiles == NULL)
    {
        // process the profile response
        if (_UserListApiHandleProfileResponse(pRef, pRequest) != 0)
        {
            return(0);
        }
        // if presence was requested and we have profiles to request for, send the request for that now
        if ((pRequest->uTypeMask & USERLISTAPI_MASK_PRESENCE) && (pRequest->iNumProfiles > 0))
        {
            GgpPlayerId *pPlayerIds;

            // allocate a request buffer
            if ((pPlayerIds = (GgpPlayerId *)DirtyMemAlloc(sizeof(*pPlayerIds) * pRequest->iNumProfiles, USERLISTAPI_MEMID, pRef->iMemGroup, pRef->pMemGroupUserData)) == NULL)
            {
                NetPrintf(("userlistapistadia: [%p] failed to allocate memory for the presence request\n", pRef));
                _UserListApiHandleListError(pRef, pRequest, -1);
                return(0);
            }
            // add the profiles we are requesting
            for (iProfile = 0; iProfile < pRequest->iNumProfiles; iProfile += 1)
            {
                pPlayerIds[iProfile] = pRequest->pProfiles[iProfile].player_id;
            }
            pRequest->PresenceBlock = GgpBatchGetPresence(pPlayerIds, pRequest->iNumProfiles);
            DirtyMemFree(pPlayerIds, USERLISTAPI_MEMID, pRef->iMemGroup, pRef->pMemGroupUserData);
            return(1);
        }
    }
    // process the presence request, if we made one based on profiles
    if ((pRequest->uTypeMask & USERLISTAPI_MASK_PRESENCE) && (pRequest->iNumProfiles > 0))
    {
        // check on the future
        if (!GgpFutureIsReady(pRequest->PresenceBlock))
        {
            return(1);
        }
        // process the presence response
        if (_UserListApiHandlePresenceResponse(pRef, pRequest) != 0)
        {
            return(0);
        }
    }

    // loop through the friends list to fire the callback
    for (iProfile = 0; iProfile < pRequest->iNumProfiles; iProfile += 1)
    {
        const GgpProfile *pProfile = &pRequest->pProfiles[iProfile];

        // fill in the information
        ds_memclr(&EventData, sizeof(EventData));
        DirtyUserFromNativeUser(&EventData.UserData.DirtyUser, &pProfile->player_id);
        DirtyUserFromNativeUser(&EventData.UserData.ExtendedUserData.DirtyUser, &pProfile->player_id);
        EventData.UserData.bIsMutualFriend = TRUE;
        EventData.UserData.bIsFavorite = FALSE;
        EventData.UserData.eRequestType = pRequest->eType;
        EventData.UserData.uUserIndex = pRequest->uUserIndex;
        EventData.UserData.pUserListFilters = NULL;
        EventData.UserData.pRawData = (void *)pProfile;

        // fill out the profile extended data
        EventData.UserData.ExtendedUserData.uUserDataMask |= USERAPI_MASK_PROFILE;
        #if GGP_VERSION >= GGP_MAKE_VERSION(1, 50, 0)
        ds_strnzcpy(EventData.UserData.ExtendedUserData.Profile.strGamertag, pProfile->stadia_name, sizeof(EventData.UserData.ExtendedUserData.Profile.strGamertag));
        #else
        ds_strnzcpy(EventData.UserData.ExtendedUserData.Profile.strGamertag, pProfile->gamer_tag, sizeof(EventData.UserData.ExtendedUserData.Profile.strGamertag));
        #endif
        EventData.UserData.ExtendedUserData.Profile.pRawData = (void *)pProfile;

        // fill out the presence extended data if enabled
        if (pRequest->uTypeMask & USERLISTAPI_MASK_PRESENCE)
        {
            EventData.UserData.iUserProfileError |= _UserListApiFilloutPresence(pRef, pRequest, &EventData.UserData.ExtendedUserData, pProfile->player_id);
        }

        // fire the callback
        pRequest->pNotifyCb(pRef, TYPE_USER_DATA, &EventData, pRequest->pUserData);
    }

    // send the end of list now that we are done
    ds_memclr(&EventData, sizeof(EventData));
    EventData.ListEndData.eRequestType = pRequest->eType;
    EventData.ListEndData.pUserListFilters = NULL;
    EventData.ListEndData.uUserIndex = pRequest->uUserIndex;
    EventData.ListEndData.iLimit = pRequest->iLimit;
    EventData.ListEndData.iOffset = pRequest->iOffset;
    EventData.ListEndData.iTotalFriendCount = pRequest->iNumProfiles;
    EventData.ListEndData.uTypeMask = pRequest->uTypeMask;

    // fire the callback
    pRequest->pNotifyCb(pRef, TYPE_LIST_END, &EventData, pRequest->pUserData);
    return(0);
}

/*F*************************************************************************************/
/*!
    \Function _UserListApiHandleBlockListResponse

    \Description
        Handles the block list response

    \Input *pRef        - module ref
    \Input *pRequest    - user index specific request we are handling

    \Output
        int32_t         - 0=success and can clear request data, 1=pending

    \Version 03/05/2020 (eesponda)
*/
/*************************************************************************************F*/
static int32_t _UserListApiHandleBlockListResponse(UserListApiRefT *pRef, UserListApiRequestT *pRequest)
{
    UserListApiEventDataT EventData;
    GgpStatus Status;
    int32_t iResultCount, iIndex, iBlockedSize;
    GgpProfile *pBlocked;
    char strError[256];

    // get the number of results
    iResultCount = GgpFutureGetResultCount(pRequest->AsyncBlock, &Status);
    if (Status.status_code != kGgpStatusCode_Ok)
    {
        GgpGetStatusMessage(Status.message_token, strError, sizeof(strError));
        NetPrintf(("userlistapistadia: [%p] failed to get the blocked list count (err=%s/%s)\n", pRef, USERLISTAPI_GetGgpStatus(Status.status_code), strError));
        _UserListApiHandleListError(pRef, pRequest, Status.status_code);
        return(0);
    }
    iBlockedSize = sizeof(*pBlocked) * iResultCount;
    // allocate a buffer for the result
    if ((pBlocked = (GgpProfile *)DirtyMemAlloc(iBlockedSize, USERLISTAPI_MEMID, pRef->iMemGroup, pRef->pMemGroupUserData)) == NULL)
    {
        NetPrintf(("userlistapistadia: [%p] failed to allocate memory for the result\n", pRef));
        _UserListApiHandleListError(pRef, pRequest, -1);
        return(0);
    }
    // get the results
    if (!GgpFutureGetMultipleResult(pRequest->AsyncBlock, pBlocked, iBlockedSize, iResultCount, NULL, &Status))
    {
        GgpGetStatusMessage(Status.message_token, strError, sizeof(strError));
        NetPrintf(("userlistapistadia: [%p] failed to get the blocked list result (err=%s/%s)\n", pRef, USERLISTAPI_GetGgpStatus(Status.status_code), strError));
        _UserListApiHandleListError(pRef, pRequest, Status.status_code);
        return(0);
    }

    // loop through the response
    for (iIndex = 0; iIndex < iResultCount; iIndex += 1)
    {
        // fill only the basic request and user information and fire the callback
        ds_memclr(&EventData, sizeof(EventData));
        EventData.UserData.eRequestType = pRequest->eType;
        EventData.UserData.uUserIndex = pRequest->uUserIndex;

        DirtyUserFromNativeUser(&EventData.UserData.DirtyUser, &pBlocked[iIndex].player_id);
        EventData.UserData.pRawData = (void *)&pBlocked[iIndex];
        pRequest->pNotifyCb(pRef, TYPE_USER_DATA, &EventData, pRequest->pUserData);
    }

    // fill in the information
    ds_memclr(&EventData, sizeof(EventData));
    EventData.ListEndData.eRequestType = pRequest->eType;
    EventData.ListEndData.pUserListFilters = NULL;
    EventData.ListEndData.uUserIndex = pRequest->uUserIndex;
    EventData.ListEndData.iLimit = pRequest->iLimit;
    EventData.ListEndData.iOffset = pRequest->iOffset;
    EventData.ListEndData.iTotalFriendCount = iResultCount;

    // fire the callback
    pRequest->pNotifyCb(pRef, TYPE_LIST_END, &EventData, pRequest->pUserData);

    DirtyMemFree(pBlocked, USERLISTAPI_MEMID, pRef->iMemGroup, pRef->pMemGroupUserData);
    return(0);
}

/*F*************************************************************************************/
/*!
    \Function   _UserListApiCleanupRequestData

    \Description
        Cleans up the necessary resources that are part of the request data

    \Version 03/05/2020 (eesponda)
*/
/*************************************************************************************F*/
static void _UserListApiCleanupRequestData(UserListApiRefT *pRef, UserListApiRequestT *pRequest)
{
    if (pRequest->pProfiles != NULL)
    {
        DirtyMemFree(pRequest->pProfiles, USERLISTAPI_MEMID, pRef->iMemGroup, pRef->pMemGroupUserData);
    }
    if (pRequest->pPresenceRecords != NULL)
    {
        DirtyMemFree(pRequest->pPresenceRecords, USERLISTAPI_MEMID, pRef->iMemGroup, pRef->pMemGroupUserData);
    }
    // reset the request back to default
    ds_memclr(pRequest, sizeof(*pRequest));
}

/*** Public functions ******************************************************************/


/*F*************************************************************************************/
/*!
    \Function   UserListApiCreate

    \Description
        Startup UserListApi module.

    \Input uMaxResponseSize  - unused

    \Output
        UserListApiRefT *    - pointer to reference

    \Version 03/05/2020 (eesponda)
*/
/*************************************************************************************F*/
UserListApiRefT *UserListApiCreate(uint32_t uMaxResponseSize)
{
    UserListApiRefT *pRef;
    int32_t iMemGroup;
    void *pMemGroupUserData;

    // Query current mem group data
    DirtyMemGroupQuery(&iMemGroup, &pMemGroupUserData);

    // allocate and init module state
    if ((pRef = (UserListApiRefT *)DirtyMemAlloc(sizeof(*pRef), USERLISTAPI_MEMID, iMemGroup, pMemGroupUserData)) == NULL)
    {
        NetPrintf(("userlistapistadia: could not allocate module state\n"));
        return(NULL);
    }
    ds_memclr(pRef, sizeof(*pRef));
    pRef->iMemGroup = iMemGroup;
    pRef->pMemGroupUserData = pMemGroupUserData;
    return(pRef);
}

/*F*************************************************************************************/
/*!
    \Function   UserListApiDestroy

    \Description
        Shutdown UserListApi module.

    \Input *pRef - module ref

    \Version 03/05/2020 (eesponda)
*/
/*************************************************************************************F*/
void UserListApiDestroy(UserListApiRefT *pRef)
{
    UserListApiCancelAll(pRef);
    DirtyMemFree(pRef, USERLISTAPI_MEMID, pRef->iMemGroup, pRef->pMemGroupUserData);
}

/*F*************************************************************************************/
/*!
    \Function   UserListApiRegisterUpdateEvent

    \Description
        Set the callback used for registering userlist update event.

    \Input *pRef        - module ref
    \Input  uUserIndex  - user index
    \Input *eType       - type of callback requested
    \Input *pNotifyCb   - the callback pointer
    \Input *pUserData   - the user data pointer

    \Output
        int32_t         - 0 for success, negative for error

    \Version 03/05/2020 (eesponda)
*/
/*************************************************************************************F*/
int32_t UserListApiRegisterUpdateEvent(UserListApiRefT *pRef, uint32_t uUserIndex, UserListApiNotifyTypeE eType, UserListApiUpdateCallbackT *pNotifyCb, void *pUserData)
{
    return(USERLISTAPI_ERROR_UNSUPPORTED);
}

/*F*************************************************************************************/
/*!
    \Function   UserListApiIsFriendAsync

    \Description
        Checks if a specific user is on the friend list

    \Input *pRef            - module ref
    \Input  uUserIndex      - current user index
    \Input *pUser           - target user id (XUID for xboxone)
    \Input *pNotifyCb       - callback
    \Input *pUserData       - user data

    \Output
        int32_t             - 0 for success, negative for error

    \Version 03/05/2020 (eesponda)
*/
/*************************************************************************************F*/
int32_t UserListApiIsFriendAsync(UserListApiRefT *pRef, uint32_t uUserIndex, DirtyUserT *pUser, UserListApiIsAQueryCallbackT *pNotifyCb, void *pUserData)
{
    return(USERLISTAPI_ERROR_UNSUPPORTED);
}

/*F*************************************************************************************/
/*!
    \Function   UserListApiIsBlockedAsync

    \Description
        Checks if a specific user is on the block list

    \Input *pRef                - module ref
    \Input  uUserIndex          - current user index
    \Input *pUser               - target user id (XUID for xboxone)
    \Input *pNotifyCb           - callback
    \Input *pUserData           - user data

    \Output
        int32_t                 - 0 for success, negative for error

    \Version 03/05/2020 (eesponda)
*/
/*************************************************************************************F*/
int32_t UserListApiIsBlockedAsync(UserListApiRefT *pRef, uint32_t uUserIndex, DirtyUserT *pUser, UserListApiIsAQueryCallbackT *pNotifyCb, void *pUserData)
{
    return(USERLISTAPI_ERROR_UNSUPPORTED);
}

/*F*************************************************************************************/
/*!
    \Function   UserListApiUpdate

    \Description
        Updates UserListApi module.

    \Input *pRef    - module ref

    \Version  03/05/2020 (eesponda)
*/
/*************************************************************************************F*/
void UserListApiUpdate(UserListApiRefT *pRef)
{
    uint32_t uUserIndex;

    // loop through all the requests
    for (uUserIndex = 0; uUserIndex < NETCONN_MAXLOCALUSERS; uUserIndex += 1)
    {
        UserListApiRequestT *pRequest = &pRef->aRequests[uUserIndex];
        int32_t iResult = 0;

        // if the callback is null we are currently not handling a request
        if (pRequest->pNotifyCb == NULL)
        {
            continue;
        }
        // check the async status to see if there is anything for us to do
        if ((pRequest->pProfiles == NULL) && !GgpFutureIsReady(pRequest->AsyncBlock))
        {
            continue;
        }

        // handle the response
        if (pRequest->eType == USERLISTAPI_TYPE_FRIENDS)
        {
            iResult = _UserListApiHandleFriendsListResponse(pRef, pRequest);
        }
        else if (pRequest->eType == USERLISTAPI_TYPE_BLOCKED)
        {
            iResult = _UserListApiHandleBlockListResponse(pRef, pRequest);
        }

        // clean up the request if done
        if (iResult == 0)
        {
            _UserListApiCleanupRequestData(pRef, pRequest);
        }
    }
}

/*F*************************************************************************************/
/*!
    \Function   UserListApiStatus

    \Description
        Query status of module.

    \Input *pRef    - module ref
    \Input iSelect  - status selector
    \Input *pBuf    - [in/out] control value
    \Input iBufSize - size of pBuf

    \Output
        int32_t     - zero=success; negative=error

    \Version 03/05/2020 (eesponda)
*/
/*************************************************************************************F*/
int32_t UserListApiStatus(UserListApiRefT *pRef, int32_t iSelect, void *pBuf, int32_t iBufSize)
{
    NetPrintf(("userlistapistadia: [%p] unhandled status selector '%C'\n", pRef, iSelect));
    return(-1);
}

/*F*************************************************************************************/
/*!
    \Function   UserListApiControl

    \Description
        Control behavior of module.

    \Input *pRef    - module ref
    \Input iControl - status selector
    \Input iValue   - control value
    \Input iValue2  - control value
    \Input *pValue  - [in/out] control value

    \Output
        int32_t     - zero=success; negative=error

    \Version 03/05/2020 (eesponda)
*/
/*************************************************************************************F*/
int32_t UserListApiControl(UserListApiRefT *pRef, int32_t iControl, int32_t iValue, int32_t iValue2, void *pValue)
{
    NetPrintf(("userlistapistaida: [%p] unhandled control selector '%C'\n", pRef, iControl));
    return(-1);
}

/*F*************************************************************************************/
/*!
    \Function   UserListApiGetListAsync

    \Description
        Retrieve a list of friends or blocked users from 1st party

    \Input *pRef        - module ref
    \Input uUserIndex   - local user index
    \Input eType        - type of users to query
    \Input iLimit       - maximum users to return per query
    \Input iOffset      - start index of query
    \Input *pFilter     - unused
    \Input *pNotifyCb   - callback function pointer
    \Input uTypeMask    - mask
    \Input *pUserData   - user data

    \Output
        int32_t         - zero=success; negative=error

    \Version 03/05/2020 (eesponda)
*/
/*************************************************************************************F*/
int32_t UserListApiGetListAsync(UserListApiRefT *pRef, uint32_t uUserIndex, UserListApiTypeE eType, int32_t iLimit, int32_t iOffset, UserListApiFiltersT *pFilter, UserListApiCallbackT *pNotifyCb, uint32_t uTypeMask, void *pUserData)
{
    GgpPlayerId PlayerId;
    UserListApiRequestT *pRequest = &pRef->aRequests[uUserIndex];

    // attempt to pull out user
    if (NetConnStatus('gpid', uUserIndex, &PlayerId, sizeof(PlayerId)) < 0)
    {
        NetPrintf(("userlistapistadia: [%p] could not retrieve the user id at index %u\n", pRef, uUserIndex));
        return(USERLISTAPI_ERROR_NO_USER);
    }

    // make sure the parameters are valid
    if (pNotifyCb == NULL)
    {
        NetPrintf(("userlistapistadia: [%p] async request requires a callback\n", pRef));
        return(USERLISTAPI_ERROR_BAD_PARAM);
    }

    // check if we are still in-progress for a different request
    if (pRequest->pNotifyCb != NULL)
    {
        NetPrintf(("userlistapistadia: [%p] request for user index %u is still in progress, another request cannot be started\n", pRef, uUserIndex));
        return(USERLISTAPI_ERROR_INPROGRESS);
    }

    if (eType == USERLISTAPI_TYPE_FRIENDS)
    {
        // get the friends list
        pRequest->AsyncBlock = GgpListFriends(PlayerId);
    }
    else if (eType == USERLISTAPI_TYPE_BLOCKED)
    {
        // get the block list
        pRequest->AsyncBlock = GgpListBlockedPlayers(PlayerId);
    }
    else
    {
        NetPrintf(("userlistapistadia: [%p] type %d is not supported on this platform\n", pRef, eType));
        _UserListApiCleanupRequestData(pRef, pRequest);
        return(USERLISTAPI_ERROR_UNSUPPORTED);
    }
    pRequest->uUserIndex = uUserIndex;
    pRequest->eType = eType;
    pRequest->iLimit = iLimit;
    pRequest->iOffset = iOffset;
    pRequest->pNotifyCb = pNotifyCb;
    pRequest->pUserData = pUserData;
    pRequest->uTypeMask = uTypeMask;
    // success
    return(0);
}

/*F*************************************************************************************/
/*!
    \Function   UserListApiCancelAll

    \Description
        Cancels all active requests.

    \Input *pRef       - module ref

    \Version 03/05/2020 (eesponda)
*/
/*************************************************************************************F*/
void UserListApiCancelAll(UserListApiRefT *pRef)
{
    uint32_t uUserIndex;

    // loop through all the requests to cancel them
    for (uUserIndex = 0; uUserIndex < NETCONN_MAXLOCALUSERS; uUserIndex += 1)
    {
        UserListApiRequestT *pRequest = &pRef->aRequests[uUserIndex];

        // if the callback is null we are currently not handling a request
        if (pRequest->pNotifyCb == NULL)
        {
            continue;
        }

        // cancel the request and clear the data
        GgpFutureDetach(pRequest->AsyncBlock);
        GgpFutureDetach(pRequest->PresenceBlock);
        _UserListApiCleanupRequestData(pRef, pRequest);
    }
}
