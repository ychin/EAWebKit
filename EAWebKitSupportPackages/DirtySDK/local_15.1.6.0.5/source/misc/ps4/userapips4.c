/*H*************************************************************************************************/
/*!

    \File    userapips4.c

    \Description
        Implements the ps4 specific code to request UserProfiles, and parse the responses.

    \Notes
         None.

    \Copyright
        Copyright (c) Tiburon Entertainment / Electronic Arts 2001-2013.    ALL RIGHTS RESERVED.

    \Version 04/11/13 (akirchner) First Version

*/
/*************************************************************************************************H*/


/*** Include files ********************************************************************************/

#include <string.h>
#include <np.h>
#ifndef EA_PLATFORM_PS5
 #include <np/np_npid.h>
#endif
#include <scetypes.h>

#include "userapipriv.h"

#include "DirtySDK/dirtysock/ps4/dirtywebapips4.h"
#include "DirtySDK/dirtylang.h"
#include "DirtySDK/dirtysock/dirtymem.h"
#include "DirtySDK/util/jsonparse.h"


/*** Defines **************************************************************************************/

#define USERAPI_MAX_LOOKUPS_PER_REQUEST         (50)

#define USERAPI_WEBAPI_GROUP                    "sdk:userProfile"
#define USERAPI_WEBAPI_RECENTLY_MET_GROUP       "sdk:activityFeed"
#define USERAPI_PATH_PROFILE_FORMAT             "/v1/profiles?accountIds=%s&fields=@default,personalDetail&avatarSize=%c"
#define USERAPI_PATH_PRESENCE_FORMAT            "/v1/users/%llu/profile?fields=@default,personalDetail,presence"
#define USERAPI_PATH_RECENTLY_MET_FORMAT        "/v1/users/%llu/feed"
#define USERAPI_PATH_RECENTLY_MET_POST_FORMAT   "{\"storyType\":\"PLAYED_WITH\",\"targets\":[{\"type\":\"ONLINE_ID\", \"accountId\": \"%llu\"},{\"type\": \"PLAYED_DESCRIPTION\",\"meta\": \"%s\"},{\"type\": \"TITLE_ID\",\"meta\": \"%s\"}]}"
#define USERAPI_PATH_RICH_PRESENCE_FORMAT1      "/v1/users/%llu/presence/gameStatus"
#define USERAPI_PATH_RICH_PRESENCE_FORMAT2      "/v1/users/%llu/presence/gameData"
#define USERAPI_PATH_RICH_PRESENCE_POST_FORMAT1 "{\"gameStatus\": \"%s\"}"
#define USERAPI_PATH_RICH_PRESENCE_POST_FORMAT2 "{\"gameData\": \"%s\"}"

#define USERAPI_PATH_PROFILE_FORMAT_LEN_MAX     (128 + ((20) * USERAPI_MAX_LOOKUPS_PER_REQUEST))
#define USERAPI_PATH_RECENTLY_MET_LEN_MAX       (256)

#define USERAPI_HTTP_RESP_BODY_JSON_MAX_LEN     (1024)

/*** Macros ***************************************************************************************/

/*** Type Definitions *****************************************************************************/

struct UserApiPlatResponseT
{
    char    *pData[NETCONN_MAXLOCALUSERS];
    int32_t  iDataLen[NETCONN_MAXLOCALUSERS];
    int32_t  iStatusCode[NETCONN_MAXLOCALUSERS];
    void    *pUserData[NETCONN_MAXLOCALUSERS];
};

struct UserApiPlatformDataT
{
    uint8_t bRawResponse;

    char pAvatarSize;
    DirtyWebApiRefT *pWebApi;
    DirtyWebApiRefT *pWebApiPresence;
    DirtyWebApiRefT *pWebApiRMP;

    struct UserApiPlatResponseT ProfilePlatResponse;
    struct UserApiPlatResponseT PresencePlatResponse;
    struct UserApiPlatResponseT RmpPlatResponse;
};


/*** Function Prototypes **************************************************************************/

static void _UserApiWebRequestCallback(int32_t iSceError, int32_t uUserIndex, int32_t iStatusCode, const char *pRespBody, int32_t iRespBodyLength, void *pUserData);
static void _UserApiWebPresenceRequestCallback(int32_t iSceError, int32_t uUserIndex, int32_t iStatusCode, const char *pRespBody, int32_t iRespBodyLength, void *pUserData);
static void _UserApiWebRecentlyMetRequestCallback(int32_t iSceError, int32_t uUserIndex, int32_t iStatusCode, const char *pRespBody, int32_t iRespBodyLength, void *pUserData);

/*** Variables ************************************************************************************/

/*** Private Functions ****************************************************************************/

/*F*************************************************************************************************/
/*!
    \Function    _UserApiTriggerPostCallback

    \Description
        Called by update function in the platform specific UserApi modules when a profile
        or error is ready. 

    \Input *pRef                 - pointer to UserApiT module reference.
    \Input *uUserIndex           - The index of the user associated with this profile request.

    \Version 12/10/2013 (amakoukji) - First version
*/
/*************************************************************************************************F*/
void _UserApiTriggerPostCallback(UserApiRefT *pRef, uint32_t uUserIndex)
{
    UserApiPostResponseT PostResponseData;

    PostResponseData.pMessage = pRef->pPlatformData->RmpPlatResponse.pData[uUserIndex];
    PostResponseData.eError   = (pRef->pPlatformData->RmpPlatResponse.iStatusCode[uUserIndex] >= 200 && pRef->pPlatformData->RmpPlatResponse.iStatusCode[uUserIndex] < 300) ? USERAPI_ERROR_OK: USERAPI_ERROR_REQUEST_FAILED;
    PostResponseData.uUserIndex = uUserIndex;

    NetCritEnter(&pRef->postCrit);

    if (pRef->pPostCallback[uUserIndex] != NULL)
    {
        pRef->pPostCallback[uUserIndex](pRef, &PostResponseData, pRef->pUserDataPost[uUserIndex]);
    }

    NetCritLeave(&pRef->postCrit);
}

/*F*************************************************************************************************/
/*!
    \Function _UserApiWebRequestCallback

    \Description
        Initiates the presence request to PSN 
        
    \Input iSceError           - error code from PSN
    \Input uUserIndex          - The index of the user making the request
    \Input iStatusCode         - http error code
    \Input *pRespBody          - response
    \Input iRespBodyLength     - response length
    \Input *pUserData          - a pointer to the user specified data

    \Output int32_t            - 0 for success, negative for error
        
    \Version 12/10/2013 (amakoukji)
*/
/*************************************************************************************************F*/
static void _UserApiWebRequestCallback(int32_t iSceError, int32_t uUserIndex, int32_t iStatusCode, const char *pRespBody, int32_t iRespBodyLength, void *pUserData)
{
    UserApiRefT *pRef = (UserApiRefT *)pUserData;

    NetCritEnter(&pRef->crit);
    if (iStatusCode >= 200 && iStatusCode < 300)
    {
        // Copy the data for processing
        if (pRef->pPlatformData->ProfilePlatResponse.iDataLen[uUserIndex] <= iRespBodyLength)
        {
            if(pRef->pPlatformData->ProfilePlatResponse.pData[uUserIndex] != NULL)
            {
                DirtyMemFree(pRef->pPlatformData->ProfilePlatResponse.pData[uUserIndex], USERAPI_MEMID, pRef->iMemGroup, pRef->pMemGroupUserData); 
            }

            pRef->pPlatformData->ProfilePlatResponse.pData[uUserIndex] = (char*)DirtyMemAlloc(iRespBodyLength + 1 , USERAPI_MEMID, pRef->iMemGroup, pRef->pMemGroupUserData);
            ds_memcpy(pRef->pPlatformData->ProfilePlatResponse.pData[uUserIndex], pRespBody, iRespBodyLength);
            pRef->pPlatformData->ProfilePlatResponse.pData[uUserIndex][iRespBodyLength] = '\0';
        }
    }

    pRef->bAvailableDataIndex[uUserIndex] = TRUE;
    pRef->pPlatformData->ProfilePlatResponse.iStatusCode[uUserIndex] = iStatusCode;
    pRef->pPlatformData->ProfilePlatResponse.pUserData[uUserIndex] = pUserData;

    NetCritLeave(&pRef->crit);
}

/*F*************************************************************************************************/
/*!
    \Function _UserApiWebPresenceRequestCallback

    \Description
        Initiates the presence request to PSN 
        
    \Input iSceError           - error code from PSN
    \Input uUserIndex          - The index of the user making the request
    \Input iStatusCode         - http error code
    \Input *pRespBody          - response
    \Input iRespBodyLength     - response length
    \Input *pUserData          - a pointer to the user specified data

    \Output int32_t            - 0 for success, negative for error
        
    \Version 12/10/2013 (amakoukji)
*/
/*************************************************************************************************F*/
void _UserApiWebPresenceRequestCallback(int32_t iSceError, int32_t uUserIndex, int32_t iStatusCode, const char *pRespBody, int32_t iRespBodyLength, void *pUserData)
{
    UserApiRefT *pRef = (UserApiRefT *)pUserData;

    NetCritEnter(&pRef->crit);
    if (iStatusCode >= 200 && iStatusCode < 300)
    {
        // Copy the data for processing
        if (pRef->pPlatformData->PresencePlatResponse.iDataLen[uUserIndex] <= iRespBodyLength)
        {
            if(pRef->pPlatformData->PresencePlatResponse.pData[uUserIndex] != NULL)
            {
                DirtyMemFree(pRef->pPlatformData->PresencePlatResponse.pData[uUserIndex], USERAPI_MEMID, pRef->iMemGroup, pRef->pMemGroupUserData); 
            }

            pRef->pPlatformData->PresencePlatResponse.pData[uUserIndex] = (char*)DirtyMemAlloc(iRespBodyLength + 1 , USERAPI_MEMID, pRef->iMemGroup, pRef->pMemGroupUserData);
            ds_memcpy(pRef->pPlatformData->PresencePlatResponse.pData[uUserIndex], pRespBody, iRespBodyLength);
            pRef->pPlatformData->PresencePlatResponse.pData[uUserIndex][iRespBodyLength] = '\0';
        }
    }

    if (pRef->uUserDataMask[uUserIndex] & USERAPI_MASK_PRESENCE)
    {
        pRef->bAvailableDataIndexPresence[uUserIndex] = TRUE;
    }
    if (pRef->uUserDataMask[uUserIndex] & USERAPI_MASK_RICH_PRESENCE)
    {
        pRef->bAvailableDataIndexRichPresence[uUserIndex] = TRUE;
    }

    pRef->pPlatformData->PresencePlatResponse.iStatusCode[uUserIndex] = iStatusCode;
    pRef->pPlatformData->PresencePlatResponse.pUserData[uUserIndex] = pUserData;

    NetCritLeave(&pRef->crit);
}

/*F*************************************************************************************************/
/*!
    \Function _UserApiWebRecentlyMetRequestCallback

    \Description
        Initiates the recently met player request to PSN Services
        
    \Input iSceError           - Sony's error number
    \Input uUserIndex          - The index of the user making the request
    \Input iStatusCode         - http return code
    \Input *pRespBody          - response
    \Input iRespBodyLength     - response length
    \Input *pUserData          - user data for callback
            
    \Version 06/14/2013 (amakoukji)
*/
/*************************************************************************************************F*/
void _UserApiWebRecentlyMetRequestCallback(int32_t iSceError, int32_t uUserIndex, int32_t iStatusCode, const char *pRespBody, int32_t iRespBodyLength, void *pUserData)
{
    UserApiRefT *pRef = (UserApiRefT *)pUserData;

    NetCritEnter(&pRef->postCrit);
    // Copy the data for processing
    if (pRef->pPlatformData->RmpPlatResponse.iDataLen[uUserIndex] <= iRespBodyLength)
    {
        if(pRef->pPlatformData->RmpPlatResponse.pData[uUserIndex] != NULL)
        {
            DirtyMemFree(pRef->pPlatformData->RmpPlatResponse.pData[uUserIndex], USERAPI_MEMID, pRef->iMemGroup, pRef->pMemGroupUserData); 
        }

        pRef->pPlatformData->RmpPlatResponse.pData[uUserIndex] = (char*)DirtyMemAlloc(iRespBodyLength + 1 , USERAPI_MEMID, pRef->iMemGroup, pRef->pMemGroupUserData);
        ds_memcpy(pRef->pPlatformData->RmpPlatResponse.pData[uUserIndex], pRespBody, iRespBodyLength);
        pRef->pPlatformData->RmpPlatResponse.pData[uUserIndex][iRespBodyLength] = '\0';
    }

    pRef->bAvailableDataIndexRMP[uUserIndex] = TRUE;

    pRef->pPlatformData->RmpPlatResponse.iStatusCode[uUserIndex] = iStatusCode;
    pRef->pPlatformData->RmpPlatResponse.pUserData[uUserIndex] = pUserData;

    NetCritLeave(&pRef->postCrit);
}

/*F*************************************************************************************************/
/*!
    \Function _UserApiProcessProfileResponse

    \Description
        Initiates the recently met player request to PSN Services
        
    \Input *pRef           - module ref
    \Input  uUserIndex     - The index of the user making the request
    \Input  bBatch         - batch flag
    \Input *ProfileData    - response data
    \Input *pUserData      - user data for callback

    \Output
        int32_t                 - 0 for success, negative for error
            
    \Version 06/14/2013 (amakoukji)
*/
/*************************************************************************************************F*/
int32_t _UserApiProcessProfileResponse(UserApiRefT *pRef, int32_t uUserIndex, uint8_t bBatch, UserApiProfileT *ProfileData, UserApiUserDataT *pUserData)
{
    ds_memclr(ProfileData, sizeof(UserApiProfileT));

    if (pRef->pPlatformData->ProfilePlatResponse.iStatusCode[uUserIndex] == 200)
    {
        uint16_t uJasonList[USERAPI_HTTP_RESP_BODY_JSON_MAX_LEN * 2];
        int16_t uCountry   = 0;
        int16_t uLanguage  = 0;
        const char *strPosGamerTag;
        const char *strPosFirstName;
        const char *strPosLastName;
        const char *strPosAvatarUrl;
        const char *strPosRegion;
        const char *strPosLanguages;
        const char *pTmp;
        int64_t size = 0;
        int32_t iSmallSize = 0;

        const char *pFriendStart = NULL;
        const char *pFriendEnd = NULL;
        
        ProfileData->strAvatarUrl[0] = '\0';
        ProfileData->strDisplayName[0] = '\0';
        ProfileData->strGamertag[0] = '\0';
        ProfileData->uLocale = 0;

        if (pRef->pPlatformData->bRawResponse == TRUE)
        {
            ProfileData->pRawData = pRef->pPlatformData->ProfilePlatResponse.pData[uUserIndex];
        }
        else
        {
            ProfileData->pRawData = NULL;
        }

        // Extract the size of the list
        pTmp = strstr(pRef->pPlatformData->ProfilePlatResponse.pData[uUserIndex], "\"size\"");
        if (pTmp)
        {
           pTmp = JsonSeekValue(pTmp);

           if (pTmp)
           {
                if (*pTmp == '"')
                {
                    ++pTmp;
                }
                size = JsonGetInteger(pTmp, 0);
           }
           else
           {
               NetPrintf(("userapips4: [%p] failed to parse http response, 'size' attribute improperly formatted\n", pRef));
               pRef->UserContextList[uUserIndex].iTotalErrors += pRef->iLookupsSent[uUserIndex];
               return(USERAPI_ERROR_REQUEST_PARSE_FAILED);
           }
        }
        else
        {
            NetPrintf(("userapips4: [%p] failed to parse http response, no 'size' attribute\n", pRef));
            pRef->UserContextList[uUserIndex].iTotalErrors += pRef->iLookupsSent[uUserIndex];
            return(USERAPI_ERROR_REQUEST_PARSE_FAILED);
        }

        // because it is possible for less items to be returned than the number queried (as is the case with duplicates)
        // we must adjust the number of total errors
        iSmallSize = (int32_t)size;
        if (pRef->iLookupsSent[uUserIndex] - iSmallSize > 0)
        {
            NetPrintf(("userapips4: [%p] warning, less %d profiles queried but only %d received\n", pRef, pRef->iLookupsSent[uUserIndex], iSmallSize));
            pRef->UserContextList[uUserIndex].iTotalErrors += pRef->iLookupsSent[uUserIndex] - iSmallSize;
        }

        pTmp = strstr(pRef->pPlatformData->ProfilePlatResponse.pData[uUserIndex], "\"profiles\"");
        if (pTmp)
        {
            pTmp = JsonSeekValue(pTmp);
            if (pTmp)
            {
                pFriendStart = pTmp + 1; // profiles is a list, skip the '['
            } 
            else
            {
                NetPrintf(("userapips4: [%p] failed to parse http response, no 'profiles' attribute\n", pRef));
                pRef->UserContextList[uUserIndex].iTotalErrors += iSmallSize;
                return(USERAPI_ERROR_REQUEST_PARSE_FAILED);
            }
        }
        else
        {
            NetPrintf(("userapips4: [%p] failed to parse http response, no 'profiles' attribute\n", pRef));
            pRef->UserContextList[uUserIndex].iTotalErrors += iSmallSize;
            return(USERAPI_ERROR_REQUEST_PARSE_FAILED);
        }

        for (int64_t i = 0; i < size; ++i)
        {
            // start by fetching only a single friend and parsing it
            pFriendEnd = JsonGetListItemEnd(pFriendStart);
            if (pFriendEnd == NULL)
            {
                NetPrintf(("userapips4: [%p] failed to parse http response, 'profiles' attribute improperly formatted\n", pRef));
                pRef->UserContextList[uUserIndex].iTotalErrors += iSmallSize;
                return(USERAPI_ERROR_REQUEST_PARSE_FAILED);
            }

            // parse JSON response, 1 user at a time
            if (JsonParse(uJasonList, USERAPI_HTTP_RESP_BODY_JSON_MAX_LEN * 2, pFriendStart, pFriendEnd - pFriendStart) < 0)
            {
                NetPrintf(("userapips4: [%p] failed to parse http response\n", pRef));
                pRef->UserContextList[uUserIndex].iTotalErrors += iSmallSize;
                return(USERAPI_ERROR_REQUEST_PARSE_FAILED);
            }

            ProfileData->strGamertag[0] = '\0';
            ProfileData->strDisplayName[0] = '\0';
            ProfileData->strAvatarUrl[0] = '\0';
            ProfileData->uLocale = 0;

            // get GamerTag
            strPosGamerTag = JsonFind(uJasonList, "user.onlineId");
            if (strPosGamerTag)
            {
                JsonGetString(strPosGamerTag, ProfileData->strGamertag, sizeof(ProfileData->strGamertag), "");
            }

            // get account id
            strPosGamerTag = JsonFind(uJasonList, "user.accountId");
            if (strPosGamerTag)
            {
                SceNpAccountId NativeId = 0;
                char strNativeId[20];
                JsonGetString(strPosGamerTag, strNativeId, sizeof(strNativeId), "");
                NativeId = ds_strtoull(strNativeId, NULL, 10);

                DirtyUserFromNativeUser(&pUserData->DirtyUser, &NativeId);
            }

            // get DisplayName
            strPosFirstName = JsonFind(uJasonList, "personalDetail.firstName");
            strPosLastName = JsonFind(uJasonList, "personalDetail.lastName");
            if (strPosFirstName && strPosLastName)
            {
                int32_t len = JsonGetString(strPosFirstName, ProfileData->strDisplayName, sizeof(ProfileData->strDisplayName), "");
                len += ds_strnzcat(ProfileData->strDisplayName + len, " ", sizeof(ProfileData->strDisplayName) - len);
                JsonGetString(strPosLastName, ProfileData->strDisplayName + len, sizeof(ProfileData->strDisplayName) - len, strPosLastName);
            }
            else if (strPosFirstName)
            {
                JsonGetString(strPosFirstName, ProfileData->strDisplayName, sizeof(ProfileData->strDisplayName), "");
            }
            else if (strPosLastName)
            {
                JsonGetString(strPosLastName, ProfileData->strDisplayName, sizeof(ProfileData->strDisplayName), "");
            }


            if (ProfileData->strDisplayName[0] == '\0')
            {
                ds_strnzcpy(ProfileData->strDisplayName, ProfileData->strGamertag, sizeof(ProfileData->strDisplayName));
            }

            // get AvatarUrl
            strPosAvatarUrl = JsonFind(uJasonList, "avatarUrl");
            if (strPosAvatarUrl)
            {
                JsonGetString(strPosAvatarUrl, ProfileData->strAvatarUrl, sizeof(ProfileData->strAvatarUrl), "");
            }

            // get region
            strPosRegion = JsonFind(uJasonList, "region");
            if (strPosRegion)
            {
                char strRegion[8];
                JsonGetString(strPosRegion, strRegion, sizeof(strRegion), "");
                uCountry = (strRegion[0] << 8) + strRegion[1];
            }

            // get language
            // use the first in the list
            strPosLanguages = JsonFind(uJasonList, "languagesUsed");
            if (strPosLanguages)
            {
                const char *strLanguage;
                strLanguage = strstr(strPosLanguages, "\"") + 1;
                uLanguage = (strLanguage[0] << 8) + strLanguage[1];
            }

            // build local
            ProfileData->uLocale = LOBBYAPI_LocalizerTokenCreate(uLanguage, uCountry);

            // set the raw data
            // for our purposes this will be a pointer to the start of the friend object in the buffer
            if (pRef->pPlatformData->bRawResponse == TRUE)
            {
                ProfileData->pRawData = (char*)pFriendStart;
            }
            else
            {
                ProfileData->pRawData = NULL;
            }

            // If we're in batch mode send the callbacks right away, otherwise do nothing and just have the data stored in ProfileData
            if (bBatch)
            {
                _UserApiTriggerCallback(pRef, uUserIndex, USERAPI_ERROR_OK, USERAPI_EVENT_DATA, pUserData);
            }

            // prep the next friend parse
            pFriendStart = pFriendEnd + 1;
        }

        pRef->UserContextList[uUserIndex].iTotalReceived += iSmallSize;
    }
    else
    {
        pRef->UserContextList[uUserIndex].iTotalErrors += pRef->iLookupsSent[uUserIndex];
        return(USERAPI_ERROR_REQUEST_FAILED);
    }

    return(0);
}

/*F*************************************************************************************************/
/*!
    \Function _UserApiProcessPresenceResponse

    \Description
        Initiates the recently met player request to PSN Services
        
    \Input *pRef           - module ref
    \Input  uUserIndex     - The index of the user making the request
    \Input *pPresenceData  - response data
    \Input *pUserData      - user data for callback

    \Output
        int32_t                 - 0 for success, negative for error
            
    \Version 06/14/2013 (amakoukji)
*/
/*************************************************************************************************F*/
int32_t _UserApiProcessPresenceResponse(UserApiRefT *pRef, int32_t uUserIndex, UserApiPresenceT *pPresenceData, UserApiUserDataT *pUserData)
{
    ds_memclr(pPresenceData, sizeof(UserApiPresenceT));

    if (pRef->pPlatformData->PresencePlatResponse.iStatusCode[uUserIndex] == 200)
    {
        uint16_t uJasonList[USERAPI_HTTP_RESP_BODY_JSON_MAX_LEN * 2];
        char strCurrentTitle[SCE_NP_TITLE_ID_LEN + 1];
        const char *pTmp;

        // get the current title for the user
        NetConnStatus('titl', uUserIndex, strCurrentTitle, sizeof(strCurrentTitle));
                
        // parse JSON response, 1 user at a time
        if (JsonParse(uJasonList, USERAPI_HTTP_RESP_BODY_JSON_MAX_LEN * 2, pRef->pPlatformData->PresencePlatResponse.pData[uUserIndex], 
                      strlen(pRef->pPlatformData->PresencePlatResponse.pData[uUserIndex])) < 0)
        {
            NetPrintf(("userapips4: [%p] failed to parse http response\n", pRef));
            pRef->UserPresenceList[uUserIndex].iTotalErrors += 1;
            return(USERAPI_ERROR_REQUEST_PARSE_FAILED);
        }

        pPresenceData->ePresenceStatus = USERAPI_PRESENCE_UNKNOWN;
        pPresenceData->strPlatform[0] = '\0';
        pPresenceData->strTitleId[0] = '\0';
        pPresenceData->strTitleName[0] = '\0';

        if (pRef->pPlatformData->bRawResponse == TRUE)
        {
            pPresenceData->pRawData = pRef->pPlatformData->PresencePlatResponse.pData[uUserIndex];
        }
        else
        {
            pPresenceData->pRawData = NULL;
        }

        // get account id
        pTmp = JsonFind(uJasonList, "user.accountId");
        if (pTmp)
        {
            SceNpAccountId NativeId = 0;
            char strNativeId[20];
            JsonGetString(pTmp, strNativeId, sizeof(strNativeId), "");
            NativeId = ds_strtoull(strNativeId, NULL, 10);

            DirtyUserFromNativeUser(&pUserData->DirtyUser, &NativeId);
        }

        // get online status
        pTmp = JsonFind(uJasonList, "presence.primaryInfo.onlineStatus");
        if (pTmp)
        {
            char strOnlineStatus[32];
            
            JsonGetString(pTmp, strOnlineStatus, sizeof(strOnlineStatus), "");
           
            if (ds_stricmp(strOnlineStatus, "online") == 0)
            {
                pPresenceData->ePresenceStatus = USERAPI_PRESENCE_ONLINE;
            }
            else if (ds_stricmp(strOnlineStatus, "offline") == 0)
            {
                pPresenceData->ePresenceStatus = USERAPI_PRESENCE_OFFLINE;
            }
            else
            {
                pPresenceData->ePresenceStatus = USERAPI_PRESENCE_UNKNOWN;
            }
        }

        // get platform
        pTmp = JsonFind(uJasonList, "presence.primaryInfo.platform");
        if (pTmp)
        {
            JsonGetString(pTmp, pPresenceData->strPlatform, sizeof(pPresenceData->strPlatform), "");
        }

        // get title name
        pTmp = JsonFind(uJasonList, "presence.primaryInfo.gameTitleInfo.titleName");
        if (pTmp)
        {
            JsonGetString(pTmp, pPresenceData->strTitleName, sizeof(pPresenceData->strPlatform), "");
        }

        // get title id
        pTmp = JsonFind(uJasonList, "presence.primaryInfo.gameTitleInfo.npTitleId");
        if (pTmp)
        {
            JsonGetString(pTmp, pPresenceData->strTitleId, sizeof(pPresenceData->strPlatform), "");
        }

        // check if this is the same title id as the user is currently playing
        if (ds_stricmp(strCurrentTitle, pPresenceData->strTitleId) == 0)
        {
            pPresenceData->bIsPlayingSameTitle = TRUE;
        }
        else
        {
            pPresenceData->bIsPlayingSameTitle = FALSE;
        }

        pRef->UserPresenceList[uUserIndex].iTotalReceived += 1;
    }
    else
    {
        pRef->UserPresenceList[uUserIndex].iTotalErrors += 1;
        return(USERAPI_ERROR_REQUEST_FAILED);
    }

    return(0);
}

/*F*************************************************************************************************/
/*!
    \Function _UserApiProcessRichPresenceResponse

    \Description
        Initiates the recently met player request to PSN Services
        
    \Input *pRef              - module ref
    \Input  uUserIndex        - The index of the user making the request
    \Input *pRichPresenceData - response data
    \Input *pUserData         - user data for callback

    \Output
        int32_t                 - 0 for success, negative for error
            
    \Version 06/14/2013 (amakoukji)
*/
/*************************************************************************************************F*/
int32_t _UserApiProcessRichPresenceResponse(UserApiRefT *pRef, int32_t uUserIndex, UserApiRichPresenceT *pRichPresenceData, UserApiUserDataT *pUserData)
{
    // For PS4 rich presence data comes in the regular presence query
    ds_memclr(pRichPresenceData, sizeof(UserApiRichPresenceT));

    if (pRef->pPlatformData->PresencePlatResponse.iStatusCode[uUserIndex] == 200)
    {
        uint16_t uJasonList[USERAPI_HTTP_RESP_BODY_JSON_MAX_LEN * 2];
        const char *pTmp;
                
        // parse JSON response, 1 user at a time
        if (JsonParse(uJasonList, USERAPI_HTTP_RESP_BODY_JSON_MAX_LEN * 2, pRef->pPlatformData->PresencePlatResponse.pData[uUserIndex], 
                      strlen(pRef->pPlatformData->PresencePlatResponse.pData[uUserIndex])) < 0)
        {
            NetPrintf(("userapips4: [%p] failed to parse http response\n", pRef));
            pRef->UserRichPresenceList[uUserIndex].iTotalErrors += 1;
            return(USERAPI_ERROR_REQUEST_PARSE_FAILED);
        }

        pRichPresenceData->strData[0] = '\0';

        if (pRef->pPlatformData->bRawResponse == TRUE)
        {
            pRichPresenceData->pRawData = pRef->pPlatformData->PresencePlatResponse.pData[uUserIndex];
        }
        else
        {
            pRichPresenceData->pRawData = NULL;
        }

        // get NpId
        pTmp = JsonFind(uJasonList, "user.accountId");
        if (pTmp)
        {
            char strAccountId[20];
            SceNpAccountId accountId;

            JsonGetString(pTmp, strAccountId, sizeof(strAccountId), "");
            accountId = ds_strtoull(strAccountId, NULL, 10);

            DirtyUserFromNativeUser(&pUserData->DirtyUser, &accountId);
        }

        // get game status
        pTmp = JsonFind(uJasonList, "presence.gameStatus");
        if (pTmp)
        {
            JsonGetString(pTmp, pRichPresenceData->strData, sizeof(pRichPresenceData->strData), "");
        }

        // get game data
        pTmp = JsonFind(uJasonList, "presence.gameData");
        if (pTmp)
        {
            JsonGetString(pTmp, pRichPresenceData->strGameData, sizeof(pRichPresenceData->strGameData), "");
        }

        pRef->UserRichPresenceList[uUserIndex].iTotalReceived += 1;
    }
    else
    {
        pRef->UserRichPresenceList[uUserIndex].iTotalErrors += 1;
        return(USERAPI_ERROR_REQUEST_FAILED);
    }

    return(0);
}

/*F*************************************************************************************************/
/*!
    \Function _UserApiProcessRmpResponse

    \Description
        Initiates the recently met player request to PSN Services
        
    \Input *pRef              - module ref
    \Input  uUserIndex        - The index of the user making the request

    \Output
        int32_t                 - 0 for success, negative for error
            
    \Version 06/14/2013 (amakoukji)
*/
/*************************************************************************************************F*/
int32_t _UserApiProcessRmpResponse(UserApiRefT *pRef, uint32_t uUserIndex)
{
    return(0);
}

/*F*************************************************************************************************/
/*!
    \Function   _UserAliOnWebApiUpdate

    \Description
        Callback for when friends update events happen.

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

    \Version 08/02/2013 (efan)
*/
/*************************************************************************************************F*/
static void _UserAliOnWebApiUpdate(int32_t userCtxId, int32_t callbackId, const char *pNpServiceName, SceNpServiceLabel npServiceLabel, const SceNpPeerAddressA *pTo, const SceNpPeerAddressA *pFrom, const SceNpWebApiPushEventDataType *pDataType, const char *pData, size_t dataLen, void *pUserArg)
{
    UserApiNotificationT (*pList)[] = NULL;
    UserApiNotifyTypeE eType = USERAPI_NOTIFY_PRESENCE_UPDATE;
    int32_t iMemGroup;
    void *pMemGroupUserData;
    int32_t iLoop = 0;
    UserApiNotifyDataT *data = NULL;
    uint32_t uUserIndex = 0; // by default notification for user at index 0

    if (pUserArg != NULL)
    {
        UserApiRefT *pRef = (UserApiRefT*)pUserArg;
        NetPrintf(("userapips4: [%p] detected userlist updated event.\n", pRef));

        // create a DirtyUserT with the user name info we have
        DirtyUserT dirtyUser;
        SceNpAccountId FromAccountId = pFrom->accountId;

        // create a UserListApiNotifyDataT 
        DirtyMemGroupQuery(&iMemGroup, &pMemGroupUserData);
        data = (UserApiNotifyDataT*)DirtyMemAlloc(sizeof(UserApiNotifyDataT), USERAPI_MEMID, iMemGroup, pMemGroupUserData);
        ds_memclr(data, sizeof(UserApiNotifyDataT));

        DirtyUserFromNativeUser(&dirtyUser, &FromAccountId);

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

        if (ds_stricmp(pDataType->val, "np:service:presence:onlineStatus") == 0)
        {
            pList = &pRef->PresenceNotification;
            eType = USERAPI_NOTIFY_PRESENCE_UPDATE;
            ds_memcpy_s(&data->PresenceData.DirtyUser, sizeof(data->PresenceData.DirtyUser), &dirtyUser, sizeof(dirtyUser));
            data->PresenceData.uUserIndex = uUserIndex;
        }
        else if (ds_stricmp(pDataType->val, "np:service:presence:gameTitleInfo") == 0)
        {
            pList = &pRef->TitleNotification;
            eType = USERAPI_NOTIFY_TITLE_UPDATE;
            ds_memcpy_s(&data->TitleData.DirtyUser, sizeof(data->TitleData.DirtyUser), &dirtyUser, sizeof(dirtyUser));
            data->TitleData.uTitleId = 0; // title on PS4 is always the same as the active one
            data->TitleData.uUserIndex = uUserIndex;
        }
        else if (ds_stricmp(pDataType->val, "np:service:presence:gameStatus") == 0)
        {
            pList = &pRef->RichPresenceNotification;
            eType = USERAPI_NOTIFY_RICH_PRESENCE_UPDATE;
            ds_memcpy_s(&data->RichPresenceData.DirtyUser, sizeof(data->RichPresenceData.DirtyUser), &dirtyUser, sizeof(dirtyUser));
            data->RichPresenceData.uUserIndex = uUserIndex;
        }
        else
        {
            NetPrintf(("userapips4: received unknown push event: %s\n", pDataType->val));
            return;
        }

        NetCritEnter(&pRef->crit);
        // find an empty slot
        for (iLoop = 0; iLoop < USERAPI_MAX_QUEUED_NOTIFICATIONS; ++iLoop)
        {
            if (pRef->UserApiNotifyEvent[iLoop].pNotificationData == NULL)
            {
                pRef->UserApiNotifyEvent[iLoop].pNotificationData = data;
                pRef->UserApiNotifyEvent[iLoop].pNotificationList = pList;
                pRef->UserApiNotifyEvent[iLoop].pNotificationType = eType;
                pRef->UserApiNotifyEvent[iLoop].uUserIndex        = uUserIndex;
                break;
            }
        }
        if (iLoop == USERAPI_MAX_QUEUED_NOTIFICATIONS)
        {
            NetPrintf(("userapips4: maximum notifications in queue exceeded, notification dropped\n"));
        }
        NetCritLeave(&pRef->crit);
    }
    else
    {
        NetPrintf(("userapips4: null UserApiRefT from pUserArg.\n"));
    }
}

/*F********************************************************************************/
/*!
    \Function UserApiPlatUpdate

    \Description
        Control behavior of module.

    \Input *pRef    - pointer to module state

    \Output
        int32_t     - Return 0 if successful. Otherwise a selector specific error

    \Version 08/08/2013 (amakoukji) - First version
*/
/********************************************************************************F*/
int32_t UserApiPlatUpdate(UserApiRefT *pRef)
{
    if (pRef)
    {
        for (uint32_t i = 0; i < NETCONN_MAXLOCALUSERS; ++i)
        {
            // Process profile requests
            // These can be handled in batches of up to 50 at a time
            NetCritEnter(&pRef->crit);
            if (pRef->bLookupUserAvailable[i] && pRef->iLookupUserIndex[i] > -1 && pRef->iLookupUserIndex[i] < pRef->iLookupUsersLength[i])
            {
                SceNpAccountId NativeUser;
                char strPath[USERAPI_PATH_PROFILE_FORMAT_LEN_MAX];
                char strUserList[((20) * USERAPI_MAX_LOOKUPS_PER_REQUEST)];
                int32_t iLookups = 0;
                int32_t iWritten = 0;

                // Construct the list of users to lookup that is no more than USERAPI_MAX_LOOKUPS_PER_REQUEST in length
                while (pRef->iLookupUserIndex[i] < pRef->iLookupUsersLength[i] && iLookups < USERAPI_MAX_LOOKUPS_PER_REQUEST)
                {
                    if (!(DirtyUserToNativeUser(&NativeUser, sizeof(NativeUser), ((pRef->aLookupUsers[i]) + pRef->iLookupUserIndex[i]))))
                    {
                        NetPrintf(("userapips4: [%p] failed to get first party id\n", pRef));
                        ++pRef->UserContextList[i].iTotalErrors;
                    }
                    else
                    {
                        iWritten += ds_snzprintf(strUserList + iWritten, sizeof(strUserList) - iWritten, "%llu,", NativeUser);
                        ++iLookups;
                    }
                    
                    ++(pRef->iLookupUserIndex[i]);
                }

                // Remove the trailing comma
                strUserList[iWritten - 1] = '\0'; 

                // Remember how many users were requested this round as its possible to not get back the same number of responses
                pRef->iLookupsSent[i] = iLookups;

                // Perform the lookup
                ds_snzprintf(strPath, sizeof(strPath), USERAPI_PATH_PROFILE_FORMAT, strUserList, pRef->pPlatformData->pAvatarSize);

                if (!(DirtyWebApiRequest(pRef->pPlatformData->pWebApi, i, USERAPI_WEBAPI_GROUP, SCE_NP_WEBAPI_HTTP_METHOD_GET, strPath, NULL, 0, NULL, _UserApiWebRequestCallback, pRef) >= 0))
                {
                    pRef->UserContextList[i].iTotalErrors += iLookups;
                }
                else
                {
                    pRef->bLookupUserAvailable[i] = FALSE;
                }
            }
            NetCritLeave(&pRef->crit);

        }
    }
    return(0);
}

/*** Public Functions *****************************************************************************/

/*F*************************************************************************************************/
/*!
    \Function    UserApiPlatCreateData

    \Description
        Allocates and initializes resources needed for the UserApi on PS4.

    \Input *pRef                  - pointer to UserApiT module reference.

    \Output UserApiPlatformDataT* - pointer to reference

    \Version 04/17/2013 (mcorcoran) - First version
*/
/*************************************************************************************************F*/
UserApiPlatformDataT * UserApiPlatCreateData(UserApiRefT *pRef)
{
    UserApiPlatformDataT *pPlatformData;

    DirtyWebApiCreateParamsT createParams;

    // allocate and init module state
    if ((pPlatformData = (UserApiPlatformDataT*)DirtyMemAlloc(sizeof(*pPlatformData), USERAPI_MEMID, pRef->iMemGroup, pRef->pMemGroupUserData)) == NULL)
    {
        NetPrintf(("userapips4: [%p] failed to allocate platform module state.\n", pRef));
        return(NULL);
    }
    
    ds_memclr(pPlatformData, sizeof(UserApiPlatformDataT));
    ds_memclr(&createParams, sizeof(createParams));
    createParams.readBufferSize = 51200;
    pPlatformData->pAvatarSize = 's';
    pPlatformData->bRawResponse = TRUE;

    pPlatformData->pWebApi = DirtyWebApiCreate(&createParams);
    if (pPlatformData->pWebApi == NULL)
    {
        NetPrintf(("userapips4: [%p] failed to create DirtyWebApi.\n", pRef));
        UserApiPlatDestroyData(pRef, pPlatformData);
        return(NULL);
    }

    createParams.readBufferSize = 0; // use default
    pPlatformData->pWebApiPresence = DirtyWebApiCreate(&createParams);
    if (pPlatformData->pWebApiPresence == NULL)
    {
        NetPrintf(("userapips4: [%p] failed to create DirtyWebApi.\n", pRef));
        UserApiPlatDestroyData(pRef, pPlatformData);
        return(NULL);
    }

    createParams.readBufferSize = 0;  // use default
    pPlatformData->pWebApiRMP = DirtyWebApiCreate(&createParams);
    if (pPlatformData->pWebApiRMP == NULL)
    {
        NetPrintf(("userapips4: [%p] failed to create DirtyWebApi.\n", pRef));
        UserApiPlatDestroyData(pRef, pPlatformData);
        return(NULL);
    }

    // return the module ref
    return(pPlatformData);
}

/*F*************************************************************************************************/
/*!
    \Function    UserApiPlatDestroyData

    \Description
        Destroys the ps4 specific resources used by the UserApi module.

    \Input *pRef           - pointer to UserApiT module reference.
    \Input *pPlatformData  - pointer to the UserApiPlatformDataT struct to be destroyed.

    \Output
        int32_t  - Return 0 if successful. Otherwise -1.

    \Version 04/17/2013 (akirchner) - First version
*/
/*************************************************************************************************F*/
int32_t UserApiPlatDestroyData(UserApiRefT *pRef, UserApiPlatformDataT *pPlatformData)
{
    for (uint32_t iValue = 0; iValue < NETCONN_MAXLOCALUSERS; ++iValue)
    {
        UserApiCancel(pRef, iValue);

        if (pRef->aLookupUsers[iValue])
        {
            DirtyMemFree(pRef->aLookupUsers[iValue], USERAPI_MEMID, pRef->iMemGroup, pRef->pMemGroupUserData); 
            pRef->aLookupUsers[iValue] = NULL;
        }

        if (pPlatformData->ProfilePlatResponse.pData[iValue])
        {   
            DirtyMemFree(pPlatformData->ProfilePlatResponse.pData[iValue], USERAPI_MEMID, pRef->iMemGroup, pRef->pMemGroupUserData); 
        }

        if (pPlatformData->PresencePlatResponse.pData[iValue])
        {
            DirtyMemFree(pPlatformData->PresencePlatResponse.pData[iValue], USERAPI_MEMID, pRef->iMemGroup, pRef->pMemGroupUserData); 
        }

        if (pPlatformData->RmpPlatResponse.pData[iValue])
        {
            DirtyMemFree(pPlatformData->RmpPlatResponse.pData[iValue], USERAPI_MEMID, pRef->iMemGroup, pRef->pMemGroupUserData); 
        }
    }

    if (pPlatformData->pWebApi != NULL)
    {
        DirtyWebApiDestroy(pPlatformData->pWebApi);
    }

    if (pPlatformData->pWebApiPresence != NULL)
    {
        DirtyWebApiDestroy(pPlatformData->pWebApiPresence);
    }

    if (pPlatformData->pWebApiRMP != NULL)
    {
        DirtyWebApiDestroy(pPlatformData->pWebApiRMP);
    }

    DirtyMemFree(pPlatformData, USERAPI_MEMID, pRef->iMemGroup, pRef->pMemGroupUserData);

    return(0);
}

/*F*************************************************************************************************/
/*!
    \Function UserApiPlatRequestProfile

    \Description
        Initiates the profile request to PSN
        
    \Input *pRef               - module ref
    \Input uUserIndex          - The index of the user making the request
    \Input pLookupUsers        - Array of DirtyUserT struct containing the users to lookup
    \Input iLookupUsersLength  - maximum users to return per query

    \Output int32_t            - 0 for success, negative for error
        
    \Version 04/17/2013 (mcorcoran)
*/
/*************************************************************************************************F*/
int32_t UserApiPlatRequestProfile(UserApiRefT *pRef, uint32_t uUserIndex, DirtyUserT *pLookupUsers, int32_t iLookupUsersLength)
{
    // Copy the request for processing
    // We defer processing to UserApiUpdate() calls because processing then entire set of users could 
    // take more than 1 batch request to the servers. This way subsequent batch commands can be started
    // with the same logic without it being reproduced here at the cost of 1 frame of processing time.
    NetCritEnter(&pRef->crit);
    pRef->iLookupUserIndex[uUserIndex] = 0;
    pRef->iLookupUsersLength[uUserIndex] = iLookupUsersLength;
    pRef->bLookupUserAvailable[uUserIndex] = TRUE;
    pRef->iLookupsSent[uUserIndex] = 0;
    pRef->aLookupUsers[uUserIndex] = (DirtyUserT*)DirtyMemAlloc((sizeof(DirtyUserT) * iLookupUsersLength), USERAPI_MEMID, pRef->iMemGroup, pRef->pMemGroupUserData); 

    if ((pRef->aLookupUsers[uUserIndex]) == NULL)
    {
        NetCritLeave(&pRef->crit);
        NetPrintf(("userapips4: [%p] failed to allocate memory for user lookup.\n", pRef));
        return(-1);
    }

    ds_memcpy(pRef->aLookupUsers[uUserIndex], pLookupUsers, (sizeof(DirtyUserT) * iLookupUsersLength));
    NetCritLeave(&pRef->crit);
    return(0);
}

/*F*************************************************************************************************/
/*!
    \Function UserApiPlatRequestPresence

    \Description
        Initiates the presence request to PSN 
        
    \Input *pRef               - module ref
    \Input uUserIndex          - The index of the user making the request
    \Input pLookupUser        - Array of DirtyUserT struct containing the users to lookup

    \Output int32_t            - 0 for success, negative for error
        
    \Version 12/10/2013 (amakoukji)
*/
/*************************************************************************************************F*/
int32_t UserApiPlatRequestPresence(UserApiRefT *pRef, uint32_t uUserIndex, DirtyUserT *pLookupUser)
{
    // process right away, since this is never done in batches like profiles
    char strPath[USERAPI_PATH_RECENTLY_MET_LEN_MAX];
    SceNpAccountId AccountId;

    NetCritEnter(&pRef->crit);
    if (DirtyUserToNativeUser(&AccountId, sizeof(AccountId), pLookupUser))
    {
        ds_snzprintf(strPath, sizeof(strPath), USERAPI_PATH_PRESENCE_FORMAT, AccountId);
        pRef->UserPresenceList[uUserIndex].iTotalErrors = 0;
        pRef->UserPresenceList[uUserIndex].iTotalReceived = 0;
        pRef->UserPresenceList[uUserIndex].iTotalRequested = 1;

        if (!(DirtyWebApiRequest(pRef->pPlatformData->pWebApiPresence, uUserIndex, USERAPI_WEBAPI_GROUP, SCE_NP_WEBAPI_HTTP_METHOD_GET, strPath, NULL, 0, NULL, _UserApiWebPresenceRequestCallback, pRef) >= 0))
        {
            pRef->UserPresenceList[uUserIndex].iTotalErrors += 1;
            NetCritLeave(&pRef->crit);
            return(-1);
        }
    }

    NetCritLeave(&pRef->crit);

    return(0);
}

/*F*************************************************************************************************/
/*!
    \Function UserApiPlatRequestRichPresence

    \Description
        Initiates the rich presence request to PSN 
        
    \Input *pRef               - module ref
    \Input uUserIndex          - The index of the user making the request
    \Input pLookupUser        - Array of DirtyUserT struct containing the users to lookup

    \Output int32_t            - 0 for success, negative for error
        
    \Version 12/10/2013 (amakoukji)
*/
/*************************************************************************************************F*/
int32_t UserApiPlatRequestRichPresence(UserApiRefT *pRef, uint32_t uUserIndex, DirtyUserT *pLookupUser)
{
    // process right away, since this is never done in batches like profiles
    char strPath[USERAPI_PATH_RECENTLY_MET_LEN_MAX];
    SceNpAccountId AccountId;

    NetCritEnter(&pRef->crit);

    // for rich presence on PS4 its the same request as presence
    // if the presence request is started skip this
    pRef->UserRichPresenceList[uUserIndex].iTotalErrors = 0;
    pRef->UserRichPresenceList[uUserIndex].iTotalReceived = 0;
    pRef->UserRichPresenceList[uUserIndex].iTotalRequested = 1;
    if (pRef->UserPresenceList[uUserIndex].iTotalRequested == 0)
    {
        if (DirtyUserToNativeUser(&AccountId, sizeof(AccountId), pLookupUser))
        {
            ds_snzprintf(strPath, sizeof(strPath), USERAPI_PATH_PRESENCE_FORMAT, AccountId);

            if (!(DirtyWebApiRequest(pRef->pPlatformData->pWebApiPresence, uUserIndex, USERAPI_WEBAPI_GROUP, SCE_NP_WEBAPI_HTTP_METHOD_GET, strPath, NULL, 0, NULL, _UserApiWebPresenceRequestCallback, pRef) >= 0))
            {
                pRef->UserRichPresenceList[uUserIndex].iTotalErrors += 1;
                NetCritLeave(&pRef->crit);
                return(-1);
            }
        }
    }

    NetCritLeave(&pRef->crit);

    return(0);
}

/*F*************************************************************************************************/
/*!
    \Function UserApiPlatRequestRecentlyMet

    \Description
        Initiates the recently met player request to PSN Services
        
    \Input *pRef               - module ref
    \Input  uUserIndex         - The index of the user making the request
    \Input *pPlayerMet         - DirtyUserT struct containing the users to lookup
    \Input *pAdditionalInfo    - Additional user info

    \Output int32_t            - 0 for success, negative for error

    \Notes
        Addition info is required for PS4 as on SDK 4.00 as both Online ID and Account ID are now 
        needed. 
        See https://ps4.scedev.net/support/issue/103816/_Usage_of_the_Activity_Feed_with_PS4_SDK_4.00_and_account_IDs_as_primary_keys#n1384712
        
    \Version 06/14/2013 (amakoukji)
*/
/*************************************************************************************************F*/
int32_t UserApiPlatRequestRecentlyMet(UserApiRefT *pRef, uint32_t uUserIndex, DirtyUserT *pPlayerMet, void *pAdditionalInfo)
{
    char strPath[USERAPI_PATH_RECENTLY_MET_LEN_MAX];
    char strPostData[USERAPI_PATH_RECENTLY_MET_LEN_MAX];
    char strTitleId[SCE_NP_TITLE_ID_LEN + 1];
    SceNpAccountId RecentlyMetUser;
    SceNpAccountId LocalUser;

    if (NetConnStatus('acct', uUserIndex, &LocalUser, sizeof(LocalUser)) < 0)
    {
        NetPrintf(("userapips4: [%p] failed to get first party id of local user %d\n", pRef, uUserIndex));
        return(-1);
    }

    NetCritEnter(&pRef->postCrit);
    if (DirtyUserToNativeUser(&RecentlyMetUser, sizeof(RecentlyMetUser), pPlayerMet))
    {
        ds_memclr(strTitleId, sizeof(strTitleId));
        NetConnStatus('titl', 0, strTitleId, SCE_NP_TITLE_ID_LEN + 1);
        if (strlen(strTitleId) == 0)
        {
            NetPrintf(("userapips4: title id not set, cannot submit recently met players.\n"));
            NetCritLeave(&pRef->postCrit);
            return(-2);
        }

        ds_snzprintf(strPath, sizeof(strPath), USERAPI_PATH_RECENTLY_MET_FORMAT, LocalUser);
        ds_snzprintf(strPostData, sizeof(strPostData), USERAPI_PATH_RECENTLY_MET_POST_FORMAT, RecentlyMetUser, "", strTitleId);

        pRef->UserRmpList[uUserIndex].iTotalErrors = 0;
        pRef->UserRmpList[uUserIndex].iTotalReceived = 0;
        pRef->UserRmpList[uUserIndex].iTotalRequested = 1;

        if (DirtyWebApiRequest(pRef->pPlatformData->pWebApiRMP, uUserIndex, USERAPI_WEBAPI_RECENTLY_MET_GROUP, SCE_NP_WEBAPI_HTTP_METHOD_POST, strPath, (uint8_t*)strPostData, strlen(strPostData), SCE_NP_WEBAPI_CONTENT_TYPE_APPLICATION_JSON_UTF8, _UserApiWebRecentlyMetRequestCallback, pRef) < 0)
        {
            ++pRef->UserRmpList[uUserIndex].iTotalErrors;
        }
    }
    else
    {
        NetPrintf(("userapips4: [%p] failed to get first party id\n", pRef));
        NetCritLeave(&pRef->postCrit);
        return(-3);
    }

    NetCritLeave(&pRef->postCrit);
    return(0);
}

/*F*************************************************************************************************/
/*!
    \Function UserApiPlatRequestPostRichPresence

    \Description
        Initiates the recently met player request to PSN Services
        
    \Input *pRef               - module ref
    \Input uUserIndex          - The index of the user making the request
    \Input pData               - Rich presence data

    \Output int32_t            - 0 for success, negative for error
        
    \Version 06/14/2013 (amakoukji)
*/
/*************************************************************************************************F*/
int32_t UserApiPlatRequestPostRichPresence(UserApiRefT *pRef, uint32_t uUserIndex, UserApiRichPresenceT *pData)
{
    char strPath[USERAPI_PATH_RECENTLY_MET_LEN_MAX];
    char strPostData[USERAPI_PATH_RECENTLY_MET_LEN_MAX];
    SceNpAccountId LocalUser;
    int32_t data1Len = 0;
    
    strPostData[0] = '\0';
    if (pData)
    {
        data1Len = strlen(pData->strData);
    }

    if (NetConnStatus('acct', uUserIndex, &LocalUser, sizeof(LocalUser)) < 0)
    {
        NetPrintf(("userapips4: [%p] failed to get first party id of local user %d\n", pRef, uUserIndex));
        return(-3);
    }

    NetCritEnter(&pRef->postCrit);

    pRef->UserRmpList[uUserIndex].iTotalErrors = 0;
    pRef->UserRmpList[uUserIndex].iTotalReceived = 0;
    pRef->UserRmpList[uUserIndex].iTotalRequested = 1;

    ds_snzprintf(strPath, sizeof(strPath), USERAPI_PATH_RICH_PRESENCE_FORMAT1, LocalUser);

    if (data1Len > 0)
    {
        // setting game status
        ds_snzprintf(strPostData, sizeof(strPostData), USERAPI_PATH_RICH_PRESENCE_POST_FORMAT1, pData->strData);
    }

    if (DirtyWebApiRequest(pRef->pPlatformData->pWebApiRMP, 
                           uUserIndex, 
                           USERAPI_WEBAPI_GROUP, 
                           (data1Len > 0 ? SCE_NP_WEBAPI_HTTP_METHOD_PUT : SCE_NP_WEBAPI_HTTP_METHOD_DELETE), 
                           strPath, 
                           (data1Len > 0 ? (uint8_t*)strPostData : NULL), 
                           strlen(strPostData), 
                           (data1Len > 0 ? SCE_NP_WEBAPI_CONTENT_TYPE_APPLICATION_JSON_UTF8 : NULL), 
                           _UserApiWebRecentlyMetRequestCallback, 
                           pRef) < 0)
    {
        ++pRef->UserRmpList[uUserIndex].iTotalErrors;
    }

    NetCritLeave(&pRef->postCrit);
    return(0);
}

/*F********************************************************************************/
/*!
    \Function UserApiPlatAbortRequests

    \Description
        Abort all web requests for a user.

    \Input *pRef      - pointer to module state
    \Input uUserIndex - user index

    \Output
        int32_t     - Return 0 if successful. Otherwise a selector specific error

    \Version 09/26/2013 (amakoukji) - First documentation
*/
/********************************************************************************F*/
int32_t UserApiPlatAbortRequests(UserApiRefT *pRef, uint32_t uUserIndex)
{
    int32_t iFinal = 0;
    if ((pRef != NULL) && (pRef->pPlatformData != NULL))
    {
        int32_t iTmp = DirtyWebApiAbortRequests(pRef->pPlatformData->pWebApi, uUserIndex);
        if (iTmp < iFinal)
        {
            iFinal = iTmp;
        }

        iTmp = DirtyWebApiAbortRequests(pRef->pPlatformData->pWebApiPresence, uUserIndex);
        if (iTmp < iFinal)
        {
            iFinal = iTmp;
        }

        iTmp = _UserApiPlatAbortPostRequests(pRef, uUserIndex);
        if (iTmp < iFinal)
        {
            iFinal = iTmp;
        }
    }

    return(iFinal);
}

/*F*************************************************************************************************/
/*!
    \Function _UserApiPlatAbortPostRequests

    \Description
        Aborts the recently met player request to PSN Services
        
    \Input *pRef               - module ref
    \Input uUserIndex          - The index of the user making the request

    \Output int32_t            - 0 for success, negative for error
        
    \Version 06/14/2013 (amakoukji)
*/
/*************************************************************************************************F*/
int32_t _UserApiPlatAbortPostRequests(UserApiRefT *pRef, uint32_t uUserIndex)
{
    return(DirtyWebApiAbortRequests(pRef->pPlatformData->pWebApiRMP, uUserIndex));
}

/*F********************************************************************************/
/*!
    \Function UserApiPlatControl

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
            'avsz' - Set which avatar size will be retrieved. iValue = 's' for small, 'm' for medium and 'l' for big. 's' is the default value, and this is just functional for PS4.
            'rrsp' - iValue = 1 if UserApi is to return raw repsonse. iValue = 0 if UserApi is not to return raw response
        \endverbatim

    \Version 05/10/2013 (mcorcoran) - First version
*/
/********************************************************************************F*/
int32_t UserApiPlatControl(UserApiRefT *pRef, int32_t iControl, int32_t iValue, int32_t iValue2, void *pValue)
{
    if (iControl == 'avsz')
    {
        switch (iValue)
        {
            case 's':
            case 'm':
            case 'l':
                pRef->pPlatformData->pAvatarSize = iValue;
                return(0);

            default:
                NetPrintf(("userapips4: [%p] invalid avatar size\n", pRef));
                return(-1);
        }
    }
    else if (iControl == 'abrt')
    {
        if ((iValue < 0) || (iValue >= NETCONN_MAXLOCALUSERS))
        {
            NetPrintf(("userapips4: [%p] iValue(%d) is not a valid user index\n", pRef, iValue));
            return(-1);
        }

        pRef->pPostCallback[iValue] = NULL;
        pRef->pUserDataPost[iValue] = NULL;

        _UserApiPlatAbortPostRequests(pRef, iValue);

        pRef->UserRmpList[iValue].iTotalRequested = 0;
        pRef->UserRmpList[iValue].iTotalReceived = 0;
        pRef->UserRmpList[iValue].iTotalErrors = 0;

        return(0);
    }
    else if (iControl == 'rrsp')
    {
        pRef->pPlatformData->bRawResponse = iValue;
        return(0);
    }

    return(-1);
}

/*F********************************************************************************/
/*!
    \Function UserApiPlatRegisterUpdateEvent

    \Description
        Control behavior of module.

    \Input *pRef      - pointer to module state
    \Input uUserIndex - user index
    \Input eType      - type of callback to register

    \Output
        int32_t     - Return 0 if successful. Otherwise a selector specific error

    \Version 08/08/2013 (amakoukji) - First version
*/
/********************************************************************************F*/
int32_t UserApiPlatRegisterUpdateEvent(UserApiRefT *pRef, uint32_t uUserIndex, UserApiNotifyTypeE eType)
{
    int32_t iReturn = 0;
    switch (eType)
    {
    case USERAPI_NOTIFY_PRESENCE_UPDATE:
        iReturn = DirtyWebApiAddPushEventListener(pRef->pPlatformData->pWebApi, "np:service:presence:onlineStatus", _UserAliOnWebApiUpdate, pRef);
        break;

    case USERAPI_NOTIFY_TITLE_UPDATE:
        iReturn = DirtyWebApiAddPushEventListenerEx(pRef->pPlatformData->pWebApi, "np:service:presence:gameTitleInfo", "inGamePresence", SCE_NP_DEFAULT_SERVICE_LABEL, _UserAliOnWebApiUpdate, pRef);
        break;

    case USERAPI_NOTIFY_RICH_PRESENCE_UPDATE:
        iReturn = DirtyWebApiAddPushEventListenerEx(pRef->pPlatformData->pWebApi, "np:service:presence:gameStatus", "inGamePresence", SCE_NP_DEFAULT_SERVICE_LABEL, _UserAliOnWebApiUpdate, pRef);
        break;
    default:
        NetPrintf(("userapips4: event type not supported on PS4!\n"));
        iReturn = USERAPI_ERROR_UNSUPPORTED;
        break;
    }
    
    return(iReturn);
}
