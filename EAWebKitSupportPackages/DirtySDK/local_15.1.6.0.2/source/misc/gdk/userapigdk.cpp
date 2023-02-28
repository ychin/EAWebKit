/*H*************************************************************************************/
/*!
    \File userapigdk.cpp

    \Description
        Implements the xboxone specific code to request UserProfiles, and parse the responses.

    \Copyright
        Copyright (c) Electronic Arts 2020

    \Version 01/15/2020 (eesponda)
*/
/*************************************************************************************H*/

/*** Include files *********************************************************************/

#include "DirtySDK/platform.h"

#include <string.h>
#include <stdlib.h>
#include <xsapi-c/services_c.h>
#include <XGame.h>

#include "DirtySDK/dirtysock/dirtyerr.h"
#include "DirtySDK/dirtysock/dirtylib.h"
#include "DirtySDK/dirtysock/dirtymem.h"
#include "DirtySDK/dirtysock/netconn.h"
#include "DirtySDK/misc/userapi.h"

/*** Defines ***************************************************************************/

#define USERAPI_HOMEMENU_TITLEID    (714681658) //!< title id for the home app

#define USERAPI_AVATARSIZE_SMALL    (64)        //!< width/height of small avatar
#define USERAPI_AVATARSIZE_MEDIUM   (208)       //!< width/height of medium avatar
#define USERAPI_AVATARSIZE_LARGE    (424)       //!< width/height of large avatar

/*** Macros ****************************************************************************/

/*** Type Definitions ******************************************************************/

typedef struct UserApiRequestDataT
{
    XblContextHandle pXblContext;       //!< xbox live context used for service calls
    XAsyncBlock ProfileBlock;           //!< async block used for profile(s) handling
    XAsyncBlock PresenceBlock;          //!< async block used for presence handling
    UserApiCallbackT *pCallback;        //!< callback fired after the get async completion
    UserApiPostCallbackT *pPostCallback;//!< callback fired after the set async completion
    void *pUserData;                    //!< callback user data
    uint32_t uUserDataMask;             //!< mask of what is being request (USERAPI_MASK_*)
    int32_t iLookupUsersLength;         //!< amount of users being request
    uint32_t uUserIndex;                //!< user index of the requestors
} UserApiRequestDataT;

struct UserApiRefT
{
    int32_t iMemGroup;                  //!< memgroup id
    void *pMemGroupUserData;            //!< memgroup userdata
    int32_t iAvatarSize;                //!< size of the avatar
    UserApiRequestDataT aRequests[NETCONN_MAXLOCALUSERS];   //!< request data
};

/*** Function Prototypes ***************************************************************/

/*** Variables *************************************************************************/

/*** Private functions *****************************************************************/

/*F*************************************************************************************/
/*!
    \Function _UserApiConvertError

    \Description
        Converts the hresult to the userapi error

    \Output
        UserApiEventErrorE  - the converted error

    \Version 01/15/2020 (eesponda)
*/
/*************************************************************************************F*/
static UserApiEventErrorE _UserApiConvertError(HRESULT hError)
{
    // set the error based on the condition that is mappable to one of our errors. non-mappable errors go to default
    switch (XblGetErrorCondition(hError))
    {
        case XblErrorCondition::NoError:
            return(USERAPI_ERROR_OK);
        case XblErrorCondition::Auth:
            return(USERAPI_ERROR_REQUEST_FORBIDDEN);
        case XblErrorCondition::Http404NotFound:
            return(USERAPI_ERROR_REQUEST_NOT_FOUND);
        case XblErrorCondition::HttpServiceTimeout:
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

    \Version 01/15/2020 (eesponda)
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

    \Output
        HRESULT         - result of pulling the profile response

    \Version 01/15/2020 (eesponda)
*/
/*************************************************************************************F*/
static HRESULT _UserApiFilloutProfile(UserApiRefT *pRef, UserApiRequestDataT *pRequest, UserApiEventDataT *pEventData)
{
    XblUserProfile Profile;
    HRESULT hResult;

    if (FAILED(hResult = XblProfileGetUserProfileResult(&pRequest->ProfileBlock, &Profile)))
    {
        NetPrintf(("userapigdk: [%p] failed to get user profile result (err=%s)\n", pRef, DirtyErrGetName(hResult)));
        return(hResult);
    }

    DirtyUserFromNativeUser(&pEventData->EventDetails.UserData.DirtyUser, &Profile.xboxUserId);
    pEventData->EventDetails.UserData.uUserDataMask |= USERAPI_MASK_PROFILE;
    ds_strnzcpy(pEventData->EventDetails.UserData.Profile.strGamertag, Profile.gamertag, sizeof(pEventData->EventDetails.UserData.Profile.strGamertag));
    ds_strnzcpy(pEventData->EventDetails.UserData.Profile.strDisplayName, Profile.gameDisplayName, sizeof(pEventData->EventDetails.UserData.Profile.strDisplayName));
    ds_snzprintf(pEventData->EventDetails.UserData.Profile.strAvatarUrl, sizeof(pEventData->EventDetails.UserData.Profile.strAvatarUrl), "%s&w=%d&h=%d",
        Profile.gameDisplayPictureResizeUri, pRef->iAvatarSize, pRef->iAvatarSize);
    pEventData->EventDetails.UserData.Profile.pRawData = (void *)&Profile;
    return(hResult);
}

/*F*************************************************************************************/
/*!
    \Function   _UserApiFilterPresence

    \Description
        Uses an algorithm to filter the various title and device presence records

    \Input *pDeviceRecords  - the presence on the many devices supported by microsoft
    \Input uNumDeviceRecords- the number of records
    \Input uTitleId         - the current title id
    \Input **pDeviceRecord  - [out] the device record that was choosen
    \Input **pTitleRecord   - [out] the title record that was choosen

    \Output
        bool                - true=successfully selected, false=unsuccessful in selection

    \Version 01/15/2020 (eesponda)
*/
/*************************************************************************************F*/
static bool _UserApiFilterPresence(const XblPresenceDeviceRecord *pDeviceRecords, size_t uNumDeviceRecords, uint32_t uTitleId, const XblPresenceDeviceRecord **pDeviceRecord, const XblPresenceTitleRecord **pTitleRecord)
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
            else if (pCurrentTitleRecord->titleActive && (pCurrentDeviceRecord->deviceType == XblPresenceDeviceType::XboxOne) && (pCurrentTitleRecord->titleId != USERAPI_HOMEMENU_TITLEID))
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
    \Function _UserApiFilloutPresence

    \Description
        Fillout the presence data for the callback response

    \Input *pRef        - module state
    \Input *pRequest    - request data
    \Input *pEventData  - the event data we are filling out

    \Output
        HRESULT         - result of pulling the presence response

    \Version 01/15/2020 (eesponda)
*/
/*************************************************************************************F*/
static HRESULT _UserApiFilloutPresence(UserApiRefT *pRef, UserApiRequestDataT *pRequest, UserApiEventDataT *pEventData)
{
    XblPresenceRecordHandle PresenceHandle;
    XblPresenceUserState eUserState;
    HRESULT hResult;
    const XblPresenceDeviceRecord *pDeviceRecords;
    size_t uNumDeviceRecords = 0;
    uint32_t uTitleId;
    uint64_t uXuid;

    if (FAILED(hResult = XblPresenceGetPresenceResult(&pRequest->PresenceBlock, &PresenceHandle)))
    {
        NetPrintf(("userapigdk: [%p] failed to get presence result (err=%s)\n", pRef, DirtyErrGetName(hResult)));
        return(hResult);
    }

    // set the callback flags
    pEventData->EventDetails.UserData.uUserDataMask |= (USERAPI_MASK_PRESENCE | USERAPI_MASK_RICH_PRESENCE);

    // set the user
    if (SUCCEEDED(hResult = XblPresenceRecordGetXuid(PresenceHandle, &uXuid)))
    {
        DirtyUserFromNativeUser(&pEventData->EventDetails.UserData.DirtyUser, &uXuid);
    }
    else
    {
        NetPrintf(("userapigdk: [%p] failed to get the xuid from the presence record (err=%s)\n", pRef, DirtyErrGetName(hResult)));
        XblPresenceRecordCloseHandle(PresenceHandle);
        return(hResult);
    }

    // get the title id, used for presence
    if (FAILED(hResult = XGameGetXboxTitleId(&uTitleId)))
    {
        NetPrintf(("userapigdk: [%p] failed to get title id (err=%s)\n", pRef, DirtyErrGetName(hResult)));
        XblPresenceRecordCloseHandle(PresenceHandle);
        return(hResult);
    }

    // get the presence state of the user
    if (SUCCEEDED(hResult = XblPresenceRecordGetUserState(PresenceHandle, &eUserState)))
    {
        if (eUserState == XblPresenceUserState::Unknown)
        {
            pEventData->EventDetails.UserData.Presence.ePresenceStatus = USERAPI_PRESENCE_UNKNOWN;
        }
        else if (eUserState == XblPresenceUserState::Online)
        {
            pEventData->EventDetails.UserData.Presence.ePresenceStatus = USERAPI_PRESENCE_ONLINE;
        }
        else if (eUserState == XblPresenceUserState::Away)
        {
            pEventData->EventDetails.UserData.Presence.ePresenceStatus = USERAPI_PRESENCE_AWAY;
        }
        else if (eUserState == XblPresenceUserState::Offline)
        {
            pEventData->EventDetails.UserData.Presence.ePresenceStatus = USERAPI_PRESENCE_OFFLINE;
        }
    }
    else
    {
        NetPrintf(("userapigdk: [%p] failed to get the presence record's user state (err=%s)\n", pRef, DirtyErrGetName(hResult)));
        XblPresenceRecordCloseHandle(PresenceHandle);
        return(hResult);
    }

    // get the device records and prioritize is using our standard algorithm to fill in the remaining data
    if (SUCCEEDED(hResult = XblPresenceRecordGetDeviceRecords(PresenceHandle, &pDeviceRecords, &uNumDeviceRecords)))
    {
        const XblPresenceDeviceRecord *pDeviceRecord = NULL;
        const XblPresenceTitleRecord *pTitleRecord = NULL;
        if (_UserApiFilterPresence(pDeviceRecords, uNumDeviceRecords, uTitleId, &pDeviceRecord, &pTitleRecord))
        {
            pEventData->EventDetails.UserData.Presence.bIsPlayingSameTitle = (uTitleId == pTitleRecord->titleId);
            ds_snzprintf(pEventData->EventDetails.UserData.Presence.strTitleId, sizeof(pEventData->EventDetails.UserData.Presence.strTitleId), "%u", pTitleRecord->titleId);
            ds_snzprintf(pEventData->EventDetails.UserData.Presence.strPlatform, sizeof(pEventData->EventDetails.UserData.Presence.strPlatform), "%i", pDeviceRecord->deviceType);
            ds_strnzcpy(pEventData->EventDetails.UserData.Presence.strTitleName, pTitleRecord->titleName, sizeof(pEventData->EventDetails.UserData.Presence.strTitleName));
            pEventData->EventDetails.UserData.Presence.pRawData = (void *)pTitleRecord;

            ds_strnzcpy(pEventData->EventDetails.UserData.RichPresence.strData, pTitleRecord->richPresenceString, sizeof(pEventData->EventDetails.UserData.RichPresence.strData));
            pEventData->EventDetails.UserData.RichPresence.pRawData = (void *)pTitleRecord;
        }
    }
    else
    {
        NetPrintf(("userapigdk: [%p] failed to get the presence record's device records (err=%s)\n", pRef, DirtyErrGetName(hResult)));
    }

    // close the handle
    XblPresenceRecordCloseHandle(PresenceHandle);

    return(hResult);
}

/*F*************************************************************************************/
/*!
    \Function _UserApiHandleProfiles

    \Description
        Handles the profiles request and fire the appropriate callbacks

    \Input *pRef        - module state
    \Input *pRequest    - request data

    \Version 01/15/2020 (eesponda)
*/
/*************************************************************************************F*/
static void _UserApiHandleProfiles(UserApiRefT *pRef, UserApiRequestDataT *pRequest)
{
    XblUserProfile *pProfiles;
    size_t uNumProfiles, uProfile;
    HRESULT hResult;

    if (FAILED(hResult = XblProfileGetUserProfilesResultCount(&pRequest->ProfileBlock, &uNumProfiles)))
    {
        NetPrintf(("userapigdk: [%p] failed to get user profiles result count (err=%s)\n", pRef, DirtyErrGetName(hResult)));
        _UserApiFireEndOfList(pRef, pRequest, 0, pRequest->iLookupUsersLength);
        return;
    }
    if (uNumProfiles == 0)
    {
        NetPrintf(("userapigdk: [%p] user profiles result did not return any profiles\n", pRef));
        _UserApiFireEndOfList(pRef, pRequest, 0, pRequest->iLookupUsersLength);
        return;
    }
    if ((pProfiles = (XblUserProfile *)DirtyMemAlloc(sizeof(*pProfiles) * (signed)uNumProfiles, USERAPI_MEMID, pRef->iMemGroup, pRef->pMemGroupUserData)) == NULL)
    {
        NetPrintf(("userapigdk: [%p] failed to allocate space for the user profiles result\n", pRef));
        _UserApiFireEndOfList(pRef, pRequest, 0, pRequest->iLookupUsersLength);
        return;
    }
    if (FAILED(hResult = XblProfileGetUserProfilesResult(&pRequest->ProfileBlock, uNumProfiles, pProfiles)))
    {
        NetPrintf(("userapigdk: [%p] failed to get user profiles result (err=%s)\n", pRef, DirtyErrGetName(hResult)));
        _UserApiFireEndOfList(pRef, pRequest, 0, pRequest->iLookupUsersLength);
        DirtyMemFree(pProfiles, USERAPI_MEMID, pRef->iMemGroup, pRef->pMemGroupUserData);
        return;
    }

    // loop through the profiles and send them back to the user
    for (uProfile = 0; uProfile < uNumProfiles; uProfile += 1)
    {
        UserApiEventDataT EventData;
        const XblUserProfile *pProfile = &pProfiles[uProfile];

        // fill out the information using the profile
        ds_memclr(&EventData, sizeof(EventData));
        EventData.eEventType = USERAPI_EVENT_DATA;
        EventData.uUserIndex = pRequest->uUserIndex;
        DirtyUserFromNativeUser(&EventData.EventDetails.UserData.DirtyUser, &pProfile->xboxUserId);
        EventData.EventDetails.UserData.uUserDataMask = USERAPI_MASK_PROFILE;
        ds_strnzcpy(EventData.EventDetails.UserData.Profile.strGamertag, pProfile->gamertag, sizeof(EventData.EventDetails.UserData.Profile.strGamertag));
        ds_strnzcpy(EventData.EventDetails.UserData.Profile.strDisplayName, pProfile->gameDisplayName, sizeof(EventData.EventDetails.UserData.Profile.strDisplayName));
        ds_strnzcpy(EventData.EventDetails.UserData.Profile.strAvatarUrl, pProfile->gameDisplayPictureResizeUri, sizeof(EventData.EventDetails.UserData.Profile.strAvatarUrl));
        EventData.EventDetails.UserData.Profile.pRawData = (void *)pProfile;

        // fire the callback
        pRequest->pCallback(pRef, &EventData, pRequest->pUserData);
    }

    // cleanup the allocated memory
    DirtyMemFree(pProfiles, USERAPI_MEMID, pRef->iMemGroup, pRef->pMemGroupUserData);

    // fire the end of callback
    _UserApiFireEndOfList(pRef, pRequest, (signed)uNumProfiles, 0);
}

/*F*************************************************************************************/
/*!
    \Function _UserApiHandleRequest

    \Description
        Handle the requests that are pending

    \Input *pRef        - module state
    \Input *pRequest    - request data

    \Version 01/15/2020 (eesponda)
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
        HRESULT hResult;

        // handle the data
        ds_memclr(&EventData, sizeof(EventData));
        EventData.eEventType = USERAPI_EVENT_DATA;
        EventData.uUserIndex = pRequest->uUserIndex;
        EventData.eError = USERAPI_ERROR_OK;

        if (pRequest->uUserDataMask & USERAPI_MASK_PROFILE)
        {
            if (FAILED(hResult = _UserApiFilloutProfile(pRef, pRequest, &EventData)))
            {
                EventData.eError = _UserApiConvertError(hResult);
            }
        }
        if ((pRequest->uUserDataMask & USERAPI_MASK_PRESENCE) || (pRequest->uUserDataMask & USERAPI_MASK_RICH_PRESENCE))
        {
            if (FAILED(hResult = _UserApiFilloutPresence(pRef, pRequest, &EventData)))
            {
                EventData.eError = _UserApiConvertError(hResult);
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

    \Version 01/15/2020 (eesponda)
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
        NetPrintf(("userapigdk: could not allocate module state\n"));
        return(NULL);
    }
    ds_memclr(pRef, sizeof(*pRef));
    pRef->iMemGroup = iMemGroup;
    pRef->pMemGroupUserData = pMemGroupUserData;
    pRef->iAvatarSize = USERAPI_AVATARSIZE_LARGE;

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

    \Version 01/15/2020 (eesponda)
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

    \Version 01/15/2020 (eesponda)
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
            'avsz' - Set which avatar size will be retrieved. iValue = 's' for small (64x64), 'm' for medium (208x208) and 'l' for big (424x424). The default value is 'l' on xboxone
        \endverbatim

    \Version 01/15/2020 (eesponda)
*/
/*************************************************************************************F*/
int32_t UserApiControl(UserApiRefT *pRef, int32_t iControl, int32_t iValue, int32_t iValue2, void *pValue)
{
    // set the avatar sizes
    if (iControl == 'avsz')
    {
        if (iValue == 's')
        {
            pRef->iAvatarSize = USERAPI_AVATARSIZE_SMALL;
            return(0);
        }
        else if (iValue == 'm')
        {
            pRef->iAvatarSize = USERAPI_AVATARSIZE_MEDIUM;
            return(0);
        }
        else if (iValue == 'l')
        {
            pRef->iAvatarSize = USERAPI_AVATARSIZE_LARGE;
            return(0);
        }
        else
        {
            NetPrintf(("userapigdk: [%p] unsupported avatar size %C\n", pRef, iValue));
        }
    }
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

    \Version 01/15/2020 (eesponda)
*/
/*************************************************************************************F*/
int32_t UserApiRequestProfilesAsync(UserApiRefT *pRef, uint32_t uUserIndex, DirtyUserT *pLookupUsers, int32_t iLookupUsersLength, UserApiCallbackT *pCallback, void *pUserData)
{
    XUserHandle pUser;
    uint64_t *pXuid;
    int32_t iLookupUser;
    HRESULT hResult;
    UserApiRequestDataT *pRequest = &pRef->aRequests[uUserIndex];

    // request are not supported for guests
    if (NetConnStatus('gust', uUserIndex, NULL, 0) != 0)
    {
        NetPrintf(("userapigdk: [%p] cannot perform operation as a guest user at index %u\n", pRef, uUserIndex));
        return(-1);
    }

    // attempt to pull out user
    if (NetConnStatus('xusr', uUserIndex, &pUser, sizeof(pUser)) < 0)
    {
        NetPrintf(("userapigdk: [%p] could not receive user handle at index %u\n", pRef, uUserIndex));
        return(-1);
    }

    // allocate temporary buffer for the request
    if ((pXuid = (uint64_t *)DirtyMemAlloc(sizeof(*pXuid) * iLookupUsersLength, USERAPI_MEMID, pRef->iMemGroup, pRef->pMemGroupUserData)) == NULL)
    {
        NetPrintf(("userapigdk: [%p] unable to allocate temporary buffer for profiles request\n", pRef));
        return(-1);
    }
    // fill in the request based on the inputs
    for (iLookupUser = 0; iLookupUser < iLookupUsersLength; iLookupUser += 1)
    {
        DirtyUserToNativeUser(&pXuid[iLookupUser], sizeof(*pXuid), &pLookupUsers[iLookupUser]);
    }

    // make sure the parameters are valid
    if (pCallback == NULL)
    {
        NetPrintf(("userapigdk: [%p] async request requires a callback\n", pRef));
        DirtyMemFree(pXuid, USERAPI_MEMID, pRef->iMemGroup, pRef->pMemGroupUserData);
        return(-1);
    }

    // check if we are still in-progress for a different request
    if ((pRequest->pCallback != NULL) || (pRequest->pPostCallback != NULL))
    {
        NetPrintf(("userapigdk: [%p] request for user index %u is still in progress, another request cannot be started\n", pRef, uUserIndex));
        DirtyMemFree(pXuid, USERAPI_MEMID, pRef->iMemGroup, pRef->pMemGroupUserData);
        return(-1);
    }

    // create a xbox live context if one doesn't exist
    if (FAILED(hResult = XblContextCreateHandle(pUser, &pRequest->pXblContext)))
    {
        NetPrintf(("userapigdk: [%p] failed to create xbox live context (err=%s)\n", pRef, DirtyErrGetName(hResult)));
        DirtyMemFree(pXuid, USERAPI_MEMID, pRef->iMemGroup, pRef->pMemGroupUserData);
        return(-1);
    }

    // start the async fetch of the profile
    if (SUCCEEDED(hResult = XblProfileGetUserProfilesAsync(pRequest->pXblContext, pXuid, iLookupUsersLength, &pRequest->ProfileBlock)))
    {
        pRequest->pCallback = pCallback;
        pRequest->pUserData = pUserData;
        pRequest->iLookupUsersLength = iLookupUsersLength;
        pRequest->uUserIndex = uUserIndex;
        pRequest->uUserDataMask = USERAPI_MASK_PROFILES;
        DirtyMemFree(pXuid, USERAPI_MEMID, pRef->iMemGroup, pRef->pMemGroupUserData);
        return(0);
    }
    else
    {
        NetPrintf(("userapigdk: [%p] failed to get profiles (err=%s)\n", pRef, DirtyErrGetName(hResult)));
        XblContextCloseHandle(pRequest->pXblContext);
        pRequest->pXblContext = NULL;
        DirtyMemFree(pXuid, USERAPI_MEMID, pRef->iMemGroup, pRef->pMemGroupUserData);
        return(-1);
    }
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

    \Version 01/15/2020 (eesponda)
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

    \Version 01/15/2020 (eesponda)
*/
/*************************************************************************************F*/
int32_t UserApiRequestPresenceAsync(UserApiRefT *pRef, uint32_t uUserIndex, DirtyUserT *pLookupUser, UserApiCallbackT *pCallback, uint32_t uUserDataMask, void *pUserData)
{
    XUserHandle pUser;
    uint64_t uXuid;
    HRESULT hResult;
    UserApiRequestDataT *pRequest = &pRef->aRequests[uUserIndex];

    // request are not supported for guests
    if (NetConnStatus('gust', uUserIndex, NULL, 0) != 0)
    {
        NetPrintf(("userapigdk: [%p] cannot perform operation as a guest user at index %u\n", pRef, uUserIndex));
        return(-1);
    }

    // attempt to pull out user
    if (NetConnStatus('xusr', uUserIndex, &pUser, sizeof(pUser)) < 0)
    {
        NetPrintf(("userapigdk: [%p] could not receive user handle at index %u\n", pRef, uUserIndex));
        return(-1);
    }
    DirtyUserToNativeUser(&uXuid, sizeof(uXuid), pLookupUser);

    // make sure the parameters are valid
    if (pCallback == NULL)
    {
        NetPrintf(("userapigdk: [%p] async request requires a callback\n", pRef));
        return(-1);
    }
    if ((uUserDataMask == 0) || (uUserDataMask & USERAPI_MASK_PROFILES))
    {
        NetPrintf(("userapigdk: [%p] invalid mask used (mask=0x%08x)\n", pRef, uUserDataMask));
    }

    // check if we are still in-progress for a different request
    if ((pRequest->pCallback != NULL) || (pRequest->pPostCallback != NULL))
    {
        NetPrintf(("userapigdk: [%p] request for user index %u is still in progress, another request cannot be started\n", pRef, uUserIndex));
        return(-1);
    }

    // create a xbox live context if one doesn't exist
    if (FAILED(hResult = XblContextCreateHandle(pUser, &pRequest->pXblContext)))
    {
        NetPrintf(("userapigdk: [%p] failed to create xbox live context (err=%s)\n", pRef, DirtyErrGetName(hResult)));
        return(-1);
    }

    if ((uUserDataMask & USERAPI_MASK_PRESENCE) || (uUserDataMask & USERAPI_MASK_RICH_PRESENCE))
    {
        // start the async fetch of the presence
        if (FAILED(hResult = XblPresenceGetPresenceAsync(pRequest->pXblContext, uXuid, &pRequest->PresenceBlock)))
        {
            NetPrintf(("userapigdk: [%p] failed to get profile (err=%s)\n", pRef, DirtyErrGetName(hResult)));
            XblContextCloseHandle(pRequest->pXblContext);
            pRequest->pXblContext = NULL;
            return(-1);
        }
    }
    if (uUserDataMask & USERAPI_MASK_PROFILE)
    {
        // start the async fetch of the profile
        if (FAILED(hResult = XblProfileGetUserProfileAsync(pRequest->pXblContext, uXuid, &pRequest->ProfileBlock)))
        {
            NetPrintf(("userapigdk: [%p] failed to get profile (err=%s)\n", pRef, DirtyErrGetName(hResult)));
            XblContextCloseHandle(pRequest->pXblContext);
            pRequest->pXblContext = NULL;
            return(-1);
        }
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

    \Version 01/15/2020 (eesponda)
*/
/*************************************************************************************F*/
int32_t UserApiRequestRichPresenceAsync(UserApiRefT *pRef, uint32_t uUserIndex, DirtyUserT *pLookupUser, UserApiCallbackT *pCallback, uint32_t uUserDataMask, void *pUserData)
{
    // presence and rich presence belong to the same request
    return(UserApiRequestPresenceAsync(pRef, uUserIndex, pLookupUser, pCallback, uUserDataMask, pUserData));
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

    \Version 01/15/2020 (eesponda)
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

    \Version 01/15/2020 (eesponda)
*/
/*************************************************************************************F*/
int32_t UserApiPostRichPresenceAsync(UserApiRefT *pRef, uint32_t uUserIndex, UserApiRichPresenceT *pData, UserApiPostCallbackT *pCallback, void *pUserData)
{
    XUserHandle pUser;
    HRESULT hResult;
    XblPresenceRichPresenceIds PresenceIds;
    const char *pScid;
    UserApiRequestDataT *pRequest = &pRef->aRequests[uUserIndex];

    // request are not supported for guests
    if (NetConnStatus('gust', uUserIndex, NULL, 0) != 0)
    {
        NetPrintf(("userapigdk: [%p] cannot perform operation as a guest user at index %u\n", pRef, uUserIndex));
        return(-1);
    }

    // attempt to pull out user
    if (NetConnStatus('xusr', uUserIndex, &pUser, sizeof(pUser)) < 0)
    {
        NetPrintf(("userapigdk: [%p] could not receive user handle at index %u\n", pRef, uUserIndex));
        return(-1);
    }

    // make sure the parameters are valid
    if (pCallback == NULL)
    {
        NetPrintf(("userapigdk: [%p] async request requires a callback\n", pRef));
        return(-1);
    }
    // check if we are still in-progress for a different request
    if ((pRequest->pPostCallback != NULL) || (pRequest->pCallback != NULL))
    {
        NetPrintf(("userapigdk: [%p] request for user index %u is still in progress, another request cannot be started\n", pRef, uUserIndex));
        return(-1);
    }

    // create a xbox live context if one doesn't exist
    if (FAILED(hResult = XblContextCreateHandle(pUser, &pRequest->pXblContext)))
    {
        NetPrintf(("userapigdk: [%p] failed to create xbox live context (err=%s)\n", pRef, DirtyErrGetName(hResult)));
        return(-1);
    }

    // fill out the presence infornmation
    ds_memclr(&PresenceIds, sizeof(PresenceIds));
    if (SUCCEEDED(hResult = XblGetScid(&pScid)))
    {
        ds_strnzcpy(PresenceIds.scid, pScid, sizeof(PresenceIds.scid));
    }
    else
    {
        NetPrintf(("userapigdk: [%p] failed to get scid (err=%s)\n", pRef, DirtyErrGetName(hResult)));
    }
    PresenceIds.presenceId = pData->strData;

    // kick off the set presence request
    if (FAILED(hResult = XblPresenceSetPresenceAsync(pRequest->pXblContext, true, &PresenceIds, &pRequest->PresenceBlock)))
    {
        NetPrintf(("userapigdk: [%p] failed to start request for setting presence information (err=%s)\n", pRef, DirtyErrGetName(hResult)));
        XblContextCloseHandle(pRequest->pXblContext);
        pRequest->pXblContext = NULL;
        return(-1);
    }
    pRequest->pPostCallback = pCallback;
    pRequest->pUserData = pUserData;
    pRequest->uUserIndex = uUserIndex;
    return(0);
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

    \Version 01/15/2020 (eesponda)
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

    \Version 01/15/2020 (eesponda)
*/
/*************************************************************************************F*/
void UserApiUpdate(UserApiRefT *pRef)
{
    int32_t iUserIndex;

    for (iUserIndex = 0; iUserIndex < NETCONN_MAXLOCALUSERS; iUserIndex += 1)
    {
        UserApiRequestDataT *pRequest = &pRef->aRequests[iUserIndex];
        if ((pRequest->pCallback == NULL) && (pRequest->pPostCallback == NULL))
        {
            continue;
        }

        // the get callback is registered for get requests
        if (pRequest->pCallback != NULL)
        {
            if (((pRequest->uUserDataMask & USERAPI_MASK_PROFILE) || (pRequest->uUserDataMask & USERAPI_MASK_PROFILES)) && (XAsyncGetStatus(&pRequest->ProfileBlock, false) == E_PENDING))
            {
                continue;
            }
            if ((pRequest->uUserDataMask & USERAPI_MASK_PRESENCE) && (XAsyncGetStatus(&pRequest->PresenceBlock, false) == E_PENDING))
            {
                continue;
            }
            _UserApiHandleRequest(pRef, pRequest);
        }
        // otherwise check if it is a post request
        else if (pRequest->pPostCallback != NULL)
        {
            UserApiPostResponseT Response;
            HRESULT hResult;

            // the only set request we support is presence, we know what async block to check
            if ((hResult = XAsyncGetStatus(&pRequest->PresenceBlock, false)) == E_PENDING)
            {
                continue;
            }
            ds_memclr(&Response, sizeof(Response));
            if (FAILED(hResult))
            {
                NetPrintf(("userapigdk: [%p] setting rich presence ended in failure (err=%s)\n", pRef, DirtyErrGetName(hResult)));
                Response.eError = USERAPI_ERROR_REQUEST_FAILED;
                Response.pMessage = DirtyErrGetName(hResult);
            }
            Response.uUserIndex = pRequest->uUserIndex;
            pRequest->pPostCallback(pRef, &Response, pRequest->pUserData);
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

    \Version 01/15/2020 (eesponda)
*/
/*************************************************************************************F*/
int32_t UserApiCancel(UserApiRefT *pRef, uint32_t uUserIndex)
{
    UserApiRequestDataT *pRequest = &pRef->aRequests[uUserIndex];
    if ((pRequest->pCallback == NULL) && (pRequest->pPostCallback == NULL))
    {
        return(0);
    }

    // cancel the pending async operations
    if (pRequest->pCallback != NULL)
    {
        if ((pRequest->uUserDataMask & USERAPI_MASK_PROFILE) || (pRequest->uUserDataMask & USERAPI_MASK_PROFILES))
        {
            XAsyncCancel(&pRequest->ProfileBlock);
        }
        if ((pRequest->uUserDataMask & USERAPI_MASK_PRESENCE) || (pRequest->uUserDataMask & USERAPI_MASK_RICH_PRESENCE))
        {
            XAsyncCancel(&pRequest->PresenceBlock);
        }
    }
    else if (pRequest->pPostCallback != NULL)
    {
        XAsyncCancel(&pRequest->PresenceBlock);
    }

    // clear the request data
    ds_memclr(pRequest, sizeof(*pRequest));
    return(0);
}
