/*H********************************************************************************/
/*!
    \File userlistapips5.c

    \Description
        See userlistapi.h

    \Copyright
        Copyright (c) Electronic Arts 2020

    \Version 1.0 01/28/2020 (tcho) First Version
*/
/********************************************************************************H*/

/*** Include files ***********************************************************/

#include <kernel.h>
#include <string.h>
#include <net.h>
#include <stdlib.h>
#include <stdint.h>

#include "DirtySDK/misc/userlistapi.h"
#include "DirtySDK/misc/ps5/userlistapips5.h"
#include "DirtySDK/dirtysock.h"
#include "DirtySDK/dirtylang.h"
#include "DirtySDK/dirtysock/dirtyerr.h"
#include "DirtySDK/dirtysock/dirtylib.h"
#include "DirtySDK/dirtysock/dirtymem.h"
#include "DirtySDK/dirtysock/netconn.h"
#include "DirtySDK/proto/protohttp.h"
#include "DirtySDK/util/jsonparse.h"
#include "DirtySDK/dirtysock/ps5/dirtywebapi2ps5.h"


/*** Defines ***************************************************************************/

#define USERLISTAPI_AVATAR_LIST_SIZE            (4) 
#define USERLISTAPI_DEFAULT_READ_BUFFER_SIZE    (2 * 1024)
#define USERLISTAPI_ONE_ENTRY_BUFFER_SIZE       (1024)
#define USERLISTAPI_API_GROUP                   "userProfile"
#define USERLISTAPI_ACTIVITY_API_GROUP          "activities"
#define USERLISTAPI_PATH_FRIENDS                "/v1/users/%llu/friends"
#define USERLISTAPI_PATH_BLOCKED                "/v1/users/me/blocks"
#define USERLISTAPI_PATH_PROFILES               "/v1/users/profiles"
#define USERLISTAPI_PATH_PRESENCES              "/v1/users/basicPresences"
#define USERLISTAPI_PATH_ACTIVITY               "/v1/users/activities"
#define USERLISTAPI_BLOCKED_MAX_REQUEST_RESULT  (2000)
#define USERLISTAPI_FRIENDS_MAX_REQUEST_RESULT  (100)
#define USERLISTAPI_MAX_ACCOUNTIDS              USERLISTAPI_BLOCKED_MAX_REQUEST_RESULT
#define USERLISTAPI_MAX_QUEUED_NOTIFICATIONS    (100)
#define USERLISTAPI_WEBAPI_BUFFER               (100 * 1024) // this should be more than enough room to fit 100 entries
#define USERLISTAPI_PATH_LEN_MAX                (2048)

/*** Macros ****************************************************************************/

/*** Type Definitions ******************************************************************/

typedef enum UserListApiRequestStateE
{
    USERLISTAPI_GETACCOUNTIDS,
    USERLISTAPI_GETACCOUNTIDS_WAIT,
    USERLISTAPI_GETLIST,
    USERLISTAPI_GETLIST_WAIT
} UserListApiRequestStateE;

typedef enum UserListApiRequestTypeE
{
    USERLISTAPI_REQUEST_TYPE_FRIENDS,
    USERLISTAPI_REQUEST_TYPE_BLOCKED
} UserListApiRequestTypeE;

typedef struct UserListRequestT
{
    UserListApiTypeE eRequestType;             //!< request type
    UserListApiRequestStateE eRequestState;    //!< request state
    struct UserListRequestT *pNext;            //!< next request
    UserListApiCallbackT *pNotifyCb;           //!< request callback
    UserListApiFiltersT Filters;               //!< request filter
    int64_t iLimit;                            //!< result limit (max support is 100 on PS5)
    int64_t iOffset;                           //!< result offset 
    int64_t iWebRequestId;                     //!< web request id
    uint32_t uUserIndex;                       //!< user index of the local user making the req
    uint32_t uTypeMask;                        //!< USERLISTAPI_MASK
    uint32_t uMaskProcessed;                   //!< current mask being processed
    void *pUserData;                           //!< pUserData for the reques callback
} UserListRequestT;

typedef struct UserListApiNotificationT
{
    UserListApiUpdateCallbackT *pCallback;         //!< function address
    void                       *pUserData;         //!< user data to return
    uint32_t                    uUserIndex;        //!< user for whom the notification is destined
} UserListApiNotificationT;

typedef struct UserListApiNotifyEventT
{
    UserListApiNotifyDataT    NotificationData;
    UserListApiNotificationT  *pNotificationList;
    UserListApiNotifyTypeE    eNotificationType;
    uint32_t                  uUserIndex;
} UserListApiNotifyEventT;

//! module state
struct UserListApiRefT
{
    // module memory group
    int32_t iMemGroup;              //!< module mem group id
    void *pMemGroupUserData;        //!< user data associated with mem group

    uint32_t uMaxResponseSize;      //!< max size for http response

    NetCritT crit;                  //!< critical section used to ensure thread safety, used to synchornize 
                                    //   access to the pRequestList, pActiveRequest and bDataReady flag

    int64_t  iCurrentWebRequest;    //!< request id used for the currently processed web request

    char *pReadBuffer;              //!< buffer for data from WebApi
    int32_t iReadBufferSize;        //!< the number of "meaningful" bytes in the strReadBuffer buffer
    int32_t iReadError;             //!< flag to notify the user of an error

    uint8_t bDataReady;             //!< data ready flag 

    struct UserListRequestT *pRequestList;    //!< list of pending queries to PSN
    struct UserListRequestT *pActiveRequest;  //!< current request being processed
    DirtyWebApiRefT * pWebApi;

    uint32_t uTotalFrdsCount;                                //!< total number of friends
    uint32_t uAccountIdsNum;                                 //!< number of valid account ids in aAccountIds
    SceNpAccountId aAccountIds[USERLISTAPI_MAX_ACCOUNTIDS];  //!< list of account ids retrun from GetFriends or GetBlocked

    // Callbacks
    uint8_t bFriendNotificationStarted;                                                   //!< is friend list change notification subscribed with PSN
    uint8_t bBlockedNotificationStarted;                                                  //!< is blocked list change notification subscribed with PSN
    UserListApiNotificationT aFriendNotificaton[USERLISTAPI_NOTIFY_LIST_MAX_SIZE];        //!< registered friends list notificaton callbacks 
    UserListApiNotificationT aBlockedNotificaton[USERLISTAPI_NOTIFY_LIST_MAX_SIZE];        //!< registered blocked list notificaton callbacks
    UserListApiNotifyEventT aUserListApiNotifyEvent[USERLISTAPI_MAX_QUEUED_NOTIFICATIONS]; //!< queued PSN notification events
};

/*** Function Prototypes ***************************************************************/
/*** Variables *************************************************************************/

#if DIRTYCODE_LOGGING
static const char *_UserListApi_strRequestState[] =
{
    "USERLISTAPI_GETACCOUNTIDS",
    "USERLISTAPI_GETACCOUNTIDS_WAIT",
    "USERLISTAPI_GETLIST",
    "USERLISTAPI_GETLIST_WAIT"
};
#endif

static const char *_UserListApi_strAvatarSize[] =
{
    "",
    "s",
    "m",
    "l",
    "xl"
};

/*** Private functions ******************************************************/

/*F*************************************************************************************************/
/*!
    \Function   _UserListApiDirtyWebApiCallback

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
static void _UserListApiDirtyWebApiCallback(int32_t iSceError, int32_t iUserIndex, int32_t iStatusCode, const char *pRespBody, int32_t iRespBodyLength, void *pUserData)
{
    UserListApiRefT *pRef = (UserListApiRefT*)pUserData;

    if (iSceError == SCE_OK)
    {
        int32_t iRespBodySize = iRespBodyLength + 1; // the size of the response is the length + 1 (including the null terminator)

        if (iRespBodySize > pRef->iReadBufferSize)
        {
            char *pTmp;
            NetPrintf(("userlistapips5: [%p] resizing pReadBuffer from %d to %d bytes\n", pRef, pRef->iReadBufferSize, iRespBodySize));
            if ((pTmp = (char*)DirtyMemAlloc(iRespBodySize, USERLISTAPI_MEMID, pRef->iMemGroup, pRef->pMemGroupUserData)) != NULL)
            {
                // note, we do NOT need to copy the contents over
                DirtyMemFree(pRef->pReadBuffer, USERLISTAPI_MEMID, pRef->iMemGroup, pRef->pMemGroupUserData);
                pRef->pReadBuffer = pTmp;
                pRef->iReadBufferSize = iRespBodySize;
            }
            else
            {
                NetPrintf(("userlistapips5: [%p] could not allocate space for the pReadBuffer\n", pRef));
                pRef->iReadError = -1;
            }
        }

        if (iRespBodySize <= pRef->iReadBufferSize)
        {
            if ((iStatusCode >= 200) && (iStatusCode < 300))
            {
                if (iRespBodyLength > 0)
                {
                    // copy the response into the read buffer
                    ds_strnzcpy(pRef->pReadBuffer, pRespBody, iRespBodyLength);
                    NetPrintf(("userlistapips5: [%p] WebApi returned status code (%d), %d characters read.\n", pRef, iStatusCode, iRespBodyLength));
                }
                else
                {
                    // I would never expect this to happen for these types of requests if the response code is 2xx
                    NetPrintf(("userlistapips5: [%p] WebApi warning, no response body was found.\n", pRef));
                }
            }
            else
            {
                NetPrintf(("userlistapips5: [%p] WebApi returned an unexpected status code (%d).\n", pRef, iStatusCode));
                pRef->iReadError = -1;
            }
        }
    }
    else
    {
        NetPrintf(("userlistapips5: [%p] WebApi returned an SCE error code (%s).\n", pRef, DirtyErrGetName(iSceError)));
        pRef->iReadError = iSceError;
    }

    // mark the process as done, the UserListApiUpdate() function will call the users callback as needed
    NetCritEnter(&pRef->crit);
    pRef->bDataReady = TRUE;
    NetCritLeave(&pRef->crit);
}

/*F*************************************************************************************************/
/*!
    \Function   _UserListApiAddFieldsToQuery

    \Description
        Add the fields selection data to the people query

    \Input *pActiveRequst - active request
    \Input *pBuffer       - [out] buffer where the request is stored
    \Input uBufferSize    - size of the buffer

    \Output
        int32_t           - 0 for success, negative for error

    \Version 01/28/2020 (tcho)
*/
/*************************************************************************************************F*/
static int32_t _UserListApiAddQueryParamsToRequest(UserListRequestT *pActiveRequst, char *pBuffer, uint32_t uBufferSize)
{
    int32_t iResult = 0;
    char strPageParams[48];

    // form the page params
    if ((iResult = ds_snzprintf(strPageParams, sizeof(strPageParams), "?limit=%d&offset=%d", pActiveRequst->iLimit, pActiveRequst->iOffset)) < 0)
    {
        return(iResult);
    }

    if (pActiveRequst->eRequestType != USERLISTAPI_TYPE_FRIENDS)
    {
        // currently only GetFriends request support other query parameters
        return(0);
    }

    // concatenate page parame to path
    if ((iResult = ds_strnzcat(pBuffer, strPageParams, uBufferSize)) < 0)
    {
        return(iResult);
    }

    // concatenate filter status to path
    if (pActiveRequst->Filters.eStatus == USERLISTAPI_STATUS_ONLINE)
    {
        if ((iResult = ds_strnzcat(pBuffer, "&filter=online", uBufferSize)) < 0)
        {
            return(iResult);
        }
    }

    // concatenate order to path
    if (pActiveRequst->Filters.eOrder == USERLISTAPI_ORDERBY_ID)
    {
        if ((iResult = ds_strnzcat(pBuffer, "&order=onlineId", uBufferSize)) < 0)
        {
            return(iResult);
        }
    }
    else if (pActiveRequst->Filters.eOrder == USERLISTAPI_ORDERBY_STATUS)
    {
        if ((iResult = ds_strnzcat(pBuffer, "&order=onlineStatus", uBufferSize)) < 0)
        {
            return(iResult);
        }
    }
    else if (pActiveRequst->Filters.eOrder == USERLISTAPI_ORDERBY_ID_AND_STATUS)
    {
        if ((iResult = ds_strnzcat(pBuffer, "&order=onlineStatus+onlineId", uBufferSize)) < 0)
        {
            return(iResult);
        }
    }

    return(iResult);
}

/*F*************************************************************************************************/
/*!
    \Function   _UserListApiQueueListQuery

    \Description
        Add a query to the list to be processed

    \Input *pRef        - module ref
    \Input  uUserIndex  - local user index
    \Input  eType       - type of query
    \Input  iLimit      - maximum users to return in the query
    \Input  iOffset     - query offset
    \Input *pFilter      - query filters
    \Input *pNotifyCb   - user callback
    \Input  uTypeMask   - mask
    \Input *pUserData   - pointer to user data for callback

    \Output
        int32_t         - 0 for success, negative for error

    \Version 01/28/2020 (tcho)
*/
/*************************************************************************************************F*/
static int32_t _UserListApiQueueListQuery(UserListApiRefT *pRef, uint32_t uUserIndex, UserListApiTypeE eType, int32_t iLimit, int32_t iOffset, UserListApiFiltersT *pFilter, UserListApiCallbackT *pNotifyCb, uint32_t uTypeMask, void *pUserData)
{
    NetCritEnter(&pRef->crit);

    // create a new request and put it in the list
    UserListRequestT *pRequest = NULL;
    if ((pRequest = (UserListRequestT*)DirtyMemAlloc(sizeof(UserListRequestT), USERLISTAPI_MEMID, pRef->iMemGroup, pRef->pMemGroupUserData)) == NULL)
    {
        NetPrintf(("userlistapips5: [%p] could not allocate memory for UserListRequestT\n"));
        NetCritLeave(&pRef->crit);
        return(-1);
    }

    ds_memclr(pRequest, sizeof(UserListRequestT));

    // Copy the data into the permanent request object
    switch (eType)
    {
        case USERLISTAPI_TYPE_FRIENDS:
        case USERLISTAPI_TYPE_BLOCKED:
            pRequest->eRequestState = USERLISTAPI_GETACCOUNTIDS;
            break;

        default:
            NetPrintf(("userlistapips5: unexpected request type\n"));
            DirtyMemFree(pRequest, USERLISTAPI_MEMID, pRef->iMemGroup, pRef->pMemGroupUserData);
            NetCritLeave(&pRef->crit);
            return(USERLISTAPI_ERROR_UNSUPPORTED);
    };

    // put the request at the head of the list
    pRequest->pNext = pRef->pRequestList;
    pRef->pRequestList = pRequest;

    pRequest->eRequestType = eType;
    pRequest->pNotifyCb = pNotifyCb;
    pRequest->pUserData = pUserData;
    pRequest->iLimit = iLimit;
    pRequest->iOffset = iOffset;
    pRequest->uUserIndex = uUserIndex;
    pRequest->uTypeMask = uTypeMask;

    if (pFilter != NULL)
    {
        ds_memcpy((void*)&(pRequest->Filters), pFilter, sizeof(UserListApiFiltersT));
    }

    NetCritLeave(&pRef->crit);
    return(0);
}

/*F*************************************************************************************************/
/*!
    \Function   _UserListApiHandleParseError

    \Description
        Get friends list

    \Input *pRef        - module ref
    \Input *pEventData  - event data
    \Input *pRequest    - active request

    \Version 01/27/2020 (tcho)
*/
/*************************************************************************************************F*/
static void _UserListApiHandleParseError(UserListApiRefT *pRef, UserListApiEventDataT *pEventData, UserListRequestT *pRequest)
{
    ds_memclr(pEventData, sizeof(UserApiEventDataT));
    pEventData->ListEndData.pUserListFilters = &pRequest->Filters;
    pEventData->ListEndData.uUserIndex = pRequest->uUserIndex;
    pEventData->ListEndData.uTypeMask = pRequest->uTypeMask;
    pEventData->ListEndData.Error = -1;
    pRequest->pNotifyCb(pRef, TYPE_LIST_END, pEventData, pRequest->pUserData);
}

/*F*************************************************************************************************/
/*!
    \Function   _UserListApiGetAccountIds

    \Description
        Get friends / blocking list

    \Input *pRef            - module ref
    \Input *pRequest        - active request

    \Output
        int32_t             - positive for success, negative for error

    \Version 01/27/2020 (tcho)
*/
/*************************************************************************************************F*/
static int32_t _UserListApiGetAccountIds(UserListApiRefT *pRef, UserListRequestT *pRequest)
{
    int32_t iResult;
    int64_t iWebRequestId;
    SceNpAccountId accountId;
    char strPath[USERLISTAPI_PATH_LEN_MAX];

    if ((iResult = NetConnStatus('acct', pRequest->uUserIndex, &accountId, sizeof(accountId))) < 0)
    {
        NetPrintf(("userlistapips5: [%p]  cannont get account id for user index %d\n", pRef, pRequest->uUserIndex));
        return(iResult);
    }

    // construct the base path with the local users account id
    if (pRequest->eRequestType == USERLISTAPI_REQUEST_TYPE_FRIENDS)
    {
        ds_snzprintf(strPath, sizeof(strPath), USERLISTAPI_PATH_FRIENDS, accountId);
    }
    else if (pRequest->eRequestType == USERLISTAPI_REQUEST_TYPE_BLOCKED)
    {
        ds_snzprintf(strPath, sizeof(strPath), USERLISTAPI_PATH_BLOCKED);
    }

    // add query parameters
    if ((iResult = _UserListApiAddQueryParamsToRequest(pRequest, strPath, sizeof(strPath))) < 0)
    {
        NetPrintf(("userlistapi5: [%p] cannot add query parameters to the request\n"));
        return(iResult);
    }

    //send request
    if ((iWebRequestId = DirtyWebApiRequest(pRef->pWebApi, pRequest->uUserIndex, USERLISTAPI_API_GROUP, SCE_NP_WEBAPI2_HTTP_METHOD_GET, strPath, NULL, 0, NULL, &_UserListApiDirtyWebApiCallback, pRef)) < 0)
    {
        NetPrintf(("userlistapi5: [%p]  WebApi request failed\n"));
        return((int32_t)iWebRequestId);
    }

    pRequest->iWebRequestId = iWebRequestId;

    return(iResult);
}

/*F*************************************************************************************************/
/*!
    \Function   _UserListApiAddAccountIdArray

    \Description
        Add array of account ids to path

    \Input *pRef          - module ref
    \Input *pBuffer       - [out] buffer where the request is stored
    \Input uBufferSize    - size of the buffer

    \Output
        int32_t           - positive for success, negative for error

    \Version 01/27/2020 (tcho)
*/
/*************************************************************************************************F*/
static int32_t _UserListApiAddAccountIdArray(UserListApiRefT *pRef, char *pBuffer, uint32_t uBufferSize)
{
    int32_t iResult = 0;
    int32_t iIndex = 0;

    if ((iResult = ds_strnzcat(pBuffer, "?accountIds=", uBufferSize)) < 0)
    {
        return(iResult);
    }

    for (; (pRef->aAccountIds[iIndex] != SCE_NP_INVALID_ACCOUNT_ID); ++iIndex)
    {
        char strAccountId[64];

        if ((iResult = ds_snzprintf(strAccountId, sizeof(strAccountId), (iIndex == 0) ? "%llu" : ",%llu", pRef->aAccountIds[iIndex])) < 0)
        {
            return(iResult);
        }

        if ((iResult = ds_strnzcat(pBuffer, strAccountId, uBufferSize)) < 0)
        {
            return(iResult);
        }
    }

    return(iResult);
}

/*F*************************************************************************************************/
/*!
    \Function   _UserListApiParseGetAccountIdsResult

    \Description
        Parse get friends list / get blocking list results

    \Input *pRef            - module ref
    \Input *pRequest        - active request

    \Output
        int32_t             - 1 for waiting, 0 for success , negative for error

    \Version 01/27/2020 (tcho)
*/
/*************************************************************************************************F*/
static int32_t _UserListApiParseGetAccountIdsResult(UserListApiRefT *pRef, UserListRequestT *pRequest)
{
    char strRawData[USERLISTAPI_ONE_ENTRY_BUFFER_SIZE];
    int64_t iNextOffset = 0;
    int64_t iPrevOffset = 0;
    int64_t iSize;
    UserListApiEventDataT EventData;

    // variables for parsing
    uint16_t *pJsonParse;
    const char *pCurElement;
    const char *pList;
    const char *pEntryEnd;

    // native id varaiable 
    char strNativeId[DIRTYUSER_NATIVEUSER_MAXSIZE];

    // check if response is ready to be processed
    if (pRef->bDataReady == FALSE)
    {
        return(1);
    }
    else if (pRef->iReadError < 0)
    {
        // get a WebApi error
        _UserListApiHandleParseError(pRef, &EventData, pRequest);
        return(-1);
    }

    // clear account id array
    pRef->uAccountIdsNum = 0;
    ds_memclr(&pRef->aAccountIds[0], sizeof(SceNpAccountId) * USERLISTAPI_MAX_ACCOUNTIDS);

    // allocate space for JSON parser
    if ((pJsonParse = JsonParse2(pRef->pReadBuffer, pRef->iReadBufferSize, USERLISTAPI_MEMID, pRef->iMemGroup, pRef->pMemGroupUserData)) == NULL)
    {
        _UserListApiHandleParseError(pRef, &EventData, pRequest);
        NetPrintf(("userlistapips5: [%p] could allocate space for json parser\n", pRef));
        return(-1);
    }

    // extract total item count 
    if ((pCurElement = JsonFind(pJsonParse, "totalItemCount")) == NULL)
    {
        _UserListApiHandleParseError(pRef, &EventData, pRequest);
        NetPrintf(("userlistapips5: [%p] unexpected json response\n", pRef));
        return(-2);
    }
    
    pRef->uTotalFrdsCount = (uint32_t)JsonGetInteger(pCurElement, 0);

    // extract next offset 
    if ((pCurElement = JsonFind(pJsonParse, "nextOffset")) != NULL)
    {
        iNextOffset = JsonGetInteger(pCurElement, 0);
    }

    // extract previous offset
    if ((pCurElement = JsonFind(pJsonParse, "previousOffset")) != NULL)
    {
        iPrevOffset = JsonGetInteger(pCurElement, 0);
    }

    // extract online id 
    iSize = (iNextOffset != 0) ? (iNextOffset - iPrevOffset - 1) : pRef->uTotalFrdsCount;

    for (int32_t iElement = 0; iElement < iSize; ++iElement)
    {
        if ((pList = JsonFind2(pJsonParse, NULL, (pRequest->eRequestType == USERLISTAPI_TYPE_FRIENDS) ? "friends[" : "blocks[", iElement)) == NULL)
        {
            _UserListApiHandleParseError(pRef, &EventData, pRequest);
            NetPrintf(("userlistapips5: [%p] unexpected json response\n", pRef));
            return(-3);
        }

        if (JsonGetString(pList, strNativeId, sizeof(strNativeId), "") < 0)
        {
            _UserListApiHandleParseError(pRef, &EventData, pRequest);
            NetPrintf(("userlistapips5: [%p] unexpected json response\n", pRef));
            return(-4);
        }

        // save down the list of account ids (use to make further requests)
        pRef->uAccountIdsNum++;
        pRef->aAccountIds[iElement] = ds_strtoull(strNativeId, NULL, 10);

        // return the results to caller if no further resquest required
        if (pRequest->uTypeMask == 0)
        {
            ds_memclr(&EventData, sizeof(EventData));
            ds_memclr(&strRawData, sizeof(strRawData));

            if (pRequest->eRequestType == USERLISTAPI_TYPE_FRIENDS)
            {
                EventData.UserData.bIsMutualFriend = TRUE;  // always mutual friend on PSN
                EventData.UserData.bIsFavorite = FALSE;     // no such concept on PSN
            }

            EventData.UserData.eRequestType = pRequest->eRequestType;
            EventData.UserData.pUserListFilters = &pRequest->Filters;
            EventData.UserData.uUserIndex = pRequest->uUserIndex;
            DirtyUserFromNativeUser(&EventData.UserData.DirtyUser, &pRef->aAccountIds[iElement]);

            // populate raw data
            if ((pEntryEnd = JsonGetListItemEnd(pList)) == NULL)
            {
                _UserListApiHandleParseError(pRef, &EventData, pRequest);
                NetPrintf(("userlistapips5: [%p] unexpected json response\n", pRef));
                return(-5);
            }

            ds_strnzcpy(strRawData, pList, pEntryEnd - pList + 1);
            EventData.UserData.pRawData = &strRawData[0];

            // send the callback for a single user
            pRequest->pNotifyCb(pRef, TYPE_USER_DATA, &EventData, pRequest->pUserData);
        }
    }

    // notify a list end if needed
    if ((pRequest->uTypeMask == 0) || (pRef->uTotalFrdsCount == 0))
    {
        ds_memclr(&EventData, sizeof(EventData));
        EventData.ListEndData.eRequestType = pRequest->eRequestType;
        EventData.ListEndData.pUserListFilters = &pRequest->Filters;
        EventData.ListEndData.uUserIndex = pRequest->uUserIndex;
        EventData.ListEndData.uTypeMask = pRequest->uTypeMask;
        EventData.ListEndData.iLimit = pRequest->iLimit;
        EventData.ListEndData.iOffset = pRequest->iOffset;
        EventData.ListEndData.iTotalFriendCount = pRef->uTotalFrdsCount;
        pRequest->pNotifyCb(pRef, TYPE_LIST_END, &EventData, pRequest->pUserData);

        // clear the type mask to skip getting other list (there are no users)
        pRequest->uTypeMask = 0;
    }

    DirtyMemFree(pJsonParse, USERLISTAPI_MEMID, pRef->iMemGroup, pRef->pMemGroupUserData);
    pRef->bDataReady = FALSE;
    return(0);
}

/*F*************************************************************************************************/
/*!
    \Function   _UserListApiGetList

    \Description
         Get profiles / presences / block user state list

    \Input *pRef            - module ref
    \Input *pRequest        - active request

    \Output
        int32_t             - positive for success, negative for error

    \Version 01/27/2020 (tcho)
*/
/*************************************************************************************************F*/
static int32_t _UserListApiGetList(UserListApiRefT *pRef, UserListRequestT *pRequest)
{
    int32_t iResult;
    int64_t iWebRequestId;
    char strPath[USERLISTAPI_PATH_LEN_MAX];

    // set the correct base path based on the mask type
    if (pRequest->uTypeMask & USERLISTAPI_MASK_PROFILE)
    {
        pRequest->uMaskProcessed = USERLISTAPI_MASK_PROFILE;
        ds_snzprintf(strPath, sizeof(strPath), USERLISTAPI_PATH_PROFILES);
    }
    else if (pRequest->uTypeMask & USERLISTAPI_MASK_PRESENCE)
    {
        pRequest->uMaskProcessed = USERLISTAPI_MASK_PRESENCE;
        ds_snzprintf(strPath, sizeof(strPath), USERLISTAPI_PATH_PRESENCES);
    }
    else if (pRequest->uTypeMask & USERLISTAPI_MASK_RICH_PRESENCE)
    {
        // on ps5 rich presence is activity
        pRequest->uMaskProcessed = USERLISTAPI_MASK_RICH_PRESENCE;
        ds_snzprintf(strPath, sizeof(strPath), USERLISTAPI_PATH_ACTIVITY);
    }
    else
    {
        NetPrintf(("userlistapips5: [%p] unsupported mask type %d\n", pRef, pRequest->uTypeMask));
        return(-1);
    }

    if ((iResult = _UserListApiAddAccountIdArray(pRef, strPath, sizeof(strPath))) < 0)
    {
        NetPrintf(("userlistapips5: [%p] failed to add account id array\n", pRef));
        return(iResult);
    }

    // if this is an acitvity request we will limit this to return the only the lastest result
    if (pRequest->uMaskProcessed == USERLISTAPI_MASK_RICH_PRESENCE)
    {
        if ((iResult = ds_strnzcat(strPath, "&limit=10", sizeof(strPath))) < 0)
        {
            NetPrintf(("userlistapips5: [%p] failed to add limit to activity request (user %d)\n", pRef));
            return(iResult);
        }
    }

    //send request
    if ((iWebRequestId = DirtyWebApiRequest(pRef->pWebApi, pRequest->uUserIndex,(pRequest->uMaskProcessed == USERLISTAPI_MASK_RICH_PRESENCE) ? USERLISTAPI_ACTIVITY_API_GROUP : USERLISTAPI_API_GROUP, SCE_NP_WEBAPI2_HTTP_METHOD_GET, strPath, NULL, 0, NULL, &_UserListApiDirtyWebApiCallback, pRef)) < 0)
    {
        NetPrintf(("userlistapi5: [%p]  WebApi request failed\n", pRef));
        return((int32_t)iWebRequestId);
    }

    pRequest->iWebRequestId = iWebRequestId;

    return(iResult);
}

/*F*************************************************************************************************/
/*!
    \Function   _UserListApiParseGetListResult

    \Description
         Parse get profiles / presences / block user state list results

    \Input *pRef            - module ref
    \Input *pRequest        - active request

    \Output
        int32_t             - 1 for waiting, 0 for success , negative for error

    \Version 01/27/2020 (tcho)
*/
/*************************************************************************************************F*/
static int32_t _UserListApiParseGetListResult(UserListApiRefT *pRef, UserListRequestT *pRequest)
{
    char strRawData[USERLISTAPI_ONE_ENTRY_BUFFER_SIZE];
    UserListApiEventDataT EventData;

    // variables for parsing
    uint16_t *pJsonParse;
    const char *pList;
    const char *pCurElement;
    const char *pEntryEnd;

    // check if response is ready to be processed
    if (pRef->bDataReady == FALSE)
    {
        return(1);
    }
    else if (pRef->iReadError < 0)
    {
        // get a WebApi error
        _UserListApiHandleParseError(pRef, &EventData, pRequest);
        return(-1);
    }

    // allocate space for JSON parser
    if ((pJsonParse = JsonParse2(pRef->pReadBuffer, pRef->iReadBufferSize, USERLISTAPI_MEMID, pRef->iMemGroup, pRef->pMemGroupUserData)) == NULL)
    {
        _UserListApiHandleParseError(pRef, &EventData, pRequest);
        NetPrintf(("userlistapips5: [%p] could allocate space for json parser\n", pRef));
        return(-2);
    }

    // go through array of results
    for (uint32_t iElement = 0; iElement < pRef->uAccountIdsNum; ++iElement)
    {
        char strTmpString[DIRTYUSER_NATIVEUSER_MAXSIZE] = { 0 };
        ds_memclr(&EventData, sizeof(EventData));

        switch (pRequest->uMaskProcessed)
        {
            case USERLISTAPI_MASK_PROFILE:
            {
                const char *pAvatarList;
                const char *pLocaleList;

                if ((pList = JsonFind2(pJsonParse, NULL, "profiles[", iElement)) == NULL)
                {
                    _UserListApiHandleParseError(pRef, &EventData, pRequest);
                    DirtyMemFree(pJsonParse, USERLISTAPI_MEMID, pRef->iMemGroup, pRef->pMemGroupUserData);
                    NetPrintf(("userlistapips5: [%p] unexpected json response\n", pRef));
                    return(-3);
                }

                // parse online id which is the gamer tag on PSN
                pCurElement = JsonFind2(pJsonParse, pList, ".onlineId", iElement);
                JsonGetString(pCurElement, EventData.UserData.ExtendedUserData.Profile.strGamertag, sizeof(EventData.UserData.ExtendedUserData.Profile.strGamertag), "");

                // parse display name
                if ((pCurElement = JsonFind2(pJsonParse, pList, ".personalDetail.firstName", iElement)) != NULL)
                {
                    // if the personal name is not private, the display name is firstName_lastName
                    JsonGetString(pCurElement, strTmpString, sizeof(strTmpString), "");
                    ds_strnzcat(EventData.UserData.ExtendedUserData.Profile.strDisplayName, strTmpString, sizeof(EventData.UserData.ExtendedUserData.Profile.strDisplayName));
                    pCurElement = JsonFind2(pJsonParse, pList, ".personalDetail.lastName", iElement);
                    JsonGetString(pCurElement, strTmpString, sizeof(strTmpString), "");
                    ds_strnzcat(EventData.UserData.ExtendedUserData.Profile.strDisplayName, " ", sizeof(EventData.UserData.ExtendedUserData.Profile.strDisplayName));
                    ds_strnzcat(EventData.UserData.ExtendedUserData.Profile.strDisplayName, strTmpString, sizeof(EventData.UserData.ExtendedUserData.Profile.strDisplayName));
                }
                else
                {
                    // if the personal name is private the display name is the online id
                    ds_strnzcpy(EventData.UserData.ExtendedUserData.Profile.strDisplayName, EventData.UserData.ExtendedUserData.Profile.strGamertag, SCE_NP_ONLINEID_MAX_LENGTH + 1);
                }

                // get the correct avatar url
                for (int32_t iAvatarIndex = 0; iAvatarIndex < USERLISTAPI_AVATAR_LIST_SIZE; ++iAvatarIndex)
                {
                    char strAvatarSize[4] = { 0 };
                    pAvatarList = JsonFind2(pJsonParse, pList, ".avatars[", iAvatarIndex);
                    pCurElement = JsonFind2(pJsonParse, pAvatarList, ".size", iAvatarIndex);
                    JsonGetString(pCurElement, strAvatarSize, sizeof(strAvatarSize), "");

                    // grab the correct avatar size url
                    if (ds_stricmp(strAvatarSize, _UserListApi_strAvatarSize[pRequest->Filters.eAvatarSize]) == 0)
                    {
                        pCurElement = JsonFind2(pJsonParse, pAvatarList, ".url", iAvatarIndex);
                        JsonGetString(pCurElement, EventData.UserData.ExtendedUserData.Profile.strAvatarUrl, sizeof(EventData.UserData.ExtendedUserData.Profile.strAvatarUrl), "");
                        break;
                    }
                }

                // grab the locale code
                pLocaleList = JsonFind2(pJsonParse, pList, ".languages[", 0);
                JsonGetString(pLocaleList, strTmpString, sizeof(strTmpString), "");

                // remove the underscore
                memmove(strTmpString + 2, strTmpString + 3, sizeof(strTmpString) - 2);
                EventData.UserData.ExtendedUserData.Profile.uLocale = LOBBYAPI_LocalizerTokenCreateFromString(strTmpString);
                break;
            }
            case USERLISTAPI_MASK_PRESENCE:
            {
                if ((pList = JsonFind2(pJsonParse, NULL, "basicPresences[", iElement)) == NULL)
                {
                    _UserListApiHandleParseError(pRef, &EventData, pRequest);
                    DirtyMemFree(pJsonParse, USERLISTAPI_MEMID, pRef->iMemGroup, pRef->pMemGroupUserData);
                    NetPrintf(("userlistapips5: [%p] unexpected json response\n", pRef));
                    return(-4);
                }

                // parse online status
                pCurElement = JsonFind2(pJsonParse, pList, ".onlineStatus", iElement);
                JsonGetString(pCurElement, strTmpString, sizeof(strTmpString), "");

                if (ds_stricmp(strTmpString, "online") == 0)
                {
                    EventData.UserData.ExtendedUserData.Presence.ePresenceStatus = USERAPI_PRESENCE_ONLINE;
                }
                else if (ds_stricmp(strTmpString, "offline") == 0)
                {
                    EventData.UserData.ExtendedUserData.Presence.ePresenceStatus = USERAPI_PRESENCE_OFFLINE;
                }
                else
                {
                    EventData.UserData.ExtendedUserData.Presence.ePresenceStatus = USERAPI_PRESENCE_UNKNOWN;
                }

                // parse inContext
                pCurElement = JsonFind2(pJsonParse, pList, ".inContext", iElement);

                if (JsonGetBoolean(pCurElement, FALSE) == TRUE)
                {
                    EventData.UserData.ExtendedUserData.Presence.bIsPlayingSameTitle = TRUE;
                }
                else
                {
                    EventData.UserData.ExtendedUserData.Presence.bIsPlayingSameTitle = FALSE;
                }

                break;
            }
            case USERLISTAPI_MASK_RICH_PRESENCE:
            {
                const char *pActivityList;
                uint32_t uActivityIndex = 0;

                if ((pList = JsonFind2(pJsonParse, NULL, "users[", iElement)) == NULL)
                {
                    _UserListApiHandleParseError(pRef, &EventData, pRequest);
                    DirtyMemFree(pJsonParse, USERLISTAPI_MEMID, pRef->iMemGroup, pRef->pMemGroupUserData);
                    NetPrintf(("userlistapips5: [%p] unexpected json response\n", pRef));
                    return(-4);
                }

                // we will only return the latest unhidden activity 
                while ((pActivityList = JsonFind2(pJsonParse, pList, ".activities[", uActivityIndex)) != NULL)
                {
                    pCurElement = JsonFind2(pJsonParse, pActivityList, ".hidden", uActivityIndex);

                    if (JsonGetBoolean(pCurElement, FALSE) == FALSE)
                    {
                        pCurElement = JsonFind2(pJsonParse, pActivityList, ".name", uActivityIndex);
                        JsonGetString(pCurElement, EventData.UserData.ExtendedUserData.RichPresence.strData, sizeof(EventData.UserData.ExtendedUserData.RichPresence.strData), "");

                        break;
                    }

                    uActivityIndex++;
                }

                break;
            }
        }

        ds_memclr(&strRawData, sizeof(strRawData));

        if (pRequest->eRequestType == USERLISTAPI_TYPE_FRIENDS)
        {
            EventData.UserData.bIsMutualFriend = TRUE;  // always mutual friend on PSN
            EventData.UserData.bIsFavorite = FALSE;     // no such concept on PSN
        }

        EventData.UserData.eRequestType = pRequest->eRequestType;
        EventData.UserData.ExtendedUserData.uUserDataMask = pRequest->uMaskProcessed;
        EventData.UserData.pUserListFilters = &pRequest->Filters;
        EventData.UserData.uUserIndex = pRequest->uUserIndex;
        DirtyUserFromNativeUser(&EventData.UserData.DirtyUser, &pRef->aAccountIds[iElement]);

        // populate raw data
        if ((pEntryEnd = JsonGetListItemEnd(pList)) == NULL)
        {
            _UserListApiHandleParseError(pRef, &EventData, pRequest);
            DirtyMemFree(pJsonParse, USERLISTAPI_MEMID, pRef->iMemGroup, pRef->pMemGroupUserData);
            NetPrintf(("userlistapips5: [%p] unexpected json response\n", pRef));
            return(-5);
        }

        ds_strnzcpy(strRawData, pList, pEntryEnd - pList + 1);

        if (pRequest->uMaskProcessed == USERLISTAPI_MASK_PRESENCE)
        {
            EventData.UserData.ExtendedUserData.Presence.pRawData = &strRawData[0];
        }
        else if (pRequest->uMaskProcessed == USERLISTAPI_MASK_RICH_PRESENCE)
        {
            EventData.UserData.ExtendedUserData.RichPresence.pRawData = &strRawData[0];
        }
        else if (pRequest->uMaskProcessed == USERLISTAPI_MASK_PROFILE)
        {
            EventData.UserData.ExtendedUserData.Profile.pRawData = &strRawData[0];
        }

        // send the callback for a single user
        pRequest->pNotifyCb(pRef, TYPE_USER_DATA, &EventData, pRequest->pUserData);
    }

    // this is the end of list
    ds_memclr(&EventData, sizeof(EventData));
    EventData.ListEndData.eRequestType = pRequest->eRequestType;
    EventData.ListEndData.pUserListFilters = &pRequest->Filters;
    EventData.ListEndData.uUserIndex = pRequest->uUserIndex;
    EventData.ListEndData.uTypeMask = pRequest->uMaskProcessed;
    EventData.ListEndData.iLimit = pRequest->iLimit;
    EventData.ListEndData.iOffset = pRequest->iOffset;
    EventData.ListEndData.iTotalFriendCount = pRef->uTotalFrdsCount;
    pRequest->pNotifyCb(pRef, TYPE_LIST_END, &EventData, pRequest->pUserData);

    DirtyMemFree(pJsonParse, USERLISTAPI_MEMID, pRef->iMemGroup, pRef->pMemGroupUserData);
    pRequest->uTypeMask &= ~pRequest->uMaskProcessed;
    pRef->bDataReady = FALSE;

    return(0);
}

/*F*************************************************************************************************/
/*!
    \Function   _UserListApiProcessRequest

    \Description
        Process queue list queries

    \Input *pRef        - module ref

    \Output
        int32_t         - positive for success, negative for error

    \Version 01/28/2020 (tcho)
*/
/*************************************************************************************************F*/
static int32_t _UserListApiProcessRequest(UserListApiRefT *pRef)
{
    uint8_t bCleanupRequest = FALSE;
    int32_t iResult = 0;
    UserListRequestT *pCurrent = NULL;
    UserListRequestT *pLast = NULL;

    if (pRef->pActiveRequest == NULL)
    {
        if ((pCurrent = pRef->pRequestList) == NULL)
        {
            // no request to handle
            return(0);
        }

        // search for the last entry
        while (pCurrent->pNext != NULL)
        {
            pLast = pCurrent;
            pCurrent = pCurrent->pNext;
        }

        // remove the last request from the list for processing
        if (pLast)
        {
            pLast->pNext = NULL;
        }
        else
        {
            pRef->pRequestList = NULL;
        }

        pRef->pActiveRequest = pCurrent;
    }

    // process current active request state
    switch (pRef->pActiveRequest->eRequestState)
    {
        case USERLISTAPI_GETACCOUNTIDS:
        {
            // submit webapi request
            if ((iResult = _UserListApiGetAccountIds(pRef, pRef->pActiveRequest)) < 0)
            {
                NetPrintf(("userlistapips5: [%p] encountered error with the %s (%s) request, error code %d\n", pRef,
                    (pRef->pActiveRequest->eRequestType == USERLISTAPI_TYPE_FRIENDS) ? "USERLISTAPI_TYPE_FRIENDS" : "USELISTAPI_TYPE_BLOCKED",
                    _UserListApi_strRequestState[USERLISTAPI_GETACCOUNTIDS], iResult));

                bCleanupRequest = TRUE;
            }
            // move on to the wait state
            else
            {
                pRef->pActiveRequest->eRequestState = USERLISTAPI_GETACCOUNTIDS_WAIT;
            }
            break;
        }
        case USERLISTAPI_GETACCOUNTIDS_WAIT:
        {
            // try to parse results 
            if ((iResult = _UserListApiParseGetAccountIdsResult(pRef, pRef->pActiveRequest)) < 0)
            {
                NetPrintf(("userlistapips5: [%p] encountered error with parsing result %s (%s), error code %d\n", pRef,
                    (pRef->pActiveRequest->eRequestType == USERLISTAPI_TYPE_FRIENDS) ? "USERLISTAPI_TYPE_FRIENDS" : "USELISTAPI_TYPE_BLOCKED",
                    _UserListApi_strRequestState[USERLISTAPI_GETACCOUNTIDS_WAIT], iResult));

                bCleanupRequest = TRUE;
            }
            // finished with the current request but there are more requests to do
            else if ((pRef->pActiveRequest->uTypeMask != 0) && (iResult != 1))
            {
                pRef->pActiveRequest->eRequestState = USERLISTAPI_GETLIST;
            }
            // done with request
            else if (iResult != 1)
            {
                bCleanupRequest = TRUE;
            }
            break;
        }
        case USERLISTAPI_GETLIST:
        {
            if ((iResult = _UserListApiGetList(pRef, pRef->pActiveRequest)) < 0)
            {
                NetPrintf(("userlistapips5: [%p] encoutered error with get list %s (%s) request (mask = %d) error code %d\n", pRef,
                    (pRef->pActiveRequest->eRequestType == USERLISTAPI_TYPE_FRIENDS) ? "USERLISTAPI_TYPE_FRIENDS" : "USELISTAPI_TYPE_BLOCKED",
                    _UserListApi_strRequestState[USERLISTAPI_GETACCOUNTIDS_WAIT], pRef->pActiveRequest->uMaskProcessed, iResult));

                bCleanupRequest = TRUE;
            }
            else
            {
                pRef->pActiveRequest->eRequestState = USERLISTAPI_GETLIST_WAIT;
            }
            break;
        }

        case USERLISTAPI_GETLIST_WAIT:
        {
            if ((iResult = _UserListApiParseGetListResult(pRef, pRef->pActiveRequest)) < 0)
            {
                NetPrintf(("userlistapips5: [%p] encoutered error with parsing get list results %s (%s) request (mask = %d) error code %d\n", pRef,
                    (pRef->pActiveRequest->eRequestType == USERLISTAPI_TYPE_FRIENDS) ? "USERLISTAPI_TYPE_FRIENDS" : "USELISTAPI_TYPE_BLOCKED",
                    _UserListApi_strRequestState[USERLISTAPI_GETACCOUNTIDS_WAIT],
                    pRef->pActiveRequest->uMaskProcessed, iResult));

                bCleanupRequest = TRUE;
            }
            else if ((pRef->pActiveRequest->uTypeMask != 0) && (iResult >= 0) && (iResult != 1))
            {
                // we need to do more requests
                pRef->pActiveRequest->eRequestState = USERLISTAPI_GETLIST;
            }
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
        DirtyMemFree(pRef->pActiveRequest, USERLISTAPI_MEMID, pRef->iMemGroup, pRef->pMemGroupUserData);
        pRef->bDataReady = FALSE;
        pRef->pActiveRequest = NULL;
    }

    return(iResult);
}

/*F*************************************************************************************************/
/*!
    \Function   _UserListApiProcessNotifications

    \Description
        Process first party list update notifications

    \Input *pRef        - module ref

    \Output
        int32_t         - positive for success, negative for error

    \Version 02/13/2020 (tcho)
*/
/*************************************************************************************************F*/
static void _UserListApiProcessNotifications(UserListApiRefT *pRef)
{
    if (pRef->aUserListApiNotifyEvent[0].pNotificationList == NULL)
    {
        // no notification to process
        return;
    }

    // at least one notification is in the list, do processing
    for (uint32_t uPsnEventIndex = 0; uPsnEventIndex < USERLISTAPI_MAX_QUEUED_NOTIFICATIONS; ++uPsnEventIndex)
    {
        UserListApiNotificationT *pNotificationList = pRef->aUserListApiNotifyEvent[uPsnEventIndex].pNotificationList;
        uint32_t uNotficationUserId = pRef->aUserListApiNotifyEvent[uPsnEventIndex].uUserIndex;

        if (pRef->aUserListApiNotifyEvent[uPsnEventIndex].pNotificationList == NULL)
        {
            continue;
        }

        // dispatch notifications
        for (uint32_t uNotifyCbIndex = 0; uNotifyCbIndex < USERLISTAPI_NOTIFY_LIST_MAX_SIZE; ++uNotifyCbIndex)
        {
            if (pNotificationList[uNotifyCbIndex].pCallback == NULL)
            {
                continue;
            }

            // if the notification is destined for the appropriate local user dispatch it 
            if (uNotficationUserId == pNotificationList[uNotifyCbIndex].uUserIndex)
            {
                pNotificationList[uNotifyCbIndex].pCallback(pRef,
                    pRef->aUserListApiNotifyEvent[uPsnEventIndex].eNotificationType,
                    &pRef->aUserListApiNotifyEvent[uPsnEventIndex].NotificationData,
                    pRef->aUserListApiNotifyEvent[uPsnEventIndex].pNotificationList[uNotifyCbIndex].pUserData);
            }
        }

        // clean up
        pRef->aUserListApiNotifyEvent[uPsnEventIndex].pNotificationList = NULL;
    }
}

/*F*************************************************************************************************/
/*!
    \Function   _UserListOnWebApiUpdate

    \Description
        Callback for when friends update events happen.

        \Input iUserCtxId        - user web api context
        \Input iCallbackId       - callback id generated at registration
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
static void _UserListOnWebApiUpdate(int32_t iUserCtxId, int32_t iCallbackId, const char *pNpServiceName, SceNpServiceLabel npServiceLabel, const SceNpPeerAddressA *pTo, const SceNpPeerAddressA *pFrom, const SceNpWebApi2PushEventDataType *pDataType, const char *pData, size_t dataLen, void *pUserArg)
{
    UserListApiRefT *pRef;
    UserListApiNotificationT *pList = NULL;
    UserListApiNotifyTypeE eType = USERLISTAPI_NOTIFY_FRIENDLIST_UPDATE;
    uint32_t uNotifyIndex = 0;
    uint32_t uUserIndex = 0; // default to user 0

    if (pUserArg == NULL)
    {
        NetPrintf(("userlistapips5: null UserListApiRefT from pUserArg.\n"));
        return;
    }

    pRef = (UserListApiRefT*)pUserArg;
    NetPrintf(("userlistapips5: [%p] detected userlist updated event.\n", pRef));

    // find the correct user whom this is for
    for (uint32_t uIndex = 0; uIndex < NETCONN_MAXLOCALUSERS; ++uIndex)
    {
        SceNpAccountId AccountId;
        int32_t iReturn = NetConnStatus('acct', uIndex, &AccountId, sizeof(AccountId));

        if ((iReturn >= 0) && (AccountId == pTo->accountId))
        {
            uUserIndex = uIndex;
            break;
        }
    }

    NetCritEnter(&pRef->crit);
    // find an empty slot
    for (uNotifyIndex = 0; uNotifyIndex < USERLISTAPI_MAX_QUEUED_NOTIFICATIONS; ++uNotifyIndex)
    {
        UserListApiNotifyDataT *pNotifyData = &pRef->aUserListApiNotifyEvent[uNotifyIndex].NotificationData;

        if (pRef->aUserListApiNotifyEvent[uNotifyIndex].pNotificationList == NULL)
        {
            continue;
        }

        if (ds_stricmp(pDataType->val, "np:service:friendlist:friend") == 0)
        {
            pList = pRef->aFriendNotificaton;
            eType = USERLISTAPI_NOTIFY_FRIENDLIST_UPDATE;
            DirtyUserFromNativeUser(&pNotifyData->FriendListData.DirtyUser, &pTo->accountId);
            pNotifyData->FriendListData.uUserIndex = uUserIndex;
        }
        else if (ds_stricmp(pDataType->val, "np:service:blocklist") == 0)
        {
            pList = pRef->aBlockedNotificaton;
            eType = USERLISTAPI_NOTIFY_BLOCKEDLIST_UPDATE;
            DirtyUserFromNativeUser(&pNotifyData->BlockedListData.DirtyUser, &pTo->accountId);
            pNotifyData->BlockedListData.uUserIndex = uUserIndex;
        }
        else
        {
            NetPrintf(("userlistapips5: received unknown push event: %s\n", pDataType->val));
            NetCritLeave(&pRef->crit);
            return;
        }

        pRef->aUserListApiNotifyEvent[uNotifyIndex].pNotificationList = pList;
        pRef->aUserListApiNotifyEvent[uNotifyIndex].eNotificationType = eType;
        pRef->aUserListApiNotifyEvent[uNotifyIndex].uUserIndex = uUserIndex;
        break;
    }

    if (uNotifyIndex == USERLISTAPI_MAX_QUEUED_NOTIFICATIONS)
    {
        NetPrintf(("userlistapips5: maximum notifications in queue exceeded, notification dropped\n"));
    }

    NetCritLeave(&pRef->crit);
}

/*** Public functions ********************************************************/

/*F*************************************************************************************************/
/*!
    \Function   UserListApiCreate

    \Description
        Startup UserListApi module.

    \Input uMaxResponseSize     - size of the ProtoHttpBuffer (not used)

    \Output
        UserListApiRefT*        - pointer to new ref or NULL for error

    \Version 01/28/2020 (tcho)
*/
/*************************************************************************************************F*/
UserListApiRefT *UserListApiCreate(uint32_t uMaxResponseSize)
{
    UserListApiRefT *pState = NULL;
    int32_t iMemGroup;
    void *pMemGroupUserData;

    // Query current mem group data
    DirtyMemGroupQuery(&iMemGroup, &pMemGroupUserData);

    // allocate and init module state
    if ((pState = (UserListApiRefT*)DirtyMemAlloc(sizeof(*pState), USERLISTAPI_MEMID, iMemGroup, pMemGroupUserData)) == NULL)
    {
        NetPrintf(("userlistapips5: could not allocate module state\n"));
        return(NULL);
    }
    ds_memclr(pState, sizeof(*pState));
    pState->iMemGroup = iMemGroup;
    pState->pMemGroupUserData = pMemGroupUserData;

    pState->iReadBufferSize = USERLISTAPI_DEFAULT_READ_BUFFER_SIZE;
    if ((pState->pReadBuffer = (char*)DirtyMemAlloc(pState->iReadBufferSize, USERLISTAPI_MEMID, pState->iMemGroup, pState->pMemGroupUserData)) == NULL)
    {
        NetPrintf(("userlistapips5: could not allocate space for the pReadBuffer\n"));
        DirtyMemFree(pState, USERLISTAPI_MEMID, iMemGroup, pMemGroupUserData);
        return(NULL);
    }

    // retrieve webapi from netconn
    NetConnStatus('wapi', 0, &pState->pWebApi, sizeof(&pState->pWebApi));

    if (pState->pWebApi == NULL)
    {
        NetPrintf(("userlistapips5: webapi is null!\n"));
        return(NULL);
    }

    // increase the buffer size so it will at least hold 100 responses
    DirtyWebApiControl(pState->pWebApi, 'rbuf', USERLISTAPI_WEBAPI_BUFFER, 0, NULL);

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

    \Version 01/28/2020 (tcho)
*/
/*************************************************************************************************F*/
void UserListApiDestroy(UserListApiRefT *pRef)
{
    UserListApiCancelAll(pRef);

    // destroy crit
    NetCritKill(&pRef->crit);

    DirtyMemFree(pRef->pReadBuffer, USERLISTAPI_MEMID, pRef->iMemGroup, pRef->pMemGroupUserData);

    // free the memory
    DirtyMemFree(pRef, USERLISTAPI_MEMID, pRef->iMemGroup, pRef->pMemGroupUserData);

    NetPrintf(("userlistapips5: [%p] freed userlistapi\n", pRef));
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

    \Version 01/28/2020 (tcho)
*/
/*************************************************************************************************F*/
int32_t UserListApiRegisterUpdateEvent(UserListApiRefT *pRef, uint32_t uUserIndex, UserListApiNotifyTypeE eType, UserListApiUpdateCallbackT *pNotifyCb, void *pUserData)
{
    int32_t iReturn = 0;
    UserListApiNotificationT(*pList)[] = NULL;

    if (eType == USERLISTAPI_NOTIFY_FRIENDLIST_UPDATE)
    {
        pList = &pRef->aFriendNotificaton;
        if (pRef->bFriendNotificationStarted != TRUE)
        {
            pRef->bFriendNotificationStarted = TRUE;
            iReturn = DirtyWebApiAddPushEventListener(pRef->pWebApi, "np:service:friendlist:friend", _UserListOnWebApiUpdate, pRef);
        }
    }
    else if (eType == USERLISTAPI_NOTIFY_BLOCKEDLIST_UPDATE)
    {
        pList = &pRef->aBlockedNotificaton;
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

    for (int32_t iListIndex = 0; iListIndex < USERLISTAPI_NOTIFY_LIST_MAX_SIZE; ++iListIndex)
    {
        if ((*pList)[iListIndex].pCallback == NULL)
        {
            (*pList)[iListIndex].pCallback = pNotifyCb;
            (*pList)[iListIndex].pUserData = pUserData;
            (*pList)[iListIndex].uUserIndex = uUserIndex;
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

    \Version 01/28/2020 (tcho)
*/
/*************************************************************************************************F*/
int32_t UserListApiIsFriendAsync(UserListApiRefT *pRef, uint32_t uUserIndex, DirtyUserT *pUser, UserListApiIsAQueryCallbackT *pNotifyCb, void *pUserData)
{
    return(USERLISTAPI_ERROR_UNSUPPORTED);
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

    \Version 01/28/2020 (tcho)
*/
/*************************************************************************************************F*/
int32_t UserListApiIsBlockedAsync(UserListApiRefT *pRef, uint32_t uUserIndex, DirtyUserT *pUser, UserListApiIsAQueryCallbackT *pNotifyCb, void *pUserData)
{
    return(USERLISTAPI_ERROR_UNSUPPORTED);
}

/*F*************************************************************************************************/
/*!
    \Function   UserListApiUpdate

    \Description
        Updates UserListApi module.

    \Input *pRef - module ref

    \Version 01/28/2020 (tcho)
*/
/*************************************************************************************************F*/
void UserListApiUpdate(UserListApiRefT *pRef)
{
    if (!NetCritTry(&pRef->crit))
    {
        return;
    }

    _UserListApiProcessRequest(pRef);
    _UserListApiProcessNotifications(pRef);

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

    \Version 01/28/2020 (tcho)
*/
/*************************************************************************************************F*/
int32_t UserListApiStatus(UserListApiRefT *pRef, int32_t iSelect, void *pBuf, int32_t iBufSize)
{
    NetPrintf(("userlistapips5: unhandled status selector '%C'\n", iSelect));
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

    \Version 01/28/2020 (tcho)
*/
/*************************************************************************************************F*/
int32_t UserListApiControl(UserListApiRefT *pRef, int32_t iControl, int32_t iValue, int32_t iValue2, void *pValue)
{
    NetPrintf(("userlistapips5: unhandled control selector '%C'\n", iControl));
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
    \Input *pFilter     - PS5 specific filter set
    \Input *pNotifyCb   - callback function pointer
    \Input uTypeMask    - mask for types of data expected
    \Input *pUserData   - user data

    \Output
        int32_t         - negative for error, positive for success

    \Version 01/28/2020 (tcho)
*/
/*************************************************************************************************F*/
int32_t UserListApiGetListAsync(UserListApiRefT *pRef, uint32_t uUserIndex, UserListApiTypeE eType, int32_t iLimit, int32_t iOffset, UserListApiFiltersT *pFilter, UserListApiCallbackT *pNotifyCb, uint32_t uTypeMask, void *pUserData)
{
    int32_t iReturn = 0;

    if ((uUserIndex >= NETCONN_MAXLOCALUSERS) || (iLimit <= 0))
    {
        // user index out of range
        NetPrintf(("userlistapips5: [%p] UserListApiGeListAsync() called with invalid parameters.\n", pRef));
        return(USERLISTAPI_ERROR_BAD_PARAM);
    }

    if ((eType == USERLISTAPI_TYPE_FRIENDS) && (iLimit > USERLISTAPI_FRIENDS_MAX_REQUEST_RESULT))
    {
        // over the limit for friends list request
        NetPrintf(("userlistapips5: [%p] UserListApiGeListAsync() friends list request only supports a max limit of %d.\n", pRef, USERLISTAPI_FRIENDS_MAX_REQUEST_RESULT));
        return(USERLISTAPI_ERROR_BAD_PARAM);
    }

    if (eType == USERLISTAPI_TYPE_BLOCKED)
    {
        if (iLimit > USERLISTAPI_BLOCKED_MAX_REQUEST_RESULT)
        {
            // over the limit for blocked list request
            NetPrintf(("userlistapips5: [%p] UserListApiGeListAsync() blocked list request only supports a limit of %d.\n", pRef, USERLISTAPI_BLOCKED_MAX_REQUEST_RESULT));
            return(USERLISTAPI_ERROR_BAD_PARAM);
        }

        // we do not support filters or querying profiles and presences for blocked users
        pFilter = NULL;
        uTypeMask = 0;
    }

    switch (eType)
    {
        case USERLISTAPI_TYPE_FRIENDS:
        case USERLISTAPI_TYPE_BLOCKED:
        {
            _UserListApiQueueListQuery(pRef, uUserIndex, eType, iLimit, iOffset, pFilter, pNotifyCb, uTypeMask, pUserData);
            break;
        }
        default:
            iReturn = USERLISTAPI_ERROR_UNSUPPORTED;
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

    \Version 01/28/2020 (tcho)
*/
/*************************************************************************************************F*/
void UserListApiCancelAll(UserListApiRefT *pRef)
{
    NetCritEnter(&pRef->crit);

    // delete all the requests in queue
    if (pRef->pActiveRequest)
    {
        if ((pRef->pActiveRequest->eRequestState == USERLISTAPI_GETACCOUNTIDS_WAIT) || (pRef->pActiveRequest->eRequestState == USERLISTAPI_GETLIST_WAIT))
        {
            DirtyWebApiAbortRequestById(pRef->pWebApi, -1, pRef->pActiveRequest->iWebRequestId);
        }

        DirtyMemFree(pRef->pActiveRequest, USERLISTAPI_MEMID, pRef->iMemGroup, pRef->pMemGroupUserData);
        pRef->pActiveRequest = NULL;
    }
    while (pRef->pRequestList != NULL)
    {
        UserListRequestT *pNext = pRef->pRequestList->pNext;
        DirtyMemFree(pRef->pRequestList, USERLISTAPI_MEMID, pRef->iMemGroup, pRef->pMemGroupUserData);
        pRef->pRequestList = pNext;
    }

    NetCritLeave(&pRef->crit);
}
