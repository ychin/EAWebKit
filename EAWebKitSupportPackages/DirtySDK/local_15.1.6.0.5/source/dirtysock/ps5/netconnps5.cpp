/*H********************************************************************************/
/*!
    \File netconnps5.cpp

    \Description
        Provides network setup and teardown support. Does not actually create any
        kind of network connections.

    \Copyright
        Copyright (c) 2012 Electronic Arts Inc.

    \Version 10/24/2012 (jbrookes) First version; a vanilla port from the Unix version
*/
/********************************************************************************H*/


/*** Include files ****************************************************************/

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <libnetctl.h>
#include <libsysmodule.h>
#include <np.h>
#include <np/np_common.h>
#include <user_service.h>
#include <kernel.h>
#include <app_content.h>
#include <np/np_entitlement_access.h>
#include <system_param.h>
#include <system_service.h>

#if DIRTYCODE_PLAYFIRSTTRIAL
#include <np/np_entitlement_access_pft.h>
#endif

#include "DirtySDK/dirtysock.h"
#include "DirtySDK/dirtyvers.h"
#include "DirtySDK/dirtylang.h"
#include "DirtySDK/dirtysock/dirtyerr.h"
#include "DirtySDK/dirtysock/dirtymem.h"
#include "DirtySDK/dirtysock/dirtycert.h"
#include "DirtySDK/dirtysock/ps4/netconnps4.h"
#include "DirtySDK/dirtysock/ps4/dirtycontextmanagerps4.h"
#include "DirtySDK/dirtysock/ps5/dirtywebapi2ps5.h"
#include "DirtySDK/dirtysock/netconn.h"
#include "DirtySDK/proto/protoupnp.h"
#include "netconncommon.h"
#include "netconnlocaluser.h"

#include "IEAUser/IEAUser.h"
#include "EAUser/SonyUser.h"

/*** Defines **********************************************************************/

#define NETCONNPS4_SAMPLE_TITLE_ID         "NPXX53581_00"
#define NETCONNPS4_CLIENT_ID               "c87351d1-f7d9-4d53-8354-8608e71c1416"  //Harcoded Client Id from Sony
#define NETCONNPS4_SCOPE_FOR_AUTH_CODE     "psn:s2s openid id_token:psn.basic_claims"

// communication restriction webapi
#define NETCONNPS4_PATH_COMMRESTRICTION       "/v3/users/%llu/communication/restriction/status"
#define NETCONNPS4_API_GROUP_COMMRESTRICTION  "communicationRestrictionStatus"


//! UPNP port
#define NETCONN_DEFAULT_UPNP_PORT          (3659)

/*** Macros ***********************************************************************/

/*** Type Definitions *************************************************************/
typedef struct NetConnRefT NetConnRefT;
typedef struct NetConnAsyncRequestDataT NetConnAsyncRequestDataT;
typedef int32_t(*NetConnAsyncRequestT)(NetConnRefT *pRef, int32_t iUserIndex, SceUserServiceUserId userId, NetConnAsyncRequestDataT *pRequestData);
typedef int32_t(*NetConnAsyncResponseT)(NetConnRefT *pRef, int32_t iUserIndex, void *pBuf, int32_t iBufSize);

typedef enum NetConnNpRequestTypeE
{
    NETCONN_NP_REQUEST_TYPE_PREMIUM,                    //!< request for premium status
    NETCONN_NP_REQUEST_TYPE_REACHABILITY                //!< request for rechability check
} NetConnNpRequestTypeE;

typedef enum NetConnNpRequestStateE
{
    NETCONN_NP_REQUEST_STATE_INVALID = 0,               //!< auth code has not yet been requested
    NETCONN_NP_REQUEST_STATE_FAILED,                    //!< auth code acquisition failed
    NETCONN_NP_REQUEST_STATE_INPROG,                    //!< auth code acquisition in progress
    NETCONN_NP_REQUEST_STATE_VALID,                     //!< auth code valid
    NETCONN_NP_REQUEST_STATE_UNDERAGE                   //!< special case error code
} NetConnNpRequestStateE;

typedef struct NetConnUserAuthCodeT
{
    NetConnNpRequestStateE          eAuthCodeState;     //!< current progress of getting an auth code
    int32_t                         iAuthCodeRequestId; //!< request id into sce libs for auth code
    int32_t                         iIssuerId;          //!< auth code issuer id
    SceNpAuthorizationCode          NpAuthCode;         //!< completed auth code
} NetConnUserAuthCodeT;

struct NetConnAsyncRequestDataT
{
    NetConnNpRequestStateE          eRequestState;      //!< current progress of the async request
    int32_t                         iRequestId;         //!< request id for the async request
};

typedef struct NetConnReachabilityT
{
    NetConnAsyncRequestDataT        ReqData;           //!< async request info
    int32_t iResult;
} NetConnReachabilityT;

typedef struct NetConnPremiumT
{
    NetConnAsyncRequestDataT        ReqData;               //!< async request info
    SceNpCheckPremiumParameter      NpCheckPremiumParam;   //!< premium features to check
    SceNpCheckPremiumResult         NpCheckPremiumResult;  //!< premium check result
} NetConnPremiumT;

typedef struct NetConnCommRestrictionT
{
    NetConnAsyncRequestDataT        ReqData;               //!< async request info
    uint8_t                         bRestricted;           //!< communication restriction flag
} NetConnCommRestrictionT;

typedef struct NetConnSceNpAccountIdQueryT
{
    SceNpAccountId                  NpAccountId;
    int32_t                         iResult;
    uint8_t                         bInvalid;
    uint8_t                         _pad[3];
} NetConnSceNpAccountIdQueryT;

typedef struct NetConnSceNpOnlineIdQueryT
{
    SceNpOnlineId                   NpOnlineId;
    int32_t                         iResult;
    uint8_t                         bInvalid;
    uint8_t                         _pad[3];
} NetConnSceNpOnlineIdQueryT;

typedef struct NetConnUserT
{
    uint8_t                             isPushContextIdValid; //!< is push context id valid 
    SceNpOnlineId                       NpOnlineId;           //!< user's OnlineId
    SceNpReachabilityState              NpReachState;         //!< tracks the NP reach reachability state
    SceNpState                          NpState;              //!< tracks the login status of players to the Playstation Network     
    SceUserServiceUserId                UserServiceUserId;    //!< associated sceUserService id
    SceNpWebApi2PushEventPushContextId  PushEventContextId;   //!< push conext id
    NetConnSceNpOnlineIdQueryT          OnlineIdCache;        //!< online id cache
    NetConnSceNpAccountIdQueryT         AccountIdCache;       //!< account id cache
    NetConnUserAuthCodeT                AuthCode;             //!< tracks the progress of getting an auth code
    NetConnPremiumT                     Premium;              //!< tracks the progress of getting the premium membership
    NetConnReachabilityT                Reachability;         //!< tracks the progress of reachability to Playstation Network
    NetConnCommRestrictionT             CommRestriction;      //!< tracks the progress of communication restriction
} NetConnUserT;
 
typedef struct NetConnNpStateCallbackDataT
{
    SceUserServiceUserId            UserServiceUserId;
    SceNpState                      NpState;
    NetConnNpStateCallbackDataT     *pNext;
} NetConnNpStateCallbackDataT;

typedef struct NetConnSystemLangToLangCode
{
    int32_t iSystemLanguage;
    int32_t iLobbyLanguage;
} NetConnSystemLangToLangCode;

//! private module state
struct NetConnRefT
{
    NetConnCommonRefT    Common;                        //!< cross-platform netconn data (must come first!)
    NetConnLocalUserRefT *pLocalUser;                   //!< local user container for IEAUser
    NetConnUserT         aUsers[NETCONN_MAXLOCALUSERS]; //!< array of local users
    const EA::User::SonyUser *aEAUsers[NETCONN_MAXLOCALUSERS]; //!< EAUsers known by NetConn - populated when pIEAUserEventList is processed

    enum
    {
        ST_INIT,                                        //!< initialization
        ST_CONN,                                        //!< bringing up network interface
        ST_IDLE,                                        //!< active
        ST_EXT_CLEANUP                                  //!< cleaning up external instances from previous NetConn connection, can't proceed before all cleaned up
    } eState;                                           //!< internal connection state

    uint32_t             uConnStatus;                   //!< connection status (surfaced to user)
    uint32_t             uLastConnStatus;               //!< connection status before being reset by NetConnDisconnect()
    uint32_t             uPlatEnv;                      //!< Platform Environment
    uint32_t             uConnUserMode;                 //!< user mode, 0 = all user must be eligible to go +onl, 1 = only one user must be eligible to go +onl

    ProtoUpnpRefT        *pProtoUpnp;                   //!< protoupnp module state
    int32_t              iPeerPort;                     //!< peer port to be opened by upnp; if zero, still find upnp router but don't open a port
    int32_t              iNumProcCores;                 //!< number of processor cores on the system

    char                 strTitleId[SCE_NP_TITLE_ID_LEN + 1]; //!< sce title id
    SceKernelCpumask     CpuAffinity;                   //!< CPU affinity for threads used by sony
    SceNpTitleSecret     NpTitleSecret;                 //!< sce title secret
    SceNpClientId        NpClientId;                    //!< SceNpClientId used for getting auth code
    int32_t              iUserThreadPrio;               //!< the priority of the user service thread
    int32_t              iLastNpError;                  //!< holds the last error returned by a call to an sceNp library function
    int32_t              iRegisterStateCallbackId;      //!< holds the callback id from sceNpRegisterStateCallbackA
    int32_t              iThreadCpuAffinity;            //!< cpu affinity for our internal threads
    int32_t              iLocale;                       //!< locale information (system laguage + inital user country code)

    DirtyWebApiRefT      *pWebApi;                      //!< webapi ref

    NetConnNpStateCallbackT *pNpStateCallback;
    void                 *pNpStateCallbackUserData;
    NetConnNpStateCallbackDataT *pNpStateCallbackDataQueue;
    uint8_t              bNpInit;
    uint8_t              bSecretSet;                    //!< TRUE if secret has been set
    uint8_t              bConnectDelayed;               //!< if TRUE, NetConnConnect is being delayed until external cleanup phase completes
    uint8_t              bEnviRequest;                  //!< TRUE if the platform environment is requested.
    uint8_t              bNoTitleId;                     //!< TRUE if we want netconn not to set the title id and secret
    uint8_t              _pad[3];
};

/*** Function Prototypes **********************************************************/

/*** Variables ********************************************************************/

//TO DO: replace with the real secret once this is created
// Note that we do not know the use of this secret yet.
// It could be it is not a good idea to have it hardcoded
// like this for security reasons. If so, simply have
// NetConnConnect() return -1 if the secret is not set.
static uint8_t aDirtySdkSampleSecret[] = { 0x82, 0xcd, 0x3f, 0x20, 0xb7, 0x5a, 0xb5, 0x54, 0x2d, 0x0d, 0xfe, 0x04, 0xca, 0xcd, 0x54, 0x55, 0x8f, 0x81, 0xd8, 0xb4, 0x99, 0x44, 0x87, 0x17, 0x71, 0x8f, 0xef, 0xe6, 0x12, 0xb3, 0x2e, 0xdc, 0x36, 0x74, 0x5e, 0x85, 0xc4, 0xd2, 0x11, 0x74, 0xa8, 0xc1, 0x73, 0x07, 0xbf, 0xac, 0xd9, 0x85, 0x2d, 0x7d, 0xf3, 0xc1, 0x98, 0x7d, 0xbe, 0xd8, 0xa6, 0x47, 0xbf, 0x92, 0x13, 0xdb, 0x0d, 0x6e, 0x58, 0xc2, 0xaa, 0x66, 0xe6, 0x80, 0xcb, 0x16, 0xa4, 0xff, 0xe0, 0xf2, 0xab, 0x61, 0x5a, 0xea, 0x1c, 0xff, 0x59, 0x7c, 0x07, 0xa2, 0xd6, 0x69, 0x62, 0x9f, 0xa6, 0xd2, 0x2e, 0xa4, 0x15, 0x57, 0xc1, 0xf7, 0xfc, 0x1c, 0x3b, 0x37, 0x50, 0xdc, 0x90, 0x18, 0xe7, 0x85, 0xb8, 0x6e, 0xb8, 0xe0, 0x5f, 0x2b, 0x8e, 0x55, 0xea, 0x6e, 0xbb, 0x7b, 0x16, 0x06, 0x2a, 0xb2, 0x0f, 0xc3, 0x10, 0xfb };

#if DIRTYCODE_LOGGING
static const char *_NpState[] =
{
    "SCE_NP_STATE_UNKNOWN",            // 0
    "SCE_NP_STATE_SIGNED_OUT",         // 1
    "SCE_NP_STATE_SIGNED_IN"           // 2
};
#endif

static NetConnSystemLangToLangCode  _NetConn_PsnSysLanguageTable[] =
{
    { SCE_SYSTEM_PARAM_LANG_JAPANESE,      LOBBYAPI_LANGUAGE_JAPANESE   },
    { SCE_SYSTEM_PARAM_LANG_ENGLISH_US,    LOBBYAPI_LANGUAGE_ENGLISH    },
    { SCE_SYSTEM_PARAM_LANG_FRENCH,        LOBBYAPI_LANGUAGE_FRENCH     },
    { SCE_SYSTEM_PARAM_LANG_SPANISH,       LOBBYAPI_LANGUAGE_SPANISH    },
    { SCE_SYSTEM_PARAM_LANG_GERMAN,        LOBBYAPI_LANGUAGE_GERMAN     },
    { SCE_SYSTEM_PARAM_LANG_ITALIAN,       LOBBYAPI_LANGUAGE_ITALIAN    },
    { SCE_SYSTEM_PARAM_LANG_DUTCH,         LOBBYAPI_LANGUAGE_DUTCH      },
    { SCE_SYSTEM_PARAM_LANG_PORTUGUESE_PT, LOBBYAPI_LANGUAGE_PORTUGUESE },
    { SCE_SYSTEM_PARAM_LANG_RUSSIAN,       LOBBYAPI_LANGUAGE_RUSSIAN    },
    { SCE_SYSTEM_PARAM_LANG_KOREAN,        LOBBYAPI_LANGUAGE_KOREAN     },
    { SCE_SYSTEM_PARAM_LANG_CHINESE_T,     LOBBYAPI_LANGUAGE_CHINESE    },
    { SCE_SYSTEM_PARAM_LANG_CHINESE_S,     LOBBYAPI_LANGUAGE_CHINESE    },
    { SCE_SYSTEM_PARAM_LANG_FINNISH,       LOBBYAPI_LANGUAGE_FINNISH    },
    { SCE_SYSTEM_PARAM_LANG_SWEDISH,       LOBBYAPI_LANGUAGE_SWEDISH    },
    { SCE_SYSTEM_PARAM_LANG_DANISH,        LOBBYAPI_LANGUAGE_DANISH     },
    { SCE_SYSTEM_PARAM_LANG_NORWEGIAN,     LOBBYAPI_LANGUAGE_NORWEGIAN  },
    { SCE_SYSTEM_PARAM_LANG_POLISH,        LOBBYAPI_LANGUAGE_POLISH     },
    { SCE_SYSTEM_PARAM_LANG_PORTUGUESE_BR, LOBBYAPI_LANGUAGE_PORTUGUESE },
    { SCE_SYSTEM_PARAM_LANG_ENGLISH_GB,    LOBBYAPI_LANGUAGE_ENGLISH    },
    { SCE_SYSTEM_PARAM_LANG_TURKISH,       LOBBYAPI_LANGUAGE_TURKISH    },
    { SCE_SYSTEM_PARAM_LANG_SPANISH_LA,    LOBBYAPI_LANGUAGE_SPANISH    },
    { SCE_SYSTEM_PARAM_LANG_ARABIC,        LOBBYAPI_LANGUAGE_ARABIC     },
    { SCE_SYSTEM_PARAM_LANG_FRENCH_CA,     LOBBYAPI_LANGUAGE_FRENCH     },
    { SCE_SYSTEM_PARAM_LANG_CZECH,         LOBBYAPI_LANGUAGE_CZECH      },
    { SCE_SYSTEM_PARAM_LANG_HUNGARIAN,     LOBBYAPI_LANGUAGE_HUNGARIAN  },
    { SCE_SYSTEM_PARAM_LANG_GREEK,         LOBBYAPI_LANGUAGE_GREEK      },
    { SCE_SYSTEM_PARAM_LANG_ROMANIAN,      LOBBYAPI_LANGUAGE_ROMANIAN   },
    { SCE_SYSTEM_PARAM_LANG_THAI,          LOBBYAPI_LANGUAGE_THAI       },
    { SCE_SYSTEM_PARAM_LANG_VIETNAMESE,    LOBBYAPI_LANGUAGE_VIETNAMESE },
    { SCE_SYSTEM_PARAM_LANG_INDONESIAN,    LOBBYAPI_LANGUAGE_INDONESIAN },
    { -1,                                  LOBBYAPI_LANGUAGE_UNKNOWN    }
};

//! global module ref
static NetConnRefT *_NetConn_pRef = NULL;

/*** Private Functions ************************************************************/

static int32_t _NetConnSaveErr(NetConnRefT *pRef, int32_t iError)
{
    if (iError < 0)
    {
        pRef->iLastNpError = iError;
        DirtyErrAppReport(iError);
    }
    return iError;
}

static int32_t _NetConnPsnLangToLobbyLang(int32_t iSystemLanguage)
{
    int32_t iIndex = 0;
    while (_NetConn_PsnSysLanguageTable[iIndex].iSystemLanguage != -1)
    {
        if (iSystemLanguage == _NetConn_PsnSysLanguageTable[iIndex].iSystemLanguage)
        {
            return(_NetConn_PsnSysLanguageTable[iIndex].iLobbyLanguage);
        }
        ++iIndex;
    }
    return(LOBBYAPI_LANGUAGE_UNKNOWN);
}

/*F********************************************************************************/
/*!
    \Function   _NetConnInvalidateUserIdCache

    \Description
        Invalidates both the online and account id caches

    \Input *pRef             - pointer to net NetConn module ref

    \Version 04/03/2017 (tcho)
*/
/********************************************************************************F*/
static void _NetConnInvalidateUserIdCache(NetConnRefT *pRef)
{
    int32_t iLocalUserIndex;

    // mark all online ID queries as invalid
    NetCritEnter(&pRef->Common.crit);
    for (iLocalUserIndex = 0; iLocalUserIndex < NETCONN_MAXLOCALUSERS; iLocalUserIndex++)
    {
        pRef->aUsers[iLocalUserIndex].OnlineIdCache.bInvalid = TRUE;
        pRef->aUsers[iLocalUserIndex].AccountIdCache.bInvalid = TRUE;
    }
    NetCritLeave(&pRef->Common.crit);
}

/*F********************************************************************************/
/*!
    \Function   _NetConnUpdateConnStatus

    \Description
        Update the Connection Status and provide logging of status changes

    \Input *pRef             - pointer to net NetConn module ref
    \Input *uNewConnStatus   - the new conn status

    \Version 01/19/2015 (tcho)
*/
/********************************************************************************F*/
static void _NetConnUpdateConnStatus(NetConnRefT *pRef, uint32_t uNewConnStatus)
{
    int32_t iIndex;
    char strConnStatus[5];

    pRef->uConnStatus = uNewConnStatus;

    for (iIndex = 0; iIndex < 4; ++iIndex)
    {
        strConnStatus[iIndex] = ((char *) &uNewConnStatus)[3 - iIndex];
    }

    strConnStatus[4] = 0;

    NetPrintf(("netconnps5: netconn status changed to %s\n", strConnStatus));
}

/*F********************************************************************************/
/*!
    \Function   _NetConnNpCheckNpRechabilityError

    \Description
        Processes the current NP rechability error and determines the best
        error state to report to the user through NetConnStatus.

    \Input *pRef          - module state

    \Version 11/11/2019 (tcho)
*/
/********************************************************************************F*/
static void _NetConnNpCheckNpRechabilityError(NetConnRefT *pRef)
{
    int32_t iFirstErrorResult = 0;
    int32_t iLocalUserIndex;

    //find the first error result, note that this will not necessarily map to player 1
    //also assign the lowest priority to "SCE_NP_ERROR_SIGNED_OUT" since any players 
    //not signed in will be in this state
    for (iLocalUserIndex = 0; iLocalUserIndex < NETCONN_MAXLOCALUSERS; ++iLocalUserIndex)
    {
        if (pRef->aUsers[iLocalUserIndex].Reachability.iResult < 0)
        {
            iFirstErrorResult = pRef->aUsers[iLocalUserIndex].Reachability.iResult;
            if (pRef->aUsers[iLocalUserIndex].Reachability.iResult != SCE_NP_ERROR_SIGNED_OUT)
            {
                break;
            }
        }
    }

    //choose an error state based on the first error result
    switch (iFirstErrorResult)
    {
    case SCE_NP_ERROR_OUT_OF_MEMORY:
    case SCE_NP_ERROR_ABORTED:
    case SCE_NP_ERROR_REQUEST_NOT_FOUND:
        NetPrintf(("netconnps5: _NetConnNpCheckNpRechabilityError() error: %s \n", DirtyErrGetName(iFirstErrorResult)));
        _NetConnUpdateConnStatus(pRef, '-err');
        break;

    case SCE_NP_ERROR_SIGNED_OUT:
    case SCE_NP_ERROR_USER_NOT_FOUND:
    case SCE_NP_ERROR_NOT_SIGNED_UP:
    case SCE_NP_ERROR_LOGOUT:
        NetPrintf(("netconnps5: _NetConnNpCheckNpRechabilityError() account error: %s \n", DirtyErrGetName(iFirstErrorResult)));
        _NetConnUpdateConnStatus(pRef, '-act');
        break;

    case SCE_NP_ERROR_AGE_RESTRICTION:
        NetPrintf(("netconnps5:_NetConnNpCheckNpRechabilityError() account error: %s \n", DirtyErrGetName(iFirstErrorResult)));
        _NetConnUpdateConnStatus(pRef, '-uda');
        break;

    case SCE_NP_ERROR_LATEST_SYSTEM_SOFTWARE_EXIST:
    case SCE_NP_ERROR_LATEST_SYSTEM_SOFTWARE_EXIST_FOR_TITLE:
        NetPrintf(("netconnps5:_NetConnNpCheckNpRechabilityError() system update required: %s \n", DirtyErrGetName(iFirstErrorResult)));
        _NetConnUpdateConnStatus(pRef, '-ups');
        break;

    case SCE_NP_ERROR_LATEST_PATCH_PKG_EXIST:
    case SCE_NP_ERROR_LATEST_PATCH_PKG_DOWNLOADED:
        NetPrintf(("netconnps5:_NetConnNpCheckNpRechabilityError() patch update required: %s \n", DirtyErrGetName(iFirstErrorResult)));
        _NetConnUpdateConnStatus(pRef, '-upp');
        break;

    default:
        NetPrintf(("netconnps5:_NetConnNpCheckNpRechabilityError() unknown error: %s\n", DirtyErrGetName(iFirstErrorResult)));
        _NetConnUpdateConnStatus(pRef, '-err');
        break;
    }
}

/*F********************************************************************************/
/*!
    \Function   _NetConnRequestAuthCode

    \Description
        Initiate async op for acquisition of auth code from sce servers

    \Input  *pRef            - state ref
    \Input  iLocalUserIndex  - local user index

    \Output
        int32_t             - <0=error, 0=retry (a request is already pending), >0=request issued

    \Version 04/09/2013 (cvienneau)
*/
/********************************************************************************F*/
static int32_t _NetConnRequestAuthCode(NetConnRefT *pRef, int32_t iLocalUserIndex)
{
    SceUserServiceUserId userId = NetConnStatus('suid', iLocalUserIndex,  NULL, 0);
    int32_t iRet = 0; 

    if (userId >= 0)
    {
        if (pRef->aUsers[iLocalUserIndex].AuthCode.eAuthCodeState != NETCONN_NP_REQUEST_STATE_INPROG)
        {
            SceNpAuthCreateAsyncRequestParameter asyncParam;
            ds_memclr(&asyncParam, sizeof(asyncParam));
            asyncParam.size = sizeof(asyncParam);
            asyncParam.threadPriority = pRef->iUserThreadPrio;
            asyncParam.cpuAffinityMask = pRef->CpuAffinity;

            iRet = pRef->aUsers[iLocalUserIndex].AuthCode.iAuthCodeRequestId = _NetConnSaveErr(pRef, sceNpAuthCreateAsyncRequest(&asyncParam));

            if (iRet > 0)
            {
                SceNpAuthGetAuthorizationCodeParameterV3 authParam;

                ds_memclr(&authParam, sizeof(authParam));
                ds_memclr(&pRef->aUsers[iLocalUserIndex].AuthCode.NpAuthCode, sizeof(pRef->aUsers[iLocalUserIndex].AuthCode.NpAuthCode));

                authParam.size = sizeof(authParam);
                authParam.userId = userId;
                authParam.clientId = &pRef->NpClientId;
                authParam.scope = NETCONNPS4_SCOPE_FOR_AUTH_CODE;

                iRet = _NetConnSaveErr(pRef, sceNpAuthGetAuthorizationCodeV3(pRef->aUsers[iLocalUserIndex].AuthCode.iAuthCodeRequestId, &authParam, &pRef->aUsers[iLocalUserIndex].AuthCode.NpAuthCode, &pRef->aUsers[iLocalUserIndex].AuthCode.iIssuerId));
                if (iRet == 0)
                {
                    pRef->aUsers[iLocalUserIndex].AuthCode.eAuthCodeState = NETCONN_NP_REQUEST_STATE_INPROG;
                }
                else
                {
                    NetPrintf(("netconnps5: _NetConnRequestAuthCode sceNpAuthGetAuthorizationCode() failed, err = %s \n", DirtyErrGetName(iRet)));
                    pRef->aUsers[iLocalUserIndex].AuthCode.eAuthCodeState = NETCONN_NP_REQUEST_STATE_FAILED;
                    iRet = -1;
                }
            }
            else if (iRet == 0)
            {
                NetPrintf(("netconnps5: _NetConnRequestAuthCode sceNpAuthCreateAsyncRequest() failed with result 0, most likely the required PRX is not loaded \n"));
                pRef->aUsers[iLocalUserIndex].AuthCode.eAuthCodeState = NETCONN_NP_REQUEST_STATE_FAILED;
                iRet = -1;
            }
            else
            {
                NetPrintf(("netconnps5: _NetConnRequestAuthCode sceNpAuthCreateAsyncRequest() failed, err = %s \n", DirtyErrGetName(iRet)));
                pRef->aUsers[iLocalUserIndex].AuthCode.eAuthCodeState = NETCONN_NP_REQUEST_STATE_FAILED;
                iRet = -1;
            }
        }
        else
        {
            //in progress
            iRet = 0;
        }
    }
    else
    {
        NetPrintf(("netconnps5: _NetConnRequestAuthCode invalid user, err = %d \n", iRet));
        iRet = -1;
    }

    return(iRet);
}

/*F********************************************************************************/
/*!
    \Function   _NetConnCleanupAuthCodeRequest

    \Description
        Abort async op if in progress, and delete request.

    \Input *pRef            - state ref
    \Input iLocalUserIndex  - local user index

    \Version 04/09/2013 (cvienneau)
*/
/********************************************************************************F*/
static void _NetConnCleanupAuthCodeRequest(NetConnRefT *pRef, int32_t iLocalUserIndex)
{
    //be sure the user index makes sense
    if ((iLocalUserIndex >= 0) && (iLocalUserIndex < NETCONN_MAXLOCALUSERS))
    {
        int32_t iRet;
        //if there is an operation in progress we should abort the request first
        if (pRef->aUsers[iLocalUserIndex].AuthCode.eAuthCodeState == NETCONN_NP_REQUEST_STATE_INPROG)
        {
            iRet = sceNpAuthAbortRequest(pRef->aUsers[iLocalUserIndex].AuthCode.iAuthCodeRequestId);
            if (iRet != 0)
            {
                NetPrintf(("netconnps5: _NetConnCleanupAuthCodeRequest, sceNpAuthAbortRequest() failed, err = %s \n", DirtyErrGetName(iRet)));
            }
            pRef->aUsers[iLocalUserIndex].AuthCode.eAuthCodeState = NETCONN_NP_REQUEST_STATE_INVALID;
        }
        //delete the request
        if (pRef->aUsers[iLocalUserIndex].AuthCode.iAuthCodeRequestId != 0)
        {
            iRet = sceNpAuthDeleteRequest(pRef->aUsers[iLocalUserIndex].AuthCode.iAuthCodeRequestId);
            if (iRet != 0)
            {
                NetPrintf(("netconnps5: _NetConnCleanupAuthCodeRequest, sceNpAuthDeleteRequest() failed, err = %s \n", DirtyErrGetName(iRet)));
            }
            pRef->aUsers[iLocalUserIndex].AuthCode.iAuthCodeRequestId = 0;
        }
    }
    else
    {
        NetPrintf(("netconnps5: _NetConnCleanupAuthCodeRequest, invalid user id %d\n", iLocalUserIndex));
    }
}

/*F********************************************************************************/
/*!
    \Function   _NetConnUpdateAuthCodeRequests

    \Description
        Poll for completion of ticket request

    \Input *pRef            - state ref

    \Version 04/09/2013 (cvienneau)
*/
/********************************************************************************F*/
static void _NetConnUpdateAuthCodeRequests(NetConnRefT *pRef)
{
    int32_t iLocalUserIndex;

    //loop over the players, see if any of them have an outstanding request
    for (iLocalUserIndex = 0; iLocalUserIndex < NETCONN_MAXLOCALUSERS; iLocalUserIndex++)
    {
        if (pRef->aUsers[iLocalUserIndex].AuthCode.eAuthCodeState == NETCONN_NP_REQUEST_STATE_INPROG)
        {
            int32_t iResult = 0;
            int32_t iRet = sceNpAuthPollAsync(pRef->aUsers[iLocalUserIndex].AuthCode.iAuthCodeRequestId, &iResult);
            if (iResult >= 0)
            {
                if (iRet == SCE_NP_AUTH_POLL_ASYNC_RET_RUNNING)
                {
                    //continue waiting
                }
                else if (iRet == SCE_NP_AUTH_POLL_ASYNC_RET_FINISHED)
                {
                    pRef->aUsers[iLocalUserIndex].AuthCode.eAuthCodeState = NETCONN_NP_REQUEST_STATE_VALID;

                    if (pRef->bEnviRequest)
                    {
                        pRef->uPlatEnv = NETCONN_PLATENV_PROD;

                        /*  IssuerID == 1    --> SPINT Users
                            IssuerID == 8    --> PROD_QA Users
                            IssuerID == 256  --> NP Users */
                        if (pRef->aUsers[iLocalUserIndex].AuthCode.iIssuerId == 1)
                        {
                            pRef->uPlatEnv = NETCONN_PLATENV_TEST;
                            NetPrintf(("netconnps5: Platform Environment set to TEST.\n "));
                        }
                        else if (pRef->aUsers[iLocalUserIndex].AuthCode.iIssuerId == 8)
                        {
                            pRef->uPlatEnv = NETCONN_PLATENV_CERT;
                            NetPrintf(("netconnps5: Platform Environment set to CERT.\n"));
                        }
                        else if (pRef->aUsers[iLocalUserIndex].AuthCode.iIssuerId == 256)
                        {
                            NetPrintf(("netconnps5: Platform Environment set to PROD.\n"));
                        }
                        else
                        {
                            NetPrintf(("netconnps5: _NetConnUpdateAuthCodeRequests() Unknow Issuer Id. Environment defaulted to PROD.\n"));
                        }
                    }

                    pRef->bEnviRequest = FALSE;
                }
                else
                {
                    NetPrintf(("netconnps5: _NetConnUpdateAuthCodeRequests, sceNpAuthPollAsync() failed, err = %s \n", DirtyErrGetName(iRet)));
                    pRef->aUsers[iLocalUserIndex].AuthCode.eAuthCodeState = NETCONN_NP_REQUEST_STATE_FAILED;

                    //Need to reset the state in case of a failure when requesting the environment
                    if (pRef->bEnviRequest == TRUE)
                    {
                        pRef->bEnviRequest = FALSE;
                        _NetConnUpdateConnStatus(pRef, '-err');
                        pRef->eState = NetConnRefT::ST_INIT;
                    }
                }
            }
            else
            {
                NetPrintf(("netconnps5: _NetConnUpdateAuthCodeRequests, sceNpAuthPollAsync() failed iResult, err = %s \n", DirtyErrGetName(iResult)));
                pRef->aUsers[iLocalUserIndex].AuthCode.eAuthCodeState = NETCONN_NP_REQUEST_STATE_FAILED;

                //Need to reset the state in case of a failure when requesting the environment
                if (pRef->bEnviRequest == TRUE)
                {
                    pRef->bEnviRequest = FALSE;
                    _NetConnUpdateConnStatus(pRef, '-err');
                    pRef->eState = NetConnRefT::ST_INIT;
                }
            }

            if (pRef->aUsers[iLocalUserIndex].AuthCode.eAuthCodeState != NETCONN_NP_REQUEST_STATE_INPROG)
            {
                _NetConnCleanupAuthCodeRequest(pRef, iLocalUserIndex);
            }
        }
    }
}

/*F********************************************************************************/
/*!
    \Function   _NetConnReadAuthCode

    \Description
        Fills pBuf with auth code for target user, if available.

    \Input *pRef            - NetConnRefT reference
    \Input iLocalUserIndex  - local user index
    \Input *pBuf            - buffer to write auth code
    \Input iBufSize         - size of supplied buffer, at this time sizeof(SceNpAuthorizationCode) == 136

    \Output
        int32_t             -  0:"try again"; <0:error; >0:number of bytes copied into pBuf

    \Version 04/10/2013 (cvienneau)
*/
/********************************************************************************F*/
static int32_t _NetConnReadAuthCode(NetConnRefT *pRef, int32_t iLocalUserIndex, void *pBuf, int32_t iBufSize)
{
    int32_t iRet = 0;
    //be sure the user index makes sense
    if ((iLocalUserIndex >= 0) && (iLocalUserIndex < NETCONN_MAXLOCALUSERS))
    {
        if (pRef->aUsers[iLocalUserIndex].AuthCode.eAuthCodeState == NETCONN_NP_REQUEST_STATE_VALID)
        {
            //be sure we have enough buffer space
            if(pBuf == NULL)
            {
                //Returns the size of the ticket (to be consistent with other platforms)
                iRet = sizeof(SceNpAuthorizationCode);
            }
            else if ((pBuf != NULL) && (iBufSize >= (int32_t)sizeof(SceNpAuthorizationCode)))
            {
                ds_memclr(pBuf, iBufSize);
                ds_memcpy(pBuf, &pRef->aUsers[iLocalUserIndex].AuthCode.NpAuthCode, sizeof(SceNpAuthorizationCode));
                iRet = sizeof(SceNpAuthorizationCode);
            }
            else
            {
                NetPrintf(("netconnps5: NetConnStatus('tick') called with invalid buffer space.\n"));
                iRet = -1;
            }
        }
        else if (pRef->aUsers[iLocalUserIndex].AuthCode.eAuthCodeState == NETCONN_NP_REQUEST_STATE_INPROG)
        {
            iRet = 0;
        }
        else if (pRef->aUsers[iLocalUserIndex].AuthCode.eAuthCodeState == NETCONN_NP_REQUEST_STATE_INVALID)
        {
            //the user should have used the 'tick' control but we can kick off the first time for them
            iRet = _NetConnRequestAuthCode(pRef, iLocalUserIndex);
        }
        else
        {
            //its either in a failed state or a state that is unknown, there should already have been a print about this transition, lets not spam
            iRet = -1;
        }
    }
    else
    {
        NetPrintf(("netconnps5: NetConnStatus('tick') called with invalid user index %d.\n", iLocalUserIndex));
        iRet = -1;
    }
    return(iRet);
}

/*F********************************************************************************/
/*!
    \Function   _NetConnNpAsyncRequest

    \Description
        Initiate async request against NP services

    \Input *pRef                - state ref
    \Input iLocalUserIndex      - local user index
    \Input *pRequestData        - pointer to the NetConnAsyncRequestDataT struct to track this request
    \Input *pRequestCallback    - pointer to a NetConnAsyncRequestT callback which is called to initiate the actual SceNp request

    \Output
        int32_t                 - <0=error, 0=retry (a request is already pending), >0=request issued

    \Version 06/20/2013 (mcorcoran) - used code from _NetConnRequestAuthCode
*/
/********************************************************************************F*/
static int32_t _NetConnNpAsyncRequest(NetConnRefT *pRef, int32_t iLocalUserIndex, NetConnAsyncRequestDataT *pRequestData, NetConnAsyncRequestT pRequestCallback)
{
    SceUserServiceUserId userId = NetConnStatus('suid', iLocalUserIndex, NULL, 0);
    int32_t iRet = 0;
    if (userId >= 0)
    {
        if (pRequestData->eRequestState != NETCONN_NP_REQUEST_STATE_INPROG)
        {
            SceNpCreateAsyncRequestParameter asyncParam;
            ds_memclr(&asyncParam, sizeof(asyncParam));
            asyncParam.size = sizeof(asyncParam);
            asyncParam.threadPriority = pRef->iUserThreadPrio;
            asyncParam.cpuAffinityMask = pRef->CpuAffinity;

            iRet = pRequestData->iRequestId = _NetConnSaveErr(pRef, sceNpCreateAsyncRequest(&asyncParam));

            if (iRet > 0)
            {
                iRet = _NetConnSaveErr(pRef, pRequestCallback(pRef, iLocalUserIndex, userId, pRequestData));
                if (iRet == 0)
                {
                    pRequestData->eRequestState = NETCONN_NP_REQUEST_STATE_INPROG;
                }
                else
                {
                    NetPrintf(("netconnps5: _NetConnNpAsyncRequest(), call to pRequestCallback failed, err = %s \n", DirtyErrGetName(iRet)));
                    pRequestData->eRequestState = NETCONN_NP_REQUEST_STATE_FAILED;
                    iRet = -1;
                }
            }
            else
            {
                NetPrintf(("netconnps5: _NetConnNpAsyncRequest(), sceNpCreateAsyncRequest() failed, err = %s \n", DirtyErrGetName(iRet)));
                pRequestData->eRequestState = NETCONN_NP_REQUEST_STATE_FAILED;
                iRet = -1;
            }
        }
        else
        {
            //in progress
            iRet = 0;
        }
    }
    else
    {
        NetPrintf(("netconnps5: _NetConnNpAsyncRequest(), invalid user, err = %d \n", iRet));
        iRet = -1;
    }

    return(iRet);
}

/*F********************************************************************************/
/*!
    \Function   _NetConnCleanupNpRequest

    \Description
        Abort async op if in progress, and delete request.

    \Input *pRef            - state ref
    \Input iLocalUserIndex  - local user index
    \Input *pRequestData    - associated NetConnAsyncRequestDataT struct for the current request

    \Version 06/20/2013 (mcorcoran)
*/
/********************************************************************************F*/
static void _NetConnCleanupNpRequest(NetConnRefT *pRef, int32_t iLocalUserIndex, NetConnAsyncRequestDataT *pRequestData)
{
    //be sure the user index makes sense
    if ((iLocalUserIndex >= 0) && (iLocalUserIndex < NETCONN_MAXLOCALUSERS))
    {
        int32_t iRet;
        //if there is an operation in progress we should abort the request first
        if (pRequestData->eRequestState == NETCONN_NP_REQUEST_STATE_INPROG)
        {
            iRet = sceNpAbortRequest(pRequestData->iRequestId);
            if (iRet != 0)
            {
                NetPrintf(("netconnps5: _NetConnCleanupNpRequest(), sceNpAbortRequest() failed, err = %s \n", DirtyErrGetName(iRet)));
            }
            pRequestData->eRequestState = NETCONN_NP_REQUEST_STATE_INVALID;
        }
        else if (pRequestData->eRequestState == NETCONN_NP_REQUEST_STATE_FAILED)
        {
            pRequestData->eRequestState = NETCONN_NP_REQUEST_STATE_INVALID;
        }

        //delete the request
        if (pRequestData->iRequestId != 0)
        {
            iRet = sceNpDeleteRequest(pRequestData->iRequestId);
            if (iRet != 0)
            {
                NetPrintf(("netconnps5: _NetConnCleanupNpRequest(), sceNpDeleteRequest() failed, err = %s \n", DirtyErrGetName(iRet)));
            }
            pRequestData->iRequestId = 0;
        }
    }
    else
    {
        NetPrintf(("netconnps5: _NetConnCleanupNpRequest(), invalid user id %d\n", iLocalUserIndex));
    }
}

/*F********************************************************************************/
/*!
    \Function   _NetConnUpdateNpRequests

    \Description
        Poll for completion of ticket request

    \Input *pRef    - state ref
    \Input eReqType - request type

    \Version 06/20/2013 (mcorcoran) 
*/
/********************************************************************************F*/
static void _NetConnUpdateNpRequests(NetConnRefT *pRef, NetConnNpRequestTypeE eReqType)
{
    int32_t iLocalUserIndex;

    //loop over the players, see if any of them have an outstanding request
    for (iLocalUserIndex = 0; iLocalUserIndex < NETCONN_MAXLOCALUSERS; iLocalUserIndex++)
    {
        NetConnAsyncRequestDataT *pRequestData;

        switch (eReqType)
        {
            case NETCONN_NP_REQUEST_TYPE_PREMIUM:
                pRequestData = &pRef->aUsers[iLocalUserIndex].Premium.ReqData;
                break;
            
            case NETCONN_NP_REQUEST_TYPE_REACHABILITY:
                pRequestData = &pRef->aUsers[iLocalUserIndex].Reachability.ReqData;
                break;

            default:
                NetPrintf(("netconnps5: _NetConnUpdateNpRequests() early exits because request type (%d) is not supported \n", eReqType));
                return;
        }

        if (pRequestData->eRequestState == NETCONN_NP_REQUEST_STATE_INPROG)
        {
            int32_t iResult = 0;
            int32_t iRet = sceNpPollAsync(pRequestData->iRequestId, &iResult);
            if (iResult >= 0)
            {
                if (iRet == SCE_NP_POLL_ASYNC_RET_RUNNING)
                {
                    //continue waiting
                }
                else if (iRet == SCE_NP_POLL_ASYNC_RET_FINISHED)
                {
                    pRequestData->eRequestState = NETCONN_NP_REQUEST_STATE_VALID;

                    if (eReqType == NETCONN_NP_REQUEST_TYPE_REACHABILITY)
                    {
                        // request done, capture the result
                        pRef->aUsers[iLocalUserIndex].Reachability.iResult = iResult;
                    }

                    if (iResult == SCE_NP_ERROR_AGE_RESTRICTION)
                    {
                        // age restricted
                        NetPrintf(("netconnps5: _NetConnUpdateNpRequests(), sceNpPollAsync() underage user detected, err = SCE_NP_ERROR_AGE_RESTRICTION \n"));
                        pRequestData->eRequestState = NETCONN_NP_REQUEST_STATE_UNDERAGE;
                    }
                }
                else
                {
                    NetPrintf(("netconnps5: _NetConnUpdateNpRequests(), sceNpPollAsync() failed, err = %s \n", DirtyErrGetName(iRet)));
                    pRequestData->eRequestState = NETCONN_NP_REQUEST_STATE_FAILED;
                }
            }
            else
            {
                NetPrintf(("netconnps5: _NetConnUpdateNpRequests(), sceNpPollAsync() failed iResult, err = %s \n", DirtyErrGetName(iResult)));
                pRequestData->eRequestState = NETCONN_NP_REQUEST_STATE_FAILED;
            }

            if (pRequestData->eRequestState != NETCONN_NP_REQUEST_STATE_INPROG)
            {
                _NetConnCleanupNpRequest(pRef, iLocalUserIndex, pRequestData);
            }
        }
    }
}

/*F********************************************************************************/
/*!
    \Function   _NetConnReadAsyncResponse

    \Description
        Fill pBuf with data appropriate to the request type

    \Input *pRef                - NetConnRefT reference
    \Input iLocalUserIndex      - local user index
    \Input *pBuf                - buffer to copy result
    \Input iBufSize             - size of supplied buffer
    \Input *pRequestData        - pRequestCallback
    \Input pRequestCallback     - the callback to initiate the SceNp Async request
    \Input pResponseCallback    - the callback to read the SceNpa Async response

    \Output
        int32_t                 - 0 try again, negative = erros, positive = number of bytes copied into pBuf
*/
/********************************************************************************F*/
static int32_t _NetConnReadAsyncResponse(NetConnRefT *pRef, int32_t iLocalUserIndex, void *pBuf, int32_t iBufSize, NetConnAsyncRequestDataT *pRequestData, NetConnAsyncRequestT pRequestCallback, NetConnAsyncResponseT pResponseCallback)
{
    int32_t iRet = 0;
    //be sure the user index makes sense
    if ((iLocalUserIndex >= 0) && (iLocalUserIndex < NETCONN_MAXLOCALUSERS))
    {
        if (pRequestData->eRequestState == NETCONN_NP_REQUEST_STATE_VALID)
        {
            iRet = pResponseCallback(pRef, iLocalUserIndex, pBuf, iBufSize);
        }
        else if (pRequestData->eRequestState == NETCONN_NP_REQUEST_STATE_INPROG)
        {
            iRet = 0;
        }
        else if (pRequestData->eRequestState == NETCONN_NP_REQUEST_STATE_INVALID)
        {
            if (pRequestCallback != NULL)
            {
                //the user should have used the related NetConnControl() selector, but we can kick off the first time for them
                iRet = _NetConnNpAsyncRequest(pRef, iLocalUserIndex, pRequestData, pRequestCallback);
            }
            else
            {
                NetPrintf(("netconnps5: _NetConnReadAsyncResponse() called without first initiating the request with NetConnControl(), user index %d\n", iLocalUserIndex));
                iRet = -2;
            }
        }
        else if (pRequestData->eRequestState == NETCONN_NP_REQUEST_STATE_UNDERAGE)
        {
            iRet = SCE_NP_ERROR_AGE_RESTRICTION;
        }
        else
        {
            //its either in a failed state or a state that is unknown, there should already have been a print about this transition, lets not spam
            iRet = -1;
        }
    }
    else
    {
        NetPrintf(("netconnps5: _NetConnReadAsyncResponse() called with invalid user index %d\n", iLocalUserIndex));
        iRet = -1;
    }
    return(iRet);
}

/*F********************************************************************************/
/*!
    \Function   _NetConnCommRestrictionRequestCallback

    \Description
        WebApi callback to the Communication Restriction status

    \Input iSceError       - Sce Error
    \Input iUserIndex      - local user index
    \Input iStatusCode     - Http status code
    \Input pRespBody       - response body
    \Input iRespBodyLength - response body length
    \Input pUserData       - 

    \Version 11/14/2019 (tcho)
*/
/********************************************************************************F*/
static void _NetConnCommRestrictionRequestCallback(int32_t iSceError, int32_t iUserIndex, int32_t iStatusCode, const char *pRespBody, int32_t iRespBodyLength, void *pUserData)
{
    NetConnRefT *pRef = (NetConnRefT *)pUserData;
    
    NetCritEnter(&pRef->pLocalUser->crit);

    if (pRef->aUsers[iUserIndex].CommRestriction.ReqData.eRequestState == NETCONN_NP_REQUEST_STATE_INPROG)
    {
        if (pRespBody != NULL)
        {
            pRef->aUsers[iUserIndex].CommRestriction.bRestricted = (strstr(pRespBody, "\"restricted\":true") != NULL) ? TRUE : FALSE;
            pRef->aUsers[iUserIndex].CommRestriction.ReqData.eRequestState = NETCONN_NP_REQUEST_STATE_VALID;
        }
        else
        {
            NetPrintf(("netconnps5: communication restriction check failed for user index %d with error code %d\n", iUserIndex, iSceError));
            pRef->aUsers[iUserIndex].CommRestriction.ReqData.eRequestState = NETCONN_NP_REQUEST_STATE_FAILED;
        }
    }

    NetCritLeave(&pRef->pLocalUser->crit);
}
/*F********************************************************************************/
/*!
    \Function   _NetConnCommRestrictionRequest

    \Description
        Perform the WebApi call to the Communication Restriction status

    \Input *pRef            - NetConnRefT reference
    \Input iLocalUserIndex  - local user index
    \Input *pRequestData    - the generic async request data

    \Output
        int32_t             - SCE error code

    \Version 11/14/2019 (tcho)
*/
/********************************************************************************F*/
static int32_t _NetConnCommRestrictionRequest(NetConnRefT *pRef, int32_t iLocalUserIndex, NetConnAsyncRequestDataT *pRequestData)
{
    char Request[256];
    SceNpAccountId AccountId;
    int64_t iWebRequestId;
    int32_t iResult = -1;
    
    if (pRequestData->eRequestState != NETCONN_NP_REQUEST_STATE_INPROG)
    {
        NetConnStatus('acct', iLocalUserIndex, &AccountId, sizeof(AccountId));
        ds_snzprintf(Request, sizeof(Request), NETCONNPS4_PATH_COMMRESTRICTION, AccountId);
        iWebRequestId = DirtyWebApiRequest(pRef->pWebApi, iLocalUserIndex, NETCONNPS4_API_GROUP_COMMRESTRICTION, SCE_NP_WEBAPI2_HTTP_METHOD_GET, Request, NULL, 0, NULL, &_NetConnCommRestrictionRequestCallback, pRef);

        if (iWebRequestId < 0)
        {
            pRequestData->eRequestState = NETCONN_NP_REQUEST_STATE_FAILED;
            iResult = (int32_t)iWebRequestId;
        }
        else
        {
            pRequestData->eRequestState = NETCONN_NP_REQUEST_STATE_INPROG;
            iResult = 0;

        }
    }

    return ((int32_t)iResult);
}

/*F********************************************************************************/
/*!
    \Function   _NetConnReachabilityRequest

    \Description
        Callback for generic async request.  Initiates a request to get the reachability info.

    \Input *pRef            - NetConnRefT reference
    \Input iLocalUserIndex  - local user index
    \Input userId           - the SceUserServiceUserId of the user making the request
    \Input *pRequestData    - the generic async request data

    \Output
        int32_t             - SCE error code

    \Version 06/20/2013 (mcorcoran)
*/
/********************************************************************************F*/
static int32_t _NetConnReachabilityRequest(NetConnRefT *pRef, int32_t iLocalUserIndex, SceUserServiceUserId userId, NetConnAsyncRequestDataT *pRequestData)
{
    return(sceNpCheckNpReachability(pRequestData->iRequestId, userId));
}

/*F********************************************************************************/
/*!
    \Function   _NetConnPremiumRequest

    \Description
        Callback for generic async request.  Initiates a request to check premium .

    \Input *pRef            - NetConnRefT reference
    \Input iLocalUserIndex  - local user index
    \Input userId           - the SceUserServiceUserId of the user making the request
    \Input *pRequestData    - the generic async request data

    \Output
        int32_t             - SCE error code

    \Version 11/11/2019 (tcho)
*/
/********************************************************************************F*/
static int32_t _NetConnPremiumRequest(NetConnRefT *pRef, int32_t iLocalUserIndex, SceUserServiceUserId userId, NetConnAsyncRequestDataT *pRequestData)
{
    return sceNpCheckPremium(pRequestData->iRequestId, &pRef->aUsers[iLocalUserIndex].Premium.NpCheckPremiumParam, &pRef->aUsers[iLocalUserIndex].Premium.NpCheckPremiumResult);
}

/*F********************************************************************************/
/*!
    \Function   _NetConnPremiumResponse

    \Description
        Callback for reading the async response for the premium membership check.

    \Input *pRef            - NetConnRefT reference
    \Input iLocalUserIndex  - local user index
    \Input *pBuf            - pointer to a SceNpCheckPremiumResult struct
    \Input iBufSize         - size of the buffer pointed to by pBuf

    \Output
        int32_t             - number of bytes copied into pBuf, negative=error

    \Version 06/20/2013 (mcorcoran)
*/
/********************************************************************************F*/
static int32_t _NetConnPremiumResponse(NetConnRefT *pRef, int32_t iLocalUserIndex, void *pBuf, int32_t iBufSize)
{
    //be sure we have enough buffer space
    if ((pBuf != NULL) && (iBufSize >= (int32_t)sizeof(SceNpCheckPremiumResult)))
    {
        ds_memclr(pBuf, iBufSize);
        ds_memcpy(pBuf, &pRef->aUsers[iLocalUserIndex].Premium.NpCheckPremiumResult, sizeof(SceNpCheckPremiumResult));
        return(sizeof(SceNpCheckPremiumResult));
    }
    else
    {
        NetPrintf(("netconnps5: _NetConnPremiumResponse() called with invalid buffer space.\n"));
        return(-1);
    }
}

/*F********************************************************************************/
/*!
    \Function   _NetConnUpdateLocalUserNpState

    \Description
        Update NP state of specified local user.

    \Input *pRef           - NetConnRefT reference
    \Input iLocalUserIndex - local user index

    \Version 18/01/2014 (mclouatre)
*/
/********************************************************************************F*/
static void _NetConnUpdateLocalUserNpState(NetConnRefT *pRef, int32_t iLocalUserIndex)
{
    int32_t iResult;

    if (pRef->bNpInit)
    {
        SceNpState sceOldNpState = pRef->aUsers[iLocalUserIndex].NpState;
        if ((iResult = _NetConnSaveErr(pRef, sceNpGetState(pRef->aUsers[iLocalUserIndex].UserServiceUserId, &pRef->aUsers[iLocalUserIndex].NpState))) == SCE_OK)
        {
            if (sceOldNpState != pRef->aUsers[iLocalUserIndex].NpState)
            {
                NetPrintf(("netconnps5: NP state of local user 0x%08x at index %d changed from %s to %s\n",
                    pRef->aUsers[iLocalUserIndex].UserServiceUserId, iLocalUserIndex, _NpState[sceOldNpState], _NpState[pRef->aUsers[iLocalUserIndex].NpState]));
            }

            if (pRef->aUsers[iLocalUserIndex].NpState == SCE_NP_STATE_SIGNED_IN)
            {
                if ((iResult = _NetConnSaveErr(pRef, sceNpGetOnlineId(pRef->aUsers[iLocalUserIndex].UserServiceUserId, &pRef->aUsers[iLocalUserIndex].NpOnlineId))) != SCE_OK)
                {
                    NetPrintf(("netconnps5: sceNpGetNpId(0x%08x) failed (idx=%d); err=%s\n", pRef->aUsers[iLocalUserIndex].UserServiceUserId, iLocalUserIndex, DirtyErrGetName(iResult)));
                }

                // refresh the rechability
                if (pRef->aUsers[iLocalUserIndex].Reachability.ReqData.eRequestState == NETCONN_NP_REQUEST_STATE_INVALID)
                {
                    if ((iResult = _NetConnNpAsyncRequest(pRef, iLocalUserIndex, (NetConnAsyncRequestDataT*)&pRef->aUsers[iLocalUserIndex].Reachability, _NetConnReachabilityRequest)) < 0)
                    {
                        NetPrintf(("netconnps5: unable to query PSN Reachability for user (0x%08x, %d) failed; err=%s\n", pRef->aUsers[iLocalUserIndex].UserServiceUserId, iLocalUserIndex, DirtyErrGetName(iResult)));
                    }
                }

                // refresh the user's communication restriction status (WebApi request)
                if (pRef->aUsers[iLocalUserIndex].CommRestriction.ReqData.eRequestState == NETCONN_NP_REQUEST_STATE_INVALID)
                {
                    if ((iResult = _NetConnCommRestrictionRequest(pRef, iLocalUserIndex, (NetConnAsyncRequestDataT*)&pRef->aUsers[iLocalUserIndex].CommRestriction)) < 0)
                    {
                        NetPrintf(("netconnps5: uable to query for communication restriction stauts for user (0x%80x, %d) failed; err=%s\n", pRef->aUsers[iLocalUserIndex].UserServiceUserId, iLocalUserIndex, DirtyErrGetName(iResult)));
                    }
                }
            }
        }
        else
        {
            NetPrintf(("netconnps5: sceNpGetState(0x%08x) failed (idx=%d); err=%s\n", pRef->aUsers[iLocalUserIndex].UserServiceUserId, iLocalUserIndex, DirtyErrGetName(iResult)));
        }
    }
    else
    {
        NetPrintf(("netconnps5: _NetConnUpdateLocalUserNpState() ignored because NP not yet initialized\n"));
    }
}

/*F********************************************************************************/
/*!
    \Function   _NetConnAddLocalUser

    \Description
        Add a local user

    \Input *pLocalUserRef   - local user module reference
    \Input iLocalUserIndex  - local user index
    \Input *pIEAUser        - pointer to IEAUser object

    \Version 05/16/2014 (mclouatre)
*/
/********************************************************************************F*/
static void _NetConnAddLocalUser(NetConnLocalUserRefT *pLocalUserRef, int32_t iLocalUserIndex, const EA::User::IEAUser *pIEAUser)
{
    NetConnRefT *pRef = (NetConnRefT *)pLocalUserRef->pNetConn;
    const EA::User::SonyUser *pKettleUser = static_cast<const EA::User::SonyUser *>(pIEAUser);

    if (pRef->aEAUsers[iLocalUserIndex] == NULL)
    {
        pRef->aEAUsers[iLocalUserIndex] = pKettleUser;
        pRef->aEAUsers[iLocalUserIndex]->AddRef();

        if (pRef->aUsers[iLocalUserIndex].UserServiceUserId == SCE_USER_SERVICE_USER_ID_INVALID)
        {
            // obtain the local user identifier from the EAUser
            pRef->aUsers[iLocalUserIndex].UserServiceUserId = (SceUserServiceUserId)pKettleUser->GetUserID();

            NetPrintf(("netconnps5: netconn user added at local user index %d --> local user id: 0x%08x\n",
                iLocalUserIndex, pRef->aUsers[iLocalUserIndex].UserServiceUserId));

            // update NP state of this user
            _NetConnUpdateLocalUserNpState(pRef, iLocalUserIndex);
        }
        else
        {
            NetPrintf(("netconnps5: failed to add EAUser at index %d with local user id 0x%08x because aUserServiceUserId is out of sync.\n",
                iLocalUserIndex, (SceUserServiceUserId)pKettleUser->GetUserID()));
        }
    }
    else
    {
        NetPrintf(("netconnps5: failed to add EAUser at index %d with local user id 0x%08x because entry already used with local user id 0x%08x\n",
            iLocalUserIndex, (SceUserServiceUserId)pKettleUser->GetUserID(), (SceUserServiceUserId)pRef->aEAUsers[iLocalUserIndex]->GetUserID()));
    }
}

/*F********************************************************************************/
/*!
    \Function   _NetConnQueryIEAUser

    \Description
        Finds the local user index for the passed in IEAUser UserId

    \Input *pRef    - module reference
    \Input id       - the userid used for the lookup

    \Output
        int32_t - index of user, negative=not found
*/
/********************************************************************************F*/
static int32_t _NetConnQueryIEAUser(NetConnRefT *pRef, SceUserServiceUserId id)
{
    int32_t iResult = -1;
    int32_t i = 0;

    for (; i < NETCONN_MAXLOCALUSERS; i++)
    {
        if (pRef->aEAUsers[i] != NULL &&
            (SceUserServiceUserId)pRef->aEAUsers[i]->GetUserID() == id)
        {
            iResult = i;
            break;
        }
    }

    return(iResult);
}

/*F********************************************************************************/
/*!
    \Function   _NetConnRemoveLocalUser

    \Description
        Remove a local user

    \Input *pLocalUserRef   - local user module reference
    \Input iLocalUserIndex  - local user index
    \Input *pIEAUser        - pointer to IEAUser object

    \Version 05/16/2014 (mclouatre)
*/
/********************************************************************************F*/
static void _NetConnRemoveLocalUser(NetConnLocalUserRefT *pLocalUserRef, int32_t iLocalUserIndex, const EA::User::IEAUser *pIEAUser)
{
    NetConnRefT *pRef = (NetConnRefT *)pLocalUserRef->pNetConn;

    if (pRef->aEAUsers[iLocalUserIndex] != NULL)
    {
        if (pIEAUser->GetUserID() == pRef->aEAUsers[iLocalUserIndex]->GetUserID())
        {
            if (pRef->aUsers[iLocalUserIndex].UserServiceUserId != SCE_USER_SERVICE_USER_ID_INVALID)
            {
                NetPrintf(("netconnps5: netconn user removed at local user index %d --> local user id: 0x%08x\n",
                    iLocalUserIndex, pRef->aUsers[iLocalUserIndex].UserServiceUserId));

                pRef->aUsers[iLocalUserIndex].UserServiceUserId = SCE_USER_SERVICE_USER_ID_INVALID;

                ds_memclr(&pRef->aUsers[iLocalUserIndex].NpOnlineId, sizeof(pRef->aUsers[iLocalUserIndex].NpOnlineId));
                pRef->aUsers[iLocalUserIndex].NpState = SCE_NP_STATE_UNKNOWN;
                pRef->aUsers[iLocalUserIndex].NpReachState = SCE_NP_REACHABILITY_STATE_UNAVAILABLE;

                _NetConnCleanupAuthCodeRequest(pRef, iLocalUserIndex);
                pRef->aUsers[iLocalUserIndex].AuthCode.eAuthCodeState = NETCONN_NP_REQUEST_STATE_INVALID;

                _NetConnCleanupNpRequest(pRef, iLocalUserIndex, (NetConnAsyncRequestDataT*)&pRef->aUsers[iLocalUserIndex].Premium);
                pRef->aUsers[iLocalUserIndex].Premium.ReqData.eRequestState = NETCONN_NP_REQUEST_STATE_INVALID;

                _NetConnCleanupNpRequest(pRef, iLocalUserIndex, (NetConnAsyncRequestDataT*)&pRef->aUsers[iLocalUserIndex].Reachability);
                pRef->aUsers[iLocalUserIndex].Reachability.ReqData.eRequestState = NETCONN_NP_REQUEST_STATE_INVALID;

                DirtyWebApiAbortRequests(pRef->pWebApi, iLocalUserIndex);
                pRef->aUsers[iLocalUserIndex].CommRestriction.ReqData.eRequestState = NETCONN_NP_REQUEST_STATE_INVALID;

                pRef->aUsers[iLocalUserIndex].OnlineIdCache.bInvalid = TRUE;
                pRef->aUsers[iLocalUserIndex].AccountIdCache.bInvalid = TRUE;

                pRef->aEAUsers[iLocalUserIndex]->Release();
                pRef->aEAUsers[iLocalUserIndex] = NULL;
            }
            else
            {
                NetPrintf(("netconnps5: failed to remove local user at index %d - invalid local user id in internal cache\n", iLocalUserIndex));
            }
        }
        else
        {
            NetPrintf(("netconnps5: failed to remove local user at index %d - local user ids do not match (0x%08x vs 0x%08x)\n",
                iLocalUserIndex, (SceUserServiceUserId)pIEAUser->GetUserID(), (SceUserServiceUserId)pRef->aEAUsers[iLocalUserIndex]->GetUserID()));
        }
    }
    else
    {
        NetPrintf(("netconnps5: failed to remove IEAUSER at index %d - no local user at that spot\n", iLocalUserIndex));
    }
}

/*F********************************************************************************/
/*!
    \Function   _NetConnGetInterfaceType

    \Description
        Get interface type and return it to caller.

    \Input *pRef    - module state

    \Output
        uint32_t    - interface type bitfield (NETCONN_IFTYPE_*)

    \Version 10/08/2009 (jbrookes)
*/
/********************************************************************************F*/
static uint32_t _NetConnGetInterfaceType(NetConnRefT *pRef)
{
    uint32_t uIfType = NETCONN_IFTYPE_ETHER;
    union SceNetCtlInfo Info;
    int32_t iResult;

    // check for pppoe
    if (((iResult = sceNetCtlGetInfo(SCE_NET_CTL_INFO_IP_CONFIG, &Info)) == 0) && (Info.ip_config == SCE_NET_CTL_IP_PPPOE))
    {
        uIfType |= NETCONN_IFTYPE_PPPOE;
    }
    else if (iResult != 0)
    {
        NetPrintf(("netconnps5: sceNetCtlGetInfo(IP_CONFIG) failed; err=%s\n", DirtyErrGetName(iResult)));
    }
    // check for wireless
    if ((iResult = sceNetCtlGetInfo(SCE_NET_CTL_INFO_DEVICE, &Info)) == 0)
    {
        switch (Info.device)
        {
            case SCE_NET_CTL_DEVICE_WIRELESS:
                uIfType |= NETCONN_IFTYPE_WIRELESS;
                NetPrintf(("netconnps5: sceNetCtlGetInfo() returned: wireless\n"));
                break;
            case SCE_NET_CTL_DEVICE_WIRED:
                NetPrintf(("netconnps5: sceNetCtlGetInfo() returned: wired\n"));
                break;
            default:
                NetPrintf(("netconnps5: warning - unsupported device type (%d) detected\n", Info.device));
                break;
        }
    }
    else
    {
        NetPrintf(("netconnps5: sceNetCtlGetInfo(DEVICE) failed; err=%s\n", DirtyErrGetName(iResult)));
    }
    return(uIfType);
}

/*F********************************************************************************/
/*!
    \Function   _NetConnQueryNpStart

    \Description
        Starts local user Np queries if not already started

    \Input *pRef    - module state

    \Version 04/22/2014 (amakoukji)
*/
/********************************************************************************F*/
static void _NetConnQueryNpUserStart(NetConnRefT *pRef)
{
    // update np state of all the users 
    int32_t iLocalUserIndex;

    for (iLocalUserIndex = 0; iLocalUserIndex < NETCONN_MAXLOCALUSERS; ++iLocalUserIndex)
    {
        if (pRef->aUsers[iLocalUserIndex].UserServiceUserId != SCE_USER_SERVICE_USER_ID_INVALID)
        {
            _NetConnUpdateLocalUserNpState(pRef, iLocalUserIndex);
        }
    }

    _NetConnUpdateConnStatus(pRef, '~rch');
}

/*F********************************************************************************/
/*!
    \Function   _NetConnQueryNpUserIdle

    \Description
        Checks the queries for Np Reachability

    \Input *pRef    - module state

    \Version 04/22/2014 (amakoukji)
*/
/********************************************************************************F*/
static void _NetConnQueryNpUserIdle(NetConnRefT *pRef)
{
    bool bOneIncomplete = false;
    int32_t iLocalUserIndex;

    for (iLocalUserIndex = 0; iLocalUserIndex < NETCONN_MAXLOCALUSERS; ++iLocalUserIndex)
    {
        if (pRef->aUsers[iLocalUserIndex].Reachability.ReqData.eRequestState == NETCONN_NP_REQUEST_STATE_INPROG)
        {
            // still running
            bOneIncomplete = true;
        }
    }

    if (!bOneIncomplete)
    {
        bool bAllFailed = true;
        // check if all the requests failed
        for (iLocalUserIndex = 0; iLocalUserIndex < NETCONN_MAXLOCALUSERS; ++iLocalUserIndex)
        {
            if ((pRef->aUsers[iLocalUserIndex].Reachability.ReqData.eRequestState == NETCONN_NP_REQUEST_STATE_VALID) && (pRef->aUsers[iLocalUserIndex].Reachability.iResult >= 0))
            {
                bAllFailed = false;
            }
        }

        if (bAllFailed)
        {
            NetPrintf(("netconnps5: none of the users passed the rechability check\n"));
            _NetConnNpCheckNpRechabilityError(pRef);
        }
        else
        {
            // progress to the next state in the login flow
            _NetConnUpdateConnStatus(pRef, '~ens');
        }
    }
}

/*F********************************************************************************/
/*!
    \Function   _NetConnEntitlementAccessInitialize

    \Description
        Initializes NpEntitlementAccess library

    \Output
        int32_t     - negative=failure, zero=success

    \Version 12/12/2019 (jbrookes)
*/
/********************************************************************************F*/
static int32_t _NetConnEntitlementAccessInitialize(void)
{
    int32_t iResult = -1;

    if (sceSysmoduleIsLoaded(SCE_SYSMODULE_NP_ENTITLEMENT_ACCESS) == SCE_SYSMODULE_LOADED)
    {
        SceNpEntitlementAccessInitParam initParam;
        SceNpEntitlementAccessBootParam bootParam;
        int32_t iReturn;

        ds_memclr(&initParam, sizeof(SceNpEntitlementAccessInitParam));
        ds_memclr(&bootParam, sizeof(SceNpEntitlementAccessBootParam));

        iReturn = sceNpEntitlementAccessInitialize(&initParam, &bootParam);
        if ((iReturn == SCE_OK) || (iReturn == SCE_NP_ENTITLEMENT_ACCESS_ERROR_BUSY)) // Successfully loaded or already loaded
        {
            iResult = 0;
        }
        else
        {
            NetPrintf(("netconnps5: sceNpEntitlementAccessInitialize() failed with error %s\n", DirtyErrGetName(iReturn)));
        }
    }
    else
    {
        NetPrintf(("netconnps5: SCE NpEntitlementAccess PRX is not loaded - skipping trial check.\n"));
    }

    return(iResult);
}

/*F********************************************************************************/
/*!
    \Function   _NetConnQueryTrial

    \Description
        Queries the trial attribute of the running title

    \Output
        int32_t - negative= if unable query attribute, 0 if not trial, 1 if trial

    \Notes
        Assumes app content has already been initialized

    \Version 02/09/2016 (cbuffett)
*/
/********************************************************************************F*/
static int32_t _NetConnQueryTrial(void)
{
    int32_t iResult = -1, iReturn;
    SceNpEntitlementAccessSkuFlag uValue;

    // get trial flag
    if ((iReturn = sceNpEntitlementAccessGetSkuFlag(&(uValue=0,uValue))) == SCE_OK)
    {
        if (uValue == SCE_NP_ENTITLEMENT_ACCESS_SKU_FLAG_TRIAL)
        {
            iResult = 1;
        }
        else if (uValue == SCE_NP_ENTITLEMENT_ACCESS_SKU_FLAG_FULL)
        {
            iResult = 0;
        }
    }
    else
    {
        NetPrintf(("netconnps5: sceNpEntitlementAccessGetSkuFlag() failed with error %s\n", DirtyErrGetName(iReturn)));
    }

    return(iResult);
}

/*F********************************************************************************/
/*!
    \Function   _NetConnQueryPlayFirstTrial

    \Description
        Queries the play-first trial attribute of the running title

    \Output
        int32_t     - negative=error, TRUE if pft, else FALSE

    \Notes
        Assumes app content has already been initialized

    \Version 04/08/2018 (jbrookes)
*/
/********************************************************************************F*/
static int32_t _NetConnQueryPlayFirstTrial(void)
{
    #if DIRTYCODE_PLAYFIRSTTRIAL
    SceNpEntitlementAccessPftFlag uPftFlag;
    int32_t iResult, iReturn;

    // get pft flag
    if ((iReturn = sceNpEntitlementAccessGetPftFlag(&(uPftFlag=SCE_NP_ENTITLEMENT_ACCESS_PFT_FLAG_OFF,uPftFlag))) == SCE_OK)
    {
        iResult = (uPftFlag == SCE_NP_ENTITLEMENT_ACCESS_PFT_FLAG_ON) ? TRUE : FALSE;
    }
    else
    {
        NetPrintf(("netconnps5: sceNpEntitlementAccessGetPftFlag() failed with error %s\n", DirtyErrGetName(iReturn)));
        iResult = -1;
    }

    return(iResult);
    #else
    return(-1);
    #endif
}

/*F********************************************************************************/
/*!
    \Function   _NetConnGetPlatformEnvironment

    \Description
        Uses auth code issuer to determine platform environment.

    \Input *pRef    - module state

    \Version 10/07/2009 (jbrookes)
*/
/********************************************************************************F*/
static void _NetConnGetPlatformEnvironment(NetConnRefT *pRef)
{
    int32_t iRet = 0;
    int32_t iLocalUserIndex;

    // set to invalid environment and pending state
    pRef->uPlatEnv = (unsigned)-1;
    
    pRef->bEnviRequest = TRUE;
    iLocalUserIndex = NetConnQuery(NULL, NULL, 0);

    if (iLocalUserIndex >= 0)
    {
        _NetConnUpdateConnStatus(pRef, '~enw');
        iRet = _NetConnRequestAuthCode(pRef, iLocalUserIndex);
        if (iRet < 0)
        {
            NetPrintf(("netconnps5:_NetConnGetPlatformEnvironment(), failed to request auth code\n"));
            pRef->bEnviRequest = FALSE;
            _NetConnUpdateConnStatus(pRef, '-act');
        }
    }
}

#if DIRTYCODE_LOGGING
/*F********************************************************************************/
/*!
    \Function   _NetConnPrintNpState

    \Description
        Util to print the np state of all players

    \Input *pRef    - module state

    \Version 12/13/2012 (cvienneau)
*/
/********************************************************************************F*/
static void _NetConnPrintNpState(NetConnRefT *pRef)
{
    int32_t iLocalUserIndex;

    for(iLocalUserIndex = 0; iLocalUserIndex < NETCONN_MAXLOCALUSERS; iLocalUserIndex++)
    {
        if (pRef->aUsers[iLocalUserIndex].NpState == SCE_NP_STATE_SIGNED_IN)
        {
            NetPrintf(("netconnps5: user #%d, %s\n", iLocalUserIndex, pRef->aUsers[iLocalUserIndex].NpOnlineId.data));
        }
        else
        {
            NetPrintf(("netconnps5: user #%d, not signed in (%s)\n", iLocalUserIndex, _NpState[pRef->aUsers[iLocalUserIndex].NpState]));
        }
    }
}
#endif

/*F********************************************************************************/
/*!
    \Function   _NetConnSceNpStateCallback

    \Description
        Callback function provided to sceNpRegisterStateCallback on notification of
        NP state changes.

    \Input sceUserServiceUserId - the user id of the player whos state changed.
    \Input state                - the new state SCE_NP_STATE_UNKNOWN, SCE_NP_STATE_SIGNED_OUT, SCE_NP_STATE_SIGNED_IN
    \Input *pUserData           - module state

    \Version 12/13/2012 (cvienneau)
*/
/********************************************************************************F*/
static void _NetConnSceNpStateCallback(SceUserServiceUserId sceUserServiceUserId, SceNpState state, void *pUserData)
{
    // this callback is hit when a user logs in or logs out from PSN,
    // but not if a controller is plugged or unplugged.

    // update the login state of the user with sceUserServiceUserId
    NetConnRefT *pRef    = (NetConnRefT*)pUserData;

    NetPrintf(("netconnps5: NP state event for 0x%08x\n", sceUserServiceUserId));

    if (pRef)
    {
        // find the index of the user
        int32_t iLocalUserIndex;
        for (iLocalUserIndex = 0; iLocalUserIndex < NETCONN_MAXLOCALUSERS; ++iLocalUserIndex)
        {
            if (pRef->aUsers[iLocalUserIndex].UserServiceUserId == sceUserServiceUserId)
            {
                break;
            }
        }

        // it is possible, in the event of a signout for the userId
        // to not be found if _NetConnOnUserEvent() already handled
        // the event
        // in that case, simply skip because the state is up-to-date
        if (iLocalUserIndex < NETCONN_MAXLOCALUSERS)
        {
            _NetConnUpdateLocalUserNpState(pRef, iLocalUserIndex);
        }

        _NetConnInvalidateUserIdCache(pRef);

        // queue the notification for callback at the next NetConnIdle()
        if (pRef->pNpStateCallback) // queue only if someone if listening
        {
            NetCritEnter(&pRef->Common.crit);
            NetConnNpStateCallbackDataT *pTmp = (NetConnNpStateCallbackDataT*)DirtyMemAlloc(sizeof(NetConnNpStateCallbackDataT), NETCONN_MEMID, pRef->Common.iMemGroup, pRef->Common.pMemGroupUserData);
            if (pTmp)
            {
                ds_memclr(pTmp, sizeof(NetConnNpStateCallbackDataT));
                pTmp->UserServiceUserId = sceUserServiceUserId;
                pTmp->NpState = state;
                NetConnNpStateCallbackDataT *pQueue = pRef->pNpStateCallbackDataQueue;

                // stick the data at the end of the queue
                if (pQueue == NULL)
                {
                    pRef->pNpStateCallbackDataQueue = pTmp;
                }
                else
                {
                    while (pQueue->pNext != NULL)
                    {
                        pQueue = pQueue->pNext;
                    }
                    pQueue->pNext = pTmp;
                }
            }
            NetCritLeave(&pRef->Common.crit);
        }
    }
}

/*F********************************************************************************/
/*!
    \Function   _NetConnSceNpReachabilityStateCallback

    \Description
        Callback function provided to sceNpRegisterNpReachabilityStateCallback on notification of
        PSN reachability changes

    \Input sceUserServiceUserId - the user id of the player whos state changed.
    \Input state                - the new SceNpReachabilityState 
    \Input *pUserData           - module state

    \Version 11/07/2019 (tcho)
*/
/********************************************************************************F*/
static void _NetConnSceNpReachabilityStateCallback(SceUserServiceUserId sceUserServiceUserId, SceNpReachabilityState state, void *pUserData)
{
    NetConnRefT *pRef = (NetConnRefT*)pUserData;

    NetPrintf(("netconnps5: NP reachability event for 0x%08x\n", sceUserServiceUserId));

    // find the user that triggered the event and save the state
    if (pRef)
    {
        int32_t iLocalUserIndex;
        for (iLocalUserIndex = 0; iLocalUserIndex < NETCONN_MAXLOCALUSERS; ++iLocalUserIndex)
        {
            if (pRef->aUsers[iLocalUserIndex].UserServiceUserId == sceUserServiceUserId)
            {
                break;
            }
        }

        if (iLocalUserIndex < NETCONN_MAXLOCALUSERS)
        {
            pRef->aUsers[iLocalUserIndex].NpReachState = state;
        }
    }
}

/*F********************************************************************************/
/*!
    \Function    _NetConnNpInit

    \Description
        Initialize Network Platform

    \Input *pRef    - netconn state

    \Output
        int32_t     - result of initialization

    \Version 12/12/2012 (cvienneau)
*/
/********************************************************************************F*/
static int32_t _NetConnNpInit(NetConnRefT *pRef)
{
    int32_t iResult, iLocalUserIndex, iCallbackId;

    for (iLocalUserIndex = 0; iLocalUserIndex < NETCONN_MAXLOCALUSERS; iLocalUserIndex++)
    {
        ds_memclr(&pRef->aUsers[iLocalUserIndex].NpOnlineId, sizeof(pRef->aUsers[iLocalUserIndex].NpOnlineId));
        ds_memclr(&pRef->aUsers[iLocalUserIndex].NpState, sizeof(pRef->aUsers[iLocalUserIndex].NpState));
    }

    if (!pRef->bSecretSet)
    {
        // If the secret is not set, check to see if we are using the default title id
        // if we are using the default title id, use the default secret as well, otherwise
        // return an error. The game must set the appropriate title secret with NetConnControl('npsc', ...)
        if (ds_strnicmp(pRef->strTitleId, NETCONNPS4_SAMPLE_TITLE_ID, SCE_NP_TITLE_ID_LEN + 1) == 0)
        {
            // We are the default titleId, use the default title secret
            ds_memclr(&pRef->NpTitleSecret, sizeof(pRef->NpTitleSecret));
            ds_memcpy(&pRef->NpTitleSecret.data, aDirtySdkSampleSecret, SCE_NP_TITLE_SECRET_SIZE);
        }
        else
        {
            NetPrintf(("netconnps5: NP Title Secret not set. Use NetConnControl('npsc', ...)\n"));
            return(-1);
        }
    }

    // Set the title id and secret
    SceNpTitleId NpTitleId;
    ds_memclr(&NpTitleId, sizeof(NpTitleId));
    ds_strnzcpy(NpTitleId.id, pRef->strTitleId, sizeof(NpTitleId.id));

    // if the bNoTitleId is specified Setting Title Id will be the customers's responsibility
    if (!pRef->bNoTitleId)
    {
        iResult = _NetConnSaveErr(pRef, sceNpSetNpTitleId(&NpTitleId, &pRef->NpTitleSecret));
        if (iResult != SCE_OK)
        {
            NetPrintf(("netconnps5: sceNpSetNpTitleId() failed; err=%s\n", DirtyErrGetName(iResult)));
            return(iResult);
        }
    }
    
    // register for state callback
    iCallbackId =  _NetConnSaveErr(pRef, sceNpRegisterStateCallbackA(_NetConnSceNpStateCallback, pRef));
    if (iCallbackId < SCE_OK)
    {
        NetPrintf(("netconnps5: sceNpRegisterStateCallback() failed; err=%s\n", DirtyErrGetName(iCallbackId)));
        return(iCallbackId);
    }
    pRef->iRegisterStateCallbackId = iCallbackId;
    
    // register for the NpReachabilityStateCallback
    if ((iResult = _NetConnSaveErr(pRef, sceNpRegisterNpReachabilityStateCallback(_NetConnSceNpReachabilityStateCallback, pRef))) < SCE_OK)
    {
        NetPrintf(("netconnps5: sceNpRegisterNpReachabilityStateCallback() faled err=%s\n", DirtyErrGetName(iResult)));
        return(iResult);
    }

    pRef->bNpInit = TRUE;

    //get the current state, since registering the callback won't tell us where we are at
    for(iLocalUserIndex = 0; iLocalUserIndex < NETCONN_MAXLOCALUSERS; iLocalUserIndex++)
    {
        if (pRef->aUsers[iLocalUserIndex].UserServiceUserId != SCE_USER_SERVICE_USER_ID_INVALID)
        {
            // update NP state of this user
            _NetConnUpdateLocalUserNpState(pRef, iLocalUserIndex);
        }
    }

    #if DIRTYCODE_LOGGING
    NetPrintf(("netconnps5: initial NP state:\n"));
    _NetConnPrintNpState(pRef);
    #endif

    return(iResult);
}

/*F********************************************************************************/
/*!
    \Function   _NetConnNpTerm

    \Description
        Shut down Network Platform

    \Input *pRef    - netconn state

    \Version 12/12/2012 (cvienneau)
*/
/********************************************************************************F*/
static void _NetConnNpTerm(NetConnRefT *pRef)
{
    int32_t iResult;

    NetPrintf(("netconnps5: shutting down Network Platform\n"));
       
    // unregister reachability callback
    if ((iResult = sceNpUnregisterNpReachabilityStateCallback()) != SCE_OK)
    {
        NetPrintf(("netconnps5: sceNpUnregisterNpReachabilityStateCallback() failed: err=%s\n", DirtyErrGetName(iResult)));
    }

    // unregister npmanager callback
    // sony notes sceNpUnregisterStateCallbackA may be slow and blocking
    if ((iResult = sceNpUnregisterStateCallbackA(pRef->iRegisterStateCallbackId)) != SCE_OK)
    {
        NetPrintf(("netconnps5: sceNpUnregisterStateCallbackA() failed: err=%s\n", DirtyErrGetName(iResult)));
    }

    pRef->iRegisterStateCallbackId = 0;
    pRef->bNpInit = FALSE;
}

/*F********************************************************************************/
/*!
    \Function _NetConnConnect

    \Description
        Start connection process

    \Input *pRef    - netconn module state

    \Output
        int32_t     - 0 for success, negative for failure

    \Version 5/25/2013 (cvienneau)
*/
/********************************************************************************F*/
static int32_t _NetConnConnect(NetConnRefT *pRef)
{
    int32_t iResult = 0;

    // start up network interface
    if ((iResult = SocketControl(NULL, 'conn', 0, NULL, NULL)) == 0)
    {
        // initialize Network Platform
        if ((iResult = _NetConnNpInit(pRef)) != 0)
        {
            NetPrintf(("netconnps5: failed to initialize network platform\n"));
            SocketControl(NULL, 'disc', 0, NULL, NULL);
            _NetConnUpdateConnStatus(pRef, '-sce');
            return(-1);
        }

        pRef->eState = NetConnRefT::ST_CONN;
        _NetConnUpdateConnStatus(pRef, '~con');
    }
    else
    {
        NetPrintf(("netconnps5: failed to initialize network stack\n"));
        _NetConnUpdateConnStatus(pRef, '-skt');
        return(-2);
    }
    return(iResult);
}

/*F********************************************************************************/
/*!
    \Function   _NetConnUpdate

    \Description
        Update status of NetConn module.  This function is called by NetConnIdle.

    \Input *pData   - pointer to NetConn module ref
    \Input uTick    - current tick counter

    \Version 07/18/2006 (jbrookes)
*/
/********************************************************************************F*/
static void _NetConnUpdate(void *pData, uint32_t uTick)
{
    NetConnRefT *pRef = (NetConnRefT *)pData;
    int32_t iResult;
    int32_t iExternalCleanupListCount;

    // perform idle processing
    SocketControl(NULL, 'idle', uTick, NULL, NULL);

    // update local users list
    NetConnLocalUserUpdate(pRef->pLocalUser);

    // process the cleanup list
    iExternalCleanupListCount = NetConnCommonProcessExternalCleanupList(&pRef->Common);

    // waiting for external cleanup list to be empty
    if (pRef->eState == NetConnRefT::ST_EXT_CLEANUP)
    {
        if (iExternalCleanupListCount == 0)
        {
            // it is only when the external clean up list has been fully cleaned up that sceNpTerm() can be called
            _NetConnNpTerm(pRef);

            pRef->eState = NetConnRefT::ST_INIT;

            if (pRef->bConnectDelayed)
            {
                pRef->bConnectDelayed = FALSE;

                if (_NetConnConnect(pRef) < 0)
                {
                    // assumption: _NetConnConnect() has updated pRef->uConnStatus with appropriate error state
                    NetPrintf(("netconnps5: critical error - can't complete connection setup\n"));
                }
            }
        }
    }

    // wait for network active status
    if (pRef->eState == NetConnRefT::ST_CONN)
    {
        uint32_t uSocketStatus;
        uSocketStatus = SocketInfo(NULL, 'conn', 0, NULL, 0);
        if (uSocketStatus == '+onl')
        {
            if (pRef->uConnStatus == '~con')
            {
                _NetConnQueryNpUserStart(pRef);
            }
            else if (pRef->uConnStatus == '~rch')
            {
                _NetConnUpdateNpRequests(pRef, NETCONN_NP_REQUEST_TYPE_REACHABILITY);
                _NetConnQueryNpUserIdle(pRef);
            }
            else if (pRef->uConnStatus == '~ens')
            {
                _NetConnGetPlatformEnvironment(pRef);
            }
            else if (pRef->uConnStatus == '~enw')
            {
                //Update environment request which is part of an auth code request
                _NetConnUpdateAuthCodeRequests(pRef);

                if (pRef->uPlatEnv != (unsigned)(-1))
                {
                    _NetConnUpdateConnStatus(pRef, '+onl');
                }
            }

            if (pRef->uConnStatus == '+onl')
            {
                // discover upnp router information
                if (pRef->pProtoUpnp != NULL)
                {
                    if (pRef->iPeerPort != 0)
                    {
                        ProtoUpnpControl(pRef->pProtoUpnp, 'port', pRef->iPeerPort, 0, NULL);
                        ProtoUpnpControl(pRef->pProtoUpnp, 'macr', 'upnp', 0, NULL);
                    }
                    else
                    {
                        ProtoUpnpControl(pRef->pProtoUpnp, 'macr', 'dscg', 0, NULL);
                    }
                }

                pRef->eState = NetConnRefT::ST_IDLE;
            }
        }
        else if ((uSocketStatus >> 24) == '-')
        {
            _NetConnUpdateConnStatus(pRef, uSocketStatus);
        }
    }

    // update connection status while idle
    if (pRef->eState == NetConnRefT::ST_IDLE)
    {
        if (pRef->uConnStatus == '+onl')
        {
            // update any auth code requests
            _NetConnUpdateAuthCodeRequests(pRef);

            // update any Premium requests
            _NetConnUpdateNpRequests(pRef, NETCONN_NP_REQUEST_TYPE_PREMIUM);

        }

        // update connection status if not already in an error state
        if ((pRef->uConnStatus >> 24) != '-')
        {
            uint32_t uSocketConnStat = SocketInfo(NULL, 'conn', 0, NULL, 0);
            
            if (pRef->uConnStatus != uSocketConnStat)
            {
                _NetConnUpdateConnStatus(pRef, uSocketConnStat);
            }
        }
    }

    // if error status, go to idle state from any other state
    if ((pRef->eState != NetConnRefT::ST_IDLE) && (pRef->uConnStatus >> 24 == '-'))
    {
        pRef->eState = NetConnRefT::ST_IDLE;
    }

    if (pRef->bNpInit)
    {
        if ((iResult = sceNpCheckCallback()) != SCE_OK)
        {
            NetPrintf(("netconnps5: sceNpCheckCallback() failed; err=%s\n", DirtyErrGetName(iResult)));
        }
    }

    //do we still have users logged in?
    iResult = NetConnQuery(NULL, NULL, 0);
    if (iResult < 0)
    {
        if (pRef->uConnStatus >> 24 == '+')
        {
            NetPrintf(("netconnps5: no user is signed in\n"));
            _NetConnUpdateConnStatus(pRef, '-act');
        }
    }

    // surface the NpStateCallbacks
    if (pRef->pNpStateCallback != NULL)
    {
        if (pRef->pNpStateCallbackDataQueue != NULL) // don't get the crit if we dont have to
        {
            NetCritEnter(&pRef->Common.crit);
            NetConnNpStateCallbackDataT *pTmp = pRef->pNpStateCallbackDataQueue;
            while (pTmp != NULL)
            {
                NetConnNpStateCallbackDataT *pNext = pTmp->pNext;

                // surface the callback
                pRef->pNpStateCallback(pTmp->UserServiceUserId, pTmp->NpState, pRef->pNpStateCallbackUserData);

                // free the memory
                DirtyMemFree(pTmp, NETCONN_MEMID, pRef->Common.iMemGroup, pRef->Common.pMemGroupUserData);

                // move to the next item
                pTmp = pNext;
            }

            //clear the list 
            pRef->pNpStateCallbackDataQueue = NULL;
            NetCritLeave(&pRef->Common.crit);
        }
    }
}

/*F********************************************************************************/
/*!
    \Function    _NetConnShutdownInternal

    \Description
        Shutdown the network code and return to idle state for internal use

    \Input  pRef            - netconn ref
    \Input  uShutdownFlags  - shutdown configuration flags

    \Output
        int32_t             - negative=error, else zero

    \Version 1/13/2020 (tcho)
*/
/********************************************************************************F*/
int32_t _NetConnShutdownInternal(NetConnRefT *pRef, uint32_t uShutdownFlags)
{
    int32_t iLocalUserIndex, iResult = 0;
    int32_t iExternalCleanupListCnt;

    // decrement and check the refcount
    if ((iResult = NetConnCommonCheckRef((NetConnCommonRefT*)pRef)) < 0)
    {
        return(iResult);
    }

    if (_NetConn_pRef != NULL)
    {
        // disconnect network interfaces only when netconn have been sucessfully created before hand
        NetConnDisconnect();
    }
    else
    {
        // NP platform will need to be terminated only when we did not disconnect
        _NetConnNpTerm(pRef);
    }

    // make sure the external cleanup list is empty before proceeding
    if ((iExternalCleanupListCnt = NetConnCommonProcessExternalCleanupList(&pRef->Common)) != 0)
    {
#if DIRTYCODE_LOGGING
        static uint32_t uLastTimeExtCleanup = 0;
        if (!uLastTimeExtCleanup || NetTickDiff(NetTick(), uLastTimeExtCleanup) > 1000)
        {
            NetPrintf(("netconnps5: deferring shutdown while external cleanup list is not empty (%d items)\n", iExternalCleanupListCnt));
            uLastTimeExtCleanup = NetTick();
        }
#endif

        // signal "try again"
        NetPrintf(("netconnps5: deferring shutdown while external cleanup list is not empty (%d items)\n", iExternalCleanupListCnt));
        return(NETCONN_ERROR_ISACTIVE);
    }

    //cleanup any auth code requests that might be in progress and push context id
    for (iLocalUserIndex = 0; iLocalUserIndex < NETCONN_MAXLOCALUSERS; iLocalUserIndex++)
    {
        // delete push context id 
        if (pRef->aUsers[iLocalUserIndex].isPushContextIdValid == TRUE)
        {
            NetConnStatus('pscd', iLocalUserIndex, NULL, 0);
        }

        DirtyWebApiAbortRequests(pRef->pWebApi, iLocalUserIndex);
        _NetConnCleanupAuthCodeRequest(pRef, iLocalUserIndex);
        _NetConnCleanupNpRequest(pRef, iLocalUserIndex, (NetConnAsyncRequestDataT*)&pRef->aUsers[iLocalUserIndex].Premium);
        _NetConnCleanupNpRequest(pRef, iLocalUserIndex, (NetConnAsyncRequestDataT*)&pRef->aUsers[iLocalUserIndex].Reachability);
    }

    // destroy dirtywebapi
    if (pRef->pWebApi != NULL)
    {
        DirtyWebApiDestroy(pRef->pWebApi);
        pRef->pWebApi = NULL;
    }

    // destroy the upnp ref
    if (pRef->pProtoUpnp != NULL)
    {
        ProtoUpnpDestroy(pRef->pProtoUpnp);
        pRef->pProtoUpnp = NULL;
    }

    // shut down protossl
    ProtoSSLShutdown();

    // destroy the dirtycert module
    DirtyCertDestroy();

    // remove netconn idle task
    NetConnIdleDel(_NetConnUpdate, pRef);

    // shut down Idle handler
    NetConnIdleShutdown();

    // free the pending NpStateCallbacks
    pRef->pNpStateCallback = NULL;
    if (pRef->pNpStateCallbackDataQueue != NULL)
    {
        NetCritEnter(&pRef->Common.crit);
        NetConnNpStateCallbackDataT *pTmp = pRef->pNpStateCallbackDataQueue;
        while (pTmp != NULL)
        {
            NetConnNpStateCallbackDataT *pNext = pTmp->pNext;

            // free the memory
            DirtyMemFree(pTmp, NETCONN_MEMID, pRef->Common.iMemGroup, pRef->Common.pMemGroupUserData);

            // move to the next item
            pTmp = pNext;
        }

        //clear the list 
        pRef->pNpStateCallbackDataQueue = NULL;
        NetCritLeave(&pRef->Common.crit);
    }

    // free the local users
    if (pRef->pLocalUser != NULL)
    {
        NetConnLocalUserDestroy(pRef->pLocalUser);
    }

    // shutdown the network code
    SocketDestroy(0);

    // shutdown DirtyHttpContextManager
    DirtyContextManagerShutdown();

    for (iLocalUserIndex = 0; iLocalUserIndex < NETCONN_MAXLOCALUSERS; iLocalUserIndex++)
    {
        // release EAUser if we still have a reference on it.
        if (pRef->aEAUsers[iLocalUserIndex])
        {
            pRef->aEAUsers[iLocalUserIndex]->Release();
            pRef->aEAUsers[iLocalUserIndex] = NULL;
        }
    }

    // common shutdown (must come last as this frees the memory)
    NetConnCommonShutdown(&pRef->Common);
    _NetConn_pRef = NULL;

    return(0);
}

/*** Public functions *************************************************************/

/*F********************************************************************************/
/*!
    \Function    NetConnStartup

    \Description
        Bring the network connection module to life. Creates connection with IOP
        resources and gets things ready to go. Puts all device drivers into "probe"
        mode so they look for appropriate hardware. Does not actually start any
        network activity.

    \Input *pParams - startup parameters

    \Output
        int32_t     - zero=success, negative=failure

    \Notes
        NetConnRefT::iRefCount serves as a counter for the number of times
        NetConnStartup has been called. This allows us to track how many modules
        are using it and how many times we expect NetConnShutdown to the called.
        In the past we only allowed a single call to NetConnStartup but some
        libraries may need to networking without a guarentee that the game has
        already started it.

        pParams can contain the following terms:

        \verbatim
            -noupnp                     - disable upnp support
            -nptoolkit                  - enable nptoolkit-compliant mode, will override setting fromn NetConnStartup
            -titleid=titleidstr         - specify title id (default "NPXX51027_00"), will override setting fromn NetConnStartup
            -userserviceprio=priostr    - specify user service priority (default "SCE_KERNEL_PRIO_FIFO_DEFAULT")
            -affinity=<mask as hex>     - specify the cpu affinity used for our internal threads
            -checktick                  - cause the dirtyeventmanager initialization to fail if EASEMD has ticked
        \endverbatim

    \Version 10/04/2005 (jbrookes)
*/
/********************************************************************************F*/
int32_t NetConnStartup(const char *pParams)
{
    NetConnRefT *pRef = _NetConn_pRef;
    int32_t iLocalUserIndex, iResult = 0;
    char strThreadCpuAffinity[16];
    DirtyWebApi2CreateParamsT WebApiCreateParams;

    // allow NULL params
    if (pParams == NULL)
    {
        pParams = "";
    }

    // debug display of input params
    NetPrintf(("netconnps5: startup params='%s'\n", pParams));

    // common startup
    // pRef shall hold the address of the NetConnRefT after completion if no error occured
    iResult = NetConnCommonStartup(sizeof(*pRef), pParams, (NetConnCommonRefT**)(&pRef));

    // check the result of the common startup, if already started simply early out after titleid check
    if (iResult == NETCONN_ERROR_ALREADY_STARTED)
    {
        if ((strstr(pParams, "-titleid=") != NULL) && (ds_stricmp(pRef->strTitleId, NETCONNPS4_SAMPLE_TITLE_ID) == 0))
        {
            // update titleid to param for cases where bugsentry starts netconn before game code
            NetConnCopyParam(pRef->strTitleId, sizeof(pRef->strTitleId), "-titleid=", pParams, NETCONNPS4_SAMPLE_TITLE_ID);
            NetPrintf(("netconnps5: Module already started, titleId overwritten from %s to %s\n", NETCONNPS4_SAMPLE_TITLE_ID, pRef->strTitleId));
            _NetConn_pRef = pRef;
        }
        return(0);
    }
    // otherwise, if an error occured report it
    else if (iResult < 0)
    {
        return(iResult);
    }

    // local user startup
    if ((pRef->pLocalUser = NetConnLocalUserInit(pRef, _NetConnAddLocalUser, _NetConnRemoveLocalUser)) == NULL)
    {
        return(NETCONN_ERROR_NO_MEMORY);
    }

    // do not set title id and secret is notitleid is specified
    pRef->bNoTitleId = strstr(pParams, "-notitleid") != NULL;

    // initialize DirtyHttpContextManager for creating http contexts
    if (DirtyContextManagerInit() < 0)
    {
        NetPrintf(("netconnps5: could not initialize DirtyContextManager\n"));
        return(NETCONN_ERROR_PLATFORM_SPECIFIC);
    }

    pRef->eState = NetConnRefT::ST_INIT;
    pRef->iPeerPort = NETCONN_DEFAULT_UPNP_PORT;
    pRef->bEnviRequest = FALSE;
    pRef->bSecretSet = FALSE;
    pRef->CpuAffinity = SCE_KERNEL_CPUMASK_USER_ALL;
    pRef->uPlatEnv = (unsigned)(-1);
    pRef->iLocale = 0;
    _NetConnInvalidateUserIdCache(pRef);

    // set hardcoded client id value
    ds_strnzcpy(pRef->NpClientId.id, NETCONNPS4_CLIENT_ID, sizeof(pRef->NpClientId.id));

    // get the cpu affinity string from our startup params, defaulting to 0x0
    ds_memclr(strThreadCpuAffinity, sizeof(strThreadCpuAffinity));
    NetConnCopyParam(strThreadCpuAffinity, sizeof(strThreadCpuAffinity), "-affinity=", pParams, "0x0");
    pRef->iThreadCpuAffinity = strtol(strThreadCpuAffinity, NULL, 16);

    /* create network instance
       the priority calculation is a bit odd but gets us at the highest priority
       due to the EAThread implementation on PS4 */
    if (SocketCreate(SCE_KERNEL_PRIO_FIFO_DEFAULT - SCE_KERNEL_PRIO_FIFO_HIGHEST, 0, pRef->iThreadCpuAffinity) != 0)
    {
        _NetConnShutdownInternal(pRef, 0);
        NetPrintf(("netconnps5: unable to start up dirtysock\n"));
        return(NETCONN_ERROR_SOCKET_CREATE);
    }

    // create and configure dirtycert
    if (NetConnDirtyCertCreate(pParams))
    {
        _NetConnShutdownInternal(pRef, 0);
        NetPrintf(("netconnps5: unable to create dirtycert\n"));
        return(NETCONN_ERROR_DIRTYCERT_CREATE);
    }

    // create dirtywebapi
    ds_memclr(&WebApiCreateParams, sizeof(DirtyWebApi2CreateParamsT));
    WebApiCreateParams.uThreadCpuAffinity = pRef->iThreadCpuAffinity;

    if ((pRef->pWebApi = DirtyWebApiCreate(&WebApiCreateParams)) == NULL)
    {
        _NetConnShutdownInternal(pRef, 0);
        NetPrintf(("netconnps5: counld not initialize DirtyWebApi\n"));
        return(NETCONN_ERROR_PLATFORM_SPECIFIC);

    }

    // turning on qcal
    DirtyWebApiControl(pRef->pWebApi, 'qcal', TRUE, 0, NULL);
    
    // start up protossl
    if (ProtoSSLStartup() < 0)
    {
        _NetConnShutdownInternal(pRef, 0);
        NetPrintf(("netconnps5: unable to start up protossl\n"));
        return(NETCONN_ERROR_PROTOSSL_CREATE);
    }

    // create the upnp module
    if (!strstr(pParams, "-noupnp"))
    {
        pRef->pProtoUpnp = ProtoUpnpCreate();
        if (pRef->pProtoUpnp == NULL)
        {
            _NetConnShutdownInternal(pRef, 0);
            NetPrintf(("netconnps5: unable to start up protoupnp\n"));
            return(NETCONN_ERROR_PROTOUPNP_CREATE);
        }
    }

    NetConnCopyParam(pRef->strTitleId, sizeof(pRef->strTitleId), "-titleid=", pParams, NETCONNPS4_SAMPLE_TITLE_ID);
    NetPrintf(("netconnps5: setting titleid=%s\n", pRef->strTitleId));

    for (iLocalUserIndex = 0; iLocalUserIndex < NETCONN_MAXLOCALUSERS; ++iLocalUserIndex)
    {
        pRef->aUsers[iLocalUserIndex].UserServiceUserId = SCE_USER_SERVICE_USER_ID_INVALID;
    }

    // add netconn task handle
    if (NetConnIdleAdd(_NetConnUpdate, pRef) < 0)
    {
        _NetConnShutdownInternal(pRef, 0);
        NetPrintf(("netconnps5: unable to add netconn task handler\n"));
        return(NETCONN_ERROR_INTERNAL);
    }

    // save ref
    _NetConn_pRef = pRef;

    return(0);
}

/*F********************************************************************************/
/*!
    \Function   NetConnQuery

    \Description
        Returns the index of the given gamertag. If the gamertag is not
        present, find the index of the SceUserServiceUserId in the pList.
        Otherwise return the index of the first logged in user.

    \Input *pGamertag   - gamer tag
    \Input *pList       - SceUserServiceUserId to get index of
    \Input iListSize    - when pGamerTag

    \Output
        int32_t         - negative=gamertag not logged in, else index of user

    \Version 02/19/2013 (amakoukji)
*/
/********************************************************************************F*/
int32_t NetConnQuery(const char *pGamertag, NetConfigRecT *pList, int32_t iListSize)
{
    NetConnRefT *pRef = _NetConn_pRef;
    int32_t iLocalUserIndex;

    if (pGamertag != NULL)
    {
        // find the matching gamertag
        for (iLocalUserIndex = 0; iLocalUserIndex < NETCONN_MAXLOCALUSERS; ++iLocalUserIndex)
        {
            if ((pRef->aUsers[iLocalUserIndex].NpState == SCE_NP_STATE_SIGNED_IN) && (pRef->aUsers[iLocalUserIndex].NpReachState == SCE_NP_REACHABILITY_STATE_REACHABLE))
            {
                // special handling since SceNpOnlineId's .data field is not always terminated within its char array, but instead in the next .term character field
                if (ds_strnicmp(pRef->aUsers[iLocalUserIndex].NpOnlineId.data, pGamertag, SCE_NP_ONLINEID_MAX_LENGTH + 1) == 0)
                {
                    // we've found the right user, get their index
                    return(iLocalUserIndex);
                }
            }
        }
    }
    else if (pList != NULL)
    {
        // find user based on SceUserServiceUserId
        iLocalUserIndex = NetConnStatus('indx', *(SceUserServiceUserId*)pList, NULL, 0);
        if ((pRef->aUsers[iLocalUserIndex].NpState == SCE_NP_STATE_SIGNED_IN) && (pRef->aUsers[iLocalUserIndex].NpReachState == SCE_NP_REACHABILITY_STATE_REACHABLE))
        {
            return(iLocalUserIndex);
        }
    }
    else
    {
        // return the index of the first online user
        for (iLocalUserIndex = 0; iLocalUserIndex < NETCONN_MAXLOCALUSERS; ++iLocalUserIndex)
        {
            if ((pRef->aUsers[iLocalUserIndex].NpState == SCE_NP_STATE_SIGNED_IN) && (pRef->aUsers[iLocalUserIndex].NpReachState == SCE_NP_REACHABILITY_STATE_REACHABLE))
            {
                // we've found the right user, get their index
                return(iLocalUserIndex);
            }
        }
    }

    return(-1);
}

/*F********************************************************************************/
/*!
    \Function    NetConnConnect

    \Description
        Used to bring the networking online with a specific configuration. Uses a
        configuration returned by NetConnQuery.

    \Input *pConfig - unused
    \Input *pOption - asciiz list of config parameters
                      "peerport=<port>"         to specify peer port to be opened by upnp.
                      "-nptoolkit=<true/false>" to enable nptoolkit-compliant mode.
                      "-titleid=<titleidstr>"   to specify title id.
    \Input iData    - platform-specific

    \Output
        int32_t     - negative=error, zero=success

    \Version 10/04/2005 (jbrookes)
*/
/********************************************************************************F*/
int32_t NetConnConnect(const NetConfigRecT *pConfig, const char *pOption, int32_t iData)
{
    int32_t iResult = 0;
    NetConnRefT *pRef = _NetConn_pRef;

    NetPrintf(("netconnps5: connecting...\n"));

#if DIRTYCODE_LOGGING
    {
        int32_t iInitialUserIndex = NetConnStatus('iind', 0, NULL, 0);
        if (iInitialUserIndex >= 0)
        {
            NetPrintf(("netconnps5: application was lauched by user at index %d\n", iInitialUserIndex));
        }
        else
        {
            NetPrintf(("netconnps5: no initial user detected - param.sfo was modified to allow the initial user to logout during your game\n"));
        }
    }
#endif

    pRef->uLastConnStatus = 0;

    // check connection options, if present
    if ((pRef->eState == NetConnRefT::ST_INIT) || (pRef->eState == NetConnRefT::ST_EXT_CLEANUP))
    {
        // check for connect options
        if (pOption != NULL)
        {
            const char *pOpt;

            // check for specification of peer port
            if ((pOpt = strstr(pOption, "peerport=")) != NULL)
            {
                pRef->iPeerPort = strtol(pOpt + 9, NULL, 10);
            }

            // if titleid is specified overwrite the value from NetConnStartup if one was present.
            if (strstr(pOption, "-titleid="))
            {
                if (ds_stricmp(pRef->strTitleId, NETCONNPS4_SAMPLE_TITLE_ID) != 0)
                {
                    NetPrintf(("netconnps5: -titleid set in NetConnStartup (%s) overwritten in NetConnConnect\n", pRef->strTitleId));
                }
                NetConnCopyParam(pRef->strTitleId, sizeof(pRef->strTitleId), "-titleid=", pOption, NETCONNPS4_SAMPLE_TITLE_ID);
                NetPrintf(("netconnps5: setting titleid=%s\n", pRef->strTitleId));
            }
        }
        NetPrintf(("netconnps5: upnp peerport=%d %s\n",
            pRef->iPeerPort, (pRef->iPeerPort == NETCONN_DEFAULT_UPNP_PORT ? "(default)" : "(selected via netconnconnect param)")));

        // if needed, delay NetConnConnect until external cleanup phase completes
        if (pRef->eState == NetConnRefT::ST_EXT_CLEANUP)
        {
            pRef->bConnectDelayed = TRUE;
            NetPrintf(("netconnps5: delaying completion of NetConnConnect() until external cleanup phase is completed\n"));
        }
        else
        {
            iResult = _NetConnConnect(pRef);
        }
    }
    else
    {
        NetPrintf(("netconnps5: NetConnConnect() ignored because already connected!\n"));
    }

    return(iResult);
}

/*F********************************************************************************/
/*!
    \Function    NetConnDisconnect

    \Description
        Used to bring down the network connection. After calling this, it would
        be necessary to call NetConnConnect to bring the connection back up or
        NetConnShutdown to completely shutdown all network support.

    \Output
        int32_t     - negative=error, zero=success

    \Version 10/04/2005 (jbrookes)
*/
/********************************************************************************F*/
int32_t NetConnDisconnect(void)
{
    NetConnRefT *pRef = _NetConn_pRef;
    int32_t iLocalUserIndex;

    NetPrintf(("netconnps5: disconnecting...\n"));

    // early exit if NetConn is already in cleanup phase
    if (pRef->eState == NetConnRefT::ST_EXT_CLEANUP)
    {
        NetPrintf(("netconnps5: external cleanup already in progress\n"));
        return(0);
    }

    if (pRef->eState == NetConnRefT::ST_INIT)
    {
        NetPrintf(("netconnps5: already disconnected\n"));
        return(0);
    }

    // bring down network interface
    SocketControl(NULL, 'disc', 0, NULL, NULL);

    // reset status
    pRef->eState = NetConnRefT::ST_INIT;
    pRef->uLastConnStatus = pRef->uConnStatus;  // save conn status
    pRef->uConnStatus = 0;

    // destroy any unfinished sceNpAsynrequests and clear control status
    for (iLocalUserIndex = 0; iLocalUserIndex < NETCONN_MAXLOCALUSERS; ++iLocalUserIndex)
    {
        // premium
        if (pRef->aUsers[iLocalUserIndex].Premium.ReqData.iRequestId != 0)
        {
            sceNpDeleteRequest(pRef->aUsers[iLocalUserIndex].Premium.ReqData.iRequestId);
        }
        pRef->aUsers[iLocalUserIndex].Premium.ReqData.eRequestState = NETCONN_NP_REQUEST_STATE_INVALID;
        pRef->aUsers[iLocalUserIndex].Premium.ReqData.iRequestId = 0;

        // reachability
        if (pRef->aUsers[iLocalUserIndex].Reachability.ReqData.iRequestId != 0)
        {
            sceNpDeleteRequest(pRef->aUsers[iLocalUserIndex].Reachability.ReqData.iRequestId);
        }
        pRef->aUsers[iLocalUserIndex].Reachability.ReqData.eRequestState = NETCONN_NP_REQUEST_STATE_INVALID;
        pRef->aUsers[iLocalUserIndex].Reachability.ReqData.iRequestId = 0;
    }

    // clear cached NP information
    //memset(&pRef->NpId, 0, sizeof(pRef->NpId));

   /*
   transit to ST_EXT_CLEANUP state
       Upon next call to NetConnConnect(), the NetConn module will be stuck in
       ST_EXT_CLEANUP state until all pending external module destructions
       are completed.

       note: call to _NetConnNpTerm() will be performed before exiting ext_cleanup state
   */
   pRef->eState = NetConnRefT::ST_EXT_CLEANUP;

    // abort upnp operations
    if (pRef->pProtoUpnp != NULL)
    {
        ProtoUpnpControl(pRef->pProtoUpnp, 'abrt', 0, 0, NULL);
    }

    // done
    return(0);
}

/*F********************************************************************************/
/*!
    \Function   NetConnControl

    \Description
        Set module behavior based on input selector.

    \Input  iControl    - input selector
    \Input  iValue      - selector input
    \Input  iValue2     - selector input
    \Input *pValue      - selector input
    \Input *pValue2     - selector input

    \Output
        int32_t         - selector result

    \Notes
        iControl can be one of the following:

        \verbatim
            cpaf: set cpu affinity for threads
            npsc: set up SceNpTitleSecret, pValue = data, iVlaue = sizeof data
            recu: stands for "register for external cleanup"; add a new entry to the external cleanup list
            tick: initiate a new auth code request for the user index specified in iValue; <0=error, 0=retry (a request is already pending), >0=request issued
            prem: initiate a new PlayStation Premium membership check for user index iValue; iValue2=desired premium features to check; <0=error, 0=retry (a request is already pending), >0=request issued
            scid: set up SceNpClientId, pValue = data, iValue = sizeof data
            snam: set DirtyCert service name
            spam: set debug output verbosity (0...n)
            ousr: one user mode, which determines whether all users must be old enough to go into an +onl state (iValue=0) or any one user must be old enough (iValue=1)
        \endverbatim

        Unhandled selectors are passed through to NetConnCommonControl()

    \Version 1.0 04/27/2006 (jbrookes)
*/
/********************************************************************************F*/
int32_t NetConnControl(int32_t iControl, int32_t iValue, int32_t iValue2, void *pValue, void *pValue2)
{
    NetConnRefT *pRef = _NetConn_pRef;

    // make sure module is started before allowing any other control calls
    if (pRef == NULL)
    {
        NetPrintf(("netconnps5: warning - calling NetConnControl() while module is not initialized\n"));
        return(-1);
    }

    // test for tick
    if (iControl == 'cpaf')
    {
        pRef->CpuAffinity = iValue;
        return 0;
    }
    if (iControl == 'npsc')
    {
        // iValue is supposed to contain the size of the buffer to which pValue points
        if ((iValue <= (int32_t)sizeof(pRef->NpTitleSecret.data)) && (pValue != NULL))
        {
            // fill buffer with NP Title Secret passed in pValue
            ds_memclr(&pRef->NpTitleSecret.data, sizeof(pRef->NpTitleSecret.data));
            ds_memcpy(&pRef->NpTitleSecret.data, pValue, iValue);
            pRef->bSecretSet = TRUE;

            NetPrintMem(&pRef->NpTitleSecret, sizeof(pRef->NpTitleSecret), "NpTitleSecret");
        }
        else
        {
            NetPrintf(("netconnps5: 'npsc' selector ignored because input buffer is invalid.\n"));
            return(-1);
        }

        return(0);
    }
    // register for external cleanup
    if (iControl == 'recu')
    {
        return(NetConnCommonAddToExternalCleanupList(&pRef->Common, (NetConnExternalCleanupCallbackT)pValue, pValue2));
    }

    // request refresh of auth token
    if (iControl == 'tick')
    {
        return(_NetConnRequestAuthCode(pRef, iValue));
    }

    // set the "one user" mode
    if (iControl == 'ousr')
    {
        pRef->uConnUserMode = (uint32_t)iValue;
        return(0);
    }
    
    // request refresh of premium membership
    if (iControl == 'prem')
    {
        pRef->aUsers[iValue].Premium.NpCheckPremiumParam.size = sizeof(pRef->aUsers[iValue].Premium.NpCheckPremiumParam);
        pRef->aUsers[iValue].Premium.NpCheckPremiumParam.userId = pRef->aUsers[iValue].UserServiceUserId;
        pRef->aUsers[iValue].Premium.NpCheckPremiumParam.features = iValue2;
        return(_NetConnNpAsyncRequest(pRef, iValue, (NetConnAsyncRequestDataT*)&pRef->aUsers[iValue].Premium, _NetConnPremiumRequest));
    }
    if (iControl == 'scid')
    {
        // iValue is supposed to contain the size of the buffer to which pValue points
        if ((iValue <= (int32_t)sizeof(pRef->NpClientId.id)) && (pValue != NULL))
        {
            // fill buffer with NP Title Secret passed in pValue
            ds_strnzcpy(pRef->NpClientId.id, (char *)pValue, sizeof(pRef->NpClientId.id));
            NetPrintf(("netconnps5: NpClientId set to %s\n", pRef->NpClientId.id));
        }
        else
        {
            NetPrintf(("netconnps5: 'scid' selector ignored because input buffer is invalid.\n"));
            return(-1);
        }

        return(0);
    }

    // set dirtycert service name
    if (iControl == 'snam')
    {
        return(DirtyCertControl('snam', 0, 0, pValue));
    }

    #if DIRTYCODE_LOGGING
    if (iControl == 'spam')
    {
        NetPrintf(("netconnps5: changing debug level from %d to %d\n", pRef->Common.iDebugLevel, iValue));
        pRef->Common.iDebugLevel = iValue;
        // allow to fall through to socketcontrol
    }
    #endif

    // pass through unhandled selectors to NetConnCommon
    return(NetConnCommonControl(&pRef->Common, iControl, iValue, iValue2, pValue, pValue2));
}

/*F********************************************************************************/
/*!
    \Function    NetConnStatus

    \Description
        Check general network connection status. Different selectors return
        different status attributes.

    \Input iKind    - status selector ('open', 'conn', 'onln')
    \Input iData    - (optional) selector specific
    \Input *pBuf    - (optional) pointer to output buffer
    \Input iBufSize - (optional) size of output buffer

    \Output
        int32_t     - selector specific

    \Notes
        iKind can be one of the following:

        \verbatim
            acct: returns the psn account id (pBuf) given the user index (iData)
            addr: ip address of client
            affn: get the thread cpu affinity setting
            bbnd: TRUE if broadband, else FALSE
            chat: TRUE if local user cannot chat, else FALSE (provided for backward compatibility basically this is the same as calling 'crst')
            conn: connection status: +onl=online, ~<code>=in progress, -<err>=NETCONN_ERROR_*
            crst: fills pBuf with a bool containing the communication restriction status, if available. return value: 0:"try again"; <0:error; >0:number of bytes copied into pBuf
            ctnt: returns users online activity restriction (provided for backward compatibility basically this is the same as calling 'crst')
            envi: EA back-end environment type in use (-1=not available, NETCONN_PLATENV_TEST, NETCONN_PLATENV_CERT, NETCONN_PLATENV_PROD)
            eusr: returns the IEAUser pointer in pBuf given the user index
            gust: unsupported
            gtag: zero=username of user at index [0-3] in iData returned in pBuf, negative=erro
            iind: returns the index of the user that launched the game
            indx: returns the index of the user with the specified Sony User ID
            iuid: return the SceUserServiceUserId for the user that launched the application
            iusr: return the index to the IEAUser::GetUserId() passed via iData. Should be called from the thread that calls NetConnIdle(). negative=error (deprecated)
            lcon: (stands for Last CONNection status) connection status saved upon NetConnDisconnect()
            locl: return locality user account
            locn: return locality user account
            mask: returns the mask of users that are ONLINE, similar to the Xbox equivalent
            mgrp: returns memory group and user data pointer
            npid: Return the n'th users's SceNpOnlineId via pBuf, negative return=error
            npst: Return the n'th users's SceNpState also returns the SceNpReachabilityState of the user in pBuf, negative return=error 
            onln: true/false indication of whether network is operational
            open: true/false indication of whether network code running
            pftr: returns value indicating if the running title has a play-first trial license. return value: -1:"failed to fetch pft info"; 0:not pft; 1:pft
            prem: fills pBuf with a bool containing the result of the PlayStation Premium membership check for user index iData, if available. return value: 0:"try again"; <0:error; >0:number of bytes copied into pBuf
            pscd: deletes a given user's push context id
            pcsx: return or create a given user's push context id
            soid: return the SceNpOnlineId via pBuf, for the user at index in iData [0-3], negative means no user
            suid: return the SceUserServiceUserId for the user at index in iData [0-3], negative means no user
            tick: fills pBuf with authcode for user index iData, if available. return value: 0:"try again"; <0:error; >0:number of bytes copied into pBuf. If pBuf is null it will return the ticket size
            titl: fills pBuf with the title id, pBuf must be at least 13 characters large
            tria: returns value indicating if the running title has a trial license. return value: -1:"failed to fetch trial info"; 0:not trial; 1:trial         
            type: connection type: NETCONN_IFTYPE_* bitfield
            usri: return the user index for the given IEAUser
            upnp: return protoupnp external port info, if available
            ugcn: return user generated content restriction (provided for backward compatibility basically this is the same as calling 'crst')
            vers: return DirtySDK version
            lerr: return the last error that occurred as a result of an sceNp library call
        \endverbatim

        Unhandled selectors are passed through to NetConnCommonStatus()

    \Version 10/04/2005 (jbrookes)
*/
/********************************************************************************F*/
int32_t NetConnStatus(int32_t iKind, int32_t iData, void *pBuf, int32_t iBufSize)
{
    NetConnRefT *pRef = _NetConn_pRef;

    // initialize output buffer
    if (pBuf != NULL)
    {
        ds_memclr(pBuf, iBufSize);
    }

    // see if network code is initialized
    if (iKind == 'open')
    {
        return(pRef != NULL);
    }

    // return DirtySDK version
    if (iKind == 'vers')
    {
        return(DIRTYSDK_VERSION);
    }

    // make sure module is started before allowing any other status calls
    if (pRef == NULL)
    {
        NetPrintf(("netconnps5: warning - calling NetConnStatus() while module is not initialized\n"));
        return(-1);
    }

    // returns account id
    if (iKind == 'acct')
    {
        if ((iData >= 0) && (iData < NETCONN_MAXLOCALUSERS) && (pBuf != NULL) && (iBufSize == sizeof(SceNpAccountId)))
        {
            SceUserServiceUserId sceUserServiceUserId = pRef->aUsers[iData].UserServiceUserId;

            // ignore if user is not SIGNEDIN with PSN
            if (pRef->aUsers[iData].NpState == SCE_NP_STATE_SIGNED_IN)
            {
                // check if the cache is dirty
                NetCritEnter(&pRef->Common.crit);
                if (pRef->aUsers[iData].AccountIdCache.bInvalid == TRUE)
                {
                    //get a fresh value for accountid
                    pRef->aUsers[iData].AccountIdCache.iResult = _NetConnSaveErr(pRef, sceNpGetAccountIdA(sceUserServiceUserId, &pRef->aUsers[iData].AccountIdCache.NpAccountId));
                    pRef->aUsers[iData].AccountIdCache.bInvalid = FALSE;
                }

                if (pRef->aUsers[iData].AccountIdCache.iResult == SCE_OK)
                {
                    ds_memcpy(pBuf, &pRef->aUsers[iData].AccountIdCache.NpAccountId, iBufSize);
                }
                else
                {
                    NetPrintf(("netconnps5: sceNpGetAccountIdA(0x%08x) (idx=%d) failed with %s\n", sceUserServiceUserId, iData, DirtyErrGetName(pRef->aUsers[iData].AccountIdCache.iResult)));
                }
                NetCritLeave(&pRef->Common.crit);

                return(pRef->aUsers[iData].AccountIdCache.iResult);
            }
            NetPrintf(("netconnps5: NetConnStatus('acct') sceUserServiceUserId(0x%08x) (idx=%d) no op, user status(%d) not signed in\n", sceUserServiceUserId, iData, pRef->aUsers[iData].NpState));
        }
        else
        {
            NetPrintf(("netconnps5: NetConnStatus('acct'), invalid arguments!\n"));
        }

        return(-1);
    }

    // return the thread cpu affinity
    if (iKind == 'affn')
    {
        return(pRef->iThreadCpuAffinity);
    }

    // return broadband (TRUE/FALSE)
    if (iKind == 'bbnd')
    {
        return(TRUE);
    }

    // return ability of local user to chat
    if (iKind == 'chat')
    {
        return(NetConnStatus('ctnt', iData, pBuf, iBufSize));
    }

    // connection status
    if (iKind == 'conn')
    {
        return(pRef->uConnStatus);
    }

    // returns communication restriction status
    if (iKind == 'crst')
    {
        int32_t iRet;

        if ((iData < 0) || (iData >= NETCONN_MAXLOCALUSERS) || (pBuf == NULL) || (iBufSize < (int32_t)sizeof(uint8_t)))
        {
            NetPrintf(("netconnps5: NetConnStatus('crst'), invalid arguements.\n"));
            return(-1);
        }

        NetCritEnter(&pRef->pLocalUser->crit);
        if (pRef->aUsers[iData].CommRestriction.ReqData.eRequestState == NETCONN_NP_REQUEST_STATE_VALID)
        {
            // communication restriction request valid
            ds_memcpy(pBuf, &pRef->aUsers[iData].CommRestriction.bRestricted, sizeof(pRef->aUsers[iData].CommRestriction.bRestricted));
            iRet = 1;
        }
        else if (pRef->aUsers[iData].CommRestriction.ReqData.eRequestState == NETCONN_NP_REQUEST_STATE_INPROG)
        {
            // check inprogress try again
            iRet = 0;
        }
        else if (pRef->aUsers[iData].CommRestriction.ReqData.eRequestState == NETCONN_NP_REQUEST_STATE_INVALID)
        {
            if (pRef->aUsers[iData].NpState == SCE_NP_STATE_SIGNED_IN)
            {
                if ((iRet = _NetConnCommRestrictionRequest(pRef, iData, (NetConnAsyncRequestDataT*)&pRef->aUsers[iData].CommRestriction)) < 0)
                {
                    NetPrintf(("netconnps5: NetConnStatus('crst') uable to query for communication restriction stauts for user (0x%80x, %d) failed; err=%s\n", pRef->aUsers[iData].UserServiceUserId, iData, DirtyErrGetName(iRet)));
                }
                else
                {
                    iRet = 0;
                }
            }
            else
            {
                NetPrintf(("netconnps5: NetConnStatus('crst') uable to query for communication restriction stauts for user %d because user is not signed in\n"));
                iRet = -2;
            }
        }
        else
        {
            iRet = -3;
            NetPrintf(("netconnps5: NetConnStatus('crst') unknown failure\n"));
        }

        NetCritLeave(&pRef->pLocalUser->crit);
        return(iRet);
    }

    // return ability of local user to perform online activities
    if (iKind == 'ctnt')
    {
        uint8_t bRestricted;
        int32_t iRet;

        iRet = NetConnStatus('crst', iData, &bRestricted, sizeof(bRestricted));
        if (iRet == 1)
        {
            return(bRestricted);
        }
        else if (iRet == 0)
        {
            // retrieval of communication restriction value is in progress
            return(-1);
        }
        else
        {
            // an error occured retrieving communication restriction value values
            return(-2);
        }
    }

    // EA back-end environment type in use
    if (iKind == 'envi')
    {
        if (pRef->uConnStatus == '+onl')
        {
            return(pRef->uPlatEnv);
        }

        return(-1);
    }

    // returns the corresponding IEAUser pointer given the user index
    if (iKind == 'eusr')
    {
        if ((iData >= 0) && (iData < NETCONN_MAXLOCALUSERS))
        {
           if ((pBuf != NULL) && (iBufSize >= (int32_t)sizeof(void *)))
           {
               void **pIEAUser = (void **)pBuf;
               *pIEAUser = (void *)pRef->aEAUsers[iData];
               return(0);
           }
        }
        return(-1);
    }
    
    // unsupported
    if (iKind == 'gust')
    {
        /*on ps4, a guest user and a regular user are exact same except for one thing: guests have their local data deleted when they logout
        both the guest user and the regular user can be associated or not with a PSN account (signed-up or not signed-up)
        both the guest user and the regular user can be signed-in with PSN or signed-out from PSN
        there exist no api to determine if the user was created with “Create a User” or “Play as a Guest” in the system gui.
        */
        return(-1);
    }

    // zero=username of user at index [0-3] in iData returned in pBuf, negative=erro
    if (iKind == 'gtag')
    {
        if (iData < 0 || iData >= NETCONN_MAXLOCALUSERS)
        {
            // index out of range
            return(-1);
        }

        if (pRef->aUsers[iData].UserServiceUserId == SCE_USER_SERVICE_USER_ID_INVALID)
        {
            // No user at the specified index
            return(-1);
        }

        //special handling since SceNpOnlineId's .data field is not always terminated within its char array, but instead in the next .term character field
        if (iBufSize < (SCE_NP_ONLINEID_MAX_LENGTH + 1))
        {
            // Buffer too small
            return(-2);
        }
        if (pRef->aUsers[iData].NpState == SCE_NP_STATE_SIGNED_IN)
        {
            ds_strnzcpy((char*)pBuf, pRef->aUsers[iData].NpOnlineId.data, iBufSize);
            return(0);
        }

        // User Offline
        return(-3);
    }

    // return the index for the user that launched the application
    if (iKind == 'iind')
    {
        /*
        note: will always return -1 when game's param.sfo is modified for "allow the initial user to logout during your game"
              because sceUserServiceGetInitialUser() is known to be unusable/failing in that context.
        */

        SceUserServiceUserId userId = 0;
        if (sceUserServiceGetInitialUser(&userId) == SCE_OK)
        {
            int32_t iLocalUserIndex;
            for (iLocalUserIndex = 0; iLocalUserIndex < NETCONN_MAXLOCALUSERS; ++iLocalUserIndex)
            {
                if (pRef->aUsers[iLocalUserIndex].UserServiceUserId == userId)
                {
                    return(iLocalUserIndex);
                }
            }
        }
        return(-1);
    }

    // returns the index of the user with the specified Sony User ID
    if (iKind == 'indx')
    {
        int32_t iLocalUserIndex;
        for (iLocalUserIndex = 0; iLocalUserIndex < NETCONN_MAXLOCALUSERS; ++iLocalUserIndex)
        {
            if (pRef->aUsers[iLocalUserIndex].UserServiceUserId == iData)
            {
                return(iLocalUserIndex);
            }
        }

        return(-1);
    }

    // return the SceUserServiceUserId for the user that launched the application
    if (iKind == 'iuid')
    {
        /*
        note: will always return -1 when game's param.sfo is modified for "allow the initial user to logout during your game"
              because sceUserServiceGetInitialUser() is known to be unusable/failing in that context.
        */

        SceUserServiceUserId userId = 0;
        if (sceUserServiceGetInitialUser(&userId) == SCE_OK)
        {
            return userId;
        }
        return(-1);
    }

    // 'iusr' is being deprecated please use 'usri'
    if (iKind == 'iusr')
    {
        return(_NetConnQueryIEAUser(pRef, iData));
    }

    // (stands for Last CONNection status) connection status saved upon NetConnDisconnect()
    if (iKind == 'lcon')
    {
        return(pRef->uLastConnStatus);
    }

    if (iKind == 'lerr')
    {
        return(pRef->iLastNpError);
    }

    if (iKind == 'locn')
    {
        return(NetConnStatus('locl', iData, pBuf, iBufSize));
    }

    if (iKind == 'locl')
    {
        if (pRef->iLocale == 0)
        {
            // note: although system language can be queried, country is attached specifically to the user account
            //       since we want to have 'locl' work the same way as other platforms we will use the country setting 
            //       of the user that launched the application.
            SceUserServiceUserId sceUserServiceUserId = NetConnStatus('iuid', 0, NULL, 0);
            int32_t iResult = 0;
            int32_t iSystemLanguageValue = 0;
            int32_t iLobbyLanguage = 0;
            SceNpCountryCode CountryCode;

            if (sceUserServiceUserId < 0)
            {
                NetPrintf(("netconnps5: NetConnStatus('locl') failed to retrieve the initial user.\n"));
                return(-1);
            }

            // get the system language code
            iResult = sceSystemServiceParamGetInt(SCE_SYSTEM_SERVICE_PARAM_ID_LANG, &iSystemLanguageValue);
            if (iResult < 0)
            {
                NetPrintf(("netconnps5: sceSystemServiceParamGetInt(SCE_SYSTEM_SERVICE_PARAM_ID_LANG) failed with code %0x%08x\n", iResult));
                return(-1);
            }

            // get the initial user's country code
            iResult = sceNpGetAccountCountryA(sceUserServiceUserId, &CountryCode);
            if (iResult < 0)
            {
                NetPrintf(("netconnps5: sceNpGetAccountCountryA() failed for initial user %d with code %0x%08x\n", sceUserServiceUserId, iResult));
                return(-1);
            }

            // convert the PSN system language to its lobby equivalent
            iLobbyLanguage = _NetConnPsnLangToLobbyLang(iSystemLanguageValue);

            // append language and country codes into locale
            pRef->iLocale = (iLobbyLanguage << 16) + 
                            (toupper(CountryCode.data[0]) << 8) +
                            (toupper(CountryCode.data[1]));
        }

        return(pRef->iLocale);
    }

    // returns the mask of users that are ONLINE, similar to the Xbox equivalent
    if (iKind == 'mask')
    {
        int32_t iLocalUserIndex;
        int32_t iUserMask = 0x0000;

        for (iLocalUserIndex = 0; iLocalUserIndex < NETCONN_MAXLOCALUSERS; ++iLocalUserIndex)
        {
            if ((iLocalUserIndex >= 0) && (pRef->aUsers[iLocalUserIndex].NpState == SCE_NP_STATE_SIGNED_IN))
            {
                iUserMask |= (1 << iLocalUserIndex);
            }
        }
        return(iUserMask);
    }

    // mgrp: return memory group and user data pointer
    if (iKind == 'mgrp')
    {
        if ((pBuf != NULL) && (iBufSize == sizeof(void *)))
        {
            *((void **)pBuf) = pRef->Common.pMemGroupUserData;
        }
        return(pRef->Common.iMemGroup);
    }


    //npid: Return the n'th users's SceNpOnlineId
    if ((iKind == 'npid') && (iBufSize >= (int32_t)sizeof(SceNpOnlineId)) && (iData < NETCONN_MAXLOCALUSERS))
    {
        if (iData >= 0)
        {
            ds_memcpy(pBuf, &pRef->aUsers[iData].NpOnlineId, sizeof(SceNpOnlineId));
            return(0);
        }

        return(-1);
    }

    //npst: Return the n'th users's SceNpState
    if ((iKind == 'npst') && (iData < NETCONN_MAXLOCALUSERS))
    {
        if ((iData >= 0) && (iData < NETCONN_MAXLOCALUSERS))
        {
            if ((pBuf != NULL) && (iBufSize >= (int32_t)sizeof(SceNpReachabilityState)))
            {
                ds_memcpy(pBuf, &pRef->aUsers[iData].NpReachState, sizeof(SceNpReachabilityState));
            }

            return(pRef->aUsers[iData].NpState);
        }

        return(-1);
    }

    // see if connected to ISP/LAN
    if (iKind == 'onln')
    {
        return(pRef->uConnStatus == '+onl');
    }

    if (iKind == 'pftr')
    {
        // make sure app content is available and initialized
        if (_NetConnEntitlementAccessInitialize() < 0)
        {
            return(-1);
        }
        // return play-first trial status
        return(_NetConnQueryPlayFirstTrial());
    }

    if (iKind == 'prem')
    {
        if ((iData < 0) || (iData >= NETCONN_MAXLOCALUSERS) || (pBuf == NULL) || (iBufSize != sizeof(pRef->aUsers[iData].Premium.NpCheckPremiumResult.authorized)))
        {
            NetPrintf(("netconnps5: NetConnStatus('prem'), invalid arguments.\n"));
            return(-1);
        }
        else
        {
            SceNpCheckPremiumResult PremResult;
            int32_t iRet = _NetConnReadAsyncResponse(pRef, iData, &PremResult, sizeof(PremResult), (NetConnAsyncRequestDataT*)&pRef->aUsers[iData].Premium, NULL, _NetConnPremiumResponse);
            if (iRet > 0)
            {
                NetPrintf(("netconnps5: NetConnStatus('prem'), premium membership request for user (%d) returned (%s).\n", iData, pRef->aUsers[iData].Premium.NpCheckPremiumResult.authorized ? "true" : "false"));
                ds_memcpy(pBuf, &pRef->aUsers[iData].Premium.NpCheckPremiumResult.authorized, sizeof(pRef->aUsers[iData].Premium.NpCheckPremiumResult.authorized));
            }
            else if (iRet == -2)
            {
                NetPrintf(("netconnps5: NetConnStatus('prem'), premium membership request for user (%d) must be restarted manually. Use NetConnControl('pspl').\n", iData));
            }
            return(iRet);
        }
    }

    // delete push context id
    if (iKind == 'pscd')
    {
        if ((iData < 0) || (iData >= NETCONN_MAXLOCALUSERS))
        {
            NetPrintf(("netconnps5: NetConnStatus('pscd') NetConnStatus('pscd')"));
            return(-1);
        }

        if (pRef->aUsers[iData].isPushContextIdValid == TRUE)
        {
            if (DirtyWebApiDeletePushContext(pRef->pWebApi, iData, &pRef->aUsers[iData].PushEventContextId) < 0)
            {
                NetPrintf(("netconnps5: NetConnStatus('pscd') DirtyWebApiDeletePushContext failed for user index %d\n", iData));
                return(-2);
            }

            pRef->aUsers[iData].isPushContextIdValid = FALSE;
        }

        return(0);
    }

    // return push context id if there is one else create one
    if (iKind == 'pscx')
    {
        if ((iData < 0) || (iData >= NETCONN_MAXLOCALUSERS) || (pBuf == NULL) || (iBufSize < (int32_t)sizeof(SceNpWebApi2PushEventPushContextId)))
        {
            NetPrintf(("netconnps5: NetConnStatus('pscx'), invalid arguments.\n"));
            return(-1);
        }

        if (pRef->pWebApi == NULL)
        {
            NetPrintf(("netconnps5: NetConnStatus('pscx') webapi ref is null!\n"));
            return(-2);
        }

        // create one if push context id is not valid
        if (pRef->aUsers[iData].isPushContextIdValid == FALSE)
        {
            if (DirtyWebApiCreatePushContext(pRef->pWebApi, iData, &pRef->aUsers[iData].PushEventContextId) < 0)
            {
                NetPrintf(("netconnps5: NetConnStatus('pscx') DirtyWebApiCreatePushContext failed for index %d\n", iData));
                return(-3);
            }

            pRef->aUsers[iData].isPushContextIdValid = TRUE;
        }

        ds_memcpy(pBuf, &pRef->aUsers[iData].PushEventContextId, sizeof(SceNpWebApi2PushEventPushContextId));
        return(0);
    }

    // return the SceNpOnlineId for the user at index in iData [0-3], negative means error
    if (iKind == 'soid')
    {
        if ((iData >= 0) && (iData < NETCONN_MAXLOCALUSERS) && (pBuf != NULL) && (iBufSize == sizeof(SceNpOnlineId)))
        {
            SceUserServiceUserId sceUserServiceUserId = pRef->aUsers[iData].UserServiceUserId;

            // ignore if user is not SIGNEDIN with PSN
            if (pRef->aUsers[iData].NpState == SCE_NP_STATE_SIGNED_IN)
            {
                // check if the cache is dirty
                NetCritEnter(&pRef->Common.crit);
                if( pRef->aUsers[iData].OnlineIdCache.bInvalid == TRUE )
                {
                    //get a fresh value for onlineid
                    pRef->aUsers[iData].OnlineIdCache.iResult = _NetConnSaveErr(pRef, sceNpGetOnlineId(sceUserServiceUserId, &pRef->aUsers[iData].OnlineIdCache.NpOnlineId));
                    pRef->aUsers[iData].OnlineIdCache.bInvalid = FALSE;
                }

                if(pRef->aUsers[iData].OnlineIdCache.iResult == SCE_OK)
                {
                    ds_memcpy(pBuf, &pRef->aUsers[iData].OnlineIdCache.NpOnlineId, iBufSize);
                }
                else
                {
                    NetPrintf(("netconnps5: sceNpGetOnlineId(0x%08x) (idx=%d) failed with %s\n", sceUserServiceUserId, iData, DirtyErrGetName(pRef->aUsers[iData].OnlineIdCache.iResult)));
                }
                NetCritLeave(&pRef->Common.crit);

                return(pRef->aUsers[iData].OnlineIdCache.iResult);
            }
            NetPrintf(("netconnps5: NetConnStatus('soid') sceUserServiceUserId(0x%08x) (idx=%d) no op, user status(%d) not signed in\n", sceUserServiceUserId, iData, pRef->aUsers[iData].NpState));
        }
        else
        {
            NetPrintf(("netconnps5: NetConnStatus('soid'), invalid arguments!\n"));
        }

        return(-1);
    }

    // return the SceUserServiceUserId for the user at index in iData [0-3], negative means no user
    if (iKind == 'suid')
    {
        if ((iData >= 0) && (iData < NETCONN_MAXLOCALUSERS))
        {
            return(pRef->aUsers[iData].UserServiceUserId);
        }
        else
        {
            return(-1);
        }
    }

    // 'tick' - fills pBuf with authcode for user index iData, if available. return value: 0:"try again"; <0:error; >0:number of bytes copied into pBuf
    if (iKind == 'tick')
    {
        return(_NetConnReadAuthCode(pRef, iData, pBuf, iBufSize));
    }

    if (iKind == 'titl')
    {
        if (iBufSize < SCE_NP_TITLE_ID_LEN + 1)
        {
            NetPrintf(("netconnps5: buffer too small to contain title id\n"));
            return(-1);
        }
        ds_strnzcpy((char*)pBuf, pRef->strTitleId, iBufSize);
        return(0);
    }

    if (iKind == 'tria')
    {
        int32_t iTrial;
        // make sure app content is available and initialized
        if (_NetConnEntitlementAccessInitialize() < 0)
        {
            return(-1);
        }
        // get trial status
        if ((iTrial = _NetConnQueryTrial()) != TRUE)
        {
            // if trial status is not true, check if play-first trial status is true; if so, we consider the trial status to be true
            if (_NetConnQueryPlayFirstTrial() == TRUE)
            {
                iTrial = TRUE;
            }
        }
        return(iTrial);
    }

    // return interface type (more verbose)
    if (iKind == 'type')
    {
        return(_NetConnGetInterfaceType(pRef));
    }

    // return upnp addportmap info, if available
    if (iKind == 'upnp')
    {
        // if protoupnp is available, and we've added a port map, return the external port for the port mapping
        if ((pRef->pProtoUpnp != NULL) && (ProtoUpnpStatus(pRef->pProtoUpnp, 'stat', NULL, 0) & PROTOUPNP_STATUS_ADDPORTMAP))
        {
            return(ProtoUpnpStatus(pRef->pProtoUpnp, 'extp', NULL, 0));
        }
    }

    // return ability of local user to share user generated content
    if (iKind == 'ugcn')
    {
        return(NetConnStatus('ctnt', iData, pBuf, iBufSize));
    }


    // return the user index of the IEAUser
    if (iKind == 'usri')
    {
        if (pBuf == NULL)
        {
            NetPrintf(("netconnps5: NetConnStatus('usri') invalid buffer\n"));
            return (-1);
        }

        EA::User::IEAUser *pIEAUser = (EA::User::IEAUser *)pBuf;
        return (_NetConnQueryIEAUser(pRef, (SceUserServiceUserId)pIEAUser->GetUserID()));
    }

    if (iKind == 'wapi')
    {
        if (pBuf == NULL)
        {
            NetPrintf(("netconnps5: NetConnStatus('wapi') invalid buffer\n"));
            return(-1);
        }

        *(DirtyWebApiRefT **)pBuf = pRef->pWebApi;
        
        return(0);
    }

    // pass unrecognized options to NetConnCommon
    return(NetConnCommonStatus(&pRef->Common, iKind, iData, pBuf, iBufSize));
}

/*F********************************************************************************/
/*!
    \Function    NetConnShutdown

    \Description
        Shutdown the network code and return to idle state.

    \Input  uShutdownFlags  - shutdown configuration flags

    \Output
        int32_t             - negative=error, else zero

    \Version 10/04/2005 (jbrookes)
*/
/********************************************************************************F*/
int32_t NetConnShutdown(uint32_t uShutdownFlags)
{
    NetConnRefT *pRef = _NetConn_pRef;

    // make sure we've been started
    if (pRef == NULL)
    {
        return(NETCONN_ERROR_NOTACTIVE);
    }

    return(_NetConnShutdownInternal(pRef, uShutdownFlags));
}

/*F********************************************************************************/
/*!
    \Function    NetConnSleep

    \Description
        Sleep the application for some number of milliseconds.

    \Input iMilliSecs    - number of milliseconds to block for

    \Version 10/04/2005 (jbrookes)
*/
/********************************************************************************F*/
void NetConnSleep(int32_t iMilliSecs)
{
    usleep(iMilliSecs*1000);
}

/*F*************************************************************************************************/
/*!
    \Function    NetConnRegisterNpStateCallback

    \Description
        Use this function to tell netconn about a local user that no longer exists

    \Input *pCallback   - pointer to the function to call
    \Input *pUserData   - user data to pass to the callback
    
    \Version 05/11/2015 (amakoukji)
*/
/*************************************************************************************************F*/
void NetConnRegisterNpStateCallback(NetConnNpStateCallbackT *pCallback, void *pUserData)
{
    NetConnRefT *pRef = _NetConn_pRef;
    if (pRef == NULL)
    {
        return;
    }
 
    pRef->pNpStateCallback = pCallback;
    pRef->pNpStateCallbackUserData = pUserData;
}

/*F*************************************************************************************************/
/*!
    \Function    NetConnAddLocalUser

    \Description
        Use this function to tell netconn about a newly detected local user on the local console.

    \Input iLocalUserIndex  - index at which DirtySDK needs to insert this user it its internal user array
    \Input *pLocalUser      - pointer to associated IEAUser

    \Output
        int32_t             - 0 for success; negative for error

    \Version 01/16/2014 (mclouatre)
*/
/*************************************************************************************************F*/
int32_t NetConnAddLocalUser(int32_t iLocalUserIndex, const EA::User::IEAUser *pLocalUser)
{
    return(NetConnLocalUserAdd(iLocalUserIndex, pLocalUser));
}

/*F*************************************************************************************************/
/*!
    \Function    NetConnRemoveLocalUser

    \Description
        Use this function to tell netconn about a local user that no longer exists

    \Input iLocalUserIndex  - index in the internal DirtySDK user array at which the user needs to be cleared
    \Input *pLocalUser      - pointer to associated IEAUser

    \Output
        int32_t             - 0 for success; negative for error

    \Version 01/16/2014 (mclouatre)
*/
/*************************************************************************************************F*/
int32_t NetConnRemoveLocalUser(int32_t iLocalUserIndex, const EA::User::IEAUser *pLocalUser)
{
    return(NetConnLocalUserRemove(iLocalUserIndex, pLocalUser));
}
