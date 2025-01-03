/*H*************************************************************************************/
/*!
    \File userapistadia.c

    \Description
        Implements the stadia specific code to request UserProfiles, and parse the responses.

    \Copyright
        Copyright (c) Electronic Arts 2020

    \Version 03/05/2020 (eesponda)
*/
/*************************************************************************************H*/

/*** Include files *********************************************************************/

#include "DirtySDK/platform.h"

EA_DISABLE_CLANG_WARNING(-Wstrict-prototypes)
#include <ggp_c/status.h>
#include <ggp_c/profile.h>
#include <ggp_c/presence.h>
#include <ggp_c/version.h>
EA_RESTORE_CLANG_WARNING()

#include <string.h>
#include <stdlib.h>

#include "DirtySDK/dirtysock.h"
#include "DirtySDK/dirtysock/dirtyerr.h"
#include "DirtySDK/dirtysock/dirtylib.h"
#include "DirtySDK/dirtysock/dirtymem.h"
#include "DirtySDK/dirtysock/netconn.h"
#include "DirtySDK/misc/userapi.h"

/*** Defines ***************************************************************************/

/*** Macros ****************************************************************************/

//! macro that wraps the call to DirtyErrGetNameList for the callback status
#define USERAPI_GetGgpStatus(iStatus) DirtyErrGetNameList((iStatus), _UserApi_aGgpStatus)

/*** Type Definitions ******************************************************************/

typedef struct UserApiRequestDataT
{
    GgpFuture ProfileBlock;         //!< async block used for profile(s) handling
    GgpFuture PresenceBlock;        //!< asnyc block used for presence handling
    UserApiCallbackT *pCallback;    //!< callback fired after the get async completion
    void *pUserData;                //!< callback user data
    uint32_t uUserDataMask;         //!< mask of what is being request (USERAPI_MASK_*)
    int32_t iLookupUsersLength;     //!< amount of users being request
    uint32_t uUserIndex;            //!< user index of the requestors
} UserApiRequestDataT;

struct UserApiRefT
{
    int32_t iMemGroup;                                      //!< memgroup id
    void *pMemGroupUserData;                                //!< memgroup userdata
    UserApiRequestDataT aRequests[NETCONN_MAXLOCALUSERS];   //!< request data
};

/*** Function Prototypes ***************************************************************/

/*** Variables *************************************************************************/

#if DIRTYCODE_LOGGING
//! the different statuses returned by the ggp api functions
static const DirtyErrT _UserApi_aGgpStatus[] =
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
    \Function _UserApiConvertError

    \Description
        Converts the hresult to the userapi error

    \Output
        UserApiEventErrorE  - the converted error

    \Version 03/05/2020 (eesponda)
*/
/*************************************************************************************F*/
static UserApiEventErrorE _UserApiConvertError(int32_t iResult)
{
    // set the error based on the condition that is mappable to one of our errors. non-mappable errors go to default
    switch (iResult)
    {
        case kGgpStatusCode_Ok:
            return(USERAPI_ERROR_OK);
        case kGgpStatusCode_PermissionDenied:
            return(USERAPI_ERROR_REQUEST_FORBIDDEN);
        case kGgpStatusCode_NotFound:
            return(USERAPI_ERROR_REQUEST_NOT_FOUND);
        case kGgpStatusCode_DeadlineExceeded:
            return(USERAPI_ERROR_REQUEST_TIMEOUT);
        default:
            return(USERAPI_ERROR_REQUEST_FAILED);
    }
}

/*F*************************************************************************************/
/*!
    \Function _UserApiFireEndOfList

    \Description
        Fires the end of the list callback to the user

    \Input *pRef        - module state
    \Input *pRequest    - request data
    \Input iNumReceived - the number of received results
    \Input iNumErrors   - the number of results that error

    \Version 03/05/2020 (eesponda)
*/
/*************************************************************************************F*/
static void _UserApiFireEndOfList(UserApiRefT *pRef, UserApiRequestDataT *pRequest, int32_t iNumReceived, int32_t iNumErrors)
{
    UserApiEventDataT EventData;

    // fill in the error information
    ds_memclr(&EventData, sizeof(EventData));
    EventData.eEventType = USERAPI_EVENT_END_OF_LIST;
    EventData.uUserIndex = pRequest->uUserIndex;
    EventData.eError = (iNumErrors > 0) ? USERAPI_ERROR_REQUEST_FAILED : USERAPI_ERROR_OK;

    // fill in the end of list specific information
    EventData.EventDetails.EndOfList.iTotalRequested = pRequest->iLookupUsersLength;
    EventData.EventDetails.EndOfList.iTotalReceived = iNumReceived;
    EventData.EventDetails.EndOfList.iTotalErrors = iNumErrors;

    // fire the callback
    pRequest->pCallback(pRef, &EventData, pRequest->pUserData);
}

/*F*************************************************************************************/
/*!
    \Function _UserApiFilloutProfile

    \Description
        Fillout the profile data for the callback response

    \Input *pRef        - module state
    \Input *pRequest    - request data
    \Input *pEventData  - the event data we are filling out
    \Input *pProfile    - [out] profile data we pulled from the result

    \Output
        int32_t         - result of pulling the profile response

    \Notes
        We need to pass in the profile data because we are pointing to it via pRawData
        and we don't want it to go out of scope.

    \Version 03/05/2020 (eesponda)
*/
/*************************************************************************************F*/
static int32_t _UserApiFilloutProfile(UserApiRefT *pRef, UserApiRequestDataT *pRequest, UserApiEventDataT *pEventData, GgpProfile *pProfile)
{
    GgpStatus Status;
    char strError[256];

    if (!GgpFutureGetResult(pRequest->ProfileBlock, pProfile, sizeof(*pProfile), &Status))
    {
        GgpGetStatusMessage(Status.message_token, strError, sizeof(strError));
        NetPrintf(("userapistadia: [%p] failed to get user profile result (err=%s/%s)\n", pRef, USERAPI_GetGgpStatus(Status.status_code), strError));
        return(Status.status_code);
    }

    // set the callback flags
    pEventData->EventDetails.UserData.uUserDataMask |= USERAPI_MASK_PROFILE;

    // set the user
    DirtyUserFromNativeUser(&pEventData->EventDetails.UserData.DirtyUser, &pProfile->player_id);
    #if GGP_VERSION >= GGP_MAKE_VERSION(1, 50, 0)
    ds_strnzcpy(pEventData->EventDetails.UserData.Profile.strGamertag, pProfile->stadia_name, sizeof(pEventData->EventDetails.UserData.Profile.strGamertag));
    #else
    ds_strnzcpy(pEventData->EventDetails.UserData.Profile.strGamertag, pProfile->gamer_tag, sizeof(pEventData->EventDetails.UserData.Profile.strGamertag));
    #endif

    // save the raw data
    pEventData->EventDetails.UserData.Profile.pRawData = (void *)pProfile;

    return(Status.status_code);
}

/*F*************************************************************************************/
/*!
    \Function _UserApiFilloutPresence

    \Description
        Fillout the presence data for the callback response

    \Input *pRef        - module state
    \Input *pRequest    - request data
    \Input *pEventData  - the event data we are filling out
    \Input *pPresence   - [out] presence data we pulled from the result

    \Output
        int32_t         - result of pulling the presence response

    \Notes
        We need to pass in the presence data because we are pointing to it via pRawData
        and we don't want it to go out of scope.

    \Version 03/05/2020 (eesponda)
*/
/*************************************************************************************F*/
static int32_t _UserApiFilloutPresence(UserApiRefT *pRef, UserApiRequestDataT *pRequest, UserApiEventDataT *pEventData, GgpPresence *pPresence)
{
    GgpStatus Status;
    char strError[256];

    // get the result of the operation
    if (!GgpFutureGetResult(pRequest->PresenceBlock, pPresence, sizeof(*pPresence), &Status))
    {
        GgpGetStatusMessage(Status.message_token, strError, sizeof(strError));
        NetPrintf(("userapistadia: [%p] failed to get presence result (err=%s/%s)\n", pRef, USERAPI_GetGgpStatus(Status.status_code), strError));
        return(Status.status_code);
    }

    // set the callback flags
    pEventData->EventDetails.UserData.uUserDataMask |= USERAPI_MASK_PRESENCE;

    // set the user
    DirtyUserFromNativeUser(&pEventData->EventDetails.UserData.DirtyUser, &pPresence->player_id);

    // get the presence state of the user
    switch (pPresence->availability)
    {
        case kGgpPresenceAvailability_LookingForParty:
        case kGgpPresenceAvailability_Online:
        {
            pEventData->EventDetails.UserData.Presence.ePresenceStatus = USERAPI_PRESENCE_ONLINE;
            break;
        }
        case kGgpPresenceAvailability_Busy:
        case kGgpPresenceAvailability_Away:
        {
            pEventData->EventDetails.UserData.Presence.ePresenceStatus = USERAPI_PRESENCE_AWAY;
            break;
        }
        case kGgpPresenceAvailability_Offline:
        {
            pEventData->EventDetails.UserData.Presence.ePresenceStatus = USERAPI_PRESENCE_OFFLINE;
            break;
        }
        default:
        {
            pEventData->EventDetails.UserData.Presence.ePresenceStatus = USERAPI_PRESENCE_UNKNOWN;
            break;
        }
    }

    // save the raw data
    pEventData->EventDetails.UserData.Presence.pRawData = (void *)pPresence;

    return(Status.status_code);
}

/*F*************************************************************************************/
/*!
    \Function _UserApiHandleProfiles

    \Description
        Handles the profiles request and fire the appropriate callbacks

    \Input *pRef        - module state
    \Input *pRequest    - request data

    \Version 03/05/2020 (eesponda)
*/
/*************************************************************************************F*/
static void _UserApiHandleProfiles(UserApiRefT *pRef, UserApiRequestDataT *pRequest)
{
    GgpProfile *pProfiles;
    int32_t iNumProfiles, iProfile, iProfileSize;
    GgpStatus Status;
    char strError[256];

    // get size of the result
    iNumProfiles = GgpFutureGetResultCount(pRequest->ProfileBlock, &Status);
    if (Status.status_code != kGgpStatusCode_Ok)
    {
        GgpGetStatusMessage(Status.message_token, strError, sizeof(strError));
        NetPrintf(("userapistadia: [%p] failed to get user profiles result count (err=%s/%s)\n", pRef, USERAPI_GetGgpStatus(Status.status_code), strError));
        _UserApiFireEndOfList(pRef, pRequest, 0, pRequest->iLookupUsersLength);
        return;
    }
    if (iNumProfiles == 0)
    {
        NetPrintf(("userapistadia: [%p] user profiles result did not return any profiles\n", pRef));
        _UserApiFireEndOfList(pRef, pRequest, 0, pRequest->iLookupUsersLength);
        return;
    }
    iProfileSize = sizeof(*pProfiles) * iNumProfiles;
    // allocate the memory for the result
    if ((pProfiles = (GgpProfile *)DirtyMemAlloc(iProfileSize, USERAPI_MEMID, pRef->iMemGroup, pRef->pMemGroupUserData)) == NULL)
    {
        NetPrintf(("userapistadia: [%p] failed to allocate space for the user profiles result\n", pRef));
        _UserApiFireEndOfList(pRef, pRequest, 0, pRequest->iLookupUsersLength);
        return;
    }
    // get the result of the operation
    if (!GgpFutureGetMultipleResult(pRequest->ProfileBlock, pProfiles, iProfileSize, iNumProfiles, NULL, &Status))
    {
        GgpGetStatusMessage(Status.message_token, strError, sizeof(strError));
        NetPrintf(("userapistadia: [%p] failed to get user profiles result (err=%s/%s)\n", pRef, USERAPI_GetGgpStatus(Status.status_code), strError));
        _UserApiFireEndOfList(pRef, pRequest, 0, pRequest->iLookupUsersLength);
        DirtyMemFree(pProfiles, USERAPI_MEMID, pRef->iMemGroup, pRef->pMemGroupUserData);
        return;
    }

    // loop through the profiles and send them back to the user
    for (iProfile = 0; iProfile < iNumProfiles; iProfile += 1)
    {
        UserApiEventDataT EventData;
        const GgpProfile *pProfile = &pProfiles[iProfile];

        // fill out the information using the profile
        ds_memclr(&EventData, sizeof(EventData));
        EventData.eEventType = USERAPI_EVENT_DATA;
        EventData.uUserIndex = pRequest->uUserIndex;
        DirtyUserFromNativeUser(&EventData.EventDetails.UserData.DirtyUser, &pProfile->player_id);
        EventData.EventDetails.UserData.uUserDataMask = USERAPI_MASK_PROFILE;
        #if GGP_VERSION >= GGP_MAKE_VERSION(1, 50, 0)
        ds_strnzcpy(EventData.EventDetails.UserData.Profile.strGamertag, pProfile->stadia_name, sizeof(EventData.EventDetails.UserData.Profile.strGamertag));
        #else
        ds_strnzcpy(EventData.EventDetails.UserData.Profile.strGamertag, pProfile->gamer_tag, sizeof(EventData.EventDetails.UserData.Profile.strGamertag));
        #endif
        EventData.EventDetails.UserData.Profile.pRawData = (void *)pProfile;

        // fire the callback
        pRequest->pCallback(pRef, &EventData, pRequest->pUserData);
    }

    // cleanup the allocated memory
    DirtyMemFree(pProfiles, USERAPI_MEMID, pRef->iMemGroup, pRef->pMemGroupUserData);

    // fire the end of callback
    _UserApiFireEndOfList(pRef, pRequest, iNumProfiles, 0);
}

/*F*************************************************************************************/
/*!
    \Function _UserApiHandleRequest

    \Description
        Handle the requests that are pending

    \Input *pRef        - module state
    \Input *pRequest    - request data

    \Version 03/05/2020 (eesponda)
*/
/*************************************************************************************F*/
static void _UserApiHandleRequest(UserApiRefT *pRef, UserApiRequestDataT *pRequest)
{
    if (pRequest->uUserDataMask & USERAPI_MASK_PROFILES)
    {
        _UserApiHandleProfiles(pRef, pRequest);
    }
    else
    {
        UserApiEventDataT EventData;
        GgpProfile Profile;
        GgpPresence Presence;
        int32_t iResult;

        // handle the data
        ds_memclr(&EventData, sizeof(EventData));
        EventData.eEventType = USERAPI_EVENT_DATA;
        EventData.uUserIndex = pRequest->uUserIndex;
        EventData.eError = USERAPI_ERROR_OK;

        if (pRequest->uUserDataMask & USERAPI_MASK_PROFILE)
        {
            if ((iResult = _UserApiFilloutProfile(pRef, pRequest, &EventData, &Profile)) != kGgpStatusCode_Ok)
            {
                EventData.eError = _UserApiConvertError(iResult);
            }
        }
        if (pRequest->uUserDataMask & USERAPI_MASK_PRESENCE)
        {
            if ((iResult = _UserApiFilloutPresence(pRef, pRequest, &EventData, &Presence)) != kGgpStatusCode_Ok)
            {
                EventData.eError = _UserApiConvertError(iResult);
            }
        }

        // fire the data callback
        pRequest->pCallback(pRef, &EventData, pRequest->pUserData);

        // fire end of list
        if (EventData.eError == USERAPI_ERROR_OK)
        {
            _UserApiFireEndOfList(pRef, pRequest, pRequest->iLookupUsersLength, 0);
        }
        else
        {
            _UserApiFireEndOfList(pRef, pRequest, 0, pRequest->iLookupUsersLength);
        }
    }
}

/*** Public functions ******************************************************************/

/*F*************************************************************************************/
/*!
    \Function UserApiCreate

    \Description
        Create the userapi module

    \Output
        UserApiRefT *   - the module state or NULL on failure

    \Version 03/05/2020 (eesponda)
*/
/*************************************************************************************F*/
UserApiRefT *UserApiCreate(void)
{
    UserApiRefT *pRef;
    int32_t iMemGroup;
    void *pMemGroupUserData;

    // query memgroup info
    DirtyMemGroupQuery(&iMemGroup, &pMemGroupUserData);

    // allocate the platform state
    if ((pRef = (UserApiRefT *)DirtyMemAlloc(sizeof(*pRef), USERAPI_MEMID, iMemGroup, pMemGroupUserData)) == NULL)
    {
        NetPrintf(("userapistadia: could not allocate module state\n"));
        return(NULL);
    }
    ds_memclr(pRef, sizeof(*pRef));
    pRef->iMemGroup = iMemGroup;
    pRef->pMemGroupUserData = pMemGroupUserData;

    return(pRef);
}

/*F*************************************************************************************/
/*!
    \Function UserApiDestroy

    \Description
        Destroys the userapi module

    \Input *pRef    - module state

    \Output
        int32_t     - 0=success, negative=failure

    \Version 03/05/2020 (eesponda)
*/
/*************************************************************************************F*/
int32_t UserApiDestroy(UserApiRefT *pRef)
{
    int32_t iUserIndex;

    // loop through all the requests to cancel
    for (iUserIndex = 0; iUserIndex < NETCONN_MAXLOCALUSERS; iUserIndex += 1)
    {
        UserApiCancel(pRef, iUserIndex);
    }

    DirtyMemFree(pRef, USERAPI_MEMID, pRef->iMemGroup, pRef->pMemGroupUserData);
    return(0);
}

/*F********************************************************************************/
/*!
    \Function UserApiStatus

    \Description
        Get status information.

    \Input *pRef    - pointer to module state
    \Input iSelect  - status selector
    \Input *pBuf    - [out] storage for selector-specific output
    \Input iBufSize - size of output buffer

    \Output
        int32_t     - Return 0 if successful. Otherwise a selector specific error

    \Notes
        There is currently nothing to query with this module.  This is a placeholder for future implementations.

        \verbatim
        \endverbatim

    \Version 03/05/2020 (eesponda)
*/
/********************************************************************************F*/
int32_t UserApiStatus(UserApiRefT *pRef, int32_t iSelect, void *pBuf, int32_t iBufSize)
{
    return(-1);
}

/*F************************************************************************************/
/*!
    \Function UserApiControl

    \Description
        Control behavior of module.

    \Input *pRef    - pointer to module state
    \Input iControl - status selector
    \Input iValue   - control value
    \Input iValue2  - control value
    \Input *pValue  - control value

    \Output
        int32_t     - Return 0 if successful. Otherwise a selector specific error

    \Notes
        iControl can be one of the following:

        \verbatim
        \endverbatim

    \Version 03/05/2020 (eesponda)
*/
/*************************************************************************************F*/
int32_t UserApiControl(UserApiRefT *pRef, int32_t iControl, int32_t iValue, int32_t iValue2, void *pValue)
{
    return(-1);
}

/*F*************************************************************************************/
/*!
    \Function UserApiRequestProfilesAsync

    \Description
        Request a batch of profiles

    \Input *pRef                - module state
    \Input uUserIndex           - index of the local user
    \Input *pLookupUsers        - users we are looking up
    \Input iLookupUsersLength   - the number of users
    \Input *pCallback           - the callback we fire on completion
    \Input *pUserData           - callback user data

    \Output
        int32_t                 - 0=success, negative=failure

    \Version 03/05/2020 (eesponda)
*/
/*************************************************************************************F*/
int32_t UserApiRequestProfilesAsync(UserApiRefT *pRef, uint32_t uUserIndex, DirtyUserT *pLookupUsers, int32_t iLookupUsersLength, UserApiCallbackT *pCallback, void *pUserData)
{
    GgpPlayerId *pPlayerId;
    int32_t iLookupUser;
    UserApiRequestDataT *pRequest = &pRef->aRequests[uUserIndex];

    // allocate temporary buffer for the request
    if ((pPlayerId = (GgpPlayerId *)DirtyMemAlloc(sizeof(*pPlayerId) * iLookupUsersLength, USERAPI_MEMID, pRef->iMemGroup, pRef->pMemGroupUserData)) == NULL)
    {
        NetPrintf(("userapistadia: [%p] unable to allocate temporary buffer for profiles request\n", pRef));
        return(-1);
    }
    // fill in the request based on the inputs
    for (iLookupUser = 0; iLookupUser < iLookupUsersLength; iLookupUser += 1)
    {
        DirtyUserToNativeUser(&pPlayerId[iLookupUser], sizeof(*pPlayerId), &pLookupUsers[iLookupUser]);
    }

    // make sure the parameters are valid
    if (pCallback == NULL)
    {
        NetPrintf(("userapistadia: [%p] async request requires a callback\n", pRef));
        DirtyMemFree(pPlayerId, USERAPI_MEMID, pRef->iMemGroup, pRef->pMemGroupUserData);
        return(-1);
    }

    // check if we are still in-progress for a different request
    if (pRequest->pCallback != NULL)
    {
        NetPrintf(("userapistadia: [%p] request for user index %u is still in progress, another request cannot be started\n", pRef, uUserIndex));
        DirtyMemFree(pPlayerId, USERAPI_MEMID, pRef->iMemGroup, pRef->pMemGroupUserData);
        return(-1);
    }

    // start the async fetch of the profile
    pRequest->ProfileBlock = GgpBatchGetProfile(pPlayerId, iLookupUsersLength);
    pRequest->pCallback = pCallback;
    pRequest->pUserData = pUserData;
    pRequest->iLookupUsersLength = iLookupUsersLength;
    pRequest->uUserIndex = uUserIndex;
    pRequest->uUserDataMask = USERAPI_MASK_PROFILES;
    DirtyMemFree(pPlayerId, USERAPI_MEMID, pRef->iMemGroup, pRef->pMemGroupUserData);
    return(0);
}

/*F*************************************************************************************/
/*!
    \Function UserApiRequestProfileAsync

    \Description
        Request the profile data along with any additional data based on the data mask

    \Input *pRef        - module state
    \Input uUserIndex   - index of the local user
    \Input *pLookupUser - user we are looking up
    \Input *pCallback   - the callback we fire on completion
    \Input uUserDataMask- identifies what information we need to pull
    \Input *pUserData   - callback user data

    \Output
        int32_t         - 0=success, negative=failure

    \Version 03/05/2020 (eesponda)
*/
/*************************************************************************************F*/
int32_t UserApiRequestProfileAsync(UserApiRefT *pRef, uint32_t uUserIndex, DirtyUserT *pLookupUser, UserApiCallbackT *pCallback, uint32_t uUserDataMask, void *pUserData)
{
    // presence can also request the profile with the correct uUserDataMask flags
    return(UserApiRequestPresenceAsync(pRef, uUserIndex, pLookupUser, pCallback, uUserDataMask, pUserData));
}

/*F*************************************************************************************/
/*!
    \Function UserApiRequestPresenceAsync

    \Description
        Request the presence data along with any additional data based on the data mask

    \Input *pRef        - module state
    \Input uUserIndex   - index of the local user
    \Input *pLookupUser - user we are looking up
    \Input *pCallback   - the callback we fire on completion
    \Input uUserDataMask- identifies what information we need to pull
    \Input *pUserData   - callback user data

    \Output
        int32_t         - 0=success, negative=failure

    \Version 03/05/2020 (eesponda)
*/
/*************************************************************************************F*/
int32_t UserApiRequestPresenceAsync(UserApiRefT *pRef, uint32_t uUserIndex, DirtyUserT *pLookupUser, UserApiCallbackT *pCallback, uint32_t uUserDataMask, void *pUserData)
{
    GgpPlayerId PlayerId;
    UserApiRequestDataT *pRequest = &pRef->aRequests[uUserIndex];

    // pull out the user
    DirtyUserToNativeUser(&PlayerId, sizeof(PlayerId), pLookupUser);

    // make sure the parameters are valid
    if (pCallback == NULL)
    {
        NetPrintf(("userapistadia: [%p] async request requires a callback\n", pRef));
        return(-1);
    }
    // check if we are still in-progress for a different request
    if (pRequest->pCallback != NULL)
    {
        NetPrintf(("userapistadia: [%p] request for user index %u is still in progress, another request cannot be started\n", pRef, uUserIndex));
        return(-1);
    }

    // make the request based on the mask
    if (uUserDataMask & USERAPI_MASK_PRESENCE)
    {
        // start the async fetch of the presence
        pRequest->PresenceBlock = GgpBatchGetPresence(&PlayerId, 1);
    }
    if (uUserDataMask & USERAPI_MASK_PROFILE)
    {
        // start the async fetch of the profile
        pRequest->ProfileBlock = GgpGetProfile(PlayerId);
    }
    else
    {
        NetPrintf(("userapistadia: [%p] invalid mask used (mask=0x%08x)\n", pRef, uUserDataMask));
        return(-1);
    }
    pRequest->pCallback = pCallback;
    pRequest->pUserData = pUserData;
    pRequest->uUserDataMask = uUserDataMask;
    pRequest->uUserIndex = uUserIndex;
    pRequest->iLookupUsersLength = 1;
    return(0);
}

/*F*************************************************************************************/
/*!
    \Function UserApiRequestRichPresenceAsync

    \Description
        Request the rich presence data along with any additional data based on the data mask

    \Input *pRef        - module state
    \Input uUserIndex   - index of the local user
    \Input *pLookupUser - user we are looking up
    \Input *pCallback   - the callback we fire on completion
    \Input uUserDataMask- identifies what information we need to pull
    \Input *pUserData   - callback user data

    \Output
        int32_t         - 0=success, negative=failure

    \Version 03/05/2020 (eesponda)
*/
/*************************************************************************************F*/
int32_t UserApiRequestRichPresenceAsync(UserApiRefT *pRef, uint32_t uUserIndex, DirtyUserT *pLookupUser, UserApiCallbackT *pCallback, uint32_t uUserDataMask, void *pUserData)
{
    return(USERAPI_ERROR_UNSUPPORTED);
}

/*F*************************************************************************************/
/*!
    \Function UserApiPostRecentlyMetAsync

    \Description
        Posts new recently met information (unsupported)

    \Input *pRef            - module state
    \Input uUserIndex       - index of the local user
    \Input *pPlayerMet      - user we are posting we recently met
    \Input *pAdditionalInfo - additional info we pass along
    \Input *pCallback       - the callback we fire on completion
    \Input *pUserData       - callback user data

    \Output
        int32_t             - 0=success, negative=failure

    \Version 03/05/2020 (eesponda)
*/
/*************************************************************************************F*/
int32_t UserApiPostRecentlyMetAsync(UserApiRefT *pRef, uint32_t uUserIndex, DirtyUserT *pPlayerMet, void *pAdditionalInfo, UserApiPostCallbackT *pCallback, void *pUserData)
{
    return(USERAPI_ERROR_UNSUPPORTED);
}

/*F*************************************************************************************/
/*!
    \Function UserApiPostRichPresenceAsync

    \Description
        Posts new rich presence info

    \Input *pRef            - module state
    \Input uUserIndex       - index of the local user
    \Input *pData           - rich presence data we are posting
    \Input *pCallback       - the callback we fire on completion
    \Input *pUserData       - callback user data

    \Output
        int32_t             - 0=success, negative=failure

    \Version 03/05/2020 (eesponda)
*/
/*************************************************************************************F*/
int32_t UserApiPostRichPresenceAsync(UserApiRefT *pRef, uint32_t uUserIndex, UserApiRichPresenceT *pData, UserApiPostCallbackT *pCallback, void *pUserData)
{
    return(USERAPI_ERROR_UNSUPPORTED);
}

/*F*************************************************************************************/
/*!
    \Function UserApiRegisterUpdateEvent

    \Description
        Registers a callback for an update event

    \Input *pRef        - module state
    \Input uUserIndex   - index of the local user
    \Input eType        - type of event we are registering for
    \Input *pNotifyCb   - callback we fire when an event occurs
    \Input *pUserData   - the callback user data

    \Output
        int32_t         - 0=success, negative=failure

    \Version 03/05/2020 (eesponda)
*/
/*************************************************************************************F*/
int32_t UserApiRegisterUpdateEvent(UserApiRefT *pRef, uint32_t uUserIndex, UserApiNotifyTypeE eType, UserApiUpdateCallbackT *pNotifyCb, void *pUserData)
{
    return(USERAPI_ERROR_UNSUPPORTED);
}

/*F*************************************************************************************/
/*!
    \Function UserApiUpdate

    \Description
        Updates the module

    \Input *pRef    - module state

    \Version 03/05/2020 (eesponda)
*/
/*************************************************************************************F*/
void UserApiUpdate(UserApiRefT *pRef)
{
    int32_t iUserIndex;

    for (iUserIndex = 0; iUserIndex < NETCONN_MAXLOCALUSERS; iUserIndex += 1)
    {
        UserApiRequestDataT *pRequest = &pRef->aRequests[iUserIndex];
        if (pRequest->pCallback == NULL)
        {
            continue;
        }
        // the get callback is registered for get requests
        if (pRequest->pCallback != NULL)
        {
            if (((pRequest->uUserDataMask & USERAPI_MASK_PROFILE) || (pRequest->uUserDataMask & USERAPI_MASK_PROFILES)) && !GgpFutureIsReady(pRequest->ProfileBlock))
            {
                continue;
            }
            if ((pRequest->uUserDataMask & USERAPI_MASK_PRESENCE) && !GgpFutureIsReady(pRequest->PresenceBlock))
            {
                continue;
            }
            _UserApiHandleRequest(pRef, pRequest);
        }
        ds_memclr(pRequest, sizeof(*pRequest));
    }
}

/*F*************************************************************************************/
/*!
    \Function UserApiCancel

    \Description
        Cancels a request for the local user

    \Input *pRef        - module state
    \Input uUserIndex   - index of the local user

    \Output
        int32_t         - 0=success, negative=failure

    \Version 03/05/2020 (eesponda)
*/
/*************************************************************************************F*/
int32_t UserApiCancel(UserApiRefT *pRef, uint32_t uUserIndex)
{
    UserApiRequestDataT *pRequest = &pRef->aRequests[uUserIndex];
    if (pRequest->pCallback == NULL)
    {
        return(0);
    }

    // cancel the pending async operations
    if ((pRequest->uUserDataMask & USERAPI_MASK_PROFILE) || (pRequest->uUserDataMask & USERAPI_MASK_PROFILES))
    {
        GgpFutureDetach(pRequest->ProfileBlock);
    }
    if (pRequest->uUserDataMask & USERAPI_MASK_PRESENCE)
    {
        GgpFutureDetach(pRequest->PresenceBlock);
    }

    // clear the request data
    ds_memclr(pRequest, sizeof(*pRequest));
    return(0);
}
