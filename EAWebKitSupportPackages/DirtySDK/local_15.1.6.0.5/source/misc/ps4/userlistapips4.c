/*H********************************************************************************/
/*!
    \File userlistapips4.c

    \Description
        See userlistapi.h

    \Copyright
        Copyright (c) Electronic Arts 2013

    \Version 1.0 04/17/2013 (amakoukji) First Version
*/
/********************************************************************************H*/

/*** Include files ***********************************************************/

#include <kernel.h>
#include <string.h>
#include <net.h>
//#include <np/np_npid.h>
#include <stdlib.h>
#include <stdint.h>

#include "DirtySDK/misc/userlistapi.h"
#include "DirtySDK/misc/ps4/userlistapips4.h"

#include "DirtySDK/dirtysock.h"
#include "DirtySDK/dirtylang.h"
#include "DirtySDK/dirtysock/dirtyerr.h"
#include "DirtySDK/dirtysock/dirtylib.h"
#include "DirtySDK/dirtysock/dirtymem.h"
#include "DirtySDK/dirtysock/netconn.h"
#include "DirtySDK/proto/protohttp.h"
#include "DirtySDK/util/jsonparse.h"
#include "DirtySDK/dirtysock/ps4/dirtywebapips4.h"

/*** Defines ***************************************************************************/

#define USERLISTAPI_DEFAULT_READ_BUFFER_SIZE (2 * 1024)
#define USERLISTAPI_ONE_FRIEND_BUFFER_SIZE (1024)
#define USERLISTAPI_API_GROUP       "sdk:userProfile"
#define USERLISTAPI_PATH_BASE       "/v1/users"
#define USERLISTAPI_PATH_FRIENDS    "friendList"
#define USERLISTAPI_PATH_BLOCKED    "blockingUsers"
#define USERLISTAPI_USER_NOT_FOUND_ERROR_CODE  (2105356)
#define USERLISTAPI_MAX_QUEUED_NOTIFICATIONS (100)

#define USERLISTAPI_FRIEND_TYPE_FRIEND "friend"


/*** Macros ****************************************************************************/

/*** Type Definitions ******************************************************************/

typedef enum UserListApiRequestTypeE
{
    USERLISTAPI_REQUEST_TYPE_FRIENDS,
    USERLISTAPI_REQUEST_TYPE_BLOCKED,
    USERLISTAPI_REQUEST_TYPE_FRIENDS_OF_FRIEND,
    USERLISTAPI_REQUEST_TYPE_MUTED,
    USERLISTAPI_REQUEST_TYPE_ISFRIENDS,
    USERLISTAPI_REQUEST_TYPE_ISBLOCKED
} UserListApiRequestTypeE;

typedef struct UserListRequestT
{
    struct UserListRequestT      *pNext;
    int64_t                       iRequestId;
    UserListApiCallbackT         *pNotifyCb;
    UserListApiIsAQueryCallbackT *pIsAQueryNotifyCb;
    UserListApiFiltersT           Filters;
    UserListApiRequestTypeE       eRequestType;
    int64_t                       iLimit;
    int64_t                       iOffset;
    uint32_t                      uUserIndex;
    uint32_t                      uTypeMask;
    void                         *pUserData;
    SceNpAccountId                TargetUserId;
} UserListRequestT;

typedef struct UserListApiNotificationT
{
    UserListApiUpdateCallbackT *pCallback;         //!< function address
    void                       *pUserData;         //!< user data to return
    uint32_t                    uUserIndex;        //!< user for whom the notification is destined
} UserListApiNotificationT;

typedef struct UserListApiNotifyEventT
{
    UserListApiNotifyDataT    *pNotificationData;
    UserListApiNotificationT (*pNotificationList)[];
    UserListApiNotifyTypeE     pNotificationType;
    uint32_t                   uUserIndex;
} UserListApiNotifyEventT;

//! module state
struct UserListApiRefT
{
    // module memory group
    int32_t iMemGroup;              //!< module mem group id
    void *pMemGroupUserData;        //!< user data associated with mem group

    uint32_t uMaxResponseSize;      //!< max size for http response

    NetCritT crit;                  //!< critical section used to ensure thread safety, used to synchornize 
                                    //   access to the RequestList, ActiveRequest and bDataReady flag

    int64_t  iCurrentWebRequest;    //!< request id used for the currently processed web request

    char *pReadBuffer;              //!< buffer for data from WebApi
    char strRawFriendData[USERLISTAPI_ONE_FRIEND_BUFFER_SIZE]; //!< buffer to hold a single friend's data
    int32_t iReadBufferSize;        //!< the number of "meaningful" bytes in the strReadBuffer buffer
    int32_t iReadError;             //!< flag to notify the user of an error

    uint8_t bDataReady;

    struct UserListRequestT *RequestList;  //!< list of pending queries to PSN
    struct UserListRequestT *ActiveRequest;
    DirtyWebApiRefT * pWebApi;
    
    // Callbacks
    UserListApiNotificationT FriendNotificaton[USERLISTAPI_NOTIFY_LIST_MAX_SIZE];
    UserListApiNotificationT BlockedNotificaton[USERLISTAPI_NOTIFY_LIST_MAX_SIZE];
    uint8_t bFriendNotificationStarted;
    uint8_t bBlockedNotificationStarted;
    UserListApiNotifyEventT UserListApiNotifyEvent[USERLISTAPI_MAX_QUEUED_NOTIFICATIONS];
};

/*** Function Prototypes ***************************************************************/

static void _DirtyWebApiCallback(int32_t iSceError, int32_t iUserIndex, int32_t iStatusCode, const char *pRespBody, int32_t iRespBodyLength, void *pUserData);
static int32_t _UserListApiParseResponse(UserListApiRefT *pRef, UserListRequestT *pRequest);
static int32_t _UserListApiParseIsResponse(UserListApiRefT *pRef, UserListRequestT *pRequest);
static int32_t _UserListApiAddAvatarToQuery(UserListApiRefT *pRef, UserListApiFiltersT *filter, char *pBuffer, uint32_t uBufferSize, uint32_t uTypeMask);
static int32_t _UserListApiAddContextToQuery(UserListApiRefT *pRef, UserListApiFiltersT *filter, char *pBuffer, uint32_t uBufferSize);
static int32_t _UserListApiAddOrderToQuery(UserListApiRefT *pRef, UserListApiFiltersT *filter, char *pBuffer, uint32_t uBufferSize);
static int32_t _UserListApiAddDirectionToQuery(UserListApiRefT *pRef, UserListApiFiltersT *filter, char *pBuffer, uint32_t uBufferSize);
static int32_t _UserListApiAddPresenceToQuery(UserListApiRefT *pRef, UserListApiFiltersT *filter, char *pBuffer, uint32_t uBufferSize, uint32_t uTypeMask);
static int32_t _UserListApiAddFieldsToQuery(UserListApiRefT *pRef, UserListApiFiltersT *filter, char *pBuffer, uint32_t uBufferSize, uint32_t uTypeMask, UserListApiRequestTypeE eRequestType);
static int32_t _UserListApiIsQuery(UserListApiRefT *pRef, uint32_t uUserIndex, UserListApiRequestTypeE eType, UserListApiIsAQueryCallbackT *pNotifyCb, SceNpAccountId TargetUserId, void *pUserData);
static int32_t _UserListApiQueuePeopleQuery(UserListApiRefT *pRef, uint32_t uUserIndex, UserListApiTypeE eType, int32_t iLimit,  int32_t iOffset, UserListApiFiltersT *filter, UserListApiCallbackT *pNotifyCb, uint32_t uTypeMask, SceNpAccountId accountId, void *pUserData);
static int32_t _UserListApiSendPeopleQuery(UserListApiRefT *pRef);
/*** Variables *************************************************************************/

/*** Private functions ******************************************************/

/*F*************************************************************************************************/
/*!
    \Function   _DirtyWebApiCallback

    \Description
        Callback from the PS4 Web Api

    \Input  iSceError       - error
    \Input  iUserIndex      - user index
    \Input  iStatusCode     - http status
    \Input *pRespBody       - data
    \Input *iRespBodyLength - data length
    \Input *pUserData       - UserListApiRefT

    \Version 04/17/2013 (amakoukji)
*/
/*************************************************************************************************F*/
void _DirtyWebApiCallback(int32_t iSceError, int32_t iUserIndex, int32_t iStatusCode, const char *pRespBody, int32_t iRespBodyLength, void *pUserData)
{
    UserListApiRefT *pRef  = (UserListApiRefT*)pUserData;

    if (iSceError == SCE_OK)
    {
        if (iRespBodyLength + 1 > pRef->iReadBufferSize)
        {
            char *pTmp;
            NetPrintf(("userlistapips4: [%p] resizing pReadBuffer from %d to %d bytes\n", pRef, pRef->iReadBufferSize, iRespBodyLength + 1));
            if ((pTmp = (char*)DirtyMemAlloc(iRespBodyLength + 1, USERLISTAPI_MEMID, pRef->iMemGroup, pRef->pMemGroupUserData)) == NULL)
            {
                NetPrintf(("userlistapips4: could not allocate space for the pReadBuffer\n"));
                pRef->iReadError = -1;
            }
            else
            {
                // note, we do NOT need to copy the contents over
                DirtyMemFree(pRef->pReadBuffer, USERLISTAPI_MEMID, pRef->iMemGroup, pRef->pMemGroupUserData);
                pRef->pReadBuffer = pTmp;
                pRef->iReadBufferSize = iRespBodyLength + 1;
            }
        }

        if (iRespBodyLength + 1 <= pRef->iReadBufferSize)
        {
            if ((iStatusCode >= 200 && iStatusCode < 300) ||
                (pRef->ActiveRequest->eRequestType == USERLISTAPI_REQUEST_TYPE_ISFRIENDS) || (pRef->ActiveRequest->eRequestType == USERLISTAPI_REQUEST_TYPE_ISBLOCKED))
            {
                if (iRespBodyLength > 0)
                {
                    // copy the response into the read buffer
                    ds_memcpy(pRef->pReadBuffer, pRespBody, iRespBodyLength);
                    pRef->pReadBuffer[iRespBodyLength] = '\0';
                    NetPrintf(("userlistapips4: WebApi returned status code (%d), %d characters read.\n", iStatusCode, iRespBodyLength));
                }
                else
                {
                    // I would never expect this to happen for these types of requests if the response code is 2xx
                    NetPrintf(("userlistapips4: WebApi warning, no response body was found.\n"));
                }
            }
            else
            {
                NetPrintf(("userlistapips4: WebApi returned an unexpected status code (%d).\n", iStatusCode));
                pRef->iReadError = -1;
            }
        }
    }
    else
    {
        NetPrintf(("userlistapips4: WebApi returned an SCE error code (%d).\n", iSceError));
        pRef->iReadError = iSceError;
    }

    // mark the process as done, the UserListApiUpdate() function will call the users callback as needed
    NetCritEnter(&pRef->crit);
    pRef->bDataReady = TRUE;
    NetCritLeave(&pRef->crit);
}

/*F*************************************************************************************************/
/*!
    \Function   _UserListApiParseResponse

    \Description
        Parse the people list response from PSN and trigger the callbacks

    \Input *pRef       - module ref
    \Input *pRequest   - local request

    \Output
        int32_t     - 0 for success, negative for error

    \Notes
        This function is temporarily using a large amount of stack to parse the 
        whole JSON message. In the future we will need to parse the JSON a few 
        k at a time in order to be sure to be able to process the whole message.

    \Version 04/17/2013 (amakoukji)
*/
/*************************************************************************************************F*/
int32_t _UserListApiParseResponse(UserListApiRefT *pRef, UserListRequestT *pRequest)
{
    // control and buffer variables
    int32_t iReturn = 0;
    int32_t iReadOffset = 0;
    char    strTmp[64];
    char    strCurrentTitle[SCE_NP_TITLE_ID_LEN + 1];

    // variables for parsing
    int64_t iTotalResults = -1;
    int64_t iListOffset = -1;
    int64_t iSize = -1;
    uint16_t *pJsonParse;

    // Native id variables
    char strNativeId[DIRTYUSER_NATIVEUSER_MAXSIZE];
    SceNpAccountId accountId;

    // friend data for callback
    UserListApiEventDataT data;

    // user data for callback
    UserApiUserDataT     *pUserData = &data.UserData.ExtendedUserData;

    NetCritEnter(&pRef->crit);
    if (pRef->bDataReady == FALSE)
    {
        NetCritLeave(&pRef->crit);
        return(0);
    }
    NetCritLeave(&pRef->crit);

    if ((pJsonParse = (uint16_t*)DirtyMemAlloc(pRef->iReadBufferSize * sizeof(pJsonParse[0]), USERLISTAPI_MEMID, pRef->iMemGroup, pRef->pMemGroupUserData)) == NULL)
    {
        NetPrintf(("userlistapips4: could not allocate space for the Json parser\n"));
        ds_memclr(&data, sizeof(data));
        switch (pRequest->eRequestType)
        {
            case USERLISTAPI_REQUEST_TYPE_FRIENDS:
                data.ListEndData.eRequestType = USERLISTAPI_TYPE_FRIENDS;
                break;
            case USERLISTAPI_REQUEST_TYPE_BLOCKED:
                data.ListEndData.eRequestType = USERLISTAPI_TYPE_BLOCKED;
                break;
            case USERLISTAPI_REQUEST_TYPE_FRIENDS_OF_FRIEND:
                data.ListEndData.eRequestType = USERLISTAPI_TYPE_FRIENDS_OF_FRIEND;
                break;
            case USERLISTAPI_REQUEST_TYPE_MUTED:
                data.ListEndData.eRequestType = USERLISTAPI_TYPE_MUTED;
                break;
            case USERLISTAPI_REQUEST_TYPE_ISFRIENDS:
            case USERLISTAPI_REQUEST_TYPE_ISBLOCKED:
                NetPrintf(("userlistapips4: unexpected request type.\n"));
                break;
        }
        data.ListEndData.pUserListFilters = &pRequest->Filters;
        data.ListEndData.uUserIndex = pRequest->uUserIndex;
        data.ListEndData.uTypeMask = pRequest->uTypeMask;
        data.ListEndData.Error = -1;
        pRequest->pNotifyCb(pRef, TYPE_LIST_END, &data, pRequest->pUserData);
        return(-1);
    }

    // update read offset
    iReadOffset += iReturn;

    const char *pCurrent = NULL;
    const char *pFriendList = NULL;
    JsonParse(pJsonParse, pRef->iReadBufferSize, pRef->pReadBuffer, pRef->iReadBufferSize);

    // Extract top level metadata
    pCurrent = JsonFind(pJsonParse, "start");
    if (pCurrent == NULL)
    {
        ds_memclr(&data, sizeof(data));
        switch (pRequest->eRequestType)
        {
            case USERLISTAPI_REQUEST_TYPE_FRIENDS:
                data.ListEndData.eRequestType = USERLISTAPI_TYPE_FRIENDS;
                break;
            case USERLISTAPI_REQUEST_TYPE_BLOCKED:
                data.ListEndData.eRequestType = USERLISTAPI_TYPE_BLOCKED;
                break;
            case USERLISTAPI_REQUEST_TYPE_FRIENDS_OF_FRIEND:
                data.ListEndData.eRequestType = USERLISTAPI_TYPE_FRIENDS_OF_FRIEND;
                break;
            case USERLISTAPI_REQUEST_TYPE_MUTED:
                data.ListEndData.eRequestType = USERLISTAPI_TYPE_MUTED;
                break;
            case USERLISTAPI_REQUEST_TYPE_ISFRIENDS:
            case USERLISTAPI_REQUEST_TYPE_ISBLOCKED:
                NetPrintf(("userlistapips4: unexpected request type.\n"));
                break;
        }
        data.ListEndData.pUserListFilters = &pRequest->Filters;
        data.ListEndData.uUserIndex = pRequest->uUserIndex;
        data.ListEndData.uTypeMask = pRequest->uTypeMask;
        data.ListEndData.Error = -1;
        pRequest->pNotifyCb(pRef, TYPE_LIST_END, &data, pRequest->pUserData);
        DirtyMemFree(pJsonParse, USERLISTAPI_MEMID, pRef->iMemGroup, pRef->pMemGroupUserData);
        return(-1);
    }
    iListOffset = JsonGetInteger(pCurrent, 0);

    pCurrent = JsonFind(pJsonParse, "size");
    if (pCurrent == NULL)
    {
        ds_memclr(&data, sizeof(data));
        switch (pRequest->eRequestType)
        {
            case USERLISTAPI_REQUEST_TYPE_FRIENDS:
                data.ListEndData.eRequestType = USERLISTAPI_TYPE_FRIENDS;
                break;
            case USERLISTAPI_REQUEST_TYPE_BLOCKED:
                data.ListEndData.eRequestType = USERLISTAPI_TYPE_BLOCKED;
                break;
            case USERLISTAPI_REQUEST_TYPE_FRIENDS_OF_FRIEND:
                data.ListEndData.eRequestType = USERLISTAPI_TYPE_FRIENDS_OF_FRIEND;
                break;
            case USERLISTAPI_REQUEST_TYPE_MUTED:
                data.ListEndData.eRequestType = USERLISTAPI_TYPE_MUTED;
                break;
            case USERLISTAPI_REQUEST_TYPE_ISFRIENDS:
            case USERLISTAPI_REQUEST_TYPE_ISBLOCKED:
                NetPrintf(("userlistapips4: unexpected request type.\n"));
                break;
        }
        data.ListEndData.pUserListFilters = &pRequest->Filters;
        data.ListEndData.uUserIndex = pRequest->uUserIndex;
        data.ListEndData.uTypeMask = pRequest->uTypeMask;
        data.ListEndData.Error = -1;
        pRequest->pNotifyCb(pRef, TYPE_LIST_END, &data, pRequest->pUserData);
        DirtyMemFree(pJsonParse, USERLISTAPI_MEMID, pRef->iMemGroup, pRef->pMemGroupUserData);
        return(-1);
    }
    iSize = JsonGetInteger(pCurrent, 0);

    pCurrent = JsonFind(pJsonParse, "totalResults");
    if (pCurrent == NULL)
    {
        ds_memclr(&data, sizeof(data));
        switch (pRequest->eRequestType)
        {
            case USERLISTAPI_REQUEST_TYPE_FRIENDS:
                data.ListEndData.eRequestType = USERLISTAPI_TYPE_FRIENDS;
                break;
            case USERLISTAPI_REQUEST_TYPE_BLOCKED:
                data.ListEndData.eRequestType = USERLISTAPI_TYPE_BLOCKED;
                break;
            case USERLISTAPI_REQUEST_TYPE_FRIENDS_OF_FRIEND:
                data.ListEndData.eRequestType = USERLISTAPI_TYPE_FRIENDS_OF_FRIEND;
                break;
            case USERLISTAPI_REQUEST_TYPE_MUTED:
                data.ListEndData.eRequestType = USERLISTAPI_TYPE_MUTED;
                break;
            case USERLISTAPI_REQUEST_TYPE_ISFRIENDS:
            case USERLISTAPI_REQUEST_TYPE_ISBLOCKED:
                NetPrintf(("userlistapips4: unexpected request type.\n"));
                break;
        }
        data.ListEndData.pUserListFilters = &pRequest->Filters;
        data.ListEndData.uUserIndex = pRequest->uUserIndex;
        data.ListEndData.Error = -1;
        pRequest->pNotifyCb(pRef, TYPE_LIST_END, &data, pRequest->pUserData);
        DirtyMemFree(pJsonParse, USERLISTAPI_MEMID, pRef->iMemGroup, pRef->pMemGroupUserData);
        return(-1);
    }
    iTotalResults = JsonGetInteger(pCurrent, 0);

    // Extract onlineId for each friend in the list
    // and report the callback
    for (int64_t iElement = 0; iElement < iSize; ++iElement)
    {
        const char *pRawFriendEnd = NULL;
        int32_t iRes = 0;

        // Clear the data object for the callback
        ds_memclr(&data, sizeof(data));

        // Extract onlineId
        pFriendList = JsonFind2(pJsonParse, NULL, (pRequest->eRequestType == (USERLISTAPI_TYPE_FRIENDS) || pRequest->eRequestType == (USERLISTAPI_TYPE_FRIENDS_OF_FRIEND)) ? "friendList[" : "blockingUsers[", iElement);
        if (pFriendList == NULL)
        {
            ds_memclr(&data, sizeof(data));
            switch (pRequest->eRequestType)
            {
                case USERLISTAPI_REQUEST_TYPE_FRIENDS:
                    data.ListEndData.eRequestType = USERLISTAPI_TYPE_FRIENDS;
                    break;
                case USERLISTAPI_REQUEST_TYPE_BLOCKED:
                    data.ListEndData.eRequestType = USERLISTAPI_TYPE_BLOCKED;
                    break;
                case USERLISTAPI_REQUEST_TYPE_FRIENDS_OF_FRIEND:
                    data.ListEndData.eRequestType = USERLISTAPI_TYPE_FRIENDS_OF_FRIEND;
                    break;
                case USERLISTAPI_REQUEST_TYPE_MUTED:
                    data.ListEndData.eRequestType = USERLISTAPI_TYPE_MUTED;
                    break;
                case USERLISTAPI_REQUEST_TYPE_ISFRIENDS:
                case USERLISTAPI_REQUEST_TYPE_ISBLOCKED:
                    NetPrintf(("userlistapips4: unexpected request type.\n"));
                    break;
            }
            data.ListEndData.pUserListFilters = &pRequest->Filters;
            data.ListEndData.uUserIndex = pRequest->uUserIndex;
            data.ListEndData.uTypeMask = pRequest->uTypeMask;
            data.ListEndData.Error = -1;
            pRequest->pNotifyCb(pRef, TYPE_LIST_END, &data, pRequest->pUserData);
            DirtyMemFree(pJsonParse, USERLISTAPI_MEMID, pRef->iMemGroup, pRef->pMemGroupUserData);
            return(-1);
        }

        pCurrent    = JsonFind2(pJsonParse, pFriendList, ".user.accountId", iElement);
        iRes = JsonGetString(pCurrent, strNativeId, sizeof(strNativeId), "");
        if (iRes >= 0)
        {
            accountId = ds_strtoull(strNativeId, NULL, 10);
        }

        if (iRes < 0)
        {
            NetPrintf(("userlistapips4: cannot parse npId or extract account id from npId.\n" ));
            ds_memclr(&data, sizeof(data));
            switch (pRequest->eRequestType)
            {
                case USERLISTAPI_REQUEST_TYPE_FRIENDS:
                    data.ListEndData.eRequestType = USERLISTAPI_TYPE_FRIENDS;
                    break;
                case USERLISTAPI_REQUEST_TYPE_BLOCKED:
                    data.ListEndData.eRequestType = USERLISTAPI_TYPE_BLOCKED;
                    break;
                case USERLISTAPI_REQUEST_TYPE_FRIENDS_OF_FRIEND:
                    data.ListEndData.eRequestType = USERLISTAPI_TYPE_FRIENDS_OF_FRIEND;
                    break;
                case USERLISTAPI_REQUEST_TYPE_MUTED:
                    data.ListEndData.eRequestType = USERLISTAPI_TYPE_MUTED;
                    break;
                case USERLISTAPI_REQUEST_TYPE_ISFRIENDS:
                case USERLISTAPI_REQUEST_TYPE_ISBLOCKED:
                    NetPrintf(("userlistapips4: unexpected request type.\n"));
                    break;
            }
            data.ListEndData.pUserListFilters = &pRequest->Filters;
            data.ListEndData.uUserIndex = pRequest->uUserIndex;
            data.ListEndData.uTypeMask = pRequest->uTypeMask;
            data.ListEndData.Error = -1;
            pRequest->pNotifyCb(pRef, TYPE_LIST_END, &data, pRequest->pUserData);
            DirtyMemFree(pJsonParse, USERLISTAPI_MEMID, pRef->iMemGroup, pRef->pMemGroupUserData);
            return(-1);
        }

        DirtyUserFromNativeUser(&data.UserData.DirtyUser, &accountId);
        ds_memcpy_s(&data.UserData.ExtendedUserData.DirtyUser, sizeof(data.UserData.ExtendedUserData.DirtyUser), &data.UserData.DirtyUser, sizeof(data.UserData.DirtyUser));

        if (pRequest->uTypeMask & USERLISTAPI_MASK_PROFILE)
        {
            // Raw data points to the start of the friend
            pUserData->Profile.pRawData = pRef->strRawFriendData;

            // Get the avatar url
            pCurrent    = JsonFind2(pJsonParse, pFriendList, ".avatarUrl", iElement);
            JsonGetString(pCurrent, pUserData->Profile.strAvatarUrl, sizeof(pUserData->Profile.strAvatarUrl), "");

            // Copy the gamertag 
            pCurrent    = JsonFind2(pJsonParse, pFriendList, ".user.onlineId", iElement);
            JsonGetString(pCurrent, pUserData->Profile.strGamertag, sizeof(pUserData->Profile.strGamertag), "");

            // Get the country code. Note the language is not present
            pCurrent    = JsonFind2(pJsonParse, pFriendList, ".region", iElement);
            JsonGetString(pCurrent, strTmp, sizeof(strTmp), "");
            pUserData->Profile.uLocale = (uint32_t)(strTmp[0] << 8) + (uint32_t)(strTmp[1]);

            // Get the Display name if present, otherwise copy the gamertag
            pCurrent = JsonFind2(pJsonParse, pFriendList, ".personalDetail", iElement);
            if (pCurrent)
            {
                strTmp[0] = '\0';
                pCurrent = JsonFind2(pJsonParse, pFriendList, ".personalDetail.firstName", iElement);
                JsonGetString(pCurrent, strTmp, sizeof(strTmp), "");
                ds_strnzcat(pUserData->Profile.strDisplayName, strTmp, sizeof(pUserData->Profile.strDisplayName));
                pCurrent = JsonFind2(pJsonParse, pFriendList, ".personalDetail.lastName", iElement);
                JsonGetString(pCurrent, strTmp, sizeof(strTmp), "");
                ds_strnzcat(pUserData->Profile.strDisplayName, " ", sizeof(pUserData->Profile.strDisplayName));
                ds_strnzcat(pUserData->Profile.strDisplayName, strTmp, sizeof(pUserData->Profile.strDisplayName));
            }
            else
            {
                ds_strnzcpy(pUserData->Profile.strDisplayName, pUserData->Profile.strGamertag, SCE_NP_ONLINEID_MAX_LENGTH+1);
            }
        }

        if (pRequest->uTypeMask & USERLISTAPI_MASK_PRESENCE)
        {
            pCurrent = JsonFind2(pJsonParse, pFriendList, ".presence.primaryInfo.onlineStatus", iElement);
            JsonGetString(pCurrent, strTmp, sizeof(strTmp), "");
            if (ds_stricmp(strTmp, "online") == 0)
            {
                pUserData->Presence.ePresenceStatus = USERAPI_PRESENCE_ONLINE;
                
            }
            else if  (ds_stricmp(strTmp, "offline") == 0)
            {
                pUserData->Presence.ePresenceStatus = USERAPI_PRESENCE_OFFLINE;
            }
            else
            {
                pUserData->Presence.ePresenceStatus = USERAPI_PRESENCE_UNKNOWN;
            }

            pCurrent = JsonFind2(pJsonParse, pFriendList, ".presence.primaryInfo.gameTitleInfo.npTitleId", iElement);
            JsonGetString(pCurrent, pUserData->Presence.strTitleId, sizeof(pUserData->Presence.strTitleId), "");

            pCurrent = JsonFind2(pJsonParse, pFriendList, ".presence.primaryInfo.gameTitleInfo.titleName", iElement);
            JsonGetString(pCurrent, pUserData->Presence.strTitleName, sizeof(pUserData->Presence.strTitleName), "");

            pCurrent = JsonFind2(pJsonParse, pFriendList, ".presence.primaryInfo.platform", iElement);
            JsonGetString(pCurrent, pUserData->Presence.strPlatform, sizeof(pUserData->Presence.strPlatform), "");

            // check if this is the same title id as the user is currently playing
            NetConnStatus('titl', pRequest->uUserIndex, strCurrentTitle, sizeof(strCurrentTitle));
            if (ds_stricmp(strCurrentTitle, pUserData->Presence.strTitleId) == 0)
            {
                pUserData->Presence.bIsPlayingSameTitle = TRUE;
            }
            else
            {
                pUserData->Presence.bIsPlayingSameTitle = FALSE;
            }

            pUserData->Presence.pRawData = pRef->strRawFriendData;
        }

        if (pRequest->uTypeMask & USERLISTAPI_MASK_RICH_PRESENCE)
        {
            pCurrent = JsonFind2(pJsonParse, pFriendList, ".presence.primaryInfo.gameStatus", iElement);
            JsonGetString(pCurrent, pUserData->RichPresence.strData, sizeof(pUserData->RichPresence.strData), "");

            pCurrent = JsonFind2(pJsonParse, pFriendList, ".presence.primaryInfo.gameData", iElement);
            JsonGetString(pCurrent, pUserData->RichPresence.strGameData, sizeof(pUserData->RichPresence.strGameData), "");

            pUserData->RichPresence.pRawData = pRef->strRawFriendData;
        }

        // Copy the raw data into the raw data buffer
        pRawFriendEnd = JsonGetListItemEnd(pFriendList);
        if (pRawFriendEnd == NULL)
        {
            NetPrintf(("userlistapips4: Invalid JSON detected.\n"));
            ds_memclr(&data, sizeof(data));
            switch (pRequest->eRequestType)
            {
                case USERLISTAPI_REQUEST_TYPE_FRIENDS:
                    data.ListEndData.eRequestType = USERLISTAPI_TYPE_FRIENDS;
                    break;
                case USERLISTAPI_REQUEST_TYPE_BLOCKED:
                    data.ListEndData.eRequestType = USERLISTAPI_TYPE_BLOCKED;
                    break;
                case USERLISTAPI_REQUEST_TYPE_FRIENDS_OF_FRIEND:
                    data.ListEndData.eRequestType = USERLISTAPI_TYPE_FRIENDS_OF_FRIEND;
                    break;
                case USERLISTAPI_REQUEST_TYPE_MUTED:
                    data.ListEndData.eRequestType = USERLISTAPI_TYPE_MUTED;
                    break;
                case USERLISTAPI_REQUEST_TYPE_ISFRIENDS:
                case USERLISTAPI_REQUEST_TYPE_ISBLOCKED:
                    NetPrintf(("userlistapips4: unexpected request type.\n"));
                    break;
            }
            data.ListEndData.pUserListFilters = &pRequest->Filters;
            data.ListEndData.uUserIndex = pRequest->uUserIndex;
            data.ListEndData.uTypeMask = pRequest->uTypeMask;
            data.ListEndData.Error = -1;
            pRequest->pNotifyCb(pRef, TYPE_LIST_END, &data, pRequest->pUserData);
            DirtyMemFree(pJsonParse, USERLISTAPI_MEMID, pRef->iMemGroup, pRef->pMemGroupUserData);
            return(-1);
        }
        ds_strnzcpy(pRef->strRawFriendData, pFriendList, pRawFriendEnd - pFriendList + 1);

        // Populate other data
        switch (pRequest->eRequestType)
        {
            case USERLISTAPI_REQUEST_TYPE_FRIENDS:
                data.UserData.eRequestType = USERLISTAPI_TYPE_FRIENDS;
                break;
            case USERLISTAPI_REQUEST_TYPE_BLOCKED:
                data.UserData.eRequestType = USERLISTAPI_TYPE_BLOCKED;
                break;
            case USERLISTAPI_REQUEST_TYPE_FRIENDS_OF_FRIEND:
                data.UserData.eRequestType = USERLISTAPI_TYPE_FRIENDS_OF_FRIEND;
                break;
            case USERLISTAPI_REQUEST_TYPE_MUTED:
                data.UserData.eRequestType = USERLISTAPI_TYPE_MUTED;
                break;
            case USERLISTAPI_REQUEST_TYPE_ISFRIENDS:
            case USERLISTAPI_REQUEST_TYPE_ISBLOCKED:
                NetPrintf(("userlistapips4: unexpected request type.\n"));
                break;
        }
        data.UserData.pUserListFilters         = &pRequest->Filters;
        data.UserData.uUserIndex               = pRequest->uUserIndex;
        data.UserData.pRawData                 = pRef->strRawFriendData;
        data.UserData.ExtendedUserData.uUserDataMask = pRequest->uTypeMask;
        data.UserData.bIsFavorite              = FALSE; // no such concept on PS4
        data.UserData.bIsMutualFriend          = TRUE;  // always mutual friend on PS4
                
        // send the callback for a single user
        pRequest->pNotifyCb(pRef, TYPE_USER_DATA, &data, pRequest->pUserData);
    }  

    // Report metadata in the last event
    ds_memclr(&data, sizeof(data));
    switch (pRequest->eRequestType)
    {
        case USERLISTAPI_REQUEST_TYPE_FRIENDS:
            data.ListEndData.eRequestType = USERLISTAPI_TYPE_FRIENDS;
            break;
        case USERLISTAPI_REQUEST_TYPE_BLOCKED:
            data.ListEndData.eRequestType = USERLISTAPI_TYPE_BLOCKED;
            break;
        case USERLISTAPI_REQUEST_TYPE_FRIENDS_OF_FRIEND:
            data.ListEndData.eRequestType = USERLISTAPI_TYPE_FRIENDS_OF_FRIEND;
            break;
        case USERLISTAPI_REQUEST_TYPE_MUTED:
            data.ListEndData.eRequestType = USERLISTAPI_TYPE_MUTED;
            break;
        case USERLISTAPI_REQUEST_TYPE_ISFRIENDS:
        case USERLISTAPI_REQUEST_TYPE_ISBLOCKED:
            NetPrintf(("userlistapips4: unexpected request type.\n"));
            break;
    }
    data.ListEndData.pUserListFilters  = &pRequest->Filters;
    data.ListEndData.uUserIndex        = pRequest->uUserIndex;
    data.ListEndData.uTypeMask         = pRequest->uTypeMask;
    data.ListEndData.iLimit            = pRequest->iLimit;
    data.ListEndData.iOffset           = pRequest->iOffset;
    data.ListEndData.iTotalFriendCount = iTotalResults; 
                
    // send the callback for the list end event
    pRequest->pNotifyCb(pRef, TYPE_LIST_END, &data, pRequest->pUserData);

    DirtyMemFree(pJsonParse, USERLISTAPI_MEMID, pRef->iMemGroup, pRef->pMemGroupUserData);

    return(0);
}

/*F*************************************************************************************************/
/*!
    \Function   _UserListApiParseIsResponse

    \Description
        Parse relationship results

    \Input *pRef       - module ref
    \Input *pRequest   - local request

    \Output
        int32_t         - 0 for success, negative for error

    \Version 06/24/2013 (tcho)

    \Notes
*/
/*************************************************************************************************F*/
int32_t _UserListApiParseIsResponse(UserListApiRefT *pRef, UserListRequestT *pRequest)
{
    // variables for parsing
    uint16_t *pJsonParse1;
    uint16_t *pJsonParse2;

    // Native id variables
    char strRelation[1024];
    SceNpAccountId TargetUserId;

    //Error Code from response
    uint32_t iCode;

    // friend data for callback
    UserListApiIsADataT data;

    // prepare the data
    ds_memclr(&data, sizeof(data));
    TargetUserId = pRequest->TargetUserId;
    DirtyUserFromNativeUser(&data.DirtyUser, &TargetUserId);
    data.uUserIndex      = pRequest->uUserIndex;
    data.bIsFavorite     = FALSE; // no such concept on PS4
    data.bIsMutualFriend = TRUE;  // always mutual friend on PS4

    NetCritEnter(&pRef->crit);
    if (pRef->bDataReady == FALSE)
    {
        NetCritLeave(&pRef->crit);
        return(0);
    }
    NetCritLeave(&pRef->crit);

    if ((pJsonParse1 = (uint16_t*)DirtyMemAlloc(pRef->iReadBufferSize * sizeof(pJsonParse1[0]), USERLISTAPI_MEMID, pRef->iMemGroup, pRef->pMemGroupUserData)) == NULL)
    {
        NetPrintf(("userlistapips4: could not allocate space for the Json parser\n"));
        data.Error = -1;     
        pRequest->pIsAQueryNotifyCb(pRef, (pRef->ActiveRequest->eRequestType == USERLISTAPI_REQUEST_TYPE_ISFRIENDS ? USERLISTAPI_IS_FRIENDS: USERLISTAPI_IS_BLOCKED), &data, pRequest->pUserData);
        return(-1);
    }

    if ((pJsonParse2 = (uint16_t*)DirtyMemAlloc(pRef->iReadBufferSize * sizeof(pJsonParse2[0]), USERLISTAPI_MEMID, pRef->iMemGroup, pRef->pMemGroupUserData)) == NULL)
    {
        NetPrintf(("userlistapips4: could not allocate space for the Json parser\n"));
        data.Error = -1;     
        pRequest->pIsAQueryNotifyCb(pRef, (pRef->ActiveRequest->eRequestType == USERLISTAPI_REQUEST_TYPE_ISFRIENDS ? USERLISTAPI_IS_FRIENDS: USERLISTAPI_IS_BLOCKED), &data, pRequest->pUserData);
        DirtyMemFree(pJsonParse1, USERLISTAPI_MEMID, pRef->iMemGroup, pRef->pMemGroupUserData);
        return(-1);
    }

    const char *pCurrent = NULL;
    //const char *pFriendList = NULL;
    JsonParse(pJsonParse1, pRef->iReadBufferSize, pRef->pReadBuffer, pRef->iReadBufferSize);

    //Checking for error code
    pCurrent = JsonFind(pJsonParse1, "error");
    if (pCurrent != NULL)
    {
        JsonParse(pJsonParse2, pRef->iReadBufferSize, pCurrent, pRef->iReadBufferSize);

        pCurrent = JsonFind(pJsonParse2, "code");
        if (pCurrent == NULL)
        {
            NetPrintf(("userlistapips4: UserListApiParseIsFriendResponse() unknown error.\n"));
            data.Error = -1;     
            pRequest->pIsAQueryNotifyCb(pRef, (pRef->ActiveRequest->eRequestType == USERLISTAPI_REQUEST_TYPE_ISFRIENDS ? USERLISTAPI_IS_FRIENDS: USERLISTAPI_IS_BLOCKED), &data, pRequest->pUserData);
            DirtyMemFree(pJsonParse1, USERLISTAPI_MEMID, pRef->iMemGroup, pRef->pMemGroupUserData);
            DirtyMemFree(pJsonParse2, USERLISTAPI_MEMID, pRef->iMemGroup, pRef->pMemGroupUserData);
            return (-1);
        }
        iCode = JsonGetInteger(pCurrent, 0);

        if (iCode == USERLISTAPI_USER_NOT_FOUND_ERROR_CODE)
        {
            //Since xboxone doesn't support target user do not exist 
            //User not found in ps4 is treated as not a friend
            data.eIsaType = USERLISTAPI_IS_NOT_OF_TYPE;
            NetPrintf(("userlistapips4: UserListApiParseIsFriendResponse() User: %llx not found.\n", pRequest->TargetUserId));
        }
        else
        {
            //Unknown friend status
            data.eIsaType = USERLISTAPI_IS_TYPE_UNKNOWN;
            NetPrintf(("userlistapips4: UserListApiParseIsFriendResponse() Unknown response error code.\n"));
        }
    }
    else
    {
        pCurrent = JsonFind(pJsonParse1, "relation");
        if (pCurrent == NULL)
        {
            NetPrintf(("userlistapips4: UserListApiParseIsFriendResponse() unknown error.\n"));
            data.Error = -1;     
            pRequest->pIsAQueryNotifyCb(pRef, (pRef->ActiveRequest->eRequestType == USERLISTAPI_REQUEST_TYPE_ISFRIENDS ? USERLISTAPI_IS_FRIENDS: USERLISTAPI_IS_BLOCKED), &data, pRequest->pUserData);
            DirtyMemFree(pJsonParse1, USERLISTAPI_MEMID, pRef->iMemGroup, pRef->pMemGroupUserData);
            DirtyMemFree(pJsonParse2, USERLISTAPI_MEMID, pRef->iMemGroup, pRef->pMemGroupUserData);
            return(-1);
        }
        JsonGetString(pCurrent, strRelation, sizeof(strRelation), "Failed");

        if (pRef->ActiveRequest->eRequestType == USERLISTAPI_REQUEST_TYPE_ISFRIENDS)
        {
            if (strcmp(strRelation, "friend") == 0)
            {
                //Target user is a friend
                data.eIsaType = USERLISTAPI_IS_OF_TYPE;
                NetPrintf(("userlistapips4: UserListApiParseIsFriendResponse() User: %llx is a friend.\n", pRequest->TargetUserId));
            }
            else if (strcmp(strRelation, "me") == 0)
            {
                //Target user is myself
                data.eIsaType = USERLISTAPI_IS_MYSELF;
                NetPrintf(("userlistapips4: UserListApiParseIsFriendResponse() User: %llx is myself.\n", pRequest->TargetUserId));
            }
            else
            {
                //Other relationships are not supported and is treated as not a friend
                data.eIsaType = USERLISTAPI_IS_NOT_OF_TYPE;
                NetPrintf(("userlistapips4: UserListApiParseIsFriendResponse() User: %llx is not a friend.\n", pRequest->TargetUserId));
            }
        }
        else if (pRef->ActiveRequest->eRequestType == USERLISTAPI_REQUEST_TYPE_ISBLOCKED)
        {
            if (strcmp(strRelation, "blocked") == 0)
            {
                //Target user is a friend
                data.eIsaType = USERLISTAPI_IS_OF_TYPE;
                NetPrintf(("userlistapips4: UserListApiParseIsFriendResponse() User: %llx is blocked.\n", pRequest->TargetUserId));
            }
            else if (strcmp(strRelation, "me") == 0)
            {
                //Target user is myself
                data.eIsaType = USERLISTAPI_IS_MYSELF;
                NetPrintf(("userlistapips4: UserListApiParseIsFriendResponse() User: %llx is myself.\n", pRequest->TargetUserId));
            }
            else
            {
                //Other relationships are not supported and is treated as not a friend
                data.eIsaType = USERLISTAPI_IS_NOT_OF_TYPE;
                NetPrintf(("userlistapips4: UserListApiParseIsFriendResponse() User: %llx is not blocked.\n", pRequest->TargetUserId));
            }
        }
    }

    if (pRequest->pIsAQueryNotifyCb != NULL)
    {
        data.Error = 0;  
        pRequest->pIsAQueryNotifyCb(pRef, (pRef->ActiveRequest->eRequestType == USERLISTAPI_REQUEST_TYPE_ISFRIENDS ? USERLISTAPI_IS_FRIENDS: USERLISTAPI_IS_BLOCKED), &data, pRequest->pUserData);
    }

    DirtyMemFree(pJsonParse1, USERLISTAPI_MEMID, pRef->iMemGroup, pRef->pMemGroupUserData);
    DirtyMemFree(pJsonParse2, USERLISTAPI_MEMID, pRef->iMemGroup, pRef->pMemGroupUserData);

    return(0);
}

/*F*************************************************************************************************/
/*!
    \Function   _UserListApiAddAvatarToQuery

    \Description
        Add the avatar url selection data to the people query

    \Input *pRef        - module ref
    \Input *filter      - query filters
    \Input *pBuffer     - [out] buffer where the request is stored
    \Input uBufferSize  - size of the buffer 
    \Input uTypeMask    - mask

    \Output
        int32_t         - 0 for success, negative for error

    \Version 04/17/2013 (amakoukji)
*/
/*************************************************************************************************F*/
int32_t _UserListApiAddAvatarToQuery(UserListApiRefT *pRef, UserListApiFiltersT *filter, char *pBuffer, uint32_t uBufferSize, uint32_t uTypeMask)
{
    int32_t iReturn = 0;

    // Handle avatar size
    if (filter->eAvatarSize != USERLISTAPI_AVATAR_NONE)
    {
        iReturn = ds_strnzcat(pBuffer, "&avatarSize=", uBufferSize);

        if (iReturn < 0)
        {
             return(iReturn);
        }

        switch (filter->eAvatarSize)
        {
            case USERLISTAPI_AVATAR_SMALL:
                iReturn = ds_strnzcat(pBuffer, "s", uBufferSize);
                break;
            case USERLISTAPI_AVATAR_MEDIUM:
                iReturn = ds_strnzcat(pBuffer, "m", uBufferSize);
                break;
            case USERLISTAPI_AVATAR_LARGE:
            default:
                iReturn = ds_strnzcat(pBuffer, "l", uBufferSize);
                break;
        }
    }
    else if (uTypeMask & USERLISTAPI_MASK_PROFILE)
    {
        // assume large
        iReturn = ds_strnzcat(pBuffer, "&avatarSize=l", uBufferSize);
    }

    return(iReturn);
}

/*F*************************************************************************************************/
/*!
    \Function _UserListApiAddContextToQuery

    \Description
        Add the context selection data to the people query

    \Input *pRef        - module ref
    \Input *filter      - query filters
    \Input *pBuffer     - [out] buffer where the request is stored
    \Input uBufferSize  - size of the buffer 

    \Output
        int32_t         - 0 for success, negative for error

    \Version 04/17/2013 (amakoukji)
*/
/*************************************************************************************************F*/
int32_t _UserListApiAddContextToQuery(UserListApiRefT *pRef, UserListApiFiltersT *filter, char *pBuffer, uint32_t uBufferSize)
{
    int32_t iReturn = 0;

    if (filter->eContext != USERLISTAPI_CONTEXT_ALL)
    {
        iReturn = ds_strnzcat(pBuffer, "&filter=", uBufferSize);

        if (iReturn < 0)
        {
             return(iReturn);
        }

        switch (filter->eContext)
        {
            case USERLISTAPI_CONTEXT_ONLINE:
                iReturn = ds_strnzcat(pBuffer, "online", uBufferSize);
                break;
            case USERLISTAPI_CONTEXT_INCONTEXT:
            default:
                iReturn = ds_strnzcat(pBuffer, "incontext", uBufferSize);
                break;
        }
    }

    return(iReturn);
}

/*F*************************************************************************************************/
/*!
    \Function   _UserListApiAddOrderToQuery

    \Description
        Add the orderby selection data to the people query

    \Input *pRef        - module ref
    \Input *filter      - query filters
    \Input *pBuffer     - [out] buffer where the request is stored
    \Input uBufferSize  - size of the buffer 

    \Output
        int32_t         - 0 for success, negative for error

    \Version 04/17/2013 (amakoukji)
*/
/*************************************************************************************************F*/
int32_t _UserListApiAddOrderToQuery(UserListApiRefT *pRef, UserListApiFiltersT *filter, char *pBuffer, uint32_t uBufferSize)
{
    int32_t iReturn = 0;

    if (filter->eOrder != USERLISTAPI_ORDERBY_NONE)
    {
        iReturn = ds_strnzcat(pBuffer, "&sort=", uBufferSize);
        if (iReturn < 0)
        {
             return(iReturn);
        }

        switch (filter->eOrder)
        {
            case USERLISTAPI_ORDERBY_ID:
                iReturn = ds_strnzcat(pBuffer, "onlineId", uBufferSize);
                break;
            case USERLISTAPI_ORDERBY_STATUS:
                iReturn = ds_strnzcat(pBuffer, "onlineStatus", uBufferSize);
                break;
            case USERLISTAPI_ORDERBY_ID_AND_STATUS:
            default:
                iReturn = ds_strnzcat(pBuffer, "onlineStatus+onlineId", uBufferSize);
                break;
        }

        _UserListApiAddDirectionToQuery(pRef, filter, pBuffer, uBufferSize);
    }

    return(iReturn);
}

/*F*************************************************************************************************/
/*!
\Function   _UserListApiAddDirectionToQuery

\Description
Add order direction data to the people query (only matter if the order is specified)

\Input *pRef        - module ref
\Input *filter      - query filters
\Input *pBuffer     - [out] buffer where the request is stored
\Input uBufferSize  - size of the buffer

\Output
int32_t         - 0 for success, negative for error

\Version 01/09/2018 (tcho)
*/
/*************************************************************************************************F*/
int32_t _UserListApiAddDirectionToQuery(UserListApiRefT *pRef, UserListApiFiltersT *filter, char *pBuffer, uint32_t uBufferSize)
{
    int32_t iReturn = ds_strnzcat(pBuffer, "&direction=", uBufferSize);

    if (iReturn < 0)
    {
        return(iReturn);
    }
    else if (filter->eDirection == USERLISTAPI_DIRECTION_ASCENDING)
    {
        iReturn = ds_strnzcat(pBuffer, "asc", uBufferSize);
    }
    else if (filter->eDirection == USERLISTAPI_DIRECTION_DESCENDING)
    {
        iReturn = ds_strnzcat(pBuffer, "desc", uBufferSize);
    }

    return(iReturn);
}

/*F*************************************************************************************************/
/*!
    \Function   _UserListApiAddPresenceToQuery

    \Description
        Add the presence selection data to the people query

    \Input *pRef            - module ref
    \Input *filter          - query filters
    \Input *pBuffer         - [out] buffer where the request is stored
    \Input  uBufferSize     - size of the buffer 
    \Input uTypeMask        - mask

    \Output
        int32_t             - 0 for success, negative for error

    \Version 04/17/2013 (amakoukji)
*/
/*************************************************************************************************F*/
int32_t _UserListApiAddPresenceToQuery(UserListApiRefT *pRef, UserListApiFiltersT *filter, char *pBuffer, uint32_t uBufferSize, uint32_t uTypeMask)
{
    int32_t iReturn = 0;

    // 1st party specific filter comes first, the mask is checked
    if (filter->ePresence != USERLISTAPI_PRESENCE_NONE)
    {
        iReturn = ds_strnzcat(pBuffer, "&presenceType=", uBufferSize);
        if (iReturn < 0)
        {
             return(iReturn);
        }

        switch (filter->ePresence)
        {
            case USERLISTAPI_PRESENCE_PRIMARY:
                iReturn = ds_strnzcat(pBuffer, "primary", uBufferSize);
                break;
            case USERLISTAPI_PRESENCE_PLATFORM:
                iReturn = ds_strnzcat(pBuffer, "platform", uBufferSize);
                break;
            case USERLISTAPI_PRESENCE_INCONTEXT:
            default:
                iReturn = ds_strnzcat(pBuffer, "incontext", uBufferSize);
                break;
        }

        iReturn = ds_strnzcat(pBuffer, "&presenceDetail=true", uBufferSize);
    }
    else if (uTypeMask & USERLISTAPI_MASK_PRESENCE || uTypeMask & USERLISTAPI_MASK_RICH_PRESENCE)
    {
        iReturn = ds_strnzcat(pBuffer, "&presenceType=primary&presenceDetail=true", uBufferSize);
    }

    return(iReturn);
}

/*F*************************************************************************************************/
/*!
    \Function   _UserListApiAddFieldsToQuery

    \Description
        Add the fields selection data to the people query

    \Input *pRef        - module ref
    \Input *filter      - query filters
    \Input *pBuffer     - [out] buffer where the request is stored
    \Input uBufferSize  - size of the buffer
    \Input uTypeMask    - mask
    \Input eRequestType - request type

    \Output
        int32_t         - 0 for success, negative for error

    \Version 04/17/2013 (amakoukji)
*/
/*************************************************************************************************F*/
int32_t _UserListApiAddFieldsToQuery(UserListApiRefT *pRef, UserListApiFiltersT *filter, char *pBuffer, uint32_t uBufferSize, uint32_t uTypeMask, UserListApiRequestTypeE eRequestType)
{
    int32_t iReturn = 0;
    uint8_t bNeedDelimiter = FALSE;
    uint8_t bNoFields = !(filter->Fields.bUser || filter->Fields.bRegion || filter->Fields.bNpId || filter->Fields.bAvatarUrl || filter->Fields.bPersonalDetail);

    if (bNoFields && uTypeMask == 0)
    {
        // nothing to do, default case is assumed
        return(0);
    }
    else
    {
        iReturn = ds_strnzcat(pBuffer, "&fields=", uBufferSize);
        if (iReturn < 0)
        {
            return(iReturn);
        }

        if (bNoFields)
        {
            iReturn = ds_strnzcat(pBuffer, "@default", uBufferSize);
            if (iReturn < 0)
            {
                return(iReturn);
            }
            bNeedDelimiter = TRUE;
        }

        if (filter->Fields.bUser)
        {
            if (bNeedDelimiter)
            {
                iReturn = ds_strnzcat(pBuffer, ",", uBufferSize);
                if (iReturn < 0)
                {
                    return(iReturn);
                }
            }
            iReturn = ds_strnzcat(pBuffer, "user", uBufferSize);
            if (iReturn < 0)
            {
                return(iReturn);
            }
            bNeedDelimiter = TRUE;
        }

        if (filter->Fields.bRegion)
        {
            if (bNeedDelimiter)
            {
                iReturn = ds_strnzcat(pBuffer, ",", uBufferSize);
                if (iReturn < 0)
                {
                    return(iReturn);
                }
            }
            iReturn = ds_strnzcat(pBuffer, "region", uBufferSize);
            if (iReturn < 0)
            {
                return(iReturn);
            }
            bNeedDelimiter = TRUE;
        }

        if (filter->Fields.bNpId)
        {
            if (bNeedDelimiter)
            {
                iReturn = ds_strnzcat(pBuffer, ",", uBufferSize);
                if (iReturn < 0)
                {
                    return(iReturn);
                }
            }
            iReturn = ds_strnzcat(pBuffer, "npId", uBufferSize);
            if (iReturn < 0)
            {
                return(iReturn);
            }
            bNeedDelimiter = TRUE;
        }

        if ((filter->Fields.bPersonalDetail || (uTypeMask & USERLISTAPI_MASK_PROFILE)) && eRequestType != USERLISTAPI_REQUEST_TYPE_BLOCKED)
        {
            if (bNeedDelimiter)
            {
                iReturn = ds_strnzcat(pBuffer, ",", uBufferSize);
                if (iReturn < 0)
                {
                    return(iReturn);
                }
            }
            iReturn = ds_strnzcat(pBuffer, "personalDetail", uBufferSize);
            if (iReturn < 0)
            {
                return(iReturn);
            }
            bNeedDelimiter = TRUE;
        }

        if (filter->Fields.bAvatarUrl || (uTypeMask & USERLISTAPI_MASK_PROFILE))
        {
            if (bNeedDelimiter)
            {
                iReturn = ds_strnzcat(pBuffer, ",", uBufferSize);
                if (iReturn < 0)
                {
                    return(iReturn);
                }
            }
            iReturn = ds_strnzcat(pBuffer, "avatarUrl", uBufferSize);
            if (iReturn < 0)
            {
                return(iReturn);
            }
            bNeedDelimiter = TRUE;
        }
    }

    return(iReturn);
}

/*F*************************************************************************************************/
/*!
    \Function   _UserListApiQueuePeopleQuery

    \Description
        Add a query to the list to be processed

    \Input *pRef        - module ref
    \Input  uUserIndex  - local user index
    \Input  eType       - type of query
    \Input  iLimit      - maximum users to return in the query
    \Input  iOffset     - query offset
    \Input *filter      - query filters
    \Input *pNotifyCb   - user callback
    \Input  uTypeMask   - mask
    \Input accountId    - user's Account Id
    \Input *pUserData   - pointer to user data for callback

    \Output
        int32_t         - 0 for success, negative for error

    \Version 04/17/2013 (amakoukji)
*/
/*************************************************************************************************F*/
int32_t _UserListApiQueuePeopleQuery(UserListApiRefT *pRef, uint32_t uUserIndex, UserListApiTypeE eType, int32_t iLimit,  int32_t iOffset, UserListApiFiltersT *filter, UserListApiCallbackT *pNotifyCb, uint32_t uTypeMask, SceNpAccountId accountId, void *pUserData)
{
    NetCritEnter(&pRef->crit);

    // create a new request and put it in the list
    UserListRequestT *request = NULL;
    if ((request = (UserListRequestT*)DirtyMemAlloc(sizeof(UserListRequestT), USERLISTAPI_MEMID, pRef->iMemGroup, pRef->pMemGroupUserData)) == NULL) 
    {
        NetPrintf(("userlistapips4: could not allocate memory for UserListRequestT\n"));
        NetCritLeave(&pRef->crit);
        return(-1);
    }

    ds_memclr(request, sizeof(UserListRequestT));

    // put the request at the head of the list
    request->pNext = pRef->RequestList;
    pRef->RequestList = request;

    // Copy the data into the permanent request object
    switch (eType)
    {
        case USERLISTAPI_TYPE_FRIENDS:
            request->eRequestType = USERLISTAPI_REQUEST_TYPE_FRIENDS;
            break;
        case USERLISTAPI_TYPE_BLOCKED:
            request->eRequestType = USERLISTAPI_REQUEST_TYPE_BLOCKED;
            break;
        case USERLISTAPI_TYPE_FRIENDS_OF_FRIEND:
            request->eRequestType = USERLISTAPI_REQUEST_TYPE_FRIENDS_OF_FRIEND;
            break;
        case USERLISTAPI_TYPE_MUTED:
            NetPrintf(("userlistapips4: unexpected request type\n"));
            request->eRequestType = USERLISTAPI_REQUEST_TYPE_FRIENDS;
            break;
    };
    request->iRequestId   = -1;
    request->pNotifyCb    = pNotifyCb;
    request->pIsAQueryNotifyCb = NULL;
    request->pUserData    = pUserData;
    request->iLimit       = iLimit;
    request->iOffset      = iOffset;
    request->uUserIndex   = uUserIndex;
    request->uTypeMask    = uTypeMask;
    request->TargetUserId = accountId;
    if(filter)
    {
        ds_memcpy( (void*)&(request->Filters), filter, sizeof(UserListApiFiltersT));
    }

    NetCritLeave(&pRef->crit);
    return(0);
}

/*F*************************************************************************************************/
/*!
    \Function   _UserListApiIsQuery

    \Description
        Add a IsFriend query to the list to be processed

    \Input *pRef                - module ref
    \Input  uUserIndex          - local user index
    \Input  eType               - type of query
    \Input *pNotifyCb           - user callback
    \Input  TargetUserId        - target user id (account id)
    \Input *pUserData           - pointer to user data for callback

    \Output
        int32_t                 - 0 for success, negative for error

    \Version 08/09/2013 (amakoukji)
*/
/*************************************************************************************************F*/
int32_t _UserListApiIsQuery(UserListApiRefT *pRef, uint32_t uUserIndex, UserListApiRequestTypeE eType, UserListApiIsAQueryCallbackT *pNotifyCb, SceNpAccountId TargetUserId, void *pUserData)
{
    NetCritEnter(&pRef->crit);

    // create a new request and put it in the list
    UserListRequestT *request = NULL;
    if ((request = (UserListRequestT*)DirtyMemAlloc(sizeof(UserListRequestT), USERLISTAPI_MEMID, pRef->iMemGroup, pRef->pMemGroupUserData)) == NULL) 
    {
        NetPrintf(("userlistapips4: could not allocate memory for UserListRequestT\n"));
        NetCritLeave(&pRef->crit);
        return(-1);
    }

    ds_memclr(request, sizeof(UserListRequestT));

    // put the request at the head of the list
    request->pNext = pRef->RequestList;
    pRef->RequestList = request;

    // Copy the data into the permanent request object
    request->eRequestType = eType;
    request->iRequestId   = -1;
    request->pNotifyCb    = NULL;
    request->pIsAQueryNotifyCb = pNotifyCb;
    request->pUserData    = pUserData;
    request->uUserIndex   = uUserIndex;
    request->TargetUserId = TargetUserId;

    NetCritLeave(&pRef->crit);
    return(0);
}

/*F*************************************************************************************************/
/*!
    \Function   _UserListApiSendPeopleQuery

    \Description
        Construct and send the people list request to PSN

    \Input *pRef        - module ref

    \Output
        int32_t         - positive for success, negative for error

    \Version 04/17/2013 (amakoukji)
*/
/*************************************************************************************************F*/
int32_t _UserListApiSendPeopleQuery(UserListApiRefT *pRef)
{
    char requestPath[256];
    int32_t iReturn = 0;
    UserListRequestT *pCurrent = NULL;
    UserListRequestT *pLast = NULL;
    SceNpAccountId accountId;

    // find the last element in the list
    NetCritEnter(&pRef->crit);

    pCurrent = pRef->RequestList;
    if (pCurrent == NULL)
    {
        // no requests to handle at this time
        NetCritLeave(&pRef->crit);
        return(0);
    }

    while (pCurrent->pNext != NULL)
    {
        pLast = pCurrent;
        pCurrent = pCurrent->pNext;
    }

    // get the online id of the user making the request
    iReturn = NetConnStatus('acct', (signed)pCurrent->uUserIndex, &accountId, sizeof(accountId));
    if (iReturn < 0)
    {
        // error fetching id, try again later
        NetCritLeave(&pRef->crit);
        return(-1);
    }

    // remove the last request from the list for processing
    if (pLast)
    {
        pLast->pNext = NULL;
    }
    else
    {
        pRef->RequestList = NULL;
    }

    pRef->ActiveRequest = pCurrent;
    NetCritLeave(&pRef->crit);
    // build a request of the correct type
    switch(pCurrent->eRequestType)
    {
        case USERLISTAPI_REQUEST_TYPE_BLOCKED:
        {
            iReturn = ds_snzprintf(requestPath, sizeof(requestPath), "%s/%llu/%s?limit=%d&offset=%d", USERLISTAPI_PATH_BASE, accountId, USERLISTAPI_PATH_BLOCKED, pCurrent->iLimit, pCurrent->iOffset);
            if (iReturn < 0)
            {
                return(iReturn);
            }

            // handle avatar size
            iReturn = _UserListApiAddAvatarToQuery(pRef, &pCurrent->Filters, requestPath, sizeof(requestPath), pCurrent->uTypeMask);
            if (iReturn < 0)
            {
                return(iReturn);
            }

            // handle the requested fields Value
            if (!pCurrent->Filters.Fields.bNpId)
            {
                if (!(pCurrent->Filters.Fields.bNpId == FALSE && pCurrent->Filters.Fields.bAvatarUrl == FALSE && pCurrent->Filters.Fields.bUser == FALSE && pCurrent->Filters.Fields.bRegion == FALSE && pCurrent->Filters.Fields.bPersonalDetail == FALSE))
                {
                    pCurrent->Filters.Fields.bNpId = 1;
                }
            }
            
            iReturn = _UserListApiAddFieldsToQuery(pRef, &pCurrent->Filters, requestPath, sizeof(requestPath), pCurrent->uTypeMask, pCurrent->eRequestType);
            if (iReturn < 0)
            {
                return(iReturn);
            }
        }
        break;

        case USERLISTAPI_REQUEST_TYPE_ISFRIENDS:
        case USERLISTAPI_REQUEST_TYPE_ISBLOCKED:
        {
            iReturn = ds_snzprintf(requestPath, sizeof(requestPath), "%s/%llu/%s", USERLISTAPI_PATH_BASE, pCurrent->TargetUserId, "profile?fields=@default,relation");

            if (iReturn < 0)
            {
                return(iReturn);
            }
        }
        break;

        case USERLISTAPI_REQUEST_TYPE_FRIENDS_OF_FRIEND:
        case USERLISTAPI_REQUEST_TYPE_FRIENDS:
        default:
        {
            // build the request
            SceNpAccountId FriendId;
            if (pCurrent->eRequestType == USERLISTAPI_REQUEST_TYPE_FRIENDS_OF_FRIEND)
            {
                // Special case for friend of friend
                if (pCurrent->Filters.pFriend == NULL)
                {
                    NetPrintf(("userlistapips4: friends-of-friend query requires a specified friend id\n"));
                    return(USERLISTAPI_ERROR_BAD_PARAM);
                }

                DirtyUserToNativeUser(&FriendId, sizeof(FriendId), pCurrent->Filters.pFriend);
            }

            iReturn = ds_snzprintf(requestPath, sizeof(requestPath), "%s/%llu/%s?friendStatus=%s&limit=%d&offset=%d", USERLISTAPI_PATH_BASE, (pCurrent->eRequestType == USERLISTAPI_REQUEST_TYPE_FRIENDS_OF_FRIEND ? FriendId : accountId), USERLISTAPI_PATH_FRIENDS, USERLISTAPI_FRIEND_TYPE_FRIEND , pCurrent->iLimit, pCurrent->iOffset);
            if (iReturn < 0)
            {
                return(iReturn);
            }
                       
            // skip the eLevel parameter, since there is only 1 possible value

            // handle avatar size
            iReturn = _UserListApiAddAvatarToQuery(pRef, &pCurrent->Filters, requestPath, sizeof(requestPath), pCurrent->uTypeMask);
            if (iReturn < 0)
            {
                return(iReturn);
            }

            // handle context filters
            iReturn = _UserListApiAddContextToQuery(pRef, &pCurrent->Filters, requestPath, sizeof(requestPath));
            if (iReturn < 0)
            {
                return(iReturn);
            }

            // handle filters order
            iReturn = _UserListApiAddOrderToQuery(pRef, &pCurrent->Filters, requestPath, sizeof(requestPath));
            if (iReturn < 0)
            {
                return(iReturn);
            }

            // handle the Presence Value
            iReturn = _UserListApiAddPresenceToQuery(pRef, &pCurrent->Filters, requestPath, sizeof(requestPath), pCurrent->uTypeMask);
            if (iReturn < 0)
            {
                return(iReturn);
            }

            // handle the requested fields Value
            if (!pCurrent->Filters.Fields.bNpId)
            {
                if (!(pCurrent->Filters.Fields.bNpId == FALSE && pCurrent->Filters.Fields.bAvatarUrl == FALSE && pCurrent->Filters.Fields.bUser == FALSE && pCurrent->Filters.Fields.bRegion == FALSE && pCurrent->Filters.Fields.bPersonalDetail == FALSE))
                {
                    pCurrent->Filters.Fields.bNpId = 1;
                }
            }

            iReturn = _UserListApiAddFieldsToQuery(pRef, &pCurrent->Filters, requestPath, sizeof(requestPath), pCurrent->uTypeMask, pCurrent->eRequestType);
            if (iReturn < 0)
            {
                return(iReturn);
            }
        }
        break;
    }

    iReturn = DirtyWebApiRequest(pRef->pWebApi, (int32_t)pCurrent->uUserIndex, USERLISTAPI_API_GROUP, SCE_NP_WEBAPI_HTTP_METHOD_GET, requestPath, NULL, 0, NULL, &_DirtyWebApiCallback, pRef);
    return(iReturn);
}


/*F*************************************************************************************************/
/*!
    \Function   _UserListOnWebApiUpdate

    \Description
        Callback for when friends update events happen.

        \Input userCtxId        - user web api context
        \Input callbackId       - callback id generated at registration
        \Input pNpServiceName   - NP web service generating the event
        \Input npServiceLabel   - more detail on the web service
        \Input pTo              - contains the account id of target user
        \Input pFrom            - contains the account id of source user
        \Input pDataType        - event type
        \Input pData            - any data coming with the event
        \Input dataLen          - length of data with the event
        \Input pUserArg         - pointer to DirtySessionManagerRefT

    \Version 08/02/2013 (efan)
*/
/*************************************************************************************************F*/
static void _UserListOnWebApiUpdate(int32_t userCtxId, int32_t callbackId, const char *pNpServiceName, SceNpServiceLabel npServiceLabel, const SceNpPeerAddressA *pTo, const SceNpPeerAddressA *pFrom, const SceNpWebApiPushEventDataType *pDataType, const char *pData, size_t dataLen, void *pUserArg)
{
    UserListApiNotificationT (*pList)[] = NULL;
    UserListApiNotifyTypeE eType = USERLISTAPI_NOTIFY_FRIENDLIST_UPDATE;
    DirtyUserT dirtyUser;
    SceNpAccountId AccountId;
    int32_t iMemGroup;
    void *pMemGroupUserData;
    UserListApiNotifyDataT *data = NULL;
    int32_t iLoop = 0;
    uint32_t uUserIndex = 0; // default to user 0

    if (pUserArg != NULL)
    {
        UserListApiRefT *pRef = (UserListApiRefT*)pUserArg;
        NetPrintf(("userlistapips4: [%p] detected userlist updated event.\n", pRef));

        // create a UserListApiNotifyDataT 
        DirtyMemGroupQuery(&iMemGroup, &pMemGroupUserData);
        data = (UserListApiNotifyDataT*)DirtyMemAlloc(sizeof(UserListApiNotifyDataT), USERLISTAPI_MEMID, iMemGroup, pMemGroupUserData);
        ds_memclr(data, sizeof(UserListApiNotifyDataT));

        // find the correct user whom this is for
        for (int32_t iIndex = 0; iIndex < NETCONN_MAXLOCALUSERS; ++iIndex)
        {
            int32_t iReturn = NetConnStatus('acct', iIndex, &AccountId, sizeof(AccountId));

            if (iReturn >= 0 && (AccountId == pTo->accountId))
            {
                uUserIndex = (uint32_t)iIndex;
                break;
            }
        }

        AccountId = pTo->accountId;
        DirtyUserFromNativeUser(&dirtyUser, &AccountId);

        if (ds_stricmp(pDataType->val, "np:service:friendlist:friend") == 0)
        {
            pList = &pRef->FriendNotificaton;
            eType = USERLISTAPI_NOTIFY_FRIENDLIST_UPDATE;
            ds_memcpy_s(&data->FriendListData.DirtyUser, sizeof(data->FriendListData.DirtyUser), &dirtyUser, sizeof(dirtyUser));
            data->FriendListData.uUserIndex = uUserIndex;
        }
        else if (ds_stricmp(pDataType->val, "np:service:blocklist") == 0)
        {
            pList = &pRef->BlockedNotificaton;
            eType = USERLISTAPI_NOTIFY_BLOCKEDLIST_UPDATE;
            ds_memcpy_s(&data->BlockedListData.DirtyUser, sizeof(data->BlockedListData.DirtyUser), &dirtyUser, sizeof(dirtyUser));
            data->BlockedListData.uUserIndex = uUserIndex;
        }
        else
        {
            NetPrintf(("userlistapips4: received unknown push event: %s\n", pDataType->val));
            DirtyMemFree(data, USERLISTAPI_MEMID, iMemGroup, pMemGroupUserData);
            return;
        }

        NetCritEnter(&pRef->crit);
        // find an empty slot
        for (iLoop = 0; iLoop < USERLISTAPI_MAX_QUEUED_NOTIFICATIONS; ++iLoop)
        {
            if (pRef->UserListApiNotifyEvent[iLoop].pNotificationData == NULL)
            {
                pRef->UserListApiNotifyEvent[iLoop].pNotificationData = data;
                pRef->UserListApiNotifyEvent[iLoop].pNotificationList = pList;
                pRef->UserListApiNotifyEvent[iLoop].pNotificationType = eType;
                pRef->UserListApiNotifyEvent[iLoop].uUserIndex        = uUserIndex;

                break;
            }
        }
        if (iLoop == USERLISTAPI_MAX_QUEUED_NOTIFICATIONS)
        {
            NetPrintf(("userlistapips4: maximum notifications in queue exceeded, notification dropped\n"));
        }
        NetCritLeave(&pRef->crit);
    }
    else
    {
        NetPrintf(("userlistapips4: null UserApiRefT from pUserArg.\n"));
    }
}

/*** Public functions ********************************************************/

/*F*************************************************************************************************/
/*!
    \Function   UserListApiCreate

    \Description
        Startup UserListApi module.

    \Input uMaxResponseSize     - size of the ProtoHttpBuffer

    \Output
        UserListApiRefT*        - pointer to new ref or NULL for error

    \Version 04/17/2013 (amakoukji)
*/
/*************************************************************************************************F*/
UserListApiRefT *UserListApiCreate(uint32_t uMaxResponseSize)
{
    UserListApiRefT *pState = NULL;
    int32_t iMemGroup;
    void *pMemGroupUserData;

    DirtyWebApiCreateParamsT createParams;

    // Query current mem group data
    DirtyMemGroupQuery(&iMemGroup, &pMemGroupUserData);

    // allocate and init module state
    if ((pState = (UserListApiRefT*)DirtyMemAlloc(sizeof(*pState), USERLISTAPI_MEMID, iMemGroup, pMemGroupUserData)) == NULL) 
    {
        NetPrintf(("userlistapips4: could not allocate module state\n"));
        return(NULL);
    }
    ds_memclr(pState, sizeof(*pState));
    pState->iMemGroup = iMemGroup;
    pState->pMemGroupUserData = pMemGroupUserData;

    pState->iReadBufferSize = USERLISTAPI_DEFAULT_READ_BUFFER_SIZE;
    if ((pState->pReadBuffer = (char*)DirtyMemAlloc(pState->iReadBufferSize, USERLISTAPI_MEMID, pState->iMemGroup, pState->pMemGroupUserData)) == NULL)
    {
        NetPrintf(("userlistapips4: could not allocate space for the pReadBuffer\n"));
        DirtyMemFree(pState, USERLISTAPI_MEMID, iMemGroup, pMemGroupUserData);
        return(NULL);
    }

    // init DirtyWebApi
    // $todo: We initialize a very large buffer here to fit up to 500 friends at once.
    // In the future we will need to reduce the size of this buffer and process the data piece by piece.
    // Ultimately 256K will not cause too many problems on PS4 but we should optimize this as it is 
    // a much larger chunk of memory than DirtySDK typically takes.
    ds_memclr(&createParams, sizeof(createParams));
    createParams.readBufferSize = 256000;
    pState->pWebApi = DirtyWebApiCreate(&createParams); 
    if (pState->pWebApi == NULL)
    {
        NetPrintf(("userlistapips4: could not allocate WebApi\n"));
        DirtyMemFree(pState, USERLISTAPI_MEMID, iMemGroup, pMemGroupUserData);
        return(NULL);
    }

    // init crit
    NetCritInit(&pState->crit, "UserListApi");

    return(pState);
}

/*F*************************************************************************************************/
/*!
    \Function   UserListApiDestroy

    \Description
        Shutdown UserListApi module.

    \Input *pRef - module ref

    \Version 04/17/2013 (amakoukji)
*/
/*************************************************************************************************F*/
void UserListApiDestroy(UserListApiRefT *pRef)
{
    UserListApiCancelAll(pRef);

    DirtyWebApiDestroy(pRef->pWebApi);

    // destroy crit
    NetCritKill(&pRef->crit);

    DirtyMemFree(pRef->pReadBuffer, USERLISTAPI_MEMID, pRef->iMemGroup, pRef->pMemGroupUserData);

    // free the memory
    DirtyMemFree(pRef, USERLISTAPI_MEMID, pRef->iMemGroup, pRef->pMemGroupUserData);

    NetPrintf(("userlistapips4: [%p] freed userlistapi\n", pRef));
}

/*F*************************************************************************************************/
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
        int32_t                 - 0 for success, negative for error

    \Version 02/14/2014 (amakoukji)
*/
/*************************************************************************************************F*/
int32_t UserListApiRegisterUpdateEvent(UserListApiRefT *pRef, uint32_t uUserIndex, UserListApiNotifyTypeE eType, UserListApiUpdateCallbackT *pNotifyCb, void *pUserData)
{
    int32_t iReturn = 0;
    UserListApiNotificationT (*pList)[] = NULL;

    if (eType == USERLISTAPI_NOTIFY_FRIENDLIST_UPDATE)
    {
        pList = &pRef->FriendNotificaton;
        if (pRef->bFriendNotificationStarted != TRUE)
        {
            pRef->bFriendNotificationStarted = TRUE;
            iReturn = DirtyWebApiAddPushEventListener(pRef->pWebApi, "np:service:friendlist:friend", _UserListOnWebApiUpdate, pRef);
        }
    }
    else if (eType == USERLISTAPI_NOTIFY_BLOCKEDLIST_UPDATE)
    {
        pList = &pRef->BlockedNotificaton;
        if (pRef->bBlockedNotificationStarted != TRUE)
        {
            pRef->bBlockedNotificationStarted = TRUE;
            iReturn = DirtyWebApiAddPushEventListener(pRef->pWebApi, "np:service:blocklist", _UserListOnWebApiUpdate, pRef);
        }
    }
    else
    {
        return(USERLISTAPI_ERROR_UNSUPPORTED);
    }

    if (iReturn < 0)
    {
        // an error occured while initializing, surface it
        return(iReturn);
    }

    for (int i = 0; i < USERLISTAPI_NOTIFY_LIST_MAX_SIZE; ++i)
    {
        if ((*pList)[i].pCallback == NULL)
        {
            (*pList)[i].pCallback = pNotifyCb;
            (*pList)[i].pUserData = pUserData;
            (*pList)[i].uUserIndex = uUserIndex;
            break;
        }
    }

    if (pList == NULL)
    {
        return(USERAPI_ERROR_FULL);
    }
    
    return(iReturn);
}

/*F*************************************************************************************************/
/*!
    \Function   UserListApiIsFriendAsync

    \Description
        Check to see if target user is a friend of the current user.

    \Input *pRef                - module ref
    \Input uUserIndex           - current user index
    \Input *pUser               - DirtyUserT for target 
    \Input *pNotifyCb           - callback
    \Input *pUserData           - the user data

    \Output
        int32_t                 - 0 for success; negative for error

    \Version 06/24/2013 (tcho)
*/
/*************************************************************************************************F*/
int32_t UserListApiIsFriendAsync(UserListApiRefT *pRef, uint32_t uUserIndex, DirtyUserT *pUser, UserListApiIsAQueryCallbackT *pNotifyCb, void *pUserData)
{
    int32_t iReturn = 0;
    SceNpAccountId accountId;

    ds_memclr(&accountId, sizeof(accountId));

    if (DirtyUserToNativeUser(&accountId, sizeof(accountId), pUser) != TRUE)
    {
        return(USERLISTAPI_ERROR_BAD_PARAM);
    }

    iReturn = _UserListApiIsQuery(pRef, uUserIndex, USERLISTAPI_REQUEST_TYPE_ISFRIENDS , pNotifyCb, accountId, pUserData);
    return(iReturn);
}

/*F*************************************************************************************************/
/*!
    \Function   UserListApiIsBlockedAsync

    \Description
        Check to see if target user is blocked to the current user.

    \Input *pRef -              - module ref
    \Input uUserIndex           - current user index
    \Input *pUser               - DirtyUserT for target 
    \Input *pNotifyCb           - callback
    \Input *pUserData           - user data

    \Version 09/11/2013 (amakoukji)
*/
/*************************************************************************************************F*/
int32_t UserListApiIsBlockedAsync(UserListApiRefT *pRef, uint32_t uUserIndex, DirtyUserT *pUser, UserListApiIsAQueryCallbackT *pNotifyCb, void *pUserData)
{
    int32_t iReturn = 0;
    SceNpAccountId accountId;
    char strTargetUserId[SCE_NP_ONLINEID_MAX_LENGTH+1];

    ds_memclr(&accountId, sizeof(accountId));
    ds_memclr(&strTargetUserId, sizeof(strTargetUserId));

    if (DirtyUserToNativeUser(&accountId, sizeof(accountId), pUser) != TRUE)
    {
        return(USERLISTAPI_ERROR_BAD_PARAM);
    }

    iReturn = _UserListApiIsQuery(pRef, uUserIndex, USERLISTAPI_REQUEST_TYPE_ISBLOCKED , pNotifyCb, accountId, pUserData);
    return(iReturn);
}

/*F*************************************************************************************************/
/*!
    \Function   UserListApiUpdate

    \Description
        Updates UserListApi module.

    \Input *pRef - module ref

    \Version 04/17/2013 (amakoukji)
*/
/*************************************************************************************************F*/
void UserListApiUpdate(UserListApiRefT *pRef)
{
    // first check to see if there's data waiting to pick up
    // if so call the callbacks
    NetCritEnter(&pRef->crit);
    if (pRef->bDataReady && pRef->ActiveRequest)
    {
        if (pRef->iReadError != 0)
        {
            if (pRef->ActiveRequest->pNotifyCb)
            {
                UserListApiEventDataT data;
                ds_memclr(&data, sizeof(data));
                switch (pRef->ActiveRequest->eRequestType)
                {
                    case USERLISTAPI_REQUEST_TYPE_FRIENDS:
                        data.ListEndData.eRequestType = USERLISTAPI_TYPE_FRIENDS;
                        break;
                    case USERLISTAPI_REQUEST_TYPE_BLOCKED:
                        data.ListEndData.eRequestType = USERLISTAPI_TYPE_BLOCKED;
                        break;
                    case USERLISTAPI_REQUEST_TYPE_FRIENDS_OF_FRIEND:
                        data.ListEndData.eRequestType = USERLISTAPI_TYPE_FRIENDS_OF_FRIEND;
                        break;
                    case USERLISTAPI_REQUEST_TYPE_MUTED:
                        data.ListEndData.eRequestType = USERLISTAPI_TYPE_MUTED;
                        break;
                    case USERLISTAPI_REQUEST_TYPE_ISFRIENDS:
                    case USERLISTAPI_REQUEST_TYPE_ISBLOCKED:
                        NetPrintf(("userlistapips4: unexpected request type\n"));
                        data.ListEndData.eRequestType = USERLISTAPI_TYPE_FRIENDS;
                        break;
                }
                data.ListEndData.pUserListFilters = &pRef->ActiveRequest->Filters;
                data.ListEndData.uUserIndex = pRef->ActiveRequest->uUserIndex;
                data.ListEndData.Error = -1;
                pRef->ActiveRequest->pNotifyCb(pRef, TYPE_LIST_END, &data, pRef->ActiveRequest->pUserData);
            }
            else if (pRef->ActiveRequest->pIsAQueryNotifyCb)
            {
                UserListApiIsADataT data;
                ds_memclr(&data, sizeof(data));
                UserListApiIfATypeE eType = USERLISTAPI_IS_FRIENDS;
                SceNpAccountId accountId;

                switch (pRef->ActiveRequest->eRequestType)
                {
                    case USERLISTAPI_REQUEST_TYPE_FRIENDS:
                    case USERLISTAPI_REQUEST_TYPE_MUTED:
                    case USERLISTAPI_REQUEST_TYPE_BLOCKED:
                    case USERLISTAPI_REQUEST_TYPE_FRIENDS_OF_FRIEND:
                        NetPrintf(("userlistapips4: unexpected request type\n"));
                        break;
                    case USERLISTAPI_REQUEST_TYPE_ISFRIENDS:
                        eType = USERLISTAPI_IS_FRIENDS;
                    case USERLISTAPI_REQUEST_TYPE_ISBLOCKED:
                        eType = USERLISTAPI_IS_BLOCKED;
                        break;
                }

                data.eIsaType = USERLISTAPI_IS_TYPE_UNKNOWN;
                accountId = pRef->ActiveRequest->TargetUserId;
                DirtyUserFromNativeUser(&data.DirtyUser, &accountId);
                data.Error = pRef->iReadError;
                pRef->ActiveRequest->pIsAQueryNotifyCb(pRef, eType, &data, pRef->ActiveRequest->pUserData);

            }
        }
        else if(pRef->ActiveRequest->eRequestType != USERLISTAPI_REQUEST_TYPE_ISFRIENDS && pRef->ActiveRequest->eRequestType != USERLISTAPI_REQUEST_TYPE_ISBLOCKED)
        {
            _UserListApiParseResponse(pRef, pRef->ActiveRequest);
        }
        else
        {
            _UserListApiParseIsResponse(pRef, pRef->ActiveRequest);
        }

        DirtyMemFree(pRef->ActiveRequest, USERLISTAPI_MEMID, pRef->iMemGroup, pRef->pMemGroupUserData);

        pRef->ActiveRequest = NULL;
    }

    // next, check the next request on the queue
    if (pRef->ActiveRequest == NULL)
    {
        int32_t iReturn = 0;
        pRef->bDataReady = FALSE;
        pRef->iReadError = 0;

        iReturn = _UserListApiSendPeopleQuery(pRef);

        if (iReturn < 0)
        {
            NetPrintf(("userlistapips4: unable to send query, error %d was encountered. \n", iReturn));
            if (pRef->ActiveRequest != NULL)
            {
                if (pRef->ActiveRequest->pNotifyCb != NULL)
                {
                    UserListApiEventDataT data;
                    ds_memclr(&data, sizeof(data));
                    switch (pRef->ActiveRequest->eRequestType)
                    {
                        case USERLISTAPI_REQUEST_TYPE_FRIENDS:
                            data.ListEndData.eRequestType = USERLISTAPI_TYPE_FRIENDS;
                            break;
                        case USERLISTAPI_REQUEST_TYPE_BLOCKED:
                            data.ListEndData.eRequestType = USERLISTAPI_TYPE_BLOCKED;
                            break;
                        case USERLISTAPI_REQUEST_TYPE_FRIENDS_OF_FRIEND:
                            data.ListEndData.eRequestType = USERLISTAPI_TYPE_FRIENDS_OF_FRIEND;
                            break;
                        case USERLISTAPI_REQUEST_TYPE_MUTED:
                            data.ListEndData.eRequestType = USERLISTAPI_TYPE_MUTED;
                            break;
                        case USERLISTAPI_REQUEST_TYPE_ISFRIENDS:
                        case USERLISTAPI_REQUEST_TYPE_ISBLOCKED:
                            data.ListEndData.eRequestType = USERLISTAPI_TYPE_FRIENDS;
                            NetPrintf(("userlistapips4: unexpected request type.\n"));
                            break;
                    }
                    data.ListEndData.pUserListFilters = &pRef->ActiveRequest->Filters;
                    data.ListEndData.uUserIndex = pRef->ActiveRequest->uUserIndex;
                    data.ListEndData.Error = -1;
                    pRef->ActiveRequest->pNotifyCb(pRef, TYPE_LIST_END, &data, pRef->ActiveRequest->pUserData);
                }
            }
        }
    }

    // finally process notifications from Sony
    if (pRef->UserListApiNotifyEvent[0].pNotificationData != NULL)
    {
        int32_t iOuterLoop = 0;
        // at least one notification is in the list, do processing
        for (iOuterLoop = 0; iOuterLoop < USERLISTAPI_MAX_QUEUED_NOTIFICATIONS; ++iOuterLoop)
        {
            if (pRef->UserListApiNotifyEvent[iOuterLoop].pNotificationData != NULL)
            {
                // dispatch notifications
                for (int32_t iInnerLoop = 0; iInnerLoop < USERLISTAPI_NOTIFY_LIST_MAX_SIZE; ++iInnerLoop)
                {
                    if ((*(pRef->UserListApiNotifyEvent[iOuterLoop].pNotificationList))[iInnerLoop].pCallback != NULL)
                    {
                        // if the notification is destined for the appropriate local user dispatch it 
                        if ((*(pRef->UserListApiNotifyEvent[iOuterLoop].pNotificationList))[iInnerLoop].uUserIndex == pRef->UserListApiNotifyEvent[iOuterLoop].uUserIndex)
                        {
                            (*(pRef->UserListApiNotifyEvent[iOuterLoop].pNotificationList))[iInnerLoop].pCallback(pRef, 
                                                                                                                  pRef->UserListApiNotifyEvent[iOuterLoop].pNotificationType, 
                                                                                                                  pRef->UserListApiNotifyEvent[iOuterLoop].pNotificationData, 
                                                                                                                  (*(pRef->UserListApiNotifyEvent[iOuterLoop].pNotificationList))[iInnerLoop].pUserData);
                        }
                    }
                }

                // clean up
                DirtyMemFree(pRef->UserListApiNotifyEvent[iOuterLoop].pNotificationData, USERLISTAPI_MEMID, pRef->iMemGroup, pRef->pMemGroupUserData);
                pRef->UserListApiNotifyEvent[iOuterLoop].pNotificationData = NULL;
                pRef->UserListApiNotifyEvent[iOuterLoop].pNotificationList = NULL;
            }
        }
    }

    NetCritLeave(&pRef->crit);
}

/*F*************************************************************************************************/
/*!
    \Function   UserListApiStatus

    \Description
        Query status of module.

    \Input *pRef    - module ref
    \Input iSelect  - status selector
    \Input *pBuf    - [in/out] control value
    \Input iBufSize - size of pBuf

    \Output
        int32_t     - negative for error, positive for success

    \Version 04/17/2013 (amakoukji)
*/
/*************************************************************************************************F*/
int32_t UserListApiStatus(UserListApiRefT *pRef, int32_t iSelect, void *pBuf, int32_t iBufSize)
{
    NetPrintf(("userlistapips4: unhandled status selector '%C'\n", iSelect));
    return(-1);
}

/*F*************************************************************************************************/
/*!
    \Function   UserListApiControl

    \Description
        Control behavior of module.

    \Input *pRef    - module ref
    \Input iControl - control selector
    \Input iValue   - control value
    \Input iValue2  - control value
    \Input *pValue  - [in/out] control value

    \Output
        int32_t     - negative for error, positive for success

    \Version 04/17/2013 (amakoukji)
*/
/*************************************************************************************************F*/
int32_t UserListApiControl(UserListApiRefT *pRef, int32_t iControl, int32_t iValue, int32_t iValue2, void *pValue)
{
    NetPrintf(("userlistapips4: unhandled control selector '%C'\n", iControl));
    return(-1);
}

/*F*************************************************************************************************/
/*!
    \Function   UserListApiGetListAsync

    \Description
        Retrieve a list of friends or blocked users from PSN.

    \Input *pRef        - module ref
    \Input uUserIndex   - local user index
    \Input eType        - type of users to query
    \Input iLimit       - maximum users to return per query
    \Input iOffset      - start index of query
    \Input *filter      - PS4 specific filter set
    \Input *pNotifyCb   - callback function pointer
    \Input uTypeMask    - mask for types of data expected 
    \Input *pUserData   - user data

    \Output
        int32_t         - negative for error, positive for success

    \Version 04/17/2013 (amakoukji)
*/
/*************************************************************************************************F*/
int32_t UserListApiGetListAsync(UserListApiRefT *pRef, uint32_t uUserIndex, UserListApiTypeE eType, int32_t iLimit,  int32_t iOffset, UserListApiFiltersT *filter, UserListApiCallbackT *pNotifyCb, uint32_t uTypeMask, void *pUserData)
{
    SceNpAccountId accountId;
    int32_t       iReturn = 0;

    if (pRef != NULL)
    {
        if (uUserIndex >= NETCONN_MAXLOCALUSERS)
        {   
            // user index out of range
            NetPrintf(("userlistapips4: UserListApiGeListAsync() called with invalid user index %d.\n", uUserIndex));
            return(-1);
        }
        else 
        {
            switch (eType)
            {
                case USERLISTAPI_TYPE_FRIENDS_OF_FRIEND:
                case USERLISTAPI_TYPE_FRIENDS:
                case USERLISTAPI_TYPE_BLOCKED:
                    {
                        iReturn = NetConnStatus('acct', uUserIndex, &accountId, sizeof(accountId));
                        if (iReturn >= 0)
                        {
                            iReturn = _UserListApiQueuePeopleQuery(pRef, uUserIndex, eType, iLimit,  iOffset, filter, pNotifyCb, uTypeMask, accountId, pUserData);
                        }
                    }
                    break;

                case USERLISTAPI_TYPE_MUTED:
                    iReturn = USERLISTAPI_ERROR_UNSUPPORTED;
                    break;
            }
        }
    }

    return(iReturn);
}

/*F*************************************************************************************************/
/*!
    \Function   UserListApiCancelAll

    \Description
        Cancel all requests in the queue as well as the currently
        active request.

    \Input *pRef       - module ref

    \Version 06/03/2013 (amakoukji)
*/
/*************************************************************************************************F*/
void UserListApiCancelAll(UserListApiRefT *pRef)
{
    // error if not active
    if (pRef == NULL)
    {
        NetPrintf(("userlistapips4: UserListApiCancelAll() called while module is not active\n"));
        return;
    }

    // cancel the currently active WebApi requests
    for (int32_t iIndex = 0; iIndex < NETCONN_MAXLOCALUSERS; ++iIndex)
    {
        DirtyWebApiAbortRequests(pRef->pWebApi, iIndex);
    }

    // delete all the requests in queue
    if (pRef->ActiveRequest)
    {
        DirtyMemFree(pRef->ActiveRequest, USERLISTAPI_MEMID, pRef->iMemGroup, pRef->pMemGroupUserData);
        pRef->ActiveRequest = NULL;
    }
    while (pRef->RequestList != NULL)
    {
        UserListRequestT *pNext = pRef->RequestList->pNext;
        DirtyMemFree(pRef->RequestList, USERLISTAPI_MEMID, pRef->iMemGroup, pRef->pMemGroupUserData);
        pRef->RequestList = pNext;
    }
}
