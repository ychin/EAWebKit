/*H*************************************************************************************/
/*!
    \File userlistapigdk.cpp

    \Description
        See userlistapi.h

    \Copyright
        Copyright (c) Electronic Arts 2020

    \Version 01/10/2020 (eesponda)
*/
/*************************************************************************************H*/

/*** Include files *********************************************************************/

#include "DirtySDK/platform.h"

#include <stdio.h>
#include <stdlib.h>
#include <xsapi-c/services_c.h>
#include <XGame.h>

#include "DirtySDK/misc/userlistapi.h"
#include "DirtySDK/dirtysock.h"
#include "DirtySDK/dirtysock/dirtyerr.h"
#include "DirtySDK/dirtysock/dirtylib.h"
#include "DirtySDK/dirtysock/dirtymem.h"
#include "DirtySDK/dirtysock/netconn.h"

/*** Defines ***************************************************************************/

#define USERLISTAPI_HOMEMENU_TITLEID    (714681658) //!< title id for the home app

/*** Macros ****************************************************************************/

/*** Type Definitions ******************************************************************/

//! structure to store in the queue of requests
typedef struct UserListApiRequestT
{
    XblContextHandle pXblContext;       //!< xbox live context used for service calls
    XAsyncBlock AsyncBlock;             //!< main async block used for asynchronous handling
    XAsyncBlock ProfileBlock;           //!< profile async block if required
    XAsyncBlock PresenceBlock;          //!< presence async block if required
    uint32_t uUserIndex;                //!< local index of the user requesting
    UserListApiTypeE eType;             //!< type of request
    int32_t iLimit;                     //!< request limit (maximum results)
    int32_t iOffset;                    //!< offsets to start
    UserListApiFiltersT Filter;         //!< filter applied to the request
    UserListApiCallbackT *pNotifyCb;    //!< callback function pointer
    void *pUserData;                    //!< user data for the callback
    uint32_t uTypeMask;                 //!< mask for the request
    XblSocialRelationshipResultHandle pResult; //!< handle for result, for multi-page responses this holds the previous page

    XblUserProfile *pProfiles;          //!< profiles for the social group
    int32_t iNumProfiles;               //!< number of profiles retunred

    XblPresenceRecordHandle *pPresenceRecords;  //!< presence records for the social group
    int32_t iNumPresenceRecords;                //!< number of presence records returned
} UserListApiRequestT;

//! module state
struct UserListApiRefT
{
    int32_t  iMemGroup;                                     //!< module mem group id
    void    *pMemGroupUserData;                             //!< user data associated with mem group
    UserListApiRequestT aRequests[NETCONN_MAXLOCALUSERS];   //!< request data for each user
};

/*** Function Prototypes ***************************************************************/

/*** Variables *************************************************************************/

/*** Private functions *****************************************************************/

/*F*************************************************************************************/
/*!
    \Function _UserListApiHandleListError

    \Description
        Fires the end callback with the error information

    \Input *pRef        - module ref
    \Input *pRequest    - user index specific request we are handling
    \Input hError       - error the occurred

    \Version  01/10/2020 (eesponda)
*/
/*************************************************************************************F*/
static void _UserListApiHandleListError(UserListApiRefT *pRef, UserListApiRequestT *pRequest, HRESULT hError)
{
    uint32_t uDelayInSecs;
    UserListApiEventDataT EventData;
    ds_memclr(&EventData, sizeof(EventData));

    // fill in the information
    EventData.ListEndData.eRequestType = pRequest->eType;
    EventData.ListEndData.pUserListFilters = &pRequest->Filter;
    EventData.ListEndData.uUserIndex = pRequest->uUserIndex;
    EventData.ListEndData.iLimit = pRequest->iLimit;
    EventData.ListEndData.iOffset = pRequest->iOffset;
    EventData.ListEndData.iTotalFriendCount = -1;
    EventData.ListEndData.Error = hError;

    // handle special case for rate limiting
    if ((XblGetErrorCondition(hError) == XblErrorCondition::Http429TooManyRequests) && (SUCCEEDED(XblContextSettingsGetHttpRetryDelay(pRequest->pXblContext, &uDelayInSecs))))
    {
        EventData.ListEndData.Error = USERLISTAPI_ERROR_RATE_LIMITED;
        EventData.ListEndData.iRateLimRetrySecs = (signed)uDelayInSecs;
    }

    // fire the callback
    pRequest->pNotifyCb(pRef, TYPE_LIST_END, &EventData, pRequest->pUserData);
}

/*F*************************************************************************************/
/*!
    \Function   _UserListApiHandleProfileResponse

    \Description
        Handles the profile response by pulling it out of the xasync

    \Input *pRef        - module state
    \Input *pRequest    - request data

    \Version 01/14/2020 (eesponda)
*/
/*************************************************************************************F*/
static void _UserListApiHandleProfileResponse(UserListApiRefT *pRef, UserListApiRequestT *pRequest)
{
    HRESULT hResult;
    size_t uResultCount;

    if (FAILED(hResult = XblProfileGetUserProfilesForSocialGroupResultCount(&pRequest->ProfileBlock, &uResultCount)))
    {
        NetPrintf(("userlistapigdk: [%p] failed to get the user profiles for social group result count (err=%s)\n", pRef, DirtyErrGetName(hResult)));
        return;
    }
    if (uResultCount == 0)
    {
        return;
    }
    if ((pRequest->pProfiles = (XblUserProfile *)DirtyMemAlloc(sizeof(*pRequest->pProfiles) * (signed)uResultCount, USERLISTAPI_MEMID, pRef->iMemGroup, pRef->pMemGroupUserData)) == NULL)
    {
        NetPrintf(("userlistapigdk: [%p] failed to allocate memory for the user profiles\n", pRef));
        return;
    }
    if (SUCCEEDED(hResult = XblProfileGetUserProfilesForSocialGroupResult(&pRequest->ProfileBlock, uResultCount, pRequest->pProfiles)))
    {
        pRequest->iNumProfiles = (signed)uResultCount;
    }
    else
    {
        NetPrintf(("userlistapigdk: [%p] failed to get user profile for social group (err=%s)\n", pRef, DirtyErrGetName(hResult)));
        DirtyMemFree(pRequest->pProfiles, USERLISTAPI_MEMID, pRef->iMemGroup, pRef->pMemGroupUserData);
    }
}

/*F*************************************************************************************/
/*!
    \Function   _UserListApiHandlePresenceResponse

    \Description
        Handles the presence response by pulling it out of the xasync

    \Input *pRef        - module state
    \Input *pRequest    - request data

    \Version 01/14/2020 (eesponda)
*/
/*************************************************************************************F*/
static void _UserListApiHandlePresenceResponse(UserListApiRefT *pRef, UserListApiRequestT *pRequest)
{
    HRESULT hResult;
    size_t uResultCount;

    if (FAILED(hResult = XblPresenceGetPresenceForSocialGroupResultCount(&pRequest->PresenceBlock, &uResultCount)))
    {
        NetPrintf(("userlistapigdk: [%p] failed to get the presence for social group result count (err=%s)\n", pRef, DirtyErrGetName(hResult)));
        return;
    }
    if (uResultCount == 0)
    {
        return;
    }
    if ((pRequest->pPresenceRecords = (XblPresenceRecordHandle *)DirtyMemAlloc(sizeof(*pRequest->pPresenceRecords) * (signed)uResultCount, USERLISTAPI_MEMID, pRef->iMemGroup, pRef->pMemGroupUserData)) == NULL)
    {
        NetPrintf(("userlistapigdk: [%p] failed to allocate memory for the presence record handles\n", pRef));
        return;
    }
    if (SUCCEEDED(hResult = XblPresenceGetPresenceForSocialGroupResult(&pRequest->PresenceBlock, pRequest->pPresenceRecords, uResultCount)))
    {
        pRequest->iNumPresenceRecords = (signed)uResultCount;
    }
    else
    {
        NetPrintf(("userlistapigdk: [%p] failed to get the presence for social group result (err=%s)\n", pRef, DirtyErrGetName(hResult)));
        DirtyMemFree(pRequest->pPresenceRecords, USERLISTAPI_MEMID, pRef->iMemGroup, pRef->pMemGroupUserData);
    }
}

/*F*************************************************************************************/
/*!
    \Function   _UserListApiFindProfile

    \Description
        Attempts to find the profile by xuid

    \Input *pRef                - module state
    \Input *pRequest            - request data
    \Input uXuid                - the xbox userid of the user in the list

    \Output
        const XblUserProfile *  - found profile or NULL

    \Version 01/14/2020 (eesponda)
*/
/*************************************************************************************F*/
static const XblUserProfile *_UserListApiFindProfile(UserListApiRefT *pRef, UserListApiRequestT *pRequest, uint64_t uXuid)
{
    const XblUserProfile *pProfile = NULL;
    int32_t iProfileIndex;

    for (iProfileIndex = 0; iProfileIndex < pRequest->iNumProfiles; iProfileIndex += 1)
    {
        if (pRequest->pProfiles[iProfileIndex].xboxUserId == uXuid)
        {
            pProfile = &pRequest->pProfiles[iProfileIndex];
            break;
        }
    }

    return(pProfile);
}

/*F*************************************************************************************/
/*!
    \Function   _UserListApiFindPresenceHandle

    \Description
        Attempts to find the presence handle by xuid

    \Input *pRef                        - module state
    \Input *pRequest                    - request data
    \Input uXuid                        - the xbox userid of the user in the list

    \Output
        const XblPresenceRecordHandle * - found record or NULL

    \Version 01/14/2020 (eesponda)
*/
/*************************************************************************************F*/
static const XblPresenceRecordHandle *_UserListApiFindPresenceHandle(UserListApiRefT *pRef, UserListApiRequestT *pRequest, uint64_t uXuid)
{
    const XblPresenceRecordHandle *pRecordHandle = NULL;
    int32_t iPresenceIndex;
    HRESULT hResult;

    for (iPresenceIndex = 0; iPresenceIndex < pRequest->iNumPresenceRecords; iPresenceIndex += 1)
    {
        uint64_t uXuid2;
        if (FAILED(hResult = XblPresenceRecordGetXuid(pRequest->pPresenceRecords[iPresenceIndex], &uXuid2)))
        {
            NetPrintf(("userlistapigdk: [%p] getting the presence record's xuid failed (err=%s)\n", pRef, DirtyErrGetName(hResult)));
            continue;
        }
        if (uXuid == uXuid2)
        {
            pRecordHandle = &pRequest->pPresenceRecords[iPresenceIndex];
            break;
        }
    }

    return(pRecordHandle);
}

/*F*************************************************************************************/
/*!
    \Function   _UserListApiFilloutProfile

    \Description
        Fills out the profile data part of the callback events

    \Input *pRef        - module state
    \Input *pRequest    - request data
    \Input *pEventData  - the callback data we are filling out
    \Input uXuid        - the xbox userid of the user in the list

    \Output
        int32_t         - 0=success, negative=failure

    \Notes
        We assume that there is profile information for each user. Thus, we treat
        not having profile data as a failure.

    \Version 01/14/2020 (eesponda)
*/
/*************************************************************************************F*/
static int32_t _UserListApiFilloutProfile(UserListApiRefT *pRef, UserListApiRequestT *pRequest, UserApiUserDataT *pEventData, uint64_t uXuid)
{
    if (const XblUserProfile *pProfile = _UserListApiFindProfile(pRef, pRequest, uXuid))
    {
        pEventData->uUserDataMask |= USERAPI_MASK_PROFILE;
        ds_strnzcpy(pEventData->Profile.strGamertag, pProfile->gamertag, sizeof(pEventData->Profile.strGamertag));
        ds_strnzcpy(pEventData->Profile.strDisplayName, pProfile->gameDisplayName, sizeof(pEventData->Profile.strDisplayName));
        ds_strnzcpy(pEventData->Profile.strAvatarUrl, pProfile->gameDisplayPictureResizeUri, sizeof(pEventData->Profile.strAvatarUrl));
        pEventData->Profile.pRawData = (void *)pProfile;
        return(0);
    }
    return(-1);
}

/*F*************************************************************************************/
/*!
    \Function   _UserListApiFilterPresence

    \Description
        Uses an algorithm to filter the various title and device presence records

    \Input *pDeviceRecords  - the presence on the many devices supported by microsoft
    \Input uNumDeviceRecords- the number of records
    \Input uTitleId         - the current title id
    \Input **pDeviceRecord  - [out] the device record that was choosen
    \Input **pTitleRecord   - [out] the title record that was choosen

    \Output
        bool                - true=successfully selected, false=unsuccessful in selection

    \Version 01/14/2020 (eesponda)
*/
/*************************************************************************************F*/
static bool _UserListApiFilterPresence(const XblPresenceDeviceRecord *pDeviceRecords, size_t uNumDeviceRecords, uint32_t uTitleId, const XblPresenceDeviceRecord **pDeviceRecord, const XblPresenceTitleRecord **pTitleRecord)
{
    // $$TODO$$ taken from the xboxone version. come up with better algorithm for picking the presence records

    // For future consideration Win10 and XB1 will be able to play together eventually we might wish to check the priority for xbl1 first than win10.

    // Priority is chosen based on the following order,
    // Preference for the LAST item in the list;
    // Same console, same title
    // Same console different title (not home menu)
    // Same console
    // Different console
    // Inactive
    // Note that originally players playing the same title on different consoles ranked #2 but this comparison is currently impossible without
    // foreknowledge of the particular title's equivalent id on other platforms (they are the same title id on XB1 and X360 for example)

    const XblPresenceTitleRecord *pTitleRecord1 = NULL, *pTitleRecord2 = NULL, *pTitleRecord3 = NULL, *pTitleRecord4 = NULL, *pTitleRecord5 = NULL;
    const XblPresenceDeviceRecord *pDeviceRecord1 = NULL, *pDeviceRecord2 = NULL, *pDeviceRecord3 = NULL, *pDeviceRecord4 = NULL, *pDeviceRecord5 = NULL;
    bool bSameTitle = false;
    size_t uDeviceRecord;

    // if we have no records then nothing to do
    if (uNumDeviceRecords == 0)
    {
        return(false);
    }

    // rank by priorities
    for (uDeviceRecord = 0; (uDeviceRecord < uNumDeviceRecords) && (!bSameTitle); uDeviceRecord += 1)
    {
        size_t uTitleRecord;
        const XblPresenceDeviceRecord *pCurrentDeviceRecord = &pDeviceRecords[uDeviceRecord];

        for (uTitleRecord = 0; uTitleRecord < pCurrentDeviceRecord->titleRecordsCount; uTitleRecord += 1)
        {
            const XblPresenceTitleRecord *pCurrentTitleRecord = &pCurrentDeviceRecord->titleRecords[uTitleRecord];

            if (pCurrentTitleRecord->titleActive && (pCurrentDeviceRecord->deviceType == XblPresenceDeviceType::XboxOne) && (pCurrentTitleRecord->titleId == uTitleId))
            {
                pTitleRecord1 = pCurrentTitleRecord;
                pDeviceRecord1 = pCurrentDeviceRecord;
                bSameTitle = TRUE;
                break;
            }
            else if (pCurrentTitleRecord->titleActive && (pCurrentDeviceRecord->deviceType == XblPresenceDeviceType::XboxOne) && (pCurrentTitleRecord->titleId != USERLISTAPI_HOMEMENU_TITLEID))
            {
                pTitleRecord2 = pCurrentTitleRecord;
                pDeviceRecord2 = pCurrentDeviceRecord;
            }
            else if (pCurrentTitleRecord->titleActive && (pCurrentDeviceRecord->deviceType == XblPresenceDeviceType::XboxOne))
            {
                pTitleRecord3 = pCurrentTitleRecord;
                pDeviceRecord3 = pCurrentDeviceRecord;
            }
            else if (pCurrentTitleRecord->titleActive)
            {
                pTitleRecord4 = pCurrentTitleRecord;
                pDeviceRecord4 = pCurrentDeviceRecord;
            }
            else
            {
                pTitleRecord5 = pCurrentTitleRecord;
                pDeviceRecord5 = pCurrentDeviceRecord;
            }
        }
    }

    if ((pTitleRecord1 != NULL) && (pDeviceRecord1 != NULL))
    {
        *pTitleRecord = pTitleRecord1;
        *pDeviceRecord = pDeviceRecord1;
    }
    else if ((pTitleRecord2 != NULL) && (pDeviceRecord2 != NULL))
    {
        *pTitleRecord = pTitleRecord2;
        *pDeviceRecord = pDeviceRecord2;
    }
    else if ((pTitleRecord3 != NULL) && (pDeviceRecord3 != NULL))
    {
        *pTitleRecord = pTitleRecord3;
        *pDeviceRecord = pDeviceRecord3;
    }
    else if ((pTitleRecord4 != NULL) && (pDeviceRecord4 != NULL))
    {
        *pTitleRecord = pTitleRecord4;
        *pDeviceRecord = pDeviceRecord4;
    }
    else if ((pTitleRecord5 != NULL) && (pDeviceRecord5 != NULL))
    {
        *pTitleRecord = pTitleRecord2;
        *pDeviceRecord = pDeviceRecord2;
    }

    return((*pTitleRecord != NULL) && (*pDeviceRecord != NULL));
}

/*F*************************************************************************************/
/*!
    \Function   _UserListApiFilloutPresence

    \Description
        Fills out the presence data part of the callback events

    \Input *pRef        - module state
    \Input *pRequest    - request data
    \Input *pEventData  - the callback data we are filling out
    \Input uXuid        - the xbox userid of the user in the list

    \Output
        int32_t         - 0=success, negative=failure

    \Notes
        We assume that there is presence information for each user. Thus, we treat
        not having presence records as a failure.

    \Version 01/14/2020 (eesponda)
*/
/*************************************************************************************F*/
static int32_t _UserListApiFilloutPresence(UserListApiRefT *pRef, UserListApiRequestT *pRequest, UserApiUserDataT *pEventData, uint64_t uXuid)
{
    const XblPresenceDeviceRecord *pDeviceRecords;
    XblPresenceUserState eUserState;
    size_t uNumDeviceRecords = 0;
    uint32_t uTitleId;
    HRESULT hResult;

    const XblPresenceRecordHandle *pRecordHandle = _UserListApiFindPresenceHandle(pRef, pRequest, uXuid);
    if (pRecordHandle == NULL)
    {
        return(-1);
    }

    // set the flags for the callback
    pEventData->uUserDataMask |= (USERAPI_MASK_PRESENCE | USERAPI_MASK_RICH_PRESENCE);

    // get the title id, used for presence
    if (FAILED(hResult = XGameGetXboxTitleId(&uTitleId)))
    {
        NetPrintf(("userlistapigdk: [%p] failed to get title id (err=%s)\n", pRef, DirtyErrGetName(hResult)));
        uTitleId = 0;
    }

    // get the presence state of the user
    if (SUCCEEDED(hResult = XblPresenceRecordGetUserState(*pRecordHandle, &eUserState)))
    {
        if (eUserState == XblPresenceUserState::Unknown)
        {
            pEventData->Presence.ePresenceStatus = USERAPI_PRESENCE_UNKNOWN;
        }
        else if (eUserState == XblPresenceUserState::Online)
        {
            pEventData->Presence.ePresenceStatus = USERAPI_PRESENCE_ONLINE;
        }
        else if (eUserState == XblPresenceUserState::Away)
        {
            pEventData->Presence.ePresenceStatus = USERAPI_PRESENCE_AWAY;
        }
        else if (eUserState == XblPresenceUserState::Offline)
        {
            pEventData->Presence.ePresenceStatus = USERAPI_PRESENCE_OFFLINE;
        }
    }
    else
    {
        NetPrintf(("userlistapigdk: [%p] failed to get the presence record's user state (err=%s)\n", pRef, DirtyErrGetName(hResult)));
    }

    // get the device records and prioritize is using our standard algorithm to fill in the remaining data
    if (SUCCEEDED(hResult = XblPresenceRecordGetDeviceRecords(*pRecordHandle, &pDeviceRecords, &uNumDeviceRecords)))
    {
        const XblPresenceDeviceRecord *pDeviceRecord = NULL;
        const XblPresenceTitleRecord *pTitleRecord = NULL;
        if (_UserListApiFilterPresence(pDeviceRecords, uNumDeviceRecords, uTitleId, &pDeviceRecord, &pTitleRecord))
        {
            pEventData->Presence.bIsPlayingSameTitle = (uTitleId == pTitleRecord->titleId);
            ds_snzprintf(pEventData->Presence.strTitleId, sizeof(pEventData->Presence.strTitleId), "%u", pTitleRecord->titleId);
            ds_snzprintf(pEventData->Presence.strPlatform, sizeof(pEventData->Presence.strPlatform), "%i", pDeviceRecord->deviceType);
            ds_strnzcpy(pEventData->Presence.strTitleName, pTitleRecord->titleName, sizeof(pEventData->Presence.strTitleName));
            pEventData->Presence.pRawData = (void *)pTitleRecord;

            ds_strnzcpy(pEventData->RichPresence.strData, pTitleRecord->richPresenceString, sizeof(pEventData->RichPresence.strData));
            pEventData->RichPresence.pRawData = (void *)pTitleRecord;
        }
    }
    else
    {
        NetPrintf(("userlistapigdk: [%p] failed to get the presence record's device records (err=%s)\n", pRef, DirtyErrGetName(hResult)));
    }
    return(0);
}

/*F*************************************************************************************/
/*!
    \Function _UserListApiHandleRelationshipResponse

    \Description
        Handles the social relationship response from microsoft's services

    \Input *pRef        - module ref
    \Input *pRequest    - user index specific request we are handling

    \Output
        int32_t         - 0=success and can clear request data, 1=pending

    \Version  01/10/2020 (eesponda)
*/
/*************************************************************************************F*/
static int32_t _UserListApiHandleRelationshipResponse(UserListApiRefT *pRef, UserListApiRequestT *pRequest)
{
    const XblSocialRelationship *pRelationships;
    size_t uNumRelationships, uRelationship;
    bool bHasNext;
    HRESULT hResult;

    if (pRequest->pResult == NULL)
    {
        // pull the profiles if enabled
        if (pRequest->uTypeMask & USERLISTAPI_MASK_PROFILE)
        {
            _UserListApiHandleProfileResponse(pRef, pRequest);
        }
        // pull the presence if eanbled
        if (pRequest->uTypeMask & USERLISTAPI_MASK_PRESENCE)
        {
            _UserListApiHandlePresenceResponse(pRef, pRequest);
        }

        // retrieve the result from the async operation
        if (FAILED(hResult = XblSocialGetSocialRelationshipsResult(&pRequest->AsyncBlock, &pRequest->pResult)))
        {
            NetPrintf(("userlistapigdk: [%p] failed to get the social relationship result (err=%s)\n", pRef, DirtyErrGetName(hResult)));
            _UserListApiHandleListError(pRef, pRequest, hResult);
            return(0);
        }
    }
    else
    {
        // close handle from previous page
        XblSocialRelationshipResultCloseHandle(pRequest->pResult);
        pRequest->pResult = NULL;

        // retrieve the result from the next page of the async operation
        if (FAILED(hResult = XblSocialRelationshipResultGetNextResult(&pRequest->AsyncBlock, &pRequest->pResult)))
        {
            NetPrintf(("userlistapigdk: [%p] failed to get the social relationship next page result (err=%s)\n", pRef, DirtyErrGetName(hResult)));
            _UserListApiHandleListError(pRef, pRequest, hResult);
            return(0);
        }
    }

    // pull the relationships out of the result
    if (FAILED(hResult = XblSocialRelationshipResultGetRelationships(pRequest->pResult, &pRelationships, &uNumRelationships)))
    {
        NetPrintf(("userlistapigdk: [%p] failed to get social relationships from the result (err=%s)\n", pRef, DirtyErrGetName(hResult)));
        _UserListApiHandleListError(pRef, pRequest, hResult);
        return(0);
    }

    // loop through the relationships to fire the callback
    for (uRelationship = 0; uRelationship < uNumRelationships; uRelationship += 1)
    {
        UserListApiEventDataT EventData;
        ds_memclr(&EventData, sizeof(EventData));

        const XblSocialRelationship *pRelationship = &pRelationships[uRelationship];

        // fill in the information
        DirtyUserFromNativeUser(&EventData.UserData.DirtyUser, &pRelationship->xboxUserId);
        DirtyUserFromNativeUser(&EventData.UserData.ExtendedUserData.DirtyUser, &pRelationship->xboxUserId);
        EventData.UserData.bIsMutualFriend = pRelationship->isFollowingCaller;
        EventData.UserData.bIsFavorite = pRelationship->isFavorite;
        EventData.UserData.eRequestType = pRequest->eType;
        EventData.UserData.uUserIndex = pRequest->uUserIndex;
        EventData.UserData.pUserListFilters = &pRequest->Filter;
        EventData.UserData.pRawData = (void *)pRelationship;
        // fill out the profile extended data if enabled
        if (pRequest->uTypeMask & USERLISTAPI_MASK_PROFILE)
        {
            EventData.UserData.iUserProfileError |= _UserListApiFilloutProfile(pRef, pRequest, &EventData.UserData.ExtendedUserData, pRelationship->xboxUserId);
        }
        // fill out the presence extended data if either are enabled
        if ((pRequest->uTypeMask & USERLISTAPI_MASK_PRESENCE) || (pRequest->uTypeMask & USERLISTAPI_MASK_RICH_PRESENCE))
        {
            EventData.UserData.iUserProfileError |= _UserListApiFilloutPresence(pRef, pRequest, &EventData.UserData.ExtendedUserData, pRelationship->xboxUserId);
        }

        // fire the callback
        pRequest->pNotifyCb(pRef, TYPE_USER_DATA, &EventData, pRequest->pUserData);
    }

    // check for more results
    if (FAILED(hResult = XblSocialRelationshipResultHasNext(pRequest->pResult, &bHasNext)))
    {
        NetPrintf(("userlistapigdk: [%p] failed to check for more results (err=%s)\n", pRef, DirtyErrGetName(hResult)));
        _UserListApiHandleListError(pRef, pRequest, hResult);
        return(0);
    }

    // if we have more results try to get them
    if (bHasNext)
    {
        if (FAILED(hResult = XblSocialRelationshipResultGetNextAsync(pRequest->pXblContext, pRequest->pResult, 0, &pRequest->AsyncBlock)))
        {
            NetPrintf(("userlistapigdk: [%p] failed to get the page of results (err=%s)\n", pRef, DirtyErrGetName(hResult)));
            _UserListApiHandleListError(pRef, pRequest, hResult);
            return(0);
        }
        return(1);
    }
    // otherwise send the end of the list callback
    else
    {
        UserListApiEventDataT EventData;
        size_t uTotalCount;
        ds_memclr(&EventData, sizeof(EventData));

        // get the total count
        if (FAILED(hResult = XblSocialRelationshipResultGetTotalCount(pRequest->pResult, &uTotalCount)))
        {
            NetPrintf(("userlistapigdk: [%p] failed to get result total count (err=%s)\n", pRef, DirtyErrGetName(hResult)));
            _UserListApiHandleListError(pRef, pRequest, hResult);
            return(0);
        }

        // fill in the information
        EventData.ListEndData.eRequestType = pRequest->eType;
        EventData.ListEndData.pUserListFilters = &pRequest->Filter;
        EventData.ListEndData.uUserIndex = pRequest->uUserIndex;
        EventData.ListEndData.iLimit = pRequest->iLimit;
        EventData.ListEndData.iOffset = pRequest->iOffset;
        EventData.ListEndData.iTotalFriendCount = (signed)uTotalCount;
        EventData.ListEndData.uTypeMask = pRequest->uTypeMask;

        // fire the callback
        pRequest->pNotifyCb(pRef, TYPE_LIST_END, &EventData, pRequest->pUserData);
        return(0);
    }
}

/*F*************************************************************************************/
/*!
    \Function _UserListApiHandleMuteResponse

    \Description
        Handles the privacy mute response from microsoft's services

    \Input *pRef        - module ref
    \Input *pRequest    - user index specific request we are handling

    \Output
        int32_t         - 0=success and can clear request data, 1=pending

    \Version  01/10/2020 (eesponda)
*/
/*************************************************************************************F*/
static int32_t _UserListApiHandleMuteResponse(UserListApiRefT *pRef, UserListApiRequestT *pRequest)
{
    HRESULT hResult;
    UserListApiEventDataT EventData;
    size_t uIndex, uResultCount;
    uint64_t *pXuid;

    // get the number of results
    if (FAILED(hResult = XblPrivacyGetMuteListResultCount(&pRequest->AsyncBlock, &uResultCount)))
    {
        NetPrintf(("userlistapigdk: [%p] failed to get the mute list count (err=%s)\n", pRef, DirtyErrGetName(hResult)));
        _UserListApiHandleListError(pRef, pRequest, hResult);
        return(0);
    }
    // allocate a buffer for the result
    if ((pXuid = (uint64_t *)DirtyMemAlloc(sizeof(*pXuid) * (signed)uResultCount, USERLISTAPI_MEMID, pRef->iMemGroup, pRef->pMemGroupUserData)) == NULL)
    {
        NetPrintf(("userlistapigdk: [%p] failed to allocate memory for the result\n", pRef));
        _UserListApiHandleListError(pRef, pRequest, S_FALSE);
        return(0);
    }
    // get the results
    if (FAILED(hResult = XblPrivacyGetMuteListResult(&pRequest->AsyncBlock, uResultCount, pXuid)))
    {
        NetPrintf(("userlistapigdk: [%p] failed to get the mute list result (err=%s)\n", pRef, DirtyErrGetName(hResult)));
        _UserListApiHandleListError(pRef, pRequest, hResult);
        return(0);
    }

    // loop through the response
    for (uIndex = 0; uIndex < uResultCount; uIndex += 1)
    {
        // fill only the basic request and user information and fire the callback
        ds_memclr(&EventData, sizeof(EventData));
        EventData.UserData.eRequestType = pRequest->eType;
        EventData.UserData.uUserIndex = pRequest->uUserIndex;

        DirtyUserFromNativeUser(&EventData.UserData.DirtyUser, &pXuid[uIndex]);
        EventData.UserData.pRawData = (void *)&pXuid[uIndex];
        pRequest->pNotifyCb(pRef, TYPE_USER_DATA, &EventData, pRequest->pUserData);
    }

    // fill in the information
    ds_memclr(&EventData, sizeof(EventData));
    EventData.ListEndData.eRequestType = pRequest->eType;
    EventData.ListEndData.pUserListFilters = &pRequest->Filter;
    EventData.ListEndData.uUserIndex = pRequest->uUserIndex;
    EventData.ListEndData.iLimit = pRequest->iLimit;
    EventData.ListEndData.iOffset = pRequest->iOffset;
    EventData.ListEndData.iTotalFriendCount = (signed)uResultCount;

    // fire the callback
    pRequest->pNotifyCb(pRef, TYPE_LIST_END, &EventData, pRequest->pUserData);

    DirtyMemFree(pXuid, USERLISTAPI_MEMID, pRef->iMemGroup, pRef->pMemGroupUserData);
    return(0);
}

/*F*************************************************************************************/
/*!
    \Function _UserListApiHandleAvoidResponse

    \Description
        Handles the privacy avoid response from microsoft's services

    \Input *pRef        - module ref
    \Input *pRequest    - user index specific request we are handling

    \Output
        int32_t         - 0=success and can clear request data, 1=pending

    \Version  01/10/2020 (eesponda)
*/
/*************************************************************************************F*/
static int32_t _UserListApiHandleAvoidResponse(UserListApiRefT *pRef, UserListApiRequestT *pRequest)
{
    HRESULT hResult;
    size_t uIndex, uResultCount;
    uint64_t *pXuid;
    UserListApiEventDataT EventData;

    // get the number of results
    if (FAILED(hResult = XblPrivacyGetAvoidListResultCount(&pRequest->AsyncBlock, &uResultCount)))
    {
        NetPrintf(("userlistapigdk: [%p] failed to get the avoid list count (err=%s)\n", pRef, DirtyErrGetName(hResult)));
        _UserListApiHandleListError(pRef, pRequest, hResult);
        return(0);
    }
    // allocate a buffer for the result
    if ((pXuid = (uint64_t *)DirtyMemAlloc(sizeof(*pXuid) * (signed)uResultCount, USERLISTAPI_MEMID, pRef->iMemGroup, pRef->pMemGroupUserData)) == NULL)
    {
        NetPrintf(("userlistapigdk: [%p] failed to allocate memory for the result\n", pRef));
        _UserListApiHandleListError(pRef, pRequest, S_FALSE);
        return(0);
    }
    // get the results
    if (FAILED(hResult = XblPrivacyGetAvoidListResult(&pRequest->AsyncBlock, uResultCount, pXuid)))
    {
        NetPrintf(("userlistapigdk: [%p] failed to get the avoid list result (err=%s)\n", pRef, DirtyErrGetName(hResult)));
        _UserListApiHandleListError(pRef, pRequest, hResult);
        return(0);
    }

    // loop through the response
    for (uIndex = 0; uIndex < uResultCount; uIndex += 1)
    {
        // fill only the basic request and user information and fire the callback
        ds_memclr(&EventData, sizeof(EventData));
        EventData.UserData.eRequestType = pRequest->eType;
        EventData.UserData.uUserIndex = pRequest->uUserIndex;

        DirtyUserFromNativeUser(&EventData.UserData.DirtyUser, &pXuid[uIndex]);
        EventData.UserData.pRawData = (void *)&pXuid[uIndex];
        pRequest->pNotifyCb(pRef, TYPE_USER_DATA, &EventData, pRequest->pUserData);
    }

    // fill in the information
    ds_memclr(&EventData, sizeof(EventData));
    EventData.ListEndData.eRequestType = pRequest->eType;
    EventData.ListEndData.pUserListFilters = &pRequest->Filter;
    EventData.ListEndData.uUserIndex = pRequest->uUserIndex;
    EventData.ListEndData.iLimit = pRequest->iLimit;
    EventData.ListEndData.iOffset = pRequest->iOffset;
    EventData.ListEndData.iTotalFriendCount = (signed)uResultCount;

    // fire the callback
    pRequest->pNotifyCb(pRef, TYPE_LIST_END, &EventData, pRequest->pUserData);

    DirtyMemFree(pXuid, USERLISTAPI_MEMID, pRef->iMemGroup, pRef->pMemGroupUserData);
    return(0);
}

/*F*************************************************************************************/
/*!
    \Function   _UserListApiCleanupRequestData

    \Description
        Cleans up the necessary resources that are part of the request data

    \Version 01/14/2020 (eesponda)
*/
/*************************************************************************************F*/
static void _UserListApiCleanupRequestData(UserListApiRefT *pRef, UserListApiRequestT *pRequest)
{
    int32_t iPresenceRecord;

    if (pRequest->pXblContext != NULL)
    {
        XblContextCloseHandle(pRequest->pXblContext);
    }
    if (pRequest->pProfiles != NULL)
    {
        DirtyMemFree(pRequest->pProfiles, USERLISTAPI_MEMID, pRef->iMemGroup, pRef->pMemGroupUserData);
    }
    for (iPresenceRecord = 0; iPresenceRecord < pRequest->iNumPresenceRecords; iPresenceRecord += 1)
    {
        XblPresenceRecordCloseHandle(pRequest->pPresenceRecords[iPresenceRecord]);
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

    \Version 01/10/2020 (eesponda)
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
        NetPrintf(("userlistapigdk: could not allocate module state\n"));
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

    \Version 01/10/2020 (eesponda)
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

    \Version 01/10/2020 (eesponda)
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

    \Version 01/10/2020 (eesponda)
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

    \Version 01/10/2020 (eesponda)
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

    \Version  01/10/2020 (eesponda)
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
        if (XAsyncGetStatus(&pRequest->AsyncBlock, false) == E_PENDING)
        {
            continue;
        }
        if ((pRequest->uTypeMask & USERLISTAPI_MASK_PROFILE) && (XAsyncGetStatus(&pRequest->ProfileBlock, false) == E_PENDING))
        {
            continue;
        }
        if ((pRequest->uTypeMask & USERLISTAPI_MASK_PRESENCE) && (XAsyncGetStatus(&pRequest->PresenceBlock, false) == E_PENDING))
        {
            continue;
        }

        // handle the response
        if (pRequest->eType == USERLISTAPI_TYPE_FRIENDS)
        {
            iResult = _UserListApiHandleRelationshipResponse(pRef, pRequest);
        }
        else if (pRequest->eType == USERLISTAPI_TYPE_MUTED)
        {
            iResult = _UserListApiHandleMuteResponse(pRef, pRequest);
        }
        else if (pRequest->eType == USERLISTAPI_TYPE_BLOCKED)
        {
            iResult = _UserListApiHandleAvoidResponse(pRef, pRequest);
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

    \Version 01/10/2020 (eesponda)
*/
/*************************************************************************************F*/
int32_t UserListApiStatus(UserListApiRefT *pRef, int32_t iSelect, void *pBuf, int32_t iBufSize)
{
    NetPrintf(("userlistapigdk: [%p] unhandled status selector '%C'\n", pRef, iSelect));
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

    \Version 01/10/2020 (eesponda)
*/
/*************************************************************************************F*/
int32_t UserListApiControl(UserListApiRefT *pRef, int32_t iControl, int32_t iValue, int32_t iValue2, void *pValue)
{
    NetPrintf(("userlistapigdk: [%p] unhandled control selector '%C'\n", pRef, iControl));
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
    \Input *pFilter     - xboxone specific filter set
    \Input *pNotifyCb   - callback function pointer
    \Input uTypeMask    - mask
    \Input *pUserData   - user data

    \Output
        int32_t         - zero=success; negative=error

    \Version 01/10/2020 (eesponda)
*/
/*************************************************************************************F*/
int32_t UserListApiGetListAsync(UserListApiRefT *pRef, uint32_t uUserIndex, UserListApiTypeE eType, int32_t iLimit, int32_t iOffset, UserListApiFiltersT *pFilter, UserListApiCallbackT *pNotifyCb, uint32_t uTypeMask, void *pUserData)
{
    XUserHandle pUser;
    HRESULT hResult;
    uint64_t uXuid;
    const UserListApiFiltersT DefaultFilter = { USERLISTAPI_LEVEL_FRIEND };
    XblSocialRelationshipFilter eSocialFilter;
    const char *pSocialGroup;
    UserListApiRequestT *pRequest = &pRef->aRequests[uUserIndex];

    // set the social filters and groups based on the filters
    if ((pFilter != NULL) && (pFilter->eLevel == USERLISTAPI_LEVEL_FAVORITE))
    {
        eSocialFilter = XblSocialRelationshipFilter::Favorite;
        pSocialGroup = "Favorite";
    }
    else
    {
        eSocialFilter = XblSocialRelationshipFilter::All;
        pSocialGroup = "People";
    }

    // request are not supported for guests
    if (NetConnStatus('gust', uUserIndex, NULL, 0) != 0)
    {
        NetPrintf(("userlistapigdk: [%p] cannot perform operation as a guest user at index %u\n", pRef, uUserIndex));
        return(USERLISTAPI_ERROR_GUEST);
    }

    // attempt to pull out user
    if (NetConnStatus('xusr', uUserIndex, &pUser, sizeof(pUser)) < 0)
    {
        NetPrintf(("userlistapigdk: [%p] could not receive user handle at index %u\n", pRef, uUserIndex));
        return(USERLISTAPI_ERROR_NO_USER);
    }
    else if (NetConnStatus('xuid', uUserIndex, &uXuid, sizeof(uXuid)) < 0)
    {
        NetPrintf(("userlistapigdk: [%p] could not retrieve the user xuid at index %u\n", pRef, uUserIndex));
        return(USERLISTAPI_ERROR_NO_USER);
    }

    // make sure the parameters are valid
    if (pNotifyCb == NULL)
    {
        NetPrintf(("userlistapigdk: [%p] async request requires a callback\n", pRef));
        return(USERLISTAPI_ERROR_BAD_PARAM);
    }

    // check if we are still in-progress for a different request
    if (pRequest->pNotifyCb != NULL)
    {
        NetPrintf(("userlistapigdk: [%p] request for user index %u is still in progress, another request cannot be started\n", pRef, uUserIndex));
        return(USERLISTAPI_ERROR_INPROGRESS);
    }

    // create a xbox live context if one doesn't exist
    if (FAILED(hResult = XblContextCreateHandle(pUser, &pRequest->pXblContext)))
    {
        NetPrintf(("userlistapigdk: [%p] failed to create xbox live context (err=%s)\n", pRef, DirtyErrGetName(hResult)));
        return(USERLISTAPI_ERROR_UNKNOWN);
    }

    if (eType == USERLISTAPI_TYPE_FRIENDS)
    {
        // get the social relationships
        if (FAILED(hResult = XblSocialGetSocialRelationshipsAsync(pRequest->pXblContext, uXuid, eSocialFilter, iOffset, iLimit, &pRequest->AsyncBlock)))
        {
            NetPrintf(("userlistapigdk: [%p] failed to get social relationships (err=%s)\n", pRef, DirtyErrGetName(hResult)));
            _UserListApiCleanupRequestData(pRef, pRequest);
            return(USERLISTAPI_ERROR_UNKNOWN);
        }

        if (uTypeMask & USERLISTAPI_MASK_PROFILE)
        {
            if (FAILED(hResult = XblProfileGetUserProfilesForSocialGroupAsync(pRequest->pXblContext, pSocialGroup, &pRequest->ProfileBlock)))
            {
                NetPrintf(("userlistapigdk: [%p] failed to get profiles for the social group (err=%s)\n", pRef, DirtyErrGetName(hResult)));
                _UserListApiCleanupRequestData(pRef, pRequest);
                return(USERLISTAPI_ERROR_UNKNOWN);
            }
        }
        if ((uTypeMask & USERLISTAPI_MASK_PRESENCE) || (uTypeMask & USERLISTAPI_MASK_RICH_PRESENCE))
        {
            if (FAILED(hResult = XblPresenceGetPresenceForSocialGroupAsync(pRequest->pXblContext, pSocialGroup, NULL, NULL, &pRequest->PresenceBlock)))
            {
                NetPrintf(("userlistapigdk: [%p] failed to get presence for the social group (err=%s)\n", pRef, DirtyErrGetName(hResult)));
                _UserListApiCleanupRequestData(pRef, pRequest);
                return(USERLISTAPI_ERROR_UNKNOWN);
            }
        }
    }
    else if (eType == USERLISTAPI_TYPE_MUTED)
    {
        // get the mute list
        if (FAILED(hResult = XblPrivacyGetMuteListAsync(pRequest->pXblContext, &pRequest->AsyncBlock)))
        {
            NetPrintf(("userlistapigdk: [%p] failed to get mute list (err=%s)\n", pRef, DirtyErrGetName(hResult)));
            _UserListApiCleanupRequestData(pRef, pRequest);
            return(USERLISTAPI_ERROR_UNKNOWN);
        }
    }
    else if (eType == USERLISTAPI_TYPE_BLOCKED)
    {
        // get the avoid list
        if (FAILED(hResult = XblPrivacyGetAvoidListAsync(pRequest->pXblContext, &pRequest->AsyncBlock)))
        {
            NetPrintf(("userlistapigdk: [%p] failed to get avoid (block) list (err=%s)\n", pRef, DirtyErrGetName(hResult)));
            _UserListApiCleanupRequestData(pRef, pRequest);
            return(USERLISTAPI_ERROR_UNKNOWN);
        }
    }
    else
    {
        NetPrintf(("userlistapigdk: [%p] type %d is not supported on this platform\n", pRef, eType));
        _UserListApiCleanupRequestData(pRef, pRequest);
        return(USERLISTAPI_ERROR_UNSUPPORTED);
    }
    pRequest->uUserIndex = uUserIndex;
    pRequest->eType = eType;
    pRequest->iLimit = iLimit;
    pRequest->iOffset = iOffset;
    pRequest->Filter = (pFilter != NULL) ? *pFilter : DefaultFilter;
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

    \Version 01/10/2020 (eesponda)
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
        XAsyncCancel(&pRequest->AsyncBlock);
        XAsyncCancel(&pRequest->ProfileBlock);
        XAsyncCancel(&pRequest->PresenceBlock);
        _UserListApiCleanupRequestData(pRef, pRequest);
    }
}
