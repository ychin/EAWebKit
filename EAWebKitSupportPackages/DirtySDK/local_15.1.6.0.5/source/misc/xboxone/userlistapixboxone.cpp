/*H********************************************************************************/
/*!
    \File userlistapixboxone.cpp

    \Description
        See userlistapi.h

    \Copyright
        Copyright (c) Electronic Arts 2013

    \Version 1.0 25/04/2013 (amakoukji) First Version
*/
/********************************************************************************H*/

/*** Include files ***********************************************************/

#include "DirtySDK/platform.h"

#include <xdk.h>
#include <collection.h>

#include "DirtySDK/misc/userlistapi.h"
#include "DirtySDK/misc/xboxone/userlistapixboxone.h"

#include "DirtySDK/dirtysock.h"
#include "DirtySDK/dirtysock/dirtyerr.h"
#include "DirtySDK/dirtysock/dirtylib.h"
#include "DirtySDK/dirtysock/dirtymem.h"
#include "DirtySDK/dirtysock/netconn.h"
#include "DirtySDK/proto/protohttp.h"
#include "DirtySDK/util/jsonparse.h"
#include "DirtySDK/util/utf8.h"

using namespace Platform;
using namespace Platform::Collections;
using namespace Windows::Data;
using namespace Windows::Foundation;
using namespace Windows::Foundation::Collections;
using namespace Windows::Xbox::System;
using namespace Microsoft::Xbox::Services;
using namespace Microsoft::Xbox::Services::Social;
using namespace Microsoft::Xbox::Services::Presence;

/*** Defines ***************************************************************************/
#define USERLISTAPI_READ_BUFFER_SIZE                (2*64)
#define USERLISTAPI_AUTH_TOKEN_SIZE                 (16 * 1024)
#define USERLISTAPI_REQUEST_PATH_SIZE               (512)
#define USERLISTAPI_ISFRIEND_PATH                   "https://social.xboxlive.com/users/xuid(%llu)/people/xuid(%llu)"
#define USERLISTAPI_HTTP_READ_BUFFER_SIZE           (2*1024)
#define USERLISTAPI_HTTP_HEADER_BUFFER_SIZE         (1024)
#define USERLISTAPI_NOT_IN_PEOPLE_LIST_ERROR_CODE   (1016)
#define USERLISTAPI_MYSELF_ERROR_CODE               (1017)
#define USERLISTAPI_ASYNC_TIMOUT_MS                 (20000) // 20 seconds
#define HOME_MENU_TITLE_ID                          (714681658)
#define HTTP_RESPONSE_RATE_LIMITING                 (429)
#define HTTP_RESPONSE_HEADER_RETRY_AFTER_FIELD_W    L"retry-after:"
#define HTTP_RESPONSE_HEADER_RETRY_AFTER_FIELD      "retry-after:"

/*** Macros ****************************************************************************/

/*** Type Definitions ******************************************************************/

//! query types
typedef enum UserListApiRequestTypeE
{
    USERLISTAPI_REQUEST_TYPE_FRIENDS,
    USERLISTAPI_REQUEST_TYPE_BLOCKED,
    USERLISTAPI_REQUEST_TYPE_FRIENDS_OF_FRIEND,
    USERLISTAPI_REQUEST_TYPE_MUTED,
    USERLISTAPI_REQUEST_TYPE_ISFRIENDS,
    USERLISTAPI_REQUEST_TYPE_ISBLOCKED
} UserListApiRequestTypeE;

//! flags for data synchronization
// $todo - mclouatre - Aug. 12 2014
// we don't really need these sub-status...  all logics involving them can be changed to use _UserListApiIsAsyncOpInProgress()
typedef struct UserListApiPlatformRequestListT
{
    bool bWaitingForFriendsList;
    bool bWaitingForPresenceList;
    bool bWaitingForProfileList;
} UserListApiPlatformRequestListT;

//! result struct
struct UserListApiResultT
{
    bool bResponseProcessed;                        // flag to see if first party data has been processed
    bool bNeedsMemFreed;
    XboxSocialRelationshipResult^  refResultFriend;    // friend data from first party
    IVectorView<XboxUserProfile^>^ refResultProfile;   // profile data from first party
    IVectorView<PresenceRecord^>^  refResultPresence;  // presence data from first party
    int32_t iErrorFriend;                           // error code
    int32_t iErrorProfile;                          // error code
    int32_t iErrorPresence;                         // error code
    UserListApiResultT* pNext;                      // the next item in the list

    SocialRelationship SocialView;                  // storage for request parameters
    uint32_t uUserIndex;
    UserListApiRequestTypeE eRequestType;
    int32_t iLimit;
    int32_t iOffset;
    uint32_t uTypeMask;
    UserListApiCallbackT *pNotifyCb;
    UserListApiIsAQueryCallbackT *pIsAQueryNotifyCb;
    DirtyUserT pTargetUserId;
    UserListApiPlatformRequestListT SubQueryStatuses;
    void *pUserData;
};

//! structure to store in the queue of requests
typedef struct UserListApiRequestT
{
    uint32_t uUserIndex;
    UserListApiRequestTypeE eType;
    int32_t iLimit;
    int32_t iOffset;
    UserListApiFiltersT Filter;
    UserListApiCallbackT *pNotifyCb;
    uint32_t uTypeMask;
    void *pUserData;
} UserListApiRequestT;

//! platform context and operation objects for MS queries
typedef struct UserListApiPlatformUserContextT
{
    XboxLiveContext ^refXblUserContext;
    IAsyncOperation<XboxSocialRelationshipResult^>  ^refAsyncOpFriend;
    IAsyncOperation<IVectorView<XboxUserProfile^>^> ^refAsyncOpProfile;
    IAsyncOperation<IVectorView<PresenceRecord^>^>  ^refAsyncOpPresence;

    uint32_t uOpFriendTimer;
    uint32_t uOpProfileTimer;
    uint32_t uOpPresenceTimer;

    int32_t  iRateLimRetrySecs;
} UserListApiPlatformUserContextT;

typedef struct UserListApiIsFriendQueueT
{
    char strRequest[USERLISTAPI_REQUEST_PATH_SIZE];
    UserListApiResultT *pAssociatedResult;
    uint32_t uUserIndex;
    UserListApiIsFriendQueueT *pNext;
} UserListApiIsFriendQueueT;

//! module state
struct UserListApiRefT
{
    // module memory group
    int32_t  iMemGroup;             //!< module mem group id
    void    *pMemGroupUserData;     //!< user data associated with mem group

    UserListApiPlatformUserContextT aXblContext[NETCONN_MAXLOCALUSERS];

    User^    refUsers[NETCONN_MAXLOCALUSERS];
    NetCritT crit;                  //!< critical section used to protect ResultList
    UserListApiResultT *ResultList; //!< list of result to be treated

    ProtoHttpRefT *pHttp;           //!< ProtoHttp
    uint32_t       uHttpOpTimer;
    UserListApiIsFriendQueueT *pIsFriendRequestQueue;

    uint8_t        bShuttingDown;
    uint8_t        _pad[3];
};

/*** Function Prototypes ***************************************************************/

/*** Variables *************************************************************************/

/*** Private functions ******************************************************/

/*F*************************************************************************************************/
/*!
    \Function _UserListApiIsAsyncOpInProgress

    \Description
        Checks if any async op is in progress for the specified user index.

    \Input *pRef            - module ref
    \Input iLocalUserIndex  - user index

    \Output
        uint32_t            - TRUE or FALSE

    \Version 08/12/2014 (mclouatre)
*/
/*************************************************************************************************F*/
static uint32_t _UserListApiIsAsyncOpInProgress(UserListApiRefT *pRef, int32_t iLocalUserIndex)
{
    if (pRef->aXblContext[iLocalUserIndex].refAsyncOpFriend != nullptr ||
        pRef->aXblContext[iLocalUserIndex].refAsyncOpProfile != nullptr ||
        pRef->aXblContext[iLocalUserIndex].refAsyncOpPresence != nullptr)
    {
        return(TRUE);
    }

    return(FALSE);
}

/*F*************************************************************************************************/
/*!
    \Function _UserListApiFreeCallback

    \Description
        Callback registered with the netconn external cleanup mechanism. It proceeds with destroying
        the UserListApi instance only when there is no longer an internal async operation in progress
        and the memory can safely be freed.

    \Input *pMem     - pointer to the UserListApi memory buffer

    \Output
        int32_t      - zero=success; -1=try again; other negative=error

    \Version 09/25/2013 (amakoukji)
*/
/*************************************************************************************************F*/
static int32_t _UserListApiFreeCallback(void *pMem)
{
    UserListApiRefT *pRef = (UserListApiRefT*)pMem;

    for (uint32_t i = 0; i < NETCONN_MAXLOCALUSERS; ++i)
    {
        if (_UserListApiIsAsyncOpInProgress(pRef, i))
        {
            return(-1);
        }
    }

    ProtoHttpDestroy(pRef->pHttp);

    while (pRef->ResultList)
    {
        UserListApiResultT *pNext = pRef->ResultList->pNext;
        pRef->ResultList->refResultFriend = nullptr;
        pRef->ResultList->refResultPresence = nullptr;
        pRef->ResultList->refResultProfile = nullptr;
        if (pRef->ResultList->bNeedsMemFreed)
        {
            DirtyMemFree(pRef->ResultList, USERLISTAPI_MEMID, pRef->iMemGroup, pRef->pMemGroupUserData);
        }
        pRef->ResultList = pNext;
    }

    for (int32_t iIndex = 0; iIndex < NETCONN_MAXLOCALUSERS; ++iIndex)
    {
        pRef->refUsers[iIndex] = nullptr;
        pRef->aXblContext[iIndex].refAsyncOpFriend = nullptr;
        pRef->aXblContext[iIndex].refAsyncOpProfile = nullptr;
        pRef->aXblContext[iIndex].refAsyncOpPresence = nullptr;
        pRef->aXblContext[iIndex].refXblUserContext = nullptr;
    }

    // free any queued isFriend requests
    UserListApiIsFriendQueueT *pTmpQueuePtr = pRef->pIsFriendRequestQueue;
    while (pTmpQueuePtr != NULL)
    {
        UserListApiIsFriendQueueT *pNext = pTmpQueuePtr->pNext;
        DirtyMemFree(pTmpQueuePtr, USERLISTAPI_MEMID, pRef->iMemGroup, pRef->pMemGroupUserData);
        pTmpQueuePtr = pNext;
    }

    NetCritKill(&pRef->crit);

    NetPrintf(("userlistapixboxone: [%p] destroy complete\n", pRef));
    DirtyMemFree(pRef, USERLISTAPI_MEMID, pRef->iMemGroup, pRef->pMemGroupUserData);
    return(0);
}

/*F*************************************************************************************************/
/*!
    \Function _UserListApiGetPriorityTitle

    \Description
        Choose the most relevant title from the list provided.
        The logic should be identical to the equivalent method in userapi.

    \Input presenceDeviceRecordList - list of active title presence data

    \param refOutTitleRecord  - Highest priority title, or nullptr if failed
    \param refOutDeviceRecord - The device presence record that contains the highest priority title

    \Version 04/24/2014 (amakoukji)
*/
/*************************************************************************************************F*/
static void _UserListApiGetPriorityTitle(IVectorView<PresenceDeviceRecord^>^ presenceDeviceRecordList, PresenceDeviceRecord^& refOutDeviceRecord, PresenceTitleRecord^& refOutTitleRecord)
{
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

    uint32_t uCurrentTitleId = _wcstoui64(Windows::Xbox::Services::XboxLiveConfiguration::TitleId->Data(), NULL, 10);
    
    PresenceTitleRecord ^refTitlePriority1 = nullptr;
    PresenceTitleRecord ^refTitlePriority2 = nullptr;
    PresenceTitleRecord ^refTitlePriority3 = nullptr;
    PresenceTitleRecord ^refTitlePriority4 = nullptr;
    PresenceTitleRecord ^refTitlePriority5 = nullptr;
    PresenceDeviceRecord ^refDevicePriority1 = nullptr;
    PresenceDeviceRecord ^refDevicePriority2 = nullptr;
    PresenceDeviceRecord ^refDevicePriority3 = nullptr;
    PresenceDeviceRecord ^refDevicePriority4 = nullptr;
    PresenceDeviceRecord ^refDevicePriority5 = nullptr;
   
    IIterator<PresenceDeviceRecord^> ^refDeviceIter = nullptr;
    IIterator<PresenceTitleRecord^> ^refTitleIter = nullptr;
    
    uint8_t bSameTitle = FALSE;

    if (presenceDeviceRecordList == nullptr)
    {
        return;
    }

    refDeviceIter = presenceDeviceRecordList->First();

    // rank by priorities
    while (refDeviceIter->HasCurrent)
    {
        PresenceDeviceRecord ^refDeviceRecord = refDeviceIter->Current;
        if (refDeviceRecord != nullptr)
        {
            refTitleIter = refDeviceRecord->PresenceTitleRecords->First();

            while (refTitleIter->HasCurrent)
            {
                PresenceTitleRecord ^refTitleRecord = refTitleIter->Current;

                if (refTitleRecord->IsTitleActive && refDeviceRecord->DeviceType == Microsoft::Xbox::Services::Presence::PresenceDeviceType::XboxOne && refTitleRecord->TitleId == uCurrentTitleId )
                {
                    refTitlePriority1 = refTitleRecord;
                    refDevicePriority1 = refDeviceRecord;
                    bSameTitle = TRUE;
                    break;
                }
                else if (refTitleRecord->IsTitleActive && refDeviceRecord->DeviceType == Microsoft::Xbox::Services::Presence::PresenceDeviceType::XboxOne && refTitleRecord->TitleId != HOME_MENU_TITLE_ID)
                {
                    refTitlePriority2 = refTitleRecord;
                    refDevicePriority2 = refDeviceRecord;
                }
                else if (refTitleRecord->IsTitleActive && refDeviceRecord->DeviceType == Microsoft::Xbox::Services::Presence::PresenceDeviceType::XboxOne)
                {
                    refTitlePriority3 = refTitleRecord;
                    refDevicePriority3 = refDeviceRecord;
                }
                else if (refTitleRecord->IsTitleActive)
                {
                    refTitlePriority4 = refTitleRecord;
                    refDevicePriority4 = refDeviceRecord;
                }
                else
                {
                    refTitlePriority5 = refTitleRecord;
                    refDevicePriority5 = refDeviceRecord;
                }

                refTitleIter->MoveNext();
            }
        }

        refDeviceIter->MoveNext();
        if (bSameTitle == TRUE)
        {
            break;
        }
    }

    if ((refTitlePriority1 != nullptr) && (refDevicePriority1 != nullptr))
    {
        refOutDeviceRecord = refDevicePriority1;
        refOutTitleRecord = refTitlePriority1;
    }
    else if ((refTitlePriority2 != nullptr) && (refDevicePriority2 != nullptr))
    {
        refOutDeviceRecord = refDevicePriority2;
        refOutTitleRecord = refTitlePriority2;
    }
    else if ((refTitlePriority3 != nullptr) && (refDevicePriority3 != nullptr))
    {
        refOutDeviceRecord = refDevicePriority3;
        refOutTitleRecord = refTitlePriority3;
    }
    else if ((refTitlePriority4 != nullptr) && (refDevicePriority4 != nullptr))
    {
        refOutDeviceRecord = refDevicePriority4;
        refOutTitleRecord = refTitlePriority4;
    }
    else if ((refTitlePriority5 != nullptr) && (refDevicePriority5 != nullptr))
    {
        refOutDeviceRecord = refDevicePriority5;
        refOutTitleRecord = refTitlePriority5;
    }
}

/*F*************************************************************************************************/
/*!
    \Function _UserListApiProcessIsFriend

    \Description
        Process IsFriend requests by cleaning up previously completed operations and 
        starting queued ones.

    \Input *pRef     - pointer to the UserListApiRefT
    
    \Version 05/08/2015 (amakoukji)
*/
/*************************************************************************************************F*/
static void _UserListApiProcessIsFriend(UserListApiRefT *pRef)
{
    UserListApiIsADataT   isAQueryData;
    UserListApiIsFriendQueueT *pTmpQueuePtr = pRef->pIsFriendRequestQueue;

    ds_memclr(&isAQueryData, sizeof(UserListApiIsADataT));
    isAQueryData.eIsaType = USERLISTAPI_IS_TYPE_UNKNOWN;

    // start next isFriend request in the queue
    if (pTmpQueuePtr != NULL)
    {
        int32_t iReturn = 0;

        // remove pTmpQueuePtr from the queue and start the request
        pRef->pIsFriendRequestQueue = pTmpQueuePtr->pNext;

        //Append XSTS Token with Request
        ProtoHttpControl(pRef->pHttp, 'xtok', 1, pTmpQueuePtr->uUserIndex, NULL);
        iReturn = ProtoHttpGet(pRef->pHttp, pTmpQueuePtr->strRequest, 0 );

        // check for errors
        if (iReturn < 0)
        {
            // if we fail, callback and remove result from results queue
            NetPrintf(("userlistapixboxone: [%p] UserListApiIsFriendAsync() ProtoHttp Failed", pRef));
            isAQueryData.Error = USERLISTAPI_ERROR_UNKNOWN;
            if (pTmpQueuePtr->pAssociatedResult->pIsAQueryNotifyCb != NULL)
            {
                pTmpQueuePtr->pAssociatedResult->pIsAQueryNotifyCb(pRef, USERLISTAPI_IS_FRIENDS, &isAQueryData, pTmpQueuePtr->pAssociatedResult->pUserData);
            }

            // find and remove from results queue
            UserListApiResultT *pPrev = NULL;
            UserListApiResultT *pCurr = pRef->ResultList;
            while (pCurr)
            {
                if (pCurr == pTmpQueuePtr->pAssociatedResult)
                {
                    if (pPrev)
                    {
                        pPrev->pNext = pCurr->pNext;
                    }
                    else
                    {
                        pRef->ResultList = pCurr->pNext;
                    }

                    pCurr->refResultFriend = nullptr;
                    pCurr->refResultPresence = nullptr;
                    pCurr->refResultProfile = nullptr;
                    DirtyMemFree(pCurr, USERLISTAPI_MEMID, pRef->iMemGroup, pRef->pMemGroupUserData);
                    break;
                }

                pPrev = pCurr;
                pCurr = pCurr->pNext;
            }
            
        }

        // free the UserListApiIsFriendQueueT
        DirtyMemFree(pTmpQueuePtr, USERLISTAPI_MEMID, pRef->iMemGroup, pRef->pMemGroupUserData);
    }
}

/*F*************************************************************************************************/
/*!
    \Function _GetRetryAfterFieldFromHTTPResponseHeader

    \Description
        Get the "Retry-After" value from the given HTTP response header

    \Input *strRespHeader     - pointer to the HTTP response header string

    \Output
        int32_t      - "Retry-After" value from the given HTTP response header else -1 if not found

    \Version 01/31/2018 (dminton)
*/
/*************************************************************************************************F*/
static int32_t _GetRetryAfterFieldFromHTTPResponseHeader(const char* strRespHeader)
{
    int32_t iRetryAfter = -1;

    if (nullptr != strRespHeader)
    {
        const char *strRetryAfter = ds_stristr(strRespHeader, HTTP_RESPONSE_HEADER_RETRY_AFTER_FIELD);
        if (nullptr != strRetryAfter)
        {
            sscanf(strRetryAfter + strlen(HTTP_RESPONSE_HEADER_RETRY_AFTER_FIELD), "%d", &iRetryAfter);
        }
    }

    return(iRetryAfter);
}

/*F*************************************************************************************************/
/*!
    \Function _GetRetryAfterFieldFromHTTPResponseHeaderW

    \Description
        Get the "Retry-After" value from the given HTTP response header (wide characters)

    \Input *strRespHeader     - pointer to the HTTP response header string (wide characters)

    \Output
        int32_t      - "Retry-After" value from the given HTTP response header else -1 if not found
    
    \Version 01/31/2018 (dminton)
*/
/*************************************************************************************************F*/
static int32_t _GetRetryAfterFieldFromHTTPResponseHeaderW(const wchar_t* strRespHeader)
{
    int32_t iRetryAfter = -1;

    if (nullptr != strRespHeader)
    {
        // convert to normal char
        int32_t iDataLen = (int32_t)wcstombs(NULL, strRespHeader, wcslen(strRespHeader));
        int32_t iMemGroup;
        void *pMemGroupUserData;
        char *pStr;

        DirtyMemGroupQuery(&iMemGroup, &pMemGroupUserData);
        pStr = (char *)DirtyMemAlloc(iDataLen + 1 , USERLISTAPI_MEMID, iMemGroup, pMemGroupUserData);
       
        if (pStr != NULL)
        {
            wcstombs(pStr, strRespHeader, iDataLen + 1);
            pStr[iDataLen] = '\0';
            iRetryAfter = _GetRetryAfterFieldFromHTTPResponseHeader(pStr);
            DirtyMemFree(pStr, USERLISTAPI_MEMID, iMemGroup, pMemGroupUserData);
        }
        else
        {
            NetPrintf(("userlistapixboxone: _GetRetryAfterFieldFromHTTPResponseHeaderW() could not allocate memory\n"));
        }
    }

    return(iRetryAfter);
}

/*** Public functions ********************************************************/


/*F*************************************************************************************************/
/*!
    \Function   UserListApiCreate

    \Description
        Startup UserListApi module.

    \Input uMaxResponseSize  - size of the ProtoHttpBuffer

    \Output
        UserListApiRefT*    - pointer to reference

    \Version 04/17/2013 (amakoukji)
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
        NetPrintf(("userlistapixboxone: could not allocate module state\n"));
        return(NULL);
    }
    ds_memclr(pState, sizeof(*pState));
    pState->iMemGroup = iMemGroup;
    pState->pMemGroupUserData = pMemGroupUserData;
    pState->pHttp = ProtoHttpCreate(30 * 1024);

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
    pRef->bShuttingDown = TRUE;

    // we intentionally do not cancel requests because the behavior of cancelled requests is undefined
    // instead we allow the jobs to finish normally and watch _UserListApiIsAsyncOpInProgress()
    //UserListApiCancelAll(pRef);

    if (_UserListApiFreeCallback((void*)pRef) < 0)
    {
        NetPrintf(("userlistapixboxone: [%p] destroy deferred to netconn due to pending async operation.\n", pRef));
        NetConnControl('recu', 0, 0, (void *)_UserListApiFreeCallback, pRef);
    }
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
        int32_t         - 0 for success, negative for error

    \Version 02/14/2014 (amakoukji)
*/
/*************************************************************************************************F*/
int32_t UserListApiRegisterUpdateEvent(UserListApiRefT *pRef, uint32_t uUserIndex, UserListApiNotifyTypeE eType, UserListApiUpdateCallbackT *pNotifyCb, void *pUserData)
{
    // First party does not support registering user list update event.
    // Note: Hopefully this will change in the future based on our feedback to MS
    return(USERLISTAPI_ERROR_UNSUPPORTED);
}

/*F*************************************************************************************************/
/*!
    \Function   UserListApiIsFriendAsync

    \Description
        Updates UserListApiXboxOne module.

    \Input *pRef  -         - module ref
    \Input  uUserIndex      - current user index
    \Input *pUser           - target user id (XUID for xboxone)
    \Input *pNotifyCb       - callback
    \Input *pUserData       - user data

    \Output
        int32_t             - 0 for success, negative for error

    \Version 07/09/2013 (tcho)
*/
/*************************************************************************************************F*/
int32_t UserListApiIsFriendAsync(UserListApiRefT *pRef, uint32_t uUserIndex, DirtyUserT *pUser, UserListApiIsAQueryCallbackT *pNotifyCb, void *pUserData)
{
    char requestPath[USERLISTAPI_REQUEST_PATH_SIZE];
    uint64_t uCurrentUserXUID;
    int32_t iReturn = 0;
    int32_t iGuestRet;
    uint64_t uTargetXuid;

    if (uUserIndex >= NETCONN_MAXLOCALUSERS)
    {
        NetPrintf(("userlistapixboxone: UserListApiIsFriendAsync()  called with invalid user index %d.\n", uUserIndex));
        return(USERLISTAPI_ERROR_USER_INDEX_RANGE);
    }
    
    // check if we have a guest user
    iGuestRet = NetConnStatus('gust', uUserIndex, NULL, 0);
    
    if (iGuestRet == 1)
    {
        NetPrintf(("userlistapixboxone: UserListApiIsFriendAsync() cannot perform this operation with a guest.\n"));
        return(USERLISTAPI_ERROR_GUEST);
    }
    else if (iGuestRet < 0)
    {
        NetPrintf(("userlistapixboxone: UserListApiIsFriendAsync() failed to determine if the user is a non-psn guest or not. Ignoring request.\n"));
        return(USERLISTAPI_ERROR_INVALID_USER);
    }
    
    if (pRef->pHttp != NULL)
    {
        //Grab the current user's XUID
        if (NetConnStatus('xuid', uUserIndex, &uCurrentUserXUID, sizeof(uCurrentUserXUID)))
        {
            NetPrintf(("userlistapixboxone: [%p] UserListApiIsFriendAsync() Failed to retrieve xuid for user at index %i \n", pRef, uUserIndex));
            return(-4);
        }

        // Extract the target user's XUID
        DirtyUserToNativeUser(&uTargetXuid, sizeof(uTargetXuid), pUser);

        //Builds the request path
        iReturn = ds_snzprintf(requestPath, sizeof(requestPath), USERLISTAPI_ISFRIEND_PATH, uCurrentUserXUID, uTargetXuid);
        if (iReturn < 0)
        {
            return(-5);
        }

        //Adding a new result to the result list
        NetCritEnter(&pRef->crit);

        UserListApiIsFriendQueueT *pLastQueuedIsFriendRequest = NULL;
        //Make the actual request if the queue is empty
        if (pRef->ResultList == NULL)
        {
            //Append XSTS Token with Request
            ProtoHttpControl(pRef->pHttp, 'xtok', 1, uUserIndex, NULL);
            iReturn = ProtoHttpGet(pRef->pHttp, requestPath, 0 );
            if (iReturn < 0)
            {
                NetPrintf(("userlistapixboxone: [%p] UserListApiIsFriendAsync() ProtoHttp Failed", pRef));
                return(-1);
            }
        }
        else
        {
            // add request to queue
            UserListApiIsFriendQueueT *pQueueItem = (UserListApiIsFriendQueueT*)DirtyMemAlloc(sizeof(UserListApiIsFriendQueueT), USERLISTAPI_MEMID, pRef->iMemGroup , pRef->pMemGroupUserData);
            ds_memclr(pQueueItem, sizeof(*pQueueItem));
            ds_strnzcpy(pQueueItem->strRequest, requestPath, (int32_t)sizeof(pQueueItem->strRequest));
            pQueueItem->uUserIndex = uUserIndex;

            // add to the end of the queue
            UserListApiIsFriendQueueT *pTmpQueuePtr = pRef->pIsFriendRequestQueue;
            if (pTmpQueuePtr == NULL)
            {
                pRef->pIsFriendRequestQueue = pQueueItem;
            }
            else
            {
                while (pTmpQueuePtr->pNext != NULL)
                {
                    pTmpQueuePtr = pTmpQueuePtr->pNext;
                }
                pTmpQueuePtr->pNext = pQueueItem;
            }
            pLastQueuedIsFriendRequest = pTmpQueuePtr;         
        }

        UserListApiResultT *pResults = (UserListApiResultT *)DirtyMemAlloc(sizeof(* pResults), USERLISTAPI_MEMID, pRef->iMemGroup , pRef->pMemGroupUserData);
        ds_memclr(pResults, sizeof(*pResults));
        pResults->bNeedsMemFreed = true;

        // Insert at the end of the list
        UserListApiResultT *pTmp = pRef->ResultList;
        if (pTmp == NULL)
        {
            pRef->ResultList = pResults;
        }
        else
        {
            while (pTmp->pNext != NULL)
            {
                pTmp = pTmp->pNext;
            }
            pTmp->pNext = pResults;
        }

        pResults->pNext = NULL;
        pResults->eRequestType = USERLISTAPI_REQUEST_TYPE_ISFRIENDS;
        pResults->pNotifyCb = NULL;
        pResults->pIsAQueryNotifyCb = pNotifyCb;
        pResults->pUserData = pUserData;
        ds_memcpy_s(&pResults->pTargetUserId, sizeof(DirtyUserT), pUser, sizeof(DirtyUserT));
        pResults->uUserIndex = uUserIndex;
        pResults->SubQueryStatuses.bWaitingForFriendsList = true;
        pResults->SubQueryStatuses.bWaitingForPresenceList = false;
        pResults->SubQueryStatuses.bWaitingForProfileList = false;
        pRef->uHttpOpTimer = NetTick();

        if (pLastQueuedIsFriendRequest)
        {
            pLastQueuedIsFriendRequest->pAssociatedResult = pResults;
        }

        NetCritLeave(&pRef->crit);
    }

    return(iReturn);
}

/*F*************************************************************************************************/
/*!
    \Function   UserListApiIsBlockedAsync

    \Description
        Stub for UserListApiIsBlockedAsync()

    \Input *pRef                - module ref
    \Input  uUserIndex          - current user index
    \Input *pUser               - target user id (XUID for xboxone)
    \Input *pNotifyCb           - callback
    \Input *pUserData           - user data

    \Output
        int32_t                 - 0 for success, negative for error

    \Version 07/09/2013 (tcho)
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

    \Input *pRef    - module ref

    \Version  04/17/2013 (amakoukji)
*/
/*************************************************************************************************F*/
void UserListApiUpdate(UserListApiRefT *pRef)
{
    UserListApiEventDataT data;
    UserListApiIsADataT   isAQueryData;
    UserApiUserDataT     *pUserApiData = &data.UserData.ExtendedUserData;
    UserApiProfileT      *pUserApiProfile = &pUserApiData->Profile;
    UserApiPresenceT     *pUserApiPresence = &pUserApiData->Presence;
    UserApiRichPresenceT *pUserApiRichPresence = &pUserApiData->RichPresence;

    NetCritEnter(&pRef->crit);

    UserListApiResultT *pCurrent  =  pRef->ResultList;
    UserListApiResultT *pPrevious =  NULL;

    while (pCurrent)
    {
        UserListApiFiltersT currentFilters;

        // clear the response buffers
        ds_memclr(&data,                sizeof(UserListApiEventDataT));
        ds_memclr(&isAQueryData,        sizeof(UserListApiIsADataT));

        isAQueryData.eIsaType = USERLISTAPI_IS_TYPE_UNKNOWN;

        // treat the results, starting with the front of the list
        switch(pCurrent->eRequestType)
        {
            case USERLISTAPI_REQUEST_TYPE_FRIENDS:
            {
                // verify that async operations timeouts have not been surpassed

                // $todo - mclouatre - Aug. 12 2014
                // upon timeout, the user's callback should be notified but the result entry should not be freed.
                // it needs to survive until all associated async op are completed.

                uint32_t uCurrentTime = NetTick();
                if (pCurrent->SubQueryStatuses.bWaitingForFriendsList && NetTickDiff(uCurrentTime, pRef->aXblContext[pCurrent->uUserIndex].uOpFriendTimer) > USERLISTAPI_ASYNC_TIMOUT_MS)
                {
                    NetPrintf(("userlistapixboxone: [%p] friend query timed out\n", pRef));
                    pCurrent->SubQueryStatuses.bWaitingForFriendsList = false;
                    pCurrent->iErrorFriend = USERLISTAPI_ERROR_TIMEOUT;
                }
                if (pCurrent->SubQueryStatuses.bWaitingForPresenceList && NetTickDiff(uCurrentTime, pRef->aXblContext[pCurrent->uUserIndex].uOpPresenceTimer) > USERLISTAPI_ASYNC_TIMOUT_MS)
                {
                    NetPrintf(("userlistapixboxone: [%p] presence query timed out\n", pRef));
                    pCurrent->SubQueryStatuses.bWaitingForPresenceList = false;
                    pCurrent->iErrorPresence = USERLISTAPI_ERROR_TIMEOUT;
                }
                if (pCurrent->SubQueryStatuses.bWaitingForProfileList && NetTickDiff(uCurrentTime, pRef->aXblContext[pCurrent->uUserIndex].uOpProfileTimer) > USERLISTAPI_ASYNC_TIMOUT_MS)
                {
                    NetPrintf(("userlistapixboxone: [%p] profile query timed out\n", pRef));
                    pCurrent->SubQueryStatuses.bWaitingForProfileList = false;
                    pCurrent->iErrorProfile = USERLISTAPI_ERROR_TIMEOUT;
                }

                // check if we are done waiting for callbacks
                if (!pCurrent->SubQueryStatuses.bWaitingForFriendsList && !pCurrent->SubQueryStatuses.bWaitingForPresenceList && !pCurrent->SubQueryStatuses.bWaitingForProfileList)
                {
                    // check that no errors occured
                    if (pCurrent->iErrorFriend >= 0 && pCurrent->iErrorPresence >= 0 && pCurrent->iErrorProfile >= 0)
                    {
                        XboxSocialRelationshipResult^  refCurrentResultFriend   = nullptr;
                        IVectorView<PresenceRecord^>^  refCurrentResultPresence = nullptr;
                        IVectorView<XboxUserProfile^>^ refCurrentResultProfile  = nullptr;

                        refCurrentResultFriend = (pCurrent->refResultFriend);

                        if (pCurrent->uTypeMask & USERLISTAPI_MASK_PROFILE)
                        {
                            refCurrentResultProfile = (pCurrent->refResultProfile);
                        }

                        if (pCurrent->uTypeMask & USERLISTAPI_MASK_PRESENCE || pCurrent->uTypeMask & USERLISTAPI_MASK_RICH_PRESENCE)
                        {
                            refCurrentResultPresence = (pCurrent->refResultPresence);
                        }


                        // match up the results
                        // process the current item
                        Platform::Collections::Map<String ^, XboxUserProfile^> ^refProfileMap  = nullptr;
                        Platform::Collections::Map<String ^, PresenceRecord ^> ^refPresenceMap = nullptr;

                        // Populate maps
                        if (refCurrentResultPresence)
                        {
                            refPresenceMap = ref new Platform::Collections::Map<String ^, PresenceRecord ^>();
                            for (uint32_t presIndex = 0; presIndex < refCurrentResultPresence->Size; presIndex++)
                            {
                                PresenceRecord  ^tmpPresence =  refCurrentResultPresence->GetAt(presIndex);
                                refPresenceMap->Insert(tmpPresence->XboxUserId, tmpPresence);
                            }
                        }
                        if (refCurrentResultProfile)
                        {
                            refProfileMap  = ref new Platform::Collections::Map<String ^, XboxUserProfile ^>();
                            for (uint32_t profIndex = 0; profIndex < refCurrentResultProfile->Size; profIndex++)
                            {
                                XboxUserProfile  ^tmpProfile =  refCurrentResultProfile->GetAt(profIndex);
                                refProfileMap->Insert(tmpProfile->XboxUserId, tmpProfile);
                            }
                        }

                        if( refCurrentResultFriend != nullptr && refCurrentResultFriend->Items != nullptr )
                        {
                            IVectorView<XboxSocialRelationship ^>^ list = refCurrentResultFriend->Items;

                            // for each friend in the list find the matching profile and presence data if they are present and populate the response data
                            for( uint32_t index = 0; index < list->Size; index++ )
                            {
                                XboxSocialRelationship ^refPerson  = list->GetAt(index);
                                PresenceRecord  ^refPersonPresence = nullptr;
                                XboxUserProfile ^refPersonProfile  = nullptr;
                                Platform::String ^strXuid = refPerson->XboxUserId;
                                uint64_t uXuid  = (uint64_t)_wtoi64(refPerson->XboxUserId->ToString()->Data());

                                ds_memclr(&data, sizeof(data));

                                // Find matching Presence
                                if (refCurrentResultPresence)
                                {
                                    try
                                    {
                                        refPersonPresence = refPresenceMap->Lookup(refPerson->XboxUserId);
                                    }
                                    catch (Exception ^ e)
                                    {
                                        refPersonPresence = nullptr;
                                        NetPrintf(("userlistapixboxone: [%p] no matching presence was found for user %S\n", pRef, refPerson->XboxUserId->Data()));
                                    }
                                }

                                // Find matching Profile info
                                if (refCurrentResultProfile)
                                {
                                    try
                                    {
                                        refPersonProfile = refProfileMap->Lookup(refPerson->XboxUserId);
                                    }
                                    catch (Exception ^ e)
                                    {
                                        refPersonProfile = nullptr;
                                        NetPrintf(("userlistapixboxone: [%p] no matching profile was found for user %S\n", pRef, refPerson->XboxUserId->Data()));
                                    }
                                }

                                // Populate generic and friend data into response structure
                                DirtyUserFromNativeUser(&data.UserData.DirtyUser, &uXuid);
                                data.UserData.iUserProfileError = (refPersonProfile == nullptr && refPersonPresence == nullptr) ? -1 : 0;
                                data.UserData.eRequestType = USERLISTAPI_TYPE_FRIENDS;
                                data.UserData.bIsFavorite = refPerson->IsFavorite ? TRUE : FALSE;
                                data.UserData.bIsMutualFriend = refPerson->IsFollowingCaller ? TRUE : FALSE;
                                data.UserData.uUserIndex = pCurrent->uUserIndex;
                                data.UserData.pUserListFilters = &currentFilters;
                                data.UserData.pUserListFilters->eLevel = (pCurrent->SocialView == SocialRelationship::Favorite) ? USERLISTAPI_LEVEL_FAVORITE : USERLISTAPI_LEVEL_FRIEND;
                                data.UserData.pRawData = &refPerson;


                                // Populate the profile data into the response
                                if (refPersonProfile)
                                {
                                    try
                                    {
                                        pUserApiData->uUserDataMask |= USERAPI_MASK_PROFILE;

                                        if (refPersonProfile->GameDisplayPictureResizeUri != nullptr)
                                        {
                                            Utf8EncodeFromUCS2(pUserApiProfile->strAvatarUrl, sizeof(pUserApiProfile->strAvatarUrl), (uint16 *)refPersonProfile->GameDisplayPictureResizeUri->DisplayUri->Data());
                                        }
                                        else
                                        {
                                            NetPrintf(("userlistapixboxone: [%p] no game display picture uri found for user %S\n", pRef, refPerson->XboxUserId->Data()));
                                        }

                                        if (refPersonProfile->Gamertag != nullptr)
                                        {
                                            Utf8EncodeFromUCS2(pUserApiProfile->strGamertag, sizeof(pUserApiProfile->strGamertag), (uint16 *)refPersonProfile->Gamertag->Data());
                                        }
                                        else
                                        {
                                            NetPrintf(("userlistapixboxone: [%p] no gamertag found for user %S\n", pRef, refPerson->XboxUserId->Data()));
                                        }

                                        if (refPersonProfile->ApplicationDisplayName != nullptr)
                                        {
                                            Utf8EncodeFromUCS2(pUserApiProfile->strDisplayName, sizeof(pUserApiProfile->strDisplayName), (uint16 *)refPersonProfile->ApplicationDisplayName->Data());
                                        }
                                        else
                                        {
                                            NetPrintf(("userlistapixboxone: [%p] no application display name for use %S\n", pRef, refPerson->XboxUserId->Data()));
                                        }
                                    }
                                    catch (Platform::Exception^ ex)
                                    {
                                        NetPrintf(("userlistapixboxone: [%p] unknown error when reading profile results for user %S\n", pRef, refPerson->XboxUserId->Data()));
                                    }

                                    pUserApiProfile->uLocale = 0; // this info is hidden
                                    pUserApiProfile->pRawData = &refPersonProfile;
                                }

                                // Populate the presence data into the response
                                if (refPersonPresence)
                                {
                                    pUserApiData->uUserDataMask |= USERAPI_MASK_PRESENCE;
                                    pUserApiData->uUserDataMask |= USERAPI_MASK_RICH_PRESENCE;
                                    pUserApiPresence->pRawData   = &refPersonPresence;
                                    pUserApiRichPresence->pRawData = &refPersonPresence;
                                    ds_memcpy_s(&pUserApiData->DirtyUser, sizeof(DirtyUserT), &data.UserData.DirtyUser, sizeof(DirtyUserT));

                                    switch(refPersonPresence->UserState)
                                    {
                                        case  UserPresenceState::Away:
                                            pUserApiPresence->ePresenceStatus = USERAPI_PRESENCE_AWAY;
                                            break;

                                        case UserPresenceState::Offline:
                                            pUserApiPresence->ePresenceStatus = USERAPI_PRESENCE_OFFLINE;
                                            break;

                                        case UserPresenceState::Online:
                                            pUserApiPresence->ePresenceStatus = USERAPI_PRESENCE_ONLINE;
                                            break;

                                        case UserPresenceState::Unknown:
                                            pUserApiPresence->ePresenceStatus = USERAPI_PRESENCE_UNKNOWN;
                                            break;
                                    }

                                    if (refPersonPresence->PresenceDeviceRecords->Size > 0)
                                    {
                                        // Return data from the 1st device the user is online with XBox One with only
                                        PresenceDeviceRecord^ refDeviceRecord = nullptr;
                                        PresenceTitleRecord^ refTitleRecord = nullptr;
                                        uint32_t uCurrentTitleId = _wcstoui64(Windows::Xbox::Services::XboxLiveConfiguration::TitleId->Data(), NULL, 10);

                                         _UserListApiGetPriorityTitle(refPersonPresence->PresenceDeviceRecords, refDeviceRecord, refTitleRecord);

                                        if (refTitleRecord != nullptr)
                                        {
                                            Utf8EncodeFromUCS2 (pUserApiPresence->strTitleName,    sizeof(pUserApiPresence->strTitleName),    (uint16 *)refTitleRecord->TitleName->ToString()->Data());
                                            ds_snzprintf(pUserApiPresence->strTitleId, sizeof(pUserApiPresence->strTitleId), "%u", refTitleRecord->TitleId);
                                            ds_snzprintf(pUserApiPresence->strPlatform, sizeof(pUserApiPresence->strPlatform), "%i", refDeviceRecord->DeviceType);
                                            Utf8EncodeFromUCS2 (pUserApiRichPresence->strData,    sizeof(pUserApiRichPresence->strData),    (uint16 *)refTitleRecord->Presence->ToString()->Data());

                                            if (uCurrentTitleId == refTitleRecord->TitleId)
                                            {
                                                pUserApiPresence->bIsPlayingSameTitle = TRUE;
                                            }
                                        }
                                    }
                                }

                                if (pCurrent->pNotifyCb)
                                {
                                    pCurrent->pNotifyCb(pRef, TYPE_USER_DATA, &data, pCurrent->pUserData);
                                }

                            }

                            // Signal the end of list and send metadata
                            data.ListEndData.eRequestType = USERLISTAPI_TYPE_FRIENDS;
                            data.ListEndData.iLimit = pCurrent->iLimit;
                            data.ListEndData.iOffset = pCurrent->iOffset;
                            data.ListEndData.iTotalFriendCount = (int32_t)refCurrentResultFriend->TotalCount;
                            data.ListEndData.pUserListFilters = &currentFilters;
                            data.ListEndData.pUserListFilters->eLevel = (pCurrent->SocialView == SocialRelationship::Favorite) ? USERLISTAPI_LEVEL_FAVORITE : USERLISTAPI_LEVEL_FRIEND;
                            data.ListEndData.uUserIndex = pCurrent->uUserIndex;
                            data.ListEndData.uTypeMask = pCurrent->uTypeMask;
                            data.ListEndData.Error = 0;
                            data.ListEndData.iRateLimRetrySecs = 0;

                            if (pCurrent->pNotifyCb)
                            {
                                pCurrent->pNotifyCb(pRef, TYPE_LIST_END, &data, pCurrent->pUserData);
                            }

                            pCurrent->bResponseProcessed = true;
                        }
                    }
                    else
                    {
                        // Call the callback with iTotalFriendCount < 0
                        data.ListEndData.eRequestType = USERLISTAPI_TYPE_FRIENDS;
                        data.ListEndData.iLimit = pCurrent->iLimit;
                        data.ListEndData.iOffset = pCurrent->iOffset;
                        data.ListEndData.iTotalFriendCount = -1;
                        data.ListEndData.pUserListFilters = &currentFilters;
                        data.ListEndData.pUserListFilters->eLevel = (pCurrent->SocialView == SocialRelationship::Favorite) ? USERLISTAPI_LEVEL_FAVORITE : USERLISTAPI_LEVEL_FRIEND;
                        data.ListEndData.uUserIndex = pCurrent->uUserIndex;
                        data.ListEndData.uTypeMask = pCurrent->uTypeMask;

                        if (pRef->aXblContext[pCurrent->uUserIndex].iRateLimRetrySecs > 0)
                        {
                            data.ListEndData.Error = USERLISTAPI_ERROR_RATE_LIMITED;
                            data.ListEndData.iRateLimRetrySecs = pRef->aXblContext[pCurrent->uUserIndex].iRateLimRetrySecs;
                        }
                        else
                        {
                            // prioritize error codes, friends then presence then profile
                            data.ListEndData.Error = (pCurrent->iErrorFriend < 0) ? pCurrent->iErrorFriend : ((pCurrent->iErrorPresence < 0) ? pCurrent->iErrorPresence : pCurrent->iErrorProfile);
                            data.ListEndData.iRateLimRetrySecs = 0;
                        }

                        if (pCurrent->pNotifyCb)
                        {
                            pCurrent->pNotifyCb(pRef, TYPE_LIST_END, &data, pCurrent->pUserData);
                        }

                        pCurrent->bResponseProcessed = true;
                    }
                }

            // $todo: when events are queued up check if one is active and start a new one here 
            }
            break;

            case USERLISTAPI_REQUEST_TYPE_ISFRIENDS:
            {
                if (!(pCurrent->bResponseProcessed))
                {
                    char pBuf[USERLISTAPI_HTTP_READ_BUFFER_SIZE];
                    uint32_t iCode = 0;
                    int32_t  iHttpStatus = 0;

                    ProtoHttpUpdate(pRef->pHttp);

                    // check for a timeout
                    if (NetTickDiff(NetTick(), pRef->uHttpOpTimer) > USERLISTAPI_ASYNC_TIMOUT_MS)
                    {
                        pCurrent->bResponseProcessed = true;
                        isAQueryData.Error = USERLISTAPI_ERROR_TIMEOUT;
                        if (pCurrent->pIsAQueryNotifyCb != NULL)
                        {
                            pCurrent->pIsAQueryNotifyCb(pRef, USERLISTAPI_IS_FRIENDS, &isAQueryData, pCurrent->pUserData);
                        }
                    }
                    else
                    {
                        int32_t iStatusCode = 0;
                        if ((iStatusCode = ProtoHttpStatus(pRef->pHttp, 'done', NULL, 0)) > 0)
                        {
                            if ((iHttpStatus = ProtoHttpRecvAll(pRef->pHttp, pBuf, sizeof(pBuf))) > 0)
                            {
                                NetPrintf(("userlistapixboxone: [%p] UserListApiUpdate() Processing IsFriends Response.\n", pRef));

                                uint16_t aJsonList[USERLISTAPI_READ_BUFFER_SIZE];
                                iStatusCode = ProtoHttpStatus(pRef->pHttp, 'code', NULL, 0);
                                const char *pJson = NULL;

                                JsonParse(aJsonList, sizeof(aJsonList)/sizeof(aJsonList[0]), pBuf, sizeof(pBuf));

                                pCurrent->bResponseProcessed = true;

                                ds_memcpy_s(&isAQueryData.DirtyUser, sizeof(DirtyUserT), &pCurrent->pTargetUserId, sizeof(DirtyUserT));
                                isAQueryData.Error = 0;
                                isAQueryData.uUserIndex = pCurrent->uUserIndex;
                                isAQueryData.bIsMutualFriend = FALSE;
                                isAQueryData.bIsFavorite = FALSE;

                                //On Xbox One any target user not on the current user's people list will result in an error.
                                if (iStatusCode == 404 || iStatusCode == 400)
                                {
                                    pJson = JsonFind(aJsonList, "code");
                                    if (pJson == NULL)
                                    {
                                        NetPrintf(("userlistapixboxone: [%p] UserListApiUpdate() Failed to parse IsFriend Json Repsonse.\n", pRef));
                                        iCode = 0;
                                        isAQueryData.Error = USERLISTAPI_ERROR_UNKNOWN;
                                        isAQueryData.eIsaType = USERLISTAPI_IS_TYPE_UNKNOWN;
                                        if (pCurrent->pIsAQueryNotifyCb != NULL)
                                        {
                                            pCurrent->pIsAQueryNotifyCb(pRef, USERLISTAPI_IS_FRIENDS, &isAQueryData, pCurrent->pUserData);
                                        }
                                    }

                                    iCode = JsonGetInteger(pJson, 0);

                                    if (iCode == USERLISTAPI_NOT_IN_PEOPLE_LIST_ERROR_CODE)
                                    {
                                        //Not in people list (Not a friend)
                                        isAQueryData.eIsaType = USERLISTAPI_IS_NOT_OF_TYPE;
                                        NetPrintf(("userlistapixboxone: [%p] UserListApiUpdate() User: %llu is not a friend or might not exist. Code: %i.\n", pRef, pCurrent->pTargetUserId, iCode));
                                    }
                                    else if (iCode == USERLISTAPI_MYSELF_ERROR_CODE)
                                    {
                                        //Target user is the current user
                                        isAQueryData.eIsaType = USERLISTAPI_IS_MYSELF;
                                        NetPrintf(("userlistapixboxone: [%p] UserListApiUpdate() User: %llu is Myself. Code: %i\n", pRef, pCurrent->pTargetUserId, iCode));
                                    }
                                    else
                                    {
                                        //Unknown Friend Status
                                        isAQueryData.eIsaType = USERLISTAPI_IS_TYPE_UNKNOWN;
                                        NetPrintf(("userlistapixboxone: [%p] UserListApiUpdate() Unknown IsFriend Response Error Code: %i for user %llu\n", pRef, iCode, pCurrent->pTargetUserId));
                                    }
                                }
                                else if (iStatusCode >= 200 && iStatusCode < 300)
                                {
                                    //This person is in the current user's people list
                                    isAQueryData.eIsaType = USERLISTAPI_IS_OF_TYPE;
                                    NetPrintf(("userlistapixboxone: [%p] UserListApiUpdate() User: %llu is a friend.\n", pRef, pCurrent->pTargetUserId));

                                    // check friendship details
                                    // check isFavorite
                                    pJson = JsonFind(aJsonList, "isFavorite");
                                    if (pJson != NULL)
                                    {
                                        int32_t iRet = JsonGetBoolean(pJson, FALSE);
                                        isAQueryData.bIsFavorite = (iRet == TRUE ? TRUE : FALSE);
                                    }

                                    // check mutual friendship
                                    pJson = JsonFind(aJsonList, "isFollowingCaller");
                                    if (pJson != NULL)
                                    {
                                        int32_t iRet = JsonGetBoolean(pJson, FALSE);
                                        isAQueryData.bIsMutualFriend = (iRet == TRUE ? TRUE : FALSE);
                                    }
                                }
                                else if (HTTP_RESPONSE_RATE_LIMITING == iStatusCode)
                                {
                                    NetPrintf(("userlistapixboxone: [%p] received an HTTP_RESPONSE_RATE_LIMITING\n", pRef));

                                    char strHdr[USERLISTAPI_HTTP_HEADER_BUFFER_SIZE] = "";
                                    ProtoHttpStatus(pRef->pHttp, 'htxt', strHdr, USERLISTAPI_HTTP_HEADER_BUFFER_SIZE);

                                    int32_t iRateLimRetrySecs = _GetRetryAfterFieldFromHTTPResponseHeader(strHdr);
                                    if (iRateLimRetrySecs > 0)
                                    {
                                        isAQueryData.eIsaType = USERLISTAPI_IS_TYPE_UNKNOWN;
                                        isAQueryData.Error = USERLISTAPI_ERROR_RATE_LIMITED;
                                        isAQueryData.iRateLimRetrySecs = iRateLimRetrySecs;
                                    }
                                    else
                                    {
                                        //Unknown Friend Status
                                        isAQueryData.eIsaType = USERLISTAPI_IS_TYPE_UNKNOWN;
                                        isAQueryData.Error = iStatusCode;
                                        NetPrintf(("userlistapixboxone: [%p] UserListApiUpdate() Failed to parse HTTP_RESPONSE_RATE_LIMITING Header!\n", pRef));
                                    }
                                }
                                else
                                {
                                    //Unknown Friend Status
                                    isAQueryData.eIsaType = USERLISTAPI_IS_TYPE_UNKNOWN;
                                    isAQueryData.Error = iStatusCode;
                                    NetPrintf(("userlistapixboxone: [%p] UserListApiUpdate() Failed to parse Is Friend Response. Unhandle Response Status Code!\n", pRef));
                                }

                                if (pCurrent->pIsAQueryNotifyCb != NULL)
                                {
                                    pCurrent->pIsAQueryNotifyCb(pRef, USERLISTAPI_IS_FRIENDS, &isAQueryData, pCurrent->pUserData);
                                }
                            }
                            else if (iHttpStatus <= PROTOHTTP_TIMEOUT)
                            {
                                pCurrent->bResponseProcessed = true;
                                isAQueryData.Error = iHttpStatus;
                                if (pCurrent->pIsAQueryNotifyCb != NULL)
                                {
                                    pCurrent->pIsAQueryNotifyCb(pRef, USERLISTAPI_IS_FRIENDS, &isAQueryData, pCurrent->pUserData);
                                }
                            }
                        }
                        else if (iStatusCode < 0)
                        {
                            NetPrintf(("userlistapixboxone: [%p] UserListApiUpdate() to complete IsFriend request.\n", pRef));
                            ds_memcpy_s(&isAQueryData.DirtyUser, sizeof(DirtyUserT), &pCurrent->pTargetUserId, sizeof(DirtyUserT));
                            isAQueryData.Error = 0;
                            isAQueryData.uUserIndex = pCurrent->uUserIndex;
                            isAQueryData.Error = USERLISTAPI_ERROR_UNKNOWN;
                            isAQueryData.eIsaType = USERLISTAPI_IS_TYPE_UNKNOWN;
                            if (pCurrent->pIsAQueryNotifyCb != NULL)
                            {
                                pCurrent->pIsAQueryNotifyCb(pRef, USERLISTAPI_IS_FRIENDS, &isAQueryData, pCurrent->pUserData);
                            }

                            pCurrent->bResponseProcessed = true;
                        }
                    }
                }
            }
            break;
        }

        if (pCurrent->bResponseProcessed)
        {
            UserListApiResultT *pTmp = pCurrent->pNext;
            if (pPrevious)
            {
                pPrevious->pNext = pCurrent->pNext;
            }
            else
            {
                pRef->ResultList = pCurrent->pNext;
            }

            if (pCurrent->eRequestType == USERLISTAPI_REQUEST_TYPE_ISFRIENDS)
            {
                _UserListApiProcessIsFriend(pRef);
            }
            
            pCurrent->refResultFriend = nullptr;
            pCurrent->refResultPresence = nullptr;
            pCurrent->refResultProfile = nullptr;
            DirtyMemFree(pCurrent, USERLISTAPI_MEMID, pRef->iMemGroup, pRef->pMemGroupUserData);

            pCurrent = pTmp;
        }
        else
        {
            pPrevious = pCurrent;
            pCurrent = pCurrent->pNext;
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
        int32_t     - zero=success; negative=error

    \Version 04/17/2013 (amakoukji)
*/
/*************************************************************************************************F*/
int32_t UserListApiStatus(UserListApiRefT *pRef, int32_t iSelect, void *pBuf, int32_t iBufSize)
{
    NetPrintf(("userlistapixboxone: [%p] unhandled status selector '%C'\n", pRef, iSelect));
    return(-1);
}

/*F*************************************************************************************************/
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

    \Version 04/17/2013 (amakoukji)
*/
/*************************************************************************************************F*/
int32_t UserListApiControl(UserListApiRefT *pRef, int32_t iControl, int32_t iValue, int32_t iValue2, void *pValue)
{
    NetPrintf(("userlistapixboxone: [%p] unhandled control selector '%C'\n", pRef, iControl));
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
    \Input *filter      - xboxone specific filter set
    \Input *pNotifyCb   - callback function pointer
    \Input uTypeMask    - mask
    \Input *pUserData   - user data

    \Output
        int32_t         - zero=success; negative=error

    \Version 04/17/2013 (amakoukji)
*/
/*************************************************************************************************F*/
int32_t UserListApiGetListAsync(UserListApiRefT *pRef, uint32_t uUserIndex, UserListApiTypeE eType, int32_t iLimit, int32_t iOffset, UserListApiFiltersT *filter, UserListApiCallbackT *pNotifyCb, uint32_t uTypeMask, void *pUserData)
{
    User^ refUser;
    int32_t iGuestRet = 0;
    
    if (uUserIndex >= NETCONN_MAXLOCALUSERS)
    {
        NetPrintf(("userlistapixboxone: UserListApiIsFriendAsync()  called with invalid user index %d.\n", uUserIndex));
        return(USERLISTAPI_ERROR_USER_INDEX_RANGE);
    }
    
    // check if we have a guest user
    iGuestRet = NetConnStatus('gust', uUserIndex, NULL, 0);
    
    if (iGuestRet == 1)
    {
        NetPrintf(("userlistapixboxone: UserListApiIsFriendAsync() cannot perform this operation with a guest.\n"));
        return(USERLISTAPI_ERROR_GUEST);
    }
    else if (iGuestRet < 0)
    {
        NetPrintf(("userlistapixboxone: UserListApiIsFriendAsync() failed to determine if the user is a non-psn guest or not. Ignoring request.\n"));
        return(USERLISTAPI_ERROR_INVALID_USER);
    }

    if (NetConnQuery(NULL, NULL, 0) < 0)
    {
        NetPrintf(("userlistapixboxone: [%p] At least 1 user must be signed in for UserListApiGeListAsync() call.\n", pRef));
        return(USERLISTAPI_ERROR_NO_USER);
    }
    else if (NetConnStatus('xusr', (int32_t)uUserIndex, &refUser, sizeof(refUser)) < 0)
    {
        NetPrintf(("userlistapixboxone: [%p] No user at index %d.\n", pRef, uUserIndex));
        return(USERLISTAPI_ERROR_NO_USER);
    }
    else if (pNotifyCb == NULL)
    {
        NetPrintf(("userlistapixboxone: [%p] UserListApiGeListAsync() called without a valid callback.\n", pRef));
        return(USERLISTAPI_ERROR_BAD_PARAM);
    }
    else if (_UserListApiIsAsyncOpInProgress(pRef, uUserIndex))
    {
        // $todo: queue up instead, use _UserListApiIsAsyncOpInProgress() to unlock next item in list
        NetPrintf(("userlistapixboxone: [%p] request for user %d already in progress.\n", pRef, uUserIndex));
        return(USERLISTAPI_ERROR_INPROGRESS);
    }
    else
    {
        uint32_t bIsSignedIn, bIsGuest, uUserId;

        switch (eType)
        {
            case USERLISTAPI_TYPE_FRIENDS:
                {
                    // Currrently only friends is supported
                }
                break;

            // It is currently unknown how these categories will be handled
            case USERLISTAPI_TYPE_BLOCKED:
            case USERLISTAPI_TYPE_MUTED:
            case USERLISTAPI_TYPE_FRIENDS_OF_FRIEND:
            default:
                return(USERLISTAPI_ERROR_UNSUPPORTED);
        }

        try
        {
            bIsSignedIn = refUser->IsSignedIn;
            bIsGuest = refUser->IsGuest;
            uUserId = refUser->Id;
        }
        catch (Exception ^ e)
        {
            NetPrintf(("userapixboxone: exception thrown while reading user attributes in UserListApiGetListAsync() (%S/0x%08x)\n", e->Message->Data(), e->HResult));
            return(USERLISTAPI_ERROR_NO_USER);
        }

        pRef->refUsers[uUserIndex] = refUser;

        if (bIsSignedIn && !bIsGuest)
        {
            uint32_t uCurrentId = 0;
            if (pRef->aXblContext[uUserIndex].refXblUserContext != nullptr)
            {
                uCurrentId = pRef->aXblContext[uUserIndex].refXblUserContext->User->Id;
            }

            if ((pRef->aXblContext[uUserIndex].refXblUserContext == nullptr) || (uUserId != uCurrentId))
            {
                try
                {
                    pRef->aXblContext[uUserIndex].refXblUserContext = ref new XboxLiveContext(pRef->refUsers[uUserIndex]);

                    pRef->aXblContext[uUserIndex].refXblUserContext->Settings->EnableServiceCallRoutedEvents = true;
                    pRef->aXblContext[uUserIndex].refXblUserContext->Settings->ServiceCallRouted += ref new EventHandler<XboxServiceCallRoutedEventArgs^>(
                        [pRef, uUserIndex](Platform::Object^ sender, XboxServiceCallRoutedEventArgs^ refArgs)
                        {
                            if (HTTP_RESPONSE_RATE_LIMITING == refArgs->HttpStatus)
                            {
                                NetPrintf(("userlistapixboxone: [%p] received an HTTP_RESPONSE_RATE_LIMITING\n", pRef));

                                int32_t iRateLimRetrySecs = _GetRetryAfterFieldFromHTTPResponseHeaderW(refArgs->ResponseHeaders->Data());
                                if (iRateLimRetrySecs > 0)
                                {
                                    pRef->aXblContext[uUserIndex].iRateLimRetrySecs = DS_MAX(pRef->aXblContext[uUserIndex].iRateLimRetrySecs, iRateLimRetrySecs);
                                }
                                else
                                {
                                    NetPrintf(("userlistapixboxone: [%p] UserListApiUpdate() Failed to parse HTTP_RESPONSE_RATE_LIMITING Header!\n", pRef));
                                }
                            }
                        });
                }
                catch (Exception ^ e)
                {
                    NetPrintf(("userlistapixboxone: [%p] unable to instantiate XboxLiveContext (%S/0x%08x)\n", pRef,
                                e->Message->Data(), e->HResult));
                    NetPrintf(("userlistapixboxone: [%p] WARNING!!! Microsoft.Xbox.Services.dll is likely not loaded. Please add a reference to 'XBox Services API' in your top-level project file to make this feature useable.\n", pRef));
                    return(USERLISTAPI_ERROR_UNKNOWN);
                }
            }
        }
        else if (refUser->IsGuest)
        {
            NetPrintf(("userlistapixboxxone: [%p] Cannot perform get list operation on a guest.\n"));
            return(USERLISTAPI_ERROR_GUEST);
        }
        else
        {
            NetPrintf(("userlistapixboxone: [%p] UserListApiGeListAsync() must be called with signed-in user. Current index %d.\n", pRef, uUserIndex));
            return(USERLISTAPI_ERROR_NO_USER);
        }

        // fetch appropriate user context
        SocialRelationship socialView;

        if (filter)
        {
            switch(filter->eLevel)
            {
                case USERLISTAPI_LEVEL_FAVORITE:
                    socialView = SocialRelationship::Favorite;
                break;
                case USERLISTAPI_LEVEL_FRIEND:
                default:
                    socialView = SocialRelationship::All;
                break;
            }
        }
        else
        {
            socialView = SocialRelationship::All;
        }

        UserListApiResultT *pResults = (UserListApiResultT *)DirtyMemAlloc(sizeof(* pResults), USERLISTAPI_MEMID, pRef->iMemGroup , pRef->pMemGroupUserData);
        ds_memclr(pResults, sizeof(*pResults));
        pResults->bNeedsMemFreed = true;

        // Put the result in the queue at the back and initialize it
        NetCritEnter(&pRef->crit);
        UserListApiResultT *pTmp = pRef->ResultList;
        if (pTmp == NULL)
        {
            pRef->ResultList = pResults;
        }
        else
        {
            while (pTmp->pNext != NULL)
            {
                pTmp = pTmp->pNext;
            }
            pTmp->pNext = pResults;
        }
        pResults->pNext = NULL;
        NetCritLeave(&pRef->crit);

        pResults->eRequestType = USERLISTAPI_REQUEST_TYPE_FRIENDS;
        pResults->pNotifyCb = pNotifyCb;
        pResults->pIsAQueryNotifyCb = NULL;
        pResults->pUserData = pUserData;
        pResults->uUserIndex = uUserIndex;

        pResults->bResponseProcessed = false;
        pResults->iErrorFriend = 0;
        pResults->iErrorProfile = 0;
        pResults->iErrorPresence = 0;
        pResults->uUserIndex = uUserIndex;
        pResults->SocialView = socialView;
        pResults->pUserData  = pUserData;
        pResults->iLimit     = iLimit;
        pResults->iOffset    = iOffset;
        pResults->uTypeMask  = uTypeMask;
        pResults->pNotifyCb  = pNotifyCb;

        pRef->aXblContext[uUserIndex].iRateLimRetrySecs = 0;

        // Send the 1st party data requests
        if (uTypeMask & USERLISTAPI_MASK_PROFILE)
        {
            pResults->SubQueryStatuses.bWaitingForProfileList = true;

            pRef->aXblContext[uUserIndex].uOpProfileTimer = NetTick();
            pRef->aXblContext[uUserIndex].refAsyncOpProfile = pRef->aXblContext[uUserIndex].refXblUserContext->ProfileService->
                                                           GetUserProfilesForSocialGroupAsync(socialView == SocialRelationship::Favorite ?
                                                                                              Microsoft::Xbox::Services::Social::SocialGroupConstants::Favorite :
                                                                                              Microsoft::Xbox::Services::Social::SocialGroupConstants::People);
            pRef->aXblContext[uUserIndex].refAsyncOpProfile->Completed = ref new AsyncOperationCompletedHandler<IVectorView<XboxUserProfile^>^>
            ( [pRef, pResults, uUserIndex] (IAsyncOperation<IVectorView<XboxUserProfile^>^> ^resultTask, Windows::Foundation::AsyncStatus status)
            {
                NetPrintf(("userlistapixboxone: [%p] GetUserProfilesForSocialGroupAsync() completed\n", pRef));

                // Find pResults in the list of results. If it is not in the list it means the operation timed out on our side and we should not process the response
                UserListApiResultT *pTmp = pRef->ResultList;
                bool bFound = false;
                while (pTmp != NULL)
                {
                    if (pTmp == pResults)
                    {
                        bFound = true;
                        break;
                    }
                    pTmp = pTmp->pNext;
                }

                if (pRef->bShuttingDown != TRUE && bFound)
                {                       
                    switch (status)
                    {
                        case Windows::Foundation::AsyncStatus::Completed:
                        {
                            IVectorView<XboxUserProfile^>^ refResult = nullptr;
                            try
                            {
                                refResult = resultTask->GetResults();
                            }
                            catch (Platform::Exception^ ex)
                            {
                                // no callback occurs in the case of an exception
                                // this is because the pRef could no longer exist
                                NetPrintf(("userlistapixboxone: [%p] error fetching people list (profiles), error code 0x%0.8x\n", pRef, ex->HResult));
                                refResult = nullptr;
                            }

                            if (refResult)
                            {
                                pResults->refResultProfile = refResult;
                            }
                            else
                            {
                                pResults->iErrorProfile = USERLISTAPI_ERROR_UNKNOWN;
                                pResults->refResultProfile = nullptr;
                            }

                            break;
                        }
                        case Windows::Foundation::AsyncStatus::Error:
                        {
                            NetPrintf(("userlistapixboxone: An error occured in call to UserListApiGetListAsync() (profile), ErrorCode=%S (0x%08x)\n", resultTask->ErrorCode.ToString()->Data(), resultTask->ErrorCode));

                            pResults->iErrorProfile = resultTask->ErrorCode.Value;
                            pResults->refResultProfile = nullptr;

                            break;
                        }
                        case Windows::Foundation::AsyncStatus::Canceled:
                        {
                            NetPrintf(("userlistapixboxone: [%p] UserListApiGetListAsync(), async operation (profile) was cancelled\n", pRef));
                            break;
                        }
                    }
                }

                resultTask->Close();
                pResults->SubQueryStatuses.bWaitingForProfileList = false;

                pRef->aXblContext[uUserIndex].refAsyncOpProfile->Close();
                delete pRef->aXblContext[uUserIndex].refAsyncOpProfile;
                pRef->aXblContext[uUserIndex].refAsyncOpProfile = nullptr;
            });
        }

        if (uTypeMask & USERLISTAPI_MASK_PRESENCE || uTypeMask & USERLISTAPI_MASK_RICH_PRESENCE)
        {
            pResults->SubQueryStatuses.bWaitingForPresenceList = true;

            pRef->aXblContext[uUserIndex].uOpPresenceTimer = NetTick();
            pRef->aXblContext[uUserIndex].refAsyncOpPresence = pRef->aXblContext[uUserIndex].refAsyncOpPresence = pRef->aXblContext[uUserIndex].refXblUserContext->PresenceService->
                                                            GetPresenceForSocialGroupAsync(socialView == SocialRelationship::Favorite ?
                                                                                           Microsoft::Xbox::Services::Social::SocialGroupConstants::Favorite :
                                                                                           Microsoft::Xbox::Services::Social::SocialGroupConstants::People);
            pRef->aXblContext[uUserIndex].refAsyncOpPresence->Completed = ref new AsyncOperationCompletedHandler<IVectorView<PresenceRecord^>^>
            ( [pRef, pResults, uUserIndex] (IAsyncOperation<IVectorView<PresenceRecord^>^> ^resultTask, Windows::Foundation::AsyncStatus status)
            {
                NetPrintf(("userlistapixboxone: [%p] GetPresenceForSocialGroupAsync() completed\n", pRef));

                // Find pResults in the list of results. If it is not in the list it means the operation timed out on our side and we should not process the response
                UserListApiResultT *pTmp = pRef->ResultList;
                bool bFound = false;
                while (pTmp != NULL)
                {
                    if (pTmp == pResults)
                    {
                        bFound = true;
                        break;
                    }
                    pTmp = pTmp->pNext;
                }

                if (pRef->bShuttingDown != TRUE && bFound)
                {
                    switch (status)
                    {
                        case Windows::Foundation::AsyncStatus::Completed:
                        {
                            IVectorView<PresenceRecord^>^ refResult = nullptr;
                            try
                            {
                                refResult = resultTask->GetResults();
                            }
                            catch (Platform::Exception^ ex)
                            {
                                // no callback occurs in the case of an exception
                                // this is because the pRef could no longer exist
                                NetPrintf(("userlistapixboxone: [%p] error fetching people list (presence), error code 0x%0.8x\n", pRef, ex->HResult));
                                refResult = nullptr;
                            }

                            if (refResult)
                            {
                                pResults->refResultPresence = refResult;
                            }
                            else
                            {
                                pResults->iErrorPresence = USERLISTAPI_ERROR_UNKNOWN;
                                pResults->refResultPresence = nullptr;
                            }

                            break;
                        }
                        case Windows::Foundation::AsyncStatus::Error:
                        {
                            NetPrintf(("userlistapixboxone: [%p] An error occured in call to UserListApiGetListAsync() (presence), ErrorCode=%S (0x%08x)\n", pRef, resultTask->ErrorCode.ToString()->Data(), resultTask->ErrorCode));

                            pResults->iErrorPresence = resultTask->ErrorCode.Value;
                            pResults->refResultPresence = nullptr;

                            break;
                        }
                        case Windows::Foundation::AsyncStatus::Canceled:
                        {
                            NetPrintf(("userlistapixboxone: [%p] UserListApiGetListAsync(), async operation (presence) was cancelled\n", pRef));
                            break;
                        }
                    }
                }

                resultTask->Close();
                pResults->SubQueryStatuses.bWaitingForPresenceList = false;

                pRef->aXblContext[uUserIndex].refAsyncOpPresence->Close();
                delete pRef->aXblContext[uUserIndex].refAsyncOpPresence;
                pRef->aXblContext[uUserIndex].refAsyncOpPresence = nullptr;
            });
        }

        pResults->SubQueryStatuses.bWaitingForFriendsList = true;
        pRef->aXblContext[uUserIndex].uOpFriendTimer = NetTick();
        pRef->aXblContext[uUserIndex].refAsyncOpFriend = pRef->aXblContext[uUserIndex].refXblUserContext->SocialService->GetSocialRelationshipsAsync(socialView, iOffset, iLimit);
        pRef->aXblContext[uUserIndex].refAsyncOpFriend->Completed = ref new AsyncOperationCompletedHandler<XboxSocialRelationshipResult^>
        ( [pRef, pResults, uUserIndex] (IAsyncOperation<XboxSocialRelationshipResult^> ^resultTask, Windows::Foundation::AsyncStatus status)
        {
            NetPrintf(("userlistapixboxone: [%p] GetSocialRelationshipsAsync() completed\n", pRef));

            // Find pResults in the list of results. If it is not in the list it means the operation timed out on our side and we should not process the response
            UserListApiResultT *pTmp = pRef->ResultList;
            bool bFound = false;
            while (pTmp != NULL)
            {
                if (pTmp == pResults)
                {
                    bFound = true;
                    break;
                }
                pTmp = pTmp->pNext;
            }

            if (pRef->bShuttingDown != TRUE && bFound)
            {
                switch (status)
                {
                    case Windows::Foundation::AsyncStatus::Completed:
                    {
                        XboxSocialRelationshipResult^ refResult = nullptr;
                        try
                        {
                            refResult = resultTask->GetResults();
                        }
                        catch (Platform::Exception^ ex)
                        {
                            // no callback occurs in the case of an exception
                            // this is because the pRef could no longer exist
                            NetPrintf(("userlistapixboxone: [%p] error fetching people list, error code 0x%0.8x\n", pRef, ex->HResult));
                            refResult = nullptr;
                        }

                        if (refResult)
                        {
                            pResults->refResultFriend = refResult;
                        }
                        else
                        {
                            pResults->iErrorFriend = USERLISTAPI_ERROR_UNKNOWN;
                            pResults->refResultFriend = nullptr;
                        }

                        break;
                    }
                    case Windows::Foundation::AsyncStatus::Error:
                    {
                        NetPrintf(("userlistapixboxone: [%p] An error occured in call to UserListApiGetListAsync() (friend), ErrorCode=%S (0x%08x)\n", pRef, resultTask->ErrorCode.ToString()->Data(), resultTask->ErrorCode));

                        pResults->iErrorFriend = resultTask->ErrorCode.Value;
                        pResults->refResultFriend = nullptr;

                        break;
                    }
                    case Windows::Foundation::AsyncStatus::Canceled:
                    {
                        NetPrintf(("userlistapixboxone: [%p] UserListApiGetListAsync(), async operation (friend) was cancelled\n", pRef));
                        break;
                    }
                }
            }
            resultTask->Close();
            pResults->SubQueryStatuses.bWaitingForFriendsList = false;

            pRef->aXblContext[uUserIndex].refAsyncOpFriend->Close();
            delete pRef->aXblContext[uUserIndex].refAsyncOpFriend;
            pRef->aXblContext[uUserIndex].refAsyncOpFriend = nullptr;
        });
    }

    return(0);
}

/*F*************************************************************************************************/
/*!
    \Function   UserListApiCancelAll

    \Description
        Cancels all active requests.

    \Input *pRef       - module ref

    \Version 04/17/2013 (amakoukji)
*/
/*************************************************************************************************F*/
void UserListApiCancelAll(UserListApiRefT *pRef)
{
    // $todo - mclouatre - Aug. 12 2014
    // how are pending async op canceled or queued for completion here?    seems to be missing


    // free all the mem from unprocessed results
    NetCritEnter(&pRef->crit);
    while (pRef->ResultList)
    {
        UserListApiResultT *pNext = pRef->ResultList->pNext;
        pRef->ResultList->refResultFriend   = nullptr;
        pRef->ResultList->refResultProfile  = nullptr;
        pRef->ResultList->refResultPresence = nullptr;

        if (pRef->ResultList->bNeedsMemFreed)
        {
            DirtyMemFree(pRef->ResultList, USERLISTAPI_MEMID, pRef->iMemGroup, pRef->pMemGroupUserData);
        }
        else
        {
            pRef->ResultList->bResponseProcessed = true;
        }
        
        pRef->ResultList->SubQueryStatuses.bWaitingForFriendsList  = false;
        pRef->ResultList->SubQueryStatuses.bWaitingForProfileList  = false;
        pRef->ResultList->SubQueryStatuses.bWaitingForPresenceList = false;

        pRef->ResultList = pNext;
    }

    // free any queued isFriend requests
    UserListApiIsFriendQueueT *pTmpQueuePtr = pRef->pIsFriendRequestQueue;
    while (pTmpQueuePtr != NULL)
    {
        UserListApiIsFriendQueueT *pNext = pTmpQueuePtr->pNext;
        DirtyMemFree(pTmpQueuePtr, USERLISTAPI_MEMID, pRef->iMemGroup, pRef->pMemGroupUserData);
        pTmpQueuePtr = pNext;
    }
    NetCritLeave(&pRef->crit);
}
