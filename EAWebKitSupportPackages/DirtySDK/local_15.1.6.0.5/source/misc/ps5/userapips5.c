/*H*************************************************************************************************/
/*!

    \File    userapips5.c

    \Description
        Implements the ps5 specific code to request UserProfiles, and parse the responses.

    \Copyright
        Copyright (c) Electronic Arts 2020.    ALL RIGHTS RESERVED.

    \Version 01/28/2020 (tcho) First Version
*/
/*************************************************************************************************H*/

/*** Include files ********************************************************************************/
#include <np.h>
#include <string.h>
#include <scetypes.h>

#include "DirtySDK/dirtylang.h"
#include "DirtySDK/dirtysock/dirtyerr.h"
#include "DirtySDK/dirtysock/dirtymem.h"
#include "DirtySDK/dirtysock/dirtylib.h"
#include "DirtySDK/dirtysock/netconn.h"
#include "DirtySDK/util/jsonparse.h"
#include "DirtySDK/dirtysock/ps5/dirtywebapi2ps5.h"
#include "DirtySDK/misc/userapi.h"

/*** Defines **************************************************************************************/
#define USERAPI_API_GROUP                     "userProfile"
#define USERAPI_ACTIVITY_API_GROUP            "activities"
#define USERAPI_PATH_PROFILES                 "/v1/users/profiles"
#define USERAPI_PATH_PRESENCE                 "/v1/users/basicPresences"
#define USERAPI_PATH_ACTIVITY                 "/v1/users/activities"
#define USERAPI_WEBAPI_BUFFER                 (51200)
#define USERAPI_PATH_LEN_MAX                  (2048)
#define USERAPI_MAX_LOOKUPS_PER_REQUEST       (100)
#define USERAPI_MAX_QUEUED_NOTIFICATIONS      (50)
#define USERAPI_AVATAR_LIST_SIZE              (4)
#define USERAPI_ONE_ENTRY_BUFFER_SIZE         (1024)

/*** Type Definitions *****************************************************************************/

typedef enum UserApiAvatarSizeE
{
    USERAPI_AVATAR_S,
    USERAPI_AVATAR_M,
    USERAPI_AVATAR_L,
    USERAPI_AVATAR_XL
} UserApiAvatarSizeE;

typedef enum UserApiRequestTStateE
{
    USERAPI_IDLE,
    USERAPI_GETLIST,
    USERAPI_GETLIST_WAIT
} UserApiRequestTStateE;

typedef struct UserApiNotificationT
{
    uint32_t               uUserIndex;    //!< user index
    UserApiUpdateCallbackT *pCallback;    //!< function address
    void                   *pUserData;    //!< user data to return
} UserApiNotificationT;

typedef struct UserApiNotifyEventT
{
    UserApiNotifyDataT    NotificationData;
    UserApiNotificationT  *pNotificationList;
    UserApiNotifyTypeE    eNotificationType;
    uint32_t              uUserIndex;
} UserApiNotifyEventT;

typedef struct UserApiRequestT
{
    void *pUserData;                      //!< user data for the request callback
    UserApiCallbackT *pRequestCallback;   //!< user request callback
    UserApiRequestTStateE eRequestState;   //!< request state
    DirtyUserT *pLookupUser;              //!< buffer to hold the array of users to lookup
    char *pReadBuffer;                    //!< buffer for data from WebApi
    int64_t iWebRequestId;                //!< current web request id
    int32_t iLookUpBufferSize;            //!< current size of the lookup buffer;
    int32_t iReadBufferSize;              //!< the number of "meaningful" bytes in the strReadBuffer buffer
    int32_t iReadError;                   //!< flag to notify the user of an error
    int32_t iLookupUserTotal;             //!< number of users to lookup in total
    int32_t iLookupUserRemain;            //!< number of users to lookup in that we have not sent to PSN
    int32_t iLookupUserTotalSent;         //!< number of users to lookup that we have sent to PSN
    int32_t iLookupUserTotalRecv;         //!< number of users to lookup with with successful PSN response
    int32_t iLookupUserCurSent;           //!< number of users sent with the latest request
    uint32_t uUserDataMask;               //!< user data mask
    uint32_t uUserDataMaskProcessed;      //!< user data mask processed;
    uint8_t bDataReady;                   //!< is data ready
    uint8_t bValid;                       //!< FALSE if canceled or not started, TRUE if request is started
} UserApiRequestT;

typedef struct UserApiRefT
{
    int32_t iMemGroup;              //!< memgroup id
    void *pMemGroupUserData;        //!< memgroup userdata

    NetCritT crit;                        
    DirtyWebApiRefT *pWebApi;

    UserApiRequestT aUserRequest[NETCONN_MAXLOCALUSERS];
    UserApiNotifyEventT aUserApiNotifyEvent[USERAPI_MAX_QUEUED_NOTIFICATIONS];
    UserApiNotificationT aPresenceNotification[USERAPI_NOTIFY_LIST_MAX_SIZE];
    UserApiAvatarSizeE eAvatarSize;
    uint8_t bNotificationStarted;

} UserApiRefT;

/*** Function Prototypes **************************************************************************/
/*** Variables ************************************************************************************/

static const char *_UserApi_strAvatarSize[] =
{
    "s",
    "m",
    "l",
    "xl"
};

/*** Private Functions ****************************************************************************/
/*F*************************************************************************************************/
/*!
    \Function   _UserApiDirtyWebApiCallback

    \Description
        Callback from the PS5 Web Api

    \Input  iSceError       - error
    \Input  iUserIndex      - user index
    \Input  iStatusCode     - http status
    \Input *pRespBody       - data
    \Input *iRespBodyLength - data length
    \Input *pUserData       - UserListApiRefT

    \Version 01/28/2020 (tcho)
*/
/*************************************************************************************************F*/
static void _UserApiDirtyWebApiCallback(int32_t iSceError, int32_t iUserIndex, int32_t iStatusCode, const char *pRespBody, int32_t iRespBodyLength, void *pUserData)
{
    UserApiRefT *pRef = (UserApiRefT*)pUserData;
    UserApiRequestT *pRequest = &pRef->aUserRequest[iUserIndex];

    NetCritEnter(&pRef->crit);

    if ((iSceError == SCE_OK) || (pRef->aUserRequest[iUserIndex].bValid))
    {
        int32_t iRespBodySize = iRespBodyLength + 1;

        if (iRespBodySize > pRequest->iReadBufferSize)
        {
            char *pTmp;
            NetPrintf(("userapips5: [%p] resizing pReadBuffer from %d to %d bytes (user %d)\n", pRef, pRequest->iReadBufferSize, iRespBodySize, iUserIndex));
            if ((pTmp = (char*)DirtyMemAlloc(iRespBodyLength + 1, USERAPI_MEMID, pRef->iMemGroup, pRef->pMemGroupUserData)) != NULL)
            {
                // note, we do NOT need to copy the contents over
                DirtyMemFree(pRequest->pReadBuffer, USERAPI_MEMID, pRef->iMemGroup, pRef->pMemGroupUserData);
                pRequest->pReadBuffer = pTmp;
                pRequest->iReadBufferSize = iRespBodyLength + 1;
            }
            else
            {
                NetPrintf(("userapips5: [%p] could not allocate space for the pReadBuffer (user %d)\n", pRef, iUserIndex));
                pRequest->iReadError = -1;
            }
        }

        if (iRespBodySize <= pRequest->iReadBufferSize)
        {
            if ((iStatusCode >= 200) && (iStatusCode < 300))
            {
                if (iRespBodyLength > 0)
                {
                    // copy the response into the read buffer

                    ds_strnzcpy(pRequest->pReadBuffer, pRespBody, iRespBodyLength);
                    NetPrintf(("userapips5: [%p] WebApi returned status code (%d), %d characters read (user %d).\n", pRef, iStatusCode, iRespBodyLength, iUserIndex));
                }
                else
                {
                    // I would never expect this to happen for these types of requests if the response code is 2xx
                    NetPrintf(("userapips5: [%p] WebApi warning, no response body was found (user %d)\n", pRef, iUserIndex));
                }
            }
            else
            {
                NetPrintf(("userapips5: [%p] WebApi returned an unexpected status code (%d) (user %d).\n", pRef, iStatusCode, iUserIndex));
                pRequest->iReadError = -1;
            }
        }
    }
    else
    {
        NetPrintf(("userapips5: [%p] WebApi returned an SCE error code (%s) or request cancel (user %d)\n", pRef, DirtyErrGetName(iSceError), iUserIndex));
        pRequest->iReadError = iSceError;
    }

    pRequest->bDataReady = TRUE;
    NetCritLeave(&pRef->crit);
}

/*F*************************************************************************************************/
/*!
    \Function   _UserApiOnEventUpdate

    \Description
        Callback for push events notification.

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

    \Version 02/21/2020 (tcho)
*/
/*************************************************************************************************F*/
static void _UserApiOnEventUpdate(int32_t userCtxId, int32_t callbackId, const char *pNpServiceName, SceNpServiceLabel npServiceLabel, const SceNpPeerAddressA *pTo, const SceNpPeerAddressA *pFrom, const SceNpWebApi2PushEventDataType *pDataType, const char *pData, size_t dataLen, void *pUserArg)
{
    UserApiRefT *pRef;
    uint32_t uUserIndex = 0;
    uint32_t uNotifyIndex = 0;

    if (pUserArg == NULL)
    {
        NetPrintf(("userapips5: null UserApiRefT from pUserArg.\n"));
        return;
    }

    pRef = (UserApiRefT*)pUserArg;

    NetCritEnter(&pRef->crit);

    // only support onlinestatus event
    if (ds_stricmp(pDataType->val, "np:service:presence2:onlineStatus") != 0)
    {
        return;
    }

    // find the correct user whom this is for
    for (int32_t iIndex = 0; iIndex < NETCONN_MAXLOCALUSERS; ++iIndex)
    {
        SceNpAccountId AccountId;
        int32_t iReturn = NetConnStatus('acct', iIndex, &AccountId, sizeof(AccountId));

        if (iReturn >= 0 && (AccountId == pTo->accountId))
        {
            uUserIndex = (uint32_t)iIndex;
            break;
        }
    }

    // find an empty slot
    for (uNotifyIndex = 0; uNotifyIndex < USERAPI_MAX_QUEUED_NOTIFICATIONS; ++uNotifyIndex)
    {
        if (pRef->aUserApiNotifyEvent[uNotifyIndex].pNotificationList == NULL)
        {
            pRef->aUserApiNotifyEvent[uNotifyIndex].eNotificationType = USERAPI_NOTIFY_PRESENCE_UPDATE;
            pRef->aUserApiNotifyEvent[uNotifyIndex].uUserIndex = uUserIndex;
            pRef->aUserApiNotifyEvent[uNotifyIndex].pNotificationList = pRef->aPresenceNotification;
            pRef->aUserApiNotifyEvent[uNotifyIndex].NotificationData.PresenceData.uUserIndex = uUserIndex;
            DirtyUserFromNativeUser(&pRef->aUserApiNotifyEvent[uNotifyIndex].NotificationData.PresenceData.DirtyUser, &pTo->accountId);
            break;
        }
    }

    if (uNotifyIndex == USERAPI_MAX_QUEUED_NOTIFICATIONS)
    {
        NetPrintf(("userapips5: maximum notifications in queue exceeded, notification dropped\n"));
    }

    NetCritLeave(&pRef->crit);
}

/*F**********************************************************************************************/
/*!
    \Function   _UserApiHandleParseError

    \Description
        Get friends list

    \Input *pRef        - module ref
    \Input *pEventData  - event data
    \Input *pRequest    - active request
    \Input uUserIndex   - user index

    \Version 01/27/2020 (tcho)
*/
/*************************************************************************************************F*/
static void _UserApiHandleParseError(UserApiRefT *pRef, UserApiEventDataT *pEventData, UserApiRequestT *pRequest, uint32_t uUserIndex)
{
    ds_memclr(pEventData, sizeof(UserApiEventDataT));
    pEventData->eError = -1;
    pEventData->eEventType = USERAPI_EVENT_END_OF_LIST;
    pEventData->uUserIndex = uUserIndex;
    pEventData->EventDetails.EndOfList.iTotalRequested = pRequest->iLookupUserTotal;
    pEventData->EventDetails.EndOfList.iTotalReceived = pRequest->iLookupUserTotalRecv;

    pRequest->pRequestCallback(pRef, pEventData, pRequest->pUserData);
}

/*F**********************************************************************************************/
/*!
    \Function _UserApiAddAccountIdArray

    \Description
        Add array of account ids to path

    \Input *pRef          - pointer to module state
    \Input *pRequest      - request to add account ids to
    \Input *pBuffer       - [out] buffer where the request is stored
    \Input uBufferSize    - size of the buffer

    \Output
        int32_t           - positive for success, negative for error

    \Version 01/27/2020 (tcho)
*/
/*************************************************************************************************F*/
static int32_t _UserApiAddAccountIdArray(UserApiRefT *pRef, UserApiRequestT *pRequest, char *pBuffer, uint32_t uBufferSize)
{
    SceNpAccountId AccountId;
    int32_t iLookUpIndex = pRequest->iLookupUserTotalSent;
    int32_t iLookUpRemain = pRequest->iLookupUserRemain;
    int32_t iAccountIdAdded = 0;
    int32_t iResult = 0;

    if ((iResult = ds_strnzcat(pBuffer, "?accountIds=", uBufferSize)) < 0)
    {
        return(iResult);
    }

    // up to USERAPI_MAX_LOOKUPS_PER_REQUEST of account ids to the request
    for (; (iAccountIdAdded < USERAPI_MAX_LOOKUPS_PER_REQUEST) && ((iLookUpRemain - iAccountIdAdded) > 0); ++iAccountIdAdded)
    {
        if (DirtyUserToNativeUser(&AccountId, sizeof(AccountId), pRequest->pLookupUser + iLookUpIndex + iAccountIdAdded))
        {
            char strAccountId[64];

            if ((iResult = ds_snzprintf(strAccountId, sizeof(strAccountId), (iAccountIdAdded == 0) ? "%llu" : ",%llu", AccountId)) < 0)
            {
                return(iResult);
            }

            if ((iResult = ds_strnzcat(pBuffer, strAccountId, uBufferSize)) < 0)
            {
                return(iResult);
            }
        }
    }

    pRequest->iLookupUserTotalSent += iAccountIdAdded;
    pRequest->iLookupUserRemain -= iAccountIdAdded;
    pRequest->iLookupUserCurSent = iAccountIdAdded;

    return(0);
}

/*F*************************************************************************************************/
/*!
    \Function   _UserApiGetList

    \Description
        Get Profiles / Get Presence list

    \Input *pRef            - module ref
    \Input uUserIndex       - local user index

    \Output
        int32_t             - positive for success, negative for error

    \Version 02/21/2020 (tcho)
*/
/*************************************************************************************************F*/
static int32_t _UserApiGetList(UserApiRefT *pRef, uint32_t uUserIndex)
{
    int32_t iResult;
    int64_t iWebRequestId;
    char strPath[USERAPI_PATH_LEN_MAX];
    UserApiRequestT *pRequest = &pRef->aUserRequest[uUserIndex];

    if (pRequest->uUserDataMask & USERAPI_MASK_PROFILE)
    {
        pRequest->uUserDataMaskProcessed = USERAPI_MASK_PROFILE;
        ds_snzprintf(strPath, sizeof(strPath), USERAPI_PATH_PROFILES);
    }
    else if (pRequest->uUserDataMask & USERAPI_MASK_PRESENCE)
    {
        pRequest->uUserDataMaskProcessed = USERAPI_MASK_PRESENCE;
        ds_snzprintf(strPath, sizeof(strPath), USERAPI_PATH_PRESENCE);
    }
    else if (pRequest->uUserDataMask & USERAPI_MASK_RICH_PRESENCE)
    {
        // on ps5 activity is rich presence
        pRequest->uUserDataMaskProcessed = USERAPI_MASK_RICH_PRESENCE;
        ds_snzprintf(strPath, sizeof(strPath), USERAPI_PATH_ACTIVITY);
    }
    else
    {
        NetPrintf(("userapips5: [%p] unsuported mask type %d (user %d)\n", pRef, pRequest->uUserDataMask, uUserIndex));
        return(-1);
    }

    if ((iResult = _UserApiAddAccountIdArray(pRef, pRequest, strPath, sizeof(strPath))) < 0)
    {
        NetPrintf(("userapips5: [%p] failed to add account id array (user %d)\n", pRef, uUserIndex));
        return(iResult);
    }

    // if this is an actvity request we will limit this to the previous 10 entries (without a limit the request fails)
    if (pRequest->uUserDataMaskProcessed == USERAPI_MASK_RICH_PRESENCE)
    {
        if ((iResult = ds_strnzcat(strPath, "&limit=10", sizeof(strPath))) < 0)
        {
            NetPrintf(("userapips5: [%p] failed to add limit to activity request (user %d)\n", pRef, uUserIndex));
            return(iResult);
        }
    }

    //send request
    if ((iWebRequestId = DirtyWebApiRequest(pRef->pWebApi, uUserIndex, (pRequest->uUserDataMaskProcessed == USERAPI_MASK_RICH_PRESENCE) ? USERAPI_ACTIVITY_API_GROUP : USERAPI_API_GROUP, SCE_NP_WEBAPI2_HTTP_METHOD_GET, 
        strPath, NULL, 0, NULL, &_UserApiDirtyWebApiCallback, pRef)) < 0)
    {
        NetPrintf(("userlistapi5: [%p]  WebApi request failed (user %d)\n", pRef, uUserIndex));
        return((int32_t)iWebRequestId);
    }

    pRequest->iWebRequestId = iWebRequestId;

    return(iResult);
}

/*F*************************************************************************************************/
/*!
    \Function   _UserApiParseGetListResult

    \Description
         Parse get profiles / presences / block user state list results

    \Input *pRef            - module ref
    \Input uUserIndex       - local user index

    \Output
        int32_t             - 1 for waiting, 0 for success , negative for error

    \Version 01/27/2020 (tcho)
*/
/*************************************************************************************************F*/
static int32_t _UserApiParseGetListResult(UserApiRefT *pRef, uint32_t uUserIndex)
{
    char strRawData[USERAPI_ONE_ENTRY_BUFFER_SIZE];
    int32_t iElementSize;
    UserApiEventDataT EventData;
    UserApiRequestT *pRequest = &pRef->aUserRequest[uUserIndex];

    // variables for parsing
    uint16_t *pJsonParse;
    const char *pList;
    const char *pCurElement;
    const char *pEntryEnd;

    // check if response is ready to be processed
    if (pRequest->bDataReady == FALSE)
    {
        return(1);
    }
    else if (pRequest->iReadError < 0)
    {
        _UserApiHandleParseError(pRef, &EventData, pRequest, uUserIndex);
        return(-1);
    }

    // allocate space for JSON parser
    if ((pJsonParse = JsonParse2(pRequest->pReadBuffer, pRequest->iReadBufferSize, USERAPI_MEMID, pRef->iMemGroup, pRef->pMemGroupUserData)) == NULL)
    {
        _UserApiHandleParseError(pRef, &EventData, pRequest, uUserIndex);
        NetPrintf(("userapips5: [%p] could allocate space for json parser (user %d)\n", pRef, uUserIndex));
        return(-2);
    }

    iElementSize = pRequest->iLookupUserCurSent;

    // go through the array of result
    for (int32_t iElement = 0; iElement < iElementSize; ++iElement)
    {
        char strTmpString[DIRTYUSER_NATIVEUSER_MAXSIZE] = { 0 };
        ds_memclr(&EventData, sizeof(EventData));

        switch (pRequest->uUserDataMaskProcessed)
        {
            case USERAPI_MASK_PROFILE:
            {
                const char *pAvatarList;
                const char *pLocaleList;

                if ((pList = JsonFind2(pJsonParse, NULL, "profiles[", iElement)) == NULL)
                {
                    _UserApiHandleParseError(pRef, &EventData, pRequest, uUserIndex);
                    DirtyMemFree(pJsonParse, USERAPI_MEMID, pRef->iMemGroup, pRef->pMemGroupUserData);
                    NetPrintf(("userapips5: [%p] unexpected json response (user %d)\n", pRef, uUserIndex));
                    return(-3);
                }

                // parse online id which is the gamer tag on PSN
                pCurElement = JsonFind2(pJsonParse, pList, ".onlineId", iElement);
                JsonGetString(pCurElement, EventData.EventDetails.UserData.Profile.strGamertag, sizeof(EventData.EventDetails.UserData.Profile.strGamertag), "");

                // parse display name
                if ((pCurElement = JsonFind2(pJsonParse, pList, ".personalDetail.firstName", iElement)) != NULL)
                {
                    // if the personal name is not private, the display name is firstName_lastName
                    JsonGetString(pCurElement, strTmpString, sizeof(strTmpString), "");
                    ds_strnzcat(EventData.EventDetails.UserData.Profile.strDisplayName, strTmpString, sizeof(EventData.EventDetails.UserData.Profile.strDisplayName));
                    pCurElement = JsonFind2(pJsonParse, pList, ".personalDetail.lastName", iElement);
                    JsonGetString(pCurElement, strTmpString, sizeof(strTmpString), "");
                    ds_strnzcat(EventData.EventDetails.UserData.Profile.strDisplayName, " ", sizeof(EventData.EventDetails.UserData.Profile.strDisplayName));
                    ds_strnzcat(EventData.EventDetails.UserData.Profile.strDisplayName, strTmpString, sizeof(EventData.EventDetails.UserData.Profile.strDisplayName));
                }
                else
                {
                    // if the personal name is private the display name is the online id
                    ds_strnzcpy(EventData.EventDetails.UserData.Profile.strDisplayName, EventData.EventDetails.UserData.Profile.strGamertag, SCE_NP_ONLINEID_MAX_LENGTH + 1);
                }

                // get the correct avatar url
                for (int32_t iAvatarIndex = 0; iAvatarIndex < USERAPI_AVATAR_LIST_SIZE; ++iAvatarIndex)
                {
                    char strAvatarSize[4] = { 0 };
                    pAvatarList = JsonFind2(pJsonParse, pList, ".avatars[", iAvatarIndex);
                    pCurElement = JsonFind2(pJsonParse, pAvatarList, ".size", iAvatarIndex);
                    JsonGetString(pCurElement, strAvatarSize, sizeof(strAvatarSize), "");

                    // grab the correct avatar size url
                    if (ds_stricmp(strAvatarSize, _UserApi_strAvatarSize[pRef->eAvatarSize]) == 0)
                    {
                        pCurElement = JsonFind2(pJsonParse, pAvatarList, ".url", iAvatarIndex);
                        JsonGetString(pCurElement, EventData.EventDetails.UserData.Profile.strAvatarUrl, sizeof(EventData.EventDetails.UserData.Profile.strAvatarUrl), "");
                        break;
                    }
                }

                //grab the locale code
                pLocaleList = JsonFind2(pJsonParse, pList, ".languages[", 0);
                JsonGetString(pLocaleList, strTmpString, sizeof(strTmpString), "");

                // remove the underscore
                memmove(strTmpString + 2, strTmpString + 3, sizeof(strTmpString) - 2);
                EventData.EventDetails.UserData.Profile.uLocale = LOBBYAPI_LocalizerTokenCreateFromString(strTmpString);

                break;
            }
            case USERAPI_MASK_PRESENCE:
            {
                if ((pList = JsonFind2(pJsonParse, NULL, "basicPresences[", iElement)) == NULL)
                {
                    _UserApiHandleParseError(pRef, &EventData, pRequest, uUserIndex);
                    DirtyMemFree(pJsonParse, USERAPI_MEMID, pRef->iMemGroup, pRef->pMemGroupUserData);
                    NetPrintf(("userapips5: [%p] unexpected json response (user %d)\n", pRef, uUserIndex));
                    return(-4);
                }

                // parse online status
                pCurElement = JsonFind2(pJsonParse, pList, ".onlineStatus", iElement);
                JsonGetString(pCurElement, strTmpString, sizeof(strTmpString), "");

                if (ds_stricmp(strTmpString, "online") == 0)
                {
                    EventData.EventDetails.UserData.Presence.ePresenceStatus = USERAPI_PRESENCE_ONLINE;
                }
                else if (ds_stricmp(strTmpString, "offline") == 0)
                {
                    EventData.EventDetails.UserData.Presence.ePresenceStatus = USERAPI_PRESENCE_OFFLINE;
                }
                else
                {
                    EventData.EventDetails.UserData.Presence.ePresenceStatus = USERAPI_PRESENCE_UNKNOWN;
                }

                // parse inContext
                pCurElement = JsonFind2(pJsonParse, pList, ".inContext", iElement);

                if (JsonGetBoolean(pCurElement, FALSE) == TRUE)
                {
                    EventData.EventDetails.UserData.Presence.bIsPlayingSameTitle = TRUE;
                }
                else
                {
                    EventData.EventDetails.UserData.Presence.bIsPlayingSameTitle = FALSE;
                }

                break;
            }
            case USERAPI_MASK_RICH_PRESENCE:
            {
                const char *pActivityList;
                uint32_t uActivityIndex = 0;

                if ((pList = JsonFind2(pJsonParse, NULL, "users[", iElement)) == NULL)
                {
                    _UserApiHandleParseError(pRef, &EventData, pRequest, uUserIndex);
                    DirtyMemFree(pJsonParse, USERAPI_MEMID, pRef->iMemGroup, pRef->pMemGroupUserData);
                    NetPrintf(("userapips5: [%p] unexpected json response (user %d)\n", pRef, uUserIndex));
                    return(-5);
                }

                // we will only return the latest unhidden activity 
                while ((pActivityList = JsonFind2(pJsonParse, pList, ".activities[", uActivityIndex)) != NULL)
                {
                    pCurElement = JsonFind2(pJsonParse, pActivityList, ".hidden", uActivityIndex);

                    if (JsonGetBoolean(pCurElement, FALSE) == FALSE)
                    {
                        pCurElement = JsonFind2(pJsonParse, pActivityList, ".name", uActivityIndex);
                        JsonGetString(pCurElement, EventData.EventDetails.UserData.RichPresence.strData, sizeof(EventData.EventDetails.UserData.RichPresence.strData), "");

                        break;
                    }

                    uActivityIndex++;
                }

                break;
            }
        }

        ds_memclr(&strRawData, sizeof(strRawData));
        EventData.eEventType = USERAPI_EVENT_DATA;
        EventData.uUserIndex = uUserIndex;
        EventData.EventDetails.UserData.uUserDataMask = pRequest->uUserDataMaskProcessed;
        ds_memcpy(&EventData.EventDetails.UserData.DirtyUser, pRequest->pLookupUser + iElement, sizeof(DirtyUserT));

        // populate raw data
        if ((pEntryEnd = JsonGetListItemEnd(pList)) == NULL)
        {
            _UserApiHandleParseError(pRef, &EventData, pRequest, uUserIndex);
            DirtyMemFree(pJsonParse, USERAPI_MEMID, pRef->iMemGroup, pRef->pMemGroupUserData);
            NetPrintf(("userapips5: [%p] unexpected json response (user %d)\n", pRef, uUserIndex));
            return(-5);
        }

        ds_strnzcpy(strRawData, pList, pEntryEnd - pList + 1);

        if (pRequest->uUserDataMaskProcessed == USERAPI_MASK_PRESENCE)
        {
            EventData.EventDetails.UserData.Presence.pRawData = &strRawData[0];
        }
        else if (pRequest->uUserDataMaskProcessed == USERAPI_MASK_PROFILE)
        {
            EventData.EventDetails.UserData.Profile.pRawData = &strRawData[0];
        }
        else if (pRequest->uUserDataMaskProcessed == USERAPI_MASK_RICH_PRESENCE)
        {
            EventData.EventDetails.UserData.RichPresence.pRawData = &strRawData[0];
        }

        pRequest->pRequestCallback(pRef, &EventData, pRequest->pUserData);
    }

    pRequest->iLookupUserTotalRecv += pRequest->iLookupUserCurSent;
    pRequest->iLookupUserCurSent = 0;

    // this is the end of the list 
    if (pRequest->iLookupUserRemain == 0)
    {
        ds_memclr(&EventData, sizeof(EventData));
        EventData.eEventType = USERAPI_EVENT_END_OF_LIST;
        EventData.uUserIndex = uUserIndex;
        EventData.EventDetails.EndOfList.iTotalErrors = pRequest->iLookupUserTotalSent - pRequest->iLookupUserTotalRecv;
        EventData.EventDetails.EndOfList.iTotalReceived = pRequest->iLookupUserTotalRecv;
        EventData.EventDetails.EndOfList.iTotalRequested = pRequest->iLookupUserTotalSent;
        pRequest->uUserDataMask &= ~pRequest->uUserDataMaskProcessed;
        pRequest->pRequestCallback(pRef, &EventData, pRequest->pUserData);
    }
    
    DirtyMemFree(pJsonParse, USERAPI_MEMID, pRef->iMemGroup, pRef->pMemGroupUserData);
    pRequest->bDataReady = FALSE;

    return(0);
}

/*F*************************************************************************************************/
/*!
    \Function   _UserApiProcessRequest

    \Description
        Process request

    \Input *pRef        - module ref

    \Output
        int32_t         - positive for success, negative for error

    \Version 01/28/2020 (tcho)
*/
/*************************************************************************************************F*/
static int32_t _UserApiProcessRequest(UserApiRefT *pRef)
{
    int32_t iResult = 0;

    for (uint32_t uUserIndex = 0; uUserIndex < NETCONN_MAXLOCALUSERS; ++uUserIndex)
    {
        uint8_t bCleanupRequest = FALSE;
        UserApiRequestT *pRequest = &pRef->aUserRequest[uUserIndex];

        switch (pRequest->eRequestState)
        {
            case USERAPI_GETLIST:
            {
                // submit webapi request
                if ((iResult = _UserApiGetList(pRef, uUserIndex)) < 0)
                {
                    NetPrintf(("userapips5: [%p] encountered error with request mask: %d (user %d)\n", pRef, pRequest->uUserDataMaskProcessed, uUserIndex));
                    bCleanupRequest = TRUE;
                }
                // move on to the wait state
                else
                {
                    pRequest->eRequestState = USERAPI_GETLIST_WAIT;
                }
                break;
            }
            case USERAPI_GETLIST_WAIT:
            {
                // try to parse results 
                if ((iResult = _UserApiParseGetListResult(pRef, uUserIndex)) < 0)
                {
                    NetPrintf(("userapips5: [%p] encountered error parsing results mask: %d (user %d)\n", pRef, pRequest->uUserDataMaskProcessed, uUserIndex));
                    bCleanupRequest = TRUE;
                }
                // finished with the current request but there are more requests to do
                else if ((pRequest->uUserDataMask != 0) && (iResult != 1))
                {
                    // reset the requst state to do another request
                    pRequest->eRequestState = USERAPI_GETLIST;

                    // reset the request if we are doing a completely different request
                    if (pRequest->iLookupUserRemain == 0)
                    {
                        pRequest->iLookupUserRemain = pRequest->iLookupUserTotal;
                        pRequest->iLookupUserCurSent = 0;
                        pRequest->iLookupUserTotalRecv = 0;
                        pRequest->iLookupUserTotalSent = 0;
                    }
                }
                // done with request
                else if (iResult != 1)
                {
                    bCleanupRequest = TRUE;
                }
                break;
            }
            default:
                return(0);
        }

        if (bCleanupRequest == TRUE)
        {
            pRequest->bDataReady = FALSE;
            pRequest->bValid = FALSE;
            pRequest->eRequestState = USERAPI_IDLE;
        }
    }

    return(iResult);
}

/*F*************************************************************************************************/
/*!
    \Function   _UserApiProcessNotifications

    \Description
        Process first party list update notifications

    \Input *pRef        - module ref

    \Output
        int32_t         - positive for success, negative for error

    \Version 02/13/2020 (tcho)
*/
/*************************************************************************************************F*/
static void _UserApiProcessNotifications(UserApiRefT *pRef)
{
    if (pRef->aUserApiNotifyEvent[0].pNotificationList == NULL)
    {
        // no notification to process
        return;
    }

    // at least one notification is in the list, do processing
    for (uint32_t uPsnEventIndex = 0; uPsnEventIndex < USERAPI_MAX_QUEUED_NOTIFICATIONS; ++uPsnEventIndex)
    {
        UserApiNotificationT *pNotificationList = pRef->aUserApiNotifyEvent[uPsnEventIndex].pNotificationList;
        uint32_t uNotficationUserId = pRef->aUserApiNotifyEvent[uPsnEventIndex].uUserIndex;

        if (pNotificationList == NULL)
        {
            continue;
        }

        // dispatch notifications
        for (uint32_t uNotifyCbIndex = 0; uNotifyCbIndex < USERAPI_NOTIFY_LIST_MAX_SIZE; ++uNotifyCbIndex)
        {
            if (pNotificationList[uNotifyCbIndex].pCallback == NULL)
            {
                continue;
            }

            // if the notification is for the requestor of the callback
            if (uNotficationUserId == pNotificationList[uNotifyCbIndex].uUserIndex)
            {
                pNotificationList[uNotifyCbIndex].pCallback(pRef,
                    pRef->aUserApiNotifyEvent[uPsnEventIndex].eNotificationType,
                    &pRef->aUserApiNotifyEvent[uPsnEventIndex].NotificationData,
                    pNotificationList[uNotifyCbIndex].pUserData);
            }
        }

        // clean up
        pRef->aUserApiNotifyEvent[uPsnEventIndex].pNotificationList = NULL;
    }
}

/*** Public Functions *****************************************************************************/

/*F*************************************************************************************************/
/*!
    \Function    UserApiCreate

    \Description
        Starts UserApi.

    \Output
        UserApiRefT*  - pointer to module ref if successful, NULL otherwise.

    \Version 01/27/2020 (tcho)
*/
/*************************************************************************************************F*/
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
        NetPrintf(("userapips5: could not allocate module state\n"));
        return(NULL);
    }

    ds_memclr(pRef, sizeof(*pRef));
    pRef->iMemGroup = iMemGroup;
    pRef->pMemGroupUserData = pMemGroupUserData;
    pRef->eAvatarSize = USERAPI_AVATAR_S;

    NetCritInit(&pRef->crit, "userapi crit");

    // retrieve webapi from netconn
    NetConnStatus('wapi', 0, &pRef->pWebApi, sizeof(&pRef->pWebApi));
    if (pRef->pWebApi == NULL)
    {
        NetPrintf(("userapips5: [%p] webapi is null.\n", pRef));
        UserApiDestroy(pRef);
        return(NULL);
    }

    // increase the buffer size so it will at least hold 100 responses
    DirtyWebApiControl(pRef->pWebApi, 'rbuf', USERAPI_WEBAPI_BUFFER, 0, NULL);

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

    \Version 01/27/2020 (tcho)
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

    NetCritKill(&pRef->crit);
    DirtyMemFree(pRef, USERAPI_MEMID, pRef->iMemGroup, pRef->pMemGroupUserData);
    return(0);
}

/*F*************************************************************************************************/
/*!
    \Function    UserApiCancel

    \Description
        Cancel all queries to the 1st party.

    \Input *pRef        - pointer to UserApiT module reference
    \Input uUserIndex   - index of user associated with this request

    \Output
        int32_t         - result of abort request; see UserApiPlatAbortRequests()

   \Version 01/27/2020 (tcho)
*/
/*************************************************************************************************F*/
int32_t UserApiCancel(UserApiRefT *pRef, uint32_t uUserIndex)
{
    NetCritEnter(&pRef->crit);

    // aborts on going requests
    if (pRef->pWebApi != NULL)
    {
        for (int32_t iUserIndex = 0; iUserIndex < NETCONN_MAXLOCALUSERS; ++iUserIndex)
        {
            if (pRef->aUserRequest[iUserIndex].eRequestState == USERAPI_GETLIST_WAIT)
            {
                DirtyWebApiAbortRequestById(pRef->pWebApi, iUserIndex, pRef->aUserRequest[iUserIndex].iWebRequestId);
            }
        }
    }

    if (pRef->aUserRequest[uUserIndex].pLookupUser != NULL)
    {
        DirtyMemFree(pRef->aUserRequest[uUserIndex].pLookupUser, USERAPI_MEMID, pRef->iMemGroup, pRef->pMemGroupUserData);
    }

    if (pRef->aUserRequest[uUserIndex].pReadBuffer != NULL)
    {
        DirtyMemFree(pRef->aUserRequest[uUserIndex].pReadBuffer, USERAPI_MEMID, pRef->iMemGroup, pRef->pMemGroupUserData);
    }

    ds_memclr(&pRef->aUserRequest[uUserIndex], sizeof(UserApiRequestT));

    NetCritLeave(&pRef->crit);

    return(0);
}

/*F*************************************************************************************************/
/*!
    \Function    UserApiRequestTProfilesAsync

    \Description
        Starts the process to retrieve user information of players. pLookupUsers is a pointer to the first DirtyUserT, and iLookupUsersLength is the number of DirtyUserTs.

    \Input *pRef                - pointer to UserApiT module reference
    \Input  uUserIndex          - Index used to specify which local user owns the request
    \Input *pLookupUsers        - Pointer to first DityUserT in an array of iLookupUsersLength elements
    \Input  iLookupUsersLength  - Number of elements in the pLookupUsers array
    \Input *pCallback           - Callback that is going to be called when responses for these requests are received
    \Input *pUserData           - This pointer is going to be passed to the callback when it is called. This parameter can be NULL

    \Output
         int32_t                - Return 0 if successful, -1 otherwise.

    \Version 02/17/2020 (tcho)
*/
/*************************************************************************************************F*/
int32_t UserApiRequestProfilesAsync(UserApiRefT *pRef, uint32_t uUserIndex, DirtyUserT *pLookupUsers, int32_t iLookupUsersLength, UserApiCallbackT *pCallback, void *pUserData)
{
    // check user index range
    if (uUserIndex >= NETCONN_MAXLOCALUSERS)
    {
        NetPrintf(("userapips5: [%p] user index out of range (user %u)\n", pRef, uUserIndex));
        return(-1);
    }

    // plookup users cannot be null and legth must be greater than 0
    if ((pLookupUsers == NULL) || (iLookupUsersLength <= 0))
    {
        NetPrintf(("userapips5: [%p] invalid pointer to DirtyUserT list\n", pRef));
        return(-2);
    }

    // pCallback cannot be null
    if (pCallback == NULL)
    {
        NetPrintf(("userapips5: [%p] invalid pointer to callback\n", pRef));
        return(-3);
    }

    NetCritEnter(&pRef->crit);

    // check if a request for is already in progress for the the user
    if (pRef->aUserRequest[uUserIndex].eRequestState != USERAPI_IDLE)
    {
        NetPrintf(("userapips5: [%p] module is already handling a request for user (%u)\n", pRef, uUserIndex));
        NetCritLeave(&pRef->crit);
        return(-5);
    }

    // save the lookup array
    if (pRef->aUserRequest[uUserIndex].iLookUpBufferSize < (int32_t)(sizeof(DirtyUserT) * iLookupUsersLength))
    {
        if (pRef->aUserRequest[uUserIndex].pLookupUser != NULL)
        {
            DirtyMemFree(pRef->aUserRequest[uUserIndex].pLookupUser, USERAPI_MEMID, pRef->iMemGroup, pRef->pMemGroupUserData);
        }

        if ((pRef->aUserRequest[uUserIndex].pLookupUser = DirtyMemAlloc(sizeof(DirtyUserT) * iLookupUsersLength, USERAPI_MEMID, pRef->iMemGroup, pRef->pMemGroupUserData)) == NULL)
        {
            NetPrintf(("userapips5: [%p] cannont allocate memory to save down lookup users (user %d)\n ", pRef, uUserIndex));
            NetCritLeave(&pRef->crit);
            return(-6);
        }

        pRef->aUserRequest[uUserIndex].iLookUpBufferSize = sizeof(DirtyUserT) * iLookupUsersLength;
    }

    ds_memcpy(pRef->aUserRequest[uUserIndex].pLookupUser, pLookupUsers, sizeof(DirtyUserT) * iLookupUsersLength);
    pRef->aUserRequest[uUserIndex].iLookupUserTotal = iLookupUsersLength;
    pRef->aUserRequest[uUserIndex].iLookupUserRemain = iLookupUsersLength;
    pRef->aUserRequest[uUserIndex].iLookupUserTotalSent = 0;
    pRef->aUserRequest[uUserIndex].iLookupUserTotalRecv = 0;
    pRef->aUserRequest[uUserIndex].iLookupUserCurSent = 0;
    pRef->aUserRequest[uUserIndex].pRequestCallback = pCallback;
    pRef->aUserRequest[uUserIndex].pUserData = pUserData;
    pRef->aUserRequest[uUserIndex].uUserDataMask = USERAPI_MASK_PROFILE;
    pRef->aUserRequest[uUserIndex].uUserDataMaskProcessed = 0;
    pRef->aUserRequest[uUserIndex].eRequestState = USERAPI_GETLIST;
    pRef->aUserRequest[uUserIndex].bValid = TRUE;

    NetCritLeave(&pRef->crit);
    return(0);
}

/*F*************************************************************************************************/
/*!
    \Function    UserApiRequestTProfileAsync

    \Description
        Starts the process to retrieve profile information of players. pLookupUser is a pointer to DirtyUserT.

    \Input *pRef                - pointer to UserApiT module reference
    \Input  uUserIndex          - Index used to specify which local user owns the request
    \Input *pLookupUser         - Pointer to DityUserT
    \Input *pCallback           - Callback that is going to be called when responses for these requests are received
    \Input  uUserDataMask       - A mask value defining which elements are needed; USERAPI_MASK_*
    \Input *pUserData           - This pointer is going to be passed to the callback when it is called. This parameter can be NULL

    \Output
         int32_t                - Return 0 if successful, -1 otherwise.

   \Version 01/27/2020 (tcho)
*/
/*************************************************************************************************F*/
int32_t UserApiRequestProfileAsync(UserApiRefT *pRef, uint32_t uUserIndex, DirtyUserT *pLookupUser, UserApiCallbackT *pCallback, uint32_t uUserDataMask, void *pUserData)
{
    return(UserApiRequestPresenceAsync(pRef, uUserIndex, pLookupUser, pCallback, uUserDataMask, pUserData));
}

/*F*************************************************************************************************/
/*!
    \Function    UserApiRequestRichPresenceAsync

    \Description
        Starts the process to retrieve rich presence information of players.

    \Input *pRef                - pointer to UserApiT module reference
    \Input  uUserIndex          - Index used to specify which local user owns the request
    \Input *pLookupUser         - Pointer to DirtyUserT
    \Input *pCallback           - Callback that is going to be called when responses for these requests are received
    \Input uUserDataMask        - A mask value defining which elements are needed; USERAPI_MASK_*
    \Input *pUserData           - This pointer is going to be passed to the callback when it is called. This parameter can be NULL

    \Output
         int32_t                - Return 0 if successful, -1 otherwise.

   \Version 01/27/2020 (tcho)
*/
/*************************************************************************************************F*/
int32_t UserApiRequestRichPresenceAsync(UserApiRefT *pRef, uint32_t uUserIndex, DirtyUserT *pLookupUser, UserApiCallbackT *pCallback, uint32_t uUserDataMask, void *pUserData)
{
    return(UserApiRequestPresenceAsync(pRef, uUserIndex, pLookupUser, pCallback, uUserDataMask, pUserData));
}

/*F*************************************************************************************************/
/*!
    \Function    UserApiRequestPresenceAsync

    \Description
        Starts the process to retrieve profile information of players. pLookupUser is a pointer to DirtyUserT.

    \Input *pRef                - pointer to UserApiT module reference
    \Input  uUserIndex          - Index used to specify which local user owns the request
    \Input *pLookupUser         - Pointer to DityUserT
    \Input *pCallback           - Callback that is going to be called when responses for these requests are received
    \Input  uUserDataMask       - A mask value defining which elements are needed; USERAPI_MASK_*
    \Input *pUserData           - This pointer is going to be passed to the callback when it is called. This parameter can be NULL

    \Output
         int32_t                - Return 0 if successful, -1 otherwise.

   \Version 01/27/2020 (tcho)
*/
/*************************************************************************************************F*/
int32_t UserApiRequestPresenceAsync(UserApiRefT *pRef, uint32_t uUserIndex, DirtyUserT *pLookupUser, UserApiCallbackT *pCallback, uint32_t uUserDataMask, void *pUserData)
{
    // check user index range
    if (uUserIndex >= NETCONN_MAXLOCALUSERS)
    {
        NetPrintf(("userapips5: [%p] user index out of range (user %u)\n", pRef, uUserIndex));
        return(-1);
    }

    // uUserDataMask must be set
    if (uUserDataMask == 0)
    {
        NetPrintf(("userapips5: [%p] query without a mask value submitted (user %u)\n", pRef, uUserIndex));
        return(-2);
    }

    // plookup users cannot be null
    if (pLookupUser == NULL)
    {
        NetPrintf(("userapips5: [%p] invalid pointer to DirtyUserT list\n", pRef));
        return(-3);
    }

    // pCallback cannot be null
    if (pCallback == NULL)
    {
        NetPrintf(("userapips5: [%p] invalid pointer to callback\n", pRef));
        return(-4);
    }

    NetCritEnter(&pRef->crit);

    // check if a request for is already in progress for the the user
    if (pRef->aUserRequest[uUserIndex].eRequestState != USERAPI_IDLE)
    {
        NetPrintf(("userapips5: [%p] module is already handling a request for user (%u)\n", pRef, uUserIndex));
        NetCritLeave(&pRef->crit);
        return(-5);
    }

    // save the lookup array (only support 1 user non batched)
    if (pRef->aUserRequest[uUserIndex].iLookUpBufferSize < (int32_t)sizeof(DirtyUserT))
    {
        if (pRef->aUserRequest[uUserIndex].pLookupUser != NULL)
        {
            DirtyMemFree(pRef->aUserRequest[uUserIndex].pLookupUser, USERAPI_MEMID, pRef->iMemGroup, pRef->pMemGroupUserData);
        }

        if ((pRef->aUserRequest[uUserIndex].pLookupUser = DirtyMemAlloc(sizeof(DirtyUserT), USERAPI_MEMID, pRef->iMemGroup, pRef->pMemGroupUserData)) == NULL)
        {
            NetPrintf(("userapips5: [%p] cannont allocate memory to save down lookup users (user %d)\n ", pRef, uUserIndex));
            NetCritLeave(&pRef->crit);
            return(-6);
        }

        pRef->aUserRequest[uUserIndex].iLookUpBufferSize = sizeof(DirtyUserT);
    }

    ds_memcpy(pRef->aUserRequest[uUserIndex].pLookupUser, pLookupUser, sizeof(DirtyUserT));
    pRef->aUserRequest[uUserIndex].iLookupUserTotal = 1;
    pRef->aUserRequest[uUserIndex].iLookupUserRemain = 1;
    pRef->aUserRequest[uUserIndex].iLookupUserTotalSent = 0;
    pRef->aUserRequest[uUserIndex].iLookupUserTotalRecv = 0;
    pRef->aUserRequest[uUserIndex].iLookupUserCurSent = 0;
    pRef->aUserRequest[uUserIndex].pRequestCallback = pCallback;
    pRef->aUserRequest[uUserIndex].pUserData = pUserData;
    pRef->aUserRequest[uUserIndex].uUserDataMask = uUserDataMask;
    pRef->aUserRequest[uUserIndex].uUserDataMaskProcessed = 0;
    pRef->aUserRequest[uUserIndex].bValid = TRUE;
    pRef->aUserRequest[uUserIndex].eRequestState = USERAPI_GETLIST;

    NetCritLeave(&pRef->crit);

    return(0);
}

/*F*************************************************************************************************/
/*!
    \Function    UserApiRegisterUpdateEvent

    \Description
        Register for notifications from first part

    \Input *pRef                - pointer to UserApiT module reference
    \Input  uUserIndex          - Index used to specify which local user owns the request
    \Input  eType               - type of event to register for
    \Input *pNotifyCb           - Callback that is going to be called when responses for these requests are received
    \Input *pUserData           - This pointer is going to be passed to the callback when it is called. This parameter can be NULL

    \Output
         int32_t                - Return 0 if successful, -1 otherwise.

   \Version 01/27/2020 (tcho)
*/
/*************************************************************************************************F*/
int32_t UserApiRegisterUpdateEvent(UserApiRefT *pRef, uint32_t uUserIndex, UserApiNotifyTypeE eType, UserApiUpdateCallbackT *pNotifyCb, void *pUserData)
{
    int32_t iResult = 0;
    int32_t iNotifyIndex;

    if (uUserIndex >= NETCONN_MAXLOCALUSERS)
    {
        NetPrintf(("userapips5: [%p] user index out of range\n", pRef));
        return(-1);
    }

    if (pNotifyCb == NULL)
    {
        NetPrintf(("userapips5: [%p] notify callback cannot be null\n"));
        return(-2);
    }

    NetCritEnter(&pRef->crit);

    // look for empty slot to add callback on to the notify list
    for (iNotifyIndex = 0; iNotifyIndex < USERAPI_NOTIFY_LIST_MAX_SIZE; ++iNotifyIndex)
    {
        if ((pRef->aPresenceNotification[iNotifyIndex].pCallback == pNotifyCb) && (pRef->aPresenceNotification[iNotifyIndex].uUserIndex == uUserIndex))
        {
            // already added
            break;
        }

        if (pRef->aPresenceNotification[iNotifyIndex].pCallback == NULL)
        {
            pRef->aPresenceNotification[iNotifyIndex].pCallback = pNotifyCb;
            pRef->aPresenceNotification[iNotifyIndex].pUserData = pUserData;
            pRef->aPresenceNotification[iNotifyIndex].uUserIndex = uUserIndex;
            break;
        }
    }

    if (iNotifyIndex != USERAPI_NOTIFY_LIST_MAX_SIZE)
    {
        // add psn push events
        if (pRef->bNotificationStarted == FALSE)
        {
            switch (eType)
            {
                case USERAPI_NOTIFY_PRESENCE_UPDATE:
                {
                    iResult = DirtyWebApiAddPushEventListener(pRef->pWebApi, "np:service:presence2:onlineStatus", _UserApiOnEventUpdate, pRef);
                    pRef->bNotificationStarted = TRUE;
                    break;
                }
                default:
                {
                    iResult = USERAPI_ERROR_UNSUPPORTED;
                    NetPrintf(("userapips5: [%p] event type not supported on PS5!\n"));
                    break;
                }
            }
        }
    }
    else
    {
        NetPrintf(("userapips5: [%p] notification slots is all full\n"));
        iResult = USERAPI_ERROR_FULL;
    }

    NetCritLeave(&pRef->crit);

    return(iResult);
}

/*F********************************************************************************/
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
            'abrt' - Abort the current request associated with the user at index iValue
            'avsz' - Set which avatar size will be retrieved. iValue = 's' for small, 'm' for medium and 'l' for big. 's' is the default value, and this is just functional for PS4.
        \endverbatim

   \Version 01/27/2020 (tcho)
*/
/********************************************************************************F*/
int32_t UserApiControl(UserApiRefT *pRef, int32_t iControl, int32_t iValue, int32_t iValue2, void *pValue)
{
    if (iControl == 'abrt')
    {
        if ((iValue < 0) || (iValue >= NETCONN_MAXLOCALUSERS))
        {
            NetPrintf(("userapips5: [%p] iValue(%d) is not a valid user index\n", pRef, iValue));
            return(-1);
        }

        UserApiCancel(pRef, iValue);
        return(0);
    }
    
    if (iControl == 'avsz')
    {
        switch (iValue)
        {
            case 's':
            {
                pRef->eAvatarSize = USERAPI_AVATAR_S;
                break;
            }
            case 'm':
            {
                pRef->eAvatarSize = USERAPI_AVATAR_M;
                break;
            }
            case 'l':
            {
                pRef->eAvatarSize = USERAPI_AVATAR_L;
                break;
            }
            case 'x':
            {
                pRef->eAvatarSize = USERAPI_AVATAR_XL;
                break;
            }
        }
        return(0);
    }

    return(-1);
}

/*F*************************************************************************************************/
/*!
    \Function    UserApiUpdate

    \Description
        Update the internal state of the module, and call registered UserApiCallbackT callback if there are GamerCard/Profile responses available. This function should be called periodically.

    \Input *pRef - pointer to UserApiT module reference

    \Version 01/27/2020 (tcho)
*/
/*************************************************************************************************F*/
void UserApiUpdate(UserApiRefT *pRef)
{
    if (!NetCritTry(&pRef->crit))
    {
        return;
    }

    _UserApiProcessRequest(pRef);
    _UserApiProcessNotifications(pRef);

    NetCritLeave(&pRef->crit);
}

/*F*************************************************************************************************/
/*!
    \Function    UserApiPostRecentlyMetAsync

    \Description
        Starts the process to post that a certain player was recently encountered.

    \Input *pRef                - pointer to UserApiT module reference
    \Input  uUserIndex          - Index used to specify which local user owns the request
    \Input *pPlayerMet          - Pointer to the DityUserT to add to the recently met players list
    \Input *pAdditionalInfo     - Pointer to additional info container which a platform may require
    \Input *pCallback           - Callback that is going to be called when responses for these requests are received
    \Input *pUserData           - This pointer is going to be passed to the callback when it is called. This parameter can be NULL

    \Output
         int32_t                - Return 0 if successful, -1 otherwise.

  \Version 01/27/2020 (tcho)
*/
/*************************************************************************************************F*/
int32_t UserApiPostRecentlyMetAsync(UserApiRefT *pRef, uint32_t uUserIndex, DirtyUserT *pPlayerMet, void *pAdditionalInfo, UserApiPostCallbackT *pCallback, void *pUserData)
{
    NetPrintf(("userapips5: [%p] post recently met is not support on PS5\n", pRef));
    return(USERAPI_ERROR_UNSUPPORTED);
}

/*F*************************************************************************************************/
/*!
    \Function    UserApiPostRichPresenceAsync

    \Description
        Starts the process to post that a certain player was recently encountered.

    \Input *pRef                - pointer to UserApiT module reference
    \Input  uUserIndex          - Index used to specify which local user owns the request
    \Input *pData               - Rich Presence to post
    \Input *pCallback           - Callback that is going to be called when responses for these requests are received
    \Input *pUserData           - This pointer is going to be passed to the callback when it is called. This parameter can be NULL

    \Output
         int32_t                - Return 0 if successful, -1 otherwise.

    \Version 01/27/2020 (tcho)
*/
/*************************************************************************************************F*/
int32_t UserApiPostRichPresenceAsync(UserApiRefT *pRef, uint32_t uUserIndex, UserApiRichPresenceT *pData, UserApiPostCallbackT *pCallback, void *pUserData)
{
    NetPrintf(("userapips5: [%p] Rich presence is not support on PS5\n", pRef));
    return(USERAPI_ERROR_UNSUPPORTED);
}
